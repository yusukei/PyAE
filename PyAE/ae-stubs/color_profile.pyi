# ae.color_profile - Color Profile API
# PyAE - Python for After Effects
#
# High-level API for color profile management and OCIO settings

from typing import Tuple

class ColorProfile:
    """
    カラープロファイルクラス

    作業用カラースペースのカラープロファイルを管理します。
    ICC プロファイルデータの読み書き、プロファイル情報の取得などを提供します。

    Example:
        >>> # コンポジションのカラープロファイルを取得
        >>> comp = ae.Comp.get_active()
        >>> profile = comp.color_profile
        >>> print(profile.description)
        >>> print(f"Gamma: {profile.gamma}")

        >>> # ICC プロファイルからColorProfileを作成
        >>> with open("sRGB.icc", "rb") as f:
        ...     icc_data = f.read()
        >>> profile = ColorProfile.from_icc_data(icc_data)
        >>> comp.color_profile = profile
    """

    @property
    def valid(self) -> bool:
        """カラープロファイルが有効かどうか"""
        ...

    @property
    def description(self) -> str:
        """
        カラープロファイルの説明（人間が読める形式）

        Example:
            >>> profile.description
            'sRGB IEC61966-2.1'
        """
        ...

    @property
    def gamma(self) -> float:
        """
        カラープロファイルのおおよそのガンマ値

        Example:
            >>> profile.gamma
            2.2
        """
        ...

    @property
    def is_rgb(self) -> bool:
        """
        RGBカラープロファイルかどうか

        Example:
            >>> profile.is_rgb
            True
        """
        ...

    def to_icc_data(self) -> bytes:
        """
        カラープロファイルをICCプロファイルデータとしてエクスポート

        Returns:
            bytes: ICCプロファイルデータ

        Example:
            >>> icc_data = profile.to_icc_data()
            >>> with open("exported.icc", "wb") as f:
            ...     f.write(icc_data)
        """
        ...

    @staticmethod
    def from_icc_data(icc_data: bytes) -> "ColorProfile":
        """
        ICCプロファイルデータからColorProfileを作成

        Args:
            icc_data: ICCプロファイルデータ（bytes）

        Returns:
            ColorProfile: 新しいColorProfileオブジェクト

        Example:
            >>> with open("AdobeRGB.icc", "rb") as f:
            ...     icc_data = f.read()
            >>> profile = ColorProfile.from_icc_data(icc_data)
        """
        ...

    @staticmethod
    def from_comp(comp_handle: int) -> "ColorProfile":
        """
        コンポジションの作業用カラースペースからColorProfileを取得

        Args:
            comp_handle: コンポジションハンドル（内部使用）

        Returns:
            ColorProfile: コンポジションのカラープロファイル
        """
        ...


# ===========================================
# ae.color_settings サブモジュール
# ===========================================

def is_ocio_enabled() -> bool:
    """
    OCIO（OpenColorIO）カラーマネジメントが有効かどうかを確認

    Returns:
        bool: OCIOが使用されている場合はTrue

    Note:
        After Effects 24.0以降でのみ利用可能

    Example:
        >>> import ae
        >>> if ae.color_settings.is_ocio_enabled():
        ...     print("OCIO is enabled")
    """
    ...


def get_ocio_config_file() -> str:
    """
    OCIO設定ファイル名を取得

    Returns:
        str: OCIO設定ファイル名（パスなし）

    Note:
        After Effects 24.0以降でのみ利用可能

    Example:
        >>> ae.color_settings.get_ocio_config_file()
        'aces_1.2_config.ocio'
    """
    ...


def get_ocio_config_file_path() -> str:
    """
    OCIO設定ファイルのフルパスを取得

    Returns:
        str: OCIO設定ファイルのフルパス

    Note:
        After Effects 24.0以降でのみ利用可能

    Example:
        >>> ae.color_settings.get_ocio_config_file_path()
        'C:/Program Files/Adobe/Adobe After Effects 2024/Support Files/OCIOConfigs/aces_1.2_config.ocio'
    """
    ...


def get_ocio_working_color_space() -> str:
    """
    OCIO作業用カラースペース名を取得

    Returns:
        str: 作業用カラースペース名（例: "ACEScg"）

    Note:
        After Effects 24.0以降でのみ利用可能

    Example:
        >>> ae.color_settings.get_ocio_working_color_space()
        'ACEScg'
    """
    ...


def get_ocio_display_view() -> Tuple[str, str]:
    """
    OCIOディスプレイとビューのカラースペース名を取得

    Returns:
        Tuple[str, str]: (display, view) のタプル

    Note:
        After Effects 24.0以降でのみ利用可能

    Example:
        >>> display, view = ae.color_settings.get_ocio_display_view()
        >>> print(f"Display: {display}, View: {view}")
        Display: sRGB, View: ACES 1.0 SDR-video
    """
    ...


def is_color_space_aware_effects_enabled() -> bool:
    """
    カラースペース対応エフェクトが有効かどうかを確認

    Returns:
        bool: カラースペース対応エフェクトが有効な場合はTrue

    Note:
        After Effects 24.0以降でのみ利用可能
    """
    ...


def get_lut_interpolation_method() -> int:
    """
    LUT（Look-Up Table）補間方式を取得

    Returns:
        int: LUT補間方式（整数値）

    Note:
        After Effects 24.0以降でのみ利用可能
    """
    ...


def get_graphics_white_luminance() -> int:
    """
    グラフィックホワイト輝度値を取得

    Returns:
        int: ホワイト輝度値（cd/m²、例: 100, 300）

    Note:
        After Effects 24.0以降でのみ利用可能

    Example:
        >>> luminance = ae.color_settings.get_graphics_white_luminance()
        >>> print(f"White luminance: {luminance} cd/m²")
    """
    ...


def get_working_color_space_id() -> bytes:
    """
    作業用カラースペースのGUID（16バイト）を取得

    Returns:
        bytes: GUID（16バイト）

    Note:
        After Effects 24.0以降でのみ利用可能

    Example:
        >>> guid = ae.color_settings.get_working_color_space_id()
        >>> print(f"GUID: {guid.hex()}")
    """
    ...


__all__ = [
    "ColorProfile",
    # color_settings functions
    "is_ocio_enabled",
    "get_ocio_config_file",
    "get_ocio_config_file_path",
    "get_ocio_working_color_space",
    "get_ocio_display_view",
    "is_color_space_aware_effects_enabled",
    "get_lut_interpolation_method",
    "get_graphics_white_luminance",
    "get_working_color_space_id",
]
