AEGP_WorldSuite3
================

.. currentmodule:: ae.sdk

AEGP_WorldSuite3は、After Effectsのワールド（画像バッファ）を作成、操作、管理するためのSDK APIです。

概要
----

**実装状況**: 13/13関数実装 ✅

AEGP_WorldSuite3は以下の機能を提供します:

- ワールド（フレームバッファ）の作成と破棄
- ピクセルデータへの直接アクセス（8/16/32ビット）
- ワールド情報の取得（サイズ、型、行バイト数）
- 高速ブラー処理
- プラットフォーム固有のワールド管理

基本概念
--------

ワールド (World)
~~~~~~~~~~~~~~~~

ワールドは、After Effectsにおける画像バッファの基本単位です。レイヤーのフレーム、エフェクトの入出力、レンダリング結果などはすべてワールドとして扱われます。

.. important::
   - ワールドは使用後に必ず ``AEGP_DisposeWorld()`` で破棄する必要があります
   - メモリリークを防ぐため、確実にリソース管理を行ってください
   - Python の ``try-finally`` ブロックを使用してクリーンアップを保証することを推奨します

ワールドハンドル (AEGP_WorldH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ワールドを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

ワールドタイプ
~~~~~~~~~~~~~~

ワールドは以下のピクセル形式をサポートします:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
     - 1ピクセルあたりのバイト数
   * - ``AEGP_WorldType_NONE``
     - 0
     - 未定義（通常は使用されない）
     - N/A
   * - ``AEGP_WorldType_8``
     - 1
     - 8ビット/チャンネル（0-255）
     - 4バイト (RGBA)
   * - ``AEGP_WorldType_16``
     - 2
     - 16ビット/チャンネル（0-32768）
     - 8バイト (RGBA)
   * - ``AEGP_WorldType_32``
     - 3
     - 32ビット浮動小数点/チャンネル
     - 16バイト (RGBA)

.. tip::
   - **8ビット**: メモリ効率が良く、標準的な画像処理に最適
   - **16ビット**: 色のバンディングを防ぎ、高品質な処理が可能
   - **32ビット**: HDR、カラーグレーディング、VFX作業に必要

ピクセルフォーマット
~~~~~~~~~~~~~~~~~~~~

各ワールドタイプは異なるピクセル構造を持ちます:

**8ビット**: ``PF_Pixel8`` (4バイト)

.. code-block:: c

   struct PF_Pixel8 {
       A_u_char alpha, red, green, blue;  // 各0-255
   };

**16ビット**: ``PF_Pixel16`` (8バイト)

.. code-block:: c

   struct PF_Pixel16 {
       A_u_short alpha, red, green, blue;  // 各0-32768
   };

**32ビット**: ``PF_PixelFloat`` (16バイト)

.. code-block:: c

   struct PF_PixelFloat {
       PF_FpShort alpha, red, green, blue;  // 各float (通常0.0-1.0、HDRでは範囲外も可能)
   };

.. note::
   - すべてのピクセル形式はプリマルチプライドアルファ（alpha premultiplied）を使用します
   - カラー値はアルファ値で事前に乗算されています: ``color = color * alpha``

行バイト数 (Row Bytes)
~~~~~~~~~~~~~~~~~~~~~~~

ワールドのメモリレイアウトでは、各行の開始位置が特定のバイト境界にアライメントされます。

.. code-block:: python

   # 行バイト数の計算例（実際の値は AEGP_GetWorldRowBytes() で取得）
   width = 1920
   bytes_per_pixel = 4  # 8ビット RGBA
   row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)  # 実際の値（アライメントを含む）

   # row_bytes は width * bytes_per_pixel 以上になる場合がある

プラットフォームワールド (Platform World)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

プラットフォーム固有の最適化されたワールド形式。通常のワールドと異なり、GPU処理や特殊なメモリ配置に対応しています。

API リファレンス
----------------

ワールドの作成と破棄
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NewWorld(plugin_id: int, world_type: int, width: int, height: int) -> int

   新しいワールド（フレームバッファ）を作成します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param world_type: ワールドタイプ（``AEGP_WorldType_8``, ``AEGP_WorldType_16``, ``AEGP_WorldType_32``）
   :type world_type: int
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int
   :return: ワールドハンドル
   :rtype: int
   :raises ValueError: world_type が無効、width/height が0以下、またはサイズが30000x30000を超える場合
   :raises RuntimeError: ワールド作成に失敗した場合（メモリ不足など）

   .. note::
      作成されたワールドは使用後に必ず ``AEGP_DisposeWorld()`` で破棄してください。

   .. warning::
      最大サイズは 30000x30000 ピクセルです。これを超えるとエラーになります。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()

      # 1920x1080の8ビットワールドを作成
      worldH = ae.sdk.AEGP_NewWorld(
          plugin_id,
          ae.sdk.AEGP_WorldType_8,
          1920,
          1080
      )

      try:
          # ワールドを使用した処理
          pass
      finally:
          # 必ずクリーンアップ
          ae.sdk.AEGP_DisposeWorld(worldH)

.. function:: AEGP_DisposeWorld(worldH: int) -> None

   ワールドを破棄し、リソースを解放します。

   :param worldH: ワールドハンドル
   :type worldH: int
   :raises ValueError: worldH が null の場合
   :raises RuntimeError: ワールド破棄に失敗した場合

   .. danger::
      破棄後のワールドハンドルは無効になります。再利用しないでください。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DisposeWorld(worldH)

ワールド情報の取得
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetWorldType(worldH: int) -> int

   ワールドのピクセルタイプを取得します。

   :param worldH: ワールドハンドル
   :type worldH: int
   :return: ワールドタイプ（``AEGP_WorldType_8``, ``AEGP_WorldType_16``, ``AEGP_WorldType_32``）
   :rtype: int
   :raises ValueError: worldH が null の場合
   :raises RuntimeError: 取得に失敗した場合

   **例**:

   .. code-block:: python

      world_type = ae.sdk.AEGP_GetWorldType(worldH)

      if world_type == ae.sdk.AEGP_WorldType_8:
          print("8ビットワールド")
      elif world_type == ae.sdk.AEGP_WorldType_16:
          print("16ビットワールド")
      elif world_type == ae.sdk.AEGP_WorldType_32:
          print("32ビット浮動小数点ワールド")

.. function:: AEGP_GetWorldSize(worldH: int) -> tuple[int, int]

   ワールドの幅と高さを取得します。

   :param worldH: ワールドハンドル
   :type worldH: int
   :return: ``(width, height)`` のタプル
   :rtype: tuple[int, int]
   :raises ValueError: worldH が null の場合
   :raises RuntimeError: 取得に失敗した場合

   **例**:

   .. code-block:: python

      width, height = ae.sdk.AEGP_GetWorldSize(worldH)
      print(f"ワールドサイズ: {width}x{height}")

.. function:: AEGP_GetWorldRowBytes(worldH: int) -> int

   ワールドの1行あたりのバイト数（ストライド）を取得します。

   :param worldH: ワールドハンドル
   :type worldH: int
   :return: 1行あたりのバイト数
   :rtype: int
   :raises ValueError: worldH が null の場合
   :raises RuntimeError: 取得に失敗した場合

   .. note::
      行バイト数は ``width * bytes_per_pixel`` より大きい場合があります。
      これはメモリアライメントの最適化によるものです。

   **例**:

   .. code-block:: python

      row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)
      width, height = ae.sdk.AEGP_GetWorldSize(worldH)

      bytes_per_pixel = 4  # 8ビットの場合
      print(f"行バイト数: {row_bytes}")
      print(f"最小行バイト数: {width * bytes_per_pixel}")

ピクセルデータへのアクセス
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetWorldBaseAddr8(worldH: int) -> int

   8ビットワールドのピクセルデータのベースアドレスを取得します。

   :param worldH: ワールドハンドル（``AEGP_WorldType_8`` でなければならない）
   :type worldH: int
   :return: ピクセルデータのメモリアドレス（ポインタ値）
   :rtype: int
   :raises ValueError: worldH が null の場合
   :raises RuntimeError: ワールドタイプが ``AEGP_WorldType_8`` でない場合

   .. warning::
      このAPIは低レベルメモリアクセスのため、Python側では通常使用しません。
      NumPy、PIL、OpenCVなどの画像処理ライブラリと連携する場合に使用します。

   **例**:

   .. code-block:: python

      # 8ビットワールドのピクセルデータアドレスを取得
      base_addr = ae.sdk.AEGP_GetWorldBaseAddr8(worldH)

      # NumPyと連携する例（高度な使用法）
      import numpy as np
      import ctypes

      width, height = ae.sdk.AEGP_GetWorldSize(worldH)
      row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)

      # メモリから配列を作成（危険：アドレスが有効な間のみ使用可能）
      ptr = ctypes.cast(base_addr, ctypes.POINTER(ctypes.c_uint8))
      buffer = np.ctypeslib.as_array(ptr, shape=(height * row_bytes,))

      # RGBAとして整形（ストライドを考慮）
      pixels = np.zeros((height, width, 4), dtype=np.uint8)
      for y in range(height):
          row_start = y * row_bytes
          row_data = buffer[row_start:row_start + width * 4]
          pixels[y] = row_data.reshape(width, 4)

.. function:: AEGP_GetWorldBaseAddr16(worldH: int) -> int

   16ビットワールドのピクセルデータのベースアドレスを取得します。

   :param worldH: ワールドハンドル（``AEGP_WorldType_16`` でなければならない）
   :type worldH: int
   :return: ピクセルデータのメモリアドレス（ポインタ値）
   :rtype: int
   :raises ValueError: worldH が null の場合
   :raises RuntimeError: ワールドタイプが ``AEGP_WorldType_16`` でない場合

   **例**:

   .. code-block:: python

      base_addr = ae.sdk.AEGP_GetWorldBaseAddr16(worldH)

.. function:: AEGP_GetWorldBaseAddr32(worldH: int) -> int

   32ビット浮動小数点ワールドのピクセルデータのベースアドレスを取得します。

   :param worldH: ワールドハンドル（``AEGP_WorldType_32`` でなければならない）
   :type worldH: int
   :return: ピクセルデータのメモリアドレス（ポインタ値）
   :rtype: int
   :raises ValueError: worldH が null の場合
   :raises RuntimeError: ワールドタイプが ``AEGP_WorldType_32`` でない場合

   **例**:

   .. code-block:: python

      base_addr = ae.sdk.AEGP_GetWorldBaseAddr32(worldH)

PF_EffectWorld 構造体
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_FillOutPFEffectWorld(worldH: int) -> dict

   ワールドから PF_EffectWorld 構造体の情報を取得します。

   :param worldH: ワールドハンドル
   :type worldH: int
   :return: PF_EffectWorld 構造体の情報を含む辞書
   :rtype: dict
   :raises ValueError: worldH が null の場合
   :raises RuntimeError: 取得に失敗した場合

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``width``
        - int
        - 幅（ピクセル）
      * - ``height``
        - int
        - 高さ（ピクセル）
      * - ``rowbytes``
        - int
        - 1行あたりのバイト数
      * - ``world_flags``
        - int
        - ワールドフラグ
      * - ``data``
        - int
        - ピクセルデータのアドレス（ポインタ値）

   **例**:

   .. code-block:: python

      pf_world = ae.sdk.AEGP_FillOutPFEffectWorld(worldH)

      print(f"サイズ: {pf_world['width']}x{pf_world['height']}")
      print(f"行バイト数: {pf_world['rowbytes']}")
      print(f"データアドレス: 0x{pf_world['data']:X}")

ワールド処理
~~~~~~~~~~~~

.. function:: AEGP_FastBlur(radius: float, mode_flags: int, quality: int, worldH: int) -> None

   ワールドに高速ブラー処理を適用します。

   :param radius: ブラー半径（ピクセル、0.0以上）
   :type radius: float
   :param mode_flags: アルファ処理モード（``PF_MF_Alpha_PREMUL`` または ``PF_MF_Alpha_STRAIGHT``）
   :type mode_flags: int
   :param quality: 品質（``PF_Quality_LO`` または ``PF_Quality_HI``）
   :type quality: int
   :param worldH: ワールドハンドル（ユーザーが作成したワールドのみ）
   :type worldH: int
   :raises ValueError: パラメータが無効な場合
   :raises RuntimeError: ブラー処理に失敗した場合

   **mode_flags の値**:

   - ``ae.sdk.PF_MF_Alpha_PREMUL`` (0): プリマルチプライドアルファ（推奨）
   - ``ae.sdk.PF_MF_Alpha_STRAIGHT`` (1): ストレートアルファ

   **quality の値**:

   - ``ae.sdk.PF_Quality_LO`` (0): 低品質（高速）
   - ``ae.sdk.PF_Quality_HI`` (1): 高品質（低速）

   .. warning::
      この関数は **ユーザーが作成したワールドのみ** に使用できます。
      ``AEGP_GetLayerFrame()`` などで取得したフレームには使用できません。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      worldH = ae.sdk.AEGP_NewWorld(
          plugin_id,
          ae.sdk.AEGP_WorldType_8,
          1920,
          1080
      )

      try:
          # ブラー処理を適用
          ae.sdk.AEGP_FastBlur(
              10.0,                           # 半径10ピクセル
              ae.sdk.PF_MF_Alpha_PREMUL,      # プリマルチプライドアルファ
              ae.sdk.PF_Quality_HI,           # 高品質
              worldH
          )
      finally:
          ae.sdk.AEGP_DisposeWorld(worldH)

プラットフォームワールド
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NewPlatformWorld(plugin_id: int, world_type: int, width: int, height: int) -> int

   プラットフォーム固有の最適化されたワールドを作成します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param world_type: ワールドタイプ
   :type world_type: int
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int
   :return: プラットフォームワールドハンドル
   :rtype: int
   :raises ValueError: パラメータが無効な場合
   :raises RuntimeError: 作成に失敗した場合

   .. note::
      プラットフォームワールドは、通常のワールドよりもGPU処理や特殊なメモリ配置に最適化されています。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      platformWorldH = ae.sdk.AEGP_NewPlatformWorld(
          plugin_id,
          ae.sdk.AEGP_WorldType_32,
          1920,
          1080
      )

      try:
          # プラットフォームワールドを使用した処理
          pass
      finally:
          ae.sdk.AEGP_DisposePlatformWorld(platformWorldH)

.. function:: AEGP_DisposePlatformWorld(platformWorldH: int) -> None

   プラットフォームワールドを破棄します。

   :param platformWorldH: プラットフォームワールドハンドル
   :type platformWorldH: int
   :raises ValueError: platformWorldH が null の場合
   :raises RuntimeError: 破棄に失敗した場合

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DisposePlatformWorld(platformWorldH)

.. function:: AEGP_NewReferenceFromPlatformWorld(plugin_id: int, platformWorldH: int) -> int

   プラットフォームワールドから通常のワールドハンドル参照を作成します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param platformWorldH: プラットフォームワールドハンドル
   :type platformWorldH: int
   :return: ワールドハンドル（参照）
   :rtype: int
   :raises ValueError: platformWorldH が null の場合
   :raises RuntimeError: 参照作成に失敗した場合

   .. note::
      この関数で作成されたワールドハンドルは、プラットフォームワールドへの参照です。
      ``AEGP_DisposeWorld()`` で破棄しても、元のプラットフォームワールドは影響を受けません。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      platformWorldH = ae.sdk.AEGP_NewPlatformWorld(
          plugin_id,
          ae.sdk.AEGP_WorldType_8,
          1920,
          1080
      )

      try:
          # プラットフォームワールドから通常のワールド参照を作成
          worldH = ae.sdk.AEGP_NewReferenceFromPlatformWorld(plugin_id, platformWorldH)

          try:
              # worldH を使用した処理
              width, height = ae.sdk.AEGP_GetWorldSize(worldH)
          finally:
              ae.sdk.AEGP_DisposeWorld(worldH)
      finally:
          ae.sdk.AEGP_DisposePlatformWorld(platformWorldH)

使用例
------

ワールドの作成と基本操作
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_and_inspect_world():
       """ワールドを作成して情報を確認"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # 1920x1080の8ビットワールドを作成
       worldH = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           1920,
           1080
       )

       try:
           # ワールド情報を取得
           world_type = ae.sdk.AEGP_GetWorldType(worldH)
           width, height = ae.sdk.AEGP_GetWorldSize(worldH)
           row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)

           print(f"ワールドタイプ: {world_type}")
           print(f"サイズ: {width}x{height}")
           print(f"行バイト数: {row_bytes}")

           # PF_EffectWorld構造体を取得
           pf_world = ae.sdk.AEGP_FillOutPFEffectWorld(worldH)
           print(f"PF_EffectWorld: {pf_world}")

       finally:
           # 必ずクリーンアップ
           ae.sdk.AEGP_DisposeWorld(worldH)

   # 実行
   create_and_inspect_world()

ブラー処理の適用
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def apply_blur_to_world():
       """ワールドにブラー処理を適用"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # 1920x1080の8ビットワールドを作成
       worldH = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           1920,
           1080
       )

       try:
           # TODO: ワールドにピクセルデータを書き込む処理

           # ブラー処理を適用（半径5ピクセル、高品質）
           ae.sdk.AEGP_FastBlur(
               5.0,                           # ブラー半径
               ae.sdk.PF_MF_Alpha_PREMUL,     # プリマルチプライドアルファ
               ae.sdk.PF_Quality_HI,          # 高品質
               worldH
           )

           print("ブラー処理を適用しました")

       finally:
           ae.sdk.AEGP_DisposeWorld(worldH)

   # 実行
   apply_blur_to_world()

異なるビット深度のワールドを比較
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def compare_world_types():
       """異なるビット深度のワールドを作成して比較"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       world_types = [
           (ae.sdk.AEGP_WorldType_8, "8ビット", 4),
           (ae.sdk.AEGP_WorldType_16, "16ビット", 8),
           (ae.sdk.AEGP_WorldType_32, "32ビット浮動小数点", 16),
       ]

       width, height = 1920, 1080

       print(f"ワールドサイズ: {width}x{height}\n")

       for world_type, name, bytes_per_pixel in world_types:
           worldH = ae.sdk.AEGP_NewWorld(plugin_id, world_type, width, height)

           try:
               row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)
               expected_bytes = width * bytes_per_pixel
               total_memory = row_bytes * height

               print(f"{name}:")
               print(f"  1ピクセルあたり: {bytes_per_pixel}バイト")
               print(f"  理論上の行バイト数: {expected_bytes}バイト")
               print(f"  実際の行バイト数: {row_bytes}バイト")
               print(f"  総メモリ使用量: {total_memory / (1024*1024):.2f} MB")
               print()

           finally:
               ae.sdk.AEGP_DisposeWorld(worldH)

   # 実行
   compare_world_types()

NumPyとの連携（高度な例）
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import numpy as np
   import ctypes

   def world_to_numpy(worldH):
       """ワールドをNumPy配列に変換（8ビットのみ）"""
       # ワールドタイプを確認
       world_type = ae.sdk.AEGP_GetWorldType(worldH)
       if world_type != ae.sdk.AEGP_WorldType_8:
           raise ValueError("この関数は8ビットワールドのみサポートしています")

       # ワールド情報を取得
       width, height = ae.sdk.AEGP_GetWorldSize(worldH)
       row_bytes = ae.sdk.AEGP_GetWorldRowBytes(worldH)
       base_addr = ae.sdk.AEGP_GetWorldBaseAddr8(worldH)

       # メモリからNumPy配列を作成
       ptr = ctypes.cast(base_addr, ctypes.POINTER(ctypes.c_uint8))
       buffer = np.ctypeslib.as_array(ptr, shape=(height * row_bytes,))

       # RGBA配列として整形（ストライドを考慮）
       pixels = np.zeros((height, width, 4), dtype=np.uint8)
       for y in range(height):
           row_start = y * row_bytes
           row_data = buffer[row_start:row_start + width * 4]
           pixels[y] = row_data.reshape(width, 4)

       return pixels

   def process_world_with_numpy():
       """NumPyを使用してワールドを処理"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # ワールドを作成
       worldH = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           640,
           480
       )

       try:
           # TODO: ワールドにピクセルデータを書き込む

           # NumPy配列に変換
           pixels = world_to_numpy(worldH)

           # NumPyで画像処理（例：明度を上げる）
           pixels_float = pixels.astype(np.float32)
           pixels_float[:, :, :3] *= 1.5  # RGB値を1.5倍に（アルファは除く）
           pixels[:] = np.clip(pixels_float, 0, 255).astype(np.uint8)

           print(f"処理完了: {pixels.shape}")

       finally:
           ae.sdk.AEGP_DisposeWorld(worldH)

   # 実行（注意: ワールドにピクセルデータを書き込む処理が必要）
   # process_world_with_numpy()

プラットフォームワールドの使用
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def use_platform_world():
       """プラットフォームワールドを作成して使用"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # プラットフォームワールドを作成
       platformWorldH = ae.sdk.AEGP_NewPlatformWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_32,  # 32ビット浮動小数点
           1920,
           1080
       )

       try:
           print("プラットフォームワールドを作成しました")

           # プラットフォームワールドから通常のワールド参照を作成
           worldH = ae.sdk.AEGP_NewReferenceFromPlatformWorld(plugin_id, platformWorldH)

           try:
               # ワールド参照を使用して情報を取得
               width, height = ae.sdk.AEGP_GetWorldSize(worldH)
               world_type = ae.sdk.AEGP_GetWorldType(worldH)

               print(f"ワールド参照を作成しました")
               print(f"サイズ: {width}x{height}")
               print(f"タイプ: {world_type}")

           finally:
               ae.sdk.AEGP_DisposeWorld(worldH)

       finally:
           ae.sdk.AEGP_DisposePlatformWorld(platformWorldH)

   # 実行
   use_platform_world()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリ管理は必須**

   ワールドは必ず ``AEGP_DisposeWorld()`` で破棄してください。メモリリークを防ぐため、``try-finally`` ブロックを使用することを強く推奨します。

2. **ワールドサイズの制限**

   最大サイズは 30000x30000 ピクセルです。これを超えるワールドは作成できません。

3. **FastBlurの制約**

   ``AEGP_FastBlur()`` は **ユーザーが作成したワールドのみ** に使用できます。レイヤーから取得したフレーム（``AEGP_GetLayerFrame()``）には使用できません。

4. **ピクセルデータへの直接アクセス**

   ``AEGP_GetWorldBaseAddr*()`` 関数は低レベルメモリアクセスのため、Pythonから直接使用する際は十分注意してください。

5. **行バイト数の重要性**

   ピクセルデータにアクセスする際は、必ず ``AEGP_GetWorldRowBytes()`` で取得した行バイト数を使用してください。``width * bytes_per_pixel`` を使用すると不正確な結果になります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

リソース管理パターン
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 推奨: try-finallyでクリーンアップを保証
   plugin_id = ae.sdk.AEGP_GetPluginID()
   worldH = ae.sdk.AEGP_NewWorld(plugin_id, ae.sdk.AEGP_WorldType_8, 1920, 1080)

   try:
       # ワールドを使用した処理
       pass
   finally:
       ae.sdk.AEGP_DisposeWorld(worldH)

ワールドタイプの適切な選択
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 用途に応じたワールドタイプの選択

   # 標準的な画像処理: 8ビット（メモリ効率が良い）
   worldH_8 = ae.sdk.AEGP_NewWorld(plugin_id, ae.sdk.AEGP_WorldType_8, w, h)

   # 高品質処理: 16ビット（バンディング防止）
   worldH_16 = ae.sdk.AEGP_NewWorld(plugin_id, ae.sdk.AEGP_WorldType_16, w, h)

   # HDR/VFX処理: 32ビット浮動小数点
   worldH_32 = ae.sdk.AEGP_NewWorld(plugin_id, ae.sdk.AEGP_WorldType_32, w, h)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       worldH = ae.sdk.AEGP_NewWorld(plugin_id, world_type, width, height)
   except ValueError as e:
       print(f"パラメータエラー: {e}")
   except RuntimeError as e:
       print(f"ワールド作成失敗: {e}")

パフォーマンス考慮
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # メモリ使用量の計算
   width, height = 1920, 1080
   bytes_per_pixel = 4  # 8ビットRGBA
   estimated_memory = width * height * bytes_per_pixel / (1024 * 1024)
   print(f"推定メモリ使用量: {estimated_memory:.2f} MB")

   # 大きなワールドを扱う場合は8ビットを推奨
   if estimated_memory > 100:  # 100MB以上
       world_type = ae.sdk.AEGP_WorldType_8
   else:
       world_type = ae.sdk.AEGP_WorldType_32

関連項目
--------

- :doc:`AEGP_LayerRenderOptionsSuite2` - レイヤーレンダリング（フレーム取得）
- :doc:`AEGP_RenderSuite5` - レンダリング処理
- :doc:`index` - 低レベルAPI概要
