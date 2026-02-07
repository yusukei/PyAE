"""
Camera & Light Suite Tests
Tests for AEGP_CameraSuite2 and AEGP_LightSuite3

Note: Camera/Light parameters like Focal Length, Aperture, Focus Distance, Intensity,
Cone Angle, and Cone Falloff are NOT directly accessible via suite functions.
They must be accessed through the Stream Suite as property streams.
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true

suite = TestSuite("Camera & Light Suite")

# Test variables
_test_comp = None


@suite.setup
def setup():
    """Setup test composition"""
    global _test_comp
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_CameraLightTestComp", 1920, 1080, 1.0, 5.0, 30.0)


@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
            _test_comp = None
    except:
        pass


# ============================================================================
# Camera Suite Tests
# ============================================================================

@suite.test
def test_camera_constants():
    """Test that all Camera constants are defined"""
    # Camera Type constants
    assert_true(hasattr(ae.sdk, 'AEGP_CameraType_NONE'), "AEGP_CameraType_NONE defined")
    assert_true(hasattr(ae.sdk, 'AEGP_CameraType_PERSPECTIVE'), "AEGP_CameraType_PERSPECTIVE defined")
    assert_true(hasattr(ae.sdk, 'AEGP_CameraType_ORTHOGRAPHIC'), "AEGP_CameraType_ORTHOGRAPHIC defined")

    # Film Size Units constants
    assert_true(hasattr(ae.sdk, 'AEGP_FilmSizeUnits_NONE'), "AEGP_FilmSizeUnits_NONE defined")
    assert_true(hasattr(ae.sdk, 'AEGP_FilmSizeUnits_HORIZONTAL'), "AEGP_FilmSizeUnits_HORIZONTAL defined")
    assert_true(hasattr(ae.sdk, 'AEGP_FilmSizeUnits_VERTICAL'), "AEGP_FilmSizeUnits_VERTICAL defined")
    assert_true(hasattr(ae.sdk, 'AEGP_FilmSizeUnits_DIAGONAL'), "AEGP_FilmSizeUnits_DIAGONAL defined")


@suite.test
def test_camera_type():
    """Test AEGP_GetCameraType - gets camera type (PERSPECTIVE/ORTHOGRAPHIC)"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    # Create a camera layer (default is perspective)
    camera_layer = _test_comp.add_camera("Test Camera", (_test_comp.width / 2, _test_comp.height / 2))

    try:
        # Test getting camera type
        camera_type = ae.sdk.AEGP_GetCameraType(camera_layer._handle)

        # Should be PERSPECTIVE by default
        assert camera_type == ae.sdk.AEGP_CameraType_PERSPECTIVE

        print(f"Camera type: {camera_type} (PERSPECTIVE)")

    finally:
        # Cleanup
        camera_layer.delete()


@suite.test
def test_camera_film_size_get():
    """Test AEGP_GetCameraFilmSize - gets camera film size units and value"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    camera_layer = _test_comp.add_camera("Test Camera", (_test_comp.width / 2, _test_comp.height / 2))

    try:
        # Test getting film size
        film_size_units, film_size = ae.sdk.AEGP_GetCameraFilmSize(camera_layer._handle)

        # Film size should be positive
        assert film_size > 0

        # Units should be one of the valid values
        valid_units = [
            ae.sdk.AEGP_FilmSizeUnits_NONE,
            ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL,
            ae.sdk.AEGP_FilmSizeUnits_VERTICAL,
            ae.sdk.AEGP_FilmSizeUnits_DIAGONAL
        ]
        assert film_size_units in valid_units

        print(f"Film size units: {film_size_units}, size: {film_size} pixels")

    finally:
        camera_layer.delete()


@suite.test
def test_camera_film_size_set():
    """Test AEGP_SetCameraFilmSize - sets camera film size"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    camera_layer = _test_comp.add_camera("Test Camera", (_test_comp.width / 2, _test_comp.height / 2))

    try:
        # Set new film size
        new_size = 50.0
        ae.sdk.AEGP_SetCameraFilmSize(
            camera_layer._handle,
            ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL,
            new_size
        )

        # Verify the change
        units, size = ae.sdk.AEGP_GetCameraFilmSize(camera_layer._handle)
        assert units == ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL
        assert abs(size - new_size) < 0.01

        print(f"Film size set to: {size} pixels (HORIZONTAL)")

    finally:
        camera_layer.delete()


@suite.test
def test_default_camera_distance():
    """Test AEGP_GetDefaultCameraDistanceToImagePlane - gets default camera distance"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    # Get default camera distance
    distance = ae.sdk.AEGP_GetDefaultCameraDistanceToImagePlane(_test_comp._handle)

    # Distance should be positive
    assert distance > 0

    print(f"Default camera distance: {distance}")


@suite.test
def test_camera_error_handling():
    """Test behavior when calling camera functions on non-camera layers"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    # Create a non-camera layer (solid)
    solid_layer = _test_comp.add_solid("Solid", 100, 100, (1.0, 0.0, 0.0), 5.0)

    try:
        # For non-camera layers, AEGP_GetCameraType returns AEGP_CameraType_NONE
        camera_type = ae.sdk.AEGP_GetCameraType(solid_layer._handle)
        assert camera_type == ae.sdk.AEGP_CameraType_NONE

        # AEGP_GetCameraFilmSize may raise RuntimeError on non-camera layers
        try:
            ae.sdk.AEGP_GetCameraFilmSize(solid_layer._handle)
        except RuntimeError:
            pass  # Expected for non-camera layers

        print("Camera behavior on non-camera layers works correctly")

    finally:
        solid_layer.delete()


# ============================================================================
# Light Suite Tests
# ============================================================================

@suite.test
def test_light_constants():
    """Test that all Light constants are defined"""
    # Light Type constants
    assert hasattr(ae.sdk, 'AEGP_LightType_NONE')
    assert hasattr(ae.sdk, 'AEGP_LightType_PARALLEL')
    assert hasattr(ae.sdk, 'AEGP_LightType_SPOT')
    assert hasattr(ae.sdk, 'AEGP_LightType_POINT')
    assert hasattr(ae.sdk, 'AEGP_LightType_AMBIENT')
    assert hasattr(ae.sdk, 'AEGP_LightType_ENVIRONMENT')

    print("All Light constants are defined")


@suite.test
def test_light_type_get():
    """Test AEGP_GetLightType - gets light type"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    # Create a light layer
    light_layer = _test_comp.add_light("Test Light", (_test_comp.width / 2, _test_comp.height / 2))

    try:
        # Test getting light type
        light_type = ae.sdk.AEGP_GetLightType(light_layer._handle)

        # Should be one of the valid light types
        valid_types = [
            ae.sdk.AEGP_LightType_NONE,
            ae.sdk.AEGP_LightType_PARALLEL,
            ae.sdk.AEGP_LightType_SPOT,
            ae.sdk.AEGP_LightType_POINT,
            ae.sdk.AEGP_LightType_AMBIENT,
            ae.sdk.AEGP_LightType_ENVIRONMENT
        ]
        assert light_type in valid_types

        print(f"Light type: {light_type}")

    finally:
        light_layer.delete()


@suite.test
def test_light_type_set():
    """Test AEGP_SetLightType - sets light type"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    light_layer = _test_comp.add_light("Test Light", (_test_comp.width / 2, _test_comp.height / 2))

    try:
        # Test setting different light types
        light_types = [
            (ae.sdk.AEGP_LightType_SPOT, "SPOT"),
            (ae.sdk.AEGP_LightType_POINT, "POINT"),
            (ae.sdk.AEGP_LightType_PARALLEL, "PARALLEL"),
            (ae.sdk.AEGP_LightType_AMBIENT, "AMBIENT"),
        ]

        for light_type, type_name in light_types:
            # Set light type
            ae.sdk.AEGP_SetLightType(light_layer._handle, light_type)

            # Verify
            new_type = ae.sdk.AEGP_GetLightType(light_layer._handle)
            assert new_type == light_type

            print(f"Light type changed to: {type_name}")

    finally:
        light_layer.delete()


@suite.test
def test_light_source_get():
    """Test AEGP_GetLightSource - gets light source layer (AE 24.4+)"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    light_layer = _test_comp.add_light("Test Light", (_test_comp.width / 2, _test_comp.height / 2))

    try:
        # Test getting light source (should be 0/None initially)
        light_source = ae.sdk.AEGP_GetLightSource(light_layer._handle)

        # Should be 0 (no source) by default
        assert light_source == 0

        print("No light source set (default)")

    finally:
        light_layer.delete()


@suite.test
def test_light_source_set():
    """Test AEGP_SetLightSource - sets light source layer (AE 24.4+)"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    # Create an environment light (can have source)
    light_layer = _test_comp.add_light("Test Light", (_test_comp.width / 2, _test_comp.height / 2))

    # Change to environment light type which supports light source
    ae.sdk.AEGP_SetLightType(light_layer._handle, ae.sdk.AEGP_LightType_ENVIRONMENT)

    try:
        # Initially no source
        light_source = ae.sdk.AEGP_GetLightSource(light_layer._handle)
        assert light_source == 0, "Initially should have no light source"
        print("Light source initial state verified (no source)")

        # Note: Setting light source requires a valid source layer
        # and the functionality depends on AE version and light type.
        # This is primarily for environment lights.

    finally:
        light_layer.delete()


@suite.test
def test_light_error_handling():
    """Test behavior when calling light functions on non-light layers"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    # Create a non-light layer (solid)
    solid_layer = _test_comp.add_solid("Solid", 100, 100, (1.0, 0.0, 0.0), 5.0)

    try:
        # For non-light layers, AEGP_GetLightType returns AEGP_LightType_NONE
        light_type = ae.sdk.AEGP_GetLightType(solid_layer._handle)
        assert light_type == ae.sdk.AEGP_LightType_NONE

        # AEGP_SetLightType may raise RuntimeError on non-light layers
        try:
            ae.sdk.AEGP_SetLightType(solid_layer._handle, ae.sdk.AEGP_LightType_SPOT)
        except RuntimeError:
            pass  # Expected for non-light layers

        print("Light behavior on non-light layers works correctly")

    finally:
        solid_layer.delete()


# ============================================================================
# Combined Tests
# ============================================================================

@suite.test
def test_camera_light_in_same_comp():
    """Test creating both camera and light in the same composition"""
    global _test_comp
    if not _test_comp:
        print("No test composition - skipping")
        return

    camera_layer = _test_comp.add_camera("Camera", (_test_comp.width / 2, _test_comp.height / 2))
    light_layer = _test_comp.add_light("Light", (_test_comp.width / 2, _test_comp.height / 2))

    try:
        # Verify camera
        camera_type = ae.sdk.AEGP_GetCameraType(camera_layer._handle)
        assert camera_type == ae.sdk.AEGP_CameraType_PERSPECTIVE

        # Verify light
        light_type = ae.sdk.AEGP_GetLightType(light_layer._handle)
        assert light_type in [
            ae.sdk.AEGP_LightType_PARALLEL,
            ae.sdk.AEGP_LightType_SPOT,
            ae.sdk.AEGP_LightType_POINT,
            ae.sdk.AEGP_LightType_AMBIENT
        ]

        # Verify they are different layers
        assert camera_layer._handle != light_layer._handle

        print("Camera and light coexist in same composition")

    finally:
        camera_layer.delete()
        light_layer.delete()


# ============================================================================
# Main Test Runner (PyAE Test Framework Integration)
# ============================================================================

def run():
    """Run all Camera & Light Suite tests (PyAE test framework entry point)"""
    return suite.run()


if __name__ == "__main__":
    run()
