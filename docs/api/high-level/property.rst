Property
========

.. currentmodule:: ae

Propertyクラスは、After Effectsのプロパティ（位置、回転、エフェクトパラメータなど）を表します。

概要
----

``Property`` クラスは、After Effectsのプロパティ操作のためのAPIを提供します。
プロパティ値の取得・設定、キーフレーム操作、エクスプレッションの制御などが可能です。

**特徴**:

- プロパティ値の取得と設定
- キーフレームの追加・削除・編集
- 補間タイプとイージングの制御
- エクスプレッションの設定
- シリアライゼーション（エクスポート/インポート）

**基本的なインポート**:

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layer(0)

   # プロパティを取得
   position = layer.property("ADBE Transform Group/ADBE Position")

クラスリファレンス
------------------

.. class:: Property

   After Effectsプロパティを表すクラス。

   位置、回転、不透明度などのトランスフォームプロパティや、
   エフェクトパラメータにアクセスできます。

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
     - プロパティが有効かどうか（読み取り専用）
   * - ``name``
     - str
     - プロパティ名（読み取り専用）
   * - ``match_name``
     - str
     - マッチ名（固有識別子、読み取り専用）
   * - ``depth``
     - int
     - プロパティ階層の深さ（読み取り専用）
   * - ``stream_value_type``
     - StreamValueType
     - ストリーム値タイプ（読み取り専用）
   * - ``grouping_type``
     - StreamGroupingType
     - グルーピングタイプ（読み取り専用）
   * - ``type``
     - PropertyType
     - プロパティタイプ（読み取り専用）
   * - ``is_group``
     - bool
     - プロパティグループかどうか（読み取り専用）
   * - ``is_leaf``
     - bool
     - リーフ（値を持つ）かどうか（読み取り専用）
   * - ``is_effect_param``
     - bool
     - エフェクトパラメータかどうか（読み取り専用）
   * - ``index``
     - int
     - エフェクトパラメータインデックス（0-based、エフェクトパラメータでない場合は -1、読み取り専用）
   * - ``can_vary_over_time``
     - bool
     - 時間によって変化できるか（読み取り専用）
   * - ``can_have_keyframes``
     - bool
     - キーフレームを持てるか（読み取り専用）
   * - ``can_have_expression``
     - bool
     - エクスプレッションを持てるか（読み取り専用）
   * - ``can_set_value``
     - bool
     - 値を設定できるか（読み取り専用）
   * - ``value``
     - Any
     - プロパティ値（読み書き可能）
   * - ``num_keyframes``
     - int
     - キーフレーム数（読み取り専用）
   * - ``is_time_varying``
     - bool
     - 時間によって変化するか（読み取り専用）
   * - ``keyframes``
     - List[Keyframe]
     - キーフレーム情報のリスト（読み取り専用）
   * - ``has_expression``
     - bool
     - エクスプレッションの有無（読み取り専用）
   * - ``expression``
     - str
     - エクスプレッション文字列（読み書き可能）
   * - ``num_properties``
     - int
     - 子プロパティ数（読み取り専用）
   * - ``properties``
     - List[Property]
     - 子プロパティのリスト（読み取り専用）

メソッド一覧
~~~~~~~~~~~~

値操作
^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``get_value(time=0.0) -> Any``
     - 指定時間の値を取得
   * - ``set_value(value)``
     - 値を設定（全時間に適用）
   * - ``get_value_at_time(time) -> Any``
     - 指定時間の値を取得
   * - ``set_value_at_time(time, value)``
     - 指定時間に値を設定（キーフレーム作成）
   * - ``get_raw_bytes(time=0.0) -> bytes``
     - 生のバイナリデータを取得
   * - ``get_stream_metadata() -> Dict``
     - ストリームメタデータを取得

キーフレーム操作
^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``add_keyframe(time, value) -> int``
     - キーフレームを追加
   * - ``remove_keyframe(index)``
     - キーフレームを削除
   * - ``remove_all_keyframes()``
     - 全キーフレームを削除
   * - ``get_keyframe_time(index) -> float``
     - キーフレーム時間を取得
   * - ``get_keyframe_value(index) -> Any``
     - キーフレーム値を取得
   * - ``set_keyframe_value(index, value)``
     - キーフレーム値を設定
   * - ``get_keyframe_interpolation(index) -> Tuple[str, str]``
     - 補間タイプを取得 (in, out)
   * - ``set_keyframe_interpolation(index, in_type, out_type)``
     - 補間タイプを設定
   * - ``keyframe_time(index) -> float``
     - キーフレーム時間を取得（``get_keyframe_time`` のエイリアス）
   * - ``keyframe_in_interpolation(index) -> str``
     - キーフレーム入力補間タイプを取得
   * - ``keyframe_out_interpolation(index) -> str``
     - キーフレーム出力補間タイプを取得
   * - ``set_temporal_ease(index, ease_in, ease_out)``
     - キーフレームの時間的イーズを設定（ease_in/ease_out は [speed, influence] リスト）

エクスプレッション
^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - メソッド
     - 説明
   * - ``enable_expression(enable)``
     - エクスプレッションの有効/無効切り替え

子プロパティアクセス
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``property(key) -> Optional[Property]``
     - インデックスまたはマッチ名で子プロパティを取得
   * - ``set_name(name)``
     - プロパティ名を変更

コレクションプロトコル
^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__len__() -> int``
     - 子プロパティ数を返す（``num_properties`` と同等、グループプロパティの場合）
   * - ``__contains__(name) -> bool``
     - 名前で子プロパティの存在を確認
   * - ``__bool__() -> bool``
     - プロパティが有効かどうか（``valid`` プロパティと同等）

シリアライゼーション
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - メソッド
     - 説明
   * - ``to_dict() -> Dict``
     - プロパティを辞書にエクスポート
   * - ``update_from_dict(data)``
     - 辞書データでプロパティを更新

プロパティ
~~~~~~~~~~

.. attribute:: Property.valid
   :type: bool

   プロパティが有効かどうか。

   **例**:

   .. code-block:: python

      prop = layer.property("ADBE Transform Group/ADBE Position")
      if prop and prop.valid:
          print(f"プロパティ名: {prop.name}")

.. attribute:: Property.value
   :type: Any

   プロパティの現在値。

   値の型はプロパティタイプによって異なります。

   **例**:

   .. code-block:: python

      # 位置を設定
      position.value = (960, 540)

      # 不透明度を設定
      opacity.value = 50.0

.. attribute:: Property.expression
   :type: str

   エクスプレッション文字列。

   **例**:

   .. code-block:: python

      position.expression = "wiggle(5, 50)"

Keyframe クラス
---------------

.. class:: Keyframe

   キーフレームを表すクラス。

   時間、値、補間タイプ、イージング情報を保持します。

Keyframe プロパティ一覧
~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 30 20 50

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - 有効かどうか
   * - ``time``
     - float
     - 時間（秒）
   * - ``value``
     - Any
     - 値
   * - ``index``
     - int
     - インデックス
   * - ``in_interpolation``
     - KeyInterpolation
     - 入力補間タイプ
   * - ``out_interpolation``
     - KeyInterpolation
     - 出力補間タイプ
   * - ``temporal_ease_in``
     - BezierTangent
     - テンポラルイーズイン
   * - ``temporal_ease_out``
     - BezierTangent
     - テンポラルイーズアウト
   * - ``spatial_tangent_in``
     - SpatialTangent
     - 空間タンジェントイン
   * - ``spatial_tangent_out``
     - SpatialTangent
     - 空間タンジェントアウト
   * - ``roving``
     - bool
     - ローヴィング
   * - ``temporal_continuous``
     - bool
     - テンポラル連続
   * - ``temporal_auto_bezier``
     - bool
     - テンポラル自動ベジェ

Keyframe メソッド一覧
~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 55 45

   * - メソッド
     - 説明
   * - ``set_interpolation(in_type, out_type)``
     - 補間タイプを設定
   * - ``set_easing(easing_type)``
     - イージングを設定
   * - ``set_temporal_ease_in(ease)``
     - テンポラルイーズインを設定
   * - ``set_temporal_ease_out(ease)``
     - テンポラルイーズアウトを設定
   * - ``set_spatial_tangents(in, out)``
     - 空間タンジェントを設定

補助クラス
----------

BezierTangent
~~~~~~~~~~~~~

ベジェタンジェント（テンポラルイージング用）。

.. list-table::
   :header-rows: 1
   :widths: 25 15 60

   * - 属性
     - 型
     - 説明
   * - ``speed``
     - float
     - 速度
   * - ``influence``
     - float
     - 影響度（デフォルト: 33.33）

SpatialTangent
~~~~~~~~~~~~~~

空間タンジェント（モーションパス用）。

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - 属性
     - 型
     - 説明
   * - ``x``
     - float
     - X成分
   * - ``y``
     - float
     - Y成分
   * - ``z``
     - float
     - Z成分

KeyInterpolation 列挙型
-----------------------

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - 値
     - 説明
   * - ``LINEAR``
     - リニア補間
   * - ``BEZIER``
     - ベジェ補間
   * - ``HOLD``
     - ホールド（一定値）

使用例
------

プロパティの基本操作
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layer(0)

   # プロパティを取得
   position = layer.property("ADBE Transform Group/ADBE Position")

   # 値を設定
   position.value = (960, 540)

   # 値を取得
   current_pos = position.value
   print(f"現在の位置: {current_pos}")

キーフレーム操作
~~~~~~~~~~~~~~~~

.. code-block:: python

   # キーフレームを追加
   position.add_keyframe(0.0, (100, 540))
   position.add_keyframe(3.0, (1820, 540))
   position.add_keyframe(6.0, (960, 100))

   # キーフレーム数
   print(f"キーフレーム数: {position.num_keyframes}")

   # すべてのキーフレームを取得
   for kf in position.keyframes:
       print(f"時間: {kf.time}, 値: {kf.value}")

キーフレーム補間
~~~~~~~~~~~~~~~~

.. code-block:: python

   # イージングを設定
   position.set_keyframe_interpolation(
       index=0,
       in_type="bezier",
       out_type="bezier"
   )

   # リニア補間
   position.set_keyframe_interpolation(0, "linear", "linear")

   # ホールド
   position.set_keyframe_interpolation(0, "hold", "hold")

エクスプレッション
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # エクスプレッションを設定
   position.expression = "wiggle(5, 50)"

   # エクスプレッションを有効化
   position.enable_expression(True)

   # エクスプレッションを取得
   expr = position.expression
   print(f"エクスプレッション: {expr}")

プロパティグループ
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # トランスフォームグループを取得
   transform = layer.property("ADBE Transform Group")

   # 子プロパティにアクセス
   position = transform.property("ADBE Position")
   scale = transform.property("ADBE Scale")
   rotation = transform.property("ADBE Rotate Z")

   # すべての子プロパティをループ
   for prop in transform.properties:
       print(f"- {prop.name}")

シリアライゼーション
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import json

   # プロパティを辞書にエクスポート
   data = position.to_dict()

   # JSONとして保存
   with open("position.json", "w", encoding="utf-8") as f:
       json.dump(data, f, indent=2, ensure_ascii=False)

   # JSONから読み込んでプロパティを更新
   with open("position.json", "r", encoding="utf-8") as f:
       data = json.load(f)
   position.update_from_dict(data)

データ分類
----------

構造的分類（StreamGroupingType）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

プロパティの階層構造における役割を定義します。
``property.grouping_type`` で取得できます。

.. list-table::
   :header-rows: 1
   :widths: 30 10 60

   * - タイプ
     - 子要素
     - 例
   * - ``LEAF``
     - なし
     - 不透明度、位置、スライダー値など（値を持つ末端プロパティ）
   * - ``NAMED_GROUP``
     - あり
     - トランスフォーム、マテリアルオプション、各エフェクト
   * - ``INDEXED_GROUP``
     - あり
     - エフェクト一覧、マスク一覧、テキストアニメーター

値の型分類（StreamValueType）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

プロパティが保持するデータの型を定義します。
``property.stream_value_type`` で取得できます。

.. list-table::
   :header-rows: 1
   :widths: 22 8 8 8 8 46

   * - タイプ
     - 次元
     - KF
     - Expr
     - Set
     - 説明
   * - ``NO_DATA``
     - 0
     - --
     - --
     - --
     - プロパティグループ等（値を持たない）
   * - ``OneD``
     - 1
     - 可
     - 可
     - 可
     - 不透明度、回転、スライダーなど
   * - ``TwoD``
     - 2
     - 可
     - 可
     - 可
     - 2次元数値
   * - ``TwoD_SPATIAL``
     - 2
     - 可
     - 可
     - 可
     - 2D位置、アンカーポイント
   * - ``ThreeD``
     - 3
     - 可
     - 可
     - 可
     - スケール、方向
   * - ``ThreeD_SPATIAL``
     - 3
     - 可
     - 可
     - 可
     - 3D位置
   * - ``COLOR``
     - 4
     - 可
     - 可
     - 可
     - カラー (RGBA)
   * - ``ARB``
     - --
     - 条件付
     - 条件付
     - 不可
     - エフェクト固有のカスタムデータ
   * - ``MARKER``
     - --
     - --
     - --
     - 不可
     - マーカー
   * - ``LAYER_ID``
     - --
     - --
     - --
     - 不可
     - レイヤー参照
   * - ``MASK_ID``
     - --
     - --
     - --
     - 不可
     - マスク参照
   * - ``MASK``
     - --
     - 可
     - 不可
     - 不可
     - マスク形状（ベジェ曲線）
   * - ``TEXT_DOCUMENT``
     - --
     - 可
     - 不可
     - 不可
     - テキスト（文字列+スタイル）

**凡例**: KF = キーフレーム、Expr = エクスプレッション、Set = ``AEGP_SetStreamValue``

条件付きプロパティの詳細
^^^^^^^^^^^^^^^^^^^^^^^^

**ARB（Arbitrary Data）**:

エフェクトプラグインが定義するカスタムデータ型です。
キーフレームはエフェクトが ``PF_Arbitrary_INTERP_FUNC`` を実装している場合のみ可能です。
``AEGP_SetStreamValue`` は使用不可で、値の取得は ``get_raw_bytes()`` でバイナリとして行います。
代表例: カラーグラデーション、パス/シェイプデータ、Hue/Saturation等のエフェクト固有データ

**MASK（マスク形状）**:

マスクのパスデータ（ベジェ曲線の集合）です。
キーフレームによるアニメーションは可能ですが、エクスプレッションは設定不可です。
``AEGP_SetStreamValue`` は使用不可で、``get_raw_bytes()`` で頂点データをバイナリとして取得します。

**TEXT_DOCUMENT（テキストドキュメント）**:

テキストレイヤーのソーステキスト（文字列+スタイル情報）です。
ホールドキーフレームのみ可能（補間なし）。エクスプレッションは通常のストリームでは不可です。
値の設定は ``set_source_text()`` で行います。

プリミティブ型 vs 非プリミティブ型
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``AEGP_SetStreamValue`` で値を直接設定できるのはプリミティブ型のみです。
``property.can_set_value`` で判定できます。

.. list-table::
   :header-rows: 1
   :widths: 20 40 40

   * - 分類
     - タイプ
     - 値の設定
   * - プリミティブ型
     - OneD, TwoD, TwoD_SPATIAL, ThreeD, ThreeD_SPATIAL, COLOR
     - ``property.value = ...`` で直接設定可能
   * - 非プリミティブ型
     - ARB, MARKER, LAYER_ID, MASK_ID, MASK, TEXT_DOCUMENT
     - 専用API（``set_source_text()``, ``add_marker()`` 等）が必要

プロパティカテゴリ
------------------

標準レイヤープロパティ
~~~~~~~~~~~~~~~~~~~~~~

すべてのレイヤーに共通のプロパティです。

**トランスフォームグループ** (``ADBE Transform Group``):

.. list-table::
   :header-rows: 1
   :widths: 35 20 45

   * - Match Name
     - 型
     - 説明
   * - ``ADBE Anchor Point``
     - ThreeD
     - アンカーポイント
   * - ``ADBE Position``
     - ThreeD_SPATIAL
     - 位置
   * - ``ADBE Scale``
     - ThreeD
     - スケール
   * - ``ADBE Orientation``
     - ThreeD
     - 方向（3Dレイヤーのみ）
   * - ``ADBE Rotate X``
     - OneD
     - X回転（3Dレイヤーのみ）
   * - ``ADBE Rotate Y``
     - OneD
     - Y回転（3Dレイヤーのみ）
   * - ``ADBE Rotate Z``
     - OneD
     - 回転 / Z回転
   * - ``ADBE Opacity``
     - OneD
     - 不透明度

**その他の標準グループ**:

.. list-table::
   :header-rows: 1
   :widths: 40 20 40

   * - Match Name
     - 構造
     - 説明
   * - ``ADBE Audio Group``
     - NAMED_GROUP
     - オーディオ設定
   * - ``ADBE Material Options Group``
     - NAMED_GROUP
     - マテリアルオプション（3Dレイヤーのみ）
   * - ``ADBE Layer Styles``
     - INDEXED_GROUP
     - レイヤースタイル

エフェクトプロパティ
~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 35 20 45

   * - Match Name
     - 構造
     - 説明
   * - ``ADBE Effect Parade``
     - INDEXED_GROUP
     - エフェクト一覧
   * - *(エフェクト名)*
     - NAMED_GROUP
     - 各エフェクト
   * - *(パラメータ)*
     - LEAF
     - エフェクトパラメータ

エフェクトパラメータは ``PF_ParamType`` で定義され、一部は ``AEGP_StreamType_ARB`` として扱われます。

マスクプロパティ
~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 35 20 45

   * - Match Name
     - 型
     - 説明
   * - ``ADBE Mask Parade``
     - INDEXED_GROUP
     - マスク一覧
   * - ``ADBE Mask Atom``
     - NAMED_GROUP
     - 各マスク
   * - ``ADBE Mask Shape``
     - MASK
     - マスク形状
   * - ``ADBE Mask Feather``
     - OneD
     - マスクのぼかし
   * - ``ADBE Mask Opacity``
     - OneD
     - マスクの不透明度
   * - ``ADBE Mask Offset``
     - OneD
     - マスクの拡張

テキストプロパティ
~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 35 20 45

   * - Match Name
     - 型
     - 説明
   * - ``ADBE Text Layer``
     - NAMED_GROUP
     - テキストレイヤールート
   * - ``ADBE Text Properties``
     - NAMED_GROUP
     - テキストプロパティ
   * - ``ADBE Text Document``
     - TEXT_DOCUMENT
     - ソーステキスト
   * - ``ADBE Text Path Options``
     - NAMED_GROUP
     - パスオプション
   * - ``ADBE Text More Options``
     - NAMED_GROUP
     - その他のオプション
   * - ``ADBE Text Animators``
     - INDEXED_GROUP
     - テキストアニメーター

シェイプレイヤープロパティ
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 40 20 40

   * - Match Name
     - 型
     - 説明
   * - ``ADBE Root Vectors Group``
     - INDEXED_GROUP
     - Contents
   * - ``ADBE Vector Group``
     - NAMED_GROUP
     - グループ
   * - ``ADBE Vector Shape - Group``
     - NAMED_GROUP
     - パス
   * - ``ADBE Vector Shape``
     - ARB
     - パスデータ
   * - ``ADBE Vector Graphic - Fill``
     - NAMED_GROUP
     - 塗り
   * - ``ADBE Vector Graphic - Stroke``
     - NAMED_GROUP
     - 線
   * - ``ADBE Vector Graphic - G-Fill``
     - NAMED_GROUP
     - グラデーション塗り
   * - ``ADBE Vector Graphic - G-Stroke``
     - NAMED_GROUP
     - グラデーション線
   * - ``ADBE Vector Filter - Repeater``
     - NAMED_GROUP
     - リピーター

カメラ/ライトプロパティ
~~~~~~~~~~~~~~~~~~~~~~~

**カメラ** (``ADBE Camera Options Group``):

.. list-table::
   :header-rows: 1
   :widths: 40 15 45

   * - Match Name
     - 型
     - 説明
   * - ``ADBE Camera Zoom``
     - OneD
     - ズーム
   * - ``ADBE Camera Depth of Field``
     - OneD
     - 被写界深度
   * - ``ADBE Camera Focus Distance``
     - OneD
     - フォーカス距離
   * - ``ADBE Camera Aperture``
     - OneD
     - 絞り
   * - ``ADBE Camera Blur Level``
     - OneD
     - ブラーレベル

**ライト** (``ADBE Light Options Group``):

.. list-table::
   :header-rows: 1
   :widths: 40 15 45

   * - Match Name
     - 型
     - 説明
   * - ``ADBE Light Intensity``
     - OneD
     - 強度
   * - ``ADBE Light Color``
     - COLOR
     - 色
   * - ``ADBE Light Cone Angle``
     - OneD
     - 円錐角度
   * - ``ADBE Light Cone Feather``
     - OneD
     - 円錐のぼかし
   * - ``ADBE Light Shadow Darkness``
     - OneD
     - シャドウの暗さ

プロパティ特性の判定
--------------------

SDK API と Property クラスの対応
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 35 30 35

   * - SDK API
     - Property 属性
     - 用途
   * - ``AEGP_CanVaryOverTime()``
     - ``can_vary_over_time``
     - エクスプレッション/キーフレーム取得前の事前チェック
   * - ``AEGP_GetStreamNumKFs()``
     - ``num_keyframes``
     - キーフレーム取得可否（-1 = 不可）
   * - ``AEGP_IsStreamTimevarying()``
     - ``is_time_varying``
     - 現在アニメーション中か
   * - ``AEGP_GetStreamType()``
     - ``stream_value_type``
     - 値の型取得
   * - ``AEGP_GetStreamGroupingType()``
     - ``grouping_type``
     - 構造的分類取得
   * - *（PyAE内部で判定）*
     - ``can_have_expression``
     - エクスプレッション設定可否
   * - *（PyAE内部で判定）*
     - ``can_set_value``
     - ``AEGP_SetStreamValue`` 使用可否

判定フローチャート
~~~~~~~~~~~~~~~~~~

プロパティアクセス時の判定フロー::

   StreamRefH
       │
       ▼
   grouping_type を確認
       │
   ┌───┴───────────┬──────────────┐
   │               │              │
   LEAF         NAMED_GROUP   INDEXED_GROUP
   │               │              │
   │         子プロパティ     子プロパティ
   │         をイテレート     をイテレート
   │
   ▼
   can_vary_over_time を確認
       │
   ┌───┴───┐
   True    False → 静的値のみ
   │
   ▼
   stream_value_type を確認
       │
   ┌───┴──────────────┐
   プリミティブ型       非プリミティブ型
   (OneD〜COLOR)      (ARB, MASK, TEXT等)
   │                   │
   value で直接設定    専用APIで操作

注意事項
--------

.. note::
   - プロパティはマッチ名で指定することを推奨（例: ``ADBE Position``）
   - キーフレームインデックスは0から始まります
   - ``can_set_value`` が ``False`` のプロパティには値を設定できません

.. warning::
   - 無効なプロパティへのアクセスはエラーを引き起こします
   - エクスプレッションのエラーはAEの警告として表示されます

関連項目
--------

- :doc:`layer` - レイヤー
- :doc:`effect` - エフェクト
- :doc:`serialize` - シリアライゼーション（bdata形式、特殊プロパティ処理）
- :doc:`enums` - 列挙型（StreamValueType, StreamGroupingType）
- :doc:`/api/low-level/AEGP_StreamSuite6` - 低レベルストリームAPI
- :doc:`/api/low-level/AEGP_KeyframeSuite5` - 低レベルキーフレームAPI
