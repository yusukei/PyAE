// PyCompClasses.h
// PyAE - Python for After Effects
// コンポジション関連クラスの宣言
//
// PYAE-008: PyComp.cppから分割
// クラス宣言のみ、実装は各cppファイルに分離

#pragma once

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

namespace py = pybind11;

namespace PyAE {

// 前方宣言
class PyLayer;  // PyLayerClasses.hで定義
class PyLayerRef;
class PyComp;
class PyMask;

// Helper function from PyEffect.cpp
py::object AddEffectToLayer(AEGP_LayerH layerH, const std::string& matchName);

// =============================================================
// PyComp - コンポジションクラス
// 実装: PyComp_Core.cpp
// =============================================================
class PyComp {
public:
    PyComp();
    explicit PyComp(AEGP_CompH compH);

    bool IsValid() const;

    // 名前
    std::string GetName() const;
    void SetName(const std::string& name);

    // サイズ
    int GetWidth() const;
    int GetHeight() const;
    py::tuple GetResolution() const;

    // 時間関連
    double GetDuration() const;
    void SetDuration(double duration);
    double GetFrameRate() const;
    void SetFrameRate(double fps);
    double GetCTITime() const;
    void SetCTITime(double seconds);
    double GetWorkAreaStart() const;
    void SetWorkAreaStart(double time);
    double GetWorkAreaDuration() const;
    void SetWorkAreaDuration(double duration);

    // プロパティ
    double GetPixelAspect() const;
    void SetPixelAspect(double aspect);
    py::tuple GetBgColor() const;
    void SetBgColor(py::tuple color);

    // 新規追加プロパティ
    void SetDimensions(int width, int height);
    double GetDisplayStartTime() const;
    void SetDisplayStartTime(double seconds);
    bool GetShowLayerNames() const;
    void SetShowLayerNames(bool show);
    bool GetShowBlendModes() const;
    void SetShowBlendModes(bool show);
    bool GetDisplayDropFrame() const;
    void SetDisplayDropFrame(bool dropFrame);
    double GetFrameDuration() const;
    py::tuple GetShutterAnglePhase() const;
    int GetMotionBlurSamples() const;
    void SetMotionBlurSamples(int samples);
    int GetMotionBlurAdaptiveSampleLimit() const;
    void SetMotionBlurAdaptiveSampleLimit(int samples);

    // レイヤー操作
    int GetNumLayers() const;
    PyLayer GetLayerByIndex(int index) const;
    PyLayer GetLayerByName(const std::string& name) const;
    PyLayer GetLayer(const py::object& key) const; // Overload for index/name
    std::vector<PyLayer> GetLayers() const;
    std::vector<PyLayer> GetSelectedLayers() const;

    // レイヤー追加
    PyLayer AddSolid(const std::string& name, int width, int height,
                     double r, double g, double b, double duration = -1.0);
    PyLayer AddNull(const std::string& name = "Null", double duration = -1.0);
    PyLayer AddCamera(const std::string& name, double x = 0.0, double y = 0.0);
    PyLayer AddLight(const std::string& name, double x = 0.0, double y = 0.0);
    PyLayer AddLayer(AEGP_ItemH itemH, double duration = -1.0);
    PyLayer AddText(const std::string& text = "");
    PyLayer AddBoxText(double boxWidth, double boxHeight, bool horizontal = true);
    PyLayer AddShape();

    // コンポジション操作
    PyComp Duplicate() const;

    // ハンドルアクセス
    AEGP_CompH GetHandle() const;
    AEGP_ItemH GetItemHandle() const;

    // カラープロファイル
    py::object GetColorProfile() const;
    void SetColorProfile(py::object profile);

    // 静的メソッド
    static PyComp GetActive();
    static PyComp GetMostRecentlyUsed();
    static PyComp Create(const std::string& name, int width, int height,
                        double pixelAspect = 1.0, double duration = 10.0, double frameRate = 30.0);

private:
    AEGP_CompH m_compH;
    AEGP_ItemH m_itemH;
};

} // namespace PyAE
