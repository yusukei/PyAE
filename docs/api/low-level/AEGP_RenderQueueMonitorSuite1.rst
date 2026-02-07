AEGP_RenderQueueMonitorSuite1
==============================

.. currentmodule:: ae.sdk

AEGP_RenderQueueMonitorSuite1は、After Effectsのレンダーキュー操作を監視し、レンダリングジョブの情報を取得するためのSDK APIです。

概要
----

**実装状況**: 17/17関数バインド済み（うち2関数はスタブ実装）

AEGP_RenderQueueMonitorSuite1は以下の機能を提供します:

- レンダリングジョブの監視（リスナー登録）
- プロジェクト名、アプリケーションバージョンの取得
- ジョブアイテムの情報取得
- レンダリング設定、出力モジュール設定の取得
- 出力モジュールの警告メッセージの取得
- フレームプロパティの取得
- フレームサムネイルの取得

.. important::
   **リスナー機能の制限**

   ``AEGP_RQM_RegisterListener`` と ``AEGP_RQM_DeregisterListener`` はCコールバック関数ポインタを必要とするため、Pythonから直接呼び出すことはできません。これらの関数はスタブ実装として提供されており、呼び出すと例外をスローします。

   リスナー機能が必要な場合は、After Effects C++ SDKを直接使用してください。

基本概念
--------

セッションID (AEGP_RQM_SessionId)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダリングセッションを識別するためのID。通常はリスナーコールバックから取得されます。

.. note::
   リスナー登録ができないため、``session_id`` に ``0`` を使用してテストすることは可能ですが、すべてのケースで動作するとは限りません。

アイテムID (AEGP_RQM_ItemId)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダーキュー内のアイテムを識別するためのID。``AEGP_RQM_GetJobItemID()`` で取得します。

フレームID (AEGP_RQM_FrameId)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダリングされたフレームを識別するためのID。フレームコールバックから取得されます。

完了ステータス (AEGP_RQM_FinishedStatus)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

レンダリングジョブの完了状態を示す定数:

.. list-table::
   :header-rows: 1

   * - 定数
     - 値
     - 説明
   * - ``AEGP_RQM_FinishedStatus_UNKNOWN``
     - 0
     - 不明
   * - ``AEGP_RQM_FinishedStatus_SUCCEEDED``
     - 1
     - 成功
   * - ``AEGP_RQM_FinishedStatus_ABORTED``
     - 2
     - 中止
   * - ``AEGP_RQM_FinishedStatus_ERRED``
     - 3
     - エラー

インデックスの規則
~~~~~~~~~~~~~~~~~~

このSuiteのすべてのインデックスは **0ベース** です。

API リファレンス
----------------

リスナー登録（スタブ実装）
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_RegisterListener() -> None

   レンダーキューリスナーを登録します。

   .. error::
      この関数はPythonから呼び出すことができません。

      Cコールバック関数ポインタが必要なため、スタブ実装として例外をスローします。
      リスナー機能が必要な場合は、After Effects C++ SDKを直接使用してください。

   :raises RuntimeError: 常にスローされます（スタブ実装）

.. function:: AEGP_RQM_DeregisterListener() -> None

   レンダーキューリスナーを登録解除します。

   .. error::
      この関数はPythonから呼び出すことができません。

      Cコールバック関数ポインタが必要なため、スタブ実装として例外をスローします。
      リスナー機能が必要な場合は、After Effects C++ SDKを直接使用してください。

   :raises RuntimeError: 常にスローされます（スタブ実装）

プロジェクト情報の取得
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetProjectName(session_id: int) -> str

   レンダリングジョブに関連付けられたプロジェクト名を取得します。

   :param session_id: セッションID（リスナーコールバックから取得、テスト用に0を使用可能）
   :type session_id: int
   :return: プロジェクト名（UTF-16文字列）
   :rtype: str
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      project_name = ae.sdk.AEGP_RQM_GetProjectName(session_id)
      print(f"プロジェクト名: {project_name}")

.. function:: AEGP_RQM_GetAppVersion(session_id: int) -> str

   After Effects のアプリケーションバージョンを取得します。

   :param session_id: セッションID（リスナーコールバックから取得、テスト用に0を使用可能）
   :type session_id: int
   :return: アプリケーションバージョン文字列
   :rtype: str
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      app_version = ae.sdk.AEGP_RQM_GetAppVersion(session_id)
      print(f"After Effects バージョン: {app_version}")

ジョブアイテムの取得
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetNumJobItems(session_id: int) -> int

   レンダリングジョブ内のアイテム数を取得します。

   :param session_id: セッションID
   :type session_id: int
   :return: ジョブアイテムの数
   :rtype: int
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      num_items = ae.sdk.AEGP_RQM_GetNumJobItems(session_id)
      print(f"ジョブアイテム数: {num_items}")

.. function:: AEGP_RQM_GetJobItemID(session_id: int, job_item_index: int) -> int

   指定されたインデックスのジョブアイテムIDを取得します。

   :param session_id: セッションID
   :type session_id: int
   :param job_item_index: ジョブアイテムのインデックス（0ベース）
   :type job_item_index: int
   :return: ジョブアイテムID
   :rtype: int
   :raises ValueError: job_item_index が負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      item_id = ae.sdk.AEGP_RQM_GetJobItemID(session_id, 0)

レンダリング設定の取得
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetNumJobItemRenderSettings(session_id: int, item_id: int) -> int

   ジョブアイテムのレンダリング設定の数を取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :return: レンダリング設定の数
   :rtype: int
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      num_settings = ae.sdk.AEGP_RQM_GetNumJobItemRenderSettings(session_id, item_id)

.. function:: AEGP_RQM_GetJobItemRenderSetting(session_id: int, item_id: int, setting_index: int) -> tuple[str, str]

   ジョブアイテムのレンダリング設定を取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param setting_index: 設定のインデックス（0ベース）
   :type setting_index: int
   :return: (name, value) - 設定名と値のタプル
   :rtype: tuple[str, str]
   :raises ValueError: setting_index が負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      name, value = ae.sdk.AEGP_RQM_GetJobItemRenderSetting(session_id, item_id, 0)
      print(f"{name}: {value}")

出力モジュールの取得
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetNumJobItemOutputModules(session_id: int, item_id: int) -> int

   ジョブアイテムの出力モジュールの数を取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :return: 出力モジュールの数
   :rtype: int
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      num_modules = ae.sdk.AEGP_RQM_GetNumJobItemOutputModules(session_id, item_id)

出力モジュール設定の取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetNumJobItemOutputModuleSettings(session_id: int, item_id: int, output_module_index: int) -> int

   出力モジュールの設定の数を取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param output_module_index: 出力モジュールのインデックス（0ベース）
   :type output_module_index: int
   :return: 出力モジュール設定の数
   :rtype: int
   :raises ValueError: output_module_index が負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      num_settings = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleSettings(session_id, item_id, 0)

.. function:: AEGP_RQM_GetJobItemOutputModuleSetting(session_id: int, item_id: int, output_module_index: int, setting_index: int) -> tuple[str, str]

   出力モジュールの設定を取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param output_module_index: 出力モジュールのインデックス（0ベース）
   :type output_module_index: int
   :param setting_index: 設定のインデックス（0ベース）
   :type setting_index: int
   :return: (name, value) - 設定名と値のタプル
   :rtype: tuple[str, str]
   :raises ValueError: インデックスが負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      name, value = ae.sdk.AEGP_RQM_GetJobItemOutputModuleSetting(
          session_id, item_id, 0, 0)
      print(f"{name}: {value}")

出力モジュールプロパティの取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetNumJobItemOutputModuleProperties(session_id: int, item_id: int, output_module_index: int) -> int

   出力モジュールのプロパティの数を取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param output_module_index: 出力モジュールのインデックス（0ベース）
   :type output_module_index: int
   :return: 出力モジュールプロパティの数
   :rtype: int
   :raises ValueError: output_module_index が負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      num_props = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleProperties(
          session_id, item_id, 0)

.. function:: AEGP_RQM_GetJobItemOutputModuleProperty(session_id: int, item_id: int, output_module_index: int, property_index: int) -> tuple[str, str]

   出力モジュールのプロパティを取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param output_module_index: 出力モジュールのインデックス（0ベース）
   :type output_module_index: int
   :param property_index: プロパティのインデックス（0ベース）
   :type property_index: int
   :return: (name, value) - プロパティ名と値のタプル
   :rtype: tuple[str, str]
   :raises ValueError: インデックスが負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      name, value = ae.sdk.AEGP_RQM_GetJobItemOutputModuleProperty(
          session_id, item_id, 0, 0)
      print(f"{name}: {value}")

警告メッセージの取得
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetNumJobItemOutputModuleWarnings(session_id: int, item_id: int, output_module_index: int) -> int

   出力モジュールの警告の数を取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param output_module_index: 出力モジュールのインデックス（0ベース）
   :type output_module_index: int
   :return: 警告の数
   :rtype: int
   :raises ValueError: output_module_index が負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      num_warnings = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleWarnings(
          session_id, item_id, 0)

.. function:: AEGP_RQM_GetJobItemOutputModuleWarning(session_id: int, item_id: int, output_module_index: int, warning_index: int) -> str

   出力モジュールの警告メッセージを取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param output_module_index: 出力モジュールのインデックス（0ベース）
   :type output_module_index: int
   :param warning_index: 警告のインデックス（0ベース）
   :type warning_index: int
   :return: 警告メッセージ
   :rtype: str
   :raises ValueError: インデックスが負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      warning = ae.sdk.AEGP_RQM_GetJobItemOutputModuleWarning(
          session_id, item_id, 0, 0)
      print(f"警告: {warning}")

フレームプロパティの取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetNumJobItemFrameProperties(session_id: int, item_id: int, frame_id: int) -> int

   フレームのプロパティの数を取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param frame_id: フレームID
   :type frame_id: int
   :return: フレームプロパティの数
   :rtype: int
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      num_props = ae.sdk.AEGP_RQM_GetNumJobItemFrameProperties(
          session_id, item_id, frame_id)

.. function:: AEGP_RQM_GetJobItemFrameProperty(session_id: int, item_id: int, frame_id: int, property_index: int) -> tuple[str, str]

   フレームのプロパティを取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param frame_id: フレームID
   :type frame_id: int
   :param property_index: プロパティのインデックス（0ベース）
   :type property_index: int
   :return: (name, value) - プロパティ名と値のタプル
   :rtype: tuple[str, str]
   :raises ValueError: property_index が負の場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   **例**:

   .. code-block:: python

      name, value = ae.sdk.AEGP_RQM_GetJobItemFrameProperty(
          session_id, item_id, frame_id, 0)
      print(f"{name}: {value}")

フレームサムネイルの取得
~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RQM_GetJobItemFrameThumbnail(session_id: int, item_id: int, frame_id: int, width: int, height: int) -> tuple[int, int, bytes]

   フレームのサムネイル画像をJPEG形式で取得します。

   :param session_id: セッションID
   :type session_id: int
   :param item_id: ジョブアイテムID
   :type item_id: int
   :param frame_id: フレームID
   :type frame_id: int
   :param width: 最大幅（実際の幅が返されます）
   :type width: int
   :param height: 最大高さ（実際の高さが返されます）
   :type height: int
   :return: (actual_width, actual_height, jpeg_bytes) - 実際のサムネイルサイズとJPEG画像データ
   :rtype: tuple[int, int, bytes]
   :raises ValueError: width または height が正でない場合
   :raises RuntimeError: Suite が利用できない場合、または API 呼び出しが失敗した場合

   .. note::
      返されるサイズは、指定した最大サイズ以下でアスペクト比を維持したサイズです。

   **例**:

   .. code-block:: python

      width, height, jpeg_data = ae.sdk.AEGP_RQM_GetJobItemFrameThumbnail(
          session_id, item_id, frame_id, 640, 480)
      print(f"サムネイルサイズ: {width}x{height}")

      # JPEG画像として保存
      with open("thumbnail.jpg", "wb") as f:
          f.write(jpeg_data)

使用例
------

.. note::
   以下の例は、リスナーコールバックから ``session_id``、``item_id``、``frame_id`` が取得できることを前提としています。

   実際の使用にはC++でリスナーを実装する必要があります。

レンダリングジョブ情報の取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_render_job_info(session_id):
       """レンダリングジョブの基本情報を表示"""
       # プロジェクト情報
       project_name = ae.sdk.AEGP_RQM_GetProjectName(session_id)
       app_version = ae.sdk.AEGP_RQM_GetAppVersion(session_id)

       print("=" * 60)
       print(f"プロジェクト名: {project_name}")
       print(f"After Effects バージョン: {app_version}")

       # ジョブアイテムの数
       num_items = ae.sdk.AEGP_RQM_GetNumJobItems(session_id)
       print(f"ジョブアイテム数: {num_items}")
       print("=" * 60)

レンダリング設定の詳細表示
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_render_settings(session_id, item_id):
       """レンダリング設定をすべて表示"""
       num_settings = ae.sdk.AEGP_RQM_GetNumJobItemRenderSettings(
           session_id, item_id)

       print(f"レンダリング設定 ({num_settings}個):")
       for i in range(num_settings):
           name, value = ae.sdk.AEGP_RQM_GetJobItemRenderSetting(
               session_id, item_id, i)
           print(f"  {name}: {value}")

出力モジュール情報の詳細表示
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_output_module_info(session_id, item_id):
       """出力モジュールの詳細情報を表示"""
       num_modules = ae.sdk.AEGP_RQM_GetNumJobItemOutputModules(
           session_id, item_id)

       print(f"出力モジュール数: {num_modules}")

       for module_idx in range(num_modules):
           print(f"\n--- 出力モジュール {module_idx} ---")

           # 設定を表示
           num_settings = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleSettings(
               session_id, item_id, module_idx)
           print(f"設定 ({num_settings}個):")
           for setting_idx in range(num_settings):
               name, value = ae.sdk.AEGP_RQM_GetJobItemOutputModuleSetting(
                   session_id, item_id, module_idx, setting_idx)
               print(f"  {name}: {value}")

           # プロパティを表示
           num_props = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleProperties(
               session_id, item_id, module_idx)
           print(f"プロパティ ({num_props}個):")
           for prop_idx in range(num_props):
               name, value = ae.sdk.AEGP_RQM_GetJobItemOutputModuleProperty(
                   session_id, item_id, module_idx, prop_idx)
               print(f"  {name}: {value}")

           # 警告を表示
           num_warnings = ae.sdk.AEGP_RQM_GetNumJobItemOutputModuleWarnings(
               session_id, item_id, module_idx)
           if num_warnings > 0:
               print(f"警告 ({num_warnings}個):")
               for warning_idx in range(num_warnings):
                   warning = ae.sdk.AEGP_RQM_GetJobItemOutputModuleWarning(
                       session_id, item_id, module_idx, warning_idx)
                   print(f"  - {warning}")

フレームサムネイルの保存
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import os

   def save_frame_thumbnail(session_id, item_id, frame_id, output_dir):
       """フレームのサムネイルを保存"""
       # サムネイルを取得
       width, height, jpeg_data = ae.sdk.AEGP_RQM_GetJobItemFrameThumbnail(
           session_id, item_id, frame_id, 640, 480)

       # ファイル名を生成
       filename = f"frame_{frame_id}_{width}x{height}.jpg"
       filepath = os.path.join(output_dir, filename)

       # JPEG画像として保存
       with open(filepath, "wb") as f:
           f.write(jpeg_data)

       print(f"サムネイルを保存しました: {filepath}")
       print(f"サイズ: {width}x{height}")

フレームプロパティの詳細表示
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_frame_properties(session_id, item_id, frame_id):
       """フレームのプロパティをすべて表示"""
       num_props = ae.sdk.AEGP_RQM_GetNumJobItemFrameProperties(
           session_id, item_id, frame_id)

       print(f"フレームID {frame_id} のプロパティ ({num_props}個):")
       for i in range(num_props):
           name, value = ae.sdk.AEGP_RQM_GetJobItemFrameProperty(
               session_id, item_id, frame_id, i)
           print(f"  {name}: {value}")

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **リスナー登録の制限**

   ``AEGP_RQM_RegisterListener`` と ``AEGP_RQM_DeregisterListener`` はPythonから呼び出せません。
   レンダーキューの監視が必要な場合は、C++プラグインを作成してください。

2. **session_idの取得**

   ``session_id`` は通常、リスナーコールバックから取得されます。
   テスト目的で ``0`` を使用することもできますが、すべてのケースで動作する保証はありません。

3. **インデックスの規則**

   すべてのインデックスは0ベースです。範囲外のインデックスを指定するとエラーが発生します。

4. **文字列エンコーディング**

   このSuiteが返す文字列はUTF-16でエンコードされています。
   Pythonでは自動的に処理されるため、特別な対応は不要です。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       num_items = ae.sdk.AEGP_RQM_GetNumJobItems(session_id)
   except RuntimeError as e:
       print(f"エラー: {e}")

インデックスの範囲確認
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   num_items = ae.sdk.AEGP_RQM_GetNumJobItems(session_id)
   for i in range(num_items):
       item_id = ae.sdk.AEGP_RQM_GetJobItemID(session_id, i)
       # 処理...

サムネイルのバッチ保存
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   import ae
   import os

   def save_all_frame_thumbnails(session_id, item_id, frame_ids, output_dir):
       """複数のフレームサムネイルを保存"""
       os.makedirs(output_dir, exist_ok=True)

       for frame_id in frame_ids:
           try:
               save_frame_thumbnail(session_id, item_id, frame_id, output_dir)
           except Exception as e:
               print(f"フレーム {frame_id} のサムネイル保存に失敗: {e}")

制限事項
--------

1. **Pythonからのリスナー登録不可**

   リスナー機能（``AEGP_RQM_RegisterListener``、``AEGP_RQM_DeregisterListener``）は、
   Cコールバック関数ポインタを必要とするため、Pythonから直接使用できません。

2. **セッションIDの取得制限**

   ``session_id`` は通常リスナーコールバックから取得されるため、
   Pythonのみでこの Suite を完全に活用することは困難です。

3. **C++プラグインとの併用が推奨**

   レンダーキューの監視を行う場合は、C++でリスナーを実装し、
   取得した情報をPythonスクリプトに渡す方法が推奨されます。

関連項目
--------

- :doc:`AEGP_RenderQueueSuite1` - レンダーキューの操作
- :doc:`AEGP_RQItemSuite4` - レンダーキューアイテムの管理
- :doc:`AEGP_OutputModuleSuite4` - 出力モジュールの設定
- :doc:`index` - 低レベルAPI概要
