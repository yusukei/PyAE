AEGP_UtilitySuite6
==================

.. currentmodule:: ae.sdk

AEGP_UtilitySuite6は、After Effectsプラグイン開発における様々なユーティリティ機能を提供するSDK APIです。

概要
----

**実装状況**: 30/30関数実装 ✅

AEGP_UtilitySuite6は以下の機能を提供します:

- Undo/Redoグループの管理
- ExtendScriptの実行
- エラー処理とレポート
- ダイアログ表示
- ペイント/文字パレットの色設定
- プラットフォーム情報の取得
- デバッグログとコンソール出力
- 数値変換ユーティリティ

基本概念
--------

Undoグループ
~~~~~~~~~~~~

複数の操作を1つのUndo単位としてグループ化できます。ユーザーが「元に戻す」を実行した際、グループ内のすべての操作が一度に取り消されます。

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("複数の変更")
   try:
       # 複数の操作
       layer.SetPosition(100, 100)
       layer.SetScale(200)
   finally:
       ae.sdk.AEGP_EndUndoGroup()

ExtendScript実行
~~~~~~~~~~~~~~~~

After Effectsの旧スクリプトエンジン（ExtendScript/JavaScript）を実行できます。レガシーコードとの互換性や、Python APIで提供されていない機能へのアクセスに使用します。

.. warning::
   ExtendScriptは古いAPIです。可能な限りPython APIを使用することを推奨します。

エラー抑制
~~~~~~~~~~

通常、SDK関数がエラーを返すとAfter Effectsがエラーダイアログを表示します。``AEGP_StartQuietErrors`` を使用すると、このダイアログを一時的に抑制できます。

.. code-block:: python

   err_state = ae.sdk.AEGP_StartQuietErrors()
   try:
       # エラーが発生する可能性のある処理
       result = risky_operation()
   finally:
       ae.sdk.AEGP_EndQuietErrors(True, err_state)  # エラーを報告

API リファレンス
----------------

Undo/Redo管理
~~~~~~~~~~~~~

.. function:: AEGP_StartUndoGroup(undo_name: str) -> None

   Undoグループを開始します。

   :param undo_name: Undo履歴に表示される操作名
   :type undo_name: str

   .. note::
      必ず ``AEGP_EndUndoGroup()`` とペアで使用してください。try-finallyブロックの使用を推奨します。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_StartUndoGroup("レイヤー設定変更")
      try:
          layer.SetPosition(100, 100)
          layer.SetOpacity(50)
      finally:
          ae.sdk.AEGP_EndUndoGroup()

.. function:: AEGP_EndUndoGroup() -> None

   Undoグループを終了します。

   .. warning::
      ``AEGP_StartUndoGroup()`` を呼び出していない状態で ``AEGP_EndUndoGroup()`` を呼び出すとエラーになります。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_StartUndoGroup("処理名")
      try:
          # 複数の変更処理
          pass
      finally:
          ae.sdk.AEGP_EndUndoGroup()

ExtendScript実行
~~~~~~~~~~~~~~~~

.. function:: AEGP_ExecuteScript(script: str) -> str

   ExtendScriptコードを実行し、結果を文字列として返します。

   :param script: 実行するExtendScript（JavaScript）コード
   :type script: str
   :return: スクリプトの実行結果（文字列）
   :rtype: str
   :raises RuntimeError: スクリプトにエラーがある場合

   .. warning::
      ExtendScriptは古いAPIです。可能な限りPython APIを使用してください。

   **例**:

   .. code-block:: python

      # ExtendScriptでプロジェクト名を取得
      script = "app.project.file.name"
      result = ae.sdk.AEGP_ExecuteScript(script)
      print(f"プロジェクト名: {result}")

      # ExtendScriptで複数の処理を実行
      script = """
      var comp = app.project.item(1);
      comp.name = "New Composition Name";
      comp.name;  // 戻り値
      """
      result = ae.sdk.AEGP_ExecuteScript(script)

プラグインID
~~~~~~~~~~~~

.. function:: AEGP_GetPluginID() -> int

   PyAEプラグインのプラグインIDを取得します。

   :return: プラグインID
   :rtype: int

   .. note::
      多くのSDK関数は ``plugin_id`` パラメータを要求します。この関数を1度だけ呼び出し、結果を変数に保存して再利用することを推奨します。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      # plugin_id を他のSDK関数で使用

ダイアログ表示
~~~~~~~~~~~~~~

.. function:: AEGP_ReportInfo(plugin_id: int, info_string: str) -> None

   情報ダイアログを表示します（ASCII文字列）。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param info_string: 表示するメッセージ
   :type info_string: str

   .. note::
      日本語などのUnicode文字を含む場合は ``AEGP_ReportInfoUnicode()`` を使用してください。

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      ae.sdk.AEGP_ReportInfo(plugin_id, "Processing complete!")

.. function:: AEGP_ReportInfoUnicode(plugin_id: int, info_string: str) -> None

   情報ダイアログを表示します（Unicode対応）。

   :param plugin_id: プラグインID（``AEGP_GetPluginID()`` で取得）
   :type plugin_id: int
   :param info_string: 表示するメッセージ（日本語対応）
   :type info_string: str

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      ae.sdk.AEGP_ReportInfoUnicode(plugin_id, "処理が完了しました")

バージョン情報
~~~~~~~~~~~~~~

.. function:: AEGP_GetDriverPluginInitFuncVersion() -> tuple[int, int]

   ドライバープラグインの初期化関数バージョンを取得します。

   :return: (major, minor) のタプル
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      major, minor = ae.sdk.AEGP_GetDriverPluginInitFuncVersion()
      print(f"Init Func Version: {major}.{minor}")

.. function:: AEGP_GetDriverImplementationVersion() -> tuple[int, int]

   ドライバー実装バージョンを取得します。

   :return: (major, minor) のタプル
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      major, minor = ae.sdk.AEGP_GetDriverImplementationVersion()
      print(f"Implementation Version: {major}.{minor}")

エラー処理
~~~~~~~~~~

.. function:: AEGP_StartQuietErrors() -> int

   エラーダイアログの表示を抑制します。

   :return: エラー状態トークン（``AEGP_EndQuietErrors()`` に渡す）
   :rtype: int

   .. important::
      必ず ``AEGP_EndQuietErrors()`` とペアで使用してください。

   **例**:

   .. code-block:: python

      err_state = ae.sdk.AEGP_StartQuietErrors()
      try:
          # エラーが発生する可能性のある処理
          pass
      finally:
          ae.sdk.AEGP_EndQuietErrors(True, err_state)

.. function:: AEGP_EndQuietErrors(report_quieted_errors: bool, err_state: int) -> None

   エラー抑制を終了します。

   :param report_quieted_errors: 抑制されたエラーを報告するか（``True`` で最後のエラーを表示）
   :type report_quieted_errors: bool
   :param err_state: ``AEGP_StartQuietErrors()`` が返したトークン
   :type err_state: int

   **例**:

   .. code-block:: python

      err_state = ae.sdk.AEGP_StartQuietErrors()
      try:
          result = risky_operation()
      finally:
          ae.sdk.AEGP_EndQuietErrors(False, err_state)  # エラーを報告しない

.. function:: AEGP_GetLastErrorMessage() -> tuple[str, int]

   最後に発生したエラーのメッセージと番号を取得します。

   :return: (エラーメッセージ, エラー番号) のタプル
   :rtype: tuple[str, int]

   .. note::
      日本語環境では、エラーメッセージはShift-JISからUTF-8に自動変換されます。

   **例**:

   .. code-block:: python

      err_msg, err_num = ae.sdk.AEGP_GetLastErrorMessage()
      print(f"エラー #{err_num}: {err_msg}")

プラットフォーム情報
~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_RegisterWithAEGP(global_refcon: int, plugin_name: str) -> int

   プラグインをAfter Effectsに登録し、プラグインIDを取得します。

   :param global_refcon: グローバルリファレンスコンテキスト（通常は0）
   :type global_refcon: int
   :param plugin_name: プラグイン名
   :type plugin_name: str
   :return: 登録されたプラグインID
   :rtype: int

   .. note::
      PyAEプラグインでは通常、この関数を直接呼び出す必要はありません。

.. function:: AEGP_GetMainHWND() -> int

   After Effectsのメインウィンドウハンドルを取得します。

   :return: ウィンドウハンドル（Windowsの場合はHWND、macOSではNULL）
   :rtype: int

   **例**:

   .. code-block:: python

      hwnd = ae.sdk.AEGP_GetMainHWND()
      # カスタムダイアログの親ウィンドウとして使用

.. function:: AEGP_ShowHideAllFloaters(include_tool_pal: bool) -> None

   すべてのフローティングパレットの表示/非表示を切り替えます。

   :param include_tool_pal: ツールパレットも含めるか
   :type include_tool_pal: bool

   **例**:

   .. code-block:: python

      # すべてのパレットを非表示
      ae.sdk.AEGP_ShowHideAllFloaters(True)

      # もう一度呼び出すと表示
      ae.sdk.AEGP_ShowHideAllFloaters(True)

ペイントパレット
~~~~~~~~~~~~~~~~

.. function:: AEGP_PaintPalGetForeColor() -> dict

   ペイントパレットの前景色を取得します。

   :return: RGBA色値の辞書 (``red``, ``green``, ``blue``, ``alpha`` キー、各値は0.0-1.0)
   :rtype: dict

   **例**:

   .. code-block:: python

      color = ae.sdk.AEGP_PaintPalGetForeColor()
      print(f"前景色: R={color['red']}, G={color['green']}, B={color['blue']}")

.. function:: AEGP_PaintPalGetBackColor() -> dict

   ペイントパレットの背景色を取得します。

   :return: RGBA色値の辞書 (``red``, ``green``, ``blue``, ``alpha`` キー、各値は0.0-1.0)
   :rtype: dict

   **例**:

   .. code-block:: python

      color = ae.sdk.AEGP_PaintPalGetBackColor()

.. function:: AEGP_PaintPalSetForeColor(color: dict) -> None

   ペイントパレットの前景色を設定します。

   :param color: RGBA色値の辞書 (``red``, ``green``, ``blue``, ``alpha`` キー、各値は0.0-1.0)
   :type color: dict

   **例**:

   .. code-block:: python

      color = {
          'red': 1.0,
          'green': 0.0,
          'blue': 0.0,
          'alpha': 1.0
      }
      ae.sdk.AEGP_PaintPalSetForeColor(color)  # 赤色

.. function:: AEGP_PaintPalSetBackColor(color: dict) -> None

   ペイントパレットの背景色を設定します。

   :param color: RGBA色値の辞書 (``red``, ``green``, ``blue``, ``alpha`` キー、各値は0.0-1.0)
   :type color: dict

   **例**:

   .. code-block:: python

      color = {
          'red': 1.0,
          'green': 1.0,
          'blue': 1.0,
          'alpha': 1.0
      }
      ae.sdk.AEGP_PaintPalSetBackColor(color)  # 白色

文字パレット
~~~~~~~~~~~~

.. function:: AEGP_CharPalGetFillColor() -> tuple[bool, dict]

   テキストの塗りつぶし色を取得します。

   :return: (色が定義されているか, RGBA色値の辞書) のタプル
   :rtype: tuple[bool, dict]

   .. note::
      複数の異なる色が選択されている場合、``is_defined`` が ``False`` になります。

   **例**:

   .. code-block:: python

      is_defined, color = ae.sdk.AEGP_CharPalGetFillColor()
      if is_defined:
          print(f"塗りつぶし色: R={color['red']}, G={color['green']}")
      else:
          print("複数の色が選択されています")

.. function:: AEGP_CharPalGetStrokeColor() -> tuple[bool, dict]

   テキストの線色を取得します。

   :return: (色が定義されているか, RGBA色値の辞書) のタプル
   :rtype: tuple[bool, dict]

   **例**:

   .. code-block:: python

      is_defined, color = ae.sdk.AEGP_CharPalGetStrokeColor()

.. function:: AEGP_CharPalSetFillColor(color: dict) -> None

   テキストの塗りつぶし色を設定します。

   :param color: RGBA色値の辞書 (``red``, ``green``, ``blue``, ``alpha`` キー、各値は0.0-1.0)
   :type color: dict

   **例**:

   .. code-block:: python

      color = {'red': 0.0, 'green': 0.0, 'blue': 1.0, 'alpha': 1.0}
      ae.sdk.AEGP_CharPalSetFillColor(color)  # 青色

.. function:: AEGP_CharPalSetStrokeColor(color: dict) -> None

   テキストの線色を設定します。

   :param color: RGBA色値の辞書 (``red``, ``green``, ``blue``, ``alpha`` キー、各値は0.0-1.0)
   :type color: dict

   **例**:

   .. code-block:: python

      color = {'red': 0.0, 'green': 0.0, 'blue': 0.0, 'alpha': 1.0}
      ae.sdk.AEGP_CharPalSetStrokeColor(color)  # 黒色

.. function:: AEGP_CharPalIsFillColorUIFrontmost() -> bool

   文字パレットで塗りつぶし色UIが前面にあるか確認します。

   :return: 塗りつぶし色UIが前面の場合は ``True``、線色UIが前面の場合は ``False``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_CharPalIsFillColorUIFrontmost():
          print("塗りつぶし色が選択されています")
      else:
          print("線色が選択されています")

数値変換
~~~~~~~~

.. function:: AEGP_ConvertFpLongToHSFRatio(number: float) -> tuple[int, int]

   浮動小数点数をHSF比率（分数）に変換します。

   :param number: 変換する浮動小数点数
   :type number: float
   :return: (分子, 分母) のタプル
   :rtype: tuple[int, int]

   **例**:

   .. code-block:: python

      num, den = ae.sdk.AEGP_ConvertFpLongToHSFRatio(0.5)
      print(f"{num}/{den}")  # 例: 1/2

.. function:: AEGP_ConvertHSFRatioToFpLong(numerator: int, denominator: int) -> float

   HSF比率（分数）を浮動小数点数に変換します。

   :param numerator: 分子
   :type numerator: int
   :param denominator: 分母
   :type denominator: int
   :return: 浮動小数点数
   :rtype: float

   **例**:

   .. code-block:: python

      result = ae.sdk.AEGP_ConvertHSFRatioToFpLong(1, 2)
      print(result)  # 0.5

その他のユーティリティ
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_CauseIdleRoutinesToBeCalled() -> None

   アイドルルーチンの呼び出しを要求します。

   .. note::
      この関数はスレッドセーフで非同期に動作します。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_CauseIdleRoutinesToBeCalled()

.. function:: AEGP_GetSuppressInteractiveUI() -> bool

   インタラクティブUIが抑制されているか確認します（ヘッドレスモード/サービスモード）。

   :return: UIが抑制されている場合は ``True``
   :rtype: bool

   .. note::
      レンダーエンジンやサービスモードで実行している場合、この関数は ``True`` を返します。

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_GetSuppressInteractiveUI():
          print("ヘッドレスモードで実行中")

.. function:: AEGP_WriteToOSConsole(text: str) -> None

   OSコンソールにテキストを出力します。

   :param text: 出力するテキスト
   :type text: str

   .. note::
      UIが抑制されている環境でも確実に出力できます。

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_WriteToOSConsole("デバッグメッセージ")

.. function:: AEGP_WriteToDebugLog(subsystem: str, event_type: str, info: str) -> None

   デバッグログにエントリを書き込みます。

   :param subsystem: サブシステム名
   :type subsystem: str
   :param event_type: イベントタイプ
   :type event_type: str
   :param info: 情報テキスト
   :type info: str

   **例**:

   .. code-block:: python

      ae.sdk.AEGP_WriteToDebugLog("PyAE", "INFO", "処理開始")

.. function:: AEGP_IsScriptingAvailable() -> bool

   スクリプト機能が利用可能か確認します。

   :return: スクリプト機能が利用可能な場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_IsScriptingAvailable():
          result = ae.sdk.AEGP_ExecuteScript("app.project.file.name")

.. function:: AEGP_HostIsActivated() -> bool

   After Effectsがアクティベート（ライセンス認証）されているか確認します。

   :return: アクティベートされている場合は ``True``
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.sdk.AEGP_HostIsActivated():
          print("After Effectsはライセンス認証済みです")

.. function:: AEGP_GetPluginPlatformRef(plugin_id: int) -> int

   プラグインのプラットフォーム参照を取得します。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :return: プラットフォーム参照（macOSではCFBundleRef、WindowsではNULL）
   :rtype: int

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()
      plat_ref = ae.sdk.AEGP_GetPluginPlatformRef(plugin_id)

.. function:: AEGP_UpdateFontList() -> None

   システムフォントリストを再スキャンします。

   .. note::
      フォントリストが変更されていない場合は、すぐに戻ります。

   **例**:

   .. code-block:: python

      # 新しいフォントをインストールした後
      ae.sdk.AEGP_UpdateFontList()

.. function:: AEGP_GetPluginPaths(plugin_id: int, path_type: int) -> str

   プラグインに関連するパスを取得します（UTF-16）。

   :param plugin_id: プラグインID
   :type plugin_id: int
   :param path_type: パスタイプ（1=USER_PLUGIN, 2=ALLUSER_PLUGIN, 3=APP）
   :type path_type: int
   :return: パス文字列（UTF-16からUTF-8に自動変換）
   :rtype: str

   .. warning::
      ``path_type=0`` (AEGP_GetPathTypes_PLUGIN) はAfter Effectsで実装されていません。

   **パスタイプ**:

   - ``1``: ``AEGP_GetPathTypes_USER_PLUGIN`` - ユーザー固有プラグインディレクトリ
   - ``2``: ``AEGP_GetPathTypes_ALLUSER_PLUGIN`` - 全ユーザー共有プラグインディレクトリ
   - ``3``: ``AEGP_GetPathTypes_APP`` - After Effects実行ファイルの場所

   **例**:

   .. code-block:: python

      plugin_id = ae.sdk.AEGP_GetPluginID()

      # ユーザープラグインディレクトリ
      user_plugin_path = ae.sdk.AEGP_GetPluginPaths(plugin_id, 1)
      print(f"ユーザープラグイン: {user_plugin_path}")

      # After Effects実行ファイルの場所
      app_path = ae.sdk.AEGP_GetPluginPaths(plugin_id, 3)
      print(f"AE実行ファイル: {app_path}")

使用例
------

Undoグループで複数の変更を管理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def apply_preset(layer, preset_name):
       """複数の設定を1つのUndo操作として適用"""
       ae.sdk.AEGP_StartUndoGroup(f"プリセット適用: {preset_name}")
       try:
           # 位置、スケール、不透明度を一度に変更
           layer.SetPosition(100, 100)
           layer.SetScale(150)
           layer.SetOpacity(75)

           # ブレンドモードも変更
           layer.SetBlendMode(ae.sdk.PF_Xfer_MULTIPLY)

           print(f"プリセット '{preset_name}' を適用しました")

       except Exception as e:
           print(f"エラー: {e}")
           raise

       finally:
           ae.sdk.AEGP_EndUndoGroup()

   # 実行
   layer = ae.Layer()
   apply_preset(layer, "スタンダードフェード")

エラーを抑制して処理を継続
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def batch_process_layers(layers):
       """複数レイヤーを処理（エラーがあってもスキップして続行）"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       for i, layer in enumerate(layers):
           err_state = ae.sdk.AEGP_StartQuietErrors()
           try:
               # エラーが発生する可能性のある処理
               layer.SetScale(200)
               print(f"レイヤー {i+1}: 成功")

           except Exception as e:
               # エラーメッセージを取得
               err_msg, err_num = ae.sdk.AEGP_GetLastErrorMessage()
               print(f"レイヤー {i+1}: スキップ (エラー #{err_num}: {err_msg})")

           finally:
               ae.sdk.AEGP_EndQuietErrors(False, err_state)  # エラーダイアログを表示しない

   # 使用例
   comp = ae.Composition()
   layers = comp.GetLayers()
   batch_process_layers(layers)

ExtendScriptとの連携
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def export_comp_to_file_via_jsx(comp_name, output_path):
       """ExtendScriptを使用してコンポジションをエクスポート"""

       # ExtendScriptコードを構築
       script = f"""
       var comp = null;
       for (var i = 1; i <= app.project.numItems; i++) {{
           if (app.project.item(i).name === "{comp_name}") {{
               comp = app.project.item(i);
               break;
           }}
       }}

       if (comp) {{
           var renderQueue = app.project.renderQueue;
           var renderItem = renderQueue.items.add(comp);
           renderItem.outputModule(1).file = new File("{output_path}");
           "Render queue item added";
       }} else {{
           throw "Composition not found: {comp_name}";
       }}
       """

       try:
           result = ae.sdk.AEGP_ExecuteScript(script)
           print(f"結果: {result}")
           return True

       except Exception as e:
           print(f"ExtendScriptエラー: {e}")
           return False

   # 実行
   export_comp_to_file_via_jsx("Main Comp", r"C:\Output\output.mov")

ペイント/文字パレット色の一括設定
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_color_palette():
       """ペイントと文字パレットの色を設定"""

       # ペイントパレット: 前景色=赤、背景色=白
       paint_fore = {'red': 1.0, 'green': 0.0, 'blue': 0.0, 'alpha': 1.0}
       paint_back = {'red': 1.0, 'green': 1.0, 'blue': 1.0, 'alpha': 1.0}

       ae.sdk.AEGP_PaintPalSetForeColor(paint_fore)
       ae.sdk.AEGP_PaintPalSetBackColor(paint_back)

       # 文字パレット: 塗りつぶし=青、線=黒
       text_fill = {'red': 0.0, 'green': 0.0, 'blue': 1.0, 'alpha': 1.0}
       text_stroke = {'red': 0.0, 'green': 0.0, 'blue': 0.0, 'alpha': 1.0}

       ae.sdk.AEGP_CharPalSetFillColor(text_fill)
       ae.sdk.AEGP_CharPalSetStrokeColor(text_stroke)

       plugin_id = ae.sdk.AEGP_GetPluginID()
       ae.sdk.AEGP_ReportInfoUnicode(plugin_id, "カラーパレットを設定しました")

   # 実行
   setup_color_palette()

環境情報の取得とログ出力
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dump_environment_info():
       """After Effects環境情報をログに出力"""
       plugin_id = ae.sdk.AEGP_GetPluginID()

       # バージョン情報
       init_major, init_minor = ae.sdk.AEGP_GetDriverPluginInitFuncVersion()
       impl_major, impl_minor = ae.sdk.AEGP_GetDriverImplementationVersion()

       # ライセンス状態
       is_activated = ae.sdk.AEGP_HostIsActivated()

       # スクリプト機能
       scripting_available = ae.sdk.AEGP_IsScriptingAvailable()

       # UIモード
       ui_suppressed = ae.sdk.AEGP_GetSuppressInteractiveUI()

       # パス情報
       try:
           app_path = ae.sdk.AEGP_GetPluginPaths(plugin_id, 3)  # APP
       except:
           app_path = "取得できません"

       # デバッグログに出力
       ae.sdk.AEGP_WriteToDebugLog("PyAE", "INFO", "=== 環境情報 ===")
       ae.sdk.AEGP_WriteToDebugLog("PyAE", "VERSION", f"Init: {init_major}.{init_minor}, Impl: {impl_major}.{impl_minor}")
       ae.sdk.AEGP_WriteToDebugLog("PyAE", "LICENSE", f"Activated: {is_activated}")
       ae.sdk.AEGP_WriteToDebugLog("PyAE", "SCRIPT", f"Scripting Available: {scripting_available}")
       ae.sdk.AEGP_WriteToDebugLog("PyAE", "UI", f"UI Suppressed: {ui_suppressed}")
       ae.sdk.AEGP_WriteToDebugLog("PyAE", "PATH", f"AE Path: {app_path}")

       # コンソールにも出力
       ae.sdk.AEGP_WriteToOSConsole("環境情報をデバッグログに出力しました")

   # 実行
   dump_environment_info()

色の取得と適用
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def copy_paint_color_to_text():
       """ペイントパレットの前景色を文字パレットの塗りつぶし色にコピー"""

       # ペイントパレットから前景色を取得
       paint_color = ae.sdk.AEGP_PaintPalGetForeColor()

       print(f"コピーする色: R={paint_color['red']:.2f}, G={paint_color['green']:.2f}, B={paint_color['blue']:.2f}")

       # 文字パレットの塗りつぶし色に設定
       ae.sdk.AEGP_CharPalSetFillColor(paint_color)

       plugin_id = ae.sdk.AEGP_GetPluginID()
       ae.sdk.AEGP_ReportInfoUnicode(plugin_id, "色をコピーしました")

   # 実行
   copy_paint_color_to_text()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **Undoグループは必ずペアで使用**

   ``AEGP_StartUndoGroup()`` と ``AEGP_EndUndoGroup()`` は必ずペアで使用してください。try-finallyブロックを推奨します。

2. **エラー抑制は慎重に使用**

   ``AEGP_StartQuietErrors()`` を使用すると、ユーザーがエラーに気づかなくなる可能性があります。必要な場合のみ使用してください。

3. **ExtendScriptは最終手段**

   ExtendScriptは古いAPIです。Python APIで同等の機能が提供されている場合は、そちらを優先してください。

4. **色値の範囲**

   すべての色値（``red``, ``green``, ``blue``, ``alpha``）は0.0-1.0の範囲で指定してください。範囲外の値はエラーになります。

5. **プラグインIDのキャッシュ**

   ``AEGP_GetPluginID()`` は1度だけ呼び出し、結果を変数に保存して再利用してください。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

Undoグループの使用
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   ae.sdk.AEGP_StartUndoGroup("操作名")
   try:
       # 複数の変更処理
       pass
   finally:
       ae.sdk.AEGP_EndUndoGroup()

エラー処理
^^^^^^^^^^

.. code-block:: python

   err_state = ae.sdk.AEGP_StartQuietErrors()
   try:
       # エラーが発生する可能性のある処理
       pass
   except Exception:
       err_msg, err_num = ae.sdk.AEGP_GetLastErrorMessage()
       # エラーを処理
   finally:
       ae.sdk.AEGP_EndQuietErrors(False, err_state)

色の設定
^^^^^^^^

.. code-block:: python

   # 0.0-1.0の範囲で指定
   color = {
       'red': 1.0,
       'green': 0.5,
       'blue': 0.0,
       'alpha': 1.0
   }
   ae.sdk.AEGP_PaintPalSetForeColor(color)

プラグインIDのキャッシュ
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # グローバルまたはクラス変数として保存
   plugin_id = ae.sdk.AEGP_GetPluginID()

   # 複数の場所で再利用
   ae.sdk.AEGP_ReportInfoUnicode(plugin_id, "メッセージ1")
   ae.sdk.AEGP_ReportInfoUnicode(plugin_id, "メッセージ2")

関連項目
--------

- :doc:`AEGP_ProjSuite6` - プロジェクト管理
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`AEGP_LayerSuite9` - レイヤー操作
- :doc:`index` - 低レベルAPI概要
