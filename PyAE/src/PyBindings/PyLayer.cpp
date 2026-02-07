// PyLayer.cpp
// PyAE - Python for After Effects
// レイヤーAPIバインディング
//
// PYAE-008b: クラス定義をPyLayerClasses.hに分離

#include "PyLayerClasses.h"
#include "PyMarker.h"

void init_layer(pybind11::module_& m) {
    namespace py = pybind11;

    // レイヤータイプ列挙
    py::enum_<PyAE::LayerType>(m, "LayerType")
        .value("None_", PyAE::LayerType::None)  // Python予約語回避
        .value("AV", PyAE::LayerType::AV)
        .value("Light", PyAE::LayerType::Light)
        .value("Camera", PyAE::LayerType::Camera)
        .value("Text", PyAE::LayerType::Text)
        .value("Adjustment", PyAE::LayerType::Adjustment)
        .value("Null", PyAE::LayerType::Null)
        .value("Shape", PyAE::LayerType::Shape)
        .value("Solid", PyAE::LayerType::Solid);

    // レイヤー品質列挙
    py::enum_<PyAE::LayerQuality>(m, "LayerQuality", py::arithmetic())
        .value("None_", PyAE::LayerQuality::None)
        .value("Wireframe", PyAE::LayerQuality::Wireframe)
        .value("Draft", PyAE::LayerQuality::Draft)
        .value("Best", PyAE::LayerQuality::Best);

    // サンプリング品質列挙
    py::enum_<PyAE::SamplingQuality>(m, "SamplingQuality", py::arithmetic())
        .value("Bilinear", PyAE::SamplingQuality::Bilinear)
        .value("Bicubic", PyAE::SamplingQuality::Bicubic);

    // ブレンドモード列挙
    py::enum_<PyAE::BlendMode>(m, "BlendMode", py::arithmetic())
        .value("None_", PyAE::BlendMode::None)
        .value("Copy", PyAE::BlendMode::Copy)
        .value("Behind", PyAE::BlendMode::Behind)
        .value("InFront", PyAE::BlendMode::InFront)
        .value("Dissolve", PyAE::BlendMode::Dissolve)
        .value("Add", PyAE::BlendMode::Add)
        .value("Multiply", PyAE::BlendMode::Multiply)
        .value("Screen", PyAE::BlendMode::Screen)
        .value("Overlay", PyAE::BlendMode::Overlay)
        .value("SoftLight", PyAE::BlendMode::SoftLight)
        .value("HardLight", PyAE::BlendMode::HardLight)
        .value("Darken", PyAE::BlendMode::Darken)
        .value("Lighten", PyAE::BlendMode::Lighten)
        .value("Difference", PyAE::BlendMode::Difference)
        .value("Hue", PyAE::BlendMode::Hue)
        .value("Saturation", PyAE::BlendMode::Saturation)
        .value("Color", PyAE::BlendMode::Color)
        .value("Luminosity", PyAE::BlendMode::Luminosity)
        .value("MultiplyAlpha", PyAE::BlendMode::MultiplyAlpha)
        .value("MultiplyAlphaLuma", PyAE::BlendMode::MultiplyAlphaLuma)
        .value("MultiplyNotAlpha", PyAE::BlendMode::MultiplyNotAlpha)
        .value("MultiplyNotAlphaLuma", PyAE::BlendMode::MultiplyNotAlphaLuma)
        .value("AdditivePremul", PyAE::BlendMode::AdditivePremul)
        .value("AlphaAdd", PyAE::BlendMode::AlphaAdd)
        .value("ColorDodge", PyAE::BlendMode::ColorDodge)
        .value("ColorBurn", PyAE::BlendMode::ColorBurn)
        .value("Exclusion", PyAE::BlendMode::Exclusion)
        .value("Difference2", PyAE::BlendMode::Difference2)
        .value("ColorDodge2", PyAE::BlendMode::ColorDodge2)
        .value("ColorBurn2", PyAE::BlendMode::ColorBurn2)
        .value("LinearDodge", PyAE::BlendMode::LinearDodge)
        .value("LinearBurn", PyAE::BlendMode::LinearBurn)
        .value("LinearLight", PyAE::BlendMode::LinearLight)
        .value("VividLight", PyAE::BlendMode::VividLight)
        .value("PinLight", PyAE::BlendMode::PinLight)
        .value("HardMix", PyAE::BlendMode::HardMix)
        .value("LighterColor", PyAE::BlendMode::LighterColor)
        .value("DarkerColor", PyAE::BlendMode::DarkerColor)
        .value("Subtract", PyAE::BlendMode::Subtract)
        .value("Divide", PyAE::BlendMode::Divide);

    // トラックマットモード列挙
    py::enum_<PyAE::TrackMatteMode>(m, "TrackMatteMode", py::arithmetic())
        .value("NoTrackMatte", PyAE::TrackMatteMode::NoTrackMatte)
        .value("Alpha", PyAE::TrackMatteMode::Alpha)
        .value("NotAlpha", PyAE::TrackMatteMode::NotAlpha)
        .value("Luma", PyAE::TrackMatteMode::Luma)
        .value("NotLuma", PyAE::TrackMatteMode::NotLuma);

    // トラックマット定数は SDK/Constants.cpp で定義されています

    // レイヤークラス
    py::class_<PyAE::PyLayer>(m, "Layer")
        .def(py::init<>())
        // 有効性チェック
        .def_property_readonly("valid", &PyAE::PyLayer::IsValid,
                              "Check if layer is valid")
        // 基本プロパティ
        .def_property("name", &PyAE::PyLayer::GetName, &PyAE::PyLayer::SetName,
                     "Layer name")
        .def_property("comment", &PyAE::PyLayer::GetComment, &PyAE::PyLayer::SetComment,
                     "Layer comment")
        // 品質・ラベル
        .def_property("quality",
            [](const PyAE::PyLayer& self) { return static_cast<PyAE::LayerQuality>(self.GetQuality()); },
            [](PyAE::PyLayer& self, int val) { self.SetQuality(val); },
            "Layer quality")
        .def_property("sampling_quality",
            [](const PyAE::PyLayer& self) { return static_cast<PyAE::SamplingQuality>(self.GetSamplingQuality()); },
            [](PyAE::PyLayer& self, int val) { self.SetSamplingQuality(val); },
            "Layer sampling quality")
        .def_property("label", &PyAE::PyLayer::GetLabel, &PyAE::PyLayer::SetLabel,
                     "Layer label color ID (-1=NONE, 0=NO_LABEL, 1-16=color labels)")
        // マーカー
        .def("add_marker", &PyAE::PyLayer::AddMarker,
             "Add a marker with comment at specified time",
             py::arg("time"), py::arg("comment") = "")
        .def("add_marker", &PyAE::PyLayer::AddMarkerObject,
             "Add a marker object at specified time",
             py::arg("time"), py::arg("marker"))
        .def("get_marker_comment", &PyAE::PyLayer::GetMarkerComment,
             "Get marker comment at specified time",
             py::arg("time"))
        .def_property("index", &PyAE::PyLayer::GetIndex, &PyAE::PyLayer::SetIndex,
                     "Layer index (0-based)")
        // 時間関連
        .def_property("in_point", &PyAE::PyLayer::GetInPoint, &PyAE::PyLayer::SetInPoint,
                     "Layer in point in seconds")
        .def_property("out_point", &PyAE::PyLayer::GetOutPoint, &PyAE::PyLayer::SetOutPoint,
                     "Layer out point in seconds")
        .def_property("start_time", &PyAE::PyLayer::GetStartTime, &PyAE::PyLayer::SetStartTime,
                     "Layer start time (offset) in seconds")
        .def_property_readonly("duration", &PyAE::PyLayer::GetDuration,
                              "Layer duration in seconds")
        .def_property("stretch",
            &PyAE::PyLayer::GetStretch,
            [](PyAE::PyLayer& self, std::pair<int, int> stretch) {
                self.SetStretch(stretch.first, stretch.second);
            },
            "Time stretch ratio as tuple (numerator, denominator)")
        .def_property_readonly("current_time", [](const PyAE::PyLayer& self) {
            return self.GetCurrentTime();
        }, "Current time of the layer in comp time (seconds)")
        // フラグ
        .def_property("enabled", &PyAE::PyLayer::GetEnabled, &PyAE::PyLayer::SetEnabled,
                     "Layer visibility (video active)")
        .def_property("audio_enabled", &PyAE::PyLayer::GetAudioEnabled, &PyAE::PyLayer::SetAudioEnabled,
                     "Layer audio active")
        .def_property("solo", &PyAE::PyLayer::GetSolo, &PyAE::PyLayer::SetSolo,
                     "Layer solo state")
        .def_property("locked", &PyAE::PyLayer::GetLocked, &PyAE::PyLayer::SetLocked,
                     "Layer locked state")
        .def_property("shy", &PyAE::PyLayer::GetShy, &PyAE::PyLayer::SetShy,
                     "Layer shy state")
        .def_property("collapse_transformation", &PyAE::PyLayer::GetCollapseTransformation,
                     &PyAE::PyLayer::SetCollapseTransformation,
                     "Collapse transformation / Continuously rasterize")
        // 親レイヤー
        .def_property("parent", &PyAE::PyLayer::GetParent, &PyAE::PyLayer::SetParent,
                     "Parent layer (None if no parent)")
        // タイプ判定
        .def_property_readonly("layer_type", &PyAE::PyLayer::GetLayerType,
                              "Layer object type")
        .def_property_readonly("is_adjustment_layer", &PyAE::PyLayer::IsAdjustmentLayer,
                              "Check if layer is an adjustment layer")
        .def_property("is_adjustment", &PyAE::PyLayer::IsAdjustmentLayer, &PyAE::PyLayer::SetAdjustmentLayer,
                     "Adjustment layer flag")
        .def_property_readonly("is_null_layer", &PyAE::PyLayer::IsNullLayer,
                              "Check if layer is a null layer")
        .def_property_readonly("is_null", &PyAE::PyLayer::IsNullLayer,
                              "Check if layer is a null layer")
        .def_property_readonly("is_3d_layer", &PyAE::PyLayer::Is3DLayer,
                              "Check if layer is a 3D layer")
        .def_property("three_d", &PyAE::PyLayer::Is3DLayer, &PyAE::PyLayer::Set3DLayer,
                     "3D layer flag")
        .def_property("is_3d", &PyAE::PyLayer::Is3DLayer, &PyAE::PyLayer::Set3DLayer,
                     "3D layer flag (alias for three_d)")
        .def_property("time_remapping_enabled", &PyAE::PyLayer::IsTimeRemappingEnabled, &PyAE::PyLayer::SetTimeRemappingEnabled,
                     "Time remapping enabled flag")
        .def_property("selected", &PyAE::PyLayer::GetSelected, &PyAE::PyLayer::SetSelected,
                     "Layer selection state")
        // ID関連
        .def_property_readonly("id", &PyAE::PyLayer::GetLayerID,
                              "Layer ID (read-only)")
        .def_property_readonly("source_item_id", &PyAE::PyLayer::GetSourceItemID,
                              "Source item ID (read-only)")
        .def_property_readonly("object_type", &PyAE::PyLayer::GetObjectType,
                              "Object type (read-only)")
        .def_property_readonly("is_2d", &PyAE::PyLayer::Is2D,
                              "Check if layer is 2D (read-only)")
        // トラックマット
        .def_property_readonly("has_track_matte", &PyAE::PyLayer::HasTrackMatte,
                              "Check if layer has a track matte")
        .def_property_readonly("is_used_as_track_matte", &PyAE::PyLayer::IsUsedAsTrackMatte,
                              "Check if layer is used as a track matte")
        .def_property_readonly("track_matte_layer", &PyAE::PyLayer::GetTrackMatteLayer,
                              "Get the track matte layer (None if no track matte)")
        .def("set_track_matte", &PyAE::PyLayer::SetTrackMatte,
             "Set track matte for this layer",
             py::arg("matte_layer"), py::arg("matte_mode"))
        .def("remove_track_matte", &PyAE::PyLayer::RemoveTrackMatte,
             "Remove track matte from this layer")
        // Transform ショートカット
        .def_property("position", &PyAE::PyLayer::GetPosition, &PyAE::PyLayer::SetPosition,
                     "Layer position")
        .def_property("scale", &PyAE::PyLayer::GetScale, &PyAE::PyLayer::SetScale,
                     "Layer scale")
        .def_property("rotation", &PyAE::PyLayer::GetRotation, &PyAE::PyLayer::SetRotation,
                     "Layer rotation")
        .def_property("anchor_point", &PyAE::PyLayer::GetAnchorPoint, &PyAE::PyLayer::SetAnchorPoint,
                     "Layer anchor point")
        .def_property("opacity", &PyAE::PyLayer::GetOpacity, &PyAE::PyLayer::SetOpacity,
                     "Layer opacity (0-100)")
        .def_property("transfer_mode", &PyAE::PyLayer::GetTransferMode, &PyAE::PyLayer::SetTransferMode,
                     "Layer transfer mode as dict with keys: mode, flags, track_matte")
        .def("to_world_xform", &PyAE::PyLayer::ToWorldXform,
             "Get layer to world transform matrix at specified comp time",
             py::arg("comp_time"))
        // High-level Keyframe API
        .def("set_position_keyframe", &PyAE::PyLayer::SetPositionKeyframe,
             "Add a position keyframe at specified time",
             py::arg("time"), py::arg("value"))
        .def("set_scale_keyframe", &PyAE::PyLayer::SetScaleKeyframe,
             "Add a scale keyframe at specified time",
             py::arg("time"), py::arg("value"))
        .def("set_rotation_keyframe", &PyAE::PyLayer::SetRotationKeyframe,
             "Add a rotation keyframe at specified time",
             py::arg("time"), py::arg("value"))
        .def("set_opacity_keyframe", &PyAE::PyLayer::SetOpacityKeyframe,
             "Add an opacity keyframe at specified time",
             py::arg("time"), py::arg("value"))
        // 操作
        .def("delete", &PyAE::PyLayer::Delete,
             "Delete the layer")
        .def("duplicate", &PyAE::PyLayer::Duplicate,
             "Duplicate the layer")
        .def("move_to", &PyAE::PyLayer::MoveTo,
             "Move layer to specified index",
             py::arg("index"))
        // エフェクト
        .def_property_readonly("num_effects", &PyAE::PyLayer::GetNumEffects,
                              "Number of effects on layer")
        .def_property_readonly("effects", &PyAE::PyLayer::GetAllEffects,
                              "List of all effects on layer")
        .def("effect", [](PyAE::PyLayer& self, py::object index_or_name) {
            if (py::isinstance<py::int_>(index_or_name)) {
                return self.GetEffectByIndex(index_or_name.cast<int>());
            } else {
                return self.GetEffectByName(index_or_name.cast<std::string>());
            }
        }, "Get effect by index (int) or name (str)",
           py::arg("index_or_name"))
        .def("add_effect", &PyAE::PyLayer::AddEffect,
             "Add effect to layer by match name",
             py::arg("match_name"))
        // マスク
        .def_property_readonly("num_masks", &PyAE::PyLayer::GetNumMasks,
                              "Number of masks on layer")
        .def_property_readonly("masks", &PyAE::PyLayer::GetAllMasks,
                              "List of all masks on layer")
        .def("mask", [](PyAE::PyLayer& self, py::object index_or_name) {
            if (py::isinstance<py::int_>(index_or_name)) {
                return self.GetMaskByIndex(index_or_name.cast<int>());
            } else {
                return self.GetMaskByName(index_or_name.cast<std::string>());
            }
        }, "Get mask by index (int) or name (str)",
           py::arg("index_or_name"))
        .def("add_mask", &PyAE::PyLayer::AddMask,
             "Add mask to layer with vertices",
             py::arg("vertices") = py::list())
        // get_property エイリアス (property メソッドは PyProperty.cpp で追加される)
        .def("get_property", [](py::object self, const std::string& name) {
            // property メソッドを呼び出す
            return self.attr("property")(name);
        }, "Get a property by name (alias for property method)",
           py::arg("name"))
        // ハンドルアクセス（内部使用）
        .def_property_readonly("_handle", [](const PyAE::PyLayer& self) {
            return reinterpret_cast<uintptr_t>(self.GetHandle());
        }, "Internal: layer handle as integer")

        // Python protocol methods
        .def("__repr__", [](const PyAE::PyLayer& self) {
            if (!self.IsValid()) return std::string("<Layer: invalid>");
            std::string name = self.GetName();
            int idx = self.GetIndex();
            return std::string("<Layer: '") + name + "' index=" + std::to_string(idx) + ">";
        })
        .def("__bool__", &PyAE::PyLayer::IsValid)
        .def("__eq__", [](const PyAE::PyLayer& self, const py::object& other) {
            if (!py::isinstance<PyAE::PyLayer>(other)) return false;
            return self.GetHandle() == other.cast<PyAE::PyLayer>().GetHandle();
        })
        .def("__hash__", [](const PyAE::PyLayer& self) {
            return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(self.GetHandle()));
        })

        // Serialization methods
        .def("to_dict", [](PyAE::PyLayer& self) -> py::dict {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                return serialize.attr("layer_to_dict")(self).cast<py::dict>();
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to serialize layer: ") + e.what());
            }
        }, "Export layer to dictionary")

        .def("update_from_dict", [](PyAE::PyLayer& self, const py::dict& data) {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                serialize.attr("layer_update_from_dict")(self, data);
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to update layer from dict: ") + e.what());
            }
        }, "Update existing layer from dictionary",
           py::arg("data"));
}
