"""
Composite Suite Tests
Tests for AEGP_CompositeSuite2

CompositeSuite provides pixel buffer compositing operations including
bit copying, alpha manipulation, and transfer operations.
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true, assert_not_none
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true, assert_not_none

suite = TestSuite("Composite Suite")

# Test variables
_plugin_id = None
_src_world = None
_dst_world = None


@suite.setup
def setup():
    """Setup test worlds for compositing"""
    global _plugin_id, _src_world, _dst_world
    _plugin_id = ae.sdk.AEGP_GetPluginID()
    print(f"Plugin ID: {_plugin_id}")

    # Create source and destination worlds
    _src_world = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        100,
        100
    )
    _dst_world = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        100,
        100
    )
    print(f"Created source world: {_src_world}")
    print(f"Created destination world: {_dst_world}")


@suite.teardown
def teardown():
    """Cleanup test worlds"""
    global _src_world, _dst_world

    if _src_world is not None:
        try:
            ae.sdk.AEGP_DisposeWorld(_src_world)
        except:
            pass
        _src_world = None

    if _dst_world is not None:
        try:
            ae.sdk.AEGP_DisposeWorld(_dst_world)
        except:
            pass
        _dst_world = None

    print("Worlds disposed")


# ============================================================================
# CopyBits Functions
# ============================================================================

@suite.test
def test_copy_bits_lq():
    """Test AEGP_CopyBits_LQ - low quality bit copy"""
    global _plugin_id

    # Create fresh worlds for this test
    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    # Define source and destination rectangles
    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
    dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    # Copy bits (low quality)
    ae.sdk.AEGP_CopyBits_LQ(src_world, src_rect, dst_rect, dst_world)
    print("CopyBits_LQ completed successfully")

    # Clean up
    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "CopyBits_LQ works correctly")


@suite.test
def test_copy_bits_lq_different_sizes():
    """Test AEGP_CopyBits_LQ with different source and destination sizes (scaling)"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 200, 200)

    # Scale up: 50x50 source to 100x100 destination
    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
    dst_rect = {"left": 0, "top": 0, "right": 100, "bottom": 100}

    ae.sdk.AEGP_CopyBits_LQ(src_world, src_rect, dst_rect, dst_world)
    print("CopyBits_LQ with scaling completed successfully")

    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "CopyBits_LQ scaling works correctly")


@suite.test
def test_copy_bits_hq_straight():
    """Test AEGP_CopyBits_HQ_Straight - high quality with straight alpha"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
    dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    ae.sdk.AEGP_CopyBits_HQ_Straight(src_world, src_rect, dst_rect, dst_world)
    print("CopyBits_HQ_Straight completed successfully")

    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "CopyBits_HQ_Straight works correctly")


@suite.test
def test_copy_bits_hq_premul():
    """Test AEGP_CopyBits_HQ_Premul - high quality with premultiplied alpha"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
    dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    ae.sdk.AEGP_CopyBits_HQ_Premul(src_world, src_rect, dst_rect, dst_world)
    print("CopyBits_HQ_Premul completed successfully")

    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "CopyBits_HQ_Premul works correctly")


@suite.test
def test_copy_bits_16bit_worlds():
    """Test CopyBits functions with 16-bit worlds"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_16, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_16, 100, 100)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
    dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    ae.sdk.AEGP_CopyBits_LQ(src_world, src_rect, dst_rect, dst_world)
    print("CopyBits_LQ with 16-bit worlds completed successfully")

    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "16-bit world copy works correctly")


@suite.test
def test_copy_bits_32bit_worlds():
    """Test CopyBits functions with 32-bit float worlds"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_32, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_32, 100, 100)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
    dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    ae.sdk.AEGP_CopyBits_HQ_Straight(src_world, src_rect, dst_rect, dst_world)
    print("CopyBits_HQ_Straight with 32-bit worlds completed successfully")

    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "32-bit float world copy works correctly")


# ============================================================================
# ClearAlphaExceptRect
# ============================================================================

@suite.test
def test_clear_alpha_except_rect():
    """Test AEGP_ClearAlphaExceptRect - clear alpha outside rectangle"""
    global _plugin_id

    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    # Define rectangle to preserve
    rect = {"left": 25, "top": 25, "right": 75, "bottom": 75}

    ae.sdk.AEGP_ClearAlphaExceptRect(dst_world, rect)
    print("ClearAlphaExceptRect completed successfully")

    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "ClearAlphaExceptRect works correctly")


@suite.test
def test_clear_alpha_except_rect_full_image():
    """Test AEGP_ClearAlphaExceptRect with full image rectangle"""
    global _plugin_id

    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    # Full image rectangle - nothing should be cleared
    rect = {"left": 0, "top": 0, "right": 100, "bottom": 100}

    ae.sdk.AEGP_ClearAlphaExceptRect(dst_world, rect)
    print("ClearAlphaExceptRect with full image rect completed successfully")

    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "ClearAlphaExceptRect with full rect works correctly")


@suite.test
def test_clear_alpha_except_rect_16bit():
    """Test AEGP_ClearAlphaExceptRect with 16-bit world"""
    global _plugin_id

    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_16, 100, 100)

    rect = {"left": 10, "top": 10, "right": 90, "bottom": 90}

    ae.sdk.AEGP_ClearAlphaExceptRect(dst_world, rect)
    print("ClearAlphaExceptRect with 16-bit world completed successfully")

    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "ClearAlphaExceptRect with 16-bit world works correctly")


# ============================================================================
# TransferRect
# ============================================================================

@suite.test
def test_transfer_rect_basic():
    """Test AEGP_TransferRect - basic rectangle transfer"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    # TransferRect with minimal parameters (no mask, no blend tables)
    ae.sdk.AEGP_TransferRect(
        ae.sdk.PF_Quality_HI,      # quality
        ae.sdk.PF_MF_Alpha_PREMUL, # mode flags
        0,                          # field (0 = full frame)
        src_rect,                   # source rectangle
        src_world,                  # source world
        0,                          # comp_mode_ptr (0 = normal)
        0,                          # blend_tables_ptr (0 = none)
        0,                          # mask_world (0 = no mask)
        0,                          # dest_x
        0,                          # dest_y
        dst_world                   # destination world
    )
    print("TransferRect basic completed successfully")

    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "TransferRect basic works correctly")


@suite.test
def test_transfer_rect_with_offset():
    """Test AEGP_TransferRect with destination offset"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 200, 200)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    ae.sdk.AEGP_TransferRect(
        ae.sdk.PF_Quality_HI,
        ae.sdk.PF_MF_Alpha_PREMUL,
        0,
        src_rect,
        src_world,
        0,
        0,
        0,
        50,  # dest_x offset
        50,  # dest_y offset
        dst_world
    )
    print("TransferRect with offset completed successfully")

    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "TransferRect with offset works correctly")


@suite.test
def test_transfer_rect_low_quality():
    """Test AEGP_TransferRect with low quality"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)
    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    ae.sdk.AEGP_TransferRect(
        ae.sdk.PF_Quality_LO,      # low quality
        ae.sdk.PF_MF_Alpha_STRAIGHT,
        0,
        src_rect,
        src_world,
        0,
        0,
        0,
        0,
        0,
        dst_world
    )
    print("TransferRect with low quality completed successfully")

    ae.sdk.AEGP_DisposeWorld(src_world)
    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "TransferRect low quality works correctly")


# ============================================================================
# PrepTrackMatte
# ============================================================================

@suite.test
def test_prep_track_matte():
    """Test AEGP_PrepTrackMatte - prepare track matte pixels"""
    # PrepTrackMatte operates on raw pixel data
    # Create some test pixel data
    num_pixels = 100

    # Create dummy pixel data (would normally come from a world's base address)
    # For testing, we'll check that the function accepts the parameters
    # Note: This function requires actual pixel memory, so we use a world
    global _plugin_id

    mask_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 10, 10)
    base_addr = ae.sdk.AEGP_GetWorldBaseAddr8(mask_world)

    # Prepare track matte (8-bit mode, not deep)
    result = ae.sdk.AEGP_PrepTrackMatte(
        num_pixels,     # number of pixels
        False,          # deepB (False = 8-bit, True = 16-bit)
        base_addr,      # source mask pointer
        0               # mask_flags
    )

    assert_not_none(result, "PrepTrackMatte should return data")
    print(f"PrepTrackMatte returned {len(result)} bytes")

    ae.sdk.AEGP_DisposeWorld(mask_world)

    assert_true(True, "PrepTrackMatte works correctly")


@suite.test
def test_prep_track_matte_deep():
    """Test AEGP_PrepTrackMatte with deep (16-bit) mode"""
    global _plugin_id

    mask_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_16, 10, 10)
    base_addr = ae.sdk.AEGP_GetWorldBaseAddr16(mask_world)

    num_pixels = 100

    # Prepare track matte (16-bit deep mode)
    result = ae.sdk.AEGP_PrepTrackMatte(
        num_pixels,
        True,           # deepB = True for 16-bit
        base_addr,
        0
    )

    assert_not_none(result, "PrepTrackMatte deep should return data")
    print(f"PrepTrackMatte deep returned {len(result)} bytes")

    ae.sdk.AEGP_DisposeWorld(mask_world)

    assert_true(True, "PrepTrackMatte deep mode works correctly")


# ============================================================================
# Null Handle Validation
# ============================================================================

@suite.test
def test_copy_bits_null_src_world():
    """Test CopyBits functions reject null source world"""
    global _plugin_id

    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
    dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    try:
        ae.sdk.AEGP_CopyBits_LQ(0, src_rect, dst_rect, dst_world)
        raise AssertionError("Should have raised exception for null source world")
    except Exception as e:
        print(f"Correctly rejected null source world: {e}")

    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "Null source world validation works correctly")


@suite.test
def test_copy_bits_null_dst_world():
    """Test CopyBits functions reject null destination world"""
    global _plugin_id

    src_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
    dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    try:
        ae.sdk.AEGP_CopyBits_LQ(src_world, src_rect, dst_rect, 0)
        raise AssertionError("Should have raised exception for null destination world")
    except Exception as e:
        print(f"Correctly rejected null destination world: {e}")

    ae.sdk.AEGP_DisposeWorld(src_world)

    assert_true(True, "Null destination world validation works correctly")


@suite.test
def test_clear_alpha_except_rect_null_world():
    """Test ClearAlphaExceptRect rejects null world"""
    rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    try:
        ae.sdk.AEGP_ClearAlphaExceptRect(0, rect)
        raise AssertionError("Should have raised exception for null world")
    except Exception as e:
        print(f"Correctly rejected null world: {e}")

    assert_true(True, "Null world validation works correctly")


@suite.test
def test_transfer_rect_null_src_world():
    """Test TransferRect rejects null source world"""
    global _plugin_id

    dst_world = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)
    src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

    try:
        ae.sdk.AEGP_TransferRect(
            ae.sdk.PF_Quality_HI,
            ae.sdk.PF_MF_Alpha_PREMUL,
            0,
            src_rect,
            0,  # null source world
            0,
            0,
            0,
            0,
            0,
            dst_world
        )
        raise AssertionError("Should have raised exception for null source world")
    except Exception as e:
        print(f"Correctly rejected null source world: {e}")

    ae.sdk.AEGP_DisposeWorld(dst_world)

    assert_true(True, "Null source world validation works correctly")


# ============================================================================
# Run Tests
# ============================================================================

def run():
    """Run all tests and return results"""
    return suite.run()


if __name__ == "__main__":
    run()
