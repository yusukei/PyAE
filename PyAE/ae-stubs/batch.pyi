# ae.batch - Batch Operations and Performance API
# PyAE - Python for After Effects

from typing import Any, Dict

class Operation:
    """
    バッチ操作のコンテキストマネージャー

    Example:
        with ae.batch.Operation() as op:
            # operations are batched
            ...
            op.commit()  # explicit commit (optional)
    """
    def __init__(self) -> None: ...
    def __enter__(self) -> "Operation": ...
    def __exit__(self, exc_type: Any, exc_val: Any, exc_tb: Any) -> None: ...
    def commit(self) -> None:
        """コンテキスト終了前に操作をコミット"""
        ...
    def rollback(self) -> None:
        """コンテキスト終了前に操作をロールバック"""
        ...

# Batch operation functions
def begin() -> None:
    """バッチ操作を開始"""
    ...

def end() -> None:
    """現在のバッチ操作を終了"""
    ...

def commit() -> None:
    """保留中のすべての操作をコミット"""
    ...

def rollback() -> None:
    """保留中のすべての操作をロールバック"""
    ...

def is_active() -> bool:
    """バッチモードがアクティブか確認"""
    ...

def pending_count() -> int:
    """保留中の操作数を取得"""
    ...

__all__ = [
    "Operation",
    "begin",
    "end",
    "commit",
    "rollback",
    "is_active",
    "pending_count",
]
