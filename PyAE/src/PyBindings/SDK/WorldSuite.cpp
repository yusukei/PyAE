#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_Effect.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_WorldSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_WorldSuite3 - World (frame buffer) management
    // -----------------------------------------------------------------------

    // AEGP_WorldType enum values
    sdk.attr("AEGP_WorldType_NONE") = py::int_(static_cast<int>(AEGP_WorldType_NONE));
    sdk.attr("AEGP_WorldType_8") = py::int_(static_cast<int>(AEGP_WorldType_8));
    sdk.attr("AEGP_WorldType_16") = py::int_(static_cast<int>(AEGP_WorldType_16));
    sdk.attr("AEGP_WorldType_32") = py::int_(static_cast<int>(AEGP_WorldType_32));

    // PF_Quality enum values (for FastBlur)
    sdk.attr("PF_Quality_LO") = py::int_(static_cast<int>(PF_Quality_LO));
    sdk.attr("PF_Quality_HI") = py::int_(static_cast<int>(PF_Quality_HI));

    // PF_ModeFlags enum values (for FastBlur alpha handling)
    sdk.attr("PF_MF_Alpha_PREMUL") = py::int_(static_cast<int>(PF_MF_Alpha_PREMUL));
    sdk.attr("PF_MF_Alpha_STRAIGHT") = py::int_(static_cast<int>(PF_MF_Alpha_STRAIGHT));

    // -----------------------------------------------------------------------
    // AEGP_New - Create a new world (frame buffer)
    // -----------------------------------------------------------------------
    sdk.def("AEGP_NewWorld", [](int plugin_id, int world_type, int width, int height) -> uintptr_t {
        // Validate parameters
        if (world_type < AEGP_WorldType_NONE || world_type > AEGP_WorldType_32) {
            throw std::invalid_argument("AEGP_NewWorld: world_type must be AEGP_WorldType_NONE (0), AEGP_WorldType_8 (1), AEGP_WorldType_16 (2), or AEGP_WorldType_32 (3)");
        }
        Validation::RequirePositive(width, "AEGP_NewWorld: width");
        Validation::RequirePositive(height, "AEGP_NewWorld: height");

        // Practical size limits for worlds
        constexpr int MAX_WORLD_DIM = 30000;
        if (width > MAX_WORLD_DIM || height > MAX_WORLD_DIM) {
            throw std::invalid_argument("AEGP_NewWorld: dimensions exceed maximum (30000x30000)");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = nullptr;
        A_Err err = suites.worldSuite->AEGP_New(
            static_cast<AEGP_PluginID>(plugin_id),
            static_cast<AEGP_WorldType>(world_type),
            static_cast<A_long>(width),
            static_cast<A_long>(height),
            &worldH
        );
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_NewWorld failed with error code: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(worldH);
    }, py::arg("plugin_id"), py::arg("world_type"), py::arg("width"), py::arg("height"),
    "Create a new world (frame buffer) with specified type and dimensions");

    // -----------------------------------------------------------------------
    // AEGP_Dispose - Dispose a world
    // -----------------------------------------------------------------------
    sdk.def("AEGP_DisposeWorld", [](uintptr_t worldH_ptr) {
        // Validate parameters
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_DisposeWorld: worldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);
        A_Err err = suites.worldSuite->AEGP_Dispose(worldH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_DisposeWorld failed with error code: " + std::to_string(err));
        }
    }, py::arg("worldH"),
    "Dispose a world (free resources)");

    // -----------------------------------------------------------------------
    // AEGP_GetType - Get world type
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetWorldType", [](uintptr_t worldH_ptr) -> int {
        // Validate parameters
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetWorldType: worldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);
        AEGP_WorldType type = AEGP_WorldType_NONE;

        A_Err err = suites.worldSuite->AEGP_GetType(worldH, &type);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetWorldType failed with error code: " + std::to_string(err));
        }

        return static_cast<int>(type);
    }, py::arg("worldH"),
    "Get the pixel type of a world");

    // -----------------------------------------------------------------------
    // AEGP_GetSize - Get world dimensions
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetWorldSize", [](uintptr_t worldH_ptr) -> py::tuple {
        // Validate parameters
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetWorldSize: worldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);
        A_long width = 0, height = 0;

        A_Err err = suites.worldSuite->AEGP_GetSize(worldH, &width, &height);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetWorldSize failed with error code: " + std::to_string(err));
        }

        return py::make_tuple(static_cast<int>(width), static_cast<int>(height));
    }, py::arg("worldH"),
    "Get the dimensions of a world (width, height)");

    // -----------------------------------------------------------------------
    // AEGP_GetRowBytes - Get bytes per row
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetWorldRowBytes", [](uintptr_t worldH_ptr) -> int {
        // Validate parameters
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetWorldRowBytes: worldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);
        A_u_long row_bytes = 0;

        A_Err err = suites.worldSuite->AEGP_GetRowBytes(worldH, &row_bytes);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetWorldRowBytes failed with error code: " + std::to_string(err));
        }

        return static_cast<int>(row_bytes);
    }, py::arg("worldH"),
    "Get the bytes per row (stride) of a world");

    // -----------------------------------------------------------------------
    // AEGP_GetBaseAddr8 - Get base address for 8-bit world
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetWorldBaseAddr8", [](uintptr_t worldH_ptr) -> uintptr_t {
        // Validate parameters
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetWorldBaseAddr8: worldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);
        PF_Pixel8* base_addr = nullptr;

        A_Err err = suites.worldSuite->AEGP_GetBaseAddr8(worldH, &base_addr);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetWorldBaseAddr8 failed (world must be AEGP_WorldType_8)");
        }

        return reinterpret_cast<uintptr_t>(base_addr);
    }, py::arg("worldH"),
    "Get the base address of an 8-bit world's pixel data");

    // -----------------------------------------------------------------------
    // AEGP_GetBaseAddr16 - Get base address for 16-bit world
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetWorldBaseAddr16", [](uintptr_t worldH_ptr) -> uintptr_t {
        // Validate parameters
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetWorldBaseAddr16: worldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);
        PF_Pixel16* base_addr = nullptr;

        A_Err err = suites.worldSuite->AEGP_GetBaseAddr16(worldH, &base_addr);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetWorldBaseAddr16 failed (world must be AEGP_WorldType_16)");
        }

        return reinterpret_cast<uintptr_t>(base_addr);
    }, py::arg("worldH"),
    "Get the base address of a 16-bit world's pixel data");

    // -----------------------------------------------------------------------
    // AEGP_GetBaseAddr32 - Get base address for 32-bit float world
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetWorldBaseAddr32", [](uintptr_t worldH_ptr) -> uintptr_t {
        // Validate parameters
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetWorldBaseAddr32: worldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);
        PF_PixelFloat* base_addr = nullptr;

        A_Err err = suites.worldSuite->AEGP_GetBaseAddr32(worldH, &base_addr);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetWorldBaseAddr32 failed (world must be AEGP_WorldType_32)");
        }

        return reinterpret_cast<uintptr_t>(base_addr);
    }, py::arg("worldH"),
    "Get the base address of a 32-bit float world's pixel data");

    // -----------------------------------------------------------------------
    // AEGP_FillOutPFEffectWorld - Fill out PF_EffectWorld structure
    // Note: This returns the structure data as a dict for Python use
    // -----------------------------------------------------------------------
    sdk.def("AEGP_FillOutPFEffectWorld", [](uintptr_t worldH_ptr) -> py::dict {
        // Validate parameters
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_FillOutPFEffectWorld: worldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);
        PF_EffectWorld pf_world = {};

        A_Err err = suites.worldSuite->AEGP_FillOutPFEffectWorld(worldH, &pf_world);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_FillOutPFEffectWorld failed with error code: " + std::to_string(err));
        }

        // Return as dictionary for Python access
        py::dict result;
        result["width"] = pf_world.width;
        result["height"] = pf_world.height;
        result["rowbytes"] = pf_world.rowbytes;
        result["world_flags"] = pf_world.world_flags;
        result["data"] = reinterpret_cast<uintptr_t>(pf_world.data);

        return result;
    }, py::arg("worldH"),
    "Fill out a PF_EffectWorld structure from an AEGP_WorldH");

    // -----------------------------------------------------------------------
    // AEGP_FastBlur - Apply fast blur to a world
    // -----------------------------------------------------------------------
    sdk.def("AEGP_FastBlur", [](double radius, int mode_flags, int quality, uintptr_t worldH_ptr) {
        // Validate parameters
        Validation::RequireNonNegative(radius, "AEGP_FastBlur: radius");
        if (worldH_ptr == 0) {
            throw std::invalid_argument("AEGP_FastBlur: worldH cannot be null");
        }
        if (mode_flags != PF_MF_Alpha_PREMUL && mode_flags != PF_MF_Alpha_STRAIGHT) {
            throw std::invalid_argument("AEGP_FastBlur: mode_flags must be PF_MF_Alpha_PREMUL (0) or PF_MF_Alpha_STRAIGHT (1)");
        }
        if (quality != PF_Quality_LO && quality != PF_Quality_HI) {
            throw std::invalid_argument("AEGP_FastBlur: quality must be PF_Quality_LO (0) or PF_Quality_HI (1)");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_WorldH worldH = reinterpret_cast<AEGP_WorldH>(worldH_ptr);

        A_Err err = suites.worldSuite->AEGP_FastBlur(
            static_cast<A_FpLong>(radius),
            static_cast<PF_ModeFlags>(mode_flags),
            static_cast<PF_Quality>(quality),
            worldH
        );
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_FastBlur failed with error code: " + std::to_string(err));
        }
    }, py::arg("radius"), py::arg("mode_flags"), py::arg("quality"), py::arg("worldH"),
    "Apply fast blur to a world (only for user-allocated worlds, not checked-out frames)");

    // -----------------------------------------------------------------------
    // AEGP_NewPlatformWorld - Create a platform-specific world
    // -----------------------------------------------------------------------
    sdk.def("AEGP_NewPlatformWorld", [](int plugin_id, int world_type, int width, int height) -> uintptr_t {
        // Validate parameters
        if (world_type < AEGP_WorldType_NONE || world_type > AEGP_WorldType_32) {
            throw std::invalid_argument("AEGP_NewPlatformWorld: world_type must be AEGP_WorldType_NONE (0), AEGP_WorldType_8 (1), AEGP_WorldType_16 (2), or AEGP_WorldType_32 (3)");
        }
        Validation::RequirePositive(width, "AEGP_NewPlatformWorld: width");
        Validation::RequirePositive(height, "AEGP_NewPlatformWorld: height");

        constexpr int MAX_WORLD_DIM = 30000;
        if (width > MAX_WORLD_DIM || height > MAX_WORLD_DIM) {
            throw std::invalid_argument("AEGP_NewPlatformWorld: dimensions exceed maximum (30000x30000)");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_PlatformWorldH platformWorldH = nullptr;
        A_Err err = suites.worldSuite->AEGP_NewPlatformWorld(
            static_cast<AEGP_PluginID>(plugin_id),
            static_cast<AEGP_WorldType>(world_type),
            static_cast<A_long>(width),
            static_cast<A_long>(height),
            &platformWorldH
        );
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_NewPlatformWorld failed with error code: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(platformWorldH);
    }, py::arg("plugin_id"), py::arg("world_type"), py::arg("width"), py::arg("height"),
    "Create a platform-specific world");

    // -----------------------------------------------------------------------
    // AEGP_DisposePlatformWorld - Dispose a platform-specific world
    // -----------------------------------------------------------------------
    sdk.def("AEGP_DisposePlatformWorld", [](uintptr_t platformWorldH_ptr) {
        // Validate parameters
        if (platformWorldH_ptr == 0) {
            throw std::invalid_argument("AEGP_DisposePlatformWorld: platformWorldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_PlatformWorldH platformWorldH = reinterpret_cast<AEGP_PlatformWorldH>(platformWorldH_ptr);
        A_Err err = suites.worldSuite->AEGP_DisposePlatformWorld(platformWorldH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_DisposePlatformWorld failed with error code: " + std::to_string(err));
        }
    }, py::arg("platformWorldH"),
    "Dispose a platform-specific world");

    // -----------------------------------------------------------------------
    // AEGP_NewReferenceFromPlatformWorld - Create AEGP_WorldH from platform world
    // -----------------------------------------------------------------------
    sdk.def("AEGP_NewReferenceFromPlatformWorld", [](int plugin_id, uintptr_t platformWorldH_ptr) -> uintptr_t {
        // Validate parameters
        if (platformWorldH_ptr == 0) {
            throw std::invalid_argument("AEGP_NewReferenceFromPlatformWorld: platformWorldH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.worldSuite) throw std::runtime_error("World Suite not available");

        AEGP_PlatformWorldH platformWorldH = reinterpret_cast<AEGP_PlatformWorldH>(platformWorldH_ptr);
        AEGP_WorldH worldH = nullptr;

        A_Err err = suites.worldSuite->AEGP_NewReferenceFromPlatformWorld(
            static_cast<AEGP_PluginID>(plugin_id),
            platformWorldH,
            &worldH
        );
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_NewReferenceFromPlatformWorld failed with error code: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(worldH);
    }, py::arg("plugin_id"), py::arg("platformWorldH"),
    "Create an AEGP_WorldH reference from a platform-specific world");
}

} // namespace SDK
} // namespace PyAE
