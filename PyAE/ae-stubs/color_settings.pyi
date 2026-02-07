# ae.color_settings - Color Settings Submodule
# PyAE - Python for After Effects
#
# OCIO and color management settings

from typing import Tuple


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
