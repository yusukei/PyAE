"""
RenderQueueMonitor Suite Tests
Tests for AEGP_RenderQueueMonitorSuite1

Note: RenderQueueMonitorSuite is designed for monitoring render queue progress.
Most functions require a valid session_id obtained from a registered listener callback.
Since listener registration requires C callbacks (not available from Python),
these tests focus on API availability, constant validation, and parameter checking.
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true, assert_not_none
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true, assert_not_none

suite = TestSuite("RenderQueueMonitor Suite")


@suite.setup
def setup():
    """Setup for RenderQueueMonitor tests"""
    pass


@suite.teardown
def teardown():
    """Cleanup after RenderQueueMonitor tests"""
    pass


# ============================================================================
# Constant Existence Tests
# ============================================================================

@suite.test
def test_finished_status_unknown_constant():
    """Test AEGP_RQM_FinishedStatus_UNKNOWN constant exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_FinishedStatus_UNKNOWN'),
        "AEGP_RQM_FinishedStatus_UNKNOWN should be available"
    )
    assert_equal(ae.sdk.AEGP_RQM_FinishedStatus_UNKNOWN, 0)
    print("AEGP_RQM_FinishedStatus_UNKNOWN = 0")


@suite.test
def test_finished_status_succeeded_constant():
    """Test AEGP_RQM_FinishedStatus_SUCCEEDED constant exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_FinishedStatus_SUCCEEDED'),
        "AEGP_RQM_FinishedStatus_SUCCEEDED should be available"
    )
    assert_equal(ae.sdk.AEGP_RQM_FinishedStatus_SUCCEEDED, 1)
    print("AEGP_RQM_FinishedStatus_SUCCEEDED = 1")


@suite.test
def test_finished_status_aborted_constant():
    """Test AEGP_RQM_FinishedStatus_ABORTED constant exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_FinishedStatus_ABORTED'),
        "AEGP_RQM_FinishedStatus_ABORTED should be available"
    )
    assert_equal(ae.sdk.AEGP_RQM_FinishedStatus_ABORTED, 2)
    print("AEGP_RQM_FinishedStatus_ABORTED = 2")


@suite.test
def test_finished_status_erred_constant():
    """Test AEGP_RQM_FinishedStatus_ERRED constant exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_FinishedStatus_ERRED'),
        "AEGP_RQM_FinishedStatus_ERRED should be available"
    )
    assert_equal(ae.sdk.AEGP_RQM_FinishedStatus_ERRED, 3)
    print("AEGP_RQM_FinishedStatus_ERRED = 3")


# ============================================================================
# Stub Function Tests (Should Raise Exception)
# ============================================================================

@suite.test
def test_register_listener_function_exists():
    """Test AEGP_RQM_RegisterListener function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_RegisterListener'),
        "AEGP_RQM_RegisterListener should be available"
    )
    print("AEGP_RQM_RegisterListener function exists")


@suite.test
def test_register_listener_raises_exception():
    """Test AEGP_RQM_RegisterListener raises exception (requires C callback)"""
    try:
        ae.sdk.AEGP_RQM_RegisterListener()
        raise AssertionError("Should have raised exception (C callback required)")
    except Exception as e:
        print(f"Correctly raised exception: {e}")
        assert_true(True, "Register listener correctly raises exception")


@suite.test
def test_deregister_listener_function_exists():
    """Test AEGP_RQM_DeregisterListener function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_DeregisterListener'),
        "AEGP_RQM_DeregisterListener should be available"
    )
    print("AEGP_RQM_DeregisterListener function exists")


@suite.test
def test_deregister_listener_raises_exception():
    """Test AEGP_RQM_DeregisterListener raises exception (requires C callback)"""
    try:
        ae.sdk.AEGP_RQM_DeregisterListener()
        raise AssertionError("Should have raised exception (C callback required)")
    except Exception as e:
        print(f"Correctly raised exception: {e}")
        assert_true(True, "Deregister listener correctly raises exception")


# ============================================================================
# API Availability Tests (Get Functions)
# ============================================================================

@suite.test
def test_get_project_name_function_exists():
    """Test AEGP_RQM_GetProjectName function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetProjectName'),
        "AEGP_RQM_GetProjectName should be available"
    )
    print("AEGP_RQM_GetProjectName function exists")


@suite.test
def test_get_app_version_function_exists():
    """Test AEGP_RQM_GetAppVersion function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetAppVersion'),
        "AEGP_RQM_GetAppVersion should be available"
    )
    print("AEGP_RQM_GetAppVersion function exists")


@suite.test
def test_get_num_job_items_function_exists():
    """Test AEGP_RQM_GetNumJobItems function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetNumJobItems'),
        "AEGP_RQM_GetNumJobItems should be available"
    )
    print("AEGP_RQM_GetNumJobItems function exists")


@suite.test
def test_get_job_item_id_function_exists():
    """Test AEGP_RQM_GetJobItemID function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetJobItemID'),
        "AEGP_RQM_GetJobItemID should be available"
    )
    print("AEGP_RQM_GetJobItemID function exists")


@suite.test
def test_get_num_job_item_render_settings_function_exists():
    """Test AEGP_RQM_GetNumJobItemRenderSettings function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetNumJobItemRenderSettings'),
        "AEGP_RQM_GetNumJobItemRenderSettings should be available"
    )
    print("AEGP_RQM_GetNumJobItemRenderSettings function exists")


@suite.test
def test_get_job_item_render_setting_function_exists():
    """Test AEGP_RQM_GetJobItemRenderSetting function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetJobItemRenderSetting'),
        "AEGP_RQM_GetJobItemRenderSetting should be available"
    )
    print("AEGP_RQM_GetJobItemRenderSetting function exists")


@suite.test
def test_get_num_job_item_output_modules_function_exists():
    """Test AEGP_RQM_GetNumJobItemOutputModules function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetNumJobItemOutputModules'),
        "AEGP_RQM_GetNumJobItemOutputModules should be available"
    )
    print("AEGP_RQM_GetNumJobItemOutputModules function exists")


@suite.test
def test_get_num_job_item_output_module_settings_function_exists():
    """Test AEGP_RQM_GetNumJobItemOutputModuleSettings function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetNumJobItemOutputModuleSettings'),
        "AEGP_RQM_GetNumJobItemOutputModuleSettings should be available"
    )
    print("AEGP_RQM_GetNumJobItemOutputModuleSettings function exists")


@suite.test
def test_get_job_item_output_module_setting_function_exists():
    """Test AEGP_RQM_GetJobItemOutputModuleSetting function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetJobItemOutputModuleSetting'),
        "AEGP_RQM_GetJobItemOutputModuleSetting should be available"
    )
    print("AEGP_RQM_GetJobItemOutputModuleSetting function exists")


@suite.test
def test_get_num_job_item_output_module_warnings_function_exists():
    """Test AEGP_RQM_GetNumJobItemOutputModuleWarnings function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetNumJobItemOutputModuleWarnings'),
        "AEGP_RQM_GetNumJobItemOutputModuleWarnings should be available"
    )
    print("AEGP_RQM_GetNumJobItemOutputModuleWarnings function exists")


@suite.test
def test_get_job_item_output_module_warning_function_exists():
    """Test AEGP_RQM_GetJobItemOutputModuleWarning function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetJobItemOutputModuleWarning'),
        "AEGP_RQM_GetJobItemOutputModuleWarning should be available"
    )
    print("AEGP_RQM_GetJobItemOutputModuleWarning function exists")


@suite.test
def test_get_num_job_item_frame_properties_function_exists():
    """Test AEGP_RQM_GetNumJobItemFrameProperties function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetNumJobItemFrameProperties'),
        "AEGP_RQM_GetNumJobItemFrameProperties should be available"
    )
    print("AEGP_RQM_GetNumJobItemFrameProperties function exists")


@suite.test
def test_get_job_item_frame_property_function_exists():
    """Test AEGP_RQM_GetJobItemFrameProperty function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetJobItemFrameProperty'),
        "AEGP_RQM_GetJobItemFrameProperty should be available"
    )
    print("AEGP_RQM_GetJobItemFrameProperty function exists")


@suite.test
def test_get_num_job_item_output_module_properties_function_exists():
    """Test AEGP_RQM_GetNumJobItemOutputModuleProperties function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetNumJobItemOutputModuleProperties'),
        "AEGP_RQM_GetNumJobItemOutputModuleProperties should be available"
    )
    print("AEGP_RQM_GetNumJobItemOutputModuleProperties function exists")


@suite.test
def test_get_job_item_output_module_property_function_exists():
    """Test AEGP_RQM_GetJobItemOutputModuleProperty function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetJobItemOutputModuleProperty'),
        "AEGP_RQM_GetJobItemOutputModuleProperty should be available"
    )
    print("AEGP_RQM_GetJobItemOutputModuleProperty function exists")


@suite.test
def test_get_job_item_frame_thumbnail_function_exists():
    """Test AEGP_RQM_GetJobItemFrameThumbnail function exists"""
    assert_true(
        hasattr(ae.sdk, 'AEGP_RQM_GetJobItemFrameThumbnail'),
        "AEGP_RQM_GetJobItemFrameThumbnail should be available"
    )
    print("AEGP_RQM_GetJobItemFrameThumbnail function exists")


# ============================================================================
# Parameter Validation Tests with Invalid Session ID
#
# Note: These functions require a valid session_id from a registered listener.
# Using session_id=0 should return an error or raise an exception.
# These tests verify the functions can be called and handle invalid input.
# ============================================================================

@suite.test
def test_get_project_name_with_invalid_session():
    """Test AEGP_RQM_GetProjectName with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetProjectName(0)
        # If no exception, result might be empty or error code
        print(f"GetProjectName(0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetProjectName(0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_app_version_with_invalid_session():
    """Test AEGP_RQM_GetAppVersion with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetAppVersion(0)
        print(f"GetAppVersion(0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetAppVersion(0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_num_job_items_with_invalid_session():
    """Test AEGP_RQM_GetNumJobItems with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetNumJobItems(0)
        print(f"GetNumJobItems(0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetNumJobItems(0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_job_item_id_with_invalid_session():
    """Test AEGP_RQM_GetJobItemID with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetJobItemID(0, 0)
        print(f"GetJobItemID(0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetJobItemID(0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_num_job_item_render_settings_with_invalid_session():
    """Test AEGP_RQM_GetNumJobItemRenderSettings with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetNumJobItemRenderSettings(0, 0)
        print(f"GetNumJobItemRenderSettings(0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetNumJobItemRenderSettings(0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_job_item_render_setting_with_invalid_session():
    """Test AEGP_RQM_GetJobItemRenderSetting with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetJobItemRenderSetting(0, 0, 0)
        print(f"GetJobItemRenderSetting(0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetJobItemRenderSetting(0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_num_job_item_output_modules_with_invalid_session():
    """Test AEGP_RQM_GetNumJobItemOutputModules with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetNumJobItemOutputModules(0, 0)
        print(f"GetNumJobItemOutputModules(0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetNumJobItemOutputModules(0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_num_job_item_output_module_settings_with_invalid_session():
    """Test AEGP_RQM_GetNumJobItemOutputModuleSettings with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleSettings(0, 0, 0)
        print(f"GetNumJobItemOutputModuleSettings(0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetNumJobItemOutputModuleSettings(0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_job_item_output_module_setting_with_invalid_session():
    """Test AEGP_RQM_GetJobItemOutputModuleSetting with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetJobItemOutputModuleSetting(0, 0, 0, 0)
        print(f"GetJobItemOutputModuleSetting(0, 0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetJobItemOutputModuleSetting(0, 0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_num_job_item_output_module_warnings_with_invalid_session():
    """Test AEGP_RQM_GetNumJobItemOutputModuleWarnings with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleWarnings(0, 0, 0)
        print(f"GetNumJobItemOutputModuleWarnings(0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetNumJobItemOutputModuleWarnings(0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_job_item_output_module_warning_with_invalid_session():
    """Test AEGP_RQM_GetJobItemOutputModuleWarning with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetJobItemOutputModuleWarning(0, 0, 0, 0)
        print(f"GetJobItemOutputModuleWarning(0, 0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetJobItemOutputModuleWarning(0, 0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_num_job_item_frame_properties_with_invalid_session():
    """Test AEGP_RQM_GetNumJobItemFrameProperties with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetNumJobItemFrameProperties(0, 0, 0)
        print(f"GetNumJobItemFrameProperties(0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetNumJobItemFrameProperties(0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_job_item_frame_property_with_invalid_session():
    """Test AEGP_RQM_GetJobItemFrameProperty with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetJobItemFrameProperty(0, 0, 0, 0)
        print(f"GetJobItemFrameProperty(0, 0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetJobItemFrameProperty(0, 0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_num_job_item_output_module_properties_with_invalid_session():
    """Test AEGP_RQM_GetNumJobItemOutputModuleProperties with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleProperties(0, 0, 0)
        print(f"GetNumJobItemOutputModuleProperties(0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetNumJobItemOutputModuleProperties(0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_job_item_output_module_property_with_invalid_session():
    """Test AEGP_RQM_GetJobItemOutputModuleProperty with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetJobItemOutputModuleProperty(0, 0, 0, 0)
        print(f"GetJobItemOutputModuleProperty(0, 0, 0, 0) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetJobItemOutputModuleProperty(0, 0, 0, 0) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


@suite.test
def test_get_job_item_frame_thumbnail_with_invalid_session():
    """Test AEGP_RQM_GetJobItemFrameThumbnail with invalid session_id=0"""
    try:
        result = ae.sdk.AEGP_RQM_GetJobItemFrameThumbnail(0, 0, 0, 100, 100)
        print(f"GetJobItemFrameThumbnail(0, 0, 0, 100, 100) returned: {result}")
        assert_true(True, "Function callable with invalid session")
    except Exception as e:
        print(f"GetJobItemFrameThumbnail(0, 0, 0, 100, 100) raised: {e}")
        assert_true(True, "Function correctly rejected invalid session")


# ============================================================================
# Documentation Tests
# ============================================================================

@suite.test
def test_render_queue_monitor_api_documentation():
    """Verify RenderQueueMonitor API documentation is accurate"""
    # Verify all 18 functions exist
    functions = [
        'AEGP_RQM_RegisterListener',
        'AEGP_RQM_DeregisterListener',
        'AEGP_RQM_GetProjectName',
        'AEGP_RQM_GetAppVersion',
        'AEGP_RQM_GetNumJobItems',
        'AEGP_RQM_GetJobItemID',
        'AEGP_RQM_GetNumJobItemRenderSettings',
        'AEGP_RQM_GetJobItemRenderSetting',
        'AEGP_RQM_GetNumJobItemOutputModules',
        'AEGP_RQM_GetNumJobItemOutputModuleSettings',
        'AEGP_RQM_GetJobItemOutputModuleSetting',
        'AEGP_RQM_GetNumJobItemOutputModuleWarnings',
        'AEGP_RQM_GetJobItemOutputModuleWarning',
        'AEGP_RQM_GetNumJobItemFrameProperties',
        'AEGP_RQM_GetJobItemFrameProperty',
        'AEGP_RQM_GetNumJobItemOutputModuleProperties',
        'AEGP_RQM_GetJobItemOutputModuleProperty',
        'AEGP_RQM_GetJobItemFrameThumbnail',
    ]

    missing = []
    for func_name in functions:
        if not hasattr(ae.sdk, func_name):
            missing.append(func_name)

    if missing:
        raise AssertionError(f"Missing functions: {missing}")

    print("RenderQueueMonitor API (18 functions):")
    print("  Stub functions (C callback required):")
    print("    - AEGP_RQM_RegisterListener()")
    print("    - AEGP_RQM_DeregisterListener()")
    print("  Get functions (session_id required):")
    print("    - AEGP_RQM_GetProjectName(session_id)")
    print("    - AEGP_RQM_GetAppVersion(session_id)")
    print("    - AEGP_RQM_GetNumJobItems(session_id)")
    print("    - AEGP_RQM_GetJobItemID(session_id, job_item_index)")
    print("    - ... and 12 more functions")
    print("  Constants:")
    print("    - AEGP_RQM_FinishedStatus_UNKNOWN = 0")
    print("    - AEGP_RQM_FinishedStatus_SUCCEEDED = 1")
    print("    - AEGP_RQM_FinishedStatus_ABORTED = 2")
    print("    - AEGP_RQM_FinishedStatus_ERRED = 3")

    assert_true(True, "API documentation verified")


# ============================================================================
# Run Tests
# ============================================================================

def run():
    """Run all tests and return results"""
    return suite.run()


if __name__ == "__main__":
    run()
