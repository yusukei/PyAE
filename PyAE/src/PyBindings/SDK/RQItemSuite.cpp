#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_RQItemSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_RQItemSuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetNumRQItems", []() -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        A_long num_items = 0;
        A_Err err = suites.rqItemSuite->AEGP_GetNumRQItems(&num_items);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumRQItems failed");
        
        return (int)num_items;
    });

    sdk.def("AEGP_GetRQItemByIndex", [](int index) -> uintptr_t {
         auto& state = PyAE::PluginState::Instance();
         const auto& suites = state.GetSuites();
         if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

         // Index range validation
         Validation::RequireNonNegative(index, "index");

         // Range check - get number of render queue items first
         A_long num_items = 0;
         A_Err err = suites.rqItemSuite->AEGP_GetNumRQItems(&num_items);
         if (err != A_Err_NONE) throw std::runtime_error("Failed to get render queue item count");

         // Check bounds (0-based indexing)
         Validation::RequireValidIndex(index, static_cast<int>(num_items), "Render queue item");

         AEGP_RQItemRefH itemH = nullptr;
         err = suites.rqItemSuite->AEGP_GetRQItemByIndex(index, &itemH);
         if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetRQItemByIndex failed");

         return reinterpret_cast<uintptr_t>(itemH);
    }, py::arg("index"));

    sdk.def("AEGP_GetNextRQItem", [](uintptr_t current_rq_itemH_ptr) -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH current_rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(current_rq_itemH_ptr);
        AEGP_RQItemRefH next_rq_itemH = nullptr;

        A_Err err = suites.rqItemSuite->AEGP_GetNextRQItem(current_rq_itemH, &next_rq_itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNextRQItem failed");

        return reinterpret_cast<uintptr_t>(next_rq_itemH);
    }, py::arg("current_rq_itemH"), "Get next RQ item. Pass 0 for current_rq_itemH to get first item.");

    sdk.def("AEGP_GetNumOutputModulesForRQItem", [](uintptr_t rq_itemH_ptr) -> int {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        A_long num_modules = 0;
        A_Err err = suites.rqItemSuite->AEGP_GetNumOutputModulesForRQItem(rq_itemH, &num_modules);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumOutputModulesForRQItem failed");

        return (int)num_modules;
    }, py::arg("rq_itemH"));

    sdk.def("AEGP_GetRenderState", [](uintptr_t rq_itemH_ptr) -> int {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_RenderItemStatusType status;
        A_Err err = suites.rqItemSuite->AEGP_GetRenderState(rq_itemH, &status);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetRenderState failed");

        return (int)status;
    }, py::arg("rq_itemH"));

    sdk.def("AEGP_SetRenderState", [](uintptr_t rq_itemH_ptr, int status) {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        A_Err err = suites.rqItemSuite->AEGP_SetRenderState(rq_itemH, (AEGP_RenderItemStatusType)status);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetRenderState failed");
    }, py::arg("rq_itemH"), py::arg("status"));

    sdk.def("AEGP_GetStartedTime", [](uintptr_t rq_itemH_ptr) -> double {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        A_Time started_time = {0, 1};

        A_Err err = suites.rqItemSuite->AEGP_GetStartedTime(rq_itemH, &started_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetStartedTime failed");

        return AETypeUtils::TimeToSeconds(started_time);
    }, py::arg("rq_itemH"), "Get render started time in seconds. Returns 0 if not started.");

    sdk.def("AEGP_GetElapsedTime", [](uintptr_t rq_itemH_ptr) -> double {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        A_Time render_time = {0, 1};

        A_Err err = suites.rqItemSuite->AEGP_GetElapsedTime(rq_itemH, &render_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetElapsedTime failed");

        return AETypeUtils::TimeToSeconds(render_time);
    }, py::arg("rq_itemH"), "Get elapsed render time in seconds. Returns 0 if not rendered.");

    sdk.def("AEGP_GetLogType", [](uintptr_t rq_itemH_ptr) -> int {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_LogType logtype = 0;

        A_Err err = suites.rqItemSuite->AEGP_GetLogType(rq_itemH, &logtype);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLogType failed");

        return static_cast<int>(logtype);
    }, py::arg("rq_itemH"), "Get log type for RQ item.");

    sdk.def("AEGP_SetLogType", [](uintptr_t rq_itemH_ptr, int logtype) {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);

        A_Err err = suites.rqItemSuite->AEGP_SetLogType(rq_itemH, static_cast<AEGP_LogType>(logtype));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLogType failed");
    }, py::arg("rq_itemH"), py::arg("logtype"), "Set log type for RQ item.");

    sdk.def("AEGP_RemoveOutputModule", [](uintptr_t rq_itemH_ptr, uintptr_t outmodH_ptr) {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");
        Validation::RequireNonZero(outmodH_ptr, "outmodH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_OutputModuleRefH outmodH = reinterpret_cast<AEGP_OutputModuleRefH>(outmodH_ptr);

        A_Err err = suites.rqItemSuite->AEGP_RemoveOutputModule(rq_itemH, outmodH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_RemoveOutputModule failed");
    }, py::arg("rq_itemH"), py::arg("outmodH"), "Remove output module from RQ item.");

    sdk.def("AEGP_GetComment", [](uintptr_t rq_itemH_ptr) -> std::wstring {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_MemHandle unicodeH = nullptr;

        A_Err err = suites.rqItemSuite->AEGP_GetComment(rq_itemH, &unicodeH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetComment failed");

        if (!unicodeH) return std::wstring();

        // Use RAII helpers for automatic memory management
        PyAE::ScopedMemHandle scopedHandle(state.GetPluginID(), suites.memorySuite, unicodeH);
        PyAE::ScopedMemLock lock(suites.memorySuite, unicodeH);

        if (!lock.IsValid()) {
            return std::wstring();  // RAII destructors will free memory
        }

        return std::wstring(reinterpret_cast<const wchar_t*>(lock.As<A_UTF16Char>()));
    }, py::arg("rq_itemH"), "Get comment for RQ item.");

    sdk.def("AEGP_SetComment", [](uintptr_t rq_itemH_ptr, const std::wstring& comment) {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        const A_UTF16Char* commentZ = reinterpret_cast<const A_UTF16Char*>(comment.c_str());

        A_Err err = suites.rqItemSuite->AEGP_SetComment(rq_itemH, commentZ);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetComment failed");
    }, py::arg("rq_itemH"), py::arg("comment"), "Set comment for RQ item.");
    
    sdk.def("AEGP_DeleteRQItem", [](uintptr_t rq_itemH_ptr) {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        A_Err err = suites.rqItemSuite->AEGP_DeleteRQItem(rq_itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DeleteRQItem failed");
    }, py::arg("rq_itemH"));

    sdk.def("AEGP_GetCompFromRQItem", [](uintptr_t rq_itemH_ptr) -> uintptr_t {
        // NULL pointer validation
        Validation::RequireNonZero(rq_itemH_ptr, "rq_itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.rqItemSuite) throw std::runtime_error("RQItem Suite not available");

        AEGP_RQItemRefH rq_itemH = reinterpret_cast<AEGP_RQItemRefH>(rq_itemH_ptr);
        AEGP_CompH compH = nullptr;
        A_Err err = suites.rqItemSuite->AEGP_GetCompFromRQItem(rq_itemH, &compH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetCompFromRQItem failed");

        return reinterpret_cast<uintptr_t>(compH);
    }, py::arg("rq_itemH"));

    // Constants
    sdk.attr("AEGP_RenderItemStatus_NEEDS_OUTPUT") = (int)AEGP_RenderItemStatus_NEEDS_OUTPUT;
    sdk.attr("AEGP_RenderItemStatus_UNQUEUED") = (int)AEGP_RenderItemStatus_UNQUEUED;
    sdk.attr("AEGP_RenderItemStatus_QUEUED") = (int)AEGP_RenderItemStatus_QUEUED;
    sdk.attr("AEGP_RenderItemStatus_RENDERING") = (int)AEGP_RenderItemStatus_RENDERING;
    sdk.attr("AEGP_RenderItemStatus_USER_STOPPED") = (int)AEGP_RenderItemStatus_USER_STOPPED;
    sdk.attr("AEGP_RenderItemStatus_ERR_STOPPED") = (int)AEGP_RenderItemStatus_ERR_STOPPED;
    sdk.attr("AEGP_RenderItemStatus_DONE") = (int)AEGP_RenderItemStatus_DONE;
}

} // namespace SDK
} // namespace PyAE
