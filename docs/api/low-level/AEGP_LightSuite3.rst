AEGP_LightSuite3
================

.. currentmodule:: ae.sdk

AEGP_LightSuite3は、After Effectsのライトレイヤーの種類や設定を取得・変更するためのSDK APIです。

概要
----

**実装状況**: 4/4関数実装 ✅（AE 24.4+で2関数追加）

AEGP_LightSuite3は以下の機能を提供します:

- ライトタイプの取得と設定
- 環境ライトソースの取得と設定（AE 24.4+）

.. important::
   このSuiteは **ライトレイヤーの種類（タイプ）** を操作するためのものです。
   ライトのプロパティ（色、強度、コーン角度など）の操作には、 :doc:`AEGP_StreamSuite6` を使用してください。

基本概念
--------

ライトタイプ (AEGP_LightType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsは以下のライトタイプをサポートしています:

.. list-table::
   :header-rows: 1

   * - タイプ
     - 定数値
     - 説明
   * - ``AEGP_LightType_NONE``
     - -1
     - ライトなし（レイヤーがライトではない場合）
   * - ``AEGP_LightType_PARALLEL``
     - 0
     - 平行光源（Parallel Light）- 太陽光のように均一な方向から照射
   * - ``AEGP_LightType_SPOT``
     - 1
     - スポットライト - コーン形状で照射、角度調整可能
   * - ``AEGP_LightType_POINT``
     - 2
     - ポイントライト - 全方向に照射、電球のような光源
   * - ``AEGP_LightType_AMBIENT``
     - 3
     - 環境光（Ambient Light）- 全体を均一に照らす
   * - ``AEGP_LightType_ENVIRONMENT``
     - 4
     - 環境マップライト（AE 24.4+）- HDRIマップなどを使用

ライトレイヤーの識別
~~~~~~~~~~~~~~~~~~~~

ライトレイヤーは ``AEGP_GetLayerObjectType`` で識別できます:

.. code-block:: python

   obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)
   if obj_type == ae.ObjectType.LIGHT:  # AEGP_ObjectType_LIGHT
       # ライトレイヤー
       light_type = ae.sdk.AEGP_GetLightType(layerH)

ライトプロパティとストリーム
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ライトタイプ以外のプロパティは **ストリーム** として操作します:

.. list-table::
   :header-rows: 1

   * - プロパティ
     - ストリーム定数
     - 型
   * - 強度（Intensity）
     - ``AEGP_LayerStream_INTENSITY``
     - 1D (double)
   * - 色（Color）
     - ``AEGP_LayerStream_COLOR``
     - Color
   * - コーン角度（Cone Angle）
     - ``AEGP_LayerStream_CONE_ANGLE``
     - 1D (double)
   * - コーンフェザー（Cone Feather）
     - ``AEGP_LayerStream_CONE_FEATHER``
     - 1D (double)
   * - 減衰（Falloff）
     - ``AEGP_LayerStream_FALLOFF_START`` 等
     - 1D (double)

API リファレンス
----------------

ライトタイプの取得と設定
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLightType(light_layerH: int) -> int

   ライトレイヤーのタイプを取得します。

   :param light_layerH: ライトレイヤーハンドル
   :type light_layerH: int
   :return: ライトタイプ（``AEGP_LightType_*`` 定数）
   :rtype: int

   .. note::
      - ライトレイヤー以外を渡すと ``AEGP_LightType_NONE`` (-1) を返します
      - エラーダイアログは表示されません（内部で検証済み）

   **例**:

   .. code-block:: python

      light_type = ae.sdk.AEGP_GetLightType(layerH)
      if light_type == 0:
          print("平行光源（Parallel Light）")
      elif light_type == 1:
          print("スポットライト（Spot Light）")
      elif light_type == 2:
          print("ポイントライト（Point Light）")
      elif light_type == 3:
          print("環境光（Ambient Light）")
      elif light_type == 4:
          print("環境マップライト（Environment Light）")

.. function:: AEGP_SetLightType(light_layerH: int, light_type: int) -> None

   ライトレイヤーのタイプを設定します（**Undo可能**）。

   :param light_layerH: ライトレイヤーハンドル
   :type light_layerH: int
   :param light_type: ライトタイプ（``AEGP_LightType_*`` 定数）
   :type light_type: int

   .. warning::
      - ライトレイヤー以外を渡すとエラーが発生します
      - ライトタイプを変更すると、一部のプロパティがリセットされる場合があります

   **例**:

   .. code-block:: python

      # スポットライトに変更
      ae.sdk.AEGP_SetLightType(layerH, 1)  # AEGP_LightType_SPOT

環境ライトソースの取得と設定（AE 24.4+）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLightSource(light_layerH: int) -> int

   環境マップライトのソースレイヤーを取得します（**AE 24.4以降**）。

   :param light_layerH: ライトレイヤーハンドル
   :type light_layerH: int
   :return: ライトソースレイヤーハンドル（ソースがない場合は ``0``）
   :rtype: int

   .. note::
      - この関数は ``AEGP_LightType_ENVIRONMENT`` の環境マップライトでのみ使用されます
      - 他のライトタイプでは ``0`` を返します
      - ライトレイヤー以外を渡すと ``0`` を返します（エラーなし）

   .. versionadded:: AE 24.4
      環境マップライト機能の追加に伴い実装されました。

   **例**:

   .. code-block:: python

      sourceH = ae.sdk.AEGP_GetLightSource(layerH)
      if sourceH != 0:
          source_name = ae.sdk.AEGP_GetLayerName(sourceH)
          print(f"ライトソース: {source_name}")

.. function:: AEGP_SetLightSource(light_layerH: int, light_sourceH: int) -> None

   環境マップライトのソースレイヤーを設定します（**AE 24.4以降**）。

   :param light_layerH: ライトレイヤーハンドル
   :type light_layerH: int
   :param light_sourceH: ライトソースレイヤーハンドル（``0`` でクリア）
   :type light_sourceH: int

   .. note::
      - ``light_sourceH`` に ``0`` を渡すと、ライトソースをクリアします
      - ライトレイヤー以外を渡すとエラーが発生します

   .. versionadded:: AE 24.4
      環境マップライト機能の追加に伴い実装されました。

   **例**:

   .. code-block:: python

      # HDRIレイヤーをライトソースとして設定
      ae.sdk.AEGP_SetLightSource(light_layerH, hdri_layerH)

      # ライトソースをクリア
      ae.sdk.AEGP_SetLightSource(light_layerH, 0)

使用例
------

ライトタイプの取得と判定
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_light_info(layerH):
       """ライトレイヤーの情報を取得"""
       # レイヤーがライトかどうか確認
       obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)
       if obj_type != ae.ObjectType.LIGHT:
           print("このレイヤーはライトではありません")
           return

       # ライトタイプを取得
       light_type = ae.sdk.AEGP_GetLightType(layerH)
       layer_name = ae.sdk.AEGP_GetLayerName(layerH)

       type_names = {
           -1: "なし",
           0: "平行光源（Parallel）",
           1: "スポットライト（Spot）",
           2: "ポイントライト（Point）",
           3: "環境光（Ambient）",
           4: "環境マップライト（Environment）"
       }

       print(f"レイヤー: {layer_name}")
       print(f"ライトタイプ: {type_names.get(light_type, '不明')}")

   # 実行
   compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)
   layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 0)
   get_light_info(layerH)

ライトタイプの変更
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def change_light_type(layerH, new_type):
       """ライトタイプを安全に変更"""
       # ライトレイヤーかどうか確認
       obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)
       if obj_type != ae.ObjectType.LIGHT:
           print("エラー: ライトレイヤーではありません")
           return False

       # 現在のタイプを取得
       current_type = ae.sdk.AEGP_GetLightType(layerH)

       if current_type == new_type:
           print("既に同じタイプです")
           return False

       # Undoグループでタイプ変更
       ae.sdk.AEGP_StartUndoGroup("ライトタイプ変更")
       try:
           ae.sdk.AEGP_SetLightType(layerH, new_type)
           print(f"ライトタイプを変更しました: {current_type} → {new_type}")
           return True
       finally:
           ae.sdk.AEGP_EndUndoGroup()

   # スポットライトに変更
   compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)
   layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 0)
   change_light_type(layerH, 1)  # AEGP_LightType_SPOT

全ライトレイヤーの列挙
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def list_all_lights(compH):
       """コンポジション内のすべてのライトレイヤーを列挙"""
       num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
       lights = []

       for i in range(num_layers):
           layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)
           obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)

           if obj_type == ae.ObjectType.LIGHT:
               layer_name = ae.sdk.AEGP_GetLayerName(layerH)
               light_type = ae.sdk.AEGP_GetLightType(layerH)

               type_names = {
                   0: "Parallel",
                   1: "Spot",
                   2: "Point",
                   3: "Ambient",
                   4: "Environment"
               }

               lights.append({
                   'name': layer_name,
                   'type': type_names.get(light_type, 'Unknown'),
                   'handle': layerH
               })

       return lights

   # 使用例
   compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)
   lights = list_all_lights(compH)

   print(f"ライトレイヤー数: {len(lights)}")
   for light in lights:
       print(f"- {light['name']} ({light['type']})")

ライトプロパティの取得と設定（ストリーム使用）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_light_properties(layerH):
       """ライトプロパティを取得"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # 強度を取得
       streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, ae.LayerStream.INTENSITY)
       intensity = ae.sdk.AEGP_GetNewStreamValue(plugin_id, streamH, ae.LTimeMode.CompTime, 0.0, False)
       print(f"強度: {intensity['val']['one_d']}%")
       ae.sdk.AEGP_DisposeStream(streamH)

       # 色を取得
       streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, ae.LayerStream.COLOR)
       color = ae.sdk.AEGP_GetNewStreamValue(plugin_id, streamH, ae.LTimeMode.CompTime, 0.0, False)
       print(f"色: R={color['val']['color']['redF']}, "
             f"G={color['val']['color']['greenF']}, "
             f"B={color['val']['color']['blueF']}")
       ae.sdk.AEGP_DisposeStream(streamH)

   def set_light_intensity(layerH, intensity):
       """ライトの強度を設定"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, ae.LayerStream.INTENSITY)
       stream_value = {'val': {'one_d': intensity}}
       ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, stream_value)
       ae.sdk.AEGP_DisposeStream(streamH)

   # 使用例
   compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)
   layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 0)

   get_light_properties(layerH)
   set_light_intensity(layerH, 150.0)  # 150%に設定

環境マップライトの設定（AE 24.4+）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_environment_light(light_layerH, hdri_layerH):
       """環境マップライトを設定（AE 24.4以降）"""
       # ライトタイプを環境マップに変更
       ae.sdk.AEGP_StartUndoGroup("環境マップライト設定")
       try:
           ae.sdk.AEGP_SetLightType(light_layerH, 4)  # AEGP_LightType_ENVIRONMENT
           ae.sdk.AEGP_SetLightSource(light_layerH, hdri_layerH)
           print("環境マップライトを設定しました")
       finally:
           ae.sdk.AEGP_EndUndoGroup()

   # 使用例
   compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)
   light_layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 0)
   hdri_layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 1)

   # SDK互換性チェック
   if ae.sdk.HAS_LIGHT_SUITE_V3:
       setup_environment_light(light_layerH, hdri_layerH)
   else:
       print("環境マップライトはAE 24.4以降でサポートされます")

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **ライトレイヤーの確認必須**

   ``AEGP_SetLightType`` を呼ぶ前に、レイヤーがライトであることを確認してください:

   .. code-block:: python

      obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)
      if obj_type == ae.ObjectType.LIGHT:
          ae.sdk.AEGP_SetLightType(layerH, new_type)

2. **プロパティはストリーム経由**

   ライトタイプ以外のプロパティ（色、強度、角度等）は ``AEGP_StreamSuite6`` 経由で操作します。

3. **環境マップライトはAE 24.4以降**

   ``AEGP_GetLightSource`` と ``AEGP_SetLightSource`` は新しいSDKでのみ使用可能です。

4. **タイプ変更の影響**

   ライトタイプを変更すると、一部のプロパティがリセットされる場合があります。
   重要なプロパティは変更後に再設定してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

レイヤータイプの検証
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)
   if obj_type != ae.ObjectType.LIGHT:
       raise ValueError("このレイヤーはライトではありません")

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("ライト設定変更")
   try:
       ae.sdk.AEGP_SetLightType(layerH, new_type)
       # その他の設定変更
   finally:
       ae.sdk.AEGP_EndUndoGroup()

SDK互換性チェック
^^^^^^^^^^^^^^^^^

.. code-block:: python

   # AE 24.4以降の機能を使用する前にチェック
   if ae.sdk.HAS_LIGHT_SUITE_V3:
       sourceH = ae.sdk.AEGP_GetLightSource(layerH)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       ae.sdk.AEGP_SetLightType(layerH, new_type)
   except RuntimeError as e:
       print(f"ライトタイプの変更に失敗: {e}")

ライトタイプ定数の使用
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # マジックナンバーを避け、定数を使用（または独自の定数を定義）
   LIGHT_TYPE_PARALLEL = 0
   LIGHT_TYPE_SPOT = 1
   LIGHT_TYPE_POINT = 2
   LIGHT_TYPE_AMBIENT = 3
   LIGHT_TYPE_ENVIRONMENT = 4

   ae.sdk.AEGP_SetLightType(layerH, LIGHT_TYPE_SPOT)

パフォーマンスに関する注意
~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **ライトタイプの変更は高コスト**

   ライトタイプの変更は内部的に多くの処理を伴うため、頻繁に実行しないでください。

2. **バッチ処理では一度に実行**

   複数のライトを変更する場合は、1つのUndoグループ内で実行してください。

3. **レンダリング中は変更しない**

   レンダリング中にライト設定を変更すると、予期しない結果になる可能性があります。

関連項目
--------

- :doc:`AEGP_StreamSuite6` - ライトプロパティ（色、強度、角度等）の取得・設定
- :doc:`AEGP_LayerSuite9` - レイヤー操作、オブジェクトタイプ取得
- :doc:`AEGP_CameraSuite2` - カメラレイヤー操作
- :doc:`AEGP_CompSuite12` - コンポジション内のレイヤー列挙
- :doc:`index` - 低レベルAPI概要
