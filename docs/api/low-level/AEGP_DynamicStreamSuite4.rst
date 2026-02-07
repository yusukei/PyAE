AEGP_DynamicStreamSuite4
========================

.. currentmodule:: ae.sdk

AEGP_DynamicStreamSuite4は、After Effectsのプロパティツリーを動的に探索し、レイヤー・マスク・エフェクトの階層構造を再帰的に取得するためのSDK APIです。

概要
----

**実装状況**: 6/6関数実装 ✅

AEGP_DynamicStreamSuite4は以下の機能を提供します:

- プロパティツリーのルート取得（レイヤー/マスク）
- プロパティツリーの階層構造の探索（親/子関係）
- ストリームのグループ化タイプの判定（リーフ/グループ/インデックスグループ）
- 子ストリームの列挙とアクセス

基本概念
--------

プロパティツリーとは
~~~~~~~~~~~~~~~~~~~~

After Effectsのプロパティは、ツリー構造（階層構造）で管理されています:

.. code-block:: none

   レイヤー (AEGP_StreamGroupingType_NAMED_GROUP)
   ├── Transform (AEGP_StreamGroupingType_NAMED_GROUP)
   │   ├── Position (AEGP_StreamGroupingType_LEAF)
   │   ├── Scale (AEGP_StreamGroupingType_LEAF)
   │   └── Rotation (AEGP_StreamGroupingType_LEAF)
   ├── Effects (AEGP_StreamGroupingType_INDEXED_GROUP)
   │   ├── [0] Effect1 (AEGP_StreamGroupingType_NAMED_GROUP)
   │   │   ├── Param1 (AEGP_StreamGroupingType_LEAF)
   │   │   └── Param2 (AEGP_StreamGroupingType_LEAF)
   │   └── [1] Effect2 (AEGP_StreamGroupingType_NAMED_GROUP)
   └── Masks (AEGP_StreamGroupingType_INDEXED_GROUP)

**DynamicStreamSuite4の役割**:

- このツリー構造を探索し、すべてのプロパティにアクセスする
- シリアライゼーション（Export/Import）に必要不可欠
- 静的なAPIでは取得できない動的なプロパティ（エフェクト、マスクなど）にアクセス

ストリームグループタイプ (AEGP_StreamGroupingType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

プロパティの種類を表す列挙型:

.. list-table::
   :header-rows: 1

   * - タイプ
     - 値
     - 説明
   * - ``AEGP_StreamGroupingType_NONE``
     - 0
     - グループ化なし（使用されない）
   * - ``AEGP_StreamGroupingType_LEAF``
     - 1
     - **リーフノード**: 値を持つプロパティ（Position, Scaleなど）
   * - ``AEGP_StreamGroupingType_NAMED_GROUP``
     - 2
     - **名前付きグループ**: 名前でアクセスする子を持つ（Transform, Effectsなど）
   * - ``AEGP_StreamGroupingType_INDEXED_GROUP``
     - 3
     - **インデックスグループ**: インデックスでアクセスする子を持つ（Effects[0], Masks[1]など）

**判定フロー**:

.. code-block:: python

   group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

   if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
       # リーフノード: 値を取得
       value = ae.sdk.AEGP_GetNewStreamValue(...)
   elif group_type in [ae.sdk.AEGP_StreamGroupingType_NAMED_GROUP,
                       ae.sdk.AEGP_StreamGroupingType_INDEXED_GROUP]:
       # グループ: 子ストリームを再帰的に探索
       num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
       for i in range(num_children):
           childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
           # 再帰的に処理

プロパティツリー探索の基本パターン
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**再帰的な探索**:

.. code-block:: python

   def traverse_properties(streamH, depth=0):
       """プロパティツリーを再帰的に探索"""
       name = ae.sdk.AEGP_GetStreamName(streamH, False)
       indent = "  " * depth

       group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

       if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
           print(f"{indent}[LEAF] {name}")
           # 値を取得・処理
       else:
           print(f"{indent}[GROUP] {name}")
           # 子ストリームを探索
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               traverse_properties(childH, depth + 1)
               ae.sdk.AEGP_DisposeStream(childH)

       ae.sdk.AEGP_DisposeStream(streamH)

   # レイヤーのプロパティツリーを探索
   layerH = ae.sdk.AEGP_GetLayerByIndex(...)
   rootStreamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)
   traverse_properties(rootStreamH)

API リファレンス
----------------

プロパティツリーのルート取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewStreamRefForLayer(layerH: int) -> int

   レイヤーのプロパティツリーのルートストリームを取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: ルートストリームハンドル
   :rtype: int

   .. note::
      このストリームは **必ずdisposeする必要があります**: ``AEGP_DisposeStream()``

   **例**:

   .. code-block:: python

      layerH = ae.sdk.AEGP_GetLayerByIndex(compH, 0)
      rootStreamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)
      # ... 処理 ...
      ae.sdk.AEGP_DisposeStream(rootStreamH)

.. function:: AEGP_GetNewStreamRefForMask(maskH: int) -> int

   マスクのプロパティツリーのルートストリームを取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: ルートストリームハンドル
   :rtype: int

   .. note::
      このストリームは **必ずdisposeする必要があります**: ``AEGP_DisposeStream()``

   **例**:

   .. code-block:: python

      maskH = ae.sdk.AEGP_GetLayerMaskByIndex(layerH, 0)
      rootStreamH = ae.sdk.AEGP_GetNewStreamRefForMask(maskH)
      # ... 処理 ...
      ae.sdk.AEGP_DisposeStream(rootStreamH)

プロパティツリーの階層構造探索
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewParentStreamRef(streamH: int) -> int

   ストリームの親ストリームを取得します（ツリーを上に移動）。

   :param streamH: 子ストリームハンドル
   :type streamH: int
   :return: 親ストリームハンドル
   :rtype: int

   .. note::
      - ルートストリームの場合、エラーが発生します
      - このストリームは **必ずdisposeする必要があります**: ``AEGP_DisposeStream()``

   **例**:

   .. code-block:: python

      childStreamH = ae.sdk.AEGP_GetNewLayerStream(...)
      parentStreamH = ae.sdk.AEGP_GetNewParentStreamRef(childStreamH)
      # ... 処理 ...
      ae.sdk.AEGP_DisposeStream(parentStreamH)

.. function:: AEGP_GetStreamGroupingType(streamH: int) -> int

   ストリームのグループ化タイプを取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :return: グループ化タイプ（``AEGP_StreamGroupingType_*``）
   :rtype: int

   **戻り値**:

   - ``AEGP_StreamGroupingType_LEAF`` (1): リーフノード（値を持つ）
   - ``AEGP_StreamGroupingType_NAMED_GROUP`` (2): 名前付きグループ
   - ``AEGP_StreamGroupingType_INDEXED_GROUP`` (3): インデックスグループ

   **例**:

   .. code-block:: python

      group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

      if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
          print("リーフノード: 値を取得可能")
      elif group_type == ae.sdk.AEGP_StreamGroupingType_NAMED_GROUP:
          print("名前付きグループ: 子を持つ")
      elif group_type == ae.sdk.AEGP_StreamGroupingType_INDEXED_GROUP:
          print("インデックスグループ: 動的な子を持つ")

.. function:: AEGP_GetNumStreamsInGroup(streamH: int) -> int

   グループストリームの子ストリーム数を取得します。

   :param streamH: グループストリームハンドル
   :type streamH: int
   :return: 子ストリーム数
   :rtype: int

   .. note::
      リーフノード（``AEGP_StreamGroupingType_LEAF``）に対して呼び出すとエラーになります。
      必ず ``AEGP_GetStreamGroupingType()`` で確認してから使用してください。

   **例**:

   .. code-block:: python

      group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

      if group_type != ae.sdk.AEGP_StreamGroupingType_LEAF:
          num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
          print(f"子ストリーム数: {num_children}")

.. function:: AEGP_GetNewStreamRefByIndex(parentStreamH: int, index: int) -> int

   グループストリームの指定インデックスの子ストリームを取得します。

   :param parentStreamH: 親ストリームハンドル
   :type parentStreamH: int
   :param index: 子ストリームのインデックス（0から始まる）
   :type index: int
   :return: 子ストリームハンドル
   :rtype: int

   .. note::
      このストリームは **必ずdisposeする必要があります**: ``AEGP_DisposeStream()``

   **例**:

   .. code-block:: python

      num_children = ae.sdk.AEGP_GetNumStreamsInGroup(parentStreamH)
      for i in range(num_children):
          childStreamH = ae.sdk.AEGP_GetNewStreamRefByIndex(parentStreamH, i)
          # ... 処理 ...
          ae.sdk.AEGP_DisposeStream(childStreamH)

使用例
------

プロパティツリーの完全なダンプ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_property_tree(streamH, depth=0):
       """プロパティツリーを再帰的にダンプ"""
       indent = "  " * depth

       # ストリーム名を取得
       name = ae.sdk.AEGP_GetStreamName(streamH, False)

       # グループタイプを判定
       group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

       if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
           # リーフノード: 値を表示
           stream_type = ae.sdk.AEGP_GetStreamType(streamH)
           print(f"{indent}[LEAF] {name} (type={stream_type})")

       elif group_type == ae.sdk.AEGP_StreamGroupingType_NAMED_GROUP:
           print(f"{indent}[NAMED_GROUP] {name}")
           # 子ストリームを再帰的に処理
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               dump_property_tree(childH, depth + 1)
               ae.sdk.AEGP_DisposeStream(childH)

       elif group_type == ae.sdk.AEGP_StreamGroupingType_INDEXED_GROUP:
           print(f"{indent}[INDEXED_GROUP] {name}")
           # 子ストリームを再帰的に処理
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               print(f"{indent}  [{i}]")
               dump_property_tree(childH, depth + 2)
               ae.sdk.AEGP_DisposeStream(childH)

   # レイヤーのプロパティツリーをダンプ
   compH = ae.sdk.AEGP_GetItemFromActiveComp()
   layerH = ae.sdk.AEGP_GetLayerByIndex(compH, 0)
   rootStreamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)

   print(f"=== Layer Property Tree ===")
   dump_property_tree(rootStreamH)
   ae.sdk.AEGP_DisposeStream(rootStreamH)

**出力例**:

.. code-block:: none

   === Layer Property Tree ===
   [NAMED_GROUP] Layer1
     [NAMED_GROUP] Transform
       [LEAF] Anchor Point (type=6)
       [LEAF] Position (type=7)
       [LEAF] Scale (type=5)
       [LEAF] Rotation (type=1)
       [LEAF] Opacity (type=1)
     [INDEXED_GROUP] Effects
       [0]
         [NAMED_GROUP] Gaussian Blur
           [LEAF] Blurriness (type=1)
           [LEAF] Blur Dimensions (type=1)

特定のプロパティの検索
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def find_property_by_name(streamH, property_name):
       """プロパティツリーから特定の名前のプロパティを検索"""
       name = ae.sdk.AEGP_GetStreamName(streamH, False)

       if name == property_name:
           return streamH  # 見つかった

       group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

       if group_type != ae.sdk.AEGP_StreamGroupingType_LEAF:
           # 子ストリームを再帰的に検索
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               found = find_property_by_name(childH, property_name)
               if found:
                   return found
               ae.sdk.AEGP_DisposeStream(childH)

       return None

   # "Opacity"プロパティを検索
   layerH = ae.sdk.AEGP_GetLayerByIndex(compH, 0)
   rootStreamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)

   opacity_streamH = find_property_by_name(rootStreamH, "Opacity")
   if opacity_streamH:
       print("Opacityプロパティが見つかりました")
       # 値を取得・設定
       plugin_id = ae.sdk.AEGP_GetPluginID()
       stream_type, valueH = ae.sdk.AEGP_GetNewStreamValue(
           plugin_id, opacity_streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False)
       opacity = ae.sdk.AEGP_GetStreamValue_Double(valueH)
       print(f"不透明度: {opacity}%")
       ae.sdk.AEGP_DisposeStreamValue(valueH)

   ae.sdk.AEGP_DisposeStream(rootStreamH)

プロパティツリーのシリアライゼーション
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def serialize_property_tree(streamH):
       """プロパティツリーを辞書形式にシリアライズ"""
       name = ae.sdk.AEGP_GetStreamName(streamH, False)
       group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

       result = {
           "name": name,
           "group_type": group_type
       }

       if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
           # リーフノード: 値を保存
           stream_type = ae.sdk.AEGP_GetStreamType(streamH)
           result["stream_type"] = stream_type

           # 値を取得（簡易版）
           plugin_id = ae.sdk.AEGP_GetPluginID()
           _, valueH = ae.sdk.AEGP_GetNewStreamValue(
               plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False)

           if stream_type == ae.sdk.AEGP_StreamType_OneD:
               result["value"] = ae.sdk.AEGP_GetStreamValue_Double(valueH)
           elif stream_type == ae.sdk.AEGP_StreamType_TwoD:
               x, y = ae.sdk.AEGP_GetStreamValue_Point(valueH)
               result["value"] = [x, y]
           # ... 他のタイプも処理

           ae.sdk.AEGP_DisposeStreamValue(valueH)

       else:
           # グループ: 子をシリアライズ
           children = []
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               children.append(serialize_property_tree(childH))
               ae.sdk.AEGP_DisposeStream(childH)
           result["children"] = children

       return result

   # レイヤーをシリアライズ
   layerH = ae.sdk.AEGP_GetLayerByIndex(compH, 0)
   rootStreamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)
   layer_data = serialize_property_tree(rootStreamH)
   ae.sdk.AEGP_DisposeStream(rootStreamH)

   # JSON形式で保存
   import json
   with open("layer_data.json", "w") as f:
       json.dump(layer_data, f, indent=2)

エフェクトのパラメータ列挙
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def list_effect_parameters(layerH):
       """レイヤーの全エフェクトパラメータをリスト"""
       rootStreamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)

       # "Effects"グループを探索
       num_children = ae.sdk.AEGP_GetNumStreamsInGroup(rootStreamH)
       for i in range(num_children):
           childH = ae.sdk.AEGP_GetNewStreamRefByIndex(rootStreamH, i)
           name = ae.sdk.AEGP_GetStreamName(childH, False)

           if name == "Effects":
               # エフェクトグループが見つかった
               num_effects = ae.sdk.AEGP_GetNumStreamsInGroup(childH)
               print(f"エフェクト数: {num_effects}")

               for j in range(num_effects):
                   effectStreamH = ae.sdk.AEGP_GetNewStreamRefByIndex(childH, j)
                   effect_name = ae.sdk.AEGP_GetStreamName(effectStreamH, False)
                   print(f"\n[{j}] {effect_name}")

                   # エフェクトのパラメータを列挙
                   num_params = ae.sdk.AEGP_GetNumStreamsInGroup(effectStreamH)
                   for k in range(num_params):
                       paramStreamH = ae.sdk.AEGP_GetNewStreamRefByIndex(effectStreamH, k)
                       param_name = ae.sdk.AEGP_GetStreamName(paramStreamH, False)
                       print(f"  - {param_name}")
                       ae.sdk.AEGP_DisposeStream(paramStreamH)

                   ae.sdk.AEGP_DisposeStream(effectStreamH)

           ae.sdk.AEGP_DisposeStream(childH)

       ae.sdk.AEGP_DisposeStream(rootStreamH)

   # 実行
   compH = ae.sdk.AEGP_GetItemFromActiveComp()
   layerH = ae.sdk.AEGP_GetLayerByIndex(compH, 0)
   list_effect_parameters(layerH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **ストリームハンドルの必須dispose**

   ``AEGP_GetNewStreamRefForLayer()``、``AEGP_GetNewStreamRefByIndex()`` などで取得したストリームハンドルは、**必ず** ``AEGP_DisposeStream()`` で解放する必要があります。

   .. code-block:: python

      streamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)
      # ... 処理 ...
      ae.sdk.AEGP_DisposeStream(streamH)  # 必須

2. **リーフノードへの誤った操作**

   ``AEGP_StreamGroupingType_LEAF`` に対して ``AEGP_GetNumStreamsInGroup()`` を呼び出すとエラーになります。
   必ず ``AEGP_GetStreamGroupingType()`` でタイプを確認してから操作してください。

3. **再帰処理時のメモリ管理**

   再帰的に探索する場合、各レベルで取得したストリームハンドルを適切にdisposeする必要があります。

4. **親ストリームの存在確認**

   ``AEGP_GetNewParentStreamRef()`` は、ルートストリームに対して呼び出すとエラーになります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

再帰処理のパターン
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def process_stream(streamH):
       """ストリームを処理する基本パターン"""
       try:
           group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

           if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
               # リーフノード: 値を処理
               pass
           else:
               # グループ: 子を処理
               num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
               for i in range(num_children):
                   childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
                   try:
                       process_stream(childH)
                   finally:
                       ae.sdk.AEGP_DisposeStream(childH)
       finally:
           # 必要に応じてdispose（呼び出し元の責任による）
           pass

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def safe_traverse(streamH):
       """安全なプロパティツリー探索"""
       try:
           name = ae.sdk.AEGP_GetStreamName(streamH, False)
           group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)
           # ... 処理 ...
       except Exception as e:
           print(f"エラー: {e}")
       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

パフォーマンスの考慮
^^^^^^^^^^^^^^^^^^^^

大量のプロパティを処理する場合は、必要なプロパティのみを探索してください:

.. code-block:: python

   def find_and_stop(streamH, target_name):
       """特定のプロパティを見つけたら探索を停止"""
       name = ae.sdk.AEGP_GetStreamName(streamH, False)
       if name == target_name:
           return streamH  # 見つかったら即座に返す

       # 見つからない場合のみ子を探索
       group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)
       if group_type != ae.sdk.AEGP_StreamGroupingType_LEAF:
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               found = find_and_stop(childH, target_name)
               if found:
                   return found  # 見つかったら探索を停止
               ae.sdk.AEGP_DisposeStream(childH)

       return None

よくある使用パターン
~~~~~~~~~~~~~~~~~~~~

パターン1: 全プロパティの列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def list_all_properties(streamH, depth=0):
       name = ae.sdk.AEGP_GetStreamName(streamH, False)
       print("  " * depth + name)

       group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)
       if group_type != ae.sdk.AEGP_StreamGroupingType_LEAF:
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               list_all_properties(childH, depth + 1)
               ae.sdk.AEGP_DisposeStream(childH)

パターン2: 条件付き探索
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def find_properties_by_type(streamH, target_stream_type):
       """特定のストリームタイプのプロパティのみを収集"""
       results = []

       group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)
       if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
           stream_type = ae.sdk.AEGP_GetStreamType(streamH)
           if stream_type == target_stream_type:
               name = ae.sdk.AEGP_GetStreamName(streamH, False)
               results.append((name, streamH))
       else:
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               childH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               results.extend(find_properties_by_type(childH, target_stream_type))
               ae.sdk.AEGP_DisposeStream(childH)

       return results

パターン3: 階層パスの取得
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def get_stream_path(streamH):
       """ストリームの階層パスを取得 (例: "Transform/Position")"""
       path_parts = []
       current = streamH

       while current:
           name = ae.sdk.AEGP_GetStreamName(current, False)
           path_parts.insert(0, name)

           try:
               parent = ae.sdk.AEGP_GetNewParentStreamRef(current)
               current = parent
           except:
               # ルートに到達
               break

       return "/".join(path_parts)

関連項目
--------

- :doc:`AEGP_StreamSuite6` - ストリーム値の取得・設定
- :doc:`AEGP_KeyframeSuite5` - キーフレーム管理
- :doc:`AEGP_EffectSuite5` - エフェクト管理
- :doc:`AEGP_MaskSuite6` - マスク管理
- :doc:`index` - 低レベルAPI概要
