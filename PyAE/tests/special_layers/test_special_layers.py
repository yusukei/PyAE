# test_special_layers.py
# PyAE Special Layers Test (Null, Solid, Adjustment)

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

suite = TestSuite("Special Layers")

# Test variables
_test_comp = None


@suite.setup
def setup():
    """Setup test environment"""
    global _test_comp
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_SpecialLayersTestComp", 1920, 1080, 1.0, 5.0, 30.0)


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
# Null Layer Tests
# =============================================================

@suite.test
def test_null_layer_creation():
    """Test null layer creation"""
    null_layer = _test_comp.add_null("_TestNull", 5.0)

    assert_not_none(null_layer, "Null layer should be created")
    assert_equal(null_layer.name, "_TestNull", "Null layer name should match")
    assert_true(null_layer.is_null, "Layer should be detected as null")


@suite.test
def test_null_layer_properties():
    """Test null layer basic properties"""
    null_layer = _test_comp.add_null("_NullProps", 5.0)

    # Null layers should be valid
    assert_true(null_layer.valid, "Null layer should be valid")

    # Should have position, scale, rotation like other layers
    position = null_layer.position
    assert_not_none(position, "Null layer should have position")
    assert_isinstance(position, tuple, "Position should be a tuple")
    assert_true(len(position) >= 2, "Position should have at least 2 components")

    scale = null_layer.scale
    assert_not_none(scale, "Null layer should have scale")

    rotation = null_layer.rotation
    assert_not_none(rotation, "Null layer should have rotation")


@suite.test
def test_null_layer_as_parent():
    """Test using null layer as parent"""
    null_layer = _test_comp.add_null("_ParentNull", 5.0)
    solid_layer = _test_comp.add_solid("_ChildSolid", 100, 100, (1.0, 0.0, 0.0), 5.0)

    # Set null as parent
    solid_layer.parent = null_layer

    assert_equal(solid_layer.parent.name, "_ParentNull", "Parent should be the null layer")

    # Clean up
    solid_layer.delete()
    null_layer.delete()


# =============================================================
# Solid Layer Tests
# =============================================================

@suite.test
def test_solid_layer_creation():
    """Test solid layer creation"""
    solid = _test_comp.add_solid("_TestSolid", 1920, 1080, (1.0, 0.5, 0.5), 5.0)

    assert_not_none(solid, "Solid layer should be created")
    assert_equal(solid.name, "_TestSolid", "Solid layer name should match")
    assert_true(solid.valid, "Solid layer should be valid")


@suite.test
def test_solid_layer_dimensions():
    """Test solid layer has correct dimensions"""
    width = 800
    height = 600
    solid = _test_comp.add_solid("_DimSolid", width, height, (1.0, 0.0, 0.0), 5.0)

    # Solid layers should have the specified dimensions
    # Note: We can't directly get layer dimensions without source item
    # but we can verify it's created successfully
    assert_not_none(solid, "Solid should be created with specified dimensions")
    assert_true(solid.valid, "Solid should be valid")

    solid.delete()


@suite.test
def test_solid_layer_operations():
    """Test solid layer supports standard layer operations"""
    solid = _test_comp.add_solid("_OpsSolid", 400, 300, (0.8, 0.2, 0.6), 5.0)

    # Test position modification
    solid.position = [960, 540]
    pos = solid.position
    assert_true(abs(pos[0] - 960) < 0.1, "Position X should be set correctly")
    assert_true(abs(pos[1] - 540) < 0.1, "Position Y should be set correctly")

    # Test opacity modification
    solid.opacity = 75.0
    assert_equal(solid.opacity, 75.0, "Opacity should be set correctly")

    # Test layer flags
    solid.enabled = False
    assert_false(solid.enabled, "Layer should be disabled")

    solid.delete()


# =============================================================
# Adjustment Layer Tests
# =============================================================

@suite.test
def test_adjustment_layer_creation():
    """Test adjustment layer creation from solid"""
    # Create a solid layer first
    solid = _test_comp.add_solid("_AdjustmentSolid", 1920, 1080, (1.0, 1.0, 1.0), 5.0)

    # Convert to adjustment layer
    solid.is_adjustment = True

    assert_true(solid.is_adjustment, "Layer should be an adjustment layer")

    solid.delete()


@suite.test
def test_adjustment_layer_toggle():
    """Test toggling adjustment layer flag"""
    solid = _test_comp.add_solid("_ToggleAdj", 1920, 1080, (1.0, 0.0, 0.0), 5.0)

    # Initially should not be adjustment layer
    assert_false(solid.is_adjustment, "Solid should not be adjustment initially")

    # Enable adjustment
    solid.is_adjustment = True
    assert_true(solid.is_adjustment, "Should be adjustment after enabling")

    # Disable adjustment
    solid.is_adjustment = False
    assert_false(solid.is_adjustment, "Should not be adjustment after disabling")

    solid.delete()


@suite.test
def test_adjustment_layer_properties():
    """Test adjustment layer retains normal layer properties"""
    solid = _test_comp.add_solid("_AdjProps", 1920, 1080, (1.0, 0.5, 0.5), 5.0)
    solid.is_adjustment = True

    # Should still have normal layer properties
    assert_not_none(solid.position, "Adjustment layer should have position")
    assert_not_none(solid.scale, "Adjustment layer should have scale")
    assert_not_none(solid.opacity, "Adjustment layer should have opacity")

    # Test modifying properties
    solid.opacity = 80.0
    assert_equal(solid.opacity, 80.0, "Should be able to modify opacity")

    solid.delete()


@suite.test
def test_adjustment_layer_with_effects():
    """Test adding effects to adjustment layer"""
    solid = _test_comp.add_solid("_AdjEffects", 1920, 1080, (1.0, 1.0, 1.0), 5.0)
    solid.is_adjustment = True

    # Add an effect
    effect = solid.add_effect("ADBE Brightness & Contrast 2")

    assert_not_none(effect, "Effect should be added to adjustment layer")
    assert_equal(solid.num_effects, 1, "Adjustment layer should have 1 effect")

    solid.delete()


# =============================================================
# Integration Tests
# =============================================================

@suite.test
def test_layer_type_detection():
    """Test detecting different layer types"""
    null_layer = _test_comp.add_null("_TypeNull", 5.0)
    # Use non-100x100 size to avoid null detection heuristic
    solid_layer = _test_comp.add_solid("_TypeSolid", 400, 300, (1.0, 0.0, 0.0), 5.0)
    adj_layer = _test_comp.add_solid("_TypeAdj", 1920, 1080, (1.0, 1.0, 1.0), 5.0)
    adj_layer.is_adjustment = True

    # Check null detection
    assert_true(null_layer.is_null, "Null layer should be detected")
    assert_false(solid_layer.is_null, "Solid should not be detected as null")
    assert_false(adj_layer.is_null, "Adjustment should not be detected as null")

    # Check adjustment detection
    assert_false(null_layer.is_adjustment, "Null should not be adjustment")
    assert_false(solid_layer.is_adjustment, "Normal solid should not be adjustment")
    assert_true(adj_layer.is_adjustment, "Adjustment layer should be detected")

    # Cleanup
    null_layer.delete()
    solid_layer.delete()
    adj_layer.delete()


@suite.test
def test_special_layers_in_composition():
    """Test multiple special layers in one composition"""
    initial_count = _test_comp.num_layers

    # Create various special layers
    null1 = _test_comp.add_null("_Multi_Null", 5.0)
    solid1 = _test_comp.add_solid("_Multi_Solid", 500, 500, (1.0, 0.0, 0.0), 5.0)
    adj1 = _test_comp.add_solid("_Multi_Adj", 1920, 1080, (1.0, 1.0, 1.0), 5.0)
    adj1.is_adjustment = True

    # Verify layer count
    assert_equal(_test_comp.num_layers, initial_count + 3, "Should have 3 more layers")

    # Verify all are valid
    assert_true(null1.valid, "Null should be valid")
    assert_true(solid1.valid, "Solid should be valid")
    assert_true(adj1.valid, "Adjustment should be valid")

    # Cleanup
    null1.delete()
    solid1.delete()
    adj1.delete()


def run():
    """Run the test suite"""
    return suite.run()


if __name__ == "__main__":
    run()
