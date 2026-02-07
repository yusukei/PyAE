# test_layer_quality_label.py
# PyAE Layer Quality and Label テスト

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
    )

suite = TestSuite("Layer Quality/Label")

# Test variables
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    """Setup before tests"""
    global _test_comp, _test_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_QualityLabelTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    _test_layer = _test_comp.add_solid("_TestLayer", 100, 100)


@suite.teardown
def teardown():
    """Cleanup after tests"""
    global _test_comp
    if _test_comp:
        _test_comp.delete()


@suite.test
def test_layer_quality():
    """Test layer quality property (WIREFRAME/DRAFT/BEST)"""
    quality = _test_layer.quality
    assert_true(isinstance(quality, ae.LayerQuality), "Quality should be a LayerQuality enum")

    # Set quality to WIREFRAME
    _test_layer.quality = ae.LayerQuality.Wireframe
    assert_equal(_test_layer.quality, ae.LayerQuality.Wireframe, "Quality should be WIREFRAME")

    # Set quality to DRAFT
    _test_layer.quality = ae.LayerQuality.Draft
    assert_equal(_test_layer.quality, ae.LayerQuality.Draft, "Quality should be DRAFT")

    # Set quality to BEST
    _test_layer.quality = ae.LayerQuality.Best
    assert_equal(_test_layer.quality, ae.LayerQuality.Best, "Quality should be BEST")


@suite.test
def test_layer_sampling_quality():
    """Test layer sampling quality property (BILINEAR/BICUBIC)"""
    sampling_quality = _test_layer.sampling_quality
    assert_true(isinstance(sampling_quality, ae.SamplingQuality), "Sampling quality should be a SamplingQuality enum")

    # Set sampling quality to BILINEAR
    _test_layer.sampling_quality = ae.SamplingQuality.Bilinear
    assert_equal(_test_layer.sampling_quality, ae.SamplingQuality.Bilinear, "Sampling quality should be BILINEAR")

    # Set sampling quality to BICUBIC
    _test_layer.sampling_quality = ae.SamplingQuality.Bicubic
    assert_equal(_test_layer.sampling_quality, ae.SamplingQuality.Bicubic, "Sampling quality should be BICUBIC")


@suite.test
def test_layer_label():
    """Test layer label property (color label ID)"""
    label = _test_layer.label
    assert_true(label >= -1 and label <= 16, "Label should be between -1 and 16")

    # Set label to NO_LABEL (0)
    _test_layer.label = 0
    assert_equal(_test_layer.label, 0, "Label should be 0 (NO_LABEL)")

    # Set label to color 1
    _test_layer.label = 1
    assert_equal(_test_layer.label, 1, "Label should be 1")

    # Set label to color 5
    _test_layer.label = 5
    assert_equal(_test_layer.label, 5, "Label should be 5")

    # Set label to color 16
    _test_layer.label = 16
    assert_equal(_test_layer.label, 16, "Label should be 16")


@suite.test
def test_layer_quality_and_sampling_interaction():
    """Test that sampling quality works correctly with layer quality"""
    # Set to BEST quality
    _test_layer.quality = 2  # BEST

    # Set sampling quality to BICUBIC
    _test_layer.sampling_quality = 1  # BICUBIC

    # Verify both settings - Note: getters return enum types
    assert_equal(int(_test_layer.quality), 2, "Quality should be 2 (BEST)")
    assert_equal(int(_test_layer.sampling_quality), 1, "Sampling quality should be 1 (BICUBIC)")

    # Change quality to DRAFT - sampling quality should persist
    _test_layer.quality = 1  # DRAFT
    assert_equal(int(_test_layer.quality), 1, "Quality should be 1 (DRAFT)")
    assert_equal(int(_test_layer.sampling_quality), 1, "Sampling quality should persist as 1 (BICUBIC)")


def run():
    """Run all tests in this module."""
    return suite.run()


if __name__ == "__main__":
    run()
