AEGP_CommandSuite1
==================

.. currentmodule:: ae.sdk

AEGP_CommandSuite1は、After Effectsのメニューコマンドの作成、管理、実行を行うためのSDK APIです。

概要
----

**実装状況**: 8/8関数実装 ✅

AEGP_CommandSuite1は以下の機能を提供します:

- カスタムメニューコマンドの作成と登録
- メニューコマンドの有効化/無効化
- メニュー項目の名前変更とチェックマーク設定
- プログラムによるコマンド実行

基本概念
--------

コマンドID (AEGP_Command)
~~~~~~~~~~~~~~~~~~~~~~~~~~

メニューコマンドを識別するための一意のID。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - コマンドIDは ``AEGP_GetUniqueCommand()`` で取得する必要があります
   - 同じコマンドIDを複数のメニュー項目に使用しないでください
   - コマンドIDはプラグインの存続期間中、保持しておく必要があります

メニューID (AEGP_MenuID)
~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsの組み込みメニューを識別するためのID。以下の値が使用できます:

.. list-table:: メニューID一覧
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_Menu_NONE``
     - 0
     - メニューなし
   * - ``AEGP_Menu_APPLE``
     - 1
     - Appleメニュー（macOSのみ）
   * - ``AEGP_Menu_FILE``
     - 2
     - ファイルメニュー
   * - ``AEGP_Menu_EDIT``
     - 3
     - 編集メニュー
   * - ``AEGP_Menu_COMPOSITION``
     - 4
     - コンポジションメニュー
   * - ``AEGP_Menu_LAYER``
     - 5
     - レイヤーメニュー
   * - ``AEGP_Menu_EFFECT``
     - 6
     - エフェクトメニュー
   * - ``AEGP_Menu_WINDOW``
     - 7
     - ウィンドウメニュー
   * - ``AEGP_Menu_FLOATERS``
     - 8
     - フローターメニュー
   * - ``AEGP_Menu_KF_ASSIST``
     - 9
     - キーフレームアシスタントメニュー
   * - ``AEGP_Menu_IMPORT``
     - 10
     - インポートメニュー
   * - ``AEGP_Menu_SAVE_FRAME_AS``
     - 11
     - フレームを保存メニュー
   * - ``AEGP_Menu_PREFS``
     - 12
     - 環境設定メニュー
   * - ``AEGP_Menu_EXPORT``
     - 13
     - エクスポートメニュー
   * - ``AEGP_Menu_ANIMATION``
     - 14
     - アニメーションメニュー
   * - ``AEGP_Menu_PURGE``
     - 15
     - 消去メニュー
   * - ``AEGP_Menu_NEW``
     - 16
     - 新規メニュー

挿入位置の指定
~~~~~~~~~~~~~~

``AEGP_InsertMenuCommand()`` の ``after_item`` パラメータで、メニュー項目の挿入位置を指定できます:

- ``-1``: メニューの最後に追加
- ``0以上``: 指定した位置の後に挿入（0が最初の項目）

API リファレンス
----------------

コマンドIDの取得
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetUniqueCommand() -> int

   新しいコマンドIDを取得します。

   :return: 一意のコマンドID
   :rtype: int

   .. important::
      メニューコマンドを作成する前に、必ずこの関数でコマンドIDを取得してください。

   **例**:

   .. code-block:: python

      command_id = ae.sdk.AEGP_GetUniqueCommand()
      print(f"コマンドID: {command_id}")

メニューコマンドの管理
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_InsertMenuCommand(command: int, name: str, menu_id: int, after_item: int) -> None

   メニューコマンドを指定のメニューに追加します。

   :param command: コマンドID（``AEGP_GetUniqueCommand()`` で取得）
   :type command: int
   :param name: メニュー項目の表示名
   :type name: str
   :param menu_id: 追加先のメニューID（``AEGP_Menu_*`` 定数）
   :type menu_id: int
   :param after_item: 挿入位置（-1でメニューの最後、0以上で指定位置の後）
   :type after_item: int

   .. note::
      - メニュー項目が追加されますが、コマンドの実行処理は別途実装する必要があります
      - 通常、Command Hookを使用してコマンドの実行を処理します

   **例**:

   .. code-block:: python

      # ウィンドウメニューの最後に追加
      command_id = ae.sdk.AEGP_GetUniqueCommand()
      ae.sdk.AEGP_InsertMenuCommand(
          command_id,
          "My Custom Window",
          ae.sdk.AEGP_Menu_WINDOW,  # ウィンドウメニュー
          -1  # メニューの最後
      )

.. function:: AEGP_RemoveMenuCommand(command: int) -> None

   メニューからコマンドを削除します。

   :param command: 削除するコマンドID
   :type command: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_RemoveMenuCommand(command_id)

.. function:: AEGP_SetMenuCommandName(command: int, name: str) -> None

   メニューコマンドの表示名を変更します。

   :param command: コマンドID
   :type command: int
   :param name: 新しい表示名
   :type name: str

   .. tip::
      この関数を使用して、メニュー項目の名前を動的に変更できます（例: "Show Panel" ↔ "Hide Panel"）。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetMenuCommandName(command_id, "Updated Command Name")

メニューコマンドの状態管理
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_EnableCommand(command: int) -> None

   コマンドを有効化します（メニュー項目がクリック可能になります）。

   :param command: コマンドID
   :type command: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_EnableCommand(command_id)

.. function:: AEGP_DisableCommand(command: int) -> None

   コマンドを無効化します（メニュー項目がグレーアウトされます）。

   :param command: コマンドID
   :type command: int

   .. note::
      無効化されたコマンドは、ユーザーがクリックしても実行されません。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DisableCommand(command_id)

.. function:: AEGP_CheckMarkMenuCommand(command: int, check: bool) -> None

   メニューコマンドのチェックマークを設定/解除します。

   :param command: コマンドID
   :type command: int
   :param check: チェックマークを表示する場合は ``True``、非表示にする場合は ``False``
   :type check: bool

   .. tip::
      トグル機能（ON/OFF切り替え）を実装する際に便利です。

   **例**:

   .. code-block:: python

      # チェックマークを表示
      ae.sdk.AEGP_CheckMarkMenuCommand(command_id, True)

      # チェックマークを非表示
      ae.sdk.AEGP_CheckMarkMenuCommand(command_id, False)

コマンドの実行
~~~~~~~~~~~~~~

.. function:: AEGP_DoCommand(command: int) -> None

   指定したコマンドをプログラムから実行します。

   :param command: 実行するコマンドID
   :type command: int

   .. warning::
      この関数は、登録済みのコマンドハンドラを呼び出します。
      コマンドハンドラが実装されていない場合、エラーになる可能性があります。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DoCommand(command_id)

使用例
------

基本的なメニューコマンドの追加
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # コマンドIDを取得
   command_id = ae.sdk.AEGP_GetUniqueCommand()

   # ウィンドウメニューに追加
   ae.sdk.AEGP_InsertMenuCommand(
       command_id,
       "My Custom Tool",
       7,  # AEGP_Menu_WINDOW
       -1  # メニューの最後
   )

   print(f"メニューコマンドを追加しました（ID: {command_id}）")

トグル機能付きメニュー
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   class ToggleMenuCommand:
       def __init__(self):
           self.command_id = ae.sdk.AEGP_GetUniqueCommand()
           self.is_checked = False

           # メニューに追加
           ae.sdk.AEGP_InsertMenuCommand(
               self.command_id,
               "Toggle Feature",
               7,  # AEGP_Menu_WINDOW
               -1
           )

       def toggle(self):
           """トグル処理"""
           self.is_checked = not self.is_checked
           ae.sdk.AEGP_CheckMarkMenuCommand(self.command_id, self.is_checked)

           if self.is_checked:
               print("Feature enabled")
           else:
               print("Feature disabled")

   # 使用例
   toggle_cmd = ToggleMenuCommand()
   toggle_cmd.toggle()  # ON
   toggle_cmd.toggle()  # OFF

動的なメニュー項目の名前変更
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   class DynamicMenuCommand:
       def __init__(self):
           self.command_id = ae.sdk.AEGP_GetUniqueCommand()
           self.is_visible = False

           ae.sdk.AEGP_InsertMenuCommand(
               self.command_id,
               "Show Panel",
               7,  # AEGP_Menu_WINDOW
               -1
           )

       def toggle_panel(self):
           """パネルの表示/非表示を切り替え"""
           self.is_visible = not self.is_visible

           if self.is_visible:
               ae.sdk.AEGP_SetMenuCommandName(self.command_id, "Hide Panel")
               print("Panel shown")
           else:
               ae.sdk.AEGP_SetMenuCommandName(self.command_id, "Show Panel")
               print("Panel hidden")

   # 使用例
   panel_cmd = DynamicMenuCommand()
   panel_cmd.toggle_panel()

条件付きメニュー有効化
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def update_menu_state():
       """コンポジションが選択されている場合のみコマンドを有効化"""
       command_id = ae.sdk.AEGP_GetUniqueCommand()

       # メニューに追加
       ae.sdk.AEGP_InsertMenuCommand(
           command_id,
           "Process Composition",
           4,  # AEGP_Menu_COMPOSITION
           -1
       )

       # コンポジションの有無を確認
       try:
           projH = ae.sdk.AEGP_GetProjectByIndex(0)
           num_items = ae.sdk.AEGP_GetNumItemsInFolder(projH, 0)

           if num_items > 0:
               ae.sdk.AEGP_EnableCommand(command_id)
               print("Command enabled")
           else:
               ae.sdk.AEGP_DisableCommand(command_id)
               print("Command disabled (no compositions)")
       except:
           ae.sdk.AEGP_DisableCommand(command_id)

   # 実行
   update_menu_state()

複数メニューへの追加
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def add_commands_to_multiple_menus():
       """複数のメニューにコマンドを追加"""

       # ファイルメニュー
       file_cmd = ae.sdk.AEGP_GetUniqueCommand()
       ae.sdk.AEGP_InsertMenuCommand(
           file_cmd,
           "Export Custom Format",
           2,  # AEGP_Menu_FILE
           -1
       )

       # 編集メニュー
       edit_cmd = ae.sdk.AEGP_GetUniqueCommand()
       ae.sdk.AEGP_InsertMenuCommand(
           edit_cmd,
           "Custom Paste",
           3,  # AEGP_Menu_EDIT
           -1
       )

       # レイヤーメニュー
       layer_cmd = ae.sdk.AEGP_GetUniqueCommand()
       ae.sdk.AEGP_InsertMenuCommand(
           layer_cmd,
           "Apply Custom Effect",
           5,  # AEGP_Menu_LAYER
           -1
       )

       print(f"追加されたコマンド:")
       print(f"  ファイル: {file_cmd}")
       print(f"  編集: {edit_cmd}")
       print(f"  レイヤー: {layer_cmd}")

   # 実行
   add_commands_to_multiple_menus()

メニュー項目の動的管理
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   class MenuManager:
       def __init__(self):
           self.commands = {}

       def add_command(self, name, menu_id, position=-1):
           """コマンドを追加して管理"""
           command_id = ae.sdk.AEGP_GetUniqueCommand()
           ae.sdk.AEGP_InsertMenuCommand(command_id, name, menu_id, position)

           self.commands[name] = {
               'id': command_id,
               'enabled': True,
               'checked': False
           }

           return command_id

       def remove_command(self, name):
           """コマンドを削除"""
           if name in self.commands:
               ae.sdk.AEGP_RemoveMenuCommand(self.commands[name]['id'])
               del self.commands[name]

       def set_enabled(self, name, enabled):
           """有効/無効を切り替え"""
           if name in self.commands:
               if enabled:
                   ae.sdk.AEGP_EnableCommand(self.commands[name]['id'])
               else:
                   ae.sdk.AEGP_DisableCommand(self.commands[name]['id'])
               self.commands[name]['enabled'] = enabled

       def set_checked(self, name, checked):
           """チェックマークを設定"""
           if name in self.commands:
               ae.sdk.AEGP_CheckMarkMenuCommand(
                   self.commands[name]['id'],
                   checked
               )
               self.commands[name]['checked'] = checked

       def list_commands(self):
           """登録されているコマンドを一覧表示"""
           print("Registered commands:")
           for name, info in self.commands.items():
               status = []
               if not info['enabled']:
                   status.append("disabled")
               if info['checked']:
                   status.append("checked")

               status_str = f" ({', '.join(status)})" if status else ""
               print(f"  {name} [ID: {info['id']}]{status_str}")

   # 使用例
   menu_mgr = MenuManager()

   # コマンドを追加
   menu_mgr.add_command("Tool 1", 7, -1)  # ウィンドウメニュー
   menu_mgr.add_command("Tool 2", 7, -1)
   menu_mgr.add_command("Tool 3", 7, -1)

   # 状態を変更
   menu_mgr.set_enabled("Tool 2", False)
   menu_mgr.set_checked("Tool 3", True)

   # 一覧表示
   menu_mgr.list_commands()

   # コマンドを削除
   menu_mgr.remove_command("Tool 2")

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **コマンドIDの管理**

   ``AEGP_GetUniqueCommand()`` で取得したコマンドIDは、プラグインの存続期間中、保持しておく必要があります。

2. **コマンドハンドラの実装**

   メニュー項目を追加しただけでは、クリックしても何も起こりません。Command Hookを使用して、コマンドの実行処理を実装する必要があります。

3. **メニューIDの有効性**

   一部のメニューIDは、プラットフォーム（Windows/macOS）によって利用できない場合があります。

4. **スレッド安全性**

   メニュー操作は、メインスレッドから実行する必要があります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

コマンドIDの保存
^^^^^^^^^^^^^^^^

.. code-block:: python

   # 良い例：クラスで管理
   class MyPlugin:
       def __init__(self):
           self.command_id = ae.sdk.AEGP_GetUniqueCommand()

   # 悪い例：毎回取得
   def bad_example():
       command_id = ae.sdk.AEGP_GetUniqueCommand()  # 新しいIDが毎回生成される

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       command_id = ae.sdk.AEGP_GetUniqueCommand()
       ae.sdk.AEGP_InsertMenuCommand(command_id, "My Command", 7, -1)
   except Exception as e:
       print(f"メニューコマンドの追加に失敗: {e}")

メニュー項目の整理
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # セパレータを使用してメニューをグループ化
   # （注: セパレータの追加は別のAPIで行います）

   # 関連するコマンドをまとめて追加
   base_position = 0
   ae.sdk.AEGP_InsertMenuCommand(cmd1, "Tool A", 7, base_position)
   ae.sdk.AEGP_InsertMenuCommand(cmd2, "Tool B", 7, base_position + 1)
   ae.sdk.AEGP_InsertMenuCommand(cmd3, "Tool C", 7, base_position + 2)

状態の同期
^^^^^^^^^^

.. code-block:: python

   class StatefulCommand:
       def __init__(self):
           self.command_id = ae.sdk.AEGP_GetUniqueCommand()
           self._state = False

       @property
       def state(self):
           return self._state

       @state.setter
       def state(self, value):
           self._state = value
           # UIとの同期
           ae.sdk.AEGP_CheckMarkMenuCommand(self.command_id, value)

制限事項
~~~~~~~~

1. **コマンドハンドラ未実装**

   現在のPyAEバインディングでは、Command Hook（コマンドの実行処理）の登録はサポートされていません。
   メニュー項目の追加・管理のみ可能です。

2. **組み込みコマンドの実行**

   ``AEGP_DoCommand()`` は、カスタムコマンドだけでなく、After Effectsの組み込みコマンドも実行できますが、
   組み込みコマンドIDのリストは、現在のバージョンでは提供されていません。

関連項目
--------

- :doc:`AEGP_UtilitySuite6` - ユーティリティ関数
- :doc:`index` - 低レベルAPI概要
