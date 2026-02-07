"""
TextDocument Suite Tests
Tests for AEGP_TextDocumentSuite1

TextDocument Suiteはテキストレイヤーのテキスト内容を取得・設定する機能を提供します。
テキストのスタイリング情報（フォント、サイズ、色など）はStreamSuite経由でアクセスします。
"""
import ae

try:
    from test_utils import TestSuite, assert_equal, assert_true
except ImportError:
    from .test_utils import TestSuite, assert_equal, assert_true

suite = TestSuite("TextDocument Suite")

# Test variables
_test_comp = None
_test_text_layer = None


@suite.setup
def setup():
    """Setup test composition with text layer"""
    global _test_comp, _test_text_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_TextDocTestComp", 1920, 1080, 1.0, 5.0, 30.0)

    # Create a text layer
    _test_text_layer = _test_comp.add_text("Test Text")


@suite.teardown
def teardown():
    """Cleanup test composition"""
    global _test_comp, _test_text_layer
    if _test_comp is not None:
        _test_comp.delete()
        _test_comp = None
        _test_text_layer = None


# ============================================================================
# TextDocument Suite Tests
# ============================================================================

@suite.test
def test_get_new_text():
    """Test AEGP_GetNewText - gets text content from text layer"""
    global _test_text_layer
    if not _test_text_layer:
        print("No test text layer - skipping")
        return

    # Get the Source Text stream (AEGP_LayerStream_SOURCE_TEXT)
    plugin_id = ae.sdk.AEGP_GetPluginID()
    streamH = ae.sdk.AEGP_GetNewLayerStream(
        plugin_id,
        _test_text_layer._handle,
        ae.sdk.AEGP_LayerStream_SOURCE_TEXT
    )

    try:
        # Get the stream value at time 0
        value_type, value_handle = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,  # time in seconds
            False  # pre_expression
        )

        # value_type should be AEGP_StreamType_TEXT_DOCUMENT
        assert_equal(value_type, ae.sdk.AEGP_StreamType_TEXT_DOCUMENT,
                    "Stream value type is TEXT_DOCUMENT")

        # Extract AEGP_TextDocumentH from AEGP_StreamValue2
        text_documentH = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle)

        # Get the text content
        text = ae.sdk.AEGP_GetNewText(text_documentH)
        print(f"Text content: '{text}'")

        # Should match the text we created with
        assert_equal(text, "Test Text", "Text content matches")

        # Clean up
        ae.sdk.AEGP_DisposeStreamValue(value_handle)

    finally:
        ae.sdk.AEGP_DisposeStream(streamH)


@suite.test
def test_set_text():
    """Test AEGP_SetText - sets text content to text layer"""
    global _test_text_layer
    if not _test_text_layer:
        print("No test text layer - skipping")
        return

    new_text = "Modified Text Content"

    # Get the Source Text stream
    plugin_id = ae.sdk.AEGP_GetPluginID()
    streamH = ae.sdk.AEGP_GetNewLayerStream(
        plugin_id,
        _test_text_layer._handle,
        ae.sdk.AEGP_LayerStream_SOURCE_TEXT
    )

    try:
        # Get the current stream value
        value_type, value_handle = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,  # time in seconds
            False  # pre_expression
        )

        text_documentH = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle)

        # Set new text content
        ae.sdk.AEGP_SetText(text_documentH, new_text)
        print(f"Set text to: '{new_text}'")

        # Update the stream value with modified text_documentH
        ae.sdk.AEGP_SetStreamValue_TextDocument(value_handle, text_documentH)

        # Set the modified value back to the stream
        ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, value_handle)

        # Clean up the old handle
        ae.sdk.AEGP_DisposeStreamValue(value_handle)

        # Verify by reading it back
        value_type2, value_handle2 = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,  # time in seconds
            False  # pre_expression
        )

        text_documentH2 = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle2)
        retrieved_text = ae.sdk.AEGP_GetNewText(text_documentH2)
        print(f"Retrieved text: '{retrieved_text}'")

        assert_equal(retrieved_text, new_text, "Text content was modified")

        # Clean up
        ae.sdk.AEGP_DisposeStreamValue(value_handle2)

    finally:
        ae.sdk.AEGP_DisposeStream(streamH)


@suite.test
def test_text_with_unicode():
    """Test text with Unicode characters"""
    global _test_text_layer
    if not _test_text_layer:
        print("No test text layer - skipping")
        return

    # Unicode text with Japanese characters
    unicode_text = "日本語テキスト 🎨"

    plugin_id = ae.sdk.AEGP_GetPluginID()
    streamH = ae.sdk.AEGP_GetNewLayerStream(
        plugin_id,
        _test_text_layer._handle,
        ae.sdk.AEGP_LayerStream_SOURCE_TEXT
    )

    try:
        value_type, value_handle = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,  # time in seconds
            False  # pre_expression
        )

        text_documentH = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle)

        # Set Unicode text
        ae.sdk.AEGP_SetText(text_documentH, unicode_text)
        print(f"Set Unicode text: '{unicode_text}'")

        # Update the stream value with modified text_documentH
        ae.sdk.AEGP_SetStreamValue_TextDocument(value_handle, text_documentH)

        # Set back to stream
        ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, value_handle)
        ae.sdk.AEGP_DisposeStreamValue(value_handle)

        # Read back
        value_type2, value_handle2 = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,
            False
        )

        text_documentH2 = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle2)
        retrieved_text = ae.sdk.AEGP_GetNewText(text_documentH2)
        print(f"Retrieved Unicode text: '{retrieved_text}'")

        assert_equal(retrieved_text, unicode_text, "Unicode text preserved")

        ae.sdk.AEGP_DisposeStreamValue(value_handle2)

    finally:
        ae.sdk.AEGP_DisposeStream(streamH)


@suite.test
def test_empty_text():
    """Test setting and getting empty text"""
    global _test_text_layer
    if not _test_text_layer:
        print("No test text layer - skipping")
        return

    empty_text = ""

    plugin_id = ae.sdk.AEGP_GetPluginID()
    streamH = ae.sdk.AEGP_GetNewLayerStream(
        plugin_id,
        _test_text_layer._handle,
        ae.sdk.AEGP_LayerStream_SOURCE_TEXT
    )

    try:
        value_type, value_handle = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,  # time in seconds
            False  # pre_expression
        )

        text_documentH = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle)

        # Set empty text
        ae.sdk.AEGP_SetText(text_documentH, empty_text)
        print("Set empty text")

        # Update the stream value with modified text_documentH
        ae.sdk.AEGP_SetStreamValue_TextDocument(value_handle, text_documentH)

        # Set back to stream
        ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, value_handle)
        ae.sdk.AEGP_DisposeStreamValue(value_handle)

        # Read back
        value_type2, value_handle2 = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,
            False
        )

        text_documentH2 = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle2)
        retrieved_text = ae.sdk.AEGP_GetNewText(text_documentH2)
        print(f"Retrieved text length: {len(retrieved_text)}")

        assert_equal(retrieved_text, empty_text, "Empty text preserved")

        ae.sdk.AEGP_DisposeStreamValue(value_handle2)

    finally:
        ae.sdk.AEGP_DisposeStream(streamH)


@suite.test
def test_multiline_text():
    """Test text with multiple lines"""
    global _test_text_layer
    if not _test_text_layer:
        print("No test text layer - skipping")
        return

    multiline_text = "Line 1\nLine 2\nLine 3"

    plugin_id = ae.sdk.AEGP_GetPluginID()
    streamH = ae.sdk.AEGP_GetNewLayerStream(
        plugin_id,
        _test_text_layer._handle,
        ae.sdk.AEGP_LayerStream_SOURCE_TEXT
    )

    try:
        value_type, value_handle = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,  # time in seconds
            False  # pre_expression
        )

        text_documentH = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle)

        # Set multiline text
        ae.sdk.AEGP_SetText(text_documentH, multiline_text)
        print(f"Set multiline text: {repr(multiline_text)}")

        # Update the stream value with modified text_documentH
        ae.sdk.AEGP_SetStreamValue_TextDocument(value_handle, text_documentH)

        # Set back to stream
        ae.sdk.AEGP_SetStreamValue(plugin_id, streamH, value_handle)
        ae.sdk.AEGP_DisposeStreamValue(value_handle)

        # Read back
        value_type2, value_handle2 = ae.sdk.AEGP_GetNewStreamValue(
            plugin_id,
            streamH,
            ae.sdk.AEGP_LTimeMode_LayerTime,
            0.0,
            False
        )

        text_documentH2 = ae.sdk.AEGP_GetStreamValue_TextDocument(value_handle2)
        retrieved_text = ae.sdk.AEGP_GetNewText(text_documentH2)
        print(f"Retrieved multiline text: {repr(retrieved_text)}")

        # Note: Line endings might be normalized
        # Check that lines are preserved
        assert_true("Line 1" in retrieved_text, "Line 1 preserved")
        assert_true("Line 2" in retrieved_text, "Line 2 preserved")
        assert_true("Line 3" in retrieved_text, "Line 3 preserved")

        ae.sdk.AEGP_DisposeStreamValue(value_handle2)

    finally:
        ae.sdk.AEGP_DisposeStream(streamH)


# Run tests
def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
