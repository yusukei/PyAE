# ae.const - Constants
# PyAE - Python for After Effects

from typing import Dict, Tuple

PI: float
"""円周率"""

E: float
"""ネイピア数"""

GOLDEN_RATIO: float
"""黄金比"""

FPS_24: float
"""24.0 fps"""

FPS_25: float
"""25.0 fps"""

FPS_29_97: float
"""29.97 fps"""

FPS_30: float
"""30.0 fps"""

FPS_60: float
"""60.0 fps"""

RESOLUTIONS: Dict[str, Tuple[int, int]]
"""一般的な解像度辞書 (例: 'HD', '4K')"""

__all__ = [
    "PI",
    "E",
    "GOLDEN_RATIO",
    "FPS_24",
    "FPS_25",
    "FPS_29_97",
    "FPS_30",
    "FPS_60",
    "RESOLUTIONS",
]
