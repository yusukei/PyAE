AEGP_CompositeSuite2
====================

.. currentmodule:: ae.sdk

AEGP_CompositeSuite2は、画像バッファ（PF_EffectWorld）の合成、コピー、アルファチャンネル操作を行うためのSDK APIです。

概要
----

**実装状況**: 6/6関数実装、6/6関数テスト済み ✅

AEGP_CompositeSuite2は以下の機能を提供します:

- 画像バッファのコピー（低品質・高品質、ストレート/プリマルチプライドアルファ）
- 矩形領域の転送とコンポジット処理
- アルファチャンネルのクリア
- トラックマットの前処理

基本概念
--------

PF_EffectWorld (ワールド)
~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsの画像バッファを表す構造体。PyAEでは整数値（ハンドル、``uintptr_t``）として扱われます。

.. important::
   - ワールドは ``AEGP_NewWorld()`` で作成し、使用後は ``AEGP_DisposeWorld()`` で必ず解放する必要があります
   - ワールドのビット深度（8/16/32bit）に応じた関数を使用してください
   - ワールドのサイズは作成時に指定し、後から変更できません

アルファチャンネルの扱い
~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsでは、アルファチャンネルの扱いに2つのモードがあります:

- **Straight Alpha (ストレートアルファ)**: RGBとアルファが独立（``PF_MF_Alpha_STRAIGHT``）
- **Premultiplied Alpha (プリマルチプライドアルファ)**: RGBがアルファで乗算済み（``PF_MF_Alpha_PREMUL``）

.. note::
   After Effectsの標準はプリマルチプライドアルファです。透明度を扱う際は、アルファモードに注意してください。

品質モード
~~~~~~~~~~

画像処理の品質を指定するフラグ:

- ``PF_Quality_LO`` (0): 低品質（高速、最近傍法）
- ``PF_Quality_HI`` (1): 高品質（低速、バイリニア補間）

API リファレンス
----------------

ビットコピー - 低品質（高速）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CopyBits_LQ(src_worldH: int, src_rect: dict | None, dst_rect: dict | None, dst_worldH: int) -> None

   低品質モードでビットをコピーします（最近傍法、高速）。

   :param src_worldH: ソースPF_EffectWorldのハンドル
   :type src_worldH: int
   :param src_rect: ソース矩形。``None`` の場合はワールド全体。
   :type src_rect: dict | None
   :param dst_rect: 転送先矩形。``None`` の場合はワールド全体。
   :type dst_rect: dict | None
   :param dst_worldH: 転送先PF_EffectWorldのハンドル
   :type dst_worldH: int

   **矩形の構造** (``dict``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``left``
        - int
        - 左端X座標
      * - ``top``
        - int
        - 上端Y座標
      * - ``right``
        - int
        - 右端X座標
      * - ``bottom``
        - int
        - 下端Y座標

   .. note::
      ソース矩形と転送先矩形のサイズが異なる場合、スケーリングが行われます。
      低品質モードでは最近傍法（Nearest Neighbor）が使用されます。

   **例**:

   .. code-block:: python

      # 50x50の領域をコピー
      src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
      dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
      ae.sdk.AEGP_CopyBits_LQ(src_world, src_rect, dst_rect, dst_world)

      # ワールド全体をコピー（Noneを指定）
      ae.sdk.AEGP_CopyBits_LQ(src_world, None, None, dst_world)

      # スケーリング（50x50 → 100x100）
      src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
      dst_rect = {"left": 0, "top": 0, "right": 100, "bottom": 100}
      ae.sdk.AEGP_CopyBits_LQ(src_world, src_rect, dst_rect, dst_world)

ビットコピー - 高品質（ストレートアルファ）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CopyBits_HQ_Straight(src_worldH: int, src_rect: dict | None, dst_rect: dict | None, dst_worldH: int) -> None

   高品質モードでビットをコピーします（ストレートアルファ、バイリニア補間）。

   :param src_worldH: ソースPF_EffectWorldのハンドル
   :type src_worldH: int
   :param src_rect: ソース矩形。``None`` の場合はワールド全体。
   :type src_rect: dict | None
   :param dst_rect: 転送先矩形。``None`` の場合はワールド全体。
   :type dst_rect: dict | None
   :param dst_worldH: 転送先PF_EffectWorldのハンドル
   :type dst_worldH: int

   .. note::
      この関数はストレートアルファモードで動作します。スケーリング時にバイリニア補間を使用します。

   **例**:

   .. code-block:: python

      # 高品質コピー（ストレートアルファ）
      src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
      dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
      ae.sdk.AEGP_CopyBits_HQ_Straight(src_world, src_rect, dst_rect, dst_world)

ビットコピー - 高品質（プリマルチプライドアルファ）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CopyBits_HQ_Premul(src_worldH: int, src_rect: dict | None, dst_rect: dict | None, dst_worldH: int) -> None

   高品質モードでビットをコピーします（プリマルチプライドアルファ、バイリニア補間）。

   :param src_worldH: ソースPF_EffectWorldのハンドル
   :type src_worldH: int
   :param src_rect: ソース矩形。``None`` の場合はワールド全体。
   :type src_rect: dict | None
   :param dst_rect: 転送先矩形。``None`` の場合はワールド全体。
   :type dst_rect: dict | None
   :param dst_worldH: 転送先PF_EffectWorldのハンドル
   :type dst_worldH: int

   .. note::
      この関数はプリマルチプライドアルファモード（After Effectsの標準）で動作します。
      スケーリング時にバイリニア補間を使用します。

   **例**:

   .. code-block:: python

      # 高品質コピー（プリマルチプライドアルファ）
      src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
      dst_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
      ae.sdk.AEGP_CopyBits_HQ_Premul(src_world, src_rect, dst_rect, dst_world)

アルファチャンネルのクリア
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_ClearAlphaExceptRect(dst_worldH: int, rect: dict) -> None

   指定された矩形領域以外のアルファチャンネルをクリア（透明化）します。

   :param dst_worldH: 対象となるPF_EffectWorldのハンドル
   :type dst_worldH: int
   :param rect: アルファを保持する矩形領域
   :type rect: dict

   .. note::
      この関数は、矩形領域内のピクセルのアルファ値は変更せず、矩形外のピクセルのアルファ値を0（透明）にします。
      マスク処理や画像の切り抜きに便利です。

   **例**:

   .. code-block:: python

      # 中央50x50の領域を除いてアルファをクリア
      rect = {"left": 25, "top": 25, "right": 75, "bottom": 75}
      ae.sdk.AEGP_ClearAlphaExceptRect(dst_world, rect)

      # 画像全体のアルファを保持（何も変更しない）
      rect = {"left": 0, "top": 0, "right": 100, "bottom": 100}
      ae.sdk.AEGP_ClearAlphaExceptRect(dst_world, rect)

矩形転送とコンポジット
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_TransferRect(quality: int, m_flags: int, field: int, src_rect: dict, src_worldH: int, comp_mode_ptr: int, blend_tables_ptr: int, mask_worldH: int, dest_x: int, dest_y: int, dst_worldH: int) -> None

   ソース画像から矩形領域を転送し、コンポジット処理を行います。

   :param quality: レンダリング品質（``PF_Quality_LO`` または ``PF_Quality_HI``）
   :type quality: int
   :param m_flags: モードフラグ（``PF_MF_Alpha_PREMUL`` または ``PF_MF_Alpha_STRAIGHT``）
   :type m_flags: int
   :param field: フィールドモード（通常は ``0`` = フルフレーム）
   :type field: int
   :param src_rect: ソース矩形
   :type src_rect: dict
   :param src_worldH: ソースPF_EffectWorldのハンドル
   :type src_worldH: int
   :param comp_mode_ptr: コンポジットモード（PF_CompositeMode*）。``0`` で標準（Normal）。
   :type comp_mode_ptr: int
   :param blend_tables_ptr: ブレンドテーブル（PF_EffectBlendingTables）。``0`` でブレンドなし。
   :type blend_tables_ptr: int
   :param mask_worldH: マスクワールド（PF_MaskWorld*）。``0`` でマスクなし。
   :type mask_worldH: int
   :param dest_x: 転送先X座標
   :type dest_x: int
   :param dest_y: 転送先Y座標
   :type dest_y: int
   :param dst_worldH: 転送先PF_EffectWorldのハンドル
   :type dst_worldH: int

   .. note::
      この関数は、ソース矩形の内容を転送先の指定位置に転送し、
      オプションのコンポジットモード、ブレンドテーブル、マスクを適用します。

   **品質フラグ**:

   - ``ae.sdk.PF_Quality_LO`` (0): 低品質（高速）
   - ``ae.sdk.PF_Quality_HI`` (1): 高品質（低速）

   **モードフラグ**:

   - ``ae.sdk.PF_MF_Alpha_PREMUL`` (0): プリマルチプライドアルファ
   - ``ae.sdk.PF_MF_Alpha_STRAIGHT`` (1): ストレートアルファ

   **例**:

   .. code-block:: python

      # 基本的な転送（マスクなし、ブレンドなし）
      src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

      ae.sdk.AEGP_TransferRect(
          ae.sdk.PF_Quality_HI,       # 高品質
          ae.sdk.PF_MF_Alpha_PREMUL,  # プリマルチプライドアルファ
          0,                          # フィールドなし（フルフレーム）
          src_rect,                   # ソース矩形
          src_world,                  # ソースワールド
          0,                          # コンポジットモードなし（Normal）
          0,                          # ブレンドテーブルなし
          0,                          # マスクなし
          0,                          # 転送先X座標
          0,                          # 転送先Y座標
          dst_world                   # 転送先ワールド
      )

      # オフセット付き転送（50, 50の位置に転送）
      ae.sdk.AEGP_TransferRect(
          ae.sdk.PF_Quality_HI,
          ae.sdk.PF_MF_Alpha_PREMUL,
          0,
          src_rect,
          src_world,
          0,
          0,
          0,
          50,  # 転送先X座標
          50,  # 転送先Y座標
          dst_world
      )

トラックマットの前処理
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_PrepTrackMatte(num_pix: int, deepB: bool, src_mask_ptr: int, mask_flags: int) -> bytes

   トラックマットの前処理を行います。

   :param num_pix: ピクセル数
   :type num_pix: int
   :param deepB: ``True`` の場合16bpc、``False`` の場合8bpc
   :type deepB: bool
   :param src_mask_ptr: ソースマスクデータのポインタ（``uintptr_t``）
   :type src_mask_ptr: int
   :param mask_flags: マスクフラグ（``PF_MaskFlags``）
   :type mask_flags: int
   :return: 処理済みマスクデータ（バイナリ）
   :rtype: bytes

   .. note::
      この関数は、トラックマットのピクセルデータを前処理し、
      コンポジット処理に適した形式に変換します。

   **例**:

   .. code-block:: python

      # マスクワールドからベースアドレスを取得
      mask_world = ae.sdk.AEGP_NewWorld(plugin_id, ae.sdk.AEGP_WorldType_8, 10, 10)
      base_addr = ae.sdk.AEGP_GetWorldBaseAddr8(mask_world)

      # トラックマットを前処理（8bpc）
      num_pixels = 100
      result = ae.sdk.AEGP_PrepTrackMatte(
          num_pixels,   # ピクセル数
          False,        # deepB (False = 8-bit)
          base_addr,    # ソースマスクポインタ
          0             # マスクフラグ
      )

      print(f"処理済みマスクデータ: {len(result)} bytes")

      # 16bpcの場合
      mask_world_16 = ae.sdk.AEGP_NewWorld(plugin_id, ae.sdk.AEGP_WorldType_16, 10, 10)
      base_addr_16 = ae.sdk.AEGP_GetWorldBaseAddr16(mask_world_16)

      result_16 = ae.sdk.AEGP_PrepTrackMatte(
          num_pixels,
          True,         # deepB (True = 16-bit)
          base_addr_16,
          0
      )

使用例
------

画像バッファのコピーと変換
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def copy_and_scale_world():
       """画像ワールドをコピーしてスケーリング"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # ソースワールドを作成（100x100、8ビット）
       src_world = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           100,
           100
       )

       # 転送先ワールドを作成（200x200、8ビット）
       dst_world = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           200,
           200
       )

       try:
           # 低品質コピーでスケーリング（2倍）
           src_rect = {"left": 0, "top": 0, "right": 100, "bottom": 100}
           dst_rect = {"left": 0, "top": 0, "right": 200, "bottom": 200}

           ae.sdk.AEGP_CopyBits_LQ(src_world, src_rect, dst_rect, dst_world)
           print("低品質でスケーリングしました（高速）")

       finally:
           # リソース解放
           ae.sdk.AEGP_DisposeWorld(src_world)
           ae.sdk.AEGP_DisposeWorld(dst_world)

   # 実行
   copy_and_scale_world()

高品質スケーリング
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def high_quality_resize():
       """高品質でリサイズ（バイリニア補間）"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # 16ビットワールドで高品質処理
       src_world = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_16,
           100,
           100
       )

       dst_world = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_16,
           150,
           150
       )

       try:
           # 高品質コピー（プリマルチプライドアルファ）
           src_rect = {"left": 0, "top": 0, "right": 100, "bottom": 100}
           dst_rect = {"left": 0, "top": 0, "right": 150, "bottom": 150}

           ae.sdk.AEGP_CopyBits_HQ_Premul(
               src_world,
               src_rect,
               dst_rect,
               dst_world
           )

           print("高品質でリサイズしました（1.5倍）")

       finally:
           ae.sdk.AEGP_DisposeWorld(src_world)
           ae.sdk.AEGP_DisposeWorld(dst_world)

   # 実行
   high_quality_resize()

マスク処理（矩形切り抜き）
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def apply_rectangular_mask():
       """矩形マスクを適用（矩形外を透明化）"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # ワールドを作成
       world = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           200,
           200
       )

       try:
           # 中央100x100の領域を除いてアルファをクリア
           mask_rect = {
               "left": 50,
               "top": 50,
               "right": 150,
               "bottom": 150
           }

           ae.sdk.AEGP_ClearAlphaExceptRect(world, mask_rect)
           print("矩形マスクを適用しました")

       finally:
           ae.sdk.AEGP_DisposeWorld(world)

   # 実行
   apply_rectangular_mask()

画像合成（オフセット付き転送）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def composite_images_with_offset():
       """画像を合成（オフセット付き）"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # 背景（200x200）
       background = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           200,
           200
       )

       # 前景（50x50）
       foreground = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           50,
           50
       )

       try:
           # 前景を背景の (75, 75) 位置に転送
           src_rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}

           ae.sdk.AEGP_TransferRect(
               ae.sdk.PF_Quality_HI,       # 高品質
               ae.sdk.PF_MF_Alpha_PREMUL,  # プリマルチプライドアルファ
               0,                          # フルフレーム
               src_rect,                   # ソース矩形（全体）
               foreground,                 # 前景
               0,                          # コンポジットモードなし
               0,                          # ブレンドテーブルなし
               0,                          # マスクなし
               75,                         # X座標
               75,                         # Y座標
               background                  # 背景
           )

           print("前景を背景の (75, 75) に合成しました")

       finally:
           ae.sdk.AEGP_DisposeWorld(foreground)
           ae.sdk.AEGP_DisposeWorld(background)

   # 実行
   composite_images_with_offset()

複数ビット深度の処理
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def process_multiple_bit_depths():
       """異なるビット深度のワールドを処理"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # 8ビット、16ビット、32ビットワールドを作成
       world_8 = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_8,
           100,
           100
       )

       world_16 = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_16,
           100,
           100
       )

       world_32 = ae.sdk.AEGP_NewWorld(
           plugin_id,
           ae.sdk.AEGP_WorldType_32,
           100,
           100
       )

       try:
           # 8ビット → 8ビット（低品質コピー）
           ae.sdk.AEGP_CopyBits_LQ(world_8, None, None, world_8)
           print("8ビットワールドをコピー（LQ）")

           # 16ビット → 16ビット（高品質コピー）
           ae.sdk.AEGP_CopyBits_HQ_Straight(world_16, None, None, world_16)
           print("16ビットワールドをコピー（HQ Straight）")

           # 32ビット → 32ビット（高品質コピー、プリマルチプライド）
           ae.sdk.AEGP_CopyBits_HQ_Premul(world_32, None, None, world_32)
           print("32ビットワールドをコピー（HQ Premul）")

       finally:
           ae.sdk.AEGP_DisposeWorld(world_8)
           ae.sdk.AEGP_DisposeWorld(world_16)
           ae.sdk.AEGP_DisposeWorld(world_32)

   # 実行
   process_multiple_bit_depths()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリ管理**

   ワールドは必ず ``AEGP_DisposeWorld()`` で解放してください。
   リソースリークの原因となります。

2. **ビット深度の一致**

   ソースと転送先のワールドは同じビット深度である必要があります。
   異なるビット深度間のコピーは保証されません。

3. **矩形の範囲チェック**

   矩形がワールドの範囲内に収まっていることを確認してください。
   範囲外の矩形を指定すると、予期しない動作やクラッシュの原因となります。

4. **アルファモードの選択**

   - After Effectsの標準はプリマルチプライドアルファです
   - 透明度を扱う際は、適切なアルファモードを選択してください
   - ストレートアルファは外部アプリケーションとの互換性が高い場合があります

ベストプラクティス
~~~~~~~~~~~~~~~~~~

リソースの確実な解放
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   plugin_id = ae.sdk.AEGP_GetPluginID()
   world = ae.sdk.AEGP_NewWorld(plugin_id, ae.sdk.AEGP_WorldType_8, 100, 100)

   try:
       # ワールドを使用
       pass
   finally:
       # 確実に解放
       ae.sdk.AEGP_DisposeWorld(world)

矩形の範囲チェック
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def is_rect_valid(rect, world_width, world_height):
       """矩形がワールドの範囲内かチェック"""
       return (
           rect["left"] >= 0 and
           rect["top"] >= 0 and
           rect["right"] <= world_width and
           rect["bottom"] <= world_height and
           rect["left"] < rect["right"] and
           rect["top"] < rect["bottom"]
       )

   # 使用例
   rect = {"left": 0, "top": 0, "right": 50, "bottom": 50}
   if is_rect_valid(rect, 100, 100):
       ae.sdk.AEGP_CopyBits_LQ(src_world, rect, rect, dst_world)

品質とパフォーマンスのトレードオフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 高速処理が必要な場合
   ae.sdk.AEGP_CopyBits_LQ(src_world, rect, rect, dst_world)

   # 品質が重要な場合
   ae.sdk.AEGP_CopyBits_HQ_Premul(src_world, rect, rect, dst_world)

ビット深度の確認
^^^^^^^^^^^^^^^^

.. code-block:: python

   # ワールドのビット深度を確認
   world_type = ae.sdk.AEGP_GetWorldType(world)

   if world_type == ae.sdk.AEGP_WorldType_8:
       base_addr = ae.sdk.AEGP_GetWorldBaseAddr8(world)
   elif world_type == ae.sdk.AEGP_WorldType_16:
       base_addr = ae.sdk.AEGP_GetWorldBaseAddr16(world)
   elif world_type == ae.sdk.AEGP_WorldType_32:
       base_addr = ae.sdk.AEGP_GetWorldBaseAddr32(world)

パフォーマンス最適化
~~~~~~~~~~~~~~~~~~~~

1. **品質モードの選択**

   - プレビューには ``PF_Quality_LO`` を使用
   - 最終レンダリングには ``PF_Quality_HI`` を使用

2. **矩形サイズの最小化**

   - 必要な領域のみをコピー
   - 全体コピーが不要な場合は、矩形を指定

3. **ワールドの再利用**

   - 頻繁に使用するワールドは再利用
   - 不要になったワールドは即座に解放

関連項目
--------

- :doc:`AEGP_WorldSuite3` - ワールドの作成、破棄、情報取得
- :doc:`AEGP_RenderSuite5` - レンダリング機能
- :doc:`index` - 低レベルAPI概要
