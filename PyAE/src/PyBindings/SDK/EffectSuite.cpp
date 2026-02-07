#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_EffectUI.h"
#include "AE_Effect.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_EffectSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_EffectSuite
    // -----------------------------------------------------------------------
    
    sdk.def("AEGP_GetLayerNumEffects", [](uintptr_t layerH_ptr) -> int {
        // Validate parameters
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerNumEffects: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
        A_long num_effects = 0;

        A_Err err = suites.effectSuite->AEGP_GetLayerNumEffects(layerH, &num_effects);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerNumEffects failed");

        return (int)num_effects;
    }, py::arg("layerH"));

    sdk.def("AEGP_GetLayerEffectByIndex", [](int plugin_id, uintptr_t layerH_ptr, int effect_index) -> uintptr_t {
        // Validate parameters
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerEffectByIndex: layerH cannot be null");
        }
        Validation::RequireNonNegative(effect_index, "AEGP_GetLayerEffectByIndex: effect_index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Range check - get number of effects first
        A_long num_effects = 0;
        A_Err err = suites.effectSuite->AEGP_GetLayerNumEffects(layerH, &num_effects);
        if (err != A_Err_NONE) throw std::runtime_error("Failed to get layer effect count");

        // Check bounds (0-based indexing)
        Validation::RequireValidIndex(effect_index, (int)num_effects, "AEGP_GetLayerEffectByIndex: effect");

        AEGP_EffectRefH effectH = nullptr;

        err = suites.effectSuite->AEGP_GetLayerEffectByIndex(
            plugin_id,
            layerH,
            (AEGP_EffectIndex)effect_index,
            &effectH
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerEffectByIndex failed");

        return reinterpret_cast<uintptr_t>(effectH);
    }, py::arg("plugin_id"), py::arg("layerH"), py::arg("effect_index"));

    sdk.def("AEGP_GetInstalledKeyFromLayerEffect", [](uintptr_t effectH_ptr) -> int {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetInstalledKeyFromLayerEffect: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        AEGP_InstalledEffectKey key = AEGP_InstalledEffectKey_NONE;

        A_Err err = suites.effectSuite->AEGP_GetInstalledKeyFromLayerEffect(effectH, &key);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInstalledKeyFromLayerEffect failed");

        return (int)key;
    }, py::arg("effectH"));

    sdk.def("AEGP_GetEffectFlags", [](uintptr_t effectH_ptr) -> int {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetEffectFlags: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        AEGP_EffectFlags flags = AEGP_EffectFlags_NONE;

        A_Err err = suites.effectSuite->AEGP_GetEffectFlags(effectH, &flags);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEffectFlags failed");

        return (int)flags;
    }, py::arg("effectH"));

    sdk.def("AEGP_SetEffectFlags", [](uintptr_t effectH_ptr, int mask, int flags) {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetEffectFlags: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);

        A_Err err = suites.effectSuite->AEGP_SetEffectFlags(
            effectH,
            (AEGP_EffectFlags)mask,
            (AEGP_EffectFlags)flags
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetEffectFlags failed");
    }, py::arg("effectH"), py::arg("mask"), py::arg("flags"));

    sdk.def("AEGP_ReorderEffect", [](uintptr_t effectH_ptr, int effect_index) {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_ReorderEffect: effectH cannot be null");
        }
        Validation::RequireNonNegative(effect_index, "AEGP_ReorderEffect: effect_index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);

        A_Err err = suites.effectSuite->AEGP_ReorderEffect(effectH, (A_long)effect_index);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ReorderEffect failed");
    }, py::arg("effectH"), py::arg("effect_index"));

    sdk.def("AEGP_DisposeEffect", [](uintptr_t effectH_ptr) {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_DisposeEffect: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);

        A_Err err = suites.effectSuite->AEGP_DisposeEffect(effectH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisposeEffect failed");
    }, py::arg("effectH"));

    sdk.def("AEGP_ApplyEffect", [](int plugin_id, uintptr_t layerH_ptr, int installed_key) -> uintptr_t {
        // Validate parameters
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_ApplyEffect: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
        AEGP_EffectRefH effectH = nullptr;

        A_Err err = suites.effectSuite->AEGP_ApplyEffect(plugin_id, layerH, (AEGP_InstalledEffectKey)installed_key, &effectH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ApplyEffect failed");

        return reinterpret_cast<uintptr_t>(effectH);
    }, py::arg("plugin_id"), py::arg("layerH"), py::arg("installed_key"));

    // Effect Discovery
    sdk.def("AEGP_GetNumInstalledEffects", []() -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");
        
        A_long num_effects = 0;
        A_Err err = suites.effectSuite->AEGP_GetNumInstalledEffects(&num_effects);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumInstalledEffects failed");
        return (int)num_effects;
    });

    sdk.def("AEGP_GetNextInstalledEffect", [](int installed_key) -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");
        
        AEGP_InstalledEffectKey next_key = AEGP_InstalledEffectKey_NONE;
        A_Err err = suites.effectSuite->AEGP_GetNextInstalledEffect((AEGP_InstalledEffectKey)installed_key, &next_key);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNextInstalledEffect failed");
        return (int)next_key;
    }, py::arg("installed_key"));

    sdk.def("AEGP_GetEffectName", [](int installed_key) -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");
        
        char name[256] = {'\0'};
        A_Err err = suites.effectSuite->AEGP_GetEffectName((AEGP_InstalledEffectKey)installed_key, name);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEffectName failed");
        return std::string(name);
    }, py::arg("installed_key"));

    sdk.def("AEGP_GetEffectMatchName", [](int installed_key) -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");
        
        char name[256] = {'\0'};
        A_Err err = suites.effectSuite->AEGP_GetEffectMatchName((AEGP_InstalledEffectKey)installed_key, name);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEffectMatchName failed");
        return std::string(name);
    }, py::arg("installed_key"));

    sdk.def("AEGP_GetEffectCategory", [](int installed_key) -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");
        
        char name[256] = {'\0'};
        A_Err err = suites.effectSuite->AEGP_GetEffectCategory((AEGP_InstalledEffectKey)installed_key, name);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEffectCategory failed");
        return std::string(name);
    }, py::arg("installed_key"));

    sdk.def("AEGP_DuplicateEffect", [](uintptr_t effectH_ptr) -> uintptr_t {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_DuplicateEffect: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        AEGP_EffectRefH dupEffectH = nullptr;
        A_Err err = suites.effectSuite->AEGP_DuplicateEffect(effectH, &dupEffectH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DuplicateEffect failed");
        return reinterpret_cast<uintptr_t>(dupEffectH);
    }, py::arg("effectH"));

    sdk.def("AEGP_DeleteLayerEffect", [](uintptr_t effectH_ptr) {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_DeleteLayerEffect: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);

        A_Err err = suites.effectSuite->AEGP_DeleteLayerEffect(effectH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DeleteLayerEffect failed");
    }, py::arg("effectH"));

    // Effect Masks (AE 13.0+)
    sdk.def("AEGP_NumEffectMask", [](uintptr_t effectH_ptr) -> int {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_NumEffectMask: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        A_u_long num_masks = 0;

        A_Err err = suites.effectSuite->AEGP_NumEffectMask(effectH, &num_masks);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NumEffectMask failed");

        return (int)num_masks;
    }, py::arg("effectH"));

    sdk.def("AEGP_GetEffectMaskID", [](uintptr_t effectH_ptr, int mask_index) -> int {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetEffectMaskID: effectH cannot be null");
        }
        Validation::RequireNonNegative(mask_index, "AEGP_GetEffectMaskID: mask_index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        AEGP_MaskIDVal id_val = 0;

        A_Err err = suites.effectSuite->AEGP_GetEffectMaskID(effectH, (A_u_long)mask_index, &id_val);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEffectMaskID failed");

        return (int)id_val;
    }, py::arg("effectH"), py::arg("mask_index"));

    sdk.def("AEGP_AddEffectMask", [](uintptr_t effectH_ptr, int id_val) -> uintptr_t {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_AddEffectMask: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        AEGP_StreamRefH streamH = nullptr;

        A_Err err = suites.effectSuite->AEGP_AddEffectMask(effectH, (AEGP_MaskIDVal)id_val, &streamH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_AddEffectMask failed");

        return reinterpret_cast<uintptr_t>(streamH);
    }, py::arg("effectH"), py::arg("id_val"));

    sdk.def("AEGP_RemoveEffectMask", [](uintptr_t effectH_ptr, int id_val) {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_RemoveEffectMask: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);

        A_Err err = suites.effectSuite->AEGP_RemoveEffectMask(effectH, (AEGP_MaskIDVal)id_val);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_RemoveEffectMask failed");
    }, py::arg("effectH"), py::arg("id_val"));

    sdk.def("AEGP_SetEffectMask", [](uintptr_t effectH_ptr, int mask_index, int id_val) -> uintptr_t {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetEffectMask: effectH cannot be null");
        }
        Validation::RequireNonNegative(mask_index, "AEGP_SetEffectMask: mask_index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        AEGP_StreamRefH streamH = nullptr;

        A_Err err = suites.effectSuite->AEGP_SetEffectMask(effectH, (A_u_long)mask_index, (AEGP_MaskIDVal)id_val, &streamH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetEffectMask failed");

        return reinterpret_cast<uintptr_t>(streamH);
    }, py::arg("effectH"), py::arg("mask_index"), py::arg("id_val"));

#if defined(kAEGPEffectSuiteVersion5)
    // AEGP_GetIsInternalEffect is only available in EffectSuite5 (AE 24.1+)
    sdk.def("AEGP_GetIsInternalEffect", [](int installed_key) -> bool {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        A_Boolean is_internal = FALSE;

        A_Err err = suites.effectSuite->AEGP_GetIsInternalEffect((AEGP_InstalledEffectKey)installed_key, &is_internal);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetIsInternalEffect failed");

        return (is_internal != FALSE);
    }, py::arg("installed_key"));
#endif

    // AEGP_GetEffectParamUnionByIndex - Get parameter definition union
    //
    // FUTURE USE - Effect Serialization:
    // This function is primarily useful for detecting parameter types when building
    // effect serialization/deserialization systems. Key use cases:
    //
    // 1. Detect PF_Param_ARBITRARY_DATA - Custom plugin data that needs special handling
    //    Example: Plugin-specific settings, internal state, custom data structures
    //
    // 2. Identify parameter structure - Find GROUP_START/END, BUTTON, NO_DATA parameters
    //    to understand the effect's parameter organization
    //
    // 3. Build serialization strategies - Determine which parameters need custom serializers
    //    vs. standard value serialization via StreamSuite
    //
    // Example usage for effect serialization:
    //   for i in range(effect.num_properties):
    //       info = ae.sdk.AEGP_GetEffectParamUnionByIndex(plugin_id, effect_h, i)
    //       if info["param_type"] == ae.sdk.PF_Param_ARBITRARY_DATA:
    //           # Use custom serialization for this parameter
    //           serialize_arbitrary_data(effect, i)
    //       elif info["param_type"] not in [ae.sdk.PF_Param_BUTTON, ae.sdk.PF_Param_NO_DATA]:
    //           # Use standard value serialization via StreamSuite
    //           stream = ae.sdk.AEGP_GetNewEffectStreamByIndex(plugin_id, effect_h, i)
    //           value = ae.sdk.AEGP_GetNewStreamValue(plugin_id, stream, time)
    //
    sdk.def("AEGP_GetEffectParamUnionByIndex", [](int plugin_id, uintptr_t effectH_ptr, int param_index) -> py::dict {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetEffectParamUnionByIndex: effectH cannot be null");
        }
        Validation::RequireNonNegative(param_index, "AEGP_GetEffectParamUnionByIndex: param_index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        PF_ParamType param_type = PF_Param_RESERVED;
        PF_ParamDefUnion param_union;
        std::memset(&param_union, 0, sizeof(PF_ParamDefUnion));

        A_Err err = suites.effectSuite->AEGP_GetEffectParamUnionByIndex(
            (AEGP_PluginID)plugin_id,
            effectH,
            (PF_ParamIndex)param_index,
            &param_type,
            &param_union
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEffectParamUnionByIndex failed");

        // Return as dictionary with param_type and selected union fields
        // NOTE: As per SDK documentation, DO NOT USE THE VALUE FROM THIS PARAMDEF!
        //       This function returns the parameter definition structure, not actual values
        //       Use AEGP_GetNewEffectStreamByIndex + AEGP_GetNewStreamValue for actual values
        py::dict result;
        result["param_type"] = (int)param_type;
        result["param_type_name"] = [param_type]() -> std::string {
            switch (param_type) {
                case PF_Param_LAYER: return "LAYER";
                case PF_Param_SLIDER: return "SLIDER";
                case PF_Param_FIX_SLIDER: return "FIX_SLIDER";
                case PF_Param_ANGLE: return "ANGLE";
                case PF_Param_CHECKBOX: return "CHECKBOX";
                case PF_Param_COLOR: return "COLOR";
                case PF_Param_POINT: return "POINT";
                case PF_Param_POPUP: return "POPUP";
                case PF_Param_NO_DATA: return "NO_DATA";
                case PF_Param_FLOAT_SLIDER: return "FLOAT_SLIDER";
                case PF_Param_ARBITRARY_DATA: return "ARBITRARY_DATA";
                case PF_Param_PATH: return "PATH";
                case PF_Param_GROUP_START: return "GROUP_START";
                case PF_Param_GROUP_END: return "GROUP_END";
                case PF_Param_BUTTON: return "BUTTON";
                case PF_Param_POINT_3D: return "POINT_3D";
                default: return "UNKNOWN";
            }
        }();

        // WARNING: Do not extract actual parameter values from this union
        // The SDK documentation explicitly states not to use values from PF_ParamDefUnion
        // This dictionary contains only structural information about the parameter type
        result["warning"] = "DO NOT USE VALUES - use AEGP_GetNewEffectStreamByIndex + AEGP_GetNewStreamValue for actual parameter values";

        return result;
    }, py::arg("plugin_id"), py::arg("effectH"), py::arg("param_index"));

    // AEGP_EffectCallGeneric - Generic effect function call
    //
    // FUTURE USE - Effect Deserialization:
    // This function is critical for effect deserialization to notify effects of state changes.
    // After restoring parameters from serialized data, use this to:
    //
    // 1. Reinitialize effect internal state - After loading parameters
    //    Example: PF_Cmd_PARAMS_SETUP, PF_Cmd_SEQUENCE_SETUP
    //
    // 2. Trigger parameter-dependent updates - When parameters affect other parameters
    //    Example: PF_Cmd_USER_CHANGED_PARAM, PF_Cmd_UPDATE_PARAMS_UI
    //
    // 3. Force recalculation - Ensure effect processes new parameter values
    //    Example: PF_Cmd_RENDER (for specific time points)
    //
    // Example usage for effect deserialization:
    //   # 1. Restore effect with match name
    //   effect = layer.add_effect(effect_data["match_name"])
    //
    //   # 2. Load all parameter values
    //   for param_idx, param_value in effect_data["params"].items():
    //       effect.property_by_index(param_idx).set_value(param_value)
    //
    //   # 3. Notify effect of parameter changes
    //   plugin_id = ae.get_plugin_id()
    //   effect_h = effect._handle
    //
    //   # Reinitialize parameters
    //   ae.sdk.AEGP_EffectCallGeneric(plugin_id, effect_h,
    //                                  effect_cmd=ae.sdk.PF_Cmd_PARAMS_SETUP)
    //
    //   # Update UI to reflect restored state
    //   ae.sdk.AEGP_EffectCallGeneric(plugin_id, effect_h,
    //                                  effect_cmd=ae.sdk.PF_Cmd_UPDATE_PARAMS_UI)
    //
    sdk.def("AEGP_EffectCallGeneric", [](int plugin_id, uintptr_t effectH_ptr, py::object time_obj, int effect_cmd) -> py::none {
        // Validate parameters
        if (effectH_ptr == 0) {
            throw std::invalid_argument("AEGP_EffectCallGeneric: effectH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);

        // Convert time from seconds to A_Time
        A_Time time = {0, 1};
        if (!time_obj.is_none()) {
            double seconds = time_obj.cast<double>();
            // Get frame rate from comp (simplified - using 30fps default)
            // In production, this should query the actual comp frame rate
            double fps = 30.0;
            time = AETypeUtils::SecondsToTimeWithFps(seconds, fps);
        }

        // effect_extraPV is nullptr for PF_Cmd_COMPLETELY_GENERAL
        // For other commands, this would need to be passed as a parameter
        void* effect_extraPV = nullptr;

        A_Err err = suites.effectSuite->AEGP_EffectCallGeneric(
            (AEGP_PluginID)plugin_id,
            effectH,
            &time,
            (PF_Cmd)effect_cmd,
            effect_extraPV
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_EffectCallGeneric failed with error: " + std::to_string(err));

        return py::none();
    }, py::arg("plugin_id"), py::arg("effectH"), py::arg("time") = py::none(), py::arg("effect_cmd") = (int)PF_Cmd_COMPLETELY_GENERAL);

    // Effect Flags Constants
    sdk.attr("AEGP_EffectFlags_NONE") = (int)AEGP_EffectFlags_NONE;
    sdk.attr("AEGP_EffectFlags_ACTIVE") = (int)AEGP_EffectFlags_ACTIVE;
    sdk.attr("AEGP_EffectFlags_AUDIO_ONLY") = (int)AEGP_EffectFlags_AUDIO_ONLY;
    sdk.attr("AEGP_EffectFlags_AUDIO_TOO") = (int)AEGP_EffectFlags_AUDIO_TOO;
    sdk.attr("AEGP_EffectFlags_MISSING") = (int)AEGP_EffectFlags_MISSING;
    sdk.attr("AEGP_InstalledEffectKey_NONE") = (int)AEGP_InstalledEffectKey_NONE;

    // PF_ParamType Constants
    sdk.attr("PF_Param_LAYER") = (int)PF_Param_LAYER;
    sdk.attr("PF_Param_SLIDER") = (int)PF_Param_SLIDER;
    sdk.attr("PF_Param_FIX_SLIDER") = (int)PF_Param_FIX_SLIDER;
    sdk.attr("PF_Param_ANGLE") = (int)PF_Param_ANGLE;
    sdk.attr("PF_Param_CHECKBOX") = (int)PF_Param_CHECKBOX;
    sdk.attr("PF_Param_COLOR") = (int)PF_Param_COLOR;
    sdk.attr("PF_Param_POINT") = (int)PF_Param_POINT;
    sdk.attr("PF_Param_POPUP") = (int)PF_Param_POPUP;
    sdk.attr("PF_Param_NO_DATA") = (int)PF_Param_NO_DATA;
    sdk.attr("PF_Param_FLOAT_SLIDER") = (int)PF_Param_FLOAT_SLIDER;
    sdk.attr("PF_Param_ARBITRARY_DATA") = (int)PF_Param_ARBITRARY_DATA;
    sdk.attr("PF_Param_PATH") = (int)PF_Param_PATH;
    sdk.attr("PF_Param_GROUP_START") = (int)PF_Param_GROUP_START;
    sdk.attr("PF_Param_GROUP_END") = (int)PF_Param_GROUP_END;
    sdk.attr("PF_Param_BUTTON") = (int)PF_Param_BUTTON;
    sdk.attr("PF_Param_POINT_3D") = (int)PF_Param_POINT_3D;

    // PF_Cmd Constants (for AEGP_EffectCallGeneric)
    sdk.attr("PF_Cmd_COMPLETELY_GENERAL") = (int)PF_Cmd_COMPLETELY_GENERAL;
    sdk.attr("PF_Cmd_ABOUT") = (int)PF_Cmd_ABOUT;
    sdk.attr("PF_Cmd_GLOBAL_SETUP") = (int)PF_Cmd_GLOBAL_SETUP;
    sdk.attr("PF_Cmd_GLOBAL_SETDOWN") = (int)PF_Cmd_GLOBAL_SETDOWN;
    sdk.attr("PF_Cmd_PARAMS_SETUP") = (int)PF_Cmd_PARAMS_SETUP;
    sdk.attr("PF_Cmd_SEQUENCE_SETUP") = (int)PF_Cmd_SEQUENCE_SETUP;
    sdk.attr("PF_Cmd_RENDER") = (int)PF_Cmd_RENDER;
    sdk.attr("PF_Cmd_USER_CHANGED_PARAM") = (int)PF_Cmd_USER_CHANGED_PARAM;
    sdk.attr("PF_Cmd_UPDATE_PARAMS_UI") = (int)PF_Cmd_UPDATE_PARAMS_UI;
}

} // namespace SDK
} // namespace PyAE
