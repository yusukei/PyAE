Layer
=====

.. currentmodule:: ae

Layerクラスは、After Effectsのレイヤーを表します。

概要
----

``Layer`` クラスは、After Effectsのレイヤー操作のための包括的なAPIを提供します。
トランスフォーム、エフェクト、マスク、マーカーなどの操作が可能です。

**特徴**:

- トランスフォーム（位置、スケール、回転など）の制御
- エフェクトとマスクの追加・管理
- 親子関係の設定
- プロパティへのアクセスとキーフレームの操作

**基本的なインポート**:

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layer(0)  # 最前面のレイヤー

クラスリファレンス
------------------

.. class:: Layer

   After Effectsレイヤーを表すクラス。

   レイヤーの作成、変更、アニメーションなどの機能を提供します。

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
     - レイヤーが有効かどうか（読み取り専用）
   * - ``name``
     - str
     - レイヤー名（読み書き可能）
   * - ``comment``
     - str
     - コメント（読み書き可能）
   * - ``index``
     - int
     - レイヤーインデックス（0から開始）
   * - ``id``
     - int
     - レイヤーID（読み取り専用）
   * - ``source_item_id``
     - int
     - ソースアイテムID（読み取り専用）
   * - ``object_type``
     - int
     - オブジェクトタイプ（読み取り専用）
   * - ``label``
     - int
     - ラベルカラーID（-1=NONE, 0=NO_LABEL, 1-16=カラー）
   * - ``quality``
     - LayerQuality
     - レイヤー品質（LayerQuality enum）
   * - ``sampling_quality``
     - SamplingQuality
     - サンプリング品質（SamplingQuality enum）
   * - ``in_point``
     - float
     - インポイント（秒）
   * - ``out_point``
     - float
     - アウトポイント（秒）
   * - ``start_time``
     - float
     - スタートタイム（秒）
   * - ``duration``
     - float
     - デュレーション（秒、読み取り専用）
   * - ``stretch``
     - Tuple[int, int]
     - タイムストレッチ比率 (numerator, denominator)
   * - ``current_time``
     - float
     - 現在の時刻（秒、読み取り専用）
   * - ``enabled``
     - bool
     - 有効/無効（ビデオスイッチ）
   * - ``audio_enabled``
     - bool
     - オーディオ有効/無効
   * - ``solo``
     - bool
     - ソロ状態
   * - ``locked``
     - bool
     - ロック状態
   * - ``shy``
     - bool
     - シャイ状態
   * - ``selected``
     - bool
     - 選択状態（読み書き可能）
   * - ``collapse_transformation``
     - bool
     - コラップス/連続ラスタライズ
   * - ``time_remapping_enabled``
     - bool
     - タイムリマップ有効/無効
   * - ``position``
     - Tuple[float, ...]
     - 位置（2Dまたは3D）
   * - ``scale``
     - Tuple[float, ...]
     - スケール（%）
   * - ``rotation``
     - float
     - 回転（度）
   * - ``anchor_point``
     - Tuple[float, ...]
     - アンカーポイント
   * - ``opacity``
     - float
     - 不透明度（0-100）
   * - ``transfer_mode``
     - TransferMode
     - 転送モード（ブレンドモード）
   * - ``parent``
     - Optional[Layer]
     - 親レイヤー（Noneで親を解除）
   * - ``layer_type``
     - LayerType
     - レイヤータイプ（読み取り専用）
   * - ``is_2d``
     - bool
     - 2Dレイヤーかどうか（読み取り専用）
   * - ``is_3d``
     - bool
     - 3Dレイヤーかどうか（読み書き可能）
   * - ``three_d``
     - bool
     - 3Dレイヤーフラグ（読み書き可能、is_3d のエイリアス）
   * - ``is_3d_layer``
     - bool
     - 3Dレイヤーかどうか（読み取り専用）
   * - ``is_adjustment_layer``
     - bool
     - 調整レイヤーかどうか（読み取り専用）
   * - ``is_adjustment``
     - bool
     - 調整レイヤーフラグ（読み書き可能、is_adjustment_layer のエイリアス）
   * - ``is_null_layer``
     - bool
     - ヌルレイヤーかどうか（読み取り専用）
   * - ``is_null``
     - bool
     - ヌルレイヤーかどうか（読み取り専用、is_null_layer のエイリアス）
   * - ``has_track_matte``
     - bool
     - トラックマットを持つかどうか（読み取り専用）
   * - ``is_used_as_track_matte``
     - bool
     - トラックマットとして使用されているか（読み取り専用）
   * - ``track_matte_layer``
     - Optional[Layer]
     - トラックマットレイヤー（読み取り専用）
   * - ``num_effects``
     - int
     - エフェクト数（読み取り専用）
   * - ``effects``
     - List[Effect]
     - エフェクトのリスト（読み取り専用）
   * - ``num_masks``
     - int
     - マスク数（読み取り専用）
   * - ``masks``
     - List[Mask]
     - マスクのリスト（読み取り専用）
   * - ``properties``
     - Property
     - ルートプロパティグループ（読み取り専用）

メソッド一覧
~~~~~~~~~~~~

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``effect(index_or_name) -> Effect``
     - インデックスまたは名前でエフェクトを取得
   * - ``add_effect(match_name) -> Effect``
     - エフェクトを追加（マッチネームで指定）
   * - ``mask(index_or_name) -> Mask``
     - インデックスまたは名前でマスクを取得
   * - ``add_mask(vertices=[]) -> Mask``
     - マスクを追加（頂点リストで形状指定）
   * - ``property(name) -> Property``
     - 名前でプロパティを取得（例: "Transform/Position"）
   * - ``set_position_keyframe(time, value)``
     - 指定時間にポジションキーフレームを追加
   * - ``set_scale_keyframe(time, value)``
     - 指定時間にスケールキーフレームを追加
   * - ``set_rotation_keyframe(time, value)``
     - 指定時間にローテーションキーフレームを追加
   * - ``set_opacity_keyframe(time, value)``
     - 指定時間にオパシティキーフレームを追加
   * - ``add_marker(time, comment="")``
     - 指定時間にマーカーを追加
   * - ``get_marker_comment(time) -> str``
     - 指定時間のマーカーコメントを取得
   * - ``set_track_matte(matte_layer, matte_mode: TrackMatteMode)``
     - トラックマットを設定
   * - ``remove_track_matte()``
     - トラックマットを削除
   * - ``to_world_xform(comp_time) -> List[List[float]]``
     - 指定時間でのワールド変換行列（4x4）を取得
   * - ``delete()``
     - レイヤーを削除
   * - ``duplicate() -> Layer``
     - レイヤーを複製
   * - ``move_to(index)``
     - 指定インデックスへ移動
   * - ``to_dict() -> dict``
     - レイヤーを辞書にエクスポート
   * - ``update_from_dict(data)``
     - 辞書データで既存レイヤーを更新

Pythonプロトコルメソッド
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__repr__() -> str``
     - 文字列表現を返す（例: ``<Layer: 'Name' index=1>``）
   * - ``__bool__() -> bool``
     - ``valid`` プロパティと同等。レイヤーが有効なら ``True``
   * - ``__eq__(other) -> bool``
     - ハンドルベースの等値比較
   * - ``__hash__() -> int``
     - ハンドルベースのハッシュ値

プロパティ
~~~~~~~~~~

.. attribute:: Layer.valid
   :type: bool

   レイヤーが有効かどうか。

   **例**:

   .. code-block:: python

      if layer.valid:
          print(f"レイヤー名: {layer.name}")

.. attribute:: Layer.position
   :type: Tuple[float, ...]

   レイヤーの位置。2Dの場合は (x, y)、3Dの場合は (x, y, z)。

   **例**:

   .. code-block:: python

      layer.position = (960, 540)        # 2D
      layer.position = (960, 540, 0)     # 3D

.. attribute:: Layer.parent
   :type: Optional[Layer]

   親レイヤー。``None`` を設定すると親を解除します。

   **例**:

   .. code-block:: python

      # 親を設定
      child_layer.parent = parent_layer

      # 親を解除
      child_layer.parent = None

.. attribute:: Layer.selected
   :type: bool

   レイヤーの選択状態。設定可能なプロパティです。

   **例**:

   .. code-block:: python

      # 選択状態を取得
      if layer.selected:
          print(f"{layer.name} は選択されています")

      # レイヤーを選択
      layer.selected = True

.. attribute:: Layer.is_adjustment
   :type: bool

   調整レイヤーフラグ。``is_adjustment_layer`` の読み書き可能なエイリアスです。

   **例**:

   .. code-block:: python

      # 調整レイヤーかどうか確認
      if layer.is_adjustment:
          print("調整レイヤーです")

      # 調整レイヤーフラグを設定
      layer.is_adjustment = True

.. attribute:: Layer.is_null
   :type: bool

   ヌルレイヤーかどうか。``is_null_layer`` の読み取り専用エイリアスです。

   **例**:

   .. code-block:: python

      if layer.is_null:
          print("ヌルレイヤーです")

.. attribute:: Layer.three_d
   :type: bool

   3Dレイヤーフラグ。``is_3d`` の読み書き可能なエイリアスです。

   **例**:

   .. code-block:: python

      # 3Dレイヤーかどうか確認
      if layer.three_d:
          print("3Dレイヤーです")

      # 3Dレイヤーに設定
      layer.three_d = True

メソッド
~~~~~~~~

.. method:: Layer.set_position_keyframe(time: float, value: Tuple[float, ...]) -> None

   指定時間にポジションキーフレームを追加します。

   :param time: キーフレームの時間（秒）
   :type time: float
   :param value: 位置の値（2Dの場合は (x, y)、3Dの場合は (x, y, z)）
   :type value: Tuple[float, ...]

   **例**:

   .. code-block:: python

      # 0秒と2秒にポジションキーフレームを追加
      layer.set_position_keyframe(0.0, (0, 540))
      layer.set_position_keyframe(2.0, (1920, 540))

.. method:: Layer.set_scale_keyframe(time: float, value: Tuple[float, ...]) -> None

   指定時間にスケールキーフレームを追加します。

   :param time: キーフレームの時間（秒）
   :type time: float
   :param value: スケールの値（%）
   :type value: Tuple[float, ...]

   **例**:

   .. code-block:: python

      # 0秒と1秒にスケールキーフレームを追加
      layer.set_scale_keyframe(0.0, (0, 0))
      layer.set_scale_keyframe(1.0, (100, 100))

.. method:: Layer.set_rotation_keyframe(time: float, value: float) -> None

   指定時間にローテーションキーフレームを追加します。

   :param time: キーフレームの時間（秒）
   :type time: float
   :param value: 回転の値（度）
   :type value: float

   **例**:

   .. code-block:: python

      # 0秒と3秒にローテーションキーフレームを追加
      layer.set_rotation_keyframe(0.0, 0.0)
      layer.set_rotation_keyframe(3.0, 360.0)

.. method:: Layer.set_opacity_keyframe(time: float, value: float) -> None

   指定時間にオパシティキーフレームを追加します。

   :param time: キーフレームの時間（秒）
   :type time: float
   :param value: 不透明度の値（0-100）
   :type value: float

   **例**:

   .. code-block:: python

      # フェードインアニメーション
      layer.set_opacity_keyframe(0.0, 0.0)
      layer.set_opacity_keyframe(1.0, 100.0)

LayerType 列挙型
----------------

.. list-table::
   :header-rows: 1
   :widths: 20 10 70

   * - 値
     - 数値
     - 説明
   * - ``None_``
     - 0
     - なし/無効
   * - ``AV``
     - 1
     - オーディオ/ビデオレイヤー
   * - ``Light``
     - 2
     - ライトレイヤー
   * - ``Camera``
     - 3
     - カメラレイヤー
   * - ``Text``
     - 4
     - テキストレイヤー
   * - ``Vector``
     - 5
     - ベクトル（シェイプ）レイヤー

LayerQuality 列挙型
-------------------

レイヤーの描画品質を表す ``ae.types.LayerQuality`` 列挙型（IntEnum）です。

.. list-table::
   :header-rows: 1
   :widths: 20 10 70

   * - 値
     - 数値
     - 説明
   * - ``None_``
     - 0
     - なし/無効
   * - ``Wireframe``
     - 1
     - ワイヤーフレーム
   * - ``Draft``
     - 2
     - ドラフト
   * - ``Best``
     - 3
     - 最高品質

**使用例**:

.. code-block:: python

   from ae.types import LayerQuality

   layer.quality = LayerQuality.Best
   print(layer.quality)  # LayerQuality.Best

SamplingQuality 列挙型
----------------------

レイヤーのサンプリング品質を表す ``ae.types.SamplingQuality`` 列挙型（IntEnum）です。

.. list-table::
   :header-rows: 1
   :widths: 20 10 70

   * - 値
     - 数値
     - 説明
   * - ``Bilinear``
     - 0
     - バイリニア補間
   * - ``Bicubic``
     - 1
     - バイキュービック補間

**使用例**:

.. code-block:: python

   from ae.types import SamplingQuality

   layer.sampling_quality = SamplingQuality.Bicubic

BlendMode / TrackMatteMode 列挙型
----------------------------------

``BlendMode`` および ``TrackMatteMode`` は ``ae.types`` モジュールで定義されている IntEnum 列挙型です。

- **BlendMode** - ブレンドモード（None\_, Copy, Behind, InFront, Dissolve, Add, Multiply, Screen, Overlay, SoftLight, HardLight, Darken, Lighten, ... 等 全40種）
- **TrackMatteMode** - トラックマットモード（NoTrackMatte, Alpha, NotAlpha, Luma, NotLuma）

詳細は ``ae.types`` モジュールリファレンスを参照してください。

**使用例**:

.. code-block:: python

   from ae.types import BlendMode, TrackMatteMode

   # ブレンドモードの設定
   layer.transfer_mode = {"mode": BlendMode.Multiply, "flags": 0, "track_matte": TrackMatteMode.NoTrackMatte}

   # トラックマットの設定
   target_layer.set_track_matte(matte_layer, TrackMatteMode.Alpha)

TransferMode 型
---------------

レイヤーの転送モード（ブレンドモード）情報を表すTypedDictです。

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - キー
     - 型
     - 説明
   * - ``mode``
     - BlendMode
     - ブレンドモード（ ``ae.types.BlendMode`` enum）
   * - ``flags``
     - int
     - 転送フラグ
   * - ``track_matte``
     - TrackMatteMode
     - トラックマットモード（ ``ae.types.TrackMatteMode`` enum）

使用例
------

基本的なプロパティ
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layers[0]

   # 名前とコメント
   layer.name = "MyLayer"
   layer.comment = "This is a test layer"

   # インデックス
   print(f"レイヤーインデックス: {layer.index}")

時間プロパティ
~~~~~~~~~~~~~~

.. code-block:: python

   # インポイント・アウトポイント
   layer.in_point = 2.0   # 2秒から開始
   layer.out_point = 8.0  # 8秒で終了

   # スタートタイム
   layer.start_time = 1.0

   # デュレーション（読み取り専用）
   print(f"デュレーション: {layer.duration}秒")

トランスフォーム
~~~~~~~~~~~~~~~~

.. code-block:: python

   # 位置
   layer.position = (960, 540)

   # スケール
   layer.scale = (100, 100)

   # 回転
   layer.rotation = 45.0

   # アンカーポイント
   layer.anchor_point = (960, 540)

   # 不透明度
   layer.opacity = 50.0

エフェクト
~~~~~~~~~~

.. code-block:: python

   # エフェクトを追加
   blur = layer.add_effect("ADBE Gaussian Blur 2")
   blur.property("Blurriness").value = 50.0

   # エフェクトを取得
   effect = layer.effect(1)  # インデックス
   effect = layer.effect("Gaussian Blur")  # 名前

   # すべてのエフェクトをループ
   for effect in layer.effects:
       print(f"- {effect.name}")

マスク
~~~~~~

.. code-block:: python

   # マスクを追加
   vertices = [(100, 100), (500, 100), (500, 500), (100, 500)]
   mask = layer.add_mask(vertices)
   mask.name = "MyMask"

   # マスクを取得
   mask = layer.mask(1)
   mask = layer.mask("MyMask")

トラックマット
~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from ae.types import TrackMatteMode

   comp = ae.Comp.get_active()
   target_layer = comp.layer(1)
   matte_layer = comp.layer(0)

   # トラックマットを設定
   target_layer.set_track_matte(matte_layer, TrackMatteMode.Alpha)

   # トラックマットを削除
   target_layer.remove_track_matte()

シリアライゼーション
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import json

   # レイヤーを辞書にエクスポート
   data = layer.to_dict()

   # JSONとして保存
   with open("layer_backup.json", "w", encoding="utf-8") as f:
       json.dump(data, f, indent=2, ensure_ascii=False)

   # 既存レイヤーを辞書データで更新
   with open("layer_backup.json", "r", encoding="utf-8") as f:
       data = json.load(f)
   layer.update_from_dict(data)

注意事項
--------

.. note::
   - ``index`` は 1 から始まります
   - ``duration`` や ``current_time`` は秒単位です
   - 3Dレイヤーの場合、位置やアンカーポイントは 3 要素のタプルになります

.. warning::
   - 削除されたレイヤーへの参照は無効になります
   - ``valid`` プロパティで有効性を確認することを推奨します

関連項目
--------

- :doc:`comp` - コンポジション
- :doc:`effect` - エフェクト
- :doc:`mask` - マスク
- :doc:`property` - プロパティ
- :doc:`/api/low-level/AEGP_LayerSuite9` - 低レベルAPI
