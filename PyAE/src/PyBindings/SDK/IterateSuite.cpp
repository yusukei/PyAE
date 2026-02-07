// IterateSuite.cpp
// PyAE - Python for After Effects
// AEGP_IterateSuite2 bindings

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include "PluginState.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

// Structure to hold Python callback and results for iteration
struct IterateContext {
    py::function callback;
    py::object exception;  // Store any exception that occurs
    bool hasException = false;
};

// C callback function that wraps the Python callback
static A_Err IterateCallback(void* refconPV, A_long thread_indexL, A_long i, A_long iterationsL) {
    IterateContext* ctx = static_cast<IterateContext*>(refconPV);
    if (!ctx || ctx->hasException) {
        return A_Err_NONE;  // Skip further processing if there was an exception
    }

    // Acquire GIL since we're in a callback from C
    py::gil_scoped_acquire acquire;

    try {
        // Call the Python callback
        py::object result = ctx->callback(thread_indexL, i, iterationsL);

        // If callback returns a value, check if it's an error code
        if (!result.is_none()) {
            int err = result.cast<int>();
            if (err != 0) {
                return static_cast<A_Err>(err);
            }
        }
        return A_Err_NONE;
    } catch (const py::error_already_set& e) {
        // Store the exception to re-raise later
        ctx->hasException = true;
        ctx->exception = py::cast(std::string(e.what()));
        return A_Err_GENERIC;  // Return error to stop iteration
    } catch (const std::exception& e) {
        ctx->hasException = true;
        ctx->exception = py::cast(std::string(e.what()));
        return A_Err_GENERIC;
    }
}

void init_IterateSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_IterateSuite2
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetNumThreads", []() -> int {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.iterateSuite) throw std::runtime_error("Iterate Suite not available");

        A_long numThreads = 0;
        A_Err err = suites.iterateSuite->AEGP_GetNumThreads(&numThreads);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumThreads failed");

        return static_cast<int>(numThreads);
    }, "Get the number of threads available for iteration processing");

    sdk.def("AEGP_IterateGeneric", [](int iterations, py::function callback) -> void {
        // Validate arguments
        // Note: -1 (PF_Iterations_ONCE_PER_PROCESSOR) is allowed as a special value
        if (iterations < -1) {
            throw std::invalid_argument("iterations must be >= -1 (use -1 for ONCE_PER_PROCESSOR)");
        }
        if (callback.is_none()) {
            throw std::invalid_argument("callback cannot be None");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.iterateSuite) throw std::runtime_error("Iterate Suite not available");

        // Set up the context for the callback
        IterateContext ctx;
        ctx.callback = callback;
        ctx.hasException = false;

        // Release GIL during the iteration to allow callbacks from other threads
        {
            py::gil_scoped_release release;

            A_Err err = suites.iterateSuite->AEGP_IterateGeneric(
                static_cast<A_long>(iterations),
                static_cast<void*>(&ctx),
                IterateCallback
            );

            // Re-acquire GIL for exception handling
            py::gil_scoped_acquire acquire;

            // Check for errors
            if (ctx.hasException) {
                std::string errMsg = ctx.exception.cast<std::string>();
                throw std::runtime_error("Iteration callback error: " + errMsg);
            }

            if (err != A_Err_NONE) {
                throw std::runtime_error("AEGP_IterateGeneric failed with error: " + std::to_string(err));
            }
        }
    },
    py::arg("iterations"),
    py::arg("callback"),
    R"doc(
        Execute a generic iteration across multiple threads.

        This function divides the work across available threads and calls the
        callback function for each iteration. The callback receives:
        - thread_index: The index of the thread executing this iteration
        - i: The iteration index (0 to iterations-1)
        - iterations: The total number of iterations

        The callback can return an error code (non-zero to stop iteration) or
        None to continue.

        Note: The callback may be called from multiple threads simultaneously.
        Ensure thread-safety if accessing shared resources.

        Special value: Use iterations=-1 (PF_Iterations_ONCE_PER_PROCESSOR)
        to execute the callback once per available processor.

        Args:
            iterations: Number of iterations to perform, or -1 for once per processor
            callback: Function(thread_index, i, iterations) -> int or None

        Raises:
            RuntimeError: If iteration fails or callback raises an exception
    )doc");

    // -----------------------------------------------------------------------
    // Helper constant for ONCE_PER_PROCESSOR
    // -----------------------------------------------------------------------
    sdk.attr("PF_Iterations_ONCE_PER_PROCESSOR") = -1;
}

} // namespace SDK
} // namespace PyAE
