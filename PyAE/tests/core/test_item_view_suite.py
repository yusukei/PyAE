# test_item_view_suite.py
# PyAE ItemViewSuite (AEGP_ItemViewSuite1) Tests

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
    )

suite = TestSuite("ItemViewSuite")

# Test variables
_test_comp = None
_plugin_id = None


@suite.setup
def setup():
    """Test setup - create a test composition"""
    global _test_comp, _plugin_id
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_ItemViewSuiteTest", 1920, 1080, 1.0, 5.0, 30.0)
    _plugin_id = ae.sdk.AEGP_GetPluginID()


@suite.teardown
def teardown():
    """Test teardown - cleanup"""
    global _test_comp
    if _test_comp is not None:
        _test_comp.delete()


@suite.test
def test_get_item_mru_view():
    """AEGP_GetItemMRUView returns a view handle for a composition"""
    global _test_comp, _plugin_id

    # Get the item handle for the composition
    item_h = _test_comp._handle
    assert_not_none(item_h, "Item handle should not be None")
    assert_true(item_h != 0, "Item handle should be non-zero")

    # Get the MRU (Most Recently Used) view for this item
    # Note: This may return 0 if no view is open for this item
    view_h = ae.sdk.AEGP_GetItemMRUView(item_h)

    # view_h can be 0 if no viewer is open for the item
    # This is expected behavior when running tests without UI interaction
    assert_isinstance(view_h, int, "View handle should be an integer")


@suite.test
def test_get_item_view_playback_time_with_view():
    """AEGP_GetItemViewPlaybackTime works when a view is available"""
    global _test_comp

    # Get the item handle
    item_h = _test_comp._handle

    # Get the MRU view
    view_h = ae.sdk.AEGP_GetItemMRUView(item_h)

    if view_h == 0:
        # No view available - skip this test
        # This is expected when running automated tests without UI
        print("  (Skipped - no view available for item)")
        return

    # Get the playback time
    result = ae.sdk.AEGP_GetItemViewPlaybackTime(view_h)

    # Validate result structure
    assert_isinstance(result, dict, "Result should be a dictionary")
    assert_true("is_previewing" in result, "Result should contain 'is_previewing'")
    assert_true("time_seconds" in result, "Result should contain 'time_seconds'")
    assert_true("time_value" in result, "Result should contain 'time_value'")
    assert_true("time_scale" in result, "Result should contain 'time_scale'")

    # Validate types
    assert_isinstance(result["is_previewing"], bool, "is_previewing should be a boolean")
    assert_isinstance(result["time_seconds"], float, "time_seconds should be a float")
    assert_isinstance(result["time_value"], int, "time_value should be an int")
    assert_isinstance(result["time_scale"], int, "time_scale should be an int")

    # Time should be non-negative
    assert_true(result["time_seconds"] >= 0.0, "time_seconds should be non-negative")


@suite.test
def test_get_item_view_playback_time_null_check():
    """AEGP_GetItemViewPlaybackTime validates null pointer"""
    # Passing 0 (null) should raise an exception
    error_raised = False
    try:
        ae.sdk.AEGP_GetItemViewPlaybackTime(0)
    except (ValueError, RuntimeError) as e:
        error_raised = True
        # Check that the error message mentions null
        assert_true("null" in str(e).lower() or "cannot be null" in str(e).lower(),
                   f"Error message should mention null: {e}")

    assert_true(error_raised, "Should raise error for null item_viewP")


@suite.test
def test_item_view_suite_available():
    """ItemView Suite should be available in AE 13.6+"""
    # Simply verify that the function exists and is callable
    assert_true(hasattr(ae.sdk, "AEGP_GetItemViewPlaybackTime"),
               "AEGP_GetItemViewPlaybackTime should be available")


@suite.test
def test_get_mru_view_null_check():
    """AEGP_GetItemMRUView validates null pointer"""
    # Passing 0 (null) should raise an exception
    error_raised = False
    try:
        ae.sdk.AEGP_GetItemMRUView(0)
    except (ValueError, RuntimeError) as e:
        error_raised = True
        # Check that the error message mentions null
        assert_true("null" in str(e).lower() or "cannot be null" in str(e).lower(),
                   f"Error message should mention null: {e}")

    assert_true(error_raised, "Should raise error for null itemH")


def run():
    """Run all ItemViewSuite tests"""
    return suite.run()


if __name__ == "__main__":
    run()
