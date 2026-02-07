// PyItem.cpp
// PyAE - Python for After Effects
// アイテムAPIバインディング

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PluginState.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "PyRefTypes.h"
#include "PyCompClasses.h"
#include "PyLayerClasses.h"
#include "PyFootageClasses.h"

namespace py = pybind11;

namespace PyAE {

// アイテムタイプ列挙
enum class ItemType {
    None = 0,
    Folder,
    Comp,
    Footage,
    Solid
};

// 前方宣言
class PyItem;
class PyFolder;
class PyCompItem;
// Note: PyFootageItem is removed - use PyFootage from PyFootageClasses.h instead

// 注: PyItemRef と PyFolderRef は PyRefTypes.h で定義されている

// =============================================================
// PyItem - 基本アイテムクラス
// =============================================================
class PyItem {
public:
    PyItem() : m_itemH(nullptr) {}
    explicit PyItem(AEGP_ItemH itemH) : m_itemH(itemH) {}

    bool IsValid() const { return m_itemH != nullptr; }

    ItemType GetType() const {
        if (!m_itemH) return ItemType::None;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        AEGP_ItemType type;
        A_Err err = suites.itemSuite->AEGP_GetItemType(m_itemH, &type);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get item type");
        }

        switch (type) {
            case AEGP_ItemType_FOLDER: return ItemType::Folder;
            case AEGP_ItemType_COMP:   return ItemType::Comp;
            case AEGP_ItemType_FOOTAGE: return ItemType::Footage;
            default:                   return ItemType::None;
        }
    }

    std::string GetName() const {
        if (!m_itemH) return "";

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite || !suites.memorySuite) {
            throw std::runtime_error("Required suites not available");
        }

        AEGP_MemHandle nameH;
        A_Err err = suites.itemSuite->AEGP_GetItemName(
            state.GetPluginID(), m_itemH, &nameH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get item name");
        }

        ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);

        ScopedMemLock lock(suites.memorySuite, nameH);
        A_UTF16Char* namePtr = lock.As<A_UTF16Char>();
        if (!namePtr) return "";

        std::string result = StringUtils::Utf16ToUtf8(namePtr);
        return result;
    }

    void SetName(const std::string& name) {
        if (!m_itemH) {
            throw std::runtime_error("Invalid item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        std::wstring wname = StringUtils::Utf8ToWide(name);
        A_Err err = suites.itemSuite->AEGP_SetItemName(
            m_itemH, reinterpret_cast<const A_UTF16Char*>(wname.c_str()));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set item name");
        }
    }

    std::string GetComment() const {
        if (!m_itemH) return "";

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite || !suites.memorySuite) {
            throw std::runtime_error("Required suites not available");
        }

        AEGP_MemHandle commentH;
        A_Err err = suites.itemSuite->AEGP_GetItemComment(m_itemH, &commentH);
        if (err != A_Err_NONE) {
            return "";
        }

        ScopedMemHandle scopedComment(state.GetPluginID(), suites.memorySuite, commentH);

        ScopedMemLock lock(suites.memorySuite, commentH);
        A_UTF16Char* commentPtr = lock.As<A_UTF16Char>();
        if (!commentPtr) return "";

        std::string result = StringUtils::Utf16ToUtf8(commentPtr);
        return result;
    }

    void SetComment(const std::string& comment) {
        if (!m_itemH) {
            throw std::runtime_error("Invalid item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        std::wstring wcomment = StringUtils::Utf8ToWide(comment);
        A_Err err = suites.itemSuite->AEGP_SetItemComment(
            m_itemH, reinterpret_cast<const A_UTF16Char*>(wcomment.c_str()));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set item comment");
        }
    }

    int GetWidth() const {
        if (!m_itemH) return 0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_long width, height;
        A_Err err = suites.itemSuite->AEGP_GetItemDimensions(m_itemH, &width, &height);
        if (err != A_Err_NONE) return 0;

        return static_cast<int>(width);
    }

    int GetHeight() const {
        if (!m_itemH) return 0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_long width, height;
        A_Err err = suites.itemSuite->AEGP_GetItemDimensions(m_itemH, &width, &height);
        if (err != A_Err_NONE) return 0;

        return static_cast<int>(height);
    }

    double GetDuration() const {
        if (!m_itemH) return 0.0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_Time duration;
        A_Err err = suites.itemSuite->AEGP_GetItemDuration(m_itemH, &duration);
        if (err != A_Err_NONE) return 0.0;

        if (duration.scale == 0) return 0.0;
        return static_cast<double>(duration.value) / duration.scale;
    }

    double GetFrameRate() const {
        if (!m_itemH) return 0.0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // フレームレートはコンポジションから取得する必要がある
        AEGP_ItemType type;
        A_Err err = suites.itemSuite->AEGP_GetItemType(m_itemH, &type);
        if (err != A_Err_NONE || type != AEGP_ItemType_COMP) {
            return 0.0;
        }

        AEGP_CompH compH;
        err = suites.compSuite->AEGP_GetCompFromItem(m_itemH, &compH);
        if (err != A_Err_NONE) return 0.0;

        A_FpLong fps;
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) return 0.0;

        return static_cast<double>(fps);
    }

    bool GetSelected() const {
        if (!m_itemH) return false;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_Boolean selected;
        A_Err err = suites.itemSuite->AEGP_IsItemSelected(m_itemH, &selected);
        if (err != A_Err_NONE) return false;

        return selected != 0;
    }

    void SetSelected(bool selected) {
        if (!m_itemH) return;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        suites.itemSuite->AEGP_SelectItem(m_itemH, selected ? TRUE : FALSE, TRUE);
    }

    // 親フォルダ取得
    // 内部使用: PyFolderRefを返す（バインディングでラムダを使用して変換）
    PyFolderRef GetParentFolderInternal() const {
        if (!m_itemH) return PyFolderRef();

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        AEGP_ItemH parentH;
        A_Err err = suites.itemSuite->AEGP_GetItemParentFolder(m_itemH, &parentH);
        if (err != A_Err_NONE || !parentH) {
            return PyFolderRef();
        }

        return PyFolderRef(parentH);
    }

    // 親フォルダ設定
    void SetParentFolder(py::object folder) {
        if (!m_itemH) {
            throw std::runtime_error("Invalid item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite || !suites.projSuite) {
            throw std::runtime_error("Required suites not available");
        }

        AEGP_ItemH parentH = nullptr;
        if (folder.is_none()) {
            // Get root folder when None is passed
            // Use the first (and usually only) project
            AEGP_ProjectH projectH = nullptr;
            A_Err err = suites.projSuite->AEGP_GetProjectByIndex(0, &projectH);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to get project");
            }

            err = suites.projSuite->AEGP_GetProjectRootFolder(projectH, &parentH);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to get root folder");
            }
        } else {
            if (py::isinstance<PyFolderRef>(folder)) {
                parentH = folder.cast<PyFolderRef>().GetHandle();
            } else if (py::isinstance<PyItem>(folder)) {
                PyItem& item = folder.cast<PyItem&>();
                if (item.GetType() != ItemType::Folder) {
                    throw std::runtime_error("Parent must be a folder");
                }
                parentH = item.GetHandle();
            }
        }

        A_Err err = suites.itemSuite->AEGP_SetItemParentFolder(m_itemH, parentH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set parent folder");
        }
    }

    // アイテム削除
    void Delete() {
        if (!m_itemH) {
            throw std::runtime_error("Invalid item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        A_Err err = suites.itemSuite->AEGP_DeleteItem(m_itemH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to delete item");
        }

        m_itemH = nullptr;
    }

    // アイテム複製
    PyItemRef Duplicate() const {
        if (!m_itemH) {
            throw std::runtime_error("Invalid item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // アイテムタイプによって処理を分ける
        AEGP_ItemType type;
        A_Err err = suites.itemSuite->AEGP_GetItemType(m_itemH, &type);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get item type");
        }

        if (type == AEGP_ItemType_COMP) {
            // コンポジションの複製
            if (!suites.compSuite) {
                throw std::runtime_error("Comp suite not available");
            }

            AEGP_CompH compH;
            err = suites.compSuite->AEGP_GetCompFromItem(m_itemH, &compH);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to get comp from item");
            }

            AEGP_CompH newCompH;
            err = suites.compSuite->AEGP_DuplicateComp(compH, &newCompH);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to duplicate comp");
            }

            AEGP_ItemH newItemH;
            err = suites.compSuite->AEGP_GetItemFromComp(newCompH, &newItemH);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to get item from duplicated comp");
            }

            return PyItemRef(newItemH);
        } else if (type == AEGP_ItemType_FOOTAGE) {
            // フッテージの複製（AEでは直接複製できないため、同じソースで新規作成）
            throw std::runtime_error("Footage items cannot be directly duplicated. Use import instead.");
        } else if (type == AEGP_ItemType_FOLDER) {
            // フォルダの複製（AEでは直接複製できない）
            throw std::runtime_error("Folder items cannot be directly duplicated. Create a new folder instead.");
        }

        throw std::runtime_error("Cannot duplicate this item type");
    }

    // タイムライン上のラベルカラー取得
    int GetLabel() const {
        if (!m_itemH) return 0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        AEGP_LabelID label;
        A_Err err = suites.itemSuite->AEGP_GetItemLabel(m_itemH, &label);
        if (err != A_Err_NONE) return 0;

        return static_cast<int>(label);
    }

    // ラベルカラー設定
    void SetLabel(int label) {
        if (!m_itemH) {
            throw std::runtime_error("Invalid item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        A_Err err = suites.itemSuite->AEGP_SetItemLabel(m_itemH, static_cast<AEGP_LabelID>(label));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set item label");
        }
    }

    // アイテムID取得
    int GetID() const {
        if (!m_itemH) return 0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        A_long id;
        A_Err err = suites.itemSuite->AEGP_GetItemID(m_itemH, &id);
        if (err != A_Err_NONE) return 0;

        return static_cast<int>(id);
    }

    // 使用フラグ取得
    bool GetUseProxy() const {
        if (!m_itemH) return false;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        // Use AEGP_GetItemFlags to check AEGP_ItemFlag_USING_PROXY
        AEGP_ItemFlags flags;
        A_Err err = suites.itemSuite->AEGP_GetItemFlags(m_itemH, &flags);
        if (err != A_Err_NONE) return false;

        return (flags & AEGP_ItemFlag_USING_PROXY) != 0;
    }

    // プロキシ使用設定
    void SetUseProxy(bool useProxy) {
        if (!m_itemH) {
            throw std::runtime_error("Invalid item");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        A_Err err = suites.itemSuite->AEGP_SetItemUseProxy(m_itemH, useProxy ? TRUE : FALSE);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set item use proxy");
        }
    }

    // 欠落フラグ取得
    bool GetMissing() const {
        if (!m_itemH) return true;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        // Use AEGP_GetItemFlags to check AEGP_ItemFlag_MISSING
        AEGP_ItemFlags flags;
        A_Err err = suites.itemSuite->AEGP_GetItemFlags(m_itemH, &flags);
        if (err != A_Err_NONE) return false;

        return (flags & AEGP_ItemFlag_MISSING) != 0;
    }

    AEGP_ItemH GetHandle() const { return m_itemH; }

    // 静的メソッド：アクティブアイテム取得
    static PyItem GetActiveItem() {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            throw std::runtime_error("Item suite not available");
        }

        AEGP_ItemH itemH;
        A_Err err = suites.itemSuite->AEGP_GetActiveItem(&itemH);
        if (err != A_Err_NONE || !itemH) {
            return PyItem();
        }

        return PyItem(itemH);
    }

protected:
    AEGP_ItemH m_itemH;
};

// =============================================================
// PyFolder - フォルダクラス
// =============================================================
class PyFolder : public PyItem {
public:
    PyFolder() : PyItem() {}
    explicit PyFolder(AEGP_ItemH itemH) : PyItem(itemH) {
        // 型チェック
        if (m_itemH) {
            auto& state = PluginState::Instance();
            const auto& suites = state.GetSuites();

            AEGP_ItemType type;
            A_Err err = suites.itemSuite->AEGP_GetItemType(m_itemH, &type);
            if (err != A_Err_NONE || type != AEGP_ItemType_FOLDER) {
                m_itemH = nullptr;
            }
        }
    }

    // フォルダ内のアイテム数取得
    int GetNumItems() const {
        if (!m_itemH) return 0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite || !suites.projSuite) {
            throw std::runtime_error("Item/Project suite not available");
        }

        // プロジェクトハンドルを取得
        AEGP_ProjectH projH;
        A_Err err = suites.projSuite->AEGP_GetProjectByIndex(0, &projH);
        if (err != A_Err_NONE || !projH) return 0;

        // フォルダの子要素のみカウント
        int count = 0;
        AEGP_ItemH curItem = nullptr;

        // プロジェクトの最初のアイテムから探索
        err = suites.itemSuite->AEGP_GetFirstProjItem(projH, &curItem);

        while (curItem && err == A_Err_NONE) {
            AEGP_ItemH parentH;
            suites.itemSuite->AEGP_GetItemParentFolder(curItem, &parentH);
            if (parentH == m_itemH) {
                count++;
            }

            AEGP_ItemH nextItem;
            err = suites.itemSuite->AEGP_GetNextProjItem(projH, curItem, &nextItem);
            curItem = nextItem;
        }

        return count;
    }

    // フォルダ内のアイテム取得
    // 内部使用: PyItemRefのベクトルを返す（バインディングでラムダを使用して変換）
    std::vector<PyItemRef> GetItemsInternal() const {
        std::vector<PyItemRef> items;
        if (!m_itemH) return items;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite || !suites.projSuite) {
            throw std::runtime_error("Item/Project suite not available");
        }

        // プロジェクトハンドルを取得
        AEGP_ProjectH projH;
        A_Err err = suites.projSuite->AEGP_GetProjectByIndex(0, &projH);
        if (err != A_Err_NONE || !projH) return items;

        AEGP_ItemH curItem = nullptr;
        err = suites.itemSuite->AEGP_GetFirstProjItem(projH, &curItem);

        while (curItem && err == A_Err_NONE) {
            AEGP_ItemH parentH;
            suites.itemSuite->AEGP_GetItemParentFolder(curItem, &parentH);
            if (parentH == m_itemH) {
                items.push_back(PyItemRef(curItem));
            }

            AEGP_ItemH nextItem;
            err = suites.itemSuite->AEGP_GetNextProjItem(projH, curItem, &nextItem);
            curItem = nextItem;
        }

        return items;
    }
};

// =============================================================
// PyCompItem - コンポジションアイテムクラス
// =============================================================
class PyCompItem : public PyItem {
public:
    PyCompItem() : PyItem(), m_compH(nullptr) {}
    explicit PyCompItem(AEGP_ItemH itemH) : PyItem(itemH), m_compH(nullptr) {
        if (m_itemH) {
            auto& state = PluginState::Instance();
            const auto& suites = state.GetSuites();

            // suites の null チェック
            if (!suites.itemSuite || !suites.compSuite) {
                m_itemH = nullptr;
                return;
            }

            AEGP_ItemType type;
            A_Err err = suites.itemSuite->AEGP_GetItemType(m_itemH, &type);
            if (err != A_Err_NONE || type != AEGP_ItemType_COMP) {
                m_itemH = nullptr;
            } else {
                err = suites.compSuite->AEGP_GetCompFromItem(m_itemH, &m_compH);
                if (err != A_Err_NONE) {
                    m_compH = nullptr;
                }
            }
        }
    }

    AEGP_CompH GetCompHandle() const { return m_compH; }

private:
    AEGP_CompH m_compH;
};

// =============================================================
// ResolveItemHandle: AEGP_ItemH を実オブジェクトに変換
// PyRefTypes.h で宣言されているヘルパー関数
// PyProject.cpp など他のファイルから呼び出し可能
// =============================================================
py::object ResolveItemHandle(AEGP_ItemH itemH) {
    if (!itemH) return py::none();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.itemSuite) return py::cast(PyItem(itemH));

    AEGP_ItemType type;
    A_Err err = suites.itemSuite->AEGP_GetItemType(itemH, &type);
    if (err != A_Err_NONE) return py::cast(PyItem(itemH));

    switch (type) {
        case AEGP_ItemType_COMP:
            return py::cast(PyCompItem(itemH));
        case AEGP_ItemType_FOOTAGE:
            // Use unified PyFootage class (from PyFootageClasses.h)
            // Pass shared_ptr directly - PyFootage is non-copyable, so dereferencing would fail
            return py::cast(PyFootage::FromItem(itemH));
        case AEGP_ItemType_FOLDER:
            return py::cast(PyFolder(itemH));
        default:
            return py::cast(PyItem(itemH));
    }
}

} // namespace PyAE

void init_item(py::module_& m) {
    // ヘルパーラムダ: AEGP_ItemH を自動的に resolve して実オブジェクトを返す
    // ItemRef/FolderRefは内部使用のみで、Python APIには露出しない
    auto resolve_item_handle = [](AEGP_ItemH itemH) -> py::object {
        if (!itemH) return py::none();

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) return py::cast(PyAE::PyItem(itemH));

        AEGP_ItemType type;
        A_Err err = suites.itemSuite->AEGP_GetItemType(itemH, &type);
        if (err != A_Err_NONE) return py::cast(PyAE::PyItem(itemH));

        switch (type) {
            case AEGP_ItemType_COMP:
                return py::cast(PyAE::PyCompItem(itemH));
            case AEGP_ItemType_FOOTAGE:
                // Use unified PyFootage class (from PyFootageClasses.h)
                // Pass shared_ptr directly - PyFootage is non-copyable, so dereferencing would fail
                return py::cast(PyAE::PyFootage::FromItem(itemH));
            case AEGP_ItemType_FOLDER:
                return py::cast(PyAE::PyFolder(itemH));
            default:
                return py::cast(PyAE::PyItem(itemH));
        }
    };

    // PyItemRefからresolveするラムダ
    auto resolve_item = [resolve_item_handle](const PyAE::PyItemRef& ref) -> py::object {
        if (!ref.IsValid()) return py::none();
        return resolve_item_handle(ref.GetHandle());
    };

    // アイテムタイプ列挙
    py::enum_<PyAE::ItemType>(m, "ItemType")
        .value("None_", PyAE::ItemType::None)  // Python予約語回避
        .value("Folder", PyAE::ItemType::Folder)
        .value("Comp", PyAE::ItemType::Comp)
        .value("Footage", PyAE::ItemType::Footage)
        .value("Solid", PyAE::ItemType::Solid);

    // Note: FootageType enum is now bound in PyFootage.cpp
    // Note: ItemRef/FolderRef are internal-use only and not exposed to Python API

    // アイテムクラス
    py::class_<PyAE::PyItem>(m, "Item")
        .def(py::init<>())
        .def_property_readonly("valid", &PyAE::PyItem::IsValid,
                              "Check if item is valid")
        .def_property_readonly("type", &PyAE::PyItem::GetType,
                              "Get item type")
        .def_property_readonly("id", &PyAE::PyItem::GetID,
                              "Get item ID")
        .def_property("name", &PyAE::PyItem::GetName, &PyAE::PyItem::SetName,
                     "Item name")
        .def_property("comment", &PyAE::PyItem::GetComment, &PyAE::PyItem::SetComment,
                     "Item comment")
        .def_property_readonly("width", &PyAE::PyItem::GetWidth,
                              "Item width in pixels")
        .def_property_readonly("height", &PyAE::PyItem::GetHeight,
                              "Item height in pixels")
        .def_property_readonly("duration", &PyAE::PyItem::GetDuration,
                              "Item duration in seconds")
        .def_property_readonly("frame_rate", &PyAE::PyItem::GetFrameRate,
                              "Item frame rate (for comps)")
        .def_property("selected", &PyAE::PyItem::GetSelected, &PyAE::PyItem::SetSelected,
                     "Item selection state")
        .def_property("label", &PyAE::PyItem::GetLabel, &PyAE::PyItem::SetLabel,
                     "Item label color index")
        .def_property("use_proxy", &PyAE::PyItem::GetUseProxy, &PyAE::PyItem::SetUseProxy,
                     "Whether item uses proxy")
        .def_property_readonly("missing", &PyAE::PyItem::GetMissing,
                              "Whether item has missing footage")
        .def_property("parent_folder",
            [](PyAE::PyItem& self) -> py::object {
                // GetParentFolderInternalはPyFolderRefを返すので、PyFolderに変換
                PyAE::PyFolderRef ref = self.GetParentFolderInternal();
                if (!ref.IsValid()) return py::none();
                return py::cast(PyAE::PyFolder(ref.GetHandle()));
            },
            &PyAE::PyItem::SetParentFolder,
            "Parent folder of this item")
        .def("delete", &PyAE::PyItem::Delete,
             "Delete this item from the project")
        .def("delete", &PyAE::PyItem::Delete,
             "Delete this item from the project")
        .def("duplicate", [resolve_item](PyAE::PyItem& self) {
            PyAE::PyItemRef ref = self.Duplicate();
            return resolve_item(ref);
        }, "Duplicate this item (comps only)")
        .def_static("get_active", &PyAE::PyItem::GetActiveItem,
                   "Get the currently active item")
        // Python protocol methods
        .def("__repr__", [](const PyAE::PyItem& self) {
            if (!self.IsValid()) return std::string("<Item: invalid>");
            std::string name = self.GetName();
            int id = self.GetID();
            return std::string("<Item: '") + name + "' id=" + std::to_string(id) + ">";
        })
        .def("__bool__", &PyAE::PyItem::IsValid)
        .def("__eq__", [](const PyAE::PyItem& self, const py::object& other) {
            if (!py::isinstance<PyAE::PyItem>(other)) return false;
            return self.GetHandle() == other.cast<PyAE::PyItem>().GetHandle();
        })
        .def("__hash__", [](const PyAE::PyItem& self) {
            return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(self.GetHandle()));
        })
        .def_property_readonly("_handle", [](PyAE::PyItem& self) -> uintptr_t {
            AEGP_ItemH itemH = self.GetHandle();
            if (!itemH) {
                return 0;
            }
            return reinterpret_cast<uintptr_t>(itemH);
        }, "Internal: Item handle as integer")
        .def("_get_handle_ptr", [](PyAE::PyItem& self) -> uintptr_t {
            AEGP_ItemH itemH = self.GetHandle();
            if (!itemH) {
                throw std::runtime_error("Invalid item handle");
            }
            return reinterpret_cast<uintptr_t>(itemH);
        }, "Internal: Get item handle as integer pointer (Legacy)");

    // フォルダクラス
    py::class_<PyAE::PyFolder, PyAE::PyItem>(m, "Folder")
        .def(py::init<>())
        .def_property_readonly("num_items", &PyAE::PyFolder::GetNumItems,
                              "Number of items in this folder")
        .def_property_readonly("items",
            [resolve_item_handle](PyAE::PyFolder& self) -> py::list {
                // GetItemsInternalはstd::vector<PyItemRef>を返すので、実オブジェクトに変換
                py::list result;
                std::vector<PyAE::PyItemRef> items = self.GetItemsInternal();
                for (const auto& ref : items) {
                    py::object item = resolve_item_handle(ref.GetHandle());
                    if (!item.is_none()) {
                        result.append(item);
                    }
                }
                return result;
            },
            "List of items in this folder")
        // Python protocol methods
        .def("__repr__", [](const PyAE::PyFolder& self) {
            if (!self.IsValid()) return std::string("<Folder: invalid>");
            std::string name = self.GetName();
            int n = self.GetNumItems();
            return std::string("<Folder: '") + name + "' items=" + std::to_string(n) + ">";
        })
        .def("__bool__", &PyAE::PyFolder::IsValid)
        // Collection protocols
        .def("__len__", &PyAE::PyFolder::GetNumItems)
        .def("__iter__", [resolve_item_handle](PyAE::PyFolder& self) {
            py::list result;
            std::vector<PyAE::PyItemRef> items = self.GetItemsInternal();
            for (const auto& ref : items) {
                py::object item = resolve_item_handle(ref.GetHandle());
                if (!item.is_none()) result.append(item);
            }
            return result.attr("__iter__")();
        })
        .def("__getitem__", [resolve_item_handle](PyAE::PyFolder& self, int index) {
            std::vector<PyAE::PyItemRef> items = self.GetItemsInternal();
            int num = static_cast<int>(items.size());
            if (index < 0) index += num;
            if (index < 0 || index >= num) throw py::index_error("Folder item index out of range");
            py::object item = resolve_item_handle(items[index].GetHandle());
            if (item.is_none()) throw py::index_error("Failed to resolve item");
            return item;
        })
        .def("__contains__", [resolve_item_handle](PyAE::PyFolder& self, const std::string& name) {
            std::vector<PyAE::PyItemRef> items = self.GetItemsInternal();
            for (const auto& ref : items) {
                py::object item = resolve_item_handle(ref.GetHandle());
                if (!item.is_none() && py::hasattr(item, "name")) {
                    if (item.attr("name").cast<std::string>() == name) return true;
                }
            }
            return false;
        });

    // コンポアイテムクラス
    py::class_<PyAE::PyCompItem, PyAE::PyItem>(m, "CompItem")
        .def(py::init<>())
        // 内部ハンドル取得（RenderQueue等で使用）
        .def_property_readonly("_handle", [](PyAE::PyCompItem& self) -> uintptr_t {
             AEGP_ItemH itemH = self.GetHandle();
             if (!itemH) return 0;
             return reinterpret_cast<uintptr_t>(itemH);
        }, "Internal: Item handle (to AEGP_ItemH)")
        .def_property_readonly("_comp_handle", [](PyAE::PyCompItem& self) -> uintptr_t {
             AEGP_CompH compH = self.GetCompHandle();
             if (!compH) return 0;
             return reinterpret_cast<uintptr_t>(compH);
        }, "Internal: Comp handle (to AEGP_CompH)")
        .def("_get_comp_handle_ptr", [](PyAE::PyCompItem& self) -> uintptr_t {
            AEGP_CompH compH = self.GetCompHandle();
            if (!compH) {
                throw std::runtime_error("Invalid composition handle");
            }
            return reinterpret_cast<uintptr_t>(compH);
        }, "Internal: Get comp handle as integer pointer (Legacy)")
        // レイヤー追加メソッド (PyCompへ委譲)
        .def("add_solid", [](PyAE::PyCompItem& self, const std::string& name, int width, int height,
                             py::object color, double duration) {
            py::sequence seq = color.cast<py::sequence>();
            if (py::len(seq) < 3) throw std::invalid_argument("Color must have at least 3 elements (RGB)");
            PyAE::PyComp comp(self.GetCompHandle());
            return comp.AddSolid(name, width, height,
                                 seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>(), duration);
        }, "Add a solid layer",
           py::arg("name"), py::arg("width"), py::arg("height"),
           py::arg("color") = py::make_tuple(1.0, 1.0, 1.0), py::arg("duration") = -1.0)
        .def("add_null", [](PyAE::PyCompItem& self, const std::string& name, double duration) {
            PyAE::PyComp comp(self.GetCompHandle());
            return comp.AddNull(name, duration);
        }, "Add a null layer",
           py::arg("name") = "Null", py::arg("duration") = -1.0)
        .def("add_camera", [](PyAE::PyCompItem& self, const std::string& name, py::object center) {
            py::sequence seq = center.cast<py::sequence>();
            if (py::len(seq) < 2) throw std::invalid_argument("Center must have at least 2 elements (x, y)");
            PyAE::PyComp comp(self.GetCompHandle());
            return comp.AddCamera(name, seq[0].cast<double>(), seq[1].cast<double>());
        }, "Add a camera layer",
           py::arg("name") = "Camera", py::arg("center") = py::make_tuple(0.0, 0.0))
        .def("add_light", [](PyAE::PyCompItem& self, const std::string& name, py::object center) {
            py::sequence seq = center.cast<py::sequence>();
            if (py::len(seq) < 2) throw std::invalid_argument("Center must have at least 2 elements (x, y)");
            PyAE::PyComp comp(self.GetCompHandle());
            return comp.AddLight(name, seq[0].cast<double>(), seq[1].cast<double>());
        }, "Add a light layer",
           py::arg("name") = "Light", py::arg("center") = py::make_tuple(0.0, 0.0))
        .def("add_layer", [](PyAE::PyCompItem& self, py::object item, double duration) {
            PyAE::PyComp comp(self.GetCompHandle());
            // PyItemからAEGP_ItemHを取得
            AEGP_ItemH itemH = nullptr;
            if (py::hasattr(item, "_handle")) {
                uintptr_t ptr = item.attr("_handle").cast<uintptr_t>();
                itemH = reinterpret_cast<AEGP_ItemH>(ptr);
            } else if (py::hasattr(item, "_get_handle_ptr")) {
                // Legacy support
                uintptr_t ptr = item.attr("_get_handle_ptr")().cast<uintptr_t>();
                itemH = reinterpret_cast<AEGP_ItemH>(ptr);
            } else {
                throw std::runtime_error("Invalid item type - expected Item or ItemRef");
            }
            return comp.AddLayer(itemH, duration);
        }, "Add a layer from an item (footage, comp, etc.)",
           py::arg("item"), py::arg("duration") = -1.0)
        .def("add_text", [](PyAE::PyCompItem& self, const std::string& text) {
            PyAE::PyComp comp(self.GetCompHandle());
            return comp.AddText(text);
        }, "Add a text layer",
           py::arg("text") = "")
        .def("add_shape", [](PyAE::PyCompItem& self) {
            PyAE::PyComp comp(self.GetCompHandle());
            return comp.AddShape();
        }, "Add a shape layer")
        // Composition specific properties
        .def_property_readonly("num_layers", [](PyAE::PyCompItem& self) {
            return PyAE::PyComp(self.GetCompHandle()).GetNumLayers();
        }, "Number of layers")
        // layer() - int と string 両方に対応
        .def("layer", [](PyAE::PyCompItem& self, py::object key) {
            PyAE::PyComp comp(self.GetCompHandle());
            return comp.GetLayer(key);
        }, "Get layer by index (0-based) or name",
           py::arg("key"))
        .def_property_readonly("layers", [](PyAE::PyCompItem& self) {
            return PyAE::PyComp(self.GetCompHandle()).GetLayers();
        }, "Get all layers")
        // 選択レイヤー
        .def_property_readonly("selected_layers", [](PyAE::PyCompItem& self) {
            return PyAE::PyComp(self.GetCompHandle()).GetSelectedLayers();
        }, "Get selected layers")
        // ピクセルアスペクト
        .def_property("pixel_aspect",
            [](PyAE::PyCompItem& self) {
                return PyAE::PyComp(self.GetCompHandle()).GetPixelAspect();
            },
            [](PyAE::PyCompItem& self, double value) {
                PyAE::PyComp(self.GetCompHandle()).SetPixelAspect(value);
            }, "Pixel aspect ratio")
        // 背景色
        .def_property("bg_color",
            [](PyAE::PyCompItem& self) {
                return PyAE::PyComp(self.GetCompHandle()).GetBgColor();
            },
            [](PyAE::PyCompItem& self, py::tuple color) {
                PyAE::PyComp(self.GetCompHandle()).SetBgColor(color);
            }, "Background color (r, g, b)")
        // ワークエリア
        .def_property("work_area_start",
            [](PyAE::PyCompItem& self) {
                return PyAE::PyComp(self.GetCompHandle()).GetWorkAreaStart();
            },
            [](PyAE::PyCompItem& self, double value) {
                PyAE::PyComp(self.GetCompHandle()).SetWorkAreaStart(value);
            }, "Work area start time")
        .def_property("work_area_duration",
            [](PyAE::PyCompItem& self) {
                return PyAE::PyComp(self.GetCompHandle()).GetWorkAreaDuration();
            },
            [](PyAE::PyCompItem& self, double value) {
                PyAE::PyComp(self.GetCompHandle()).SetWorkAreaDuration(value);
            }, "Work area duration")
        // duration setter を追加
        .def_property("duration",
            [](PyAE::PyCompItem& self) {
                return PyAE::PyComp(self.GetCompHandle()).GetDuration();
            },
            [](PyAE::PyCompItem& self, double value) {
                PyAE::PyComp(self.GetCompHandle()).SetDuration(value);
            }, "Composition duration")
        // frame_rate setter を追加
        .def_property("frame_rate",
            [](PyAE::PyCompItem& self) {
                return PyAE::PyComp(self.GetCompHandle()).GetFrameRate();
            },
            [](PyAE::PyCompItem& self, double value) {
                PyAE::PyComp(self.GetCompHandle()).SetFrameRate(value);
            }, "Composition frame rate")
        // Python protocol methods
        .def("__repr__", [](const PyAE::PyCompItem& self) {
            if (!self.IsValid()) return std::string("<CompItem: invalid>");
            std::string name = self.GetName();
            int w = self.GetWidth();
            int h = self.GetHeight();
            return std::string("<CompItem: '") + name + "' " + std::to_string(w) + "x" + std::to_string(h) + ">";
        })
        .def("__bool__", &PyAE::PyCompItem::IsValid)
        .def("__eq__", [](const PyAE::PyCompItem& self, const py::object& other) {
            if (!py::isinstance<PyAE::PyCompItem>(other)) return false;
            return self.GetHandle() == other.cast<PyAE::PyCompItem>().GetHandle();
        })
        .def("__hash__", [](const PyAE::PyCompItem& self) {
            return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(self.GetHandle()));
        })
        // Collection protocols (layers)
        .def("__len__", [](PyAE::PyCompItem& self) {
            return PyAE::PyComp(self.GetCompHandle()).GetNumLayers();
        })
        .def("__iter__", [](PyAE::PyCompItem& self) {
            py::list layers = py::cast(PyAE::PyComp(self.GetCompHandle()).GetLayers());
            return layers.attr("__iter__")();
        })
        .def("__getitem__", [](PyAE::PyCompItem& self, py::object key) {
            PyAE::PyComp comp(self.GetCompHandle());
            if (py::isinstance<py::int_>(key)) {
                int idx = key.cast<int>();
                int num = comp.GetNumLayers();
                if (idx < 0) idx += num;
                if (idx < 0 || idx >= num) throw py::index_error("Layer index out of range");
                return comp.GetLayer(py::cast(idx));
            } else if (py::isinstance<py::str>(key)) {
                auto result = comp.GetLayerByName(key.cast<std::string>());
                if (!result.IsValid()) throw py::key_error(key.cast<std::string>());
                return result;
            }
            throw py::type_error("Layer index must be int or str");
        })
        .def("__contains__", [](PyAE::PyCompItem& self, const std::string& name) {
            PyAE::PyComp comp(self.GetCompHandle());
            auto result = comp.GetLayerByName(name);
            return result.IsValid();
        })
        // Serialization methods (delegate to ae_serialize module)
        .def("to_dict", [](PyAE::PyCompItem& self) -> py::dict {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                PyAE::PyComp comp(self.GetCompHandle());
                return serialize.attr("comp_to_dict")(comp).cast<py::dict>();
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to serialize comp: ") + e.what());
            }
        }, "Export composition to dictionary")
        .def("update_from_dict", [](PyAE::PyCompItem& self, const py::dict& data) {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                PyAE::PyComp comp(self.GetCompHandle());
                serialize.attr("comp_update_from_dict")(comp, data);
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to update comp from dict: ") + e.what());
            }
        }, "Update existing composition from dictionary",
           py::arg("data"));

    // Note: FootageItem has been unified with Footage class
    // The Footage class is bound in PyFootage.cpp
    // FootageItem alias will be created in Python (__init__.pyi) for backward compatibility
}
