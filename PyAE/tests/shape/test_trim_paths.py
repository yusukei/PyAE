"""
Shape Layer Trim Paths Test

Tests:
- Adding Trim Paths to shape group
- Trim Start property (percentage)
- Trim End property (percentage)
- Trim Offset property (degrees)
- Trim Paths animation (keyframes)
- Multiple Trim Paths operations
- Trim Type (Individually/Simultaneously)
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

suite = TestSuite("Trim Paths")

# Global test composition
_test_comp = None

@suite.setup
def setup():
    """Setup test composition"""
    global _test_comp
    project = ae.Project.get_current()
    _test_comp = project.create_comp("Test_TrimPaths", 1920, 1080, 1.0, 10.0, 30.0)

@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp
    if _test_comp:
        _test_comp.delete()
        _test_comp = None

# ====================================================================
# Helper: Create shape layer with rectangle and stroke
# ====================================================================

def _create_shape_with_stroke(name):
    """Create a shape layer with rectangle and stroke for testing trim paths"""
    layer = _test_comp.add_shape()
    layer.name = name

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        # Add rectangle shape
        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")
            size_prop = rect.property("ADBE Vector Rect Size")
            if size_prop:
                size_prop.value = [200.0, 200.0]

        # Add stroke to make path visible
        if group.can_add_stream("ADBE Vector Graphic - Stroke"):
            stroke = group.add_stream("ADBE Vector Graphic - Stroke")
            width_prop = stroke.property("ADBE Vector Stroke Width")
            if width_prop:
                width_prop.value = 5.0

        return layer, group

    return layer, None

# ====================================================================
# Test: Adding Trim Paths
# ====================================================================

@suite.test
def test_add_trim_paths():
    """Test adding Trim Paths to shape group"""
    layer, group = _create_shape_with_stroke("Test_AddTrimPaths")
    assert group is not None, "Shape group not created"

    # Add Trim Paths
    if group.can_add_stream("ADBE Vector Filter - Trim"):
        trim = group.add_stream("ADBE Vector Filter - Trim")
        assert trim is not None, "Trim Paths not created"
        assert trim.match_name == "ADBE Vector Filter - Trim", f"Trim Paths match name incorrect: {trim.match_name}"

# ====================================================================
# Test: Trim Start Property
# ====================================================================

@suite.test
def test_trim_start():
    """Test Trim Start property (percentage 0-100)"""
    layer, group = _create_shape_with_stroke("Test_TrimStart")
    assert group is not None, "Shape group not created"

    if group.can_add_stream("ADBE Vector Filter - Trim"):
        trim = group.add_stream("ADBE Vector Filter - Trim")

        # Get start property
        start_prop = trim.property("ADBE Vector Trim Start")
        assert start_prop is not None, "Trim Start property not found"

        # Default value should be 0
        default_value = start_prop.value
        assert abs(default_value - 0.0) < 0.01, f"Default Trim Start should be 0, got {default_value}"

        # Set start to 25%
        start_prop.value = 25.0
        result = start_prop.value
        assert abs(result - 25.0) < 0.01, f"Trim Start incorrect: expected 25.0, got {result}"

        # Set start to 50%
        start_prop.value = 50.0
        result = start_prop.value
        assert abs(result - 50.0) < 0.01, f"Trim Start incorrect: expected 50.0, got {result}"

        # Set start to 100%
        start_prop.value = 100.0
        result = start_prop.value
        assert abs(result - 100.0) < 0.01, f"Trim Start incorrect: expected 100.0, got {result}"

# ====================================================================
# Test: Trim End Property
# ====================================================================

@suite.test
def test_trim_end():
    """Test Trim End property (percentage 0-100)"""
    layer, group = _create_shape_with_stroke("Test_TrimEnd")
    assert group is not None, "Shape group not created"

    if group.can_add_stream("ADBE Vector Filter - Trim"):
        trim = group.add_stream("ADBE Vector Filter - Trim")

        # Get end property
        end_prop = trim.property("ADBE Vector Trim End")
        assert end_prop is not None, "Trim End property not found"

        # Default value should be 100
        default_value = end_prop.value
        assert abs(default_value - 100.0) < 0.01, f"Default Trim End should be 100, got {default_value}"

        # Set end to 75%
        end_prop.value = 75.0
        result = end_prop.value
        assert abs(result - 75.0) < 0.01, f"Trim End incorrect: expected 75.0, got {result}"

        # Set end to 50%
        end_prop.value = 50.0
        result = end_prop.value
        assert abs(result - 50.0) < 0.01, f"Trim End incorrect: expected 50.0, got {result}"

        # Set end to 0%
        end_prop.value = 0.0
        result = end_prop.value
        assert abs(result - 0.0) < 0.01, f"Trim End incorrect: expected 0.0, got {result}"

# ====================================================================
# Test: Trim Offset Property
# ====================================================================

@suite.test
def test_trim_offset():
    """Test Trim Offset property (degrees)"""
    layer, group = _create_shape_with_stroke("Test_TrimOffset")
    assert group is not None, "Shape group not created"

    if group.can_add_stream("ADBE Vector Filter - Trim"):
        trim = group.add_stream("ADBE Vector Filter - Trim")

        # Get offset property
        offset_prop = trim.property("ADBE Vector Trim Offset")
        assert offset_prop is not None, "Trim Offset property not found"

        # Default value should be 0
        default_value = offset_prop.value
        assert abs(default_value - 0.0) < 0.01, f"Default Trim Offset should be 0, got {default_value}"

        # Set offset to 90 degrees
        offset_prop.value = 90.0
        result = offset_prop.value
        assert abs(result - 90.0) < 0.01, f"Trim Offset incorrect: expected 90.0, got {result}"

        # Set offset to 180 degrees
        offset_prop.value = 180.0
        result = offset_prop.value
        assert abs(result - 180.0) < 0.01, f"Trim Offset incorrect: expected 180.0, got {result}"

        # Set offset to -90 degrees
        offset_prop.value = -90.0
        result = offset_prop.value
        assert abs(result - (-90.0)) < 0.01, f"Trim Offset incorrect: expected -90.0, got {result}"

        # Set offset to 360 degrees
        offset_prop.value = 360.0
        result = offset_prop.value
        assert abs(result - 360.0) < 0.01, f"Trim Offset incorrect: expected 360.0, got {result}"

# ====================================================================
# Test: Trim Paths Animation
# ====================================================================

@suite.test
def test_trim_paths_animation():
    """Test Trim Paths animation with keyframes"""
    layer, group = _create_shape_with_stroke("Test_TrimAnimation")
    assert group is not None, "Shape group not created"

    if group.can_add_stream("ADBE Vector Filter - Trim"):
        trim = group.add_stream("ADBE Vector Filter - Trim")

        # Get start and end properties
        start_prop = trim.property("ADBE Vector Trim Start")
        end_prop = trim.property("ADBE Vector Trim End")
        assert start_prop is not None, "Trim Start property not found"
        assert end_prop is not None, "Trim End property not found"

        # Add keyframes for Start property
        # Frame 0: Start = 0
        start_prop.set_value_at_time(0.0, 0.0)
        # Frame 5 (at 30fps = 0.167 seconds): Start = 50
        start_prop.set_value_at_time(50.0, 5.0 / 30.0)

        # Verify keyframe count
        num_keys = start_prop.num_keys
        assert num_keys >= 2, f"Expected at least 2 keyframes, got {num_keys}"

        # Verify keyframe values
        value_at_0 = start_prop.value_at_time(0.0)
        value_at_5 = start_prop.value_at_time(5.0 / 30.0)

        assert abs(value_at_0 - 0.0) < 0.01, f"Keyframe 1 value incorrect: expected 0.0, got {value_at_0}"
        assert abs(value_at_5 - 50.0) < 0.01, f"Keyframe 2 value incorrect: expected 50.0, got {value_at_5}"

        # Add keyframes for End property (reverse animation)
        # Frame 0: End = 100
        end_prop.set_value_at_time(100.0, 0.0)
        # Frame 5: End = 50
        end_prop.set_value_at_time(50.0, 5.0 / 30.0)

        end_num_keys = end_prop.num_keys
        assert end_num_keys >= 2, f"Expected at least 2 keyframes for End, got {end_num_keys}"

# ====================================================================
# Test: Multiple Trim Paths
# ====================================================================

@suite.test
def test_multiple_trim_paths():
    """Test adding multiple Trim Paths to the same group"""
    layer, group = _create_shape_with_stroke("Test_MultipleTrimPaths")
    assert group is not None, "Shape group not created"

    if group.can_add_stream("ADBE Vector Filter - Trim"):
        # Add first Trim Paths
        trim1 = group.add_stream("ADBE Vector Filter - Trim")
        assert trim1 is not None, "First Trim Paths not created"

        # Configure first trim
        start1 = trim1.property("ADBE Vector Trim Start")
        end1 = trim1.property("ADBE Vector Trim End")
        if start1:
            start1.value = 0.0
        if end1:
            end1.value = 50.0

        # Add second Trim Paths
        if group.can_add_stream("ADBE Vector Filter - Trim"):
            trim2 = group.add_stream("ADBE Vector Filter - Trim")
            assert trim2 is not None, "Second Trim Paths not created"

            # Configure second trim
            start2 = trim2.property("ADBE Vector Trim Start")
            end2 = trim2.property("ADBE Vector Trim End")
            if start2:
                start2.value = 25.0
            if end2:
                end2.value = 75.0

            # Verify both trims exist with different values
            result_start1 = start1.value if start1 else 0
            result_end1 = end1.value if end1 else 0
            result_start2 = start2.value if start2 else 0
            result_end2 = end2.value if end2 else 0

            assert abs(result_start1 - 0.0) < 0.01, f"Trim1 Start incorrect: {result_start1}"
            assert abs(result_end1 - 50.0) < 0.01, f"Trim1 End incorrect: {result_end1}"
            assert abs(result_start2 - 25.0) < 0.01, f"Trim2 Start incorrect: {result_start2}"
            assert abs(result_end2 - 75.0) < 0.01, f"Trim2 End incorrect: {result_end2}"

# ====================================================================
# Test: Trim Paths Individually Setting
# ====================================================================

@suite.test
def test_trim_paths_individually():
    """Test Trim Type property (Simultaneously=1 / Individually=2)"""
    layer, group = _create_shape_with_stroke("Test_TrimIndividually")
    assert group is not None, "Shape group not created"

    if group.can_add_stream("ADBE Vector Filter - Trim"):
        trim = group.add_stream("ADBE Vector Filter - Trim")

        # Get trim type property
        type_prop = trim.property("ADBE Vector Trim Type")
        assert type_prop is not None, "Trim Type property not found"

        # Get default value (should be 1 = Simultaneously)
        default_value = type_prop.value
        # Note: Default may be 1 (Simultaneously) or 2 (Individually) depending on AE version
        assert default_value in [1, 2], f"Trim Type should be 1 or 2, got {default_value}"

        # Set to Simultaneously (1)
        type_prop.value = 1
        result = type_prop.value
        assert result == 1, f"Trim Type incorrect: expected 1 (Simultaneously), got {result}"

        # Set to Individually (2)
        type_prop.value = 2
        result = type_prop.value
        assert result == 2, f"Trim Type incorrect: expected 2 (Individually), got {result}"

# ====================================================================
# Test: Trim Start and End Combined
# ====================================================================

@suite.test
def test_trim_start_end_combined():
    """Test setting both Trim Start and End to create a visible segment"""
    layer, group = _create_shape_with_stroke("Test_TrimCombined")
    assert group is not None, "Shape group not created"

    if group.can_add_stream("ADBE Vector Filter - Trim"):
        trim = group.add_stream("ADBE Vector Filter - Trim")

        start_prop = trim.property("ADBE Vector Trim Start")
        end_prop = trim.property("ADBE Vector Trim End")
        offset_prop = trim.property("ADBE Vector Trim Offset")

        assert start_prop is not None, "Trim Start property not found"
        assert end_prop is not None, "Trim End property not found"
        assert offset_prop is not None, "Trim Offset property not found"

        # Set a specific visible segment (25% to 75% with 45 degree offset)
        start_prop.value = 25.0
        end_prop.value = 75.0
        offset_prop.value = 45.0

        # Verify all values
        result_start = start_prop.value
        result_end = end_prop.value
        result_offset = offset_prop.value

        assert abs(result_start - 25.0) < 0.01, f"Start incorrect: {result_start}"
        assert abs(result_end - 75.0) < 0.01, f"End incorrect: {result_end}"
        assert abs(result_offset - 45.0) < 0.01, f"Offset incorrect: {result_offset}"

        # Verify the segment length (End - Start = 50%)
        segment_length = result_end - result_start
        assert abs(segment_length - 50.0) < 0.01, f"Segment length incorrect: {segment_length}"

# ====================================================================
# Test: Trim Paths on Ellipse Shape
# ====================================================================

@suite.test
def test_trim_paths_on_ellipse():
    """Test Trim Paths applied to ellipse shape"""
    layer = _test_comp.add_shape()
    layer.name = "Test_TrimEllipse"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        # Add ellipse shape
        if group.can_add_stream("ADBE Vector Shape - Ellipse"):
            ellipse = group.add_stream("ADBE Vector Shape - Ellipse")
            size_prop = ellipse.property("ADBE Vector Ellipse Size")
            if size_prop:
                size_prop.value = [200.0, 100.0]

        # Add stroke
        if group.can_add_stream("ADBE Vector Graphic - Stroke"):
            stroke = group.add_stream("ADBE Vector Graphic - Stroke")
            width_prop = stroke.property("ADBE Vector Stroke Width")
            if width_prop:
                width_prop.value = 3.0

        # Add Trim Paths
        if group.can_add_stream("ADBE Vector Filter - Trim"):
            trim = group.add_stream("ADBE Vector Filter - Trim")
            assert trim is not None, "Trim Paths not created for ellipse"

            # Set trim values
            start_prop = trim.property("ADBE Vector Trim Start")
            end_prop = trim.property("ADBE Vector Trim End")

            if start_prop:
                start_prop.value = 0.0
            if end_prop:
                end_prop.value = 75.0

            # Verify
            result = end_prop.value if end_prop else 0
            assert abs(result - 75.0) < 0.01, f"Trim End on ellipse incorrect: {result}"

# ====================================================================
# Test: Trim Paths Boundary Values
# ====================================================================

@suite.test
def test_trim_paths_boundary_values():
    """Test Trim Paths with boundary values"""
    layer, group = _create_shape_with_stroke("Test_TrimBoundary")
    assert group is not None, "Shape group not created"

    if group.can_add_stream("ADBE Vector Filter - Trim"):
        trim = group.add_stream("ADBE Vector Filter - Trim")

        start_prop = trim.property("ADBE Vector Trim Start")
        end_prop = trim.property("ADBE Vector Trim End")

        # Test Start = 0, End = 0 (invisible path)
        start_prop.value = 0.0
        end_prop.value = 0.0
        assert abs(start_prop.value - 0.0) < 0.01, "Start at 0 failed"
        assert abs(end_prop.value - 0.0) < 0.01, "End at 0 failed"

        # Test Start = 100, End = 100 (invisible path)
        start_prop.value = 100.0
        end_prop.value = 100.0
        assert abs(start_prop.value - 100.0) < 0.01, "Start at 100 failed"
        assert abs(end_prop.value - 100.0) < 0.01, "End at 100 failed"

        # Test Start = 0, End = 100 (full path)
        start_prop.value = 0.0
        end_prop.value = 100.0
        assert abs(start_prop.value - 0.0) < 0.01, "Start at 0 failed (full path)"
        assert abs(end_prop.value - 100.0) < 0.01, "End at 100 failed (full path)"


# Run all tests
def run():
    return suite.run()
