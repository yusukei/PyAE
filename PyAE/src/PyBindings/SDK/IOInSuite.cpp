// IOInSuite.cpp
// PyAE - Python for After Effects
// AEGP_IOInSuite7 Python Bindings
//
// Provides Python bindings for After Effects IO Input Specification Suite.
// This suite manages metadata for input footage specifications (InSpec).

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "AETypeUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_IO.h"
#include "FIEL_Public.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

// Helper: Convert A_Fixed (16.16 fixed point) to double
inline double FixedToDouble(A_Fixed fixed) {
    return static_cast<double>(fixed) / 65536.0;
}

// Helper: Convert double to A_Fixed (16.16 fixed point)
inline A_Fixed DoubleToFixed(double value) {
    return static_cast<A_Fixed>(value * 65536.0 + (value < 0 ? -0.5 : 0.5));
}

void init_IOInSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_IOInSuite7 - IO Input Specification Functions
    // -----------------------------------------------------------------------

    // 1. AEGP_GetInSpecOptionsHandle
    sdk.def("AEGP_GetInSpecOptionsHandle", [](uintptr_t inH_ptr) -> uintptr_t {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        void* optionsPV = nullptr;

        A_Err err = suites.ioInSuite->AEGP_GetInSpecOptionsHandle(inH, &optionsPV);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecOptionsHandle failed");

        return reinterpret_cast<uintptr_t>(optionsPV);
    }, py::arg("inH"),
       "Get the options handle for an input specification");

    // 2. AEGP_SetInSpecOptionsHandle
    sdk.def("AEGP_SetInSpecOptionsHandle", [](uintptr_t inH_ptr, uintptr_t optionsPV_ptr) -> uintptr_t {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        void* optionsPV = reinterpret_cast<void*>(optionsPV_ptr);
        void* old_optionsPV = nullptr;

        A_Err err = suites.ioInSuite->AEGP_SetInSpecOptionsHandle(inH, optionsPV, &old_optionsPV);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecOptionsHandle failed");

        return reinterpret_cast<uintptr_t>(old_optionsPV);
    }, py::arg("inH"), py::arg("optionsPV"),
       "Set the options handle for an input specification. Returns old options handle.");

    // 3. AEGP_GetInSpecFilePath
    sdk.def("AEGP_GetInSpecFilePath", [](uintptr_t inH_ptr) -> uintptr_t {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEGP_MemHandle unicode_pathH = nullptr;

        A_Err err = suites.ioInSuite->AEGP_GetInSpecFilePath(inH, &unicode_pathH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecFilePath failed");

        // Return AEGP_MemHandle as uintptr_t; caller must free with AEGP_FreeMemHandle
        return reinterpret_cast<uintptr_t>(unicode_pathH);
    }, py::arg("inH"),
       "Get the file path for an input specification. Returns AEGP_MemHandle (must be freed with AEGP_FreeMemHandle)");

    // 4. AEGP_GetInSpecNativeFPS
    sdk.def("AEGP_GetInSpecNativeFPS", [](uintptr_t inH_ptr) -> double {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Fixed native_fps = 0;

        A_Err err = suites.ioInSuite->AEGP_GetInSpecNativeFPS(inH, &native_fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecNativeFPS failed");

        return FixedToDouble(native_fps);
    }, py::arg("inH"),
       "Get the native frame rate of an input specification");

    // 5. AEGP_SetInSpecNativeFPS
    sdk.def("AEGP_SetInSpecNativeFPS", [](uintptr_t inH_ptr, double fps) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Fixed native_fps = DoubleToFixed(fps);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecNativeFPS(inH, native_fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecNativeFPS failed");
    }, py::arg("inH"), py::arg("fps"),
       "Set the native frame rate of an input specification");

    // 6. AEGP_GetInSpecDepth
    sdk.def("AEGP_GetInSpecDepth", [](uintptr_t inH_ptr) -> int {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_short depth = 0;

        A_Err err = suites.ioInSuite->AEGP_GetInSpecDepth(inH, &depth);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecDepth failed");

        return static_cast<int>(depth);
    }, py::arg("inH"),
       "Get the bit depth of an input specification");

    // 7. AEGP_SetInSpecDepth
    sdk.def("AEGP_SetInSpecDepth", [](uintptr_t inH_ptr, int depth) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecDepth(inH, static_cast<A_short>(depth));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecDepth failed");
    }, py::arg("inH"), py::arg("depth"),
       "Set the bit depth of an input specification");

    // 8. AEGP_GetInSpecSize
    sdk.def("AEGP_GetInSpecSize", [](uintptr_t inH_ptr) -> uint64_t {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEIO_FileSize size = 0;

        A_Err err = suites.ioInSuite->AEGP_GetInSpecSize(inH, &size);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecSize failed");

        return static_cast<uint64_t>(size);
    }, py::arg("inH"),
       "Get the file size of an input specification in bytes");

    // 9. AEGP_SetInSpecSize
    sdk.def("AEGP_SetInSpecSize", [](uintptr_t inH_ptr, uint64_t size) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecSize(inH, static_cast<AEIO_FileSize>(size));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecSize failed");
    }, py::arg("inH"), py::arg("size"),
       "Set the file size of an input specification in bytes");

    // 10. AEGP_GetInSpecInterlaceLabel
    sdk.def("AEGP_GetInSpecInterlaceLabel", [](uintptr_t inH_ptr) -> py::dict {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        FIEL_Label interlace;
        memset(&interlace, 0, sizeof(interlace));

        A_Err err = suites.ioInSuite->AEGP_GetInSpecInterlaceLabel(inH, &interlace);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecInterlaceLabel failed");

        py::dict result;
        result["signature"] = static_cast<uint32_t>(interlace.signature);
        result["version"] = static_cast<int>(interlace.version);
        result["type"] = static_cast<int>(interlace.type);
        result["order"] = static_cast<int>(interlace.order);
        result["reserved"] = static_cast<uint32_t>(interlace.reserved);
        return result;
    }, py::arg("inH"),
       "Get the interlace label of an input specification");

    // 11. AEGP_SetInSpecInterlaceLabel
    sdk.def("AEGP_SetInSpecInterlaceLabel", [](uintptr_t inH_ptr, uint32_t signature, int version, int type, int order, uint32_t reserved) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        FIEL_Label interlace;
        interlace.signature = signature;
        interlace.version = static_cast<int16_t>(version);
        interlace.type = static_cast<FIEL_Type>(type);
        interlace.order = static_cast<FIEL_Order>(order);
        interlace.reserved = reserved;

        A_Err err = suites.ioInSuite->AEGP_SetInSpecInterlaceLabel(inH, &interlace);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecInterlaceLabel failed");
    }, py::arg("inH"), py::arg("signature"), py::arg("version"), py::arg("type"), py::arg("order"), py::arg("reserved"),
       "Set the interlace label of an input specification");

    // 12. AEGP_GetInSpecAlphaLabel
    sdk.def("AEGP_GetInSpecAlphaLabel", [](uintptr_t inH_ptr) -> py::dict {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEIO_AlphaLabel alpha;
        memset(&alpha, 0, sizeof(alpha));

        A_Err err = suites.ioInSuite->AEGP_GetInSpecAlphaLabel(inH, &alpha);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecAlphaLabel failed");

        py::dict result;
        result["version"] = static_cast<int>(alpha.version);
        result["flags"] = static_cast<uint32_t>(alpha.flags);
        result["red"] = static_cast<int>(alpha.red);
        result["green"] = static_cast<int>(alpha.green);
        result["blue"] = static_cast<int>(alpha.blue);
        result["alpha"] = static_cast<int>(alpha.alpha);
        return result;
    }, py::arg("inH"),
       "Get the alpha label of an input specification");

    // 13. AEGP_SetInSpecAlphaLabel
    sdk.def("AEGP_SetInSpecAlphaLabel", [](uintptr_t inH_ptr, int version, uint32_t flags, int red, int green, int blue, int alpha_type) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEIO_AlphaLabel alpha;
        alpha.version = static_cast<A_short>(version);
        alpha.flags = static_cast<AEIO_AlphaFlags>(flags);
        alpha.red = static_cast<A_u_char>(red);
        alpha.green = static_cast<A_u_char>(green);
        alpha.blue = static_cast<A_u_char>(blue);
        alpha.alpha = static_cast<AEIO_AlphaType>(alpha_type);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecAlphaLabel(inH, &alpha);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecAlphaLabel failed");
    }, py::arg("inH"), py::arg("version"), py::arg("flags"), py::arg("red"), py::arg("green"), py::arg("blue"), py::arg("alpha_type"),
       "Set the alpha label of an input specification");

    // 14. AEGP_GetInSpecDuration
    sdk.def("AEGP_GetInSpecDuration", [](uintptr_t inH_ptr) -> py::tuple {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Time duration = {0, 1};

        A_Err err = suites.ioInSuite->AEGP_GetInSpecDuration(inH, &duration);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecDuration failed");

        return py::make_tuple(static_cast<int64_t>(duration.value), static_cast<uint32_t>(duration.scale));
    }, py::arg("inH"),
       "Get the duration of an input specification. Returns (value, scale)");

    // 15. AEGP_SetInSpecDuration
    sdk.def("AEGP_SetInSpecDuration", [](uintptr_t inH_ptr, int64_t value, uint32_t scale) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Time duration;
        duration.value = static_cast<A_long>(value);
        duration.scale = static_cast<A_u_long>(scale);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecDuration(inH, &duration);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecDuration failed");
    }, py::arg("inH"), py::arg("value"), py::arg("scale"),
       "Set the duration of an input specification");

    // 16. AEGP_GetInSpecDimensions
    sdk.def("AEGP_GetInSpecDimensions", [](uintptr_t inH_ptr) -> py::tuple {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_long width = 0, height = 0;

        A_Err err = suites.ioInSuite->AEGP_GetInSpecDimensions(inH, &width, &height);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecDimensions failed");

        return py::make_tuple(static_cast<int>(width), static_cast<int>(height));
    }, py::arg("inH"),
       "Get the dimensions of an input specification. Returns (width, height)");

    // 17. AEGP_SetInSpecDimensions
    sdk.def("AEGP_SetInSpecDimensions", [](uintptr_t inH_ptr, int width, int height) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecDimensions(inH, static_cast<A_long>(width), static_cast<A_long>(height));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecDimensions failed");
    }, py::arg("inH"), py::arg("width"), py::arg("height"),
       "Set the dimensions of an input specification");

    // 18. AEGP_InSpecGetRationalDimensions
    sdk.def("AEGP_InSpecGetRationalDimensions", [](uintptr_t inH_ptr, int rs_x_num, int rs_x_den, int rs_y_num, int rs_y_den) -> py::dict {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEIO_RationalScale rs;
        rs.x.num = static_cast<A_long>(rs_x_num);
        rs.x.den = static_cast<A_long>(rs_x_den);
        rs.y.num = static_cast<A_long>(rs_y_num);
        rs.y.den = static_cast<A_long>(rs_y_den);

        A_long width = 0, height = 0;
        A_Rect rect = {0, 0, 0, 0};

        A_Err err = suites.ioInSuite->AEGP_InSpecGetRationalDimensions(inH, &rs, &width, &height, &rect);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_InSpecGetRationalDimensions failed");

        py::dict result;
        result["width"] = static_cast<int>(width);
        result["height"] = static_cast<int>(height);
        result["rect"] = py::make_tuple(
            static_cast<int>(rect.left),
            static_cast<int>(rect.top),
            static_cast<int>(rect.right),
            static_cast<int>(rect.bottom)
        );
        return result;
    }, py::arg("inH"), py::arg("rs_x_num"), py::arg("rs_x_den"), py::arg("rs_y_num"), py::arg("rs_y_den"),
       "Get rational dimensions of an input specification. Returns {width, height, rect}");

    // 19. AEGP_GetInSpecHSF
    sdk.def("AEGP_GetInSpecHSF", [](uintptr_t inH_ptr) -> py::tuple {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Ratio hsf = {0, 1};

        A_Err err = suites.ioInSuite->AEGP_GetInSpecHSF(inH, &hsf);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecHSF failed");

        return py::make_tuple(static_cast<int>(hsf.num), static_cast<int>(hsf.den));
    }, py::arg("inH"),
       "Get the horizontal scale factor (pixel aspect ratio) of an input specification. Returns (num, den)");

    // 20. AEGP_SetInSpecHSF
    sdk.def("AEGP_SetInSpecHSF", [](uintptr_t inH_ptr, int num, int den) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Ratio hsf;
        hsf.num = static_cast<A_long>(num);
        hsf.den = static_cast<A_long>(den);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecHSF(inH, &hsf);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecHSF failed");
    }, py::arg("inH"), py::arg("num"), py::arg("den"),
       "Set the horizontal scale factor (pixel aspect ratio) of an input specification");

    // 21. AEGP_GetInSpecSoundRate
    sdk.def("AEGP_GetInSpecSoundRate", [](uintptr_t inH_ptr) -> double {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_FpLong rate = 0.0;

        A_Err err = suites.ioInSuite->AEGP_GetInSpecSoundRate(inH, &rate);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecSoundRate failed");

        return static_cast<double>(rate);
    }, py::arg("inH"),
       "Get the sound sample rate of an input specification");

    // 22. AEGP_SetInSpecSoundRate
    sdk.def("AEGP_SetInSpecSoundRate", [](uintptr_t inH_ptr, double rate) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecSoundRate(inH, static_cast<A_FpLong>(rate));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecSoundRate failed");
    }, py::arg("inH"), py::arg("rate"),
       "Set the sound sample rate of an input specification");

    // 23. AEGP_GetInSpecSoundEncoding
    sdk.def("AEGP_GetInSpecSoundEncoding", [](uintptr_t inH_ptr) -> int {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEIO_SndEncoding encoding = static_cast<AEIO_SndEncoding>(0);

        A_Err err = suites.ioInSuite->AEGP_GetInSpecSoundEncoding(inH, &encoding);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecSoundEncoding failed");

        return static_cast<int>(encoding);
    }, py::arg("inH"),
       "Get the sound encoding of an input specification (1=unsigned PCM, 2=signed PCM, 3=float)");

    // 24. AEGP_SetInSpecSoundEncoding
    sdk.def("AEGP_SetInSpecSoundEncoding", [](uintptr_t inH_ptr, int encoding) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecSoundEncoding(inH, static_cast<AEIO_SndEncoding>(encoding));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecSoundEncoding failed");
    }, py::arg("inH"), py::arg("encoding"),
       "Set the sound encoding of an input specification (1=unsigned PCM, 2=signed PCM, 3=float)");

    // 25. AEGP_GetInSpecSoundSampleSize
    sdk.def("AEGP_GetInSpecSoundSampleSize", [](uintptr_t inH_ptr) -> int {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEIO_SndSampleSize bytes_per_sample = static_cast<AEIO_SndSampleSize>(0);

        A_Err err = suites.ioInSuite->AEGP_GetInSpecSoundSampleSize(inH, &bytes_per_sample);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecSoundSampleSize failed");

        return static_cast<int>(bytes_per_sample);
    }, py::arg("inH"),
       "Get the sound sample size in bytes (1, 2, or 4)");

    // 26. AEGP_SetInSpecSoundSampleSize
    sdk.def("AEGP_SetInSpecSoundSampleSize", [](uintptr_t inH_ptr, int bytes_per_sample) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecSoundSampleSize(inH, static_cast<AEIO_SndSampleSize>(bytes_per_sample));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecSoundSampleSize failed");
    }, py::arg("inH"), py::arg("bytes_per_sample"),
       "Set the sound sample size in bytes (1, 2, or 4)");

    // 27. AEGP_GetInSpecSoundChannels
    sdk.def("AEGP_GetInSpecSoundChannels", [](uintptr_t inH_ptr) -> int {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEIO_SndChannels num_channels = static_cast<AEIO_SndChannels>(0);

        A_Err err = suites.ioInSuite->AEGP_GetInSpecSoundChannels(inH, &num_channels);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecSoundChannels failed");

        return static_cast<int>(num_channels);
    }, py::arg("inH"),
       "Get the number of sound channels (1=mono, 2=stereo)");

    // 28. AEGP_SetInSpecSoundChannels
    sdk.def("AEGP_SetInSpecSoundChannels", [](uintptr_t inH_ptr, int num_channels) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecSoundChannels(inH, static_cast<AEIO_SndChannels>(num_channels));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecSoundChannels failed");
    }, py::arg("inH"), py::arg("num_channels"),
       "Set the number of sound channels (1=mono, 2=stereo)");

    // 29. AEGP_AddAuxExtMap
    sdk.def("AEGP_AddAuxExtMap", [](const std::string& extension, int file_type, int creator) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        A_Err err = suites.ioInSuite->AEGP_AddAuxExtMap(extension.c_str(), static_cast<A_long>(file_type), static_cast<A_long>(creator));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_AddAuxExtMap failed");
    }, py::arg("extension"), py::arg("file_type"), py::arg("creator"),
       "Add an auxiliary extension mapping");

    // 30. AEGP_SetInSpecEmbeddedColorProfile
    sdk.def("AEGP_SetInSpecEmbeddedColorProfile", [](uintptr_t inH_ptr, uintptr_t color_profileP_ptr, const std::wstring& profile_desc) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEGP_ConstColorProfileP color_profileP = reinterpret_cast<AEGP_ConstColorProfileP>(color_profileP_ptr);

        // Convert wstring to UTF16 for AE
        const A_UTF16Char* profile_descP = profile_desc.empty() ? nullptr : reinterpret_cast<const A_UTF16Char*>(profile_desc.c_str());

        A_Err err = suites.ioInSuite->AEGP_SetInSpecEmbeddedColorProfile(inH, color_profileP, profile_descP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecEmbeddedColorProfile failed");
    }, py::arg("inH"), py::arg("color_profileP"), py::arg("profile_desc"),
       "Set the embedded color profile. Either color_profileP or profile_desc should be used, not both.");

    // 31. AEGP_SetInSpecAssignedColorProfile
    sdk.def("AEGP_SetInSpecAssignedColorProfile", [](uintptr_t inH_ptr, uintptr_t color_profileP_ptr) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        AEGP_ConstColorProfileP color_profileP = reinterpret_cast<AEGP_ConstColorProfileP>(color_profileP_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecAssignedColorProfile(inH, color_profileP);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecAssignedColorProfile failed");
    }, py::arg("inH"), py::arg("color_profileP"),
       "Assign a valid RGB profile to the footage");

    // 32. AEGP_GetInSpecNativeStartTime
    sdk.def("AEGP_GetInSpecNativeStartTime", [](uintptr_t inH_ptr) -> py::tuple {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Time startTime = {0, 1};

        A_Err err = suites.ioInSuite->AEGP_GetInSpecNativeStartTime(inH, &startTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecNativeStartTime failed");

        return py::make_tuple(static_cast<int64_t>(startTime.value), static_cast<uint32_t>(startTime.scale));
    }, py::arg("inH"),
       "Get the native start time of an input specification. Returns (value, scale)");

    // 33. AEGP_SetInSpecNativeStartTime
    sdk.def("AEGP_SetInSpecNativeStartTime", [](uintptr_t inH_ptr, int64_t value, uint32_t scale) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Time startTime;
        startTime.value = static_cast<A_long>(value);
        startTime.scale = static_cast<A_u_long>(scale);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecNativeStartTime(inH, &startTime);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecNativeStartTime failed");
    }, py::arg("inH"), py::arg("value"), py::arg("scale"),
       "Set the native start time of an input specification");

    // 34. AEGP_ClearInSpecNativeStartTime
    sdk.def("AEGP_ClearInSpecNativeStartTime", [](uintptr_t inH_ptr) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_ClearInSpecNativeStartTime(inH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ClearInSpecNativeStartTime failed");
    }, py::arg("inH"),
       "Clear the native start time of an input specification");

    // 35. AEGP_GetInSpecNativeDisplayDropFrame
    sdk.def("AEGP_GetInSpecNativeDisplayDropFrame", [](uintptr_t inH_ptr) -> bool {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Boolean displayDropFrame = FALSE;

        A_Err err = suites.ioInSuite->AEGP_GetInSpecNativeDisplayDropFrame(inH, &displayDropFrame);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetInSpecNativeDisplayDropFrame failed");

        return displayDropFrame != FALSE;
    }, py::arg("inH"),
       "Get whether drop frame timecode is used for display");

    // 36. AEGP_SetInSpecNativeDisplayDropFrame
    sdk.def("AEGP_SetInSpecNativeDisplayDropFrame", [](uintptr_t inH_ptr, bool displayDropFrame) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecNativeDisplayDropFrame(inH, displayDropFrame ? TRUE : FALSE);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecNativeDisplayDropFrame failed");
    }, py::arg("inH"), py::arg("displayDropFrame"),
       "Set whether drop frame timecode is used for display");

    // 37. AEGP_SetInSpecStillSequenceNativeFPS
    sdk.def("AEGP_SetInSpecStillSequenceNativeFPS", [](uintptr_t inH_ptr, double fps) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);
        A_Fixed native_still_seq_fps = DoubleToFixed(fps);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecStillSequenceNativeFPS(inH, native_still_seq_fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecStillSequenceNativeFPS failed");
    }, py::arg("inH"), py::arg("fps"),
       "Set the native FPS for a still image sequence");

#if defined(kAEGPIOInSuiteVersion7)
    // Functions 38-39 are only available in IOInSuite7 (AE 25.3+)

    // 38. AEGP_SetInSpecColorSpaceFromCICP
    sdk.def("AEGP_SetInSpecColorSpaceFromCICP", [](uintptr_t inH_ptr, int colorPrimariesCode, int transferCharacteristicsCode, int matrixCoefficientsCode, int fullRangeVideoFlag, int bitDepth, bool isRGB) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecColorSpaceFromCICP(
            inH,
            static_cast<A_long>(colorPrimariesCode),
            static_cast<A_long>(transferCharacteristicsCode),
            static_cast<A_long>(matrixCoefficientsCode),
            static_cast<A_long>(fullRangeVideoFlag),
            static_cast<A_long>(bitDepth),
            isRGB ? TRUE : FALSE
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecColorSpaceFromCICP failed");
    }, py::arg("inH"), py::arg("colorPrimariesCode"), py::arg("transferCharacteristicsCode"), py::arg("matrixCoefficientsCode"), py::arg("fullRangeVideoFlag"), py::arg("bitDepth"), py::arg("isRGB"),
       "Set color space from CICP (ITU-T-H.273) parameters");

    // 39. AEGP_SetInSpecColorSpaceFromCICP2
    sdk.def("AEGP_SetInSpecColorSpaceFromCICP2", [](uintptr_t inH_ptr, int colorPrimariesCode, int transferCharacteristicsCode, int matrixCoefficientsCode, int fullRangeVideoFlag, int bitDepth, int colorModel, int colorimetry) {
        if (inH_ptr == 0) throw std::runtime_error("Invalid InSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioInSuite) throw std::runtime_error("IO In Suite not available");

        AEIO_InSpecH inH = reinterpret_cast<AEIO_InSpecH>(inH_ptr);

        A_Err err = suites.ioInSuite->AEGP_SetInSpecColorSpaceFromCICP2(
            inH,
            static_cast<A_long>(colorPrimariesCode),
            static_cast<A_long>(transferCharacteristicsCode),
            static_cast<A_long>(matrixCoefficientsCode),
            static_cast<A_long>(fullRangeVideoFlag),
            static_cast<A_long>(bitDepth),
            static_cast<A_long>(colorModel),
            static_cast<A_long>(colorimetry)
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetInSpecColorSpaceFromCICP2 failed");
    }, py::arg("inH"), py::arg("colorPrimariesCode"), py::arg("transferCharacteristicsCode"), py::arg("matrixCoefficientsCode"), py::arg("fullRangeVideoFlag"), py::arg("bitDepth"), py::arg("colorModel"), py::arg("colorimetry"),
       "Set color space from CICP2 (extended ITU-T-H.273) parameters with color model and colorimetry");
#endif

    // -----------------------------------------------------------------------
    // Constants for AEIO types
    // -----------------------------------------------------------------------

    // FIEL_Type constants
    sdk.attr("FIEL_Type_FRAME_RENDERED") = py::int_(static_cast<int>(FIEL_Type_FRAME_RENDERED));
    sdk.attr("FIEL_Type_INTERLACED") = py::int_(static_cast<int>(FIEL_Type_INTERLACED));
    sdk.attr("FIEL_Type_HALF_HEIGHT") = py::int_(static_cast<int>(FIEL_Type_HALF_HEIGHT));
    sdk.attr("FIEL_Type_FIELD_DOUBLED") = py::int_(static_cast<int>(FIEL_Type_FIELD_DOUBLED));
    sdk.attr("FIEL_Type_UNSPECIFIED") = py::int_(static_cast<int>(FIEL_Type_UNSPECIFIED));

    // FIEL_Order constants
    sdk.attr("FIEL_Order_UPPER_FIRST") = py::int_(static_cast<int>(FIEL_Order_UPPER_FIRST));
    sdk.attr("FIEL_Order_LOWER_FIRST") = py::int_(static_cast<int>(FIEL_Order_LOWER_FIRST));

    // FIEL_Tag constant
    sdk.attr("FIEL_Tag") = py::int_(static_cast<int>(FIEL_Tag));

    // AEIO_AlphaType constants
    sdk.attr("AEIO_Alpha_STRAIGHT") = py::int_(static_cast<int>(AEIO_Alpha_STRAIGHT));
    sdk.attr("AEIO_Alpha_PREMUL") = py::int_(static_cast<int>(AEIO_Alpha_PREMUL));
    sdk.attr("AEIO_Alpha_IGNORE") = py::int_(static_cast<int>(AEIO_Alpha_IGNORE));
    sdk.attr("AEIO_Alpha_NONE") = py::int_(static_cast<int>(AEIO_Alpha_NONE));

    // AEIO_AlphaFlags constants
    sdk.attr("AEIO_AlphaPremul") = py::int_(static_cast<int>(AEIO_AlphaPremul));
    sdk.attr("AEIO_AlphaInverted") = py::int_(static_cast<int>(AEIO_AlphaInverted));

    // AEIO_AlphaLabel version constant
    sdk.attr("AEIO_AlphaLabel_VERSION") = py::int_(static_cast<int>(AEIO_AlphaLabel_VERSION));

    // AEIO_SndEncoding constants
    sdk.attr("AEIO_E_UNSIGNED_PCM") = py::int_(static_cast<int>(AEIO_E_UNSIGNED_PCM));
    sdk.attr("AEIO_E_SIGNED_PCM") = py::int_(static_cast<int>(AEIO_E_SIGNED_PCM));
    sdk.attr("AEIO_E_SIGNED_FLOAT") = py::int_(static_cast<int>(AEIO_E_SIGNED_FLOAT));

    // AEIO_SndSampleSize constants
    sdk.attr("AEIO_SS_1") = py::int_(static_cast<int>(AEIO_SS_1));
    sdk.attr("AEIO_SS_2") = py::int_(static_cast<int>(AEIO_SS_2));
    sdk.attr("AEIO_SS_4") = py::int_(static_cast<int>(AEIO_SS_4));

    // AEIO_SndChannels constants
    sdk.attr("AEIO_SndChannels_MONO") = py::int_(static_cast<int>(AEIO_SndChannels_MONO));
    sdk.attr("AEIO_SndChannels_STEREO") = py::int_(static_cast<int>(AEIO_SndChannels_STEREO));
}

} // namespace SDK
} // namespace PyAE
