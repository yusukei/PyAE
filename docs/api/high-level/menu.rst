メニュー・コマンド
==================

.. currentmodule:: ae.menu

After Effectsのメニューにカスタムコマンドを追加し、Pythonコールバックで処理するためのAPIです。

概要
----

``ae.menu`` モジュールは、After Effectsのメニューにカスタムコマンドを追加し、
クリック時にPythonコールバックを実行するための高レベルAPIを提供します。

**主な機能**:

- カスタムメニュー項目の追加
- Pythonコールバックによるクリック処理
- メニューの有効/無効、チェックマークの動的制御
- メニュー更新コールバックによる状態管理

.. note::
   このAPIはPyAE独自の高レベルAPIです。低レベルの SDK 関数
   （``ae.sdk.AEGP_InsertMenuCommand`` 等）と組み合わせて使用することもできます。

基本的な使い方
--------------

コールバック付きメニュー追加
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def my_script():
       """メニュークリック時に実行される処理"""
       print("My script is running!")
       comp = ae.get_active_item()
       if comp:
           print(f"Active comp: {comp.name}")

   # ウィンドウメニューにコマンドを追加（コールバック付き）
   cmd = ae.menu.register_command("My Script", ae.menu.WINDOW, my_script)

コールバックなしでの追加（後から設定）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # メニューに追加（コールバックなし）
   cmd = ae.menu.register_command("My Tool", ae.menu.WINDOW)

   # 後からコールバックを設定
   def on_click():
       print("Tool activated!")

   cmd.set_on_click(on_click)

   # コールバックを解除
   cmd.set_on_click(None)

API リファレンス
----------------

モジュール関数
~~~~~~~~~~~~~~

.. function:: register_command(name: str, menu_id: int, callback: Optional[Callable[[], None]] = None, after_item: int = -1) -> Command

   メニューにカスタムコマンドを追加します。

   :param name: メニュー項目の表示名
   :type name: str
   :param menu_id: 追加先メニューのID（下記のメニューID定数を参照）
   :type menu_id: int
   :param callback: クリック時に呼び出される関数（省略可）
   :type callback: Optional[Callable[[], None]]
   :param after_item: 挿入位置（-1 = 末尾、0 = 先頭）
   :type after_item: int
   :return: 作成されたコマンドオブジェクト
   :rtype: Command

   **例**:

   .. code-block:: python

      # コールバック付きで追加
      cmd = ae.menu.register_command("Process", ae.menu.WINDOW, lambda: print("Done!"))

      # コールバックなしで追加（後から設定）
      cmd = ae.menu.register_command("My Tool", ae.menu.COMPOSITION)

.. function:: add_to_window(name: str, callback: Optional[Callable] = None, after_item: int = -1) -> Command

   ウィンドウメニュー（Window）にコマンドを追加します。

.. function:: add_to_file(name: str, callback: Optional[Callable] = None, after_item: int = -1) -> Command

   ファイルメニュー（File）にコマンドを追加します。

.. function:: add_to_edit(name: str, callback: Optional[Callable] = None, after_item: int = -1) -> Command

   編集メニュー（Edit）にコマンドを追加します。

.. function:: add_to_composition(name: str, callback: Optional[Callable] = None, after_item: int = -1) -> Command

   コンポジションメニュー（Composition）にコマンドを追加します。

.. function:: add_to_layer(name: str, callback: Optional[Callable] = None, after_item: int = -1) -> Command

   レイヤーメニュー（Layer）にコマンドを追加します。

.. function:: add_to_effect(name: str, callback: Optional[Callable] = None, after_item: int = -1) -> Command

   エフェクトメニュー（Effect）にコマンドを追加します。

.. function:: add_to_animation(name: str, callback: Optional[Callable] = None, after_item: int = -1) -> Command

   アニメーションメニュー（Animation）にコマンドを追加します。

.. function:: execute(command_id: int) -> None

   指定されたIDのコマンドを実行します。

   :param command_id: コマンドID
   :type command_id: int

.. function:: get_unique_id() -> int

   一意のコマンドIDを取得します。

   :return: 新しいコマンドID
   :rtype: int

.. function:: clear_all_callbacks() -> None

   すべての登録済みコールバックをクリアします。

.. function:: get_registered_command_ids() -> List[int]

   コールバックが登録されているコマンドIDのリストを取得します。

   :return: コマンドIDのリスト
   :rtype: List[int]

Command クラス
~~~~~~~~~~~~~~

.. class:: Command

   メニューコマンドを表すクラス。

   .. attribute:: name
      :type: str

      メニュー項目の表示名。読み書き可能。

   .. attribute:: enabled
      :type: bool

      コマンドが有効かどうか。無効の場合はグレーアウトされます。

   .. attribute:: checked
      :type: bool

      チェックマークの表示状態。

   .. attribute:: command_id
      :type: int

      コマンドの一意ID（読み取り専用）。

   .. attribute:: menu_id
      :type: int

      コマンドが配置されているメニューのID（読み取り専用）。

   .. method:: set_on_click(callback: Optional[Callable[[], None]]) -> None

      クリック時のコールバックを設定します。

      :param callback: コールバック関数、または ``None`` で解除

      **例**:

      .. code-block:: python

         def handler():
             print("Clicked!")

         cmd.set_on_click(handler)
         cmd.set_on_click(None)  # 解除

   .. method:: set_on_update(callback: Optional[Callable[[], bool]]) -> None

      メニュー更新時のコールバックを設定します。
      このコールバックはメニューが開かれるたびに呼び出され、
      戻り値でコマンドの有効/無効を制御できます。

      :param callback: ``bool`` を返すコールバック関数（True=有効、False=無効）

      **例**:

      .. code-block:: python

         def should_enable():
             # アクティブなコンポジションがある場合のみ有効
             return ae.get_active_item() is not None

         cmd.set_on_update(should_enable)

   .. method:: execute() -> None

      コマンドをプログラムから実行します。

   .. method:: remove() -> None

      メニューからコマンドを削除し、関連するコールバックも解除します。

メニューID定数
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 30 10 60

   * - 定数
     - 値
     - 説明
   * - ``ae.menu.NONE``
     - 0
     - メニューなし
   * - ``ae.menu.APPLE``
     - 1
     - Appleメニュー（macOSのみ）
   * - ``ae.menu.FILE``
     - 2
     - ファイルメニュー
   * - ``ae.menu.EDIT``
     - 3
     - 編集メニュー
   * - ``ae.menu.COMPOSITION``
     - 4
     - コンポジションメニュー
   * - ``ae.menu.LAYER``
     - 5
     - レイヤーメニュー
   * - ``ae.menu.EFFECT``
     - 6
     - エフェクトメニュー
   * - ``ae.menu.WINDOW``
     - 7
     - ウィンドウメニュー
   * - ``ae.menu.FLOATERS``
     - 8
     - フローターメニュー
   * - ``ae.menu.KF_ASSIST``
     - 9
     - キーフレームアシスタント
   * - ``ae.menu.IMPORT``
     - 10
     - インポートサブメニュー
   * - ``ae.menu.SAVE_FRAME_AS``
     - 11
     - フレームを保存サブメニュー
   * - ``ae.menu.PREFS``
     - 12
     - 環境設定サブメニュー
   * - ``ae.menu.EXPORT``
     - 13
     - エクスポートサブメニュー
   * - ``ae.menu.ANIMATION``
     - 14
     - アニメーションメニュー
   * - ``ae.menu.PURGE``
     - 15
     - 消去サブメニュー
   * - ``ae.menu.NEW``
     - 16
     - 新規サブメニュー

実用例
------

ツールバー風メニュー
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   class MyToolbar:
       """複数のツールを管理するクラス"""

       def __init__(self):
           self.commands = {}
           self.current_tool = None

       def add_tool(self, name: str, action):
           """ツールを追加"""
           def on_click():
               self.select_tool(name)
               action()

           cmd = ae.menu.add_to_window(f"[Tool] {name}", on_click)
           self.commands[name] = cmd

       def select_tool(self, name: str):
           """ツールを選択（チェックマーク表示）"""
           for tool_name, cmd in self.commands.items():
               cmd.checked = (tool_name == name)
           self.current_tool = name

       def cleanup(self):
           """すべてのツールをメニューから削除"""
           for cmd in self.commands.values():
               cmd.remove()
           self.commands.clear()

   # 使用例
   toolbar = MyToolbar()
   toolbar.add_tool("Move", lambda: print("Move tool"))
   toolbar.add_tool("Scale", lambda: print("Scale tool"))
   toolbar.add_tool("Rotate", lambda: print("Rotate tool"))

   toolbar.select_tool("Move")  # Move にチェックマーク

コンテキスト依存メニュー
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def process_selected_layers():
       """選択レイヤーを処理"""
       comp = ae.get_active_item()
       if not comp:
           return

       layers = comp.selected_layers
       print(f"Processing {len(layers)} layers...")
       for layer in layers:
           # 何らかの処理
           print(f"  - {layer.name}")

   def has_selected_layers():
       """選択レイヤーがあるかチェック（メニュー有効化条件）"""
       comp = ae.get_active_item()
       if not comp:
           return False
       return len(comp.selected_layers) > 0

   # メニューを追加
   cmd = ae.menu.add_to_layer("Process Selected Layers", process_selected_layers)

   # 選択レイヤーがある場合のみ有効化
   cmd.set_on_update(has_selected_layers)

トグル機能
~~~~~~~~~~

.. code-block:: python

   import ae

   class FeatureToggle:
       """機能のON/OFFをトグルするメニュー"""

       def __init__(self, name: str, menu_id: int = ae.menu.WINDOW):
           self.name = name
           self.is_active = False
           self.cmd = ae.menu.register_command(
               f"{name} [OFF]",
               menu_id,
               self.toggle
           )

       def toggle(self):
           """状態をトグル"""
           self.is_active = not self.is_active

           # メニュー名とチェックマークを更新
           if self.is_active:
               self.cmd.name = f"{self.name} [ON]"
               self.cmd.checked = True
               self.on_activate()
           else:
               self.cmd.name = f"{self.name} [OFF]"
               self.cmd.checked = False
               self.on_deactivate()

       def on_activate(self):
           """機能有効化時の処理（オーバーライド用）"""
           print(f"{self.name} activated")

       def on_deactivate(self):
           """機能無効化時の処理（オーバーライド用）"""
           print(f"{self.name} deactivated")

       def cleanup(self):
           """クリーンアップ"""
           self.cmd.remove()

   # 使用例
   auto_save = FeatureToggle("Auto Save")
   # メニュークリックでON/OFFが切り替わる

スクリプトランチャー
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import os

   class ScriptLauncher:
       """スクリプトファイルをメニューから実行"""

       def __init__(self, scripts_dir: str):
           self.scripts_dir = scripts_dir
           self.commands = []
           self.scan_and_register()

       def scan_and_register(self):
           """スクリプトディレクトリをスキャンしてメニュー登録"""
           if not os.path.exists(self.scripts_dir):
               return

           for filename in os.listdir(self.scripts_dir):
               if filename.endswith('.py'):
                   script_path = os.path.join(self.scripts_dir, filename)
                   name = os.path.splitext(filename)[0]

                   def make_runner(path):
                       def run():
                           exec(open(path, encoding='utf-8').read())
                       return run

                   cmd = ae.menu.add_to_window(
                       f"[Script] {name}",
                       make_runner(script_path)
                   )
                   self.commands.append(cmd)

       def cleanup(self):
           """すべてのメニューを削除"""
           for cmd in self.commands:
               cmd.remove()
           self.commands.clear()

   # 使用例
   launcher = ScriptLauncher("C:/scripts/ae")

注意事項
--------

.. note::
   - コールバックはメインスレッドで実行されます
   - コールバック内で長時間の処理を行うと、UIがフリーズします
   - スクリプト終了時には ``cmd.remove()`` でメニューを削除してください

.. warning::
   - メニュー名は一意である必要はありませんが、区別しやすい名前を推奨します
   - ``set_on_update`` コールバックは頻繁に呼び出されるため、軽量な処理にしてください
   - 例外がコールバック内で発生した場合、エラーはログに記録されますがUIには表示されません

関連項目
--------

- :doc:`/api/low-level/AEGP_CommandSuite1` - 低レベルCommand Suite API
- :doc:`panel_ui` - カスタムパネルUI
