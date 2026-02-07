# ae module type stubs
# PyAE - Python for After Effects

from typing import Optional, Any, Callable, Union, List
from .project import Project
from .item import Item, CompItem, Folder, FootageItem
from .comp import Comp
from .layer import Layer
from .effect import Effect
from .mask import Mask, MaskVertex
from .property import Property, Keyframe, BezierTangent, SpatialTangent
from .render_queue import RenderQueueItem, OutputModule
from .marker import Marker
from .color_profile import ColorProfile
from .world import World, WorldType
from .footage import Footage, FootageSignature, FootageType, InterpretationStyle
from .render import (
    RenderOptions, FrameReceipt, Renderer,
    MatteMode, ChannelOrder, FieldRender, RenderQuality
)
from .layer_render_options import LayerRenderOptions
from .sound_data import SoundData, SoundEncoding
from .three_d import *
from .types import (
    LayerType,
    ItemType,
    MaskMode,
    RenderStatus,
    PropertyType,
    StreamValueType,
    StreamGroupingType,
    KeyInterpolation,
    EasingType,
    LayerQuality,
    SamplingQuality,
    BlendMode,
    TrackMatteMode,
    EmbedOptions,
    LightType,
)
from . import const
from . import sdk
from . import marker
from . import color_settings
from . import persistent_data
from . import menu
from . import render_monitor
from . import batch
from . import cache
from . import perf
from . import async_render
from . import render_queue
from . import render
from . import three_d
from . import world
from . import sound_data
from . import layer_render_options
from . import color_profile
from . import footage

# バージョン情報
def version() -> str:
    """PyAEバージョン文字列を取得"""
    ...

def is_initialized() -> bool:
    """プラグインが初期化されているか確認"""
    ...

# ログ関数
def log(level: str, message: str) -> None:
    """メッセージをログ出力"""
    ...

def log_debug(message: str) -> None:
    """デバッグメッセージをログ出力"""
    ...

def log_info(message: str) -> None:
    """情報メッセージをログ出力"""
    ...

def log_warning(message: str) -> None:
    """警告メッセージをログ出力"""
    ...

def log_error(message: str) -> None:
    """エラーメッセージをログ出力"""
    ...

# アラート・コンソール
def alert(message: str, title: str = "PyAE") -> None:
    """アラートダイアログを表示"""
    ...

def write_ln(message: str) -> None:
    """コンソールに出力"""
    ...

# アンドゥグループ
def begin_undo_group(name: str) -> None:
    """アンドゥグループを開始"""
    ...

def end_undo_group() -> None:
    """現在のアンドゥグループを終了"""
    ...

# 表示更新
def refresh() -> None:
    """すべてのウィンドウを強制リフレッシュ（コンプビューア含む）"""
    ...

def touch_active_item() -> None:
    """アクティブアイテムを「変更済み」としてマークし、再評価をトリガー"""
    ...

def force_foreground() -> None:
    """After Effectsをフォアグラウンドに移動"""
    ...

# AE情報
def get_ae_info() -> dict[str, Any]:
    """After Effects情報を取得"""
    ...

def get_project_path() -> str:
    """現在のプロジェクトファイルパスを取得"""
    ...

def get_documents_folder() -> str:
    """ユーザーのドキュメントフォルダパスを取得"""
    ...

# プロジェクト操作
def new_project() -> None:
    """新規プロジェクトを作成"""
    ...

# 時間変換
def frames_to_seconds(frames: int, fps: float) -> float:
    """フレームを秒に変換"""
    ...

def seconds_to_frames(seconds: float, fps: float) -> int:
    """秒をフレームに変換"""
    ...

def timecode_from_seconds(seconds: float, fps: float) -> str:
    """秒をタイムコード文字列に変換"""
    ...

# Layer Quality constants
AEGP_LayerQual_NONE: int
AEGP_LayerQual_WIREFRAME: int
AEGP_LayerQual_DRAFT: int
AEGP_LayerQual_BEST: int

# Layer Sampling Quality constants
AEGP_LayerSamplingQual_BILINEAR: int
AEGP_LayerSamplingQual_BICUBIC: int

# Layer Label constants
AEGP_Label_NONE: int
AEGP_Label_NO_LABEL: int
AEGP_Label_1: int
AEGP_Label_2: int
AEGP_Label_3: int
AEGP_Label_4: int
AEGP_Label_5: int
AEGP_Label_6: int
AEGP_Label_7: int
AEGP_Label_8: int
AEGP_Label_9: int
AEGP_Label_10: int
AEGP_Label_11: int
AEGP_Label_12: int
AEGP_Label_13: int
AEGP_Label_14: int
AEGP_Label_15: int
AEGP_Label_16: int

# トラックマット定数
TRACK_MATTE_NO_TRACK_MATTE: int
TRACK_MATTE_ALPHA: int
TRACK_MATTE_NOT_ALPHA: int
TRACK_MATTE_LUMA: int
TRACK_MATTE_NOT_LUMA: int

# Transfer Mode (Blend Mode) constants
PF_Xfer_NONE: int
PF_Xfer_COPY: int
PF_Xfer_BEHIND: int
PF_Xfer_IN_FRONT: int
PF_Xfer_DISSOLVE: int
PF_Xfer_ADD: int
PF_Xfer_MULTIPLY: int
PF_Xfer_SCREEN: int
PF_Xfer_OVERLAY: int
PF_Xfer_SOFT_LIGHT: int
PF_Xfer_HARD_LIGHT: int
PF_Xfer_DARKEN: int
PF_Xfer_LIGHTEN: int
PF_Xfer_DIFFERENCE: int
PF_Xfer_HUE: int
PF_Xfer_SATURATION: int
PF_Xfer_COLOR: int
PF_Xfer_LUMINOSITY: int
PF_Xfer_MULTIPLY_ALPHA: int
PF_Xfer_MULTIPLY_ALPHA_LUMA: int
PF_Xfer_MULTIPLY_NOT_ALPHA: int
PF_Xfer_MULTIPLY_NOT_ALPHA_LUMA: int
PF_Xfer_ADDITIVE_PREMUL: int
PF_Xfer_ALPHA_ADD: int
PF_Xfer_COLOR_DODGE: int
PF_Xfer_COLOR_BURN: int
PF_Xfer_EXCLUSION: int
PF_Xfer_DIFFERENCE2: int
PF_Xfer_COLOR_DODGE2: int
PF_Xfer_COLOR_BURN2: int
PF_Xfer_LINEAR_DODGE: int
PF_Xfer_LINEAR_BURN: int
PF_Xfer_LINEAR_LIGHT: int
PF_Xfer_VIVID_LIGHT: int
PF_Xfer_PIN_LIGHT: int
PF_Xfer_HARD_MIX: int
PF_Xfer_LIGHTER_COLOR: int
PF_Xfer_DARKER_COLOR: int
PF_Xfer_SUBTRACT: int
PF_Xfer_DIVIDE: int

def seconds_from_timecode(timecode: str, fps: float) -> float:
    """タイムコード文字列を秒に変換"""
    ...

# アイドルタスク
def schedule_idle_task(func: Callable[[], None]) -> None:
    """アイドル時に実行するタスクをスケジュール"""
    ...

# スクリプト実行
def execute_script_file(path: str) -> Any:
    """Pythonスクリプトファイルを実行"""
    ...

def execute_script(script: str) -> Any:
    """Python文字列を実行"""
    ...

def execute_extendscript(script: str) -> str:
    """
    ExtendScript (JavaScript) コードを実行

    Args:
        script: 実行する ExtendScript コード

    Returns:
        スクリプトの実行結果（文字列）

    Example:
        >>> result = ae.execute_extendscript("app.project.activeItem.name")
        >>> print(result)  # アクティブアイテムの名前
    """
    ...

def execute_command(command_id: int) -> None:
    """
    メニューコマンドを実行
    Args:
        command_id: コマンドID
    """
    ...

# プロジェクト・アイテム取得
def get_project() -> Project:
    """現在のプロジェクトを取得"""
    ...

def get_active_comp() -> Optional[Comp]:
    """アクティブなコンポジションを取得"""
    ...

def get_active_item() -> Optional[Union[Item, Folder, CompItem, FootageItem]]:
    """アクティブなアイテムを取得"""
    ...

# メモリ診断
class MemStats:
    """メモリ使用統計"""
    handle_count: int
    """割り当てられたメモリハンドル数"""
    total_size: int
    """メモリ総サイズ（バイト）"""
    def __repr__(self) -> str: ...

def get_memory_stats() -> MemStats:
    """現在のメモリ使用統計を取得"""
    ...

def enable_memory_reporting(enable: bool) -> None:
    """詳細なメモリレポートの有効/無効を切り替え（デバッグ用）"""
    ...

def log_memory_stats(context: str) -> None:
    """現在のメモリ統計をログ出力"""
    ...

def check_memory_leak(
    before: MemStats,
    after: MemStats,
    tolerance_handles: int = 0,
    tolerance_size: int = 0,
) -> bool:
    """2つのメモリ統計スナップショット間でメモリリークをチェック"""
    ...

def log_memory_leak_details(
    before: MemStats, after: MemStats, context: str
) -> None:
    """詳細なメモリリーク情報をログ出力"""
    ...

# バッチ操作
def batch_operation() -> "batch.Operation":
    """バッチ操作コンテキストマネージャーを作成"""
    ...

# アンドゥグループ（コンテキストマネージャ）
class UndoGroup:
    """アンドゥグループをコンテキストマネージャーとして使用"""
    def __init__(self, name: str) -> None: ...
    def __enter__(self) -> "UndoGroup": ...
    def __exit__(self, exc_type: Any, exc_val: Any, exc_tb: Any) -> None: ...

__all__ = [
    # 関数
    "version",
    "is_initialized",
    "log",
    "log_debug",
    "log_info",
    "log_warning",
    "log_error",
    "alert",
    "write_ln",
    "begin_undo_group",
    "end_undo_group",
    "refresh",
    "get_ae_info",
    "get_project_path",
    "get_documents_folder",
    "new_project",
    "frames_to_seconds",
    "seconds_to_frames",
    "timecode_from_seconds",
    "seconds_from_timecode",
    "schedule_idle_task",
    "execute_script_file",
    "execute_script",
    "execute_extendscript",
    "execute_command",
    "get_project",
    "get_active_comp",
    "get_active_item",
    # クラス
    "UndoGroup",
    "Project",
    "Item",
    "Comp",
    "CompItem",
    "Folder",
    "FootageItem",
    "Layer",
    "Effect",
    "Mask",
    "MaskVertex",
    "Property",
    "Keyframe",
    "BezierTangent",
    "SpatialTangent",
    "RenderQueueItem",
    "OutputModule",
    "Marker",
    "ColorProfile",
    "World",
    "Footage",
    "RenderOptions",
    "FrameReceipt",
    "Renderer",
    "LayerRenderOptions",
    "SoundData",
    # Enum
    "WorldType",
    "SoundEncoding",
    "FootageSignature",
    "InterpretationStyle",
    "MatteMode",
    "ChannelOrder",
    "FieldRender",
    "RenderQuality",
    "LayerType",
    "ItemType",
    "FootageType",
    "MaskMode",
    "RenderStatus",
    "PropertyType",
    "StreamValueType",
    "StreamGroupingType",
    "KeyInterpolation",
    "EasingType",
    "LayerQuality",
    "SamplingQuality",
    "BlendMode",
    "TrackMatteMode",
    "EmbedOptions",
    "LightType",
    "MemStats",
    "get_memory_stats",
    "enable_memory_reporting",
    "log_memory_stats",
    "check_memory_leak",
    "log_memory_leak_details",
    "batch_operation",
    # Submodules
    "const",
    "sdk",
    "color_settings",
    "persistent_data",
    "menu",
    "render_monitor",
    "batch",
    "cache",
    "perf",
    "async_render",
    "render_queue",
    "render",
    "three_d",
    "world",
    "sound_data",
    "layer_render_options",
    "color_profile",
    "footage",
    "marker",
]
