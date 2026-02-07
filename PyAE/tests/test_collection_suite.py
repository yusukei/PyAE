"""
Collection Suite Tests
Tests for AEGP_CollectionSuite2

Collection Suiteは選択されたアイテム（レイヤー、マスク、エフェクト、ストリームなど）を
コレクションとして管理する機能を提供します。
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true

suite = TestSuite("Collection Suite")

# Test variables
_test_comp = None
_test_layer1 = None
_test_layer2 = None


@suite.setup
def setup():
    """Setup test composition with layers"""
    global _test_comp, _test_layer1, _test_layer2
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_CollectionTestComp", 1920, 1080, 1.0, 5.0, 30.0)

    # Create test layers
    _test_layer1 = _test_comp.add_solid("_TestSolid1", 100, 100, (1.0, 0.0, 0.0))
    _test_layer2 = _test_comp.add_solid("_TestSolid2", 100, 100, (0.0, 1.0, 0.0))


@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp, _test_layer1, _test_layer2
    if _test_comp is not None:
        _test_comp.delete()
        _test_comp = None
        _test_layer1 = None
        _test_layer2 = None


# ============================================================================
# Collection Suite Tests
# ============================================================================

@suite.test
def test_collection_constants():
    """Test that all Collection constants are defined"""
    # Collection Item Type constants
    assert_true(hasattr(ae.sdk, 'AEGP_CollectionItemType_NONE'),
                "AEGP_CollectionItemType_NONE defined")
    assert_true(hasattr(ae.sdk, 'AEGP_CollectionItemType_LAYER'),
                "AEGP_CollectionItemType_LAYER defined")
    assert_true(hasattr(ae.sdk, 'AEGP_CollectionItemType_MASK'),
                "AEGP_CollectionItemType_MASK defined")
    assert_true(hasattr(ae.sdk, 'AEGP_CollectionItemType_EFFECT'),
                "AEGP_CollectionItemType_EFFECT defined")
    assert_true(hasattr(ae.sdk, 'AEGP_CollectionItemType_STREAM'),
                "AEGP_CollectionItemType_STREAM defined")
    assert_true(hasattr(ae.sdk, 'AEGP_CollectionItemType_MASK_VERTEX'),
                "AEGP_CollectionItemType_MASK_VERTEX defined")
    assert_true(hasattr(ae.sdk, 'AEGP_CollectionItemType_KEYFRAME'),
                "AEGP_CollectionItemType_KEYFRAME defined")


@suite.test
def test_new_collection():
    """Test AEGP_NewCollection - creates new collection"""
    plugin_id = ae.sdk.AEGP_GetPluginID()

    collectionH = ae.sdk.AEGP_NewCollection(plugin_id)
    assert_true(collectionH != 0, "Created collection handle")
    print(f"Collection handle: {collectionH}")

    # Clean up
    ae.sdk.AEGP_DisposeCollection(collectionH)
    print("Disposed collection")


@suite.test
def test_collection_num_items():
    """Test AEGP_GetCollectionNumItems - gets number of items in collection"""
    plugin_id = ae.sdk.AEGP_GetPluginID()
    collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

    try:
        # Initially should be empty
        num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
        assert_equal(num_items, 0, "Collection is initially empty")
        print(f"Collection has {num_items} items")

    finally:
        ae.sdk.AEGP_DisposeCollection(collectionH)


@suite.test
def test_collection_push_back_layer():
    """Test AEGP_CollectionPushBack - adds layer to collection"""
    global _test_layer1
    if not _test_layer1:
        print("No test layer - skipping")
        return

    plugin_id = ae.sdk.AEGP_GetPluginID()
    collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

    try:
        # Create a collection item for the layer
        item = {
            "type": ae.sdk.AEGP_CollectionItemType_LAYER,
            "layerH": _test_layer1._handle
        }

        # Add to collection
        ae.sdk.AEGP_CollectionPushBack(collectionH, item)

        # Verify it was added
        num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
        assert_equal(num_items, 1, "Collection has 1 item after push_back")
        print(f"Added layer to collection, now has {num_items} items")

    finally:
        ae.sdk.AEGP_DisposeCollection(collectionH)


@suite.test
def test_collection_get_item_by_index():
    """Test AEGP_GetCollectionItemByIndex - retrieves item from collection"""
    global _test_layer1
    if not _test_layer1:
        print("No test layer - skipping")
        return

    plugin_id = ae.sdk.AEGP_GetPluginID()
    collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

    try:
        # Add layer to collection
        item = {
            "type": ae.sdk.AEGP_CollectionItemType_LAYER,
            "layerH": _test_layer1._handle
        }
        ae.sdk.AEGP_CollectionPushBack(collectionH, item)

        # Get the item back
        retrieved_item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, 0)

        # Verify the type
        assert_equal(retrieved_item["type"], ae.sdk.AEGP_CollectionItemType_LAYER,
                    "Retrieved item is a layer")

        # Verify the layer handle matches
        assert_equal(retrieved_item["layerH"], _test_layer1._handle,
                    "Layer handle matches")

        print(f"Retrieved item type: {retrieved_item['type']}")
        print(f"Layer handle: {retrieved_item['layerH']}")

    finally:
        ae.sdk.AEGP_DisposeCollection(collectionH)


@suite.test
def test_collection_multiple_items():
    """Test adding multiple items to collection"""
    global _test_layer1, _test_layer2
    if not _test_layer1 or not _test_layer2:
        print("No test layers - skipping")
        return

    plugin_id = ae.sdk.AEGP_GetPluginID()
    collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

    try:
        # Add first layer
        item1 = {
            "type": ae.sdk.AEGP_CollectionItemType_LAYER,
            "layerH": _test_layer1._handle
        }
        ae.sdk.AEGP_CollectionPushBack(collectionH, item1)

        # Add second layer
        item2 = {
            "type": ae.sdk.AEGP_CollectionItemType_LAYER,
            "layerH": _test_layer2._handle
        }
        ae.sdk.AEGP_CollectionPushBack(collectionH, item2)

        # Verify count
        num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
        assert_equal(num_items, 2, "Collection has 2 items")

        # Verify items
        retrieved_item1 = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, 0)
        retrieved_item2 = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, 1)

        assert_equal(retrieved_item1["layerH"], _test_layer1._handle,
                    "First item matches layer1")
        assert_equal(retrieved_item2["layerH"], _test_layer2._handle,
                    "Second item matches layer2")

        print(f"Collection has {num_items} layers")

    finally:
        ae.sdk.AEGP_DisposeCollection(collectionH)


@suite.test
def test_collection_erase():
    """Test AEGP_CollectionErase - removes items from collection"""
    global _test_layer1, _test_layer2
    if not _test_layer1 or not _test_layer2:
        print("No test layers - skipping")
        return

    plugin_id = ae.sdk.AEGP_GetPluginID()
    collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

    try:
        # Add two layers
        item1 = {
            "type": ae.sdk.AEGP_CollectionItemType_LAYER,
            "layerH": _test_layer1._handle
        }
        item2 = {
            "type": ae.sdk.AEGP_CollectionItemType_LAYER,
            "layerH": _test_layer2._handle
        }
        ae.sdk.AEGP_CollectionPushBack(collectionH, item1)
        ae.sdk.AEGP_CollectionPushBack(collectionH, item2)

        # Verify we have 2 items
        num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
        assert_equal(num_items, 2, "Collection has 2 items before erase")

        # Erase first item (index 0 to 1 - exclusive range)
        # Adobe SDK: index_last appears to be exclusive (C++ iterator style)
        ae.sdk.AEGP_CollectionErase(collectionH, 0, 1)

        # Verify we now have 1 item
        num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
        assert_equal(num_items, 1, "Collection has 1 item after erase")

        # Verify the remaining item is layer2
        remaining_item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, 0)
        assert_equal(remaining_item["layerH"], _test_layer2._handle,
                    "Remaining item is layer2")

        print(f"Erased first item, collection now has {num_items} items")

    finally:
        ae.sdk.AEGP_DisposeCollection(collectionH)


@suite.test
def test_collection_with_mask():
    """Test collection with mask items"""
    global _test_layer1
    if not _test_layer1:
        print("No test layer - skipping")
        return

    # Add a mask to the layer
    _test_layer1.add_mask()

    plugin_id = ae.sdk.AEGP_GetPluginID()
    collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

    try:
        # Add mask to collection
        mask_item = {
            "type": ae.sdk.AEGP_CollectionItemType_MASK,
            "layerH": _test_layer1._handle,
            "mask_index": 0
        }
        ae.sdk.AEGP_CollectionPushBack(collectionH, mask_item)

        # Verify
        num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
        assert_equal(num_items, 1, "Collection has 1 mask item")

        retrieved_item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, 0)
        assert_equal(retrieved_item["type"], ae.sdk.AEGP_CollectionItemType_MASK,
                    "Item is a mask")
        assert_equal(retrieved_item["mask_index"], 0, "Mask index is 0")

        print(f"Added mask to collection")

    finally:
        ae.sdk.AEGP_DisposeCollection(collectionH)


# Run tests
def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
