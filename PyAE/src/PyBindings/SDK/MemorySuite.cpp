// MemorySuite.cpp
// PyAE - Python for After Effects
// AEGP_MemorySuite1 bindings for memory management
//
// WARNING: Low-level memory operations. Use ae.get_memory_stats() and
// ae.enable_memory_reporting() for diagnostics instead.
// Direct memory handle operations should only be used when absolutely necessary.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_MemorySuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_MemorySuite1 - Memory management (8 functions)
    // -----------------------------------------------------------------------

    // AEGP_NewMemHandle - Allocate a memory handle
    // Returns the handle as uintptr_t
    sdk.def("AEGP_NewMemHandle", [](const std::string& what, int size, int flags) -> uintptr_t {
        // Validation
        if (size <= 0) {
            throw std::invalid_argument("AEGP_NewMemHandle: size must be positive, got " + std::to_string(size));
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_MemHandle memH = nullptr;
        A_Err err = suites.memorySuite->AEGP_NewMemHandle(
            state.GetPluginID(),
            what.c_str(),
            static_cast<AEGP_MemSize>(size),
            static_cast<AEGP_MemFlag>(flags),
            &memH
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_NewMemHandle failed with error code: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(memH);
    }, py::arg("what"), py::arg("size"), py::arg("flags") = 0,
    "Allocate a new memory handle. Returns handle as int.\n"
    "WARNING: You must call AEGP_FreeMemHandle to release the memory.\n"
    "Args:\n"
    "  what: Description string for debugging\n"
    "  size: Size in bytes to allocate\n"
    "  flags: AEGP_MemFlag_NONE (0), AEGP_MemFlag_CLEAR (1), AEGP_MemFlag_QUIET (2)");

    // AEGP_FreeMemHandle - Free a memory handle
    sdk.def("AEGP_FreeMemHandle", [](uintptr_t memH_ptr) {
        if (memH_ptr == 0) {
            throw std::invalid_argument("AEGP_FreeMemHandle: memH cannot be null (0)");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_MemHandle memH = reinterpret_cast<AEGP_MemHandle>(memH_ptr);
        A_Err err = suites.memorySuite->AEGP_FreeMemHandle(memH);

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_FreeMemHandle failed with error code: " + std::to_string(err));
        }
    }, py::arg("memH"),
    "Free a memory handle allocated by AEGP_NewMemHandle.");

    // AEGP_LockMemHandle - Lock a memory handle and get pointer
    // Note: In Python, we can't directly return a raw pointer.
    // This is provided for completeness but typically used internally.
    sdk.def("AEGP_LockMemHandle", [](uintptr_t memH_ptr) -> uintptr_t {
        if (memH_ptr == 0) {
            throw std::invalid_argument("AEGP_LockMemHandle: memH cannot be null (0)");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_MemHandle memH = reinterpret_cast<AEGP_MemHandle>(memH_ptr);
        void* ptr = nullptr;
        A_Err err = suites.memorySuite->AEGP_LockMemHandle(memH, &ptr);

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_LockMemHandle failed with error code: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(ptr);
    }, py::arg("memH"),
    "Lock a memory handle and return pointer as int.\n"
    "Locks are nestable - each lock must be matched with an unlock.\n"
    "Returns: Pointer to locked memory as int.");

    // AEGP_UnlockMemHandle - Unlock a memory handle
    sdk.def("AEGP_UnlockMemHandle", [](uintptr_t memH_ptr) {
        if (memH_ptr == 0) {
            throw std::invalid_argument("AEGP_UnlockMemHandle: memH cannot be null (0)");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_MemHandle memH = reinterpret_cast<AEGP_MemHandle>(memH_ptr);
        A_Err err = suites.memorySuite->AEGP_UnlockMemHandle(memH);

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_UnlockMemHandle failed with error code: " + std::to_string(err));
        }
    }, py::arg("memH"),
    "Unlock a memory handle. Must match each AEGP_LockMemHandle call.");

    // AEGP_GetMemHandleSize - Get the size of a memory handle
    sdk.def("AEGP_GetMemHandleSize", [](uintptr_t memH_ptr) -> int {
        if (memH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMemHandleSize: memH cannot be null (0)");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_MemHandle memH = reinterpret_cast<AEGP_MemHandle>(memH_ptr);
        AEGP_MemSize size = 0;
        A_Err err = suites.memorySuite->AEGP_GetMemHandleSize(memH, &size);

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetMemHandleSize failed with error code: " + std::to_string(err));
        }

        return static_cast<int>(size);
    }, py::arg("memH"),
    "Get the size of a memory handle in bytes.");

    // AEGP_ResizeMemHandle - Resize a memory handle
    sdk.def("AEGP_ResizeMemHandle", [](const std::string& what, int new_size, uintptr_t memH_ptr) {
        if (memH_ptr == 0) {
            throw std::invalid_argument("AEGP_ResizeMemHandle: memH cannot be null (0)");
        }
        if (new_size < 0) {
            throw std::invalid_argument("AEGP_ResizeMemHandle: new_size cannot be negative, got " + std::to_string(new_size));
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_MemHandle memH = reinterpret_cast<AEGP_MemHandle>(memH_ptr);
        A_Err err = suites.memorySuite->AEGP_ResizeMemHandle(
            what.c_str(),
            static_cast<AEGP_MemSize>(new_size),
            memH
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_ResizeMemHandle failed with error code: " + std::to_string(err));
        }
    }, py::arg("what"), py::arg("new_size"), py::arg("memH"),
    "Resize a memory handle.\n"
    "WARNING: Underlying memory may be reallocated. Do not use old pointers.\n"
    "Args:\n"
    "  what: Description string for debugging\n"
    "  new_size: New size in bytes\n"
    "  memH: Memory handle to resize");

    // AEGP_SetMemReportingOn - Enable/disable memory reporting
    sdk.def("AEGP_SetMemReportingOn", [](bool enable) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        A_Err err = suites.memorySuite->AEGP_SetMemReportingOn(enable ? TRUE : FALSE);

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetMemReportingOn failed with error code: " + std::to_string(err));
        }
    }, py::arg("enable"),
    "Enable or disable detailed memory reporting.\n"
    "When enabled, After Effects reports all memory allocations/frees.\n"
    "Use for debugging only - impacts performance.");

    // AEGP_GetMemStats - Get memory statistics
    sdk.def("AEGP_GetMemStats", []() -> py::dict {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        A_long count = 0;
        A_long size = 0;
        A_Err err = suites.memorySuite->AEGP_GetMemStats(
            state.GetPluginID(),
            &count,
            &size
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetMemStats failed with error code: " + std::to_string(err));
        }

        py::dict result;
        result["handle_count"] = count;
        result["total_size"] = size;
        return result;
    },
    "Get memory statistics for this plugin.\n"
    "Returns dict with 'handle_count' and 'total_size' (bytes).\n"
    "Note: Prefer ae.get_memory_stats() for high-level diagnostics.");

    // -----------------------------------------------------------------------
    // Constants for memory flags
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_MemFlag_NONE") = 0;
    sdk.attr("AEGP_MemFlag_CLEAR") = 1;
    sdk.attr("AEGP_MemFlag_QUIET") = 2;
}

} // namespace SDK
} // namespace PyAE
