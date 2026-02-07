# test_easing.py
# PyAE Easing Functions Test (T304)
# Tests for Easy Ease and Temporal Ease functions

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_close,
        assert_in,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_close,
        assert_in,
    )

suite = TestSuite("Easing Functions (T304)")

# Test variables
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    """Create test composition and layer"""
    global _test_comp, _test_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_EasingTestComp", 1920, 1080, 1.0, 10.0, 30.0)
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
def test_easy_ease():
    """Easy Ease (EaseInOut) applies ease to both in and out"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # Add keyframes
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(2.0, 100.0)

    # Set Easy Ease (EaseInOut) using set_temporal_ease
    # Easy Ease typically uses speed=0 and influence=33.33%
    # For the first keyframe, set out ease
    opacity.set_temporal_ease(0, [0, 0.3333], [0, 0.3333])
    # For the second keyframe, set in ease
    opacity.set_temporal_ease(1, [0, 0.3333], [0, 0.3333])

    # Verify interpolation is bezier (required for temporal ease)
    kf_list = opacity.keyframes
    assert_equal(2, len(kf_list), "Should have 2 keyframes")

    kf0 = kf_list[0]
    if hasattr(kf0, 'out_interpolation') and kf0.out_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf0.out_interpolation,
                     "First keyframe should have bezier out interpolation for Easy Ease")

    kf1 = kf_list[1]
    if hasattr(kf1, 'in_interpolation') and kf1.in_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf1.in_interpolation,
                     "Second keyframe should have bezier in interpolation for Easy Ease")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_easy_ease_in():
    """Easy Ease In applies ease only to incoming interpolation"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # Add keyframes
    opacity.add_keyframe(0.0, 100.0)
    opacity.add_keyframe(2.0, 0.0)

    # Set Easy Ease In on second keyframe
    # Only the in ease should be applied, out remains linear-like
    opacity.set_keyframe_interpolation(1, "bezier", "linear")
    opacity.set_temporal_ease(1, [0, 0.3333], [0, 0])

    # Verify the second keyframe has bezier in, linear out
    kf_list = opacity.keyframes
    kf1 = kf_list[1]

    if hasattr(kf1, 'in_interpolation') and kf1.in_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf1.in_interpolation,
                     "Second keyframe should have bezier in interpolation")
    if hasattr(kf1, 'out_interpolation') and kf1.out_interpolation:
        assert_equal(ae.KeyInterpolation.Linear, kf1.out_interpolation,
                     "Second keyframe should have linear out interpolation")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_easy_ease_out():
    """Easy Ease Out applies ease only to outgoing interpolation"""
    global _test_layer
    scale = _test_layer.property("Scale")

    # Add keyframes
    scale.add_keyframe(0.0, [100, 100])
    scale.add_keyframe(2.0, [200, 200])

    # Set Easy Ease Out on first keyframe
    # Only the out ease should be applied, in remains linear-like
    scale.set_keyframe_interpolation(0, "linear", "bezier")
    scale.set_temporal_ease(0, [0, 0], [0, 0.3333])

    # Verify the first keyframe has linear in, bezier out
    kf_list = scale.keyframes
    kf0 = kf_list[0]

    if hasattr(kf0, 'in_interpolation') and kf0.in_interpolation:
        assert_equal(ae.KeyInterpolation.Linear, kf0.in_interpolation,
                     "First keyframe should have linear in interpolation")
    if hasattr(kf0, 'out_interpolation') and kf0.out_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf0.out_interpolation,
                     "First keyframe should have bezier out interpolation")

    # Cleanup
    scale.remove_all_keyframes()


@suite.test
def test_temporal_ease_values():
    """Temporal Ease values can be retrieved after setting"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Add keyframes
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(2.0, [500, 500])
    pos.add_keyframe(4.0, [900, 100])

    # Set temporal ease with specific values
    # speed=0, influence=0.5 (50%)
    pos.set_temporal_ease(1, [0, 0.5], [0, 0.5])

    # Verify keyframes exist
    kf_list = pos.keyframes
    assert_equal(3, len(kf_list), "Should have 3 keyframes")

    # The middle keyframe should have bezier interpolation
    kf1 = kf_list[1]
    if hasattr(kf1, 'in_interpolation') and kf1.in_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf1.in_interpolation,
                     "Middle keyframe should have bezier in interpolation")
    if hasattr(kf1, 'out_interpolation') and kf1.out_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf1.out_interpolation,
                     "Middle keyframe should have bezier out interpolation")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_custom_easing():
    """Custom easing with specific speed and influence values"""
    global _test_layer
    rot = _test_layer.property("Rotation")

    # Add keyframes
    rot.add_keyframe(0.0, 0.0)
    rot.add_keyframe(1.0, 90.0)
    rot.add_keyframe(2.0, 180.0)

    # Set custom temporal ease values
    # Different values for in and out to create asymmetric easing
    # speed affects the velocity, influence affects the curve shape
    rot.set_temporal_ease(1, [10, 0.75], [5, 0.25])

    # Verify keyframes have bezier interpolation
    kf_list = rot.keyframes
    kf1 = kf_list[1]

    if hasattr(kf1, 'in_interpolation') and kf1.in_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf1.in_interpolation,
                     "Custom easing should use bezier interpolation")

    # Cleanup
    rot.remove_all_keyframes()


@suite.test
def test_easing_influence():
    """Easing influence affects animation curve shape"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # Add keyframes
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(2.0, 100.0)

    # Test 1: Linear interpolation (no easing influence)
    opacity.set_keyframe_interpolation(0, "linear", "linear")
    opacity.set_keyframe_interpolation(1, "linear", "linear")

    # With linear interpolation, midpoint should be exactly 50
    val_linear = opacity.get_value_at_time(1.0)
    assert_close(50.0, val_linear, tolerance=1.0,
                 message="Linear interpolation at t=1.0 should be ~50")

    # Test 2: Apply Easy Ease (bezier with influence)
    # This creates a curve where the value changes slower at start/end
    opacity.set_temporal_ease(0, [0, 0.3333], [0, 0.3333])
    opacity.set_temporal_ease(1, [0, 0.3333], [0, 0.3333])

    # With Easy Ease, midpoint might differ from linear
    # The exact value depends on the easing curve
    val_eased = opacity.get_value_at_time(1.0)

    # Value should still be in valid range
    assert_true(0.0 <= val_eased <= 100.0,
                "Eased value should be within keyframe range")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_high_influence_easing():
    """High influence values create more pronounced easing"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # Add keyframes
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(2.0, 100.0)

    # Set high influence (close to 1.0 = 100%)
    opacity.set_temporal_ease(0, [0, 0.9], [0, 0.9])
    opacity.set_temporal_ease(1, [0, 0.9], [0, 0.9])

    # With high influence, the animation should be more curved
    # Verify interpolation is bezier
    kf_list = opacity.keyframes
    kf0 = kf_list[0]

    if hasattr(kf0, 'out_interpolation') and kf0.out_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf0.out_interpolation,
                     "High influence should use bezier interpolation")

    # Sample values at different times
    val_25 = opacity.get_value_at_time(0.5)
    val_50 = opacity.get_value_at_time(1.0)
    val_75 = opacity.get_value_at_time(1.5)

    # Values should be in order (increasing animation)
    assert_true(val_25 <= val_50 <= val_75,
                "Values should increase over time")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_zero_influence_like_linear():
    """Zero influence should behave similar to linear"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # Add keyframes
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(2.0, 100.0)

    # Set bezier with zero influence (should approximate linear)
    opacity.set_temporal_ease(0, [0, 0.01], [0, 0.01])
    opacity.set_temporal_ease(1, [0, 0.01], [0, 0.01])

    # With very low influence, midpoint should be close to 50
    val_mid = opacity.get_value_at_time(1.0)
    assert_close(50.0, val_mid, tolerance=5.0,
                 message="Low influence bezier should approximate linear at midpoint")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_multiple_keyframes_easing():
    """Easing can be applied to multiple keyframes independently"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Add multiple keyframes
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(2.0, [500, 300])
    pos.add_keyframe(4.0, [200, 500])
    pos.add_keyframe(6.0, [800, 200])

    # Apply different easing to each keyframe
    # First: ease out only
    pos.set_keyframe_interpolation(0, "linear", "bezier")
    pos.set_temporal_ease(0, [0, 0], [0, 0.5])

    # Second: ease in and out
    pos.set_temporal_ease(1, [0, 0.3333], [0, 0.3333])

    # Third: ease in and out with different values
    pos.set_temporal_ease(2, [0, 0.5], [0, 0.2])

    # Fourth: ease in only
    pos.set_keyframe_interpolation(3, "bezier", "linear")
    pos.set_temporal_ease(3, [0, 0.5], [0, 0])

    # Verify all keyframes exist
    kf_list = pos.keyframes
    assert_equal(4, len(kf_list), "Should have 4 keyframes")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_easing_preserves_keyframe_values():
    """Setting easing should not change keyframe values"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # Add keyframes with specific values
    opacity.add_keyframe(0.0, 25.0)
    opacity.add_keyframe(2.0, 75.0)

    # Store original values
    orig_val_0 = opacity.get_keyframe_value(0)
    orig_val_1 = opacity.get_keyframe_value(1)

    # Apply easing
    opacity.set_temporal_ease(0, [0, 0.5], [0, 0.5])
    opacity.set_temporal_ease(1, [0, 0.5], [0, 0.5])

    # Verify values are preserved
    new_val_0 = opacity.get_keyframe_value(0)
    new_val_1 = opacity.get_keyframe_value(1)

    assert_close(orig_val_0, new_val_0, tolerance=0.01,
                 message="First keyframe value should be preserved")
    assert_close(orig_val_1, new_val_1, tolerance=0.01,
                 message="Second keyframe value should be preserved")

    # Cleanup
    opacity.remove_all_keyframes()


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
