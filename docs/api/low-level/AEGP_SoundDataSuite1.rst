AEGP_SoundDataSuite1
====================

.. currentmodule:: ae.sdk

AEGP_SoundDataSuite1は、After Effectsのオーディオデータを管理・処理するためのSDK APIです。

概要
----

**実装状況**: 6/6関数実装（補助関数含む）

AEGP_SoundDataSuite1は以下の機能を提供します:

- サウンドデータバッファの作成と破棄
- オーディオフォーマット（サンプルレート、エンコーディング、チャンネル数）の管理
- オーディオサンプルデータへのアクセス（読み取り/書き込み）
- サンプル数の取得

基本概念
--------

サウンドデータハンドル (AEGP_SoundDataH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

オーディオデータを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - サウンドデータハンドルは ``AEGP_NewSoundData()`` で作成されます
   - 使用後は必ず ``AEGP_DisposeSoundData()`` で破棄してください
   - 破棄しないとメモリリークが発生します

サウンドデータフォーマット (AEGP_SoundDataFormat)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

オーディオデータのフォーマットを定義する構造体。以下の要素で構成されます:

.. list-table::
   :header-rows: 1

   * - パラメータ
     - 型
     - 説明
   * - ``sample_rate``
     - float
     - サンプルレート（Hz単位）。例: 44100.0, 48000.0
   * - ``encoding``
     - int
     - エンコーディング形式（3=UNSIGNED_PCM, 4=SIGNED_PCM, 5=FLOAT）
   * - ``bytes_per_sample``
     - int
     - サンプルあたりのバイト数（1, 2, 4）。FLOAT形式では無視される
   * - ``num_channels``
     - int
     - チャンネル数（1=モノラル, 2=ステレオ）

エンコーディング形式
~~~~~~~~~~~~~~~~~~~~

AEGP_SoundDataSuite1は3種類のエンコーディングをサポートします:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_SoundEncoding_UNSIGNED_PCM``
     - 3
     - 符号なしPCM（0-255または0-65535）
   * - ``AEGP_SoundEncoding_SIGNED_PCM``
     - 4
     - 符号付きPCM（-128~127または-32768~32767）
   * - ``AEGP_SoundEncoding_FLOAT``
     - 5
     - 浮動小数点（-1.0~1.0の範囲、常に4バイト）

.. note::
   ``AEGP_SoundEncoding_FLOAT`` を使用する場合、``bytes_per_sample`` は無視され、常に4バイト（FpShort型）になります。

ロック/アンロックの重要性
~~~~~~~~~~~~~~~~~~~~~~~~~~

サウンドデータのサンプルにアクセスする際は、**必ずロック/アンロックのペアを使用**してください:

1. ``AEGP_LockSoundDataSamples()`` でサンプルデータをロック
2. データの読み取り/書き込みを実行
3. ``AEGP_UnlockSoundDataSamples()`` でサンプルデータをアンロック

.. warning::
   ロック後にアンロックを忘れると、After Effectsがクラッシュする可能性があります。
   ``try-finally`` ブロックを使用して、確実にアンロックすることを推奨します。

API リファレンス
----------------

サウンドデータの作成と破棄
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NewSoundData(sample_rate: float, encoding: int, bytes_per_sample: int, num_channels: int) -> int

   新しいサウンドデータバッファを作成します。

   :param sample_rate: サンプルレート（Hz）。例: 44100.0, 48000.0
   :type sample_rate: float
   :param encoding: エンコーディング形式（3=UNSIGNED_PCM, 4=SIGNED_PCM, 5=FLOAT）
   :type encoding: int
   :param bytes_per_sample: サンプルあたりのバイト数（1, 2, 4）。FLOAT形式では無視される
   :type bytes_per_sample: int
   :param num_channels: チャンネル数（1=モノラル, 2=ステレオ）
   :type num_channels: int
   :return: サウンドデータハンドル（0の場合もあり）
   :rtype: int

   **例**:

   .. code-block:: python

      # 44.1kHz、ステレオ、SIGNED_PCMで16ビット（2バイト）のサウンドデータを作成
      soundH = ae.sdk.AEGP_NewSoundData(
          sample_rate=44100.0,
          encoding=ae.sdk.AEGP_SoundEncoding_SIGNED_PCM,
          bytes_per_sample=2,
          num_channels=2
      )

      # 48kHz、モノラル、FLOATのサウンドデータを作成
      soundH = ae.sdk.AEGP_NewSoundData(
          sample_rate=48000.0,
          encoding=ae.sdk.AEGP_SoundEncoding_FLOAT,
          bytes_per_sample=4,  # FLOATでは無視される
          num_channels=1
      )

.. function:: AEGP_DisposeSoundData(sound_dataH: int) -> None

   サウンドデータバッファを破棄します。

   :param sound_dataH: サウンドデータハンドル
   :type sound_dataH: int

   .. note::
      ハンドルが0（NULL）の場合、この関数は何もせず正常終了します。

   **例**:

   .. code-block:: python

      # サウンドデータを使用後に破棄
      ae.sdk.AEGP_DisposeSoundData(soundH)

サウンドデータフォーマットの取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetSoundDataFormat(soundH: int) -> tuple[float, int, int, int]

   サウンドデータのフォーマット情報を取得します。

   :param soundH: サウンドデータハンドル
   :type soundH: int
   :return: (sample_rate, encoding, bytes_per_sample, num_channels) のタプル
   :rtype: tuple[float, int, int, int]

   **例**:

   .. code-block:: python

      sample_rate, encoding, bytes_per_sample, num_channels = ae.sdk.AEGP_GetSoundDataFormat(soundH)
      print(f"サンプルレート: {sample_rate}Hz")
      print(f"エンコーディング: {encoding}")
      print(f"サンプルあたりのバイト数: {bytes_per_sample}")
      print(f"チャンネル数: {num_channels}")

サンプルデータへのアクセス
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_LockSoundDataSamples(soundH: int) -> tuple[int, int]

   サウンドデータのサンプルをロックし、メモリアドレスとサイズを取得します。

   :param soundH: サウンドデータハンドル
   :type soundH: int
   :return: (samples_ptr, buffer_size_bytes) のタプル
   :rtype: tuple[int, int]

   .. danger::
      この関数を使用した後は、**必ず** ``AEGP_UnlockSoundDataSamples()`` を呼び出してください。
      アンロックを忘れるとクラッシュの原因になります。

   **例**:

   .. code-block:: python

      # サンプルデータをロック
      samples_ptr, buffer_size = ae.sdk.AEGP_LockSoundDataSamples(soundH)
      try:
          # サンプルデータの処理（ctypes等を使用）
          pass
      finally:
          # 必ずアンロック
          ae.sdk.AEGP_UnlockSoundDataSamples(soundH)

.. function:: AEGP_LockSoundDataSamplesAsBytes(soundH: int) -> bytes

   サウンドデータのサンプルをロックし、Pythonのbytesオブジェクトとして取得します。

   :param soundH: サウンドデータハンドル
   :type soundH: int
   :return: サンプルデータのコピー
   :rtype: bytes

   .. tip::
      この関数は、データをコピーした後に自動的にアンロックします。
      読み取り専用のアクセスには、この関数を使用することを推奨します。

   **例**:

   .. code-block:: python

      # サンプルデータをbytesとして取得（自動アンロック）
      samples_bytes = ae.sdk.AEGP_LockSoundDataSamplesAsBytes(soundH)
      print(f"サンプルデータサイズ: {len(samples_bytes)}バイト")

.. function:: AEGP_UnlockSoundDataSamples(soundH: int) -> None

   サウンドデータのサンプルをアンロックします。

   :param soundH: サウンドデータハンドル
   :type soundH: int

   .. note::
      ``AEGP_LockSoundDataSamples()`` を呼び出した後は、必ずこの関数を呼び出してください。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_UnlockSoundDataSamples(soundH)

サンプル数の取得
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNumSamples(soundH: int) -> int

   サウンドデータバッファ内のサンプル数を取得します。

   :param soundH: サウンドデータハンドル
   :type soundH: int
   :return: サンプル数（チャンネルごと）
   :rtype: int

   .. note::
      ステレオの場合、この関数は **チャンネルごと** のサンプル数を返します。
      合計サンプル数は ``num_samples * num_channels`` になります。

   **例**:

   .. code-block:: python

      num_samples = ae.sdk.AEGP_GetNumSamples(soundH)
      _, _, _, num_channels = ae.sdk.AEGP_GetSoundDataFormat(soundH)
      total_samples = num_samples * num_channels
      print(f"チャンネルあたりのサンプル数: {num_samples}")
      print(f"合計サンプル数: {total_samples}")

定数
----

エンコーディング定数
~~~~~~~~~~~~~~~~~~~~

以下の定数は ``ae.sdk`` モジュールで利用可能です:

.. py:data:: AEGP_SoundEncoding_UNSIGNED_PCM
   :value: 3

   符号なしPCMエンコーディング

.. py:data:: AEGP_SoundEncoding_SIGNED_PCM
   :value: 4

   符号付きPCMエンコーディング

.. py:data:: AEGP_SoundEncoding_FLOAT
   :value: 5

   浮動小数点エンコーディング（-1.0~1.0）

.. py:data:: AEGP_SoundEncoding_BEGIN
   :value: 3

   エンコーディング範囲の開始

.. py:data:: AEGP_SoundEncoding_END
   :value: 6

   エンコーディング範囲の終了

使用例
------

サウンドデータの作成と破棄
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_empty_audio_buffer():
       """空のオーディオバッファを作成して破棄する例"""
       # 44.1kHz、ステレオ、16ビットSIGNED_PCMのサウンドデータを作成
       soundH = ae.sdk.AEGP_NewSoundData(
           sample_rate=44100.0,
           encoding=ae.sdk.AEGP_SoundEncoding_SIGNED_PCM,
           bytes_per_sample=2,
           num_channels=2
       )

       try:
           print(f"サウンドデータハンドル: {soundH}")

           # フォーマット情報を取得
           sample_rate, encoding, bytes_per_sample, num_channels = ae.sdk.AEGP_GetSoundDataFormat(soundH)
           print(f"サンプルレート: {sample_rate}Hz")
           print(f"エンコーディング: {encoding}")
           print(f"サンプルあたりのバイト数: {bytes_per_sample}")
           print(f"チャンネル数: {num_channels}")

       finally:
           # 必ず破棄
           ae.sdk.AEGP_DisposeSoundData(soundH)

   # 実行
   create_empty_audio_buffer()

サンプルデータの読み取り（安全な方法）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def read_audio_samples(soundH):
       """サンプルデータを安全に読み取る例"""
       # サンプルデータをbytesとして取得（自動アンロック）
       samples_bytes = ae.sdk.AEGP_LockSoundDataSamplesAsBytes(soundH)

       # フォーマット情報を取得
       sample_rate, encoding, bytes_per_sample, num_channels = ae.sdk.AEGP_GetSoundDataFormat(soundH)
       num_samples = ae.sdk.AEGP_GetNumSamples(soundH)

       print(f"サンプルデータサイズ: {len(samples_bytes)}バイト")
       print(f"サンプル数: {num_samples}")
       print(f"チャンネル数: {num_channels}")

       # バイトデータを処理（例: numpy配列に変換）
       import struct
       if encoding == ae.sdk.AEGP_SoundEncoding_SIGNED_PCM and bytes_per_sample == 2:
           # 16ビット符号付きPCM
           format_char = 'h'  # signed short
           samples = struct.unpack(f'{len(samples_bytes)//2}{format_char}', samples_bytes)
           print(f"最初の10サンプル: {samples[:10]}")

   # 使用例（soundHが既に存在する場合）
   # read_audio_samples(soundH)

サンプルデータの読み取り（手動ロック/アンロック）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ctypes

   def read_audio_samples_manual(soundH):
       """サンプルデータを手動でロック/アンロックして読み取る例"""
       # サンプルデータをロック
       samples_ptr, buffer_size = ae.sdk.AEGP_LockSoundDataSamples(soundH)

       try:
           # フォーマット情報を取得
           sample_rate, encoding, bytes_per_sample, num_channels = ae.sdk.AEGP_GetSoundDataFormat(soundH)
           num_samples = ae.sdk.AEGP_GetNumSamples(soundH)

           print(f"サンプルデータポインタ: 0x{samples_ptr:X}")
           print(f"バッファサイズ: {buffer_size}バイト")

           # ctypesを使用してメモリからデータを読み取る
           if encoding == ae.sdk.AEGP_SoundEncoding_SIGNED_PCM and bytes_per_sample == 2:
               # 16ビット符号付きPCM
               total_samples = num_samples * num_channels
               samples_array = (ctypes.c_int16 * total_samples).from_address(samples_ptr)
               print(f"最初の10サンプル: {list(samples_array[:10])}")

       finally:
           # 必ずアンロック
           ae.sdk.AEGP_UnlockSoundDataSamples(soundH)

   # 使用例（soundHが既に存在する場合）
   # read_audio_samples_manual(soundH)

オーディオフォーマットの変換
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def convert_audio_to_float(source_soundH):
       """PCMオーディオをFLOAT形式に変換する例"""
       # 元のフォーマット情報を取得
       sample_rate, encoding, bytes_per_sample, num_channels = ae.sdk.AEGP_GetSoundDataFormat(source_soundH)
       num_samples = ae.sdk.AEGP_GetNumSamples(source_soundH)

       print(f"元のフォーマット:")
       print(f"  サンプルレート: {sample_rate}Hz")
       print(f"  エンコーディング: {encoding}")
       print(f"  サンプル数: {num_samples}")

       # FLOAT形式の新しいサウンドデータを作成
       float_soundH = ae.sdk.AEGP_NewSoundData(
           sample_rate=sample_rate,
           encoding=ae.sdk.AEGP_SoundEncoding_FLOAT,
           bytes_per_sample=4,  # FLOATでは無視される
           num_channels=num_channels
       )

       try:
           print(f"\n変換後のフォーマット:")
           new_sample_rate, new_encoding, new_bytes_per_sample, new_num_channels = ae.sdk.AEGP_GetSoundDataFormat(float_soundH)
           print(f"  サンプルレート: {new_sample_rate}Hz")
           print(f"  エンコーディング: {new_encoding}")
           print(f"  バイト/サンプル: {new_bytes_per_sample}")

           # ここで実際のサンプルデータ変換処理を行う
           # （簡略化のため省略）

           return float_soundH

       except Exception as e:
           ae.sdk.AEGP_DisposeSoundData(float_soundH)
           raise

   # 使用例（source_soundHが既に存在する場合）
   # float_soundH = convert_audio_to_float(source_soundH)

オーディオ情報のダンプ
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_sound_data_info(soundH):
       """サウンドデータの詳細情報を表示"""
       print("=" * 60)
       print("サウンドデータ情報")
       print("=" * 60)

       # フォーマット情報
       sample_rate, encoding, bytes_per_sample, num_channels = ae.sdk.AEGP_GetSoundDataFormat(soundH)

       encoding_names = {
           3: "UNSIGNED_PCM",
           4: "SIGNED_PCM",
           5: "FLOAT"
       }

       print(f"\nフォーマット:")
       print(f"  サンプルレート: {sample_rate}Hz")
       print(f"  エンコーディング: {encoding_names.get(encoding, f'Unknown({encoding})')}")
       print(f"  バイト/サンプル: {bytes_per_sample}")
       print(f"  チャンネル数: {num_channels} ({'モノラル' if num_channels == 1 else 'ステレオ'})")

       # サンプル数
       num_samples = ae.sdk.AEGP_GetNumSamples(soundH)
       total_samples = num_samples * num_channels

       print(f"\nサンプル:")
       print(f"  チャンネルあたりのサンプル数: {num_samples:,}")
       print(f"  合計サンプル数: {total_samples:,}")

       # データサイズ
       if encoding == ae.sdk.AEGP_SoundEncoding_FLOAT:
           actual_bytes_per_sample = 4
       else:
           actual_bytes_per_sample = bytes_per_sample

       total_size = total_samples * actual_bytes_per_sample

       print(f"\nデータサイズ:")
       print(f"  合計: {total_size:,}バイト ({total_size / 1024:.2f}KB)")

       # 再生時間
       if sample_rate > 0:
           duration_seconds = num_samples / sample_rate
           minutes = int(duration_seconds // 60)
           seconds = duration_seconds % 60
           print(f"\n再生時間: {minutes}分{seconds:.2f}秒")

       print("=" * 60)

   # 使用例（soundHが既に存在する場合）
   # dump_sound_data_info(soundH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリ管理**

   ``AEGP_NewSoundData()`` で作成したハンドルは、必ず ``AEGP_DisposeSoundData()`` で破棄してください。

2. **ロック/アンロックのペア**

   ``AEGP_LockSoundDataSamples()`` を呼び出したら、必ず ``AEGP_UnlockSoundDataSamples()`` を呼び出してください。
   ``try-finally`` ブロックの使用を推奨します。

3. **FLOATエンコーディングのバイト数**

   ``AEGP_SoundEncoding_FLOAT`` を使用する場合、``bytes_per_sample`` パラメータは無視され、常に4バイトになります。

4. **ステレオのサンプル数**

   ステレオオーディオの場合、``AEGP_GetNumSamples()`` は **チャンネルごと** のサンプル数を返します。
   合計サンプル数は ``num_samples * num_channels`` です。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

安全なサンプルアクセス
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 推奨: 読み取り専用の場合
   samples_bytes = ae.sdk.AEGP_LockSoundDataSamplesAsBytes(soundH)

   # 手動ロックが必要な場合
   samples_ptr, buffer_size = ae.sdk.AEGP_LockSoundDataSamples(soundH)
   try:
       # データ処理
       pass
   finally:
       ae.sdk.AEGP_UnlockSoundDataSamples(soundH)

メモリリークの防止
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   soundH = ae.sdk.AEGP_NewSoundData(44100.0, ae.sdk.AEGP_SoundEncoding_FLOAT, 4, 2)
   try:
       # サウンドデータを使用
       pass
   finally:
       ae.sdk.AEGP_DisposeSoundData(soundH)

エンコーディングの選択
^^^^^^^^^^^^^^^^^^^^^^

- **UNSIGNED_PCM**: 古いフォーマット、互換性のため
- **SIGNED_PCM**: 標準的なPCMオーディオ（推奨）
- **FLOAT**: 高精度オーディオ処理、エフェクト処理時（最高品質）

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       soundH = ae.sdk.AEGP_NewSoundData(44100.0, ae.sdk.AEGP_SoundEncoding_SIGNED_PCM, 2, 2)
   except Exception as e:
       print(f"サウンドデータ作成失敗: {e}")
       return

パフォーマンス考慮事項
~~~~~~~~~~~~~~~~~~~~~~

1. **大きなオーディオデータの処理**

   大量のサンプルを処理する場合は、チャンク単位で処理することを検討してください。

2. **bytesコピーのオーバーヘッド**

   ``AEGP_LockSoundDataSamplesAsBytes()`` はデータをコピーするため、大きなバッファではオーバーヘッドがあります。
   パフォーマンスが重要な場合は、``AEGP_LockSoundDataSamples()`` と ``ctypes`` を使用してください。

3. **ロック時間の最小化**

   サンプルデータをロックしている間は、必要最小限の処理のみを行い、すぐにアンロックしてください。

関連項目
--------

- :doc:`AEGP_LayerSuite9` - レイヤーのオーディオデータ取得
- :doc:`AEGP_FootageSuite5` - フッテージのオーディオデータ取得
- :doc:`AEGP_RenderSuite5` - オーディオのレンダリング
- :doc:`index` - 低レベルAPI概要
