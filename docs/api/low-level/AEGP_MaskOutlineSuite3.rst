AEGP_MaskOutlineSuite3
======================

.. currentmodule:: ae.sdk

AEGP_MaskOutlineSuite3は、After Effectsマスクのアウトライン（ベジェパス）を詳細に操作するためのSDK APIです。

概要
----

**実装状況**: 11/11関数実装 ✅

AEGP_MaskOutlineSuite3は以下の機能を提供します:

- マスクのアウトラインのオープン/クローズ状態の取得・設定
- 頂点（Vertex）の取得・設定・作成・削除
- フェザー（Feather）の取得・設定・作成・削除

.. note::
   このSuiteは :doc:`AEGP_MaskSuite6` と組み合わせて使用します。
   MaskSuiteでマスクハンドル (``AEGP_MaskRefH``) を取得した後、このSuiteで詳細なパス操作を行います。

基本概念
--------

マスクアウトラインハンドル (AEGP_MaskOutlineValH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

マスクのアウトライン（ベジェパス）を識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - マスクアウトラインハンドルは、ストリーム値から取得します
   - マスクパスプロパティのストリームから ``AEGP_GetNewStreamValue()`` で取得
   - 使用後は適切に破棄する必要があります

オープン/クローズ状態
~~~~~~~~~~~~~~~~~~~~

マスクアウトラインには「オープン」と「クローズ」の2つの状態があります:

- **クローズ（閉じたパス）**: 最後の頂点が最初の頂点に接続され、閉じた形状を形成
- **オープン（開いたパス）**: 最後の頂点が最初の頂点に接続されず、線として描画

頂点 (Vertex)
~~~~~~~~~~~~~

ベジェパスを構成する制御点。各頂点は以下の情報を持ちます:

.. list-table::
   :header-rows: 1

   * - プロパティ
     - 型
     - 説明
   * - ``x``
     - float
     - X座標
   * - ``y``
     - float
     - Y座標
   * - ``tan_in_x``
     - float
     - 入力接線ハンドルのX座標（相対）
   * - ``tan_in_y``
     - float
     - 入力接線ハンドルのY座標（相対）
   * - ``tan_out_x``
     - float
     - 出力接線ハンドルのX座標（相対）
   * - ``tan_out_y``
     - float
     - 出力接線ハンドルのY座標（相対）

フェザー (Feather)
~~~~~~~~~~~~~~~~~~

マスクエッジのぼかし効果。マスクのアウトライン上の任意の位置にフェザーポイントを配置できます。

.. list-table::
   :header-rows: 1

   * - プロパティ
     - 型
     - 説明
   * - ``segment``
     - int
     - フェザーが配置されるセグメントのインデックス
   * - ``segment_sF``
     - float
     - セグメント上の位置（0.0〜1.0）
   * - ``radiusF``
     - float
     - フェザーの半径
   * - ``ui_corner_angleF``
     - float
     - UIでの角度表示
   * - ``tensionF``
     - float
     - フェザーカーブのテンション
   * - ``interp``
     - int
     - フェザー補間タイプ（AEGP_MaskFeatherInterp）
   * - ``type``
     - int
     - フェザータイプ（AEGP_MaskFeatherType）

API リファレンス
----------------

オープン/クローズ状態
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_IsMaskOutlineOpen(mask_outlineH: int) -> bool

   マスクアウトラインがオープン（開いた）状態かどうかを確認します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :return: オープン状態の場合は ``True``、クローズ状態の場合は ``False``
   :rtype: bool

   **例**:

   .. code-block:: python

      is_open = ae.sdk.AEGP_IsMaskOutlineOpen(mask_outlineH)
      print(f"マスクはオープン: {is_open}")

.. function:: AEGP_SetMaskOutlineOpen(mask_outlineH: int, open: bool) -> None

   マスクアウトラインのオープン/クローズ状態を設定します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param open: ``True`` でオープン（開いた）状態、``False`` でクローズ（閉じた）状態
   :type open: bool

   **例**:

   .. code-block:: python

      # マスクを閉じた状態に設定
      ae.sdk.AEGP_SetMaskOutlineOpen(mask_outlineH, False)

セグメント情報
~~~~~~~~~~~~~~

.. function:: AEGP_GetMaskOutlineNumSegments(mask_outlineH: int) -> int

   マスクアウトラインのセグメント数を取得します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :return: セグメント数
   :rtype: int

   .. note::
      セグメント数は頂点数と同じです（クローズパスの場合）。
      オープンパスの場合は、頂点数 - 1 になります。

   **例**:

   .. code-block:: python

      num_segments = ae.sdk.AEGP_GetMaskOutlineNumSegments(mask_outlineH)
      print(f"セグメント数: {num_segments}")

頂点操作
~~~~~~~~

.. function:: AEGP_GetMaskOutlineVertexInfo(mask_outlineH: int, index: int) -> dict

   指定インデックスの頂点情報を取得します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param index: 頂点のインデックス（0から始まる）
   :type index: int
   :return: 頂点情報の辞書
   :rtype: dict

   **戻り値の構造** (``AEGP_MaskVertex``):

   .. code-block:: python

      {
          "x": float,           # X座標
          "y": float,           # Y座標
          "tan_in_x": float,    # 入力接線X（相対）
          "tan_in_y": float,    # 入力接線Y（相対）
          "tan_out_x": float,   # 出力接線X（相対）
          "tan_out_y": float    # 出力接線Y（相対）
      }

   **例**:

   .. code-block:: python

      vertex = ae.sdk.AEGP_GetMaskOutlineVertexInfo(mask_outlineH, 0)
      print(f"頂点座標: ({vertex['x']}, {vertex['y']})")
      print(f"接線ハンドル: in=({vertex['tan_in_x']}, {vertex['tan_in_y']}), "
            f"out=({vertex['tan_out_x']}, {vertex['tan_out_y']})")

.. function:: AEGP_SetMaskOutlineVertexInfo(mask_outlineH: int, index: int, vertex: dict) -> None

   指定インデックスの頂点情報を設定します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param index: 頂点のインデックス（0から始まる）
   :type index: int
   :param vertex: 頂点情報の辞書（すべてのキーを含む必要があります）
   :type vertex: dict

   **頂点情報の構造**:

   辞書には以下のキーが含まれている必要があります:

   - ``x``: X座標 (float)
   - ``y``: Y座標 (float)
   - ``tan_in_x``: 入力接線X（相対） (float)
   - ``tan_in_y``: 入力接線Y（相対） (float)
   - ``tan_out_x``: 出力接線X（相対） (float)
   - ``tan_out_y``: 出力接線Y（相対） (float)

   **例**:

   .. code-block:: python

      # 頂点を取得
      vertex = ae.sdk.AEGP_GetMaskOutlineVertexInfo(mask_outlineH, 0)

      # 座標を変更
      vertex["x"] = 100.0
      vertex["y"] = 150.0

      # 設定を反映
      ae.sdk.AEGP_SetMaskOutlineVertexInfo(mask_outlineH, 0, vertex)

.. function:: AEGP_CreateVertex(mask_outlineH: int, index: int) -> None

   指定位置に新しい頂点を作成します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param index: 新しい頂点を挿入するインデックス
   :type index: int

   .. note::
      頂点は指定インデックスの位置に挿入されます。既存の頂点は後方にシフトされます。

   **例**:

   .. code-block:: python

      # 2番目の位置に新しい頂点を作成
      ae.sdk.AEGP_CreateVertex(mask_outlineH, 1)

.. function:: AEGP_DeleteVertex(mask_outlineH: int, index: int) -> None

   指定インデックスの頂点を削除します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param index: 削除する頂点のインデックス
   :type index: int

   .. warning::
      マスクには最低3つの頂点が必要です。それ以下にすると、マスクが無効になる可能性があります。

   **例**:

   .. code-block:: python

      # 2番目の頂点を削除
      ae.sdk.AEGP_DeleteVertex(mask_outlineH, 1)

フェザー操作
~~~~~~~~~~~~

.. function:: AEGP_GetMaskOutlineNumFeathers(mask_outlineH: int) -> int

   マスクアウトラインのフェザー数を取得します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :return: フェザー数
   :rtype: int

   **例**:

   .. code-block:: python

      num_feathers = ae.sdk.AEGP_GetMaskOutlineNumFeathers(mask_outlineH)
      print(f"フェザー数: {num_feathers}")

.. function:: AEGP_GetMaskOutlineFeatherInfo(mask_outlineH: int, which_feather: int) -> dict

   指定インデックスのフェザー情報を取得します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param which_feather: フェザーのインデックス（0から始まる）
   :type which_feather: int
   :return: フェザー情報の辞書
   :rtype: dict

   **戻り値の構造** (``AEGP_MaskFeather``):

   .. code-block:: python

      {
          "segment": int,            # セグメントインデックス
          "segment_sF": float,       # セグメント上の位置（0.0〜1.0）
          "radiusF": float,          # フェザー半径
          "ui_corner_angleF": float, # UI角度
          "tensionF": float,         # テンション
          "interp": int,             # 補間タイプ
          "type": int                # フェザータイプ
      }

   **例**:

   .. code-block:: python

      feather = ae.sdk.AEGP_GetMaskOutlineFeatherInfo(mask_outlineH, 0)
      print(f"セグメント: {feather['segment']}, 位置: {feather['segment_sF']}")
      print(f"半径: {feather['radiusF']}")

.. function:: AEGP_SetMaskOutlineFeatherInfo(mask_outlineH: int, which_feather: int, feather: dict) -> None

   指定インデックスのフェザー情報を設定します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param which_feather: フェザーのインデックス（0から始まる）
   :type which_feather: int
   :param feather: フェザー情報の辞書
   :type feather: dict

   **フェザー情報の構造**:

   辞書には以下のキーが含まれている必要があります:

   - ``segment``: セグメントインデックス (int)
   - ``segment_sF``: セグメント上の位置（0.0〜1.0） (float)
   - ``radiusF``: フェザー半径 (float)
   - ``ui_corner_angleF``: UI角度 (float)
   - ``tensionF``: テンション (float)
   - ``interp``: 補間タイプ (int)
   - ``type``: フェザータイプ (int)

   **例**:

   .. code-block:: python

      # フェザーを取得
      feather = ae.sdk.AEGP_GetMaskOutlineFeatherInfo(mask_outlineH, 0)

      # 半径を変更
      feather["radiusF"] = 20.0

      # 設定を反映
      ae.sdk.AEGP_SetMaskOutlineFeatherInfo(mask_outlineH, 0, feather)

.. function:: AEGP_CreateMaskOutlineFeather(mask_outlineH: int, feather: dict) -> int

   新しいフェザーを作成します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param feather: フェザー情報の辞書
   :type feather: dict
   :return: 作成されたフェザーのインデックス
   :rtype: int

   **例**:

   .. code-block:: python

      # 新しいフェザーを作成
      new_feather = {
          "segment": 0,
          "segment_sF": 0.5,     # セグメントの中間
          "radiusF": 15.0,
          "ui_corner_angleF": 0.0,
          "tensionF": 0.0,
          "interp": 0,
          "type": 0
      }

      feather_index = ae.sdk.AEGP_CreateMaskOutlineFeather(mask_outlineH, new_feather)
      print(f"フェザーを作成しました: インデックス {feather_index}")

.. function:: AEGP_DeleteMaskOutlineFeather(mask_outlineH: int, index: int) -> None

   指定インデックスのフェザーを削除します。

   :param mask_outlineH: マスクアウトラインハンドル
   :type mask_outlineH: int
   :param index: 削除するフェザーのインデックス
   :type index: int

   **例**:

   .. code-block:: python

      # 最初のフェザーを削除
      ae.sdk.AEGP_DeleteMaskOutlineFeather(mask_outlineH, 0)

使用例
------

マスクアウトラインの基本操作
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def analyze_mask_outline(layerH, mask_index):
       """マスクアウトラインの詳細情報を取得"""
       # マスクハンドルを取得
       maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, mask_index)

       try:
           # マスクパスストリームを取得
           streamH = ae.sdk.AEGP_GetNewMaskStream(maskH, ae.sdk.AEGP_MaskStream_OUTLINE)

           try:
               # 現在時刻を取得
               comp_time = 0.0  # または ae.sdk.AEGP_GetCompTime(...) で取得

               # ストリーム値（アウトライン）を取得
               stream_value = ae.sdk.AEGP_GetNewStreamValue(
                   streamH, ae.sdk.AEGP_LTimeMode_CompTime, comp_time, False
               )

               # AEGP_MaskOutlineValH として扱う
               mask_outlineH = stream_value

               try:
                   # オープン/クローズ状態を確認
                   is_open = ae.sdk.AEGP_IsMaskOutlineOpen(mask_outlineH)
                   print(f"マスクはオープン: {is_open}")

                   # セグメント数を取得
                   num_segments = ae.sdk.AEGP_GetMaskOutlineNumSegments(mask_outlineH)
                   print(f"セグメント数: {num_segments}")

                   # 各頂点を列挙
                   print("\n頂点情報:")
                   for i in range(num_segments):
                       vertex = ae.sdk.AEGP_GetMaskOutlineVertexInfo(mask_outlineH, i)
                       print(f"  頂点 {i}: ({vertex['x']:.2f}, {vertex['y']:.2f})")

                   # フェザー情報を取得
                   num_feathers = ae.sdk.AEGP_GetMaskOutlineNumFeathers(mask_outlineH)
                   print(f"\nフェザー数: {num_feathers}")

                   for i in range(num_feathers):
                       feather = ae.sdk.AEGP_GetMaskOutlineFeatherInfo(mask_outlineH, i)
                       print(f"  フェザー {i}: セグメント={feather['segment']}, "
                             f"半径={feather['radiusF']:.2f}")

               finally:
                   # ストリーム値を破棄
                   ae.sdk.AEGP_DisposeStreamValue(stream_value)

           finally:
               # ストリームを破棄
               ae.sdk.AEGP_DisposeStream(streamH)

       finally:
           # マスクハンドルを破棄
           ae.sdk.AEGP_DisposeMask(maskH)

   # 使用例
   plugin_id = ae.sdk.AEGP_GetPluginID()
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetActiveComp()
   layerH = ae.sdk.AEGP_GetCompLayerByIndex(itemH, 0)

   analyze_mask_outline(layerH, 0)

マスクアウトラインの変更
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def modify_mask_outline(layerH, mask_index):
       """マスクアウトラインを変更"""
       # マスクハンドルを取得
       maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, mask_index)

       try:
           # Undoグループを開始
           ae.sdk.AEGP_StartUndoGroup("マスクアウトライン変更")

           try:
               # マスクパスストリームを取得
               streamH = ae.sdk.AEGP_GetNewMaskStream(maskH, ae.sdk.AEGP_MaskStream_OUTLINE)

               try:
                   # ストリーム値を取得
                   comp_time = 0.0
                   stream_value = ae.sdk.AEGP_GetNewStreamValue(
                       streamH, ae.sdk.AEGP_LTimeMode_CompTime, comp_time, False
                   )

                   mask_outlineH = stream_value

                   try:
                       # マスクを閉じた状態に変更
                       ae.sdk.AEGP_SetMaskOutlineOpen(mask_outlineH, False)

                       # 最初の頂点を移動
                       vertex = ae.sdk.AEGP_GetMaskOutlineVertexInfo(mask_outlineH, 0)
                       vertex["x"] += 50.0
                       vertex["y"] += 30.0
                       ae.sdk.AEGP_SetMaskOutlineVertexInfo(mask_outlineH, 0, vertex)

                       # 変更をストリームに書き戻す
                       ae.sdk.AEGP_SetStreamValue(
                           streamH, stream_value
                       )

                   finally:
                       ae.sdk.AEGP_DisposeStreamValue(stream_value)

               finally:
                   ae.sdk.AEGP_DisposeStream(streamH)

           finally:
               ae.sdk.AEGP_EndUndoGroup()

       finally:
           ae.sdk.AEGP_DisposeMask(maskH)

   # 使用例
   plugin_id = ae.sdk.AEGP_GetPluginID()
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetActiveComp()
   layerH = ae.sdk.AEGP_GetCompLayerByIndex(itemH, 0)

   modify_mask_outline(layerH, 0)

頂点の追加・削除
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def add_vertex_to_mask(layerH, mask_index, insert_index):
       """マスクに新しい頂点を追加"""
       maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, mask_index)

       try:
           ae.sdk.AEGP_StartUndoGroup("頂点追加")

           try:
               streamH = ae.sdk.AEGP_GetNewMaskStream(maskH, ae.sdk.AEGP_MaskStream_OUTLINE)

               try:
                   stream_value = ae.sdk.AEGP_GetNewStreamValue(
                       streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False
                   )

                   mask_outlineH = stream_value

                   try:
                       # 頂点を追加
                       ae.sdk.AEGP_CreateVertex(mask_outlineH, insert_index)

                       # 追加した頂点の位置を設定
                       vertex = ae.sdk.AEGP_GetMaskOutlineVertexInfo(mask_outlineH, insert_index)
                       vertex["x"] = 200.0
                       vertex["y"] = 100.0
                       ae.sdk.AEGP_SetMaskOutlineVertexInfo(mask_outlineH, insert_index, vertex)

                       # 変更を書き戻す
                       ae.sdk.AEGP_SetStreamValue(streamH, stream_value)

                       print(f"頂点を追加しました: インデックス {insert_index}")

                   finally:
                       ae.sdk.AEGP_DisposeStreamValue(stream_value)

               finally:
                   ae.sdk.AEGP_DisposeStream(streamH)

           finally:
               ae.sdk.AEGP_EndUndoGroup()

       finally:
           ae.sdk.AEGP_DisposeMask(maskH)

   # 使用例
   plugin_id = ae.sdk.AEGP_GetPluginID()
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetActiveComp()
   layerH = ae.sdk.AEGP_GetCompLayerByIndex(itemH, 0)

   add_vertex_to_mask(layerH, 0, 1)  # 2番目の位置に頂点を追加

フェザーの管理
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def add_feather_to_mask(layerH, mask_index, segment, position, radius):
       """マスクにフェザーを追加"""
       maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, mask_index)

       try:
           ae.sdk.AEGP_StartUndoGroup("フェザー追加")

           try:
               streamH = ae.sdk.AEGP_GetNewMaskStream(maskH, ae.sdk.AEGP_MaskStream_OUTLINE)

               try:
                   stream_value = ae.sdk.AEGP_GetNewStreamValue(
                       streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False
                   )

                   mask_outlineH = stream_value

                   try:
                       # フェザーを作成
                       new_feather = {
                           "segment": segment,
                           "segment_sF": position,
                           "radiusF": radius,
                           "ui_corner_angleF": 0.0,
                           "tensionF": 0.0,
                           "interp": 0,
                           "type": 0
                       }

                       feather_index = ae.sdk.AEGP_CreateMaskOutlineFeather(
                           mask_outlineH, new_feather
                       )

                       # 変更を書き戻す
                       ae.sdk.AEGP_SetStreamValue(streamH, stream_value)

                       print(f"フェザーを追加しました: インデックス {feather_index}")

                   finally:
                       ae.sdk.AEGP_DisposeStreamValue(stream_value)

               finally:
                   ae.sdk.AEGP_DisposeStream(streamH)

           finally:
               ae.sdk.AEGP_EndUndoGroup()

       finally:
           ae.sdk.AEGP_DisposeMask(maskH)

   # 使用例
   plugin_id = ae.sdk.AEGP_GetPluginID()
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetActiveComp()
   layerH = ae.sdk.AEGP_GetCompLayerByIndex(itemH, 0)

   # セグメント0の中間（0.5）に半径20のフェザーを追加
   add_feather_to_mask(layerH, 0, segment=0, position=0.5, radius=20.0)

マスクアウトラインのコピー
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def copy_mask_outline(src_layerH, src_mask_index, dst_layerH, dst_mask_index):
       """マスクアウトラインを別のマスクにコピー"""
       # ソースマスクから情報を取得
       src_maskH = ae.sdk.AEGP_GetLayerMaskByIndex(src_layerH, src_mask_index)

       try:
           src_streamH = ae.sdk.AEGP_GetNewMaskStream(src_maskH, ae.sdk.AEGP_MaskStream_OUTLINE)

           try:
               src_value = ae.sdk.AEGP_GetNewStreamValue(
                   src_streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False
               )

               src_outlineH = src_value

               try:
                   # 頂点情報をすべてコピー
                   num_segments = ae.sdk.AEGP_GetMaskOutlineNumSegments(src_outlineH)
                   is_open = ae.sdk.AEGP_IsMaskOutlineOpen(src_outlineH)
                   vertices = []

                   for i in range(num_segments):
                       vertex = ae.sdk.AEGP_GetMaskOutlineVertexInfo(src_outlineH, i)
                       vertices.append(vertex)

                   # デスティネーションマスクに設定
                   dst_maskH = ae.sdk.AEGP_GetLayerMaskByIndex(dst_layerH, dst_mask_index)

                   try:
                       ae.sdk.AEGP_StartUndoGroup("マスクアウトラインコピー")

                       try:
                           dst_streamH = ae.sdk.AEGP_GetNewMaskStream(
                               dst_maskH, ae.sdk.AEGP_MaskStream_OUTLINE
                           )

                           try:
                               dst_value = ae.sdk.AEGP_GetNewStreamValue(
                                   dst_streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False
                               )

                               dst_outlineH = dst_value

                               try:
                                   # オープン/クローズ状態を設定
                                   ae.sdk.AEGP_SetMaskOutlineOpen(dst_outlineH, is_open)

                                   # 既存の頂点を削除
                                   dst_num = ae.sdk.AEGP_GetMaskOutlineNumSegments(dst_outlineH)
                                   for i in range(dst_num - 1, -1, -1):
                                       ae.sdk.AEGP_DeleteVertex(dst_outlineH, i)

                                   # 新しい頂点を追加
                                   for i, vertex in enumerate(vertices):
                                       ae.sdk.AEGP_CreateVertex(dst_outlineH, i)
                                       ae.sdk.AEGP_SetMaskOutlineVertexInfo(dst_outlineH, i, vertex)

                                   # 変更を書き戻す
                                   ae.sdk.AEGP_SetStreamValue(dst_streamH, dst_value)

                                   print(f"{num_segments}個の頂点をコピーしました")

                               finally:
                                   ae.sdk.AEGP_DisposeStreamValue(dst_value)

                           finally:
                               ae.sdk.AEGP_DisposeStream(dst_streamH)

                       finally:
                           ae.sdk.AEGP_EndUndoGroup()

                   finally:
                       ae.sdk.AEGP_DisposeMask(dst_maskH)

               finally:
                   ae.sdk.AEGP_DisposeStreamValue(src_value)

           finally:
               ae.sdk.AEGP_DisposeStream(src_streamH)

       finally:
           ae.sdk.AEGP_DisposeMask(src_maskH)

   # 使用例
   plugin_id = ae.sdk.AEGP_GetPluginID()
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetActiveComp()
   layer1H = ae.sdk.AEGP_GetCompLayerByIndex(itemH, 0)
   layer2H = ae.sdk.AEGP_GetCompLayerByIndex(itemH, 1)

   copy_mask_outline(layer1H, 0, layer2H, 0)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **ハンドルの管理**

   - マスクアウトラインハンドルは、ストリーム値として扱われます
   - 使用後は必ず ``AEGP_DisposeStreamValue()`` で破棄してください

2. **変更の反映**

   - アウトラインを変更した後は、``AEGP_SetStreamValue()`` で変更をストリームに書き戻す必要があります
   - 書き戻しを忘れると、変更が反映されません

3. **Undo操作**

   - マスクアウトラインの変更はUndo可能です
   - 複数の変更を行う場合は、``AEGP_StartUndoGroup()`` と ``AEGP_EndUndoGroup()`` でグループ化してください

4. **頂点の最小数**

   - マスクには最低3つの頂点が必要です
   - それ以下にすると、マスクが無効になる可能性があります

5. **座標系**

   - 頂点座標はレイヤー座標系で表現されます
   - 接線ハンドル (``tan_in``, ``tan_out``) は、頂点座標からの相対座標です

ベストプラクティス
~~~~~~~~~~~~~~~~~~

リソース管理
^^^^^^^^^^^^

.. code-block:: python

   maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, mask_index)
   try:
       streamH = ae.sdk.AEGP_GetNewMaskStream(maskH, ae.sdk.AEGP_MaskStream_OUTLINE)
       try:
           stream_value = ae.sdk.AEGP_GetNewStreamValue(...)
           try:
               # 処理
           finally:
               ae.sdk.AEGP_DisposeStreamValue(stream_value)
       finally:
           ae.sdk.AEGP_DisposeStream(streamH)
   finally:
       ae.sdk.AEGP_DisposeMask(maskH)

変更の書き戻し
^^^^^^^^^^^^^^

.. code-block:: python

   # 変更を行った後、必ず書き戻す
   ae.sdk.AEGP_SetMaskOutlineVertexInfo(mask_outlineH, 0, vertex)
   ae.sdk.AEGP_SetStreamValue(streamH, stream_value)

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("マスク編集")
   try:
       # 複数の変更
   finally:
       ae.sdk.AEGP_EndUndoGroup()

インデックスの範囲チェック
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   num_segments = ae.sdk.AEGP_GetMaskOutlineNumSegments(mask_outlineH)
   if index < 0 or index >= num_segments:
       raise ValueError(f"Invalid vertex index: {index}")

関連項目
--------

- :doc:`AEGP_MaskSuite6` - マスク基本操作
- :doc:`AEGP_StreamSuite6` - ストリーム値の取得・設定
- :doc:`AEGP_KeyframeSuite5` - キーフレーム操作
- :doc:`index` - 低レベルAPI概要
