AEGP_TextDocumentSuite1
=======================

.. currentmodule:: ae.sdk

AEGP_TextDocumentSuite1は、After Effectsのテキストドキュメント（テキストコンテンツ）の取得と設定を行うためのSDK APIです。

概要
----

**実装状況**: 2/2関数実装 ✅

AEGP_TextDocumentSuite1は以下の機能を提供します:

- テキストドキュメントハンドルからテキスト内容の取得
- テキストドキュメントハンドルへのテキスト内容の設定

基本概念
--------

テキストドキュメントハンドル (AEGP_TextDocumentH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのテキストドキュメントを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - テキストドキュメントハンドルは、``AEGP_StreamSuite``の``AEGP_GetNewStreamValue()``から取得します
   - テキストレイヤーの``AEGP_SourceText``ストリームから取得可能です
   - テキストドキュメントハンドルは、使用後に適切に解放する必要があります

文字エンコーディング
~~~~~~~~~~~~~~~~~~~~

PyAEでは文字エンコーディングを自動的に処理します:

- **入力（Python → AE）**: UTF-8文字列をUTF-16に自動変換
- **出力（AE → Python）**: UTF-16文字列をUTF-8に自動変換

.. tip::
   日本語、中国語、絵文字などのマルチバイト文字も正しく処理されます。

API リファレンス
----------------

テキスト取得
~~~~~~~~~~~~

.. function:: AEGP_GetNewText(text_documentH: int) -> str

   テキストドキュメントハンドルからテキスト内容を取得します。

   :param text_documentH: テキストドキュメントハンドル
   :type text_documentH: int
   :return: テキスト内容（UTF-8文字列）
   :rtype: str
   :raises RuntimeError: テキストドキュメントハンドルが無効な場合、またはメモリ操作に失敗した場合

   .. note::
      内部的にUTF-16からUTF-8に自動変換されます。日本語テキストも正しく取得できます。

   **例**:

   .. code-block:: python

      # テキストレイヤーからテキストドキュメントを取得
      stream_ref = ae.sdk.AEGP_GetNewLayerStream(layerH, ae.sdk.AEGP_LayerStream_SOURCE_TEXT)
      streamH = ae.sdk.AEGP_GetStream(stream_ref)

      # 現在時刻のストリーム値を取得
      stream_val = ae.sdk.AEGP_GetNewStreamValue(streamH, ae.sdk.AEGP_LTimeMode_LayerTime, time)

      # テキストドキュメントハンドルを取得
      text_documentH = stream_val['val']['td']

      # テキスト内容を取得
      text = ae.sdk.AEGP_GetNewText(text_documentH)
      print(f"テキスト内容: {text}")

テキスト設定
~~~~~~~~~~~~

.. function:: AEGP_SetText(text_documentH: int, text: str) -> None

   テキストドキュメントハンドルにテキスト内容を設定します。

   :param text_documentH: テキストドキュメントハンドル
   :type text_documentH: int
   :param text: 設定するテキスト内容（UTF-8文字列）
   :type text: str
   :raises RuntimeError: テキストドキュメントハンドルが無効な場合、または設定に失敗した場合

   .. note::
      内部的にUTF-8からUTF-16に自動変換されます。日本語テキストも正しく設定できます。

   **例**:

   .. code-block:: python

      # テキストドキュメントハンドルを取得（上記と同様）
      text_documentH = stream_val['val']['td']

      # テキスト内容を変更
      ae.sdk.AEGP_SetText(text_documentH, "新しいテキスト")

      # ストリーム値を更新
      ae.sdk.AEGP_SetStreamValue(streamH, stream_val)

使用例
------

テキストレイヤーのテキスト取得と変更
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_and_set_text_layer_content(layerH):
       """テキストレイヤーのテキストを取得して変更"""
       # Source Textストリームを取得
       stream_ref = ae.sdk.AEGP_GetNewLayerStream(
           layerH,
           ae.sdk.AEGP_LayerStream_SOURCE_TEXT
       )
       streamH = ae.sdk.AEGP_GetStream(stream_ref)

       # 現在時刻（0秒）のストリーム値を取得
       time = {'value': 0, 'scale': 1}
       stream_val = ae.sdk.AEGP_GetNewStreamValue(
           streamH,
           ae.sdk.AEGP_LTimeMode_LayerTime,
           time
       )

       # テキストドキュメントハンドルを取得
       text_documentH = stream_val['val']['td']

       # 現在のテキストを取得
       current_text = ae.sdk.AEGP_GetNewText(text_documentH)
       print(f"現在のテキスト: {current_text}")

       # テキストを変更
       new_text = f"{current_text} - 更新済み"
       ae.sdk.AEGP_SetText(text_documentH, new_text)

       # ストリーム値を反映
       ae.sdk.AEGP_SetStreamValue(streamH, stream_val)
       print(f"新しいテキスト: {new_text}")

       # リソース解放
       ae.sdk.AEGP_DisposeStreamValue(stream_val)
       ae.sdk.AEGP_DisposeStream(streamH)

   # 実行例
   layerH = ae.sdk.AEGP_GetActiveLayer()
   get_and_set_text_layer_content(layerH)

複数テキストレイヤーの一括変更
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def replace_text_in_all_layers(compH, old_text, new_text):
       """コンポジション内の全テキストレイヤーでテキストを置換"""
       num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
       replaced_count = 0

       for i in range(num_layers):
           layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)

           # テキストレイヤーか確認
           if not ae.sdk.AEGP_IsTextLayer(layerH):
               continue

           try:
               # Source Textストリームを取得
               stream_ref = ae.sdk.AEGP_GetNewLayerStream(
                   layerH,
                   ae.sdk.AEGP_LayerStream_SOURCE_TEXT
               )
               streamH = ae.sdk.AEGP_GetStream(stream_ref)

               # 現在のストリーム値を取得
               time = {'value': 0, 'scale': 1}
               stream_val = ae.sdk.AEGP_GetNewStreamValue(
                   streamH,
                   ae.sdk.AEGP_LTimeMode_LayerTime,
                   time
               )

               # テキストドキュメントハンドルを取得
               text_documentH = stream_val['val']['td']

               # テキストを取得
               current_text = ae.sdk.AEGP_GetNewText(text_documentH)

               # テキストを置換
               if old_text in current_text:
                   updated_text = current_text.replace(old_text, new_text)
                   ae.sdk.AEGP_SetText(text_documentH, updated_text)
                   ae.sdk.AEGP_SetStreamValue(streamH, stream_val)
                   replaced_count += 1
                   print(f"置換: {current_text} -> {updated_text}")

               # リソース解放
               ae.sdk.AEGP_DisposeStreamValue(stream_val)
               ae.sdk.AEGP_DisposeStream(streamH)

           except Exception as e:
               print(f"エラー（レイヤー {i}）: {e}")

       print(f"\n{replaced_count}個のレイヤーでテキストを置換しました")

   # 実行例
   compH = ae.sdk.AEGP_GetActiveComp()
   replace_text_in_all_layers(compH, "旧テキスト", "新テキスト")

テキスト内容のエクスポート
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import json

   def export_text_layer_content(compH, output_path):
       """コンポジション内の全テキストレイヤーの内容をエクスポート"""
       num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
       text_data = []

       for i in range(num_layers):
           layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)

           # テキストレイヤーか確認
           if not ae.sdk.AEGP_IsTextLayer(layerH):
               continue

           try:
               # レイヤー名を取得
               layer_name = ae.sdk.AEGP_GetLayerName(layerH)

               # Source Textストリームを取得
               stream_ref = ae.sdk.AEGP_GetNewLayerStream(
                   layerH,
                   ae.sdk.AEGP_LayerStream_SOURCE_TEXT
               )
               streamH = ae.sdk.AEGP_GetStream(stream_ref)

               # テキスト内容を取得
               time = {'value': 0, 'scale': 1}
               stream_val = ae.sdk.AEGP_GetNewStreamValue(
                   streamH,
                   ae.sdk.AEGP_LTimeMode_LayerTime,
                   time
               )
               text_documentH = stream_val['val']['td']
               text = ae.sdk.AEGP_GetNewText(text_documentH)

               # データを追加
               text_data.append({
                   'layer_name': layer_name,
                   'text': text
               })

               # リソース解放
               ae.sdk.AEGP_DisposeStreamValue(stream_val)
               ae.sdk.AEGP_DisposeStream(streamH)

           except Exception as e:
               print(f"エラー（レイヤー {i}）: {e}")

       # JSONファイルに保存
       with open(output_path, 'w', encoding='utf-8') as f:
           json.dump(text_data, f, ensure_ascii=False, indent=2)

       print(f"{len(text_data)}個のテキストレイヤーをエクスポートしました: {output_path}")

   # 実行例
   compH = ae.sdk.AEGP_GetActiveComp()
   export_text_layer_content(compH, r"C:\Export\text_content.json")

テキスト内容のインポート
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import json

   def import_text_layer_content(compH, input_path):
       """JSONファイルからテキストレイヤーの内容をインポート"""
       # JSONファイルを読み込み
       with open(input_path, 'r', encoding='utf-8') as f:
           text_data = json.load(f)

       # レイヤー名からレイヤーハンドルへのマップを作成
       num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
       layer_map = {}

       for i in range(num_layers):
           layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)
           layer_name = ae.sdk.AEGP_GetLayerName(layerH)
           layer_map[layer_name] = layerH

       # Undoグループ開始
       ae.sdk.AEGP_StartUndoGroup("テキスト内容インポート")

       try:
           imported_count = 0

           for item in text_data:
               layer_name = item['layer_name']
               text = item['text']

               # レイヤーを検索
               if layer_name not in layer_map:
                   print(f"警告: レイヤー '{layer_name}' が見つかりません")
                   continue

               layerH = layer_map[layer_name]

               # テキストレイヤーか確認
               if not ae.sdk.AEGP_IsTextLayer(layerH):
                   print(f"警告: レイヤー '{layer_name}' はテキストレイヤーではありません")
                   continue

               try:
                   # Source Textストリームを取得
                   stream_ref = ae.sdk.AEGP_GetNewLayerStream(
                       layerH,
                       ae.sdk.AEGP_LayerStream_SOURCE_TEXT
                   )
                   streamH = ae.sdk.AEGP_GetStream(stream_ref)

                   # ストリーム値を取得
                   time = {'value': 0, 'scale': 1}
                   stream_val = ae.sdk.AEGP_GetNewStreamValue(
                       streamH,
                       ae.sdk.AEGP_LTimeMode_LayerTime,
                       time
                   )

                   # テキストを設定
                   text_documentH = stream_val['val']['td']
                   ae.sdk.AEGP_SetText(text_documentH, text)
                   ae.sdk.AEGP_SetStreamValue(streamH, stream_val)

                   imported_count += 1
                   print(f"インポート: {layer_name}")

                   # リソース解放
                   ae.sdk.AEGP_DisposeStreamValue(stream_val)
                   ae.sdk.AEGP_DisposeStream(streamH)

               except Exception as e:
                   print(f"エラー（レイヤー '{layer_name}'）: {e}")

           print(f"\n{imported_count}個のレイヤーにテキストをインポートしました")

       finally:
           # Undoグループ終了
           ae.sdk.AEGP_EndUndoGroup()

   # 実行例
   compH = ae.sdk.AEGP_GetActiveComp()
   import_text_layer_content(compH, r"C:\Export\text_content.json")

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **テキストドキュメントハンドルの取得**

   テキストドキュメントハンドルは、``AEGP_StreamSuite``の``AEGP_GetNewStreamValue()``から取得します。
   直接作成することはできません。

2. **リソース管理**

   ``AEGP_GetNewStreamValue()``で取得したストリーム値は、使用後に``AEGP_DisposeStreamValue()``で解放する必要があります。

3. **文字エンコーディング**

   PyAEは自動的にUTF-8とUTF-16の変換を行います。日本語やマルチバイト文字も正しく処理されます。

4. **テキスト変更の反映**

   ``AEGP_SetText()``でテキストを変更した後、``AEGP_SetStreamValue()``を呼び出してストリーム値を反映する必要があります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

リソース管理の徹底
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       stream_val = ae.sdk.AEGP_GetNewStreamValue(streamH, mode, time)
       text_documentH = stream_val['val']['td']
       # テキスト操作
   finally:
       ae.sdk.AEGP_DisposeStreamValue(stream_val)

テキストレイヤーの確認
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   if ae.sdk.AEGP_IsTextLayer(layerH):
       # テキスト操作

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       text = ae.sdk.AEGP_GetNewText(text_documentH)
   except Exception as e:
       print(f"テキスト取得エラー: {e}")

Undoグループの使用
^^^^^^^^^^^^^^^^^^

複数のテキスト変更を行う場合は、Undoグループでまとめる:

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("テキスト一括変更")
   try:
       # 複数のテキスト変更
   finally:
       ae.sdk.AEGP_EndUndoGroup()

関連項目
--------

- :doc:`AEGP_StreamSuite6` - ストリーム値の取得と設定
- :doc:`AEGP_TextLayerSuite1` - テキストレイヤー操作、アウトライン取得
- :doc:`AEGP_LayerSuite9` - レイヤー操作
- :doc:`index` - 低レベルAPI概要
