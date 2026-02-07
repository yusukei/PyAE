AEGP_FootageSuite5
==================

.. currentmodule:: ae.sdk

AEGP_FootageSuite5は、After Effectsのフッテージ（素材）を管理、作成、設定するためのSDK APIです。

概要
----

AEGP_FootageSuite5は以下の機能を提供します:

- フッテージハンドルの取得（メイン・プロキシ）
- フッテージの作成（ファイル、ソリッド、プレースホルダー）
- フッテージ情報の取得（パス、署名、ファイル数）
- フッテージ解釈設定の取得・設定
- ソリッドフッテージの属性管理
- プロジェクトへのフッテージ追加

基本概念
--------

フッテージハンドル (AEGP_FootageH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

フッテージを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - フッテージハンドルは、プロジェクトに追加されるまで明示的に破棄する必要があります
   - プロジェクトに追加されたフッテージは、プロジェクトが所有権を持ち、自動的に管理されます
   - 未追加のフッテージは ``AEGP_DisposeFootage()`` で破棄してください

メインフッテージとプロキシフッテージ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのアイテムは2種類のフッテージを持つことができます:

- **メインフッテージ** - オリジナルの高解像度素材
- **プロキシフッテージ** - 編集用の低解像度素材（オプション）

プロキシフッテージが設定されている場合、After Effectsはプレビュー時にプロキシを使用し、最終レンダリング時にメインフッテージを使用します。

フッテージの種類
~~~~~~~~~~~~~~~~

AEGP_FootageSuite5は以下の種類のフッテージを扱えます:

- **ファイルフッテージ** - 画像・動画ファイルから作成
- **ソリッドフッテージ** - 単色の平面レイヤー
- **プレースホルダーフッテージ** - 実際のファイルがない仮の素材
- **シーケンスフッテージ** - 連番画像ファイル

フッテージ署名 (AEGP_FootageSignature)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

フッテージの種類を示す識別子:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_FootageSignature_NONE``
     - 0
     - 通常のフッテージ
   * - ``AEGP_FootageSignature_MISSING``
     -
     - ファイルが見つからないフッテージ
   * - ``AEGP_FootageSignature_SOLID``
     -
     - ソリッドフッテージ

API リファレンス
----------------

フッテージハンドルの取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetMainFootageFromItem(itemH: int) -> int

   アイテムのメインフッテージハンドルを取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: フッテージハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      footageH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)

.. function:: AEGP_GetProxyFootageFromItem(itemH: int) -> int

   アイテムのプロキシフッテージハンドルを取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :return: プロキシフッテージハンドル（プロキシが設定されていない場合は ``0``）
   :rtype: int

   .. note::
      この関数は、プロキシが設定されていない場合にエラーを出さず ``0`` を返します。

   **例**:

   .. code-block:: python

      proxyH = ae.sdk.AEGP_GetProxyFootageFromItem(itemH)
      if proxyH:
          print("プロキシが設定されています")
      else:
          print("プロキシはありません")

フッテージ情報の取得
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetFootageNumFiles(footageH: int) -> tuple[int, int]

   フッテージのファイル数を取得します。

   :param footageH: フッテージハンドル
   :type footageH: int
   :return: ``(total_files, files_per_frame)`` のタプル
   :rtype: tuple[int, int]

   **戻り値**:

   - ``total_files``: 総ファイル数
   - ``files_per_frame``: 1フレームあたりのファイル数（通常は1、ステレオ映像などで2以上）

   **例**:

   .. code-block:: python

      num_files, files_per_frame = ae.sdk.AEGP_GetFootageNumFiles(footageH)
      print(f"総ファイル数: {num_files}, フレームあたり: {files_per_frame}")

.. function:: AEGP_GetFootagePath(plugin_id: int, footageH: int, frame_num: int, file_index: int) -> str

   フッテージの特定フレーム・ファイルのパスを取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param footageH: フッテージハンドル
   :type footageH: int
   :param frame_num: フレーム番号（0から始まる）
   :type frame_num: int
   :param file_index: ファイルインデックス（通常は0）
   :type file_index: int
   :return: ファイルパス（UTF-8文字列）
   :rtype: str

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      path = ae.sdk.AEGP_GetFootagePath(plugin_id, footageH, 0, 0)
      print(f"フッテージパス: {path}")

.. function:: AEGP_GetFootageSignature(footageH: int) -> int

   フッテージの署名（種類）を取得します。

   :param footageH: フッテージハンドル
   :type footageH: int
   :return: フッテージ署名（``AEGP_FootageSignature_*`` 定数）
   :rtype: int

   **例**:

   .. code-block:: python

      sig = ae.sdk.AEGP_GetFootageSignature(footageH)
      if sig == ae.sdk.AEGP_FootageSignature_SOLID:
          print("ソリッドレイヤーです")
      elif sig == ae.sdk.AEGP_FootageSignature_MISSING:
          print("ファイルが見つかりません")

フッテージの作成
~~~~~~~~~~~~~~~~

.. function:: AEGP_NewFootage(plugin_id: int, path: str, interp_style: int) -> int

   ファイルから新しいフッテージを作成します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param path: ファイルパス
   :type path: str
   :param interp_style: 解釈スタイル（``AEGP_InterpretationStyle_*`` 定数）
   :type interp_style: int
   :return: 新しいフッテージハンドル
   :rtype: int

   **解釈スタイル**:

   .. list-table::
      :header-rows: 1

      * - 定数
        - 説明
      * - ``AEGP_InterpretationStyle_NO_DIALOG_GUESS``
        - ダイアログを表示せず、自動推測
      * - ``AEGP_InterpretationStyle_DIALOG_OK``
        - 必要に応じてダイアログを表示
      * - ``AEGP_InterpretationStyle_NO_DIALOG_NO_GUESS``
        - ダイアログも推測もしない

   .. warning::
      作成したフッテージは、``AEGP_AddFootageToProject()`` でプロジェクトに追加するか、
      ``AEGP_DisposeFootage()`` で破棄する必要があります。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      footageH = ae.sdk.AEGP_NewFootage(
          plugin_id,
          r"C:\Media\video.mp4",
          ae.sdk.AEGP_InterpretationStyle_NO_DIALOG_GUESS
      )

.. function:: AEGP_NewSolidFootage(name: str, width: int, height: int, color_rgba: tuple) -> int

   新しいソリッドフッテージを作成します。

   :param name: ソリッドの名前
   :type name: str
   :param width: 幅（1-30000ピクセル）
   :type width: int
   :param height: 高さ（1-30000ピクセル）
   :type height: int
   :param color_rgba: 色 ``(r, g, b)`` または ``(r, g, b, a)``（各値0.0-1.0の浮動小数点）
   :type color_rgba: tuple
   :return: 新しいフッテージハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      # 赤いソリッド（1920x1080）
      footageH = ae.sdk.AEGP_NewSolidFootage(
          "Red Solid",
          1920, 1080,
          (1.0, 0.0, 0.0)  # 赤
      )

      # 半透明の青いソリッド
      footageH = ae.sdk.AEGP_NewSolidFootage(
          "Blue Solid",
          1920, 1080,
          (0.0, 0.0, 1.0, 0.5)  # 青、不透明度50%
      )

.. function:: AEGP_NewPlaceholderFootage(plugin_id: int, name: str, width: int, height: int, duration_sec: float) -> int

   新しいプレースホルダーフッテージを作成します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param name: プレースホルダーの名前
   :type name: str
   :param width: 幅（1-30000ピクセル）
   :type width: int
   :param height: 高さ（1-30000ピクセル）
   :type height: int
   :param duration_sec: デュレーション（秒）
   :type duration_sec: float
   :return: 新しいフッテージハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      footageH = ae.sdk.AEGP_NewPlaceholderFootage(
          plugin_id,
          "Placeholder",
          1920, 1080,
          10.0  # 10秒
      )

.. function:: AEGP_NewPlaceholderFootageWithPath(plugin_id: int, path: str, path_platform: int, file_type: int, width: int, height: int, duration_sec: float) -> int

   パス情報付きのプレースホルダーフッテージを作成します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param path: ファイルパス（実際には存在しない）
   :type path: str
   :param path_platform: プラットフォーム（``AEGP_Platform`` 定数）
   :type path_platform: int
   :param file_type: ファイルタイプ（``AEIO_FileType`` 定数）
   :type file_type: int
   :param width: 幅（1-30000ピクセル）
   :type width: int
   :param height: 高さ（1-30000ピクセル）
   :type height: int
   :param duration_sec: デュレーション（秒）
   :type duration_sec: float
   :return: 新しいフッテージハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      footageH = ae.sdk.AEGP_NewPlaceholderFootageWithPath(
          plugin_id,
          r"C:\Media\missing_file.mp4",
          0,  # platform
          0,  # file_type
          1920, 1080,
          10.0
      )

プロジェクトへの追加
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_AddFootageToProject(footageH: int, folderH: int) -> int

   フッテージをプロジェクトのフォルダに追加します。

   :param footageH: フッテージハンドル
   :type footageH: int
   :param folderH: 追加先のフォルダアイテムハンドル（``AEGP_GetProjectRootFolder()`` で取得）
   :type folderH: int
   :return: 新しく作成されたアイテムハンドル
   :rtype: int

   .. important::
      フッテージがプロジェクトに追加されると、プロジェクトが所有権を持ちます。
      ``AEGP_DisposeFootage()`` を呼び出してはいけません。

   **例**:

   .. code-block:: python

      # プロジェクトのルートフォルダを取得
      projH = ae.sdk.AEGP_GetProjectByIndex(0)
      rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

      # フッテージを追加
      itemH = ae.sdk.AEGP_AddFootageToProject(footageH, rootFolderH)

フッテージの変更
~~~~~~~~~~~~~~~~

.. function:: AEGP_ReplaceItemMainFootage(footageH: int, itemH: int) -> None

   アイテムのメインフッテージを置き換えます。

   :param footageH: 新しいフッテージハンドル
   :type footageH: int
   :param itemH: 置き換え対象のアイテムハンドル
   :type itemH: int

   .. important::
      フッテージはプロジェクトに採用されます。``AEGP_DisposeFootage()`` を呼び出してはいけません。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      new_footageH = ae.sdk.AEGP_NewFootage(
          plugin_id,
          r"C:\Media\new_video.mp4",
          ae.sdk.AEGP_InterpretationStyle_NO_DIALOG_GUESS
      )
      ae.sdk.AEGP_ReplaceItemMainFootage(new_footageH, itemH)

.. function:: AEGP_SetItemProxyFootage(itemH: int, footageH: int) -> None

   アイテムのプロキシフッテージを設定します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param footageH: プロキシフッテージハンドル
   :type footageH: int

   .. important::
      フッテージはプロジェクトに採用されます。``AEGP_DisposeFootage()`` を呼び出してはいけません。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      proxy_footageH = ae.sdk.AEGP_NewFootage(
          plugin_id,
          r"C:\Media\proxy_video.mp4",
          ae.sdk.AEGP_InterpretationStyle_NO_DIALOG_GUESS
      )
      ae.sdk.AEGP_SetItemProxyFootage(itemH, proxy_footageH)

.. function:: AEGP_DisposeFootage(footageH: int) -> None

   フッテージハンドルを破棄します。

   :param footageH: フッテージハンドル
   :type footageH: int

   .. warning::
      この関数は、フッテージがプロジェクトに追加されていない場合のみ使用してください。
      プロジェクトに追加されたフッテージを破棄するとクラッシュします。

   **例**:

   .. code-block:: python

      # フッテージを作成
      footageH = ae.sdk.AEGP_NewSolidFootage("Temp", 100, 100, (1, 0, 0))

      # 使わない場合は破棄
      ae.sdk.AEGP_DisposeFootage(footageH)

フッテージ解釈設定
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetFootageInterpretation(itemH: int, proxy: bool = False) -> dict

   フッテージの解釈設定を取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param proxy: プロキシフッテージの設定を取得する場合は ``True``（デフォルト: ``False``）
   :type proxy: bool
   :return: 解釈設定の辞書
   :rtype: dict

   **戻り値の構造** (``AEGP_FootageInterp``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``interlace_type``
        - int
        - インターレース種類
      * - ``interlace_order``
        - int
        - インターレース順序
      * - ``alpha_flags``
        - int
        - アルファチャンネルフラグ
      * - ``alpha_premul_color``
        - tuple[int, int, int]
        - アルファプリマルチプライカラー（RGB、0-255）
      * - ``pulldown_phase``
        - int
        - プルダウンフェーズ
      * - ``loop_count``
        - int
        - ループ回数
      * - ``pixel_aspect_num``
        - int
        - ピクセルアスペクト比の分子
      * - ``pixel_aspect_den``
        - int
        - ピクセルアスペクト比の分母
      * - ``native_fps``
        - float
        - ネイティブフレームレート
      * - ``conform_fps``
        - float
        - 調整後のフレームレート
      * - ``depth``
        - int
        - ビット深度
      * - ``motion_detect``
        - bool
        - モーション検出の有効/無効

   **例**:

   .. code-block:: python

      interp = ae.sdk.AEGP_GetFootageInterpretation(itemH)
      print(f"フレームレート: {interp['native_fps']}")
      print(f"アルファフラグ: {interp['alpha_flags']}")

.. function:: AEGP_SetFootageInterpretation(itemH: int, proxy: bool, interp_dict: dict) -> None

   フッテージの解釈設定を変更します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param proxy: プロキシフッテージの設定を変更する場合は ``True``
   :type proxy: bool
   :param interp_dict: 解釈設定の辞書（変更したいキーのみ含む）
   :type interp_dict: dict

   .. note::
      すべてのキーを含む必要はありません。変更したいキーのみを含む辞書を渡してください。
      内部で現在の設定を取得し、指定されたキーのみを上書きします。

   **例**:

   .. code-block:: python

      # フレームレートを24fpsに変更
      ae.sdk.AEGP_SetFootageInterpretation(itemH, False, {
          "conform_fps": 24.0
      })

      # アルファチャンネルをストレートに変更
      ae.sdk.AEGP_SetFootageInterpretation(itemH, False, {
          "alpha_flags": 1  # Straight alpha
      })

ソリッドフッテージ属性
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetSolidFootageColor(itemH: int, proxy: bool = False) -> tuple[float, float, float, float]

   ソリッドフッテージの色を取得します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param proxy: プロキシフッテージの色を取得する場合は ``True``（デフォルト: ``False``）
   :type proxy: bool
   :return: ``(r, g, b, a)`` の色（各値0.0-1.0の浮動小数点）
   :rtype: tuple[float, float, float, float]

   .. warning::
      この関数はソリッドフッテージに対してのみ使用できます。
      他の種類のフッテージで呼び出すとエラーになります。

   **例**:

   .. code-block:: python

      r, g, b, a = ae.sdk.AEGP_GetSolidFootageColor(itemH)
      print(f"ソリッドカラー: RGB({r}, {g}, {b}), Alpha={a}")

.. function:: AEGP_SetSolidFootageColor(itemH: int, proxy: bool, color_rgba: tuple) -> None

   ソリッドフッテージの色を設定します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param proxy: プロキシフッテージの色を設定する場合は ``True``
   :type proxy: bool
   :param color_rgba: ``(r, g, b)`` または ``(r, g, b, a)``（各値0.0-1.0の浮動小数点）
   :type color_rgba: tuple

   .. warning::
      この関数はソリッドフッテージに対してのみ使用できます。

   **例**:

   .. code-block:: python

      # 緑色に変更
      ae.sdk.AEGP_SetSolidFootageColor(itemH, False, (0.0, 1.0, 0.0))

.. function:: AEGP_SetSolidFootageDimensions(itemH: int, proxy: bool, width: int, height: int) -> None

   ソリッドフッテージのサイズを設定します。

   :param itemH: アイテムハンドル
   :type itemH: int
   :param proxy: プロキシフッテージのサイズを設定する場合は ``True``
   :type proxy: bool
   :param width: 幅（1-30000ピクセル）
   :type width: int
   :param height: 高さ（1-30000ピクセル）
   :type height: int

   .. warning::
      この関数はソリッドフッテージに対してのみ使用できます。

   **例**:

   .. code-block:: python

      # 4K サイズに変更
      ae.sdk.AEGP_SetSolidFootageDimensions(itemH, False, 3840, 2160)

追加情報の取得
~~~~~~~~~~~~~~

.. function:: AEGP_GetFootageLayerKey(footageH: int) -> dict

   フッテージのレイヤーキー情報を取得します。

   :param footageH: フッテージハンドル
   :type footageH: int
   :return: レイヤーキー情報の辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``layer_id``
        - int
        - レイヤーID
      * - ``layer_index``
        - int
        - レイヤーインデックス
      * - ``name``
        - str
        - レイヤー名
      * - ``layer_draw_style``
        - int
        - レイヤー描画スタイル

   **例**:

   .. code-block:: python

      layer_key = ae.sdk.AEGP_GetFootageLayerKey(footageH)
      print(f"レイヤー名: {layer_key['name']}")

.. function:: AEGP_GetFootageSoundDataFormat(footageH: int) -> dict

   フッテージの音声データ形式を取得します。

   :param footageH: フッテージハンドル
   :type footageH: int
   :return: 音声データ形式の辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``sample_rate``
        - float
        - サンプリングレート（Hz）
      * - ``encoding``
        - int
        - エンコーディング形式
      * - ``bytes_per_sample``
        - int
        - サンプルあたりのバイト数
      * - ``num_channels``
        - int
        - チャンネル数

   **例**:

   .. code-block:: python

      sound = ae.sdk.AEGP_GetFootageSoundDataFormat(footageH)
      print(f"サンプリングレート: {sound['sample_rate']} Hz")
      print(f"チャンネル数: {sound['num_channels']}")

.. function:: AEGP_GetFootageSequenceImportOptions(footageH: int) -> dict

   フッテージのシーケンスインポートオプションを取得します。

   :param footageH: フッテージハンドル
   :type footageH: int
   :return: シーケンスインポートオプションの辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``all_in_folder``
        - bool
        - フォルダ内のすべてのファイルを含むか
      * - ``force_alphabetical``
        - bool
        - アルファベット順を強制するか
      * - ``start_frame``
        - int
        - 開始フレーム番号
      * - ``end_frame``
        - int
        - 終了フレーム番号

   **例**:

   .. code-block:: python

      seq_opts = ae.sdk.AEGP_GetFootageSequenceImportOptions(footageH)
      print(f"フレーム範囲: {seq_opts['start_frame']}-{seq_opts['end_frame']}")

使用例
------

フッテージの追加と基本操作
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def add_footage_to_project(file_path):
       """ファイルをプロジェクトに追加"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # プロジェクトとルートフォルダを取得
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

       # フッテージを作成
       footageH = ae.sdk.AEGP_NewFootage(
           plugin_id,
           file_path,
           ae.sdk.AEGP_InterpretationStyle_NO_DIALOG_GUESS
       )

       # プロジェクトに追加
       itemH = ae.sdk.AEGP_AddFootageToProject(footageH, rootFolderH)
       print(f"フッテージを追加しました: {file_path}")

       return itemH

   # 使用例
   itemH = add_footage_to_project(r"C:\Media\video.mp4")

ソリッドレイヤーの作成
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_solid_layer(name, width, height, color):
       """ソリッドレイヤーをプロジェクトに追加"""
       # プロジェクトとルートフォルダを取得
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

       # ソリッドフッテージを作成
       footageH = ae.sdk.AEGP_NewSolidFootage(
           name,
           width,
           height,
           color
       )

       # プロジェクトに追加
       itemH = ae.sdk.AEGP_AddFootageToProject(footageH, rootFolderH)
       print(f"ソリッドを作成しました: {name}")

       return itemH

   # カラフルなソリッドを作成
   red_solid = create_solid_layer("Red BG", 1920, 1080, (1.0, 0.0, 0.0))
   green_solid = create_solid_layer("Green BG", 1920, 1080, (0.0, 1.0, 0.0))
   blue_solid = create_solid_layer("Blue BG", 1920, 1080, (0.0, 0.0, 1.0))

プロキシの設定
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_proxy_for_item(itemH, proxy_path):
       """アイテムにプロキシを設定"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # プロキシフッテージを作成
       proxy_footageH = ae.sdk.AEGP_NewFootage(
           plugin_id,
           proxy_path,
           ae.sdk.AEGP_InterpretationStyle_NO_DIALOG_GUESS
       )

       # プロキシを設定
       ae.sdk.AEGP_SetItemProxyFootage(itemH, proxy_footageH)
       print(f"プロキシを設定しました: {proxy_path}")

   # 使用例
   set_proxy_for_item(itemH, r"C:\Media\proxy_video.mp4")

フッテージ情報の取得
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_footage_info(itemH):
       """フッテージの詳細情報を表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # メインフッテージを取得
       footageH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)

       # 基本情報
       sig = ae.sdk.AEGP_GetFootageSignature(footageH)
       num_files, files_per_frame = ae.sdk.AEGP_GetFootageNumFiles(footageH)

       print("=" * 50)
       print("フッテージ情報:")
       print(f"  署名: {sig}")
       print(f"  ファイル数: {num_files}")
       print(f"  フレームあたりのファイル数: {files_per_frame}")

       # ソリッドの場合、色を表示
       if sig == ae.sdk.AEGP_FootageSignature_SOLID:
           r, g, b, a = ae.sdk.AEGP_GetSolidFootageColor(itemH)
           print(f"  ソリッドカラー: RGB({r:.2f}, {g:.2f}, {b:.2f}), Alpha={a:.2f}")

       # 解釈設定
       interp = ae.sdk.AEGP_GetFootageInterpretation(itemH)
       print(f"  ネイティブFPS: {interp['native_fps']}")
       print(f"  調整FPS: {interp['conform_fps']}")
       print(f"  ビット深度: {interp['depth']}")

       # プロキシの確認
       proxyH = ae.sdk.AEGP_GetProxyFootageFromItem(itemH)
       if proxyH:
           print("  プロキシ: 設定あり")
       else:
           print("  プロキシ: なし")

       print("=" * 50)

   # 使用例
   dump_footage_info(itemH)

フッテージの解釈設定変更
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_footage_framerate(itemH, new_fps):
       """フッテージのフレームレートを変更"""
       # 現在の設定を取得
       interp = ae.sdk.AEGP_GetFootageInterpretation(itemH)
       old_fps = interp['conform_fps']

       # フレームレートを変更
       ae.sdk.AEGP_SetFootageInterpretation(itemH, False, {
           "conform_fps": new_fps
       })

       print(f"フレームレートを変更: {old_fps} -> {new_fps} fps")

   def set_straight_alpha(itemH):
       """アルファチャンネルをストレートに変更"""
       ae.sdk.AEGP_SetFootageInterpretation(itemH, False, {
           "alpha_flags": 1  # Straight alpha
       })
       print("アルファチャンネルをストレートに設定しました")

   # 使用例
   set_footage_framerate(itemH, 24.0)
   set_straight_alpha(itemH)

プレースホルダーの作成
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_placeholder(name, width, height, duration_sec):
       """プレースホルダーフッテージを作成"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)

       # プレースホルダーを作成
       footageH = ae.sdk.AEGP_NewPlaceholderFootage(
           plugin_id,
           name,
           width,
           height,
           duration_sec
       )

       # プロジェクトに追加
       itemH = ae.sdk.AEGP_AddFootageToProject(footageH, rootFolderH)
       print(f"プレースホルダーを作成: {name} ({width}x{height}, {duration_sec}秒)")

       return itemH

   # 使用例
   placeholder = create_placeholder("Temp Footage", 1920, 1080, 10.0)

ソリッドの色とサイズの変更
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def change_solid_properties(itemH, new_color, new_width, new_height):
       """ソリッドの色とサイズを変更"""
       # 署名を確認
       footageH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)
       sig = ae.sdk.AEGP_GetFootageSignature(footageH)

       if sig != ae.sdk.AEGP_FootageSignature_SOLID:
           print("エラー: このアイテムはソリッドではありません")
           return

       # 色を変更
       ae.sdk.AEGP_SetSolidFootageColor(itemH, False, new_color)

       # サイズを変更
       ae.sdk.AEGP_SetSolidFootageDimensions(itemH, False, new_width, new_height)

       print(f"ソリッドを変更: 色={new_color}, サイズ={new_width}x{new_height}")

   # 使用例
   change_solid_properties(itemH, (1.0, 0.5, 0.0), 3840, 2160)  # オレンジ色、4Kサイズ

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **フッテージの所有権管理**

   - プロジェクトに追加されたフッテージは、プロジェクトが所有権を持ちます
   - プロジェクトに追加後は ``AEGP_DisposeFootage()`` を呼び出してはいけません
   - 未追加のフッテージは必ず ``AEGP_DisposeFootage()`` で破棄してください

2. **プロキシフッテージの扱い**

   - ``AEGP_GetProxyFootageFromItem()`` はプロキシがない場合 ``0`` を返します（エラーではありません）
   - プロキシを設定する前に、必ずメインフッテージが存在することを確認してください

3. **ソリッド専用関数**

   - ``AEGP_GetSolidFootageColor()``、``AEGP_SetSolidFootageColor()``、``AEGP_SetSolidFootageDimensions()`` は
     ソリッドフッテージにのみ使用できます
   - 他の種類のフッテージで呼び出すとエラーになります

4. **フッテージ解釈設定の変更**

   - ``AEGP_SetFootageInterpretation()`` は内部で現在の設定を取得してから更新します
   - 変更したいキーのみを含む辞書を渡してください

ベストプラクティス
~~~~~~~~~~~~~~~~~~

フッテージの所有権管理
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 良い例: プロジェクトに追加
   footageH = ae.sdk.AEGP_NewFootage(plugin_id, path, interp_style)
   itemH = ae.sdk.AEGP_AddFootageToProject(footageH, rootFolderH)
   # プロジェクトが所有権を持つため、DisposeFootageは不要

   # 良い例: 未追加のフッテージを破棄
   footageH = ae.sdk.AEGP_NewSolidFootage("Temp", 100, 100, (1, 0, 0))
   ae.sdk.AEGP_DisposeFootage(footageH)

   # 悪い例: プロジェクトに追加後に破棄（クラッシュ）
   footageH = ae.sdk.AEGP_NewFootage(plugin_id, path, interp_style)
   itemH = ae.sdk.AEGP_AddFootageToProject(footageH, rootFolderH)
   ae.sdk.AEGP_DisposeFootage(footageH)  # NG!

プロキシの確認
^^^^^^^^^^^^^^

.. code-block:: python

   # プロキシの有無を確認
   proxyH = ae.sdk.AEGP_GetProxyFootageFromItem(itemH)
   if proxyH:
       print("プロキシが設定されています")
   else:
       print("プロキシはありません")

ソリッドの確認
^^^^^^^^^^^^^^

.. code-block:: python

   # ソリッドかどうかを確認してから操作
   footageH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)
   sig = ae.sdk.AEGP_GetFootageSignature(footageH)

   if sig == ae.sdk.AEGP_FootageSignature_SOLID:
       color = ae.sdk.AEGP_GetSolidFootageColor(itemH)
       print(f"ソリッドカラー: {color}")
   else:
       print("このアイテムはソリッドではありません")

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       footageH = ae.sdk.AEGP_NewFootage(
           plugin_id,
           path,
           ae.sdk.AEGP_InterpretationStyle_NO_DIALOG_GUESS
       )
       itemH = ae.sdk.AEGP_AddFootageToProject(footageH, rootFolderH)
   except Exception as e:
       print(f"フッテージの追加に失敗: {e}")
       # フッテージがプロジェクトに追加されていない場合のみ破棄
       if footageH:
           ae.sdk.AEGP_DisposeFootage(footageH)

関連項目
--------

- :doc:`AEGP_ItemSuite9` - プロジェクトアイテム管理
- :doc:`AEGP_ProjSuite6` - プロジェクト管理
- :doc:`AEGP_LayerSuite9` - レイヤー管理
- :doc:`index` - 低レベルAPI概要
