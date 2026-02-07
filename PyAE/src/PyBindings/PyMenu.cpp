// PyMenu.cpp
// PyAE - Python for After Effects
// High-level API for menu commands with Python callback support

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "PluginState.h"
#include "PyMenuClasses.h"
#include "Logger.h"

#include <unordered_map>
#include <optional>

namespace py = pybind11;

namespace PyAE {

// =============================================================
// MenuCallbackBridge implementation
// =============================================================
bool MenuCallbackBridge::InvokeCallback(AEGP_Command cmdId) {
    MenuCallback callback;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_callbacks.find(cmdId);
        if (it == m_callbacks.end()) {
            return false;
        }
        callback = it->second;
    }

    // Invoke callback with GIL
    try {
        py::gil_scoped_acquire gil;
        callback();
        return true;
    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("Menu", "Python callback error: " + std::string(e.what()));
        return true;  // Still handled, just errored
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("Menu", "Callback error: " + std::string(e.what()));
        return true;
    }
}

std::optional<bool> MenuCallbackBridge::InvokeUpdateCallback(AEGP_Command cmdId) {
    UpdateMenuCallback callback;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_updateCallbacks.find(cmdId);
        if (it == m_updateCallbacks.end()) {
            return std::nullopt;
        }
        callback = it->second;
    }

    // Invoke callback with GIL
    try {
        py::gil_scoped_acquire gil;
        return callback();
    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("Menu", "Python update callback error: " + std::string(e.what()));
        return true;  // Default to enabled on error
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("Menu", "Update callback error: " + std::string(e.what()));
        return true;
    }
}

// =============================================================
// PyMenuCommand - Represents a menu command
// =============================================================
class PyMenuCommand {
public:
    PyMenuCommand(AEGP_Command cmdId, const std::string& name, int menuId)
        : m_cmdId(cmdId), m_name(name), m_menuId(menuId), m_enabled(true), m_checked(false) {}

    // Properties
    bool GetEnabled() const { return m_enabled; }

    void SetEnabled(bool enabled) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) {
            throw std::runtime_error("Command Suite not available");
        }

        A_Err err;
        if (enabled) {
            err = suites.commandSuite->AEGP_EnableCommand(m_cmdId);
        } else {
            err = suites.commandSuite->AEGP_DisableCommand(m_cmdId);
        }

        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set command enabled state");
        }
        m_enabled = enabled;
    }

    bool GetChecked() const { return m_checked; }

    void SetChecked(bool checked) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) {
            throw std::runtime_error("Command Suite not available");
        }

        A_Err err = suites.commandSuite->AEGP_CheckMarkMenuCommand(
            m_cmdId, checked ? TRUE : FALSE);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set command check mark");
        }
        m_checked = checked;
    }

    std::string GetName() const { return m_name; }

    void SetName(const std::string& name) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) {
            throw std::runtime_error("Command Suite not available");
        }

        A_Err err = suites.commandSuite->AEGP_SetMenuCommandName(m_cmdId, name.c_str());
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set command name");
        }
        m_name = name;
    }

    int GetCommandId() const { return static_cast<int>(m_cmdId); }
    int GetMenuId() const { return m_menuId; }

    // Set click callback
    void SetOnClick(std::optional<std::function<void()>> callback) {
        if (callback) {
            MenuCallbackBridge::Instance().RegisterCallback(m_cmdId, *callback);
        } else {
            MenuCallbackBridge::Instance().UnregisterCallback(m_cmdId);
        }
    }

    // Set update menu callback
    void SetOnUpdate(std::optional<std::function<bool()>> callback) {
        if (callback) {
            MenuCallbackBridge::Instance().RegisterUpdateCallback(m_cmdId, *callback);
        }
    }

    // Execute the command
    void Execute() {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) {
            throw std::runtime_error("Command Suite not available");
        }

        A_Err err = suites.commandSuite->AEGP_DoCommand(m_cmdId);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to execute command");
        }
    }

    // Remove the command from menu
    void Remove() {
        // Unregister callback first
        MenuCallbackBridge::Instance().UnregisterCallback(m_cmdId);

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) {
            throw std::runtime_error("Command Suite not available");
        }

        A_Err err = suites.commandSuite->AEGP_RemoveMenuCommand(m_cmdId);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to remove command");
        }
    }

    // String representation
    std::string Repr() const {
        return "MenuCommand(id=" + std::to_string(m_cmdId) +
               ", name='" + m_name + "')";
    }

private:
    AEGP_Command m_cmdId;
    std::string m_name;
    int m_menuId;
    bool m_enabled;
    bool m_checked;
};

// =============================================================
// MenuRegistry - Singleton for managing registered commands
// =============================================================
class MenuRegistry {
public:
    static MenuRegistry& Instance() {
        static MenuRegistry instance;
        return instance;
    }

    // Register a new menu command with optional callback
    PyMenuCommand Register(const std::string& name, int menuId,
                          std::optional<std::function<void()>> callback = std::nullopt,
                          int afterItem = -1) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) {
            throw std::runtime_error("Command Suite not available");
        }

        // Get unique command ID
        AEGP_Command cmdId = 0;
        A_Err err = suites.commandSuite->AEGP_GetUniqueCommand(&cmdId);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get unique command ID");
        }

        // Insert menu command
        err = suites.commandSuite->AEGP_InsertMenuCommand(
            cmdId, name.c_str(), static_cast<AEGP_MenuID>(menuId), static_cast<A_long>(afterItem));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to insert menu command");
        }

        // Register callback if provided
        if (callback) {
            MenuCallbackBridge::Instance().RegisterCallback(cmdId, *callback);
        }

        PYAE_LOG_INFO("Menu", "Registered command '" + name + "' (ID: " + std::to_string(cmdId) + ")");

        return PyMenuCommand(cmdId, name, menuId);
    }

    // Execute a command by ID
    void Execute(int commandId) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) {
            throw std::runtime_error("Command Suite not available");
        }

        A_Err err = suites.commandSuite->AEGP_DoCommand(static_cast<AEGP_Command>(commandId));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to execute command: " + std::to_string(commandId));
        }
    }

    // Get a unique command ID
    int GetUniqueId() {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.commandSuite) {
            throw std::runtime_error("Command Suite not available");
        }

        AEGP_Command cmdId = 0;
        A_Err err = suites.commandSuite->AEGP_GetUniqueCommand(&cmdId);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to get unique command ID");
        }

        return static_cast<int>(cmdId);
    }

private:
    MenuRegistry() = default;
};

} // namespace PyAE

// =============================================================
// Python bindings
// =============================================================
void init_menu(py::module_& m) {
    // Create submodule
    py::module_ menu = m.def_submodule("menu", "Menu command management with callback support");

    // MenuCommand class
    py::class_<PyAE::PyMenuCommand>(menu, "Command",
        R"doc(
Represents a menu command.

A menu command can be created using register_command() and can have
a Python callback that is invoked when the menu item is clicked.

Example:
    def on_click():
        print("Menu item clicked!")

    cmd = ae.menu.register_command("My Script", ae.menu.WINDOW, on_click)
    cmd.enabled = True
    cmd.checked = False
)doc")
        .def_property("enabled", &PyAE::PyMenuCommand::GetEnabled, &PyAE::PyMenuCommand::SetEnabled,
             "Whether the command is enabled (clickable)")
        .def_property("checked", &PyAE::PyMenuCommand::GetChecked, &PyAE::PyMenuCommand::SetChecked,
             "Whether the command has a check mark")
        .def_property("name", &PyAE::PyMenuCommand::GetName, &PyAE::PyMenuCommand::SetName,
             "The display name of the command")
        .def_property_readonly("command_id", &PyAE::PyMenuCommand::GetCommandId,
             "The unique command ID")
        .def_property_readonly("menu_id", &PyAE::PyMenuCommand::GetMenuId,
             "The menu ID where this command is located")
        .def("set_on_click", &PyAE::PyMenuCommand::SetOnClick,
             R"doc(
Set the callback function for when the menu item is clicked.

Args:
    callback: A function taking no arguments, or None to clear.

Example:
    def my_handler():
        print("Clicked!")

    cmd.set_on_click(my_handler)
    cmd.set_on_click(None)  # Clear callback
)doc", py::arg("callback"))
        .def("set_on_update", &PyAE::PyMenuCommand::SetOnUpdate,
             R"doc(
Set the callback for menu update (determines enabled state).

Args:
    callback: A function returning bool (True=enabled), or None to clear.

Example:
    def should_enable():
        return ae.project.active_item is not None

    cmd.set_on_update(should_enable)
)doc", py::arg("callback"))
        .def("execute", &PyAE::PyMenuCommand::Execute,
             "Execute the command programmatically")
        .def("remove", &PyAE::PyMenuCommand::Remove,
             "Remove the command from the menu")
        .def("__repr__", &PyAE::PyMenuCommand::Repr);

    // Menu ID constants (directly on submodule)
    menu.attr("NONE") = 0;
    menu.attr("APPLE") = 1;
    menu.attr("FILE") = 2;
    menu.attr("EDIT") = 3;
    menu.attr("COMPOSITION") = 4;
    menu.attr("LAYER") = 5;
    menu.attr("EFFECT") = 6;
    menu.attr("WINDOW") = 7;
    menu.attr("FLOATERS") = 8;
    menu.attr("KF_ASSIST") = 9;
    menu.attr("IMPORT") = 10;
    menu.attr("SAVE_FRAME_AS") = 11;
    menu.attr("PREFS") = 12;
    menu.attr("EXPORT") = 13;
    menu.attr("ANIMATION") = 14;
    menu.attr("PURGE") = 15;
    menu.attr("NEW") = 16;

    // Main registration function with callback support
    menu.def("register_command",
        [](const std::string& name, int menuId,
           std::optional<std::function<void()>> callback, int afterItem) {
            return PyAE::MenuRegistry::Instance().Register(name, menuId, callback, afterItem);
        },
        R"doc(
Register a new menu command with optional callback.

Args:
    name: The display name of the menu item
    menu_id: The menu to add to (e.g., ae.menu.WINDOW)
    callback: Optional function to call when clicked
    after_item: Position hint (-1 for sorted)

Returns:
    Command: The created menu command object

Example:
    # Simple registration
    cmd = ae.menu.register_command("My Script", ae.menu.WINDOW)

    # With callback
    def on_click():
        print("Clicked!")

    cmd = ae.menu.register_command("Run My Script", ae.menu.WINDOW, on_click)
)doc",
        py::arg("name"), py::arg("menu_id"),
        py::arg("callback") = py::none(), py::arg("after_item") = -1);

    menu.def("execute", [](int commandId) {
        PyAE::MenuRegistry::Instance().Execute(commandId);
    }, "Execute a command by ID",
       py::arg("command_id"));

    menu.def("get_unique_id", []() {
        return PyAE::MenuRegistry::Instance().GetUniqueId();
    }, "Get a unique command ID");

    // Convenience function to insert command into specific menu
    menu.def("add_to_file",
        [](const std::string& name, std::optional<std::function<void()>> callback, int afterItem) {
            return PyAE::MenuRegistry::Instance().Register(name, 2, callback, afterItem);
        }, "Add a command to the File menu",
        py::arg("name"), py::arg("callback") = py::none(), py::arg("after_item") = -1);

    menu.def("add_to_edit",
        [](const std::string& name, std::optional<std::function<void()>> callback, int afterItem) {
            return PyAE::MenuRegistry::Instance().Register(name, 3, callback, afterItem);
        }, "Add a command to the Edit menu",
        py::arg("name"), py::arg("callback") = py::none(), py::arg("after_item") = -1);

    menu.def("add_to_composition",
        [](const std::string& name, std::optional<std::function<void()>> callback, int afterItem) {
            return PyAE::MenuRegistry::Instance().Register(name, 4, callback, afterItem);
        }, "Add a command to the Composition menu",
        py::arg("name"), py::arg("callback") = py::none(), py::arg("after_item") = -1);

    menu.def("add_to_layer",
        [](const std::string& name, std::optional<std::function<void()>> callback, int afterItem) {
            return PyAE::MenuRegistry::Instance().Register(name, 5, callback, afterItem);
        }, "Add a command to the Layer menu",
        py::arg("name"), py::arg("callback") = py::none(), py::arg("after_item") = -1);

    menu.def("add_to_effect",
        [](const std::string& name, std::optional<std::function<void()>> callback, int afterItem) {
            return PyAE::MenuRegistry::Instance().Register(name, 6, callback, afterItem);
        }, "Add a command to the Effect menu",
        py::arg("name"), py::arg("callback") = py::none(), py::arg("after_item") = -1);

    menu.def("add_to_window",
        [](const std::string& name, std::optional<std::function<void()>> callback, int afterItem) {
            return PyAE::MenuRegistry::Instance().Register(name, 7, callback, afterItem);
        }, "Add a command to the Window menu",
        py::arg("name"), py::arg("callback") = py::none(), py::arg("after_item") = -1);

    menu.def("add_to_animation",
        [](const std::string& name, std::optional<std::function<void()>> callback, int afterItem) {
            return PyAE::MenuRegistry::Instance().Register(name, 14, callback, afterItem);
        }, "Add a command to the Animation menu",
        py::arg("name"), py::arg("callback") = py::none(), py::arg("after_item") = -1);

    // Utility functions
    menu.def("clear_all_callbacks", []() {
        PyAE::MenuCallbackBridge::Instance().ClearAllCallbacks();
    }, "Clear all registered menu callbacks");

    menu.def("get_registered_command_ids", []() {
        return PyAE::MenuCallbackBridge::Instance().GetRegisteredCommands();
    }, "Get list of command IDs with registered callbacks");

    // Module documentation
    menu.attr("__doc__") = R"doc(
Menu Command API with Python Callback Support

This module allows you to create custom menu items in After Effects
and handle their clicks with Python callbacks.

Menu IDs:
    NONE (0), APPLE (1), FILE (2), EDIT (3), COMPOSITION (4),
    LAYER (5), EFFECT (6), WINDOW (7), FLOATERS (8), KF_ASSIST (9),
    IMPORT (10), SAVE_FRAME_AS (11), PREFS (12), EXPORT (13),
    ANIMATION (14), PURGE (15), NEW (16)

Example:
    import ae

    # Create a menu item with callback
    def my_script():
        print("Running my script!")
        comp = ae.project.active_item
        if comp:
            print(f"Active comp: {comp.name}")

    cmd = ae.menu.register_command("My Script", ae.menu.WINDOW, my_script)

    # Control appearance
    cmd.enabled = True
    cmd.checked = False
    cmd.name = "New Name"

    # Change callback later
    cmd.set_on_click(lambda: print("New handler!"))

    # Remove when done
    cmd.remove()
)doc";
}
