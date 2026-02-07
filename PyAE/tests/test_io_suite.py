"""
I/O Suite API Tests

Note: I/O Suite (IOInSuite7, IOOutSuite6) is designed for AEIO plugins only.
AEGP plugins like PyAE cannot obtain AEIO_InSpecH or AEIO_OutSpecH handles
through standard AEGP APIs.

These tests verify:
1. API existence and binding
2. Error handling for invalid handles
3. Function signatures

For actual I/O operations, use FootageSuite and RenderQueueSuite instead.
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true

suite = TestSuite("I/O Suite")


# =============================================================================
# IOInSuite7 API List (39 functions)
# =============================================================================
IOIN_SUITE_FUNCTIONS = [
    "AEGP_GetInSpecOptionsHandle",
    "AEGP_SetInSpecOptionsHandle",
    "AEGP_GetInSpecFilePath",
    "AEGP_GetInSpecNativeFPS",
    "AEGP_SetInSpecNativeFPS",
    "AEGP_GetInSpecDepth",
    "AEGP_SetInSpecDepth",
    "AEGP_GetInSpecSize",
    "AEGP_SetInSpecSize",
    "AEGP_GetInSpecInterlaceLabel",
    "AEGP_SetInSpecInterlaceLabel",
    "AEGP_GetInSpecAlphaLabel",
    "AEGP_SetInSpecAlphaLabel",
    "AEGP_GetInSpecDuration",
    "AEGP_SetInSpecDuration",
    "AEGP_GetInSpecDimensions",
    "AEGP_SetInSpecDimensions",
    "AEGP_InSpecGetRationalDimensions",
    "AEGP_GetInSpecHSF",
    "AEGP_SetInSpecHSF",
    "AEGP_GetInSpecSoundRate",
    "AEGP_SetInSpecSoundRate",
    "AEGP_GetInSpecSoundEncoding",
    "AEGP_SetInSpecSoundEncoding",
    "AEGP_GetInSpecSoundSampleSize",
    "AEGP_SetInSpecSoundSampleSize",
    "AEGP_GetInSpecSoundChannels",
    "AEGP_SetInSpecSoundChannels",
    "AEGP_AddAuxExtMap",
    "AEGP_SetInSpecEmbeddedColorProfile",
    "AEGP_SetInSpecAssignedColorProfile",
    "AEGP_GetInSpecNativeStartTime",
    "AEGP_SetInSpecNativeStartTime",
    "AEGP_ClearInSpecNativeStartTime",
    "AEGP_GetInSpecNativeDisplayDropFrame",
    "AEGP_SetInSpecNativeDisplayDropFrame",
    "AEGP_SetInSpecStillSequenceNativeFPS",
    "AEGP_SetInSpecColorSpaceFromCICP",
    "AEGP_SetInSpecColorSpaceFromCICP2",
]

# =============================================================================
# IOOutSuite6 API List (35 functions - AEGP_GetOutSpecFrameTime excluded)
# =============================================================================
IOOUT_SUITE_FUNCTIONS = [
    "AEGP_GetOutSpecOptionsHandle",
    "AEGP_SetOutSpecOptionsHandle",
    "AEGP_GetOutSpecFilePath",
    "AEGP_GetOutSpecFPS",
    "AEGP_SetOutSpecNativeFPS",
    "AEGP_GetOutSpecDepth",
    "AEGP_SetOutSpecDepth",
    "AEGP_GetOutSpecInterlaceLabel",
    "AEGP_SetOutSpecInterlaceLabel",
    "AEGP_GetOutSpecAlphaLabel",
    "AEGP_SetOutSpecAlphaLabel",
    "AEGP_GetOutSpecDuration",
    "AEGP_SetOutSpecDuration",
    "AEGP_GetOutSpecDimensions",
    "AEGP_GetOutSpecHSF",
    "AEGP_SetOutSpecHSF",
    "AEGP_GetOutSpecSoundRate",
    "AEGP_SetOutSpecSoundRate",
    "AEGP_GetOutSpecSoundEncoding",
    "AEGP_SetOutSpecSoundEncoding",
    "AEGP_GetOutSpecSoundSampleSize",
    "AEGP_SetOutSpecSoundSampleSize",
    "AEGP_GetOutSpecSoundChannels",
    "AEGP_SetOutSpecSoundChannels",
    "AEGP_GetOutSpecIsStill",
    "AEGP_GetOutSpecPosterTime",
    "AEGP_GetOutSpecStartFrame",
    "AEGP_GetOutSpecPullDown",
    "AEGP_GetOutSpecIsMissing",
    "AEGP_GetOutSpecShouldEmbedICCProfile",
    "AEGP_GetNewOutSpecColorProfile",
    "AEGP_GetOutSpecOutputModule",
    "AEGP_GetOutSpecStartTime",
    # "AEGP_GetOutSpecFrameTime",  # Excluded: causes memory allocation error at pybind11 binding time
    "AEGP_GetOutSpecIsDropFrame",
    "AEGP_GetOutSpecColorSpaceAsCICPIfCompatible",
]


# =============================================================================
# IOInSuite7 API Existence Tests
# =============================================================================

@suite.test
def test_ioin_suite_function_count():
    """Verify expected function count for IOInSuite7"""
    assert_equal(len(IOIN_SUITE_FUNCTIONS), 39, "IOInSuite7 function count")


@suite.test
def test_ioin_all_functions_exist():
    """Verify all IOInSuite7 functions exist in ae.sdk module"""
    missing = []
    for func_name in IOIN_SUITE_FUNCTIONS:
        if not hasattr(ae.sdk, func_name):
            missing.append(func_name)

    if missing:
        raise AssertionError(
            f"Missing IOInSuite7 functions ({len(missing)}/{len(IOIN_SUITE_FUNCTIONS)}): "
            f"{', '.join(missing[:5])}{'...' if len(missing) > 5 else ''}"
        )

    print(f"✓ All {len(IOIN_SUITE_FUNCTIONS)} IOInSuite7 functions exist")


@suite.test
def test_ioin_all_functions_callable():
    """Verify all IOInSuite7 functions are callable"""
    non_callable = []
    for func_name in IOIN_SUITE_FUNCTIONS:
        func = getattr(ae.sdk, func_name, None)
        if func is None or not callable(func):
            non_callable.append(func_name)

    if non_callable:
        raise AssertionError(
            f"Non-callable IOInSuite7 functions: {', '.join(non_callable[:5])}"
        )

    print(f"✓ All {len(IOIN_SUITE_FUNCTIONS)} IOInSuite7 functions are callable")


# =============================================================================
# IOOutSuite6 API Existence Tests
# =============================================================================

@suite.test
def test_ioout_suite_function_count():
    """Verify expected function count for IOOutSuite6"""
    # 35 functions (36 - 1 excluded: AEGP_GetOutSpecFrameTime)
    assert_equal(len(IOOUT_SUITE_FUNCTIONS), 35, "IOOutSuite6 function count")


@suite.test
def test_ioout_all_functions_exist():
    """Verify all IOOutSuite6 functions exist in ae.sdk module"""
    missing = []
    for func_name in IOOUT_SUITE_FUNCTIONS:
        if not hasattr(ae.sdk, func_name):
            missing.append(func_name)

    if missing:
        raise AssertionError(
            f"Missing IOOutSuite6 functions ({len(missing)}/{len(IOOUT_SUITE_FUNCTIONS)}): "
            f"{', '.join(missing[:5])}{'...' if len(missing) > 5 else ''}"
        )

    print(f"✓ All {len(IOOUT_SUITE_FUNCTIONS)} IOOutSuite6 functions exist")


@suite.test
def test_ioout_all_functions_callable():
    """Verify all IOOutSuite6 functions are callable"""
    non_callable = []
    for func_name in IOOUT_SUITE_FUNCTIONS:
        func = getattr(ae.sdk, func_name, None)
        if func is None or not callable(func):
            non_callable.append(func_name)

    if non_callable:
        raise AssertionError(
            f"Non-callable IOOutSuite6 functions: {', '.join(non_callable[:5])}"
        )

    print(f"✓ All {len(IOOUT_SUITE_FUNCTIONS)} IOOutSuite6 functions are callable")


# =============================================================================
# Invalid Handle Error Tests
# =============================================================================

@suite.test
def test_ioin_invalid_handle_errors():
    """Test IOInSuite7 functions properly reject invalid handles"""
    # Test subset of getter functions with invalid handle (0)
    test_functions = [
        "AEGP_GetInSpecNativeFPS",
        "AEGP_GetInSpecDepth",
        "AEGP_GetInSpecSize",
        "AEGP_GetInSpecDuration",
        "AEGP_GetInSpecHSF",
        "AEGP_GetInSpecSoundRate",
    ]

    passed = 0
    failed = []

    for func_name in test_functions:
        func = getattr(ae.sdk, func_name, None)
        if func is None:
            continue

        try:
            func(0)  # Invalid handle
            failed.append(func_name)
        except (RuntimeError, TypeError, ValueError):
            passed += 1  # Expected error

    if failed:
        raise AssertionError(
            f"Functions accepted invalid handle: {', '.join(failed)}"
        )

    print(f"✓ {passed} IOInSuite7 functions properly reject invalid handles")


@suite.test
def test_ioout_invalid_handle_errors():
    """Test IOOutSuite6 functions properly reject invalid handles"""
    # Test subset of getter functions with invalid handle (0)
    test_functions = [
        "AEGP_GetOutSpecFPS",
        "AEGP_GetOutSpecDepth",
        "AEGP_GetOutSpecDuration",
        "AEGP_GetOutSpecHSF",
        "AEGP_GetOutSpecSoundRate",
        "AEGP_GetOutSpecIsStill",
    ]

    passed = 0
    failed = []

    for func_name in test_functions:
        func = getattr(ae.sdk, func_name, None)
        if func is None:
            continue

        try:
            func(0)  # Invalid handle
            failed.append(func_name)
        except (RuntimeError, TypeError, ValueError):
            passed += 1  # Expected error

    if failed:
        raise AssertionError(
            f"Functions accepted invalid handle: {', '.join(failed)}"
        )

    print(f"✓ {passed} IOOutSuite6 functions properly reject invalid handles")


# =============================================================================
# Summary Test
# =============================================================================

@suite.test
def test_total_io_suite_functions():
    """Verify total function count for I/O Suites"""
    total = len(IOIN_SUITE_FUNCTIONS) + len(IOOUT_SUITE_FUNCTIONS)
    # 39 + 35 = 74 (AEGP_GetOutSpecFrameTime excluded)
    assert_equal(total, 74, "Total I/O Suite functions")


# =============================================================================
# Main Test Runner (PyAE Test Framework Integration)
# =============================================================================

def run():
    """Run all I/O Suite tests (PyAE test framework entry point)"""
    return suite.run()


if __name__ == "__main__":
    run()
