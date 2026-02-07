#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_CollectionSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_CollectionSuite
    // -----------------------------------------------------------------------
    
    sdk.def("AEGP_NewCollection", [](int plugin_id) -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.collectionSuite) throw std::runtime_error("Collection Suite not available");

        AEGP_Collection2H collectionH = nullptr;
        
        A_Err err = suites.collectionSuite->AEGP_NewCollection(plugin_id, &collectionH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NewCollection failed");
        
        return reinterpret_cast<uintptr_t>(collectionH);
    }, py::arg("plugin_id"));

    sdk.def("AEGP_DisposeCollection", [](uintptr_t collectionH_ptr) {
        // Validate: NULL pointer check
        if (collectionH_ptr == 0) {
            throw std::invalid_argument("collectionH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.collectionSuite) throw std::runtime_error("Collection Suite not available");

        AEGP_Collection2H collectionH = reinterpret_cast<AEGP_Collection2H>(collectionH_ptr);

        A_Err err = suites.collectionSuite->AEGP_DisposeCollection(collectionH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisposeCollection failed");
    }, py::arg("collectionH"));

    sdk.def("AEGP_GetCollectionNumItems", [](uintptr_t collectionH_ptr) -> int {
        // Validate: NULL pointer check
        if (collectionH_ptr == 0) {
            throw std::invalid_argument("collectionH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.collectionSuite) throw std::runtime_error("Collection Suite not available");

        AEGP_Collection2H collectionH = reinterpret_cast<AEGP_Collection2H>(collectionH_ptr);
        A_u_long num_items = 0;

        A_Err err = suites.collectionSuite->AEGP_GetCollectionNumItems(collectionH, &num_items);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCollectionNumItems failed");

        return (int)num_items;
    }, py::arg("collectionH"));

    sdk.def("AEGP_GetCollectionItemByIndex", [](uintptr_t collectionH_ptr, int index) -> py::dict {
        // Validate: NULL pointer check
        if (collectionH_ptr == 0) {
            throw std::invalid_argument("collectionH cannot be null");
        }
        // Validate: index must be non-negative
        Validation::RequireNonNegative(index, "index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.collectionSuite) throw std::runtime_error("Collection Suite not available");

        AEGP_Collection2H collectionH = reinterpret_cast<AEGP_Collection2H>(collectionH_ptr);

        // Range check - get number of items first
        A_u_long num_items = 0;
        A_Err err = suites.collectionSuite->AEGP_GetCollectionNumItems(collectionH, &num_items);
        if (err != A_Err_NONE) throw std::runtime_error("Failed to get collection item count");

        // Validate: index must be within range
        Validation::RequireValidIndex(index, static_cast<int>(num_items), "Collection item");

        AEGP_CollectionItemV2 item;

        err = suites.collectionSuite->AEGP_GetCollectionItemByIndex(collectionH, (A_u_long)index, &item);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCollectionItemByIndex failed");
        
        py::dict result;
        result["type"] = (int)item.type;
        result["stream_refH"] = reinterpret_cast<uintptr_t>(item.stream_refH);
        
        // Return basic info - detailed extraction can be done via stream_refH
        switch (item.type) {
            case AEGP_CollectionItemType_LAYER:
                result["layerH"] = reinterpret_cast<uintptr_t>(item.u.layer.layerH);
                break;
            case AEGP_CollectionItemType_MASK:
                result["layerH"] = reinterpret_cast<uintptr_t>(item.u.mask.layerH);
                result["mask_index"] = (int)item.u.mask.index;
                break;
            case AEGP_CollectionItemType_EFFECT:
                result["layerH"] = reinterpret_cast<uintptr_t>(item.u.effect.layerH);
                result["effect_index"] = (int)item.u.effect.index;
                break;
            default:
                break;
        }
        
        return result;
    }, py::arg("collectionH"), py::arg("index"));

    sdk.def("AEGP_CollectionPushBack", [](uintptr_t collectionH_ptr, py::dict item_dict) {
        // Validate: NULL pointer check
        if (collectionH_ptr == 0) {
            throw std::invalid_argument("collectionH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.collectionSuite) throw std::runtime_error("Collection Suite not available");

        AEGP_Collection2H collectionH = reinterpret_cast<AEGP_Collection2H>(collectionH_ptr);
        AEGP_CollectionItemV2 item;
        memset(&item, 0, sizeof(item));
        
        item.type = (AEGP_CollectionItemType)item_dict["type"].cast<int>();
        
        if (item_dict.contains("stream_refH")) {
            item.stream_refH = reinterpret_cast<AEGP_StreamRefH>(item_dict["stream_refH"].cast<uintptr_t>());
        }
        
        switch (item.type) {
            case AEGP_CollectionItemType_LAYER:
                item.u.layer.layerH = reinterpret_cast<AEGP_LayerH>(item_dict["layerH"].cast<uintptr_t>());
                break;
            case AEGP_CollectionItemType_MASK:
                item.u.mask.layerH = reinterpret_cast<AEGP_LayerH>(item_dict["layerH"].cast<uintptr_t>());
                item.u.mask.index = (AEGP_MaskIndex)item_dict["mask_index"].cast<int>();
                break;
            case AEGP_CollectionItemType_EFFECT:
                item.u.effect.layerH = reinterpret_cast<AEGP_LayerH>(item_dict["layerH"].cast<uintptr_t>());
                item.u.effect.index = (AEGP_EffectIndex)item_dict["effect_index"].cast<int>();
                break;
            default:
                throw std::runtime_error("Unsupported collection item type");
        }
        
        A_Err err = suites.collectionSuite->AEGP_CollectionPushBack(collectionH, &item);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CollectionPushBack failed");
    }, py::arg("collectionH"), py::arg("item"));

    sdk.def("AEGP_CollectionErase", [](uintptr_t collectionH_ptr, int index_first, int index_last) {
        // Validate: NULL pointer check
        if (collectionH_ptr == 0) {
            throw std::invalid_argument("collectionH cannot be null");
        }
        // Validate: indices must be non-negative
        Validation::RequireNonNegative(index_first, "index_first");
        Validation::RequireNonNegative(index_last, "index_last");
        // Validate: index_first must be <= index_last
        if (index_first > index_last) {
            throw std::invalid_argument("index_first must be <= index_last");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.collectionSuite) throw std::runtime_error("Collection Suite not available");

        AEGP_Collection2H collectionH = reinterpret_cast<AEGP_Collection2H>(collectionH_ptr);

        A_Err err = suites.collectionSuite->AEGP_CollectionErase(
            collectionH,
            (A_u_long)index_first,
            (A_u_long)index_last
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CollectionErase failed");
    }, py::arg("collectionH"), py::arg("index_first"), py::arg("index_last"));

    // Collection Item Type Constants
    sdk.attr("AEGP_CollectionItemType_NONE") = (int)AEGP_CollectionItemType_NONE;
    sdk.attr("AEGP_CollectionItemType_LAYER") = (int)AEGP_CollectionItemType_LAYER;
    sdk.attr("AEGP_CollectionItemType_MASK") = (int)AEGP_CollectionItemType_MASK;
    sdk.attr("AEGP_CollectionItemType_EFFECT") = (int)AEGP_CollectionItemType_EFFECT;
    sdk.attr("AEGP_CollectionItemType_STREAM") = (int)AEGP_CollectionItemType_STREAM;
    sdk.attr("AEGP_CollectionItemType_MASK_VERTEX") = (int)AEGP_CollectionItemType_MASK_VERTEX;
    sdk.attr("AEGP_CollectionItemType_KEYFRAME") = (int)AEGP_CollectionItemType_KEYFRAME;
}

} // namespace SDK
} // namespace PyAE
