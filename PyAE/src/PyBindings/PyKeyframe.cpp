// PyKeyframe.cpp
// PyAE - Python for After Effects
// キーフレームAPIバインディング
//
// PYAE: クラス定義をPyKeyframeClasses.hに分離

#include "PyKeyframeClasses.h"
#include "ErrorHandling.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// PyKeyframe - メソッド実装
// =============================================================

bool PyKeyframe::IsValid() const {
    if (m_streamH == nullptr || m_keyIndex < 0) {
        return false;
    }

    // Check upper bound - verify keyIndex is within valid range
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long numKeys = 0;
    A_Err err = suites.keyframeSuite->AEGP_GetStreamNumKFs(m_streamH, &numKeys);
    if (err != A_Err_NONE) {
        return false;
    }

    return m_keyIndex < numKeys;
}

double PyKeyframe::GetTime() const {
    if (!IsValid()) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time time;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeTime(
        m_streamH, m_keyIndex, AEGP_LTimeMode_CompTime, &time);
    if (err != A_Err_NONE || time.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(time);
}

void PyKeyframe::SetTime(double newTime) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    // Note: AE SDK does not provide AEGP_SetKeyframeTime directly.
    // To change keyframe time, you would need to:
    // 1. Get the current keyframe value and properties
    // 2. Delete the current keyframe
    // 3. Insert a new keyframe at the new time
    // 4. Set the value and properties on the new keyframe
    // This is not implemented yet due to complexity.
    throw std::runtime_error("SetTime is not supported in this version of AE SDK. "
                             "To move a keyframe, delete it and create a new one at the desired time.");
}

py::object PyKeyframe::GetValue() const {
    if (!IsValid()) return py::none();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Check stream type - NO_DATA streams cannot have values
    AEGP_StreamType streamType;
    A_Err err = suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);
    if (err != A_Err_NONE || streamType == AEGP_StreamType_NO_DATA) return py::none();

    // Get keyframe time
    A_Time time;
    err = suites.keyframeSuite->AEGP_GetKeyframeTime(
        m_streamH, m_keyIndex, AEGP_LTimeMode_CompTime, &time);
    if (err != A_Err_NONE) return py::none();

    // Get value at that time
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), m_streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) return py::none();

    py::object result = StreamValueToPyObject(value);
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void PyKeyframe::SetValue(py::object value) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get stream type
    AEGP_StreamType streamType;
    A_Err err = suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get stream type");
    }

    // Build value
    AEGP_StreamValue2 streamValue;
    memset(&streamValue, 0, sizeof(streamValue));
    streamValue.streamH = m_streamH;

    if (!PyObjectToStreamValue(value, streamType, streamValue)) {
        throw std::runtime_error("Failed to convert value");
    }

    // Set keyframe value
    err = suites.keyframeSuite->AEGP_SetKeyframeValue(
        m_streamH, m_keyIndex, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set keyframe value");
    }
}

KeyInterpolation PyKeyframe::GetInInterpolation() const {
    if (!IsValid()) return KeyInterpolation::Linear;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_KeyframeInterpolationType inType, outType;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeInterpolation(
        m_streamH, m_keyIndex, &inType, &outType);
    if (err != A_Err_NONE) return KeyInterpolation::Linear;

    return AEInterpToKeyInterp(inType);
}

KeyInterpolation PyKeyframe::GetOutInterpolation() const {
    if (!IsValid()) return KeyInterpolation::Linear;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_KeyframeInterpolationType inType, outType;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeInterpolation(
        m_streamH, m_keyIndex, &inType, &outType);
    if (err != A_Err_NONE) return KeyInterpolation::Linear;

    return AEInterpToKeyInterp(outType);
}

void PyKeyframe::SetInterpolation(KeyInterpolation inType, KeyInterpolation outType) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.keyframeSuite->AEGP_SetKeyframeInterpolation(
        m_streamH, m_keyIndex, KeyInterpToAEInterp(inType), KeyInterpToAEInterp(outType));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set interpolation");
    }
}

void PyKeyframe::SetEasing(EasingType easingType) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    switch (easingType) {
        case EasingType::Linear:
            SetInterpolation(KeyInterpolation::Linear, KeyInterpolation::Linear);
            break;
        case EasingType::EaseIn:
            SetInterpolation(KeyInterpolation::Bezier, KeyInterpolation::Linear);
            SetTemporalEaseIn(BezierTangent(0.0, 33.33));
            break;
        case EasingType::EaseOut:
            SetInterpolation(KeyInterpolation::Linear, KeyInterpolation::Bezier);
            SetTemporalEaseOut(BezierTangent(0.0, 33.33));
            break;
        case EasingType::EaseInOut:
            SetInterpolation(KeyInterpolation::Bezier, KeyInterpolation::Bezier);
            SetTemporalEaseIn(BezierTangent(0.0, 33.33));
            SetTemporalEaseOut(BezierTangent(0.0, 33.33));
            break;
        default:
            break;
    }
}

BezierTangent PyKeyframe::GetTemporalEaseIn() const {
    if (!IsValid()) return BezierTangent();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get dimension count
    A_short dimension = 1;
    AEGP_StreamType streamType;
    suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);
    if (streamType == AEGP_StreamType_TwoD || streamType == AEGP_StreamType_TwoD_SPATIAL) {
        dimension = 2;
    } else if (streamType == AEGP_StreamType_ThreeD || streamType == AEGP_StreamType_ThreeD_SPATIAL) {
        dimension = 3;
    }

    AEGP_KeyframeEase easeIn, easeOut;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeTemporalEase(
        m_streamH, m_keyIndex, 0, &easeIn, &easeOut);
    if (err != A_Err_NONE) return BezierTangent();

    return BezierTangent(easeIn.speedF, easeIn.influenceF * 100.0);
}

BezierTangent PyKeyframe::GetTemporalEaseOut() const {
    if (!IsValid()) return BezierTangent();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_KeyframeEase easeIn, easeOut;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeTemporalEase(
        m_streamH, m_keyIndex, 0, &easeIn, &easeOut);
    if (err != A_Err_NONE) return BezierTangent();

    return BezierTangent(easeOut.speedF, easeOut.influenceF * 100.0);
}

void PyKeyframe::SetTemporalEaseIn(const BezierTangent& ease) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get current values
    AEGP_KeyframeEase easeIn, easeOut;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeTemporalEase(
        m_streamH, m_keyIndex, 0, &easeIn, &easeOut);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get current ease");
    }

    // Update input side only
    easeIn.speedF = static_cast<A_FpLong>(ease.speed);
    easeIn.influenceF = static_cast<A_FpLong>(ease.influence / 100.0);

    // Temporal ease applies to the entire keyframe, not per-dimension
    // Always use dimension=0 regardless of stream type
    err = suites.keyframeSuite->AEGP_SetKeyframeTemporalEase(
        m_streamH, m_keyIndex, 0, &easeIn, &easeOut);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set temporal ease");
    }
}

void PyKeyframe::SetTemporalEaseOut(const BezierTangent& ease) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_KeyframeEase easeIn, easeOut;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeTemporalEase(
        m_streamH, m_keyIndex, 0, &easeIn, &easeOut);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get current ease");
    }

    easeOut.speedF = static_cast<A_FpLong>(ease.speed);
    easeOut.influenceF = static_cast<A_FpLong>(ease.influence / 100.0);

    // Temporal ease applies to the entire keyframe, not per-dimension
    // Always use dimension=0 regardless of stream type
    err = suites.keyframeSuite->AEGP_SetKeyframeTemporalEase(
        m_streamH, m_keyIndex, 0, &easeIn, &easeOut);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set temporal ease");
    }
}

SpatialTangent PyKeyframe::GetSpatialTangentIn() const {
    if (!IsValid()) return SpatialTangent();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamValue2 tangentIn, tangentOut;
    A_Err err = suites.keyframeSuite->AEGP_GetNewKeyframeSpatialTangents(
        state.GetPluginID(), m_streamH, m_keyIndex, &tangentIn, &tangentOut);
    if (err != A_Err_NONE) return SpatialTangent();

    // Convert based on stream type
    AEGP_StreamType streamType;
    suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);

    SpatialTangent result;
    if (streamType == AEGP_StreamType_TwoD_SPATIAL) {
        result.x = tangentIn.val.two_d.x;
        result.y = tangentIn.val.two_d.y;
    } else if (streamType == AEGP_StreamType_ThreeD_SPATIAL) {
        result.x = tangentIn.val.three_d.x;
        result.y = tangentIn.val.three_d.y;
        result.z = tangentIn.val.three_d.z;
    }

    return result;
}

SpatialTangent PyKeyframe::GetSpatialTangentOut() const {
    if (!IsValid()) return SpatialTangent();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamValue2 tangentIn, tangentOut;
    A_Err err = suites.keyframeSuite->AEGP_GetNewKeyframeSpatialTangents(
        state.GetPluginID(), m_streamH, m_keyIndex, &tangentIn, &tangentOut);
    if (err != A_Err_NONE) return SpatialTangent();

    AEGP_StreamType streamType;
    suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);

    SpatialTangent result;
    if (streamType == AEGP_StreamType_TwoD_SPATIAL) {
        result.x = tangentOut.val.two_d.x;
        result.y = tangentOut.val.two_d.y;
    } else if (streamType == AEGP_StreamType_ThreeD_SPATIAL) {
        result.x = tangentOut.val.three_d.x;
        result.y = tangentOut.val.three_d.y;
        result.z = tangentOut.val.three_d.z;
    }

    return result;
}

void PyKeyframe::SetSpatialTangents(const SpatialTangent& tangentIn, const SpatialTangent& tangentOut) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamType streamType;
    A_Err err = suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get stream type");
    }

    AEGP_StreamValue2 inVal, outVal;
    memset(&inVal, 0, sizeof(inVal));
    memset(&outVal, 0, sizeof(outVal));
    inVal.streamH = m_streamH;
    outVal.streamH = m_streamH;

    if (streamType == AEGP_StreamType_TwoD_SPATIAL) {
        inVal.val.two_d.x = static_cast<A_FpLong>(tangentIn.x);
        inVal.val.two_d.y = static_cast<A_FpLong>(tangentIn.y);
        outVal.val.two_d.x = static_cast<A_FpLong>(tangentOut.x);
        outVal.val.two_d.y = static_cast<A_FpLong>(tangentOut.y);
    } else if (streamType == AEGP_StreamType_ThreeD_SPATIAL) {
        inVal.val.three_d.x = static_cast<A_FpLong>(tangentIn.x);
        inVal.val.three_d.y = static_cast<A_FpLong>(tangentIn.y);
        inVal.val.three_d.z = static_cast<A_FpLong>(tangentIn.z);
        outVal.val.three_d.x = static_cast<A_FpLong>(tangentOut.x);
        outVal.val.three_d.y = static_cast<A_FpLong>(tangentOut.y);
        outVal.val.three_d.z = static_cast<A_FpLong>(tangentOut.z);
    } else {
        throw std::runtime_error("Spatial tangents only available for spatial properties");
    }

    err = suites.keyframeSuite->AEGP_SetKeyframeSpatialTangents(
        m_streamH, m_keyIndex, &inVal, &outVal);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set spatial tangents");
    }
}

bool PyKeyframe::GetSelected() const {
    if (!IsValid()) return false;

    // AEGP API limitation: no selection flag available
    // Keyframe flags only include: ROVING, TEMPORAL_CONTINUOUS, TEMPORAL_AUTOBEZIER
    return false;
}

bool PyKeyframe::GetRoving() const {
    if (!IsValid()) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_KeyframeFlags flags;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeFlags(m_streamH, m_keyIndex, &flags);
    if (err != A_Err_NONE) return false;

    return (flags & AEGP_KeyframeFlag_ROVING) != 0;
}

void PyKeyframe::SetRoving(bool roving) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.keyframeSuite->AEGP_SetKeyframeFlag(
        m_streamH, m_keyIndex, AEGP_KeyframeFlag_ROVING, roving ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set roving flag");
    }
}

bool PyKeyframe::GetTemporalContinuous() const {
    if (!IsValid()) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_KeyframeFlags flags;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeFlags(m_streamH, m_keyIndex, &flags);
    if (err != A_Err_NONE) return false;

    return (flags & AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS) != 0;
}

void PyKeyframe::SetTemporalContinuous(bool continuous) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.keyframeSuite->AEGP_SetKeyframeFlag(
        m_streamH, m_keyIndex, AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS, continuous ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set temporal continuous flag");
    }
}

bool PyKeyframe::GetTemporalAutoBezier() const {
    if (!IsValid()) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_KeyframeFlags flags;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeFlags(m_streamH, m_keyIndex, &flags);
    if (err != A_Err_NONE) return false;

    return (flags & AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER) != 0;
}

void PyKeyframe::SetTemporalAutoBezier(bool autoBezier) {
    if (!IsValid()) {
        throw std::runtime_error("Invalid keyframe");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.keyframeSuite->AEGP_SetKeyframeFlag(
        m_streamH, m_keyIndex, AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER, autoBezier ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set temporal auto bezier flag");
    }
}

// Helper functions
KeyInterpolation PyKeyframe::AEInterpToKeyInterp(AEGP_KeyframeInterpolationType type) {
    switch (type) {
        case AEGP_KeyInterp_LINEAR: return KeyInterpolation::Linear;
        case AEGP_KeyInterp_BEZIER: return KeyInterpolation::Bezier;
        case AEGP_KeyInterp_HOLD:   return KeyInterpolation::Hold;
        default:                    return KeyInterpolation::Linear;
    }
}

AEGP_KeyframeInterpolationType PyKeyframe::KeyInterpToAEInterp(KeyInterpolation type) {
    switch (type) {
        case KeyInterpolation::Linear: return AEGP_KeyInterp_LINEAR;
        case KeyInterpolation::Bezier: return AEGP_KeyInterp_BEZIER;
        case KeyInterpolation::Hold:   return AEGP_KeyInterp_HOLD;
        default:                       return AEGP_KeyInterp_LINEAR;
    }
}

A_short PyKeyframe::GetStreamDimension() const {
    if (!m_streamH) return 1;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamType streamType;
    suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);

    switch (streamType) {
        case AEGP_StreamType_TwoD:
        case AEGP_StreamType_TwoD_SPATIAL:
            return 2;
        case AEGP_StreamType_ThreeD:
        case AEGP_StreamType_ThreeD_SPATIAL:
            return 3;
        case AEGP_StreamType_COLOR:
            return 4;
        default:
            return 1;
    }
}

py::object PyKeyframe::StreamValueToPyObject(const AEGP_StreamValue2& value) const {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamType streamType;
    suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);

    switch (streamType) {
        case AEGP_StreamType_OneD:
            return py::float_(value.val.one_d);
        case AEGP_StreamType_TwoD:
        case AEGP_StreamType_TwoD_SPATIAL:
            return py::make_tuple(value.val.two_d.x, value.val.two_d.y);
        case AEGP_StreamType_ThreeD:
        case AEGP_StreamType_ThreeD_SPATIAL:
            return py::make_tuple(
                value.val.three_d.x,
                value.val.three_d.y,
                value.val.three_d.z
            );
        case AEGP_StreamType_COLOR:
            return py::make_tuple(
                value.val.color.redF,
                value.val.color.greenF,
                value.val.color.blueF,
                value.val.color.alphaF
            );
        default:
            return py::none();
    }
}

bool PyKeyframe::PyObjectToStreamValue(py::object obj, AEGP_StreamType type, AEGP_StreamValue2& value) const {
    try {
        switch (type) {
            case AEGP_StreamType_OneD:
                value.val.one_d = obj.cast<double>();
                break;
            case AEGP_StreamType_TwoD:
            case AEGP_StreamType_TwoD_SPATIAL: {
                py::tuple t = obj.cast<py::tuple>();
                value.val.two_d.x = t[0].cast<double>();
                value.val.two_d.y = t[1].cast<double>();
                break;
            }
            case AEGP_StreamType_ThreeD:
            case AEGP_StreamType_ThreeD_SPATIAL: {
                py::tuple t = obj.cast<py::tuple>();
                value.val.three_d.x = t[0].cast<double>();
                value.val.three_d.y = t[1].cast<double>();
                value.val.three_d.z = t[2].cast<double>();
                break;
            }
            case AEGP_StreamType_COLOR: {
                py::tuple t = obj.cast<py::tuple>();
                value.val.color.redF = t[0].cast<double>();
                value.val.color.greenF = t[1].cast<double>();
                value.val.color.blueF = t[2].cast<double>();
                value.val.color.alphaF = (py::len(t) > 3) ? t[3].cast<double>() : 1.0;
                break;
            }
            default:
                return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

// =============================================================
// KeyframeUtils - キーフレームユーティリティ
// =============================================================
class KeyframeUtils {
public:
    // Add keyframe to property
    static PyKeyframe AddKeyframe(AEGP_StreamRefH streamH, double time) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_Time timeT = AETypeUtils::SecondsToTime(time);

        A_long keyIndex;
        A_Err err = suites.keyframeSuite->AEGP_InsertKeyframe(
            streamH, AEGP_LTimeMode_CompTime, &timeT, &keyIndex);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to add keyframe");
        }

        return PyKeyframe(streamH, keyIndex);
    }

    // Delete keyframe
    static void DeleteKeyframe(AEGP_StreamRefH streamH, int index) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_Err err = suites.keyframeSuite->AEGP_DeleteKeyframe(
            streamH, static_cast<A_long>(index));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to delete keyframe");
        }
    }

    // Get all keyframes
    static std::vector<PyKeyframe> GetAllKeyframes(AEGP_StreamRefH streamH) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_long numKeys;
        A_Err err = suites.keyframeSuite->AEGP_GetStreamNumKFs(streamH, &numKeys);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get keyframe count");
        }

        std::vector<PyKeyframe> keyframes;
        keyframes.reserve(numKeys);

        for (A_long i = 0; i < numKeys; ++i) {
            keyframes.emplace_back(streamH, i);
        }

        return keyframes;
    }

    // Get keyframe index at specific time (-1 if not found)
    static int GetKeyframeIndexAtTime(AEGP_StreamRefH streamH, double time) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_Time timeT = AETypeUtils::SecondsToTime(time);

        A_long numKeys;
        A_Err err = suites.keyframeSuite->AEGP_GetStreamNumKFs(streamH, &numKeys);
        if (err != A_Err_NONE) return -1;

        for (A_long i = 0; i < numKeys; ++i) {
            A_Time keyTime;
            err = suites.keyframeSuite->AEGP_GetKeyframeTime(
                streamH, i, AEGP_LTimeMode_CompTime, &keyTime);
            if (err == A_Err_NONE) {
                // Tolerance: 1ms
                double diff = std::abs(AETypeUtils::TimeToSeconds(keyTime) - time);
                if (diff < 0.001) {
                    return static_cast<int>(i);
                }
            }
        }

        return -1;
    }

    // Delete all keyframes
    static void DeleteAllKeyframes(AEGP_StreamRefH streamH) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_long numKeys;
        A_Err err = suites.keyframeSuite->AEGP_GetStreamNumKFs(streamH, &numKeys);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get keyframe count");
        }

        // Delete from the end (to avoid index shifting)
        for (A_long i = numKeys - 1; i >= 0; --i) {
            err = suites.keyframeSuite->AEGP_DeleteKeyframe(streamH, i);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to delete keyframe");
            }
        }
    }
};

} // namespace PyAE

void init_keyframe(py::module_& m) {
    // Interpolation type enum
    py::enum_<PyAE::KeyInterpolation>(m, "KeyInterpolation")
        .value("Linear", PyAE::KeyInterpolation::Linear)
        .value("Bezier", PyAE::KeyInterpolation::Bezier)
        .value("Hold", PyAE::KeyInterpolation::Hold);

    // Easing type enum
    py::enum_<PyAE::EasingType>(m, "EasingType")
        .value("Linear", PyAE::EasingType::Linear)
        .value("EaseIn", PyAE::EasingType::EaseIn)
        .value("EaseOut", PyAE::EasingType::EaseOut)
        .value("EaseInOut", PyAE::EasingType::EaseInOut)
        .value("Custom", PyAE::EasingType::Custom);

    // BezierTangent struct
    py::class_<PyAE::BezierTangent>(m, "BezierTangent")
        .def(py::init<>())
        .def(py::init<double, double>(),
             py::arg("speed") = 0.0, py::arg("influence") = 33.33)
        .def_readwrite("speed", &PyAE::BezierTangent::speed,
                      "Speed of the tangent")
        .def_readwrite("influence", &PyAE::BezierTangent::influence,
                      "Influence percentage (0-100)")
        .def("__repr__", [](const PyAE::BezierTangent& self) {
            return std::string("<BezierTangent: speed=") + std::to_string(self.speed) +
                   " influence=" + std::to_string(self.influence) + ">";
        });

    // SpatialTangent struct
    py::class_<PyAE::SpatialTangent>(m, "SpatialTangent")
        .def(py::init<>())
        .def(py::init<double, double, double>(),
             py::arg("x") = 0.0, py::arg("y") = 0.0, py::arg("z") = 0.0)
        .def_readwrite("x", &PyAE::SpatialTangent::x)
        .def_readwrite("y", &PyAE::SpatialTangent::y)
        .def_readwrite("z", &PyAE::SpatialTangent::z)
        .def("__repr__", [](const PyAE::SpatialTangent& self) {
            return std::string("<SpatialTangent: (") + std::to_string(self.x) + ", " +
                   std::to_string(self.y) + ", " + std::to_string(self.z) + ")>";
        });

    // Keyframe class
    py::class_<PyAE::PyKeyframe>(m, "Keyframe")
        .def(py::init<>())
        .def_property_readonly("valid", &PyAE::PyKeyframe::IsValid,
                              "Check if keyframe is valid")
        .def_property("time", &PyAE::PyKeyframe::GetTime, &PyAE::PyKeyframe::SetTime,
                     "Keyframe time in seconds")
        .def_property("value", &PyAE::PyKeyframe::GetValue, &PyAE::PyKeyframe::SetValue,
                     "Keyframe value")
        .def_property_readonly("index", &PyAE::PyKeyframe::GetIndex,
                              "Keyframe index")
        .def_property_readonly("in_interpolation", &PyAE::PyKeyframe::GetInInterpolation,
                              "Incoming interpolation type")
        .def_property_readonly("out_interpolation", &PyAE::PyKeyframe::GetOutInterpolation,
                              "Outgoing interpolation type")
        .def("set_interpolation", &PyAE::PyKeyframe::SetInterpolation,
             "Set keyframe interpolation",
             py::arg("in_type"), py::arg("out_type"))
        .def("set_easing", &PyAE::PyKeyframe::SetEasing,
             "Set easing preset",
             py::arg("easing_type"))
        .def_property_readonly("temporal_ease_in", &PyAE::PyKeyframe::GetTemporalEaseIn,
                              "Get incoming temporal ease (bezier tangent)")
        .def_property_readonly("temporal_ease_out", &PyAE::PyKeyframe::GetTemporalEaseOut,
                              "Get outgoing temporal ease (bezier tangent)")
        .def("set_temporal_ease_in", &PyAE::PyKeyframe::SetTemporalEaseIn,
             "Set incoming temporal ease",
             py::arg("ease"))
        .def("set_temporal_ease_out", &PyAE::PyKeyframe::SetTemporalEaseOut,
             "Set outgoing temporal ease",
             py::arg("ease"))
        .def_property_readonly("spatial_tangent_in", &PyAE::PyKeyframe::GetSpatialTangentIn,
                              "Get incoming spatial tangent")
        .def_property_readonly("spatial_tangent_out", &PyAE::PyKeyframe::GetSpatialTangentOut,
                              "Get outgoing spatial tangent")
        .def("set_spatial_tangents", &PyAE::PyKeyframe::SetSpatialTangents,
             "Set spatial tangents for spatial properties",
             py::arg("tangent_in"), py::arg("tangent_out"))
        .def_property("roving", &PyAE::PyKeyframe::GetRoving, &PyAE::PyKeyframe::SetRoving,
                     "Whether this is a roving keyframe")
        .def_property("temporal_continuous", &PyAE::PyKeyframe::GetTemporalContinuous,
                     &PyAE::PyKeyframe::SetTemporalContinuous,
                     "Whether temporal interpolation is continuous")
        .def_property("temporal_auto_bezier", &PyAE::PyKeyframe::GetTemporalAutoBezier,
                     &PyAE::PyKeyframe::SetTemporalAutoBezier,
                     "Whether auto-bezier is enabled")
        .def("__repr__", [](const PyAE::PyKeyframe& self) {
            if (!self.IsValid()) return std::string("<Keyframe: invalid>");
            double time = self.GetTime();
            int idx = self.GetIndex();
            return std::string("<Keyframe: time=") + std::to_string(time) + " index=" + std::to_string(idx) + ">";
        })
        .def("__bool__", &PyAE::PyKeyframe::IsValid);
}
