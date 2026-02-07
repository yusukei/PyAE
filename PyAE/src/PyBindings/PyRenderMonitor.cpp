// PyRenderMonitor.cpp
// PyAE - Python for After Effects
// High-level API for render queue monitoring with Python callback support

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "Logger.h"

#include <mutex>
#include <optional>

namespace py = pybind11;

namespace PyAE {

// =============================================================
// RenderMonitorListener - Manages Python callbacks
// =============================================================
class RenderMonitorListener {
public:
    // Callback function types
    using JobStartedCallback = std::function<void(uint64_t sessionId)>;
    using JobEndedCallback = std::function<void(uint64_t sessionId)>;
    using ItemStartedCallback = std::function<void(uint64_t sessionId, uint64_t itemId)>;
    using ItemUpdatedCallback = std::function<void(uint64_t sessionId, uint64_t itemId, uint64_t frameId)>;
    using ItemEndedCallback = std::function<void(uint64_t sessionId, uint64_t itemId, int finishedStatus)>;
    using ReportLogCallback = std::function<void(uint64_t sessionId, uint64_t itemId, bool isError, const std::string& message)>;

    static RenderMonitorListener& Instance() {
        static RenderMonitorListener instance;
        return instance;
    }

    // Set Python callbacks
    void SetJobStartedCallback(std::optional<JobStartedCallback> cb) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_onJobStarted = std::move(cb);
    }

    void SetJobEndedCallback(std::optional<JobEndedCallback> cb) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_onJobEnded = std::move(cb);
    }

    void SetItemStartedCallback(std::optional<ItemStartedCallback> cb) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_onItemStarted = std::move(cb);
    }

    void SetItemUpdatedCallback(std::optional<ItemUpdatedCallback> cb) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_onItemUpdated = std::move(cb);
    }

    void SetItemEndedCallback(std::optional<ItemEndedCallback> cb) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_onItemEnded = std::move(cb);
    }

    void SetReportLogCallback(std::optional<ReportLogCallback> cb) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_onReportLog = std::move(cb);
    }

    // Register listener with After Effects
    bool RegisterListener() {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_isRegistered) {
            return true;
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            PYAE_LOG_ERROR("RenderMonitor", "RenderQueueMonitor Suite not available");
            return false;
        }

        // Set up function block with static callbacks
        m_functionBlock.AEGP_RQM_RenderJobStarted = &OnRenderJobStarted;
        m_functionBlock.AEGP_RQM_RenderJobEnded = &OnRenderJobEnded;
        m_functionBlock.AEGP_RQM_RenderJobItemStarted = &OnRenderJobItemStarted;
        m_functionBlock.AEGP_RQM_RenderJobItemUpdated = &OnRenderJobItemUpdated;
        m_functionBlock.AEGP_RQM_RenderJobItemEnded = &OnRenderJobItemEnded;
        m_functionBlock.AEGP_RQM_RenderJobItemReportLog = &OnRenderJobItemReportLog;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_RegisterListener(
            state.GetPluginID(),
            reinterpret_cast<AEGP_RQM_Refcon>(this),
            &m_functionBlock);

        if (err != A_Err_NONE) {
            PYAE_LOG_ERROR("RenderMonitor", "Failed to register listener: " + std::to_string(err));
            return false;
        }

        m_isRegistered = true;
        PYAE_LOG_INFO("RenderMonitor", "Listener registered successfully");
        return true;
    }

    // Unregister listener
    bool UnregisterListener() {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_isRegistered) {
            return true;
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            return false;
        }

        A_Err err = suites.renderQueueMonitorSuite->AEGP_DeregisterListener(
            state.GetPluginID(),
            reinterpret_cast<AEGP_RQM_Refcon>(this));

        if (err != A_Err_NONE) {
            PYAE_LOG_ERROR("RenderMonitor", "Failed to unregister listener: " + std::to_string(err));
            return false;
        }

        m_isRegistered = false;
        PYAE_LOG_INFO("RenderMonitor", "Listener unregistered successfully");
        return true;
    }

    bool IsRegistered() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_isRegistered;
    }

private:
    RenderMonitorListener() = default;

    // Static callbacks that bridge to Python
    static A_Err OnRenderJobStarted(AEGP_RQM_BasicData* basicDataP, AEGP_RQM_SessionId jobId) {
        auto* self = reinterpret_cast<RenderMonitorListener*>(basicDataP->aegp_refconPV);
        if (!self) return A_Err_NONE;

        std::lock_guard<std::mutex> lock(self->m_mutex);
        if (self->m_onJobStarted) {
            try {
                py::gil_scoped_acquire gil;
                (*self->m_onJobStarted)(static_cast<uint64_t>(jobId));
            } catch (const py::error_already_set& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Python callback error (JobStarted): " + std::string(e.what()));
            } catch (const std::exception& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Callback error (JobStarted): " + std::string(e.what()));
            }
        }
        return A_Err_NONE;
    }

    static A_Err OnRenderJobEnded(AEGP_RQM_BasicData* basicDataP, AEGP_RQM_SessionId jobId) {
        auto* self = reinterpret_cast<RenderMonitorListener*>(basicDataP->aegp_refconPV);
        if (!self) return A_Err_NONE;

        std::lock_guard<std::mutex> lock(self->m_mutex);
        if (self->m_onJobEnded) {
            try {
                py::gil_scoped_acquire gil;
                (*self->m_onJobEnded)(static_cast<uint64_t>(jobId));
            } catch (const py::error_already_set& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Python callback error (JobEnded): " + std::string(e.what()));
            } catch (const std::exception& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Callback error (JobEnded): " + std::string(e.what()));
            }
        }
        return A_Err_NONE;
    }

    static A_Err OnRenderJobItemStarted(AEGP_RQM_BasicData* basicDataP, AEGP_RQM_SessionId jobId, AEGP_RQM_ItemId itemId) {
        auto* self = reinterpret_cast<RenderMonitorListener*>(basicDataP->aegp_refconPV);
        if (!self) return A_Err_NONE;

        std::lock_guard<std::mutex> lock(self->m_mutex);
        if (self->m_onItemStarted) {
            try {
                py::gil_scoped_acquire gil;
                (*self->m_onItemStarted)(static_cast<uint64_t>(jobId), static_cast<uint64_t>(itemId));
            } catch (const py::error_already_set& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Python callback error (ItemStarted): " + std::string(e.what()));
            } catch (const std::exception& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Callback error (ItemStarted): " + std::string(e.what()));
            }
        }
        return A_Err_NONE;
    }

    static A_Err OnRenderJobItemUpdated(AEGP_RQM_BasicData* basicDataP, AEGP_RQM_SessionId jobId,
                                         AEGP_RQM_ItemId itemId, AEGP_RQM_FrameId frameId) {
        auto* self = reinterpret_cast<RenderMonitorListener*>(basicDataP->aegp_refconPV);
        if (!self) return A_Err_NONE;

        std::lock_guard<std::mutex> lock(self->m_mutex);
        if (self->m_onItemUpdated) {
            try {
                py::gil_scoped_acquire gil;
                (*self->m_onItemUpdated)(static_cast<uint64_t>(jobId),
                                         static_cast<uint64_t>(itemId),
                                         static_cast<uint64_t>(frameId));
            } catch (const py::error_already_set& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Python callback error (ItemUpdated): " + std::string(e.what()));
            } catch (const std::exception& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Callback error (ItemUpdated): " + std::string(e.what()));
            }
        }
        return A_Err_NONE;
    }

    static A_Err OnRenderJobItemEnded(AEGP_RQM_BasicData* basicDataP, AEGP_RQM_SessionId jobId,
                                       AEGP_RQM_ItemId itemId, AEGP_RQM_FinishedStatus fstatus) {
        auto* self = reinterpret_cast<RenderMonitorListener*>(basicDataP->aegp_refconPV);
        if (!self) return A_Err_NONE;

        std::lock_guard<std::mutex> lock(self->m_mutex);
        if (self->m_onItemEnded) {
            try {
                py::gil_scoped_acquire gil;
                (*self->m_onItemEnded)(static_cast<uint64_t>(jobId),
                                       static_cast<uint64_t>(itemId),
                                       static_cast<int>(fstatus));
            } catch (const py::error_already_set& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Python callback error (ItemEnded): " + std::string(e.what()));
            } catch (const std::exception& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Callback error (ItemEnded): " + std::string(e.what()));
            }
        }
        return A_Err_NONE;
    }

    static A_Err OnRenderJobItemReportLog(AEGP_RQM_BasicData* basicDataP, AEGP_RQM_SessionId jobId,
                                           AEGP_RQM_ItemId itemId, A_Boolean isError, AEGP_MemHandle logbuf) {
        auto* self = reinterpret_cast<RenderMonitorListener*>(basicDataP->aegp_refconPV);
        if (!self) return A_Err_NONE;

        std::lock_guard<std::mutex> lock(self->m_mutex);
        if (self->m_onReportLog && logbuf) {
            try {
                // Extract log message from MemHandle
                auto& state = PluginState::Instance();
                const auto& suites = state.GetSuites();
                std::string message;

                if (suites.memorySuite) {
                    void* ptr = nullptr;
                    if (suites.memorySuite->AEGP_LockMemHandle(logbuf, &ptr) == A_Err_NONE && ptr) {
                        message = StringUtils::Utf16ToUtf8(static_cast<A_UTF16Char*>(ptr));
                        suites.memorySuite->AEGP_UnlockMemHandle(logbuf);
                    }
                }

                py::gil_scoped_acquire gil;
                (*self->m_onReportLog)(static_cast<uint64_t>(jobId),
                                       static_cast<uint64_t>(itemId),
                                       isError != FALSE,
                                       message);
            } catch (const py::error_already_set& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Python callback error (ReportLog): " + std::string(e.what()));
            } catch (const std::exception& e) {
                PYAE_LOG_ERROR("RenderMonitor", "Callback error (ReportLog): " + std::string(e.what()));
            }
        }
        return A_Err_NONE;
    }

    mutable std::mutex m_mutex;
    bool m_isRegistered = false;
    AEGP_RQM_FunctionBlock1 m_functionBlock = {};

    // Python callbacks
    std::optional<JobStartedCallback> m_onJobStarted;
    std::optional<JobEndedCallback> m_onJobEnded;
    std::optional<ItemStartedCallback> m_onItemStarted;
    std::optional<ItemUpdatedCallback> m_onItemUpdated;
    std::optional<ItemEndedCallback> m_onItemEnded;
    std::optional<ReportLogCallback> m_onReportLog;
};

// =============================================================
// RenderMonitor - Singleton for render queue monitoring
// =============================================================
class RenderMonitor {
public:
    static RenderMonitor& Instance() {
        static RenderMonitor instance;
        return instance;
    }

    // Get project name
    std::string GetProjectName(uint64_t sessionId) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite || !suites.memorySuite) {
            throw std::runtime_error("Required suites not available");
        }

        AEGP_MemHandle nameH = nullptr;
        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetProjectName(
            static_cast<AEGP_RQM_SessionId>(sessionId), &nameH);
        if (err != A_Err_NONE || !nameH) {
            throw std::runtime_error("Failed to get project name");
        }

        ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);

        void* ptr = nullptr;
        err = suites.memorySuite->AEGP_LockMemHandle(nameH, &ptr);
        if (err != A_Err_NONE || !ptr) {
            throw std::runtime_error("Failed to lock project name");
        }

        std::string result = StringUtils::Utf16ToUtf8(static_cast<A_UTF16Char*>(ptr));
        suites.memorySuite->AEGP_UnlockMemHandle(nameH);

        return result;
    }

    // Get app version
    std::string GetAppVersion(uint64_t sessionId) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite || !suites.memorySuite) {
            throw std::runtime_error("Required suites not available");
        }

        AEGP_MemHandle versionH = nullptr;
        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetAppVersion(
            static_cast<AEGP_RQM_SessionId>(sessionId), &versionH);
        if (err != A_Err_NONE || !versionH) {
            throw std::runtime_error("Failed to get app version");
        }

        ScopedMemHandle scopedVersion(state.GetPluginID(), suites.memorySuite, versionH);

        void* ptr = nullptr;
        err = suites.memorySuite->AEGP_LockMemHandle(versionH, &ptr);
        if (err != A_Err_NONE || !ptr) {
            throw std::runtime_error("Failed to lock app version");
        }

        std::string result = StringUtils::Utf16ToUtf8(static_cast<A_UTF16Char*>(ptr));
        suites.memorySuite->AEGP_UnlockMemHandle(versionH);

        return result;
    }

    // Get number of job items
    int GetNumJobItems(uint64_t sessionId) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        A_long numItems = 0;
        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItems(
            static_cast<AEGP_RQM_SessionId>(sessionId), &numItems);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get number of job items");
        }

        return static_cast<int>(numItems);
    }

    // Get job item ID
    uint64_t GetJobItemId(uint64_t sessionId, int index) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        AEGP_RQM_ItemId itemId = 0;
        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemID(
            static_cast<AEGP_RQM_SessionId>(sessionId),
            static_cast<A_long>(index),
            &itemId);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get job item ID");
        }

        return static_cast<uint64_t>(itemId);
    }

    // Get render settings for a job item
    std::vector<std::pair<std::string, std::string>> GetRenderSettings(
        uint64_t sessionId, uint64_t itemId) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        std::vector<std::pair<std::string, std::string>> result;

        if (!suites.renderQueueMonitorSuite || !suites.memorySuite) {
            throw std::runtime_error("Required suites not available");
        }

        A_long numSettings = 0;
        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemRenderSettings(
            static_cast<AEGP_RQM_SessionId>(sessionId),
            static_cast<AEGP_RQM_ItemId>(itemId),
            &numSettings);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get number of render settings");
        }

        for (A_long i = 0; i < numSettings; ++i) {
            AEGP_MemHandle nameH = nullptr;
            AEGP_MemHandle valueH = nullptr;

            err = suites.renderQueueMonitorSuite->AEGP_GetJobItemRenderSetting(
                static_cast<AEGP_RQM_SessionId>(sessionId),
                static_cast<AEGP_RQM_ItemId>(itemId),
                i, &nameH, &valueH);

            if (err != A_Err_NONE) continue;

            std::string name, value;

            if (nameH) {
                ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);
                void* ptr = nullptr;
                if (suites.memorySuite->AEGP_LockMemHandle(nameH, &ptr) == A_Err_NONE && ptr) {
                    name = StringUtils::Utf16ToUtf8(static_cast<A_UTF16Char*>(ptr));
                    suites.memorySuite->AEGP_UnlockMemHandle(nameH);
                }
            }

            if (valueH) {
                ScopedMemHandle scopedValue(state.GetPluginID(), suites.memorySuite, valueH);
                void* ptr = nullptr;
                if (suites.memorySuite->AEGP_LockMemHandle(valueH, &ptr) == A_Err_NONE && ptr) {
                    value = StringUtils::Utf16ToUtf8(static_cast<A_UTF16Char*>(ptr));
                    suites.memorySuite->AEGP_UnlockMemHandle(valueH);
                }
            }

            result.push_back({name, value});
        }

        return result;
    }

    // Get number of output modules
    int GetNumOutputModules(uint64_t sessionId, uint64_t itemId) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite) {
            throw std::runtime_error("RenderQueueMonitor Suite not available");
        }

        A_long numModules = 0;
        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemOutputModules(
            static_cast<AEGP_RQM_SessionId>(sessionId),
            static_cast<AEGP_RQM_ItemId>(itemId),
            &numModules);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get number of output modules");
        }

        return static_cast<int>(numModules);
    }

    // Get output module settings
    std::vector<std::pair<std::string, std::string>> GetOutputModuleSettings(
        uint64_t sessionId, uint64_t itemId, int omIndex) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        std::vector<std::pair<std::string, std::string>> result;

        if (!suites.renderQueueMonitorSuite || !suites.memorySuite) {
            throw std::runtime_error("Required suites not available");
        }

        A_long numSettings = 0;
        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetNumJobItemOutputModuleSettings(
            static_cast<AEGP_RQM_SessionId>(sessionId),
            static_cast<AEGP_RQM_ItemId>(itemId),
            static_cast<A_long>(omIndex),
            &numSettings);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get number of output module settings");
        }

        for (A_long i = 0; i < numSettings; ++i) {
            AEGP_MemHandle nameH = nullptr;
            AEGP_MemHandle valueH = nullptr;

            err = suites.renderQueueMonitorSuite->AEGP_GetJobItemOutputModuleSetting(
                static_cast<AEGP_RQM_SessionId>(sessionId),
                static_cast<AEGP_RQM_ItemId>(itemId),
                static_cast<A_long>(omIndex),
                i, &nameH, &valueH);

            if (err != A_Err_NONE) continue;

            std::string name, value;

            if (nameH) {
                ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);
                void* ptr = nullptr;
                if (suites.memorySuite->AEGP_LockMemHandle(nameH, &ptr) == A_Err_NONE && ptr) {
                    name = StringUtils::Utf16ToUtf8(static_cast<A_UTF16Char*>(ptr));
                    suites.memorySuite->AEGP_UnlockMemHandle(nameH);
                }
            }

            if (valueH) {
                ScopedMemHandle scopedValue(state.GetPluginID(), suites.memorySuite, valueH);
                void* ptr = nullptr;
                if (suites.memorySuite->AEGP_LockMemHandle(valueH, &ptr) == A_Err_NONE && ptr) {
                    value = StringUtils::Utf16ToUtf8(static_cast<A_UTF16Char*>(ptr));
                    suites.memorySuite->AEGP_UnlockMemHandle(valueH);
                }
            }

            result.push_back({name, value});
        }

        return result;
    }

    // Get frame thumbnail
    py::tuple GetThumbnail(uint64_t sessionId, uint64_t itemId, uint64_t frameId,
                          int maxWidth, int maxHeight) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueMonitorSuite || !suites.memorySuite) {
            throw std::runtime_error("Required suites not available");
        }

        // Width and height are in/out parameters: pass max, get actual
        A_long width = static_cast<A_long>(maxWidth);
        A_long height = static_cast<A_long>(maxHeight);
        AEGP_MemHandle jpegH = nullptr;

        A_Err err = suites.renderQueueMonitorSuite->AEGP_GetJobItemFrameThumbnail(
            static_cast<AEGP_RQM_SessionId>(sessionId),
            static_cast<AEGP_RQM_ItemId>(itemId),
            static_cast<AEGP_RQM_FrameId>(frameId),
            &width,
            &height,
            &jpegH);

        if (err != A_Err_NONE || !jpegH) {
            throw std::runtime_error("Failed to get frame thumbnail");
        }

        ScopedMemHandle scopedJpeg(state.GetPluginID(), suites.memorySuite, jpegH);

        void* ptr = nullptr;
        AEGP_MemSize size = 0;
        err = suites.memorySuite->AEGP_LockMemHandle(jpegH, &ptr);
        if (err != A_Err_NONE || !ptr) {
            throw std::runtime_error("Failed to lock thumbnail data");
        }

        err = suites.memorySuite->AEGP_GetMemHandleSize(jpegH, &size);
        if (err != A_Err_NONE) {
            suites.memorySuite->AEGP_UnlockMemHandle(jpegH);
            throw std::runtime_error("Failed to get thumbnail size");
        }

        py::bytes jpegBytes(static_cast<const char*>(ptr), size);
        suites.memorySuite->AEGP_UnlockMemHandle(jpegH);

        return py::make_tuple(width, height, jpegBytes);
    }

private:
    RenderMonitor() = default;
};

} // namespace PyAE

// =============================================================
// Python bindings
// =============================================================
void init_render_monitor(py::module_& m) {
    // Create submodule
    py::module_ rm = m.def_submodule("render_monitor", "Render queue monitoring");

    // Finished status constants
    rm.attr("STATUS_UNKNOWN") = 0;
    rm.attr("STATUS_SUCCEEDED") = 1;
    rm.attr("STATUS_ABORTED") = 2;
    rm.attr("STATUS_ERRED") = 3;

    // Project info
    rm.def("get_project_name", [](uint64_t sessionId) {
        return PyAE::RenderMonitor::Instance().GetProjectName(sessionId);
    }, "Get the project name for a render session",
       py::arg("session_id") = 0);

    rm.def("get_app_version", [](uint64_t sessionId) {
        return PyAE::RenderMonitor::Instance().GetAppVersion(sessionId);
    }, "Get the After Effects version",
       py::arg("session_id") = 0);

    // Job items
    rm.def("get_num_job_items", [](uint64_t sessionId) {
        return PyAE::RenderMonitor::Instance().GetNumJobItems(sessionId);
    }, "Get the number of job items in the render queue",
       py::arg("session_id") = 0);

    rm.def("get_job_item_id", [](uint64_t sessionId, int index) {
        return PyAE::RenderMonitor::Instance().GetJobItemId(sessionId, index);
    }, "Get a job item ID by index",
       py::arg("session_id") = 0, py::arg("index") = 0);

    // Render settings
    rm.def("get_render_settings", [](uint64_t sessionId, uint64_t itemId) {
        return PyAE::RenderMonitor::Instance().GetRenderSettings(sessionId, itemId);
    }, "Get render settings for a job item (returns list of (name, value) tuples)",
       py::arg("session_id") = 0, py::arg("item_id") = 0);

    // Output modules
    rm.def("get_num_output_modules", [](uint64_t sessionId, uint64_t itemId) {
        return PyAE::RenderMonitor::Instance().GetNumOutputModules(sessionId, itemId);
    }, "Get the number of output modules for a job item",
       py::arg("session_id") = 0, py::arg("item_id") = 0);

    rm.def("get_output_module_settings", [](uint64_t sessionId, uint64_t itemId, int omIndex) {
        return PyAE::RenderMonitor::Instance().GetOutputModuleSettings(sessionId, itemId, omIndex);
    }, "Get output module settings (returns list of (name, value) tuples)",
       py::arg("session_id") = 0, py::arg("item_id") = 0, py::arg("om_index") = 0);

    // Thumbnail
    rm.def("get_thumbnail", [](uint64_t sessionId, uint64_t itemId, uint64_t frameId,
                               int maxWidth, int maxHeight) {
        return PyAE::RenderMonitor::Instance().GetThumbnail(
            sessionId, itemId, frameId, maxWidth, maxHeight);
    }, "Get frame thumbnail as JPEG (returns (width, height, jpeg_bytes))",
       py::arg("session_id") = 0, py::arg("item_id") = 0, py::arg("frame_id") = 0,
       py::arg("max_width") = 320, py::arg("max_height") = 240);

    // =============================================================
    // Listener registration and callbacks
    // =============================================================

    rm.def("register_listener", []() {
        return PyAE::RenderMonitorListener::Instance().RegisterListener();
    }, R"doc(
Register a render queue monitor listener.

This must be called before any callbacks will be invoked.
Call unregister_listener() when done to clean up.

Returns:
    bool: True if registration succeeded, False otherwise.
)doc");

    rm.def("unregister_listener", []() {
        return PyAE::RenderMonitorListener::Instance().UnregisterListener();
    }, "Unregister the render queue monitor listener");

    rm.def("is_listener_registered", []() {
        return PyAE::RenderMonitorListener::Instance().IsRegistered();
    }, "Check if the listener is currently registered");

    // Callback setters
    rm.def("set_on_job_started", [](std::optional<std::function<void(uint64_t)>> callback) {
        PyAE::RenderMonitorListener::Instance().SetJobStartedCallback(std::move(callback));
    }, R"doc(
Set callback for when a render job starts.

Args:
    callback: Function taking (session_id: int). Pass None to clear.

Example:
    def on_job_started(session_id):
        print(f"Render job started: {session_id}")

    ae.render_monitor.set_on_job_started(on_job_started)
)doc", py::arg("callback"));

    rm.def("set_on_job_ended", [](std::optional<std::function<void(uint64_t)>> callback) {
        PyAE::RenderMonitorListener::Instance().SetJobEndedCallback(std::move(callback));
    }, R"doc(
Set callback for when a render job ends.

Args:
    callback: Function taking (session_id: int). Pass None to clear.
)doc", py::arg("callback"));

    rm.def("set_on_item_started", [](std::optional<std::function<void(uint64_t, uint64_t)>> callback) {
        PyAE::RenderMonitorListener::Instance().SetItemStartedCallback(std::move(callback));
    }, R"doc(
Set callback for when a render item starts.

Args:
    callback: Function taking (session_id: int, item_id: int). Pass None to clear.
)doc", py::arg("callback"));

    rm.def("set_on_item_updated", [](std::optional<std::function<void(uint64_t, uint64_t, uint64_t)>> callback) {
        PyAE::RenderMonitorListener::Instance().SetItemUpdatedCallback(std::move(callback));
    }, R"doc(
Set callback for when a render item frame is updated.

Args:
    callback: Function taking (session_id: int, item_id: int, frame_id: int). Pass None to clear.

Note: This callback may be called frequently during rendering.
)doc", py::arg("callback"));

    rm.def("set_on_item_ended", [](std::optional<std::function<void(uint64_t, uint64_t, int)>> callback) {
        PyAE::RenderMonitorListener::Instance().SetItemEndedCallback(std::move(callback));
    }, R"doc(
Set callback for when a render item ends.

Args:
    callback: Function taking (session_id: int, item_id: int, finished_status: int). Pass None to clear.

finished_status values:
    - STATUS_UNKNOWN (0)
    - STATUS_SUCCEEDED (1)
    - STATUS_ABORTED (2)
    - STATUS_ERRED (3)
)doc", py::arg("callback"));

    rm.def("set_on_report_log", [](std::optional<std::function<void(uint64_t, uint64_t, bool, const std::string&)>> callback) {
        PyAE::RenderMonitorListener::Instance().SetReportLogCallback(std::move(callback));
    }, R"doc(
Set callback for render log messages.

Args:
    callback: Function taking (session_id: int, item_id: int, is_error: bool, message: str). Pass None to clear.

Example:
    def on_log(session_id, item_id, is_error, message):
        prefix = "ERROR" if is_error else "INFO"
        print(f"[{prefix}] {message}")

    ae.render_monitor.set_on_report_log(on_log)
)doc", py::arg("callback"));

    // Documentation
    rm.attr("__doc__") = R"doc(
Render Queue Monitor API

This module provides monitoring of After Effects render queue with callback support.

Query Functions:
    get_project_name(session_id) - Get project name for a render session
    get_app_version(session_id) - Get After Effects version
    get_num_job_items(session_id) - Get number of render items
    get_job_item_id(session_id, index) - Get item ID by index
    get_render_settings(session_id, item_id) - Get render settings
    get_num_output_modules(session_id, item_id) - Get output module count
    get_output_module_settings(session_id, item_id, om_index) - Get output module settings
    get_thumbnail(session_id, item_id, frame_id, max_width, max_height) - Get frame thumbnail

Listener Functions:
    register_listener() - Start receiving callbacks
    unregister_listener() - Stop receiving callbacks
    is_listener_registered() - Check if listener is active

Callback Setters:
    set_on_job_started(callback) - Called when render job starts
    set_on_job_ended(callback) - Called when render job ends
    set_on_item_started(callback) - Called when item render starts
    set_on_item_updated(callback) - Called when frame is rendered
    set_on_item_ended(callback) - Called when item render ends
    set_on_report_log(callback) - Called for log messages

Example:
    import ae

    # Set up callbacks
    def on_started(session_id):
        print(f"Render started: {session_id}")

    def on_progress(session_id, item_id, frame_id):
        print(f"Frame {frame_id} rendered")

    def on_ended(session_id, item_id, status):
        status_names = ["Unknown", "Succeeded", "Aborted", "Error"]
        print(f"Item {item_id} finished: {status_names[status]}")

    ae.render_monitor.set_on_job_started(on_started)
    ae.render_monitor.set_on_item_updated(on_progress)
    ae.render_monitor.set_on_item_ended(on_ended)

    # Register to start receiving callbacks
    ae.render_monitor.register_listener()

    # ... render queue runs ...

    # Clean up when done
    ae.render_monitor.unregister_listener()
)doc";
}
