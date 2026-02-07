#pragma once

#include <pybind11/pybind11.h>

namespace py = pybind11;

// SDK Suite Initialization Functions
// Each function initializes bindings for a specific AEGP Suite

namespace PyAE {
namespace SDK {

// Constants
void init_Constants(py::module_& sdk);

// Project & Item Management
void init_ProjSuite(py::module_& sdk);
void init_ItemSuite(py::module_& sdk);
void init_ItemViewSuite(py::module_& sdk);
void init_CompSuite(py::module_& sdk);
void init_FootageSuite(py::module_& sdk);

// Layer Management
void init_LayerSuite(py::module_& sdk);
void init_EffectSuite(py::module_& sdk);

// Streams & Keyframes
void init_StreamSuite(py::module_& sdk);
void init_KeyframeSuite(py::module_& sdk);
void init_MarkerSuite(py::module_& sdk);
void init_StreamValueHelpers(py::module_& sdk);

// Masks
void init_MaskSuite(py::module_& sdk);
void init_MaskOutlineSuite(py::module_& sdk);

// Text
void init_TextDocumentSuite(py::module_& sdk);
void init_TextLayerSuite(py::module_& sdk);

// Rendering
void init_RenderSuite(py::module_& sdk);
void init_RenderOptionsSuite(py::module_& sdk);
void init_LayerRenderOptionsSuite(py::module_& sdk);
void init_WorldSuite(py::module_& sdk);
void init_CompositeSuite(py::module_& sdk);  // AEGP_CompositeSuite2
void init_RenderAsyncManagerSuite(py::module_& sdk);

// Render Queue
void init_RenderQueueSuite(py::module_& sdk);
void init_RQItemSuite(py::module_& sdk);
void init_RenderQueueMonitorSuite(py::module_& sdk);
void init_OutputModuleSuite(py::module_& sdk);

// Utilities
void init_UtilitySuite(py::module_& sdk);
void init_CommandSuite(py::module_& sdk);
void init_CollectionSuite(py::module_& sdk);

// Camera & Light
void init_CameraSuite(py::module_& sdk);
void init_LightSuite(py::module_& sdk);

// I/O
void init_IOInSuite(py::module_& sdk);
void init_IOOutSuite(py::module_& sdk);

// Other
void init_PersistentDataSuite(py::module_& sdk);
void init_ColorSettingsSuite(py::module_& sdk);
void init_SoundDataSuite(py::module_& sdk);
void init_IterateSuite(py::module_& sdk);

// Math
void init_MathSuite(py::module_& sdk);

// Memory
void init_MemorySuite(py::module_& sdk);

// PF Advanced Suites
void init_AdvAppSuite(py::module_& sdk);
void init_AdvItemSuite(py::module_& sdk);

// Note: QueryXformSuite excluded - Artisan plugin API, incompatible with AEGP
// See docs/development/QueryXformSuite_exclusion_rationale.md

} // namespace SDK
} // namespace PyAE
