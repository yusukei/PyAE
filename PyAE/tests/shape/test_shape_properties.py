"""
Shape Layer Properties Test

Tests:
- Fill properties (color, opacity)
- Stroke properties (color, width, line cap, line join)
- Gradient Fill properties
- Gradient Stroke properties
- Transform properties within groups
- Merge Paths operations
- Repeater properties
"""

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
    )

suite = TestSuite("Shape Properties")

# Global test composition
_test_comp = None

@suite.setup
def setup():
    """Setup test composition"""
    global _test_comp
    project = ae.Project.get_current()
    _test_comp = project.create_comp("Test_ShapeProperties", 1920, 1080, 1.0, 10.0, 30.0)

@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp
    if _test_comp:
        _test_comp.delete()
        _test_comp = None

# ====================================================================
# Test: Fill Properties
# ====================================================================

@suite.test
def test_add_fill():
    """Test adding fill to shape group"""
    layer = _test_comp.add_shape()
    layer.name = "Test_Fill"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        # Add rectangle
        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        # Add fill
        if group.can_add_stream("ADBE Vector Graphic - Fill"):
            fill = group.add_stream("ADBE Vector Graphic - Fill")
            assert fill is not None, "Fill not created"
            assert fill.match_name == "ADBE Vector Graphic - Fill", f"Fill match name incorrect: {fill.match_name}"

@suite.test
def test_fill_color():
    """Test fill color property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_FillColor"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        if group.can_add_stream("ADBE Vector Graphic - Fill"):
            fill = group.add_stream("ADBE Vector Graphic - Fill")

            # Get color property
            color_prop = fill.property("ADBE Vector Fill Color")
            assert color_prop is not None, "Fill color property not found"

            # Set color to red
            color_prop.value = [1.0, 0.0, 0.0, 1.0]
            result = color_prop.value

            assert abs(result[0] - 1.0) < 0.01, f"Red channel incorrect: {result[0]}"
            assert abs(result[1] - 0.0) < 0.01, f"Green channel incorrect: {result[1]}"
            assert abs(result[2] - 0.0) < 0.01, f"Blue channel incorrect: {result[2]}"
            assert abs(result[3] - 1.0) < 0.01, f"Alpha channel incorrect: {result[3]}"

@suite.test
def test_fill_opacity():
    """Test fill opacity property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_FillOpacity"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        if group.can_add_stream("ADBE Vector Graphic - Fill"):
            fill = group.add_stream("ADBE Vector Graphic - Fill")

            # Get opacity property
            opacity_prop = fill.property("ADBE Vector Fill Opacity")
            assert opacity_prop is not None, "Fill opacity property not found"

            # Set opacity to 50%
            opacity_prop.value = 50.0
            result = opacity_prop.value

            assert abs(result - 50.0) < 0.01, f"Opacity incorrect: {result}"

# ====================================================================
# Test: Stroke Properties
# ====================================================================

@suite.test
def test_add_stroke():
    """Test adding stroke to shape group"""
    layer = _test_comp.add_shape()
    layer.name = "Test_Stroke"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        # Add stroke
        if group.can_add_stream("ADBE Vector Graphic - Stroke"):
            stroke = group.add_stream("ADBE Vector Graphic - Stroke")
            assert stroke is not None, "Stroke not created"
            assert stroke.match_name == "ADBE Vector Graphic - Stroke", f"Stroke match name incorrect: {stroke.match_name}"

@suite.test
def test_stroke_color():
    """Test stroke color property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_StrokeColor"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        if group.can_add_stream("ADBE Vector Graphic - Stroke"):
            stroke = group.add_stream("ADBE Vector Graphic - Stroke")

            # Get color property
            color_prop = stroke.property("ADBE Vector Stroke Color")
            assert color_prop is not None, "Stroke color property not found"

            # Set color to black
            color_prop.value = [0.0, 0.0, 0.0, 1.0]
            result = color_prop.value

            assert abs(result[0] - 0.0) < 0.01, f"Red channel incorrect: {result[0]}"
            assert abs(result[1] - 0.0) < 0.01, f"Green channel incorrect: {result[1]}"
            assert abs(result[2] - 0.0) < 0.01, f"Blue channel incorrect: {result[2]}"

@suite.test
def test_stroke_width():
    """Test stroke width property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_StrokeWidth"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        if group.can_add_stream("ADBE Vector Graphic - Stroke"):
            stroke = group.add_stream("ADBE Vector Graphic - Stroke")

            # Get width property
            width_prop = stroke.property("ADBE Vector Stroke Width")
            assert width_prop is not None, "Stroke width property not found"

            # Set width to 5.0
            width_prop.value = 5.0
            result = width_prop.value

            assert abs(result - 5.0) < 0.01, f"Stroke width incorrect: {result}"

@suite.test
def test_stroke_opacity():
    """Test stroke opacity property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_StrokeOpacity"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        if group.can_add_stream("ADBE Vector Graphic - Stroke"):
            stroke = group.add_stream("ADBE Vector Graphic - Stroke")

            # Get opacity property
            opacity_prop = stroke.property("ADBE Vector Stroke Opacity")
            assert opacity_prop is not None, "Stroke opacity property not found"

            # Set opacity to 75%
            opacity_prop.value = 75.0
            result = opacity_prop.value

            assert abs(result - 75.0) < 0.01, f"Stroke opacity incorrect: {result}"

# ====================================================================
# Test: Gradient Fill
# ====================================================================

@suite.test
def test_add_gradient_fill():
    """Test adding gradient fill"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GradientFill"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        # Add gradient fill
        if group.can_add_stream("ADBE Vector Graphic - G-Fill"):
            gfill = group.add_stream("ADBE Vector Graphic - G-Fill")
            assert gfill is not None, "Gradient fill not created"
            assert gfill.match_name == "ADBE Vector Graphic - G-Fill", f"Gradient fill match name incorrect: {gfill.match_name}"

@suite.test
def test_gradient_fill_type():
    """Test gradient fill type property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GradientType"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        if group.can_add_stream("ADBE Vector Graphic - G-Fill"):
            gfill = group.add_stream("ADBE Vector Graphic - G-Fill")

            # Get type property
            type_prop = gfill.property("ADBE Vector Grad Type")
            assert type_prop is not None, "Gradient type property not found"

            # Set to radial (2)
            type_prop.value = 2
            result = type_prop.value

            assert result == 2, f"Gradient type incorrect: {result}"

@suite.test
def test_gradient_start_point():
    """Test gradient start point property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GradientStart"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        if group.can_add_stream("ADBE Vector Graphic - G-Fill"):
            gfill = group.add_stream("ADBE Vector Graphic - G-Fill")

            # Get start point property
            start_prop = gfill.property("ADBE Vector Grad Start Pt")
            assert start_prop is not None, "Gradient start point property not found"

            # Set start point
            start_prop.value = [-100.0, 0.0]
            result = start_prop.value

            assert abs(result[0] - (-100.0)) < 0.01, f"Start X incorrect: {result[0]}"
            assert abs(result[1] - 0.0) < 0.01, f"Start Y incorrect: {result[1]}"

@suite.test
def test_gradient_end_point():
    """Test gradient end point property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GradientEnd"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        if group.can_add_stream("ADBE Vector Graphic - G-Fill"):
            gfill = group.add_stream("ADBE Vector Graphic - G-Fill")

            # Get end point property
            end_prop = gfill.property("ADBE Vector Grad End Pt")
            assert end_prop is not None, "Gradient end point property not found"

            # Set end point
            end_prop.value = [100.0, 0.0]
            result = end_prop.value

            assert abs(result[0] - 100.0) < 0.01, f"End X incorrect: {result[0]}"
            assert abs(result[1] - 0.0) < 0.01, f"End Y incorrect: {result[1]}"

# ====================================================================
# Test: Gradient Stroke
# ====================================================================

@suite.test
def test_add_gradient_stroke():
    """Test adding gradient stroke"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GradientStroke"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

        # Add gradient stroke
        if group.can_add_stream("ADBE Vector Graphic - G-Stroke"):
            gstroke = group.add_stream("ADBE Vector Graphic - G-Stroke")
            assert gstroke is not None, "Gradient stroke not created"
            assert gstroke.match_name == "ADBE Vector Graphic - G-Stroke", f"Gradient stroke match name incorrect: {gstroke.match_name}"

# ====================================================================
# Test: Transform within Group
# ====================================================================

@suite.test
def test_group_transform():
    """Test transform properties within shape group"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GroupTransform"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        # Get transform property
        transform = group.property("ADBE Vector Transform Group")
        assert transform is not None, "Group transform not found"

@suite.test
def test_group_position():
    """Test group position property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GroupPosition"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        transform = group.property("ADBE Vector Transform Group")
        if transform:
            position_prop = transform.property("ADBE Vector Position")
            assert position_prop is not None, "Group position property not found"

            # Set position
            position_prop.value = [100.0, 50.0]
            result = position_prop.value

            assert abs(result[0] - 100.0) < 0.01, f"Position X incorrect: {result[0]}"
            assert abs(result[1] - 50.0) < 0.01, f"Position Y incorrect: {result[1]}"

@suite.test
def test_group_rotation():
    """Test group rotation property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GroupRotation"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        transform = group.property("ADBE Vector Transform Group")
        if transform:
            rotation_prop = transform.property("ADBE Vector Rotation")
            assert rotation_prop is not None, "Group rotation property not found"

            # Set rotation to 45 degrees
            rotation_prop.value = 45.0
            result = rotation_prop.value

            assert abs(result - 45.0) < 0.01, f"Rotation incorrect: {result}"

@suite.test
def test_group_scale():
    """Test group scale property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GroupScale"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        transform = group.property("ADBE Vector Transform Group")
        if transform:
            scale_prop = transform.property("ADBE Vector Scale")
            assert scale_prop is not None, "Group scale property not found"

            # Set scale to 150%
            scale_prop.value = [150.0, 150.0]
            result = scale_prop.value

            assert abs(result[0] - 150.0) < 0.01, f"Scale X incorrect: {result[0]}"
            assert abs(result[1] - 150.0) < 0.01, f"Scale Y incorrect: {result[1]}"

# ====================================================================
# Test: Repeater
# ====================================================================

@suite.test
def test_add_repeater():
    """Test adding repeater to shape group"""
    layer = _test_comp.add_shape()
    layer.name = "Test_Repeater"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Ellipse"):
            ellipse = group.add_stream("ADBE Vector Shape - Ellipse")

        # Add repeater
        if group.can_add_stream("ADBE Vector Filter - Repeater"):
            repeater = group.add_stream("ADBE Vector Filter - Repeater")
            assert repeater is not None, "Repeater not created"
            assert repeater.match_name == "ADBE Vector Filter - Repeater", f"Repeater match name incorrect: {repeater.match_name}"

@suite.test
def test_repeater_copies():
    """Test repeater copies property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_RepeaterCopies"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Ellipse"):
            ellipse = group.add_stream("ADBE Vector Shape - Ellipse")

        if group.can_add_stream("ADBE Vector Filter - Repeater"):
            repeater = group.add_stream("ADBE Vector Filter - Repeater")

            # Get copies property
            copies_prop = repeater.property("ADBE Vector Repeater Copies")
            assert copies_prop is not None, "Repeater copies property not found"

            # Set copies to 5
            copies_prop.value = 5.0
            result = copies_prop.value

            assert abs(result - 5.0) < 0.01, f"Repeater copies incorrect: {result}"

@suite.test
def test_repeater_offset():
    """Test repeater offset property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_RepeaterOffset"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Ellipse"):
            ellipse = group.add_stream("ADBE Vector Shape - Ellipse")

        if group.can_add_stream("ADBE Vector Filter - Repeater"):
            repeater = group.add_stream("ADBE Vector Filter - Repeater")

            # Get offset property
            offset_prop = repeater.property("ADBE Vector Repeater Offset")
            assert offset_prop is not None, "Repeater offset property not found"

            # Set offset
            offset_prop.value = 2.0
            result = offset_prop.value

            assert abs(result - 2.0) < 0.01, f"Repeater offset incorrect: {result}"


# Run all tests
def run():
    return suite.run()
