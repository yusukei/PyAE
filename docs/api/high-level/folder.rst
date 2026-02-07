Folder
======

.. currentmodule:: ae

Folderクラスは、プロジェクト内のアイテムを整理するためのコンテナです。

概要
----

``Folder`` クラスは、After Effectsプロジェクト内のフォルダを表します。
``Item`` クラスを継承し、フォルダ固有の機能を提供します。

**特徴**:

- アイテムの整理とグループ化
- 階層構造によるプロジェクト管理
- フォルダ内アイテムへのアクセス

**基本的なインポート**:

.. code-block:: python

   import ae
   from ae import ItemType

   project = ae.Project.get_current()

   # フォルダを作成
   folder = project.create_folder("My Folder")

**クラス階層**:

.. code-block:: text

   Item (基底クラス)
   └── Folder ← このクラス

クラスリファレンス
------------------

.. class:: Folder

   フォルダアイテムを表すクラス。

   ``Item`` クラスを継承し、フォルダ内のアイテム管理機能を追加します。

プロパティ一覧
~~~~~~~~~~~~~~

継承プロパティ（Item）
^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 25 20 55

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - アイテムが有効かどうか（読み取り専用）
   * - ``type``
     - ItemType
     - アイテムタイプ（常に ``ItemType.Folder``、読み取り専用）
   * - ``id``
     - int
     - アイテムID（プロジェクト内で一意、読み取り専用）
   * - ``name``
     - str
     - フォルダ名（読み書き可能）
   * - ``comment``
     - str
     - コメント（読み書き可能）
   * - ``label``
     - int
     - ラベルカラー番号 0-16（読み書き可能）
   * - ``selected``
     - bool
     - 選択状態（読み書き可能）
   * - ``parent_folder``
     - Optional[Folder]
     - 親フォルダ（読み書き可能、Noneでルートへ移動）

Folder固有プロパティ
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 25 20 55

   * - プロパティ
     - 型
     - 説明
   * - ``num_items``
     - int
     - フォルダ内のアイテム数（読み取り専用）
   * - ``items``
     - List[Item]
     - フォルダ内のアイテムリスト（読み取り専用）

メソッド一覧
~~~~~~~~~~~~

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``delete()``
     - フォルダを削除（中身も削除される）

Pythonプロトコルメソッド
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``__repr__() -> str``
     - 文字列表現を返す（例: ``<Folder: 'Name' items=5>``）
   * - ``__bool__() -> bool``
     - ``valid`` プロパティと同等。``if folder:`` で有効性を判定可能
   * - ``__len__() -> int``
     - ``num_items`` と同等。``len(folder)`` でアイテム数を取得可能
   * - ``__iter__() -> Iterator[Item]``
     - フォルダ内のアイテムをイテレーション
   * - ``__getitem__(key) -> Item``
     - インデックスでアイテムを取得。負数インデックスに対応
   * - ``__contains__(name) -> bool``
     - 名前でアイテムの存在を確認。``"name" in folder`` の形式で使用可能

プロパティ
~~~~~~~~~~

.. attribute:: Folder.num_items
   :type: int

   フォルダ内のアイテム数。

   直接の子アイテムのみをカウントします（孫アイテムは含まれません）。

   **例**:

   .. code-block:: python

      folder = project.item_by_name("Footage")
      print(f"フォルダ内のアイテム数: {folder.num_items}")

.. attribute:: Folder.items
   :type: List[Item]

   フォルダ内のアイテムリスト。

   直接の子アイテムのみを返します（孫アイテムは含まれません）。

   **例**:

   .. code-block:: python

      folder = project.item_by_name("Footage")
      for item in folder.items:
          print(f"- {item.name} ({item.type})")

Pythonプロトコルメソッド
~~~~~~~~~~~~~~~~~~~~~~~

.. method:: Folder.__repr__()
   :rtype: str

   フォルダの文字列表現を返します。

   フォーマット: ``<Folder: 'Name' items=5>``

   **例**:

   .. code-block:: python

      folder = project.item_by_name("Footage")
      print(folder)  # <Folder: 'Footage' items=5>

.. method:: Folder.__bool__()
   :rtype: bool

   フォルダが有効かどうかを返します。``valid`` プロパティと同等です。

   ``if folder:`` のような条件式で直接使用できます。

   **例**:

   .. code-block:: python

      folder = project.item_by_name("Footage")
      if folder:
          print("フォルダは有効です")

.. method:: Folder.__len__()
   :rtype: int

   フォルダ内のアイテム数を返します。``num_items`` プロパティと同等です。

   **例**:

   .. code-block:: python

      folder = project.item_by_name("Footage")
      print(f"アイテム数: {len(folder)}")

.. method:: Folder.__iter__()
   :rtype: Iterator[Item]

   フォルダ内のアイテムをイテレーションします。

   ``for item in folder:`` の形式で使用できます。

   **例**:

   .. code-block:: python

      folder = project.item_by_name("Footage")
      for item in folder:
          print(item.name)

.. method:: Folder.__getitem__(key)
   :param int key: アイテムのインデックス
   :rtype: Item

   インデックスでアイテムを取得します。

   負数インデックスに対応しており、``folder[-1]`` で最後のアイテムを取得できます。
   範囲外のインデックスを指定した場合は ``IndexError`` が送出されます。

   **例**:

   .. code-block:: python

      folder = project.item_by_name("Footage")
      first = folder[0]    # 最初のアイテム
      last = folder[-1]    # 最後のアイテム

.. method:: Folder.__contains__(name)
   :param str name: 検索するアイテム名
   :rtype: bool

   指定した名前のアイテムがフォルダ内に存在するかを確認します。

   ``"name" in folder`` の形式で使用できます。

   **例**:

   .. code-block:: python

      folder = project.item_by_name("Footage")
      if "background.mp4" in folder:
          print("フッテージが見つかりました")

使用例
------

コレクションプロトコル
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   project = ae.Project.get_current()
   folder = project.item_by_name("Footage")

   # アイテム数を取得
   print(len(folder))  # 例: 5

   # インデックスでアクセス
   item = folder[0]     # 最初のアイテム
   last = folder[-1]    # 最後のアイテム

   # 名前で存在確認
   if "my_comp" in folder:
       print("見つかりました")

   # イテレーション
   for item in folder:
       print(item)

フォルダの作成
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   project = ae.Project.get_current()

   # ルートにフォルダを作成
   footage_folder = project.create_folder("Footage")

   # 親フォルダを指定して作成
   sub_folder = project.create_folder("Images", parent_folder=footage_folder)

フォルダ内のアイテムを列挙
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from ae import ItemType

   project = ae.Project.get_current()

   # フォルダを取得
   folder = project.item_by_name("Footage")

   if folder and folder.type == ItemType.Folder:
       print(f"フォルダ '{folder.name}' には {folder.num_items} 個のアイテムがあります")

       for item in folder.items:
           print(f"  - {item.name} ({item.type})")

フォルダの階層を走査
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from ae import ItemType

   def list_folder_contents(folder, indent=0):
       """フォルダの内容を再帰的に表示"""
       prefix = "  " * indent
       print(f"{prefix}{folder.name}/")

       for item in folder.items:
           if item.type == ItemType.Folder:
               list_folder_contents(item, indent + 1)
           else:
               print(f"{prefix}  {item.name}")

   project = ae.Project.get_current()
   root = project.root_folder
   list_folder_contents(root)

アイテムをフォルダに移動
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from ae import ItemType

   project = ae.Project.get_current()

   # 移動先フォルダを作成または取得
   archive = project.item_by_name("Archive")
   if not archive:
       archive = project.create_folder("Archive")

   # コンポジションをアーカイブフォルダに移動
   for item in project.items:
       if item.type == ItemType.Comp and "old" in item.name.lower():
           item.parent_folder = archive
           print(f"Moved: {item.name}")

フォルダ構造の作成
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_project_structure(project):
       """標準的なプロジェクト構造を作成"""
       # メインフォルダ
       comps = project.create_folder("01_Comps")
       footage = project.create_folder("02_Footage")
       precomps = project.create_folder("03_Precomps")
       assets = project.create_folder("04_Assets")

       # サブフォルダ
       project.create_folder("Video", parent_folder=footage)
       project.create_folder("Audio", parent_folder=footage)
       project.create_folder("Images", parent_folder=footage)

       project.create_folder("Graphics", parent_folder=assets)
       project.create_folder("Fonts", parent_folder=assets)

       return comps, footage, precomps, assets

   project = ae.Project.get_current()
   create_project_structure(project)

注意事項
--------

.. note::
   - ``Folder`` は ``Item`` を継承していますが、``width``、``height``、``duration``、``frame_rate`` は意味のある値を返しません
   - ``items`` プロパティは直接の子アイテムのみを返します（孫アイテムは含まれません）
   - フォルダを削除すると、中のアイテムもすべて削除されます

.. warning::
   - 空でないフォルダを削除する場合は注意してください
   - フォルダの削除は取り消しできない場合があります

関連項目
--------

- :doc:`item` - アイテム基底クラス
- :doc:`project` - プロジェクトクラス
- :doc:`comp` - コンポジションクラス（CompItem）
- :doc:`/api/low-level/AEGP_ItemSuite9` - 低レベルAPI
