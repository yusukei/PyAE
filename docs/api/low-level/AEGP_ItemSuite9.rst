AEGP_ItemSuite9
===============

.. currentmodule:: ae.sdk

AEGP_ItemSuite9は、After Effectsプロジェクト内のアイテム（コンポジション、フッテージ、フォルダなど）の管理と操作を行うためのSDK APIです。

概要
----

**実装状況**: 26/26関数実装 ✅

AEGP_ItemSuite9は以下の機能を提供します:

- プロジェクトアイテムの列挙と検索
- アイテム情報（名前、タイプ、サイズ、デュレーション）の取得と設定
- アイテムの選択状態の制御
- フォルダ構造の作成と管理
- アイテムプロパティ（コメント、ラベル、プロキシ）の管理
- アイテムの削除と親子関係の変更

基本概念
--------

アイテムハンドル (AEGP_ItemH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのプロジェクトアイテムを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - アイテムハンドルは、アイテムが存在する間のみ有効です
   - アイテムを削除した後は、ハンドルは無効になります
   - すべてのアイテム（コンポジション、フッテージ、フォルダ）は共通のアイテムハンドルを持ちます

アイテムタイプ (AEGP_ItemType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのプロジェクトアイテムには以下のタイプがあります:

.. list-table::
   :header-rows: 1

   * - タイプ定数
     - 値
     - 説明
   * - ``AEGP_ItemType_NONE``
     - 0
     - 未定義
   * - ``AEGP_ItemType_FOLDER``
     - 1
     - フォルダ
   * - ``AEGP_ItemType_COMP``
     - 2
     - コンポジション
   * - ``AEGP_ItemType_SOLID``
     - 3
     - 平面
   * - ``AEGP_ItemType_FOOTAGE``
     - 4
     - フッテージ（画像、動画、音声など）

アイテムフラグ (AEGP_ItemFlags)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

アイテムの状態を示すビットフラグです:

.. list-table::
   :header-rows: 1

   * - フラグ定数
     - 値
     - 説明
   * - ``AEGP_ItemFlag_MISSING``
     - 0x01
     - ファイルが見つからない
   * - ``AEGP_ItemFlag_HAS_PROXY``
     - 0x02
     - プロキシが設定されている
   * - ``AEGP_ItemFlag_USING_PROXY``
     - 0x04
     - プロキシを使用中
   * - ``AEGP_ItemFlag_MISSING_PROXY``
     - 0x08
     - プロキシファイルが見つからない
   * - ``AEGP_ItemFlag_HAS_VIDEO``
     - 0x10
     - ビデオデータを含む
   * - ``AEGP_ItemFlag_HAS_AUDIO``
     - 0x20
     - オーディオデータを含む
   * - ``AEGP_ItemFlag_STILL``
     - 0x40
     - 静止画像
   * - ``AEGP_ItemFlag_HAS_ACTIVE_AUDIO``
     - 0x80
     - アクティブなオーディオを含む

ラベルカラー (AEGP_LabelID)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

プロジェクトパネルでのアイテムのラベル色を示します（0～16の整数値）。

.. note::
   ラベルカラーは視覚的な分類に使用され、0が「なし（デフォルト）」、1～16が各色に対応します。

プロジェクト構造
~~~~~~~~~~~~~~~~

After Effectsのプロジェクトは階層構造を持ちます:

- **ルートフォルダ**: プロジェクトの最上位フォルダ（``AEGP_GetProjectRootFolder`` で取得）
- **フォルダ**: アイテムをグループ化するコンテナ
- **アイテム**: コンポジション、フッテージ、平面などの実体

.. code-block:: text

   Project Root
   ├── Folder A
   │   ├── Comp 1
   │   └── Footage 1
   ├── Folder B
   │   └── Comp 2
   └── Footage 2

Undoable操作
~~~~~~~~~~~~

以下の操作は **Undo可能** です（After EffectsのUndo履歴に記録されます）:

- ``AEGP_SetItemName`` - 名前変更
- ``AEGP_SetItemComment`` - コメント設定
- ``AEGP_SetItemLabel`` - ラベル設定
- ``AEGP_SetItemParentFolder`` - 親フォルダ変更
- ``AEGP_SetItemCurrentTime`` - カレント時間変更
- ``AEGP_SetItemUseProxy`` - プロキシ使用設定
- ``AEGP_DeleteItem`` - アイテム削除

.. tip::
   複数の変更を行う場合は、``AEGP_StartUndoGroup()`` と ``AEGP_EndUndoGroup()`` でグループ化することを推奨します。

API リファレンス
----------------

アイテム列挙
~~~~~~~~~~~~

.. function:: AEGP_GetFirstProjItem(projectH: int) -> int

   プロジェクトの最初のアイテムを取得します。

   :param projectH: プロジェクトハンドル
   :type projectH: int
   :return: 最初のアイテムハンドル
   :rtype: int

   .. note::
      アイテムは深さ優先順（フォルダの子→次のフォルダ）で列挙されます。

   **例**:

   .. code-block:: python

      projH = ae.sdk.AEGP_GetProjectByIndex(0)
      itemH = ae.sdk.AEGP_GetFirstProjItem(projH)

.. function:: AEGP_GetNextProjItem(projectH: int, itemH: int) -> int

   次のプロジェクトアイテムを取得します。

   :param projectH: プロジェクトハンドル
   :type projectH: int
   :param itemH: 現在のアイテムハンドル
   :type itemH: int
   :return: 次のアイテムハンドル（最後のアイテムの場合は ``0``）
   :rtype: int

   .. note::
      最後のアイテムの後は ``0`` を返します。これを使ってループを終了します。

   **例**:

   .. code-block:: python

      projH = ae.sdk.AEGP_GetProjectByIndex(0)
      itemH = ae.sdk.AEGP_GetFirstProjItem(projH)

      while itemH != 0:
          # アイテム処理
          itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)

.. function:: AEGP_GetActiveItem() -> int

   現在アクティブなアイテムを取得します。

   :return: アクティブなアイテムハンドル（アクティブなアイテムがない場合は ``0``）
   :rtype: int

   **例**:

   .. code-block:: python

      active_itemH = ae.sdk.AEGP_GetActiveItem()
      if active_itemH != 0:
          print("アクティブなアイテムがあります")

アイテム情報の取得
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetItemType(itemH: int) -> int

   アイテムのタイプを取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: アイテムタイプ（``AEGP_ItemType_*`` 定数）
   :rtype: int

   **例**:

   .. code-block:: python

      item_type = ae.sdk.AEGP_GetItemType(itemH)
      if item_type == 2:  # AEGP_ItemType_COMP
          print("コンポジションです")

.. function:: AEGP_GetTypeName(item_type: int) -> str

   アイテムタイプの名前を取得します。

   :param item_type: アイテムタイプ（``AEGP_ItemType_*`` 定数）
   :type item_type: int
   :return: タイプ名（例: "Composition", "Footage", "Folder"）
   :rtype: str

   **例**:

   .. code-block:: python

      type_name = ae.sdk.AEGP_GetTypeName(item_type)
      print(f"アイテムタイプ: {type_name}")

.. function:: AEGP_GetItemName(plugin_id: int, itemH: int) -> str

   アイテムの名前を取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param itemH: アイテムハンドル
   :type itemH: int
   :return: アイテム名（UTF-8文字列）
   :rtype: str

   .. note::
      内部的にUTF-16からUTF-8に自動変換されます。日本語のアイテム名も正しく取得できます。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      name = ae.sdk.AEGP_GetItemName(plugin_id, itemH)
      print(f"アイテム名: {name}")

.. function:: AEGP_GetItemID(itemH: int) -> int

   アイテムの一意識別子（ID）を取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: アイテムID（プロジェクト内で一意）
   :rtype: int

   .. note::
      IDはプロジェクトが開かれている間、一貫して同じアイテムを識別します。

   **例**:

   .. code-block:: python

      item_id = ae.sdk.AEGP_GetItemID(itemH)
      print(f"アイテムID: {item_id}")

.. function:: AEGP_GetItemFlags(itemH: int) -> int

   アイテムのフラグを取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: アイテムフラグ（ビットフラグの組み合わせ）
   :rtype: int

   **例**:

   .. code-block:: python

      flags = ae.sdk.AEGP_GetItemFlags(itemH)

      # ビットフラグをチェック
      if flags & 0x01:  # AEGP_ItemFlag_MISSING
          print("ファイルが見つかりません")
      if flags & 0x02:  # AEGP_ItemFlag_HAS_PROXY
          print("プロキシが設定されています")
      if flags & 0x10:  # AEGP_ItemFlag_HAS_VIDEO
          print("ビデオを含みます")

.. function:: AEGP_GetItemDuration(itemH: int) -> float

   アイテムのデュレーション（秒単位）を取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: デュレーション（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      duration = ae.sdk.AEGP_GetItemDuration(itemH)
      print(f"デュレーション: {duration:.2f}秒")

.. function:: AEGP_GetItemCurrentTime(itemH: int) -> float

   アイテムの現在時刻（秒単位）を取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: 現在時刻（秒）
   :rtype: float

   .. warning::
      この値はレンダリング中は更新されません。

   **例**:

   .. code-block:: python

      current_time = ae.sdk.AEGP_GetItemCurrentTime(itemH)
      print(f"現在時刻: {current_time:.2f}秒")

.. function:: AEGP_GetItemDimensions(itemH: int) -> tuple[int, int]

   アイテムの寸法（幅、高さ）を取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: (width, height) のタプル
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      width, height = ae.sdk.AEGP_GetItemDimensions(itemH)
      print(f"サイズ: {width}x{height}")

.. function:: AEGP_GetItemPixelAspectRatio(itemH: int) -> tuple[int, int]

   アイテムのピクセルアスペクト比を取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: (numerator, denominator) のタプル
   :rtype: tuple[int, int]

   .. note::
      ピクセルアスペクト比は分数として表現されます（例: 1:1 = スクエアピクセル）。

   **例**:

   .. code-block:: python

      num, den = ae.sdk.AEGP_GetItemPixelAspectRatio(itemH)
      aspect_ratio = num / den
      print(f"ピクセルアスペクト比: {num}:{den} ({aspect_ratio:.3f})")

アイテム情報の設定
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetItemName(itemH: int, name: str) -> None

   アイテムの名前を設定します（**Undo可能**）。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param name: 新しい名前（空文字列不可）
   :type name: str

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetItemName(itemH, "新しい名前")

.. function:: AEGP_SetItemCurrentTime(itemH: int, time_seconds: float) -> None

   アイテムの現在時刻を設定します（**Undo可能**）。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param time_seconds: 新しい現在時刻（秒、非負）
   :type time_seconds: float

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetItemCurrentTime(itemH, 5.0)  # 5秒に設定

選択とアクティブ状態
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_IsItemSelected(itemH: int) -> bool

   アイテムが選択されているかを確認します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: 選択されている場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_IsItemSelected(itemH):
          print("アイテムは選択されています")

.. function:: AEGP_SelectItem(itemH: int, select: bool, deselect_others: bool) -> None

   アイテムの選択状態を設定します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param select: ``True`` で選択、``False`` で選択解除
   :type select: bool
   :param deselect_others: ``True`` で他のすべてのアイテムを選択解除
   :type deselect_others: bool

   **例**:

   .. code-block:: python

      # アイテムを選択し、他を選択解除
      ae.sdk.AEGP_SelectItem(itemH, True, True)

      # 複数選択に追加
      ae.sdk.AEGP_SelectItem(itemH, True, False)

      # 選択解除
      ae.sdk.AEGP_SelectItem(itemH, False, False)

フォルダ構造の管理
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetItemParentFolder(itemH: int) -> int

   アイテムの親フォルダを取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: 親フォルダのアイテムハンドル（ルート階層の場合は ``0``）
   :rtype: int

   **例**:

   .. code-block:: python

      parent_folderH = ae.sdk.AEGP_GetItemParentFolder(itemH)
      if parent_folderH == 0:
          print("アイテムはルート階層にあります")

.. function:: AEGP_SetItemParentFolder(itemH: int, parent_folderH: int) -> None

   アイテムの親フォルダを設定します（**Undo可能**）。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param parent_folderH: 新しい親フォルダのハンドル（``0`` でルート階層に移動）
   :type parent_folderH: int

   **例**:

   .. code-block:: python

      # ルート階層に移動
      ae.sdk.AEGP_SetItemParentFolder(itemH, 0)

      # 特定のフォルダに移動
      ae.sdk.AEGP_SetItemParentFolder(itemH, folder_itemH)

.. function:: AEGP_CreateNewFolder(name: str, parent_folderH: int) -> int

   新しいフォルダを作成します。

   :param name: フォルダ名（空文字列不可）
   :type name: str
   :param parent_folderH: 親フォルダのハンドル（``0`` でルート階層に作成）
   :type parent_folderH: int
   :return: 新しいフォルダのアイテムハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      # ルート階層に作成
      new_folderH = ae.sdk.AEGP_CreateNewFolder("新しいフォルダ", 0)

      # 既存フォルダ内に作成
      sub_folderH = ae.sdk.AEGP_CreateNewFolder("サブフォルダ", parent_folderH)

アイテムの削除
~~~~~~~~~~~~~~

.. function:: AEGP_DeleteItem(itemH: int) -> None

   アイテムを削除します（**Undo可能**）。

   :param itemH: アイテムハンドル
   :type itemH: int

   .. warning::
      この操作は元に戻せません（Undoを除く）。アイテムがコンポジションで使用されている場合、すべての参照が削除されます。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DeleteItem(itemH)

コメントとラベル
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetItemComment(plugin_id: int, itemH: int) -> str

   アイテムのコメントを取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param itemH: アイテムハンドル
   :type itemH: int
   :return: コメント文字列（コメントがない場合は空文字列）
   :rtype: str

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      comment = ae.sdk.AEGP_GetItemComment(plugin_id, itemH)
      print(f"コメント: {comment}")

.. function:: AEGP_SetItemComment(itemH: int, comment: str) -> None

   アイテムのコメントを設定します（**Undo可能**）。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param comment: コメント文字列（空文字列でコメントをクリア）
   :type comment: str

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetItemComment(itemH, "重要なアイテム")

      # コメントをクリア
      ae.sdk.AEGP_SetItemComment(itemH, "")

.. function:: AEGP_GetItemLabel(itemH: int) -> int

   アイテムのラベルカラーを取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: ラベルID（0～16）
   :rtype: int

   **例**:

   .. code-block:: python

      label = ae.sdk.AEGP_GetItemLabel(itemH)
      print(f"ラベル: {label}")

.. function:: AEGP_SetItemLabel(itemH: int, label: int) -> None

   アイテムのラベルカラーを設定します（**Undo可能**）。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param label: ラベルID（0～16）
   :type label: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetItemLabel(itemH, 5)  # ラベル5に設定

プロキシ管理
~~~~~~~~~~~~

.. function:: AEGP_SetItemUseProxy(itemH: int, use_proxy: bool) -> None

   プロキシの使用設定を変更します（**Undo可能**）。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param use_proxy: ``True`` でプロキシを使用、``False`` でオリジナルを使用
   :type use_proxy: bool

   .. warning::
      アイテムにプロキシが設定されていない場合、エラーが発生します。

   **例**:

   .. code-block:: python

      # プロキシが設定されているか確認
      flags = ae.sdk.AEGP_GetItemFlags(itemH)
      if flags & 0x02:  # AEGP_ItemFlag_HAS_PROXY
          ae.sdk.AEGP_SetItemUseProxy(itemH, True)

その他
~~~~~~

.. function:: AEGP_GetItemMRUView(itemH: int) -> int

   アイテムの最近使用されたビューを取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: ビューのポインタ（ビューが利用できない場合は ``0``）
   :rtype: int

   .. note::
      この関数は、コンポジションやフッテージビューアーの参照を取得するために使用されます。

   **例**:

   .. code-block:: python

      viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
      if viewP != 0:
          print("ビューが利用可能です")

使用例
------

プロジェクト内のすべてのアイテムを列挙
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def list_all_items():
       """プロジェクト内のすべてのアイテムを列挙し、情報を表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       print("=" * 70)
       print("プロジェクトアイテム一覧")
       print("=" * 70)

       itemH = ae.sdk.AEGP_GetFirstProjItem(projH)
       count = 0

       while itemH != 0:
           count += 1

           # 基本情報を取得
           name = ae.sdk.AEGP_GetItemName(plugin_id, itemH)
           item_type = ae.sdk.AEGP_GetItemType(itemH)
           type_name = ae.sdk.AEGP_GetTypeName(item_type)
           item_id = ae.sdk.AEGP_GetItemID(itemH)

           print(f"{count}. [{type_name}] {name} (ID: {item_id})")

           # タイプに応じた追加情報
           if item_type in [2, 3, 4]:  # COMP, SOLID, FOOTAGE
               width, height = ae.sdk.AEGP_GetItemDimensions(itemH)
               duration = ae.sdk.AEGP_GetItemDuration(itemH)
               print(f"   サイズ: {width}x{height}, デュレーション: {duration:.2f}秒")

           # 次のアイテムへ
           itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)

       print("=" * 70)
       print(f"合計: {count}アイテム")

   # 実行
   list_all_items()

フォルダ構造の作成と整理
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def organize_project_by_type():
       """アイテムをタイプごとにフォルダに整理"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       # Undoグループ開始
       ae.sdk.AEGP_StartUndoGroup("プロジェクト整理")

       try:
           # フォルダを作成
           comps_folderH = ae.sdk.AEGP_CreateNewFolder("Compositions", 0)
           footage_folderH = ae.sdk.AEGP_CreateNewFolder("Footage", 0)
           solids_folderH = ae.sdk.AEGP_CreateNewFolder("Solids", 0)

           # すべてのアイテムを列挙
           itemH = ae.sdk.AEGP_GetFirstProjItem(projH)
           moved_count = 0

           while itemH != 0:
               next_itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)

               # 親フォルダをチェック（ルート階層のみ処理）
               parent_folderH = ae.sdk.AEGP_GetItemParentFolder(itemH)

               if parent_folderH == 0:
                   item_type = ae.sdk.AEGP_GetItemType(itemH)

                   # タイプに応じてフォルダに移動
                   if item_type == 2:  # AEGP_ItemType_COMP
                       ae.sdk.AEGP_SetItemParentFolder(itemH, comps_folderH)
                       moved_count += 1
                   elif item_type == 3:  # AEGP_ItemType_SOLID
                       ae.sdk.AEGP_SetItemParentFolder(itemH, solids_folderH)
                       moved_count += 1
                   elif item_type == 4:  # AEGP_ItemType_FOOTAGE
                       ae.sdk.AEGP_SetItemParentFolder(itemH, footage_folderH)
                       moved_count += 1

               itemH = next_itemH

           print(f"プロジェクトを整理しました: {moved_count}アイテム移動")

       finally:
           # Undoグループ終了
           ae.sdk.AEGP_EndUndoGroup()

   # 実行
   organize_project_by_type()

アイテムの検索と選択
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def find_and_select_items(search_name):
       """名前でアイテムを検索して選択"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       found_items = []
       itemH = ae.sdk.AEGP_GetFirstProjItem(projH)

       # すべてのアイテムを検索
       while itemH != 0:
           name = ae.sdk.AEGP_GetItemName(plugin_id, itemH)

           if search_name.lower() in name.lower():
               found_items.append((itemH, name))

           itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)

       # 結果を表示
       if found_items:
           print(f"'{search_name}' を含む {len(found_items)} アイテムが見つかりました:")

           for i, (itemH, name) in enumerate(found_items):
               print(f"  {i+1}. {name}")

               # 最初のアイテムを選択（他を選択解除）
               if i == 0:
                   ae.sdk.AEGP_SelectItem(itemH, True, True)
               else:
                   # 複数選択に追加
                   ae.sdk.AEGP_SelectItem(itemH, True, False)

           print(f"{len(found_items)} アイテムを選択しました")
       else:
           print(f"'{search_name}' を含むアイテムが見つかりませんでした")

   # 実行例
   find_and_select_items("comp")

アイテム情報の一括更新
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def batch_update_labels(label_id, target_type=None):
       """指定されたタイプのすべてのアイテムにラベルを設定"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       ae.sdk.AEGP_StartUndoGroup("ラベル一括設定")

       try:
           itemH = ae.sdk.AEGP_GetFirstProjItem(projH)
           updated_count = 0

           while itemH != 0:
               item_type = ae.sdk.AEGP_GetItemType(itemH)

               # タイプフィルタ（指定されていない場合はすべて）
               if target_type is None or item_type == target_type:
                   ae.sdk.AEGP_SetItemLabel(itemH, label_id)
                   updated_count += 1

               itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)

           type_name = ae.sdk.AEGP_GetTypeName(target_type) if target_type else "すべて"
           print(f"{type_name}のラベルを更新しました: {updated_count}アイテム")

       finally:
           ae.sdk.AEGP_EndUndoGroup()

   # 実行例: すべてのコンポジションをラベル5に設定
   batch_update_labels(5, target_type=2)  # 2 = AEGP_ItemType_COMP

プロキシ状態のチェック
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def check_proxy_status():
       """プロジェクト内のプロキシ状態を確認"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)

       proxy_items = []
       using_proxy_items = []
       missing_proxy_items = []

       itemH = ae.sdk.AEGP_GetFirstProjItem(projH)

       while itemH != 0:
           flags = ae.sdk.AEGP_GetItemFlags(itemH)
           name = ae.sdk.AEGP_GetItemName(plugin_id, itemH)

           if flags & 0x02:  # AEGP_ItemFlag_HAS_PROXY
               proxy_items.append(name)

               if flags & 0x04:  # AEGP_ItemFlag_USING_PROXY
                   using_proxy_items.append(name)

               if flags & 0x08:  # AEGP_ItemFlag_MISSING_PROXY
                   missing_proxy_items.append(name)

           itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)

       # レポート表示
       print("=" * 70)
       print("プロキシ状態レポート")
       print("=" * 70)
       print(f"プロキシ設定済み: {len(proxy_items)}アイテム")
       print(f"プロキシ使用中: {len(using_proxy_items)}アイテム")
       print(f"プロキシ欠落: {len(missing_proxy_items)}アイテム")

       if missing_proxy_items:
           print("\n警告: 以下のアイテムのプロキシが見つかりません:")
           for name in missing_proxy_items:
               print(f"  - {name}")

   # 実行
   check_proxy_status()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **アイテムハンドルの有効期限**

   アイテムハンドルは、アイテムが存在する間のみ有効です。削除後は無効になります。

2. **プロジェクトアイテムの列挙順序**

   アイテムは深さ優先順（DFS）で列挙されます。フォルダの子を先に処理してから次のフォルダに進みます。

3. **AEGP_GetNextProjItemの終了条件**

   最後のアイテムの後は ``0`` を返します。これを使ってループを終了します。

4. **アイテム削除時の注意**

   アイテムを削除すると、そのアイテムを使用しているコンポジションからも削除されます。

5. **プロキシ操作の前提条件**

   ``AEGP_SetItemUseProxy`` は、アイテムにプロキシが設定されている場合のみ成功します。
   事前に ``AEGP_ItemFlag_HAS_PROXY`` フラグをチェックしてください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

plugin_idの再利用
^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 良い例: plugin_idを1度だけ取得して再利用
   plugin_id = ae.sdk.AEGP_GetPluginID()

   itemH = ae.sdk.AEGP_GetFirstProjItem(projH)
   while itemH != 0:
       name = ae.sdk.AEGP_GetItemName(plugin_id, itemH)  # 再利用
       itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)

   # 悪い例: 毎回取得
   itemH = ae.sdk.AEGP_GetFirstProjItem(projH)
   while itemH != 0:
       name = ae.sdk.AEGP_GetItemName(ae.sdk.AEGP_GetPluginID(), itemH)  # 無駄
       itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)

アイテム削除時の安全な列挙
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 削除する場合は、次のアイテムを先に取得
   itemH = ae.sdk.AEGP_GetFirstProjItem(projH)

   while itemH != 0:
       next_itemH = ae.sdk.AEGP_GetNextProjItem(projH, itemH)  # 先に取得

       # アイテムを削除
       ae.sdk.AEGP_DeleteItem(itemH)

       itemH = next_itemH  # 次へ

フラグのチェック
^^^^^^^^^^^^^^^^

.. code-block:: python

   flags = ae.sdk.AEGP_GetItemFlags(itemH)

   # ビットマスクでフラグをチェック
   has_proxy = bool(flags & 0x02)     # AEGP_ItemFlag_HAS_PROXY
   using_proxy = bool(flags & 0x04)   # AEGP_ItemFlag_USING_PROXY
   has_video = bool(flags & 0x10)     # AEGP_ItemFlag_HAS_VIDEO
   has_audio = bool(flags & 0x20)     # AEGP_ItemFlag_HAS_AUDIO

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("複数の変更")
   try:
       # 複数の変更を実行
       ae.sdk.AEGP_SetItemName(itemH, "新しい名前")
       ae.sdk.AEGP_SetItemLabel(itemH, 5)
       ae.sdk.AEGP_SetItemComment(itemH, "更新しました")
   finally:
       ae.sdk.AEGP_EndUndoGroup()

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       ae.sdk.AEGP_SetItemUseProxy(itemH, True)
   except RuntimeError as e:
       # プロキシが設定されていない場合のエラー処理
       print(f"プロキシを有効にできませんでした: {e}")

関連項目
--------

- :doc:`AEGP_ProjSuite6` - プロジェクト管理
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`AEGP_FootageSuite5` - フッテージ管理
- :doc:`AEGP_UtilitySuite6` - Undo管理、エラー処理
- :doc:`index` - 低レベルAPI概要
