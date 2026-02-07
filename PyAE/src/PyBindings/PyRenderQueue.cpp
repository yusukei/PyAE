// PyRenderQueue.cpp
// PyAE - Python for After Effects
// レンダーキュー操作APIバインディング (Phase 5)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PluginState.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "Logger.h"
#include "IdleHandler.h"
#include "PyRefTypes.h"

namespace py = pybind11;

namespace PyAE {

// レンダーキューアイテムの状態
enum class RenderStatus {
    WillContinue,
    NeedsOutput,
    Queued,
    Rendering,
    UserStopped,
    ErrStopped,
    Done,
    Unqueued
};

// 埋め込みオプション
enum class EmbedOptions : int {
    None = 0,
    Link = 1,
    All = 2
};

// 前方宣言
class PyOutputModule;
class PyRenderQueueItem;

// =============================================================
// PyOutputModule - 出力モジュールクラス
// =============================================================
// IMPORTANT: SDK documentation states:
// "All AEGP_OutputModuleRefHs are invalidated by ANY re-ordering, addition or removal
// of output modules. DO NOT CACHE THEM."
// Therefore, we store the index and RQItemH, and get a fresh handle on each API call.
class PyOutputModule {
public:
    PyOutputModule() : m_index(-1), m_rqItemH(nullptr) {}

    // Constructor takes index (0-based from Python) and RQItemH
    PyOutputModule(int index, AEGP_RQItemRefH rqItemH)
        : m_index(index)
        , m_rqItemH(rqItemH)
    {}

    ~PyOutputModule() {
        // No handle to release - we get fresh handles on each call
    }

    bool IsValid() const { return m_index >= 0 && m_rqItemH != nullptr; }

    // ==========================================
    // Helper: Get fresh OutputModule handle
    // ==========================================
    // IMPORTANT: AEGP_GetOutputModuleByIndex uses 0-based indexing!
    // This is an EXCEPTION in the AEGP SDK - most other functions use 1-based indexing.
    AEGP_OutputModuleRefH GetFreshHandle() const {
        if (!IsValid()) return nullptr;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.outputModuleSuite) return nullptr;

        AEGP_OutputModuleRefH omH = nullptr;
        // AEGP_GetOutputModuleByIndex uses 0-based indexing (SDK exception!)
        A_Err err = suites.outputModuleSuite->AEGP_GetOutputModuleByIndex(
            m_rqItemH, static_cast<A_long>(m_index), &omH);
        if (err != A_Err_NONE) return nullptr;

        return omH;
    }

    // ==========================================
    // Helper: Get fresh OutputModule handle with success status
    // ==========================================
    // Returns true if handle was successfully obtained (even if omH == 0)
    // Returns false if there was an error
    // IMPORTANT: omH = 0 is a VALID handle (first output module), not null!
    bool TryGetFreshHandle(AEGP_OutputModuleRefH& outHandle) const {
        outHandle = nullptr;

        if (!IsValid()) return false;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.outputModuleSuite) return false;

        // AEGP_GetOutputModuleByIndex uses 0-based indexing (SDK exception!)
        A_Err err = suites.outputModuleSuite->AEGP_GetOutputModuleByIndex(
            m_rqItemH, static_cast<A_long>(m_index), &outHandle);

        // Return true only if SDK returned success
        // Note: outHandle may be 0, which is a valid handle!
        return (err == A_Err_NONE);
    }

    // ==========================================
    // ファイルパス
    // ==========================================
    // IMPORTANT: We must use consistent handle pairing:
    // - (0, 0) pattern: Both handles are 0 (Queuebert legacy pattern)
    // - (rq_itemH, omH) pattern: Both handles are valid (recommended)
    // Mixing (0, omH) causes AE internal validation error {39} (5027, 204)
    std::string GetFilePath() const {
        AEGP_OutputModuleRefH omH;
        if (!TryGetFreshHandle(omH)) return "";  // Failed to get handle

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        // Note: suites check already done in TryGetFreshHandle

        AEGP_MemHandle pathH;
        // Use consistent handle pairing: both rq_itemH and omH must match
        A_Err err = suites.outputModuleSuite->AEGP_GetOutputFilePath(
            m_rqItemH, omH, &pathH);
        if (err != A_Err_NONE || !pathH) return "";

        ScopedMemHandle scopedPath(state.GetPluginID(), suites.memorySuite, pathH);

        ScopedMemLock lock(suites.memorySuite, pathH);
        A_UTF16Char* pathPtr = lock.As<A_UTF16Char>();
        if (!pathPtr) return "";

        std::string result = StringUtils::Utf16ToUtf8(pathPtr);

        return result;
    }

    void SetFilePath(const std::string& path) {
        AEGP_OutputModuleRefH omH;
        if (!TryGetFreshHandle(omH)) {
            throw std::runtime_error("Invalid output module");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        // Note: suites check already done in TryGetFreshHandle

        std::wstring wpath = StringUtils::Utf8ToWide(path);

        // Use consistent handle pairing: both rq_itemH and omH must match
        A_Err err = suites.outputModuleSuite->AEGP_SetOutputFilePath(
            m_rqItemH, omH,
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str()));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set output file path");
        }
    }

    // ==========================================
    // 埋め込みオプション
    // ==========================================
    // IMPORTANT: We must use consistent handle pairing:
    // - (0, 0) pattern: Both handles are 0 (Queuebert legacy pattern)
    // - (rq_itemH, omH) pattern: Both handles are valid (recommended)
    int GetEmbedOptions() const {
        AEGP_OutputModuleRefH omH;
        if (!TryGetFreshHandle(omH)) return 0;  // Failed to get handle

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        // Note: suites check already done in TryGetFreshHandle

        AEGP_EmbeddingType embedType;
        // Use consistent handle pairing: both rq_itemH and omH must match
        A_Err err = suites.outputModuleSuite->AEGP_GetEmbedOptions(
            m_rqItemH, omH, &embedType);
        if (err != A_Err_NONE) return 0;

        return static_cast<int>(embedType);
    }

    void SetEmbedOptions(int embedType) {
        AEGP_OutputModuleRefH omH;
        if (!TryGetFreshHandle(omH)) {
            throw std::runtime_error("Invalid output module");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        // Note: suites check already done in TryGetFreshHandle

        // Use consistent handle pairing: both rq_itemH and omH must match
        A_Err err = suites.outputModuleSuite->AEGP_SetEmbedOptions(
            m_rqItemH, omH, static_cast<AEGP_EmbeddingType>(embedType));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set embed options");
        }
    }

    // Get handle - returns fresh handle (may be null if invalid)
    AEGP_OutputModuleRefH GetHandle() const { return GetFreshHandle(); }

    // Get index (0-based)
    int GetIndex() const { return m_index; }

private:
    int m_index;                    // 0-based index (stored, not cached)
    AEGP_RQItemRefH m_rqItemH;      // Parent RQ item handle
};

// =============================================================
// PyRenderQueueItem - レンダーキューアイテムクラス
// =============================================================
class PyRenderQueueItem {
public:
    PyRenderQueueItem() : m_itemH(nullptr) {}

    explicit PyRenderQueueItem(AEGP_RQItemRefH itemH)
        : m_itemH(itemH)
    {}

    bool IsValid() const { return m_itemH != nullptr; }

    // ==========================================
    // 状態
    // ==========================================
    RenderStatus GetStatus() const {
        if (!m_itemH) return RenderStatus::Unqueued;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.rqItemSuite) return RenderStatus::Unqueued;

        AEGP_RenderItemStatusType status;
        A_Err err = suites.rqItemSuite->AEGP_GetRenderState(m_itemH, &status);
        if (err != A_Err_NONE) return RenderStatus::Unqueued;

        switch (status) {
            case AEGP_RenderItemStatus_WILL_CONTINUE: return RenderStatus::WillContinue;
            case AEGP_RenderItemStatus_NEEDS_OUTPUT:  return RenderStatus::NeedsOutput;
            case AEGP_RenderItemStatus_QUEUED:        return RenderStatus::Queued;
            case AEGP_RenderItemStatus_RENDERING:     return RenderStatus::Rendering;
            case AEGP_RenderItemStatus_USER_STOPPED:  return RenderStatus::UserStopped;
            case AEGP_RenderItemStatus_ERR_STOPPED:   return RenderStatus::ErrStopped;
            case AEGP_RenderItemStatus_DONE:          return RenderStatus::Done;
            case AEGP_RenderItemStatus_UNQUEUED:      return RenderStatus::Unqueued;
            default: return RenderStatus::Unqueued;
        }
    }

    void SetQueued(bool queued) {
        if (!m_itemH) {
            throw std::runtime_error("Invalid render queue item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.rqItemSuite) {
            throw std::runtime_error("RQItemSuite not available");
        }

        A_Err err = suites.rqItemSuite->AEGP_SetRenderState(
            m_itemH,
            queued ? AEGP_RenderItemStatus_QUEUED : AEGP_RenderItemStatus_UNQUEUED);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set render queue item status");
        }
    }

    // ==========================================
    // コンポジション参照
    // ==========================================
    std::string GetCompName() const {
        if (!m_itemH) return "";

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.rqItemSuite) return "";

        AEGP_CompH compH;
        A_Err err = suites.rqItemSuite->AEGP_GetCompFromRQItem(m_itemH, &compH);
        if (err != A_Err_NONE || !compH) return "";

        AEGP_ItemH itemH;
        err = suites.compSuite->AEGP_GetItemFromComp(compH, &itemH);
        if (err != A_Err_NONE || !itemH) return "";

        AEGP_MemHandle nameH;
        err = suites.itemSuite->AEGP_GetItemName(state.GetPluginID(), itemH, &nameH);
        if (err != A_Err_NONE || !nameH) return "";

        ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);

        ScopedMemLock lock(suites.memorySuite, nameH);
        A_UTF16Char* namePtr = lock.As<A_UTF16Char>();
        if (!namePtr) return "";

        std::string result = StringUtils::Utf16ToUtf8(namePtr);

        return result;
    }

    // ==========================================
    // 出力モジュール
    // ==========================================
    int GetNumOutputModules() const {
        if (!m_itemH) return 0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.rqItemSuite) return 0;

        A_long numOM;
        A_Err err = suites.rqItemSuite->AEGP_GetNumOutputModulesForRQItem(m_itemH, &numOM);
        if (err != A_Err_NONE) return 0;

        return static_cast<int>(numOM);
    }

    std::shared_ptr<PyOutputModule> GetOutputModule(int index) {
        if (!m_itemH) {
            throw std::runtime_error("Invalid render queue item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.outputModuleSuite) {
            throw std::runtime_error("OutputModuleSuite not available");
        }

        // Validate index by attempting to get handle (but don't cache it)
        AEGP_OutputModuleRefH omH = nullptr;
        // IMPORTANT: AEGP_GetOutputModuleByIndex uses 0-based indexing (SDK exception!)
        // NOTE: omH = 0 is a VALID handle (first output module), not null!
        A_Err err = suites.outputModuleSuite->AEGP_GetOutputModuleByIndex(
            m_itemH, static_cast<A_long>(index), &omH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get output module (error code: " +
                                   std::to_string(err) + ", index: " + std::to_string(index) + ")");
        }

        // Create PyOutputModule with index and RQItemH (not caching the handle)
        return std::make_shared<PyOutputModule>(index, m_itemH);
    }

    // ==========================================
    // 削除
    // ==========================================
    void Delete() {
        if (!m_itemH) {
            throw std::runtime_error("Invalid render queue item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.rqItemSuite) {
            throw std::runtime_error("RQItemSuite not available");
        }

        A_Err err = suites.rqItemSuite->AEGP_DeleteRQItem(m_itemH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to delete render queue item");
        }

        m_itemH = nullptr;
    }

    AEGP_RQItemRefH GetHandle() const { return m_itemH; }

private:
    AEGP_RQItemRefH m_itemH;
};

// =============================================================
// PyRenderQueue - レンダーキュークラス
// =============================================================
class PyRenderQueue {
public:
    static PyRenderQueue& Instance() {
        static PyRenderQueue instance;
        return instance;
    }

    // ==========================================
    // アイテム操作
    // ==========================================
    int GetNumItems() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.rqItemSuite) return 0;

        A_long numItems;
        A_Err err = suites.rqItemSuite->AEGP_GetNumRQItems(&numItems);
        if (err != A_Err_NONE) return 0;

        return static_cast<int>(numItems);
    }

    std::shared_ptr<PyRenderQueueItem> GetItem(int index) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.rqItemSuite) {
            throw std::runtime_error("RQItemSuite not available");
        }

        char logBuf[256];
        snprintf(logBuf, sizeof(logBuf), "GetItem called with index: %d", index);
        PYAE_LOG_DEBUG("RenderQueue", logBuf);

        AEGP_RQItemRefH itemH;
        A_Err err = suites.rqItemSuite->AEGP_GetRQItemByIndex(
            static_cast<A_long>(index), &itemH);
        
        if (err != A_Err_NONE || !itemH) {
            snprintf(logBuf, sizeof(logBuf), "AEGP_GetRQItemByIndex failed: err=%d, itemH=%p", err, itemH);
            PYAE_LOG_ERROR("RenderQueue", logBuf);
            throw std::runtime_error("Failed to get render queue item");
        }

        PYAE_LOG_DEBUG("RenderQueue", "GetItem succeeded");
        return std::make_shared<PyRenderQueueItem>(itemH);
    }

    std::vector<std::shared_ptr<PyRenderQueueItem>> GetAllItems() {
        std::vector<std::shared_ptr<PyRenderQueueItem>> items;

        int numItems = GetNumItems();
        for (int i = 0; i < numItems; ++i) {
            try {
                items.push_back(GetItem(i));
            } catch (...) {
                // アイテムの取得に失敗した場合はスキップ
            }
        }

        return items;
    }

    std::shared_ptr<PyRenderQueueItem> AddComp(AEGP_CompH compH, const std::string& path = "") {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.renderQueueSuite) {
            PYAE_LOG_ERROR("RenderQueue", "RenderQueueSuite is null");
            throw std::runtime_error("RenderQueueSuite not available");
        }

        // デバッグ: コンポの有効性チェック
        if (compH) {
            A_long width = 0, height = 0;
            AEGP_ItemH itemH = nullptr;
            suites.compSuite->AEGP_GetItemFromComp(compH, &itemH);
            if (itemH) {
                suites.itemSuite->AEGP_GetItemDimensions(itemH, &width, &height);
                // std::string + string format は Loggerの実装によるが、
                // ポインタ値を直接埋め込む
                char buf[256];
                snprintf(buf, sizeof(buf), "Comp valid. Size: %d x %d, Handle: %p", width, height, compH);
                PYAE_LOG_DEBUG("RenderQueue", buf);
            } else {
                PYAE_LOG_ERROR("RenderQueue", "CompH has no ItemH");
            }
        }

        const A_char* pathPtr = nullptr;
        std::string safePath;
        if (!path.empty()) {
            safePath = path;
            pathPtr = safePath.c_str();
        } else {
            // クラッシュ回避のためにデフォルトパスを指定してみる
            safePath = "Comp_Render";
            pathPtr = safePath.c_str();
        }
        
        char logBuf[256];
        snprintf(logBuf, sizeof(logBuf), "Calling AEGP_AddCompToRenderQueue with path: %s", pathPtr ? pathPtr : "NULL");
        PYAE_LOG_DEBUG("RenderQueue", logBuf);        
        // AddComp中にIdleHandlerが走ると再入/競合の可能性があるため一時停止
        PyAE::IdleHandler::Instance().SetSuspended(true);
        A_Err err = suites.renderQueueSuite->AEGP_AddCompToRenderQueue(compH, pathPtr);
        PyAE::IdleHandler::Instance().SetSuspended(false);

        if (err != A_Err_NONE) {
            snprintf(logBuf, sizeof(logBuf), "AEGP_AddCompToRenderQueue failed: %d", err);
            PYAE_LOG_ERROR("RenderQueue", logBuf);
            throw std::runtime_error("Failed to add comp to render queue");
        }
        PYAE_LOG_DEBUG("RenderQueue", "AEGP_AddCompToRenderQueue success");

        // 追加されたアイテムを取得（最後のアイテム）
        int numItems = GetNumItems();
        snprintf(logBuf, sizeof(logBuf), "NumItems after add: %d", numItems);
        PYAE_LOG_DEBUG("RenderQueue", logBuf);
        
        if (numItems > 0) {
            auto item = GetItem(numItems - 1);
            if (item) {
                PYAE_LOG_DEBUG("RenderQueue", "Successfully retrieved item");
                return item;
            } else {
                PYAE_LOG_ERROR("RenderQueue", "GetItem returned nullptr");
            }
        } else {
            PYAE_LOG_ERROR("RenderQueue", "No items in queue after add");
        }

        return nullptr;
    }

private:
    PyRenderQueue() = default;
};

} // namespace PyAE

void init_render_queue(py::module_& m) {
    // レンダー状態列挙
    py::enum_<PyAE::RenderStatus>(m, "RenderStatus")
        .value("WillContinue", PyAE::RenderStatus::WillContinue)
        .value("NeedsOutput", PyAE::RenderStatus::NeedsOutput)
        .value("Queued", PyAE::RenderStatus::Queued)
        .value("Rendering", PyAE::RenderStatus::Rendering)
        .value("UserStopped", PyAE::RenderStatus::UserStopped)
        .value("ErrStopped", PyAE::RenderStatus::ErrStopped)
        .value("Done", PyAE::RenderStatus::Done)
        .value("Unqueued", PyAE::RenderStatus::Unqueued);

    // 埋め込みオプション列挙
    py::enum_<PyAE::EmbedOptions>(m, "EmbedOptions", py::arithmetic())
        .value("None_", PyAE::EmbedOptions::None)
        .value("Link", PyAE::EmbedOptions::Link)
        .value("All", PyAE::EmbedOptions::All);

    // 出力モジュールクラス
    py::class_<PyAE::PyOutputModule, std::shared_ptr<PyAE::PyOutputModule>>(m, "OutputModule")
        .def_property_readonly("valid", &PyAE::PyOutputModule::IsValid,
                              "Check if output module is valid")
        .def_property("file_path", &PyAE::PyOutputModule::GetFilePath, &PyAE::PyOutputModule::SetFilePath,
                     "Output file path")
        .def_property("embed_options",
            [](const PyAE::PyOutputModule& self) { return static_cast<PyAE::EmbedOptions>(self.GetEmbedOptions()); },
            [](PyAE::PyOutputModule& self, int val) { self.SetEmbedOptions(val); },
            "Embed options")
        .def("__repr__", [](const PyAE::PyOutputModule& self) {
            if (!self.IsValid()) return std::string("<OutputModule: invalid>");
            std::string path = self.GetFilePath();
            return std::string("<OutputModule: '") + path + "'>";
        })
        .def("__bool__", &PyAE::PyOutputModule::IsValid)
        .def_property_readonly("_handle",
                              [](const PyAE::PyOutputModule& self) {
                                  return reinterpret_cast<uintptr_t>(self.GetHandle());
                              },
                              "Internal handle (for low-level SDK access)");

    // レンダーキューアイテムクラス
    py::class_<PyAE::PyRenderQueueItem, std::shared_ptr<PyAE::PyRenderQueueItem>>(m, "RenderQueueItem")
        .def_property_readonly("valid", &PyAE::PyRenderQueueItem::IsValid,
                              "Check if render queue item is valid")
        .def_property_readonly("status", &PyAE::PyRenderQueueItem::GetStatus,
                              "Render queue item status")
        .def_property("queued",
                     [](const PyAE::PyRenderQueueItem& self) {
                         return self.GetStatus() == PyAE::RenderStatus::Queued;
                     },
                     &PyAE::PyRenderQueueItem::SetQueued,
                     "Whether item is queued for rendering")
        .def_property_readonly("comp_name", &PyAE::PyRenderQueueItem::GetCompName,
                              "Name of the composition")
        .def_property_readonly("num_output_modules", &PyAE::PyRenderQueueItem::GetNumOutputModules,
                              "Number of output modules")
        .def("__repr__", [](const PyAE::PyRenderQueueItem& self) {
            if (!self.IsValid()) return std::string("<RenderQueueItem: invalid>");
            std::string name = self.GetCompName();
            return std::string("<RenderQueueItem: '") + name + "'>";
        })
        .def("__bool__", &PyAE::PyRenderQueueItem::IsValid)
        .def_property_readonly("_handle",
                              [](const PyAE::PyRenderQueueItem& self) {
                                  return reinterpret_cast<uintptr_t>(self.GetHandle());
                              },
                              "Internal handle (for low-level SDK access)")
        .def("output_module", &PyAE::PyRenderQueueItem::GetOutputModule,
             "Get output module by index",
             py::arg("index") = 0)
        .def("delete", &PyAE::PyRenderQueueItem::Delete,
             "Delete this render queue item");

    // レンダーキュークラス
    py::class_<PyAE::PyRenderQueue>(m, "RenderQueue")
        .def_property_readonly("num_items", &PyAE::PyRenderQueue::GetNumItems,
                              "Number of items in render queue")
        .def("item", &PyAE::PyRenderQueue::GetItem,
             "Get render queue item by index",
             py::arg("index"))
        .def("items", &PyAE::PyRenderQueue::GetAllItems,
             "Get all render queue items");

    // レンダーキューサブモジュール
    py::module_ rq = m.def_submodule("render_queue", "Render Queue operations");

    rq.def("num_items", []() {
        return PyAE::PyRenderQueue::Instance().GetNumItems();
    }, "Get number of items in render queue");

    rq.def("item", [](int index) {
        return PyAE::PyRenderQueue::Instance().GetItem(index);
    }, "Get render queue item by index",
    py::arg("index"));

    rq.def("items", []() {
        return PyAE::PyRenderQueue::Instance().GetAllItems();
    }, "Get all render queue items");

    // テンプレート関連（SDKでは直接サポートされていないため、空リストを返す）
    rq.def("render_settings_templates", []() -> std::vector<std::string> {
        // NOTE: AEGP SDKではRender Settingsテンプレート名を取得するAPIが提供されていません
        // ExtendScriptのRenderQueue.templatesに相当する機能はAEGPでは利用不可
        PYAE_LOG_WARNING("RenderQueue",
            "render_settings_templates: AEGP SDK does not provide template enumeration API");
        return {};
    }, "Get render settings template names (Note: AEGP SDK limitation - returns empty list)");

    rq.def("add_comp", [](py::object comp, const std::string& path) {
        AEGP_CompH compH = nullptr;

        // PyCompRef の場合
        if (py::isinstance<PyAE::PyCompRef>(comp)) {
            auto compRef = comp.cast<PyAE::PyCompRef>();
            if (!compRef.IsValid()) {
                throw std::runtime_error("Invalid composition reference");
            }
            compH = compRef.GetHandle();
        }
        // CompItem (ae.CompItem) の場合
        // _get_comp_handle_ptr() メソッドで AEGP_CompH を取得
        else if (py::hasattr(comp, "_get_comp_handle_ptr")) {
            try {
                uintptr_t ptr = comp.attr("_get_comp_handle_ptr")().cast<uintptr_t>();
                compH = reinterpret_cast<AEGP_CompH>(ptr);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Failed to get comp handle: ") + e.what());
            }
        }

        if (!compH) {
            throw std::runtime_error("Invalid composition: expected CompItem or CompRef");
        }

        return PyAE::PyRenderQueue::Instance().AddComp(compH, path);
    }, "Add composition to render queue",
       py::arg("comp"),
       py::arg("path") = "");

    rq.def("output_module_templates", []() -> std::vector<std::string> {
        // NOTE: AEGP SDKではOutput Moduleテンプレート名を取得するAPIが提供されていません
        PYAE_LOG_WARNING("RenderQueue",
            "output_module_templates: AEGP SDK does not provide template enumeration API");
        return {};
    }, "Get output module template names (Note: AEGP SDK limitation - returns empty list)");
}
