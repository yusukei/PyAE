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

void init_UtilitySuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_UtilitySuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_StartUndoGroup", [](const std::string& undo_name) {
        Validation::RequireNonEmpty(undo_name, "undo_name");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Err err = suites.utilitySuite->AEGP_StartUndoGroup(undo_name.c_str());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_StartUndoGroup failed");
    }, py::arg("undo_name"));

    sdk.def("AEGP_EndUndoGroup", []() {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Err err = suites.utilitySuite->AEGP_EndUndoGroup();
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_EndUndoGroup failed");
    });

    // -----------------------------------------------------------------------
    // AEGP_ExecuteScript - Execute ExtendScript code
    // -----------------------------------------------------------------------

    sdk.def("AEGP_ExecuteScript", [](const std::string& script) -> std::string {
        Validation::RequireNonEmpty(script, "script");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_MemHandle resultH = nullptr;
        AEGP_MemHandle errorH = nullptr;

        A_Err err = suites.utilitySuite->AEGP_ExecuteScript(
            state.GetPluginID(),
            script.c_str(),
            TRUE,  // platform_encoding
            &resultH,
            &errorH
        );

        std::string result;
        std::string errorStr;

        // Get error string if available (RAII helpers ensure proper cleanup)
        if (errorH) {
            PyAE::ScopedMemHandle scopedErrorHandle(state.GetPluginID(), suites.memorySuite, errorH);
            PyAE::ScopedMemLock errorLock(suites.memorySuite, errorH);
            if (errorLock.IsValid()) {
                errorStr = errorLock.As<A_char>();
            }
        }

        // Get result string if available
        if (resultH) {
            PyAE::ScopedMemHandle scopedResultHandle(state.GetPluginID(), suites.memorySuite, resultH);
            PyAE::ScopedMemLock resultLock(suites.memorySuite, resultH);
            if (resultLock.IsValid()) {
                result = resultLock.As<A_char>();
            }
        }

        if (err != A_Err_NONE || !errorStr.empty()) {
            throw std::runtime_error("ExtendScript error: " + errorStr);
        }

        return result;
    }, py::arg("script"),
    "Execute ExtendScript code and return the result as a string");

    // -----------------------------------------------------------------------
    // Plugin ID Helper
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetPluginID", []() -> int {
        return PyAE::PluginState::Instance().GetPluginID();
    }, "Get the plugin ID required for many SDK calls");

    // -----------------------------------------------------------------------
    // Report/Dialog Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_ReportInfo", [](int plugin_id, const std::string& info_string) -> void {
        Validation::RequireNonEmpty(info_string, "info_string");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Err err = suites.utilitySuite->AEGP_ReportInfo(
            (AEGP_PluginID)plugin_id,
            info_string.c_str()
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ReportInfo failed");
    }, py::arg("plugin_id"), py::arg("info_string"),
    "Display an information dialog with the plugin name and message");

    sdk.def("AEGP_ReportInfoUnicode", [](int plugin_id, const std::string& info_string) -> void {
        Validation::RequireNonEmpty(info_string, "info_string");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        std::wstring wstr = PyAE::StringUtils::Utf8ToWide(info_string);
        A_Err err = suites.utilitySuite->AEGP_ReportInfoUnicode(
            (AEGP_PluginID)plugin_id,
            reinterpret_cast<const A_UTF16Char*>(wstr.c_str())
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ReportInfoUnicode failed");
    }, py::arg("plugin_id"), py::arg("info_string"),
    "Display an information dialog with Unicode string support");

    // -----------------------------------------------------------------------
    // Version Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetDriverPluginInitFuncVersion", []() -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_short major = 0, minor = 0;
        A_Err err = suites.utilitySuite->AEGP_GetDriverPluginInitFuncVersion(&major, &minor);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetDriverPluginInitFuncVersion failed");

        return py::make_tuple(static_cast<int>(major), static_cast<int>(minor));
    }, "Get the driver plugin initialization function version");

    sdk.def("AEGP_GetDriverImplementationVersion", []() -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_short major = 0, minor = 0;
        A_Err err = suites.utilitySuite->AEGP_GetDriverImplementationVersion(&major, &minor);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetDriverImplementationVersion failed");

        return py::make_tuple(static_cast<int>(major), static_cast<int>(minor));
    }, "Get the driver implementation version");

    // -----------------------------------------------------------------------
    // Error Handling Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_StartQuietErrors", []() -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ErrReportState errState;
        std::memset(&errState, 0, sizeof(AEGP_ErrReportState));
        A_Err err = suites.utilitySuite->AEGP_StartQuietErrors(&errState);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_StartQuietErrors failed");

        // Store as opaque pointer
        AEGP_ErrReportState* statePtr = new AEGP_ErrReportState(errState);
        return reinterpret_cast<uintptr_t>(statePtr);
    }, "Start suppressing error dialogs, returns error state token");

    sdk.def("AEGP_EndQuietErrors", [](bool report_quieted_errors, uintptr_t err_state_ptr) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ErrReportState* statePtr = reinterpret_cast<AEGP_ErrReportState*>(err_state_ptr);
        if (!statePtr) throw std::runtime_error("Invalid error state pointer");

        A_Err err = suites.utilitySuite->AEGP_EndQuietErrors(
            report_quieted_errors ? TRUE : FALSE,
            statePtr
        );

        delete statePtr;  // Clean up allocated memory

        if (err != A_Err_NONE) throw std::runtime_error("AEGP_EndQuietErrors failed");
    }, py::arg("report_quieted_errors"), py::arg("err_state"),
    "End error suppression and optionally report the last quieted error");

    sdk.def("AEGP_GetLastErrorMessage", []() -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        const A_long bufferSize = 1024;
        A_char errorStr[1024] = {0};
        A_Err errorNum = A_Err_NONE;

        A_Err err = suites.utilitySuite->AEGP_GetLastErrorMessage(
            bufferSize,
            errorStr,
            &errorNum
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLastErrorMessage failed");

        // Convert from system encoding (Shift-JIS on Japanese Windows) to UTF-8 via UTF-16
        std::wstring wstr;
        #ifdef _WIN32
        // On Windows, convert from ANSI code page (e.g., Shift-JIS) to UTF-16
        int wlen = MultiByteToWideChar(CP_ACP, 0, errorStr, -1, NULL, 0);
        if (wlen > 0) {
            wstr.resize(wlen - 1);  // -1 to exclude null terminator
            MultiByteToWideChar(CP_ACP, 0, errorStr, -1, &wstr[0], wlen);
        }
        #else
        // On other platforms, assume UTF-8
        wstr = std::wstring(errorStr, errorStr + strlen(errorStr));
        #endif

        return py::make_tuple(
            wstr,  // pybind11 will convert std::wstring to Python str (UTF-8)
            static_cast<int>(errorNum)
        );
    }, "Get the last error message and error number");

    // -----------------------------------------------------------------------
    // Registration and Platform Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_RegisterWithAEGP", [](uintptr_t global_refcon, const std::string& plugin_name) -> int {
        Validation::RequireNonEmpty(plugin_name, "plugin_name");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_PluginID pluginID = 0;
        A_Err err = suites.utilitySuite->AEGP_RegisterWithAEGP(
            reinterpret_cast<AEGP_GlobalRefcon>(global_refcon),
            plugin_name.c_str(),
            &pluginID
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_RegisterWithAEGP failed");

        return static_cast<int>(pluginID);
    }, py::arg("global_refcon"), py::arg("plugin_name"),
    "Register a plugin with After Effects and get a plugin ID");

    sdk.def("AEGP_GetMainHWND", []() -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        void* hwnd = nullptr;
        A_Err err = suites.utilitySuite->AEGP_GetMainHWND(&hwnd);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMainHWND failed");

        return reinterpret_cast<uintptr_t>(hwnd);
    }, "Get the main window handle (HWND on Windows)");

    sdk.def("AEGP_ShowHideAllFloaters", [](bool include_tool_pal) -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Err err = suites.utilitySuite->AEGP_ShowHideAllFloaters(
            include_tool_pal ? TRUE : FALSE
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ShowHideAllFloaters failed");
    }, py::arg("include_tool_pal"),
    "Toggle visibility of all floating palettes");

    // -----------------------------------------------------------------------
    // Paint Palette Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_PaintPalGetForeColor", []() -> py::dict {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ColorVal color;
        A_Err err = suites.utilitySuite->AEGP_PaintPalGetForeColor(&color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_PaintPalGetForeColor failed");

        py::dict result;
        result["red"] = color.redF;
        result["green"] = color.greenF;
        result["blue"] = color.blueF;
        result["alpha"] = color.alphaF;
        return result;
    }, "Get the foreground color from the paint palette");

    sdk.def("AEGP_PaintPalGetBackColor", []() -> py::dict {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ColorVal color;
        A_Err err = suites.utilitySuite->AEGP_PaintPalGetBackColor(&color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_PaintPalGetBackColor failed");

        py::dict result;
        result["red"] = color.redF;
        result["green"] = color.greenF;
        result["blue"] = color.blueF;
        result["alpha"] = color.alphaF;
        return result;
    }, "Get the background color from the paint palette");

    sdk.def("AEGP_PaintPalSetForeColor", [](py::dict color) -> void {
        double red = color["red"].cast<double>();
        double green = color["green"].cast<double>();
        double blue = color["blue"].cast<double>();
        double alpha = color["alpha"].cast<double>();
        Validation::RequireColorRange(red, "red");
        Validation::RequireColorRange(green, "green");
        Validation::RequireColorRange(blue, "blue");
        Validation::RequireColorRange(alpha, "alpha");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ColorVal colorVal;
        colorVal.redF = red;
        colorVal.greenF = green;
        colorVal.blueF = blue;
        colorVal.alphaF = alpha;

        A_Err err = suites.utilitySuite->AEGP_PaintPalSetForeColor(&colorVal);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_PaintPalSetForeColor failed");
    }, py::arg("color"),
    "Set the foreground color in the paint palette");

    sdk.def("AEGP_PaintPalSetBackColor", [](py::dict color) -> void {
        double red = color["red"].cast<double>();
        double green = color["green"].cast<double>();
        double blue = color["blue"].cast<double>();
        double alpha = color["alpha"].cast<double>();
        Validation::RequireColorRange(red, "red");
        Validation::RequireColorRange(green, "green");
        Validation::RequireColorRange(blue, "blue");
        Validation::RequireColorRange(alpha, "alpha");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ColorVal colorVal;
        colorVal.redF = red;
        colorVal.greenF = green;
        colorVal.blueF = blue;
        colorVal.alphaF = alpha;

        A_Err err = suites.utilitySuite->AEGP_PaintPalSetBackColor(&colorVal);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_PaintPalSetBackColor failed");
    }, py::arg("color"),
    "Set the background color in the paint palette");

    // -----------------------------------------------------------------------
    // Character Palette Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_CharPalGetFillColor", []() -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Boolean isDefined = FALSE;
        AEGP_ColorVal color;
        A_Err err = suites.utilitySuite->AEGP_CharPalGetFillColor(&isDefined, &color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CharPalGetFillColor failed");

        py::dict colorDict;
        if (isDefined) {
            colorDict["red"] = color.redF;
            colorDict["green"] = color.greenF;
            colorDict["blue"] = color.blueF;
            colorDict["alpha"] = color.alphaF;
        }

        return py::make_tuple(static_cast<bool>(isDefined), colorDict);
    }, "Get the text fill color (returns tuple: is_defined, color)");

    sdk.def("AEGP_CharPalGetStrokeColor", []() -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Boolean isDefined = FALSE;
        AEGP_ColorVal color;
        A_Err err = suites.utilitySuite->AEGP_CharPalGetStrokeColor(&isDefined, &color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CharPalGetStrokeColor failed");

        py::dict colorDict;
        if (isDefined) {
            colorDict["red"] = color.redF;
            colorDict["green"] = color.greenF;
            colorDict["blue"] = color.blueF;
            colorDict["alpha"] = color.alphaF;
        }

        return py::make_tuple(static_cast<bool>(isDefined), colorDict);
    }, "Get the text stroke color (returns tuple: is_defined, color)");

    sdk.def("AEGP_CharPalSetFillColor", [](py::dict color) -> void {
        double red = color["red"].cast<double>();
        double green = color["green"].cast<double>();
        double blue = color["blue"].cast<double>();
        double alpha = color["alpha"].cast<double>();
        Validation::RequireColorRange(red, "red");
        Validation::RequireColorRange(green, "green");
        Validation::RequireColorRange(blue, "blue");
        Validation::RequireColorRange(alpha, "alpha");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ColorVal colorVal;
        colorVal.redF = red;
        colorVal.greenF = green;
        colorVal.blueF = blue;
        colorVal.alphaF = alpha;

        A_Err err = suites.utilitySuite->AEGP_CharPalSetFillColor(&colorVal);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CharPalSetFillColor failed");
    }, py::arg("color"),
    "Set the text fill color");

    sdk.def("AEGP_CharPalSetStrokeColor", [](py::dict color) -> void {
        double red = color["red"].cast<double>();
        double green = color["green"].cast<double>();
        double blue = color["blue"].cast<double>();
        double alpha = color["alpha"].cast<double>();
        Validation::RequireColorRange(red, "red");
        Validation::RequireColorRange(green, "green");
        Validation::RequireColorRange(blue, "blue");
        Validation::RequireColorRange(alpha, "alpha");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ColorVal colorVal;
        colorVal.redF = red;
        colorVal.greenF = green;
        colorVal.blueF = blue;
        colorVal.alphaF = alpha;

        A_Err err = suites.utilitySuite->AEGP_CharPalSetStrokeColor(&colorVal);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CharPalSetStrokeColor failed");
    }, py::arg("color"),
    "Set the text stroke color");

    sdk.def("AEGP_CharPalIsFillColorUIFrontmost", []() -> bool {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Boolean isFillColorSelected = FALSE;
        A_Err err = suites.utilitySuite->AEGP_CharPalIsFillColorUIFrontmost(&isFillColorSelected);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CharPalIsFillColorUIFrontmost failed");

        return static_cast<bool>(isFillColorSelected);
    }, "Check if fill color UI is frontmost (otherwise stroke color is frontmost)");

    // -----------------------------------------------------------------------
    // Conversion Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_ConvertFpLongToHSFRatio", [](double number) -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Ratio ratio;
        A_Err err = suites.utilitySuite->AEGP_ConvertFpLongToHSFRatio(number, &ratio);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ConvertFpLongToHSFRatio failed");

        return py::make_tuple(static_cast<int>(ratio.num), static_cast<int>(ratio.den));
    }, py::arg("number"),
    "Convert floating point number to HSF ratio (returns tuple: numerator, denominator)");

    sdk.def("AEGP_ConvertHSFRatioToFpLong", [](int numerator, int denominator) -> double {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Ratio ratio;
        ratio.num = numerator;
        ratio.den = denominator;

        A_FpLong number = 0.0;
        A_Err err = suites.utilitySuite->AEGP_ConvertHSFRatioToFpLong(ratio, &number);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ConvertHSFRatioToFpLong failed");

        return static_cast<double>(number);
    }, py::arg("numerator"), py::arg("denominator"),
    "Convert HSF ratio to floating point number");

    // -----------------------------------------------------------------------
    // Miscellaneous Utility Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_CauseIdleRoutinesToBeCalled", []() -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Err err = suites.utilitySuite->AEGP_CauseIdleRoutinesToBeCalled();
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CauseIdleRoutinesToBeCalled failed");
    }, "Request that idle routines be called (thread-safe, asynchronous)");

    sdk.def("AEGP_GetSuppressInteractiveUI", []() -> bool {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Boolean uiIsSuppressed = FALSE;
        A_Err err = suites.utilitySuite->AEGP_GetSuppressInteractiveUI(&uiIsSuppressed);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetSuppressInteractiveUI failed");

        return static_cast<bool>(uiIsSuppressed);
    }, "Check if interactive UI is suppressed (headless/service mode)");

    sdk.def("AEGP_WriteToOSConsole", [](const std::string& text) -> void {
        Validation::RequireNonEmpty(text, "text");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Err err = suites.utilitySuite->AEGP_WriteToOSConsole(text.c_str());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_WriteToOSConsole failed");
    }, py::arg("text"),
    "Write text to the OS console (guaranteed available when UI is suppressed)");

    sdk.def("AEGP_WriteToDebugLog", [](const std::string& subsystem, const std::string& event_type, const std::string& info) -> void {
        Validation::RequireNonEmpty(subsystem, "subsystem");
        Validation::RequireNonEmpty(event_type, "event_type");
        Validation::RequireNonEmpty(info, "info");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Err err = suites.utilitySuite->AEGP_WriteToDebugLog(
            subsystem.c_str(),
            event_type.c_str(),
            info.c_str()
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_WriteToDebugLog failed");
    }, py::arg("subsystem"), py::arg("event_type"), py::arg("info"),
    "Write an entry to the debug log");

    sdk.def("AEGP_IsScriptingAvailable", []() -> bool {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Boolean isAvailable = FALSE;
        A_Err err = suites.utilitySuite->AEGP_IsScriptingAvailable(&isAvailable);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsScriptingAvailable failed");

        return static_cast<bool>(isAvailable);
    }, "Check if scripting functionality is available");

    sdk.def("AEGP_HostIsActivated", []() -> bool {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Boolean isActivated = FALSE;
        A_Err err = suites.utilitySuite->AEGP_HostIsActivated(&isActivated);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_HostIsActivated failed");

        return static_cast<bool>(isActivated);
    }, "Check if After Effects is activated (licensed)");

    sdk.def("AEGP_GetPluginPlatformRef", [](int plugin_id) -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        void* platRef = nullptr;
        A_Err err = suites.utilitySuite->AEGP_GetPluginPlatformRef(
            (AEGP_PluginID)plugin_id,
            &platRef
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetPluginPlatformRef failed");

        return reinterpret_cast<uintptr_t>(platRef);
    }, py::arg("plugin_id"),
    "Get the plugin platform reference (CFBundleRef on Mac, NULL on Windows)");

    sdk.def("AEGP_UpdateFontList", []() -> void {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        A_Err err = suites.utilitySuite->AEGP_UpdateFontList();
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_UpdateFontList failed");
    }, "Rescan the system font list (returns quickly if unchanged)");

    sdk.def("AEGP_GetPluginPaths", [](int plugin_id, int path_type) -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        // AEGP_GetPathTypes_PLUGIN (0) is not implemented according to AE SDK documentation
        if (path_type == 0) {
            throw std::runtime_error("AEGP_GetPathTypes_PLUGIN (0) is not implemented by After Effects");
        }

        AEGP_MemHandle pathH = nullptr;
        A_Err err = suites.utilitySuite->AEGP_GetPluginPaths(
            (AEGP_PluginID)plugin_id,
            static_cast<AEGP_GetPathTypes>(path_type),
            &pathH
        );
        if (err != A_Err_NONE || !pathH) throw std::runtime_error("AEGP_GetPluginPaths failed");

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, pathH);
        PyAE::ScopedMemLock lock(suites.memorySuite, pathH);
        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, py::arg("plugin_id"), py::arg("path_type"),
    "Get a path associated with the plugin (UTF-16). Note: path_type=0 (PLUGIN) is not implemented.");
}

} // namespace SDK
} // namespace PyAE
