#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"
#include <functional>

// AE SDK Headers
#include "AE_GeneralPlug.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_LayerSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_LayerSuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetCompNumLayers", [](uintptr_t compH_ptr) -> int {
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompNumLayers: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_long num_layers = 0;
        A_Err err = suites.layerSuite->AEGP_GetCompNumLayers(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &num_layers);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompNumLayers failed with error: " + std::to_string(err));

        return (int)num_layers;
    }, py::arg("compH"));

    sdk.def("AEGP_GetCompLayerByIndex", [](uintptr_t compH_ptr, int index) -> uintptr_t {
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompLayerByIndex: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_CompH compH = reinterpret_cast<AEGP_CompH>(compH_ptr);

        // Range check - get number of layers first
        A_long num_layers = 0;
        A_Err err = suites.layerSuite->AEGP_GetCompNumLayers(compH, &num_layers);
        if (err != A_Err_NONE) throw std::runtime_error("Failed to get comp layer count with error: " + std::to_string(err));

        // Check bounds (0-based indexing)
        if (index < 0 || index >= num_layers) {
            throw std::out_of_range("Layer index out of range: " + std::to_string(index) +
                                   " (valid range: 0-" + std::to_string(num_layers - 1) + ")");
        }

        AEGP_LayerH layerH = nullptr;
        err = suites.layerSuite->AEGP_GetCompLayerByIndex(compH, index, &layerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompLayerByIndex failed");

        return reinterpret_cast<uintptr_t>(layerH);
    }, py::arg("compH"), py::arg("index"));

    sdk.def("AEGP_GetLayerName", [](int plugin_id, uintptr_t layerH_ptr) -> std::string {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerName: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_MemHandle nameH = nullptr;
        AEGP_MemHandle sourceNameH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerName(
            plugin_id, reinterpret_cast<AEGP_LayerH>(layerH_ptr), &nameH, &sourceNameH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerName failed");
        if (!nameH) return "";

        if (sourceNameH) suites.memorySuite->AEGP_FreeMemHandle(sourceNameH);

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, nameH);
        PyAE::ScopedMemLock lock(suites.memorySuite, nameH);
        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, py::arg("plugin_id"), py::arg("layerH"));

    sdk.def("AEGP_GetLayerIndex", [](uintptr_t layerH_ptr) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerIndex: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_long index = 0;
        A_Err err = suites.layerSuite->AEGP_GetLayerIndex(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &index);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerIndex failed");

        return (int)index;
    }, py::arg("layerH"));

    sdk.def("AEGP_GetLayerSourceItem", [](uintptr_t layerH_ptr) -> uintptr_t {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerSourceItem: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_ItemH itemH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerSourceItem(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerSourceItem failed");

        return reinterpret_cast<uintptr_t>(itemH);
    }, py::arg("layerH"));

    sdk.def("AEGP_GetLayerParent", [](uintptr_t layerH_ptr) -> uintptr_t {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerParent: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH parentH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParent(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &parentH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerParent failed");

        return reinterpret_cast<uintptr_t>(parentH);
    }, py::arg("layerH"));

    sdk.def("AEGP_SetLayerParent", [](uintptr_t layerH_ptr, uintptr_t parentH_ptr) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetLayerParent: layerH cannot be null");
        }
        // Note: parentH_ptr can be 0 (null) to remove parent

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
        AEGP_LayerH parentH = reinterpret_cast<AEGP_LayerH>(parentH_ptr);

        // Circular reference checks (only if parent is not null)
        if (parentH != nullptr) {
            // Check 1: Cannot set self as parent
            if (parentH == layerH) {
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
                if (nextAncestorH == layerH) {
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

            if (isDescendant(layerH, parentH)) {
                throw std::runtime_error("Cannot set a layer's descendant as its parent (circular reference)");
            }
        }

        A_Err err = suites.layerSuite->AEGP_SetLayerParent(layerH, parentH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLayerParent failed");
    }, py::arg("layerH"), py::arg("parentLayerH"));

    sdk.def("AEGP_GetLayerFlags", [](uintptr_t layerH_ptr) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerFlags: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerFlags flags = 0;
        A_Err err = suites.layerSuite->AEGP_GetLayerFlags(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &flags);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerFlags failed");

        return (int)flags;
    }, py::arg("layerH"));

    sdk.def("AEGP_SetLayerFlag", [](uintptr_t layerH_ptr, int flag, int value) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetLayerFlag: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr),
            (AEGP_LayerFlags)flag,
            (A_Boolean)value);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLayerFlag failed");
    }, py::arg("layerH"), py::arg("flag"), py::arg("value"));

    sdk.def("AEGP_ReorderLayer", [](uintptr_t layerH_ptr, int index) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_ReorderLayer: layerH cannot be null");
        }
        Validation::RequireNonNegative(index, "index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Err err = suites.layerSuite->AEGP_ReorderLayer(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), (A_long)index);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ReorderLayer failed");
    }, py::arg("layerH"), py::arg("index"));

    // -----------------------------------------------------------------------
    // AEGP_LayerSuite9 - Phase 1: CRITICAL Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetLayerParentComp", [](uintptr_t layerH_ptr) -> uintptr_t {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerParentComp: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_CompH compH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &compH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerParentComp failed");

        return reinterpret_cast<uintptr_t>(compH);
    }, py::arg("layerH"));

    sdk.def("AEGP_GetActiveLayer", []() -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetActiveLayer(&layerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetActiveLayer failed");

        // Return 0 if no layer is active (valid non-error condition)
        return reinterpret_cast<uintptr_t>(layerH);
    });

    sdk.def("AEGP_GetLayerID", [](uintptr_t layerH_ptr) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerID: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerIDVal id = 0;
        A_Err err = suites.layerSuite->AEGP_GetLayerID(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &id);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerID failed");

        return (int)id;
    }, py::arg("layerH"));

    sdk.def("AEGP_GetLayerFromLayerID", [](uintptr_t compH_ptr, int layer_id) -> uintptr_t {
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerFromLayerID: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerFromLayerID(
            reinterpret_cast<AEGP_CompH>(compH_ptr), (AEGP_LayerIDVal)layer_id, &layerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerFromLayerID failed");

        return reinterpret_cast<uintptr_t>(layerH);
    }, py::arg("compH"), py::arg("layer_id"));

    sdk.def("AEGP_IsLayer3D", [](uintptr_t layerH_ptr) -> bool {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsLayer3D: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Boolean is_3d = FALSE;
        A_Err err = suites.layerSuite->AEGP_IsLayer3D(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &is_3d);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsLayer3D failed");

        return is_3d == TRUE;
    }, py::arg("layerH"));

    sdk.def("AEGP_IsLayer2D", [](uintptr_t layerH_ptr) -> bool {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsLayer2D: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Boolean is_2d = FALSE;
        A_Err err = suites.layerSuite->AEGP_IsLayer2D(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &is_2d);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsLayer2D failed");

        return is_2d == TRUE;
    }, py::arg("layerH"));

    // -----------------------------------------------------------------------
    // AEGP_LayerSuite9 - Phase 2: HIGH PRIORITY Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetLayerStretch", [](uintptr_t layerH_ptr) -> py::tuple {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerStretch: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Ratio stretch = {0, 1};
        A_Err err = suites.layerSuite->AEGP_GetLayerStretch(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &stretch);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerStretch failed");

        return py::make_tuple(stretch.num, stretch.den);
    }, py::arg("layerH"));

    sdk.def("AEGP_SetLayerStretch", [](uintptr_t layerH_ptr, int num, int den) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetLayerStretch: layerH cannot be null");
        }
        Validation::RequireNonZero(den, "den");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Ratio stretch;
        stretch.num = num;
        stretch.den = (A_u_long)den;

        A_Err err = suites.layerSuite->AEGP_SetLayerStretch(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &stretch);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLayerStretch failed");
    }, py::arg("layerH"), py::arg("num"), py::arg("den"));

    sdk.def("AEGP_GetLayerTransferMode", [](uintptr_t layerH_ptr) -> py::dict {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerTransferMode: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerTransferMode transfer_mode;
        A_Err err = suites.layerSuite->AEGP_GetLayerTransferMode(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &transfer_mode);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerTransferMode failed");

        py::dict result;
        result["mode"] = (int)transfer_mode.mode;
        result["flags"] = (int)transfer_mode.flags;
        result["track_matte"] = (int)transfer_mode.track_matte;
        return result;
    }, py::arg("layerH"));

    sdk.def("AEGP_SetLayerTransferMode", [](uintptr_t layerH_ptr, int mode, int flags, int track_matte) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetLayerTransferMode: layerH cannot be null");
        }
        // PF_Xfer_NONE = 0, PF_Xfer_NUM_MODES - 1 is the max (approximately 39 modes)
        Validation::RequireRange(mode, 0, 50, "mode");
        // AEGP_TrackMatte values: 0-4
        Validation::RequireRange(track_matte, 0, 4, "track_matte");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerTransferMode transfer_mode;
        transfer_mode.mode = (PF_TransferMode)mode;
        transfer_mode.flags = (AEGP_TransferFlags)flags;
        transfer_mode.track_matte = (AEGP_TrackMatte)track_matte;

        A_Err err = suites.layerSuite->AEGP_SetLayerTransferMode(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &transfer_mode);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLayerTransferMode failed");
    }, py::arg("layerH"), py::arg("mode"), py::arg("flags"), py::arg("track_matte"));

    sdk.def("AEGP_GetLayerQuality", [](uintptr_t layerH_ptr) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerQuality: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerQuality quality = AEGP_LayerQual_NONE;
        A_Err err = suites.layerSuite->AEGP_GetLayerQuality(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &quality);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerQuality failed");

        return (int)quality;
    }, py::arg("layerH"));

    sdk.def("AEGP_SetLayerQuality", [](uintptr_t layerH_ptr, int quality) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetLayerQuality: layerH cannot be null");
        }
        // AEGP_LayerQual_NONE=0, AEGP_LayerQual_WIREFRAME=1, AEGP_LayerQual_DRAFT=2, AEGP_LayerQual_BEST=3
        Validation::RequireRange(quality, 0, 3, "quality");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Err err = suites.layerSuite->AEGP_SetLayerQuality(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), (AEGP_LayerQuality)quality);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLayerQuality failed");
    }, py::arg("layerH"), py::arg("quality"));

    sdk.def("AEGP_GetLayerSamplingQuality", [](uintptr_t layerH_ptr) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerSamplingQuality: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerSamplingQuality quality;
        A_Err err = suites.layerSuite->AEGP_GetLayerSamplingQuality(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &quality);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerSamplingQuality failed");

        return (int)quality;
    }, py::arg("layerH"));

    sdk.def("AEGP_SetLayerSamplingQuality", [](uintptr_t layerH_ptr, int quality) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetLayerSamplingQuality: layerH cannot be null");
        }
        // AEGP_LayerSamplingQual_BILINEAR=0, AEGP_LayerSamplingQual_BICUBIC=1
        Validation::RequireRange(quality, 0, 1, "quality");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Err err = suites.layerSuite->AEGP_SetLayerSamplingQuality(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), (AEGP_LayerSamplingQuality)quality);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLayerSamplingQuality failed");
    }, py::arg("layerH"), py::arg("quality"));

    sdk.def("AEGP_GetLayerLabel", [](uintptr_t layerH_ptr) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerLabel: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LabelID label = AEGP_Label_NONE;
        A_Err err = suites.layerSuite->AEGP_GetLayerLabel(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &label);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerLabel failed");

        return (int)label;
    }, py::arg("layerH"));

    sdk.def("AEGP_SetLayerLabel", [](uintptr_t layerH_ptr, int label) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetLayerLabel: layerH cannot be null");
        }
        // AE has 17 labels (0-16): AEGP_Label_NONE=0 to AEGP_Label_16=16
        Validation::RequireRange(label, 0, 16, "label");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Err err = suites.layerSuite->AEGP_SetLayerLabel(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), (AEGP_LabelID)label);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLayerLabel failed");
    }, py::arg("layerH"), py::arg("label"));

    // Note: AEGP_GetNewLayerStream and AEGP_DisposeStream are defined in StreamSuite.cpp
    // to avoid duplicate registration

    // -----------------------------------------------------------------------
    // AEGP_LayerSuite9 - Phase 3: MEDIUM PRIORITY Functions
    // -----------------------------------------------------------------------

    // Time conversion functions
    sdk.def("AEGP_ConvertCompToLayerTime", [](uintptr_t layerH_ptr, double comp_time_seconds) -> double {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_ConvertCompToLayerTime: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Get comp handle to get framerate
        AEGP_CompH compH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(layerH, &compH);
        if (err != A_Err_NONE || !compH) throw std::runtime_error("Failed to get parent comp");

        A_FpLong fps = 30.0;
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time comp_time = AETypeUtils::SecondsToTimeWithFps(comp_time_seconds, fps);
        A_Time layer_time = {0, 1};

        err = suites.layerSuite->AEGP_ConvertCompToLayerTime(layerH, &comp_time, &layer_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ConvertCompToLayerTime failed");

        return AETypeUtils::TimeToSeconds(layer_time);
    }, py::arg("layerH"), py::arg("comp_time"));

    sdk.def("AEGP_ConvertLayerToCompTime", [](uintptr_t layerH_ptr, double layer_time_seconds) -> double {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_ConvertLayerToCompTime: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Get comp handle to get framerate
        AEGP_CompH compH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(layerH, &compH);
        if (err != A_Err_NONE || !compH) throw std::runtime_error("Failed to get parent comp");

        A_FpLong fps = 30.0;
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time layer_time = AETypeUtils::SecondsToTimeWithFps(layer_time_seconds, fps);
        A_Time comp_time = {0, 1};

        err = suites.layerSuite->AEGP_ConvertLayerToCompTime(layerH, &layer_time, &comp_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ConvertLayerToCompTime failed");

        return AETypeUtils::TimeToSeconds(comp_time);
    }, py::arg("layerH"), py::arg("layer_time"));

    // Layer information functions
    sdk.def("AEGP_GetLayerSourceItemID", [](uintptr_t layerH_ptr) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerSourceItemID: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_long source_item_id = 0;
        A_Err err = suites.layerSuite->AEGP_GetLayerSourceItemID(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &source_item_id);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerSourceItemID failed");

        return static_cast<int>(source_item_id);
    }, py::arg("layerH"));

    sdk.def("AEGP_GetLayerCurrentTime", [](uintptr_t layerH_ptr, int time_mode) -> double {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerCurrentTime: layerH cannot be null");
        }
        // AEGP_LTimeMode_CompTime=0, AEGP_LTimeMode_LayerTime=1
        Validation::RequireRange(time_mode, 0, 1, "time_mode");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Time curr_time = {0, 1};
        A_Err err = suites.layerSuite->AEGP_GetLayerCurrentTime(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr),
            static_cast<AEGP_LTimeMode>(time_mode),
            &curr_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerCurrentTime failed");

        return AETypeUtils::TimeToSeconds(curr_time);
    }, py::arg("layerH"), py::arg("time_mode"));

    sdk.def("AEGP_GetLayerObjectType", [](uintptr_t layerH_ptr) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerObjectType: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_ObjectType object_type;
        A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &object_type);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerObjectType failed");

        return static_cast<int>(object_type);
    }, py::arg("layerH"));

    sdk.def("AEGP_GetLayerMaskedBounds", [](uintptr_t layerH_ptr, int time_mode, double time_seconds) -> py::tuple {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerMaskedBounds: layerH cannot be null");
        }
        // AEGP_LTimeMode_CompTime=0, AEGP_LTimeMode_LayerTime=1
        Validation::RequireRange(time_mode, 0, 1, "time_mode");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Get comp handle to get framerate
        AEGP_CompH compH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(layerH, &compH);
        if (err != A_Err_NONE || !compH) throw std::runtime_error("Failed to get parent comp");

        A_FpLong fps = 30.0;
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time time = AETypeUtils::SecondsToTimeWithFps(time_seconds, fps);
        A_FloatRect bounds = {0, 0, 0, 0};

        err = suites.layerSuite->AEGP_GetLayerMaskedBounds(
            layerH, static_cast<AEGP_LTimeMode>(time_mode), &time, &bounds);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerMaskedBounds failed");

        return py::make_tuple(bounds.left, bounds.top, bounds.right, bounds.bottom);
    }, py::arg("layerH"), py::arg("time_mode"), py::arg("time"));

    sdk.def("AEGP_GetLayerDancingRandValue", [](uintptr_t layerH_ptr, double comp_time_seconds) -> int {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerDancingRandValue: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Get comp handle to get framerate
        AEGP_CompH compH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(layerH, &compH);
        if (err != A_Err_NONE || !compH) throw std::runtime_error("Failed to get parent comp");

        A_FpLong fps = 30.0;
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time comp_time = AETypeUtils::SecondsToTimeWithFps(comp_time_seconds, fps);
        A_long rand_value = 0;

        err = suites.layerSuite->AEGP_GetLayerDancingRandValue(layerH, &comp_time, &rand_value);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerDancingRandValue failed");

        return static_cast<int>(rand_value);
    }, py::arg("layerH"), py::arg("comp_time"));

    sdk.def("AEGP_IsLayerVideoReallyOn", [](uintptr_t layerH_ptr) -> bool {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsLayerVideoReallyOn: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Boolean is_on = FALSE;
        A_Err err = suites.layerSuite->AEGP_IsLayerVideoReallyOn(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &is_on);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsLayerVideoReallyOn failed");

        return is_on != FALSE;
    }, py::arg("layerH"));

    sdk.def("AEGP_IsLayerAudioReallyOn", [](uintptr_t layerH_ptr) -> bool {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsLayerAudioReallyOn: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Boolean is_on = FALSE;
        A_Err err = suites.layerSuite->AEGP_IsLayerAudioReallyOn(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &is_on);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsLayerAudioReallyOn failed");

        return is_on != FALSE;
    }, py::arg("layerH"));

    sdk.def("AEGP_IsVideoActive", [](uintptr_t layerH_ptr, int time_mode, double time_seconds) -> bool {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsVideoActive: layerH cannot be null");
        }
        // AEGP_LTimeMode_CompTime=0, AEGP_LTimeMode_LayerTime=1
        Validation::RequireRange(time_mode, 0, 1, "time_mode");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Get comp handle to get framerate
        AEGP_CompH compH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(layerH, &compH);
        if (err != A_Err_NONE || !compH) throw std::runtime_error("Failed to get parent comp");

        A_FpLong fps = 30.0;
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time time = AETypeUtils::SecondsToTimeWithFps(time_seconds, fps);
        A_Boolean is_active = FALSE;

        err = suites.layerSuite->AEGP_IsVideoActive(
            layerH, static_cast<AEGP_LTimeMode>(time_mode), &time, &is_active);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsVideoActive failed");

        return is_active != FALSE;
    }, py::arg("layerH"), py::arg("time_mode"), py::arg("time"));

    // Transform functions
    sdk.def("AEGP_GetLayerToWorldXform", [](uintptr_t layerH_ptr, double comp_time_seconds) -> py::list {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerToWorldXform: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Get comp handle to get framerate
        AEGP_CompH compH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(layerH, &compH);
        if (err != A_Err_NONE || !compH) throw std::runtime_error("Failed to get parent comp");

        A_FpLong fps = 30.0;
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time comp_time = AETypeUtils::SecondsToTimeWithFps(comp_time_seconds, fps);
        A_Matrix4 matrix;

        err = suites.layerSuite->AEGP_GetLayerToWorldXform(layerH, &comp_time, &matrix);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerToWorldXform failed");

        // Convert 4x4 matrix to Python list
        py::list result;
        for (int i = 0; i < 4; i++) {
            py::list row;
            for (int j = 0; j < 4; j++) {
                row.append(matrix.mat[i][j]);
            }
            result.append(row);
        }
        return result;
    }, py::arg("layerH"), py::arg("comp_time"));

    sdk.def("AEGP_GetLayerToWorldXformFromView", [](uintptr_t layerH_ptr, double view_time_seconds, double comp_time_seconds) -> py::list {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerToWorldXformFromView: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Get comp handle to get framerate
        AEGP_CompH compH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(layerH, &compH);
        if (err != A_Err_NONE || !compH) throw std::runtime_error("Failed to get parent comp");

        A_FpLong fps = 30.0;
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time view_time = AETypeUtils::SecondsToTimeWithFps(view_time_seconds, fps);
        A_Time comp_time = AETypeUtils::SecondsToTimeWithFps(comp_time_seconds, fps);
        A_Matrix4 matrix;

        err = suites.layerSuite->AEGP_GetLayerToWorldXformFromView(layerH, &view_time, &comp_time, &matrix);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerToWorldXformFromView failed");

        // Convert 4x4 matrix to Python list
        py::list result;
        for (int i = 0; i < 4; i++) {
            py::list row;
            for (int j = 0; j < 4; j++) {
                row.append(matrix.mat[i][j]);
            }
            result.append(row);
        }
        return result;
    }, py::arg("layerH"), py::arg("view_time"), py::arg("comp_time"));

    // Layer operation functions
    sdk.def("AEGP_IsAddLayerValid", [](uintptr_t itemH_ptr, uintptr_t compH_ptr) -> bool {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsAddLayerValid: itemH cannot be null");
        }
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsAddLayerValid: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Boolean valid = FALSE;
        A_Err err = suites.layerSuite->AEGP_IsAddLayerValid(
            reinterpret_cast<AEGP_ItemH>(itemH_ptr),
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &valid);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsAddLayerValid failed");

        return valid != FALSE;
    }, py::arg("itemH"), py::arg("compH"));

    sdk.def("AEGP_AddLayer", [](uintptr_t itemH_ptr, uintptr_t compH_ptr) -> uintptr_t {
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_AddLayer: itemH cannot be null");
        }
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_AddLayer: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH added_layerH = nullptr;
        A_Err err = suites.layerSuite->AEGP_AddLayer(
            reinterpret_cast<AEGP_ItemH>(itemH_ptr),
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &added_layerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_AddLayer failed");

        return reinterpret_cast<uintptr_t>(added_layerH);
    }, py::arg("itemH"), py::arg("compH"));

    // Track matte functions
    sdk.def("AEGP_IsLayerUsedAsTrackMatte", [](uintptr_t layerH_ptr, bool fill_must_be_active) -> bool {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsLayerUsedAsTrackMatte: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Boolean is_track_matte = FALSE;
        A_Err err = suites.layerSuite->AEGP_IsLayerUsedAsTrackMatte(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr),
            fill_must_be_active ? TRUE : FALSE,
            &is_track_matte);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsLayerUsedAsTrackMatte failed");

        return is_track_matte != FALSE;
    }, py::arg("layerH"), py::arg("fill_must_be_active"));

    sdk.def("AEGP_DoesLayerHaveTrackMatte", [](uintptr_t layerH_ptr) -> bool {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_DoesLayerHaveTrackMatte: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Boolean has_track_matte = FALSE;
        A_Err err = suites.layerSuite->AEGP_DoesLayerHaveTrackMatte(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &has_track_matte);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DoesLayerHaveTrackMatte failed");

        return has_track_matte != FALSE;
    }, py::arg("layerH"));

#if defined(kAEGPLayerSuiteVersion9)
    // TrackMatte functions are only available in LayerSuite9 (AE 23.0+)

    sdk.def("AEGP_GetTrackMatteLayer", [](uintptr_t layerH_ptr) -> uintptr_t {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetTrackMatteLayer: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        AEGP_LayerH track_matte_layerH = nullptr;
        A_Err err = suites.layerSuite->AEGP_GetTrackMatteLayer(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr), &track_matte_layerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetTrackMatteLayer failed");

        return reinterpret_cast<uintptr_t>(track_matte_layerH);
    }, py::arg("layerH"));

    sdk.def("AEGP_SetTrackMatte", [](uintptr_t layerH_ptr, uintptr_t track_matte_layerH_ptr, int track_matte_type) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetTrackMatte: layerH cannot be null");
        }
        if (track_matte_layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetTrackMatte: track_matte_layerH cannot be null");
        }
        // AEGP_TrackMatte values: 0-4
        Validation::RequireRange(track_matte_type, 0, 4, "track_matte_type");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Err err = suites.layerSuite->AEGP_SetTrackMatte(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr),
            reinterpret_cast<AEGP_LayerH>(track_matte_layerH_ptr),
            static_cast<AEGP_TrackMatte>(track_matte_type));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetTrackMatte failed");
    }, py::arg("layerH"), py::arg("track_matte_layerH"), py::arg("track_matte_type"));

    sdk.def("AEGP_RemoveTrackMatte", [](uintptr_t layerH_ptr) {
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_RemoveTrackMatte: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        A_Err err = suites.layerSuite->AEGP_RemoveTrackMatte(
            reinterpret_cast<AEGP_LayerH>(layerH_ptr));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_RemoveTrackMatte failed");
    }, py::arg("layerH"));
#endif
}

} // namespace SDK
} // namespace PyAE
