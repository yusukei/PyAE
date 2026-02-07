# test_comp.py
# PyAE Composition テスト

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

suite = TestSuite("Composition")

# テスト用変数
_test_comp = None
_test_solid = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp, _test_solid
    # ae.Compを直接作成（新機能テスト用）
    _test_comp = ae.Comp.create("_CompTest", 1920, 1080, 1.0, 10.0, 30.0)
    # ソリッドレイヤーを追加
    _test_solid = _test_comp.add_solid("_TestSolid", 1920, 1080, (1.0, 0.0, 0.0), 5.0)


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    # Compから対応するItemを取得して削除
    if _test_comp is not None and _test_comp.valid:
        # Compから対応するItemを取得
        proj = ae.Project.get_current()
        items = proj.items
        for item in items:
            if item.name == "_CompTest" or item.name.startswith("_CompTest"):
                item.delete()


@suite.test
def test_comp_name():
    """コンポジション名が取得・設定できる"""
    global _test_comp
    assert_equal("_CompTest", _test_comp.name)
    _test_comp.name = "_CompTestRenamed"
    assert_equal("_CompTestRenamed", _test_comp.name)
    _test_comp.name = "_CompTest"  # 元に戻す


@suite.test
def test_comp_dimensions():
    """コンポジションのサイズが正しい"""
    global _test_comp
    assert_equal(1920, _test_comp.width)
    assert_equal(1080, _test_comp.height)


@suite.test
def test_comp_pixel_aspect():
    """ピクセルアスペクト比が正しい"""
    global _test_comp
    assert_close(1.0, _test_comp.pixel_aspect)


@suite.test
def test_comp_duration():
    """コンポジションのデュレーションが正しい"""
    global _test_comp
    assert_close(10.0, _test_comp.duration)


@suite.test
def test_comp_framerate():
    """フレームレートが正しい"""
    global _test_comp
    assert_close(30.0, _test_comp.frame_rate)


@suite.test
def test_comp_set_duration():
    """デュレーションを変更できる"""
    global _test_comp
    _test_comp.duration = 15.0
    assert_close(15.0, _test_comp.duration)
    _test_comp.duration = 10.0  # 元に戻す


@suite.test
def test_comp_set_framerate():
    """フレームレートを変更できる"""
    global _test_comp
    try:
        _test_comp.frame_rate = 24.0
        assert_close(24.0, _test_comp.frame_rate)
        _test_comp.frame_rate = 30.0  # 元に戻す
    except RuntimeError as e:
        if "not supported" in str(e):
            # SDK limitation - skip test
            pass
        else:
            raise


@suite.test
def test_comp_layers():
    """レイヤー一覧が取得できる"""
    global _test_comp
    layers = _test_comp.layers
    assert_isinstance(layers, list)
    assert_true(len(layers) >= 1, "Should have at least one layer")


@suite.test
def test_comp_num_layers():
    """レイヤー数が取得できる"""
    global _test_comp
    num = _test_comp.num_layers
    assert_true(num >= 1, "Should have at least one layer")


@suite.test
def test_comp_layer_by_index():
    """インデックスでレイヤーを取得できる"""
    global _test_comp
    layer = _test_comp.layer(0)  # 0-based indexing
    assert_not_none(layer, "Layer at index 0 should exist")


@suite.test
def test_comp_layer_by_name():
    """名前でレイヤーを取得できる"""
    global _test_comp
    layer = _test_comp.layer("_TestSolid")
    assert_not_none(layer, "Should find layer by name")
    assert_equal("_TestSolid", layer.name)


@suite.test
def test_comp_add_solid():
    """ソリッドレイヤーを追加できる"""
    global _test_comp
    layer = _test_comp.add_solid("Solid", 1920, 1080, (1.0, 0.0, 0.0), 5.0)
    assert_not_none(layer, "Added solid should not be None")
    assert_equal("Solid", layer.name)
    # クリーンアップ
    layer.delete()


@suite.test
def test_comp_add_null():
    """ヌルレイヤーを追加できる"""
    global _test_comp
    null_layer = _test_comp.add_null("_TestNull", 5.0)
    assert_not_none(null_layer, "Added null should not be None")
    assert_equal("_TestNull", null_layer.name)
    # クリーンアップ
    null_layer.delete()


@suite.test
def test_comp_selected_layers():
    """選択レイヤーが取得できる"""
    global _test_comp, _test_solid
    # レイヤーを選択
    _test_solid.selected = True
    selected = _test_comp.selected_layers
    assert_isinstance(selected, list)
    # 選択解除
    _test_solid.selected = False


@suite.test
def test_comp_work_area():
    """ワークエリアが取得・設定できる"""
    global _test_comp
    start, duration = _test_comp.work_area_start, _test_comp.work_area_duration
    assert_true(isinstance(start, (int, float)))
    assert_true(isinstance(duration, (int, float)))


@suite.test
def test_comp_background_color():
    """背景色が取得・設定できる"""
    global _test_comp
    color = _test_comp.bg_color
    assert_isinstance(color, tuple)
    assert_equal(3, len(color), "Background color should have 3 components (RGB)")


@suite.test
def test_comp_set_dimensions():
    """コンポジションサイズを変更できる"""
    global _test_comp
    _test_comp.set_dimensions(1280, 720)
    assert_equal(1280, _test_comp.width)
    assert_equal(720, _test_comp.height)
    # 元に戻す
    _test_comp.set_dimensions(1920, 1080)


@suite.test
def test_comp_display_start_time():
    """表示開始時間が取得・設定できる"""
    global _test_comp
    start_time = _test_comp.display_start_time
    assert_true(isinstance(start_time, (int, float)))
    # 1秒に設定
    _test_comp.display_start_time = 1.0
    new_start = _test_comp.display_start_time
    assert_close(1.0, new_start, 0.1)
    # 元に戻す
    _test_comp.display_start_time = 0.0


@suite.test
def test_comp_show_layer_names():
    """レイヤー名/ソース名表示が取得・設定できる"""
    global _test_comp
    show = _test_comp.show_layer_names
    assert_isinstance(show, bool)
    # 反転
    _test_comp.show_layer_names = not show
    assert_equal(not show, _test_comp.show_layer_names)
    # 元に戻す
    _test_comp.show_layer_names = show


@suite.test
def test_comp_show_blend_modes():
    """ブレンドモード表示が取得・設定できる"""
    global _test_comp
    show = _test_comp.show_blend_modes
    assert_isinstance(show, bool)
    # 反転
    _test_comp.show_blend_modes = not show
    assert_equal(not show, _test_comp.show_blend_modes)
    # 元に戻す
    _test_comp.show_blend_modes = show


@suite.test
def test_comp_display_drop_frame():
    """ドロップフレーム表示状態が取得できる"""
    global _test_comp
    drop_frame = _test_comp.display_drop_frame
    assert_isinstance(drop_frame, bool)


@suite.test
def test_comp_frame_duration():
    """フレーム継続時間が取得できる"""
    global _test_comp
    frame_duration = _test_comp.frame_duration
    assert_true(isinstance(frame_duration, (int, float)))
    assert_true(frame_duration > 0, "Frame duration should be positive")
    # 30fpsの場合、約0.033秒
    expected = 1.0 / 30.0
    assert_close(expected, frame_duration, 0.001)


@suite.test
def test_comp_shutter_angle_phase():
    """シャッター角度と位相が取得できる"""
    global _test_comp
    angle_phase = _test_comp.shutter_angle_phase
    assert_isinstance(angle_phase, tuple)
    assert_equal(2, len(angle_phase), "Should return (angle, phase)")
    angle, phase = angle_phase
    assert_true(isinstance(angle, (int, float)))
    assert_true(isinstance(phase, (int, float)))


@suite.test
def test_comp_motion_blur_samples():
    """モーションブラーサンプル数が取得・設定できる"""
    global _test_comp
    samples = _test_comp.motion_blur_samples
    assert_isinstance(samples, int)
    # 16に設定
    _test_comp.motion_blur_samples = 16
    assert_equal(16, _test_comp.motion_blur_samples)
    # 元に戻す
    _test_comp.motion_blur_samples = samples


@suite.test
def test_comp_motion_blur_adaptive_sample_limit():
    """モーションブラー適応サンプル制限が取得・設定できる"""
    global _test_comp
    limit = _test_comp.motion_blur_adaptive_sample_limit
    assert_isinstance(limit, int)
    # 128に設定
    _test_comp.motion_blur_adaptive_sample_limit = 128
    assert_equal(128, _test_comp.motion_blur_adaptive_sample_limit)
    # 元に戻す
    _test_comp.motion_blur_adaptive_sample_limit = limit


@suite.test
def test_comp_duplicate():
    """コンポジションを複製できる"""
    global _test_comp
    duplicate = _test_comp.duplicate()
    assert_not_none(duplicate, "Duplicated comp should not be None")
    assert_true(duplicate.valid, "Duplicated comp should be valid")
    assert_equal(_test_comp.width, duplicate.width)
    assert_equal(_test_comp.height, duplicate.height)


@suite.test
def test_comp_add_box_text():
    """ボックステキストレイヤーを追加できる"""
    global _test_comp
    box_text = _test_comp.add_box_text(400, 200, True)
    assert_not_none(box_text, "Box text layer should not be None")
    assert_true(box_text.valid, "Box text layer should be valid")
    # クリーンアップ
    box_text.delete()


@suite.test
def test_comp_get_most_recently_used():
    """最近使用したコンポジションを取得できる"""
    comp = ae.Comp.get_most_recently_used()
    # 結果はNoneまたは有効なCompオブジェクト
    if comp is not None:
        assert_isinstance(comp, ae.Comp)


@suite.test
def test_comp_add_camera():
    """カメラレイヤーを追加できる"""
    global _test_comp
    camera = _test_comp.add_camera("_TestCamera", (0.0, 0.0))
    assert_not_none(camera, "Added camera should not be None")
    assert_equal("_TestCamera", camera.name)
    # クリーンアップ
    camera.delete()


@suite.test
def test_comp_add_light():
    """ライトレイヤーを追加できる"""
    global _test_comp
    light = _test_comp.add_light("_TestLight", (0.0, 0.0))
    assert_not_none(light, "Added light should not be None")
    assert_equal("_TestLight", light.name)
    # クリーンアップ
    light.delete()


@suite.test
def test_comp_add_text():
    """テキストレイヤーを追加できる"""
    global _test_comp
    text = _test_comp.add_text("Test Text")
    assert_not_none(text, "Added text layer should not be None")
    # クリーンアップ
    text.delete()


@suite.test
def test_comp_add_shape():
    """シェイプレイヤーを追加できる"""
    global _test_comp
    shape = _test_comp.add_shape()
    assert_not_none(shape, "Added shape layer should not be None")
    # クリーンアップ
    shape.delete()


@suite.test
def test_comp_set_pixel_aspect():
    """ピクセルアスペクト比を設定できる"""
    global _test_comp
    original = _test_comp.pixel_aspect
    _test_comp.pixel_aspect = 1.2
    assert_close(1.2, _test_comp.pixel_aspect, 0.01)
    # 元に戻す
    _test_comp.pixel_aspect = original


@suite.test
def test_comp_set_display_drop_frame():
    """ドロップフレーム表示を設定できる"""
    global _test_comp
    original = _test_comp.display_drop_frame
    _test_comp.display_drop_frame = not original
    assert_equal(not original, _test_comp.display_drop_frame)
    # 元に戻す
    _test_comp.display_drop_frame = original


@suite.test
def test_comp_set_bg_color():
    """背景色を設定できる"""
    global _test_comp
    original = _test_comp.bg_color
    _test_comp.bg_color = (1.0, 0.0, 0.0)  # 赤
    new_color = _test_comp.bg_color
    assert_close(1.0, new_color[0], 0.01)
    assert_close(0.0, new_color[1], 0.01)
    assert_close(0.0, new_color[2], 0.01)
    # 元に戻す
    _test_comp.bg_color = original


@suite.test
def test_comp_set_work_area():
    """ワークエリアを設定できる（SetWorkAreaStartAndDuration統合テスト）"""
    global _test_comp
    original_start = _test_comp.work_area_start
    original_duration = _test_comp.work_area_duration

    # 個別に設定
    _test_comp.work_area_start = 1.0
    _test_comp.work_area_duration = 5.0

    assert_close(1.0, _test_comp.work_area_start, 0.1)
    assert_close(5.0, _test_comp.work_area_duration, 0.1)

    # 元に戻す
    _test_comp.work_area_start = original_start
    _test_comp.work_area_duration = original_duration


@suite.test
def test_comp_resolution_property():
    """resolutionプロパティが正しく動作する"""
    global _test_comp
    resolution = _test_comp.resolution
    assert_isinstance(resolution, tuple)
    assert_equal(2, len(resolution))
    assert_equal(_test_comp.width, resolution[0])
    assert_equal(_test_comp.height, resolution[1])


@suite.test
def test_comp_current_time():
    """現在時間（CTI位置）プロパティが存在する"""
    global _test_comp
    # CTI取得/設定はCompSuite12には無いため、存在確認のみ
    time = _test_comp.current_time
    assert_true(isinstance(time, (int, float)))


@suite.test
def test_comp_layer_by_name_method():
    """layer_by_nameメソッドでレイヤーを取得できる"""
    global _test_comp
    layer = _test_comp.layer_by_name("_TestSolid")
    assert_not_none(layer, "Should find layer by name")
    assert_equal("_TestSolid", layer.name)


@suite.test
def test_comp_create_with_parameters():
    """ae.Comp.create()で指定パラメータのコンポジションを作成できる"""
    comp = ae.Comp.create("_TestCompCreate", 1280, 720, 1.0, 5.0, 24.0)
    assert_not_none(comp)
    assert_equal("_TestCompCreate", comp.name)
    assert_equal(1280, comp.width)
    assert_equal(720, comp.height)
    assert_close(5.0, comp.duration, 0.1)
    # フレームレート設定は環境依存でスキップ可能
    # クリーンアップ
    try:
        proj = ae.Project.get_current()
        for item in proj.items:
            if item.name == "_TestCompCreate":
                item.delete()
    except:
        pass


@suite.test
def test_comp_valid_property():
    """validプロパティが正しく動作する"""
    global _test_comp
    assert_true(_test_comp.valid, "Test comp should be valid")


@suite.test
def test_comp_num_layers_consistency():
    """num_layersとlayersの長さが一致する"""
    global _test_comp
    num = _test_comp.num_layers
    layers_list = _test_comp.layers
    assert_equal(num, len(layers_list), "num_layers should match layers list length")


# ===== Phase 3: CompSuite12 低レベルAPI =====


@suite.test
def test_comp_marker_stream():
    """コンポジションのマーカーストリームを取得できる"""
    global _test_comp
    import ae.sdk as sdk

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)

    # マーカーストリームを取得
    marker_streamH = sdk.AEGP_GetNewCompMarkerStream(compH)
    assert_true(marker_streamH != 0, "Should get valid marker stream handle")

    # ストリームを破棄
    sdk.AEGP_DisposeStream(marker_streamH)


@suite.test
def test_set_comp_duration():
    """コンポジションのデュレーションを設定できる（SDK経由）"""
    global _test_comp
    import ae.sdk as sdk

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)
    original_duration = _test_comp.duration

    # デュレーションを設定
    sdk.AEGP_SetCompDuration(compH, 8.0)

    # 確認（高レベルAPI経由）
    assert_close(8.0, _test_comp.duration, 0.1)

    # 元に戻す
    sdk.AEGP_SetCompDuration(compH, original_duration)


@suite.test
def test_set_comp_display_drop_frame_sdk():
    """ドロップフレーム表示を設定できる（SDK経由）"""
    global _test_comp
    import ae.sdk as sdk

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)
    original = sdk.AEGP_GetCompDisplayDropFrame(compH)

    # ドロップフレーム設定を切り替え
    sdk.AEGP_SetCompDisplayDropFrame(compH, not original)
    new_value = sdk.AEGP_GetCompDisplayDropFrame(compH)
    assert_equal(not original, new_value, "Drop frame should be toggled")

    # 元に戻す
    sdk.AEGP_SetCompDisplayDropFrame(compH, original)


@suite.test
def test_get_collection_from_comp_selection():
    """コンポジションの選択からコレクションを取得できる"""
    global _test_comp, _test_solid
    import ae.sdk as sdk

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)

    # レイヤーを選択
    _test_solid.selected = True

    # 選択からコレクションを取得
    collectionH = sdk.AEGP_GetNewCollectionFromCompSelection(compH)
    assert_true(collectionH != 0, "Should get valid collection handle")

    # コレクション内のアイテム数を確認
    num_items = sdk.AEGP_GetCollectionNumItems(collectionH)
    assert_true(num_items >= 1, "Collection should have at least 1 item (selected layer)")
    print(f"Selected items in collection: {num_items}")

    # コレクションを破棄
    sdk.AEGP_DisposeCollection(collectionH)

    # 選択解除
    _test_solid.selected = False


@suite.test
def test_set_selection():
    """コンポジションの選択を設定できる"""
    global _test_comp, _test_solid
    import ae.sdk as sdk

    plugin_id = sdk.AEGP_GetPluginID()
    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)

    # 新しいコレクションを作成
    collectionH = sdk.AEGP_NewCollection(plugin_id)

    # レイヤーをコレクションに追加
    layer_handle = _test_solid._handle
    item = {
        "type": sdk.AEGP_CollectionItemType_LAYER,
        "layerH": layer_handle
    }
    sdk.AEGP_CollectionPushBack(collectionH, item)

    # コレクションで選択を設定
    sdk.AEGP_SetSelection(compH, collectionH)

    # 選択されていることを確認
    assert_true(_test_solid.selected, "Layer should be selected")

    # クリーンアップ
    sdk.AEGP_DisposeCollection(collectionH)
    _test_solid.selected = False


@suite.test
def test_reorder_comp_selection():
    """コンポジションの選択を並び替えできる"""
    global _test_comp
    import ae.sdk as sdk

    compH = sdk.AEGP_GetCompFromItem(_test_comp._handle)

    # 複数のレイヤーを作成
    layer1 = _test_comp.add_solid("_ReorderTest1", 100, 100, (1.0, 0.0, 0.0), 5.0)
    layer2 = _test_comp.add_solid("_ReorderTest2", 100, 100, (0.0, 1.0, 0.0), 5.0)

    # 両方を選択
    layer1.selected = True
    layer2.selected = True

    # 選択を並び替え（最後尾に移動）
    num_layers = _test_comp.num_layers
    sdk.AEGP_ReorderCompSelection(compH, num_layers - 1)

    # 選択を解除
    layer1.selected = False
    layer2.selected = False

    # クリーンアップ
    layer1.delete()
    layer2.delete()


# ===== Tuple/List Overload Tests =====


@suite.test
def test_comp_add_solid_with_color_tuple():
    """add_solid()にカラーをタプルで渡せる"""
    global _test_comp
    layer = _test_comp.add_solid("_TupleSolid", 100, 100, (1.0, 0.0, 0.0), 5.0)
    assert_not_none(layer, "Solid with color tuple should be created")
    assert_equal("_TupleSolid", layer.name)
    layer.delete()


@suite.test
def test_comp_add_solid_with_color_list():
    """add_solid()にカラーをリストで渡せる"""
    global _test_comp
    layer = _test_comp.add_solid("_ListSolid", 100, 100, [0.0, 1.0, 0.0], 5.0)
    assert_not_none(layer, "Solid with color list should be created")
    assert_equal("_ListSolid", layer.name)
    layer.delete()


@suite.test
def test_comp_add_camera_with_center_tuple():
    """add_camera()にcenterをタプルで渡せる"""
    global _test_comp
    camera = _test_comp.add_camera("_TupleCamera", (960.0, 540.0))
    assert_not_none(camera, "Camera with center tuple should be created")
    assert_equal("_TupleCamera", camera.name)
    camera.delete()


@suite.test
def test_comp_add_light_with_center_tuple():
    """add_light()にcenterをタプルで渡せる"""
    global _test_comp
    light = _test_comp.add_light("_TupleLight", (960.0, 540.0))
    assert_not_none(light, "Light with center tuple should be created")
    assert_equal("_TupleLight", light.name)
    light.delete()


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
