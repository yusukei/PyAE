"""
PyAE Footage Module

Provides access to After Effects footage creation and management.

Unified Footage class handles both pre-project and in-project states:
- Before project: Created via factory methods (from_file, solid, etc.)
- In project: After add_to_project() or retrieved from project items

Use is_in_project property to check the current state.
"""

from enum import IntEnum
from typing import Dict, Tuple, Optional
from .item import Folder


class FootageSignature(IntEnum):
    """Footage signature (source state before project).

    Attributes:
        NONE: No footage / invalid
        MISSING: Source file is missing
        SOLID: Solid color footage
    """
    NONE = 0
    MISSING = 1
    SOLID = 2


class FootageType(IntEnum):
    """Footage type (in-project state).

    Attributes:
        None_: Invalid or unknown
        Solid: Solid color footage
        Missing: Source file is missing
        Placeholder: Placeholder footage
    """
    None_ = 0
    Solid = 1
    Missing = 2
    Placeholder = 3


class InterpretationStyle(IntEnum):
    """Footage interpretation style.

    Attributes:
        NO_DIALOG_NO_GUESS: Use stored interpretation
        DIALOG_OK: Show dialog if needed
        NO_DIALOG_GUESS: Guess without dialog (default)
    """
    NO_DIALOG_NO_GUESS = 0
    DIALOG_OK = 1
    NO_DIALOG_GUESS = 2


class Footage:
    """Unified footage class for both pre-project and in-project states.

    A Footage object can be in one of two states:
    - Before project: Created via factory methods (from_file, solid, etc.)
    - In project: After add_to_project() or retrieved from project.items

    Use is_in_project property to check the current state.

    Example::

        # Create and add footage
        footage = ae.Footage.from_file('/path/to/video.mp4')
        print(footage.is_in_project)  # False

        footage.add_to_project()
        print(footage.is_in_project)  # True

        # Now you can access in-project properties
        footage.name = 'My Video'
        footage.set_interpretation('premultiplied', fps=29.97)

        # Create a solid
        solid = ae.Footage.solid('Red', 1920, 1080, (1.0, 0.0, 0.0, 1.0))
        solid.add_to_project()
        solid.set_solid_color((0.5, 0.5, 0.5, 1.0))
    """

    # =========================================================
    # State Properties
    # =========================================================

    @property
    def valid(self) -> bool:
        """Check if footage is valid."""
        ...

    @property
    def is_in_project(self) -> bool:
        """Check if footage has been added to project.

        Returns:
            True if in project, False if before project.
        """
        ...

    # =========================================================
    # Common Properties (both states)
    # =========================================================

    @property
    def path(self) -> str:
        """Get file path (for file-based footage)."""
        ...

    @property
    def layer_key(self) -> Dict:
        """Get layer key info (for layered formats like PSD).

        Returns:
            Dict with keys: layer_id, layer_index, name, layer_draw_style
        """
        ...

    @property
    def sound_format(self) -> Dict:
        """Get sound data format info.

        Returns:
            Dict with keys: sample_rate, encoding, bytes_per_sample, num_channels
        """
        ...

    @property
    def sequence_options(self) -> Dict:
        """Get sequence import options.

        Returns:
            Dict with keys: is_sequence, force_alphabetical, start_frame, end_frame
        """
        ...

    # =========================================================
    # Pre-project Properties
    # =========================================================

    @property
    def num_files(self) -> Tuple[int, int]:
        """Get number of files as (total_files, files_per_frame) tuple.

        Only available before adding to project.
        """
        ...

    @property
    def signature(self) -> FootageSignature:
        """Get footage signature (NONE, MISSING, or SOLID)."""
        ...

    # =========================================================
    # Project Operation
    # =========================================================

    def add_to_project(self, folder: Optional["Folder"] = None) -> int:
        """Add footage to project.

        Args:
            folder: Parent folder item handle (0 for root)

        Returns:
            Created item handle

        Note:
            After calling, is_in_project becomes True and
            in-project properties/methods become available.
        """
        ...

    # =========================================================
    # In-project Properties
    # =========================================================

    @property
    def footage_type(self) -> FootageType:
        """Get footage type (Solid, Missing, Placeholder, None).

        Only available after adding to project.
        """
        ...

    @property
    def interpretation(self) -> Dict:
        """Get interpretation rules.

        Returns:
            Dict with keys: alpha_mode, premul_color (if premul), native_fps, conform_fps

        Only available after adding to project.
        """
        ...

    def set_interpretation(self, alpha_mode: str = "", fps: float = 0.0) -> None:
        """Set interpretation rules.

        Args:
            alpha_mode: 'straight', 'premultiplied', or 'ignore'
            fps: Conform frame rate (0.0 = no change)

        Only available after adding to project.
        """
        ...

    def get_solid_color(self, proxy: bool = False) -> Tuple[float, float, float, float]:
        """Get solid color (RGBA, 0.0-1.0).

        Args:
            proxy: Use proxy instead of main

        Returns:
            (r, g, b, a) tuple

        Only available for solid footage in project.
        """
        ...

    def set_solid_color(self, color_rgba: Tuple[float, float, float, float], proxy: bool = False) -> None:
        """Set solid color.

        Args:
            color_rgba: (r, g, b, a) tuple (0.0-1.0)
            proxy: Use proxy instead of main

        Only available for solid footage in project.
        """
        ...

    def set_solid_dimensions(self, width: int, height: int, proxy: bool = False) -> None:
        """Set solid dimensions.

        Args:
            width: Width in pixels (1-30000)
            height: Height in pixels (1-30000)
            proxy: Use proxy instead of main

        Only available for solid footage in project.
        """
        ...

    # =========================================================
    # Item Properties (in-project only)
    # =========================================================

    @property
    def name(self) -> str:
        """Get item name. Only available in project."""
        ...

    @name.setter
    def name(self, value: str) -> None:
        """Set item name. Only available in project."""
        ...

    @property
    def comment(self) -> str:
        """Get item comment. Only available in project."""
        ...

    @comment.setter
    def comment(self, value: str) -> None:
        """Set item comment. Only available in project."""
        ...

    @property
    def label(self) -> int:
        """Get item label color index. Only available in project."""
        ...

    @label.setter
    def label(self, value: int) -> None:
        """Set item label color index. Only available in project."""
        ...

    @property
    def selected(self) -> bool:
        """Get item selection state. Only available in project."""
        ...

    @selected.setter
    def selected(self, value: bool) -> None:
        """Set item selection state. Only available in project."""
        ...

    @property
    def width(self) -> int:
        """Get width in pixels. Only available in project."""
        ...

    @property
    def height(self) -> int:
        """Get height in pixels. Only available in project."""
        ...

    @property
    def duration(self) -> float:
        """Get duration in seconds. Only available in project."""
        ...

    @property
    def frame_rate(self) -> float:
        """Get frame rate. Only available in project."""
        ...

    @property
    def missing(self) -> bool:
        """Check if source file is missing. Only available in project."""
        ...

    @property
    def item_id(self) -> int:
        """Get item ID. Only available in project."""
        ...

    def delete(self) -> None:
        """Delete from project. Only available in project."""
        ...

    # =========================================================
    # Internal Handle Access
    # =========================================================

    @property
    def _handle(self) -> int:
        """Internal: Get raw handle as integer.

        Returns item handle if in project, otherwise footage handle.
        """
        ...

    @property
    def _item_handle(self) -> int:
        """Internal: Get item handle as integer."""
        ...

    @property
    def _footage_handle(self) -> int:
        """Internal: Get footage handle as integer."""
        ...

    # =========================================================
    # Factory Methods
    # =========================================================

    @staticmethod
    def from_file(path: str, style: InterpretationStyle = InterpretationStyle.NO_DIALOG_GUESS) -> 'Footage':
        """Create footage from file.

        Args:
            path: File path
            style: Interpretation style (default: NO_DIALOG_GUESS)

        Returns:
            New Footage instance (not yet in project)
        """
        ...

    @staticmethod
    def solid(name: str, width: int, height: int, color: Tuple[float, float, float, float]) -> 'Footage':
        """Create solid color footage.

        Args:
            name: Solid name
            width: Width in pixels
            height: Height in pixels
            color: RGBA tuple (0.0-1.0)

        Returns:
            New Footage instance (not yet in project)
        """
        ...

    @staticmethod
    def placeholder(name: str, width: int, height: int, duration: float) -> 'Footage':
        """Create placeholder footage.

        Args:
            name: Placeholder name
            width: Width in pixels
            height: Height in pixels
            duration: Duration in seconds

        Returns:
            New Footage instance (not yet in project)
        """
        ...

    @staticmethod
    def placeholder_with_path(path: str, width: int, height: int, duration: float) -> 'Footage':
        """Create placeholder footage with file path.

        Args:
            path: Placeholder file path
            width: Width in pixels
            height: Height in pixels
            duration: Duration in seconds

        Returns:
            New Footage instance (not yet in project)
        """
        ...


# Backward compatibility alias
FootageItem = Footage
"""Deprecated: Use Footage instead. FootageItem has been unified with Footage."""
