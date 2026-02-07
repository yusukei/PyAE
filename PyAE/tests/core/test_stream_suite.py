# test_stream_suite.py
# PyAE StreamSuite & KeyframeSuite Low-level API テスト

import ae
import ae.sdk as sdk

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
        assert_close,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
        assert_close,
    )

suite = TestSuite("StreamSuite Low-level API")

# テスト用変数
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp, _test_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_StreamTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    _test_layer = _test_comp.add_solid("_TestLayer", 100, 100, (1.0, 0.0, 0.0), 10.0)


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    if _test_comp:
        _test_comp.delete()


@suite.test
def test_stream_basic_functions():
    """StreamSuiteの基本的な関数をテスト"""
    global _test_layer

    # Opacityプロパティを取得
    opacity_prop = _test_layer.property("Opacity")
    assert_not_none(opacity_prop, "Should get Opacity property")

    # ストリームハンドル取得
    streamH = opacity_prop._handle
    assert_not_none(streamH, "Should get stream handle")

    # ストリーム名取得
    name = sdk.AEGP_GetStreamName(streamH, False)
    assert_isinstance(name, str)
    assert_true(len(name) > 0, "Stream name should not be empty")

    # ストリームタイプ取得
    stream_type = sdk.AEGP_GetStreamType(streamH)
    assert_isinstance(stream_type, int)
    # OpacityはOneD (NO_DATAでないことを確認)
    assert_true(stream_type != sdk.AEGP_StreamType_NO_DATA)

    # ストリームプロパティ取得
    props = sdk.AEGP_GetStreamProperties(streamH)
    assert_isinstance(props, dict)
    assert_true("flags" in props)
    assert_true("min" in props)
    assert_true("max" in props)


@suite.test
def test_stream_value_double():
    """1D値（Double）のストリーム値操作をテスト"""
    global _test_layer
    plugin_id = sdk.AEGP_GetPluginID()

    # Opacityプロパティ
    opacity_prop = _test_layer.property("Opacity")
    streamH = opacity_prop._handle

    # 値を取得
    stream_type, valueP = sdk.AEGP_GetNewStreamValue(
        plugin_id, streamH, sdk.AEGP_LTimeMode_CompTime, 0.0, False
    )
    assert_not_none(valueP, "Should get stream value")
    assert_isinstance(valueP, int)  # uintptr_t

    # Double値を取得
    val = sdk.AEGP_GetStreamValue_Double(valueP)
    assert_isinstance(val, float)
    assert_close(100.0, val)  # デフォルトのOpacityは100

    # Double値を設定
    sdk.AEGP_SetStreamValue_Double(valueP, 50.0)
    val2 = sdk.AEGP_GetStreamValue_Double(valueP)
    assert_close(50.0, val2)

    # 値を破棄
    sdk.AEGP_DisposeStreamValue(valueP)


@suite.test
def test_stream_value_point():
    """2D値（Point）のストリーム値操作をテスト"""
    global _test_layer
    plugin_id = sdk.AEGP_GetPluginID()

    # Positionプロパティ
    pos_prop = _test_layer.property("Position")
    streamH = pos_prop._handle

    # 値を取得
    stream_type, valueP = sdk.AEGP_GetNewStreamValue(
        plugin_id, streamH, sdk.AEGP_LTimeMode_CompTime, 0.0, False
    )
    assert_not_none(valueP)

    # Point値を取得
    point = sdk.AEGP_GetStreamValue_Point(valueP)
    assert_isinstance(point, tuple)
    assert_equal(2, len(point))

    # Point値を設定
    sdk.AEGP_SetStreamValue_Point(valueP, 200.0, 300.0)
    point2 = sdk.AEGP_GetStreamValue_Point(valueP)
    assert_close(200.0, point2[0])
    assert_close(300.0, point2[1])

    # 値を破棄
    sdk.AEGP_DisposeStreamValue(valueP)


@suite.test
def test_stream_value_color():
    """Color値のストリーム値操作をテスト"""
    global _test_comp
    plugin_id = sdk.AEGP_GetPluginID()

    # Solid Settingsエフェクトを追加してColorプロパティを取得
    # または背景色を使う
    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)

    # 背景色を取得・設定してテスト
    bg_color = sdk.AEGP_GetCompBGColor(compH)
    assert_isinstance(bg_color, tuple)
    assert_equal(3, len(bg_color))

    # 背景色を設定
    sdk.AEGP_SetCompBGColor(compH, 0.5, 0.6, 0.7)
    bg_color2 = sdk.AEGP_GetCompBGColor(compH)
    # 8-bit color quantization tolerance: 1/255 ≈ 0.00392
    assert_close(0.5, bg_color2[0], tolerance=0.005)
    assert_close(0.6, bg_color2[1], tolerance=0.005)
    assert_close(0.7, bg_color2[2], tolerance=0.005)

    # 元に戻す
    sdk.AEGP_SetCompBGColor(compH, bg_color[0], bg_color[1], bg_color[2])


@suite.test
def test_stream_value_threed():
    """3D値のストリーム値操作をテスト"""
    global _test_comp
    plugin_id = sdk.AEGP_GetPluginID()

    # 3Dレイヤーを作成
    layer_3d = _test_comp.add_null("_Test3D", 10.0)
    layer_3d.three_d = True

    # Positionプロパティ（3Dになっているはず）
    pos_prop = layer_3d.property("Position")
    streamH = pos_prop._handle

    # 値を取得
    stream_type, valueP = sdk.AEGP_GetNewStreamValue(
        plugin_id, streamH, sdk.AEGP_LTimeMode_CompTime, 0.0, False
    )
    assert_not_none(valueP)

    # ThreeD値を取得
    point = sdk.AEGP_GetStreamValue_ThreeD(valueP)
    assert_isinstance(point, tuple)
    assert_equal(3, len(point))

    # ThreeD値を設定
    sdk.AEGP_SetStreamValue_ThreeD(valueP, 100.0, 200.0, 300.0)
    point2 = sdk.AEGP_GetStreamValue_ThreeD(valueP)
    assert_close(100.0, point2[0])
    assert_close(200.0, point2[1])
    assert_close(300.0, point2[2])

    # 値を破棄
    sdk.AEGP_DisposeStreamValue(valueP)


@suite.test
def test_keyframe_suite():
    """KeyframeSuiteの基本的な関数をテスト"""
    global _test_layer

    # Opacityプロパティ
    opacity_prop = _test_layer.property("Opacity")
    streamH = opacity_prop._handle

    # 最初はキーフレームなし
    num_kfs = sdk.AEGP_GetStreamNumKfs(streamH)
    assert_equal(0, num_kfs)

    # キーフレーム追加
    key_idx1 = sdk.AEGP_InsertKeyframe(streamH, sdk.AEGP_LTimeMode_CompTime, 0.0)
    assert_equal(0, key_idx1)

    key_idx2 = sdk.AEGP_InsertKeyframe(streamH, sdk.AEGP_LTimeMode_CompTime, 1.0)
    assert_equal(1, key_idx2)

    # キーフレーム数確認
    num_kfs = sdk.AEGP_GetStreamNumKfs(streamH)
    assert_equal(2, num_kfs)

    # キーフレーム時刻取得
    time1 = sdk.AEGP_GetKeyframeTime(streamH, 0, sdk.AEGP_LTimeMode_CompTime)
    time2 = sdk.AEGP_GetKeyframeTime(streamH, 1, sdk.AEGP_LTimeMode_CompTime)
    assert_close(0.0, time1)
    assert_close(1.0, time2)

    # キーフレーム値取得・設定
    plugin_id = sdk.AEGP_GetPluginID()
    valueP = sdk.AEGP_GetNewKeyframeValue(plugin_id, streamH, 0)
    assert_not_none(valueP)

    val = sdk.AEGP_GetStreamValue_Double(valueP)
    assert_isinstance(val, float)

    # 値を変更
    sdk.AEGP_SetStreamValue_Double(valueP, 75.0)
    sdk.AEGP_SetKeyframeValue(streamH, 0, valueP)

    # 値を破棄
    sdk.AEGP_DisposeStreamValue(valueP)

    # 確認（再取得）
    valueP2 = sdk.AEGP_GetNewKeyframeValue(plugin_id, streamH, 0)
    val2 = sdk.AEGP_GetStreamValue_Double(valueP2)
    assert_close(75.0, val2)
    sdk.AEGP_DisposeStreamValue(valueP2)

    # キーフレーム削除
    sdk.AEGP_DeleteKeyframe(streamH, 1)
    num_kfs = sdk.AEGP_GetStreamNumKfs(streamH)
    assert_equal(1, num_kfs)

    sdk.AEGP_DeleteKeyframe(streamH, 0)
    num_kfs = sdk.AEGP_GetStreamNumKfs(streamH)
    assert_equal(0, num_kfs)


@suite.test
def test_keyframe_flags():
    """キーフレームフラグ操作をテスト"""
    global _test_layer

    # Positionプロパティ
    pos_prop = _test_layer.property("Position")
    streamH = pos_prop._handle

    # キーフレーム追加
    key_idx = sdk.AEGP_InsertKeyframe(streamH, sdk.AEGP_LTimeMode_CompTime, 0.5)

    # フラグ取得
    flags = sdk.AEGP_GetKeyframeFlags(streamH, key_idx)
    assert_isinstance(flags, int)

    # フラグ設定（TEMPORAL_CONTINUOUS）
    sdk.AEGP_SetKeyframeFlag(
        streamH, key_idx, sdk.AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS, True
    )

    # 確認
    flags2 = sdk.AEGP_GetKeyframeFlags(streamH, key_idx)
    assert_true(flags2 & sdk.AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS)

    # クリーンアップ
    sdk.AEGP_DeleteKeyframe(streamH, key_idx)


@suite.test
def test_keyframe_temporal_ease():
    """キーフレームのテンポラルイーズを取得・設定できる"""
    global _test_layer
    plugin_id = sdk.AEGP_GetPluginID()

    # Opacityプロパティ
    opacity_prop = _test_layer.property("Opacity")
    streamH = opacity_prop._handle

    # キーフレームを2つ追加
    key_idx1 = sdk.AEGP_InsertKeyframe(streamH, sdk.AEGP_LTimeMode_CompTime, 0.0)
    key_idx2 = sdk.AEGP_InsertKeyframe(streamH, sdk.AEGP_LTimeMode_CompTime, 1.0)

    # テンポラルイーズを取得（dimension 0 = 1D property）
    ease = sdk.AEGP_GetKeyframeTemporalEase(streamH, key_idx1, 0)
    assert_isinstance(ease, tuple)
    assert_equal(len(ease), 2, "Should return (in_ease, out_ease)")
    in_ease, out_ease = ease
    assert_isinstance(in_ease, tuple)
    assert_isinstance(out_ease, tuple)
    assert_equal(len(in_ease), 2, "in_ease should have (speed, influence)")
    assert_equal(len(out_ease), 2, "out_ease should have (speed, influence)")
    print(f"Initial ease: in={in_ease}, out={out_ease}")

    # テンポラルイーズを設定
    # Note: Adobe SDK uses 0.0-1.0 range for influenceF (not 0-100)
    # SDK sample (Easy_Cheese.cpp) uses influenceF = 0.33333333 (33.33%)
    sdk.AEGP_SetKeyframeTemporalEase(
        streamH, key_idx1, 0,
        in_speed=0.0, in_influence=0.5,  # 0.5 = 50%
        out_speed=0.0, out_influence=0.5
    )

    # CRITICAL: Temporal ease requires Bezier interpolation
    # SDK sample (Easy_Cheese.cpp) calls SetKeyframeInterpolation after SetKeyframeTemporalEase
    sdk.AEGP_SetKeyframeInterpolation(
        streamH, key_idx1,
        sdk.AEGP_KeyInterp_BEZIER,  # in_interp
        sdk.AEGP_KeyInterp_BEZIER   # out_interp
    )

    # 設定が反映されていることを確認
    ease2 = sdk.AEGP_GetKeyframeTemporalEase(streamH, key_idx1, 0)
    in_ease2, out_ease2 = ease2
    print(f"After set ease: in={in_ease2}, out={out_ease2}")
    # SDK internal representation: 0.0-1.0 range
    assert_close(0.5, in_ease2[1], tolerance=0.01)
    assert_close(0.5, out_ease2[1], tolerance=0.01)

    # クリーンアップ
    sdk.AEGP_DeleteKeyframe(streamH, 1)
    sdk.AEGP_DeleteKeyframe(streamH, 0)


@suite.test
def test_keyframe_spatial_tangents():
    """キーフレームのスペーシャルタンジェントを取得・設定できる"""
    global _test_layer
    plugin_id = sdk.AEGP_GetPluginID()

    # Positionプロパティ（2D/3D）
    pos_prop = _test_layer.property("Position")
    streamH = pos_prop._handle

    # キーフレームを追加
    key_idx = sdk.AEGP_InsertKeyframe(streamH, sdk.AEGP_LTimeMode_CompTime, 0.5)

    # スペーシャルタンジェントを取得
    in_tan_ptr, out_tan_ptr = sdk.AEGP_GetNewKeyframeSpatialTangents(plugin_id, streamH, key_idx)
    assert_true(in_tan_ptr != 0, "in_tan should be valid")
    assert_true(out_tan_ptr != 0, "out_tan should be valid")

    # タンジェント値を設定
    sdk.AEGP_SetKeyframeSpatialTangents(streamH, key_idx, in_tan_ptr, out_tan_ptr)

    # クリーンアップ
    sdk.AEGP_DisposeStreamValue(in_tan_ptr)
    sdk.AEGP_DisposeStreamValue(out_tan_ptr)
    sdk.AEGP_DeleteKeyframe(streamH, key_idx)


@suite.test
def test_keyframe_label_color():
    """キーフレームラベルカラーを取得・設定できる"""
    global _test_layer

    # Opacityプロパティ
    opacity_prop = _test_layer.property("Opacity")
    streamH = opacity_prop._handle

    # キーフレームを追加
    key_idx = sdk.AEGP_InsertKeyframe(streamH, sdk.AEGP_LTimeMode_CompTime, 0.5)

    # ラベルカラーを取得
    label = sdk.AEGP_GetKeyframeLabelColorIndex(streamH, key_idx)
    assert_isinstance(label, int)
    print(f"Initial keyframe label: {label}")

    # ラベルカラーを設定（1=Red）
    sdk.AEGP_SetKeyframeLabelColorIndex(streamH, key_idx, 1)
    label2 = sdk.AEGP_GetKeyframeLabelColorIndex(streamH, key_idx)
    assert_equal(label2, 1, "Label should be set to 1 (Red)")

    # クリーンアップ
    sdk.AEGP_DeleteKeyframe(streamH, key_idx)


@suite.test
def test_batch_keyframe_operations():
    """バッチキーフレーム操作（StartAddKeyframes/AddKeyframes/SetAddKeyframe/EndAddKeyframes）"""
    global _test_layer
    plugin_id = sdk.AEGP_GetPluginID()

    # Opacityプロパティ
    opacity_prop = _test_layer.property("Opacity")
    streamH = opacity_prop._handle

    # 既存のキーフレームを削除
    while sdk.AEGP_GetStreamNumKfs(streamH) > 0:
        sdk.AEGP_DeleteKeyframe(streamH, 0)

    # バッチ操作開始
    akH = sdk.AEGP_StartAddKeyframes(streamH)
    assert_true(akH != 0, "Should get valid add keyframes handle")

    # 複数のキーフレームを追加
    key_idx0 = sdk.AEGP_AddKeyframes(akH, sdk.AEGP_LTimeMode_CompTime, 0.0)
    key_idx1 = sdk.AEGP_AddKeyframes(akH, sdk.AEGP_LTimeMode_CompTime, 1.0)
    key_idx2 = sdk.AEGP_AddKeyframes(akH, sdk.AEGP_LTimeMode_CompTime, 2.0)

    # 各キーフレームに値を設定
    stream_type, valueP0 = sdk.AEGP_GetNewStreamValue(
        plugin_id, streamH, sdk.AEGP_LTimeMode_CompTime, 0.0, False
    )
    sdk.AEGP_SetStreamValue_Double(valueP0, 100.0)
    sdk.AEGP_SetAddKeyframe(akH, key_idx0, valueP0)
    sdk.AEGP_DisposeStreamValue(valueP0)

    stream_type, valueP1 = sdk.AEGP_GetNewStreamValue(
        plugin_id, streamH, sdk.AEGP_LTimeMode_CompTime, 0.0, False
    )
    sdk.AEGP_SetStreamValue_Double(valueP1, 50.0)
    sdk.AEGP_SetAddKeyframe(akH, key_idx1, valueP1)
    sdk.AEGP_DisposeStreamValue(valueP1)

    stream_type, valueP2 = sdk.AEGP_GetNewStreamValue(
        plugin_id, streamH, sdk.AEGP_LTimeMode_CompTime, 0.0, False
    )
    sdk.AEGP_SetStreamValue_Double(valueP2, 100.0)
    sdk.AEGP_SetAddKeyframe(akH, key_idx2, valueP2)
    sdk.AEGP_DisposeStreamValue(valueP2)

    # バッチ操作終了（add=Trueでキーフレームを追加）
    sdk.AEGP_EndAddKeyframes(True, akH)

    # キーフレーム数を確認
    num_kfs = sdk.AEGP_GetStreamNumKfs(streamH)
    assert_equal(num_kfs, 3, "Should have 3 keyframes")

    # クリーンアップ
    while sdk.AEGP_GetStreamNumKfs(streamH) > 0:
        sdk.AEGP_DeleteKeyframe(streamH, 0)


@suite.test
def test_dynamic_stream_suite_property_tree():
    """DynamicStreamSuite - Property Tree Traversal"""
    global _test_layer

    # Get root stream reference for layer
    root_stream = sdk.AEGP_GetNewStreamRefForLayer(_test_layer._handle)
    assert_not_none(root_stream)

    # Check grouping type (should be NAMED_GROUP or INDEXED_GROUP, not LEAF)
    group_type = sdk.AEGP_GetStreamGroupingType(root_stream)
    assert_true(group_type != sdk.AEGP_StreamGroupingType_LEAF)

    # Get number of child streams
    num_children = sdk.AEGP_GetNumStreamsInGroup(root_stream)
    assert_true(num_children > 0)

    # Get first child stream
    child_stream = sdk.AEGP_GetNewStreamRefByIndex(root_stream, 0)
    assert_not_none(child_stream)

    # Navigate back to parent
    parent_stream = sdk.AEGP_GetNewParentStreamRef(child_stream)
    assert_not_none(parent_stream)

    # Clean up
    sdk.AEGP_DisposeStream(child_stream)
    sdk.AEGP_DisposeStream(parent_stream)
    sdk.AEGP_DisposeStream(root_stream)


@suite.test
def test_get_layer_stream_value():
    """AEGP_GetLayerStreamValue - レイヤーストリーム値を直接取得できる"""
    global _test_layer
    plugin_id = sdk.AEGP_GetPluginID()

    layer_handle = _test_layer._handle

    # AEGP_LayerStream_OPACITY でテスト
    stream_type = sdk.AEGP_LayerStream_OPACITY

    # 時間0でストリーム値を取得
    # Note: AEGP_GetLayerStreamValue returns (stream_type, value) directly
    value_type, value = sdk.AEGP_GetLayerStreamValue(
        layer_handle, stream_type,
        sdk.AEGP_LTimeMode_CompTime, 0.0,
        False  # pre_expression
    )
    assert_not_none(value)
    assert_isinstance(value_type, int)
    print(f"Layer stream value type: {value_type}")

    # 値は直接返される（Opacityは1D double）
    assert_isinstance(value, float)
    print(f"Opacity value: {value}")


@suite.test
def test_stream_value_text_document():
    """TextDocument値のストリーム値操作をテスト"""
    global _test_comp
    plugin_id = sdk.AEGP_GetPluginID()

    # テキストレイヤーを作成
    text_layer = _test_comp.add_text("Test Text for StreamValue")

    # Source Textストリームを取得
    streamH = sdk.AEGP_GetNewLayerStream(
        plugin_id,
        text_layer._handle,
        sdk.AEGP_LayerStream_SOURCE_TEXT
    )
    assert_true(streamH != 0, "Should get valid stream handle")

    # ストリーム値を取得
    stream_type, valueP = sdk.AEGP_GetNewStreamValue(
        plugin_id, streamH, sdk.AEGP_LTimeMode_LayerTime, 0.0, False
    )
    assert_equal(stream_type, sdk.AEGP_StreamType_TEXT_DOCUMENT,
                "Stream type should be TEXT_DOCUMENT")

    # TextDocument値を取得
    text_docH = sdk.AEGP_GetStreamValue_TextDocument(valueP)
    assert_true(text_docH != 0, "Should get valid text document handle")

    # テキスト内容を取得
    text = sdk.AEGP_GetNewText(text_docH)
    assert_equal(text, "Test Text for StreamValue", "Text should match")

    # テキストを変更
    sdk.AEGP_SetText(text_docH, "Modified Text")

    # TextDocument値を設定
    sdk.AEGP_SetStreamValue_TextDocument(valueP, text_docH)

    # ストリーム値を設定
    sdk.AEGP_SetStreamValue(plugin_id, streamH, valueP)

    # クリーンアップ
    sdk.AEGP_DisposeStreamValue(valueP)
    sdk.AEGP_DisposeStream(streamH)
    text_layer.delete()


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
