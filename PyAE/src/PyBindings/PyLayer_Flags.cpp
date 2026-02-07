// PyLayer_Flags.cpp
// PyAE - Python for After Effects
// PyLayer フラグ関連実装
//
// PYAE-008b: PyLayer.cppから分割

#include "PyLayerClasses.h"

namespace PyAE {

// =============================================================
// PyLayer フラグ関連実装
// =============================================================

bool PyLayer::GetEnabled() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerFlags flags;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerFlags failed");
    }

    return (flags & AEGP_LayerFlag_VIDEO_ACTIVE) != 0;
}

void PyLayer::SetEnabled(bool enabled) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_VIDEO_ACTIVE, enabled ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer enabled state");
    }
}

bool PyLayer::GetAudioEnabled() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerFlags flags;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerFlags failed");
    }

    return (flags & AEGP_LayerFlag_AUDIO_ACTIVE) != 0;
}

void PyLayer::SetAudioEnabled(bool enabled) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_AUDIO_ACTIVE, enabled ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set audio enabled state");
    }
}

bool PyLayer::GetSolo() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerFlags flags;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerFlags failed");
    }

    return (flags & AEGP_LayerFlag_SOLO) != 0;
}

void PyLayer::SetSolo(bool solo) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_SOLO, solo ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set solo state");
    }
}

bool PyLayer::GetLocked() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerFlags flags;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerFlags failed");
    }

    return (flags & AEGP_LayerFlag_LOCKED) != 0;
}

void PyLayer::SetLocked(bool locked) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_LOCKED, locked ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set locked state");
    }
}

bool PyLayer::GetShy() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerFlags flags;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerFlags failed");
    }

    return (flags & AEGP_LayerFlag_SHY) != 0;
}

void PyLayer::SetShy(bool shy) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_SHY, shy ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set shy state");
    }
}

bool PyLayer::GetCollapseTransformation() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerFlags flags;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerFlags failed");
    }

    return (flags & AEGP_LayerFlag_COLLAPSE) != 0;
}

void PyLayer::SetCollapseTransformation(bool collapse) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        m_layerH, AEGP_LayerFlag_COLLAPSE, collapse ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set collapse transformation");
    }
}

} // namespace PyAE
