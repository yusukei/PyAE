// PyLayer_Matte.cpp
// PyAE - Python for After Effects
// PyLayer トラックマット実装

#include "PyLayerClasses.h"
#include "SDKVersionCompat.h"

namespace PyAE {

// =============================================================
// PyLayer トラックマット実装
// =============================================================

bool PyLayer::HasTrackMatte() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean has_matte = FALSE;
    A_Err err = suites.layerSuite->AEGP_DoesLayerHaveTrackMatte(m_layerH, &has_matte);
    if (err != A_Err_NONE) return false;

    return has_matte != FALSE;
}

bool PyLayer::IsUsedAsTrackMatte() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean is_track_matte = FALSE;
    A_Err err = suites.layerSuite->AEGP_IsLayerUsedAsTrackMatte(
        m_layerH,
        TRUE,  // fill_must_be_activeB - check if fill layer must be active
        &is_track_matte);
    if (err != A_Err_NONE) return false;

    return is_track_matte != FALSE;
}

py::object PyLayer::GetTrackMatteLayer() const {
#if defined(kAEGPLayerSuiteVersion9)
    if (!m_layerH) return py::none();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerH matteLayerH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetTrackMatteLayer(m_layerH, &matteLayerH);
    if (err != A_Err_NONE || !matteLayerH) {
        return py::none();
    }

    return py::cast(PyLayer(matteLayerH));
#else
    throw std::runtime_error("GetTrackMatteLayer requires AE 23.0+ (LayerSuite9)");
#endif
}

void PyLayer::SetTrackMatte(PyLayer& matte_layer, int matte_mode) {
#if defined(kAEGPLayerSuiteVersion9)
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }
    if (!matte_layer.m_layerH) {
        throw std::runtime_error("Invalid matte layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Validate matte_mode
    if (matte_mode < AEGP_TrackMatte_NO_TRACK_MATTE || matte_mode > AEGP_TrackMatte_NOT_LUMA) {
        throw std::runtime_error("Invalid track matte mode");
    }

    A_Err err = suites.layerSuite->AEGP_SetTrackMatte(
        m_layerH,
        matte_layer.m_layerH,
        static_cast<AEGP_TrackMatte>(matte_mode));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set track matte");
    }
#else
    (void)matte_layer;
    (void)matte_mode;
    throw std::runtime_error("SetTrackMatte requires AE 23.0+ (LayerSuite9)");
#endif
}

void PyLayer::RemoveTrackMatte() {
#if defined(kAEGPLayerSuiteVersion9)
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_RemoveTrackMatte(m_layerH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to remove track matte");
    }
#else
    throw std::runtime_error("RemoveTrackMatte requires AE 23.0+ (LayerSuite9)");
#endif
}

} // namespace PyAE
