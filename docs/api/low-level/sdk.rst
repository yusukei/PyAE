SDK Module
==========

.. currentmodule:: ae.sdk

ae.sdkモジュールは、AEGP SDKへの直接アクセスを提供します。

モジュールリファレンス
----------------------

``ae.sdk`` モジュールは、AEGP SDK関数への直接アクセスを提供します。

.. note::
   各SDK関数の詳細なリファレンスは、個別のSuiteドキュメントを参照してください。

使用例
------

プロジェクトとアイテム
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae.sdk as sdk

   # プロジェクトハンドルを取得
   project_h = sdk.get_project_handle()

   # アイテム数を取得
   num_items = sdk.get_num_items(project_h)

   # アイテムを列挙
   for i in range(1, num_items + 1):
       item_h = sdk.get_item_by_index(project_h, i)
       name = sdk.get_item_name(item_h)
       item_type = sdk.get_item_type(item_h)
       print(f"{i}: {name} ({item_type})")

コンポジション
~~~~~~~~~~~~~~

.. code-block:: python

   # コンポジションハンドルを取得
   comp_h = sdk.get_comp_from_item(item_h)

   # コンポジション情報を取得
   width = sdk.get_comp_width(comp_h)
   height = sdk.get_comp_height(comp_h)
   duration = sdk.get_comp_duration(comp_h)
   fps = sdk.get_comp_framerate(comp_h)

   print(f"サイズ: {width}x{height}")
   print(f"デュレーション: {duration}秒")
   print(f"FPS: {fps}")

レイヤー
~~~~~~~~

.. code-block:: python

   # レイヤー数を取得
   num_layers = sdk.get_num_layers(comp_h)

   # レイヤーを列挙
   for i in range(1, num_layers + 1):
       layer_h = sdk.get_layer_by_index(comp_h, i)
       name = sdk.get_layer_name(layer_h)
       print(f"レイヤー {i}: {name}")

フッテージ
~~~~~~~~~~

.. code-block:: python

   # フッテージハンドルを取得
   footage_h = sdk.get_footage_from_item(item_h)

   # フッテージ情報を取得
   width = sdk.get_footage_width(footage_h)
   height = sdk.get_footage_height(footage_h)
   duration = sdk.get_footage_duration(footage_h)

   print(f"フッテージサイズ: {width}x{height}")

エラーハンドリング
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   try:
       # SDK関数を呼び出し
       result = sdk.some_sdk_function()
   except RuntimeError as e:
       print(f"SDK Error: {e}")

注意事項
--------

- **高レベルAPIを優先**: 可能な限り高レベルAPIを使用してください
- **ハンドルの管理**: ハンドルは適切に管理する必要があります
- **エラーチェック**: すべての戻り値を確認してください
- **ドキュメント参照**: Adobe SDK Documentationを参照してください

関連項目
--------

- :doc:`index` - 低レベルAPI概要
- :doc:`../high-level/index` - 高レベルAPI
