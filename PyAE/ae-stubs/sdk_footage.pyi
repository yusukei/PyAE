"""Type stubs for AEGP_FootageSuite functions."""

from typing import Any, Dict

# AEGP_FootageSuite - Basic Footage Access
def AEGP_GetMainFootageFromItem(itemH: int) -> int:
    """Get main footage handle from item. Returns FootageH."""
    ...

def AEGP_GetProxyFootageFromItem(itemH: int) -> int:
    """Get proxy footage handle from item. Returns FootageH. Raises error if no proxy."""
    ...

def AEGP_GetFootageNumFiles(footageH: int) -> tuple[int, int]:
    """Get number of files and files per frame. Returns (num_files, files_per_frame)."""
    ...

def AEGP_GetFootagePath(
    plugin_id: int, footageH: int, frame_num: int, file_index: int
) -> str:
    """Get footage file path for specified frame and file index."""
    ...

def AEGP_GetFootageSignature(footageH: int) -> int:
    """Get footage signature (NONE, MISSING, or SOLID)."""
    ...

# Footage Creation
def AEGP_NewFootage(plugin_id: int, path: str, interp_style: int) -> int:
    """Create new footage from file path. Returns FootageH."""
    ...

def AEGP_NewSolidFootage(name: str, width: int, height: int, color_rgba: tuple[float, float, float, float]) -> int:
    """Create new solid footage. Returns FootageH."""
    ...

def AEGP_NewPlaceholderFootage(plugin_id: int, name: str, width: int, height: int, duration_sec: float) -> int:
    """Create placeholder footage with specified dimensions and duration. Returns FootageH."""
    ...

def AEGP_NewPlaceholderFootageWithPath(
    plugin_id: int, path: str, path_platform: int, file_type: int,
    width: int, height: int, duration_sec: float
) -> int:
    """Create placeholder footage with path. Returns FootageH."""
    ...

# Footage Project Management
def AEGP_AddFootageToProject(footageH: int, folderH: int) -> int:
    """Add footage to project in specified folder. Returns ItemH."""
    ...

def AEGP_ReplaceItemMainFootage(footageH: int, itemH: int) -> None:
    """Replace item's main footage. Footage is adopted by project."""
    ...

def AEGP_SetItemProxyFootage(footageH: int, itemH: int) -> None:
    """Set item's proxy footage. Footage is adopted by project."""
    ...

def AEGP_DisposeFootage(footageH: int) -> None:
    """Dispose footage handle (only if not added to project)."""
    ...

# Footage Interpretation
def AEGP_GetFootageInterpretation(itemH: int, proxy: bool = False) -> Dict[str, Any]:
    """
    Get footage interpretation settings.

    Returns dict with keys:
    - interlace_label: int
    - alpha_label: int
    - pulldown_phase: int
    - loop_count: int
    - pixel_aspect_num: int
    - pixel_aspect_den: int
    - native_fps: float
    - conform_fps: float
    - depth: int
    - motion_detect: bool
    """
    ...

def AEGP_SetFootageInterpretation(itemH: int, proxy: bool, interp_dict: Dict[str, Any]) -> None:
    """
    Set footage interpretation settings.

    interp_dict can contain:
    - interlace_label: int
    - alpha_label: int
    - pulldown_phase: int
    - loop_count: int
    - pixel_aspect_num: int, pixel_aspect_den: int
    - native_fps: float
    - conform_fps: float
    - depth: int
    - motion_detect: bool
    """
    ...

# Solid Footage Attributes
def AEGP_GetSolidFootageColor(itemH: int, proxy: bool = False) -> tuple[float, float, float, float]:
    """Get solid footage color (RGBA). Error if not solid."""
    ...

def AEGP_SetSolidFootageColor(itemH: int, proxy: bool, color_rgba: tuple[float, float, float, float]) -> None:
    """Set solid footage color (RGBA). Error if not solid."""
    ...

def AEGP_SetSolidFootageDimensions(itemH: int, proxy: bool, width: int, height: int) -> None:
    """Set solid footage dimensions (1-30000). Error if not solid."""
    ...

# Footage Layer Key
def AEGP_GetFootageLayerKey(footageH: int) -> Dict[str, Any]:
    """
    Get footage layer key information.

    Returns dict with keys:
    - layer_id: int
    - layer_index: int
    - name: str
    - layer_draw_style: int
    """
    ...

# Sound Data Format
def AEGP_GetFootageSoundDataFormat(footageH: int) -> Dict[str, Any]:
    """
    Get footage sound data format.

    Returns dict with keys:
    - sample_rate: float
    - encoding: int (UNSIGNED_PCM, SIGNED_PCM, FLOAT)
    - bytes_per_sample: int
    - num_channels: int (1=mono, 2=stereo)
    """
    ...

# Sequence Import Options
def AEGP_GetFootageSequenceImportOptions(footageH: int) -> Dict[str, Any]:
    """
    Get footage sequence import options.

    Returns dict with keys:
    - all_in_folder: bool
    - force_alphabetical: bool
    - start_frame: int
    - end_frame: int
    """
    ...

# Footage Signature Constants
AEGP_FootageSignature_NONE: int
AEGP_FootageSignature_MISSING: int
AEGP_FootageSignature_SOLID: int

# Interpretation Style Constants
AEGP_InterpretationStyle_NO_DIALOG_GUESS: int
AEGP_InterpretationStyle_DIALOG_OK: int
AEGP_InterpretationStyle_NO_DIALOG_NO_GUESS: int
