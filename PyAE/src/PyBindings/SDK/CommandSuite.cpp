#include <pybind11/pybind11.h>
#include "PluginState.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_CommandSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_CommandSuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetUniqueCommand", []() -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) throw std::runtime_error("Command Suite not available");

        AEGP_Command command = 0;
        A_Err err = suites.commandSuite->AEGP_GetUniqueCommand(&command);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetUniqueCommand failed");

        return (int)command;
    }, "Get a unique command ID");

    sdk.def("AEGP_InsertMenuCommand", [](int command, const std::string& name, int menu_id, int after_item) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) throw std::runtime_error("Command Suite not available");

        A_Err err = suites.commandSuite->AEGP_InsertMenuCommand(
            (AEGP_Command)command,
            name.c_str(),
            (AEGP_MenuID)menu_id,
            (A_long)after_item
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_InsertMenuCommand failed");
    }, py::arg("command"), py::arg("name"), py::arg("menu_id"), py::arg("after_item"),
    "Insert a menu command into the specified menu");

    sdk.def("AEGP_RemoveMenuCommand", [](int command) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) throw std::runtime_error("Command Suite not available");

        A_Err err = suites.commandSuite->AEGP_RemoveMenuCommand((AEGP_Command)command);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_RemoveMenuCommand failed");
    }, py::arg("command"),
    "Remove a menu command");

    sdk.def("AEGP_SetMenuCommandName", [](int command, const std::string& name) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) throw std::runtime_error("Command Suite not available");

        A_Err err = suites.commandSuite->AEGP_SetMenuCommandName(
            (AEGP_Command)command,
            name.c_str()
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMenuCommandName failed");
    }, py::arg("command"), py::arg("name"),
    "Set the name of a menu command");

    sdk.def("AEGP_EnableCommand", [](int command) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) throw std::runtime_error("Command Suite not available");

        A_Err err = suites.commandSuite->AEGP_EnableCommand((AEGP_Command)command);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_EnableCommand failed");
    }, py::arg("command"),
    "Enable a command");

    sdk.def("AEGP_DisableCommand", [](int command) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) throw std::runtime_error("Command Suite not available");

        A_Err err = suites.commandSuite->AEGP_DisableCommand((AEGP_Command)command);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisableCommand failed");
    }, py::arg("command"),
    "Disable a command");

    sdk.def("AEGP_CheckMarkMenuCommand", [](int command, bool check) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) throw std::runtime_error("Command Suite not available");

        A_Err err = suites.commandSuite->AEGP_CheckMarkMenuCommand(
            (AEGP_Command)command,
            check ? TRUE : FALSE
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CheckMarkMenuCommand failed");
    }, py::arg("command"), py::arg("check"),
    "Set or clear checkmark on a menu command");

    sdk.def("AEGP_DoCommand", [](int command) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) throw std::runtime_error("Command Suite not available");

        A_Err err = suites.commandSuite->AEGP_DoCommand((AEGP_Command)command);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DoCommand failed");
    }, py::arg("command"),
    "Execute a command");
}

} // namespace SDK
} // namespace PyAE
