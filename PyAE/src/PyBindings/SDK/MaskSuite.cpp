#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_EffectUI.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_MaskSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_MaskSuite
    // -----------------------------------------------------------------------
    
    sdk.def("AEGP_GetLayerNumMasks", [](uintptr_t layerH_ptr) -> int {
        // Argument validation
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerNumMasks: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
        A_long num_masks = 0;

        A_Err err = suites.maskSuite->AEGP_GetLayerNumMasks(layerH, &num_masks);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerNumMasks failed");

        return (int)num_masks;
    }, py::arg("layerH"));

    sdk.def("AEGP_GetLayerMaskByIndex", [](uintptr_t layerH_ptr, int mask_index) -> uintptr_t {
        // Argument validation
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetLayerMaskByIndex: layerH cannot be null");
        }
        Validation::RequireNonNegative(mask_index, "mask_index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);

        // Range check - get number of masks first
        A_long num_masks = 0;
        A_Err err = suites.maskSuite->AEGP_GetLayerNumMasks(layerH, &num_masks);
        if (err != A_Err_NONE) throw std::runtime_error("Failed to get layer mask count");

        // Check bounds (0-based indexing)
        Validation::RequireValidIndex(mask_index, (int)num_masks, "Mask");

        AEGP_MaskRefH maskH = nullptr;

        err = suites.maskSuite->AEGP_GetLayerMaskByIndex(layerH, (A_long)mask_index, &maskH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLayerMaskByIndex failed");

        return reinterpret_cast<uintptr_t>(maskH);
    }, py::arg("layerH"), py::arg("mask_index"));

    sdk.def("AEGP_DisposeMask", [](uintptr_t maskH_ptr) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_DisposeMask: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);

        A_Err err = suites.maskSuite->AEGP_DisposeMask(maskH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisposeMask failed");
    }, py::arg("maskH"));

    sdk.def("AEGP_GetMaskInvert", [](uintptr_t maskH_ptr) -> bool {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskInvert: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        A_Boolean invert = FALSE;

        A_Err err = suites.maskSuite->AEGP_GetMaskInvert(maskH, &invert);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskInvert failed");

        return invert != FALSE;
    }, py::arg("maskH"));

    sdk.def("AEGP_SetMaskInvert", [](uintptr_t maskH_ptr, bool invert) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskInvert: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);

        A_Err err = suites.maskSuite->AEGP_SetMaskInvert(maskH, invert ? TRUE : FALSE);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskInvert failed");
    }, py::arg("maskH"), py::arg("invert"));

    sdk.def("AEGP_GetMaskMode", [](uintptr_t maskH_ptr) -> int {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskMode: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        PF_MaskMode mode = PF_MaskMode_NONE;

        A_Err err = suites.maskSuite->AEGP_GetMaskMode(maskH, &mode);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskMode failed");

        return (int)mode;
    }, py::arg("maskH"));

    sdk.def("AEGP_SetMaskMode", [](uintptr_t maskH_ptr, int mode) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskMode: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);

        A_Err err = suites.maskSuite->AEGP_SetMaskMode(maskH, (PF_MaskMode)mode);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskMode failed");
    }, py::arg("maskH"), py::arg("mode"));

    sdk.def("AEGP_GetMaskMotionBlurState", [](uintptr_t maskH_ptr) -> int {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskMotionBlurState: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        AEGP_MaskMBlur blur_state = AEGP_MaskMBlur_SAME_AS_LAYER;

        A_Err err = suites.maskSuite->AEGP_GetMaskMotionBlurState(maskH, &blur_state);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskMotionBlurState failed");

        return (int)blur_state;
    }, py::arg("maskH"));

    sdk.def("AEGP_SetMaskMotionBlurState", [](uintptr_t maskH_ptr, int blur_state) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskMotionBlurState: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);

        A_Err err = suites.maskSuite->AEGP_SetMaskMotionBlurState(maskH, (AEGP_MaskMBlur)blur_state);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskMotionBlurState failed");
    }, py::arg("maskH"), py::arg("blur_state"));

    sdk.def("AEGP_GetMaskFeatherFalloff", [](uintptr_t maskH_ptr) -> int {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskFeatherFalloff: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        AEGP_MaskFeatherFalloff falloff = AEGP_MaskFeatherFalloff_SMOOTH;

        A_Err err = suites.maskSuite->AEGP_GetMaskFeatherFalloff(maskH, &falloff);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskFeatherFalloff failed");

        return (int)falloff;
    }, py::arg("maskH"));

    sdk.def("AEGP_SetMaskFeatherFalloff", [](uintptr_t maskH_ptr, int falloff) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskFeatherFalloff: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);

        A_Err err = suites.maskSuite->AEGP_SetMaskFeatherFalloff(maskH, (AEGP_MaskFeatherFalloff)falloff);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskFeatherFalloff failed");
    }, py::arg("maskH"), py::arg("falloff"));

    sdk.def("AEGP_GetMaskID", [](uintptr_t maskH_ptr) -> int {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskID: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        AEGP_MaskIDVal id_val = 0;

        A_Err err = suites.maskSuite->AEGP_GetMaskID(maskH, &id_val);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskID failed");

        return (int)id_val;
    }, py::arg("maskH"));

    sdk.def("AEGP_CreateNewMask", [](uintptr_t layerH_ptr) -> py::tuple {
        // Argument validation
        if (layerH_ptr == 0) {
            throw std::invalid_argument("AEGP_CreateNewMask: layerH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
        AEGP_MaskRefH maskH = nullptr;
        A_long mask_index = 0;

        A_Err err = suites.maskSuite->AEGP_CreateNewMask(layerH, &maskH, &mask_index);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_CreateNewMask failed");

        return py::make_tuple(reinterpret_cast<uintptr_t>(maskH), (int)mask_index);
    }, py::arg("layerH"));

    sdk.def("AEGP_DeleteMaskFromLayer", [](uintptr_t maskH_ptr) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_DeleteMaskFromLayer: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);

        A_Err err = suites.maskSuite->AEGP_DeleteMaskFromLayer(maskH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DeleteMaskFromLayer failed");
    }, py::arg("maskH"));

    sdk.def("AEGP_DuplicateMask", [](uintptr_t maskH_ptr) -> uintptr_t {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_DuplicateMask: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        AEGP_MaskRefH duplicate_maskH = nullptr;

        A_Err err = suites.maskSuite->AEGP_DuplicateMask(maskH, &duplicate_maskH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DuplicateMask failed");

        return reinterpret_cast<uintptr_t>(duplicate_maskH);
    }, py::arg("maskH"));

    sdk.def("AEGP_GetMaskColor", [](uintptr_t maskH_ptr) -> py::tuple {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskColor: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        AEGP_ColorVal color;

        A_Err err = suites.maskSuite->AEGP_GetMaskColor(maskH, &color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskColor failed");

        return py::make_tuple(color.redF, color.greenF, color.blueF);
    }, py::arg("maskH"));

    sdk.def("AEGP_SetMaskColor", [](uintptr_t maskH_ptr, double red, double green, double blue) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskColor: maskH cannot be null");
        }
        Validation::RequireColorRange(red, "red");
        Validation::RequireColorRange(green, "green");
        Validation::RequireColorRange(blue, "blue");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        AEGP_ColorVal color;
        color.alphaF = 1.0;
        color.redF = red;
        color.greenF = green;
        color.blueF = blue;

        A_Err err = suites.maskSuite->AEGP_SetMaskColor(maskH, &color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskColor failed");
    }, py::arg("maskH"), py::arg("red"), py::arg("green"), py::arg("blue"));

    sdk.def("AEGP_GetMaskLockState", [](uintptr_t maskH_ptr) -> bool {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskLockState: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        A_Boolean is_locked = FALSE;

        A_Err err = suites.maskSuite->AEGP_GetMaskLockState(maskH, &is_locked);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskLockState failed");

        return is_locked != FALSE;
    }, py::arg("maskH"));

    sdk.def("AEGP_SetMaskLockState", [](uintptr_t maskH_ptr, bool lock) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskLockState: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);

        A_Err err = suites.maskSuite->AEGP_SetMaskLockState(maskH, lock ? TRUE : FALSE);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskLockState failed");
    }, py::arg("maskH"), py::arg("lock"));

    sdk.def("AEGP_GetMaskIsRotoBezier", [](uintptr_t maskH_ptr) -> bool {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_GetMaskIsRotoBezier: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);
        A_Boolean is_roto_bezier = FALSE;

        A_Err err = suites.maskSuite->AEGP_GetMaskIsRotoBezier(maskH, &is_roto_bezier);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMaskIsRotoBezier failed");

        return is_roto_bezier != FALSE;
    }, py::arg("maskH"));

    sdk.def("AEGP_SetMaskIsRotoBezier", [](uintptr_t maskH_ptr, bool is_roto_bezier) {
        // Argument validation
        if (maskH_ptr == 0) {
            throw std::invalid_argument("AEGP_SetMaskIsRotoBezier: maskH cannot be null");
        }

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.maskSuite) throw std::runtime_error("Mask Suite not available");

        AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskH_ptr);

        A_Err err = suites.maskSuite->AEGP_SetMaskIsRotoBezier(maskH, is_roto_bezier ? TRUE : FALSE);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetMaskIsRotoBezier failed");
    }, py::arg("maskH"), py::arg("is_roto_bezier"));

    // Mask Mode Constants
    sdk.attr("PF_MaskMode_NONE") = (int)PF_MaskMode_NONE;
    sdk.attr("PF_MaskMode_ADD") = (int)PF_MaskMode_ADD;
    sdk.attr("PF_MaskMode_SUBTRACT") = (int)PF_MaskMode_SUBTRACT;
    sdk.attr("PF_MaskMode_INTERSECT") = (int)PF_MaskMode_INTERSECT;
    sdk.attr("PF_MaskMode_LIGHTEN") = (int)PF_MaskMode_LIGHTEN;
    sdk.attr("PF_MaskMode_DARKEN") = (int)PF_MaskMode_DARKEN;
    sdk.attr("PF_MaskMode_DIFFERENCE") = (int)PF_MaskMode_DIFFERENCE;

    // Mask Motion Blur Constants
    sdk.attr("AEGP_MaskMBlur_SAME_AS_LAYER") = (int)AEGP_MaskMBlur_SAME_AS_LAYER;
    sdk.attr("AEGP_MaskMBlur_OFF") = (int)AEGP_MaskMBlur_OFF;
    sdk.attr("AEGP_MaskMBlur_ON") = (int)AEGP_MaskMBlur_ON;

    // Mask Feather Falloff Constants
    sdk.attr("AEGP_MaskFeatherFalloff_SMOOTH") = (int)AEGP_MaskFeatherFalloff_SMOOTH;
    sdk.attr("AEGP_MaskFeatherFalloff_LINEAR") = (int)AEGP_MaskFeatherFalloff_LINEAR;
}

} // namespace SDK
} // namespace PyAE
