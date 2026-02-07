"""
ae.marker - Marker API
"""

from typing import Optional

class Marker:
    """
    マーカークラス.
    AEGP_NewMarker がサポートする全てのパラメータへのアクセスを提供します。
    """

    def __init__(self) -> None:
        """
        新しいマーカーを作成します。
        """
        ...

    @property
    def comment(self) -> str:
        """マーカーコメント"""
        ...
    @comment.setter
    def comment(self, value: str) -> None: ...
    @property
    def chapter(self) -> str:
        """チャプター名"""
        ...
    @chapter.setter
    def chapter(self, value: str) -> None: ...
    @property
    def url(self) -> str:
        """URL"""
        ...
    @url.setter
    def url(self, value: str) -> None: ...
    @property
    def frame_target(self) -> str:
        """フレームターゲット"""
        ...
    @frame_target.setter
    def frame_target(self, value: str) -> None: ...
    @property
    def cue_point_name(self) -> str:
        """キューポイント名"""
        ...
    @cue_point_name.setter
    def cue_point_name(self, value: str) -> None: ...
    @property
    def duration(self) -> float:
        """デュレーション（秒）"""
        ...
    @duration.setter
    def duration(self, value: float) -> None: ...
    @property
    def label(self) -> int:
        """ラベル番号 (0-16)"""
        ...
    @label.setter
    def label(self, value: int) -> None: ...
    @property
    def protected(self) -> bool:
        """保護領域フラグ"""
        ...
    @protected.setter
    def protected(self, value: bool) -> None: ...
    @property
    def navigation(self) -> bool:
        """ナビゲーションフラグ"""
        ...
    @navigation.setter
    def navigation(self, value: bool) -> None: ...
    @property
    def _handle(self) -> int:
        """Internal: AEGP_MarkerValP (uintptr_t)"""
        ...
