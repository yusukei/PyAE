"""
PyAE Layer Render Options Module

Provides layer-specific render options for rendering individual layers.
"""

from typing import Tuple
from .world import WorldType
from .render import MatteMode


class LayerRenderOptions:
    """Layer rendering options for individual layer rendering.

    Unlike RenderOptions which is for composition-level rendering,
    LayerRenderOptions focuses on individual layer render control.

    Example::

        # Create options from a layer
        layer = comp.layers[0]
        options = ae.LayerRenderOptions.from_layer(layer._handle)
        options.time = 2.5
        options.world_type = ae.WorldType.BIT16

        # Render the layer
        receipt = ae.Renderer.render_layer_frame(options._handle)
    """

    @property
    def valid(self) -> bool:
        """Check if the options handle is valid."""
        ...

    @property
    def time(self) -> float:
        """Get render time in seconds."""
        ...

    @time.setter
    def time(self, value: float) -> None:
        """Set render time in seconds."""
        ...

    @property
    def time_step(self) -> float:
        """Get time step for motion blur in seconds."""
        ...

    @time_step.setter
    def time_step(self, value: float) -> None:
        """Set time step for motion blur in seconds."""
        ...

    @property
    def world_type(self) -> WorldType:
        """Get world type (bit depth): NONE, BIT8, BIT16, or BIT32."""
        ...

    @world_type.setter
    def world_type(self, value: WorldType) -> None:
        """Set world type (bit depth): NONE, BIT8, BIT16, or BIT32."""
        ...

    @property
    def downsample_factor(self) -> Tuple[int, int]:
        """Get downsample factor as (x, y) tuple. 1=100%, 2=50%, etc."""
        ...

    @downsample_factor.setter
    def downsample_factor(self, value: Tuple[int, int]) -> None:
        """Set downsample factor as (x, y) tuple. 1=100%, 2=50%, etc."""
        ...

    @property
    def matte_mode(self) -> MatteMode:
        """Get matte mode: STRAIGHT, PREMUL_BLACK, or PREMUL_BG_COLOR."""
        ...

    @matte_mode.setter
    def matte_mode(self, value: MatteMode) -> None:
        """Set matte mode: STRAIGHT, PREMUL_BLACK, or PREMUL_BG_COLOR."""
        ...

    def duplicate(self) -> 'LayerRenderOptions':
        """Create a duplicate of these options.

        Returns:
            New LayerRenderOptions instance
        """
        ...

    @property
    def _handle(self) -> int:
        """Internal: Get raw handle as integer."""
        ...

    @staticmethod
    def from_layer(layer_handle: int) -> 'LayerRenderOptions':
        """Create render options from a layer.

        Args:
            layer_handle: Layer handle (int)

        Returns:
            New LayerRenderOptions instance

        Initial values:
            - Time: Layer's current time
            - TimeStep: Layer's frame duration
            - EffectsToRender: "all"
        """
        ...

    @staticmethod
    def from_upstream_of_effect(effect_handle: int) -> 'LayerRenderOptions':
        """Create render options from upstream of an effect.

        Args:
            effect_handle: Effect handle (int)

        Returns:
            New LayerRenderOptions instance

        Note:
            EffectsToRender is set to the specified effect's index.
        """
        ...

    @staticmethod
    def from_downstream_of_effect(effect_handle: int) -> 'LayerRenderOptions':
        """Create render options from downstream of an effect.

        Args:
            effect_handle: Effect handle (int)

        Returns:
            New LayerRenderOptions instance

        Warning:
            This function can only be called from the UI thread.
        """
        ...
