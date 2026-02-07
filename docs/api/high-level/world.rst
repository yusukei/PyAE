World
=====

.. currentmodule:: ae

フレームバッファ（画像データ）を管理するAPIです。

概要
----

``World`` クラスは、After Effectsのフレームバッファを表します。
レンダリング結果の取得、ピクセルデータの読み書きに使用します。

クラスリファレンス
------------------

.. autoclass:: World
   :members:
   :undoc-members:
   :show-inheritance:

.. autoclass:: WorldType
   :members:
   :undoc-members:

基本的な使用方法
----------------

レンダリング結果からの取得
~~~~~~~~~~~~~~~~~~~~~~~~~~

通常、``World`` オブジェクトは ``FrameReceipt`` から取得します。

.. code-block:: python

   import ae

   # コンポジションのレンダリング
   comp = ae.Comp.get_active()
   options = ae.RenderOptions.from_item(comp._handle)
   options.time = 2.5

   with ae.Renderer.render_frame(options) as receipt:
       # ワールドを取得
       world = receipt.world

       # サイズ情報
       print(f"Size: {world.width}x{world.height}")
       print(f"Type: {world.type}")

       # ピクセルデータを取得
       pixels = world.get_pixels()
       print(f"Data size: {len(pixels)} bytes")

新規ワールドの作成
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # 新しいワールドを作成
   world = ae.World.create(
       type=ae.WorldType.BIT8,
       width=1920,
       height=1080
   )

   print(f"Created: {world.width}x{world.height}")

WorldType 列挙型
----------------

.. list-table::
   :header-rows: 1
   :widths: 25 10 65

   * - 値
     - 数値
     - 説明
   * - ``NONE``
     - 0
     - 未定義
   * - ``BIT8``
     - 1
     - 8ビット（チャンネルあたり8ビット、ARGB各8ビット）
   * - ``BIT16``
     - 2
     - 16ビット（チャンネルあたり16ビット）
   * - ``BIT32``
     - 3
     - 32ビット浮動小数点

プロパティ
----------

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - ワールドが有効かどうか
   * - ``type``
     - WorldType
     - ビット深度タイプ
   * - ``width``
     - int
     - 幅（ピクセル）
   * - ``height``
     - int
     - 高さ（ピクセル）
   * - ``size``
     - Tuple[int, int]
     - (width, height) タプル
   * - ``row_bytes``
     - int
     - 1行あたりのバイト数

メソッド
--------

get_pixels()
~~~~~~~~~~~~

全ピクセルデータをバイト列として取得します。

.. code-block:: python

   pixels = world.get_pixels()
   print(f"Total bytes: {len(pixels)}")

.. note::
   - 8ビットワールド: ARGB各8ビット（4バイト/ピクセル）
   - 16ビットワールド: ARGB各16ビット（8バイト/ピクセル）
   - 32ビットワールド: ARGB各32ビットfloat（16バイト/ピクセル）

set_pixels(data)
~~~~~~~~~~~~~~~~

全ピクセルデータをバイト列から設定します。

.. code-block:: python

   # ピクセルデータを設定
   world.set_pixels(new_pixel_data)

get_pixel(x, y)
~~~~~~~~~~~~~~~

指定座標のピクセル値を取得します。

.. code-block:: python

   # (100, 50)のピクセルを取得
   r, g, b, a = world.get_pixel(100, 50)
   print(f"RGBA: ({r:.2f}, {g:.2f}, {b:.2f}, {a:.2f})")

.. note::
   値は0.0〜1.0の正規化された浮動小数点で返されます。

set_pixel(x, y, r, g, b, a)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

指定座標のピクセル値を設定します。

.. code-block:: python

   # (100, 50)のピクセルを赤に設定
   world.set_pixel(100, 50, 1.0, 0.0, 0.0, 1.0)

fast_blur(radius, flags=0, quality=1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

高速ブラー処理を適用します。

.. code-block:: python

   # 半径10ピクセルでブラー
   world.fast_blur(radius=10.0)

   # 品質設定付き
   world.fast_blur(radius=5.0, quality=2)

実用的な使用例
--------------

フレームをファイルに保存
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def save_frame_as_raw(world, filepath):
       """フレームをRAWファイルとして保存"""
       pixels = world.get_pixels()

       with open(filepath, 'wb') as f:
           # ヘッダー情報
           f.write(world.width.to_bytes(4, 'little'))
           f.write(world.height.to_bytes(4, 'little'))
           f.write(int(world.type).to_bytes(4, 'little'))
           # ピクセルデータ
           f.write(pixels)

       print(f"Saved: {filepath} ({len(pixels)} bytes)")

   # 使用例
   comp = ae.Comp.get_active()
   options = ae.RenderOptions.from_item(comp._handle)

   with ae.Renderer.render_frame(options) as receipt:
       save_frame_as_raw(receipt.world, "frame.raw")

ピクセル統計の計算
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import struct

   def analyze_8bit_frame(world):
       """8ビットフレームの統計を計算"""
       if world.type != ae.WorldType.BIT8:
           print("8ビットワールドのみ対応")
           return

       pixels = world.get_pixels()
       total_pixels = world.width * world.height

       # ARGB8の場合、4バイト/ピクセル
       r_sum, g_sum, b_sum, a_sum = 0, 0, 0, 0

       for i in range(total_pixels):
           offset = i * 4
           a = pixels[offset]
           r = pixels[offset + 1]
           g = pixels[offset + 2]
           b = pixels[offset + 3]

           a_sum += a
           r_sum += r
           g_sum += g
           b_sum += b

       # 平均値（0-255）
       avg_r = r_sum / total_pixels
       avg_g = g_sum / total_pixels
       avg_b = b_sum / total_pixels
       avg_a = a_sum / total_pixels

       print(f"Average RGBA: ({avg_r:.1f}, {avg_g:.1f}, {avg_b:.1f}, {avg_a:.1f})")

連続フレームのレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def render_frame_sequence(comp, start_frame, end_frame, fps=30.0):
       """フレームシーケンスをレンダリング"""
       frames = []

       for frame in range(start_frame, end_frame + 1):
           time_seconds = frame / fps

           options = ae.RenderOptions.from_item(comp._handle)
           options.time = time_seconds
           options.world_type = ae.WorldType.BIT8

           with ae.Renderer.render_frame(options) as receipt:
               if receipt and receipt.valid:
                   pixels = receipt.world.get_pixels()
                   frames.append({
                       'frame': frame,
                       'time': time_seconds,
                       'width': receipt.world.width,
                       'height': receipt.world.height,
                       'pixels': pixels
                   })
                   print(f"Rendered frame {frame}")

       return frames

   # 使用例
   comp = ae.Comp.get_active()
   frames = render_frame_sequence(comp, 0, 29)  # 0-29フレーム

ビット深度変換の参考
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # 8ビットでレンダリング（高速）
   options = ae.RenderOptions.from_item(comp._handle)
   options.world_type = ae.WorldType.BIT8

   with ae.Renderer.render_frame(options) as receipt:
       world_8bit = receipt.world
       print(f"8bit: {world_8bit.row_bytes} bytes/row")

   # 16ビットでレンダリング（高品質）
   options.world_type = ae.WorldType.BIT16

   with ae.Renderer.render_frame(options) as receipt:
       world_16bit = receipt.world
       print(f"16bit: {world_16bit.row_bytes} bytes/row")

   # 32ビットfloatでレンダリング（最高品質）
   options.world_type = ae.WorldType.BIT32

   with ae.Renderer.render_frame(options) as receipt:
       world_32bit = receipt.world
       print(f"32bit: {world_32bit.row_bytes} bytes/row")

注意事項
--------

.. note::
   - ``FrameReceipt`` から取得した ``World`` は読み取り専用です
   - ``World.create()`` で作成したワールドは読み書き可能です
   - ピクセルデータのフォーマットはビット深度によって異なります

.. warning::
   - 無効なワールドで操作するとエラーが発生します
   - 大きな画像はメモリを大量に消費します
   - ``FrameReceipt`` のチェックイン後は ``World`` にアクセスしないでください

関連項目
--------

- :class:`RenderOptions` - レンダリング設定
- :class:`FrameReceipt` - レンダリング結果
- :class:`Renderer` - レンダリング実行
- :class:`LayerRenderOptions` - レイヤーレンダリング設定
- :doc:`/api/low-level/AEGP_WorldSuite3` - 低レベルAPI
