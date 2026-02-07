AEGP_StreamSuite6
=================

.. currentmodule:: ae.sdk

AEGP_StreamSuite6は、After Effectsのプロパティストリーム（レイヤー、エフェクト、マスクのプロパティ）を操作するためのSDK APIです。

概要
----

**実装状況**: 37/37関数実装 ✅

AEGP_StreamSuite6は以下の機能を提供します:

- ストリーム参照の取得・管理（レイヤー、エフェクト、マスク）
- ストリーム値の読み書き（位置、回転、不透明度など）
- エクスプレッションの取得・設定
- ストリームプロパティの取得（名前、型、範囲、単位）
- ストリーム階層のトラバース（動的プロパティアクセス）
- ストリームの操作（リセット、削除、並び替え）

基本概念
--------

AEGP_StreamRefH（ストリーム参照ハンドル）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのプロパティストリーム（アニメート可能なパラメータ）を識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - ストリーム参照は、使用後に ``AEGP_DisposeStream()`` で解放する必要があります
   - ストリームハンドルは、そのレイヤー/エフェクト/マスクが存在する間のみ有効です
   - 複数のストリーム参照が同じプロパティを指すことがあります（``AEGP_DuplicateStreamRef()``）

AEGP_StreamType（ストリーム型）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ストリームが保持するデータの型を示します:

.. list-table::
   :header-rows: 1

   * - 定数
     - 説明
   * - ``AEGP_StreamType_NO_DATA``
     - データなし（グループプロパティなど）
   * - ``AEGP_StreamType_TwoD``
     - 2次元ベクトル（例: スケール、アンカーポイント）
   * - ``AEGP_StreamType_TwoD_SPATIAL``
     - 2次元空間ベクトル（例: 位置）
   * - ``AEGP_StreamType_ThreeD``
     - 3次元ベクトル（例: 3Dスケール、回転）
   * - ``AEGP_StreamType_ThreeD_SPATIAL``
     - 3次元空間ベクトル（例: 3D位置）
   * - ``AEGP_StreamType_COLOR``
     - カラー値（RGBA）
   * - ``AEGP_StreamType_MARKER``
     - マーカー
   * - ``AEGP_StreamType_LAYER_ID``
     - レイヤーID
   * - ``AEGP_StreamType_MASK_ID``
     - マスクID
   * - ``AEGP_StreamType_MASK``
     - マスクデータ
   * - ``AEGP_StreamType_TEXT_DOCUMENT``
     - テキストドキュメント

AEGP_StreamValue2（ストリーム値）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ストリームの値を保持する構造体。型に応じて異なるフィールドを使用します:

- ``val.one_d``: 1次元値（不透明度、回転など）
- ``val.two_d``: 2次元ベクトル（位置、スケールなど）
- ``val.three_d``: 3次元ベクトル（3D位置、回転など）
- ``val.color``: カラー値（RGBA）
- ``val.text_documentH``: テキストドキュメントハンドル

.. warning::
   ``AEGP_StreamValue2`` はヒープに割り当てられ、``AEGP_GetNewStreamValue()`` から返されます。
   使用後は **必ず** ``AEGP_DisposeStreamValue()`` で解放してください。

AEGP_StreamGroupingType（グループ化タイプ）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ストリームが子プロパティを持つかどうかを示します:

.. list-table::
   :header-rows: 1

   * - 定数
     - 説明
   * - ``AEGP_StreamGroupingType_NONE``
     - グループなし
   * - ``AEGP_StreamGroupingType_LEAF``
     - リーフノード（子なし）
   * - ``AEGP_StreamGroupingType_NAMED_GROUP``
     - 名前付きグループ（例: Transform）
   * - ``AEGP_StreamGroupingType_INDEXED_GROUP``
     - インデックス付きグループ（例: Effects、Masks）

ストリームの階層構造
~~~~~~~~~~~~~~~~~~~~

After Effectsのプロパティはツリー構造を持ちます:

.. code-block:: text

   Layer (Root)
   ├── Transform
   │   ├── Anchor Point
   │   ├── Position
   │   ├── Scale
   │   ├── Rotation
   │   └── Opacity
   ├── Effects (Indexed Group)
   │   ├── Effect[0] (Named Group)
   │   │   ├── Param[0]
   │   │   ├── Param[1]
   │   │   └── ...
   │   └── Effect[1]
   └── Masks (Indexed Group)
       ├── Mask[0]
       └── Mask[1]

API リファレンス
----------------

ストリーム参照の取得
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewLayerStream(plugin_id: int, layerH: int, which_stream: int) -> int

   レイヤーのストリーム参照を取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param layerH: レイヤーハンドル
   :type layerH: int
   :param which_stream: ストリームの種類（``AEGP_LayerStream`` 定数）
   :type which_stream: int
   :return: ストリーム参照ハンドル
   :rtype: int

   .. note::
      取得したストリーム参照は、使用後に ``AEGP_DisposeStream()`` で解放する必要があります。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      streamH = ae.sdk.AEGP_GetNewLayerStream(
          plugin_id, layerH, ae.sdk.AEGP_LayerStream_OPACITY)

.. function:: AEGP_GetNewEffectStreamByIndex(plugin_id: int, effectH: int, param_index: int) -> int

   エフェクトのパラメータストリームを取得します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param effectH: エフェクトハンドル
   :type effectH: int
   :param param_index: パラメータインデックス（0から始まる）
   :type param_index: int
   :return: ストリーム参照ハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      streamH = ae.sdk.AEGP_GetNewEffectStreamByIndex(plugin_id, effectH, 0)

.. function:: AEGP_GetEffectNumParamStreams(effectH: int) -> int

   エフェクトのパラメータストリーム数を取得します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: パラメータ数
   :rtype: int

   **例**:

   .. code-block:: python

      num_params = ae.sdk.AEGP_GetEffectNumParamStreams(effectH)
      for i in range(num_params):
          streamH = ae.sdk.AEGP_GetNewEffectStreamByIndex(plugin_id, effectH, i)
          # ... ストリーム処理

.. function:: AEGP_GetNewMaskStream(plugin_id: int, maskH: int, which_stream: int) -> int

   マスクのストリーム参照を取得します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param maskH: マスクハンドル
   :type maskH: int
   :param which_stream: ストリームの種類（``AEGP_MaskStream`` 定数）
   :type which_stream: int
   :return: ストリーム参照ハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      streamH = ae.sdk.AEGP_GetNewMaskStream(
          plugin_id, maskH, ae.sdk.AEGP_MaskStream_FEATHER)

ストリーム参照の管理
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_DisposeStream(streamH: int) -> None

   ストリーム参照を解放します。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int

   .. important::
      ``AEGP_GetNewLayerStream()`` などで取得したストリーム参照は、使用後に必ずこの関数で解放してください。

   **例**:

   .. code-block:: python

      streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, stream_type)
      try:
          # ストリーム処理
          pass
      finally:
          ae.sdk.AEGP_DisposeStream(streamH)

.. function:: AEGP_DuplicateStreamRef(plugin_id: int, streamH: int) -> int

   ストリーム参照を複製します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param streamH: 元のストリーム参照ハンドル
   :type streamH: int
   :return: 複製されたストリーム参照ハンドル
   :rtype: int

   .. note::
      複製されたストリーム参照も、使用後に ``AEGP_DisposeStream()`` で解放する必要があります。

   **例**:

   .. code-block:: python

      dup_streamH = ae.sdk.AEGP_DuplicateStreamRef(plugin_id, streamH)

ストリーム情報の取得
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetStreamName(streamH: int, force_english: bool) -> str

   ストリームの名前を取得します。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :param force_english: 英語名を強制する場合は ``True``
   :type force_english: bool
   :return: ストリーム名（UTF-16文字列）
   :rtype: str

   **例**:

   .. code-block:: python

      name = ae.sdk.AEGP_GetStreamName(streamH, False)  # ローカライズ名
      name_en = ae.sdk.AEGP_GetStreamName(streamH, True)  # 英語名

.. function:: AEGP_GetStreamType(streamH: int) -> int

   ストリームのデータ型を取得します。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: ストリーム型（``AEGP_StreamType`` 定数）
   :rtype: int

   **例**:

   .. code-block:: python

      stream_type = ae.sdk.AEGP_GetStreamType(streamH)
      if stream_type == ae.sdk.AEGP_StreamType_TwoD_SPATIAL:
          print("2D空間ストリーム（位置など）")

.. function:: AEGP_GetStreamUnitsText(streamH: int, force_english: bool) -> str

   ストリームの単位テキストを取得します（例: "degrees", "pixels"）。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :param force_english: 英語単位を強制する場合は ``True``
   :type force_english: bool
   :return: 単位テキスト
   :rtype: str

   **例**:

   .. code-block:: python

      units = ae.sdk.AEGP_GetStreamUnitsText(streamH, True)
      print(f"単位: {units}")

.. function:: AEGP_GetStreamProperties(streamH: int) -> dict

   ストリームのプロパティ（フラグ、最小値、最大値）を取得します。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: プロパティの辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``flags``
        - int
        - ストリームフラグ
      * - ``min``
        - float
        - 最小値
      * - ``max``
        - float
        - 最大値

   **例**:

   .. code-block:: python

      props = ae.sdk.AEGP_GetStreamProperties(streamH)
      print(f"範囲: {props['min']} - {props['max']}")

.. function:: AEGP_IsStreamTimevarying(streamH: int) -> bool

   ストリームが時間的に変化するか（キーフレームまたはエクスプレッションを持つか）を確認します。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: 時間的に変化する場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_IsStreamTimevarying(streamH):
          print("このストリームはアニメーション化されています")

.. function:: AEGP_CanVaryOverTime(streamH: int) -> bool

   ストリームが時間的に変化可能か（キーフレームを設定できるか）を確認します。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: 時間的に変化可能な場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_CanVaryOverTime(streamH):
          print("このストリームはアニメーション可能です")

.. function:: AEGP_IsStreamLegal(layerH: int, which_stream: int) -> bool

   レイヤーに対して指定されたストリームが有効か確認します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param which_stream: ストリームの種類（``AEGP_LayerStream`` 定数）
   :type which_stream: int
   :return: ストリームが有効な場合は ``True``
   :rtype: bool

   .. note::
      例えば、2DレイヤーにZ Position（3D）ストリームは無効です。

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_IsStreamLegal(layerH, ae.sdk.AEGP_LayerStream_POSITION_Z):
          print("このレイヤーは3Dです")

ストリーム値の取得・設定
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewStreamValue(plugin_id: int, streamH: int, time_mode: int, time: float, pre_expression: bool) -> tuple[int, int]

   ストリームの値を取得します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :param time_mode: 時間モード（``AEGP_LTimeMode``）
   :type time_mode: int
   :param time: 時間（秒）
   :type time: float
   :param pre_expression: エクスプレッション適用前の値を取得する場合は ``True``
   :type pre_expression: bool
   :return: ``(stream_type, valueP)`` のタプル（stream_typeはストリーム型、valuePは値ポインタ）
   :rtype: tuple[int, int]

   .. warning::
      返されたvaluePは、使用後に ``AEGP_DisposeStreamValue()`` で解放する必要があります。

   **例**:

   .. code-block:: python

      stream_type, valueP = ae.sdk.AEGP_GetNewStreamValue(
          plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False)
      try:
          if stream_type == ae.sdk.AEGP_StreamType_TwoD_SPATIAL:
              x, y = ae.sdk.AEGP_GetStreamValue_Point(valueP)
              print(f"位置: ({x}, {y})")
      finally:
          ae.sdk.AEGP_DisposeStreamValue(valueP)

.. function:: AEGP_DisposeStreamValue(valueP: int) -> None

   ストリーム値を解放します。

   :param valueP: 値ポインタ（``AEGP_GetNewStreamValue()`` から取得）
   :type valueP: int

   .. important::
      ``AEGP_GetNewStreamValue()`` で取得した値ポインタは、使用後に必ずこの関数で解放してください。

.. function:: AEGP_SetStreamValue(plugin_id: int, streamH: int, valueP: int) -> None

   ストリームの値を設定します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :param valueP: 設定する値ポインタ
   :type valueP: int

   .. note::
      valuePは ``AEGP_GetNewStreamValue()`` で取得し、適切に変更したものを渡します。

   **例**:

   .. code-block:: python

      stream_type, valueP = ae.sdk.AEGP_GetNewStreamValue(
          plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False)
      try:
          # 値を変更
          ae.sdk.AEGP_SetStreamValue_Point(valueP, 100.0, 200.0)
          # 設定
          ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, valueP)
      finally:
          ae.sdk.AEGP_DisposeStreamValue(valueP)

.. function:: AEGP_GetLayerStreamValue(layerH: int, which_stream: int, time_mode: int, time: float, pre_expression: bool) -> tuple[int, float]

   レイヤーのストリーム値を簡易的に取得します（ストリーム参照を取得せずに値を取得）。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :param which_stream: ストリームの種類（``AEGP_LayerStream`` 定数）
   :type which_stream: int
   :param time_mode: 時間モード
   :type time_mode: int
   :param time: 時間（秒）
   :type time: float
   :param pre_expression: エクスプレッション適用前の値を取得する場合は ``True``
   :type pre_expression: bool
   :return: ``(stream_type, value)`` のタプル（簡易版、1次元値のみ）
   :rtype: tuple[int, float]

   .. note::
      この関数は簡易版で、1次元値のみを返します。複雑な値を取得する場合は ``AEGP_GetNewStreamValue()`` を使用してください。

ストリーム値のヘルパー関数
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetStreamValue_Double(valueP: int) -> float

   ストリーム値から1次元値（double）を取得します。

   :param valueP: 値ポインタ
   :type valueP: int
   :return: 1次元値
   :rtype: float

.. function:: AEGP_SetStreamValue_Double(valueP: int, val: float) -> None

   ストリーム値に1次元値（double）を設定します。

   :param valueP: 値ポインタ
   :type valueP: int
   :param val: 設定する値
   :type val: float

.. function:: AEGP_GetStreamValue_Point(valueP: int) -> tuple[float, float]

   ストリーム値から2次元ベクトル（Point）を取得します。

   :param valueP: 値ポインタ
   :type valueP: int
   :return: ``(x, y)`` のタプル
   :rtype: tuple[float, float]

.. function:: AEGP_SetStreamValue_Point(valueP: int, x: float, y: float) -> None

   ストリーム値に2次元ベクトル（Point）を設定します。

   :param valueP: 値ポインタ
   :type valueP: int
   :param x: X座標
   :type x: float
   :param y: Y座標
   :type y: float

.. function:: AEGP_GetStreamValue_ThreeD(valueP: int) -> tuple[float, float, float]

   ストリーム値から3次元ベクトル（ThreeD）を取得します。

   :param valueP: 値ポインタ
   :type valueP: int
   :return: ``(x, y, z)`` のタプル
   :rtype: tuple[float, float, float]

.. function:: AEGP_SetStreamValue_ThreeD(valueP: int, x: float, y: float, z: float) -> None

   ストリーム値に3次元ベクトル（ThreeD）を設定します。

   :param valueP: 値ポインタ
   :type valueP: int
   :param x: X座標
   :type x: float
   :param y: Y座標
   :type y: float
   :param z: Z座標
   :type z: float

.. function:: AEGP_GetStreamValue_Color(valueP: int) -> tuple[float, float, float, float]

   ストリーム値からカラー値（RGBA）を取得します。

   :param valueP: 値ポインタ
   :type valueP: int
   :return: ``(r, g, b, a)`` のタプル（各値は0.0～1.0の範囲）
   :rtype: tuple[float, float, float, float]

.. function:: AEGP_SetStreamValue_Color(valueP: int, r: float, g: float, b: float, a: float) -> None

   ストリーム値にカラー値（RGBA）を設定します。

   :param valueP: 値ポインタ
   :type valueP: int
   :param r: 赤成分（0.0～1.0）
   :type r: float
   :param g: 緑成分（0.0～1.0）
   :type g: float
   :param b: 青成分（0.0～1.0）
   :type b: float
   :param a: アルファ成分（0.0～1.0）
   :type a: float

.. function:: AEGP_GetStreamValue_TextDocument(valueP: int) -> int

   ストリーム値からテキストドキュメントハンドルを取得します。

   :param valueP: 値ポインタ
   :type valueP: int
   :return: テキストドキュメントハンドル
   :rtype: int

.. function:: AEGP_SetStreamValue_TextDocument(valueP: int, text_documentH: int) -> None

   ストリーム値にテキストドキュメントハンドルを設定します。

   :param valueP: 値ポインタ
   :type valueP: int
   :param text_documentH: テキストドキュメントハンドル
   :type text_documentH: int

エクスプレッションの操作
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetExpressionState(plugin_id: int, streamH: int) -> bool

   ストリームのエクスプレッション有効状態を取得します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: エクスプレッションが有効な場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_GetExpressionState(plugin_id, streamH):
          expr = ae.sdk.AEGP_GetExpression(plugin_id, streamH)
          print(f"エクスプレッション: {expr}")

.. function:: AEGP_SetExpressionState(plugin_id: int, streamH: int, enabled: bool) -> None

   ストリームのエクスプレッション有効状態を設定します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :param enabled: エクスプレッションを有効にする場合は ``True``
   :type enabled: bool

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetExpressionState(plugin_id, streamH, True)

.. function:: AEGP_GetExpression(plugin_id: int, streamH: int) -> str

   ストリームのエクスプレッションを取得します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: エクスプレッション文字列（UTF-16）
   :rtype: str

   **例**:

   .. code-block:: python

      expr = ae.sdk.AEGP_GetExpression(plugin_id, streamH)
      print(f"エクスプレッション: {expr}")

.. function:: AEGP_SetExpression(plugin_id: int, streamH: int, expression: str) -> None

   ストリームのエクスプレッションを設定します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :param expression: エクスプレッション文字列
   :type expression: str

   .. note::
      エクスプレッションを設定すると、自動的にエクスプレッション有効状態が ``True`` になります。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_SetExpression(plugin_id, streamH, "wiggle(5, 20)")

キーフレーム補間
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetValidInterpolations(streamH: int) -> int

   ストリームでサポートされるキーフレーム補間の種類を取得します。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: 補間マスク（``AEGP_KeyInterpolationMask`` フラグ）
   :rtype: int

   **例**:

   .. code-block:: python

      mask = ae.sdk.AEGP_GetValidInterpolations(streamH)
      print(f"サポートされる補間: {mask}")

動的ストリームアクセス（階層トラバース）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewStreamRefForLayer(layerH: int) -> int

   レイヤーのプロパティツリーのルートストリームを取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: ルートストリーム参照ハンドル
   :rtype: int

   .. note::
      DynamicStreamSuite4の機能です。プロパティツリーを動的に走査する際に使用します。

   **例**:

   .. code-block:: python

      root_streamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)

.. function:: AEGP_GetNewStreamRefForMask(maskH: int) -> int

   マスクのプロパティツリーのルートストリームを取得します。

   :param maskH: マスクハンドル
   :type maskH: int
   :return: ルートストリーム参照ハンドル
   :rtype: int

.. function:: AEGP_GetNewParentStreamRef(streamH: int) -> int

   ストリームの親ストリームを取得します（プロパティツリーを上に移動）。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: 親ストリーム参照ハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      parent_streamH = ae.sdk.AEGP_GetNewParentStreamRef(streamH)

.. function:: AEGP_GetStreamGroupingType(streamH: int) -> int

   ストリームのグループ化タイプを取得します（リーフ、名前付きグループ、インデックス付きグループ）。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: グループ化タイプ（``AEGP_StreamGroupingType`` 定数）
   :rtype: int

   **例**:

   .. code-block:: python

      group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)
      if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
          print("リーフノード（値を持つ）")
      elif group_type == ae.sdk.AEGP_StreamGroupingType_NAMED_GROUP:
          print("名前付きグループ（子プロパティを持つ）")

.. function:: AEGP_GetNumStreamsInGroup(streamH: int) -> int

   グループストリームの子ストリーム数を取得します。

   :param streamH: グループストリーム参照ハンドル
   :type streamH: int
   :return: 子ストリーム数
   :rtype: int

   **例**:

   .. code-block:: python

      num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
      for i in range(num_children):
          child_streamH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
          # ... 子ストリーム処理

.. function:: AEGP_GetNewStreamRefByIndex(parentStreamH: int, index: int) -> int

   グループストリームの子ストリームをインデックスで取得します。

   :param parentStreamH: 親グループストリーム参照ハンドル
   :type parentStreamH: int
   :param index: 子ストリームのインデックス（0から始まる）
   :type index: int
   :return: 子ストリーム参照ハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      child_streamH = ae.sdk.AEGP_GetNewStreamRefByIndex(parent_streamH, 0)

.. function:: AEGP_GetUniqueStreamID(streamH: int) -> int

   ストリームの一意なIDを取得します（AE 24.0+）。

   :param streamH: ストリーム参照ハンドル
   :type streamH: int
   :return: ストリームID
   :rtype: int

   .. note::
      この関数はAEGP_StreamSuite6（AE 24.0以降）でのみ利用可能です。

使用例
------

レイヤープロパティの取得と設定
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def set_layer_opacity(layerH, opacity_percent):
       """レイヤーの不透明度を設定"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # 不透明度ストリームを取得
       streamH = ae.sdk.AEGP_GetNewLayerStream(
           plugin_id, layerH, ae.sdk.AEGP_LayerStream_OPACITY)

       try:
           # 現在の値を取得
           stream_type, valueP = ae.sdk.AEGP_GetNewStreamValue(
               plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False)

           try:
               # 値を変更（0-100のパーセント値）
               ae.sdk.AEGP_SetStreamValue_Double(valueP, opacity_percent)

               # 設定
               ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, valueP)
               print(f"不透明度を{opacity_percent}%に設定しました")

           finally:
               ae.sdk.AEGP_DisposeStreamValue(valueP)

       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

   # 使用例
   comp = ae.ActiveComp
   layer = comp.layers[0]
   set_layer_opacity(layer.handle, 50.0)

位置プロパティの操作
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_layer_position(layerH, time_seconds):
       """指定時刻のレイヤー位置を取得"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # 位置ストリームを取得
       streamH = ae.sdk.AEGP_GetNewLayerStream(
           plugin_id, layerH, ae.sdk.AEGP_LayerStream_POSITION)

       try:
           # 値を取得
           stream_type, valueP = ae.sdk.AEGP_GetNewStreamValue(
               plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime,
               time_seconds, False)

           try:
               # 2D位置を取得
               if stream_type == ae.sdk.AEGP_StreamType_TwoD_SPATIAL:
                   x, y = ae.sdk.AEGP_GetStreamValue_Point(valueP)
                   return (x, y)
               # 3D位置を取得
               elif stream_type == ae.sdk.AEGP_StreamType_ThreeD_SPATIAL:
                   x, y, z = ae.sdk.AEGP_GetStreamValue_ThreeD(valueP)
                   return (x, y, z)

           finally:
               ae.sdk.AEGP_DisposeStreamValue(valueP)

       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

   def set_layer_position(layerH, x, y, z=None):
       """レイヤーの位置を設定"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       streamH = ae.sdk.AEGP_GetNewLayerStream(
           plugin_id, layerH, ae.sdk.AEGP_LayerStream_POSITION)

       try:
           stream_type, valueP = ae.sdk.AEGP_GetNewStreamValue(
               plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime, 0.0, False)

           try:
               if z is None:
                   # 2D位置
                   ae.sdk.AEGP_SetStreamValue_Point(valueP, x, y)
               else:
                   # 3D位置
                   ae.sdk.AEGP_SetStreamValue_ThreeD(valueP, x, y, z)

               ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, valueP)

           finally:
               ae.sdk.AEGP_DisposeStreamValue(valueP)

       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

   # 使用例
   comp = ae.ActiveComp
   layer = comp.layers[0]

   # 現在位置を取得
   pos = get_layer_position(layer.handle, 0.0)
   print(f"現在位置: {pos}")

   # 新しい位置を設定
   set_layer_position(layer.handle, 960, 540)  # 画面中央

エクスプレッションの操作
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def add_wiggle_expression(layerH, frequency=5, amplitude=20):
       """レイヤーの位置にwiggleエクスプレッションを追加"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       streamH = ae.sdk.AEGP_GetNewLayerStream(
           plugin_id, layerH, ae.sdk.AEGP_LayerStream_POSITION)

       try:
           # エクスプレッションを設定
           expression = f"wiggle({frequency}, {amplitude})"
           ae.sdk.AEGP_SetExpression(plugin_id, streamH, expression)

           # 有効状態を確認
           if ae.sdk.AEGP_GetExpressionState(plugin_id, streamH):
               print(f"エクスプレッションを追加: {expression}")

       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

   def list_expressions(layerH):
       """レイヤーのすべてのエクスプレッションをリスト表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # レイヤープロパティの一覧
       layer_streams = [
           ae.sdk.AEGP_LayerStream_ANCHORPOINT,
           ae.sdk.AEGP_LayerStream_POSITION,
           ae.sdk.AEGP_LayerStream_SCALE,
           ae.sdk.AEGP_LayerStream_ROTATION,
           ae.sdk.AEGP_LayerStream_OPACITY,
       ]

       stream_names = [
           "Anchor Point", "Position", "Scale", "Rotation", "Opacity"
       ]

       print("エクスプレッション一覧:")

       for stream_type, stream_name in zip(layer_streams, stream_names):
           try:
               streamH = ae.sdk.AEGP_GetNewLayerStream(
                   plugin_id, layerH, stream_type)

               try:
                   if ae.sdk.AEGP_GetExpressionState(plugin_id, streamH):
                       expr = ae.sdk.AEGP_GetExpression(plugin_id, streamH)
                       print(f"  {stream_name}: {expr}")

               finally:
                   ae.sdk.AEGP_DisposeStream(streamH)

           except Exception as e:
               # このレイヤーで無効なストリーム（例: 2DレイヤーのZ位置）
               pass

   # 使用例
   comp = ae.ActiveComp
   layer = comp.layers[0]

   add_wiggle_expression(layer.handle, 5, 20)
   list_expressions(layer.handle)

プロパティツリーの動的走査
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def traverse_property_tree(streamH, indent=0):
       """プロパティツリーを再帰的に走査して表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # ストリーム名を取得
       name = ae.sdk.AEGP_GetStreamName(streamH, True)

       # グループタイプを取得
       group_type = ae.sdk.AEGP_GetStreamGroupingType(streamH)

       # インデント表示
       prefix = "  " * indent

       if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
           # リーフノード（値を持つ）
           stream_type = ae.sdk.AEGP_GetStreamType(streamH)
           units = ae.sdk.AEGP_GetStreamUnitsText(streamH, True)
           print(f"{prefix}├─ {name} (型: {stream_type}, 単位: {units})")

       elif group_type in [ae.sdk.AEGP_StreamGroupingType_NAMED_GROUP,
                           ae.sdk.AEGP_StreamGroupingType_INDEXED_GROUP]:
           # グループノード
           print(f"{prefix}├─ {name} [グループ]")

           # 子ストリームを走査
           num_children = ae.sdk.AEGP_GetNumStreamsInGroup(streamH)
           for i in range(num_children):
               child_streamH = ae.sdk.AEGP_GetNewStreamRefByIndex(streamH, i)
               try:
                   traverse_property_tree(child_streamH, indent + 1)
               finally:
                   ae.sdk.AEGP_DisposeStream(child_streamH)

   def dump_layer_properties(layerH):
       """レイヤーのすべてのプロパティをツリー表示"""
       root_streamH = ae.sdk.AEGP_GetNewStreamRefForLayer(layerH)
       try:
           print("レイヤープロパティツリー:")
           traverse_property_tree(root_streamH)
       finally:
           ae.sdk.AEGP_DisposeStream(root_streamH)

   # 使用例
   comp = ae.ActiveComp
   layer = comp.layers[0]
   dump_layer_properties(layer.handle)

エフェクトパラメータの操作
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def list_effect_parameters(effectH):
       """エフェクトのすべてのパラメータをリスト表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # パラメータ数を取得
       num_params = ae.sdk.AEGP_GetEffectNumParamStreams(effectH)
       print(f"パラメータ数: {num_params}")

       for i in range(num_params):
           streamH = ae.sdk.AEGP_GetNewEffectStreamByIndex(
               plugin_id, effectH, i)

           try:
               # パラメータ名と型を取得
               name = ae.sdk.AEGP_GetStreamName(streamH, True)
               stream_type = ae.sdk.AEGP_GetStreamType(streamH)

               # 現在値を取得
               stream_type, valueP = ae.sdk.AEGP_GetNewStreamValue(
                   plugin_id, streamH, ae.sdk.AEGP_LTimeMode_CompTime,
                   0.0, False)

               try:
                   if stream_type == ae.sdk.AEGP_StreamType_TwoD_SPATIAL:
                       x, y = ae.sdk.AEGP_GetStreamValue_Point(valueP)
                       print(f"  {i}: {name} = ({x}, {y})")
                   else:
                       val = ae.sdk.AEGP_GetStreamValue_Double(valueP)
                       print(f"  {i}: {name} = {val}")

               finally:
                   ae.sdk.AEGP_DisposeStreamValue(valueP)

           finally:
               ae.sdk.AEGP_DisposeStream(streamH)

   # 使用例
   comp = ae.ActiveComp
   layer = comp.layers[0]

   # レイヤーの最初のエフェクトを取得
   num_effects = ae.sdk.AEGP_GetLayerNumEffects(layer.handle)
   if num_effects > 0:
       effectH = ae.sdk.AEGP_GetLayerEffectByIndex(
           ae.sdk.AEGP_GetPluginID(), layer.handle, 0)
       list_effect_parameters(effectH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリ管理が必須**

   - ``AEGP_GetNewStreamValue()`` で取得した値ポインタは、必ず ``AEGP_DisposeStreamValue()`` で解放
   - ``AEGP_GetNewLayerStream()`` などで取得したストリーム参照は、必ず ``AEGP_DisposeStream()`` で解放
   - リソースリークを防ぐため、try-finally構文を使用

2. **ストリーム型に応じた処理**

   ``AEGP_GetStreamType()`` で型を確認してから、適切なヘルパー関数を使用してください。

3. **エクスプレッションとキーフレーム**

   ``pre_expression`` パラメータで、エクスプレッション適用前/後の値を取得できます:

   - ``pre_expression=True``: キーフレーム値（エクスプレッション適用前）
   - ``pre_expression=False``: 最終値（エクスプレッション適用後）

4. **3Dレイヤーとストリームの有効性**

   2Dレイヤーには3D関連のストリーム（``AEGP_LayerStream_POSITION_Z`` など）は無効です。
   ``AEGP_IsStreamLegal()`` で有効性を確認してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

リソース管理
^^^^^^^^^^^^

.. code-block:: python

   # ✅ 推奨: try-finallyで確実に解放
   streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, stream_type)
   try:
       # ストリーム処理
       pass
   finally:
       ae.sdk.AEGP_DisposeStream(streamH)

ストリーム型の確認
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # ✅ 推奨: 型を確認してから処理
   stream_type, valueP = ae.sdk.AEGP_GetNewStreamValue(...)
   try:
       if stream_type == ae.sdk.AEGP_StreamType_TwoD_SPATIAL:
           x, y = ae.sdk.AEGP_GetStreamValue_Point(valueP)
       elif stream_type == ae.sdk.AEGP_StreamType_COLOR:
           r, g, b, a = ae.sdk.AEGP_GetStreamValue_Color(valueP)
   finally:
       ae.sdk.AEGP_DisposeStreamValue(valueP)

エクスプレッションの確認
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # ✅ 推奨: エクスプレッション状態を確認してから取得
   if ae.sdk.AEGP_GetExpressionState(plugin_id, streamH):
       expr = ae.sdk.AEGP_GetExpression(plugin_id, streamH)

ストリームの有効性確認
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # ✅ 推奨: ストリームの有効性を確認してから取得
   if ae.sdk.AEGP_IsStreamLegal(layerH, ae.sdk.AEGP_LayerStream_POSITION_Z):
       streamH = ae.sdk.AEGP_GetNewLayerStream(
           plugin_id, layerH, ae.sdk.AEGP_LayerStream_POSITION_Z)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       streamH = ae.sdk.AEGP_GetNewLayerStream(plugin_id, layerH, stream_type)
   except Exception as e:
       print(f"ストリーム取得失敗: {e}")
       return

関連項目
--------

- :doc:`AEGP_KeyframeSuite5` - キーフレーム操作
- :doc:`AEGP_LayerSuite9` - レイヤー管理
- :doc:`AEGP_EffectSuite5` - エフェクト管理
- :doc:`AEGP_MaskSuite6` - マスク管理
- :doc:`index` - 低レベルAPI概要
