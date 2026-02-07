列挙型・定数
============

.. currentmodule:: ae

``ae`` モジュール直下で利用できる列挙型（Enum）と定数のリファレンスです。

列挙型はすべて ``IntEnum`` を継承しており、整数値として比較・演算が可能です。

.. code-block:: python

   import ae

   # Enum値として使用
   if layer.layer_type == ae.LayerType.Camera:
       print("This is a camera layer")

   # 整数値としても使用可能
   print(int(ae.BlendMode.Add))  # 5

列挙型
------

LayerType
~~~~~~~~~

.. class:: LayerType

   レイヤータイプを表す列挙型。

   .. attribute:: None_ = 0

      タイプなし

   .. attribute:: AV = 1

      AVレイヤー（映像/音声）

   .. attribute:: Light = 2

      ライト

   .. attribute:: Camera = 3

      カメラ

   .. attribute:: Text = 4

      テキスト

   .. attribute:: Adjustment = 5

      調整レイヤー

   .. attribute:: Null = 6

      ヌル

   .. attribute:: Shape = 7

      シェイプ

   .. attribute:: Solid = 8

      平面（ソリッド）

ItemType
~~~~~~~~

.. class:: ItemType

   アイテムタイプを表す列挙型。

   .. attribute:: None_ = 0

      タイプなし

   .. attribute:: Folder = 1

      フォルダ

   .. attribute:: Comp = 2

      コンポジション

   .. attribute:: Footage = 3

      フッテージ

   .. attribute:: Solid = 4

      平面（ソリッド）

FootageType
~~~~~~~~~~~

.. class:: FootageType

   フッテージタイプを表す列挙型。

   .. attribute:: None_ = 0

      タイプなし

   .. attribute:: Solid = 1

      平面（ソリッド）

   .. attribute:: Missing = 2

      不明（ファイルが見つからない）

   .. attribute:: Placeholder = 3

      プレースホルダー

MaskMode
~~~~~~~~

.. class:: MaskMode

   マスクモードを表す列挙型。

   .. attribute:: None_ = 0

      なし

   .. attribute:: Add = 1

      加算

   .. attribute:: Subtract = 2

      減算

   .. attribute:: Intersect = 3

      交差

   .. attribute:: Lighten = 4

      比較（明）

   .. attribute:: Darken = 5

      比較（暗）

   .. attribute:: Difference = 6

      差

RenderStatus
~~~~~~~~~~~~

.. class:: RenderStatus

   レンダーキューのステータスを表す列挙型。

   .. attribute:: WillContinue = 0

      続行予定

   .. attribute:: NeedsOutput = 1

      出力が必要

   .. attribute:: Queued = 2

      キュー待ち

   .. attribute:: Rendering = 3

      レンダリング中

   .. attribute:: UserStopped = 4

      ユーザーが停止

   .. attribute:: ErrStopped = 5

      エラーで停止

   .. attribute:: Done = 6

      完了

   .. attribute:: Unqueued = 7

      キューから外れている

PropertyType
~~~~~~~~~~~~

.. class:: PropertyType

   プロパティタイプを表す列挙型。

   .. attribute:: None_ = 0

      なし

   .. attribute:: OneDim = 1

      1次元（スカラー）

   .. attribute:: TwoDim = 2

      2次元

   .. attribute:: ThreeDim = 3

      3次元

   .. attribute:: Color = 4

      カラー

   .. attribute:: Arbitrary = 5

      任意データ

   .. attribute:: NoValue = 6

      値なし

StreamValueType
~~~~~~~~~~~~~~~

.. class:: StreamValueType

   ストリーム（プロパティ）値のデータ型を表す列挙型。

   .. attribute:: NoData = 0

      データなし

   .. attribute:: ThreeD_Spatial = 1

      3D空間座標

   .. attribute:: ThreeD = 2

      3D値

   .. attribute:: TwoD_Spatial = 3

      2D空間座標

   .. attribute:: TwoD = 4

      2D値

   .. attribute:: OneD = 5

      1D値（スカラー）

   .. attribute:: Color = 6

      カラー

   .. attribute:: ARB = 7

      任意データ (Arbitrary)

   .. attribute:: Marker = 8

      マーカー

   .. attribute:: LayerID = 9

      レイヤーID

   .. attribute:: MaskID = 10

      マスクID

   .. attribute:: Mask = 11

      マスク

   .. attribute:: TextDocument = 12

      テキストドキュメント

StreamGroupingType
~~~~~~~~~~~~~~~~~~

.. class:: StreamGroupingType

   ストリームのグルーピングタイプを表す列挙型。

   .. attribute:: Invalid = 0

      無効

   .. attribute:: Leaf = 1

      リーフ（値を持つプロパティ）

   .. attribute:: NamedGroup = 2

      名前付きグループ

   .. attribute:: IndexedGroup = 3

      インデックス付きグループ

KeyInterpolation
~~~~~~~~~~~~~~~~

.. class:: KeyInterpolation

   キーフレーム補間タイプを表す列挙型。

   .. attribute:: Linear = 0

      リニア（直線）

   .. attribute:: Bezier = 1

      ベジェ

   .. attribute:: Hold = 2

      ホールド（停止）

EasingType
~~~~~~~~~~

.. class:: EasingType

   イージングタイプを表す列挙型。

   .. attribute:: Linear = 0

      リニア

   .. attribute:: EaseIn = 1

      イーズイン

   .. attribute:: EaseOut = 2

      イーズアウト

   .. attribute:: EaseInOut = 3

      イーズインアウト

   .. attribute:: Custom = 4

      カスタム

LayerQuality
~~~~~~~~~~~~

.. class:: LayerQuality

   レイヤー品質を表す列挙型。

   .. attribute:: None_ = 0

      なし

   .. attribute:: Wireframe = 1

      ワイヤーフレーム

   .. attribute:: Draft = 2

      ドラフト

   .. attribute:: Best = 3

      最高品質

SamplingQuality
~~~~~~~~~~~~~~~

.. class:: SamplingQuality

   サンプリング品質を表す列挙型。

   .. attribute:: Bilinear = 0

      バイリニア

   .. attribute:: Bicubic = 1

      バイキュービック

BlendMode
~~~~~~~~~

.. class:: BlendMode

   ブレンドモード（描画モード）を表す列挙型。

   **基本モード:**

   .. attribute:: None_ = 0

      なし

   .. attribute:: Copy = 1

      コピー

   .. attribute:: Behind = 2

      背面

   .. attribute:: InFront = 3

      前面

   .. attribute:: Dissolve = 4

      ディザ合成

   **加算・乗算系:**

   .. attribute:: Add = 5

      加算

   .. attribute:: Multiply = 6

      乗算

   .. attribute:: Screen = 7

      スクリーン

   .. attribute:: Overlay = 8

      オーバーレイ

   **ライト系:**

   .. attribute:: SoftLight = 9

      ソフトライト

   .. attribute:: HardLight = 10

      ハードライト

   .. attribute:: LinearLight = 32

      リニアライト

   .. attribute:: VividLight = 33

      ビビッドライト

   .. attribute:: PinLight = 34

      ピンライト

   .. attribute:: HardMix = 35

      ハードミックス

   **比較系:**

   .. attribute:: Darken = 11

      比較（暗）

   .. attribute:: Lighten = 12

      比較（明）

   .. attribute:: DarkerColor = 37

      カラー比較（暗）

   .. attribute:: LighterColor = 36

      カラー比較（明）

   **差分系:**

   .. attribute:: Difference = 13

      差

   .. attribute:: Exclusion = 26

      除外

   .. attribute:: Subtract = 38

      減算

   .. attribute:: Divide = 39

      除算

   **HSL系:**

   .. attribute:: Hue = 14

      色相

   .. attribute:: Saturation = 15

      彩度

   .. attribute:: Color = 16

      カラー

   .. attribute:: Luminosity = 17

      輝度

   **覆い焼き・焼き込み:**

   .. attribute:: ColorDodge = 24

      覆い焼きカラー

   .. attribute:: ColorBurn = 25

      焼き込みカラー

   .. attribute:: LinearDodge = 30

      覆い焼きリニア（加算）

   .. attribute:: LinearBurn = 31

      焼き込みリニア

   **アルファ系:**

   .. attribute:: MultiplyAlpha = 18

      アルファ乗算

   .. attribute:: MultiplyAlphaLuma = 19

      アルファ輝度乗算

   .. attribute:: MultiplyNotAlpha = 20

      非アルファ乗算

   .. attribute:: MultiplyNotAlphaLuma = 21

      非アルファ輝度乗算

   .. attribute:: AdditivePremul = 22

      加算（プリマルチプライ）

   .. attribute:: AlphaAdd = 23

      アルファ加算

   **互換モード:**

   .. attribute:: Difference2 = 27

      差2

   .. attribute:: ColorDodge2 = 28

      覆い焼きカラー2

   .. attribute:: ColorBurn2 = 29

      焼き込みカラー2

TrackMatteMode
~~~~~~~~~~~~~~

.. class:: TrackMatteMode

   トラックマットモードを表す列挙型。

   .. attribute:: NoTrackMatte = 0

      トラックマットなし

   .. attribute:: Alpha = 1

      アルファマット

   .. attribute:: NotAlpha = 2

      アルファ反転マット

   .. attribute:: Luma = 3

      ルミナンスキーマット

   .. attribute:: NotLuma = 4

      ルミナンスキー反転マット

EmbedOptions
~~~~~~~~~~~~

.. class:: EmbedOptions

   フッテージ埋め込みオプションを表す列挙型。

   .. attribute:: None_ = 0

      埋め込みなし

   .. attribute:: Link = 1

      リンク

   .. attribute:: All = 2

      すべて埋め込み

LightType
~~~~~~~~~

.. class:: LightType

   ライトタイプを表す列挙型。

   .. attribute:: Parallel = 0

      平行光源

   .. attribute:: Spot = 1

      スポットライト

   .. attribute:: Point = 2

      ポイントライト

   .. attribute:: Ambient = 3

      環境光

定数
----

以下の定数は ``ae`` モジュール直下で直接アクセスできます。
Enum型での使用が推奨されますが、後方互換性のため整数定数も提供されています。

レイヤー品質
~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - 定数
     - 値
     - 説明
   * - ``ae.AEGP_LayerQual_NONE``
     - 0
     - 品質なし
   * - ``ae.AEGP_LayerQual_WIREFRAME``
     - 1
     - ワイヤーフレーム
   * - ``ae.AEGP_LayerQual_DRAFT``
     - 2
     - ドラフト
   * - ``ae.AEGP_LayerQual_BEST``
     - 3
     - 最高品質

サンプリング品質
~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - 定数
     - 値
     - 説明
   * - ``ae.AEGP_LayerSamplingQual_BILINEAR``
     - 0
     - バイリニア
   * - ``ae.AEGP_LayerSamplingQual_BICUBIC``
     - 1
     - バイキュービック

レイヤーラベル
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - 定数
     - 値
     - 説明
   * - ``ae.AEGP_Label_NONE``
     - 0
     - ラベルなし
   * - ``ae.AEGP_Label_NO_LABEL``
     - 0
     - ラベルなし（別名）
   * - ``ae.AEGP_Label_1`` ～ ``ae.AEGP_Label_16``
     - 1～16
     - カラーラベル 1～16

トラックマット
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - 定数
     - 値
     - 説明
   * - ``ae.TRACK_MATTE_NO_TRACK_MATTE``
     - 0
     - トラックマットなし
   * - ``ae.TRACK_MATTE_ALPHA``
     - 1
     - アルファマット
   * - ``ae.TRACK_MATTE_NOT_ALPHA``
     - 2
     - アルファ反転マット
   * - ``ae.TRACK_MATTE_LUMA``
     - 3
     - ルミナンスキーマット
   * - ``ae.TRACK_MATTE_NOT_LUMA``
     - 4
     - ルミナンスキー反転マット

転送モード（ブレンドモード）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 40 10 50

   * - 定数
     - 値
     - 説明
   * - ``ae.PF_Xfer_NONE``
     - 0
     - なし
   * - ``ae.PF_Xfer_COPY``
     - 1
     - コピー
   * - ``ae.PF_Xfer_BEHIND``
     - 2
     - 背面
   * - ``ae.PF_Xfer_IN_FRONT``
     - 3
     - 前面
   * - ``ae.PF_Xfer_DISSOLVE``
     - 4
     - ディザ合成
   * - ``ae.PF_Xfer_ADD``
     - 5
     - 加算
   * - ``ae.PF_Xfer_MULTIPLY``
     - 6
     - 乗算
   * - ``ae.PF_Xfer_SCREEN``
     - 7
     - スクリーン
   * - ``ae.PF_Xfer_OVERLAY``
     - 8
     - オーバーレイ
   * - ``ae.PF_Xfer_SOFT_LIGHT``
     - 9
     - ソフトライト
   * - ``ae.PF_Xfer_HARD_LIGHT``
     - 10
     - ハードライト
   * - ``ae.PF_Xfer_DARKEN``
     - 11
     - 比較（暗）
   * - ``ae.PF_Xfer_LIGHTEN``
     - 12
     - 比較（明）
   * - ``ae.PF_Xfer_DIFFERENCE``
     - 13
     - 差
   * - ``ae.PF_Xfer_HUE``
     - 14
     - 色相
   * - ``ae.PF_Xfer_SATURATION``
     - 15
     - 彩度
   * - ``ae.PF_Xfer_COLOR``
     - 16
     - カラー
   * - ``ae.PF_Xfer_LUMINOSITY``
     - 17
     - 輝度
   * - ``ae.PF_Xfer_MULTIPLY_ALPHA``
     - 18
     - アルファ乗算
   * - ``ae.PF_Xfer_MULTIPLY_ALPHA_LUMA``
     - 19
     - アルファ輝度乗算
   * - ``ae.PF_Xfer_MULTIPLY_NOT_ALPHA``
     - 20
     - 非アルファ乗算
   * - ``ae.PF_Xfer_MULTIPLY_NOT_ALPHA_LUMA``
     - 21
     - 非アルファ輝度乗算
   * - ``ae.PF_Xfer_ADDITIVE_PREMUL``
     - 22
     - 加算（プリマルチプライ）
   * - ``ae.PF_Xfer_ALPHA_ADD``
     - 23
     - アルファ加算
   * - ``ae.PF_Xfer_COLOR_DODGE``
     - 24
     - 覆い焼きカラー
   * - ``ae.PF_Xfer_COLOR_BURN``
     - 25
     - 焼き込みカラー
   * - ``ae.PF_Xfer_EXCLUSION``
     - 26
     - 除外
   * - ``ae.PF_Xfer_DIFFERENCE2``
     - 27
     - 差2
   * - ``ae.PF_Xfer_COLOR_DODGE2``
     - 28
     - 覆い焼きカラー2
   * - ``ae.PF_Xfer_COLOR_BURN2``
     - 29
     - 焼き込みカラー2
   * - ``ae.PF_Xfer_LINEAR_DODGE``
     - 30
     - 覆い焼きリニア
   * - ``ae.PF_Xfer_LINEAR_BURN``
     - 31
     - 焼き込みリニア
   * - ``ae.PF_Xfer_LINEAR_LIGHT``
     - 32
     - リニアライト
   * - ``ae.PF_Xfer_VIVID_LIGHT``
     - 33
     - ビビッドライト
   * - ``ae.PF_Xfer_PIN_LIGHT``
     - 34
     - ピンライト
   * - ``ae.PF_Xfer_HARD_MIX``
     - 35
     - ハードミックス
   * - ``ae.PF_Xfer_LIGHTER_COLOR``
     - 36
     - カラー比較（明）
   * - ``ae.PF_Xfer_DARKER_COLOR``
     - 37
     - カラー比較（暗）
   * - ``ae.PF_Xfer_SUBTRACT``
     - 38
     - 減算
   * - ``ae.PF_Xfer_DIVIDE``
     - 39
     - 除算

関連項目
--------

- :doc:`functions` - トップレベル関数
- :doc:`property` - Property クラス
- :doc:`layer` - Layer クラス
- :doc:`mask` - Mask クラス
- :doc:`render_queue` - RenderQueueItem クラス
