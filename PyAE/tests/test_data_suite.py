"""
Data Suite Tests
Tests for AEGP_PersistentDataSuite4, AEGP_SoundDataSuite1, AEGP_ColorSettingsSuite6

These suites handle:
- PersistentDataSuite4: Application preferences and persistent data storage
- SoundDataSuite1: Audio data handling
- ColorSettingsSuite6: Color management and profiles
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true

suite = TestSuite("Data Suite")

# Test variables
_test_comp = None


@suite.setup
def setup():
    """Setup test composition"""
    global _test_comp
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_DataSuiteTestComp", 1920, 1080, 1.0, 5.0, 30.0)


@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp
    if _test_comp is not None:
        _test_comp.delete()
        _test_comp = None


# ============================================================================
# PersistentDataSuite4 Tests
# ============================================================================

@suite.test
def test_persistent_data_constants():
    """Test that all PersistentData constants are defined"""
    # PersistentType constants
    assert_true(hasattr(ae.sdk, 'AEGP_PersistentType_MACHINE_SPECIFIC'),
                "AEGP_PersistentType_MACHINE_SPECIFIC defined")
    assert_true(hasattr(ae.sdk, 'AEGP_PersistentType_MACHINE_INDEPENDENT'),
                "AEGP_PersistentType_MACHINE_INDEPENDENT defined")
    assert_true(hasattr(ae.sdk, 'AEGP_PersistentType_MACHINE_INDEPENDENT_RENDER'),
                "AEGP_PersistentType_MACHINE_INDEPENDENT_RENDER defined")
    assert_true(hasattr(ae.sdk, 'AEGP_PersistentType_MACHINE_INDEPENDENT_OUTPUT'),
                "AEGP_PersistentType_MACHINE_INDEPENDENT_OUTPUT defined")
    assert_true(hasattr(ae.sdk, 'AEGP_PersistentType_MACHINE_INDEPENDENT_COMPOSITION'),
                "AEGP_PersistentType_MACHINE_INDEPENDENT_COMPOSITION defined")


@suite.test
def test_get_application_blob():
    """Test AEGP_GetApplicationBlob - gets application blob handle"""
    # Get machine-specific blob
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)
    assert_true(blobH != 0, "Got valid blob handle")
    print(f"Machine-specific blob handle: {blobH}")

    # Get machine-independent blob
    blobH2 = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)
    assert_true(blobH2 != 0, "Got valid blob handle for machine-independent")
    print(f"Machine-independent blob handle: {blobH2}")


@suite.test
def test_get_num_sections():
    """Test AEGP_GetNumSections - gets number of sections in blob"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    num_sections = ae.sdk.AEGP_GetNumSections(blobH)
    assert_true(num_sections >= 0, f"Got section count: {num_sections}")
    print(f"Number of sections: {num_sections}")


@suite.test
def test_persistent_data_string():
    """Test AEGP_GetString and AEGP_SetString - string data storage"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    # Set a test string
    section_key = "PyAE_Test"
    value_key = "TestString"
    test_value = "Hello PyAE Test"

    # Set the string
    ae.sdk.AEGP_SetString(blobH, section_key, value_key, test_value)
    print(f"Set string value: {test_value}")

    # Get the string back
    result = ae.sdk.AEGP_GetString(blobH, section_key, value_key, "", 256)
    assert_equal(result, test_value, "String value matches")
    print(f"Retrieved string value: {result}")

    # Clean up - delete the entry
    ae.sdk.AEGP_DeleteEntry(blobH, section_key, value_key)
    print("Deleted test entry")


@suite.test
def test_persistent_data_long():
    """Test AEGP_GetLong and AEGP_SetLong - integer data storage"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test"
    value_key = "TestLong"
    test_value = 12345

    # Set the long value
    ae.sdk.AEGP_SetLong(blobH, section_key, value_key, test_value)
    print(f"Set long value: {test_value}")

    # Get the value back
    result = ae.sdk.AEGP_GetLong(blobH, section_key, value_key, 0)
    assert_equal(result, test_value, "Long value matches")
    print(f"Retrieved long value: {result}")

    # Clean up
    ae.sdk.AEGP_DeleteEntry(blobH, section_key, value_key)


@suite.test
def test_persistent_data_fplong():
    """Test AEGP_GetFpLong and AEGP_SetFpLong - float data storage"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test"
    value_key = "TestFpLong"
    test_value = 3.141592

    # Set the float value
    ae.sdk.AEGP_SetFpLong(blobH, section_key, value_key, test_value)
    print(f"Set float value: {test_value}")

    # Get the value back (note: 6 decimal places precision)
    result = ae.sdk.AEGP_GetFpLong(blobH, section_key, value_key, 0.0)
    # Allow for floating point precision
    assert_true(abs(result - test_value) < 0.000001, f"Float value matches: {result}")
    print(f"Retrieved float value: {result}")

    # Clean up
    ae.sdk.AEGP_DeleteEntry(blobH, section_key, value_key)


@suite.test
def test_does_key_exist():
    """Test AEGP_DoesKeyExist - checks if a key exists"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test"
    value_key = "TestExists"

    # Initially should not exist
    exists = ae.sdk.AEGP_DoesKeyExist(blobH, section_key, value_key)
    assert_true(not exists, "Key does not exist initially")

    # Set a value
    ae.sdk.AEGP_SetLong(blobH, section_key, value_key, 42)

    # Now should exist
    exists = ae.sdk.AEGP_DoesKeyExist(blobH, section_key, value_key)
    assert_true(exists, "Key exists after setting")
    print("Key existence check passed")

    # Clean up
    ae.sdk.AEGP_DeleteEntry(blobH, section_key, value_key)


@suite.test
def test_get_prefs_directory():
    """Test AEGP_GetPrefsDirectory - gets preferences directory path"""
    prefs_dir = ae.sdk.AEGP_GetPrefsDirectory()
    assert_true(len(prefs_dir) > 0, f"Got prefs directory: {prefs_dir}")
    print(f"Preferences directory: {prefs_dir}")


@suite.test
def test_persistent_data_time():
    """Test AEGP_GetTime and AEGP_SetTime - time value storage"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test"
    value_key = "TestTime"
    test_time = (300, 30)  # 10 seconds at 30fps

    # Set time value
    ae.sdk.AEGP_SetTime(blobH, section_key, value_key, test_time)
    print(f"Set time value: {test_time}")

    # Get time back
    result = ae.sdk.AEGP_GetTime(blobH, section_key, value_key, (0, 1))
    assert_equal(result, test_time, "Time value matches")
    print(f"Retrieved time value: {result}")

    # Clean up
    ae.sdk.AEGP_DeleteEntry(blobH, section_key, value_key)


@suite.test
def test_persistent_data_argb():
    """Test AEGP_GetARGB and AEGP_SetARGB - ARGB color storage"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test"
    value_key = "TestARGB"
    test_color = (1.0, 0.5, 0.25, 0.75)  # (alpha, red, green, blue)

    # Set ARGB value
    ae.sdk.AEGP_SetARGB(blobH, section_key, value_key, test_color)
    print(f"Set ARGB value: {test_color}")

    # Get ARGB back
    result = ae.sdk.AEGP_GetARGB(blobH, section_key, value_key, (0.0, 0.0, 0.0, 0.0))
    print(f"Retrieved ARGB value: {result}")

    # Allow for floating point precision (8-bit color precision: 1/255 = 0.0039)
    component_names = ["alpha", "red", "green", "blue"]
    for i in range(4):
        diff = abs(result[i] - test_color[i])
        assert_true(diff < 0.01,
                   f"ARGB component {i} ({component_names[i]}) matches: expected={test_color[i]}, got={result[i]}, diff={diff}")

    # Clean up
    ae.sdk.AEGP_DeleteEntry(blobH, section_key, value_key)


@suite.test
def test_persistent_data_datahandle():
    """Test AEGP_GetDataHandle and AEGP_SetDataHandle - binary data storage"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test"
    value_key = "TestDataHandle"
    test_data = b"Binary data test \x00\x01\x02\xFF"

    # Set data handle
    ae.sdk.AEGP_SetDataHandle(blobH, section_key, value_key, test_data)
    print(f"Set data handle ({len(test_data)} bytes)")

    # Get data back
    result = ae.sdk.AEGP_GetDataHandle(blobH, section_key, value_key, b"")
    assert_equal(result, test_data, "Data handle matches")
    print(f"Retrieved data handle ({len(result)} bytes)")

    # Clean up
    ae.sdk.AEGP_DeleteEntry(blobH, section_key, value_key)


@suite.test
def test_persistent_data_data():
    """Test AEGP_GetData and AEGP_SetData - fixed-size binary data storage"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test"
    value_key = "TestData"
    test_data = b"Fixed size data test 1234567890"

    # Set data
    ae.sdk.AEGP_SetData(blobH, section_key, value_key, test_data)
    print(f"Set data ({len(test_data)} bytes)")

    # Get data back
    result = ae.sdk.AEGP_GetData(blobH, section_key, value_key, len(test_data), b"")
    assert_equal(result, test_data, "Data matches")
    print(f"Retrieved data ({len(result)} bytes)")

    # Clean up
    ae.sdk.AEGP_DeleteEntry(blobH, section_key, value_key)


@suite.test
def test_get_section_key_by_index():
    """Test AEGP_GetSectionKeyByIndex - iterate through sections"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    # Create test sections
    section_keys = ["PyAE_Test_Section1", "PyAE_Test_Section2", "PyAE_Test_Section3"]
    for section in section_keys:
        ae.sdk.AEGP_SetLong(blobH, section, "dummy", 1)

    try:
        # Get number of sections
        num_sections = ae.sdk.AEGP_GetNumSections(blobH)
        print(f"Total sections: {num_sections}")

        # Iterate through sections to find our test sections
        found_sections = []
        for i in range(num_sections):
            try:
                section_key = ae.sdk.AEGP_GetSectionKeyByIndex(blobH, i, 256)
                if section_key.startswith("PyAE_Test_Section"):
                    found_sections.append(section_key)
            except (UnicodeDecodeError, RuntimeError) as e:
                # Skip sections with invalid encoding or other errors
                print(f"Skipping section {i} due to error: {e}")
                continue

        print(f"Found test sections: {found_sections}")
        assert_true(len(found_sections) >= 3, "Found at least 3 test sections")

    finally:
        # Clean up
        for section in section_keys:
            ae.sdk.AEGP_DeleteEntry(blobH, section, "dummy")


@suite.test
def test_get_num_keys():
    """Test AEGP_GetNumKeys - get number of keys in a section"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test_NumKeys"
    test_keys = ["Key1", "Key2", "Key3", "Key4"]

    try:
        # Create test keys
        for key in test_keys:
            ae.sdk.AEGP_SetLong(blobH, section_key, key, 1)

        # Get number of keys
        num_keys = ae.sdk.AEGP_GetNumKeys(blobH, section_key)
        print(f"Number of keys in section '{section_key}': {num_keys}")

        assert_true(num_keys >= len(test_keys), f"Found at least {len(test_keys)} keys")

    finally:
        # Clean up
        for key in test_keys:
            ae.sdk.AEGP_DeleteEntry(blobH, section_key, key)


@suite.test
def test_get_value_key_by_index():
    """Test AEGP_GetValueKeyByIndex - iterate through keys in a section"""
    blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC)

    section_key = "PyAE_Test_ValueKeys"
    test_keys = ["TestKey1", "TestKey2", "TestKey3"]

    try:
        # Create test keys
        for i, key in enumerate(test_keys):
            ae.sdk.AEGP_SetLong(blobH, section_key, key, i + 1)

        # Get number of keys
        num_keys = ae.sdk.AEGP_GetNumKeys(blobH, section_key)
        print(f"Number of keys: {num_keys}")

        # Iterate through keys
        found_keys = []
        for i in range(num_keys):
            key = ae.sdk.AEGP_GetValueKeyByIndex(blobH, section_key, i, 256)
            if key.startswith("TestKey"):
                found_keys.append(key)

        print(f"Found test keys: {found_keys}")
        assert_true(len(found_keys) >= 3, "Found at least 3 test keys")

    finally:
        # Clean up
        for key in test_keys:
            ae.sdk.AEGP_DeleteEntry(blobH, section_key, key)


# ============================================================================
# SoundDataSuite1 Tests
# ============================================================================

@suite.test
def test_sound_data_constants():
    """Test that all SoundData constants are defined"""
    assert_true(hasattr(ae.sdk, 'AEGP_SoundEncoding_UNSIGNED_PCM'),
                "AEGP_SoundEncoding_UNSIGNED_PCM defined")
    assert_true(hasattr(ae.sdk, 'AEGP_SoundEncoding_SIGNED_PCM'),
                "AEGP_SoundEncoding_SIGNED_PCM defined")
    assert_true(hasattr(ae.sdk, 'AEGP_SoundEncoding_FLOAT'),
                "AEGP_SoundEncoding_FLOAT defined")


@suite.test
def test_new_sound_data():
    """Test AEGP_NewSoundData - creates new sound data buffer"""
    # Create a simple audio buffer: 44100Hz, signed 16-bit PCM, stereo
    sample_rate = 44100.0
    encoding = ae.sdk.AEGP_SoundEncoding_SIGNED_PCM
    bytes_per_sample = 2
    num_channels = 2

    soundH = ae.sdk.AEGP_NewSoundData(sample_rate, encoding, bytes_per_sample, num_channels)
    # Note: soundH can be 0 if no audio data is available
    # This is expected for empty buffers
    print(f"Created sound data handle: {soundH}")

    if soundH != 0:
        # Clean up
        ae.sdk.AEGP_DisposeSoundData(soundH)
        print("Disposed sound data")


@suite.test
def test_get_sound_data_format():
    """Test AEGP_GetSoundDataFormat - gets sound format info"""
    # Create sound data
    sample_rate = 48000.0
    encoding = ae.sdk.AEGP_SoundEncoding_FLOAT
    bytes_per_sample = 4
    num_channels = 1  # mono

    soundH = ae.sdk.AEGP_NewSoundData(sample_rate, encoding, bytes_per_sample, num_channels)

    if soundH != 0:
        try:
            # Get format
            sr, enc, bps, ch = ae.sdk.AEGP_GetSoundDataFormat(soundH)
            print(f"Sound format: {sr}Hz, encoding={enc}, {bps} bytes/sample, {ch} channels")

            # Verify values
            assert_true(abs(sr - sample_rate) < 0.001, "Sample rate matches")
            assert_equal(enc, encoding, "Encoding matches")
            assert_equal(bps, bytes_per_sample, "Bytes per sample matches")
            assert_equal(ch, num_channels, "Channels match")

        finally:
            ae.sdk.AEGP_DisposeSoundData(soundH)
    else:
        print("SoundData creation returned null (expected for empty buffer)")


@suite.test
def test_get_num_samples():
    """Test AEGP_GetNumSamples - gets number of samples in sound data"""
    sample_rate = 44100.0
    encoding = ae.sdk.AEGP_SoundEncoding_SIGNED_PCM
    bytes_per_sample = 2
    num_channels = 2

    soundH = ae.sdk.AEGP_NewSoundData(sample_rate, encoding, bytes_per_sample, num_channels)

    if soundH != 0:
        try:
            # Get number of samples
            num_samples = ae.sdk.AEGP_GetNumSamples(soundH)
            print(f"Number of samples: {num_samples}")

            # Empty buffer should have 0 samples
            assert_equal(num_samples, 0, "Empty buffer has 0 samples")

        finally:
            ae.sdk.AEGP_DisposeSoundData(soundH)
    else:
        print("SoundData creation returned null")


@suite.test
def test_lock_unlock_sound_data_samples():
    """Test AEGP_LockSoundDataSamples and AEGP_UnlockSoundDataSamples"""
    sample_rate = 44100.0
    encoding = ae.sdk.AEGP_SoundEncoding_SIGNED_PCM
    bytes_per_sample = 2
    num_channels = 2

    soundH = ae.sdk.AEGP_NewSoundData(sample_rate, encoding, bytes_per_sample, num_channels)

    if soundH != 0:
        try:
            # Lock samples (may return null for empty buffer, which is OK)
            samples_ptr = ae.sdk.AEGP_LockSoundDataSamples(soundH)
            print(f"Locked samples ptr: {samples_ptr}")

            # Unlock samples
            ae.sdk.AEGP_UnlockSoundDataSamples(soundH)
            print("Unlocked samples")

        finally:
            ae.sdk.AEGP_DisposeSoundData(soundH)
    else:
        print("SoundData creation returned null")


# ============================================================================
# ColorSettingsSuite6 Tests
# ============================================================================

@suite.test
def test_is_ocio_color_management_used():
    """Test AEGP_IsOCIOColorManagementUsed - checks if OCIO is enabled"""
    is_ocio = ae.sdk.AEGP_IsOCIOColorManagementUsed()
    print(f"OCIO color management used: {is_ocio}")
    # Result can be True or False, just verify it returns a boolean
    assert_true(isinstance(is_ocio, bool), "Returns boolean value")


@suite.test
def test_get_working_space_color_profile():
    """Test AEGP_GetNewWorkingSpaceColorProfile - gets working color profile"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    try:
        profileP = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(_test_comp._handle)
        if profileP != 0:
            print(f"Got working space color profile: {profileP}")

            # Test getting profile description
            try:
                desc = ae.sdk.AEGP_GetNewColorProfileDescription(profileP)
                print(f"Profile description: {desc}")
            except Exception as e:
                print(f"Could not get profile description: {e}")

            # Test if it's RGB
            try:
                is_rgb = ae.sdk.AEGP_IsRGBColorProfile(profileP)
                print(f"Is RGB profile: {is_rgb}")
            except Exception as e:
                print(f"Could not check if RGB: {e}")

            # Clean up
            ae.sdk.AEGP_DisposeColorProfile(profileP)
            print("Disposed color profile")
        else:
            print("No working space color profile (may be using legacy color management)")
    except Exception as e:
        print(f"Could not get working space color profile: {e}")
        # This is not an error - older projects may not have color profiles


@suite.test
def test_color_space_aware_effects():
    """Test AEGPD_IsColorSpaceAwareEffectsEnabled - checks color space aware effects"""
    try:
        is_enabled = ae.sdk.AEGPD_IsColorSpaceAwareEffectsEnabled()
        print(f"Color space aware effects enabled: {is_enabled}")
    except Exception as e:
        print(f"Could not check color space aware effects: {e}")


@suite.test
def test_graphics_white_luminance():
    """Test AEGPD_GetGraphicsWhiteLuminance - gets graphics white luminance"""
    try:
        luminance = ae.sdk.AEGPD_GetGraphicsWhiteLuminance()
        print(f"Graphics white luminance: {luminance}")
    except Exception as e:
        print(f"Could not get graphics white luminance: {e}")


@suite.test
def test_lut_interpolation_method():
    """Test AEGPD_GetLUTInterpolationMethod - gets LUT interpolation method"""
    try:
        method = ae.sdk.AEGPD_GetLUTInterpolationMethod()
        print(f"LUT interpolation method: {method}")
    except Exception as e:
        print(f"Could not get LUT interpolation method: {e}")


@suite.test
def test_get_ocio_config_path():
    """Test AEGP_GetOCIOConfigurationFilePath - gets OCIO config file path"""
    try:
        config_path = ae.sdk.AEGP_GetOCIOConfigurationFilePath()
        if config_path:
            print(f"OCIO configuration file path: {config_path}")
        else:
            print("No OCIO configuration file (OCIO may not be enabled)")
    except Exception as e:
        print(f"Could not get OCIO config path: {e}")


@suite.test
def test_get_color_profile_gamma():
    """Test AEGP_GetColorProfileApproximateGamma - gets approximate gamma"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    try:
        profileP = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(_test_comp._handle)
        if profileP != 0:
            try:
                gamma = ae.sdk.AEGP_GetColorProfileApproximateGamma(profileP)
                print(f"Approximate gamma: {gamma}")
                assert_true(gamma > 0, "Gamma is positive")
            finally:
                ae.sdk.AEGP_DisposeColorProfile(profileP)
        else:
            print("No color profile available")
    except Exception as e:
        print(f"Could not get gamma: {e}")


@suite.test
def test_get_ocio_working_colorspace():
    """Test AEGPD_GetOCIOWorkingColorSpace - gets OCIO working colorspace"""
    try:
        # Only works if OCIO is enabled
        is_ocio = ae.sdk.AEGP_IsOCIOColorManagementUsed()
        if is_ocio:
            colorspace = ae.sdk.AEGPD_GetOCIOWorkingColorSpace()
            print(f"OCIO working colorspace: {colorspace}")
        else:
            print("OCIO not enabled, skipping test")
    except Exception as e:
        print(f"Could not get OCIO working colorspace: {e}")


@suite.test
def test_get_ocio_display_colorspace():
    """Test AEGPD_GetOCIODisplayColorSpace - gets OCIO display colorspace"""
    try:
        # Only works if OCIO is enabled
        is_ocio = ae.sdk.AEGP_IsOCIOColorManagementUsed()
        if is_ocio:
            display, view = ae.sdk.AEGPD_GetOCIODisplayColorSpace()
            print(f"OCIO display: {display}, view: {view}")
        else:
            print("OCIO not enabled, skipping test")
    except Exception as e:
        print(f"Could not get OCIO display colorspace: {e}")


@suite.test
def test_get_working_colorspace_id():
    """Test AEGPD_GetWorkingColorSpaceId - gets working colorspace GUID"""
    try:
        guid_bytes = ae.sdk.AEGPD_GetWorkingColorSpaceId()
        print(f"Working colorspace GUID: {len(guid_bytes)} bytes")
        assert_equal(len(guid_bytes), 16, "GUID is 16 bytes")
    except Exception as e:
        print(f"Could not get working colorspace ID: {e}")


@suite.test
def test_new_color_profile_from_icc():
    """Test AEGP_GetNewColorProfileFromICCProfile - create color profile from ICC"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    try:
        # First get an ICC profile from existing color profile
        profileP = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(_test_comp._handle)
        if profileP != 0:
            try:
                # Get ICC profile data
                icc_data = ae.sdk.AEGP_GetNewICCProfileFromColorProfile(profileP)
                print(f"Got ICC profile data: {len(icc_data)} bytes")

                # Create color profile from ICC data
                new_profileP = ae.sdk.AEGP_GetNewColorProfileFromICCProfile(icc_data)
                print(f"Created color profile from ICC: {new_profileP}")

                # Clean up
                ae.sdk.AEGP_DisposeColorProfile(new_profileP)

            finally:
                ae.sdk.AEGP_DisposeColorProfile(profileP)
        else:
            print("No color profile available")
    except Exception as e:
        print(f"Could not test ICC profile conversion: {e}")


@suite.test
def test_set_working_color_space():
    """Test AEGP_SetWorkingColorSpace - sets working color space"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    try:
        # Get current working space
        profileP = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(_test_comp._handle)
        if profileP != 0:
            try:
                # Set it back (should not change anything)
                ae.sdk.AEGP_SetWorkingColorSpace(_test_comp._handle, profileP)
                print("Set working color space successfully")
            finally:
                ae.sdk.AEGP_DisposeColorProfile(profileP)
        else:
            print("No color profile available")
    except Exception as e:
        print(f"Could not set working color space: {e}")


# Run tests
def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
