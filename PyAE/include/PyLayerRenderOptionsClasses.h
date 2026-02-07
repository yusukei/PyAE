// PyLayerRenderOptionsClasses.h
// PyAE - Python for After Effects
// レイヤーレンダリングオプションクラスの宣言
//
// LayerRenderOptionsSuite2 の高レベルAPI

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <tuple>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#include "AE_GeneralPlug.h"
#include "PyWorldClasses.h"  // For WorldType enum
#include "PyRenderClasses.h" // For MatteMode enum

namespace py = pybind11;

namespace PyAE {

// Forward declarations
class PyLayerRenderOptions;

// =============================================================
// PyLayerRenderOptions - レイヤーレンダリング設定クラス
// AEGP_LayerRenderOptionsH のラッパー
// =============================================================
class PyLayerRenderOptions {
public:
    // Default constructor (invalid options)
    PyLayerRenderOptions();

    // Constructor from raw handle (takes ownership if owned=true)
    explicit PyLayerRenderOptions(AEGP_LayerRenderOptionsH optionsH, bool owned = true);

    // Copy constructor (disabled)
    PyLayerRenderOptions(const PyLayerRenderOptions&) = delete;
    PyLayerRenderOptions& operator=(const PyLayerRenderOptions&) = delete;

    // Move constructor
    PyLayerRenderOptions(PyLayerRenderOptions&& other) noexcept;
    PyLayerRenderOptions& operator=(PyLayerRenderOptions&& other) noexcept;

    // Destructor - disposes the options if owned
    ~PyLayerRenderOptions();

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

    std::tuple<int, int> GetDownsampleFactor() const;
    void SetDownsampleFactor(int x, int y);

    MatteMode GetMatteMode() const;
    void SetMatteMode(MatteMode mode);

    // =============================================================
    // Duplication
    // =============================================================

    std::shared_ptr<PyLayerRenderOptions> Duplicate() const;

    // =============================================================
    // Handle access
    // =============================================================

    AEGP_LayerRenderOptionsH GetHandle() const;
    AEGP_LayerRenderOptionsH Release();
    bool IsOwned() const;

    // =============================================================
    // Static factory methods
    // =============================================================

    // Create layer render options from a layer
    static std::shared_ptr<PyLayerRenderOptions> FromLayer(uintptr_t layerH);

    // Create layer render options from upstream of an effect
    static std::shared_ptr<PyLayerRenderOptions> FromUpstreamOfEffect(uintptr_t effectH);

    // Create layer render options from downstream of an effect
    static std::shared_ptr<PyLayerRenderOptions> FromDownstreamOfEffect(uintptr_t effectH);

private:
    AEGP_LayerRenderOptionsH m_optionsH;
    bool m_owned;

    void Dispose();
};

// =============================================================
// Module init function declaration
// =============================================================
void init_layer_render_options(py::module_& m);

} // namespace PyAE
