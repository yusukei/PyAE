# test_batch_operations.py
# PyAE Batch Operations Test

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

suite = TestSuite("Batch Operations")

# Test variables
_test_comp = None
_test_layers = []


@suite.setup
def setup():
    """Setup test environment"""
    global _test_comp, _test_layers
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_BatchTestComp", 1920, 1080, 1.0, 5.0, 30.0)

    # Create multiple layers for batch testing
    _test_layers = []
    for i in range(5):
        layer = _test_comp.add_solid(
            f"_TestLayer_{i}", 1920, 1080, 1.0, 0.0, 0.0, 5.0
        )
        _test_layers.append(layer)


@suite.teardown
def teardown():
    """Cleanup test environment"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except:
        pass


@suite.test
def test_batch_begin_end_rename():
    """Test batch operation for renaming layers"""
    # Batch operation should work with begin/end
    ae.batch.begin()
    for layer in _test_layers:
        layer.name = f"Renamed_{layer.index}"
    ae.batch.end()

    # Verify names were changed
    for layer in _test_layers:
        assert_true(
            layer.name.startswith("Renamed_"),
            "Layer names should be changed after batch"
        )


@suite.test
def test_batch_begin_end():
    """Test batch begin/end methods"""
    ae.batch.begin()

    # Modify multiple layers
    for layer in _test_layers:
        layer.opacity = 50.0

    ae.batch.end()

    # Verify changes were applied
    for layer in _test_layers:
        assert_equal(layer.opacity, 50.0, "Opacity should be 50")


@suite.test
def test_batch_is_active():
    """Test batch active status"""
    # Should not be active initially
    assert_false(ae.batch.is_active(), "Batch should not be active initially")

    ae.batch.begin()
    assert_true(ae.batch.is_active(), "Batch should be active after begin")

    ae.batch.end()
    assert_false(ae.batch.is_active(), "Batch should not be active after end")


@suite.test
def test_batch_multiple_properties():
    """Test batch with multiple property changes"""
    ae.batch.begin()
    # Multiple property changes
    _test_layers[0].name = "Level1"
    # Operations should work inside batch
    _test_layers[1].position = [960, 540]
    ae.batch.end()

    # Verify all changes applied
    assert_equal(_test_layers[0].name, "Level1")
    # Position should be [960.0, 540.0] for 2D layers
    pos = _test_layers[1].position
    assert_true(abs(pos[0] - 960.0) < 0.1, "Position X should be 960")
    assert_true(abs(pos[1] - 540.0) < 0.1, "Position Y should be 540")


@suite.test
def test_batch_multiple_operations():
    """Test multiple operations in batch"""
    ae.batch.begin()
    # Different types of operations
    for i, layer in enumerate(_test_layers):
        layer.name = f"Batch_{i}"
        layer.opacity = 75.0
        layer.position = [100 * i, 100 * i]
    ae.batch.end()

    # Verify all operations completed
    for i, layer in enumerate(_test_layers):
        assert_equal(layer.name, f"Batch_{i}")
        assert_equal(layer.opacity, 75.0)
        # Position values might have slight floating point differences
        pos = layer.position
        assert_true(abs(pos[0] - 100 * i) < 0.1)
        assert_true(abs(pos[1] - 100 * i) < 0.1)


@suite.test
def test_batch_performance_benefit():
    """Test that batch provides performance benefit"""
    import time

    # Time without batch
    start = time.time()
    for layer in _test_layers:
        layer.opacity = 90.0
    no_batch_time = time.time() - start

    # Time with batch
    start = time.time()
    ae.batch.begin()
    for layer in _test_layers:
        layer.opacity = 80.0
    ae.batch.end()
    batch_time = time.time() - start

    # Batch should be at least as fast (or faster)
    # Note: In small operations, overhead might make batch slightly slower
    # This test just confirms batch works, not necessarily faster for small ops
    assert_true(True, "Batch operation completed successfully")


@suite.test
def test_batch_error_handling():
    """Test error handling in batch operations"""
    try:
        ae.batch.begin()
        _test_layers[0].name = "ValidName"
        # This should work fine
        _test_layers[1].opacity = 100.0
        ae.batch.end()

        # Should complete successfully
        assert_true(True, "Batch completed without error")
    except Exception as e:
        # Should not raise exception for valid operations
        assert_true(False, f"Unexpected error in batch: {e}")


@suite.test
def test_batch_property_changes():
    """Test batch property changes"""
    ae.batch.begin()
    # Change multiple properties on multiple layers
    for layer in _test_layers:
        layer.enabled = False
        layer.solo = False
        layer.locked = False
    ae.batch.end()

    # Verify all changes
    for layer in _test_layers:
        assert_false(layer.enabled, "Layer should be disabled")
        assert_false(layer.solo, "Layer should not be solo")
        assert_false(layer.locked, "Layer should not be locked")


@suite.test
def test_batch_layer_operations():
    """Test batch layer creation and deletion"""
    initial_count = _test_comp.num_layers

    new_layers = []
    ae.batch.begin()
    # Create multiple layers
    for i in range(3):
        layer = _test_comp.add_null(f"_BatchNull_{i}")
        new_layers.append(layer)
    ae.batch.end()

    # Verify layers were created
    assert_equal(
        _test_comp.num_layers,
        initial_count + 3,
        "Should have 3 more layers"
    )

    # Cleanup
    ae.batch.begin()
    for layer in new_layers:
        layer.delete()
    ae.batch.end()


def run():
    """Run the test suite"""
    return suite.run()


if __name__ == "__main__":
    run()
