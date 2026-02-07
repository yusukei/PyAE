AEGP_RQItemSuite4
=================

.. currentmodule:: ae.sdk

AEGP_RQItemSuite4は、After Effectsのレンダーキューアイテム（Render Queue Items）を管理するためのSDK APIです。

概要
----

**実装状況**: 14/14関数実装 ✅

AEGP_RQItemSuite4は以下の機能を提供します:

- レンダーキューアイテムの取得、列挙
- レンダー状態（キュー状態、レンダリング中、完了など）の取得・設定
- レンダー時間情報（開始時刻、経過時間）の取得
- ログタイプ設定の取得・設定
- 出力モジュールの管理
- コメントの取得・設定
- レンダーキューアイテムの削除

基本概念
--------

レンダーキューアイテムハンドル (AEGP_RQItemRefH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダーキューに追加されたコンポジションを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - レンダーキューアイテムハンドルは、アイテムがレンダーキューに存在する間のみ有効です
   - レンダーキューアイテムを削除した後は、ハンドルは無効になります
   - 各レンダーキューアイテムには、1つ以上の出力モジュールが関連付けられています

レンダー状態 (AEGP_RenderItemStatusType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダーキューアイテムの現在の状態を表します。

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_RenderItemStatus_NEEDS_OUTPUT``
     - 0
     - 出力モジュールが必要
   * - ``AEGP_RenderItemStatus_UNQUEUED``
     - 1
     - レンダー準備完了だがキューに含まれていない
   * - ``AEGP_RenderItemStatus_QUEUED``
     - 2
     - レンダー準備完了かつキューに含まれている
   * - ``AEGP_RenderItemStatus_RENDERING``
     - 3
     - レンダリング中
   * - ``AEGP_RenderItemStatus_USER_STOPPED``
     - 4
     - ユーザーによって停止
   * - ``AEGP_RenderItemStatus_ERR_STOPPED``
     - 5
     - エラーで停止
   * - ``AEGP_RenderItemStatus_DONE``
     - 6
     - レンダリング完了

PyAEでの利用:

.. code-block:: python

   from ae import sdk

   # 定数として利用可能
   sdk.AEGP_RenderItemStatus_QUEUED  # 2
   sdk.AEGP_RenderItemStatus_DONE    # 6

ログタイプ (AEGP_LogType)
~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダーキューアイテムのログ記録レベルを指定します。

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_LogType_NONE``
     - -1
     - ログを記録しない
   * - ``AEGP_LogType_ERRORS_ONLY``
     - 0
     - エラーのみ記録
   * - ``AEGP_LogType_PLUS_SETTINGS``
     - 1
     - エラーと設定情報を記録
   * - ``AEGP_LogType_PER_FRAME_INFO``
     - 2
     - フレームごとの詳細情報を記録

API リファレンス
----------------

レンダーキューアイテムの取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNumRQItems() -> int

   レンダーキュー内のアイテム数を取得します。

   :return: レンダーキューアイテム数
   :rtype: int

   **例**:

   .. code-block:: python

      num_items = ae.sdk.AEGP_GetNumRQItems()
      print(f"レンダーキューアイテム数: {num_items}")

.. function:: AEGP_GetRQItemByIndex(index: int) -> int

   インデックスからレンダーキューアイテムハンドルを取得します。

   :param index: レンダーキューアイテムのインデックス（0から始まる）
   :type index: int
   :return: レンダーキューアイテムハンドル
   :rtype: int

   .. note::
      インデックスは0から始まります。範囲外のインデックスを指定するとエラーになります。

   **例**:

   .. code-block:: python

      rq_itemH = ae.sdk.AEGP_GetRQItemByIndex(0)  # 最初のアイテム

.. function:: AEGP_GetNextRQItem(current_rq_itemH: int) -> int

   次のレンダーキューアイテムハンドルを取得します。

   :param current_rq_itemH: 現在のレンダーキューアイテムハンドル（0を渡すと最初のアイテムを取得）
   :type current_rq_itemH: int
   :return: 次のレンダーキューアイテムハンドル（次がない場合は0）
   :rtype: int

   .. tip::
      ``current_rq_itemH`` に0を渡すと、最初のレンダーキューアイテムを取得できます。
      戻り値が0の場合、次のアイテムは存在しません。

   **例**:

   .. code-block:: python

      # 最初のアイテムを取得
      rq_itemH = ae.sdk.AEGP_GetNextRQItem(0)

      # レンダーキュー全体を列挙
      rq_itemH = ae.sdk.AEGP_GetNextRQItem(0)
      while rq_itemH != 0:
          # アイテムを処理
          rq_itemH = ae.sdk.AEGP_GetNextRQItem(rq_itemH)

.. function:: AEGP_GetCompFromRQItem(rq_itemH: int) -> int

   レンダーキューアイテムから、元のコンポジションハンドルを取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :return: コンポジションハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      compH = ae.sdk.AEGP_GetCompFromRQItem(rq_itemH)

レンダー状態の取得・設定
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetRenderState(rq_itemH: int) -> int

   レンダーキューアイテムの現在のレンダー状態を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :return: レンダー状態（``AEGP_RenderItemStatusType``）
   :rtype: int

   **例**:

   .. code-block:: python

      status = ae.sdk.AEGP_GetRenderState(rq_itemH)

      if status == ae.sdk.AEGP_RenderItemStatus_QUEUED:
          print("レンダリング準備完了")
      elif status == ae.sdk.AEGP_RenderItemStatus_RENDERING:
          print("レンダリング中")
      elif status == ae.sdk.AEGP_RenderItemStatus_DONE:
          print("レンダリング完了")

.. function:: AEGP_SetRenderState(rq_itemH: int, status: int) -> None

   レンダーキューアイテムのレンダー状態を設定します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param status: 設定するレンダー状態（``AEGP_RenderItemStatusType``）
   :type status: int

   .. warning::
      一部の状態遷移は制限されています。たとえば、``AEGP_RenderItemStatus_USER_STOPPED`` から ``AEGP_RenderItemStatus_RENDERING`` への遷移はできません。

   **例**:

   .. code-block:: python

      # レンダーキューアイテムをキューに追加
      ae.sdk.AEGP_SetRenderState(rq_itemH, ae.sdk.AEGP_RenderItemStatus_QUEUED)

レンダー時間情報
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetStartedTime(rq_itemH: int) -> float

   レンダリングが開始された時刻を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :return: レンダリング開始時刻（秒単位、未開始の場合は0）
   :rtype: float

   .. note::
      この値は、After Effectsの内部時間表現から秒単位に変換されています。
      レンダリングが開始されていない場合は0を返します。

   **例**:

   .. code-block:: python

      started_time = ae.sdk.AEGP_GetStartedTime(rq_itemH)
      if started_time > 0:
          print(f"レンダリング開始時刻: {started_time}秒")

.. function:: AEGP_GetElapsedTime(rq_itemH: int) -> float

   レンダリングの経過時間を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :return: レンダリング経過時間（秒単位、未レンダリングの場合は0）
   :rtype: float

   .. note::
      レンダリングが完了していない場合は0を返します。

   **例**:

   .. code-block:: python

      elapsed_time = ae.sdk.AEGP_GetElapsedTime(rq_itemH)
      if elapsed_time > 0:
          print(f"レンダリング経過時間: {elapsed_time:.2f}秒")

ログタイプの取得・設定
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLogType(rq_itemH: int) -> int

   レンダーキューアイテムのログタイプを取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :return: ログタイプ（``AEGP_LogType``）
   :rtype: int

   **例**:

   .. code-block:: python

      log_type = ae.sdk.AEGP_GetLogType(rq_itemH)
      print(f"ログタイプ: {log_type}")

.. function:: AEGP_SetLogType(rq_itemH: int, logtype: int) -> None

   レンダーキューアイテムのログタイプを設定します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param logtype: ログタイプ（``AEGP_LogType``）
   :type logtype: int

   **例**:

   .. code-block:: python

      # 詳細ログを有効化
      ae.sdk.AEGP_SetLogType(rq_itemH, 2)  # AEGP_LogType_PER_FRAME_INFO

出力モジュール管理
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNumOutputModulesForRQItem(rq_itemH: int) -> int

   レンダーキューアイテムに関連付けられた出力モジュールの数を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :return: 出力モジュール数
   :rtype: int

   **例**:

   .. code-block:: python

      num_modules = ae.sdk.AEGP_GetNumOutputModulesForRQItem(rq_itemH)
      print(f"出力モジュール数: {num_modules}")

.. function:: AEGP_RemoveOutputModule(rq_itemH: int, outmodH: int) -> None

   レンダーキューアイテムから出力モジュールを削除します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int

   .. warning::
      レンダーキューアイテムには、最低1つの出力モジュールが必要です。
      最後の出力モジュールを削除しようとするとエラーになります。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_RemoveOutputModule(rq_itemH, outmodH)

コメントの取得・設定
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetComment(rq_itemH: int) -> str

   レンダーキューアイテムのコメントを取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :return: コメント（Unicode文字列、コメントがない場合は空文字列）
   :rtype: str

   .. note::
      内部的には ``AEGP_MemorySuite`` を使用してメモリ管理が行われます。
      メモリリークを防ぐため、自動的にクリーンアップされます。

   **例**:

   .. code-block:: python

      comment = ae.sdk.AEGP_GetComment(rq_itemH)
      if comment:
          print(f"コメント: {comment}")

.. function:: AEGP_SetComment(rq_itemH: int, comment: str) -> None

   レンダーキューアイテムにコメントを設定します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param comment: 設定するコメント（Unicode文字列）
   :type comment: str

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetComment(rq_itemH, "最終レンダリング - HD品質")

レンダーキューアイテムの削除
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_DeleteRQItem(rq_itemH: int) -> None

   レンダーキューアイテムを削除します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int

   .. warning::
      削除後、ハンドルは無効になります。削除後のハンドルを使用しないでください。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DeleteRQItem(rq_itemH)

使用例
------

レンダーキューの情報を取得
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_render_queue_info():
       """レンダーキュー全体の情報を表示"""
       num_items = ae.sdk.AEGP_GetNumRQItems()
       print(f"レンダーキューアイテム数: {num_items}")
       print("=" * 60)

       for i in range(num_items):
           rq_itemH = ae.sdk.AEGP_GetRQItemByIndex(i)

           # コンポジション名を取得
           compH = ae.sdk.AEGP_GetCompFromRQItem(rq_itemH)
           comp_name = ae.sdk.AEGP_GetCompName(compH)

           # レンダー状態を取得
           status = ae.sdk.AEGP_GetRenderState(rq_itemH)
           status_names = {
               0: "出力モジュール必要",
               1: "キュー外",
               2: "キューに追加済み",
               3: "レンダリング中",
               4: "ユーザーによって停止",
               5: "エラーで停止",
               6: "完了"
           }
           status_name = status_names.get(status, "不明")

           # コメントを取得
           comment = ae.sdk.AEGP_GetComment(rq_itemH)

           # 出力モジュール数
           num_modules = ae.sdk.AEGP_GetNumOutputModulesForRQItem(rq_itemH)

           print(f"[{i}] {comp_name}")
           print(f"    状態: {status_name} ({status})")
           print(f"    出力モジュール数: {num_modules}")
           if comment:
               print(f"    コメント: {comment}")

           # レンダリング時間情報
           elapsed = ae.sdk.AEGP_GetElapsedTime(rq_itemH)
           if elapsed > 0:
               print(f"    経過時間: {elapsed:.2f}秒")
           print()

   # 実行
   dump_render_queue_info()

レンダーキューの状態を一括変更
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def queue_all_items():
       """すべてのレンダーキューアイテムをキューに追加"""
       num_items = ae.sdk.AEGP_GetNumRQItems()

       for i in range(num_items):
           rq_itemH = ae.sdk.AEGP_GetRQItemByIndex(i)

           # 現在の状態を確認
           status = ae.sdk.AEGP_GetRenderState(rq_itemH)

           # キューに追加されていない場合のみ追加
           if status != ae.sdk.AEGP_RenderItemStatus_QUEUED:
               ae.sdk.AEGP_SetRenderState(
                   rq_itemH, ae.sdk.AEGP_RenderItemStatus_QUEUED
               )
               compH = ae.sdk.AEGP_GetCompFromRQItem(rq_itemH)
               comp_name = ae.sdk.AEGP_GetCompName(compH)
               print(f"キューに追加: {comp_name}")

   # 実行
   queue_all_items()

レンダーキューアイテムの列挙（GetNextRQItem使用）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def enumerate_render_queue():
       """GetNextRQItemを使用してレンダーキューを列挙"""
       rq_itemH = ae.sdk.AEGP_GetNextRQItem(0)  # 最初のアイテム

       index = 0
       while rq_itemH != 0:
           # コンポジション名を取得
           compH = ae.sdk.AEGP_GetCompFromRQItem(rq_itemH)
           comp_name = ae.sdk.AEGP_GetCompName(compH)

           print(f"[{index}] {comp_name}")

           # 次のアイテムへ
           rq_itemH = ae.sdk.AEGP_GetNextRQItem(rq_itemH)
           index += 1

   # 実行
   enumerate_render_queue()

完了したアイテムのクリーンアップ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def cleanup_completed_items():
       """レンダリング完了したアイテムをレンダーキューから削除"""
       # 後ろから削除していく（インデックスが変わらないように）
       num_items = ae.sdk.AEGP_GetNumRQItems()

       for i in range(num_items - 1, -1, -1):
           rq_itemH = ae.sdk.AEGP_GetRQItemByIndex(i)
           status = ae.sdk.AEGP_GetRenderState(rq_itemH)

           if status == ae.sdk.AEGP_RenderItemStatus_DONE:
               compH = ae.sdk.AEGP_GetCompFromRQItem(rq_itemH)
               comp_name = ae.sdk.AEGP_GetCompName(compH)

               ae.sdk.AEGP_DeleteRQItem(rq_itemH)
               print(f"削除: {comp_name}")

   # 実行
   cleanup_completed_items()

ログレベルを一括設定
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_detailed_logging_for_all():
       """すべてのレンダーキューアイテムに詳細ログを設定"""
       num_items = ae.sdk.AEGP_GetNumRQItems()

       for i in range(num_items):
           rq_itemH = ae.sdk.AEGP_GetRQItemByIndex(i)

           # 詳細ログを有効化（フレームごとの情報）
           ae.sdk.AEGP_SetLogType(rq_itemH, 2)  # AEGP_LogType_PER_FRAME_INFO

           compH = ae.sdk.AEGP_GetCompFromRQItem(rq_itemH)
           comp_name = ae.sdk.AEGP_GetCompName(compH)
           print(f"詳細ログを有効化: {comp_name}")

   # 実行
   set_detailed_logging_for_all()

レンダーキューの状態監視
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import time

   def monitor_render_progress():
       """レンダリングの進行状況を監視"""
       num_items = ae.sdk.AEGP_GetNumRQItems()

       while True:
           all_done = True

           for i in range(num_items):
               rq_itemH = ae.sdk.AEGP_GetRQItemByIndex(i)
               status = ae.sdk.AEGP_GetRenderState(rq_itemH)

               compH = ae.sdk.AEGP_GetCompFromRQItem(rq_itemH)
               comp_name = ae.sdk.AEGP_GetCompName(compH)

               if status == ae.sdk.AEGP_RenderItemStatus_RENDERING:
                   elapsed = ae.sdk.AEGP_GetElapsedTime(rq_itemH)
                   print(f"レンダリング中: {comp_name} - {elapsed:.1f}秒経過")
                   all_done = False

               elif status != ae.sdk.AEGP_RenderItemStatus_DONE:
                   all_done = False

           if all_done:
               print("すべてのレンダリングが完了しました")
               break

           time.sleep(5)  # 5秒ごとにチェック

   # 注意: この関数はレンダリングを開始しません。
   # レンダリングはユーザーまたは別のスクリプトで開始する必要があります。

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **ハンドルの有効期限**

   レンダーキューアイテムハンドルは、アイテムがレンダーキューに存在する間のみ有効です。
   削除後はハンドルを使用しないでください。

2. **状態遷移の制限**

   一部のレンダー状態間の遷移は制限されています。
   たとえば、レンダリング中のアイテムを直接キューに戻すことはできません。

3. **最後の出力モジュールの削除禁止**

   レンダーキューアイテムには、最低1つの出力モジュールが必要です。

4. **インデックスの範囲**

   ``AEGP_GetRQItemByIndex()`` は範囲チェックを行います。
   0から ``AEGP_GetNumRQItems()-1`` の範囲内のインデックスを指定してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

アイテム削除時は後ろから
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 正しい: 後ろから削除
   for i in range(num_items - 1, -1, -1):
       ae.sdk.AEGP_DeleteRQItem(...)

   # 間違い: 前から削除（インデックスがずれる）
   for i in range(num_items):
       ae.sdk.AEGP_DeleteRQItem(...)

GetNextRQItem を使った列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   rq_itemH = ae.sdk.AEGP_GetNextRQItem(0)
   while rq_itemH != 0:
       # 処理
       rq_itemH = ae.sdk.AEGP_GetNextRQItem(rq_itemH)

状態確認後の操作
^^^^^^^^^^^^^^^^

.. code-block:: python

   status = ae.sdk.AEGP_GetRenderState(rq_itemH)
   if status != ae.sdk.AEGP_RenderItemStatus_RENDERING:
       # レンダリング中でない場合のみ操作

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       rq_itemH = ae.sdk.AEGP_GetRQItemByIndex(index)
   except Exception as e:
       print(f"レンダーキューアイテムの取得に失敗: {e}")

関連項目
--------

- :doc:`AEGP_RenderQueueSuite1` - レンダーキュー全体の管理
- :doc:`AEGP_OutputModuleSuite4` - 出力モジュール管理
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`index` - 低レベルAPI概要
