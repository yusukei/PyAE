Render
======

.. currentmodule:: ae

フレームレンダリングを管理するAPIです。

概要
----

レンダリングAPI群は、コンポジションやレイヤーのフレームをプログラムから
レンダリングするための機能を提供します。

- ``RenderOptions`` - コンポジションのレンダリング設定
- ``FrameReceipt`` - レンダリング結果の管理
- ``Renderer`` - レンダリング実行の静的メソッド

クラスリファレンス
------------------

RenderOptions
~~~~~~~~~~~~~

.. autoclass:: RenderOptions
   :members:
   :undoc-members:
   :show-inheritance:

FrameReceipt
~~~~~~~~~~~~

.. autoclass:: FrameReceipt
   :members:
   :undoc-members:
   :show-inheritance:

Renderer
~~~~~~~~

.. autoclass:: Renderer
   :members:
   :undoc-members:

列挙型
------

MatteMode
~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 30 10 60

   * - 値
     - 数値
     - 説明
   * - ``STRAIGHT``
     - 0
     - ストレートアルファ
   * - ``PREMUL_BLACK``
     - 1
     - プリマルチプライド（黒）
   * - ``PREMUL_BG_COLOR``
     - 2
     - プリマルチプライド（背景色）

ChannelOrder
~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 25 10 65

   * - 値
     - 数値
     - 説明
   * - ``ARGB``
     - 0
     - Alpha, Red, Green, Blue
   * - ``BGRA``
     - 1
     - Blue, Green, Red, Alpha

FieldRender
~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 25 10 65

   * - 値
     - 数値
     - 説明
   * - ``FRAME``
     - 0
     - フレーム（フィールドなし）
   * - ``UPPER``
     - 1
     - 上位フィールド
   * - ``LOWER``
     - 2
     - 下位フィールド

RenderQuality
~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 25 10 65

   * - 値
     - 数値
     - 説明
   * - ``DRAFT``
     - 0
     - ドラフト品質（高速）
   * - ``BEST``
     - 1
     - 最高品質

基本的な使用方法
----------------

コンポジションのレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # アクティブなコンポジションを取得
   comp = ae.Comp.get_active()

   # レンダリングオプションを作成
   options = ae.RenderOptions.from_item(comp._handle)

   # 設定
   options.time = 2.5  # 2.5秒の位置
   options.world_type = ae.WorldType.BIT8  # 8ビット

   # レンダリング実行
   receipt = ae.Renderer.render_frame(options)

   if receipt and receipt.valid:
       # 結果を取得
       world = receipt.world
       print(f"Rendered: {world.width}x{world.height}")

       # チェックイン（リソース解放）
       receipt.checkin()

コンテキストマネージャの使用
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   options = ae.RenderOptions.from_item(comp._handle)
   options.time = 1.0

   # withステートメントで自動チェックイン
   with ae.Renderer.render_frame(options) as receipt:
       world = receipt.world
       pixels = world.get_pixels()
       print(f"Got {len(pixels)} bytes")
   # 自動的にcheckin()が呼ばれる

RenderOptions プロパティ
------------------------

.. list-table::
   :header-rows: 1
   :widths: 25 20 55

   * - プロパティ
     - 型
     - 説明
   * - ``time``
     - float
     - レンダリング時間（秒）
   * - ``time_step``
     - float
     - タイムステップ（秒）
   * - ``world_type``
     - WorldType
     - ビット深度
   * - ``field_render``
     - FieldRender
     - フィールドレンダリング
   * - ``downsample_factor``
     - Tuple[int, int]
     - ダウンサンプル係数
   * - ``region_of_interest``
     - Dict
     - 関心領域（ROI）
   * - ``matte_mode``
     - MatteMode
     - マットモード
   * - ``channel_order``
     - ChannelOrder
     - チャンネル順序
   * - ``render_guide_layers``
     - bool
     - ガイドレイヤーをレンダリング
   * - ``render_quality``
     - RenderQuality
     - レンダリング品質

FrameReceipt プロパティ
-----------------------

.. list-table::
   :header-rows: 1
   :widths: 25 15 60

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - レシートが有効か
   * - ``world``
     - World
     - レンダリング結果のフレームバッファ
   * - ``rendered_region``
     - Dict
     - 実際にレンダリングされた領域
   * - ``guid``
     - str
     - フレームのGUID

Renderer 静的メソッド
---------------------

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``render_frame(options)``
     - RenderOptionsでフレームをレンダリング
   * - ``render_layer_frame(layer_options_handle)``
     - LayerRenderOptionsでレイヤーフレームをレンダリング
   * - ``get_current_timestamp()``
     - 現在のプロジェクトタイムスタンプを取得
   * - ``has_item_changed(...)``
     - アイテムが変更されたかチェック
   * - ``is_item_worthwhile_to_render(...)``
     - レンダリングする価値があるかチェック

実用的な使用例
--------------

高品質レンダリング
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_high_quality(comp, time_seconds):
       """高品質でフレームをレンダリング"""
       options = ae.RenderOptions.from_item(comp._handle)

       # 高品質設定
       options.time = time_seconds
       options.world_type = ae.WorldType.BIT16  # 16ビット
       options.render_quality = ae.RenderQuality.BEST
       options.matte_mode = ae.MatteMode.STRAIGHT
       options.downsample_factor = (1, 1)  # フル解像度

       with ae.Renderer.render_frame(options) as receipt:
           if receipt and receipt.valid:
               return receipt.world.get_pixels()
       return None

   # 使用例
   comp = ae.Comp.get_active()
   pixels = render_high_quality(comp, 5.0)

プレビュー用の高速レンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_preview(comp, time_seconds):
       """プレビュー用に高速レンダリング"""
       options = ae.RenderOptions.from_item(comp._handle)

       # 高速設定
       options.time = time_seconds
       options.world_type = ae.WorldType.BIT8  # 8ビット
       options.render_quality = ae.RenderQuality.DRAFT
       options.downsample_factor = (2, 2)  # 50%解像度

       with ae.Renderer.render_frame(options) as receipt:
           if receipt and receipt.valid:
               world = receipt.world
               return {
                   'width': world.width,
                   'height': world.height,
                   'pixels': world.get_pixels()
               }
       return None

ROI（関心領域）レンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_roi(comp, time_seconds, left, top, right, bottom):
       """指定領域のみレンダリング"""
       options = ae.RenderOptions.from_item(comp._handle)
       options.time = time_seconds

       # ROIを設定（プロパティで代入）
       options.region_of_interest = {"left": left, "top": top, "right": right, "bottom": bottom}

       with ae.Renderer.render_frame(options) as receipt:
           if receipt and receipt.valid:
               region = receipt.rendered_region
               print(f"Rendered region: {region}")
               return receipt.world.get_pixels()
       return None

   # 使用例：中央の400x300ピクセルをレンダリング
   comp = ae.Comp.get_active()
   pixels = render_roi(comp, 0.0, 760, 390, 1160, 690)

フレームシーケンスのレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_sequence(comp, start_frame, end_frame, fps=30.0):
       """フレームシーケンスをレンダリング"""
       frames = []

       for frame in range(start_frame, end_frame + 1):
           time_seconds = frame / fps

           options = ae.RenderOptions.from_item(comp._handle)
           options.time = time_seconds

           with ae.Renderer.render_frame(options) as receipt:
               if receipt and receipt.valid:
                   frames.append({
                       'frame': frame,
                       'time': time_seconds,
                       'guid': receipt.guid,
                       'pixels': receipt.world.get_pixels()
                   })

           print(f"Rendered frame {frame}/{end_frame}")

       return frames

   # 使用例
   comp = ae.Comp.get_active()
   frames = render_sequence(comp, 0, 29)  # 1秒分（30fps）

キャッシュ検証
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_if_changed(comp, options, last_timestamp):
       """アイテムが変更された場合のみレンダリング"""
       current_timestamp = ae.Renderer.get_current_timestamp()

       # 変更チェック
       changed = ae.Renderer.has_item_changed_since_timestamp(
           comp._handle,
           options.time,
           0.0,  # duration
           last_timestamp
       )

       if changed:
           print("Item changed, rendering...")
           with ae.Renderer.render_frame(options) as receipt:
               if receipt and receipt.valid:
                   return receipt.world.get_pixels(), current_timestamp
       else:
           print("No changes, using cached result")

       return None, current_timestamp

オプションの複製と比較
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def is_rerender_needed(original_options, new_options):
       """再レンダリングが必要かチェック"""
       return not ae.Renderer.is_rendered_frame_sufficient(
           original_options,
           new_options
       )

   # 使用例
   comp = ae.Comp.get_active()
   options1 = ae.RenderOptions.from_item(comp._handle)
   options2 = options1.duplicate()

   # options2を変更
   options2.downsample_factor = (2, 2)

   if is_rerender_needed(options1, options2):
       print("Re-render needed")

注意事項
--------

.. note::
   - ``FrameReceipt`` は使用後に必ず ``checkin()`` を呼ぶか、
     ``with`` ステートメントを使用してください
   - ``world`` プロパティから取得した ``World`` は読み取り専用です
   - レンダリングはメインスレッドで実行する必要があります

.. warning::
   - 無効なオプションやレシートで操作するとエラーが発生します
   - 大量のフレームを連続レンダリングするとメモリを消費します
   - チェックイン後の ``FrameReceipt`` にアクセスしないでください

関連項目
--------

- :class:`LayerRenderOptions` - レイヤーレンダリング設定
- :class:`World` - フレームバッファ
- :class:`WorldType` - ビット深度
- :class:`SoundData` - サウンドデータ
- :class:`Comp` - コンポジション
- :doc:`/api/low-level/AEGP_RenderOptionsSuite4` - 低レベルRenderOptions API
- :doc:`/api/low-level/AEGP_RenderSuite5` - 低レベルRender API
