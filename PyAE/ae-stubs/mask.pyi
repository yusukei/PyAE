# ae.mask - Mask API
# PyAE - Python for After Effects

from typing import Optional, Tuple
from .types import MaskMode

class MaskVertex:
    """マスク頂点クラス"""

    x: float
    """X座標"""

    y: float
    """Y座標"""

    in_tangent_x: float
    """入力タンジェントX"""

    in_tangent_y: float
    """入力タンジェントY"""

    out_tangent_x: float
    """出力タンジェントX"""

    out_tangent_y: float
    """出力タンジェントY"""

    def __init__(
        self,
        x: float = 0.0,
        y: float = 0.0,
        in_tangent_x: float = 0.0,
        in_tangent_y: float = 0.0,
        out_tangent_x: float = 0.0,
        out_tangent_y: float = 0.0,
    ) -> None: ...

class Mask:
    """マスククラス"""

    @property
    def valid(self) -> bool:
        """マスクが有効かどうか"""
        ...

    @property
    def name(self) -> str:
        """マスク名"""
        ...

    @property
    def index(self) -> int:
        """マスクインデックス（0から開始）"""
        ...

    @property
    def mask_id(self) -> int:
        """マスクID"""
        ...

    @property
    def mode(self) -> MaskMode:
        """マスクモード"""
        ...
    @mode.setter
    def mode(self, value: MaskMode) -> None: ...
    @property
    def inverted(self) -> bool:
        """反転"""
        ...
    @inverted.setter
    def inverted(self, value: bool) -> None: ...
    @property
    def locked(self) -> bool:
        """ロック状態"""
        ...
    @locked.setter
    def locked(self, value: bool) -> None: ...
    @property
    def is_roto_bezier(self) -> bool:
        """RotoBezier状態"""
        ...
    @is_roto_bezier.setter
    def is_roto_bezier(self, value: bool) -> None: ...
    @property
    def opacity(self) -> float:
        """不透明度（0-100）"""
        ...
    @opacity.setter
    def opacity(self, value: float) -> None: ...
    @property
    def feather(self) -> Tuple[float, float]:
        """フェザー (x, y)"""
        ...
    @feather.setter
    def feather(self, value: Tuple[float, float]) -> None: ...

    @property
    def color(self) -> Tuple[float, float, float]:
        """マスク色 (red, green, blue) - 0.0-1.0範囲"""
        ...
    @color.setter
    def color(self, value: Tuple[float, float, float]) -> None: ...

    @property
    def expansion(self) -> float:
        """拡張（ピクセル）"""
        ...
    @expansion.setter
    def expansion(self, value: float) -> None: ...
    @property
    def outline(self) -> int:
        """Mask outline handle (for MaskOutlineSuite3 functions)"""
        ...

    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...

    def delete(self) -> None:
        """マスクを削除"""
        ...

    def duplicate(self) -> "Mask":
        """マスクを複製"""
        ...

__all__ = ["Mask", "MaskVertex"]
