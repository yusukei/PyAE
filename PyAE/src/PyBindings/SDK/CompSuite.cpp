#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_CompSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_CompSuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetCompFromItem", [](uintptr_t itemH_ptr) -> uintptr_t {
        // Argument validation
        if (itemH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompFromItem: itemH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_CompH compH = nullptr;
        A_Err err = suites.compSuite->AEGP_GetCompFromItem(
            reinterpret_cast<AEGP_ItemH>(itemH_ptr), &compH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFromItem failed with error: " + std::to_string(err));

        return reinterpret_cast<uintptr_t>(compH);
    }, py::arg("itemH"));

    sdk.def("AEGP_GetItemFromComp", [](uintptr_t compH_ptr) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemFromComp: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_ItemH itemH = nullptr;
        A_Err err = suites.compSuite->AEGP_GetItemFromComp(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetItemFromComp failed with error: " + std::to_string(err));

        return reinterpret_cast<uintptr_t>(itemH);
    }, py::arg("compH"));

    sdk.def("AEGP_GetCompFlags", [](uintptr_t compH_ptr) -> int {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompFlags: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_CompFlags flags;
        A_Err err = suites.compSuite->AEGP_GetCompFlags(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &flags);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFlags failed");

        return (int)flags;
    }, py::arg("compH"));

    sdk.def("AEGP_GetCompBGColor", [](uintptr_t compH_ptr) -> py::tuple {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompBGColor: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_ColorVal color;
        A_Err err = suites.compSuite->AEGP_GetCompBGColor(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompBGColor failed");

        return py::make_tuple(color.redF, color.greenF, color.blueF);
    }, py::arg("compH"));

    sdk.def("AEGP_SetCompBGColor", [](uintptr_t compH_ptr, float r, float g, float b) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompBGColor: compH cannot be null");
        }
        Validation::RequireColorRange(r, "AEGP_SetCompBGColor: red");
        Validation::RequireColorRange(g, "AEGP_SetCompBGColor: green");
        Validation::RequireColorRange(b, "AEGP_SetCompBGColor: blue");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_ColorVal color;
        color.alphaF = 1.0;
        color.redF = (A_FpLong)r;
        color.greenF = (A_FpLong)g;
        color.blueF = (A_FpLong)b;

        A_Err err = suites.compSuite->AEGP_SetCompBGColor(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompBGColor failed");
    }, py::arg("compH"), py::arg("r"), py::arg("g"), py::arg("b"));

    sdk.def("AEGP_GetCompFramerate", [](uintptr_t compH_ptr) -> double {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompFramerate: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_FpLong fps = 0.0;
        A_Err err = suites.compSuite->AEGP_GetCompFramerate(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        return (double)fps;
    }, py::arg("compH"));

    sdk.def("AEGP_GetCompDownSampleFactor", [](uintptr_t compH_ptr) -> py::tuple {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompDownSampleFactor: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_DownsampleFactor dsf;
        A_Err err = suites.compSuite->AEGP_GetCompDownsampleFactor(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &dsf);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompDownsampleFactor failed");

        return py::make_tuple(dsf.xS, dsf.yS);
    }, py::arg("compH"));

    sdk.def("AEGP_SetCompDownSampleFactor", [](uintptr_t compH_ptr, int x, int y) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompDownSampleFactor: compH cannot be null");
        }
        Validation::RequirePositive(x, "AEGP_SetCompDownSampleFactor: x");
        Validation::RequirePositive(y, "AEGP_SetCompDownSampleFactor: y");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_DownsampleFactor dsf;
        dsf.xS = (A_short)x;
        dsf.yS = (A_short)y;

        A_Err err = suites.compSuite->AEGP_SetCompDownsampleFactor(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &dsf);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompDownsampleFactor failed");
    }, py::arg("compH"), py::arg("x"), py::arg("y"));

    sdk.def("AEGP_SetCompFrameRate", [](uintptr_t compH_ptr, double fps) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompFrameRate: compH cannot be null");
        }
        Validation::RequirePositive(fps, "AEGP_SetCompFrameRate: fps");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_FpLong fpsVal = (A_FpLong)fps;

        A_Err err = suites.compSuite->AEGP_SetCompFrameRate(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &fpsVal);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompFrameRate failed");
    }, py::arg("compH"), py::arg("fps"));

    sdk.def("AEGP_SetCompFrameDuration", [](uintptr_t compH_ptr, double duration) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompFrameDuration: compH cannot be null");
        }
        Validation::RequirePositive(duration, "AEGP_SetCompFrameDuration: duration");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_FpLong fps = 1.0 / duration;

        A_Err err = suites.compSuite->AEGP_SetCompFrameRate(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompFrameRate failed");
    }, py::arg("compH"), py::arg("duration"));

    // Composition manipulation functions
    sdk.def("AEGP_SetCompPixelAspectRatio", [](uintptr_t compH_ptr, int num, int den) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompPixelAspectRatio: compH cannot be null");
        }
        Validation::RequirePositive(num, "AEGP_SetCompPixelAspectRatio: num");
        Validation::RequireNonZero(den, "AEGP_SetCompPixelAspectRatio: den");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Ratio ratio;
        ratio.num = static_cast<A_long>(num);
        ratio.den = static_cast<A_u_long>(den);

        A_Err err = suites.compSuite->AEGP_SetCompPixelAspectRatio(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &ratio);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompPixelAspectRatio failed");
    }, py::arg("compH"), py::arg("num"), py::arg("den"));

    sdk.def("AEGP_SetCompDimensions", [](uintptr_t compH_ptr, int width, int height) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompDimensions: compH cannot be null");
        }
        Validation::RequireValidCompDimensions(width, height);

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Err err = suites.compSuite->AEGP_SetCompDimensions(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            static_cast<A_long>(width),
            static_cast<A_long>(height));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompDimensions failed");
    }, py::arg("compH"), py::arg("width"), py::arg("height"));

    sdk.def("AEGP_DuplicateComp", [](uintptr_t compH_ptr) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_DuplicateComp: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_CompH newCompH = nullptr;
        A_Err err = suites.compSuite->AEGP_DuplicateComp(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &newCompH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DuplicateComp failed");

        return reinterpret_cast<uintptr_t>(newCompH);
    }, py::arg("compH"));

    sdk.def("AEGP_GetCompFrameDuration", [](uintptr_t compH_ptr) -> double {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompFrameDuration: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Time frameTime = {0, 1};
        A_Err err = suites.compSuite->AEGP_GetCompFrameDuration(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &frameTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFrameDuration failed");

        if (frameTime.scale == 0) return 0.0;
        return static_cast<double>(frameTime.value) / static_cast<double>(frameTime.scale);
    }, py::arg("compH"));

    sdk.def("AEGP_GetMostRecentlyUsedComp", []() -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_CompH compH = nullptr;
        A_Err err = suites.compSuite->AEGP_GetMostRecentlyUsedComp(&compH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMostRecentlyUsedComp failed");

        return reinterpret_cast<uintptr_t>(compH);
    });

    // Display settings
    sdk.def("AEGP_GetShowLayerNameOrSourceName", [](uintptr_t compH_ptr) -> bool {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetShowLayerNameOrSourceName: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Boolean showLayerNames = FALSE;
        A_Err err = suites.compSuite->AEGP_GetShowLayerNameOrSourceName(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &showLayerNames);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetShowLayerNameOrSourceName failed");

        return showLayerNames != FALSE;
    }, py::arg("compH"));

    sdk.def("AEGP_SetShowLayerNameOrSourceName", [](uintptr_t compH_ptr, bool showLayerNames) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetShowLayerNameOrSourceName: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Err err = suites.compSuite->AEGP_SetShowLayerNameOrSourceName(
            reinterpret_cast<AEGP_CompH>(compH_ptr), showLayerNames ? TRUE : FALSE);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetShowLayerNameOrSourceName failed");
    }, py::arg("compH"), py::arg("show_layer_names"));

    sdk.def("AEGP_GetShowBlendModes", [](uintptr_t compH_ptr) -> bool {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetShowBlendModes: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Boolean showBlendModes = FALSE;
        A_Err err = suites.compSuite->AEGP_GetShowBlendModes(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &showBlendModes);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetShowBlendModes failed");

        return showBlendModes != FALSE;
    }, py::arg("compH"));

    sdk.def("AEGP_SetShowBlendModes", [](uintptr_t compH_ptr, bool showBlendModes) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetShowBlendModes: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Err err = suites.compSuite->AEGP_SetShowBlendModes(
            reinterpret_cast<AEGP_CompH>(compH_ptr), showBlendModes ? TRUE : FALSE);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetShowBlendModes failed");
    }, py::arg("compH"), py::arg("show_blend_modes"));

    sdk.def("AEGP_GetCompDisplayStartTime", [](uintptr_t compH_ptr) -> double {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompDisplayStartTime: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Time startTime = {0, 1};
        A_Err err = suites.compSuite->AEGP_GetCompDisplayStartTime(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &startTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompDisplayStartTime failed");

        if (startTime.scale == 0) return 0.0;
        return static_cast<double>(startTime.value) / static_cast<double>(startTime.scale);
    }, py::arg("compH"));

    sdk.def("AEGP_SetCompDisplayStartTime", [](uintptr_t compH_ptr, double seconds) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompDisplayStartTime: compH cannot be null");
        }
        Validation::RequireNonNegative(seconds, "AEGP_SetCompDisplayStartTime: seconds");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        // Get framerate to convert seconds to A_Time
        A_FpLong fps = 30.0;
        A_Err err = suites.compSuite->AEGP_GetCompFramerate(reinterpret_cast<AEGP_CompH>(compH_ptr), &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time startTime = AETypeUtils::SecondsToTimeWithFps(seconds, fps);

        err = suites.compSuite->AEGP_SetCompDisplayStartTime(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &startTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompDisplayStartTime failed");
    }, py::arg("compH"), py::arg("seconds"));

    sdk.def("AEGP_GetCompDisplayDropFrame", [](uintptr_t compH_ptr) -> bool {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompDisplayDropFrame: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Boolean dropFrame = FALSE;
        A_Err err = suites.compSuite->AEGP_GetCompDisplayDropFrame(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &dropFrame);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompDisplayDropFrame failed");

        return dropFrame != FALSE;
    }, py::arg("compH"));

    // Motion Blur settings
    sdk.def("AEGP_GetCompShutterAnglePhase", [](uintptr_t compH_ptr) -> py::tuple {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompShutterAnglePhase: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Ratio angle = {0, 1};
        A_Ratio phase = {0, 1};
        A_Err err = suites.compSuite->AEGP_GetCompShutterAnglePhase(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &angle, &phase);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompShutterAnglePhase failed");

        double angleVal = (angle.den != 0) ? static_cast<double>(angle.num) / angle.den : 0.0;
        double phaseVal = (phase.den != 0) ? static_cast<double>(phase.num) / phase.den : 0.0;
        return py::make_tuple(angleVal, phaseVal);
    }, py::arg("compH"));

    sdk.def("AEGP_GetCompShutterFrameRange", [](uintptr_t compH_ptr, double compTime) -> py::tuple {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompShutterFrameRange: compH cannot be null");
        }
        Validation::RequireNonNegative(compTime, "AEGP_GetCompShutterFrameRange: compTime");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        // Convert compTime to A_Time
        A_FpLong fps = 30.0;
        A_Err err = suites.compSuite->AEGP_GetCompFramerate(reinterpret_cast<AEGP_CompH>(compH_ptr), &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time compTimeVal = AETypeUtils::SecondsToTimeWithFps(compTime, fps);

        A_Time start = {0, 1};
        A_Time duration = {0, 1};
        err = suites.compSuite->AEGP_GetCompShutterFrameRange(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &compTimeVal, &start, &duration);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompShutterFrameRange failed");

        double startSec = (start.scale != 0) ? static_cast<double>(start.value) / start.scale : 0.0;
        double durationSec = (duration.scale != 0) ? static_cast<double>(duration.value) / duration.scale : 0.0;
        return py::make_tuple(startSec, durationSec);
    }, py::arg("compH"), py::arg("comp_time"));

    sdk.def("AEGP_GetCompSuggestedMotionBlurSamples", [](uintptr_t compH_ptr) -> int {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompSuggestedMotionBlurSamples: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_long samples = 0;
        A_Err err = suites.compSuite->AEGP_GetCompSuggestedMotionBlurSamples(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &samples);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompSuggestedMotionBlurSamples failed");

        return static_cast<int>(samples);
    }, py::arg("compH"));

    sdk.def("AEGP_SetCompSuggestedMotionBlurSamples", [](uintptr_t compH_ptr, int samples) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompSuggestedMotionBlurSamples: compH cannot be null");
        }
        Validation::RequirePositive(samples, "AEGP_SetCompSuggestedMotionBlurSamples: samples");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Err err = suites.compSuite->AEGP_SetCompSuggestedMotionBlurSamples(
            reinterpret_cast<AEGP_CompH>(compH_ptr), static_cast<A_long>(samples));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompSuggestedMotionBlurSamples failed");
    }, py::arg("compH"), py::arg("samples"));

    sdk.def("AEGP_GetCompMotionBlurAdaptiveSampleLimit", [](uintptr_t compH_ptr) -> int {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompMotionBlurAdaptiveSampleLimit: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_long samples = 0;
        A_Err err = suites.compSuite->AEGP_GetCompMotionBlurAdaptiveSampleLimit(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &samples);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompMotionBlurAdaptiveSampleLimit failed");

        return static_cast<int>(samples);
    }, py::arg("compH"));

    sdk.def("AEGP_SetCompMotionBlurAdaptiveSampleLimit", [](uintptr_t compH_ptr, int samples) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompMotionBlurAdaptiveSampleLimit: compH cannot be null");
        }
        Validation::RequirePositive(samples, "AEGP_SetCompMotionBlurAdaptiveSampleLimit: samples");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Err err = suites.compSuite->AEGP_SetCompMotionBlurAdaptiveSampleLimit(
            reinterpret_cast<AEGP_CompH>(compH_ptr), static_cast<A_long>(samples));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompMotionBlurAdaptiveSampleLimit failed");
    }, py::arg("compH"), py::arg("samples"));

    // Text layer creation
#if defined(kAEGPCompSuiteVersion12)
    // AE 24.0+ (CompSuite12): supports horizontal parameter
    sdk.def("AEGP_CreateBoxTextLayerInComp", [](uintptr_t compH_ptr, bool selectNewLayer, double boxWidth, double boxHeight, bool horizontal) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateBoxTextLayerInComp: compH cannot be null");
        }
        Validation::RequirePositive(boxWidth, "AEGP_CreateBoxTextLayerInComp: boxWidth");
        Validation::RequirePositive(boxHeight, "AEGP_CreateBoxTextLayerInComp: boxHeight");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_FloatPoint boxDimensions;
        boxDimensions.x = static_cast<A_FpLong>(boxWidth);
        boxDimensions.y = static_cast<A_FpLong>(boxHeight);

        AEGP_LayerH newLayerH = nullptr;
        A_Err err = suites.compSuite->AEGP_CreateBoxTextLayerInComp(
            reinterpret_cast<AEGP_CompH>(compH_ptr),  // parent_compH
            selectNewLayer ? TRUE : FALSE,            // select_new_layerB
            boxDimensions,                             // box_dimensions
            horizontal ? TRUE : FALSE,                 // horzB (CompSuite12 only)
            &newLayerH);                               // new_text_layerPH
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateBoxTextLayerInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"), py::arg("select_new_layer"), py::arg("box_width"), py::arg("box_height"), py::arg("horizontal"));
#else
    // AE 22.x/23.x (CompSuite11): no horizontal parameter
    sdk.def("AEGP_CreateBoxTextLayerInComp", [](uintptr_t compH_ptr, bool selectNewLayer, double boxWidth, double boxHeight) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateBoxTextLayerInComp: compH cannot be null");
        }
        Validation::RequirePositive(boxWidth, "AEGP_CreateBoxTextLayerInComp: boxWidth");
        Validation::RequirePositive(boxHeight, "AEGP_CreateBoxTextLayerInComp: boxHeight");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_FloatPoint boxDimensions;
        boxDimensions.x = static_cast<A_FpLong>(boxWidth);
        boxDimensions.y = static_cast<A_FpLong>(boxHeight);

        AEGP_LayerH newLayerH = nullptr;
        A_Err err = suites.compSuite->AEGP_CreateBoxTextLayerInComp(
            reinterpret_cast<AEGP_CompH>(compH_ptr),  // parent_compH
            selectNewLayer ? TRUE : FALSE,            // select_new_layerB
            boxDimensions,                             // box_dimensions
            &newLayerH);                               // new_text_layerPH
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateBoxTextLayerInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"), py::arg("select_new_layer"), py::arg("box_width"), py::arg("box_height"));
#endif

    // Composition marker stream
    sdk.def("AEGP_GetNewCompMarkerStream", [](uintptr_t compH_ptr) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNewCompMarkerStream: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_StreamRefH streamH = nullptr;
        A_Err err = suites.compSuite->AEGP_GetNewCompMarkerStream(
            state.GetPluginID(),
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &streamH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewCompMarkerStream failed");

        return reinterpret_cast<uintptr_t>(streamH);
    }, py::arg("compH"));

    // -----------------------------------------------------------------------
    // Layer Creation Functions (already used in PyComp_Core.cpp)
    // -----------------------------------------------------------------------

    sdk.def("AEGP_CreateSolidInComp", [](uintptr_t compH_ptr, const std::string& name, int width, int height,
                                          float red, float green, float blue, double duration) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateSolidInComp: compH cannot be null");
        }
        Validation::RequireNonEmpty(name, "AEGP_CreateSolidInComp: name");
        Validation::RequireValidCompDimensions(width, height);
        Validation::RequireColorRange(red, "AEGP_CreateSolidInComp: red");
        Validation::RequireColorRange(green, "AEGP_CreateSolidInComp: green");
        Validation::RequireColorRange(blue, "AEGP_CreateSolidInComp: blue");
        Validation::RequirePositive(duration, "AEGP_CreateSolidInComp: duration");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_ColorVal color;
        color.alphaF = 1.0;
        color.redF = static_cast<A_FpLong>(red);
        color.greenF = static_cast<A_FpLong>(green);
        color.blueF = static_cast<A_FpLong>(blue);

        // Get framerate to convert duration to A_Time
        A_FpLong fps = 30.0;
        A_Err err = suites.compSuite->AEGP_GetCompFramerate(reinterpret_cast<AEGP_CompH>(compH_ptr), &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(duration, fps);

        std::wstring wname = StringUtils::Utf8ToWide(name);
        AEGP_LayerH newLayerH = nullptr;
        err = suites.compSuite->AEGP_CreateSolidInComp(
            reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
            static_cast<A_long>(width),
            static_cast<A_long>(height),
            &color,
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &durationTime,
            &newLayerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateSolidInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"), py::arg("name"), py::arg("width"), py::arg("height"),
       py::arg("red"), py::arg("green"), py::arg("blue"), py::arg("duration"));

    sdk.def("AEGP_CreateCameraInComp", [](uintptr_t compH_ptr, const std::string& name) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateCameraInComp: compH cannot be null");
        }
        Validation::RequireNonEmpty(name, "AEGP_CreateCameraInComp: name");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        std::wstring wname = StringUtils::Utf8ToWide(name);
        AEGP_LayerH newLayerH = nullptr;
        A_Err err = suites.compSuite->AEGP_CreateCameraInComp(
            reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
            A_FloatPoint{0.0, 0.0},  // center_point
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &newLayerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateCameraInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"), py::arg("name"));

    sdk.def("AEGP_CreateLightInComp", [](uintptr_t compH_ptr, const std::string& name, int lightType) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateLightInComp: compH cannot be null");
        }
        Validation::RequireNonEmpty(name, "AEGP_CreateLightInComp: name");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        std::wstring wname = StringUtils::Utf8ToWide(name);
        AEGP_LayerH newLayerH = nullptr;
        A_Err err = suites.compSuite->AEGP_CreateLightInComp(
            reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
            A_FloatPoint{0.0, 0.0},  // center_point
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &newLayerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateLightInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"), py::arg("name"), py::arg("light_type"));

    sdk.def("AEGP_CreateNullInComp", [](uintptr_t compH_ptr, const std::string& name, double duration) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateNullInComp: compH cannot be null");
        }
        Validation::RequireNonEmpty(name, "AEGP_CreateNullInComp: name");
        Validation::RequirePositive(duration, "AEGP_CreateNullInComp: duration");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        // Get framerate to convert duration to A_Time
        A_FpLong fps = 30.0;
        A_Err err = suites.compSuite->AEGP_GetCompFramerate(reinterpret_cast<AEGP_CompH>(compH_ptr), &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(duration, fps);

        std::wstring wname = StringUtils::Utf8ToWide(name);
        AEGP_LayerH newLayerH = nullptr;
        err = suites.compSuite->AEGP_CreateNullInComp(
            reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &durationTime,
            &newLayerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateNullInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"), py::arg("name"), py::arg("duration"));

    sdk.def("AEGP_CreateComp", [](uintptr_t parentFolderH_ptr, const std::string& name, int width, int height,
                                   int pixelAspectNum, int pixelAspectDen, double duration, double framerate) -> uintptr_t {
        // Argument validation
        // parentFolderH can be 0 (null) for root folder
        Validation::RequireNonEmpty(name, "AEGP_CreateComp: name");
        Validation::RequireValidCompDimensions(width, height);
        Validation::RequirePositive(pixelAspectNum, "AEGP_CreateComp: pixelAspectNum");
        Validation::RequireNonZero(pixelAspectDen, "AEGP_CreateComp: pixelAspectDen");
        Validation::RequirePositive(duration, "AEGP_CreateComp: duration");
        Validation::RequirePositive(framerate, "AEGP_CreateComp: framerate");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Ratio pixelAspect;
        pixelAspect.num = static_cast<A_long>(pixelAspectNum);
        pixelAspect.den = static_cast<A_u_long>(pixelAspectDen);

        A_Time durationTime;
        durationTime.value = static_cast<A_long>(duration * framerate);
        durationTime.scale = static_cast<A_u_long>(framerate);

        A_Ratio fpsRatio;
        fpsRatio.num = static_cast<A_long>(framerate * 1000);
        fpsRatio.den = 1000;

        std::wstring wname = StringUtils::Utf8ToWide(name);
        AEGP_CompH newCompH = nullptr;
        A_Err err = suites.compSuite->AEGP_CreateComp(
            reinterpret_cast<AEGP_ItemH>(parentFolderH_ptr),
            reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
            static_cast<A_long>(width),
            static_cast<A_long>(height),
            &pixelAspect,
            &durationTime,
            &fpsRatio,
            &newCompH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateComp failed");

        return reinterpret_cast<uintptr_t>(newCompH);
    }, py::arg("parentFolderH"), py::arg("name"), py::arg("width"), py::arg("height"),
       py::arg("pixel_aspect_num"), py::arg("pixel_aspect_den"), py::arg("duration"), py::arg("framerate"));

#if defined(kAEGPCompSuiteVersion12)
    // AE 24.0+ (CompSuite12): supports horizontal parameter
    sdk.def("AEGP_CreateTextLayerInComp", [](uintptr_t compH_ptr, bool selectNewLayer, bool horizontal) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateTextLayerInComp: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_LayerH newLayerH = nullptr;
        A_Err err = suites.compSuite->AEGP_CreateTextLayerInComp(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            selectNewLayer ? TRUE : FALSE,
            horizontal ? TRUE : FALSE,  // horzB (CompSuite12 only)
            &newLayerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateTextLayerInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"), py::arg("select_new_layer"), py::arg("horizontal"));
#else
    // AE 22.x/23.x (CompSuite11): no horizontal parameter
    sdk.def("AEGP_CreateTextLayerInComp", [](uintptr_t compH_ptr, bool selectNewLayer) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateTextLayerInComp: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_LayerH newLayerH = nullptr;
        A_Err err = suites.compSuite->AEGP_CreateTextLayerInComp(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            selectNewLayer ? TRUE : FALSE,
            &newLayerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateTextLayerInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"), py::arg("select_new_layer"));
#endif

    sdk.def("AEGP_CreateVectorLayerInComp", [](uintptr_t compH_ptr) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateVectorLayerInComp: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_LayerH newLayerH = nullptr;
        A_Err err = suites.compSuite->AEGP_CreateVectorLayerInComp(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &newLayerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateVectorLayerInComp failed");

        return reinterpret_cast<uintptr_t>(newLayerH);
    }, py::arg("compH"));

    // -----------------------------------------------------------------------
    // Work Area Functions (already used in PyComp_Core.cpp)
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetCompWorkAreaStart", [](uintptr_t compH_ptr) -> double {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompWorkAreaStart: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Time startTime = {0, 1};
        A_Err err = suites.compSuite->AEGP_GetCompWorkAreaStart(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &startTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompWorkAreaStart failed");

        if (startTime.scale == 0) return 0.0;
        return static_cast<double>(startTime.value) / static_cast<double>(startTime.scale);
    }, py::arg("compH"));

    sdk.def("AEGP_GetCompWorkAreaDuration", [](uintptr_t compH_ptr) -> double {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetCompWorkAreaDuration: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Time durationTime = {0, 1};
        A_Err err = suites.compSuite->AEGP_GetCompWorkAreaDuration(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &durationTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompWorkAreaDuration failed");

        if (durationTime.scale == 0) return 0.0;
        return static_cast<double>(durationTime.value) / static_cast<double>(durationTime.scale);
    }, py::arg("compH"));

    sdk.def("AEGP_SetCompWorkAreaStartAndDuration", [](uintptr_t compH_ptr, double start, double duration) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompWorkAreaStartAndDuration: compH cannot be null");
        }
        Validation::RequireNonNegative(start, "AEGP_SetCompWorkAreaStartAndDuration: start");
        Validation::RequirePositive(duration, "AEGP_SetCompWorkAreaStartAndDuration: duration");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        // Get framerate to convert seconds to A_Time
        A_FpLong fps = 30.0;
        A_Err err = suites.compSuite->AEGP_GetCompFramerate(reinterpret_cast<AEGP_CompH>(compH_ptr), &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time startTime = AETypeUtils::SecondsToTimeWithFps(start, fps);
        A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(duration, fps);

        err = suites.compSuite->AEGP_SetCompWorkAreaStartAndDuration(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &startTime,
            &durationTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompWorkAreaStartAndDuration failed");
    }, py::arg("compH"), py::arg("start"), py::arg("duration"));

    // -----------------------------------------------------------------------
    // Collection/Selection Functions (already used in PyComp_Core.cpp)
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetNewCollectionFromCompSelection", [](uintptr_t compH_ptr) -> uintptr_t {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetNewCollectionFromCompSelection: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        AEGP_Collection2H collectionH = nullptr;
        A_Err err = suites.compSuite->AEGP_GetNewCollectionFromCompSelection(
            state.GetPluginID(),
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &collectionH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewCollectionFromCompSelection failed");

        return reinterpret_cast<uintptr_t>(collectionH);
    }, py::arg("compH"));

    // -----------------------------------------------------------------------
    // Phase 2: New Functions (4 completely unimplemented functions)
    // -----------------------------------------------------------------------

    sdk.def("AEGP_SetCompDuration", [](uintptr_t compH_ptr, double duration) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompDuration: compH cannot be null");
        }
        Validation::RequirePositive(duration, "AEGP_SetCompDuration: duration");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        // Get framerate to convert duration to A_Time
        A_FpLong fps = 30.0;
        A_Err err = suites.compSuite->AEGP_GetCompFramerate(reinterpret_cast<AEGP_CompH>(compH_ptr), &fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFramerate failed");

        A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(duration, fps);

        err = suites.compSuite->AEGP_SetCompDuration(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            &durationTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompDuration failed");
    }, py::arg("compH"), py::arg("duration"));

    sdk.def("AEGP_SetCompDisplayDropFrame", [](uintptr_t compH_ptr, bool dropFrame) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetCompDisplayDropFrame: compH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Err err = suites.compSuite->AEGP_SetCompDisplayDropFrame(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            dropFrame ? TRUE : FALSE);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCompDisplayDropFrame failed");
    }, py::arg("compH"), py::arg("drop_frame"));

    sdk.def("AEGP_SetSelection", [](uintptr_t compH_ptr, uintptr_t collectionH_ptr) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetSelection: compH cannot be null");
        }
        if (collectionH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetSelection: collectionH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Err err = suites.compSuite->AEGP_SetSelection(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            reinterpret_cast<AEGP_Collection2H>(collectionH_ptr));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetSelection failed");
    }, py::arg("compH"), py::arg("collectionH"));

    sdk.def("AEGP_ReorderCompSelection", [](uintptr_t compH_ptr, int index) {
        // Argument validation
        if (compH_ptr == 0) {
            throw std::invalid_argument("AEGP_ReorderCompSelection: compH cannot be null");
        }
        Validation::RequireNonNegative(index, "AEGP_ReorderCompSelection: index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compSuite) throw std::runtime_error("Comp Suite not available");

        A_Err err = suites.compSuite->AEGP_ReorderCompSelection(
            reinterpret_cast<AEGP_CompH>(compH_ptr),
            static_cast<A_long>(index));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ReorderCompSelection failed");
    }, py::arg("compH"), py::arg("index"));
}

} // namespace SDK
} // namespace PyAE
