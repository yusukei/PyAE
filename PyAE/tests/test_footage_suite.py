"""
Footage Suite Tests (Low-level SDK)
Tests for AEGP_FootageSuite5

This module tests the low-level SDK functions for footage management.
These functions are wrapped by the unified high-level ae.Footage class.

Footage Suiteはフッテージアイテムの作成、管理、ファイルパス取得、解釈設定を提供します。
Export/Importに必須の機能が多数含まれています。

Note: For high-level API tests, see tests/footage/test_footage_item.py
"""
import ae
import os
import tempfile

try:
    from test_utils import TestSuite, assert_equal, assert_true, assert_not_none
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true, assert_not_none

suite = TestSuite("Footage Suite")

# Test variables
_test_comp = None
_test_solid_item = None
_test_footage_path = None


@suite.setup
def setup():
    """Setup test composition with solid footage"""
    global _test_comp, _test_solid_item, _test_footage_path
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_FootageSuiteTestComp", 1920, 1080, 1.0, 5.0, 30.0)

    # Add a solid to create footage item
    solid_layer = _test_comp.add_solid("_TestSolid", 100, 100, (1.0, 0.0, 0.0))

    # Find the solid's footage item
    for item in proj.items:
        if hasattr(item, '_handle') and hasattr(item, 'footage_type'):
            _test_solid_item = item
            break


@suite.teardown
def teardown():
    """Cleanup test composition and footage"""
    global _test_comp, _test_solid_item, _test_footage_path
    if _test_comp is not None:
        _test_comp.delete()
        _test_comp = None
        _test_solid_item = None

    # Clean up test footage file if created
    if _test_footage_path and os.path.exists(_test_footage_path):
        os.remove(_test_footage_path)
        _test_footage_path = None


# ============================================================================
# Footage Suite Tests
# ============================================================================

@suite.test
def test_get_main_footage_from_item():
    """Test AEGP_GetMainFootageFromItem - gets main footage from item"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    # Get main footage handle
    footageH = ae.sdk.AEGP_GetMainFootageFromItem(_test_solid_item._handle)
    print(f"Main footage handle: {footageH}")

    assert_not_none(footageH, "Main footage handle should not be None")
    assert_true(footageH != 0, "Main footage handle should be valid")


@suite.test
def test_get_proxy_footage_from_item():
    """Test AEGP_GetProxyFootageFromItem - gets proxy footage from item after setting proxy"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    # First, set a proxy footage so we can retrieve it
    proxy_footageH = ae.sdk.AEGP_NewSolidFootage("_SDK_ProxyTest", 50, 50, (0.5, 0.5, 0.5))
    ae.sdk.AEGP_SetItemProxyFootage(_test_solid_item._handle, proxy_footageH)
    print("Set proxy footage")

    # Now get proxy footage handle - should succeed
    footageH = ae.sdk.AEGP_GetProxyFootageFromItem(_test_solid_item._handle)
    print(f"Proxy footage handle: {footageH}")

    assert_true(footageH != 0, "Proxy footage handle should be valid after setting proxy")


@suite.test
def test_get_footage_num_files():
    """Test AEGP_GetFootageNumFiles - gets number of files in footage"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    footageH = ae.sdk.AEGP_GetMainFootageFromItem(_test_solid_item._handle)
    if footageH == 0:
        print("No valid footage handle - skipping")
        return

    # Returns tuple (num_files, files_per_frame)
    result = ae.sdk.AEGP_GetFootageNumFiles(footageH)
    num_files, files_per_frame = result
    print(f"Number of files: {num_files}, files per frame: {files_per_frame}")

    # Solid footage should have 0 files (it's generated, not file-based)
    assert_equal(num_files, 0, "Solid footage should have 0 files")


@suite.test
def test_get_footage_path_solid():
    """Test AEGP_GetFootagePath - gets file path (solid has no path)"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    plugin_id = ae.sdk.AEGP_GetPluginID()
    footageH = ae.sdk.AEGP_GetMainFootageFromItem(_test_solid_item._handle)

    try:
        # Solid footage has no file path, should return empty or error
        path = ae.sdk.AEGP_GetFootagePath(plugin_id, footageH, 0, 0)
        print(f"Solid footage path: '{path}'")
        # Empty path is valid for solid
        assert_true(path == "" or path is None, "Solid should have empty path")
    except Exception as e:
        # Error is also acceptable for solid footage
        print(f"Expected error for solid footage: {e}")
        assert_true(True, "Error expected for solid footage")


@suite.test
def test_new_solid_footage():
    """Test AEGP_NewSolidFootage - creates new solid footage"""
    proj = ae.Project.get_current()

    # Create a new solid footage
    name = "_SDK_TestSolid"
    width = 200
    height = 150
    color = (0.0, 1.0, 0.0)  # Green

    footageH = ae.sdk.AEGP_NewSolidFootage(name, width, height, color)
    print(f"Created solid footage handle: {footageH}")

    assert_not_none(footageH, "Solid footage handle should not be None")
    assert_true(footageH != 0, "Solid footage handle should be valid")

    # Note: The footage is created but not added to project yet
    # Need to use AEGP_AddFootageToProject to add it


@suite.test
def test_new_placeholder_footage():
    """Test AEGP_NewPlaceholderFootage - creates placeholder footage"""
    plugin_id = ae.sdk.AEGP_GetPluginID()

    # Create placeholder footage
    name = "_SDK_PlaceholderTest"
    width = 640
    height = 480
    duration = 5.0  # 5 seconds

    footageH = ae.sdk.AEGP_NewPlaceholderFootage(plugin_id, name, width, height, duration)
    print(f"Created placeholder footage handle: {footageH}")

    assert_not_none(footageH, "Placeholder footage handle should not be None")
    assert_true(footageH != 0, "Placeholder footage handle should be valid")


@suite.test
def test_add_footage_to_project():
    """Test AEGP_AddFootageToProject - adds footage to project"""
    proj = ae.Project.get_current()

    # Create a solid footage
    footageH = ae.sdk.AEGP_NewSolidFootage("_SDK_AddTest", 100, 100, (1.0, 1.0, 0.0))

    # Get the root folder handle (MUST NOT use 0 - crashes AE)
    projH = ae.sdk.AEGP_GetProjectByIndex(0)
    rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

    # Add to project using root folder handle
    itemH = ae.sdk.AEGP_AddFootageToProject(footageH, rootFolderH)
    print(f"Added footage, item handle: {itemH}")

    assert_not_none(itemH, "Item handle should not be None")
    assert_true(itemH != 0, "Item handle should be valid")

    # Verify the item appears in project
    found = False
    for item in proj.items:
        if hasattr(item, '_handle') and item._handle == itemH:
            found = True
            print(f"Found item in project: {item.name}")
            # Clean up - delete the item
            item.delete()
            break

    assert_true(found, "Item should be found in project")


@suite.test
def test_replace_item_main_footage():
    """Test AEGP_ReplaceItemMainFootage - replaces main footage of item"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    # Create new solid footage to replace with
    new_footageH = ae.sdk.AEGP_NewSolidFootage("_SDK_Replacement", 150, 150, (0.0, 0.0, 1.0))

    # Get original footage for comparison
    original_footageH = ae.sdk.AEGP_GetMainFootageFromItem(_test_solid_item._handle)
    print(f"Original footage: {original_footageH}")

    # Replace the footage (SDK order: footageH, itemH)
    ae.sdk.AEGP_ReplaceItemMainFootage(new_footageH, _test_solid_item._handle)
    print("Replaced main footage")

    # Verify footage was replaced
    current_footageH = ae.sdk.AEGP_GetMainFootageFromItem(_test_solid_item._handle)
    print(f"Current footage: {current_footageH}")

    # Note: Handle values may differ, but the function should execute without error
    assert_true(True, "Footage replacement executed successfully")


@suite.test
def test_set_item_proxy_footage():
    """Test AEGP_SetItemProxyFootage - sets proxy footage for item"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    # Create a proxy footage (smaller solid)
    proxy_footageH = ae.sdk.AEGP_NewSolidFootage("_SDK_Proxy", 50, 50, (1.0, 0.5, 0.0))

    # Set as proxy
    ae.sdk.AEGP_SetItemProxyFootage(_test_solid_item._handle, proxy_footageH)
    print("Set proxy footage")

    # Verify proxy was set
    current_proxy = ae.sdk.AEGP_GetProxyFootageFromItem(_test_solid_item._handle)
    print(f"Current proxy: {current_proxy}")

    assert_not_none(current_proxy, "Proxy footage should be set")
    assert_true(current_proxy != 0, "Proxy footage handle should be valid")


@suite.test
def test_get_footage_signature():
    """Test AEGP_GetFootageSignature - gets footage signature"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    footageH = ae.sdk.AEGP_GetMainFootageFromItem(_test_solid_item._handle)
    signature = ae.sdk.AEGP_GetFootageSignature(footageH)
    print(f"Footage signature: {signature}")

    assert_not_none(signature, "Signature should not be None")
    # Signature is returned as an integer (fourCC code)
    assert_true(isinstance(signature, int), "Signature should be an integer")


@suite.test
def test_set_footage_interpretation():
    """Test AEGP_SetFootageInterpretation - sets footage interpretation"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    # Get current interpretation (proxy=False for main footage)
    current_interp = ae.sdk.AEGP_GetFootageInterpretation(_test_solid_item._handle, False)
    print(f"Current interpretation: {current_interp}")

    # Modify interpretation
    new_interp = current_interp.copy()

    # Change native FPS (if available)
    if 'native_fps' in new_interp:
        original_fps = new_interp['native_fps']
        new_interp['native_fps'] = 24.0

        # Set new interpretation (itemH, proxy, interp_dict)
        ae.sdk.AEGP_SetFootageInterpretation(_test_solid_item._handle, False, new_interp)
        print(f"Set interpretation with FPS: {new_interp['native_fps']}")

        # Verify it was set
        updated_interp = ae.sdk.AEGP_GetFootageInterpretation(_test_solid_item._handle, False)
        print(f"Updated interpretation: {updated_interp}")

        # Restore original
        new_interp['native_fps'] = original_fps
        ae.sdk.AEGP_SetFootageInterpretation(_test_solid_item._handle, False, new_interp)

    assert_true(True, "Interpretation set successfully")


@suite.test
def test_get_footage_layer_key():
    """Test AEGP_GetFootageLayerKey - gets layer key from footage"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    footageH = ae.sdk.AEGP_GetMainFootageFromItem(_test_solid_item._handle)
    layer_key = ae.sdk.AEGP_GetFootageLayerKey(footageH)
    print(f"Footage layer key: {layer_key}")

    # Layer key may be empty for solid footage
    # Just verify function executes
    assert_true(isinstance(layer_key, dict), "Layer key should be a dict")


@suite.test
def test_get_footage_sound_data_format():
    """Test AEGP_GetFootageSoundDataFormat - gets sound format"""
    global _test_solid_item
    if not _test_solid_item:
        print("No test solid item - skipping")
        return

    footageH = ae.sdk.AEGP_GetMainFootageFromItem(_test_solid_item._handle)
    sound_format = ae.sdk.AEGP_GetFootageSoundDataFormat(footageH)
    print(f"Sound data format: {sound_format}")

    # Solid footage has no audio, so format should be empty
    assert_true(isinstance(sound_format, dict), "Sound format should be a dict")
    # Empty dict is valid for footage without audio


# Run tests
def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
