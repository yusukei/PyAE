AEGP_IOInSuite7
===============

.. currentmodule:: ae.sdk

AEGP_IOInSuite7は、After Effectsの入力フッテージ仕様（InSpec）のメタデータを管理するためのSDK APIです。

概要
----

**実装状況**: 39/39関数実装 ✅

AEGP_IOInSuite7は以下の機能を提供します:

- フッテージファイルのパス、サイズ、解像度の取得・設定
- フレームレート（FPS）とデュレーションの管理
- ビット深度とピクセルアスペクト比の設定
- インターレース・アルファチャンネル情報の管理
- オーディオ設定（サンプルレート、エンコーディング、チャンネル数）
- カラープロファイルとカラースペースの設定
- タイムコード情報（開始時刻、ドロップフレーム）の管理

基本概念
--------

InSpec (AEIO_InSpecH)
~~~~~~~~~~~~~~~~~~~~~

InSpecは、After Effectsに読み込まれたフッテージファイルのメタデータを表すハンドルです。PyAEでは整数値（``int``）として扱われます。

.. important::
   - InSpecハンドルは、フッテージアイテムが存在する間のみ有効です
   - フッテージを削除した後は、ハンドルは無効になります
   - InSpecは読み取り専用のフッテージ情報を格納します

InSpecで管理される情報
~~~~~~~~~~~~~~~~~~~~~

**ビデオ情報**:

- 解像度（幅×高さ）
- フレームレート（FPS）
- デュレーション（継続時間）
- ビット深度（8/16/32ビット）
- ピクセルアスペクト比（HSF: Horizontal Scale Factor）
- インターレース方式
- アルファチャンネル情報

**オーディオ情報**:

- サンプルレート（Hz）
- エンコーディング形式（PCM/Float）
- サンプルサイズ（1/2/4バイト）
- チャンネル数（モノラル/ステレオ）

**カラー情報**:

- 埋め込みカラープロファイル
- 割り当てられたカラープロファイル
- CICP（ITU-T H.273）カラースペース

**タイムコード情報**:

- ネイティブ開始時刻
- ドロップフレーム表示設定

API リファレンス
----------------

オプションハンドル
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetInSpecOptionsHandle(inH: int) -> int

   InSpecのオプションハンドルを取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: オプションハンドル（void*をintにキャスト）
   :rtype: int

   **例**:

   .. code-block:: python

      optionsH = ae.sdk.AEGP_GetInSpecOptionsHandle(inH)

.. function:: AEGP_SetInSpecOptionsHandle(inH: int, optionsPV: int) -> int

   InSpecのオプションハンドルを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param optionsPV: 新しいオプションハンドル
   :type optionsPV: int
   :return: 古いオプションハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      old_optionsH = ae.sdk.AEGP_SetInSpecOptionsHandle(inH, new_optionsH)

ファイルパスとサイズ
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetInSpecFilePath(inH: int) -> int

   InSpecのファイルパスを取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: ファイルパスを含むAEGP_MemHandle（``AEGP_FreeMemHandle`` で解放が必要）
   :rtype: int

   .. warning::
      戻り値のメモリハンドルは ``AEGP_FreeMemHandle()`` で必ず解放してください。

   **例**:

   .. code-block:: python

      memH = ae.sdk.AEGP_GetInSpecFilePath(inH)
      # メモリハンドルからパス文字列を取得
      # 使用後は AEGP_FreeMemHandle(memH) で解放

.. function:: AEGP_GetInSpecSize(inH: int) -> int

   InSpecのファイルサイズを取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: ファイルサイズ（バイト）
   :rtype: int

   **例**:

   .. code-block:: python

      size = ae.sdk.AEGP_GetInSpecSize(inH)
      print(f"ファイルサイズ: {size / (1024*1024):.2f} MB")

.. function:: AEGP_SetInSpecSize(inH: int, size: int) -> None

   InSpecのファイルサイズを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param size: ファイルサイズ（バイト）
   :type size: int

フレームレートとデュレーション
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetInSpecNativeFPS(inH: int) -> float

   InSpecのネイティブフレームレートを取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: フレームレート（FPS）
   :rtype: float

   .. note::
      内部的にA_Fixed（16.16固定小数点）からdoubleに変換されます。

   **例**:

   .. code-block:: python

      fps = ae.sdk.AEGP_GetInSpecNativeFPS(inH)
      print(f"フレームレート: {fps} FPS")

.. function:: AEGP_SetInSpecNativeFPS(inH: int, fps: float) -> None

   InSpecのネイティブフレームレートを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param fps: フレームレート（FPS）
   :type fps: float

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecNativeFPS(inH, 29.97)

.. function:: AEGP_GetInSpecDuration(inH: int) -> tuple[int, int]

   InSpecのデュレーション（継続時間）を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: (value, scale) のタプル。実際の秒数は value/scale。
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      value, scale = ae.sdk.AEGP_GetInSpecDuration(inH)
      duration_sec = value / scale
      print(f"デュレーション: {duration_sec} 秒")

.. function:: AEGP_SetInSpecDuration(inH: int, value: int, scale: int) -> None

   InSpecのデュレーション（継続時間）を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param value: デュレーションの値
   :type value: int
   :param scale: デュレーションのスケール
   :type scale: int

   **例**:

   .. code-block:: python

      # 10秒のデュレーションを設定
      ae.sdk.AEGP_SetInSpecDuration(inH, 10, 1)

解像度とアスペクト比
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetInSpecDimensions(inH: int) -> tuple[int, int]

   InSpecの解像度（幅×高さ）を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: (width, height) のタプル
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      width, height = ae.sdk.AEGP_GetInSpecDimensions(inH)
      print(f"解像度: {width}×{height}")

.. function:: AEGP_SetInSpecDimensions(inH: int, width: int, height: int) -> None

   InSpecの解像度（幅×高さ）を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecDimensions(inH, 1920, 1080)

.. function:: AEGP_InSpecGetRationalDimensions(inH: int, rs_x_num: int, rs_x_den: int, rs_y_num: int, rs_y_den: int) -> dict

   有理数スケールを適用した解像度を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :param rs_x_num: X方向スケールの分子
   :type rs_x_num: int
   :param rs_x_den: X方向スケールの分母
   :type rs_x_den: int
   :param rs_y_num: Y方向スケールの分子
   :type rs_y_num: int
   :param rs_y_den: Y方向スケールの分母
   :type rs_y_den: int
   :return: ``{"width": int, "height": int, "rect": (left, top, right, bottom)}``
   :rtype: dict

   **例**:

   .. code-block:: python

      # 1/2スケールで取得
      result = ae.sdk.AEGP_InSpecGetRationalDimensions(inH, 1, 2, 1, 2)
      print(f"スケール適用後: {result['width']}×{result['height']}")

.. function:: AEGP_GetInSpecHSF(inH: int) -> tuple[int, int]

   InSpecのピクセルアスペクト比（HSF: Horizontal Scale Factor）を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: (num, den) のタプル。実際のアスペクト比は num/den。
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      num, den = ae.sdk.AEGP_GetInSpecHSF(inH)
      par = num / den
      print(f"ピクセルアスペクト比: {par:.3f}")

.. function:: AEGP_SetInSpecHSF(inH: int, num: int, den: int) -> None

   InSpecのピクセルアスペクト比（HSF）を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param num: アスペクト比の分子
   :type num: int
   :param den: アスペクト比の分母
   :type den: int

   **例**:

   .. code-block:: python

      # アナモルフィック 1.33 (4:3)
      ae.sdk.AEGP_SetInSpecHSF(inH, 4, 3)

ビット深度
~~~~~~~~~~

.. function:: AEGP_GetInSpecDepth(inH: int) -> int

   InSpecのビット深度を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: ビット深度（8, 16, 32）
   :rtype: int

   **例**:

   .. code-block:: python

      depth = ae.sdk.AEGP_GetInSpecDepth(inH)
      print(f"ビット深度: {depth}ビット")

.. function:: AEGP_SetInSpecDepth(inH: int, depth: int) -> None

   InSpecのビット深度を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param depth: ビット深度（8, 16, 32）
   :type depth: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecDepth(inH, 16)

インターレース情報
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetInSpecInterlaceLabel(inH: int) -> dict

   InSpecのインターレース情報を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: インターレース情報の辞書
   :rtype: dict

   **戻り値の構造** (``FIEL_Label``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``signature``
        - int
        - フィールド識別子（通常は ``FIEL_Tag``）
      * - ``version``
        - int
        - バージョン番号
      * - ``type``
        - int
        - インターレースタイプ（``FIEL_Type_*`` 定数）
      * - ``order``
        - int
        - フィールド順序（``FIEL_Order_*`` 定数）
      * - ``reserved``
        - int
        - 予約領域

   **インターレースタイプ (type)**:

   - ``FIEL_Type_FRAME_RENDERED``: フレーム描画
   - ``FIEL_Type_INTERLACED``: インターレース
   - ``FIEL_Type_HALF_HEIGHT``: ハーフハイト
   - ``FIEL_Type_FIELD_DOUBLED``: フィールド2倍
   - ``FIEL_Type_UNSPECIFIED``: 未指定

   **フィールド順序 (order)**:

   - ``FIEL_Order_UPPER_FIRST``: 上位フィールド優先
   - ``FIEL_Order_LOWER_FIRST``: 下位フィールド優先

   **例**:

   .. code-block:: python

      interlace = ae.sdk.AEGP_GetInSpecInterlaceLabel(inH)
      print(f"インターレースタイプ: {interlace['type']}")
      print(f"フィールド順序: {interlace['order']}")

.. function:: AEGP_SetInSpecInterlaceLabel(inH: int, signature: int, version: int, type: int, order: int, reserved: int) -> None

   InSpecのインターレース情報を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param signature: フィールド識別子（``FIEL_Tag``）
   :type signature: int
   :param version: バージョン番号
   :type version: int
   :param type: インターレースタイプ
   :type type: int
   :param order: フィールド順序
   :type order: int
   :param reserved: 予約領域（0）
   :type reserved: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecInterlaceLabel(
          inH,
          ae.sdk.FIEL_Tag,
          1,  # version
          ae.sdk.FIEL_Type_INTERLACED,
          ae.sdk.FIEL_Order_UPPER_FIRST,
          0   # reserved
      )

アルファチャンネル情報
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetInSpecAlphaLabel(inH: int) -> dict

   InSpecのアルファチャンネル情報を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: アルファチャンネル情報の辞書
   :rtype: dict

   **戻り値の構造** (``AEIO_AlphaLabel``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``version``
        - int
        - バージョン番号（通常は ``AEIO_AlphaLabel_VERSION``）
      * - ``flags``
        - int
        - アルファフラグ（``AEIO_Alpha*`` 定数）
      * - ``red``
        - int
        - マットカラーの赤成分（0-255）
      * - ``green``
        - int
        - マットカラーの緑成分（0-255）
      * - ``blue``
        - int
        - マットカラーの青成分（0-255）
      * - ``alpha``
        - int
        - アルファタイプ（``AEIO_Alpha_*`` 定数）

   **アルファタイプ (alpha)**:

   - ``AEIO_Alpha_STRAIGHT``: ストレートアルファ
   - ``AEIO_Alpha_PREMUL``: 乗算済みアルファ
   - ``AEIO_Alpha_IGNORE``: アルファを無視
   - ``AEIO_Alpha_NONE``: アルファなし

   **アルファフラグ (flags)**:

   - ``AEIO_AlphaPremul``: 乗算済み
   - ``AEIO_AlphaInverted``: 反転

   **例**:

   .. code-block:: python

      alpha = ae.sdk.AEGP_GetInSpecAlphaLabel(inH)
      print(f"アルファタイプ: {alpha['alpha']}")
      print(f"マットカラー: RGB({alpha['red']}, {alpha['green']}, {alpha['blue']})")

.. function:: AEGP_SetInSpecAlphaLabel(inH: int, version: int, flags: int, red: int, green: int, blue: int, alpha_type: int) -> None

   InSpecのアルファチャンネル情報を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param version: バージョン番号（``AEIO_AlphaLabel_VERSION``）
   :type version: int
   :param flags: アルファフラグ
   :type flags: int
   :param red: マットカラーの赤成分（0-255）
   :type red: int
   :param green: マットカラーの緑成分（0-255）
   :type green: int
   :param blue: マットカラーの青成分（0-255）
   :type blue: int
   :param alpha_type: アルファタイプ
   :type alpha_type: int

   **例**:

   .. code-block:: python

      # 白マットの乗算済みアルファ
      ae.sdk.AEGP_SetInSpecAlphaLabel(
          inH,
          ae.sdk.AEIO_AlphaLabel_VERSION,
          ae.sdk.AEIO_AlphaPremul,
          255,  # red
          255,  # green
          255,  # blue
          ae.sdk.AEIO_Alpha_PREMUL
      )

オーディオ設定
~~~~~~~~~~~~~~

.. function:: AEGP_GetInSpecSoundRate(inH: int) -> float

   InSpecのオーディオサンプルレートを取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: サンプルレート（Hz）
   :rtype: float

   **例**:

   .. code-block:: python

      rate = ae.sdk.AEGP_GetInSpecSoundRate(inH)
      print(f"サンプルレート: {rate} Hz")

.. function:: AEGP_SetInSpecSoundRate(inH: int, rate: float) -> None

   InSpecのオーディオサンプルレートを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param rate: サンプルレート（Hz）
   :type rate: float

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecSoundRate(inH, 48000.0)

.. function:: AEGP_GetInSpecSoundEncoding(inH: int) -> int

   InSpecのオーディオエンコーディング形式を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: エンコーディング形式
   :rtype: int

   **エンコーディング形式**:

   - ``AEIO_E_UNSIGNED_PCM`` (1): 符号なしPCM
   - ``AEIO_E_SIGNED_PCM`` (2): 符号付きPCM
   - ``AEIO_E_SIGNED_FLOAT`` (3): 浮動小数点

   **例**:

   .. code-block:: python

      encoding = ae.sdk.AEGP_GetInSpecSoundEncoding(inH)
      if encoding == ae.sdk.AEIO_E_SIGNED_FLOAT:
          print("エンコーディング: Float")

.. function:: AEGP_SetInSpecSoundEncoding(inH: int, encoding: int) -> None

   InSpecのオーディオエンコーディング形式を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param encoding: エンコーディング形式
   :type encoding: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecSoundEncoding(inH, ae.sdk.AEIO_E_SIGNED_PCM)

.. function:: AEGP_GetInSpecSoundSampleSize(inH: int) -> int

   InSpecのオーディオサンプルサイズを取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: サンプルサイズ（バイト）
   :rtype: int

   **サンプルサイズ**:

   - ``AEIO_SS_1`` (1): 8ビット
   - ``AEIO_SS_2`` (2): 16ビット
   - ``AEIO_SS_4`` (4): 32ビット

   **例**:

   .. code-block:: python

      sample_size = ae.sdk.AEGP_GetInSpecSoundSampleSize(inH)
      print(f"サンプルサイズ: {sample_size * 8}ビット")

.. function:: AEGP_SetInSpecSoundSampleSize(inH: int, bytes_per_sample: int) -> None

   InSpecのオーディオサンプルサイズを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param bytes_per_sample: サンプルサイズ（1, 2, 4バイト）
   :type bytes_per_sample: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecSoundSampleSize(inH, ae.sdk.AEIO_SS_2)  # 16ビット

.. function:: AEGP_GetInSpecSoundChannels(inH: int) -> int

   InSpecのオーディオチャンネル数を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: チャンネル数
   :rtype: int

   **チャンネル数**:

   - ``AEIO_SndChannels_MONO`` (1): モノラル
   - ``AEIO_SndChannels_STEREO`` (2): ステレオ

   **例**:

   .. code-block:: python

      channels = ae.sdk.AEGP_GetInSpecSoundChannels(inH)
      print(f"チャンネル数: {channels}")

.. function:: AEGP_SetInSpecSoundChannels(inH: int, num_channels: int) -> None

   InSpecのオーディオチャンネル数を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param num_channels: チャンネル数（1=モノラル, 2=ステレオ）
   :type num_channels: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecSoundChannels(inH, ae.sdk.AEIO_SndChannels_STEREO)

カラープロファイル
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetInSpecEmbeddedColorProfile(inH: int, color_profileP: int, profile_desc: str) -> None

   InSpecに埋め込みカラープロファイルを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param color_profileP: カラープロファイルポインタ（0の場合は profile_desc を使用）
   :type color_profileP: int
   :param profile_desc: プロファイル説明文字列（UTF-16）
   :type profile_desc: str

   .. note::
      ``color_profileP`` と ``profile_desc`` のいずれか一方を使用します。両方指定しないでください。

   **例**:

   .. code-block:: python

      # プロファイル説明文字列で設定
      ae.sdk.AEGP_SetInSpecEmbeddedColorProfile(inH, 0, "sRGB IEC61966-2.1")

.. function:: AEGP_SetInSpecAssignedColorProfile(inH: int, color_profileP: int) -> None

   InSpecに有効なRGBプロファイルを割り当てます。

   :param inH: InSpecハンドル
   :type inH: int
   :param color_profileP: カラープロファイルポインタ
   :type color_profileP: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecAssignedColorProfile(inH, profile_ptr)

タイムコード情報
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetInSpecNativeStartTime(inH: int) -> tuple[int, int]

   InSpecのネイティブ開始時刻を取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: (value, scale) のタプル。実際の秒数は value/scale。
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      value, scale = ae.sdk.AEGP_GetInSpecNativeStartTime(inH)
      start_sec = value / scale
      print(f"開始時刻: {start_sec} 秒")

.. function:: AEGP_SetInSpecNativeStartTime(inH: int, value: int, scale: int) -> None

   InSpecのネイティブ開始時刻を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param value: 開始時刻の値
   :type value: int
   :param scale: 開始時刻のスケール
   :type scale: int

   **例**:

   .. code-block:: python

      # 1時間の開始時刻を設定（3600秒）
      ae.sdk.AEGP_SetInSpecNativeStartTime(inH, 3600, 1)

.. function:: AEGP_ClearInSpecNativeStartTime(inH: int) -> None

   InSpecのネイティブ開始時刻をクリアします。

   :param inH: InSpecハンドル
   :type inH: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_ClearInSpecNativeStartTime(inH)

.. function:: AEGP_GetInSpecNativeDisplayDropFrame(inH: int) -> bool

   InSpecでドロップフレームタイムコード表示を使用するかを取得します。

   :param inH: InSpecハンドル
   :type inH: int
   :return: ドロップフレーム表示の有効/無効
   :rtype: bool

   **例**:

   .. code-block:: python

      use_drop_frame = ae.sdk.AEGP_GetInSpecNativeDisplayDropFrame(inH)
      print(f"ドロップフレーム: {'有効' if use_drop_frame else '無効'}")

.. function:: AEGP_SetInSpecNativeDisplayDropFrame(inH: int, displayDropFrame: bool) -> None

   InSpecでドロップフレームタイムコード表示を設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param displayDropFrame: ドロップフレーム表示の有効/無効
   :type displayDropFrame: bool

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetInSpecNativeDisplayDropFrame(inH, True)

静止画シーケンス
~~~~~~~~~~~~~~~~

.. function:: AEGP_SetInSpecStillSequenceNativeFPS(inH: int, fps: float) -> None

   静止画シーケンスのネイティブFPSを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param fps: フレームレート（FPS）
   :type fps: float

   **例**:

   .. code-block:: python

      # 静止画シーケンスを24fpsで再生
      ae.sdk.AEGP_SetInSpecStillSequenceNativeFPS(inH, 24.0)

拡張マッピング
~~~~~~~~~~~~~~

.. function:: AEGP_AddAuxExtMap(extension: str, file_type: int, creator: int) -> None

   補助的なファイル拡張子マッピングを追加します。

   :param extension: ファイル拡張子（例: ".mov"）
   :type extension: str
   :param file_type: ファイルタイプコード
   :type file_type: int
   :param creator: クリエーターコード
   :type creator: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_AddAuxExtMap(".mp4", 0x4D503420, 0x4D504547)  # 'MP4 ', 'MPEG'

CICP カラースペース設定（AE 25.3+）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetInSpecColorSpaceFromCICP(inH: int, colorPrimariesCode: int, transferCharacteristicsCode: int, matrixCoefficientsCode: int, fullRangeVideoFlag: int, bitDepth: int, isRGB: bool) -> None

   CICP（ITU-T H.273）パラメータからカラースペースを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param colorPrimariesCode: 色域コード（1=BT.709, 9=BT.2020など）
   :type colorPrimariesCode: int
   :param transferCharacteristicsCode: 伝達特性コード（1=BT.709, 16=PQ, 18=HLGなど）
   :type transferCharacteristicsCode: int
   :param matrixCoefficientsCode: マトリックス係数コード（1=BT.709, 9=BT.2020 NCなど）
   :type matrixCoefficientsCode: int
   :param fullRangeVideoFlag: フルレンジビデオフラグ（0=リミテッド, 1=フル）
   :type fullRangeVideoFlag: int
   :param bitDepth: ビット深度（8, 10, 12など）
   :type bitDepth: int
   :param isRGB: RGBカラースペースかどうか
   :type isRGB: bool

   .. note::
      この関数はAEGP_IOInSuite7（After Effects 25.3以降）でのみ利用可能です。

   **例**:

   .. code-block:: python

      # BT.2020 PQ (HDR10)
      ae.sdk.AEGP_SetInSpecColorSpaceFromCICP(
          inH,
          9,   # BT.2020 color primaries
          16,  # PQ transfer characteristics
          9,   # BT.2020 non-constant luminance
          1,   # Full range
          10,  # 10-bit
          False
      )

.. function:: AEGP_SetInSpecColorSpaceFromCICP2(inH: int, colorPrimariesCode: int, transferCharacteristicsCode: int, matrixCoefficientsCode: int, fullRangeVideoFlag: int, bitDepth: int, colorModel: int, colorimetry: int) -> None

   拡張CICP2パラメータからカラースペースを設定します。

   :param inH: InSpecハンドル
   :type inH: int
   :param colorPrimariesCode: 色域コード
   :type colorPrimariesCode: int
   :param transferCharacteristicsCode: 伝達特性コード
   :type transferCharacteristicsCode: int
   :param matrixCoefficientsCode: マトリックス係数コード
   :type matrixCoefficientsCode: int
   :param fullRangeVideoFlag: フルレンジビデオフラグ
   :type fullRangeVideoFlag: int
   :param bitDepth: ビット深度
   :type bitDepth: int
   :param colorModel: カラーモデル
   :type colorModel: int
   :param colorimetry: 測色法
   :type colorimetry: int

   .. note::
      この関数はAEGP_IOInSuite7（After Effects 25.3以降）でのみ利用可能です。

定数一覧
--------

インターレース定数
~~~~~~~~~~~~~~~~~~

.. data:: FIEL_Type_FRAME_RENDERED
          FIEL_Type_INTERLACED
          FIEL_Type_HALF_HEIGHT
          FIEL_Type_FIELD_DOUBLED
          FIEL_Type_UNSPECIFIED

   インターレースタイプ

.. data:: FIEL_Order_UPPER_FIRST
          FIEL_Order_LOWER_FIRST

   フィールド順序

.. data:: FIEL_Tag

   フィールドタグ識別子

アルファチャンネル定数
~~~~~~~~~~~~~~~~~~~~~~

.. data:: AEIO_Alpha_STRAIGHT
          AEIO_Alpha_PREMUL
          AEIO_Alpha_IGNORE
          AEIO_Alpha_NONE

   アルファタイプ

.. data:: AEIO_AlphaPremul
          AEIO_AlphaInverted

   アルファフラグ

.. data:: AEIO_AlphaLabel_VERSION

   アルファラベルバージョン

オーディオ定数
~~~~~~~~~~~~~~

.. data:: AEIO_E_UNSIGNED_PCM
          AEIO_E_SIGNED_PCM
          AEIO_E_SIGNED_FLOAT

   オーディオエンコーディング形式

.. data:: AEIO_SS_1
          AEIO_SS_2
          AEIO_SS_4

   オーディオサンプルサイズ（バイト）

.. data:: AEIO_SndChannels_MONO
          AEIO_SndChannels_STEREO

   オーディオチャンネル数

使用例
------

フッテージ情報の取得
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_footage_info(itemH):
       """フッテージの詳細情報を取得"""
       # FootageからInSpecを取得（FootageSuite経由）
       inH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)
       inspecH = ae.sdk.AEGP_GetInSpecFromFootage(inH)

       # ビデオ情報
       width, height = ae.sdk.AEGP_GetInSpecDimensions(inspecH)
       fps = ae.sdk.AEGP_GetInSpecNativeFPS(inspecH)
       value, scale = ae.sdk.AEGP_GetInSpecDuration(inspecH)
       duration = value / scale
       depth = ae.sdk.AEGP_GetInSpecDepth(inspecH)

       # ピクセルアスペクト比
       num, den = ae.sdk.AEGP_GetInSpecHSF(inspecH)
       par = num / den

       # アルファ情報
       alpha = ae.sdk.AEGP_GetInSpecAlphaLabel(inspecH)

       # オーディオ情報
       sample_rate = ae.sdk.AEGP_GetInSpecSoundRate(inspecH)
       channels = ae.sdk.AEGP_GetInSpecSoundChannels(inspecH)

       print("=" * 60)
       print(f"解像度: {width}×{height}")
       print(f"フレームレート: {fps} FPS")
       print(f"デュレーション: {duration:.2f} 秒")
       print(f"ビット深度: {depth}ビット")
       print(f"ピクセルアスペクト比: {par:.3f}")
       print(f"アルファタイプ: {alpha['alpha']}")
       print(f"サンプルレート: {sample_rate} Hz")
       print(f"チャンネル数: {channels}")
       print("=" * 60)

   # 使用例
   itemH = ae.sdk.AEGP_GetActiveItem()
   get_footage_info(itemH)

インターレース設定の変更
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_interlaced_footage(inspecH, upper_first=True):
       """フッテージをインターレース形式に設定"""
       order = (ae.sdk.FIEL_Order_UPPER_FIRST if upper_first
                else ae.sdk.FIEL_Order_LOWER_FIRST)

       ae.sdk.AEGP_SetInSpecInterlaceLabel(
           inspecH,
           ae.sdk.FIEL_Tag,
           1,  # version
           ae.sdk.FIEL_Type_INTERLACED,
           order,
           0   # reserved
       )

       print(f"インターレース設定完了: {'上位' if upper_first else '下位'}フィールド優先")

   # 使用例
   itemH = ae.sdk.AEGP_GetActiveItem()
   inH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)
   inspecH = ae.sdk.AEGP_GetInSpecFromFootage(inH)
   set_interlaced_footage(inspecH, upper_first=True)

オーディオ設定の変更
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_audio_for_broadcast():
       """放送用オーディオ設定を適用"""
       itemH = ae.sdk.AEGP_GetActiveItem()
       inH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)
       inspecH = ae.sdk.AEGP_GetInSpecFromFootage(inH)

       # 48kHz, 16ビット, ステレオ, 符号付きPCM
       ae.sdk.AEGP_SetInSpecSoundRate(inspecH, 48000.0)
       ae.sdk.AEGP_SetInSpecSoundSampleSize(inspecH, ae.sdk.AEIO_SS_2)
       ae.sdk.AEGP_SetInSpecSoundChannels(inspecH, ae.sdk.AEIO_SndChannels_STEREO)
       ae.sdk.AEGP_SetInSpecSoundEncoding(inspecH, ae.sdk.AEIO_E_SIGNED_PCM)

       print("放送用オーディオ設定を適用しました")
       print("  サンプルレート: 48 kHz")
       print("  サンプルサイズ: 16ビット")
       print("  チャンネル: ステレオ")
       print("  エンコーディング: 符号付きPCM")

   # 実行
   setup_audio_for_broadcast()

HDRカラースペース設定（AE 25.3+）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_hdr10_colorspace(inspecH):
       """HDR10カラースペースを設定"""
       try:
           ae.sdk.AEGP_SetInSpecColorSpaceFromCICP(
               inspecH,
               9,   # BT.2020 color primaries
               16,  # PQ (SMPTE ST 2084) transfer
               9,   # BT.2020 non-constant luminance matrix
               1,   # Full range
               10,  # 10-bit
               False  # YUV
           )
           print("HDR10カラースペース設定完了")
       except Exception as e:
           print(f"カラースペース設定エラー: {e}")
           print("注意: CICP設定はAE 25.3以降でのみ利用可能です")

   # 使用例
   itemH = ae.sdk.AEGP_GetActiveItem()
   inH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)
   inspecH = ae.sdk.AEGP_GetInSpecFromFootage(inH)
   set_hdr10_colorspace(inspecH)

フッテージメタデータの一括設定
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def apply_footage_metadata(inspecH, metadata):
       """フッテージメタデータを一括設定"""
       # ビデオ設定
       if 'width' in metadata and 'height' in metadata:
           ae.sdk.AEGP_SetInSpecDimensions(
               inspecH, metadata['width'], metadata['height'])

       if 'fps' in metadata:
           ae.sdk.AEGP_SetInSpecNativeFPS(inspecH, metadata['fps'])

       if 'duration' in metadata:
           ae.sdk.AEGP_SetInSpecDuration(
               inspecH, metadata['duration'], 1)

       if 'depth' in metadata:
           ae.sdk.AEGP_SetInSpecDepth(inspecH, metadata['depth'])

       if 'par' in metadata:
           # Pixel Aspect Ratio (例: 1.0, 1.33など)
           par = metadata['par']
           ae.sdk.AEGP_SetInSpecHSF(inspecH, int(par * 1000), 1000)

       # アルファ設定
       if 'alpha_type' in metadata:
           ae.sdk.AEGP_SetInSpecAlphaLabel(
               inspecH,
               ae.sdk.AEIO_AlphaLabel_VERSION,
               0,  # flags
               255, 255, 255,  # white matte
               metadata['alpha_type']
           )

       # オーディオ設定
       if 'sample_rate' in metadata:
           ae.sdk.AEGP_SetInSpecSoundRate(inspecH, metadata['sample_rate'])

       if 'audio_channels' in metadata:
           ae.sdk.AEGP_SetInSpecSoundChannels(inspecH, metadata['audio_channels'])

       print("メタデータ設定完了")

   # 使用例
   metadata = {
       'width': 1920,
       'height': 1080,
       'fps': 29.97,
       'duration': 300,  # 5分
       'depth': 16,
       'par': 1.0,
       'alpha_type': ae.sdk.AEIO_Alpha_PREMUL,
       'sample_rate': 48000.0,
       'audio_channels': ae.sdk.AEIO_SndChannels_STEREO
   }

   itemH = ae.sdk.AEGP_GetActiveItem()
   inH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)
   inspecH = ae.sdk.AEGP_GetInSpecFromFootage(inH)
   apply_footage_metadata(inspecH, metadata)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **InSpecハンドルの取得**

   InSpecハンドルは ``AEGP_FootageSuite`` の ``AEGP_GetInSpecFromFootage()`` で取得します。

2. **メモリ管理**

   ``AEGP_GetInSpecFilePath()`` が返すメモリハンドルは、必ず ``AEGP_FreeMemHandle()`` で解放してください。

3. **CICP関数の可用性**

   ``AEGP_SetInSpecColorSpaceFromCICP`` と ``AEGP_SetInSpecColorSpaceFromCICP2`` は、After Effects 25.3以降でのみ利用可能です。

4. **アルファタイプとフラグの整合性**

   アルファラベルのフラグとタイプは整合性を保つ必要があります（例: ``AEIO_Alpha_PREMUL`` と ``AEIO_AlphaPremul`` を同時に使用）。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

InSpecハンドルの取得
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # FootageからInSpecを取得
   inH = ae.sdk.AEGP_GetMainFootageFromItem(itemH)
   inspecH = ae.sdk.AEGP_GetInSpecFromFootage(inH)

メモリハンドルの解放
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   memH = ae.sdk.AEGP_GetInSpecFilePath(inspecH)
   try:
       # メモリハンドルから文字列を取得
       path = ae.sdk.AEGP_LockMemHandle(memH)
   finally:
       ae.sdk.AEGP_FreeMemHandle(memH)

バージョン互換性の確認
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       ae.sdk.AEGP_SetInSpecColorSpaceFromCICP(inspecH, 9, 16, 9, 1, 10, False)
   except AttributeError:
       print("警告: CICP関数はAE 25.3以降でのみ利用可能です")

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       fps = ae.sdk.AEGP_GetInSpecNativeFPS(inspecH)
   except Exception as e:
       print(f"FPS取得エラー: {e}")

関連項目
--------

- :doc:`AEGP_FootageSuite5` - フッテージ管理、InSpec取得
- :doc:`AEGP_IOOutSuite6` - 出力仕様（OutSpec）管理
- :doc:`AEGP_ItemSuite9` - プロジェクトアイテム管理
- :doc:`AEGP_ColorSettingsSuite6` - カラープロファイル管理
- :doc:`index` - 低レベルAPI概要
