# test_expression_links.py
# T502: Expression Links Test
# Tests for property links, layer links, comp references, time-based expressions

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
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
        assert_close,
    )

suite = TestSuite("Expression Links (T502)")

# Test variables
_test_comp = None
_source_layer = None
_target_layer = None


@suite.setup
def setup():
    """Create test composition and layers"""
    global _test_comp, _source_layer, _target_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_ExpressionLinksTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    _source_layer = _test_comp.add_solid("_SourceLayer", 200, 200, (1.0, 0.5, 0.0), 10.0)
    _target_layer = _test_comp.add_solid("_TargetLayer", 100, 100, (0.0, 0.5, 1.0), 10.0)


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
def test_expression_property_link():
    """Link target layer property to source layer property via expression"""
    global _source_layer, _target_layer

    # Get source and target Opacity properties
    source_opacity = _source_layer.property("Opacity")
    target_opacity = _target_layer.property("Opacity")

    assert_not_none(source_opacity, "Source opacity should exist")
    assert_not_none(target_opacity, "Target opacity should exist")

    # Set source opacity value
    source_opacity.value = 75.0

    # Link target to source using expression
    # thisComp.layer("_SourceLayer").opacity
    expr = 'thisComp.layer("_SourceLayer").transform.opacity'
    target_opacity.expression = expr

    # Verify expression is set
    assert_true(target_opacity.has_expression, "Target should have expression")
    assert_equal(expr, target_opacity.expression, "Expression text should match")

    # The target's value should now be linked to source
    # Note: Expression evaluation happens in AE, so we verify the expression is set
    target_val = target_opacity.value
    assert_close(75.0, target_val, tolerance=1.0,
                 message=f"Target opacity should match source (75.0), got {target_val}")

    # Cleanup
    target_opacity.expression = ""


@suite.test
def test_expression_layer_link():
    """Link target layer position to source layer position"""
    global _source_layer, _target_layer

    # Get position properties
    source_pos = _source_layer.property("Position")
    target_pos = _target_layer.property("Position")

    assert_not_none(source_pos, "Source position should exist")
    assert_not_none(target_pos, "Target position should exist")

    # Set source position
    source_pos.value = [500, 300]

    # Link target position to source position with offset
    expr = 'thisComp.layer("_SourceLayer").transform.position + [100, 50]'
    target_pos.expression = expr

    # Verify expression is set
    assert_true(target_pos.has_expression, "Target should have expression")

    # Evaluate: source [500, 300] + offset [100, 50] = [600, 350]
    target_val = target_pos.value
    assert_close(600.0, target_val[0], tolerance=1.0,
                 message=f"Target X should be 600, got {target_val[0]}")
    assert_close(350.0, target_val[1], tolerance=1.0,
                 message=f"Target Y should be 350, got {target_val[1]}")

    # Cleanup
    target_pos.expression = ""


@suite.test
def test_expression_comp_link():
    """Expression referencing composition properties"""
    global _test_comp, _target_layer

    # Get target position
    target_pos = _target_layer.property("Position")
    assert_not_none(target_pos, "Target position should exist")

    # Set expression to center of composition
    expr = "[thisComp.width/2, thisComp.height/2]"
    target_pos.expression = expr

    # Verify expression is set
    assert_true(target_pos.has_expression, "Target should have expression")

    # Composition is 1920x1080, so center is [960, 540]
    target_val = target_pos.value
    assert_close(960.0, target_val[0], tolerance=1.0,
                 message=f"Target X should be 960 (comp center), got {target_val[0]}")
    assert_close(540.0, target_val[1], tolerance=1.0,
                 message=f"Target Y should be 540 (comp center), got {target_val[1]}")

    # Cleanup
    target_pos.expression = ""


@suite.test
def test_expression_time_link():
    """Time-based expression evaluation"""
    global _target_layer

    # Get rotation property
    rotation = _target_layer.property("Rotation")
    assert_not_none(rotation, "Rotation should exist")

    # Set time-based expression: rotation = time * 90 degrees
    expr = "time * 90"
    rotation.expression = expr

    # Verify expression is set
    assert_true(rotation.has_expression, "Should have expression")
    assert_equal(expr, rotation.expression, "Expression text should match")

    # Test value at different times
    # At time=0, rotation should be 0
    val_at_0 = rotation.get_value_at_time(0.0)
    assert_close(0.0, val_at_0, tolerance=0.1,
                 message=f"Rotation at time 0 should be 0, got {val_at_0}")

    # At time=1, rotation should be 90
    val_at_1 = rotation.get_value_at_time(1.0)
    assert_close(90.0, val_at_1, tolerance=0.1,
                 message=f"Rotation at time 1 should be 90, got {val_at_1}")

    # At time=2, rotation should be 180
    val_at_2 = rotation.get_value_at_time(2.0)
    assert_close(180.0, val_at_2, tolerance=0.1,
                 message=f"Rotation at time 2 should be 180, got {val_at_2}")

    # Cleanup
    rotation.expression = ""


@suite.test
def test_expression_error_handling():
    """Expression error handling"""
    global _target_layer

    # Get opacity property
    opacity = _target_layer.property("Opacity")
    assert_not_none(opacity, "Opacity should exist")

    # Set invalid expression
    invalid_expr = "undefinedVariable + 100"

    # Setting invalid expression may throw or set expression_error
    try:
        opacity.expression = invalid_expr

        # Check if expression has error
        # AE will evaluate the expression and report error
        has_expr = opacity.has_expression
        expr_error = opacity.expression_error

        # Either the expression should have an error, or has_expression is false
        # depending on AE's handling of invalid expressions
        if has_expr:
            # Expression was set but has error
            assert_true(len(expr_error) > 0,
                        "Invalid expression should have error message")
        else:
            # Expression was not enabled due to error
            pass

    except Exception as e:
        # Setting invalid expression raised an exception - acceptable behavior
        pass
    finally:
        # Cleanup - try to clear expression
        try:
            opacity.expression = ""
        except:
            pass


@suite.test
def test_expression_with_parenting():
    """Expression combined with layer parenting"""
    global _test_comp, _source_layer, _target_layer

    # Create parent layer
    parent_layer = _test_comp.add_solid("_ParentForExpr", 150, 150, (0.5, 0.5, 0.5), 10.0)

    # Set up parenting: target is child of parent
    _target_layer.parent = parent_layer

    # Set parent position
    parent_pos = parent_layer.property("Position")
    parent_pos.value = [300, 200]

    # Get target rotation and set expression to follow source rotation
    source_rot = _source_layer.property("Rotation")
    target_rot = _target_layer.property("Rotation")

    # Set source rotation
    source_rot.value = 45.0

    # Link target rotation to source via expression
    expr = 'thisComp.layer("_SourceLayer").transform.rotation'
    target_rot.expression = expr

    # Verify expression is set
    assert_true(target_rot.has_expression, "Target rotation should have expression")

    # Target rotation should match source despite parenting
    target_val = target_rot.value
    assert_close(45.0, target_val, tolerance=1.0,
                 message=f"Target rotation should be 45, got {target_val}")

    # Test that parenting still works for position
    assert_not_none(_target_layer.parent, "Target should have parent")
    assert_equal(parent_layer.index, _target_layer.parent.index,
                 "Parent should be the parent layer")

    # Cleanup
    target_rot.expression = ""
    _target_layer.parent = None
    parent_layer.delete()


@suite.test
def test_expression_scale_link():
    """Link scale property between layers"""
    global _source_layer, _target_layer

    # Get scale properties
    source_scale = _source_layer.property("Scale")
    target_scale = _target_layer.property("Scale")

    assert_not_none(source_scale, "Source scale should exist")
    assert_not_none(target_scale, "Target scale should exist")

    # Set source scale
    source_scale.value = [150, 150]

    # Link target scale to source scale at half size
    expr = 'thisComp.layer("_SourceLayer").transform.scale * 0.5'
    target_scale.expression = expr

    # Verify expression is set
    assert_true(target_scale.has_expression, "Target should have expression")

    # Target scale should be half of source: [75, 75]
    target_val = target_scale.value
    assert_close(75.0, target_val[0], tolerance=1.0,
                 message=f"Target scale X should be 75, got {target_val[0]}")
    assert_close(75.0, target_val[1], tolerance=1.0,
                 message=f"Target scale Y should be 75, got {target_val[1]}")

    # Cleanup
    target_scale.expression = ""


@suite.test
def test_expression_enable_disable():
    """Enable and disable expression"""
    global _target_layer

    # Get opacity property
    opacity = _target_layer.property("Opacity")
    assert_not_none(opacity, "Opacity should exist")

    # Set initial value
    opacity.value = 100.0

    # Set expression
    expr = "50"
    opacity.expression = expr

    # Verify expression is enabled
    assert_true(opacity.has_expression, "Expression should be enabled")

    # Value should be from expression (50)
    val_with_expr = opacity.value
    assert_close(50.0, val_with_expr, tolerance=1.0,
                 message=f"Value with expression should be 50, got {val_with_expr}")

    # Disable expression
    opacity.enable_expression(False)

    # Expression should be disabled
    assert_false(opacity.has_expression, "Expression should be disabled")

    # Value should return to original (or static value)
    # Note: When expression is disabled, AE uses the last evaluated value or static value

    # Re-enable expression
    opacity.enable_expression(True)
    assert_true(opacity.has_expression, "Expression should be re-enabled")

    # Cleanup
    opacity.expression = ""


@suite.test
def test_expression_anchor_point_link():
    """Link anchor point between layers"""
    global _source_layer, _target_layer

    # Get anchor point properties
    source_anchor = _source_layer.property("Anchor Point")
    target_anchor = _target_layer.property("Anchor Point")

    assert_not_none(source_anchor, "Source anchor should exist")
    assert_not_none(target_anchor, "Target anchor should exist")

    # Set source anchor point
    source_anchor.value = [50, 75]

    # Link target anchor to source anchor
    expr = 'thisComp.layer("_SourceLayer").transform.anchorPoint'
    target_anchor.expression = expr

    # Verify expression is set
    assert_true(target_anchor.has_expression, "Target should have expression")

    # Target anchor should match source
    target_val = target_anchor.value
    assert_close(50.0, target_val[0], tolerance=1.0,
                 message=f"Target anchor X should be 50, got {target_val[0]}")
    assert_close(75.0, target_val[1], tolerance=1.0,
                 message=f"Target anchor Y should be 75, got {target_val[1]}")

    # Cleanup
    target_anchor.expression = ""


@suite.test
def test_expression_math_operations():
    """Expression with mathematical operations"""
    global _target_layer

    # Get opacity property for simple value testing
    opacity = _target_layer.property("Opacity")
    assert_not_none(opacity, "Opacity should exist")

    # Expression with math: sin wave oscillating between 50 and 100
    # At time=0, sin(0)=0, so value = 75 + 0 = 75
    expr = "75 + Math.sin(time * Math.PI * 2) * 25"
    opacity.expression = expr

    # Verify expression is set
    assert_true(opacity.has_expression, "Should have expression")

    # At time=0, sin(0)=0, value should be 75
    val_at_0 = opacity.get_value_at_time(0.0)
    assert_close(75.0, val_at_0, tolerance=1.0,
                 message=f"Value at time 0 should be 75, got {val_at_0}")

    # At time=0.25 (quarter period), sin(PI/2)=1, value should be 100
    val_at_025 = opacity.get_value_at_time(0.25)
    assert_close(100.0, val_at_025, tolerance=1.0,
                 message=f"Value at time 0.25 should be 100, got {val_at_025}")

    # At time=0.75 (three quarter period), sin(3*PI/2)=-1, value should be 50
    val_at_075 = opacity.get_value_at_time(0.75)
    assert_close(50.0, val_at_075, tolerance=1.0,
                 message=f"Value at time 0.75 should be 50, got {val_at_075}")

    # Cleanup
    opacity.expression = ""


@suite.test
def test_expression_conditional():
    """Expression with conditional logic"""
    global _target_layer

    # Get opacity property
    opacity = _target_layer.property("Opacity")
    assert_not_none(opacity, "Opacity should exist")

    # Conditional expression: opacity is 100 in first half of comp, 50 in second half
    # Comp duration is 10 seconds
    expr = "time < 5 ? 100 : 50"
    opacity.expression = expr

    # Verify expression is set
    assert_true(opacity.has_expression, "Should have expression")

    # At time=2, should be 100
    val_at_2 = opacity.get_value_at_time(2.0)
    assert_close(100.0, val_at_2, tolerance=1.0,
                 message=f"Value at time 2 should be 100, got {val_at_2}")

    # At time=7, should be 50
    val_at_7 = opacity.get_value_at_time(7.0)
    assert_close(50.0, val_at_7, tolerance=1.0,
                 message=f"Value at time 7 should be 50, got {val_at_7}")

    # Cleanup
    opacity.expression = ""


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
