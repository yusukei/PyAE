# test_menu.py
# Tests for ae.menu high-level API
#
# This module tests the high-level menu command API that provides
# menu command registration and management functionality.

import ae

try:
    from ..test_utils import (
        TestSuite, assert_true, assert_false, assert_equal,
        assert_not_none, assert_none, assert_isinstance,
        assert_close, assert_in, skip,
    )
except ImportError:
    from test_utils import (
        TestSuite, assert_true, assert_false, assert_equal,
        assert_not_none, assert_none, assert_isinstance,
        assert_close, assert_in, skip,
    )

suite = TestSuite("Menu API")


# -----------------------------------------------------------------------
# Menu ID Constants Tests
# -----------------------------------------------------------------------

@suite.test
def test_menu_id_constants():
    """Test that menu ID constants are defined"""
    assert_equal(2, ae.menu.FILE)
    assert_equal(3, ae.menu.EDIT)
    assert_equal(4, ae.menu.COMPOSITION)
    assert_equal(5, ae.menu.LAYER)
    assert_equal(6, ae.menu.EFFECT)
    assert_equal(7, ae.menu.WINDOW)
    assert_equal(14, ae.menu.ANIMATION)


# -----------------------------------------------------------------------
# MenuCommand Tests
# -----------------------------------------------------------------------

@suite.test
def test_register_command():
    """Test registering a menu command"""
    cmd = ae.menu.register_command("PyAE Test Command", ae.menu.WINDOW)

    assert_not_none(cmd)
    assert_equal("PyAE Test Command", cmd.name)
    assert_equal(ae.menu.WINDOW, cmd.menu_id)
    assert_true(cmd.command_id > 0, "command_id should be positive")

    # Clean up
    cmd.remove()


@suite.test
def test_command_enabled():
    """Test enabling/disabling a command"""
    cmd = ae.menu.register_command("PyAE Test Enable", ae.menu.WINDOW)

    # Default should be enabled
    assert_true(cmd.enabled, "Default should be enabled")

    # Disable
    cmd.enabled = False
    assert_false(cmd.enabled, "Should be disabled after setting to False")

    # Re-enable
    cmd.enabled = True
    assert_true(cmd.enabled, "Should be enabled after setting to True")

    # Clean up
    cmd.remove()


@suite.test
def test_command_checked():
    """Test checking/unchecking a command"""
    cmd = ae.menu.register_command("PyAE Test Check", ae.menu.WINDOW)

    # Default should be unchecked
    assert_false(cmd.checked, "Default should be unchecked")

    # Check
    cmd.checked = True
    assert_true(cmd.checked, "Should be checked after setting to True")

    # Uncheck
    cmd.checked = False
    assert_false(cmd.checked, "Should be unchecked after setting to False")

    # Clean up
    cmd.remove()


@suite.test
def test_command_rename():
    """Test renaming a command"""
    cmd = ae.menu.register_command("Original Name", ae.menu.WINDOW)

    assert_equal("Original Name", cmd.name)

    # Rename
    cmd.name = "New Name"
    assert_equal("New Name", cmd.name)

    # Clean up
    cmd.remove()


# -----------------------------------------------------------------------
# Menu Helper Functions Tests
# -----------------------------------------------------------------------

@suite.test
def test_add_to_window():
    """Test add_to_window helper"""
    cmd = ae.menu.add_to_window("PyAE Window Test")

    assert_not_none(cmd)
    assert_equal(ae.menu.WINDOW, cmd.menu_id)

    cmd.remove()


@suite.test
def test_add_to_file():
    """Test add_to_file helper"""
    cmd = ae.menu.add_to_file("PyAE File Test")

    assert_not_none(cmd)
    assert_equal(ae.menu.FILE, cmd.menu_id)

    cmd.remove()


@suite.test
def test_get_unique_id():
    """Test getting unique command ID"""
    id1 = ae.menu.get_unique_id()
    id2 = ae.menu.get_unique_id()

    assert_true(id1 != id2, "Each call should return a different ID")


# -----------------------------------------------------------------------
# String Representation Tests
# -----------------------------------------------------------------------

@suite.test
def test_command_repr():
    """Test __repr__ method"""
    cmd = ae.menu.register_command("Repr Test", ae.menu.WINDOW)

    repr_str = repr(cmd)
    assert_true("MenuCommand" in repr_str, "repr should contain 'MenuCommand'")
    assert_true("Repr Test" in repr_str, "repr should contain command name")

    cmd.remove()


# -----------------------------------------------------------------------
# Callback Tests
# -----------------------------------------------------------------------

@suite.test
def test_callback_functions_exist():
    """Test that callback-related functions exist"""
    assert_true(hasattr(ae.menu, 'clear_all_callbacks'),
                "ae.menu should have 'clear_all_callbacks'")
    assert_true(hasattr(ae.menu, 'get_registered_command_ids'),
                "ae.menu should have 'get_registered_command_ids'")


@suite.test
def test_register_with_callback():
    """Test registering a command with a callback"""
    callback_data = []

    def my_callback():
        callback_data.append('called')

    # Register with callback
    cmd = ae.menu.register_command(
        "Callback Test", ae.menu.WINDOW, my_callback
    )

    assert_not_none(cmd)
    assert_true(cmd.command_id > 0, "command_id should be positive")

    # Verify command is in registered list
    registered = ae.menu.get_registered_command_ids()
    assert_in(cmd.command_id, registered)

    # Clean up
    cmd.remove()


@suite.test
def test_set_on_click():
    """Test set_on_click method"""
    cmd = ae.menu.register_command("Click Test", ae.menu.WINDOW)

    # Command should have set_on_click
    assert_true(hasattr(cmd, 'set_on_click'),
                "Command should have 'set_on_click' method")

    # Set callback
    cmd.set_on_click(lambda: None)

    # Clear callback
    cmd.set_on_click(None)

    cmd.remove()


@suite.test
def test_set_on_update():
    """Test set_on_update method"""
    cmd = ae.menu.register_command("Update Test", ae.menu.WINDOW)

    # Method should exist
    assert_true(hasattr(cmd, 'set_on_update'),
                "Command should have 'set_on_update' method")

    # Set update callback
    cmd.set_on_update(lambda: True)

    cmd.remove()


@suite.test
def test_clear_all_callbacks():
    """Test clearing all callbacks"""
    cmd1 = ae.menu.register_command("Clear Test 1", ae.menu.WINDOW, lambda: None)
    cmd2 = ae.menu.register_command("Clear Test 2", ae.menu.WINDOW, lambda: None)

    # Both should be registered
    registered = ae.menu.get_registered_command_ids()
    assert_in(cmd1.command_id, registered)
    assert_in(cmd2.command_id, registered)

    # Clear all
    ae.menu.clear_all_callbacks()

    # Should be empty now
    registered_after = ae.menu.get_registered_command_ids()
    assert_equal(0, len(registered_after))

    # Clean up menu items
    cmd1.remove()
    cmd2.remove()


@suite.test
def test_helper_with_callback():
    """Test helper functions with callbacks"""
    called = []

    cmd = ae.menu.add_to_window("Helper Callback", lambda: called.append(1))
    assert_not_none(cmd)

    # Verify registered
    registered = ae.menu.get_registered_command_ids()
    assert_in(cmd.command_id, registered)

    cmd.remove()


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
