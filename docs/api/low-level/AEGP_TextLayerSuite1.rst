AEGP_TextLayerSuite1
====================

.. currentmodule:: ae.sdk

AEGP_TextLayerSuite1は、テキストレイヤーからアウトライン（パス）情報を取得するためのSDK APIです。

概要
----

**実装状況**: 4/4関数実装 ✅

AEGP_TextLayerSuite1は以下の機能を提供します:

- テキストレイヤーからテキストアウトライン（パス）を取得
- アウトラインの数を取得
- 個別のアウトラインパスを取得
- アウトラインリソースの解放

.. note::
   このSuiteは**テキストのアウトライン（ベジェパス）**を取得するために使用されます。
   テキストの内容（文字列）を取得・設定する場合は、``AEGP_TextDocumentSuite1`` を使用してください。

基本概念
--------

テキストアウトラインハンドル (AEGP_TextOutlinesH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

テキストレイヤーから取得したアウトライン情報を保持するハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - ``AEGP_GetNewTextOutlines()`` で取得したハンドルは、必ず ``AEGP_DisposeTextOutlines()`` で解放する必要があります
   - ハンドルを解放せずに残すとメモリリークが発生します
   - 特定の時刻のテキスト状態のスナップショットを保持します

パスアウトラインポインタ (PF_PathOutlinePtr)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

個別のアウトラインパス（ベジェ曲線）を表すポインタ。

.. warning::
   - ``AEGP_GetIndexedTextOutline()`` で取得したパスポインタは **解放してはいけません**
   - パスはアウトラインハンドル (``AEGP_TextOutlinesH``) によって管理されています
   - アウトラインハンドルを解放すると、すべてのパスポインタが無効になります

時刻とアウトライン
~~~~~~~~~~~~~~~~~~

テキストレイヤーのアウトラインは、時刻によって変化する可能性があります（アニメーション、エクスプレッション等）。

.. note::
   現在の実装では、``AEGP_GetNewTextOutlines()`` は時刻0（秒）でアウトラインを取得します。
   異なる時刻のアウトラインが必要な場合は、実装の拡張が必要です。

API リファレンス
----------------

テキストアウトライン取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewTextOutlines(layerH: int) -> int

   テキストレイヤーから新しいテキストアウトライン（パス）を取得します。

   :param layerH: テキストレイヤーのハンドル（テキストレイヤーである必要があります）
   :type layerH: int
   :return: テキストアウトラインハンドル（``AEGP_TextOutlinesH``）
   :rtype: int
   :raises RuntimeError: レイヤーがテキストレイヤーでない場合、またはAPI呼び出しが失敗した場合

   .. important::
      戻り値のハンドルは必ず ``AEGP_DisposeTextOutlines()`` で解放してください。

   .. note::
      現在の実装は時刻0（秒）でアウトラインを取得します。

   **例**:

   .. code-block:: python

      # テキストレイヤーからアウトラインを取得
      outlinesH = ae.sdk.AEGP_GetNewTextOutlines(text_layerH)

      try:
          # アウトラインを使用した処理
          num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)
          print(f"パス数: {num_paths}")
      finally:
          # 必ず解放
          ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

.. function:: AEGP_DisposeTextOutlines(outlinesH: int) -> None

   テキストアウトラインハンドルを解放します。

   :param outlinesH: 解放するテキストアウトラインハンドル
   :type outlinesH: int
   :raises RuntimeError: ハンドルが無効な場合、またはAPI呼び出しが失敗した場合

   .. warning::
      - 同じハンドルを2回解放しないでください（クラッシュの原因になります）
      - ハンドルを解放すると、``AEGP_GetIndexedTextOutline()`` で取得したすべてのパスポインタが無効になります

   **例**:

   .. code-block:: python

      outlinesH = ae.sdk.AEGP_GetNewTextOutlines(text_layerH)

      # ... 処理 ...

      # 必ず解放
      ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

アウトライン情報取得
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNumTextOutlines(outlinesH: int) -> int

   テキストアウトラインに含まれるパスの数を取得します。

   :param outlinesH: テキストアウトラインハンドル
   :type outlinesH: int
   :return: パスの数
   :rtype: int
   :raises RuntimeError: ハンドルが無効な場合、またはAPI呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      outlinesH = ae.sdk.AEGP_GetNewTextOutlines(text_layerH)

      try:
          num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)
          print(f"テキストには {num_paths} 個のパスがあります")
      finally:
          ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

.. function:: AEGP_GetIndexedTextOutline(outlinesH: int, path_index: int) -> int

   指定したインデックスのアウトラインパスを取得します。

   :param outlinesH: テキストアウトラインハンドル
   :type outlinesH: int
   :param path_index: パスのインデックス（0から始まる）
   :type path_index: int
   :return: パスアウトラインポインタ（``PF_PathOutlinePtr``）
   :rtype: int
   :raises ValueError: path_indexが範囲外の場合
   :raises RuntimeError: ハンドルが無効な場合、またはAPI呼び出しが失敗した場合

   .. warning::
      - 戻り値のパスポインタは **解放してはいけません**
      - パスはアウトラインハンドルによって管理されています
      - アウトラインハンドルを解放すると、このパスポインタは無効になります

   .. note::
      ``path_index`` の有効範囲は ``0`` から ``AEGP_GetNumTextOutlines() - 1`` までです。

   **例**:

   .. code-block:: python

      outlinesH = ae.sdk.AEGP_GetNewTextOutlines(text_layerH)

      try:
          num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)

          # すべてのパスを取得
          for i in range(num_paths):
              pathP = ae.sdk.AEGP_GetIndexedTextOutline(outlinesH, i)
              print(f"パス {i}: {pathP}")
              # pathPは解放しない
      finally:
          ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

使用例
------

テキストレイヤーのアウトライン数を取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def count_text_outlines(text_layerH):
       """テキストレイヤーのアウトライン（パス）数を取得"""
       outlinesH = ae.sdk.AEGP_GetNewTextOutlines(text_layerH)

       try:
           num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)
           return num_paths
       finally:
           ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

   # 使用例
   num_paths = count_text_outlines(text_layerH)
   print(f"テキストには {num_paths} 個のアウトラインがあります")

すべてのアウトラインを列挙
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def enumerate_text_outlines(text_layerH):
       """テキストレイヤーのすべてのアウトラインパスを列挙"""
       outlinesH = ae.sdk.AEGP_GetNewTextOutlines(text_layerH)

       try:
           num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)
           paths = []

           for i in range(num_paths):
               pathP = ae.sdk.AEGP_GetIndexedTextOutline(outlinesH, i)
               paths.append(pathP)

           return paths
       finally:
           ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

   # 使用例
   paths = enumerate_text_outlines(text_layerH)
   print(f"取得したパス数: {len(paths)}")
   for i, pathP in enumerate(paths):
       print(f"  パス {i}: {pathP}")

テキストレイヤーのアウトライン情報をダンプ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_text_outline_info(text_layer):
       """テキストレイヤーのアウトライン情報を表示"""
       # レイヤーハンドルを取得
       layerH = text_layer._layerH

       # アウトラインを取得
       outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layerH)

       try:
           num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)

           print("=" * 50)
           print(f"テキストレイヤー: {text_layer.name}")
           print(f"アウトライン数: {num_paths}")
           print("-" * 50)

           for i in range(num_paths):
               pathP = ae.sdk.AEGP_GetIndexedTextOutline(outlinesH, i)
               print(f"  [{i}] パスポインタ: 0x{pathP:08X}")

           print("=" * 50)

       finally:
           ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

   # 使用例
   app = ae.app()
   comp = app.active_comp

   for layer in comp.layers:
       # テキストレイヤーのみ処理
       if hasattr(layer, 'source_text'):  # テキストレイヤーの判定
           dump_text_outline_info(layer)

コンテキストマネージャーを使った安全なアウトライン管理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from contextlib import contextmanager

   @contextmanager
   def text_outlines(text_layerH):
       """テキストアウトラインのコンテキストマネージャー"""
       outlinesH = ae.sdk.AEGP_GetNewTextOutlines(text_layerH)
       try:
           yield outlinesH
       finally:
           ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

   # 使用例
   with text_outlines(text_layerH) as outlinesH:
       num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)
       print(f"パス数: {num_paths}")

       for i in range(num_paths):
           pathP = ae.sdk.AEGP_GetIndexedTextOutline(outlinesH, i)
           # パスの処理
           print(f"パス {i}: 0x{pathP:08X}")

   # コンテキスト終了時に自動的にAEGP_DisposeTextOutlinesが呼ばれる

複数レイヤーのアウトライン数を比較
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def compare_text_outline_counts():
       """すべてのテキストレイヤーのアウトライン数を比較"""
       app = ae.app()
       comp = app.active_comp

       results = []

       for layer in comp.layers:
           # テキストレイヤーかチェック
           if not hasattr(layer, 'source_text'):
               continue

           layerH = layer._layerH
           outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layerH)

           try:
               num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)
               results.append({
                   'name': layer.name,
                   'index': layer.index,
                   'num_paths': num_paths
               })
           finally:
               ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

       # 結果を表示（パス数でソート）
       results.sort(key=lambda x: x['num_paths'], reverse=True)

       print("テキストレイヤーのアウトライン数:")
       print("=" * 60)
       for r in results:
           print(f"  {r['name']:30s} : {r['num_paths']:4d} パス")
       print("=" * 60)

   # 実行
   compare_text_outline_counts()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **必ずアウトラインハンドルを解放する**

   ``AEGP_GetNewTextOutlines()`` で取得したハンドルは、必ず ``AEGP_DisposeTextOutlines()`` で解放してください。
   解放しないとメモリリークが発生します。

   .. code-block:: python

      # 良い例
      outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layerH)
      try:
          # 処理
      finally:
          ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

2. **パスポインタを解放しない**

   ``AEGP_GetIndexedTextOutline()`` で取得したパスポインタは解放してはいけません。
   パスはアウトラインハンドルによって管理されています。

3. **アウトラインハンドル解放後はパスポインタが無効**

   アウトラインハンドルを解放すると、そこから取得したすべてのパスポインタが無効になります。
   パスポインタを長期間保持する必要がある場合は、アウトラインハンドルも保持してください。

4. **テキストレイヤーでない場合はエラー**

   ``AEGP_GetNewTextOutlines()`` はテキストレイヤーに対してのみ使用できます。
   他のレイヤータイプで実行するとエラーになります。

5. **時刻の制限**

   現在の実装は時刻0（秒）でアウトラインを取得します。
   アニメーションされたテキストの特定時刻のアウトラインを取得する場合は、実装の拡張が必要です。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

例外安全なリソース管理
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # コンテキストマネージャーを使用
   from contextlib import contextmanager

   @contextmanager
   def text_outlines(layerH):
       outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layerH)
       try:
           yield outlinesH
       finally:
           ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

   # または try-finally を使用
   outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layerH)
   try:
       # 処理
   finally:
       ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

テキストレイヤーの検証
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def is_text_layer(layer):
       """レイヤーがテキストレイヤーかチェック"""
       return hasattr(layer, 'source_text')

   # 使用例
   if is_text_layer(layer):
       outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layer._layerH)
       # ...

インデックスの範囲チェック
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layerH)
   try:
       num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)

       # 範囲チェック
       for i in range(num_paths):  # 自動的に範囲内に制限される
           pathP = ae.sdk.AEGP_GetIndexedTextOutline(outlinesH, i)
           # ...
   finally:
       ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layerH)
   except RuntimeError as e:
       print(f"テキストアウトライン取得失敗: {e}")
       return

   try:
       num_paths = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)
       # ...
   except Exception as e:
       print(f"処理中にエラー: {e}")
   finally:
       ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

制限事項
--------

時刻の指定
~~~~~~~~~~

現在の実装では、``AEGP_GetNewTextOutlines()`` は常に時刻0（秒）でアウトラインを取得します。

.. note::
   将来的には、時刻を引数として受け取るバージョンを提供する予定です。

   .. code-block:: python

      # 将来的な実装例（未実装）
      outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layerH, time_seconds=2.5)

パス構造の詳細情報
~~~~~~~~~~~~~~~~~~

``PF_PathOutlinePtr`` は不透明なポインタとして扱われ、現在の実装ではパスの詳細情報（頂点、ベジェハンドル等）にアクセスする手段は提供されていません。

.. note::
   パスの詳細情報が必要な場合は、``PF_PathQuerySuite`` の追加実装が必要です。

関連Suite
---------

AEGP_TextDocumentSuite1
~~~~~~~~~~~~~~~~~~~~~~~

テキストの内容（文字列）を取得・設定するためのSuiteです。

**関数**:

- ``AEGP_GetNewText(text_documentH)`` - テキスト文字列を取得
- ``AEGP_SetText(text_documentH, text)`` - テキスト文字列を設定

.. seealso::
   ``AEGP_TextDocumentSuite1`` の詳細は、別ドキュメントを参照してください。

   **使用例**:

   .. code-block:: python

      # テキストプロパティからテキストドキュメントハンドルを取得
      # （具体的な取得方法はプロパティAPIに依存）
      text_documentH = ...  # テキストドキュメントハンドル

      # テキスト内容を取得
      text = ae.sdk.AEGP_GetNewText(text_documentH)
      print(f"テキスト: {text}")

      # テキスト内容を設定
      ae.sdk.AEGP_SetText(text_documentH, "新しいテキスト")

関連項目
--------

- :doc:`AEGP_LayerSuite9` - レイヤー管理、レイヤータイプの判定
- :doc:`AEGP_StreamSuite6` - プロパティストリーム管理（テキストプロパティへのアクセス）
- :doc:`index` - 低レベルAPI概要
