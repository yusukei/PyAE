// PyPropertyTypes.h
// PyAE - Python for After Effects
// プロパティ関連の型定義

#pragma once

#include "AE_GeneralPlug.h"

namespace PyAE {

// =============================================================
// ストリーム値タイプ（SDK AEGP_StreamType に対応）
// =============================================================
enum class StreamValueType {
    NoData = 0,          // AEGP_StreamType_NO_DATA - 値なし
    OneDim,              // AEGP_StreamType_OneD - 1次元
    TwoDim,              // AEGP_StreamType_TwoD - 2次元
    TwoDimSpatial,       // AEGP_StreamType_TwoD_SPATIAL - 2次元空間
    ThreeDim,            // AEGP_StreamType_ThreeD - 3次元
    ThreeDimSpatial,     // AEGP_StreamType_ThreeD_SPATIAL - 3次元空間
    Color,               // AEGP_StreamType_COLOR - 色
    Arbitrary,           // AEGP_StreamType_ARB - カスタムデータ
    Marker,              // AEGP_StreamType_MARKER - マーカー
    LayerID,             // AEGP_StreamType_LAYER_ID - レイヤー参照
    MaskID,              // AEGP_StreamType_MASK_ID - マスク参照
    Mask,                // AEGP_StreamType_MASK - マスク形状
    TextDocument         // AEGP_StreamType_TEXT_DOCUMENT - テキスト
};

// =============================================================
// ストリーム構造タイプ（SDK AEGP_StreamGroupingType に対応）
// =============================================================
enum class StreamGroupingType {
    Invalid = -1,        // AEGP_StreamGroupingType_NONE
    Leaf = 0,            // AEGP_StreamGroupingType_LEAF
    NamedGroup,          // AEGP_StreamGroupingType_NAMED_GROUP
    IndexedGroup         // AEGP_StreamGroupingType_INDEXED_GROUP
};

// =============================================================
// 後方互換性用 PropertyType（簡略化タイプ）
// =============================================================
enum class PropertyType {
    None = 0,
    OneDim,     // 1次元
    TwoDim,     // 2次元
    ThreeDim,   // 3次元
    Color,      // 色
    Arbitrary,  // カスタムデータ
    NoValue     // 値なし（グループなど）
};

// =============================================================
// SDK型からの変換ユーティリティ
// =============================================================
namespace PropertyTypeUtils {

// AEGP_StreamType → StreamValueType
inline StreamValueType FromAEStreamType(AEGP_StreamType type) {
    switch (type) {
        case AEGP_StreamType_NO_DATA:       return StreamValueType::NoData;
        case AEGP_StreamType_OneD:          return StreamValueType::OneDim;
        case AEGP_StreamType_TwoD:          return StreamValueType::TwoDim;
        case AEGP_StreamType_TwoD_SPATIAL:  return StreamValueType::TwoDimSpatial;
        case AEGP_StreamType_ThreeD:        return StreamValueType::ThreeDim;
        case AEGP_StreamType_ThreeD_SPATIAL: return StreamValueType::ThreeDimSpatial;
        case AEGP_StreamType_COLOR:         return StreamValueType::Color;
        case AEGP_StreamType_ARB:           return StreamValueType::Arbitrary;
        case AEGP_StreamType_MARKER:        return StreamValueType::Marker;
        case AEGP_StreamType_LAYER_ID:      return StreamValueType::LayerID;
        case AEGP_StreamType_MASK_ID:       return StreamValueType::MaskID;
        case AEGP_StreamType_MASK:          return StreamValueType::Mask;
        case AEGP_StreamType_TEXT_DOCUMENT: return StreamValueType::TextDocument;
        default:                            return StreamValueType::NoData;
    }
}

// AEGP_StreamGroupingType → StreamGroupingType
inline StreamGroupingType FromAEGroupingType(AEGP_StreamGroupingType type) {
    switch (type) {
        case AEGP_StreamGroupingType_NONE:          return StreamGroupingType::Invalid;
        case AEGP_StreamGroupingType_LEAF:          return StreamGroupingType::Leaf;
        case AEGP_StreamGroupingType_NAMED_GROUP:   return StreamGroupingType::NamedGroup;
        case AEGP_StreamGroupingType_INDEXED_GROUP: return StreamGroupingType::IndexedGroup;
        default:                                    return StreamGroupingType::Invalid;
    }
}

// StreamValueType → 後方互換PropertyType
inline PropertyType ToLegacyType(StreamValueType type) {
    switch (type) {
        case StreamValueType::NoData:        return PropertyType::NoValue;
        case StreamValueType::OneDim:        return PropertyType::OneDim;
        case StreamValueType::TwoDim:
        case StreamValueType::TwoDimSpatial: return PropertyType::TwoDim;
        case StreamValueType::ThreeDim:
        case StreamValueType::ThreeDimSpatial: return PropertyType::ThreeDim;
        case StreamValueType::Color:         return PropertyType::Color;
        case StreamValueType::Arbitrary:     return PropertyType::Arbitrary;
        default:                             return PropertyType::None;
    }
}

// StreamValueType がプリミティブ型か判定
inline bool IsPrimitiveType(StreamValueType type) {
    switch (type) {
        case StreamValueType::OneDim:
        case StreamValueType::TwoDim:
        case StreamValueType::TwoDimSpatial:
        case StreamValueType::ThreeDim:
        case StreamValueType::ThreeDimSpatial:
        case StreamValueType::Color:
            return true;
        default:
            return false;
    }
}

// StreamValueType がエクスプレッションを持てない型か判定
// Note: TextDocument はエクスプレッション対応（Source Text Expressionは一般的な機能）
// Note: Mask（マスクパス）もエクスプレッション対応（マスクパスのアニメーション）
inline bool IsExpressionForbiddenType(StreamValueType type) {
    switch (type) {
        case StreamValueType::Marker:
        case StreamValueType::LayerID:
        case StreamValueType::MaskID:
        case StreamValueType::NoData:
            return true;
        default:
            return false;
    }
}

// 型名を文字列で取得
inline const char* StreamValueTypeName(StreamValueType type) {
    switch (type) {
        case StreamValueType::NoData:        return "NoData";
        case StreamValueType::OneDim:        return "OneDim";
        case StreamValueType::TwoDim:        return "TwoDim";
        case StreamValueType::TwoDimSpatial: return "TwoDimSpatial";
        case StreamValueType::ThreeDim:      return "ThreeDim";
        case StreamValueType::ThreeDimSpatial: return "ThreeDimSpatial";
        case StreamValueType::Color:         return "Color";
        case StreamValueType::Arbitrary:     return "Arbitrary";
        case StreamValueType::Marker:        return "Marker";
        case StreamValueType::LayerID:       return "LayerID";
        case StreamValueType::MaskID:        return "MaskID";
        case StreamValueType::Mask:          return "Mask";
        case StreamValueType::TextDocument:  return "TextDocument";
        default:                             return "Unknown";
    }
}

inline const char* StreamGroupingTypeName(StreamGroupingType type) {
    switch (type) {
        case StreamGroupingType::Invalid:      return "Invalid";
        case StreamGroupingType::Leaf:         return "Leaf";
        case StreamGroupingType::NamedGroup:   return "NamedGroup";
        case StreamGroupingType::IndexedGroup: return "IndexedGroup";
        default:                               return "Unknown";
    }
}

} // namespace PropertyTypeUtils

} // namespace PyAE
