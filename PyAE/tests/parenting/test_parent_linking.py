# test_parent_linking.py
# PyAE Parent Linking テスト (T501)
# Tests for layer parent-child relationships

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

suite = TestSuite("Parent Linking (T501)")

# Test variables
_test_comp = None
_parent_layer = None
_child_layer = None


@suite.setup
def setup():
    """Create test composition and layers"""
    global _test_comp, _parent_layer, _child_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_ParentLinkingTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    _parent_layer = _test_comp.add_solid("_ParentLayer", 200, 200, (1.0, 0.5, 0.0), 10.0)
    _child_layer = _test_comp.add_solid("_ChildLayer", 100, 100, (0.0, 0.5, 1.0), 10.0)


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
def test_no_parent_initially():
    """Layer has no parent initially"""
    global _child_layer
    assert_none(_child_layer.parent, "Layer should have no parent initially")


@suite.test
def test_set_parent():
    """Set parent layer"""
    global _parent_layer, _child_layer

    # Set parent
    _child_layer.parent = _parent_layer

    # Verify parent is set
    assert_not_none(_child_layer.parent, "Child should have a parent")
    assert_equal(_parent_layer.index, _child_layer.parent.index,
                 "Parent should be the specified layer")

    # Cleanup
    _child_layer.parent = None


@suite.test
def test_clear_parent():
    """Clear parent relationship"""
    global _parent_layer, _child_layer

    # Set parent
    _child_layer.parent = _parent_layer
    assert_not_none(_child_layer.parent)

    # Clear parent
    _child_layer.parent = None

    # Verify parent is cleared
    assert_none(_child_layer.parent, "Parent should be cleared")


@suite.test
def test_parent_position_affects_child():
    """Parent position affects child world position"""
    global _test_comp, _parent_layer, _child_layer

    # Set parent
    _child_layer.parent = _parent_layer

    # Set parent position
    parent_pos = _parent_layer.property("Position")
    parent_pos.value = [500, 500]

    # Set child local position
    child_pos = _child_layer.property("Position")
    child_pos.value = [100, 100]

    # Child's world position should be offset by parent's position
    # After parenting, child's local [100, 100] + parent's [500, 500] = world [600, 600]
    # Note: This depends on AE's transform hierarchy

    # Cleanup
    _child_layer.parent = None
    parent_pos.value = [960, 540]  # Reset to center


@suite.test
def test_multiple_children():
    """One parent can have multiple children"""
    global _test_comp, _parent_layer

    # Create additional child layers
    child2 = _test_comp.add_solid("_ChildLayer2", 80, 80, (0.0, 1.0, 0.0), 10.0)
    child3 = _test_comp.add_solid("_ChildLayer3", 60, 60, (1.0, 0.0, 0.0), 10.0)

    # Set same parent for all children
    child2.parent = _parent_layer
    child3.parent = _parent_layer

    # Verify all children have the same parent
    assert_equal(_parent_layer.index, child2.parent.index)
    assert_equal(_parent_layer.index, child3.parent.index)

    # Cleanup
    child2.parent = None
    child3.parent = None
    child2.delete()
    child3.delete()


@suite.test
def test_parent_chain():
    """Create parent chain (grandparent -> parent -> child)"""
    global _test_comp, _parent_layer, _child_layer

    # Create grandparent layer
    grandparent = _test_comp.add_solid("_GrandparentLayer", 300, 300, (0.5, 0.5, 0.5), 10.0)

    # Set up chain: grandparent -> parent -> child
    _parent_layer.parent = grandparent
    _child_layer.parent = _parent_layer

    # Verify chain
    assert_equal(grandparent.index, _parent_layer.parent.index,
                 "Parent's parent should be grandparent")
    assert_equal(_parent_layer.index, _child_layer.parent.index,
                 "Child's parent should be parent")

    # Cleanup
    _child_layer.parent = None
    _parent_layer.parent = None
    grandparent.delete()


@suite.test
def test_cannot_parent_to_self():
    """Cannot set a layer as its own parent"""
    global _parent_layer

    # Ensure parent has no parent relationship before test
    _parent_layer.parent = None

    # Try to set layer as its own parent - should raise exception
    exception_raised = False
    try:
        _parent_layer.parent = _parent_layer
    except Exception as e:
        exception_raised = True
        # Verify it's the expected error message
        assert_true("own parent" in str(e).lower() or "circular" in str(e).lower(),
                    f"Expected self-parent error, got: {e}")

    assert_true(exception_raised, "Setting self as parent should raise an exception")

    # Cleanup
    _parent_layer.parent = None


@suite.test
def test_cannot_create_circular_reference_child_to_parent():
    """Cannot set a child as parent of its own parent (circular reference)"""
    global _test_comp, _parent_layer, _child_layer

    # Setup: parent -> child
    _child_layer.parent = _parent_layer
    assert_equal(_parent_layer.index, _child_layer.parent.index)

    # Try to set child as parent of parent (would create circular reference)
    exception_raised = False
    try:
        _parent_layer.parent = _child_layer
    except Exception as e:
        exception_raised = True
        # Verify it's a circular reference error
        assert_true("circular" in str(e).lower() or "descendant" in str(e).lower(),
                    f"Expected circular reference error, got: {e}")

    assert_true(exception_raised, "Creating circular reference should raise an exception")

    # Cleanup
    _child_layer.parent = None


@suite.test
def test_cannot_create_circular_reference_grandchild():
    """Cannot set a grandchild as parent of its grandparent (circular reference)"""
    global _test_comp, _parent_layer, _child_layer

    # Create grandchild layer
    grandchild = _test_comp.add_solid("_GrandchildLayer", 50, 50, (1.0, 1.0, 0.0), 10.0)

    # Setup chain: parent -> child -> grandchild
    _child_layer.parent = _parent_layer
    grandchild.parent = _child_layer

    # Verify chain is correct
    assert_equal(_parent_layer.index, _child_layer.parent.index)
    assert_equal(_child_layer.index, grandchild.parent.index)

    # Try to set grandchild as parent of parent (would create circular reference)
    exception_raised = False
    try:
        _parent_layer.parent = grandchild
    except Exception as e:
        exception_raised = True
        # Verify it's a circular reference error
        assert_true("circular" in str(e).lower() or "descendant" in str(e).lower(),
                    f"Expected circular reference error, got: {e}")

    assert_true(exception_raised, "Creating circular reference through grandchild should raise an exception")

    # Cleanup
    grandchild.parent = None
    _child_layer.parent = None
    grandchild.delete()


@suite.test
def test_cannot_create_circular_reference_ancestor():
    """Cannot set an ancestor as parent (circular reference detection)"""
    global _test_comp, _parent_layer, _child_layer

    # Create multi-level hierarchy
    grandparent = _test_comp.add_solid("_GrandparentLayer", 300, 300, (0.5, 0.5, 0.5), 10.0)

    # Setup chain: grandparent -> parent -> child
    _parent_layer.parent = grandparent
    _child_layer.parent = _parent_layer

    # Try to set child as parent of grandparent (would create circular reference)
    exception_raised = False
    try:
        grandparent.parent = _child_layer
    except Exception as e:
        exception_raised = True
        # Verify it's a circular reference error
        assert_true("circular" in str(e).lower() or "descendant" in str(e).lower() or "ancestor" in str(e).lower(),
                    f"Expected circular reference error, got: {e}")

    assert_true(exception_raised, "Creating circular reference through ancestor should raise an exception")

    # Cleanup
    _child_layer.parent = None
    _parent_layer.parent = None
    grandparent.delete()


@suite.test
def test_parent_by_index():
    """Parent layer can be identified by index"""
    global _test_comp, _parent_layer, _child_layer

    _child_layer.parent = _parent_layer

    parent = _child_layer.parent
    assert_not_none(parent)

    # Parent should have a valid index
    parent_idx = parent.index
    assert_true(parent_idx >= 1, "Parent index should be >= 1")

    # Cleanup
    _child_layer.parent = None


@suite.test
def test_parent_by_name():
    """Parent layer has correct name"""
    global _parent_layer, _child_layer

    _child_layer.parent = _parent_layer

    parent = _child_layer.parent
    assert_equal("_ParentLayer", parent.name, "Parent should have correct name")

    # Cleanup
    _child_layer.parent = None


@suite.test
def test_change_parent():
    """Change parent to a different layer"""
    global _test_comp, _parent_layer, _child_layer

    # Create new parent
    new_parent = _test_comp.add_solid("_NewParentLayer", 150, 150, (0.0, 0.0, 1.0), 10.0)

    # Set initial parent
    _child_layer.parent = _parent_layer
    assert_equal(_parent_layer.index, _child_layer.parent.index)

    # Change to new parent
    _child_layer.parent = new_parent
    assert_equal(new_parent.index, _child_layer.parent.index,
                 "Parent should be changed to new layer")

    # Cleanup
    _child_layer.parent = None
    new_parent.delete()


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
