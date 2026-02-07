// PyLayerClasses.h
// PyAE - Python for After Effects
// レイヤー関連クラスの宣言
//
// PYAE-008b: PyLayer.cppから分割
// クラス宣言のみ、実装は各cppファイルに分離

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PluginState.h"
#include "IdleHandler.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "Logger.h"

namespace py = pybind11;

namespace PyAE {

// Helper function from PyEffect.cpp
py::object AddEffectToLayer(AEGP_LayerH layerH, const std::string& matchName);

// =============================================================
// LayerType - レイヤータイプ列挙
// =============================================================
enum class LayerType {
    None = 0,
    AV,         // Audio/Video
    Light,
    Camera,
    Text,
    Adjustment,
    Null,
    Shape,
    Solid
};

// =============================================================
// LayerQuality - レイヤー品質
// =============================================================
enum class LayerQuality : int {
    None = AEGP_LayerQual_NONE,
    Wireframe = AEGP_LayerQual_WIREFRAME,
    Draft = AEGP_LayerQual_DRAFT,
    Best = AEGP_LayerQual_BEST
};

// =============================================================
// SamplingQuality - サンプリング品質
// =============================================================
enum class SamplingQuality : int {
    Bilinear = AEGP_LayerSamplingQual_BILINEAR,
    Bicubic = AEGP_LayerSamplingQual_BICUBIC
};

// =============================================================
// BlendMode - ブレンドモード (PF_TransferMode)
// =============================================================
enum class BlendMode : int {
    None = PF_Xfer_NONE,
    Copy = PF_Xfer_COPY,
    Behind = PF_Xfer_BEHIND,
    InFront = PF_Xfer_IN_FRONT,
    Dissolve = PF_Xfer_DISSOLVE,
    Add = PF_Xfer_ADD,
    Multiply = PF_Xfer_MULTIPLY,
    Screen = PF_Xfer_SCREEN,
    Overlay = PF_Xfer_OVERLAY,
    SoftLight = PF_Xfer_SOFT_LIGHT,
    HardLight = PF_Xfer_HARD_LIGHT,
    Darken = PF_Xfer_DARKEN,
    Lighten = PF_Xfer_LIGHTEN,
    Difference = PF_Xfer_DIFFERENCE,
    Hue = PF_Xfer_HUE,
    Saturation = PF_Xfer_SATURATION,
    Color = PF_Xfer_COLOR,
    Luminosity = PF_Xfer_LUMINOSITY,
    MultiplyAlpha = PF_Xfer_MULTIPLY_ALPHA,
    MultiplyAlphaLuma = PF_Xfer_MULTIPLY_ALPHA_LUMA,
    MultiplyNotAlpha = PF_Xfer_MULTIPLY_NOT_ALPHA,
    MultiplyNotAlphaLuma = PF_Xfer_MULTIPLY_NOT_ALPHA_LUMA,
    AdditivePremul = PF_Xfer_ADDITIVE_PREMUL,
    AlphaAdd = PF_Xfer_ALPHA_ADD,
    ColorDodge = PF_Xfer_COLOR_DODGE,
    ColorBurn = PF_Xfer_COLOR_BURN,
    Exclusion = PF_Xfer_EXCLUSION,
    Difference2 = PF_Xfer_DIFFERENCE2,
    ColorDodge2 = PF_Xfer_COLOR_DODGE2,
    ColorBurn2 = PF_Xfer_COLOR_BURN2,
    LinearDodge = PF_Xfer_LINEAR_DODGE,
    LinearBurn = PF_Xfer_LINEAR_BURN,
    LinearLight = PF_Xfer_LINEAR_LIGHT,
    VividLight = PF_Xfer_VIVID_LIGHT,
    PinLight = PF_Xfer_PIN_LIGHT,
    HardMix = PF_Xfer_HARD_MIX,
    LighterColor = PF_Xfer_LIGHTER_COLOR,
    DarkerColor = PF_Xfer_DARKER_COLOR,
    Subtract = PF_Xfer_SUBTRACT,
    Divide = PF_Xfer_DIVIDE
};

// =============================================================
// TrackMatteMode - トラックマットモード
// =============================================================
enum class TrackMatteMode : int {
    NoTrackMatte = AEGP_TrackMatte_NO_TRACK_MATTE,
    Alpha = AEGP_TrackMatte_ALPHA,
    NotAlpha = AEGP_TrackMatte_NOT_ALPHA,
    Luma = AEGP_TrackMatte_LUMA,
    NotLuma = AEGP_TrackMatte_NOT_LUMA
};

// =============================================================
// PyLayer - レイヤークラス
// 実装: PyLayer_Core.cpp, PyLayer_Time.cpp, PyLayer_Flags.cpp,
//       PyLayer_Relations.cpp, PyLayer_Transform.cpp
// =============================================================
class PyLayer {
public:
    PyLayer();
    explicit PyLayer(AEGP_LayerH layerH);

    bool IsValid() const;

    // ==========================================
    // 名前（実装: PyLayer_Core.cpp）
    // ==========================================
    std::string GetName() const;
    void SetName(const std::string& name);

    // ==========================================
    // コメント（実装: PyLayer_Core.cpp）
    // ==========================================
    std::string GetComment() const;
    void SetComment(const std::string& comment);

    // ==========================================
    // 品質・ラベル（実装: PyLayer_Core.cpp）
    // ==========================================
    int GetQuality() const;
    void SetQuality(int quality);
    int GetSamplingQuality() const;
    void SetSamplingQuality(int sampling_quality);
    int GetLabel() const;
    void SetLabel(int label);

    // ==========================================
    // マーカー（実装: PyLayer_Marker.cpp）
    // ==========================================
    void AddMarker(double time, const std::string& comment);
    void AddMarkerObject(double time, std::shared_ptr<class PyMarker> marker);
    std::string GetMarkerComment(double time) const;

    // ==========================================
    // インデックス（実装: PyLayer_Core.cpp）
    // ==========================================
    int GetIndex() const;
    void SetIndex(int index);

    // ==========================================
    // 時間関連（実装: PyLayer_Time.cpp）
    // ==========================================
    double GetInPoint() const;
    void SetInPoint(double seconds);
    double GetOutPoint() const;
    void SetOutPoint(double seconds);
    double GetStartTime() const;
    void SetStartTime(double seconds);
    double GetDuration() const;
    std::pair<int, int> GetStretch() const;
    void SetStretch(int num, int den);
    double GetCurrentTime() const;

    // ==========================================
    // フラグ（実装: PyLayer_Flags.cpp）
    // ==========================================
    bool GetEnabled() const;
    void SetEnabled(bool enabled);
    bool GetAudioEnabled() const;
    void SetAudioEnabled(bool enabled);
    bool GetSolo() const;
    void SetSolo(bool solo);
    bool GetLocked() const;
    void SetLocked(bool locked);
    bool GetShy() const;
    void SetShy(bool shy);
    bool GetCollapseTransformation() const;
    void SetCollapseTransformation(bool collapse);

    // ==========================================
    // 親レイヤー・タイプ（実装: PyLayer_Relations.cpp）
    // ==========================================
    py::object GetParent() const;
    void SetParent(py::object parent);
    LayerType GetLayerType() const;
    bool IsAdjustmentLayer() const;
    void SetAdjustmentLayer(bool value);
    bool IsNullLayer() const;
    bool Is3DLayer() const;
    void Set3DLayer(bool value);
    bool IsTimeRemappingEnabled() const;
    void SetTimeRemappingEnabled(bool value);
    bool GetSelected() const;
    void SetSelected(bool value);

    // ==========================================
    // ID関連（実装: PyLayer_Core.cpp）
    // ==========================================
    int GetLayerID() const;
    int GetSourceItemID() const;
    int GetObjectType() const;
    bool Is2D() const;

    // ==========================================
    // トラックマット（実装: PyLayer_Matte.cpp）
    // ==========================================
    bool HasTrackMatte() const;
    bool IsUsedAsTrackMatte() const;
    py::object GetTrackMatteLayer() const;
    void SetTrackMatte(PyLayer& matte_layer, int matte_mode);
    void RemoveTrackMatte();

    // ==========================================
    // Transform プロパティ（実装: PyLayer_Transform.cpp）
    // ==========================================
    py::object GetPosition() const;
    void SetPosition(py::object value);
    py::object GetScale() const;
    void SetScale(py::object value);
    py::object GetRotation() const;
    void SetRotation(py::object value);
    py::object GetAnchorPoint() const;
    void SetAnchorPoint(py::object value);
    double GetOpacity() const;
    void SetOpacity(double value);
    py::dict GetTransferMode() const;
    void SetTransferMode(const py::dict& transfer_mode);
    py::list ToWorldXform(double comp_time) const;

    // ==========================================
    // High-level Keyframe API（実装: PyLayer_Transform.cpp）
    // ==========================================
    void SetPositionKeyframe(double time, py::object value);
    void SetScaleKeyframe(double time, py::object value);
    void SetRotationKeyframe(double time, double value);
    void SetOpacityKeyframe(double time, double value);

    // ==========================================
    // 操作（実装: PyLayer_Core.cpp）
    // ==========================================
    void Delete();
    PyLayer Duplicate();
    void MoveTo(int index);

    // ==========================================
    // メタデータ取得（実装: PyLayer_Core.cpp）
    // ==========================================
    py::dict GetLayerMetadata() const;

    // ==========================================
    // エフェクト（実装: PyLayer_Effect.cpp）
    // ==========================================
    int GetNumEffects() const;
    py::list GetAllEffects();
    py::object GetEffectByIndex(int index);
    py::object GetEffectByName(const std::string& name);
    py::object AddEffect(const std::string& matchName);

    // ==========================================
    // マスク（実装: PyLayer_Mask.cpp）
    // ==========================================
    int GetNumMasks() const;
    py::list GetAllMasks();
    py::object GetMaskByIndex(int index);
    py::object GetMaskByName(const std::string& name);
    py::object AddMask(const py::list& vertices);

    // ==========================================
    // ハンドルアクセス
    // ==========================================
    AEGP_LayerH GetHandle() const;

private:
    // ストリーム値取得/設定ヘルパー（実装: PyLayer_Transform.cpp）
    py::object GetStreamValue(AEGP_LayerStream streamType) const;
    void SetStreamValue(AEGP_LayerStream streamType, py::object value);

    AEGP_LayerH m_layerH;
};

} // namespace PyAE
