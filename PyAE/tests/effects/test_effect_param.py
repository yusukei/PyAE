# test_effect_param.py
# PyAE Effect Parameter and Keyframe API Tests

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_equal,
        assert_not_none,
        assert_isinstance,
        assert_close,
        assert_greater_than,
        assert_raises,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_equal,
        assert_not_none,
        assert_isinstance,
        assert_close,
        assert_greater_than,
        assert_raises,
    )

suite = TestSuite("EffectParam")

# Test variables
_test_comp = None
_test_layer = None
_test_effect = None


@suite.setup
def setup():
    """Create test composition, solid layer, and Gaussian Blur effect."""
    global _test_comp, _test_layer, _test_effect
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_EffectParamTestComp", 1920, 1080, 1.0, 5.0, 30.0)
    _test_layer = _test_comp.add_solid(
        "_EffectParamTestLayer", 1920, 1080, (1.0, 0.0, 0.0), 5.0
    )
    _test_effect = _test_layer.add_effect("ADBE Gaussian Blur 2")


@suite.teardown
def teardown():
    """Delete the test composition to clean up."""
    global _test_comp
    if _test_comp is not None:
        _test_comp.delete()
        _test_comp = None


def _get_animatable_param():
    """Find the first animatable (non-group) parameter on the effect.

    For Gaussian Blur 2, index 0 is 'Blurriness' (OneD slider).
    If that happens to be a NO_DATA group param, iterate to find
    the first parameter whose get_value() does not return None.
    """
    global _test_effect
    num = _test_effect.num_properties
    for i in range(num):
        p = _test_effect.property_by_index(i)
        val = p.get_value()
        if val is not None:
            return p
    return None


# -----------------------------------------------------------------------
# 1. test_param_by_index
# -----------------------------------------------------------------------
@suite.test
def test_param_by_index():
    """Retrieve a parameter via effect.property_by_index(0)."""
    global _test_effect
    param = _test_effect.property_by_index(0)
    assert_not_none(param, "param_by_index(0) should return a parameter")
    assert_true(param.valid, "Parameter should be valid")


# -----------------------------------------------------------------------
# 2. test_param_name
# -----------------------------------------------------------------------
@suite.test
def test_param_name():
    """param.name returns a non-empty string."""
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")
    name = param.name
    assert_isinstance(name, str)
    assert_true(len(name) > 0, "Parameter name should not be empty")
    print(f"  Animatable param name: {name}")


# -----------------------------------------------------------------------
# 3. test_param_index
# -----------------------------------------------------------------------
@suite.test
def test_param_index():
    """param.index returns a non-negative integer (0-based)."""
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")
    idx = param.index
    assert_isinstance(idx, int)
    assert_true(idx >= 0, f"Parameter index should be >= 0, got {idx}")


# -----------------------------------------------------------------------
# 4. test_param_valid
# -----------------------------------------------------------------------
@suite.test
def test_param_valid():
    """param.valid is True for a successfully acquired parameter."""
    global _test_effect
    param = _test_effect.property_by_index(0)
    assert_true(param.valid, "Parameter should report valid=True")


# -----------------------------------------------------------------------
# 5. test_param_value_get
# -----------------------------------------------------------------------
@suite.test
def test_param_value_get():
    """param.get_value() returns a numeric value for the Blurriness slider."""
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")
    val = param.get_value()
    assert_not_none(val, "get_value() should not return None for animatable param")
    # Blurriness default is typically 0.0
    assert_isinstance(val, (int, float), f"Expected numeric value, got {type(val).__name__}")
    print(f"  Value at t=0: {val}")


# -----------------------------------------------------------------------
# 6. test_param_value_set
# -----------------------------------------------------------------------
@suite.test
def test_param_value_set():
    """param.set_value() changes the parameter value."""
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")

    original = param.get_value()
    new_val = 42.0
    param.set_value(new_val)
    after = param.get_value()
    assert_close(new_val, after, tolerance=0.1,
                 message=f"Expected ~{new_val} after set_value, got {after}")

    # Restore original value
    param.set_value(original)


# -----------------------------------------------------------------------
# 7. test_param_can_vary_over_time
# -----------------------------------------------------------------------
@suite.test
def test_param_can_vary_over_time():
    """param.can_vary_over_time is True for the Blurriness slider."""
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")
    assert_true(param.can_vary_over_time,
                "Blurriness slider should be animatable (can_vary_over_time=True)")


# -----------------------------------------------------------------------
# 8. test_param_num_keyframes_initial
# -----------------------------------------------------------------------
@suite.test
def test_param_num_keyframes_initial():
    """A freshly-added effect parameter has 0 keyframes."""
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")
    assert_equal(0, param.num_keyframes,
                 "Initial keyframe count should be 0")


# -----------------------------------------------------------------------
# 9. test_param_add_keyframe
# -----------------------------------------------------------------------
@suite.test
def test_param_add_keyframe():
    """add_keyframe(time, value) inserts a keyframe and returns its index."""
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")

    # Add two keyframes
    idx0 = param.add_keyframe(0.0, 10.0)
    assert_isinstance(idx0, int, "add_keyframe should return an int index")
    assert_true(idx0 >= 0, f"Keyframe index should be >= 0, got {idx0}")

    idx1 = param.add_keyframe(2.0, 50.0)
    assert_true(idx1 >= 0, f"Second keyframe index should be >= 0, got {idx1}")

    num = param.num_keyframes
    assert_equal(2, num, f"Expected 2 keyframes after adding two, got {num}")

    # Verify value at first keyframe time
    val_at_0 = param.get_value(0.0)
    assert_close(10.0, val_at_0, tolerance=0.1,
                 message=f"Value at t=0 should be ~10.0, got {val_at_0}")


# -----------------------------------------------------------------------
# 10. test_param_remove_keyframe
# -----------------------------------------------------------------------
@suite.test
def test_param_remove_keyframe():
    """remove_keyframe(index) deletes a keyframe."""
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")

    # Ensure keyframes exist from previous test; add if needed
    if param.num_keyframes == 0:
        param.add_keyframe(0.0, 10.0)
        param.add_keyframe(2.0, 50.0)

    before = param.num_keyframes
    assert_true(before > 0, "Need at least one keyframe to test removal")

    # Remove the last keyframe (highest index)
    param.remove_keyframe(before - 1)
    after = param.num_keyframes
    assert_equal(before - 1, after,
                 f"Expected {before - 1} keyframes after removal, got {after}")

    # Clean up remaining keyframes (remove from end to start)
    while param.num_keyframes > 0:
        param.remove_keyframe(param.num_keyframes - 1)

    assert_equal(0, param.num_keyframes, "All keyframes should be removed")


# -----------------------------------------------------------------------
# 11. test_effect_to_dict
# -----------------------------------------------------------------------
@suite.test
def test_effect_to_dict():
    """effect.to_dict() returns a dictionary with expected keys."""
    global _test_effect
    data = _test_effect.to_dict()
    assert_isinstance(data, dict, "to_dict() should return a dict")
    assert_true("match_name" in data,
                "Serialized dict should contain 'match_name'")
    assert_equal("ADBE Gaussian Blur 2", data["match_name"],
                 "match_name should be 'ADBE Gaussian Blur 2'")
    print(f"  to_dict keys: {list(data.keys())}")


# -----------------------------------------------------------------------
# 12. test_effect_update_from_dict
# -----------------------------------------------------------------------
@suite.test
def test_effect_update_from_dict():
    """effect.update_from_dict(data) restores parameter values."""
    global _test_effect

    # Set a known value on the animatable parameter
    param = _get_animatable_param()
    assert_not_none(param, "Should find an animatable parameter")
    param.set_value(75.0)

    # Export
    data = _test_effect.to_dict()

    # Change the value
    param.set_value(0.0)
    assert_close(0.0, param.get_value(), tolerance=0.1,
                 message="Value should be 0 after reset")

    # Restore from dict
    _test_effect.update_from_dict(data)

    # Verify the value was restored
    restored = param.get_value()
    assert_close(75.0, restored, tolerance=0.5,
                 message=f"Value should be ~75.0 after update_from_dict, got {restored}")

    # Reset to default
    param.set_value(0.0)


def run():
    """Run the test suite."""
    return suite.run()


if __name__ == "__main__":
    run()
