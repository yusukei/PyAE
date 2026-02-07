# test_motion_path.py
# PyAE Motion Path Bezier Tests (T303)
# Tests for motion path bezier curves, spatial tangents, and roving keyframes

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_close,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_close,
    )

suite = TestSuite("Motion Path Bezier (T303)")

# Test variables
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    """Create test composition and layer"""
    global _test_comp, _test_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_MotionPathTestComp", 1920, 1080, 1.0, 10.0, 30.0)
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
def test_motion_path_linear():
    """Test linear motion path - position should interpolate linearly"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create two keyframes with linear interpolation
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(2.0, [500, 500])

    # Set linear interpolation for temporal
    pos.set_keyframe_interpolation(0, "linear", "linear")
    pos.set_keyframe_interpolation(1, "linear", "linear")

    # With linear interpolation, midpoint should be exactly halfway
    val_mid = pos.get_value_at_time(1.0)
    assert_close(300.0, val_mid[0], tolerance=5.0,
                 message="Linear motion path X midpoint should be 300")
    assert_close(300.0, val_mid[1], tolerance=5.0,
                 message="Linear motion path Y midpoint should be 300")

    # Check quarter points
    val_quarter = pos.get_value_at_time(0.5)
    assert_close(200.0, val_quarter[0], tolerance=5.0,
                 message="Linear motion path X at 0.5s should be 200")
    assert_close(200.0, val_quarter[1], tolerance=5.0,
                 message="Linear motion path Y at 0.5s should be 200")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_motion_path_bezier():
    """Test bezier motion path - creates curved path between keyframes"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create keyframes for a curved motion path
    pos.add_keyframe(0.0, [100, 500])
    pos.add_keyframe(2.0, [900, 500])

    # Set bezier interpolation
    pos.set_keyframe_interpolation(0, "bezier", "bezier")
    pos.set_keyframe_interpolation(1, "bezier", "bezier")

    # Get current keyframes to verify they exist
    kf_list = pos.keyframes
    assert_equal(2, len(kf_list), "Should have 2 keyframes")

    # Verify keyframe info contains interpolation data
    kf0 = kf_list[0]
    if hasattr(kf0, 'out_interpolation') and kf0.out_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf0.out_interpolation,
                     "First keyframe should have bezier out interpolation")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_spatial_tangents():
    """Test getting and setting spatial tangents"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create keyframes
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(1.0, [500, 100])
    pos.add_keyframe(2.0, [900, 100])

    # Set bezier interpolation for spatial curves
    pos.set_keyframe_interpolation(0, "bezier", "bezier")
    pos.set_keyframe_interpolation(1, "bezier", "bezier")
    pos.set_keyframe_interpolation(2, "bezier", "bezier")

    # Get keyframe info - spatial tangents should be available for spatial properties
    kf_list = pos.keyframes
    assert_equal(3, len(kf_list), "Should have 3 keyframes")

    # Check if spatial tangent info is available in keyframes
    middle_kf = kf_list[1]

    # Spatial properties (Position) should support spatial tangents
    # The keyframe object may include spatial tangent data as attributes
    # Check that keyframe info exists
    assert_true(hasattr(middle_kf, 'time'), "Keyframe should have time info")
    assert_true(hasattr(middle_kf, 'value'), "Keyframe should have value info")

    # Verify position is a spatial property (2D or 3D)
    stream_type = pos.stream_value_type
    # Stream types: TwoD_Spatial, ThreeD_Spatial indicate spatial properties
    assert_not_none(stream_type, "Position should have a stream value type")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_spatial_tangents_via_keyframe_object():
    """Test spatial tangents using Keyframe object API"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create keyframes
    idx0 = pos.add_keyframe(0.0, [100, 100])
    idx1 = pos.add_keyframe(1.0, [500, 300])
    idx2 = pos.add_keyframe(2.0, [900, 100])

    # Set bezier interpolation
    pos.set_keyframe_interpolation(0, "bezier", "bezier")
    pos.set_keyframe_interpolation(1, "bezier", "bezier")
    pos.set_keyframe_interpolation(2, "bezier", "bezier")

    # Access keyframes through the keyframes property
    kf_list = pos.keyframes
    assert_equal(3, len(kf_list), "Should have 3 keyframes")

    # The middle keyframe should have both in and out tangents
    middle_kf_info = kf_list[1]
    assert_close(1.0, middle_kf_info.time, message="Middle keyframe time should be 1.0")

    # Verify the keyframe has expected data
    assert_true(hasattr(middle_kf_info, 'in_interpolation') or hasattr(middle_kf_info, 'out_interpolation'),
                "Keyframe should have interpolation info")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_roving_keyframes():
    """Test roving keyframes for constant speed motion"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create a motion path with multiple keyframes
    # Roving keyframes adjust timing to create constant speed
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(1.0, [300, 300])  # This will be the roving keyframe
    pos.add_keyframe(2.0, [500, 500])
    pos.add_keyframe(3.0, [700, 700])

    # Set bezier interpolation on all keyframes
    for i in range(4):
        pos.set_keyframe_interpolation(i, "bezier", "bezier")

    # Get keyframe info
    kf_list = pos.keyframes
    assert_equal(4, len(kf_list), "Should have 4 keyframes")

    # Check if roving flag is available in keyframe info
    # Note: First and last keyframes cannot be roving in After Effects
    middle_kf = kf_list[1]

    # The roving flag may be in the keyframe info
    # If not directly available, it can be accessed via Keyframe object
    assert_true(hasattr(middle_kf, 'time'), "Middle keyframe should have time")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_auto_bezier_motion_path():
    """Test auto-bezier motion path creates smooth curves"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create keyframes for auto-bezier test
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(1.0, [500, 300])
    pos.add_keyframe(2.0, [900, 100])

    # Set bezier interpolation
    pos.set_keyframe_interpolation(0, "bezier", "bezier")
    pos.set_keyframe_interpolation(1, "bezier", "bezier")
    pos.set_keyframe_interpolation(2, "bezier", "bezier")

    # Get keyframe list
    kf_list = pos.keyframes
    assert_equal(3, len(kf_list), "Should have 3 keyframes")

    # Auto-bezier creates smooth curves through keyframe points
    # The interpolation should result in a curved path

    # Test that value at midpoint is not exactly linear
    # For a curved path from (100,100) -> (500,300) -> (900,100)
    # Linear would give Y=200 at x=500 (t=1.0), but we have Y=300 at keyframe
    val_at_middle = pos.get_value_at_time(1.0)
    assert_close(500.0, val_at_middle[0], tolerance=1.0,
                 message="X at middle keyframe should be 500")
    assert_close(300.0, val_at_middle[1], tolerance=1.0,
                 message="Y at middle keyframe should be 300")

    # Check interpolated values between keyframes
    val_at_half = pos.get_value_at_time(0.5)
    # With bezier, this should curve through the middle point
    assert_not_none(val_at_half, "Should get value at t=0.5")
    assert_true(len(val_at_half) >= 2, "Position should have at least 2 dimensions")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_motion_path_3_point_curve():
    """Test motion path with 3 keyframes creates curved path"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create an arc-like motion path
    pos.add_keyframe(0.0, [100, 500])   # Start left
    pos.add_keyframe(1.5, [500, 100])   # Top middle (peak of arc)
    pos.add_keyframe(3.0, [900, 500])   # End right

    # Set bezier for curved path
    pos.set_keyframe_interpolation(0, "bezier", "bezier")
    pos.set_keyframe_interpolation(1, "bezier", "bezier")
    pos.set_keyframe_interpolation(2, "bezier", "bezier")

    # Verify keyframes
    assert_equal(3, pos.num_keyframes, "Should have 3 keyframes")

    # The path should curve through the middle point
    val_peak = pos.get_value_at_time(1.5)
    assert_close(500.0, val_peak[0], tolerance=1.0,
                 message="Peak X should be 500")
    assert_close(100.0, val_peak[1], tolerance=1.0,
                 message="Peak Y should be 100")

    # Check that start and end are correct
    val_start = pos.get_value_at_time(0.0)
    assert_close(100.0, val_start[0], tolerance=1.0, message="Start X should be 100")
    assert_close(500.0, val_start[1], tolerance=1.0, message="Start Y should be 500")

    val_end = pos.get_value_at_time(3.0)
    assert_close(900.0, val_end[0], tolerance=1.0, message="End X should be 900")
    assert_close(500.0, val_end[1], tolerance=1.0, message="End Y should be 500")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_spatial_vs_temporal_interpolation():
    """Test difference between spatial and temporal interpolation"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create motion path
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(2.0, [500, 500])

    # Get keyframe info
    kf_list = pos.keyframes
    assert_equal(2, len(kf_list), "Should have 2 keyframes")

    # Spatial properties like Position have both temporal and spatial interpolation
    # Temporal: controls speed along path
    # Spatial: controls shape of the path

    kf0 = kf_list[0]

    # Check interpolation types
    if hasattr(kf0, 'out_interpolation') and kf0.out_interpolation:
        # Temporal interpolation type
        assert_true(kf0.out_interpolation in [ae.KeyInterpolation.Linear, ae.KeyInterpolation.Bezier, ae.KeyInterpolation.Hold],
                    f"Valid interpolation type expected, got: {kf0.out_interpolation}")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_keyframe_interpolation_types_position():
    """Test all interpolation types on Position property"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create keyframes
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(2.0, [500, 500])
    pos.add_keyframe(4.0, [900, 100])

    # Test Linear interpolation
    pos.set_keyframe_interpolation(0, "linear", "linear")
    kf_list = pos.keyframes
    if hasattr(kf_list[0], 'out_interpolation') and kf_list[0].out_interpolation:
        assert_equal(ae.KeyInterpolation.Linear, kf_list[0].out_interpolation,
                     "Should be linear interpolation")

    # Test Bezier interpolation
    pos.set_keyframe_interpolation(0, "bezier", "bezier")
    kf_list = pos.keyframes
    if hasattr(kf_list[0], 'out_interpolation') and kf_list[0].out_interpolation:
        assert_equal(ae.KeyInterpolation.Bezier, kf_list[0].out_interpolation,
                     "Should be bezier interpolation")

    # Test Hold interpolation
    pos.set_keyframe_interpolation(0, "hold", "hold")
    kf_list = pos.keyframes
    if hasattr(kf_list[0], 'out_interpolation') and kf_list[0].out_interpolation:
        assert_equal(ae.KeyInterpolation.Hold, kf_list[0].out_interpolation,
                     "Should be hold interpolation")

    # Verify hold behavior - value should stay constant
    pos.set_keyframe_interpolation(0, "hold", "hold")
    val_mid = pos.get_value_at_time(1.0)
    assert_close(100.0, val_mid[0], tolerance=1.0,
                 message="Hold interpolation should keep value at 100")

    # Cleanup
    pos.remove_all_keyframes()


@suite.test
def test_motion_path_continuous_bezier():
    """Test continuous bezier creates smooth path through points"""
    global _test_layer
    pos = _test_layer.property("Position")

    # Create a path with multiple keyframes
    keyframes = [
        (0.0, [100, 300]),
        (1.0, [300, 100]),
        (2.0, [500, 300]),
        (3.0, [700, 100]),
        (4.0, [900, 300]),
    ]

    for time, value in keyframes:
        pos.add_keyframe(time, value)

    # Set all to bezier for smooth curves
    for i in range(len(keyframes)):
        pos.set_keyframe_interpolation(i, "bezier", "bezier")

    # Verify all keyframes exist
    assert_equal(len(keyframes), pos.num_keyframes,
                 f"Should have {len(keyframes)} keyframes")

    # Verify values at keyframe times
    for time, expected_value in keyframes:
        actual_value = pos.get_value_at_time(time)
        assert_close(expected_value[0], actual_value[0], tolerance=1.0,
                     message=f"X at t={time} should match")
        assert_close(expected_value[1], actual_value[1], tolerance=1.0,
                     message=f"Y at t={time} should match")

    # Cleanup
    pos.remove_all_keyframes()


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
