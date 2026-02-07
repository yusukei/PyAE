#include <pybind11/pybind11.h>
#include "SDK/SDKBindings.h"

namespace py = pybind11;

void init_sdk(py::module_& m) {
    py::module_ sdk = m.def_submodule("sdk", "Low-level Access to After Effects SDK Suites");

    // Initialize all SDK Suite bindings
    // Each suite is defined in its own file under SDK/

    // Constants (AEGP_LayerStream, AEGP_MarkerString, AEGP_LTimeMode, etc.)
    PyAE::SDK::init_Constants(sdk);

    // Utility Suite & Plugin ID Helper
    PyAE::SDK::init_UtilitySuite(sdk);

    // Project & Item Management
    PyAE::SDK::init_ProjSuite(sdk);
    PyAE::SDK::init_ItemSuite(sdk);
    PyAE::SDK::init_ItemViewSuite(sdk);
    PyAE::SDK::init_CompSuite(sdk);
    PyAE::SDK::init_FootageSuite(sdk);

    // Layer Management
    PyAE::SDK::init_LayerSuite(sdk);
    PyAE::SDK::init_EffectSuite(sdk);

    // Streams & Keyframes
    // SDK APIバージョン問題は2026-02-01に解決済み（plugin_id追加、MemHandle処理実装完了）
    PyAE::SDK::init_StreamSuite(sdk);
    PyAE::SDK::init_KeyframeSuite(sdk);
    PyAE::SDK::init_MarkerSuite(sdk);
    PyAE::SDK::init_StreamValueHelpers(sdk);

    // Masks
    PyAE::SDK::init_MaskSuite(sdk);
    PyAE::SDK::init_MaskOutlineSuite(sdk);

    // Text
    PyAE::SDK::init_TextDocumentSuite(sdk);
    PyAE::SDK::init_TextLayerSuite(sdk);

    // Render Queue
    PyAE::SDK::init_RenderQueueSuite(sdk);
    PyAE::SDK::init_RQItemSuite(sdk);
    PyAE::SDK::init_OutputModuleSuite(sdk);
    PyAE::SDK::init_RenderQueueMonitorSuite(sdk);

    // Utilities
    PyAE::SDK::init_CommandSuite(sdk);
    PyAE::SDK::init_CollectionSuite(sdk);

    // Camera & Light - Testing Camera + Light
    PyAE::SDK::init_CameraSuite(sdk);
    PyAE::SDK::init_LightSuite(sdk);  // Test Light Suite

    // I/O - Testing both IOInSuite and IOOutSuite
    PyAE::SDK::init_IOInSuite(sdk);
    PyAE::SDK::init_IOOutSuite(sdk);  // Testing IOOutSuite

    // Sound Data
    PyAE::SDK::init_SoundDataSuite(sdk);

    // Color Settings
    PyAE::SDK::init_ColorSettingsSuite(sdk);

    // Persistent Data (Preferences)
    PyAE::SDK::init_PersistentDataSuite(sdk);

    // Math (Matrix operations)
    PyAE::SDK::init_MathSuite(sdk);

    // World Suite (frame buffers)
    PyAE::SDK::init_WorldSuite(sdk);

    // Composite Suite (image compositing)
    PyAE::SDK::init_CompositeSuite(sdk);

    // Iterate Suite (Multi-threaded iteration)
    PyAE::SDK::init_IterateSuite(sdk);

    // Render Async Manager Suite
    PyAE::SDK::init_RenderAsyncManagerSuite(sdk);

    // Render Options Suite
    PyAE::SDK::init_RenderOptionsSuite(sdk);

    // Layer Render Options Suite
    PyAE::SDK::init_LayerRenderOptionsSuite(sdk);

    // Render Suite (frame/sound rendering)
    PyAE::SDK::init_RenderSuite(sdk);

    // Memory Suite (memory management)
    PyAE::SDK::init_MemorySuite(sdk);

    // PF Advanced Suites (Refresh/Recalculation)
    PyAE::SDK::init_AdvAppSuite(sdk);
    PyAE::SDK::init_AdvItemSuite(sdk);

    // Note: RegisterSuite excluded - Effect plugin registration API, requires C callbacks
    // Note: QueryXformSuite excluded - Artisan plugin API only
    // Note: TrackerSuite1 excluded - PT_TrackingContextPtr only available in tracker plugin callbacks
    // Note: CanvasSuite8 excluded - PR_RenderContextH only available in Artisan renderer callbacks
    // Note: ComputeCacheSuite1 excluded - Requires C callback function pointers
    // Note: FIMSuite4 excluded - Requires C callback function pointers
    // Note: ArtisanUtilSuite1 excluded - Artisan plugin API only
    // Note: PFInterfaceSuite1 excluded - Effect plugin API only (PF_ProgPtr required)
    // See docs/development/aegp_suites_status.md for details
}
