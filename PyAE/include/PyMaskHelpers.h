// PyMaskHelpers.h
// PyAE - Python for After Effects
// PyMask helper functions forward declarations

#pragma once

#include <memory>
#include "AE_Effect.h"
#include "AE_GeneralPlug.h"

namespace PyAE {

// Forward declaration only
class PyMask;

// Helper functions declared in PyMask.cpp
int GetLayerNumMasks(AEGP_LayerH layerH);
std::shared_ptr<PyMask> GetLayerMaskByIndex(AEGP_LayerH layerH, int index);
std::shared_ptr<PyMask> AddMaskToLayer(AEGP_LayerH layerH);

} // namespace PyAE
