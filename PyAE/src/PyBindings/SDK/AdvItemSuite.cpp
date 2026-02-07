#include <pybind11/pybind11.h>
#include "PluginState.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_AdvItemSuite(py::module_& sdk) {

    // PF_Step constants
    sdk.attr("PF_Step_FORWARD") = py::int_(static_cast<int>(PF_Step_FORWARD));
    sdk.attr("PF_Step_BACKWARD") = py::int_(static_cast<int>(PF_Step_BACKWARD));

    sdk.def("PF_MoveTimeStepActiveItem", [](int time_dir, int num_steps) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advItemSuite) {
            throw std::runtime_error("AdvItem Suite not available");
        }
        A_Err err = suites.advItemSuite->PF_MoveTimeStepActiveItem(
            static_cast<PF_Step>(time_dir), static_cast<A_long>(num_steps));
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_MoveTimeStepActiveItem failed: error " + std::to_string(err));
        }
    }, py::arg("time_dir"), py::arg("num_steps"),
    R"(Move the active item's time step

Args:
    time_dir: PF_Step_FORWARD (0) or PF_Step_BACKWARD (1)
    num_steps: Number of steps to move)");

    sdk.def("PF_TouchActiveItem", []() {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advItemSuite) {
            throw std::runtime_error("AdvItem Suite not available");
        }
        A_Err err = suites.advItemSuite->PF_TouchActiveItem();
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_TouchActiveItem failed: error " + std::to_string(err));
        }
    }, R"(Mark the active item as modified and trigger re-evaluation)");

    sdk.def("PF_EffectIsActiveOrEnabled", [](uintptr_t contextH_ptr) -> bool {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advItemSuite) {
            throw std::runtime_error("AdvItem Suite not available");
        }
        PF_ContextH contextH = reinterpret_cast<PF_ContextH>(contextH_ptr);
        PF_Boolean enabled = FALSE;
        A_Err err = suites.advItemSuite->PF_EffectIsActiveOrEnabled(contextH, &enabled);
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_EffectIsActiveOrEnabled failed: error " + std::to_string(err));
        }
        return enabled != 0;
    }, py::arg("contextH"),
    R"(Check if an effect is active or enabled)");

    // Note: PF_MoveTimeStep and PF_ForceRerender require PF_InData* and PF_EffectWorld*
    // which are only available in effect plugin context, not from AEGP context.
}

} // namespace SDK
} // namespace PyAE
