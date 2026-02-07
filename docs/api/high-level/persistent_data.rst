永続データ（設定保存）
======================

.. currentmodule:: ae.persistent_data

``ae.persistent_data`` モジュールは、After Effectsの設定（プリファレンス）を永続的に保存・読み込みするためのAPIを提供します。

概要
----

このモジュールを使用すると、スクリプトやプラグインの設定を After Effects の環境設定ファイルに保存できます。
保存されたデータは After Effects を再起動しても保持されます。

**主な機能**:

- 文字列、整数、浮動小数点数の保存と読み込み
- セクション/キー形式での階層的なデータ管理
- マシン固有またはマシン非依存のデータストレージ
- AEの環境設定ディレクトリへのアクセス

基本的な使い方
--------------

データの保存と読み込み
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # ブロブを取得（マシン固有）
   blob = ae.persistent_data.get_machine_specific()

   # 文字列の保存と読み込み
   blob.set_string("MyPlugin", "last_path", "/path/to/file.aep")
   path = blob.get_string("MyPlugin", "last_path", "")  # デフォルト値 ""

   # 整数の保存と読み込み
   blob.set_int("MyPlugin", "window_width", 800)
   width = blob.get_int("MyPlugin", "window_width", 640)

   # 浮動小数点数の保存と読み込み
   blob.set_float("MyPlugin", "zoom_level", 1.5)
   zoom = blob.get_float("MyPlugin", "zoom_level", 1.0)

セクションの使用
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   blob = ae.persistent_data.get_machine_specific()

   # セクションを取得
   section = blob["MyPlugin"]

   # セクション内でデータを操作
   section.set_string("username", "user@example.com")
   section.set_int("max_items", 100)

   # キーの存在確認
   if "username" in section:
       print(f"Username: {section.get_string('username', '')}")

   # セクション内のすべてのキーを取得
   keys = section.keys()
   for key in keys:
       print(f"Key: {key}")

API リファレンス
----------------

モジュール関数
~~~~~~~~~~~~~~

.. function:: get_machine_specific() -> Blob

   マシン固有のデータブロブを取得します。

   このブロブに保存されたデータは、現在のコンピュータでのみ有効です。
   同じユーザーが別のコンピュータで After Effects を使用した場合、
   このデータにはアクセスできません。

   :return: マシン固有のデータブロブ
   :rtype: Blob

.. function:: get_machine_independent() -> Blob

   マシン非依存のデータブロブを取得します。

   このブロブに保存されたデータは、ユーザーの環境設定と共に
   同期される可能性があります（Creative Cloud 同期など）。

   :return: マシン非依存のデータブロブ
   :rtype: Blob

.. function:: get_prefs_directory() -> str

   After Effects の環境設定ディレクトリのパスを取得します。

   :return: 環境設定ディレクトリのフルパス
   :rtype: str

   **例**:

   .. code-block:: python

      prefs_dir = ae.persistent_data.get_prefs_directory()
      print(f"設定ディレクトリ: {prefs_dir}")
      # 例: C:/Users/Username/AppData/Roaming/Adobe/After Effects/25.0

.. function:: get_blob(blob_type: int) -> Blob

   指定されたタイプのブロブを取得します。

   :param blob_type: ブロブタイプ（0: マシン固有、1: マシン非依存）
   :type blob_type: int
   :return: データブロブ
   :rtype: Blob

Blob クラス
~~~~~~~~~~~

.. class:: Blob

   永続データを管理するクラス。セクションとキーの階層構造でデータを保存します。

   .. method:: __getitem__(section_name: str) -> Section

      セクションを取得します。

      :param section_name: セクション名
      :return: セクションオブジェクト

   .. method:: get_string(section: str, key: str, default: str = "") -> str

      文字列値を取得します。

      :param section: セクション名
      :param key: キー名
      :param default: キーが存在しない場合のデフォルト値
      :return: 保存された文字列、またはデフォルト値

   .. method:: set_string(section: str, key: str, value: str) -> None

      文字列値を保存します。

      :param section: セクション名
      :param key: キー名
      :param value: 保存する文字列

   .. method:: get_int(section: str, key: str, default: int = 0) -> int

      整数値を取得します。

      :param section: セクション名
      :param key: キー名
      :param default: キーが存在しない場合のデフォルト値
      :return: 保存された整数、またはデフォルト値

   .. method:: set_int(section: str, key: str, value: int) -> None

      整数値を保存します。

      :param section: セクション名
      :param key: キー名
      :param value: 保存する整数

   .. method:: get_float(section: str, key: str, default: float = 0.0) -> float

      浮動小数点数を取得します。

      :param section: セクション名
      :param key: キー名
      :param default: キーが存在しない場合のデフォルト値
      :return: 保存された浮動小数点数、またはデフォルト値

   .. method:: set_float(section: str, key: str, value: float) -> None

      浮動小数点数を保存します。

      :param section: セクション名
      :param key: キー名
      :param value: 保存する浮動小数点数

   .. method:: contains(section: str, key: str) -> bool

      指定されたセクション/キーが存在するかチェックします。

      :param section: セクション名
      :param key: キー名
      :return: 存在する場合は True

   .. method:: delete(section: str, key: str) -> None

      指定されたキーを削除します。

      :param section: セクション名
      :param key: キー名

   .. method:: sections() -> List[str]

      すべてのセクション名のリストを取得します。

      :return: セクション名のリスト

Section クラス
~~~~~~~~~~~~~~

.. class:: Section

   Blob 内の特定のセクションを操作するクラス。

   .. method:: get_string(key: str, default: str = "") -> str

      文字列値を取得します。

   .. method:: set_string(key: str, value: str) -> None

      文字列値を保存します。

   .. method:: get_int(key: str, default: int = 0) -> int

      整数値を取得します。

   .. method:: set_int(key: str, value: int) -> None

      整数値を保存します。

   .. method:: get_float(key: str, default: float = 0.0) -> float

      浮動小数点数を取得します。

   .. method:: set_float(key: str, value: float) -> None

      浮動小数点数を保存します。

   .. method:: __contains__(key: str) -> bool

      キーが存在するかチェックします（``in`` 演算子）。

   .. method:: delete(key: str) -> None

      キーを削除します。

   .. method:: keys() -> List[str]

      セクション内のすべてのキー名を取得します。

実用例
------

プラグイン設定の管理
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   class PluginSettings:
       """プラグイン設定を管理するクラス"""

       SECTION = "MyAwesomePlugin"

       def __init__(self):
           self.blob = ae.persistent_data.get_machine_specific()

       def get(self, key: str, default=None):
           """設定値を取得"""
           section = self.blob[self.SECTION]
           if key not in section:
               return default

           # 型に応じて取得
           if isinstance(default, int):
               return section.get_int(key, default)
           elif isinstance(default, float):
               return section.get_float(key, default)
           else:
               return section.get_string(key, str(default) if default else "")

       def set(self, key: str, value):
           """設定値を保存"""
           section = self.blob[self.SECTION]

           if isinstance(value, int):
               section.set_int(key, value)
           elif isinstance(value, float):
               section.set_float(key, value)
           else:
               section.set_string(key, str(value))

       def delete(self, key: str):
           """設定を削除"""
           self.blob[self.SECTION].delete(key)

   # 使用例
   settings = PluginSettings()

   # 設定を保存
   settings.set("output_path", "C:/renders/")
   settings.set("quality", 95)
   settings.set("frame_rate", 29.97)

   # 設定を読み込み
   path = settings.get("output_path", "")
   quality = settings.get("quality", 80)
   fps = settings.get("frame_rate", 24.0)

   print(f"Output: {path}, Quality: {quality}, FPS: {fps}")

ウィンドウ位置の記憶
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def save_window_state(name: str, x: int, y: int, width: int, height: int):
       """ウィンドウの状態を保存"""
       blob = ae.persistent_data.get_machine_specific()
       section = f"WindowState_{name}"

       blob.set_int(section, "x", x)
       blob.set_int(section, "y", y)
       blob.set_int(section, "width", width)
       blob.set_int(section, "height", height)

   def load_window_state(name: str, defaults: dict) -> dict:
       """ウィンドウの状態を読み込み"""
       blob = ae.persistent_data.get_machine_specific()
       section = f"WindowState_{name}"

       return {
           "x": blob.get_int(section, "x", defaults.get("x", 100)),
           "y": blob.get_int(section, "y", defaults.get("y", 100)),
           "width": blob.get_int(section, "width", defaults.get("width", 400)),
           "height": blob.get_int(section, "height", defaults.get("height", 300)),
       }

   # 使用例
   # ウィンドウを閉じる前に状態を保存
   save_window_state("MainPanel", 200, 150, 800, 600)

   # 次回起動時に状態を復元
   state = load_window_state("MainPanel", {"x": 0, "y": 0, "width": 640, "height": 480})
   print(f"Window position: ({state['x']}, {state['y']})")
   print(f"Window size: {state['width']}x{state['height']}")

最近使用したファイルの記録
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import json

   class RecentFiles:
       """最近使用したファイルを管理"""

       def __init__(self, section: str = "RecentFiles", max_items: int = 10):
           self.blob = ae.persistent_data.get_machine_specific()
           self.section = section
           self.max_items = max_items

       def add(self, file_path: str):
           """ファイルを追加"""
           files = self.get_all()

           # 既存のエントリを削除
           if file_path in files:
               files.remove(file_path)

           # 先頭に追加
           files.insert(0, file_path)

           # 最大数を超えたら削除
           files = files[:self.max_items]

           # JSON として保存
           self.blob.set_string(self.section, "files", json.dumps(files))

       def get_all(self) -> list:
           """すべてのファイルを取得"""
           data = self.blob.get_string(self.section, "files", "[]")
           try:
               return json.loads(data)
           except json.JSONDecodeError:
               return []

       def clear(self):
           """履歴をクリア"""
           self.blob.set_string(self.section, "files", "[]")

   # 使用例
   recent = RecentFiles()
   recent.add("C:/projects/project1.aep")
   recent.add("C:/projects/project2.aep")

   for i, path in enumerate(recent.get_all()):
       print(f"{i + 1}. {path}")

注意事項
--------

.. note::
   - セクション名とキー名は一意である必要があります
   - 保存されたデータは After Effects の環境設定と共に管理されます
   - 環境設定をリセットすると、保存したデータも失われます

.. warning::
   - 機密データ（パスワードなど）を保存しないでください
   - 大量のデータを保存すると、After Effects の起動が遅くなる可能性があります
   - セクション名には一意のプレフィックスを使用して、他のプラグインとの衝突を避けてください

関連項目
--------

- :doc:`/api/low-level/AEGP_PersistentDataSuite4` - 低レベルAPI
