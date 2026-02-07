// PyStreamUtils.h
// PyAE - Python for After Effects
// ストリーム値変換ユーティリティ
//
// PythonToStreamValue / StreamValueToPython の共通実装を提供

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <stdexcept>

#include "PluginState.h"

namespace py = pybind11;

namespace PyAE {
namespace StreamUtils {

// =============================================================
// 未知のストリームタイプの処理方針
// =============================================================
enum class UnknownTypePolicy {
    ThrowException,     // 例外を投げる（PyProperty用）
    TreatAsOneD         // one_dとして処理（PyEffect用）
};

// =============================================================
// StreamValueToPython
// AEGP_StreamValue2をPythonオブジェクトに変換
// =============================================================
inline py::object StreamValueToPython(
    const AEGP_StreamValue2& value,
    AEGP_StreamRefH streamH)
{
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamType type;
    suites.streamSuite->AEGP_GetStreamType(streamH, &type);

    switch (type) {
        case AEGP_StreamType_OneD:
            return py::float_(value.val.one_d);

        case AEGP_StreamType_TwoD:
        case AEGP_StreamType_TwoD_SPATIAL:
            return py::make_tuple(value.val.two_d.x, value.val.two_d.y);

        case AEGP_StreamType_ThreeD:
        case AEGP_StreamType_ThreeD_SPATIAL:
            return py::make_tuple(
                value.val.three_d.x,
                value.val.three_d.y,
                value.val.three_d.z
            );

        case AEGP_StreamType_COLOR:
            return py::make_tuple(
                value.val.color.redF,
                value.val.color.greenF,
                value.val.color.blueF,
                value.val.color.alphaF
            );

        case AEGP_StreamType_ARB: {
            if (!value.val.arbH) {
                return py::bytes("");
            }

            // ARB stream values use A_Handle (= PF_Handle), NOT AEGP_MemHandle.
            // PF_HandleSuite1 functions use C-style direct returns (not AEGP error-code pattern).
            PF_HandleSuite1* handleSuite = state.GetSuiteHandler().HandleSuite1();

            PF_Handle pfH = reinterpret_cast<PF_Handle>(value.val.arbH);
            void* ptr = handleSuite->host_lock_handle(pfH);
            if (!ptr) {
                return py::none();
            }

            A_HandleSize size = handleSuite->host_get_handle_size(pfH);

            py::object result = py::bytes(static_cast<const char*>(ptr), static_cast<size_t>(size));

            handleSuite->host_unlock_handle(pfH);
            return result;
        }

        default:
            return py::none();
    }
}

// =============================================================
// PythonToStreamValue
// PythonオブジェクトをAEGP_StreamValue2に変換
// =============================================================
inline AEGP_StreamValue2 PythonToStreamValue(
    py::object value,
    AEGP_StreamRefH streamH,
    UnknownTypePolicy unknownPolicy = UnknownTypePolicy::ThrowException,
    const char* arbMemName = "PyAE_ArbData")
{
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamType type;
    suites.streamSuite->AEGP_GetStreamType(streamH, &type);

    AEGP_StreamValue2 streamValue = {};

    switch (type) {
        case AEGP_StreamType_OneD:
            streamValue.val.one_d = value.cast<double>();
            break;

        case AEGP_StreamType_TwoD:
        case AEGP_StreamType_TwoD_SPATIAL: {
            auto seq = value.cast<py::sequence>();
            if (py::len(seq) < 2) {
                throw std::runtime_error("Expected sequence of length 2 for 2D property");
            }
            streamValue.val.two_d.x = seq[0].cast<double>();
            streamValue.val.two_d.y = seq[1].cast<double>();
            break;
        }

        case AEGP_StreamType_ThreeD:
        case AEGP_StreamType_ThreeD_SPATIAL: {
            auto seq = value.cast<py::sequence>();
            if (py::len(seq) < 3) {
                // 2要素の場合はZ=0として許容するか？
                // AEの Position (2D) が誤って3Dと判定された場合の対策として、
                // 2要素なら Z=0 で補完するのも親切。
                if (py::len(seq) == 2) {
                    streamValue.val.three_d.x = seq[0].cast<double>();
                    streamValue.val.three_d.y = seq[1].cast<double>();
                    streamValue.val.three_d.z = 0.0;
                } else {
                    throw std::runtime_error("Expected sequence of length 3 for 3D property");
                }
            } else {
                streamValue.val.three_d.x = seq[0].cast<double>();
                streamValue.val.three_d.y = seq[1].cast<double>();
                streamValue.val.three_d.z = seq[2].cast<double>();
            }
            break;
        }

        case AEGP_StreamType_COLOR: {
            auto seq = value.cast<py::sequence>();
            if (py::len(seq) < 3) {
                 throw std::runtime_error("Expected sequence of length 3 or 4 for Color property");
            }
            streamValue.val.color.redF = seq[0].cast<double>();
            streamValue.val.color.greenF = seq[1].cast<double>();
            streamValue.val.color.blueF = seq[2].cast<double>();
            if (py::len(seq) > 3) {
                streamValue.val.color.alphaF = seq[3].cast<double>();
            } else {
                streamValue.val.color.alphaF = 1.0;
            }
            break;
        }

        case AEGP_StreamType_ARB: {
            std::string data;
            if (py::isinstance<py::bytes>(value)) {
                data = value.cast<std::string>();
            } else {
                py::buffer buffer = value.cast<py::buffer>();
                py::buffer_info info = buffer.request();
                data.assign(static_cast<const char*>(info.ptr), info.size * info.itemsize);
            }

            // ARB stream values use A_Handle (= PF_Handle), NOT AEGP_MemHandle.
            // PF_HandleSuite1 functions use C-style direct returns.
            PF_HandleSuite1* handleSuite = state.GetSuiteHandler().HandleSuite1();

            PF_Handle pfH = handleSuite->host_new_handle(static_cast<A_HandleSize>(data.size()));
            if (!pfH) {
                throw std::runtime_error("Failed to allocate handle for arbitrary data");
            }

            void* ptr = handleSuite->host_lock_handle(pfH);
            if (ptr) {
                memcpy(ptr, data.data(), data.size());
                handleSuite->host_unlock_handle(pfH);
                streamValue.val.arbH = pfH;
            } else {
                handleSuite->host_dispose_handle(pfH);
                throw std::runtime_error("Failed to lock handle for arbitrary data");
            }
            break;
        }

        // PyEffect特有: 直接設定できないタイプ
        case AEGP_StreamType_MARKER:
        case AEGP_StreamType_LAYER_ID:
        case AEGP_StreamType_MASK_ID:
        case AEGP_StreamType_MASK:
        case AEGP_StreamType_TEXT_DOCUMENT:
            throw std::runtime_error("Cannot directly set this parameter type");

        default:
            if (unknownPolicy == UnknownTypePolicy::TreatAsOneD) {
                // Boolean や他の型はone_dとして扱う（PyEffect用）
                if (py::isinstance<py::bool_>(value)) {
                    streamValue.val.one_d = value.cast<bool>() ? 1.0 : 0.0;
                } else {
                    streamValue.val.one_d = value.cast<double>();
                }
            } else {
                // 例外を投げる（PyProperty用）
                throw std::runtime_error("Cannot set value for this property type");
            }
            break;
    }

    return streamValue;
}

} // namespace StreamUtils
} // namespace PyAE
