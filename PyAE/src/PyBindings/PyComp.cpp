// PyComp.cpp
// PyAE - Python for After Effects
// コンポジションAPIバインディング
//
// PYAE-008: クラス定義をPyCompClasses.hに分離

#include "PyCompClasses.h"
#include "PyLayerClasses.h"

void init_comp(pybind11::module_& m) {
    namespace py = pybind11;

    // Compクラス
    py::class_<PyAE::PyComp>(m, "Comp")
        .def(py::init<>())
        // 基本情報
        .def_property_readonly("valid", &PyAE::PyComp::IsValid,
                              "Check if comp is valid")
        .def_property("name", &PyAE::PyComp::GetName, &PyAE::PyComp::SetName,
                     "Composition name")
        .def_property_readonly("width", &PyAE::PyComp::GetWidth,
                              "Width in pixels")
        .def_property_readonly("height", &PyAE::PyComp::GetHeight,
                              "Height in pixels")
        .def_property_readonly("resolution", &PyAE::PyComp::GetResolution,
                              "Resolution as (width, height) tuple")
        // 時間
        .def_property("duration", &PyAE::PyComp::GetDuration, &PyAE::PyComp::SetDuration,
                               "Duration in seconds")
        .def_property("frame_rate", &PyAE::PyComp::GetFrameRate, &PyAE::PyComp::SetFrameRate,
                     "Frame rate")
        .def_property("current_time", &PyAE::PyComp::GetCTITime, &PyAE::PyComp::SetCTITime,
                     "Current time indicator position")
        // レイヤー
        .def_property_readonly("num_layers", &PyAE::PyComp::GetNumLayers,
                              "Number of layers")
        .def("layer", &PyAE::PyComp::GetLayer,
             "Get layer by index (0-based) or name",
             py::arg("key"))
        .def("layer_by_name", &PyAE::PyComp::GetLayerByName,
             "Get layer by name",
             py::arg("name"))
        .def_property_readonly("layers", &PyAE::PyComp::GetLayers,
                              "List of all layers")
        .def_property_readonly("selected_layers", &PyAE::PyComp::GetSelectedLayers,
                              "List of selected layers")
        // レイヤー追加
        .def("add_solid", [](PyAE::PyComp& self, const std::string& name, int width, int height,
                             py::object color, double duration) {
            py::sequence seq = color.cast<py::sequence>();
            if (py::len(seq) < 3) throw std::invalid_argument("Color must have at least 3 elements (RGB)");
            return self.AddSolid(name, width, height,
                                 seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>(), duration);
        }, "Add a solid layer",
             py::arg("name"),
             py::arg("width"),
             py::arg("height"),
             py::arg("color") = py::make_tuple(1.0, 1.0, 1.0),
             py::arg("duration") = -1.0)
        .def("add_null", &PyAE::PyComp::AddNull,
             "Add a null layer",
             py::arg("name") = "Null",
             py::arg("duration") = -1.0)
        .def("add_camera", [](PyAE::PyComp& self, const std::string& name, py::object center) {
            py::sequence seq = center.cast<py::sequence>();
            if (py::len(seq) < 2) throw std::invalid_argument("Center must have at least 2 elements (x, y)");
            return self.AddCamera(name, seq[0].cast<double>(), seq[1].cast<double>());
        }, "Add a camera layer",
             py::arg("name") = "Camera",
             py::arg("center") = py::make_tuple(0.0, 0.0))
        .def("add_light", [](PyAE::PyComp& self, const std::string& name, py::object center) {
            py::sequence seq = center.cast<py::sequence>();
            if (py::len(seq) < 2) throw std::invalid_argument("Center must have at least 2 elements (x, y)");
            return self.AddLight(name, seq[0].cast<double>(), seq[1].cast<double>());
        }, "Add a light layer",
             py::arg("name") = "Light",
             py::arg("center") = py::make_tuple(0.0, 0.0))
        .def("add_layer", [](PyAE::PyComp& self, py::object item, double duration) {
            // PyItemからAEGP_ItemHを取得
            AEGP_ItemH itemH = nullptr;
            if (py::hasattr(item, "_handle")) {
                uintptr_t ptr = item.attr("_handle").cast<uintptr_t>();
                itemH = reinterpret_cast<AEGP_ItemH>(ptr);
            } else if (py::hasattr(item, "_get_handle_ptr")) {
                // Legacy support
                uintptr_t ptr = item.attr("_get_handle_ptr")().cast<uintptr_t>();
                itemH = reinterpret_cast<AEGP_ItemH>(ptr);
            } else {
                throw std::runtime_error("Invalid item type - expected Item or ItemRef");
            }
            return self.AddLayer(itemH, duration);
        }, "Add a layer from an item (footage, comp, etc.)",
             py::arg("item"),
             py::arg("duration") = -1.0)
        .def("add_text", &PyAE::PyComp::AddText,
             "Add a text layer",
             py::arg("text") = "")
        .def_property_readonly("_handle", [](PyAE::PyComp& self) -> uintptr_t {
             AEGP_CompH compH = self.GetHandle();
             if (!compH) return 0;
             return reinterpret_cast<uintptr_t>(compH);
        }, "Internal: Comp handle (to AEGP_CompH)")
        .def("add_box_text", &PyAE::PyComp::AddBoxText,
             "Add a box text layer",
             py::arg("box_width"),
             py::arg("box_height"),
             py::arg("horizontal") = true)
        .def("add_shape", &PyAE::PyComp::AddShape,
             "Add a shape layer")

        // 追加プロパティ
        .def_property("pixel_aspect", &PyAE::PyComp::GetPixelAspect, &PyAE::PyComp::SetPixelAspect,
                     "Pixel aspect ratio")
        .def_property("bg_color", &PyAE::PyComp::GetBgColor, &PyAE::PyComp::SetBgColor,
                     "Background color (r, g, b)")
        .def_property("work_area_start", &PyAE::PyComp::GetWorkAreaStart, &PyAE::PyComp::SetWorkAreaStart,
                     "Work area start time")
        .def_property("work_area_duration", &PyAE::PyComp::GetWorkAreaDuration, &PyAE::PyComp::SetWorkAreaDuration,
                     "Work area duration")

        // 新規プロパティ
        .def("set_dimensions", &PyAE::PyComp::SetDimensions,
             "Set comp dimensions",
             py::arg("width"), py::arg("height"))
        .def_property("display_start_time", &PyAE::PyComp::GetDisplayStartTime, &PyAE::PyComp::SetDisplayStartTime,
                     "Display start time")
        .def_property("show_layer_names", &PyAE::PyComp::GetShowLayerNames, &PyAE::PyComp::SetShowLayerNames,
                     "Show layer names (True) or source names (False)")
        .def_property("show_blend_modes", &PyAE::PyComp::GetShowBlendModes, &PyAE::PyComp::SetShowBlendModes,
                     "Show blend modes in timeline")
        .def_property("display_drop_frame", &PyAE::PyComp::GetDisplayDropFrame, &PyAE::PyComp::SetDisplayDropFrame,
                     "Display drop frame timecode")
        .def_property_readonly("frame_duration", &PyAE::PyComp::GetFrameDuration,
                              "Frame duration in seconds")
        .def_property_readonly("shutter_angle_phase", &PyAE::PyComp::GetShutterAnglePhase,
                              "Shutter angle and phase as tuple (angle, phase)")
        .def_property("motion_blur_samples", &PyAE::PyComp::GetMotionBlurSamples, &PyAE::PyComp::SetMotionBlurSamples,
                     "Suggested motion blur samples")
        .def_property("motion_blur_adaptive_sample_limit",
                     &PyAE::PyComp::GetMotionBlurAdaptiveSampleLimit,
                     &PyAE::PyComp::SetMotionBlurAdaptiveSampleLimit,
                     "Motion blur adaptive sample limit")

        // コンポジション操作
        .def("duplicate", &PyAE::PyComp::Duplicate,
             "Duplicate this composition")

        // カラープロファイル
        .def_property("color_profile", &PyAE::PyComp::GetColorProfile, &PyAE::PyComp::SetColorProfile,
                     "Working space color profile for this composition.\n"
                     "Returns ColorProfile object. Can be set with ColorProfile or ICC data (bytes).")

        // 静的メソッド
        .def_static("get_active", &PyAE::PyComp::GetActive,
                   "Get the active composition")
        .def_static("get_most_recently_used", &PyAE::PyComp::GetMostRecentlyUsed,
                   "Get the most recently used composition")
        .def_static("create", &PyAE::PyComp::Create,
                   "Create a new composition",
                   py::arg("name"),
                   py::arg("width"),
                   py::arg("height"),
                   py::arg("pixel_aspect") = 1.0,
                   py::arg("duration") = 10.0,
                   py::arg("frame_rate") = 30.0)


        // Python protocol methods
        .def("__repr__", [](const PyAE::PyComp& self) {
            if (!self.IsValid()) return std::string("<Comp: invalid>");
            std::string name = self.GetName();
            int w = self.GetWidth();
            int h = self.GetHeight();
            double fps = self.GetFrameRate();
            return std::string("<Comp: '") + name + "' " + std::to_string(w) + "x" + std::to_string(h) + " @" + std::to_string(static_cast<int>(fps)) + "fps>";
        })
        .def("__bool__", &PyAE::PyComp::IsValid)
        .def("__eq__", [](const PyAE::PyComp& self, const py::object& other) {
            if (!py::isinstance<PyAE::PyComp>(other)) return false;
            return self.GetHandle() == other.cast<PyAE::PyComp>().GetHandle();
        })
        .def("__hash__", [](const PyAE::PyComp& self) {
            return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(self.GetHandle()));
        })
        // Collection protocols (layers)
        .def("__len__", &PyAE::PyComp::GetNumLayers)
        .def("__iter__", [](PyAE::PyComp& self) {
            py::list layers = py::cast(self.GetLayers());
            return layers.attr("__iter__")();
        })
        .def("__getitem__", [](PyAE::PyComp& self, py::object key) {
            if (py::isinstance<py::int_>(key)) {
                int idx = key.cast<int>();
                int num = self.GetNumLayers();
                if (idx < 0) idx += num;
                if (idx < 0 || idx >= num) throw py::index_error("Layer index out of range");
                return self.GetLayer(py::cast(idx));
            } else if (py::isinstance<py::str>(key)) {
                auto result = self.GetLayerByName(key.cast<std::string>());
                if (!result.IsValid()) throw py::key_error(key.cast<std::string>());
                return result;
            }
            throw py::type_error("Layer index must be int or str");
        })
        .def("__contains__", [](PyAE::PyComp& self, const std::string& name) {
            auto result = self.GetLayerByName(name);
            return result.IsValid();
        })

        // Serialization methods
        .def("to_dict", [](PyAE::PyComp& self) -> py::dict {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                return serialize.attr("comp_to_dict")(self).cast<py::dict>();
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to serialize comp: ") + e.what());
            }
        }, "Export composition to dictionary")

        .def_static("from_dict", [](const py::dict& data, py::object parent_folder) -> py::object {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                // comp_from_dict returns a CompItem - return it directly
                // The caller can use the CompItem which has all the same properties as Comp
                return serialize.attr("comp_from_dict")(data, parent_folder);
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to create comp from dict: ") + e.what());
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Failed to create comp from dict: ") + e.what());
            }
        }, "Create new composition from dictionary",
           py::arg("data"),
           py::arg("parent_folder") = py::none())

        .def("update_from_dict", [](PyAE::PyComp& self, const py::dict& data) {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                serialize.attr("comp_update_from_dict")(self, data);
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to update comp from dict: ") + e.what());
            }
        }, "Update existing composition from dictionary",
           py::arg("data"));
}
