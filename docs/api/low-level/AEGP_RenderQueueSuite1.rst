AEGP_RenderQueueSuite1
======================

.. currentmodule:: ae.sdk

AEGP_RenderQueueSuite1は、After Effectsのレンダーキューを操作するためのSDK APIです。

概要
----

**実装状況**: 3/3関数実装済み ✅

AEGP_RenderQueueSuite1は以下の機能を提供します:

- コンポジションをレンダーキューに追加
- レンダーキューの状態を取得・設定
- レンダリングの開始・一時停止・停止

基本概念
--------

レンダーキューの状態 (AEGP_RenderQueueState)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダーキューは以下の3つの状態を持ちます:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 状態
     - 説明
   * - ``AEGP_RenderQueueState_STOPPED``
     - 停止状態（レンダリングが行われていない）
   * - ``AEGP_RenderQueueState_PAUSED``
     - 一時停止状態（レンダリング中断）
   * - ``AEGP_RenderQueueState_RENDERING``
     - レンダリング中

.. note::
   PyAEでは、これらの状態は整数値として扱われます:

   - ``ae.sdk.AEGP_RenderQueueState_STOPPED`` = 0
   - ``ae.sdk.AEGP_RenderQueueState_PAUSED`` = 1
   - ``ae.sdk.AEGP_RenderQueueState_RENDERING`` = 2

レンダーキュー項目
~~~~~~~~~~~~~~~~~~

``AEGP_AddCompToRenderQueue`` でコンポジションをレンダーキューに追加すると、新しいレンダーキュー項目が作成されます。
各項目には以下が含まれます:

- 出力パス
- 出力モジュール設定
- レンダー設定

.. tip::
   レンダーキュー項目の詳細な操作には、``AEGP_RQItemSuite`` を使用します。

API リファレンス
----------------

コンポジションの追加
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_AddCompToRenderQueue(compH: int, path: str) -> None

   コンポジションをレンダーキューに追加し、出力パスを設定します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param path: 出力ファイルのパス（絶対パスを推奨）
   :type path: str
   :raises RuntimeError: コンポジションハンドルがNULL、パスが空、または追加に失敗した場合
   :raises ValueError: 引数の検証に失敗した場合

   .. important::
      - ``compH`` は有効なコンポジションハンドルである必要があります
      - ``path`` は空文字列であってはいけません
      - 出力パスは絶対パスを指定することを推奨します

   **例**:

   .. code-block:: python

      import ae

      # コンポジションを取得
      projH = ae.sdk.AEGP_GetProjectByIndex(0)
      compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)

      # レンダーキューに追加
      output_path = r"C:\Output\render.mov"
      ae.sdk.AEGP_AddCompToRenderQueue(compH, output_path)

レンダーキューの状態管理
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetRenderQueueState() -> int

   レンダーキューの現在の状態を取得します。

   :return: レンダーキューの状態（AEGP_RenderQueueState）
   :rtype: int
   :raises RuntimeError: RenderQueue Suiteが利用できない、または取得に失敗した場合

   **戻り値**:

   - ``AEGP_RenderQueueState_STOPPED`` (0) - 停止状態
   - ``AEGP_RenderQueueState_PAUSED`` (1) - 一時停止状態
   - ``AEGP_RenderQueueState_RENDERING`` (2) - レンダリング中

   **例**:

   .. code-block:: python

      import ae

      state = ae.sdk.AEGP_GetRenderQueueState()

      if state == ae.sdk.AEGP_RenderQueueState_STOPPED:
          print("レンダーキューは停止しています")
      elif state == ae.sdk.AEGP_RenderQueueState_PAUSED:
          print("レンダーキューは一時停止中です")
      elif state == ae.sdk.AEGP_RenderQueueState_RENDERING:
          print("レンダリング中です")

.. function:: AEGP_SetRenderQueueState(state: int) -> None

   レンダーキューの状態を設定します。

   :param state: 設定する状態（AEGP_RenderQueueState）
   :type state: int
   :raises RuntimeError: RenderQueue Suiteが利用できない、または設定に失敗した場合

   **設定可能な状態**:

   - ``AEGP_RenderQueueState_STOPPED`` (0) - レンダリングを停止
   - ``AEGP_RenderQueueState_PAUSED`` (1) - レンダリングを一時停止
   - ``AEGP_RenderQueueState_RENDERING`` (2) - レンダリングを開始

   .. warning::
      レンダリング中（``RENDERING``）に ``STOPPED`` を設定すると、レンダリングは即座に中断されます。
      一時停止（``PAUSED``）から再開するには、``RENDERING`` を設定します。

   **例**:

   .. code-block:: python

      import ae

      # レンダリングを開始
      ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_RENDERING)

      # 一時停止
      ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_PAUSED)

      # 停止
      ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_STOPPED)

使用例
------

基本的なレンダリングワークフロー
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import time

   def render_composition(comp_index=0, output_path=r"C:\Output\render.mov"):
       """コンポジションをレンダリングする基本的なワークフロー"""

       # プロジェクトとコンポジションを取得
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       compH = ae.sdk.AEGP_GetCompFromProject(projH, comp_index)

       # レンダーキューに追加
       ae.sdk.AEGP_AddCompToRenderQueue(compH, output_path)
       print(f"コンポジションをレンダーキューに追加しました: {output_path}")

       # レンダリングを開始
       ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_RENDERING)
       print("レンダリングを開始しました")

       # レンダリングの完了を待機（簡易的なポーリング）
       while True:
           state = ae.sdk.AEGP_GetRenderQueueState()
           if state == ae.sdk.AEGP_RenderQueueState_STOPPED:
               print("レンダリングが完了しました")
               break
           elif state == ae.sdk.AEGP_RenderQueueState_RENDERING:
               print("レンダリング中...")
               time.sleep(1)
           else:
               time.sleep(0.5)

   # 実行
   render_composition()

複数のコンポジションをバッチレンダリング
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def batch_render_compositions(output_dir=r"C:\Output"):
       """プロジェクト内のすべてのコンポジションをレンダリング"""

       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       num_items = ae.sdk.AEGP_GetNumItemsInProject(projH)

       comp_count = 0

       # すべてのアイテムをチェック
       for i in range(num_items):
           itemH = ae.sdk.AEGP_GetItemByIndex(projH, i)
           item_type = ae.sdk.AEGP_GetItemType(itemH)

           # コンポジションのみ処理
           if item_type == ae.sdk.ItemType_COMP:
               comp_name = ae.sdk.AEGP_GetItemName(itemH)
               compH = ae.sdk.AEGP_GetCompFromItem(itemH)

               # 出力パスを生成
               output_path = f"{output_dir}\\{comp_name}.mov"

               # レンダーキューに追加
               ae.sdk.AEGP_AddCompToRenderQueue(compH, output_path)
               comp_count += 1
               print(f"追加: {comp_name} -> {output_path}")

       print(f"\n{comp_count}個のコンポジションを追加しました")

       # バッチレンダリングを開始
       ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_RENDERING)
       print("バッチレンダリングを開始しました")

   # 実行
   batch_render_compositions()

レンダリングの監視と制御
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import time

   def monitored_render():
       """レンダリングを監視し、必要に応じて制御する"""

       # レンダリングを開始
       ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_RENDERING)
       print("レンダリングを開始しました")

       render_time = 0
       max_render_time = 300  # 最大5分

       try:
           while True:
               state = ae.sdk.AEGP_GetRenderQueueState()

               if state == ae.sdk.AEGP_RenderQueueState_STOPPED:
                   print("レンダリングが完了しました")
                   break
               elif state == ae.sdk.AEGP_RenderQueueState_RENDERING:
                   render_time += 1
                   print(f"レンダリング中... ({render_time}秒経過)")

                   # タイムアウトチェック
                   if render_time >= max_render_time:
                       print("タイムアウト: レンダリングを停止します")
                       ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_STOPPED)
                       break

                   time.sleep(1)
               else:
                   time.sleep(0.5)

       except KeyboardInterrupt:
           # ユーザーがCtrl+Cを押した場合
           print("\nレンダリングを中断します")
           ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_STOPPED)

   # 実行
   monitored_render()

一時停止・再開機能
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import time

   def render_with_pause_resume():
       """レンダリングを一時停止・再開する例"""

       # レンダリングを開始
       ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_RENDERING)
       print("レンダリングを開始しました")

       # 5秒後に一時停止
       time.sleep(5)
       ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_PAUSED)
       print("レンダリングを一時停止しました")

       # ユーザーに確認
       input("Enterキーを押すとレンダリングを再開します...")

       # 再開
       ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_RENDERING)
       print("レンダリングを再開しました")

       # 完了まで待機
       while ae.sdk.AEGP_GetRenderQueueState() != ae.sdk.AEGP_RenderQueueState_STOPPED:
           time.sleep(1)

       print("レンダリングが完了しました")

   # 実行
   render_with_pause_resume()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **出力パスの指定**

   ``AEGP_AddCompToRenderQueue`` では絶対パスを指定してください。相対パスは予期しない場所に出力される可能性があります。

2. **レンダーキューの状態遷移**

   以下の状態遷移が可能です:

   - ``STOPPED`` → ``RENDERING`` : レンダリング開始
   - ``RENDERING`` → ``PAUSED`` : 一時停止
   - ``PAUSED`` → ``RENDERING`` : 再開
   - ``RENDERING`` → ``STOPPED`` : 停止（中断）
   - ``PAUSED`` → ``STOPPED`` : 停止（キャンセル）

3. **レンダリング中のプロジェクト操作**

   レンダリング中にプロジェクトを編集すると、予期しない動作が発生する可能性があります。
   レンダリング前にプロジェクトを保存することを推奨します。

4. **出力ディレクトリの存在確認**

   出力先のディレクトリが存在しない場合、レンダリングは失敗します。
   事前にディレクトリの存在を確認するか、作成してください。

5. **非同期処理**

   ``AEGP_SetRenderQueueState(RENDERING)`` はレンダリングを開始しますが、完了を待ちません。
   レンダリングの完了を待つには、状態をポーリングする必要があります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

出力パスの検証
^^^^^^^^^^^^^^

.. code-block:: python

   import os

   output_path = r"C:\Output\render.mov"
   output_dir = os.path.dirname(output_path)

   # ディレクトリが存在しない場合は作成
   if not os.path.exists(output_dir):
       os.makedirs(output_dir)

   ae.sdk.AEGP_AddCompToRenderQueue(compH, output_path)

プロジェクトの保存
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # レンダリング前にプロジェクトを保存
   plugin_id = ae.sdk.AEGP_GetPluginID()
   projH = ae.sdk.AEGP_GetProjectByIndex(0)

   if ae.sdk.AEGP_ProjectIsDirty(projH):
       current_path = ae.sdk.AEGP_GetProjectPath(plugin_id, projH)
       if current_path:
           ae.sdk.AEGP_SaveProjectToPath(projH, current_path)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       ae.sdk.AEGP_AddCompToRenderQueue(compH, output_path)
       ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_RENDERING)
   except RuntimeError as e:
       print(f"レンダリングエラー: {e}")
   except ValueError as e:
       print(f"引数エラー: {e}")

レンダリング完了の待機
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   import time

   # レンダリング開始
   ae.sdk.AEGP_SetRenderQueueState(ae.sdk.AEGP_RenderQueueState_RENDERING)

   # 完了を待機
   while ae.sdk.AEGP_GetRenderQueueState() != ae.sdk.AEGP_RenderQueueState_STOPPED:
       time.sleep(1)  # 1秒ごとにチェック

   print("レンダリング完了")

制限事項
~~~~~~~~

1. **レンダーキュー項目の詳細設定**

   このSuiteでは、レンダーキュー項目の詳細設定（出力モジュール、レンダー設定など）は行えません。
   これらの設定には、``AEGP_RQItemSuite`` や ``AEGP_OutputModuleSuite`` を使用してください。

2. **レンダリングの進捗取得**

   レンダリングの進捗（％）を取得する機能は提供されていません。
   詳細な進捗監視には、``AEGP_RenderQueueMonitorSuite`` を使用してください。

3. **複数のレンダーキュー項目の制御**

   このSuiteでは、レンダーキュー全体の状態のみを制御できます。
   個別のレンダーキュー項目を制御するには、``AEGP_RQItemSuite`` を使用してください。

関連項目
--------

- :doc:`AEGP_RQItemSuite4` - レンダーキュー項目の詳細操作
- :doc:`AEGP_OutputModuleSuite4` - 出力モジュール設定
- :doc:`AEGP_RenderOptionsSuite4` - レンダー設定
- :doc:`AEGP_RenderQueueMonitorSuite1` - レンダリング進捗の監視
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`index` - 低レベルAPI概要
