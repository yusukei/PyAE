AEGP_CompSuite12
================

.. currentmodule:: ae.sdk

AEGP_CompSuite12は、After Effectsコンポジションの作成、管理、設定変更を行うためのSDK APIです。

概要
----

**実装状況**: 45/45関数実装 ✅

AEGP_CompSuite12は以下の機能を提供します:

- コンポジションの作成と複製
- コンポジション設定（サイズ、フレームレート、ピクセルアスペクト比、背景色など）
- レイヤー作成（ソリッド、カメラ、ライト、Null、テキスト、ベクターレイヤー）
- ワークエリア管理
- モーションブラー設定
- 表示設定（レイヤー名/ソース名、ブレンドモード表示など）
- レイヤー選択とコレクション管理

基本概念
--------

コンポジションハンドル (AEGP_CompH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのコンポジションを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - コンポジションハンドルは、コンポジションが存在する間のみ有効です
   - コンポジションを削除した後は、ハンドルは無効になります
   - ``AEGP_GetCompFromItem()`` でアイテムからコンポジションハンドルを取得できます
   - ``AEGP_GetItemFromComp()`` でコンポジションからアイテムハンドルを取得できます

Undoable操作
~~~~~~~~~~~~

以下の操作は **Undo可能** です（After EffectsのUndo履歴に記録されます）:

- ``AEGP_SetCompBGColor`` - 背景色設定
- ``AEGP_SetCompFrameRate`` - フレームレート設定
- ``AEGP_SetCompDownSampleFactor`` - ダウンサンプル係数設定
- ``AEGP_SetCompPixelAspectRatio`` - ピクセルアスペクト比設定
- ``AEGP_SetCompDimensions`` - コンポジションサイズ設定
- ``AEGP_SetCompDuration`` - コンポジションデュレーション設定
- レイヤー作成関数すべて
- ``AEGP_SetCompWorkAreaStartAndDuration`` - ワークエリア設定

.. tip::
   複数の変更を行う場合は、``AEGP_StartUndoGroup()`` と ``AEGP_EndUndoGroup()`` でグループ化することを推奨します。

SDKバージョン互換性
~~~~~~~~~~~~~~~~~~~

PyAEはマルチSDKビルドシステムを採用しており、After Effectsのバージョンに応じて自動的に適切なAPIを提供します:

**CompSuite12 (AE 2024.0+)**:
  - ``AEGP_CreateTextLayerInComp`` と ``AEGP_CreateBoxTextLayerInComp`` に ``horizontal`` パラメータあり
  - テキストレイヤーの縦書き/横書きを指定可能

**CompSuite11 (AE 2022/2023)**:
  - ``horizontal`` パラメータなし
  - テキストレイヤーはデフォルト設定で作成

API リファレンス
----------------

コンポジション情報
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetCompFromItem(itemH: int) -> int

   アイテムハンドルからコンポジションハンドルを取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: コンポジションハンドル
   :rtype: int

   .. note::
      アイテムがコンポジションでない場合はエラーになります。
      ``AEGP_GetItemType()`` で事前にタイプを確認することを推奨します。

   **例**:

   .. code-block:: python

      itemH = ae.sdk.AEGP_GetActiveItem()
      compH = ae.sdk.AEGP_GetCompFromItem(itemH)

.. function:: AEGP_GetItemFromComp(compH: int) -> int

   コンポジションハンドルからアイテムハンドルを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: アイテムハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      compH = ae.sdk.AEGP_GetMostRecentlyUsedComp()
      itemH = ae.sdk.AEGP_GetItemFromComp(compH)

.. function:: AEGP_GetMostRecentlyUsedComp() -> int

   最後に使用されたコンポジションハンドルを取得します。

   :return: コンポジションハンドル
   :rtype: int

   .. note::
      「最後に使用された」とは、最後にアクティブだったコンポジションウィンドウを指します。

   **例**:

   .. code-block:: python

      compH = ae.sdk.AEGP_GetMostRecentlyUsedComp()

.. function:: AEGP_GetCompFlags(compH: int) -> int

   コンポジションのフラグ（状態情報）を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: フラグ値（ビットマスク）
   :rtype: int

   **例**:

   .. code-block:: python

      flags = ae.sdk.AEGP_GetCompFlags(compH)

コンポジション設定（取得）
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetCompBGColor(compH: int) -> tuple

   コンポジションの背景色を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: RGB値のタプル (red, green, blue)、各値は0.0～1.0の範囲
   :rtype: tuple

   **例**:

   .. code-block:: python

      r, g, b = ae.sdk.AEGP_GetCompBGColor(compH)
      print(f"背景色: R={r:.2f}, G={g:.2f}, B={b:.2f}")

.. function:: AEGP_GetCompFramerate(compH: int) -> float

   コンポジションのフレームレート（fps）を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: フレームレート（fps）
   :rtype: float

   **例**:

   .. code-block:: python

      fps = ae.sdk.AEGP_GetCompFramerate(compH)
      print(f"フレームレート: {fps} fps")

.. function:: AEGP_GetCompFrameDuration(compH: int) -> float

   1フレームの時間（秒）を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: 1フレームの時間（秒）
   :rtype: float

   .. note::
      フレームデュレーション = 1.0 / フレームレート

   **例**:

   .. code-block:: python

      duration = ae.sdk.AEGP_GetCompFrameDuration(compH)
      print(f"1フレーム: {duration:.6f}秒")

.. function:: AEGP_GetCompDownSampleFactor(compH: int) -> tuple

   コンポジションのダウンサンプル係数を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: (x, y) ダウンサンプル係数のタプル
   :rtype: tuple

   **ダウンサンプル係数**:

   - ``(1, 1)``: フル解像度
   - ``(2, 2)``: 1/2解像度
   - ``(3, 3)``: 1/3解像度
   - ``(4, 4)``: 1/4解像度

   **例**:

   .. code-block:: python

      x, y = ae.sdk.AEGP_GetCompDownSampleFactor(compH)
      print(f"ダウンサンプル: {x}x{y}")

コンポジション設定（変更）
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetCompBGColor(compH: int, r: float, g: float, b: float) -> None

   コンポジションの背景色を設定します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param r: 赤成分（0.0～1.0）
   :type r: float
   :param g: 緑成分（0.0～1.0）
   :type g: float
   :param b: 青成分（0.0～1.0）
   :type b: float

   **例**:

   .. code-block:: python

      # 白背景に設定
      ae.sdk.AEGP_SetCompBGColor(compH, 1.0, 1.0, 1.0)

      # 黒背景に設定
      ae.sdk.AEGP_SetCompBGColor(compH, 0.0, 0.0, 0.0)

.. function:: AEGP_SetCompFrameRate(compH: int, fps: float) -> None

   コンポジションのフレームレートを設定します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param fps: フレームレート（fps）
   :type fps: float

   **一般的なフレームレート**:

   - 23.976: 映画（NTSC）
   - 24.0: 映画
   - 25.0: PAL
   - 29.97: NTSC
   - 30.0: NTSC（ノンドロップ）
   - 60.0: 高フレームレート

   **例**:

   .. code-block:: python

      # 60fpsに設定
      ae.sdk.AEGP_SetCompFrameRate(compH, 60.0)

.. function:: AEGP_SetCompFrameDuration(compH: int, duration: float) -> None

   1フレームの時間を設定します（フレームレートを間接的に設定）（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param duration: 1フレームの時間（秒）
   :type duration: float

   .. note::
      内部的に ``fps = 1.0 / duration`` が計算され、フレームレートが設定されます。

   **例**:

   .. code-block:: python

      # 60fpsに設定（1/60 = 0.01666...秒）
      ae.sdk.AEGP_SetCompFrameDuration(compH, 1.0 / 60.0)

.. function:: AEGP_SetCompDownSampleFactor(compH: int, x: int, y: int) -> None

   コンポジションのダウンサンプル係数を設定します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param x: X方向のダウンサンプル係数（1以上）
   :type x: int
   :param y: Y方向のダウンサンプル係数（1以上）
   :type y: int

   **例**:

   .. code-block:: python

      # フル解像度に設定
      ae.sdk.AEGP_SetCompDownSampleFactor(compH, 1, 1)

      # 1/2解像度に設定
      ae.sdk.AEGP_SetCompDownSampleFactor(compH, 2, 2)

.. function:: AEGP_SetCompPixelAspectRatio(compH: int, num: int, den: int) -> None

   コンポジションのピクセルアスペクト比を設定します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param num: ピクセルアスペクト比の分子
   :type num: int
   :param den: ピクセルアスペクト比の分母
   :type den: int

   **一般的なピクセルアスペクト比**:

   - ``(1, 1)``: 正方形ピクセル（Square）
   - ``(10, 11)``: D1/DV NTSC（0.9091）
   - ``(40, 33)``: D1/DV NTSC Widescreen（1.2121）
   - ``(768, 702)``: D1/DV PAL（1.0940）

   **例**:

   .. code-block:: python

      # 正方形ピクセルに設定
      ae.sdk.AEGP_SetCompPixelAspectRatio(compH, 1, 1)

      # D1/DV NTSCに設定
      ae.sdk.AEGP_SetCompPixelAspectRatio(compH, 10, 11)

.. function:: AEGP_SetCompDimensions(compH: int, width: int, height: int) -> None

   コンポジションのサイズ（幅・高さ）を設定します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int

   **例**:

   .. code-block:: python

      # フルHDに変更
      ae.sdk.AEGP_SetCompDimensions(compH, 1920, 1080)

      # 4Kに変更
      ae.sdk.AEGP_SetCompDimensions(compH, 3840, 2160)

.. function:: AEGP_SetCompDuration(compH: int, duration: float) -> None

   コンポジションのデュレーション（長さ）を設定します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param duration: デュレーション（秒）
   :type duration: float

   **例**:

   .. code-block:: python

      # 10秒に設定
      ae.sdk.AEGP_SetCompDuration(compH, 10.0)

      # 1分に設定
      ae.sdk.AEGP_SetCompDuration(compH, 60.0)

コンポジション作成
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CreateComp(parentFolderH: int, name: str, width: int, height: int, pixel_aspect_num: int, pixel_aspect_den: int, duration: float, framerate: float) -> int

   新しいコンポジションを作成します（**Undo可能**）。

   :param parentFolderH: 親フォルダのアイテムハンドル（0の場合はルートフォルダ）
   :type parentFolderH: int
   :param name: コンポジション名
   :type name: str
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int
   :param pixel_aspect_num: ピクセルアスペクト比の分子
   :type pixel_aspect_num: int
   :param pixel_aspect_den: ピクセルアスペクト比の分母
   :type pixel_aspect_den: int
   :param duration: デュレーション（秒）
   :type duration: float
   :param framerate: フレームレート（fps）
   :type framerate: float
   :return: 新しいコンポジションハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      # フルHD、30fps、10秒のコンポジションを作成
      compH = ae.sdk.AEGP_CreateComp(
          0,  # ルートフォルダ
          "新規コンポジション",
          1920, 1080,  # フルHD
          1, 1,  # 正方形ピクセル
          10.0,  # 10秒
          30.0   # 30fps
      )

.. function:: AEGP_DuplicateComp(compH: int) -> int

   コンポジションを複製します（**Undo可能**）。

   :param compH: 元のコンポジションハンドル
   :type compH: int
   :return: 複製されたコンポジションハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      newCompH = ae.sdk.AEGP_DuplicateComp(compH)

レイヤー作成
~~~~~~~~~~~~

.. function:: AEGP_CreateSolidInComp(compH: int, name: str, width: int, height: int, red: float, green: float, blue: float, duration: float) -> int

   コンポジションにソリッドレイヤーを作成します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param name: ソリッド名
   :type name: str
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int
   :param red: 赤成分（0.0～1.0）
   :type red: float
   :param green: 緑成分（0.0～1.0）
   :type green: float
   :param blue: 青成分（0.0～1.0）
   :type blue: float
   :param duration: デュレーション（秒）
   :type duration: float
   :return: 新しいレイヤーハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      # 赤いソリッド（1920x1080、10秒）を作成
      layerH = ae.sdk.AEGP_CreateSolidInComp(
          compH, "赤背景", 1920, 1080,
          1.0, 0.0, 0.0,  # 赤
          10.0
      )

.. function:: AEGP_CreateCameraInComp(compH: int, name: str) -> int

   コンポジションにカメラレイヤーを作成します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param name: カメラ名
   :type name: str
   :return: 新しいカメラレイヤーハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      cameraH = ae.sdk.AEGP_CreateCameraInComp(compH, "Camera 1")

.. function:: AEGP_CreateLightInComp(compH: int, name: str, light_type: int) -> int

   コンポジションにライトレイヤーを作成します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param name: ライト名
   :type name: str
   :param light_type: ライトタイプ（現在未使用、0を推奨）
   :type light_type: int
   :return: 新しいライトレイヤーハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      lightH = ae.sdk.AEGP_CreateLightInComp(compH, "Light 1", 0)

.. function:: AEGP_CreateNullInComp(compH: int, name: str, duration: float) -> int

   コンポジションにNullオブジェクトを作成します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param name: Null名
   :type name: str
   :param duration: デュレーション（秒）
   :type duration: float
   :return: 新しいNullレイヤーハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      nullH = ae.sdk.AEGP_CreateNullInComp(compH, "Null 1", 10.0)

.. function:: AEGP_CreateTextLayerInComp(compH: int, select_new_layer: bool, horizontal: bool = True) -> int

   コンポジションにテキストレイヤーを作成します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param select_new_layer: 作成後にレイヤーを選択するか
   :type select_new_layer: bool
   :param horizontal: 横書きテキストか（CompSuite12のみ、AE 2024.0+）
   :type horizontal: bool
   :return: 新しいテキストレイヤーハンドル
   :rtype: int

   .. note::
      ``horizontal`` パラメータはAE 2024.0以降（CompSuite12）でのみ利用可能です。
      AE 2022/2023では、このパラメータは無視されます。

   **例（AE 2024.0+）**:

   .. code-block:: python

      # 横書きテキスト
      textH = ae.sdk.AEGP_CreateTextLayerInComp(compH, True, True)

      # 縦書きテキスト
      textH = ae.sdk.AEGP_CreateTextLayerInComp(compH, True, False)

   **例（AE 2022/2023）**:

   .. code-block:: python

      textH = ae.sdk.AEGP_CreateTextLayerInComp(compH, True)

.. function:: AEGP_CreateBoxTextLayerInComp(compH: int, select_new_layer: bool, box_width: float, box_height: float, horizontal: bool = True) -> int

   コンポジションにボックステキストレイヤーを作成します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param select_new_layer: 作成後にレイヤーを選択するか
   :type select_new_layer: bool
   :param box_width: ボックスの幅（ピクセル）
   :type box_width: float
   :param box_height: ボックスの高さ（ピクセル）
   :type box_height: float
   :param horizontal: 横書きテキストか（CompSuite12のみ、AE 2024.0+）
   :type horizontal: bool
   :return: 新しいボックステキストレイヤーハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      # 500x200のボックステキスト（横書き）
      boxTextH = ae.sdk.AEGP_CreateBoxTextLayerInComp(
          compH, True, 500.0, 200.0, True
      )

.. function:: AEGP_CreateVectorLayerInComp(compH: int) -> int

   コンポジションにベクターレイヤー（シェイプレイヤー）を作成します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: 新しいベクターレイヤーハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      shapeH = ae.sdk.AEGP_CreateVectorLayerInComp(compH)

ワークエリア管理
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetCompWorkAreaStart(compH: int) -> float

   ワークエリアの開始時間を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: ワークエリアの開始時間（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      start = ae.sdk.AEGP_GetCompWorkAreaStart(compH)
      print(f"ワークエリア開始: {start}秒")

.. function:: AEGP_GetCompWorkAreaDuration(compH: int) -> float

   ワークエリアのデュレーション（長さ）を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: ワークエリアのデュレーション（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      duration = ae.sdk.AEGP_GetCompWorkAreaDuration(compH)
      print(f"ワークエリア長: {duration}秒")

.. function:: AEGP_SetCompWorkAreaStartAndDuration(compH: int, start: float, duration: float) -> None

   ワークエリアの開始時間とデュレーションを設定します（**Undo可能**）。

   :param compH: コンポジションハンドル
   :type compH: int
   :param start: 開始時間（秒）
   :type start: float
   :param duration: デュレーション（秒）
   :type duration: float

   **例**:

   .. code-block:: python

      # 2秒～7秒をワークエリアに設定
      ae.sdk.AEGP_SetCompWorkAreaStartAndDuration(compH, 2.0, 5.0)

表示設定
~~~~~~~~

.. function:: AEGP_GetShowLayerNameOrSourceName(compH: int) -> bool

   レイヤー名またはソース名のどちらを表示しているかを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: ``True``ならレイヤー名を表示、``False``ならソース名を表示
   :rtype: bool

   **例**:

   .. code-block:: python

      showLayerName = ae.sdk.AEGP_GetShowLayerNameOrSourceName(compH)
      if showLayerName:
          print("レイヤー名を表示中")
      else:
          print("ソース名を表示中")

.. function:: AEGP_SetShowLayerNameOrSourceName(compH: int, show_layer_names: bool) -> None

   レイヤー名またはソース名の表示を切り替えます。

   :param compH: コンポジションハンドル
   :type compH: int
   :param show_layer_names: ``True``ならレイヤー名を表示、``False``ならソース名を表示
   :type show_layer_names: bool

   **例**:

   .. code-block:: python

      # レイヤー名を表示
      ae.sdk.AEGP_SetShowLayerNameOrSourceName(compH, True)

      # ソース名を表示
      ae.sdk.AEGP_SetShowLayerNameOrSourceName(compH, False)

.. function:: AEGP_GetShowBlendModes(compH: int) -> bool

   タイムラインパネルでブレンドモードカラムを表示しているかを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: ``True``なら表示中、``False``なら非表示
   :rtype: bool

   **例**:

   .. code-block:: python

      showBlend = ae.sdk.AEGP_GetShowBlendModes(compH)

.. function:: AEGP_SetShowBlendModes(compH: int, show_blend_modes: bool) -> None

   タイムラインパネルでブレンドモードカラムの表示/非表示を切り替えます。

   :param compH: コンポジションハンドル
   :type compH: int
   :param show_blend_modes: ``True``なら表示、``False``なら非表示
   :type show_blend_modes: bool

   **例**:

   .. code-block:: python

      # ブレンドモードカラムを表示
      ae.sdk.AEGP_SetShowBlendModes(compH, True)

.. function:: AEGP_GetCompDisplayStartTime(compH: int) -> float

   コンポジションの表示開始時間を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: 表示開始時間（秒）
   :rtype: float

   .. note::
      表示開始時間は、タイムラインの0秒地点が実際の何秒に対応するかを示します。

   **例**:

   .. code-block:: python

      startTime = ae.sdk.AEGP_GetCompDisplayStartTime(compH)

.. function:: AEGP_SetCompDisplayStartTime(compH: int, seconds: float) -> None

   コンポジションの表示開始時間を設定します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param seconds: 表示開始時間（秒）
   :type seconds: float

   **例**:

   .. code-block:: python

      # タイムラインを10秒から開始
      ae.sdk.AEGP_SetCompDisplayStartTime(compH, 10.0)

.. function:: AEGP_GetCompDisplayDropFrame(compH: int) -> bool

   ドロップフレーム表示を使用しているかを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: ``True``ならドロップフレーム表示、``False``なら非ドロップフレーム表示
   :rtype: bool

   **例**:

   .. code-block:: python

      dropFrame = ae.sdk.AEGP_GetCompDisplayDropFrame(compH)

.. function:: AEGP_SetCompDisplayDropFrame(compH: int, drop_frame: bool) -> None

   ドロップフレーム表示を設定します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param drop_frame: ``True``ならドロップフレーム表示、``False``なら非ドロップフレーム表示
   :type drop_frame: bool

   **例**:

   .. code-block:: python

      # ドロップフレーム表示を有効化
      ae.sdk.AEGP_SetCompDisplayDropFrame(compH, True)

モーションブラー設定
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetCompShutterAnglePhase(compH: int) -> tuple

   モーションブラーのシャッター角度とシャッターフェーズを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: (angle, phase) シャッター角度とフェーズのタプル（度数法）
   :rtype: tuple

   **例**:

   .. code-block:: python

      angle, phase = ae.sdk.AEGP_GetCompShutterAnglePhase(compH)
      print(f"シャッター角度: {angle}度, フェーズ: {phase}度")

.. function:: AEGP_GetCompShutterFrameRange(compH: int, comp_time: float) -> tuple

   指定時間でのモーションブラーのシャッターフレーム範囲を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param comp_time: コンポジション時間（秒）
   :type comp_time: float
   :return: (start, duration) シャッター開始時間とデュレーション（秒）
   :rtype: tuple

   **例**:

   .. code-block:: python

      start, duration = ae.sdk.AEGP_GetCompShutterFrameRange(compH, 1.0)
      print(f"シャッター範囲: {start}～{start + duration}秒")

.. function:: AEGP_GetCompSuggestedMotionBlurSamples(compH: int) -> int

   推奨されるモーションブラーサンプル数を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: サンプル数
   :rtype: int

   **例**:

   .. code-block:: python

      samples = ae.sdk.AEGP_GetCompSuggestedMotionBlurSamples(compH)
      print(f"推奨サンプル数: {samples}")

.. function:: AEGP_SetCompSuggestedMotionBlurSamples(compH: int, samples: int) -> None

   推奨されるモーションブラーサンプル数を設定します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param samples: サンプル数（1以上）
   :type samples: int

   **例**:

   .. code-block:: python

      # サンプル数を16に設定
      ae.sdk.AEGP_SetCompSuggestedMotionBlurSamples(compH, 16)

.. function:: AEGP_GetCompMotionBlurAdaptiveSampleLimit(compH: int) -> int

   適応サンプリングの上限値を取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: 適応サンプリング上限値
   :rtype: int

   **例**:

   .. code-block:: python

      limit = ae.sdk.AEGP_GetCompMotionBlurAdaptiveSampleLimit(compH)

.. function:: AEGP_SetCompMotionBlurAdaptiveSampleLimit(compH: int, samples: int) -> None

   適応サンプリングの上限値を設定します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param samples: 適応サンプリング上限値（1以上）
   :type samples: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetCompMotionBlurAdaptiveSampleLimit(compH, 128)

コレクション・選択管理
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewCollectionFromCompSelection(compH: int) -> int

   コンポジションの現在の選択からコレクションを作成します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: コレクションハンドル
   :rtype: int

   .. note::
      コレクションは使用後に ``AEGP_DisposeCollection()`` で解放する必要があります。

   **例**:

   .. code-block:: python

      collectionH = ae.sdk.AEGP_GetNewCollectionFromCompSelection(compH)
      # ... コレクションを使用
      ae.sdk.AEGP_DisposeCollection(collectionH)

.. function:: AEGP_SetSelection(compH: int, collectionH: int) -> None

   コンポジションの選択を設定します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param collectionH: コレクションハンドル
   :type collectionH: int

   **例**:

   .. code-block:: python

      # 選択を変更
      ae.sdk.AEGP_SetSelection(compH, collectionH)

.. function:: AEGP_ReorderCompSelection(compH: int, index: int) -> None

   選択したレイヤーを指定インデックスに並べ替えます。

   :param compH: コンポジションハンドル
   :type compH: int
   :param index: 移動先のインデックス
   :type index: int

   **例**:

   .. code-block:: python

      # 選択したレイヤーを先頭に移動
      ae.sdk.AEGP_ReorderCompSelection(compH, 0)

マーカー管理
~~~~~~~~~~~~

.. function:: AEGP_GetNewCompMarkerStream(compH: int) -> int

   コンポジションマーカーのストリームを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: ストリームハンドル
   :rtype: int

   .. note::
      ストリームは ``AEGP_StreamSuite`` の関数でマーカーの追加・削除・取得に使用できます。

   **例**:

   .. code-block:: python

      streamH = ae.sdk.AEGP_GetNewCompMarkerStream(compH)
      # ... ストリームを使用してマーカーを操作

使用例
------

コンポジション作成と基本設定
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_composition():
       """新しいコンポジションを作成して基本設定を行う"""
       # コンポジションを作成
       compH = ae.sdk.AEGP_CreateComp(
           0,  # ルートフォルダ
           "My Composition",
           1920, 1080,  # フルHD
           1, 1,  # 正方形ピクセル
           10.0,  # 10秒
           30.0   # 30fps
       )

       # 背景色を白に設定
       ae.sdk.AEGP_SetCompBGColor(compH, 1.0, 1.0, 1.0)

       # ダウンサンプルをフル解像度に設定
       ae.sdk.AEGP_SetCompDownSampleFactor(compH, 1, 1)

       # ワークエリアを2秒～8秒に設定
       ae.sdk.AEGP_SetCompWorkAreaStartAndDuration(compH, 2.0, 6.0)

       print(f"コンポジション作成完了: compH={compH}")
       return compH

   # 実行
   compH = create_composition()

レイヤー一括作成
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_basic_scene(compH):
       """基本的なシーンセットアップ（背景、カメラ、ライト）"""
       # Undoグループ開始
       ae.sdk.AEGP_StartUndoGroup("シーンセットアップ")

       try:
           # 背景ソリッド（青）
           bgH = ae.sdk.AEGP_CreateSolidInComp(
               compH, "Background", 1920, 1080,
               0.2, 0.4, 0.8,  # 青
               10.0
           )

           # カメラ作成
           cameraH = ae.sdk.AEGP_CreateCameraInComp(compH, "Camera 1")

           # ライト作成
           lightH = ae.sdk.AEGP_CreateLightInComp(compH, "Key Light", 0)

           # Null作成（コントローラー用）
           nullH = ae.sdk.AEGP_CreateNullInComp(compH, "Controller", 10.0)

           # テキストレイヤー作成
           textH = ae.sdk.AEGP_CreateTextLayerInComp(compH, False, True)

           print("シーンセットアップ完了")

       finally:
           # Undoグループ終了
           ae.sdk.AEGP_EndUndoGroup()

   # 実行
   compH = ae.sdk.AEGP_GetMostRecentlyUsedComp()
   setup_basic_scene(compH)

コンポジション設定の一括変更
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def optimize_for_preview(compH):
       """プレビュー用にコンポジション設定を最適化"""
       ae.sdk.AEGP_StartUndoGroup("プレビュー最適化")

       try:
           # ダウンサンプルを1/2に設定
           ae.sdk.AEGP_SetCompDownSampleFactor(compH, 2, 2)

           # ブレンドモード表示を無効化
           ae.sdk.AEGP_SetShowBlendModes(compH, False)

           # モーションブラーサンプル数を削減
           ae.sdk.AEGP_SetCompSuggestedMotionBlurSamples(compH, 4)
           ae.sdk.AEGP_SetCompMotionBlurAdaptiveSampleLimit(compH, 32)

           print("プレビュー最適化完了")

       finally:
           ae.sdk.AEGP_EndUndoGroup()

   def optimize_for_final_render(compH):
       """最終レンダリング用にコンポジション設定を最適化"""
       ae.sdk.AEGP_StartUndoGroup("レンダリング最適化")

       try:
           # フル解像度に設定
           ae.sdk.AEGP_SetCompDownSampleFactor(compH, 1, 1)

           # モーションブラーサンプル数を増やす
           ae.sdk.AEGP_SetCompSuggestedMotionBlurSamples(compH, 32)
           ae.sdk.AEGP_SetCompMotionBlurAdaptiveSampleLimit(compH, 256)

           print("レンダリング最適化完了")

       finally:
           ae.sdk.AEGP_EndUndoGroup()

   # 実行例
   compH = ae.sdk.AEGP_GetMostRecentlyUsedComp()
   optimize_for_preview(compH)  # プレビュー用
   # optimize_for_final_render(compH)  # レンダリング用

コンポジション情報のダンプ
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_comp_info(compH):
       """コンポジションの詳細情報を表示"""
       # アイテム情報
       itemH = ae.sdk.AEGP_GetItemFromComp(compH)
       name = ae.sdk.AEGP_GetItemName(itemH)

       # 基本設定
       fps = ae.sdk.AEGP_GetCompFramerate(compH)
       frameDuration = ae.sdk.AEGP_GetCompFrameDuration(compH)
       r, g, b = ae.sdk.AEGP_GetCompBGColor(compH)
       dsfX, dsfY = ae.sdk.AEGP_GetCompDownSampleFactor(compH)

       # ワークエリア
       waStart = ae.sdk.AEGP_GetCompWorkAreaStart(compH)
       waDuration = ae.sdk.AEGP_GetCompWorkAreaDuration(compH)

       # 表示設定
       showLayerName = ae.sdk.AEGP_GetShowLayerNameOrSourceName(compH)
       showBlend = ae.sdk.AEGP_GetShowBlendModes(compH)
       dropFrame = ae.sdk.AEGP_GetCompDisplayDropFrame(compH)

       # モーションブラー
       angle, phase = ae.sdk.AEGP_GetCompShutterAnglePhase(compH)
       mbSamples = ae.sdk.AEGP_GetCompSuggestedMotionBlurSamples(compH)
       mbLimit = ae.sdk.AEGP_GetCompMotionBlurAdaptiveSampleLimit(compH)

       print("=" * 60)
       print(f"コンポジション名: {name}")
       print(f"フレームレート: {fps:.2f} fps")
       print(f"フレームデュレーション: {frameDuration:.6f}秒")
       print(f"背景色: R={r:.2f}, G={g:.2f}, B={b:.2f}")
       print(f"ダウンサンプル: {dsfX}x{dsfY}")
       print(f"ワークエリア: {waStart:.2f}～{waStart + waDuration:.2f}秒 ({waDuration:.2f}秒)")
       print(f"レイヤー名表示: {'レイヤー名' if showLayerName else 'ソース名'}")
       print(f"ブレンドモード表示: {'有効' if showBlend else '無効'}")
       print(f"ドロップフレーム: {'有効' if dropFrame else '無効'}")
       print(f"シャッター角度: {angle:.1f}度, フェーズ: {phase:.1f}度")
       print(f"モーションブラーサンプル: {mbSamples}, 上限: {mbLimit}")
       print("=" * 60)

   # 実行
   compH = ae.sdk.AEGP_GetMostRecentlyUsedComp()
   dump_comp_info(compH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **コンポジションハンドルの有効期限**

   コンポジションハンドルは、コンポジションが存在する間のみ有効です。
   コンポジションを削除した後は、ハンドルは無効になります。

2. **Undo操作**

   多くの設定変更関数はUndo可能です。
   複数の変更を行う場合は、``AEGP_StartUndoGroup()`` でグループ化してください。

3. **SDK バージョン互換性**

   ``AEGP_CreateTextLayerInComp`` と ``AEGP_CreateBoxTextLayerInComp`` の
   ``horizontal`` パラメータはAE 2024.0以降でのみ利用可能です。
   古いバージョンでは、このパラメータは無視されます。

4. **カラー値の範囲**

   ``AEGP_SetCompBGColor`` と ``AEGP_CreateSolidInComp`` のカラー値は0.0～1.0の範囲です。
   255階調の値を使う場合は、255で割ってください。

5. **時間値の単位**

   時間値はすべて秒単位です。フレーム番号ではありません。
   フレーム番号から秒に変換するには、フレームレートで割ってください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("処理名")
   try:
       # 複数の変更
       ae.sdk.AEGP_SetCompBGColor(compH, 1.0, 1.0, 1.0)
       ae.sdk.AEGP_SetCompFrameRate(compH, 60.0)
   finally:
       ae.sdk.AEGP_EndUndoGroup()

コンポジションハンドルの再利用
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 良い例: ハンドルを変数に保存
   compH = ae.sdk.AEGP_GetMostRecentlyUsedComp()
   ae.sdk.AEGP_SetCompBGColor(compH, 1.0, 1.0, 1.0)
   ae.sdk.AEGP_SetCompFrameRate(compH, 60.0)

   # 悪い例: 毎回取得
   ae.sdk.AEGP_SetCompBGColor(ae.sdk.AEGP_GetMostRecentlyUsedComp(), 1.0, 1.0, 1.0)
   ae.sdk.AEGP_SetCompFrameRate(ae.sdk.AEGP_GetMostRecentlyUsedComp(), 60.0)

カラー値の扱い
^^^^^^^^^^^^^^

.. code-block:: python

   # RGB 255階調から変換
   r, g, b = 255, 128, 0  # オレンジ
   ae.sdk.AEGP_SetCompBGColor(compH, r/255.0, g/255.0, b/255.0)

   # 16進数カラーから変換
   hex_color = 0xFF8000  # オレンジ
   r = ((hex_color >> 16) & 0xFF) / 255.0
   g = ((hex_color >> 8) & 0xFF) / 255.0
   b = (hex_color & 0xFF) / 255.0
   ae.sdk.AEGP_SetCompBGColor(compH, r, g, b)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       compH = ae.sdk.AEGP_CreateComp(
           0, "Test", 1920, 1080,
           1, 1, 10.0, 30.0
       )
   except Exception as e:
       print(f"コンポジション作成失敗: {e}")

関連項目
--------

- :doc:`AEGP_ItemSuite9` - プロジェクトアイテム管理
- :doc:`AEGP_LayerSuite9` - レイヤー管理
- :doc:`AEGP_CollectionSuite2` - コレクション管理
- :doc:`AEGP_StreamSuite6` - ストリーム管理（マーカー操作）
- :doc:`AEGP_UtilitySuite6` - Undo管理、エラー処理
- :doc:`index` - 低レベルAPI概要
