"""
RenderQueue Suite Tests
Tests for AEGP_RenderQueueSuite1

RenderQueue Suiteはレンダーキューの状態管理とコンポジションの追加機能を提供します。
"""
import ae
import os

try:
    from test_utils import TestSuite, assert_equal, assert_true
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true

suite = TestSuite("RenderQueue Suite")

# Test variables
_test_comp = None


@suite.setup
def setup():
    """Setup test composition"""
    global _test_comp
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_RQTestComp", 1920, 1080, 1.0, 2.0, 30.0)
    # Add a simple layer
    _test_comp.add_solid("_TestSolid", 100, 100, (1.0, 0.0, 0.0))


@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp
    if _test_comp is not None:
        _test_comp.delete()
        _test_comp = None


# ============================================================================
# RenderQueue Suite Tests
# ============================================================================

@suite.test
def test_renderqueue_constants():
    """Test that all RenderQueue constants are defined"""
    # RenderQueue State constants
    assert_true(hasattr(ae.sdk, 'AEGP_RenderQueueState_STOPPED'),
                "AEGP_RenderQueueState_STOPPED defined")
    assert_true(hasattr(ae.sdk, 'AEGP_RenderQueueState_PAUSED'),
                "AEGP_RenderQueueState_PAUSED defined")
    assert_true(hasattr(ae.sdk, 'AEGP_RenderQueueState_RENDERING'),
                "AEGP_RenderQueueState_RENDERING defined")


@suite.test
def test_get_render_queue_state():
    """Test AEGP_GetRenderQueueState - gets current render queue state"""
    state = ae.sdk.AEGP_GetRenderQueueState()
    print(f"Render queue state: {state}")

    # Should be one of the valid states
    valid_states = [
        ae.sdk.AEGP_RenderQueueState_STOPPED,
        ae.sdk.AEGP_RenderQueueState_PAUSED,
        ae.sdk.AEGP_RenderQueueState_RENDERING
    ]
    assert_true(state in valid_states, f"State {state} is valid")

    # Initially should be STOPPED
    assert_equal(state, ae.sdk.AEGP_RenderQueueState_STOPPED,
                "Render queue is initially stopped")


@suite.test
def test_add_comp_to_render_queue():
    """Test AEGP_AddCompToRenderQueue - adds composition to render queue"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    # Create a temporary output path
    import tempfile
    output_dir = tempfile.gettempdir()
    output_path = os.path.join(output_dir, "_test_render_output.mov")

    try:
        # Add comp to render queue
        ae.sdk.AEGP_AddCompToRenderQueue(_test_comp._handle, output_path)
        print(f"Added composition to render queue with output: {output_path}")

        # Verify via high-level API
        # Note: render_queue is accessed via ae.render_queue module, not Project property
        num_items = ae.render_queue.num_items()
        assert_true(num_items > 0, "Render queue has at least one item")
        print(f"Render queue has {num_items} items")

        # Clean up - remove the render queue item
        # (Will be cleaned up automatically by teardown)

    except Exception as e:
        print(f"Error adding to render queue: {e}")
        raise


# Run tests
def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
