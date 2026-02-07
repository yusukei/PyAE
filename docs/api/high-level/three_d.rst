3Dレイヤー・カメラ・ライト
==========================

.. currentmodule:: ae.three_d

``ae.three_d`` モジュールは、3Dレイヤー、カメラ、ライトの操作を行うための低レベルAPIを提供します。

概要
----

このモジュールを使用すると、レイヤーの3D状態の設定、3D変換（位置、回転、スケール）、
カメラのズームや注視点、ライトの色や強度などを直接操作できます。

**主な機能**:

- 3Dレイヤーの有効化/無効化
- 3D位置、回転、スケール、オリエンテーションの取得/設定
- カメラのズーム、注視点の操作
- ライトの種類、強度、色、コーン角度の操作

.. note::
   このモジュールの関数は低レベルAPIであり、レイヤーハンドル（``layer._handle``）を直接使用します。
   高レベルな操作には ``Layer`` クラスのプロパティ経由でのアクセスを推奨します。

基本的な使い方
--------------

3Dレイヤーの操作
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.three_d as td

   comp = ae.Comp.get_active()
   layer = comp.layer(0)

   # 3D状態を確認
   if td.is_3d(layer._handle):
       print(f"{layer.name} is 3D")

   # 3Dを有効化
   td.set_3d(layer._handle, True)

   # 3D位置を取得
   x, y, z = td.get_position(layer._handle)
   print(f"Position: ({x}, {y}, {z})")

   # 3D位置を設定
   td.set_position(layer._handle, 960, 540, 100)

カメラの操作
~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.three_d as td

   comp = ae.Comp.get_active()

   # カメラレイヤーを追加
   camera = comp.add_camera("Main Camera")

   # カメラかどうかを確認
   if td.is_camera(camera._handle):
       # ズームを取得/設定
       zoom = td.get_camera_zoom(camera._handle)
       print(f"Camera zoom: {zoom}")

       td.set_camera_zoom(camera._handle, 1500)

ライトの操作
~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.three_d as td

   comp = ae.Comp.get_active()

   # ライトレイヤーを追加
   light = comp.add_light("Key Light")

   # ライトかどうかを確認
   if td.is_light(light._handle):
       # ライトタイプを取得
       light_type = td.get_light_type(light._handle)
       print(f"Light type: {light_type}")

       # 強度を設定
       td.set_light_intensity(light._handle, 150)

       # 色を設定 (RGB: 0.0-1.0)
       td.set_light_color(light._handle, 1.0, 0.9, 0.8)

API リファレンス
----------------

3D状態
~~~~~~

.. function:: is_3d(layer_handle: int) -> bool

   レイヤーが3Dかどうかを確認します。

   :param layer_handle: レイヤーハンドル
   :return: 3Dの場合は True

.. function:: set_3d(layer_handle: int, is_3d: bool) -> None

   レイヤーの3D状態を設定します。

   :param layer_handle: レイヤーハンドル
   :param is_3d: 3Dにする場合は True

3D変換
~~~~~~

.. function:: get_position(layer_handle: int) -> Tuple[float, float, float]

   3D位置を取得します。

   :param layer_handle: レイヤーハンドル
   :return: (x, y, z) のタプル

.. function:: set_position(layer_handle: int, position: Tuple[float, float, float]) -> None

   3D位置を設定します。

   :param layer_handle: レイヤーハンドル
   :param position: (x, y, z) のタプル

.. function:: get_rotation(layer_handle: int) -> Tuple[float, float, float]

   3D回転を取得します（度数法）。

   :param layer_handle: レイヤーハンドル
   :return: (rotX, rotY, rotZ) のタプル（度）

.. function:: set_rotation(layer_handle: int, rotation: Tuple[float, float, float]) -> None

   3D回転を設定します（度数法）。

   :param layer_handle: レイヤーハンドル
   :param rotation: (rotX, rotY, rotZ) のタプル（度）

.. function:: get_scale(layer_handle: int) -> Tuple[float, float, float]

   3Dスケールを取得します（パーセント）。

   :param layer_handle: レイヤーハンドル
   :return: (scaleX, scaleY, scaleZ) のタプル（%）

.. function:: set_scale(layer_handle: int, scale: Tuple[float, float, float]) -> None

   3Dスケールを設定します（パーセント）。

   :param layer_handle: レイヤーハンドル
   :param scale: (scaleX, scaleY, scaleZ) のタプル（%）

.. function:: get_orientation(layer_handle: int) -> Tuple[float, float, float]

   オリエンテーションを取得します。

   :param layer_handle: レイヤーハンドル
   :return: (x, y, z) のタプル

.. function:: set_orientation(layer_handle: int, orientation: Tuple[float, float, float]) -> None

   オリエンテーションを設定します。

   :param layer_handle: レイヤーハンドル
   :param orientation: (x, y, z) のタプル

カメラ
~~~~~~

.. function:: is_camera(layer_handle: int) -> bool

   レイヤーがカメラかどうかを確認します。

   :param layer_handle: レイヤーハンドル
   :return: カメラの場合は True

.. function:: get_camera_zoom(layer_handle: int) -> float

   カメラのズーム値を取得します。

   :param layer_handle: カメラレイヤーのハンドル
   :return: ズーム値（ピクセル）

.. function:: set_camera_zoom(layer_handle: int, zoom: float) -> None

   カメラのズーム値を設定します。

   :param layer_handle: カメラレイヤーのハンドル
   :param zoom: ズーム値（ピクセル）

.. function:: get_camera_poi(layer_handle: int) -> Tuple[float, float, float]

   カメラの注視点（Point of Interest）を取得します。

   :param layer_handle: カメラレイヤーのハンドル
   :return: (x, y, z) のタプル

   .. note::
      この関数は AEGP SDK では直接サポートされていない場合があります。

.. function:: set_camera_poi(layer_handle: int, poi: Tuple[float, float, float]) -> None

   カメラの注視点を設定します。

   :param layer_handle: カメラレイヤーのハンドル
   :param poi: (x, y, z) のタプル

ライト
~~~~~~

.. function:: is_light(layer_handle: int) -> bool

   レイヤーがライトかどうかを確認します。

   :param layer_handle: レイヤーハンドル
   :return: ライトの場合は True

.. function:: get_light_type(layer_handle: int) -> LightType

   ライトの種類を取得します。

   :param layer_handle: ライトレイヤーのハンドル
   :return: ライトタイプ（LightType列挙型）

   **ライトタイプ**:

   - ``LightType.NONE`` - なし
   - ``LightType.PARALLEL`` - 平行光源
   - ``LightType.SPOT`` - スポットライト
   - ``LightType.POINT`` - ポイントライト
   - ``LightType.AMBIENT`` - アンビエントライト

.. function:: get_light_intensity(layer_handle: int) -> float

   ライトの強度を取得します。

   :param layer_handle: ライトレイヤーのハンドル
   :return: 強度（%）

.. function:: set_light_intensity(layer_handle: int, intensity: float) -> None

   ライトの強度を設定します。

   :param layer_handle: ライトレイヤーのハンドル
   :param intensity: 強度（%）

.. function:: get_light_color(layer_handle: int) -> Tuple[float, float, float]

   ライトの色を取得します。

   :param layer_handle: ライトレイヤーのハンドル
   :return: (r, g, b) のタプル（0.0-1.0）

.. function:: set_light_color(layer_handle: int, color: Tuple[float, float, float]) -> None

   ライトの色を設定します。

   :param layer_handle: ライトレイヤーのハンドル
   :param color: (r, g, b) のタプル（0.0-1.0）

.. function:: get_light_cone_angle(layer_handle: int) -> float

   スポットライトのコーン角度を取得します。

   :param layer_handle: ライトレイヤーのハンドル
   :return: コーン角度（度）

.. function:: set_light_cone_angle(layer_handle: int, angle: float) -> None

   スポットライトのコーン角度を設定します。

   :param layer_handle: ライトレイヤーのハンドル
   :param angle: コーン角度（度）

.. function:: get_light_cone_feather(layer_handle: int) -> float

   スポットライトのコーンフェザーを取得します。

   :param layer_handle: ライトレイヤーのハンドル
   :return: フェザー値（%）

.. function:: set_light_cone_feather(layer_handle: int, feather: float) -> None

   スポットライトのコーンフェザーを設定します。

   :param layer_handle: ライトレイヤーのハンドル
   :param feather: フェザー値（%）

実用例
------

3Dシーンのセットアップ
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.three_d as td

   def setup_3d_scene():
       """3Dシーンを基本セットアップ"""
       comp = ae.Comp.get_active()
       if not comp:
           return

       with ae.UndoGroup("Setup 3D Scene"):
           # カメラを作成
           camera = comp.add_camera("Main Camera")
           td.set_position(camera._handle, 960, 540, -1500)
           td.set_camera_zoom(camera._handle, 1500)

           # キーライトを作成
           key_light = comp.add_light("Key Light")
           td.set_position(key_light._handle, 500, 300, -800)
           td.set_light_intensity(key_light._handle, 120)
           td.set_light_color(key_light._handle, 1.0, 0.95, 0.9)

           # フィルライトを作成
           fill_light = comp.add_light("Fill Light")
           td.set_position(fill_light._handle, 1400, 600, -600)
           td.set_light_intensity(fill_light._handle, 60)
           td.set_light_color(fill_light._handle, 0.8, 0.85, 1.0)

           print("3D scene setup complete")

   setup_3d_scene()

レイヤーの3D変換
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.three_d as td
   import math

   def rotate_layers_in_circle(num_layers: int = 6, radius: float = 300):
       """レイヤーを円形に配置"""
       comp = ae.Comp.get_active()
       if not comp:
           return

       layers = comp.layers[:num_layers]

       with ae.UndoGroup("Arrange Layers in Circle"):
           for i, layer in enumerate(layers):
               # 3Dを有効化
               td.set_3d(layer._handle, True)

               # 角度を計算
               angle = (2 * math.pi * i) / num_layers

               # 位置を計算
               x = comp.width / 2 + radius * math.cos(angle)
               y = comp.height / 2 + radius * math.sin(angle)
               z = 0

               td.set_position(layer._handle, (x, y, z))

               # 中心を向くように回転
               rotation = math.degrees(angle) + 90
               td.set_rotation(layer._handle, (0, rotation, 0))

   rotate_layers_in_circle()

カメラアニメーション用データ取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.three_d as td

   def get_camera_info():
       """シーン内のすべてのカメラ情報を取得"""
       comp = ae.Comp.get_active()
       if not comp:
           return []

       cameras = []
       for layer in comp.layers:
           if td.is_camera(layer._handle):
               pos = td.get_position(layer._handle)
               zoom = td.get_camera_zoom(layer._handle)
               poi = td.get_camera_poi(layer._handle)

               cameras.append({
                   'name': layer.name,
                   'position': pos,
                   'zoom': zoom,
                   'poi': poi
               })

       return cameras

   # 使用例
   for cam in get_camera_info():
       print(f"Camera: {cam['name']}")
       print(f"  Position: {cam['position']}")
       print(f"  Zoom: {cam['zoom']}")
       print(f"  POI: {cam['poi']}")

ライト情報のダンプ
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.three_d as td

   def dump_light_info():
       """シーン内のすべてのライト情報を表示"""
       comp = ae.Comp.get_active()
       if not comp:
           return

       print("=" * 50)
       print("Light Information")
       print("=" * 50)

       for layer in comp.layers:
           if not td.is_light(layer._handle):
               continue

           light_type = td.get_light_type(layer._handle)
           intensity = td.get_light_intensity(layer._handle)
           color = td.get_light_color(layer._handle)
           pos = td.get_position(layer._handle)

           print(f"\n{layer.name}:")
           print(f"  Type: {light_type}")
           print(f"  Position: ({pos[0]:.1f}, {pos[1]:.1f}, {pos[2]:.1f})")
           print(f"  Intensity: {intensity}%")
           print(f"  Color: RGB({color[0]:.2f}, {color[1]:.2f}, {color[2]:.2f})")

           # スポットライトの場合は追加情報
           if light_type == ae.LightType.SPOT:
               cone = td.get_light_cone_angle(layer._handle)
               feather = td.get_light_cone_feather(layer._handle)
               print(f"  Cone Angle: {cone}°")
               print(f"  Cone Feather: {feather}%")

   dump_light_info()

注意事項
--------

.. note::
   - このモジュールの関数はレイヤーハンドル（``layer._handle``）を直接使用します
   - カメラとライトの一部のプロパティは、AEGP SDK の制限により直接アクセスできない場合があります
   - 3D変換の値は、現在時刻でのスナップショットです。アニメーションにはキーフレームを使用してください

.. warning::
   - 無効なレイヤーハンドルを渡すとエラーが発生します
   - カメラ/ライト専用の関数を通常のレイヤーに対して呼び出すとエラーになる場合があります
   - 変換値の単位に注意してください（角度は度、スケールは%）

関連項目
--------

- :doc:`layer` - レイヤークラス
- :doc:`/api/low-level/AEGP_LayerSuite9` - 低レベルレイヤーAPI
- :doc:`/api/low-level/AEGP_CameraSuite2` - 低レベルカメラAPI
- :doc:`/api/low-level/AEGP_LightSuite3` - 低レベルライトAPI
