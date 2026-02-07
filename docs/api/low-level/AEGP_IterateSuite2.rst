AEGP_IterateSuite2
==================

.. currentmodule:: ae.sdk

AEGP_IterateSuite2は、After Effectsでマルチスレッド並列処理を実行するためのSDK APIです。

概要
----

**実装状況**: 2/2関数実装、テスト済み ✅

AEGP_IterateSuite2は以下の機能を提供します:

- 利用可能なスレッド数の取得
- マルチスレッド並列処理の実行
- コールバック関数による反復処理
- スレッドセーフな処理の実装支援

基本概念
--------

マルチスレッド反復処理
~~~~~~~~~~~~~~~~~~~~~~

AEGP_IterateSuite2は、重い処理を複数のスレッドに分散して並列実行する仕組みを提供します。
After Effectsが自動的にワークロードを分割し、利用可能なCPUコアを効率的に活用します。

.. important::
   - コールバック関数は **複数のスレッドから同時に呼び出されます**
   - 共有リソースにアクセスする場合は **スレッドセーフ** な実装が必須です
   - Pythonのグローバル変数やリストへのアクセスには注意が必要です

スレッドセーフティ
~~~~~~~~~~~~~~~~~~

Pythonでマルチスレッド処理を実装する際の注意点:

1. **GIL (Global Interpreter Lock)**

   PyAEは内部的にGILを適切に管理しますが、コールバック内でのPython操作は1つのスレッドでしか実行されません。

2. **共有データの保護**

   複数スレッドから同じデータにアクセスする場合は、``threading.Lock`` を使用してください。

3. **例外処理**

   コールバック内で発生した例外は自動的にキャプチャされ、``AEGP_IterateGeneric`` が ``RuntimeError`` を送出します。

反復処理のパターン
~~~~~~~~~~~~~~~~~~

**標準的な反復処理**:

各スレッドがインデックス範囲を分担して処理を実行します。

.. code-block:: python

   def callback(thread_index, i, total):
       # thread_index: 実行しているスレッドのインデックス
       # i: 現在の反復インデックス (0 から total-1)
       # total: 全体の反復回数
       process_item(i)
       return None  # または 0（成功）

   ae.sdk.AEGP_IterateGeneric(100, callback)  # 100回の反復を並列実行

**プロセッサごとに1回実行**:

利用可能なCPUコアごとに1回だけコールバックを実行します。

.. code-block:: python

   def init_per_processor(thread_index, i, total):
       # スレッドごとの初期化処理
       setup_thread_local_data(thread_index)
       return None

   ae.sdk.AEGP_IterateGeneric(
       ae.sdk.PF_Iterations_ONCE_PER_PROCESSOR,
       init_per_processor
   )

API リファレンス
----------------

スレッド情報
~~~~~~~~~~~~

.. function:: AEGP_GetNumThreads() -> int

   After Effectsが使用可能なスレッド数を取得します。

   この値は、After Effectsの環境設定で設定された「マルチプロセッサの並列処理」の設定に基づきます。

   :return: 利用可能なスレッド数
   :rtype: int

   **例**:

   .. code-block:: python

      num_threads = ae.sdk.AEGP_GetNumThreads()
      print(f"利用可能なスレッド数: {num_threads}")

      # スレッド数に応じて処理を調整
      if num_threads >= 8:
          print("高性能マシンです")
      elif num_threads >= 4:
          print("標準的なマシンです")
      else:
          print("シングルスレッドまたは低性能マシンです")

反復処理
~~~~~~~~

.. function:: AEGP_IterateGeneric(iterations: int, callback: Callable[[int, int, int], Optional[int]]) -> None

   指定された回数だけコールバック関数を並列実行します。

   ワークロードは自動的に複数のスレッドに分散され、各スレッドがコールバック関数を呼び出します。
   コールバック関数は同時に複数スレッドから実行される可能性があるため、スレッドセーフな実装が必要です。

   :param iterations: 反復回数。-1を指定すると ``PF_Iterations_ONCE_PER_PROCESSOR`` (各プロセッサで1回実行)
   :type iterations: int
   :param callback: コールバック関数。シグネチャは ``callback(thread_index: int, i: int, iterations: int) -> Optional[int]``
   :type callback: Callable[[int, int, int], Optional[int]]
   :raises ValueError: ``iterations`` が -1 未満、または ``callback`` が ``None`` の場合
   :raises RuntimeError: 反復処理が失敗した場合、またはコールバック内で例外が発生した場合

   **コールバック関数の引数**:

   .. list-table::
      :header-rows: 1

      * - 引数
        - 型
        - 説明
      * - ``thread_index``
        - int
        - このコールバックを実行しているスレッドのインデックス (0から始まる)
      * - ``i``
        - int
        - 現在の反復インデックス (0 から iterations-1)
      * - ``iterations``
        - int
        - 全体の反復回数

   **コールバック関数の戻り値**:

   - ``None`` または ``0``: 処理を続行
   - ``非ゼロの整数``: エラーコード。反復処理を中断します

   .. warning::
      コールバック関数は **複数スレッドから同時に実行されます**。
      共有データにアクセスする場合は、必ずロックを使用してください。

   .. note::
      ``iterations = -1`` を指定すると、利用可能なプロセッサごとに1回だけコールバックが実行されます。
      この場合、``i`` の値はスレッドインデックスと同じになります。

   **基本的な使用例**:

   .. code-block:: python

      def process_item(thread_index, i, total):
          print(f"Thread {thread_index}: Processing item {i}/{total}")
          # ここで実際の処理を行う
          return None  # または 0 (成功)

      # 100回の反復を並列実行
      ae.sdk.AEGP_IterateGeneric(100, process_item)

   **エラーハンドリングの例**:

   .. code-block:: python

      def validate_and_process(thread_index, i, total):
          try:
              # 処理を実行
              result = perform_computation(i)
              if not result:
                  return 1  # エラーコードを返して反復を中断
              return 0  # 成功
          except Exception as e:
              print(f"Error in iteration {i}: {e}")
              return 1  # エラーコードで中断

      try:
          ae.sdk.AEGP_IterateGeneric(50, validate_and_process)
      except RuntimeError as e:
          print(f"Iteration failed: {e}")

   **プロセッサごとに1回実行**:

   .. code-block:: python

      def init_thread(thread_index, i, total):
          print(f"Initializing thread {thread_index}")
          # スレッドローカルな初期化処理
          return None

      # 各プロセッサで1回だけ実行
      ae.sdk.AEGP_IterateGeneric(
          ae.sdk.PF_Iterations_ONCE_PER_PROCESSOR,
          init_thread
      )

定数
~~~~

.. data:: PF_Iterations_ONCE_PER_PROCESSOR
   :type: int
   :value: -1

   ``AEGP_IterateGeneric`` に渡すことで、利用可能な各プロセッサで1回だけコールバックを実行します。

   **使用例**:

   .. code-block:: python

      ae.sdk.AEGP_IterateGeneric(
          ae.sdk.PF_Iterations_ONCE_PER_PROCESSOR,
          callback
      )

使用例
------

画像処理の並列化
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import numpy as np
   import threading

   def parallel_image_processing():
       """画像の各行を並列処理する例"""
       # 仮想的な画像データ (1920x1080)
       width, height = 1920, 1080
       image = np.zeros((height, width, 3), dtype=np.uint8)

       # スレッドセーフなロック
       lock = threading.Lock()

       def process_row(thread_index, row, total_rows):
           """各行を処理"""
           # 画像処理（例: 輝度調整）
           processed_data = image[row] * 1.2

           # 結果を書き戻す（ロックを使用）
           with lock:
               image[row] = np.clip(processed_data, 0, 255).astype(np.uint8)

           # 進捗表示（100行ごと）
           if row % 100 == 0:
               print(f"Thread {thread_index}: Processed row {row}/{total_rows}")

           return None

       print(f"Starting parallel processing with {ae.sdk.AEGP_GetNumThreads()} threads")

       # 全行を並列処理
       ae.sdk.AEGP_IterateGeneric(height, process_row)

       print("Image processing completed")
       return image

   # 実行
   result = parallel_image_processing()

レイヤーの一括処理
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import threading

   def process_all_layers_parallel(compH):
       """コンポジション内の全レイヤーを並列処理"""
       # レイヤーリストを取得
       num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
       layer_handles = []

       for i in range(num_layers):
           layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)
           layer_handles.append(layerH)

       # 結果を保存するリスト（スレッドセーフに保護）
       results = []
       lock = threading.Lock()

       def process_layer(thread_index, index, total):
           """各レイヤーを処理"""
           try:
               layerH = layer_handles[index]

               # レイヤー情報を取得
               name = ae.sdk.AEGP_GetLayerName(layerH)
               quality = ae.sdk.AEGP_GetLayerQuality(layerH)

               # 処理結果を保存
               result = {
                   'name': name,
                   'quality': quality,
                   'processed_by_thread': thread_index
               }

               with lock:
                   results.append(result)

               return None

           except Exception as e:
               print(f"Error processing layer {index}: {e}")
               return 1  # エラーで中断

       # 並列処理実行
       print(f"Processing {num_layers} layers with {ae.sdk.AEGP_GetNumThreads()} threads")
       ae.sdk.AEGP_IterateGeneric(num_layers, process_layer)

       print(f"Processed {len(results)} layers successfully")
       return results

   # 使用例
   compH = ae.sdk.AEGP_GetCompByIndex(0)
   layer_info = process_all_layers_parallel(compH)

   for info in layer_info:
       print(f"Layer: {info['name']}, Quality: {info['quality']}, Thread: {info['processed_by_thread']}")

データ集計の並列化
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import threading

   def parallel_data_aggregation(data_list):
       """大量のデータを並列で集計"""
       # スレッドローカルな結果を保存
       thread_local_results = {}
       lock = threading.Lock()

       def aggregate_chunk(thread_index, i, total):
           """各データポイントを処理"""
           value = data_list[i]

           # スレッドローカルな結果を更新
           with lock:
               if thread_index not in thread_local_results:
                   thread_local_results[thread_index] = {
                       'sum': 0,
                       'count': 0,
                       'min': float('inf'),
                       'max': float('-inf')
                   }

               result = thread_local_results[thread_index]
               result['sum'] += value
               result['count'] += 1
               result['min'] = min(result['min'], value)
               result['max'] = max(result['max'], value)

           return None

       # 並列処理実行
       ae.sdk.AEGP_IterateGeneric(len(data_list), aggregate_chunk)

       # スレッド結果を統合
       final_result = {
           'sum': 0,
           'count': 0,
           'min': float('inf'),
           'max': float('-inf')
       }

       for thread_result in thread_local_results.values():
           final_result['sum'] += thread_result['sum']
           final_result['count'] += thread_result['count']
           final_result['min'] = min(final_result['min'], thread_result['min'])
           final_result['max'] = max(final_result['max'], thread_result['max'])

       final_result['average'] = final_result['sum'] / final_result['count']

       return final_result

   # 使用例
   data = list(range(100000))  # 大量のデータ
   stats = parallel_data_aggregation(data)
   print(f"Sum: {stats['sum']}, Average: {stats['average']}, Min: {stats['min']}, Max: {stats['max']}")

プログレスバー付き並列処理
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import threading
   import time

   def parallel_with_progress(total_items):
       """プログレス表示付き並列処理"""
       # 進捗カウンタ
       completed = {'count': 0}
       lock = threading.Lock()

       def process_with_progress(thread_index, i, total):
           """処理とプログレス更新"""
           # 時間のかかる処理をシミュレート
           time.sleep(0.01)

           # プログレス更新
           with lock:
               completed['count'] += 1
               current = completed['count']

               # 10%ごとに表示
               if current % (total // 10) == 0:
                   percent = (current / total) * 100
                   print(f"Progress: {percent:.1f}% ({current}/{total})")

           return None

       print("Starting parallel processing...")
       start_time = time.time()

       # 並列実行
       ae.sdk.AEGP_IterateGeneric(total_items, process_with_progress)

       elapsed = time.time() - start_time
       print(f"Completed in {elapsed:.2f} seconds")
       print(f"Items per second: {total_items / elapsed:.2f}")

   # 実行
   parallel_with_progress(1000)

スレッドごとの初期化と処理
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import threading

   def multi_phase_parallel_processing():
       """スレッドごとに初期化してから並列処理"""
       # フェーズ1: 各スレッドを初期化
       thread_data = {}
       lock = threading.Lock()

       def initialize_thread(thread_index, i, total):
           """各スレッドの初期化"""
           print(f"Initializing thread {thread_index}")

           with lock:
               thread_data[thread_index] = {
                   'processed_items': [],
                   'start_time': threading.current_thread().name
               }

           return None

       # 各プロセッサで1回初期化
       print("Phase 1: Initializing threads...")
       ae.sdk.AEGP_IterateGeneric(
           ae.sdk.PF_Iterations_ONCE_PER_PROCESSOR,
           initialize_thread
       )

       print(f"Initialized {len(thread_data)} threads")

       # フェーズ2: データを並列処理
       def process_item(thread_index, i, total):
           """各アイテムを処理"""
           # スレッドローカルデータに結果を保存
           with lock:
               if thread_index in thread_data:
                   thread_data[thread_index]['processed_items'].append(i)

           return None

       print("Phase 2: Processing data...")
       ae.sdk.AEGP_IterateGeneric(100, process_item)

       # 結果を表示
       print("\nResults per thread:")
       for thread_index, data in thread_data.items():
           items = data['processed_items']
           print(f"Thread {thread_index}: Processed {len(items)} items")

   # 実行
   multi_phase_parallel_processing()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **スレッドセーフティ**

   コールバック関数は複数のスレッドから同時に呼び出されます。共有データにアクセスする場合は、必ず ``threading.Lock`` を使用してください。

2. **例外処理**

   コールバック内で発生した例外は自動的にキャプチャされ、``AEGP_IterateGeneric`` が ``RuntimeError`` を送出します。
   エラーハンドリングは ``try-except`` ブロックで行ってください。

3. **GIL (Global Interpreter Lock)**

   PythonのGILにより、純粋なPython計算は並列化されません。
   この関数は主に、I/O処理やAEGP API呼び出しの並列化に適しています。

4. **反復処理の中断**

   コールバックから非ゼロの値を返すと、反復処理が中断されます。
   正常に続行する場合は ``None`` または ``0`` を返してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

スレッド数の確認
^^^^^^^^^^^^^^^^

.. code-block:: python

   num_threads = ae.sdk.AEGP_GetNumThreads()
   if num_threads == 1:
       print("Warning: Single-threaded mode")
   else:
       print(f"Using {num_threads} threads")

ロックの使用
^^^^^^^^^^^^

.. code-block:: python

   import threading

   lock = threading.Lock()
   shared_data = []

   def safe_callback(thread_index, i, total):
       with lock:
           shared_data.append(i)
       return None

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       ae.sdk.AEGP_IterateGeneric(100, callback)
   except RuntimeError as e:
       print(f"Iteration failed: {e}")
       # エラー処理

パフォーマンスの測定
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   import time

   start = time.time()
   ae.sdk.AEGP_IterateGeneric(10000, callback)
   elapsed = time.time() - start
   print(f"Completed in {elapsed:.2f} seconds")

プログレス表示
^^^^^^^^^^^^^^

.. code-block:: python

   completed = {'count': 0}
   lock = threading.Lock()

   def callback_with_progress(thread_index, i, total):
       # 処理

       with lock:
           completed['count'] += 1
           if completed['count'] % 100 == 0:
               percent = (completed['count'] / total) * 100
               print(f"{percent:.1f}% complete")

       return None

パフォーマンス考慮事項
~~~~~~~~~~~~~~~~~~~~~~

1. **粒度の調整**

   反復回数が少なすぎると、スレッド管理のオーバーヘッドが大きくなります。
   最低でも ``スレッド数 × 10`` 程度の反復回数を推奨します。

2. **ロックの最小化**

   ロックを保持する時間を最小限に抑えてください。
   重い処理はロックの外で行い、結果の書き込みだけをロック内で行います。

3. **メモリ使用量**

   各スレッドがメモリを使用するため、メモリ消費量に注意してください。

適用可能な処理
^^^^^^^^^^^^^^

**適している処理**:

- レイヤーの一括処理
- フレームごとの解析
- データの集計・統計処理
- I/O操作（ファイル読み込みなど）
- AEGP API呼び出し

**適していない処理**:

- 純粋なPython計算（GILの影響を受ける）
- 非常に軽い処理（オーバーヘッドが大きい）
- 順序に依存する処理

関連項目
--------

- :doc:`AEGP_UtilitySuite6` - エラー処理、Undo管理
- :doc:`AEGP_LayerSuite9` - レイヤー操作（並列処理の対象として）
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`index` - 低レベルAPI概要
