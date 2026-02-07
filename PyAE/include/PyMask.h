// PyMask.h
// PyAE - Python for After Effects

#pragma once

#include <pybind11/pybind11.h>
#include "AE_Effect.h"
#include "AE_GeneralPlug.h"
#include "AEGP_SuiteHandler.h"

namespace py = pybind11;

namespace PyAE {

// Mask Mode
enum class MaskMode {
    None = 0,
    Add,
    Subtract,
    Intersect,
    Lighten,
    Darken,
    Difference
};

// Bezier Structure
struct MaskVertex {
    double x, y;
    double inTangentX, inTangentY;
    double outTangentX, outTangentY;
};

// =============================================================
// PyMask - Mask Class
// =============================================================
class PyMask {
public:
    PyMask();
    PyMask(AEGP_MaskRefH maskH, AEGP_LayerH layerH, bool ownsHandle = true);
    ~PyMask();

    // Copy/Move
    PyMask(const PyMask&) = delete;
    PyMask& operator=(const PyMask&) = delete;
    PyMask(PyMask&& other) noexcept;
    PyMask& operator=(PyMask&& other) noexcept;

    bool IsValid() const;

    // Basic
    std::string GetName() const;
    int GetIndex() const;
    A_long GetMaskID() const;

    // Mask Mode
    MaskMode GetMode() const;
    void SetMode(MaskMode mode);

    // Flags
    bool GetInverted() const;
    void SetInverted(bool inverted);
    bool GetLocked() const;
    void SetLocked(bool locked);
    bool GetIsRotoBezier() const;
    void SetIsRotoBezier(bool is_roto_bezier);

    // Properties
    double GetOpacity() const;
    void SetOpacity(double value);
    double GetExpansion() const;
    void SetExpansion(double value);
    py::tuple GetFeather() const;
    void SetFeather(double x, double y);
    py::tuple GetColor() const;
    void SetColor(double red, double green, double blue);

    // Outline (for MaskOutlineSuite3)
    uintptr_t GetOutline() const;

    // Operation
    void Delete();
    std::shared_ptr<PyMask> Duplicate() const;

    AEGP_MaskRefH GetHandle() const;

private:
    double GetMaskStreamValue(AEGP_MaskStream streamType) const;
    void SetMaskStreamValue(AEGP_MaskStream streamType, double value);

    AEGP_MaskRefH m_maskH;
    AEGP_LayerH m_layerH;
    bool m_ownsHandle;
};

// =============================================================
// Helper Functions
// =============================================================
int GetLayerNumMasks(AEGP_LayerH layerH);
std::shared_ptr<PyMask> GetLayerMaskByIndex(AEGP_LayerH layerH, int index);
std::shared_ptr<PyMask> AddMaskToLayer(AEGP_LayerH layerH);

} // namespace PyAE
