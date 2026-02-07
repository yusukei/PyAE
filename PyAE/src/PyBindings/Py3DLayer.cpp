// Py3DLayer.cpp
// PyAE - Python for After Effects
// 3Dレイヤー、カメラ、ライト操作APIバインディング (Phase 5)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstdint>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PluginState.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "Logger.h"

namespace py = pybind11;

namespace PyAE {

// ライトタイプ列挙
enum class LightType {
    Parallel = 0,
    Spot,
    Point,
    Ambient
};

// =============================================================
// 3Dレイヤー操作ヘルパー関数
// =============================================================

// レイヤーを3Dに設定
void SetLayer3D(uintptr_t layerHandle, bool is3D) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Err err = suites.layerSuite->AEGP_SetLayerFlag(
        layerH, AEGP_LayerFlag_LAYER_IS_3D, is3D ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set 3D layer flag");
    }
}

bool IsLayer3D(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Boolean is3D;
    A_Err err = suites.layerSuite->AEGP_IsLayer3D(layerH, &is3D);
    if (err != A_Err_NONE) return false;

    return is3D != FALSE;
}

// 3D位置を取得
py::tuple Get3DPosition(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_POSITION, &streamH);
    if (err != A_Err_NONE || !streamH) {
        return py::make_tuple(0.0, 0.0, 0.0);
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) {
        return py::make_tuple(0.0, 0.0, 0.0);
    }

    py::tuple result = py::make_tuple(value.val.three_d.x, value.val.three_d.y, value.val.three_d.z);
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

// 3D位置を設定
void Set3DPosition(uintptr_t layerHandle, double x, double y, double z) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_POSITION, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get position stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.three_d.x = x;
    streamValue.val.three_d.y = y;
    streamValue.val.three_d.z = z;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set position");
    }
}

// 3D回転を取得（X, Y, Z）
py::tuple Get3DRotation(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    auto getStreamValue = [&](AEGP_LayerStream streamType) -> double {
        AEGP_StreamRefH streamH = nullptr;
        A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
            state.GetPluginID(), layerH, streamType, &streamH);
        if (err != A_Err_NONE || !streamH) return 0.0;

        ScopedStreamRef scopedStream(suites.streamSuite, streamH);

        A_Time time = {0, 1000};
        AEGP_StreamValue2 value;
        err = suites.streamSuite->AEGP_GetNewStreamValue(
            state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
        if (err != A_Err_NONE) return 0.0;

        double result = value.val.one_d;
        suites.streamSuite->AEGP_DisposeStreamValue(&value);
        return result;
    };

    double rotX = getStreamValue(AEGP_LayerStream_ROTATE_X);
    double rotY = getStreamValue(AEGP_LayerStream_ROTATE_Y);
    double rotZ = getStreamValue(AEGP_LayerStream_ROTATE_Z);

    return py::make_tuple(rotX, rotY, rotZ);
}

// 3D回転を設定
void Set3DRotation(uintptr_t layerHandle, double rotX, double rotY, double rotZ) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    auto setStreamValue = [&](AEGP_LayerStream streamType, double value) {
        AEGP_StreamRefH streamH = nullptr;
        A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
            state.GetPluginID(), layerH, streamType, &streamH);
        if (err != A_Err_NONE || !streamH) return;

        ScopedStreamRef scopedStream(suites.streamSuite, streamH);

        AEGP_StreamValue2 streamValue = {};
        streamValue.val.one_d = value;

        suites.streamSuite->AEGP_SetStreamValue(
            state.GetPluginID(), streamH, &streamValue);
    };

    setStreamValue(AEGP_LayerStream_ROTATE_X, rotX);
    setStreamValue(AEGP_LayerStream_ROTATE_Y, rotY);
    setStreamValue(AEGP_LayerStream_ROTATE_Z, rotZ);
}

// 3Dスケールを取得
py::tuple Get3DScale(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_SCALE, &streamH);
    if (err != A_Err_NONE || !streamH) {
        return py::make_tuple(100.0, 100.0, 100.0);
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) {
        return py::make_tuple(100.0, 100.0, 100.0);
    }

    py::tuple result = py::make_tuple(value.val.three_d.x, value.val.three_d.y, value.val.three_d.z);
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

// 3Dスケールを設定
void Set3DScale(uintptr_t layerHandle, double scaleX, double scaleY, double scaleZ) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_SCALE, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get scale stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.three_d.x = scaleX;
    streamValue.val.three_d.y = scaleY;
    streamValue.val.three_d.z = scaleZ;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set scale");
    }
}

// オリエンテーションを取得
py::tuple GetOrientation(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_ORIENTATION, &streamH);
    if (err != A_Err_NONE || !streamH) {
        return py::make_tuple(0.0, 0.0, 0.0);
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) {
        return py::make_tuple(0.0, 0.0, 0.0);
    }

    py::tuple result = py::make_tuple(value.val.three_d.x, value.val.three_d.y, value.val.three_d.z);
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

// オリエンテーションを設定
void SetOrientation(uintptr_t layerHandle, double x, double y, double z) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_ORIENTATION, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get orientation stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.three_d.x = x;
    streamValue.val.three_d.y = y;
    streamValue.val.three_d.z = z;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set orientation");
    }
}

// =============================================================
// カメラ操作ヘルパー関数
// =============================================================

bool IsCamera(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_ObjectType objType;
    A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
    if (err != A_Err_NONE) return false;

    return objType == AEGP_ObjectType_CAMERA;
}

// カメラズームを取得
double GetCameraZoom(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_ZOOM, &streamH);
    if (err != A_Err_NONE || !streamH) return 0.0;

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) return 0.0;

    double result = value.val.one_d;
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void SetCameraZoom(uintptr_t layerHandle, double zoom) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_ZOOM, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get zoom stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.one_d = zoom;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set zoom");
    }
}

// カメラ注視点を取得
// Note: AEGP SDKにはPOI専用のストリームがないため、
// 二ノードカメラの場合はエクスプレッションまたはスクリプトで取得
py::tuple GetCameraPointOfInterest(uintptr_t /*layerHandle*/) {
    // POIストリームは存在しないため、デフォルト値を返す
    // 二ノードカメラのPOIはカメラの子レイヤーとして管理される
    PYAE_LOG_WARNING("3DLayer", "Camera POI stream is not available via AEGP SDK");
    return py::make_tuple(0.0, 0.0, 0.0);
}

void SetCameraPointOfInterest(uintptr_t /*layerHandle*/, double /*x*/, double /*y*/, double /*z*/) {
    // POIストリームは存在しないため、操作不可
    PYAE_LOG_WARNING("3DLayer", "Camera POI stream is not available via AEGP SDK");
    throw std::runtime_error("Camera POI is not directly accessible via AEGP SDK. Use expressions or ExtendScript.");
}

// =============================================================
// ライト操作ヘルパー関数
// =============================================================

bool IsLight(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_ObjectType objType;
    A_Err err = suites.layerSuite->AEGP_GetLayerObjectType(layerH, &objType);
    if (err != A_Err_NONE) return false;

    return objType == AEGP_ObjectType_LIGHT;
}

LightType GetLightType(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.lightSuite) {
        PYAE_LOG_WARNING("3DLayer", "LightSuite not available");
        return LightType::Point;
    }

    AEGP_LightType type;
    A_Err err = suites.lightSuite->AEGP_GetLightType(layerH, &type);
    if (err != A_Err_NONE) return LightType::Point;

    switch (type) {
        case AEGP_LightType_PARALLEL: return LightType::Parallel;
        case AEGP_LightType_SPOT:     return LightType::Spot;
        case AEGP_LightType_POINT:    return LightType::Point;
        case AEGP_LightType_AMBIENT:  return LightType::Ambient;
        default:                      return LightType::Point;
    }
}

void SetLightType(uintptr_t /*layerHandle*/, LightType /*type*/) {
    // Note: AEGP doesn't provide a way to change light type directly
    // This would require creating a new light layer
    PYAE_LOG_WARNING("3DLayer", "SetLightType: Light type cannot be changed after creation");
}

// ライト強度を取得
double GetLightIntensity(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_INTENSITY, &streamH);
    if (err != A_Err_NONE || !streamH) return 100.0;

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) return 100.0;

    double result = value.val.one_d;
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void SetLightIntensity(uintptr_t layerHandle, double intensity) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_INTENSITY, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get intensity stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.one_d = intensity;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set intensity");
    }
}

// ライト色を取得
py::tuple GetLightColor(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_COLOR, &streamH);
    if (err != A_Err_NONE || !streamH) {
        return py::make_tuple(1.0, 1.0, 1.0);
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) {
        return py::make_tuple(1.0, 1.0, 1.0);
    }

    py::tuple result = py::make_tuple(
        value.val.color.redF,
        value.val.color.greenF,
        value.val.color.blueF
    );
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void SetLightColor(uintptr_t layerHandle, double r, double g, double b) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_COLOR, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get color stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.color.redF = static_cast<A_FpShort>(r);
    streamValue.val.color.greenF = static_cast<A_FpShort>(g);
    streamValue.val.color.blueF = static_cast<A_FpShort>(b);
    streamValue.val.color.alphaF = 1.0;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set color");
    }
}

// スポットライトのコーン角度を取得
double GetLightConeAngle(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_CONE_ANGLE, &streamH);
    if (err != A_Err_NONE || !streamH) return 90.0;

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) return 90.0;

    double result = value.val.one_d;
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void SetLightConeAngle(uintptr_t layerHandle, double angle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_CONE_ANGLE, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get cone angle stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.one_d = angle;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set cone angle");
    }
}

// コーンフェザーを取得
double GetLightConeFeather(uintptr_t layerHandle) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_CONE_FEATHER, &streamH);
    if (err != A_Err_NONE || !streamH) return 50.0;

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    A_Time time = {0, 1000};
    AEGP_StreamValue2 value;
    err = suites.streamSuite->AEGP_GetNewStreamValue(
        state.GetPluginID(), streamH, AEGP_LTimeMode_CompTime, &time, FALSE, &value);
    if (err != A_Err_NONE) return 50.0;

    double result = value.val.one_d;
    suites.streamSuite->AEGP_DisposeStreamValue(&value);
    return result;
}

void SetLightConeFeather(uintptr_t layerHandle, double feather) {
    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerHandle);
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_StreamRefH streamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), layerH, AEGP_LayerStream_CONE_FEATHER, &streamH);
    if (err != A_Err_NONE || !streamH) {
        throw std::runtime_error("Failed to get cone feather stream");
    }

    ScopedStreamRef scopedStream(suites.streamSuite, streamH);

    AEGP_StreamValue2 streamValue = {};
    streamValue.val.one_d = feather;

    err = suites.streamSuite->AEGP_SetStreamValue(
        state.GetPluginID(), streamH, &streamValue);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set cone feather");
    }
}

} // namespace PyAE

void init_3d_layer(py::module_& m) {
    // ライトタイプ列挙
    py::enum_<PyAE::LightType>(m, "LightType")
        .value("Parallel", PyAE::LightType::Parallel)
        .value("Spot", PyAE::LightType::Spot)
        .value("Point", PyAE::LightType::Point)
        .value("Ambient", PyAE::LightType::Ambient);

    // 3Dサブモジュール
    py::module_ three_d = m.def_submodule("three_d", "3D layer, camera, and light operations");

    // 3Dレイヤー操作
    three_d.def("is_3d", &PyAE::IsLayer3D,
                "Check if layer is 3D",
                py::arg("layer_handle"));

    three_d.def("set_3d", &PyAE::SetLayer3D,
                "Set layer 3D state",
                py::arg("layer_handle"), py::arg("is_3d"));

    three_d.def("get_position", &PyAE::Get3DPosition,
                "Get 3D position (x, y, z)",
                py::arg("layer_handle"));

    three_d.def("set_position", [](uintptr_t layerHandle, py::object pos) {
                py::sequence seq = pos.cast<py::sequence>();
                if (py::len(seq) < 3) throw std::invalid_argument("Position must have at least 3 elements (x, y, z)");
                PyAE::Set3DPosition(layerHandle, seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>());
                }, "Set 3D position",
                py::arg("layer_handle"), py::arg("position"));

    three_d.def("get_rotation", &PyAE::Get3DRotation,
                "Get 3D rotation (x, y, z)",
                py::arg("layer_handle"));

    three_d.def("set_rotation", [](uintptr_t layerHandle, py::object rot) {
                py::sequence seq = rot.cast<py::sequence>();
                if (py::len(seq) < 3) throw std::invalid_argument("Rotation must have at least 3 elements (x, y, z)");
                PyAE::Set3DRotation(layerHandle, seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>());
                }, "Set 3D rotation",
                py::arg("layer_handle"), py::arg("rotation"));

    three_d.def("get_scale", &PyAE::Get3DScale,
                "Get 3D scale (x, y, z)",
                py::arg("layer_handle"));

    three_d.def("set_scale", [](uintptr_t layerHandle, py::object scale) {
                py::sequence seq = scale.cast<py::sequence>();
                if (py::len(seq) < 3) throw std::invalid_argument("Scale must have at least 3 elements (x, y, z)");
                PyAE::Set3DScale(layerHandle, seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>());
                }, "Set 3D scale",
                py::arg("layer_handle"), py::arg("scale"));

    three_d.def("get_orientation", &PyAE::GetOrientation,
                "Get orientation (x, y, z)",
                py::arg("layer_handle"));

    three_d.def("set_orientation", [](uintptr_t layerHandle, py::object orient) {
                py::sequence seq = orient.cast<py::sequence>();
                if (py::len(seq) < 3) throw std::invalid_argument("Orientation must have at least 3 elements (x, y, z)");
                PyAE::SetOrientation(layerHandle, seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>());
                }, "Set orientation",
                py::arg("layer_handle"), py::arg("orientation"));

    // カメラ操作
    three_d.def("is_camera", &PyAE::IsCamera,
                "Check if layer is a camera",
                py::arg("layer_handle"));

    three_d.def("get_camera_zoom", &PyAE::GetCameraZoom,
                "Get camera zoom",
                py::arg("layer_handle"));

    three_d.def("set_camera_zoom", &PyAE::SetCameraZoom,
                "Set camera zoom",
                py::arg("layer_handle"), py::arg("zoom"));

    three_d.def("get_camera_poi", &PyAE::GetCameraPointOfInterest,
                "Get camera point of interest (x, y, z) - Note: Not available via AEGP SDK",
                py::arg("layer_handle"));

    three_d.def("set_camera_poi", [](uintptr_t layerHandle, py::object poi) {
                py::sequence seq = poi.cast<py::sequence>();
                if (py::len(seq) < 3) throw std::invalid_argument("POI must have at least 3 elements (x, y, z)");
                PyAE::SetCameraPointOfInterest(layerHandle, seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>());
                }, "Set camera point of interest - Note: Not available via AEGP SDK",
                py::arg("layer_handle"), py::arg("poi"));

    // ライト操作
    three_d.def("is_light", &PyAE::IsLight,
                "Check if layer is a light",
                py::arg("layer_handle"));

    three_d.def("get_light_type", &PyAE::GetLightType,
                "Get light type",
                py::arg("layer_handle"));

    three_d.def("get_light_intensity", &PyAE::GetLightIntensity,
                "Get light intensity",
                py::arg("layer_handle"));

    three_d.def("set_light_intensity", &PyAE::SetLightIntensity,
                "Set light intensity",
                py::arg("layer_handle"), py::arg("intensity"));

    three_d.def("get_light_color", &PyAE::GetLightColor,
                "Get light color (r, g, b)",
                py::arg("layer_handle"));

    three_d.def("set_light_color", [](uintptr_t layerHandle, py::object color) {
                py::sequence seq = color.cast<py::sequence>();
                if (py::len(seq) < 3) throw std::invalid_argument("Color must have at least 3 elements (r, g, b)");
                PyAE::SetLightColor(layerHandle, seq[0].cast<double>(), seq[1].cast<double>(), seq[2].cast<double>());
                }, "Set light color",
                py::arg("layer_handle"), py::arg("color"));

    three_d.def("get_light_cone_angle", &PyAE::GetLightConeAngle,
                "Get spotlight cone angle",
                py::arg("layer_handle"));

    three_d.def("set_light_cone_angle", &PyAE::SetLightConeAngle,
                "Set spotlight cone angle",
                py::arg("layer_handle"), py::arg("angle"));

    three_d.def("get_light_cone_feather", &PyAE::GetLightConeFeather,
                "Get spotlight cone feather",
                py::arg("layer_handle"));

    three_d.def("set_light_cone_feather", &PyAE::SetLightConeFeather,
                "Set spotlight cone feather",
                py::arg("layer_handle"), py::arg("feather"));
}
