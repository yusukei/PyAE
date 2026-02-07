// PyLayer_Time.cpp
// PyAE - Python for After Effects
// PyLayer 時間関連実装
//
// PYAE-008b: PyLayer.cppから分割

#include "PyLayerClasses.h"

namespace PyAE {

// =============================================================
// PyLayer 時間関連実装
// =============================================================

double PyLayer::GetInPoint() const {
    if (!m_layerH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time inPoint = {0, 1};  // Initialize with safe default
    A_Err err = suites.layerSuite->AEGP_GetLayerInPoint(m_layerH, AEGP_LTimeMode_CompTime, &inPoint);
    if (err != A_Err_NONE) {
        // Some layer types (camera, light) may not support in_point
        return 0.0;
    }

    if (inPoint.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(inPoint);
}

void PyLayer::SetInPoint(double seconds) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    // Validate non-negative time value
    if (seconds < 0.0) {
        throw std::invalid_argument(
            "In point must be non-negative, got " + std::to_string(seconds) + " seconds");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get current duration to preserve it
    A_Time currentDuration = {0, 1};
    A_Err err = suites.layerSuite->AEGP_GetLayerDuration(m_layerH, AEGP_LTimeMode_CompTime, &currentDuration);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get current duration");
    }

    // コンポのフレームレートを取得
    AEGP_CompH compH = nullptr;
    err = suites.layerSuite->AEGP_GetLayerParentComp(m_layerH, &compH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get layer parent composition");
    }

    A_FpLong fps = 30.0;
    if (compH) {
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get composition framerate");
        }
    }

    A_Time inPoint = AETypeUtils::SecondsToTimeWithFps(seconds, fps);

    // Must pass valid pointers - cannot use nullptr
    err = suites.layerSuite->AEGP_SetLayerInPointAndDuration(
        m_layerH, AEGP_LTimeMode_CompTime, &inPoint, &currentDuration);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set in point");
    }
}

double PyLayer::GetOutPoint() const {
    if (!m_layerH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time inPoint = {0, 1};  // Initialize with safe default
    A_Time duration = {0, 1}; // Initialize with safe default

    A_Err err = suites.layerSuite->AEGP_GetLayerInPoint(m_layerH, AEGP_LTimeMode_CompTime, &inPoint);
    if (err != A_Err_NONE) {
        // Some layer types (camera, light) may not support in_point
        return 0.0;
    }

    err = suites.layerSuite->AEGP_GetLayerDuration(m_layerH, AEGP_LTimeMode_CompTime, &duration);
    if (err != A_Err_NONE) {
        // Some layer types (camera, light) may not support duration
        return 0.0;
    }

    if (duration.scale == 0) return 0.0;

    // out = in + duration (同じscaleと仮定)
    return AETypeUtils::TimeToSeconds(inPoint) + AETypeUtils::TimeToSeconds(duration);
}

void PyLayer::SetOutPoint(double seconds) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    // Validate non-negative time value
    if (seconds < 0.0) {
        throw std::invalid_argument(
            "Out point must be non-negative, got " + std::to_string(seconds) + " seconds");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // in pointを取得
    A_Time inPoint = {0, 1};  // Initialize with safe default
    A_Err err = suites.layerSuite->AEGP_GetLayerInPoint(m_layerH, AEGP_LTimeMode_CompTime, &inPoint);
    if (err != A_Err_NONE) {
        // Some layer types (camera, light) may not support in_point/duration modification
        throw std::runtime_error("This layer type does not support out point modification");
    }

    // コンポのフレームレートを取得
    AEGP_CompH compH = nullptr;
    err = suites.layerSuite->AEGP_GetLayerParentComp(m_layerH, &compH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get layer parent composition");
    }

    A_FpLong fps = 30.0;
    if (compH) {
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get composition framerate");
        }
    }

    // duration = out - in
    double inSec = AETypeUtils::TimeToSeconds(inPoint);
    double durationSec = seconds - inSec;

    // Validate that out point is after in point
    if (durationSec <= 0.0) {
        throw std::invalid_argument(
            "Out point (" + std::to_string(seconds) + "s) must be after in point (" +
            std::to_string(inSec) + "s)");
    }

    A_Time duration = AETypeUtils::SecondsToTimeWithFps(durationSec, fps);

    // Must pass valid pointers - cannot use nullptr
    err = suites.layerSuite->AEGP_SetLayerInPointAndDuration(
        m_layerH, AEGP_LTimeMode_CompTime, &inPoint, &duration);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set out point");
    }
}

double PyLayer::GetStartTime() const {
    if (!m_layerH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time startTime;
    A_Err err = suites.layerSuite->AEGP_GetLayerOffset(m_layerH, &startTime);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerOffset failed");
    }
    if (startTime.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(startTime);
}

void PyLayer::SetStartTime(double seconds) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // コンポのフレームレートを取得
    AEGP_CompH compH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(m_layerH, &compH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get layer parent composition");
    }

    A_FpLong fps = 30.0;
    if (compH) {
        err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get composition framerate");
        }
    }

    A_Time startTime = AETypeUtils::SecondsToTimeWithFps(seconds, fps);

    err = suites.layerSuite->AEGP_SetLayerOffset(m_layerH, &startTime);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set start time");
    }
}

double PyLayer::GetDuration() const {
    if (!m_layerH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time duration = {0, 1};  // Initialize with safe default
    A_Err err = suites.layerSuite->AEGP_GetLayerDuration(m_layerH, AEGP_LTimeMode_CompTime, &duration);
    if (err != A_Err_NONE) {
        // Some layer types (camera, light) may not support duration
        return 0.0;
    }

    if (duration.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(duration);
}

std::pair<int, int> PyLayer::GetStretch() const {
    if (!m_layerH) return {100, 100};

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Ratio stretch = {100, 100};
    A_Err err = suites.layerSuite->AEGP_GetLayerStretch(m_layerH, &stretch);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerStretch failed");
    }

    return {stretch.num, stretch.den};
}

void PyLayer::SetStretch(int num, int den) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    // Zero division check
    if (den == 0) {
        throw std::invalid_argument(
            "Stretch denominator cannot be zero (got num=" + std::to_string(num) +
            ", den=" + std::to_string(den) + ")");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Ratio stretch = {num, den};
    A_Err err = suites.layerSuite->AEGP_SetLayerStretch(m_layerH, &stretch);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer stretch");
    }
}

double PyLayer::GetCurrentTime() const {
    if (!m_layerH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time currentTime = {0, 1};
    A_Err err = suites.layerSuite->AEGP_GetLayerCurrentTime(m_layerH, AEGP_LTimeMode_CompTime, &currentTime);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerCurrentTime failed");
    }
    if (currentTime.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(currentTime);
}

} // namespace PyAE
