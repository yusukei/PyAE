# ae.render_monitor - Render Queue Monitor API
# PyAE - Python for After Effects

from typing import Optional, Callable, List, Tuple

# Finished status constants
STATUS_UNKNOWN: int
STATUS_SUCCEEDED: int
STATUS_ABORTED: int
STATUS_ERRED: int

# Query functions
def get_project_name(session_id: int = 0) -> str:
    """レンダーセッションのプロジェクト名を取得"""
    ...

def get_app_version(session_id: int = 0) -> str:
    """After Effectsバージョンを取得"""
    ...

def get_num_job_items(session_id: int = 0) -> int:
    """レンダーキュー内のジョブアイテム数を取得"""
    ...

def get_job_item_id(session_id: int = 0, index: int = 0) -> int:
    """インデックスでジョブアイテムIDを取得"""
    ...

def get_render_settings(
    session_id: int = 0, item_id: int = 0
) -> List[Tuple[str, str]]:
    """ジョブアイテムのレンダー設定を取得（(名前, 値)タプルのリスト）"""
    ...

def get_num_output_modules(session_id: int = 0, item_id: int = 0) -> int:
    """ジョブアイテムの出力モジュール数を取得"""
    ...

def get_output_module_settings(
    session_id: int = 0, item_id: int = 0, om_index: int = 0
) -> List[Tuple[str, str]]:
    """出力モジュール設定を取得（(名前, 値)タプルのリスト）"""
    ...

def get_thumbnail(
    session_id: int = 0,
    item_id: int = 0,
    frame_id: int = 0,
    max_width: int = 320,
    max_height: int = 240,
) -> Tuple[int, int, bytes]:
    """
    フレームサムネイルをJPEGとして取得

    Returns:
        (width, height, jpeg_bytes) のタプル
    """
    ...

# Listener functions
def register_listener() -> bool:
    """
    レンダーキューモニターリスナーを登録

    コールバックを受信するにはこれを呼ぶ必要があります。
    完了時にunregister_listener()でクリーンアップしてください。

    Returns:
        登録が成功した場合True
    """
    ...

def unregister_listener() -> bool:
    """レンダーキューモニターリスナーを解除"""
    ...

def is_listener_registered() -> bool:
    """リスナーが現在登録されているか確認"""
    ...

# Callback setters
def set_on_job_started(callback: Optional[Callable[[int], None]]) -> None:
    """
    レンダージョブ開始時のコールバックを設定

    Args:
        callback: (session_id: int) を受け取る関数。クリアするにはNone。
    """
    ...

def set_on_job_ended(callback: Optional[Callable[[int], None]]) -> None:
    """
    レンダージョブ終了時のコールバックを設定

    Args:
        callback: (session_id: int) を受け取る関数。クリアするにはNone。
    """
    ...

def set_on_item_started(callback: Optional[Callable[[int, int], None]]) -> None:
    """
    レンダーアイテム開始時のコールバックを設定

    Args:
        callback: (session_id: int, item_id: int) を受け取る関数。クリアするにはNone。
    """
    ...

def set_on_item_updated(callback: Optional[Callable[[int, int, int], None]]) -> None:
    """
    レンダーアイテムフレーム更新時のコールバックを設定

    Args:
        callback: (session_id: int, item_id: int, frame_id: int) を受け取る関数。クリアするにはNone。

    Note:
        レンダリング中に頻繁に呼ばれる可能性があります。
    """
    ...

def set_on_item_ended(callback: Optional[Callable[[int, int, int], None]]) -> None:
    """
    レンダーアイテム終了時のコールバックを設定

    Args:
        callback: (session_id: int, item_id: int, finished_status: int) を受け取る関数。クリアするにはNone。

    finished_status の値:
        - STATUS_UNKNOWN (0)
        - STATUS_SUCCEEDED (1)
        - STATUS_ABORTED (2)
        - STATUS_ERRED (3)
    """
    ...

def set_on_report_log(
    callback: Optional[Callable[[int, int, bool, str], None]]
) -> None:
    """
    レンダーログメッセージのコールバックを設定

    Args:
        callback: (session_id: int, item_id: int, is_error: bool, message: str) を受け取る関数。クリアするにはNone。
    """
    ...

__all__ = [
    "STATUS_UNKNOWN",
    "STATUS_SUCCEEDED",
    "STATUS_ABORTED",
    "STATUS_ERRED",
    "get_project_name",
    "get_app_version",
    "get_num_job_items",
    "get_job_item_id",
    "get_render_settings",
    "get_num_output_modules",
    "get_output_module_settings",
    "get_thumbnail",
    "register_listener",
    "unregister_listener",
    "is_listener_registered",
    "set_on_job_started",
    "set_on_job_ended",
    "set_on_item_started",
    "set_on_item_updated",
    "set_on_item_ended",
    "set_on_report_log",
]
