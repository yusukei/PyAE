Comp
====

.. currentmodule:: ae

Compクラスは、After Effectsのコンポジションを表します。

概要
----

``Comp`` クラスは、After Effectsのコンポジション操作のための中心的なAPIを提供します。
コンポジションの作成、設定変更、レイヤーの追加・管理、レンダリングなどの操作が可能です。

**特徴**:

- コンポジションの作成と設定変更
- レイヤーの追加・管理
- 時間とワークエリアの制御
- シリアライゼーション（エクスポート/インポート）

**基本的なインポート**:

.. code-block:: python

   import ae

   # アクティブなコンポジションを取得
   comp = ae.Comp.get_active()

クラスリファレンス
------------------

.. class:: Comp

   After Effectsコンポジションを表すクラス。

   コンポジションの作成、レイヤー管理、時間設定などの機能を提供します。

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
     - コンポジションが有効かどうか（読み取り専用）
   * - ``name``
     - str
     - コンポジション名（読み書き可能）
   * - ``width``
     - int
     - 幅（ピクセル、読み取り専用）
   * - ``height``
     - int
     - 高さ（ピクセル、読み取り専用）
   * - ``resolution``
     - Tuple[int, int]
     - 解像度 (width, height)（読み取り専用）
   * - ``pixel_aspect``
     - float
     - ピクセルアスペクト比（読み書き可能）
   * - ``bg_color``
     - Tuple[float, float, float]
     - 背景色 RGB (0.0-1.0)（読み書き可能）
   * - ``duration``
     - float
     - デュレーション（秒）
   * - ``frame_rate``
     - float
     - フレームレート (fps)
   * - ``frame_duration``
     - float
     - 1フレームの継続時間（秒、読み取り専用）
   * - ``current_time``
     - float
     - 現在の時間（CTI位置、秒）
   * - ``display_start_time``
     - float
     - タイムライン表示開始時間（秒）
   * - ``work_area_start``
     - float
     - ワークエリア開始位置（秒）
   * - ``work_area_duration``
     - float
     - ワークエリアのデュレーション（秒）
   * - ``show_layer_names``
     - bool
     - True=レイヤー名表示, False=ソース名表示
   * - ``show_blend_modes``
     - bool
     - タイムラインにブレンドモードを表示
   * - ``display_drop_frame``
     - bool
     - ドロップフレームタイムコード表示
   * - ``shutter_angle_phase``
     - Tuple[float, float]
     - (シャッター角度, 位相)（読み取り専用）
   * - ``motion_blur_samples``
     - int
     - 推奨モーションブラーサンプル数
   * - ``motion_blur_adaptive_sample_limit``
     - int
     - モーションブラー適応サンプル制限
   * - ``num_layers``
     - int
     - レイヤー数（読み取り専用）
   * - ``layers``
     - List[Layer]
     - すべてのレイヤーのリスト（読み取り専用）
   * - ``selected_layers``
     - List[Layer]
     - 選択中のレイヤーのリスト（読み取り専用）
   * - ``color_profile``
     - ColorProfile
     - 作業用カラースペースのプロファイル

メソッド一覧
~~~~~~~~~~~~

静的メソッド
^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``Comp.get_active() -> Optional[Comp]``
     - アクティブなコンポジションを取得
   * - ``Comp.get_most_recently_used() -> Optional[Comp]``
     - 最近使用したコンポジションを取得
   * - ``Comp.create(...) -> Comp``
     - 新しいコンポジションを作成
   * - ``Comp.from_dict(data, parent_folder=None) -> Comp``
     - 辞書からコンポジションを作成

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 55 45

   * - メソッド
     - 説明
   * - ``layer(key) -> Optional[Layer]``
     - インデックス(0-based)または名前でレイヤーを取得
   * - ``layer_by_name(name) -> Optional[Layer]``
     - 名前でレイヤーを取得
   * - ``add_solid(name, width, height, color=(1,1,1), duration=-1) -> Layer``
     - 平面レイヤーを追加
   * - ``add_null(name="Null", duration=-1.0) -> Layer``
     - ヌルオブジェクトを追加
   * - ``add_camera(name="Camera", center=(0,0)) -> Layer``
     - カメラを追加
   * - ``add_light(name="Light", center=(0,0)) -> Layer``
     - ライトを追加
   * - ``add_text(text="") -> Layer``
     - ポイントテキストレイヤーを追加
   * - ``add_box_text(box_width, box_height, horizontal=True) -> Layer``
     - ボックステキストレイヤーを追加
   * - ``add_shape() -> Layer``
     - シェイプレイヤーを追加
   * - ``add_layer(item, duration=-1.0) -> Layer``
     - アイテムからレイヤーを作成
   * - ``set_dimensions(width, height)``
     - コンポジションのサイズを設定
   * - ``duplicate() -> Comp``
     - コンポジションを複製
   * - ``to_dict() -> dict``
     - コンポジションを辞書にエクスポート
   * - ``update_from_dict(data)``
     - 辞書データで既存コンポジションを更新

Pythonプロトコルメソッド
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__repr__()``
     - 文字列表現を返す（例: ``<Comp: 'Name' 1920x1080 @30fps>``）
   * - ``__bool__()``
     - ``valid`` プロパティと同等。有効なら ``True``
   * - ``__eq__(other)``
     - ハンドルベースの等値比較
   * - ``__hash__()``
     - ハンドルベースのハッシュ値を返す

コレクションプロトコル
^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__len__()``
     - レイヤー数を返す（``num_layers`` と同等）
   * - ``__iter__()``
     - レイヤーのイテレーションをサポート
   * - ``__getitem__(key)``
     - int（インデックス）または str（名前）でレイヤーを取得。負数インデックス対応
   * - ``__contains__(name)``
     - 名前でレイヤーの存在を確認

プロパティ
~~~~~~~~~~

.. attribute:: Comp.valid
   :type: bool

   コンポジションが有効かどうか。

   **例**:

   .. code-block:: python

      comp = ae.Comp.get_active()
      if comp and comp.valid:
          print("コンポジションは有効です")

.. attribute:: Comp.duration
   :type: float

   コンポジションのデュレーション（秒）。

   **例**:

   .. code-block:: python

      comp.duration = 30.0  # 30秒に設定

.. attribute:: Comp.frame_rate
   :type: float

   フレームレート (fps)。

   **例**:

   .. code-block:: python

      comp.frame_rate = 24.0  # 24fpsに設定

.. attribute:: Comp.layers
   :type: List[Layer]

   すべてのレイヤーのリスト。

   **例**:

   .. code-block:: python

      for layer in comp.layers:
          print(f"- {layer.name}")

使用例
------

コンポジションの取得
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # アクティブなコンポジションを取得
   comp = ae.Comp.get_active()
   if comp:
       print(f"アクティブコンポ: {comp.name}")

   # 最近使用したコンポジションを取得
   mru_comp = ae.Comp.get_most_recently_used()
   if mru_comp:
       print(f"最近使用: {mru_comp.name}")

コンポジションの作成
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # 新しいコンポジションを作成
   comp = ae.Comp.create(
       name="My Composition",
       width=1920,
       height=1080,
       pixel_aspect=1.0,
       duration=10.0,
       frame_rate=30.0
   )

   print(f"作成: {comp.name} ({comp.width}x{comp.height})")

コンポジション情報の取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()

   # 基本情報
   print(f"名前: {comp.name}")
   print(f"サイズ: {comp.width} x {comp.height}")
   print(f"デュレーション: {comp.duration}秒")
   print(f"フレームレート: {comp.frame_rate} fps")

   # 背景色 (RGB, 0.0-1.0)
   r, g, b = comp.bg_color
   print(f"背景色: R={r}, G={g}, B={b}")

コンポジション設定の変更
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # 名前を変更
   comp.name = "Renamed Composition"

   # デュレーションを変更
   comp.duration = 30.0

   # フレームレートを変更
   comp.frame_rate = 24.0

   # 背景色を変更 (青)
   comp.bg_color = (0.0, 0.0, 1.0)

   # サイズを変更
   comp.set_dimensions(3840, 2160)

レイヤー操作
~~~~~~~~~~~~

.. code-block:: python

   # レイヤー数
   print(f"レイヤー数: {comp.num_layers}")

   # すべてのレイヤーを取得
   for layer in comp.layers:
       print(f"- {layer.name}")

   # インデックスでアクセス (0-based)
   layer = comp.layer(0)  # 最前面のレイヤー

   # 名前でアクセス
   layer = comp.layer("Background")

   # 選択中のレイヤー
   for layer in comp.selected_layers:
       print(f"選択中: {layer.name}")

コレクションプロトコル
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # レイヤー数
   print(len(comp))

   # インデックスでアクセス (0-based)
   layer = comp[0]   # 最前面のレイヤー
   layer = comp[-1]  # 最背面のレイヤー

   # 名前でアクセス
   layer = comp["Background"]

   # レイヤーの存在確認
   if "Background" in comp:
       print("Background レイヤーが存在します")

   # イテレーション
   for layer in comp:
       print(layer)

レイヤーの作成
~~~~~~~~~~~~~~

.. code-block:: python

   # 平面レイヤーを追加
   solid = comp.add_solid(
       name="Red Background",
       width=1920,
       height=1080,
       color=(1.0, 0.0, 0.0),
       duration=5.0
   )

   # ヌルオブジェクトを追加
   null = comp.add_null("Controller")

   # カメラを追加
   camera = comp.add_camera("Main Camera", center=(960, 540))

   # ライトを追加
   light = comp.add_light("Key Light", center=(960, 540))

   # テキストレイヤーを追加
   text = comp.add_text("Hello World")

   # シェイプレイヤーを追加
   shape = comp.add_shape()

シリアライゼーション
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import json

   # コンポジションを辞書にエクスポート
   data = comp.to_dict()

   # JSONとして保存
   with open("composition.json", "w", encoding="utf-8") as f:
       json.dump(data, f, indent=2, ensure_ascii=False)

   # JSONから読み込んで新しいコンポジションを作成
   with open("composition.json", "r", encoding="utf-8") as f:
       data = json.load(f)
   new_comp = ae.Comp.from_dict(data)

   # 既存のコンポジションを辞書データで更新
   comp.update_from_dict(data)

注意事項
--------

.. note::
   - ``comp.layer(index)`` のインデックスは0から始まります
   - レイヤーの順序は、タイムラインの上から下（最前面が0）です
   - ``duration`` や ``current_time`` は秒単位です

.. warning::
   - 無効なコンポジションハンドルを使用するとクラッシュする可能性があります
   - ``valid`` プロパティで有効性を確認することを推奨します

関連項目
--------

- :doc:`layer` - レイヤー
- :doc:`project` - プロジェクト
- :doc:`render_queue` - レンダーキュー
- :doc:`/api/low-level/AEGP_CompSuite12` - 低レベルAPI
