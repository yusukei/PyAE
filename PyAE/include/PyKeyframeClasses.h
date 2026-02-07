// PyKeyframeClasses.h
// PyAE - Python for After Effects
// キーフレーム関連クラスの宣言
//
// PyKeyframe.cppから分割

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "PluginState.h"
#include "AETypeUtils.h"

namespace py = pybind11;

namespace PyAE {

// キーフレーム補間タイプ
enum class KeyInterpolation {
    Linear,
    Bezier,
    Hold
};

// イージングタイプ（プリセット）
enum class EasingType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Custom
};

// ベジェタンジェントデータ
struct BezierTangent {
    double speed;       // 速度
    double influence;   // 影響度 (0-100%)

    BezierTangent() : speed(0.0), influence(33.33) {}
    BezierTangent(double s, double i) : speed(s), influence(i) {}
};

// 空間タンジェントデータ（3D用）
struct SpatialTangent {
    double x;
    double y;
    double z;

    SpatialTangent() : x(0.0), y(0.0), z(0.0) {}
    SpatialTangent(double tx, double ty, double tz = 0.0) : x(tx), y(ty), z(tz) {}
};

// =============================================================
// PyKeyframe - キーフレームクラス
// =============================================================
class PyKeyframe {
public:
    PyKeyframe() : m_streamH(nullptr), m_keyIndex(-1) {}
    PyKeyframe(AEGP_StreamRefH streamH, A_long keyIndex)
        : m_streamH(streamH), m_keyIndex(keyIndex) {}

    bool IsValid() const;
    double GetTime() const;
    void SetTime(double newTime);
    py::object GetValue() const;
    void SetValue(py::object value);
    KeyInterpolation GetInInterpolation() const;
    KeyInterpolation GetOutInterpolation() const;
    void SetInterpolation(KeyInterpolation inType, KeyInterpolation outType);
    void SetEasing(EasingType easingType);
    BezierTangent GetTemporalEaseIn() const;
    BezierTangent GetTemporalEaseOut() const;
    void SetTemporalEaseIn(const BezierTangent& ease);
    void SetTemporalEaseOut(const BezierTangent& ease);
    SpatialTangent GetSpatialTangentIn() const;
    SpatialTangent GetSpatialTangentOut() const;
    void SetSpatialTangents(const SpatialTangent& tangentIn, const SpatialTangent& tangentOut);
    bool GetSelected() const;
    bool GetRoving() const;
    void SetRoving(bool roving);
    bool GetTemporalContinuous() const;
    void SetTemporalContinuous(bool continuous);
    bool GetTemporalAutoBezier() const;
    void SetTemporalAutoBezier(bool autoBezier);

    int GetIndex() const { return static_cast<int>(m_keyIndex); }
    AEGP_StreamRefH GetStreamHandle() const { return m_streamH; }

private:
    AEGP_StreamRefH m_streamH;
    A_long m_keyIndex;

    // Helper functions
    static KeyInterpolation AEInterpToKeyInterp(AEGP_KeyframeInterpolationType type);
    static AEGP_KeyframeInterpolationType KeyInterpToAEInterp(KeyInterpolation type);
    A_short GetStreamDimension() const;
    py::object StreamValueToPyObject(const AEGP_StreamValue2& value) const;
    bool PyObjectToStreamValue(py::object obj, AEGP_StreamType type, AEGP_StreamValue2& value) const;
};

} // namespace PyAE
