// PyProperty.h
// PyAE - Python for After Effects
// プロパティクラス宣言

#pragma once

#include <string>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "AE_GeneralPlug.h"
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"

namespace PyAE {

// プロパティタイプ列挙
enum class PropertyType {
    None = 0,
    OneDim,     // 1次元（不透明度など）
    TwoDim,     // 2次元（位置XYなど）
    ThreeDim,   // 3次元（位置XYZなど）
    Color,      // 色
    Angle,      // 角度
    NoValue     // 値なし（グループなど）
};

// プロパティラッパークラス
class PyProperty {
public:
    PyProperty() : m_streamH(nullptr), m_ownsHandle(false) {}

    explicit PyProperty(AEGP_StreamRefH streamH, bool ownsHandle = true)
        : m_streamH(streamH)
        , m_ownsHandle(ownsHandle)
    {}

    ~PyProperty();

    // コピー禁止、ムーブのみ
    PyProperty(const PyProperty&) = delete;
    PyProperty& operator=(const PyProperty&) = delete;
    PyProperty(PyProperty&& other) noexcept;
    PyProperty& operator=(PyProperty&& other) noexcept;

    bool IsValid() const { return m_streamH != nullptr; }

    // 基本情報
    std::string GetName() const;
    PropertyType GetType() const;

    // 値の取得・設定（pybind11側で実装）
    // Expression関連
    bool HasExpression() const;
    std::string GetExpression() const;
    void SetExpression(const std::string& expression);
    void EnableExpression(bool enable);

    AEGP_StreamRefH GetHandle() const { return m_streamH; }

private:
    AEGP_StreamRefH m_streamH;
    bool m_ownsHandle;
};

// レイヤーストリーム名からAEGP_LayerStreamへの変換
AEGP_LayerStream NameToLayerStream(const std::string& name);

// レイヤーからプロパティを取得するヘルパー関数
std::shared_ptr<PyProperty> GetLayerProperty(AEGP_LayerH layerH, const std::string& name);

} // namespace PyAE
