AEGP_MarkerSuite3
=================

.. currentmodule:: ae.sdk

AEGP_MarkerSuite3は、After Effectsのマーカーの作成、編集、取得を行うためのSDK APIです。

概要
----

**実装状況**: 14/14関数実装 ✅

AEGP_MarkerSuite3は以下の機能を提供します:

- マーカーの作成と削除
- マーカー文字列（コメント、チャプター、URL等）の取得と設定
- マーカーフラグ（ナビゲーション、保護リージョン）の操作
- マーカーの継続時間とラベル色の管理
- キューポイントパラメータの操作

基本概念
--------

マーカーハンドル (AEGP_MarkerValP)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのマーカーを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - マーカーハンドルは、``AEGP_NewMarker()`` で作成後、必ず ``AEGP_DisposeMarker()`` で解放する必要があります
   - マーカーハンドルはメモリリークを防ぐため、使用後は必ず破棄してください
   - ``AEGP_DuplicateMarker()`` で複製したマーカーも同様に破棄が必要です

マーカー文字列の種類 (AEGP_MarkerStringType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

マーカーには以下の種類の文字列を設定できます:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_MarkerString_NONE``
     - 0
     - なし（使用しない）
   * - ``AEGP_MarkerString_COMMENT``
     - 1
     - コメント（一般的なマーカーテキスト）
   * - ``AEGP_MarkerString_CHAPTER``
     - 2
     - チャプター名（ビデオのセクション分け）
   * - ``AEGP_MarkerString_URL``
     - 3
     - URL（Webリンク）
   * - ``AEGP_MarkerString_FRAME_TARGET``
     - 4
     - フレームターゲット（ジャンプ先）
   * - ``AEGP_MarkerString_CUE_POINT_NAME``
     - 5
     - キューポイント名（Flash/Web用）

マーカーフラグ (AEGP_MarkerFlagType)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

マーカーの動作を制御するフラグ:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_MarkerFlag_NONE``
     - 0x00000000
     - フラグなし
   * - ``AEGP_MarkerFlag_NAVIGATION``
     - 0x00000001
     - ナビゲーションマーカー（0の場合はイベントマーカー）
   * - ``AEGP_MarkerFlag_PROTECT_REGION``
     - 0x00000002
     - 保護リージョン（プリコンプレイヤー上でタイムストレッチから保護）

マーカーラベル色
~~~~~~~~~~~~~~~~

マーカーには0〜16のラベル色インデックスを設定できます。色はAfter Effectsのラベル設定に対応します。

API リファレンス
----------------

マーカーの作成と削除
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NewMarker() -> int

   新しいマーカーを作成します。

   :return: 新しいマーカーハンドル
   :rtype: int

   .. warning::
      作成したマーカーは必ず ``AEGP_DisposeMarker()`` で解放してください。
      解放しない場合、メモリリークが発生します。

   **例**:

   .. code-block:: python

      markerP = ae.sdk.AEGP_NewMarker()
      try:
          # マーカーを使用
          ae.sdk.AEGP_SetMarkerString(markerP, 1, "My Marker")
      finally:
          ae.sdk.AEGP_DisposeMarker(markerP)

.. function:: AEGP_DisposeMarker(markerP: int) -> None

   マーカーハンドルを解放します。

   :param markerP: マーカーハンドル
   :type markerP: int

   .. note::
      この関数は、``AEGP_NewMarker()`` または ``AEGP_DuplicateMarker()`` で作成したマーカーにのみ使用してください。
      レイヤーから取得したマーカーハンドルには使用しないでください。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DisposeMarker(markerP)

.. function:: AEGP_DuplicateMarker(markerP: int) -> int

   マーカーを複製します。

   :param markerP: 複製元のマーカーハンドル
   :type markerP: int
   :return: 新しいマーカーハンドル
   :rtype: int

   .. warning::
      複製したマーカーは必ず ``AEGP_DisposeMarker()`` で解放してください。

   **例**:

   .. code-block:: python

      original_markerP = ae.sdk.AEGP_NewMarker()
      ae.sdk.AEGP_SetMarkerString(original_markerP, 1, "Original")

      # 複製
      duplicate_markerP = ae.sdk.AEGP_DuplicateMarker(original_markerP)

      # 両方とも解放が必要
      ae.sdk.AEGP_DisposeMarker(original_markerP)
      ae.sdk.AEGP_DisposeMarker(duplicate_markerP)

マーカー文字列の操作
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetMarkerString(markerP: int, strType: int, str: str) -> None

   マーカーに文字列を設定します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :param strType: 文字列タイプ（1=COMMENT, 2=CHAPTER, 3=URL, 4=FRAME_TARGET, 5=CUE_POINT_NAME）
   :type strType: int
   :param str: 設定する文字列（UTF-8）
   :type str: str

   .. note::
      内部的にUTF-8からUTF-16に自動変換されます。日本語などのマルチバイト文字も正しく処理されます。

   **例**:

   .. code-block:: python

      markerP = ae.sdk.AEGP_NewMarker()

      # コメントを設定
      ae.sdk.AEGP_SetMarkerString(markerP, 1, "重要なシーン")

      # チャプター名を設定
      ae.sdk.AEGP_SetMarkerString(markerP, 2, "第1章")

      # URLを設定
      ae.sdk.AEGP_SetMarkerString(markerP, 3, "https://example.com")

.. function:: AEGP_GetMarkerString(plugin_id: int, markerP: int, strType: int) -> str

   マーカーの文字列を取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param markerP: マーカーハンドル
   :type markerP: int
   :param strType: 文字列タイプ（1=COMMENT, 2=CHAPTER, 3=URL, 4=FRAME_TARGET, 5=CUE_POINT_NAME）
   :type strType: int
   :return: マーカーの文字列（UTF-8）。設定されていない場合は空文字列。
   :rtype: str

   .. note::
      ``plugin_id`` はメモリ管理のために必要です。``ae.sdk.AEGP_GetPluginID()`` で取得してください。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()

      # コメントを取得
      comment = ae.sdk.AEGP_GetMarkerString(plugin_id, markerP, 1)
      print(f"コメント: {comment}")

      # URLを取得
      url = ae.sdk.AEGP_GetMarkerString(plugin_id, markerP, 3)
      if url:
          print(f"URL: {url}")

マーカーフラグの操作
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetMarkerFlag(markerP: int, flagType: int) -> bool

   マーカーフラグの値を取得します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :param flagType: フラグタイプ（1=NAVIGATION, 2=PROTECT_REGION）
   :type flagType: int
   :return: フラグが設定されている場合は ``True``、そうでない場合は ``False``
   :rtype: bool

   **フラグタイプ**:

   - ``1`` (NAVIGATION): ナビゲーションマーカー（タイムラインのジャンプポイント）
   - ``2`` (PROTECT_REGION): 保護リージョン（タイムストレッチから保護）

   **例**:

   .. code-block:: python

      is_navigation = ae.sdk.AEGP_GetMarkerFlag(markerP, 1)
      is_protected = ae.sdk.AEGP_GetMarkerFlag(markerP, 2)

      if is_navigation:
          print("このマーカーはナビゲーション用です")
      if is_protected:
          print("このマーカーは保護されています")

.. function:: AEGP_SetMarkerFlag(markerP: int, flagType: int, valueB: bool) -> None

   マーカーフラグの値を設定します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :param flagType: フラグタイプ（1=NAVIGATION, 2=PROTECT_REGION）
   :type flagType: int
   :param valueB: フラグの値（``True`` で有効、``False`` で無効）
   :type valueB: bool

   **例**:

   .. code-block:: python

      markerP = ae.sdk.AEGP_NewMarker()

      # ナビゲーションマーカーとして設定
      ae.sdk.AEGP_SetMarkerFlag(markerP, 1, True)

      # 保護リージョンとして設定
      ae.sdk.AEGP_SetMarkerFlag(markerP, 2, True)

マーカーの継続時間とラベル
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetMarkerDuration(markerP: int) -> dict

   マーカーの継続時間を取得します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :return: 継続時間を表す辞書（``A_Time`` 構造体）
   :rtype: dict

   **戻り値の構造** (``A_Time``):

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``value``
        - int
        - 時間の値
      * - ``scale``
        - int
        - スケール（1秒あたりのユニット数）

   .. note::
      継続時間は ``value / scale`` 秒として計算されます。
      継続時間が0の場合、マーカーは単一のフレームマーカーです。

   **例**:

   .. code-block:: python

      duration = ae.sdk.AEGP_GetMarkerDuration(markerP)
      duration_seconds = duration['value'] / duration['scale']
      print(f"継続時間: {duration_seconds}秒")

.. function:: AEGP_SetMarkerDuration(markerP: int, durationT: dict) -> None

   マーカーの継続時間を設定します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :param durationT: 継続時間を表す辞書（``value`` と ``scale`` キーを含む）
   :type durationT: dict

   .. note::
      継続時間を設定すると、マーカーは範囲マーカーになります。

   **例**:

   .. code-block:: python

      # 2秒の継続時間を設定
      ae.sdk.AEGP_SetMarkerDuration(markerP, {'value': 2, 'scale': 1})

      # 30フレーム（FPS=30の場合は1秒）
      ae.sdk.AEGP_SetMarkerDuration(markerP, {'value': 30, 'scale': 30})

.. function:: AEGP_GetMarkerLabel(markerP: int) -> int

   マーカーのラベル色インデックスを取得します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :return: ラベル色インデックス（0〜16）
   :rtype: int

   **例**:

   .. code-block:: python

      label = ae.sdk.AEGP_GetMarkerLabel(markerP)
      print(f"ラベル色: {label}")

.. function:: AEGP_SetMarkerLabel(markerP: int, value: int) -> None

   マーカーのラベル色インデックスを設定します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :param value: ラベル色インデックス（0〜16）
   :type value: int

   .. note::
      ラベル色はAfter Effectsのラベル設定（環境設定）に対応します。

   **例**:

   .. code-block:: python

      # ラベル色を赤（インデックス1）に設定
      ae.sdk.AEGP_SetMarkerLabel(markerP, 1)

キューポイントパラメータの操作
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CountCuePointParams(markerP: int) -> int

   マーカーに設定されているキューポイントパラメータの数を取得します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :return: キューポイントパラメータ数
   :rtype: int

   .. note::
      キューポイントパラメータは、Flash/Web用のメタデータとして使用されます。

   **例**:

   .. code-block:: python

      param_count = ae.sdk.AEGP_CountCuePointParams(markerP)
      print(f"キューポイントパラメータ数: {param_count}")

.. function:: AEGP_GetIndCuePointParam(plugin_id: int, markerP: int, param_indexL: int) -> dict

   指定したインデックスのキューポイントパラメータを取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param markerP: マーカーハンドル
   :type markerP: int
   :param param_indexL: パラメータのインデックス（0から始まる）
   :type param_indexL: int
   :return: キューポイントパラメータの辞書（``key`` と ``value`` キーを含む）
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``key``
        - str
        - パラメータのキー（UTF-8）
      * - ``value``
        - str
        - パラメータの値（UTF-8）

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()

      param_count = ae.sdk.AEGP_CountCuePointParams(markerP)
      for i in range(param_count):
          param = ae.sdk.AEGP_GetIndCuePointParam(plugin_id, markerP, i)
          print(f"キー: {param['key']}, 値: {param['value']}")

.. function:: AEGP_SetIndCuePointParam(markerP: int, param_indexL: int, key: str, value: str) -> None

   指定したインデックスのキューポイントパラメータを設定します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :param param_indexL: パラメータのインデックス（0から始まる）
   :type param_indexL: int
   :param key: パラメータのキー（UTF-8）
   :type key: str
   :param value: パラメータの値（UTF-8）
   :type value: str

   .. note::
      インデックスが存在しない場合、先に ``AEGP_InsertCuePointParam()`` を呼び出す必要があります。

   **例**:

   .. code-block:: python

      # スロットを挿入してから設定
      ae.sdk.AEGP_InsertCuePointParam(markerP, 0)
      ae.sdk.AEGP_SetIndCuePointParam(markerP, 0, "type", "action")

      ae.sdk.AEGP_InsertCuePointParam(markerP, 1)
      ae.sdk.AEGP_SetIndCuePointParam(markerP, 1, "name", "play_sound")

.. function:: AEGP_InsertCuePointParam(markerP: int, param_indexL: int) -> None

   新しいキューポイントパラメータスロットを挿入します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :param param_indexL: 挿入位置のインデックス（0から始まる）
   :type param_indexL: int

   .. important::
      この関数は空のスロットを挿入するだけです。
      実際のキー・値を設定するには、``AEGP_SetIndCuePointParam()`` を呼び出す必要があります。

   **例**:

   .. code-block:: python

      # 新しいスロットを挿入
      ae.sdk.AEGP_InsertCuePointParam(markerP, 0)

      # キー・値を設定
      ae.sdk.AEGP_SetIndCuePointParam(markerP, 0, "key1", "value1")

.. function:: AEGP_DeleteIndCuePointParam(markerP: int, param_indexL: int) -> None

   指定したインデックスのキューポイントパラメータを削除します。

   :param markerP: マーカーハンドル
   :type markerP: int
   :param param_indexL: パラメータのインデックス（0から始まる）
   :type param_indexL: int

   **例**:

   .. code-block:: python

      # インデックス0のパラメータを削除
      ae.sdk.AEGP_DeleteIndCuePointParam(markerP, 0)

使用例
------

基本的なマーカーの作成と設定
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_marker_example():
       """基本的なマーカーを作成し、レイヤーに追加"""
       # マーカーを作成
       markerP = ae.sdk.AEGP_NewMarker()

       try:
           # コメントを設定
           ae.sdk.AEGP_SetMarkerString(markerP, 1, "重要なシーン")

           # ナビゲーションマーカーとして設定
           ae.sdk.AEGP_SetMarkerFlag(markerP, 1, True)

           # ラベル色を設定（赤=1）
           ae.sdk.AEGP_SetMarkerLabel(markerP, 1)

           # レイヤーにマーカーを追加（時間=2秒）
           layer = ae.app.active_comp.layer(1)
           layer.add_marker_at_time(2.0, markerP)

           print("マーカーを作成しました")

       finally:
           # マーカーハンドルを解放
           ae.sdk.AEGP_DisposeMarker(markerP)

   # 実行
   create_marker_example()

範囲マーカーの作成
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_range_marker():
       """継続時間を持つ範囲マーカーを作成"""
       markerP = ae.sdk.AEGP_NewMarker()

       try:
           # コメントを設定
           ae.sdk.AEGP_SetMarkerString(markerP, 1, "シーン1")

           # 継続時間を3秒に設定
           ae.sdk.AEGP_SetMarkerDuration(markerP, {'value': 3, 'scale': 1})

           # チャプター名を設定
           ae.sdk.AEGP_SetMarkerString(markerP, 2, "第1章: イントロダクション")

           # 保護リージョンとして設定
           ae.sdk.AEGP_SetMarkerFlag(markerP, 2, True)

           # レイヤーに追加
           layer = ae.app.active_comp.layer(1)
           layer.add_marker_at_time(0.0, markerP)

           print("範囲マーカーを作成しました")

       finally:
           ae.sdk.AEGP_DisposeMarker(markerP)

   # 実行
   create_range_marker()

キューポイントパラメータの使用
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_cue_point_marker():
       """キューポイントパラメータ付きマーカーを作成"""
       markerP = ae.sdk.AEGP_NewMarker()
       plugin_id = ae.sdk.AEGP_GetPluginID()

       try:
           # マーカー名を設定
           ae.sdk.AEGP_SetMarkerString(markerP, 5, "play_animation")

           # キューポイントパラメータを追加
           ae.sdk.AEGP_InsertCuePointParam(markerP, 0)
           ae.sdk.AEGP_SetIndCuePointParam(markerP, 0, "type", "action")

           ae.sdk.AEGP_InsertCuePointParam(markerP, 1)
           ae.sdk.AEGP_SetIndCuePointParam(markerP, 1, "target", "character1")

           ae.sdk.AEGP_InsertCuePointParam(markerP, 2)
           ae.sdk.AEGP_SetIndCuePointParam(markerP, 2, "duration", "2.5")

           # パラメータを確認
           param_count = ae.sdk.AEGP_CountCuePointParams(markerP)
           print(f"キューポイントパラメータ数: {param_count}")

           for i in range(param_count):
               param = ae.sdk.AEGP_GetIndCuePointParam(plugin_id, markerP, i)
               print(f"  {param['key']} = {param['value']}")

           # レイヤーに追加
           layer = ae.app.active_comp.layer(1)
           layer.add_marker_at_time(5.0, markerP)

       finally:
           ae.sdk.AEGP_DisposeMarker(markerP)

   # 実行
   create_cue_point_marker()

マーカーの複製と変更
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def duplicate_and_modify_marker():
       """既存のマーカーを複製して変更"""
       # 元のマーカーを作成
       original_markerP = ae.sdk.AEGP_NewMarker()

       try:
           # 元のマーカーを設定
           ae.sdk.AEGP_SetMarkerString(original_markerP, 1, "オリジナル")
           ae.sdk.AEGP_SetMarkerLabel(original_markerP, 1)  # 赤

           # マーカーを複製
           duplicate_markerP = ae.sdk.AEGP_DuplicateMarker(original_markerP)

           try:
               # 複製したマーカーを変更
               ae.sdk.AEGP_SetMarkerString(duplicate_markerP, 1, "コピー")
               ae.sdk.AEGP_SetMarkerLabel(duplicate_markerP, 2)  # 別の色

               # 両方のマーカーをレイヤーに追加
               layer = ae.app.active_comp.layer(1)
               layer.add_marker_at_time(1.0, original_markerP)
               layer.add_marker_at_time(3.0, duplicate_markerP)

               print("マーカーを複製して追加しました")

           finally:
               ae.sdk.AEGP_DisposeMarker(duplicate_markerP)

       finally:
           ae.sdk.AEGP_DisposeMarker(original_markerP)

   # 実行
   duplicate_and_modify_marker()

マーカー情報のダンプ
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_marker_info(markerP):
       """マーカーの全情報を表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       print("=" * 50)
       print("マーカー情報:")
       print("-" * 50)

       # 文字列情報
       strings = {
           1: "コメント",
           2: "チャプター",
           3: "URL",
           4: "フレームターゲット",
           5: "キューポイント名"
       }

       for str_type, name in strings.items():
           value = ae.sdk.AEGP_GetMarkerString(plugin_id, markerP, str_type)
           if value:
               print(f"{name}: {value}")

       # フラグ情報
       is_navigation = ae.sdk.AEGP_GetMarkerFlag(markerP, 1)
       is_protected = ae.sdk.AEGP_GetMarkerFlag(markerP, 2)
       print(f"ナビゲーション: {is_navigation}")
       print(f"保護リージョン: {is_protected}")

       # 継続時間
       duration = ae.sdk.AEGP_GetMarkerDuration(markerP)
       duration_seconds = duration['value'] / duration['scale']
       print(f"継続時間: {duration_seconds}秒")

       # ラベル色
       label = ae.sdk.AEGP_GetMarkerLabel(markerP)
       print(f"ラベル色インデックス: {label}")

       # キューポイントパラメータ
       param_count = ae.sdk.AEGP_CountCuePointParams(markerP)
       if param_count > 0:
           print(f"\nキューポイントパラメータ ({param_count}個):")
           for i in range(param_count):
               param = ae.sdk.AEGP_GetIndCuePointParam(plugin_id, markerP, i)
               print(f"  {i}: {param['key']} = {param['value']}")

       print("=" * 50)

   # 使用例
   markerP = ae.sdk.AEGP_NewMarker()
   try:
       ae.sdk.AEGP_SetMarkerString(markerP, 1, "テストマーカー")
       ae.sdk.AEGP_SetMarkerDuration(markerP, {'value': 2, 'scale': 1})
       dump_marker_info(markerP)
   finally:
       ae.sdk.AEGP_DisposeMarker(markerP)

チャプターマーカーの一括作成
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_chapter_markers():
       """ビデオのチャプターマーカーを一括作成"""
       chapters = [
           {"time": 0.0, "name": "イントロダクション", "duration": 10.0},
           {"time": 10.0, "name": "第1章: 概要", "duration": 20.0},
           {"time": 30.0, "name": "第2章: 詳細", "duration": 25.0},
           {"time": 55.0, "name": "第3章: まとめ", "duration": 15.0},
           {"time": 70.0, "name": "エンディング", "duration": 5.0}
       ]

       layer = ae.app.active_comp.layer(1)

       for chapter in chapters:
           markerP = ae.sdk.AEGP_NewMarker()

           try:
               # チャプター名を設定
               ae.sdk.AEGP_SetMarkerString(markerP, 2, chapter["name"])

               # コメントも設定
               ae.sdk.AEGP_SetMarkerString(markerP, 1, f"チャプター: {chapter['name']}")

               # 継続時間を設定
               ae.sdk.AEGP_SetMarkerDuration(markerP, {
                   'value': int(chapter["duration"] * 30),
                   'scale': 30
               })

               # ナビゲーションマーカーとして設定
               ae.sdk.AEGP_SetMarkerFlag(markerP, 1, True)

               # レイヤーに追加
               layer.add_marker_at_time(chapter["time"], markerP)

               print(f"チャプター追加: {chapter['name']} ({chapter['time']}秒)")

           finally:
               ae.sdk.AEGP_DisposeMarker(markerP)

       print(f"\n合計 {len(chapters)} 個のチャプターマーカーを作成しました")

   # 実行
   create_chapter_markers()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリ管理が必須**

   ``AEGP_NewMarker()`` と ``AEGP_DuplicateMarker()`` で作成したマーカーは、必ず ``AEGP_DisposeMarker()`` で解放してください。
   解放しない場合、メモリリークが発生します。

2. **文字列のエンコーディング**

   マーカー文字列はUTF-16で保存されますが、PyAEでは自動的にUTF-8との変換を行います。
   日本語などのマルチバイト文字も正しく処理されます。

3. **キューポイントパラメータの順序**

   ``AEGP_InsertCuePointParam()`` は指定したインデックスに**挿入**します。
   既存のパラメータは後方にシフトされます。

4. **ラベル色の範囲**

   ``AEGP_SetMarkerLabel()`` には0〜16の値を指定できます。
   範囲外の値を指定するとエラーになります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

try-finallyでメモリ解放を保証
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   markerP = ae.sdk.AEGP_NewMarker()
   try:
       # マーカーを使用
       ae.sdk.AEGP_SetMarkerString(markerP, 1, "My Marker")
   finally:
       # 必ず解放
       ae.sdk.AEGP_DisposeMarker(markerP)

plugin_idの再利用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # plugin_idは1度だけ取得
   plugin_id = ae.sdk.AEGP_GetPluginID()

   # 再利用
   for markerP in markers:
       comment = ae.sdk.AEGP_GetMarkerString(plugin_id, markerP, 1)

キューポイントパラメータの追加パターン
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 常にInsert → Setの順序で行う
   ae.sdk.AEGP_InsertCuePointParam(markerP, 0)
   ae.sdk.AEGP_SetIndCuePointParam(markerP, 0, "key", "value")

空文字列のチェック
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   url = ae.sdk.AEGP_GetMarkerString(plugin_id, markerP, 3)
   if url:
       print(f"URL: {url}")
   else:
       print("URLは設定されていません")

関連項目
--------

- :doc:`AEGP_LayerSuite9` - レイヤーマーカーの操作（``AEGP_GetLayerMarkerByIndex()`` 等）
- :doc:`AEGP_StreamSuite6` - マーカーストリームの操作
- :doc:`index` - 低レベルAPI概要
