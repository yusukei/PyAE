AEGP_KeyframeSuite5
===================

.. currentmodule:: ae.sdk

AEGP_KeyframeSuite5は、After Effectsのキーフレーム操作を行うためのSDK APIです。

概要
----

**実装状況**: 22/22関数実装 ✅

AEGP_KeyframeSuite5は以下の機能を提供します:

- キーフレームの追加、削除、時間の取得
- キーフレームの値の取得と設定
- 補間タイプ（Linear、Bezier、Hold）の制御
- テンポラルイージング（速度、影響度）の制御
- 空間タンジェント（モーションパス）の制御
- キーフレームフラグ（連続性、Auto Bezier、Rovingなど）の操作
- バッチ操作による複数キーフレームの効率的な追加
- キーフレームラベル色の取得・設定（AE 22.5+）

基本概念
--------

キーフレームインデックス
~~~~~~~~~~~~~~~~~~~~~~~~

キーフレームは、ストリーム内で **0から始まるインデックス** で識別されます。

.. important::
   - キーフレームインデックスは時間順に並んでいます（0 = 最初のキーフレーム）
   - キーフレームを追加・削除すると、後続のキーフレームのインデックスが変わります
   - 時間でキーフレームを特定したい場合は、``AEGP_GetKeyframeTime()`` を使用してください

補間タイプ（Interpolation Type）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

キーフレーム間の補間方法を制御します。各キーフレームは **In補間** と **Out補間** を持ちます。

.. list-table:: 補間タイプ
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_KeyInterp_NONE``
     - 0
     - 補間なし
   * - ``AEGP_KeyInterp_LINEAR``
     - 1
     - リニア補間（直線）
   * - ``AEGP_KeyInterp_BEZIER``
     - 2
     - ベジェ補間（曲線）
   * - ``AEGP_KeyInterp_HOLD``
     - 3
     - ホールド補間（値を保持）

**補間の向き**:

- **In補間**: 前のキーフレームからこのキーフレームへの補間
- **Out補間**: このキーフレームから次のキーフレームへの補間

テンポラルイージング（Temporal Easing）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

時間軸での加速・減速を制御します。``AEGP_KeyframeEase`` 構造体で表現されます。

.. list-table:: AEGP_KeyframeEase
   :header-rows: 1

   * - フィールド
     - 型
     - 説明
   * - ``speedF``
     - float
     - イージング速度（単位/秒）
   * - ``influenceF``
     - float
     - 影響範囲（0.1 ～ 100.0%）

**イージングの向き**:

- **In Ease**: キーフレームへの入り方（到達速度）
- **Out Ease**: キーフレームからの出方（出発速度）

空間タンジェント（Spatial Tangents）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

位置プロパティのモーションパスを制御します。``AEGP_StreamValue2`` 構造体で表現されます。

.. important::
   - 空間タンジェントは **位置プロパティ（Position）のみ** に適用されます
   - 2次元ポジションは2D値、3次元ポジションは3D値としてタンジェントを持ちます
   - タンジェントは **相対座標**（キーフレーム位置からの相対ベクトル）です

**タンジェントの向き**:

- **In Tangent**: 前のキーフレームからこのキーフレームへの曲線の向き
- **Out Tangent**: このキーフレームから次のキーフレームへの曲線の向き

キーフレームフラグ（Keyframe Flags）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

キーフレームの動作を制御するフラグです。ビットフラグとして組み合わせ可能です。

.. list-table:: キーフレームフラグ
   :header-rows: 1

   * - 定数
     - 説明
   * - ``AEGP_KeyframeFlag_NONE``
     - フラグなし
   * - ``AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS``
     - テンポラル連続性（速度が連続）
   * - ``AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER``
     - テンポラルAuto Bezier（自動ベジェ調整）
   * - ``AEGP_KeyframeFlag_SPATIAL_CONTINUOUS``
     - 空間連続性（モーションパスが滑らか）
   * - ``AEGP_KeyframeFlag_SPATIAL_AUTOBEZIER``
     - 空間Auto Bezier（モーションパスの自動調整）
   * - ``AEGP_KeyframeFlag_ROVING``
     - Roving（時間を自動調整して等速度に）

時間モード（Time Mode）
~~~~~~~~~~~~~~~~~~~~~~~

時間の参照基準を指定します。

.. list-table:: AEGP_LTimeMode
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_LTimeMode_LayerTime``
     - 0
     - レイヤー時間（レイヤーのIn Pointが0秒）
   * - ``AEGP_LTimeMode_CompTime``
     - 1
     - コンポジション時間（コンプの開始が0秒）

ストリーム次元（Stream Dimensionality）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

プロパティが持つ値の次元数です。

- **値の次元** (``AEGP_GetStreamValueDimensionality``): プロパティが持つ値の次元（例: Position = 2 or 3, Rotation = 1）
- **テンポラル次元** (``AEGP_GetStreamTemporalDimensionality``): キーフレーム可能な次元数（通常は値の次元と同じ）

API リファレンス
----------------

キーフレーム数と時間
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetStreamNumKfs(streamH: int) -> int

   ストリーム内のキーフレーム数を取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :return: キーフレーム数
   :rtype: int

   **例**:

   .. code-block:: python

      num_kfs = ae.sdk.AEGP_GetStreamNumKfs(streamH)
      print(f"キーフレーム数: {num_kfs}")

.. function:: AEGP_GetKeyframeTime(streamH: int, keyIndex: int, time_mode: int) -> float

   キーフレームの時間を取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス（0から始まる）
   :type keyIndex: int
   :param time_mode: 時間モード（``AEGP_LTimeMode_LayerTime`` or ``AEGP_LTimeMode_CompTime``）
   :type time_mode: int
   :return: 時間（秒）
   :rtype: float

   **例**:

   .. code-block:: python

      time = ae.sdk.AEGP_GetKeyframeTime(streamH, 0, ae.sdk.AEGP_LTimeMode_CompTime)
      print(f"最初のキーフレームの時間: {time}秒")

キーフレームの追加と削除
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_InsertKeyframe(streamH: int, time_mode: int, time: float) -> int

   キーフレームを挿入します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param time_mode: 時間モード
   :type time_mode: int
   :param time: 挿入する時間（秒）
   :type time: float
   :return: 挿入されたキーフレームのインデックス
   :rtype: int

   .. note::
      既に同じ時間にキーフレームが存在する場合、そのインデックスが返されます。

   **例**:

   .. code-block:: python

      # 1秒の位置にキーフレームを挿入
      key_idx = ae.sdk.AEGP_InsertKeyframe(streamH, ae.sdk.AEGP_LTimeMode_CompTime, 1.0)
      print(f"挿入されたキーフレームのインデックス: {key_idx}")

.. function:: AEGP_DeleteKeyframe(streamH: int, keyIndex: int) -> None

   キーフレームを削除します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: 削除するキーフレームのインデックス
   :type keyIndex: int

   **例**:

   .. code-block:: python

      # 最初のキーフレームを削除
      ae.sdk.AEGP_DeleteKeyframe(streamH, 0)

キーフレームの値
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewKeyframeValue(plugin_id: int, streamH: int, keyIndex: int) -> int

   キーフレームの値を取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :return: ``AEGP_StreamValue2`` 構造体へのポインタ（``int``）
   :rtype: int

   .. important::
      - 戻り値のポインタは **ヒープ割り当て** されています
      - 使用後は ``ae.sdk.AEGP_DisposeStreamValue()`` で解放する必要があります
      - メモリリークを防ぐため、必ず解放してください

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      valueP = ae.sdk.AEGP_GetNewKeyframeValue(plugin_id, streamH, 0)
      try:
          # 値を使用
          value = ae.sdk.AEGP_GetStreamValue(valueP)
      finally:
          # 必ず解放
          ae.sdk.AEGP_DisposeStreamValue(valueP)

.. function:: AEGP_SetKeyframeValue(streamH: int, keyIndex: int, valueP: int) -> None

   キーフレームの値を設定します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :param valueP: ``AEGP_StreamValue2`` 構造体へのポインタ
   :type valueP: int

   **例**:

   .. code-block:: python

      # 値を取得して変更
      valueP = ae.sdk.AEGP_GetNewKeyframeValue(plugin_id, streamH, 0)
      try:
          # 値を変更（StreamValueヘルパー使用）
          ae.sdk.AEGP_SetStreamValueOneDVal(valueP, 50.0)
          # 変更をキーフレームに適用
          ae.sdk.AEGP_SetKeyframeValue(streamH, 0, valueP)
      finally:
          ae.sdk.AEGP_DisposeStreamValue(valueP)

ストリーム次元
~~~~~~~~~~~~~~

.. function:: AEGP_GetStreamValueDimensionality(streamH: int) -> int

   ストリームの値の次元数を取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :return: 次元数（1 = 1D、2 = 2D、3 = 3D、4 = Color）
   :rtype: int

   **例**:

   .. code-block:: python

      dim = ae.sdk.AEGP_GetStreamValueDimensionality(streamH)
      # Position: 2 or 3, Rotation: 1, Scale: 2 or 3

.. function:: AEGP_GetStreamTemporalDimensionality(streamH: int) -> int

   ストリームのテンポラル次元数を取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :return: テンポラル次元数
   :rtype: int

   .. note::
      通常、値の次元と同じですが、一部のプロパティは異なる場合があります。

補間タイプ
~~~~~~~~~~

.. function:: AEGP_GetKeyframeInterpolation(streamH: int, keyIndex: int) -> tuple

   キーフレームの補間タイプを取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :return: (in_interp, out_interp) - In補間とOut補間のタプル
   :rtype: tuple[int, int]

   **補間タイプ**:

   - ``ae.sdk.AEGP_KeyInterp_NONE`` (0) - なし
   - ``ae.sdk.AEGP_KeyInterp_LINEAR`` (1) - リニア
   - ``ae.sdk.AEGP_KeyInterp_BEZIER`` (2) - ベジェ
   - ``ae.sdk.AEGP_KeyInterp_HOLD`` (3) - ホールド

   **例**:

   .. code-block:: python

      in_interp, out_interp = ae.sdk.AEGP_GetKeyframeInterpolation(streamH, 0)
      if out_interp == ae.sdk.AEGP_KeyInterp_LINEAR:
          print("Out補間はリニアです")

.. function:: AEGP_SetKeyframeInterpolation(streamH: int, keyIndex: int, in_interp: int, out_interp: int) -> None

   キーフレームの補間タイプを設定します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :param in_interp: In補間タイプ
   :type in_interp: int
   :param out_interp: Out補間タイプ
   :type out_interp: int

   **例**:

   .. code-block:: python

      # ベジェ補間に設定
      ae.sdk.AEGP_SetKeyframeInterpolation(
          streamH, 0,
          ae.sdk.AEGP_KeyInterp_BEZIER,
          ae.sdk.AEGP_KeyInterp_BEZIER
      )

テンポラルイージング
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetKeyframeTemporalEase(streamH: int, keyIndex: int, dimension: int) -> tuple

   キーフレームのテンポラルイージングを取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :param dimension: 次元（0から始まる）
   :type dimension: int
   :return: ((in_speed, in_influence), (out_speed, out_influence))
   :rtype: tuple[tuple[float, float], tuple[float, float]]

   **例**:

   .. code-block:: python

      (in_speed, in_influence), (out_speed, out_influence) = \
          ae.sdk.AEGP_GetKeyframeTemporalEase(streamH, 0, 0)
      print(f"In イージング速度: {in_speed}, 影響度: {in_influence}")

.. function:: AEGP_SetKeyframeTemporalEase(streamH: int, keyIndex: int, dimension: int, in_speed: float, in_influence: float, out_speed: float, out_influence: float) -> None

   キーフレームのテンポラルイージングを設定します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :param dimension: 次元（0から始まる）
   :type dimension: int
   :param in_speed: In速度（単位/秒）
   :type in_speed: float
   :param in_influence: In影響度（0.1 ～ 100.0%）
   :type in_influence: float
   :param out_speed: Out速度（単位/秒）
   :type out_speed: float
   :param out_influence: Out影響度（0.1 ～ 100.0%）
   :type out_influence: float

   .. note::
      - ``speed``: キーフレームでの速度（単位/秒）
      - ``influence``: イージングの影響範囲（0.1 ～ 100.0%、通常は33.33%）

   **例**:

   .. code-block:: python

      # Easy Ease（イージーイーズ）を設定
      ae.sdk.AEGP_SetKeyframeTemporalEase(
          streamH, 0, 0,
          in_speed=0.0,    # 速度0（減速して到達）
          in_influence=33.33,
          out_speed=0.0,   # 速度0（ゆっくり出発）
          out_influence=33.33
      )

空間タンジェント
~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewKeyframeSpatialTangents(plugin_id: int, streamH: int, keyIndex: int) -> tuple

   キーフレームの空間タンジェントを取得します（位置プロパティのみ）。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :return: (in_tanP, out_tanP) - InタンジェントとOutタンジェントのポインタ
   :rtype: tuple[int, int]

   .. important::
      - 戻り値のポインタはヒープ割り当てされています
      - 使用後は両方とも ``AEGP_DisposeStreamValue()`` で解放する必要があります
      - **位置プロパティ（Position）でのみ使用可能**

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      in_tanP, out_tanP = ae.sdk.AEGP_GetNewKeyframeSpatialTangents(
          plugin_id, streamH, 0
      )
      try:
          # タンジェント値を使用
          in_tan = ae.sdk.AEGP_GetStreamValue(in_tanP)
          out_tan = ae.sdk.AEGP_GetStreamValue(out_tanP)
      finally:
          ae.sdk.AEGP_DisposeStreamValue(in_tanP)
          ae.sdk.AEGP_DisposeStreamValue(out_tanP)

.. function:: AEGP_SetKeyframeSpatialTangents(streamH: int, keyIndex: int, in_tanP: int, out_tanP: int) -> None

   キーフレームの空間タンジェントを設定します（位置プロパティのみ）。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :param in_tanP: Inタンジェント（``AEGP_StreamValue2`` ポインタ）
   :type in_tanP: int
   :param out_tanP: Outタンジェント（``AEGP_StreamValue2`` ポインタ）
   :type out_tanP: int

   **例**:

   .. code-block:: python

      # タンジェントを取得して変更
      in_tanP, out_tanP = ae.sdk.AEGP_GetNewKeyframeSpatialTangents(
          plugin_id, streamH, 0
      )
      try:
          # Outタンジェントを変更
          ae.sdk.AEGP_SetStreamValueTwoDVal(out_tanP, 100.0, 0.0)
          # 変更を適用
          ae.sdk.AEGP_SetKeyframeSpatialTangents(streamH, 0, in_tanP, out_tanP)
      finally:
          ae.sdk.AEGP_DisposeStreamValue(in_tanP)
          ae.sdk.AEGP_DisposeStreamValue(out_tanP)

キーフレームフラグ
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetKeyframeFlags(streamH: int, keyIndex: int) -> int

   キーフレームのフラグを取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :return: キーフレームフラグ（ビットフラグの組み合わせ）
   :rtype: int

   **フラグ**:

   - ``AEGP_KeyframeFlag_NONE`` - なし
   - ``AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS`` - テンポラル連続性
   - ``AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER`` - テンポラルAuto Bezier
   - ``AEGP_KeyframeFlag_SPATIAL_CONTINUOUS`` - 空間連続性
   - ``AEGP_KeyframeFlag_SPATIAL_AUTOBEZIER`` - 空間Auto Bezier
   - ``AEGP_KeyframeFlag_ROVING`` - Roving

   **例**:

   .. code-block:: python

      flags = ae.sdk.AEGP_GetKeyframeFlags(streamH, 0)
      if flags & ae.sdk.AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS:
          print("テンポラル連続性が有効です")

.. function:: AEGP_SetKeyframeFlag(streamH: int, keyIndex: int, flag: int, value: bool) -> None

   キーフレームの特定のフラグを設定します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :param flag: 設定するフラグ（単一フラグ）
   :type flag: int
   :param value: フラグの値（True = 有効、False = 無効）
   :type value: bool

   **例**:

   .. code-block:: python

      # Auto Bezierを有効化
      ae.sdk.AEGP_SetKeyframeFlag(
          streamH, 0,
          ae.sdk.AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER,
          True
      )

バッチキーフレーム操作
~~~~~~~~~~~~~~~~~~~~~~

複数のキーフレームを効率的に追加するためのAPI群です。

.. function:: AEGP_StartAddKeyframes(streamH: int) -> int

   バッチキーフレーム追加セッションを開始します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :return: 追加情報ハンドル（``AEGP_AddKeyframesInfoH``）
   :rtype: int

   **例**:

   .. code-block:: python

      akH = ae.sdk.AEGP_StartAddKeyframes(streamH)

.. function:: AEGP_AddKeyframes(akH: int, time_mode: int, time: float) -> int

   バッチにキーフレームを追加します。

   :param akH: 追加情報ハンドル
   :type akH: int
   :param time_mode: 時間モード
   :type time_mode: int
   :param time: 時間（秒）
   :type time: float
   :return: キーフレームインデックス（バッチ内）
   :rtype: int

   **例**:

   .. code-block:: python

      key_idx = ae.sdk.AEGP_AddKeyframes(akH, ae.sdk.AEGP_LTimeMode_CompTime, 1.0)

.. function:: AEGP_SetAddKeyframe(akH: int, keyIndex: int, valueP: int) -> None

   バッチ内のキーフレームの値を設定します。

   :param akH: 追加情報ハンドル
   :type akH: int
   :param keyIndex: バッチ内のキーフレームインデックス
   :type keyIndex: int
   :param valueP: ``AEGP_StreamValue2`` 構造体へのポインタ
   :type valueP: int

   **例**:

   .. code-block:: python

      # 値を作成
      valueP = ae.sdk.AEGP_NewStreamValue(plugin_id, streamH)
      ae.sdk.AEGP_SetStreamValueOneDVal(valueP, 100.0)
      # バッチに設定
      ae.sdk.AEGP_SetAddKeyframe(akH, key_idx, valueP)

.. function:: AEGP_EndAddKeyframes(add: bool, akH: int) -> None

   バッチキーフレーム追加セッションを終了します。

   :param add: キーフレームを実際に追加するか（False = キャンセル）
   :type add: bool
   :param akH: 追加情報ハンドル
   :type akH: int

   .. note::
      ``add=False`` を指定すると、追加されたキーフレームがすべて破棄されます。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_EndAddKeyframes(True, akH)  # キーフレームを追加

キーフレームラベル色（AE 22.5+）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetKeyframeLabelColorIndex(streamH: int, keyIndex: int) -> int

   キーフレームのラベル色インデックスを取得します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :return: ラベル色インデックス（0 ～ 16）
   :rtype: int

   .. note::
      この関数はAE 22.5以降（KeyframeSuite5）でのみ使用可能です。

   **例**:

   .. code-block:: python

      label = ae.sdk.AEGP_GetKeyframeLabelColorIndex(streamH, 0)
      print(f"ラベル色: {label}")

.. function:: AEGP_SetKeyframeLabelColorIndex(streamH: int, keyIndex: int, label: int) -> None

   キーフレームのラベル色を設定します。

   :param streamH: ストリームハンドル
   :type streamH: int
   :param keyIndex: キーフレームインデックス
   :type keyIndex: int
   :param label: ラベル色インデックス（0 ～ 16）
   :type label: int

   .. note::
      この関数はAE 22.5以降（KeyframeSuite5）でのみ使用可能です。

   **例**:

   .. code-block:: python

      # ラベル色を赤（インデックス1）に設定
      ae.sdk.AEGP_SetKeyframeLabelColorIndex(streamH, 0, 1)

使用例
------

キーフレームの追加と削除
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def add_keyframes_to_position():
       """レイヤーの位置にキーフレームを追加"""
       # レイヤーを取得
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       itemH = ae.sdk.AEGP_GetProjectItemByIndex(projH, 0)
       compH = ae.sdk.AEGP_GetCompFromItem(itemH)
       layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, 0)

       # 位置ストリームを取得
       streamH = ae.sdk.AEGP_GetNewLayerStream(ae.sdk.AEGP_GetPluginID(), layerH, ae.sdk.AEGP_LayerStream_POSITION)

       try:
           # 3つのキーフレームを追加
           times = [0.0, 1.0, 2.0]
           for t in times:
               key_idx = ae.sdk.AEGP_InsertKeyframe(streamH, ae.sdk.AEGP_LTimeMode_CompTime, t)
               print(f"{t}秒にキーフレーム {key_idx} を追加")

           # キーフレーム数を確認
           num_kfs = ae.sdk.AEGP_GetStreamNumKfs(streamH)
           print(f"合計キーフレーム数: {num_kfs}")

           # 中間のキーフレームを削除
           ae.sdk.AEGP_DeleteKeyframe(streamH, 1)
           print("2番目のキーフレームを削除しました")

       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

   add_keyframes_to_position()

イージングの設定
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def apply_easy_ease():
       """選択されたキーフレームにEasy Easeを適用"""
       # レイヤーとストリームを取得（省略）
       streamH = ...  # AEGP_GetNewLayerStream() で取得

       try:
           # ストリームの次元数を取得
           dim = ae.sdk.AEGP_GetStreamValueDimensionality(streamH)

           # すべてのキーフレームにEasy Easeを適用
           num_kfs = ae.sdk.AEGP_GetStreamNumKfs(streamH)
           for key_idx in range(num_kfs):
               # 各次元にイージングを設定
               for d in range(dim):
                   ae.sdk.AEGP_SetKeyframeTemporalEase(
                       streamH, key_idx, d,
                       in_speed=0.0,       # ゼロ速度で到達
                       in_influence=33.33,
                       out_speed=0.0,      # ゼロ速度で出発
                       out_influence=33.33
                   )

               # 補間をベジェに設定
               ae.sdk.AEGP_SetKeyframeInterpolation(
                   streamH, key_idx,
                   ae.sdk.AEGP_KeyInterp_BEZIER,
                   ae.sdk.AEGP_KeyInterp_BEZIER
               )

           print(f"{num_kfs} 個のキーフレームにEasy Easeを適用しました")

       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

   apply_easy_ease()

モーションパスの操作（空間タンジェント）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def adjust_motion_path():
       """モーションパスをカスタマイズ（空間タンジェント調整）"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # レイヤーの位置ストリームを取得（省略）
       streamH = ...  # Position stream

       try:
           num_kfs = ae.sdk.AEGP_GetStreamNumKfs(streamH)

           for key_idx in range(num_kfs):
               # 空間タンジェントを取得
               in_tanP, out_tanP = ae.sdk.AEGP_GetNewKeyframeSpatialTangents(
                   plugin_id, streamH, key_idx
               )

               try:
                   # Outタンジェントを右水平方向に設定
                   ae.sdk.AEGP_SetStreamValueTwoDVal(out_tanP, 100.0, 0.0)

                   # Inタンジェントを左水平方向に設定
                   ae.sdk.AEGP_SetStreamValueTwoDVal(in_tanP, -100.0, 0.0)

                   # タンジェントを適用
                   ae.sdk.AEGP_SetKeyframeSpatialTangents(streamH, key_idx, in_tanP, out_tanP)

                   # 空間連続性フラグを有効化
                   ae.sdk.AEGP_SetKeyframeFlag(
                       streamH, key_idx,
                       ae.sdk.AEGP_KeyframeFlag_SPATIAL_CONTINUOUS,
                       True
                   )

                   print(f"キーフレーム {key_idx} のモーションパスを調整しました")

               finally:
                   ae.sdk.AEGP_DisposeStreamValue(in_tanP)
                   ae.sdk.AEGP_DisposeStreamValue(out_tanP)

       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

   adjust_motion_path()

バッチ操作で複数キーフレームを追加
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def batch_add_keyframes():
       """バッチAPIで複数のキーフレームを効率的に追加"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       streamH = ...  # ストリームを取得

       try:
           # バッチセッション開始
           akH = ae.sdk.AEGP_StartAddKeyframes(streamH)

           try:
               # 10個のキーフレームを追加
               key_indices = []
               for i in range(10):
                   time = i * 0.5  # 0.5秒間隔
                   key_idx = ae.sdk.AEGP_AddKeyframes(akH, ae.sdk.AEGP_LTimeMode_CompTime, time)
                   key_indices.append(key_idx)

               # 各キーフレームに値を設定
               for i, key_idx in enumerate(key_indices):
                   # 値を作成
                   valueP = ae.sdk.AEGP_NewStreamValue(plugin_id, streamH)
                   try:
                       value = i * 10.0  # 0, 10, 20, ...
                       ae.sdk.AEGP_SetStreamValueOneDVal(valueP, value)
                       # バッチに設定
                       ae.sdk.AEGP_SetAddKeyframe(akH, key_idx, valueP)
                   finally:
                       ae.sdk.AEGP_DisposeStreamValue(valueP)

               # バッチを適用
               ae.sdk.AEGP_EndAddKeyframes(True, akH)
               print(f"{len(key_indices)} 個のキーフレームを追加しました")

           except Exception as e:
               # エラー時はキャンセル
               ae.sdk.AEGP_EndAddKeyframes(False, akH)
               raise

       finally:
           ae.sdk.AEGP_DisposeStream(streamH)

   batch_add_keyframes()

キーフレームデータの完全コピー
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def copy_keyframe_data(src_streamH, dst_streamH):
       """キーフレームデータを完全にコピー（値、補間、イージング、タンジェント）"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # ソースのキーフレーム数を取得
       num_kfs = ae.sdk.AEGP_GetStreamNumKfs(src_streamH)
       dim = ae.sdk.AEGP_GetStreamValueDimensionality(src_streamH)

       for key_idx in range(num_kfs):
           # 時間を取得
           time = ae.sdk.AEGP_GetKeyframeTime(src_streamH, key_idx, ae.sdk.AEGP_LTimeMode_CompTime)

           # キーフレームを挿入
           new_key_idx = ae.sdk.AEGP_InsertKeyframe(dst_streamH, ae.sdk.AEGP_LTimeMode_CompTime, time)

           # 値をコピー
           valueP = ae.sdk.AEGP_GetNewKeyframeValue(plugin_id, src_streamH, key_idx)
           try:
               ae.sdk.AEGP_SetKeyframeValue(dst_streamH, new_key_idx, valueP)
           finally:
               ae.sdk.AEGP_DisposeStreamValue(valueP)

           # 補間タイプをコピー
           in_interp, out_interp = ae.sdk.AEGP_GetKeyframeInterpolation(src_streamH, key_idx)
           ae.sdk.AEGP_SetKeyframeInterpolation(dst_streamH, new_key_idx, in_interp, out_interp)

           # テンポラルイージングをコピー
           for d in range(dim):
               (in_speed, in_inf), (out_speed, out_inf) = \
                   ae.sdk.AEGP_GetKeyframeTemporalEase(src_streamH, key_idx, d)
               ae.sdk.AEGP_SetKeyframeTemporalEase(
                   dst_streamH, new_key_idx, d,
                   in_speed, in_inf, out_speed, out_inf
               )

           # フラグをコピー
           flags = ae.sdk.AEGP_GetKeyframeFlags(src_streamH, key_idx)
           for flag in [
               ae.sdk.AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS,
               ae.sdk.AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER,
               ae.sdk.AEGP_KeyframeFlag_SPATIAL_CONTINUOUS,
               ae.sdk.AEGP_KeyframeFlag_SPATIAL_AUTOBEZIER,
               ae.sdk.AEGP_KeyframeFlag_ROVING
           ]:
               if flags & flag:
                   ae.sdk.AEGP_SetKeyframeFlag(dst_streamH, new_key_idx, flag, True)

       print(f"{num_kfs} 個のキーフレームをコピーしました")

   # 使用例
   # copy_keyframe_data(src_streamH, dst_streamH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリ管理**

   ``AEGP_GetNewKeyframeValue()`` と ``AEGP_GetNewKeyframeSpatialTangents()`` で取得したポインタは、使用後に必ず ``AEGP_DisposeStreamValue()`` で解放してください。

2. **キーフレームインデックスの変動**

   キーフレームを追加・削除すると、後続のキーフレームのインデックスが変わります。時間でキーフレームを特定する場合は ``AEGP_GetKeyframeTime()`` を使用してください。

3. **空間タンジェントは位置プロパティのみ**

   ``AEGP_GetNewKeyframeSpatialTangents()`` と ``AEGP_SetKeyframeSpatialTangents()`` は位置（Position）プロパティでのみ使用可能です。

4. **次元の指定**

   テンポラルイージングは次元ごとに設定します（例: Positionは2次元なら0と1）。

5. **バッチ操作のキャンセル**

   ``AEGP_EndAddKeyframes(False, akH)`` でバッチ操作をキャンセルできます。エラー処理で活用してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

メモリ管理の徹底
^^^^^^^^^^^^^^^^

.. code-block:: python

   # try-finally を使用して確実に解放
   valueP = ae.sdk.AEGP_GetNewKeyframeValue(plugin_id, streamH, key_idx)
   try:
       # 値を使用
       pass
   finally:
       ae.sdk.AEGP_DisposeStreamValue(valueP)

次元数の確認
^^^^^^^^^^^^

.. code-block:: python

   # イージングを設定する前に次元数を確認
   dim = ae.sdk.AEGP_GetStreamValueDimensionality(streamH)
   for d in range(dim):
       ae.sdk.AEGP_SetKeyframeTemporalEase(streamH, key_idx, d, ...)

補間タイプの確認
^^^^^^^^^^^^^^^^

.. code-block:: python

   # イージングを設定する前に補間タイプをベジェに設定
   ae.sdk.AEGP_SetKeyframeInterpolation(
       streamH, key_idx,
       ae.sdk.AEGP_KeyInterp_BEZIER,
       ae.sdk.AEGP_KeyInterp_BEZIER
   )

バッチ操作の活用
^^^^^^^^^^^^^^^^

.. code-block:: python

   # 複数のキーフレームを追加する場合はバッチ操作を使用
   akH = ae.sdk.AEGP_StartAddKeyframes(streamH)
   try:
       for time in times:
           key_idx = ae.sdk.AEGP_AddKeyframes(akH, time_mode, time)
           # ...
       ae.sdk.AEGP_EndAddKeyframes(True, akH)
   except:
       ae.sdk.AEGP_EndAddKeyframes(False, akH)
       raise

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("キーフレーム操作")
   try:
       # 複数のキーフレーム操作
       pass
   finally:
       ae.sdk.AEGP_EndUndoGroup()

関連項目
--------

- :doc:`AEGP_StreamSuite6` - ストリーム管理（キーフレームのベース）
- :doc:`AEGP_DynamicStreamSuite4` - ダイナミックストリーム操作
- :doc:`AEGP_UtilitySuite6` - Undo管理、エラー処理
- :doc:`index` - 低レベルAPI概要
