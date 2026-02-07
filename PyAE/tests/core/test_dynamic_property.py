import ae
import sys

# テスト用のユーティリティ関数をインポート
try:
    from ..test_utils import *
except ImportError:
    from test_utils import *

# TestSuiteの作成
suite = TestSuite("Dynamic Property")

_test_project = None
_test_comp = None
_test_layer = None


@suite.setup
def setup():
    global _test_project, _test_comp, _test_layer
    print("Setting up Dynamic Property tests...")

    # プロジェクト取得または作成
    _test_project = ae.Project.get_current()
    if not _test_project:
        _test_project = ae.new_project()

    # コンポジション作成
    _test_comp = _test_project.create_comp(
        "DynamicPropTestComp", 1920, 1080, 1.0, 10.0, 30.0
    )

    # 平面レイヤー作成
    _test_layer = _test_comp.add_solid(
        "TestSolid", 1920, 1080, 1.0, 0.0, 0.0, 10.0
    )


@suite.test
def test_property_access_by_name():
    """レイヤーからプロパティを名前で取得できる"""
    # Positionプロパティを取得
    pos = _test_layer.property("Position")
    assert_not_none(pos, "Should get Position property")
    assert_true(pos.valid, "Property should be valid")


@suite.test
def test_property_value_get_set():
    """プロパティの値の取得と設定"""
    # Opacityの値を取得
    opacity = _test_layer.property("Opacity")
    assert_not_none(opacity, "Should get Opacity property")

    # デフォルト値は100
    initial_value = opacity.value
    assert_equal(100.0, initial_value, "Default opacity should be 100")

    # 値を変更
    opacity.value = 50.0
    assert_equal(50.0, opacity.value, "Opacity should be 50")


@suite.test
def test_transform_property_access():
    """Transform系プロパティへのアクセス"""
    # Positionの値を取得
    pos = _test_layer.property("Position")
    assert_not_none(pos, "Should get Position property")

    pos_value = pos.value
    assert_not_none(pos_value, "Position value should not be None")
    # Positionは[x, y]または[x, y, z]の配列
    assert_true(len(pos_value) >= 2, "Position should have at least 2 components")


@suite.test
def test_layer_properties_root():
    """layer.properties でルートプロパティグループを取得"""
    root = _test_layer.properties
    assert_not_none(root, "Should get root property group")
    assert_true(root.is_group, "Root should be a group")
    print(f"  Root match_name: {root.match_name}")
    print(f"  Root num_properties: {root.num_properties}")


@suite.test
def test_property_iteration():
    """プロパティグループのイテレーション"""
    root = _test_layer.properties
    assert_true(root.is_group, "Root should be a group")

    count = 0
    for child in root:
        count += 1
        print(f"  Child: {child.name} ({child.match_name})")
        if count >= 5:
            print("  ... (truncated)")
            break

    assert_true(count > 0, "Should have at least one child property")


@suite.test
def test_property_by_index():
    """インデックスでプロパティを取得"""
    root = _test_layer.properties
    if root.num_properties > 0:
        first = root.property(0)
        assert_not_none(first, "Should get first property by index")
        print(f"  First property: {first.name} ({first.match_name})")


@suite.test
def test_property_by_matchname():
    """マッチ名でプロパティを取得"""
    root = _test_layer.properties
    # ADBE Transform Group は標準的なマッチ名
    transform = root.property("ADBE Transform Group")
    if transform:
        print(f"  Transform group found: {transform.name}")
        assert_true(transform.is_group, "Transform should be a group")
    else:
        print("  ADBE Transform Group not found (may vary by layer type)")


@suite.teardown
def teardown():
    """クリーンアップ"""
    pass


def run():
    """テストを実行"""
    return suite.run()
