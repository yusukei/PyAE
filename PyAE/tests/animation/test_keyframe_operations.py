# test_keyframe_operations.py
# PyAE Keyframe Operations テスト (T301)
# Tests for adding, removing, and editing keyframes

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_close,
        assert_isinstance,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_close,
        assert_isinstance,
    )

suite = TestSuite("Keyframe Operations (T301)")

# Test variables
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    """Create test composition and layer"""
    global _test_comp, _test_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_KeyframeTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    _test_layer = _test_comp.add_solid("_TestLayer", 100, 100, (1.0, 0.0, 0.0), 10.0)


@suite.teardown
def teardown():
    """Clean up test composition"""
    global _test_comp
    if _test_comp:
        try:
            _test_comp.delete()
        except:
            pass


@suite.test
def test_add_keyframe_basic():
    """Basic keyframe addition"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Initially no keyframes
    assert_equal(0, pos.num_keyframes, "Should have no keyframes initially")
    assert_false(pos.is_time_varying, "Should not be time-varying initially")

    # Add first keyframe
    idx = pos.add_keyframe(0.0, [100, 200])
    assert_equal(0, idx, "First keyframe index should be 0")
    assert_equal(1, pos.num_keyframes, "Should have 1 keyframe")
    assert_true(pos.is_time_varying, "Should be time-varying after adding keyframe")

    # Add second keyframe
    idx = pos.add_keyframe(1.0, [300, 400])
    assert_equal(1, idx, "Second keyframe index should be 1")
    assert_equal(2, pos.num_keyframes, "Should have 2 keyframes")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_add_keyframe_various_times():
    """Add keyframes at various time positions"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # Add keyframes in non-sequential order
    opacity.add_keyframe(2.0, 50.0)
    opacity.add_keyframe(0.0, 100.0)
    opacity.add_keyframe(1.0, 75.0)

    assert_equal(3, opacity.num_keyframes, "Should have 3 keyframes")

    # Keyframes should be ordered by time
    assert_close(0.0, opacity.get_keyframe_time(0), message="First keyframe should be at t=0")
    assert_close(1.0, opacity.get_keyframe_time(1), message="Second keyframe should be at t=1")
    assert_close(2.0, opacity.get_keyframe_time(2), message="Third keyframe should be at t=2")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_get_keyframe_value():
    """Retrieve keyframe values"""
    global _test_layer
    pos = _test_layer.property("Position")

    pos.add_keyframe(0.0, [100, 200])
    pos.add_keyframe(1.0, [300, 400])

    # Get values
    val0 = pos.get_keyframe_value(0)
    val1 = pos.get_keyframe_value(1)

    assert_close(100.0, val0[0], message="First keyframe X should be 100")
    assert_close(200.0, val0[1], message="First keyframe Y should be 200")
    assert_close(300.0, val1[0], message="Second keyframe X should be 300")
    assert_close(400.0, val1[1], message="Second keyframe Y should be 400")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_get_keyframe_time():
    """Retrieve keyframe times"""
    global _test_layer
    scale = _test_layer.property("Scale")

    scale.add_keyframe(0.5, [100, 100])
    scale.add_keyframe(2.5, [150, 150])

    assert_close(0.5, scale.get_keyframe_time(0), message="First keyframe time should be 0.5")
    assert_close(2.5, scale.get_keyframe_time(1), message="Second keyframe time should be 2.5")

    # Cleanup
    scale.remove_all_keyframes()


@suite.test
def test_set_keyframe_value():
    """Edit keyframe values"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    opacity.add_keyframe(0.0, 100.0)
    opacity.add_keyframe(1.0, 50.0)

    # Modify first keyframe value
    opacity.set_keyframe_value(0, 80.0)

    val = opacity.get_keyframe_value(0)
    assert_close(80.0, val, message="Modified keyframe value should be 80")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_remove_keyframe_single():
    """Remove a single keyframe"""
    global _test_layer
    rot = _test_layer.property("Rotation")

    rot.add_keyframe(0.0, 0.0)
    rot.add_keyframe(1.0, 90.0)
    rot.add_keyframe(2.0, 180.0)

    assert_equal(3, rot.num_keyframes, "Should have 3 keyframes")

    # Remove middle keyframe
    rot.remove_keyframe(1)

    assert_equal(2, rot.num_keyframes, "Should have 2 keyframes after removal")

    # Verify remaining keyframes
    assert_close(0.0, rot.get_keyframe_time(0))
    assert_close(2.0, rot.get_keyframe_time(1))

    # Cleanup
    rot.remove_all_keyframes()


@suite.test
def test_remove_all_keyframes():
    """Remove all keyframes at once"""
    global _test_layer
    pos = _test_layer.property("Position")

    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(1.0, [200, 200])
    pos.add_keyframe(2.0, [300, 300])
    pos.add_keyframe(3.0, [400, 400])

    assert_equal(4, pos.num_keyframes, "Should have 4 keyframes")

    pos.remove_all_keyframes()

    assert_equal(0, pos.num_keyframes, "Should have 0 keyframes after removal")
    assert_false(pos.is_time_varying, "Should not be time-varying after removing all")


@suite.test
def test_keyframes_property():
    """Test keyframes property returns list of keyframe info"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    opacity.add_keyframe(0.0, 100.0)
    opacity.add_keyframe(1.0, 50.0)

    kf_list = opacity.keyframes

    assert_not_none(kf_list, "keyframes property should return data")
    assert_equal(2, len(kf_list), "Should have 2 keyframe entries")

    # Each entry is a Keyframe object with attributes (not dict)
    kf0 = kf_list[0]
    assert_true(hasattr(kf0, 'time'), "Keyframe should have 'time' attribute")
    assert_true(hasattr(kf0, 'value'), "Keyframe should have 'value' attribute")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_value_at_time_with_keyframes():
    """Get interpolated value at specific time"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(1.0, 100.0)

    # Value at midpoint should be interpolated
    val_mid = opacity.get_value_at_time(0.5)
    assert_close(50.0, val_mid, tolerance=1.0, message="Value at t=0.5 should be ~50")

    # Value at keyframe times
    assert_close(0.0, opacity.get_value_at_time(0.0), message="Value at t=0 should be 0")
    assert_close(100.0, opacity.get_value_at_time(1.0), message="Value at t=1 should be 100")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_can_have_keyframes():
    """Test can_have_keyframes property"""
    global _test_layer

    # Position should support keyframes
    pos = _test_layer.property("Position")
    assert_true(pos.can_have_keyframes, "Position should support keyframes")

    # Opacity should support keyframes
    opacity = _test_layer.property("Opacity")
    assert_true(opacity.can_have_keyframes, "Opacity should support keyframes")


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
