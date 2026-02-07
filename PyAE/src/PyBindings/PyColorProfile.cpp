// PyColorProfile.cpp
// PyAE - Python for After Effects
// カラープロファイル関連クラスの実装
//
// ColorSettingsSuite6の高レベルAPI

#include "PyColorProfile.h"
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "SDKVersionCompat.h"

namespace PyAE {

// =============================================================
// PyColorProfile Implementation
// =============================================================

PyColorProfile::PyColorProfile()
    : m_profileP(nullptr)
{
}

PyColorProfile::PyColorProfile(AEGP_ColorProfileP profileP)
    : m_profileP(profileP)
{
}

PyColorProfile::PyColorProfile(PyColorProfile&& other) noexcept
    : m_profileP(other.m_profileP)
{
    other.m_profileP = nullptr;
}

PyColorProfile& PyColorProfile::operator=(PyColorProfile&& other) noexcept
{
    if (this != &other) {
        Dispose();
        m_profileP = other.m_profileP;
        other.m_profileP = nullptr;
    }
    return *this;
}

PyColorProfile::~PyColorProfile()
{
    Dispose();
}

void PyColorProfile::Dispose()
{
    if (m_profileP) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.colorSettingsSuite) {
            suites.colorSettingsSuite->AEGP_DisposeColorProfile(m_profileP);
        }
        m_profileP = nullptr;
    }
}

bool PyColorProfile::IsValid() const
{
    return m_profileP != nullptr;
}

std::string PyColorProfile::GetDescription() const
{
    if (!m_profileP) {
        throw std::runtime_error("Invalid color profile");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }
    if (!suites.memorySuite) {
        throw std::runtime_error("Memory Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_MemHandle unicode_descH = nullptr;

    A_Err err = suites.colorSettingsSuite->AEGP_GetNewColorProfileDescription(
        plugin_id, m_profileP, &unicode_descH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetNewColorProfileDescription failed");
    }

    if (!unicode_descH) {
        return "";
    }

    // Use scoped handles for RAII cleanup
    ScopedMemHandle scoped(plugin_id, suites.memorySuite, unicode_descH);
    ScopedMemLock lock(suites.memorySuite, unicode_descH);

    if (!lock.IsValid()) {
        throw std::runtime_error("Failed to lock memory handle");
    }

    return StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
}

float PyColorProfile::GetGamma() const
{
    if (!m_profileP) {
        throw std::runtime_error("Invalid color profile");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    A_FpShort approx_gamma = 0.0f;
    A_Err err = suites.colorSettingsSuite->AEGP_GetColorProfileApproximateGamma(
        m_profileP, &approx_gamma);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetColorProfileApproximateGamma failed");
    }

    return static_cast<float>(approx_gamma);
}

bool PyColorProfile::IsRGB() const
{
    if (!m_profileP) {
        throw std::runtime_error("Invalid color profile");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    A_Boolean is_rgb = FALSE;
    A_Err err = suites.colorSettingsSuite->AEGP_IsRGBColorProfile(m_profileP, &is_rgb);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_IsRGBColorProfile failed");
    }

    return is_rgb != FALSE;
}

py::bytes PyColorProfile::ToICCData() const
{
    if (!m_profileP) {
        throw std::runtime_error("Invalid color profile");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }
    if (!suites.memorySuite) {
        throw std::runtime_error("Memory Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_MemHandle icc_profileH = nullptr;

    A_Err err = suites.colorSettingsSuite->AEGP_GetNewICCProfileFromColorProfile(
        plugin_id, m_profileP, &icc_profileH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetNewICCProfileFromColorProfile failed");
    }

    if (!icc_profileH) {
        return py::bytes("");
    }

    ScopedMemHandle scoped(plugin_id, suites.memorySuite, icc_profileH);
    ScopedMemLock lock(suites.memorySuite, icc_profileH);

    if (!lock.IsValid()) {
        throw std::runtime_error("Failed to lock memory handle");
    }

    // Get size
    AEGP_MemSize size = 0;
    err = suites.memorySuite->AEGP_GetMemHandleSize(icc_profileH, &size);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetMemHandleSize failed");
    }

    return py::bytes(static_cast<const char*>(lock.Get()), size);
}

AEGP_ColorProfileP PyColorProfile::GetHandle() const
{
    return m_profileP;
}

AEGP_ColorProfileP PyColorProfile::Release()
{
    AEGP_ColorProfileP handle = m_profileP;
    m_profileP = nullptr;
    return handle;
}

// Static factory methods

std::shared_ptr<PyColorProfile> PyColorProfile::FromICCData(const py::bytes& iccData)
{
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();

    std::string icc_str = iccData;
    A_long icc_size = static_cast<A_long>(icc_str.size());
    const void* icc_dataPV = icc_str.data();

    AEGP_ColorProfileP colorProfileP = nullptr;

    A_Err err = suites.colorSettingsSuite->AEGP_GetNewColorProfileFromICCProfile(
        plugin_id, icc_size, icc_dataPV, &colorProfileP);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetNewColorProfileFromICCProfile failed");
    }

    return std::make_shared<PyColorProfile>(colorProfileP);
}

std::shared_ptr<PyColorProfile> PyColorProfile::FromComp(uintptr_t compH_ptr)
{
    if (compH_ptr == 0) {
        throw std::runtime_error("Invalid comp handle (null)");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_CompH compH = reinterpret_cast<AEGP_CompH>(compH_ptr);
    AEGP_ColorProfileP colorProfileP = nullptr;

    A_Err err = suites.colorSettingsSuite->AEGP_GetNewWorkingSpaceColorProfile(
        plugin_id, compH, &colorProfileP);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetNewWorkingSpaceColorProfile failed");
    }

    return std::make_shared<PyColorProfile>(colorProfileP);
}

// =============================================================
// ColorSettings Namespace Implementation
// =============================================================

namespace ColorSettings {

bool IsOCIOEnabled()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    A_Boolean is_ocio = FALSE;

    A_Err err = suites.colorSettingsSuite->AEGP_IsOCIOColorManagementUsed(plugin_id, &is_ocio);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_IsOCIOColorManagementUsed failed");
    }

    return is_ocio != FALSE;
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

std::string GetOCIOConfigFile()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }
    if (!suites.memorySuite) {
        throw std::runtime_error("Memory Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_MemHandle config_fileH = nullptr;

    A_Err err = suites.colorSettingsSuite->AEGP_GetOCIOConfigurationFile(plugin_id, &config_fileH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetOCIOConfigurationFile failed");
    }

    if (!config_fileH) {
        return "";
    }

    ScopedMemHandle scoped(plugin_id, suites.memorySuite, config_fileH);
    ScopedMemLock lock(suites.memorySuite, config_fileH);

    if (!lock.IsValid()) {
        throw std::runtime_error("Failed to lock memory handle");
    }

    return StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

std::string GetOCIOConfigFilePath()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }
    if (!suites.memorySuite) {
        throw std::runtime_error("Memory Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_MemHandle config_fileH = nullptr;

    A_Err err = suites.colorSettingsSuite->AEGP_GetOCIOConfigurationFilePath(plugin_id, &config_fileH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetOCIOConfigurationFilePath failed");
    }

    if (!config_fileH) {
        return "";
    }

    ScopedMemHandle scoped(plugin_id, suites.memorySuite, config_fileH);
    ScopedMemLock lock(suites.memorySuite, config_fileH);

    if (!lock.IsValid()) {
        throw std::runtime_error("Failed to lock memory handle");
    }

    return StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

std::string GetOCIOWorkingColorSpace()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }
    if (!suites.memorySuite) {
        throw std::runtime_error("Memory Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_MemHandle working_csH = nullptr;

    A_Err err = suites.colorSettingsSuite->AEGPD_GetOCIOWorkingColorSpace(plugin_id, &working_csH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGPD_GetOCIOWorkingColorSpace failed");
    }

    if (!working_csH) {
        return "";
    }

    ScopedMemHandle scoped(plugin_id, suites.memorySuite, working_csH);
    ScopedMemLock lock(suites.memorySuite, working_csH);

    if (!lock.IsValid()) {
        throw std::runtime_error("Failed to lock memory handle");
    }

    return StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

py::tuple GetOCIODisplayView()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }
    if (!suites.memorySuite) {
        throw std::runtime_error("Memory Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_MemHandle displayH = nullptr;
    AEGP_MemHandle viewH = nullptr;

    A_Err err = suites.colorSettingsSuite->AEGPD_GetOCIODisplayColorSpace(plugin_id, &displayH, &viewH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGPD_GetOCIODisplayColorSpace failed");
    }

    std::string display_str, view_str;

    if (displayH) {
        ScopedMemHandle scoped(plugin_id, suites.memorySuite, displayH);
        ScopedMemLock lock(suites.memorySuite, displayH);
        if (lock.IsValid()) {
            display_str = StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
        }
    }

    if (viewH) {
        ScopedMemHandle scoped(plugin_id, suites.memorySuite, viewH);
        ScopedMemLock lock(suites.memorySuite, viewH);
        if (lock.IsValid()) {
            view_str = StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
        }
    }

    return py::make_tuple(display_str, view_str);
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

bool IsColorSpaceAwareEffectsEnabled()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    A_Boolean is_enabled = FALSE;

    A_Err err = suites.colorSettingsSuite->AEGPD_IsColorSpaceAwareEffectsEnabled(plugin_id, &is_enabled);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGPD_IsColorSpaceAwareEffectsEnabled failed");
    }

    return is_enabled != FALSE;
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

int GetLUTInterpolationMethod()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    A_u_short method = 0;

    A_Err err = suites.colorSettingsSuite->AEGPD_GetLUTInterpolationMethod(plugin_id, &method);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGPD_GetLUTInterpolationMethod failed");
    }

    return static_cast<int>(method);
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

int GetGraphicsWhiteLuminance()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    A_u_short luminance = 0;

    A_Err err = suites.colorSettingsSuite->AEGPD_GetGraphicsWhiteLuminance(plugin_id, &luminance);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGPD_GetGraphicsWhiteLuminance failed");
    }

    return static_cast<int>(luminance);
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

py::bytes GetWorkingColorSpaceId()
{
#if defined(kAEGPColorSettingsSuiteVersion6)
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();

    // GUID is 16 bytes
    A_u_char guid_buffer[16] = {0};
    AEGP_GuidP guidP = reinterpret_cast<AEGP_GuidP>(guid_buffer);

    A_Err err = suites.colorSettingsSuite->AEGPD_GetWorkingColorSpaceId(plugin_id, guidP);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGPD_GetWorkingColorSpaceId failed");
    }

    return py::bytes(reinterpret_cast<const char*>(guid_buffer), 16);
#else
    throw std::runtime_error("OCIO features require After Effects 24.0 or later");
#endif
}

} // namespace ColorSettings

// =============================================================
// Python Bindings
// =============================================================

void init_color_profile(py::module_& m) {
    // ColorProfile class
    py::class_<PyColorProfile, std::shared_ptr<PyColorProfile>>(m, "ColorProfile",
        "Color profile for working space color management.\n\n"
        "ColorProfile wraps AEGP_ColorProfileP and provides access to\n"
        "ICC profile data, gamma information, and profile description.")

        .def_property_readonly("valid", &PyColorProfile::IsValid,
            "Check if color profile is valid")

        .def_property_readonly("description", &PyColorProfile::GetDescription,
            "Human-readable description of the color profile")

        .def_property_readonly("gamma", &PyColorProfile::GetGamma,
            "Approximate gamma value of the color profile")

        .def_property_readonly("is_rgb", &PyColorProfile::IsRGB,
            "Check if this is an RGB color profile")

        .def("to_icc_data", &PyColorProfile::ToICCData,
            "Export color profile as ICC profile data (bytes)")

        .def_static("from_icc_data", &PyColorProfile::FromICCData,
            "Create ColorProfile from ICC profile data",
            py::arg("icc_data"))

        .def_static("from_comp", &PyColorProfile::FromComp,
            "Get working space ColorProfile from a composition",
            py::arg("comp_handle"))

        .def("__repr__", [](const PyColorProfile& self) {
            if (!self.IsValid()) {
                return std::string("<ColorProfile: invalid>");
            }
            try {
                return std::string("<ColorProfile: ") + self.GetDescription() + ">";
            } catch (...) {
                return std::string("<ColorProfile: valid>");
            }
        });

    // color_settings submodule
    py::module_ color_settings = m.def_submodule("color_settings",
        "Color settings and OCIO configuration access.\n\n"
        "Provides access to project-level color settings and OCIO configuration.\n"
        "OCIO functions require After Effects 24.0 or later.");

    color_settings.def("is_ocio_enabled", &ColorSettings::IsOCIOEnabled,
        "Check if OCIO color management is used");

    color_settings.def("get_ocio_config_file", &ColorSettings::GetOCIOConfigFile,
        "Get OCIO configuration file name");

    color_settings.def("get_ocio_config_file_path", &ColorSettings::GetOCIOConfigFilePath,
        "Get OCIO configuration file full path");

    color_settings.def("get_ocio_working_color_space", &ColorSettings::GetOCIOWorkingColorSpace,
        "Get OCIO working color space name");

    color_settings.def("get_ocio_display_view", &ColorSettings::GetOCIODisplayView,
        "Get OCIO display and view names as (display, view) tuple");

    color_settings.def("is_color_space_aware_effects_enabled", &ColorSettings::IsColorSpaceAwareEffectsEnabled,
        "Check if color space aware effects are enabled");

    color_settings.def("get_lut_interpolation_method", &ColorSettings::GetLUTInterpolationMethod,
        "Get LUT interpolation method");

    color_settings.def("get_graphics_white_luminance", &ColorSettings::GetGraphicsWhiteLuminance,
        "Get graphics white luminance value");

    color_settings.def("get_working_color_space_id", &ColorSettings::GetWorkingColorSpaceId,
        "Get working color space GUID (16 bytes)");
}

} // namespace PyAE
