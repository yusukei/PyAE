// AETypeUtils.h
// PyAE - Python for After Effects
// AE型変換ユーティリティ

#pragma once

#include "AE_GeneralPlug.h"
#include <string>
#include <unordered_map>
#include <optional>

namespace PyAE {
namespace AETypeUtils {

// =============================================================
// A_Ratio 変換
// =============================================================

// double → A_Ratio 変換
// 警告: reinterpret_cast<const A_Ratio*>(&doubleValue) は完全に間違い。
// A_Ratio は分子/分母のペアであり、double とはメモリレイアウトが異なる。
inline A_Ratio DoubleToRatio(double value, A_long denominator = 10000) {
    A_Ratio ratio;
    ratio.num = static_cast<A_long>(value * denominator);
    ratio.den = denominator;
    return ratio;
}

// A_Ratio → double 変換
inline double RatioToDouble(const A_Ratio& ratio) {
    if (ratio.den == 0) return 0.0;
    return static_cast<double>(ratio.num) / static_cast<double>(ratio.den);
}

// =============================================================
// A_Time 変換
// =============================================================

// double秒 → A_Time 変換
inline A_Time SecondsToTime(double seconds, A_u_long scale = 1000000) {
    A_Time time;
    time.scale = scale;
    time.value = static_cast<A_long>(seconds * scale);
    return time;
}

// A_Time → double秒 変換
inline double TimeToSeconds(const A_Time& time) {
    if (time.scale == 0) return 0.0;
    return static_cast<double>(time.value) / static_cast<double>(time.scale);
}

// フレームレートを考慮したA_Time作成
inline A_Time SecondsToTimeWithFps(double seconds, double fps) {
    // fpsに基づいてスケールを決定（高精度のため1000倍）
    A_u_long scale = static_cast<A_u_long>(fps * 1000);
    if (scale == 0) scale = 30000; // デフォルト30fps

    A_Time time;
    time.scale = scale;
    time.value = static_cast<A_long>(seconds * scale);
    return time;
}

// =============================================================
// A_FloatPoint / A_FloatPoint3 変換
// =============================================================

// 2Dポイント
inline A_FloatPoint MakeFloatPoint(double x, double y) {
    A_FloatPoint pt;
    pt.x = static_cast<A_FpLong>(x);
    pt.y = static_cast<A_FpLong>(y);
    return pt;
}

// 3Dポイント
inline A_FloatPoint3 MakeFloatPoint3(double x, double y, double z) {
    A_FloatPoint3 pt;
    pt.x = static_cast<A_FpLong>(x);
    pt.y = static_cast<A_FpLong>(y);
    pt.z = static_cast<A_FpLong>(z);
    return pt;
}

// =============================================================
// AEGP_ColorVal 変換
// =============================================================

// RGBA配列からAEGP_ColorVal作成
inline AEGP_ColorVal MakeColorVal(double r, double g, double b, double a = 1.0) {
    AEGP_ColorVal color;
    color.redF = static_cast<A_FpShort>(r);
    color.greenF = static_cast<A_FpShort>(g);
    color.blueF = static_cast<A_FpShort>(b);
    color.alphaF = static_cast<A_FpShort>(a);
    return color;
}

// =============================================================
// ストリーム型判定
// =============================================================

// ストリームタイプから次元数を取得
inline int GetStreamDimensionality(AEGP_StreamType type) {
    switch (type) {
        case AEGP_StreamType_NO_DATA:
            return 0;
        case AEGP_StreamType_OneD:
            return 1;
        case AEGP_StreamType_TwoD:
        case AEGP_StreamType_TwoD_SPATIAL:
            return 2;
        case AEGP_StreamType_ThreeD:
        case AEGP_StreamType_ThreeD_SPATIAL:
            return 3;
        case AEGP_StreamType_COLOR:
            return 4;  // RGBA
        default:
            return 0;
    }
}

// ストリームタイプが空間タイプかどうか
inline bool IsSpatialStream(AEGP_StreamType type) {
    return type == AEGP_StreamType_TwoD_SPATIAL ||
           type == AEGP_StreamType_ThreeD_SPATIAL;
}

// =============================================================
// レイヤーストリーム名マッピング
// =============================================================

// 文字列名からAEGP_LayerStreamへのマッピング（共通定義）
inline const std::unordered_map<std::string, AEGP_LayerStream>& GetLayerStreamMap() {
    static const std::unordered_map<std::string, AEGP_LayerStream> streamMap = {
        // Position
        {"Position", AEGP_LayerStream_POSITION},
        {"position", AEGP_LayerStream_POSITION},
        {"ADBE Position", AEGP_LayerStream_POSITION},
        // Scale
        {"Scale", AEGP_LayerStream_SCALE},
        {"scale", AEGP_LayerStream_SCALE},
        {"ADBE Scale", AEGP_LayerStream_SCALE},
        // Rotation
        {"Rotation", AEGP_LayerStream_ROTATION},
        {"rotation", AEGP_LayerStream_ROTATION},
        {"ADBE Rotation", AEGP_LayerStream_ROTATION},
        {"Z Rotation", AEGP_LayerStream_ROTATE_Z},
        {"ADBE Rotate Z", AEGP_LayerStream_ROTATE_Z},
        {"X Rotation", AEGP_LayerStream_ROTATE_X},
        {"ADBE Rotate X", AEGP_LayerStream_ROTATE_X},
        {"Y Rotation", AEGP_LayerStream_ROTATE_Y},
        {"ADBE Rotate Y", AEGP_LayerStream_ROTATE_Y},
        // Opacity
        {"Opacity", AEGP_LayerStream_OPACITY},
        {"opacity", AEGP_LayerStream_OPACITY},
        {"ADBE Opacity", AEGP_LayerStream_OPACITY},
        // Anchor Point
        {"Anchor Point", AEGP_LayerStream_ANCHORPOINT},
        {"anchorpoint", AEGP_LayerStream_ANCHORPOINT},
        {"anchor_point", AEGP_LayerStream_ANCHORPOINT},
        {"ADBE Anchor Point", AEGP_LayerStream_ANCHORPOINT},
        // Orientation
        {"Orientation", AEGP_LayerStream_ORIENTATION},
        {"orientation", AEGP_LayerStream_ORIENTATION}
    };
    return streamMap;
}

// 文字列名からAEGP_LayerStreamを取得（見つからなければnulloptを返す）
inline std::optional<AEGP_LayerStream> GetLayerStreamFromName(const std::string& name) {
    const auto& streamMap = GetLayerStreamMap();
    auto it = streamMap.find(name);
    if (it != streamMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

} // namespace AETypeUtils
} // namespace PyAE
