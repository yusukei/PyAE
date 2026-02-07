# ae.project - Project API
# PyAE - Python for After Effects

from typing import Optional, Union, List, Dict, Any
from .item import Item, Folder, CompItem, FootageItem

class Project:
    """After Effects プロジェクト"""

    def __init__(self) -> None: ...

    # ========================================
    # プロパティ
    # ========================================

    @property
    def valid(self) -> bool:
        """プロジェクトが有効かどうか"""
        ...

    @property
    def name(self) -> str:
        """プロジェクト名"""
        ...

    @property
    def path(self) -> str:
        """プロジェクトファイルパス（保存されていない場合は空文字列）"""
        ...

    @property
    def bit_depth(self) -> int:
        """ビット深度 (8, 16, 32)"""
        ...
    @bit_depth.setter
    def bit_depth(self, value: int) -> None: ...

    @property
    def is_dirty(self) -> bool:
        """プロジェクトが最後の保存以降に変更されたかどうか"""
        ...

    def set_dirty(self) -> None:
        """プロジェクトを「未保存の変更あり」としてマーク"""
        ...

    @property
    def items(self) -> List[Union[Item, Folder, CompItem, FootageItem]]:
        """全アイテムのリスト"""
        ...

    @property
    def root_folder(self) -> Folder:
        """ルートフォルダ"""
        ...

    @property
    def active_item(self) -> Optional[Union[Item, Folder, CompItem, FootageItem]]:
        """アクティブなアイテム（選択されていない場合は None）"""
        ...

    @property
    def num_items(self) -> int:
        """アイテム数"""
        ...

    # ========================================
    # 時間表示
    # ========================================

    def get_time_display(self) -> Dict[str, Any]:
        """時間表示設定を取得"""
        ...

    def set_time_display(self, time_display: Dict[str, Any]) -> None:
        """時間表示設定を変更（Undoable）"""
        ...

    # ========================================
    # メソッド
    # ========================================

    def item_by_name(
        self, name: str
    ) -> Optional[Union[Item, Folder, CompItem, FootageItem]]:
        """
        名前でアイテムを検索
        Args:
            name: アイテム名
        Returns:
            見つかったアイテム、見つからない場合は None
        """
        ...

    def create_folder(
        self, name: str, parent_folder: Optional[Union[Folder, Item]] = None
    ) -> Folder:
        """
        新しいフォルダを作成
        Args:
            name: フォルダ名
            parent_folder: 親フォルダ（省略時はルート）
        Returns:
            作成されたフォルダ
        """
        ...

    def create_comp(
        self,
        name: str,
        width: int,
        height: int,
        pixel_aspect: float = 1.0,
        duration: float = 10.0,
        frame_rate: float = 30.0,
        parent_folder: Optional[Union[Folder, Item]] = None,
    ) -> Optional[CompItem]:
        """
        新しいコンポジションを作成
        """
        ...

    def import_file(
        self,
        file_path: str,
        parent_folder: Optional[Union[Folder, Item]] = None,
        sequence_options: Optional[Dict[str, Any]] = None,
    ) -> Optional[FootageItem]:
        """
        ファイルをインポート
        Args:
            file_path: ファイルパス
            parent_folder: 親フォルダ（省略時はルート）
            sequence_options: シーケンスオプション辞書
                - is_sequence: bool
                - force_alphabetical: bool
                - start_frame: int
                - end_frame: int
        """
        ...

    def save(self) -> bool:
        """
        プロジェクトを保存
        Returns: 成功したかどうか
        """
        ...

    def save_as(self, file_path: str) -> bool:
        """
        名前を付けて保存
        Args:
            file_path: 保存先パス
        Returns: 成功したかどうか
        """
        ...

    @staticmethod
    def get_current() -> "Project":
        """現在のプロジェクトを取得"""
        ...

    @staticmethod
    def is_open() -> bool:
        """プロジェクトが開かれているか"""
        ...

    @staticmethod
    def num_projects() -> int:
        """プロジェクト数を取得"""
        ...

    @staticmethod
    def get_by_index(index: int) -> "Project":
        """インデックスでプロジェクトを取得"""
        ...

    @staticmethod
    def new_project() -> "Project":
        """新しいプロジェクトを作成"""
        ...

    @staticmethod
    def open_project(file_path: str) -> "Project":
        """プロジェクトファイルを開く"""
        ...

    # ========================================
    # Serialization
    # ========================================

    def to_dict(self) -> Dict[str, Any]:
        """
        Export project to dictionary.

        Returns:
            dict: Project data with version and items list
        """
        ...

    @staticmethod
    def from_dict(data: Dict[str, Any]) -> "Project":
        """
        Import project items from dictionary into current project.

        Args:
            data: Project dictionary (from to_dict())

        Returns:
            Project: The current project
        """
        ...

    def update_from_dict(self, data: Dict[str, Any]) -> None:
        """
        Update existing project from dictionary.

        Args:
            data: Project dictionary
        """
        ...

__all__ = ["Project"]
