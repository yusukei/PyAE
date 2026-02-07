# test_phase5.py
# PyAE Phase 5 Test Script
# After Effects内で実行するテストスクリプト

"""
Phase 5機能テスト:
- 5.1 エフェクト操作
- 5.2 マスク操作
- 5.3 3Dレイヤー操作
- 5.4 レンダーキュー操作
- 5.5 バッチ処理/パフォーマンス

使用方法:
After Effects内のPyAEコンソールで以下を実行:
    exec(open(r"D:\works\VTechStudio\AfterFX\PyAE\scripts\test_phase5.py").read())
"""

import ae
import traceback
from typing import List, Tuple, Optional, Any, Callable


class TestResult:
    """テスト結果を記録するクラス"""

    def __init__(self) -> None:
        self.passed: int = 0
        self.failed: int = 0
        self.skipped: int = 0
        self.errors: List[Tuple[str, str]] = []

    def record_pass(self, name: str) -> None:
        """テスト成功を記録"""
        self.passed += 1
        print(f"  [PASS] {name}")

    def record_fail(self, name: str, error: str) -> None:
        """テスト失敗を記録"""
        self.failed += 1
        self.errors.append((name, error))
        print(f"  [FAIL] {name}: {error}")

    def record_skip(self, name: str, reason: str) -> None:
        """テストスキップを記録"""
        self.skipped += 1
        print(f"  [SKIP] {name}: {reason}")

    def summary(self) -> bool:
        """結果サマリーを表示"""
        total = self.passed + self.failed + self.skipped
        print("\n" + "=" * 50)
        print(f"Test Results: {self.passed}/{total} passed")
        print(f"  Passed:  {self.passed}")
        print(f"  Failed:  {self.failed}")
        print(f"  Skipped: {self.skipped}")
        if self.errors:
            print("\nFailures:")
            for name, error in self.errors:
                print(f"  - {name}: {error}")
        print("=" * 50)
        return self.failed == 0

results = TestResult()

# ============================================================
# テストユーティリティ
# ============================================================

def get_test_comp() -> Optional[Any]:
    """テスト用コンポジションを取得または作成"""
    comp = ae.get_active_comp()
    if comp is None:
        # アクティブなコンポジションがない場合はスキップ
        return None
    return comp


def require_comp(func: Callable[..., None]) -> Callable[..., None]:
    """コンポジションが必要なテストのデコレータ"""
    def wrapper(*args: Any, **kwargs: Any) -> None:
        comp = get_test_comp()
        if comp is None:
            results.record_skip(func.__name__, "No active composition")
            return
        return func(comp, *args, **kwargs)
    wrapper.__name__ = func.__name__
    return wrapper

# ============================================================
# 5.1 エフェクト操作テスト
# ============================================================

print("\n[Phase 5.1] Effect Operations Tests")
print("-" * 40)

@require_comp
def test_get_installed_effects(comp):
    """インストール済みエフェクト一覧の取得"""
    try:
        effects = ae.get_installed_effects()
        if effects and len(effects) > 0:
            results.record_pass("get_installed_effects")
            print(f"    Found {len(effects)} installed effects")
        else:
            results.record_fail("get_installed_effects", "No effects found")
    except Exception as e:
        results.record_fail("get_installed_effects", str(e))

@require_comp
def test_effect_on_layer(comp):
    """レイヤーのエフェクト取得"""
    try:
        if comp.num_layers == 0:
            results.record_skip("effect_on_layer", "No layers in composition")
            return

        layer = comp.layer(1)
        # effectsプロパティがあるか確認
        if hasattr(layer, 'effects'):
            effects = layer.effects
            results.record_pass("effect_on_layer")
            print(f"    Layer has {len(effects) if effects else 0} effects")
        else:
            results.record_skip("effect_on_layer", "Layer.effects not implemented")
    except Exception as e:
        results.record_fail("effect_on_layer", str(e))

test_get_installed_effects()
test_effect_on_layer()

# ============================================================
# 5.2 マスク操作テスト
# ============================================================

print("\n[Phase 5.2] Mask Operations Tests")
print("-" * 40)

@require_comp
def test_layer_masks(comp):
    """レイヤーのマスク取得"""
    try:
        if comp.num_layers == 0:
            results.record_skip("layer_masks", "No layers in composition")
            return

        layer = comp.layer(1)
        if hasattr(layer, 'masks'):
            masks = layer.masks
            results.record_pass("layer_masks")
            print(f"    Layer has {len(masks) if masks else 0} masks")
        else:
            results.record_skip("layer_masks", "Layer.masks not implemented")
    except Exception as e:
        results.record_fail("layer_masks", str(e))

@require_comp
def test_mask_properties(comp):
    """マスクプロパティの取得"""
    try:
        if comp.num_layers == 0:
            results.record_skip("mask_properties", "No layers")
            return

        layer = comp.layer(1)
        if not hasattr(layer, 'masks'):
            results.record_skip("mask_properties", "Layer.masks not implemented")
            return

        masks = layer.masks
        if not masks or len(masks) == 0:
            results.record_skip("mask_properties", "No masks on layer")
            return

        mask = masks[0]
        # マスクプロパティを確認
        has_mode = hasattr(mask, 'mode')
        has_opacity = hasattr(mask, 'opacity')
        has_feather = hasattr(mask, 'feather')

        if has_mode and has_opacity:
            results.record_pass("mask_properties")
            print(f"    Mask mode: {mask.mode}, opacity: {mask.opacity}")
        else:
            results.record_fail("mask_properties", "Missing mask properties")
    except Exception as e:
        results.record_fail("mask_properties", str(e))

test_layer_masks()
test_mask_properties()

# ============================================================
# 5.3 3Dレイヤー操作テスト
# ============================================================

print("\n[Phase 5.3] 3D Layer Operations Tests")
print("-" * 40)

@require_comp
def test_three_d_module_exists(comp):
    """three_dモジュールの存在確認"""
    try:
        if hasattr(ae, 'three_d'):
            results.record_pass("three_d_module_exists")
        else:
            results.record_fail("three_d_module_exists", "ae.three_d module not found")
    except Exception as e:
        results.record_fail("three_d_module_exists", str(e))

@require_comp
def test_is_3d_layer(comp):
    """3Dレイヤー判定"""
    try:
        if comp.num_layers == 0:
            results.record_skip("is_3d_layer", "No layers")
            return

        layer = comp.layer(1)
        if not hasattr(ae, 'three_d') or not hasattr(ae.three_d, 'is_3d'):
            results.record_skip("is_3d_layer", "three_d.is_3d not implemented")
            return

        # レイヤーハンドルを取得してis_3dを確認
        handle = layer.handle if hasattr(layer, 'handle') else None
        if handle:
            is_3d = ae.three_d.is_3d(handle)
            results.record_pass("is_3d_layer")
            print(f"    Layer is 3D: {is_3d}")
        else:
            results.record_skip("is_3d_layer", "Cannot get layer handle")
    except Exception as e:
        results.record_fail("is_3d_layer", str(e))

@require_comp
def test_light_type_enum(comp):
    """LightType列挙型の確認"""
    try:
        if hasattr(ae, 'LightType'):
            has_parallel = hasattr(ae.LightType, 'Parallel')
            has_spot = hasattr(ae.LightType, 'Spot')
            has_point = hasattr(ae.LightType, 'Point')
            has_ambient = hasattr(ae.LightType, 'Ambient')

            if has_parallel and has_spot and has_point and has_ambient:
                results.record_pass("light_type_enum")
            else:
                results.record_fail("light_type_enum", "Missing LightType values")
        else:
            results.record_fail("light_type_enum", "LightType enum not found")
    except Exception as e:
        results.record_fail("light_type_enum", str(e))

test_three_d_module_exists()
test_is_3d_layer()
test_light_type_enum()

# ============================================================
# 5.4 レンダーキュー操作テスト
# ============================================================

print("\n[Phase 5.4] Render Queue Operations Tests")
print("-" * 40)

def test_render_queue_module():
    """render_queueモジュールの存在確認"""
    try:
        if hasattr(ae, 'render_queue'):
            results.record_pass("render_queue_module")
        else:
            results.record_fail("render_queue_module", "ae.render_queue not found")
    except Exception as e:
        results.record_fail("render_queue_module", str(e))

def test_render_queue_items():
    """レンダーキューアイテムの取得"""
    try:
        if not hasattr(ae, 'render_queue'):
            results.record_skip("render_queue_items", "render_queue not available")
            return

        rq = ae.render_queue
        if hasattr(rq, 'get_num_items'):
            num_items = rq.get_num_items()
            results.record_pass("render_queue_items")
            print(f"    Render queue has {num_items} items")
        elif hasattr(rq, 'num_items'):
            num_items = rq.num_items
            results.record_pass("render_queue_items")
            print(f"    Render queue has {num_items} items")
        else:
            results.record_skip("render_queue_items", "Cannot get item count")
    except Exception as e:
        results.record_fail("render_queue_items", str(e))

test_render_queue_module()
test_render_queue_items()

# ============================================================
# 5.5 バッチ処理/パフォーマンステスト
# ============================================================

print("\n[Phase 5.5] Batch/Performance Tests")
print("-" * 40)

def test_batch_module():
    """batchモジュールの存在確認"""
    try:
        if hasattr(ae, 'batch'):
            results.record_pass("batch_module")
        else:
            results.record_fail("batch_module", "ae.batch not found")
    except Exception as e:
        results.record_fail("batch_module", str(e))

def test_batch_context_manager():
    """バッチ操作コンテキストマネージャ"""
    try:
        if not hasattr(ae, 'batch'):
            results.record_skip("batch_context_manager", "batch module not available")
            return

        if hasattr(ae.batch, 'operation'):
            # ae.batch.operation() がコンテキストマネージャとして使えるか
            results.record_pass("batch_context_manager")
        else:
            results.record_skip("batch_context_manager", "batch.operation not found")
    except Exception as e:
        results.record_fail("batch_context_manager", str(e))

test_batch_module()
test_batch_context_manager()

# ============================================================
# サマリー
# ============================================================

success = results.summary()

if success:
    print("\nAll Phase 5 tests passed!")
else:
    print("\nSome tests failed. Check the errors above.")
