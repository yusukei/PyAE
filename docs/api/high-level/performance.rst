パフォーマンス最適化
====================

PyAE では、大量のプロパティ操作やレイヤー処理を効率的に行うためのパフォーマンス最適化APIを提供しています。

概要
----

**利用可能なサブモジュール**:

- ``ae.batch`` - バッチ操作（複数操作のグループ化）
- ``ae.cache`` - プロパティキャッシュ管理
- ``ae.perf`` - パフォーマンス監視

これらのモジュールを使用することで、大量のプロパティ変更やレイヤー操作のパフォーマンスを
大幅に向上させることができます。

ae.batch（バッチ操作）
----------------------

.. currentmodule:: ae.batch

``ae.batch`` モジュールは、複数の操作をバッチとしてグループ化し、
まとめて実行することでパフォーマンスを向上させます。

基本的な使い方
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # コンテキストマネージャを使用（推奨）
   with ae.batch.Operation():
       for layer in comp.layers:
           layer.name = f"Layer_{layer.index}"
           # ... 多数の操作

   # または手動制御
   ae.batch.begin()
   try:
       # 操作...
       ae.batch.commit()
   except:
       ae.batch.rollback()
       raise
   finally:
       ae.batch.end()

API リファレンス
~~~~~~~~~~~~~~~~

.. function:: begin() -> None

   バッチ操作を開始します。

   .. warning::
      ``begin()`` を呼び出したら、必ず ``end()`` を呼び出してください。
      コンテキストマネージャの使用を推奨します。

.. function:: end() -> None

   バッチ操作を終了します。

.. function:: commit() -> None

   保留中の操作をコミットします。

.. function:: rollback() -> None

   保留中の操作をロールバックします。

.. function:: is_active() -> bool

   バッチ操作がアクティブかどうかを確認します。

   :return: アクティブな場合は True

.. function:: pending_count() -> int

   保留中の操作の数を取得します。

   :return: 保留中の操作数

Operation クラス
~~~~~~~~~~~~~~~~

.. class:: Operation

   バッチ操作のコンテキストマネージャ。

   .. code-block:: python

      with ae.batch.Operation() as op:
          # 操作...
          pass  # 自動的にコミットされる

   .. method:: commit()

      明示的にコミットします。

   .. method:: rollback()

      ロールバックします。

使用例
~~~~~~

.. code-block:: python

   import ae

   def batch_rename_layers(prefix: str):
       """レイヤーを一括リネーム"""
       comp = ae.Comp.get_active()
       if not comp:
           return

       with ae.UndoGroup("Batch Rename"):
           with ae.batch.Operation():
               for i, layer in enumerate(comp.layers):
                   layer.name = f"{prefix}_{i + 1:03d}"

       print(f"Renamed {len(comp.layers)} layers")

   batch_rename_layers("Shot_A")

ae.cache（キャッシュ管理）
--------------------------

.. currentmodule:: ae.cache

``ae.cache`` モジュールは、プロパティ値のキャッシュを管理します。
頻繁にアクセスするプロパティのパフォーマンスを向上させます。

API リファレンス
~~~~~~~~~~~~~~~~

.. function:: clear() -> None

   すべてのキャッシュをクリアします。

   .. code-block:: python

      ae.cache.clear()

.. function:: size() -> int

   現在のキャッシュエントリ数を取得します。

   :return: キャッシュされているエントリの数

   .. code-block:: python

      print(f"Cache size: {ae.cache.size()} entries")

.. function:: set_ttl(milliseconds: int) -> None

   キャッシュの TTL（Time To Live）を設定します。

   :param milliseconds: キャッシュの有効期間（ミリ秒）

   .. code-block:: python

      # キャッシュを5秒間有効に
      ae.cache.set_ttl(5000)

      # キャッシュを無効化
      ae.cache.set_ttl(0)

使用例
~~~~~~

.. code-block:: python

   import ae

   def process_with_cache():
       """キャッシュを活用した処理"""
       # 長いTTLを設定（処理中は値が変わらないと仮定）
       ae.cache.set_ttl(10000)

       try:
           comp = ae.Comp.get_active()
           for layer in comp.layers:
               # プロパティへの繰り返しアクセスがキャッシュされる
               pos = layer.property("Transform/Position")
               if pos.value[0] > comp.width / 2:
                   # 処理...
                   pass

       finally:
           # 処理後にキャッシュをクリア
           ae.cache.clear()
           ae.cache.set_ttl(1000)  # デフォルトに戻す

ae.perf（パフォーマンス監視）
-----------------------------

.. currentmodule:: ae.perf

``ae.perf`` モジュールは、API呼び出しのパフォーマンス統計を収集・表示します。

API リファレンス
~~~~~~~~~~~~~~~~

.. function:: stats() -> Dict[str, Dict[str, float]]

   パフォーマンス統計を取得します。

   :return: 操作名をキーとする辞書。各値は count, total_ms, avg_ms, max_ms, min_ms を持つ辞書

   .. code-block:: python

      stats = ae.perf.stats()
      for op_name, op_stats in stats.items():
          print(f"{op_name}: {op_stats.get('count', 0)} calls, avg {op_stats.get('avg_ms', 0):.2f} ms")

.. function:: reset() -> None

   パフォーマンス統計をリセットします。

   .. code-block:: python

      ae.perf.reset()

使用例
~~~~~~

.. code-block:: python

   import ae

   def profile_operation():
       """操作のパフォーマンスを計測"""
       # 統計をリセット
       ae.perf.reset()

       # 操作を実行
       comp = ae.Comp.get_active()
       for layer in comp.layers:
           _ = layer.name
           _ = layer.in_point
           _ = layer.out_point

       # 統計を表示
       stats = ae.perf.stats()
       print("Performance Stats:")
       for key, value in stats.items():
           print(f"  {key}: {value}")

   profile_operation()

ベストプラクティス
------------------

1. バッチ操作の活用
~~~~~~~~~~~~~~~~~~~

大量の変更を行う場合は、必ずバッチ操作を使用してください。

.. code-block:: python

   # 悪い例：個別に操作
   for layer in comp.layers:
       layer.name = f"New_{layer.index}"

   # 良い例：バッチ操作
   with ae.batch.Operation():
       for layer in comp.layers:
           layer.name = f"New_{layer.index}"

2. キャッシュの適切な使用
~~~~~~~~~~~~~~~~~~~~~~~~~

読み取り専用の処理では、キャッシュTTLを長めに設定します。

.. code-block:: python

   def analyze_composition():
       ae.cache.set_ttl(30000)  # 30秒
       try:
           # 読み取り専用の分析処理
           pass
       finally:
           ae.cache.set_ttl(1000)
           ae.cache.clear()

3. UndoGroup との併用
~~~~~~~~~~~~~~~~~~~~~

バッチ操作と UndoGroup を併用することで、パフォーマンスと取り消し機能の両方を実現します。

.. code-block:: python

   with ae.UndoGroup("Batch Edit"):
       with ae.batch.Operation():
           # 操作...
           pass

4. パフォーマンス計測
~~~~~~~~~~~~~~~~~~~~~

最適化の効果を確認するために、``ae.perf`` を使用して計測します。

.. code-block:: python

   ae.perf.reset()

   # 最適化前
   for layer in comp.layers:
       process_layer(layer)

   before_stats = ae.perf.stats()

   ae.perf.reset()

   # 最適化後
   with ae.batch.Operation():
       for layer in comp.layers:
           process_layer(layer)

   after_stats = ae.perf.stats()

   # 比較
   print(f"Before: {before_stats}")
   print(f"After: {after_stats}")

注意事項
--------

.. note::
   - バッチ操作は、操作が完了するまで画面の更新を抑制する場合があります
   - キャッシュはメモリを消費するため、大規模なプロジェクトでは注意が必要です
   - パフォーマンス監視自体にもわずかなオーバーヘッドがあります

.. warning::
   - バッチ操作中にエラーが発生した場合、適切にロールバックしてください
   - キャッシュを有効にしている間に外部で値が変更されると、不整合が発生する可能性があります
   - 本番環境では ``ae.perf`` のリセットを忘れずに行ってください

関連項目
--------

- :class:`ae.UndoGroup` - アンドゥグループ
- :doc:`layer` - レイヤー操作
- :doc:`property` - プロパティ操作
