ColorProfile（カラープロファイル）
===================================

.. currentmodule:: ae

ColorProfileクラスは、After Effectsのカラープロファイル管理を提供します。
作業用カラースペースの取得・設定、ICCプロファイルの読み書きなどが可能です。

概要
----

**主な機能**:

- コンポジションの作業用カラースペースの取得・設定
- ICCプロファイルデータの読み書き
- プロファイル情報（説明、ガンマ、RGBかどうか）の取得
- OCIO（OpenColorIO）設定へのアクセス（AE 24.0+）

基本的な使い方
--------------

カラープロファイルの取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # アクティブなコンポジションを取得
   comp = ae.Comp.get_active()

   # カラープロファイルを取得
   profile = comp.color_profile

   # プロファイル情報を表示
   print(f"プロファイル: {profile.description}")
   print(f"ガンマ: {profile.gamma}")
   print(f"RGB: {profile.is_rgb}")

ICCプロファイルのエクスポート
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   comp = ae.Comp.get_active()
   profile = comp.color_profile

   # ICCデータをエクスポート
   icc_data = profile.to_icc_data()

   # ファイルに保存
   with open("working_space.icc", "wb") as f:
       f.write(icc_data)

   print(f"ICCプロファイルを保存しました ({len(icc_data)} bytes)")

ICCプロファイルの適用
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # ICCファイルを読み込み
   with open("AdobeRGB1998.icc", "rb") as f:
       icc_data = f.read()

   # ColorProfileを作成
   profile = ae.ColorProfile.from_icc_data(icc_data)

   # プロファイル情報を確認
   print(f"プロファイル: {profile.description}")
   print(f"RGB: {profile.is_rgb}")

   # コンポジションに適用
   comp = ae.Comp.get_active()
   comp.color_profile = profile

   # または直接ICCデータを設定
   comp.color_profile = icc_data

ColorProfile API
----------------

クラス属性
~~~~~~~~~~

.. attribute:: valid
   :type: bool

   カラープロファイルが有効かどうか

.. attribute:: description
   :type: str

   カラープロファイルの説明（人間が読める形式）

   例: ``"sRGB IEC61966-2.1"``

.. attribute:: gamma
   :type: float

   おおよそのガンマ値

   例: ``2.2``

.. attribute:: is_rgb
   :type: bool

   RGBカラープロファイルかどうか

メソッド
~~~~~~~~

.. method:: to_icc_data() -> bytes

   カラープロファイルをICCプロファイルデータとしてエクスポート

   :return: ICCプロファイルデータ
   :rtype: bytes

.. staticmethod:: from_icc_data(icc_data: bytes) -> ColorProfile

   ICCプロファイルデータからColorProfileを作成

   :param icc_data: ICCプロファイルデータ
   :type icc_data: bytes
   :return: 新しいColorProfileオブジェクト
   :rtype: ColorProfile

.. staticmethod:: from_comp(comp_handle: int) -> ColorProfile

   コンポジションの作業用カラースペースからColorProfileを取得

   :param comp_handle: コンポジションハンドル（内部使用）
   :type comp_handle: int
   :return: コンポジションのカラープロファイル
   :rtype: ColorProfile

Comp.color_profile プロパティ
-----------------------------

.. code-block:: python

   # 取得
   profile = comp.color_profile

   # 設定（ColorProfileオブジェクト）
   comp.color_profile = profile

   # 設定（ICCデータ直接）
   with open("profile.icc", "rb") as f:
       comp.color_profile = f.read()

ae.color_settings サブモジュール
--------------------------------

OCIO（OpenColorIO）設定へのアクセスを提供します。

.. note::
   OCIO関連の機能は **After Effects 24.0以降** でのみ利用可能です。

OCIO状態の確認
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # OCIOが有効か確認
   if ae.color_settings.is_ocio_enabled():
       print("OCIOカラーマネジメントが有効です")

       # OCIO設定情報を取得
       config_file = ae.color_settings.get_ocio_config_file()
       config_path = ae.color_settings.get_ocio_config_file_path()
       working_cs = ae.color_settings.get_ocio_working_color_space()
       display, view = ae.color_settings.get_ocio_display_view()

       print(f"設定ファイル: {config_file}")
       print(f"設定パス: {config_path}")
       print(f"作業用カラースペース: {working_cs}")
       print(f"ディスプレイ: {display}, ビュー: {view}")

color_settings API
~~~~~~~~~~~~~~~~~~

.. function:: is_ocio_enabled() -> bool

   OCIOカラーマネジメントが有効かどうかを確認

.. function:: get_ocio_config_file() -> str

   OCIO設定ファイル名を取得

.. function:: get_ocio_config_file_path() -> str

   OCIO設定ファイルのフルパスを取得

.. function:: get_ocio_working_color_space() -> str

   OCIO作業用カラースペース名を取得

.. function:: get_ocio_display_view() -> Tuple[str, str]

   OCIOディスプレイとビューのカラースペース名を取得

   :return: ``(display, view)`` のタプル

.. function:: is_color_space_aware_effects_enabled() -> bool

   カラースペース対応エフェクトが有効かどうかを確認

.. function:: get_lut_interpolation_method() -> int

   LUT補間方式を取得

.. function:: get_graphics_white_luminance() -> int

   グラフィックホワイト輝度値を取得（cd/m²）

.. function:: get_working_color_space_id() -> bytes

   作業用カラースペースのGUID（16バイト）を取得

使用例
------

カラープロファイル情報の一括表示
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_color_profile_info():
       """カラープロファイル情報を一括表示"""
       comp = ae.Comp.get_active()
       if not comp:
           print("アクティブなコンポジションがありません")
           return

       profile = comp.color_profile

       print("=" * 50)
       print("カラープロファイル情報")
       print("=" * 50)
       print(f"コンポジション: {comp.name}")
       print(f"プロファイル: {profile.description}")
       print(f"ガンマ: {profile.gamma}")
       print(f"RGB: {'Yes' if profile.is_rgb else 'No'}")
       print("=" * 50)

       # OCIO情報（AE 24.0+）
       try:
           if ae.color_settings.is_ocio_enabled():
               print("\nOCIO設定:")
               print(f"  設定ファイル: {ae.color_settings.get_ocio_config_file()}")
               print(f"  作業用CS: {ae.color_settings.get_ocio_working_color_space()}")
               display, view = ae.color_settings.get_ocio_display_view()
               print(f"  ディスプレイ: {display}")
               print(f"  ビュー: {view}")
       except RuntimeError:
           print("\nOCIO: 利用不可（AE 24.0以降が必要）")

   dump_color_profile_info()

複数コンポジションのカラースペース統一
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def unify_color_spaces(target_profile_path: str):
       """全コンポジションのカラースペースを統一"""
       # ICCプロファイルを読み込み
       with open(target_profile_path, "rb") as f:
           icc_data = f.read()

       profile = ae.ColorProfile.from_icc_data(icc_data)
       print(f"ターゲットプロファイル: {profile.description}")

       if not profile.is_rgb:
           print("警告: RGBプロファイルではありません")
           return

       # プロジェクト内の全コンポジションを処理
       project = ae.Project.get_current()
       count = 0

       with ae.UndoGroup("カラースペース統一"):
           for item in project.items:
               if isinstance(item, ae.CompItem):
                   comp = ae.Comp(item._handle)
                   old_profile = comp.color_profile
                   print(f"{comp.name}: {old_profile.description} -> {profile.description}")
                   comp.color_profile = profile
                   count += 1

       print(f"\n{count} 個のコンポジションを更新しました")

   # 使用例
   unify_color_spaces("C:/Profiles/ACEScg.icc")

関連項目
--------

- :doc:`comp` - コンポジション管理
- :doc:`/api/low-level/AEGP_ColorSettingsSuite6` - 低レベルAPIリファレンス
