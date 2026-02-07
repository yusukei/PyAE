Effect
======

.. currentmodule:: ae

Effectクラスは、レイヤーに適用されたエフェクトを表します。

概要
----

``Effect`` クラスは、After Effectsのエフェクト操作のためのAPIを提供します。
エフェクトの追加、パラメータの変更、アニメーションの設定などが可能です。

**特徴**:

- エフェクトパラメータへのアクセス
- キーフレームアニメーションの設定
- マスク参照の管理（AE 13.0+）
- シリアライゼーション（エクスポート/インポート）

**基本的なインポート**:

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layer(0)

   # エフェクトを追加
   blur = layer.add_effect("ADBE Gaussian Blur 2")

クラスリファレンス
------------------

.. class:: Effect

   レイヤーに適用されたエフェクトを表すクラス。

   エフェクトのパラメータにアクセスし、値の取得・設定、アニメーションが可能です。

プロパティ一覧
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 25 20 55

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - エフェクトが有効かどうか（読み取り専用）
   * - ``name``
     - str
     - エフェクト表示名（読み取り専用）
   * - ``match_name``
     - str
     - マッチ名（固有識別子、読み取り専用）
   * - ``index``
     - int
     - エフェクトインデックス（0から開始、読み取り専用）
   * - ``enabled``
     - bool
     - 有効/無効状態（読み書き可能）
   * - ``num_properties``
     - int
     - パラメータ数（読み取り専用）
   * - ``properties``
     - List[Property]
     - 全パラメータのリスト（読み取り専用）
   * - ``stream_name``
     - str
     - エフェクトストリーム名（読み取り専用）
   * - ``num_masks``
     - int
     - 割り当てられたマスク数（読み取り専用、AE 13.0+）

メソッド一覧
~~~~~~~~~~~~

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``property(name) -> Property``
     - 名前でパラメータを取得
   * - ``property_by_index(index) -> Property``
     - インデックスでパラメータを取得
   * - ``set_name(name)``
     - エフェクト名を設定
   * - ``duplicate() -> Effect``
     - エフェクトを複製
   * - ``delete()``
     - エフェクトを削除
   * - ``get_mask_id(mask_index) -> int``
     - 指定インデックスのマスクIDを取得（AE 13.0+）
   * - ``add_mask(mask_id) -> int``
     - エフェクトにマスクを追加（AE 13.0+）
   * - ``remove_mask(mask_id)``
     - エフェクトからマスクを削除（AE 13.0+）
   * - ``update_params_ui()``
     - パラメータ変更後にUI更新を通知
   * - ``to_dict() -> dict``
     - エフェクトを辞書にエクスポート
   * - ``update_from_dict(data)``
     - 辞書データでエフェクトを更新

Pythonプロトコルメソッド
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__repr__()``
     - 文字列表現を返す（例: ``<Effect: 'Name' match='ADBE ...'>``）
   * - ``__bool__()``
     - ``valid`` プロパティと同等。有効なら ``True``

コレクションプロトコル
^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - メソッド
     - 説明
   * - ``__len__()``
     - パラメータ数を返す（``num_properties`` と同等）
   * - ``__iter__()``
     - パラメータのイテレーションをサポート
   * - ``__getitem__(key)``
     - int（インデックス）または str（名前）でパラメータを取得
   * - ``__contains__(name)``
     - 名前でパラメータの存在を確認

プロパティ
~~~~~~~~~~

.. attribute:: Effect.valid
   :type: bool

   エフェクトが有効かどうか。

   **例**:

   .. code-block:: python

      if effect.valid:
          print(f"エフェクト名: {effect.name}")

.. attribute:: Effect.enabled
   :type: bool

   エフェクトの有効/無効状態。

   **例**:

   .. code-block:: python

      effect.enabled = False  # エフェクトを無効化

.. attribute:: Effect.match_name
   :type: str

   エフェクトのマッチ名（固有識別子）。

   **例**:

   .. code-block:: python

      print(f"マッチ名: {effect.match_name}")
      # 例: "ADBE Gaussian Blur 2"

エフェクトパラメータ
--------------------

エフェクトの各パラメータは :class:`Property` クラスとして返されます。
``is_effect_param`` が ``True`` のプロパティはエフェクトパラメータであり、
``index`` でパラメータインデックス（0-based）を取得できます。

詳細は :doc:`property` を参照してください。

モジュール関数
--------------

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - 関数
     - 説明
   * - ``get_installed_effects() -> List[dict]``
     - インストール済みエフェクトのリストを取得

使用例
------

エフェクトの追加
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   layer = comp.layers[0]

   # エフェクトを追加
   blur = layer.add_effect("ADBE Gaussian Blur 2")

   # エフェクト情報
   print(f"エフェクト名: {blur.name}")
   print(f"マッチ名: {blur.match_name}")

パラメータの操作
~~~~~~~~~~~~~~~~

.. code-block:: python

   # パラメータを取得
   blurriness = blur.property("Blurriness")

   # 値を設定
   blurriness.value = 50.0

   # 値を取得
   print(f"ブラー量: {blurriness.value}")

キーフレームアニメーション
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # ブラー量をアニメーション
   blurriness = blur.property("Blurriness")
   blurriness.add_keyframe(0.0, 0.0)
   blurriness.add_keyframe(3.0, 100.0)

パラメータ変更後のUI更新
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # パラメータを変更
   blurriness = blur.property("Blurriness")
   blurriness.value = 50.0

   # エフェクトUIの更新を通知
   blur.update_params_ui()

コレクションプロトコル
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   # パラメータ数
   print(len(effect))

   # インデックスでアクセス
   param = effect[0]

   # 名前でアクセス
   param = effect["Opacity"]

   # パラメータの存在確認
   if "Opacity" in effect:
       opacity = effect["Opacity"]

   # イテレーション
   for param in effect:
       print(param)

よく使うエフェクトのマッチ名
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   effects = {
       "Gaussian Blur": "ADBE Gaussian Blur 2",
       "Glow": "ADBE Glo2",
       "Levels": "ADBE Pro Levels2",
       "Curves": "ADBE CurvesCustom",
       "Hue/Saturation": "ADBE HUE SATURATION",
       "Transform": "ADBE Geometry2",
       "Drop Shadow": "ADBE Drop Shadow",
       "Fill": "ADBE Fill",
   }

注意事項
--------

.. note::
   - ``index`` は 1 から始まります
   - マッチ名はエフェクトのバージョンに依存する場合があります
   - マスク操作は After Effects 13.0 以降でサポートされています

.. warning::
   - 削除されたエフェクトへの参照は無効になります
   - ``valid`` プロパティで有効性を確認することを推奨します

関連項目
--------

- :doc:`layer` - レイヤー
- :doc:`property` - プロパティ
- :doc:`/api/low-level/AEGP_EffectSuite5` - 低レベルAPI
