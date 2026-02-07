// AEModule.cpp
// PyAE - Python for After Effects
// メインPythonモジュール定義

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShlObj.h>
#endif

#include "PluginState.h"
#include "IdleHandler.h"
#include "Logger.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "ScopedHandles.h"
#include "MemoryDiagnostics.h"

namespace py = pybind11;

// 前方宣言（他のバインディングファイルで定義）
void init_project(py::module_& m);
void init_item(py::module_& m);
void init_comp(py::module_& m);
void init_layer(py::module_& m);
void init_property(py::module_& m);
void init_keyframe(py::module_& m);
// Phase 5 追加
void init_effect(py::module_& m);
void init_mask(py::module_& m);
void init_render_queue(py::module_& m);
void init_batch(py::module_& m);
void init_3d_layer(py::module_& m);
void init_marker(py::module_& m); // Marker wrapper
void init_sdk(py::module_& m); // Low-level API
// High-level APIs
void init_persistent_data(py::module_& m); // Persistent data (preferences)
void init_menu(py::module_& m); // Menu commands
void init_render_monitor(py::module_& m); // Render queue monitoring
void init_async_render(py::module_& m); // Async rendering

namespace PyAE {
void init_color_profile(py::module_& m); // Color profile and OCIO settings
void init_world(py::module_& m);         // World (frame buffer) API
void init_footage(py::module_& m);       // Footage API
void init_render(py::module_& m);        // Render API
void init_layer_render_options(py::module_& m); // Layer render options API
void init_sound_data(py::module_& m);    // Sound data API
}

namespace PyAE {
void init_panel_ui(py::module_& m); // PySide6 Panel UI
}

namespace PyAE {

// AEメインスレッドで関数を実行
template<typename Func>
auto ExecuteOnMainThread(Func&& func) {
    using ReturnType = std::invoke_result_t<Func>;

    if constexpr (std::is_void_v<ReturnType>) {
        auto future = IdleHandler::Instance().EnqueueTaskWithResult([func = std::forward<Func>(func)]() {
            func();
        });
        // GILを解放して待機
        py::gil_scoped_release release;
        future.wait();
    } else {
        auto future = IdleHandler::Instance().EnqueueTaskWithResult([func = std::forward<Func>(func)]() {
            return func();
        });
        // GILを解放して待機
        py::gil_scoped_release release;
        return future.get();
    }
}

// ユーティリティ関数
std::string GetPluginVersion() {
    return PYAE_VERSION;
}

bool IsInitialized() {
    return PluginState::Instance().IsInitialized();
}

void Log(const std::string& level, const std::string& message) {
    if (level == "debug") {
        PYAE_LOG_DEBUG("Python", message);
    } else if (level == "info") {
        PYAE_LOG_INFO("Python", message);
    } else if (level == "warning") {
        PYAE_LOG_WARNING("Python", message);
    } else if (level == "error") {
        PYAE_LOG_ERROR("Python", message);
    } else {
        PYAE_LOG_INFO("Python", message);
    }
}

// =============================================================
// グローバル関数
// =============================================================

// アラートダイアログ表示
void Alert(const std::string& message, const std::string& title = "PyAE") {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.utilitySuite) {
        throw std::runtime_error("Utility suite not available");
    }

    std::wstring wmessage = StringUtils::Utf8ToWide(message);
    std::wstring wtitle = StringUtils::Utf8ToWide(title);

    suites.utilitySuite->AEGP_ReportInfo(
        state.GetPluginID(),
        reinterpret_cast<const A_char*>(message.c_str()));
}

// コンソール出力（デバッグ用）
void WriteLn(const std::string& message) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (suites.utilitySuite) {
        suites.utilitySuite->AEGP_ReportInfo(
            state.GetPluginID(),
            reinterpret_cast<const A_char*>(message.c_str()));
    }

    // ログにも出力
    PYAE_LOG_INFO("Console", message);
}

// アンドゥグループ開始
void BeginUndoGroup(const std::string& name) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.utilitySuite) {
        throw std::runtime_error("Utility suite not available");
    }

    A_Err err = suites.utilitySuite->AEGP_StartUndoGroup(name.c_str());
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to begin undo group");
    }
}

// アンドゥグループ終了
void EndUndoGroup() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.utilitySuite) {
        throw std::runtime_error("Utility suite not available");
    }

    A_Err err = suites.utilitySuite->AEGP_EndUndoGroup();
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to end undo group");
    }
}

// Timeline refresh (uses PF_RefreshAllWindows when available)
void RefreshTimeline() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Prefer PF_RefreshAllWindows (more comprehensive)
    if (suites.advAppSuite) {
        A_Err err = suites.advAppSuite->PF_RefreshAllWindows();
        if (err == A_Err_NONE) {
            return;
        }
    }

    // Fallback to AEGP_CauseIdleRoutinesToBeCalled
    if (!suites.utilitySuite) {
        throw std::runtime_error("No refresh suite available");
    }
    suites.utilitySuite->AEGP_CauseIdleRoutinesToBeCalled();
}

// Trigger re-evaluation of the active item
void TouchActiveItem() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.advItemSuite) {
        throw std::runtime_error("AdvItem Suite not available");
    }

    A_Err err = suites.advItemSuite->PF_TouchActiveItem();
    if (err != A_Err_NONE) {
        throw std::runtime_error("PF_TouchActiveItem failed: error " + std::to_string(err));
    }
}

// Bring AE to the foreground
void ForceForeground() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.advAppSuite) {
        throw std::runtime_error("AdvApp Suite not available");
    }

    A_Err err = suites.advAppSuite->PF_ForceForeground();
    if (err != A_Err_NONE) {
        throw std::runtime_error("PF_ForceForeground failed: error " + std::to_string(err));
    }
}

// AE情報取得
py::dict GetAEInfo() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    py::dict info;

    // AEバージョン取得
    if (suites.utilitySuite) {
        A_short major, minor;
        A_Err err = suites.utilitySuite->AEGP_GetDriverImplementationVersion(&major, &minor);
        if (err == A_Err_NONE) {
            info["driver_major"] = major;
            info["driver_minor"] = minor;
        }
    }

    // プラグインID
    info["plugin_id"] = state.GetPluginID();

    // PyAEバージョン
    info["pyae_version"] = PYAE_VERSION;

    return info;
}

// 時間変換ユーティリティ
double FramesToSeconds(int frames, double fps) {
    if (fps <= 0) return 0.0;
    return static_cast<double>(frames) / fps;
}

int SecondsToFrames(double seconds, double fps) {
    if (fps <= 0) return 0;
    return static_cast<int>(seconds * fps + 0.5);  // 四捨五入
}

std::string TimecodeFromSeconds(double seconds, double fps, bool dropFrame = false) {
    if (fps <= 0) return "00:00:00:00";

    int totalFrames = static_cast<int>(seconds * fps + 0.5);
    int framesPerSecond = static_cast<int>(fps + 0.5);

    int hours = totalFrames / (3600 * framesPerSecond);
    totalFrames %= (3600 * framesPerSecond);

    int minutes = totalFrames / (60 * framesPerSecond);
    totalFrames %= (60 * framesPerSecond);

    int secs = totalFrames / framesPerSecond;
    int frames = totalFrames % framesPerSecond;

    char buffer[32];
    const char* separator = dropFrame ? ";" : ":";
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d%s%02d",
             hours, minutes, secs, separator, frames);

    return std::string(buffer);
}

double SecondsFromTimecode(const std::string& timecode, double fps) {
    if (fps <= 0) return 0.0;

    int hours = 0, minutes = 0, seconds = 0, frames = 0;

    // ドロップフレーム（;）と非ドロップ（:）両方対応
    std::string tc = timecode;
    for (char& c : tc) {
        if (c == ';') c = ':';
    }

    if (sscanf(tc.c_str(), "%d:%d:%d:%d", &hours, &minutes, &seconds, &frames) != 4) {
        return 0.0;
    }

    int framesPerSecond = static_cast<int>(fps + 0.5);
    int totalFrames = hours * 3600 * framesPerSecond +
                      minutes * 60 * framesPerSecond +
                      seconds * framesPerSecond +
                      frames;

    return static_cast<double>(totalFrames) / fps;
}

// プロジェクトパス取得
std::string GetProjectPath() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.projSuite || !suites.memorySuite) {
        return "";
    }

    AEGP_ProjectH projH;
    A_Err err = suites.projSuite->AEGP_GetProjectByIndex(0, &projH);
    if (err != A_Err_NONE || !projH) {
        return "";
    }

    AEGP_MemHandle pathH;
    err = suites.projSuite->AEGP_GetProjectPath(projH, &pathH);
    if (err != A_Err_NONE || !pathH) {
        return "";
    }

    ScopedMemHandle scopedPath(state.GetPluginID(), suites.memorySuite, pathH);

    ScopedMemLock lock(suites.memorySuite, pathH);
    A_UTF16Char* pathPtr = lock.As<A_UTF16Char>();
    if (!pathPtr) return "";

    std::string result = StringUtils::Utf16ToUtf8(pathPtr);
    return result;
}

// 新規プロジェクトを作成
void NewProject() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.projSuite) {
        throw std::runtime_error("Project suite not available");
    }

    AEGP_ProjectH newProjH = nullptr;
    A_Err err = suites.projSuite->AEGP_NewProject(&newProjH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to create new project");
    }
}

// システムフォルダパス取得
std::string GetUserDocumentsFolder() {
#ifdef _WIN32
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);
    if (SUCCEEDED(hr) && path) {
        std::wstring wpath(path);
        CoTaskMemFree(path);
        return StringUtils::WideToUtf8(wpath);
    }
    return "";
#else
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/Documents";
    }
    return "";
#endif
}

// スクリプト実行（ファイルから）
void ExecuteScriptFile(const std::string& filePath) {
    try {
        py::eval_file(filePath);
    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("Script", std::string("Script error: ") + e.what());
        throw;
    }
}

// スクリプト実行（文字列から）
py::object ExecuteScript(const std::string& script) {
    try {
        return py::eval(script);
    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("Script", std::string("Script error: ") + e.what());
        throw;
    }
}

// メニューコマンドを実行
void ExecuteCommand(int commandId) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.commandSuite) {
        throw std::runtime_error("Command suite not available");
    }

    A_Err err = suites.commandSuite->AEGP_DoCommand(static_cast<AEGP_Command>(commandId));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to execute command: " + std::to_string(commandId));
    }
}

} // namespace PyAE

// メインモジュール定義
// PYBIND11_EMBEDDED_MODULE: 埋め込み用（DLLエクスポートを生成しない）
PYBIND11_EMBEDDED_MODULE(ae, m) {
    m.doc() = "PyAE - Python bindings for Adobe After Effects";

    // バージョン情報
    m.attr("__version__") = PYAE_VERSION;

    // ユーティリティ関数
    m.def("version", &PyAE::GetPluginVersion, "Get PyAE version string");
    m.def("is_initialized", &PyAE::IsInitialized, "Check if plugin is initialized");

    // ロギング
    m.def("log", &PyAE::Log, "Log a message",
          py::arg("level"), py::arg("message"));

    m.def("log_debug", [](const std::string& msg) { PyAE::Log("debug", msg); },
          "Log debug message");
    m.def("log_info", [](const std::string& msg) { PyAE::Log("info", msg); },
          "Log info message");
    m.def("log_warning", [](const std::string& msg) { PyAE::Log("warning", msg); },
          "Log warning message");
    m.def("log_error", [](const std::string& msg) { PyAE::Log("error", msg); },
          "Log error message");

    m.def("enable_file_logging", [](bool enabled) {
        PyAE::Logger::Instance().SetFileOutputEnabled(enabled);
    }, "Enable or disable file logging output", py::arg("enabled") = true);
    m.def("is_file_logging_enabled", []() -> bool {
        return PyAE::Logger::Instance().IsFileOutputEnabled();
    }, "Check if file logging output is enabled");
    m.def("get_log_path", []() -> std::string {
        return PyAE::Logger::Instance().GetLogPath().string();
    }, "Get current log file path (empty if file logging is disabled)");

    // アラートとコンソール
    m.def("alert", &PyAE::Alert, "Show alert dialog",
          py::arg("message"), py::arg("title") = "PyAE");
    m.def("write_ln", &PyAE::WriteLn, "Write to console",
          py::arg("message"));

    // アンドゥグループ
    m.def("begin_undo_group", &PyAE::BeginUndoGroup, "Begin an undo group",
          py::arg("name"));
    m.def("end_undo_group", &PyAE::EndUndoGroup, "End the current undo group");

    // タイムライン更新
    m.def("refresh", &PyAE::RefreshTimeline, "Refresh the timeline display");
    m.def("touch_active_item", &PyAE::TouchActiveItem,
          "Mark the active item as modified and trigger re-evaluation");
    m.def("force_foreground", &PyAE::ForceForeground,
          "Bring After Effects to the foreground");

    // システム情報
    m.def("get_ae_info", &PyAE::GetAEInfo, "Get After Effects information");
    m.def("get_project_path", &PyAE::GetProjectPath, "Get current project file path");
    m.def("get_documents_folder", &PyAE::GetUserDocumentsFolder, "Get user's documents folder path");

    // プロジェクト操作
    m.def("new_project", &PyAE::NewProject, "Create a new project");

    // 時間変換ユーティリティ
    m.def("frames_to_seconds", &PyAE::FramesToSeconds, "Convert frames to seconds",
          py::arg("frames"), py::arg("fps"));
    m.def("seconds_to_frames", &PyAE::SecondsToFrames, "Convert seconds to frames",
          py::arg("seconds"), py::arg("fps"));
    m.def("timecode_from_seconds", &PyAE::TimecodeFromSeconds, "Convert seconds to timecode string",
          py::arg("seconds"), py::arg("fps"), py::arg("drop_frame") = false);
    m.def("seconds_from_timecode", &PyAE::SecondsFromTimecode, "Convert timecode string to seconds",
          py::arg("timecode"), py::arg("fps"));

    // アイドルタスクスケジュール
    m.def("schedule_idle_task", [](const std::function<void()>& func) {
        PyAE::IdleHandler::Instance().EnqueueTask(func);
    }, "Schedule a task to run during AE idle time (safe for UI/Scene updates)",
    py::arg("func"));

    // スクリプト実行 (Python)
    m.def("execute_script_file", &PyAE::ExecuteScriptFile, "Execute a Python script file",
          py::arg("file_path"));
    m.def("execute_script", &PyAE::ExecuteScript, "Execute a Python script string",
          py::arg("script"));

    // ExtendScript 実行
    m.def("execute_extendscript", [](const std::string& script) -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_MemHandle resultH = nullptr;
        AEGP_MemHandle errorH = nullptr;

        A_Err err = suites.utilitySuite->AEGP_ExecuteScript(
            state.GetPluginID(),
            script.c_str(),
            TRUE,  // platform_encoding
            &resultH,
            &errorH
        );

        std::string result;
        std::string errorStr;

        // Get error string if available
        if (errorH) {
            A_char* errorPtr = nullptr;
            suites.memorySuite->AEGP_LockMemHandle(errorH, reinterpret_cast<void**>(&errorPtr));
            if (errorPtr) {
                errorStr = errorPtr;
            }
            suites.memorySuite->AEGP_UnlockMemHandle(errorH);
            suites.memorySuite->AEGP_FreeMemHandle(errorH);
        }

        // Get result string if available
        if (resultH) {
            A_char* resultPtr = nullptr;
            suites.memorySuite->AEGP_LockMemHandle(resultH, reinterpret_cast<void**>(&resultPtr));
            if (resultPtr) {
                result = resultPtr;
            }
            suites.memorySuite->AEGP_UnlockMemHandle(resultH);
            suites.memorySuite->AEGP_FreeMemHandle(resultH);
        }

        if (err != A_Err_NONE || !errorStr.empty()) {
            throw std::runtime_error("ExtendScript error: " + errorStr);
        }

        return result;
    }, py::arg("script"),
    "Execute ExtendScript (JavaScript) code and return the result as a string");

    // メニューコマンド実行
    m.def("execute_command", &PyAE::ExecuteCommand,
          "Execute a menu command by ID (e.g., Layer Styles)",
          py::arg("command_id"));

    // サブモジュール初期化
    init_project(m);
    init_item(m);
    init_comp(m);
    init_layer(m);
    init_property(m);
    init_keyframe(m);
    // Phase 5 追加
    init_effect(m);
    init_mask(m);
    init_render_queue(m);
    init_batch(m);
    init_3d_layer(m);
    init_marker(m);
    init_sdk(m);
    PyAE::init_panel_ui(m);
    PyAE::init_color_profile(m);
    PyAE::init_world(m);
    PyAE::init_footage(m);
    PyAE::init_render(m);
    PyAE::init_layer_render_options(m);
    PyAE::init_sound_data(m);
    // High-level APIs (new)
    init_persistent_data(m);
    init_menu(m);
    init_render_monitor(m);
    init_async_render(m);

    // メモリ診断API
    py::class_<PyAE::MemoryDiagnostics::MemStats>(m, "MemStats")
        .def(py::init<>())
        .def_readonly("handle_count", &PyAE::MemoryDiagnostics::MemStats::handleCount,
                      "Number of memory handles allocated")
        .def_readonly("total_size", &PyAE::MemoryDiagnostics::MemStats::totalSize,
                      "Total memory size in bytes")
        .def("__repr__", [](const PyAE::MemoryDiagnostics::MemStats& stats) {
            return "MemStats(handles=" + std::to_string(stats.handleCount) +
                   ", size=" + std::to_string(stats.totalSize) + " bytes)";
        });

    m.def("get_memory_stats", &PyAE::MemoryDiagnostics::GetMemoryStats,
          "Get current memory usage statistics");
    m.def("enable_memory_reporting", &PyAE::MemoryDiagnostics::EnableMemoryReporting,
          "Enable/disable detailed memory reporting (debug only)",
          py::arg("enable"));
    m.def("log_memory_stats", &PyAE::MemoryDiagnostics::LogMemoryStats,
          "Log current memory statistics",
          py::arg("context"));
    m.def("check_memory_leak", &PyAE::MemoryDiagnostics::CheckMemoryLeak,
          "Check for memory leak between two memory stats snapshots",
          py::arg("before"), py::arg("after"),
          py::arg("tolerance_handles") = 0, py::arg("tolerance_size") = 0);
    m.def("log_memory_leak_details", &PyAE::MemoryDiagnostics::LogMemoryLeakDetails,
          "Log detailed memory leak information",
          py::arg("before"), py::arg("after"), py::arg("context"));

    // LayerクラスにpropertyメソッドはPyProperty.cpp内のinit_propertyで追加される
    // (init_layerの後にinit_propertyが呼ばれるため)

    // Shortcut functions removed - use ae.Project.get_current(), ae.Comp.get_active(), ae.Item.get_active() instead

    // アンドゥグループをコンテキストマネージャーとして使うためのクラス
    py::class_<PyAE::ScopedUndoGroup>(m, "UndoGroup")
        .def(py::init([](const std::string& name) {
            auto& state = PyAE::PluginState::Instance();
            const auto& suites = state.GetSuites();
            return new PyAE::ScopedUndoGroup(suites.utilitySuite, state.GetPluginID(), name.c_str());
        }), py::arg("name"))
        .def("__enter__", [](PyAE::ScopedUndoGroup& self) -> PyAE::ScopedUndoGroup& {
            return self;
        })
        .def("__exit__", [](PyAE::ScopedUndoGroup& self, py::object, py::object, py::object) {
            self.End();
        });

    // 便利な定数
    py::module_ constants = m.def_submodule("const", "PyAE Constants");
    constants.attr("PI") = 3.14159265358979323846;
    constants.attr("E") = 2.71828182845904523536;
    constants.attr("GOLDEN_RATIO") = 1.61803398874989484820;

    // 一般的なフレームレート
    constants.attr("FPS_24") = 24.0;
    constants.attr("FPS_25") = 25.0;
    constants.attr("FPS_29_97") = 29.97;
    constants.attr("FPS_30") = 30.0;
    constants.attr("FPS_60") = 60.0;

    // 一般的な解像度
    py::dict resolutions;
    resolutions["HD"] = py::make_tuple(1920, 1080);
    resolutions["4K"] = py::make_tuple(3840, 2160);
    resolutions["720p"] = py::make_tuple(1280, 720);
    resolutions["1080p"] = py::make_tuple(1920, 1080);
    resolutions["2160p"] = py::make_tuple(3840, 2160);
    resolutions["Instagram_Square"] = py::make_tuple(1080, 1080);
    resolutions["Instagram_Story"] = py::make_tuple(1080, 1920);
    resolutions["YouTube_Short"] = py::make_tuple(1080, 1920);
    constants.attr("RESOLUTIONS") = resolutions;
}
