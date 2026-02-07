#include <pybind11/pybind11.h>

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_StreamValueHelpers(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // Helpers for accessing AEGP_StreamValue2 union members
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetStreamValue_TextDocumentH", [](uintptr_t valueP_ptr) -> uintptr_t {
        if (!valueP_ptr) {
            throw std::invalid_argument("AEGP_GetStreamValue_TextDocumentH: valueP cannot be null");
        }
        AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
        return reinterpret_cast<uintptr_t>(valueP->val.text_documentH);
    }, py::arg("valueP"));

    sdk.def("AEGP_GetStreamValue_MaskOutlineH", [](uintptr_t valueP_ptr) -> uintptr_t {
        if (!valueP_ptr) {
            throw std::invalid_argument("AEGP_GetStreamValue_MaskOutlineH: valueP cannot be null");
        }
        AEGP_StreamValue2* valueP = reinterpret_cast<AEGP_StreamValue2*>(valueP_ptr);
        return reinterpret_cast<uintptr_t>(valueP->val.mask);
    }, py::arg("valueP"));
}

} // namespace SDK
} // namespace PyAE
