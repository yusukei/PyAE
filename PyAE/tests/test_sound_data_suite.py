# test_sound_data_suite.py
# PyAE SoundData Suite tests

import ae

try:
    from .test_utils import (
        TestSuite,
        assert_true,
        assert_equal,
        assert_not_none,
        assert_isinstance,
        assert_greater_than,
        assert_close,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_equal,
        assert_not_none,
        assert_isinstance,
        assert_greater_than,
        assert_close,
    )

suite = TestSuite("SoundData Suite")

# Shared state for tests
_sound_data = {}


@suite.setup
def setup():
    """Create SoundData instances for testing"""
    # Stereo float sound data at 48kHz
    _sound_data["stereo_float"] = ae.SoundData.create(
        sample_rate=48000.0,
        encoding=ae.SoundEncoding.FLOAT,
        bytes_per_sample=4,
        num_channels=2,
    )
    # Mono signed PCM sound data at 44100Hz
    _sound_data["mono_pcm"] = ae.SoundData.create(
        sample_rate=44100.0,
        encoding=ae.SoundEncoding.SIGNED_PCM,
        bytes_per_sample=2,
        num_channels=1,
    )


@suite.teardown
def teardown():
    """Clean up SoundData instances"""
    _sound_data.clear()


# ------------------------------------------------------------------
# Test: create
# ------------------------------------------------------------------

@suite.test
def test_create_sound_data():
    """SoundData.create() returns valid instances with various params"""
    # Float stereo
    sd_float = ae.SoundData.create(
        sample_rate=48000.0,
        encoding=ae.SoundEncoding.FLOAT,
        bytes_per_sample=4,
        num_channels=2,
    )
    assert_true(sd_float.valid, "Float stereo SoundData should be valid")

    # Signed PCM mono
    sd_pcm = ae.SoundData.create(
        sample_rate=44100.0,
        encoding=ae.SoundEncoding.SIGNED_PCM,
        bytes_per_sample=2,
        num_channels=1,
    )
    assert_true(sd_pcm.valid, "Signed PCM mono SoundData should be valid")

    # Unsigned PCM stereo, 1 byte per sample
    sd_upcm = ae.SoundData.create(
        sample_rate=22050.0,
        encoding=ae.SoundEncoding.UNSIGNED_PCM,
        bytes_per_sample=1,
        num_channels=2,
    )
    assert_true(sd_upcm.valid, "Unsigned PCM stereo SoundData should be valid")


# ------------------------------------------------------------------
# Test: valid
# ------------------------------------------------------------------

@suite.test
def test_sound_data_valid():
    """valid property is True for created SoundData and False for default"""
    sd = _sound_data["stereo_float"]
    assert_true(sd.valid, "Created SoundData should be valid")

    # Default-constructed SoundData should be invalid
    sd_empty = ae.SoundData()
    assert_true(not sd_empty.valid, "Default SoundData should not be valid")


# ------------------------------------------------------------------
# Test: sample_rate
# ------------------------------------------------------------------

@suite.test
def test_sample_rate():
    """sample_rate property returns the configured sample rate"""
    sd_48k = _sound_data["stereo_float"]
    assert_close(48000.0, sd_48k.sample_rate, tolerance=0.1,
                 message="Stereo float sample rate should be 48000 Hz")

    sd_44k = _sound_data["mono_pcm"]
    assert_close(44100.0, sd_44k.sample_rate, tolerance=0.1,
                 message="Mono PCM sample rate should be 44100 Hz")


# ------------------------------------------------------------------
# Test: encoding
# ------------------------------------------------------------------

@suite.test
def test_encoding():
    """encoding property returns the configured SoundEncoding"""
    sd_float = _sound_data["stereo_float"]
    assert_equal(ae.SoundEncoding.FLOAT, sd_float.encoding,
                 "Stereo float encoding should be FLOAT")

    sd_pcm = _sound_data["mono_pcm"]
    assert_equal(ae.SoundEncoding.SIGNED_PCM, sd_pcm.encoding,
                 "Mono PCM encoding should be SIGNED_PCM")


# ------------------------------------------------------------------
# Test: bytes_per_sample
# ------------------------------------------------------------------

@suite.test
def test_bytes_per_sample():
    """bytes_per_sample property returns expected values"""
    # FLOAT encoding always returns 4 regardless of the argument
    sd_float = _sound_data["stereo_float"]
    assert_equal(4, sd_float.bytes_per_sample,
                 "FLOAT encoding bytes_per_sample should be 4")

    sd_pcm = _sound_data["mono_pcm"]
    assert_equal(2, sd_pcm.bytes_per_sample,
                 "SIGNED_PCM 16-bit bytes_per_sample should be 2")


# ------------------------------------------------------------------
# Test: num_channels (mono)
# ------------------------------------------------------------------

@suite.test
def test_num_channels_mono():
    """num_channels returns 1 for mono SoundData"""
    sd_mono = _sound_data["mono_pcm"]
    assert_equal(1, sd_mono.num_channels,
                 "Mono SoundData should have 1 channel")


# ------------------------------------------------------------------
# Test: num_channels (stereo)
# ------------------------------------------------------------------

@suite.test
def test_num_channels_stereo():
    """num_channels returns 2 for stereo SoundData"""
    sd_stereo = _sound_data["stereo_float"]
    assert_equal(2, sd_stereo.num_channels,
                 "Stereo SoundData should have 2 channels")


# ------------------------------------------------------------------
# Test: num_samples
# ------------------------------------------------------------------

@suite.test
def test_num_samples():
    """num_samples property returns a non-negative integer"""
    sd = _sound_data["stereo_float"]
    num = sd.num_samples
    assert_isinstance(num, int, "num_samples should be an int")
    assert_true(num >= 0, f"num_samples should be non-negative, got {num}")


# ------------------------------------------------------------------
# Test: duration
# ------------------------------------------------------------------

@suite.test
def test_duration():
    """duration property returns a non-negative float"""
    sd = _sound_data["stereo_float"]
    dur = sd.duration
    assert_isinstance(dur, float, "duration should be a float")
    assert_true(dur >= 0.0, f"duration should be non-negative, got {dur}")


# ------------------------------------------------------------------
# Test: format dict
# ------------------------------------------------------------------

@suite.test
def test_format_dict():
    """format property returns a dict with expected keys"""
    sd = _sound_data["stereo_float"]
    fmt = sd.format
    assert_isinstance(fmt, dict, "format should return a dict")

    # Check required keys
    assert_true("sample_rate" in fmt, "format should contain 'sample_rate'")
    assert_true("encoding" in fmt, "format should contain 'encoding'")
    assert_true("bytes_per_sample" in fmt, "format should contain 'bytes_per_sample'")
    assert_true("num_channels" in fmt, "format should contain 'num_channels'")

    # Verify values match individual properties
    assert_close(sd.sample_rate, fmt["sample_rate"], tolerance=0.1,
                 message="format['sample_rate'] should match sample_rate property")
    assert_equal(sd.bytes_per_sample, fmt["bytes_per_sample"],
                 "format['bytes_per_sample'] should match bytes_per_sample property")
    assert_equal(sd.num_channels, fmt["num_channels"],
                 "format['num_channels'] should match num_channels property")


# ------------------------------------------------------------------
# Test: get_samples / set_samples round trip
# ------------------------------------------------------------------

@suite.test
def test_get_set_samples():
    """get_samples/set_samples round-trip preserves data"""
    sd = _sound_data["stereo_float"]
    num = sd.num_samples
    if num == 0:
        # Newly created SoundData may have 0 samples; nothing to round-trip
        assert_equal(b"", sd.get_samples(),
                     "get_samples on 0-sample SoundData should return empty bytes")
        return

    # Read original samples
    original = sd.get_samples()
    assert_isinstance(original, bytes, "get_samples should return bytes")

    expected_size = num * sd.num_channels * sd.bytes_per_sample
    assert_equal(expected_size, len(original),
                 f"get_samples byte length should be {expected_size}")

    # Write back the same data
    sd.set_samples(original)

    # Read again and verify round-trip
    after = sd.get_samples()
    assert_equal(original, after,
                 "set_samples then get_samples should return identical data")


# ------------------------------------------------------------------
# Test: context manager (lock / unlock)
# ------------------------------------------------------------------

@suite.test
def test_context_manager():
    """with-statement locks and unlocks SoundData without error"""
    sd = _sound_data["stereo_float"]
    # Enter and exit the context manager; no exception means success
    with sd:
        pass

    # After exiting, the SoundData should still be usable
    assert_true(sd.valid, "SoundData should remain valid after context manager exit")
    # Properties should still be accessible
    _ = sd.sample_rate


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
