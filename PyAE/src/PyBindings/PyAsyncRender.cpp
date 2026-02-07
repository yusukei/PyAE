// PyAsyncRender.cpp
// PyAE - Python for After Effects
// High-level API for async rendering (custom UI plugins)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "PluginState.h"
#include "Logger.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// AsyncRender - Wrapper for async rendering functions
// =============================================================

// Render an item frame asynchronously
uintptr_t RenderItemFrameAsync(uintptr_t asyncManagerPtr, uint64_t purposeId, uintptr_t renderOptionsPtr) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.renderAsyncManagerSuite) {
        throw std::runtime_error("RenderAsyncManager Suite not available");
    }

    if (asyncManagerPtr == 0) {
        throw std::runtime_error("Invalid async manager pointer (null). "
            "This API can only be used in PF_Event_DRAW context with "
            "PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER flag set.");
    }

    if (renderOptionsPtr == 0) {
        throw std::runtime_error("Invalid render options pointer (null)");
    }

    AEGP_FrameReceiptH receiptH = nullptr;
    A_Err err = suites.renderAsyncManagerSuite->AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
        reinterpret_cast<PF_AsyncManagerP>(asyncManagerPtr),
        static_cast<AEGP_AsyncRequestId>(purposeId),
        reinterpret_cast<AEGP_RenderOptionsH>(renderOptionsPtr),
        &receiptH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to checkout/render item frame asynchronously");
    }

    return reinterpret_cast<uintptr_t>(receiptH);
}

// Render a layer frame asynchronously
uintptr_t RenderLayerFrameAsync(uintptr_t asyncManagerPtr, uint64_t purposeId, uintptr_t layerRenderOptionsPtr) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.renderAsyncManagerSuite) {
        throw std::runtime_error("RenderAsyncManager Suite not available");
    }

    if (asyncManagerPtr == 0) {
        throw std::runtime_error("Invalid async manager pointer (null). "
            "This API can only be used in PF_Event_DRAW context with "
            "PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER flag set.");
    }

    if (layerRenderOptionsPtr == 0) {
        throw std::runtime_error("Invalid layer render options pointer (null)");
    }

    AEGP_FrameReceiptH receiptH = nullptr;
    A_Err err = suites.renderAsyncManagerSuite->AEGP_CheckoutOrRender_LayerFrame_AsyncManager(
        reinterpret_cast<PF_AsyncManagerP>(asyncManagerPtr),
        static_cast<AEGP_AsyncRequestId>(purposeId),
        reinterpret_cast<AEGP_LayerRenderOptionsH>(layerRenderOptionsPtr),
        &receiptH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to checkout/render layer frame asynchronously");
    }

    return reinterpret_cast<uintptr_t>(receiptH);
}

} // namespace PyAE

// =============================================================
// Python bindings
// =============================================================
void init_async_render(py::module_& m) {
    // Create submodule
    py::module_ ar = m.def_submodule("async_render", "Async rendering for custom UI plugins");

    // Render functions
    ar.def("render_item_frame", &PyAE::RenderItemFrameAsync,
        R"doc(
Render an item frame asynchronously.

IMPORTANT: This function can ONLY be used in PF_Event_DRAW context
with PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER flag set in your effect plugin.

Args:
    async_manager: PF_AsyncManagerP pointer (from PF_Event_DRAW extra)
    purpose_id: Unique ID for this render request. Requests with the same
                purpose_id will auto-cancel older requests.
    render_options: AEGP_RenderOptionsH handle

Returns:
    AEGP_FrameReceiptH handle. Use AEGP_GetReceiptWorld() to get the pixel data.
    Note: The receipt may not contain pixel data immediately (rendering in progress).

Example (C++ effect plugin):
    // In your PF_Event_DRAW handler:
    PF_AsyncManagerP async_mgr = extra->u.draw.async_managerP;

    // Pass to Python:
    py::module_ ae = py::module_::import("ae");
    auto receipt = ae.attr("async_render").attr("render_item_frame")(
        reinterpret_cast<uintptr_t>(async_mgr),
        comp_id,  // purpose_id
        reinterpret_cast<uintptr_t>(render_options)
    );
)doc",
        py::arg("async_manager"), py::arg("purpose_id"), py::arg("render_options"));

    ar.def("render_layer_frame", &PyAE::RenderLayerFrameAsync,
        R"doc(
Render a layer frame asynchronously.

IMPORTANT: This function can ONLY be used in PF_Event_DRAW context
with PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER flag set in your effect plugin.

Args:
    async_manager: PF_AsyncManagerP pointer (from PF_Event_DRAW extra)
    purpose_id: Unique ID for this render request. Requests with the same
                purpose_id will auto-cancel older requests.
    layer_render_options: AEGP_LayerRenderOptionsH handle

Returns:
    AEGP_FrameReceiptH handle. Use AEGP_GetReceiptWorld() to get the pixel data.
    Note: The receipt may not contain pixel data immediately (rendering in progress).

Example (C++ effect plugin):
    // In your PF_Event_DRAW handler:
    PF_AsyncManagerP async_mgr = extra->u.draw.async_managerP;

    // Pass to Python:
    py::module_ ae = py::module_::import("ae");
    auto receipt = ae.attr("async_render").attr("render_layer_frame")(
        reinterpret_cast<uintptr_t>(async_mgr),
        layer_id,  // purpose_id
        reinterpret_cast<uintptr_t>(layer_render_options)
    );
)doc",
        py::arg("async_manager"), py::arg("purpose_id"), py::arg("layer_render_options"));

    // Module documentation
    ar.attr("__doc__") = R"doc(
Async Render API for Custom UI Plugins

This API provides non-blocking frame rendering for effect plugins with custom UI.

REQUIREMENTS:
1. Your effect plugin must set PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER flag
2. These functions can ONLY be called during PF_Event_DRAW events
3. The async_manager parameter comes from PF_Event_DRAW's extra data

PURPOSE ID:
The purpose_id parameter is used to identify render requests. When a new
request is made with the same purpose_id, older requests are automatically
cancelled. This prevents render requests from piling up when the user
scrubs the timeline quickly.

Typical usage pattern:
- Use comp_id as purpose_id for composition-level rendering
- Use layer_id as purpose_id for layer-level rendering

LIMITATIONS:
- Cannot be used outside of PF_Event_DRAW context
- Requires C++ effect plugin to pass the async_manager pointer
- The returned receipt may not have pixel data immediately

For most Python automation tasks, use the synchronous render APIs in ae.sdk instead:
- ae.sdk.AEGP_RenderAndCheckoutFrame()
- ae.sdk.AEGP_RenderAndCheckoutLayerFrame()
)doc";
}
