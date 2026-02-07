#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Memory Leak Check - All Tests
全テストを実行してメモリリークをチェック
"""

import ae
import sys
import os

# テストディレクトリをパスに追加
test_dir = r"C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\tests"
if test_dir not in sys.path:
    sys.path.insert(0, test_dir)

def main():
    """全テストを実行してメモリリークをチェック"""
    print("=" * 80)
    print("PyAE Full Test Suite with Memory Leak Check")
    print("=" * 80)

    # 初期メモリ状態を記録
    print("\n[Initial Memory State]")
    ae.log_memory_stats("Initial state (before all tests)")
    mem_initial = ae.get_memory_stats()
    print(f"  Handles: {mem_initial.handle_count}")
    print(f"  Size:    {mem_initial.total_size} bytes ({mem_initial.total_size / 1024:.2f} KB)")

    # test_runnerモジュールをインポート
    try:
        import test_runner
        print("\n[Running Full Test Suite]")
        print("This may take several minutes...")

        # 全テストを実行
        result = test_runner.run_all_tests()

        # テスト結果サマリー
        total_passed = result.get("total_passed", 0)
        total_failed = result.get("total_failed", 0)
        total_tests = total_passed + total_failed

        print(f"\n[Test Results Summary]")
        print(f"  Total:  {total_tests}")
        print(f"  Passed: {total_passed} ({total_passed * 100.0 / total_tests:.1f}%)")
        print(f"  Failed: {total_failed} ({total_failed * 100.0 / total_tests:.1f}%)")

    except Exception as e:
        print(f"\n[Error]")
        print(f"Failed to run tests: {e}")
        import traceback
        traceback.print_exc()
        return False

    # 最終メモリ状態を記録
    print("\n[Final Memory State]")
    ae.log_memory_stats("Final state (after all tests)")
    mem_final = ae.get_memory_stats()
    print(f"  Handles: {mem_final.handle_count}")
    print(f"  Size:    {mem_final.total_size} bytes ({mem_final.total_size / 1024:.2f} KB)")

    # メモリリーク分析
    print("\n[Memory Leak Analysis]")
    print("-" * 80)

    mem_diff_handles = mem_final.handle_count - mem_initial.handle_count
    mem_diff_size = mem_final.total_size - mem_initial.total_size

    print(f"  Handle Growth: {mem_diff_handles:+d} handles")
    print(f"  Size Growth:   {mem_diff_size:+d} bytes ({mem_diff_size / 1024:+.2f} KB)")

    # 詳細ログ出力
    ae.log_memory_leak_details(mem_initial, mem_final, "Full Test Suite")

    # メモリリーク判定
    # 許容範囲: ハンドル数の増加が10以下、サイズ増加が100KB以下
    TOLERANCE_HANDLES = 10
    TOLERANCE_SIZE = 100 * 1024  # 100KB

    has_leak = ae.check_memory_leak(
        mem_initial,
        mem_final,
        tolerance_handles=TOLERANCE_HANDLES,
        tolerance_size=TOLERANCE_SIZE
    )

    print("\n[Memory Leak Verdict]")
    print("-" * 80)
    if has_leak:
        print("  ⚠️  MEMORY LEAK DETECTED")
        print(f"  Handle growth ({mem_diff_handles}) exceeds tolerance ({TOLERANCE_HANDLES})")
        print(f"  OR size growth ({mem_diff_size / 1024:.2f} KB) exceeds tolerance ({TOLERANCE_SIZE / 1024:.0f} KB)")
        verdict = False
    else:
        print("  ✅ NO SIGNIFICANT MEMORY LEAK")
        print(f"  Memory growth is within acceptable range")
        print(f"  (Tolerance: {TOLERANCE_HANDLES} handles, {TOLERANCE_SIZE / 1024:.0f} KB)")
        verdict = True

    # パフォーマンス統計
    if mem_initial.handle_count > 0:
        growth_rate = (mem_diff_handles * 100.0) / mem_initial.handle_count
        print(f"\n  Handle Growth Rate: {growth_rate:+.2f}%")

    if mem_initial.total_size > 0:
        size_growth_rate = (mem_diff_size * 100.0) / mem_initial.total_size
        print(f"  Size Growth Rate:   {size_growth_rate:+.2f}%")

    print("\n" + "=" * 80)
    if verdict and total_failed == 0:
        print("SUCCESS: All tests passed and no memory leaks detected!")
    elif verdict and total_failed > 0:
        print(f"PARTIAL SUCCESS: {total_failed} test(s) failed but no memory leaks")
    elif not verdict and total_failed == 0:
        print("WARNING: All tests passed but memory leak detected")
    else:
        print(f"FAILURE: {total_failed} test(s) failed and memory leak detected")
    print("=" * 80)

    return verdict and (total_failed == 0)

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
