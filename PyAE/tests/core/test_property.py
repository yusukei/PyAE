# test_property.py
# PyAE Property API テスト

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

suite = TestSuite("Property")

# テスト用変数
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp, _test_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_PropertyTestComp", 1920, 1080, 1.0, 10.0, 30.0)
    _test_layer = _test_comp.add_solid("_TestLayer", 100, 100, (1.0, 0.0, 0.0), 10.0)


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    if _test_comp:
        _test_comp.delete()


@suite.test
def test_property_access():
    """レイヤーからプロパティを取得できる"""
    global _test_layer
    # Positionプロパティを取得
    pos = _test_layer.property("Position")
    assert_not_none(pos, "Should get Position property")
    # Display name is localized (e.g. "位置" in Japanese), so just check it's not empty
    assert_true(len(pos.name) > 0)
    assert_true(pos.valid, "Property should be valid")

    # Opacityプロパティを取得
    opacity = _test_layer.property("Opacity")
    assert_not_none(opacity, "Should get Opacity property")
    assert_true(len(opacity.name) > 0)


@suite.test
def test_property_value_access():
    """プロパティの値を取得・設定できる"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # 値の取得
    val = opacity.value
    assert_isinstance(val, float)

    # 値の設定
    opacity.value = 50.0
    assert_close(50.0, opacity.value)

    # 元に戻す
    opacity.value = 100.0


@suite.test
def test_property_value_at_time():
    """指定時刻の値を取得・設定できる"""
    global _test_layer
    opacity = _test_layer.property("Opacity")

    # 時間変化しない場合
    opacity.set_value_at_time(0.5, 40.0)

    # アニメーションがない場合、set_value_at_timeはset_valueと同じ挙動（キーフレームなしなら）
    # ストップウォッチがOFFなら全体が変わるはず
    assert_close(40.0, opacity.get_value_at_time(0.5))
    assert_close(40.0, opacity.get_value_at_time(1.0))

    opacity.value = 100.0


@suite.test
def test_keyframes():
    """キーフレーム操作ができる"""
    global _test_layer
    # Positionでアニメーション
    pos = _test_layer.property("Position")

    assert_equal(0, pos.num_keyframes)
    assert_false(pos.is_time_varying)

    # キーフレーム追加
    pos.add_keyframe(0.0, [100, 100])
    pos.add_keyframe(1.0, [200, 200])

    assert_equal(2, pos.num_keyframes)
    assert_true(pos.is_time_varying)

    # キーフレーム情報の取得
    kf1_val = pos.get_keyframe_value(0)
    kf2_val = pos.get_keyframe_value(1)

    # 座標はタプルかリストで返るはず（実装依存だがPyProperty.cppではlist/tuple変換しているはず）
    # PyStreamUtilsの実装によるが、ここでは値が近似しているか確認
    assert_close(100.0, kf1_val[0])
    assert_close(200.0, kf2_val[0])

    assert_close(0.0, pos.get_keyframe_time(0))
    assert_close(1.0, pos.get_keyframe_time(1))

    # キーフレーム全削除
    pos.remove_all_keyframes()
    assert_equal(0, pos.num_keyframes)


@suite.test
def test_expression():
    """エクスプレッション操作ができる"""
    global _test_layer
    # Rotation
    rot = _test_layer.property("Rotation")

    # エクスプレッション設定
    expr_str = "time * 100"
    rot.expression = expr_str

    assert_true(rot.has_expression)
    assert_equal(expr_str, rot.expression)

    # エクスプレッション無効化
    rot.enable_expression(False)
    # has_expressionは「エクスプレッションが設定されているか」なのでTrueのままかも？
    # AE APIのAEGP_GetExpressionStateはEnabledを返すのか？
    # 実装を見ると AEGP_GetExpressionState を返している。これはEnable状態。
    # なのでFalseになるはず
    assert_false(rot.has_expression)

    # エクスプレッション削除（空文字設定）
    rot.expression = ""
    assert_equal("", rot.expression)


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
