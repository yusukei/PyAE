// PyLayer_Transform.cpp
// PyAE - Python for After Effects
// PyLayer Transform関連実装
//
// PYAE-008b: PyLayer.cppから分割

#include "PyLayerClasses.h"

namespace PyAE {

// =============================================================
// PyLayer Transform関連実装
// =============================================================

py::object PyLayer::GetPosition() const {
    return GetStreamValue(AEGP_LayerStream_POSITION);
}

void PyLayer::SetPosition(py::object value) {
    SetStreamValue(AEGP_LayerStream_POSITION, value);
}

py::object PyLayer::GetScale() const {
    return GetStreamValue(AEGP_LayerStream_SCALE);
}

void PyLayer::SetScale(py::object value) {
    SetStreamValue(AEGP_LayerStream_SCALE, value);
}

py::object PyLayer::GetRotation() const {
    return GetStreamValue(AEGP_LayerStream_ROTATION);
}

void PyLayer::SetRotation(py::object value) {
    SetStreamValue(AEGP_LayerStream_ROTATION, value);
}

py::object PyLayer::GetAnchorPoint() const {
    return GetStreamValue(AEGP_LayerStream_ANCHORPOINT);
}

void PyLayer::SetAnchorPoint(py::object value) {
    SetStreamValue(AEGP_LayerStream_ANCHORPOINT, value);
}

double PyLayer::GetOpacity() const {
    auto val = GetStreamValue(AEGP_LayerStream_OPACITY);
    if (val.is_none()) return 100.0;
    return val.cast<double>();
}

void PyLayer::SetOpacity(double value) {
    SetStreamValue(AEGP_LayerStream_OPACITY, py::float_(value));
}

// =============================================================
// プライベートヘルパーメソッド
// =============================================================

py::object PyLayer::GetStreamValue(AEGP_LayerStream streamType) const {
    if (!m_layerH) return py::none();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), m_layerH, streamType, &streamH);
    if (err != A_Err_NONE || !streamH) {
        return py::none();
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    // Check stream type BEFORE getting value - NO_DATA streams cannot have values
    AEGP_StreamType type;
    err = suites.streamSuite->AEGP_GetStreamType(streamH, &type);
    if (err != A_Err_NONE || type == AEGP_StreamType_NO_DATA) {
        return py::none();
    }

    // 現在時刻（0秒）での値を取得
    A_Time time = {0, 1000};

    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) {
        return py::none();
    }

    py::object result;

    switch (type) {
        case AEGP_StreamType_OneD:
            result = py::float_(value.val.one_d);
            break;
        case AEGP_StreamType_TwoD:
        case AEGP_StreamType_TwoD_SPATIAL:
            result = py::make_tuple(value.val.two_d.x, value.val.two_d.y);
            break;
        case AEGP_StreamType_ThreeD:
        case AEGP_StreamType_ThreeD_SPATIAL:
            result = py::make_tuple(
                value.val.three_d.x,
                value.val.three_d.y,
                value.val.three_d.z
            );
            break;
        case AEGP_StreamType_COLOR:
            result = py::make_tuple(
                value.val.color.redF,
                value.val.color.greenF,
                value.val.color.blueF,
                value.val.color.alphaF
            );
            break;
        default:
            result = py::none();
            break;
    }

    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void PyLayer::SetStreamValue(AEGP_LayerStream streamType, py::object value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), m_layerH, streamType, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    // ストリームタイプを取得
    AEGP_StreamType type;
    suites.streamSuite->AEGP_GetStreamType(streamH, &type);

    AEGP_StreamValue2 streamValue = {};

    switch (type) {
        case AEGP_StreamType_OneD:
            streamValue.val.one_d = value.cast<double>();
            break;

        case AEGP_StreamType_TwoD:
        case AEGP_StreamType_TwoD_SPATIAL: {
            auto seq = value.cast<py::sequence>();
            size_t len = py::len(seq);
            if (len < 2) {
                throw std::runtime_error("2D stream requires at least 2 values");
            }
            streamValue.val.two_d.x = seq[0].cast<double>();
            streamValue.val.two_d.y = seq[1].cast<double>();
            break;
        }

        case AEGP_StreamType_ThreeD:
        case AEGP_StreamType_ThreeD_SPATIAL: {
            auto seq = value.cast<py::sequence>();
            size_t len = py::len(seq);
            if (len < 2) {
                throw std::runtime_error("3D stream requires at least 2 values (X, Y), Z is optional");
            }
            streamValue.val.three_d.x = seq[0].cast<double>();
            streamValue.val.three_d.y = seq[1].cast<double>();
            // Z is optional - default to 0.0 if not provided (for 2D layers)
            streamValue.val.three_d.z = (len >= 3) ? seq[2].cast<double>() : 0.0;
            break;
        }

        case AEGP_StreamType_COLOR: {
            auto seq = value.cast<py::sequence>();
            size_t len = py::len(seq);
            if (len < 3) {
                throw std::runtime_error("Color stream requires at least 3 values (RGB)");
            }
            streamValue.val.color.redF = seq[0].cast<double>();
            streamValue.val.color.greenF = seq[1].cast<double>();
            streamValue.val.color.blueF = seq[2].cast<double>();
            if (len > 3) {
                streamValue.val.color.alphaF = seq[3].cast<double>();
            } else {
                streamValue.val.color.alphaF = 1.0;
            }
            break;
        }

        default:
            throw std::runtime_error("Cannot set value for this stream type");
    }

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set stream value");
    }
}

// =============================================================
// High-level Keyframe API
// =============================================================

void PyLayer::SetPositionKeyframe(double time, py::object value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), m_layerH, AEGP_LayerStream_POSITION, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get position stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    // Convert time to A_Time
    A_Time aeTime;
    aeTime.value = static_cast<A_long>(time * 1000);
    aeTime.scale = 1000;

    // Convert value to stream value
    AEGP_StreamValue2 streamValue = {};
    auto seq = value.cast<py::sequence>();
    size_t len = py::len(seq);

    if (len < 2) {
        throw std::runtime_error("Position value must have at least 2 components (X, Y)");
    }

    // Always use 3D format and default Z to 0.0 if not provided
    streamValue.val.three_d.x = seq[0].cast<double>();
    streamValue.val.three_d.y = seq[1].cast<double>();
    streamValue.val.three_d.z = (len >= 3) ? seq[2].cast<double>() : 0.0;

    // Add keyframe
    A_long keyIndex = 0;
    err = suites.keyframeSuite->AEGP_InsertKeyframe(streamH, AEGP_LTimeMode_CompTime, &aeTime, &keyIndex);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to insert position keyframe");
    }

    // Set keyframe value using AEGP_SetKeyframeValue
    err = suites.keyframeSuite->AEGP_SetKeyframeValue(streamH, keyIndex, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set position keyframe value");
    }
}

void PyLayer::SetScaleKeyframe(double time, py::object value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), m_layerH, AEGP_LayerStream_SCALE, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get scale stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    // Convert time to A_Time
    A_Time aeTime;
    aeTime.value = static_cast<A_long>(time * 1000);
    aeTime.scale = 1000;

    // Convert value to stream value
    AEGP_StreamValue2 streamValue = {};
    auto seq = value.cast<py::sequence>();
    size_t len = py::len(seq);

    if (len < 2) {
        throw std::runtime_error("Scale value must have at least 2 components (X, Y)");
    }

    // Always use 3D format and default Z to 100.0 (100%) if not provided
    streamValue.val.three_d.x = seq[0].cast<double>();
    streamValue.val.three_d.y = seq[1].cast<double>();
    streamValue.val.three_d.z = (len >= 3) ? seq[2].cast<double>() : 100.0;

    // Add keyframe
    A_long keyIndex = 0;
    err = suites.keyframeSuite->AEGP_InsertKeyframe(streamH, AEGP_LTimeMode_CompTime, &aeTime, &keyIndex);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to insert scale keyframe");
    }

    // Set keyframe value using AEGP_SetKeyframeValue
    err = suites.keyframeSuite->AEGP_SetKeyframeValue(streamH, keyIndex, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set scale keyframe value");
    }
}

void PyLayer::SetRotationKeyframe(double time, double value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), m_layerH, AEGP_LayerStream_ROTATION, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get rotation stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    // Convert time to A_Time
    A_Time aeTime;
    aeTime.value = static_cast<A_long>(time * 1000);
    aeTime.scale = 1000;

    // Convert value to stream value
    AEGP_StreamValue2 streamValue = {};
    streamValue.val.one_d = value;

    // Add keyframe
    A_long keyIndex = 0;
    err = suites.keyframeSuite->AEGP_InsertKeyframe(streamH, AEGP_LTimeMode_CompTime, &aeTime, &keyIndex);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to insert rotation keyframe");
    }

    // Set keyframe value using AEGP_SetKeyframeValue
    err = suites.keyframeSuite->AEGP_SetKeyframeValue(streamH, keyIndex, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set rotation keyframe value");
    }
}

void PyLayer::SetOpacityKeyframe(double time, double value) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), m_layerH, AEGP_LayerStream_OPACITY, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get opacity stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    // Convert time to A_Time
    A_Time aeTime;
    aeTime.value = static_cast<A_long>(time * 1000);
    aeTime.scale = 1000;

    // Convert value to stream value
    AEGP_StreamValue2 streamValue = {};
    streamValue.val.one_d = value;

    // Add keyframe
    A_long keyIndex = 0;
    err = suites.keyframeSuite->AEGP_InsertKeyframe(streamH, AEGP_LTimeMode_CompTime, &aeTime, &keyIndex);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to insert opacity keyframe");
    }

    // Set keyframe value using AEGP_SetKeyframeValue
    err = suites.keyframeSuite->AEGP_SetKeyframeValue(streamH, keyIndex, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set opacity keyframe value");
    }
}

// =============================================================
// Transfer Mode
// =============================================================

py::dict PyLayer::GetTransferMode() const {
    if (!m_layerH) return py::dict();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerTransferMode transferMode;
    A_Err err = suites.layerSuite->AEGP_GetLayerTransferMode(m_layerH, &transferMode);
    if (err != A_Err_NONE) return py::dict();

    py::dict result;
    result["mode"] = static_cast<PyAE::BlendMode>(transferMode.mode);
    result["flags"] = static_cast<int>(transferMode.flags);
    result["track_matte"] = static_cast<PyAE::TrackMatteMode>(transferMode.track_matte);

    return result;
}

void PyLayer::SetTransferMode(const py::dict& transfer_mode) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerTransferMode tm;
    tm.mode = transfer_mode["mode"].cast<PF_TransferMode>();
    tm.flags = transfer_mode.contains("flags") ? transfer_mode["flags"].cast<AEGP_TransferFlags>() : 0;
    tm.track_matte = transfer_mode.contains("track_matte") ? transfer_mode["track_matte"].cast<AEGP_TrackMatte>() : AEGP_TrackMatte_NO_TRACK_MATTE;

    A_Err err = suites.layerSuite->AEGP_SetLayerTransferMode(m_layerH, &tm);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer transfer mode");
    }
}

// =============================================================
// Transform Matrix
// =============================================================

py::list PyLayer::ToWorldXform(double comp_time) const {
    if (!m_layerH) return py::list();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get parent comp to retrieve FPS
    AEGP_CompH compH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetLayerParentComp(m_layerH, &compH);
    if (err != A_Err_NONE || !compH) return py::list();

    A_FpLong fps = 30.0;
    err = suites.compSuite->AEGP_GetCompFramerate(compH, &fps);
    if (err != A_Err_NONE) return py::list();

    A_Time time = AETypeUtils::SecondsToTimeWithFps(comp_time, fps);
    A_Matrix4 matrix;
    err = suites.layerSuite->AEGP_GetLayerToWorldXform(m_layerH, &time, &matrix);
    if (err != A_Err_NONE) return py::list();

    // Convert to nested list (4x4 matrix)
    py::list result;
    for (int i = 0; i < 4; i++) {
        py::list row;
        for (int j = 0; j < 4; j++) {
            row.append(matrix.mat[i][j]);
        }
        result.append(row);
    }
    return result;
}

} // namespace PyAE
