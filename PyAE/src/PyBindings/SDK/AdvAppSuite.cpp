#include <pybind11/pybind11.h>
#include "PluginState.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_AdvAppSuite(py::module_& sdk) {

    sdk.def("PF_SetProjectDirty", []() {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_SetProjectDirty();
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_SetProjectDirty failed: error " + std::to_string(err));
        }
    }, R"(Mark the project as dirty (unsaved changes))");

    sdk.def("PF_SaveProject", []() {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_SaveProject();
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_SaveProject failed: error " + std::to_string(err));
        }
    }, R"(Save the current project)");

    sdk.def("PF_SaveBackgroundState", []() {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_SaveBackgroundState();
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_SaveBackgroundState failed: error " + std::to_string(err));
        }
    }, R"(Save the background state)");

    sdk.def("PF_ForceForeground", []() {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_ForceForeground();
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_ForceForeground failed: error " + std::to_string(err));
        }
    }, R"(Bring After Effects to the foreground)");

    sdk.def("PF_RestoreBackgroundState", []() {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_RestoreBackgroundState();
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_RestoreBackgroundState failed: error " + std::to_string(err));
        }
    }, R"(Restore the background state)");

    sdk.def("PF_RefreshAllWindows", []() {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_RefreshAllWindows();
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_RefreshAllWindows failed: error " + std::to_string(err));
        }
    }, R"(Force refresh all windows including comp viewers)");

    sdk.def("PF_InfoDrawText", [](const std::string& line1, const std::string& line2) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_InfoDrawText(
            line1.c_str(), line2.c_str());
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_InfoDrawText failed: error " + std::to_string(err));
        }
    }, py::arg("line1"), py::arg("line2"),
    R"(Display text in the info panel (2 lines))");

    sdk.def("PF_InfoDrawText3", [](const std::string& line1, const std::string& line2, const std::string& line3) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_InfoDrawText3(
            line1.c_str(), line2.c_str(), line3.c_str());
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_InfoDrawText3 failed: error " + std::to_string(err));
        }
    }, py::arg("line1"), py::arg("line2"), py::arg("line3"),
    R"(Display text in the info panel (3 lines))");

    sdk.def("PF_AppendInfoText", [](const std::string& text) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_AppendInfoText(text.c_str());
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_AppendInfoText failed: error " + std::to_string(err));
        }
    }, py::arg("text"),
    R"(Append text to the info panel)");

    sdk.def("PF_InfoDrawColor", [](int red, int green, int blue, int alpha) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        PF_Pixel color;
        color.red = static_cast<A_u_char>(red);
        color.green = static_cast<A_u_char>(green);
        color.blue = static_cast<A_u_char>(blue);
        color.alpha = static_cast<A_u_char>(alpha);
        A_Err err = suites.advAppSuite->PF_InfoDrawColor(color);
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_InfoDrawColor failed: error " + std::to_string(err));
        }
    }, py::arg("red"), py::arg("green"), py::arg("blue"), py::arg("alpha") = 255,
    R"(Display a color in the info panel)");

    sdk.def("PF_InfoDrawText3Plus", [](const std::string& line1,
                                       const std::string& line2_jr, const std::string& line2_jl,
                                       const std::string& line3_jr, const std::string& line3_jl) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }
        A_Err err = suites.advAppSuite->PF_InfoDrawText3Plus(
            line1.c_str(), line2_jr.c_str(), line2_jl.c_str(),
            line3_jr.c_str(), line3_jl.c_str());
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_InfoDrawText3Plus failed: error " + std::to_string(err));
        }
    }, py::arg("line1"), py::arg("line2_jr"), py::arg("line2_jl"),
       py::arg("line3_jr"), py::arg("line3_jl"),
    R"(Display formatted text in the info panel (right/left justified))");
}

} // namespace SDK
} // namespace PyAE
