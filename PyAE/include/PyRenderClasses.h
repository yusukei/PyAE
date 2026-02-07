// PyRenderClasses.h
// PyAE - Python for After Effects
// レンダリング関連クラスの宣言
//
// RenderOptionsSuite4, RenderSuite5 の高レベルAPI

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <tuple>
#include <memory>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#include "AE_GeneralPlug.h"
#include "PyWorldClasses.h"

namespace py = pybind11;

namespace PyAE {

// Forward declarations
class PyRenderOptions;
class PyFrameReceipt;

// =============================================================
// MatteMode - マットモード
// =============================================================
enum class MatteMode {
    STRAIGHT = 0,       // ストレートアルファ
    PREMUL_BLACK = 1,   // プリマルチプライド（黒）
    PREMUL_BG_COLOR = 2 // プリマルチプライド（背景色）
};

// =============================================================
// ChannelOrder - チャンネル順序
// =============================================================
enum class ChannelOrder {
    ARGB = 0,
    BGRA = 1
};

// =============================================================
// FieldRender - フィールドレンダリング
// =============================================================
enum class FieldRender {
    FRAME = 0,  // フレーム（フィールドなし）
    UPPER = 1,  // 上位フィールド
    LOWER = 2   // 下位フィールド
};

// =============================================================
// RenderQuality - レンダリング品質
// =============================================================
enum class RenderQuality {
    DRAFT = 0,  // ドラフト品質（高速）
    BEST = 1    // 最高品質
};

// =============================================================
// PyRenderOptions - レンダリング設定クラス
// AEGP_RenderOptionsH のラッパー
// =============================================================
class PyRenderOptions {
public:
    // Default constructor (invalid options)
    PyRenderOptions();

    // Constructor from raw handle (takes ownership if owned=true)
    explicit PyRenderOptions(AEGP_RenderOptionsH optionsH, bool owned = true);

    // Copy constructor (disabled)
    PyRenderOptions(const PyRenderOptions&) = delete;
    PyRenderOptions& operator=(const PyRenderOptions&) = delete;

    // Move constructor
    PyRenderOptions(PyRenderOptions&& other) noexcept;
    PyRenderOptions& operator=(PyRenderOptions&& other) noexcept;

    // Destructor - disposes the options if owned
    ~PyRenderOptions();

    // =============================================================
    // State check
    // =============================================================

    bool IsValid() const;

    // =============================================================
    // Time properties
    // =============================================================

    double GetTime() const;
    void SetTime(double seconds);

    double GetTimeStep() const;
    void SetTimeStep(double seconds);

    // =============================================================
    // Rendering properties
    // =============================================================

    WorldType GetWorldType() const;
    void SetWorldType(WorldType type);

    FieldRender GetFieldRender() const;
    void SetFieldRender(FieldRender field);

    std::tuple<int, int> GetDownsampleFactor() const;
    void SetDownsampleFactor(int x, int y);

    py::dict GetRegionOfInterest() const;
    void SetRegionOfInterest(int left, int top, int right, int bottom);

    MatteMode GetMatteMode() const;
    void SetMatteMode(MatteMode mode);

    ChannelOrder GetChannelOrder() const;
    void SetChannelOrder(ChannelOrder order);

    bool GetRenderGuideLayers() const;
    void SetRenderGuideLayers(bool render);

    RenderQuality GetRenderQuality() const;
    void SetRenderQuality(RenderQuality quality);

    // =============================================================
    // Duplication
    // =============================================================

    std::shared_ptr<PyRenderOptions> Duplicate() const;

    // =============================================================
    // Handle access
    // =============================================================

    AEGP_RenderOptionsH GetHandle() const;
    AEGP_RenderOptionsH Release();
    bool IsOwned() const;

    // =============================================================
    // Static factory method
    // =============================================================

    // Create render options from an item
    static std::shared_ptr<PyRenderOptions> FromItem(uintptr_t itemH);

private:
    AEGP_RenderOptionsH m_optionsH;
    bool m_owned;

    void Dispose();
};

// =============================================================
// PyFrameReceipt - レンダリング結果クラス
// AEGP_FrameReceiptH のラッパー
// =============================================================
class PyFrameReceipt {
public:
    // Default constructor (invalid receipt)
    PyFrameReceipt();

    // Constructor from raw handle (takes ownership)
    explicit PyFrameReceipt(AEGP_FrameReceiptH receiptH);

    // Copy constructor (disabled)
    PyFrameReceipt(const PyFrameReceipt&) = delete;
    PyFrameReceipt& operator=(const PyFrameReceipt&) = delete;

    // Move constructor
    PyFrameReceipt(PyFrameReceipt&& other) noexcept;
    PyFrameReceipt& operator=(PyFrameReceipt&& other) noexcept;

    // Destructor - checks in the frame
    ~PyFrameReceipt();

    // =============================================================
    // State check
    // =============================================================

    bool IsValid() const;

    // =============================================================
    // Properties
    // =============================================================

    // Get the rendered world (frame buffer)
    // Note: The returned world is READ-ONLY and owned by the receipt
    std::shared_ptr<PyWorld> GetWorld() const;

    // Get the rendered region
    py::dict GetRenderedRegion() const;

    // Get the GUID of this frame
    std::string GetGuid() const;

    // =============================================================
    // Checkin (explicit release)
    // =============================================================

    // Explicitly check in the frame (release resources)
    // After calling this, the receipt becomes invalid
    void Checkin();

    // =============================================================
    // Context manager support
    // =============================================================

    PyFrameReceipt* Enter();
    void Exit(py::object exc_type, py::object exc_val, py::object exc_tb);

    // =============================================================
    // Handle access
    // =============================================================

    AEGP_FrameReceiptH GetHandle() const;

private:
    AEGP_FrameReceiptH m_receiptH;
    bool m_checkedIn;
};

// =============================================================
// PyRenderer - レンダリング実行クラス（静的メソッドのみ）
// =============================================================
class PyRenderer {
public:
    // Render a frame using RenderOptions
    static std::shared_ptr<PyFrameReceipt> RenderFrame(
        const std::shared_ptr<PyRenderOptions>& options);

    // Render a layer frame using LayerRenderOptions (handle as uintptr_t)
    static std::shared_ptr<PyFrameReceipt> RenderLayerFrame(uintptr_t layerOptionsH);

    // Get current project timestamp (for cache validation)
    static std::tuple<int, int, int, int> GetCurrentTimestamp();

    // Check if an item has changed since a timestamp
    static bool HasItemChangedSinceTimestamp(
        uintptr_t itemH,
        double startTime,
        double duration,
        const std::tuple<int, int, int, int>& timestamp);

    // Check if an item is worthwhile to render
    static bool IsItemWorthwhileToRender(
        const std::shared_ptr<PyRenderOptions>& options,
        const std::tuple<int, int, int, int>& timestamp);

    // Check if a rendered frame is sufficient for proposed options
    static bool IsRenderedFrameSufficient(
        const std::shared_ptr<PyRenderOptions>& renderedOptions,
        const std::shared_ptr<PyRenderOptions>& proposedOptions);
};

// =============================================================
// Module init function declaration
// =============================================================
void init_render(py::module_& m);

} // namespace PyAE
