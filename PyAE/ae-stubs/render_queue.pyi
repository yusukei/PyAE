# ae.render_queue - RenderQueue API
# PyAE - Python for After Effects

from typing import Optional, List, Union
from .item import Comp, CompItem
from .types import RenderStatus, EmbedOptions

class OutputModule:
    """出力モジュールクラス"""

    def __init__(self) -> None: ...
    @property
    def valid(self) -> bool:
        """有効かどうか"""
        ...

    @property
    def file_path(self) -> str:
        """出力ファイルパス"""
        ...
    @file_path.setter
    def file_path(self, value: str) -> None: ...
    @property
    def embed_options(self) -> EmbedOptions:
        """埋め込みオプション"""
        ...
    @embed_options.setter
    def embed_options(self, value: EmbedOptions) -> None: ...

    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...

class RenderQueueItem:
    """レンダーキューアイテムクラス"""

    def __init__(self) -> None: ...
    @property
    def valid(self) -> bool:
        """有効かどうか"""
        ...

    @property
    def comp_name(self) -> str:
        """関連するコンポジション名"""
        ...

    @property
    def status(self) -> RenderStatus:
        """レンダーステータス"""
        ...

    @property
    def queued(self) -> bool:
        """レンダリング待機中かどうか"""
        ...
    @queued.setter
    def queued(self, value: bool) -> None: ...
    @property
    def num_output_modules(self) -> int:
        """出力モジュール数"""
        ...

    def output_module(self, index: int = 0) -> Optional[OutputModule]:
        """
        インデックスで出力モジュールを取得
        Args:
            index: インデックス（0から開始）
        """
        ...

    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...

    def delete(self) -> None:
        """レンダーキューアイテムを削除"""
        ...

# Module-level functions (matching C++ submodule)

def items() -> List[RenderQueueItem]:
    """レンダーキューアイテムのリスト"""
    ...

def num_items() -> int:
    """レンダーキューアイテム数"""
    ...

def item(index: int) -> Optional[RenderQueueItem]:
    """
    インデックスでレンダーキューアイテムを取得
    Args:
        index: インデックス（0から開始）
    """
    ...

def add_comp(comp: Union[Comp, CompItem], path: str = "") -> Optional[RenderQueueItem]:
    """
    コンポジションをレンダーキューに追加
    Args:
        comp: 追加するコンポジション
        path: 出力パス（オプション）
    Returns:
        作成されたレンダーキューアイテム
    """
    ...

def render_settings_templates() -> List[str]:
    """レンダー設定テンプレート名リスト (現在は空リスト)"""
    ...

def output_module_templates() -> List[str]:
    """出力モジュールテンプレート名リスト (現在は空リスト)"""
    ...

__all__ = [
    "RenderQueueItem",
    "OutputModule",
    "items",
    "num_items",
    "item",
    "add_comp",
    "render_settings_templates",
    "output_module_templates",
]
