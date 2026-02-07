# ae.cache - Property Cache API
# PyAE - Python for After Effects

def clear() -> None:
    """プロパティキャッシュをクリア"""
    ...

def size() -> int:
    """キャッシュされたエントリ数を取得"""
    ...

def set_ttl(milliseconds: int) -> None:
    """キャッシュTTLをミリ秒で設定"""
    ...

__all__ = [
    "clear",
    "size",
    "set_ttl",
]
