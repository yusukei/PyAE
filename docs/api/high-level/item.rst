Item
====

.. currentmodule:: ae

Itemクラスは、プロジェクト内のすべてのアイテムの基底クラスです。

概要
----

``Item`` クラスは、After Effectsプロジェクト内のアイテム（コンポジション、フッテージ、フォルダ）への共通インターフェースを提供します。

**特徴**:

- すべてのアイテムタイプの共通プロパティを提供
- 名前、コメント、ラベル、選択状態などの管理
- 親フォルダの設定と階層構造の操作

**基本的なインポート**:

.. code-block:: python

   import ae
   from ae import ItemType

   # アクティブなアイテムを取得
   item = ae.Item.get_active()

**クラス階層**:

.. code-block:: text

   Item (基底クラス)
   ├── Folder
   ├── CompItem
   └── FootageItem

クラスリファレンス
------------------

.. class:: Item

   プロジェクトアイテムの基底クラス。

   すべてのアイテムタイプ（Folder、CompItem、FootageItem）はこのクラスを継承します。

プロパティ一覧
~~~~~~~~~~~~~~

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
     - アイテムタイプ（読み取り専用）
   * - ``id``
     - int
     - アイテムID（プロジェクト内で一意、読み取り専用）
   * - ``name``
     - str
     - アイテム名（読み書き可能）
   * - ``comment``
     - str
     - コメント（読み書き可能）
   * - ``label``
     - int
     - ラベルカラー番号 0-16（読み書き可能）
   * - ``selected``
     - bool
     - 選択状態（読み書き可能）
   * - ``width``
     - int
     - 幅（ピクセル、読み取り専用）
   * - ``height``
     - int
     - 高さ（ピクセル、読み取り専用）
   * - ``duration``
     - float
     - デュレーション（秒、読み取り専用）
   * - ``frame_rate``
     - float
     - フレームレート（コンポジションのみ、読み取り専用）
   * - ``use_proxy``
     - bool
     - プロキシを使用しているか（読み書き可能）
   * - ``missing``
     - bool
     - フッテージが欠落しているか（読み取り専用）
   * - ``parent_folder``
     - Optional[Folder]
     - 親フォルダ（読み書き可能、Noneでルートへ移動）

メソッド一覧
~~~~~~~~~~~~

静的メソッド
^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``Item.get_active() -> Optional[Item]``
     - アクティブなアイテムを取得

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``delete()``
     - アイテムを削除
   * - ``duplicate() -> Item``
     - アイテムを複製（コンポジションのみサポート）

Pythonプロトコルメソッド
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``__repr__() -> str``
     - 文字列表現を返す（例: ``<Item: 'Name' id=42>``）
   * - ``__bool__() -> bool``
     - ``valid`` プロパティと同等。``if item:`` で有効性を判定可能
   * - ``__eq__(other) -> bool``
     - ハンドルベースの等値比較
   * - ``__hash__() -> int``
     - ハンドルベースのハッシュ。辞書のキーやセットの要素として使用可能

プロパティ
~~~~~~~~~~

.. attribute:: Item.valid
   :type: bool

   アイテムが有効かどうか。

   アイテムが削除された場合は ``False`` になります。

   **例**:

   .. code-block:: python

      item = ae.Item.get_active()
      if item and item.valid:
          print("アイテムは有効です")

.. attribute:: Item.type
   :type: ItemType

   アイテムタイプ。

   ``ItemType.Folder``、``ItemType.Comp``、``ItemType.Footage`` のいずれか。

.. attribute:: Item.name
   :type: str

   アイテム名。

   読み書き可能なプロパティです。

   **例**:

   .. code-block:: python

      item.name = "New Name"

.. attribute:: Item.parent_folder
   :type: Optional[Folder]

   親フォルダ。

   ``None`` を設定するとルートフォルダに移動します。

   **例**:

   .. code-block:: python

      # フォルダを作成してアイテムを移動
      folder = project.create_folder("Archive")
      item.parent_folder = folder

      # ルートに移動
      item.parent_folder = None

Pythonプロトコルメソッド
~~~~~~~~~~~~~~~~~~~~~~~

.. method:: Item.__repr__()
   :rtype: str

   アイテムの文字列表現を返します。

   フォーマット: ``<Item: 'Name' id=42>``

   **例**:

   .. code-block:: python

      item = ae.Item.get_active()
      print(item)  # <Item: 'Main Comp' id=42>

.. method:: Item.__bool__()
   :rtype: bool

   アイテムが有効かどうかを返します。``valid`` プロパティと同等です。

   ``if item:`` のような条件式で直接使用できます。

   **例**:

   .. code-block:: python

      item = ae.Item.get_active()
      if item:
          print("アイテムは有効です")

.. method:: Item.__eq__(other)
   :rtype: bool

   ハンドルベースの等値比較を行います。

   同じAfter Effectsアイテムを参照している場合に ``True`` を返します。

   **例**:

   .. code-block:: python

      item1 = project.item_by_name("Main Comp")
      item2 = ae.Item.get_active()
      if item1 == item2:
          print("同じアイテムです")

.. method:: Item.__hash__()
   :rtype: int

   ハンドルベースのハッシュ値を返します。

   辞書のキーやセットの要素として使用できます。

   **例**:

   .. code-block:: python

      # セットでアイテムを管理
      visited = set()
      visited.add(item)

      # 辞書のキーとして使用
      metadata = {item: {"status": "done"}}

ItemType 列挙型
---------------

.. list-table::
   :header-rows: 1
   :widths: 20 10 70

   * - 値
     - 数値
     - 説明
   * - ``None_``
     - 0
     - なし/無効
   * - ``Folder``
     - 1
     - フォルダ
   * - ``Comp``
     - 2
     - コンポジション
   * - ``Footage``
     - 3
     - フッテージ
   * - ``Solid``
     - 4
     - ソリッド

使用例
------

アイテムの取得
~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from ae import ItemType

   project = ae.Project.get_current()

   # すべてのアイテムを取得
   for item in project.items:
       print(f"{item.name} ({item.type})")

   # アクティブなアイテムを取得
   active = ae.Item.get_active()
   if active:
       print(f"Active: {active.name}")

   # 名前で検索
   item = project.item_by_name("Main Comp")

アイテムタイプの判定
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from ae import ItemType

   project = ae.Project.get_current()

   for item in project.items:
       if item.type == ItemType.Folder:
           print(f"Folder: {item.name}")
       elif item.type == ItemType.Comp:
           print(f"Comp: {item.name}")
       elif item.type == ItemType.Footage:
           print(f"Footage: {item.name}")

アイテムのプロパティ変更
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   project = ae.Project.get_current()
   item = project.item_by_name("Main Comp")

   if item:
       # 名前の変更
       item.name = "Main Comp v2"

       # コメントの設定
       item.comment = "最終バージョン"

       # ラベルカラーの設定
       item.label = 9  # 緑色

       # 選択
       item.selected = True

注意事項
--------

.. note::
   - ``Item`` は抽象基底クラスとして機能し、直接インスタンス化することは通常ありません
   - ``duplicate()`` メソッドはコンポジションのみサポートしています
   - ``parent_folder`` を ``None`` に設定するとルートフォルダに移動します

.. warning::
   - ``delete()`` は取り消しできない場合があります
   - 削除されたアイテムへの参照は無効になります（``valid`` が False になります）

関連項目
--------

- :doc:`folder` - フォルダクラス
- :doc:`project` - プロジェクトクラス
- :doc:`comp` - コンポジションクラス（CompItem）
- :doc:`/api/low-level/AEGP_ItemSuite9` - 低レベルAPI
