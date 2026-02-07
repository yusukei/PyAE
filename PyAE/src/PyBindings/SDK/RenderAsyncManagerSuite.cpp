#include <pybind11/pybind11.h>
#include "PluginState.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_EffectSuites.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_RenderAsyncManagerSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_RenderAsyncManagerSuite1
    // -----------------------------------------------------------------------

    sdk.def("AEGP_CheckoutOrRender_ItemFrame_AsyncManager",
        [](uintptr_t async_managerP_ptr, unsigned long purpose_id, uintptr_t roH_ptr) -> uintptr_t {
        // Argument validation
        Validation::RequireNonNull(async_managerP_ptr, "async_managerP");
        Validation::RequireNonNull(roH_ptr, "roH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderAsyncManagerSuite) {
            throw std::runtime_error("RenderAsyncManager Suite not available");
        }

        PF_AsyncManagerP async_managerP = reinterpret_cast<PF_AsyncManagerP>(async_managerP_ptr);
        AEGP_RenderOptionsH roH = reinterpret_cast<AEGP_RenderOptionsH>(roH_ptr);

        AEGP_FrameReceiptH receiptH = nullptr;
        A_Err err = suites.renderAsyncManagerSuite->AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
            async_managerP,
            purpose_id,
            roH,
            &receiptH
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_CheckoutOrRender_ItemFrame_AsyncManager failed with error: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(receiptH);
    },
    py::arg("async_managerP"),
    py::arg("purpose_id"),
    py::arg("roH"),
    R"(非同期レンダリングマネージャを使用してアイテムフレームをチェックアウトまたはレンダリング

AsyncManagerは複数の非同期レンダリングリクエストを自動的に管理します。
同じpurpose_idで新しいリクエストが発行されると、古いリクエストは自動的にキャンセルされます。

Args:
    async_managerP (int): 非同期マネージャのポインタ（PF_AsyncManagerP）
    purpose_id (int): リクエストを識別するための一意ID（同じ用途のリクエストをキャンセルするヒントとして使用）
    roH (int): レンダリングするアイテムフレームの記述（AEGP_RenderOptionsH）

Returns:
    int: レンダリングされたフレームのレシート（AEGP_FrameReceiptH）
         成功した場合でもピクセルデータがない場合があります（worldなし）

Raises:
    RuntimeError: レンダリングリクエストが失敗した場合
    ValueError: 引数が無効な場合

Note:
    - このAPIは主にPF_Event_DRAWでのカスタムUI用（PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER）
    - purpose_idは、古いリクエストを自動キャンセルするための識別子です
    - フレームレシートはAEGP_FrameSuite経由でアクセスする必要があります
)");

    sdk.def("AEGP_CheckoutOrRender_LayerFrame_AsyncManager",
        [](uintptr_t async_managerP_ptr, unsigned long purpose_id, uintptr_t lroH_ptr) -> uintptr_t {
        // Argument validation
        Validation::RequireNonNull(async_managerP_ptr, "async_managerP");
        Validation::RequireNonNull(lroH_ptr, "lroH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderAsyncManagerSuite) {
            throw std::runtime_error("RenderAsyncManager Suite not available");
        }

        PF_AsyncManagerP async_managerP = reinterpret_cast<PF_AsyncManagerP>(async_managerP_ptr);
        AEGP_LayerRenderOptionsH lroH = reinterpret_cast<AEGP_LayerRenderOptionsH>(lroH_ptr);

        AEGP_FrameReceiptH receiptH = nullptr;
        A_Err err = suites.renderAsyncManagerSuite->AEGP_CheckoutOrRender_LayerFrame_AsyncManager(
            async_managerP,
            purpose_id,
            lroH,
            &receiptH
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_CheckoutOrRender_LayerFrame_AsyncManager failed with error: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(receiptH);
    },
    py::arg("async_managerP"),
    py::arg("purpose_id"),
    py::arg("lroH"),
    R"(非同期レンダリングマネージャを使用してレイヤーフレームをチェックアウトまたはレンダリング

AsyncManagerは複数の非同期レンダリングリクエストを自動的に管理します。
同じpurpose_idで新しいリクエストが発行されると、古いリクエストは自動的にキャンセルされます。

Args:
    async_managerP (int): 非同期マネージャのポインタ（PF_AsyncManagerP）
    purpose_id (int): リクエストを識別するための一意ID（同じ用途のリクエストをキャンセルするヒントとして使用）
    lroH (int): レンダリングするレイヤーフレームの記述（AEGP_LayerRenderOptionsH）

Returns:
    int: レンダリングされたフレームのレシート（AEGP_FrameReceiptH）
         成功した場合でもピクセルデータがない場合があります（worldなし）

Raises:
    RuntimeError: レンダリングリクエストが失敗した場合
    ValueError: 引数が無効な場合

Note:
    - このAPIは主にPF_Event_DRAWでのカスタムUI用（PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER）
    - purpose_idは、古いリクエストを自動キャンセルするための識別子です
    - フレームレシートはAEGP_FrameSuite経由でアクセスする必要があります
)");

}

} // namespace SDK
} // namespace PyAE
