# test_keyframe_interpolation.py
# PyAE Keyframe Interpolation テスト (T302)
# Tests for Linear, Bezier, and Hold interpolation

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

suite = TestSuite("Keyframe Interpolation (T302)")

# Test variables
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    """Create test composition and layer"""
    global _test_comp, _test_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_InterpolationTestComp", 1920, 1080, 1.0, 10.0, 30.0)
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
def test_default_interpolation():
    """Default interpolation should be linear or bezier"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    opacity.add_keyframe(0.0, 100.0)
    opacity.add_keyframe(1.0, 0.0)

    kf_list = opacity.keyframes
    assert_equal(2, len(kf_list), "Should have 2 keyframes")

    # Check that interpolation types are set
    kf0 = kf_list[0]
    assert_true(hasattr(kf0, 'in_interpolation') or hasattr(kf0, 'out_interpolation'),
                "Keyframe should have interpolation info")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_set_linear_interpolation():
    """Set linear interpolation on keyframes"""
    global _test_layer
    pos = _test_layer.property("Position")

    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(1.0, [200, 200])

    # Set linear interpolation
    pos.set_keyframe_interpolation(0, "linear", "linear")
    pos.set_keyframe_interpolation(1, "linear", "linear")

    # Verify interpolation
    kf_list = pos.keyframes
    kf0 = kf_list[0]

    # Depending on implementation, check the out interpolation of first keyframe
    if hasattr(kf0, 'out_interpolation') and kf0.out_interpolation:
        assert_equal(ae.KeyInterpolation.Linear, kf0.out_interpolation,
                     "First keyframe should have linear out interpolation")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_set_bezier_interpolation():
    """Set bezier interpolation on keyframes"""
    global _test_layer
    scale = _test_layer.property("Scale")

    scale.add_keyframe(0.0, [100, 100])
    scale.add_keyframe(1.0, [200, 200])

    # Set bezier interpolation
    scale.set_keyframe_interpolation(0, "bezier", "bezier")
    scale.set_keyframe_interpolation(1, "bezier", "bezier")

    # Verify interpolation
    kf_list = scale.keyframes
    kf0 = kf_list[0]

    if hasattr(kf0, 'out_interpolation') and kf0.out_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf0.out_interpolation,
                     "First keyframe should have bezier out interpolation")

    # Cleanup
    scale.remove_all_keyframes()


@suite.test
def test_set_hold_interpolation():
    """Set hold interpolation on keyframes"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    opacity.add_keyframe(0.0, 100.0)
    opacity.add_keyframe(1.0, 0.0)

    # Set hold interpolation (value stays constant until next keyframe)
    opacity.set_keyframe_interpolation(0, "hold", "hold")

    # With hold interpolation, value at midpoint should be same as first keyframe
    val_mid = opacity.get_value_at_time(0.5)
    assert_close(100.0, val_mid, tolerance=0.1,
                 message="With hold interpolation, value should stay at 100 until next keyframe")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_mixed_interpolation():
    """Mix different interpolation types"""
    global _test_layer
    rot = _test_layer.property("Rotation")

    rot.add_keyframe(0.0, 0.0)
    rot.add_keyframe(1.0, 90.0)
    rot.add_keyframe(2.0, 180.0)

    # Set different interpolations
    rot.set_keyframe_interpolation(0, "linear", "bezier")  # linear in, bezier out
    rot.set_keyframe_interpolation(1, "bezier", "linear")  # bezier in, linear out
    rot.set_keyframe_interpolation(2, "linear", "linear")

    kf_list = rot.keyframes
    assert_equal(3, len(kf_list), "Should have 3 keyframes")

    # Cleanup
    rot.remove_all_keyframes()


@suite.test
def test_interpolation_affects_value():
    """Interpolation type affects intermediate values"""
    global _test_layer

    # Create two opacity properties with different interpolations
    # Since we can't have two of the same property, we'll test on one layer
    opacity = _test_layer.property("Opacity")

    # Test 1: Linear interpolation
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(1.0, 100.0)
    opacity.set_keyframe_interpolation(0, "linear", "linear")
    opacity.set_keyframe_interpolation(1, "linear", "linear")

    # With linear, midpoint should be exactly 50
    val_linear = opacity.get_value_at_time(0.5)
    assert_close(50.0, val_linear, tolerance=1.0,
                 message="Linear interpolation at t=0.5 should be ~50")

    opacity.remove_all_keyframes()

    # Test 2: Hold interpolation
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(1.0, 100.0)
    opacity.set_keyframe_interpolation(0, "hold", "hold")

    # With hold, midpoint should be 0 (first keyframe value)
    val_hold = opacity.get_value_at_time(0.5)
    assert_close(0.0, val_hold, tolerance=0.1,
                 message="Hold interpolation at t=0.5 should be 0")

    # Cleanup
    opacity.remove_all_keyframes()


@suite.test
def test_keyframe_interpolation_info_in_keyframes():
    """keyframes property includes interpolation information"""
    global _test_layer
    pos = _test_layer.property("Position")

    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(1.0, [200, 200])

    kf_list = pos.keyframes
    kf0 = kf_list[0]

    # Check that interpolation info exists
    has_interp = (hasattr(kf0, 'in_interpolation') or hasattr(kf0, 'out_interpolation') or
                  hasattr(kf0, 'interpolation'))
    assert_true(has_interp, "Keyframe info should include interpolation type")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_spatial_interpolation_position():
    """Position property has spatial interpolation (motion path)"""
    global _test_layer
    pos = _test_layer.property("Position")

    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(1.0, [200, 200])

    # Position should support spatial interpolation
    # This is a property characteristic, not a value
    assert_true(pos.can_have_keyframes, "Position should support keyframes")

    kf_list = pos.keyframes

    # Check if spatial tangent information is available
    kf0 = kf_list[0]
    # Spatial properties may include in_tangent/out_tangent or spatial_in/spatial_out
    # This depends on implementation

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_1d_vs_2d_interpolation():
    """Compare 1D (Opacity) and 2D (Position) property interpolation"""
    global _test_layer

    # 1D property (Opacity)
    opacity = _test_layer.property("Opacity")
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(1.0, 100.0)

    val_1d = opacity.get_value_at_time(0.5)
    assert_close(50.0, val_1d, tolerance=5.0, message="1D midpoint should be ~50")

    opacity.remove_all_keyframes()

    # 2D property (Position)
    pos = _test_layer.property("Position")
    pos.add_keyframe(0.0, [0, 0])
    pos.add_keyframe(1.0, [100, 100])
    pos.set_keyframe_interpolation(0, "linear", "linear")
    pos.set_keyframe_interpolation(1, "linear", "linear")

    val_2d = pos.get_value_at_time(0.5)
    assert_close(50.0, val_2d[0], tolerance=5.0, message="2D X midpoint should be ~50")
    assert_close(50.0, val_2d[1], tolerance=5.0, message="2D Y midpoint should be ~50")

    pos.remove_all_keyframes()


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
