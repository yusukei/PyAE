レンダーモニター
================

.. currentmodule:: ae.render_monitor

``ae.render_monitor`` モジュールは、レンダーキューの進行状況をリアルタイムで監視するための
コールバックベースのAPIを提供します。

概要
----

このモジュールは、低レベルの ``AEGP_RenderQueueMonitorSuite1`` を Python で使いやすく
ラップしたもので、Pythonコールバック関数を使用してレンダリングイベントを監視できます。

**主な機能**:

- レンダリングジョブの開始/終了の監視
- 個別アイテムの進捗監視
- フレーム更新の通知
- ログメッセージの取得
- サムネイル画像の取得

.. important::
   このモジュールの機能は、PyAE独自の拡張であり、低レベルAPIの制限を回避して
   Pythonコールバックを使用可能にしています。

基本的な使い方
--------------

リスナーの登録
~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def on_job_started(session_id):
       """レンダリングジョブ開始時のコールバック"""
       project = ae.render_monitor.get_project_name(session_id)
       print(f"Render started: {project}")

   def on_job_ended(session_id):
       """レンダリングジョブ終了時のコールバック"""
       print(f"Render ended: session {session_id}")

   def on_item_updated(session_id, item_id, frame_id):
       """フレーム更新時のコールバック"""
       print(f"Frame rendered: {frame_id}")

   # コールバックを設定
   ae.render_monitor.set_on_job_started(on_job_started)
   ae.render_monitor.set_on_job_ended(on_job_ended)
   ae.render_monitor.set_on_item_updated(on_item_updated)

   # リスナーを登録
   ae.render_monitor.register_listener()

リスナーの解除
~~~~~~~~~~~~~~

.. code-block:: python

   # リスナーを解除
   ae.render_monitor.unregister_listener()

   # コールバックをクリア
   ae.render_monitor.set_on_job_started(None)
   ae.render_monitor.set_on_job_ended(None)
   ae.render_monitor.set_on_item_updated(None)

API リファレンス
----------------

リスナー管理
~~~~~~~~~~~~

.. function:: register_listener() -> bool

   レンダーキューモニターリスナーを登録します。

   :return: 登録が成功した場合は True
   :rtype: bool

   .. note::
      リスナーを登録する前に、少なくとも1つのコールバックを設定してください。

.. function:: unregister_listener() -> bool

   リスナーを登録解除します。

   :return: 解除が成功した場合は True
   :rtype: bool

.. function:: is_listener_registered() -> bool

   リスナーが登録されているかどうかを確認します。

   :return: 登録されている場合は True
   :rtype: bool

コールバック設定
~~~~~~~~~~~~~~~~

.. function:: set_on_job_started(callback: Optional[Callable[[int], None]]) -> None

   ジョブ開始時のコールバックを設定します。

   :param callback: コールバック関数 ``(session_id) -> None``、または ``None`` で解除
   :type callback: Optional[Callable[[int], None]]

.. function:: set_on_job_ended(callback: Optional[Callable[[int], None]]) -> None

   ジョブ終了時のコールバックを設定します。

   :param callback: コールバック関数 ``(session_id) -> None``、または ``None`` で解除

.. function:: set_on_item_started(callback: Optional[Callable[[int, int], None]]) -> None

   アイテムレンダリング開始時のコールバックを設定します。

   :param callback: コールバック関数 ``(session_id, item_id) -> None``

.. function:: set_on_item_updated(callback: Optional[Callable[[int, int, int], None]]) -> None

   フレーム更新時のコールバックを設定します。

   :param callback: コールバック関数 ``(session_id, item_id, frame_id) -> None``

.. function:: set_on_item_ended(callback: Optional[Callable[[int, int, int], None]]) -> None

   アイテムレンダリング終了時のコールバックを設定します。

   :param callback: コールバック関数 ``(session_id, item_id, status) -> None``

   ``status`` は完了ステータス（``AEGP_RQM_FinishedStatus_*`` 定数）

.. function:: set_on_report_log(callback: Optional[Callable[[int, int, bool, str], None]]) -> None

   ログメッセージ受信時のコールバックを設定します。

   :param callback: コールバック関数 ``(session_id, item_id, is_error, message) -> None``

情報取得
~~~~~~~~

.. function:: get_project_name(session_id: int) -> str

   レンダリング中のプロジェクト名を取得します。

   :param session_id: セッションID
   :return: プロジェクト名

.. function:: get_app_version(session_id: int) -> str

   After Effects のバージョンを取得します。

   :param session_id: セッションID
   :return: バージョン文字列

.. function:: get_num_job_items(session_id: int) -> int

   ジョブ内のアイテム数を取得します。

   :param session_id: セッションID
   :return: アイテム数

.. function:: get_job_item_id(session_id: int, index: int) -> int

   指定インデックスのアイテムIDを取得します。

   :param session_id: セッションID
   :param index: インデックス（0ベース）
   :return: アイテムID

.. function:: get_render_settings(session_id: int, item_id: int) -> List[Tuple[str, str]]

   レンダリング設定を取得します。

   :param session_id: セッションID
   :param item_id: アイテムID
   :return: (設定名, 設定値) タプルのリスト

.. function:: get_num_output_modules(session_id: int, item_id: int) -> int

   出力モジュール数を取得します。

   :param session_id: セッションID
   :param item_id: アイテムID
   :return: 出力モジュール数

.. function:: get_output_module_settings(session_id: int, item_id: int, om_index: int) -> List[Tuple[str, str]]

   出力モジュール設定を取得します。

   :param session_id: セッションID
   :param item_id: アイテムID
   :param om_index: 出力モジュールインデックス
   :return: (設定名, 設定値) タプルのリスト

.. function:: get_thumbnail(session_id: int, item_id: int, frame_id: int, width: int, height: int) -> Tuple[int, int, bytes]

   フレームのサムネイルを取得します。

   :param session_id: セッションID
   :param item_id: アイテムID
   :param frame_id: フレームID
   :param width: 最大幅
   :param height: 最大高さ
   :return: (actual_width, actual_height, jpeg_data) のタプル

実用例
------

レンダリング進捗モニター
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import os
   from datetime import datetime

   class RenderProgressMonitor:
       """レンダリング進捗を監視するクラス"""

       def __init__(self, output_dir: str = None):
           self.output_dir = output_dir
           self.start_time = None
           self.frame_count = 0
           self.setup_callbacks()

       def setup_callbacks(self):
           """コールバックを設定"""
           ae.render_monitor.set_on_job_started(self.on_job_started)
           ae.render_monitor.set_on_job_ended(self.on_job_ended)
           ae.render_monitor.set_on_item_started(self.on_item_started)
           ae.render_monitor.set_on_item_updated(self.on_item_updated)
           ae.render_monitor.set_on_item_ended(self.on_item_ended)
           ae.render_monitor.set_on_report_log(self.on_log)

       def start(self):
           """監視を開始"""
           ae.render_monitor.register_listener()
           print("Render monitor started")

       def stop(self):
           """監視を停止"""
           ae.render_monitor.unregister_listener()
           print("Render monitor stopped")

       def on_job_started(self, session_id):
           self.start_time = datetime.now()
           self.frame_count = 0

           project = ae.render_monitor.get_project_name(session_id)
           app_ver = ae.render_monitor.get_app_version(session_id)
           num_items = ae.render_monitor.get_num_job_items(session_id)

           print("=" * 60)
           print(f"Render Job Started: {self.start_time}")
           print(f"Project: {project}")
           print(f"After Effects: {app_ver}")
           print(f"Items: {num_items}")
           print("=" * 60)

       def on_job_ended(self, session_id):
           elapsed = datetime.now() - self.start_time
           print("=" * 60)
           print(f"Render Job Completed")
           print(f"Total frames: {self.frame_count}")
           print(f"Elapsed time: {elapsed}")
           print("=" * 60)

       def on_item_started(self, session_id, item_id):
           settings = ae.render_monitor.get_render_settings(session_id, item_id)
           print(f"\nItem {item_id} started")
           print(f"Settings: {settings}")

       def on_item_updated(self, session_id, item_id, frame_id):
           self.frame_count += 1

           # サムネイルを保存（オプション）
           if self.output_dir:
               self.save_thumbnail(session_id, item_id, frame_id)

           # 進捗表示
           print(f"  Frame {frame_id} rendered (total: {self.frame_count})")

       def on_item_ended(self, session_id, item_id, status):
           status_names = {
               0: "Unknown",
               1: "Succeeded",
               2: "Aborted",
               3: "Error"
           }
           print(f"\nItem {item_id} ended: {status_names.get(status, 'Unknown')}")

       def on_log(self, session_id, item_id, is_error, message):
           prefix = "ERROR" if is_error else "INFO"
           print(f"[{prefix}] {message}")

       def save_thumbnail(self, session_id, item_id, frame_id):
           """サムネイルを保存"""
           if not self.output_dir:
               return

           try:
               w, h, data = ae.render_monitor.get_thumbnail(
                   session_id, item_id, frame_id, 320, 180)

               filename = f"thumb_{item_id}_{frame_id}.jpg"
               filepath = os.path.join(self.output_dir, filename)

               with open(filepath, 'wb') as f:
                   f.write(data)

           except Exception as e:
               print(f"Failed to save thumbnail: {e}")

   # 使用例
   monitor = RenderProgressMonitor("C:/thumbnails")
   monitor.start()

   # レンダリングを実行...

   # 終了時
   # monitor.stop()

シンプルな通知システム
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def setup_simple_notifications():
       """シンプルなレンダリング通知"""

       def notify_start(session_id):
           project = ae.render_monitor.get_project_name(session_id)
           ae.alert(f"レンダリング開始: {project}")

       def notify_end(session_id):
           ae.alert("レンダリング完了!")

       def notify_error(session_id, item_id, is_error, message):
           if is_error:
               ae.alert(f"レンダリングエラー: {message}")

       ae.render_monitor.set_on_job_started(notify_start)
       ae.render_monitor.set_on_job_ended(notify_end)
       ae.render_monitor.set_on_report_log(notify_error)

       ae.render_monitor.register_listener()

   setup_simple_notifications()

レンダリング統計収集
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import time

   class RenderStats:
       """レンダリング統計を収集"""

       def __init__(self):
           self.items = {}
           self.current_item = None
           self.item_start_time = None

       def start(self):
           ae.render_monitor.set_on_item_started(self.on_item_start)
           ae.render_monitor.set_on_item_updated(self.on_frame)
           ae.render_monitor.set_on_item_ended(self.on_item_end)
           ae.render_monitor.register_listener()

       def stop(self):
           ae.render_monitor.unregister_listener()
           return self.get_summary()

       def on_item_start(self, session_id, item_id):
           self.current_item = item_id
           self.item_start_time = time.time()
           self.items[item_id] = {
               'frames': 0,
               'start_time': self.item_start_time,
               'end_time': None,
               'status': None
           }

       def on_frame(self, session_id, item_id, frame_id):
           if item_id in self.items:
               self.items[item_id]['frames'] += 1

       def on_item_end(self, session_id, item_id, status):
           if item_id in self.items:
               self.items[item_id]['end_time'] = time.time()
               self.items[item_id]['status'] = status

       def get_summary(self):
           """統計サマリーを取得"""
           summary = {
               'total_items': len(self.items),
               'total_frames': sum(i['frames'] for i in self.items.values()),
               'items': []
           }

           for item_id, data in self.items.items():
               duration = (data['end_time'] or time.time()) - data['start_time']
               fps = data['frames'] / duration if duration > 0 else 0

               summary['items'].append({
                   'id': item_id,
                   'frames': data['frames'],
                   'duration': duration,
                   'fps': fps,
                   'status': data['status']
               })

           return summary

   # 使用例
   stats = RenderStats()
   stats.start()

   # レンダリング実行後...
   # summary = stats.stop()
   # print(summary)

注意事項
--------

.. note::
   - コールバック関数はメインスレッドで実行されます
   - コールバック内で長時間の処理を行うと、レンダリングパフォーマンスに影響します
   - リスナーは使用後に必ず解除してください

.. warning::
   - コールバック内で例外が発生すると、リスナーが正常に動作しなくなる可能性があります
   - サムネイル取得は追加のオーバーヘッドがあります。必要な場合のみ使用してください
   - 複数のリスナーを同時に登録することはできません

関連項目
--------

- :doc:`render_queue` - レンダーキュー操作
- :doc:`/api/low-level/AEGP_RenderQueueMonitorSuite1` - 低レベルAPI
- :doc:`async_render` - 非同期レンダリング
