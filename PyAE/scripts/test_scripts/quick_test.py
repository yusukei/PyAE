# quick_test.py
# PyAE クイックテスト - 各機能を個別にテストするシンプルなスクリプト
# Author: VTech Studio

"""
PyAE クイックテスト

このスクリプトは、PyAE の各機能を対話的にテストするためのものです。
各関数を個別に呼び出すことで、特定の機能をテストできます。
"""

import ae

def info():
    """基本情報を表示"""
    print("=" * 50)
    print("PyAE Quick Test")
    print("=" * 50)
    print(f"Version: {ae.version()}")
    print(f"Initialized: {ae.is_initialized()}")
    print()

# =============================================================
# 1. プロジェクト関連
# =============================================================
def test_project():
    """プロジェクト取得テスト"""
    print("\n--- Test: get_project() ---")
    try:
        project = ae.get_project()
        print(f"Project: {project}")
        if project:
            print(f"  Name: {project.name}")
            print(f"  Items: {project.num_items}")
        return project
    except Exception as e:
        print(f"ERROR: {e}")
        return None

# =============================================================
# 2. コンポジション関連
# =============================================================
def test_active_comp():
    """アクティブコンポ取得テスト"""
    print("\n--- Test: get_active_comp() ---")
    try:
        comp = ae.get_active_comp()
        print(f"Active comp: {comp}")
        if comp and comp.valid:
            print(f"  Name: {comp.name}")
            print(f"  Size: {comp.width}x{comp.height}")
            print(f"  Duration: {comp.duration}s")
            print(f"  FPS: {comp.frame_rate}")
            print(f"  Layers: {comp.num_layers}")
        return comp
    except Exception as e:
        print(f"ERROR: {e}")
        return None

def test_create_comp(name="QuickTest_Comp", width=1920, height=1080, duration=5.0, fps=30.0):
    """コンポ作成テスト"""
    print(f"\n--- Test: Create Comp '{name}' ---")
    try:
        comp = ae.Comp.create(
            name=name,
            width=width,
            height=height,
            pixel_aspect=1.0,
            duration=duration,
            frame_rate=fps
        )
        print(f"Created: {comp.name if comp else 'None'}")
        return comp
    except Exception as e:
        print(f"ERROR: {e}")
        return None

def test_japanese_name():
    """日本語名テスト"""
    print("\n--- Test: Japanese Names ---")
    try:
        comp = ae.Comp.create(
            name="日本語テスト_コンポジション",
            width=1280,
            height=720,
            pixel_aspect=1.0,
            duration=3.0,
            frame_rate=24.0
        )
        if comp:
            print(f"Created comp: {comp.name}")

            layer = comp.add_solid(
                name="日本語レイヤー_テスト",
                width=100, height=100,
                color=(1.0, 0.5, 0.0),
                duration=3.0
            )
            if layer:
                print(f"Created layer: {layer.name}")
        return comp
    except Exception as e:
        print(f"ERROR: {e}")
        return None

# =============================================================
# 3. レイヤー関連
# =============================================================
def test_add_solid(comp=None, name="Test Solid", color=(1.0, 0.0, 0.0)):
    """ソリッドレイヤー追加テスト"""
    print(f"\n--- Test: Add Solid '{name}' ---")
    if comp is None:
        comp = ae.get_active_comp()
    if not comp or not comp.valid:
        print("ERROR: No valid comp")
        return None

    try:
        layer = comp.add_solid(
            name=name,
            width=comp.width,
            height=comp.height,
            color=color,
            duration=comp.duration
        )
        if layer:
            print(f"Created: {layer.name}")
            print(f"  Index: {layer.index}")
        return layer
    except Exception as e:
        print(f"ERROR: {e}")
        return None

def test_layer_properties(layer=None):
    """レイヤープロパティテスト"""
    print("\n--- Test: Layer Properties ---")
    if layer is None:
        comp = ae.get_active_comp()
        if comp and comp.valid and comp.num_layers > 0:
            layer = comp.layer(1)

    if not layer or not layer.valid:
        print("ERROR: No valid layer")
        return False

    try:
        print(f"Layer: {layer.name}")
        print(f"  Index: {layer.index}")
        print(f"  Position: {layer.position}")
        print(f"  Opacity: {layer.opacity}")
        print(f"  Scale: {layer.scale}")
        print(f"  Rotation: {layer.rotation}")

        # 位置変更テスト
        print("\n  Setting position to [500, 300]...")
        layer.position = [500.0, 300.0]
        print(f"  New position: {layer.position}")

        # 不透明度変更テスト
        print("  Setting opacity to 50%...")
        layer.opacity = 50.0
        print(f"  New opacity: {layer.opacity}")

        return True
    except Exception as e:
        print(f"ERROR: {e}")
        return False

# =============================================================
# 4. キーフレーム関連
# =============================================================
def test_keyframes(layer=None):
    """キーフレームテスト"""
    print("\n--- Test: Keyframes ---")
    if layer is None:
        comp = ae.get_active_comp()
        if comp and comp.valid and comp.num_layers > 0:
            layer = comp.layer(1)

    if not layer or not layer.valid:
        print("ERROR: No valid layer")
        return False

    try:
        prop = layer.property("Position")
        if not prop:
            print("ERROR: Position property not found")
            return False

        print(f"Property: Position")
        print(f"  Keyframes: {prop.num_keyframes}")

        # キーフレーム追加
        print("\n  Adding keyframes...")
        prop.add_keyframe(0.0, [100.0, 100.0])
        prop.add_keyframe(1.0, [500.0, 300.0])
        prop.add_keyframe(2.0, [900.0, 500.0])

        print(f"  Keyframes after add: {prop.num_keyframes}")

        # キーフレーム情報
        for i in range(prop.num_keyframes):
            kf = prop.keyframe(i)
            if kf:
                print(f"    [{i}] Time: {kf.time}s, Value: {kf.value}")

        return True
    except Exception as e:
        print(f"ERROR: {e}")
        return False

# =============================================================
# 5. Undo/Redo関連
# =============================================================
def test_undo_group(comp=None):
    """Undoグループテスト"""
    print("\n--- Test: Undo Group ---")
    if comp is None:
        comp = ae.get_active_comp()
    if not comp or not comp.valid:
        print("ERROR: No valid comp")
        return False

    try:
        print("Creating layer within undo group...")
        with ae.UndoGroup("PyAE Quick Test"):
            layer = comp.add_solid(
                name="Undo Test",
                width=100, height=100,
                color=(0.0, 1.0, 0.0),
                duration=2.0
            )
            if layer:
                layer.position = [960.0, 540.0]
                layer.opacity = 75.0

        print("Done. Use Ctrl+Z to undo the entire operation.")
        return True
    except Exception as e:
        print(f"ERROR: {e}")
        return False

# =============================================================
# 6. エラーハンドリング
# =============================================================
def test_deleted_object():
    """削除されたオブジェクトへのアクセステスト"""
    print("\n--- Test: Deleted Object Access ---")
    comp = ae.get_active_comp()
    if not comp or not comp.valid:
        print("ERROR: No valid comp")
        return False

    try:
        # テストレイヤーを作成
        layer = comp.add_solid(
            name="Delete Test",
            width=50, height=50,
            color=(1.0, 1.0, 0.0),
            duration=1.0
        )
        print(f"Created layer: {layer.name}")

        # 削除
        layer.delete()
        print("Layer deleted")

        # アクセス試行
        print("Attempting to access deleted layer...")
        try:
            _ = layer.name
            print("WARNING: No error raised!")
        except Exception as e:
            print(f"Expected error raised: {type(e).__name__}: {e}")

        print(f"layer.valid = {layer.valid if hasattr(layer, 'valid') else 'N/A'}")
        return True
    except Exception as e:
        print(f"ERROR: {e}")
        return False

# =============================================================
# 全テスト実行
# =============================================================
def run_all():
    """全テストを実行"""
    info()

    print("\n" + "=" * 50)
    print("Running All Tests")
    print("=" * 50)

    # プロジェクト
    test_project()

    # コンポ
    comp = test_create_comp("AllTests_Comp")
    test_active_comp()

    # 日本語
    test_japanese_name()

    if comp and comp.valid:
        # レイヤー
        layer = test_add_solid(comp)
        if layer:
            test_layer_properties(layer)
            test_keyframes(layer)

        # Undo
        test_undo_group(comp)

        # 削除
        test_deleted_object()

    print("\n" + "=" * 50)
    print("All Tests Completed")
    print("=" * 50)

# メイン
if __name__ == "__main__":
    run_all()
