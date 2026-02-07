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

void init_CameraSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_CameraSuite2 - Camera Layer Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetCamera", [](uintptr_t render_contextH_ptr, double comp_time_seconds) -> uintptr_t {
        // Argument validation
        if (render_contextH_ptr == 0) throw std::invalid_argument("render_contextH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.cameraSuite) throw std::runtime_error("Camera Suite not available");

        PR_RenderContextH render_contextH = reinterpret_cast<PR_RenderContextH>(render_contextH_ptr);

        // Get framerate from comp (assuming 30.0 as default)
        A_FpLong fps = 30.0;

        A_Time comp_time = AETypeUtils::SecondsToTimeWithFps(comp_time_seconds, fps);
        AEGP_LayerH camera_layerH = nullptr;

        A_Err err = suites.cameraSuite->AEGP_GetCamera(
            render_contextH, &comp_time, &camera_layerH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCamera failed");

        return reinterpret_cast<uintptr_t>(camera_layerH);
    }, py::arg("render_contextH"), py::arg("comp_time"),
       "Get the camera layer for a render context at a specific time");

    sdk.def("AEGP_GetCameraType", [](uintptr_t camera_layerH_ptr) -> int {
        // Argument validation
        if (camera_layerH_ptr == 0) throw std::invalid_argument("camera_layerH cannot be null");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.cameraSuite) throw std::runtime_error("Camera Suite not available");
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        // Check if this is actually a camera layer to avoid AE error dialogs
        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(camera_layerH_ptr);
        AEGP_ObjectType objType;
        A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
        if (err != A_Err_NONE || objType != AEGP_ObjectType_CAMERA) {
            // Return AEGP_CameraType_NONE for non-camera layers
            return static_cast<int>(AEGP_CameraType_NONE);
        }

        AEGP_CameraType camera_type;
        err = suites.cameraSuite->AEGP_GetCameraType(layerH, &camera_type);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCameraType failed");

        return static_cast<int>(camera_type);
    }, py::arg("camera_layerH"),
       "Get the camera type (PERSPECTIVE, ORTHOGRAPHIC). Returns AEGP_CameraType_NONE for non-camera layers.");

    sdk.def("AEGP_GetDefaultCameraDistanceToImagePlane", [](uintptr_t compH_ptr) -> double {
        // Argument validation
        if (compH_ptr == 0) throw std::invalid_argument("compH cannot be null");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.cameraSuite) throw std::runtime_error("Camera Suite not available");

        A_FpLong dist_to_plane = 0.0;
        A_Err err = suites.cameraSuite->AEGP_GetDefaultCameraDistanceToImagePlane(
            reinterpret_cast<AEGP_CompH>(compH_ptr), &dist_to_plane);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetDefaultCameraDistanceToImagePlane failed");

        return static_cast<double>(dist_to_plane);
    }, py::arg("compH"),
       "Get the default camera distance to image plane for a composition");

    sdk.def("AEGP_GetCameraFilmSize", [](uintptr_t camera_layerH_ptr) -> py::tuple {
        // Argument validation
        if (camera_layerH_ptr == 0) throw std::invalid_argument("camera_layerH cannot be null");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.cameraSuite) throw std::runtime_error("Camera Suite not available");
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        // Check if this is actually a camera layer to avoid AE error dialogs
        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(camera_layerH_ptr);
        AEGP_ObjectType objType;
        A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
        if (err != A_Err_NONE || objType != AEGP_ObjectType_CAMERA) {
            throw std::runtime_error("AEGP_GetCameraFilmSize requires a camera layer");
        }

        AEGP_FilmSizeUnits film_size_units;
        A_FpLong film_size = 0.0;

        err = suites.cameraSuite->AEGP_GetCameraFilmSize(layerH, &film_size_units, &film_size);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCameraFilmSize failed");

        return py::make_tuple(static_cast<int>(film_size_units), static_cast<double>(film_size));
    }, py::arg("camera_layerH"),
       "Get camera film size units and size in pixels. Returns (film_size_units, film_size). Requires a camera layer.");

    sdk.def("AEGP_SetCameraFilmSize", [](uintptr_t camera_layerH_ptr, int film_size_units, double film_size) {
        // Argument validation
        if (camera_layerH_ptr == 0) throw std::invalid_argument("camera_layerH cannot be null");
        Validation::RequirePositive(film_size, "film_size");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.cameraSuite) throw std::runtime_error("Camera Suite not available");
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        // Check if this is actually a camera layer to avoid AE error dialogs
        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(camera_layerH_ptr);
        AEGP_ObjectType objType;
        A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
        if (err != A_Err_NONE || objType != AEGP_ObjectType_CAMERA) {
            throw std::runtime_error("AEGP_SetCameraFilmSize requires a camera layer");
        }

        A_FpLong film_size_f = static_cast<A_FpLong>(film_size);

        err = suites.cameraSuite->AEGP_SetCameraFilmSize(
            layerH,
            static_cast<AEGP_FilmSizeUnits>(film_size_units),
            &film_size_f);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetCameraFilmSize failed");
    }, py::arg("camera_layerH"), py::arg("film_size_units"), py::arg("film_size"),
       "Set camera film size units and size in pixels. Requires a camera layer.");
}

void init_LightSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_LightSuite3 - Light Layer Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetLightType", [](uintptr_t light_layerH_ptr) -> int {
        // Argument validation
        if (light_layerH_ptr == 0) throw std::invalid_argument("light_layerH cannot be null");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.lightSuite) throw std::runtime_error("Light Suite not available");
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        // Check if this is actually a light layer to avoid AE error dialogs
        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(light_layerH_ptr);
        AEGP_ObjectType objType;
        A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
        if (err != A_Err_NONE || objType != AEGP_ObjectType_LIGHT) {
            // Return AEGP_LightType_NONE for non-light layers
            return static_cast<int>(AEGP_LightType_NONE);
        }

        AEGP_LightType light_type;
        err = suites.lightSuite->AEGP_GetLightType(layerH, &light_type);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLightType failed");

        return static_cast<int>(light_type);
    }, py::arg("light_layerH"),
       "Get the light type (PARALLEL, SPOT, POINT, AMBIENT, ENVIRONMENT). Returns AEGP_LightType_NONE for non-light layers.");

    sdk.def("AEGP_SetLightType", [](uintptr_t light_layerH_ptr, int light_type) {
        // Argument validation
        if (light_layerH_ptr == 0) throw std::invalid_argument("light_layerH cannot be null");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.lightSuite) throw std::runtime_error("Light Suite not available");
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        // Check if this is actually a light layer to avoid AE error dialogs
        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(light_layerH_ptr);
        AEGP_ObjectType objType;
        A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
        if (err != A_Err_NONE || objType != AEGP_ObjectType_LIGHT) {
            throw std::runtime_error("AEGP_SetLightType requires a light layer");
        }

        err = suites.lightSuite->AEGP_SetLightType(layerH, static_cast<AEGP_LightType>(light_type));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLightType failed");
    }, py::arg("light_layerH"), py::arg("light_type"),
       "Set the light type. Requires a light layer.");

#if defined(kAEGPLightSuiteVersion3)
    // AE 24.4+ only: Environment light source APIs
    sdk.def("AEGP_GetLightSource", [](uintptr_t light_layerH_ptr) -> uintptr_t {
        // Argument validation
        if (light_layerH_ptr == 0) throw std::invalid_argument("light_layerH cannot be null");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.lightSuite) throw std::runtime_error("Light Suite not available");
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        // Check if this is actually a light layer to avoid AE error dialogs
        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(light_layerH_ptr);
        AEGP_ObjectType objType;
        A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
        if (err != A_Err_NONE || objType != AEGP_ObjectType_LIGHT) {
            // Return 0 for non-light layers (no source)
            return 0;
        }

        AEGP_LayerH light_sourceH = nullptr;
        err = suites.lightSuite->AEGP_GetLightSource(layerH, &light_sourceH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLightSource failed");

        return reinterpret_cast<uintptr_t>(light_sourceH);
    }, py::arg("light_layerH"),
       "Get the light source layer (AE 24.4+). Returns 0 if no source or for non-light layers.");

    sdk.def("AEGP_SetLightSource", [](uintptr_t light_layerH_ptr, uintptr_t light_sourceH_ptr) {
        // Argument validation
        if (light_layerH_ptr == 0) throw std::invalid_argument("light_layerH cannot be null");
        // Note: light_sourceH_ptr can be 0 to clear the light source
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.lightSuite) throw std::runtime_error("Light Suite not available");
        if (!suites.layerSuite) throw std::runtime_error("Layer Suite not available");

        // Check if this is actually a light layer to avoid AE error dialogs
        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(light_layerH_ptr);
        AEGP_ObjectType objType;
        A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
        if (err != A_Err_NONE || objType != AEGP_ObjectType_LIGHT) {
            throw std::runtime_error("AEGP_SetLightSource requires a light layer");
        }

        err = suites.lightSuite->AEGP_SetLightSource(
            layerH,
            reinterpret_cast<AEGP_LayerH>(light_sourceH_ptr));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLightSource failed");
    }, py::arg("light_layerH"), py::arg("light_sourceH"),
       "Set the light source layer (AE 24.4+). Requires a light layer.");
#endif
}

} // namespace SDK
} // namespace PyAE
