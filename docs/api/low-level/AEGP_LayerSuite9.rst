AEGP_LayerSuite9
===============

.. currentmodule:: ae.sdk

AEGP_LayerSuite9は、After Effectsのレイヤー操作、情報取得、プロパティ設定を行うためのSDK APIです。

概要
----

**実装状況**: 45/70関数実装 🚧

AEGP_LayerSuite9は以下の機能を提供します:

- レイヤーの取得、追加、削除、複製
- レイヤー名、インデックス、ID、タイプの取得・設定
- レイヤーの親子関係の操作
- In Point / Out Point / Duration の取得・設定
- レイヤーフラグ（表示/ロック等）の取得・設定
- レイヤー品質（Quality、Sampling Quality）の設定
- ブレンドモード、トラックマット、Stretch の設定
- 3D/2Dレイヤー判定、座標変換
- レイヤーのTransform行列の取得

基本概念
--------

レイヤーハンドル (AEGP_LayerH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのレイヤーを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - レイヤーハンドルは、レイヤーが存在する間のみ有効です
   - レイヤーを削除した後は、ハンドルは無効になります
   - コンポジション内の各レイヤーに固有のハンドルがあります

レイヤータイプ (AEGP_ObjectType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レイヤーのタイプを識別する値:

.. list-table::
   :header-rows: 1

   * - 値
     - レイヤータイプ
   * - 0
     - AEGP_ObjectType_NONE
   * - 1
     - AEGP_ObjectType_AV (映像・音声)
   * - 2
     - AEGP_ObjectType_LIGHT (ライト)
   * - 3
     - AEGP_ObjectType_CAMERA (カメラ)
   * - 4
     - AEGP_ObjectType_TEXT (テキスト)
   * - 5
     - AEGP_ObjectType_VECTOR (シェイプ)

レイヤー品質 (AEGP_LayerQuality)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - 値
     - 品質設定
   * - 0
     - AEGP_LayerQual_NONE
   * - 1
     - AEGP_LayerQual_WIREFRAME (ワイヤーフレーム)
   * - 2
     - AEGP_LayerQual_DRAFT (ドラフト)
   * - 3
     - AEGP_LayerQual_BEST (最高品質)

レイヤーフラグ (AEGP_LayerFlags)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レイヤーの状態を表すビットフラグ:

.. code-block:: python

   AEGP_LayerFlag_NONE           = 0
   AEGP_LayerFlag_VIDEO_ACTIVE   = 1 << 0  # ビデオトラック有効
   AEGP_LayerFlag_AUDIO_ACTIVE   = 1 << 1  # オーディオトラック有効
   AEGP_LayerFlag_EFFECTS_ACTIVE = 1 << 2  # エフェクト有効
   AEGP_LayerFlag_MOTION_BLUR    = 1 << 3  # モーションブラー
   AEGP_LayerFlag_FRAME_BLENDING = 1 << 4  # フレームブレンド
   AEGP_LayerFlag_LOCKED         = 1 << 5  # ロック
   AEGP_LayerFlag_SHY            = 1 << 6  # シャイ
   AEGP_LayerFlag_COLLAPSE       = 1 << 7  # コラプス
   AEGP_LayerFlag_QUALITY        = 1 << 8  # 品質
   AEGP_LayerFlag_ADJUSTMENT_LAYER = 1 << 10  # 調整レイヤー
   AEGP_LayerFlag_GUIDE_LAYER    = 1 << 13  # ガイドレイヤー
   AEGP_LayerFlag_3D_LAYER       = 1 << 14  # 3Dレイヤー
   AEGP_LayerFlag_SOLO           = 1 << 15  # ソロ

Undoable操作
~~~~~~~~~~~~

以下の操作は **Undo可能** です（After EffectsのUndo履歴に記録されます）:

- ``AEGP_SetLayerName`` - レイヤー名の設定
- ``AEGP_SetLayerParent`` - 親レイヤーの設定
- ``AEGP_SetLayerFlag`` - レイヤーフラグの設定
- ``AEGP_SetLayerQuality`` - 品質設定
- ``AEGP_SetLayerTransferMode`` - ブレンドモード設定
- ``AEGP_AddLayer`` / ``AEGP_DeleteLayer`` / ``AEGP_DuplicateLayer`` - レイヤーの追加・削除・複製
- ``AEGP_ReorderLayer`` - レイヤー順序の変更
- ``AEGP_SetLayerStretch`` - タイムストレッチの設定

.. tip::
   複数の変更を行う場合は、``AEGP_StartUndoGroup()`` と ``AEGP_EndUndoGroup()`` でグループ化することを推奨します。

API リファレンス
----------------

レイヤー取得
~~~~~~~~~~~~

.. function:: AEGP_GetCompNumLayers(compH: int) -> int

   コンポジション内のレイヤー数を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: レイヤー数
   :rtype: int

   **例**:

   .. code-block:: python

      num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
      print(f"レイヤー数: {num_layers}")

.. function:: AEGP_GetCompLayerByIndex(compH: int, index: int) -> int

   インデックスからレイヤーハンドルを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param index: レイヤーのインデックス（0から始まる）
   :type index: int
   :return: レイヤーハンドル
   :rtype: int

   .. note::
      インデックスは0から始まります。範囲外のインデックスを指定すると例外が発生します。

   **例**:

   .. code-block:: python

      layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 0)  # 最初のレイヤー

.. function:: AEGP_GetActiveLayer() -> int

   現在アクティブ（選択中）なレイヤーを取得します。

   :return: アクティブなレイヤーハンドル。アクティブなレイヤーがない場合は0
   :rtype: int

   **例**:

   .. code-block:: python

      layerH = ae.sdk.AEGP_GetActiveLayer()
      if layerH:
          print("アクティブなレイヤーが存在します")

.. function:: AEGP_GetLayerFromLayerID(compH: int, layer_id: int) -> int

   レイヤーIDからレイヤーハンドルを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param layer_id: レイヤーID（``AEGP_GetLayerID()`` で取得）
   :type layer_id: int
   :return: レイヤーハンドル
   :rtype: int

   .. note::
      レイヤーIDは、レイヤーの位置が変わっても保持される永続的な識別子です。
      インデックスは順序変更で変わりますが、IDは変わりません。

   **例**:

   .. code-block:: python

      layer_id = ae.sdk.AEGP_GetLayerID(layerH)
      # 後でIDから復元
      layerH = ae.sdk.AEGP_GetLayerFromLayerID(compH, layer_id)

レイヤー情報
~~~~~~~~~~~~

.. function:: AEGP_GetLayerName(plugin_id: int, layerH: int) -> str

   レイヤーの名前を取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: レイヤー名（UTF-8文字列）
   :rtype: str

   .. note::
      内部的にUTF-16からUTF-8に自動変換されます。日本語のレイヤー名も正しく取得できます。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      name = ae.sdk.AEGP_GetLayerName(plugin_id, layerH)
      print(f"レイヤー名: {name}")

.. function:: AEGP_GetLayerIndex(layerH: int) -> int

   レイヤーのインデックス（0から始まる順序）を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: レイヤーインデックス（0から始まる）
   :rtype: int

   **例**:

   .. code-block:: python

      index = ae.sdk.AEGP_GetLayerIndex(layerH)
      print(f"レイヤーインデックス: {index}")

.. function:: AEGP_GetLayerID(layerH: int) -> int

   レイヤーの固有ID（永続的な識別子）を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: レイヤーID
   :rtype: int

   .. note::
      レイヤーIDはレイヤーの順序が変わっても保持されます。

   **例**:

   .. code-block:: python

      layer_id = ae.sdk.AEGP_GetLayerID(layerH)

.. function:: AEGP_GetLayerSourceItem(layerH: int) -> int

   レイヤーのソースアイテム（フッテージ、コンポジション等）を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: アイテムハンドル。ソースがない場合は0
   :rtype: int

   **例**:

   .. code-block:: python

      itemH = ae.sdk.AEGP_GetLayerSourceItem(layerH)
      if itemH:
          item_name = ae.sdk.AEGP_GetItemName(plugin_id, itemH)
          print(f"ソース: {item_name}")

.. function:: AEGP_GetLayerSourceItemID(layerH: int) -> int

   レイヤーのソースアイテムIDを取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: ソースアイテムID
   :rtype: int

.. function:: AEGP_GetLayerParentComp(layerH: int) -> int

   レイヤーが所属するコンポジションを取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: コンポジションハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      compH = ae.sdk.AEGP_GetLayerParentComp(layerH)

.. function:: AEGP_GetLayerObjectType(layerH: int) -> int

   レイヤーのタイプ（AV、ライト、カメラ、テキスト等）を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: オブジェクトタイプ（AEGP_ObjectType）
   :rtype: int

   **例**:

   .. code-block:: python

      obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)
      if obj_type == 1:  # AEGP_ObjectType_AV
          print("映像/音声レイヤー")
      elif obj_type == 3:  # AEGP_ObjectType_CAMERA
          print("カメラレイヤー")

.. function:: AEGP_IsLayer3D(layerH: int) -> bool

   レイヤーが3Dレイヤーかどうかを確認します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: 3Dレイヤーの場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_IsLayer3D(layerH):
          print("3Dレイヤーです")

.. function:: AEGP_IsLayer2D(layerH: int) -> bool

   レイヤーが2Dレイヤーかどうかを確認します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: 2Dレイヤーの場合は ``True``
   :rtype: bool

レイヤーの親子関係
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerParent(layerH: int) -> int

   レイヤーの親レイヤーを取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: 親レイヤーハンドル。親がない場合は0
   :rtype: int

   **例**:

   .. code-block:: python

      parentH = ae.sdk.AEGP_GetLayerParent(layerH)
      if parentH:
          print("親レイヤーが存在します")

.. function:: AEGP_SetLayerParent(layerH: int, parentLayerH: int) -> None

   レイヤーの親レイヤーを設定します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param parentLayerH: 親レイヤーハンドル（0で親を解除）
   :type parentLayerH: int

   .. warning::
      循環参照（自分自身、子孫、祖先を親に設定）はエラーになります。

   **例**:

   .. code-block:: python

      # 親レイヤーを設定
      ae.sdk.AEGP_SetLayerParent(childLayerH, parentLayerH)

      # 親を解除
      ae.sdk.AEGP_SetLayerParent(layerH, 0)

レイヤーフラグ
~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerFlags(layerH: int) -> int

   レイヤーのフラグ（表示状態、ロック等）を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: レイヤーフラグ（ビットフラグ）
   :rtype: int

   **例**:

   .. code-block:: python

      flags = ae.sdk.AEGP_GetLayerFlags(layerH)

      # ビデオトラックが有効か確認
      AEGP_LayerFlag_VIDEO_ACTIVE = 1 << 0
      if flags & AEGP_LayerFlag_VIDEO_ACTIVE:
          print("ビデオトラック有効")

.. function:: AEGP_SetLayerFlag(layerH: int, flag: int, value: int) -> None

   レイヤーフラグの特定のビットを設定します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param flag: 設定するフラグ（AEGP_LayerFlags）
   :type flag: int
   :param value: 値（0または1）
   :type value: int

   **例**:

   .. code-block:: python

      # ビデオトラックを有効化
      AEGP_LayerFlag_VIDEO_ACTIVE = 1 << 0
      ae.sdk.AEGP_SetLayerFlag(layerH, AEGP_LayerFlag_VIDEO_ACTIVE, 1)

      # レイヤーをロック
      AEGP_LayerFlag_LOCKED = 1 << 5
      ae.sdk.AEGP_SetLayerFlag(layerH, AEGP_LayerFlag_LOCKED, 1)

レイヤー品質設定
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerQuality(layerH: int) -> int

   レイヤーの品質設定を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: 品質設定（AEGP_LayerQuality）
   :rtype: int

   **例**:

   .. code-block:: python

      quality = ae.sdk.AEGP_GetLayerQuality(layerH)
      if quality == 3:  # AEGP_LayerQual_BEST
          print("最高品質")

.. function:: AEGP_SetLayerQuality(layerH: int, quality: int) -> None

   レイヤーの品質設定を変更します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param quality: 品質設定（0-3）
   :type quality: int

   **品質設定の選択肢**:

   - ``0``: AEGP_LayerQual_NONE
   - ``1``: AEGP_LayerQual_WIREFRAME（ワイヤーフレーム）
   - ``2``: AEGP_LayerQual_DRAFT（ドラフト）
   - ``3``: AEGP_LayerQual_BEST（最高品質）

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetLayerQuality(layerH, 3)  # 最高品質に設定

.. function:: AEGP_GetLayerSamplingQuality(layerH: int) -> int

   レイヤーのサンプリング品質を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: サンプリング品質（0: Bilinear, 1: Bicubic）
   :rtype: int

.. function:: AEGP_SetLayerSamplingQuality(layerH: int, quality: int) -> None

   レイヤーのサンプリング品質を設定します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param quality: サンプリング品質（0: Bilinear, 1: Bicubic）
   :type quality: int

レイヤーラベル
~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerLabel(layerH: int) -> int

   レイヤーのラベル（カラーラベル）を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: ラベルID（0-16）
   :rtype: int

.. function:: AEGP_SetLayerLabel(layerH: int, label: int) -> None

   レイヤーのラベル（カラーラベル）を設定します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param label: ラベルID（0: None, 1-16: カラーラベル）
   :type label: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetLayerLabel(layerH, 5)  # ラベル5を設定

ブレンドモード・トラックマット
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerTransferMode(layerH: int) -> dict

   レイヤーのブレンドモード、フラグ、トラックマット設定を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: ブレンドモード情報の辞書
   :rtype: dict

   **戻り値の構造** (``AEGP_LayerTransferMode``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``mode``
        - int
        - ブレンドモード（PF_TransferMode）
      * - ``flags``
        - int
        - 転送フラグ
      * - ``track_matte``
        - int
        - トラックマット設定（AEGP_TrackMatte）

   **例**:

   .. code-block:: python

      transfer = ae.sdk.AEGP_GetLayerTransferMode(layerH)
      print(f"ブレンドモード: {transfer['mode']}")
      print(f"トラックマット: {transfer['track_matte']}")

.. function:: AEGP_SetLayerTransferMode(layerH: int, mode: int, flags: int, track_matte: int) -> None

   レイヤーのブレンドモード、フラグ、トラックマット設定を変更します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param mode: ブレンドモード（0-50程度）
   :type mode: int
   :param flags: 転送フラグ
   :type flags: int
   :param track_matte: トラックマット設定（0-4）
   :type track_matte: int

   **トラックマット設定**:

   - ``0``: AEGP_TrackMatte_NO_TRACK_MATTE
   - ``1``: AEGP_TrackMatte_ALPHA
   - ``2``: AEGP_TrackMatte_NOT_ALPHA
   - ``3``: AEGP_TrackMatte_LUMA
   - ``4``: AEGP_TrackMatte_NOT_LUMA

   **例**:

   .. code-block:: python

      # 乗算ブレンドモードに設定
      ae.sdk.AEGP_SetLayerTransferMode(layerH, 5, 0, 0)

.. function:: AEGP_DoesLayerHaveTrackMatte(layerH: int) -> bool

   レイヤーがトラックマットを持っているか確認します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: トラックマットを持っている場合は ``True``
   :rtype: bool

.. function:: AEGP_IsLayerUsedAsTrackMatte(layerH: int, fill_must_be_active: bool) -> bool

   レイヤーがトラックマットとして使用されているか確認します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param fill_must_be_active: 塗りレイヤーがアクティブである必要があるか
   :type fill_must_be_active: bool
   :return: トラックマットとして使用されている場合は ``True``
   :rtype: bool

.. function:: AEGP_GetTrackMatteLayer(layerH: int) -> int

   レイヤーのトラックマットレイヤーを取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: トラックマットレイヤーハンドル。トラックマットがない場合は0
   :rtype: int

   .. note::
      この関数は LayerSuite9（AE 23.0以降）でのみ利用可能です。

.. function:: AEGP_SetTrackMatte(layerH: int, track_matte_layerH: int, track_matte_type: int) -> None

   レイヤーにトラックマットを設定します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param track_matte_layerH: トラックマットレイヤーハンドル
   :type track_matte_layerH: int
   :param track_matte_type: トラックマットのタイプ（0-4）
   :type track_matte_type: int

   .. note::
      この関数は LayerSuite9（AE 23.0以降）でのみ利用可能です。

.. function:: AEGP_RemoveTrackMatte(layerH: int) -> None

   レイヤーからトラックマットを削除します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int

   .. note::
      この関数は LayerSuite9（AE 23.0以降）でのみ利用可能です。

タイムストレッチ
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerStretch(layerH: int) -> tuple

   レイヤーのタイムストレッチ（時間伸縮）を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: (分子, 分母) のタプル
   :rtype: tuple

   .. note::
      100% = (100, 100)、50% = (50, 100)、200% = (200, 100)

   **例**:

   .. code-block:: python

      num, den = ae.sdk.AEGP_GetLayerStretch(layerH)
      percent = (num / den) * 100
      print(f"タイムストレッチ: {percent}%")

.. function:: AEGP_SetLayerStretch(layerH: int, num: int, den: int) -> None

   レイヤーのタイムストレッチを設定します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param num: 分子
   :type num: int
   :param den: 分母（0以外）
   :type den: int

   **例**:

   .. code-block:: python

      # 50%スローモーション
      ae.sdk.AEGP_SetLayerStretch(layerH, 50, 100)

レイヤー順序
~~~~~~~~~~~~

.. function:: AEGP_ReorderLayer(layerH: int, index: int) -> None

   レイヤーの順序を変更します（**Undo可能**）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param index: 新しいインデックス（0から始まる）
   :type index: int

   **例**:

   .. code-block:: python

      # レイヤーを先頭に移動
      ae.sdk.AEGP_ReorderLayer(layerH, 0)

レイヤーの追加・削除・複製
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_IsAddLayerValid(itemH: int, compH: int) -> bool

   アイテムをコンポジションにレイヤーとして追加可能か確認します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param compH: コンポジションハンドル
   :type compH: int
   :return: 追加可能な場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_IsAddLayerValid(itemH, compH):
          ae.sdk.AEGP_AddLayer(itemH, compH)

.. function:: AEGP_AddLayer(itemH: int, compH: int) -> int

   アイテムをコンポジションにレイヤーとして追加します（**Undo可能**）。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param compH: コンポジションハンドル
   :type compH: int
   :return: 追加されたレイヤーハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      new_layerH = ae.sdk.AEGP_AddLayer(itemH, compH)
      print(f"レイヤーを追加しました: {new_layerH}")

時間変換
~~~~~~~~

.. function:: AEGP_ConvertCompToLayerTime(layerH: int, comp_time: float) -> float

   コンポジション時間をレイヤー時間に変換します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param comp_time: コンポジション時間（秒）
   :type comp_time: float
   :return: レイヤー時間（秒）
   :rtype: float

   .. note::
      タイムストレッチや In Point の影響を考慮した変換が行われます。

   **例**:

   .. code-block:: python

      layer_time = ae.sdk.AEGP_ConvertCompToLayerTime(layerH, 5.0)
      print(f"レイヤー時間: {layer_time}秒")

.. function:: AEGP_ConvertLayerToCompTime(layerH: int, layer_time: float) -> float

   レイヤー時間をコンポジション時間に変換します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param layer_time: レイヤー時間（秒）
   :type layer_time: float
   :return: コンポジション時間（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      comp_time = ae.sdk.AEGP_ConvertLayerToCompTime(layerH, 2.0)
      print(f"コンポジション時間: {comp_time}秒")

レイヤー時間情報
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerCurrentTime(layerH: int, time_mode: int) -> float

   レイヤーの現在時刻を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param time_mode: 時間モード（0: Comp Time, 1: Layer Time）
   :type time_mode: int
   :return: 現在時刻（秒）
   :rtype: float

Transform行列
~~~~~~~~~~~~~

.. function:: AEGP_GetLayerToWorldXform(layerH: int, comp_time: float) -> list

   レイヤーのローカル座標からワールド座標への変換行列を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param comp_time: コンポジション時間（秒）
   :type comp_time: float
   :return: 4x4変換行列（リストのリスト）
   :rtype: list

   **例**:

   .. code-block:: python

      matrix = ae.sdk.AEGP_GetLayerToWorldXform(layerH, 0.0)
      # matrix[0][0], matrix[0][1], ... matrix[3][3]

.. function:: AEGP_GetLayerToWorldXformFromView(layerH: int, view_time: float, comp_time: float) -> list

   指定されたビュー時間でのレイヤーの変換行列を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param view_time: ビュー時間（秒）
   :type view_time: float
   :param comp_time: コンポジション時間（秒）
   :type comp_time: float
   :return: 4x4変換行列（リストのリスト）
   :rtype: list

レイヤー境界・状態
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerMaskedBounds(layerH: int, time_mode: int, time: float) -> tuple

   マスク適用後のレイヤー境界を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param time_mode: 時間モード（0: Comp Time, 1: Layer Time）
   :type time_mode: int
   :param time: 時間（秒）
   :type time: float
   :return: (left, top, right, bottom) のタプル
   :rtype: tuple

.. function:: AEGP_GetLayerDancingRandValue(layerH: int, comp_time: float) -> int

   レイヤーのランダムシード値（wiggle等で使用）を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param comp_time: コンポジション時間（秒）
   :type comp_time: float
   :return: ランダムシード値
   :rtype: int

.. function:: AEGP_IsLayerVideoReallyOn(layerH: int) -> bool

   レイヤーのビデオトラックが実際に有効か確認します（親の影響も考慮）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: ビデオトラックが有効な場合は ``True``
   :rtype: bool

.. function:: AEGP_IsLayerAudioReallyOn(layerH: int) -> bool

   レイヤーのオーディオトラックが実際に有効か確認します（親の影響も考慮）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: オーディオトラックが有効な場合は ``True``
   :rtype: bool

.. function:: AEGP_IsVideoActive(layerH: int, time_mode: int, time: float) -> bool

   指定時刻でレイヤーのビデオが表示されるか確認します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param time_mode: 時間モード（0: Comp Time, 1: Layer Time）
   :type time_mode: int
   :param time: 時間（秒）
   :type time: float
   :return: ビデオが表示される場合は ``True``
   :rtype: bool

ストリーム取得
~~~~~~~~~~~~~~

.. function:: AEGP_GetNewLayerStream(plugin_id: int, layerH: int, stream_type: int) -> int

   レイヤーのストリーム（プロパティ）を取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param layerH: レイヤーハンドル
   :type layerH: int
   :param stream_type: ストリームタイプ（AEGP_LayerStream）
   :type stream_type: int
   :return: ストリームハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      # Position ストリームを取得（stream_type = 1）
      streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, 1)

.. function:: AEGP_DisposeStream(streamH: int) -> None

   ストリームハンドルを解放します。

   :param streamH: ストリームハンドル
   :type streamH: int

   .. note::
      ``AEGP_GetNewLayerStream()`` で取得したストリームは使用後に必ず解放してください。

   **例**:

   .. code-block:: python

      streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, 1)
      try:
          # ストリームを使用
          pass
      finally:
          ae.sdk.AEGP_DisposeStream(streamH)

使用例
------

すべてのレイヤーをリスト表示
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def list_all_layers():
       """コンポジション内のすべてのレイヤーをリスト表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

       # 最初のコンポジションを取得
       num_items = ae.sdk.AEGP_GetNumItems(rootFolderH)
       compH = None
       for i in range(num_items):
           itemH = ae.sdk.AEGP_GetItemByIndex(rootFolderH, i)
           item_type = ae.sdk.AEGP_GetItemType(itemH)
           if item_type == 1:  # AEGP_ItemType_COMP
               compH = ae.sdk.AEGP_GetCompFromItem(itemH)
               break

       if not compH:
           print("コンポジションが見つかりません")
           return

       # レイヤー一覧を表示
       num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
       print(f"=== レイヤー数: {num_layers} ===")

       for i in range(num_layers):
           layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)
           name = ae.sdk.AEGP_GetLayerName(plugin_id, layerH)
           index = ae.sdk.AEGP_GetLayerIndex(layerH)
           is_3d = ae.sdk.AEGP_IsLayer3D(layerH)
           quality = ae.sdk.AEGP_GetLayerQuality(layerH)

           quality_str = ["None", "Wireframe", "Draft", "Best"][quality]
           dimension = "3D" if is_3d else "2D"

           print(f"{index}: {name} ({dimension}, {quality_str})")

   # 実行
   list_all_layers()

レイヤー品質の一括設定
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_all_layers_quality(quality=3):
       """すべてのレイヤーの品質を変更"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

       # コンポジションを取得
       num_items = ae.sdk.AEGP_GetNumItems(rootFolderH)
       for i in range(num_items):
           itemH = ae.sdk.AEGP_GetItemByIndex(rootFolderH, i)
           item_type = ae.sdk.AEGP_GetItemType(itemH)
           if item_type == 1:  # AEGP_ItemType_COMP
               compH = ae.sdk.AEGP_GetCompFromItem(itemH)

               # Undoグループ開始
               ae.sdk.AEGP_StartUndoGroup("レイヤー品質変更")

               try:
                   num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
                   for j in range(num_layers):
                       layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, j)
                       ae.sdk.AEGP_SetLayerQuality(layerH, quality)

                   print(f"コンポジション内の{num_layers}個のレイヤー品質を変更しました")

               finally:
                   # Undoグループ終了
                   ae.sdk.AEGP_EndUndoGroup()

   # 実行 - すべてのレイヤーを最高品質に設定
   set_all_layers_quality(3)

親子階層の可視化
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def visualize_layer_hierarchy():
       """レイヤーの親子階層を可視化"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

       # コンポジションを取得
       num_items = ae.sdk.AEGP_GetNumItems(rootFolderH)
       for i in range(num_items):
           itemH = ae.sdk.AEGP_GetItemByIndex(rootFolderH, i)
           item_type = ae.sdk.AEGP_GetItemType(itemH)
           if item_type == 1:  # AEGP_ItemType_COMP
               compH = ae.sdk.AEGP_GetCompFromItem(itemH)
               comp_name = ae.sdk.AEGP_GetItemName(plugin_id, itemH)

               print(f"\n=== コンポジション: {comp_name} ===")

               num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)

               # すべてのレイヤーの親子関係を記録
               layers_info = []
               for j in range(num_layers):
                   layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, j)
                   name = ae.sdk.AEGP_GetLayerName(plugin_id, layerH)
                   parentH = ae.sdk.AEGP_GetLayerParent(layerH)

                   parent_name = None
                   if parentH:
                       parent_name = ae.sdk.AEGP_GetLayerName(plugin_id, parentH)

                   layers_info.append((name, parent_name))

               # 階層を表示
               def print_hierarchy(layer_name, indent=0):
                   """階層を再帰的に表示"""
                   prefix = "  " * indent + "|- "
                   print(f"{prefix}{layer_name}")

                   # 子レイヤーを探す
                   for name, parent_name in layers_info:
                       if parent_name == layer_name:
                           print_hierarchy(name, indent + 1)

               # 親を持たないレイヤーから開始
               for name, parent_name in layers_info:
                   if parent_name is None:
                       print_hierarchy(name)

   # 実行
   visualize_layer_hierarchy()

レイヤーのフィルタリング
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_3d_layers():
       """3Dレイヤーのみを取得"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

       num_items = ae.sdk.AEGP_GetNumItems(rootFolderH)
       for i in range(num_items):
           itemH = ae.sdk.AEGP_GetItemByIndex(rootFolderH, i)
           item_type = ae.sdk.AEGP_GetItemType(itemH)
           if item_type == 1:  # AEGP_ItemType_COMP
               compH = ae.sdk.AEGP_GetCompFromItem(itemH)
               num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)

               print("=== 3Dレイヤー ===")
               for j in range(num_layers):
                   layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, j)

                   if ae.sdk.AEGP_IsLayer3D(layerH):
                       name = ae.sdk.AEGP_GetLayerName(plugin_id, layerH)
                       obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)

                       type_names = ["None", "AV", "Light", "Camera", "Text", "Vector"]
                       type_str = type_names[obj_type] if obj_type < len(type_names) else "Unknown"

                       print(f"  - {name} ({type_str})")

   # 実行
   get_3d_layers()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **レイヤーハンドルの有効期限**

   レイヤーハンドルは、レイヤーが存在する間のみ有効です。レイヤーを削除した後は、ハンドルは無効になります。

2. **循環参照の禁止**

   ``AEGP_SetLayerParent()`` では、循環参照（自分自身、子孫、祖先を親に設定）はエラーになります。

3. **Undo操作**

   多くの設定変更操作はUndo可能です。複数の変更を行う場合は、``AEGP_StartUndoGroup()`` でグループ化してください。

4. **ストリームの解放**

   ``AEGP_GetNewLayerStream()`` で取得したストリームは、使用後に必ず ``AEGP_DisposeStream()`` で解放してください。

5. **plugin_idの取得**

   ``plugin_id`` は、``AEGP_GetPluginID()`` で1度だけ取得し、変数に保存して再利用することを推奨します。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

レイヤー存在確認
^^^^^^^^^^^^^^^^

.. code-block:: python

   layerH = ae.sdk.AEGP_GetActiveLayer()
   if layerH == 0:
       print("アクティブなレイヤーがありません")
       return

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("レイヤー設定変更")
   try:
       ae.sdk.AEGP_SetLayerQuality(layerH, 3)
       ae.sdk.AEGP_SetLayerFlag(layerH, flag, 1)
   finally:
       ae.sdk.AEGP_EndUndoGroup()

ストリームの安全な使用
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, stream_type)
   try:
       # ストリーム操作
       pass
   finally:
       ae.sdk.AEGP_DisposeStream(streamH)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, index)
   except Exception as e:
       print(f"レイヤー取得エラー: {e}")

関連項目
--------

- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`AEGP_ItemSuite9` - プロジェクトアイテム管理
- :doc:`AEGP_StreamSuite6` - プロパティストリーム管理
- :doc:`AEGP_UtilitySuite6` - Undo管理、エラー処理
- :doc:`index` - 低レベルAPI概要
