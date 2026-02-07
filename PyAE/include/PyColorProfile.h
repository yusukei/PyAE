// PyColorProfile.h
// PyAE - Python for After Effects
// カラープロファイル関連クラスの宣言
//
// ColorSettingsSuite6の高レベルAPI

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// PyColorProfile - カラープロファイルクラス
// AEGP_ColorProfilePのラッパー
// =============================================================
class PyColorProfile {
public:
    // Default constructor (invalid profile)
    PyColorProfile();

    // Constructor from raw pointer (takes ownership)
    explicit PyColorProfile(AEGP_ColorProfileP profileP);

    // Copy constructor (disabled - profiles cannot be copied)
    PyColorProfile(const PyColorProfile&) = delete;
    PyColorProfile& operator=(const PyColorProfile&) = delete;

    // Move constructor
    PyColorProfile(PyColorProfile&& other) noexcept;
    PyColorProfile& operator=(PyColorProfile&& other) noexcept;

    // Destructor - disposes the profile
    ~PyColorProfile();

    // Check if profile is valid
    bool IsValid() const;

    // Get profile description (human-readable name)
    std::string GetDescription() const;

    // Get approximate gamma value
    float GetGamma() const;

    // Check if this is an RGB color profile
    bool IsRGB() const;

    // Export to ICC profile data
    py::bytes ToICCData() const;

    // Get raw handle (for internal use)
    AEGP_ColorProfileP GetHandle() const;

    // Release ownership and return handle (caller takes ownership)
    AEGP_ColorProfileP Release();

    // Static factory methods

    // Create from ICC profile data
    static std::shared_ptr<PyColorProfile> FromICCData(const py::bytes& iccData);

    // Create from composition's working space
    static std::shared_ptr<PyColorProfile> FromComp(uintptr_t compH);

private:
    AEGP_ColorProfileP m_profileP;

    // Internal dispose helper
    void Dispose();
};

// =============================================================
// PyColorSettings - カラー設定モジュール関数
// プロジェクト/アプリケーションレベルの設定
// =============================================================
namespace ColorSettings {

// Check if OCIO color management is used
bool IsOCIOEnabled();

// Get OCIO configuration file name
std::string GetOCIOConfigFile();

// Get OCIO configuration file full path
std::string GetOCIOConfigFilePath();

// Get OCIO working color space name
std::string GetOCIOWorkingColorSpace();

// Get OCIO display and view names
py::tuple GetOCIODisplayView();

// Check if color space aware effects are enabled
bool IsColorSpaceAwareEffectsEnabled();

// Get LUT interpolation method
int GetLUTInterpolationMethod();

// Get graphics white luminance
int GetGraphicsWhiteLuminance();

// Get working color space GUID
py::bytes GetWorkingColorSpaceId();

} // namespace ColorSettings

} // namespace PyAE
