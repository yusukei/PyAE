#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_ProjSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_ProjSuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetProjectByIndex", [](int index) -> uintptr_t {
        // Argument validation
        Validation::RequireNonNegative(index, "index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        AEGP_ProjectH projH = nullptr;
        A_Err err = suites.projSuite->AEGP_GetProjectByIndex(index, &projH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetProjectByIndex failed");

        return reinterpret_cast<uintptr_t>(projH);
    }, py::arg("index"));

    sdk.def("AEGP_GetProjectName", [](uintptr_t projH_ptr) -> std::string {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        A_char name[AEGP_MAX_PROJ_NAME_SIZE] = {0};
        A_Err err = suites.projSuite->AEGP_GetProjectName(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr), name);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetProjectName failed");

        // A_char はローカルエンコーディング (Shift-JIS等) なので UTF-8 に変換
        return PyAE::StringUtils::LocalToUtf8(name);
    }, py::arg("projH"),
    "Get the name of a project");

    sdk.def("AEGP_GetNumProjects", []() -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        A_long numProjs = 0;
        A_Err err = suites.projSuite->AEGP_GetNumProjects(&numProjs);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumProjects failed");

        return static_cast<int>(numProjs);
    }, "Get the number of open projects");

    sdk.def("AEGP_GetProjectPath", [](int plugin_id, uintptr_t projH_ptr) -> std::string {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        AEGP_MemHandle pathH = nullptr;
        A_Err err = suites.projSuite->AEGP_GetProjectPath(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr), &pathH);
        if (err != A_Err_NONE || !pathH) throw std::runtime_error("AEGP_GetProjectPath failed");

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, pathH);
        PyAE::ScopedMemLock lock(suites.memorySuite, pathH);
        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, py::arg("plugin_id"), py::arg("projH"),
    "Get the file path of a project (UTF-16)");

    sdk.def("AEGP_GetProjectRootFolder", [](uintptr_t projH_ptr) -> uintptr_t {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        AEGP_ItemH rootFolderH = nullptr;
        A_Err err = suites.projSuite->AEGP_GetProjectRootFolder(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr), &rootFolderH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetProjectRootFolder failed");

        return reinterpret_cast<uintptr_t>(rootFolderH);
    }, py::arg("projH"),
    "Get the root folder item of a project");

    sdk.def("AEGP_SaveProjectToPath", [](uintptr_t projH_ptr, const std::string& path) -> void {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }
        Validation::RequireNonEmpty(path, "path");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        std::wstring wpath = PyAE::StringUtils::Utf8ToWide(path);
        A_Err err = suites.projSuite->AEGP_SaveProjectToPath(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr),
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str()));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SaveProjectToPath failed");
    }, py::arg("projH"), py::arg("path"),
    "Save project to the specified path");

    sdk.def("AEGP_GetProjectTimeDisplay", [](uintptr_t projH_ptr) -> py::dict {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        AEGP_TimeDisplay3 timeDisplay;
        A_Err err = suites.projSuite->AEGP_GetProjectTimeDisplay(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr), &timeDisplay);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetProjectTimeDisplay failed");

        py::dict result;
        result["display_mode"] = static_cast<int>(timeDisplay.display_mode);
        result["footage_display_mode"] = static_cast<int>(timeDisplay.footage_display_mode);
        result["display_dropframe"] = static_cast<bool>(timeDisplay.display_dropframeB);
        result["use_feet_frames"] = static_cast<bool>(timeDisplay.use_feet_framesB);
        result["timebase"] = static_cast<int>(timeDisplay.timebaseC);
        result["frames_per_foot"] = static_cast<int>(timeDisplay.frames_per_footC);
        result["frames_display_mode"] = static_cast<int>(timeDisplay.frames_display_mode);
        return result;
    }, py::arg("projH"),
    "Get the time display settings of a project");

    sdk.def("AEGP_SetProjectTimeDisplay", [](uintptr_t projH_ptr, py::dict timeDisplay) -> void {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        AEGP_TimeDisplay3 td;
        td.display_mode = static_cast<AEGP_TimeDisplayMode>(timeDisplay["display_mode"].cast<int>());
        td.footage_display_mode = static_cast<AEGP_SourceTimecodeDisplayMode>(timeDisplay["footage_display_mode"].cast<int>());
        td.display_dropframeB = timeDisplay["display_dropframe"].cast<bool>() ? TRUE : FALSE;
        td.use_feet_framesB = timeDisplay["use_feet_frames"].cast<bool>() ? TRUE : FALSE;
        td.timebaseC = static_cast<A_char>(timeDisplay["timebase"].cast<int>());
        td.frames_per_footC = static_cast<A_char>(timeDisplay["frames_per_foot"].cast<int>());
        td.frames_display_mode = static_cast<AEGP_FramesDisplayMode>(timeDisplay["frames_display_mode"].cast<int>());

        A_Err err = suites.projSuite->AEGP_SetProjectTimeDisplay(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr), &td);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetProjectTimeDisplay failed");
    }, py::arg("projH"), py::arg("time_display"),
    "Set the time display settings of a project (UNDOABLE)");

    sdk.def("AEGP_ProjectIsDirty", [](uintptr_t projH_ptr) -> bool {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        A_Boolean isDirty = false;
        A_Err err = suites.projSuite->AEGP_ProjectIsDirty(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr), &isDirty);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ProjectIsDirty failed");

        return static_cast<bool>(isDirty);
    }, py::arg("projH"),
    "Check if a project has been modified since last save");

    sdk.def("AEGP_SaveProjectAs", [](uintptr_t projH_ptr, const std::string& path) -> void {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }
        Validation::RequireNonEmpty(path, "path");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        std::wstring wpath = PyAE::StringUtils::Utf8ToWide(path);
        A_Err err = suites.projSuite->AEGP_SaveProjectAs(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr),
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str()));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SaveProjectAs failed");
    }, py::arg("projH"), py::arg("path"),
    "Save project with a new path");

    sdk.def("AEGP_NewProject", []() -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        AEGP_ProjectH newProjH = nullptr;
        A_Err err = suites.projSuite->AEGP_NewProject(&newProjH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NewProject failed");

        return reinterpret_cast<uintptr_t>(newProjH);
    }, "Create a new project");

    sdk.def("AEGP_OpenProjectFromPath", [](const std::string& path) -> uintptr_t {
        // Argument validation
        Validation::RequireNonEmpty(path, "path");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        std::wstring wpath = PyAE::StringUtils::Utf8ToWide(path);
        AEGP_ProjectH projH = nullptr;
        A_Err err = suites.projSuite->AEGP_OpenProjectFromPath(
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str()), &projH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_OpenProjectFromPath failed");

        return reinterpret_cast<uintptr_t>(projH);
    }, py::arg("path"),
    "Open a project from file path (WARNING: Will close any open projects)");

    sdk.def("AEGP_GetProjectBitDepth", [](uintptr_t projH_ptr) -> int {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        AEGP_ProjBitDepth bitDepth;
        A_Err err = suites.projSuite->AEGP_GetProjectBitDepth(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr), &bitDepth);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetProjectBitDepth failed");

        // Convert enum to user-friendly bit depth values
        switch (bitDepth) {
            case AEGP_ProjBitDepth_8:  return 8;
            case AEGP_ProjBitDepth_16: return 16;
            case AEGP_ProjBitDepth_32: return 32;
            default: throw std::runtime_error("Unknown bit depth value");
        }
    }, py::arg("projH"),
    "Get the bit depth of a project (8, 16, or 32)");

    sdk.def("AEGP_SetProjectBitDepth", [](uintptr_t projH_ptr, int bitDepth) -> void {
        // Argument validation
        if (projH_ptr == 0) {
            throw std::invalid_argument("projH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.projSuite) throw std::runtime_error("Project Suite not available");

        // Convert user-friendly bit depth to enum value
        AEGP_ProjBitDepth projBitDepth;
        switch (bitDepth) {
            case 8:  projBitDepth = AEGP_ProjBitDepth_8; break;
            case 16: projBitDepth = AEGP_ProjBitDepth_16; break;
            case 32: projBitDepth = AEGP_ProjBitDepth_32; break;
            default:
                throw std::invalid_argument("Invalid bit_depth (must be 8, 16, or 32)");
        }

        A_Err err = suites.projSuite->AEGP_SetProjectBitDepth(
            reinterpret_cast<AEGP_ProjectH>(projH_ptr), projBitDepth);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetProjectBitDepth failed");
    }, py::arg("projH"), py::arg("bit_depth"),
    "Set the bit depth of a project (UNDOABLE)");
}

} // namespace SDK
} // namespace PyAE
