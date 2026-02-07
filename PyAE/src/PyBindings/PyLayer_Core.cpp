// PyLayer_Core.cpp
// PyAE - Python for After Effects
// PyLayer 基本実装
//
// PYAE-008b: PyLayer.cppから分割

#include "PyLayerClasses.h"

namespace PyAE {

// =============================================================
// PyLayer 基本実装
// =============================================================

PyLayer::PyLayer() : m_layerH(nullptr) {}

PyLayer::PyLayer(AEGP_LayerH layerH) : m_layerH(layerH) {}

bool PyLayer::IsValid() const {
    return m_layerH != nullptr;
}

// ==========================================
// 名前
// ==========================================
std::string PyLayer::GetName() const {
    if (!m_layerH) return "";

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_MemHandle layerNameH = nullptr;
    AEGP_MemHandle sourceNameH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetLayerName(
        state.GetPluginID(), m_layerH, &layerNameH, &sourceNameH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerName failed");
    }

    std::string result;

    // Try layer name first
    if (layerNameH) {
        ScopedMemHandle scopedLayerName(state.GetPluginID(), suites.memorySuite, layerNameH);
        ScopedMemLock lock(suites.memorySuite, layerNameH);
        A_UTF16Char* namePtr = lock.As<A_UTF16Char>();
        if (namePtr && namePtr[0] != 0) {
            result = StringUtils::Utf16ToUtf8(namePtr);
        }
    }

    // Fall back to source name if layer name is empty
    if (result.empty() && sourceNameH) {
        ScopedMemHandle scopedSourceName(state.GetPluginID(), suites.memorySuite, sourceNameH);
        ScopedMemLock lock(suites.memorySuite, sourceNameH);
        A_UTF16Char* sourcePtr = lock.As<A_UTF16Char>();
        if (sourcePtr && sourcePtr[0] != 0) {
            result = StringUtils::Utf16ToUtf8(sourcePtr);
        }
    }

    return result;
}

void PyLayer::SetName(const std::string& name) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    std::wstring wname = StringUtils::Utf8ToWide(name);

    A_Err err = suites.layerSuite->AEGP_SetLayerName(
        m_layerH, reinterpret_cast<const A_UTF16Char*>(wname.c_str()));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer name");
    }
}

// ==========================================
// コメント
// ==========================================
std::string PyLayer::GetComment() const {
    if (!m_layerH) return "";

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.layerSuite || !suites.itemSuite || !suites.memorySuite) {
        throw std::runtime_error("Required suites not available");
    }

    // Get layer source item
    AEGP_ItemH sourceItemH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetLayerSourceItem(m_layerH, &sourceItemH);
    if (err != A_Err_NONE || !sourceItemH) {
        throw std::runtime_error("AEGP_GetLayerSourceItem failed");
    }

    // Get source item comment
    AEGP_MemHandle commentH;
    err = suites.itemSuite->AEGP_GetItemComment(sourceItemH, &commentH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetItemComment failed");
    }

    ScopedMemHandle scopedComment(state.GetPluginID(), suites.memorySuite, commentH);

    // ScopedMemLockを使用して例外安全にロック/アンロック
    ScopedMemLock lock(suites.memorySuite, commentH);
    A_UTF16Char* commentPtr = lock.As<A_UTF16Char>();
    if (!commentPtr) return "";

    std::string result = StringUtils::Utf16ToUtf8(commentPtr);
    // lockのデストラクタで自動的にUnlock

    return result;
}

void PyLayer::SetComment(const std::string& comment) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.layerSuite || !suites.itemSuite) {
        throw std::runtime_error("Required suites not available");
    }

    // レイヤーのソースアイテムを取得
    AEGP_ItemH sourceItemH = nullptr;
    A_Err err = suites.layerSuite->AEGP_GetLayerSourceItem(m_layerH, &sourceItemH);
    if (err != A_Err_NONE || !sourceItemH) {
        throw std::runtime_error("Failed to get layer source item");
    }

    // ソースアイテムにコメントを設定
    std::wstring wcomment = StringUtils::Utf8ToWide(comment);
    err = suites.itemSuite->AEGP_SetItemComment(
        sourceItemH, reinterpret_cast<const A_UTF16Char*>(wcomment.c_str()));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set item comment");
    }
}


// ==========================================
// 品質・ラベル
// ==========================================
int PyLayer::GetQuality() const {
    if (!m_layerH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerQuality quality = AEGP_LayerQual_NONE;
    A_Err err = suites.layerSuite->AEGP_GetLayerQuality(m_layerH, &quality);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerQuality failed");
    }

    return static_cast<int>(quality);
}

void PyLayer::SetQuality(int quality) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerQuality(
        m_layerH, static_cast<AEGP_LayerQuality>(quality));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer quality");
    }
}

int PyLayer::GetSamplingQuality() const {
    if (!m_layerH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerSamplingQuality sampling_quality = AEGP_LayerSamplingQual_BILINEAR;
    A_Err err = suites.layerSuite->AEGP_GetLayerSamplingQuality(m_layerH, &sampling_quality);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerSamplingQuality failed");
    }

    return static_cast<int>(sampling_quality);
}

void PyLayer::SetSamplingQuality(int sampling_quality) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerSamplingQuality(
        m_layerH, static_cast<AEGP_LayerSamplingQuality>(sampling_quality));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer sampling quality");
    }
}

int PyLayer::GetLabel() const {
    if (!m_layerH) return -1;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LabelID label = AEGP_Label_NONE;
    A_Err err = suites.layerSuite->AEGP_GetLayerLabel(m_layerH, &label);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerLabel failed");
    }

    return static_cast<int>(label);
}

void PyLayer::SetLabel(int label) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerLabel(
        m_layerH, static_cast<AEGP_LabelID>(label));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer label");
    }
}

// ==========================================
// インデックス
// ==========================================
int PyLayer::GetIndex() const {
    if (!m_layerH) return -1;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long index;
    A_Err err = suites.layerSuite->AEGP_GetLayerIndex(m_layerH, &index);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerIndex failed");
    }

    return static_cast<int>(index);
}

void PyLayer::SetIndex(int index) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get parent comp and layer count for bounds checking
    AEGP_CompH compH = nullptr;
    if (suites.layerSuite->AEGP_GetLayerParentComp(m_layerH, &compH) != A_Err_NONE || !compH) {
        throw std::runtime_error("Failed to get parent composition");
    }

    A_long numLayers = 0;
    if (suites.layerSuite->AEGP_GetCompNumLayers(compH, &numLayers) != A_Err_NONE) {
        throw std::runtime_error("Failed to get layer count");
    }

    // Python-side bounds checking (0-based indexing)
    if (index < 0 || index >= numLayers) {
        throw std::out_of_range("Layer index out of range: " + std::to_string(index) +
                                " (valid range: 0-" + std::to_string(numLayers - 1) + ")");
    }

    // AEGP_ReorderLayer expects 0-based index
    A_Err err = suites.layerSuite->AEGP_ReorderLayer(m_layerH, static_cast<A_long>(index));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set layer index");
    }
}

// ==========================================
// 操作
// ==========================================
void PyLayer::Delete() {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_DeleteLayer(m_layerH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to delete layer");
    }

    m_layerH = nullptr;
}

PyLayer PyLayer::Duplicate() {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerH newLayerH;
    A_Err err = suites.layerSuite->AEGP_DuplicateLayer(m_layerH, &newLayerH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to duplicate layer");
    }

    return PyLayer(newLayerH);
}

void PyLayer::MoveTo(int index) {
    SetIndex(index);
}

// ==========================================
// メタデータ取得
// ==========================================
py::dict PyLayer::GetLayerMetadata() const {
    py::dict metadata;

    if (!m_layerH) {
        return metadata;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // レイヤーフラグ
    AEGP_LayerFlags flags = 0;
    A_Err err = suites.layerSuite->AEGP_GetLayerFlags(m_layerH, &flags);
    if (err == A_Err_NONE) {
        metadata["flags"] = static_cast<int>(flags);
    }

    // オブジェクトタイプ
    AEGP_ObjectType objType;
    err = suites.layerSuite->AEGP_GetLayerObjectType(m_layerH, &objType);
    if (err == A_Err_NONE) {
        metadata["object_type"] = static_cast<int>(objType);
    }

    // 品質設定
    AEGP_LayerQuality quality;
    err = suites.layerSuite->AEGP_GetLayerQuality(m_layerH, &quality);
    if (err == A_Err_NONE) {
        metadata["quality"] = static_cast<int>(quality);
    }

    // サンプリング品質
    AEGP_LayerSamplingQuality samplingQuality;
    err = suites.layerSuite->AEGP_GetLayerSamplingQuality(m_layerH, &samplingQuality);
    if (err == A_Err_NONE) {
        metadata["sampling_quality"] = static_cast<int>(samplingQuality);
    }

    // 転送モード（ブレンドモード）
    AEGP_LayerTransferMode transferMode;
    err = suites.layerSuite->AEGP_GetLayerTransferMode(m_layerH, &transferMode);
    if (err == A_Err_NONE) {
        metadata["transfer_mode"] = static_cast<int>(transferMode.mode);
        metadata["transfer_flags"] = static_cast<int>(transferMode.flags);
        metadata["transfer_track_matte"] = static_cast<int>(transferMode.track_matte);
    }

    // 時間ストレッチ
    // Note: AEGP_GetLayerTimeStretch is not available in AE 2025 SDK
    // A_Ratio timeStretch;
    // err = suites.layerSuite->AEGP_GetLayerTimeStretch(m_layerH, &timeStretch);
    // if (err == A_Err_NONE) {
    //     metadata["time_stretch_num"] = static_cast<int>(timeStretch.num);
    //     metadata["time_stretch_den"] = static_cast<int>(timeStretch.den);
    // }

    // レイヤーのラベルカラー
    AEGP_LabelID label;
    err = suites.layerSuite->AEGP_GetLayerLabel(m_layerH, &label);
    if (err == A_Err_NONE) {
        metadata["label"] = static_cast<int>(label);
    }

    // ソースアイテムID
    AEGP_ItemH sourceItemH;
    err = suites.layerSuite->AEGP_GetLayerSourceItem(m_layerH, &sourceItemH);
    if (err == A_Err_NONE && sourceItemH) {
        A_long itemID;
        err = suites.itemSuite->AEGP_GetItemID(sourceItemH, &itemID);
        if (err == A_Err_NONE) {
            metadata["source_item_id"] = static_cast<int>(itemID);
        }
    }

    return metadata;
}

// ==========================================
// ID関連
// ==========================================
int PyLayer::GetLayerID() const {
    if (!m_layerH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerIDVal id = 0;
    A_Err err = suites.layerSuite->AEGP_GetLayerID(m_layerH, &id);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerID failed");
    }

    return static_cast<int>(id);
}

int PyLayer::GetSourceItemID() const {
    if (!m_layerH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long sourceItemID = 0;
    A_Err err = suites.layerSuite->AEGP_GetLayerSourceItemID(m_layerH, &sourceItemID);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerSourceItemID failed");
    }

    return static_cast<int>(sourceItemID);
}

int PyLayer::GetObjectType() const {
    if (!m_layerH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_ObjectType objType;
    A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(m_layerH, &objType);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetLayerObjectType failed");
    }

    return static_cast<int>(objType);
}

bool PyLayer::Is2D() const {
    if (!m_layerH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean is_2d = FALSE;
    A_Err err = suites.layerSuite->AEGP_IsLayer2D(m_layerH, &is_2d);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_IsLayer2D failed");
    }

    return is_2d != FALSE;
}

// ==========================================
// ハンドルアクセス
// ==========================================
AEGP_LayerH PyLayer::GetHandle() const {
    return m_layerH;
}

} // namespace PyAE
