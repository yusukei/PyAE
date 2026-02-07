AEGP_RenderAsyncManagerSuite1
=============================

.. currentmodule:: ae.sdk

AEGP_RenderAsyncManagerSuite1は、カスタムUIを持つエフェクトプラグインで非同期レンダリングを管理するためのSDK APIです。

概要
----

**実装状況**: 2/2関数実装 ✅

AEGP_RenderAsyncManagerSuite1は以下の機能を提供します:

- 非同期レンダリングリクエストの自動管理
- 古いレンダリングリクエストの自動キャンセル
- アイテムフレーム・レイヤーフレームの非同期レンダリング
- カスタムUIでのレスポンシブな描画

基本概念
--------

非同期レンダリングマネージャ (PF_AsyncManagerP)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

カスタムUIを持つエフェクトプラグイン（``PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER`` フラグ設定）で、``PF_Event_DRAW`` イベント時に提供される非同期レンダリングマネージャです。

.. important::
   - AsyncManagerは ``PF_Event_DRAW`` イベントでのみ提供されます
   - 他のイベントタイプでは使用できません
   - ``PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER`` フラグの設定が必須です

Purpose ID
~~~~~~~~~~

``purpose_id`` は、レンダリングリクエストを識別するための一意の識別子です。

**重要な動作**:

- 同じ ``purpose_id`` で新しいリクエストが発行されると、古いリクエストは **自動的にキャンセル** されます
- これにより、複数のレンダリングリクエストが溜まることを防ぎます
- カスタムUIの描画では、最新のリクエストのみが処理されます

**使用例**:

.. code-block:: python

   # コンポジションIDをpurpose_idとして使用
   purpose_id = comp_id

   # 新しいリクエストが発行されると、同じpurpose_idの古いリクエストはキャンセルされる
   receipt = ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
       async_manager, purpose_id, render_options
   )

フレームレシート (AEGP_FrameReceiptH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダリング結果を識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. note::
   - レシートが返されても、ピクセルデータ（world）が存在しない場合があります
   - ピクセルデータは ``AEGP_FrameSuite`` 経由でアクセスします
   - レシートはレンダリングが完了するまで有効です

レンダリングオプション
~~~~~~~~~~~~~~~~~~~~~~

レンダリング対象を指定するためのハンドル:

- **AEGP_RenderOptionsH**: アイテムフレームのレンダリングオプション
- **AEGP_LayerRenderOptionsH**: レイヤーフレームのレンダリングオプション

これらは ``AEGP_RenderOptionsSuite4`` で作成・設定します。

API リファレンス
----------------

アイテムフレームの非同期レンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CheckoutOrRender_ItemFrame_AsyncManager(async_managerP: int, purpose_id: int, roH: int) -> int

   非同期レンダリングマネージャを使用してアイテムフレーム（コンポジションフレーム）をチェックアウトまたはレンダリングします。

   :param async_managerP: 非同期マネージャのポインタ（``PF_Event_DRAW`` で提供される ``PF_AsyncManagerP``）
   :type async_managerP: int
   :param purpose_id: リクエストを識別するための一意ID（同じ用途のリクエストをキャンセルするヒントとして使用）
   :type purpose_id: int
   :param roH: レンダリングするアイテムフレームの記述（``AEGP_RenderOptionsH``）
   :type roH: int
   :return: レンダリングされたフレームのレシート（``AEGP_FrameReceiptH``）。成功した場合でもピクセルデータがない場合があります（worldなし）
   :rtype: int
   :raises RuntimeError: レンダリングリクエストが失敗した場合
   :raises ValueError: 引数が無効な場合（``None`` や ``0`` が渡された場合）

   .. note::
      - このAPIは主に ``PF_Event_DRAW`` でのカスタムUI用（``PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER``）
      - ``purpose_id`` は、古いリクエストを自動キャンセルするための識別子です
      - フレームレシートは ``AEGP_FrameSuite`` 経由でアクセスする必要があります

   .. warning::
      - 同じ ``purpose_id`` で新しいリクエストが発行されると、古いリクエストは自動的にキャンセルされます
      - レシートが返されても、ピクセルデータが存在しない場合があります

   **例**: コンポジションの現在フレームを非同期レンダリング

   .. code-block:: python

      import ae

      def draw_custom_ui(async_manager, comp_id, current_time):
          """カスタムUIでコンポジションのフレームを描画"""
          # レンダリングオプションを作成
          roH = ae.sdk.AEGP_NewFromItem(
              ae.sdk.AEGP_GetPluginID(),
              comp_id
          )

          # レンダリング時刻を設定
          ae.sdk.AEGP_SetRenderOptionsTime(roH, current_time)

          # 非同期レンダリングをリクエスト
          # purpose_idにcomp_idを使用（コンポジションごとに1つのリクエストのみ）
          receipt = ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
              async_manager,
              comp_id,  # purpose_id
              roH
          )

          # レシートからピクセルデータを取得
          try:
              world = ae.sdk.AEGP_GetReceiptWorld(receipt)
              if world:
                  # worldを使ってカスタムUIを描画
                  draw_world_to_ui(world)
          finally:
              # レシートを必ず解放
              ae.sdk.AEGP_CheckinReceipt(receipt)

レイヤーフレームの非同期レンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CheckoutOrRender_LayerFrame_AsyncManager(async_managerP: int, purpose_id: int, lroH: int) -> int

   非同期レンダリングマネージャを使用してレイヤーフレームをチェックアウトまたはレンダリングします。

   :param async_managerP: 非同期マネージャのポインタ（``PF_Event_DRAW`` で提供される ``PF_AsyncManagerP``）
   :type async_managerP: int
   :param purpose_id: リクエストを識別するための一意ID（同じ用途のリクエストをキャンセルするヒントとして使用）
   :type purpose_id: int
   :param lroH: レンダリングするレイヤーフレームの記述（``AEGP_LayerRenderOptionsH``）
   :type lroH: int
   :return: レンダリングされたフレームのレシート（``AEGP_FrameReceiptH``）。成功した場合でもピクセルデータがない場合があります（worldなし）
   :rtype: int
   :raises RuntimeError: レンダリングリクエストが失敗した場合
   :raises ValueError: 引数が無効な場合（``None`` や ``0`` が渡された場合）

   .. note::
      - このAPIは主に ``PF_Event_DRAW`` でのカスタムUI用（``PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER``）
      - ``purpose_id`` は、古いリクエストを自動キャンセルするための識別子です
      - フレームレシートは ``AEGP_FrameSuite`` 経由でアクセスする必要があります

   .. warning::
      - 同じ ``purpose_id`` で新しいリクエストが発行されると、古いリクエストは自動的にキャンセルされます
      - レシートが返されても、ピクセルデータが存在しない場合があります

   **例**: レイヤーの現在フレームを非同期レンダリング

   .. code-block:: python

      import ae

      def draw_layer_preview(async_manager, layer_id, current_time):
          """カスタムUIでレイヤーのフレームをプレビュー"""
          # レイヤーレンダリングオプションを作成
          lroH = ae.sdk.AEGP_NewFromLayer(
              ae.sdk.AEGP_GetPluginID(),
              layer_id
          )

          # レンダリング時刻を設定
          ae.sdk.AEGP_SetLayerRenderOptionsTime(lroH, current_time)

          # 非同期レンダリングをリクエスト
          # purpose_idにlayer_idを使用（レイヤーごとに1つのリクエストのみ）
          receipt = ae.sdk.AEGP_CheckoutOrRender_LayerFrame_AsyncManager(
              async_manager,
              layer_id,  # purpose_id
              lroH
          )

          # レシートからピクセルデータを取得
          try:
              world = ae.sdk.AEGP_GetReceiptWorld(receipt)
              if world:
                  # worldを使ってカスタムUIを描画
                  draw_layer_world_to_ui(world)
          finally:
              # レシートを必ず解放
              ae.sdk.AEGP_CheckinReceipt(receipt)

使用例
------

基本的な非同期レンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # PF_Event_DRAWイベントハンドラ（C++側で実装）
   # このサンプルはPythonでの使用イメージを示しています

   def on_draw_event(async_manager, comp_id, current_time):
       """カスタムUIの描画イベント"""
       # レンダリングオプションを作成
       roH = ae.sdk.AEGP_NewFromItem(
           ae.sdk.AEGP_GetPluginID(),
           comp_id
       )

       # レンダリング時刻を設定
       ae.sdk.AEGP_SetRenderOptionsTime(roH, current_time)

       # 非同期レンダリングをリクエスト
       receipt = ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
           async_manager,
           comp_id,  # purpose_id
           roH
       )

       # レシートからピクセルデータを取得
       world = ae.sdk.AEGP_GetReceiptWorld(receipt)

       if world:
           # worldを使って描画
           width, height = ae.sdk.AEGP_GetWorldDimensions(world)
           print(f"レンダリング完了: {width}x{height}")
       else:
           print("レンダリング中または失敗")

       # レシートを解放
       ae.sdk.AEGP_CheckinReceipt(receipt)

複数レイヤーの非同期レンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_multiple_layers(async_manager, layer_ids, current_time):
       """複数レイヤーを非同期レンダリング"""
       receipts = []

       for layer_id in layer_ids:
           # レイヤーレンダリングオプションを作成
           lroH = ae.sdk.AEGP_NewFromLayer(
               ae.sdk.AEGP_GetPluginID(),
               layer_id
           )

           # レンダリング時刻を設定
           ae.sdk.AEGP_SetLayerRenderOptionsTime(lroH, current_time)

           # 非同期レンダリングをリクエスト
           # purpose_idにlayer_idを使用（レイヤーごとに独立したリクエスト）
           receipt = ae.sdk.AEGP_CheckoutOrRender_LayerFrame_AsyncManager(
               async_manager,
               layer_id,  # purpose_id
               lroH
           )

           receipts.append((layer_id, receipt))

       # すべてのレシートを処理
       for layer_id, receipt in receipts:
           try:
               world = ae.sdk.AEGP_GetReceiptWorld(receipt)
               if world:
                   print(f"レイヤー {layer_id} のレンダリング完了")
               else:
                   print(f"レイヤー {layer_id} のレンダリング待機中")
           finally:
               # レシートを必ず解放
               ae.sdk.AEGP_CheckinReceipt(receipt)

purpose_idの適切な管理
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   class RenderRequestManager:
       """レンダリングリクエストを管理するクラス"""

       def __init__(self):
           self.purpose_id_counter = 0
           self.active_requests = {}

       def request_render(self, async_manager, comp_id, current_time):
           """コンポジションのレンダリングをリクエスト"""
           # コンポジションごとに一意のpurpose_idを使用
           purpose_id = comp_id

           # レンダリングオプションを作成
           roH = ae.sdk.AEGP_NewFromItem(
               ae.sdk.AEGP_GetPluginID(),
               comp_id
           )
           ae.sdk.AEGP_SetRenderOptionsTime(roH, current_time)

           # 古いリクエストがある場合は自動的にキャンセルされる
           receipt = ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
               async_manager,
               purpose_id,
               roH
           )

           # 古いレシートがあれば解放
           if comp_id in self.active_requests:
               old_receipt = self.active_requests[comp_id]
               ae.sdk.AEGP_CheckinReceipt(old_receipt)

           # 新しいレシートを保存
           self.active_requests[comp_id] = receipt

           return receipt

       def cleanup(self):
           """すべてのレシートを解放"""
           for receipt in self.active_requests.values():
               ae.sdk.AEGP_CheckinReceipt(receipt)
           self.active_requests.clear()

   # 使用例
   manager = RenderRequestManager()

   # 描画イベント1回目
   receipt1 = manager.request_render(async_manager, comp_id, time1)

   # 描画イベント2回目（古いリクエストは自動キャンセルされる）
   receipt2 = manager.request_render(async_manager, comp_id, time2)

   # クリーンアップ
   manager.cleanup()

エラーハンドリング
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def safe_async_render(async_manager, comp_id, current_time):
       """エラーハンドリングを含む安全な非同期レンダリング"""
       receipt = None
       roH = None

       try:
           # レンダリングオプションを作成
           roH = ae.sdk.AEGP_NewFromItem(
               ae.sdk.AEGP_GetPluginID(),
               comp_id
           )

           # レンダリング時刻を設定
           ae.sdk.AEGP_SetRenderOptionsTime(roH, current_time)

           # 非同期レンダリングをリクエスト
           receipt = ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
               async_manager,
               comp_id,
               roH
           )

           # レシートからピクセルデータを取得
           world = ae.sdk.AEGP_GetReceiptWorld(receipt)

           if world:
               # レンダリング成功
               width, height = ae.sdk.AEGP_GetWorldDimensions(world)
               print(f"レンダリング成功: {width}x{height}")
               return world
           else:
               # レンダリング待機中
               print("レンダリング待機中")
               return None

       except ValueError as e:
           print(f"引数エラー: {e}")
           return None

       except RuntimeError as e:
           print(f"レンダリングエラー: {e}")
           return None

       finally:
           # レシートを必ず解放
           if receipt:
               ae.sdk.AEGP_CheckinReceipt(receipt)

           # レンダリングオプションを解放（必要に応じて）
           if roH:
               ae.sdk.AEGP_DisposeRenderOptions(roH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER フラグが必須**

   AsyncManagerを使用するには、エフェクトプラグインで ``PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER`` フラグを設定する必要があります。

2. **PF_Event_DRAW イベントでのみ使用可能**

   AsyncManagerは ``PF_Event_DRAW`` イベントでのみ提供されます。他のイベントでは使用できません。

3. **purpose_id による自動キャンセル**

   同じ ``purpose_id`` で新しいリクエストが発行されると、古いリクエストは自動的にキャンセルされます。
   これにより、リクエストが溜まることを防ぎます。

4. **レシートは必ず解放**

   ``AEGP_FrameReceiptH`` は使用後に必ず ``AEGP_CheckinReceipt()`` で解放してください。

5. **ピクセルデータの存在確認**

   レシートが返されても、ピクセルデータ（world）が存在しない場合があります。
   必ず ``AEGP_GetReceiptWorld()`` で確認してください。

6. **スレッドセーフティ**

   このAPIはメインスレッドでのみ使用してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

purpose_id の適切な設計
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 良い例: コンポジションIDをpurpose_idとして使用
   purpose_id = comp_id  # コンポジションごとに1つのリクエスト

   # 良い例: レイヤーIDをpurpose_idとして使用
   purpose_id = layer_id  # レイヤーごとに1つのリクエスト

   # 悪い例: すべてのリクエストで同じpurpose_id
   purpose_id = 0  # 複数のリクエストが互いにキャンセルしてしまう

レシートの適切な管理
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   receipt = None
   try:
       receipt = ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
           async_manager, purpose_id, roH
       )
       # レシートを使用
   finally:
       if receipt:
           ae.sdk.AEGP_CheckinReceipt(receipt)

ピクセルデータの存在確認
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   receipt = ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
       async_manager, purpose_id, roH
   )

   world = ae.sdk.AEGP_GetReceiptWorld(receipt)
   if world:
       # worldが存在する場合のみ処理
       process_world(world)
   else:
       # レンダリング待機中または失敗
       print("レンダリング待機中")

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       receipt = ae.sdk.AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
           async_manager, purpose_id, roH
       )
   except ValueError as e:
       print(f"引数が無効です: {e}")
   except RuntimeError as e:
       print(f"レンダリングに失敗しました: {e}")

パフォーマンス最適化
~~~~~~~~~~~~~~~~~~~~

1. **purpose_id を適切に設計**

   不要なリクエストのキャンセルを避けるため、purpose_id を適切に設計してください。

2. **レンダリングオプションの再利用**

   可能な場合は、レンダリングオプションを再利用してオーバーヘッドを削減してください。

3. **レシートのキャッシュ**

   頻繁にアクセスするレシートはキャッシュして、再レンダリングを避けてください。

4. **不要なレンダリングの回避**

   描画が必要な場合のみレンダリングをリクエストしてください。

制限事項
~~~~~~~~

1. **カスタムUIプラグイン専用**

   このAPIは ``PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER`` フラグを持つエフェクトプラグイン専用です。

2. **PF_Event_DRAW イベント専用**

   AsyncManagerは ``PF_Event_DRAW`` イベントでのみ提供されます。

3. **メインスレッド専用**

   このAPIはメインスレッドでのみ使用してください。

4. **レンダリング結果の保証なし**

   レシートが返されても、ピクセルデータが存在する保証はありません。

関連項目
--------

- ``AEGP_RenderOptionsSuite4`` - レンダリングオプションの作成・設定
- ``AEGP_FrameSuite`` - フレームレシートの管理、ピクセルデータへのアクセス
- ``AEGP_WorldSuite3`` - ピクセルデータ（world）の操作
- ``AEGP_RenderSuite5`` - 同期レンダリング（通常のレンダリング）
- :doc:`index` - 低レベルAPI概要
