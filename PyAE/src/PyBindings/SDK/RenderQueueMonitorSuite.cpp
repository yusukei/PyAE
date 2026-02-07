#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

// RAII helper class for AEGP_MemHandle to ensure proper cleanup
class RQMScopedMemHandle {
public:
    RQMScopedMemHandle(AEGP_MemHandle handle, const AEGP_MemorySuite1* memorySuite)
        : m_handle(handle), m_memorySuite(memorySuite), m_locked(false), m_ptr(nullptr) {}

    ~RQMScopedMemHandle() {
        if (m_locked && m_ptr) {
            m_memorySuite->AEGP_UnlockMemHandle(m_handle);
        }
        if (m_handle) {
            m_memorySuite->AEGP_FreeMemHandle(m_handle);
        }
    }

    // Delete copy constructor and assignment operator
    RQMScopedMemHandle(const RQMScopedMemHandle&) = delete;
    RQMScopedMemHandle& operator=(const RQMScopedMemHandle&) = delete;

    A_Err Lock(void** ptr) {
        A_Err err = m_memorySuite->AEGP_LockMemHandle(m_handle, ptr);
        if (err == A_Err_NONE) {
            m_locked = true;
            m_ptr = *ptr;
        }
        return err;
    }

    // Get the size of the memory block
    A_Err GetSize(AEGP_MemSize* size) const {
        return m_memorySuite->AEGP_GetMemHandleSize(m_handle, size);
    }

private:
    AEGP_MemHandle m_handle;
    const AEGP_MemorySuite1* m_memorySuite;
    bool m_locked;
    void* m_ptr;
};

// Helper function: Convert UTF-16 MemHandle to std::wstring
static std::wstring MemHandleToWString(AEGP_MemHandle memH, const AEGP_MemorySuite1* memorySuite) {
    if (!memH || !memorySuite) return std::wstring();

    RQMScopedMemHandle scopedHandle(memH, memorySuite);
    A_UTF16Char* utf16_str = nullptr;
    A_Err err = scopedHandle.Lock(reinterpret_cast<void**>(&utf16_str));
    if (err != A_Err_NONE || !utf16_str) {
        return std::wstring();
    }

    // UTF-16 to wstring (on Windows, wchar_t is UTF-16)
    return std::wstring(reinterpret_cast<const wchar_t*>(utf16_str));
}

void init_RenderQueueMonitorSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_RenderQueueMonitorSuite1
    // -----------------------------------------------------------------------
    //
    // This suite provides monitoring capabilities for render queue operations.
    //
    // IMPORTANT: Listener Registration Limitation
    // -------------------------------------------
    // AEGP_RegisterListener and AEGP_DeregisterListener require C function
    // pointers (callbacks) which cannot be directly created from Python.
    // These functions are implemented as stubs that throw exceptions.
    //
    // The Get* functions require a valid session_id which is normally obtained
    // from listener callbacks. For testing purposes, you can try using 0 as
    // session_id, but this may not work in all cases.
    //
    // Index Convention: All indices in this suite are 0-based.
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Constants: Finished Status
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_RQM_FinishedStatus_UNKNOWN") = static_cast<int>(AEGP_RQM_FinishedStatus_UNKNOWN);
    sdk.attr("AEGP_RQM_FinishedStatus_SUCCEEDED") = static_cast<int>(AEGP_RQM_FinishedStatus_SUCCEEDED);
    sdk.attr("AEGP_RQM_FinishedStatus_ABORTED") = static_cast<int>(AEGP_RQM_FinishedStatus_ABORTED);
    sdk.attr("AEGP_RQM_FinishedStatus_ERRED") = static_cast<int>(AEGP_RQM_FinishedStatus_ERRED);

    // -----------------------------------------------------------------------
    // AEGP_RegisterListener (STUB - Cannot be implemented in Python)
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_RegisterListener", []() {
        throw std::runtime_error(
            "AEGP_RegisterListener cannot be called from Python. "
            "This function requires C callback function pointers which cannot be created from Python. "
            "Use the After Effects C++ SDK directly if you need listener functionality.");
    },
    R"doc(
    リスナーを登録します。

    **注意**: この関数はPythonから呼び出すことはできません。
    Cコールバック関数ポインタが必要なため、スタブ実装として例外をスローします。
    リスナー機能が必要な場合は、After Effects C++ SDKを直接使用してください。

    Raises:
        RuntimeError: 常にスローされます（スタブ実装）
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_DeregisterListener (STUB - Cannot be implemented in Python)
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_DeregisterListener", []() {
        throw std::runtime_error(
            "AEGP_DeregisterListener cannot be called from Python. "
            "This function requires C callback function pointers which cannot be created from Python. "
            "Use the After Effects C++ SDK directly if you need listener functionality.");
    },
    R"doc(
    リスナーを登録解除します。

    **注意**: この関数はPythonから呼び出すことはできません。
    Cコールバック関数ポインタが必要なため、スタブ実装として例外をスローします。
    リスナー機能が必要な場合は、After Effects C++ SDKを直接使用してください。

    Raises:
        RuntimeError: 常にスローされます（スタブ実装）
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetProjectName
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetProjectName", [](uint64_t session_id) -> std::wstring {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_MemHandle nameH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetProjectName(sessid, &nameH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetProjectName failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) + ")");
        }

        return MemHandleToWString(nameH, suites.memorySuite);
    }, py::arg("session_id"),
    R"doc(
    レンダリングジョブに関連付けられたプロジェクト名を取得します。

    Args:
        session_id: セッションID（リスナーコールバックから取得、テスト用に0を使用可能）

    Returns:
        str: プロジェクト名（UTF-16文字列）

    Raises:
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetAppVersion
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetAppVersion", [](uint64_t session_id) -> std::wstring {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_MemHandle versionH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetAppVersion(sessid, &versionH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetAppVersion failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) + ")");
        }

        return MemHandleToWString(versionH, suites.memorySuite);
    }, py::arg("session_id"),
    R"doc(
    After Effects のアプリケーションバージョンを取得します。

    Args:
        session_id: セッションID（リスナーコールバックから取得、テスト用に0を使用可能）

    Returns:
        str: アプリケーションバージョン文字列

    Raises:
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetNumJobItems
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetNumJobItems", [](uint64_t session_id) -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        A_long num_items = 0;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItems(sessid, &num_items);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetNumJobItems failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) + ")");
        }

        return static_cast<int>(num_items);
    }, py::arg("session_id"),
    R"doc(
    レンダリングジョブ内のアイテム数を取得します。

    Args:
        session_id: セッションID

    Returns:
        int: ジョブアイテムの数

    Raises:
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetJobItemID
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetJobItemID", [](uint64_t session_id, int job_item_index) -> uint64_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        Validation::RequireNonNegative(job_item_index, "job_item_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId item_id = 0;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemID(sessid, job_item_index, &item_id);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetJobItemID failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", job_item_index: " + std::to_string(job_item_index) + ")");
        }

        return static_cast<uint64_t>(item_id);
    }, py::arg("session_id"), py::arg("job_item_index"),
    R"doc(
    指定されたインデックスのジョブアイテムIDを取得します。

    Args:
        session_id: セッションID
        job_item_index: ジョブアイテムのインデックス（0ベース）

    Returns:
        int: ジョブアイテムID

    Raises:
        ValueError: job_item_index が負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetNumJobItemRenderSettings
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetNumJobItemRenderSettings", [](uint64_t session_id, uint64_t item_id) -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        A_long num_settings = 0;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemRenderSettings(sessid, itemid, &num_settings);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetNumJobItemRenderSettings failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) + ")");
        }

        return static_cast<int>(num_settings);
    }, py::arg("session_id"), py::arg("item_id"),
    R"doc(
    ジョブアイテムのレンダリング設定の数を取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID

    Returns:
        int: レンダリング設定の数

    Raises:
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetJobItemRenderSetting
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetJobItemRenderSetting", [](uint64_t session_id, uint64_t item_id, int setting_index) -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        Validation::RequireNonNegative(setting_index, "setting_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        AEGP_MemHandle nameH = nullptr;
        AEGP_MemHandle valueH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemRenderSetting(
            sessid, itemid, setting_index, &nameH, &valueH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetJobItemRenderSetting failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", setting_index: " + std::to_string(setting_index) + ")");
        }

        std::wstring name = MemHandleToWString(nameH, suites.memorySuite);
        std::wstring value = MemHandleToWString(valueH, suites.memorySuite);

        return py::make_tuple(name, value);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("setting_index"),
    R"doc(
    ジョブアイテムのレンダリング設定を取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        setting_index: 設定のインデックス（0ベース）

    Returns:
        tuple: (name, value) - 設定名と値のタプル

    Raises:
        ValueError: setting_index が負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetNumJobItemOutputModules
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetNumJobItemOutputModules", [](uint64_t session_id, uint64_t item_id) -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        A_long num_modules = 0;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemOutputModules(sessid, itemid, &num_modules);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetNumJobItemOutputModules failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) + ")");
        }

        return static_cast<int>(num_modules);
    }, py::arg("session_id"), py::arg("item_id"),
    R"doc(
    ジョブアイテムの出力モジュールの数を取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID

    Returns:
        int: 出力モジュールの数

    Raises:
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetNumJobItemOutputModuleSettings
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetNumJobItemOutputModuleSettings", [](uint64_t session_id, uint64_t item_id, int output_module_index) -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        Validation::RequireNonNegative(output_module_index, "output_module_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        A_long num_settings = 0;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemOutputModuleSettings(
            sessid, itemid, output_module_index, &num_settings);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetNumJobItemOutputModuleSettings failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", output_module_index: " + std::to_string(output_module_index) + ")");
        }

        return static_cast<int>(num_settings);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("output_module_index"),
    R"doc(
    出力モジュールの設定の数を取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        output_module_index: 出力モジュールのインデックス（0ベース）

    Returns:
        int: 出力モジュール設定の数

    Raises:
        ValueError: output_module_index が負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetJobItemOutputModuleSetting
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetJobItemOutputModuleSetting", [](uint64_t session_id, uint64_t item_id,
                                                          int output_module_index, int setting_index) -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        Validation::RequireNonNegative(output_module_index, "output_module_index");
        Validation::RequireNonNegative(setting_index, "setting_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        AEGP_MemHandle nameH = nullptr;
        AEGP_MemHandle valueH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemOutputModuleSetting(
            sessid, itemid, output_module_index, setting_index, &nameH, &valueH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetJobItemOutputModuleSetting failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", output_module_index: " + std::to_string(output_module_index) +
                                   ", setting_index: " + std::to_string(setting_index) + ")");
        }

        std::wstring name = MemHandleToWString(nameH, suites.memorySuite);
        std::wstring value = MemHandleToWString(valueH, suites.memorySuite);

        return py::make_tuple(name, value);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("output_module_index"), py::arg("setting_index"),
    R"doc(
    出力モジュールの設定を取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        output_module_index: 出力モジュールのインデックス（0ベース）
        setting_index: 設定のインデックス（0ベース）

    Returns:
        tuple: (name, value) - 設定名と値のタプル

    Raises:
        ValueError: インデックスが負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetNumJobItemOutputModuleWarnings
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetNumJobItemOutputModuleWarnings", [](uint64_t session_id, uint64_t item_id, int output_module_index) -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        Validation::RequireNonNegative(output_module_index, "output_module_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        A_long num_warnings = 0;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemOutputModuleWarnings(
            sessid, itemid, output_module_index, &num_warnings);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetNumJobItemOutputModuleWarnings failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", output_module_index: " + std::to_string(output_module_index) + ")");
        }

        return static_cast<int>(num_warnings);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("output_module_index"),
    R"doc(
    出力モジュールの警告の数を取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        output_module_index: 出力モジュールのインデックス（0ベース）

    Returns:
        int: 警告の数

    Raises:
        ValueError: output_module_index が負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetJobItemOutputModuleWarning
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetJobItemOutputModuleWarning", [](uint64_t session_id, uint64_t item_id,
                                                          int output_module_index, int warning_index) -> std::wstring {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        Validation::RequireNonNegative(output_module_index, "output_module_index");
        Validation::RequireNonNegative(warning_index, "warning_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        AEGP_MemHandle warningH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemOutputModuleWarning(
            sessid, itemid, output_module_index, warning_index, &warningH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetJobItemOutputModuleWarning failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", output_module_index: " + std::to_string(output_module_index) +
                                   ", warning_index: " + std::to_string(warning_index) + ")");
        }

        return MemHandleToWString(warningH, suites.memorySuite);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("output_module_index"), py::arg("warning_index"),
    R"doc(
    出力モジュールの警告メッセージを取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        output_module_index: 出力モジュールのインデックス（0ベース）
        warning_index: 警告のインデックス（0ベース）

    Returns:
        str: 警告メッセージ

    Raises:
        ValueError: インデックスが負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetNumJobItemFrameProperties
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetNumJobItemFrameProperties", [](uint64_t session_id, uint64_t item_id, uint64_t frame_id) -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        AEGP_RQM_FrameId frameid = static_cast<AEGP_RQM_FrameId>(frame_id);
        A_long num_properties = 0;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemFrameProperties(
            sessid, itemid, frameid, &num_properties);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetNumJobItemFrameProperties failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", frame_id: " + std::to_string(frame_id) + ")");
        }

        return static_cast<int>(num_properties);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("frame_id"),
    R"doc(
    フレームのプロパティの数を取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        frame_id: フレームID

    Returns:
        int: フレームプロパティの数

    Raises:
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetJobItemFrameProperty
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetJobItemFrameProperty", [](uint64_t session_id, uint64_t item_id,
                                                    uint64_t frame_id, int property_index) -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        Validation::RequireNonNegative(property_index, "property_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        AEGP_RQM_FrameId frameid = static_cast<AEGP_RQM_FrameId>(frame_id);
        AEGP_MemHandle nameH = nullptr;
        AEGP_MemHandle valueH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemFrameProperty(
            sessid, itemid, frameid, property_index, &nameH, &valueH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetJobItemFrameProperty failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", frame_id: " + std::to_string(frame_id) +
                                   ", property_index: " + std::to_string(property_index) + ")");
        }

        std::wstring name = MemHandleToWString(nameH, suites.memorySuite);
        std::wstring value = MemHandleToWString(valueH, suites.memorySuite);

        return py::make_tuple(name, value);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("frame_id"), py::arg("property_index"),
    R"doc(
    フレームのプロパティを取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        frame_id: フレームID
        property_index: プロパティのインデックス（0ベース）

    Returns:
        tuple: (name, value) - プロパティ名と値のタプル

    Raises:
        ValueError: property_index が負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetNumJobItemOutputModuleProperties
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetNumJobItemOutputModuleProperties", [](uint64_t session_id, uint64_t item_id, int output_module_index) -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        Validation::RequireNonNegative(output_module_index, "output_module_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        A_long num_properties = 0;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemOutputModuleProperties(
            sessid, itemid, output_module_index, &num_properties);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetNumJobItemOutputModuleProperties failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", output_module_index: " + std::to_string(output_module_index) + ")");
        }

        return static_cast<int>(num_properties);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("output_module_index"),
    R"doc(
    出力モジュールのプロパティの数を取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        output_module_index: 出力モジュールのインデックス（0ベース）

    Returns:
        int: 出力モジュールプロパティの数

    Raises:
        ValueError: output_module_index が負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetJobItemOutputModuleProperty
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetJobItemOutputModuleProperty", [](uint64_t session_id, uint64_t item_id,
                                                           int output_module_index, int property_index) -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        Validation::RequireNonNegative(output_module_index, "output_module_index");
        Validation::RequireNonNegative(property_index, "property_index");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        AEGP_MemHandle nameH = nullptr;
        AEGP_MemHandle valueH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemOutputModuleProperty(
            sessid, itemid, output_module_index, property_index, &nameH, &valueH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetJobItemOutputModuleProperty failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", output_module_index: " + std::to_string(output_module_index) +
                                   ", property_index: " + std::to_string(property_index) + ")");
        }

        std::wstring name = MemHandleToWString(nameH, suites.memorySuite);
        std::wstring value = MemHandleToWString(valueH, suites.memorySuite);

        return py::make_tuple(name, value);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("output_module_index"), py::arg("property_index"),
    R"doc(
    出力モジュールのプロパティを取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        output_module_index: 出力モジュールのインデックス（0ベース）
        property_index: プロパティのインデックス（0ベース）

    Returns:
        tuple: (name, value) - プロパティ名と値のタプル

    Raises:
        ValueError: インデックスが負の場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合
    )doc");

    // -----------------------------------------------------------------------
    // AEGP_GetJobItemFrameThumbnail
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RQM_GetJobItemFrameThumbnail", [](uint64_t session_id, uint64_t item_id,
                                                     uint64_t frame_id, int width, int height) -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        Validation::RequirePositive(width, "width");
        Validation::RequirePositive(height, "height");

        AEGP_RQM_SessionId sessid = static_cast<AEGP_RQM_SessionId>(session_id);
        AEGP_RQM_ItemId itemid = static_cast<AEGP_RQM_ItemId>(item_id);
        AEGP_RQM_FrameId frameid = static_cast<AEGP_RQM_FrameId>(frame_id);
        A_long actual_width = static_cast<A_long>(width);
        A_long actual_height = static_cast<A_long>(height);
        AEGP_MemHandle thumbnailH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemFrameThumbnail(
            sessid, itemid, frameid, &actual_width, &actual_height, &thumbnailH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetJobItemFrameThumbnail failed (error code: " +
                                   std::to_string(err) + ", session_id: " + std::to_string(session_id) +
                                   ", item_id: " + std::to_string(item_id) +
                                   ", frame_id: " + std::to_string(frame_id) +
                                   ", width: " + std::to_string(width) +
                                   ", height: " + std::to_string(height) + ")");
        }

        if (!thumbnailH) {
            return py::make_tuple(static_cast<int>(actual_width), static_cast<int>(actual_height), py::bytes());
        }

        // Get JPEG data from MemHandle
        RQMScopedMemHandle scopedHandle(thumbnailH, suites.memorySuite);
        void* dataPtr = nullptr;
        err = scopedHandle.Lock(&dataPtr);
        if (err != A_Err_NONE || !dataPtr) {
            return py::make_tuple(static_cast<int>(actual_width), static_cast<int>(actual_height), py::bytes());
        }

        // Get the size of the JPEG data
        AEGP_MemSize dataSize = 0;
        err = scopedHandle.GetSize(&dataSize);
        if (err != A_Err_NONE || dataSize == 0) {
            return py::make_tuple(static_cast<int>(actual_width), static_cast<int>(actual_height), py::bytes());
        }

        // Create Python bytes object from JPEG data
        py::bytes jpeg_bytes(static_cast<const char*>(dataPtr), dataSize);

        return py::make_tuple(static_cast<int>(actual_width), static_cast<int>(actual_height), jpeg_bytes);
    }, py::arg("session_id"), py::arg("item_id"), py::arg("frame_id"), py::arg("width"), py::arg("height"),
    R"doc(
    フレームのサムネイル画像をJPEG形式で取得します。

    Args:
        session_id: セッションID
        item_id: ジョブアイテムID
        frame_id: フレームID
        width: 最大幅（実際の幅が返されます）
        height: 最大高さ（実際の高さが返されます）

    Returns:
        tuple: (actual_width, actual_height, jpeg_bytes)
            - actual_width: 実際のサムネイル幅
            - actual_height: 実際のサムネイル高さ
            - jpeg_bytes: JPEG形式の画像データ（bytes）

    Raises:
        ValueError: width または height が正でない場合
        RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

    Note:
        返されるサイズは、指定した最大サイズ以下でアスペクト比を維持したサイズです。
    )doc");
}

} // namespace SDK
} // namespace PyAE
