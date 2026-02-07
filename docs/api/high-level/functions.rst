トップレベル関数
================

.. currentmodule:: ae

``ae`` モジュール直下で利用できるトップレベル関数のリファレンスです。

.. code-block:: python

   import ae

   # バージョン情報
   print(ae.version())

   # プロジェクトを取得
   project = ae.get_project()

   # アクティブなコンポジションを取得
   comp = ae.get_active_comp()

バージョン・初期化
------------------

.. function:: version() -> str

   PyAEのバージョン文字列を取得します。

   :return: バージョン文字列（例: ``"1.0.0"``)
   :rtype: str

   .. code-block:: python

      >>> ae.version()
      '1.0.0'

.. function:: is_initialized() -> bool

   プラグインが初期化されているか確認します。

   :return: 初期化済みの場合 ``True``
   :rtype: bool

ログ関数
--------

.. function:: log(level: str, message: str) -> None

   指定したレベルでメッセージをログ出力します。

   :param level: ログレベル（``"debug"``, ``"info"``, ``"warning"``, ``"error"``）
   :type level: str
   :param message: ログメッセージ
   :type message: str

.. function:: log_debug(message: str) -> None

   デバッグレベルのメッセージをログ出力します。

   :param message: ログメッセージ
   :type message: str

.. function:: log_info(message: str) -> None

   情報レベルのメッセージをログ出力します。

   :param message: ログメッセージ
   :type message: str

.. function:: log_warning(message: str) -> None

   警告レベルのメッセージをログ出力します。

   :param message: ログメッセージ
   :type message: str

.. function:: log_error(message: str) -> None

   エラーレベルのメッセージをログ出力します。

   :param message: ログメッセージ
   :type message: str

アラート・コンソール
--------------------

.. function:: alert(message: str, title: str = "PyAE") -> None

   アラートダイアログを表示します。

   :param message: 表示するメッセージ
   :type message: str
   :param title: ダイアログのタイトル（デフォルト: ``"PyAE"``）
   :type title: str

   .. code-block:: python

      ae.alert("Processing complete!")
      ae.alert("Warning: no layers selected", "PyAE Warning")

.. function:: write_ln(message: str) -> None

   コンソール（Info パネル）にメッセージを出力します。

   :param message: 出力するメッセージ
   :type message: str

アンドゥ
--------

.. function:: begin_undo_group(name: str) -> None

   アンドゥグループを開始します。

   このグループ内で行われた変更は、1回のUndo操作でまとめて元に戻せます。
   必ず :func:`end_undo_group` とペアで使用してください。

   :param name: アンドゥ操作の表示名
   :type name: str

   .. code-block:: python

      ae.begin_undo_group("Move Layers")
      # ... 複数の変更操作 ...
      ae.end_undo_group()

.. function:: end_undo_group() -> None

   現在のアンドゥグループを終了します。

.. class:: UndoGroup(name: str)

   アンドゥグループをコンテキストマネージャーとして使用するためのクラスです。
   ``with`` ステートメントで使用すると、ブロックを抜ける際に自動的にアンドゥグループが終了します。

   :param name: アンドゥ操作の表示名
   :type name: str

   .. code-block:: python

      with ae.UndoGroup("Batch Edit"):
          layer.name = "New Name"
          layer.opacity = 50.0

表示更新・再計算
----------------

.. function:: refresh() -> None

   すべてのウィンドウ（コンプビューア含む）を強制リフレッシュします。

   プロパティ変更後やバッチ操作後に、UIに変更を反映させるために使用します。
   内部的に ``PF_RefreshAllWindows()`` を使用します（利用不可の場合は ``AEGP_CauseIdleRoutinesToBeCalled()`` にフォールバック）。

   .. code-block:: python

      # 複数のレイヤーを変更した後に更新
      for layer in comp.layers:
          layer.opacity = 80.0
      ae.refresh()

.. function:: touch_active_item() -> None

   アクティブアイテム（コンポジション/レイヤー）を「変更済み」としてマークし、
   依存するエフェクトやエクスプレッションの再評価をトリガーします。

   プロパティを直接変更した後に、AEに再計算を促すために使用します。

   .. code-block:: python

      # プロパティ変更後に再評価をトリガー
      with ae.UndoGroup("Update Effect"):
          effect.param("Opacity").value = 50.0
          ae.touch_active_item()
          ae.refresh()

.. function:: force_foreground() -> None

   After Effectsをフォアグラウンドに移動します。

   バックグラウンドで実行中のスクリプトが完了した際にユーザーの注意を引くために使用します。

.. seealso::

   - :meth:`Project.set_dirty` - プロジェクトを「変更あり」としてマーク
   - :meth:`Project.save` - プロジェクトを保存

AE情報
------

.. function:: get_ae_info() -> dict[str, Any]

   After Effectsの情報を辞書として取得します。

   :return: AE情報の辞書
   :rtype: dict[str, Any]

   返される辞書のキー:

   .. list-table::
      :header-rows: 1
      :widths: 25 25 50

      * - キー
        - 型
        - 説明
      * - ``driver_major``
        - int
        - AEのメジャーバージョン
      * - ``driver_minor``
        - int
        - AEのマイナーバージョン
      * - ``plugin_id``
        - int
        - プラグインID
      * - ``pyae_version``
        - str
        - PyAEのバージョン文字列

   .. code-block:: python

      info = ae.get_ae_info()
      print(f"AE {info['driver_major']}.{info['driver_minor']}")
      print(f"PyAE {info['pyae_version']}")

.. function:: get_project_path() -> str

   現在開いているプロジェクトのファイルパスを取得します。

   :return: プロジェクトファイルの絶対パス（未保存の場合は空文字列）
   :rtype: str

.. function:: get_documents_folder() -> str

   ユーザーのドキュメントフォルダのパスを取得します。

   :return: ドキュメントフォルダの絶対パス
   :rtype: str

プロジェクト操作
----------------

.. function:: new_project() -> None

   新規プロジェクトを作成します。

   現在のプロジェクトが未保存の場合、保存ダイアログは表示されません。

.. function:: get_project() -> Project

   現在のプロジェクトを取得します。

   :return: 現在のプロジェクト
   :rtype: :class:`Project`

   .. code-block:: python

      project = ae.get_project()
      print(f"Project: {project.name}")
      print(f"Items: {project.num_items}")

.. function:: get_active_comp() -> Optional[Comp]

   アクティブなコンポジションを取得します。

   :return: アクティブなコンポジション。なければ ``None``
   :rtype: Optional[:class:`Comp`]

   .. code-block:: python

      comp = ae.get_active_comp()
      if comp:
          print(f"Active comp: {comp.name}")
          print(f"Layers: {comp.num_layers}")

.. function:: get_active_item() -> Optional[Union[Item, Folder, CompItem, FootageItem]]

   アクティブなアイテムを取得します。

   プロジェクトパネルで選択されているアイテム、またはアクティブなコンポジションのアイテムが返されます。

   :return: アクティブなアイテム。なければ ``None``
   :rtype: Optional[Union[:class:`Item`, :class:`Folder`, :class:`CompItem`, :class:`FootageItem`]]

時間変換
--------

.. function:: frames_to_seconds(frames: int, fps: float) -> float

   フレーム数を秒に変換します。

   :param frames: フレーム数
   :type frames: int
   :param fps: フレームレート（fps）
   :type fps: float
   :return: 秒数
   :rtype: float

   .. code-block:: python

      seconds = ae.frames_to_seconds(150, 29.97)
      print(f"{seconds:.3f}s")  # 5.005s

.. function:: seconds_to_frames(seconds: float, fps: float) -> int

   秒をフレーム数に変換します。

   :param seconds: 秒数
   :type seconds: float
   :param fps: フレームレート（fps）
   :type fps: float
   :return: フレーム数
   :rtype: int

   .. code-block:: python

      frames = ae.seconds_to_frames(5.0, 29.97)
      print(f"{frames} frames")  # 150 frames

.. function:: timecode_from_seconds(seconds: float, fps: float) -> str

   秒をタイムコード文字列に変換します。

   :param seconds: 秒数
   :type seconds: float
   :param fps: フレームレート（fps）
   :type fps: float
   :return: タイムコード文字列（例: ``"00:00:05:00"``）
   :rtype: str

   .. code-block:: python

      tc = ae.timecode_from_seconds(65.5, 24.0)
      print(tc)  # "00:01:05:12"

.. function:: seconds_from_timecode(timecode: str, fps: float) -> float

   タイムコード文字列を秒に変換します。

   :param timecode: タイムコード文字列（例: ``"00:01:05:12"``）
   :type timecode: str
   :param fps: フレームレート（fps）
   :type fps: float
   :return: 秒数
   :rtype: float

   .. code-block:: python

      seconds = ae.seconds_from_timecode("00:01:05:12", 24.0)
      print(f"{seconds:.3f}s")

スクリプト実行
--------------

.. function:: execute_script(script: str) -> Any

   Python文字列をスクリプトとして実行します。

   :param script: 実行するPythonコード
   :type script: str
   :return: 実行結果
   :rtype: Any

.. function:: execute_script_file(path: str) -> Any

   Pythonスクリプトファイルを実行します。

   :param path: スクリプトファイルのパス
   :type path: str
   :return: 実行結果
   :rtype: Any

.. function:: execute_extendscript(script: str) -> str

   ExtendScript (JavaScript) コードを実行し、結果を文字列として取得します。

   :param script: 実行するExtendScriptコード
   :type script: str
   :return: スクリプトの実行結果（文字列）
   :rtype: str

   .. code-block:: python

      result = ae.execute_extendscript("app.project.activeItem.name")
      print(result)  # アクティブアイテムの名前

.. function:: execute_command(command_id: int) -> None

   メニューコマンドをID指定で実行します。

   :param command_id: コマンドID
   :type command_id: int

アイドルタスク
--------------

.. function:: schedule_idle_task(func: Callable[[], None]) -> None

   After Effectsのアイドル時に実行するタスクをスケジュールします。

   AEのメインスレッド上で安全にコードを実行するために使用します。

   :param func: アイドル時に実行する引数なしの関数
   :type func: Callable[[], None]

   .. code-block:: python

      def deferred_task():
          comp = ae.get_active_comp()
          if comp:
              ae.log_info(f"Deferred: {comp.name}")

      ae.schedule_idle_task(deferred_task)

バッチ操作
----------

.. function:: batch_operation() -> batch.Operation

   バッチ操作のコンテキストマネージャーを作成します。

   バッチ操作中はUI更新が抑制され、複数の変更をまとめて効率的に実行できます。

   :return: バッチ操作コンテキストマネージャー
   :rtype: :class:`~ae.batch.Operation`

   .. code-block:: python

      with ae.batch_operation():
          for layer in comp.layers:
              layer.opacity = 100.0
              layer.position = [960, 540]
      # ブロックを抜けるとUI更新が行われる

メモリ診断
----------

.. class:: MemStats

   メモリ使用統計を保持するクラス。

   .. attribute:: handle_count
      :type: int

      割り当てられたメモリハンドル数。

   .. attribute:: total_size
      :type: int

      メモリ総サイズ（バイト）。

.. function:: get_memory_stats() -> MemStats

   現在のメモリ使用統計を取得します。

   :return: メモリ統計
   :rtype: :class:`MemStats`

   .. code-block:: python

      stats = ae.get_memory_stats()
      print(f"Handles: {stats.handle_count}, Size: {stats.total_size}")

.. function:: enable_memory_reporting(enable: bool) -> None

   詳細なメモリレポートの有効/無効を切り替えます（デバッグ用）。

   :param enable: ``True`` で有効、``False`` で無効
   :type enable: bool

.. function:: log_memory_stats(context: str) -> None

   現在のメモリ統計をログ出力します。

   :param context: ログに含めるコンテキスト情報
   :type context: str

.. function:: check_memory_leak(before: MemStats, after: MemStats, tolerance_handles: int = 0, tolerance_size: int = 0) -> bool

   2つのメモリ統計スナップショット間でメモリリークをチェックします。

   :param before: 操作前のメモリ統計
   :type before: :class:`MemStats`
   :param after: 操作後のメモリ統計
   :type after: :class:`MemStats`
   :param tolerance_handles: 許容するハンドル数の増加
   :type tolerance_handles: int
   :param tolerance_size: 許容するサイズの増加（バイト）
   :type tolerance_size: int
   :return: リークが検出された場合 ``True``
   :rtype: bool

   .. code-block:: python

      before = ae.get_memory_stats()
      # ... 何らかの操作 ...
      after = ae.get_memory_stats()

      if ae.check_memory_leak(before, after):
          ae.log_memory_leak_details(before, after, "my operation")

.. function:: log_memory_leak_details(before: MemStats, after: MemStats, context: str) -> None

   詳細なメモリリーク情報をログ出力します。

   :param before: 操作前のメモリ統計
   :type before: :class:`MemStats`
   :param after: 操作後のメモリ統計
   :type after: :class:`MemStats`
   :param context: ログに含めるコンテキスト情報
   :type context: str

関連項目
--------

- :doc:`project` - Project クラス
- :doc:`comp` - Comp クラス
- :doc:`layer` - Layer クラス
- :doc:`enums` - 列挙型・定数
- :doc:`performance` - バッチ・キャッシュ・パフォーマンス
