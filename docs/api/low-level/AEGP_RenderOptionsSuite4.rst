AEGP_RenderOptionsSuite4
========================

.. currentmodule:: ae.sdk

AEGP_RenderOptionsSuite4は、After Effectsのレンダリングパラメータを設定・取得するためのSDK APIです。

概要
----

**実装状況**: 18/18関数実装 ✅

AEGP_RenderOptionsSuite4は以下の機能を提供します:

- レンダリングオプションハンドルの作成・複製・解放
- レンダリング時間とタイムステップの設定
- ビット深度（ワールドタイプ）の設定
- ダウンサンプリング設定
- 関心領域（ROI）の設定
- フィールドレンダリングモードの設定
- マットモードとチャンネル順序の設定
- ガイドレイヤーの表示設定
- レンダリング品質の設定

基本概念
--------

レンダリングオプションハンドル (AEGP_RenderOptionsH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダリング設定を保持するハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - レンダリングオプションハンドルは、``AEGP_RenderOptions_NewFromItem()`` または ``AEGP_RenderOptions_Duplicate()`` で作成します
   - 作成されたハンドルは使用後に **必ず** ``AEGP_RenderOptions_Dispose()`` で解放する必要があります
   - ハンドルを解放しないとメモリリークが発生します

ハンドルのライフサイクル
~~~~~~~~~~~~~~~~~~~~~~~~

1. **作成**: ``AEGP_RenderOptions_NewFromItem(itemH)`` でアイテムから作成
2. **設定**: 各種Set関数でパラメータを設定
3. **使用**: レンダリングAPIで使用
4. **解放**: ``AEGP_RenderOptions_Dispose(optionsH)`` で解放

.. code-block:: python

   # ライフサイクルの例
   optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)
   try:
       ae.sdk.AEGP_RenderOptions_SetTime(optionsH, 1.5)
       ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_16)
       # ... レンダリング処理 ...
   finally:
       ae.sdk.AEGP_RenderOptions_Dispose(optionsH)  # 必ず解放

定数リファレンス
----------------

ビット深度 (AEGP_WorldType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_WorldType_NONE``
     - 0
     - 未指定
   * - ``AEGP_WorldType_8``
     - 1
     - 8ビット/チャンネル
   * - ``AEGP_WorldType_16``
     - 2
     - 16ビット/チャンネル
   * - ``AEGP_WorldType_32``
     - 3
     - 32ビット/チャンネル（浮動小数点）

フィールドレンダリングモード (PF_Field)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``PF_Field_FRAME``
     - 0
     - フレーム（フィールドなし）
   * - ``PF_Field_UPPER``
     - 1
     - 上位フィールド
   * - ``PF_Field_LOWER``
     - 2
     - 下位フィールド

マットモード (AEGP_MatteMode)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_MatteMode_STRAIGHT``
     - 0
     - ストレートアルファ
   * - ``AEGP_MatteMode_PREMUL_BLACK``
     - 1
     - プリマルチプライド（黒背景）
   * - ``AEGP_MatteMode_PREMUL_BG_COLOR``
     - 2
     - プリマルチプライド（背景色）

チャンネル順序 (AEGP_ChannelOrder)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_ChannelOrder_ARGB``
     - 0
     - ARGB順序（Alpha, Red, Green, Blue）
   * - ``AEGP_ChannelOrder_BGRA``
     - 1
     - BGRA順序（Blue, Green, Red, Alpha）

レンダリング品質 (AEGP_ItemQuality)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_ItemQuality_DRAFT``
     - 0
     - ドラフト品質（高速、低品質）
   * - ``AEGP_ItemQuality_BEST``
     - 1
     - 最高品質（低速、高品質）

APIリファレンス
----------------

ハンドル管理
~~~~~~~~~~~~

.. function:: AEGP_RenderOptions_NewFromItem(itemH: int) -> int

   アイテムから新しいレンダリングオプションハンドルを作成します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: レンダリングオプションハンドル
   :rtype: int

   **初期値**:

   - Time: 0
   - Time step: フレームデュレーション
   - Field render: none
   - Depth: アイテムの最高解像度

   .. important::
      作成したハンドルは使用後に ``AEGP_RenderOptions_Dispose()`` で解放する必要があります。

   **例**:

   .. code-block:: python

      itemH = ae.sdk.AEGP_GetActiveItem()
      optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)

.. function:: AEGP_RenderOptions_Duplicate(optionsH: int) -> int

   レンダリングオプションを複製します。

   :param optionsH: 複製元のレンダリングオプションハンドル
   :type optionsH: int
   :return: 複製されたレンダリングオプションハンドル
   :rtype: int

   .. important::
      複製されたハンドルも使用後に ``AEGP_RenderOptions_Dispose()`` で解放する必要があります。

   **例**:

   .. code-block:: python

      copyH = ae.sdk.AEGP_RenderOptions_Duplicate(optionsH)

.. function:: AEGP_RenderOptions_Dispose(optionsH: int) -> None

   レンダリングオプションハンドルを解放します。

   :param optionsH: 解放するレンダリングオプションハンドル
   :type optionsH: int

   .. important::
      ``NewFromItem`` または ``Duplicate`` で作成したハンドルは必ずこの関数で解放してください。
      解放しないとメモリリークが発生します。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_RenderOptions_Dispose(optionsH)

時間設定
~~~~~~~~

.. function:: AEGP_RenderOptions_SetTime(optionsH: int, time_seconds: float) -> None

   レンダリング時間を設定します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param time_seconds: レンダリング時間（秒）
   :type time_seconds: float

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_RenderOptions_SetTime(optionsH, 1.5)  # 1.5秒の時点でレンダリング

.. function:: AEGP_RenderOptions_GetTime(optionsH: int) -> float

   レンダリング時間を取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: レンダリング時間（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      time = ae.sdk.AEGP_RenderOptions_GetTime(optionsH)
      print(f"レンダリング時間: {time}秒")

.. function:: AEGP_RenderOptions_SetTimeStep(optionsH: int, time_step_seconds: float) -> None

   フレームの時間幅を設定します。モーションブラーに重要な設定です。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param time_step_seconds: 時間幅（秒）
   :type time_step_seconds: float

   .. note::
      モーションブラーを正しく計算するために、フレームの時間幅（通常は1フレーム分のデュレーション）を設定します。

   **例**:

   .. code-block:: python

      # 30fpsの場合、1フレームは約0.0333秒
      ae.sdk.AEGP_RenderOptions_SetTimeStep(optionsH, 1.0 / 30.0)

.. function:: AEGP_RenderOptions_GetTimeStep(optionsH: int) -> float

   フレームの時間幅を取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: 時間幅（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      time_step = ae.sdk.AEGP_RenderOptions_GetTimeStep(optionsH)

ビット深度設定
~~~~~~~~~~~~~~

.. function:: AEGP_RenderOptions_SetWorldType(optionsH: int, type: int) -> None

   ワールドタイプ（ビット深度）を設定します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param type: ワールドタイプ（``AEGP_WorldType_8``, ``AEGP_WorldType_16``, ``AEGP_WorldType_32``）
   :type type: int

   **例**:

   .. code-block:: python

      # 16ビット/チャンネルでレンダリング
      ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_16)

.. function:: AEGP_RenderOptions_GetWorldType(optionsH: int) -> int

   ワールドタイプ（ビット深度）を取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: ワールドタイプ
   :rtype: int

   **例**:

   .. code-block:: python

      world_type = ae.sdk.AEGP_RenderOptions_GetWorldType(optionsH)
      if world_type == ae.sdk.AEGP_WorldType_32:
          print("32ビット浮動小数点でレンダリング")

フィールドレンダリング
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RenderOptions_SetFieldRender(optionsH: int, field_render: int) -> None

   フィールドレンダリングモードを設定します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param field_render: フィールドモード（``PF_Field_FRAME``, ``PF_Field_UPPER``, ``PF_Field_LOWER``）
   :type field_render: int

   **例**:

   .. code-block:: python

      # 上位フィールドでレンダリング
      ae.sdk.AEGP_RenderOptions_SetFieldRender(optionsH, ae.sdk.PF_Field_UPPER)

.. function:: AEGP_RenderOptions_GetFieldRender(optionsH: int) -> int

   フィールドレンダリングモードを取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: フィールドモード
   :rtype: int

   **例**:

   .. code-block:: python

      field_render = ae.sdk.AEGP_RenderOptions_GetFieldRender(optionsH)

ダウンサンプリング
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RenderOptions_SetDownsampleFactor(optionsH: int, x: int, y: int) -> None

   ダウンサンプル係数を設定します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param x: 水平方向のダウンサンプル係数（1=100%, 2=50%, 4=25%）
   :type x: int
   :param y: 垂直方向のダウンサンプル係数（1=100%, 2=50%, 4=25%）
   :type y: int

   .. note::
      - 1 = 100%（フル解像度）
      - 2 = 50%（半分の解像度）
      - 4 = 25%（1/4の解像度）

   **例**:

   .. code-block:: python

      # 半分の解像度でレンダリング（高速プレビュー用）
      ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(optionsH, 2, 2)

.. function:: AEGP_RenderOptions_GetDownsampleFactor(optionsH: int) -> tuple

   ダウンサンプル係数を取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: (x, y) ダウンサンプル係数のタプル
   :rtype: tuple

   **例**:

   .. code-block:: python

      x, y = ae.sdk.AEGP_RenderOptions_GetDownsampleFactor(optionsH)
      print(f"解像度: {100//x}% x {100//y}%")

関心領域（ROI）
~~~~~~~~~~~~~~~

.. function:: AEGP_RenderOptions_SetRegionOfInterest(optionsH: int, roi: dict) -> None

   関心領域（ROI）を設定します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param roi: 関心領域の辞書 ``{"left": int, "top": int, "right": int, "bottom": int}``
   :type roi: dict

   .. note::
      ``{"left": 0, "top": 0, "right": 0, "bottom": 0}`` を指定すると全体をレンダリングします。

   **例**:

   .. code-block:: python

      # 100x100から300x200の領域のみレンダリング
      ae.sdk.AEGP_RenderOptions_SetRegionOfInterest(
          optionsH,
          {"left": 100, "top": 100, "right": 300, "bottom": 200}
      )

.. function:: AEGP_RenderOptions_GetRegionOfInterest(optionsH: int) -> dict

   関心領域（ROI）を取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: ``{"left": int, "top": int, "right": int, "bottom": int}``
   :rtype: dict

   **例**:

   .. code-block:: python

      roi = ae.sdk.AEGP_RenderOptions_GetRegionOfInterest(optionsH)
      print(f"ROI: ({roi['left']}, {roi['top']}) - ({roi['right']}, {roi['bottom']})")

マットモードとチャンネル順序
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RenderOptions_SetMatteMode(optionsH: int, mode: int) -> None

   マットモードを設定します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param mode: マットモード（``AEGP_MatteMode_STRAIGHT``, ``AEGP_MatteMode_PREMUL_BLACK``, ``AEGP_MatteMode_PREMUL_BG_COLOR``）
   :type mode: int

   **例**:

   .. code-block:: python

      # プリマルチプライドアルファ（黒背景）でレンダリング
      ae.sdk.AEGP_RenderOptions_SetMatteMode(optionsH, ae.sdk.AEGP_MatteMode_PREMUL_BLACK)

.. function:: AEGP_RenderOptions_GetMatteMode(optionsH: int) -> int

   マットモードを取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: マットモード
   :rtype: int

   **例**:

   .. code-block:: python

      mode = ae.sdk.AEGP_RenderOptions_GetMatteMode(optionsH)

.. function:: AEGP_RenderOptions_SetChannelOrder(optionsH: int, order: int) -> None

   チャンネル順序を設定します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param order: チャンネル順序（``AEGP_ChannelOrder_ARGB``, ``AEGP_ChannelOrder_BGRA``）
   :type order: int

   **例**:

   .. code-block:: python

      # BGRA順序でレンダリング
      ae.sdk.AEGP_RenderOptions_SetChannelOrder(optionsH, ae.sdk.AEGP_ChannelOrder_BGRA)

.. function:: AEGP_RenderOptions_GetChannelOrder(optionsH: int) -> int

   チャンネル順序を取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: チャンネル順序
   :rtype: int

   **例**:

   .. code-block:: python

      order = ae.sdk.AEGP_RenderOptions_GetChannelOrder(optionsH)

ガイドレイヤー
~~~~~~~~~~~~~~

.. function:: AEGP_RenderOptions_SetRenderGuideLayers(optionsH: int, render_them: bool) -> None

   ガイドレイヤーをレンダリングするかどうかを設定します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param render_them: Trueでガイドレイヤーをレンダリング
   :type render_them: bool

   **例**:

   .. code-block:: python

      # ガイドレイヤーも含めてレンダリング
      ae.sdk.AEGP_RenderOptions_SetRenderGuideLayers(optionsH, True)

.. function:: AEGP_RenderOptions_GetRenderGuideLayers(optionsH: int) -> bool

   ガイドレイヤーをレンダリングするかどうかを取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: ガイドレイヤーをレンダリングする場合True
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_RenderOptions_GetRenderGuideLayers(optionsH):
          print("ガイドレイヤーをレンダリングします")

レンダリング品質
~~~~~~~~~~~~~~~~

.. function:: AEGP_RenderOptions_SetRenderQuality(optionsH: int, quality: int) -> None

   レンダリング品質を設定します（フッテージアイテムのデコード品質に影響）。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :param quality: 品質レベル（``AEGP_ItemQuality_DRAFT``, ``AEGP_ItemQuality_BEST``）
   :type quality: int

   **例**:

   .. code-block:: python

      # 最高品質でレンダリング
      ae.sdk.AEGP_RenderOptions_SetRenderQuality(optionsH, ae.sdk.AEGP_ItemQuality_BEST)

.. function:: AEGP_RenderOptions_GetRenderQuality(optionsH: int) -> int

   レンダリング品質を取得します。

   :param optionsH: レンダリングオプションハンドル
   :type optionsH: int
   :return: 品質レベル
   :rtype: int

   **例**:

   .. code-block:: python

      quality = ae.sdk.AEGP_RenderOptions_GetRenderQuality(optionsH)
      if quality == ae.sdk.AEGP_ItemQuality_DRAFT:
          print("ドラフト品質")

使用例
------

基本的なレンダリング設定
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_render_options():
       """レンダリングオプションを設定してレンダリングする例"""
       # アクティブなアイテムを取得
       itemH = ae.sdk.AEGP_GetActiveItem()

       # レンダリングオプションを作成
       optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)

       try:
           # レンダリング時間: 5秒
           ae.sdk.AEGP_RenderOptions_SetTime(optionsH, 5.0)

           # 16ビット/チャンネル
           ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_16)

           # フル解像度
           ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(optionsH, 1, 1)

           # 最高品質
           ae.sdk.AEGP_RenderOptions_SetRenderQuality(optionsH, ae.sdk.AEGP_ItemQuality_BEST)

           # ... レンダリング処理（他のAPIで使用）...

       finally:
           # 必ず解放
           ae.sdk.AEGP_RenderOptions_Dispose(optionsH)

   # 実行
   setup_render_options()

プレビュー用の軽量設定
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_preview_options():
       """プレビュー用の軽量レンダリング設定"""
       itemH = ae.sdk.AEGP_GetActiveItem()
       optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)

       try:
           # 現在の時間
           ae.sdk.AEGP_RenderOptions_SetTime(optionsH, 0.0)

           # 8ビット/チャンネル（軽量）
           ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_8)

           # 半分の解像度（高速）
           ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(optionsH, 2, 2)

           # ドラフト品質（高速）
           ae.sdk.AEGP_RenderOptions_SetRenderQuality(optionsH, ae.sdk.AEGP_ItemQuality_DRAFT)

           # ガイドレイヤーは非表示
           ae.sdk.AEGP_RenderOptions_SetRenderGuideLayers(optionsH, False)

           # ... プレビューレンダリング ...

       finally:
           ae.sdk.AEGP_RenderOptions_Dispose(optionsH)

   setup_preview_options()

ROI（部分領域）のレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_region(left, top, right, bottom):
       """画像の一部分だけをレンダリング"""
       itemH = ae.sdk.AEGP_GetActiveItem()
       optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)

       try:
           # 関心領域を設定
           ae.sdk.AEGP_RenderOptions_SetRegionOfInterest(
               optionsH,
               {"left": left, "top": top, "right": right, "bottom": bottom}
           )

           # 32ビット浮動小数点（高品質）
           ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_32)

           # フル解像度
           ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(optionsH, 1, 1)

           # ... レンダリング ...

       finally:
           ae.sdk.AEGP_RenderOptions_Dispose(optionsH)

   # 100x100から500x300の領域をレンダリング
   render_region(100, 100, 500, 300)

フィールドレンダリング
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_fields():
       """上位・下位フィールドを個別にレンダリング"""
       itemH = ae.sdk.AEGP_GetActiveItem()

       # 上位フィールドをレンダリング
       upper_optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)
       try:
           ae.sdk.AEGP_RenderOptions_SetFieldRender(upper_optionsH, ae.sdk.PF_Field_UPPER)
           # ... レンダリング ...
       finally:
           ae.sdk.AEGP_RenderOptions_Dispose(upper_optionsH)

       # 下位フィールドをレンダリング
       lower_optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)
       try:
           ae.sdk.AEGP_RenderOptions_SetFieldRender(lower_optionsH, ae.sdk.PF_Field_LOWER)
           # ... レンダリング ...
       finally:
           ae.sdk.AEGP_RenderOptions_Dispose(lower_optionsH)

   render_fields()

レンダリングオプションの複製
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def duplicate_and_modify():
       """既存のレンダリングオプションを複製して変更"""
       itemH = ae.sdk.AEGP_GetActiveItem()
       optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)

       try:
           # ベース設定
           ae.sdk.AEGP_RenderOptions_SetTime(optionsH, 1.0)
           ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_16)

           # 複製して異なる時間でレンダリング
           copy1H = ae.sdk.AEGP_RenderOptions_Duplicate(optionsH)
           try:
               ae.sdk.AEGP_RenderOptions_SetTime(copy1H, 2.0)
               # ... レンダリング ...
           finally:
               ae.sdk.AEGP_RenderOptions_Dispose(copy1H)

           copy2H = ae.sdk.AEGP_RenderOptions_Duplicate(optionsH)
           try:
               ae.sdk.AEGP_RenderOptions_SetTime(copy2H, 3.0)
               # ... レンダリング ...
           finally:
               ae.sdk.AEGP_RenderOptions_Dispose(copy2H)

       finally:
           ae.sdk.AEGP_RenderOptions_Dispose(optionsH)

   duplicate_and_modify()

現在の設定のダンプ
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_render_options(optionsH):
       """レンダリングオプションの設定を表示"""
       print("=" * 50)
       print("レンダリングオプション設定:")
       print("=" * 50)

       # 時間
       time = ae.sdk.AEGP_RenderOptions_GetTime(optionsH)
       print(f"時間: {time}秒")

       # タイムステップ
       time_step = ae.sdk.AEGP_RenderOptions_GetTimeStep(optionsH)
       print(f"タイムステップ: {time_step}秒")

       # ビット深度
       world_type = ae.sdk.AEGP_RenderOptions_GetWorldType(optionsH)
       bit_depth_names = {
           ae.sdk.AEGP_WorldType_8: "8ビット",
           ae.sdk.AEGP_WorldType_16: "16ビット",
           ae.sdk.AEGP_WorldType_32: "32ビット（浮動小数点）"
       }
       print(f"ビット深度: {bit_depth_names.get(world_type, '不明')}")

       # ダウンサンプル
       x, y = ae.sdk.AEGP_RenderOptions_GetDownsampleFactor(optionsH)
       print(f"ダウンサンプル: {x}x{y} ({100//x}% x {100//y}%)")

       # ROI
       roi = ae.sdk.AEGP_RenderOptions_GetRegionOfInterest(optionsH)
       print(f"ROI: ({roi['left']}, {roi['top']}) - ({roi['right']}, {roi['bottom']})")

       # フィールドレンダリング
       field_render = ae.sdk.AEGP_RenderOptions_GetFieldRender(optionsH)
       field_names = {
           ae.sdk.PF_Field_FRAME: "フレーム",
           ae.sdk.PF_Field_UPPER: "上位フィールド",
           ae.sdk.PF_Field_LOWER: "下位フィールド"
       }
       print(f"フィールド: {field_names.get(field_render, '不明')}")

       # マットモード
       matte_mode = ae.sdk.AEGP_RenderOptions_GetMatteMode(optionsH)
       matte_names = {
           ae.sdk.AEGP_MatteMode_STRAIGHT: "ストレートアルファ",
           ae.sdk.AEGP_MatteMode_PREMUL_BLACK: "プリマルチプライド（黒）",
           ae.sdk.AEGP_MatteMode_PREMUL_BG_COLOR: "プリマルチプライド（背景色）"
       }
       print(f"マットモード: {matte_names.get(matte_mode, '不明')}")

       # チャンネル順序
       channel_order = ae.sdk.AEGP_RenderOptions_GetChannelOrder(optionsH)
       order_names = {
           ae.sdk.AEGP_ChannelOrder_ARGB: "ARGB",
           ae.sdk.AEGP_ChannelOrder_BGRA: "BGRA"
       }
       print(f"チャンネル順序: {order_names.get(channel_order, '不明')}")

       # ガイドレイヤー
       render_guide = ae.sdk.AEGP_RenderOptions_GetRenderGuideLayers(optionsH)
       print(f"ガイドレイヤー: {'表示' if render_guide else '非表示'}")

       # 品質
       quality = ae.sdk.AEGP_RenderOptions_GetRenderQuality(optionsH)
       quality_names = {
           ae.sdk.AEGP_ItemQuality_DRAFT: "ドラフト",
           ae.sdk.AEGP_ItemQuality_BEST: "最高品質"
       }
       print(f"品質: {quality_names.get(quality, '不明')}")

       print("=" * 50)

   # 使用例
   itemH = ae.sdk.AEGP_GetActiveItem()
   optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)
   try:
       dump_render_options(optionsH)
   finally:
       ae.sdk.AEGP_RenderOptions_Dispose(optionsH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **ハンドルの解放は必須**

   ``NewFromItem`` または ``Duplicate`` で作成したハンドルは必ず ``Dispose`` で解放してください。
   解放しないとメモリリークが発生します。

2. **try-finallyパターンを使用**

   例外が発生してもハンドルを確実に解放するため、try-finallyブロックを使用してください。

3. **ROIの全体指定**

   ``{"left": 0, "top": 0, "right": 0, "bottom": 0}`` を指定すると全体をレンダリングします。

4. **ダウンサンプルは1以上**

   ダウンサンプル係数は1以上である必要があります（0以下はエラー）。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

ハンドルの確実な解放
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)
   try:
       # 処理
   finally:
       ae.sdk.AEGP_RenderOptions_Dispose(optionsH)

プレビュー/最終出力で設定を切り替え
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def get_render_options(itemH, is_preview=False):
       optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)
       if is_preview:
           ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_8)
           ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(optionsH, 2, 2)
           ae.sdk.AEGP_RenderOptions_SetRenderQuality(optionsH, ae.sdk.AEGP_ItemQuality_DRAFT)
       else:
           ae.sdk.AEGP_RenderOptions_SetWorldType(optionsH, ae.sdk.AEGP_WorldType_32)
           ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(optionsH, 1, 1)
           ae.sdk.AEGP_RenderOptions_SetRenderQuality(optionsH, ae.sdk.AEGP_ItemQuality_BEST)
       return optionsH

ダウンサンプル係数の検証
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def safe_set_downsample(optionsH, x, y):
       if x <= 0 or y <= 0:
           raise ValueError("ダウンサンプル係数は1以上である必要があります")
       ae.sdk.AEGP_RenderOptions_SetDownsampleFactor(optionsH, x, y)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       optionsH = ae.sdk.AEGP_RenderOptions_NewFromItem(itemH)
       try:
           # 設定と処理
       finally:
           ae.sdk.AEGP_RenderOptions_Dispose(optionsH)
   except Exception as e:
       print(f"レンダリングオプションの処理に失敗: {e}")

関連項目
--------

- :doc:`AEGP_RenderSuite5` - レンダリング実行
- :doc:`AEGP_ItemSuite9` - アイテム管理
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`index` - 低レベルAPI概要
