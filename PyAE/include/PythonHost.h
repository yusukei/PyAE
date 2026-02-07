// PythonHost.h
// PyAE - Python for After Effects
// Python埋め込みインタープリター管理

#pragma once

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include <string>
#include <filesystem>
#include <atomic>
#include <thread>
#include <functional>
#include "WinSync.h"

namespace py = pybind11;

namespace PyAE {

// Python初期化設定
struct PythonConfig {
    std::filesystem::path pythonHome;     // Python埋め込みディレクトリ
    std::filesystem::path scriptsDir;     // ユーザースクリプトディレクトリ
    bool enableSitePackages = false;      // site-packages有効化
    bool enableREPL = false;              // REPLサーバー有効化
    int replPort = 9999;                  // REPLポート
};

// GILスコープガード（GILを取得）
class ScopedGIL {
public:
    ScopedGIL() : m_state(PyGILState_Ensure()) {}
    ~ScopedGIL() { PyGILState_Release(m_state); }

    ScopedGIL(const ScopedGIL&) = delete;
    ScopedGIL& operator=(const ScopedGIL&) = delete;

private:
    PyGILState_STATE m_state;
};

// GIL解放スコープガード（GILを解放）
class ScopedGILRelease {
public:
    ScopedGILRelease() : m_state(PyEval_SaveThread()) {}
    ~ScopedGILRelease() { PyEval_RestoreThread(m_state); }

    ScopedGILRelease(const ScopedGILRelease&) = delete;
    ScopedGILRelease& operator=(const ScopedGILRelease&) = delete;

private:
    PyThreadState* m_state;
};

// Python出力コールバック型
using PythonOutputCallback = std::function<void(const std::string& text, bool isError)>;

class PythonHost {
public:
    static PythonHost& Instance() {
        static PythonHost instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize(const PythonConfig& config);
    void Shutdown();

    // 状態チェック
    bool IsInitialized() const { return m_initialized.load(); }
    bool IsShuttingDown() const { return m_shuttingDown.load(); }

    // スクリプト実行
    bool ExecuteFile(const std::filesystem::path& scriptPath, std::string& errorOut);
    bool ExecuteString(const std::string& code, std::string& errorOut);

    // 出力コールバック登録（パネルに出力を送るため）
    void SetOutputCallback(PythonOutputCallback callback);
    void ClearOutputCallback();

    // 出力をコールバックに送信（内部用）
    void NotifyOutput(const std::string& text, bool isError);

    // モジュール操作
    py::module_ GetAEModule();
    void ReloadAEModule();

    // コールバック登録（AEメインスレッドで実行される関数）
    using MainThreadCallback = std::function<void()>;
    void RegisterMainThreadCallback(MainThreadCallback callback);

    // Python例外情報取得
    std::string GetLastError() const;
    void ClearLastError();

    // 設定取得
    const PythonConfig& GetConfig() const { return m_config; }

private:
    PythonHost() = default;
    ~PythonHost() = default;

    PythonHost(const PythonHost&) = delete;
    PythonHost& operator=(const PythonHost&) = delete;

    bool SetupPythonPath();
    bool InitializeAEModule();
    void HandlePythonException();
    void RedirectPythonOutput();

    PythonConfig m_config;
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_shuttingDown{false};

    // Python home string must persist for the lifetime of this object
    // because Py_SetPythonHome does NOT copy the string - it stores a pointer
    std::wstring m_pythonHomeStr;

    mutable WinMutex m_errorMutex;
    std::string m_lastError;

    // 出力コールバック
    mutable WinMutex m_outputCallbackMutex;
    PythonOutputCallback m_outputCallback;

    // pybind11スコープドインタープリター
    std::unique_ptr<py::scoped_interpreter> m_interpreter;
    std::unique_ptr<py::gil_scoped_release> m_gilRelease;
};

// Python例外をC++例外に変換するヘルパー
class PythonException : public std::runtime_error {
public:
    PythonException(const std::string& type, const std::string& message, const std::string& traceback)
        : std::runtime_error(message)
        , m_type(type)
        , m_traceback(traceback)
    {}

    const std::string& GetType() const { return m_type; }
    const std::string& GetTraceback() const { return m_traceback; }

    std::string GetFullMessage() const {
        return m_type + ": " + what() + "\n" + m_traceback;
    }

private:
    std::string m_type;
    std::string m_traceback;
};

// Python実行コンテキスト
class PythonExecutionContext {
public:
    explicit PythonExecutionContext(const std::string& scriptName = "<pyae>")
        : m_scriptName(scriptName)
    {}

    // グローバル変数の設定
    void SetGlobal(const std::string& name, py::object value) {
        m_globals[name.c_str()] = std::move(value);
    }

    // ローカル変数の設定
    void SetLocal(const std::string& name, py::object value) {
        m_locals[name.c_str()] = std::move(value);
    }

    // 実行
    py::object Execute(const std::string& code);
    py::object ExecuteFile(const std::filesystem::path& path);

private:
    std::string m_scriptName;
    py::dict m_globals;
    py::dict m_locals;
};

} // namespace PyAE
