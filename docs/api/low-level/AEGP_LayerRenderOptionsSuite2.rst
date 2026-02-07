AEGP_LayerRenderOptionsSuite2
==============================

.. currentmodule:: ae.sdk

AEGP_LayerRenderOptionsSuite2は、レイヤー単位のレンダリングオプションを設定・制御するためのSDK APIです。

概要
----

AEGP_LayerRenderOptionsSuite2は以下の機能を提供します:

- レイヤー単位のレンダリング設定管理
- レンダリング時間、タイムステップの制御
- ビット深度（WorldType）の設定
- ダウンサンプル係数の制御
- マットモードの設定
- エフェクトチェーンの上流/下流のレンダリングオプション作成

基本概念
--------

LayerRenderOptionsハンドル (AEGP_LayerRenderOptionsH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レイヤーのレンダリング設定を保持するハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   **メモリ管理の重要性**

   - ``AEGP_NewFromLayer``, ``AEGP_NewFromUpstreamOfEffect``, ``AEGP_NewFromDownstreamOfEffect``,
     ``AEGP_DuplicateLayerRenderOptions`` で取得したハンドルは **必ず** ``AEGP_DisposeLayerRenderOptions``
     で解放する必要があります
   - 解放を忘れるとメモリリークが発生します

レンダリングオプションの初期値
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``AEGP_NewFromLayer`` で作成された場合の初期値:

- **Time**: レイヤーの現在時間
- **TimeStep**: レイヤーのフレーム長
- **EffectsToRender**: "all"（すべてのエフェクトを適用）

WorldType（ビット深度）
~~~~~~~~~~~~~~~~~~~~~~~~

レンダリング結果のビット深度を指定します。

.. list-table::
   :header-rows: 1

   * - 値
     - 定数名
     - 説明
   * - 0
     - AEGP_WorldType_NONE
     - 指定なし
   * - 1
     - AEGP_WorldType_8
     - 8ビット/チャンネル
   * - 2
     - AEGP_WorldType_16
     - 16ビット/チャンネル
   * - 3
     - AEGP_WorldType_32
     - 32ビット/チャンネル（浮動小数点）

MatteMode（マットモード）
~~~~~~~~~~~~~~~~~~~~~~~~~~~

アルファチャンネルの扱い方を指定します。

.. list-table::
   :header-rows: 1

   * - 値
     - 定数名
     - 説明
   * - 0
     - AEGP_MatteMode_STRAIGHT
     - ストレートアルファ
   * - 1
     - AEGP_MatteMode_PREMUL_BLACK
     - 黒でプリマルチプライ
   * - 2
     - AEGP_MatteMode_PREMUL_BG_COLOR
     - 背景色でプリマルチプライ

ダウンサンプル係数
~~~~~~~~~~~~~~~~~~~

レンダリング解像度を指定します。

- **1** = 100%（フル解像度）
- **2** = 50%
- **3** = 33.3%
- **4** = 25%

値が大きいほどレンダリング速度は向上しますが、品質は低下します。

API リファレンス
----------------

レンダリングオプションの作成
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NewFromLayer(layerH: int) -> int

   レイヤーから新しいLayerRenderOptionsハンドルを作成します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: LayerRenderOptionsハンドル
   :rtype: int

   .. note::
      戻り値のハンドルは ``AEGP_DisposeLayerRenderOptions`` で解放する必要があります。
      初期値:

      - Time: レイヤーの現在時間
      - TimeStep: レイヤーのフレーム長
      - EffectsToRender: "all"

   **例**:

   .. code-block:: python

      layerH = ae.layer.get_handle()
      optionsH = ae.sdk.AEGP_NewFromLayer(layerH)
      try:
          # レンダリングオプションを使用
          pass
      finally:
          ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

.. function:: AEGP_NewFromUpstreamOfEffect(effectH: int) -> int

   エフェクトの上流（入力側）から新しいLayerRenderOptionsハンドルを作成します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: LayerRenderOptionsハンドル
   :rtype: int

   .. note::
      戻り値のハンドルは ``AEGP_DisposeLayerRenderOptions`` で解放する必要があります。
      EffectsToRender は指定されたエフェクトのインデックスに設定されます。

   **例**:

   .. code-block:: python

      effectH = ae.effect.get_handle()
      optionsH = ae.sdk.AEGP_NewFromUpstreamOfEffect(effectH)
      try:
          # エフェクト適用前のレイヤーをレンダリング
          pass
      finally:
          ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

.. function:: AEGP_NewFromDownstreamOfEffect(effectH: int) -> int

   エフェクトの下流（出力側）から新しいLayerRenderOptionsハンドルを作成します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: LayerRenderOptionsハンドル
   :rtype: int

   .. warning::
      この関数は **UIスレッドからのみ** 呼び出すことができます。

   .. note::
      戻り値のハンドルは ``AEGP_DisposeLayerRenderOptions`` で解放する必要があります。
      EffectsToRender はエフェクト出力を含むように設定されます。

   **例**:

   .. code-block:: python

      effectH = ae.effect.get_handle()
      optionsH = ae.sdk.AEGP_NewFromDownstreamOfEffect(effectH)
      try:
          # エフェクト適用後のレイヤーをレンダリング
          pass
      finally:
          ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

複製と解放
~~~~~~~~~~

.. function:: AEGP_DuplicateLayerRenderOptions(optionsH: int) -> int

   LayerRenderOptionsハンドルを複製します。

   :param optionsH: 複製元のLayerRenderOptionsハンドル
   :type optionsH: int
   :return: 複製されたLayerRenderOptionsハンドル
   :rtype: int

   .. note::
      戻り値のハンドルは ``AEGP_DisposeLayerRenderOptions`` で解放する必要があります。

   **例**:

   .. code-block:: python

      original_optionsH = ae.sdk.AEGP_NewFromLayer(layerH)
      copied_optionsH = ae.sdk.AEGP_DuplicateLayerRenderOptions(original_optionsH)
      try:
          # 異なる設定でレンダリング
          ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(copied_optionsH, 2, 2)
      finally:
          ae.sdk.AEGP_DisposeLayerRenderOptions(copied_optionsH)
          ae.sdk.AEGP_DisposeLayerRenderOptions(original_optionsH)

.. function:: AEGP_DisposeLayerRenderOptions(optionsH: int) -> None

   LayerRenderOptionsハンドルを解放します。

   :param optionsH: 解放するLayerRenderOptionsハンドル
   :type optionsH: int

   .. warning::
      ``AEGP_NewFromLayer``, ``AEGP_NewFromUpstreamOfEffect``, ``AEGP_NewFromDownstreamOfEffect``,
      ``AEGP_DuplicateLayerRenderOptions`` で取得したハンドルは **必ず** この関数で解放してください。

   **例**:

   .. code-block:: python

      optionsH = ae.sdk.AEGP_NewFromLayer(layerH)
      try:
          # 処理
          pass
      finally:
          ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

時間設定
~~~~~~~~

.. function:: AEGP_SetLayerRenderOptionsTime(optionsH: int, time_seconds: float) -> None

   レンダリング時間を設定します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :param time_seconds: レンダリング時間（秒）
   :type time_seconds: float

   **例**:

   .. code-block:: python

      # 5秒の位置でレンダリング
      ae.sdk.AEGP_SetLayerRenderOptionsTime(optionsH, 5.0)

.. function:: AEGP_GetLayerRenderOptionsTime(optionsH: int) -> float

   レンダリング時間を取得します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :return: レンダリング時間（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      time = ae.sdk.AEGP_GetLayerRenderOptionsTime(optionsH)
      print(f"レンダリング時間: {time}秒")

タイムステップ設定
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetLayerRenderOptionsTimeStep(optionsH: int, time_step_seconds: float) -> None

   タイムステップ（フレーム長）を設定します。モーションブラーに影響します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :param time_step_seconds: タイムステップ（秒）
   :type time_step_seconds: float

   **例**:

   .. code-block:: python

      # 30fpsのフレーム長を設定
      ae.sdk.AEGP_SetLayerRenderOptionsTimeStep(optionsH, 1.0 / 30.0)

.. function:: AEGP_GetLayerRenderOptionsTimeStep(optionsH: int) -> float

   タイムステップ（フレーム長）を取得します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :return: タイムステップ（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      time_step = ae.sdk.AEGP_GetLayerRenderOptionsTimeStep(optionsH)
      fps = 1.0 / time_step
      print(f"フレームレート: {fps}fps")

ワールドタイプ（ビット深度）設定
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetLayerRenderOptionsWorldType(optionsH: int, type: int) -> None

   ワールドタイプ（ビット深度）を設定します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :param type: ワールドタイプ（0-3）
   :type type: int

   **ワールドタイプの選択肢**:

   - **0**: AEGP_WorldType_NONE（指定なし）
   - **1**: AEGP_WorldType_8（8ビット）
   - **2**: AEGP_WorldType_16（16ビット）
   - **3**: AEGP_WorldType_32（32ビット浮動小数点）

   **例**:

   .. code-block:: python

      # 32ビット浮動小数点でレンダリング
      ae.sdk.AEGP_SetLayerRenderOptionsWorldType(optionsH, 3)

.. function:: AEGP_GetLayerRenderOptionsWorldType(optionsH: int) -> int

   ワールドタイプ（ビット深度）を取得します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :return: ワールドタイプ（0-3）
   :rtype: int

   **戻り値**:

   - **0**: AEGP_WorldType_NONE（指定なし）
   - **1**: AEGP_WorldType_8（8ビット）
   - **2**: AEGP_WorldType_16（16ビット）
   - **3**: AEGP_WorldType_32（32ビット浮動小数点）

   **例**:

   .. code-block:: python

      world_type = ae.sdk.AEGP_GetLayerRenderOptionsWorldType(optionsH)
      if world_type == 3:
          print("32ビット浮動小数点モード")

ダウンサンプル係数設定
~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH: int, x: int, y: int) -> None

   ダウンサンプル係数を設定します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :param x: 水平方向のダウンサンプル係数（1以上）
   :type x: int
   :param y: 垂直方向のダウンサンプル係数（1以上）
   :type y: int

   .. note::
      **ダウンサンプル係数と解像度の関係**:

      - 1 = 100%（フル解像度）
      - 2 = 50%
      - 3 = 33.3%
      - 4 = 25%

   **例**:

   .. code-block:: python

      # 50%解像度でレンダリング
      ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH, 2, 2)

      # 水平25%、垂直50%でレンダリング
      ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH, 4, 2)

.. function:: AEGP_GetLayerRenderOptionsDownsampleFactor(optionsH: int) -> tuple[int, int]

   ダウンサンプル係数を取得します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :return: (x, y) ダウンサンプル係数のタプル
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      x, y = ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(optionsH)
      print(f"解像度: 水平{100/x}%, 垂直{100/y}%")

マットモード設定
~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetLayerRenderOptionsMatteMode(optionsH: int, mode: int) -> None

   マットモードを設定します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :param mode: マットモード（0-2）
   :type mode: int

   **マットモードの選択肢**:

   - **0**: AEGP_MatteMode_STRAIGHT（ストレートアルファ）
   - **1**: AEGP_MatteMode_PREMUL_BLACK（黒でプリマルチプライ）
   - **2**: AEGP_MatteMode_PREMUL_BG_COLOR（背景色でプリマルチプライ）

   **例**:

   .. code-block:: python

      # プリマルチプライドアルファ（黒背景）
      ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(optionsH, 1)

.. function:: AEGP_GetLayerRenderOptionsMatteMode(optionsH: int) -> int

   マットモードを取得します。

   :param optionsH: LayerRenderOptionsハンドル
   :type optionsH: int
   :return: マットモード（0-2）
   :rtype: int

   **戻り値**:

   - **0**: AEGP_MatteMode_STRAIGHT（ストレートアルファ）
   - **1**: AEGP_MatteMode_PREMUL_BLACK（黒でプリマルチプライ）
   - **2**: AEGP_MatteMode_PREMUL_BG_COLOR（背景色でプリマルチプライ）

   **例**:

   .. code-block:: python

      mode = ae.sdk.AEGP_GetLayerRenderOptionsMatteMode(optionsH)
      mode_names = ["ストレート", "プリマルチプライ（黒）", "プリマルチプライ（背景色）"]
      print(f"マットモード: {mode_names[mode]}")

使用例
------

基本的なレンダリングオプションの設定
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_layer_with_options(layerH, time_seconds):
       """レイヤーをカスタム設定でレンダリング"""
       # レンダリングオプションを作成
       optionsH = ae.sdk.AEGP_NewFromLayer(layerH)

       try:
           # レンダリング設定
           ae.sdk.AEGP_SetLayerRenderOptionsTime(optionsH, time_seconds)
           ae.sdk.AEGP_SetLayerRenderOptionsWorldType(optionsH, 3)  # 32ビット
           ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH, 1, 1)  # フル解像度
           ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(optionsH, 0)  # ストレートアルファ

           # ここでレンダリング処理（RenderSuiteを使用）
           # ...

       finally:
           # 必ずハンドルを解放
           ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

   # 使用例
   layerH = ae.layer.get_handle()
   render_layer_with_options(layerH, 5.0)

プレビュー品質でのクイックレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def quick_preview_render(layerH):
       """低解像度でクイックプレビューをレンダリング"""
       optionsH = ae.sdk.AEGP_NewFromLayer(layerH)

       try:
           # プレビュー設定（速度優先）
           ae.sdk.AEGP_SetLayerRenderOptionsWorldType(optionsH, 1)  # 8ビット
           ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH, 4, 4)  # 25%解像度

           # 現在の設定を確認
           x, y = ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(optionsH)
           print(f"プレビュー解像度: {100/x}%")

           # レンダリング処理
           # ...

       finally:
           ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

   layerH = ae.layer.get_handle()
   quick_preview_render(layerH)

エフェクト適用前後のレンダリング比較
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def compare_effect_result(effectH):
       """エフェクト適用前後のレイヤーをレンダリングして比較"""

       # エフェクト適用前のオプション
       before_optionsH = ae.sdk.AEGP_NewFromUpstreamOfEffect(effectH)
       # エフェクト適用後のオプション
       after_optionsH = ae.sdk.AEGP_NewFromDownstreamOfEffect(effectH)

       try:
           # 共通設定
           for optionsH in [before_optionsH, after_optionsH]:
               ae.sdk.AEGP_SetLayerRenderOptionsTime(optionsH, 2.0)
               ae.sdk.AEGP_SetLayerRenderOptionsWorldType(optionsH, 3)
               ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH, 1, 1)

           # エフェクト前のレンダリング
           # render_frame(before_optionsH) -> before_image

           # エフェクト後のレンダリング
           # render_frame(after_optionsH) -> after_image

           # 比較処理
           # ...

       finally:
           ae.sdk.AEGP_DisposeLayerRenderOptions(before_optionsH)
           ae.sdk.AEGP_DisposeLayerRenderOptions(after_optionsH)

   effectH = ae.effect.get_handle()
   compare_effect_result(effectH)

複数フレームの連続レンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_frame_sequence(layerH, start_time, end_time, fps=30.0):
       """指定範囲のフレームを連続レンダリング"""
       frame_duration = 1.0 / fps

       # 基本オプションを作成
       base_optionsH = ae.sdk.AEGP_NewFromLayer(layerH)

       try:
           # 高品質設定
           ae.sdk.AEGP_SetLayerRenderOptionsWorldType(base_optionsH, 3)  # 32ビット
           ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(base_optionsH, 1, 1)
           ae.sdk.AEGP_SetLayerRenderOptionsTimeStep(base_optionsH, frame_duration)
           ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(base_optionsH, 1)  # プリマルチプライ（黒）

           # 各フレームをレンダリング
           current_time = start_time
           frame_number = 0

           while current_time <= end_time:
               # オプションを複製（フレームごとに独立した設定）
               frame_optionsH = ae.sdk.AEGP_DuplicateLayerRenderOptions(base_optionsH)

               try:
                   # 時間を設定
                   ae.sdk.AEGP_SetLayerRenderOptionsTime(frame_optionsH, current_time)

                   # レンダリング処理
                   print(f"フレーム {frame_number}: {current_time:.3f}秒")
                   # render_frame(frame_optionsH) -> save as frame_{frame_number}.png

               finally:
                   ae.sdk.AEGP_DisposeLayerRenderOptions(frame_optionsH)

               current_time += frame_duration
               frame_number += 1

           print(f"合計 {frame_number} フレームをレンダリングしました")

       finally:
           ae.sdk.AEGP_DisposeLayerRenderOptions(base_optionsH)

   # 使用例: 0秒から5秒まで30fpsでレンダリング
   layerH = ae.layer.get_handle()
   render_frame_sequence(layerH, 0.0, 5.0, fps=30.0)

レンダリングオプションの設定ダンプ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_render_options(optionsH):
       """レンダリングオプションの現在の設定を表示"""
       # 時間設定
       time = ae.sdk.AEGP_GetLayerRenderOptionsTime(optionsH)
       time_step = ae.sdk.AEGP_GetLayerRenderOptionsTimeStep(optionsH)

       # ビット深度
       world_type = ae.sdk.AEGP_GetLayerRenderOptionsWorldType(optionsH)
       world_type_names = ["NONE", "8-bit", "16-bit", "32-bit float"]

       # ダウンサンプル係数
       downsample_x, downsample_y = ae.sdk.AEGP_GetLayerRenderOptionsDownsampleFactor(optionsH)

       # マットモード
       matte_mode = ae.sdk.AEGP_GetLayerRenderOptionsMatteMode(optionsH)
       matte_mode_names = ["ストレート", "プリマルチプライ（黒）", "プリマルチプライ（背景色）"]

       print("=" * 60)
       print("レンダリングオプション設定")
       print("=" * 60)
       print(f"時間: {time:.3f}秒")
       print(f"タイムステップ: {time_step:.6f}秒 (フレームレート: {1.0/time_step:.2f}fps)")
       print(f"ビット深度: {world_type_names[world_type]}")
       print(f"解像度: 水平{100/downsample_x:.1f}%, 垂直{100/downsample_y:.1f}%")
       print(f"マットモード: {matte_mode_names[matte_mode]}")
       print("=" * 60)

   # 使用例
   layerH = ae.layer.get_handle()
   optionsH = ae.sdk.AEGP_NewFromLayer(layerH)
   try:
       dump_render_options(optionsH)
   finally:
       ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

適応的品質レンダリング
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def adaptive_quality_render(layerH, time_seconds, preview_mode=False):
       """状況に応じて品質を調整してレンダリング"""
       optionsH = ae.sdk.AEGP_NewFromLayer(layerH)

       try:
           # 時間を設定
           ae.sdk.AEGP_SetLayerRenderOptionsTime(optionsH, time_seconds)

           if preview_mode:
               # プレビューモード: 速度優先
               print("プレビューモード: 速度優先設定")
               ae.sdk.AEGP_SetLayerRenderOptionsWorldType(optionsH, 1)  # 8ビット
               ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH, 2, 2)  # 50%
               ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(optionsH, 1)  # プリマルチプライ
           else:
               # 本番レンダリング: 品質優先
               print("本番レンダリング: 品質優先設定")
               ae.sdk.AEGP_SetLayerRenderOptionsWorldType(optionsH, 3)  # 32ビット
               ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH, 1, 1)  # フル解像度
               ae.sdk.AEGP_SetLayerRenderOptionsMatteMode(optionsH, 0)  # ストレート

           # 設定内容を確認
           dump_render_options(optionsH)

           # レンダリング処理
           # ...

       finally:
           ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

   # 使用例
   layerH = ae.layer.get_handle()
   adaptive_quality_render(layerH, 3.0, preview_mode=True)   # プレビュー
   adaptive_quality_render(layerH, 3.0, preview_mode=False)  # 本番

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリ管理は必須**

   ``AEGP_NewFromLayer``, ``AEGP_NewFromUpstreamOfEffect``, ``AEGP_NewFromDownstreamOfEffect``,
   ``AEGP_DuplicateLayerRenderOptions`` で取得したハンドルは **必ず** ``AEGP_DisposeLayerRenderOptions``
   で解放してください。解放を忘れるとメモリリークが発生します。

2. **UIスレッド制限**

   ``AEGP_NewFromDownstreamOfEffect`` は **UIスレッドからのみ** 呼び出し可能です。
   バックグラウンドレンダリングスレッドから呼び出すとエラーになります。

3. **ダウンサンプル係数は正の整数**

   ダウンサンプル係数は必ず1以上の値を指定してください。0以下の値はエラーになります。

4. **WorldTypeとMatteMode範囲**

   WorldTypeは0-3、MatteModeは0-2の範囲内で指定してください。範囲外の値はエラーになります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

try-finally でメモリ解放を保証
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   optionsH = ae.sdk.AEGP_NewFromLayer(layerH)
   try:
       # 処理
       pass
   finally:
       ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

設定の再利用
^^^^^^^^^^^^

.. code-block:: python

   # 基本設定を作成
   base_optionsH = ae.sdk.AEGP_NewFromLayer(layerH)
   try:
       # 共通設定
       ae.sdk.AEGP_SetLayerRenderOptionsWorldType(base_optionsH, 3)
       ae.sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(base_optionsH, 1, 1)

       # 複製して個別の時間で使用
       for time in [1.0, 2.0, 3.0]:
           optionsH = ae.sdk.AEGP_DuplicateLayerRenderOptions(base_optionsH)
           try:
               ae.sdk.AEGP_SetLayerRenderOptionsTime(optionsH, time)
               # レンダリング
           finally:
               ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)
   finally:
       ae.sdk.AEGP_DisposeLayerRenderOptions(base_optionsH)

設定値の検証
^^^^^^^^^^^^

.. code-block:: python

   # 設定後に確認
   ae.sdk.AEGP_SetLayerRenderOptionsWorldType(optionsH, 3)
   actual_type = ae.sdk.AEGP_GetLayerRenderOptionsWorldType(optionsH)
   assert actual_type == 3, "ビット深度の設定に失敗しました"

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       optionsH = ae.sdk.AEGP_NewFromLayer(layerH)
   except Exception as e:
       print(f"レンダリングオプションの作成に失敗: {e}")
       return

   try:
       # 処理
       pass
   finally:
       ae.sdk.AEGP_DisposeLayerRenderOptions(optionsH)

関連項目
--------

- :doc:`AEGP_RenderSuite5` - レンダリング実行（LayerRenderOptionsを使用）
- :doc:`AEGP_RenderOptionsSuite4` - コンポジションレベルのレンダリングオプション
- :doc:`AEGP_LayerSuite9` - レイヤー管理
- :doc:`AEGP_EffectSuite5` - エフェクト管理
- :doc:`index` - 低レベルAPI概要
