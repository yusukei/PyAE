AEGP_ProjSuite6
===============

.. currentmodule:: ae.sdk

AEGP_ProjSuite6は、After Effectsプロジェクトの管理、保存、設定変更を行うためのSDK APIです。

概要
----

**実装状況**: 14/14関数実装、14/14関数テスト済み ✅

AEGP_ProjSuite6は以下の機能を提供します:

- プロジェクトの取得、作成、開く、保存
- プロジェクト情報（名前、パス、編集状態）の取得
- ビット深度の設定
- 時間表示設定の変更

基本概念
--------

プロジェクトハンドル (AEGP_ProjectH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのプロジェクトを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - プロジェクトハンドルは、プロジェクトが開かれている間のみ有効です
   - プロジェクトを閉じた後は、ハンドルは無効になります
   - 複数プロジェクトを開いている場合、各プロジェクトに固有のハンドルがあります

Undoable操作
~~~~~~~~~~~~

以下の操作は **Undo可能** です（After EffectsのUndo履歴に記録されます）:

- ``AEGP_SetProjectTimeDisplay`` - 時間表示設定
- ``AEGP_SetProjectBitDepth`` - ビット深度設定

.. tip::
   複数の変更を行う場合は、``AEGP_StartUndoGroup()`` と ``AEGP_EndUndoGroup()`` でグループ化することを推奨します。

API リファレンス
----------------

プロジェクト取得
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNumProjects() -> int

   開いているプロジェクトの数を取得します。

   :return: 開いているプロジェクト数
   :rtype: int

   **例**:

   .. code-block:: python

      num_projects = ae.sdk.AEGP_GetNumProjects()
      print(f"開いているプロジェクト数: {num_projects}")

.. function:: AEGP_GetProjectByIndex(index: int) -> int

   インデックスからプロジェクトハンドルを取得します。

   :param index: プロジェクトのインデックス（0から始まる）
   :type index: int
   :return: プロジェクトハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      projH = ae.sdk.AEGP_GetProjectByIndex(0)  # 最初のプロジェクト

プロジェクト情報
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetProjectName(projH: int) -> str

   プロジェクトの名前を取得します。

   :param projH: プロジェクトハンドル
   :type projH: int
   :return: プロジェクト名（UTF-8文字列）
   :rtype: str

   .. note::
      内部的にShift-JISからUTF-8に自動変換されます。日本語のプロジェクト名も正しく取得できます。

   **例**:

   .. code-block:: python

      name = ae.sdk.AEGP_GetProjectName(projH)
      print(f"プロジェクト名: {name}")

.. function:: AEGP_GetProjectPath(plugin_id: int, projH: int) -> str

   プロジェクトファイルのパスを取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param projH: プロジェクトハンドル
   :type projH: int
   :return: プロジェクトファイルのパス（UTF-8文字列）。未保存の場合は空文字列。
   :rtype: str

   .. note::
      ``plugin_id`` はメモリ管理のために必要です。``ae.sdk.AEGP_GetPluginID()`` で取得してください。
      この関数は1度だけ呼び出し、結果を変数に保存して再利用することを推奨します。

   **例**:

   .. code-block:: python

      # プラグインIDを取得（1度だけ）
      plugin_id = ae.sdk.AEGP_GetPluginID()

      # プロジェクトパスを取得
      path = ae.sdk.AEGP_GetProjectPath(plugin_id, projH)
      if path:
          print(f"プロジェクトパス: {path}")
      else:
          print("プロジェクトは未保存です")

.. function:: AEGP_GetProjectRootFolder(projH: int) -> int

   プロジェクトのルートフォルダアイテムハンドルを取得します。

   :param projH: プロジェクトハンドル
   :type projH: int
   :return: ルートフォルダのアイテムハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

.. function:: AEGP_ProjectIsDirty(projH: int) -> bool

   プロジェクトが編集されているか（未保存の変更があるか）を確認します。

   :param projH: プロジェクトハンドル
   :type projH: int
   :return: 編集されている場合は ``True``、変更がない場合は ``False``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_ProjectIsDirty(projH):
          print("プロジェクトは編集されています")

プロジェクトの保存
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SaveProjectToPath(projH: int, path: str) -> None

   プロジェクトを指定パスに保存します。

   :param projH: プロジェクトハンドル
   :type projH: int
   :param path: 保存先のパス（空文字列の場合は現在のパスに保存）
   :type path: str

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SaveProjectToPath(projH, r"C:\Projects\MyProject.aep")

.. function:: AEGP_SaveProjectAs(projH: int, path: str) -> None

   プロジェクトを新しいパスに保存し、プロジェクトのパスを変更します（別名で保存）。

   :param projH: プロジェクトハンドル
   :type projH: int
   :param path: 保存先の新しいパス
   :type path: str

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SaveProjectAs(projH, r"C:\Projects\MyProject_v2.aep")

プロジェクトの作成・開く
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NewProject() -> int

   新しいプロジェクトを作成します。

   :return: 新しいプロジェクトハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      new_projH = ae.sdk.AEGP_NewProject()

.. function:: AEGP_OpenProjectFromPath(path: str) -> int

   プロジェクトファイルを開きます。

   :param path: 開くプロジェクトファイルのパス
   :type path: str
   :return: 開いたプロジェクトハンドル
   :rtype: int

   .. warning::
      この関数は **現在開いているすべてのプロジェクトを閉じます**。
      未保存の変更がある場合、データが失われる可能性があります。

   **例**:

   .. code-block:: python

      projH = ae.sdk.AEGP_OpenProjectFromPath(r"C:\Projects\MyProject.aep")

プロジェクト設定
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetProjectBitDepth(projH: int) -> int

   プロジェクトのビット深度を取得します。

   :param projH: プロジェクトハンドル
   :type projH: int
   :return: ビット深度（8, 16, 32）
   :rtype: int

   **例**:

   .. code-block:: python

      bit_depth = ae.sdk.AEGP_GetProjectBitDepth(projH)
      print(f"ビット深度: {bit_depth}ビット")

.. function:: AEGP_SetProjectBitDepth(projH: int, bit_depth: int) -> None

   プロジェクトのビット深度を設定します（**Undo可能**）。

   :param projH: プロジェクトハンドル
   :type projH: int
   :param bit_depth: ビット深度（8, 16, 32）
   :type bit_depth: int

   **ビット深度の選択肢**:

   - ``8``: 8ビット/チャンネル（標準、ファイルサイズ小）
   - ``16``: 16ビット/チャンネル（高品質、中間ファイルサイズ）
   - ``32``: 32ビット/チャンネル（最高品質、浮動小数点、ファイルサイズ大）

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetProjectBitDepth(projH, 16)  # 16ビットに設定

.. function:: AEGP_GetProjectTimeDisplay(projH: int) -> dict

   プロジェクトの時間表示設定を取得します。

   :param projH: プロジェクトハンドル
   :type projH: int
   :return: 時間表示設定の辞書
   :rtype: dict

   **戻り値の構造** (``AEGP_TimeDisplay3``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``display_mode``
        - int
        - 時間表示モード
      * - ``footage_display_mode``
        - int
        - フッテージのタイムコード表示モード
      * - ``display_dropframe``
        - bool
        - ドロップフレーム表示を使用するか
      * - ``use_feet_frames``
        - bool
        - フィート+フレーム表示を使用するか
      * - ``timebase``
        - int
        - タイムベース
      * - ``frames_per_foot``
        - int
        - 1フィートあたりのフレーム数
      * - ``frames_display_mode``
        - int
        - フレーム番号の表示モード

   **例**:

   .. code-block:: python

      time_display = ae.sdk.AEGP_GetProjectTimeDisplay(projH)
      print(f"表示モード: {time_display['display_mode']}")

.. function:: AEGP_SetProjectTimeDisplay(projH: int, time_display: dict) -> None

   プロジェクトの時間表示設定を変更します（**Undo可能**）。

   :param projH: プロジェクトハンドル
   :type projH: int
   :param time_display: 時間表示設定の辞書（すべてのキーを含む必要があります）
   :type time_display: dict

   .. note::
      すべてのキーを含む完全な辞書を渡す必要があります。

   **例**:

   .. code-block:: python

      time_display = ae.sdk.AEGP_GetProjectTimeDisplay(projH)
      time_display['display_dropframe'] = True
      ae.sdk.AEGP_SetProjectTimeDisplay(projH, time_display)

使用例
------

プロジェクト情報のダンプ
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_project_info():
       """現在のプロジェクトの詳細情報を表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       # 基本情報
       name = ae.sdk.AEGP_GetProjectName(projH)
       path = ae.sdk.AEGP_GetProjectPath(plugin_id, projH)
       is_dirty = ae.sdk.AEGP_ProjectIsDirty(projH)
       bit_depth = ae.sdk.AEGP_GetProjectBitDepth(projH)

       print("=" * 50)
       print(f"プロジェクト名: {name}")
       print(f"パス: {path if path else '(未保存)'}")
       print(f"編集状態: {'変更あり' if is_dirty else '変更なし'}")
       print(f"ビット深度: {bit_depth}ビット")

       # 時間表示設定
       time_display = ae.sdk.AEGP_GetProjectTimeDisplay(projH)
       print(f"\n時間表示設定:")
       print(f"  表示モード: {time_display['display_mode']}")
       print(f"  ドロップフレーム: {time_display['display_dropframe']}")
       print("=" * 50)

   # 実行
   dump_project_info()

プロジェクトの自動保存
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import os
   from datetime import datetime

   def auto_save_project(backup_dir=r"C:\ProjectBackups"):
       """プロジェクトを自動バックアップ"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       # 編集されている場合のみバックアップ
       if not ae.sdk.AEGP_ProjectIsDirty(projH):
           print("変更がないためバックアップをスキップしました")
           return

       # 現在のパスを取得
       current_path = ae.sdk.AEGP_GetProjectPath(plugin_id, projH)

       if not current_path:
           print("未保存のプロジェクトはバックアップできません")
           return

       # バックアップディレクトリを作成
       os.makedirs(backup_dir, exist_ok=True)

       # バックアップファイル名（タイムスタンプ付き）
       timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
       project_name = os.path.splitext(os.path.basename(current_path))[0]
       backup_filename = f"{project_name}_backup_{timestamp}.aep"
       backup_path = os.path.join(backup_dir, backup_filename)

       # バックアップ保存
       ae.sdk.AEGP_SaveProjectToPath(projH, backup_path)
       print(f"バックアップを保存しました: {backup_path}")

       # オリジナルも保存
       ae.sdk.AEGP_SaveProjectToPath(projH, current_path)
       print(f"オリジナルを保存しました: {current_path}")

   # 実行
   auto_save_project()

プロジェクト設定の一括変更
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_project_for_high_quality():
       """プロジェクトを高品質設定に変更"""
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       # Undoグループ開始
       ae.sdk.AEGP_StartUndoGroup("プロジェクト設定変更")

       try:
           # 32ビット浮動小数点に変更
           current_depth = ae.sdk.AEGP_GetProjectBitDepth(projH)
           if current_depth != 32:
               ae.sdk.AEGP_SetProjectBitDepth(projH, 32)
               print("ビット深度を32ビットに変更しました")

           # 時間表示設定を調整
           time_display = ae.sdk.AEGP_GetProjectTimeDisplay(projH)
           time_display['display_dropframe'] = False  # ドロップフレーム無効
           ae.sdk.AEGP_SetProjectTimeDisplay(projH, time_display)
           print("時間表示設定を変更しました")

           print("プロジェクトを高品質設定に変更しました")

       finally:
           # Undoグループ終了
           ae.sdk.AEGP_EndUndoGroup()

   # 実行
   setup_project_for_high_quality()

安全なプロジェクトオープン
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def safe_open_project(new_project_path):
       """既存のプロジェクトを保存してから新しいプロジェクトを開く"""
       # 現在のプロジェクトを確認
       num_projects = ae.sdk.AEGP_GetNumProjects()

       if num_projects > 0:
           projH = ae.sdk.AEGP_GetProjectByIndex(0)

           # 編集されている場合は保存
           if ae.sdk.AEGP_ProjectIsDirty(projH):
               plugin_id = ae.sdk.AEGP_GetPluginID()
               current_path = ae.sdk.AEGP_GetProjectPath(plugin_id, projH)

               if current_path:
                   ae.sdk.AEGP_SaveProjectToPath(projH, current_path)
                   print(f"現在のプロジェクトを保存しました: {current_path}")
               else:
                   print("警告: 現在のプロジェクトは未保存です")

       # 新しいプロジェクトを開く
       new_projH = ae.sdk.AEGP_OpenProjectFromPath(new_project_path)
       print(f"新しいプロジェクトを開きました: {new_project_path}")
       return new_projH

   # 使用例
   safe_open_project(r"C:\Projects\MyProject.aep")

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **AEGP_OpenProjectFromPathは既存プロジェクトを閉じます**

   実行前に必ず保存状態を確認してください。

2. **プロジェクトハンドルの有効期限**

   プロジェクトハンドルは、プロジェクトが開かれている間のみ有効です。

3. **Undo操作**

   ``AEGP_SetProjectBitDepth`` と ``AEGP_SetProjectTimeDisplay`` はUndo可能です。
   複数の変更を行う場合は、``AEGP_StartUndoGroup()`` でグループ化してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

編集状態の確認
^^^^^^^^^^^^^^

.. code-block:: python

   if ae.sdk.AEGP_ProjectIsDirty(projH):
       # 保存処理

パスの存在確認
^^^^^^^^^^^^^^

.. code-block:: python

   path = ae.sdk.AEGP_GetProjectPath(plugin_id, projH)
   if not path:
       print("プロジェクトは未保存です")

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("処理名")
   try:
       # 複数の変更
   finally:
       ae.sdk.AEGP_EndUndoGroup()

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       projH = ae.sdk.AEGP_OpenProjectFromPath(path)
   except Exception as e:
       print(f"プロジェクトを開けませんでした: {e}")

関連項目
--------

- :doc:`AEGP_ItemSuite9` - プロジェクトアイテム管理
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`AEGP_UtilitySuite6` - Undo管理、エラー処理
- :doc:`index` - 低レベルAPI概要
