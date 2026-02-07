AEGP_RenderSuite5
=================

.. currentmodule:: ae.sdk

AEGP_RenderSuite5は、After EffectsのフレームとオーディオをレンダリングするためのSDK APIです。

概要
----

**実装状況**: 14/14関数実装（2関数はスタブ） ✅

AEGP_RenderSuite5は以下の機能を提供します:

- フレームの同期レンダリング
- レイヤーフレームの同期レンダリング
- サウンドデータのレンダリング
- レンダリング結果の取得とチェックイン
- タイムスタンプベースのキャッシュ管理
- レンダリング済みフレームの検証

基本概念
--------

レンダリングワークフロー
~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのレンダリングは「チェックアウト/チェックイン」モデルを使用します:

1. **レンダリングオプションの作成** - ``AEGP_NewFromItem()`` でレンダリング設定を構成
2. **フレームのチェックアウト** - ``AEGP_RenderAndCheckoutFrame()`` でレンダリング実行
3. **フレームデータの取得** - ``AEGP_GetReceiptWorld()`` でピクセルデータを取得
4. **フレームのチェックイン** - ``AEGP_CheckinFrame()`` でリソースを解放

.. important::
   - すべてのチェックアウトしたフレームは必ずチェックインする必要があります
   - チェックインを忘れるとメモリリークが発生します
   - ``try-finally`` ブロックを使用してチェックインを保証してください

フレームレシート (AEGP_FrameReceiptH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

フレームレシートは、レンダリングされたフレームへの参照です。PyAEでは整数値 (``int``) として扱われます。

.. list-table:: フレームレシートから取得できる情報
   :header-rows: 1

   * - 関数
     - 取得内容
   * - ``AEGP_GetReceiptWorld()``
     - ピクセルデータ（AEGP_WorldH）
   * - ``AEGP_GetRenderedRegion()``
     - レンダリングされた矩形領域
   * - ``AEGP_GetReceiptGuid()``
     - フレームの一意識別子（GUID）

レンダリングタイプ
~~~~~~~~~~~~~~~~~~

AEGP_RenderSuite5は2種類のレンダリングをサポートします:

.. list-table::
   :header-rows: 1

   * - レンダリングタイプ
     - 関数
     - 用途
   * - コンポジションレンダリング
     - ``AEGP_RenderAndCheckoutFrame``
     - コンポジション全体をレンダリング
   * - レイヤーレンダリング
     - ``AEGP_RenderAndCheckoutLayerFrame``
     - 個別レイヤーをレンダリング

タイムスタンプとキャッシュ管理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

タイムスタンプは、プロジェクト内の変更を追跡するための仕組みです。

.. code-block:: python

   # タイムスタンプを取得（4つの整数値のタプル）
   ts = ae.sdk.AEGP_GetCurrentTimestamp()
   # ts = (ts0, ts1, ts2, ts3)

   # 変更をチェック
   has_changed = ae.sdk.AEGP_HasItemChangedSinceTimestamp(
       itemH, 0.0, 10.0, ts[0], ts[1], ts[2], ts[3])

.. tip::
   タイムスタンプを使用して、キャッシュされたレンダリング結果の有効性を確認できます。

サウンドエンコーディング
~~~~~~~~~~~~~~~~~~~~~~~~

``AEGP_RenderNewItemSoundData()`` でサポートされるエンコーディング:

.. list-table::
   :header-rows: 1

   * - エンコーディング
     - 値
     - 説明
   * - ``AEGP_SoundEncoding_UNSIGNED_PCM``
     - 3
     - 符号なしPCM
   * - ``AEGP_SoundEncoding_SIGNED_PCM``
     - 4
     - 符号付きPCM
   * - ``AEGP_SoundEncoding_FLOAT``
     - 5
     - 浮動小数点

Pythonからの制限事項
~~~~~~~~~~~~~~~~~~~~

以下の機能はCコールバックを必要とするため、Pythonから使用できません:

- ``AEGP_RenderAndCheckoutLayerFrame_Async`` - 非同期レンダリング（コールバック必須）
- ``AEGP_CheckinRenderedFrame`` - プラットフォームワールドの複雑な処理

.. note::
   非同期レンダリングが必要な場合は、カスタムUI内で ``RenderAsyncManagerSuite`` を使用してください。

API リファレンス
----------------

フレームレンダリング
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RenderAndCheckoutFrame(optionsH: int) -> int

   コンポジションフレームを同期的にレンダリングしてチェックアウトします。

   :param optionsH: レンダリングオプションハンドル（``AEGP_NewFromItem()`` で作成）
   :type optionsH: int
   :return: フレームレシートハンドル（AEGP_FrameReceiptH）
   :rtype: int
   :raises ValueError: optionsHがnullの場合
   :raises RuntimeError: レンダリングが失敗した場合

   .. warning::
      UIスレッドでの同期呼び出しは非推奨です。レンダリングが完了するまでUIがブロックされます。

   .. note::
      - キャンセルコールバックはPythonから使用不可（内部でnullptr固定）
      - 使用後は必ず ``AEGP_CheckinFrame()`` でチェックインしてください

   **例**:

   .. code-block:: python

      import ae

      # レンダリングオプションを作成
      plugin_id = ae.sdk.AEGP_GetPluginID()
      projH = ae.sdk.AEGP_GetProjectByIndex(0)
      itemH = ae.sdk.AEGP_GetProjectRootFolder(projH)  # 例: 最初のアイテム
      optionsH = ae.sdk.AEGP_NewFromItem(plugin_id, itemH)

      # レンダリング時間を設定
      ae.sdk.AEGP_SetTime(optionsH, 0.0)

      try:
          # フレームをレンダリング
          receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)

          # フレームデータを取得
          worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)

          # ワールド情報を取得
          info = ae.sdk.AEGP_GetWorldInfo(worldH)
          print(f"レンダリング完了: {info['width']}x{info['height']}")

      finally:
          # 必ずチェックイン
          ae.sdk.AEGP_CheckinFrame(receiptH)
          ae.sdk.AEGP_Dispose(optionsH)

.. function:: AEGP_RenderAndCheckoutLayerFrame(optionsH: int) -> int

   レイヤーフレームを同期的にレンダリングしてチェックアウトします。

   :param optionsH: レイヤーレンダリングオプションハンドル（``AEGP_NewFromLayer()`` で作成）
   :type optionsH: int
   :return: フレームレシートハンドル（AEGP_FrameReceiptH）
   :rtype: int
   :raises ValueError: optionsHがnullの場合
   :raises RuntimeError: レンダリングが失敗した場合

   .. warning::
      UIスレッドでの同期呼び出しは非推奨です。

   .. note::
      - キャンセルコールバックはPythonから使用不可（内部でnullptr固定）
      - 使用後は必ず ``AEGP_CheckinFrame()`` でチェックインしてください

   **例**:

   .. code-block:: python

      import ae

      # レイヤーレンダリングオプションを作成
      plugin_id = ae.sdk.AEGP_GetPluginID()
      layerH = ...  # レイヤーハンドル取得
      layer_optionsH = ae.sdk.AEGP_NewFromLayer(plugin_id, layerH)

      try:
          # レイヤーフレームをレンダリング
          receiptH = ae.sdk.AEGP_RenderAndCheckoutLayerFrame(layer_optionsH)
          worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)
          # ... ピクセルデータ処理 ...
      finally:
          ae.sdk.AEGP_CheckinFrame(receiptH)
          ae.sdk.AEGP_Dispose(layer_optionsH)

.. function:: AEGP_RenderAndCheckoutLayerFrame_Async(optionsH: int) -> tuple

   **スタブ（Pythonから使用不可）** - 非同期レイヤーフレームレンダリング

   この関数はCコールバックを必要とするため、Pythonからは使用できません。

   :param optionsH: レイヤーレンダリングオプションハンドル
   :type optionsH: int
   :raises RuntimeError: 常にエラーをスロー

   **代替案**:

   - 同期レンダリング: ``AEGP_RenderAndCheckoutLayerFrame()``
   - カスタムUI内での非同期レンダリング: ``RenderAsyncManagerSuite``

.. function:: AEGP_CancelAsyncRequest(asyncRequestId: int) -> None

   非同期レンダリングリクエストをキャンセルします。

   :param asyncRequestId: キャンセルするリクエストのID
   :type asyncRequestId: int
   :raises RuntimeError: キャンセルが失敗した場合

   .. note::
      ``AEGP_RenderAndCheckoutLayerFrame_Async`` はPythonから使用できないため、
      この関数は主に ``RenderAsyncManagerSuite`` と組み合わせて使用されます。

フレームリソース管理
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CheckinFrame(receiptH: int) -> None

   レンダリングされたフレームをチェックイン（リソース解放）します。

   :param receiptH: フレームレシートハンドル（AEGP_FrameReceiptH）
   :type receiptH: int
   :raises ValueError: receiptHがnullの場合
   :raises RuntimeError: チェックインが失敗した場合

   .. important::
      レンダリングしたフレームの使用が完了したら **必ず** 呼び出してください。
      メモリリークを防ぐために重要です。

   **例**:

   .. code-block:: python

      receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)
      try:
          # フレームを使用
          worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)
          # ...
      finally:
          # 必ずチェックイン
          ae.sdk.AEGP_CheckinFrame(receiptH)

.. function:: AEGP_GetReceiptWorld(receiptH: int) -> int

   レシートからワールド（フレームバッファ）を取得します。

   :param receiptH: フレームレシートハンドル（AEGP_FrameReceiptH）
   :type receiptH: int
   :return: ワールドハンドル（AEGP_WorldH）
   :rtype: int
   :raises ValueError: receiptHがnullの場合
   :raises RuntimeError: 取得が失敗した場合

   .. note::
      - 返されるワールドは **読み取り専用** です
      - ワールドの所有権はプラグインにありません
      - ``AEGP_CheckinFrame()`` を呼び出すとワールドも無効になります

   **例**:

   .. code-block:: python

      receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)
      try:
          worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)

          # ワールド情報を取得
          info = ae.sdk.AEGP_GetWorldInfo(worldH)
          print(f"サイズ: {info['width']}x{info['height']}")
          print(f"タイプ: {info['type']}")

          # ピクセルデータにアクセス（WorldSuite経由）
          base_addr = ae.sdk.AEGP_GetBaseAddr8(worldH)
          # ...
      finally:
          ae.sdk.AEGP_CheckinFrame(receiptH)

.. function:: AEGP_GetRenderedRegion(receiptH: int) -> dict

   レンダリングされた矩形領域を取得します。

   :param receiptH: フレームレシートハンドル（AEGP_FrameReceiptH）
   :type receiptH: int
   :return: レンダリング領域を表す辞書
   :rtype: dict
   :raises ValueError: receiptHがnullの場合
   :raises RuntimeError: 取得が失敗した場合

   **戻り値の構造** (``A_LRect``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``left``
        - int
        - 左端のX座標
      * - ``top``
        - int
        - 上端のY座標
      * - ``right``
        - int
        - 右端のX座標
      * - ``bottom``
        - int
        - 下端のY座標

   **例**:

   .. code-block:: python

      receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)
      try:
          region = ae.sdk.AEGP_GetRenderedRegion(receiptH)
          width = region['right'] - region['left']
          height = region['bottom'] - region['top']
          print(f"レンダリング領域: {width}x{height}")
      finally:
          ae.sdk.AEGP_CheckinFrame(receiptH)

.. function:: AEGP_GetReceiptGuid(receiptH: int) -> str

   フレームレシートのGUIDを取得します。

   :param receiptH: フレームレシートハンドル（AEGP_FrameReceiptH）
   :type receiptH: int
   :return: GUID文字列、取得できない場合は空文字列
   :rtype: str
   :raises ValueError: receiptHがnullの場合
   :raises RuntimeError: 取得が失敗した場合

   .. tip::
      GUIDはフレームのキャッシュ管理やデバッグに使用できます。

   **例**:

   .. code-block:: python

      receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)
      try:
          guid = ae.sdk.AEGP_GetReceiptGuid(receiptH)
          print(f"フレームGUID: {guid}")
      finally:
          ae.sdk.AEGP_CheckinFrame(receiptH)

レンダリング検証
~~~~~~~~~~~~~~~~

.. function:: AEGP_IsRenderedFrameSufficient(rendered_optionsH: int, proposed_optionsH: int) -> bool

   レンダリング済みフレームが新しい要件を満たすかチェックします。

   :param rendered_optionsH: 既にレンダリングされたフレームのオプション
   :type rendered_optionsH: int
   :param proposed_optionsH: 提案されている新しいオプション
   :type proposed_optionsH: int
   :return: True - 既存のレンダリングが十分、False - 再レンダリングが必要
   :rtype: bool
   :raises ValueError: いずれかのハンドルがnullの場合
   :raises RuntimeError: チェックが失敗した場合

   .. tip::
      キャッシュの有効性を確認する際に使用します。既存のレンダリングが十分な場合、
      再レンダリングを避けることでパフォーマンスを向上できます。

   **例**:

   .. code-block:: python

      # キャッシュされたフレームのオプション
      cached_optionsH = ...

      # 新しいリクエストのオプション
      new_optionsH = ...

      # キャッシュが有効かチェック
      if ae.sdk.AEGP_IsRenderedFrameSufficient(cached_optionsH, new_optionsH):
          print("キャッシュを使用")
          # キャッシュされたフレームを使用
      else:
          print("再レンダリングが必要")
          # 新しくレンダリング
          receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(new_optionsH)

タイムスタンプとキャッシュ管理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetCurrentTimestamp() -> tuple

   現在のプロジェクトタイムスタンプを取得します。

   :return: タイムスタンプ（4つの整数値のタプル）
   :rtype: tuple
   :raises RuntimeError: 取得が失敗した場合

   .. note::
      プロジェクト内でレンダリングに影響する変更が加えられるたびに更新されます。
      キャッシュの有効性チェックに使用されます。

   **例**:

   .. code-block:: python

      # タイムスタンプを取得
      ts = ae.sdk.AEGP_GetCurrentTimestamp()
      print(f"タイムスタンプ: {ts}")  # (ts0, ts1, ts2, ts3)

      # 後でキャッシュの有効性をチェック
      has_changed = ae.sdk.AEGP_HasItemChangedSinceTimestamp(
          itemH, 0.0, 10.0, ts[0], ts[1], ts[2], ts[3])

.. function:: AEGP_HasItemChangedSinceTimestamp(itemH: int, start_time_seconds: float, duration_seconds: float, ts0: int, ts1: int, ts2: int, ts3: int) -> bool

   タイムスタンプ以降にアイテムが変更されたかチェックします。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param start_time_seconds: チェック開始時間（秒）
   :type start_time_seconds: float
   :param duration_seconds: チェック期間（秒）
   :type duration_seconds: float
   :param ts0, ts1, ts2, ts3: タイムスタンプ（``AEGP_GetCurrentTimestamp()`` で取得）
   :type ts0, ts1, ts2, ts3: int
   :return: True - 変更あり、False - 変更なし
   :rtype: bool
   :raises ValueError: 引数が無効な場合
   :raises RuntimeError: チェックが失敗した場合

   .. note::
      ビデオの変更のみをチェックします。オーディオの変更は影響しません。

   **例**:

   .. code-block:: python

      # タイムスタンプを保存
      ts = ae.sdk.AEGP_GetCurrentTimestamp()

      # ... 何らかの処理 ...

      # アイテムが変更されたかチェック
      itemH = ae.sdk.AEGP_GetProjectRootFolder(projH)
      if ae.sdk.AEGP_HasItemChangedSinceTimestamp(
          itemH, 0.0, 10.0, ts[0], ts[1], ts[2], ts[3]):
          print("アイテムが変更されました - 再レンダリングが必要")
      else:
          print("アイテムは変更されていません - キャッシュ有効")

.. function:: AEGP_IsItemWorthwhileToRender(roH: int, ts0: int, ts1: int, ts2: int, ts3: int) -> bool

   アイテムがレンダリングする価値があるかチェックします。

   :param roH: レンダリングオプションハンドル（AEGP_RenderOptionsH）
   :type roH: int
   :param ts0, ts1, ts2, ts3: タイムスタンプ（``AEGP_GetCurrentTimestamp()`` で取得）
   :type ts0, ts1, ts2, ts3: int
   :return: True - レンダリングする価値あり、False - スキップ推奨
   :rtype: bool
   :raises ValueError: roHがnullの場合
   :raises RuntimeError: チェックが失敗した場合

   .. important::
      この関数は以下のタイミングで呼び出すことが推奨されます:

      1. レンダリング開始前
      2. レンダリング完了後、``AEGP_CheckinFrame()`` を呼ぶ前

   **例**:

   .. code-block:: python

      ts = ae.sdk.AEGP_GetCurrentTimestamp()

      # レンダリング開始前にチェック
      if not ae.sdk.AEGP_IsItemWorthwhileToRender(
          optionsH, ts[0], ts[1], ts[2], ts[3]):
          print("レンダリングをスキップします")
          return

      # レンダリング実行
      receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)

      try:
          # レンダリング完了後にも再チェック
          if ae.sdk.AEGP_IsItemWorthwhileToRender(
              optionsH, ts[0], ts[1], ts[2], ts[3]):
              # フレームデータを使用
              worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)
              # ...
          else:
              print("変更があったため、結果を破棄します")
      finally:
          ae.sdk.AEGP_CheckinFrame(receiptH)

.. function:: AEGP_CheckinRenderedFrame(roH: int, ts0: int, ts1: int, ts2: int, ts3: int, ticks_to_render: int, imageH: int) -> None

   **スタブ（完全未実装）** - レンダリングされたフレームをチェックイン

   この関数はAEGP_PlatformWorldHを必要とし、プラットフォーム固有の
   複雑な処理が必要なため、完全には実装されていません。

   :raises RuntimeError: 常にエラーをスロー

   **代替案**:

   標準のレンダリングパイプラインを使用:
   ``AEGP_RenderAndCheckoutFrame()`` + ``AEGP_CheckinFrame()``

サウンドデータレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RenderNewItemSoundData(itemH: int, start_time_seconds: float, duration_seconds: float, sample_rate: float, encoding: int, bytes_per_sample: int, num_channels: int) -> int

   アイテムのサウンドデータをレンダリングします。

   :param itemH: アイテムハンドル（コンポジションまたはフッテージ）
   :type itemH: int
   :param start_time_seconds: 開始時間（秒）
   :type start_time_seconds: float
   :param duration_seconds: 継続時間（秒）
   :type duration_seconds: float
   :param sample_rate: サンプルレート（Hz、例: 44100.0, 48000.0）
   :type sample_rate: float
   :param encoding: エンコーディング（3: UNSIGNED_PCM, 4: SIGNED_PCM, 5: FLOAT）
   :type encoding: int
   :param bytes_per_sample: サンプルあたりのバイト数（1, 2, 4）、FLOAT encodingの場合は無視
   :type bytes_per_sample: int
   :param num_channels: チャンネル数（1: モノラル、2: ステレオ）
   :type num_channels: int
   :return: サウンドデータハンドル（AEGP_SoundDataH）、オーディオがない場合は0
   :rtype: int
   :raises ValueError: 引数が無効な場合
   :raises RuntimeError: レンダリングが失敗した場合

   **エンコーディング定数**:

   .. list-table::
      :header-rows: 1

      * - 定数
        - 値
        - 説明
      * - ``AEGP_SoundEncoding_UNSIGNED_PCM``
        - 3
        - 符号なしPCM
      * - ``AEGP_SoundEncoding_SIGNED_PCM``
        - 4
        - 符号付きPCM（最も一般的）
      * - ``AEGP_SoundEncoding_FLOAT``
        - 5
        - 浮動小数点（高品質）

   .. note::
      - キャンセルコールバックはPythonから使用不可（内部でnullptr固定）
      - 使用後は ``AEGP_DisposeSoundData()`` で破棄する必要があります

   **例**:

   .. code-block:: python

      import ae

      # コンポジションのオーディオをレンダリング
      compItemH = ...  # コンポジションアイテムハンドル

      soundDataH = ae.sdk.AEGP_RenderNewItemSoundData(
          compItemH,
          start_time_seconds=0.0,
          duration_seconds=10.0,
          sample_rate=48000.0,
          encoding=4,  # SIGNED_PCM
          bytes_per_sample=2,
          num_channels=2  # ステレオ
      )

      if soundDataH:
          try:
              # サウンドデータを使用（SoundDataSuite経由）
              num_samples = ae.sdk.AEGP_GetNumSamples(soundDataH)
              print(f"サンプル数: {num_samples}")
              # ...
          finally:
              # 必ず破棄
              ae.sdk.AEGP_DisposeSoundData(soundDataH)
      else:
          print("オーディオデータがありません")

使用例
------

基本的なフレームレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_frame(comp_itemH, time_seconds):
       """コンポジションの特定時間のフレームをレンダリング"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # レンダリングオプションを作成
       optionsH = ae.sdk.AEGP_NewFromItem(plugin_id, comp_itemH)

       try:
           # レンダリング時間を設定
           ae.sdk.AEGP_SetTime(optionsH, time_seconds)

           # フレームをレンダリング
           receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)

           try:
               # ワールドを取得
               worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)

               # ワールド情報を取得
               info = ae.sdk.AEGP_GetWorldInfo(worldH)
               print(f"レンダリング完了: {info['width']}x{info['height']}")

               # レンダリング領域を取得
               region = ae.sdk.AEGP_GetRenderedRegion(receiptH)
               print(f"領域: {region}")

               # GUIDを取得
               guid = ae.sdk.AEGP_GetReceiptGuid(receiptH)
               print(f"GUID: {guid}")

               return worldH

           finally:
               # 必ずチェックイン
               ae.sdk.AEGP_CheckinFrame(receiptH)
       finally:
           ae.sdk.AEGP_Dispose(optionsH)

   # 実行例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)
   # 最初のコンポジションを取得（実際にはItemSuiteで検索）
   first_compH = ...
   render_frame(first_compH, 1.0)

タイムスタンプベースのキャッシュ管理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   class RenderCache:
       """タイムスタンプベースのレンダリングキャッシュ"""

       def __init__(self):
           self.cache = {}  # {(itemH, time): (timestamp, worldH)}

       def render_with_cache(self, itemH, time_seconds):
           """キャッシュを使用してレンダリング"""
           plugin_id = ae.sdk.AEGP_GetPluginID()

           # 現在のタイムスタンプを取得
           current_ts = ae.sdk.AEGP_GetCurrentTimestamp()

           # キャッシュをチェック
           cache_key = (itemH, time_seconds)
           if cache_key in self.cache:
               cached_ts, cached_worldH = self.cache[cache_key]

               # キャッシュが有効かチェック
               if not ae.sdk.AEGP_HasItemChangedSinceTimestamp(
                   itemH, time_seconds, 0.1,
                   cached_ts[0], cached_ts[1], cached_ts[2], cached_ts[3]):
                   print("キャッシュヒット")
                   return cached_worldH
               else:
                   print("キャッシュ無効 - 再レンダリング")

           # レンダリング
           print("新規レンダリング")
           optionsH = ae.sdk.AEGP_NewFromItem(plugin_id, itemH)

           try:
               ae.sdk.AEGP_SetTime(optionsH, time_seconds)

               # レンダリングする価値があるかチェック
               if not ae.sdk.AEGP_IsItemWorthwhileToRender(
                   optionsH, current_ts[0], current_ts[1],
                   current_ts[2], current_ts[3]):
                   print("レンダリング不要")
                   return None

               receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)

               try:
                   worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)

                   # キャッシュに保存
                   self.cache[cache_key] = (current_ts, worldH)

                   return worldH
               finally:
                   ae.sdk.AEGP_CheckinFrame(receiptH)
           finally:
               ae.sdk.AEGP_Dispose(optionsH)

   # 使用例
   cache = RenderCache()
   itemH = ...
   worldH1 = cache.render_with_cache(itemH, 1.0)
   worldH2 = cache.render_with_cache(itemH, 1.0)  # キャッシュヒット

オーディオのレンダリングと解析
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import struct

   def render_and_analyze_audio(comp_itemH, start_sec, duration_sec):
       """オーディオをレンダリングして解析"""

       # オーディオをレンダリング
       soundDataH = ae.sdk.AEGP_RenderNewItemSoundData(
           comp_itemH,
           start_time_seconds=start_sec,
           duration_seconds=duration_sec,
           sample_rate=48000.0,
           encoding=4,  # SIGNED_PCM
           bytes_per_sample=2,  # 16ビット
           num_channels=2  # ステレオ
       )

       if not soundDataH:
           print("オーディオデータがありません")
           return

       try:
           # サウンドデータ情報を取得
           num_samples = ae.sdk.AEGP_GetNumSamples(soundDataH)
           sample_rate = ae.sdk.AEGP_GetSampleRate(soundDataH)

           print(f"サンプル数: {num_samples}")
           print(f"サンプルレート: {sample_rate} Hz")
           print(f"継続時間: {num_samples / sample_rate:.2f} 秒")

           # オーディオデータをロック
           audio_data = ae.sdk.AEGP_LockSoundData(soundDataH)

           try:
               # データを解析（例: ピークレベルを検出）
               # audio_dataは bytes オブジェクト
               # 16ビット符号付きPCM、ステレオ = 4バイト/サンプル

               max_left = 0
               max_right = 0

               for i in range(0, len(audio_data), 4):
                   if i + 4 > len(audio_data):
                       break

                   # Left channel (16-bit signed)
                   left = struct.unpack('<h', audio_data[i:i+2])[0]
                   # Right channel (16-bit signed)
                   right = struct.unpack('<h', audio_data[i+2:i+4])[0]

                   max_left = max(max_left, abs(left))
                   max_right = max(max_right, abs(right))

               # -1.0 ～ 1.0 に正規化
               max_left_norm = max_left / 32768.0
               max_right_norm = max_right / 32768.0

               print(f"ピークレベル (L): {max_left_norm:.3f}")
               print(f"ピークレベル (R): {max_right_norm:.3f}")

           finally:
               ae.sdk.AEGP_UnlockSoundData(soundDataH)

       finally:
           ae.sdk.AEGP_DisposeSoundData(soundDataH)

   # 実行例
   comp_itemH = ...
   render_and_analyze_audio(comp_itemH, 0.0, 5.0)

レイヤーフレームのレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_layer_frame(layerH, comp_time_seconds):
       """レイヤーの特定時間のフレームをレンダリング"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # レイヤーレンダリングオプションを作成
       layer_optionsH = ae.sdk.AEGP_NewFromLayer(plugin_id, layerH)

       try:
           # レンダリング時間を設定
           ae.sdk.AEGP_SetLayerTime(layer_optionsH, comp_time_seconds)

           # レイヤーフレームをレンダリング
           receiptH = ae.sdk.AEGP_RenderAndCheckoutLayerFrame(layer_optionsH)

           try:
               # ワールドを取得
               worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)

               # レイヤーのレンダリング結果を処理
               info = ae.sdk.AEGP_GetWorldInfo(worldH)
               print(f"レイヤーサイズ: {info['width']}x{info['height']}")

               # ピクセルデータにアクセス
               if info['type'] == 1:  # 8ビット
                   base_addr = ae.sdk.AEGP_GetBaseAddr8(worldH)
                   # ... ピクセル処理 ...

               return worldH

           finally:
               ae.sdk.AEGP_CheckinFrame(receiptH)
       finally:
           ae.sdk.AEGP_Dispose(layer_optionsH)

   # 使用例
   layerH = ...  # レイヤーハンドル取得
   render_layer_frame(layerH, 2.0)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **チェックインを忘れない**

   すべてのチェックアウトしたフレームは必ずチェックインしてください。
   ``try-finally`` ブロックを使用して確実に実行することを推奨します。

2. **読み取り専用のワールド**

   ``AEGP_GetReceiptWorld()`` で取得したワールドは読み取り専用です。
   変更が必要な場合は、新しいワールドを作成してコピーしてください。

3. **UIスレッドでの同期レンダリング**

   UIスレッドで同期レンダリングを実行すると、UIがブロックされます。
   可能な限り非同期処理を検討してください。

4. **タイムスタンプの活用**

   キャッシュ管理にはタイムスタンプを積極的に活用してください。
   不要な再レンダリングを防ぎ、パフォーマンスを向上できます。

5. **サウンドデータの破棄**

   ``AEGP_RenderNewItemSoundData()`` で取得したサウンドデータは、
   使用後に必ず ``AEGP_DisposeSoundData()`` で破棄してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

確実なリソース管理
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)
   try:
       # フレームを使用
       worldH = ae.sdk.AEGP_GetReceiptWorld(receiptH)
       # ...
   finally:
       # 必ずチェックイン
       ae.sdk.AEGP_CheckinFrame(receiptH)

タイムスタンプチェック
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ts = ae.sdk.AEGP_GetCurrentTimestamp()

   # レンダリング前
   if ae.sdk.AEGP_IsItemWorthwhileToRender(
       optionsH, ts[0], ts[1], ts[2], ts[3]):
       receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)

キャッシュ検証
^^^^^^^^^^^^^^

.. code-block:: python

   if ae.sdk.AEGP_IsRenderedFrameSufficient(cached_optionsH, new_optionsH):
       # キャッシュを使用
       use_cached_frame()
   else:
       # 再レンダリング
       render_new_frame()

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       receiptH = ae.sdk.AEGP_RenderAndCheckoutFrame(optionsH)
   except RuntimeError as e:
       print(f"レンダリング失敗: {e}")
       return None

関連項目
--------

- :doc:`AEGP_RenderOptionsSuite4` - レンダリングオプションの設定
- :doc:`AEGP_LayerRenderOptionsSuite2` - レイヤーレンダリングオプションの設定
- :doc:`AEGP_WorldSuite3` - ワールド（フレームバッファ）の操作
- :doc:`AEGP_SoundDataSuite1` - サウンドデータの管理
- :doc:`AEGP_RenderQueueSuite1` - レンダーキューの管理
- :doc:`index` - 低レベルAPI概要
