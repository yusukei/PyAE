#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_FootageSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_FootageSuite
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetMainFootageFromItem", [](uintptr_t itemH_ptr) -> uintptr_t {
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_FootageH footageH = nullptr;

        A_Err err = suites.footageSuite->AEGP_GetMainFootageFromItem(itemH, &footageH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetMainFootageFromItem failed");

        return reinterpret_cast<uintptr_t>(footageH);
    }, py::arg("itemH"));

    sdk.def("AEGP_GetProxyFootageFromItem", [](uintptr_t itemH_ptr) -> uintptr_t {
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");
        if (!suites.utilitySuite) throw std::runtime_error("Utility Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_FootageH footageH = nullptr;

        // Suppress error dialog - this function commonly fails when item has no proxy
        AEGP_ErrReportState errState;
        suites.utilitySuite->AEGP_StartQuietErrors(&errState);

        A_Err err = suites.footageSuite->AEGP_GetProxyFootageFromItem(itemH, &footageH);

        suites.utilitySuite->AEGP_EndQuietErrors(FALSE, &errState);

        // Return 0 (nullptr) if item has no proxy - this is not an error
        if (err != A_Err_NONE) return 0;

        return reinterpret_cast<uintptr_t>(footageH);
    }, py::arg("itemH"));

    sdk.def("AEGP_GetFootageNumFiles", [](uintptr_t footageH_ptr) -> py::tuple {
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);
        A_long num_files = 0;
        A_long files_per_frame = 0;

        A_Err err = suites.footageSuite->AEGP_GetFootageNumFiles(footageH, &num_files, &files_per_frame);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFootageNumFiles failed");

        return py::make_tuple((int)num_files, (int)files_per_frame);
    }, py::arg("footageH"));

    sdk.def("AEGP_GetFootagePath", [](int plugin_id, uintptr_t footageH_ptr, int frame_num, int file_index) -> std::string {
        using namespace PyAE::Validation;
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");
        RequireNonNegative(frame_num, "frame_num");
        RequireNonNegative(file_index, "file_index");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);
        AEGP_MemHandle pathH = nullptr;

        A_Err err = suites.footageSuite->AEGP_GetFootagePath(
            footageH,
            (A_long)frame_num,
            (A_long)file_index,
            &pathH
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFootagePath failed");
        if (!pathH) return "";

        PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, pathH);
        PyAE::ScopedMemLock lock(suites.memorySuite, pathH);
        A_UTF16Char* chars = lock.As<A_UTF16Char>();
        if (!chars) return "";

        return PyAE::StringUtils::Utf16ToUtf8(chars);
    }, py::arg("plugin_id"), py::arg("footageH"), py::arg("frame_num"), py::arg("file_index"));

    sdk.def("AEGP_GetFootageSignature", [](uintptr_t footageH_ptr) -> int {
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);
        AEGP_FootageSignature sig;

        A_Err err = suites.footageSuite->AEGP_GetFootageSignature(footageH, &sig);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFootageSignature failed");

        return (int)sig;
    }, py::arg("footageH"));

    sdk.def("AEGP_NewFootage", [](int plugin_id, const std::string& path, int interp_style) -> uintptr_t {
        using namespace PyAE::Validation;
        RequireNonEmpty(path, "path");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        std::wstring wpath = PyAE::StringUtils::Utf8ToUtf16(path);
        AEGP_FootageH footageH = nullptr;

        A_Err err = suites.footageSuite->AEGP_NewFootage(
            plugin_id,
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str()),
            nullptr,  // layer_infoP0
            nullptr,  // sequence_optionsP0
            (AEGP_InterpretationStyle)interp_style,
            nullptr,  // reserved
            &footageH
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NewFootage failed");

        return reinterpret_cast<uintptr_t>(footageH);
    }, py::arg("plugin_id"), py::arg("path"), py::arg("interp_style"));

    sdk.def("AEGP_AddFootageToProject", [](uintptr_t footageH_ptr, uintptr_t folderH_ptr) -> uintptr_t {
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");
        if (folderH_ptr == 0) throw std::invalid_argument("folderH cannot be null - use AEGP_GetProjectRootFolder for root");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);
        AEGP_ItemH folderH = reinterpret_cast<AEGP_ItemH>(folderH_ptr);
        AEGP_ItemH itemH = nullptr;

        A_Err err = suites.footageSuite->AEGP_AddFootageToProject(footageH, folderH, &itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_AddFootageToProject failed");

        return reinterpret_cast<uintptr_t>(itemH);
    }, py::arg("footageH"), py::arg("folderH"));

    // Footage Signature Constants
    sdk.attr("AEGP_FootageSignature_NONE") = (int)AEGP_FootageSignature_NONE;
    sdk.attr("AEGP_FootageSignature_MISSING") = (int)AEGP_FootageSignature_MISSING;
    sdk.attr("AEGP_FootageSignature_SOLID") = (int)AEGP_FootageSignature_SOLID;

    // Interpretation Style Constants
    sdk.attr("AEGP_InterpretationStyle_NO_DIALOG_GUESS") = (int)AEGP_InterpretationStyle_NO_DIALOG_GUESS;
    sdk.attr("AEGP_InterpretationStyle_DIALOG_OK") = (int)AEGP_InterpretationStyle_DIALOG_OK;
    sdk.attr("AEGP_InterpretationStyle_NO_DIALOG_NO_GUESS") = (int)AEGP_InterpretationStyle_NO_DIALOG_NO_GUESS;

    // -----------------------------------------------------------------------
    // Footage Modification
    // -----------------------------------------------------------------------

    sdk.def("AEGP_ReplaceItemMainFootage", [](uintptr_t footageH_ptr, uintptr_t itemH_ptr) {
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);
        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        A_Err err = suites.footageSuite->AEGP_ReplaceItemMainFootage(footageH, itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_ReplaceItemMainFootage failed");
    }, py::arg("footageH"), py::arg("itemH"), "Replace item's main footage. Footage is adopted by project.");

    sdk.def("AEGP_SetItemProxyFootage", [](uintptr_t itemH_ptr, uintptr_t footageH_ptr) {
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);

        A_Err err = suites.footageSuite->AEGP_SetItemProxyFootage(footageH, itemH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetItemProxyFootage failed");
    }, py::arg("itemH"), py::arg("footageH"), "Set item's proxy footage. Footage is adopted by project.");

    sdk.def("AEGP_DisposeFootage", [](uintptr_t footageH_ptr) {
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);

        A_Err err = suites.footageSuite->AEGP_DisposeFootage(footageH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisposeFootage failed");
    }, py::arg("footageH"), "Dispose footage handle (only if not added to project).");

    sdk.def("AEGP_NewSolidFootage", [](const std::string& name, int width, int height, const py::tuple& color_rgba) -> uintptr_t {
        using namespace PyAE::Validation;
        RequireNonEmpty(name, "name");
        RequireRange(width, 1, 30000, "width");
        RequireRange(height, 1, 30000, "height");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_ColorVal c;
        if (len(color_rgba) >= 3) {
             float r = color_rgba[0].cast<float>();
             float g = color_rgba[1].cast<float>();
             float b = color_rgba[2].cast<float>();
             float a = (len(color_rgba) >= 4) ? color_rgba[3].cast<float>() : 1.0f;
             // Validate color range (0.0-1.0)
             RequireColorRange(r, "Red");
             RequireColorRange(g, "Green");
             RequireColorRange(b, "Blue");
             RequireColorRange(a, "Alpha");
             c.redF = r;
             c.greenF = g;
             c.blueF = b;
             c.alphaF = a;
        } else {
             throw std::invalid_argument("Color tuple must be (r,g,b) or (r,g,b,a)");
        }

        AEGP_FootageH footageH = nullptr;
        A_Err err = suites.footageSuite->AEGP_NewSolidFootage(name.c_str(), width, height, &c, &footageH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NewSolidFootage failed");

        return reinterpret_cast<uintptr_t>(footageH);
    }, py::arg("name"), py::arg("width"), py::arg("height"), py::arg("color_rgba"), "Create new solid footage.");

    // -----------------------------------------------------------------------
    // Footage Interpretation
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetFootageInterpretation", [](uintptr_t itemH_ptr, bool proxy) -> py::dict {
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_FootageInterp interp;

        A_Err err = suites.footageSuite->AEGP_GetFootageInterpretation(itemH, proxy ? TRUE : FALSE, &interp);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFootageInterpretation failed");

        py::dict result;
        // FIEL_Label (interlace) - extract type and order
        result["interlace_type"] = (int)interp.il.type;
        result["interlace_order"] = (int)interp.il.order;
        // AEGP_AlphaLabel - extract flags and premul color
        result["alpha_flags"] = (int)interp.al.flags;
        result["alpha_premul_color"] = py::make_tuple(
            (int)interp.al.redCu,
            (int)interp.al.greenCu,
            (int)interp.al.blueCu
        );
        result["pulldown_phase"] = (int)interp.pd;
        result["loop_count"] = (int)interp.loop.loops;
        result["pixel_aspect_num"] = (int)interp.pix_aspect_ratio.num;
        result["pixel_aspect_den"] = (int)interp.pix_aspect_ratio.den;
        result["native_fps"] = (double)interp.native_fpsF;
        result["conform_fps"] = (double)interp.conform_fpsF;
        result["depth"] = (int)interp.depthL;
        result["motion_detect"] = (bool)interp.motion_dB;

        return result;
    }, py::arg("itemH"), py::arg("proxy") = false, "Get footage interpretation settings.");

    sdk.def("AEGP_SetFootageInterpretation", [](uintptr_t itemH_ptr, bool proxy, const py::dict& interp_dict) {
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        // First get current interpretation
        AEGP_FootageInterp interp;
        A_Err err = suites.footageSuite->AEGP_GetFootageInterpretation(itemH, proxy ? TRUE : FALSE, &interp);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFootageInterpretation failed (required to update interpretation)");

        // Update fields from dict
        if (interp_dict.contains("interlace_type")) {
            interp.il.type = (FIEL_Type)interp_dict["interlace_type"].cast<uint32_t>();
        }
        if (interp_dict.contains("interlace_order")) {
            interp.il.order = (FIEL_Order)interp_dict["interlace_order"].cast<uint32_t>();
        }
        if (interp_dict.contains("alpha_flags")) {
            interp.al.flags = (AEGP_AlphaFlags)interp_dict["alpha_flags"].cast<A_u_long>();
        }
        if (interp_dict.contains("alpha_premul_color")) {
            auto color = interp_dict["alpha_premul_color"].cast<py::tuple>();
            if (py::len(color) >= 3) {
                interp.al.redCu = color[0].cast<A_u_char>();
                interp.al.greenCu = color[1].cast<A_u_char>();
                interp.al.blueCu = color[2].cast<A_u_char>();
            }
        }
        if (interp_dict.contains("pulldown_phase")) {
            interp.pd = interp_dict["pulldown_phase"].cast<A_long>();
        }
        if (interp_dict.contains("loop_count")) {
            interp.loop.loops = interp_dict["loop_count"].cast<A_long>();
            interp.loop.reserved = 0;
        }
        if (interp_dict.contains("pixel_aspect_num") && interp_dict.contains("pixel_aspect_den")) {
            interp.pix_aspect_ratio.num = interp_dict["pixel_aspect_num"].cast<A_u_long>();
            interp.pix_aspect_ratio.den = interp_dict["pixel_aspect_den"].cast<A_u_long>();
        }
        if (interp_dict.contains("native_fps"))
            interp.native_fpsF = interp_dict["native_fps"].cast<A_FpLong>();
        if (interp_dict.contains("conform_fps"))
            interp.conform_fpsF = interp_dict["conform_fps"].cast<A_FpLong>();
        if (interp_dict.contains("depth"))
            interp.depthL = interp_dict["depth"].cast<A_long>();
        if (interp_dict.contains("motion_detect"))
            interp.motion_dB = interp_dict["motion_detect"].cast<bool>() ? TRUE : FALSE;

        err = suites.footageSuite->AEGP_SetFootageInterpretation(itemH, proxy ? TRUE : FALSE, &interp);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetFootageInterpretation failed");
    }, py::arg("itemH"), py::arg("proxy"), py::arg("interp_dict"), "Set footage interpretation settings.");

    // -----------------------------------------------------------------------
    // Solid Footage Attributes
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetSolidFootageColor", [](uintptr_t itemH_ptr, bool proxy) -> py::tuple {
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_ColorVal color;

        A_Err err = suites.footageSuite->AEGP_GetSolidFootageColor(itemH, proxy ? TRUE : FALSE, &color);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetSolidFootageColor failed (item may not be solid)");

        return py::make_tuple((float)color.redF, (float)color.greenF, (float)color.blueF, (float)color.alphaF);
    }, py::arg("itemH"), py::arg("proxy") = false, "Get solid footage color (RGBA). Error if not solid.");

    sdk.def("AEGP_SetSolidFootageColor", [](uintptr_t itemH_ptr, bool proxy, const py::tuple& color_rgba) {
        using namespace PyAE::Validation;
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        AEGP_ColorVal c;
        if (len(color_rgba) >= 3) {
             float r = color_rgba[0].cast<float>();
             float g = color_rgba[1].cast<float>();
             float b = color_rgba[2].cast<float>();
             float a = (len(color_rgba) >= 4) ? color_rgba[3].cast<float>() : 1.0f;
             // Validate color range (0.0-1.0)
             RequireColorRange(r, "Red");
             RequireColorRange(g, "Green");
             RequireColorRange(b, "Blue");
             RequireColorRange(a, "Alpha");
             c.redF = r;
             c.greenF = g;
             c.blueF = b;
             c.alphaF = a;
        } else {
             throw std::invalid_argument("Color tuple must be (r,g,b) or (r,g,b,a)");
        }

        A_Err err = suites.footageSuite->AEGP_SetSolidFootageColor(itemH, proxy ? TRUE : FALSE, &c);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetSolidFootageColor failed (item may not be solid)");
    }, py::arg("itemH"), py::arg("proxy"), py::arg("color_rgba"), "Set solid footage color (RGBA). Error if not solid.");

    sdk.def("AEGP_SetSolidFootageDimensions", [](uintptr_t itemH_ptr, bool proxy, int width, int height) {
        using namespace PyAE::Validation;
        if (itemH_ptr == 0) throw std::invalid_argument("itemH cannot be null");
        RequireRange(width, 1, 30000, "width");
        RequireRange(height, 1, 30000, "height");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        A_Err err = suites.footageSuite->AEGP_SetSolidFootageDimensions(itemH, proxy ? TRUE : FALSE, (A_long)width, (A_long)height);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetSolidFootageDimensions failed (item may not be solid)");
    }, py::arg("itemH"), py::arg("proxy"), py::arg("width"), py::arg("height"), "Set solid footage dimensions. Error if not solid.");

    // -----------------------------------------------------------------------
    // Placeholder Footage
    // -----------------------------------------------------------------------

    sdk.def("AEGP_NewPlaceholderFootage", [](int plugin_id, const std::string& name, int width, int height, double duration_sec) -> uintptr_t {
        using namespace PyAE::Validation;
        RequireNonEmpty(name, "name");
        RequireRange(width, 1, 30000, "width");
        RequireRange(height, 1, 30000, "height");
        RequirePositive(duration_sec, "duration_sec");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        A_Time duration = PyAE::AETypeUtils::SecondsToTime(duration_sec);

        AEGP_FootageH footageH = nullptr;
        A_Err err = suites.footageSuite->AEGP_NewPlaceholderFootage(
            plugin_id,
            name.c_str(),
            (A_long)width,
            (A_long)height,
            &duration,
            &footageH
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NewPlaceholderFootage failed");

        return reinterpret_cast<uintptr_t>(footageH);
    }, py::arg("plugin_id"), py::arg("name"), py::arg("width"), py::arg("height"), py::arg("duration_sec"),
       "Create placeholder footage with specified dimensions and duration.");

    sdk.def("AEGP_NewPlaceholderFootageWithPath", [](int plugin_id, const std::string& path, int path_platform, int file_type, int width, int height, double duration_sec) -> uintptr_t {
        using namespace PyAE::Validation;
        RequireNonEmpty(path, "path");
        RequireRange(width, 1, 30000, "width");
        RequireRange(height, 1, 30000, "height");
        RequirePositive(duration_sec, "duration_sec");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        std::wstring wpath = PyAE::StringUtils::Utf8ToUtf16(path);

        A_Time duration;
        duration.value = static_cast<A_long>(duration_sec * 2147483648.0);
        duration.scale = 2147483648UL;

        AEGP_FootageH footageH = nullptr;
        A_Err err = suites.footageSuite->AEGP_NewPlaceholderFootageWithPath(
            plugin_id,
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str()),
            (AEGP_Platform)path_platform,
            (AEIO_FileType)file_type,
            (A_long)width,
            (A_long)height,
            &duration,
            &footageH
        );
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NewPlaceholderFootageWithPath failed");

        return reinterpret_cast<uintptr_t>(footageH);
    }, py::arg("plugin_id"), py::arg("path"), py::arg("path_platform"), py::arg("file_type"),
       py::arg("width"), py::arg("height"), py::arg("duration_sec"),
       "Create placeholder footage with path, platform, and file type.");

    // -----------------------------------------------------------------------
    // Layer Key, Sound Data, Sequence Import Options
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetFootageLayerKey", [](uintptr_t footageH_ptr) -> py::dict {
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);
        AEGP_FootageLayerKey layerKey;

        A_Err err = suites.footageSuite->AEGP_GetFootageLayerKey(footageH, &layerKey);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFootageLayerKey failed");

        py::dict result;
        result["layer_id"] = (int)layerKey.layer_idL;
        result["layer_index"] = (int)layerKey.layer_indexL;
        result["name"] = std::string(layerKey.nameAC);
        result["layer_draw_style"] = (int)layerKey.layer_draw_style;

        return result;
    }, py::arg("footageH"), "Get footage layer key information.");

    sdk.def("AEGP_GetFootageSoundDataFormat", [](uintptr_t footageH_ptr) -> py::dict {
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);
        AEGP_SoundDataFormat sound_format;

        A_Err err = suites.footageSuite->AEGP_GetFootageSoundDataFormat(footageH, &sound_format);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFootageSoundDataFormat failed");

        py::dict result;
        result["sample_rate"] = (double)sound_format.sample_rateF;
        result["encoding"] = (int)sound_format.encoding;
        result["bytes_per_sample"] = (int)sound_format.bytes_per_sampleL;
        result["num_channels"] = (int)sound_format.num_channelsL;

        return result;
    }, py::arg("footageH"), "Get footage sound data format.");

    sdk.def("AEGP_GetFootageSequenceImportOptions", [](uintptr_t footageH_ptr) -> py::dict {
        if (footageH_ptr == 0) throw std::invalid_argument("footageH cannot be null");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.footageSuite) throw std::runtime_error("Footage Suite not available");

        AEGP_FootageH footageH = reinterpret_cast<AEGP_FootageH>(footageH_ptr);
        AEGP_FileSequenceImportOptions options;

        A_Err err = suites.footageSuite->AEGP_GetFootageSequenceImportOptions(footageH, &options);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFootageSequenceImportOptions failed");

        py::dict result;
        result["all_in_folder"] = (bool)options.all_in_folderB;
        result["force_alphabetical"] = (bool)options.force_alphabeticalB;
        result["start_frame"] = (int)options.start_frameL;
        result["end_frame"] = (int)options.end_frameL;

        return result;
    }, py::arg("footageH"), "Get footage sequence import options.");

}

} // namespace SDK
} // namespace PyAE
