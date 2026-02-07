Render Queue
============

.. currentmodule:: ae

RenderQueueItemクラスは、レンダーキューアイテムを表します。

概要
----

``RenderQueueItem`` クラスと ``OutputModule`` クラスは、After Effectsのレンダリング設定と出力を管理します。
コンポジションのレンダーキューへの追加、出力モジュールの設定、レンダリングステータスの監視などが可能です。

**特徴**:

- レンダーキューアイテムの追加・削除・取得
- 出力モジュールの設定と管理
- レンダリングステータスの監視
- 出力パスの設定

**基本的なインポート**:

.. code-block:: python

   import ae
   import ae.render_queue as rq

   # レンダーキュー内のアイテムを取得
   items = rq.items()

クラスリファレンス
------------------

.. class:: RenderQueueItem

   レンダーキューアイテムを表すクラス。

   レンダリング対象のコンポジションと出力設定を管理します。

プロパティ一覧
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 30 20 50

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - アイテムが有効かどうか（読み取り専用）
   * - ``comp_name``
     - str
     - コンポジション名（読み取り専用）
   * - ``status``
     - RenderStatus
     - レンダーステータス（読み取り専用）
   * - ``queued``
     - bool
     - 待機中かどうか（読み書き可能）
   * - ``num_output_modules``
     - int
     - 出力モジュール数（読み取り専用）

メソッド一覧
~~~~~~~~~~~~

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``output_module(index=0) -> Optional[OutputModule]``
     - インデックスで出力モジュールを取得
   * - ``delete()``
     - レンダーキューアイテムを削除

プロトコルメソッド
^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__repr__() -> str``
     - ``<RenderQueueItem: 'CompName'>`` 形式の文字列表現
   * - ``__bool__() -> bool``
     - アイテムが有効かどうか（``valid`` と同等）

プロパティ
~~~~~~~~~~

.. attribute:: RenderQueueItem.valid
   :type: bool

   アイテムが有効かどうか。

   削除されたアイテムは ``False`` になります。

   **例**:

   .. code-block:: python

      item = rq.item(0)
      if item and item.valid:
          print(f"コンポ名: {item.comp_name}")

.. attribute:: RenderQueueItem.queued
   :type: bool

   レンダリング待機状態。

   ``True`` でレンダリング待機中、``False`` で待機解除。

   **例**:

   .. code-block:: python

      item.queued = True  # レンダリング待機状態に設定

OutputModule クラス
-------------------

.. class:: OutputModule

   出力モジュールを表すクラス。

   出力ファイルパスや埋め込みオプションを設定します。

OutputModule プロパティ一覧
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 25 15 60

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - 有効かどうか（読み取り専用）
   * - ``file_path``
     - str
     - 出力ファイルパス（読み書き可能）
   * - ``embed_options``
     - EmbedOptions
     - 埋め込みオプション（読み書き可能）

OutputModule プロトコルメソッド
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__repr__() -> str``
     - ``<OutputModule: 'path'>`` 形式の文字列表現
   * - ``__bool__() -> bool``
     - モジュールが有効かどうか（``valid`` と同等）

モジュール関数
--------------

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - 関数
     - 説明
   * - ``ae.render_queue.items() -> List[RenderQueueItem]``
     - レンダーキュー内のすべてのアイテムを取得
   * - ``ae.render_queue.num_items() -> int``
     - レンダーキュー内のアイテム数を取得
   * - ``ae.render_queue.item(index) -> Optional[RenderQueueItem]``
     - インデックスでアイテムを取得（0から開始）
   * - ``ae.render_queue.add_comp(comp, path="") -> Optional[RenderQueueItem]``
     - コンポジションをレンダーキューに追加
   * - ``ae.render_queue.render_settings_templates() -> List[str]``
     - レンダー設定テンプレート名リストを取得
   * - ``ae.render_queue.output_module_templates() -> List[str]``
     - 出力モジュールテンプレート名リストを取得

RenderStatus 列挙型
-------------------

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - 値
     - 説明
   * - ``Unqueued``
     - 待機解除
   * - ``Queued``
     - レンダリング待機中
   * - ``Rendering``
     - レンダリング中
   * - ``Stopped``
     - 停止
   * - ``Done``
     - 完了

EmbedOptions 列挙型
-------------------

.. list-table::
   :header-rows: 1
   :widths: 25 10 65

   * - 値
     - 数値
     - 説明
   * - ``None_``
     - 0
     - 埋め込みなし
   * - ``Link``
     - 1
     - リンクとして埋め込み
   * - ``All``
     - 2
     - すべて埋め込み

使用例
------

レンダーキューアイテムの取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.render_queue as rq

   # レンダーキュー内のアイテム数
   count = rq.num_items()
   print(f"レンダーキューアイテム数: {count}")

   # すべてのアイテムを取得
   for item in rq.items():
       print(f"コンポ: {item.comp_name}, ステータス: {item.status}")

   # インデックスで取得
   if count > 0:
       first_item = rq.item(0)
       print(f"最初のアイテム: {first_item.comp_name}")

レンダーキューへの追加
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.render_queue as rq

   # アクティブなコンポジションを取得
   comp = ae.Comp.get_active()

   # レンダーキューに追加
   rq_item = rq.add_comp(comp)
   print(f"追加: {rq_item.comp_name}")

   # 出力パスを指定して追加
   rq_item = rq.add_comp(comp, path="C:/output/render.mov")

出力パスの設定
~~~~~~~~~~~~~~

.. code-block:: python

   import ae.render_queue as rq

   item = rq.item(0)
   output = item.output_module(0)

   if output and output.valid:
       # 出力ファイルパスを設定
       output.file_path = "C:/output/my_render.mov"
       print(f"出力先: {output.file_path}")

待機状態の設定
~~~~~~~~~~~~~~

.. code-block:: python

   import ae.render_queue as rq

   # レンダリング待機状態に設定
   item = rq.item(0)
   item.queued = True

   # 待機解除
   item.queued = False

レンダリングワークフローの自動化
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.render_queue as rq
   import os

   def setup_render(comp_name: str, output_dir: str, format_ext: str = "mov"):
       """コンポジションのレンダリングをセットアップ"""
       project = ae.Project.get_current()

       # コンポジションを検索
       comp = project.item_by_name(comp_name)
       if not comp:
           print(f"コンポジション '{comp_name}' が見つかりません")
           return None

       # レンダーキューに追加
       rq_item = rq.add_comp(comp)

       # 出力パスを設定
       output = rq_item.output_module(0)
       if output:
           filename = f"{comp_name}.{format_ext}"
           output.file_path = os.path.join(output_dir, filename)

       # 待機状態に設定
       rq_item.queued = True

       print(f"セットアップ完了: {comp_name}")
       return rq_item

   # 使用例
   setup_render("Main Comp", "C:/output", "mov")

注意事項
--------

.. note::
   - インデックスは0から始まります
   - 出力パスには絶対パスを使用することを推奨します
   - ファイル形式は出力モジュールのテンプレートに依存します

.. warning::
   - 無効なアイテムやモジュールを操作するとエラーが発生します
   - 操作前に ``valid`` プロパティを確認してください
   - レンダリング中のアイテムを削除すると予期しない動作が発生する可能性があります

関連項目
--------

- :doc:`comp` - コンポジション
- :doc:`project` - プロジェクト
- :doc:`/api/low-level/AEGP_RenderQueueSuite1` - 低レベルレンダーキューAPI
- :doc:`/api/low-level/AEGP_RQItemSuite4` - レンダーキューアイテムAPI
- :doc:`/api/low-level/AEGP_OutputModuleSuite4` - 出力モジュールAPI
