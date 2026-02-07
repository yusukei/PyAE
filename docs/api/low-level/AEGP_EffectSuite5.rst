AEGP_EffectSuite5
=================

.. currentmodule:: ae.sdk

AEGP_EffectSuite5は、After Effectsのエフェクト操作を行うためのSDK APIです。

概要
----

**実装状況**: 23/23関数実装 ✅

AEGP_EffectSuite5は以下の機能を提供します:

- レイヤーエフェクトの取得、適用、削除、複製
- インストール済みエフェクトの列挙と情報取得
- エフェクト名、マッチ名、カテゴリの取得
- エフェクトパラメータストリームへのアクセス
- エフェクトフラグの取得・設定
- エフェクトの並び替え、有効/無効の切り替え
- エフェクトマスクの管理（AE 13.0+）
- 内部エフェクト判定（AE 24.1+）

基本概念
--------

エフェクトハンドル (AEGP_EffectRefH)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのレイヤーに適用されたエフェクトを識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - エフェクトハンドルは、エフェクトが存在する間のみ有効です
   - エフェクトを削除した後は、ハンドルは無効になります
   - 各エフェクトインスタンスに固有のハンドルがあります

インストール済みエフェクトキー (AEGP_InstalledEffectKey)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsにインストールされているエフェクトタイプを識別するためのキー。PyAEでは整数値 (``int``) として扱われます。

.. note::
   - ``AEGP_InstalledEffectKey_NONE`` (0) は無効なキーを表します
   - ``AEGP_GetNextInstalledEffect()`` で全エフェクトを列挙できます
   - エフェクトキーは After Effects のバージョンやプラグインのインストール状態によって変わる可能性があります

エフェクトマッチ名 (Match Name)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

エフェクトを一意に識別する文字列。バージョン間で一貫性があり、スクリプトでエフェクトを特定する際に使用されます。

**例**:

- ``ADBE Gaussian Blur 2`` - ぼかし（ガウス）
- ``ADBE APC Vibrance`` - 自然な彩度
- ``ADBE Fill`` - 塗り

.. tip::
   エフェクトの表示名（ローカライズされた名前）ではなく、マッチ名を使用することで、言語環境に依存しないスクリプトを作成できます。

エフェクトフラグ (AEGP_EffectFlags)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

エフェクトの状態を表すビットフラグ:

.. list-table::
   :header-rows: 1

   * - フラグ
     - 説明
   * - ``AEGP_EffectFlags_NONE``
     - フラグなし
   * - ``AEGP_EffectFlags_ACTIVE``
     - エフェクトが有効
   * - ``AEGP_EffectFlags_AUDIO_ONLY``
     - オーディオのみに適用
   * - ``AEGP_EffectFlags_AUDIO_TOO``
     - オーディオにも適用
   * - ``AEGP_EffectFlags_MISSING``
     - エフェクトプラグインが見つからない

API リファレンス
----------------

レイヤーエフェクトの取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetLayerNumEffects(layerH: int) -> int

   レイヤーに適用されているエフェクトの数を取得します。

   :param layerH: レイヤーハンドル
   :type layerH: int
   :return: エフェクト数
   :rtype: int

   **例**:

   .. code-block:: python

      num_effects = ae.sdk.AEGP_GetLayerNumEffects(layerH)
      print(f"エフェクト数: {num_effects}")

.. function:: AEGP_GetLayerEffectByIndex(plugin_id: int, layerH: int, effect_index: int) -> int

   インデックスからエフェクトハンドルを取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param layerH: レイヤーハンドル
   :type layerH: int
   :param effect_index: エフェクトのインデックス（0から始まる）
   :type effect_index: int
   :return: エフェクトハンドル
   :rtype: int

   .. note::
      エフェクトの順序は、After Effects UI のエフェクトコントロールパネルでの並び順に対応します。
      インデックス0が最上位のエフェクトです。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      effectH = ae.sdk.AEGP_GetLayerEffectByIndex(plugin_id, layerH, 0)  # 最初のエフェクト

.. function:: AEGP_GetInstalledKeyFromLayerEffect(effectH: int) -> int

   エフェクトハンドルからインストール済みエフェクトキーを取得します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: インストール済みエフェクトキー
   :rtype: int

   **例**:

   .. code-block:: python

      installed_key = ae.sdk.AEGP_GetInstalledKeyFromLayerEffect(effectH)
      match_name = ae.sdk.AEGP_GetEffectMatchName(installed_key)
      print(f"エフェクトマッチ名: {match_name}")

インストール済みエフェクトの列挙
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNumInstalledEffects() -> int

   インストールされているエフェクトの総数を取得します。

   :return: インストール済みエフェクト数
   :rtype: int

   **例**:

   .. code-block:: python

      num_installed = ae.sdk.AEGP_GetNumInstalledEffects()
      print(f"インストール済みエフェクト数: {num_installed}")

.. function:: AEGP_GetNextInstalledEffect(installed_key: int) -> int

   次のインストール済みエフェクトキーを取得します。

   :param installed_key: 現在のインストール済みエフェクトキー（``AEGP_InstalledEffectKey_NONE`` で最初のエフェクト）
   :type installed_key: int
   :return: 次のインストール済みエフェクトキー（最後に達した場合は ``AEGP_InstalledEffectKey_NONE``）
   :rtype: int

   .. note::
      全エフェクトを列挙するには、``AEGP_InstalledEffectKey_NONE`` から開始し、戻り値が ``AEGP_InstalledEffectKey_NONE`` になるまで繰り返します。

   **例**:

   .. code-block:: python

      key = ae.sdk.AEGP_InstalledEffectKey_NONE
      while True:
          key = ae.sdk.AEGP_GetNextInstalledEffect(key)
          if key == ae.sdk.AEGP_InstalledEffectKey_NONE:
              break
          name = ae.sdk.AEGP_GetEffectName(key)
          print(f"エフェクト: {name}")

エフェクト情報の取得
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetEffectName(installed_key: int) -> str

   エフェクトの表示名を取得します（ローカライズされた名前）。

   :param installed_key: インストール済みエフェクトキー
   :type installed_key: int
   :return: エフェクト表示名
   :rtype: str

   **例**:

   .. code-block:: python

      name = ae.sdk.AEGP_GetEffectName(installed_key)
      print(f"エフェクト名: {name}")

.. function:: AEGP_GetEffectMatchName(installed_key: int) -> str

   エフェクトのマッチ名を取得します（一意識別子）。

   :param installed_key: インストール済みエフェクトキー
   :type installed_key: int
   :return: エフェクトマッチ名
   :rtype: str

   .. important::
      マッチ名は、After Effects のバージョンや言語環境に依存しない一意の識別子です。
      スクリプトでエフェクトを特定する際は、表示名ではなくマッチ名を使用してください。

   **例**:

   .. code-block:: python

      match_name = ae.sdk.AEGP_GetEffectMatchName(installed_key)
      print(f"マッチ名: {match_name}")

.. function:: AEGP_GetEffectCategory(installed_key: int) -> str

   エフェクトのカテゴリを取得します。

   :param installed_key: インストール済みエフェクトキー
   :type installed_key: int
   :return: エフェクトカテゴリ
   :rtype: str

   **例**:

   .. code-block:: python

      category = ae.sdk.AEGP_GetEffectCategory(installed_key)
      print(f"カテゴリ: {category}")

.. function:: AEGP_GetIsInternalEffect(installed_key: int) -> bool

   エフェクトが内部エフェクト（After Effects 組み込み）かどうかを判定します。

   :param installed_key: インストール済みエフェクトキー
   :type installed_key: int
   :return: 内部エフェクトの場合は ``True``、サードパーティプラグインの場合は ``False``
   :rtype: bool

   .. note::
      この関数は After Effects 24.1（EffectSuite5）以降でのみ利用可能です。

   **例**:

   .. code-block:: python

      is_internal = ae.sdk.AEGP_GetIsInternalEffect(installed_key)
      if is_internal:
          print("内部エフェクト")
      else:
          print("サードパーティプラグイン")

エフェクトの適用・削除・複製
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_ApplyEffect(plugin_id: int, layerH: int, installed_key: int) -> int

   レイヤーにエフェクトを適用します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param layerH: レイヤーハンドル
   :type layerH: int
   :param installed_key: インストール済みエフェクトキー
   :type installed_key: int
   :return: 新しく適用されたエフェクトハンドル
   :rtype: int

   .. note::
      エフェクトは、エフェクトスタックの最上位（インデックス0）に追加されます。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      # マッチ名からエフェクトキーを取得（後述の補助関数を使用）
      key = find_effect_by_match_name("ADBE Gaussian Blur 2")
      effectH = ae.sdk.AEGP_ApplyEffect(plugin_id, layerH, key)

.. function:: AEGP_DeleteLayerEffect(effectH: int) -> None

   レイヤーからエフェクトを削除します。

   :param effectH: エフェクトハンドル
   :type effectH: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_DeleteLayerEffect(effectH)

.. function:: AEGP_DuplicateEffect(effectH: int) -> int

   エフェクトを複製します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: 複製されたエフェクトハンドル
   :rtype: int

   .. note::
      複製されたエフェクトは、元のエフェクトの直後（インデックス+1）に配置されます。
      パラメータ値やキーフレームもすべてコピーされます。

   **例**:

   .. code-block:: python

      dup_effectH = ae.sdk.AEGP_DuplicateEffect(effectH)

エフェクトパラメータへのアクセス
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetEffectNumParamStreams(effectH: int) -> int

   エフェクトのパラメータストリーム数を取得します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: パラメータストリーム数
   :rtype: int

   .. note::
      この関数は現在 PyAE で実装されていません。将来のリリースで追加予定です。

.. function:: AEGP_GetEffectParamStreamByIndex(plugin_id: int, effectH: int, param_index: int) -> int

   インデックスからパラメータストリームハンドルを取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param effectH: エフェクトハンドル
   :type effectH: int
   :param param_index: パラメータインデックス（0から始まる）
   :type param_index: int
   :return: パラメータストリームハンドル
   :rtype: int

   .. note::
      この関数は現在 PyAE で実装されていません。将来のリリースで追加予定です。

.. function:: AEGP_GetEffectParamUnionByIndex(plugin_id: int, effectH: int, param_index: int) -> dict

   パラメータの定義情報を取得します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param effectH: エフェクトハンドル
   :type effectH: int
   :param param_index: パラメータインデックス（0から始まる）
   :type param_index: int
   :return: パラメータ定義情報の辞書
   :rtype: dict

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``param_type``
        - int
        - パラメータタイプ（``PF_Param_*`` 定数）
      * - ``param_type_name``
        - str
        - パラメータタイプ名（人間可読）
      * - ``warning``
        - str
        - 警告メッセージ

   **パラメータタイプ** (``PF_ParamType``):

   - ``PF_Param_LAYER`` - レイヤー
   - ``PF_Param_SLIDER`` - スライダー（整数）
   - ``PF_Param_FIX_SLIDER`` - 固定小数点スライダー
   - ``PF_Param_ANGLE`` - 角度
   - ``PF_Param_CHECKBOX`` - チェックボックス
   - ``PF_Param_COLOR`` - カラー
   - ``PF_Param_POINT`` - ポイント（2D座標）
   - ``PF_Param_POPUP`` - ポップアップメニュー
   - ``PF_Param_NO_DATA`` - データなし
   - ``PF_Param_FLOAT_SLIDER`` - 浮動小数点スライダー
   - ``PF_Param_ARBITRARY_DATA`` - 任意データ
   - ``PF_Param_PATH`` - パス
   - ``PF_Param_GROUP_START`` - グループ開始
   - ``PF_Param_GROUP_END`` - グループ終了
   - ``PF_Param_BUTTON`` - ボタン
   - ``PF_Param_POINT_3D`` - 3Dポイント

   .. warning::
      この関数はパラメータの **型情報** のみを返します。実際のパラメータ値を取得するには、
      ``AEGP_GetNewEffectStreamByIndex()`` と ``AEGP_GetNewStreamValue()`` を使用してください。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      param_info = ae.sdk.AEGP_GetEffectParamUnionByIndex(plugin_id, effectH, 0)
      print(f"パラメータタイプ: {param_info['param_type_name']}")

エフェクトフラグの管理
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetEffectFlags(effectH: int) -> int

   エフェクトのフラグを取得します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: エフェクトフラグ（``AEGP_EffectFlags`` のビットOR）
   :rtype: int

   **例**:

   .. code-block:: python

      flags = ae.sdk.AEGP_GetEffectFlags(effectH)
      is_active = (flags & ae.sdk.AEGP_EffectFlags_ACTIVE) != 0
      print(f"エフェクト有効: {is_active}")

.. function:: AEGP_SetEffectFlags(effectH: int, mask: int, flags: int) -> None

   エフェクトのフラグを設定します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :param mask: 変更するフラグのマスク
   :type mask: int
   :param flags: 設定するフラグ値
   :type flags: int

   .. note::
      ``mask`` で指定したビットのみが変更されます。他のビットは変更されません。

   **例**:

   .. code-block:: python

      # エフェクトを無効化
      ae.sdk.AEGP_SetEffectFlags(
          effectH,
          ae.sdk.AEGP_EffectFlags_ACTIVE,  # マスク
          0  # 無効化
      )

      # エフェクトを有効化
      ae.sdk.AEGP_SetEffectFlags(
          effectH,
          ae.sdk.AEGP_EffectFlags_ACTIVE,  # マスク
          ae.sdk.AEGP_EffectFlags_ACTIVE   # 有効化
      )

.. function:: AEGP_EffectIsActive(effectH: int) -> bool

   エフェクトが有効かどうかを判定します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: 有効な場合は ``True``、無効な場合は ``False``
   :rtype: bool

   .. note::
      この関数は現在 PyAE で実装されていません。``AEGP_GetEffectFlags()`` を使用してください。

.. function:: AEGP_SetEffectActive(effectH: int, active: bool) -> None

   エフェクトを有効/無効に設定します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :param active: ``True`` で有効化、``False`` で無効化
   :type active: bool

   .. note::
      この関数は現在 PyAE で実装されていません。``AEGP_SetEffectFlags()`` を使用してください。

エフェクトの並び替え
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_ReorderEffect(effectH: int, effect_index: int) -> None

   エフェクトの並び順を変更します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :param effect_index: 新しいインデックス位置（0から始まる）
   :type effect_index: int

   .. note::
      エフェクトスタックの並び順は、レンダリング結果に影響します。
      インデックス0が最上位（最初に適用されるエフェクト）です。

   **例**:

   .. code-block:: python

      # エフェクトを最上位に移動
      ae.sdk.AEGP_ReorderEffect(effectH, 0)

エフェクトマスクの管理（AE 13.0+）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NumEffectMask(effectH: int) -> int

   エフェクトに適用されているマスクの数を取得します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :return: マスク数
   :rtype: int

   **例**:

   .. code-block:: python

      num_masks = ae.sdk.AEGP_NumEffectMask(effectH)
      print(f"マスク数: {num_masks}")

.. function:: AEGP_GetEffectMaskID(effectH: int, mask_index: int) -> int

   インデックスからマスクIDを取得します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :param mask_index: マスクインデックス（0から始まる）
   :type mask_index: int
   :return: マスクID
   :rtype: int

   **例**:

   .. code-block:: python

      mask_id = ae.sdk.AEGP_GetEffectMaskID(effectH, 0)

.. function:: AEGP_AddEffectMask(effectH: int, id_val: int) -> int

   エフェクトにマスクを追加します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :param id_val: マスクID
   :type id_val: int
   :return: マスクストリームハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      stream_h = ae.sdk.AEGP_AddEffectMask(effectH, mask_id)

.. function:: AEGP_RemoveEffectMask(effectH: int, id_val: int) -> None

   エフェクトからマスクを削除します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :param id_val: マスクID
   :type id_val: int

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_RemoveEffectMask(effectH, mask_id)

.. function:: AEGP_SetEffectMask(effectH: int, mask_index: int, id_val: int) -> int

   エフェクトマスクを変更します。

   :param effectH: エフェクトハンドル
   :type effectH: int
   :param mask_index: マスクインデックス（0から始まる）
   :type mask_index: int
   :param id_val: 新しいマスクID
   :type id_val: int
   :return: マスクストリームハンドル
   :rtype: int

   **例**:

   .. code-block:: python

      stream_h = ae.sdk.AEGP_SetEffectMask(effectH, 0, new_mask_id)

高度な機能
~~~~~~~~~~

.. function:: AEGP_EffectCallGeneric(plugin_id: int, effectH: int, time: float | None = None, effect_cmd: int = PF_Cmd_COMPLETELY_GENERAL) -> None

   エフェクトに汎用コマンドを送信します。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param effectH: エフェクトハンドル
   :type effectH: int
   :param time: コマンド実行時刻（秒）。``None`` の場合は現在時刻
   :type time: float | None
   :param effect_cmd: エフェクトコマンド（``PF_Cmd_*`` 定数）
   :type effect_cmd: int

   **エフェクトコマンド** (``PF_Cmd``):

   - ``PF_Cmd_COMPLETELY_GENERAL`` - 汎用コマンド
   - ``PF_Cmd_ABOUT`` - バージョン情報
   - ``PF_Cmd_GLOBAL_SETUP`` - グローバルセットアップ
   - ``PF_Cmd_PARAMS_SETUP`` - パラメータセットアップ
   - ``PF_Cmd_SEQUENCE_SETUP`` - シーケンスセットアップ
   - ``PF_Cmd_RENDER`` - レンダリング
   - ``PF_Cmd_USER_CHANGED_PARAM`` - ユーザーがパラメータを変更
   - ``PF_Cmd_UPDATE_PARAMS_UI`` - パラメータUIを更新

   .. warning::
      この関数は高度な使用例のためのものです。通常のスクリプトでは使用しません。
      エフェクトのデシリアライズや内部状態の再初期化に使用されます。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      # パラメータUIを更新
      ae.sdk.AEGP_EffectCallGeneric(
          plugin_id, effectH,
          effect_cmd=ae.sdk.PF_Cmd_UPDATE_PARAMS_UI
      )

リソース管理
~~~~~~~~~~~~

.. function:: AEGP_DisposeEffect(effectH: int) -> None

   エフェクトハンドルを解放します。

   :param effectH: エフェクトハンドル
   :type effectH: int

   .. warning::
      この関数は、エフェクトハンドルのメモリを解放するためのものです。
      エフェクトをレイヤーから削除するには ``AEGP_DeleteLayerEffect()`` を使用してください。

   **例**:

   .. code-block:: python

      # エフェクトハンドルを解放
      ae.sdk.AEGP_DisposeEffect(effectH)

使用例
------

インストール済みエフェクトの一覧取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def list_all_installed_effects():
       """インストールされているすべてのエフェクトを一覧表示"""
       effects = []

       key = ae.sdk.AEGP_InstalledEffectKey_NONE
       while True:
           key = ae.sdk.AEGP_GetNextInstalledEffect(key)
           if key == ae.sdk.AEGP_InstalledEffectKey_NONE:
               break

           name = ae.sdk.AEGP_GetEffectName(key)
           match_name = ae.sdk.AEGP_GetEffectMatchName(key)
           category = ae.sdk.AEGP_GetEffectCategory(key)

           effects.append({
               "key": key,
               "name": name,
               "match_name": match_name,
               "category": category
           })

       # カテゴリ別に表示
       effects_by_category = {}
       for effect in effects:
           cat = effect["category"]
           if cat not in effects_by_category:
               effects_by_category[cat] = []
           effects_by_category[cat].append(effect)

       for category, category_effects in sorted(effects_by_category.items()):
           print(f"\n[{category}]")
           for effect in category_effects:
               print(f"  {effect['name']} ({effect['match_name']})")

       return effects

   # 実行
   list_all_installed_effects()

マッチ名でエフェクトを検索
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def find_effect_by_match_name(match_name: str) -> int:
       """マッチ名からインストール済みエフェクトキーを検索"""
       key = ae.sdk.AEGP_InstalledEffectKey_NONE
       while True:
           key = ae.sdk.AEGP_GetNextInstalledEffect(key)
           if key == ae.sdk.AEGP_InstalledEffectKey_NONE:
               raise ValueError(f"エフェクトが見つかりません: {match_name}")

           if ae.sdk.AEGP_GetEffectMatchName(key) == match_name:
               return key

   # 使用例
   blur_key = find_effect_by_match_name("ADBE Gaussian Blur 2")
   print(f"ぼかし（ガウス）のキー: {blur_key}")

エフェクトの適用と設定
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def apply_gaussian_blur(layer, blur_amount=10.0):
       """レイヤーにぼかし（ガウス）を適用"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # エフェクトを検索
       key = ae.sdk.AEGP_InstalledEffectKey_NONE
       while True:
           key = ae.sdk.AEGP_GetNextInstalledEffect(key)
           if key == ae.sdk.AEGP_InstalledEffectKey_NONE:
               raise ValueError("ぼかし（ガウス）が見つかりません")

           match_name = ae.sdk.AEGP_GetEffectMatchName(key)
           if match_name == "ADBE Gaussian Blur 2":
               break

       # エフェクトを適用
       layerH = layer._handle
       effectH = ae.sdk.AEGP_ApplyEffect(plugin_id, layerH, key)

       print(f"ぼかし（ガウス）を適用しました")
       return effectH

   # 使用例
   comp = ae.comp()
   layer = comp.layer(1)
   effect_h = apply_gaussian_blur(layer, blur_amount=20.0)

レイヤーのすべてのエフェクトを列挙
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def list_layer_effects(layer):
       """レイヤーに適用されているすべてのエフェクトを一覧表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       layerH = layer._handle

       num_effects = ae.sdk.AEGP_GetLayerNumEffects(layerH)
       print(f"エフェクト数: {num_effects}")

       for i in range(num_effects):
           effectH = ae.sdk.AEGP_GetLayerEffectByIndex(plugin_id, layerH, i)
           installed_key = ae.sdk.AEGP_GetInstalledKeyFromLayerEffect(effectH)

           name = ae.sdk.AEGP_GetEffectName(installed_key)
           match_name = ae.sdk.AEGP_GetEffectMatchName(installed_key)
           flags = ae.sdk.AEGP_GetEffectFlags(effectH)
           is_active = (flags & ae.sdk.AEGP_EffectFlags_ACTIVE) != 0

           print(f"{i}: {name} ({match_name}) - {'有効' if is_active else '無効'}")

           # エフェクトハンドルを解放
           ae.sdk.AEGP_DisposeEffect(effectH)

   # 使用例
   comp = ae.comp()
   layer = comp.layer(1)
   list_layer_effects(layer)

エフェクトの有効/無効切り替え
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def toggle_effect(layer, effect_index: int):
       """指定したエフェクトの有効/無効を切り替え"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       layerH = layer._handle

       effectH = ae.sdk.AEGP_GetLayerEffectByIndex(plugin_id, layerH, effect_index)

       # 現在の状態を取得
       flags = ae.sdk.AEGP_GetEffectFlags(effectH)
       is_active = (flags & ae.sdk.AEGP_EffectFlags_ACTIVE) != 0

       # 反転
       new_flags = 0 if is_active else ae.sdk.AEGP_EffectFlags_ACTIVE
       ae.sdk.AEGP_SetEffectFlags(
           effectH,
           ae.sdk.AEGP_EffectFlags_ACTIVE,  # マスク
           new_flags
       )

       print(f"エフェクト{effect_index}を{'無効化' if is_active else '有効化'}しました")

       # エフェクトハンドルを解放
       ae.sdk.AEGP_DisposeEffect(effectH)

   # 使用例
   comp = ae.comp()
   layer = comp.layer(1)
   toggle_effect(layer, 0)  # 最初のエフェクトを切り替え

エフェクトパラメータの型情報取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def inspect_effect_parameters(layer, effect_index: int):
       """エフェクトのすべてのパラメータ情報を表示"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       layerH = layer._handle

       effectH = ae.sdk.AEGP_GetLayerEffectByIndex(plugin_id, layerH, effect_index)
       installed_key = ae.sdk.AEGP_GetInstalledKeyFromLayerEffect(effectH)

       effect_name = ae.sdk.AEGP_GetEffectName(installed_key)
       print(f"\nエフェクト: {effect_name}")
       print("=" * 50)

       # パラメータ数を取得（仮定: 最大100パラメータ）
       for i in range(100):
           try:
               param_info = ae.sdk.AEGP_GetEffectParamUnionByIndex(plugin_id, effectH, i)
               print(f"パラメータ {i}: {param_info['param_type_name']}")
           except:
               # パラメータが存在しない場合はループを終了
               break

       # エフェクトハンドルを解放
       ae.sdk.AEGP_DisposeEffect(effectH)

   # 使用例
   comp = ae.comp()
   layer = comp.layer(1)
   inspect_effect_parameters(layer, 0)

エフェクトの複製とカスタマイズ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def duplicate_and_customize_effect(layer, effect_index: int):
       """エフェクトを複製し、異なる設定で追加"""
       plugin_id = ae.sdk.AEGP_GetPluginID()
       layerH = layer._handle

       # 元のエフェクトを取得
       original_effectH = ae.sdk.AEGP_GetLayerEffectByIndex(plugin_id, layerH, effect_index)

       # 複製
       dup_effectH = ae.sdk.AEGP_DuplicateEffect(original_effectH)

       print("エフェクトを複製しました")

       # 複製したエフェクトをカスタマイズ
       # （実際のパラメータ設定は StreamSuite を使用）

       # エフェクトハンドルを解放
       ae.sdk.AEGP_DisposeEffect(original_effectH)
       ae.sdk.AEGP_DisposeEffect(dup_effectH)

   # 使用例
   comp = ae.comp()
   layer = comp.layer(1)
   duplicate_and_customize_effect(layer, 0)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **エフェクトハンドルの解放**

   ``AEGP_GetLayerEffectByIndex()`` で取得したエフェクトハンドルは、使用後に ``AEGP_DisposeEffect()`` で解放してください。

2. **マッチ名の使用**

   エフェクトを識別する際は、表示名ではなくマッチ名を使用してください。
   表示名は言語環境によって変わりますが、マッチ名は一貫しています。

3. **エフェクトの削除と解放の違い**

   - ``AEGP_DeleteLayerEffect()`` - エフェクトをレイヤーから削除
   - ``AEGP_DisposeEffect()`` - エフェクトハンドルのメモリを解放

4. **パラメータ値の取得**

   ``AEGP_GetEffectParamUnionByIndex()`` はパラメータの **型情報** のみを返します。
   実際のパラメータ値を取得するには、``AEGP_GetNewEffectStreamByIndex()`` と ``AEGP_GetNewStreamValue()`` を使用してください。

5. **エフェクトフラグの変更**

   ``AEGP_SetEffectFlags()`` は、``mask`` で指定したビットのみを変更します。
   他のビットを変更しないように注意してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

エフェクトハンドルの解放
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   plugin_id = ae.sdk.AEGP_GetPluginID()
   effectH = ae.sdk.AEGP_GetLayerEffectByIndex(plugin_id, layerH, 0)
   try:
       # エフェクト操作
       pass
   finally:
       ae.sdk.AEGP_DisposeEffect(effectH)

マッチ名による検索
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def find_effect_by_match_name(match_name: str) -> int:
       key = ae.sdk.AEGP_InstalledEffectKey_NONE
       while True:
           key = ae.sdk.AEGP_GetNextInstalledEffect(key)
           if key == ae.sdk.AEGP_InstalledEffectKey_NONE:
               raise ValueError(f"エフェクトが見つかりません: {match_name}")
           if ae.sdk.AEGP_GetEffectMatchName(key) == match_name:
               return key

エフェクトの有効状態確認
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   flags = ae.sdk.AEGP_GetEffectFlags(effectH)
   is_active = (flags & ae.sdk.AEGP_EffectFlags_ACTIVE) != 0

エフェクトの並び替え
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # エフェクトを最上位に移動
   ae.sdk.AEGP_ReorderEffect(effectH, 0)

   # エフェクトを最下位に移動
   num_effects = ae.sdk.AEGP_GetLayerNumEffects(layerH)
   ae.sdk.AEGP_ReorderEffect(effectH, num_effects - 1)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       effectH = ae.sdk.AEGP_GetLayerEffectByIndex(plugin_id, layerH, effect_index)
   except Exception as e:
       print(f"エフェクトの取得に失敗しました: {e}")

関連項目
--------

- :doc:`AEGP_StreamSuite6` - パラメータストリーム管理（パラメータ値の取得/設定）
- :doc:`AEGP_DynamicStreamSuite4` - ストリーム動的管理
- :doc:`AEGP_LayerSuite9` - レイヤー管理
- :doc:`index` - 低レベルAPI概要
