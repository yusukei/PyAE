Layer Render Options
====================

.. currentmodule:: ae

レイヤー単位のレンダリング設定を管理するAPIです。

概要
----

``LayerRenderOptions`` は、個別のレイヤーをレンダリングするための設定を管理します。
コンポジション全体をレンダリングする ``RenderOptions`` とは異なり、
特定のレイヤーやエフェクトの入出力をレンダリングする際に使用します。

クラスリファレンス
------------------

.. autoclass:: LayerRenderOptions
   :members:
   :undoc-members:
   :show-inheritance:

基本的な使用方法
----------------

レイヤーからオプションを作成
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # アクティブなコンポジションを取得
   comp = ae.Comp.get_active()
   if comp:
       # 最初のレイヤーを取得
       layer = comp.layer(1)

       # レイヤーからレンダリングオプションを作成
       options = ae.LayerRenderOptions.from_layer(layer._handle)

       # 設定を確認
       print(f"Time: {options.time}")
       print(f"Time Step: {options.time_step}")
       print(f"World Type: {options.world_type}")

レンダリング設定の変更
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layer(1)
   options = ae.LayerRenderOptions.from_layer(layer._handle)

   # 時間設定
   options.time = 2.5  # 2.5秒の位置
   options.time_step = 1.0 / 30.0  # 30fpsのフレーム長

   # ビット深度の設定
   options.world_type = ae.WorldType.BIT16  # 16ビット

   # ダウンサンプリング
   options.downsample_factor = (2, 2)  # 50%解像度

   # マットモード
   options.matte_mode = ae.MatteMode.STRAIGHT  # ストレートアルファ

エフェクトの入出力からオプションを作成
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layer(1)

   # レイヤーの最初のエフェクトを取得
   effect = layer.effect(1)

   if effect:
       # エフェクトの上流（入力側）からオプションを作成
       upstream_options = ae.LayerRenderOptions.from_upstream_of_effect(
           effect._handle)

       # エフェクトの下流（出力側）からオプションを作成
       # 注意: UIスレッドからのみ呼び出し可能
       downstream_options = ae.LayerRenderOptions.from_downstream_of_effect(
           effect._handle)

プロパティ
----------

.. list-table::
   :header-rows: 1
   :widths: 25 15 60

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - オプションが有効かどうか（読み取り専用）
   * - ``time``
     - float
     - レンダリング時間（秒）
   * - ``time_step``
     - float
     - タイムステップ（秒）、モーションブラーに影響
   * - ``world_type``
     - WorldType
     - ビット深度（NONE, BIT8, BIT16, BIT32）
   * - ``downsample_factor``
     - Tuple[int, int]
     - ダウンサンプル係数 (x, y)
   * - ``matte_mode``
     - MatteMode
     - マットモード

ダウンサンプル係数
~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - 値
     - 解像度
   * - (1, 1)
     - 100%（フル解像度）
   * - (2, 2)
     - 50%
   * - (3, 3)
     - 33.3%
   * - (4, 4)
     - 25%

メソッド
--------

duplicate()
~~~~~~~~~~~

オプションを複製します。

.. code-block:: python

   # オプションを複製
   options_copy = options.duplicate()

   # 複製したオプションを変更（元のオプションには影響しない）
   options_copy.time = 5.0

ファクトリメソッド
------------------

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - メソッド
     - 説明
   * - ``from_layer(layer_handle)``
     - レイヤーからオプションを作成
   * - ``from_upstream_of_effect(effect_handle)``
     - エフェクトの上流からオプションを作成
   * - ``from_downstream_of_effect(effect_handle)``
     - エフェクトの下流からオプションを作成（UIスレッドのみ）

実用的な使用例
--------------

レイヤーフレームのレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_layer_frame(layer, time_seconds, bit_depth=ae.WorldType.BIT8):
       """レイヤーの特定フレームをレンダリング"""

       # レンダリングオプションを作成
       options = ae.LayerRenderOptions.from_layer(layer._handle)
       options.time = time_seconds
       options.world_type = bit_depth

       # レンダリングを実行
       receipt = ae.Renderer.render_layer_frame(options._handle)

       if receipt and receipt.valid:
           # レンダリング結果を取得
           world = receipt.world
           print(f"Size: {world.width}x{world.height}")

           # ピクセルデータを取得
           pixels = world.get_pixels()
           print(f"Data size: {len(pixels)} bytes")

           # チェックイン
           receipt.checkin()
           return pixels

       return None

   # 使用例
   comp = ae.Comp.get_active()
   layer = comp.layer(1)
   pixels = render_layer_frame(layer, 2.5, ae.WorldType.BIT16)

エフェクト処理前後の比較
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def compare_effect_result(layer, effect_index, time_seconds):
       """エフェクト適用前後のフレームを比較"""

       effect = layer.effect(effect_index)
       if not effect:
           return None, None

       # エフェクト前（上流）
       before_options = ae.LayerRenderOptions.from_upstream_of_effect(
           effect._handle)
       before_options.time = time_seconds
       before_receipt = ae.Renderer.render_layer_frame(before_options._handle)

       # エフェクト後（下流）- UIスレッドで実行
       after_options = ae.LayerRenderOptions.from_downstream_of_effect(
           effect._handle)
       after_options.time = time_seconds
       after_receipt = ae.Renderer.render_layer_frame(after_options._handle)

       before_pixels = before_receipt.world.get_pixels() if before_receipt else None
       after_pixels = after_receipt.world.get_pixels() if after_receipt else None

       # チェックイン
       if before_receipt:
           before_receipt.checkin()
       if after_receipt:
           after_receipt.checkin()

       return before_pixels, after_pixels

注意事項
--------

.. note::
   - ``from_downstream_of_effect()`` はUIスレッドからのみ呼び出し可能です
   - ``_handle`` プロパティは内部使用のみを想定しています
   - 作成したオプションは自動的に管理されますが、パフォーマンス上の理由から
     使用後は早めに破棄することを推奨します

.. warning::
   - 無効なハンドルでメソッドを呼び出すとエラーが発生します
   - 操作前に ``valid`` プロパティを確認してください

関連項目
--------

- :class:`RenderOptions` - コンポジションレンダリング設定
- :class:`FrameReceipt` - レンダリング結果
- :class:`Renderer` - レンダリング実行
- :class:`World` - フレームバッファ
- :class:`WorldType` - ビット深度列挙型
- :class:`MatteMode` - マットモード列挙型
- :doc:`/api/low-level/AEGP_LayerRenderOptionsSuite2` - 低レベルAPI
