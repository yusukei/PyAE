AEGP_IOOutSuite6
================

.. currentmodule:: ae.sdk

AEGP_IOOutSuite6は、After Effectsの出力フッテージ仕様（Output Specification）を管理するためのSDK APIです。

概要
----

**実装状況**: 35/36関数実装 ✅

AEGP_IOOutSuite6は以下の機能を提供します:

- 出力仕様（OutSpec）のメタデータ取得・設定
- ビデオ設定（解像度、FPS、ビット深度、インターレース）
- オーディオ設定（サンプルレート、エンコーディング、チャンネル数）
- カラープロファイル、アルファチャンネル設定
- レンダリング範囲（開始時間、継続時間）
- CICP（Coding-Independent Code Points）カラースペース対応（AE 25.2+）

.. note::
   ``AEGP_GetOutSpecFrameTime`` は、pybind11バインディング時にメモリ割り当てエラー（4160 GB）が発生するため、現在除外されています。根本原因が特定されるまで使用できません。

基本概念
--------

OutSpecハンドル (AEIO_OutSpecH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsの出力仕様（レンダリング出力設定）を識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - OutSpecハンドルは、レンダーキューアイテムの出力モジュールに関連付けられています
   - ハンドルは、レンダーキューアイテムが存在する間のみ有効です
   - 複数の出力モジュールがある場合、それぞれに固有のOutSpecハンドルがあります

出力仕様の構造
~~~~~~~~~~~~~~

出力仕様には以下の設定が含まれます:

**ビデオ設定**:
- 解像度（幅×高さ）
- フレームレート（FPS）
- ビット深度（8/16/32ビット）
- インターレース設定
- アルファチャンネル設定
- カラープロファイル

**オーディオ設定**:
- サンプルレート（Hz）
- エンコーディング形式（PCM、浮動小数点）
- サンプルサイズ（1/2/4バイト）
- チャンネル数（モノラル/ステレオ）

**レンダリング範囲**:
- 開始時間
- 継続時間
- 開始フレーム番号
- ポスタータイム

API リファレンス
----------------

オプションハンドル
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecOptionsHandle(outH: int) -> int

   出力仕様のオプションハンドルを取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: オプションハンドル（不透明ポインタ）
   :rtype: int

   **例**:

   .. code-block:: python

      optionsPV = ae.sdk.AEGP_GetOutSpecOptionsHandle(outH)

.. function:: AEGP_SetOutSpecOptionsHandle(outH: int, optionsPV: int) -> int

   出力仕様のオプションハンドルを設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param optionsPV: 新しいオプションハンドル
   :type optionsPV: int
   :return: 古いオプションハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      old_optionsPV = ae.sdk.AEGP_SetOutSpecOptionsHandle(outH, new_optionsPV)

ファイルパス
~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecFilePath(outH: int) -> tuple[str, bool]

   出力ファイルのパスを取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: (ファイルパス, ファイルが予約されているか)
   :rtype: tuple[str, bool]

   .. note::
      内部的にUTF-16からUTF-8に自動変換されます。日本語のパス名も正しく取得できます。

   **例**:

   .. code-block:: python

      path, reserved = ae.sdk.AEGP_GetOutSpecFilePath(outH)
      print(f"出力パス: {path}")
      if reserved:
          print("ファイルは予約されています")

フレームレート（FPS）
~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecFPS(outH: int) -> float

   出力のネイティブFPSを取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: フレームレート（FPS）
   :rtype: float

   .. note::
      内部的に16.16固定小数点（A_Fixed）から浮動小数点に変換されます。

   **例**:

   .. code-block:: python

      fps = ae.sdk.AEGP_GetOutSpecFPS(outH)
      print(f"FPS: {fps}")

.. function:: AEGP_SetOutSpecNativeFPS(outH: int, fps: float) -> None

   出力のネイティブFPSを設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param fps: フレームレート（FPS）
   :type fps: float

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetOutSpecNativeFPS(outH, 29.97)

ビット深度
~~~~~~~~~~

.. function:: AEGP_GetOutSpecDepth(outH: int) -> int

   出力のビット深度を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: ビット深度（8, 16, 32）
   :rtype: int

   **例**:

   .. code-block:: python

      depth = ae.sdk.AEGP_GetOutSpecDepth(outH)
      print(f"ビット深度: {depth}ビット")

.. function:: AEGP_SetOutSpecDepth(outH: int, depth: int) -> None

   出力のビット深度を設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param depth: ビット深度（8, 16, 32）
   :type depth: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetOutSpecDepth(outH, 16)

インターレース設定
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecInterlaceLabel(outH: int) -> dict

   出力のインターレース設定を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: インターレース設定の辞書
   :rtype: dict

   **戻り値の構造** (``FIEL_Label``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``signature``
        - int
        - フィールドラベルのシグネチャ
      * - ``version``
        - int
        - バージョン
      * - ``type``
        - int
        - インターレースタイプ
      * - ``order``
        - int
        - フィールド順序
      * - ``reserved``
        - int
        - 予約済み

   **例**:

   .. code-block:: python

      interlace = ae.sdk.AEGP_GetOutSpecInterlaceLabel(outH)
      print(f"インターレースタイプ: {interlace['type']}")

.. function:: AEGP_SetOutSpecInterlaceLabel(outH: int, interlace: dict) -> None

   出力のインターレース設定を変更します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param interlace: インターレース設定の辞書
   :type interlace: dict

   **例**:

   .. code-block:: python

      interlace = ae.sdk.AEGP_GetOutSpecInterlaceLabel(outH)
      interlace['type'] = FIEL_Type_FRAME_RENDERED  # フレームレンダリング
      ae.sdk.AEGP_SetOutSpecInterlaceLabel(outH, interlace)

アルファチャンネル
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecAlphaLabel(outH: int) -> dict

   出力のアルファチャンネル設定を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: アルファラベル設定の辞書
   :rtype: dict

   **戻り値の構造** (``AEIO_AlphaLabel``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``flags``
        - int
        - フラグ
      * - ``red``
        - int
        - 赤成分（0-255）
      * - ``green``
        - int
        - 緑成分（0-255）
      * - ``blue``
        - int
        - 青成分（0-255）
      * - ``alpha``
        - int
        - アルファタイプ（AEIO_AlphaType）

   **例**:

   .. code-block:: python

      alpha = ae.sdk.AEGP_GetOutSpecAlphaLabel(outH)
      print(f"アルファタイプ: {alpha['alpha']}")

.. function:: AEGP_SetOutSpecAlphaLabel(outH: int, alpha: dict) -> None

   出力のアルファチャンネル設定を変更します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param alpha: アルファラベル設定の辞書
   :type alpha: dict

   **例**:

   .. code-block:: python

      alpha = ae.sdk.AEGP_GetOutSpecAlphaLabel(outH)
      alpha['alpha'] = AEIO_Alpha_STRAIGHT  # ストレートアルファ
      ae.sdk.AEGP_SetOutSpecAlphaLabel(outH, alpha)

継続時間とタイミング
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecDuration(outH: int) -> tuple[int, int]

   出力の継続時間を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: (value, scale) - A_Time形式の継続時間
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      value, scale = ae.sdk.AEGP_GetOutSpecDuration(outH)
      duration_seconds = value / scale
      print(f"継続時間: {duration_seconds}秒")

.. function:: AEGP_SetOutSpecDuration(outH: int, value: int, scale: int) -> None

   出力の継続時間を設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param value: 継続時間のvalue（A_Time）
   :type value: int
   :param scale: 継続時間のscale（A_Time）
   :type scale: int

   **例**:

   .. code-block:: python

      # 10秒間のレンダリング
      ae.sdk.AEGP_SetOutSpecDuration(outH, 10, 1)

.. function:: AEGP_GetOutSpecStartTime(outH: int) -> tuple[int, int]

   出力の開始時間を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: (value, scale) - A_Time形式の開始時間
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      value, scale = ae.sdk.AEGP_GetOutSpecStartTime(outH)
      start_seconds = value / scale
      print(f"開始時間: {start_seconds}秒")

.. function:: AEGP_GetOutSpecPosterTime(outH: int) -> tuple[int, int]

   出力のポスタータイム（代表フレーム）を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: (value, scale) - A_Time形式のポスタータイム
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      value, scale = ae.sdk.AEGP_GetOutSpecPosterTime(outH)
      poster_seconds = value / scale
      print(f"ポスタータイム: {poster_seconds}秒")

解像度とスケール
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecDimensions(outH: int) -> tuple[int, int]

   出力の解像度（幅×高さ）を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: (width, height) - 解像度
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      width, height = ae.sdk.AEGP_GetOutSpecDimensions(outH)
      print(f"解像度: {width}x{height}")

.. function:: AEGP_GetOutSpecHSF(outH: int) -> tuple[int, int]

   出力の水平スケールファクター（HSF）を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: (num, den) - A_Ratio形式のスケールファクター
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      num, den = ae.sdk.AEGP_GetOutSpecHSF(outH)
      scale_factor = num / den
      print(f"水平スケール: {scale_factor}")

.. function:: AEGP_SetOutSpecHSF(outH: int, num: int, den: int) -> None

   出力の水平スケールファクター（HSF）を設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param num: スケールファクターの分子
   :type num: int
   :param den: スケールファクターの分母
   :type den: int

   **例**:

   .. code-block:: python

      # 50%スケール
      ae.sdk.AEGP_SetOutSpecHSF(outH, 1, 2)

オーディオ設定
~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecSoundRate(outH: int) -> float

   出力のオーディオサンプルレートを取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: サンプルレート（Hz）
   :rtype: float

   **例**:

   .. code-block:: python

      rate = ae.sdk.AEGP_GetOutSpecSoundRate(outH)
      print(f"サンプルレート: {rate} Hz")

.. function:: AEGP_SetOutSpecSoundRate(outH: int, rate: float) -> None

   出力のオーディオサンプルレートを設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param rate: サンプルレート（Hz）
   :type rate: float

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetOutSpecSoundRate(outH, 48000.0)  # 48kHz

.. function:: AEGP_GetOutSpecSoundEncoding(outH: int) -> int

   出力のオーディオエンコーディング形式を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: エンコーディング形式（1=UNSIGNED_PCM, 2=SIGNED_PCM, 3=SIGNED_FLOAT）
   :rtype: int

   **例**:

   .. code-block:: python

      encoding = ae.sdk.AEGP_GetOutSpecSoundEncoding(outH)
      if encoding == 1:
          print("符号なしPCM")
      elif encoding == 2:
          print("符号付きPCM")
      elif encoding == 3:
          print("符号付き浮動小数点")

.. function:: AEGP_SetOutSpecSoundEncoding(outH: int, encoding: int) -> None

   出力のオーディオエンコーディング形式を設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param encoding: エンコーディング形式（1=UNSIGNED_PCM, 2=SIGNED_PCM, 3=SIGNED_FLOAT）
   :type encoding: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetOutSpecSoundEncoding(outH, 2)  # 符号付きPCM

.. function:: AEGP_GetOutSpecSoundSampleSize(outH: int) -> int

   出力のオーディオサンプルサイズを取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: サンプルサイズ（1, 2, 4バイト）
   :rtype: int

   **例**:

   .. code-block:: python

      sample_size = ae.sdk.AEGP_GetOutSpecSoundSampleSize(outH)
      print(f"サンプルサイズ: {sample_size}バイト")

.. function:: AEGP_SetOutSpecSoundSampleSize(outH: int, bytes_per_sample: int) -> None

   出力のオーディオサンプルサイズを設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param bytes_per_sample: サンプルサイズ（1, 2, 4バイト）
   :type bytes_per_sample: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetOutSpecSoundSampleSize(outH, 2)  # 16ビット（2バイト）

.. function:: AEGP_GetOutSpecSoundChannels(outH: int) -> int

   出力のオーディオチャンネル数を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: チャンネル数（1=MONO, 2=STEREO）
   :rtype: int

   **例**:

   .. code-block:: python

      channels = ae.sdk.AEGP_GetOutSpecSoundChannels(outH)
      print(f"チャンネル数: {channels}")

.. function:: AEGP_SetOutSpecSoundChannels(outH: int, num_channels: int) -> None

   出力のオーディオチャンネル数を設定します。

   :param outH: OutSpecハンドル
   :type outH: int
   :param num_channels: チャンネル数（1=MONO, 2=STEREO）
   :type num_channels: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetOutSpecSoundChannels(outH, 2)  # ステレオ

その他の設定
~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecIsStill(outH: int) -> bool

   出力が静止画かどうかを確認します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: 静止画の場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_GetOutSpecIsStill(outH):
          print("静止画出力です")

.. function:: AEGP_GetOutSpecStartFrame(outH: int) -> int

   出力の開始フレーム番号を取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: 開始フレーム番号
   :rtype: int

   **例**:

   .. code-block:: python

      start_frame = ae.sdk.AEGP_GetOutSpecStartFrame(outH)
      print(f"開始フレーム: {start_frame}")

.. function:: AEGP_GetOutSpecPullDown(outH: int) -> int

   出力のプルダウンモードを取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: プルダウンモード
   :rtype: int

   **例**:

   .. code-block:: python

      pulldown = ae.sdk.AEGP_GetOutSpecPullDown(outH)

.. function:: AEGP_GetOutSpecIsMissing(outH: int) -> bool

   出力ファイルが欠落しているかを確認します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: 欠落している場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_GetOutSpecIsMissing(outH):
          print("出力ファイルが見つかりません")

.. function:: AEGP_GetOutSpecIsDropFrame(outH: int) -> bool

   出力がドロップフレームタイムコードを使用しているかを確認します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: ドロップフレームの場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_GetOutSpecIsDropFrame(outH):
          print("ドロップフレームタイムコード")

カラープロファイル
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecShouldEmbedICCProfile(outH: int) -> bool

   出力にICCプロファイルを埋め込むかを確認します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: 埋め込む場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_GetOutSpecShouldEmbedICCProfile(outH):
          print("ICCプロファイルを埋め込みます")

.. function:: AEGP_GetNewOutSpecColorProfile(outH: int) -> int

   出力のカラープロファイルを取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: カラープロファイルハンドル（使用後は ``AEGP_DisposeColorProfile`` で解放が必要）
   :rtype: int

   .. warning::
      戻り値のカラープロファイルハンドルは、使用後に ``AEGP_DisposeColorProfile`` で必ず解放してください。

   **例**:

   .. code-block:: python

      color_profile = ae.sdk.AEGP_GetNewOutSpecColorProfile(outH)
      try:
          # カラープロファイルを使用
          pass
      finally:
          ae.sdk.AEGP_DisposeColorProfile(color_profile)

.. function:: AEGP_GetOutSpecColorSpaceAsCICPIfCompatible(outH: int) -> tuple[int, int, int, bool, bool]

   出力のカラースペースをCICP（Coding-Independent Code Points）形式で取得します（AE 25.2以降のみ）。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: (color_primaries, transfer_characteristics, matrix_coefficients, is_full_range, is_success)
   :rtype: tuple[int, int, int, bool, bool]

   .. note::
      この関数はAE 25.2以降でのみ利用可能です。古いSDKでビルドされた場合は使用できません。

   **例**:

   .. code-block:: python

      cp, tc, mc, full_range, success = ae.sdk.AEGP_GetOutSpecColorSpaceAsCICPIfCompatible(outH)
      if success:
          print(f"CICP: CP={cp}, TC={tc}, MC={mc}, FullRange={full_range}")
      else:
          print("CICPとして表現できないカラースペース")

レンダーキュー連携
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutSpecOutputModule(outH: int) -> tuple[int, int]

   OutSpecに関連するレンダーキューアイテムと出力モジュールを取得します。

   :param outH: OutSpecハンドル
   :type outH: int
   :return: (rq_itemH, om_refH) - レンダーキューアイテムハンドルと出力モジュールハンドル
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      rq_itemH, om_refH = ae.sdk.AEGP_GetOutSpecOutputModule(outH)
      print(f"RQアイテム: {rq_itemH}, 出力モジュール: {om_refH}")

使用例
------

出力設定の確認
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_outspec_info(outH):
       """出力仕様の詳細情報を表示"""
       # ファイルパス
       path, reserved = ae.sdk.AEGP_GetOutSpecFilePath(outH)
       print(f"出力パス: {path}")

       # ビデオ設定
       width, height = ae.sdk.AEGP_GetOutSpecDimensions(outH)
       fps = ae.sdk.AEGP_GetOutSpecFPS(outH)
       depth = ae.sdk.AEGP_GetOutSpecDepth(outH)
       print(f"解像度: {width}x{height}, FPS: {fps}, ビット深度: {depth}")

       # タイミング
       start_val, start_scale = ae.sdk.AEGP_GetOutSpecStartTime(outH)
       dur_val, dur_scale = ae.sdk.AEGP_GetOutSpecDuration(outH)
       start_sec = start_val / start_scale
       dur_sec = dur_val / dur_scale
       print(f"開始: {start_sec}秒, 継続時間: {dur_sec}秒")

       # オーディオ設定
       sample_rate = ae.sdk.AEGP_GetOutSpecSoundRate(outH)
       encoding = ae.sdk.AEGP_GetOutSpecSoundEncoding(outH)
       channels = ae.sdk.AEGP_GetOutSpecSoundChannels(outH)
       print(f"オーディオ: {sample_rate}Hz, エンコーディング: {encoding}, チャンネル: {channels}")

       # フラグ
       is_still = ae.sdk.AEGP_GetOutSpecIsStill(outH)
       is_missing = ae.sdk.AEGP_GetOutSpecIsMissing(outH)
       print(f"静止画: {is_still}, ファイル欠落: {is_missing}")

高品質出力設定
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_high_quality_output(outH):
       """出力を高品質設定に変更"""
       # 32ビット浮動小数点
       ae.sdk.AEGP_SetOutSpecDepth(outH, 32)

       # フルHD解像度（既存の解像度を維持する場合は設定不要）
       # 解像度は直接設定できないため、HSFでスケール調整
       ae.sdk.AEGP_SetOutSpecHSF(outH, 1, 1)  # 100%スケール

       # 高品質オーディオ（48kHz、ステレオ、16ビット）
       ae.sdk.AEGP_SetOutSpecSoundRate(outH, 48000.0)
       ae.sdk.AEGP_SetOutSpecSoundEncoding(outH, 2)  # SIGNED_PCM
       ae.sdk.AEGP_SetOutSpecSoundSampleSize(outH, 2)  # 16ビット
       ae.sdk.AEGP_SetOutSpecSoundChannels(outH, 2)  # ステレオ

       print("高品質出力設定に変更しました")

アルファチャンネル設定の変更
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_straight_alpha(outH):
       """アルファチャンネルをストレートに設定"""
       alpha = ae.sdk.AEGP_GetOutSpecAlphaLabel(outH)
       alpha['alpha'] = 1  # AEIO_Alpha_STRAIGHT
       ae.sdk.AEGP_SetOutSpecAlphaLabel(outH, alpha)
       print("アルファをストレートに設定しました")

レンダリング範囲の調整
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_render_range(outH, start_seconds, duration_seconds):
       """レンダリング範囲を秒単位で設定"""
       # 開始時間は読み取り専用のため、継続時間のみ設定可能
       ae.sdk.AEGP_SetOutSpecDuration(outH, int(duration_seconds), 1)
       print(f"レンダリング継続時間を {duration_seconds}秒 に設定しました")

   # 使用例: 10秒間のレンダリング
   set_render_range(outH, 0, 10)

カラースペース情報の取得（AE 25.2+）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_color_space_info(outH):
       """カラースペース情報を取得（AE 25.2以降）"""
       try:
           cp, tc, mc, full_range, success = ae.sdk.AEGP_GetOutSpecColorSpaceAsCICPIfCompatible(outH)

           if success:
               print(f"CICP カラースペース:")
               print(f"  Color Primaries: {cp}")
               print(f"  Transfer Characteristics: {tc}")
               print(f"  Matrix Coefficients: {mc}")
               print(f"  Full Range: {full_range}")
           else:
               print("カラースペースはCICPと互換性がありません")
       except AttributeError:
           print("この機能はAE 25.2以降でのみ利用可能です")

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **OutSpecハンドルの有効期限**

   OutSpecハンドルは、レンダーキューアイテムが存在する間のみ有効です。

2. **AEGP_GetOutSpecFrameTimeは使用不可**

   現在、この関数はメモリ割り当てエラーが発生するため使用できません。

3. **カラープロファイルの解放**

   ``AEGP_GetNewOutSpecColorProfile`` で取得したハンドルは、必ず ``AEGP_DisposeColorProfile`` で解放してください。

4. **解像度の変更**

   解像度は直接設定できません。HSF（水平スケールファクター）で間接的にスケール調整が可能です。

5. **読み取り専用プロパティ**

   以下のプロパティは読み取り専用です（設定関数がありません）:

   - 出力ファイルパス
   - 解像度（幅×高さ）
   - 開始時間
   - ポスタータイム
   - 開始フレーム番号
   - プルダウンモード

ベストプラクティス
~~~~~~~~~~~~~~~~~~

設定の取得・変更パターン
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 現在の設定を取得
   current_setting = ae.sdk.AEGP_GetOutSpecXXX(outH)

   # 必要に応じて変更
   ae.sdk.AEGP_SetOutSpecXXX(outH, new_value)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       fps = ae.sdk.AEGP_GetOutSpecFPS(outH)
   except Exception as e:
       print(f"FPS取得エラー: {e}")

カラープロファイルの安全な使用
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   color_profile = None
   try:
       color_profile = ae.sdk.AEGP_GetNewOutSpecColorProfile(outH)
       # カラープロファイルを使用
   finally:
       if color_profile:
           ae.sdk.AEGP_DisposeColorProfile(color_profile)

辞書型パラメータの使用
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 既存の設定を取得してから変更
   interlace = ae.sdk.AEGP_GetOutSpecInterlaceLabel(outH)
   interlace['type'] = new_type
   ae.sdk.AEGP_SetOutSpecInterlaceLabel(outH, interlace)

関連項目
--------

- :doc:`AEGP_RenderQueueSuite1` - レンダーキュー管理
- :doc:`AEGP_OutputModuleSuite4` - 出力モジュール管理
- :doc:`AEGP_RQItemSuite4` - レンダーキューアイテム管理
- :doc:`index` - 低レベルAPI概要
