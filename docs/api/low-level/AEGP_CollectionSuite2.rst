AEGP_CollectionSuite2
====================

.. currentmodule:: ae.sdk

AEGP_CollectionSuite2は、After Effectsのコレクション（レイヤー、マスク、エフェクトなどの要素の集合）を操作するためのSDK APIです。

概要
----

**実装状況**: 5/5関数実装 ✅

AEGP_CollectionSuite2は以下の機能を提供します:

- コレクションの作成・破棄
- コレクションアイテムの追加・削除
- コレクションアイテムの取得・列挙
- 複数の要素を一括操作するための管理機能

基本概念
--------

AEGP_Collection2H（コレクションハンドル）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのコレクションを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - コレクションハンドルは、使用後に ``AEGP_DisposeCollection()`` で解放する必要があります
   - コレクションは複数のレイヤー、マスク、エフェクトなどを保持できます
   - コレクション内のアイテムは順序を持ちます（インデックスでアクセス可能）

AEGP_CollectionItemV2（コレクションアイテム）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

コレクション内の個々の要素を表す構造体。以下のフィールドを持ちます:

.. list-table::
   :header-rows: 1

   * - フィールド
     - 型
     - 説明
   * - ``type``
     - int
     - アイテムの種類（``AEGP_CollectionItemType``）
   * - ``stream_refH``
     - int
     - ストリーム参照ハンドル（オプション）
   * - ``layerH``
     - int
     - レイヤーハンドル（タイプがLAYER、MASK、EFFECTの場合）
   * - ``mask_index``
     - int
     - マスクインデックス（タイプがMASKの場合）
   * - ``effect_index``
     - int
     - エフェクトインデックス（タイプがEFFECTの場合）

AEGP_CollectionItemType（コレクションアイテム型）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

コレクション内のアイテムの種類を示します:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_CollectionItemType_NONE``
     - 0
     - アイテムなし
   * - ``AEGP_CollectionItemType_LAYER``
     - 1
     - レイヤー
   * - ``AEGP_CollectionItemType_MASK``
     - 2
     - マスク
   * - ``AEGP_CollectionItemType_EFFECT``
     - 3
     - エフェクト
   * - ``AEGP_CollectionItemType_STREAM``
     - 4
     - ストリーム（プロパティ）
   * - ``AEGP_CollectionItemType_MASK_VERTEX``
     - 5
     - マスク頂点
   * - ``AEGP_CollectionItemType_KEYFRAME``
     - 6
     - キーフレーム

コレクションの使用シーン
~~~~~~~~~~~~~~~~~~~~~~~~

コレクションは以下のような用途で使用されます:

- **複数要素の一括操作** - 選択されたレイヤーをまとめて処理
- **バッチ処理** - 特定の条件に合致する要素を抽出して操作
- **Undo可能な一括変更** - 複数の要素に対する変更を1つのUndoグループにまとめる
- **UIとの連携** - After Effectsのユーザー選択を取得して処理

API リファレンス
----------------

コレクションの作成・破棄
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NewCollection(plugin_id: int) -> int

   新しい空のコレクションを作成します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :return: コレクションハンドル
   :rtype: int

   .. note::
      作成されたコレクションは、使用後に ``AEGP_DisposeCollection()`` で解放する必要があります。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

.. function:: AEGP_DisposeCollection(collectionH: int) -> None

   コレクションを破棄し、メモリを解放します。

   :param collectionH: コレクションハンドル
   :type collectionH: int

   .. warning::
      破棄後のコレクションハンドルは無効になります。再度使用しないでください。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DisposeCollection(collectionH)

コレクション情報の取得
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetCollectionNumItems(collectionH: int) -> int

   コレクション内のアイテム数を取得します。

   :param collectionH: コレクションハンドル
   :type collectionH: int
   :return: アイテム数
   :rtype: int

   **例**:

   .. code-block:: python

      num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
      print(f"コレクション内のアイテム数: {num_items}")

.. function:: AEGP_GetCollectionItemByIndex(collectionH: int, index: int) -> dict

   コレクション内の指定インデックスのアイテムを取得します。

   :param collectionH: コレクションハンドル
   :type collectionH: int
   :param index: アイテムのインデックス（0から始まる）
   :type index: int
   :return: アイテム情報の辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``type``
        - int
        - アイテムの種類（``AEGP_CollectionItemType``）
      * - ``stream_refH``
        - int
        - ストリーム参照ハンドル（存在する場合）
      * - ``layerH``
        - int
        - レイヤーハンドル（タイプがLAYER、MASK、EFFECTの場合）
      * - ``mask_index``
        - int
        - マスクインデックス（タイプがMASKの場合）
      * - ``effect_index``
        - int
        - エフェクトインデックス（タイプがEFFECTの場合）

   .. note::
      - 返される辞書のキーは、アイテムの ``type`` によって異なります
      - LAYER型の場合: ``type``, ``stream_refH``, ``layerH``
      - MASK型の場合: ``type``, ``stream_refH``, ``layerH``, ``mask_index``
      - EFFECT型の場合: ``type``, ``stream_refH``, ``layerH``, ``effect_index``

   **例**:

   .. code-block:: python

      item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, 0)

      if item['type'] == ae.sdk.AEGP_CollectionItemType_LAYER:
          print(f"レイヤーハンドル: {item['layerH']}")
      elif item['type'] == ae.sdk.AEGP_CollectionItemType_MASK:
          print(f"マスク: レイヤー={item['layerH']}, インデックス={item['mask_index']}")
      elif item['type'] == ae.sdk.AEGP_CollectionItemType_EFFECT:
          print(f"エフェクト: レイヤー={item['layerH']}, インデックス={item['effect_index']}")

コレクションアイテムの操作
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CollectionPushBack(collectionH: int, item: dict) -> None

   コレクションの末尾にアイテムを追加します。

   :param collectionH: コレクションハンドル
   :type collectionH: int
   :param item: 追加するアイテムの辞書
   :type item: dict

   **アイテム辞書の構造**:

   レイヤーを追加する場合:

   .. code-block:: python

      item = {
          'type': ae.sdk.AEGP_CollectionItemType_LAYER,
          'layerH': layerH,
          'stream_refH': streamH  # オプション
      }

   マスクを追加する場合:

   .. code-block:: python

      item = {
          'type': ae.sdk.AEGP_CollectionItemType_MASK,
          'layerH': layerH,
          'mask_index': 0,
          'stream_refH': streamH  # オプション
      }

   エフェクトを追加する場合:

   .. code-block:: python

      item = {
          'type': ae.sdk.AEGP_CollectionItemType_EFFECT,
          'layerH': layerH,
          'effect_index': 0,
          'stream_refH': streamH  # オプション
      }

   **例**:

   .. code-block:: python

      # レイヤーを追加
      item = {
          'type': ae.sdk.AEGP_CollectionItemType_LAYER,
          'layerH': layerH
      }
      ae.sdk.AEGP_CollectionPushBack(collectionH, item)

.. function:: AEGP_CollectionErase(collectionH: int, index_first: int, index_last: int) -> None

   コレクションから指定範囲のアイテムを削除します。

   :param collectionH: コレクションハンドル
   :type collectionH: int
   :param index_first: 削除範囲の開始インデックス（含む）
   :type index_first: int
   :param index_last: 削除範囲の終了インデックス（含む）
   :type index_last: int

   .. note::
      - インデックスは0から始まります
      - ``index_first`` と ``index_last`` が同じ場合、1つのアイテムのみ削除されます
      - ``index_first`` は ``index_last`` 以下である必要があります

   **例**:

   .. code-block:: python

      # 最初のアイテムを削除
      ae.sdk.AEGP_CollectionErase(collectionH, 0, 0)

      # インデックス1から3までのアイテムを削除（3つのアイテム）
      ae.sdk.AEGP_CollectionErase(collectionH, 1, 3)

使用例
------

選択されたレイヤーの一括処理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def process_selected_layers():
       """選択されたレイヤーをコレクションで管理して一括処理"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       # アクティブコンポジションを取得
       compH = ae.sdk.AEGP_GetProjectActiveComp(projH)
       if not compH:
           print("アクティブなコンポジションがありません")
           return

       # コレクションを作成
       collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

       try:
           # 選択されたレイヤーをコレクションに追加
           num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
           for i in range(num_layers):
               layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)

               # レイヤーが選択されているか確認（簡易版）
               # 実際にはAEGP_LayerSuite9のフラグで確認
               item = {
                   'type': ae.sdk.AEGP_CollectionItemType_LAYER,
                   'layerH': layerH
               }
               ae.sdk.AEGP_CollectionPushBack(collectionH, item)

           # コレクション内のレイヤーを処理
           num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
           print(f"処理対象: {num_items} レイヤー")

           for i in range(num_items):
               item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, i)
               layerH = item['layerH']

               # レイヤー名を取得
               name = ae.sdk.AEGP_GetLayerName(layerH)
               print(f"  [{i}] {name}")

               # ここで各レイヤーに対する処理を実行
               # 例: 不透明度を50%に設定
               streamH = ae.sdk.AEGP_GetNewLayerStream(
                   plugin_id, layerH, ae.sdk.AEGP_LayerStream_OPACITY)
               try:
                   comp_time = ae.sdk.AEGP_GetCompTime(compH)
                   time = ae.sdk.AEGP_GetLayerCurrentTime(layerH, ae.sdk.AEGP_LTimeMode_CompTime)

                   stream_value = ae.sdk.AEGP_GetNewStreamValue(
                       plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime,
                       time, False)
                   stream_value['val']['one_d'] = 50.0

                   ae.sdk.AEGP_SetStreamValue(
                       plugin_id, streamH, stream_value)
                   ae.sdk.AEGP_DisposeStreamValue(stream_value)
               finally:
                   ae.sdk.AEGP_DisposeStream(streamH)

       finally:
           # コレクションを破棄
           ae.sdk.AEGP_DisposeCollection(collectionH)

エフェクトのコレクション管理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def collect_effects_by_name(effect_name):
       """特定の名前のエフェクトをコレクションに集める"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       compH = ae.sdk.AEGP_GetProjectActiveComp(projH)

       # コレクションを作成
       collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

       try:
           num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)

           for layer_idx in range(num_layers):
               layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, layer_idx)
               num_effects = ae.sdk.AEGP_GetLayerNumEffects(layerH)

               for effect_idx in range(num_effects):
                   effectH = ae.sdk.AEGP_GetLayerEffectByIndex(
                       plugin_id, layerH, effect_idx)

                   # エフェクト名を確認
                   name = ae.sdk.AEGP_GetEffectName(plugin_id, effectH)

                   if name == effect_name:
                       # 一致するエフェクトをコレクションに追加
                       item = {
                           'type': ae.sdk.AEGP_CollectionItemType_EFFECT,
                           'layerH': layerH,
                           'effect_index': effect_idx
                       }
                       ae.sdk.AEGP_CollectionPushBack(collectionH, item)

           # コレクション内のエフェクトを処理
           num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
           print(f"'{effect_name}' エフェクトを {num_items} 個発見")

           return collectionH

       except Exception as e:
           ae.sdk.AEGP_DisposeCollection(collectionH)
           raise

コレクションのフィルタリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def filter_collection(collectionH, filter_func):
       """コレクションをフィルタリングして新しいコレクションを返す"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       new_collectionH = ae.sdk.AEGP_NewCollection(plugin_id)

       try:
           num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)

           for i in range(num_items):
               item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, i)

               # フィルタ関数で判定
               if filter_func(item):
                   ae.sdk.AEGP_CollectionPushBack(new_collectionH, item)

           return new_collectionH

       except Exception as e:
           ae.sdk.AEGP_DisposeCollection(new_collectionH)
           raise

   # 使用例: レイヤーのみ抽出
   def filter_layers_only(item):
       return item['type'] == ae.sdk.AEGP_CollectionItemType_LAYER

   # 元のコレクションからレイヤーのみを抽出
   original_collectionH = create_some_collection()
   layer_collectionH = filter_collection(original_collectionH, filter_layers_only)

コレクションの反復処理
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def iterate_collection(collectionH, callback):
       """コレクション内の全アイテムに対してコールバックを実行"""
       num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)

       for i in range(num_items):
           item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, i)
           callback(i, item)

   # 使用例
   def print_item_info(index, item):
       item_type = item['type']
       type_names = {
           ae.sdk.AEGP_CollectionItemType_LAYER: "レイヤー",
           ae.sdk.AEGP_CollectionItemType_MASK: "マスク",
           ae.sdk.AEGP_CollectionItemType_EFFECT: "エフェクト",
       }
       type_name = type_names.get(item_type, f"不明({item_type})")
       print(f"[{index}] {type_name}")

       if item_type == ae.sdk.AEGP_CollectionItemType_LAYER:
           layerH = item['layerH']
           name = ae.sdk.AEGP_GetLayerName(layerH)
           print(f"     レイヤー名: {name}")

   # コレクション内の全アイテムを表示
   iterate_collection(collectionH, print_item_info)

バッチ操作とUndo
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def batch_update_opacity(collectionH, opacity_value):
       """コレクション内の全レイヤーの不透明度を一括変更"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # Undoグループを開始
       ae.sdk.AEGP_StartUndoGroup("不透明度一括変更")

       try:
           num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)

           for i in range(num_items):
               item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, i)

               # レイヤー型のみ処理
               if item['type'] != ae.sdk.AEGP_CollectionItemType_LAYER:
                   continue

               layerH = item['layerH']

               # 不透明度ストリームを取得
               streamH = ae.sdk.AEGP_GetNewLayerStream(
                   plugin_id, layerH, ae.sdk.AEGP_LayerStream_OPACITY)

               try:
                   time = ae.sdk.AEGP_GetLayerCurrentTime(
                       layerH, ae.sdk.AEGP_LTimeMode_CompTime)

                   stream_value = ae.sdk.AEGP_GetNewStreamValue(
                       plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime,
                       time, False)

                   stream_value['val']['one_d'] = opacity_value

                   ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, stream_value)
                   ae.sdk.AEGP_DisposeStreamValue(stream_value)

               finally:
                   ae.sdk.AEGP_DisposeStream(streamH)

           print(f"{num_items} レイヤーの不透明度を {opacity_value}% に変更しました")

       finally:
           # Undoグループを終了
           ae.sdk.AEGP_EndUndoGroup()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリ管理**

   作成したコレクションは必ず ``AEGP_DisposeCollection()`` で解放してください。

2. **インデックス範囲**

   ``AEGP_GetCollectionItemByIndex()`` は、有効なインデックス範囲（0〜num_items-1）でのみ使用してください。

3. **アイテム型の確認**

   ``AEGP_GetCollectionItemByIndex()`` で取得したアイテムは、``type`` フィールドを確認してから適切なフィールドにアクセスしてください。

4. **削除操作**

   ``AEGP_CollectionErase()`` でアイテムを削除すると、インデックスがずれるため、逆順で削除することを推奨します。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

コレクションの安全な使用
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   collectionH = ae.sdk.AEGP_NewCollection(plugin_id)
   try:
       # コレクションを使用
       pass
   finally:
       ae.sdk.AEGP_DisposeCollection(collectionH)

アイテム型の確認
^^^^^^^^^^^^^^^^

.. code-block:: python

   item = ae.sdk.AEGP_GetCollectionItemByIndex(collectionH, i)

   if item['type'] == ae.sdk.AEGP_CollectionItemType_LAYER:
       layerH = item['layerH']
       # レイヤー処理
   elif item['type'] == ae.sdk.AEGP_CollectionItemType_MASK:
       layerH = item['layerH']
       mask_index = item['mask_index']
       # マスク処理

逆順での削除
^^^^^^^^^^^^

.. code-block:: python

   # 正しい: 逆順で削除
   num_items = ae.sdk.AEGP_GetCollectionNumItems(collectionH)
   for i in range(num_items - 1, -1, -1):
       ae.sdk.AEGP_CollectionErase(collectionH, i, i)

   # 誤り: 順方向で削除するとインデックスがずれる
   for i in range(num_items):
       ae.sdk.AEGP_CollectionErase(collectionH, i, i)  # NG!

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("コレクション操作")
   try:
       # コレクション内のアイテムを一括処理
       pass
   finally:
       ae.sdk.AEGP_EndUndoGroup()

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       collectionH = ae.sdk.AEGP_NewCollection(plugin_id)
       # コレクション処理
   except Exception as e:
       print(f"コレクション操作に失敗しました: {e}")
       if collectionH:
           ae.sdk.AEGP_DisposeCollection(collectionH)

関連項目
--------

- :doc:`AEGP_LayerSuite9` - レイヤー管理
- :doc:`AEGP_EffectSuite5` - エフェクト管理
- :doc:`AEGP_MaskSuite6` - マスク管理
- :doc:`AEGP_StreamSuite6` - ストリーム（プロパティ）管理
- :doc:`AEGP_UtilitySuite6` - Undo管理、エラー処理
- :doc:`index` - 低レベルAPI概要
