// PyLayer_Relations.cpp
// PyAE - Python for After Effects
// PyLayer 親レイヤー・タイプ判定実装
//
// PYAE-008b: PyLayer.cppから分割

#include "PyLayerClasses.h"
#include <functional>

namespace PyAE {

// =============================================================
// PyLayer 親レイヤー・タイプ判定実装
// =============================================================

py::object PyLayer::GetParent() const {
    if (!m_layerH) return py::none();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerH parentH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetLayerParent(m_layerH, &parentH);
    if (err != A_Err_NONE || !parentH) {
        return py::none();
    }

    return py::cast(PyLayer(parentH));
}

void PyLayer::SetParent(py::object parent) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerH parentH = nullptr;
    if (!parent.is_none()) {
        PyLayer parentLayer = parent.cast<PyLayer>();
        parentH = parentLayer.GetHandle();

        // Check 1: Cannot set self as parent
        if (parentH == m_layerH) {
            throw std::runtime_error("Cannot set a layer as its own parent");
        }

        // Check 2: Detect circular reference - parent's ancestors include this layer
        AEGP_LayerH ancestorH = parentH;
        while (ancestorH) {
            AEGP_LayerH nextAncestorH = nullptr;
            A_Err checkErr = suites.layerSuite->AEGP_GetLayerParent(ancestorH, &nextAncestorH);
            if (checkErr != A_Err_NONE || !nextAncestorH) {
                break;
            }
            if (nextAncestorH == m_layerH) {
                throw std::runtime_error("Cannot set a layer's ancestor as its parent (circular reference)");
            }
            ancestorH = nextAncestorH;
        }

        // Check 3: Detect circular reference - this layer's descendants include the parent
        // Helper function to check if targetH is a descendant of currentH
        std::function<bool(AEGP_LayerH, AEGP_LayerH)> isDescendant =
            [&suites, &isDescendant](AEGP_LayerH currentH, AEGP_LayerH targetH) -> bool {
            // Get composition and iterate through all layers to find children
            AEGP_CompH compH = nullptr;
            A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(currentH, &compH);
            if (err != A_Err_NONE || !compH) return false;

            A_long numLayers = 0;
            err = suites.layerSuite->AEGP_GetCompNumLayers(compH, &numLayers);
            if (err != A_Err_NONE) return false;

            for (A_long i = 0; i < numLayers; ++i) {
                AEGP_LayerH childLayerH = nullptr;
                err = suites.layerSuite->AEGP_GetCompLayerByIndex(compH, i, &childLayerH);
                if (err != A_Err_NONE || !childLayerH) continue;

                // Check if this layer's parent is currentH
                AEGP_LayerH layerParentH = nullptr;
                err = suites.layerSuite->AEGP_GetLayerParent(childLayerH, &layerParentH);
                if (err == A_Err_NONE && layerParentH == currentH) {
                    // This layer is a child of currentH
                    if (childLayerH == targetH) {
                        return true; // Found target as direct child
                    }
                    // Recursively check descendants
                    if (isDescendant(childLayerH, targetH)) {
                        return true;
                    }
                }
            }
            return false;
        };

        if (isDescendant(m_layerH, parentH)) {
            throw std::runtime_error("Cannot set a layer's descendant as its parent (circular reference)");
        }
    }

    A_Err err = suites.layerSuite->AEGP_SetLayerParent(m_layerH, parentH);
    if (err != A_Err_NONE) {
        // Error 26: AEGP_Err_CANT_SET_LAYER_PARENT_TO_CHILD
        if (err == 26) {
            throw std::runtime_error("Cannot set a layer's descendant as its parent (circular reference detected by After Effects)");
        }
        throw std::runtime_error("Failed to set parent layer (error code: " + std::to_string(err) + ")");
    }
}

LayerType PyLayer::GetLayerType() const {
    if (!m_layerH) return LayerType::None;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_ObjectType objType;
    A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(m_layerH, &objType);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerObjectType failed");
    }

    switch (objType) {
        case AEGP_ObjectType_AV:     return LayerType::AV;
        case AEGP_ObjectType_LIGHT:  return LayerType::Light;
        case AEGP_ObjectType_CAMERA: return LayerType::Camera;
        case AEGP_ObjectType_TEXT:   return LayerType::Text;
        case AEGP_ObjectType_VECTOR: return LayerType::Shape;
        default:                     return LayerType::AV;
    }
}

bool PyLayer::IsAdjustmentLayer() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerFlags flags;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerFlags failed");
    }

    return (flags & AEGP_LayerFlag_ADJUSTMENT_LAYER) != 0;
}

bool PyLayer::IsNullLayer() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // AEGP_IsLayerNull is not available in the current SDK.
    // As a workaround, check if this is an AV layer and its source item
    // is a solid with 100x100 dimensions (default null object size).
    AEGP_ObjectType objType;
    A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(m_layerH, &objType);
    if (err != A_Err_NONE || objType != AEGP_ObjectType_AV) {
        return false;
    }

    // Get source item and check if it's a solid
    AEGP_ItemH sourceItemH;
    err = suites.layerSuite->AEGP_GetLayerSourceItem(m_layerH, &sourceItemH);
    if (err != A_Err_NONE || !sourceItemH) {
        return false;
    }

    // Check item dimensions - null objects are typically 100x100
    A_long width, height;
    err = suites.itemSuite->AEGP_GetItemDimensions(sourceItemH, &width, &height);
    if (err == A_Err_NONE && width == 100 && height == 100) {
        // This is likely a null object (100x100 default size)
        return true;
    }

    return false;
}

bool PyLayer::Is3DLayer() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean is3D;
    A_Err err = suites.layerSuite->AEGP_IsLayer3D(m_layerH, &is3D);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_IsLayer3D failed");
    }

    return is3D != FALSE;
}

void PyLayer::SetAdjustmentLayer(bool value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_ADJUSTMENT_LAYER, value ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set adjustment layer flag");
    }
}

void PyLayer::Set3DLayer(bool value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_LAYER_IS_3D, value ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set 3D layer flag");
    }
}

bool PyLayer::IsTimeRemappingEnabled() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerFlags flags;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerFlags failed");
    }

    return (flags & AEGP_LayerFlag_TIME_REMAPPING) != 0;
}

void PyLayer::SetTimeRemappingEnabled(bool value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_TIME_REMAPPING, value ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set time remapping flag");
    }
}

bool PyLayer::GetSelected() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get the composition that this layer belongs to
    AEGP_CompH compH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(m_layerH, &compH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerParentComp failed");
    }
    if (!compH) {
        return false;
    }

    if (!suites.collectionSuite) {
        throw std::runtime_error("Collection suite not available");
    }

    // Get the collection of selected layers in the composition
    // Note: AEGP_GetNewCollectionFromCompSelection creates a new collection,
    // so we don't need to call AEGP_NewCollection first
    AEGP_Collection2H collectionH = nullptr;
    err = suites.compSuite->AEGP_GetNewCollectionFromCompSelection(
        state.GetPluginID(), compH, &collectionH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetNewCollectionFromCompSelection failed");
    }

    // Get the number of selected layers
    A_u_long numSelected = 0;
    err = suites.collectionSuite->AEGP_GetCollectionNumItems(collectionH, &numSelected);
    if (err != A_Err_NONE) {
        suites.collectionSuite->AEGP_DisposeCollection(collectionH);
        throw std::runtime_error("AEGP_GetCollectionNumItems failed");
    }

    // Check if this layer is in the selected collection
    bool isSelected = false;
    for (A_u_long i = 0; i < numSelected; ++i) {
        AEGP_CollectionItemV2 item;
        err = suites.collectionSuite->AEGP_GetCollectionItemByIndex(
            collectionH, i, &item);
        if (err == A_Err_NONE &&
            item.type == AEGP_CollectionItemType_LAYER &&
            item.u.layer.layerH == m_layerH) {
            isSelected = true;
            break;
        }
    }

    suites.collectionSuite->AEGP_DisposeCollection(collectionH);
    return isSelected;
}

void PyLayer::SetSelected(bool value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get the composition that this layer belongs to
    AEGP_CompH compH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(m_layerH, &compH);
    if (err != A_Err_NONE || !compH) {
        throw std::runtime_error("Failed to get parent composition");
    }

    if (!suites.collectionSuite) {
        throw std::runtime_error("Collection suite not available");
    }

    // Create a collection containing only this layer
    AEGP_Collection2H collectionH;
    err = suites.collectionSuite->AEGP_NewCollection(state.GetPluginID(), &collectionH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to create collection");
    }

    if (value) {
        // Add this layer to the collection using CollectionPushBack
        AEGP_CollectionItemV2 item;
        item.type = AEGP_CollectionItemType_LAYER;
        item.u.layer.layerH = m_layerH;
        item.stream_refH = nullptr;  // Not needed for layer selection

        err = suites.collectionSuite->AEGP_CollectionPushBack(collectionH, &item);
        if (err != A_Err_NONE) {
            suites.collectionSuite->AEGP_DisposeCollection(collectionH);
            throw std::runtime_error("Failed to add layer to collection");
        }
    }

    // Set the composition selection to this collection
    // If value is false, we're passing an empty collection which deselects all
    err = suites.compSuite->AEGP_SetSelection(compH, collectionH);

    suites.collectionSuite->AEGP_DisposeCollection(collectionH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer selection");
    }
}

} // namespace PyAE
