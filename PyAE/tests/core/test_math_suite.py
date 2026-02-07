"""
Math Suite Tests
Tests for AEGP_MathSuite1

MathSuiteは行列演算を提供するAPIです。
- 4x4行列の単位行列作成
- 4x4行列同士の乗算
- 3x3行列から4x4行列への変換
- 4x4行列と3x3行列の乗算
- 4x4行列の分解（位置、スケール、シア、回転）
"""
import ae
import math

try:
    from test_utils import TestSuite, assert_equal, assert_true, assert_close
except ImportError:
    from ..test_utils import TestSuite, assert_equal, assert_true, assert_close

suite = TestSuite("Math Suite")


def matrices_almost_equal(mat1, mat2, tolerance=1e-9):
    """Compare two matrices with tolerance"""
    if len(mat1) != len(mat2):
        return False
    for i, (a, b) in enumerate(zip(mat1, mat2)):
        if abs(a - b) > tolerance:
            return False
    return True


# ============================================================================
# Identity Matrix Tests
# ============================================================================

@suite.test
def test_identity_matrix4():
    """Test AEGP_IdentityMatrix4 - creates 4x4 identity matrix"""
    result = ae.sdk.AEGP_IdentityMatrix4()

    assert_equal(len(result), 16, "Identity matrix has 16 elements")

    # Expected identity matrix (row-major):
    # [1, 0, 0, 0]
    # [0, 1, 0, 0]
    # [0, 0, 1, 0]
    # [0, 0, 0, 1]
    expected = [
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    ]

    assert_true(matrices_almost_equal(result, expected),
                f"Identity matrix values correct. Got: {result}")
    print(f"Identity matrix created: diagonal=[{result[0]}, {result[5]}, {result[10]}, {result[15]}]")


# ============================================================================
# Matrix Multiplication Tests
# ============================================================================

@suite.test
def test_multiply_matrix4_identity():
    """Test AEGP_MultiplyMatrix4 with identity matrices"""
    identity = ae.sdk.AEGP_IdentityMatrix4()

    result = ae.sdk.AEGP_MultiplyMatrix4(identity, identity)

    assert_equal(len(result), 16, "Result matrix has 16 elements")
    assert_true(matrices_almost_equal(result, identity),
                "Identity * Identity = Identity")
    print("Identity * Identity = Identity (verified)")


@suite.test
def test_multiply_matrix4_translation():
    """Test AEGP_MultiplyMatrix4 with translation matrices"""
    identity = ae.sdk.AEGP_IdentityMatrix4()

    # Create a translation matrix: translate by (10, 20, 30)
    trans = ae.sdk.CreateTranslationMatrix4((10.0, 20.0, 30.0))

    # Identity * Translation = Translation
    result = ae.sdk.AEGP_MultiplyMatrix4(identity, trans)

    assert_true(matrices_almost_equal(result, trans),
                "Identity * Translation = Translation")

    # Translation * Identity = Translation
    result2 = ae.sdk.AEGP_MultiplyMatrix4(trans, identity)

    assert_true(matrices_almost_equal(result2, trans),
                "Translation * Identity = Translation")
    print(f"Translation matrix multiplication verified")


@suite.test
def test_multiply_matrix4_scale():
    """Test AEGP_MultiplyMatrix4 with scale matrices"""
    # Create scale matrices
    scale1 = ae.sdk.CreateScaleMatrix4((2.0, 3.0, 4.0))
    scale2 = ae.sdk.CreateScaleMatrix4((0.5, 0.5, 0.5))

    # 2x * 0.5x = 1x (identity-ish)
    result = ae.sdk.AEGP_MultiplyMatrix4(scale1, scale2)

    # Expected: (2*0.5, 3*0.5, 4*0.5) = (1, 1.5, 2)
    expected_scale = ae.sdk.CreateScaleMatrix4((1.0, 1.5, 2.0))

    assert_true(matrices_almost_equal(result, expected_scale),
                "Scale multiplication correct")
    print(f"Scale matrix multiplication verified")


@suite.test
def test_multiply_matrix4_combined():
    """Test AEGP_MultiplyMatrix4 with combined transformations"""
    # Scale first, then translate
    scale = ae.sdk.CreateScaleMatrix4((2.0, 2.0, 2.0))
    trans = ae.sdk.CreateTranslationMatrix4((10.0, 10.0, 10.0))

    # Note: Matrix multiplication order matters
    # trans * scale applies scale first, then translation
    result = ae.sdk.AEGP_MultiplyMatrix4(trans, scale)

    # Verify the result has the expected structure
    assert_equal(len(result), 16, "Result has 16 elements")

    # The translation should be (10, 10, 10) and scale should be (2, 2, 2)
    # But the exact result depends on multiplication order
    print(f"Combined transformation result: scale={result[0]}, trans=({result[3]}, {result[7]}, {result[11]})")


# ============================================================================
# Matrix3 to Matrix4 Conversion Tests
# ============================================================================

@suite.test
def test_matrix3_to_matrix4_identity():
    """Test AEGP_Matrix3ToMatrix4 with identity matrix"""
    # 3x3 identity matrix
    identity3 = ae.sdk.CreateIdentityMatrix3()

    result = ae.sdk.AEGP_Matrix3ToMatrix4(identity3)

    assert_equal(len(result), 16, "Result matrix has 16 elements")

    # Should produce a 4x4 identity-like matrix
    # The conversion typically extends the 3x3 to 4x4 by adding identity elements
    print(f"3x3 to 4x4 conversion result: {result}")


@suite.test
def test_matrix3_to_matrix4_scale():
    """Test AEGP_Matrix3ToMatrix4 with scale matrix"""
    # 3x3 scale matrix
    scale3 = ae.sdk.CreateScaleMatrix3((2.0, 3.0))

    result = ae.sdk.AEGP_Matrix3ToMatrix4(scale3)

    assert_equal(len(result), 16, "Result matrix has 16 elements")

    # The scale values should be preserved in the 4x4 matrix
    print(f"Scale 3x3 to 4x4: diag=[{result[0]}, {result[5]}, {result[10]}, {result[15]}]")


@suite.test
def test_matrix3_to_matrix4_rotation():
    """Test AEGP_Matrix3ToMatrix4 with rotation matrix"""
    # 3x3 rotation matrix (90 degrees)
    angle = math.pi / 2  # 90 degrees
    rot3 = ae.sdk.CreateRotationMatrix3(angle)

    result = ae.sdk.AEGP_Matrix3ToMatrix4(rot3)

    assert_equal(len(result), 16, "Result matrix has 16 elements")
    print(f"Rotation 3x3 to 4x4 converted successfully")


# ============================================================================
# Matrix4 by Matrix3 Multiplication Tests
# ============================================================================

@suite.test
def test_multiply_matrix4_by_3_identity():
    """Test AEGP_MultiplyMatrix4by3 with identity matrices"""
    identity4 = ae.sdk.AEGP_IdentityMatrix4()
    identity3 = ae.sdk.CreateIdentityMatrix3()

    result = ae.sdk.AEGP_MultiplyMatrix4by3(identity4, identity3)

    assert_equal(len(result), 16, "Result matrix has 16 elements")

    # Identity4 * Identity3 should give identity-like result
    print(f"Identity4 * Identity3 result: diag=[{result[0]}, {result[5]}, {result[10]}, {result[15]}]")


@suite.test
def test_multiply_matrix4_by_3_scale():
    """Test AEGP_MultiplyMatrix4by3 with scale matrices"""
    scale4 = ae.sdk.CreateScaleMatrix4((2.0, 2.0, 2.0))
    scale3 = ae.sdk.CreateScaleMatrix3((3.0, 3.0))

    result = ae.sdk.AEGP_MultiplyMatrix4by3(scale4, scale3)

    assert_equal(len(result), 16, "Result matrix has 16 elements")
    print(f"Scale4 * Scale3 result: diag=[{result[0]}, {result[5]}, {result[10]}, {result[15]}]")


# ============================================================================
# Matrix Decomposition Tests
# ============================================================================

@suite.test
def test_decompose_identity():
    """Test AEGP_MatrixDecompose4 with identity matrix"""
    identity = ae.sdk.AEGP_IdentityMatrix4()

    result = ae.sdk.AEGP_MatrixDecompose4(identity)

    assert_true("position" in result, "Result has position")
    assert_true("scale" in result, "Result has scale")
    assert_true("shear" in result, "Result has shear")
    assert_true("rotation" in result, "Result has rotation")

    pos = result["position"]
    scale = result["scale"]
    shear = result["shear"]
    rot = result["rotation"]

    # Identity should decompose to zero position, unit scale, zero shear, zero rotation
    print(f"Identity decomposition:")
    print(f"  Position: {pos}")
    print(f"  Scale: {scale}")
    print(f"  Shear: {shear}")
    print(f"  Rotation: {rot}")


@suite.test
def test_decompose_translation():
    """Test AEGP_MatrixDecompose4 with translation matrix"""
    trans = ae.sdk.CreateTranslationMatrix4((100.0, 200.0, 300.0))

    result = ae.sdk.AEGP_MatrixDecompose4(trans)

    pos = result["position"]
    scale = result["scale"]

    # Position should be (100, 200, 300)
    assert_close(100.0, pos[0], 1e-6, "Position X = 100")
    assert_close(200.0, pos[1], 1e-6, "Position Y = 200")
    assert_close(300.0, pos[2], 1e-6, "Position Z = 300")

    # Scale should be (1, 1, 1)
    assert_close(1.0, scale[0], 1e-6, "Scale X = 1")
    assert_close(1.0, scale[1], 1e-6, "Scale Y = 1")
    assert_close(1.0, scale[2], 1e-6, "Scale Z = 1")

    print(f"Translation decomposition: pos={pos}, scale={scale}")


@suite.test
def test_decompose_scale():
    """Test AEGP_MatrixDecompose4 with scale matrix"""
    scale_mat = ae.sdk.CreateScaleMatrix4((2.0, 3.0, 4.0))

    result = ae.sdk.AEGP_MatrixDecompose4(scale_mat)

    pos = result["position"]
    scale = result["scale"]

    # Position should be (0, 0, 0)
    assert_close(0.0, pos[0], 1e-6, "Position X = 0")
    assert_close(0.0, pos[1], 1e-6, "Position Y = 0")
    assert_close(0.0, pos[2], 1e-6, "Position Z = 0")

    # Scale should be (2, 3, 4)
    assert_close(2.0, scale[0], 1e-6, "Scale X = 2")
    assert_close(3.0, scale[1], 1e-6, "Scale Y = 3")
    assert_close(4.0, scale[2], 1e-6, "Scale Z = 4")

    print(f"Scale decomposition: pos={pos}, scale={scale}")


@suite.test
def test_decompose_rotation_z():
    """Test AEGP_MatrixDecompose4 with rotation around Z axis"""
    angle = math.pi / 4  # 45 degrees
    rot_mat = ae.sdk.CreateRotationZMatrix4(angle)

    result = ae.sdk.AEGP_MatrixDecompose4(rot_mat)

    rot = result["rotation"]
    scale = result["scale"]

    # Rotation should have Z component approximately equal to 45 degrees
    # Note: The exact representation depends on the SDK's decomposition algorithm
    print(f"Z-rotation decomposition: rotation={rot}, scale={scale}")


@suite.test
def test_decompose_combined():
    """Test AEGP_MatrixDecompose4 with combined transformation"""
    # Create a combined transformation: scale, then rotate, then translate
    scale = ae.sdk.CreateScaleMatrix4((2.0, 2.0, 1.0))
    trans = ae.sdk.CreateTranslationMatrix4((50.0, 100.0, 0.0))

    # Combine: trans * scale (apply scale first, then translation)
    combined = ae.sdk.AEGP_MultiplyMatrix4(trans, scale)

    result = ae.sdk.AEGP_MatrixDecompose4(combined)

    pos = result["position"]
    scale_result = result["scale"]
    shear = result["shear"]
    rot = result["rotation"]

    print(f"Combined transformation decomposition:")
    print(f"  Position: {pos}")
    print(f"  Scale: {scale_result}")
    print(f"  Shear: {shear}")
    print(f"  Rotation: {rot}")


# ============================================================================
# Helper Function Tests
# ============================================================================

@suite.test
def test_helper_translation_matrix():
    """Test CreateTranslationMatrix4 helper"""
    mat = ae.sdk.CreateTranslationMatrix4((10.0, 20.0, 30.0))

    assert_equal(len(mat), 16, "Matrix has 16 elements")

    # Check translation values (row 4 in column-major order)
    assert_close(10.0, mat[12], 1e-9, "TX = 10")
    assert_close(20.0, mat[13], 1e-9, "TY = 20")
    assert_close(30.0, mat[14], 1e-9, "TZ = 30")

    # Check diagonal (should be 1s)
    assert_close(1.0, mat[0], 1e-9, "M00 = 1")
    assert_close(1.0, mat[5], 1e-9, "M11 = 1")
    assert_close(1.0, mat[10], 1e-9, "M22 = 1")
    assert_close(1.0, mat[15], 1e-9, "M33 = 1")

    print("Translation matrix helper verified")


@suite.test
def test_helper_scale_matrix():
    """Test CreateScaleMatrix4 helper"""
    mat = ae.sdk.CreateScaleMatrix4((2.0, 3.0, 4.0))

    assert_equal(len(mat), 16, "Matrix has 16 elements")

    # Check scale values (diagonal)
    assert_close(2.0, mat[0], 1e-9, "SX = 2")
    assert_close(3.0, mat[5], 1e-9, "SY = 3")
    assert_close(4.0, mat[10], 1e-9, "SZ = 4")
    assert_close(1.0, mat[15], 1e-9, "M33 = 1")

    print("Scale matrix helper verified")


@suite.test
def test_helper_rotation_matrices():
    """Test CreateRotationXMatrix4, CreateRotationYMatrix4, CreateRotationZMatrix4 helpers"""
    angle = math.pi / 2  # 90 degrees

    # Test X rotation
    rot_x = ae.sdk.CreateRotationXMatrix4(angle)
    assert_equal(len(rot_x), 16, "X rotation matrix has 16 elements")
    assert_close(1.0, rot_x[0], 1e-9, "X rotation: M00 = 1")

    # Test Y rotation
    rot_y = ae.sdk.CreateRotationYMatrix4(angle)
    assert_equal(len(rot_y), 16, "Y rotation matrix has 16 elements")
    assert_close(1.0, rot_y[5], 1e-9, "Y rotation: M11 = 1")

    # Test Z rotation
    rot_z = ae.sdk.CreateRotationZMatrix4(angle)
    assert_equal(len(rot_z), 16, "Z rotation matrix has 16 elements")
    assert_close(1.0, rot_z[10], 1e-9, "Z rotation: M22 = 1")

    print("Rotation matrix helpers verified")


@suite.test
def test_helper_matrix3_functions():
    """Test Matrix3 helper functions"""
    # Identity 3x3
    identity3 = ae.sdk.CreateIdentityMatrix3()
    assert_equal(len(identity3), 9, "Identity3 has 9 elements")
    assert_close(1.0, identity3[0], 1e-9, "Identity3: M00 = 1")
    assert_close(1.0, identity3[4], 1e-9, "Identity3: M11 = 1")
    assert_close(1.0, identity3[8], 1e-9, "Identity3: M22 = 1")

    # Scale 3x3
    scale3 = ae.sdk.CreateScaleMatrix3((2.0, 3.0))
    assert_equal(len(scale3), 9, "Scale3 has 9 elements")
    assert_close(2.0, scale3[0], 1e-9, "Scale3: SX = 2")
    assert_close(3.0, scale3[4], 1e-9, "Scale3: SY = 3")

    # Rotation 3x3
    angle = math.pi / 4
    rot3 = ae.sdk.CreateRotationMatrix3(angle)
    assert_equal(len(rot3), 9, "Rotation3 has 9 elements")

    # Translation 3x3
    trans3 = ae.sdk.CreateTranslationMatrix3((10.0, 20.0))
    assert_equal(len(trans3), 9, "Translation3 has 9 elements")
    assert_close(10.0, trans3[2], 1e-9, "Translation3: TX = 10")
    assert_close(20.0, trans3[5], 1e-9, "Translation3: TY = 20")

    print("Matrix3 helper functions verified")


# ============================================================================
# Validation Tests
# ============================================================================

@suite.test
def test_multiply_matrix4_invalid_size():
    """Test AEGP_MultiplyMatrix4 with invalid matrix sizes"""
    invalid_mat = [1.0, 2.0, 3.0]  # Too small
    identity = ae.sdk.AEGP_IdentityMatrix4()

    error_raised = False
    try:
        ae.sdk.AEGP_MultiplyMatrix4(invalid_mat, identity)
    except (ValueError, RuntimeError) as e:
        error_raised = True
        print(f"Expected error for invalid matrix A size: {e}")

    assert_true(error_raised, "Error raised for invalid matrix A size")

    error_raised = False
    try:
        ae.sdk.AEGP_MultiplyMatrix4(identity, invalid_mat)
    except (ValueError, RuntimeError) as e:
        error_raised = True
        print(f"Expected error for invalid matrix B size: {e}")

    assert_true(error_raised, "Error raised for invalid matrix B size")


@suite.test
def test_matrix3_to_matrix4_invalid_size():
    """Test AEGP_Matrix3ToMatrix4 with invalid matrix size"""
    invalid_mat = [1.0, 2.0]  # Too small

    error_raised = False
    try:
        ae.sdk.AEGP_Matrix3ToMatrix4(invalid_mat)
    except (ValueError, RuntimeError) as e:
        error_raised = True
        print(f"Expected error for invalid matrix size: {e}")

    assert_true(error_raised, "Error raised for invalid matrix size")


@suite.test
def test_multiply_matrix4_by_3_invalid_sizes():
    """Test AEGP_MultiplyMatrix4by3 with invalid matrix sizes"""
    identity4 = ae.sdk.AEGP_IdentityMatrix4()
    identity3 = ae.sdk.CreateIdentityMatrix3()
    invalid_mat = [1.0, 2.0]

    error_raised = False
    try:
        ae.sdk.AEGP_MultiplyMatrix4by3(invalid_mat, identity3)
    except (ValueError, RuntimeError) as e:
        error_raised = True
        print(f"Expected error for invalid 4x4 matrix size: {e}")

    assert_true(error_raised, "Error raised for invalid 4x4 matrix size")

    error_raised = False
    try:
        ae.sdk.AEGP_MultiplyMatrix4by3(identity4, invalid_mat)
    except (ValueError, RuntimeError) as e:
        error_raised = True
        print(f"Expected error for invalid 3x3 matrix size: {e}")

    assert_true(error_raised, "Error raised for invalid 3x3 matrix size")


@suite.test
def test_decompose_invalid_size():
    """Test AEGP_MatrixDecompose4 with invalid matrix size"""
    invalid_mat = [1.0, 2.0, 3.0]  # Too small

    error_raised = False
    try:
        ae.sdk.AEGP_MatrixDecompose4(invalid_mat)
    except (ValueError, RuntimeError) as e:
        error_raised = True
        print(f"Expected error for invalid matrix size: {e}")

    assert_true(error_raised, "Error raised for invalid matrix size")


# ============================================================================
# Tuple/List Input Tests
# ============================================================================

@suite.test
def test_create_translation_matrix4_list():
    """CreateTranslationMatrix4 accepts list as well as tuple"""
    mat_tuple = ae.sdk.CreateTranslationMatrix4((10.0, 20.0, 30.0))
    mat_list = ae.sdk.CreateTranslationMatrix4([10.0, 20.0, 30.0])
    assert_true(matrices_almost_equal(mat_tuple, mat_list),
                "List version should produce same result as tuple version")


@suite.test
def test_create_scale_matrix4_list():
    """CreateScaleMatrix4 accepts list as well as tuple"""
    mat_tuple = ae.sdk.CreateScaleMatrix4((2.0, 3.0, 4.0))
    mat_list = ae.sdk.CreateScaleMatrix4([2.0, 3.0, 4.0])
    assert_true(matrices_almost_equal(mat_tuple, mat_list),
                "List version should produce same result as tuple version")


@suite.test
def test_create_scale_matrix3_list():
    """CreateScaleMatrix3 accepts list as well as tuple"""
    mat_tuple = ae.sdk.CreateScaleMatrix3((2.0, 3.0))
    mat_list = ae.sdk.CreateScaleMatrix3([2.0, 3.0])
    assert_true(matrices_almost_equal(mat_tuple, mat_list),
                "List version should produce same result as tuple version")


@suite.test
def test_create_translation_matrix3_list():
    """CreateTranslationMatrix3 accepts list as well as tuple"""
    mat_tuple = ae.sdk.CreateTranslationMatrix3((10.0, 20.0))
    mat_list = ae.sdk.CreateTranslationMatrix3([10.0, 20.0])
    assert_true(matrices_almost_equal(mat_tuple, mat_list),
                "List version should produce same result as tuple version")


# ============================================================================
# Run tests
# ============================================================================

def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
