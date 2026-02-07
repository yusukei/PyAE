# test_serialization.py
# PyAE Serialization API Test
# Tests to_dict() / from_dict() / update_from_dict() API

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

suite = TestSuite("Serialization API")

# Test variables
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    """Setup test environment"""
    global _test_comp, _test_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_SerializeTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    _test_layer = _test_comp.add_solid("_TestSolid", 100, 100, (1.0, 0.0, 0.0), 10.0)


@suite.teardown
def teardown():
    """Cleanup test environment"""
    global _test_comp
    try:
        if _test_comp is not None:
            # Delete the comp item
            items = ae.Project.get_current().items
            for item in items:
                if item.name == "_SerializeTestComp":
                    try:
                        item.delete()
                    except:
                        pass
                    break
    except:
        pass


# =============================================================
# Comp Serialization Tests
# =============================================================

@suite.test
def test_comp_to_dict_basic():
    """Test Comp.to_dict() returns valid dictionary"""
    data = _test_comp.to_dict()

    assert_not_none(data, "to_dict() should return a dictionary")
    assert_isinstance(data, dict, "Result should be a dict")

    # Check required keys
    assert_true("width" in data, "Should have width")
    assert_true("height" in data, "Should have height")
    assert_true("duration" in data, "Should have duration")
    assert_true("frame_rate" in data, "Should have frame_rate")


@suite.test
def test_comp_to_dict_values():
    """Test Comp.to_dict() returns correct values"""
    data = _test_comp.to_dict()

    assert_equal(data.get("width"), 1920, "Width should be 1920")
    assert_equal(data.get("height"), 1080, "Height should be 1080")
    assert_true(abs(data.get("duration", 0) - 10.0) < 0.01, "Duration should be 10.0")
    assert_true(abs(data.get("frame_rate", 0) - 30.0) < 0.01, "Frame rate should be 30.0")


@suite.test
def test_comp_to_dict_includes_layers():
    """Test Comp.to_dict() includes layers"""
    data = _test_comp.to_dict()

    assert_true("layers" in data, "Should have layers key")
    layers = data.get("layers", [])
    assert_true(len(layers) > 0, "Should have at least one layer")


@suite.test
def test_comp_from_dict_creates_comp():
    """Test Comp.from_dict() creates new composition"""
    # Import the serialize module directly
    import ae_serialize

    # Create a simple comp data dictionary
    comp_data = {
        "name": "_FromDictTestComp",
        "width": 1280,
        "height": 720,
        "pixel_aspect": 1.0,
        "duration": 5.0,
        "frame_rate": 24.0,
        "layers": []
    }

    # Call Python function directly instead of C++ binding
    new_comp = ae_serialize.comp_from_dict(comp_data)

    assert_not_none(new_comp, "from_dict() should create a Comp")
    assert_true(new_comp.valid, "Created comp should be valid")
    assert_equal(new_comp.width, 1280, "Width should match")
    assert_equal(new_comp.height, 720, "Height should match")

    # Cleanup
    try:
        items = ae.Project.get_current().items
        for item in items:
            if item.name == "_FromDictTestComp":
                item.delete()
                break
    except:
        pass


@suite.test
def test_comp_roundtrip():
    """Test Comp roundtrip: to_dict -> from_dict"""
    # Import the serialize module directly
    import ae_serialize

    # Add some animation to make it more interesting
    _test_layer.set_position_keyframe(0.0, [100, 100])
    _test_layer.set_position_keyframe(5.0, [500, 500])

    # Export
    original_data = _test_comp.to_dict()

    # Import - use Python function directly
    new_comp = ae_serialize.comp_from_dict(original_data)

    # Verify
    assert_not_none(new_comp, "Roundtrip should create a Comp")
    assert_equal(new_comp.width, _test_comp.width, "Width should match after roundtrip")
    assert_equal(new_comp.height, _test_comp.height, "Height should match after roundtrip")
    assert_true(
        abs(new_comp.frame_rate - _test_comp.frame_rate) < 0.01,
        "Frame rate should match after roundtrip"
    )

    # Cleanup
    try:
        items = ae.Project.get_current().items
        for item in items:
            if item.name == original_data.get("name") and item != _test_comp:
                item.delete()
                break
    except:
        pass


@suite.test
def test_comp_update_from_dict():
    """Test Comp.update_from_dict() updates existing comp"""
    # Create another comp to update
    proj = ae.Project.get_current()
    update_comp = proj.create_comp("_UpdateTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    update_layer = update_comp.add_solid("_UpdateSolid", 100, 100, (0.0, 1.0, 0.0), 10.0)

    # Get data from test comp
    source_data = _test_comp.to_dict()

    # Update the other comp
    update_comp.update_from_dict(source_data)

    # The comp should still exist
    assert_true(update_comp.valid, "Updated comp should still be valid")

    # Cleanup
    try:
        items = ae.Project.get_current().items
        for item in items:
            if item.name == "_UpdateTestComp":
                item.delete()
                break
    except:
        pass


# =============================================================
# Layer Serialization Tests
# =============================================================

@suite.test
def test_layer_to_dict_basic():
    """Test Layer.to_dict() returns valid dictionary"""
    data = _test_layer.to_dict()

    assert_not_none(data, "to_dict() should return a dictionary")
    assert_isinstance(data, dict, "Result should be a dict")

    # Check required keys
    assert_true("name" in data, "Should have name")
    assert_true("type" in data, "Should have type")


@suite.test
def test_layer_to_dict_values():
    """Test Layer.to_dict() returns correct values"""
    data = _test_layer.to_dict()

    assert_equal(data.get("name"), "_TestSolid", "Name should match")
    # Check in_point and out_point exist
    assert_true("in_point" in data, "Should have in_point")
    assert_true("out_point" in data, "Should have out_point")


@suite.test
def test_layer_to_dict_includes_properties():
    """Test Layer.to_dict() includes properties"""
    data = _test_layer.to_dict()

    assert_true("properties" in data, "Should have properties key")
    properties = data.get("properties", {})
    assert_isinstance(properties, dict, "Properties should be a dict")


@suite.test
def test_layer_update_from_dict():
    """Test Layer.update_from_dict() updates existing layer"""
    # Create source data with position
    source_data = {
        "name": "_UpdatedName",
        "active": True,
    }

    _test_layer.update_from_dict(source_data)

    # Layer name should be updated
    assert_equal(_test_layer.name, "_UpdatedName", "Layer name should be updated")

    # Restore original name
    _test_layer.name = "_TestSolid"


@suite.test
def test_layer_to_dict_includes_time_remapping():
    """Test Layer.to_dict() includes time_remapping_enabled flag"""
    global _test_comp

    # Time Remapping requires a footage or nested comp layer (not solid)
    proj = ae.Project.get_current()
    inner_comp = proj.create_comp("_TimeRemapSerializeInner", 100, 100, 1.0, 5.0, 30.0)
    nested_layer = _test_comp.add_layer(inner_comp)

    # Serialize
    data = nested_layer.to_dict()

    # Check time_remapping_enabled is in serialized data
    # Note: TIME_REMAPPING flag is read-only via SDK, so we only verify it's exported
    assert_true("time_remapping_enabled" in data, "Should have time_remapping_enabled")
    assert_isinstance(data.get("time_remapping_enabled"), bool, "time_remapping_enabled should be bool")

    # Cleanup
    nested_layer.delete()
    inner_comp.delete()


@suite.test
def test_layer_time_remapping_serialization_format():
    """Test time_remapping_enabled is properly formatted in serialization"""
    global _test_layer

    # Serialize the solid layer
    data = _test_layer.to_dict()

    # Check time_remapping_enabled field exists and is a boolean
    # Note: TIME_REMAPPING flag setting via SDK is limited by After Effects
    # The flag is exported but may not be restored for all layer types
    assert_true("time_remapping_enabled" in data, "Should have time_remapping_enabled in export")
    assert_isinstance(data.get("time_remapping_enabled"), bool, "time_remapping_enabled should be bool")
    # Solid layers cannot have time remapping, so it should be False
    assert_false(data.get("time_remapping_enabled"), "Solid layer should have time_remapping_enabled=False")


# =============================================================
# Property Serialization Tests
# =============================================================

@suite.test
def test_property_to_dict_basic():
    """Test Property.to_dict() returns valid dictionary"""
    position_prop = _test_layer.get_property("ADBE Position")
    assert_not_none(position_prop, "Position property should exist")

    data = position_prop.to_dict()

    assert_not_none(data, "to_dict() should return a dictionary")
    assert_isinstance(data, dict, "Result should be a dict")

    # Check required keys
    assert_true("match_name" in data, "Should have match_name")
    assert_true("type" in data, "Should have type")


@suite.test
def test_property_to_dict_value():
    """Test Property.to_dict() includes value"""
    position_prop = _test_layer.get_property("ADBE Position")

    # Set a specific value
    position_prop.value = [500, 300]

    data = position_prop.to_dict()

    # Should have either value or bdata
    has_value = "bdata" in data or "_value" in data
    assert_true(has_value, "Should have value data (bdata or _value)")


@suite.test
def test_property_to_dict_with_keyframes():
    """Test Property.to_dict() includes keyframes"""
    position_prop = _test_layer.get_property("ADBE Position")

    # Clear existing keyframes
    while position_prop.num_keyframes > 0:
        position_prop.remove_keyframe(0)

    # Add keyframes
    position_prop.add_keyframe(0.0, [100, 100])
    position_prop.add_keyframe(5.0, [900, 500])

    data = position_prop.to_dict()

    # Should have keyframes
    assert_true("keyframes" in data, "Should have keyframes key")
    keyframes = data.get("keyframes", [])
    assert_true(len(keyframes) >= 2, "Should have at least 2 keyframes")


@suite.test
def test_property_update_from_dict():
    """Test Property.update_from_dict() updates property value"""
    opacity_prop = _test_layer.get_property("ADBE Opacity")
    assert_not_none(opacity_prop, "Opacity property should exist")

    # Store original
    original_value = opacity_prop.value

    # Update with new data
    update_data = {
        "match_name": "ADBE Opacity",
        "type": "property",
        "_value": 50.0
    }

    opacity_prop.update_from_dict(update_data)

    # Value should be updated
    assert_true(abs(opacity_prop.value - 50.0) < 1.0, "Opacity should be updated to ~50")

    # Restore original
    opacity_prop.value = original_value


# =============================================================
# Effect Serialization Tests
# =============================================================

@suite.test
def test_effect_to_dict_basic():
    """Test Effect.to_dict() returns valid dictionary"""
    # Add an effect
    effect = _test_layer.add_effect("ADBE Gaussian Blur 2")
    assert_not_none(effect, "Should be able to add Gaussian Blur effect")

    data = effect.to_dict()

    assert_not_none(data, "to_dict() should return a dictionary")
    assert_isinstance(data, dict, "Result should be a dict")

    # Check required keys
    assert_true("match_name" in data, "Should have match_name")
    assert_equal(data.get("match_name"), "ADBE Gaussian Blur 2", "Match name should match")


@suite.test
def test_effect_to_dict_includes_params():
    """Test Effect.to_dict() includes parameters"""
    # Get existing effect or add one
    if _test_layer.num_effects == 0:
        _test_layer.add_effect("ADBE Gaussian Blur 2")

    effect = _test_layer.effect(0)
    data = effect.to_dict()

    # Should have params
    assert_true("params" in data, "Should have params key")
    params = data.get("params", [])
    assert_true(len(params) > 0, "Should have at least one parameter")


@suite.test
def test_effect_update_from_dict():
    """Test Effect.update_from_dict() updates effect parameters"""
    # Ensure we have an effect
    if _test_layer.num_effects == 0:
        _test_layer.add_effect("ADBE Gaussian Blur 2")

    effect = _test_layer.effect(0)

    # Get current data and modify
    update_data = {
        "match_name": "ADBE Gaussian Blur 2",
        "enabled": True,
        "params": []
    }

    effect.update_from_dict(update_data)

    # Effect should still be valid
    assert_true(effect.valid, "Effect should still be valid after update")


# =============================================================
# Project Serialization Tests
# =============================================================

@suite.test
def test_project_to_dict_basic():
    """Test Project.to_dict() returns valid dictionary"""
    proj = ae.Project.get_current()
    data = proj.to_dict()

    assert_not_none(data, "to_dict() should return a dictionary")
    assert_isinstance(data, dict, "Result should be a dict")

    # Check required keys
    assert_true("version" in data, "Should have version")
    assert_true("items" in data, "Should have items")


@suite.test
def test_project_to_dict_includes_items():
    """Test Project.to_dict() includes all items"""
    proj = ae.Project.get_current()
    data = proj.to_dict()

    items = data.get("items", [])
    assert_true(len(items) > 0, "Should have at least one item")

    # Find our test comp
    found_test_comp = False
    for item in items:
        if item.get("name") == "_SerializeTestComp":
            found_test_comp = True
            assert_equal(item.get("type"), "Comp", "Test comp should be type Comp")
            break

    assert_true(found_test_comp, "Should include test comp in items")


@suite.test
def test_project_update_from_dict():
    """Test Project.update_from_dict() updates project"""
    proj = ae.Project.get_current()

    # Get current data
    data = proj.to_dict()

    # Update (should not fail)
    proj.update_from_dict(data)

    # Project should still be valid
    assert_true(proj.valid, "Project should still be valid after update")


# =============================================================
# Data Integrity Tests
# =============================================================

@suite.test
def test_serialization_preserves_animation():
    """Test that serialization preserves animation data"""
    # Add animation to test layer
    position_prop = _test_layer.get_property("ADBE Position")

    # Clear existing keyframes
    while position_prop.num_keyframes > 0:
        position_prop.remove_keyframe(0)

    # Add specific keyframes
    position_prop.add_keyframe(0.0, [100, 200])
    position_prop.add_keyframe(2.5, [500, 400])
    position_prop.add_keyframe(5.0, [900, 100])

    original_num_kf = position_prop.num_keyframes

    # Serialize
    data = _test_layer.to_dict()

    # Check animation is in serialized data
    properties = data.get("properties", {})
    transform = None

    # Find transform group
    for key, value in properties.items():
        if isinstance(value, dict):
            if "Transform" in key or value.get("match_name", "") == "ADBE Transform Group":
                transform = value
                break

    assert_not_none(transform, "Should have transform group in serialized data")

    # Find position in transform children
    children = transform.get("children", {})
    position_data = None
    for key, value in children.items():
        if isinstance(value, dict) and value.get("match_name") == "ADBE Position":
            position_data = value
            break

    assert_not_none(position_data, "Should have position in serialized data")

    # Check keyframes
    keyframes = position_data.get("keyframes", [])
    assert_equal(len(keyframes), original_num_kf, "Serialized data should have correct number of keyframes")


@suite.test
def test_bdata_format_present():
    """Test that bdata format is used for values"""
    position_prop = _test_layer.get_property("ADBE Position")
    position_prop.value = [960, 540]

    data = position_prop.to_dict()

    # bdata should be present
    assert_true("bdata" in data, "Should have bdata field")
    bdata = data.get("bdata", "")
    assert_true(len(bdata) > 0, "bdata should not be empty")

    # bdata should be hex string
    assert_isinstance(bdata, str, "bdata should be a string")


def run():
    """Run the test suite"""
    return suite.run()


if __name__ == "__main__":
    run()
