// PyEffect.cpp
// PyAE - Python for After Effects
// エフェクト操作APIバインディング (Phase 5)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PluginState.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "Logger.h"
#include "PyStreamUtils.h"
#include "PyPropertyCore.h"
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {

// 前方宣言
class PyEffect;

// =============================================================
// PyEffect - エフェクトクラス
// =============================================================
class PyEffect {
public:
    PyEffect() : m_effectH(nullptr), m_layerH(nullptr), m_ownsHandle(false), m_cachedIndex(-1) {}

    explicit PyEffect(AEGP_EffectRefH effectH, bool ownsHandle = true, AEGP_LayerH layerH = nullptr, int index = -1)
        : m_effectH(effectH)
        , m_layerH(layerH)
        , m_ownsHandle(ownsHandle)
        , m_cachedIndex(index)
    {}

    ~PyEffect() {
        if (m_ownsHandle && m_effectH) {
            auto& state = PluginState::Instance();
            const auto& suites = state.GetSuites();
            if (suites.effectSuite) {
                suites.effectSuite->AEGP_DisposeEffect(m_effectH);
            }
            m_effectH = nullptr;
        }
    }

    // コピー禁止、ムーブのみ
    PyEffect(const PyEffect&) = delete;
    PyEffect& operator=(const PyEffect&) = delete;

    PyEffect(PyEffect&& other) noexcept
        : m_effectH(other.m_effectH)
        , m_layerH(other.m_layerH)
        , m_ownsHandle(other.m_ownsHandle)
        , m_cachedIndex(other.m_cachedIndex)
    {
        other.m_effectH = nullptr;
        other.m_layerH = nullptr;
        other.m_ownsHandle = false;
        other.m_cachedIndex = -1;
    }

    PyEffect& operator=(PyEffect&& other) noexcept {
        if (this != &other) {
            if (m_ownsHandle && m_effectH) {
                auto& state = PluginState::Instance();
                const auto& suites = state.GetSuites();
                if (suites.effectSuite) {
                    suites.effectSuite->AEGP_DisposeEffect(m_effectH);
                }
            }
            m_effectH = other.m_effectH;
            m_layerH = other.m_layerH;
            m_ownsHandle = other.m_ownsHandle;
            m_cachedIndex = other.m_cachedIndex;
            other.m_effectH = nullptr;
            other.m_layerH = nullptr;
            other.m_ownsHandle = false;
            other.m_cachedIndex = -1;
        }
        return *this;
    }

    bool IsValid() const { return m_effectH != nullptr; }

    // ==========================================
    // 基本情報
    // ==========================================
    std::string GetName() const {
        if (!m_effectH) return "";

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // まずエフェクトRefからInstalledEffectKeyを取得
        AEGP_InstalledEffectKey effectKey;
        A_Err err = suites.effectSuite->AEGP_GetInstalledKeyFromLayerEffect(m_effectH, &effectKey);
        if (err != A_Err_NONE) return "";

        // キーを使って名前を取得
        A_char name[AEGP_MAX_EFFECT_NAME_SIZE] = {0};
        err = suites.effectSuite->AEGP_GetEffectName(effectKey, name);
        if (err != A_Err_NONE) return "";

        // ローカルエンコーディング（Shift-JIS等）からUTF-8に変換
        return StringUtils::LocalToUtf8(name);
    }

    std::string GetMatchName() const {
        if (!m_effectH) return "";

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // まずエフェクトRefからInstalledEffectKeyを取得
        AEGP_InstalledEffectKey effectKey;
        A_Err err = suites.effectSuite->AEGP_GetInstalledKeyFromLayerEffect(m_effectH, &effectKey);
        if (err != A_Err_NONE) return "";

        // キーを使ってマッチ名を取得
        A_char matchName[AEGP_MAX_EFFECT_MATCH_NAME_SIZE] = {0};
        err = suites.effectSuite->AEGP_GetEffectMatchName(effectKey, matchName);
        if (err != A_Err_NONE) return "";

        // マッチ名は通常ASCII（例: "ADBE Gaussian Blur 2"）だが念のため変換
        return StringUtils::LocalToUtf8(matchName);
    }

    int GetIndex() const {
        // Use cached index if available
        if (m_cachedIndex >= 0) {
            return m_cachedIndex;
        }

        // Fallback: try to find index by comparing handles
        // Note: This may not work reliably as AEGP API returns new handles each time
        if (!m_effectH || !m_layerH) return -1;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_long numEffects = 0;
        A_Err err = suites.effectSuite->AEGP_GetLayerNumEffects(m_layerH, &numEffects);
        if (err != A_Err_NONE) return -1;

        for (A_long i = 0; i < numEffects; ++i) {
            AEGP_EffectRefH effectH = nullptr;
            err = suites.effectSuite->AEGP_GetLayerEffectByIndex(
                state.GetPluginID(), m_layerH, i, &effectH);
            if (err == A_Err_NONE && effectH) {
                // Compare effect handles by pointer value
                bool isSame = (effectH == m_effectH);
                suites.effectSuite->AEGP_DisposeEffect(effectH);

                if (isSame) {
                    return static_cast<int>(i);  // 0-based index
                }
            }
        }

        return -1;  // Not found
    }

    // ==========================================
    // 有効/無効
    // ==========================================
    bool GetEnabled() const {
        if (!m_effectH) return false;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        AEGP_EffectFlags flags;
        A_Err err = suites.effectSuite->AEGP_GetEffectFlags(m_effectH, &flags);
        if (err != A_Err_NONE) return false;

        // AEGP_EffectFlags_ACTIVE ビットをチェック
        return (flags & AEGP_EffectFlags_ACTIVE) != 0;
    }

    void SetEnabled(bool enabled) {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // AEGP_EffectFlags_ACTIVE ビットをマスクとして使用
        AEGP_EffectFlags mask = AEGP_EffectFlags_ACTIVE;
        AEGP_EffectFlags flags = enabled ? AEGP_EffectFlags_ACTIVE : AEGP_EffectFlags_NONE;

        A_Err err = suites.effectSuite->AEGP_SetEffectFlags(m_effectH, mask, flags);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set effect enabled state");
        }
    }

    // ==========================================
    // パラメータアクセス
    // ==========================================
    int GetNumParams() const {
        if (!m_effectH) return 0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // AEGP_GetEffectNumParamStreams は StreamSuite にある
        // 注: この値にはインデックス0（入力レイヤー）が含まれる
        // 実際のパラメータはインデックス1から始まるため、
        // Python側に公開する数は (numParams - 1) とする
        A_long numParams;
        A_Err err = suites.streamSuite->AEGP_GetEffectNumParamStreams(m_effectH, &numParams);
        if (err != A_Err_NONE) return 0;

        // インデックス0（入力レイヤー）を除いた数を返す
        return (numParams > 0) ? static_cast<int>(numParams - 1) : 0;
    }

    std::shared_ptr<PyProperty> GetParamByIndex(int index) {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        int numParams = GetNumParams();
        if (index < 0 || index >= numParams) {
            throw std::out_of_range("Parameter index out of range");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // Python 0-based → AE 1-based (index 0 = input layer)
        A_long aeIndex = static_cast<A_long>(index + 1);
        AEGP_StreamRefH streamH = nullptr;
        A_Err err = suites.streamSuite->AEGP_GetNewEffectStreamByIndex(
            state.GetPluginID(), m_effectH, aeIndex, &streamH);
        if (err != A_Err_NONE || !streamH) {
            throw std::runtime_error("Failed to get effect parameter stream");
        }

        return std::make_shared<PyProperty>(
            streamH, true,
            StreamUtils::UnknownTypePolicy::TreatAsOneD,
            index);
    }

    std::shared_ptr<PyProperty> GetParamByName(const std::string& name) {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        int numParams = GetNumParams();
        for (int i = 0; i < numParams; ++i) {
            auto param = GetParamByIndex(i);
            if (param->GetName() == name) {
                return param;
            }
        }

        throw std::runtime_error("Parameter not found: " + name);
    }

    py::list GetAllParams() {
        py::list params;

        int numParams = GetNumParams();
        for (int i = 0; i < numParams; ++i) {
            params.append(GetParamByIndex(i));
        }

        return params;
    }

    // ==========================================
    // 操作
    // ==========================================
    std::shared_ptr<PyEffect> Duplicate() {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        AEGP_EffectRefH newEffectH;
        A_Err err = suites.effectSuite->AEGP_DuplicateEffect(m_effectH, &newEffectH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to duplicate effect");
        }

        // Duplicated effect gets a new index (next after current)
        int newIndex = (m_cachedIndex >= 0) ? (m_cachedIndex + 1) : -1;
        return std::make_shared<PyEffect>(newEffectH, true, m_layerH, newIndex);
    }

    void Delete() {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_Err err = suites.effectSuite->AEGP_DeleteLayerEffect(m_effectH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to delete effect");
        }

        m_effectH = nullptr;
    }

    // ==========================================
    // 名前設定（パラメータの親ストリーム経由）
    void SetName(const std::string& name) {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // エフェクトのパラメータ数を確認
        A_long numParams = 0;
        A_Err err = suites.streamSuite->AEGP_GetEffectNumParamStreams(
            m_effectH,
            &numParams);

        if (err != A_Err_NONE || numParams < 2) {
            throw std::runtime_error("Effect has no parameters, cannot get parent stream");
        }

        // エフェクトの最初のパラメータのストリームを取得
        // SDK は 1 ベースのインデックス: インデックス 0 はレイヤー自体、
        // インデックス 1 が最初の実パラメータ
        // (参照: docs/CODING_GUIDELINES.md のインデックス規約)
        AEGP_StreamRefH paramStreamH = nullptr;
        err = suites.streamSuite->AEGP_GetNewEffectStreamByIndex(
            state.GetPluginID(),
            m_effectH,
            1,  // SDK インデックス 1 = 最初の実パラメータ (0 はレイヤー)
            &paramStreamH);

        if (err != A_Err_NONE || !paramStreamH) {
            throw std::runtime_error("Failed to get effect parameter stream");
        }

        ScopedStreamRef scopedParam(suites.streamSuite, paramStreamH);

        // パラメータの親ストリーム（エフェクト自体）を取得
        AEGP_StreamRefH effectStreamH = nullptr;
        err = suites.dynamicStreamSuite->AEGP_GetNewParentStreamRef(
            state.GetPluginID(),
            paramStreamH,
            &effectStreamH);

        if (err != A_Err_NONE || !effectStreamH) {
            throw std::runtime_error("Failed to get parent stream (effect stream)");
        }

        ScopedStreamRef scopedEffect(suites.streamSuite, effectStreamH);

        // エフェクト名を設定
        std::wstring wname = StringUtils::Utf8ToWide(name);
        err = suites.dynamicStreamSuite->AEGP_SetStreamName(
            effectStreamH,
            reinterpret_cast<const A_UTF16Char*>(wname.c_str()));

        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set effect name");
        }
    }

    // ストリーム名を取得（SetNameで設定した名前を取得）
    std::string GetStreamName() const {
        if (!m_effectH) {
            return "";
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // パラメータ数を確認
        A_long numParams = 0;
        A_Err err = suites.streamSuite->AEGP_GetEffectNumParamStreams(
            m_effectH,
            &numParams);

        if (err != A_Err_NONE || numParams < 2) {
            return "";  // パラメータなしのエフェクト
        }

        // 最初のパラメータのストリームを取得
        AEGP_StreamRefH paramStreamH = nullptr;
        err = suites.streamSuite->AEGP_GetNewEffectStreamByIndex(
            state.GetPluginID(),
            m_effectH,
            1,  // SDK インデックス 1 = 最初の実パラメータ
            &paramStreamH);

        if (err != A_Err_NONE || !paramStreamH) {
            return "";
        }

        ScopedStreamRef scopedParam(suites.streamSuite, paramStreamH);

        // パラメータの親ストリーム（エフェクト自体）を取得
        AEGP_StreamRefH effectStreamH = nullptr;
        err = suites.dynamicStreamSuite->AEGP_GetNewParentStreamRef(
            state.GetPluginID(),
            paramStreamH,
            &effectStreamH);

        if (err != A_Err_NONE || !effectStreamH) {
            return "";
        }

        ScopedStreamRef scopedEffect(suites.streamSuite, effectStreamH);

        // エフェクトストリームの名前を取得
        AEGP_MemHandle nameH;
        err = suites.streamSuite->AEGP_GetStreamName(
            state.GetPluginID(),
            effectStreamH,
            FALSE,  // ローカライズされた名前を取得
            &nameH);

        if (err != A_Err_NONE || !nameH) {
            return "";
        }

        ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);
        ScopedMemLock lock(suites.memorySuite, nameH);
        A_UTF16Char* namePtr = lock.As<A_UTF16Char>();
        if (!namePtr) {
            return "";
        }

        return StringUtils::WideToUtf8(reinterpret_cast<const wchar_t*>(namePtr));
    }

    // ==========================================
    // エフェクトマスク (AE 13.0+)
    // ==========================================
    int GetNumMasks() const {
        if (!m_effectH) return 0;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_u_long num_masks = 0;
        A_Err err = suites.effectSuite->AEGP_NumEffectMask(m_effectH, &num_masks);
        if (err != A_Err_NONE) return 0;

        return static_cast<int>(num_masks);
    }

    int GetMaskID(int mask_index) const {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        AEGP_MaskIDVal id_val = 0;
        A_Err err = suites.effectSuite->AEGP_GetEffectMaskID(m_effectH, static_cast<A_u_long>(mask_index), &id_val);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get effect mask ID");
        }

        return static_cast<int>(id_val);
    }

    uintptr_t AddMask(int mask_id) {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        AEGP_StreamRefH streamH = nullptr;
        A_Err err = suites.effectSuite->AEGP_AddEffectMask(m_effectH, static_cast<AEGP_MaskIDVal>(mask_id), &streamH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to add effect mask");
        }

        return reinterpret_cast<uintptr_t>(streamH);
    }

    void RemoveMask(int mask_id) {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        A_Err err = suites.effectSuite->AEGP_RemoveEffectMask(m_effectH, static_cast<AEGP_MaskIDVal>(mask_id));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to remove effect mask");
        }
    }

    uintptr_t SetMask(int mask_index, int mask_id) {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        AEGP_StreamRefH streamH = nullptr;
        A_Err err = suites.effectSuite->AEGP_SetEffectMask(
            m_effectH, static_cast<A_u_long>(mask_index), static_cast<AEGP_MaskIDVal>(mask_id), &streamH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set effect mask");
        }

        return reinterpret_cast<uintptr_t>(streamH);
    }

    // ==========================================
    // UI Update
    // ==========================================
    void UpdateParamsUI() {
        if (!m_effectH) {
            throw std::runtime_error("Invalid effect");
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.effectSuite) throw std::runtime_error("Effect Suite not available");

        // Get current comp time from layer
        A_Time time = {0, 1};
        if (m_layerH && suites.layerSuite && suites.itemSuite) {
            AEGP_CompH compH = nullptr;
            A_Err compErr = suites.layerSuite->AEGP_GetLayerParentComp(m_layerH, &compH);
            if (compErr == A_Err_NONE && compH) {
                suites.itemSuite->AEGP_GetItemCurrentTime(
                    reinterpret_cast<AEGP_ItemH>(compH), &time);
            }
        }

        A_Err err = suites.effectSuite->AEGP_EffectCallGeneric(
            (AEGP_PluginID)state.GetPluginID(),
            m_effectH,
            &time,
            PF_Cmd_UPDATE_PARAMS_UI,
            nullptr
        );
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_EffectCallGeneric(UPDATE_PARAMS_UI) failed: " + std::to_string(err));
        }
    }

    AEGP_EffectRefH GetHandle() const { return m_effectH; }

private:
    AEGP_EffectRefH m_effectH;
    AEGP_LayerH m_layerH;  // Parent layer handle for index lookup
    bool m_ownsHandle;
    int m_cachedIndex;  // Cached effect index (0-based, -1 if unknown)
};

// =============================================================
// レイヤーのエフェクト操作ヘルパー関数
// =============================================================

// レイヤーからエフェクト数を取得
int GetLayerNumEffects(AEGP_LayerH layerH) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long numEffects;
    A_Err err = suites.effectSuite->AEGP_GetLayerNumEffects(layerH, &numEffects);
    if (err != A_Err_NONE) return 0;

    return static_cast<int>(numEffects);
}

// レイヤーからエフェクトを取得（インデックス指定）
std::shared_ptr<PyEffect> GetLayerEffectByIndex(AEGP_LayerH layerH, int index) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_EffectRefH effectH;
    A_Err err = suites.effectSuite->AEGP_GetLayerEffectByIndex(
        state.GetPluginID(), layerH, static_cast<A_long>(index), &effectH);
    if (err != A_Err_NONE || !effectH) {
        throw std::runtime_error("Failed to get effect at index");
    }

    // Cache the index we used to retrieve the effect
    return std::make_shared<PyEffect>(effectH, true, layerH, index);
}

// レイヤーにエフェクトを追加（マッチ名で指定）
py::object AddEffectToLayer(AEGP_LayerH layerH, const std::string& matchName) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // まずインストール済みエフェクトからマッチ名を検索
    A_long numInstalledEffects;
    A_Err err = suites.effectSuite->AEGP_GetNumInstalledEffects(&numInstalledEffects);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get installed effects count");
    }

    AEGP_InstalledEffectKey foundKey = AEGP_InstalledEffectKey_NONE;
    AEGP_InstalledEffectKey prevKey = AEGP_InstalledEffectKey_NONE;

    for (A_long i = 0; i < numInstalledEffects; ++i) {
        AEGP_InstalledEffectKey key;
        err = suites.effectSuite->AEGP_GetNextInstalledEffect(prevKey, &key);
        if (err != A_Err_NONE) break;

        A_char name[AEGP_MAX_EFFECT_MATCH_NAME_SIZE] = {0};
        err = suites.effectSuite->AEGP_GetEffectMatchName(key, name);
        if (err == A_Err_NONE && std::string(name) == matchName) {
            foundKey = key;
            break;
        }

        prevKey = key;
    }

    if (foundKey == AEGP_InstalledEffectKey_NONE) {
        throw std::runtime_error("Effect not found: " + matchName);
    }

    AEGP_InstalledEffectKey effectKey = foundKey;

    // エフェクトを適用
    AEGP_EffectRefH effectH;
    err = suites.effectSuite->AEGP_ApplyEffect(
        state.GetPluginID(), layerH, effectKey, &effectH);
    if (!effectH) {
        throw std::runtime_error("Failed to apply effect: match_name='" + matchName +
            "', err=" + std::to_string(err));
    }
    // Note: AEGP_ApplyEffect may return non-zero err (e.g. 6401) even when
    // effectH is valid. This can happen when the effect shows a dialog during
    // initialization. We proceed as long as we have a valid handle.

    // Get the index of the newly added effect (should be the last one)
    A_long numEffects = 0;
    suites.effectSuite->AEGP_GetLayerNumEffects(layerH, &numEffects);
    int newEffectIndex = (numEffects > 0) ? static_cast<int>(numEffects - 1) : 0;

    // PyEffectオブジェクトを作成してpy::objectとして返す
    auto effectPtr = std::make_shared<PyEffect>(effectH, true, layerH, newEffectIndex);
    return py::cast(effectPtr);
}

// 全インストール済みエフェクトのリストを取得
py::list GetInstalledEffects() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    py::list effects;

    A_long numEffects;
    A_Err err = suites.effectSuite->AEGP_GetNumInstalledEffects(&numEffects);
    if (err != A_Err_NONE) return effects;

    AEGP_InstalledEffectKey prevKey = AEGP_InstalledEffectKey_NONE;

    for (A_long i = 0; i < numEffects; ++i) {
        AEGP_InstalledEffectKey key;
        err = suites.effectSuite->AEGP_GetNextInstalledEffect(prevKey, &key);
        if (err != A_Err_NONE) break;

        A_char name[AEGP_MAX_EFFECT_NAME_SIZE] = {0};
        A_char matchName[AEGP_MAX_EFFECT_MATCH_NAME_SIZE] = {0};
        A_char category[AEGP_MAX_EFFECT_CATEGORY_NAME_SIZE] = {0};

        suites.effectSuite->AEGP_GetEffectName(key, name);
        suites.effectSuite->AEGP_GetEffectMatchName(key, matchName);
        suites.effectSuite->AEGP_GetEffectCategory(key, category);

        A_Boolean is_internal = FALSE;
#if defined(kAEGPEffectSuiteVersion5)
        suites.effectSuite->AEGP_GetIsInternalEffect(key, &is_internal);
#endif

        py::dict effectInfo;
        // ローカルエンコーディング（Shift-JIS等）からUTF-8に変換
        effectInfo["name"] = StringUtils::LocalToUtf8(name);
        effectInfo["match_name"] = StringUtils::LocalToUtf8(matchName);
        effectInfo["category"] = StringUtils::LocalToUtf8(category);
        effectInfo["is_internal"] = (is_internal != FALSE);

        effects.append(effectInfo);

        prevKey = key;
    }

    return effects;
}

} // namespace PyAE

void init_effect(py::module_& m) {
    // エフェクトクラス
    py::class_<PyAE::PyEffect, std::shared_ptr<PyAE::PyEffect>>(m, "Effect")
        .def_property_readonly("valid", &PyAE::PyEffect::IsValid,
                              "Check if effect is valid")
        .def_property_readonly("name", &PyAE::PyEffect::GetName,
                              "Effect plugin name (localized)")
        .def_property_readonly("stream_name", &PyAE::PyEffect::GetStreamName,
                              "Effect stream name (user-defined name set by set_name)")
        .def_property_readonly("match_name", &PyAE::PyEffect::GetMatchName,
                              "Effect match name (internal identifier)")
        .def_property_readonly("index", &PyAE::PyEffect::GetIndex,
                              "Effect index on layer")
        .def_property("enabled", &PyAE::PyEffect::GetEnabled, &PyAE::PyEffect::SetEnabled,
                     "Effect enabled state")
        .def_property_readonly("num_properties", &PyAE::PyEffect::GetNumParams,
                              "Number of properties")
        .def("property", &PyAE::PyEffect::GetParamByName,
             "Get property by name",
             py::arg("name"))
        .def("property_by_index", &PyAE::PyEffect::GetParamByIndex,
             "Get property by index",
             py::arg("index"))
        .def_property_readonly("properties", &PyAE::PyEffect::GetAllParams,
                              "List of all properties")
        .def("duplicate", &PyAE::PyEffect::Duplicate,
             "Duplicate the effect")
        .def("delete", &PyAE::PyEffect::Delete,
             "Delete the effect from the layer")
        .def("set_name", &PyAE::PyEffect::SetName,
             "Set the effect name (via parameter parent stream)",
             py::arg("name"))
        // Effect masks (AE 13.0+)
        .def_property_readonly("num_masks", &PyAE::PyEffect::GetNumMasks,
                              "Number of effect masks")
        .def("get_mask_id", &PyAE::PyEffect::GetMaskID,
             "Get mask ID at specified index",
             py::arg("mask_index"))
        .def("add_mask", &PyAE::PyEffect::AddMask,
             "Add a mask to the effect, returns stream handle",
             py::arg("mask_id"))
        .def("remove_mask", &PyAE::PyEffect::RemoveMask,
             "Remove a mask from the effect",
             py::arg("mask_id"))
        .def("set_mask", &PyAE::PyEffect::SetMask,
             "Set mask at specified index, returns stream handle",
             py::arg("mask_index"), py::arg("mask_id"))
        // UI Update
        .def("update_params_ui", &PyAE::PyEffect::UpdateParamsUI,
             "Notify effect to update its parameter UI after value changes")
        // 内部使用: ハンドルからオブジェクト作成
        .def_static("_create_from_handle", [](uintptr_t handle, uintptr_t layerH = 0, int index = -1) {
            AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(handle);
            AEGP_LayerH layer = layerH ? reinterpret_cast<AEGP_LayerH>(layerH) : nullptr;
            return std::make_shared<PyAE::PyEffect>(effectH, true, layer, index);
        }, "Internal: create Effect from handle",
           py::arg("handle"), py::arg("layerH") = 0, py::arg("index") = -1)

        // Python protocol methods
        .def("__repr__", [](const PyAE::PyEffect& self) {
            if (!self.IsValid()) return std::string("<Effect: invalid>");
            std::string name = self.GetName();
            std::string match = self.GetMatchName();
            return std::string("<Effect: '") + name + "' match='" + match + "'>";
        })
        .def("__bool__", &PyAE::PyEffect::IsValid)
        // Collection protocols (properties)
        .def("__len__", &PyAE::PyEffect::GetNumParams)
        .def("__iter__", [](PyAE::PyEffect& self) {
            py::list props = self.GetAllParams();
            return props.attr("__iter__")();
        })
        .def("__getitem__", [](PyAE::PyEffect& self, py::object key) -> py::object {
            if (py::isinstance<py::int_>(key)) {
                int idx = key.cast<int>();
                int num = self.GetNumParams();
                if (idx < 0) idx += num;
                if (idx < 0 || idx >= num) throw py::index_error("Property index out of range");
                auto result = self.GetParamByIndex(idx);
                if (!result) throw py::index_error("Failed to get property");
                return py::cast(result);
            } else if (py::isinstance<py::str>(key)) {
                auto result = self.GetParamByName(key.cast<std::string>());
                if (!result) throw py::key_error(key.cast<std::string>());
                return py::cast(result);
            }
            throw py::type_error("Property key must be int or str");
        })
        .def("__contains__", [](PyAE::PyEffect& self, const std::string& name) {
            auto result = self.GetParamByName(name);
            return result != nullptr;
        })

        // Serialization methods
        .def("to_dict", [](PyAE::PyEffect& self) -> py::dict {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                return serialize.attr("effect_to_dict")(self).cast<py::dict>();
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to serialize effect: ") + e.what());
            }
        }, "Export effect to dictionary")

        .def("update_from_dict", [](PyAE::PyEffect& self, const py::dict& data) {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                serialize.attr("effect_update_from_dict")(self, data);
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to update effect from dict: ") + e.what());
            }
        }, "Update effect from dictionary",
           py::arg("data"));

    // グローバル関数
    m.def("get_installed_effects", &PyAE::GetInstalledEffects,
          "Get list of all installed effects");
}
