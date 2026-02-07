# test_render_monitor.py
# Tests for ae.render_monitor high-level API
#
# This module tests the high-level render queue monitoring API.
#
# Note: Full functionality requires an active render session with
# valid session_id. Using session_id=0 for basic testing.

import ae

try:
    from ..test_utils import (
        TestSuite, assert_true, assert_false, assert_equal,
        assert_not_none, assert_none, assert_isinstance,
        assert_close, skip,
    )
except ImportError:
    from test_utils import (
        TestSuite, assert_true, assert_false, assert_equal,
        assert_not_none, assert_none, assert_isinstance,
        assert_close, skip,
    )

suite = TestSuite("RenderMonitor API")


# -----------------------------------------------------------------------
# Status Constants Tests
# -----------------------------------------------------------------------

@suite.test
def test_status_constants():
    """Test that status constants are defined"""
    assert_equal(0, ae.render_monitor.STATUS_UNKNOWN)
    assert_equal(1, ae.render_monitor.STATUS_SUCCEEDED)
    assert_equal(2, ae.render_monitor.STATUS_ABORTED)
    assert_equal(3, ae.render_monitor.STATUS_ERRED)


# -----------------------------------------------------------------------
# Render Monitor Info Tests
#
# Note: These tests may return errors if no render session is active.
# The API is primarily designed for use during active rendering.
# -----------------------------------------------------------------------

@suite.test
def test_get_app_version():
    """Test getting app version (may fail without active render session)"""
    # API may return error without active session, so skip if not available
    try:
        version = ae.render_monitor.get_app_version(session_id=0)
        assert_not_none(version)
    except RuntimeError:
        # Expected if no render session is active
        skip("No active render session - get_app_version requires session_id")


@suite.test
def test_get_project_name():
    """Test getting project name (may fail without active render session)"""
    try:
        name = ae.render_monitor.get_project_name(session_id=0)
        assert_not_none(name)
    except RuntimeError:
        # Expected if no render session is active
        skip("No active render session - get_project_name requires session_id")


@suite.test
def test_get_num_job_items():
    """Test getting number of job items (may fail without active render session)"""
    try:
        num = ae.render_monitor.get_num_job_items(session_id=0)
        assert_isinstance(num, int)
        assert_true(num >= 0, "Number of job items should be non-negative")
    except RuntimeError:
        # Expected if no render session is active
        skip("No active render session - get_num_job_items requires session_id")


# -----------------------------------------------------------------------
# Module-level Tests
# -----------------------------------------------------------------------

@suite.test
def test_module_docstring():
    """Test that module has documentation"""
    assert_not_none(ae.render_monitor.__doc__)
    assert_true(len(ae.render_monitor.__doc__) > 0,
                "Module docstring should not be empty")


# -----------------------------------------------------------------------
# Callback Bridge Tests
#
# Tests the ability to register Python callbacks that receive
# C++ render queue monitoring events.
# -----------------------------------------------------------------------

@suite.test
def test_callback_functions_exist():
    """Test that callback bridge functions are available"""
    assert_true(hasattr(ae.render_monitor, 'register_listener'),
                "Should have 'register_listener'")
    assert_true(hasattr(ae.render_monitor, 'unregister_listener'),
                "Should have 'unregister_listener'")
    assert_true(hasattr(ae.render_monitor, 'is_listener_registered'),
                "Should have 'is_listener_registered'")


@suite.test
def test_callback_setters_exist():
    """Test that callback setter functions are available"""
    assert_true(hasattr(ae.render_monitor, 'set_on_job_started'),
                "Should have 'set_on_job_started'")
    assert_true(hasattr(ae.render_monitor, 'set_on_job_ended'),
                "Should have 'set_on_job_ended'")
    assert_true(hasattr(ae.render_monitor, 'set_on_item_started'),
                "Should have 'set_on_item_started'")
    assert_true(hasattr(ae.render_monitor, 'set_on_item_updated'),
                "Should have 'set_on_item_updated'")
    assert_true(hasattr(ae.render_monitor, 'set_on_item_ended'),
                "Should have 'set_on_item_ended'")
    assert_true(hasattr(ae.render_monitor, 'set_on_report_log'),
                "Should have 'set_on_report_log'")


@suite.test
def test_initial_listener_state():
    """Test that listener is not registered initially"""
    # Make sure we start from a clean state
    if ae.render_monitor.is_listener_registered():
        ae.render_monitor.unregister_listener()
    assert_false(ae.render_monitor.is_listener_registered(),
                 "Listener should not be registered initially")


@suite.test
def test_set_callback_before_registration():
    """Test setting callbacks before registration"""
    callback_called = []

    def on_job_started(session_id):
        callback_called.append(('job_started', session_id))

    # Setting callback before registration should work (no exception)
    ae.render_monitor.set_on_job_started(on_job_started)

    # Callback won't be invoked until listener is registered
    # and actual render events occur


@suite.test
def test_clear_callback():
    """Test clearing callbacks with None"""
    ae.render_monitor.set_on_job_started(None)
    ae.render_monitor.set_on_job_ended(None)
    ae.render_monitor.set_on_item_started(None)
    ae.render_monitor.set_on_item_updated(None)
    ae.render_monitor.set_on_item_ended(None)
    ae.render_monitor.set_on_report_log(None)
    # If no exception raised, test passes


@suite.test
def test_callback_docstrings():
    """Test that callback functions have documentation"""
    assert_not_none(ae.render_monitor.register_listener.__doc__,
                    "register_listener should have a docstring")
    assert_not_none(ae.render_monitor.set_on_job_started.__doc__,
                    "set_on_job_started should have a docstring")


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
