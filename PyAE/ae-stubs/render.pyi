"""
PyAE Render Module

Provides high-level API for frame rendering in After Effects.
"""

from enum import IntEnum
from typing import Dict, Tuple, Optional
from .world import World, WorldType


class MatteMode(IntEnum):
    """Matte mode for rendering.

    Attributes:
        STRAIGHT: Straight alpha
        PREMUL_BLACK: Premultiplied with black
        PREMUL_BG_COLOR: Premultiplied with background color
    """
    STRAIGHT = 0
    PREMUL_BLACK = 1
    PREMUL_BG_COLOR = 2


class ChannelOrder(IntEnum):
    """Channel order for rendered frames.

    Attributes:
        ARGB: Alpha, Red, Green, Blue
        BGRA: Blue, Green, Red, Alpha
    """
    ARGB = 0
    BGRA = 1


class FieldRender(IntEnum):
    """Field rendering mode.

    Attributes:
        FRAME: Full frame (no field)
        UPPER: Upper field
        LOWER: Lower field
    """
    FRAME = 0
    UPPER = 1
    LOWER = 2


class RenderQuality(IntEnum):
    """Render quality level.

    Attributes:
        DRAFT: Draft quality (faster)
        BEST: Best quality
    """
    DRAFT = 0
    BEST = 1


class RenderOptions:
    """Render options for frame rendering.

    Create from an item using RenderOptions.from_item(item_handle).

    Example::

        # Create options from a composition
        options = ae.RenderOptions.from_item(comp._handle)
        options.time = 2.5  # Set render time to 2.5 seconds
        options.world_type = ae.WorldType.BIT16  # 16bpc
    """

    @property
    def valid(self) -> bool:
        """Check if options are valid."""
        ...

    @property
    def time(self) -> float:
        """Render time in seconds."""
        ...

    @time.setter
    def time(self, value: float) -> None:
        ...

    @property
    def time_step(self) -> float:
        """Time step in seconds (for motion blur)."""
        ...

    @time_step.setter
    def time_step(self, value: float) -> None:
        ...

    @property
    def world_type(self) -> WorldType:
        """World type (bit depth)."""
        ...

    @world_type.setter
    def world_type(self, value: WorldType) -> None:
        ...

    @property
    def field_render(self) -> FieldRender:
        """Field rendering mode."""
        ...

    @field_render.setter
    def field_render(self, value: FieldRender) -> None:
        ...

    @property
    def downsample_factor(self) -> Tuple[int, int]:
        """Downsample factor as (x, y) tuple."""
        ...

    @downsample_factor.setter
    def downsample_factor(self, value: Tuple[int, int]) -> None:
        ...

    @property
    def region_of_interest(self) -> Dict:
        """Region of interest as dict {left, top, right, bottom}."""
        ...

    @region_of_interest.setter
    def region_of_interest(self, value: Dict) -> None:
        ...

    @property
    def matte_mode(self) -> MatteMode:
        """Matte mode."""
        ...

    @matte_mode.setter
    def matte_mode(self, value: MatteMode) -> None:
        ...

    @property
    def channel_order(self) -> ChannelOrder:
        """Channel order (ARGB or BGRA)."""
        ...

    @channel_order.setter
    def channel_order(self, value: ChannelOrder) -> None:
        ...

    @property
    def render_guide_layers(self) -> bool:
        """Whether to render guide layers."""
        ...

    @render_guide_layers.setter
    def render_guide_layers(self, value: bool) -> None:
        ...

    @property
    def render_quality(self) -> RenderQuality:
        """Render quality."""
        ...

    @render_quality.setter
    def render_quality(self, value: RenderQuality) -> None:
        ...

    def duplicate(self) -> 'RenderOptions':
        """Create a duplicate of this render options."""
        ...

    @property
    def _handle(self) -> int:
        """Internal: raw handle."""
        ...

    @staticmethod
    def from_item(item_handle: int) -> 'RenderOptions':
        """Create render options from an item (composition or footage).

        Args:
            item_handle: The item handle (AEGP_ItemH as integer)

        Returns:
            New RenderOptions instance
        """
        ...


class FrameReceipt:
    """Receipt for a rendered frame.

    Use as a context manager for automatic checkin::

        with ae.Renderer.render_frame(options) as receipt:
            world = receipt.world
            pixels = world.get_pixels()
        # Frame is automatically checked in after the with block
    """

    @property
    def valid(self) -> bool:
        """Check if receipt is valid (not checked in)."""
        ...

    @property
    def world(self) -> World:
        """Get the rendered world (frame buffer). Read-only."""
        ...

    @property
    def rendered_region(self) -> Dict:
        """Get the rendered region as dict {left, top, right, bottom}."""
        ...

    @property
    def guid(self) -> str:
        """Get the GUID of this rendered frame."""
        ...

    def checkin(self) -> None:
        """Explicitly check in the frame (release resources)."""
        ...

    def __enter__(self) -> 'FrameReceipt':
        ...

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        ...


class Renderer:
    """Static methods for rendering frames.

    Example::

        # Create render options
        options = ae.RenderOptions.from_item(comp._handle)
        options.time = 1.0

        # Render and get the frame
        with ae.Renderer.render_frame(options) as receipt:
            world = receipt.world
            print(f'Rendered: {world.width}x{world.height}')
    """

    @staticmethod
    def render_frame(options: RenderOptions) -> FrameReceipt:
        """Render a frame using RenderOptions.

        Args:
            options: Render options configured for the desired frame

        Returns:
            FrameReceipt. Use as context manager for automatic cleanup.
        """
        ...

    @staticmethod
    def render_layer_frame(layer_options_handle: int) -> FrameReceipt:
        """Render a layer frame using LayerRenderOptions handle.

        Args:
            layer_options_handle: LayerRenderOptions handle as integer

        Returns:
            FrameReceipt. Use as context manager for automatic cleanup.
        """
        ...

    @staticmethod
    def get_current_timestamp() -> Tuple[int, int, int, int]:
        """Get current project timestamp for cache validation.

        Returns:
            Tuple of 4 integers representing the timestamp.
        """
        ...

    @staticmethod
    def has_item_changed_since_timestamp(
        item_handle: int,
        start_time: float,
        duration: float,
        timestamp: Tuple[int, int, int, int]
    ) -> bool:
        """Check if an item has changed since a timestamp.

        Args:
            item_handle: Item handle as integer
            start_time: Start time in seconds
            duration: Duration in seconds
            timestamp: Timestamp tuple from get_current_timestamp()

        Returns:
            True if the item's video has changed.
        """
        ...

    @staticmethod
    def is_item_worthwhile_to_render(
        options: RenderOptions,
        timestamp: Tuple[int, int, int, int]
    ) -> bool:
        """Check if an item is worthwhile to render.

        Args:
            options: Render options for the item
            timestamp: Timestamp tuple from get_current_timestamp()

        Returns:
            True if rendering is recommended.
        """
        ...

    @staticmethod
    def is_rendered_frame_sufficient(
        rendered_options: RenderOptions,
        proposed_options: RenderOptions
    ) -> bool:
        """Check if a rendered frame meets proposed requirements.

        Args:
            rendered_options: Options used for existing render
            proposed_options: Proposed new options

        Returns:
            True if existing render is sufficient.
        """
        ...
