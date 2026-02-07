"""
PyAE Sound Data Module

Provides access to audio sample data with support for multiple formats.
"""

from enum import IntEnum
from typing import Dict, Any


class SoundEncoding(IntEnum):
    """Sound encoding format.

    Attributes:
        UNSIGNED_PCM: Unsigned PCM (value=3)
        SIGNED_PCM: Signed PCM (value=4)
        FLOAT: Floating point (value=5)
    """
    UNSIGNED_PCM = 3
    SIGNED_PCM = 4
    FLOAT = 5


class SoundData:
    """Sound data buffer for audio processing.

    Provides access to audio sample data with support for multiple
    formats including PCM and floating point.

    Example::

        # Create sound data
        sound = ae.SoundData.create(
            sample_rate=48000.0,
            encoding=ae.SoundEncoding.FLOAT,
            bytes_per_sample=4,
            num_channels=2
        )

        # Get format info
        print(f"Duration: {sound.duration}s")
        print(f"Sample rate: {sound.sample_rate}")

        # Access samples
        samples = sound.get_samples()

        # Or use context manager for lock/unlock
        with sound:
            # Sound is locked here
            pass
    """

    @property
    def valid(self) -> bool:
        """Check if the sound data handle is valid."""
        ...

    @property
    def sample_rate(self) -> float:
        """Sample rate in Hz (e.g., 44100.0, 48000.0)."""
        ...

    @property
    def encoding(self) -> SoundEncoding:
        """Sound encoding format."""
        ...

    @property
    def bytes_per_sample(self) -> int:
        """Bytes per sample (1, 2, or 4)."""
        ...

    @property
    def num_channels(self) -> int:
        """Number of channels (1 for mono, 2 for stereo)."""
        ...

    @property
    def num_samples(self) -> int:
        """Number of samples per channel."""
        ...

    @property
    def duration(self) -> float:
        """Duration in seconds."""
        ...

    @property
    def format(self) -> Dict[str, Any]:
        """Get format as dictionary.

        Returns:
            Dict with keys: sample_rate, encoding, bytes_per_sample, num_channels
        """
        ...

    def get_samples(self) -> bytes:
        """Get sample data as Python bytes.

        Returns:
            bytes: Raw sample data

        Note:
            The data is automatically copied and the buffer is unlocked after.
            For stereo audio, samples are interleaved (L, R, L, R, ...).
        """
        ...

    def set_samples(self, data: bytes) -> None:
        """Set sample data from Python bytes.

        Args:
            data: Raw sample data

        Note:
            The data size must match exactly: num_samples * num_channels * bytes_per_sample.
            For stereo audio, samples should be interleaved (L, R, L, R, ...).
        """
        ...

    def __enter__(self) -> 'SoundData':
        """Context manager entry - locks samples."""
        ...

    def __exit__(self, exc_type: Any, exc_val: Any, exc_tb: Any) -> None:
        """Context manager exit - unlocks samples."""
        ...

    @property
    def _handle(self) -> int:
        """Internal: Get raw handle as integer."""
        ...

    @staticmethod
    def create(
        sample_rate: float,
        encoding: SoundEncoding,
        bytes_per_sample: int,
        num_channels: int
    ) -> 'SoundData':
        """Create a new sound data buffer.

        Args:
            sample_rate: Sample rate in Hz (e.g., 44100.0, 48000.0)
            encoding: SoundEncoding value (UNSIGNED_PCM, SIGNED_PCM, or FLOAT)
            bytes_per_sample: 1, 2, or 4 (ignored if encoding is FLOAT)
            num_channels: 1 for mono, 2 for stereo

        Returns:
            New SoundData instance
        """
        ...
