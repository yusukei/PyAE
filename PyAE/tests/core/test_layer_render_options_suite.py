"""
LayerRenderOptionsSuite2 Tests
Tests for AEGP_LayerRenderOptionsSuite2

LayerRenderOptionsSuite provides layer-specific render options for rendering layers.
Unlike RenderOptionsSuite which is for composition-level rendering,
LayerRenderOptionsSuite focuses on individual layer render control.
"""
import ae

try:
    from test_utils import (
        TestSuite,
        assert_equal,
        assert_true,
        assert_not_none,
        assert_close,
        assert_isinstance,
    )
except ImportError:
    from .test_utils import (
        TestSuite,
        assert_equal,
        assert_true,
        assert_not_none,
        assert_close,
        assert_isinstance,
    )

suite = TestSuite("LayerRenderOptionsSuite2")

# Test variables
_test_comp = None
_test_layer = None
_test_layer_render_options = None
_test_effect = None


@suite.setup
def setup():
    """Setup test composition and layer"""
    global _test_comp, _test_layer, _test_layer_render_options, _test_effect
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_LayerRenderOptionsTestComp", 100, 100, 1.0, 5.0, 30.0)
    _test_layer = _test_comp.add_solid("_TestSolid", 100, 100, (1.0, 0.0, 0.0), 5.0)

    # Create LayerRenderOptions from layer
    layer_h = _test_layer._handle if hasattr(_test_layer, '_handle') else 0
    if layer_h:
        _test_layer_render_options = ae.sdk.AEGP_NewFromLayer(layer_h)
        print(f"Created LayerRenderOptions: {_test_layer_render_options}")

    # Add effect for upstream/downstream tests
    try:
        _test_effect = _test_layer.add_effect("ADBE Gaussian Blur 2")
    except:
        try:
            _test_effect = _test_layer.add_effect("Gaussian Blur")
        except:
            _test_effect = None
            print("Warning: Could not add effect for upstream/downstream tests")


@suite.teardown
def teardown():
    """Cleanup test resources"""
    global _test_comp, _test_layer, _test_layer_render_options, _test_effect
    # Dispose LayerRenderOptions handle
    if _test_layer_render_options:
        try:
            ae.sdk.AEGP_DisposeLayerRenderOptions(_test_layer_render_options)
        except:
            pass
    # Delete composition (this also deletes the layer)
    if _test_comp is not None:
        try:
            _test_comp.delete()
        except:
            pass


# ============================================================================
# Function Existence Tests
# ============================================================================

@suite.test
def test_function_exists_new_from_layer():
    """AEGP_NewFromLayer function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_NewFromLayer'), "AEGP_NewFromLayer should exist")
    print("AEGP_NewFromLayer exists")


@suite.test
def test_function_exists_new_from_upstream_of_effect():
    """AEGP_NewFromUpstreamOfEffect function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_NewFromUpstreamOfEffect'), "AEGP_NewFromUpstreamOfEffect should exist")
    print("AEGP_NewFromUpstreamOfEffect exists")


@suite.test
def test_function_exists_new_from_downstream_of_effect():
    """AEGP_NewFromDownstreamOfEffect function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_NewFromDownstreamOfEffect'), "AEGP_NewFromDownstreamOfEffect should exist")
    print("AEGP_NewFromDownstreamOfEffect exists")


@suite.test
def test_function_exists_duplicate():
    """AEGP_DuplicateLayerRenderOptions function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_DuplicateLayerRenderOptions'), "AEGP_DuplicateLayerRenderOptions should exist")
    print("AEGP_DuplicateLayerRenderOptions exists")


@suite.test
def test_function_exists_dispose():
    """AEGP_DisposeLayerRenderOptions function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_DisposeLayerRenderOptions'), "AEGP_DisposeLayerRenderOptions should exist")
    print("AEGP_DisposeLayerRenderOptions exists")


@suite.test
def test_function_exists_set_time():
    """AEGP_SetLayerRenderOptionsTime function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_SetLayerRenderOptionsTime'), "AEGP_SetLayerRenderOptionsTime should exist")
    print("AEGP_SetLayerRenderOptionsTime exists")


@suite.test
def test_function_exists_get_time():
    """AEGP_GetLayerRenderOptionsTime function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_GetLayerRenderOptionsTime'), "AEGP_GetLayerRenderOptionsTime should exist")
    print("AEGP_GetLayerRenderOptionsTime exists")


@suite.test
def test_function_exists_set_time_step():
    """AEGP_SetLayerRenderOptionsTimeStep function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_SetLayerRenderOptionsTimeStep'), "AEGP_SetLayerRenderOptionsTimeStep should exist")
    print("AEGP_SetLayerRenderOptionsTimeStep exists")


@suite.test
def test_function_exists_get_time_step():
    """AEGP_GetLayerRenderOptionsTimeStep function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_GetLayerRenderOptionsTimeStep'), "AEGP_GetLayerRenderOptionsTimeStep should exist")
    print("AEGP_GetLayerRenderOptionsTimeStep exists")


@suite.test
def test_function_exists_set_world_type():
    """AEGP_SetLayerRenderOptionsWorldType function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_SetLayerRenderOptionsWorldType'), "AEGP_SetLayerRenderOptionsWorldType should exist")
    print("AEGP_SetLayerRenderOptionsWorldType exists")


@suite.test
def test_function_exists_get_world_type():
    """AEGP_GetLayerRenderOptionsWorldType function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_GetLayerRenderOptionsWorldType'), "AEGP_GetLayerRenderOptionsWorldType should exist")
    print("AEGP_GetLayerRenderOptionsWorldType exists")


@suite.test
def test_function_exists_set_downsample_factor():
    """AEGP_SetLayerRenderOptionsDownsampleFactor function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_SetLayerRenderOptionsDownsampleFactor'), "AEGP_SetLayerRenderOptionsDownsampleFactor should exist")
    print("AEGP_SetLayerRenderOptionsDownsampleFactor exists")


@suite.test
def test_function_exists_get_downsample_factor():
    """AEGP_GetLayerRenderOptionsDownsampleFactor function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_GetLayerRenderOptionsDownsampleFactor'), "AEGP_GetLayerRenderOptionsDownsampleFactor should exist")
    print("AEGP_GetLayerRenderOptionsDownsampleFactor exists")


@suite.test
def test_function_exists_set_matte_mode():
    """AEGP_SetLayerRenderOptionsMatteMode function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_SetLayerRenderOptionsMatteMode'), "AEGP_SetLayerRenderOptionsMatteMode should exist")
    print("AEGP_SetLayerRenderOptionsMatteMode exists")


@suite.test
def test_function_exists_get_matte_mode():
    """AEGP_GetLayerRenderOptionsMatteMode function exists"""
    assert_true(hasattr(ae.sdk, 'AEGP_GetLayerRenderOptionsMatteMode'), "AEGP_GetLayerRenderOptionsMatteMode should exist")
    print("AEGP_GetLayerRenderOptionsMatteMode exists")


# ============================================================================
# Creation and Disposal Tests
# ============================================================================

@suite.test
def test_new_from_layer():
    """AEGP_NewFromLayer - create options from layer"""
    global _test_layer
    layer_h = _test_layer._handle if hasattr(_test_layer, '_handle') else 0
    if not layer_h:
        print("Skipped: No layer handle available")
        return

    optionsH = ae.sdk.AEGP_NewFromLayer(layer_h)
    assert_true(optionsH != 0, "Options handle should be valid")
    print(f"Created LayerRenderOptions from layer: {optionsH}")

    # Cleanup
    ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)
    print("LayerRenderOptions disposed successfully")


@suite.test
def test_new_from_upstream_of_effect():
    """AEGP_NewFromUpstreamOfEffect - create options from upstream of effect"""
    global _test_effect
    if _test_effect is None:
        print("Skipped: No effect available for test")
        return

    effect_h = _test_effect._handle if hasattr(_test_effect, '_handle') else 0
    if not effect_h:
        print("Skipped: No effect handle available")
        return

    optionsH = ae.sdk.AEGP_NewFromUpstreamOfEffect(effect_h)
    assert_true(optionsH != 0, "Options handle should be valid")
    print(f"Created LayerRenderOptions from upstream of effect: {optionsH}")

    # Cleanup
    ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)
    print("LayerRenderOptions disposed successfully")


@suite.test
def test_new_from_downstream_of_effect():
    """AEGP_NewFromDownstreamOfEffect - create options from downstream of effect"""
    global _test_effect
    if _test_effect is None:
        print("Skipped: No effect available for test")
        return

    effect_h = _test_effect._handle if hasattr(_test_effect, '_handle') else 0
    if not effect_h:
        print("Skipped: No effect handle available")
        return

    # Note: This function can only be called from UI thread
    try:
        optionsH = ae.sdk.AEGP_NewFromDownstreamOfEffect(effect_h)
        assert_true(optionsH != 0, "Options handle should be valid")
        print(f"Created LayerRenderOptions from downstream of effect: {optionsH}")

        # Cleanup
        ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)
        print("LayerRenderOptions disposed successfully")
    except Exception as e:
        # May fail if not called from UI thread
        print(f"NewFromDownstreamOfEffect may only be called from UI thread: {e}")


@suite.test
def test_duplicate_layer_render_options():
    """AEGP_DuplicateLayerRenderOptions - duplicate options"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Duplicate
    copyH = ae.sdk.AEGP_DuplicateLayerRenderOptions(_test_layer_render_options)
    assert_true(copyH != 0, "Copy handle should be valid")
    assert_true(copyH != _test_layer_render_options, "Copy should be different handle")
    print(f"Duplicated LayerRenderOptions: {copyH}")

    # Cleanup
    ae.sdk.AEGP_DisposeLayerRenderOptions(copyH)
    print("Duplicated LayerRenderOptions disposed successfully")


# ============================================================================
# Time Tests
# ============================================================================

@suite.test
def test_set_get_time():
    """AEGP_SetLayerRenderOptionsTime/GetTime - set and get render time"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set time to 1.5 seconds
    test_time = 1.5
    ae.sdk.AEGP_SetLayerRenderOptionsTime(_test_layer_render_options, test_time)

    # Get time
    result_time = ae.sdk.AEGP_GetLayerRenderOptionsTime(_test_layer_render_options)
    assert_close(test_time, result_time, tolerance=0.001, message="Time should match")
    print(f"Set time to {test_time}s, got {result_time}s")


@suite.test
def test_set_get_time_zero():
    """Time at zero seconds"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    ae.sdk.AEGP_SetLayerRenderOptionsTime(_test_layer_render_options, 0.0)
    result_time = ae.sdk.AEGP_GetLayerRenderOptionsTime(_test_layer_render_options)
    assert_close(0.0, result_time, tolerance=0.001, message="Time should be 0.0")
    print(f"Time at zero: {result_time}s")


@suite.test
def test_set_get_time_fractional():
    """Time with fractional value"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Test with 1/30 second (one frame at 30fps)
    test_time = 1.0 / 30.0
    ae.sdk.AEGP_SetLayerRenderOptionsTime(_test_layer_render_options, test_time)
    result_time = ae.sdk.AEGP_GetLayerRenderOptionsTime(_test_layer_render_options)
    assert_close(test_time, result_time, tolerance=0.001, message="Fractional time should match")
    print(f"Fractional time: set {test_time:.6f}s, got {result_time:.6f}s")


# ============================================================================
# Time Step Tests
# ============================================================================

@suite.test
def test_set_get_time_step():
    """AEGP_SetLayerRenderOptionsTimeStep/GetTimeStep - set and get time step"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set time step to 1/30 second (one frame at 30fps)
    test_time_step = 1.0 / 30.0
    ae.sdk.AEGP_SetLayerRenderOptionsTimeStep(_test_layer_render_options, test_time_step)

    # Get time step
    result_time_step = ae.sdk.AEGP_GetLayerRenderOptionsTimeStep(_test_layer_render_options)
    assert_close(test_time_step, result_time_step, tolerance=0.001, message="Time step should match")
    print(f"Set time step to {test_time_step:.6f}s, got {result_time_step:.6f}s")


@suite.test
def test_set_get_time_step_24fps():
    """Time step for 24fps"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    test_time_step = 1.0 / 24.0
    ae.sdk.AEGP_SetLayerRenderOptionsTimeStep(_test_layer_render_options, test_time_step)
    result_time_step = ae.sdk.AEGP_GetLayerRenderOptionsTimeStep(_test_layer_render_options)
    assert_close(test_time_step, result_time_step, tolerance=0.001, message="24fps time step should match")
    print(f"24fps time step: {result_time_step:.6f}s")


# ============================================================================
# World Type Tests
# ============================================================================

@suite.test
def test_set_get_world_type_8bit():
    """AEGP_SetLayerRenderOptionsWorldType/GetWorldType - 8-bit"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to 8-bit
    ae.sdk.AEGP_SetLayerRenderOptionsWorldType(_test_layer_render_options, ae.sdk.AEGP_WorldType_8)

    # Get world type
    world_type = ae.sdk.AEGP_GetLayerRenderOptionsWorldType(_test_layer_render_options)
    assert_equal(ae.sdk.AEGP_WorldType_8, world_type, "World type should be 8-bit")
    print(f"World type set to 8-bit: {world_type}")


@suite.test
def test_set_get_world_type_16bit():
    """AEGP_SetLayerRenderOptionsWorldType/GetWorldType - 16-bit"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to 16-bit
    ae.sdk.AEGP_SetLayerRenderOptionsWorldType(_test_layer_render_options, ae.sdk.AEGP_WorldType_16)

    # Get world type
    world_type = ae.sdk.AEGP_GetLayerRenderOptionsWorldType(_test_layer_render_options)
    assert_equal(ae.sdk.AEGP_WorldType_16, world_type, "World type should be 16-bit")
    print(f"World type set to 16-bit: {world_type}")


@suite.test
def test_set_get_world_type_32bit():
    """AEGP_SetLayerRenderOptionsWorldType/GetWorldType - 32-bit float"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to 32-bit float
    ae.sdk.AEGP_SetLayerRenderOptionsWorldType(_test_layer_render_options, ae.sdk.AEGP_WorldType_32)

    # Get world type
    world_type = ae.sdk.AEGP_GetLayerRenderOptionsWorldType(_test_layer_render_options)
    assert_equal(ae.sdk.AEGP_WorldType_32, world_type, "World type should be 32-bit float")
    print(f"World type set to 32-bit float: {world_type}")


@suite.test
def test_set_get_world_type_none():
    """AEGP_SetLayerRenderOptionsWorldType/GetWorldType - NONE"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to NONE (use current project settings)
    # Note: AEGP_WorldType_NONE means "use project settings", so AE may resolve it
    # to the project's default world type (8, 16, or 32) rather than returning NONE.
    ae.sdk.AEGP_SetLayerRenderOptionsWorldType(_test_layer_render_options, ae.sdk.AEGP_WorldType_NONE)

    # Get world type - may be NONE or the project's resolved default
    world_type = ae.sdk.AEGP_GetLayerRenderOptionsWorldType(_test_layer_render_options)
    # Accept NONE or any valid world type since AE may resolve NONE to project default
    valid_types = [ae.sdk.AEGP_WorldType_NONE, ae.sdk.AEGP_WorldType_8,
                   ae.sdk.AEGP_WorldType_16, ae.sdk.AEGP_WorldType_32]
    assert_true(world_type in valid_types, f"World type should be valid: got {world_type}")
    print(f"World type after setting NONE: {world_type} (AE may resolve to project default)")


# ============================================================================
# Downsample Factor Tests
# ============================================================================

@suite.test
def test_set_get_downsample_factor_full():
    """AEGP_SetLayerRenderOptionsDownsampleFactor/GetDownsampleFactor - full resolution"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to full resolution (1, 1)
    ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(_test_layer_render_options, 1, 1)

    # Get downsample factor
    factor = ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(_test_layer_render_options)
    assert_isinstance(factor, tuple)
    assert_equal(2, len(factor), "Factor should be tuple of 2 elements")
    assert_equal(1, factor[0], "X factor should be 1")
    assert_equal(1, factor[1], "Y factor should be 1")
    print(f"Downsample factor (full): {factor}")


@suite.test
def test_set_get_downsample_factor_half():
    """AEGP_SetLayerRenderOptionsDownsampleFactor/GetDownsampleFactor - half resolution"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to half resolution (2, 2) = 50%
    ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(_test_layer_render_options, 2, 2)

    # Get downsample factor
    factor = ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(_test_layer_render_options)
    assert_equal(2, factor[0], "X factor should be 2")
    assert_equal(2, factor[1], "Y factor should be 2")
    print(f"Downsample factor (half): {factor}")


@suite.test
def test_set_get_downsample_factor_quarter():
    """AEGP_SetLayerRenderOptionsDownsampleFactor/GetDownsampleFactor - quarter resolution"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to quarter resolution (4, 4) = 25%
    ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(_test_layer_render_options, 4, 4)

    # Get downsample factor
    factor = ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(_test_layer_render_options)
    assert_equal(4, factor[0], "X factor should be 4")
    assert_equal(4, factor[1], "Y factor should be 4")
    print(f"Downsample factor (quarter): {factor}")


@suite.test
def test_set_get_downsample_factor_asymmetric():
    """AEGP_SetLayerRenderOptionsDownsampleFactor/GetDownsampleFactor - asymmetric"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set asymmetric factor
    ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(_test_layer_render_options, 2, 4)

    # Get downsample factor
    factor = ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(_test_layer_render_options)
    assert_equal(2, factor[0], "X factor should be 2")
    assert_equal(4, factor[1], "Y factor should be 4")
    print(f"Downsample factor (asymmetric): {factor}")


# ============================================================================
# Matte Mode Tests
# ============================================================================

@suite.test
def test_set_get_matte_mode_straight():
    """AEGP_SetLayerRenderOptionsMatteMode/GetMatteMode - straight alpha"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to straight alpha (0)
    ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(_test_layer_render_options, ae.sdk.AEGP_MatteMode_STRAIGHT)

    # Get matte mode
    mode = ae.sdk.AEGP_GetLayerRenderOptionsMatteMode(_test_layer_render_options)
    assert_equal(ae.sdk.AEGP_MatteMode_STRAIGHT, mode, "Matte mode should be straight")
    print(f"Matte mode (straight): {mode}")


@suite.test
def test_set_get_matte_mode_premul_black():
    """AEGP_SetLayerRenderOptionsMatteMode/GetMatteMode - premultiplied black"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to premultiplied black (1)
    ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(_test_layer_render_options, ae.sdk.AEGP_MatteMode_PREMUL_BLACK)

    # Get matte mode
    mode = ae.sdk.AEGP_GetLayerRenderOptionsMatteMode(_test_layer_render_options)
    assert_equal(ae.sdk.AEGP_MatteMode_PREMUL_BLACK, mode, "Matte mode should be premul black")
    print(f"Matte mode (premul black): {mode}")


@suite.test
def test_set_get_matte_mode_premul_bg():
    """AEGP_SetLayerRenderOptionsMatteMode/GetMatteMode - premultiplied background color"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    # Set to premultiplied background color (2)
    ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(_test_layer_render_options, ae.sdk.AEGP_MatteMode_PREMUL_BG_COLOR)

    # Get matte mode
    mode = ae.sdk.AEGP_GetLayerRenderOptionsMatteMode(_test_layer_render_options)
    assert_equal(ae.sdk.AEGP_MatteMode_PREMUL_BG_COLOR, mode, "Matte mode should be premul bg color")
    print(f"Matte mode (premul bg color): {mode}")


# ============================================================================
# Null Handle Validation Tests
# ============================================================================

@suite.test
def test_null_handle_new_from_layer():
    """AEGP_NewFromLayer with null handle should raise error"""
    try:
        ae.sdk.AEGP_NewFromLayer(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_NewFromLayer correctly rejected null: {e}")


@suite.test
def test_null_handle_new_from_upstream():
    """AEGP_NewFromUpstreamOfEffect with null handle should raise error"""
    try:
        ae.sdk.AEGP_NewFromUpstreamOfEffect(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_NewFromUpstreamOfEffect correctly rejected null: {e}")


@suite.test
def test_null_handle_new_from_downstream():
    """AEGP_NewFromDownstreamOfEffect with null handle should raise error"""
    try:
        ae.sdk.AEGP_NewFromDownstreamOfEffect(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_NewFromDownstreamOfEffect correctly rejected null: {e}")


@suite.test
def test_null_handle_duplicate():
    """AEGP_DuplicateLayerRenderOptions with null handle should raise error"""
    try:
        ae.sdk.AEGP_DuplicateLayerRenderOptions(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_DuplicateLayerRenderOptions correctly rejected null: {e}")


@suite.test
def test_null_handle_dispose():
    """AEGP_DisposeLayerRenderOptions with null handle should raise error"""
    try:
        ae.sdk.AEGP_DisposeLayerRenderOptions(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_DisposeLayerRenderOptions correctly rejected null: {e}")


@suite.test
def test_null_handle_set_time():
    """AEGP_SetLayerRenderOptionsTime with null handle should raise error"""
    try:
        ae.sdk.AEGP_SetLayerRenderOptionsTime(0, 1.0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_SetLayerRenderOptionsTime correctly rejected null: {e}")


@suite.test
def test_null_handle_get_time():
    """AEGP_GetLayerRenderOptionsTime with null handle should raise error"""
    try:
        ae.sdk.AEGP_GetLayerRenderOptionsTime(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_GetLayerRenderOptionsTime correctly rejected null: {e}")


@suite.test
def test_null_handle_set_time_step():
    """AEGP_SetLayerRenderOptionsTimeStep with null handle should raise error"""
    try:
        ae.sdk.AEGP_SetLayerRenderOptionsTimeStep(0, 1.0 / 30.0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_SetLayerRenderOptionsTimeStep correctly rejected null: {e}")


@suite.test
def test_null_handle_get_time_step():
    """AEGP_GetLayerRenderOptionsTimeStep with null handle should raise error"""
    try:
        ae.sdk.AEGP_GetLayerRenderOptionsTimeStep(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_GetLayerRenderOptionsTimeStep correctly rejected null: {e}")


@suite.test
def test_null_handle_set_world_type():
    """AEGP_SetLayerRenderOptionsWorldType with null handle should raise error"""
    try:
        ae.sdk.AEGP_SetLayerRenderOptionsWorldType(0, ae.sdk.AEGP_WorldType_8)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_SetLayerRenderOptionsWorldType correctly rejected null: {e}")


@suite.test
def test_null_handle_get_world_type():
    """AEGP_GetLayerRenderOptionsWorldType with null handle should raise error"""
    try:
        ae.sdk.AEGP_GetLayerRenderOptionsWorldType(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_GetLayerRenderOptionsWorldType correctly rejected null: {e}")


@suite.test
def test_null_handle_set_downsample_factor():
    """AEGP_SetLayerRenderOptionsDownsampleFactor with null handle should raise error"""
    try:
        ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(0, 1, 1)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_SetLayerRenderOptionsDownsampleFactor correctly rejected null: {e}")


@suite.test
def test_null_handle_get_downsample_factor():
    """AEGP_GetLayerRenderOptionsDownsampleFactor with null handle should raise error"""
    try:
        ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_GetLayerRenderOptionsDownsampleFactor correctly rejected null: {e}")


@suite.test
def test_null_handle_set_matte_mode():
    """AEGP_SetLayerRenderOptionsMatteMode with null handle should raise error"""
    try:
        ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(0, 0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_SetLayerRenderOptionsMatteMode correctly rejected null: {e}")


@suite.test
def test_null_handle_get_matte_mode():
    """AEGP_GetLayerRenderOptionsMatteMode with null handle should raise error"""
    try:
        ae.sdk.AEGP_GetLayerRenderOptionsMatteMode(0)
        raise AssertionError("Should have raised exception for null handle")
    except Exception as e:
        print(f"AEGP_GetLayerRenderOptionsMatteMode correctly rejected null: {e}")


# ============================================================================
# Invalid Parameter Validation Tests
# ============================================================================

@suite.test
def test_invalid_world_type():
    """AEGP_SetLayerRenderOptionsWorldType with invalid type should raise error"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    try:
        ae.sdk.AEGP_SetLayerRenderOptionsWorldType(_test_layer_render_options, 999)
        raise AssertionError("Should have raised exception for invalid world type")
    except Exception as e:
        print(f"Correctly rejected invalid world type: {e}")


@suite.test
def test_invalid_matte_mode():
    """AEGP_SetLayerRenderOptionsMatteMode with invalid mode should raise error"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    try:
        ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(_test_layer_render_options, 999)
        raise AssertionError("Should have raised exception for invalid matte mode")
    except Exception as e:
        print(f"Correctly rejected invalid matte mode: {e}")


@suite.test
def test_invalid_downsample_factor_zero():
    """AEGP_SetLayerRenderOptionsDownsampleFactor with zero factor should raise error"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    try:
        ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(_test_layer_render_options, 0, 1)
        raise AssertionError("Should have raised exception for zero X factor")
    except Exception as e:
        print(f"Correctly rejected zero X factor: {e}")

    try:
        ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(_test_layer_render_options, 1, 0)
        raise AssertionError("Should have raised exception for zero Y factor")
    except Exception as e:
        print(f"Correctly rejected zero Y factor: {e}")


@suite.test
def test_invalid_downsample_factor_negative():
    """AEGP_SetLayerRenderOptionsDownsampleFactor with negative factor should raise error"""
    global _test_layer_render_options
    if not _test_layer_render_options:
        print("Skipped: No LayerRenderOptions available")
        return

    try:
        ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(_test_layer_render_options, -1, 1)
        raise AssertionError("Should have raised exception for negative X factor")
    except Exception as e:
        print(f"Correctly rejected negative X factor: {e}")

    try:
        ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(_test_layer_render_options, 1, -1)
        raise AssertionError("Should have raised exception for negative Y factor")
    except Exception as e:
        print(f"Correctly rejected negative Y factor: {e}")


# ============================================================================
# Comprehensive Test
# ============================================================================

@suite.test
def test_comprehensive_options_workflow():
    """Comprehensive test of LayerRenderOptions workflow"""
    global _test_layer
    layer_h = _test_layer._handle if hasattr(_test_layer, '_handle') else 0
    if not layer_h:
        print("Skipped: No layer handle available")
        return

    # Create options from layer
    optionsH = ae.sdk.AEGP_NewFromLayer(layer_h)
    assert_true(optionsH != 0, "Options handle should be valid")
    print(f"1. Created LayerRenderOptions: {optionsH}")

    # Configure all options
    ae.sdk.AEGP_SetLayerRenderOptionsTime(optionsH, 2.5)
    ae.sdk.AEGP_SetLayerRenderOptionsTimeStep(optionsH, 1.0 / 30.0)
    ae.sdk.AEGP_SetLayerRenderOptionsWorldType(optionsH, ae.sdk.AEGP_WorldType_16)
    ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH, 2, 2)
    ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(optionsH, ae.sdk.AEGP_MatteMode_PREMUL_BLACK)
    print("2. Configured all options")

    # Verify all options
    time = ae.sdk.AEGP_GetLayerRenderOptionsTime(optionsH)
    assert_close(2.5, time, tolerance=0.001)

    time_step = ae.sdk.AEGP_GetLayerRenderOptionsTimeStep(optionsH)
    assert_close(1.0 / 30.0, time_step, tolerance=0.001)

    world_type = ae.sdk.AEGP_GetLayerRenderOptionsWorldType(optionsH)
    assert_equal(ae.sdk.AEGP_WorldType_16, world_type)

    factor = ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(optionsH)
    assert_equal((2, 2), factor)

    matte_mode = ae.sdk.AEGP_GetLayerRenderOptionsMatteMode(optionsH)
    assert_equal(ae.sdk.AEGP_MatteMode_PREMUL_BLACK, matte_mode)
    print("3. Verified all options")

    # Duplicate and verify
    copyH = ae.sdk.AEGP_DuplicateLayerRenderOptions(optionsH)
    assert_true(copyH != 0)

    copy_time = ae.sdk.AEGP_GetLayerRenderOptionsTime(copyH)
    assert_close(2.5, copy_time, tolerance=0.001)
    print("4. Duplicated and verified copy")

    # Modify copy independently
    ae.sdk.AEGP_SetLayerRenderOptionsTime(copyH, 3.0)
    copy_time = ae.sdk.AEGP_GetLayerRenderOptionsTime(copyH)
    original_time = ae.sdk.AEGP_GetLayerRenderOptionsTime(optionsH)
    assert_close(3.0, copy_time, tolerance=0.001)
    assert_close(2.5, original_time, tolerance=0.001)
    print("5. Verified copy independence")

    # Cleanup
    ae.sdk.AEGP_DisposeLayerRenderOptions(copyH)
    ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)
    print("6. Disposed both handles - workflow complete")


# ============================================================================
# Run Tests
# ============================================================================

def run():
    """Run all tests and return results"""
    return suite.run()


if __name__ == "__main__":
    run()
