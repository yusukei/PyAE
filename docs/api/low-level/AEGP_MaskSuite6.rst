AEGP_MaskSuite6
===============

.. currentmodule:: ae.sdk

AEGP_MaskSuite6は、After Effectsレイヤーのマスク操作を行うためのSDK APIです。

概要
----

**実装状況**: 19/19関数実装済み ✅

AEGP_MaskSuite6は以下の機能を提供します:

- マスクの取得、作成、削除、複製
- マスクプロパティの取得・設定（反転、モード、カラー等）
- マスクアニメーション設定（モーションブラー）
- マスク形状設定（RotoBezier、フェザーフォールオフ）
- マスクロック状態の管理

基本概念
--------

マスクハンドル (AEGP_MaskRefH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのマスクを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - マスクハンドルは ``AEGP_GetLayerMaskByIndex()`` で取得します
   - マスクハンドルの使用後は ``AEGP_DisposeMask()`` で必ず解放してください
   - マスクを削除した後は、ハンドルは無効になります

マスクID (AEGP_MaskIDVal)
~~~~~~~~~~~~~~~~~~~~~~~~~~

マスクの一意な識別子。マスクのインデックスが変更されても、IDは変わりません。

.. note::
   インデックスはマスクの順序によって変わりますが、IDは不変です。
   長期的な参照にはIDを使用することを推奨します。

マスクモード (PF_MaskMode)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

マスクの合成モードを定義する列挙型:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``PF_MaskMode_NONE``
     - 0
     - マスクなし
   * - ``PF_MaskMode_ADD``
     - 1
     - 加算（マスクエリアを追加）
   * - ``PF_MaskMode_SUBTRACT``
     - 2
     - 減算（マスクエリアを削除）
   * - ``PF_MaskMode_INTERSECT``
     - 3
     - 交差（重なる部分のみ）
   * - ``PF_MaskMode_LIGHTEN``
     - 4
     - 明るくする
   * - ``PF_MaskMode_DARKEN``
     - 5
     - 暗くする
   * - ``PF_MaskMode_DIFFERENCE``
     - 6
     - 差の絶対値

マスクモーションブラー (AEGP_MaskMBlur)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

マスクのモーションブラー設定:

.. list-table::
   :header-rows: 1

   * - 定数
     - 説明
   * - ``AEGP_MaskMBlur_SAME_AS_LAYER``
     - レイヤーと同じ設定
   * - ``AEGP_MaskMBlur_OFF``
     - モーションブラー無効
   * - ``AEGP_MaskMBlur_ON``
     - モーションブラー有効

マスクフェザーフォールオフ (AEGP_MaskFeatherFalloff)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

マスクのフェザー減衰方式:

.. list-table::
   :header-rows: 1

   * - 定数
     - 説明
   * - ``AEGP_MaskFeatherFalloff_SMOOTH``
     - スムーズ減衰（滑らか）
   * - ``AEGP_MaskFeatherFalloff_LINEAR``
     - リニア減衰（直線的）

API リファレンス
----------------

マスク情報取得
~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerNumMasks(layerH: int) -> int

   レイヤーが持つマスクの数を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: マスク数
   :rtype: int
   :raises ValueError: layerHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      num_masks = ae.sdk.AEGP_GetLayerNumMasks(layerH)
      print(f"マスク数: {num_masks}")

.. function:: AEGP_GetLayerMaskByIndex(layerH: int, mask_index: int) -> int

   インデックスからマスクハンドルを取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param mask_index: マスクのインデックス（0から始まる）
   :type mask_index: int
   :return: マスクハンドル
   :rtype: int
   :raises ValueError: layerHがnullまたはインデックスが範囲外の場合
   :raises RuntimeError: API呼び出しに失敗した場合

   .. note::
      取得したマスクハンドルは、使用後に ``AEGP_DisposeMask()`` で解放する必要があります。

   **例**:

   .. code-block:: python

      maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, 0)
      # ... マスク操作 ...
      ae.sdk.AEGP_DisposeMask(maskH)

.. function:: AEGP_GetMaskID(maskH: int) -> int

   マスクの一意なIDを取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: マスクID
   :rtype: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   .. note::
      マスクIDは、マスクの順序が変わってもマスクを一意に識別できます。

   **例**:

   .. code-block:: python

      mask_id = ae.sdk.AEGP_GetMaskID(maskH)
      print(f"マスクID: {mask_id}")

マスクハンドル管理
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_DisposeMask(maskH: int) -> None

   マスクハンドルを解放します。

   :param maskH: マスクハンドル
   :type maskH: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   .. important::
      ``AEGP_GetLayerMaskByIndex()`` で取得したマスクハンドルは、
      使用後に必ず ``AEGP_DisposeMask()`` で解放してください。

   **例**:

   .. code-block:: python

      maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, 0)
      try:
          # マスク操作
          pass
      finally:
          ae.sdk.AEGP_DisposeMask(maskH)

マスク作成・削除
~~~~~~~~~~~~~~~~

.. function:: AEGP_CreateNewMask(layerH: int) -> tuple[int, int]

   レイヤーに新しいマスクを作成します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: (マスクハンドル, マスクインデックス) のタプル
   :rtype: tuple[int, int]
   :raises ValueError: layerHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   .. note::
      新しいマスクは、既存のマスクの最後に追加されます。

   **例**:

   .. code-block:: python

      maskH, mask_index = ae.sdk.AEGP_CreateNewMask(layerH)
      print(f"新しいマスクを作成しました（インデックス: {mask_index}）")

.. function:: AEGP_DeleteMaskFromLayer(maskH: int) -> None

   レイヤーからマスクを削除します。

   :param maskH: マスクハンドル
   :type maskH: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   .. warning::
      この操作は **Undo可能** です。``AEGP_StartUndoGroup()`` でグループ化することを推奨します。
      削除後、マスクハンドルは無効になります。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DeleteMaskFromLayer(maskH)

.. function:: AEGP_DuplicateMask(maskH: int) -> int

   マスクを複製します。

   :param maskH: 複製元のマスクハンドル
   :type maskH: int
   :return: 新しいマスクハンドル
   :rtype: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   .. note::
      複製されたマスクは、元のマスクの直後に追加されます。

   **例**:

   .. code-block:: python

      duplicate_maskH = ae.sdk.AEGP_DuplicateMask(maskH)
      print("マスクを複製しました")

マスクプロパティ取得・設定
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetMaskInvert(maskH: int) -> bool

   マスクの反転状態を取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: 反転されている場合は ``True``
   :rtype: bool
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      is_inverted = ae.sdk.AEGP_GetMaskInvert(maskH)
      print(f"マスク反転: {is_inverted}")

.. function:: AEGP_SetMaskInvert(maskH: int, invert: bool) -> None

   マスクの反転状態を設定します。

   :param maskH: マスクハンドル
   :type maskH: int
   :param invert: 反転するかどうか
   :type invert: bool
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetMaskInvert(maskH, True)

.. function:: AEGP_GetMaskMode(maskH: int) -> int

   マスクの合成モードを取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: マスクモード（``PF_MaskMode_*`` 定数）
   :rtype: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      mode = ae.sdk.AEGP_GetMaskMode(maskH)
      if mode == ae.sdk.PF_MaskMode_ADD:
          print("マスクモード: 加算")

.. function:: AEGP_SetMaskMode(maskH: int, mode: int) -> None

   マスクの合成モードを設定します。

   :param maskH: マスクハンドル
   :type maskH: int
   :param mode: マスクモード（``PF_MaskMode_*`` 定数）
   :type mode: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetMaskMode(maskH, ae.sdk.PF_MaskMode_SUBTRACT)

.. function:: AEGP_GetMaskColor(maskH: int) -> tuple[float, float, float]

   マスクの表示カラーを取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: (red, green, blue) のタプル（0.0～1.0）
   :rtype: tuple[float, float, float]
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      red, green, blue = ae.sdk.AEGP_GetMaskColor(maskH)
      print(f"マスクカラー: R={red}, G={green}, B={blue}")

.. function:: AEGP_SetMaskColor(maskH: int, red: float, green: float, blue: float) -> None

   マスクの表示カラーを設定します。

   :param maskH: マスクハンドル
   :type maskH: int
   :param red: 赤成分（0.0～1.0）
   :type red: float
   :param green: 緑成分（0.0～1.0）
   :type green: float
   :param blue: 青成分（0.0～1.0）
   :type blue: float
   :raises ValueError: maskHがnullまたは色成分が範囲外の場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      # マスクを赤色に設定
      ae.sdk.AEGP_SetMaskColor(maskH, 1.0, 0.0, 0.0)

マスクアニメーション設定
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetMaskMotionBlurState(maskH: int) -> int

   マスクのモーションブラー設定を取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: モーションブラー設定（``AEGP_MaskMBlur_*`` 定数）
   :rtype: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      blur_state = ae.sdk.AEGP_GetMaskMotionBlurState(maskH)
      if blur_state == ae.sdk.AEGP_MaskMBlur_ON:
          print("モーションブラー: 有効")

.. function:: AEGP_SetMaskMotionBlurState(maskH: int, blur_state: int) -> None

   マスクのモーションブラー設定を変更します。

   :param maskH: マスクハンドル
   :type maskH: int
   :param blur_state: モーションブラー設定（``AEGP_MaskMBlur_*`` 定数）
   :type blur_state: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetMaskMotionBlurState(maskH, ae.sdk.AEGP_MaskMBlur_ON)

マスク形状設定
~~~~~~~~~~~~~~

.. function:: AEGP_GetMaskFeatherFalloff(maskH: int) -> int

   マスクのフェザーフォールオフ設定を取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: フェザーフォールオフ（``AEGP_MaskFeatherFalloff_*`` 定数）
   :rtype: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      falloff = ae.sdk.AEGP_GetMaskFeatherFalloff(maskH)
      if falloff == ae.sdk.AEGP_MaskFeatherFalloff_LINEAR:
          print("フェザーフォールオフ: リニア")

.. function:: AEGP_SetMaskFeatherFalloff(maskH: int, falloff: int) -> None

   マスクのフェザーフォールオフ設定を変更します。

   :param maskH: マスクハンドル
   :type maskH: int
   :param falloff: フェザーフォールオフ（``AEGP_MaskFeatherFalloff_*`` 定数）
   :type falloff: int
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetMaskFeatherFalloff(maskH, ae.sdk.AEGP_MaskFeatherFalloff_SMOOTH)

.. function:: AEGP_GetMaskIsRotoBezier(maskH: int) -> bool

   マスクがRotoBezierかどうかを取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: RotoBezierの場合は ``True``
   :rtype: bool
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   .. note::
      RotoBezierは、自動的に滑らかな曲線を生成するマスクタイプです。

   **例**:

   .. code-block:: python

      is_roto = ae.sdk.AEGP_GetMaskIsRotoBezier(maskH)
      print(f"RotoBezier: {is_roto}")

.. function:: AEGP_SetMaskIsRotoBezier(maskH: int, is_roto_bezier: bool) -> None

   マスクをRotoBezierに設定します。

   :param maskH: マスクハンドル
   :type maskH: int
   :param is_roto_bezier: RotoBezierにするかどうか
   :type is_roto_bezier: bool
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetMaskIsRotoBezier(maskH, True)

マスクロック状態
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetMaskLockState(maskH: int) -> bool

   マスクのロック状態を取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: ロックされている場合は ``True``
   :rtype: bool
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   **例**:

   .. code-block:: python

      is_locked = ae.sdk.AEGP_GetMaskLockState(maskH)
      print(f"マスクロック: {is_locked}")

.. function:: AEGP_SetMaskLockState(maskH: int, lock: bool) -> None

   マスクのロック状態を設定します。

   :param maskH: マスクハンドル
   :type maskH: int
   :param lock: ロックするかどうか
   :type lock: bool
   :raises ValueError: maskHがnullの場合
   :raises RuntimeError: API呼び出しに失敗した場合

   .. note::
      ロックされたマスクは編集できなくなります。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetMaskLockState(maskH, True)

使用例
------

レイヤーのマスク情報を取得
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_layer_masks(layerH):
       """レイヤーのすべてのマスク情報を表示"""
       num_masks = ae.sdk.AEGP_GetLayerNumMasks(layerH)
       print(f"マスク数: {num_masks}")

       for i in range(num_masks):
           maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, i)
           try:
               # マスク情報を取得
               mask_id = ae.sdk.AEGP_GetMaskID(maskH)
               is_inverted = ae.sdk.AEGP_GetMaskInvert(maskH)
               mode = ae.sdk.AEGP_GetMaskMode(maskH)
               red, green, blue = ae.sdk.AEGP_GetMaskColor(maskH)
               is_locked = ae.sdk.AEGP_GetMaskLockState(maskH)
               is_roto = ae.sdk.AEGP_GetMaskIsRotoBezier(maskH)

               print(f"\nマスク {i}:")
               print(f"  ID: {mask_id}")
               print(f"  反転: {is_inverted}")
               print(f"  モード: {mode}")
               print(f"  カラー: R={red:.2f}, G={green:.2f}, B={blue:.2f}")
               print(f"  ロック: {is_locked}")
               print(f"  RotoBezier: {is_roto}")

           finally:
               ae.sdk.AEGP_DisposeMask(maskH)

   # 実行例
   # layerH = ... (レイヤーハンドルを取得)
   # dump_layer_masks(layerH)

マスクの一括設定
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_masks_for_composition(layerH):
       """レイヤーのすべてのマスクを設定"""
       ae.sdk.AEGP_StartUndoGroup("マスク設定変更")

       try:
           num_masks = ae.sdk.AEGP_GetLayerNumMasks(layerH)

           for i in range(num_masks):
               maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, i)
               try:
                   # マスクモードを加算に設定
                   ae.sdk.AEGP_SetMaskMode(maskH, ae.sdk.PF_MaskMode_ADD)

                   # フェザーをスムーズに設定
                   ae.sdk.AEGP_SetMaskFeatherFalloff(
                       maskH, ae.sdk.AEGP_MaskFeatherFalloff_SMOOTH)

                   # モーションブラーを有効化
                   ae.sdk.AEGP_SetMaskMotionBlurState(
                       maskH, ae.sdk.AEGP_MaskMBlur_ON)

                   # カラーを設定（インデックスに応じて色分け）
                   if i == 0:
                       ae.sdk.AEGP_SetMaskColor(maskH, 1.0, 0.0, 0.0)  # 赤
                   elif i == 1:
                       ae.sdk.AEGP_SetMaskColor(maskH, 0.0, 1.0, 0.0)  # 緑
                   else:
                       ae.sdk.AEGP_SetMaskColor(maskH, 0.0, 0.0, 1.0)  # 青

                   print(f"マスク {i} を設定しました")

               finally:
                   ae.sdk.AEGP_DisposeMask(maskH)

           print("すべてのマスクを設定しました")

       finally:
           ae.sdk.AEGP_EndUndoGroup()

   # 実行例
   # layerH = ... (レイヤーハンドルを取得)
   # setup_masks_for_composition(layerH)

マスクの複製と変更
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def duplicate_and_modify_mask(layerH, mask_index):
       """マスクを複製して変更"""
       ae.sdk.AEGP_StartUndoGroup("マスク複製")

       try:
           # 元のマスクを取得
           original_maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, mask_index)
           try:
               # マスクを複製
               duplicate_maskH = ae.sdk.AEGP_DuplicateMask(original_maskH)
               try:
                   # 複製したマスクを変更
                   ae.sdk.AEGP_SetMaskInvert(duplicate_maskH, True)  # 反転
                   ae.sdk.AEGP_SetMaskMode(
                       duplicate_maskH, ae.sdk.PF_MaskMode_SUBTRACT)  # 減算
                   ae.sdk.AEGP_SetMaskColor(
                       duplicate_maskH, 1.0, 1.0, 0.0)  # 黄色

                   print("マスクを複製して変更しました")

               finally:
                   ae.sdk.AEGP_DisposeMask(duplicate_maskH)

           finally:
               ae.sdk.AEGP_DisposeMask(original_maskH)

       finally:
           ae.sdk.AEGP_EndUndoGroup()

   # 実行例
   # layerH = ... (レイヤーハンドルを取得)
   # duplicate_and_modify_mask(layerH, 0)

マスクIDを使った識別
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def find_mask_by_id(layerH, target_mask_id):
       """マスクIDからマスクハンドルを検索"""
       num_masks = ae.sdk.AEGP_GetLayerNumMasks(layerH)

       for i in range(num_masks):
           maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, i)
           try:
               mask_id = ae.sdk.AEGP_GetMaskID(maskH)
               if mask_id == target_mask_id:
                   print(f"マスクID {target_mask_id} が見つかりました（インデックス: {i}）")
                   return maskH  # 注意: 呼び出し元で解放する必要があります
           except:
               ae.sdk.AEGP_DisposeMask(maskH)
               raise

       print(f"マスクID {target_mask_id} は見つかりませんでした")
       return None

   # 使用例
   # layerH = ... (レイヤーハンドルを取得)
   # target_id = 42
   # maskH = find_mask_by_id(layerH, target_id)
   # if maskH:
   #     try:
   #         # マスク操作
   #         pass
   #     finally:
   #         ae.sdk.AEGP_DisposeMask(maskH)

複数マスクの削除
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def delete_all_masks_from_layer(layerH):
       """レイヤーからすべてのマスクを削除"""
       ae.sdk.AEGP_StartUndoGroup("すべてのマスクを削除")

       try:
           num_masks = ae.sdk.AEGP_GetLayerNumMasks(layerH)

           # 後ろから削除（インデックスが変わらないように）
           for i in range(num_masks - 1, -1, -1):
               maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, i)
               try:
                   ae.sdk.AEGP_DeleteMaskFromLayer(maskH)
                   print(f"マスク {i} を削除しました")
               finally:
                   # 削除後はハンドルが無効になるため、Disposeは不要
                   pass

           print(f"{num_masks}個のマスクを削除しました")

       finally:
           ae.sdk.AEGP_EndUndoGroup()

   # 実行例
   # layerH = ... (レイヤーハンドルを取得)
   # delete_all_masks_from_layer(layerH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **マスクハンドルは必ず解放する**

   ``AEGP_GetLayerMaskByIndex()`` で取得したマスクハンドルは、
   使用後に必ず ``AEGP_DisposeMask()`` で解放してください。

2. **マスク削除後のハンドル**

   ``AEGP_DeleteMaskFromLayer()`` を呼び出した後は、マスクハンドルは無効になります。
   ``AEGP_DisposeMask()`` を呼び出す必要はありません。

3. **インデックスとIDの違い**

   - **インデックス**: マスクの順序（0から始まる）。並び替えで変化します。
   - **ID**: マスクの一意な識別子。変化しません。

   長期的な参照にはIDを使用してください。

4. **Undo操作**

   マスクの作成、削除、プロパティ変更は **Undo可能** です。
   複数の操作を行う場合は、``AEGP_StartUndoGroup()`` でグループ化してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

try-finallyでハンドル解放
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, 0)
   try:
       # マスク操作
       pass
   finally:
       ae.sdk.AEGP_DisposeMask(maskH)

後ろから削除
^^^^^^^^^^^^

複数のマスクを削除する場合は、後ろから削除することでインデックスの変化を回避できます。

.. code-block:: python

   num_masks = ae.sdk.AEGP_GetLayerNumMasks(layerH)
   for i in range(num_masks - 1, -1, -1):
       maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, i)
       ae.sdk.AEGP_DeleteMaskFromLayer(maskH)

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("マスク操作")
   try:
       # 複数のマスク操作
       pass
   finally:
       ae.sdk.AEGP_EndUndoGroup()

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, index)
   except ValueError as e:
       print(f"無効なインデックス: {e}")
   except RuntimeError as e:
       print(f"マスク取得エラー: {e}")

関連項目
--------

- :doc:`AEGP_MaskOutlineSuite3` - マスク形状の頂点操作、フェザー管理
- :doc:`AEGP_StreamSuite6` - マスクプロパティのストリーム操作
- :doc:`AEGP_KeyframeSuite5` - マスクアニメーションのキーフレーム操作
- :doc:`AEGP_LayerSuite9` - レイヤー管理
- :doc:`index` - 低レベルAPI概要
