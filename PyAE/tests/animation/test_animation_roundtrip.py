# test_animation_roundtrip.py
# PyAE Animation Roundtrip Test

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
    )

suite = TestSuite("Animation Roundtrip")

# Test variables
_test_comp = None
_source_layer = None
_target_layer = None


@suite.setup
def setup():
    """Setup test environment"""
    global _test_comp, _source_layer, _target_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_AnimRoundtripComp", 1920, 1080, 1.0, 10.0, 30.0)

    # Create two layers for roundtrip testing
    _source_layer = _test_comp.add_solid("_Source", 100, 100, (1.0, 0.0, 0.0), 10.0)
    _target_layer = _test_comp.add_solid("_Target", 100, 100, (0.0, 1.0, 0.0), 10.0)


@suite.teardown
def teardown():
    """Cleanup test environment"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except:
        pass


# =============================================================
# Position Animation Roundtrip
# =============================================================

@suite.test
def test_position_keyframe_roundtrip():
    """Test copying position animation from one layer to another"""
    # Set keyframes on source layer using property API
    src_prop = _source_layer.get_property("ADBE Position")
    assert_not_none(src_prop, "Position property should exist")

    src_prop.add_keyframe(0.0, [100, 100])
    src_prop.add_keyframe(3.0, [500, 500])
    src_prop.add_keyframe(6.0, [900, 100])

    num_kf = src_prop.num_keyframes
    assert_equal(num_kf, 3, "Source should have 3 keyframes")

    # Copy keyframes to target layer
    tgt_prop = _target_layer.get_property("ADBE Position")
    for i in range(num_kf):
        time = src_prop.keyframe_time(i)
        value = src_prop.get_value_at_time(time)
        tgt_prop.add_keyframe(time, value)

    # Verify target has same keyframes
    assert_equal(tgt_prop.num_keyframes, 3, "Target should have 3 keyframes")

    # Verify values match
    for i in range(num_kf):
        src_time = src_prop.keyframe_time(i)
        tgt_time = tgt_prop.keyframe_time(i)
        assert_true(abs(src_time - tgt_time) < 0.01, "Keyframe times should match")

        src_val = src_prop.get_value_at_time(src_time)
        tgt_val = tgt_prop.get_value_at_time(tgt_time)
        assert_true(abs(src_val[0] - tgt_val[0]) < 0.1, "X values should match")
        assert_true(abs(src_val[1] - tgt_val[1]) < 0.1, "Y values should match")


@suite.test
def test_scale_keyframe_roundtrip():
    """Test copying scale animation from one layer to another"""
    # Set scale keyframes on source using property API
    src_prop = _source_layer.get_property("ADBE Scale")
    assert_not_none(src_prop, "Scale property should exist")

    src_prop.add_keyframe(0.0, [100, 100])
    src_prop.add_keyframe(2.0, [150, 150])
    src_prop.add_keyframe(4.0, [50, 50])

    assert_equal(src_prop.num_keyframes, 3, "Source should have 3 scale keyframes")

    # Copy to target
    tgt_prop = _target_layer.get_property("ADBE Scale")
    for i in range(src_prop.num_keyframes):
        time = src_prop.keyframe_time(i)
        value = src_prop.get_value_at_time(time)
        tgt_prop.add_keyframe(time, value)

    # Verify
    assert_equal(tgt_prop.num_keyframes, 3, "Target should have 3 scale keyframes")

    for i in range(3):
        src_time = src_prop.keyframe_time(i)
        tgt_time = tgt_prop.keyframe_time(i)
        assert_true(abs(src_time - tgt_time) < 0.01, "Scale keyframe times should match")


@suite.test
def test_rotation_keyframe_roundtrip():
    """Test copying rotation animation from one layer to another"""
    # Set rotation keyframes
    _source_layer.set_rotation_keyframe(0.0, 0)
    _source_layer.set_rotation_keyframe(3.0, 180)
    _source_layer.set_rotation_keyframe(6.0, 360)

    # Get rotation property
    src_prop = _source_layer.get_property("ADBE Rotate Z")
    assert_equal(src_prop.num_keyframes, 3, "Source should have 3 rotation keyframes")

    # Copy to target
    for i in range(src_prop.num_keyframes):
        time = src_prop.keyframe_time(i)
        value = src_prop.get_value_at_time(time)
        _target_layer.set_rotation_keyframe(time, value)

    # Verify
    tgt_prop = _target_layer.get_property("ADBE Rotate Z")
    assert_equal(tgt_prop.num_keyframes, 3, "Target should have 3 rotation keyframes")

    for i in range(3):
        src_val = src_prop.get_value_at_time(src_prop.keyframe_time(i))
        tgt_val = tgt_prop.get_value_at_time(tgt_prop.keyframe_time(i))
        assert_true(abs(src_val - tgt_val) < 0.1, "Rotation values should match")


@suite.test
def test_opacity_keyframe_roundtrip():
    """Test copying opacity animation from one layer to another"""
    # Set opacity keyframes
    _source_layer.set_opacity_keyframe(0.0, 0)
    _source_layer.set_opacity_keyframe(2.0, 100)
    _source_layer.set_opacity_keyframe(4.0, 50)

    # Get opacity property
    src_prop = _source_layer.get_property("ADBE Opacity")
    assert_equal(src_prop.num_keyframes, 3, "Source should have 3 opacity keyframes")

    # Copy to target
    for i in range(src_prop.num_keyframes):
        time = src_prop.keyframe_time(i)
        value = src_prop.get_value_at_time(time)
        _target_layer.set_opacity_keyframe(time, value)

    # Verify
    tgt_prop = _target_layer.get_property("ADBE Opacity")
    assert_equal(tgt_prop.num_keyframes, 3, "Target should have 3 opacity keyframes")

    for i in range(3):
        src_val = src_prop.get_value_at_time(src_prop.keyframe_time(i))
        tgt_val = tgt_prop.get_value_at_time(tgt_prop.keyframe_time(i))
        assert_true(abs(src_val - tgt_val) < 0.1, "Opacity values should match")


# =============================================================
# Complex Animation Roundtrip
# =============================================================

@suite.test
def test_multiple_property_roundtrip():
    """Test copying multiple animated properties at once"""
    # Animate source layer with multiple properties
    _source_layer.set_position_keyframe(0.0, [100, 100])
    _source_layer.set_position_keyframe(5.0, [800, 600])

    _source_layer.set_scale_keyframe(0.0, [50, 50])
    _source_layer.set_scale_keyframe(5.0, [200, 200])

    _source_layer.set_rotation_keyframe(0.0, 0)
    _source_layer.set_rotation_keyframe(5.0, 720)

    # Copy all animations to target
    properties = ["ADBE Position", "ADBE Scale", "ADBE Rotate Z"]

    for prop_name in properties:
        src_prop = _source_layer.get_property(prop_name)
        if src_prop and src_prop.num_keyframes > 0:
            for i in range(src_prop.num_keyframes):
                time = src_prop.keyframe_time(i)
                value = src_prop.get_value_at_time(time)

                # Set keyframe on target based on property type
                if prop_name == "ADBE Position":
                    _target_layer.set_position_keyframe(time, value[:2])
                elif prop_name == "ADBE Scale":
                    _target_layer.set_scale_keyframe(time, value[:2])
                elif prop_name == "ADBE Rotate Z":
                    _target_layer.set_rotation_keyframe(time, value)

    # Verify all properties have matching keyframes
    for prop_name in properties:
        src_prop = _source_layer.get_property(prop_name)
        tgt_prop = _target_layer.get_property(prop_name)

        assert_equal(
            tgt_prop.num_keyframes,
            src_prop.num_keyframes,
            f"{prop_name} keyframe count should match"
        )


@suite.test
def test_temporal_interpolation_preservation():
    """Test that temporal interpolation is preserved in roundtrip"""
    # Set position keyframes with different interpolation
    _source_layer.set_position_keyframe(0.0, [100, 100])
    _source_layer.set_position_keyframe(3.0, [500, 500])
    _source_layer.set_position_keyframe(6.0, [900, 100])

    src_prop = _source_layer.get_property("ADBE Position")

    # Set bezier interpolation on middle keyframe
    src_prop.set_temporal_ease(1, [0.5, 0.5], [0.5, 0.5])

    # Get interpolation info before copy
    src_in_interp = src_prop.keyframe_in_interpolation(1)
    src_out_interp = src_prop.keyframe_out_interpolation(1)

    # Copy keyframes to target
    for i in range(src_prop.num_keyframes):
        time = src_prop.keyframe_time(i)
        value = src_prop.get_value_at_time(time)
        _target_layer.set_position_keyframe(time, value[:2])

    tgt_prop = _target_layer.get_property("ADBE Position")

    # Copy interpolation settings
    tgt_prop.set_temporal_ease(1, [0.5, 0.5], [0.5, 0.5])

    # Verify interpolation matches
    tgt_in_interp = tgt_prop.keyframe_in_interpolation(1)
    tgt_out_interp = tgt_prop.keyframe_out_interpolation(1)

    assert_equal(src_in_interp, tgt_in_interp, "In interpolation should match")
    assert_equal(src_out_interp, tgt_out_interp, "Out interpolation should match")


@suite.test
def test_keyframe_value_sampling():
    """Test sampling animated values at arbitrary times"""
    # Clear any existing keyframes from previous tests
    src_prop = _source_layer.get_property("ADBE Position")
    while src_prop.num_keyframes > 0:
        src_prop.remove_keyframe(0)

    tgt_prop = _target_layer.get_property("ADBE Position")
    while tgt_prop.num_keyframes > 0:
        tgt_prop.remove_keyframe(0)

    # Create animation curve
    _source_layer.set_position_keyframe(0.0, [100, 100])
    _source_layer.set_position_keyframe(5.0, [900, 900])

    # Sample values at various times
    sample_times = [0.0, 1.0, 2.5, 3.7, 5.0]
    sampled_values = []

    for time in sample_times:
        value = src_prop.get_value_at_time(time)
        sampled_values.append(value)

    # Create target animation using sampled values
    for time, value in zip(sample_times, sampled_values):
        _target_layer.set_position_keyframe(time, value[:2])

    # Verify target has correct number of keyframes
    assert_equal(tgt_prop.num_keyframes, len(sample_times),
                 "Target should have keyframes at all sample points")

    # Verify values match at sample times
    for i, time in enumerate(sample_times):
        tgt_val = tgt_prop.get_value_at_time(time)
        src_val = sampled_values[i]
        assert_true(abs(tgt_val[0] - src_val[0]) < 0.1, "Sampled X should match")
        assert_true(abs(tgt_val[1] - src_val[1]) < 0.1, "Sampled Y should match")


@suite.test
def test_animation_clear_and_recreate():
    """Test clearing animation and recreating it"""
    # Create initial animation
    _source_layer.set_position_keyframe(0.0, [100, 100])
    _source_layer.set_position_keyframe(3.0, [500, 500])
    _source_layer.set_position_keyframe(6.0, [900, 100])

    src_prop = _source_layer.get_property("ADBE Position")

    # Store keyframe data
    keyframe_data = []
    for i in range(src_prop.num_keyframes):
        time = src_prop.keyframe_time(i)
        value = src_prop.get_value_at_time(time)
        keyframe_data.append((time, value))

    # Clear all keyframes from source
    while src_prop.num_keyframes > 0:
        src_prop.remove_keyframe(0)

    assert_equal(src_prop.num_keyframes, 0, "All keyframes should be removed")

    # Recreate animation from stored data
    for time, value in keyframe_data:
        _source_layer.set_position_keyframe(time, value[:2])

    # Verify recreation
    assert_equal(src_prop.num_keyframes, len(keyframe_data),
                 "Recreated animation should have same keyframe count")

    for i, (orig_time, orig_value) in enumerate(keyframe_data):
        new_time = src_prop.keyframe_time(i)
        new_value = src_prop.get_value_at_time(new_time)

        assert_true(abs(orig_time - new_time) < 0.01, "Recreated time should match")
        assert_true(abs(orig_value[0] - new_value[0]) < 0.1, "Recreated X should match")
        assert_true(abs(orig_value[1] - new_value[1]) < 0.1, "Recreated Y should match")


def run():
    """Run the test suite"""
    return suite.run()


if __name__ == "__main__":
    run()
