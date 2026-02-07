#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

#include <sstream>

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_MarkerSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_MarkerSuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_NewMarker", []() -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = nullptr;
        A_Err err = suites.markerSuite->AEGP_NewMarker(&markerP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NewMarker failed");

        return reinterpret_cast<uintptr_t>(markerP);
    }, "Creates a new marker. Must be disposed with AEGP_DisposeMarker.");

    sdk.def("AEGP_DisposeMarker", [](uintptr_t markerP_ptr) {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        suites.markerSuite->AEGP_DisposeMarker(markerP);
    }, py::arg("markerP"));

    sdk.def("AEGP_SetMarkerString", [](uintptr_t markerP_ptr, int strType, const std::wstring& wstr) {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);

        // Empty string handling
        if (wstr.empty()) {
            const A_UTF16Char empty[] = {0};
            A_Err err = suites.markerSuite->AEGP_SetMarkerString(markerP, (AEGP_MarkerStringType)strType, empty, 0);
            if (err != A_Err_NONE) {
                std::ostringstream oss;
                oss << "AEGP_SetMarkerString failed for strType=" << strType
                    << " (empty string) with error code: 0x" << std::hex << err;
                throw std::runtime_error(oss.str());
            }
            return;
        }

        const A_UTF16Char* utf16Str = reinterpret_cast<const A_UTF16Char*>(wstr.c_str());

        // Length should NOT include null terminator (SDK samples show this)
        A_Err err = suites.markerSuite->AEGP_SetMarkerString(
            markerP,
            (AEGP_MarkerStringType)strType,
            utf16Str,
            static_cast<A_long>(wstr.length())
        );
        if (err != A_Err_NONE) {
            std::ostringstream oss;
            oss << "AEGP_SetMarkerString failed for strType=" << strType
                << " with error code: 0x" << std::hex << err;
            throw std::runtime_error(oss.str());
        }
    }, py::arg("markerP"), py::arg("strType"), py::arg("str"));

    sdk.def("AEGP_GetMarkerString", [](int plugin_id, uintptr_t markerP_ptr, int strType) -> std::wstring {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        AEGP_MemHandle memH = nullptr;

        A_Err err = suites.markerSuite->AEGP_GetMarkerString(
            plugin_id, markerP, (AEGP_MarkerStringType)strType, &memH);

        // If error occurs, throw exception instead of silently returning empty string
        if (err != A_Err_NONE) {
            std::ostringstream oss;
            oss << "AEGP_GetMarkerString failed for strType=" << strType
                << " with error code: 0x" << std::hex << err;
            throw std::runtime_error(oss.str());
        }

        // Empty string is valid if memH is null
        if (!memH) return L"";

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, memH);
        PyAE::ScopedMemLock lock(suites.memorySuite, memH);
        A_UTF16Char* chars = lock.As<A_UTF16Char>();
        if (!chars) return L"";

        return std::wstring(reinterpret_cast<const wchar_t*>(chars));
    }, py::arg("plugin_id"), py::arg("markerP"), py::arg("strType"));

    sdk.def("AEGP_DuplicateMarker", [](uintptr_t markerP_ptr) -> uintptr_t {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        AEGP_MarkerValP newMarkerP = nullptr;
        A_Err err = suites.markerSuite->AEGP_DuplicateMarker(markerP, &newMarkerP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DuplicateMarker failed");

        return reinterpret_cast<uintptr_t>(newMarkerP);
    }, py::arg("markerP"), "Duplicates a marker. Must be disposed with AEGP_DisposeMarker.");

    sdk.def("AEGP_GetMarkerFlag", [](uintptr_t markerP_ptr, int flagType) -> bool {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_Boolean valueB = FALSE;
        A_Err err = suites.markerSuite->AEGP_GetMarkerFlag(markerP, (AEGP_MarkerFlagType)flagType, &valueB);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMarkerFlag failed");

        return valueB != FALSE;
    }, py::arg("markerP"), py::arg("flagType"), "Gets a marker flag value.");

    sdk.def("AEGP_SetMarkerFlag", [](uintptr_t markerP_ptr, int flagType, bool valueB) {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_Err err = suites.markerSuite->AEGP_SetMarkerFlag(markerP, (AEGP_MarkerFlagType)flagType, valueB ? TRUE : FALSE);
        if (err != A_Err_NONE) {
            std::ostringstream oss;
            oss << "AEGP_SetMarkerFlag failed for flagType=" << flagType
                << ", value=" << valueB << " with error code: 0x" << std::hex << err;
            throw std::runtime_error(oss.str());
        }
    }, py::arg("markerP"), py::arg("flagType"), py::arg("valueB"), "Sets a marker flag value.");

    sdk.def("AEGP_GetMarkerDuration", [](uintptr_t markerP_ptr) -> py::dict {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_Time durationT = {0, 1};
        A_Err err = suites.markerSuite->AEGP_GetMarkerDuration(markerP, &durationT);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMarkerDuration failed");

        py::dict result;
        result["value"] = durationT.value;
        result["scale"] = durationT.scale;
        return result;
    }, py::arg("markerP"), "Gets marker duration as A_Time.");

    sdk.def("AEGP_SetMarkerDuration", [](uintptr_t markerP_ptr, py::dict durationT) {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_Time duration;
        duration.value = durationT["value"].cast<A_long>();
        duration.scale = durationT["scale"].cast<A_u_long>();

        A_Err err = suites.markerSuite->AEGP_SetMarkerDuration(markerP, &duration);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMarkerDuration failed");
    }, py::arg("markerP"), py::arg("durationT"), "Sets marker duration from A_Time dict.");

    sdk.def("AEGP_GetMarkerLabel", [](uintptr_t markerP_ptr) -> int {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_long valueP = 0;
        A_Err err = suites.markerSuite->AEGP_GetMarkerLabel(markerP, &valueP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMarkerLabel failed");

        return static_cast<int>(valueP);
    }, py::arg("markerP"), "Gets marker label color index.");

    sdk.def("AEGP_SetMarkerLabel", [](uintptr_t markerP_ptr, int value) {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }
        // Validate label range (0-16)
        Validation::RequireRange(value, 0, 16, "label");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_Err err = suites.markerSuite->AEGP_SetMarkerLabel(markerP, static_cast<A_long>(value));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMarkerLabel failed");
    }, py::arg("markerP"), py::arg("value"), "Sets marker label color index (0-16).");

    sdk.def("AEGP_CountCuePointParams", [](uintptr_t markerP_ptr) -> int {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_long paramsL = 0;
        A_Err err = suites.markerSuite->AEGP_CountCuePointParams(markerP, &paramsL);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CountCuePointParams failed");

        return static_cast<int>(paramsL);
    }, py::arg("markerP"), "Returns the number of cue point parameters.");

    sdk.def("AEGP_GetIndCuePointParam", [](int plugin_id, uintptr_t markerP_ptr, int param_indexL) -> py::dict {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }
        // Validate cue point index (must be non-negative)
        Validation::RequireNonNegative(param_indexL, "param_indexL");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        AEGP_MemHandle keyH = nullptr;
        AEGP_MemHandle valueH = nullptr;

        A_Err err = suites.markerSuite->AEGP_GetIndCuePointParam(
            plugin_id, markerP, static_cast<A_long>(param_indexL), &keyH, &valueH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetIndCuePointParam failed");

        std::string key;
        std::string value;

        if (keyH) {
            PyAE::ScopedMemHandle scopedKey(plugin_id, suites.memorySuite, keyH);
            PyAE::ScopedMemLock lockKey(suites.memorySuite, keyH);
            A_UTF16Char* keyChars = lockKey.As<A_UTF16Char>();
            if (keyChars) key = PyAE::StringUtils::Utf16ToUtf8(keyChars);
        }

        if (valueH) {
            PyAE::ScopedMemHandle scopedValue(plugin_id, suites.memorySuite, valueH);
            PyAE::ScopedMemLock lockValue(suites.memorySuite, valueH);
            A_UTF16Char* valueChars = lockValue.As<A_UTF16Char>();
            if (valueChars) value = PyAE::StringUtils::Utf16ToUtf8(valueChars);
        }

        py::dict result;
        result["key"] = key;
        result["value"] = value;
        return result;
    }, py::arg("plugin_id"), py::arg("markerP"), py::arg("param_indexL"),
       "Gets a cue point parameter at index. Returns dict with 'key' and 'value'.");

    sdk.def("AEGP_SetIndCuePointParam", [](uintptr_t markerP_ptr, int param_indexL,
                                            const std::string& key, const std::string& value) {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }
        // Validate cue point index (must be non-negative)
        Validation::RequireNonNegative(param_indexL, "param_indexL");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);

        std::wstring keyW = PyAE::StringUtils::Utf8ToWide(key);
        std::wstring valueW = PyAE::StringUtils::Utf8ToWide(value);

        A_Err err = suites.markerSuite->AEGP_SetIndCuePointParam(
            markerP, static_cast<A_long>(param_indexL),
            reinterpret_cast<const A_u_short*>(keyW.c_str()), static_cast<A_long>(keyW.length()),
            reinterpret_cast<const A_u_short*>(valueW.c_str()), static_cast<A_long>(valueW.length()));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetIndCuePointParam failed");
    }, py::arg("markerP"), py::arg("param_indexL"), py::arg("key"), py::arg("value"),
       "Sets a cue point parameter at index.");

    sdk.def("AEGP_InsertCuePointParam", [](uintptr_t markerP_ptr, int param_indexL) {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }
        // Validate cue point index (must be non-negative)
        Validation::RequireNonNegative(param_indexL, "param_indexL");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_Err err = suites.markerSuite->AEGP_InsertCuePointParam(markerP, static_cast<A_long>(param_indexL));
        if (err != A_Err_NONE) {
            char msg[256];
            snprintf(msg, sizeof(msg), "AEGP_InsertCuePointParam failed for index=%d with error code: 0x%X", param_indexL, err);
            throw std::runtime_error(msg);
        }
    }, py::arg("markerP"), py::arg("param_indexL"),
       "Inserts a new cue point parameter slot at index. Follow with AEGP_SetIndCuePointParam.");

    sdk.def("AEGP_DeleteIndCuePointParam", [](uintptr_t markerP_ptr, int param_indexL) {
        // Validate marker handle
        if (markerP_ptr == 0) {
            throw std::invalid_argument("markerP cannot be null");
        }
        // Validate cue point index (must be non-negative)
        Validation::RequireNonNegative(param_indexL, "param_indexL");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.markerSuite) throw std::runtime_error("Marker Suite not available");

        AEGP_MarkerValP markerP = reinterpret_cast<AEGP_MarkerValP>(markerP_ptr);
        A_Err err = suites.markerSuite->AEGP_DeleteIndCuePointParam(markerP, static_cast<A_long>(param_indexL));
        if (err != A_Err_NONE) {
            char msg[256];
            snprintf(msg, sizeof(msg), "AEGP_DeleteIndCuePointParam failed for index=%d with error code: 0x%X", param_indexL, err);
            throw std::runtime_error(msg);
        }
    }, py::arg("markerP"), py::arg("param_indexL"),
       "Deletes the cue point parameter at index.");
}

} // namespace SDK
} // namespace PyAE
