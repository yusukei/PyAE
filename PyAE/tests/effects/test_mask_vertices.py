"""
test_mask_vertices.py
マスク頂点設定機能のテストスクリプト

Phase 8: マスク頂点設定の実装テスト
- 三角形マスクの作成
- 四角形マスクの作成
- ベジェ曲線を含む円形マスクの作成
"""

import ae
import math


def test_triangle_mask():
    """三角形マスクの作成テスト"""
    print("\n=== Test 1: Triangle Mask ===")

    app = ae.get_application()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("Test Triangle Mask", 1920, 1080, 1.0, 5.0, 30)

    # ソリッドレイヤー作成
    layer = comp.add_solid("Red Solid", 1920, 1080, color=(1, 0, 0), duration=1.0)

    # 三角形の頂点を定義（3つの頂点、接線なし）
    vertices = [
        {"x": 960.0, "y": 340.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},  # 上
        {"x": 1280.0, "y": 740.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},  # 右下
        {"x": 640.0, "y": 740.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},  # 左下
    ]

    # マスク追加
    mask = layer.add_mask(vertices)

    print(f"Created mask: {mask.name}")
    print(f"Mask index: {mask.index}")
    print(f"Number of masks on layer: {layer.num_masks}")

    # 検証
    assert mask is not None, "Mask should be created"
    assert layer.num_masks == 1, "Layer should have 1 mask"

    print("✅ Triangle mask test passed")
    return comp


def test_rectangle_mask():
    """四角形マスクの作成テスト"""
    print("\n=== Test 2: Rectangle Mask ===")

    app = ae.get_application()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("Test Rectangle Mask", 1920, 1080, 1.0, 5.0, 30)

    # ソリッドレイヤー作成
    layer = comp.add_solid("Green Solid", 1920, 1080, color=(0, 1, 0), duration=1.0)

    # 四角形の頂点を定義（4つの頂点、接線なし）
    vertices = [
        {"x": 640.0, "y": 340.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},  # 左上
        {"x": 1280.0, "y": 340.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},  # 右上
        {"x": 1280.0, "y": 740.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},  # 右下
        {"x": 640.0, "y": 740.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},  # 左下
    ]

    # マスク追加
    mask = layer.add_mask(vertices)

    print(f"Created mask: {mask.name}")
    print(f"Mask index: {mask.index}")

    # 検証
    assert mask is not None, "Mask should be created"
    assert layer.num_masks == 1, "Layer should have 1 mask"

    print("✅ Rectangle mask test passed")
    return comp


def test_circle_mask_with_bezier():
    """ベジェ曲線を含む円形マスクの作成テスト"""
    print("\n=== Test 3: Circle Mask with Bezier Curves ===")

    app = ae.get_application()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("Test Circle Mask", 1920, 1080, 1.0, 5.0, 30)

    # ソリッドレイヤー作成
    layer = comp.add_solid("Blue Solid", 1920, 1080, color=(0, 0, 1), duration=1.0)

    # 円を近似する4つの頂点とベジェ曲線
    # ベジェ定数（円の近似に使用）
    kappa = 4 * (math.sqrt(2) - 1) / 3

    cx, cy = 960.0, 540.0  # 中心
    radius = 200.0

    vertices = [
        # 上
        {
            "x": cx, "y": cy - radius,
            "tan_in_x": -kappa * radius, "tan_in_y": 0.0,
            "tan_out_x": kappa * radius, "tan_out_y": 0.0
        },
        # 右
        {
            "x": cx + radius, "y": cy,
            "tan_in_x": 0.0, "tan_in_y": -kappa * radius,
            "tan_out_x": 0.0, "tan_out_y": kappa * radius
        },
        # 下
        {
            "x": cx, "y": cy + radius,
            "tan_in_x": kappa * radius, "tan_in_y": 0.0,
            "tan_out_x": -kappa * radius, "tan_out_y": 0.0
        },
        # 左
        {
            "x": cx - radius, "y": cy,
            "tan_in_x": 0.0, "tan_in_y": kappa * radius,
            "tan_out_x": 0.0, "tan_out_y": -kappa * radius
        },
    ]

    # マスク追加
    mask = layer.add_mask(vertices)

    print(f"Created mask: {mask.name}")
    print(f"Mask index: {mask.index}")

    # 検証
    assert mask is not None, "Mask should be created"
    assert layer.num_masks == 1, "Layer should have 1 mask"

    print("✅ Circle mask with bezier test passed")
    return comp


def test_multiple_masks():
    """複数マスクの作成テスト"""
    print("\n=== Test 4: Multiple Masks ===")

    app = ae.get_application()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("Test Multiple Masks", 1920, 1080, 1.0, 5.0, 30)

    # ソリッドレイヤー作成
    layer = comp.add_solid("Yellow Solid", 1920, 1080, color=(1, 1, 0), duration=1.0)

    # 三角形マスク1
    vertices1 = [
        {"x": 480.0, "y": 270.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},
        {"x": 640.0, "y": 540.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},
        {"x": 320.0, "y": 540.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},
    ]
    mask1 = layer.add_mask(vertices1)

    # 三角形マスク2
    vertices2 = [
        {"x": 1440.0, "y": 270.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},
        {"x": 1600.0, "y": 540.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},
        {"x": 1280.0, "y": 540.0, "tan_in_x": 0.0, "tan_in_y": 0.0, "tan_out_x": 0.0, "tan_out_y": 0.0},
    ]
    mask2 = layer.add_mask(vertices2)

    print(f"Created mask 1: {mask1.name}")
    print(f"Created mask 2: {mask2.name}")
    print(f"Number of masks on layer: {layer.num_masks}")

    # 検証
    assert mask1 is not None, "Mask 1 should be created"
    assert mask2 is not None, "Mask 2 should be created"
    assert layer.num_masks == 2, "Layer should have 2 masks"

    print("✅ Multiple masks test passed")
    return comp


def test_empty_mask():
    """頂点なしマスク（デフォルトマスク）の作成テスト"""
    print("\n=== Test 5: Empty Mask (Default) ===")

    app = ae.get_application()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("Test Empty Mask", 1920, 1080, 1.0, 5.0, 30)

    # ソリッドレイヤー作成
    layer = comp.add_solid("Magenta Solid", 1920, 1080, color=(1, 0, 1), duration=1.0)

    # 頂点なしでマスク追加（デフォルトの四角形マスクが作成される）
    mask = layer.add_mask([])

    print(f"Created mask: {mask.name}")
    print(f"Mask index: {mask.index}")

    # 検証
    assert mask is not None, "Mask should be created"
    assert layer.num_masks == 1, "Layer should have 1 mask"

    print("✅ Empty mask test passed")
    return comp


def main():
    """メインテスト実行"""
    print("=" * 60)
    print("PyAE - Mask Vertices Setting Test")
    print("Phase 8: Mask Vertex Implementation Test")
    print("=" * 60)

    try:
        # Test 1: 三角形マスク
        comp1 = test_triangle_mask()

        # Test 2: 四角形マスク
        comp2 = test_rectangle_mask()

        # Test 3: ベジェ曲線を含む円形マスク
        comp3 = test_circle_mask_with_bezier()

        # Test 4: 複数マスク
        comp4 = test_multiple_masks()

        # Test 5: 頂点なしマスク（デフォルト）
        comp5 = test_empty_mask()

        print("\n" + "=" * 60)
        print("✅ All tests passed!")
        print("=" * 60)
        print("\nCreated compositions:")
        print(f"  - {comp1.name}")
        print(f"  - {comp2.name}")
        print(f"  - {comp3.name}")
        print(f"  - {comp4.name}")
        print(f"  - {comp5.name}")
        print("\nPlease verify the masks visually in After Effects.")

    except Exception as e:
        print("\n" + "=" * 60)
        print(f"❌ Test failed: {e}")
        print("=" * 60)
        raise


if __name__ == "__main__":
    main()
