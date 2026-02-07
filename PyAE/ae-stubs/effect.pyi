# ae.effect - Effect API
# PyAE - Python for After Effects

from typing import Optional, Any, Union, List, Iterator
from .property import Property

class Effect:
    """エフェクトクラス"""

    def __init__(self) -> None: ...
    @property
    def valid(self) -> bool:
        """エフェクトが有効かどうか"""
        ...

    @property
    def name(self) -> str:
        """エフェクト表示名"""
        ...

    @property
    def match_name(self) -> str:
        """マッチ名（エフェクトの固有識別子）"""
        ...

    @property
    def index(self) -> int:
        """エフェクトインデックス（0から開始）"""
        ...

    @property
    def enabled(self) -> bool:
        """有効/無効状態"""
        ...
    @enabled.setter
    def enabled(self, value: bool) -> None: ...
    @property
    def num_properties(self) -> int:
        """パラメータ数"""
        ...

    @property
    def properties(self) -> List[Property]:
        """全パラメータのリスト"""
        ...

    def property(self, name: str) -> Property:
        """
        名前でパラメータを取得
        Args:
            name: パラメータ名
        """
        ...

    def property_by_index(self, index: int) -> Property:
        """
        インデックスでパラメータを取得
        Args:
            index: パラメータインデックス
        """
        ...

    @property
    def stream_name(self) -> str:
        """エフェクトストリーム名（set_nameで設定された名前）"""
        ...

    def set_name(self, name: str) -> None:
        """
        エフェクト名を設定
        Args:
            name: 新しい名前
        """
        ...

    # Effect Masks (AE 13.0+)
    @property
    def num_masks(self) -> int:
        """エフェクトに割り当てられたマスク数"""
        ...

    def get_mask_id(self, mask_index: int) -> int:
        """
        指定インデックスのマスクIDを取得
        Args:
            mask_index: マスクインデックス
        Returns:
            マスクID
        """
        ...

    def add_mask(self, mask_id: int) -> int:
        """
        エフェクトにマスクを追加
        Args:
            mask_id: マスクID
        Returns:
            ストリームハンドル
        """
        ...

    def remove_mask(self, mask_id: int) -> None:
        """
        エフェクトからマスクを削除
        Args:
            mask_id: マスクID
        """
        ...

    def set_mask(self, mask_index: int, mask_id: int) -> int:
        """
        指定インデックスのマスクを設定
        Args:
            mask_index: マスクインデックス
            mask_id: マスクID
        Returns:
            ストリームハンドル
        """
        ...

    # UI Update
    def update_params_ui(self) -> None:
        """
        エフェクトにパラメータUI更新を通知

        パラメータ値を変更した後に呼び出すことで、
        エフェクトのUI表示を更新します。
        内部的に AEGP_EffectCallGeneric(PF_Cmd_UPDATE_PARAMS_UI) を使用。
        """
        ...

    # ========================================
    # Python Protocol Methods
    # ========================================
    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...
    def __len__(self) -> int: ...
    def __iter__(self) -> Iterator[Property]: ...
    def __getitem__(self, key: Union[int, str]) -> Property: ...
    def __contains__(self, name: str) -> bool: ...

    def duplicate(self) -> "Effect":
        """エフェクトを複製"""
        ...

    def delete(self) -> None:
        """エフェクトを削除"""
        ...

    # ========================================
    # Serialization
    # ========================================

    def to_dict(self) -> dict:
        """
        Export effect to dictionary.

        Returns:
            dict: Effect data including parameters
        """
        ...

    def update_from_dict(self, data: dict) -> None:
        """
        Update effect from dictionary.

        Args:
            data: Effect dictionary
        """
        ...

def get_installed_effects() -> List[dict]:
    """インストール済みエフェクトのリストを取得"""
    ...

__all__ = ["Effect", "get_installed_effects"]
