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

void init_StreamSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_StreamSuite
    // -----------------------------------------------------------------------

    // Returns (stream_type, value_handle) tuple
    sdk.def("AEGP_GetNewStreamValue", [](int plugin_id, uintptr_t streamH_ptr, int time_mode, double time, bool pre_expression) -> py::tuple {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetNewStreamValue: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_Time a_time = PyAE::AETypeUtils::SecondsToTime(time);

         // Get stream type first
         AEGP_StreamType stream_type;
         A_Err err = suites.streamSuite->AEGP_GetStreamType(streamH, &stream_type);
         if (err != A_Err_NONE) {
             throw std::runtime_error("AEGP_GetStreamType failed with error: " + std::to_string(err));
         }

         // Guard: NO_DATA streams cannot have values (groups, topic headers, etc.)
         if (stream_type == AEGP_StreamType_NO_DATA) {
             throw std::runtime_error("AEGP_GetNewStreamValue: Cannot get value for AEGP_StreamType_NO_DATA stream (group/container node)");
         }

         // Alloc struct on heap
         // Note: Caller owns this pointer and must call AEGP_DisposeStreamValue to free SDK resources,
         // then delete the struct itself. The returned pointer contains deep SDK handles that must be
         // properly disposed via AEGP_DisposeStreamValue before deleting.
         AEGP_StreamValue2* outValue = new AEGP_StreamValue2();
         memset(outValue, 0, sizeof(AEGP_StreamValue2));

         err = suites.streamSuite->AEGP_GetNewStreamValue(
             plugin_id,
             streamH,
             (AEGP_LTimeMode)time_mode,
             &a_time,
             pre_expression ? TRUE : FALSE,
             outValue
         );
         if (err != A_Err_NONE) {
             delete outValue;
             throw std::runtime_error("AEGP_GetNewStreamValue failed with error: " + std::to_string(err));
         }

         return py::make_tuple(static_cast<int>(stream_type), reinterpret_cast<uintptr_t>(outValue));
    }, py::arg("plugin_id"), py::arg("streamH"), py::arg("time_mode"), py::arg("time"), py::arg("pre_expression"));

    // Frees the deep contents using AEGP_DisposeStreamValue AND deletes the struct
    sdk.def("AEGP_DisposeStreamValue", [](uintptr_t valueP_ptr) {
         // Validation - allow null for safety (early return)
         if (valueP_ptr == 0) return;

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);

         // Dispose contents via SDK
         A_Err err = suites.streamSuite->AEGP_DisposeStreamValue(valueP);
         if (err != A_Err_NONE) {
             // If dispose failed, we might still want to delete the struct?
             // But it might mean memory corruption. Throw error.
             throw std::runtime_error("AEGP_DisposeStreamValue failed");
         }
         
         // Delete struct itself
         delete valueP;
    }, py::arg("valueP"));


    sdk.def("AEGP_SetStreamValue", [](int plugin_id, uintptr_t streamH_ptr, uintptr_t valueP_ptr) {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetStreamValue: streamH cannot be null");
         }
         if (valueP_ptr == 0) {
             throw std::invalid_argument("AEGP_SetStreamValue: valueP cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
         
         A_Err err = suites.streamSuite->AEGP_SetStreamValue(plugin_id, streamH, valueP);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetStreamValue failed");
    }, py::arg("plugin_id"), py::arg("streamH"), py::arg("valueP"));

    sdk.def("AEGP_GetStreamName", [](uintptr_t streamH_ptr, bool force_english) -> std::wstring {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetStreamName: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite || !suites.memorySuite) throw std::runtime_error("Stream Suite or Memory Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_MemHandle nameHandle = nullptr;

         // AEGP_GetStreamName requires plugin_id parameter and returns AEGP_MemHandle (UTF16 string)
         A_Err err = suites.streamSuite->AEGP_GetStreamName(
             state.GetPluginID(),
             streamH,
             force_english ? TRUE : FALSE,
             &nameHandle
         );
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStreamName failed");

         // Convert UTF16 string from AEGP_MemHandle to std::wstring
         std::wstring result;
         if (nameHandle) {
             PyAE::ScopedMemHandle scopedHandle(state.GetPluginID(), suites.memorySuite, nameHandle);
             PyAE::ScopedMemLock lock(suites.memorySuite, nameHandle);

             if (lock.IsValid()) {
                 result = std::wstring(reinterpret_cast<const wchar_t*>(lock.As<A_UTF16Char>()));
             }
             // RAII destructors will unlock and free memory
         }

         return result;
    }, py::arg("streamH"), py::arg("force_english"));

    sdk.def("AEGP_GetStreamProperties", [](uintptr_t streamH_ptr) -> py::dict {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetStreamProperties: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_StreamFlags flags = 0;
         A_FpLong minVal = 0.0, maxVal = 0.0;
         
         // SPAPI A_Err (*AEGP_GetStreamProperties)(AEGP_StreamRefH streamH, AEGP_StreamFlags *flagsP, A_FpLong *minP, A_FpLong *maxP);
         A_Err err = suites.streamSuite->AEGP_GetStreamProperties(streamH, &flags, &minVal, &maxVal);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStreamProperties failed");
         
         py::dict result;
         result["flags"] = (int)flags;
         result["min"] = minVal;
         result["max"] = maxVal;
         return result;
    }, py::arg("streamH"));

    sdk.def("AEGP_GetStreamType", [](uintptr_t streamH_ptr) -> int {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetStreamType: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_StreamType type = AEGP_StreamType_NO_DATA;

         A_Err err = suites.streamSuite->AEGP_GetStreamType(streamH, &type);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStreamType failed");

         return (int)type;
    }, py::arg("streamH"));

    // Additional StreamSuite functions
    sdk.def("AEGP_GetNewLayerStream", [](int plugin_id, uintptr_t layerH_ptr, int which_stream) -> uintptr_t {
         // Validation
         if (layerH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetNewLayerStream: layerH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
         AEGP_StreamRefH streamH = nullptr;

         A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
             plugin_id, layerH, (AEGP_LayerStream)which_stream, &streamH);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewLayerStream failed");

         return reinterpret_cast<uintptr_t>(streamH);
    }, py::arg("plugin_id"), py::arg("layerH"), py::arg("which_stream"));

    sdk.def("AEGP_GetNewMaskStream", [](int plugin_id, uintptr_t maskH_ptr, int which_stream) -> uintptr_t {
         // Validation
         if (maskH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetNewMaskStream: maskH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
         AEGP_StreamRefH streamH = nullptr;

         A_Err err = suites.streamSuite->AEGP_GetNewMaskStream(
             plugin_id, maskH, (AEGP_MaskStream)which_stream, &streamH);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewMaskStream failed");

         return reinterpret_cast<uintptr_t>(streamH);
    }, py::arg("plugin_id"), py::arg("maskH"), py::arg("which_stream"));

    sdk.def("AEGP_GetEffectNumParamStreams", [](uintptr_t effectH_ptr) -> int {
         // Validation
         if (effectH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetEffectNumParamStreams: effectH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
         A_long numParams = 0;

         A_Err err = suites.streamSuite->AEGP_GetEffectNumParamStreams(effectH, &numParams);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEffectNumParamStreams failed");

         return (int)numParams;
    }, py::arg("effectH"));

    sdk.def("AEGP_GetNewEffectStreamByIndex", [](int plugin_id, uintptr_t effectH_ptr, int param_index) -> uintptr_t {
         // Validation
         if (effectH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetNewEffectStreamByIndex: effectH cannot be null");
         }
         Validation::RequireNonNegative(param_index, "AEGP_GetNewEffectStreamByIndex: param_index");

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
         AEGP_StreamRefH streamH = nullptr;

         A_Err err = suites.streamSuite->AEGP_GetNewEffectStreamByIndex(
             plugin_id, effectH, (PF_ParamIndex)param_index, &streamH);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewEffectStreamByIndex failed");

         return reinterpret_cast<uintptr_t>(streamH);
    }, py::arg("plugin_id"), py::arg("effectH"), py::arg("param_index"));

    sdk.def("AEGP_DisposeStream", [](uintptr_t streamH_ptr) {
         // Validation - allow null for safety (early return)
         if (streamH_ptr == 0) return;

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);

         A_Err err = suites.streamSuite->AEGP_DisposeStream(streamH);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisposeStream failed");
    }, py::arg("streamH"));

    sdk.def("AEGP_IsStreamLegal", [](uintptr_t layerH_ptr, int which_stream) -> bool {
         // Validation
         if (layerH_ptr == 0) {
             throw std::invalid_argument("AEGP_IsStreamLegal: layerH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
         A_Boolean is_legal = FALSE;

         A_Err err = suites.streamSuite->AEGP_IsStreamLegal(layerH, (AEGP_LayerStream)which_stream, &is_legal);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsStreamLegal failed");

         return is_legal == TRUE;
    }, py::arg("layerH"), py::arg("which_stream"));

    sdk.def("AEGP_CanVaryOverTime", [](uintptr_t streamH_ptr) -> bool {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_CanVaryOverTime: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_Boolean can_vary = FALSE;

         A_Err err = suites.streamSuite->AEGP_CanVaryOverTime(streamH, &can_vary);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_CanVaryOverTime failed");

         return can_vary == TRUE;
    }, py::arg("streamH"));

    sdk.def("AEGP_IsStreamTimevarying", [](uintptr_t streamH_ptr) -> bool {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_IsStreamTimevarying: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_Boolean is_timevarying = FALSE;

         A_Err err = suites.streamSuite->AEGP_IsStreamTimevarying(streamH, &is_timevarying);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsStreamTimevarying failed");

         return is_timevarying == TRUE;
    }, py::arg("streamH"));

    sdk.def("AEGP_GetStreamUnitsText", [](uintptr_t streamH_ptr, bool force_english) -> std::string {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetStreamUnitsText: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_char units[256];  // AEGP_MAX_STREAM_UNITS_SIZE

         A_Err err = suites.streamSuite->AEGP_GetStreamUnitsText(streamH, force_english ? TRUE : FALSE, units);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStreamUnitsText failed");

         return std::string(units);
    }, py::arg("streamH"), py::arg("force_english"));

    // Expression support
    sdk.def("AEGP_GetExpressionState", [](int plugin_id, uintptr_t streamH_ptr) -> bool {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetExpressionState: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         A_Boolean enabled = FALSE;

         A_Err err = suites.streamSuite->AEGP_GetExpressionState(plugin_id, streamH, &enabled);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetExpressionState failed");

         return enabled == TRUE;
    }, py::arg("plugin_id"), py::arg("streamH"));

    sdk.def("AEGP_SetExpressionState", [](int plugin_id, uintptr_t streamH_ptr, bool enabled) {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetExpressionState: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);

         A_Err err = suites.streamSuite->AEGP_SetExpressionState(plugin_id, streamH, enabled ? TRUE : FALSE);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetExpressionState failed");
    }, py::arg("plugin_id"), py::arg("streamH"), py::arg("enabled"));

    sdk.def("AEGP_GetExpression", [](int plugin_id, uintptr_t streamH_ptr) -> std::wstring {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetExpression: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite || !suites.memorySuite) throw std::runtime_error("Stream Suite or Memory Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_MemHandle exprHandle = nullptr;

         A_Err err = suites.streamSuite->AEGP_GetExpression(plugin_id, streamH, &exprHandle);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetExpression failed");

         std::wstring result;
         if (exprHandle) {
             PyAE::ScopedMemHandle scopedHandle(state.GetPluginID(), suites.memorySuite, exprHandle);
             PyAE::ScopedMemLock lock(suites.memorySuite, exprHandle);

             if (lock.IsValid()) {
                 result = std::wstring(reinterpret_cast<const wchar_t*>(lock.As<A_UTF16Char>()));
             }
             // RAII destructors will unlock and free memory
         }

         return result;
    }, py::arg("plugin_id"), py::arg("streamH"));

    sdk.def("AEGP_SetExpression", [](int plugin_id, uintptr_t streamH_ptr, const std::wstring& expression) {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_SetExpression: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);

         // Convert wstring to UTF16
         const A_UTF16Char* utf16expr = reinterpret_cast<const A_UTF16Char*>(expression.c_str());

         A_Err err = suites.streamSuite->AEGP_SetExpression(plugin_id, streamH, utf16expr);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetExpression failed");
    }, py::arg("plugin_id"), py::arg("streamH"), py::arg("expression"));

    sdk.def("AEGP_GetLayerStreamValue", [](uintptr_t layerH_ptr, int which_stream, int time_mode, double time, bool pre_expression) -> py::tuple {
         // Validation
         if (layerH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetLayerStreamValue: layerH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
         A_Time a_time = PyAE::AETypeUtils::SecondsToTime(time);
         AEGP_StreamVal2 stream_val;
         AEGP_StreamType stream_type;

         A_Err err = suites.streamSuite->AEGP_GetLayerStreamValue(
             layerH, (AEGP_LayerStream)which_stream, (AEGP_LTimeMode)time_mode,
             &a_time, pre_expression ? TRUE : FALSE, &stream_val, &stream_type);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerStreamValue failed");

         // Return stream type and value
         return py::make_tuple((int)stream_type, stream_val.one_d);  // Simplified
    }, py::arg("layerH"), py::arg("which_stream"), py::arg("time_mode"), py::arg("time"), py::arg("pre_expression"));

    sdk.def("AEGP_GetValidInterpolations", [](uintptr_t streamH_ptr) -> int {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetValidInterpolations: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_KeyInterpolationMask mask;

         A_Err err = suites.streamSuite->AEGP_GetValidInterpolations(streamH, &mask);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetValidInterpolations failed");

         return (int)mask;
    }, py::arg("streamH"));

    sdk.def("AEGP_DuplicateStreamRef", [](int plugin_id, uintptr_t streamH_ptr) -> uintptr_t {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_DuplicateStreamRef: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         AEGP_StreamRefH dup_streamH = nullptr;

         A_Err err = suites.streamSuite->AEGP_DuplicateStreamRef(plugin_id, streamH, &dup_streamH);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_DuplicateStreamRef failed");

         return reinterpret_cast<uintptr_t>(dup_streamH);
    }, py::arg("plugin_id"), py::arg("streamH"));

#if defined(kAEGPStreamSuiteVersion6)
    // AEGP_GetUniqueStreamID is only available in StreamSuite6 (AE 24.0+)
    sdk.def("AEGP_GetUniqueStreamID", [](uintptr_t streamH_ptr) -> int {
         // Validation
         if (streamH_ptr == 0) {
             throw std::invalid_argument("AEGP_GetUniqueStreamID: streamH cannot be null");
         }

         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.streamSuite) throw std::runtime_error("Stream Suite not available");

         AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
         int32_t streamID = 0;

         A_Err err = suites.streamSuite->AEGP_GetUniqueStreamID(streamH, &streamID);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetUniqueStreamID failed");

         return (int)streamID;
    }, py::arg("streamH"));
#endif

    // -----------------------------------------------------------------------
    // DynamicStreamSuite4 - Property Tree Traversal (for serialization)
    // -----------------------------------------------------------------------

    // AEGP_GetNewStreamRefForLayer - Get root of layer's property tree
    sdk.def("AEGP_GetNewStreamRefForLayer", [](uintptr_t layerH_ptr) -> uintptr_t {
        // Validation
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNewStreamRefForLayer: layerH cannot be null");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.dynamicStreamSuite) throw std::runtime_error("DynamicStream Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
        AEGP_StreamRefH streamH = nullptr;

        A_Err err = suites.dynamicStreamSuite->AEGP_GetNewStreamRefForLayer(
            state.GetPluginID(), layerH, &streamH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewStreamRefForLayer failed");

        return reinterpret_cast<uintptr_t>(streamH);
    }, py::arg("layerH"));

    // AEGP_GetNewStreamRefForMask - Get root of mask's property tree
    sdk.def("AEGP_GetNewStreamRefForMask", [](uintptr_t maskH_ptr) -> uintptr_t {
        // Validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNewStreamRefForMask: maskH cannot be null");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.dynamicStreamSuite) throw std::runtime_error("DynamicStream Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        AEGP_StreamRefH streamH = nullptr;

        A_Err err = suites.dynamicStreamSuite->AEGP_GetNewStreamRefForMask(
            state.GetPluginID(), maskH, &streamH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewStreamRefForMask failed");

        return reinterpret_cast<uintptr_t>(streamH);
    }, py::arg("maskH"));

    // AEGP_GetNewParentStreamRef - Navigate up the property tree
    sdk.def("AEGP_GetNewParentStreamRef", [](uintptr_t streamH_ptr) -> uintptr_t {
        // Validation
        if (streamH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNewParentStreamRef: streamH cannot be null");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.dynamicStreamSuite) throw std::runtime_error("DynamicStream Suite not available");

        AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
        AEGP_StreamRefH parentStreamH = nullptr;

        A_Err err = suites.dynamicStreamSuite->AEGP_GetNewParentStreamRef(
            state.GetPluginID(), streamH, &parentStreamH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewParentStreamRef failed");

        return reinterpret_cast<uintptr_t>(parentStreamH);
    }, py::arg("streamH"));

    // AEGP_GetStreamGroupingType - Determine if stream is leaf, group, or indexed group
    sdk.def("AEGP_GetStreamGroupingType", [](uintptr_t streamH_ptr) -> int {
        // Validation
        if (streamH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetStreamGroupingType: streamH cannot be null");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.dynamicStreamSuite) throw std::runtime_error("DynamicStream Suite not available");

        AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
        AEGP_StreamGroupingType groupType = AEGP_StreamGroupingType_NONE;

        A_Err err = suites.dynamicStreamSuite->AEGP_GetStreamGroupingType(streamH, &groupType);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStreamGroupingType failed");

        return (int)groupType;
    }, py::arg("streamH"));

    // AEGP_GetNumStreamsInGroup - Get number of child streams
    sdk.def("AEGP_GetNumStreamsInGroup", [](uintptr_t streamH_ptr) -> int {
        // Validation
        if (streamH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNumStreamsInGroup: streamH cannot be null");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.dynamicStreamSuite) throw std::runtime_error("DynamicStream Suite not available");

        AEGP_StreamRefH streamH = reinterpret_cast<AEGP_StreamRefH>(streamH_ptr);
        A_long numStreams = 0;

        A_Err err = suites.dynamicStreamSuite->AEGP_GetNumStreamsInGroup(streamH, &numStreams);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumStreamsInGroup failed");

        return (int)numStreams;
    }, py::arg("streamH"));

    // AEGP_GetNewStreamRefByIndex - Get child stream by index
    sdk.def("AEGP_GetNewStreamRefByIndex", [](uintptr_t parentStreamH_ptr, int index) -> uintptr_t {
        // Validation
        if (parentStreamH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNewStreamRefByIndex: parentStreamH cannot be null");
        }
        Validation::RequireNonNegative(index, "AEGP_GetNewStreamRefByIndex: index");

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.dynamicStreamSuite) throw std::runtime_error("DynamicStream Suite not available");

        AEGP_StreamRefH parentStreamH = reinterpret_cast<AEGP_StreamRefH>(parentStreamH_ptr);
        AEGP_StreamRefH childStreamH = nullptr;

        A_Err err = suites.dynamicStreamSuite->AEGP_GetNewStreamRefByIndex(
            state.GetPluginID(), parentStreamH, index, &childStreamH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewStreamRefByIndex failed");

        return reinterpret_cast<uintptr_t>(childStreamH);
    }, py::arg("parentStreamH"), py::arg("index"));

    // Constants
    sdk.attr("AEGP_StreamType_NO_DATA") = (int)AEGP_StreamType_NO_DATA;
    sdk.attr("AEGP_StreamType_TwoD") = (int)AEGP_StreamType_TwoD;
    sdk.attr("AEGP_StreamType_TwoD_SPATIAL") = (int)AEGP_StreamType_TwoD_SPATIAL;
    sdk.attr("AEGP_StreamType_ThreeD") = (int)AEGP_StreamType_ThreeD;
    sdk.attr("AEGP_StreamType_ThreeD_SPATIAL") = (int)AEGP_StreamType_ThreeD_SPATIAL;
    sdk.attr("AEGP_StreamType_COLOR") = (int)AEGP_StreamType_COLOR;
    sdk.attr("AEGP_StreamType_MARKER") = (int)AEGP_StreamType_MARKER;
    sdk.attr("AEGP_StreamType_LAYER_ID") = (int)AEGP_StreamType_LAYER_ID;
    sdk.attr("AEGP_StreamType_MASK_ID") = (int)AEGP_StreamType_MASK_ID;
    sdk.attr("AEGP_StreamType_MASK") = (int)AEGP_StreamType_MASK;
    sdk.attr("AEGP_StreamType_TEXT_DOCUMENT") = (int)AEGP_StreamType_TEXT_DOCUMENT;

    // Stream Grouping Types
    sdk.attr("AEGP_StreamGroupingType_NONE") = (int)AEGP_StreamGroupingType_NONE;
    sdk.attr("AEGP_StreamGroupingType_LEAF") = (int)AEGP_StreamGroupingType_LEAF;
    sdk.attr("AEGP_StreamGroupingType_NAMED_GROUP") = (int)AEGP_StreamGroupingType_NAMED_GROUP;
    sdk.attr("AEGP_StreamGroupingType_INDEXED_GROUP") = (int)AEGP_StreamGroupingType_INDEXED_GROUP;

    // -----------------------------------------------------------------------
    // Stream Value Helpers
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetStreamValue_Double", [](uintptr_t valueP_ptr) -> double {
        // Validation
        if (valueP_ptr == 0) {
            throw std::invalid_argument("AEGP_GetStreamValue_Double: valueP cannot be null");
        }
        AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
        return valueP->val.one_d;
    }, py::arg("valueP"));

    sdk.def("AEGP_SetStreamValue_Double", [](uintptr_t valueP_ptr, double val) {
        // Validation
        if (valueP_ptr == 0) {
            throw std::invalid_argument("AEGP_SetStreamValue_Double: valueP cannot be null");
        }
        AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
        valueP->val.one_d = val;
    }, py::arg("valueP"), py::arg("val"));

    sdk.def("AEGP_GetStreamValue_Color", [](uintptr_t valueP_ptr) -> py::tuple {
        // Validation
        if (valueP_ptr == 0) {
            throw std::invalid_argument("AEGP_GetStreamValue_Color: valueP cannot be null");
        }
        AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
        return py::make_tuple(valueP->val.color.redF, valueP->val.color.greenF, valueP->val.color.blueF, valueP->val.color.alphaF);
    }, py::arg("valueP"));

    sdk.def("AEGP_SetStreamValue_Color", [](uintptr_t valueP_ptr, double r, double g, double b, double a) {
         // Validation
         if (valueP_ptr == 0) {
             throw std::invalid_argument("AEGP_SetStreamValue_Color: valueP cannot be null");
         }
         AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
         valueP->val.color.redF = r;
         valueP->val.color.greenF = g;
         valueP->val.color.blueF = b;
         valueP->val.color.alphaF = a;
    }, py::arg("valueP"), py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a"));
    
    sdk.def("AEGP_GetStreamValue_Point", [](uintptr_t valueP_ptr) -> py::tuple {
        // Validation
        if (valueP_ptr == 0) {
            throw std::invalid_argument("AEGP_GetStreamValue_Point: valueP cannot be null");
        }
        AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
        return py::make_tuple(valueP->val.two_d.x, valueP->val.two_d.y);
    }, py::arg("valueP"));

    sdk.def("AEGP_SetStreamValue_Point", [](uintptr_t valueP_ptr, double x, double y) {
         // Validation
         if (valueP_ptr == 0) {
             throw std::invalid_argument("AEGP_SetStreamValue_Point: valueP cannot be null");
         }
         AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
         valueP->val.two_d.x = x;
         valueP->val.two_d.y = y;
    }, py::arg("valueP"), py::arg("x"), py::arg("y"));

    sdk.def("AEGP_GetStreamValue_ThreeD", [](uintptr_t valueP_ptr) -> py::tuple {
        // Validation
        if (valueP_ptr == 0) {
            throw std::invalid_argument("AEGP_GetStreamValue_ThreeD: valueP cannot be null");
        }
        AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
        return py::make_tuple(valueP->val.three_d.x, valueP->val.three_d.y, valueP->val.three_d.z);
    }, py::arg("valueP"));

    sdk.def("AEGP_SetStreamValue_ThreeD", [](uintptr_t valueP_ptr, double x, double y, double z) {
         // Validation
         if (valueP_ptr == 0) {
             throw std::invalid_argument("AEGP_SetStreamValue_ThreeD: valueP cannot be null");
         }
         AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
         valueP->val.three_d.x = x;
         valueP->val.three_d.y = y;
         valueP->val.three_d.z = z;
    }, py::arg("valueP"), py::arg("x"), py::arg("y"), py::arg("z"));

    sdk.def("AEGP_GetStreamValue_TextDocument", [](uintptr_t valueP_ptr) -> uintptr_t {
        // Validation
        if (valueP_ptr == 0) {
            throw std::invalid_argument("AEGP_GetStreamValue_TextDocument: valueP cannot be null");
        }
        AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
        return reinterpret_cast<uintptr_t>(valueP->val.text_documentH);
    }, py::arg("valueP"));

    sdk.def("AEGP_SetStreamValue_TextDocument", [](uintptr_t valueP_ptr, uintptr_t text_documentH_ptr) {
         // Validation
         if (valueP_ptr == 0) {
             throw std::invalid_argument("AEGP_SetStreamValue_TextDocument: valueP cannot be null");
         }
         Validation::RequireNonNull(text_documentH_ptr, "text_documentH");
         AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
         valueP->val.text_documentH = reinterpret_cast<AEGP_TextDocumentH>(text_documentH_ptr);
    }, py::arg("valueP"), py::arg("text_documentH"));

}

} // namespace SDK
} // namespace PyAE
