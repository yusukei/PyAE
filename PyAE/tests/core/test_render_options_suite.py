"""
RenderOptions Suite Tests
Tests for AEGP_RenderOptionsSuite4

RenderOptions Suite provides rendering configuration for compositions.
Essential for Export/Import workflows and render queue management.
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true, assert_not_none, assert_close
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true, assert_not_none, assert_close

suite = TestSuite("RenderOptions Suite")

# Test variables
_test_comp = None
_test_render_options = None


@suite.setup
def setup():
    """Setup test composition and render options"""
    global _test_comp, _test_render_options
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_RenderOptionsTestComp", 100, 100, 1.0, 5.0, 30.0)
    # Add a solid to make the composition renderable
    _test_comp.add_solid("_RenderOptionsTestSolid", 100, 100, (1.0, 0.0, 0.0), 5.0)
    _test_render_options = ae.sdk.AEGP_RenderOptions_NewFromItem(_test_comp._handle)
    print(f"Created test comp: {_test_comp.name}")
    print(f"Created render options handle: {_test_render_options}")


@suite.teardown
def teardown():
    """Cleanup test resources"""
    global _test_comp, _test_render_options
    # Dispose render options first
    if _test_render_options:
        try:
            ae.sdk.AEGP_RenderOptions_Dispose(_test_render_options)
        except Exception as e:
            print(f"Warning: Failed to dispose render options: {e}")
    # Delete test composition
    if _test_comp:
        try:
            ae.sdk.AEGP_DeleteItem(_test_comp._handle)
        except Exception as e:
            print(f"Warning: Failed to delete test comp: {e}")


# ============================================================================
# Function Existence Tests
# ============================================================================

@suite.test
def test_function_new_from_item_exists():
    """Test AEGP_RenderOptions_NewFromItem function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_NewFromItem"),
                "AEGP_RenderOptions_NewFromItem should exist")
    print("AEGP_RenderOptions_NewFromItem exists")


@suite.test
def test_function_duplicate_exists():
    """Test AEGP_RenderOptions_Duplicate function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_Duplicate"),
                "AEGP_RenderOptions_Duplicate should exist")
    print("AEGP_RenderOptions_Duplicate exists")


@suite.test
def test_function_dispose_exists():
    """Test AEGP_RenderOptions_Dispose function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_Dispose"),
                "AEGP_RenderOptions_Dispose should exist")
    print("AEGP_RenderOptions_Dispose exists")


@suite.test
def test_function_set_time_exists():
    """Test AEGP_RenderOptions_SetTime function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetTime"),
                "AEGP_RenderOptions_SetTime should exist")
    print("AEGP_RenderOptions_SetTime exists")


@suite.test
def test_function_get_time_exists():
    """Test AEGP_RenderOptions_GetTime function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetTime"),
                "AEGP_RenderOptions_GetTime should exist")
    print("AEGP_RenderOptions_GetTime exists")


@suite.test
def test_function_set_time_step_exists():
    """Test AEGP_RenderOptions_SetTimeStep function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetTimeStep"),
                "AEGP_RenderOptions_SetTimeStep should exist")
    print("AEGP_RenderOptions_SetTimeStep exists")


@suite.test
def test_function_get_time_step_exists():
    """Test AEGP_RenderOptions_GetTimeStep function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetTimeStep"),
                "AEGP_RenderOptions_GetTimeStep should exist")
    print("AEGP_RenderOptions_GetTimeStep exists")


@suite.test
def test_function_set_field_render_exists():
    """Test AEGP_RenderOptions_SetFieldRender function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetFieldRender"),
                "AEGP_RenderOptions_SetFieldRender should exist")
    print("AEGP_RenderOptions_SetFieldRender exists")


@suite.test
def test_function_get_field_render_exists():
    """Test AEGP_RenderOptions_GetFieldRender function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetFieldRender"),
                "AEGP_RenderOptions_GetFieldRender should exist")
    print("AEGP_RenderOptions_GetFieldRender exists")


@suite.test
def test_function_set_world_type_exists():
    """Test AEGP_RenderOptions_SetWorldType function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetWorldType"),
                "AEGP_RenderOptions_SetWorldType should exist")
    print("AEGP_RenderOptions_SetWorldType exists")


@suite.test
def test_function_get_world_type_exists():
    """Test AEGP_RenderOptions_GetWorldType function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetWorldType"),
                "AEGP_RenderOptions_GetWorldType should exist")
    print("AEGP_RenderOptions_GetWorldType exists")


@suite.test
def test_function_set_downsample_factor_exists():
    """Test AEGP_RenderOptions_SetDownsampleFactor function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetDownsampleFactor"),
                "AEGP_RenderOptions_SetDownsampleFactor should exist")
    print("AEGP_RenderOptions_SetDownsampleFactor exists")


@suite.test
def test_function_get_downsample_factor_exists():
    """Test AEGP_RenderOptions_GetDownsampleFactor function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetDownsampleFactor"),
                "AEGP_RenderOptions_GetDownsampleFactor should exist")
    print("AEGP_RenderOptions_GetDownsampleFactor exists")


@suite.test
def test_function_set_region_of_interest_exists():
    """Test AEGP_RenderOptions_SetRegionOfInterest function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetRegionOfInterest"),
                "AEGP_RenderOptions_SetRegionOfInterest should exist")
    print("AEGP_RenderOptions_SetRegionOfInterest exists")


@suite.test
def test_function_get_region_of_interest_exists():
    """Test AEGP_RenderOptions_GetRegionOfInterest function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetRegionOfInterest"),
                "AEGP_RenderOptions_GetRegionOfInterest should exist")
    print("AEGP_RenderOptions_GetRegionOfInterest exists")


@suite.test
def test_function_set_matte_mode_exists():
    """Test AEGP_RenderOptions_SetMatteMode function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetMatteMode"),
                "AEGP_RenderOptions_SetMatteMode should exist")
    print("AEGP_RenderOptions_SetMatteMode exists")


@suite.test
def test_function_get_matte_mode_exists():
    """Test AEGP_RenderOptions_GetMatteMode function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetMatteMode"),
                "AEGP_RenderOptions_GetMatteMode should exist")
    print("AEGP_RenderOptions_GetMatteMode exists")


@suite.test
def test_function_set_channel_order_exists():
    """Test AEGP_RenderOptions_SetChannelOrder function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetChannelOrder"),
                "AEGP_RenderOptions_SetChannelOrder should exist")
    print("AEGP_RenderOptions_SetChannelOrder exists")


@suite.test
def test_function_get_channel_order_exists():
    """Test AEGP_RenderOptions_GetChannelOrder function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetChannelOrder"),
                "AEGP_RenderOptions_GetChannelOrder should exist")
    print("AEGP_RenderOptions_GetChannelOrder exists")


@suite.test
def test_function_get_render_guide_layers_exists():
    """Test AEGP_RenderOptions_GetRenderGuideLayers function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetRenderGuideLayers"),
                "AEGP_RenderOptions_GetRenderGuideLayers should exist")
    print("AEGP_RenderOptions_GetRenderGuideLayers exists")


@suite.test
def test_function_set_render_guide_layers_exists():
    """Test AEGP_RenderOptions_SetRenderGuideLayers function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetRenderGuideLayers"),
                "AEGP_RenderOptions_SetRenderGuideLayers should exist")
    print("AEGP_RenderOptions_SetRenderGuideLayers exists")


@suite.test
def test_function_get_render_quality_exists():
    """Test AEGP_RenderOptions_GetRenderQuality function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_GetRenderQuality"),
                "AEGP_RenderOptions_GetRenderQuality should exist")
    print("AEGP_RenderOptions_GetRenderQuality exists")


@suite.test
def test_function_set_render_quality_exists():
    """Test AEGP_RenderOptions_SetRenderQuality function exists"""
    assert_true(hasattr(ae.sdk, "AEGP_RenderOptions_SetRenderQuality"),
                "AEGP_RenderOptions_SetRenderQuality should exist")
    print("AEGP_RenderOptions_SetRenderQuality exists")


# ============================================================================
# Constant Existence Tests
# ============================================================================

@suite.test
def test_matte_mode_constants():
    """Test AEGP_MatteMode constants are defined"""
    assert_true(hasattr(ae.sdk, "AEGP_MatteMode_STRAIGHT"),
                "AEGP_MatteMode_STRAIGHT should be defined")
    assert_true(hasattr(ae.sdk, "AEGP_MatteMode_PREMUL_BLACK"),
                "AEGP_MatteMode_PREMUL_BLACK should be defined")
    assert_true(hasattr(ae.sdk, "AEGP_MatteMode_PREMUL_BG_COLOR"),
                "AEGP_MatteMode_PREMUL_BG_COLOR should be defined")
    print(f"AEGP_MatteMode_STRAIGHT = {ae.sdk.AEGP_MatteMode_STRAIGHT}")
    print(f"AEGP_MatteMode_PREMUL_BLACK = {ae.sdk.AEGP_MatteMode_PREMUL_BLACK}")
    print(f"AEGP_MatteMode_PREMUL_BG_COLOR = {ae.sdk.AEGP_MatteMode_PREMUL_BG_COLOR}")


@suite.test
def test_channel_order_constants():
    """Test AEGP_ChannelOrder constants are defined"""
    assert_true(hasattr(ae.sdk, "AEGP_ChannelOrder_ARGB"),
                "AEGP_ChannelOrder_ARGB should be defined")
    assert_true(hasattr(ae.sdk, "AEGP_ChannelOrder_BGRA"),
                "AEGP_ChannelOrder_BGRA should be defined")
    print(f"AEGP_ChannelOrder_ARGB = {ae.sdk.AEGP_ChannelOrder_ARGB}")
    print(f"AEGP_ChannelOrder_BGRA = {ae.sdk.AEGP_ChannelOrder_BGRA}")


@suite.test
def test_item_quality_constants():
    """Test AEGP_ItemQuality constants are defined"""
    assert_true(hasattr(ae.sdk, "AEGP_ItemQuality_DRAFT"),
                "AEGP_ItemQuality_DRAFT should be defined")
    assert_true(hasattr(ae.sdk, "AEGP_ItemQuality_BEST"),
                "AEGP_ItemQuality_BEST should be defined")
    print(f"AEGP_ItemQuality_DRAFT = {ae.sdk.AEGP_ItemQuality_DRAFT}")
    print(f"AEGP_ItemQuality_BEST = {ae.sdk.AEGP_ItemQuality_BEST}")


@suite.test
def test_field_render_constants():
    """Test PF_Field constants are defined"""
    assert_true(hasattr(ae.sdk, "PF_Field_FRAME"),
                "PF_Field_FRAME should be defined")
    assert_true(hasattr(ae.sdk, "PF_Field_UPPER"),
                "PF_Field_UPPER should be defined")
    assert_true(hasattr(ae.sdk, "PF_Field_LOWER"),
                "PF_Field_LOWER should be defined")
    print(f"PF_Field_FRAME = {ae.sdk.PF_Field_FRAME}")
    print(f"PF_Field_UPPER = {ae.sdk.PF_Field_UPPER}")
    print(f"PF_Field_LOWER = {ae.sdk.PF_Field_LOWER}")


@suite.test
def test_world_type_constants():
    """Test AEGP_WorldType constants are defined"""
    assert_true(hasattr(ae.sdk, "AEGP_WorldType_NONE"),
                "AEGP_WorldType_NONE should be defined")
    assert_true(hasattr(ae.sdk, "AEGP_WorldType_8"),
                "AEGP_WorldType_8 should be defined")
    assert_true(hasattr(ae.sdk, "AEGP_WorldType_16"),
                "AEGP_WorldType_16 should be defined")
    assert_true(hasattr(ae.sdk, "AEGP_WorldType_32"),
                "AEGP_WorldType_32 should be defined")
    print(f"AEGP_WorldType_NONE = {ae.sdk.AEGP_WorldType_NONE}")
    print(f"AEGP_WorldType_8 = {ae.sdk.AEGP_WorldType_8}")
    print(f"AEGP_WorldType_16 = {ae.sdk.AEGP_WorldType_16}")
    print(f"AEGP_WorldType_32 = {ae.sdk.AEGP_WorldType_32}")


# ============================================================================
# RenderOptions Creation and Disposal Tests
# ============================================================================

@suite.test
def test_new_from_item():
    """Test AEGP_RenderOptions_NewFromItem creates valid handle"""
    global _test_render_options
    assert_true(_test_render_options != 0, "Render options handle should be valid")
    print(f"Created render options from comp: {_test_render_options}")


@suite.test
def test_duplicate():
    """Test AEGP_RenderOptions_Duplicate creates a copy"""
    global _test_render_options
    copy_handle = ae.sdk.AEGP_RenderOptions_Duplicate(_test_render_options)
    assert_true(copy_handle != 0, "Duplicate handle should be valid")
    assert_true(copy_handle != _test_render_options, "Duplicate should be different handle")
    print(f"Duplicated render options: {copy_handle}")
    # Clean up duplicate
    ae.sdk.AEGP_RenderOptions_Dispose(copy_handle)
    print("Duplicate disposed successfully")


# ============================================================================
# Time Property Tests
# ============================================================================

@suite.test
def test_set_get_time():
    """Test AEGP_RenderOptions_SetTime and GetTime"""
    global _test_render_options
    test_time = 1.5  # 1.5 seconds

    ae.sdk.AEGP_RenderOptions_SetTime(_test_render_options, test_time)
    result_time = ae.sdk.AEGP_RenderOptions_GetTime(_test_render_options)

    assert_close(test_time, result_time, 0.001, f"Time should be {test_time}, got {result_time}")
    print(f"Set time to {test_time}s, got back {result_time}s")


@suite.test
def test_set_get_time_zero():
    """Test setting time to zero"""
    global _test_render_options
    test_time = 0.0

    ae.sdk.AEGP_RenderOptions_SetTime(_test_render_options, test_time)
    result_time = ae.sdk.AEGP_RenderOptions_GetTime(_test_render_options)

    assert_close(test_time, result_time, 0.001, f"Time should be {test_time}, got {result_time}")
    print(f"Set time to {test_time}s, got back {result_time}s")


@suite.test
def test_set_get_time_step():
    """Test AEGP_RenderOptions_SetTimeStep and GetTimeStep"""
    global _test_render_options
    test_time_step = 1.0 / 30.0  # 1 frame at 30fps

    ae.sdk.AEGP_RenderOptions_SetTimeStep(_test_render_options, test_time_step)
    result_time_step = ae.sdk.AEGP_RenderOptions_GetTimeStep(_test_render_options)

    assert_close(test_time_step, result_time_step, 0.0001,
                 f"Time step should be {test_time_step}, got {result_time_step}")
    print(f"Set time step to {test_time_step}s, got back {result_time_step}s")


# ============================================================================
# Field Render Tests
# ============================================================================

@suite.test
def test_set_get_field_render_frame():
    """Test AEGP_RenderOptions_SetFieldRender with FRAME mode"""
    global _test_render_options
    field_mode = ae.sdk.PF_Field_FRAME

    ae.sdk.AEGP_RenderOptions_SetFieldRender(_test_render_options, field_mode)
    result = ae.sdk.AEGP_RenderOptions_GetFieldRender(_test_render_options)

    assert_equal(field_mode, result, f"Field render should be {field_mode}, got {result}")
    print(f"Set field render to FRAME ({field_mode}), got back {result}")


@suite.test
def test_set_get_field_render_upper():
    """Test AEGP_RenderOptions_SetFieldRender with UPPER mode"""
    global _test_render_options
    field_mode = ae.sdk.PF_Field_UPPER

    ae.sdk.AEGP_RenderOptions_SetFieldRender(_test_render_options, field_mode)
    result = ae.sdk.AEGP_RenderOptions_GetFieldRender(_test_render_options)

    assert_equal(field_mode, result, f"Field render should be {field_mode}, got {result}")
    print(f"Set field render to UPPER ({field_mode}), got back {result}")


@suite.test
def test_set_get_field_render_lower():
    """Test AEGP_RenderOptions_SetFieldRender with LOWER mode"""
    global _test_render_options
    field_mode = ae.sdk.PF_Field_LOWER

    ae.sdk.AEGP_RenderOptions_SetFieldRender(_test_render_options, field_mode)
    result = ae.sdk.AEGP_RenderOptions_GetFieldRender(_test_render_options)

    assert_equal(field_mode, result, f"Field render should be {field_mode}, got {result}")
    print(f"Set field render to LOWER ({field_mode}), got back {result}")


# ============================================================================
# World Type Tests
# ============================================================================

@suite.test
def test_set_get_world_type_8():
    """Test AEGP_RenderOptions_SetWorldType with 8-bit"""
    global _test_render_options
    world_type = ae.sdk.AEGP_WorldType_8

    ae.sdk.AEGP_RenderOptions_SetWorldType(_test_render_options, world_type)
    result = ae.sdk.AEGP_RenderOptions_GetWorldType(_test_render_options)

    assert_equal(world_type, result, f"World type should be {world_type}, got {result}")
    print(f"Set world type to 8-bit ({world_type}), got back {result}")


@suite.test
def test_set_get_world_type_16():
    """Test AEGP_RenderOptions_SetWorldType with 16-bit"""
    global _test_render_options
    world_type = ae.sdk.AEGP_WorldType_16

    ae.sdk.AEGP_RenderOptions_SetWorldType(_test_render_options, world_type)
    result = ae.sdk.AEGP_RenderOptions_GetWorldType(_test_render_options)

    assert_equal(world_type, result, f"World type should be {world_type}, got {result}")
    print(f"Set world type to 16-bit ({world_type}), got back {result}")


@suite.test
def test_set_get_world_type_32():
    """Test AEGP_RenderOptions_SetWorldType with 32-bit float"""
    global _test_render_options
    world_type = ae.sdk.AEGP_WorldType_32

    ae.sdk.AEGP_RenderOptions_SetWorldType(_test_render_options, world_type)
    result = ae.sdk.AEGP_RenderOptions_GetWorldType(_test_render_options)

    assert_equal(world_type, result, f"World type should be {world_type}, got {result}")
    print(f"Set world type to 32-bit ({world_type}), got back {result}")


# ============================================================================
# Downsample Factor Tests
# ============================================================================

@suite.test
def test_set_get_downsample_factor():
    """Test AEGP_RenderOptions_SetDownsampleFactor and GetDownsampleFactor"""
    global _test_render_options
    x_factor, y_factor = 2, 2

    ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(_test_render_options, x_factor, y_factor)
    result = ae.sdk.AEGP_RenderOptions_GetDownsampleFactor(_test_render_options)

    assert_equal(x_factor, result[0], f"X factor should be {x_factor}, got {result[0]}")
    assert_equal(y_factor, result[1], f"Y factor should be {y_factor}, got {result[1]}")
    print(f"Set downsample factor to ({x_factor}, {y_factor}), got back {result}")


@suite.test
def test_set_get_downsample_factor_full():
    """Test downsample factor with full resolution (1,1)"""
    global _test_render_options
    x_factor, y_factor = 1, 1

    ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(_test_render_options, x_factor, y_factor)
    result = ae.sdk.AEGP_RenderOptions_GetDownsampleFactor(_test_render_options)

    assert_equal(x_factor, result[0], f"X factor should be {x_factor}, got {result[0]}")
    assert_equal(y_factor, result[1], f"Y factor should be {y_factor}, got {result[1]}")
    print(f"Set downsample factor to full resolution ({x_factor}, {y_factor}), got back {result}")


@suite.test
def test_set_get_downsample_factor_asymmetric():
    """Test downsample factor with asymmetric values"""
    global _test_render_options
    x_factor, y_factor = 2, 4

    ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(_test_render_options, x_factor, y_factor)
    result = ae.sdk.AEGP_RenderOptions_GetDownsampleFactor(_test_render_options)

    assert_equal(x_factor, result[0], f"X factor should be {x_factor}, got {result[0]}")
    assert_equal(y_factor, result[1], f"Y factor should be {y_factor}, got {result[1]}")
    print(f"Set asymmetric downsample factor ({x_factor}, {y_factor}), got back {result}")


# ============================================================================
# Region of Interest Tests
# ============================================================================

@suite.test
def test_set_get_region_of_interest():
    """Test AEGP_RenderOptions_SetRegionOfInterest and GetRegionOfInterest"""
    global _test_render_options
    # ROI dictionary with left, top, right, bottom
    roi = {"left": 10, "top": 20, "right": 90, "bottom": 80}

    ae.sdk.AEGP_RenderOptions_SetRegionOfInterest(_test_render_options, roi)
    result = ae.sdk.AEGP_RenderOptions_GetRegionOfInterest(_test_render_options)

    assert_not_none(result, "ROI result should not be None")
    assert_equal(roi["left"], result["left"], f"ROI left should be {roi['left']}, got {result.get('left')}")
    assert_equal(roi["top"], result["top"], f"ROI top should be {roi['top']}, got {result.get('top')}")
    assert_equal(roi["right"], result["right"], f"ROI right should be {roi['right']}, got {result.get('right')}")
    assert_equal(roi["bottom"], result["bottom"], f"ROI bottom should be {roi['bottom']}, got {result.get('bottom')}")
    print(f"Set ROI to {roi}, got back {result}")


@suite.test
def test_set_get_region_of_interest_full():
    """Test ROI covering full composition"""
    global _test_render_options
    # Full 100x100 composition
    roi = {"left": 0, "top": 0, "right": 100, "bottom": 100}

    ae.sdk.AEGP_RenderOptions_SetRegionOfInterest(_test_render_options, roi)
    result = ae.sdk.AEGP_RenderOptions_GetRegionOfInterest(_test_render_options)

    assert_equal(roi["left"], result["left"], "ROI left should match")
    assert_equal(roi["top"], result["top"], "ROI top should match")
    assert_equal(roi["right"], result["right"], "ROI right should match")
    assert_equal(roi["bottom"], result["bottom"], "ROI bottom should match")
    print(f"Set full ROI {roi}, got back {result}")


# ============================================================================
# Matte Mode Tests
# ============================================================================

@suite.test
def test_set_get_matte_mode_straight():
    """Test AEGP_RenderOptions_SetMatteMode with STRAIGHT"""
    global _test_render_options
    mode = ae.sdk.AEGP_MatteMode_STRAIGHT

    ae.sdk.AEGP_RenderOptions_SetMatteMode(_test_render_options, mode)
    result = ae.sdk.AEGP_RenderOptions_GetMatteMode(_test_render_options)

    assert_equal(mode, result, f"Matte mode should be {mode}, got {result}")
    print(f"Set matte mode to STRAIGHT ({mode}), got back {result}")


@suite.test
def test_set_get_matte_mode_premul_black():
    """Test AEGP_RenderOptions_SetMatteMode with PREMUL_BLACK"""
    global _test_render_options
    mode = ae.sdk.AEGP_MatteMode_PREMUL_BLACK

    ae.sdk.AEGP_RenderOptions_SetMatteMode(_test_render_options, mode)
    result = ae.sdk.AEGP_RenderOptions_GetMatteMode(_test_render_options)

    assert_equal(mode, result, f"Matte mode should be {mode}, got {result}")
    print(f"Set matte mode to PREMUL_BLACK ({mode}), got back {result}")


@suite.test
def test_set_get_matte_mode_premul_bg_color():
    """Test AEGP_RenderOptions_SetMatteMode with PREMUL_BG_COLOR"""
    global _test_render_options
    mode = ae.sdk.AEGP_MatteMode_PREMUL_BG_COLOR

    ae.sdk.AEGP_RenderOptions_SetMatteMode(_test_render_options, mode)
    result = ae.sdk.AEGP_RenderOptions_GetMatteMode(_test_render_options)

    assert_equal(mode, result, f"Matte mode should be {mode}, got {result}")
    print(f"Set matte mode to PREMUL_BG_COLOR ({mode}), got back {result}")


# ============================================================================
# Channel Order Tests
# ============================================================================

@suite.test
def test_set_get_channel_order_argb():
    """Test AEGP_RenderOptions_SetChannelOrder with ARGB"""
    global _test_render_options
    order = ae.sdk.AEGP_ChannelOrder_ARGB

    ae.sdk.AEGP_RenderOptions_SetChannelOrder(_test_render_options, order)
    result = ae.sdk.AEGP_RenderOptions_GetChannelOrder(_test_render_options)

    assert_equal(order, result, f"Channel order should be {order}, got {result}")
    print(f"Set channel order to ARGB ({order}), got back {result}")


@suite.test
def test_set_get_channel_order_bgra():
    """Test AEGP_RenderOptions_SetChannelOrder with BGRA"""
    global _test_render_options
    order = ae.sdk.AEGP_ChannelOrder_BGRA

    ae.sdk.AEGP_RenderOptions_SetChannelOrder(_test_render_options, order)
    result = ae.sdk.AEGP_RenderOptions_GetChannelOrder(_test_render_options)

    assert_equal(order, result, f"Channel order should be {order}, got {result}")
    print(f"Set channel order to BGRA ({order}), got back {result}")


# ============================================================================
# Render Guide Layers Tests
# ============================================================================

@suite.test
def test_set_get_render_guide_layers_true():
    """Test AEGP_RenderOptions_SetRenderGuideLayers with True"""
    global _test_render_options

    ae.sdk.AEGP_RenderOptions_SetRenderGuideLayers(_test_render_options, True)
    result = ae.sdk.AEGP_RenderOptions_GetRenderGuideLayers(_test_render_options)

    assert_equal(True, result, f"Render guide layers should be True, got {result}")
    print(f"Set render guide layers to True, got back {result}")


@suite.test
def test_set_get_render_guide_layers_false():
    """Test AEGP_RenderOptions_SetRenderGuideLayers with False"""
    global _test_render_options

    ae.sdk.AEGP_RenderOptions_SetRenderGuideLayers(_test_render_options, False)
    result = ae.sdk.AEGP_RenderOptions_GetRenderGuideLayers(_test_render_options)

    assert_equal(False, result, f"Render guide layers should be False, got {result}")
    print(f"Set render guide layers to False, got back {result}")


# ============================================================================
# Render Quality Tests
# ============================================================================

@suite.test
def test_set_get_render_quality_draft():
    """Test AEGP_RenderOptions_SetRenderQuality with DRAFT"""
    global _test_render_options
    quality = ae.sdk.AEGP_ItemQuality_DRAFT

    ae.sdk.AEGP_RenderOptions_SetRenderQuality(_test_render_options, quality)
    result = ae.sdk.AEGP_RenderOptions_GetRenderQuality(_test_render_options)

    assert_equal(quality, result, f"Render quality should be {quality}, got {result}")
    print(f"Set render quality to DRAFT ({quality}), got back {result}")


@suite.test
def test_set_get_render_quality_best():
    """Test AEGP_RenderOptions_SetRenderQuality with BEST"""
    global _test_render_options
    quality = ae.sdk.AEGP_ItemQuality_BEST

    ae.sdk.AEGP_RenderOptions_SetRenderQuality(_test_render_options, quality)
    result = ae.sdk.AEGP_RenderOptions_GetRenderQuality(_test_render_options)

    assert_equal(quality, result, f"Render quality should be {quality}, got {result}")
    print(f"Set render quality to BEST ({quality}), got back {result}")


# ============================================================================
# NULL Handle Validation Tests
# ============================================================================

@suite.test
def test_null_handle_dispose():
    """Test AEGP_RenderOptions_Dispose with null handle"""
    null_handle = 0
    try:
        ae.sdk.AEGP_RenderOptions_Dispose(null_handle)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_RenderOptions_Dispose correctly rejected null: {e}")


@suite.test
def test_null_handle_get_time():
    """Test AEGP_RenderOptions_GetTime with null handle"""
    null_handle = 0
    try:
        ae.sdk.AEGP_RenderOptions_GetTime(null_handle)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_RenderOptions_GetTime correctly rejected null: {e}")


@suite.test
def test_null_handle_set_time():
    """Test AEGP_RenderOptions_SetTime with null handle"""
    null_handle = 0
    try:
        ae.sdk.AEGP_RenderOptions_SetTime(null_handle, 1.0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_RenderOptions_SetTime correctly rejected null: {e}")


@suite.test
def test_null_handle_duplicate():
    """Test AEGP_RenderOptions_Duplicate with null handle"""
    null_handle = 0
    try:
        ae.sdk.AEGP_RenderOptions_Duplicate(null_handle)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_RenderOptions_Duplicate correctly rejected null: {e}")


@suite.test
def test_null_handle_get_world_type():
    """Test AEGP_RenderOptions_GetWorldType with null handle"""
    null_handle = 0
    try:
        ae.sdk.AEGP_RenderOptions_GetWorldType(null_handle)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_RenderOptions_GetWorldType correctly rejected null: {e}")


# ============================================================================
# Comprehensive Workflow Tests
# ============================================================================

@suite.test
def test_full_render_options_workflow():
    """Test complete render options workflow"""
    global _test_comp

    # Create new render options from comp
    options = ae.sdk.AEGP_RenderOptions_NewFromItem(_test_comp._handle)
    assert_true(options != 0, "Should create valid render options")

    # Configure all settings
    ae.sdk.AEGP_RenderOptions_SetTime(options, 2.0)
    ae.sdk.AEGP_RenderOptions_SetTimeStep(options, 1.0/30.0)
    ae.sdk.AEGP_RenderOptions_SetFieldRender(options, ae.sdk.PF_Field_FRAME)
    ae.sdk.AEGP_RenderOptions_SetWorldType(options, ae.sdk.AEGP_WorldType_8)
    ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(options, 1, 1)
    ae.sdk.AEGP_RenderOptions_SetMatteMode(options, ae.sdk.AEGP_MatteMode_STRAIGHT)
    ae.sdk.AEGP_RenderOptions_SetChannelOrder(options, ae.sdk.AEGP_ChannelOrder_ARGB)
    ae.sdk.AEGP_RenderOptions_SetRenderGuideLayers(options, False)
    ae.sdk.AEGP_RenderOptions_SetRenderQuality(options, ae.sdk.AEGP_ItemQuality_BEST)

    # Verify all settings
    assert_close(2.0, ae.sdk.AEGP_RenderOptions_GetTime(options), 0.001)
    assert_close(1.0/30.0, ae.sdk.AEGP_RenderOptions_GetTimeStep(options), 0.0001)
    assert_equal(ae.sdk.PF_Field_FRAME, ae.sdk.AEGP_RenderOptions_GetFieldRender(options))
    assert_equal(ae.sdk.AEGP_WorldType_8, ae.sdk.AEGP_RenderOptions_GetWorldType(options))
    downsample = ae.sdk.AEGP_RenderOptions_GetDownsampleFactor(options)
    assert_equal(1, downsample[0])
    assert_equal(1, downsample[1])
    assert_equal(ae.sdk.AEGP_MatteMode_STRAIGHT, ae.sdk.AEGP_RenderOptions_GetMatteMode(options))
    assert_equal(ae.sdk.AEGP_ChannelOrder_ARGB, ae.sdk.AEGP_RenderOptions_GetChannelOrder(options))
    assert_equal(False, ae.sdk.AEGP_RenderOptions_GetRenderGuideLayers(options))
    assert_equal(ae.sdk.AEGP_ItemQuality_BEST, ae.sdk.AEGP_RenderOptions_GetRenderQuality(options))

    # Duplicate and verify copy has same settings
    copy = ae.sdk.AEGP_RenderOptions_Duplicate(options)
    assert_close(2.0, ae.sdk.AEGP_RenderOptions_GetTime(copy), 0.001)
    assert_equal(ae.sdk.AEGP_WorldType_8, ae.sdk.AEGP_RenderOptions_GetWorldType(copy))

    # Clean up
    ae.sdk.AEGP_RenderOptions_Dispose(copy)
    ae.sdk.AEGP_RenderOptions_Dispose(options)

    print("Full render options workflow completed successfully")


# ============================================================================
# Run Tests
# ============================================================================

def run():
    """Run all tests and return results"""
    return suite.run()


if __name__ == "__main__":
    run()
