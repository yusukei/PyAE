Sound Data
==========

.. currentmodule:: ae

オーディオデータを管理するAPIです。

概要
----

``SoundData`` は、After Effectsのオーディオサンプルデータを管理します。
PCM（符号なし/符号付き）および浮動小数点形式のオーディオデータに対応しています。

クラスリファレンス
------------------

.. autoclass:: SoundData
   :members:
   :undoc-members:
   :show-inheritance:

.. autoclass:: SoundEncoding
   :members:
   :undoc-members:

基本的な使用方法
----------------

サウンドデータの作成
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # 新しいサウンドデータを作成
   sound = ae.SoundData.create(
       sample_rate=48000.0,      # 48kHz
       encoding=ae.SoundEncoding.FLOAT,  # 浮動小数点
       bytes_per_sample=4,       # 4バイト（FLOATの場合は常に4）
       num_channels=2            # ステレオ
   )

   print(f"Sample Rate: {sound.sample_rate} Hz")
   print(f"Encoding: {sound.encoding}")
   print(f"Channels: {sound.num_channels}")

フォーマット情報の取得
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   sound = ae.SoundData.create(
       sample_rate=44100.0,
       encoding=ae.SoundEncoding.SIGNED_PCM,
       bytes_per_sample=2,
       num_channels=2
   )

   # 個別プロパティで取得
   print(f"Sample Rate: {sound.sample_rate}")
   print(f"Encoding: {sound.encoding}")
   print(f"Bytes/Sample: {sound.bytes_per_sample}")
   print(f"Channels: {sound.num_channels}")
   print(f"Samples: {sound.num_samples}")
   print(f"Duration: {sound.duration}s")

   # 辞書形式で取得
   fmt = sound.format
   print(f"Format: {fmt}")

サンプルデータの読み書き
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   sound = ae.SoundData.create(
       sample_rate=48000.0,
       encoding=ae.SoundEncoding.FLOAT,
       bytes_per_sample=4,
       num_channels=2
   )

   # サンプルデータを取得
   samples = sound.get_samples()
   print(f"Data size: {len(samples)} bytes")

   # サンプルデータを設定
   # サイズは num_samples * num_channels * bytes_per_sample と一致する必要あり
   # sound.set_samples(new_samples)

コンテキストマネージャの使用
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   sound = ae.SoundData.create(
       sample_rate=48000.0,
       encoding=ae.SoundEncoding.FLOAT,
       bytes_per_sample=4,
       num_channels=2
   )

   # コンテキストマネージャでサンプルをロック
   with sound:
       # この間、サウンドデータはロックされています
       samples = sound.get_samples()
       # 処理を実行...
   # 自動的にアンロックされます

SoundEncoding 列挙型
--------------------

.. list-table::
   :header-rows: 1
   :widths: 30 10 60

   * - 値
     - 数値
     - 説明
   * - ``UNSIGNED_PCM``
     - 3
     - 符号なしPCM
   * - ``SIGNED_PCM``
     - 4
     - 符号付きPCM
   * - ``FLOAT``
     - 5
     - 浮動小数点（32ビット）

プロパティ
----------

.. list-table::
   :header-rows: 1
   :widths: 25 15 60

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - データが有効かどうか（読み取り専用）
   * - ``sample_rate``
     - float
     - サンプルレート（Hz）
   * - ``encoding``
     - SoundEncoding
     - エンコーディング形式
   * - ``bytes_per_sample``
     - int
     - サンプルあたりのバイト数
   * - ``num_channels``
     - int
     - チャンネル数（1=モノラル、2=ステレオ）
   * - ``num_samples``
     - int
     - チャンネルあたりのサンプル数
   * - ``duration``
     - float
     - 継続時間（秒）
   * - ``format``
     - Dict
     - フォーマット情報の辞書

メソッド
--------

get_samples()
~~~~~~~~~~~~~

サンプルデータをバイト列として取得します。

.. code-block:: python

   samples = sound.get_samples()

.. note::
   - ステレオの場合、サンプルはインターリーブされています（L, R, L, R, ...）
   - データは自動的にコピーされ、バッファは自動的にアンロックされます

set_samples(data)
~~~~~~~~~~~~~~~~~

サンプルデータをバイト列から設定します。

.. code-block:: python

   # サンプルデータを設定
   sound.set_samples(new_samples)

.. warning::
   データサイズは ``num_samples * num_channels * bytes_per_sample`` と
   正確に一致する必要があります。

実用的な使用例
--------------

サウンドフォーマットの比較
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def print_sound_info(sound):
       """サウンドデータの情報を表示"""
       if not sound.valid:
           print("Invalid sound data")
           return

       print(f"Sample Rate: {sound.sample_rate} Hz")
       print(f"Duration: {sound.duration:.3f} seconds")
       print(f"Channels: {sound.num_channels}")
       print(f"Samples: {sound.num_samples}")

       encoding_names = {
           ae.SoundEncoding.UNSIGNED_PCM: "Unsigned PCM",
           ae.SoundEncoding.SIGNED_PCM: "Signed PCM",
           ae.SoundEncoding.FLOAT: "Float",
       }
       print(f"Encoding: {encoding_names.get(sound.encoding, 'Unknown')}")
       print(f"Bytes/Sample: {sound.bytes_per_sample}")

       # 合計データサイズを計算
       data_size = sound.num_samples * sound.num_channels * sound.bytes_per_sample
       print(f"Total Data Size: {data_size:,} bytes")

各種フォーマットでの作成
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # CD品質（44.1kHz 16bit ステレオ）
   cd_quality = ae.SoundData.create(
       sample_rate=44100.0,
       encoding=ae.SoundEncoding.SIGNED_PCM,
       bytes_per_sample=2,
       num_channels=2
   )

   # DVD品質（48kHz 24bit ステレオ）
   # 注意: 24bitはbytes_per_sample=3で表現
   dvd_quality = ae.SoundData.create(
       sample_rate=48000.0,
       encoding=ae.SoundEncoding.SIGNED_PCM,
       bytes_per_sample=3,
       num_channels=2
   )

   # プロ品質（96kHz Float ステレオ）
   pro_quality = ae.SoundData.create(
       sample_rate=96000.0,
       encoding=ae.SoundEncoding.FLOAT,
       bytes_per_sample=4,  # FLOAT時は常に4
       num_channels=2
   )

   # モノラル音声
   mono = ae.SoundData.create(
       sample_rate=48000.0,
       encoding=ae.SoundEncoding.SIGNED_PCM,
       bytes_per_sample=2,
       num_channels=1
   )

サウンドレンダリング
~~~~~~~~~~~~~~~~~~~~

サウンドのレンダリングは ``Renderer.render_sound()`` を使用します。

.. code-block:: python

   import ae

   def render_comp_audio(comp, start_time, duration):
       """コンポジションのオーディオをレンダリング"""

       sound = ae.Renderer.render_sound(
           item=comp._handle,
           start_time=start_time,
           duration=duration,
           sample_rate=48000.0,
           encoding=ae.SoundEncoding.FLOAT,
           bytes_per_sample=4,
           num_channels=2
       )

       if sound and sound.valid:
           print(f"Rendered {sound.duration:.3f} seconds of audio")
           print(f"Samples: {sound.num_samples}")

           # サンプルデータを取得
           samples = sound.get_samples()
           return samples

       return None

   # 使用例
   comp = ae.Comp.get_active()
   audio_data = render_comp_audio(comp, 0.0, 5.0)

注意事項
--------

.. note::
   - サンプルレートは一般的な値（44100, 48000, 96000など）を使用してください
   - FLOAT エンコーディングの場合、``bytes_per_sample`` は常に4になります
   - ステレオオーディオでは、サンプルはインターリーブ形式（L, R, L, R, ...）です

.. warning::
   - ``set_samples()`` で不正なサイズのデータを渡すとエラーが発生します
   - 無効なハンドルでメソッドを呼び出すとエラーが発生します
   - 操作前に ``valid`` プロパティを確認してください

関連項目
--------

- :class:`Renderer` - レンダリング実行（render_sound メソッド）
- :class:`World` - 映像フレームバッファ
- :doc:`/api/low-level/AEGP_SoundDataSuite1` - 低レベルAPI
