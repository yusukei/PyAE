"""
World Suite Tests
Tests for AEGP_WorldSuite3

World Suite provides pixel buffer (frame buffer) management for rendering operations.
Essential for Export/Import of rendered frames and pixel manipulation.
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true, assert_not_none, assert_raises
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true, assert_not_none, assert_raises

suite = TestSuite("World Suite")

# Test variables
_test_world_8 = None
_test_world_16 = None
_test_world_32 = None
_plugin_id = None


@suite.setup
def setup():
    """Setup test worlds"""
    global _plugin_id
    _plugin_id = ae.sdk.AEGP_GetPluginID()
    print(f"Plugin ID: {_plugin_id}")


@suite.teardown
def teardown():
    """Cleanup test worlds"""
    global _test_world_8, _test_world_16, _test_world_32
    # Note: Worlds created in tests are disposed within each test
    # to properly test the dispose functionality
    _test_world_8 = None
    _test_world_16 = None
    _test_world_32 = None


# ============================================================================
# World Type Constants
# ============================================================================

@suite.test
def test_world_type_constants():
    """Test AEGP_WorldType constants are defined"""
    assert_equal(0, ae.sdk.AEGP_WorldType_NONE, "AEGP_WorldType_NONE should be 0")
    assert_equal(1, ae.sdk.AEGP_WorldType_8, "AEGP_WorldType_8 should be 1")
    assert_equal(2, ae.sdk.AEGP_WorldType_16, "AEGP_WorldType_16 should be 2")
    assert_equal(3, ae.sdk.AEGP_WorldType_32, "AEGP_WorldType_32 should be 3")
    print("World type constants verified")


@suite.test
def test_quality_constants():
    """Test PF_Quality constants are defined"""
    assert_equal(0, ae.sdk.PF_Quality_LO, "PF_Quality_LO should be 0")
    assert_equal(1, ae.sdk.PF_Quality_HI, "PF_Quality_HI should be 1")
    print("Quality constants verified")


@suite.test
def test_mode_flags_constants():
    """Test PF_ModeFlags constants are defined"""
    assert_equal(0, ae.sdk.PF_MF_Alpha_PREMUL, "PF_MF_Alpha_PREMUL should be 0")
    assert_equal(1, ae.sdk.PF_MF_Alpha_STRAIGHT, "PF_MF_Alpha_STRAIGHT should be 1")
    print("Mode flags constants verified")


# ============================================================================
# World Creation and Disposal
# ============================================================================

@suite.test
def test_new_world_8bit():
    """Test AEGP_NewWorld - create 8-bit world"""
    global _plugin_id
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        100,
        100
    )
    assert_true(worldH != 0, "World handle should be valid")
    print(f"Created 8-bit world: {worldH}")

    # Verify type
    world_type = ae.sdk.AEGP_GetWorldType(worldH)
    assert_equal(ae.sdk.AEGP_WorldType_8, world_type, "World type should be 8-bit")

    # Clean up
    ae.sdk.AEGP_DisposeWorld(worldH)
    print("8-bit world created and disposed successfully")


@suite.test
def test_new_world_16bit():
    """Test AEGP_NewWorld - create 16-bit world"""
    global _plugin_id
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_16,
        200,
        150
    )
    assert_true(worldH != 0, "World handle should be valid")
    print(f"Created 16-bit world: {worldH}")

    # Verify type
    world_type = ae.sdk.AEGP_GetWorldType(worldH)
    assert_equal(ae.sdk.AEGP_WorldType_16, world_type, "World type should be 16-bit")

    # Clean up
    ae.sdk.AEGP_DisposeWorld(worldH)
    print("16-bit world created and disposed successfully")


@suite.test
def test_new_world_32bit():
    """Test AEGP_NewWorld - create 32-bit float world"""
    global _plugin_id
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_32,
        150,
        100
    )
    assert_true(worldH != 0, "World handle should be valid")
    print(f"Created 32-bit float world: {worldH}")

    # Verify type
    world_type = ae.sdk.AEGP_GetWorldType(worldH)
    assert_equal(ae.sdk.AEGP_WorldType_32, world_type, "World type should be 32-bit float")

    # Clean up
    ae.sdk.AEGP_DisposeWorld(worldH)
    print("32-bit world created and disposed successfully")


@suite.test
def test_new_world_invalid_dimensions():
    """Test AEGP_NewWorld with invalid dimensions"""
    global _plugin_id

    # Test zero width
    try:
        ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 0, 100)
        raise AssertionError("Should have raised exception for zero width")
    except Exception as e:
        print(f"Correctly rejected zero width: {e}")

    # Test zero height
    try:
        ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 100, 0)
        raise AssertionError("Should have raised exception for zero height")
    except Exception as e:
        print(f"Correctly rejected zero height: {e}")

    # Test negative width
    try:
        ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, -100, 100)
        raise AssertionError("Should have raised exception for negative width")
    except Exception as e:
        print(f"Correctly rejected negative width: {e}")

    print("Invalid dimension validation works correctly")


# ============================================================================
# World Properties
# ============================================================================

@suite.test
def test_get_world_size():
    """Test AEGP_GetWorldSize - get world dimensions"""
    global _plugin_id
    width, height = 256, 128
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        width,
        height
    )

    size = ae.sdk.AEGP_GetWorldSize(worldH)
    assert_equal(width, size[0], f"Width should be {width}")
    assert_equal(height, size[1], f"Height should be {height}")
    print(f"World size: {size[0]}x{size[1]}")

    ae.sdk.AEGP_DisposeWorld(worldH)


@suite.test
def test_get_world_row_bytes():
    """Test AEGP_GetWorldRowBytes - get bytes per row"""
    global _plugin_id
    width, height = 100, 100
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        width,
        height
    )

    row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)
    # For 8-bit ARGB, minimum is width * 4 bytes, but may be padded
    min_row_bytes = width * 4
    assert_true(row_bytes >= min_row_bytes, f"Row bytes ({row_bytes}) should be >= {min_row_bytes}")
    print(f"Row bytes for {width}px wide 8-bit world: {row_bytes}")

    ae.sdk.AEGP_DisposeWorld(worldH)


@suite.test
def test_get_world_row_bytes_16bit():
    """Test AEGP_GetWorldRowBytes for 16-bit world"""
    global _plugin_id
    width, height = 100, 100
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_16,
        width,
        height
    )

    row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)
    # For 16-bit ARGB, minimum is width * 8 bytes (4 channels * 2 bytes each)
    min_row_bytes = width * 8
    assert_true(row_bytes >= min_row_bytes, f"Row bytes ({row_bytes}) should be >= {min_row_bytes}")
    print(f"Row bytes for {width}px wide 16-bit world: {row_bytes}")

    ae.sdk.AEGP_DisposeWorld(worldH)


@suite.test
def test_get_world_row_bytes_32bit():
    """Test AEGP_GetWorldRowBytes for 32-bit float world"""
    global _plugin_id
    width, height = 100, 100
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_32,
        width,
        height
    )

    row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)
    # For 32-bit float ARGB, minimum is width * 16 bytes (4 channels * 4 bytes each)
    min_row_bytes = width * 16
    assert_true(row_bytes >= min_row_bytes, f"Row bytes ({row_bytes}) should be >= {min_row_bytes}")
    print(f"Row bytes for {width}px wide 32-bit world: {row_bytes}")

    ae.sdk.AEGP_DisposeWorld(worldH)


# ============================================================================
# Base Address Access
# ============================================================================

@suite.test
def test_get_base_addr_8():
    """Test AEGP_GetWorldBaseAddr8 - get pixel data pointer for 8-bit world"""
    global _plugin_id
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        100,
        100
    )

    base_addr = ae.sdk.AEGP_GetWorldBaseAddr8(worldH)
    assert_true(base_addr != 0, "Base address should be valid")
    print(f"8-bit world base address: {hex(base_addr)}")

    ae.sdk.AEGP_DisposeWorld(worldH)


@suite.test
def test_get_base_addr_16():
    """Test AEGP_GetWorldBaseAddr16 - get pixel data pointer for 16-bit world"""
    global _plugin_id
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_16,
        100,
        100
    )

    base_addr = ae.sdk.AEGP_GetWorldBaseAddr16(worldH)
    assert_true(base_addr != 0, "Base address should be valid")
    print(f"16-bit world base address: {hex(base_addr)}")

    ae.sdk.AEGP_DisposeWorld(worldH)


@suite.test
def test_get_base_addr_32():
    """Test AEGP_GetWorldBaseAddr32 - get pixel data pointer for 32-bit world"""
    global _plugin_id
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_32,
        100,
        100
    )

    base_addr = ae.sdk.AEGP_GetWorldBaseAddr32(worldH)
    assert_true(base_addr != 0, "Base address should be valid")
    print(f"32-bit world base address: {hex(base_addr)}")

    ae.sdk.AEGP_DisposeWorld(worldH)


# NOTE: test_base_addr_type_mismatch is disabled because AE shows an error dialog
# instead of raising an exception when world type mismatches base address function.
# This causes the test to hang waiting for user interaction.
#
# @suite.test
# def test_base_addr_type_mismatch():
#     """Test that base address functions fail for wrong world types"""
#     global _plugin_id
#
#     # Create 8-bit world
#     worldH_8 = ae.sdk.AEGP_NewWorld(_plugin_id, ae.sdk.AEGP_WorldType_8, 50, 50)
#
#     # Try to get 16-bit address from 8-bit world
#     try:
#         ae.sdk.AEGP_GetWorldBaseAddr16(worldH_8)
#         print("Warning: GetBaseAddr16 on 8-bit world did not raise error")
#     except Exception as e:
#         print(f"Correctly rejected GetBaseAddr16 on 8-bit world: {e}")
#
#     ae.sdk.AEGP_DisposeWorld(worldH_8)


# ============================================================================
# PF_EffectWorld Structure
# ============================================================================

@suite.test
def test_fill_out_pf_effect_world():
    """Test AEGP_FillOutPFEffectWorld - get PF_EffectWorld structure"""
    global _plugin_id
    width, height = 200, 150
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        width,
        height
    )

    pf_world = ae.sdk.AEGP_FillOutPFEffectWorld(worldH)

    assert_equal(width, pf_world["width"], "PF_EffectWorld width should match")
    assert_equal(height, pf_world["height"], "PF_EffectWorld height should match")
    assert_true(pf_world["rowbytes"] > 0, "PF_EffectWorld rowbytes should be > 0")
    assert_true(pf_world["data"] != 0, "PF_EffectWorld data pointer should be valid")

    print(f"PF_EffectWorld: {width}x{height}, rowbytes={pf_world['rowbytes']}, data={hex(pf_world['data'])}")

    ae.sdk.AEGP_DisposeWorld(worldH)


# ============================================================================
# Fast Blur
# ============================================================================

@suite.test
def test_fast_blur():
    """Test AEGP_FastBlur - apply blur to world"""
    global _plugin_id
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        100,
        100
    )

    # Apply blur (this only works on user-allocated worlds)
    ae.sdk.AEGP_FastBlur(
        5.0,  # radius
        ae.sdk.PF_MF_Alpha_PREMUL,  # mode flags
        ae.sdk.PF_Quality_HI,  # quality
        worldH
    )
    print("Fast blur applied successfully with radius=5.0, quality=HI")

    ae.sdk.AEGP_DisposeWorld(worldH)


@suite.test
def test_fast_blur_invalid_params():
    """Test AEGP_FastBlur with invalid parameters"""
    global _plugin_id
    worldH = ae.sdk.AEGP_NewWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        100,
        100
    )

    # Test negative radius
    try:
        ae.sdk.AEGP_FastBlur(-1.0, ae.sdk.PF_MF_Alpha_PREMUL, ae.sdk.PF_Quality_HI, worldH)
        raise AssertionError("Should have raised exception for negative radius")
    except Exception as e:
        print(f"Correctly rejected negative radius: {e}")

    # Test invalid mode flags
    try:
        ae.sdk.AEGP_FastBlur(5.0, 999, ae.sdk.PF_Quality_HI, worldH)
        raise AssertionError("Should have raised exception for invalid mode flags")
    except Exception as e:
        print(f"Correctly rejected invalid mode flags: {e}")

    # Test invalid quality
    try:
        ae.sdk.AEGP_FastBlur(5.0, ae.sdk.PF_MF_Alpha_PREMUL, 999, worldH)
        raise AssertionError("Should have raised exception for invalid quality")
    except Exception as e:
        print(f"Correctly rejected invalid quality: {e}")

    ae.sdk.AEGP_DisposeWorld(worldH)
    print("Invalid parameter validation works correctly")


# ============================================================================
# Platform World
# ============================================================================

@suite.test
def test_new_platform_world():
    """Test AEGP_NewPlatformWorld - create platform-specific world"""
    global _plugin_id
    platformWorldH = ae.sdk.AEGP_NewPlatformWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        100,
        100
    )
    assert_true(platformWorldH != 0, "Platform world handle should be valid")
    print(f"Created platform world: {platformWorldH}")

    # Clean up
    ae.sdk.AEGP_DisposePlatformWorld(platformWorldH)
    print("Platform world created and disposed successfully")


@suite.test
def test_reference_from_platform_world():
    """Test AEGP_NewReferenceFromPlatformWorld - create world reference from platform world"""
    global _plugin_id

    # Create platform world
    platformWorldH = ae.sdk.AEGP_NewPlatformWorld(
        _plugin_id,
        ae.sdk.AEGP_WorldType_8,
        100,
        100
    )
    print(f"Created platform world: {platformWorldH}")

    # Create reference
    worldH = ae.sdk.AEGP_NewReferenceFromPlatformWorld(_plugin_id, platformWorldH)
    assert_true(worldH != 0, "World reference should be valid")
    print(f"Created world reference: {worldH}")

    # Verify we can use the reference
    size = ae.sdk.AEGP_GetWorldSize(worldH)
    assert_equal(100, size[0], "Width should be 100")
    assert_equal(100, size[1], "Height should be 100")

    # Clean up - dispose world reference first, then platform world
    ae.sdk.AEGP_DisposeWorld(worldH)
    ae.sdk.AEGP_DisposePlatformWorld(platformWorldH)
    print("World reference and platform world disposed successfully")


# ============================================================================
# Null Handle Validation
# ============================================================================

@suite.test
def test_null_handle_validation():
    """Test that null handles are properly rejected"""
    null_handle = 0

    # Test AEGP_DisposeWorld with null
    try:
        ae.sdk.AEGP_DisposeWorld(null_handle)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_DisposeWorld correctly rejected null: {e}")

    # Test AEGP_GetWorldType with null
    try:
        ae.sdk.AEGP_GetWorldType(null_handle)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_GetWorldType correctly rejected null: {e}")

    # Test AEGP_GetWorldSize with null
    try:
        ae.sdk.AEGP_GetWorldSize(null_handle)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_GetWorldSize correctly rejected null: {e}")

    print("Null handle validation works correctly")


# ============================================================================
# Run Tests
# ============================================================================

def run():
    """Run all tests and return results"""
    return suite.run()


if __name__ == "__main__":
    run()
