Serialization（シリアライゼーション）
=====================================

.. currentmodule:: ae_serialize

PyAEのシリアライゼーションAPIは、After Effectsオブジェクトの
エクスポート/インポートを提供します。

概要
----

**主な機能**:

- プロジェクト全体のエクスポート/インポート
- コンポジション単位のエクスポート/インポート
- フッテージの個別エクスポート/インポート（Solid、File、Placeholderなど）
- レイヤー、エフェクト、プロパティの個別エクスポート
- bdata形式（IEEE 754 big-endian hex）のサポート
- 参照解決のためのコンテキスト管理

基本的な使い方
--------------

プロジェクトのエクスポート
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae_serialize
   import json

   # 現在のプロジェクトを辞書にエクスポート
   data = ae_serialize.project_to_dict()

   # JSONファイルに保存
   with open("project_backup.json", "w", encoding="utf-8") as f:
       json.dump(data, f, indent=2, ensure_ascii=False)

   print(f"エクスポート完了: {len(data['items'])} アイテム")

プロジェクトのインポート
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae_serialize
   import json

   # JSONファイルから読み込み
   with open("project_backup.json", "r", encoding="utf-8") as f:
       data = json.load(f)

   # 現在のプロジェクトにインポート
   context = ae_serialize.project_from_dict(data)

   print("インポート完了")

コンポジションのエクスポート/インポート
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae_serialize
   import json

   # アクティブなコンポジションをエクスポート
   comp = ae.Comp.get_active()
   data = ae_serialize.comp_to_dict(comp)

   with open("comp.json", "w", encoding="utf-8") as f:
       json.dump(data, f, indent=2, ensure_ascii=False)

   # 別のプロジェクトでインポート
   with open("comp.json", "r", encoding="utf-8") as f:
       data = json.load(f)

   new_comp = ae_serialize.comp_from_dict(data)
   print(f"作成: {new_comp.name}")

API リファレンス
----------------

SerializationContext
~~~~~~~~~~~~~~~~~~~~

.. class:: SerializationContext

   シリアライズ/デシリアライズ操作のコンテキスト管理クラス。
   IDからオブジェクトへのマッピングを管理し、参照解決に使用します。

   .. attribute:: id_to_item
      :type: Dict[int, Any]

      JSON IDからAEアイテムへのマッピング

   .. method:: register(json_id: int, ae_item: Any) -> None

      AEアイテムをJSON IDで登録

      :param json_id: JSONデータ内のID
      :param ae_item: After Effectsオブジェクト

   .. method:: resolve(json_id: int) -> Optional[Any]

      JSON IDから対応するAEアイテムを解決

      :param json_id: 解決するJSON ID
      :return: 対応するAEアイテム、見つからない場合はNone

   .. method:: generate_id() -> int

      新規アイテム用のユニークIDを生成

      :return: 新しいユニークID

プロジェクト関数
~~~~~~~~~~~~~~~~

.. function:: project_to_dict(project=None) -> Dict[str, Any]

   プロジェクトを辞書にエクスポート

   :param project: プロジェクトオブジェクト（省略時は現在のプロジェクト）
   :return: プロジェクトデータ（version, itemsを含む）
   :raises RuntimeError: プロジェクトが開かれていない場合

.. function:: project_from_dict(data, context=None) -> SerializationContext

   辞書から現在のプロジェクトにアイテムをインポート

   :param data: プロジェクト辞書
   :param context: SerializationContext（省略時は新規作成）
   :return: IDマッピングを含むSerializationContext

.. function:: project_update_from_dict(project, data, context=None) -> None

   辞書から既存プロジェクトを更新

   :param project: 更新するプロジェクト
   :param data: プロジェクト辞書
   :param context: SerializationContext

コンポジション関数
~~~~~~~~~~~~~~~~~~

.. function:: comp_to_dict(comp) -> Dict[str, Any]

   コンポジションを辞書にエクスポート

   :param comp: コンポジションオブジェクト
   :return: コンポジションデータ（レイヤーを含む）

.. function:: comp_from_dict(data, parent_folder=None, context=None) -> CompItem

   辞書から新規コンポジションを作成

   :param data: コンポジション辞書
   :param parent_folder: 親フォルダ（省略時はルート）
   :param context: SerializationContext
   :return: 新規作成されたコンポジション

.. function:: comp_update_from_dict(comp, data, context=None) -> None

   辞書から既存コンポジションを更新

   :param comp: 更新するコンポジション
   :param data: コンポジション辞書

レイヤー関数
~~~~~~~~~~~~

.. function:: layer_to_dict(layer, comp=None) -> Dict[str, Any]

   レイヤーを辞書にエクスポート

   :param layer: レイヤーオブジェクト
   :param comp: 親コンポジション（省略可）
   :return: レイヤーデータ

.. function:: layer_from_dict(comp, data, context=None) -> Optional[Layer]

   辞書からコンポジションに新規レイヤーを作成

   :param comp: 親コンポジション
   :param data: レイヤー辞書
   :param context: SerializationContext
   :return: 新規作成されたレイヤー、失敗時はNone

.. function:: layer_update_from_dict(layer, data, context=None) -> None

   辞書から既存レイヤーを更新

   :param layer: 更新するレイヤー
   :param data: レイヤー辞書

プロパティ関数
~~~~~~~~~~~~~~

.. function:: property_to_dict(prop) -> Dict[str, Any]

   プロパティを辞書にエクスポート

   :param prop: プロパティオブジェクト
   :return: プロパティデータ（値、キーフレーム、エクスプレッションを含む）

.. function:: property_update_from_dict(prop, data, context=None) -> None

   辞書からプロパティ値を復元

   :param prop: 更新するプロパティ
   :param data: プロパティ辞書

エフェクト関数
~~~~~~~~~~~~~~

.. function:: effect_to_dict(effect) -> Dict[str, Any]

   エフェクトを辞書にエクスポート

   :param effect: エフェクトオブジェクト
   :return: エフェクトデータ（パラメータを含む）

.. function:: effect_from_dict(layer, data, context=None) -> Optional[Effect]

   辞書からレイヤーにエフェクトを追加

   :param layer: エフェクトを追加するレイヤー
   :param data: エフェクト辞書
   :return: 新規作成されたエフェクト、失敗時はNone

.. function:: effect_update_from_dict(effect, data, context=None) -> None

   辞書からエフェクトパラメータを更新

   :param effect: 更新するエフェクト
   :param data: エフェクト辞書

フッテージ関数
~~~~~~~~~~~~~~

.. function:: footage_to_dict(footage) -> Dict[str, Any]

   フッテージを辞書にエクスポート

   :param footage: フッテージオブジェクト
   :return: フッテージデータ（タイプ、寸法、設定を含む）

   **エクスポートされるデータ**:

   - ``name``: フッテージ名
   - ``footage_type``: タイプ（"Solid", "File", "Placeholder", "Missing"）
   - ``width``, ``height``: 寸法
   - ``duration``: 再生時間
   - ``frame_rate``: フレームレート
   - ``file_path``: ファイルパス（File/Placeholder型の場合）
   - ``solid_color``: ソリッドカラー（Solid型の場合）
   - ``interpretation``: インタープリテーション設定
   - ``comment``, ``label``: メタデータ

   .. code-block:: python

      footage = ae.Project.get_current().items[0]
      data = ae_serialize.footage_to_dict(footage)
      # data = {
      #     "name": "video.mp4",
      #     "footage_type": "File",
      #     "width": 1920,
      #     "height": 1080,
      #     "file_path": "C:/path/to/video.mp4",
      #     ...
      # }

.. function:: footage_from_dict(data, parent_folder=None, context=None) -> Optional[Footage]

   辞書から新規フッテージを作成

   :param data: フッテージ辞書
   :param parent_folder: 親フォルダ（省略時はルート）
   :param context: SerializationContext
   :return: 新規作成されたフッテージ、失敗時はNone

   **サポートされるフッテージタイプ**:

   - ``Solid``: ``ae.Footage.solid()`` で作成
   - ``Placeholder``: ``ae.Footage.placeholder()`` で作成
   - ``File``: ``ae.Footage.from_file()`` で作成（ファイルが存在しない場合はPlaceholderにフォールバック）
   - ``Missing``: Placeholderとして作成

   .. code-block:: python

      # ソリッドフッテージを作成
      data = {
          "footage_type": "Solid",
          "name": "Red Solid",
          "width": 1920,
          "height": 1080,
          "solid_color": [1.0, 0.0, 0.0, 1.0]
      }
      footage = ae_serialize.footage_from_dict(data)

      # ファイルフッテージを作成
      data = {
          "footage_type": "File",
          "file_path": "C:/path/to/video.mp4"
      }
      footage = ae_serialize.footage_from_dict(data)

.. function:: footage_update_from_dict(footage, data, context=None) -> None

   辞書から既存フッテージを更新

   :param footage: 更新するフッテージ
   :param data: フッテージ辞書
   :param context: SerializationContext
   :raises RuntimeError: フッテージがプロジェクトに追加されていない場合

   **更新可能なプロパティ**:

   - ``name``: フッテージ名
   - ``comment``: コメント
   - ``label``: ラベル
   - ``solid_color``: ソリッドカラー（Solid型の場合のみ）
   - ``width``, ``height``: ソリッドの寸法（Solid型の場合のみ）
   - ``interpretation``: インタープリテーション設定

   .. note::
      ``footage_type`` は作成後に変更できません。

   .. code-block:: python

      footage = ae.Project.get_current().items[0]
      ae_serialize.footage_update_from_dict(footage, {
          "name": "New Name",
          "comment": "Updated comment",
          "solid_color": [0.5, 0.5, 0.5, 1.0]  # Solid型の場合のみ
      })

ユーティリティ関数
~~~~~~~~~~~~~~~~~~

.. function:: value_to_bdata(value) -> str

   値をbdata形式（IEEE 754 big-endian hex）に変換

   :param value: 変換する値（float、list、tupleなど）
   :return: bdata形式の16進数文字列

   .. code-block:: python

      bdata = ae_serialize.value_to_bdata([960.0, 540.0])
      # -> "44700000442a0000"

.. function:: bdata_to_value(bdata) -> Optional[Any]

   bdata形式を値に変換

   :param bdata: 16進数文字列
   :return: 変換された値、変換失敗時はNone

   .. code-block:: python

      value = ae_serialize.bdata_to_value("44700000442a0000")
      # -> [960.0, 540.0]

.. function:: bdata_to_path_vertices(bdata) -> Optional[Dict[str, Any]]

   bdata形式からシェイプパス頂点データに変換

   :param bdata: 16進数文字列
   :return: パス頂点データ（vertices, in_tangents, out_tangents, closed）

.. function:: item_type_to_string(item_type) -> str

   アイテムタイプを文字列に変換

   :param item_type: ItemType値
   :return: 文字列表現（"Comp", "Folder", "Footage"など）

.. function:: layer_type_to_string(layer_type) -> str

   レイヤータイプを文字列に変換

   :param layer_type: LayerType値
   :return: 文字列表現（"AV", "Text", "Shape"など）

使用例
------

プロジェクトの差分バックアップ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae_serialize
   import json
   from datetime import datetime
   import os

   def backup_project(backup_dir="backups"):
       """プロジェクトをタイムスタンプ付きでバックアップ"""
       os.makedirs(backup_dir, exist_ok=True)

       data = ae_serialize.project_to_dict()

       timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
       filename = f"project_{timestamp}.json"
       filepath = os.path.join(backup_dir, filename)

       with open(filepath, "w", encoding="utf-8") as f:
           json.dump(data, f, indent=2, ensure_ascii=False)

       print(f"バックアップ完了: {filepath}")
       return filepath

   backup_project()

選択レイヤーのエクスポート
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae_serialize
   import json

   def export_selected_layers(filename="selected_layers.json"):
       """選択中のレイヤーをエクスポート"""
       comp = ae.Comp.get_active()
       if not comp:
           print("アクティブなコンポジションがありません")
           return

       selected = comp.selected_layers
       if not selected:
           print("レイヤーが選択されていません")
           return

       layers_data = [ae_serialize.layer_to_dict(layer) for layer in selected]

       with open(filename, "w", encoding="utf-8") as f:
           json.dump(layers_data, f, indent=2, ensure_ascii=False)

       print(f"{len(layers_data)}個のレイヤーをエクスポート: {filename}")

   export_selected_layers()

エフェクトプリセットの保存/適用
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae_serialize
   import json

   def save_effect_preset(layer, effect_index, filename):
       """エフェクトをプリセットとして保存"""
       effect = layer.effects[effect_index]
       data = ae_serialize.effect_to_dict(effect)

       with open(filename, "w", encoding="utf-8") as f:
           json.dump(data, f, indent=2, ensure_ascii=False)

       print(f"プリセット保存: {effect.name} -> {filename}")

   def apply_effect_preset(layer, filename):
       """プリセットをレイヤーに適用"""
       with open(filename, "r", encoding="utf-8") as f:
           data = json.load(f)

       effect = ae_serialize.effect_from_dict(layer, data)
       if effect:
           print(f"プリセット適用: {effect.name}")
       return effect

   # 使用例
   comp = ae.Comp.get_active()
   layer = comp.layers[0]

   # 保存
   save_effect_preset(layer, 0, "blur_preset.json")

   # 適用
   apply_effect_preset(layer, "blur_preset.json")

フッテージのエクスポート/インポート
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae_serialize
   import json

   def export_footage_list(filename="footage_list.json"):
       """プロジェクト内のフッテージをエクスポート"""
       project = ae.Project.get_current()

       footage_items = []
       for item in project.items:
           if item.type == ae.ItemType.Footage:
               data = ae_serialize.footage_to_dict(item)
               footage_items.append(data)

       with open(filename, "w", encoding="utf-8") as f:
           json.dump(footage_items, f, indent=2, ensure_ascii=False)

       print(f"{len(footage_items)}個のフッテージをエクスポート: {filename}")

   def import_footage_list(filename="footage_list.json"):
       """フッテージリストをインポート"""
       with open(filename, "r", encoding="utf-8") as f:
           footage_items = json.load(f)

       created = []
       for data in footage_items:
           footage = ae_serialize.footage_from_dict(data)
           if footage:
               created.append(footage)

       print(f"{len(created)}個のフッテージをインポート")
       return created

   # 使用例
   export_footage_list()
   import_footage_list()

ソリッドカラーの一括更新
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae_serialize

   def update_all_solids_color(new_color):
       """すべてのソリッドフッテージの色を更新"""
       project = ae.Project.get_current()

       updated = 0
       for item in project.items:
           if item.type != ae.ItemType.Footage:
               continue

           data = ae_serialize.footage_to_dict(item)
           if data.get("footage_type") == "Solid":
               ae_serialize.footage_update_from_dict(item, {
                   "solid_color": new_color
               })
               updated += 1

       print(f"{updated}個のソリッドを更新しました")

   # すべてのソリッドをグレーに変更
   update_all_solids_color([0.5, 0.5, 0.5, 1.0])

コンポジションのクローン
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae_serialize
   import json

   # 元のコンポジションを取得
   original = ae.Comp.get_active()

   # エクスポート
   data = ae_serialize.comp_to_dict(original)

   # 名前を変更して新規作成
   data["name"] = original.name + "_copy"
   clone = ae_serialize.comp_from_dict(data)
   print(f"Created: {clone.name}")

アニメーションのレイヤー間コピー
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae_serialize

   comp = ae.Comp.get_active()
   source_layer = comp.layers[0]
   target_layer = comp.layers[1]

   # Positionプロパティをコピー
   source_pos = source_layer.get_property("ADBE Position")
   pos_data = ae_serialize.property_to_dict(source_pos)

   target_pos = target_layer.get_property("ADBE Position")
   ae_serialize.property_update_from_dict(target_pos, pos_data)

データフォーマット
------------------

コンポジションデータ構造
~~~~~~~~~~~~~~~~~~~~~~~~

``comp_to_dict()`` が出力するデータ構造:

.. code-block:: python

   {
       "name": "Comp Name",
       "width": 1920,
       "height": 1080,
       "pixel_aspect": 1.0,
       "duration": 10.0,
       "frame_rate": 30.0,
       "background_color": [0, 0, 0],
       "work_area_start": 0.0,
       "work_area_duration": 10.0,
       "layers": [
           # レイヤーデータの配列
       ]
   }

レイヤーデータ構造
~~~~~~~~~~~~~~~~~~

``layer_to_dict()`` が出力するデータ構造:

.. code-block:: python

   {
       "name": "Layer Name",
       "index": 1,
       "type": "solid",  # "solid", "text", "shape", "camera", "light", "av", "null"
       "active": True,
       "in_point": 0.0,
       "out_point": 10.0,
       "start_time": 0.0,
       "stretch": 1.0,
       "parent_index": None,
       "properties": {
           # プロパティツリー
       },
       "effects": [
           # エフェクトの配列
       ],
       "masks": [
           # マスクの配列
       ]
   }

プロパティデータ構造
~~~~~~~~~~~~~~~~~~~~

``property_to_dict()`` が出力するデータ構造:

.. code-block:: python

   {
       "match_name": "ADBE Position",
       "type": "property",  # "property" or "group"
       "bdata": "404e0000000000004050000000000000",  # IEEE 754 big-endian hex
       "keyframes": [
           {
               "time": 0.0,
               "bdata": "...",
               "in_interpolation": 1,
               "out_interpolation": 1
           }
       ],
       "expression": None  # または式の文字列
   }

bdata形式の詳細
~~~~~~~~~~~~~~~~

プロパティ値は ``bdata`` フィールドに IEEE 754 big-endian hex 形式で保存されます。
これにより浮動小数点の精度が完全に保持されます。

.. code-block:: text

   # 単一のdouble: 16文字 (8バイト)
   "3ff0000000000000"  = 1.0

   # 2D値 (Position等): 32文字 (16バイト)
   "408e000000000000408f400000000000"  = [960.0, 1000.0]

   # 3D値: 48文字 (24バイト)
   "408e000000000000408f4000000000000000000000000000"  = [960.0, 1000.0, 0.0]

   # 4D値 (Color): 64文字 (32バイト)
   "3ff00000000000003ff00000000000000000000000000000..."  = [1.0, 1.0, 0.0, ...]

変換にはユーティリティ関数 ``value_to_bdata()`` / ``bdata_to_value()`` を使用します。

注意事項
--------

.. note::
   - シリアライズされたデータは、異なるAfter Effectsプロジェクト間で
     移動可能ですが、参照（ソースアイテムなど）は解決できない場合があります
   - bdata形式は浮動小数点の精度を完全に保持します
   - 大きなプロジェクトのエクスポートには時間がかかる場合があります

.. warning::
   - インポート時に同名のアイテムが存在する場合、新規作成されます
   - フッテージへの参照はパスベースで解決されるため、
     ファイルパスが変更されていると解決できません
   - ``ae.Comp.from_dict()`` にはpybind11のキャスト問題があります。
     代わりに ``ae_serialize.comp_from_dict()`` を使用してください

特殊処理されるプロパティ
~~~~~~~~~~~~~~~~~~~~~~~~

インポート時に一部のプロパティは通常の復元処理をスキップし、専用APIで処理されます。

**専用処理で復元されるプロパティ**:

.. list-table::
   :header-rows: 1
   :widths: 30 30 40

   * - プロパティ
     - 専用処理
     - 説明
   * - ``ADBE Marker``
     - ``add_marker()``
     - MarkerSuite経由で復元
   * - ``ADBE Mask Parade``
     - ``add_mask()``
     - マスク専用APIで復元
   * - ``ADBE Effect Parade``
     - ``effect_from_dict()``
     - エフェクト専用APIで復元
   * - ``ADBE Text Document``
     - ``set_source_text()``
     - テキストドキュメント専用APIで復元

**フラグ設定後に復元されるプロパティ**:

以下のプロパティは、レイヤー設定を先に適用した後に通常のプロパティ復元で処理されます。

.. list-table::
   :header-rows: 1
   :widths: 35 25 40

   * - プロパティ
     - 必要なフラグ
     - 備考
   * - ``ADBE Time Remapping``
     - ``time_remapping_enabled = True``
     - フラグ設定でプロパティがアクティブになる
   * - ``ADBE Material Options Group``
     - ``is_3d = True``
     - 3Dレイヤー専用プロパティ
   * - ``ADBE Extrsn Options Group``
     - ``is_3d = True``
     - 3Dレイヤー専用プロパティ（押し出しオプション）
   * - ``ADBE Plane Options Group``
     - ``is_3d = True``
     - 3Dレイヤー専用プロパティ（平面オプション）

Time Remapping の制限事項
~~~~~~~~~~~~~~~~~~~~~~~~~

Time Remappingは「時間的なソースコンテンツを持つレイヤー」でのみ有効です。
これはAfter Effects SDKの制限ではなく、After Effectsアプリケーション自体の仕様です。

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - レイヤータイプ
     - 対応
     - 理由
   * - フッテージ（動画）
     - 可
     - 時間軸を持つコンテンツ
   * - ネストコンプ
     - 可
     - 内部にタイムラインを持つ
   * - 画像シーケンス
     - 可
     - 連番として時間を持つ
   * - ソリッド
     - 不可
     - 時間的な変化がない静的コンテンツ
   * - テキスト
     - 不可
     - 時間ソースなし
   * - シェイプ
     - 不可
     - 時間ソースなし
   * - カメラ/ライト
     - 不可
     - 視覚的なソースがない

.. note::
   ``AEGP_SetLayerFlag(AEGP_LayerFlag_TIME_REMAPPING)`` は非対応レイヤーでも
   エラーを返しませんが、フラグは無視されます。
   また、SDK経由でのフラグ設定は反映されない場合があります（フラグの読み取りは正常に動作）。

復元不可能なプロパティ
~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - プロパティ
     - 状況
     - 理由
   * - ``ADBE MTrackers``
     - 復元不可
     - ``AEGP_TrackerSuite1`` は ``PT_TrackingContextPtr`` を必要とし、
       トラッカープラグインコールバック内でのみ利用可能。
       AEGPプラグインからは使用不可。エクスポート（読み取り）は可能。

bdata形式の対応状況
~~~~~~~~~~~~~~~~~~~

``get_raw_bytes()`` / ``set_raw_bytes()`` がサポートするストリームタイプ:

.. list-table::
   :header-rows: 1
   :widths: 35 10 55

   * - ストリームタイプ
     - 対応
     - フォーマット
   * - OneDim
     - 可
     - 8バイト IEEE 754 big-endian
   * - TwoDim / TwoDimSpatial
     - 可
     - 16バイト (2 x double)
   * - ThreeDim / ThreeDimSpatial
     - 可
     - 24バイト (3 x double)
   * - Color
     - 可
     - 32バイト (4 x double)
   * - Arbitrary (ARB)
     - 可
     - 可変長バイナリ
   * - TextDocument
     - 可
     - UTF-16LE バイナリ
   * - Mask
     - 可
     - 頂点データバイナリ
   * - LayerID
     - 可
     - 4バイト int
   * - MaskID
     - 可
     - 4バイト int
   * - Marker
     - 不可
     - 空を返す（別途 ``add_marker()`` で処理）
   * - NoData
     - 不可
     - 空を返す（プロパティグループ等、値を持たないストリーム）

関連項目
--------

- :doc:`comp` - コンポジション（to_dict, from_dict）
- :doc:`layer` - レイヤー（to_dict, update_from_dict）
- :doc:`footage` - フッテージ（to_dict, from_dict, update_from_dict）
- :doc:`project` - プロジェクト
