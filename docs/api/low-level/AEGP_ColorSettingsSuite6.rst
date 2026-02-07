AEGP_ColorSettingsSuite6
========================

.. currentmodule:: ae.sdk

AEGP_ColorSettingsSuite6は、After Effectsのカラー設定、カラープロファイル、OCIO（OpenColorIO）カラーマネジメント、カラースペース変換を管理するためのSDK APIです。

概要
----

**実装状況**: 19/20関数実装（``AEGP_GetBlendingTables`` を除く）

AEGP_ColorSettingsSuite6は以下の機能を提供します:

- カラープロファイルの取得と管理
- ICCプロファイルとの相互変換
- カラースペース変換
- OCIO（OpenColorIO）カラーマネジメント設定（AE 24.0+）
- 作業用カラースペースの設定と取得
- ガンマ情報の取得

基本概念
--------

カラープロファイル (AEGP_ColorProfileP)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

カラープロファイルは、After Effectsで色をどのように解釈・表示するかを定義します。PyAEではポインタ値（整数）として扱われます。

.. important::
   - カラープロファイルは ``AEGP_GetNewWorkingSpaceColorProfile`` や ``AEGP_GetNewColorProfileFromICCProfile`` で取得します
   - 使用後は **必ず** ``AEGP_DisposeColorProfile`` で解放する必要があります
   - 解放を忘れるとメモリリークが発生します

.. code-block:: python

   # カラープロファイルの基本的な使い方
   plugin_id = ae.sdk.AEGP_GetPluginID()
   compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)

   # プロファイルを取得
   profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

   try:
       # プロファイル情報を使用
       desc = ae.sdk.AEGP_GetNewColorProfileDescription(profile)
       gamma = ae.sdk.AEGP_GetColorProfileApproximateGamma(profile)
       print(f"プロファイル: {desc}, ガンマ: {gamma}")
   finally:
       # 必ず解放
       ae.sdk.AEGP_DisposeColorProfile(profile)

ICCプロファイル
~~~~~~~~~~~~~~~

ICC（International Color Consortium）プロファイルは、デバイス間で一貫した色再現を実現するための標準形式です。

.. tip::
   - ICCプロファイルは ``bytes`` 型としてPythonで扱われます
   - ``AEGP_GetNewColorProfileFromICCProfile`` でICCデータからカラープロファイルを作成
   - ``AEGP_GetNewICCProfileFromColorProfile`` でカラープロファイルからICCデータを抽出

OCIO（OpenColorIO）
~~~~~~~~~~~~~~~~~~~

OCIOは、映像制作における高度なカラーマネジメントを提供するオープンソースのカラー管理システムです。

.. note::
   OCIO関連の機能は **After Effects 24.0以降** でのみ利用可能です（``kAEGPColorSettingsSuiteVersion6``）。

API リファレンス
----------------

カラースペース変換
~~~~~~~~~~~~~~~~~~

.. function:: AEGP_DoesViewHaveColorSpaceXform(viewP: int) -> bool

   ビューにカラースペース変換が適用されているかを確認します。

   :param viewP: ビューポインタ（``AEGP_ItemViewP``）
   :type viewP: int
   :return: カラースペース変換が適用されている場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      has_xform = ae.sdk.AEGP_DoesViewHaveColorSpaceXform(viewP)
      if has_xform:
          print("ビューにカラースペース変換が適用されています")

.. function:: AEGP_XformWorkingToViewColorSpace(viewP: int, srcH: int, dstH: int) -> None

   作業用カラースペースからビューカラースペースへピクセルデータを変換します。

   :param viewP: ビューポインタ（``AEGP_ItemViewP``）
   :type viewP: int
   :param srcH: ソース画像ハンドル（``AEGP_WorldH``）
   :type srcH: int
   :param dstH: 変換先画像ハンドル（``AEGP_WorldH``）
   :type dstH: int

   .. note::
      ソースと変換先は同じハンドルを指定できます（インプレース変換）。

   **例**:

   .. code-block:: python

      # 作業用カラースペースからビューカラースペースに変換
      ae.sdk.AEGP_XformWorkingToViewColorSpace(viewP, worldH, worldH)

カラープロファイルの取得と管理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewWorkingSpaceColorProfile(compH: int) -> int

   コンポジションの作業用カラースペースのカラープロファイルを取得します。

   :param compH: コンポジションハンドル
   :type compH: int
   :return: カラープロファイルポインタ
   :rtype: int

   .. warning::
      取得したカラープロファイルは **必ず** ``AEGP_DisposeColorProfile`` で解放してください。

   **例**:

   .. code-block:: python

      compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)
      profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

      try:
          # プロファイル情報を使用
          desc = ae.sdk.AEGP_GetNewColorProfileDescription(profile)
          print(f"作業用カラースペース: {desc}")
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)

.. function:: AEGP_GetNewColorProfileFromICCProfile(icc_data: bytes) -> int

   ICCプロファイルデータからカラープロファイルを作成します。

   :param icc_data: ICCプロファイルデータ
   :type icc_data: bytes
   :return: カラープロファイルポインタ
   :rtype: int

   .. warning::
      取得したカラープロファイルは **必ず** ``AEGP_DisposeColorProfile`` で解放してください。

   **例**:

   .. code-block:: python

      # ICCプロファイルファイルを読み込み
      with open("profile.icc", "rb") as f:
          icc_data = f.read()

      profile = ae.sdk.AEGP_GetNewColorProfileFromICCProfile(icc_data)

      try:
          # プロファイルを使用
          is_rgb = ae.sdk.AEGP_IsRGBColorProfile(profile)
          print(f"RGBプロファイル: {is_rgb}")
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)

.. function:: AEGP_GetNewICCProfileFromColorProfile(colorProfileP: int) -> bytes

   カラープロファイルからICCプロファイルデータを抽出します。

   :param colorProfileP: カラープロファイルポインタ
   :type colorProfileP: int
   :return: ICCプロファイルデータ
   :rtype: bytes

   **例**:

   .. code-block:: python

      # 作業用カラースペースのICCプロファイルを保存
      compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)
      profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

      try:
          icc_data = ae.sdk.AEGP_GetNewICCProfileFromColorProfile(profile)

          # ファイルに保存
          with open("working_space.icc", "wb") as f:
              f.write(icc_data)

          print(f"ICCプロファイルを保存しました ({len(icc_data)} bytes)")
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)

.. function:: AEGP_DisposeColorProfile(colorProfileP: int) -> None

   カラープロファイルを解放します。

   :param colorProfileP: カラープロファイルポインタ（0の場合は何もしない）
   :type colorProfileP: int

   .. important::
      以下の関数で取得したカラープロファイルは **必ず** この関数で解放してください:

      - ``AEGP_GetNewWorkingSpaceColorProfile``
      - ``AEGP_GetNewColorProfileFromICCProfile``

   **例**:

   .. code-block:: python

      profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
      try:
          # プロファイル使用
          pass
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)

カラープロファイル情報の取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetNewColorProfileDescription(colorProfileP: int) -> str

   カラープロファイルの人間が読める説明を取得します。

   :param colorProfileP: カラープロファイルポインタ
   :type colorProfileP: int
   :return: プロファイルの説明（UTF-8文字列）
   :rtype: str

   **例**:

   .. code-block:: python

      profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
      try:
          desc = ae.sdk.AEGP_GetNewColorProfileDescription(profile)
          print(f"カラープロファイル: {desc}")
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)

.. function:: AEGP_GetColorProfileApproximateGamma(colorProfileP: int) -> float

   カラープロファイルのおおよそのガンマ値を取得します。

   :param colorProfileP: カラープロファイルポインタ
   :type colorProfileP: int
   :return: ガンマ値（例: 2.2, 1.8）
   :rtype: float

   **例**:

   .. code-block:: python

      profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
      try:
          gamma = ae.sdk.AEGP_GetColorProfileApproximateGamma(profile)
          print(f"ガンマ値: {gamma}")
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)

.. function:: AEGP_IsRGBColorProfile(colorProfileP: int) -> bool

   カラープロファイルがRGBカラープロファイルかどうかを確認します。

   :param colorProfileP: カラープロファイルポインタ
   :type colorProfileP: int
   :return: RGBプロファイルの場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
      try:
          is_rgb = ae.sdk.AEGP_IsRGBColorProfile(profile)
          if is_rgb:
              print("RGBカラープロファイルです")
          else:
              print("非RGBカラープロファイルです")
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)

作業用カラースペースの設定
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_SetWorkingColorSpace(compH: int, colorProfileP: int) -> None

   コンポジションの作業用カラースペースを設定します。

   :param compH: コンポジションハンドル
   :type compH: int
   :param colorProfileP: カラープロファイルポインタ
   :type colorProfileP: int

   **例**:

   .. code-block:: python

      # ICCプロファイルから作業用カラースペースを設定
      with open("custom_profile.icc", "rb") as f:
          icc_data = f.read()

      profile = ae.sdk.AEGP_GetNewColorProfileFromICCProfile(icc_data)
      try:
          compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)
          ae.sdk.AEGP_SetWorkingColorSpace(compH, profile)
          print("作業用カラースペースを設定しました")
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)

OCIO（OpenColorIO）関連機能
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::
   以下の関数は **After Effects 24.0以降** でのみ利用可能です。

.. function:: AEGP_IsOCIOColorManagementUsed() -> bool

   OCIOカラーマネジメントが使用されているかを確認します。

   :return: OCIOが使用されている場合は ``True``
   :rtype: bool

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_IsOCIOColorManagementUsed():
          print("OCIOカラーマネジメントが有効です")
      else:
          print("従来のカラーマネジメントを使用しています")

.. function:: AEGP_GetOCIOConfigurationFile() -> str

   OCIO設定ファイル名を取得します。

   :return: OCIO設定ファイル名（パスなし）
   :rtype: str

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      config_file = ae.sdk.AEGP_GetOCIOConfigurationFile()
      print(f"OCIO設定ファイル: {config_file}")

.. function:: AEGP_GetOCIOConfigurationFilePath() -> str

   OCIO設定ファイルのフルパスを取得します。

   :return: OCIO設定ファイルのフルパス
   :rtype: str

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      config_path = ae.sdk.AEGP_GetOCIOConfigurationFilePath()
      print(f"OCIO設定パス: {config_path}")

.. function:: AEGPD_GetOCIOWorkingColorSpace() -> str

   OCIO作業用カラースペース名を取得します。

   :return: OCIO作業用カラースペース名
   :rtype: str

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      working_cs = ae.sdk.AEGPD_GetOCIOWorkingColorSpace()
      print(f"OCIO作業用カラースペース: {working_cs}")

.. function:: AEGPD_GetOCIODisplayColorSpace() -> tuple[str, str]

   OCIOディスプレイとビューのカラースペース名を取得します。

   :return: ``(display, view)`` のタプル
   :rtype: tuple[str, str]

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      display, view = ae.sdk.AEGPD_GetOCIODisplayColorSpace()
      print(f"ディスプレイ: {display}, ビュー: {view}")

.. function:: AEGPD_IsColorSpaceAwareEffectsEnabled() -> bool

   カラースペース対応エフェクトが有効かどうかを確認します。

   :return: カラースペース対応エフェクトが有効な場合は ``True``
   :rtype: bool

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      if ae.sdk.AEGPD_IsColorSpaceAwareEffectsEnabled():
          print("カラースペース対応エフェクトが有効です")

.. function:: AEGPD_GetLUTInterpolationMethod() -> int

   LUT（Look-Up Table）補間方式を取得します。

   :return: LUT補間方式（整数値）
   :rtype: int

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      method = ae.sdk.AEGPD_GetLUTInterpolationMethod()
      print(f"LUT補間方式: {method}")

.. function:: AEGPD_GetGraphicsWhiteLuminance() -> int

   グラフィックホワイト輝度値を取得します。

   :return: ホワイト輝度値（例: 100, 300）
   :rtype: int

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      luminance = ae.sdk.AEGPD_GetGraphicsWhiteLuminance()
      print(f"ホワイト輝度: {luminance} cd/m²")

.. function:: AEGPD_GetWorkingColorSpaceId() -> bytes

   作業用カラースペースのGUID（16バイト）を取得します。

   :return: GUID（16バイト）
   :rtype: bytes

   .. versionadded:: 24.0

   **例**:

   .. code-block:: python

      guid = ae.sdk.AEGPD_GetWorkingColorSpaceId()
      print(f"カラースペースGUID: {guid.hex()}")

使用例
------

カラープロファイル情報の取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_color_profile_info():
       """作業用カラースペースの情報を表示"""
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)

       profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

       try:
           # プロファイル情報を取得
           desc = ae.sdk.AEGP_GetNewColorProfileDescription(profile)
           gamma = ae.sdk.AEGP_GetColorProfileApproximateGamma(profile)
           is_rgb = ae.sdk.AEGP_IsRGBColorProfile(profile)

           print("=" * 50)
           print(f"カラープロファイル: {desc}")
           print(f"ガンマ値: {gamma}")
           print(f"RGBプロファイル: {'はい' if is_rgb else 'いいえ'}")
           print("=" * 50)

       finally:
           ae.sdk.AEGP_DisposeColorProfile(profile)

   # 実行
   dump_color_profile_info()

ICCプロファイルのエクスポート
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import os

   def export_working_space_icc(output_path="working_space.icc"):
       """作業用カラースペースのICCプロファイルをエクスポート"""
       projH = ae.sdk.AEGP_GetProjectByIndex(0)
       compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)

       profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)

       try:
           # ICCデータを抽出
           icc_data = ae.sdk.AEGP_GetNewICCProfileFromColorProfile(profile)

           # ファイルに保存
           with open(output_path, "wb") as f:
               f.write(icc_data)

           file_size = len(icc_data)
           print(f"ICCプロファイルをエクスポートしました:")
           print(f"  パス: {output_path}")
           print(f"  サイズ: {file_size:,} bytes")

       finally:
           ae.sdk.AEGP_DisposeColorProfile(profile)

   # 実行
   export_working_space_icc()

カスタムICCプロファイルの適用
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import os

   def apply_custom_icc_profile(icc_path):
       """カスタムICCプロファイルを作業用カラースペースに適用"""
       if not os.path.exists(icc_path):
           print(f"エラー: ファイルが見つかりません: {icc_path}")
           return

       # ICCファイルを読み込み
       with open(icc_path, "rb") as f:
           icc_data = f.read()

       print(f"ICCプロファイルを読み込みました ({len(icc_data):,} bytes)")

       # カラープロファイルを作成
       profile = ae.sdk.AEGP_GetNewColorProfileFromICCProfile(icc_data)

       try:
           # プロファイル情報を確認
           desc = ae.sdk.AEGP_GetNewColorProfileDescription(profile)
           gamma = ae.sdk.AEGP_GetColorProfileApproximateGamma(profile)
           is_rgb = ae.sdk.AEGP_IsRGBColorProfile(profile)

           print(f"プロファイル: {desc}")
           print(f"ガンマ: {gamma}")
           print(f"RGB: {is_rgb}")

           # RGBプロファイルでない場合は警告
           if not is_rgb:
               print("警告: 非RGBプロファイルです")
               return

           # コンポジションに適用
           projH = ae.sdk.AEGP_GetProjectByIndex(0)
           compH = ae.sdk.AEGP_GetCompFromProject(projH, 0)

           ae.sdk.AEGP_SetWorkingColorSpace(compH, profile)
           print("作業用カラースペースを設定しました")

       finally:
           ae.sdk.AEGP_DisposeColorProfile(profile)

   # 使用例
   apply_custom_icc_profile(r"C:\Profiles\AdobeRGB1998.icc")

OCIO設定情報の取得
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_ocio_settings():
       """OCIO設定情報を表示（AE 24.0+）"""
       try:
           # OCIOが使用されているかチェック
           is_ocio = ae.sdk.AEGP_IsOCIOColorManagementUsed()

           if not is_ocio:
               print("OCIOカラーマネジメントは使用されていません")
               return

           print("=" * 50)
           print("OCIO設定情報")
           print("=" * 50)

           # 設定ファイル情報
           config_file = ae.sdk.AEGP_GetOCIOConfigurationFile()
           config_path = ae.sdk.AEGP_GetOCIOConfigurationFilePath()
           print(f"設定ファイル: {config_file}")
           print(f"設定パス: {config_path}")

           # カラースペース情報
           working_cs = ae.sdk.AEGPD_GetOCIOWorkingColorSpace()
           display, view = ae.sdk.AEGPD_GetOCIODisplayColorSpace()
           print(f"\n作業用カラースペース: {working_cs}")
           print(f"ディスプレイ: {display}")
           print(f"ビュー: {view}")

           # その他の設定
           cs_aware = ae.sdk.AEGPD_IsColorSpaceAwareEffectsEnabled()
           lut_method = ae.sdk.AEGPD_GetLUTInterpolationMethod()
           luminance = ae.sdk.AEGPD_GetGraphicsWhiteLuminance()

           print(f"\nカラースペース対応エフェクト: {'有効' if cs_aware else '無効'}")
           print(f"LUT補間方式: {lut_method}")
           print(f"ホワイト輝度: {luminance} cd/m²")

           # GUID
           guid = ae.sdk.AEGPD_GetWorkingColorSpaceId()
           print(f"カラースペースGUID: {guid.hex()}")
           print("=" * 50)

       except Exception as e:
           print(f"エラー: {e}")
           print("この機能はAE 24.0以降で利用可能です")

   # 実行
   dump_ocio_settings()

カラープロファイルの比較
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def compare_color_profiles(icc_path1, icc_path2):
       """2つのICCプロファイルを比較"""
       profiles = []

       try:
           # 両方のICCファイルを読み込み
           for path in [icc_path1, icc_path2]:
               with open(path, "rb") as f:
                   icc_data = f.read()

               profile = ae.sdk.AEGP_GetNewColorProfileFromICCProfile(icc_data)
               profiles.append(profile)

           # 情報を取得して比較
           for i, (path, profile) in enumerate(zip([icc_path1, icc_path2], profiles), 1):
               desc = ae.sdk.AEGP_GetNewColorProfileDescription(profile)
               gamma = ae.sdk.AEGP_GetColorProfileApproximateGamma(profile)
               is_rgb = ae.sdk.AEGP_IsRGBColorProfile(profile)

               print(f"プロファイル {i}: {path}")
               print(f"  説明: {desc}")
               print(f"  ガンマ: {gamma}")
               print(f"  RGB: {is_rgb}")
               print()

       finally:
           # すべてのプロファイルを解放
           for profile in profiles:
               ae.sdk.AEGP_DisposeColorProfile(profile)

   # 使用例
   compare_color_profiles(
       r"C:\Profiles\sRGB.icc",
       r"C:\Profiles\AdobeRGB.icc"
   )

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **カラープロファイルは必ず解放する**

   カラープロファイルを取得したら、**必ず** ``AEGP_DisposeColorProfile`` で解放してください。
   解放を忘れるとメモリリークが発生します。

   .. code-block:: python

      profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
      try:
          # プロファイル使用
          pass
      finally:
          ae.sdk.AEGP_DisposeColorProfile(profile)  # 必須

2. **OCIO機能のバージョン確認**

   OCIO関連の機能（``AEGP_IsOCIOColorManagementUsed`` など）は **After Effects 24.0以降** でのみ利用可能です。
   古いバージョンでは例外が発生します。

   .. code-block:: python

      try:
          is_ocio = ae.sdk.AEGP_IsOCIOColorManagementUsed()
      except Exception:
          print("OCIO機能は利用できません（AE 24.0以降が必要）")

3. **RGBプロファイルの確認**

   カラースペースを設定する前に、RGBプロファイルかどうかを確認することを推奨します。

   .. code-block:: python

      if not ae.sdk.AEGP_IsRGBColorProfile(profile):
          print("警告: 非RGBプロファイルです")

4. **ICCデータのサイズ**

   ICCプロファイルデータは数KB～数MBになる場合があります。
   大量のプロファイルを扱う場合はメモリ使用量に注意してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

リソース管理の徹底
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def safe_color_profile_operation(compH):
       """安全なカラープロファイル操作"""
       profile = None
       try:
           profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
           # プロファイル操作
           return ae.sdk.AEGP_GetNewColorProfileDescription(profile)
       finally:
           if profile is not None:
               ae.sdk.AEGP_DisposeColorProfile(profile)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def get_color_profile_info(compH):
       """エラーハンドリング付きプロファイル情報取得"""
       try:
           profile = ae.sdk.AEGP_GetNewWorkingSpaceColorProfile(compH)
           try:
               desc = ae.sdk.AEGP_GetNewColorProfileDescription(profile)
               gamma = ae.sdk.AEGP_GetColorProfileApproximateGamma(profile)
               return {"description": desc, "gamma": gamma}
           finally:
               ae.sdk.AEGP_DisposeColorProfile(profile)
       except Exception as e:
           print(f"カラープロファイル情報の取得に失敗しました: {e}")
           return None

バージョン互換性の考慮
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def get_ocio_info_safe():
       """バージョン互換性を考慮したOCIO情報取得"""
       try:
           if ae.sdk.AEGP_IsOCIOColorManagementUsed():
               return {
                   "working_space": ae.sdk.AEGPD_GetOCIOWorkingColorSpace(),
                   "display_view": ae.sdk.AEGPD_GetOCIODisplayColorSpace(),
                   "config_file": ae.sdk.AEGP_GetOCIOConfigurationFile()
               }
       except AttributeError:
           print("OCIO機能は利用できません（AE 24.0以降が必要）")
       return None

関連項目
--------

- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`AEGP_WorldSuite3` - ピクセルデータ操作
- :doc:`AEGP_ItemViewSuite1` - ビュー管理
- :doc:`index` - 低レベルAPI概要
