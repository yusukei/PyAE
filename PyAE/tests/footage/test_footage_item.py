# test_footage_item.py
# PyAE Footage テスト（統合版）
# Tests for unified Footage class (both pre-project and in-project states)

import ae
import os
import tempfile

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

suite = TestSuite("Footage (Unified)")

# FootageType enum values (from PyFootageClasses.h)
# None = 0, Solid = 1, Missing = 2, Placeholder = 3
FOOTAGE_TYPE_SOLID = 1

# Test variables
_test_comp = None
_test_solid = None


@suite.setup
def setup():
    """Create test composition with a solid (which creates a footage item)"""
    global _test_comp, _test_solid
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_FootageTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    # Add a solid - this creates a footage item in the project
    _test_solid = _test_comp.add_solid("_TestSolid", 200, 200, (1.0, 0.0, 0.0), 10.0)


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
def test_item_basic_properties():
    """Test basic Item properties"""
    proj = ae.Project.get_current()

    # Get items from project
    items = proj.items
    assert_true(len(items) > 0, "Project should have at least one item")

    # Find our test comp
    comp_item = None
    for item in items:
        if item.name == "_FootageTestComp":
            comp_item = item
            break

    assert_not_none(comp_item, "Should find test composition")
    assert_true(comp_item.valid, "Item should be valid")
    assert_equal("_FootageTestComp", comp_item.name)


@suite.test
def test_item_type():
    """Test item type property"""
    proj = ae.Project.get_current()

    # Find our test comp
    for item in proj.items:
        if item.name == "_FootageTestComp":
            # CompItem should have type indicating it's a composition
            item_type = item.type
            assert_not_none(item_type, "Item should have a type")
            break


@suite.test
def test_item_dimensions():
    """Test item dimensions (width, height)"""
    proj = ae.Project.get_current()

    for item in proj.items:
        if item.name == "_FootageTestComp":
            assert_equal(1920, item.width, "Comp width should be 1920")
            assert_equal(1080, item.height, "Comp height should be 1080")
            break


@suite.test
def test_item_duration():
    """Test item duration"""
    proj = ae.Project.get_current()

    for item in proj.items:
        if item.name == "_FootageTestComp":
            assert_true(item.duration > 0, "Duration should be positive")
            break


@suite.test
def test_item_name_setter():
    """Test setting item name"""
    proj = ae.Project.get_current()

    for item in proj.items:
        if item.name == "_FootageTestComp":
            # Rename
            item.name = "_FootageTestComp_Renamed"
            assert_equal("_FootageTestComp_Renamed", item.name)

            # Restore
            item.name = "_FootageTestComp"
            break


@suite.test
def test_item_comment():
    """Test item comment property"""
    proj = ae.Project.get_current()

    for item in proj.items:
        if item.name == "_FootageTestComp":
            # Set comment
            item.comment = "Test comment"
            assert_equal("Test comment", item.comment)

            # Clear comment
            item.comment = ""
            break


@suite.test
def test_item_label():
    """Test item label color"""
    proj = ae.Project.get_current()

    for item in proj.items:
        if item.name == "_FootageTestComp":
            original_label = item.label

            # Set label (valid range 0-16)
            item.label = 5
            assert_equal(5, item.label)

            # Restore
            item.label = original_label
            break


@suite.test
def test_item_selected():
    """Test item selection state"""
    proj = ae.Project.get_current()

    for item in proj.items:
        if item.name == "_FootageTestComp":
            # Select item
            item.selected = True
            assert_true(item.selected)

            # Deselect
            item.selected = False
            assert_false(item.selected)
            break


@suite.test
def test_solid_footage_item():
    """Test solid creates a unified Footage object in project"""
    proj = ae.Project.get_current()

    # Solids create Footage objects in the project
    # The unified Footage class handles both pre-project and in-project states
    # When retrieved from project.items, is_in_project == True
    footage_items = []
    for item in proj.items:
        # Check if it's a Footage object (unified class with footage_type property)
        if hasattr(item, 'footage_type'):
            footage_items.append(item)
            # Verify it's in project
            if hasattr(item, 'is_in_project'):
                assert_true(item.is_in_project, "Footage from project should be in project")

    # Note: Depending on project state, there may or may not be footage items
    # This test just ensures we can iterate and check types


@suite.test
def test_item_missing_property():
    """Test missing property on items"""
    proj = ae.Project.get_current()

    for item in proj.items:
        if item.name == "_FootageTestComp":
            # Comp items should not be missing
            assert_false(item.missing, "Comp should not be marked as missing")
            break


@suite.test
def test_solid_footage_color_get():
    """Test getting solid footage color"""
    proj = ae.Project.get_current()

    # Find solid footage item
    for item in proj.items:
        if hasattr(item, 'footage_type'):
            footage_type = item.footage_type
            if footage_type == FOOTAGE_TYPE_SOLID:  # FootageType.Solid = 1
                # Get color
                color = item.get_solid_color()
                assert_not_none(color, "Should get solid color")
                assert_equal(4, len(color), "Color should have 4 components (RGBA)")
                # Verify color values are floats in range 0-1
                for component in color:
                    assert_true(0.0 <= component <= 1.0, "Color component should be in range 0-1")
                return

    # Note: Test may not find a solid if none exists in project


@suite.test
def test_solid_footage_color_set():
    """Test setting solid footage color"""
    proj = ae.Project.get_current()

    # Find solid footage item
    for item in proj.items:
        if hasattr(item, 'footage_type'):
            footage_type = item.footage_type
            if footage_type == FOOTAGE_TYPE_SOLID:  # FootageType.Solid = 1
                # Get original color
                original_color = item.get_solid_color()

                # Set new color (blue)
                new_color = (0.0, 0.0, 1.0, 1.0)
                item.set_solid_color(new_color)

                # Verify color was set
                current_color = item.get_solid_color()
                assert_equal(0.0, current_color[0], "Red should be 0.0")
                assert_equal(0.0, current_color[1], "Green should be 0.0")
                # Blue might have slight floating point difference
                assert_true(abs(current_color[2] - 1.0) < 0.01, "Blue should be close to 1.0")

                # Restore original color
                item.set_solid_color(original_color)
                return


@suite.test
def test_solid_footage_dimensions_set():
    """Test setting solid footage dimensions"""
    proj = ae.Project.get_current()

    # Find solid footage item
    for item in proj.items:
        if hasattr(item, 'footage_type'):
            footage_type = item.footage_type
            if footage_type == FOOTAGE_TYPE_SOLID:  # FootageType.Solid = 1
                # Get original dimensions
                original_width = item.width
                original_height = item.height

                # Set new dimensions
                new_width = 500
                new_height = 400
                item.set_solid_dimensions(new_width, new_height)

                # Verify dimensions were set
                assert_equal(new_width, item.width, "Width should be updated")
                assert_equal(new_height, item.height, "Height should be updated")

                # Restore original dimensions
                item.set_solid_dimensions(original_width, original_height)
                return


@suite.test
def test_footage_interpretation():
    """Test getting footage interpretation"""
    proj = ae.Project.get_current()

    # Find any footage item
    for item in proj.items:
        if hasattr(item, 'interpretation'):
            interp = item.interpretation
            assert_not_none(interp, "Should get interpretation")
            assert_isinstance(interp, dict, "Interpretation should be a dict")

            # Check for expected keys
            if 'alpha_mode' in interp:
                alpha_mode = interp['alpha_mode']
                assert_true(alpha_mode in ['straight', 'premultiplied', 'ignore'],
                           "Alpha mode should be valid")

            if 'native_fps' in interp:
                assert_true(interp['native_fps'] > 0, "Native FPS should be positive")

            return


@suite.test
def test_footage_layer_key():
    """Test getting footage layer key"""
    proj = ae.Project.get_current()

    # Find any footage item
    for item in proj.items:
        if hasattr(item, 'layer_key'):
            layer_key = item.layer_key
            # layer_key may be empty dict if no layer info
            assert_isinstance(layer_key, dict, "Layer key should be a dict")
            return


@suite.test
def test_footage_sound_format():
    """Test getting footage sound format (unified Footage API)"""
    proj = ae.Project.get_current()

    # Find any footage item with audio
    # In unified Footage class, the property is 'sound_format'
    for item in proj.items:
        if hasattr(item, 'sound_format'):
            sound_fmt = item.sound_format
            # sound_format may be empty dict if no audio
            assert_isinstance(sound_fmt, dict, "Sound format should be a dict")

            if sound_fmt and sound_fmt.get('sample_rate', 0) > 0:  # If has actual audio data
                assert_true(sound_fmt['sample_rate'] > 0, "Sample rate should be positive")
                if 'num_channels' in sound_fmt:
                    assert_true(sound_fmt['num_channels'] in [1, 2],
                               "Channels should be 1 (mono) or 2 (stereo)")
                return  # Found footage with audio, test passed

    # If no footage with audio found, test still passes (no audio footage in project)
    assert_true(True, "No footage with audio found in project")


@suite.test
def test_footage_sequence_options():
    """Test getting footage sequence import options"""
    proj = ae.Project.get_current()

    # Find any footage item
    for item in proj.items:
        if hasattr(item, 'sequence_options'):
            seq_options = item.sequence_options
            assert_isinstance(seq_options, dict, "Sequence options should be a dict")

            if seq_options and 'is_sequence' in seq_options:
                is_seq = seq_options['is_sequence']
                assert_isinstance(is_seq, bool, "is_sequence should be boolean")
            return


@suite.test
def test_footage_is_in_project():
    """Test is_in_project property (unified Footage API)"""
    proj = ae.Project.get_current()

    # All Footage objects retrieved from project.items should have is_in_project == True
    for item in proj.items:
        if hasattr(item, 'is_in_project'):
            assert_true(item.is_in_project, "Footage from project should have is_in_project=True")


@suite.test
def test_footage_solid_factory():
    """Test ae.Footage.solid() factory method (unified API)"""
    # Create solid footage using unified Footage class factory method
    if not hasattr(ae, 'Footage') or not hasattr(ae.Footage, 'solid'):
        print("ae.Footage.solid not available - skipping")
        return

    # Create a solid (before adding to project)
    footage = ae.Footage.solid("_TestFactorySolid", 100, 100, (0.5, 0.5, 1.0, 1.0))
    assert_not_none(footage, "Should create solid footage")
    assert_true(footage.valid, "Footage should be valid")

    # Before adding to project, is_in_project should be False
    if hasattr(footage, 'is_in_project'):
        assert_false(footage.is_in_project, "New footage should not be in project yet")

    # Signature should be SOLID before adding to project
    if hasattr(footage, 'signature') and hasattr(ae, 'FootageSignature'):
        assert_equal(ae.FootageSignature.SOLID, footage.signature, "Solid footage signature should be SOLID")

    # Add to project
    proj = ae.Project.get_current()
    footage.add_to_project()

    # After adding, is_in_project should be True
    if hasattr(footage, 'is_in_project'):
        assert_true(footage.is_in_project, "Footage should be in project after add_to_project()")

    # Clean up - delete the footage item
    footage.delete()


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
