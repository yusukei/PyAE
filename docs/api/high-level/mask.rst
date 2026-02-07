Mask
====

.. currentmodule:: ae

Maskクラスは、レイヤーに適用されたマスクを表します。

概要
----

``Mask`` クラスは、After Effectsのマスク操作のためのAPIを提供します。
マスクの作成、形状の変更、プロパティの設定などが可能です。

**特徴**:

- マスク形状の作成と編集
- マスクモード、反転、フェザーの設定
- ベジェパスの頂点操作
- シリアライゼーション（エクスポート/インポート）

**基本的なインポート**:

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layer(0)

   # マスクを追加
   vertices = [(100, 100), (500, 100), (500, 500), (100, 500)]
   mask = layer.add_mask(vertices)

クラスリファレンス
------------------

.. class:: Mask

   レイヤーに適用されたマスクを表すクラス。

   マスクの形状、モード、各種プロパティにアクセスできます。

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
     - マスクが有効かどうか（読み取り専用）
   * - ``name``
     - str
     - マスク名（読み取り専用）
   * - ``index``
     - int
     - マスクインデックス（0から開始、読み取り専用）
   * - ``mask_id``
     - int
     - マスクID（読み取り専用）
   * - ``mode``
     - MaskMode
     - マスクモード（読み書き可能）
   * - ``inverted``
     - bool
     - 反転（読み書き可能）
   * - ``locked``
     - bool
     - ロック状態（読み書き可能）
   * - ``is_roto_bezier``
     - bool
     - RotoBezier状態（読み書き可能）
   * - ``opacity``
     - float
     - 不透明度 0-100（読み書き可能）
   * - ``feather``
     - Tuple[float, float]
     - フェザー (x, y)（読み書き可能）
   * - ``color``
     - Tuple[float, float, float]
     - マスク色 RGB 0.0-1.0（読み書き可能）
   * - ``expansion``
     - float
     - 拡張（ピクセル、読み書き可能）
   * - ``outline``
     - int
     - マスクアウトラインハンドル（読み取り専用）

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
     - マスクを削除
   * - ``duplicate() -> Mask``
     - マスクを複製
   * - ``set_feather(x, y)``
     - フェザーを設定
   * - ``set_color(red, green, blue)``
     - マスク色を設定（RGB 0.0-1.0）

Pythonプロトコルメソッド
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__repr__()``
     - 文字列表現を返す（例: ``<Mask: 'Name' index=0>``）
   * - ``__bool__()``
     - ``valid`` プロパティと同等。有効なら ``True``

プロパティ
~~~~~~~~~~

.. attribute:: Mask.valid
   :type: bool

   マスクが有効かどうか。

   **例**:

   .. code-block:: python

      if mask.valid:
          print(f"マスク名: {mask.name}")

.. attribute:: Mask.mode
   :type: MaskMode

   マスクモード。

   **例**:

   .. code-block:: python

      from ae import MaskMode
      mask.mode = MaskMode.Add

.. attribute:: Mask.opacity
   :type: float

   マスクの不透明度（0-100）。

   **例**:

   .. code-block:: python

      mask.opacity = 75.0

.. attribute:: Mask.feather
   :type: Tuple[float, float]

   マスクのフェザー (x, y)。プロパティで直接設定可能。``set_feather()`` メソッドでも設定できます。

   **例**:

   .. code-block:: python

      # プロパティで設定
      mask.feather = (10.0, 10.0)

      # 取得
      fx, fy = mask.feather

.. attribute:: Mask.color
   :type: Tuple[float, float, float]

   マスクの色 (R, G, B)。値は 0.0-1.0 の範囲。プロパティで直接設定可能。``set_color()`` メソッドでも設定できます。

   **例**:

   .. code-block:: python

      # プロパティで設定
      mask.color = (1.0, 0.0, 0.0)  # 赤

      # 取得
      r, g, b = mask.color

MaskVertex クラス
-----------------

.. class:: MaskVertex

   マスクの頂点を表すクラス。

   頂点の位置とベジェタンジェントの情報を保持します。

MaskVertex 属性一覧
~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 25 15 60

   * - 属性
     - 型
     - 説明
   * - ``x``
     - float
     - X座標
   * - ``y``
     - float
     - Y座標
   * - ``in_tangent_x``
     - float
     - 入力タンジェントX
   * - ``in_tangent_y``
     - float
     - 入力タンジェントY
   * - ``out_tangent_x``
     - float
     - 出力タンジェントX
   * - ``out_tangent_y``
     - float
     - 出力タンジェントY

MaskMode 列挙型
---------------

.. list-table::
   :header-rows: 1
   :widths: 25 10 65

   * - 値
     - 数値
     - 説明
   * - ``None_``
     - 0
     - なし
   * - ``ADD``
     - 1
     - 追加
   * - ``SUBTRACT``
     - 2
     - 減算
   * - ``INTERSECT``
     - 3
     - 交差
   * - ``LIGHTEN``
     - 4
     - 比較（明）
   * - ``DARKEN``
     - 5
     - 比較（暗）
   * - ``DIFFERENCE``
     - 6
     - 差分

使用例
------

マスクの作成
~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layer(0)

   # 矩形マスクを追加
   vertices = [(100, 100), (500, 100), (500, 500), (100, 500)]
   mask = layer.add_mask(vertices)
   mask.name = "MyMask"

マスクの設定
~~~~~~~~~~~~

.. code-block:: python

   from ae import MaskMode

   # マスクモードを設定
   mask.mode = MaskMode.Subtract

   # 反転
   mask.inverted = True

   # フェザー（プロパティで直接設定）
   mask.feather = (10.0, 10.0)

   # フェザー（メソッドでも設定可能）
   mask.set_feather(10.0, 10.0)

   # マスク色（プロパティで直接設定）
   mask.color = (1.0, 0.0, 0.0)

   # マスク色（メソッドでも設定可能）
   mask.set_color(1.0, 0.0, 0.0)

   # 拡張
   mask.expansion = 5.0

   # 不透明度
   mask.opacity = 75.0

マスクの取得
~~~~~~~~~~~~

.. code-block:: python

   # インデックスで取得
   mask = layer.mask(0)

   # 名前で取得
   mask = layer.mask("MyMask")

   # すべてのマスクをループ
   for mask in layer.masks:
       print(f"- {mask.name}")

マスクの削除
~~~~~~~~~~~~

.. code-block:: python

   # マスクを削除
   mask.delete()

   # すべてのマスクを削除
   while layer.num_masks > 0:
       layer.mask(0).delete()

注意事項
--------

.. note::
   - ``index`` は 0 から始まります
   - フェザーは X と Y の 2 つの値を持ちます
   - 色は RGB 0.0-1.0 の範囲で指定します

.. warning::
   - 削除されたマスクへの参照は無効になります
   - ``valid`` プロパティで有効性を確認することを推奨します

関連項目
--------

- :doc:`layer` - レイヤー
- :doc:`property` - プロパティ
- :doc:`/api/low-level/AEGP_MaskSuite6` - 低レベルAPI
