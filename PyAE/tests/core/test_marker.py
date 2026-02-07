# test_marker.py
# PyAE Marker Suite テスト

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
    )

suite = TestSuite("Marker")

# テスト用変数
_marker1 = None
_marker2 = None
_plugin_id = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _marker1, _marker2, _plugin_id
    _plugin_id = ae.sdk.AEGP_GetPluginID()
    _marker1 = ae.sdk.AEGP_NewMarker()
    _marker2 = ae.sdk.AEGP_NewMarker()


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _marker1, _marker2
    try:
        if _marker1 is not None:
            ae.sdk.AEGP_DisposeMarker(_marker1)
        if _marker2 is not None:
            ae.sdk.AEGP_DisposeMarker(_marker2)
    except:
        pass


# -----------------------------------------------------------------------
# Basic Marker Creation/Disposal Tests
# -----------------------------------------------------------------------

@suite.test
def test_new_marker():
    """新しいマーカーを作成できる"""
    markerP = ae.sdk.AEGP_NewMarker()
    assert_isinstance(markerP, int, "Marker handle should be an integer")
    assert_true(markerP != 0, "Marker handle should be non-zero")
    ae.sdk.AEGP_DisposeMarker(markerP)


@suite.test
def test_dispose_marker():
    """マーカーを破棄できる"""
    markerP = ae.sdk.AEGP_NewMarker()
    ae.sdk.AEGP_DisposeMarker(markerP)
    assert_true(True, "Marker disposed successfully")


@suite.test
def test_duplicate_marker():
    """マーカーを複製できる"""
    global _marker1
    # 複製前に何かデータを設定 (strType=1 is COMMENT)
    ae.sdk.AEGP_SetMarkerString(_marker1, 1, "Test Comment")
    ae.sdk.AEGP_SetMarkerLabel(_marker1, 5)

    # 複製
    duplicated = ae.sdk.AEGP_DuplicateMarker(_marker1)
    assert_isinstance(duplicated, int, "Duplicated marker handle should be an integer")
    assert_true(duplicated != 0, "Duplicated marker handle should be non-zero")
    assert_true(duplicated != _marker1, "Duplicated marker should be different from original")

    # 複製されたデータを確認 (strType=1 is COMMENT)
    comment = ae.sdk.AEGP_GetMarkerString(_plugin_id, duplicated, 1)
    label = ae.sdk.AEGP_GetMarkerLabel(duplicated)
    assert_equal(comment, "Test Comment", "Comment should be duplicated")
    assert_equal(label, 5, "Label should be duplicated")

    ae.sdk.AEGP_DisposeMarker(duplicated)


# -----------------------------------------------------------------------
# Flag Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_set_marker_flag_navigation():
    """ナビゲーションフラグを取得・設定できる"""
    global _marker1
    # AEGP_MarkerFlag_NAVIGATION = 0x01

    # デフォルト値を取得
    default_val = ae.sdk.AEGP_GetMarkerFlag(_marker1, 1)
    assert_isinstance(default_val, bool, "Flag should be boolean")

    # Trueに設定
    ae.sdk.AEGP_SetMarkerFlag(_marker1, 1, True)
    val = ae.sdk.AEGP_GetMarkerFlag(_marker1, 1)
    assert_true(val, "Navigation flag should be True")

    # Falseに設定
    ae.sdk.AEGP_SetMarkerFlag(_marker1, 1, False)
    val = ae.sdk.AEGP_GetMarkerFlag(_marker1, 1)
    assert_false(val, "Navigation flag should be False")


@suite.test
def test_get_set_marker_flag_protect():
    """プロテクトフラグを取得・設定できる"""
    global _marker1
    # AEGP_MarkerFlag_PROTECT_REGION = 0x02

    ae.sdk.AEGP_SetMarkerFlag(_marker1, 2, True)
    val = ae.sdk.AEGP_GetMarkerFlag(_marker1, 2)
    assert_true(val, "Protect flag should be True")

    ae.sdk.AEGP_SetMarkerFlag(_marker1, 2, False)
    val = ae.sdk.AEGP_GetMarkerFlag(_marker1, 2)
    assert_false(val, "Protect flag should be False")


# -----------------------------------------------------------------------
# String Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_set_marker_string_comment():
    """コメント文字列を取得・設定できる"""
    global _marker1, _plugin_id
    # AEGP_MarkerString_COMMENT = 1

    test_comment = "Test Comment String"
    ae.sdk.AEGP_SetMarkerString(_marker1, 1, test_comment)
    result = ae.sdk.AEGP_GetMarkerString(_plugin_id, _marker1, 1)
    assert_equal(result, test_comment, "Comment should match")


@suite.test
def test_get_set_marker_string_chapter():
    """チャプター文字列を取得・設定できる"""
    global _marker1, _plugin_id
    # AEGP_MarkerString_CHAPTER = 2

    test_chapter = "Chapter 1"
    ae.sdk.AEGP_SetMarkerString(_marker1, 2, test_chapter)
    result = ae.sdk.AEGP_GetMarkerString(_plugin_id, _marker1, 2)
    assert_equal(result, test_chapter, "Chapter should match")


@suite.test
def test_get_set_marker_string_url():
    """URL文字列を取得・設定できる"""
    global _marker1, _plugin_id
    # AEGP_MarkerString_URL = 3

    test_url = "https://example.com"
    ae.sdk.AEGP_SetMarkerString(_marker1, 3, test_url)
    result = ae.sdk.AEGP_GetMarkerString(_plugin_id, _marker1, 3)
    assert_equal(result, test_url, "URL should match")


@suite.test
def test_get_set_marker_string_frame_target():
    """フレームターゲット文字列を取得・設定できる"""
    global _marker1, _plugin_id
    # AEGP_MarkerString_FRAME_TARGET = 4

    test_target = "_blank"
    ae.sdk.AEGP_SetMarkerString(_marker1, 4, test_target)
    result = ae.sdk.AEGP_GetMarkerString(_plugin_id, _marker1, 4)
    assert_equal(result, test_target, "Frame target should match")


@suite.test
def test_get_set_marker_string_cue_point_name():
    """キューポイント名を取得・設定できる"""
    global _marker1, _plugin_id
    # AEGP_MarkerString_CUE_POINT_NAME = 5

    test_name = "CuePoint1"
    ae.sdk.AEGP_SetMarkerString(_marker1, 5, test_name)
    result = ae.sdk.AEGP_GetMarkerString(_plugin_id, _marker1, 5)
    assert_equal(result, test_name, "Cue point name should match")


# -----------------------------------------------------------------------
# Duration Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_set_marker_duration():
    """マーカーのデュレーションを取得・設定できる"""
    global _marker1

    # デュレーションを設定 (2秒 = 2 * 30 = 60 frames at 30fps)
    test_duration = {"value": 60, "scale": 30}
    ae.sdk.AEGP_SetMarkerDuration(_marker1, test_duration)

    result = ae.sdk.AEGP_GetMarkerDuration(_marker1)
    assert_isinstance(result, dict, "Duration should be a dict")
    assert_true("value" in result, "Duration should have 'value' key")
    assert_true("scale" in result, "Duration should have 'scale' key")
    assert_equal(result["value"], 60, "Duration value should match")
    assert_equal(result["scale"], 30, "Duration scale should match")


# -----------------------------------------------------------------------
# Label Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_set_marker_label():
    """マーカーのラベル色を取得・設定できる"""
    global _marker1

    # デフォルト値を取得
    default_label = ae.sdk.AEGP_GetMarkerLabel(_marker1)
    assert_isinstance(default_label, int, "Label should be an integer")

    # ラベル色を設定 (1-16 = 色, 0 = None)
    ae.sdk.AEGP_SetMarkerLabel(_marker1, 7)
    result = ae.sdk.AEGP_GetMarkerLabel(_marker1)
    assert_equal(result, 7, "Label should be 7")

    # 0 (None) に設定
    ae.sdk.AEGP_SetMarkerLabel(_marker1, 0)
    result = ae.sdk.AEGP_GetMarkerLabel(_marker1)
    assert_equal(result, 0, "Label should be 0 (None)")


# -----------------------------------------------------------------------
# Cue Point Parameter Tests
# -----------------------------------------------------------------------

@suite.test
def test_count_cue_point_params_empty():
    """キューポイントパラメータ数を取得できる（空）"""
    global _marker1
    count = ae.sdk.AEGP_CountCuePointParams(_marker1)
    assert_isinstance(count, int, "Count should be an integer")
    assert_equal(count, 0, "New marker should have 0 cue point params")


@suite.test
def test_insert_cue_point_param():
    """キューポイントパラメータを挿入できる"""
    global _marker1

    # 最初のパラメータを挿入
    ae.sdk.AEGP_InsertCuePointParam(_marker1, 0)
    count = ae.sdk.AEGP_CountCuePointParams(_marker1)
    assert_equal(count, 1, "Should have 1 cue point param after insert")


@suite.test
def test_set_get_cue_point_param():
    """キューポイントパラメータを設定・取得できる"""
    global _marker1, _plugin_id

    # パラメータを挿入
    ae.sdk.AEGP_InsertCuePointParam(_marker1, 0)

    # パラメータを設定
    ae.sdk.AEGP_SetIndCuePointParam(_marker1, 0, "testKey", "testValue")

    # パラメータを取得
    result = ae.sdk.AEGP_GetIndCuePointParam(_plugin_id, _marker1, 0)
    assert_isinstance(result, dict, "Result should be a dict")
    assert_true("key" in result, "Result should have 'key'")
    assert_true("value" in result, "Result should have 'value'")
    assert_equal(result["key"], "testKey", "Key should match")
    assert_equal(result["value"], "testValue", "Value should match")


@suite.test
def test_multiple_cue_point_params():
    """複数のキューポイントパラメータを管理できる"""
    global _plugin_id

    # Create a fresh marker for this test
    marker = ae.sdk.AEGP_NewMarker()
    try:
        # 3つのパラメータを挿入
        ae.sdk.AEGP_InsertCuePointParam(marker, 0)
        ae.sdk.AEGP_InsertCuePointParam(marker, 1)
        ae.sdk.AEGP_InsertCuePointParam(marker, 2)

        # それぞれに値を設定
        ae.sdk.AEGP_SetIndCuePointParam(marker, 0, "key1", "value1")
        ae.sdk.AEGP_SetIndCuePointParam(marker, 1, "key2", "value2")
        ae.sdk.AEGP_SetIndCuePointParam(marker, 2, "key3", "value3")

        # 個数を確認
        count = ae.sdk.AEGP_CountCuePointParams(marker)
        assert_equal(count, 3, "Should have 3 cue point params")

        # それぞれを取得して確認
        param0 = ae.sdk.AEGP_GetIndCuePointParam(_plugin_id, marker, 0)
        assert_equal(param0["key"], "key1", "Param 0 key should match")
        assert_equal(param0["value"], "value1", "Param 0 value should match")

        param1 = ae.sdk.AEGP_GetIndCuePointParam(_plugin_id, marker, 1)
        assert_equal(param1["key"], "key2", "Param 1 key should match")
        assert_equal(param1["value"], "value2", "Param 1 value should match")

        param2 = ae.sdk.AEGP_GetIndCuePointParam(_plugin_id, marker, 2)
        assert_equal(param2["key"], "key3", "Param 2 key should match")
        assert_equal(param2["value"], "value3", "Param 2 value should match")
    finally:
        ae.sdk.AEGP_DisposeMarker(marker)


@suite.test
def test_delete_cue_point_param():
    """キューポイントパラメータを削除できる"""
    global _plugin_id

    # Create a fresh marker for this test
    marker = ae.sdk.AEGP_NewMarker()
    try:
        # 2つのパラメータを挿入
        ae.sdk.AEGP_InsertCuePointParam(marker, 0)
        ae.sdk.AEGP_InsertCuePointParam(marker, 1)
        ae.sdk.AEGP_SetIndCuePointParam(marker, 0, "key1", "value1")
        ae.sdk.AEGP_SetIndCuePointParam(marker, 1, "key2", "value2")

        # 最初のパラメータを削除
        ae.sdk.AEGP_DeleteIndCuePointParam(marker, 0)

        # 個数を確認
        count = ae.sdk.AEGP_CountCuePointParams(marker)
        assert_equal(count, 1, "Should have 1 cue point param after delete")

        # 残ったパラメータを確認（インデックスが詰まる）
        param0 = ae.sdk.AEGP_GetIndCuePointParam(_plugin_id, marker, 0)
        assert_equal(param0["key"], "key2", "Remaining param should be key2")
        assert_equal(param0["value"], "value2", "Remaining param should be value2")
    finally:
        ae.sdk.AEGP_DisposeMarker(marker)


@suite.test
def test_cue_point_param_unicode():
    """キューポイントパラメータがUnicode文字列を扱える"""
    global _marker1, _plugin_id

    ae.sdk.AEGP_InsertCuePointParam(_marker1, 0)

    # 日本語を含むキー・バリュー
    ae.sdk.AEGP_SetIndCuePointParam(_marker1, 0, "キー", "値")

    result = ae.sdk.AEGP_GetIndCuePointParam(_plugin_id, _marker1, 0)
    assert_equal(result["key"], "キー", "Unicode key should match")
    assert_equal(result["value"], "値", "Unicode value should match")


# -----------------------------------------------------------------------
# High-level API Tests (using ae.Marker class)
# -----------------------------------------------------------------------

@suite.test
def test_marker_class_creation():
    """ae.Markerクラスでマーカーを作成できる"""
    marker = ae.Marker()
    assert_not_none(marker, "Marker should be created")
    # Pythonのデストラクタで自動的に破棄される


@suite.test
def test_marker_class_properties():
    """ae.Markerクラスのプロパティが動作する"""
    marker = ae.Marker()

    # コメント
    marker.comment = "Test comment"
    assert_equal(marker.comment, "Test comment", "Comment should match")

    # チャプター
    marker.chapter = "Chapter 1"
    assert_equal(marker.chapter, "Chapter 1", "Chapter should match")

    # URL
    marker.url = "https://example.com"
    assert_equal(marker.url, "https://example.com", "URL should match")

    # フレームターゲット
    marker.frame_target = "_blank"
    assert_equal(marker.frame_target, "_blank", "Frame target should match")

    # キューポイント名
    marker.cue_point_name = "CuePoint1"
    assert_equal(marker.cue_point_name, "CuePoint1", "Cue point name should match")

    # デュレーション（秒単位）
    marker.duration = 2.5
    assert_true(abs(marker.duration - 2.5) < 0.01, "Duration should be approximately 2.5")

    # ラベル
    marker.label = 5
    assert_equal(marker.label, 5, "Label should match")

    # フラグ
    marker.navigation = True
    assert_true(marker.navigation, "Navigation flag should be True")

    marker.protected = True
    assert_true(marker.protected, "Protected flag should be True")


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
