#include "AE_GeneralPlug.h"
#include "AE_EffectSuites.h"
#include "SuiteManager.h"
#include "PluginState.h"
#include "../ValidationUtils.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_MaskOutlineSuite(py::module_& sdk) {

    sdk.def("AEGP_IsMaskOutlineOpen", [](uintptr_t mask_outlineH_ptr) -> bool {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_IsMaskOutlineOpen: mask_outlineH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        A_Boolean isOpen = false;
        A_Err err = suites.maskOutlineSuite->AEGP_IsMaskOutlineOpen(mask_outlineH, &isOpen);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_IsMaskOutlineOpen failed");

        return (bool)isOpen;
    }, py::arg("mask_outlineH"), "Check if mask outline is open.");

    sdk.def("AEGP_SetMaskOutlineOpen", [](uintptr_t mask_outlineH_ptr, bool open) {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskOutlineOpen: mask_outlineH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        A_Err err = suites.maskOutlineSuite->AEGP_SetMaskOutlineOpen(mask_outlineH, (A_Boolean)open);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskOutlineOpen failed");
    }, py::arg("mask_outlineH"), py::arg("open"), "Set mask outline open state.");

    sdk.def("AEGP_GetMaskOutlineNumSegments", [](uintptr_t mask_outlineH_ptr) -> int {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskOutlineNumSegments: mask_outlineH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        A_long num_segments = 0;
        A_Err err = suites.maskOutlineSuite->AEGP_GetMaskOutlineNumSegments(mask_outlineH, &num_segments);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskOutlineNumSegments failed");

        return num_segments;
    }, py::arg("mask_outlineH"), "Get number of segments in mask outline.");

    sdk.def("AEGP_GetMaskOutlineVertexInfo", [](uintptr_t mask_outlineH_ptr, int index) -> py::dict {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskOutlineVertexInfo: mask_outlineH cannot be null");
        }
        Validation::RequireNonNegative(index, "vertex index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        AEGP_MaskVertex vertex;
        A_Err err = suites.maskOutlineSuite->AEGP_GetMaskOutlineVertexInfo(mask_outlineH, index, &vertex);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskOutlineVertexInfo failed");

        py::dict d;
        d["x"] = vertex.x;
        d["y"] = vertex.y;
        d["tan_in_x"] = vertex.tan_in_x;
        d["tan_in_y"] = vertex.tan_in_y;
        d["tan_out_x"] = vertex.tan_out_x;
        d["tan_out_y"] = vertex.tan_out_y;
        return d;
    }, py::arg("mask_outlineH"), py::arg("index"), "Get vertex info at index.");

    sdk.def("AEGP_SetMaskOutlineVertexInfo", [](uintptr_t mask_outlineH_ptr, int index, const py::dict& vertex_dict) {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskOutlineVertexInfo: mask_outlineH cannot be null");
        }
        Validation::RequireNonNegative(index, "vertex index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        AEGP_MaskVertex vertex = {};

        if (vertex_dict.contains("x")) vertex.x = vertex_dict["x"].cast<double>();
        if (vertex_dict.contains("y")) vertex.y = vertex_dict["y"].cast<double>();
        if (vertex_dict.contains("tan_in_x")) vertex.tan_in_x = vertex_dict["tan_in_x"].cast<double>();
        if (vertex_dict.contains("tan_in_y")) vertex.tan_in_y = vertex_dict["tan_in_y"].cast<double>();
        if (vertex_dict.contains("tan_out_x")) vertex.tan_out_x = vertex_dict["tan_out_x"].cast<double>();
        if (vertex_dict.contains("tan_out_y")) vertex.tan_out_y = vertex_dict["tan_out_y"].cast<double>();

        A_Err err = suites.maskOutlineSuite->AEGP_SetMaskOutlineVertexInfo(mask_outlineH, index, &vertex);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskOutlineVertexInfo failed");
    }, py::arg("mask_outlineH"), py::arg("index"), py::arg("vertex"), "Set vertex info at index.");

    sdk.def("AEGP_CreateVertex", [](uintptr_t mask_outlineH_ptr, int index) {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateVertex: mask_outlineH cannot be null");
        }
        Validation::RequireNonNegative(index, "vertex index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        A_Err err = suites.maskOutlineSuite->AEGP_CreateVertex(mask_outlineH, index);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateVertex failed");
    }, py::arg("mask_outlineH"), py::arg("index"), "Create new vertex at index.");

    sdk.def("AEGP_DeleteVertex", [](uintptr_t mask_outlineH_ptr, int index) {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_DeleteVertex: mask_outlineH cannot be null");
        }
        Validation::RequireNonNegative(index, "vertex index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        A_Err err = suites.maskOutlineSuite->AEGP_DeleteVertex(mask_outlineH, index);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DeleteVertex failed");
    }, py::arg("mask_outlineH"), py::arg("index"), "Delete vertex at index.");

    // -----------------------------------------------------------------------
    // Feather Functions
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetMaskOutlineNumFeathers", [](uintptr_t mask_outlineH_ptr) -> int {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskOutlineNumFeathers: mask_outlineH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        A_long num_feathers = 0;
        A_Err err = suites.maskOutlineSuite->AEGP_GetMaskOutlineNumFeathers(mask_outlineH, &num_feathers);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskOutlineNumFeathers failed");

        return static_cast<int>(num_feathers);
    }, py::arg("mask_outlineH"), "Get number of feathers in mask outline.");

    sdk.def("AEGP_GetMaskOutlineFeatherInfo", [](uintptr_t mask_outlineH_ptr, int which_feather) -> py::dict {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskOutlineFeatherInfo: mask_outlineH cannot be null");
        }
        Validation::RequireNonNegative(which_feather, "feather index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        AEGP_MaskFeather feather;
        A_Err err = suites.maskOutlineSuite->AEGP_GetMaskOutlineFeatherInfo(
            mask_outlineH, static_cast<AEGP_FeatherIndex>(which_feather), &feather);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskOutlineFeatherInfo failed");

        py::dict d;
        d["segment"] = feather.segment;
        d["segment_sF"] = feather.segment_sF;
        d["radiusF"] = feather.radiusF;
        d["ui_corner_angleF"] = feather.ui_corner_angleF;
        d["tensionF"] = feather.tensionF;
        d["interp"] = static_cast<int>(feather.interp);
        d["type"] = static_cast<int>(feather.type);
        return d;
    }, py::arg("mask_outlineH"), py::arg("which_feather"), "Get feather info at index.");

    sdk.def("AEGP_SetMaskOutlineFeatherInfo", [](uintptr_t mask_outlineH_ptr, int which_feather, const py::dict& feather_dict) {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskOutlineFeatherInfo: mask_outlineH cannot be null");
        }
        Validation::RequireNonNegative(which_feather, "feather index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        AEGP_MaskFeather feather = {0};

        if (feather_dict.contains("segment")) feather.segment = feather_dict["segment"].cast<A_long>();
        if (feather_dict.contains("segment_sF")) feather.segment_sF = feather_dict["segment_sF"].cast<PF_FpLong>();
        if (feather_dict.contains("radiusF")) feather.radiusF = feather_dict["radiusF"].cast<PF_FpLong>();
        if (feather_dict.contains("ui_corner_angleF")) feather.ui_corner_angleF = feather_dict["ui_corner_angleF"].cast<PF_FpShort>();
        if (feather_dict.contains("tensionF")) feather.tensionF = feather_dict["tensionF"].cast<PF_FpShort>();
        if (feather_dict.contains("interp")) feather.interp = static_cast<AEGP_MaskFeatherInterp>(feather_dict["interp"].cast<int>());
        if (feather_dict.contains("type")) feather.type = static_cast<AEGP_MaskFeatherType>(feather_dict["type"].cast<int>());

        A_Err err = suites.maskOutlineSuite->AEGP_SetMaskOutlineFeatherInfo(
            mask_outlineH, static_cast<AEGP_VertexIndex>(which_feather), &feather);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskOutlineFeatherInfo failed");
    }, py::arg("mask_outlineH"), py::arg("which_feather"), py::arg("feather"), "Set feather info at index.");

    sdk.def("AEGP_CreateMaskOutlineFeather", [](uintptr_t mask_outlineH_ptr, const py::dict& feather_dict) -> int {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateMaskOutlineFeather: mask_outlineH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        AEGP_MaskFeather feather = {0};

        if (feather_dict.contains("segment")) feather.segment = feather_dict["segment"].cast<A_long>();
        if (feather_dict.contains("segment_sF")) feather.segment_sF = feather_dict["segment_sF"].cast<PF_FpLong>();
        if (feather_dict.contains("radiusF")) feather.radiusF = feather_dict["radiusF"].cast<PF_FpLong>();
        if (feather_dict.contains("ui_corner_angleF")) feather.ui_corner_angleF = feather_dict["ui_corner_angleF"].cast<PF_FpShort>();
        if (feather_dict.contains("tensionF")) feather.tensionF = feather_dict["tensionF"].cast<PF_FpShort>();
        if (feather_dict.contains("interp")) feather.interp = static_cast<AEGP_MaskFeatherInterp>(feather_dict["interp"].cast<int>());
        if (feather_dict.contains("type")) feather.type = static_cast<AEGP_MaskFeatherType>(feather_dict["type"].cast<int>());

        AEGP_FeatherIndex insert_position = 0;
        A_Err err = suites.maskOutlineSuite->AEGP_CreateMaskOutlineFeather(mask_outlineH, &feather, &insert_position);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateMaskOutlineFeather failed");

        return static_cast<int>(insert_position);
    }, py::arg("mask_outlineH"), py::arg("feather"), "Create new feather. Returns index of new feather.");

    sdk.def("AEGP_DeleteMaskOutlineFeather", [](uintptr_t mask_outlineH_ptr, int index) {
        // Validate arguments
        if (mask_outlineH_ptr == 0) {
            throw std::invalid_argument("AEGP_DeleteMaskOutlineFeather: mask_outlineH cannot be null");
        }
        Validation::RequireNonNegative(index, "feather index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskOutlineSuite) throw std::runtime_error("Mask Outline Suite not available");

        AEGP_MaskOutlineValH mask_outlineH = reinterpret_cast<AEGP_MaskOutlineValH>(mask_outlineH_ptr);
        A_Err err = suites.maskOutlineSuite->AEGP_DeleteMaskOutlineFeather(
            mask_outlineH, static_cast<AEGP_FeatherIndex>(index));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DeleteMaskOutlineFeather failed");
    }, py::arg("mask_outlineH"), py::arg("index"), "Delete feather at index.");
}

}}
