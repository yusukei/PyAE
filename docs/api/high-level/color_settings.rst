カラー設定（OCIO）
==================

.. currentmodule:: ae.color_settings

``ae.color_settings`` モジュールは、After Effects のカラーマネジメント設定、
特に OpenColorIO (OCIO) 関連の情報を取得するためのAPIを提供します。

概要
----

After Effects 24.0 以降では、OpenColorIO ベースのカラーマネジメントが導入されています。
このモジュールを使用すると、現在のカラー設定を取得できます。

**主な機能**:

- OCIO の有効/無効確認
- OCIO 設定ファイルの情報取得
- 作業用カラースペースの取得
- ディスプレイ/ビュー設定の取得
- LUT 補間方式の取得

.. important::
   このモジュールの機能は **After Effects 24.0 以降** でのみ利用可能です。
   それ以前のバージョンでは、一部の関数がエラーを返す場合があります。

基本的な使い方
--------------

OCIO 設定の確認
~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # OCIO が有効かどうかを確認
   if ae.color_settings.is_ocio_enabled():
       print("OCIO カラーマネジメントが有効です")

       # 設定ファイルの情報を取得
       config_file = ae.color_settings.get_ocio_config_file()
       config_path = ae.color_settings.get_ocio_config_file_path()
       print(f"設定ファイル: {config_file}")
       print(f"フルパス: {config_path}")

       # 作業用カラースペースを取得
       working_space = ae.color_settings.get_ocio_working_color_space()
       print(f"作業用カラースペース: {working_space}")

       # ディスプレイ/ビューを取得
       display, view = ae.color_settings.get_ocio_display_view()
       print(f"ディスプレイ: {display}, ビュー: {view}")
   else:
       print("OCIO は無効です（従来のカラーマネジメントを使用）")

API リファレンス
----------------

OCIO 状態確認
~~~~~~~~~~~~~

.. function:: is_ocio_enabled() -> bool

   OCIO カラーマネジメントが有効かどうかを確認します。

   :return: OCIO が有効な場合は True
   :rtype: bool

   .. code-block:: python

      if ae.color_settings.is_ocio_enabled():
          print("OCIO is enabled")

OCIO 設定ファイル
~~~~~~~~~~~~~~~~~

.. function:: get_ocio_config_file() -> str

   OCIO 設定ファイル名を取得します（パスなし）。

   :return: 設定ファイル名（例: "aces_1.2_config.ocio"）
   :rtype: str

   .. code-block:: python

      config = ae.color_settings.get_ocio_config_file()
      # 例: "aces_1.2_config.ocio"

.. function:: get_ocio_config_file_path() -> str

   OCIO 設定ファイルのフルパスを取得します。

   :return: 設定ファイルのフルパス
   :rtype: str

   .. code-block:: python

      path = ae.color_settings.get_ocio_config_file_path()
      # 例: "C:/Program Files/Adobe/Adobe After Effects 2024/Support Files/OCIOConfigs/aces_1.2_config.ocio"

カラースペース情報
~~~~~~~~~~~~~~~~~~

.. function:: get_ocio_working_color_space() -> str

   OCIO 作業用カラースペース名を取得します。

   :return: カラースペース名（例: "ACEScg"）
   :rtype: str

   .. code-block:: python

      space = ae.color_settings.get_ocio_working_color_space()
      print(f"Working space: {space}")

.. function:: get_ocio_display_view() -> Tuple[str, str]

   OCIO ディスプレイとビューのカラースペース名を取得します。

   :return: (display, view) のタプル
   :rtype: Tuple[str, str]

   .. code-block:: python

      display, view = ae.color_settings.get_ocio_display_view()
      print(f"Display: {display}, View: {view}")

.. function:: get_working_color_space_id() -> bytes

   作業用カラースペースの GUID（16バイト）を取得します。

   :return: GUID（16バイト）
   :rtype: bytes

   .. code-block:: python

      guid = ae.color_settings.get_working_color_space_id()
      print(f"GUID: {guid.hex()}")

エフェクト設定
~~~~~~~~~~~~~~

.. function:: is_color_space_aware_effects_enabled() -> bool

   カラースペース対応エフェクトが有効かどうかを確認します。

   :return: 有効な場合は True
   :rtype: bool

LUT 設定
~~~~~~~~

.. function:: get_lut_interpolation_method() -> int

   LUT（Look-Up Table）補間方式を取得します。

   :return: LUT 補間方式（整数値）
   :rtype: int

輝度設定
~~~~~~~~

.. function:: get_graphics_white_luminance() -> int

   グラフィックホワイト輝度値を取得します。

   :return: ホワイト輝度値（cd/m²、例: 100, 300）
   :rtype: int

   .. code-block:: python

      luminance = ae.color_settings.get_graphics_white_luminance()
      print(f"White luminance: {luminance} cd/m²")

実用例
------

カラー設定のレポート
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def print_color_settings():
       """現在のカラー設定をレポート"""
       print("=" * 50)
       print("After Effects Color Settings Report")
       print("=" * 50)

       if not ae.color_settings.is_ocio_enabled():
           print("OCIO: Disabled (using legacy color management)")
           return

       print("OCIO: Enabled")
       print()

       # 設定ファイル
       print(f"Config File: {ae.color_settings.get_ocio_config_file()}")
       print(f"Config Path: {ae.color_settings.get_ocio_config_file_path()}")
       print()

       # カラースペース
       print(f"Working Space: {ae.color_settings.get_ocio_working_color_space()}")

       display, view = ae.color_settings.get_ocio_display_view()
       print(f"Display: {display}")
       print(f"View: {view}")
       print()

       # その他の設定
       print(f"Color-Space Aware Effects: {ae.color_settings.is_color_space_aware_effects_enabled()}")
       print(f"LUT Interpolation: {ae.color_settings.get_lut_interpolation_method()}")
       print(f"Graphics White Luminance: {ae.color_settings.get_graphics_white_luminance()} cd/m²")

   print_color_settings()

ACES ワークフローの確認
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def is_aces_workflow():
       """ACES ワークフローを使用しているか確認"""
       if not ae.color_settings.is_ocio_enabled():
           return False

       working_space = ae.color_settings.get_ocio_working_color_space()
       return "ACES" in working_space or "ACEScg" in working_space

   if is_aces_workflow():
       print("ACES ワークフローを使用中")
   else:
       print("ACES 以外のワークフローを使用中")

注意事項
--------

.. note::
   - このモジュールの機能は After Effects 24.0 以降でのみ利用可能です
   - OCIO が無効の場合、一部の関数は空文字列またはデフォルト値を返します
   - カラー設定は読み取り専用です。設定の変更は After Effects の UI から行ってください

.. warning::
   - After Effects 24.0 未満のバージョンでこれらの関数を呼び出すと、エラーが発生する可能性があります
   - 異なるプロジェクト間でカラー設定が異なる場合があるため、プロジェクトごとに確認してください

関連項目
--------

- :doc:`color_profile` - カラープロファイル
- :doc:`/api/low-level/AEGP_ColorSettingsSuite6` - 低レベルAPI
