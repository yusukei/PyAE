"""
Shape Layer Basic Operations Test

Tests:
- Shape layer creation
- Shape layer type detection
- Basic shape group operations
- Rectangle shape creation
- Ellipse shape creation
- Path shape creation
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

suite = TestSuite("Shape Layer Basic")

# Global test composition
_test_comp = None

@suite.setup
def setup():
    """Setup test composition"""
    global _test_comp
    project = ae.Project.get_current()
    _test_comp = project.create_comp("Test_ShapeLayer_Basic", 1920, 1080, 1.0, 10.0, 30.0)

@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp
    if _test_comp:
        _test_comp.delete()
        _test_comp = None

# ====================================================================
# Test: Shape Layer Creation
# ====================================================================

@suite.test
def test_create_shape_layer():
    """Test shape layer creation"""
    layer = _test_comp.add_shape()
    layer.name = "Test_Shape"

    assert layer is not None, "Shape layer not created"
    assert layer.name == "Test_Shape", f"Name mismatch: {layer.name}"
    assert layer.layer_type == ae.LayerType.Shape, f"Layer type mismatch: {layer.layer_type}"

@suite.test
def test_shape_layer_is_shape():
    """Test shape layer type detection"""
    shape_layer = _test_comp.add_shape()
    shape_layer.name = "Shape_Check"

    # Compare with other layer types
    solid_layer = _test_comp.add_solid("Solid_Check", 100, 100, (1.0, 0.0, 0.0))
    null_layer = _test_comp.add_null("Null_Check")

    assert shape_layer.layer_type == ae.LayerType.Shape, "Shape layer type not detected"
    assert solid_layer.layer_type != ae.LayerType.Shape, "Solid incorrectly detected as shape"
    assert null_layer.layer_type != ae.LayerType.Shape, "Null incorrectly detected as shape"

@suite.test
def test_shape_layer_has_contents():
    """Test shape layer has Contents property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_Contents"

    root = layer.properties
    assert root is not None, "Layer properties not accessible"

    contents = root.property("ADBE Root Vectors Group")
    assert contents is not None, "Contents property not found"
    assert contents.match_name == "ADBE Root Vectors Group", "Contents match name incorrect"

# ====================================================================
# Test: Shape Group Operations
# ====================================================================

@suite.test
def test_add_vector_group():
    """Test adding a vector group to shape layer"""
    layer = _test_comp.add_shape()
    layer.name = "Test_AddGroup"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    # Check if can add stream
    if hasattr(contents, 'can_add_stream'):
        can_add = contents.can_add_stream("ADBE Vector Group")
        assert can_add, "Cannot add vector group"

        # Add vector group
        group = contents.add_stream("ADBE Vector Group")
        assert group is not None, "Vector group not created"
        assert group.match_name == "ADBE Vector Group", f"Group match name incorrect: {group.match_name}"

@suite.test
def test_set_group_name():
    """Test setting vector group name"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GroupName"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        # Set group name
        if hasattr(group, 'set_name'):
            group.set_name("Custom Group Name")
            assert group.name == "Custom Group Name", f"Group name not set: {group.name}"

@suite.test
def test_count_shape_groups():
    """Test counting shape groups"""
    layer = _test_comp.add_shape()
    layer.name = "Test_GroupCount"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    initial_count = contents.num_properties if hasattr(contents, 'num_properties') else 0

    # Add multiple groups
    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group1 = contents.add_stream("ADBE Vector Group")
        group2 = contents.add_stream("ADBE Vector Group")
        group3 = contents.add_stream("ADBE Vector Group")

        final_count = contents.num_properties
        assert final_count == initial_count + 3, f"Group count incorrect: expected {initial_count + 3}, got {final_count}"

# ====================================================================
# Test: Rectangle Shape
# ====================================================================

@suite.test
def test_add_rectangle_shape():
    """Test adding rectangle shape"""
    layer = _test_comp.add_shape()
    layer.name = "Test_Rectangle"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        # Add rectangle shape
        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")
            assert rect is not None, "Rectangle shape not created"
            assert rect.match_name == "ADBE Vector Shape - Rect", f"Rectangle match name incorrect: {rect.match_name}"

@suite.test
def test_rectangle_size_property():
    """Test rectangle size property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_RectSize"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

            # Get size property
            size_prop = rect.property("ADBE Vector Rect Size")
            assert size_prop is not None, "Rectangle size property not found"

            # Set size
            size_prop.value = [200.0, 150.0]
            result = size_prop.value

            assert abs(result[0] - 200.0) < 0.01, f"Rectangle width incorrect: {result[0]}"
            assert abs(result[1] - 150.0) < 0.01, f"Rectangle height incorrect: {result[1]}"

@suite.test
def test_rectangle_roundness():
    """Test rectangle roundness property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_RectRoundness"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Rect"):
            rect = group.add_stream("ADBE Vector Shape - Rect")

            # Get roundness property
            roundness_prop = rect.property("ADBE Vector Rect Roundness")
            assert roundness_prop is not None, "Rectangle roundness property not found"

            # Set roundness
            roundness_prop.value = 25.0
            result = roundness_prop.value

            assert abs(result - 25.0) < 0.01, f"Rectangle roundness incorrect: {result}"

# ====================================================================
# Test: Ellipse Shape
# ====================================================================

@suite.test
def test_add_ellipse_shape():
    """Test adding ellipse shape"""
    layer = _test_comp.add_shape()
    layer.name = "Test_Ellipse"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        # Add ellipse shape
        if group.can_add_stream("ADBE Vector Shape - Ellipse"):
            ellipse = group.add_stream("ADBE Vector Shape - Ellipse")
            assert ellipse is not None, "Ellipse shape not created"
            assert ellipse.match_name == "ADBE Vector Shape - Ellipse", f"Ellipse match name incorrect: {ellipse.match_name}"

@suite.test
def test_ellipse_size_property():
    """Test ellipse size property"""
    layer = _test_comp.add_shape()
    layer.name = "Test_EllipseSize"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Ellipse"):
            ellipse = group.add_stream("ADBE Vector Shape - Ellipse")

            # Get size property
            size_prop = ellipse.property("ADBE Vector Ellipse Size")
            assert size_prop is not None, "Ellipse size property not found"

            # Set size
            size_prop.value = [180.0, 120.0]
            result = size_prop.value

            assert abs(result[0] - 180.0) < 0.01, f"Ellipse width incorrect: {result[0]}"
            assert abs(result[1] - 120.0) < 0.01, f"Ellipse height incorrect: {result[1]}"

# ====================================================================
# Test: Path Shape (Custom)
# ====================================================================

@suite.test
def test_add_path_shape():
    """Test adding path shape"""
    layer = _test_comp.add_shape()
    layer.name = "Test_Path"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        # Add path shape
        if group.can_add_stream("ADBE Vector Shape - Group"):
            path = group.add_stream("ADBE Vector Shape - Group")
            assert path is not None, "Path shape not created"
            assert path.match_name == "ADBE Vector Shape - Group", f"Path match name incorrect: {path.match_name}"

@suite.test
def test_path_vertices():
    """Test setting and getting path vertices"""
    layer = _test_comp.add_shape()
    layer.name = "Test_PathVertices"

    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")

    if hasattr(contents, 'can_add_stream') and contents.can_add_stream("ADBE Vector Group"):
        group = contents.add_stream("ADBE Vector Group")

        if group.can_add_stream("ADBE Vector Shape - Group"):
            path = group.add_stream("ADBE Vector Shape - Group")

            # Get path property
            path_prop = path.property("ADBE Vector Shape")
            assert path_prop is not None, "Path property not found"

            # Set vertices (triangle)
            if hasattr(path_prop, 'set_shape_path_vertices'):
                vertices = [
                    {"x": 0, "y": -50, "tan_in_x": 0, "tan_in_y": 0, "tan_out_x": 0, "tan_out_y": 0},
                    {"x": 50, "y": 50, "tan_in_x": 0, "tan_in_y": 0, "tan_out_x": 0, "tan_out_y": 0},
                    {"x": -50, "y": 50, "tan_in_x": 0, "tan_in_y": 0, "tan_out_x": 0, "tan_out_y": 0}
                ]
                path_prop.set_shape_path_vertices(vertices, 0.0)

                # Get vertices back
                if hasattr(path_prop, 'get_shape_path_vertices'):
                    result = path_prop.get_shape_path_vertices(0.0)
                    assert len(result) == 3, f"Vertex count incorrect: expected 3, got {len(result)}"


# Run all tests
def run():
    return suite.run()
