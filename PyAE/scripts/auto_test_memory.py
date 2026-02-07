#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Automatic Memory Diagnostics Test
自動テスト機能を使ってメモリ診断テストを実行
"""

import ae
import sys
import os

# テストディレクトリをパスに追加
test_dir = r"D:\works\VTechStudio\AfterFX\PyAE\tests"
if test_dir not in sys.path:
    sys.path.insert(0, test_dir)

def run_memory_test():
    """メモリ診断テストを実行"""
    print("=" * 70)
    print("Automatic Memory Diagnostics Test")
    print("=" * 70)

    # テスト前のメモリ状態
    print("\n[Initial Memory State]")
    ae.log_memory_stats("Initial state")
    initial_stats = ae.get_memory_stats()
    print(f"Handles: {initial_stats.handle_count}, Size: {initial_stats.total_size} bytes")

    # test_memory_diagnosticsモジュールをインポート
    try:
        import test_memory_diagnostics
        print("\n[Running Memory Diagnostics Tests]")
        results = test_memory_diagnostics.run()

        # 結果表示
        passed = sum(1 for r in results if r.passed)
        failed = len(results) - passed

        print(f"\n[Test Results]")
        print(f"Total:  {len(results)}")
        print(f"Passed: {passed}")
        print(f"Failed: {failed}")

        # 失敗したテストを表示
        if failed > 0:
            print("\n[Failed Tests]")
            for r in results:
                if not r.passed:
                    print(f"  - {r.name}: {r.error}")

    except Exception as e:
        print(f"\n[Error]")
        print(f"Failed to run tests: {e}")
        import traceback
        traceback.print_exc()
        return False

    # テスト後のメモリ状態
    print("\n[Final Memory State]")
    ae.log_memory_stats("Final state")
    final_stats = ae.get_memory_stats()
    print(f"Handles: {final_stats.handle_count}, Size: {final_stats.total_size} bytes")

    # メモリリーク詳細
    print("\n[Memory Leak Analysis]")
    ae.log_memory_leak_details(initial_stats, final_stats, "Memory Diagnostics Tests")

    print("\n" + "=" * 70)
    if failed == 0:
        print("SUCCESS: All tests passed!")
    else:
        print(f"FAILURE: {failed} test(s) failed")
    print("=" * 70)

    return failed == 0

if __name__ == "__main__":
    success = run_memory_test()
    sys.exit(0 if success else 1)
