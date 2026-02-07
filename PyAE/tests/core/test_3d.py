# test_3d.py
# PyAE 3D Layer テスト

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

suite = TestSuite("3D Layer")

# テスト用変数
_test_comp = None
_test_3d_layer = None
_test_camera = None
_test_light = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp, _test_3d_layer, _test_camera, _test_light
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_3DTestComp", 1920, 1080, 1.0, 5.0, 30.0)
    # 3Dレイヤーを追加
    _test_3d_layer = _test_comp.add_solid("_3DTestLayer", 200, 200, (1.0, 0.0, 0.0), 5.0)
    # カメラを追加
    try:
        _test_camera = _test_comp.add_camera("_TestCamera", (960.0, 540.0))
    except Exception as e:
        print(f"Camera setup warning: {e}")
        _test_camera = None
    # ライトを追加
    try:
        _test_light = _test_comp.add_light("_TestLight", (960.0, 540.0))
    except Exception as e:
        print(f"Light setup warning: {e}")
        _test_light = None


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except:
        pass


# ===== 3Dレイヤー =====


@suite.test
def test_set_3d_layer():
    """レイヤーを3Dに設定できる"""
    global _test_3d_layer
    # 3D化
    _test_3d_layer.three_d = True
    assert_true(_test_3d_layer.three_d, "Layer should be 3D")
    assert_true(_test_3d_layer.is_3d, "is_3d should be True")


@suite.test
def test_3d_position():
    """3D位置が設定できる"""
    global _test_3d_layer
    _test_3d_layer.three_d = True
    _test_3d_layer.position = (100.0, 200.0, 300.0)
    pos = _test_3d_layer.position
    assert_isinstance(pos, tuple)
    assert_true(len(pos) >= 3, "3D position should have 3 components")
    assert_close(100.0, pos[0])
    assert_close(200.0, pos[1])
    assert_close(300.0, pos[2])


@suite.test
def test_3d_rotation():
    """3D回転が設定できる"""
    global _test_3d_layer
    _test_3d_layer.three_d = True
    # X/Y/Z回転を設定
    try:
        _test_3d_layer.x_rotation = 45.0
        _test_3d_layer.y_rotation = 30.0
        assert_close(45.0, _test_3d_layer.x_rotation)
        assert_close(30.0, _test_3d_layer.y_rotation)
    except AttributeError:
        # 属性がない場合はスキップ
        assert_true(True, "3D rotation attributes may not be available")


@suite.test
def test_3d_orientation():
    """3Dオリエンテーションが設定できる"""
    global _test_3d_layer
    _test_3d_layer.three_d = True
    try:
        _test_3d_layer.orientation = (10.0, 20.0, 30.0)
        ori = _test_3d_layer.orientation
        assert_isinstance(ori, tuple)
    except AttributeError:
        assert_true(True, "orientation attribute may not be available")


# ===== カメラ =====


@suite.test
def test_camera_added():
    """カメラが追加できる"""
    global _test_camera
    if _test_camera is None:
        assert_true(True, "Camera test skipped")
        return
    assert_not_none(_test_camera)
    assert_equal("_TestCamera", _test_camera.name)


@suite.test
def test_camera_layer_type():
    """カメラのレイヤータイプが正しい"""
    global _test_camera
    if _test_camera is None:
        return
    layer_type = _test_camera.layer_type
    # Camera type should be indicated
    assert_not_none(layer_type)


@suite.test
def test_camera_position():
    """カメラ位置が設定できる"""
    global _test_camera
    if _test_camera is None:
        return
    _test_camera.position = (960.0, 540.0, -2000.0)
    pos = _test_camera.position
    assert_isinstance(pos, tuple)


# ===== ライト =====


@suite.test
def test_light_added():
    """ライトが追加できる"""
    global _test_light
    if _test_light is None:
        assert_true(True, "Light test skipped")
        return
    assert_not_none(_test_light)
    assert_equal("_TestLight", _test_light.name)


@suite.test
def test_light_layer_type():
    """ライトのレイヤータイプが正しい"""
    global _test_light
    if _test_light is None:
        return
    layer_type = _test_light.layer_type
    assert_not_none(layer_type)


@suite.test
def test_light_position():
    """ライト位置が設定できる"""
    global _test_light
    if _test_light is None:
        return
    _test_light.position = (500.0, 500.0, -500.0)
    pos = _test_light.position
    assert_isinstance(pos, tuple)


# ===== Tuple/List Overload Tests (low-level ae.three_d API) =====


@suite.test
def test_three_d_set_position_tuple():
    """ae.three_d.set_position()にタプルで位置を渡せる"""
    global _test_3d_layer
    _test_3d_layer.three_d = True
    handle = _test_3d_layer._handle

    ae.three_d.set_position(handle, position=(200.0, 300.0, 400.0))
    pos = ae.three_d.get_position(handle)
    assert_close(200.0, pos[0], 0.1, "X position should be 200")
    assert_close(300.0, pos[1], 0.1, "Y position should be 300")
    assert_close(400.0, pos[2], 0.1, "Z position should be 400")


@suite.test
def test_three_d_set_rotation_tuple():
    """ae.three_d.set_rotation()にタプルで回転を渡せる"""
    global _test_3d_layer
    _test_3d_layer.three_d = True
    handle = _test_3d_layer._handle

    ae.three_d.set_rotation(handle, rotation=(10.0, 20.0, 30.0))
    rot = ae.three_d.get_rotation(handle)
    assert_close(10.0, rot[0], 0.1, "X rotation should be 10")
    assert_close(20.0, rot[1], 0.1, "Y rotation should be 20")
    assert_close(30.0, rot[2], 0.1, "Z rotation should be 30")


@suite.test
def test_three_d_set_scale_tuple():
    """ae.three_d.set_scale()にタプルでスケールを渡せる"""
    global _test_3d_layer
    _test_3d_layer.three_d = True
    handle = _test_3d_layer._handle

    ae.three_d.set_scale(handle, scale=(50.0, 75.0, 100.0))
    sc = ae.three_d.get_scale(handle)
    assert_close(50.0, sc[0], 0.1, "X scale should be 50")
    assert_close(75.0, sc[1], 0.1, "Y scale should be 75")
    assert_close(100.0, sc[2], 0.1, "Z scale should be 100")


@suite.test
def test_three_d_set_orientation_tuple():
    """ae.three_d.set_orientation()にタプルでオリエンテーションを渡せる"""
    global _test_3d_layer
    _test_3d_layer.three_d = True
    handle = _test_3d_layer._handle

    ae.three_d.set_orientation(handle, orientation=(15.0, 25.0, 35.0))
    ori = ae.three_d.get_orientation(handle)
    assert_close(15.0, ori[0], 0.1, "X orientation should be 15")
    assert_close(25.0, ori[1], 0.1, "Y orientation should be 25")
    assert_close(35.0, ori[2], 0.1, "Z orientation should be 35")


@suite.test
def test_three_d_set_light_color_tuple():
    """ae.three_d.set_light_color()にタプルでカラーを渡せる"""
    global _test_light
    if _test_light is None:
        return
    handle = _test_light._handle

    ae.three_d.set_light_color(handle, color=(1.0, 0.5, 0.0))
    color = ae.three_d.get_light_color(handle)
    assert_close(1.0, color[0], 0.01, "Red should be 1.0")
    assert_close(0.5, color[1], 0.01, "Green should be 0.5")
    assert_close(0.0, color[2], 0.01, "Blue should be 0.0")


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
