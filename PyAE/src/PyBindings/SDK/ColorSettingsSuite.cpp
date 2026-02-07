// ColorSettingsSuite.cpp
// PyAE - Python for After Effects
// AEGP_ColorSettingsSuite6 Python Bindings
//
// Provides Python bindings for After Effects Color Settings Suite.
// This suite manages color profiles, OCIO color management, and color space transformations.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_ColorSettingsSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_ColorSettingsSuite6 - Color Settings Functions
    // -----------------------------------------------------------------------

    // 1. AEGP_GetBlendingTables
    // Note: This function requires PR_RenderContextH and returns PF_EffectBlendingTables
    // which are complex structures used during rendering. Skipped for now as it's
    // primarily used internally during effect rendering.

    // 2. AEGP_DoesViewHaveColorSpaceXform
    sdk.def("AEGP_DoesViewHaveColorSpaceXform", [](uintptr_t viewP_ptr) -> bool {
        if (viewP_ptr == 0) throw std::runtime_error("Invalid view pointer (null)");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_ItemViewP viewP = reinterpret_cast<AEGP_ItemViewP>(viewP_ptr);
        A_Boolean has_xform = FALSE;

        A_Err err = suites.colorSettingsSuite->AEGP_DoesViewHaveColorSpaceXform(viewP, &has_xform);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DoesViewHaveColorSpaceXform failed");

        return has_xform != FALSE;
    }, py::arg("viewP"),
       "Check if the view has a color space transformation applied");

    // 3. AEGP_XformWorkingToViewColorSpace
    sdk.def("AEGP_XformWorkingToViewColorSpace", [](uintptr_t viewP_ptr, uintptr_t srcH_ptr, uintptr_t dstH_ptr) {
        if (viewP_ptr == 0) throw std::runtime_error("Invalid view pointer (null)");
        if (srcH_ptr == 0) throw std::runtime_error("Invalid source world handle (null)");
        if (dstH_ptr == 0) throw std::runtime_error("Invalid destination world handle (null)");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_ItemViewP viewP = reinterpret_cast<AEGP_ItemViewP>(viewP_ptr);
        AEGP_WorldH srcH = reinterpret_cast<AEGP_WorldH>(srcH_ptr);
        AEGP_WorldH dstH = reinterpret_cast<AEGP_WorldH>(dstH_ptr);

        A_Err err = suites.colorSettingsSuite->AEGP_XformWorkingToViewColorSpace(viewP, srcH, dstH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_XformWorkingToViewColorSpace failed");
    }, py::arg("viewP"), py::arg("srcH"), py::arg("dstH"),
       "Transform pixel data from working color space to view color space. Source and destination can be the same.");

    // 4. AEGP_GetNewWorkingSpaceColorProfile
    sdk.def("AEGP_GetNewWorkingSpaceColorProfile", [](uintptr_t compH_ptr) -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_CompH compH = reinterpret_cast<AEGP_CompH>(compH_ptr);
        AEGP_ColorProfileP colorProfileP = nullptr;

        A_Err err = suites.colorSettingsSuite->AEGP_GetNewWorkingSpaceColorProfile(
            plugin_id, compH, &colorProfileP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewWorkingSpaceColorProfile failed");

        return reinterpret_cast<uintptr_t>(colorProfileP);
    }, py::arg("compH"),
       "Get the working space color profile for a composition. Caller must dispose with AEGP_DisposeColorProfile.");

    // 5. AEGP_GetNewColorProfileFromICCProfile
    sdk.def("AEGP_GetNewColorProfileFromICCProfile", [](py::bytes icc_data) -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();

        std::string icc_str = icc_data;
        A_long icc_size = static_cast<A_long>(icc_str.size());
        const void* icc_dataPV = icc_str.data();

        AEGP_ColorProfileP colorProfileP = nullptr;

        A_Err err = suites.colorSettingsSuite->AEGP_GetNewColorProfileFromICCProfile(
            plugin_id, icc_size, icc_dataPV, &colorProfileP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewColorProfileFromICCProfile failed");

        return reinterpret_cast<uintptr_t>(colorProfileP);
    }, py::arg("icc_data"),
       "Create a color profile from ICC profile data. Caller must dispose with AEGP_DisposeColorProfile.");

    // 6. AEGP_GetNewICCProfileFromColorProfile
    sdk.def("AEGP_GetNewICCProfileFromColorProfile", [](uintptr_t colorProfileP_ptr) -> py::bytes {
        if (colorProfileP_ptr == 0) throw std::runtime_error("Invalid color profile pointer (null)");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_ConstColorProfileP colorProfileP = reinterpret_cast<AEGP_ConstColorProfileP>(colorProfileP_ptr);
        AEGP_MemHandle icc_profileH = nullptr;

        A_Err err = suites.colorSettingsSuite->AEGP_GetNewICCProfileFromColorProfile(
            plugin_id, colorProfileP, &icc_profileH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewICCProfileFromColorProfile failed");

        if (!icc_profileH) {
            return py::bytes("");
        }

        // Use RAII helpers for automatic memory management
        PyAE::ScopedMemHandle scopedHandle(plugin_id, suites.memorySuite, icc_profileH);

        // Get the size of the ICC profile
        AEGP_MemSize size = 0;
        err = suites.memorySuite->AEGP_GetMemHandleSize(icc_profileH, &size);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetMemHandleSize failed");
        }

        // Lock and copy the data
        PyAE::ScopedMemLock lock(suites.memorySuite, icc_profileH);
        if (!lock.IsValid()) {
            throw std::runtime_error("AEGP_LockMemHandle failed");
        }

        py::bytes result(static_cast<const char*>(lock.Get()), size);
        return result;  // RAII destructors will unlock and free memory
    }, py::arg("colorProfileP"),
       "Extract ICC profile data from a color profile. Returns bytes.");

    // 7. AEGP_GetNewColorProfileDescription
    sdk.def("AEGP_GetNewColorProfileDescription", [](uintptr_t colorProfileP_ptr) -> std::string {
        if (colorProfileP_ptr == 0) throw std::runtime_error("Invalid color profile pointer (null)");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_ConstColorProfileP colorProfileP = reinterpret_cast<AEGP_ConstColorProfileP>(colorProfileP_ptr);
        AEGP_MemHandle unicode_descH = nullptr;

        A_Err err = suites.colorSettingsSuite->AEGP_GetNewColorProfileDescription(
            plugin_id, colorProfileP, &unicode_descH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewColorProfileDescription failed");

        if (!unicode_descH) {
            return "";
        }

        // Use scoped handles for RAII cleanup
        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, unicode_descH);
        PyAE::ScopedMemLock lock(suites.memorySuite, unicode_descH);

        if (!lock.IsValid()) {
            throw std::runtime_error("Failed to lock memory handle");
        }

        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, py::arg("colorProfileP"),
       "Get a human-readable description of a color profile.");

    // 8. AEGP_DisposeColorProfile
    sdk.def("AEGP_DisposeColorProfile", [](uintptr_t colorProfileP_ptr) {
        if (colorProfileP_ptr == 0) return;  // Allow null for convenience

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_ColorProfileP colorProfileP = reinterpret_cast<AEGP_ColorProfileP>(colorProfileP_ptr);

        A_Err err = suites.colorSettingsSuite->AEGP_DisposeColorProfile(colorProfileP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisposeColorProfile failed");
    }, py::arg("colorProfileP"),
       "Dispose a color profile obtained from AEGP_GetNewWorkingSpaceColorProfile or similar.");

    // 9. AEGP_GetColorProfileApproximateGamma
    sdk.def("AEGP_GetColorProfileApproximateGamma", [](uintptr_t colorProfileP_ptr) -> float {
        if (colorProfileP_ptr == 0) throw std::runtime_error("Invalid color profile pointer (null)");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_ConstColorProfileP colorProfileP = reinterpret_cast<AEGP_ConstColorProfileP>(colorProfileP_ptr);
        A_FpShort approx_gamma = 0.0f;

        A_Err err = suites.colorSettingsSuite->AEGP_GetColorProfileApproximateGamma(
            colorProfileP, &approx_gamma);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetColorProfileApproximateGamma failed");

        return static_cast<float>(approx_gamma);
    }, py::arg("colorProfileP"),
       "Get the approximate gamma of a color profile.");

    // 10. AEGP_IsRGBColorProfile
    sdk.def("AEGP_IsRGBColorProfile", [](uintptr_t colorProfileP_ptr) -> bool {
        if (colorProfileP_ptr == 0) throw std::runtime_error("Invalid color profile pointer (null)");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_ConstColorProfileP colorProfileP = reinterpret_cast<AEGP_ConstColorProfileP>(colorProfileP_ptr);
        A_Boolean is_rgb = FALSE;

        A_Err err = suites.colorSettingsSuite->AEGP_IsRGBColorProfile(colorProfileP, &is_rgb);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsRGBColorProfile failed");

        return is_rgb != FALSE;
    }, py::arg("colorProfileP"),
       "Check if a color profile is an RGB color profile.");

    // 11. AEGP_SetWorkingColorSpace
    sdk.def("AEGP_SetWorkingColorSpace", [](uintptr_t compH_ptr, uintptr_t colorProfileP_ptr) {
        if (compH_ptr == 0) throw std::runtime_error("Invalid comp handle (null)");
        if (colorProfileP_ptr == 0) throw std::runtime_error("Invalid color profile pointer (null)");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_CompH compH = reinterpret_cast<AEGP_CompH>(compH_ptr);
        AEGP_ConstColorProfileP colorProfileP = reinterpret_cast<AEGP_ConstColorProfileP>(colorProfileP_ptr);

        A_Err err = suites.colorSettingsSuite->AEGP_SetWorkingColorSpace(
            plugin_id, compH, colorProfileP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetWorkingColorSpace failed");
    }, py::arg("compH"), py::arg("colorProfileP"),
       "Set the working color space for a composition.");

#if defined(kAEGPColorSettingsSuiteVersion6)
    // Functions 12-20 are only available in ColorSettingsSuite6 (AE 24.0+)

    // 12. AEGP_IsOCIOColorManagementUsed
    sdk.def("AEGP_IsOCIOColorManagementUsed", []() -> bool {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        A_Boolean is_ocio = FALSE;

        A_Err err = suites.colorSettingsSuite->AEGP_IsOCIOColorManagementUsed(plugin_id, &is_ocio);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsOCIOColorManagementUsed failed");

        return is_ocio != FALSE;
    }, "Check if OCIO color management is being used.");

    // 13. AEGP_GetOCIOConfigurationFile
    sdk.def("AEGP_GetOCIOConfigurationFile", []() -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_MemHandle config_fileH = nullptr;

        A_Err err = suites.colorSettingsSuite->AEGP_GetOCIOConfigurationFile(plugin_id, &config_fileH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOCIOConfigurationFile failed");

        if (!config_fileH) {
            return "";
        }

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, config_fileH);
        PyAE::ScopedMemLock lock(suites.memorySuite, config_fileH);

        if (!lock.IsValid()) {
            throw std::runtime_error("Failed to lock memory handle");
        }

        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, "Get the OCIO configuration file name.");

    // 14. AEGP_GetOCIOConfigurationFilePath
    sdk.def("AEGP_GetOCIOConfigurationFilePath", []() -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_MemHandle config_fileH = nullptr;

        A_Err err = suites.colorSettingsSuite->AEGP_GetOCIOConfigurationFilePath(plugin_id, &config_fileH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOCIOConfigurationFilePath failed");

        if (!config_fileH) {
            return "";
        }

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, config_fileH);
        PyAE::ScopedMemLock lock(suites.memorySuite, config_fileH);

        if (!lock.IsValid()) {
            throw std::runtime_error("Failed to lock memory handle");
        }

        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, "Get the full path to the OCIO configuration file.");

    // 15. AEGPD_GetOCIOWorkingColorSpace
    sdk.def("AEGPD_GetOCIOWorkingColorSpace", []() -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_MemHandle working_csH = nullptr;

        A_Err err = suites.colorSettingsSuite->AEGPD_GetOCIOWorkingColorSpace(plugin_id, &working_csH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGPD_GetOCIOWorkingColorSpace failed");

        if (!working_csH) {
            return "";
        }

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, working_csH);
        PyAE::ScopedMemLock lock(suites.memorySuite, working_csH);

        if (!lock.IsValid()) {
            throw std::runtime_error("Failed to lock memory handle");
        }

        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, "Get the OCIO working color space name.");

    // 16. AEGPD_GetOCIODisplayColorSpace
    sdk.def("AEGPD_GetOCIODisplayColorSpace", []() -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_MemHandle displayH = nullptr;
        AEGP_MemHandle viewH = nullptr;

        A_Err err = suites.colorSettingsSuite->AEGPD_GetOCIODisplayColorSpace(plugin_id, &displayH, &viewH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGPD_GetOCIODisplayColorSpace failed with error: " + std::to_string(err));

        std::string display_str, view_str;

        if (displayH) {
            PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, displayH);
            PyAE::ScopedMemLock lock(suites.memorySuite, displayH);
            if (lock.IsValid()) {
                display_str = PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
            }
        }

        if (viewH) {
            PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, viewH);
            PyAE::ScopedMemLock lock(suites.memorySuite, viewH);
            if (lock.IsValid()) {
                view_str = PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
            }
        }

        return py::make_tuple(display_str, view_str);
    }, "Get the OCIO display and view color space names. Returns (display, view).");

    // 17. AEGPD_IsColorSpaceAwareEffectsEnabled
    sdk.def("AEGPD_IsColorSpaceAwareEffectsEnabled", []() -> bool {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        A_Boolean is_enabled = FALSE;

        A_Err err = suites.colorSettingsSuite->AEGPD_IsColorSpaceAwareEffectsEnabled(plugin_id, &is_enabled);
        if (err != A_Err_NONE) throw std::runtime_error("AEGPD_IsColorSpaceAwareEffectsEnabled failed");

        return is_enabled != FALSE;
    }, "Check if color space aware effects are enabled.");

    // 18. AEGPD_GetLUTInterpolationMethod
    sdk.def("AEGPD_GetLUTInterpolationMethod", []() -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        A_u_short method = 0;

        A_Err err = suites.colorSettingsSuite->AEGPD_GetLUTInterpolationMethod(plugin_id, &method);
        if (err != A_Err_NONE) throw std::runtime_error("AEGPD_GetLUTInterpolationMethod failed");

        return static_cast<int>(method);
    }, "Get the LUT interpolation method.");

    // 19. AEGPD_GetGraphicsWhiteLuminance
    sdk.def("AEGPD_GetGraphicsWhiteLuminance", []() -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        A_u_short luminance = 0;

        A_Err err = suites.colorSettingsSuite->AEGPD_GetGraphicsWhiteLuminance(plugin_id, &luminance);
        if (err != A_Err_NONE) throw std::runtime_error("AEGPD_GetGraphicsWhiteLuminance failed");

        return static_cast<int>(luminance);
    }, "Get the graphics white luminance value.");
    // 20. AEGPD_GetWorkingColorSpaceId (ColorSettingsSuite6 only)
    // Returns the working color space GUID as 16 bytes (standard GUID size)
    sdk.def("AEGPD_GetWorkingColorSpaceId", []() -> py::bytes {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.colorSettingsSuite) throw std::runtime_error("ColorSettings Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();

        // Allocate space for the GUID (16 bytes is standard GUID size)
        A_u_char guid_buffer[16] = {0};
        AEGP_GuidP guidP = reinterpret_cast<AEGP_GuidP>(guid_buffer);

        A_Err err = suites.colorSettingsSuite->AEGPD_GetWorkingColorSpaceId(plugin_id, guidP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGPD_GetWorkingColorSpaceId failed");

        return py::bytes(reinterpret_cast<const char*>(guid_buffer), 16);
    }, "Get the working color space GUID as 16 bytes.");
#endif
}

} // namespace SDK
} // namespace PyAE
