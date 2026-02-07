# ae.comp - Composition API
# PyAE - Python for After Effects

from typing import Optional, Union, List, Tuple, Iterator
from .layer import Layer
from .item import Item
from .color_profile import ColorProfile

class Comp:
    """コンポジション操作クラス (Item機能とは別)"""

    def __init__(self) -> None: ...

    # ========================================
    # 静的メソッド
    # ========================================
    @staticmethod
    def get_active() -> Optional["Comp"]:
        """
        アクティブなコンポジションを取得
        Returns:
            Comp object or None
        """
        ...

    @staticmethod
    def create(
        name: str,
        width: int,
        height: int,
        pixel_aspect: float = 1.0,
        duration: float = 10.0,
        frame_rate: float = 30.0,
    ) -> "Comp":
        """
        新しいコンポジションを作成
        """
        ...

    @staticmethod
    def get_most_recently_used() -> Optional["Comp"]:
        """
        最近使用したコンポジションを取得
        Returns:
            Comp object or None
        """
        ...

    # ========================================
    # プロパティ
    # ========================================
    @property
    def valid(self) -> bool: ...
    @property
    def name(self) -> str: ...
    @name.setter
    def name(self, value: str) -> None: ...
    @property
    def width(self) -> int: ...
    @property
    def height(self) -> int: ...
    @property
    def resolution(self) -> Tuple[int, int]:
        """解像度 (width, height)"""
        ...

    @property
    def duration(self) -> float: ...
    @duration.setter
    def duration(self, value: float) -> None: ...
    @property
    def frame_rate(self) -> float: ...
    @frame_rate.setter
    def frame_rate(self, value: float) -> None: ...
    @property
    def current_time(self) -> float:
        """現在の時間 (CTI位置)"""
        ...
    @current_time.setter
    def current_time(self, value: float) -> None: ...
    @property
    def pixel_aspect(self) -> float: ...
    @pixel_aspect.setter
    def pixel_aspect(self, value: float) -> None: ...
    @property
    def bg_color(self) -> Tuple[float, float, float]: ...
    @bg_color.setter
    def bg_color(self, value: Tuple[float, float, float]) -> None: ...
    @property
    def work_area_start(self) -> float: ...
    @work_area_start.setter
    def work_area_start(self, value: float) -> None: ...
    @property
    def work_area_duration(self) -> float: ...
    @work_area_duration.setter
    def work_area_duration(self, value: float) -> None: ...

    # ========================================
    # 新規追加プロパティ (AEGP_CompSuite12完全実装)
    # ========================================
    def set_dimensions(self, width: int, height: int) -> None:
        """コンポジションのサイズを設定"""
        ...

    @property
    def display_start_time(self) -> float:
        """タイムライン表示開始時間"""
        ...
    @display_start_time.setter
    def display_start_time(self, value: float) -> None: ...

    @property
    def show_layer_names(self) -> bool:
        """レイヤー名表示 (True) またはソース名表示 (False)"""
        ...
    @show_layer_names.setter
    def show_layer_names(self, value: bool) -> None: ...

    @property
    def show_blend_modes(self) -> bool:
        """タイムラインにブレンドモードを表示"""
        ...
    @show_blend_modes.setter
    def show_blend_modes(self, value: bool) -> None: ...

    @property
    def display_drop_frame(self) -> bool:
        """ドロップフレームタイムコード表示"""
        ...
    @display_drop_frame.setter
    def display_drop_frame(self, value: bool) -> None: ...

    @property
    def frame_duration(self) -> float:
        """1フレームの継続時間（秒）"""
        ...

    @property
    def shutter_angle_phase(self) -> Tuple[float, float]:
        """シャッター角度と位相のタプル (angle, phase)"""
        ...

    @property
    def motion_blur_samples(self) -> int:
        """推奨モーションブラーサンプル数"""
        ...
    @motion_blur_samples.setter
    def motion_blur_samples(self, value: int) -> None: ...

    @property
    def motion_blur_adaptive_sample_limit(self) -> int:
        """モーションブラー適応サンプル制限"""
        ...
    @motion_blur_adaptive_sample_limit.setter
    def motion_blur_adaptive_sample_limit(self, value: int) -> None: ...

    # ========================================
    # カラープロファイル
    # ========================================
    @property
    def color_profile(self) -> ColorProfile:
        """
        コンポジションの作業用カラースペースのプロファイル

        Example:
            >>> profile = comp.color_profile
            >>> print(profile.description)
            >>> print(f"Gamma: {profile.gamma}")
        """
        ...
    @color_profile.setter
    def color_profile(self, value: Union[ColorProfile, bytes]) -> None:
        """
        コンポジションの作業用カラースペースを設定

        Args:
            value: ColorProfileオブジェクト、またはICCプロファイルデータ（bytes）

        Example:
            >>> # ColorProfileオブジェクトで設定
            >>> profile = ColorProfile.from_icc_data(icc_data)
            >>> comp.color_profile = profile

            >>> # ICCデータを直接設定
            >>> with open("profile.icc", "rb") as f:
            ...     comp.color_profile = f.read()
        """
        ...

    # ========================================
    # レイヤー
    # ========================================
    @property
    def num_layers(self) -> int: ...
    @property
    def layers(self) -> List[Layer]: ...
    @property
    def selected_layers(self) -> List[Layer]: ...
    def layer(self, key: Union[int, str]) -> Optional[Layer]:
        """インデックス(0-based)または名前でレイヤーを取得"""
        ...

    def layer_by_name(self, name: str) -> Optional[Layer]: ...

    # ========================================
    # レイヤー作成
    # ========================================
    def add_solid(
        self,
        name: str,
        width: int,
        height: int,
        color: Tuple[float, float, float] = (1.0, 1.0, 1.0),
        duration: float = -1.0,
    ) -> Layer: ...
    def add_null(self, name: str = "Null", duration: float = -1.0) -> Layer: ...
    def add_camera(
        self, name: str = "Camera", center: Tuple[float, float] = (0.0, 0.0)
    ) -> Layer: ...
    def add_light(
        self, name: str = "Light", center: Tuple[float, float] = (0.0, 0.0)
    ) -> Layer: ...
    def add_text(self, text: str = "") -> Layer: ...
    def add_box_text(
        self, box_width: float, box_height: float, horizontal: bool = True
    ) -> Layer:
        """ボックステキストレイヤーを追加"""
        ...
    def add_shape(self) -> Layer: ...
    def add_layer(self, item: Item, duration: float = -1.0) -> Layer:
        """アイテムからレイヤーを作成"""
        ...

    # ========================================
    # コンポジション操作
    # ========================================
    def duplicate(self) -> "Comp":
        """このコンポジションを複製"""
        ...

    @property
    def _handle(self) -> int:
        """内部ハンドル (int, AEGP_CompH)"""
        ...

    # ========================================
    # Python Protocol Methods
    # ========================================
    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...
    def __eq__(self, other: object) -> bool: ...
    def __hash__(self) -> int: ...
    def __len__(self) -> int: ...
    def __iter__(self) -> Iterator[Layer]: ...
    def __getitem__(self, key: Union[int, str]) -> Layer: ...
    def __contains__(self, name: str) -> bool: ...

    # ========================================
    # Serialization
    # ========================================

    def to_dict(self) -> dict:
        """
        Export composition to dictionary.

        Returns:
            dict: Composition data including layers
        """
        ...

    @staticmethod
    def from_dict(data: dict, parent_folder: Optional["Folder"] = None) -> "Comp":
        """
        Create new composition from dictionary.

        Args:
            data: Comp dictionary (from to_dict())
            parent_folder: Parent folder (optional)

        Returns:
            Comp: Newly created composition
        """
        ...

    def update_from_dict(self, data: dict) -> None:
        """
        Update existing composition from dictionary.

        Args:
            data: Comp dictionary
        """
        ...

__all__ = ["Comp"]
