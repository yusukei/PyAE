// PythonHost.cpp
// PyAE - Python for After Effects
// Python埋め込みインタープリター管理の実装

#include "PythonHost.h"
#include "Logger.h"
#include "PluginState.h"

#include <sstream>
#include <fstream>

namespace PyAE {

bool PythonHost::Initialize(const PythonConfig& config) {
    DebugOutput("PythonHost::Initialize() called");

    if (m_initialized.load()) {
        PYAE_LOG_WARNING("PythonHost", "Already initialized");
        return true;
    }

    m_config = config;

    PYAE_LOG_INFO("PythonHost", "Initializing Python interpreter...");
    PYAE_LOG_INFO("PythonHost", "Python home: " + config.pythonHome.string());
    DebugOutput("Python home: " + config.pythonHome.string());

    try {
        // Pythonホームを設定
        // CRITICAL: Py_SetPythonHome does NOT copy the string - it stores the pointer.
        // Store the wstring in m_pythonHomeStr to keep it alive for the interpreter's lifetime.
        DebugOutput("Checking Python home exists...");
        if (!config.pythonHome.empty() && std::filesystem::exists(config.pythonHome)) {
            m_pythonHomeStr = config.pythonHome.wstring();
            DebugOutput("Setting Py_SetPythonHome...");
            Py_SetPythonHome(m_pythonHomeStr.c_str());
            DebugOutput("Py_SetPythonHome done");
        } else {
            DebugOutput("WARNING: Python home does not exist or is empty!");
        }

        // インタープリター初期化
        DebugOutput("Creating py::scoped_interpreter...");
        m_interpreter = std::make_unique<py::scoped_interpreter>();
        DebugOutput("py::scoped_interpreter created OK");

        // パス設定
        DebugOutput("Setting up Python path...");
        if (!SetupPythonPath()) {
            PYAE_LOG_ERROR("PythonHost", "Failed to setup Python path");
            DebugOutput("ERROR: SetupPythonPath failed!");
            return false;
        }
        DebugOutput("SetupPythonPath OK");

        // AEモジュール初期化
        DebugOutput("Initializing AE module...");
        if (!InitializeAEModule()) {
            PYAE_LOG_ERROR("PythonHost", "Failed to initialize AE module");
            DebugOutput("ERROR: InitializeAEModule failed!");
            return false;
        }
        DebugOutput("InitializeAEModule OK");

        // GILを解放（他のスレッドからPythonを呼び出せるように）
        DebugOutput("Releasing GIL...");
        m_gilRelease = std::make_unique<py::gil_scoped_release>();
        DebugOutput("GIL released");

        m_initialized.store(true);
        PYAE_LOG_INFO("PythonHost", "Python interpreter initialized successfully");
        PYAE_LOG_INFO("PythonHost", "Python version: " + std::string(Py_GetVersion()));
        DebugOutput("Python version: " + std::string(Py_GetVersion()));

        return true;

    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("PythonHost", std::string("Python error during init: ") + e.what());
        DebugOutput("PYTHON ERROR during init: " + std::string(e.what()));
        HandlePythonException();
        return false;
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PythonHost", std::string("Exception during init: ") + e.what());
        DebugOutput("EXCEPTION during init: " + std::string(e.what()));
        return false;
    }
}

void PythonHost::Shutdown() {
    if (!m_initialized.load()) {
        return;
    }

    PYAE_LOG_INFO("PythonHost", "Shutting down Python interpreter...");

    m_shuttingDown.store(true);

    try {
        // GILを再取得
        m_gilRelease.reset();

        // インタープリター終了
        m_interpreter.reset();

    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PythonHost", std::string("Exception during shutdown: ") + e.what());
    }

    m_initialized.store(false);
    PYAE_LOG_INFO("PythonHost", "Python interpreter shutdown complete");
}

bool PythonHost::SetupPythonPath() {
    try {
        py::module_ sys = py::module_::import("sys");
        py::list path = sys.attr("path").cast<py::list>();

        // スクリプトディレクトリを追加
        if (!m_config.scriptsDir.empty()) {
            std::string scriptsDir = m_config.scriptsDir.string();
            path.insert(0, scriptsDir);
            PYAE_LOG_INFO("PythonHost", "Added scripts dir to path: " + scriptsDir);
        }

        // site-packages有効化
        if (m_config.enableSitePackages) {
            std::filesystem::path sitePackages = m_config.pythonHome / "Lib" / "site-packages";
            if (std::filesystem::exists(sitePackages)) {
                path.append(sitePackages.string());
                PYAE_LOG_INFO("PythonHost", "Added site-packages: " + sitePackages.string());
            }
        }

        // stdout/stderrをリダイレクト（print()出力をログに表示）
        RedirectPythonOutput();

        return true;

    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("PythonHost", std::string("Failed to setup path: ") + e.what());
        return false;
    }
}

void PythonHost::SetOutputCallback(PythonOutputCallback callback) {
    WinLockGuard lock(m_outputCallbackMutex);
    m_outputCallback = std::move(callback);
}

void PythonHost::ClearOutputCallback() {
    WinLockGuard lock(m_outputCallbackMutex);
    m_outputCallback = nullptr;
}

void PythonHost::NotifyOutput(const std::string& text, bool isError) {
    WinLockGuard lock(m_outputCallbackMutex);
    if (m_outputCallback) {
        m_outputCallback(text, isError);
    }
}

void PythonHost::RedirectPythonOutput() {
    try {
        // Pythonコードでstdout/stderrをリダイレクト
        const char* redirectCode = R"(
import sys

class PyAEOutput:
    def __init__(self, is_stderr=False):
        self.is_stderr = is_stderr
        self.buffer = ""

    def write(self, text):
        if text:
            self.buffer += text
            # Output each complete line (delimited by \n)
            while '\n' in self.buffer:
                line, self.buffer = self.buffer.split('\n', 1)
                # Output the line (including empty lines for print() with no args)
                if self.is_stderr:
                    _pyae_log_error(line)
                else:
                    _pyae_log_info(line)

    def flush(self):
        if self.buffer:
            if self.is_stderr:
                _pyae_log_error(self.buffer)
            else:
                _pyae_log_info(self.buffer)
            self.buffer = ""

# Set custom output objects
sys.stdout = PyAEOutput(is_stderr=False)
sys.stderr = PyAEOutput(is_stderr=True)
)";

        // ログ出力用のC++関数をPythonに登録
        py::module_ builtins = py::module_::import("builtins");

        builtins.attr("_pyae_log_info") = py::cpp_function([](const std::string& msg) {
            PYAE_LOG_INFO("Python", msg);
            DebugOutput("[Python] " + msg);
            // パネルにも出力
            PythonHost::Instance().NotifyOutput(msg, false);
        });

        builtins.attr("_pyae_log_error") = py::cpp_function([](const std::string& msg) {
            PYAE_LOG_ERROR("Python", msg);
            DebugOutput("[Python ERROR] " + msg);
            // パネルにも出力（エラー）
            PythonHost::Instance().NotifyOutput(msg, true);
        });

        // リダイレクトコードを実行
        py::exec(redirectCode);

        PYAE_LOG_INFO("PythonHost", "Python stdout/stderr redirected to PyAE log");

    } catch (const py::error_already_set& e) {
        PYAE_LOG_WARNING("PythonHost", std::string("Failed to redirect Python output: ") + e.what());
        // 失敗しても続行（致命的ではない）
    }
}

bool PythonHost::InitializeAEModule() {
    // PYBIND11_EMBEDDED_MODULE は最初の import 時に自動登録されるため、
    // ここで明示的に import する必要はない
    // （複数回 import すると型の再登録エラーが発生する）
    PYAE_LOG_INFO("PythonHost", "AE module will be initialized on first import");
    return true;
}

bool PythonHost::ExecuteFile(const std::filesystem::path& scriptPath, std::string& errorOut) {
    if (!m_initialized.load()) {
        errorOut = "Python interpreter not initialized";
        return false;
    }

    if (!std::filesystem::exists(scriptPath)) {
        errorOut = "Script file not found: " + scriptPath.string();
        return false;
    }

    PYAE_LOG_INFO("PythonHost", "Executing script: " + scriptPath.string());

    try {
        ScopedGIL gil;

        // スクリプト読み込み
        std::ifstream file(scriptPath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string code = buffer.str();

        // グローバル辞書に __file__ を設定してファイルパスをトレースバックに含める
        auto globals = py::globals();
        globals["__file__"] = scriptPath.string();

        // 実行
        py::exec(code, globals);

        PYAE_LOG_INFO("PythonHost", "Script executed successfully");
        return true;

    } catch (const py::error_already_set& e) {
        std::string errorMsg = e.what();

        try {
            ScopedGIL gil;

            // Use format_exception with stored exception info from pybind11
            py::module_ traceback = py::module_::import("traceback");
            py::list lines = traceback.attr("format_exception")(e.type(), e.value(), e.trace());
            std::string tb;
            for (const auto& line : lines) {
                tb += line.cast<std::string>();
            }
            if (!tb.empty()) {
                errorMsg = tb;
            }
        } catch (...) {
            // Traceback retrieval failure - use original error
        }

        errorOut = errorMsg;
        PYAE_LOG_ERROR("PythonHost", "Script execution failed: " + errorOut);
        return false;
    } catch (const std::exception& e) {
        errorOut = e.what();
        PYAE_LOG_ERROR("PythonHost", "Script execution failed: " + errorOut);
        return false;
    }
}

bool PythonHost::ExecuteString(const std::string& code, std::string& errorOut) {
    if (!m_initialized.load()) {
        errorOut = "Python interpreter not initialized";
        return false;
    }

    PYAE_LOG_DEBUG("PythonHost", "Executing code string");

    try {
        ScopedGIL gil;

        py::exec(code);

        // Flush stdout/stderr to ensure all buffered output is delivered
        try {
            py::module_ sys = py::module_::import("sys");
            sys.attr("stdout").attr("flush")();
            sys.attr("stderr").attr("flush")();
        } catch (...) {
            // Flush failure is non-critical
        }

        return true;

    } catch (const py::error_already_set& e) {
        std::string errorMsg = e.what();

        try {
            // Note: This is a nested GIL acquisition. The outer ScopedGIL was released when
            // the exception was thrown. We need to reacquire it to safely access Python objects
            // for traceback formatting and stdout/stderr flushing.
            ScopedGIL gil;

            // Flush stdout/stderr before error handling (output before error should be visible)
            try {
                py::module_ sys = py::module_::import("sys");
                sys.attr("stdout").attr("flush")();
                sys.attr("stderr").attr("flush")();
            } catch (...) {}

            // Use format_exception with stored exception info from pybind11.
            // format_exc() won't work because pybind11 clears sys.exc_info() on catch.
            py::module_ traceback = py::module_::import("traceback");
            py::list lines = traceback.attr("format_exception")(e.type(), e.value(), e.trace());
            std::string tb;
            for (const auto& line : lines) {
                tb += line.cast<std::string>();
            }
            if (!tb.empty()) {
                errorMsg = tb;
            }
        } catch (...) {
            // Traceback retrieval failure - use original error
        }

        errorOut = errorMsg;
        PYAE_LOG_ERROR("PythonHost", "Python error: " + errorOut);
        return false;
    } catch (const std::exception& e) {
        errorOut = e.what();
        return false;
    }
}

py::module_ PythonHost::GetAEModule() {
    ScopedGIL gil;
    return py::module_::import("ae");
}

void PythonHost::ReloadAEModule() {
    try {
        ScopedGIL gil;

        py::module_ importlib = py::module_::import("importlib");
        py::module_ ae = py::module_::import("ae");
        importlib.attr("reload")(ae);

        PYAE_LOG_INFO("PythonHost", "AE module reloaded");

    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("PythonHost", std::string("Failed to reload AE module: ") + e.what());
    }
}

void PythonHost::HandlePythonException() {
    try {
        ScopedGIL gil;

        py::module_ traceback = py::module_::import("traceback");
        py::object format_exc = traceback.attr("format_exc");
        std::string tb = format_exc().cast<std::string>();

        WinLockGuard lock(m_errorMutex);
        m_lastError = tb;

    } catch (...) {
        WinLockGuard lock(m_errorMutex);
        m_lastError = "Failed to get Python exception details";
    }
}

std::string PythonHost::GetLastError() const {
    WinLockGuard lock(m_errorMutex);
    return m_lastError;
}

void PythonHost::ClearLastError() {
    WinLockGuard lock(m_errorMutex);
    m_lastError.clear();
}

// PythonExecutionContext実装
py::object PythonExecutionContext::Execute(const std::string& code) {
    ScopedGIL gil;

    if (m_globals.empty()) {
        m_globals = py::module_::import("__main__").attr("__dict__");
    }

    return py::eval(code, m_globals, m_locals);
}

py::object PythonExecutionContext::ExecuteFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return Execute(buffer.str());
}

} // namespace PyAE
