# test_persistent_data.py
# Tests for ae.persistent_data high-level API
#
# This module tests the high-level persistent data API that provides
# Dict-like access to After Effects preferences storage.

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

suite = TestSuite("PersistentData API")


# -----------------------------------------------------------------------
# PersistentBlob and Section Access Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_machine_specific():
    """Test getting machine-specific preferences blob"""
    prefs = ae.persistent_data.get_machine_specific()
    assert_not_none(prefs)


@suite.test
def test_get_machine_independent():
    """Test getting machine-independent preferences blob"""
    prefs = ae.persistent_data.get_machine_independent()
    assert_not_none(prefs)


@suite.test
def test_get_section():
    """Test getting a section from preferences"""
    prefs = ae.persistent_data.get_machine_specific()
    section = prefs["PyAE_Test"]
    assert_not_none(section)


# -----------------------------------------------------------------------
# Dict-like Section Access Tests
# -----------------------------------------------------------------------

@suite.test
def test_set_get_string():
    """Test setting and getting string values"""
    prefs = ae.persistent_data.get_machine_specific()

    # Set value
    prefs.set_string("PyAE_Test", "test_string", "hello world")

    # Get value
    value = prefs.get_string("PyAE_Test", "test_string", "default")
    assert_equal("hello world", value)

    # Clean up
    prefs.delete("PyAE_Test", "test_string")


@suite.test
def test_set_get_int():
    """Test setting and getting integer values"""
    prefs = ae.persistent_data.get_machine_specific()

    # Set value
    prefs.set_int("PyAE_Test", "test_int", 42)

    # Get value
    value = prefs.get_int("PyAE_Test", "test_int", 0)
    assert_equal(42, value)

    # Clean up
    prefs.delete("PyAE_Test", "test_int")


@suite.test
def test_set_get_float():
    """Test setting and getting float values"""
    prefs = ae.persistent_data.get_machine_specific()

    # Set value
    prefs.set_float("PyAE_Test", "test_float", 3.14159)

    # Get value
    value = prefs.get_float("PyAE_Test", "test_float", 0.0)
    assert_close(3.14159, value, tolerance=0.0001)

    # Clean up
    prefs.delete("PyAE_Test", "test_float")


@suite.test
def test_default_value():
    """Test that default value is returned for non-existent key"""
    prefs = ae.persistent_data.get_machine_specific()

    # Non-existent key should return default
    value = prefs.get_string("PyAE_Test", "non_existent_key", "default_value")
    assert_equal("default_value", value)


@suite.test
def test_delete():
    """Test deleting a key"""
    prefs = ae.persistent_data.get_machine_specific()

    # Set value
    prefs.set_string("PyAE_Test", "to_delete", "temp")

    # Verify it exists
    value = prefs.get_string("PyAE_Test", "to_delete", "")
    assert_equal("temp", value)

    # Delete
    prefs.delete("PyAE_Test", "to_delete")

    # Verify it's gone (returns default)
    value = prefs.get_string("PyAE_Test", "to_delete", "default")
    assert_equal("default", value)


# -----------------------------------------------------------------------
# Prefs Directory Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_prefs_directory():
    """Test getting the preferences directory path"""
    prefs_dir = ae.persistent_data.get_prefs_directory()
    assert_not_none(prefs_dir)
    assert_true(len(prefs_dir) > 0, "Prefs directory path should not be empty")


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
