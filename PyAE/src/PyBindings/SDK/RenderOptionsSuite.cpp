#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_RenderOptionsSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_RenderOptionsSuite4
    // -----------------------------------------------------------------------
    //
    // Render Options are used to configure rendering parameters for items
    // (compositions, footage). These settings control time, field rendering,
    // bit depth, downsampling, region of interest, matte mode, channel order,
    // guide layer rendering, and quality.
    //
    // IMPORTANT: Handle lifecycle
    // ---------------------------
    // - Handles created by AEGP_NewFromItem or AEGP_Duplicate MUST be disposed
    //   using AEGP_Dispose when no longer needed.
    // - Failure to dispose handles will result in memory leaks.
    //
    // Usage example:
    //   itemH = ae.sdk.AEGP_GetActiveItem()
    //   optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)
    //   ae.sdk.AEGP_RenderOptions_SetTime(optionsH, 1.5)  # Set to 1.5 seconds
    //   ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_16)
    //   # ... use for rendering ...
    //   ae.sdk.AEGP_RenderOptions_Dispose(optionsH)
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Constants
    // -----------------------------------------------------------------------

    // Matte Mode
    sdk.attr("AEGP_MatteMode_STRAIGHT") = py::int_(0);
    sdk.attr("AEGP_MatteMode_PREMUL_BLACK") = py::int_(1);
    sdk.attr("AEGP_MatteMode_PREMUL_BG_COLOR") = py::int_(2);

    // Channel Order
    sdk.attr("AEGP_ChannelOrder_ARGB") = py::int_(0);
    sdk.attr("AEGP_ChannelOrder_BGRA") = py::int_(1);

    // Item Quality (for footage decoding)
    sdk.attr("AEGP_ItemQuality_DRAFT") = py::int_(0);
    sdk.attr("AEGP_ItemQuality_BEST") = py::int_(1);

    // PF_Field (field rendering mode)
    sdk.attr("PF_Field_FRAME") = py::int_(0);
    sdk.attr("PF_Field_UPPER") = py::int_(1);
    sdk.attr("PF_Field_LOWER") = py::int_(2);

    // World Type (bit depth)
    sdk.attr("AEGP_WorldType_NONE") = py::int_(0);
    sdk.attr("AEGP_WorldType_8") = py::int_(1);
    sdk.attr("AEGP_WorldType_16") = py::int_(2);
    sdk.attr("AEGP_WorldType_32") = py::int_(3);

    // -----------------------------------------------------------------------
    // AEGP_NewFromItem
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_NewFromItem", [](uintptr_t itemH_ptr) -> uintptr_t {
        /**
         * AEGP_RenderOptions_NewFromItem
         *
         * アイテムから新しいレンダリングオプションを作成します。
         * 作成されたハンドルは使用後に AEGP_RenderOptions_Dispose で解放する必要があります。
         *
         * 初期値:
         * - Time: 0
         * - Time step: フレームデュレーション
         * - Field render: none
         * - Depth: アイテムの最高解像度
         *
         * Args:
         *     itemH (int): アイテムハンドル（uintptr_t）
         *
         * Returns:
         *     int: レンダリングオプションハンドル（uintptr_t）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: itemHがNULLの場合
         */
        Validation::RequireNonNull(itemH_ptr, "itemH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
        AEGP_RenderOptionsH optionsH = nullptr;

        A_Err err = suites.renderOptionsSuite->AEGP_NewFromItem(plugin_id, itemH, &optionsH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_NewFromItem failed with error code: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(optionsH);
    }, py::arg("itemH"),
    "アイテムから新しいレンダリングオプションを作成します。");

    // -----------------------------------------------------------------------
    // AEGP_Duplicate
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_Duplicate", [](uintptr_t optionsH_ptr) -> uintptr_t {
        /**
         * AEGP_RenderOptions_Duplicate
         *
         * レンダリングオプションを複製します。
         * 複製されたハンドルは使用後に AEGP_RenderOptions_Dispose で解放する必要があります。
         *
         * Args:
         *     optionsH (int): 複製元のレンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     int: 複製されたレンダリングオプションハンドル（uintptr_t）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_PluginID plugin_id = state.GetPluginID();
        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        AEGP_RenderOptionsH copyH = nullptr;

        A_Err err = suites.renderOptionsSuite->AEGP_Duplicate(plugin_id, optionsH, &copyH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_Duplicate failed with error code: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(copyH);
    }, py::arg("optionsH"),
    "レンダリングオプションを複製します。");

    // -----------------------------------------------------------------------
    // AEGP_Dispose
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_Dispose", [](uintptr_t optionsH_ptr) {
        /**
         * AEGP_RenderOptions_Dispose
         *
         * レンダリングオプションハンドルを解放します。
         * NewFromItem または Duplicate で作成したハンドルは必ずこの関数で解放してください。
         *
         * Args:
         *     optionsH (int): 解放するレンダリングオプションハンドル（uintptr_t）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_Err err = suites.renderOptionsSuite->AEGP_Dispose(optionsH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_Dispose failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"),
    "レンダリングオプションハンドルを解放します。");

    // -----------------------------------------------------------------------
    // AEGP_SetTime
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetTime", [](uintptr_t optionsH_ptr, double time_seconds) {
        /**
         * AEGP_RenderOptions_SetTime
         *
         * レンダリング時間を設定します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     time_seconds (float): レンダリング時間（秒）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        A_Time time = AETypeUtils::SecondsToTime(time_seconds);

        A_Err err = suites.renderOptionsSuite->AEGP_SetTime(optionsH, time);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetTime failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("time_seconds"),
    "レンダリング時間を設定します。");

    // -----------------------------------------------------------------------
    // AEGP_GetTime
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetTime", [](uintptr_t optionsH_ptr) -> double {
        /**
         * AEGP_RenderOptions_GetTime
         *
         * レンダリング時間を取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     float: レンダリング時間（秒）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        A_Time time = {0, 1};

        A_Err err = suites.renderOptionsSuite->AEGP_GetTime(optionsH, &time);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetTime failed with error code: " + std::to_string(err));
        }

        return AETypeUtils::TimeToSeconds(time);
    }, py::arg("optionsH"),
    "レンダリング時間を取得します。");

    // -----------------------------------------------------------------------
    // AEGP_SetTimeStep
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetTimeStep", [](uintptr_t optionsH_ptr, double time_step_seconds) {
        /**
         * AEGP_RenderOptions_SetTimeStep
         *
         * フレームの時間幅を設定します。モーションブラーに重要な設定です。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     time_step_seconds (float): 時間幅（秒）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        A_Time time_step = AETypeUtils::SecondsToTime(time_step_seconds);

        A_Err err = suites.renderOptionsSuite->AEGP_SetTimeStep(optionsH, time_step);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetTimeStep failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("time_step_seconds"),
    "フレームの時間幅を設定します（モーションブラー用）。");

    // -----------------------------------------------------------------------
    // AEGP_GetTimeStep
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetTimeStep", [](uintptr_t optionsH_ptr) -> double {
        /**
         * AEGP_RenderOptions_GetTimeStep
         *
         * フレームの時間幅を取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     float: 時間幅（秒）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        A_Time time_step = {0, 1};

        A_Err err = suites.renderOptionsSuite->AEGP_GetTimeStep(optionsH, &time_step);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetTimeStep failed with error code: " + std::to_string(err));
        }

        return AETypeUtils::TimeToSeconds(time_step);
    }, py::arg("optionsH"),
    "フレームの時間幅を取得します。");

    // -----------------------------------------------------------------------
    // AEGP_SetFieldRender
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetFieldRender", [](uintptr_t optionsH_ptr, int field_render) {
        /**
         * AEGP_RenderOptions_SetFieldRender
         *
         * フィールドレンダリングモードを設定します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     field_render (int): フィールドモード
         *                         PF_Field_FRAME (0): フレーム（フィールドなし）
         *                         PF_Field_UPPER (1): 上位フィールド
         *                         PF_Field_LOWER (2): 下位フィールド
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_Err err = suites.renderOptionsSuite->AEGP_SetFieldRender(optionsH, static_cast<PF_Field>(field_render));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetFieldRender failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("field_render"),
    "フィールドレンダリングモードを設定します。");

    // -----------------------------------------------------------------------
    // AEGP_GetFieldRender
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetFieldRender", [](uintptr_t optionsH_ptr) -> int {
        /**
         * AEGP_RenderOptions_GetFieldRender
         *
         * フィールドレンダリングモードを取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     int: フィールドモード（PF_Field_FRAME=0, PF_Field_UPPER=1, PF_Field_LOWER=2）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        PF_Field field_render = PF_Field_FRAME;

        A_Err err = suites.renderOptionsSuite->AEGP_GetFieldRender(optionsH, &field_render);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetFieldRender failed with error code: " + std::to_string(err));
        }

        return static_cast<int>(field_render);
    }, py::arg("optionsH"),
    "フィールドレンダリングモードを取得します。");

    // -----------------------------------------------------------------------
    // AEGP_SetWorldType
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetWorldType", [](uintptr_t optionsH_ptr, int type) {
        /**
         * AEGP_RenderOptions_SetWorldType
         *
         * ワールドタイプ（ビット深度）を設定します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     type (int): ワールドタイプ
         *                 AEGP_WorldType_NONE (0)
         *                 AEGP_WorldType_8 (1): 8bpc
         *                 AEGP_WorldType_16 (2): 16bpc
         *                 AEGP_WorldType_32 (3): 32bpc (float)
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_Err err = suites.renderOptionsSuite->AEGP_SetWorldType(optionsH, static_cast<AEGP_WorldType>(type));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetWorldType failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("type"),
    "ワールドタイプ（ビット深度）を設定します。");

    // -----------------------------------------------------------------------
    // AEGP_GetWorldType
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetWorldType", [](uintptr_t optionsH_ptr) -> int {
        /**
         * AEGP_RenderOptions_GetWorldType
         *
         * ワールドタイプ（ビット深度）を取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     int: ワールドタイプ
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        AEGP_WorldType type = AEGP_WorldType_NONE;

        A_Err err = suites.renderOptionsSuite->AEGP_GetWorldType(optionsH, &type);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetWorldType failed with error code: " + std::to_string(err));
        }

        return static_cast<int>(type);
    }, py::arg("optionsH"),
    "ワールドタイプ（ビット深度）を取得します。");

    // -----------------------------------------------------------------------
    // AEGP_SetDownsampleFactor
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetDownsampleFactor", [](uintptr_t optionsH_ptr, int x, int y) {
        /**
         * AEGP_RenderOptions_SetDownsampleFactor
         *
         * ダウンサンプル係数を設定します。
         * 1 = 100%, 2 = 50%, 4 = 25% など。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     x (int): 水平方向のダウンサンプル係数（1以上）
         *     y (int): 垂直方向のダウンサンプル係数（1以上）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合、またはx,yが0以下の場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");
        Validation::RequirePositive(x, "x");
        Validation::RequirePositive(y, "y");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_Err err = suites.renderOptionsSuite->AEGP_SetDownsampleFactor(
            optionsH, static_cast<A_short>(x), static_cast<A_short>(y));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetDownsampleFactor failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("x"), py::arg("y"),
    "ダウンサンプル係数を設定します（1=100%, 2=50%）。");

    // -----------------------------------------------------------------------
    // AEGP_GetDownsampleFactor
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetDownsampleFactor", [](uintptr_t optionsH_ptr) -> py::tuple {
        /**
         * AEGP_RenderOptions_GetDownsampleFactor
         *
         * ダウンサンプル係数を取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     tuple: (x, y) ダウンサンプル係数
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        A_short x = 1, y = 1;

        A_Err err = suites.renderOptionsSuite->AEGP_GetDownsampleFactor(optionsH, &x, &y);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetDownsampleFactor failed with error code: " + std::to_string(err));
        }

        return py::make_tuple(static_cast<int>(x), static_cast<int>(y));
    }, py::arg("optionsH"),
    "ダウンサンプル係数を取得します。(x, y)のタプルを返します。");

    // -----------------------------------------------------------------------
    // AEGP_SetRegionOfInterest
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetRegionOfInterest", [](uintptr_t optionsH_ptr, const py::dict& roi) {
        /**
         * AEGP_RenderOptions_SetRegionOfInterest
         *
         * 関心領域（ROI）を設定します。
         * {left:0, top:0, right:0, bottom:0}を指定すると全体をレンダリングします。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     roi (dict): 関心領域 {"left": int, "top": int, "right": int, "bottom": int}
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_LRect roi_rect = {0, 0, 0, 0};
        if (roi.contains("left")) roi_rect.left = roi["left"].cast<A_long>();
        if (roi.contains("top")) roi_rect.top = roi["top"].cast<A_long>();
        if (roi.contains("right")) roi_rect.right = roi["right"].cast<A_long>();
        if (roi.contains("bottom")) roi_rect.bottom = roi["bottom"].cast<A_long>();

        A_Err err = suites.renderOptionsSuite->AEGP_SetRegionOfInterest(optionsH, &roi_rect);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetRegionOfInterest failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("roi"),
    "関心領域（ROI）を設定します。{left:0,top:0,right:0,bottom:0}で全体。");

    // -----------------------------------------------------------------------
    // AEGP_GetRegionOfInterest
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetRegionOfInterest", [](uintptr_t optionsH_ptr) -> py::dict {
        /**
         * AEGP_RenderOptions_GetRegionOfInterest
         *
         * 関心領域（ROI）を取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     dict: {"left": int, "top": int, "right": int, "bottom": int}
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        A_LRect roi_rect = {0, 0, 0, 0};

        A_Err err = suites.renderOptionsSuite->AEGP_GetRegionOfInterest(optionsH, &roi_rect);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetRegionOfInterest failed with error code: " + std::to_string(err));
        }

        py::dict d;
        d["left"] = roi_rect.left;
        d["top"] = roi_rect.top;
        d["right"] = roi_rect.right;
        d["bottom"] = roi_rect.bottom;
        return d;
    }, py::arg("optionsH"),
    "関心領域（ROI）を取得します。");

    // -----------------------------------------------------------------------
    // AEGP_SetMatteMode
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetMatteMode", [](uintptr_t optionsH_ptr, int mode) {
        /**
         * AEGP_RenderOptions_SetMatteMode
         *
         * マットモードを設定します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     mode (int): マットモード
         *                 AEGP_MatteMode_STRAIGHT (0): ストレートアルファ
         *                 AEGP_MatteMode_PREMUL_BLACK (1): プリマルチプライド（黒）
         *                 AEGP_MatteMode_PREMUL_BG_COLOR (2): プリマルチプライド（背景色）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_Err err = suites.renderOptionsSuite->AEGP_SetMatteMode(optionsH, static_cast<AEGP_MatteMode>(mode));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetMatteMode failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("mode"),
    "マットモードを設定します。");

    // -----------------------------------------------------------------------
    // AEGP_GetMatteMode
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetMatteMode", [](uintptr_t optionsH_ptr) -> int {
        /**
         * AEGP_RenderOptions_GetMatteMode
         *
         * マットモードを取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     int: マットモード
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        AEGP_MatteMode mode = AEGP_MatteMode_STRAIGHT;

        A_Err err = suites.renderOptionsSuite->AEGP_GetMatteMode(optionsH, &mode);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetMatteMode failed with error code: " + std::to_string(err));
        }

        return static_cast<int>(mode);
    }, py::arg("optionsH"),
    "マットモードを取得します。");

    // -----------------------------------------------------------------------
    // AEGP_SetChannelOrder
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetChannelOrder", [](uintptr_t optionsH_ptr, int order) {
        /**
         * AEGP_RenderOptions_SetChannelOrder
         *
         * チャンネル順序を設定します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     order (int): チャンネル順序
         *                  AEGP_ChannelOrder_ARGB (0)
         *                  AEGP_ChannelOrder_BGRA (1)
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_Err err = suites.renderOptionsSuite->AEGP_SetChannelOrder(optionsH, static_cast<AEGP_ChannelOrder>(order));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetChannelOrder failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("order"),
    "チャンネル順序を設定します。");

    // -----------------------------------------------------------------------
    // AEGP_GetChannelOrder
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetChannelOrder", [](uintptr_t optionsH_ptr) -> int {
        /**
         * AEGP_RenderOptions_GetChannelOrder
         *
         * チャンネル順序を取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     int: チャンネル順序
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        AEGP_ChannelOrder order = AEGP_ChannelOrder_ARGB;

        A_Err err = suites.renderOptionsSuite->AEGP_GetChannelOrder(optionsH, &order);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetChannelOrder failed with error code: " + std::to_string(err));
        }

        return static_cast<int>(order);
    }, py::arg("optionsH"),
    "チャンネル順序を取得します。");

    // -----------------------------------------------------------------------
    // AEGP_GetRenderGuideLayers
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetRenderGuideLayers", [](uintptr_t optionsH_ptr) -> bool {
        /**
         * AEGP_RenderOptions_GetRenderGuideLayers
         *
         * ガイドレイヤーをレンダリングするかどうかを取得します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     bool: ガイドレイヤーをレンダリングする場合True
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        A_Boolean will_render = FALSE;

        A_Err err = suites.renderOptionsSuite->AEGP_GetRenderGuideLayers(optionsH, &will_render);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetRenderGuideLayers failed with error code: " + std::to_string(err));
        }

        return static_cast<bool>(will_render);
    }, py::arg("optionsH"),
    "ガイドレイヤーをレンダリングするかどうかを取得します。");

    // -----------------------------------------------------------------------
    // AEGP_SetRenderGuideLayers
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetRenderGuideLayers", [](uintptr_t optionsH_ptr, bool render_them) {
        /**
         * AEGP_RenderOptions_SetRenderGuideLayers
         *
         * ガイドレイヤーをレンダリングするかどうかを設定します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     render_them (bool): Trueでガイドレイヤーをレンダリング
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_Err err = suites.renderOptionsSuite->AEGP_SetRenderGuideLayers(optionsH, render_them ? TRUE : FALSE);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetRenderGuideLayers failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("render_them"),
    "ガイドレイヤーをレンダリングするかどうかを設定します。");

    // -----------------------------------------------------------------------
    // AEGP_GetRenderQuality
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_GetRenderQuality", [](uintptr_t optionsH_ptr) -> int {
        /**
         * AEGP_RenderOptions_GetRenderQuality
         *
         * レンダリング品質を取得します。
         * この設定はフッテージアイテムのデコード品質に影響します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *
         * Returns:
         *     int: 品質レベル
         *          AEGP_ItemQuality_DRAFT (0): ドラフト品質（高速）
         *          AEGP_ItemQuality_BEST (1): 最高品質
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        AEGP_ItemQuality quality = AEGP_ItemQuality_BEST;

        A_Err err = suites.renderOptionsSuite->AEGP_GetRenderQuality(optionsH, &quality);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetRenderQuality failed with error code: " + std::to_string(err));
        }

        return static_cast<int>(quality);
    }, py::arg("optionsH"),
    "レンダリング品質を取得します（フッテージ用）。");

    // -----------------------------------------------------------------------
    // AEGP_SetRenderQuality
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderOptions_SetRenderQuality", [](uintptr_t optionsH_ptr, int quality) {
        /**
         * AEGP_RenderOptions_SetRenderQuality
         *
         * レンダリング品質を設定します。
         * この設定はフッテージアイテムのデコード品質に影響します。
         *
         * Args:
         *     optionsH (int): レンダリングオプションハンドル（uintptr_t）
         *     quality (int): 品質レベル
         *                    AEGP_ItemQuality_DRAFT (0): ドラフト品質（高速）
         *                    AEGP_ItemQuality_BEST (1): 最高品質
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: optionsHがNULLの場合
         */
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderOptionsSuite) throw std::runtime_error("RenderOptions Suite not available");

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);

        A_Err err = suites.renderOptionsSuite->AEGP_SetRenderQuality(optionsH, static_cast<AEGP_ItemQuality>(quality));
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetRenderQuality failed with error code: " + std::to_string(err));
        }
    }, py::arg("optionsH"), py::arg("quality"),
    "レンダリング品質を設定します（フッテージ用）。");
}

} // namespace SDK
} // namespace PyAE
