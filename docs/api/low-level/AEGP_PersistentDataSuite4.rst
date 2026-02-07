AEGP_PersistentDataSuite4
=========================

.. currentmodule:: ae.sdk

AEGP_PersistentDataSuite4は、After Effectsアプリケーションの環境設定や永続データを管理するためのSDK APIです。

概要
----

**実装状況**: 24/24関数実装 ✅

AEGP_PersistentDataSuite4は以下の機能を提供します:

- アプリケーション環境設定の読み書き（Blob/Section/Key構造）
- 各種データ型のサポート（整数、浮動小数点、文字列、バイナリ、時間、色）
- マシン固有/非依存の設定管理
- 環境設定ディレクトリの取得

基本概念
--------

データ構造（Blob/Section/Key）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

永続データは3階層の構造で管理されます:

.. code-block:: text

   Blob (AEGP_PersistentBlobH)
   ├── Section "MySection1"
   │   ├── Key "setting1" → 値
   │   ├── Key "setting2" → 値
   │   └── Key "setting3" → 値
   ├── Section "MySection2"
   │   ├── Key "option1" → 値
   │   └── Key "option2" → 値
   └── ...

**階層の説明**:

1. **Blob (ブロブ)**: データストレージの最上位レベル。タイプ（マシン固有/非依存など）ごとに異なる。
2. **Section (セクション)**: 関連する設定をグループ化する名前空間。
3. **Key (キー)**: 個別の設定項目。各キーは値（整数、文字列、バイナリなど）を持つ。

Blobの種類（AEGP_PersistentType）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Blobタイプ
     - 説明
   * - ``AEGP_PersistentType_MACHINE_SPECIFIC``
     - マシン固有の設定（パス、ハードウェア設定など）
   * - ``AEGP_PersistentType_MACHINE_INDEPENDENT``
     - マシン非依存の設定（共有可能）
   * - ``AEGP_PersistentType_MACHINE_INDEPENDENT_RENDER``
     - レンダリング設定（マシン非依存）
   * - ``AEGP_PersistentType_MACHINE_INDEPENDENT_OUTPUT``
     - 出力設定（マシン非依存）
   * - ``AEGP_PersistentType_MACHINE_INDEPENDENT_COMPOSITION``
     - コンポジション設定（マシン非依存）
   * - ``AEGP_PersistentType_MACHINE_SPECIFIC_TEXT``
     - テキスト関連設定（マシン固有）
   * - ``AEGP_PersistentType_MACHINE_SPECIFIC_PAINT``
     - ペイント関連設定（マシン固有）
   * - ``AEGP_PersistentType_MACHINE_SPECIFIC_EFFECTS`` [AE 24.0+]
     - エフェクト関連設定（マシン固有）
   * - ``AEGP_PersistentType_MACHINE_SPECIFIC_EXPRESSION_SNIPPETS`` [AE 24.0+]
     - エクスプレッションスニペット（マシン固有）
   * - ``AEGP_PersistentType_MACHINE_SPECIFIC_SCRIPT_SNIPPETS`` [AE 24.0+]
     - スクリプトスニペット（マシン固有）

.. tip::
   複数のマシンで設定を共有する場合は ``MACHINE_INDEPENDENT`` タイプを使用してください。
   パスやハードウェア設定など、マシン固有の情報は ``MACHINE_SPECIFIC`` タイプを使用します。

サポートされるデータ型
~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - データ型
     - Getter/Setter
     - Python型
   * - 整数
     - ``AEGP_GetLong`` / ``AEGP_SetLong``
     - ``int``
   * - 浮動小数点
     - ``AEGP_GetFpLong`` / ``AEGP_SetFpLong``
     - ``float``
   * - 文字列
     - ``AEGP_GetString`` / ``AEGP_SetString``
     - ``str``
   * - バイナリ（固定長）
     - ``AEGP_GetData`` / ``AEGP_SetData``
     - ``bytes``
   * - バイナリ（可変長）
     - ``AEGP_GetDataHandle`` / ``AEGP_SetDataHandle``
     - ``bytes``
   * - 時間
     - ``AEGP_GetTime`` / ``AEGP_SetTime``
     - ``tuple[int, int]`` (value, scale)
   * - 色（ARGB）
     - ``AEGP_GetARGB`` / ``AEGP_SetARGB``
     - ``tuple[float, float, float, float]`` (alpha, red, green, blue)

API リファレンス
----------------

Blob管理
~~~~~~~~

.. function:: AEGP_GetApplicationBlob(blob_type: int) -> int

   アプリケーションのBlobハンドルを取得します。

   :param blob_type: Blobタイプ（``AEGP_PersistentType_*`` 定数）
   :type blob_type: int
   :return: Blobハンドル（``uintptr_t``）
   :rtype: int

   **例**:

   .. code-block:: python

      # マシン非依存の設定用Blobを取得
      blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)

Section管理
~~~~~~~~~~~

.. function:: AEGP_GetNumSections(blobH: int) -> int

   Blob内のセクション数を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :return: セクション数
   :rtype: int

   **例**:

   .. code-block:: python

      num_sections = ae.sdk.AEGP_GetNumSections(blobH)
      print(f"セクション数: {num_sections}")

.. function:: AEGP_GetSectionKeyByIndex(blobH: int, section_index: int, max_size: int = 256) -> str

   インデックスからセクション名を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_index: セクションのインデックス（0から始まる）
   :type section_index: int
   :param max_size: バッファサイズ（デフォルト: 256）
   :type max_size: int
   :return: セクション名
   :rtype: str

   **例**:

   .. code-block:: python

      # 全セクションを列挙
      num_sections = ae.sdk.AEGP_GetNumSections(blobH)
      for i in range(num_sections):
          section_name = ae.sdk.AEGP_GetSectionKeyByIndex(blobH, i)
          print(f"セクション[{i}]: {section_name}")

Key管理
~~~~~~~

.. function:: AEGP_DoesKeyExist(blobH: int, section_key: str, value_key: str) -> bool

   指定したキーが存在するかどうかを確認します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :return: 存在する場合は ``True``、存在しない場合は ``False``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_DoesKeyExist(blobH, "MySection", "MySetting"):
          print("キーが存在します")

.. function:: AEGP_GetNumKeys(blobH: int, section_key: str) -> int

   セクション内のキー数を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :return: キー数
   :rtype: int

   **例**:

   .. code-block:: python

      num_keys = ae.sdk.AEGP_GetNumKeys(blobH, "MySection")
      print(f"キー数: {num_keys}")

.. function:: AEGP_GetValueKeyByIndex(blobH: int, section_key: str, key_index: int, max_size: int = 256) -> str

   インデックスからキー名を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param key_index: キーのインデックス（0から始まる）
   :type key_index: int
   :param max_size: バッファサイズ（デフォルト: 256）
   :type max_size: int
   :return: キー名
   :rtype: str

   **例**:

   .. code-block:: python

      # セクション内の全キーを列挙
      num_keys = ae.sdk.AEGP_GetNumKeys(blobH, "MySection")
      for i in range(num_keys):
          key_name = ae.sdk.AEGP_GetValueKeyByIndex(blobH, "MySection", i)
          print(f"キー[{i}]: {key_name}")

データ取得（Getters）
~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLong(blobH: int, section_key: str, value_key: str, default_value: int = 0) -> int

   整数値を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param default_value: デフォルト値（キーが存在しない場合）
   :type default_value: int
   :return: 整数値
   :rtype: int

   **例**:

   .. code-block:: python

      quality = ae.sdk.AEGP_GetLong(blobH, "RenderSettings", "Quality", 100)
      print(f"品質: {quality}")

.. function:: AEGP_GetFpLong(blobH: int, section_key: str, value_key: str, default_value: float = 0.0) -> float

   浮動小数点値を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param default_value: デフォルト値（キーが存在しない場合）
   :type default_value: float
   :return: 浮動小数点値
   :rtype: float

   **例**:

   .. code-block:: python

      scale = ae.sdk.AEGP_GetFpLong(blobH, "ViewSettings", "Scale", 1.0)
      print(f"スケール: {scale}")

.. function:: AEGP_GetString(blobH: int, section_key: str, value_key: str, default_value: str = "", buf_size: int = 1024) -> str

   文字列値を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param default_value: デフォルト値（キーが存在しない場合）
   :type default_value: str
   :param buf_size: バッファサイズ（デフォルト: 1024）
   :type buf_size: int
   :return: 文字列値
   :rtype: str

   **例**:

   .. code-block:: python

      output_path = ae.sdk.AEGP_GetString(blobH, "OutputSettings", "Path", "C:\\Output")
      print(f"出力パス: {output_path}")

.. function:: AEGP_GetData(blobH: int, section_key: str, value_key: str, data_size: int, default_data: bytes | None = None) -> bytes

   固定長バイナリデータを取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param data_size: データサイズ（バイト数）
   :type data_size: int
   :param default_data: デフォルトデータ（キーが存在しない場合）
   :type default_data: bytes | None
   :return: バイナリデータ
   :rtype: bytes

   .. note::
      データサイズが一致しない場合、デフォルト値が返されます。
      サイズが不明な場合は ``AEGP_GetDataHandle`` を使用してください。

   **例**:

   .. code-block:: python

      data = ae.sdk.AEGP_GetData(blobH, "CustomData", "Config", 128)
      print(f"データサイズ: {len(data)}")

.. function:: AEGP_GetDataHandle(blobH: int, section_key: str, value_key: str, default_data: bytes | None = None) -> bytes

   可変長バイナリデータを取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param default_data: デフォルトデータ（キーが存在しない場合）
   :type default_data: bytes | None
   :return: バイナリデータ
   :rtype: bytes

   .. tip::
      サイズが不明なバイナリデータには、``AEGP_GetData`` よりもこちらを使用してください。

   **例**:

   .. code-block:: python

      data = ae.sdk.AEGP_GetDataHandle(blobH, "CustomData", "LargeBlob")
      print(f"データサイズ: {len(data)}")

.. function:: AEGP_GetTime(blobH: int, section_key: str, value_key: str, default_time: tuple[int, int] | None = None) -> tuple[int, int]

   時間値を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param default_time: デフォルト時間 ``(value, scale)``（キーが存在しない場合）
   :type default_time: tuple[int, int] | None
   :return: 時間値 ``(value, scale)``
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      time = ae.sdk.AEGP_GetTime(blobH, "TimeSettings", "StartTime", (0, 1))
      value, scale = time
      print(f"時間: {value}/{scale} 秒")

.. function:: AEGP_GetARGB(blobH: int, section_key: str, value_key: str, default_color: tuple[float, float, float, float] | None = None) -> tuple[float, float, float, float]

   ARGB色値を取得します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param default_color: デフォルト色 ``(alpha, red, green, blue)``（キーが存在しない場合）
   :type default_color: tuple[float, float, float, float] | None
   :return: ARGB色値 ``(alpha, red, green, blue)`` (各成分0.0～1.0)
   :rtype: tuple[float, float, float, float]

   **例**:

   .. code-block:: python

      color = ae.sdk.AEGP_GetARGB(blobH, "ColorSettings", "Background", (1.0, 0.0, 0.0, 0.0))
      alpha, red, green, blue = color
      print(f"色: ARGB({alpha}, {red}, {green}, {blue})")

データ設定（Setters）
~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetLong(blobH: int, section_key: str, value_key: str, value: int) -> None

   整数値を設定します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param value: 設定する整数値
   :type value: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetLong(blobH, "RenderSettings", "Quality", 100)

.. function:: AEGP_SetFpLong(blobH: int, section_key: str, value_key: str, value: float) -> None

   浮動小数点値を設定します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param value: 設定する浮動小数点値
   :type value: float

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetFpLong(blobH, "ViewSettings", "Scale", 1.5)

.. function:: AEGP_SetString(blobH: int, section_key: str, value_key: str, value: str) -> None

   文字列値を設定します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param value: 設定する文字列値
   :type value: str

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetString(blobH, "OutputSettings", "Path", r"C:\Output")

.. function:: AEGP_SetData(blobH: int, section_key: str, value_key: str, data: bytes) -> None

   固定長バイナリデータを設定します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param data: 設定するバイナリデータ
   :type data: bytes

   **例**:

   .. code-block:: python

      data = b"\x01\x02\x03\x04"
      ae.sdk.AEGP_SetData(blobH, "CustomData", "Config", data)

.. function:: AEGP_SetDataHandle(blobH: int, section_key: str, value_key: str, data: bytes) -> None

   可変長バイナリデータを設定します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param data: 設定するバイナリデータ
   :type data: bytes

   **例**:

   .. code-block:: python

      large_data = b"\x00" * 10000
      ae.sdk.AEGP_SetDataHandle(blobH, "CustomData", "LargeBlob", large_data)

.. function:: AEGP_SetTime(blobH: int, section_key: str, value_key: str, time_tuple: tuple[int, int]) -> None

   時間値を設定します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param time_tuple: 設定する時間値 ``(value, scale)``
   :type time_tuple: tuple[int, int]

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetTime(blobH, "TimeSettings", "StartTime", (100, 30))  # 100/30秒

.. function:: AEGP_SetARGB(blobH: int, section_key: str, value_key: str, color_tuple: tuple[float, float, float, float]) -> None

   ARGB色値を設定します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str
   :param color_tuple: 設定する色値 ``(alpha, red, green, blue)`` (各成分0.0～1.0)
   :type color_tuple: tuple[float, float, float, float]

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetARGB(blobH, "ColorSettings", "Background", (1.0, 1.0, 1.0, 1.0))  # 白色

エントリ管理
~~~~~~~~~~~~

.. function:: AEGP_DeleteEntry(blobH: int, section_key: str, value_key: str) -> None

   キーを削除します。

   :param blobH: Blobハンドル
   :type blobH: int
   :param section_key: セクション名
   :type section_key: str
   :param value_key: キー名
   :type value_key: str

   .. note::
      キーが存在しない場合でもエラーは発生しません。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DeleteEntry(blobH, "MySection", "ObsoleteSetting")

環境設定ディレクトリ
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetPrefsDirectory() -> str

   After Effectsの環境設定ディレクトリパスを取得します。

   :return: 環境設定ディレクトリパス
   :rtype: str

   **例**:

   .. code-block:: python

      prefs_dir = ae.sdk.AEGP_GetPrefsDirectory()
      print(f"環境設定ディレクトリ: {prefs_dir}")

使用例
------

基本的な設定の読み書き
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def basic_persistent_data():
       """基本的な設定の読み書き"""
       # マシン非依存のBlobを取得
       blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)

       # 文字列設定
       ae.sdk.AEGP_SetString(blobH, "MyPlugin", "ProjectPath", r"C:\Projects\MyProject")
       path = ae.sdk.AEGP_GetString(blobH, "MyPlugin", "ProjectPath", "")
       print(f"プロジェクトパス: {path}")

       # 整数設定
       ae.sdk.AEGP_SetLong(blobH, "MyPlugin", "Quality", 100)
       quality = ae.sdk.AEGP_GetLong(blobH, "MyPlugin", "Quality", 0)
       print(f"品質: {quality}")

       # 浮動小数点設定
       ae.sdk.AEGP_SetFpLong(blobH, "MyPlugin", "Scale", 1.5)
       scale = ae.sdk.AEGP_GetFpLong(blobH, "MyPlugin", "Scale", 1.0)
       print(f"スケール: {scale}")

   # 実行
   basic_persistent_data()

セクションとキーの列挙
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def enumerate_persistent_data():
       """Blob内のすべてのセクションとキーを列挙"""
       blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)

       # 全セクションを列挙
       num_sections = ae.sdk.AEGP_GetNumSections(blobH)
       print(f"セクション数: {num_sections}\n")

       for section_idx in range(num_sections):
           section_name = ae.sdk.AEGP_GetSectionKeyByIndex(blobH, section_idx)
           print(f"[セクション] {section_name}")

           # セクション内の全キーを列挙
           num_keys = ae.sdk.AEGP_GetNumKeys(blobH, section_name)

           for key_idx in range(num_keys):
               key_name = ae.sdk.AEGP_GetValueKeyByIndex(blobH, section_name, key_idx)
               print(f"  - {key_name}")

           print()

   # 実行
   enumerate_persistent_data()

設定の存在確認とデフォルト値
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def safe_get_settings():
       """設定の存在を確認してから取得"""
       blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)

       section = "RenderSettings"
       key = "Quality"

       # キーの存在確認
       if ae.sdk.AEGP_DoesKeyExist(blobH, section, key):
           quality = ae.sdk.AEGP_GetLong(blobH, section, key, 0)
           print(f"既存の品質設定: {quality}")
       else:
           # デフォルト値を設定
           default_quality = 100
           ae.sdk.AEGP_SetLong(blobH, section, key, default_quality)
           print(f"デフォルト品質を設定: {default_quality}")

   # 実行
   safe_get_settings()

色設定の管理
~~~~~~~~~~~~

.. code-block:: python

   import ae

   def manage_color_settings():
       """色設定の読み書き"""
       blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)

       section = "ColorSettings"

       # 背景色を設定（黒）
       black = (1.0, 0.0, 0.0, 0.0)  # (alpha, red, green, blue)
       ae.sdk.AEGP_SetARGB(blobH, section, "Background", black)

       # ハイライト色を設定（黄色）
       yellow = (1.0, 1.0, 1.0, 0.0)
       ae.sdk.AEGP_SetARGB(blobH, section, "Highlight", yellow)

       # 色を取得
       bg_color = ae.sdk.AEGP_GetARGB(blobH, section, "Background")
       hl_color = ae.sdk.AEGP_GetARGB(blobH, section, "Highlight")

       print(f"背景色: ARGB{bg_color}")
       print(f"ハイライト色: ARGB{hl_color}")

   # 実行
   manage_color_settings()

バイナリデータの保存と読み込み
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import json

   def save_load_binary_data():
       """バイナリデータとして設定を保存"""
       blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)

       section = "CustomData"
       key = "PluginConfig"

       # 複雑な設定をJSONに変換してバイナリとして保存
       config = {
           "version": "1.0",
           "options": {
               "enable_feature_a": True,
               "enable_feature_b": False,
               "max_threads": 8
           },
           "paths": [
               r"C:\Path1",
               r"C:\Path2"
           ]
       }

       # JSON → bytes
       json_bytes = json.dumps(config, indent=2).encode('utf-8')
       ae.sdk.AEGP_SetDataHandle(blobH, section, key, json_bytes)
       print(f"設定を保存しました（{len(json_bytes)}バイト）")

       # 読み込み
       loaded_bytes = ae.sdk.AEGP_GetDataHandle(blobH, section, key)
       loaded_config = json.loads(loaded_bytes.decode('utf-8'))
       print(f"\n読み込んだ設定:\n{json.dumps(loaded_config, indent=2)}")

   # 実行
   save_load_binary_data()

設定のバックアップと復元
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import json

   def backup_restore_settings():
       """セクションの設定をバックアップ・復元"""
       blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)
       section = "MyPlugin"

       # --- バックアップ ---
       backup = {}
       num_keys = ae.sdk.AEGP_GetNumKeys(blobH, section)

       for i in range(num_keys):
           key = ae.sdk.AEGP_GetValueKeyByIndex(blobH, section, i)
           # 文字列として取得（簡易的な例）
           value = ae.sdk.AEGP_GetString(blobH, section, key, "")
           backup[key] = value

       print(f"バックアップ: {backup}")

       # --- 設定を変更 ---
       ae.sdk.AEGP_SetString(blobH, section, "TestKey", "ModifiedValue")

       # --- 復元 ---
       for key, value in backup.items():
           ae.sdk.AEGP_SetString(blobH, section, key, value)

       print("設定を復元しました")

   # 実行
   backup_restore_settings()

環境設定ディレクトリへのファイル保存
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import os

   def save_to_prefs_directory():
       """環境設定ディレクトリにファイルを保存"""
       prefs_dir = ae.sdk.AEGP_GetPrefsDirectory()
       print(f"環境設定ディレクトリ: {prefs_dir}")

       # プラグイン専用のサブディレクトリを作成
       plugin_dir = os.path.join(prefs_dir, "MyPlugin")
       os.makedirs(plugin_dir, exist_ok=True)

       # 設定ファイルを保存
       config_path = os.path.join(plugin_dir, "config.json")
       config = {"version": "1.0", "last_used": "2024-01-01"}

       with open(config_path, 'w', encoding='utf-8') as f:
           import json
           json.dump(config, f, indent=2)

       print(f"設定ファイルを保存: {config_path}")

   # 実行
   save_to_prefs_directory()

マシン固有/非依存の設定の使い分け
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def machine_specific_vs_independent():
       """マシン固有と非依存の設定の使い分け"""

       # マシン非依存設定（共有可能）
       independent_blobH = ae.sdk.AEGP_GetApplicationBlob(
           ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT
       )
       ae.sdk.AEGP_SetLong(independent_blobH, "MyPlugin", "DefaultQuality", 100)
       ae.sdk.AEGP_SetFpLong(independent_blobH, "MyPlugin", "DefaultScale", 1.0)
       print("マシン非依存設定を保存（他のマシンで共有可能）")

       # マシン固有設定（パスなど）
       specific_blobH = ae.sdk.AEGP_GetApplicationBlob(
           ae.sdk.AEGP_PersistentType_MACHINE_SPECIFIC
       )
       ae.sdk.AEGP_SetString(specific_blobH, "MyPlugin", "CachePath", r"D:\AECache")
       ae.sdk.AEGP_SetString(specific_blobH, "MyPlugin", "OutputPath", r"E:\Output")
       print("マシン固有設定を保存（このマシン専用）")

       # 取得
       quality = ae.sdk.AEGP_GetLong(independent_blobH, "MyPlugin", "DefaultQuality", 0)
       cache = ae.sdk.AEGP_GetString(specific_blobH, "MyPlugin", "CachePath", "")
       print(f"\n品質（共有設定）: {quality}")
       print(f"キャッシュパス（固有設定）: {cache}")

   # 実行
   machine_specific_vs_independent()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **Blobハンドルの有効性**

   ``AEGP_GetApplicationBlob`` で取得したハンドルは、After Effectsが実行中の間のみ有効です。

2. **セクション名・キー名の命名**

   セクション名とキー名は、他のプラグインと衝突しないように、プラグイン名をプレフィックスとして使用してください。

   .. code-block:: python

      # 良い例
      ae.sdk.AEGP_SetString(blobH, "MyPluginName", "Setting1", "value")

      # 悪い例（衝突の可能性）
      ae.sdk.AEGP_SetString(blobH, "Settings", "Value", "value")

3. **マシン固有/非依存の使い分け**

   - **非依存**: 品質、スケール、アルゴリズム設定など
   - **固有**: ファイルパス、ハードウェア設定、キャッシュディレクトリなど

4. **データサイズの確認**

   ``AEGP_GetData`` は固定長、``AEGP_GetDataHandle`` は可変長です。サイズが不明な場合は ``AEGP_GetDataHandle`` を使用してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

セクション名にプラグインIDを使用
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # プラグイン名をセクション名に含める
   PLUGIN_SECTION = "MyPluginName_v1"
   blobH = ae.sdk.AEGP_GetApplicationBlob(ae.sdk.AEGP_PersistentType_MACHINE_INDEPENDENT)
   ae.sdk.AEGP_SetString(blobH, PLUGIN_SECTION, "Setting1", "value")

キーの存在確認
^^^^^^^^^^^^^^

.. code-block:: python

   if ae.sdk.AEGP_DoesKeyExist(blobH, section, key):
       value = ae.sdk.AEGP_GetLong(blobH, section, key, 0)
   else:
       # 初回起動時の初期化処理
       ae.sdk.AEGP_SetLong(blobH, section, key, default_value)

デフォルト値の活用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # デフォルト値を指定することで、キーが存在しない場合でも安全
   quality = ae.sdk.AEGP_GetLong(blobH, "MyPlugin", "Quality", 100)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       value = ae.sdk.AEGP_GetLong(blobH, section, key, 0)
   except Exception as e:
       print(f"設定取得エラー: {e}")
       value = default_value

関連項目
--------

- :doc:`AEGP_MemorySuite1` - メモリ管理（DataHandleで使用）
- :doc:`AEGP_UtilitySuite6` - プラグインID取得
- :doc:`index` - 低レベルAPI概要
