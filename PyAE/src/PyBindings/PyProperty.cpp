// PyProperty.cpp
// PyAE - Python for After Effects
// プロパティAPI実装（新設計版）

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PyPropertyCore.h"
#include "PluginState.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "Logger.h"
#include "PyStreamUtils.h"
#include "PyCompClasses.h"
#include "PyKeyframeClasses.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// コンストラクタ / デストラクタ
// =============================================================

PyProperty::PyProperty()
    : m_streamH(nullptr)
    , m_ownsHandle(false)
    , m_unknownTypePolicy(StreamUtils::UnknownTypePolicy::ThrowException)
    , m_isEffectParam(false)
    , m_effectParamIndex(-1)
{}

PyProperty::PyProperty(AEGP_StreamRefH streamH, bool ownsHandle)
    : m_streamH(streamH)
    , m_ownsHandle(ownsHandle)
    , m_unknownTypePolicy(StreamUtils::UnknownTypePolicy::ThrowException)
    , m_isEffectParam(false)
    , m_effectParamIndex(-1)
{}

PyProperty::PyProperty(AEGP_StreamRefH streamH, bool ownsHandle,
                       StreamUtils::UnknownTypePolicy policy, int effectParamIndex)
    : m_streamH(streamH)
    , m_ownsHandle(ownsHandle)
    , m_unknownTypePolicy(policy)
    , m_isEffectParam(effectParamIndex >= 0)
    , m_effectParamIndex(effectParamIndex)
{}

PyProperty::~PyProperty() {
    if (m_ownsHandle && m_streamH) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.streamSuite) {
            suites.streamSuite->AEGP_DisposeStream(m_streamH);
        }
        m_streamH = nullptr;
    }
}

PyProperty::PyProperty(PyProperty&& other) noexcept
    : m_streamH(other.m_streamH)
    , m_ownsHandle(other.m_ownsHandle)
    , m_unknownTypePolicy(other.m_unknownTypePolicy)
    , m_isEffectParam(other.m_isEffectParam)
    , m_effectParamIndex(other.m_effectParamIndex)
{
    other.m_streamH = nullptr;
    other.m_ownsHandle = false;
}

PyProperty& PyProperty::operator=(PyProperty&& other) noexcept {
    if (this != &other) {
        if (m_ownsHandle && m_streamH) {
            auto& state = PluginState::Instance();
            const auto& suites = state.GetSuites();
            if (suites.streamSuite) {
                A_Err err = suites.streamSuite->AEGP_DisposeStream(m_streamH);
                if (err != A_Err_NONE) {
                    PYAE_LOG_WARNING("PyProperty", "Failed to dispose stream in move assignment (error code: " + std::to_string(err) + ")");
                }
            }
        }
        m_streamH = other.m_streamH;
        m_ownsHandle = other.m_ownsHandle;
        m_unknownTypePolicy = other.m_unknownTypePolicy;
        m_isEffectParam = other.m_isEffectParam;
        m_effectParamIndex = other.m_effectParamIndex;
        other.m_streamH = nullptr;
        other.m_ownsHandle = false;
    }
    return *this;
}

// =============================================================
// 基本情報
// =============================================================

bool PyProperty::IsValid() const {
    return m_streamH != nullptr;
}

std::string PyProperty::GetName() const {
    if (!m_streamH) return "";

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_MemHandle nameH;
    A_Err err = suites.streamSuite->AEGP_GetStreamName(
        state.GetPluginID(), m_streamH, FALSE, &nameH);
    if (err != A_Err_NONE || !nameH) return "";

    ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);

    // ScopedMemLockを使用して例外安全にロック/アンロック
    ScopedMemLock lock(suites.memorySuite, nameH);
    A_UTF16Char* namePtr = lock.As<A_UTF16Char>();
    if (!namePtr) return "";

    std::string result = StringUtils::Utf16ToUtf8(namePtr);

    return result;
}

void PyProperty::SetName(const std::string& name) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property stream");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // UTF-8 → UTF-16 変換
    std::wstring wname = StringUtils::Utf8ToWide(name);

    // AEGP_SetStreamName を呼び出し
    A_Err err = suites.dynamicStreamSuite->AEGP_SetStreamName(
        m_streamH,
        reinterpret_cast<const A_UTF16Char*>(wname.c_str()));

    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set property name");
    }
}

std::string PyProperty::GetMatchName() const {
    if (!m_streamH) return "";

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.dynamicStreamSuite) return "";

    char matchName[AEGP_MAX_STREAM_MATCH_NAME_SIZE + 1];
    matchName[AEGP_MAX_STREAM_MATCH_NAME_SIZE] = '\0';  // null終端を保証
    A_Err err = suites.dynamicStreamSuite->AEGP_GetMatchName(m_streamH, matchName);
    if (err != A_Err_NONE) return "";

    return std::string(matchName);
}

int PyProperty::GetDepth() const {
    if (!m_streamH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.dynamicStreamSuite) return 0;

    A_long depth;
    A_Err err = suites.dynamicStreamSuite->AEGP_GetStreamDepth(m_streamH, &depth);
    if (err != A_Err_NONE) return 0;

    return static_cast<int>(depth);
}

// =============================================================
// 型情報
// =============================================================

AEGP_StreamType PyProperty::GetRawStreamType() const {
    if (!m_streamH) return AEGP_StreamType_NO_DATA;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamType type;
    A_Err err = suites.streamSuite->AEGP_GetStreamType(m_streamH, &type);
    if (err != A_Err_NONE) return AEGP_StreamType_NO_DATA;

    return type;
}

AEGP_StreamGroupingType PyProperty::GetRawGroupingType() const {
    if (!m_streamH) return AEGP_StreamGroupingType_NONE;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.dynamicStreamSuite) return AEGP_StreamGroupingType_NONE;

    AEGP_StreamGroupingType type;
    A_Err err = suites.dynamicStreamSuite->AEGP_GetStreamGroupingType(m_streamH, &type);
    if (err != A_Err_NONE) return AEGP_StreamGroupingType_NONE;

    return type;
}

StreamValueType PyProperty::GetStreamValueType() const {
    return PropertyTypeUtils::FromAEStreamType(GetRawStreamType());
}

StreamGroupingType PyProperty::GetGroupingType() const {
    return PropertyTypeUtils::FromAEGroupingType(GetRawGroupingType());
}

PropertyType PyProperty::GetType() const {
    return PropertyTypeUtils::ToLegacyType(GetStreamValueType());
}

bool PyProperty::IsGroup() const {
    AEGP_StreamGroupingType type = GetRawGroupingType();
    return (type == AEGP_StreamGroupingType_NAMED_GROUP ||
            type == AEGP_StreamGroupingType_INDEXED_GROUP);
}

bool PyProperty::IsLeaf() const {
    return GetRawGroupingType() == AEGP_StreamGroupingType_LEAF;
}

// =============================================================
// 能力判定（SDK API直接使用）
// =============================================================

bool PyProperty::CanVaryOverTime() const {
    if (!m_streamH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // SDK API を直接使用
    A_Boolean canVary = FALSE;
    A_Err err = suites.streamSuite->AEGP_CanVaryOverTime(m_streamH, &canVary);
    if (err != A_Err_NONE) return false;

    return canVary != FALSE;
}

bool PyProperty::CanHaveKeyframes() const {
    if (!m_streamH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long numKFs;
    A_Err err = suites.keyframeSuite->AEGP_GetStreamNumKFs(m_streamH, &numKFs);
    if (err != A_Err_NONE) return false;

    // AEGP_NumKF_NO_DATA (-1) ならキーフレーム不可
    return numKFs != AEGP_NumKF_NO_DATA;
}

bool PyProperty::CanHaveExpression() const {
    if (!m_streamH) return false;

    // グループはエクスプレッションを持てない
    if (!IsLeaf()) return false;

    // 時間変化できないプロパティはエクスプレッションを持てない
    if (!CanVaryOverTime()) return false;

    // MASK, TEXT_DOCUMENT はキーフレームは可能だがエクスプレッション不可
    StreamValueType type = GetStreamValueType();
    return !PropertyTypeUtils::IsExpressionForbiddenType(type);
}

bool PyProperty::CanSetValue() const {
    // グループは値を設定できない
    if (!IsLeaf()) return false;

    // プリミティブ型のみ AEGP_SetStreamValue が使用可能
    return IsPrimitive();
}

bool PyProperty::IsPrimitive() const {
    return PropertyTypeUtils::IsPrimitiveType(GetStreamValueType());
}

bool PyProperty::IsEffectParam() const {
    return m_isEffectParam;
}

int PyProperty::GetEffectParamIndex() const {
    return m_effectParamIndex;
}

// =============================================================
// 値操作
// =============================================================

AEGP_StreamValue2 PyProperty::PythonToStreamValue(py::object value) const {
    return StreamUtils::PythonToStreamValue(
        value, m_streamH,
        m_unknownTypePolicy,
        m_isEffectParam ? "PyAE_EffectArb" : "PyAE_ArbData"
    );
}

py::object PyProperty::StreamValueToPython(const AEGP_StreamValue2& value) const {
    return StreamUtils::StreamValueToPython(value, m_streamH);
}

py::object PyProperty::GetValue(double time) const {
    if (!m_streamH) return py::none();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (m_isEffectParam) {
        // Effect params: check for NO_DATA only (same as old EffectParam behavior)
        AEGP_StreamType streamType;
        A_Err err = suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);
        if (err != A_Err_NONE || streamType == AEGP_StreamType_NO_DATA) {
            return py::none();
        }
    } else {
        // Normal property: full type checks
        if (!IsLeaf()) return py::none();

        StreamValueType type = GetStreamValueType();

        if (type == StreamValueType::NoData) {
            return py::none();
        }

        // Reference types and markers need special handling
        if (type == StreamValueType::Marker ||
            type == StreamValueType::LayerID ||
            type == StreamValueType::MaskID ||
            type == StreamValueType::Mask) {
            return py::none();
        }

        // TextDocument has dedicated processing
        if (type == StreamValueType::TextDocument) {
            return GetTextDocumentValue(time);
        }
    }

    A_Time timeT = AETypeUtils::SecondsToTime(time);

    AEGP_StreamValue2 value;
    A_Err err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), m_streamH, AEGP_LTimeMode_CompTime, &timeT, FALSE, &value);
    if (err != A_Err_NONE) {
        return py::none();
    }

    py::object result = StreamValueToPython(value);
    suites.streamSuite->AEGP_DisposeStreamValue(&value);

    return result;
}

py::bytes PyProperty::GetRawBytes(double time) const {
    if (!m_streamH) return py::bytes("");

    // グループは値を持たない
    if (!IsLeaf()) return py::bytes("");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    StreamValueType type = GetStreamValueType();

    // NO_DATAは空を返す
    if (type == StreamValueType::NoData) {
        return py::bytes("");
    }

    A_Time timeT = AETypeUtils::SecondsToTime(time);

    AEGP_StreamValue2 value;
    A_Err err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), m_streamH, AEGP_LTimeMode_CompTime, &timeT, FALSE, &value);
    if (err != A_Err_NONE) {
        return py::bytes("");
    }

    py::bytes result;

    // IEEE 754 ビッグエンディアンで出力（XMLのbdataと同形式）
    auto doubleToBytes = [](double val) -> std::string {
        union { double d; uint64_t u; } conv;
        conv.d = val;
        char buf[8];
        for (int i = 0; i < 8; ++i) {
            buf[i] = static_cast<char>((conv.u >> (56 - i * 8)) & 0xFF);
        }
        return std::string(buf, 8);
    };

    switch (type) {
        case StreamValueType::OneDim: {
            result = py::bytes(doubleToBytes(value.val.one_d));
            break;
        }
        case StreamValueType::TwoDim:
        case StreamValueType::TwoDimSpatial: {
            std::string data = doubleToBytes(value.val.two_d.x) +
                              doubleToBytes(value.val.two_d.y);
            result = py::bytes(data);
            break;
        }
        case StreamValueType::ThreeDim:
        case StreamValueType::ThreeDimSpatial: {
            std::string data = doubleToBytes(value.val.three_d.x) +
                              doubleToBytes(value.val.three_d.y) +
                              doubleToBytes(value.val.three_d.z);
            result = py::bytes(data);
            break;
        }
        case StreamValueType::Color: {
            std::string data = doubleToBytes(value.val.color.redF) +
                              doubleToBytes(value.val.color.greenF) +
                              doubleToBytes(value.val.color.blueF) +
                              doubleToBytes(value.val.color.alphaF);
            result = py::bytes(data);
            break;
        }
        case StreamValueType::Arbitrary: {
            // ARBデータはメモリハンドルから取得
            if (value.val.arbH) {
                void* ptr = nullptr;
                AEGP_MemHandle arbH = reinterpret_cast<AEGP_MemHandle>(value.val.arbH);
                err = suites.memorySuite->AEGP_LockMemHandle(arbH, &ptr);
                if (err == A_Err_NONE && ptr) {
                    A_u_long size = 0;
                    suites.memorySuite->AEGP_GetMemHandleSize(arbH, &size);
                    result = py::bytes(static_cast<const char*>(ptr), size);
                    suites.memorySuite->AEGP_UnlockMemHandle(arbH);
                }
            }
            break;
        }
        case StreamValueType::TextDocument: {
            // TextDocumentSuiteでテキストを取得し、UTF-16LEバイナリとして出力
            if (suites.textDocSuite && value.val.text_documentH) {
                AEGP_MemHandle textH = nullptr;
                A_Err textErr = suites.textDocSuite->AEGP_GetNewText(
                    state.GetPluginID(), value.val.text_documentH, &textH);

                if (textErr == A_Err_NONE && textH) {
                    // ScopedMemHandle でリソース管理
                    ScopedMemHandle scopedText(state.GetPluginID(), suites.memorySuite, textH);

                    // テキストをロックして読み取り
                    void* textPtr = nullptr;
                    A_Err lockErr = suites.memorySuite->AEGP_LockMemHandle(textH, &textPtr);
                    if (lockErr == A_Err_NONE && textPtr) {
                        // メモリサイズを取得
                        A_u_long textSize = 0;
                        suites.memorySuite->AEGP_GetMemHandleSize(textH, &textSize);

                        // UTF-16LEとしてバイナリ出力
                        result = py::bytes(static_cast<const char*>(textPtr), textSize);

                        suites.memorySuite->AEGP_UnlockMemHandle(textH);
                    }
                }
            }
            break;
        }
        case StreamValueType::Mask: {
            // MaskOutlineSuiteでマスクデータを取得
            // マスクの頂点データをバイナリ化
            if (suites.maskOutlineSuite && value.val.mask) {
                A_long numSegs = 0;
                err = suites.maskOutlineSuite->AEGP_GetMaskOutlineNumSegments(
                    value.val.mask, &numSegs);
                if (err == A_Err_NONE && numSegs > 0) {
                    std::string data;
                    // セグメント数を先頭に
                    for (int i = 0; i < 4; ++i) {
                        data += static_cast<char>((numSegs >> (24 - i * 8)) & 0xFF);
                    }
                    // 各頂点の座標を取得
                    for (A_long i = 0; i < numSegs; ++i) {
                        AEGP_MaskVertex vertex;
                        err = suites.maskOutlineSuite->AEGP_GetMaskOutlineVertexInfo(
                            value.val.mask, i, &vertex);
                        if (err == A_Err_NONE) {
                            data += doubleToBytes(vertex.x);
                            data += doubleToBytes(vertex.y);
                            data += doubleToBytes(vertex.tan_in_x);
                            data += doubleToBytes(vertex.tan_in_y);
                            data += doubleToBytes(vertex.tan_out_x);
                            data += doubleToBytes(vertex.tan_out_y);
                        }
                    }
                    result = py::bytes(data);
                }
            }
            break;
        }
        case StreamValueType::LayerID: {
            // レイヤーIDを4バイト整数として出力
            A_long layerId = value.val.layer_id;
            char buf[4];
            for (int i = 0; i < 4; ++i) {
                buf[i] = static_cast<char>((layerId >> (24 - i * 8)) & 0xFF);
            }
            result = py::bytes(buf, 4);
            break;
        }
        case StreamValueType::MaskID: {
            // マスクIDを4バイト整数として出力
            A_long maskId = value.val.mask_id;
            char buf[4];
            for (int i = 0; i < 4; ++i) {
                buf[i] = static_cast<char>((maskId >> (24 - i * 8)) & 0xFF);
            }
            result = py::bytes(buf, 4);
            break;
        }
        case StreamValueType::Marker: {
            // マーカーは複雑なので、現時点では空を返す
            // 将来的にMarkerSuiteで詳細取得可能
            break;
        }
        default:
            break;
    }

    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

// =============================================================
// メタデータ取得
// =============================================================

py::dict PyProperty::GetStreamMetadata() const {
    py::dict metadata;

    if (!m_streamH) {
        return metadata;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // AEGP_GetStreamProperties で flags, min, max を取得
    AEGP_StreamFlags flags = 0;
    A_FpLong minVal = 0.0, maxVal = 0.0;
    A_Err err = suites.streamSuite->AEGP_GetStreamProperties(
        m_streamH, &flags, &minVal, &maxVal);

    if (err == A_Err_NONE) {
        metadata["flags"] = static_cast<int>(flags);
        metadata["min_value"] = minVal;
        metadata["max_value"] = maxVal;
    }

    // グループ化タイプ
    StreamGroupingType grouping = GetGroupingType();
    metadata["grouping_type"] = PropertyTypeUtils::StreamGroupingTypeName(grouping);

    // ストリーム値タイプ
    StreamValueType streamType = GetStreamValueType();
    metadata["stream_value_type"] = PropertyTypeUtils::StreamValueTypeName(streamType);

    // 能力フラグ
    metadata["can_vary_over_time"] = CanVaryOverTime();
    metadata["can_have_keyframes"] = CanHaveKeyframes();
    metadata["can_have_expression"] = CanHaveExpression();
    metadata["is_primitive"] = IsPrimitive();

    return metadata;
}

// =============================================================
// TextDocument 値操作
// =============================================================

py::object PyProperty::GetTextDocumentValue(double time) const {
    if (!m_streamH) return py::none();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // TextDocumentSuiteが利用可能か確認
    if (!suites.textDocSuite) {
        PYAE_LOG_WARNING("Property", "TextDocumentSuite not available");
        return py::none();
    }

    A_Time timeT = AETypeUtils::SecondsToTime(time);

    // StreamValue を取得
    AEGP_StreamValue2 value;
    A_Err err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), m_streamH, AEGP_LTimeMode_CompTime, &timeT, FALSE, &value);
    if (err != A_Err_NONE) {
        PYAE_LOG_WARNING("Property", "Failed to get TextDocument stream value");
        return py::none();
    }

    py::object result = py::none();

    // TextDocumentH からテキストを取得
    if (value.val.text_documentH) {
        AEGP_MemHandle textH = nullptr;
        err = suites.textDocSuite->AEGP_GetNewText(
            state.GetPluginID(), value.val.text_documentH, &textH);

        if (err == A_Err_NONE && textH) {
            // ScopedMemHandle でリソース管理
            ScopedMemHandle scopedText(state.GetPluginID(), suites.memorySuite, textH);

            // テキストをロックして読み取り
            ScopedMemLock lock(suites.memorySuite, textH);
            A_UTF16Char* textPtr = lock.As<A_UTF16Char>();

            if (textPtr) {
                // UTF-16 から UTF-8 へ変換
                std::string utf8Text = StringUtils::Utf16ToUtf8(textPtr);
                result = py::str(utf8Text);
            }
        } else {
            PYAE_LOG_WARNING("Property", "Failed to get text from TextDocument (error: " + std::to_string(err) + ")");
        }
    }

    // StreamValue を解放
    suites.streamSuite->AEGP_DisposeStreamValue(&value);

    return result;
}

void PyProperty::SetTextDocumentValue(double time, const std::string& text) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // TextDocumentSuiteが利用可能か確認
    if (!suites.textDocSuite) {
        throw std::runtime_error("TextDocumentSuite not available");
    }

    A_Time timeT = AETypeUtils::SecondsToTime(time);

    // 現在のStreamValueを取得
    AEGP_StreamValue2 value;
    A_Err err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), m_streamH, AEGP_LTimeMode_CompTime, &timeT, FALSE, &value);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get TextDocument stream value");
    }

    // テキストを設定
    if (value.val.text_documentH) {
        // UTF-8 から UTF-16 へ変換
        std::wstring wtext = StringUtils::Utf8ToWide(text);
        const A_u_short* unicodePtr = reinterpret_cast<const A_u_short*>(wtext.c_str());
        A_long lengthL = static_cast<A_long>(wtext.length());

        err = suites.textDocSuite->AEGP_SetText(
            value.val.text_documentH, unicodePtr, lengthL);
        if (err != A_Err_NONE) {
            suites.streamSuite->AEGP_DisposeStreamValue(&value);
            throw std::runtime_error("Failed to set text in TextDocument");
        }

        // 変更をストリームに書き戻す
        err = suites.streamSuite->AEGP_SetStreamValue(
            state.GetPluginID(), m_streamH, &value);
        if (err != A_Err_NONE) {
            suites.streamSuite->AEGP_DisposeStreamValue(&value);
            throw std::runtime_error("Failed to set TextDocument stream value");
        }
    } else {
        suites.streamSuite->AEGP_DisposeStreamValue(&value);
        throw std::runtime_error("TextDocumentH is null");
    }

    // StreamValue を解放
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
}

void PyProperty::SetShapePathVertices(const py::list& vertices, double time) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    StreamValueType type = GetStreamValueType();
    if (type != StreamValueType::Mask) {
        throw std::runtime_error("Property is not a Mask/Path type");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.maskOutlineSuite) {
        throw std::runtime_error("MaskOutlineSuite not available");
    }

    // StreamValue を取得
    A_Time timeT = AETypeUtils::SecondsToTime(time);
    AEGP_StreamValue2 value;
    A_Err err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), m_streamH, AEGP_LTimeMode_CompTime, &timeT, FALSE, &value);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get stream value for Shape Path");
    }

    ScopedStreamValue scopedValue(suites.streamSuite, value);

    if (!value.val.mask) {
        throw std::runtime_error("MaskOutlineH is null");
    }

    AEGP_MaskOutlineValH outlineH = value.val.mask;

    // 既存の頂点をすべてクリア
    A_long numSegs = 0;
    err = suites.maskOutlineSuite->AEGP_GetMaskOutlineNumSegments(outlineH, &numSegs);
    if (err == A_Err_NONE && numSegs > 0) {
        for (A_long i = numSegs - 1; i >= 0; --i) {
            suites.maskOutlineSuite->AEGP_DeleteVertex(outlineH, i);
        }
    }

    // 新しい頂点を追加
    for (size_t i = 0; i < vertices.size(); ++i) {
        py::dict vertexDict = vertices[i].cast<py::dict>();

        // 頂点を作成
        err = suites.maskOutlineSuite->AEGP_CreateVertex(outlineH, static_cast<A_long>(i));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to create vertex");
        }

        // 頂点情報を設定
        AEGP_MaskVertex vertex = {};
        if (vertexDict.contains("x")) vertex.x = vertexDict["x"].cast<double>();
        if (vertexDict.contains("y")) vertex.y = vertexDict["y"].cast<double>();
        if (vertexDict.contains("tan_in_x")) vertex.tan_in_x = vertexDict["tan_in_x"].cast<double>();
        if (vertexDict.contains("tan_in_y")) vertex.tan_in_y = vertexDict["tan_in_y"].cast<double>();
        if (vertexDict.contains("tan_out_x")) vertex.tan_out_x = vertexDict["tan_out_x"].cast<double>();
        if (vertexDict.contains("tan_out_y")) vertex.tan_out_y = vertexDict["tan_out_y"].cast<double>();

        err = suites.maskOutlineSuite->AEGP_SetMaskOutlineVertexInfo(outlineH, static_cast<A_long>(i), &vertex);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set vertex info");
        }
    }

    // StreamValue を設定して変更を反映
    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), m_streamH, &value);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set Shape Path stream value");
    }
}

void PyProperty::SetValue(py::object value) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (m_isEffectParam) {
        // Effect params: check for NO_DATA only (same as old EffectParam behavior)
        AEGP_StreamType streamType;
        A_Err err = suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);
        if (err != A_Err_NONE || streamType == AEGP_StreamType_NO_DATA) {
            throw std::runtime_error("Cannot set value on group parameter (NO_DATA stream)");
        }
    } else {
        // Normal property: require primitive type
        if (!CanSetValue()) {
            throw std::runtime_error("Cannot set value on non-primitive property");
        }
    }

    AEGP_StreamValue2 streamValue = PythonToStreamValue(value);

    A_Err err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), m_streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set property value");
    }
}

py::object PyProperty::GetValueAtTime(double time) const {
    return GetValue(time);
}

void PyProperty::SetValueAtTime(double time, py::object value) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    if (!CanSetValue()) {
        throw std::runtime_error("Cannot set value on non-primitive property");
    }

    // キーフレームがある場合は、指定時間でのキーフレームを作成/更新
    if (IsTimeVarying()) {
        AddKeyframe(time, value);
    } else {
        SetValue(value);
    }
}

// =============================================================
// キーフレーム情報
// =============================================================

int PyProperty::GetNumKeyframes() const {
    if (!m_streamH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long numKeys;
    A_Err err = suites.keyframeSuite->AEGP_GetStreamNumKFs(m_streamH, &numKeys);
    if (err != A_Err_NONE) return 0;

    // AEGP_NumKF_NO_DATA (-1) は0として扱う
    return (numKeys == AEGP_NumKF_NO_DATA) ? 0 : static_cast<int>(numKeys);
}

bool PyProperty::IsTimeVarying() const {
    return GetNumKeyframes() > 0;
}

// =============================================================
// キーフレーム操作
// =============================================================

int PyProperty::AddKeyframe(double time, py::object value) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    if (!CanHaveKeyframes()) {
        throw std::runtime_error("Property cannot have keyframes");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time timeT = AETypeUtils::SecondsToTime(time);

    A_long keyIndex;
    A_Err err = suites.keyframeSuite->AEGP_InsertKeyframe(
        m_streamH, AEGP_LTimeMode_CompTime, &timeT, &keyIndex);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to insert keyframe");
    }

    AEGP_StreamValue2 streamValue = PythonToStreamValue(value);
    err = suites.keyframeSuite->AEGP_SetKeyframeValue(
        m_streamH, keyIndex, &streamValue);
    if (err != A_Err_NONE) {
        PYAE_LOG_WARNING("Property", "Failed to set keyframe value");
    }

    return static_cast<int>(keyIndex);
}

void PyProperty::RemoveKeyframe(int index) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.keyframeSuite->AEGP_DeleteKeyframe(
        m_streamH, static_cast<A_long>(index));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to delete keyframe");
    }
}

void PyProperty::RemoveAllKeyframes() {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    int numKeys = GetNumKeyframes();
    for (int i = numKeys - 1; i >= 0; --i) {
        RemoveKeyframe(i);
    }
}

double PyProperty::GetKeyframeTime(int index) const {
    if (!m_streamH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time time;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeTime(
        m_streamH, static_cast<A_long>(index), AEGP_LTimeMode_CompTime, &time);
    if (err != A_Err_NONE || time.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(time);
}

py::object PyProperty::GetKeyframeValue(int index) const {
    if (!m_streamH) return py::none();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Check stream type - NO_DATA streams cannot have values
    AEGP_StreamType streamType;
    A_Err err = suites.streamSuite->AEGP_GetStreamType(m_streamH, &streamType);
    if (err != A_Err_NONE || streamType == AEGP_StreamType_NO_DATA) return py::none();

    A_Time time;
    err = suites.keyframeSuite->AEGP_GetKeyframeTime(
        m_streamH, static_cast<A_long>(index), AEGP_LTimeMode_CompTime, &time);
    if (err != A_Err_NONE) return py::none();

    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), m_streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) return py::none();

    py::object result = StreamValueToPython(value);
    suites.streamSuite->AEGP_DisposeStreamValue(&value);

    return result;
}

void PyProperty::SetKeyframeValue(int index, py::object value) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamValue2 streamValue = PythonToStreamValue(value);

    A_Err err = suites.keyframeSuite->AEGP_SetKeyframeValue(
        m_streamH, static_cast<A_long>(index), &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set keyframe value");
    }
}

std::pair<std::string, std::string> PyProperty::GetKeyframeInterpolation(int index) const {
    if (!m_streamH) return {"linear", "linear"};

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_KeyframeInterpolationType inType, outType;
    A_Err err = suites.keyframeSuite->AEGP_GetKeyframeInterpolation(
        m_streamH, static_cast<A_long>(index), &inType, &outType);
    if (err != A_Err_NONE) return {"linear", "linear"};

    auto interpToString = [](AEGP_KeyframeInterpolationType type) -> std::string {
        switch (type) {
            case AEGP_KeyInterp_LINEAR: return "linear";
            case AEGP_KeyInterp_BEZIER: return "bezier";
            case AEGP_KeyInterp_HOLD:   return "hold";
            default:                    return "linear";
        }
    };

    return {interpToString(inType), interpToString(outType)};
}

void PyProperty::SetKeyframeInterpolation(int index, const std::string& inType, const std::string& outType) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    auto stringToInterp = [](const std::string& str) -> AEGP_KeyframeInterpolationType {
        if (str == "linear") return AEGP_KeyInterp_LINEAR;
        if (str == "bezier") return AEGP_KeyInterp_BEZIER;
        if (str == "hold")   return AEGP_KeyInterp_HOLD;
        return AEGP_KeyInterp_LINEAR;
    };

    A_Err err = suites.keyframeSuite->AEGP_SetKeyframeInterpolation(
        m_streamH,
        static_cast<A_long>(index),
        stringToInterp(inType),
        stringToInterp(outType));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set keyframe interpolation");
    }
}

py::list PyProperty::GetKeyframes() const {
    py::list keyframes;

    int numKeys = GetNumKeyframes();
    for (int i = 0; i < numKeys; ++i) {
        py::dict keyframe;
        keyframe["index"] = i;
        keyframe["time"] = GetKeyframeTime(i);
        keyframe["value"] = GetKeyframeValue(i);

        auto interp = GetKeyframeInterpolation(i);
        keyframe["in_interpolation"] = interp.first;
        keyframe["out_interpolation"] = interp.second;

        keyframes.append(keyframe);
    }

    return keyframes;
}

// =============================================================
// エクスプレッション操作
// =============================================================

bool PyProperty::HasExpression() const {
    // 事前チェック: エクスプレッションを持てないプロパティは常にfalse
    if (!CanHaveExpression()) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean hasExpr = FALSE;
    A_Err err = suites.streamSuite->AEGP_GetExpressionState(
        state.GetPluginID(), m_streamH, &hasExpr);
    if (err != A_Err_NONE) return false;

    return hasExpr != FALSE;
}

std::string PyProperty::GetExpression() const {
    if (!CanHaveExpression()) {
        throw std::runtime_error("Property cannot have expressions");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_MemHandle exprH;
    A_Err err = suites.streamSuite->AEGP_GetExpression(
        state.GetPluginID(), m_streamH, &exprH);
    if (err != A_Err_NONE || !exprH) return "";

    ScopedMemHandle scopedExpr(state.GetPluginID(), suites.memorySuite, exprH);

    // ScopedMemLockを使用して例外安全にロック/アンロック
    ScopedMemLock lock(suites.memorySuite, exprH);
    A_UTF16Char* exprPtr = lock.As<A_UTF16Char>();
    if (!exprPtr) return "";

    std::string result = StringUtils::Utf16ToUtf8(exprPtr);

    return result;
}

void PyProperty::SetExpression(const std::string& expression) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    if (!CanHaveExpression()) {
        throw std::runtime_error("Property cannot have expressions");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    std::wstring wexpr = StringUtils::Utf8ToWide(expression);

    A_Err err = suites.streamSuite->AEGP_SetExpression(
        state.GetPluginID(), m_streamH,
        reinterpret_cast<const A_UTF16Char*>(wexpr.c_str()));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set expression");
    }
}

void PyProperty::EnableExpression(bool enable) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    if (!CanHaveExpression()) {
        throw std::runtime_error("Property cannot have expressions");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.streamSuite->AEGP_SetExpressionState(
        state.GetPluginID(), m_streamH, enable ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set expression state");
    }
}

// =============================================================
// 子プロパティアクセス
// =============================================================

int PyProperty::GetNumProperties() const {
    if (!IsGroup()) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.dynamicStreamSuite) return 0;

    A_long numStreams;
    A_Err err = suites.dynamicStreamSuite->AEGP_GetNumStreamsInGroup(m_streamH, &numStreams);
    if (err != A_Err_NONE) return 0;

    return static_cast<int>(numStreams);
}

std::shared_ptr<PyProperty> PyProperty::GetProperty(py::object key) const {
    if (!m_streamH) throw std::runtime_error("Invalid property");
    if (!IsGroup()) throw std::runtime_error("Property is not a group");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.dynamicStreamSuite) throw std::runtime_error("DynamicStreamSuite not available");

    AEGP_StreamRefH childStreamH = nullptr;
    A_Err err = A_Err_NONE;

    if (py::isinstance<py::int_>(key)) {
        int index = key.cast<int>();
        err = suites.dynamicStreamSuite->AEGP_GetNewStreamRefByIndex(
            state.GetPluginID(), m_streamH, index, &childStreamH);
    } else if (py::isinstance<py::str>(key)) {
        std::string matchName = key.cast<std::string>();

        // GetNewStreamRefByMatchname は Named Group にのみ使用可能
        // Indexed Group (マスク、エフェクト等) や Leaf には使用不可
        AEGP_StreamGroupingType groupType;
        A_Err typeErr = suites.dynamicStreamSuite->AEGP_GetStreamGroupingType(m_streamH, &groupType);
        if (typeErr == A_Err_NONE && groupType != AEGP_StreamGroupingType_NAMED_GROUP) {
            // Named Group でない場合は nullptr を返す（例外ではなく）
            return nullptr;
        }

        err = suites.dynamicStreamSuite->AEGP_GetNewStreamRefByMatchname(
            state.GetPluginID(), m_streamH, matchName.c_str(), &childStreamH);
    } else {
        throw std::runtime_error("Key must be int (index) or str (match name)");
    }

    if (err != A_Err_NONE || !childStreamH) {
        return nullptr;
    }

    return std::make_shared<PyProperty>(childStreamH, true);
}

std::vector<std::shared_ptr<PyProperty>> PyProperty::GetChildren() const {
    std::vector<std::shared_ptr<PyProperty>> children;

    if (IsGroup()) {
        int count = GetNumProperties();
        for (int i = 0; i < count; ++i) {
            auto p = GetProperty(py::int_(i));
            if (p) children.push_back(p);
        }
    }

    return children;
}

// =============================================================
// 動的プロパティ追加（INDEXED_GROUP用）
// =============================================================

bool PyProperty::CanAddStream(const std::string& match_name) const {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean can_add = FALSE;
    A_Err err = suites.dynamicStreamSuite->AEGP_CanAddStream(
        m_streamH,
        match_name.c_str(),
        &can_add);

    if (err != A_Err_NONE) {
        return false;
    }

    return can_add != 0;
}

std::shared_ptr<PyProperty> PyProperty::AddStream(const std::string& match_name) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid property");
    }

    // INDEXED_GROUP かチェック
    if (GetGroupingType() != StreamGroupingType::IndexedGroup) {
        throw std::runtime_error("AddStream is only valid for INDEXED_GROUP");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // 追加可能か確認
    A_Boolean can_add = FALSE;
    A_Err err = suites.dynamicStreamSuite->AEGP_CanAddStream(
        m_streamH,
        match_name.c_str(),
        &can_add);

    if (err != A_Err_NONE || !can_add) {
        throw std::runtime_error("Cannot add stream: " + match_name);
    }

    // ストリームを追加
    AEGP_StreamRefH new_stream = nullptr;
    err = suites.dynamicStreamSuite->AEGP_AddStream(
        state.GetPluginID(),
        m_streamH,
        match_name.c_str(),
        &new_stream);

    if (err != A_Err_NONE || !new_stream) {
        throw std::runtime_error("Failed to add stream: " + match_name);
    }

    // 新しいプロパティを作成（ハンドルを所有する）
    return std::make_shared<PyProperty>(new_stream, true);
}

// =============================================================
// Dynamic Stream フラグ操作
// =============================================================

int PyProperty::GetDynamicStreamFlags() const {
    if (!m_streamH) {
        throw std::runtime_error("Invalid stream handle");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.dynamicStreamSuite) {
        throw std::runtime_error("DynamicStreamSuite not available");
    }

    AEGP_DynStreamFlags flags = 0;
    A_Err err = suites.dynamicStreamSuite->AEGP_GetDynamicStreamFlags(
        m_streamH,
        &flags);

    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get dynamic stream flags");
    }

    return static_cast<int>(flags);
}

void PyProperty::SetDynamicStreamFlag(int flag, bool undoable, bool set_flag) {
    if (!m_streamH) {
        throw std::runtime_error("Invalid stream handle");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.dynamicStreamSuite) {
        throw std::runtime_error("DynamicStreamSuite not available");
    }

    A_Err err = suites.dynamicStreamSuite->AEGP_SetDynamicStreamFlag(
        m_streamH,
        static_cast<AEGP_DynStreamFlags>(flag),
        undoable ? TRUE : FALSE,
        set_flag ? TRUE : FALSE);

    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set dynamic stream flag");
    }
}

} // namespace PyAE

// =============================================================
// Pythonバインディング
// =============================================================

void init_property(py::module_& m) {
    using namespace PyAE;

    // StreamValueType 列挙型
    py::enum_<StreamValueType>(m, "StreamValueType")
        .value("NoData", StreamValueType::NoData)
        .value("OneDim", StreamValueType::OneDim)
        .value("TwoDim", StreamValueType::TwoDim)
        .value("TwoDimSpatial", StreamValueType::TwoDimSpatial)
        .value("ThreeDim", StreamValueType::ThreeDim)
        .value("ThreeDimSpatial", StreamValueType::ThreeDimSpatial)
        .value("Color", StreamValueType::Color)
        .value("Arbitrary", StreamValueType::Arbitrary)
        .value("Marker", StreamValueType::Marker)
        .value("LayerID", StreamValueType::LayerID)
        .value("MaskID", StreamValueType::MaskID)
        .value("Mask", StreamValueType::Mask)
        .value("TextDocument", StreamValueType::TextDocument);

    // StreamGroupingType 列挙型
    py::enum_<StreamGroupingType>(m, "StreamGroupingType")
        .value("Invalid", StreamGroupingType::Invalid)
        .value("Leaf", StreamGroupingType::Leaf)
        .value("NamedGroup", StreamGroupingType::NamedGroup)
        .value("IndexedGroup", StreamGroupingType::IndexedGroup);

    // AEGP_DynStreamFlags 定数（レイヤースタイルの有効/無効など）
    m.attr("DynStreamFlag_NONE") = py::int_(0x00);
    m.attr("DynStreamFlag_ACTIVE_EYEBALL") = py::int_(0x01);
    m.attr("DynStreamFlag_HIDDEN") = py::int_(0x02);
    m.attr("DynStreamFlag_DISABLED") = py::int_(0x04);
    m.attr("DynStreamFlag_ELIDED") = py::int_(0x08);
    m.attr("DynStreamFlag_SHOWN_WHEN_EMPTY") = py::int_(0x10);
    m.attr("DynStreamFlag_SKIP_REVEAL_WHEN_UNHIDDEN") = py::int_(0x20);

    // PropertyType 列挙型（後方互換）
    py::enum_<PropertyType>(m, "PropertyType")
        .value("None_", PropertyType::None)
        .value("OneDim", PropertyType::OneDim)
        .value("TwoDim", PropertyType::TwoDim)
        .value("ThreeDim", PropertyType::ThreeDim)
        .value("Color", PropertyType::Color)
        .value("Arbitrary", PropertyType::Arbitrary)
        .value("NoValue", PropertyType::NoValue);

    // Property クラス
    py::class_<PyProperty, std::shared_ptr<PyProperty>>(m, "Property")
        .def(py::init<>())
        // 基本情報
        .def_property_readonly("valid", &PyProperty::IsValid,
                              "Check if property is valid")
        .def_property_readonly("name", &PyProperty::GetName,
                              "Property name")
        .def("set_name", &PyProperty::SetName,
             "Set property name (for IndexedGroup children like shape groups)",
             py::arg("name"))
        .def_property_readonly("match_name", &PyProperty::GetMatchName,
                              "Property match name")
        .def_property_readonly("depth", &PyProperty::GetDepth,
                              "Property depth")
        // 型情報
        .def_property_readonly("stream_value_type", &PyProperty::GetStreamValueType,
                              "SDK stream value type")
        .def_property_readonly("grouping_type", &PyProperty::GetGroupingType,
                              "SDK stream grouping type")
        .def_property_readonly("type", &PyProperty::GetType,
                              "Legacy property type (for backward compatibility)")
        .def_property_readonly("is_group", &PyProperty::IsGroup,
                              "Check if property is a group")
        .def_property_readonly("is_leaf", &PyProperty::IsLeaf,
                              "Check if property is a leaf")
        // 能力判定
        .def_property_readonly("can_vary_over_time", &PyProperty::CanVaryOverTime,
                              "Check if property can vary over time (SDK API)")
        .def_property_readonly("can_have_keyframes", &PyProperty::CanHaveKeyframes,
                              "Check if property can have keyframes")
        .def_property_readonly("can_have_expression", &PyProperty::CanHaveExpression,
                              "Check if property can have expressions")
        .def_property_readonly("can_set_value", &PyProperty::CanSetValue,
                              "Check if property value can be set directly")
        .def_property_readonly("is_primitive", &PyProperty::IsPrimitive,
                              "Check if property is a primitive type")
        .def_property_readonly("is_effect_param", &PyProperty::IsEffectParam,
                              "Check if this property is an effect parameter")
        .def_property_readonly("index", &PyProperty::GetEffectParamIndex,
                              "Effect parameter index (0-based, -1 if not an effect parameter)")
        // 値操作
        .def_property("value",
                     [](const PyProperty& self) { return self.GetValue(0.0); },
                     &PyProperty::SetValue,
                     "Property value at time 0")
        .def("get_value", &PyProperty::GetValue,
             "Get property value at specified time",
             py::arg("time") = 0.0)
        .def("set_value", &PyProperty::SetValue,
             "Set property value (affects all time if no keyframes)",
             py::arg("value"))
        .def("get_value_at_time", &PyProperty::GetValueAtTime,
             "Get property value at specified time",
             py::arg("time"))
        .def("set_value_at_time", &PyProperty::SetValueAtTime,
             "Set property value at specified time (creates keyframe if animated)",
             py::arg("time"), py::arg("value"))
        .def("get_raw_bytes", &PyProperty::GetRawBytes,
             "Get raw binary data (bdata format) at specified time",
             py::arg("time") = 0.0)
        .def("get_stream_metadata", &PyProperty::GetStreamMetadata,
             "Get stream metadata (flags, min/max values, types)")
        // キーフレーム
        .def_property_readonly("num_keyframes", &PyProperty::GetNumKeyframes,
                              "Number of keyframes")
        .def_property_readonly("is_time_varying", &PyProperty::IsTimeVarying,
                              "Check if property has keyframes")
        .def("add_keyframe", &PyProperty::AddKeyframe,
             "Add a keyframe at specified time with given value",
             py::arg("time"), py::arg("value"))
        .def("remove_keyframe", &PyProperty::RemoveKeyframe,
             "Remove keyframe at specified index",
             py::arg("index"))
        .def("remove_all_keyframes", &PyProperty::RemoveAllKeyframes,
             "Remove all keyframes")
        .def("get_keyframe_time", &PyProperty::GetKeyframeTime,
             "Get time of keyframe at index",
             py::arg("index"))
        .def("keyframe_time", &PyProperty::GetKeyframeTime,
             "Get time of keyframe at index (alias)",
             py::arg("index"))
        .def("get_keyframe_value", &PyProperty::GetKeyframeValue,
             "Get value of keyframe at index",
             py::arg("index"))
        .def("set_keyframe_value", &PyProperty::SetKeyframeValue,
             "Set value of keyframe at index",
             py::arg("index"), py::arg("value"))
        .def("get_keyframe_interpolation", &PyProperty::GetKeyframeInterpolation,
             "Get interpolation type (in, out) of keyframe at index",
             py::arg("index"))
        .def("keyframe_in_interpolation", [](const PyProperty& self, int index) {
                 return self.GetKeyframeInterpolation(index).first;
             },
             "Get in interpolation type of keyframe at index",
             py::arg("index"))
        .def("keyframe_out_interpolation", [](const PyProperty& self, int index) {
                 return self.GetKeyframeInterpolation(index).second;
             },
             "Get out interpolation type of keyframe at index",
             py::arg("index"))
        .def("set_keyframe_interpolation", &PyProperty::SetKeyframeInterpolation,
             "Set interpolation type of keyframe at index",
             py::arg("index"), py::arg("in_type"), py::arg("out_type"))
        .def("set_temporal_ease", [](PyProperty& self, int index,
                                     const std::vector<double>& ease_in,
                                     const std::vector<double>& ease_out) {
                 // Validate inputs
                 if (ease_in.size() != 2 || ease_out.size() != 2) {
                     throw std::runtime_error("Ease must be [speed, influence]");
                 }

                 // Get the keyframe for this index
                 auto keyframes = self.GetKeyframes();
                 if (index < 0 || index >= static_cast<int>(keyframes.size())) {
                     throw std::runtime_error("Keyframe index out of range");
                 }

                 auto& state = PluginState::Instance();
                 const auto& suites = state.GetSuites();
                 AEGP_StreamRefH streamH = self.GetHandle();

                 // Determine which interpolation directions need bezier based on ease values
                 // Only set bezier if influence > 0 (ease is applied)
                 // If influence is 0, preserve current interpolation type
                 constexpr double INFLUENCE_THRESHOLD = 0.001;
                 bool apply_ease_in = ease_in[1] > INFLUENCE_THRESHOLD;
                 bool apply_ease_out = ease_out[1] > INFLUENCE_THRESHOLD;

                 // Get current interpolation types to preserve non-eased directions
                 std::pair<std::string, std::string> current_interp;
                 try {
                     current_interp = self.GetKeyframeInterpolation(index);
                 } catch (...) {
                     // If getting current interpolation fails, default to linear
                     current_interp = {"linear", "linear"};
                 }

                 std::string in_type = apply_ease_in ? "bezier" : current_interp.first;
                 std::string out_type = apply_ease_out ? "bezier" : current_interp.second;

                 // Set interpolation types
                 self.SetKeyframeInterpolation(index, in_type, out_type);

                 // Set temporal ease using AEGP API
                 // AEGP API expects influence in range 0-1 (not 0-100%)
                 AEGP_KeyframeEase keyEaseIn, keyEaseOut;
                 keyEaseIn.speedF = static_cast<A_FpLong>(ease_in[0]);
                 keyEaseIn.influenceF = static_cast<A_FpLong>(ease_in[1]);
                 keyEaseOut.speedF = static_cast<A_FpLong>(ease_out[0]);
                 keyEaseOut.influenceF = static_cast<A_FpLong>(ease_out[1]);

                 // Temporal ease applies to the entire keyframe, not per-dimension
                 // Always use dimension=0 regardless of stream type (Position, Scale, etc.)
                 A_Err err = suites.keyframeSuite->AEGP_SetKeyframeTemporalEase(
                     streamH, index, 0, &keyEaseIn, &keyEaseOut);
                 if (err != A_Err_NONE) {
                     char errorMsg[256];
                     snprintf(errorMsg, sizeof(errorMsg), "Failed to set temporal ease (error code: %d)", static_cast<int>(err));
                     throw std::runtime_error(errorMsg);
                 }
             },
             "Set temporal ease for keyframe at index",
             py::arg("index"), py::arg("ease_in"), py::arg("ease_out"))
        .def_property_readonly("keyframes", [](PyProperty& self) -> py::list {
            py::list result;
            int numKeys = self.GetNumKeyframes();
            AEGP_StreamRefH streamH = self.GetHandle();
            for (int i = 0; i < numKeys; ++i) {
                result.append(PyAE::PyKeyframe(streamH, static_cast<A_long>(i)));
            }
            return result;
        }, "List of all keyframes as Keyframe objects")
        // エクスプレッション
        .def_property_readonly("has_expression", &PyProperty::HasExpression,
                              "Check if property has an expression (safe)")
        .def_property("expression", &PyProperty::GetExpression, &PyProperty::SetExpression,
                     "Property expression")
        .def("enable_expression", &PyProperty::EnableExpression,
             "Enable or disable expression",
             py::arg("enable"))
        // 子プロパティ
        .def_property_readonly("num_properties", &PyProperty::GetNumProperties,
                              "Number of child properties")
        .def("property", &PyProperty::GetProperty,
             "Get child property by index or match name",
             py::arg("key"))
        .def("__getitem__", [](const PyProperty& self, py::object key) {
            auto prop = self.GetProperty(key);
            if (!prop) throw py::index_error("Property not found");
            return prop;
        })
        .def("__iter__", [](const PyProperty& self) {
            auto children = self.GetChildren();
            py::list props;
            for (const auto& child : children) {
                props.append(child);
            }
            return props.attr("__iter__")();
        })
        .def_property_readonly("properties", [](const PyProperty& self) {
            auto children = self.GetChildren();
            py::list props;
            for (const auto& child : children) {
                props.append(child);
            }
            return props;
        }, "List of child properties")
        // Python protocol methods
        .def("__bool__", &PyProperty::IsValid)
        .def("__len__", &PyProperty::GetNumProperties)
        .def("__contains__", [](const PyProperty& self, const std::string& name) -> bool {
            try {
                auto prop = self.GetProperty(py::cast(name));
                return static_cast<bool>(prop);
            } catch (...) {
                return false;
            }
        })
        // TextDocument専用メソッド
        .def("set_source_text", [](PyProperty& self, const std::string& text, double time) {
            if (self.GetStreamValueType() != StreamValueType::TextDocument) {
                throw std::runtime_error("Property is not a TextDocument");
            }
            self.SetTextDocumentValue(time, text);
        }, "Set source text for TextDocument property",
           py::arg("text"), py::arg("time") = 0.0)
        // Shape Path専用メソッド
        .def("set_shape_path_vertices", &PyProperty::SetShapePathVertices,
            "Set vertices for Shape Path (ADBE Vector Shape - Group) property",
            py::arg("vertices"), py::arg("time") = 0.0)
        // 動的プロパティ追加（INDEXED_GROUP用）
        .def("can_add_stream", &PyProperty::CanAddStream,
            "Check if a new stream can be added (for INDEXED_GROUP)",
            py::arg("match_name"))
        .def("add_stream", &PyProperty::AddStream,
            "Add a new stream to INDEXED_GROUP (e.g., add shape to Shape Layer)",
            py::arg("match_name"))
        // Dynamic Stream フラグ操作
        .def("get_dynamic_stream_flags", &PyProperty::GetDynamicStreamFlags,
            "Get dynamic stream flags (e.g., for Layer Styles enabled/disabled state)")
        .def("set_dynamic_stream_flag", &PyProperty::SetDynamicStreamFlag,
            "Set dynamic stream flag (e.g., enable/disable Layer Style effect)",
            py::arg("flag"), py::arg("undoable") = true, py::arg("set_flag") = true)
        .def_property_readonly("_handle", [](const PyProperty& self) -> uintptr_t {
             AEGP_StreamRefH streamH = self.GetHandle();
             if (!streamH) return 0;
             return reinterpret_cast<uintptr_t>(streamH);
        }, "Internal: Stream handle (to AEGP_StreamRefH)")
        .def("__repr__", [](const PyProperty& self) {
            if (!self.IsValid()) {
                return std::string("<Property: invalid>");
            }
            std::string name = self.GetName();
            std::string typeName = PropertyTypeUtils::StreamValueTypeName(self.GetStreamValueType());
            return std::string("<Property: '") + name + "' type=" + typeName + ">";
        })

        // Serialization methods
        .def("to_dict", [](PyProperty& self) -> py::dict {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                return serialize.attr("property_to_dict")(self).cast<py::dict>();
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to serialize property: ") + e.what());
            }
        }, "Export property to dictionary")

        .def("update_from_dict", [](PyProperty& self, const py::dict& data) {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                serialize.attr("property_update_from_dict")(self, data);
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to update property from dict: ") + e.what());
            }
        }, "Update property from dictionary",
           py::arg("data"));

    // =============================================================
    // LayerからプロパティAPIを追加
    // =============================================================
    m.def("_get_layer_property", [](uintptr_t layerHandle, const std::string& name) -> std::shared_ptr<PyProperty> {
        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
        if (!layerH) {
            throw std::runtime_error("Invalid layer handle");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.streamSuite) {
            throw std::runtime_error("StreamSuite not available");
        }

        auto streamOpt = AETypeUtils::GetLayerStreamFromName(name);
        if (!streamOpt.has_value()) {
            throw std::runtime_error("Unknown property name: " + name);
        }

        AEGP_LayerStream streamType = streamOpt.value();

        AEGP_StreamRefH streamH = nullptr;
        A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
            state.GetPluginID(), layerH, streamType, &streamH);

        if (err != A_Err_NONE || !streamH) {
            throw std::runtime_error("Failed to get property stream: " + name);
        }

        return std::make_shared<PyProperty>(streamH, true);
    }, "Internal function to get layer property by name",
    py::arg("layer_handle"), py::arg("name"));

    // =============================================================
    // Layer クラスに property/properties を追加
    // =============================================================
    try {
        py::object layer_class = m.attr("Layer");

        // メソッドバインディング形式で追加（is_method=trueを指定）
        auto property_method = py::cpp_function(
            [](py::object self, const std::string& name) -> std::shared_ptr<PyProperty> {
                uintptr_t layerHandle = self.attr("_handle").cast<uintptr_t>();
                AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);

                if (!layerH) {
                    throw std::runtime_error("Invalid layer handle");
                }

                auto& state = PluginState::Instance();
                const auto& suites = state.GetSuites();

                if (!suites.streamSuite) {
                    throw std::runtime_error("StreamSuite not available");
                }

                auto streamOpt = AETypeUtils::GetLayerStreamFromName(name);
                if (!streamOpt.has_value()) {
                    throw std::runtime_error("Unknown property name: " + name);
                }

                AEGP_LayerStream streamType = streamOpt.value();

                AEGP_StreamRefH streamH = nullptr;
                A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
                    state.GetPluginID(), layerH, streamType, &streamH);

                if (err != A_Err_NONE || !streamH) {
                    throw std::runtime_error("Failed to get property stream: " + name);
                }

                return std::make_shared<PyProperty>(streamH, true);
            },
            py::arg("name"),
            py::is_method(layer_class),
            "Get a property by name (e.g., 'Position', 'Scale', 'Opacity')"
        );

        py::setattr(layer_class, "property", property_method);

        auto properties_prop = py::cpp_function(
            [](py::object self) -> std::shared_ptr<PyProperty> {
                uintptr_t layerHandle = self.attr("_handle").cast<uintptr_t>();
                AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);

                if (!layerH) {
                    throw std::runtime_error("Invalid layer handle");
                }

                auto& state = PluginState::Instance();
                const auto& suites = state.GetSuites();

                if (!suites.dynamicStreamSuite) {
                    throw std::runtime_error("DynamicStreamSuite not available");
                }

                AEGP_StreamRefH streamH = nullptr;
                A_Err err = suites.dynamicStreamSuite->AEGP_GetNewStreamRefForLayer(
                    state.GetPluginID(), layerH, &streamH);

                if (err != A_Err_NONE || !streamH) {
                    throw std::runtime_error("Failed to get layer root property stream");
                }

                return std::make_shared<PyProperty>(streamH, true);
            },
            py::is_method(layer_class),
            "Get the root property group of the layer"
        );

        py::object property_decorator = py::module_::import("builtins").attr("property");
        py::setattr(layer_class, "properties", property_decorator(properties_prop));

    } catch (const std::exception&) {
        PYAE_LOG_WARNING("Property", "Failed to add property/properties to Layer class");
    }

    // Note: _LayerRef class has been removed (Phase 2)
    // Properties are now accessed via Layer.properties
}
