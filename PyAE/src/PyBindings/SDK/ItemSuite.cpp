#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_ItemSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_ItemSuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetActiveItem", []() -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = nullptr;
        A_Err err = suites.itemSuite->AEGP_GetActiveItem(&itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetActiveItem failed with error: " + std::to_string(err));

        // Return 0 if no item is active (valid non-error condition)
        return reinterpret_cast<uintptr_t>(itemH);
    });

    sdk.def("AEGP_GetItemType", [](uintptr_t itemH_ptr) -> int {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemType: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemType itemType;
        A_Err err = suites.itemSuite->AEGP_GetItemType(
            reinterpret_cast<AEGP_ItemH>(itemH_ptr), &itemType);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemType failed with error: " + std::to_string(err));

        return (int)itemType;
    }, py::arg("itemH"));

    sdk.def("AEGP_GetItemName", [](int plugin_id, uintptr_t itemH_ptr) -> std::string {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemName: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_MemHandle nameH = nullptr;
        A_Err err = suites.itemSuite->AEGP_GetItemName(
            plugin_id, reinterpret_cast<AEGP_ItemH>(itemH_ptr), &nameH);
        if (err != A_Err_NONE || !nameH) throw std::runtime_error("AEGP_GetItemName failed");

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, nameH);
        PyAE::ScopedMemLock lock(suites.memorySuite, nameH);
        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, py::arg("plugin_id"), py::arg("itemH"));

    sdk.def("AEGP_GetItemDuration", [](uintptr_t itemH_ptr) -> double {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemDuration: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        A_Time duration;
        A_Err err = suites.itemSuite->AEGP_GetItemDuration(
            reinterpret_cast<AEGP_ItemH>(itemH_ptr), &duration);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemDuration failed");

        return PyAE::AETypeUtils::TimeToSeconds(duration);
    }, py::arg("itemH"));

    sdk.def("AEGP_GetItemFlags", [](uintptr_t itemH_ptr) -> int {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemFlags: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemFlags flags;
        A_Err err = suites.itemSuite->AEGP_GetItemFlags(
            reinterpret_cast<AEGP_ItemH>(itemH_ptr), &flags);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemFlags failed");

        return (int)flags;
    }, py::arg("itemH"));

    sdk.def("AEGP_GetItemDimensions", [](uintptr_t itemH_ptr) -> py::tuple {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemDimensions: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        A_long w = 0, h = 0;
        A_Err err = suites.itemSuite->AEGP_GetItemDimensions(
            reinterpret_cast<AEGP_ItemH>(itemH_ptr), &w, &h);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemDimensions failed");

        return py::make_tuple(w, h);
    }, py::arg("itemH"));

    sdk.def("AEGP_GetFirstProjItem", [](uintptr_t projectH_ptr) -> uintptr_t {
        if (projectH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetFirstProjItem: projectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ProjectH projectH = reinterpret_cast<AEGP_ProjectH>(projectH_ptr);
        AEGP_ItemH itemH = nullptr;

        A_Err err = suites.itemSuite->AEGP_GetFirstProjItem(projectH, &itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFirstProjItem failed");

        return reinterpret_cast<uintptr_t>(itemH);
    }, py::arg("projectH"), "Get the first project item.");

    sdk.def("AEGP_GetNextProjItem", [](uintptr_t projectH_ptr, uintptr_t itemH_ptr) -> uintptr_t {
        if (projectH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNextProjItem: projectH cannot be null");
        }
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNextProjItem: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ProjectH projectH = reinterpret_cast<AEGP_ProjectH>(projectH_ptr);
        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_ItemH next_itemH = nullptr;

        A_Err err = suites.itemSuite->AEGP_GetNextProjItem(projectH, itemH, &next_itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNextProjItem failed");

        // Returns 0 (nullptr) after last item
        return reinterpret_cast<uintptr_t>(next_itemH);
    }, py::arg("projectH"), py::arg("itemH"), "Get the next project item. Returns 0 after last item.");

    sdk.def("AEGP_GetItemID", [](uintptr_t itemH_ptr) -> int {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemID: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        A_long item_id = 0;

        A_Err err = suites.itemSuite->AEGP_GetItemID(itemH, &item_id);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemID failed");

        return (int)item_id;
    }, py::arg("itemH"), "Get item ID (unique identifier).");

    sdk.def("AEGP_DeleteItem", [](uintptr_t itemH_ptr) {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_DeleteItem: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        A_Err err = suites.itemSuite->AEGP_DeleteItem(itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DeleteItem failed");
    }, py::arg("itemH"), "Delete item (UNDOABLE). Removes item from all comps.");

    sdk.def("AEGP_SetItemName", [](uintptr_t itemH_ptr, const std::string& name) {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetItemName: itemH cannot be null");
        }
        PyAE::Validation::RequireNonEmpty(name, "name");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        std::wstring utf16_name = PyAE::StringUtils::Utf8ToUtf16(name);

        A_Err err = suites.itemSuite->AEGP_SetItemName(itemH, (const A_UTF16Char*)utf16_name.c_str());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetItemName failed");
    }, py::arg("itemH"), py::arg("name"), "Set item name (UNDOABLE).");

    sdk.def("AEGP_SelectItem", [](uintptr_t itemH_ptr, bool select, bool deselect_others) {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_SelectItem: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        A_Err err = suites.itemSuite->AEGP_SelectItem(
            itemH,
            select ? TRUE : FALSE,
            deselect_others ? TRUE : FALSE
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SelectItem failed");
    }, py::arg("itemH"), py::arg("select"), py::arg("deselect_others"),
       "Select or deselect item. deselect_others: deselect all other items.");

    sdk.def("AEGP_IsItemSelected", [](uintptr_t itemH_ptr) -> bool {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsItemSelected: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        A_Boolean selected = FALSE;

        A_Err err = suites.itemSuite->AEGP_IsItemSelected(itemH, &selected);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsItemSelected failed");

        return selected != FALSE;
    }, py::arg("itemH"), "Check if item is selected.");

    sdk.def("AEGP_GetItemParentFolder", [](uintptr_t itemH_ptr) -> uintptr_t {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemParentFolder: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_ItemH parent_folder_itemH = nullptr;

        A_Err err = suites.itemSuite->AEGP_GetItemParentFolder(itemH, &parent_folder_itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemParentFolder failed");

        return reinterpret_cast<uintptr_t>(parent_folder_itemH);
    }, py::arg("itemH"), "Get parent folder of item. Returns 0 if item is at root.");

    sdk.def("AEGP_SetItemParentFolder", [](uintptr_t itemH_ptr, uintptr_t parent_folder_itemH_ptr) {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetItemParentFolder: itemH cannot be null");
        }
        // Note: parent_folder_itemH_ptr can be 0 to move to root

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_ItemH parent_folder_itemH = reinterpret_cast<AEGP_ItemH>(parent_folder_itemH_ptr);

        A_Err err = suites.itemSuite->AEGP_SetItemParentFolder(itemH, parent_folder_itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetItemParentFolder failed");
    }, py::arg("itemH"), py::arg("parent_folder_itemH"), "Set parent folder of item (UNDOABLE).");

    sdk.def("AEGP_GetItemPixelAspectRatio", [](uintptr_t itemH_ptr) -> py::tuple {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemPixelAspectRatio: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        A_Ratio pixel_aspect_ratio = {0, 1};

        A_Err err = suites.itemSuite->AEGP_GetItemPixelAspectRatio(itemH, &pixel_aspect_ratio);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemPixelAspectRatio failed");

        return py::make_tuple(pixel_aspect_ratio.num, pixel_aspect_ratio.den);
    }, py::arg("itemH"), "Get pixel aspect ratio as (numerator, denominator) tuple.");

    sdk.def("AEGP_GetItemComment", [](int plugin_id, uintptr_t itemH_ptr) -> std::string {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemComment: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_MemHandle commentH = nullptr;
        A_Err err = suites.itemSuite->AEGP_GetItemComment(
            reinterpret_cast<AEGP_ItemH>(itemH_ptr), &commentH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemComment failed");
        if (!commentH) return "";

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, commentH);
        PyAE::ScopedMemLock lock(suites.memorySuite, commentH);
        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, py::arg("plugin_id"), py::arg("itemH"), "Get item comment.");

    sdk.def("AEGP_SetItemComment", [](uintptr_t itemH_ptr, const std::string& comment) {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetItemComment: itemH cannot be null");
        }
        // Note: comment can be empty to clear the comment

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        std::wstring utf16_comment = PyAE::StringUtils::Utf8ToUtf16(comment);

        A_Err err = suites.itemSuite->AEGP_SetItemComment(itemH, (const A_UTF16Char*)utf16_comment.c_str());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetItemComment failed");
    }, py::arg("itemH"), py::arg("comment"), "Set item comment (UNDOABLE).");

    sdk.def("AEGP_GetItemLabel", [](uintptr_t itemH_ptr) -> int {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemLabel: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_LabelID label = 0;

        A_Err err = suites.itemSuite->AEGP_GetItemLabel(itemH, &label);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemLabel failed");

        return (int)label;
    }, py::arg("itemH"), "Get item label color.");

    sdk.def("AEGP_SetItemLabel", [](uintptr_t itemH_ptr, int label) {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetItemLabel: itemH cannot be null");
        }
        PyAE::Validation::RequireRange(label, 0, 16, "label");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        A_Err err = suites.itemSuite->AEGP_SetItemLabel(itemH, static_cast<AEGP_LabelID>(label));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetItemLabel failed");
    }, py::arg("itemH"), py::arg("label"), "Set item label color (UNDOABLE).");

    sdk.def("AEGP_GetItemCurrentTime", [](uintptr_t itemH_ptr) -> double {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemCurrentTime: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        A_Time current_time;

        A_Err err = suites.itemSuite->AEGP_GetItemCurrentTime(itemH, &current_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemCurrentTime failed");

        return PyAE::AETypeUtils::TimeToSeconds(current_time);
    }, py::arg("itemH"), "Get item current time (not updated while rendering).");

    sdk.def("AEGP_SetItemCurrentTime", [](uintptr_t itemH_ptr, double time_seconds) {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetItemCurrentTime: itemH cannot be null");
        }
        PyAE::Validation::RequireNonNegative(time_seconds, "time_seconds");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        // Get item type to determine fps
        AEGP_ItemType item_type;
        A_Err err = suites.itemSuite->AEGP_GetItemType(itemH, &item_type);
        if (err != A_Err_NONE) throw std::runtime_error("Failed to get item type");

        double fps = 30.0;
        if (item_type == AEGP_ItemType_COMP) {
            AEGP_CompH compH = nullptr;
            err = suites.compSuite->AEGP_GetCompFromItem(itemH, &compH);
            if (err == A_Err_NONE && compH) {
                A_FpLong comp_fps;
                err = suites.compSuite->AEGP_GetCompFramerate(compH, &comp_fps);
                if (err == A_Err_NONE) fps = comp_fps;
            }
        }

        A_Time new_time = PyAE::AETypeUtils::SecondsToTimeWithFps(time_seconds, fps);
        err = suites.itemSuite->AEGP_SetItemCurrentTime(itemH, &new_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetItemCurrentTime failed");
    }, py::arg("itemH"), py::arg("time_seconds"), "Set item current time (UNDOABLE).");

    sdk.def("AEGP_SetItemUseProxy", [](uintptr_t itemH_ptr, bool use_proxy) {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetItemUseProxy: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        A_Err err = suites.itemSuite->AEGP_SetItemUseProxy(itemH, use_proxy ? TRUE : FALSE);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetItemUseProxy failed");
    }, py::arg("itemH"), py::arg("use_proxy"), "Set whether to use proxy (UNDOABLE). Error if item has no proxy.");

    sdk.def("AEGP_CreateNewFolder", [](const std::string& name, uintptr_t parent_folderH_ptr) -> uintptr_t {
        PyAE::Validation::RequireNonEmpty(name, "name");
        // Note: parent_folderH_ptr can be 0 to create at root

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        std::wstring utf16_name = PyAE::StringUtils::Utf8ToUtf16(name);
        AEGP_ItemH parent_folderH = reinterpret_cast<AEGP_ItemH>(parent_folderH_ptr);
        AEGP_ItemH new_folderH = nullptr;

        A_Err err = suites.itemSuite->AEGP_CreateNewFolder(
            (const A_UTF16Char*)utf16_name.c_str(),
            parent_folderH,
            &new_folderH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateNewFolder failed");

        return reinterpret_cast<uintptr_t>(new_folderH);
    }, py::arg("name"), py::arg("parent_folderH"), "Create new folder. Pass 0 for parent to create at root.");

    sdk.def("AEGP_GetTypeName", [](int item_type) -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        A_char name[AEGP_MAX_TYPE_NAME_SIZE] = {0};
        A_Err err = suites.itemSuite->AEGP_GetTypeName(static_cast<AEGP_ItemType>(item_type), name);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetTypeName failed");

        return PyAE::StringUtils::LocalToUtf8(name);
    }, py::arg("item_type"), "Get type name string from item type constant.");

    sdk.def("AEGP_GetItemMRUView", [](uintptr_t itemH_ptr) -> uintptr_t {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemMRUView: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemSuite) throw std::runtime_error("Item Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_ItemViewP mru_viewP = nullptr;

        A_Err err = suites.itemSuite->AEGP_GetItemMRUView(itemH, &mru_viewP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemMRUView failed");

        return reinterpret_cast<uintptr_t>(mru_viewP);
    }, py::arg("itemH"), "Get most recently used view for item. Returns 0 if no view available.");
}

} // namespace SDK
} // namespace PyAE
