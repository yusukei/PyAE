// PyWorldClasses.h
// PyAE - Python for After Effects
// フレームバッファ（World）関連クラスの宣言
//
// WorldSuite3の高レベルAPI

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

namespace py = pybind11;

namespace PyAE {

// =============================================================
// WorldType - ワールド（フレームバッファ）のビット深度
// =============================================================
enum class WorldType {
    NONE = 0,    // AEGP_WorldType_NONE - Invalid/no world
    BIT8 = 1,    // AEGP_WorldType_8 - 8bpc
    BIT16 = 2,   // AEGP_WorldType_16 - 16bpc
    BIT32 = 3    // AEGP_WorldType_32 - 32bpc (float)
};

// =============================================================
// PyWorld - フレームバッファクラス
// AEGP_WorldHのラッパー
// =============================================================
class PyWorld {
public:
    // Default constructor (invalid world)
    PyWorld();

    // Constructor from raw handle (takes ownership if owned=true)
    explicit PyWorld(AEGP_WorldH worldH, bool owned = true);

    // Copy constructor (disabled - worlds are expensive to copy)
    PyWorld(const PyWorld&) = delete;
    PyWorld& operator=(const PyWorld&) = delete;

    // Move constructor
    PyWorld(PyWorld&& other) noexcept;
    PyWorld& operator=(PyWorld&& other) noexcept;

    // Destructor - disposes the world if owned
    ~PyWorld();

    // =============================================================
    // State check
    // =============================================================

    // Check if world is valid
    bool IsValid() const;

    // =============================================================
    // Properties (read-only)
    // =============================================================

    // Get world type (bit depth)
    WorldType GetType() const;

    // Get width in pixels
    int GetWidth() const;

    // Get height in pixels
    int GetHeight() const;

    // Get size as tuple (width, height)
    std::tuple<int, int> GetSize() const;

    // Get bytes per row (row stride)
    int GetRowBytes() const;

    // =============================================================
    // Pixel data access
    // =============================================================

    // Get all pixel data as bytes
    // Returns raw pixel data in ARGB format
    // For 8bpc: 4 bytes per pixel (ARGB)
    // For 16bpc: 8 bytes per pixel (ARGB, 16-bit each)
    // For 32bpc: 16 bytes per pixel (ARGB, float each)
    py::bytes GetPixels() const;

    // Set all pixel data from bytes
    // Data size must match GetRowBytes() * GetHeight()
    void SetPixels(const py::bytes& data);

    // Get single pixel value at (x, y)
    // Returns tuple (R, G, B, A) normalized to 0.0-1.0
    std::tuple<float, float, float, float> GetPixel(int x, int y) const;

    // Set single pixel value at (x, y)
    // Values should be normalized 0.0-1.0
    void SetPixel(int x, int y, float r, float g, float b, float a);

    // =============================================================
    // Image processing
    // =============================================================

    // Apply fast blur
    // radius: blur radius in pixels
    // flags: quality flags (0 = default)
    // quality: quality level (0-5, default 1)
    void FastBlur(float radius, int flags = 0, int quality = 1);

    // =============================================================
    // Handle access
    // =============================================================

    // Get raw handle (for internal use)
    AEGP_WorldH GetHandle() const;

    // Release ownership and return handle (caller takes ownership)
    AEGP_WorldH Release();

    // Check if this world owns its handle
    bool IsOwned() const;

    // =============================================================
    // Static factory methods
    // =============================================================

    // Create a new world with specified dimensions
    static std::shared_ptr<PyWorld> Create(WorldType type, int width, int height);

private:
    AEGP_WorldH m_worldH;
    bool m_owned;

    // Internal dispose helper
    void Dispose();

    // Get base address for different bit depths
    void* GetBaseAddr() const;
};

// =============================================================
// Module init function declaration
// =============================================================
void init_world(py::module_& m);

} // namespace PyAE
