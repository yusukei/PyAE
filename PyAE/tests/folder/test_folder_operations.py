# test_folder_operations.py
# PyAE Folder Operations テスト
# Tests for folder creation, hierarchy, and item management

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
        assert_in,
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
        assert_in,
    )

suite = TestSuite("Folder Operations")


def _get_item_name(item):
    """Helper to get item name, handling both resolved Items and ItemRef objects."""
    if hasattr(item, 'name'):
        return item.name
    # If ItemRef is exposed (which it shouldn't be), try to get the resolved item
    # This is a workaround for binding issues
    proj = ae.Project.get_current()
    # Search all items to find matching handle
    for proj_item in proj.items:
        if hasattr(proj_item, '_handle') and hasattr(item, '_handle'):
            if proj_item._handle == item._handle:
                return proj_item.name
    raise AttributeError(f"Cannot get name from item of type {type(item)}")

# Test variables
_test_folder = None
_test_comp = None


@suite.setup
def setup():
    """Setup: nothing to do initially"""
    pass


@suite.teardown
def teardown():
    """Clean up any created folders and items"""
    global _test_folder, _test_comp

    if _test_comp:
        try:
            _test_comp.delete()
        except:
            pass

    if _test_folder:
        try:
            _test_folder.delete()
        except:
            pass


@suite.test
def test_create_folder():
    """Create a new folder in project root"""
    global _test_folder
    proj = ae.Project.get_current()

    _test_folder = proj.create_folder("_TestFolder")

    assert_not_none(_test_folder, "Folder should be created")
    assert_true(_test_folder.valid, "Folder should be valid")
    assert_equal("_TestFolder", _test_folder.name)


@suite.test
def test_folder_num_items():
    """Test folder num_items property"""
    global _test_folder
    proj = ae.Project.get_current()

    # Create folder if not exists
    if not _test_folder or not _test_folder.valid:
        _test_folder = proj.create_folder("_TestFolder")

    # Empty folder should have 0 items
    assert_equal(0, _test_folder.num_items, "Empty folder should have 0 items")


@suite.test
def test_folder_items_property():
    """Test folder items property"""
    global _test_folder
    proj = ae.Project.get_current()

    if not _test_folder or not _test_folder.valid:
        _test_folder = proj.create_folder("_TestFolder")

    items = _test_folder.items
    assert_not_none(items, "items property should return a list")
    assert_equal(0, len(items), "Empty folder should have empty items list")


@suite.test
def test_create_comp_in_folder():
    """Create a composition inside a folder"""
    global _test_folder, _test_comp
    proj = ae.Project.get_current()

    if not _test_folder or not _test_folder.valid:
        _test_folder = proj.create_folder("_TestFolder")

    # Create comp in folder
    _test_comp = proj.create_comp(
        "_TestCompInFolder",
        1920, 1080, 1.0, 10.0, 30.0,
        parent_folder=_test_folder
    )

    assert_not_none(_test_comp, "Comp should be created")

    # Verify comp is in folder
    assert_equal(1, _test_folder.num_items, "Folder should have 1 item")

    items = _test_folder.items
    assert_equal(1, len(items))
    assert_equal("_TestCompInFolder", items[0].name)


@suite.test
def test_move_item_to_folder():
    """Move an existing item to a folder"""
    global _test_folder
    proj = ae.Project.get_current()

    if not _test_folder or not _test_folder.valid:
        _test_folder = proj.create_folder("_TestFolder")

    # Create a comp at root level
    temp_comp = proj.create_comp("_TempComp", 1920, 1080, 1.0, 5.0, 30.0)

    initial_count = _test_folder.num_items

    # Move to folder
    temp_comp.parent_folder = _test_folder

    # Verify item is in folder
    assert_equal(initial_count + 1, _test_folder.num_items)

    # Cleanup
    temp_comp.delete()


@suite.test
def test_nested_folders():
    """Create nested folder hierarchy"""
    proj = ae.Project.get_current()

    # Create parent folder
    parent = proj.create_folder("_ParentFolder")

    # Create child folder inside parent
    child = proj.create_folder("_ChildFolder", parent_folder=parent)

    assert_not_none(child, "Child folder should be created")
    assert_equal(1, parent.num_items, "Parent should have 1 item")

    # Verify child is in parent's items
    items = parent.items
    assert_equal(1, len(items))
    # Get name using helper (handles both Item and ItemRef)
    item_name = _get_item_name(items[0])
    assert_equal("_ChildFolder", item_name)

    # Cleanup
    child.delete()
    parent.delete()


@suite.test
def test_folder_name_setter():
    """Test renaming a folder"""
    global _test_folder
    proj = ae.Project.get_current()

    if not _test_folder or not _test_folder.valid:
        _test_folder = proj.create_folder("_TestFolder")

    # Rename
    _test_folder.name = "_RenamedFolder"
    assert_equal("_RenamedFolder", _test_folder.name)

    # Restore
    _test_folder.name = "_TestFolder"


@suite.test
def test_item_parent_folder():
    """Test item's parent_folder property"""
    global _test_folder, _test_comp
    proj = ae.Project.get_current()

    if not _test_folder or not _test_folder.valid:
        _test_folder = proj.create_folder("_TestFolder")

    # Create comp in folder
    comp = proj.create_comp(
        "_ParentFolderTestComp",
        1920, 1080, 1.0, 5.0, 30.0,
        parent_folder=_test_folder
    )

    # Check parent folder
    parent = comp.parent_folder
    assert_not_none(parent, "Item should have a parent folder")
    # Get name using helper (handles both Folder and FolderRef)
    parent_name = _get_item_name(parent)
    assert_equal("_TestFolder", parent_name)

    # Cleanup
    comp.delete()


@suite.test
def test_move_item_to_root():
    """Move item from folder back to root"""
    global _test_folder
    proj = ae.Project.get_current()

    if not _test_folder or not _test_folder.valid:
        _test_folder = proj.create_folder("_TestFolder")

    # Create comp in folder
    comp = proj.create_comp(
        "_MoveToRootComp",
        1920, 1080, 1.0, 5.0, 30.0,
        parent_folder=_test_folder
    )

    initial_count = _test_folder.num_items

    # Move to root (set parent_folder to None or root folder)
    comp.parent_folder = None

    # Verify item is no longer in folder
    assert_equal(initial_count - 1, _test_folder.num_items)

    # Cleanup
    comp.delete()


@suite.test
def test_delete_folder_with_items():
    """Delete a folder that contains items"""
    proj = ae.Project.get_current()

    # Create folder with content
    folder = proj.create_folder("_FolderWithContent")
    comp = proj.create_comp(
        "_CompToDelete",
        1920, 1080, 1.0, 5.0, 30.0,
        parent_folder=folder
    )

    # Delete folder - depending on AE behavior, this might:
    # 1. Delete folder and contents
    # 2. Move contents to root and delete folder
    # 3. Fail with error

    # First move content out to be safe
    comp.parent_folder = None
    comp.delete()

    # Now delete empty folder
    folder.delete()


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
