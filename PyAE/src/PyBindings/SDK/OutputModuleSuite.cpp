#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_OutputModuleSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_OutputModuleSuite
    // -----------------------------------------------------------------------
    //
    // IMPORTANT: Index conventions for OutputModuleSuite
    // --------------------------------------------------
    // AEGP_GetOutputModuleByIndex uses 0-based indexing (unlike most AEGP functions).
    // This is an exception in the SDK - most other AEGP functions use 1-based indexing.
    //
    // IMPORTANT: Handle pairing rules
    // -------------------------------
    // rq_itemH and outmodH must be used as consistent pairs:
    //   - (rq_itemH, outmodH) - Both actual handles from the same RQ item
    //   - (0, 0) - Queuebert pattern (first RQ item, first output module)
    //   - Mixing patterns (e.g., 0 with actual handle) causes errors
    //
    // Usage example:
    //   rq_itemH = rq_item._handle
    //   outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)  // 0-based!
    //   ae.sdk.AEGP_SetOutputFilePath(rq_itemH, outmodH, "D:/test.mov")
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetOutputModuleByIndex", [](uintptr_t rq_itemH_ptr, int index) -> uintptr_t {
        // NOTE: This function uses 0-based indexing (exception in AEGP SDK)
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = nullptr;

        A_Err err = suites.outputModuleSuite->AEGP_GetOutputModuleByIndex(rq_itemH, index, &outmodH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetOutputModuleByIndex failed (error code: " +
                                   std::to_string(err) + ", rq_itemH: " + std::to_string(rq_itemH_ptr) +
                                   ", index: " + std::to_string(index) + ")");
        }

        return reinterpret_cast<uintptr_t>(outmodH);
    }, py::arg("rq_itemH"), py::arg("index"));

    sdk.def("AEGP_SetOutputFilePath", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr, const std::wstring& path) {
        // No validation - pass through directly to SDK for testing
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        const A_UTF16Char* pathZ = reinterpret_cast<const A_UTF16Char*>(path.c_str());
        A_Err err = suites.outputModuleSuite->AEGP_SetOutputFilePath(rq_itemH, outmodH, pathZ);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetOutputFilePath failed (error code: " + std::to_string(err) +
                                   ", rq_itemH: " + std::to_string(rq_itemH_ptr) +
                                   ", outmodH: " + std::to_string(outmodH_ptr) + ")");
        }
    }, py::arg("rq_itemH"), py::arg("outmodH"), py::arg("path"));

    sdk.def("AEGP_GetOutputFilePath", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> std::wstring {
        // No validation - pass through directly to SDK for testing
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);
        AEGP_MemHandle pathH = nullptr;

        A_Err err = suites.outputModuleSuite->AEGP_GetOutputFilePath(rq_itemH, outmodH, &pathH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetOutputFilePath failed (error code: " + std::to_string(err) +
                                   ", rq_itemH: " + std::to_string(rq_itemH_ptr) +
                                   ", outmodH: " + std::to_string(outmodH_ptr) + ")");
        }

        if (!pathH) return std::wstring();

        // Use RAII helpers to ensure memory is always freed
        PyAE::ScopedMemHandle scopedHandle(state.GetPluginID(), suites.memorySuite, pathH);
        PyAE::ScopedMemLock lock(suites.memorySuite, pathH);
        if (!lock.IsValid()) {
            return std::wstring();
        }

        return std::wstring(reinterpret_cast<const wchar_t*>(lock.As<A_UTF16Char>()));
    }, py::arg("rq_itemH"), py::arg("outmodH"));

    sdk.def("AEGP_AddDefaultOutputModule", [](uintptr_t rq_itemH_ptr) -> uintptr_t {
        // NOTE: Queuebert SDK sample uses 0 for rq_itemH (first RQ item)
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = nullptr;

        A_Err err = suites.outputModuleSuite->AEGP_AddDefaultOutputModule(rq_itemH, &outmodH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_AddDefaultOutputModule failed");

        return reinterpret_cast<uintptr_t>(outmodH);
    }, py::arg("rq_itemH"));

    // -----------------------------------------------------------------------
    // Embed Options
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetEmbedOptions", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> int {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // This appears to be a legacy/undocumented behavior where 0 means "first item"
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        AEGP_EmbeddingType embed_options = AEGP_Embedding_NONE;
        A_Err err = suites.outputModuleSuite->AEGP_GetEmbedOptions(rq_itemH, outmodH, &embed_options);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEmbedOptions failed");

        return static_cast<int>(embed_options);
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Get embedding options for output module.");

    sdk.def("AEGP_SetEmbedOptions", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr, int embed_options) {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Err err = suites.outputModuleSuite->AEGP_SetEmbedOptions(rq_itemH, outmodH, static_cast<AEGP_EmbeddingType>(embed_options));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetEmbedOptions failed");
    }, py::arg("rq_itemH"), py::arg("outmodH"), py::arg("embed_options"), "Set embedding options for output module.");

    // -----------------------------------------------------------------------
    // Post Render Action
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetPostRenderAction", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> int {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        AEGP_PostRenderAction post_render_action = AEGP_PostRenderOptions_NONE;
        A_Err err = suites.outputModuleSuite->AEGP_GetPostRenderAction(rq_itemH, outmodH, &post_render_action);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetPostRenderAction failed");

        return static_cast<int>(post_render_action);
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Get post-render action for output module.");

    sdk.def("AEGP_SetPostRenderAction", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr, int post_render_action) {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Err err = suites.outputModuleSuite->AEGP_SetPostRenderAction(rq_itemH, outmodH, static_cast<AEGP_PostRenderAction>(post_render_action));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetPostRenderAction failed");
    }, py::arg("rq_itemH"), py::arg("outmodH"), py::arg("post_render_action"), "Set post-render action for output module.");

    // -----------------------------------------------------------------------
    // Enabled Outputs
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetEnabledOutputs", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> int {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        AEGP_OutputTypes enabled_types = AEGP_OutputType_NONE;
        A_Err err = suites.outputModuleSuite->AEGP_GetEnabledOutputs(rq_itemH, outmodH, &enabled_types);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetEnabledOutputs failed");

        return static_cast<int>(enabled_types);
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Get enabled output types (bitfield: VIDEO=1, AUDIO=2).");

    sdk.def("AEGP_SetEnabledOutputs", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr, int enabled_types) {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Err err = suites.outputModuleSuite->AEGP_SetEnabledOutputs(rq_itemH, outmodH, static_cast<AEGP_OutputTypes>(enabled_types));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetEnabledOutputs failed");
    }, py::arg("rq_itemH"), py::arg("outmodH"), py::arg("enabled_types"), "Set enabled output types (bitfield: VIDEO=1, AUDIO=2).");

    // -----------------------------------------------------------------------
    // Output Channels
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetOutputChannels", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> int {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        AEGP_VideoChannels output_channels = AEGP_VideoChannels_NONE;
        A_Err err = suites.outputModuleSuite->AEGP_GetOutputChannels(rq_itemH, outmodH, &output_channels);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutputChannels failed");

        return static_cast<int>(output_channels);
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Get output video channels.");

    sdk.def("AEGP_SetOutputChannels", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr, int output_channels) {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Err err = suites.outputModuleSuite->AEGP_SetOutputChannels(rq_itemH, outmodH, static_cast<AEGP_VideoChannels>(output_channels));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutputChannels failed");
    }, py::arg("rq_itemH"), py::arg("outmodH"), py::arg("output_channels"), "Set output video channels.");

    // -----------------------------------------------------------------------
    // Stretch Info
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetStretchInfo", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> py::dict {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Boolean is_enabled = FALSE;
        AEGP_StretchQuality stretch_quality = AEGP_StretchQual_NONE;
        A_Boolean locked = FALSE;

        A_Err err = suites.outputModuleSuite->AEGP_GetStretchInfo(rq_itemH, outmodH, &is_enabled, &stretch_quality, &locked);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStretchInfo failed");

        py::dict d;
        d["is_enabled"] = static_cast<bool>(is_enabled);
        d["stretch_quality"] = static_cast<int>(stretch_quality);
        d["locked"] = static_cast<bool>(locked);
        return d;
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Get stretch info for output module.");

    sdk.def("AEGP_SetStretchInfo", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr, bool is_enabled, int stretch_quality) {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Err err = suites.outputModuleSuite->AEGP_SetStretchInfo(rq_itemH, outmodH, is_enabled, static_cast<AEGP_StretchQuality>(stretch_quality));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetStretchInfo failed");
    }, py::arg("rq_itemH"), py::arg("outmodH"), py::arg("is_enabled"), py::arg("stretch_quality"), "Set stretch info for output module.");

    // -----------------------------------------------------------------------
    // Crop Info
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetCropInfo", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> py::dict {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Boolean is_enabled = FALSE;
        A_Rect crop_rect = {0, 0, 0, 0};

        A_Err err = suites.outputModuleSuite->AEGP_GetCropInfo(rq_itemH, outmodH, &is_enabled, &crop_rect);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCropInfo failed");

        py::dict d;
        d["is_enabled"] = static_cast<bool>(is_enabled);
        d["left"] = crop_rect.left;
        d["top"] = crop_rect.top;
        d["right"] = crop_rect.right;
        d["bottom"] = crop_rect.bottom;
        return d;
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Get crop info for output module.");

    sdk.def("AEGP_SetCropInfo", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr, bool is_enabled, const py::dict& crop_rect) {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Rect rect = {0, 0, 0, 0};
        if (crop_rect.contains("left")) rect.left = crop_rect["left"].cast<A_long>();
        if (crop_rect.contains("top")) rect.top = crop_rect["top"].cast<A_long>();
        if (crop_rect.contains("right")) rect.right = crop_rect["right"].cast<A_long>();
        if (crop_rect.contains("bottom")) rect.bottom = crop_rect["bottom"].cast<A_long>();

        A_Err err = suites.outputModuleSuite->AEGP_SetCropInfo(rq_itemH, outmodH, is_enabled, rect);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCropInfo failed");
    }, py::arg("rq_itemH"), py::arg("outmodH"), py::arg("is_enabled"), py::arg("crop_rect"), "Set crop info for output module.");

    // -----------------------------------------------------------------------
    // Sound Format Info
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetSoundFormatInfo", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> py::dict {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // This appears to be a legacy/undocumented behavior where 0 means "first item"
        // We allow 0 values for compatibility testing
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        AEGP_SoundDataFormat sound_format_info = {0.0, 0, 0, 0};
        A_Boolean audio_enabled = FALSE;

        A_Err err = suites.outputModuleSuite->AEGP_GetSoundFormatInfo(rq_itemH, outmodH, &sound_format_info, &audio_enabled);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetSoundFormatInfo failed");

        py::dict d;
        d["sample_rate"] = sound_format_info.sample_rateF;
        d["encoding"] = static_cast<int>(sound_format_info.encoding);
        d["bytes_per_sample"] = sound_format_info.bytes_per_sampleL;
        d["num_channels"] = sound_format_info.num_channelsL;
        d["audio_enabled"] = static_cast<bool>(audio_enabled);
        return d;
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Get sound format info for output module.");

    sdk.def("AEGP_SetSoundFormatInfo", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr, const py::dict& sound_format, bool audio_enabled) {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        // CRITICAL: Check if the Output Module supports audio before attempting to set sound format
        // Calling AEGP_SetSoundFormatInfo on a non-audio Output Module causes memory errors
        AEGP_OutputTypes enabled_types = AEGP_OutputType_NONE;
        A_Err err = suites.outputModuleSuite->AEGP_GetEnabledOutputs(rq_itemH, outmodH, &enabled_types);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get enabled outputs: error code " + std::to_string(err));
        }

        // Check if audio output is supported (AEGP_OutputType_AUDIO = 2)
        const bool audio_supported = (enabled_types & AEGP_OutputType_AUDIO) != 0;
        if (!audio_supported && audio_enabled) {
            throw std::runtime_error(
                "Cannot enable audio on this Output Module: audio output is not supported by the current format. "
                "The Output Module's enabled_types=" + std::to_string(enabled_types) + " does not include AUDIO (2).");
        }

        // IMPORTANT: AEGP_SetSoundFormatInfo requires ALL fields to be properly initialized
        // We must get current values first, then modify only what the user specified
        AEGP_SoundDataFormat current_format;
        A_Boolean current_audio_enabled = FALSE;
        err = suites.outputModuleSuite->AEGP_GetSoundFormatInfo(rq_itemH, outmodH, &current_format, &current_audio_enabled);

        // If Get fails, initialize with safe defaults
        if (err != A_Err_NONE) {
            // Use safe default values
            current_format.sample_rateF = 44100.0;
            current_format.encoding = AEGP_SoundEncoding_UNSIGNED_PCM;
            current_format.bytes_per_sampleL = 2;
            current_format.num_channelsL = 1;
        }

        // Update only the fields provided by user
        if (sound_format.contains("sample_rate")) {
            current_format.sample_rateF = sound_format["sample_rate"].cast<A_FpLong>();
        }
        if (sound_format.contains("encoding")) {
            current_format.encoding = static_cast<AEGP_SoundEncoding>(sound_format["encoding"].cast<int>());
        }
        if (sound_format.contains("bytes_per_sample")) {
            current_format.bytes_per_sampleL = sound_format["bytes_per_sample"].cast<A_long>();
        }
        if (sound_format.contains("num_channels")) {
            current_format.num_channelsL = sound_format["num_channels"].cast<A_long>();
        }

        // Set the sound format (only if audio is supported or we're disabling audio)
        err = suites.outputModuleSuite->AEGP_SetSoundFormatInfo(rq_itemH, outmodH, current_format, audio_enabled ? TRUE : FALSE);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetSoundFormatInfo failed with error code: " + std::to_string(err));
        }
    }, py::arg("rq_itemH"), py::arg("outmodH"), py::arg("sound_format"), py::arg("audio_enabled"), "Set sound format info for output module.");

    // -----------------------------------------------------------------------
    // Extra Output Module Info
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetExtraOutputModuleInfo", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) -> py::dict {
        // NOTE: Queuebert SDK sample uses 0, 0 for rq_itemH and outmodH
        // Validation::RequireNonNull(rq_itemH_ptr, "rq_itemH");
        // Validation::RequireNonNull(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.outputModuleSuite) throw std::runtime_error("OutputModule Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        AEGP_MemHandle format_unicodeH = nullptr;
        AEGP_MemHandle info_unicodeH = nullptr;
        A_Boolean is_sequence = FALSE;
        A_Boolean multi_frame = FALSE;

        A_Err err = suites.outputModuleSuite->AEGP_GetExtraOutputModuleInfo(
            rq_itemH, outmodH, &format_unicodeH, &info_unicodeH, &is_sequence, &multi_frame);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetExtraOutputModuleInfo failed");

        py::dict d;

        // Use RAII helpers to extract format string safely
        if (format_unicodeH) {
            PyAE::ScopedMemHandle scopedFormat(state.GetPluginID(), suites.memorySuite, format_unicodeH);
            PyAE::ScopedMemLock formatLock(suites.memorySuite, format_unicodeH);
            if (formatLock.IsValid()) {
                d["format"] = std::wstring(reinterpret_cast<const wchar_t*>(formatLock.As<A_UTF16Char>()));
            } else {
                d["format"] = std::wstring();
            }
        } else {
            d["format"] = std::wstring();
        }

        // Use RAII helpers to extract info string safely
        if (info_unicodeH) {
            PyAE::ScopedMemHandle scopedInfo(state.GetPluginID(), suites.memorySuite, info_unicodeH);
            PyAE::ScopedMemLock infoLock(suites.memorySuite, info_unicodeH);
            if (infoLock.IsValid()) {
                d["info"] = std::wstring(reinterpret_cast<const wchar_t*>(infoLock.As<A_UTF16Char>()));
            } else {
                d["info"] = std::wstring();
            }
        } else {
            d["info"] = std::wstring();
        }

        d["is_sequence"] = static_cast<bool>(is_sequence);
        d["multi_frame"] = static_cast<bool>(multi_frame);

        return d;
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Get extra output module info (format, info strings, sequence flags).");
}

} // namespace SDK
} // namespace PyAE
