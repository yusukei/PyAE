// PyLayer_Mask.cpp
// PyAE - Python for After Effects
// PyLayer マスク操作実装

#include "PyLayerClasses.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// PyLayer マスク操作実装
//
// Note: PyMaskクラスはPyMask.cppで定義されており、
// ここでは前方宣言とpybind11のtype_casterを使用できないため、
// ae.Maskクラスを通じてPython側で操作する
// =============================================================

int PyLayer::GetNumMasks() const {
    if (!m_layerH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long numMasks;
    A_Err err = suites.maskSuite->AEGP_GetLayerNumMasks(m_layerH, &numMasks);
    if (err != A_Err_NONE) return 0;

    return static_cast<int>(numMasks);
}

py::list PyLayer::GetAllMasks() {
    py::list masks;

    if (!m_layerH) return masks;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    int numMasks = GetNumMasks();
    for (int i = 0; i < numMasks; ++i) {
        try {
            AEGP_MaskRefH maskH;
            A_Err err = suites.maskSuite->AEGP_GetLayerMaskByIndex(
                m_layerH, static_cast<A_long>(i), &maskH);
            if (err == A_Err_NONE && maskH) {
                py::module_ ae = py::module_::import("ae");
                py::object maskClass = ae.attr("Mask");
                // _create_from_handleを使用してマスクオブジェクトを作成
                py::object mask = maskClass.attr("_create_from_handle")(
                    reinterpret_cast<uintptr_t>(maskH),
                    reinterpret_cast<uintptr_t>(m_layerH));
                masks.append(mask);
            }
        } catch (const std::exception& e) {
            PYAE_LOG_WARNING("Layer", std::string("Failed to get mask at index ") + std::to_string(i) + ": " + e.what());
        }
    }

    return masks;
}

py::object PyLayer::GetMaskByIndex(int index) {
    if (!m_layerH) {
        return py::none();
    }

    int numMasks = GetNumMasks();
    if (index < 0 || index >= numMasks) {
        return py::none();
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    try {
        AEGP_MaskRefH maskH;
        A_Err err = suites.maskSuite->AEGP_GetLayerMaskByIndex(
            m_layerH, static_cast<A_long>(index), &maskH);
        if (err == A_Err_NONE && maskH) {
            py::module_ ae = py::module_::import("ae");
            py::object maskClass = ae.attr("Mask");
            return maskClass.attr("_create_from_handle")(
                reinterpret_cast<uintptr_t>(maskH),
                reinterpret_cast<uintptr_t>(m_layerH));
        }
    } catch (const std::exception& e) {
        PYAE_LOG_WARNING("Layer", std::string("Failed to get mask by index: ") + e.what());
    }

    return py::none();
}

py::object PyLayer::GetMaskByName(const std::string& name) {
    if (!m_layerH) {
        return py::none();
    }

    // 全マスクを取得して名前で検索
    py::list masks = GetAllMasks();
    for (auto mask : masks) {
        try {
            std::string maskName = mask.attr("name").cast<std::string>();
            if (maskName == name) {
                return py::reinterpret_borrow<py::object>(mask);
            }
        } catch (const std::exception&) {
            continue;
        }
    }

    return py::none();
}

py::object PyLayer::AddMask(const py::list& vertices) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // 新しいマスクを作成
    AEGP_MaskRefH maskH;
    A_long index;
    A_Err err = suites.maskSuite->AEGP_CreateNewMask(m_layerH, &maskH, &index);
    if (err != A_Err_NONE || !maskH) {
        throw std::runtime_error("Failed to create mask");
    }

    // 頂点データが指定されている場合は設定
    if (!vertices.empty()) {
        try {
            // マスクのOUTLINEストリームを取得
            AEGP_StreamRefH streamH = nullptr;
            err = suites.streamSuite->AEGP_GetNewMaskStream(
                state.GetPluginID(), maskH, AEGP_MaskStream_OUTLINE, &streamH);
            if (err != A_Err_NONE || !streamH) {
                throw std::runtime_error("Failed to get mask outline stream");
            }

            ScopedStreamRef scopedStream(suites.streamSuite, streamH);

            // StreamValueを取得してMaskOutlineValHを取得
            A_Time time = {0, 1};  // Initialize with safe default (time 0)
            AEGP_StreamValue2 streamValue = {};
            err = suites.streamSuite->AEGP_GetNewStreamValue(
                state.GetPluginID(), streamH, AEGP_LTimeMode_LayerTime,
                &time, FALSE, &streamValue);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to get stream value");
            }

            ScopedStreamValue scopedValue(suites.streamSuite, streamValue);

            if (!streamValue.val.mask) {
                throw std::runtime_error("Mask outline handle is null");
            }

            AEGP_MaskOutlineValH outlineH = streamValue.val.mask;

            // 既存の頂点をすべてクリア（デフォルトの四角形を削除）
            A_long numSegs = 0;
            err = suites.maskOutlineSuite->AEGP_GetMaskOutlineNumSegments(outlineH, &numSegs);
            if (err == A_Err_NONE) {
                for (A_long i = numSegs - 1; i >= 0; --i) {
                    suites.maskOutlineSuite->AEGP_DeleteVertex(outlineH, i);
                }
            }

            // 新しい頂点を追加
            for (size_t i = 0; i < vertices.size(); ++i) {
                py::dict vertexDict = vertices[i].cast<py::dict>();

                // 頂点を作成
                err = suites.maskOutlineSuite->AEGP_CreateVertex(outlineH, static_cast<A_long>(i));
                if (err != A_Err_NONE) {
                    throw std::runtime_error("Failed to create vertex");
                }

                // 頂点情報を設定
                AEGP_MaskVertex vertex = {};
                if (vertexDict.contains("x")) vertex.x = vertexDict["x"].cast<double>();
                if (vertexDict.contains("y")) vertex.y = vertexDict["y"].cast<double>();
                if (vertexDict.contains("tan_in_x")) vertex.tan_in_x = vertexDict["tan_in_x"].cast<double>();
                if (vertexDict.contains("tan_in_y")) vertex.tan_in_y = vertexDict["tan_in_y"].cast<double>();
                if (vertexDict.contains("tan_out_x")) vertex.tan_out_x = vertexDict["tan_out_x"].cast<double>();
                if (vertexDict.contains("tan_out_y")) vertex.tan_out_y = vertexDict["tan_out_y"].cast<double>();

                err = suites.maskOutlineSuite->AEGP_SetMaskOutlineVertexInfo(outlineH, static_cast<A_long>(i), &vertex);
                if (err != A_Err_NONE) {
                    throw std::runtime_error("Failed to set vertex info");
                }
            }

            // StreamValueを設定してマスクに反映
            err = suites.streamSuite->AEGP_SetStreamValue(
                state.GetPluginID(), streamH, &streamValue);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to set stream value");
            }

        } catch (const std::exception& e) {
            // Mask was created but vertex setup failed - log and re-throw
            PYAE_LOG_WARNING("Layer", std::string("Failed to set mask vertices: ") + e.what());
            throw;  // Re-throw to propagate error
        }
    }

    // Pythonオブジェクトとして返す
    py::module_ ae = py::module_::import("ae");
    py::object maskClass = ae.attr("Mask");
    return maskClass.attr("_create_from_handle")(
        reinterpret_cast<uintptr_t>(maskH),
        reinterpret_cast<uintptr_t>(m_layerH));
}

} // namespace PyAE
