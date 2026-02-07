Project
=======

.. currentmodule:: ae

Projectクラスは、After Effectsのプロジェクトを表します。

概要
----

``Project`` クラスは、After Effectsプロジェクトへの高レベルなアクセスを提供します。プロジェクト内のアイテム（コンポジション、フッテージ、フォルダ）の管理、プロジェクト設定の変更、保存などの機能を提供します。

**特徴**:

- Pythonicなインターフェース
- 型ヒント完備
- 自動リソース管理
- エラーハンドリング

**基本的なインポート**:

.. code-block:: python

   import ae
   from typing import Optional, List, Union, Dict, Any

   # プロジェクトを取得
   project = ae.Project.get_current()

クラスリファレンス
------------------

.. class:: Project

   After Effectsプロジェクトを表すクラス。

   プロジェクト内のアイテム（コンポジション、フッテージ、フォルダ）へのアクセスと
   プロジェクト設定の管理を提供します。

プロパティ一覧
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 25 25 50

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - プロジェクトが有効かどうか（読み取り専用）
   * - ``name``
     - str
     - プロジェクト名（読み取り専用）
   * - ``path``
     - str
     - プロジェクトファイルパス（読み取り専用）
   * - ``bit_depth``
     - int
     - ビット深度 (8, 16, 32)（読み書き可能）
   * - ``is_dirty``
     - bool
     - 未保存の変更があるか（読み取り専用）
   * - ``items``
     - List[Item]
     - 全アイテムのリスト（読み取り専用）
   * - ``root_folder``
     - Folder
     - ルートフォルダ（読み取り専用）
   * - ``active_item``
     - Optional[Item]
     - アクティブなアイテム（読み取り専用）
   * - ``num_items``
     - int
     - アイテム数（読み取り専用）

メソッド一覧
~~~~~~~~~~~~

静的メソッド
^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``Project.get_current() -> Project``
     - 現在のプロジェクトを取得
   * - ``Project.is_open() -> bool``
     - プロジェクトが開かれているか確認
   * - ``Project.num_projects() -> int``
     - 開いているプロジェクト数を取得
   * - ``Project.get_by_index(index) -> Project``
     - インデックスでプロジェクトを取得
   * - ``Project.new_project() -> Project``
     - 新しいプロジェクトを作成
   * - ``Project.open_project(file_path) -> Project``
     - プロジェクトファイルを開く
   * - ``Project.from_dict(data) -> Project``
     - 辞書からプロジェクトを作成

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 55 45

   * - メソッド
     - 説明
   * - ``item_by_name(name) -> Optional[Item]``
     - 名前でアイテムを検索
   * - ``create_folder(name, parent_folder=None) -> Folder``
     - 新しいフォルダを作成
   * - ``create_comp(...) -> Optional[CompItem]``
     - 新しいコンポジションを作成
   * - ``import_file(file_path, ...) -> Optional[FootageItem]``
     - ファイルをインポート
   * - ``save() -> bool``
     - プロジェクトを保存
   * - ``save_as(file_path) -> bool``
     - 名前を付けて保存
   * - ``get_time_display() -> dict``
     - 時間表示設定を取得
   * - ``set_time_display(time_display)``
     - 時間表示設定を変更
   * - ``to_dict() -> dict``
     - プロジェクトを辞書にエクスポート
   * - ``update_from_dict(data)``
     - 辞書でプロジェクトを更新

プロパティ
~~~~~~~~~~

.. attribute:: Project.valid
   :type: bool

   プロジェクトが有効かどうか。

   プロジェクトが閉じられた場合は ``False`` になります。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      if project.valid:
          print("プロジェクトは有効です")

.. attribute:: Project.name
   :type: str

   プロジェクト名。

   保存されていないプロジェクトの場合は "Untitled Project" などのデフォルト名が返されます。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      print(f"プロジェクト名: {project.name}")

.. attribute:: Project.path
   :type: str

   プロジェクトファイルのパス。

   保存されていない場合は空文字列 ``""`` が返されます。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      if project.path:
          print(f"プロジェクトパス: {project.path}")
      else:
          print("プロジェクトは未保存です")

.. attribute:: Project.bit_depth
   :type: int

   プロジェクトのビット深度 (8, 16, 32)。

   設定可能なプロパティです。

   **ビット深度の選択肢**:

   - ``8``: 8ビット/チャンネル（標準、ファイルサイズ小）
   - ``16``: 16ビット/チャンネル（高品質、中間ファイルサイズ）
   - ``32``: 32ビット/チャンネル（最高品質、浮動小数点、ファイルサイズ大）

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()

      # 現在のビット深度を取得
      print(f"現在のビット深度: {project.bit_depth}ビット")

      # ビット深度を変更
      project.bit_depth = 32  # 32ビットに変更

.. attribute:: Project.is_dirty
   :type: bool

   プロジェクトが編集されているか（未保存の変更があるか）。

   保存されていない変更がある場合は ``True`` を返します。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      if project.is_dirty:
          print("プロジェクトに未保存の変更があります")

メソッド
~~~~~~~~

.. method:: Project.set_dirty() -> None

   プロジェクトを「未保存の変更あり」としてマークします。

   スクリプトからプロジェクトのデータを変更した際に、AEの保存ダイアログが表示されるようにするために使用します。

   :raises RuntimeError: AdvApp Suite が利用できない場合

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      # プロジェクトデータを変更した後
      project.set_dirty()

.. attribute:: Project.items
   :type: List[Union[Item, Folder, CompItem, FootageItem]]

   プロジェクト内のすべてのアイテムのリスト。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      for item in project.items:
          print(f"- {item.name}")

.. attribute:: Project.root_folder
   :type: Folder

   プロジェクトのルートフォルダ。

   すべてのアイテムはルートフォルダまたはその子フォルダに含まれます。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      root = project.root_folder
      print(f"ルートフォルダのアイテム数: {len(root.items)}")

.. attribute:: Project.active_item
   :type: Optional[Union[Item, Folder, CompItem, FootageItem]]

   現在アクティブな（選択されている）アイテム。

   選択されていない場合は ``None`` を返します。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      active = project.active_item
      if active:
          print(f"アクティブアイテム: {active.name}")
      else:
          print("アイテムが選択されていません")

.. attribute:: Project.num_items
   :type: int

   プロジェクト内のアイテム数。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      print(f"アイテム数: {project.num_items}")

メソッド
~~~~~~~~

.. method:: Project.item_by_name(name: str) -> Optional[Union[Item, Folder, CompItem, FootageItem]]

   名前でアイテムを検索します。

   :param name: アイテム名
   :type name: str
   :return: 見つかったアイテム、見つからない場合は ``None``
   :rtype: Optional[Union[Item, Folder, CompItem, FootageItem]]

   .. note::
      同じ名前のアイテムが複数ある場合、最初に見つかったアイテムが返されます。

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      item = project.item_by_name("MyComp")
      if item:
          print(f"見つかりました: {item.name}")
      else:
          print("アイテムが見つかりませんでした")

.. method:: Project.create_folder(name: str, parent_folder: Optional[Union[Folder, Item]] = None) -> Folder

   新しいフォルダを作成します。

   :param name: フォルダ名
   :type name: str
   :param parent_folder: 親フォルダ（省略時はルートフォルダ）
   :type parent_folder: Optional[Union[Folder, Item]]
   :return: 作成されたフォルダ
   :rtype: Folder

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()

      # ルートフォルダにフォルダを作成
      folder = project.create_folder("Assets")

      # ネストしたフォルダを作成
      subfolder = project.create_folder("Videos", parent_folder=folder)

.. method:: Project.create_comp(name: str, width: int, height: int, pixel_aspect: float = 1.0, duration: float = 10.0, frame_rate: float = 30.0, parent_folder: Optional[Union[Folder, Item]] = None) -> Optional[CompItem]

   新しいコンポジションを作成します。

   :param name: コンポジション名
   :type name: str
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int
   :param pixel_aspect: ピクセルアスペクト比（デフォルト: 1.0）
   :type pixel_aspect: float
   :param duration: デュレーション（秒、デフォルト: 10.0）
   :type duration: float
   :param frame_rate: フレームレート（fps、デフォルト: 30.0）
   :type frame_rate: float
   :param parent_folder: 親フォルダ（省略時はルートフォルダ）
   :type parent_folder: Optional[Union[Folder, Item]]
   :return: 作成されたコンポジション
   :rtype: Optional[CompItem]

   **一般的な解像度とフレームレート**:

   - HD (1280x720): 16:9アスペクト比
   - Full HD (1920x1080): 16:9アスペクト比
   - 4K (3840x2160): 16:9アスペクト比
   - フレームレート: 24fps (映画), 30fps (標準), 60fps (高フレームレート)

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()

      # Full HD、30fps、10秒のコンポジションを作成
      comp = project.create_comp(
          name="MyComp",
          width=1920,
          height=1080,
          pixel_aspect=1.0,
          duration=10.0,
          frame_rate=30.0
      )

      # 4K、24fps、5秒のコンポジションを作成
      comp_4k = project.create_comp(
          name="4K Comp",
          width=3840,
          height=2160,
          duration=5.0,
          frame_rate=24.0
      )

.. method:: Project.import_file(file_path: str, parent_folder: Optional[Union[Folder, Item]] = None, sequence_options: Optional[Dict[str, Any]] = None) -> Optional[FootageItem]

   ファイルをプロジェクトにインポートします。

   :param file_path: インポートするファイルのパス
   :type file_path: str
   :param parent_folder: 親フォルダ（省略時はルートフォルダ）
   :type parent_folder: Optional[Union[Folder, Item]]
   :param sequence_options: シーケンスオプション辞書（省略可）
   :type sequence_options: Optional[Dict[str, Any]]
   :return: インポートされたフッテージアイテム
   :rtype: Optional[FootageItem]

   **シーケンスオプション** (``sequence_options``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``is_sequence``
        - bool
        - 画像シーケンスとしてインポート
      * - ``force_alphabetical``
        - bool
        - アルファベット順を強制
      * - ``start_frame``
        - int
        - 開始フレーム番号
      * - ``end_frame``
        - int
        - 終了フレーム番号

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()

      # 単一の画像をインポート
      footage = project.import_file(r"C:\path\to\image.png")

      # 動画をインポート
      video = project.import_file(r"C:\path\to\video.mp4")

      # 画像シーケンスとしてインポート
      sequence = project.import_file(
          r"C:\path\to\frame_0001.png",
          sequence_options={
              'is_sequence': True,
              'force_alphabetical': True,
              'start_frame': 1,
              'end_frame': 100
          }
      )

      # フォルダにインポート
      folder = project.create_folder("Assets")
      footage = project.import_file(
          r"C:\path\to\image.png",
          parent_folder=folder
      )

.. method:: Project.save() -> bool

   プロジェクトを保存します。

   プロジェクトが保存されていない場合は失敗します。その場合は ``save_as()`` を使用してください。

   :return: 保存が成功した場合は ``True``、失敗した場合は ``False``
   :rtype: bool

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()

      if project.save():
          print("プロジェクトを保存しました")
      else:
          print("プロジェクトの保存に失敗しました")

.. method:: Project.save_as(file_path: str) -> bool

   プロジェクトを新しいパスに保存します（名前を付けて保存）。

   :param file_path: 保存先のパス
   :type file_path: str
   :return: 保存が成功した場合は ``True``、失敗した場合は ``False``
   :rtype: bool

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()

      if project.save_as(r"C:\Projects\MyProject_v2.aep"):
          print("プロジェクトを保存しました")
      else:
          print("プロジェクトの保存に失敗しました")

.. method:: Project.get_time_display() -> dict

   プロジェクトの時間表示設定を取得します。

   :return: 時間表示設定の辞書
   :rtype: dict

   **戻り値の構造**:

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

      project = ae.Project.get_current()
      time_display = project.get_time_display()
      print(f"表示モード: {time_display['display_mode']}")

.. method:: Project.set_time_display(time_display: dict) -> None

   プロジェクトの時間表示設定を変更します。

   :param time_display: 時間表示設定の辞書（すべてのキーを含む必要があります）
   :type time_display: dict

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()

      # 現在の設定を取得
      time_display = project.get_time_display()

      # ドロップフレーム表示を有効化
      time_display['display_dropframe'] = True

      # 変更を適用
      project.set_time_display(time_display)

静的メソッド
~~~~~~~~~~~~

.. staticmethod:: Project.get_current() -> Project

   現在のプロジェクトを取得します。

   :return: 現在のプロジェクト
   :rtype: Project

   **例**:

   .. code-block:: python

      project = ae.Project.get_current()
      print(f"プロジェクト名: {project.name}")

.. staticmethod:: Project.is_open() -> bool

   プロジェクトが開かれているかを確認します。

   :return: プロジェクトが開かれている場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.Project.is_open():
          print("プロジェクトが開かれています")

.. staticmethod:: Project.num_projects() -> int

   開いているプロジェクトの数を取得します。

   :return: 開いているプロジェクト数
   :rtype: int

   **例**:

   .. code-block:: python

      num_projects = ae.Project.num_projects()
      print(f"開いているプロジェクト数: {num_projects}")

.. staticmethod:: Project.get_by_index(index: int) -> Project

   インデックスからプロジェクトを取得します。

   :param index: プロジェクトのインデックス（0から始まる）
   :type index: int
   :return: プロジェクト
   :rtype: Project

   **例**:

   .. code-block:: python

      project = ae.Project.get_by_index(0)  # 最初のプロジェクト

.. staticmethod:: Project.new_project() -> Project

   新しいプロジェクトを作成します。

   :return: 作成された新しいプロジェクト
   :rtype: Project

   **例**:

   .. code-block:: python

      project = ae.Project.new_project()
      print(f"新しいプロジェクト: {project.name}")

.. staticmethod:: Project.open_project(file_path: str) -> Project

   プロジェクトファイルを開きます。

   :param file_path: 開くプロジェクトファイルのパス
   :type file_path: str
   :return: 開いたプロジェクト
   :rtype: Project

   **例**:

   .. code-block:: python

      project = ae.Project.open_project(r"C:\Projects\MyProject.aep")
      print(f"プロジェクトを開きました: {project.name}")

使用例
------

プロジェクトの基本操作
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # 現在のプロジェクトを取得
   project = ae.Project.get_current()

   # プロジェクト情報を表示
   print(f"プロジェクト名: {project.name}")
   print(f"パス: {project.path if project.path else '(未保存)'}")
   print(f"ビット深度: {project.bit_depth}ビット")
   print(f"アイテム数: {project.num_items}")

コンポジションとフォルダの作成
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # フォルダを作成
   assets_folder = project.create_folder("Assets")
   comps_folder = project.create_folder("Comps")

   # コンポジションを作成
   main_comp = project.create_comp(
       name="MainComp",
       width=1920,
       height=1080,
       duration=10.0,
       frame_rate=30.0,
       parent_folder=comps_folder
   )

   print(f"コンポジション作成: {main_comp.name}")

ファイルのインポート
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # 画像をインポート
   logo = project.import_file(
       r"C:\Assets\logo.png",
       parent_folder=assets_folder
   )

   # 動画をインポート
   video = project.import_file(
       r"C:\Assets\background.mp4",
       parent_folder=assets_folder
   )

   # 画像シーケンスをインポート
   sequence = project.import_file(
       r"C:\Assets\frames\frame_0001.png",
       parent_folder=assets_folder,
       sequence_options={
           'is_sequence': True,
           'force_alphabetical': True,
           'start_frame': 1,
           'end_frame': 120
       }
   )

   print(f"インポート完了: {len(project.items)} アイテム")

アイテムの検索と操作
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # 名前でアイテムを検索
   comp = project.item_by_name("MainComp")
   if comp:
       print(f"見つかりました: {comp.name}")

   # すべてのアイテムをループ
   print("プロジェクト内のアイテム:")
   for item in project.items:
       print(f"  - {item.name} ({type(item).__name__})")

   # アクティブなアイテムを取得
   active = project.active_item
   if active:
       print(f"アクティブアイテム: {active.name}")

プロジェクトの保存
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # 編集状態を確認
   if project.is_dirty:
       print("プロジェクトに未保存の変更があります")

       # 保存されているプロジェクトの場合
       if project.path:
           project.save()
           print("プロジェクトを保存しました")
       else:
           # 未保存のプロジェクトの場合
           project.save_as(r"C:\Projects\MyProject.aep")
           print("プロジェクトを新規保存しました")

プロジェクト設定の変更
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # ビット深度を変更
   project.bit_depth = 32  # 32ビット浮動小数点
   print(f"ビット深度を {project.bit_depth}ビット に変更しました")

   # 時間表示設定を変更
   time_display = project.get_time_display()
   time_display['display_dropframe'] = True
   project.set_time_display(time_display)
   print("時間表示設定を変更しました")

プロジェクト情報のバックアップ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import json
   import os
   from datetime import datetime

   def backup_project_info():
       """プロジェクト情報をJSONファイルにバックアップ"""
       project = ae.Project.get_current()

       # プロジェクト情報を収集
       info = {
           'name': project.name,
           'path': project.path,
           'bit_depth': project.bit_depth,
           'num_items': project.num_items,
           'time_display': project.get_time_display(),
           'backup_date': datetime.now().isoformat(),
           'items': []
       }

       # アイテム情報を収集
       for item in project.items:
           info['items'].append({
               'name': item.name,
               'type': type(item).__name__,
           })

       # JSONファイルに保存
       backup_path = os.path.join(
           os.path.dirname(project.path) if project.path else "C:\\",
           f"{project.name}_backup.json"
       )

       with open(backup_path, 'w', encoding='utf-8') as f:
           json.dump(info, f, indent=2, ensure_ascii=False)

       print(f"バックアップを保存しました: {backup_path}")

   # 実行
   backup_project_info()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **プロジェクトの有効性確認**

   プロジェクトが閉じられた後は ``valid`` プロパティが ``False`` になります。

   .. code-block:: python

      if project.valid:
          # プロジェクト操作

2. **保存前の確認**

   未保存のプロジェクトの場合、``save()`` は失敗します。``save_as()`` を使用してください。

   .. code-block:: python

      if project.path:
          project.save()
      else:
          project.save_as(r"C:\Projects\NewProject.aep")

3. **編集状態の確認**

   保存が必要かどうかを ``is_dirty`` プロパティで確認できます。

   .. code-block:: python

      if project.is_dirty:
          print("未保存の変更があります")

ベストプラクティス
~~~~~~~~~~~~~~~~~~

Undoグループの使用
^^^^^^^^^^^^^^^^^^

複数の変更を行う場合は、Undoグループでまとめることを推奨します。

.. code-block:: python

   with ae.UndoGroup("複数の変更"):
       project.bit_depth = 32
       # その他の変更...

エラーハンドリング
^^^^^^^^^^^^^^^^^^

ファイル操作やインポートは失敗する可能性があるため、エラーハンドリングを実装してください。

.. code-block:: python

   try:
       footage = project.import_file(r"C:\path\to\file.png")
       if footage:
           print(f"インポート成功: {footage.name}")
       else:
           print("インポートに失敗しました")
   except Exception as e:
       print(f"エラー: {e}")

パスの検証
^^^^^^^^^^

ファイルパスは存在を確認してから使用してください。

.. code-block:: python

   import os

   file_path = r"C:\path\to\file.png"
   if os.path.exists(file_path):
       footage = project.import_file(file_path)

関連項目
--------

- :doc:`comp` - コンポジション操作
- :doc:`layer` - レイヤー操作
- :doc:`../low-level/AEGP_ProjSuite6` - 低レベルAPI (SDK)
- :doc:`../low-level/AEGP_ItemSuite9` - アイテム管理 (SDK)
