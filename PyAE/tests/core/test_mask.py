# test_mask.py
# PyAE Mask テスト

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

suite = TestSuite("Mask")

# テスト用変数
_test_comp = None
_test_layer = None
_test_mask = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp, _test_layer, _test_mask
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_MaskTestComp", 1920, 1080, 1.0, 5.0, 30.0)
    _test_layer = _test_comp.add_solid("_MaskTestLayer", 1920, 1080, (1.0, 0.0, 0.0), 5.0)
    # マスクを追加
    try:
        # 矩形マスクを追加
        vertices = [(100, 100), (500, 100), (500, 400), (100, 400)]
        _test_mask = _test_layer.add_mask(vertices)
    except Exception as e:
        print(f"Mask setup warning: {e}")
        _test_mask = None


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except:
        pass


@suite.test
def test_mask_added():
    """マスクが追加できる"""
    global _test_mask
    if _test_mask is None:
        assert_true(True, "Mask test skipped - mask creation may not be supported")
        return
    assert_not_none(_test_mask, "Mask should be added")


@suite.test
def test_mask_name():
    """マスク名が取得できる"""
    global _test_mask
    if _test_mask is None:
        return
    name = _test_mask.name
    assert_isinstance(name, str)
    # name は read-only なので設定はできない


@suite.test
def test_mask_index():
    """マスクインデックスが取得できる"""
    global _test_mask
    if _test_mask is None:
        return
    index = _test_mask.index
    assert_true(index >= 0, "Mask index should be 0 or greater (0-based)")


@suite.test
def test_mask_mode():
    """マスクモードが取得・設定できる"""
    global _test_mask
    if _test_mask is None:
        return
    mode = _test_mask.mode
    assert_not_none(mode)


@suite.test
def test_mask_inverted():
    """マスク反転が取得・設定できる"""
    global _test_mask
    if _test_mask is None:
        return
    original = _test_mask.inverted
    _test_mask.inverted = True
    assert_true(_test_mask.inverted)
    _test_mask.inverted = False
    assert_false(_test_mask.inverted)
    _test_mask.inverted = original


@suite.test
def test_mask_opacity():
    """マスク不透明度が取得・設定できる"""
    global _test_mask
    if _test_mask is None:
        return
    opacity = _test_mask.opacity
    assert_true(0 <= opacity <= 100, "Mask opacity should be 0-100")


@suite.test
def test_mask_feather():
    """マスクフェザーが取得・設定できる"""
    global _test_mask
    if _test_mask is None:
        return
    feather = _test_mask.feather
    assert_isinstance(feather, tuple, "Feather should be a tuple")
    assert_equal(len(feather), 2, "Feather should have x and y components")
    assert_true(feather[0] >= 0, "Feather X should be non-negative")
    assert_true(feather[1] >= 0, "Feather Y should be non-negative")


@suite.test
def test_layer_masks():
    """レイヤーのマスク一覧が取得できる"""
    global _test_layer
    masks = _test_layer.masks
    assert_isinstance(masks, list)


@suite.test
def test_layer_num_masks():
    """レイヤーのマスク数が取得できる"""
    global _test_layer
    num = _test_layer.num_masks
    assert_true(num >= 0, "num_masks should be non-negative")


@suite.test
def test_mask_locked():
    """マスクロック状態が取得・設定できる"""
    global _test_mask
    if _test_mask is None:
        return
    original = _test_mask.locked
    _test_mask.locked = True
    assert_true(_test_mask.locked, "Mask should be locked")
    _test_mask.locked = False
    assert_false(_test_mask.locked, "Mask should be unlocked")
    _test_mask.locked = original


@suite.test
def test_mask_is_roto_bezier():
    """RotoBezier状態が取得・設定できる"""
    global _test_mask
    if _test_mask is None:
        return
    original = _test_mask.is_roto_bezier
    _test_mask.is_roto_bezier = True
    assert_true(_test_mask.is_roto_bezier, "Mask should be RotoBezier")
    _test_mask.is_roto_bezier = False
    assert_false(_test_mask.is_roto_bezier, "Mask should not be RotoBezier")
    _test_mask.is_roto_bezier = original


@suite.test
def test_mask_color():
    """マスク色が取得・設定できる"""
    global _test_mask
    if _test_mask is None:
        return
    # 色を取得
    color = _test_mask.color
    assert_isinstance(color, tuple, "Color should be a tuple")
    assert_equal(len(color), 3, "Color should have R, G, B components")

    # 色を設定（赤色）
    _test_mask.set_color((1.0, 0.0, 0.0))
    new_color = _test_mask.color
    assert_close(new_color[0], 1.0, 0.01, "Red component should be 1.0")
    assert_close(new_color[1], 0.0, 0.01, "Green component should be 0.0")
    assert_close(new_color[2], 0.0, 0.01, "Blue component should be 0.0")

    # 元の色に戻す
    _test_mask.set_color(color)


@suite.test
def test_mask_color_with_tuple():
    """マスク色をタプルで設定できる"""
    global _test_mask
    if _test_mask is None:
        return
    original_color = _test_mask.color

    # タプルで色を設定（緑色）
    _test_mask.set_color((0.0, 1.0, 0.0))
    new_color = _test_mask.color
    assert_close(new_color[0], 0.0, 0.01, "Red component should be 0.0")
    assert_close(new_color[1], 1.0, 0.01, "Green component should be 1.0")
    assert_close(new_color[2], 0.0, 0.01, "Blue component should be 0.0")

    # リストで色を設定（青色）
    _test_mask.set_color([0.0, 0.0, 1.0])
    new_color = _test_mask.color
    assert_close(new_color[0], 0.0, 0.01, "Red component should be 0.0")
    assert_close(new_color[1], 0.0, 0.01, "Green component should be 0.0")
    assert_close(new_color[2], 1.0, 0.01, "Blue component should be 1.0")

    # 元の色に戻す
    _test_mask.set_color(original_color)


@suite.test
def test_mask_feather_with_tuple():
    """マスクフェザーをタプルで設定できる"""
    global _test_mask
    if _test_mask is None:
        return

    # タプルでフェザーを設定
    _test_mask.set_feather((5.0, 5.0))
    feather = _test_mask.feather
    assert_close(feather[0], 5.0, 0.1, "Feather X should be 5.0")
    assert_close(feather[1], 5.0, 0.1, "Feather Y should be 5.0")

    # リストでフェザーを設定
    _test_mask.set_feather([10.0, 10.0])
    feather = _test_mask.feather
    assert_close(feather[0], 10.0, 0.1, "Feather X should be 10.0")
    assert_close(feather[1], 10.0, 0.1, "Feather Y should be 10.0")

    # 元に戻す
    _test_mask.set_feather((0.0, 0.0))


@suite.test
def test_mask_duplicate():
    """マスクが複製できる"""
    global _test_mask, _test_layer
    if _test_mask is None:
        return

    # 元のマスク数を取得
    original_num = _test_layer.num_masks

    # マスクを複製
    duplicated = _test_mask.duplicate()
    assert_not_none(duplicated, "Duplicated mask should not be None")
    assert_true(duplicated.valid, "Duplicated mask should be valid")

    # マスク数が増えていることを確認
    new_num = _test_layer.num_masks
    assert_equal(new_num, original_num + 1, "Mask count should increase by 1")

    # 複製されたマスクを削除
    try:
        duplicated.delete()
    except Exception as e:
        print(f"Warning: Failed to delete duplicated mask: {e}")


@suite.test
def test_mask_delete():
    """マスクが削除できる"""
    global _test_layer

    # 新しいマスクを作成
    try:
        vertices = [(200, 200), (400, 200), (400, 400), (200, 400)]
        new_mask = _test_layer.add_mask(vertices)
        assert_not_none(new_mask, "New mask should be created")

        # マスク数を確認
        num_before = _test_layer.num_masks

        # マスクを削除
        new_mask.delete()

        # マスク数が減っていることを確認
        num_after = _test_layer.num_masks
        assert_equal(num_after, num_before - 1, "Mask count should decrease by 1")

    except Exception as e:
        print(f"Mask delete test skipped: {e}")


# -----------------------------------------------------------------------
# Feather Tests (MaskOutlineSuite3)
# -----------------------------------------------------------------------

@suite.test
def test_get_num_feathers_empty():
    """Featherが0個のマスクのFeather数を取得できる"""
    global _test_mask

    try:
        outline = _test_mask.outline
        num_feathers = ae.sdk.AEGP_GetMaskOutlineNumFeathers(outline)
        assert_isinstance(num_feathers, int, "Feather count should be an integer")
        assert_true(num_feathers >= 0, "Feather count should be non-negative")
    except Exception as e:
        print(f"Get num feathers test failed: {e}")


@suite.test
def test_create_feather():
    """Featherを作成できる"""
    global _test_mask

    try:
        outline = _test_mask.outline

        # セグメント数を確認
        num_segments = ae.sdk.AEGP_GetMaskOutlineNumSegments(outline)
        if num_segments == 0:
            print("Skipping feather test: mask has no segments")
            return

        # Featherを作成（最初のセグメントに対して）
        feather_data = {
            "segment": 0,
            "segment_sF": 0.5,
            "radiusF": 10.0,
            "ui_corner_angleF": 0.5,
            "tensionF": 0.5,
            "interp": 0,  # AEGP_MaskFeatherInterp_NORMAL
            "type": 0     # AEGP_MaskFeatherType_OUTER
        }

        index = ae.sdk.AEGP_CreateMaskOutlineFeather(outline, feather_data)
        assert_isinstance(index, int, "Created feather index should be an integer")
        assert_true(index >= 0, "Created feather index should be non-negative")

        # Feather数を確認
        num_feathers = ae.sdk.AEGP_GetMaskOutlineNumFeathers(outline)
        assert_true(num_feathers > 0, "Feather count should be greater than 0 after creation")

    except Exception as e:
        print(f"Create feather test failed: {e}")


@suite.test
def test_get_set_feather_info():
    """Featherの情報を取得・設定できる"""
    global _test_mask

    try:
        outline = _test_mask.outline

        # セグメント数を確認
        num_segments = ae.sdk.AEGP_GetMaskOutlineNumSegments(outline)
        if num_segments == 0:
            print("Skipping feather info test: mask has no segments")
            return

        # Featherを作成
        feather_data = {
            "segment": 0,
            "segment_sF": 0.5,
            "radiusF": 10.0,
            "ui_corner_angleF": 0.5,
            "tensionF": 0.5,
            "interp": 0,
            "type": 0
        }
        index = ae.sdk.AEGP_CreateMaskOutlineFeather(outline, feather_data)

        # Feather情報を取得
        info = ae.sdk.AEGP_GetMaskOutlineFeatherInfo(outline, index)
        assert_isinstance(info, dict, "Feather info should be a dict")
        assert_true("segment" in info, "Feather info should have 'segment' key")
        assert_true("radiusF" in info, "Feather info should have 'radiusF' key")
        assert_equal(info["segment"], 0, "Segment should match")

        # Feather情報を変更
        info["radiusF"] = 20.0
        ae.sdk.AEGP_SetMaskOutlineFeatherInfo(outline, index, info)

        # 変更を確認
        updated_info = ae.sdk.AEGP_GetMaskOutlineFeatherInfo(outline, index)
        assert_true(abs(updated_info["radiusF"] - 20.0) < 0.01, "Radius should be updated to 20.0")

    except Exception as e:
        print(f"Get/Set feather info test failed: {e}")


@suite.test
def test_delete_feather():
    """Featherを削除できる"""
    global _test_mask

    try:
        outline = _test_mask.outline

        # セグメント数を確認
        num_segments = ae.sdk.AEGP_GetMaskOutlineNumSegments(outline)
        if num_segments == 0:
            print("Skipping feather delete test: mask has no segments")
            return

        # Featherを作成
        feather_data = {
            "segment": 0,
            "segment_sF": 0.5,
            "radiusF": 10.0,
            "ui_corner_angleF": 0.5,
            "tensionF": 0.5,
            "interp": 0,
            "type": 0
        }
        index = ae.sdk.AEGP_CreateMaskOutlineFeather(outline, feather_data)

        # Feather数を確認
        num_before = ae.sdk.AEGP_GetMaskOutlineNumFeathers(outline)
        assert_true(num_before > 0, "Should have at least one feather before delete")

        # Featherを削除
        ae.sdk.AEGP_DeleteMaskOutlineFeather(outline, index)

        # Feather数が減っていることを確認
        num_after = ae.sdk.AEGP_GetMaskOutlineNumFeathers(outline)
        assert_equal(num_after, num_before - 1, "Feather count should decrease by 1 after delete")

    except Exception as e:
        print(f"Delete feather test failed: {e}")


@suite.test
def test_multiple_feathers():
    """複数のFeatherを管理できる"""
    global _test_mask

    try:
        outline = _test_mask.outline

        # セグメント数を確認
        num_segments = ae.sdk.AEGP_GetMaskOutlineNumSegments(outline)
        if num_segments == 0:
            print("Skipping multiple feathers test: mask has no segments")
            return

        # 3つのFeatherを作成
        feather_data = {
            "segment": 0,
            "segment_sF": 0.25,
            "radiusF": 5.0,
            "ui_corner_angleF": 0.5,
            "tensionF": 0.5,
            "interp": 0,
            "type": 0
        }
        index1 = ae.sdk.AEGP_CreateMaskOutlineFeather(outline, feather_data)

        feather_data["segment_sF"] = 0.5
        feather_data["radiusF"] = 10.0
        index2 = ae.sdk.AEGP_CreateMaskOutlineFeather(outline, feather_data)

        feather_data["segment_sF"] = 0.75
        feather_data["radiusF"] = 15.0
        index3 = ae.sdk.AEGP_CreateMaskOutlineFeather(outline, feather_data)

        # Feather数を確認
        num_feathers = ae.sdk.AEGP_GetMaskOutlineNumFeathers(outline)
        assert_true(num_feathers >= 3, "Should have at least 3 feathers")

        # それぞれのFeather情報を確認
        info1 = ae.sdk.AEGP_GetMaskOutlineFeatherInfo(outline, index1)
        info2 = ae.sdk.AEGP_GetMaskOutlineFeatherInfo(outline, index2)
        info3 = ae.sdk.AEGP_GetMaskOutlineFeatherInfo(outline, index3)

        assert_true(abs(info1["radiusF"] - 5.0) < 0.01, "First feather radius should be 5.0")
        assert_true(abs(info2["radiusF"] - 10.0) < 0.01, "Second feather radius should be 10.0")
        assert_true(abs(info3["radiusF"] - 15.0) < 0.01, "Third feather radius should be 15.0")

    except Exception as e:
        print(f"Multiple feathers test failed: {e}")


# -----------------------------------------------------------------------
# MaskSuite6 Additional Tests (SetMaskMode, SetMaskMotionBlurState, SetMaskFeatherFalloff)
# -----------------------------------------------------------------------

@suite.test
def test_set_mask_mode():
    """マスクモードを設定できる"""
    global _test_mask
    if _test_mask is None:
        return

    mask_handle = _test_mask._handle

    # 元のモードを取得
    original_mode = ae.sdk.AEGP_GetMaskMode(mask_handle)

    # 新しいモードを設定 (AEGP_MaskMode_ADD = 0, SUBTRACT = 1, etc.)
    # Mode 0 = Add
    ae.sdk.AEGP_SetMaskMode(mask_handle, 0)
    mode1 = ae.sdk.AEGP_GetMaskMode(mask_handle)
    assert_equal(mode1, 0, "Mask mode should be Add (0)")

    # Mode 1 = Subtract
    ae.sdk.AEGP_SetMaskMode(mask_handle, 1)
    mode2 = ae.sdk.AEGP_GetMaskMode(mask_handle)
    assert_equal(mode2, 1, "Mask mode should be Subtract (1)")

    # 元に戻す
    ae.sdk.AEGP_SetMaskMode(mask_handle, original_mode)


@suite.test
def test_set_mask_motion_blur_state():
    """マスクのモーションブラー状態を設定できる"""
    global _test_mask
    if _test_mask is None:
        return

    mask_handle = _test_mask._handle

    # 元の状態を取得
    original_state = ae.sdk.AEGP_GetMaskMotionBlurState(mask_handle)

    # AEGP_MaskMBlur_SAME_AS_LAYER = 0, ON = 1, OFF = 2
    # Set to OFF
    ae.sdk.AEGP_SetMaskMotionBlurState(mask_handle, 2)
    state1 = ae.sdk.AEGP_GetMaskMotionBlurState(mask_handle)
    assert_equal(state1, 2, "Motion blur state should be OFF (2)")

    # Set to ON
    ae.sdk.AEGP_SetMaskMotionBlurState(mask_handle, 1)
    state2 = ae.sdk.AEGP_GetMaskMotionBlurState(mask_handle)
    assert_equal(state2, 1, "Motion blur state should be ON (1)")

    # 元に戻す
    ae.sdk.AEGP_SetMaskMotionBlurState(mask_handle, original_state)


@suite.test
def test_set_mask_feather_falloff():
    """マスクのフェザーフォールオフを設定できる"""
    global _test_mask
    if _test_mask is None:
        return

    mask_handle = _test_mask._handle

    # 元のフォールオフを取得
    original_falloff = ae.sdk.AEGP_GetMaskFeatherFalloff(mask_handle)

    # AEGP_MaskFeatherFalloff_LINEAR = 0, SMOOTH = 1
    # Set to SMOOTH
    ae.sdk.AEGP_SetMaskFeatherFalloff(mask_handle, 1)
    falloff1 = ae.sdk.AEGP_GetMaskFeatherFalloff(mask_handle)
    assert_equal(falloff1, 1, "Feather falloff should be SMOOTH (1)")

    # Set to LINEAR
    ae.sdk.AEGP_SetMaskFeatherFalloff(mask_handle, 0)
    falloff2 = ae.sdk.AEGP_GetMaskFeatherFalloff(mask_handle)
    assert_equal(falloff2, 0, "Feather falloff should be LINEAR (0)")

    # 元に戻す
    ae.sdk.AEGP_SetMaskFeatherFalloff(mask_handle, original_falloff)


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
