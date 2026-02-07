# ae.three_d - 3D Layer API
# PyAE - Python for After Effects

from typing import Tuple
from .types import LightType

def is_3d(layer_handle: int) -> bool:
    """Check if layer is 3D"""
    ...

def set_3d(layer_handle: int, is_3d: bool) -> None:
    """Set layer 3D state"""
    ...

def get_position(layer_handle: int) -> Tuple[float, float, float]:
    """Get 3D position (x, y, z)"""
    ...

def set_position(layer_handle: int, position: Tuple[float, float, float]) -> None:
    """Set 3D position"""
    ...

def get_rotation(layer_handle: int) -> Tuple[float, float, float]:
    """Get 3D rotation (x, y, z)"""
    ...

def set_rotation(layer_handle: int, rotation: Tuple[float, float, float]) -> None:
    """Set 3D rotation"""
    ...

def get_scale(layer_handle: int) -> Tuple[float, float, float]:
    """Get 3D scale (x, y, z)"""
    ...

def set_scale(layer_handle: int, scale: Tuple[float, float, float]) -> None:
    """Set 3D scale"""
    ...

def get_orientation(layer_handle: int) -> Tuple[float, float, float]:
    """Get orientation (x, y, z)"""
    ...

def set_orientation(layer_handle: int, orientation: Tuple[float, float, float]) -> None:
    """Set orientation"""
    ...

def is_camera(layer_handle: int) -> bool:
    """Check if layer is a camera"""
    ...

def get_camera_zoom(layer_handle: int) -> float:
    """Get camera zoom"""
    ...

def set_camera_zoom(layer_handle: int, zoom: float) -> None:
    """Set camera zoom"""
    ...

def get_camera_poi(layer_handle: int) -> Tuple[float, float, float]:
    """Get camera point of interest (x, y, z) - Note: Not available via AEGP SDK"""
    ...

def set_camera_poi(layer_handle: int, poi: Tuple[float, float, float]) -> None:
    """Set camera point of interest - Note: Not available via AEGP SDK"""
    ...

def is_light(layer_handle: int) -> bool:
    """Check if layer is a light"""
    ...

def get_light_type(layer_handle: int) -> LightType:
    """Get light type"""
    ...

def get_light_intensity(layer_handle: int) -> float:
    """Get light intensity"""
    ...

def set_light_intensity(layer_handle: int, intensity: float) -> None:
    """Set light intensity"""
    ...

def get_light_color(layer_handle: int) -> Tuple[float, float, float]:
    """Get light color (r, g, b)"""
    ...

def set_light_color(layer_handle: int, color: Tuple[float, float, float]) -> None:
    """Set light color"""
    ...

def get_light_cone_angle(layer_handle: int) -> float:
    """Get spotlight cone angle"""
    ...

def set_light_cone_angle(layer_handle: int, angle: float) -> None:
    """Set spotlight cone angle"""
    ...

def get_light_cone_feather(layer_handle: int) -> float:
    """Get spotlight cone feather"""
    ...

def set_light_cone_feather(layer_handle: int, feather: float) -> None:
    """Set spotlight cone feather"""
    ...

__all__ = [
    "is_3d",
    "set_3d",
    "get_position",
    "set_position",
    "get_rotation",
    "set_rotation",
    "get_scale",
    "set_scale",
    "get_orientation",
    "set_orientation",
    "is_camera",
    "get_camera_zoom",
    "set_camera_zoom",
    "get_camera_poi",
    "set_camera_poi",
    "is_light",
    "get_light_type",
    "get_light_intensity",
    "set_light_intensity",
    "get_light_color",
    "set_light_color",
    "get_light_cone_angle",
    "set_light_cone_angle",
    "get_light_cone_feather",
    "set_light_cone_feather",
]
