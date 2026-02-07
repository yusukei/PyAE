// =============================================================
// SDKVersionCompat.h
// PyAE - Python for After Effects
// SDK Version Compatibility Layer
//
// Provides type aliases and feature flags based on SDK version.
// Uses SDK header macros to detect available Suite versions.
// =============================================================

#pragma once

#include "AE_GeneralPlug.h"

namespace PyAE::SDKCompat {

// =============================================================
// CompSuite
// SDK 2022: V11 (kAEGPCompSuiteVersion11)
// SDK 25.2+: V12 (kAEGPCompSuiteVersion12) - adds horzB param to CreateTextLayerInComp
// =============================================================
#if defined(kAEGPCompSuiteVersion12)
    using CompSuiteType = AEGP_CompSuite12;
    inline constexpr int CompSuiteVersion = kAEGPCompSuiteVersion12;
    inline constexpr bool HasCompSuite12 = true;
#else
    using CompSuiteType = AEGP_CompSuite11;
    inline constexpr int CompSuiteVersion = kAEGPCompSuiteVersion11;
    inline constexpr bool HasCompSuite12 = false;
#endif

// =============================================================
// LightSuite
// SDK 2022/2023: V2 (kAEGPLightSuiteVersion2)
// SDK 24.4+: V3 (kAEGPLightSuiteVersion3) - adds environment light support
// =============================================================
#if defined(kAEGPLightSuiteVersion3)
    using LightSuiteType = AEGP_LightSuite3;
    inline constexpr int LightSuiteVersion = kAEGPLightSuiteVersion3;
    inline constexpr bool HasLightSuite3 = true;
#else
    using LightSuiteType = AEGP_LightSuite2;
    inline constexpr int LightSuiteVersion = kAEGPLightSuiteVersion2;
    inline constexpr bool HasLightSuite3 = false;
#endif

// =============================================================
// IOInSuite
// SDK 2022/2023: V5 (kAEGPIOInSuiteVersion5)
// SDK 25.3+: V7 (kAEGPIOInSuiteVersion7)
// =============================================================
#if defined(kAEGPIOInSuiteVersion7)
    using IOInSuiteType = AEGP_IOInSuite7;
    inline constexpr int IOInSuiteVersion = kAEGPIOInSuiteVersion7;
#elif defined(kAEGPIOInSuiteVersion6)
    using IOInSuiteType = AEGP_IOInSuite6;
    inline constexpr int IOInSuiteVersion = kAEGPIOInSuiteVersion6;
#else
    using IOInSuiteType = AEGP_IOInSuite5;
    inline constexpr int IOInSuiteVersion = kAEGPIOInSuiteVersion5;
#endif

// =============================================================
// IOOutSuite
// SDK 2022/2023: V5 (kAEGPIOOutSuiteVersion5)
// SDK 25.2+: V6 (kAEGPIOOutSuiteVersion6)
// =============================================================
#if defined(kAEGPIOOutSuiteVersion6)
    using IOOutSuiteType = AEGP_IOOutSuite6;
    inline constexpr int IOOutSuiteVersion = kAEGPIOOutSuiteVersion6;
#else
    using IOOutSuiteType = AEGP_IOOutSuite5;
    inline constexpr int IOOutSuiteVersion = kAEGPIOOutSuiteVersion5;
#endif

// =============================================================
// ColorSettingsSuite
// SDK 2022: V3 (kAEGPColorSettingsSuiteVersion3)
// SDK 2023: V5 (kAEGPColorSettingsSuiteVersion5)
// SDK 25.1+: V6 (kAEGPColorSettingsSuiteVersion6)
// =============================================================
#if defined(kAEGPColorSettingsSuiteVersion6)
    using ColorSettingsSuiteType = AEGP_ColorSettingsSuite6;
    inline constexpr int ColorSettingsSuiteVersion = kAEGPColorSettingsSuiteVersion6;
#elif defined(kAEGPColorSettingsSuiteVersion5)
    using ColorSettingsSuiteType = AEGP_ColorSettingsSuite5;
    inline constexpr int ColorSettingsSuiteVersion = kAEGPColorSettingsSuiteVersion5;
#else
    using ColorSettingsSuiteType = AEGP_ColorSettingsSuite3;
    inline constexpr int ColorSettingsSuiteVersion = kAEGPColorSettingsSuiteVersion3;
#endif

// =============================================================
// EffectSuite
// SDK 2022/2023: V4 (kAEGPEffectSuiteVersion4)
// SDK 24.1+: V5 (kAEGPEffectSuiteVersion5)
// =============================================================
#if defined(kAEGPEffectSuiteVersion5)
    using EffectSuiteType = AEGP_EffectSuite5;
    inline constexpr int EffectSuiteVersion = kAEGPEffectSuiteVersion5;
#else
    using EffectSuiteType = AEGP_EffectSuite4;
    inline constexpr int EffectSuiteVersion = kAEGPEffectSuiteVersion4;
#endif

// =============================================================
// StreamSuite
// SDK 2022: V5 (kAEGPStreamSuiteVersion5)
// SDK 24.0+: V6 (kAEGPStreamSuiteVersion6)
// =============================================================
#if defined(kAEGPStreamSuiteVersion6)
    using StreamSuiteType = AEGP_StreamSuite6;
    inline constexpr int StreamSuiteVersion = kAEGPStreamSuiteVersion6;
#else
    using StreamSuiteType = AEGP_StreamSuite5;
    inline constexpr int StreamSuiteVersion = kAEGPStreamSuiteVersion5;
#endif

// =============================================================
// LayerSuite
// SDK 2022: V8 (kAEGPLayerSuiteVersion8)
// SDK 23.0+: V9 (kAEGPLayerSuiteVersion9)
// =============================================================
#if defined(kAEGPLayerSuiteVersion9)
    using LayerSuiteType = AEGP_LayerSuite9;
    inline constexpr int LayerSuiteVersion = kAEGPLayerSuiteVersion9;
#else
    using LayerSuiteType = AEGP_LayerSuite8;
    inline constexpr int LayerSuiteVersion = kAEGPLayerSuiteVersion8;
#endif

// =============================================================
// KeyframeSuite
// SDK 2022: V4 (kAEGPKeyframeSuiteVersion4)
// SDK 22.5+: V5 (kAEGPKeyframeSuiteVersion5)
// =============================================================
#if defined(kAEGPKeyframeSuiteVersion5)
    using KeyframeSuiteType = AEGP_KeyframeSuite5;
    inline constexpr int KeyframeSuiteVersion = kAEGPKeyframeSuiteVersion5;
#else
    using KeyframeSuiteType = AEGP_KeyframeSuite4;
    inline constexpr int KeyframeSuiteVersion = kAEGPKeyframeSuiteVersion4;
#endif

// =============================================================
// SDKInfo - Debugging and diagnostics
// =============================================================
struct SDKInfo {
    static constexpr int CompSuiteVer = CompSuiteVersion;
    static constexpr int LightSuiteVer = LightSuiteVersion;
    static constexpr int IOInSuiteVer = IOInSuiteVersion;
    static constexpr int IOOutSuiteVer = IOOutSuiteVersion;
    static constexpr int ColorSettingsSuiteVer = ColorSettingsSuiteVersion;
    static constexpr int EffectSuiteVer = EffectSuiteVersion;

    // Feature flags
    static constexpr bool HasEnvironmentLight = HasLightSuite3;
    static constexpr bool HasTextLayerHorizontal = HasCompSuite12;
    static constexpr bool HasLightSourceAPI = HasLightSuite3;

    // Get a human-readable SDK description
    static const char* GetSDKDescription() {
#if defined(kAEGPCompSuiteVersion12) && defined(kAEGPLightSuiteVersion3)
        return "AE 24.0+ SDK (CompSuite12, LightSuite3)";
#elif defined(kAEGPCompSuiteVersion12)
        return "AE 24.0+ SDK (CompSuite12)";
#else
        return "AE 22.x/23.x SDK (CompSuite11, LightSuite2)";
#endif
    }

    // Get detailed version info
    static const char* GetVersionDetails() {
#if defined(kAEGPIOInSuiteVersion7)
        return "IOInSuite7, IOOutSuite6, ColorSettingsSuite6, EffectSuite5";
#elif defined(kAEGPColorSettingsSuiteVersion6)
        return "IOInSuite5, IOOutSuite6, ColorSettingsSuite6, EffectSuite5";
#elif defined(kAEGPColorSettingsSuiteVersion5)
        return "IOInSuite5, IOOutSuite5, ColorSettingsSuite5, EffectSuite4";
#else
        return "IOInSuite5, IOOutSuite5, ColorSettingsSuite3, EffectSuite4";
#endif
    }
};

} // namespace PyAE::SDKCompat
