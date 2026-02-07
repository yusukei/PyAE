# ae.persistent_data - Persistent Data (Preferences) API
# PyAE - Python for After Effects

from typing import List

class Section:
    """永続データストレージ内のセクション"""

    @property
    def name(self) -> str:
        """セクション名"""
        ...

    def get_string(self, key: str, default: str = "") -> str:
        """文字列値を取得"""
        ...

    def set_string(self, key: str, value: str) -> None:
        """文字列値を設定"""
        ...

    def get_int(self, key: str, default: int = 0) -> int:
        """整数値を取得"""
        ...

    def set_int(self, key: str, value: int) -> None:
        """整数値を設定"""
        ...

    def get_float(self, key: str, default: float = 0.0) -> float:
        """浮動小数点値を取得"""
        ...

    def set_float(self, key: str, value: float) -> None:
        """浮動小数点値を設定"""
        ...

    def __contains__(self, key: str) -> bool:
        """キーが存在するか確認"""
        ...

    def delete(self, key: str) -> None:
        """キーを削除"""
        ...

    def keys(self) -> List[str]:
        """セクション内のすべてのキーを取得"""
        ...

class Blob:
    """永続データBlob（セクションのコンテナ）"""

    def __getitem__(self, section: str) -> Section:
        """セクション名でセクションを取得"""
        ...

    def get_string(self, section: str, key: str, default: str = "") -> str:
        """文字列値を取得"""
        ...

    def set_string(self, section: str, key: str, value: str) -> None:
        """文字列値を設定"""
        ...

    def get_int(self, section: str, key: str, default: int = 0) -> int:
        """整数値を取得"""
        ...

    def set_int(self, section: str, key: str, value: int) -> None:
        """整数値を設定"""
        ...

    def get_float(self, section: str, key: str, default: float = 0.0) -> float:
        """浮動小数点値を取得"""
        ...

    def set_float(self, section: str, key: str, value: float) -> None:
        """浮動小数点値を設定"""
        ...

    def contains(self, section: str, key: str) -> bool:
        """キーが存在するか確認"""
        ...

    def delete(self, section: str, key: str) -> None:
        """キーを削除"""
        ...

    def sections(self) -> List[str]:
        """すべてのセクション名を取得"""
        ...

# Blob type constants
MACHINE_SPECIFIC: int
MACHINE_INDEPENDENT: int
MACHINE_INDEPENDENT_RENDER: int
MACHINE_INDEPENDENT_OUTPUT: int
MACHINE_INDEPENDENT_COMPOSITION: int
MACHINE_SPECIFIC_TEXT: int
MACHINE_SPECIFIC_PAINT: int

def get_machine_specific() -> Blob:
    """マシン固有の環境設定Blobを取得"""
    ...

def get_machine_independent() -> Blob:
    """マシン非依存の環境設定Blobを取得"""
    ...

def get_prefs_directory() -> str:
    """環境設定ディレクトリパスを取得"""
    ...

def get_blob(blob_type: int) -> Blob:
    """タイプを指定して環境設定Blobを取得"""
    ...

__all__ = [
    "Section",
    "Blob",
    "MACHINE_SPECIFIC",
    "MACHINE_INDEPENDENT",
    "MACHINE_INDEPENDENT_RENDER",
    "MACHINE_INDEPENDENT_OUTPUT",
    "MACHINE_INDEPENDENT_COMPOSITION",
    "MACHINE_SPECIFIC_TEXT",
    "MACHINE_SPECIFIC_PAINT",
    "get_machine_specific",
    "get_machine_independent",
    "get_prefs_directory",
    "get_blob",
]
