// ItemViewSuite.cpp
// PyAE - Python for After Effects
// AEGP_ItemViewSuite1 bindings

#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_ItemViewSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_ItemViewSuite1
    // -----------------------------------------------------------------------
    // This suite provides access to item view playback time information.
    // Each item view can have its own playback time if previewing is active,
    // otherwise it returns the default item current time.

    sdk.def("AEGP_GetItemViewPlaybackTime", [](uintptr_t item_viewP_ptr) -> py::dict {
        // Validate input
        if (item_viewP_ptr == 0) {
            throw std::invalid_argument("AEGP_GetItemViewPlaybackTime: item_viewP cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.itemViewSuite) {
            throw std::runtime_error("ItemView Suite not available");
        }

        AEGP_ItemViewP item_viewP = reinterpret_cast<AEGP_ItemViewP>(item_viewP_ptr);
        A_Boolean is_currently_previewing = FALSE;
        A_Time curr_time = {0, 1};  // Initialize with safe default

        A_Err err = suites.itemViewSuite->AEGP_GetItemViewPlaybackTime(
            item_viewP,
            &is_currently_previewing,
            &curr_time
        );
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetItemViewPlaybackTime failed with error code: " + std::to_string(err));
        }

        // Return a dictionary with both the time and the previewing state
        py::dict result;
        result["is_previewing"] = (is_currently_previewing != FALSE);
        result["time_seconds"] = PyAE::AETypeUtils::TimeToSeconds(curr_time);
        result["time_value"] = curr_time.value;
        result["time_scale"] = curr_time.scale;

        return result;
    }, py::arg("item_viewP"),
       R"doc(Get the playback time for an item view.

Each item view can have its own playback time if previewing is active.
If not previewing, returns the default item current time.

Args:
    item_viewP: The item view handle (AEGP_ItemViewP) obtained from AEGP_GetItemMRUView.

Returns:
    dict: A dictionary containing:
        - is_previewing (bool): True if the time is playback time during preview,
                                False if it's the default item current time.
        - time_seconds (float): The current time in seconds.
        - time_value (int): Raw A_Time value.
        - time_scale (int): Raw A_Time scale.

Raises:
    ValueError: If item_viewP is null.
    RuntimeError: If the ItemView Suite is not available or the API call fails.

Note:
    To get an item view handle, use AEGP_GetItemMRUView from ItemSuite.
)doc");

}

} // namespace SDK
} // namespace PyAE
