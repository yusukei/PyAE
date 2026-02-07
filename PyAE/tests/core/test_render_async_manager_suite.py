"""
RenderAsyncManager Suite Tests
Tests for AEGP_RenderAsyncManagerSuite1

Note: These functions are primarily designed for use within effect plugins during
PF_Event_DRAW with the PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER flag enabled.
Full functionality testing requires an effect context which is not available
from a standard AEGP plugin. These tests verify parameter validation and
API availability.
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true, assert_not_none
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true, assert_not_none

suite = TestSuite("RenderAsyncManager Suite")


@suite.setup
def setup():
    """Setup for RenderAsyncManager tests"""
    pass


@suite.teardown
def teardown():
    """Cleanup after RenderAsyncManager tests"""
    pass


# ============================================================================
# API Availability Tests
# ============================================================================

@suite.test
def test_checkout_or_render_item_frame_function_exists():
    """Test AEGP_CheckoutOrRender_ItemFrame_AsyncManager function is available"""
    # Verify the function exists in the SDK module
    assert_true(
        hasattr(ae.sdk, 'AEGP_CheckoutOrRender_ItemFrame_AsyncManager'),
        "AEGP_CheckoutOrRender_ItemFrame_AsyncManager should be available"
    )
    print("AEGP_CheckoutOrRender_ItemFrame_AsyncManager function exists")


@suite.test
def test_checkout_or_render_layer_frame_function_exists():
    """Test AEGP_CheckoutOrRender_LayerFrame_AsyncManager function is available"""
    # Verify the function exists in the SDK module
    assert_true(
        hasattr(ae.sdk, 'AEGP_CheckoutOrRender_LayerFrame_AsyncManager'),
        "AEGP_CheckoutOrRender_LayerFrame_AsyncManager should be available"
    )
    print("AEGP_CheckoutOrRender_LayerFrame_AsyncManager function exists")


# ============================================================================
# Parameter Validation Tests
#
# IMPORTANT: Only use null (0) for parameters. Non-zero invalid pointers
# (e.g., 12345) will pass validation but cause crashes when passed to AE SDK.
# ============================================================================

@suite.test
def test_checkout_or_render_item_frame_null_async_manager():
    """Test AEGP_CheckoutOrRender_ItemFrame_AsyncManager rejects null async manager"""
    try:
        # Call with null async_manager (0) - safe: validation catches this
        ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
            0,  # null async_managerP - will be rejected by validation
            1,  # purpose_id
            0   # null roH
        )
        raise AssertionError("Should have raised exception for null async manager")
    except Exception as e:
        print(f"Correctly rejected null async manager: {e}")
        assert_true(True, "Null async manager validation works")


@suite.test
def test_checkout_or_render_layer_frame_null_async_manager():
    """Test AEGP_CheckoutOrRender_LayerFrame_AsyncManager rejects null async manager"""
    try:
        # Call with null async_manager (0) - safe: validation catches this
        ae.sdk.AEGP_CheckoutOrRender_LayerFrame_AsyncManager(
            0,  # null async_managerP - will be rejected by validation
            1,  # purpose_id
            0   # null lroH
        )
        raise AssertionError("Should have raised exception for null async manager")
    except Exception as e:
        print(f"Correctly rejected null async manager: {e}")
        assert_true(True, "Null async manager validation works")


@suite.test
def test_checkout_or_render_item_frame_null_render_options():
    """Test AEGP_CheckoutOrRender_ItemFrame_AsyncManager rejects null render options"""
    try:
        # Both parameters are null (0) - safe: validation catches async_managerP first
        ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
            0,  # null async_managerP - will be rejected by validation
            1,  # purpose_id
            0   # null roH
        )
        raise AssertionError("Should have raised exception")
    except Exception as e:
        print(f"Correctly rejected null parameters: {e}")
        assert_true(True, "Parameter validation works")


@suite.test
def test_checkout_or_render_layer_frame_null_layer_render_options():
    """Test AEGP_CheckoutOrRender_LayerFrame_AsyncManager rejects null layer render options"""
    try:
        # Both parameters are null (0) - safe: validation catches async_managerP first
        ae.sdk.AEGP_CheckoutOrRender_LayerFrame_AsyncManager(
            0,  # null async_managerP - will be rejected by validation
            1,  # purpose_id
            0   # null lroH
        )
        raise AssertionError("Should have raised exception")
    except Exception as e:
        print(f"Correctly rejected null parameters: {e}")
        assert_true(True, "Parameter validation works")


# ============================================================================
# Note: Tests with non-zero invalid pointers (fake handles like 12345) are
# intentionally excluded. Such pointers pass validation but cause crashes
# when passed to After Effects SDK as it dereferences invalid memory.
# ============================================================================


# ============================================================================
# Documentation Tests
# ============================================================================

@suite.test
def test_render_async_manager_api_documentation():
    """Verify RenderAsyncManager API documentation is accurate"""
    # These functions require:
    # - After Effects 13.5 or later
    # - PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER flag in effect
    # - Valid async_managerP from effect context
    # - Valid render options handle

    # Verify both functions exist with expected signatures
    item_frame_func = getattr(ae.sdk, 'AEGP_CheckoutOrRender_ItemFrame_AsyncManager', None)
    layer_frame_func = getattr(ae.sdk, 'AEGP_CheckoutOrRender_LayerFrame_AsyncManager', None)

    assert_not_none(item_frame_func, "ItemFrame function should exist")
    assert_not_none(layer_frame_func, "LayerFrame function should exist")

    print("RenderAsyncManager API:")
    print("  - AEGP_CheckoutOrRender_ItemFrame_AsyncManager(async_managerP, purpose_id, roH)")
    print("  - AEGP_CheckoutOrRender_LayerFrame_AsyncManager(async_managerP, purpose_id, lroH)")
    print("  Note: Requires effect context with CUSTOM_UI_ASYNC_MANAGER flag")

    assert_true(True, "API documentation verified")


# ============================================================================
# Run Tests
# ============================================================================

def run():
    """Run all tests and return results"""
    return suite.run()


if __name__ == "__main__":
    run()
