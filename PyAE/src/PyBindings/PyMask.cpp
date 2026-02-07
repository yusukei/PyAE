// PyMask.cpp
// PyAE - Python for After Effects
// マスク操作APIバインディング (Phase 5)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PluginState.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "ValidationUtils.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "Logger.h"
#include "PyMask.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// PyMask - 実装
// =============================================================

PyMask::PyMask() : m_maskH(nullptr), m_layerH(nullptr), m_ownsHandle(false) {}

PyMask::PyMask(AEGP_MaskRefH maskH, AEGP_LayerH layerH, bool ownsHandle)
    : m_maskH(maskH)
    , m_layerH(layerH)
    , m_ownsHandle(ownsHandle)
{}

PyMask::~PyMask() {
    if (m_ownsHandle && m_maskH) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.maskSuite) {
            suites.maskSuite->AEGP_DisposeMask(m_maskH);
        }
        m_maskH = nullptr;
    }
}

PyMask::PyMask(PyMask&& other) noexcept
    : m_maskH(other.m_maskH)
    , m_layerH(other.m_layerH)
    , m_ownsHandle(other.m_ownsHandle)
{
    other.m_maskH = nullptr;
    other.m_ownsHandle = false;
}

PyMask& PyMask::operator=(PyMask&& other) noexcept {
    if (this != &other) {
        if (m_ownsHandle && m_maskH) {
            auto& state = PluginState::Instance();
            const auto& suites = state.GetSuites();
            if (suites.maskSuite) {
                suites.maskSuite->AEGP_DisposeMask(m_maskH);
            }
        }
        m_maskH = other.m_maskH;
        m_layerH = other.m_layerH;
        m_ownsHandle = other.m_ownsHandle;
        other.m_maskH = nullptr;
        other.m_ownsHandle = false;
    }
    return *this;
}

bool PyMask::IsValid() const { return m_maskH != nullptr; }

std::string PyMask::GetName() const {
    if (!m_maskH) return "";
    
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    
    // MaskSuite6 では GetMaskName が obsolete
    // ストリーム経由で名前を取得
    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewMaskStream(
        state.GetPluginID(), m_maskH, AEGP_MaskStream_OUTLINE, &streamH);
    
    if (err != A_Err_NONE || !streamH) {
        return "";
    }
    
    ScopedStreamRef scopedStream(suites.streamSuite, streamH);
    
    AEGP_MemHandle nameH = nullptr;
    err = suites.streamSuite->AEGP_GetStreamName(
        state.GetPluginID(), streamH, FALSE, &nameH);
    
    if (err != A_Err_NONE || !nameH) {
        return "";
    }

    ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);

    // ScopedMemLockを使用して例外安全にロック/アンロック
    ScopedMemLock lock(suites.memorySuite, nameH);
    A_char* namePtr = lock.As<A_char>();
    if (!namePtr) {
        return "";
    }

    std::string result;
    // Windows環境でのエンコーディング変換
    // AE SDKの文字列はマルチバイト（CP_ACP）の可能性がある
#ifdef _WIN32
    try {
        // マルチバイト → ワイド文字 → UTF-8
        int wideSize = MultiByteToWideChar(CP_ACP, 0, namePtr, -1, nullptr, 0);
        if (wideSize > 0) {
            std::wstring wideStr(wideSize, 0);
            MultiByteToWideChar(CP_ACP, 0, namePtr, -1, &wideStr[0], wideSize);

            int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (utf8Size > 0) {
                result.resize(utf8Size - 1); // null終端を除く
                WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &result[0], utf8Size, nullptr, nullptr);
            }
        }
    } catch (...) {
        // 変換失敗時は空文字列
        result = "";
    }
#else
    // 非Windows環境ではそのまま使用
    result = std::string(namePtr);
#endif
    // lockのデストラクタで自動的にUnlock
    // scopedNameのデストラクタで自動的にFreeMemHandle
    
    return result;
}

int PyMask::GetIndex() const {
    if (!m_maskH || !m_layerH) return -1;
    
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    
    // レイヤーの全マスクを取得して、このマスクのインデックスを見つける
    AEGP_MaskIDVal targetID;
    A_Err err = suites.maskSuite->AEGP_GetMaskID(m_maskH, &targetID);
    if (err != A_Err_NONE) return -1;
    
    A_long numMasks = 0;
    err = suites.maskSuite->AEGP_GetLayerNumMasks(m_layerH, &numMasks);
    if (err != A_Err_NONE) return -1;
    
    for (A_long i = 0; i < numMasks; ++i) {
        AEGP_MaskRefH maskH = nullptr;
        err = suites.maskSuite->AEGP_GetLayerMaskByIndex(m_layerH, i, &maskH);
        if (err == A_Err_NONE && maskH) {
            AEGP_MaskIDVal currentID;
            err = suites.maskSuite->AEGP_GetMaskID(maskH, &currentID);
            suites.maskSuite->AEGP_DisposeMask(maskH);
            
            if (err == A_Err_NONE && currentID == targetID) {
                // 0-based インデックスを返す（Python標準形式）
                return static_cast<int>(i);
            }
        }
    }
    
    return -1;
}

A_long PyMask::GetMaskID() const {
    if (!m_maskH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_MaskIDVal idVal;
    A_Err err = suites.maskSuite->AEGP_GetMaskID(m_maskH, &idVal);
    if (err != A_Err_NONE) return 0;

    return static_cast<A_long>(idVal);
}

MaskMode PyMask::GetMode() const {
    if (!m_maskH) return MaskMode::None;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    PF_MaskMode mode;
    A_Err err = suites.maskSuite->AEGP_GetMaskMode(m_maskH, &mode);
    if (err != A_Err_NONE) return MaskMode::None;

    switch (mode) {
        case PF_MaskMode_NONE:       return MaskMode::None;
        case PF_MaskMode_ADD:        return MaskMode::Add;
        case PF_MaskMode_SUBTRACT:   return MaskMode::Subtract;
        case PF_MaskMode_INTERSECT:  return MaskMode::Intersect;
        case PF_MaskMode_LIGHTEN:    return MaskMode::Lighten;
        case PF_MaskMode_DARKEN:     return MaskMode::Darken;
        case PF_MaskMode_DIFFERENCE: return MaskMode::Difference;
        default:                     return MaskMode::None;
    }
}

void PyMask::SetMode(MaskMode mode) {
    if (!m_maskH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    PF_MaskMode aeMode;
    switch (mode) {
        case MaskMode::None:       aeMode = PF_MaskMode_NONE; break;
        case MaskMode::Add:        aeMode = PF_MaskMode_ADD; break;
        case MaskMode::Subtract:   aeMode = PF_MaskMode_SUBTRACT; break;
        case MaskMode::Intersect:  aeMode = PF_MaskMode_INTERSECT; break;
        case MaskMode::Lighten:    aeMode = PF_MaskMode_LIGHTEN; break;
        case MaskMode::Darken:     aeMode = PF_MaskMode_DARKEN; break;
        case MaskMode::Difference: aeMode = PF_MaskMode_DIFFERENCE; break;
        default:                   aeMode = PF_MaskMode_ADD; break;
    }

    A_Err err = suites.maskSuite->AEGP_SetMaskMode(m_maskH, aeMode);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set mask mode");
    }
}

bool PyMask::GetInverted() const {
    if (!m_maskH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean inverted;
    A_Err err = suites.maskSuite->AEGP_GetMaskInvert(m_maskH, &inverted);
    if (err != A_Err_NONE) return false;

    return inverted != FALSE;
}

void PyMask::SetInverted(bool inverted) {
    if (!m_maskH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.maskSuite->AEGP_SetMaskInvert(m_maskH, inverted ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set mask inverted state");
    }
}

bool PyMask::GetLocked() const {
    if (!m_maskH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean locked;
    A_Err err = suites.maskSuite->AEGP_GetMaskLockState(m_maskH, &locked);
    if (err != A_Err_NONE) return false;

    return locked != FALSE;
}

void PyMask::SetLocked(bool locked) {
    if (!m_maskH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.maskSuite->AEGP_SetMaskLockState(m_maskH, locked ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set mask locked state");
    }
}

bool PyMask::GetIsRotoBezier() const {
    if (!m_maskH) return false;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean is_roto_bezier;
    A_Err err = suites.maskSuite->AEGP_GetMaskIsRotoBezier(m_maskH, &is_roto_bezier);
    if (err != A_Err_NONE) return false;

    return is_roto_bezier != FALSE;
}

void PyMask::SetIsRotoBezier(bool is_roto_bezier) {
    if (!m_maskH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.maskSuite->AEGP_SetMaskIsRotoBezier(m_maskH, is_roto_bezier ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set mask RotoBezier state");
    }
}

double PyMask::GetOpacity() const {
    return GetMaskStreamValue(AEGP_MaskStream_OPACITY);
}

void PyMask::SetOpacity(double value) {
    // Validate opacity range (0-100)
    Validation::RequireValidOpacity(value);

    SetMaskStreamValue(AEGP_MaskStream_OPACITY, value);
}

double PyMask::GetExpansion() const {
    return GetMaskStreamValue(AEGP_MaskStream_EXPANSION);
}

void PyMask::SetExpansion(double value) {
    SetMaskStreamValue(AEGP_MaskStream_EXPANSION, value);
}

py::tuple PyMask::GetFeather() const {
    if (!m_maskH) return py::make_tuple(0.0, 0.0);

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewMaskStream(
        state.GetPluginID(), m_maskH, AEGP_MaskStream_FEATHER, &streamH);
    if (err != A_Err_NONE || !streamH) {
        return py::make_tuple(0.0, 0.0);
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) {
        return py::make_tuple(0.0, 0.0);
    }

    py::tuple result = py::make_tuple(value.val.two_d.x, value.val.two_d.y);
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void PyMask::SetFeather(double x, double y) {
    if (!m_maskH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewMaskStream(
        state.GetPluginID(), m_maskH, AEGP_MaskStream_FEATHER, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get feather stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.two_d.x = x;
    streamValue.val.two_d.y = y;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set feather value");
    }
}

py::tuple PyMask::GetColor() const {
    if (!m_maskH) return py::make_tuple(0.0, 0.0, 0.0);

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_ColorVal color;
    A_Err err = suites.maskSuite->AEGP_GetMaskColor(m_maskH, &color);
    if (err != A_Err_NONE) return py::make_tuple(0.0, 0.0, 0.0);

    return py::make_tuple(color.redF, color.greenF, color.blueF);
}

void PyMask::SetColor(double red, double green, double blue) {
    if (!m_maskH) {
        throw std::runtime_error("Invalid mask");
    }

    // Validate color ranges
    Validation::RequireColorRange(red, "Red");
    Validation::RequireColorRange(green, "Green");
    Validation::RequireColorRange(blue, "Blue");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_ColorVal color;
    color.alphaF = 1.0;
    color.redF = red;
    color.greenF = green;
    color.blueF = blue;

    A_Err err = suites.maskSuite->AEGP_SetMaskColor(m_maskH, &color);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set mask color");
    }
}

void PyMask::Delete() {
    if (!m_maskH || !m_layerH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.maskSuite->AEGP_DeleteMaskFromLayer(m_maskH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to delete mask");
    }

    m_maskH = nullptr;
}

std::shared_ptr<PyMask> PyMask::Duplicate() const {
    if (!m_maskH || !m_layerH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_MaskRefH duplicate_maskH = nullptr;
    A_Err err = suites.maskSuite->AEGP_DuplicateMask(m_maskH, &duplicate_maskH);
    if (err != A_Err_NONE || !duplicate_maskH) {
        throw std::runtime_error("Failed to duplicate mask");
    }

    return std::make_shared<PyMask>(duplicate_maskH, m_layerH, true);
}

AEGP_MaskRefH PyMask::GetHandle() const { return m_maskH; }

uintptr_t PyMask::GetOutline() const {
    if (!m_maskH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get mask path stream
    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewMaskStream(
        state.GetPluginID(), m_maskH, AEGP_MaskStream_OUTLINE, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get mask stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    // Get outline value from stream
    A_Time time = {0, 1};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get stream value");
    }

    if (!value.val.mask) {
        throw std::runtime_error("Mask outline is null");
    }

    return reinterpret_cast<uintptr_t>(value.val.mask);
}

double PyMask::GetMaskStreamValue(AEGP_MaskStream streamType) const {
    if (!m_maskH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewMaskStream(
        state.GetPluginID(), m_maskH, streamType, &streamH);
    if (err != A_Err_NONE || !streamH) {
        return 0.0;
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) {
        return 0.0;
    }

    double result = value.val.one_d;
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void PyMask::SetMaskStreamValue(AEGP_MaskStream streamType, double value) {
    if (!m_maskH) {
        throw std::runtime_error("Invalid mask");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewMaskStream(
        state.GetPluginID(), m_maskH, streamType, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.one_d = value;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set stream value");
    }
}

// =============================================================
// レイヤーのマスク操作ヘルパー関数
// =============================================================

// レイヤーからマスク数を取得
int GetLayerNumMasks(AEGP_LayerH layerH) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long numMasks;
    A_Err err = suites.maskSuite->AEGP_GetLayerNumMasks(layerH, &numMasks);
    if (err != A_Err_NONE) return 0;

    return static_cast<int>(numMasks);
}

// レイヤーからマスクを取得（インデックス指定）
std::shared_ptr<PyMask> GetLayerMaskByIndex(AEGP_LayerH layerH, int index) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_MaskRefH maskH;
    A_Err err = suites.maskSuite->AEGP_GetLayerMaskByIndex(
        layerH, static_cast<A_long>(index), &maskH);
    if (err != A_Err_NONE || !maskH) {
        throw std::runtime_error("Failed to get mask at index");
    }

    return std::make_shared<PyMask>(maskH, layerH, true);
}

// レイヤーにマスクを追加
std::shared_ptr<PyMask> AddMaskToLayer(AEGP_LayerH layerH) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_MaskRefH maskH;
    A_long index;
    A_Err err = suites.maskSuite->AEGP_CreateNewMask(layerH, &maskH, &index);
    if (err != A_Err_NONE || !maskH) {
        throw std::runtime_error("Failed to create mask");
    }

    return std::make_shared<PyMask>(maskH, layerH, true);
}

} // namespace PyAE

void init_mask(py::module_& m) {
    // マスクモード列挙
    py::enum_<PyAE::MaskMode>(m, "MaskMode")
        .value("None_", PyAE::MaskMode::None)  // Python予約語回避
        .value("Add", PyAE::MaskMode::Add)
        .value("Subtract", PyAE::MaskMode::Subtract)
        .value("Intersect", PyAE::MaskMode::Intersect)
        .value("Lighten", PyAE::MaskMode::Lighten)
        .value("Darken", PyAE::MaskMode::Darken)
        .value("Difference", PyAE::MaskMode::Difference);

    // マスク頂点構造体
    py::class_<PyAE::MaskVertex>(m, "MaskVertex")
        .def(py::init<>())
        .def_readwrite("x", &PyAE::MaskVertex::x)
        .def_readwrite("y", &PyAE::MaskVertex::y)
        .def_readwrite("in_tangent_x", &PyAE::MaskVertex::inTangentX)
        .def_readwrite("in_tangent_y", &PyAE::MaskVertex::inTangentY)
        .def_readwrite("out_tangent_x", &PyAE::MaskVertex::outTangentX)
        .def_readwrite("out_tangent_y", &PyAE::MaskVertex::outTangentY);

    // マスククラス
    py::class_<PyAE::PyMask, std::shared_ptr<PyAE::PyMask>>(m, "Mask")
        .def_property_readonly("valid", &PyAE::PyMask::IsValid,
                              "Check if mask is valid")
        .def_property_readonly("name", &PyAE::PyMask::GetName,
                              "Mask name")
        .def_property_readonly("index", &PyAE::PyMask::GetIndex,
                              "Mask index in layer (0-based)")
        .def_property_readonly("mask_id", &PyAE::PyMask::GetMaskID,
                              "Mask ID (unique identifier)")
        .def_property("mode", &PyAE::PyMask::GetMode, &PyAE::PyMask::SetMode,
                     "Mask mode (Add, Subtract, etc.)")
        .def_property("inverted", &PyAE::PyMask::GetInverted, &PyAE::PyMask::SetInverted,
                     "Mask inverted state")
        .def_property("locked", &PyAE::PyMask::GetLocked, &PyAE::PyMask::SetLocked,
                     "Mask locked state")
        .def_property("is_roto_bezier", &PyAE::PyMask::GetIsRotoBezier, &PyAE::PyMask::SetIsRotoBezier,
                     "RotoBezier state")
        .def_property("opacity", &PyAE::PyMask::GetOpacity, &PyAE::PyMask::SetOpacity,
                     "Mask opacity (0-100)")
        .def_property("expansion", &PyAE::PyMask::GetExpansion, &PyAE::PyMask::SetExpansion,
                     "Mask expansion in pixels")
        .def_property("feather",
            &PyAE::PyMask::GetFeather,
            [](PyAE::PyMask& self, py::object feather) {
                py::sequence seq = feather.cast<py::sequence>();
                if (py::len(seq) < 2) throw std::invalid_argument("Feather must have at least 2 elements (x, y)");
                self.SetFeather(seq[0].cast<double>(), seq[1].cast<double>());
            },
            "Mask feather as (x, y) tuple")
        .def_property("color",
            &PyAE::PyMask::GetColor,
            [](PyAE::PyMask& self, py::object color) {
                py::sequence seq = color.cast<py::sequence>();
                if (py::len(seq) < 3) throw std::invalid_argument("Color must have at least 3 elements (r, g, b)");
                self.SetColor(seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>());
            },
            "Mask color as (red, green, blue) tuple (0.0-1.0 range)")
        .def_property_readonly("outline", &PyAE::PyMask::GetOutline,
                              "Mask outline handle (for MaskOutlineSuite3 functions)")
        // Python protocol methods
        .def("__repr__", [](const PyAE::PyMask& self) {
            if (!self.IsValid()) return std::string("<Mask: invalid>");
            std::string name = self.GetName();
            int idx = self.GetIndex();
            return std::string("<Mask: '") + name + "' index=" + std::to_string(idx) + ">";
        })
        .def("__bool__", &PyAE::PyMask::IsValid)
        .def("delete", &PyAE::PyMask::Delete,
             "Delete the mask from the layer")
        .def("duplicate", &PyAE::PyMask::Duplicate,
             "Duplicate the mask")
        // 内部使用: マスクハンドルを取得
        .def_property_readonly("_handle", [](const PyAE::PyMask& self) {
            return reinterpret_cast<uintptr_t>(self.GetHandle());
        }, "Internal: get mask handle (for SDK direct access)")
        // 内部使用: ハンドルからオブジェクト作成
        .def_static("_create_from_handle", [](uintptr_t maskHandle, uintptr_t layerHandle) {
            AEGP_MaskRefH maskH = reinterpret_cast<AEGP_MaskRefH>(maskHandle);
            AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
            return std::make_shared<PyAE::PyMask>(maskH, layerH, true);
        }, "Internal: create Mask from handle",
           py::arg("mask_handle"), py::arg("layer_handle"));
}
