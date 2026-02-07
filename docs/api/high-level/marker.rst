Marker
======

.. currentmodule:: ae

Markerクラスは、コンポジションやレイヤーに追加するマーカーを表します。

概要
----

``Marker`` クラスは、After Effectsのマーカー操作のためのAPIを提供します。
タイムライン上の特定の位置に配置できる注釈で、
コメント、チャプター情報、URL、キューポイントなどを含めることができます。

**特徴**:

- コメント、チャプター名、URL、キューポイント名の設定
- デュレーションの設定（範囲マーカー）
- ラベルカラーの設定
- 保護領域/ナビゲーションフラグの設定

**基本的なインポート**:

.. code-block:: python

   import ae

   # 新しいマーカーを作成
   marker = ae.Marker()
   marker.comment = "重要なシーン"
   marker.duration = 2.0

   # レイヤーにマーカーを追加
   layer = ae.Comp.get_active().layer(0)
   layer.add_marker(1.5, marker)

クラスリファレンス
------------------

.. class:: Marker

   マーカーを表すクラス。

   タイムライン上の注釈として、コメント、チャプター、URL、キューポイントなどの情報を保持します。

プロパティ一覧
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 25 15 60

   * - プロパティ
     - 型
     - 説明
   * - ``comment``
     - str
     - マーカーコメント（メインテキスト）
   * - ``chapter``
     - str
     - チャプター名（DVD/Blu-ray出力用）
   * - ``url``
     - str
     - URL（Web出力用）
   * - ``frame_target``
     - str
     - フレームターゲット（例: "_blank", "_self"）
   * - ``cue_point_name``
     - str
     - キューポイント名（Flash/ActionScript出力用）
   * - ``duration``
     - float
     - デュレーション（秒）。0より大きい値で範囲マーカーになる
   * - ``label``
     - int
     - ラベルカラー番号（0-16）
   * - ``protected``
     - bool
     - 保護領域フラグ
   * - ``navigation``
     - bool
     - ナビゲーションフラグ

メソッド一覧
~~~~~~~~~~~~

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``__init__()``
     - 新しいマーカーを作成

プロパティ
~~~~~~~~~~

.. attribute:: Marker.comment
   :type: str

   マーカーのコメント（メインテキスト）。

   **例**:

   .. code-block:: python

      marker.comment = "重要なシーン"

.. attribute:: Marker.chapter
   :type: str

   チャプター名。DVD/Blu-ray出力時に使用されます。

   **例**:

   .. code-block:: python

      marker.chapter = "Chapter 1: Introduction"

.. attribute:: Marker.url
   :type: str

   URL。Web出力時にリンク先として使用されます。

   **例**:

   .. code-block:: python

      marker.url = "https://example.com/details"
      marker.frame_target = "_blank"

.. attribute:: Marker.duration
   :type: float

   マーカーのデュレーション（秒）。
   0より大きい値を設定すると、マーカーは範囲として表示されます。

   **例**:

   .. code-block:: python

      marker.duration = 2.0  # 2秒間の範囲マーカー

.. attribute:: Marker.label
   :type: int

   ラベルカラー番号（0-16）。After Effectsのラベルカラーに対応します。

   **例**:

   .. code-block:: python

      marker.label = 5  # ラベルカラー

使用例
------

マーカーの作成と追加
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # 新しいマーカーを作成
   marker = ae.Marker()
   marker.comment = "重要なシーン"
   marker.duration = 2.0
   marker.label = 5

   # レイヤーにマーカーを追加
   layer = ae.Comp.get_active().layer(0)
   layer.add_marker(1.5, marker)  # 1.5秒の位置に追加

チャプターマーカー
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   marker = ae.Marker()
   marker.chapter = "Chapter 1: Introduction"
   marker.comment = "オープニングシーケンス"
   marker.navigation = True

Webマーカー
~~~~~~~~~~~

.. code-block:: python

   import ae

   marker = ae.Marker()
   marker.url = "https://example.com/details"
   marker.frame_target = "_blank"
   marker.comment = "詳細はこちら"

シーンマーカーの一括作成
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   scenes = [
       (0.0, "Intro", 5.0),
       (5.0, "Main Scene", 30.0),
       (35.0, "Outro", 5.0),
   ]

   comp = ae.Comp.get_active()
   layer = comp.layer(0)

   with ae.UndoGroup("Create Scene Markers"):
       for time, name, duration in scenes:
           marker = ae.Marker()
           marker.comment = name
           marker.duration = duration
           marker.label = 3
           layer.add_marker(time, marker)

注意事項
--------

.. note::
   - マーカーは作成後、レイヤーに追加するまで効果がありません
   - ``duration`` を 0 より大きく設定すると、マーカーは範囲として表示されます
   - ``label`` は 0-16 の範囲で指定してください

.. warning::
   - マーカーを追加できるのはレイヤーのみです（コンポジションレベルのマーカーは現在サポートされていません）
   - 無効なレイヤーにマーカーを追加しようとするとエラーが発生します

関連項目
--------

- :doc:`layer` - レイヤークラス
- :doc:`/api/low-level/AEGP_MarkerSuite3` - 低レベルAPI
