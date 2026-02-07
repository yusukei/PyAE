AEGP_OutputModuleSuite4
=======================

.. currentmodule:: ae.sdk

AEGP_OutputModuleSuite4は、After Effectsのレンダーキューにおける出力モジュールの管理と設定を行うためのSDK APIです。

概要
----

**実装状況**: 17/17関数実装済み ✅

AEGP_OutputModuleSuite4は以下の機能を提供します:

- 出力モジュールの取得と作成
- 出力ファイルパスの設定と取得
- 埋め込みオプション（リンクプロジェクト）の管理
- レンダー後のアクション設定
- 出力タイプ（ビデオ/オーディオ）の有効化制御
- ビデオチャンネル設定（RGB、アルファなど）
- ストレッチ、クロップ情報の管理
- オーディオフォーマット設定
- 出力モジュール詳細情報の取得

基本概念
--------

出力モジュールハンドル (AEGP_OutputModuleRefH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダーキューアイテムに関連付けられた出力モジュールを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - 出力モジュールハンドルは、必ず対応するレンダーキューアイテムハンドルとペアで使用する必要があります
   - ハンドルペアの組み合わせルール:

     - ``(rq_itemH, outmodH)`` - 同一RQアイテムからの実際のハンドルペア
     - ``(0, 0)`` - Queuebertパターン（最初のRQアイテム、最初の出力モジュール）
     - 混在パターン（例: ``0``と実際のハンドル）は **エラーの原因** となります

インデックス規則
~~~~~~~~~~~~~~~~

.. warning::
   ``AEGP_GetOutputModuleByIndex()`` は **0ベースのインデックス** を使用します。
   これは、AEGP SDKのほとんどの関数が1ベースインデックスを使用するのに対する例外です。

.. code-block:: python

   # 最初の出力モジュールを取得（index=0）
   outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)

レンダー後のアクション
~~~~~~~~~~~~~~~~~~~~~~

レンダー完了後に実行するアクションを設定できます:

- なし（何もしない）
- プロジェクトをインポート
- プロジェクトを設定してレンダリング

埋め込みオプション
~~~~~~~~~~~~~~~~~~

出力にプロジェクトリンクを埋め込むかどうかを設定できます:

- なし
- プロジェクトリンクを埋め込む
- プロジェクトリンクとプロキシを埋め込む

API リファレンス
----------------

出力モジュールの取得と作成
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutputModuleByIndex(rq_itemH: int, index: int) -> int

   レンダーキューアイテムから、インデックスで出力モジュールハンドルを取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param index: 出力モジュールのインデックス（0から始まる）
   :type index: int
   :return: 出力モジュールハンドル
   :rtype: int

   .. warning::
      この関数は **0ベースのインデックス** を使用します（AEGP SDKの例外）。

   **例**:

   .. code-block:: python

      # 最初の出力モジュールを取得
      outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)

.. function:: AEGP_AddDefaultOutputModule(rq_itemH: int) -> int

   レンダーキューアイテムにデフォルトの出力モジュールを追加します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :return: 新しい出力モジュールハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      outmodH = ae.sdk.AEGP_AddDefaultOutputModule(rq_itemH)

出力ファイルパス
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetOutputFilePath(rq_itemH: int, outmodH: int) -> str

   出力モジュールの出力ファイルパスを取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: 出力ファイルパス（UTF-16文字列）
   :rtype: str

   **例**:

   .. code-block:: python

      path = ae.sdk.AEGP_GetOutputFilePath(rq_itemH, outmodH)
      print(f"出力パス: {path}")

.. function:: AEGP_SetOutputFilePath(rq_itemH: int, outmodH: int, path: str) -> None

   出力モジュールの出力ファイルパスを設定します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :param path: 出力ファイルパス
   :type path: str

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetOutputFilePath(rq_itemH, outmodH, "D:/output/test.mov")

埋め込みオプション
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetEmbedOptions(rq_itemH: int, outmodH: int) -> int

   埋め込みオプション（プロジェクトリンク）を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: 埋め込みタイプ（AEGP_EmbeddingType）
   :rtype: int

   **埋め込みタイプ**:

   - ``AEGP_Embedding_NONE`` (0): なし
   - ``AEGP_Embedding_LINK`` (1): プロジェクトリンクを埋め込む
   - ``AEGP_Embedding_LINK_AND_COPY`` (2): プロジェクトリンクとプロキシを埋め込む

   **例**:

   .. code-block:: python

      embed = ae.sdk.AEGP_GetEmbedOptions(rq_itemH, outmodH)

.. function:: AEGP_SetEmbedOptions(rq_itemH: int, outmodH: int, embed_options: int) -> None

   埋め込みオプションを設定します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :param embed_options: 埋め込みタイプ（AEGP_EmbeddingType）
   :type embed_options: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetEmbedOptions(rq_itemH, outmodH, 1)  # プロジェクトリンクを埋め込む

レンダー後のアクション
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetPostRenderAction(rq_itemH: int, outmodH: int) -> int

   レンダー完了後のアクションを取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: レンダー後のアクション（AEGP_PostRenderAction）
   :rtype: int

   **レンダー後のアクション**:

   - ``AEGP_PostRenderOptions_NONE`` (0): なし
   - ``AEGP_PostRenderOptions_IMPORT`` (1): プロジェクトにインポート
   - ``AEGP_PostRenderOptions_IMPORT_AND_REPLACE_USAGE`` (2): インポートして使用箇所を置換
   - ``AEGP_PostRenderOptions_SET_PROXY`` (3): プロキシとして設定

   **例**:

   .. code-block:: python

      action = ae.sdk.AEGP_GetPostRenderAction(rq_itemH, outmodH)

.. function:: AEGP_SetPostRenderAction(rq_itemH: int, outmodH: int, post_render_action: int) -> None

   レンダー完了後のアクションを設定します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :param post_render_action: レンダー後のアクション（AEGP_PostRenderAction）
   :type post_render_action: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetPostRenderAction(rq_itemH, outmodH, 1)  # プロジェクトにインポート

有効な出力タイプ
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetEnabledOutputs(rq_itemH: int, outmodH: int) -> int

   有効な出力タイプ（ビデオ/オーディオ）のビットフィールドを取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: 有効な出力タイプのビットフィールド（AEGP_OutputTypes）
   :rtype: int

   **出力タイプのビットフィールド**:

   - ``AEGP_OutputType_NONE`` (0): なし
   - ``AEGP_OutputType_VIDEO`` (1): ビデオ出力が有効
   - ``AEGP_OutputType_AUDIO`` (2): オーディオ出力が有効
   - ``VIDEO | AUDIO`` (3): ビデオとオーディオ両方が有効

   **例**:

   .. code-block:: python

      enabled = ae.sdk.AEGP_GetEnabledOutputs(rq_itemH, outmodH)
      has_video = (enabled & 1) != 0
      has_audio = (enabled & 2) != 0

.. function:: AEGP_SetEnabledOutputs(rq_itemH: int, outmodH: int, enabled_types: int) -> None

   有効な出力タイプを設定します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :param enabled_types: 有効な出力タイプのビットフィールド（AEGP_OutputTypes）
   :type enabled_types: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetEnabledOutputs(rq_itemH, outmodH, 3)  # VIDEO | AUDIO

出力チャンネル
~~~~~~~~~~~~~~

.. function:: AEGP_GetOutputChannels(rq_itemH: int, outmodH: int) -> int

   出力ビデオチャンネル設定を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: 出力チャンネル（AEGP_VideoChannels）
   :rtype: int

   **ビデオチャンネル**:

   - ``AEGP_VideoChannels_RGB``: RGBのみ
   - ``AEGP_VideoChannels_RGBA``: RGB + アルファ
   - ``AEGP_VideoChannels_ALPHA``: アルファのみ

   **例**:

   .. code-block:: python

      channels = ae.sdk.AEGP_GetOutputChannels(rq_itemH, outmodH)

.. function:: AEGP_SetOutputChannels(rq_itemH: int, outmodH: int, output_channels: int) -> None

   出力ビデオチャンネルを設定します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :param output_channels: 出力チャンネル（AEGP_VideoChannels）
   :type output_channels: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetOutputChannels(rq_itemH, outmodH, AEGP_VideoChannels_RGBA)

ストレッチ情報
~~~~~~~~~~~~~~

.. function:: AEGP_GetStretchInfo(rq_itemH: int, outmodH: int) -> dict

   出力ストレッチ設定を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: ストレッチ情報の辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``is_enabled``
        - bool
        - ストレッチが有効か
      * - ``stretch_quality``
        - int
        - ストレッチ品質（AEGP_StretchQuality）
      * - ``locked``
        - bool
        - ロックされているか

   **例**:

   .. code-block:: python

      stretch = ae.sdk.AEGP_GetStretchInfo(rq_itemH, outmodH)
      if stretch['is_enabled']:
          print(f"ストレッチ品質: {stretch['stretch_quality']}")

.. function:: AEGP_SetStretchInfo(rq_itemH: int, outmodH: int, is_enabled: bool, stretch_quality: int) -> None

   出力ストレッチ設定を変更します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :param is_enabled: ストレッチを有効にするか
   :type is_enabled: bool
   :param stretch_quality: ストレッチ品質（AEGP_StretchQuality）
   :type stretch_quality: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetStretchInfo(rq_itemH, outmodH, True, 1)

クロップ情報
~~~~~~~~~~~~

.. function:: AEGP_GetCropInfo(rq_itemH: int, outmodH: int) -> dict

   出力クロップ設定を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: クロップ情報の辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``is_enabled``
        - bool
        - クロップが有効か
      * - ``left``
        - int
        - 左のクロップ量（ピクセル）
      * - ``top``
        - int
        - 上のクロップ量（ピクセル）
      * - ``right``
        - int
        - 右のクロップ量（ピクセル）
      * - ``bottom``
        - int
        - 下のクロップ量（ピクセル）

   **例**:

   .. code-block:: python

      crop = ae.sdk.AEGP_GetCropInfo(rq_itemH, outmodH)
      if crop['is_enabled']:
          print(f"クロップ領域: L={crop['left']}, T={crop['top']}, R={crop['right']}, B={crop['bottom']}")

.. function:: AEGP_SetCropInfo(rq_itemH: int, outmodH: int, is_enabled: bool, crop_rect: dict) -> None

   出力クロップ設定を変更します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :param is_enabled: クロップを有効にするか
   :type is_enabled: bool
   :param crop_rect: クロップ矩形の辞書（keys: left, top, right, bottom）
   :type crop_rect: dict

   **例**:

   .. code-block:: python

      crop_rect = {'left': 10, 'top': 10, 'right': 10, 'bottom': 10}
      ae.sdk.AEGP_SetCropInfo(rq_itemH, outmodH, True, crop_rect)

サウンドフォーマット情報
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetSoundFormatInfo(rq_itemH: int, outmodH: int) -> dict

   オーディオフォーマット設定を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: サウンドフォーマット情報の辞書
   :rtype: dict

   **戻り値の構造** (``AEGP_SoundDataFormat``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``sample_rate``
        - float
        - サンプルレート（Hz）
      * - ``encoding``
        - int
        - エンコーディング（AEGP_SoundEncoding）
      * - ``bytes_per_sample``
        - int
        - サンプルあたりのバイト数
      * - ``num_channels``
        - int
        - チャンネル数（1=モノラル、2=ステレオ）
      * - ``audio_enabled``
        - bool
        - オーディオが有効か

   **例**:

   .. code-block:: python

      sound_fmt = ae.sdk.AEGP_GetSoundFormatInfo(rq_itemH, outmodH)
      if sound_fmt['audio_enabled']:
          print(f"サンプルレート: {sound_fmt['sample_rate']} Hz")
          print(f"チャンネル数: {sound_fmt['num_channels']}")

.. function:: AEGP_SetSoundFormatInfo(rq_itemH: int, outmodH: int, sound_format: dict, audio_enabled: bool) -> None

   オーディオフォーマット設定を変更します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :param sound_format: サウンドフォーマット設定の辞書（一部のキーのみ指定可）
   :type sound_format: dict
   :param audio_enabled: オーディオを有効にするか
   :type audio_enabled: bool

   .. warning::
      オーディオ出力をサポートしない出力モジュールに対して ``audio_enabled=True`` を設定すると、エラーが発生します。
      事前に ``AEGP_GetEnabledOutputs()`` でオーディオサポートを確認してください。

   **例**:

   .. code-block:: python

      # オーディオをサポートしているか確認
      enabled = ae.sdk.AEGP_GetEnabledOutputs(rq_itemH, outmodH)
      if (enabled & 2) != 0:
          sound_fmt = {
              'sample_rate': 48000.0,
              'num_channels': 2
          }
          ae.sdk.AEGP_SetSoundFormatInfo(rq_itemH, outmodH, sound_fmt, True)

拡張出力モジュール情報
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetExtraOutputModuleInfo(rq_itemH: int, outmodH: int) -> dict

   出力モジュールの詳細情報を取得します。

   :param rq_itemH: レンダーキューアイテムハンドル
   :type rq_itemH: int
   :param outmodH: 出力モジュールハンドル
   :type outmodH: int
   :return: 拡張情報の辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``format``
        - str
        - フォーマット文字列（例: "QuickTime"）
      * - ``info``
        - str
        - 詳細情報文字列（コーデック情報など）
      * - ``is_sequence``
        - bool
        - シーケンス（画像連番）か
      * - ``multi_frame``
        - bool
        - マルチフレームか

   **例**:

   .. code-block:: python

      info = ae.sdk.AEGP_GetExtraOutputModuleInfo(rq_itemH, outmodH)
      print(f"フォーマット: {info['format']}")
      print(f"詳細: {info['info']}")
      print(f"シーケンス: {info['is_sequence']}")

使用例
------

基本的な出力モジュール設定
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_output_module(rq_itemH, output_path):
       """出力モジュールを設定して出力パスを指定"""
       # 最初の出力モジュールを取得
       outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)

       # 出力ファイルパスを設定
       ae.sdk.AEGP_SetOutputFilePath(rq_itemH, outmodH, output_path)

       # ビデオとオーディオ両方を有効化
       ae.sdk.AEGP_SetEnabledOutputs(rq_itemH, outmodH, 3)  # VIDEO | AUDIO

       # RGBAチャンネルを出力
       ae.sdk.AEGP_SetOutputChannels(rq_itemH, outmodH, AEGP_VideoChannels_RGBA)

       print(f"出力モジュールを設定しました: {output_path}")

   # 使用例
   setup_output_module(rq_itemH, "D:/output/render.mov")

クロップとストレッチの設定
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def apply_crop_and_stretch(rq_itemH):
       """出力にクロップとストレッチを適用"""
       outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)

       # クロップを設定（各辺10ピクセル）
       crop_rect = {
           'left': 10,
           'top': 10,
           'right': 10,
           'bottom': 10
       }
       ae.sdk.AEGP_SetCropInfo(rq_itemH, outmodH, True, crop_rect)

       # ストレッチを有効化（品質: 高）
       ae.sdk.AEGP_SetStretchInfo(rq_itemH, outmodH, True, 1)

       print("クロップとストレッチを適用しました")

   # 使用例
   apply_crop_and_stretch(rq_itemH)

オーディオ設定の変更
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def configure_audio_output(rq_itemH, sample_rate=48000.0, channels=2):
       """オーディオ出力設定を変更"""
       outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)

       # オーディオがサポートされているか確認
       enabled = ae.sdk.AEGP_GetEnabledOutputs(rq_itemH, outmodH)
       if (enabled & 2) == 0:
           print("この出力モジュールはオーディオをサポートしていません")
           return

       # オーディオフォーマットを設定
       sound_format = {
           'sample_rate': sample_rate,
           'num_channels': channels
       }
       ae.sdk.AEGP_SetSoundFormatInfo(rq_itemH, outmodH, sound_format, True)

       print(f"オーディオ設定を変更しました: {sample_rate}Hz, {channels}ch")

   # 使用例
   configure_audio_output(rq_itemH, 44100.0, 2)

レンダー後のアクション設定
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_post_render_import(rq_itemH):
       """レンダー後にプロジェクトへインポート"""
       outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)

       # レンダー後のアクション: プロジェクトにインポート
       ae.sdk.AEGP_SetPostRenderAction(rq_itemH, outmodH, 1)  # IMPORT

       # プロジェクトリンクを埋め込む
       ae.sdk.AEGP_SetEmbedOptions(rq_itemH, outmodH, 1)  # LINK

       print("レンダー後にプロジェクトへインポートします")

   # 使用例
   setup_post_render_import(rq_itemH)

出力モジュール情報の取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_output_module_info(rq_itemH):
       """出力モジュールの詳細情報を表示"""
       outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)

       # 基本情報
       path = ae.sdk.AEGP_GetOutputFilePath(rq_itemH, outmodH)
       enabled = ae.sdk.AEGP_GetEnabledOutputs(rq_itemH, outmodH)
       channels = ae.sdk.AEGP_GetOutputChannels(rq_itemH, outmodH)

       print("=" * 50)
       print(f"出力パス: {path}")
       print(f"ビデオ有効: {(enabled & 1) != 0}")
       print(f"オーディオ有効: {(enabled & 2) != 0}")
       print(f"チャンネル: {channels}")

       # 拡張情報
       extra = ae.sdk.AEGP_GetExtraOutputModuleInfo(rq_itemH, outmodH)
       print(f"\nフォーマット: {extra['format']}")
       print(f"詳細: {extra['info']}")
       print(f"シーケンス: {extra['is_sequence']}")

       # クロップ情報
       crop = ae.sdk.AEGP_GetCropInfo(rq_itemH, outmodH)
       if crop['is_enabled']:
           print(f"\nクロップ: L={crop['left']}, T={crop['top']}, R={crop['right']}, B={crop['bottom']}")

       # ストレッチ情報
       stretch = ae.sdk.AEGP_GetStretchInfo(rq_itemH, outmodH)
       if stretch['is_enabled']:
           print(f"ストレッチ品質: {stretch['stretch_quality']}")

       # オーディオ情報
       if (enabled & 2) != 0:
           sound = ae.sdk.AEGP_GetSoundFormatInfo(rq_itemH, outmodH)
           if sound['audio_enabled']:
               print(f"\nオーディオ:")
               print(f"  サンプルレート: {sound['sample_rate']} Hz")
               print(f"  チャンネル数: {sound['num_channels']}")

       print("=" * 50)

   # 使用例
   dump_output_module_info(rq_itemH)

複数出力モジュールの作成
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_multiple_outputs(rq_itemH):
       """複数の出力モジュールを作成して異なる設定を適用"""

       # ProRes出力
       outmod1 = ae.sdk.AEGP_AddDefaultOutputModule(rq_itemH)
       ae.sdk.AEGP_SetOutputFilePath(rq_itemH, outmod1, "D:/output/prores.mov")
       ae.sdk.AEGP_SetEnabledOutputs(rq_itemH, outmod1, 3)  # VIDEO | AUDIO

       # H.264出力
       outmod2 = ae.sdk.AEGP_AddDefaultOutputModule(rq_itemH)
       ae.sdk.AEGP_SetOutputFilePath(rq_itemH, outmod2, "D:/output/h264.mp4")
       ae.sdk.AEGP_SetEnabledOutputs(rq_itemH, outmod2, 3)  # VIDEO | AUDIO

       # PNG連番（ビデオのみ）
       outmod3 = ae.sdk.AEGP_AddDefaultOutputModule(rq_itemH)
       ae.sdk.AEGP_SetOutputFilePath(rq_itemH, outmod3, "D:/output/frames/[####].png")
       ae.sdk.AEGP_SetEnabledOutputs(rq_itemH, outmod3, 1)  # VIDEO only

       print("3つの出力モジュールを作成しました")

   # 使用例
   create_multiple_outputs(rq_itemH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **ハンドルペアの一貫性**

   ``rq_itemH`` と ``outmodH`` は必ず同じレンダーキューアイテムからのペアを使用してください。

   .. code-block:: python

      # 正しい
      outmodH = ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0)
      ae.sdk.AEGP_SetOutputFilePath(rq_itemH, outmodH, path)

      # 間違い（異なるRQアイテムのハンドル）
      ae.sdk.AEGP_SetOutputFilePath(other_rq_itemH, outmodH, path)

2. **インデックスは0ベース**

   ``AEGP_GetOutputModuleByIndex()`` は0ベースのインデックスを使用します（AEGP SDKの例外）。

3. **オーディオサポートの確認**

   オーディオ設定を変更する前に、必ず ``AEGP_GetEnabledOutputs()`` でオーディオサポートを確認してください。

   .. code-block:: python

      enabled = ae.sdk.AEGP_GetEnabledOutputs(rq_itemH, outmodH)
      if (enabled & 2) != 0:
          # オーディオをサポートしている
          ae.sdk.AEGP_SetSoundFormatInfo(rq_itemH, outmodH, sound_fmt, True)

4. **サウンドフォーマットの完全性**

   ``AEGP_SetSoundFormatInfo()`` は、すべてのフィールドが適切に初期化されている必要があります。
   現在の値を取得してから一部のみ変更する方式が推奨されます。

   .. code-block:: python

      # 現在の値を取得
      current = ae.sdk.AEGP_GetSoundFormatInfo(rq_itemH, outmodH)

      # 一部のみ変更
      current['sample_rate'] = 48000.0
      ae.sdk.AEGP_SetSoundFormatInfo(rq_itemH, outmodH, current, True)

ベストプラクティス
~~~~~~~~~~~~~~~~~~

出力パスの確認
^^^^^^^^^^^^^^

.. code-block:: python

   path = ae.sdk.AEGP_GetOutputFilePath(rq_itemH, outmodH)
   if not path:
       print("出力パスが設定されていません")

ハンドルの検証
^^^^^^^^^^^^^^

.. code-block:: python

   if outmodH == 0:
       raise ValueError("無効な出力モジュールハンドル")

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       ae.sdk.AEGP_SetOutputFilePath(rq_itemH, outmodH, path)
   except Exception as e:
       print(f"出力パスの設定に失敗しました: {e}")

出力タイプのビット演算
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # ビデオのみ有効化
   VIDEO_ONLY = 1

   # オーディオのみ有効化
   AUDIO_ONLY = 2

   # ビデオとオーディオ両方
   VIDEO_AND_AUDIO = 3

   ae.sdk.AEGP_SetEnabledOutputs(rq_itemH, outmodH, VIDEO_AND_AUDIO)

関連項目
--------

- :doc:`AEGP_RenderQueueSuite1` - レンダーキュー管理
- :doc:`AEGP_RQItemSuite4` - レンダーキューアイテム管理
- :doc:`AEGP_RenderOptionsSuite4` - レンダーオプション設定
- :doc:`index` - 低レベルAPI概要
