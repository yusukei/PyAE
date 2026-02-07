# test_layer.py
# PyAE Layer テスト

import ae

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

suite = TestSuite("Layer")

# テスト用変数
_test_comp = None
_test_layer = None
_parent_layer = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp, _test_layer, _parent_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_LayerTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    _test_layer = _test_comp.add_solid("_TestLayer", 200, 200, (1.0, 0.0, 0.0), 5.0)
    _parent_layer = _test_comp.add_null("_ParentNull", 10.0)


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    if _test_comp is not None:
        _test_comp.delete()


# ===== 基本プロパティ =====


@suite.test
def test_layer_name():
    """レイヤー名が取得・設定できる"""
    global _test_layer
    assert_equal("_TestLayer", _test_layer.name)
    _test_layer.name = "_RenamedLayer"
    assert_equal("_RenamedLayer", _test_layer.name)
    _test_layer.name = "_TestLayer"  # 元に戻す


@suite.test
def test_layer_index():
    """レイヤーインデックスが取得できる"""
    global _test_layer
    index = _test_layer.index
    assert_true(index >= 0, "Layer index should be 0 or greater (0-based)")


@suite.test
def test_layer_is_valid():
    """レイヤーが有効か確認できる"""
    # レイヤーが有効か確認
    # Note: Using .valid as per updated bindings
    assert_true(_test_layer.valid, "Layer should be valid")


# ===== 時間プロパティ =====


@suite.test
def test_layer_in_point():
    """インポイントが取得・設定できる"""
    global _test_layer
    original = _test_layer.in_point
    _test_layer.in_point = 1.0
    assert_close(1.0, _test_layer.in_point)
    _test_layer.in_point = original  # 元に戻す


@suite.test
def test_layer_out_point():
    """アウトポイントが取得・設定できる"""
    global _test_layer
    original = _test_layer.out_point
    _test_layer.out_point = 4.0
    assert_close(4.0, _test_layer.out_point)
    _test_layer.out_point = original  # 元に戻す


@suite.test
def test_layer_start_time():
    """スタートタイムが取得・設定できる"""
    global _test_layer
    original = _test_layer.start_time
    _test_layer.start_time = 0.5
    assert_close(0.5, _test_layer.start_time)
    _test_layer.start_time = original  # 元に戻す


@suite.test
def test_layer_duration():
    """デュレーションが取得できる"""
    global _test_layer
    duration = _test_layer.duration
    assert_true(duration > 0, "Duration should be positive")


# ===== フラグ =====


@suite.test
def test_layer_enabled():
    """表示/非表示が取得・設定できる"""
    global _test_layer
    original = _test_layer.enabled
    _test_layer.enabled = False
    assert_false(_test_layer.enabled)
    _test_layer.enabled = True
    assert_true(_test_layer.enabled)
    _test_layer.enabled = original  # 元に戻す


@suite.test
def test_layer_solo():
    """ソロが取得・設定できる"""
    global _test_layer
    original = _test_layer.solo
    _test_layer.solo = True
    assert_true(_test_layer.solo)
    _test_layer.solo = False
    assert_false(_test_layer.solo)
    _test_layer.solo = original  # 元に戻す


@suite.test
def test_layer_locked():
    """ロックが取得・設定できる"""
    global _test_layer
    original = _test_layer.locked
    _test_layer.locked = True
    assert_true(_test_layer.locked)
    _test_layer.locked = False
    assert_false(_test_layer.locked)
    _test_layer.locked = original  # 元に戻す


@suite.test
def test_layer_shy():
    """シャイが取得・設定できる"""
    global _test_layer
    original = _test_layer.shy
    _test_layer.shy = True
    assert_true(_test_layer.shy)
    _test_layer.shy = False
    assert_false(_test_layer.shy)
    _test_layer.shy = original  # 元に戻す


# ===== トランスフォーム =====


@suite.test
def test_layer_position():
    """位置が取得・設定できる"""
    global _test_layer
    pos = _test_layer.position
    assert_isinstance(pos, tuple)
    assert_true(len(pos) >= 2, "Position should have at least 2 components")
    # 新しい位置を設定
    _test_layer.position = (100.0, 200.0, 0.0)
    new_pos = _test_layer.position
    assert_close(100.0, new_pos[0])
    assert_close(200.0, new_pos[1])


@suite.test
def test_layer_scale():
    """スケールが取得・設定できる"""
    global _test_layer
    scale = _test_layer.scale
    assert_isinstance(scale, tuple)
    # 新しいスケールを設定
    _test_layer.scale = (50.0, 50.0, 100.0)
    new_scale = _test_layer.scale
    assert_close(50.0, new_scale[0])
    assert_close(50.0, new_scale[1])
    # 元に戻す
    _test_layer.scale = (100.0, 100.0, 100.0)


@suite.test
def test_layer_rotation():
    """回転が取得・設定できる"""
    global _test_layer
    rot = _test_layer.rotation
    assert_true(isinstance(rot, (int, float)))
    _test_layer.rotation = 45.0
    assert_close(45.0, _test_layer.rotation)
    _test_layer.rotation = 0.0  # 元に戻す


@suite.test
def test_layer_anchor_point():
    """アンカーポイントが取得・設定できる"""
    global _test_layer
    anchor = _test_layer.anchor_point
    assert_isinstance(anchor, tuple)


@suite.test
def test_layer_opacity():
    """不透明度が取得・設定できる"""
    global _test_layer
    opacity = _test_layer.opacity
    assert_true(0 <= opacity <= 100, "Opacity should be 0-100")
    _test_layer.opacity = 50.0
    assert_close(50.0, _test_layer.opacity)
    _test_layer.opacity = 100.0  # 元に戻す


# ===== 親子関係 =====


@suite.test
def test_layer_parent():
    """親レイヤーが取得・設定できる"""
    global _test_layer, _parent_layer
    # 親なし確認
    original_parent = _test_layer.parent
    # 親を設定
    _test_layer.parent = _parent_layer
    assert_not_none(_test_layer.parent, "Parent should be set")
    # 親を解除
    _test_layer.parent = None
    assert_none(_test_layer.parent, "Parent should be None after clearing")


@suite.test
def test_layer_type():
    """レイヤータイプが取得できる"""
    global _test_layer
    layer_type = _test_layer.layer_type
    assert_not_none(layer_type)


@suite.test
def test_layer_is_null():
    """ヌルレイヤー判定ができる"""
    global _parent_layer
    is_null = _parent_layer.is_null
    assert_true(is_null, "Null layer should be detected as null")


# ===== 操作 =====


@suite.test
def test_layer_duplicate():
    """レイヤーを複製できる"""
    global _test_layer
    dup = _test_layer.duplicate()
    assert_not_none(dup, "Duplicated layer should not be None")
    # クリーンアップ
    dup.delete()


@suite.test
def test_layer_move_to():
    """レイヤーを移動できる（0-based indexing）"""
    global _test_comp, _test_layer
    original_index = _test_layer.index
    num_layers = _test_comp.num_layers
    if num_layers >= 2:
        # 最後に移動 (0-based: last index = num_layers - 1)
        last_index = num_layers - 1
        _test_layer.move_to(last_index)
        assert_equal(last_index, _test_layer.index)
        # 元に戻す
        _test_layer.move_to(original_index)


@suite.test
def test_layer_selected():
    """選択状態が取得・設定できる"""
    global _test_layer
    _test_layer.selected = True
    assert_true(_test_layer.selected)
    _test_layer.selected = False
    assert_false(_test_layer.selected)


# ===== Phase 1: CRITICAL関数 =====


@suite.test
def test_layer_parent_comp_via_sdk():
    """レイヤーの親コンポジションが取得できる（SDK経由）"""
    global _test_layer, _test_comp
    # SDK function を使用してテスト
    import ae.sdk as sdk
    layer_handle = _test_layer._handle
    comp_handle = sdk.AEGP_GetLayerParentComp(layer_handle)
    assert_true(comp_handle > 0, "Parent comp handle should be positive")


@suite.test
def test_active_layer_via_sdk():
    """アクティブレイヤーが取得できる（SDK経由）"""
    global _test_layer
    # レイヤーを選択
    _test_layer.selected = True
    # SDK function を使用してテスト
    import ae.sdk as sdk
    active_layer_handle = sdk.AEGP_GetActiveLayer()
    # アクティブレイヤーがある場合はハンドルが返る
    # 0の場合はアクティブレイヤーがない
    assert_true(active_layer_handle >= 0, "Active layer handle should be non-negative")


@suite.test
def test_layer_id():
    """レイヤーIDが取得できる"""
    global _test_layer
    layer_id = _test_layer.id
    assert_true(layer_id > 0, "Layer ID should be positive")


@suite.test
def test_layer_from_id_via_sdk():
    """レイヤーIDからレイヤーを取得できる（SDK経由）"""
    global _test_comp, _test_layer
    import ae.sdk as sdk
    layer_id = _test_layer.id
    comp_handle = _test_comp._handle
    # IDからレイヤーハンドルを取得
    found_layer_handle = sdk.AEGP_GetLayerFromLayerID(comp_handle, layer_id)
    assert_true(found_layer_handle > 0, "Should find layer handle by ID")


@suite.test
def test_layer_is_2d():
    """2Dレイヤー判定ができる"""
    global _test_layer
    # Most layers are 2D by default
    is_2d = _test_layer.is_2d
    assert_true(is_2d, "Test layer should be 2D")


@suite.test
def test_layer_is_3d():
    """3Dレイヤー判定ができる"""
    global _test_layer
    # Default layer is 2D, not 3D
    is_3d = _test_layer.is_3d_layer
    assert_false(is_3d, "Test layer should not be 3D by default")


@suite.test
def test_layer_time_remapping_enabled():
    """タイムリマップの有効フラグが取得できる"""
    global _test_comp

    # Time Remapping requires a footage or nested comp layer (not solid)
    # Create a nested comp to test time remapping
    proj = ae.Project.get_current()
    inner_comp = proj.create_comp("_TimeRemapInnerComp", 100, 100, 1.0, 5.0, 30.0)
    nested_layer = _test_comp.add_layer(inner_comp)

    # Get current state (should be False by default)
    # Note: AEGP_SetLayerFlag for TIME_REMAPPING flag is read-only in After Effects
    # The flag can be read but setting it via SDK may not work for all layer types
    time_remap_enabled = nested_layer.time_remapping_enabled
    assert_isinstance(time_remap_enabled, bool, "time_remapping_enabled should return bool")
    assert_false(time_remap_enabled, "Time remapping should be disabled by default for new layers")

    # Cleanup
    nested_layer.delete()
    inner_comp.delete()


# ===== Phase 2: HIGH PRIORITY関数 =====


@suite.test
def test_layer_stretch():
    """タイムストレッチが取得・設定できる"""
    global _test_layer
    original = _test_layer.stretch
    # Set to 50% speed (1/2)
    _test_layer.stretch = (1, 2)
    stretch = _test_layer.stretch
    assert_equal(1, stretch[0], "Stretch numerator should be 1")
    assert_equal(2, stretch[1], "Stretch denominator should be 2")
    # Restore
    _test_layer.stretch = original


@suite.test
def test_layer_transfer_mode_get():
    """転送モード（ブレンドモード）が取得できる"""
    global _test_layer
    transfer_mode = _test_layer.transfer_mode
    assert_isinstance(transfer_mode, dict, "Transfer mode should be a dict")
    assert_true("mode" in transfer_mode, "Should have 'mode' key")
    assert_true("flags" in transfer_mode, "Should have 'flags' key")
    assert_true("track_matte" in transfer_mode, "Should have 'track_matte' key")


@suite.test
def test_layer_transfer_mode_set():
    """転送モード（ブレンドモード）が設定できる"""
    global _test_layer
    original = _test_layer.transfer_mode
    # Set to Multiply using BlendMode enum
    new_mode = {"mode": ae.BlendMode.Multiply, "flags": original["flags"], "track_matte": original["track_matte"]}
    _test_layer.transfer_mode = new_mode
    current = _test_layer.transfer_mode
    assert_equal(ae.BlendMode.Multiply, current["mode"], "Transfer mode should be Multiply")
    # Restore
    _test_layer.transfer_mode = original


@suite.test
def test_layer_quality():
    """レイヤー品質が取得・設定できる"""
    global _test_layer
    original = _test_layer.quality
    # Set to BEST using LayerQuality enum
    _test_layer.quality = ae.LayerQuality.Best
    assert_equal(ae.LayerQuality.Best, _test_layer.quality, "Quality should be BEST")
    # Set to DRAFT
    _test_layer.quality = ae.LayerQuality.Draft
    assert_equal(ae.LayerQuality.Draft, _test_layer.quality, "Quality should be DRAFT")
    # Restore
    _test_layer.quality = original


@suite.test
def test_layer_sampling_quality():
    """サンプリング品質が取得・設定できる"""
    global _test_layer
    original = _test_layer.sampling_quality
    # Set to BICUBIC using SamplingQuality enum
    _test_layer.sampling_quality = ae.SamplingQuality.Bicubic
    assert_equal(ae.SamplingQuality.Bicubic, _test_layer.sampling_quality, "Sampling quality should be BICUBIC")
    # Set to BILINEAR
    _test_layer.sampling_quality = ae.SamplingQuality.Bilinear
    assert_equal(ae.SamplingQuality.Bilinear, _test_layer.sampling_quality, "Sampling quality should be BILINEAR")
    # Restore
    _test_layer.sampling_quality = original


@suite.test
def test_layer_label_get():
    """ラベルカラーが取得できる"""
    global _test_layer
    label = _test_layer.label
    # Label should be -1 (NONE) or 0-16 (color labels)
    assert_true(label >= -1 and label <= 16, "Label should be in valid range")


@suite.test
def test_layer_label_set():
    """ラベルカラーが設定できる"""
    global _test_layer
    original = _test_layer.label
    # Set to Red (1)
    _test_layer.label = 1
    assert_equal(1, _test_layer.label, "Label should be Red")
    # Set to Blue (5)
    _test_layer.label = 5
    assert_equal(5, _test_layer.label, "Label should be Blue")
    # Restore
    _test_layer.label = original


# ===== Phase 3: LayerSuite9 低レベルAPI =====


@suite.test
def test_convert_comp_to_layer_time():
    """コンポジション時間をレイヤー時間に変換できる"""
    global _test_comp, _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    comp_time = 1.0  # 1 second in comp time

    layer_time = sdk.AEGP_ConvertCompToLayerTime(layer_handle, comp_time)
    assert_true(isinstance(layer_time, (int, float)), "Should return a number")
    print(f"Comp time {comp_time}s -> Layer time {layer_time}s")


@suite.test
def test_convert_layer_to_comp_time():
    """レイヤー時間をコンポジション時間に変換できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    layer_time = 0.5  # 0.5 seconds in layer time

    comp_time = sdk.AEGP_ConvertLayerToCompTime(layer_handle, layer_time)
    assert_true(isinstance(comp_time, (int, float)), "Should return a number")
    print(f"Layer time {layer_time}s -> Comp time {comp_time}s")


@suite.test
def test_layer_source_item_id():
    """レイヤーのソースアイテムIDを取得できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    source_item_id = sdk.AEGP_GetLayerSourceItemID(layer_handle)
    assert_true(isinstance(source_item_id, int), "Should return integer")
    assert_true(source_item_id >= 0, "Source item ID should be non-negative")
    print(f"Source item ID: {source_item_id}")


@suite.test
def test_layer_current_time():
    """レイヤーの現在時間を取得できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    current_time = sdk.AEGP_GetLayerCurrentTime(layer_handle, sdk.AEGP_LTimeMode_CompTime)
    assert_true(isinstance(current_time, (int, float)), "Should return a number")
    print(f"Layer current time: {current_time}s")


@suite.test
def test_layer_object_type():
    """レイヤーのオブジェクトタイプを取得できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    obj_type = sdk.AEGP_GetLayerObjectType(layer_handle)
    assert_isinstance(obj_type, int)
    # AEGP_ObjectType_AV = 0, _LIGHT = 1, _CAMERA = 2, etc.
    print(f"Layer object type: {obj_type}")


@suite.test
def test_is_layer_video_really_on():
    """レイヤーのビデオが実際にオンか確認できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    is_on = sdk.AEGP_IsLayerVideoReallyOn(layer_handle)
    assert_isinstance(is_on, bool)
    print(f"Is layer video really on: {is_on}")


@suite.test
def test_is_layer_audio_really_on():
    """レイヤーのオーディオが実際にオンか確認できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    is_on = sdk.AEGP_IsLayerAudioReallyOn(layer_handle)
    assert_isinstance(is_on, bool)
    print(f"Is layer audio really on: {is_on}")


@suite.test
def test_is_video_active():
    """レイヤーのビデオがアクティブか確認できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    is_active = sdk.AEGP_IsVideoActive(layer_handle, sdk.AEGP_LTimeMode_CompTime, 0.0)
    assert_isinstance(is_active, bool)
    print(f"Is video active: {is_active}")


@suite.test
def test_layer_masked_bounds():
    """レイヤーのマスク適用後の境界を取得できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    bounds = sdk.AEGP_GetLayerMaskedBounds(
        layer_handle, sdk.AEGP_LTimeMode_CompTime, 0.0
    )
    # Returns tuple: (left, top, right, bottom)
    assert_isinstance(bounds, tuple)
    assert_equal(len(bounds), 4)
    left, top, right, bottom = bounds
    assert_isinstance(left, (int, float))
    assert_isinstance(top, (int, float))
    assert_isinstance(right, (int, float))
    assert_isinstance(bottom, (int, float))
    print(f"Masked bounds: left={left}, top={top}, right={right}, bottom={bottom}")


@suite.test
def test_layer_to_world_xform():
    """レイヤーからワールドへの変換行列を取得できる"""
    global _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle
    matrix = sdk.AEGP_GetLayerToWorldXform(layer_handle, 0.0)
    assert_isinstance(matrix, (list, tuple))
    # 4x4 matrix = 16 elements (or 4x4 nested)
    print(f"Layer to world matrix: {matrix}")


@suite.test
def test_is_add_layer_valid():
    """レイヤー追加が有効か確認できる"""
    global _test_comp, _test_layer
    import ae.sdk as sdk

    comp_handle = _test_comp._handle

    # Get the source item from an existing layer
    layer_handle = _test_layer._handle
    source_item = sdk.AEGP_GetLayerSourceItem(layer_handle)

    is_valid = sdk.AEGP_IsAddLayerValid(source_item, comp_handle)
    assert_isinstance(is_valid, bool)
    print(f"Is add layer valid: {is_valid}")


@suite.test
def test_track_matte_functions():
    """トラックマット関連の関数が動作する"""
    global _test_comp, _test_layer
    import ae.sdk as sdk

    layer_handle = _test_layer._handle

    # レイヤーがトラックマットとして使用されているか確認
    is_used_as_matte = sdk.AEGP_IsLayerUsedAsTrackMatte(layer_handle, False)
    assert_isinstance(is_used_as_matte, bool)
    print(f"Is layer used as track matte: {is_used_as_matte}")

    # レイヤーがトラックマットを持っているか確認
    has_matte = sdk.AEGP_DoesLayerHaveTrackMatte(layer_handle)
    assert_isinstance(has_matte, bool)
    print(f"Does layer have track matte: {has_matte}")

    # トラックマットレイヤーを取得（持っている場合）
    if has_matte:
        matte_layer = sdk.AEGP_GetTrackMatteLayer(layer_handle)
        print(f"Track matte layer: {matte_layer}")


@suite.test
def test_set_and_remove_track_matte():
    """トラックマットを設定・削除できる"""
    global _test_comp, _test_layer
    import ae.sdk as sdk

    # 2つのソリッドを作成
    matte_layer = _test_comp.add_solid("_MatteLayer", 200, 200, (1.0, 1.0, 1.0), 5.0)
    fill_layer = _test_comp.add_solid("_FillLayer", 200, 200, (1.0, 0.0, 0.0), 5.0)

    matte_handle = matte_layer._handle
    fill_handle = fill_layer._handle

    # トラックマットを設定 (Alpha Matte = 1)
    sdk.AEGP_SetTrackMatte(fill_handle, matte_handle, 1)

    # 設定されたことを確認
    has_matte = sdk.AEGP_DoesLayerHaveTrackMatte(fill_handle)
    assert_true(has_matte, "Fill layer should have track matte")

    # トラックマットを削除
    sdk.AEGP_RemoveTrackMatte(fill_handle)

    # 削除されたことを確認
    has_matte2 = sdk.AEGP_DoesLayerHaveTrackMatte(fill_handle)
    assert_false(has_matte2, "Fill layer should not have track matte after removal")

    # クリーンアップ
    matte_layer.delete()
    fill_layer.delete()


@suite.test
def test_get_new_layer_stream():
    """レイヤーストリームを取得できる"""
    global _test_layer
    import ae.sdk as sdk

    plugin_id = sdk.AEGP_GetPluginID()
    layer_handle = _test_layer._handle

    # AEGP_LayerStream_POSITION = 16
    streamH = sdk.AEGP_GetNewLayerStream(
        plugin_id, layer_handle, sdk.AEGP_LayerStream_POSITION
    )
    assert_true(streamH != 0, "Should get valid stream handle")

    # ストリームを破棄
    sdk.AEGP_DisposeStream(streamH)


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
