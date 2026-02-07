#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "StringUtils.h"
#include "../ValidationUtils.h"
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_TextDocumentSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_TextDocumentSuite
    // -----------------------------------------------------------------------
    
    sdk.def("AEGP_GetNewText", [](uintptr_t text_documentH_ptr) -> std::string {
        // Validate arguments
        Validation::RequireNonNull(text_documentH_ptr, "text_documentH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.textDocSuite) throw std::runtime_error("Text Document Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_TextDocumentH text_documentH = reinterpret_cast<AEGP_TextDocumentH>(text_documentH_ptr);
        AEGP_MemHandle unicodePH = nullptr;
        
        A_Err err = suites.textDocSuite->AEGP_GetNewText(state.GetPluginID(), text_documentH, &unicodePH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewText failed");
        
        // Convert UTF-16 to UTF-8
        A_u_short* unicode_str = nullptr;
        err = suites.memorySuite->AEGP_LockMemHandle(unicodePH, (void**)&unicode_str);
        if (err != A_Err_NONE) {
             suites.memorySuite->AEGP_FreeMemHandle(unicodePH);
             throw std::runtime_error("AEGP_LockMemHandle failed");
        }
        
        // SDK ensures null-terminated UTF-16 string
        std::string result;
        try {
            if (unicode_str) {
                result = PyAE::StringUtils::Utf16ToUtf8(reinterpret_cast<const A_UTF16Char*>(unicode_str));
            }
        } catch (...) {
            suites.memorySuite->AEGP_UnlockMemHandle(unicodePH);
            suites.memorySuite->AEGP_FreeMemHandle(unicodePH);
            throw;
        }
        
        suites.memorySuite->AEGP_UnlockMemHandle(unicodePH);
        suites.memorySuite->AEGP_FreeMemHandle(unicodePH);
        
        return result;
    }, py::arg("text_documentH"), "Get text content from text document handle.");

    sdk.def("AEGP_SetText", [](uintptr_t text_documentH_ptr, const std::string& text) {
        // Validate arguments
        Validation::RequireNonNull(text_documentH_ptr, "text_documentH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.textDocSuite) throw std::runtime_error("Text Document Suite not available");

        AEGP_TextDocumentH text_documentH = reinterpret_cast<AEGP_TextDocumentH>(text_documentH_ptr);
        
        std::wstring utf16_text = PyAE::StringUtils::Utf8ToUtf16(text);
        
        A_Err err = suites.textDocSuite->AEGP_SetText(
            text_documentH, 
            (const A_u_short*)utf16_text.c_str(), 
            (A_long)utf16_text.length()
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetText failed");
    }, py::arg("text_documentH"), py::arg("text"), "Set text content to text document handle.");
}

void init_TextLayerSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_TextLayerSuite
    // -----------------------------------------------------------------------
    
    sdk.def("AEGP_GetNewTextOutlines", [](uintptr_t layerH_ptr) -> uintptr_t {
        // Validate arguments
        Validation::RequireNonNull(layerH_ptr, "layerH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.textLayerSuite) throw std::runtime_error("Text Layer Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
        AEGP_TextOutlinesH outlinesH = nullptr;
        
        // We use current time, or passed time? SDK signature:
        // AEGP_GetNewTextOutlines(layerH, &time, &outlinesH)
        // For simplicity, let's use time 0 or modify signature to accept time.
        // Let's modify signature to accept time. Wait, A_Time is struct.
        // For now let's use default time 0, as checking Text features often doesn't depend on time 
        // unless text is animated.
        // Actually, let's create a A_Time at 0.
        A_Time time = {0, 1}; // 0 seconds
        
        A_Err err = suites.textLayerSuite->AEGP_GetNewTextOutlines(layerH, &time, &outlinesH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewTextOutlines failed");
        
        return reinterpret_cast<uintptr_t>(outlinesH);
    }, py::arg("layerH"), "Get new text outlines from layer (at time 0). Returns AEGP_TextOutlinesH.");

    sdk.def("AEGP_DisposeTextOutlines", [](uintptr_t outlinesH_ptr) {
        // Validate arguments
        Validation::RequireNonNull(outlinesH_ptr, "outlinesH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.textLayerSuite) throw std::runtime_error("Text Layer Suite not available");

        AEGP_TextOutlinesH outlinesH = reinterpret_cast<AEGP_TextOutlinesH>(outlinesH_ptr);
        A_Err err = suites.textLayerSuite->AEGP_DisposeTextOutlines(outlinesH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisposeTextOutlines failed");
    }, py::arg("outlinesH"), "Dispose text outlines.");

    sdk.def("AEGP_GetNumTextOutlines", [](uintptr_t outlinesH_ptr) -> int {
        // Validate arguments
        Validation::RequireNonNull(outlinesH_ptr, "outlinesH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.textLayerSuite) throw std::runtime_error("Text Layer Suite not available");

        AEGP_TextOutlinesH outlinesH = reinterpret_cast<AEGP_TextOutlinesH>(outlinesH_ptr);
        A_long num_outlines = 0;

        A_Err err = suites.textLayerSuite->AEGP_GetNumTextOutlines(outlinesH, &num_outlines);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumTextOutlines failed");

        return (int)num_outlines;
    }, py::arg("outlinesH"), "Get number of text outlines.");

    sdk.def("AEGP_GetIndexedTextOutline", [](uintptr_t outlinesH_ptr, int path_index) -> uintptr_t {
        // Validate arguments
        Validation::RequireNonNull(outlinesH_ptr, "outlinesH");
        Validation::RequireNonNegative(path_index, "path_index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.textLayerSuite) throw std::runtime_error("Text Layer Suite not available");

        AEGP_TextOutlinesH outlinesH = reinterpret_cast<AEGP_TextOutlinesH>(outlinesH_ptr);

        // Range check (upper bound)
        A_long num_outlines = 0;
        A_Err err = suites.textLayerSuite->AEGP_GetNumTextOutlines(outlinesH, &num_outlines);
        if (err != A_Err_NONE) throw std::runtime_error("Failed to get number of text outlines");

        Validation::RequireValidIndex(path_index, static_cast<int>(num_outlines), "path_index");

        PF_PathOutlinePtr pathP = nullptr;
        err = suites.textLayerSuite->AEGP_GetIndexedTextOutline(outlinesH, static_cast<A_long>(path_index), &pathP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetIndexedTextOutline failed");

        // Note: SDK says "DO NOT DISPOSE" - the path pointer is owned by the outlinesH
        return reinterpret_cast<uintptr_t>(pathP);
    }, py::arg("outlinesH"), py::arg("path_index"), "Get indexed text outline path. Returns PF_PathOutlinePtr (do not dispose).");
}

} // namespace SDK
} // namespace PyAE
