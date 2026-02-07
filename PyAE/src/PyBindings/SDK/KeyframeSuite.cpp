#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_KeyframeSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_KeyframeSuite
    // -----------------------------------------------------------------------
    
    sdk.def("AEGP_GetStreamNumKfs", [](uintptr_t streamH_ptr) -> int {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetStreamNumKFs: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_long numKfs = 0;
         A_Err err = suites.keyframeSuite->AEGP_GetStreamNumKFs(streamH, &numKfs);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStreamNumKFs failed with error: " + std::to_string(err));

         return (int)numKfs;
    }, py::arg("streamH"));

    sdk.def("AEGP_GetKeyframeTime", [](uintptr_t streamH_ptr, int keyIndex, int time_mode) -> double {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetKeyframeTime: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_Time time;
         A_Err err = suites.keyframeSuite->AEGP_GetKeyframeTime(streamH, (A_long)keyIndex, (AEGP_LTimeMode)time_mode, &time);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetKeyframeTime failed with error: " + std::to_string(err));

         return PyAE::AETypeUtils::TimeToSeconds(time);
    }, py::arg("streamH"), py::arg("keyIndex"), py::arg("time_mode"));

    sdk.def("AEGP_InsertKeyframe", [](uintptr_t streamH_ptr, int time_mode, double time) -> int {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_InsertKeyframe: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_Time a_time = PyAE::AETypeUtils::SecondsToTime(time);

         A_long keyIndex = 0;
         A_Err err = suites.keyframeSuite->AEGP_InsertKeyframe(
            streamH,
            (AEGP_LTimeMode)time_mode,
            &a_time,
            &keyIndex
         );
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_InsertKeyframe failed");

         return (int)keyIndex;
    }, py::arg("streamH"), py::arg("time_mode"), py::arg("time"));

    sdk.def("AEGP_DeleteKeyframe", [](uintptr_t streamH_ptr, int keyIndex) {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_DeleteKeyframe: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_Err err = suites.keyframeSuite->AEGP_DeleteKeyframe(streamH, (A_long)keyIndex);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_DeleteKeyframe failed");
    }, py::arg("streamH"), py::arg("keyIndex"));

    sdk.def("AEGP_GetKeyframeFlags", [](uintptr_t streamH_ptr, int keyIndex) -> int {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetKeyframeFlags: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_KeyframeFlags flags = 0;
         A_Err err = suites.keyframeSuite->AEGP_GetKeyframeFlags(streamH, (A_long)keyIndex, &flags);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetKeyframeFlags failed");

         return (int)flags;
    }, py::arg("streamH"), py::arg("keyIndex"));

    sdk.def("AEGP_SetKeyframeFlag", [](uintptr_t streamH_ptr, int keyIndex, int flag, bool value) {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeFlag: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_Err err = suites.keyframeSuite->AEGP_SetKeyframeFlag(
             streamH,
             (A_long)keyIndex,
             (AEGP_KeyframeFlags)flag,
             value ? TRUE : FALSE
         );
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetKeyframeFlag failed");
    }, py::arg("streamH"), py::arg("keyIndex"), py::arg("flag"), py::arg("value"));

    // Returns a pointer to a new heap-allocated AEGP_StreamValue2
    sdk.def("AEGP_GetNewKeyframeValue", [](int plugin_id, uintptr_t streamH_ptr, int keyIndex) -> uintptr_t {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetNewKeyframeValue: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);

         // Alloc struct on heap
         AEGP_StreamValue2* outValue = new AEGP_StreamValue2();
         memset(outValue, 0, sizeof(AEGP_StreamValue2));

         A_Err err = suites.keyframeSuite->AEGP_GetNewKeyframeValue(plugin_id, streamH, (A_long)keyIndex, outValue);
         if (err != A_Err_NONE) {
             delete outValue;
             throw std::runtime_error("AEGP_GetNewKeyframeValue failed");
         }

         return reinterpret_cast<uintptr_t>(outValue);
    }, py::arg("plugin_id"), py::arg("streamH"), py::arg("keyIndex"));

    sdk.def("AEGP_SetKeyframeValue", [](uintptr_t streamH_ptr, int keyIndex, uintptr_t valueP_ptr) {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeValue: streamH cannot be null");
         }
         if (valueP_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeValue: valueP cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);

         A_Err err = suites.keyframeSuite->AEGP_SetKeyframeValue(streamH, (A_long)keyIndex, valueP);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetKeyframeValue failed");
    }, py::arg("streamH"), py::arg("keyIndex"), py::arg("valueP"));

    // Stream dimensionality
    sdk.def("AEGP_GetStreamValueDimensionality", [](uintptr_t streamH_ptr) -> int {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetStreamValueDimensionality: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_short dim = 0;
         A_Err err = suites.keyframeSuite->AEGP_GetStreamValueDimensionality(streamH, &dim);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStreamValueDimensionality failed");

         return (int)dim;
    }, py::arg("streamH"));

    sdk.def("AEGP_GetStreamTemporalDimensionality", [](uintptr_t streamH_ptr) -> int {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetStreamTemporalDimensionality: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_short dim = 0;
         A_Err err = suites.keyframeSuite->AEGP_GetStreamTemporalDimensionality(streamH, &dim);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStreamTemporalDimensionality failed");

         return (int)dim;
    }, py::arg("streamH"));

    // Spatial tangents
    sdk.def("AEGP_GetNewKeyframeSpatialTangents", [](int plugin_id, uintptr_t streamH_ptr, int keyIndex) -> py::tuple {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetNewKeyframeSpatialTangents: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);

         AEGP_StreamValue2* in_tan = new AEGP_StreamValue2();
         AEGP_StreamValue2* out_tan = new AEGP_StreamValue2();
         memset(in_tan, 0, sizeof(AEGP_StreamValue2));
         memset(out_tan, 0, sizeof(AEGP_StreamValue2));

         A_Err err = suites.keyframeSuite->AEGP_GetNewKeyframeSpatialTangents(
             plugin_id, streamH, (A_long)keyIndex, in_tan, out_tan);
         if (err != A_Err_NONE) {
             delete in_tan;
             delete out_tan;
             throw std::runtime_error("AEGP_GetNewKeyframeSpatialTangents failed");
         }

         return py::make_tuple(reinterpret_cast<uintptr_t>(in_tan), reinterpret_cast<uintptr_t>(out_tan));
    }, py::arg("plugin_id"), py::arg("streamH"), py::arg("keyIndex"));

    sdk.def("AEGP_SetKeyframeSpatialTangents", [](uintptr_t streamH_ptr, int keyIndex, uintptr_t in_tan_ptr, uintptr_t out_tan_ptr) {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeSpatialTangents: streamH cannot be null");
         }
         if (in_tan_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeSpatialTangents: in_tanP cannot be null");
         }
         if (out_tan_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeSpatialTangents: out_tanP cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_StreamValue2* in_tan = reinterpret_cast<AEGP_StreamValue2*>(in_tan_ptr);
         AEGP_StreamValue2* out_tan = reinterpret_cast<AEGP_StreamValue2*>(out_tan_ptr);

         A_Err err = suites.keyframeSuite->AEGP_SetKeyframeSpatialTangents(streamH, (A_long)keyIndex, in_tan, out_tan);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetKeyframeSpatialTangents failed");
    }, py::arg("streamH"), py::arg("keyIndex"), py::arg("in_tanP"), py::arg("out_tanP"));

    // Temporal ease
    sdk.def("AEGP_GetKeyframeTemporalEase", [](uintptr_t streamH_ptr, int keyIndex, int dimension) -> py::tuple {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetKeyframeTemporalEase: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");
         Validation::RequireNonNegative(dimension, "dimension");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_KeyframeEase in_ease, out_ease;

         A_Err err = suites.keyframeSuite->AEGP_GetKeyframeTemporalEase(
             streamH, (A_long)keyIndex, (A_long)dimension, &in_ease, &out_ease);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetKeyframeTemporalEase failed");

         return py::make_tuple(
             py::make_tuple(in_ease.speedF, in_ease.influenceF),
             py::make_tuple(out_ease.speedF, out_ease.influenceF)
         );
    }, py::arg("streamH"), py::arg("keyIndex"), py::arg("dimension"));

    sdk.def("AEGP_SetKeyframeTemporalEase", [](uintptr_t streamH_ptr, int keyIndex, int dimension,
                                                 double in_speed, double in_influence,
                                                 double out_speed, double out_influence) {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeTemporalEase: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");
         Validation::RequireNonNegative(dimension, "dimension");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_KeyframeEase in_ease, out_ease;
         in_ease.speedF = in_speed;
         in_ease.influenceF = in_influence;
         out_ease.speedF = out_speed;
         out_ease.influenceF = out_influence;

         A_Err err = suites.keyframeSuite->AEGP_SetKeyframeTemporalEase(
             streamH, (A_long)keyIndex, (A_long)dimension, &in_ease, &out_ease);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetKeyframeTemporalEase failed");
    }, py::arg("streamH"), py::arg("keyIndex"), py::arg("dimension"),
       py::arg("in_speed"), py::arg("in_influence"), py::arg("out_speed"), py::arg("out_influence"));

    // Interpolation
    sdk.def("AEGP_GetKeyframeInterpolation", [](uintptr_t streamH_ptr, int keyIndex) -> py::tuple {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetKeyframeInterpolation: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_KeyframeInterpolationType in_interp, out_interp;

         A_Err err = suites.keyframeSuite->AEGP_GetKeyframeInterpolation(streamH, (A_long)keyIndex, &in_interp, &out_interp);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetKeyframeInterpolation failed");

         return py::make_tuple((int)in_interp, (int)out_interp);
    }, py::arg("streamH"), py::arg("keyIndex"));

    sdk.def("AEGP_SetKeyframeInterpolation", [](uintptr_t streamH_ptr, int keyIndex, int in_interp, int out_interp) {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeInterpolation: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);

         A_Err err = suites.keyframeSuite->AEGP_SetKeyframeInterpolation(
             streamH, (A_long)keyIndex,
             (AEGP_KeyframeInterpolationType)in_interp,
             (AEGP_KeyframeInterpolationType)out_interp);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetKeyframeInterpolation failed");
    }, py::arg("streamH"), py::arg("keyIndex"), py::arg("in_interp"), py::arg("out_interp"));

    // Batch keyframe operations
    sdk.def("AEGP_StartAddKeyframes", [](uintptr_t streamH_ptr) -> uintptr_t {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_StartAddKeyframes: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_AddKeyframesInfoH akH = nullptr;

         A_Err err = suites.keyframeSuite->AEGP_StartAddKeyframes(streamH, &akH);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_StartAddKeyframes failed");

         return reinterpret_cast<uintptr_t>(akH);
    }, py::arg("streamH"));

    sdk.def("AEGP_AddKeyframes", [](uintptr_t akH_ptr, int time_mode, double time) -> int {
         if (akH_ptr == 0) {
             throw std::invalid_argument("AEGP_AddKeyframes: akH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_AddKeyframesInfoH akH = reinterpret_cast<AEGP_AddKeyframesInfoH>(akH_ptr);
         A_Time a_time = PyAE::AETypeUtils::SecondsToTime(time);
         A_long keyIndex = 0;

         A_Err err = suites.keyframeSuite->AEGP_AddKeyframes(akH, (AEGP_LTimeMode)time_mode, &a_time, &keyIndex);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_AddKeyframes failed");

         return (int)keyIndex;
    }, py::arg("akH"), py::arg("time_mode"), py::arg("time"));

    sdk.def("AEGP_SetAddKeyframe", [](uintptr_t akH_ptr, int keyIndex, uintptr_t valueP_ptr) {
         if (akH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetAddKeyframe: akH cannot be null");
         }
         if (valueP_ptr == 0) {
             throw std::invalid_argument("AEGP_SetAddKeyframe: valueP cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_AddKeyframesInfoH akH = reinterpret_cast<AEGP_AddKeyframesInfoH>(akH_ptr);
         AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);

         A_Err err = suites.keyframeSuite->AEGP_SetAddKeyframe(akH, (A_long)keyIndex, valueP);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetAddKeyframe failed");
    }, py::arg("akH"), py::arg("keyIndex"), py::arg("valueP"));

    sdk.def("AEGP_EndAddKeyframes", [](bool add, uintptr_t akH_ptr) {
         if (akH_ptr == 0) {
             throw std::invalid_argument("AEGP_EndAddKeyframes: akH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_AddKeyframesInfoH akH = reinterpret_cast<AEGP_AddKeyframesInfoH>(akH_ptr);

         A_Err err = suites.keyframeSuite->AEGP_EndAddKeyframes(add ? TRUE : FALSE, akH);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_EndAddKeyframes failed");
    }, py::arg("add"), py::arg("akH"));

#if defined(kAEGPKeyframeSuiteVersion5)
    // Keyframe label color functions are only available in KeyframeSuite5 (AE 22.5+)

    sdk.def("AEGP_GetKeyframeLabelColorIndex", [](uintptr_t streamH_ptr, int keyIndex) -> int {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetKeyframeLabelColorIndex: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_long label = 0;

         A_Err err = suites.keyframeSuite->AEGP_GetKeyframeLabelColorIndex(streamH, (A_long)keyIndex, &label);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetKeyframeLabelColorIndex failed");

         return (int)label;
    }, py::arg("streamH"), py::arg("keyIndex"));

    sdk.def("AEGP_SetKeyframeLabelColorIndex", [](uintptr_t streamH_ptr, int keyIndex, int label) {
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetKeyframeLabelColorIndex: streamH cannot be null");
         }
         Validation::RequireNonNegative(keyIndex, "keyIndex");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.keyframeSuite) throw std::runtime_error("Keyframe Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);

         A_Err err = suites.keyframeSuite->AEGP_SetKeyframeLabelColorIndex(streamH, (A_long)keyIndex, (A_long)label);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetKeyframeLabelColorIndex failed");
    }, py::arg("streamH"), py::arg("keyIndex"), py::arg("label"));
#endif

    // Keyframe Flags (AE 2025: AUTO_BEZIER → AUTOBEZIER)
    sdk.attr("AEGP_KeyframeFlag_NONE") = (int)AEGP_KeyframeFlag_NONE;
    sdk.attr("AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS") = (int)AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS;
    sdk.attr("AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER") = (int)AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER;
    sdk.attr("AEGP_KeyframeFlag_SPATIAL_CONTINUOUS") = (int)AEGP_KeyframeFlag_SPATIAL_CONTINUOUS;
    sdk.attr("AEGP_KeyframeFlag_SPATIAL_AUTOBEZIER") = (int)AEGP_KeyframeFlag_SPATIAL_AUTOBEZIER;
    sdk.attr("AEGP_KeyframeFlag_ROVING") = (int)AEGP_KeyframeFlag_ROVING;
}

} // namespace SDK
} // namespace PyAE
