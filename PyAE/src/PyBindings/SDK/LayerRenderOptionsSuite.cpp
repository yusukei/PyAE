#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_LayerRenderOptionsSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_LayerRenderOptionsSuite2
    // -----------------------------------------------------------------------
    //
    // This suite provides layer-specific render options for rendering layers.
    // Unlike RenderOptionsSuite which is for composition-level rendering,
    // LayerRenderOptionsSuite focuses on individual layer render control.
    //
    // IMPORTANT: Memory management
    // ---------------------------
    // - Handles returned by AEGP_NewFromLayer, AEGP_NewFromUpstreamOfEffect,
    //   AEGP_NewFromDownstreamOfEffect, and AEGP_Duplicate MUST be disposed
    //   by the calling code using AEGP_Dispose.
    //
    // IMPORTANT: WorldType values
    // ---------------------------
    // - AEGP_WorldType_NONE = 0
    // - AEGP_WorldType_8    = 1 (8-bit)
    // - AEGP_WorldType_16   = 2 (16-bit)
    // - AEGP_WorldType_32   = 3 (32-bit float)
    //
    // IMPORTANT: MatteMode values
    // ---------------------------
    // - AEGP_MatteMode_STRAIGHT        = 0
    // - AEGP_MatteMode_PREMUL_BLACK    = 1
    // - AEGP_MatteMode_PREMUL_BG_COLOR = 2
    //
    // IMPORTANT: Downsample factor
    // ---------------------------
    // - 1 = 100% (full resolution)
    // - 2 = 50%
    // - 3 = 33.3%
    // - 4 = 25%
    // - etc.
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Create New Options
    // -----------------------------------------------------------------------

    sdk.def("AEGP_NewFromLayer", [](uintptr_t layerH_ptr) -> uintptr_t {
        Validation::RequireNonNull(layerH_ptr, "layerH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
        AEGP_LayerRenderOptionsH optionsH = nullptr;

        A_Err err = suites.layerRenderOptionsSuite->AEGP_NewFromLayer(
            state.GetPluginID(), layerH, &optionsH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_NewFromLayer failed (error code: " + std::to_string(err) + ")");
        }

        return reinterpret_cast<uintptr_t>(optionsH);
    }, py::arg("layerH"),
    u8R"doc(レイヤーから新しいLayerRenderOptionsハンドルを作成します。

Args:
    layerH: レイヤーハンドル

Returns:
    LayerRenderOptionsハンドル (uintptr_t)

Note:
    戻り値のハンドルは AEGP_DisposeLayerRenderOptions で解放する必要があります。
    初期値:
    - Time: レイヤーの現在時間
    - TimeStep: レイヤーのフレーム長
    - EffectsToRender: "all"
)doc");

    sdk.def("AEGP_NewFromUpstreamOfEffect", [](uintptr_t effectH_ptr) -> uintptr_t {
        Validation::RequireNonNull(effectH_ptr, "effectH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        AEGP_LayerRenderOptionsH optionsH = nullptr;

        A_Err err = suites.layerRenderOptionsSuite->AEGP_NewFromUpstreamOfEffect(
            state.GetPluginID(), effectH, &optionsH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_NewFromUpstreamOfEffect failed (error code: " + std::to_string(err) + ")");
        }

        return reinterpret_cast<uintptr_t>(optionsH);
    }, py::arg("effectH"),
    u8R"doc(エフェクトの上流（入力側）から新しいLayerRenderOptionsハンドルを作成します。

Args:
    effectH: エフェクトハンドル

Returns:
    LayerRenderOptionsハンドル (uintptr_t)

Note:
    戻り値のハンドルは AEGP_DisposeLayerRenderOptions で解放する必要があります。
    EffectsToRender は指定されたエフェクトのインデックスに設定されます。
)doc");

    sdk.def("AEGP_NewFromDownstreamOfEffect", [](uintptr_t effectH_ptr) -> uintptr_t {
        Validation::RequireNonNull(effectH_ptr, "effectH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
        AEGP_LayerRenderOptionsH optionsH = nullptr;

        A_Err err = suites.layerRenderOptionsSuite->AEGP_NewFromDownstreamOfEffect(
            state.GetPluginID(), effectH, &optionsH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_NewFromDownstreamOfEffect failed (error code: " + std::to_string(err) + ")");
        }

        return reinterpret_cast<uintptr_t>(optionsH);
    }, py::arg("effectH"),
    u8R"doc(エフェクトの下流（出力側）から新しいLayerRenderOptionsハンドルを作成します。

Args:
    effectH: エフェクトハンドル

Returns:
    LayerRenderOptionsハンドル (uintptr_t)

Warning:
    この関数はUIスレッドからのみ呼び出すことができます。

Note:
    戻り値のハンドルは AEGP_DisposeLayerRenderOptions で解放する必要があります。
    EffectsToRender はエフェクト出力を含むように設定されます。
)doc");

    // -----------------------------------------------------------------------
    // Duplicate and Dispose
    // -----------------------------------------------------------------------

    sdk.def("AEGP_DuplicateLayerRenderOptions", [](uintptr_t optionsH_ptr) -> uintptr_t {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        AEGP_LayerRenderOptionsH copyH = nullptr;

        A_Err err = suites.layerRenderOptionsSuite->AEGP_Duplicate(
            state.GetPluginID(), optionsH, &copyH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_Duplicate (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }

        return reinterpret_cast<uintptr_t>(copyH);
    }, py::arg("optionsH"),
    u8R"doc(LayerRenderOptionsハンドルを複製します。

Args:
    optionsH: 複製元のLayerRenderOptionsハンドル

Returns:
    複製されたLayerRenderOptionsハンドル (uintptr_t)

Note:
    戻り値のハンドルは AEGP_DisposeLayerRenderOptions で解放する必要があります。
)doc");

    sdk.def("AEGP_DisposeLayerRenderOptions", [](uintptr_t optionsH_ptr) {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);

        A_Err err = suites.layerRenderOptionsSuite->AEGP_Dispose(optionsH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_Dispose (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }
    }, py::arg("optionsH"),
    u8R"doc(LayerRenderOptionsハンドルを解放します。

Args:
    optionsH: 解放するLayerRenderOptionsハンドル

Note:
    AEGP_NewFromLayer, AEGP_NewFromUpstreamOfEffect, AEGP_NewFromDownstreamOfEffect,
    AEGP_DuplicateLayerRenderOptions で取得したハンドルは必ずこの関数で解放してください。
)doc");

    // -----------------------------------------------------------------------
    // Time (render time)
    // -----------------------------------------------------------------------

    sdk.def("AEGP_SetLayerRenderOptionsTime", [](uintptr_t optionsH_ptr, double time_seconds) {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        A_Time time = AETypeUtils::SecondsToTime(time_seconds);

        A_Err err = suites.layerRenderOptionsSuite->AEGP_SetTime(optionsH, time);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetTime (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }
    }, py::arg("optionsH"), py::arg("time_seconds"),
    u8R"doc(レンダリング時間を設定します。

Args:
    optionsH: LayerRenderOptionsハンドル
    time_seconds: レンダリング時間（秒）
)doc");

    sdk.def("AEGP_GetLayerRenderOptionsTime", [](uintptr_t optionsH_ptr) -> double {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        A_Time time = {0, 1};

        A_Err err = suites.layerRenderOptionsSuite->AEGP_GetTime(optionsH, &time);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetTime (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }

        return AETypeUtils::TimeToSeconds(time);
    }, py::arg("optionsH"),
    u8R"doc(レンダリング時間を取得します。

Args:
    optionsH: LayerRenderOptionsハンドル

Returns:
    レンダリング時間（秒）
)doc");

    // -----------------------------------------------------------------------
    // Time Step (frame duration for motion blur)
    // -----------------------------------------------------------------------

    sdk.def("AEGP_SetLayerRenderOptionsTimeStep", [](uintptr_t optionsH_ptr, double time_step_seconds) {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        A_Time time_step = AETypeUtils::SecondsToTime(time_step_seconds);

        A_Err err = suites.layerRenderOptionsSuite->AEGP_SetTimeStep(optionsH, time_step);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetTimeStep (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }
    }, py::arg("optionsH"), py::arg("time_step_seconds"),
    u8R"doc(タイムステップ（フレーム長）を設定します。モーションブラーに影響します。

Args:
    optionsH: LayerRenderOptionsハンドル
    time_step_seconds: タイムステップ（秒）
)doc");

    sdk.def("AEGP_GetLayerRenderOptionsTimeStep", [](uintptr_t optionsH_ptr) -> double {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        A_Time time_step = {0, 1};

        A_Err err = suites.layerRenderOptionsSuite->AEGP_GetTimeStep(optionsH, &time_step);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetTimeStep (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }

        return AETypeUtils::TimeToSeconds(time_step);
    }, py::arg("optionsH"),
    u8R"doc(タイムステップ（フレーム長）を取得します。

Args:
    optionsH: LayerRenderOptionsハンドル

Returns:
    タイムステップ（秒）
)doc");

    // -----------------------------------------------------------------------
    // World Type (bit depth)
    // -----------------------------------------------------------------------

    sdk.def("AEGP_SetLayerRenderOptionsWorldType", [](uintptr_t optionsH_ptr, int type) {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");
        Validation::RequireRange(type, 0, 3, "type");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);

        A_Err err = suites.layerRenderOptionsSuite->AEGP_SetWorldType(optionsH, static_cast<AEGP_WorldType>(type));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetWorldType (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }
    }, py::arg("optionsH"), py::arg("type"),
    u8R"doc(ワールドタイプ（ビット深度）を設定します。

Args:
    optionsH: LayerRenderOptionsハンドル
    type: ワールドタイプ
        - 0: AEGP_WorldType_NONE
        - 1: AEGP_WorldType_8 (8-bit)
        - 2: AEGP_WorldType_16 (16-bit)
        - 3: AEGP_WorldType_32 (32-bit float)
)doc");

    sdk.def("AEGP_GetLayerRenderOptionsWorldType", [](uintptr_t optionsH_ptr) -> int {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        AEGP_WorldType type = AEGP_WorldType_NONE;

        A_Err err = suites.layerRenderOptionsSuite->AEGP_GetWorldType(optionsH, &type);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetWorldType (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }

        return static_cast<int>(type);
    }, py::arg("optionsH"),
    u8R"doc(ワールドタイプ（ビット深度）を取得します。

Args:
    optionsH: LayerRenderOptionsハンドル

Returns:
    ワールドタイプ:
        - 0: AEGP_WorldType_NONE
        - 1: AEGP_WorldType_8 (8-bit)
        - 2: AEGP_WorldType_16 (16-bit)
        - 3: AEGP_WorldType_32 (32-bit float)
)doc");

    // -----------------------------------------------------------------------
    // Downsample Factor
    // -----------------------------------------------------------------------

    sdk.def("AEGP_SetLayerRenderOptionsDownsampleFactor", [](uintptr_t optionsH_ptr, int x, int y) {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");
        Validation::RequirePositive(x, "x");
        Validation::RequirePositive(y, "y");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);

        A_Err err = suites.layerRenderOptionsSuite->AEGP_SetDownsampleFactor(
            optionsH, static_cast<A_short>(x), static_cast<A_short>(y));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetDownsampleFactor (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }
    }, py::arg("optionsH"), py::arg("x"), py::arg("y"),
    u8R"doc(ダウンサンプル係数を設定します。

Args:
    optionsH: LayerRenderOptionsハンドル
    x: 水平方向のダウンサンプル係数
    y: 垂直方向のダウンサンプル係数

Note:
    - 1 = 100% (フル解像度)
    - 2 = 50%
    - 3 = 33.3%
    - 4 = 25%
    等
)doc");

    sdk.def("AEGP_GetLayerRenderOptionsDownsampleFactor", [](uintptr_t optionsH_ptr) -> py::tuple {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        A_short x = 1, y = 1;

        A_Err err = suites.layerRenderOptionsSuite->AEGP_GetDownsampleFactor(optionsH, &x, &y);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetDownsampleFactor (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }

        return py::make_tuple(static_cast<int>(x), static_cast<int>(y));
    }, py::arg("optionsH"),
    u8R"doc(ダウンサンプル係数を取得します。

Args:
    optionsH: LayerRenderOptionsハンドル

Returns:
    tuple(x, y): ダウンサンプル係数
        - 1 = 100% (フル解像度)
        - 2 = 50%
        - 3 = 33.3%
        - 4 = 25%
        等
)doc");

    // -----------------------------------------------------------------------
    // Matte Mode
    // -----------------------------------------------------------------------

    sdk.def("AEGP_SetLayerRenderOptionsMatteMode", [](uintptr_t optionsH_ptr, int mode) {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");
        Validation::RequireRange(mode, 0, 2, "mode");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);

        A_Err err = suites.layerRenderOptionsSuite->AEGP_SetMatteMode(optionsH, static_cast<AEGP_MatteMode>(mode));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetMatteMode (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }
    }, py::arg("optionsH"), py::arg("mode"),
    u8R"doc(マットモードを設定します。

Args:
    optionsH: LayerRenderOptionsハンドル
    mode: マットモード
        - 0: AEGP_MatteMode_STRAIGHT (ストレートアルファ)
        - 1: AEGP_MatteMode_PREMUL_BLACK (黒でプリマルチプライ)
        - 2: AEGP_MatteMode_PREMUL_BG_COLOR (背景色でプリマルチプライ)
)doc");

    sdk.def("AEGP_GetLayerRenderOptionsMatteMode", [](uintptr_t optionsH_ptr) -> int {
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.layerRenderOptionsSuite) throw std::runtime_error("Layer Render Options Suite not available");

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        AEGP_MatteMode mode = AEGP_MatteMode_STRAIGHT;

        A_Err err = suites.layerRenderOptionsSuite->AEGP_GetMatteMode(optionsH, &mode);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetMatteMode (LayerRenderOptions) failed (error code: " + std::to_string(err) + ")");
        }

        return static_cast<int>(mode);
    }, py::arg("optionsH"),
    u8R"doc(マットモードを取得します。

Args:
    optionsH: LayerRenderOptionsハンドル

Returns:
    マットモード:
        - 0: AEGP_MatteMode_STRAIGHT (ストレートアルファ)
        - 1: AEGP_MatteMode_PREMUL_BLACK (黒でプリマルチプライ)
        - 2: AEGP_MatteMode_PREMUL_BG_COLOR (背景色でプリマルチプライ)
)doc");
}

} // namespace SDK
} // namespace PyAE
