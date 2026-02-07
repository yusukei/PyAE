非同期レンダリング
==================

.. currentmodule:: ae.async_render

``ae.async_render`` モジュールは、カスタムUIを持つエフェクトプラグインで
非ブロッキングのフレームレンダリングを行うためのAPIを提供します。

概要
----

このモジュールは、エフェクトプラグインの描画イベント（``PF_Event_DRAW``）内で
フレームを非同期にレンダリングするための低レベルAPIです。

**主な機能**:

- アイテム（コンポジション）フレームの非同期レンダリング
- レイヤーフレームの非同期レンダリング
- 重複リクエストの自動キャンセル

.. important::
   このAPIは **C++ エフェクトプラグイン** からのみ使用可能です。

   - ``PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER`` フラグの設定が必須
   - ``PF_Event_DRAW`` コンテキスト内でのみ呼び出し可能
   - ``async_manager`` パラメータは C++ プラグインから渡す必要がある

   通常のPythonスクリプトでの自動化には、同期レンダリングAPIを使用してください:

   - ``ae.sdk.AEGP_RenderAndCheckoutFrame()``
   - ``ae.sdk.AEGP_RenderAndCheckoutLayerFrame()``

基本的な使い方
--------------

C++ エフェクトプラグインからの呼び出し
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   // C++ エフェクトプラグインの PF_Event_DRAW ハンドラ内

   #include <pybind11/pybind11.h>
   namespace py = pybind11;

   // async_manager を取得
   PF_AsyncManagerP async_mgr = extra->u.draw.async_managerP;

   // Python モジュールをインポート
   py::module_ ae = py::module_::import("ae");

   // アイテムフレームの非同期レンダリング
   auto receipt = ae.attr("async_render").attr("render_item_frame")(
       reinterpret_cast<uintptr_t>(async_mgr),
       comp_id,    // purpose_id
       reinterpret_cast<uintptr_t>(render_options)
   );

   // レイヤーフレームの非同期レンダリング
   auto receipt2 = ae.attr("async_render").attr("render_layer_frame")(
       reinterpret_cast<uintptr_t>(async_mgr),
       layer_id,   // purpose_id
       reinterpret_cast<uintptr_t>(layer_render_options)
   );

API リファレンス
----------------

レンダリング関数
~~~~~~~~~~~~~~~~

.. function:: render_item_frame(async_manager: int, purpose_id: int, render_options: int) -> int

   アイテム（コンポジション）フレームを非同期にレンダリングします。

   :param async_manager: ``PF_AsyncManagerP`` ポインタ（``PF_Event_DRAW`` の extra から取得）
   :type async_manager: int
   :param purpose_id: このレンダリングリクエストの一意のID。同じIDで新しいリクエストを行うと、
                      古いリクエストは自動的にキャンセルされます。
   :type purpose_id: int
   :param render_options: ``AEGP_RenderOptionsH`` ハンドル
   :type render_options: int
   :return: ``AEGP_FrameReceiptH`` ハンドル
   :rtype: int

   :raises RuntimeError: RenderAsyncManager Suite が利用できない場合
   :raises RuntimeError: async_manager が null の場合
   :raises RuntimeError: render_options が null の場合
   :raises RuntimeError: レンダリングに失敗した場合

   .. note::
      返される receipt は、レンダリング完了直後はピクセルデータを含まない場合があります。
      ``AEGP_GetReceiptWorld()`` を使用してピクセルデータを取得してください。

.. function:: render_layer_frame(async_manager: int, purpose_id: int, layer_render_options: int) -> int

   レイヤーフレームを非同期にレンダリングします。

   :param async_manager: ``PF_AsyncManagerP`` ポインタ（``PF_Event_DRAW`` の extra から取得）
   :type async_manager: int
   :param purpose_id: このレンダリングリクエストの一意のID。同じIDで新しいリクエストを行うと、
                      古いリクエストは自動的にキャンセルされます。
   :type purpose_id: int
   :param layer_render_options: ``AEGP_LayerRenderOptionsH`` ハンドル
   :type layer_render_options: int
   :return: ``AEGP_FrameReceiptH`` ハンドル
   :rtype: int

   :raises RuntimeError: RenderAsyncManager Suite が利用できない場合
   :raises RuntimeError: async_manager が null の場合
   :raises RuntimeError: layer_render_options が null の場合
   :raises RuntimeError: レンダリングに失敗した場合

Purpose ID について
-------------------

``purpose_id`` パラメータは、レンダリングリクエストを識別するために使用されます。

**目的**:

- 同じ ``purpose_id`` で新しいリクエストを行うと、古いリクエストは自動的にキャンセルされます
- ユーザーがタイムラインを素早くスクラブしたときに、レンダリングリクエストが積み上がるのを防ぎます

**推奨される使用パターン**:

.. code-block:: cpp

   // コンポジションレベルのレンダリング
   auto receipt = ae.attr("async_render").attr("render_item_frame")(
       async_mgr,
       comp_id,  // コンポジションIDを purpose_id として使用
       render_options
   );

   // レイヤーレベルのレンダリング
   auto receipt2 = ae.attr("async_render").attr("render_layer_frame")(
       async_mgr,
       layer_id,  // レイヤーIDを purpose_id として使用
       layer_render_options
   );

実用例
------

カスタムUIエフェクトでのプレビューレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   // CustomUIEffect.cpp - PF_Event_DRAW ハンドラ

   #include <pybind11/pybind11.h>
   #include <pybind11/embed.h>
   namespace py = pybind11;

   PF_Err HandleDrawEvent(
       PF_InData* in_data,
       PF_OutData* out_data,
       PF_ParamDef* params[],
       PF_LayerDef* output,
       PF_EventExtra* extra)
   {
       PF_Err err = PF_Err_NONE;

       // async_manager を取得
       PF_AsyncManagerP async_mgr = extra->u.draw.async_managerP;

       if (async_mgr) {
           try {
               // Python インタープリタを初期化（まだの場合）
               if (!Py_IsInitialized()) {
                   py::scoped_interpreter guard{};
               }

               // レンダーオプションを作成
               AEGP_RenderOptionsH render_options = nullptr;
               // ... render_options の設定 ...

               // Python で非同期レンダリング
               py::module_ ae = py::module_::import("ae");
               py::object receipt = ae.attr("async_render").attr("render_item_frame")(
                   reinterpret_cast<uintptr_t>(async_mgr),
                   static_cast<uint64_t>(in_data->effect_ref),  // purpose_id
                   reinterpret_cast<uintptr_t>(render_options)
               );

               // receipt からピクセルデータを取得（必要に応じて）
               // ...

           } catch (const py::error_already_set& e) {
               // Python エラーを処理
               err = PF_Err_INTERNAL_STRUCT_DAMAGED;
           }
       }

       return err;
   }

同期レンダリング（通常のPythonスクリプト用）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

通常のPythonスクリプトでは、同期APIを使用してください:

.. code-block:: python

   import ae

   # コンポジションのフレームをレンダリング
   comp = ae.Comp.get_active()
   if comp:
       # レンダーオプションを作成
       render_opts_h = ae.sdk.AEGP_NewFromItem(comp.item_handle)

       # 時刻を設定（例: 1秒目）
       ae.sdk.AEGP_SetTime(render_opts_h, 1.0)

       # フレームをレンダリング
       receipt_h = ae.sdk.AEGP_RenderAndCheckoutFrame(render_opts_h)

       # ピクセルデータを取得
       world_h = ae.sdk.AEGP_GetReceiptWorld(receipt_h)

       # 使用後は解放
       ae.sdk.AEGP_CheckinFrame(receipt_h)
       ae.sdk.AEGP_Dispose(render_opts_h)

必要条件
--------

このAPIを使用するには、以下の条件を満たす必要があります:

1. **C++ エフェクトプラグイン** - ``PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER`` フラグを設定

   .. code-block:: cpp

      // GlobalSetup 内
      out_data->out_flags2 |= PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER;

2. **PF_Event_DRAW コンテキスト** - これらの関数は ``PF_Event_DRAW`` イベント内でのみ呼び出し可能

3. **async_manager ポインタ** - ``PF_Event_DRAW`` の extra データから取得

   .. code-block:: cpp

      PF_AsyncManagerP async_mgr = extra->u.draw.async_managerP;

制限事項
--------

.. warning::
   - ``PF_Event_DRAW`` コンテキスト外では使用できません
   - C++ エフェクトプラグインから ``async_manager`` ポインタを渡す必要があります
   - 返される receipt はすぐにはピクセルデータを含まない場合があります
   - 通常の Python スクリプトからは使用できません

関連項目
--------

- :doc:`render_queue` - レンダーキュー操作
- :doc:`render_monitor` - レンダリング監視
- :doc:`/api/low-level/AEGP_RenderAsyncManagerSuite1` - 低レベルAPI
- :doc:`/api/low-level/AEGP_RenderOptionsSuite4` - レンダーオプションAPI
- :doc:`/api/low-level/AEGP_LayerRenderOptionsSuite2` - レイヤーレンダーオプションAPI
