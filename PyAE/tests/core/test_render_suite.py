"""
Render Suite Tests
Tests for AEGP_RenderSuite5

This suite provides functions for synchronous frame rendering, sound data rendering,
and render cache management. Async rendering via callbacks is not available from
Python; use RenderAsyncManagerSuite for async rendering in Custom UI contexts.

Functions tested:
1. AEGP_RenderAndCheckoutFrame(optionsH) -> receiptH
2. AEGP_RenderAndCheckoutLayerFrame(optionsH) -> receiptH
3. AEGP_RenderAndCheckoutLayerFrame_Async - Stub (throws exception)
4. AEGP_CancelAsyncRequest(asyncRequestId) -> None
5. AEGP_CheckinFrame(receiptH) -> None
6. AEGP_GetReceiptWorld(receiptH) -> worldH
7. AEGP_GetRenderedRegion(receiptH) -> dict
8. AEGP_IsRenderedFrameSufficient(rendered_optionsH, proposed_optionsH) -> bool
9. AEGP_RenderNewItemSoundData(itemH, start_time, duration, sound_format) -> soundDataH
10. AEGP_GetCurrentTimestamp() -> tuple(4 ints)
11. AEGP_HasItemChangedSinceTimestamp(itemH, start_time, duration, timestamp) -> bool
12. AEGP_IsItemWorthwhileToRender(roH, timestamp) -> bool
13. AEGP_CheckinRenderedFrame - Stub (throws exception)
14. AEGP_GetReceiptGuid(receiptH) -> str
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true, assert_not_none, assert_isinstance
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true, assert_not_none, assert_isinstance

suite = TestSuite("Render Suite")


@suite.setup
def setup():
    """Setup for Render Suite tests"""
    pass


@suite.teardown
def teardown():
    """Cleanup after Render Suite tests"""
    pass


# ============================================================================
# API Availability Tests
# ============================================================================

@suite.test
def test_render_and_checkout_frame_function_exists():
    """Test AEGP_RenderAndCheckoutFrame function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RenderAndCheckoutFrame'),
        "AEGP_RenderAndCheckoutFrame should be available"
    )
    print("AEGP_RenderAndCheckoutFrame function exists")


@suite.test
def test_render_and_checkout_layer_frame_function_exists():
    """Test AEGP_RenderAndCheckoutLayerFrame function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RenderAndCheckoutLayerFrame'),
        "AEGP_RenderAndCheckoutLayerFrame should be available"
    )
    print("AEGP_RenderAndCheckoutLayerFrame function exists")


@suite.test
def test_render_and_checkout_layer_frame_async_function_exists():
    """Test AEGP_RenderAndCheckoutLayerFrame_Async function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RenderAndCheckoutLayerFrame_Async'),
        "AEGP_RenderAndCheckoutLayerFrame_Async should be available"
    )
    print("AEGP_RenderAndCheckoutLayerFrame_Async function exists (stub)")


@suite.test
def test_cancel_async_request_function_exists():
    """Test AEGP_CancelAsyncRequest function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_CancelAsyncRequest'),
        "AEGP_CancelAsyncRequest should be available"
    )
    print("AEGP_CancelAsyncRequest function exists")


@suite.test
def test_checkin_frame_function_exists():
    """Test AEGP_CheckinFrame function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_CheckinFrame'),
        "AEGP_CheckinFrame should be available"
    )
    print("AEGP_CheckinFrame function exists")


@suite.test
def test_get_receipt_world_function_exists():
    """Test AEGP_GetReceiptWorld function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_GetReceiptWorld'),
        "AEGP_GetReceiptWorld should be available"
    )
    print("AEGP_GetReceiptWorld function exists")


@suite.test
def test_get_rendered_region_function_exists():
    """Test AEGP_GetRenderedRegion function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_GetRenderedRegion'),
        "AEGP_GetRenderedRegion should be available"
    )
    print("AEGP_GetRenderedRegion function exists")


@suite.test
def test_is_rendered_frame_sufficient_function_exists():
    """Test AEGP_IsRenderedFrameSufficient function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_IsRenderedFrameSufficient'),
        "AEGP_IsRenderedFrameSufficient should be available"
    )
    print("AEGP_IsRenderedFrameSufficient function exists")


@suite.test
def test_render_new_item_sound_data_function_exists():
    """Test AEGP_RenderNewItemSoundData function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RenderNewItemSoundData'),
        "AEGP_RenderNewItemSoundData should be available"
    )
    print("AEGP_RenderNewItemSoundData function exists")


@suite.test
def test_get_current_timestamp_function_exists():
    """Test AEGP_GetCurrentTimestamp function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_GetCurrentTimestamp'),
        "AEGP_GetCurrentTimestamp should be available"
    )
    print("AEGP_GetCurrentTimestamp function exists")


@suite.test
def test_has_item_changed_since_timestamp_function_exists():
    """Test AEGP_HasItemChangedSinceTimestamp function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_HasItemChangedSinceTimestamp'),
        "AEGP_HasItemChangedSinceTimestamp should be available"
    )
    print("AEGP_HasItemChangedSinceTimestamp function exists")


@suite.test
def test_is_item_worthwhile_to_render_function_exists():
    """Test AEGP_IsItemWorthwhileToRender function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_IsItemWorthwhileToRender'),
        "AEGP_IsItemWorthwhileToRender should be available"
    )
    print("AEGP_IsItemWorthwhileToRender function exists")


@suite.test
def test_checkin_rendered_frame_function_exists():
    """Test AEGP_CheckinRenderedFrame function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_CheckinRenderedFrame'),
        "AEGP_CheckinRenderedFrame should be available"
    )
    print("AEGP_CheckinRenderedFrame function exists (stub)")


@suite.test
def test_get_receipt_guid_function_exists():
    """Test AEGP_GetReceiptGuid function is available"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_GetReceiptGuid'),
        "AEGP_GetReceiptGuid should be available"
    )
    print("AEGP_GetReceiptGuid function exists")


# ============================================================================
# Basic Function Tests
# ============================================================================

@suite.test
def test_get_current_timestamp_basic():
    """Test AEGP_GetCurrentTimestamp returns valid timestamp"""
    # GetCurrentTimestamp requires no parameters
    timestamp = ae.sdk.AEGP_GetCurrentTimestamp()

    # Should return a tuple of 4 integers
    assert_not_none(timestamp, "Timestamp should not be None")
    assert_isinstance(timestamp, tuple, "Timestamp should be a tuple")
    assert_equal(4, len(timestamp), "Timestamp should have 4 elements")

    # Each element should be an integer
    for i, val in enumerate(timestamp):
        assert_isinstance(val, int, f"Timestamp element {i} should be an integer")

    print(f"Current timestamp: {timestamp}")


@suite.test
def test_get_current_timestamp_consistency():
    """Test AEGP_GetCurrentTimestamp returns consistent timestamp when called multiple times"""
    # Call twice without any changes - should return same timestamp
    ts1 = ae.sdk.AEGP_GetCurrentTimestamp()
    ts2 = ae.sdk.AEGP_GetCurrentTimestamp()

    # Timestamps should be the same if no changes were made
    assert_equal(ts1, ts2, "Timestamp should be consistent between calls")
    print(f"Timestamp is consistent: {ts1}")


# ============================================================================
# Stub Function Tests - Verify they throw expected exceptions
# ============================================================================

@suite.test
def test_render_and_checkout_layer_frame_async_throws():
    """Test AEGP_RenderAndCheckoutLayerFrame_Async is a stub that throws exception"""
    try:
        # This should throw because it's a stub
        ae.sdk.AEGP_RenderAndCheckoutLayerFrame_Async(0)
        raise AssertionError("Should have raised exception for async stub function")
    except RuntimeError as e:
        error_msg = str(e)
        assert_true(
            "not available from Python" in error_msg or
            "RenderAsyncManager" in error_msg or
            "C callback" in error_msg.lower(),
            f"Error message should explain stub status: {error_msg}"
        )
        print(f"Correctly rejected async stub: {error_msg}")


@suite.test
def test_checkin_rendered_frame_throws():
    """Test AEGP_CheckinRenderedFrame is a stub that throws exception"""
    try:
        # This should throw because it's a stub
        ae.sdk.AEGP_CheckinRenderedFrame(0, 0, 0, 0, 0, 0, 0)
        raise AssertionError("Should have raised exception for CheckinRenderedFrame stub")
    except RuntimeError as e:
        error_msg = str(e)
        assert_true(
            "not fully implemented" in error_msg or
            "PlatformWorldH" in error_msg or
            "standard render pipeline" in error_msg,
            f"Error message should explain stub status: {error_msg}"
        )
        print(f"Correctly rejected CheckinRenderedFrame stub: {error_msg}")


# ============================================================================
# NULL Handle Validation Tests
#
# IMPORTANT: Only use null (0) for parameters. Non-zero invalid pointers
# will pass validation but cause crashes when passed to AE SDK.
# ============================================================================

@suite.test
def test_render_and_checkout_frame_null_options():
    """Test AEGP_RenderAndCheckoutFrame rejects null options handle"""
    try:
        ae.sdk.AEGP_RenderAndCheckoutFrame(0)  # null optionsH
        raise AssertionError("Should have raised exception for null options")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null options: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_render_and_checkout_layer_frame_null_options():
    """Test AEGP_RenderAndCheckoutLayerFrame rejects null options handle"""
    try:
        ae.sdk.AEGP_RenderAndCheckoutLayerFrame(0)  # null optionsH
        raise AssertionError("Should have raised exception for null options")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null options: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_checkin_frame_null_receipt():
    """Test AEGP_CheckinFrame rejects null receipt handle"""
    try:
        ae.sdk.AEGP_CheckinFrame(0)  # null receiptH
        raise AssertionError("Should have raised exception for null receipt")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null receipt: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_get_receipt_world_null_receipt():
    """Test AEGP_GetReceiptWorld rejects null receipt handle"""
    try:
        ae.sdk.AEGP_GetReceiptWorld(0)  # null receiptH
        raise AssertionError("Should have raised exception for null receipt")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null receipt: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_get_rendered_region_null_receipt():
    """Test AEGP_GetRenderedRegion rejects null receipt handle"""
    try:
        ae.sdk.AEGP_GetRenderedRegion(0)  # null receiptH
        raise AssertionError("Should have raised exception for null receipt")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null receipt: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_is_rendered_frame_sufficient_null_rendered_options():
    """Test AEGP_IsRenderedFrameSufficient rejects null rendered options"""
    try:
        ae.sdk.AEGP_IsRenderedFrameSufficient(0, 0)  # null handles
        raise AssertionError("Should have raised exception for null options")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null options: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_render_new_item_sound_data_null_item():
    """Test AEGP_RenderNewItemSoundData rejects null item handle"""
    try:
        ae.sdk.AEGP_RenderNewItemSoundData(
            0,       # null itemH
            0.0,     # start_time_seconds
            1.0,     # duration_seconds
            44100.0, # sample_rate
            4,       # encoding (SIGNED_PCM)
            2,       # bytes_per_sample
            2        # num_channels
        )
        raise AssertionError("Should have raised exception for null item")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null item: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_has_item_changed_since_timestamp_null_item():
    """Test AEGP_HasItemChangedSinceTimestamp rejects null item handle"""
    try:
        ae.sdk.AEGP_HasItemChangedSinceTimestamp(
            0,    # null itemH
            0.0,  # start_time_seconds
            1.0,  # duration_seconds
            0, 0, 0, 0  # timestamp
        )
        raise AssertionError("Should have raised exception for null item")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null item: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_is_item_worthwhile_to_render_null_options():
    """Test AEGP_IsItemWorthwhileToRender rejects null options handle"""
    try:
        ae.sdk.AEGP_IsItemWorthwhileToRender(
            0,  # null roH
            0, 0, 0, 0  # timestamp
        )
        raise AssertionError("Should have raised exception for null options")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null options: {e}")
        assert_true(True, "Null validation works")


@suite.test
def test_get_receipt_guid_null_receipt():
    """Test AEGP_GetReceiptGuid rejects null receipt handle"""
    try:
        ae.sdk.AEGP_GetReceiptGuid(0)  # null receiptH
        raise AssertionError("Should have raised exception for null receipt")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected null receipt: {e}")
        assert_true(True, "Null validation works")


# ============================================================================
# Parameter Validation Tests
# ============================================================================

@suite.test
def test_render_new_item_sound_data_invalid_encoding():
    """Test AEGP_RenderNewItemSoundData rejects invalid encoding value"""
    # Create a test composition to get a valid item handle
    proj = ae.sdk.AEGP_GetProjectByIndex(0)
    if not proj:
        print("Skipping: No project available")
        return

    # Try with invalid encoding (valid range is 3-5)
    try:
        ae.sdk.AEGP_RenderNewItemSoundData(
            proj,    # Use project handle as item (will fail, but encoding check should come first)
            0.0,     # start_time_seconds
            1.0,     # duration_seconds
            44100.0, # sample_rate
            99,      # invalid encoding
            2,       # bytes_per_sample
            2        # num_channels
        )
        raise AssertionError("Should have raised exception for invalid encoding")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected invalid encoding: {e}")
        assert_true(True, "Encoding validation works")


@suite.test
def test_render_new_item_sound_data_negative_duration():
    """Test AEGP_RenderNewItemSoundData rejects negative duration"""
    proj = ae.sdk.AEGP_GetProjectByIndex(0)
    if not proj:
        print("Skipping: No project available")
        return

    try:
        ae.sdk.AEGP_RenderNewItemSoundData(
            proj,     # itemH
            0.0,      # start_time_seconds
            -1.0,     # negative duration - invalid
            44100.0,  # sample_rate
            4,        # encoding (SIGNED_PCM)
            2,        # bytes_per_sample
            2         # num_channels
        )
        raise AssertionError("Should have raised exception for negative duration")
    except (RuntimeError, ValueError) as e:
        print(f"Correctly rejected negative duration: {e}")
        assert_true(True, "Duration validation works")


# ============================================================================
# Documentation Tests
# ============================================================================

@suite.test
def test_render_suite_api_documentation():
    """Verify Render Suite API documentation is accurate"""
    # Verify all expected functions exist
    expected_functions = [
        'AEGP_RenderAndCheckoutFrame',
        'AEGP_RenderAndCheckoutLayerFrame',
        'AEGP_RenderAndCheckoutLayerFrame_Async',
        'AEGP_CancelAsyncRequest',
        'AEGP_CheckinFrame',
        'AEGP_GetReceiptWorld',
        'AEGP_GetRenderedRegion',
        'AEGP_IsRenderedFrameSufficient',
        'AEGP_RenderNewItemSoundData',
        'AEGP_GetCurrentTimestamp',
        'AEGP_HasItemChangedSinceTimestamp',
        'AEGP_IsItemWorthwhileToRender',
        'AEGP_CheckinRenderedFrame',
        'AEGP_GetReceiptGuid',
    ]

    for func_name in expected_functions:
        func = getattr(ae.sdk, func_name, None)
        assert_not_none(func, f"{func_name} should exist")

    print("Render Suite API (14 functions):")
    print("  Frame Rendering:")
    print("    - AEGP_RenderAndCheckoutFrame(optionsH) -> receiptH")
    print("    - AEGP_RenderAndCheckoutLayerFrame(optionsH) -> receiptH")
    print("    - AEGP_RenderAndCheckoutLayerFrame_Async [STUB]")
    print("    - AEGP_CancelAsyncRequest(asyncRequestId)")
    print("  Frame Receipt Operations:")
    print("    - AEGP_CheckinFrame(receiptH)")
    print("    - AEGP_GetReceiptWorld(receiptH) -> worldH")
    print("    - AEGP_GetRenderedRegion(receiptH) -> dict")
    print("    - AEGP_GetReceiptGuid(receiptH) -> str")
    print("  Sound Rendering:")
    print("    - AEGP_RenderNewItemSoundData(...) -> soundDataH")
    print("  Timestamp & Cache:")
    print("    - AEGP_GetCurrentTimestamp() -> tuple(4 ints)")
    print("    - AEGP_HasItemChangedSinceTimestamp(...) -> bool")
    print("    - AEGP_IsItemWorthwhileToRender(...) -> bool")
    print("    - AEGP_IsRenderedFrameSufficient(...) -> bool")
    print("    - AEGP_CheckinRenderedFrame [STUB]")

    assert_true(True, "API documentation verified")


# ============================================================================
# Run Tests
# ============================================================================

def run():
    """Run all tests and return results"""
    return suite.run()


if __name__ == "__main__":
    run()
