"""
PyAE World (Frame Buffer) Module

Provides access to After Effects frame buffers for image data manipulation.
"""

from enum import IntEnum
from typing import Tuple


class WorldType(IntEnum):
    """Pixel bit depth for World (frame buffer).

    Attributes:
        NONE: Invalid/no world
        BIT8: 8 bits per channel
        BIT16: 16 bits per channel
        BIT32: 32 bits per channel (float)
    """
    NONE = 0
    BIT8 = 1
    BIT16 = 2
    BIT32 = 3


class World:
    """Frame buffer for image data.

    World represents a pixel buffer that can be used for rendering
    and image processing operations.

    Example::

        # Create a new 8-bit world
        world = ae.World.create(ae.WorldType.BIT8, 1920, 1080)

        # Get pixel data
        pixels = world.get_pixels()

        # Get individual pixel
        r, g, b, a = world.get_pixel(100, 100)

        # Set pixel
        world.set_pixel(100, 100, 1.0, 0.0, 0.0, 1.0)  # Red

        # Apply blur
        world.fast_blur(5.0)
    """

    @property
    def valid(self) -> bool:
        """Check if world is valid."""
        ...

    @property
    def type(self) -> WorldType:
        """Get world type (bit depth)."""
        ...

    @property
    def width(self) -> int:
        """Get width in pixels."""
        ...

    @property
    def height(self) -> int:
        """Get height in pixels."""
        ...

    @property
    def size(self) -> Tuple[int, int]:
        """Get size as (width, height) tuple."""
        ...

    @property
    def row_bytes(self) -> int:
        """Get bytes per row (row stride)."""
        ...

    def get_pixels(self) -> bytes:
        """Get all pixel data as bytes.

        Returns raw pixel data in ARGB format.
        The size depends on the world type:
        - BIT8: 4 bytes per pixel
        - BIT16: 8 bytes per pixel
        - BIT32: 16 bytes per pixel (float)

        Returns:
            Raw pixel data as bytes
        """
        ...

    def set_pixels(self, data: bytes) -> None:
        """Set all pixel data from bytes.

        Data size must match row_bytes * height.

        Args:
            data: Raw pixel data as bytes
        """
        ...

    def get_pixel(self, x: int, y: int) -> Tuple[float, float, float, float]:
        """Get pixel value at (x, y).

        Args:
            x: X coordinate (0 to width-1)
            y: Y coordinate (0 to height-1)

        Returns:
            (R, G, B, A) tuple normalized to 0.0-1.0
        """
        ...

    def set_pixel(self, x: int, y: int, r: float, g: float, b: float, a: float) -> None:
        """Set pixel value at (x, y).

        Args:
            x: X coordinate (0 to width-1)
            y: Y coordinate (0 to height-1)
            r: Red component (0.0-1.0)
            g: Green component (0.0-1.0)
            b: Blue component (0.0-1.0)
            a: Alpha component (0.0-1.0)
        """
        ...

    def fast_blur(self, radius: float, flags: int = 0, quality: int = 1) -> None:
        """Apply fast blur effect.

        Args:
            radius: Blur radius in pixels
            flags: Quality flags (0 = default)
            quality: Quality level 0-5 (default 1)
        """
        ...

    @property
    def _handle(self) -> int:
        """Internal: Get raw handle as integer."""
        ...

    @staticmethod
    def create(type: WorldType, width: int, height: int) -> 'World':
        """Create a new world with specified dimensions.

        Args:
            type: WorldType (BIT8, BIT16, or BIT32)
            width: Width in pixels (1-30000)
            height: Height in pixels (1-30000)

        Returns:
            New World instance
        """
        ...
