# test_colorsettings_suite.py
# PyAE ColorSettings Suite Tests
# Tests for AEGP_ColorSettingsSuite6

import ae
import ae.sdk as sdk

try:
    from .test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
        assert_close,
        skip,
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
        assert_close,
        skip,
    )


suite = TestSuite("ColorSettings Suite")

# Test variables
_test_comp = None
_is_ocio = False  # OCIO and ColorProfile are mutually exclusive


@suite.setup
def setup():
    """Setup test composition and detect color management mode"""
    global _test_comp, _is_ocio
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_ColorSettingsTestComp", 1920, 1080, 1.0, 5.0, 30.0)

    # Detect color management mode (OCIO vs ColorProfile are mutually exclusive)
    _is_ocio = sdk.AEGP_IsOCIOColorManagementUsed()
    print(f"Color management mode: {'OCIO' if _is_ocio else 'ColorProfile (ICC)'}")


@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp
    if _test_comp is not None:
        _test_comp.delete()
        _test_comp = None


# ============================================================================
# OCIO Color Management Tests (No parameters required)
# ============================================================================

@suite.test
def test_is_ocio_color_management_used():
    """Test AEGP_IsOCIOColorManagementUsed - checks if OCIO is used"""
    is_ocio = sdk.AEGP_IsOCIOColorManagementUsed()
    assert_isinstance(is_ocio, bool, "Should return boolean")
    print(f"OCIO color management used: {is_ocio}")


@suite.test
def test_get_ocio_configuration_file():
    """Test AEGP_GetOCIOConfigurationFile - gets OCIO config file name"""
    if not _is_ocio:
        skip("OCIO color management is not enabled")
        return

    config_file = sdk.AEGP_GetOCIOConfigurationFile()
    assert_isinstance(config_file, str, "Should return string")
    print(f"OCIO config file: '{config_file}'")


@suite.test
def test_get_ocio_configuration_file_path():
    """Test AEGP_GetOCIOConfigurationFilePath - gets OCIO config full path"""
    if not _is_ocio:
        skip("OCIO color management is not enabled")
        return

    config_path = sdk.AEGP_GetOCIOConfigurationFilePath()
    assert_isinstance(config_path, str, "Should return string")
    print(f"OCIO config path: '{config_path}'")


@suite.test
def test_get_ocio_working_color_space():
    """Test AEGPD_GetOCIOWorkingColorSpace - gets OCIO working color space"""
    if not _is_ocio:
        skip("OCIO color management is not enabled")
        return

    working_cs = sdk.AEGPD_GetOCIOWorkingColorSpace()
    assert_isinstance(working_cs, str, "Should return string")
    print(f"OCIO working color space: '{working_cs}'")


@suite.test
def test_get_ocio_display_color_space():
    """Test AEGPD_GetOCIODisplayColorSpace - gets OCIO display and view"""
    if not _is_ocio:
        skip("OCIO color management is not enabled")
        return

    # This API fails with "Display view color space not valid" (error 5027)
    # when OCIO display/view is not properly configured in the environment
    try:
        result = sdk.AEGPD_GetOCIODisplayColorSpace()
    except RuntimeError as e:
        skip(f"OCIO display color space not available: {e}")
        return

    assert_isinstance(result, tuple, "Should return tuple")
    assert_equal(len(result), 2, "Should return (display, view)")
    display, view = result
    assert_isinstance(display, str, "Display should be string")
    assert_isinstance(view, str, "View should be string")
    print(f"OCIO display: '{display}', view: '{view}'")


@suite.test
def test_is_color_space_aware_effects_enabled():
    """Test AEGPD_IsColorSpaceAwareEffectsEnabled - checks color space aware effects"""
    if not _is_ocio:
        skip("OCIO color management is not enabled")
        return

    is_enabled = sdk.AEGPD_IsColorSpaceAwareEffectsEnabled()
    assert_isinstance(is_enabled, bool, "Should return boolean")
    print(f"Color space aware effects enabled: {is_enabled}")


@suite.test
def test_get_lut_interpolation_method():
    """Test AEGPD_GetLUTInterpolationMethod - gets LUT interpolation method"""
    if not _is_ocio:
        skip("OCIO color management is not enabled")
        return

    method = sdk.AEGPD_GetLUTInterpolationMethod()
    assert_isinstance(method, int, "Should return integer")
    print(f"LUT interpolation method: {method}")


@suite.test
def test_get_graphics_white_luminance():
    """Test AEGPD_GetGraphicsWhiteLuminance - gets graphics white luminance"""
    if not _is_ocio:
        skip("OCIO color management is not enabled")
        return

    luminance = sdk.AEGPD_GetGraphicsWhiteLuminance()
    assert_isinstance(luminance, int, "Should return integer")
    assert_true(luminance >= 0, "Luminance should be non-negative")
    print(f"Graphics white luminance: {luminance}")


@suite.test
def test_get_working_color_space_id():
    """Test AEGPD_GetWorkingColorSpaceId - gets working color space GUID"""
    if not _is_ocio:
        skip("OCIO color management is not enabled")
        return

    guid = sdk.AEGPD_GetWorkingColorSpaceId()
    assert_isinstance(guid, bytes, "Should return bytes")
    assert_equal(len(guid), 16, "GUID should be 16 bytes")
    print(f"Working color space GUID: {guid.hex()}")


# ============================================================================
# Color Profile Tests (Requires composition, ICC/ColorProfile mode)
# ============================================================================

@suite.test
def test_get_new_working_space_color_profile():
    """Test AEGP_GetNewWorkingSpaceColorProfile - gets working color profile"""
    global _test_comp
    if _test_comp is None:
        skip("No test comp")
        return
    if _is_ocio:
        skip("ColorProfile tests require non-OCIO mode")
        return

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)
    assert_true(compH != 0, "Should get valid comp handle")

    colorProfileP = sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
    assert_true(colorProfileP != 0, "Should get valid color profile")

    sdk.AEGP_DisposeColorProfile(colorProfileP)


@suite.test
def test_get_color_profile_description():
    """Test AEGP_GetNewColorProfileDescription - gets profile description"""
    global _test_comp
    if _test_comp is None:
        skip("No test comp")
        return
    if _is_ocio:
        skip("ColorProfile tests require non-OCIO mode")
        return

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)
    colorProfileP = sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

    if colorProfileP != 0:
        description = sdk.AEGP_GetNewColorProfileDescription(colorProfileP)
        assert_isinstance(description, str, "Should return string")
        print(f"Color profile description: '{description}'")

        sdk.AEGP_DisposeColorProfile(colorProfileP)


@suite.test
def test_get_color_profile_approximate_gamma():
    """Test AEGP_GetColorProfileApproximateGamma - gets approximate gamma"""
    global _test_comp
    if _test_comp is None:
        skip("No test comp")
        return
    if _is_ocio:
        skip("ColorProfile tests require non-OCIO mode")
        return

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)
    colorProfileP = sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

    if colorProfileP != 0:
        gamma = sdk.AEGP_GetColorProfileApproximateGamma(colorProfileP)
        assert_isinstance(gamma, float, "Should return float")
        assert_true(gamma > 0, "Gamma should be positive")
        print(f"Color profile approximate gamma: {gamma}")

        sdk.AEGP_DisposeColorProfile(colorProfileP)


@suite.test
def test_is_rgb_color_profile():
    """Test AEGP_IsRGBColorProfile - checks if profile is RGB"""
    global _test_comp
    if _test_comp is None:
        skip("No test comp")
        return
    if _is_ocio:
        skip("ColorProfile tests require non-OCIO mode")
        return

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)
    colorProfileP = sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

    if colorProfileP != 0:
        is_rgb = sdk.AEGP_IsRGBColorProfile(colorProfileP)
        assert_isinstance(is_rgb, bool, "Should return boolean")
        print(f"Is RGB color profile: {is_rgb}")

        sdk.AEGP_DisposeColorProfile(colorProfileP)


@suite.test
def test_get_icc_profile_from_color_profile():
    """Test AEGP_GetNewICCProfileFromColorProfile - extracts ICC profile data"""
    global _test_comp
    if _test_comp is None:
        skip("No test comp")
        return
    if _is_ocio:
        skip("ColorProfile tests require non-OCIO mode")
        return

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)
    colorProfileP = sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

    if colorProfileP != 0:
        icc_data = sdk.AEGP_GetNewICCProfileFromColorProfile(colorProfileP)
        assert_isinstance(icc_data, bytes, "Should return bytes")
        print(f"ICC profile size: {len(icc_data)} bytes")

        sdk.AEGP_DisposeColorProfile(colorProfileP)


@suite.test
def test_color_profile_icc_roundtrip():
    """Test ICC profile roundtrip - create profile from ICC data"""
    global _test_comp
    if _test_comp is None:
        skip("No test comp")
        return
    if _is_ocio:
        skip("ColorProfile tests require non-OCIO mode")
        return

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)
    original_profileP = sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

    if original_profileP != 0:
        icc_data = sdk.AEGP_GetNewICCProfileFromColorProfile(original_profileP)

        if len(icc_data) > 0:
            new_profileP = sdk.AEGP_GetNewColorProfileFromICCProfile(icc_data)
            assert_true(new_profileP != 0, "Should create profile from ICC data")

            orig_desc = sdk.AEGP_GetNewColorProfileDescription(original_profileP)
            new_desc = sdk.AEGP_GetNewColorProfileDescription(new_profileP)
            print(f"Original: '{orig_desc}', Recreated: '{new_desc}'")

            sdk.AEGP_DisposeColorProfile(new_profileP)

        sdk.AEGP_DisposeColorProfile(original_profileP)


@suite.test
def test_dispose_color_profile_null():
    """Test AEGP_DisposeColorProfile with null - should not raise"""
    sdk.AEGP_DisposeColorProfile(0)
    assert_true(True, "Disposing null profile should be safe")


@suite.test
def test_set_working_color_space():
    """Test AEGP_SetWorkingColorSpace - sets working color space"""
    global _test_comp
    if _test_comp is None:
        skip("No test comp")
        return
    if _is_ocio:
        skip("ColorProfile tests require non-OCIO mode")
        return

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)

    original_profileP = sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
    if original_profileP == 0:
        skip("Could not get working space profile")
        return

    orig_desc = sdk.AEGP_GetNewColorProfileDescription(original_profileP)
    print(f"Original working space: '{orig_desc}'")

    # Set the same profile back (to avoid changing project settings)
    sdk.AEGP_SetWorkingColorSpace(compH, original_profileP)

    verify_profileP = sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
    if verify_profileP != 0:
        verify_desc = sdk.AEGP_GetNewColorProfileDescription(verify_profileP)
        print(f"After set working space: '{verify_desc}'")
        sdk.AEGP_DisposeColorProfile(verify_profileP)

    sdk.AEGP_DisposeColorProfile(original_profileP)


# ============================================================================
# View-related Tests (Require AEGP_ItemViewP - may not be available)
# ============================================================================

@suite.test
def test_does_view_have_color_space_xform():
    """Test AEGP_DoesViewHaveColorSpaceXform - checks view xform (requires view)"""
    global _test_comp
    if _test_comp is None:
        skip("No test comp")
        return

    itemH = _test_comp._handle
    viewP = sdk.AEGP_GetItemMRUView(itemH)

    if viewP == 0:
        skip("No view available")
        return

    has_xform = sdk.AEGP_DoesViewHaveColorSpaceXform(viewP)
    assert_isinstance(has_xform, bool, "Should return boolean")
    print(f"View has color space xform: {has_xform}")


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
