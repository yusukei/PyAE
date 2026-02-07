// RenderSuite.cpp
// PyAE - Python for After Effects
// AEGP_RenderSuite5 Python Bindings
//
// Provides Python bindings for After Effects Render Suite.
// This suite handles frame rendering and sound data rendering.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_EffectSuites.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_RenderSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_RenderSuite5 - Frame and Sound Rendering Functions
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // 1. AEGP_RenderAndCheckoutFrame
    // Synchronously render and checkout a frame
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderAndCheckoutFrame",
        [](uintptr_t optionsH_ptr) -> uintptr_t {
        // Argument validation
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_RenderOptionsH optionsH = reinterpret_cast<AEGP_RenderOptionsH>(optionsH_ptr);
        AEGP_FrameReceiptH receiptH = nullptr;

        // cancel_functionP0 and cancel_function_refconP0 are set to nullptr
        // because Python cannot provide C callbacks
        A_Err err = suites.renderSuite->AEGP_RenderAndCheckoutFrame(
            optionsH,
            nullptr,  // cancel_functionP0 - not available from Python
            nullptr,  // cancel_function_refconP0
            &receiptH
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_RenderAndCheckoutFrame failed with error: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(receiptH);
    },
    py::arg("optionsH"),
    R"doc(フレームを同期的にレンダリングしてチェックアウト

レンダリングオプションで指定されたフレームをレンダリングし、
フレームレシートを返します。

Args:
    optionsH (int): レンダリングオプションハンドル（AEGP_RenderOptionsH）
                   AEGP_NewFromItemで作成

Returns:
    int: フレームレシートハンドル（AEGP_FrameReceiptH）
         使用後はAEGP_CheckinFrameでチェックインする必要があります

Raises:
    RuntimeError: レンダリングが失敗した場合
    ValueError: optionsHがnullの場合

Note:
    - UIスレッドでの同期呼び出しは非推奨
    - キャンセルコールバックはPythonから使用不可（nullptr固定）
    - レンダリング完了後は必ずAEGP_CheckinFrameを呼び出してください
)doc");

    // -----------------------------------------------------------------------
    // 2. AEGP_RenderAndCheckoutLayerFrame
    // Synchronously render and checkout a layer frame
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderAndCheckoutLayerFrame",
        [](uintptr_t optionsH_ptr) -> uintptr_t {
        // Argument validation
        Validation::RequireNonNull(optionsH_ptr, "optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(optionsH_ptr);
        AEGP_FrameReceiptH receiptH = nullptr;

        // cancel_functionP0 and cancel_function_refconP0 are set to nullptr
        A_Err err = suites.renderSuite->AEGP_RenderAndCheckoutLayerFrame(
            optionsH,
            nullptr,  // cancel_functionP0 - not available from Python
            nullptr,  // cancel_function_refconP0
            &receiptH
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_RenderAndCheckoutLayerFrame failed with error: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(receiptH);
    },
    py::arg("optionsH"),
    R"doc(レイヤーフレームを同期的にレンダリングしてチェックアウト

レイヤーレンダリングオプションで指定されたフレームをレンダリングし、
フレームレシートを返します。

Args:
    optionsH (int): レイヤーレンダリングオプションハンドル（AEGP_LayerRenderOptionsH）

Returns:
    int: フレームレシートハンドル（AEGP_FrameReceiptH）
         使用後はAEGP_CheckinFrameでチェックインする必要があります

Raises:
    RuntimeError: レンダリングが失敗した場合
    ValueError: optionsHがnullの場合

Note:
    - UIスレッドでの同期呼び出しは非推奨
    - キャンセルコールバックはPythonから使用不可（nullptr固定）
)doc");

    // -----------------------------------------------------------------------
    // 3. AEGP_RenderAndCheckoutLayerFrame_Async - Stub (Python callbacks not supported)
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderAndCheckoutLayerFrame_Async",
        [](uintptr_t optionsH_ptr) -> py::tuple {
        (void)optionsH_ptr;
        throw std::runtime_error(
            "AEGP_RenderAndCheckoutLayerFrame_Async is not available from Python. "
            "This function requires C callbacks which cannot be provided from Python. "
            "Use AEGP_RenderAndCheckoutLayerFrame for synchronous rendering, or "
            "use RenderAsyncManagerSuite for async rendering in Custom UI contexts."
        );
        return py::make_tuple(0, 0);  // Never reached
    },
    py::arg("optionsH"),
    R"doc(非同期レイヤーフレームレンダリング（スタブ - Pythonから使用不可）

この関数はCコールバックを必要とするため、Pythonからは使用できません。

代替案:
    - 同期レンダリング: AEGP_RenderAndCheckoutLayerFrame
    - カスタムUI内での非同期: RenderAsyncManagerSuiteを使用

Raises:
    RuntimeError: 常にエラーをスロー
)doc");

    // -----------------------------------------------------------------------
    // 4. AEGP_CancelAsyncRequest
    // Cancel an async render request
    // -----------------------------------------------------------------------
    sdk.def("AEGP_CancelAsyncRequest",
        [](uint64_t asyncRequestId) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_AsyncRequestId reqId = static_cast<AEGP_AsyncRequestId>(asyncRequestId);

        A_Err err = suites.renderSuite->AEGP_CancelAsyncRequest(reqId);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_CancelAsyncRequest failed with error: " + std::to_string(err));
        }
    },
    py::arg("asyncRequestId"),
    R"doc(非同期レンダリングリクエストをキャンセル

AEGP_RenderAndCheckoutLayerFrame_Asyncから返されたリクエストIDを使用して
保留中の非同期レンダリングをキャンセルします。

Args:
    asyncRequestId (int): キャンセルするリクエストのID

Raises:
    RuntimeError: キャンセルが失敗した場合

Note:
    AEGP_RenderAndCheckoutLayerFrame_AsyncはPythonから使用できないため、
    この関数は主にRenderAsyncManagerSuiteと組み合わせて使用されます。
)doc");

    // -----------------------------------------------------------------------
    // 5. AEGP_CheckinFrame
    // Check in a rendered frame (release resources)
    // -----------------------------------------------------------------------
    sdk.def("AEGP_CheckinFrame",
        [](uintptr_t receiptH_ptr) {
        // Argument validation
        Validation::RequireNonNull(receiptH_ptr, "receiptH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_FrameReceiptH receiptH = reinterpret_cast<AEGP_FrameReceiptH>(receiptH_ptr);

        A_Err err = suites.renderSuite->AEGP_CheckinFrame(receiptH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_CheckinFrame failed with error: " + std::to_string(err));
        }
    },
    py::arg("receiptH"),
    R"doc(レンダリングされたフレームをチェックイン（リソース解放）

AEGP_RenderAndCheckoutFrameまたはAEGP_RenderAndCheckoutLayerFrameで
取得したフレームレシートを解放します。

Args:
    receiptH (int): フレームレシートハンドル（AEGP_FrameReceiptH）

Raises:
    RuntimeError: チェックインが失敗した場合
    ValueError: receiptHがnullの場合

Note:
    レンダリングしたフレームの使用が完了したら必ず呼び出してください。
    メモリリークを防ぐために重要です。
)doc");

    // -----------------------------------------------------------------------
    // 6. AEGP_GetReceiptWorld
    // Get the world (frame buffer) from a receipt
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetReceiptWorld",
        [](uintptr_t receiptH_ptr) -> uintptr_t {
        // Argument validation
        Validation::RequireNonNull(receiptH_ptr, "receiptH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_FrameReceiptH receiptH = reinterpret_cast<AEGP_FrameReceiptH>(receiptH_ptr);
        AEGP_WorldH worldH = nullptr;

        A_Err err = suites.renderSuite->AEGP_GetReceiptWorld(receiptH, &worldH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetReceiptWorld failed with error: " + std::to_string(err));
        }

        return reinterpret_cast<uintptr_t>(worldH);
    },
    py::arg("receiptH"),
    R"doc(レシートからワールド（フレームバッファ）を取得

フレームレシートからレンダリングされたピクセルデータを含む
AEGP_WorldHを取得します。

Args:
    receiptH (int): フレームレシートハンドル（AEGP_FrameReceiptH）

Returns:
    int: ワールドハンドル（AEGP_WorldH）
         読み取り専用で、プラグインが所有するものではありません

Raises:
    RuntimeError: 取得が失敗した場合
    ValueError: receiptHがnullの場合

Note:
    - 返されるワールドは読み取り専用です
    - ワールドの所有権はプラグインにありません
    - CheckinFrameを呼び出すとワールドも無効になります
)doc");

    // -----------------------------------------------------------------------
    // 7. AEGP_GetRenderedRegion
    // Get the rendered region of a frame
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetRenderedRegion",
        [](uintptr_t receiptH_ptr) -> py::dict {
        // Argument validation
        Validation::RequireNonNull(receiptH_ptr, "receiptH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_FrameReceiptH receiptH = reinterpret_cast<AEGP_FrameReceiptH>(receiptH_ptr);
        A_LRect rendered_region = {0, 0, 0, 0};

        A_Err err = suites.renderSuite->AEGP_GetRenderedRegion(receiptH, &rendered_region);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetRenderedRegion failed with error: " + std::to_string(err));
        }

        py::dict result;
        result["left"] = rendered_region.left;
        result["top"] = rendered_region.top;
        result["right"] = rendered_region.right;
        result["bottom"] = rendered_region.bottom;

        return result;
    },
    py::arg("receiptH"),
    R"doc(レンダリングされた領域を取得

フレームレシートから実際にレンダリングされた矩形領域を取得します。

Args:
    receiptH (int): フレームレシートハンドル（AEGP_FrameReceiptH）

Returns:
    dict: レンダリング領域
        - left (int): 左端のX座標
        - top (int): 上端のY座標
        - right (int): 右端のX座標
        - bottom (int): 下端のY座標

Raises:
    RuntimeError: 取得が失敗した場合
    ValueError: receiptHがnullの場合
)doc");

    // -----------------------------------------------------------------------
    // 8. AEGP_IsRenderedFrameSufficient
    // Check if a rendered frame meets the requirements
    // -----------------------------------------------------------------------
    sdk.def("AEGP_IsRenderedFrameSufficient",
        [](uintptr_t rendered_optionsH_ptr, uintptr_t proposed_optionsH_ptr) -> bool {
        // Argument validation
        Validation::RequireNonNull(rendered_optionsH_ptr, "rendered_optionsH");
        Validation::RequireNonNull(proposed_optionsH_ptr, "proposed_optionsH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_RenderOptionsH rendered_optionsH = reinterpret_cast<AEGP_RenderOptionsH>(rendered_optionsH_ptr);
        AEGP_RenderOptionsH proposed_optionsH = reinterpret_cast<AEGP_RenderOptionsH>(proposed_optionsH_ptr);
        A_Boolean is_sufficient = FALSE;

        A_Err err = suites.renderSuite->AEGP_IsRenderedFrameSufficient(
            rendered_optionsH,
            proposed_optionsH,
            &is_sufficient
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_IsRenderedFrameSufficient failed with error: " + std::to_string(err));
        }

        return is_sufficient != FALSE;
    },
    py::arg("rendered_optionsH"),
    py::arg("proposed_optionsH"),
    R"doc(レンダリング済みフレームが要件を満たすかチェック

既にレンダリングされたフレームの設定が、新しい要件を満たすかどうかを
確認します。

Args:
    rendered_optionsH (int): 既にレンダリングされたフレームのオプション
    proposed_optionsH (int): 提案されている新しいオプション

Returns:
    bool: True - 既存のレンダリングが十分、False - 再レンダリングが必要

Raises:
    RuntimeError: チェックが失敗した場合
    ValueError: いずれかのハンドルがnullの場合

Note:
    キャッシュの有効性を確認する際に使用します。
)doc");

    // -----------------------------------------------------------------------
    // 9. AEGP_RenderNewItemSoundData
    // Render sound data for an item
    // -----------------------------------------------------------------------
    sdk.def("AEGP_RenderNewItemSoundData",
        [](uintptr_t itemH_ptr, double start_time_seconds, double duration_seconds,
           double sample_rate, int encoding, int bytes_per_sample, int num_channels) -> uintptr_t {
        // Argument validation
        Validation::RequireNonNull(itemH_ptr, "itemH");
        Validation::RequireNonNegative(start_time_seconds, "start_time_seconds");
        Validation::RequirePositive(duration_seconds, "duration_seconds");
        Validation::RequirePositive(sample_rate, "sample_rate");
        Validation::RequireRange(encoding, 3, 5, "encoding");  // UNSIGNED_PCM(3), SIGNED_PCM(4), FLOAT(5)
        Validation::RequireRange(bytes_per_sample, 1, 4, "bytes_per_sample");
        Validation::RequireRange(num_channels, 1, 2, "num_channels");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        // Convert seconds to A_Time
        A_Time start_time = PyAE::AETypeUtils::SecondsToTime(start_time_seconds);
        A_Time duration = PyAE::AETypeUtils::SecondsToTime(duration_seconds);

        // Setup sound format
        AEGP_SoundDataFormat sound_format;
        sound_format.sample_rateF = static_cast<A_FpLong>(sample_rate);
        sound_format.encoding = static_cast<AEGP_SoundEncoding>(encoding);
        sound_format.bytes_per_sampleL = static_cast<A_long>(bytes_per_sample);
        sound_format.num_channelsL = static_cast<A_long>(num_channels);

        AEGP_SoundDataH soundDataH = nullptr;

        // cancel_functionP0 and cancel_function_refconP0 are set to nullptr
        A_Err err = suites.renderSuite->AEGP_RenderNewItemSoundData(
            itemH,
            &start_time,
            &duration,
            &sound_format,
            nullptr,  // cancel_functionP0 - not available from Python
            nullptr,  // cancel_function_refconP0
            &soundDataH
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_RenderNewItemSoundData failed with error: " + std::to_string(err));
        }

        // Note: soundDataH can be NULL if no audio
        return reinterpret_cast<uintptr_t>(soundDataH);
    },
    py::arg("itemH"),
    py::arg("start_time_seconds"),
    py::arg("duration_seconds"),
    py::arg("sample_rate"),
    py::arg("encoding"),
    py::arg("bytes_per_sample"),
    py::arg("num_channels"),
    R"doc(アイテムのサウンドデータをレンダリング

コンポジションまたはフッテージアイテムからオーディオデータを
レンダリングします。

Args:
    itemH (int): アイテムハンドル（コンポジションまたはフッテージ）
    start_time_seconds (float): 開始時間（秒）
    duration_seconds (float): 継続時間（秒）
    sample_rate (float): サンプルレート（Hz、例: 44100.0, 48000.0）
    encoding (int): エンコーディング
        - AEGP_SoundEncoding_UNSIGNED_PCM (3)
        - AEGP_SoundEncoding_SIGNED_PCM (4)
        - AEGP_SoundEncoding_FLOAT (5)
    bytes_per_sample (int): サンプルあたりのバイト数（1, 2, 4）
        FLOAT encodingの場合は無視されます
    num_channels (int): チャンネル数（1: モノラル、2: ステレオ）

Returns:
    int: サウンドデータハンドル（AEGP_SoundDataH）
         オーディオがない場合は0を返す場合があります
         使用後はAEGP_DisposeSoundDataで破棄する必要があります

Raises:
    RuntimeError: レンダリングが失敗した場合
    ValueError: 引数が無効な場合

Note:
    キャンセルコールバックはPythonから使用不可（nullptr固定）
)doc");

    // -----------------------------------------------------------------------
    // 10. AEGP_GetCurrentTimestamp
    // Get the current project timestamp
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetCurrentTimestamp",
        []() -> py::tuple {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_TimeStamp timestamp;
        memset(&timestamp, 0, sizeof(timestamp));

        A_Err err = suites.renderSuite->AEGP_GetCurrentTimestamp(&timestamp);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetCurrentTimestamp failed with error: " + std::to_string(err));
        }

        // Return as tuple of 4 characters (bytes)
        return py::make_tuple(
            static_cast<int>(timestamp.a[0]),
            static_cast<int>(timestamp.a[1]),
            static_cast<int>(timestamp.a[2]),
            static_cast<int>(timestamp.a[3])
        );
    },
    R"doc(現在のプロジェクトタイムスタンプを取得

プロジェクト内でレンダリングに影響する変更が加えられるたびに
更新されるタイムスタンプを取得します。

Returns:
    tuple: タイムスタンプ（4つの整数値のタプル）
           AEGP_HasItemChangedSinceTimestamp等で使用

Raises:
    RuntimeError: 取得が失敗した場合

Note:
    このタイムスタンプはキャッシュの有効性チェックに使用されます。
)doc");

    // -----------------------------------------------------------------------
    // 11. AEGP_HasItemChangedSinceTimestamp
    // Check if an item has changed since a timestamp
    // -----------------------------------------------------------------------
    sdk.def("AEGP_HasItemChangedSinceTimestamp",
        [](uintptr_t itemH_ptr, double start_time_seconds, double duration_seconds,
           int ts0, int ts1, int ts2, int ts3) -> bool {
        // Argument validation
        Validation::RequireNonNull(itemH_ptr, "itemH");
        Validation::RequireNonNegative(start_time_seconds, "start_time_seconds");
        Validation::RequirePositive(duration_seconds, "duration_seconds");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

        // Convert seconds to A_Time
        A_Time start_time = PyAE::AETypeUtils::SecondsToTime(start_time_seconds);
        A_Time duration = PyAE::AETypeUtils::SecondsToTime(duration_seconds);

        // Setup timestamp
        AEGP_TimeStamp timestamp;
        timestamp.a[0] = static_cast<A_char>(ts0);
        timestamp.a[1] = static_cast<A_char>(ts1);
        timestamp.a[2] = static_cast<A_char>(ts2);
        timestamp.a[3] = static_cast<A_char>(ts3);

        A_Boolean has_changed = FALSE;

        A_Err err = suites.renderSuite->AEGP_HasItemChangedSinceTimestamp(
            itemH,
            &start_time,
            &duration,
            &timestamp,
            &has_changed
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_HasItemChangedSinceTimestamp failed with error: " + std::to_string(err));
        }

        return has_changed != FALSE;
    },
    py::arg("itemH"),
    py::arg("start_time_seconds"),
    py::arg("duration_seconds"),
    py::arg("ts0"), py::arg("ts1"), py::arg("ts2"), py::arg("ts3"),
    R"doc(タイムスタンプ以降にアイテムが変更されたかチェック

指定したタイムスタンプ以降、アイテムのビデオが変更されたかどうかを
確認します。オーディオの変更は影響しません。

Args:
    itemH (int): アイテムハンドル
    start_time_seconds (float): チェック開始時間（秒）
    duration_seconds (float): チェック期間（秒）
    ts0, ts1, ts2, ts3 (int): タイムスタンプ（AEGP_GetCurrentTimestampで取得）

Returns:
    bool: True - 変更あり、False - 変更なし

Raises:
    RuntimeError: チェックが失敗した場合
    ValueError: 引数が無効な場合

Note:
    キャッシュの有効性を確認する際に使用します。
)doc");

    // -----------------------------------------------------------------------
    // 12. AEGP_IsItemWorthwhileToRender
    // Check if an item is worthwhile to render
    // -----------------------------------------------------------------------
    sdk.def("AEGP_IsItemWorthwhileToRender",
        [](uintptr_t roH_ptr, int ts0, int ts1, int ts2, int ts3) -> bool {
        // Argument validation
        Validation::RequireNonNull(roH_ptr, "roH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }

        AEGP_RenderOptionsH roH = reinterpret_cast<AEGP_RenderOptionsH>(roH_ptr);

        // Setup timestamp
        AEGP_TimeStamp timestamp;
        timestamp.a[0] = static_cast<A_char>(ts0);
        timestamp.a[1] = static_cast<A_char>(ts1);
        timestamp.a[2] = static_cast<A_char>(ts2);
        timestamp.a[3] = static_cast<A_char>(ts3);

        A_Boolean is_worthwhile = FALSE;

        A_Err err = suites.renderSuite->AEGP_IsItemWorthwhileToRender(
            roH,
            &timestamp,
            &is_worthwhile
        );

        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_IsItemWorthwhileToRender failed with error: " + std::to_string(err));
        }

        return is_worthwhile != FALSE;
    },
    py::arg("roH"),
    py::arg("ts0"), py::arg("ts1"), py::arg("ts2"), py::arg("ts3"),
    R"doc(アイテムがレンダリングする価値があるかチェック

レンダリングを開始する前と、完了してチェックインする前の両方で
呼び出すことが推奨されます。

Args:
    roH (int): レンダリングオプションハンドル（AEGP_RenderOptionsH）
    ts0, ts1, ts2, ts3 (int): タイムスタンプ（AEGP_GetCurrentTimestampで取得）

Returns:
    bool: True - レンダリングする価値あり、False - スキップ推奨

Raises:
    RuntimeError: チェックが失敗した場合
    ValueError: roHがnullの場合

Note:
    - レンダリング開始前に呼び出す
    - レンダリング完了後、AEGP_CheckinRenderedFrame前にも呼び出す
    - AEGP_HasItemChangedSinceTimestampと併用推奨
)doc");

    // -----------------------------------------------------------------------
    // 13. AEGP_CheckinRenderedFrame - Stub (complex AEGP_PlatformWorldH handling)
    // -----------------------------------------------------------------------
    sdk.def("AEGP_CheckinRenderedFrame",
        [](uintptr_t roH_ptr, int ts0, int ts1, int ts2, int ts3,
           unsigned long ticks_to_render, uintptr_t imageH_ptr) {
        (void)roH_ptr;
        (void)ts0; (void)ts1; (void)ts2; (void)ts3;
        (void)ticks_to_render;
        (void)imageH_ptr;
        throw std::runtime_error(
            "AEGP_CheckinRenderedFrame is not fully implemented. "
            "This function requires AEGP_PlatformWorldH which has complex "
            "platform-specific handling. Use the standard render pipeline "
            "(AEGP_RenderAndCheckoutFrame + AEGP_CheckinFrame) instead."
        );
    },
    py::arg("roH"),
    py::arg("ts0"), py::arg("ts1"), py::arg("ts2"), py::arg("ts3"),
    py::arg("ticks_to_render"),
    py::arg("imageH"),
    R"doc(レンダリングされたフレームをチェックイン（スタブ - 完全未実装）

この関数はAEGP_PlatformWorldHを必要とし、プラットフォーム固有の
複雑な処理が必要なため、完全には実装されていません。

代替案:
    標準のレンダリングパイプラインを使用:
    AEGP_RenderAndCheckoutFrame + AEGP_CheckinFrame

Raises:
    RuntimeError: 常にエラーをスロー
)doc");

    // -----------------------------------------------------------------------
    // 14. AEGP_GetReceiptGuid
    // Get the GUID of a rendered frame receipt
    // -----------------------------------------------------------------------
    sdk.def("AEGP_GetReceiptGuid",
        [](uintptr_t receiptH_ptr) -> std::string {
        // Argument validation
        Validation::RequireNonNull(receiptH_ptr, "receiptH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.renderSuite) {
            throw std::runtime_error("Render Suite not available");
        }
        if (!suites.memorySuite) {
            throw std::runtime_error("Memory Suite not available");
        }

        AEGP_FrameReceiptH receiptH = reinterpret_cast<AEGP_FrameReceiptH>(receiptH_ptr);
        AEGP_MemHandle guidMH = nullptr;

        A_Err err = suites.renderSuite->AEGP_GetReceiptGuid(receiptH, &guidMH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_GetReceiptGuid failed with error: " + std::to_string(err));
        }

        if (!guidMH) {
            return "";
        }

        // Use RAII wrappers for automatic memory management
        ScopedMemHandle scopedGuid(state.GetPluginID(), suites.memorySuite, guidMH);
        ScopedMemLock lock(suites.memorySuite, guidMH);

        std::string result;
        const char* guidData = lock.As<const char>();
        if (guidData) {
            result = std::string(guidData);
        }

        return result;
    },
    py::arg("receiptH"),
    R"doc(フレームレシートのGUIDを取得

レンダリングされたフレームを一意に識別するGUID文字列を取得します。

Args:
    receiptH (int): フレームレシートハンドル（AEGP_FrameReceiptH）

Returns:
    str: GUID文字列、取得できない場合は空文字列

Raises:
    RuntimeError: 取得が失敗した場合
    ValueError: receiptHがnullの場合

Note:
    GUIDはフレームのキャッシュ管理やデバッグに使用できます。
)doc");

}

} // namespace SDK
} // namespace PyAE
