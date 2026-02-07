AEGP_CameraSuite2 / AEGP_LightSuite3
====================================

.. currentmodule:: ae.sdk

AEGP_CameraSuite2とAEGP_LightSuite3は、After Effectsのカメラレイヤーとライトレイヤーの操作を行うためのSDK APIです。

概要
----

**実装状況**:

- **AEGP_CameraSuite2**: 5/5関数実装 ✅
- **AEGP_LightSuite3**: 4/4関数実装 ✅ (AE 24.4+の2関数を含む)

これらのSuiteは以下の機能を提供します:

- カメラレイヤーの取得と設定（カメラタイプ、フィルムサイズ）
- ライトレイヤーの取得と設定（ライトタイプ、光源レイヤー）
- デフォルトカメラの距離設定
- 3Dコンポジションでのカメラ/ライト制御

基本概念
--------

カメラレイヤーとライトレイヤー
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsの3D空間では、カメラレイヤーとライトレイヤーが重要な役割を果たします。

.. important::
   - カメラレイヤー: 3Dシーンの視点を制御
   - ライトレイヤー: 3Dレイヤーの照明を制御
   - 2Dレイヤーにはカメラ/ライト関数は適用できません

カメラタイプ (AEGP_CameraType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsで使用可能なカメラの種類:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_CameraType_NONE``
     - 0
     - カメラではない（非カメラレイヤー）
   * - ``AEGP_CameraType_PERSPECTIVE``
     - 1
     - パースペクティブカメラ（透視投影）
   * - ``AEGP_CameraType_ORTHOGRAPHIC``
     - 2
     - オルソグラフィックカメラ（平行投影）

**使用例**:

.. code-block:: python

   # カメラタイプの確認
   camera_type = ae.sdk.AEGP_GetCameraType(camera_layerH)

   if camera_type == ae.sdk.AEGP_CameraType_PERSPECTIVE:
       print("パースペクティブカメラです")
   elif camera_type == ae.sdk.AEGP_CameraType_ORTHOGRAPHIC:
       print("オルソグラフィックカメラです")

ライトタイプ (AEGP_LightType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsで使用可能なライトの種類:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_LightType_NONE``
     - 0
     - ライトではない（非ライトレイヤー）
   * - ``AEGP_LightType_PARALLEL``
     - 1
     - 平行光源（太陽光のような方向性のある光）
   * - ``AEGP_LightType_SPOT``
     - 2
     - スポットライト（円錐状の光）
   * - ``AEGP_LightType_POINT``
     - 3
     - ポイントライト（全方向に広がる光）
   * - ``AEGP_LightType_AMBIENT``
     - 4
     - アンビエントライト（環境光）
   * - ``AEGP_LightType_ENVIRONMENT``
     - 5
     - 環境ライト（AE 24.4+、HDR画像ベースの照明）

**使用例**:

.. code-block:: python

   # ライトタイプの設定
   ae.sdk.AEGP_SetLightType(light_layerH, ae.sdk.AEGP_LightType_SPOT)

フィルムサイズユニット (AEGP_FilmSizeUnits)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

カメラのフィルムサイズを指定する単位:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_FilmSizeUnits_NONE``
     - 0
     - 未設定
   * - ``AEGP_FilmSizeUnits_HORIZONTAL``
     - 1
     - 水平サイズ（幅）
   * - ``AEGP_FilmSizeUnits_VERTICAL``
     - 2
     - 垂直サイズ（高さ）
   * - ``AEGP_FilmSizeUnits_DIAGONAL``
     - 3
     - 対角線サイズ

**使用例**:

.. code-block:: python

   # フィルムサイズの取得
   film_size_units, film_size = ae.sdk.AEGP_GetCameraFilmSize(camera_layerH)

   if film_size_units == ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL:
       print(f"水平フィルムサイズ: {film_size}px")

API リファレンス
----------------

AEGP_CameraSuite2 - カメラ操作
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

カメラレイヤーの取得
^^^^^^^^^^^^^^^^^^^^

.. function:: AEGP_GetCamera(render_contextH: int, comp_time: float) -> int

   レンダーコンテキストから、指定時刻のアクティブカメラレイヤーを取得します。

   :param render_contextH: レンダーコンテキストハンドル
   :type render_contextH: int
   :param comp_time: コンポジション時間（秒単位）
   :type comp_time: float
   :return: カメラレイヤーハンドル
   :rtype: int

   .. note::
      この関数はレンダーコンテキストから呼び出す必要があります。
      通常のスクリプトからは使用できません。

   **例**:

   .. code-block:: python

      camera_layerH = ae.sdk.AEGP_GetCamera(render_contextH, comp_time=2.5)

カメラタイプの取得と設定
^^^^^^^^^^^^^^^^^^^^^^^^

.. function:: AEGP_GetCameraType(camera_layerH: int) -> int

   カメラレイヤーのカメラタイプを取得します。

   :param camera_layerH: カメラレイヤーハンドル
   :type camera_layerH: int
   :return: カメラタイプ（``AEGP_CameraType_*``）
   :rtype: int

   .. note::
      非カメラレイヤーに対して呼び出すと ``AEGP_CameraType_NONE`` を返します（エラーダイアログは表示されません）。

   **例**:

   .. code-block:: python

      camera_type = ae.sdk.AEGP_GetCameraType(camera_layerH)

      if camera_type == ae.sdk.AEGP_CameraType_PERSPECTIVE:
          print("パースペクティブカメラ")
      elif camera_type == ae.sdk.AEGP_CameraType_ORTHOGRAPHIC:
          print("オルソグラフィックカメラ")
      else:
          print("カメラレイヤーではありません")

デフォルトカメラの距離
^^^^^^^^^^^^^^^^^^^^^^

.. function:: AEGP_GetDefaultCameraDistanceToImagePlane(compH: int) -> float

   コンポジションのデフォルトカメラからイメージプレーンまでの距離を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: カメラからイメージプレーンまでの距離（ピクセル単位）
   :rtype: float

   .. note::
      この値は、カメラを作成しない3Dコンポジションで使用されるデフォルトカメラの位置を決定します。

   **例**:

   .. code-block:: python

      distance = ae.sdk.AEGP_GetDefaultCameraDistanceToImagePlane(compH)
      print(f"デフォルトカメラ距離: {distance}px")

カメラフィルムサイズの取得と設定
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. function:: AEGP_GetCameraFilmSize(camera_layerH: int) -> tuple[int, float]

   カメラのフィルムサイズユニットとサイズを取得します。

   :param camera_layerH: カメラレイヤーハンドル
   :type camera_layerH: int
   :return: (フィルムサイズユニット, フィルムサイズ値)
   :rtype: tuple[int, float]
   :raises RuntimeError: 非カメラレイヤーに対して呼び出した場合

   .. warning::
      この関数はカメラレイヤーに対してのみ使用できます。
      非カメラレイヤーに対して呼び出すとエラーが発生します。

   **例**:

   .. code-block:: python

      film_size_units, film_size = ae.sdk.AEGP_GetCameraFilmSize(camera_layerH)

      if film_size_units == ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL:
          print(f"水平フィルムサイズ: {film_size}px")
      elif film_size_units == ae.sdk.AEGP_FilmSizeUnits_VERTICAL:
          print(f"垂直フィルムサイズ: {film_size}px")

.. function:: AEGP_SetCameraFilmSize(camera_layerH: int, film_size_units: int, film_size: float) -> None

   カメラのフィルムサイズユニットとサイズを設定します。

   :param camera_layerH: カメラレイヤーハンドル
   :type camera_layerH: int
   :param film_size_units: フィルムサイズユニット（``AEGP_FilmSizeUnits_*``）
   :type film_size_units: int
   :param film_size: フィルムサイズ値（ピクセル単位、正の値）
   :type film_size: float
   :raises ValueError: film_sizeが負または0の場合
   :raises RuntimeError: 非カメラレイヤーに対して呼び出した場合

   .. warning::
      この関数はカメラレイヤーに対してのみ使用できます。

   **例**:

   .. code-block:: python

      # 水平フィルムサイズを36mmに設定（35mmフィルム相当）
      ae.sdk.AEGP_SetCameraFilmSize(
          camera_layerH,
          ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL,
          36.0
      )

AEGP_LightSuite3 - ライト操作
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ライトタイプの取得と設定
^^^^^^^^^^^^^^^^^^^^^^^^

.. function:: AEGP_GetLightType(light_layerH: int) -> int

   ライトレイヤーのライトタイプを取得します。

   :param light_layerH: ライトレイヤーハンドル
   :type light_layerH: int
   :return: ライトタイプ（``AEGP_LightType_*``）
   :rtype: int

   .. note::
      非ライトレイヤーに対して呼び出すと ``AEGP_LightType_NONE`` を返します（エラーダイアログは表示されません）。

   **例**:

   .. code-block:: python

      light_type = ae.sdk.AEGP_GetLightType(light_layerH)

      if light_type == ae.sdk.AEGP_LightType_SPOT:
          print("スポットライト")
      elif light_type == ae.sdk.AEGP_LightType_POINT:
          print("ポイントライト")

.. function:: AEGP_SetLightType(light_layerH: int, light_type: int) -> None

   ライトレイヤーのライトタイプを設定します。

   :param light_layerH: ライトレイヤーハンドル
   :type light_layerH: int
   :param light_type: ライトタイプ（``AEGP_LightType_*``）
   :type light_type: int
   :raises RuntimeError: 非ライトレイヤーに対して呼び出した場合

   .. warning::
      この関数はライトレイヤーに対してのみ使用できます。

   **例**:

   .. code-block:: python

      # スポットライトに変更
      ae.sdk.AEGP_SetLightType(light_layerH, ae.sdk.AEGP_LightType_SPOT)

ライトソースの取得と設定 (AE 24.4+)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. function:: AEGP_GetLightSource(light_layerH: int) -> int

   環境ライトの光源レイヤーを取得します（AE 24.4以降でのみ利用可能）。

   :param light_layerH: ライトレイヤーハンドル
   :type light_layerH: int
   :return: 光源レイヤーハンドル（設定されていない場合は0）
   :rtype: int

   .. versionadded:: AE 24.4

   .. note::
      環境ライト (``AEGP_LightType_ENVIRONMENT``) に対してのみ意味のある値を返します。
      非ライトレイヤーに対して呼び出すと0を返します。

   **例**:

   .. code-block:: python

      light_sourceH = ae.sdk.AEGP_GetLightSource(light_layerH)

      if light_sourceH != 0:
          print("光源レイヤーが設定されています")

.. function:: AEGP_SetLightSource(light_layerH: int, light_sourceH: int) -> None

   環境ライトの光源レイヤーを設定します（AE 24.4以降でのみ利用可能）。

   :param light_layerH: ライトレイヤーハンドル
   :type light_layerH: int
   :param light_sourceH: 光源レイヤーハンドル（0を指定すると光源をクリア）
   :type light_sourceH: int
   :raises RuntimeError: 非ライトレイヤーに対して呼び出した場合

   .. versionadded:: AE 24.4

   .. note::
      環境ライト (``AEGP_LightType_ENVIRONMENT``) に対して使用されます。
      HDR画像レイヤーを光源として設定することができます。

   **例**:

   .. code-block:: python

      # HDRレイヤーを環境ライトの光源として設定
      ae.sdk.AEGP_SetLightSource(light_layerH, hdr_layerH)

      # 光源をクリア
      ae.sdk.AEGP_SetLightSource(light_layerH, 0)

使用例
------

カメラの詳細情報を取得
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def print_camera_info(layerH):
       """カメラレイヤーの詳細情報を表示"""
       # カメラタイプを確認
       camera_type = ae.sdk.AEGP_GetCameraType(layerH)

       if camera_type == ae.sdk.AEGP_CameraType_NONE:
           print("このレイヤーはカメラではありません")
           return

       # カメラタイプを表示
       type_names = {
           ae.sdk.AEGP_CameraType_PERSPECTIVE: "パースペクティブ",
           ae.sdk.AEGP_CameraType_ORTHOGRAPHIC: "オルソグラフィック"
       }
       print(f"カメラタイプ: {type_names.get(camera_type, '不明')}")

       # フィルムサイズを取得
       try:
           film_size_units, film_size = ae.sdk.AEGP_GetCameraFilmSize(layerH)

           unit_names = {
               ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL: "水平",
               ae.sdk.AEGP_FilmSizeUnits_VERTICAL: "垂直",
               ae.sdk.AEGP_FilmSizeUnits_DIAGONAL: "対角線"
           }
           unit_name = unit_names.get(film_size_units, "不明")

           print(f"フィルムサイズ: {film_size}px ({unit_name})")
       except Exception as e:
           print(f"フィルムサイズの取得に失敗: {e}")

   # 使用例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetProjectRootFolder(projH)
   compH = ae.sdk.AEGP_GetFirstCompFromItem(itemH)
   camera_layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 0)

   print_camera_info(camera_layerH)

ライトの一括設定
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_three_point_lighting(compH):
       """3点照明（キーライト、フィルライト、バックライト）をセットアップ"""
       # コンポジション情報を取得
       num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)

       lights = []

       # ライトレイヤーを検索
       for i in range(num_layers):
           layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)
           obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)

           if obj_type == ae.sdk.AEGP_ObjectType_LIGHT:
               lights.append(layerH)

       if len(lights) < 3:
           print(f"警告: ライトレイヤーが{len(lights)}個しかありません（3個必要）")
           return

       # キーライト（スポットライト）
       ae.sdk.AEGP_SetLightType(lights[0], ae.sdk.AEGP_LightType_SPOT)
       print("キーライトをスポットライトに設定")

       # フィルライト（ポイントライト）
       ae.sdk.AEGP_SetLightType(lights[1], ae.sdk.AEGP_LightType_POINT)
       print("フィルライトをポイントライトに設定")

       # バックライト（平行光源）
       ae.sdk.AEGP_SetLightType(lights[2], ae.sdk.AEGP_LightType_PARALLEL)
       print("バックライトを平行光源に設定")

       print("3点照明のセットアップが完了しました")

   # 使用例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)
   compH = ae.sdk.AEGP_GetFirstCompFromItem(rootFolderH)

   setup_three_point_lighting(compH)

カメラとライトのスキャン
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def scan_cameras_and_lights(compH):
       """コンポジション内のカメラとライトを検索"""
       num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)

       cameras = []
       lights = []

       for i in range(num_layers):
           layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)
           obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)

           if obj_type == ae.sdk.AEGP_ObjectType_CAMERA:
               camera_type = ae.sdk.AEGP_GetCameraType(layerH)
               layer_name = ae.sdk.AEGP_GetLayerName(layerH)
               cameras.append((layer_name, camera_type))

           elif obj_type == ae.sdk.AEGP_ObjectType_LIGHT:
               light_type = ae.sdk.AEGP_GetLightType(layerH)
               layer_name = ae.sdk.AEGP_GetLayerName(layerH)
               lights.append((layer_name, light_type))

       # 結果を表示
       print("=" * 50)
       print(f"カメラ: {len(cameras)}個")
       for name, cam_type in cameras:
           type_str = "パースペクティブ" if cam_type == ae.sdk.AEGP_CameraType_PERSPECTIVE else "オルソグラフィック"
           print(f"  - {name} ({type_str})")

       print(f"\nライト: {len(lights)}個")
       light_type_names = {
           ae.sdk.AEGP_LightType_PARALLEL: "平行光源",
           ae.sdk.AEGP_LightType_SPOT: "スポットライト",
           ae.sdk.AEGP_LightType_POINT: "ポイントライト",
           ae.sdk.AEGP_LightType_AMBIENT: "アンビエント",
           ae.sdk.AEGP_LightType_ENVIRONMENT: "環境ライト"
       }
       for name, light_type in lights:
           type_str = light_type_names.get(light_type, "不明")
           print(f"  - {name} ({type_str})")
       print("=" * 50)

   # 使用例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)
   compH = ae.sdk.AEGP_GetFirstCompFromItem(rootFolderH)

   scan_cameras_and_lights(compH)

フィルムサイズの設定（35mm相当カメラ）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_35mm_camera(camera_layerH):
       """カメラを35mmフィルム相当に設定"""
       # 35mmフィルムの水平サイズは36mm
       # After Effectsではピクセル単位で指定
       film_size_mm = 36.0

       try:
           ae.sdk.AEGP_SetCameraFilmSize(
               camera_layerH,
               ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL,
               film_size_mm
           )
           print(f"カメラのフィルムサイズを{film_size_mm}mmに設定しました")

           # 確認
           units, size = ae.sdk.AEGP_GetCameraFilmSize(camera_layerH)
           print(f"設定後の値: {size}mm")

       except Exception as e:
           print(f"フィルムサイズの設定に失敗: {e}")

   # 使用例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)
   compH = ae.sdk.AEGP_GetFirstCompFromItem(rootFolderH)
   camera_layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 0)

   setup_35mm_camera(camera_layerH)

環境ライトのセットアップ (AE 24.4+)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_environment_light(light_layerH, hdr_layerH):
       """環境ライトをHDR画像で設定（AE 24.4以降）"""
       # ライトタイプを環境ライトに変更
       try:
           ae.sdk.AEGP_SetLightType(light_layerH, ae.sdk.AEGP_LightType_ENVIRONMENT)
           print("ライトを環境ライトに設定しました")

           # HDR画像を光源として設定
           ae.sdk.AEGP_SetLightSource(light_layerH, hdr_layerH)
           print("HDR画像を光源として設定しました")

           # 確認
           sourceH = ae.sdk.AEGP_GetLightSource(light_layerH)
           if sourceH != 0:
               print("光源が正しく設定されています")
           else:
               print("警告: 光源の設定に失敗しました")

       except Exception as e:
           print(f"環境ライトのセットアップに失敗: {e}")

   # 使用例（AE 24.4以降でのみ動作）
   # projH = ae.sdk.AEGP_GetProjectByIndex(0)
   # ...
   # setup_environment_light(light_layerH, hdr_layerH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **カメラ/ライトレイヤーの確認**

   関数を呼び出す前に、レイヤーが実際にカメラまたはライトであることを確認してください。

   .. code-block:: python

      obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)

      if obj_type == ae.sdk.AEGP_ObjectType_CAMERA:
          # カメラ操作
      elif obj_type == ae.sdk.AEGP_ObjectType_LIGHT:
          # ライト操作

2. **レンダーコンテキスト**

   ``AEGP_GetCamera()`` はレンダーコンテキストからのみ呼び出し可能です。
   通常のスクリプトからは使用できません。

3. **AE 24.4+ の機能**

   ``AEGP_GetLightSource()`` と ``AEGP_SetLightSource()`` はAE 24.4以降でのみ利用可能です。
   古いバージョンではこれらの関数は存在しません。

4. **フィルムサイズの単位**

   フィルムサイズの値は常にピクセル単位です。ミリメートルに変換する場合は、
   コンポジションの解像度とサイズを考慮する必要があります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

レイヤータイプの事前確認
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   obj_type = ae.sdk.AEGP_GetLayerObjectType(layerH)

   if obj_type == ae.sdk.AEGP_ObjectType_CAMERA:
       camera_type = ae.sdk.AEGP_GetCameraType(layerH)

タイプチェック関数の使用
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # カメラタイプが NONE でないことを確認
   camera_type = ae.sdk.AEGP_GetCameraType(layerH)
   if camera_type != ae.sdk.AEGP_CameraType_NONE:
       # カメラ操作

正の値のバリデーション
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   film_size = 36.0
   if film_size > 0:
       ae.sdk.AEGP_SetCameraFilmSize(
           camera_layerH,
           ae.sdk.AEGP_FilmSizeUnits_HORIZONTAL,
           film_size
       )

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       film_size_units, film_size = ae.sdk.AEGP_GetCameraFilmSize(camera_layerH)
   except RuntimeError as e:
       print(f"カメラではないレイヤーです: {e}")

バージョンチェック
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # AE 24.4+ の機能を使用する場合
   try:
       light_sourceH = ae.sdk.AEGP_GetLightSource(light_layerH)
   except AttributeError:
       print("この機能はAE 24.4以降でのみ利用可能です")

関連項目
--------

- :doc:`AEGP_LayerSuite9` - レイヤー管理、オブジェクトタイプ取得
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`AEGP_StreamSuite6` - カメラ/ライトプロパティのストリーム操作
- :doc:`index` - 低レベルAPI概要
