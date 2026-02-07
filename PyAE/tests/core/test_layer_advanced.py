# test_layer_advanced.py
# PyAE Layer Advanced API Tests (Phase 3 - MEDIUM PRIORITY)
# Tests for AEGP_LayerSuite9 advanced functions

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
        assert_close,
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
        assert_close,
    )

suite = TestSuite("Layer Advanced")

# Test variables
_test_comp = None
_test_layer = None
_matte_layer = None
_source_layer = None


@suite.setup
def setup():
    """Test setup"""
    global _test_comp, _test_layer, _matte_layer, _source_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_LayerAdvancedTest", 1920, 1080, 1.0, 10.0, 30.0)
    # Create test layers
    _test_layer = _test_comp.add_solid("_TestLayer", 400, 400, (1.0, 0.0, 0.0), 10.0)
    _matte_layer = _test_comp.add_solid("_MatteLayer", 400, 400, (0.0, 1.0, 0.0), 10.0)
    _source_layer = _test_comp.add_solid("_SourceLayer", 400, 400, (0.0, 0.0, 1.0), 10.0)


@suite.teardown
def teardown():
    """Test cleanup"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except:
        pass


# ===== Layer Information Functions =====


@suite.test
def test_layer_source_item_id():
    """Get layer source item ID"""
    global _test_layer
    source_id = _test_layer.source_item_id
    assert_isinstance(source_id, int, "Source item ID should be an integer")
    # Note: Solid layers may have source_item_id of 0, so we just check it's non-negative
    assert_true(source_id >= 0, "Source item ID should be non-negative")


@suite.test
def test_layer_current_time():
    """Get layer current time"""
    global _test_layer
    current_time = _test_layer.current_time
    assert_isinstance(current_time, (int, float), "Current time should be numeric")
    assert_true(current_time >= 0.0, "Current time should be non-negative")


@suite.test
def test_layer_object_type():
    """Get layer object type"""
    global _test_layer
    obj_type = _test_layer.object_type
    assert_isinstance(obj_type, int, "Object type should be an integer")


@suite.test
def test_layer_is_2d():
    """Check if layer is 2D"""
    global _test_layer
    is_2d = _test_layer.is_2d
    assert_isinstance(is_2d, bool, "is_2d should be boolean")
    # Newly created solid should be 2D by default
    assert_true(is_2d, "New solid layer should be 2D by default")


# ===== Transform Functions =====


@suite.test
def test_layer_to_world_xform_at_zero():
    """Get transform matrix at time 0"""
    global _test_layer
    matrix = _test_layer.to_world_xform(0.0)
    assert_not_none(matrix, "Transform matrix should not be None")
    assert_isinstance(matrix, list, "Transform matrix should be a list")
    assert_equal(4, len(matrix), "Matrix should be 4x4")
    # Check first row
    assert_isinstance(matrix[0], list, "Matrix row should be a list")
    assert_equal(4, len(matrix[0]), "Matrix row should have 4 elements")


@suite.test
def test_layer_to_world_xform_at_different_times():
    """Get transform matrix at different times"""
    global _test_layer
    # Set position keyframes
    _test_layer.position = (960.0, 540.0)

    matrix1 = _test_layer.to_world_xform(0.0)
    matrix2 = _test_layer.to_world_xform(1.0)
    matrix3 = _test_layer.to_world_xform(5.0)

    assert_not_none(matrix1, "Matrix at t=0 should not be None")
    assert_not_none(matrix2, "Matrix at t=1 should not be None")
    assert_not_none(matrix3, "Matrix at t=5 should not be None")


@suite.test
def test_layer_to_world_xform_with_rotation():
    """Get transform matrix with rotation applied"""
    global _test_layer
    # Apply rotation
    _test_layer.rotation = 45.0

    matrix = _test_layer.to_world_xform(0.0)
    assert_not_none(matrix, "Transform matrix with rotation should not be None")
    assert_equal(4, len(matrix), "Matrix should be 4x4")


@suite.test
def test_layer_to_world_xform_with_scale():
    """Get transform matrix with scale applied"""
    global _test_layer
    # Apply scale
    _test_layer.scale = (150.0, 150.0)

    matrix = _test_layer.to_world_xform(0.0)
    assert_not_none(matrix, "Transform matrix with scale should not be None")
    assert_equal(4, len(matrix), "Matrix should be 4x4")


# ===== Track Matte Functions =====


@suite.test
def test_layer_has_track_matte_default():
    """Check if layer has track matte (default: no)"""
    global _test_layer
    has_matte = _test_layer.has_track_matte
    assert_isinstance(has_matte, bool, "has_track_matte should be boolean")
    # New layer should not have track matte
    assert_false(has_matte, "New layer should not have track matte by default")


@suite.test
def test_layer_is_used_as_track_matte_default():
    """Check if layer is used as track matte (default: no)"""
    global _test_layer
    is_matte = _test_layer.is_used_as_track_matte
    assert_isinstance(is_matte, bool, "is_used_as_track_matte should be boolean")
    # New layer should not be used as track matte
    assert_false(is_matte, "New layer should not be used as track matte by default")


@suite.test
def test_layer_get_track_matte_layer_none():
    """Get track matte layer (default: None)"""
    global _test_layer
    matte_layer = _test_layer.track_matte_layer
    assert_none(matte_layer, "New layer should have no track matte layer")


@suite.test
def test_layer_set_track_matte_alpha():
    """Set track matte with alpha mode"""
    global _test_layer, _matte_layer
    # Set alpha matte
    _test_layer.set_track_matte(_matte_layer, 1)  # TRACK_MATTE_ALPHA

    # Verify
    assert_true(_test_layer.has_track_matte, "Layer should have track matte after setting")
    assert_true(_matte_layer.is_used_as_track_matte, "Matte layer should be used as track matte")

    # Get matte layer
    matte = _test_layer.track_matte_layer
    assert_not_none(matte, "Should get track matte layer")


@suite.test
def test_layer_set_track_matte_luma():
    """Set track matte with luma mode"""
    global _source_layer, _matte_layer
    # Set luma matte
    _source_layer.set_track_matte(_matte_layer, 3)  # TRACK_MATTE_LUMA

    # Verify
    assert_true(_source_layer.has_track_matte, "Layer should have track matte")
    assert_true(_matte_layer.is_used_as_track_matte, "Matte layer should be used as track matte")


@suite.test
def test_layer_remove_track_matte():
    """Remove track matte"""
    global _test_layer, _matte_layer
    # Set matte first
    _test_layer.set_track_matte(_matte_layer, 1)  # TRACK_MATTE_ALPHA
    assert_true(_test_layer.has_track_matte, "Layer should have track matte")

    # Remove matte
    _test_layer.remove_track_matte()

    # Verify
    assert_false(_test_layer.has_track_matte, "Layer should not have track matte after removal")
    matte = _test_layer.track_matte_layer
    assert_none(matte, "Track matte layer should be None after removal")


@suite.test
def test_layer_track_matte_roundtrip():
    """Set and remove track matte multiple times"""
    global _test_layer, _matte_layer

    # First set
    _test_layer.set_track_matte(_matte_layer, 1)
    assert_true(_test_layer.has_track_matte, "Should have matte after first set")

    # Remove
    _test_layer.remove_track_matte()
    assert_false(_test_layer.has_track_matte, "Should not have matte after first remove")

    # Second set
    _test_layer.set_track_matte(_matte_layer, 2)  # TRACK_MATTE_NOT_ALPHA
    assert_true(_test_layer.has_track_matte, "Should have matte after second set")

    # Remove again
    _test_layer.remove_track_matte()
    assert_false(_test_layer.has_track_matte, "Should not have matte after second remove")


# ===== Integration Tests =====


@suite.test
def test_layer_advanced_info_integration():
    """Integration test for layer information properties"""
    global _test_layer

    # Get all info properties
    source_id = _test_layer.source_item_id
    current_time = _test_layer.current_time
    obj_type = _test_layer.object_type
    is_2d = _test_layer.is_2d

    # All should be valid
    assert_isinstance(source_id, int, "source_item_id should be int")
    assert_isinstance(current_time, (int, float), "current_time should be numeric")
    assert_isinstance(obj_type, int, "object_type should be int")
    assert_isinstance(is_2d, bool, "is_2d should be bool")


@suite.test
def test_layer_transform_and_matte_integration():
    """Integration test combining transform and track matte"""
    global _test_layer, _matte_layer

    # Set transform properties
    _test_layer.position = (960.0, 540.0)
    _test_layer.rotation = 45.0
    _test_layer.scale = (200.0, 200.0)

    # Get transform matrix
    matrix = _test_layer.to_world_xform(0.0)
    assert_not_none(matrix, "Should get transform matrix")

    # Set track matte
    _test_layer.set_track_matte(_matte_layer, 1)
    assert_true(_test_layer.has_track_matte, "Should have track matte")

    # Transform matrix should still work
    matrix2 = _test_layer.to_world_xform(0.0)
    assert_not_none(matrix2, "Transform matrix should work with track matte")

    # Remove track matte
    _test_layer.remove_track_matte()
    assert_false(_test_layer.has_track_matte, "Should not have track matte after removal")


def run():
    """Run the test suite"""
    return suite.run()


if __name__ == "__main__":
    run()
