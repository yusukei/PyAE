// MathSuite.cpp
// PyAE - Python for After Effects
// AEGP_MathSuite1 bindings for matrix operations

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "A.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_MathSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_MathSuite1 - Matrix operations
    // -----------------------------------------------------------------------

    // AEGP_IdentityMatrix4 - Create a 4x4 identity matrix
    // Returns a list of 16 values representing the 4x4 matrix (row-major)
    sdk.def("AEGP_IdentityMatrix4", []() -> std::vector<double> {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.mathSuite) {
            throw std::runtime_error("Math Suite not available");
        }

        A_Matrix4 matrix;
        // Initialize to zero before calling the API
        memset(&matrix, 0, sizeof(A_Matrix4));

        A_Err err = suites.mathSuite->AEGP_IdentityMatrix4(&matrix);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_IdentityMatrix4 failed with error code: " + std::to_string(err));
        }

        // Convert A_Matrix4 to flat list (row-major order)
        std::vector<double> result(16);
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                result[row * 4 + col] = matrix.mat[row][col];
            }
        }
        return result;
    });

    // AEGP_MultiplyMatrix4 - Multiply two 4x4 matrices
    // Both inputs are lists of 16 values (row-major)
    // Returns a list of 16 values representing the result matrix
    sdk.def("AEGP_MultiplyMatrix4", [](const std::vector<double>& matA, const std::vector<double>& matB) -> std::vector<double> {
        // Validation
        if (matA.size() != 16) {
            throw std::invalid_argument("AEGP_MultiplyMatrix4: matA must have exactly 16 elements, got " + std::to_string(matA.size()));
        }
        if (matB.size() != 16) {
            throw std::invalid_argument("AEGP_MultiplyMatrix4: matB must have exactly 16 elements, got " + std::to_string(matB.size()));
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.mathSuite) {
            throw std::runtime_error("Math Suite not available");
        }

        // Convert input lists to A_Matrix4
        A_Matrix4 A, B, result;
        memset(&result, 0, sizeof(A_Matrix4));

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                A.mat[row][col] = matA[row * 4 + col];
                B.mat[row][col] = matB[row * 4 + col];
            }
        }

        A_Err err = suites.mathSuite->AEGP_MultiplyMatrix4(&A, &B, &result);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_MultiplyMatrix4 failed with error code: " + std::to_string(err));
        }

        // Convert result to flat list
        std::vector<double> resultList(16);
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                resultList[row * 4 + col] = result.mat[row][col];
            }
        }
        return resultList;
    }, py::arg("matA"), py::arg("matB"));

    // AEGP_Matrix3ToMatrix4 - Convert a 3x3 matrix to a 4x4 matrix
    // Input is a list of 9 values (row-major)
    // Returns a list of 16 values representing the 4x4 matrix
    sdk.def("AEGP_Matrix3ToMatrix4", [](const std::vector<double>& mat3) -> std::vector<double> {
        // Validation
        if (mat3.size() != 9) {
            throw std::invalid_argument("AEGP_Matrix3ToMatrix4: mat3 must have exactly 9 elements, got " + std::to_string(mat3.size()));
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.mathSuite) {
            throw std::runtime_error("Math Suite not available");
        }

        // Convert input list to A_Matrix3
        A_Matrix3 A;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                A.mat[row][col] = mat3[row * 3 + col];
            }
        }

        A_Matrix4 result;
        memset(&result, 0, sizeof(A_Matrix4));

        A_Err err = suites.mathSuite->AEGP_Matrix3ToMatrix4(&A, &result);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_Matrix3ToMatrix4 failed with error code: " + std::to_string(err));
        }

        // Convert result to flat list
        std::vector<double> resultList(16);
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                resultList[row * 4 + col] = result.mat[row][col];
            }
        }
        return resultList;
    }, py::arg("mat3"));

    // AEGP_MultiplyMatrix4by3 - Multiply a 4x4 matrix by a 3x3 matrix
    // matA is a list of 16 values, matB is a list of 9 values (both row-major)
    // Returns a list of 16 values representing the result 4x4 matrix
    sdk.def("AEGP_MultiplyMatrix4by3", [](const std::vector<double>& matA, const std::vector<double>& matB) -> std::vector<double> {
        // Validation
        if (matA.size() != 16) {
            throw std::invalid_argument("AEGP_MultiplyMatrix4by3: matA must have exactly 16 elements, got " + std::to_string(matA.size()));
        }
        if (matB.size() != 9) {
            throw std::invalid_argument("AEGP_MultiplyMatrix4by3: matB must have exactly 9 elements, got " + std::to_string(matB.size()));
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.mathSuite) {
            throw std::runtime_error("Math Suite not available");
        }

        // Convert input lists to matrices
        A_Matrix4 A;
        A_Matrix3 B;
        A_Matrix4 result;
        memset(&result, 0, sizeof(A_Matrix4));

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                A.mat[row][col] = matA[row * 4 + col];
            }
        }
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                B.mat[row][col] = matB[row * 3 + col];
            }
        }

        A_Err err = suites.mathSuite->AEGP_MultiplyMatrix4by3(&A, &B, &result);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_MultiplyMatrix4by3 failed with error code: " + std::to_string(err));
        }

        // Convert result to flat list
        std::vector<double> resultList(16);
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                resultList[row * 4 + col] = result.mat[row][col];
            }
        }
        return resultList;
    }, py::arg("matA"), py::arg("matB"));

    // AEGP_MatrixDecompose4 - Decompose a 4x4 matrix into position, scale, shear, and rotation
    // Input is a list of 16 values (row-major)
    // Returns a dict with keys: "position", "scale", "shear", "rotation"
    // Each value is a tuple of (x, y, z)
    sdk.def("AEGP_MatrixDecompose4", [](const std::vector<double>& mat4) -> py::dict {
        // Validation
        if (mat4.size() != 16) {
            throw std::invalid_argument("AEGP_MatrixDecompose4: mat4 must have exactly 16 elements, got " + std::to_string(mat4.size()));
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.mathSuite) {
            throw std::runtime_error("Math Suite not available");
        }

        // Convert input list to A_Matrix4
        A_Matrix4 A;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                A.mat[row][col] = mat4[row * 4 + col];
            }
        }

        // Output parameters - must be valid pointers, not NULL
        A_FloatPoint3 position = {0, 0, 0};
        A_FloatPoint3 scale = {0, 0, 0};
        A_FloatPoint3 shear = {0, 0, 0};
        A_FloatPoint3 rotation = {0, 0, 0};

        A_Err err = suites.mathSuite->AEGP_MatrixDecompose4(&A, &position, &scale, &shear, &rotation);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_MatrixDecompose4 failed with error code: " + std::to_string(err));
        }

        // Build result dictionary
        py::dict result;
        result["position"] = py::make_tuple(position.x, position.y, position.z);
        result["scale"] = py::make_tuple(scale.x, scale.y, scale.z);
        result["shear"] = py::make_tuple(shear.x, shear.y, shear.z);
        result["rotation"] = py::make_tuple(rotation.x, rotation.y, rotation.z);

        return result;
    }, py::arg("mat4"));

    // -----------------------------------------------------------------------
    // Helper functions for matrix construction
    // These are Python-side utilities to create common transformation matrices
    // -----------------------------------------------------------------------

    // Create a 4x4 translation matrix
    sdk.def("CreateTranslationMatrix4", [](py::object t) -> std::vector<double> {
        // Translation matrix (column-major order, translation in last column):
        // [ 1  0  0  0 ]
        // [ 0  1  0  0 ]
        // [ 0  0  1  0 ]
        // [ tx ty tz 1 ]
        py::sequence seq = t.cast<py::sequence>();
        if (py::len(seq) < 3) throw std::invalid_argument("Translation must have at least 3 elements (tx, ty, tz)");
        double tx = seq[0].cast<double>(), ty = seq[1].cast<double>(), tz = seq[2].cast<double>();
        return {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            tx,  ty,  tz,  1.0
        };
    }, py::arg("translation"));

    // Create a 4x4 scale matrix
    sdk.def("CreateScaleMatrix4", [](py::object s) -> std::vector<double> {
        // Scale matrix:
        // [ sx  0   0   0 ]
        // [ 0   sy  0   0 ]
        // [ 0   0   sz  0 ]
        // [ 0   0   0   1 ]
        py::sequence seq = s.cast<py::sequence>();
        if (py::len(seq) < 3) throw std::invalid_argument("Scale must have at least 3 elements (sx, sy, sz)");
        double sx = seq[0].cast<double>(), sy = seq[1].cast<double>(), sz = seq[2].cast<double>();
        return {
            sx,  0.0, 0.0, 0.0,
            0.0, sy,  0.0, 0.0,
            0.0, 0.0, sz,  0.0,
            0.0, 0.0, 0.0, 1.0
        };
    }, py::arg("scale"));

    // Create a 4x4 rotation matrix around X axis (angle in radians)
    sdk.def("CreateRotationXMatrix4", [](double angle) -> std::vector<double> {
        double c = std::cos(angle);
        double s = std::sin(angle);
        // Rotation around X:
        // [ 1  0   0  0 ]
        // [ 0  c  -s  0 ]
        // [ 0  s   c  0 ]
        // [ 0  0   0  1 ]
        return {
            1.0, 0.0, 0.0, 0.0,
            0.0, c,   -s,  0.0,
            0.0, s,   c,   0.0,
            0.0, 0.0, 0.0, 1.0
        };
    }, py::arg("angle"));

    // Create a 4x4 rotation matrix around Y axis (angle in radians)
    sdk.def("CreateRotationYMatrix4", [](double angle) -> std::vector<double> {
        double c = std::cos(angle);
        double s = std::sin(angle);
        // Rotation around Y:
        // [ c   0  s  0 ]
        // [ 0   1  0  0 ]
        // [-s   0  c  0 ]
        // [ 0   0  0  1 ]
        return {
            c,   0.0, s,   0.0,
            0.0, 1.0, 0.0, 0.0,
            -s,  0.0, c,   0.0,
            0.0, 0.0, 0.0, 1.0
        };
    }, py::arg("angle"));

    // Create a 4x4 rotation matrix around Z axis (angle in radians)
    sdk.def("CreateRotationZMatrix4", [](double angle) -> std::vector<double> {
        double c = std::cos(angle);
        double s = std::sin(angle);
        // Rotation around Z:
        // [ c  -s  0  0 ]
        // [ s   c  0  0 ]
        // [ 0   0  1  0 ]
        // [ 0   0  0  1 ]
        return {
            c,   -s,  0.0, 0.0,
            s,   c,   0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        };
    }, py::arg("angle"));

    // Create a 3x3 identity matrix
    sdk.def("CreateIdentityMatrix3", []() -> std::vector<double> {
        return {
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0
        };
    });

    // Create a 3x3 scale matrix
    sdk.def("CreateScaleMatrix3", [](py::object s) -> std::vector<double> {
        py::sequence seq = s.cast<py::sequence>();
        if (py::len(seq) < 2) throw std::invalid_argument("Scale must have at least 2 elements (sx, sy)");
        double sx = seq[0].cast<double>(), sy = seq[1].cast<double>();
        return {
            sx,  0.0, 0.0,
            0.0, sy,  0.0,
            0.0, 0.0, 1.0
        };
    }, py::arg("scale"));

    // Create a 3x3 rotation matrix (angle in radians)
    sdk.def("CreateRotationMatrix3", [](double angle) -> std::vector<double> {
        double c = std::cos(angle);
        double s = std::sin(angle);
        return {
            c,   -s,  0.0,
            s,   c,   0.0,
            0.0, 0.0, 1.0
        };
    }, py::arg("angle"));

    // Create a 3x3 translation matrix (2D translation in homogeneous coordinates)
    sdk.def("CreateTranslationMatrix3", [](py::object t) -> std::vector<double> {
        py::sequence seq = t.cast<py::sequence>();
        if (py::len(seq) < 2) throw std::invalid_argument("Translation must have at least 2 elements (tx, ty)");
        double tx = seq[0].cast<double>(), ty = seq[1].cast<double>();
        return {
            1.0, 0.0, tx,
            0.0, 1.0, ty,
            0.0, 0.0, 1.0
        };
    }, py::arg("translation"));
}

} // namespace SDK
} // namespace PyAE
