# ae.perf - Performance Monitoring API
# PyAE - Python for After Effects

from typing import Dict

def stats() -> Dict[str, Dict[str, float]]:
    """
    パフォーマンス統計を取得

    Returns:
        操作名をキーとする辞書。各値は以下のキーを持つ辞書:
        - count: 操作回数
        - total_ms: 合計時間(ms)
        - avg_ms: 平均時間(ms)
        - max_ms: 最大時間(ms)
        - min_ms: 最小時間(ms)
    """
    ...

def reset() -> None:
    """パフォーマンス統計をリセット"""
    ...

__all__ = [
    "stats",
    "reset",
]
