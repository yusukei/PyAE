#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_EffectCB.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_CompositeSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_CompositeSuite2
    // -----------------------------------------------------------------------

    sdk.def("AEGP_ClearAlphaExceptRect", [](uintptr_t dst_worldH, const py::dict& rect) {
        /**
         * AEGP_ClearAlphaExceptRect
         *
         * 指定された矩形領域以外のアルファチャンネルをクリアします。
         *
         * Args:
         *     dst_worldH (int): 対象となるPF_EffectWorldのハンドル（uintptr_t）
         *     rect (dict): クリアしない領域の矩形 {"left": int, "top": int, "right": int, "bottom": int}
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: 引数が不正な場合
         */
        // Argument validation
        Validation::RequireNonNull(dst_worldH, "dst_worldH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compositeSuite) throw std::runtime_error("Composite Suite not available");

        PF_EffectWorld* dst_world = reinterpret_cast<PF_EffectWorld*>(dst_worldH);

        // Build A_Rect from dict
        A_Rect clipped_rect = {0, 0, 0, 0};
        if (rect.contains("left")) clipped_rect.left = rect["left"].cast<A_long>();
        if (rect.contains("top")) clipped_rect.top = rect["top"].cast<A_long>();
        if (rect.contains("right")) clipped_rect.right = rect["right"].cast<A_long>();
        if (rect.contains("bottom")) clipped_rect.bottom = rect["bottom"].cast<A_long>();

        A_Err err = suites.compositeSuite->AEGP_ClearAlphaExceptRect(&clipped_rect, dst_world);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_ClearAlphaExceptRect failed with error code: " + std::to_string(err));
        }
    }, py::arg("dst_worldH"), py::arg("rect"),
    "指定された矩形領域以外のアルファチャンネルをクリアします。");

    sdk.def("AEGP_PrepTrackMatte", [](int num_pix, bool deepB, uintptr_t src_mask_ptr, int mask_flags) -> py::bytes {
        /**
         * AEGP_PrepTrackMatte
         *
         * トラックマットの前処理を行います。
         *
         * Args:
         *     num_pix (int): ピクセル数
         *     deepB (bool): 16bpcの場合True、8bpcの場合False
         *     src_mask_ptr (int): ソースマスクデータのポインタ（uintptr_t）
         *     mask_flags (int): マスクフラグ（PF_MaskFlags）
         *
         * Returns:
         *     bytes: 処理済みマスクデータ（バイナリ）
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: 引数が不正な場合
         */
        // Argument validation
        Validation::RequireNonNegative(num_pix, "num_pix");
        Validation::RequireNonNull(src_mask_ptr, "src_mask_ptr");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compositeSuite) throw std::runtime_error("Composite Suite not available");

        const PF_Pixel* src_mask = reinterpret_cast<const PF_Pixel*>(src_mask_ptr);

        // Allocate destination buffer
        size_t pixel_size = sizeof(PF_Pixel);
        std::vector<PF_Pixel> dst_mask(num_pix);

        A_Err err = suites.compositeSuite->AEGP_PrepTrackMatte(
            num_pix,
            deepB ? TRUE : FALSE,
            src_mask,
            static_cast<PF_MaskFlags>(mask_flags),
            dst_mask.data()
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_PrepTrackMatte failed with error code: " + std::to_string(err));
        }

        // Return as bytes
        return py::bytes(reinterpret_cast<const char*>(dst_mask.data()), num_pix * pixel_size);
    }, py::arg("num_pix"), py::arg("deepB"), py::arg("src_mask_ptr"), py::arg("mask_flags"),
    "トラックマットの前処理を行います。");

    sdk.def("AEGP_TransferRect", [](
        int quality,
        int m_flags,
        int field,
        const py::dict& src_rect,
        uintptr_t src_worldH,
        uintptr_t comp_mode_ptr,
        uintptr_t blend_tables_ptr,
        uintptr_t mask_worldH,
        int dest_x,
        int dest_y,
        uintptr_t dst_worldH) {
        /**
         * AEGP_TransferRect
         *
         * ソース画像から矩形領域を転送し、コンポジット処理を行います。
         *
         * Args:
         *     quality (int): レンダリング品質（PF_Quality）
         *     m_flags (int): モードフラグ（PF_ModeFlags）
         *     field (int): フィールドモード（PF_Field）
         *     src_rect (dict): ソース矩形 {"left": int, "top": int, "right": int, "bottom": int}
         *     src_worldH (int): ソースPF_EffectWorldのハンドル
         *     comp_mode_ptr (int): コンポジットモード（PF_CompositeMode*）、0でNULL
         *     blend_tables_ptr (int): ブレンドテーブル（PF_EffectBlendingTables）、0でNULL
         *     mask_worldH (int): マスクワールド（PF_MaskWorld*）、0でNULL
         *     dest_x (int): 転送先X座標
         *     dest_y (int): 転送先Y座標
         *     dst_worldH (int): 転送先PF_EffectWorldのハンドル
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: 引数が不正な場合
         */
        // Argument validation
        Validation::RequireNonNull(src_worldH, "src_worldH");
        Validation::RequireNonNull(dst_worldH, "dst_worldH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compositeSuite) throw std::runtime_error("Composite Suite not available");

        // Build A_Rect from dict
        A_Rect src_r = {0, 0, 0, 0};
        if (src_rect.contains("left")) src_r.left = src_rect["left"].cast<A_long>();
        if (src_rect.contains("top")) src_r.top = src_rect["top"].cast<A_long>();
        if (src_rect.contains("right")) src_r.right = src_rect["right"].cast<A_long>();
        if (src_rect.contains("bottom")) src_r.bottom = src_rect["bottom"].cast<A_long>();

        PF_EffectWorld* src_world = reinterpret_cast<PF_EffectWorld*>(src_worldH);
        PF_EffectWorld* dst_world = reinterpret_cast<PF_EffectWorld*>(dst_worldH);
        const PF_CompositeMode* comp_mode = comp_mode_ptr ? reinterpret_cast<const PF_CompositeMode*>(comp_mode_ptr) : nullptr;
        PF_EffectBlendingTables blend_tables = blend_tables_ptr ? reinterpret_cast<PF_EffectBlendingTables>(blend_tables_ptr) : nullptr;
        const PF_MaskWorld* mask_world = mask_worldH ? reinterpret_cast<const PF_MaskWorld*>(mask_worldH) : nullptr;

        A_Err err = suites.compositeSuite->AEGP_TransferRect(
            static_cast<PF_Quality>(quality),
            static_cast<PF_ModeFlags>(m_flags),
            static_cast<PF_Field>(field),
            &src_r,
            src_world,
            comp_mode,
            blend_tables,
            mask_world,
            dest_x,
            dest_y,
            dst_world
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_TransferRect failed with error code: " + std::to_string(err));
        }
    }, py::arg("quality"), py::arg("m_flags"), py::arg("field"), py::arg("src_rect"),
       py::arg("src_worldH"), py::arg("comp_mode_ptr"), py::arg("blend_tables_ptr"),
       py::arg("mask_worldH"), py::arg("dest_x"), py::arg("dest_y"), py::arg("dst_worldH"),
    "ソース画像から矩形領域を転送し、コンポジット処理を行います。");

    sdk.def("AEGP_CopyBits_LQ", [](
        uintptr_t src_worldH,
        py::object src_rect_obj,
        py::object dst_rect_obj,
        uintptr_t dst_worldH) {
        /**
         * AEGP_CopyBits_LQ
         *
         * 低品質モードでビットをコピーします（高速）。
         *
         * Args:
         *     src_worldH (int): ソースPF_EffectWorldのハンドル
         *     src_rect (dict|None): ソース矩形、Noneの場合はワールド全体
         *     dst_rect (dict|None): 転送先矩形、Noneの場合はワールド全体
         *     dst_worldH (int): 転送先PF_EffectWorldのハンドル
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: 引数が不正な場合
         */
        // Argument validation
        Validation::RequireNonNull(src_worldH, "src_worldH");
        Validation::RequireNonNull(dst_worldH, "dst_worldH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compositeSuite) throw std::runtime_error("Composite Suite not available");

        PF_EffectWorld* src_world = reinterpret_cast<PF_EffectWorld*>(src_worldH);
        PF_EffectWorld* dst_world = reinterpret_cast<PF_EffectWorld*>(dst_worldH);

        A_Rect src_r = {0, 0, 0, 0};
        A_Rect dst_r = {0, 0, 0, 0};
        A_Rect* src_r_ptr = nullptr;
        A_Rect* dst_r_ptr = nullptr;

        if (!src_rect_obj.is_none()) {
            py::dict src_rect = src_rect_obj.cast<py::dict>();
            if (src_rect.contains("left")) src_r.left = src_rect["left"].cast<A_long>();
            if (src_rect.contains("top")) src_r.top = src_rect["top"].cast<A_long>();
            if (src_rect.contains("right")) src_r.right = src_rect["right"].cast<A_long>();
            if (src_rect.contains("bottom")) src_r.bottom = src_rect["bottom"].cast<A_long>();
            src_r_ptr = &src_r;
        }

        if (!dst_rect_obj.is_none()) {
            py::dict dst_rect = dst_rect_obj.cast<py::dict>();
            if (dst_rect.contains("left")) dst_r.left = dst_rect["left"].cast<A_long>();
            if (dst_rect.contains("top")) dst_r.top = dst_rect["top"].cast<A_long>();
            if (dst_rect.contains("right")) dst_r.right = dst_rect["right"].cast<A_long>();
            if (dst_rect.contains("bottom")) dst_r.bottom = dst_rect["bottom"].cast<A_long>();
            dst_r_ptr = &dst_r;
        }

        A_Err err = suites.compositeSuite->AEGP_CopyBits_LQ(src_world, src_r_ptr, dst_r_ptr, dst_world);

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_CopyBits_LQ failed with error code: " + std::to_string(err));
        }
    }, py::arg("src_worldH"), py::arg("src_rect"), py::arg("dst_rect"), py::arg("dst_worldH"),
    "低品質モードでビットをコピーします（高速）。");

    sdk.def("AEGP_CopyBits_HQ_Straight", [](
        uintptr_t src_worldH,
        py::object src_rect_obj,
        py::object dst_rect_obj,
        uintptr_t dst_worldH) {
        /**
         * AEGP_CopyBits_HQ_Straight
         *
         * 高品質モードでビットをコピーします（ストレートアルファ）。
         *
         * Args:
         *     src_worldH (int): ソースPF_EffectWorldのハンドル
         *     src_rect (dict|None): ソース矩形、Noneの場合はワールド全体
         *     dst_rect (dict|None): 転送先矩形、Noneの場合はワールド全体
         *     dst_worldH (int): 転送先PF_EffectWorldのハンドル
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: 引数が不正な場合
         */
        // Argument validation
        Validation::RequireNonNull(src_worldH, "src_worldH");
        Validation::RequireNonNull(dst_worldH, "dst_worldH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compositeSuite) throw std::runtime_error("Composite Suite not available");

        PF_EffectWorld* src_world = reinterpret_cast<PF_EffectWorld*>(src_worldH);
        PF_EffectWorld* dst_world = reinterpret_cast<PF_EffectWorld*>(dst_worldH);

        A_Rect src_r = {0, 0, 0, 0};
        A_Rect dst_r = {0, 0, 0, 0};
        A_Rect* src_r_ptr = nullptr;
        A_Rect* dst_r_ptr = nullptr;

        if (!src_rect_obj.is_none()) {
            py::dict src_rect = src_rect_obj.cast<py::dict>();
            if (src_rect.contains("left")) src_r.left = src_rect["left"].cast<A_long>();
            if (src_rect.contains("top")) src_r.top = src_rect["top"].cast<A_long>();
            if (src_rect.contains("right")) src_r.right = src_rect["right"].cast<A_long>();
            if (src_rect.contains("bottom")) src_r.bottom = src_rect["bottom"].cast<A_long>();
            src_r_ptr = &src_r;
        }

        if (!dst_rect_obj.is_none()) {
            py::dict dst_rect = dst_rect_obj.cast<py::dict>();
            if (dst_rect.contains("left")) dst_r.left = dst_rect["left"].cast<A_long>();
            if (dst_rect.contains("top")) dst_r.top = dst_rect["top"].cast<A_long>();
            if (dst_rect.contains("right")) dst_r.right = dst_rect["right"].cast<A_long>();
            if (dst_rect.contains("bottom")) dst_r.bottom = dst_rect["bottom"].cast<A_long>();
            dst_r_ptr = &dst_r;
        }

        A_Err err = suites.compositeSuite->AEGP_CopyBits_HQ_Straight(src_world, src_r_ptr, dst_r_ptr, dst_world);

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_CopyBits_HQ_Straight failed with error code: " + std::to_string(err));
        }
    }, py::arg("src_worldH"), py::arg("src_rect"), py::arg("dst_rect"), py::arg("dst_worldH"),
    "高品質モードでビットをコピーします（ストレートアルファ）。");

    sdk.def("AEGP_CopyBits_HQ_Premul", [](
        uintptr_t src_worldH,
        py::object src_rect_obj,
        py::object dst_rect_obj,
        uintptr_t dst_worldH) {
        /**
         * AEGP_CopyBits_HQ_Premul
         *
         * 高品質モードでビットをコピーします（プリマルチプライドアルファ）。
         *
         * Args:
         *     src_worldH (int): ソースPF_EffectWorldのハンドル
         *     src_rect (dict|None): ソース矩形、Noneの場合はワールド全体
         *     dst_rect (dict|None): 転送先矩形、Noneの場合はワールド全体
         *     dst_worldH (int): 転送先PF_EffectWorldのハンドル
         *
         * Raises:
         *     RuntimeError: API呼び出しに失敗した場合
         *     ValueError: 引数が不正な場合
         */
        // Argument validation
        Validation::RequireNonNull(src_worldH, "src_worldH");
        Validation::RequireNonNull(dst_worldH, "dst_worldH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.compositeSuite) throw std::runtime_error("Composite Suite not available");

        PF_EffectWorld* src_world = reinterpret_cast<PF_EffectWorld*>(src_worldH);
        PF_EffectWorld* dst_world = reinterpret_cast<PF_EffectWorld*>(dst_worldH);

        A_Rect src_r = {0, 0, 0, 0};
        A_Rect dst_r = {0, 0, 0, 0};
        A_Rect* src_r_ptr = nullptr;
        A_Rect* dst_r_ptr = nullptr;

        if (!src_rect_obj.is_none()) {
            py::dict src_rect = src_rect_obj.cast<py::dict>();
            if (src_rect.contains("left")) src_r.left = src_rect["left"].cast<A_long>();
            if (src_rect.contains("top")) src_r.top = src_rect["top"].cast<A_long>();
            if (src_rect.contains("right")) src_r.right = src_rect["right"].cast<A_long>();
            if (src_rect.contains("bottom")) src_r.bottom = src_rect["bottom"].cast<A_long>();
            src_r_ptr = &src_r;
        }

        if (!dst_rect_obj.is_none()) {
            py::dict dst_rect = dst_rect_obj.cast<py::dict>();
            if (dst_rect.contains("left")) dst_r.left = dst_rect["left"].cast<A_long>();
            if (dst_rect.contains("top")) dst_r.top = dst_rect["top"].cast<A_long>();
            if (dst_rect.contains("right")) dst_r.right = dst_rect["right"].cast<A_long>();
            if (dst_rect.contains("bottom")) dst_r.bottom = dst_rect["bottom"].cast<A_long>();
            dst_r_ptr = &dst_r;
        }

        A_Err err = suites.compositeSuite->AEGP_CopyBits_HQ_Premul(src_world, src_r_ptr, dst_r_ptr, dst_world);

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_CopyBits_HQ_Premul failed with error code: " + std::to_string(err));
        }
    }, py::arg("src_worldH"), py::arg("src_rect"), py::arg("dst_rect"), py::arg("dst_worldH"),
    "高品質モードでビットをコピーします（プリマルチプライドアルファ）。");
}

} // namespace SDK
} // namespace PyAE
