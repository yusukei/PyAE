#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "PluginState.h"
#include "AETypeUtils.h"

#ifdef _WIN32
#include <Windows.h>
#endif

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_IO.h"
#include "FIEL_Public.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace {
    // Helper function to convert UTF-16 to UTF-8
    std::string utf16_to_utf8(const A_UTF16Char* utf16_str) {
        if (!utf16_str || !*utf16_str) return "";

#ifdef _WIN32
        // Calculate the length of the UTF-16 string
        int utf16_len = 0;
        const A_UTF16Char* p = utf16_str;
        while (*p) { utf16_len++; p++; }

        // Calculate required buffer size for UTF-8
        int utf8_len = WideCharToMultiByte(CP_UTF8, 0,
            reinterpret_cast<const wchar_t*>(utf16_str), utf16_len,
            nullptr, 0, nullptr, nullptr);

        if (utf8_len == 0) return "";

        std::string utf8_str(utf8_len, '\0');
        WideCharToMultiByte(CP_UTF8, 0,
            reinterpret_cast<const wchar_t*>(utf16_str), utf16_len,
            &utf8_str[0], utf8_len, nullptr, nullptr);

        return utf8_str;
#else
        // Fallback for non-Windows platforms
        std::string result;
        while (*utf16_str) {
            result += static_cast<char>(*utf16_str & 0xFF);
            utf16_str++;
        }
        return result;
#endif
    }
}

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_IOOutSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_IOOutSuite6 - Output Specification Functions
    // Functions for managing output (rendering) metadata
    // -----------------------------------------------------------------------

    // 1. AEGP_GetOutSpecOptionsHandle
    sdk.def("AEGP_GetOutSpecOptionsHandle", [](uintptr_t outH_ptr) -> uintptr_t {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        void* optionsPV = nullptr;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecOptionsHandle(outH, &optionsPV);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecOptionsHandle failed");

        return reinterpret_cast<uintptr_t>(optionsPV);
    }, py::arg("outH"),
       "Get the options handle for an output specification");

    // 2. AEGP_SetOutSpecOptionsHandle
    sdk.def("AEGP_SetOutSpecOptionsHandle", [](uintptr_t outH_ptr, uintptr_t optionsPV_ptr) -> uintptr_t {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        void* optionsPV = reinterpret_cast<void*>(optionsPV_ptr);
        void* old_optionsPV = nullptr;

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecOptionsHandle(outH, optionsPV, &old_optionsPV);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecOptionsHandle failed");

        return reinterpret_cast<uintptr_t>(old_optionsPV);
    }, py::arg("outH"), py::arg("optionsPV"),
       "Set the options handle for an output specification. Returns the old options handle.");

    // 3. AEGP_GetOutSpecFilePath
    sdk.def("AEGP_GetOutSpecFilePath", [](uintptr_t outH_ptr) -> py::tuple {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEGP_MemHandle unicode_pathH = nullptr;
        A_Boolean file_reserved = FALSE;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecFilePath(outH, &unicode_pathH, &file_reserved);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecFilePath failed");

        // Convert AEGP_MemHandle to string
        std::string path;
        if (unicode_pathH) {
            // Use RAII helpers for automatic memory management
            PyAE::ScopedMemHandle scopedHandle(state.GetPluginID(), suites.memorySuite, unicode_pathH);
            PyAE::ScopedMemLock lock(suites.memorySuite, unicode_pathH);

            if (lock.IsValid()) {
                // Convert UTF-16 to UTF-8 using Windows API
                path = utf16_to_utf8(lock.As<A_UTF16Char>());
            }
            // RAII destructors will unlock and free memory
        }

        return py::make_tuple(path, static_cast<bool>(file_reserved));
    }, py::arg("outH"),
       "Get the file path for an output specification. Returns (path, file_reserved).");

    // 4. AEGP_GetOutSpecFPS
    sdk.def("AEGP_GetOutSpecFPS", [](uintptr_t outH_ptr) -> double {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Fixed native_fps = 0;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecFPS(outH, &native_fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecFPS failed");

        // A_Fixed is 16.16 fixed point
        return static_cast<double>(native_fps) / 65536.0;
    }, py::arg("outH"),
       "Get the native FPS for an output specification");

    // 5. AEGP_SetOutSpecNativeFPS
    sdk.def("AEGP_SetOutSpecNativeFPS", [](uintptr_t outH_ptr, double fps) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        // Convert double to A_Fixed (16.16 fixed point)
        A_Fixed native_fps = static_cast<A_Fixed>(fps * 65536.0);

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecNativeFPS(outH, native_fps);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecNativeFPS failed");
    }, py::arg("outH"), py::arg("fps"),
       "Set the native FPS for an output specification");

    // 6. AEGP_GetOutSpecDepth
    sdk.def("AEGP_GetOutSpecDepth", [](uintptr_t outH_ptr) -> int {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_short depth = 0;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecDepth(outH, &depth);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecDepth failed");

        return static_cast<int>(depth);
    }, py::arg("outH"),
       "Get the bit depth for an output specification");

    // 7. AEGP_SetOutSpecDepth
    sdk.def("AEGP_SetOutSpecDepth", [](uintptr_t outH_ptr, int depth) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecDepth(outH, static_cast<A_short>(depth));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecDepth failed");
    }, py::arg("outH"), py::arg("depth"),
       "Set the bit depth for an output specification");

    // 8. AEGP_GetOutSpecInterlaceLabel
    sdk.def("AEGP_GetOutSpecInterlaceLabel", [](uintptr_t outH_ptr) -> py::dict {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        FIEL_Label interlace;
        memset(&interlace, 0, sizeof(interlace));

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecInterlaceLabel(outH, &interlace);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecInterlaceLabel failed");

        py::dict result;
        result["signature"] = static_cast<uint32_t>(interlace.signature);
        result["version"] = static_cast<int>(interlace.version);
        result["type"] = static_cast<uint32_t>(interlace.type);
        result["order"] = static_cast<uint32_t>(interlace.order);
        result["reserved"] = static_cast<uint32_t>(interlace.reserved);
        return result;
    }, py::arg("outH"),
       "Get the interlace label for an output specification");

    // 9. AEGP_SetOutSpecInterlaceLabel
    sdk.def("AEGP_SetOutSpecInterlaceLabel", [](uintptr_t outH_ptr, py::dict interlace_dict) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        FIEL_Label interlace;
        memset(&interlace, 0, sizeof(interlace));

        interlace.signature = interlace_dict.contains("signature") ?
            py::cast<uint32_t>(interlace_dict["signature"]) : FIEL_Tag;
        interlace.version = interlace_dict.contains("version") ?
            static_cast<int16_t>(py::cast<int>(interlace_dict["version"])) : FIEL_Label_VERSION;
        interlace.type = interlace_dict.contains("type") ?
            py::cast<uint32_t>(interlace_dict["type"]) : FIEL_Type_FRAME_RENDERED;
        interlace.order = interlace_dict.contains("order") ?
            py::cast<uint32_t>(interlace_dict["order"]) : FIEL_Order_UPPER_FIRST;
        interlace.reserved = interlace_dict.contains("reserved") ?
            py::cast<uint32_t>(interlace_dict["reserved"]) : 0;

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecInterlaceLabel(outH, &interlace);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecInterlaceLabel failed");
    }, py::arg("outH"), py::arg("interlace"),
       "Set the interlace label for an output specification");

    // 10. AEGP_GetOutSpecAlphaLabel
    sdk.def("AEGP_GetOutSpecAlphaLabel", [](uintptr_t outH_ptr) -> py::dict {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEIO_AlphaLabel alpha;
        memset(&alpha, 0, sizeof(alpha));

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecAlphaLabel(outH, &alpha);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecAlphaLabel failed");

        py::dict result;
        result["flags"] = static_cast<uint32_t>(alpha.flags);
        result["red"] = static_cast<int>(alpha.red);
        result["green"] = static_cast<int>(alpha.green);
        result["blue"] = static_cast<int>(alpha.blue);
        result["alpha"] = static_cast<int>(alpha.alpha);
        return result;
    }, py::arg("outH"),
       "Get the alpha label for an output specification");

    // 11. AEGP_SetOutSpecAlphaLabel
    sdk.def("AEGP_SetOutSpecAlphaLabel", [](uintptr_t outH_ptr, py::dict alpha_dict) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEIO_AlphaLabel alpha;
        memset(&alpha, 0, sizeof(alpha));

        alpha.flags = alpha_dict.contains("flags") ?
            py::cast<A_u_long>(alpha_dict["flags"]) : 0;
        alpha.red = alpha_dict.contains("red") ?
            static_cast<A_u_char>(py::cast<int>(alpha_dict["red"])) : 0;
        alpha.green = alpha_dict.contains("green") ?
            static_cast<A_u_char>(py::cast<int>(alpha_dict["green"])) : 0;
        alpha.blue = alpha_dict.contains("blue") ?
            static_cast<A_u_char>(py::cast<int>(alpha_dict["blue"])) : 0;
        alpha.alpha = alpha_dict.contains("alpha") ?
            static_cast<AEIO_AlphaType>(py::cast<int>(alpha_dict["alpha"])) : AEIO_Alpha_NONE;

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecAlphaLabel(outH, &alpha);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecAlphaLabel failed");
    }, py::arg("outH"), py::arg("alpha"),
       "Set the alpha label for an output specification");

    // 12. AEGP_GetOutSpecDuration
    sdk.def("AEGP_GetOutSpecDuration", [](uintptr_t outH_ptr) -> py::tuple {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Time duration = {0, 1};

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecDuration(outH, &duration);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecDuration failed");

        return py::make_tuple(duration.value, duration.scale);
    }, py::arg("outH"),
       "Get the duration for an output specification. Returns (value, scale).");

    // 13. AEGP_SetOutSpecDuration
    sdk.def("AEGP_SetOutSpecDuration", [](uintptr_t outH_ptr, int64_t value, uint32_t scale) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Time duration;
        duration.value = static_cast<A_long>(value);
        duration.scale = static_cast<A_u_long>(scale);

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecDuration(outH, &duration);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecDuration failed");
    }, py::arg("outH"), py::arg("value"), py::arg("scale"),
       "Set the duration for an output specification");

    // 14. AEGP_GetOutSpecDimensions
    sdk.def("AEGP_GetOutSpecDimensions", [](uintptr_t outH_ptr) -> py::tuple {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_long width = 0;
        A_long height = 0;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecDimensions(outH, &width, &height);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecDimensions failed");

        return py::make_tuple(static_cast<int>(width), static_cast<int>(height));
    }, py::arg("outH"),
       "Get the dimensions for an output specification. Returns (width, height).");

    // 15. AEGP_GetOutSpecHSF
    sdk.def("AEGP_GetOutSpecHSF", [](uintptr_t outH_ptr) -> py::tuple {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Ratio hsf = {1, 1};

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecHSF(outH, &hsf);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecHSF failed");

        return py::make_tuple(hsf.num, hsf.den);
    }, py::arg("outH"),
       "Get the horizontal scale factor for an output specification. Returns (num, den).");

    // 16. AEGP_SetOutSpecHSF
    sdk.def("AEGP_SetOutSpecHSF", [](uintptr_t outH_ptr, int32_t num, uint32_t den) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Ratio hsf;
        hsf.num = static_cast<A_long>(num);
        hsf.den = static_cast<A_u_long>(den);

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecHSF(outH, &hsf);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecHSF failed");
    }, py::arg("outH"), py::arg("num"), py::arg("den"),
       "Set the horizontal scale factor for an output specification");

    // 17. AEGP_GetOutSpecSoundRate
    sdk.def("AEGP_GetOutSpecSoundRate", [](uintptr_t outH_ptr) -> double {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_FpLong rate = 0.0;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecSoundRate(outH, &rate);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecSoundRate failed");

        return static_cast<double>(rate);
    }, py::arg("outH"),
       "Get the sound sample rate for an output specification");

    // 18. AEGP_SetOutSpecSoundRate
    sdk.def("AEGP_SetOutSpecSoundRate", [](uintptr_t outH_ptr, double rate) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecSoundRate(outH, static_cast<A_FpLong>(rate));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecSoundRate failed");
    }, py::arg("outH"), py::arg("rate"),
       "Set the sound sample rate for an output specification");

    // 19. AEGP_GetOutSpecSoundEncoding
    sdk.def("AEGP_GetOutSpecSoundEncoding", [](uintptr_t outH_ptr) -> int {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEIO_SndEncoding encoding = AEIO_E_UNSIGNED_PCM;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecSoundEncoding(outH, &encoding);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecSoundEncoding failed");

        return static_cast<int>(encoding);
    }, py::arg("outH"),
       "Get the sound encoding for an output specification (1=UNSIGNED_PCM, 2=SIGNED_PCM, 3=SIGNED_FLOAT)");

    // 20. AEGP_SetOutSpecSoundEncoding
    sdk.def("AEGP_SetOutSpecSoundEncoding", [](uintptr_t outH_ptr, int encoding) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecSoundEncoding(outH, static_cast<AEIO_SndEncoding>(encoding));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecSoundEncoding failed");
    }, py::arg("outH"), py::arg("encoding"),
       "Set the sound encoding for an output specification (1=UNSIGNED_PCM, 2=SIGNED_PCM, 3=SIGNED_FLOAT)");

    // 21. AEGP_GetOutSpecSoundSampleSize
    sdk.def("AEGP_GetOutSpecSoundSampleSize", [](uintptr_t outH_ptr) -> int {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEIO_SndSampleSize bytes_per_sample = AEIO_SS_1;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecSoundSampleSize(outH, &bytes_per_sample);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecSoundSampleSize failed");

        return static_cast<int>(bytes_per_sample);
    }, py::arg("outH"),
       "Get the sound sample size in bytes for an output specification (1, 2, or 4)");

    // 22. AEGP_SetOutSpecSoundSampleSize
    sdk.def("AEGP_SetOutSpecSoundSampleSize", [](uintptr_t outH_ptr, int bytes_per_sample) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecSoundSampleSize(outH, static_cast<AEIO_SndSampleSize>(bytes_per_sample));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecSoundSampleSize failed");
    }, py::arg("outH"), py::arg("bytes_per_sample"),
       "Set the sound sample size in bytes for an output specification (1, 2, or 4)");

    // 23. AEGP_GetOutSpecSoundChannels
    sdk.def("AEGP_GetOutSpecSoundChannels", [](uintptr_t outH_ptr) -> int {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEIO_SndChannels num_channels = AEIO_SndChannels_MONO;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecSoundChannels(outH, &num_channels);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecSoundChannels failed");

        return static_cast<int>(num_channels);
    }, py::arg("outH"),
       "Get the number of sound channels for an output specification (1=MONO, 2=STEREO)");

    // 24. AEGP_SetOutSpecSoundChannels
    sdk.def("AEGP_SetOutSpecSoundChannels", [](uintptr_t outH_ptr, int num_channels) {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);

        A_Err err = suites.ioOutSuite->AEGP_SetOutSpecSoundChannels(outH, static_cast<AEIO_SndChannels>(num_channels));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetOutSpecSoundChannels failed");
    }, py::arg("outH"), py::arg("num_channels"),
       "Set the number of sound channels for an output specification (1=MONO, 2=STEREO)");

    // 25. AEGP_GetOutSpecIsStill
    sdk.def("AEGP_GetOutSpecIsStill", [](uintptr_t outH_ptr) -> bool {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Boolean is_still = FALSE;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecIsStill(outH, &is_still);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecIsStill failed");

        return static_cast<bool>(is_still);
    }, py::arg("outH"),
       "Check if the output specification is for a still image");

    // 26. AEGP_GetOutSpecPosterTime
    sdk.def("AEGP_GetOutSpecPosterTime", [](uintptr_t outH_ptr) -> py::tuple {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Time poster_time = {0, 1};

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecPosterTime(outH, &poster_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecPosterTime failed");

        return py::make_tuple(poster_time.value, poster_time.scale);
    }, py::arg("outH"),
       "Get the poster time for an output specification. Returns (value, scale).");

    // 27. AEGP_GetOutSpecStartFrame
    sdk.def("AEGP_GetOutSpecStartFrame", [](uintptr_t outH_ptr) -> int {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_long start_frame = 0;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecStartFrame(outH, &start_frame);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecStartFrame failed");

        return static_cast<int>(start_frame);
    }, py::arg("outH"),
       "Get the start frame for an output specification");

    // 28. AEGP_GetOutSpecPullDown
    sdk.def("AEGP_GetOutSpecPullDown", [](uintptr_t outH_ptr) -> int {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEIO_Pulldown pulldown = AEIO_Phase_NO_PULLDOWN;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecPullDown(outH, &pulldown);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecPullDown failed");

        return static_cast<int>(pulldown);
    }, py::arg("outH"),
       "Get the pulldown mode for an output specification");

    // 29. AEGP_GetOutSpecIsMissing
    sdk.def("AEGP_GetOutSpecIsMissing", [](uintptr_t outH_ptr) -> bool {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Boolean missing = FALSE;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecIsMissing(outH, &missing);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecIsMissing failed");

        return static_cast<bool>(missing);
    }, py::arg("outH"),
       "Check if the output file is missing");

    // 30. AEGP_GetOutSpecShouldEmbedICCProfile
    sdk.def("AEGP_GetOutSpecShouldEmbedICCProfile", [](uintptr_t outH_ptr) -> bool {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Boolean embed = FALSE;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecShouldEmbedICCProfile(outH, &embed);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecShouldEmbedICCProfile failed");

        return static_cast<bool>(embed);
    }, py::arg("outH"),
       "Check if the output should embed an ICC profile");

    // 31. AEGP_GetNewOutSpecColorProfile
    sdk.def("AEGP_GetNewOutSpecColorProfile", [](uintptr_t outH_ptr) -> uintptr_t {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEGP_ColorProfileP color_profile = nullptr;

        A_Err err = suites.ioOutSuite->AEGP_GetNewOutSpecColorProfile(plugin_id, outH, &color_profile);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNewOutSpecColorProfile failed");

        return reinterpret_cast<uintptr_t>(color_profile);
    }, py::arg("outH"),
       "Get a new color profile for an output specification. Caller must dispose with AEGP_DisposeColorProfile.");

    // 32. AEGP_GetOutSpecOutputModule
    sdk.def("AEGP_GetOutSpecOutputModule", [](uintptr_t outH_ptr) -> py::tuple {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        AEGP_RQItemRefH rq_itemH = nullptr;
        AEGP_OutputModuleRefH om_refH = nullptr;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecOutputModule(outH, &rq_itemH, &om_refH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecOutputModule failed");

        return py::make_tuple(
            reinterpret_cast<uintptr_t>(rq_itemH),
            reinterpret_cast<uintptr_t>(om_refH)
        );
    }, py::arg("outH"),
       "Get the render queue item and output module for an output specification. Returns (rq_itemH, om_refH).");

    // 33. AEGP_GetOutSpecStartTime
    sdk.def("AEGP_GetOutSpecStartTime", [](uintptr_t outH_ptr) -> py::tuple {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Time start_time = {0, 1};

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecStartTime(outH, &start_time);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecStartTime failed");

        return py::make_tuple(start_time.value, start_time.scale);
    }, py::arg("outH"),
       "Get the start time for an output specification. Returns (value, scale).");

    // 34. AEGP_GetOutSpecFrameTime
    // NOTE: This function causes memory allocation error (4160 GB) at pybind11 binding time.
    // The root cause is unclear - the code looks normal but triggers issues during module initialization.
    // Excluded until root cause is identified.
    // sdk.def("AEGP_GetOutSpecFrameTime", [](uintptr_t outH_ptr) -> py::tuple {
    //     auto& state = PyAE::PluginState::Instance();
    //     const auto& suites = state.GetSuites();
    //     if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");
    //
    //     AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
    //     A_Time frame_time = {0, 1};
    //
    //     A_Err err = suites.ioOutSuite->AEGP_GetOutSpecFrameTime(outH, &frame_time);
    //     if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecFrameTime failed");
    //
    //     return py::make_tuple(frame_time.value, frame_time.scale);
    // }, py::arg("outH"),
    //    "Get the frame time (relative to start time) for an output specification. Returns (value, scale).");

    // Additional functions found in IOOutSuite6

    // 35. AEGP_GetOutSpecIsDropFrame
    sdk.def("AEGP_GetOutSpecIsDropFrame", [](uintptr_t outH_ptr) -> bool {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_Boolean is_drop_frame = FALSE;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecIsDropFrame(outH, &is_drop_frame);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecIsDropFrame failed");

        return static_cast<bool>(is_drop_frame);
    }, py::arg("outH"),
       "Check if the output specification uses drop frame timecode");

#if defined(kAEGPIOOutSuiteVersion6)
    // Function 36 is only available in IOOutSuite6 (AE 25.2+)

    // 36. AEGP_GetOutSpecColorSpaceAsCICPIfCompatible
    sdk.def("AEGP_GetOutSpecColorSpaceAsCICPIfCompatible", [](uintptr_t outH_ptr) -> py::tuple {
        if (outH_ptr == 0) throw std::runtime_error("Invalid OutSpec handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.ioOutSuite) throw std::runtime_error("IOOut Suite not available");

        AEIO_OutSpecH outH = reinterpret_cast<AEIO_OutSpecH>(outH_ptr);
        A_long color_primaries_code = 0;
        A_long transfer_characteristics_code = 0;
        A_long matrix_coefficients_code = 0;
        A_long is_full_range = 0;
        A_Boolean is_success = FALSE;

        A_Err err = suites.ioOutSuite->AEGP_GetOutSpecColorSpaceAsCICPIfCompatible(
            outH,
            &color_primaries_code,
            &transfer_characteristics_code,
            &matrix_coefficients_code,
            &is_full_range,
            &is_success
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetOutSpecColorSpaceAsCICPIfCompatible failed");

        return py::make_tuple(
            static_cast<int>(color_primaries_code),
            static_cast<int>(transfer_characteristics_code),
            static_cast<int>(matrix_coefficients_code),
            static_cast<bool>(is_full_range != 0),
            static_cast<bool>(is_success)
        );
    }, py::arg("outH"),
       "Get the color space as CICP if compatible. Returns (color_primaries, transfer_characteristics, matrix_coefficients, is_full_range, is_success).");
#endif

}

} // namespace SDK
} // namespace PyAE
