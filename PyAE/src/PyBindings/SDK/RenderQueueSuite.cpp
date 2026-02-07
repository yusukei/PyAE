#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "StringUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_RenderQueueSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_RenderQueueSuite
    // -----------------------------------------------------------------------
    
    sdk.def("AEGP_AddCompToRenderQueue", [](uintptr_t compH_ptr, const std::string& path) {
        // Argument validation
        Validation::RequireNonNull(compH_ptr, "compH");
        Validation::RequireNonEmpty(path, "path");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueSuite) throw std::runtime_error("RenderQueue Suite not available");

        AEGP_CompH compH = reinterpret_cast<AEGP_CompH>(compH_ptr);

        A_Err err = suites.renderQueueSuite->AEGP_AddCompToRenderQueue(compH, path.c_str());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_AddCompToRenderQueue failed");
    }, py::arg("compH"), py::arg("path"),
       "Add a composition to the render queue with specified output path.");

    sdk.def("AEGP_SetRenderQueueState", [](int state) {
        auto& plugin_state = PyAE::PluginState::Instance();
        const auto& suites = plugin_state.GetSuites();
        if (!suites.renderQueueSuite) throw std::runtime_error("RenderQueue Suite not available");

        A_Err err = suites.renderQueueSuite->AEGP_SetRenderQueueState((AEGP_RenderQueueState)state);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetRenderQueueState failed");
    }, py::arg("state"));

    sdk.def("AEGP_GetRenderQueueState", []() -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderQueueSuite) throw std::runtime_error("RenderQueue Suite not available");

        AEGP_RenderQueueState rq_state = AEGP_RenderQueueState_STOPPED;
        
        A_Err err = suites.renderQueueSuite->AEGP_GetRenderQueueState(&rq_state);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetRenderQueueState failed");
        
        return (int)rq_state;
    });

    // Render Queue State Constants
    sdk.attr("AEGP_RenderQueueState_STOPPED") = (int)AEGP_RenderQueueState_STOPPED;
    sdk.attr("AEGP_RenderQueueState_PAUSED") = (int)AEGP_RenderQueueState_PAUSED;
    sdk.attr("AEGP_RenderQueueState_RENDERING") = (int)AEGP_RenderQueueState_RENDERING;
}

} // namespace SDK
} // namespace PyAE
