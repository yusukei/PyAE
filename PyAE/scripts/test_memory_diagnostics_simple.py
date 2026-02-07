#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Simple Memory Diagnostics Test
メモリ診断機能の簡単な動作確認スクリプト

After Effectsのスクリプトエディタで実行:
exec(open(r"D:\works\VTechStudio\AfterFX\PyAE\scripts\test_memory_diagnostics_simple.py").read())
"""

import ae

def test_memory_diagnostics():
    """メモリ診断機能の基本動作を確認"""
    print("=" * 70)
    print("Memory Diagnostics - Simple Test")
    print("=" * 70)

    # 1. メモリ統計を取得
    print("\n[Test 1] Get memory stats")
    stats = ae.get_memory_stats()
    print(f"  Result: {stats}")
    print(f"  Handles: {stats.handle_count}")
    print(f"  Size: {stats.total_size} bytes ({stats.total_size / 1024:.2f} KB)")

    # 2. メモリ統計をログ出力
    print("\n[Test 2] Log memory stats")
    ae.log_memory_stats("Test context")
    print("  OK - Check logs for output")

    # 3. メモリリークチェック
    print("\n[Test 3] Memory leak detection")
    mem_before = ae.get_memory_stats()
    print(f"  Before: {mem_before.handle_count} handles, {mem_before.total_size} bytes")

    # テスト用のコンポジションとレイヤーを作成・削除
    comp = ae.Comp.create("_MemTest", 1920, 1080, 1.0, 5.0, 30.0)
    text = comp.add_text("Test")
    text.delete()

    mem_after = ae.get_memory_stats()
    print(f"  After:  {mem_after.handle_count} handles, {mem_after.total_size} bytes")

    has_leak = ae.check_memory_leak(mem_before, mem_after)
    print(f"  Leak detected: {has_leak}")

    # 詳細をログ出力
    ae.log_memory_leak_details(mem_before, mem_after, "test_leak_detection")

    # クリーンアップ
    proj = ae.Project.get_current()
    for item in proj.items:
        if item.name == "_MemTest":
            item.delete()
            break

    # 4. メモリレポート有効化/無効化
    print("\n[Test 4] Enable/Disable memory reporting")
    ae.enable_memory_reporting(True)
    print("  Enabled memory reporting")
    ae.enable_memory_reporting(False)
    print("  Disabled memory reporting")

    # 5. 複数回実行でのメモリ増加チェック
    print("\n[Test 5] Memory growth over multiple operations")
    initial_stats = ae.get_memory_stats()
    print(f"  Initial: {initial_stats.handle_count} handles")

    for i in range(10):
        comp = ae.Comp.create(f"_Test{i}", 1920, 1080, 1.0, 5.0, 30.0)
        layer = comp.add_text(f"Test {i}")
        layer.delete()

        # アイテムを削除
        proj = ae.Project.get_current()
        for item in proj.items:
            if item.name == f"_Test{i}":
                item.delete()
                break

    final_stats = ae.get_memory_stats()
    print(f"  Final:   {final_stats.handle_count} handles")
    print(f"  Growth:  {final_stats.handle_count - initial_stats.handle_count} handles")

    ae.log_memory_leak_details(initial_stats, final_stats, "10 iterations")

    print("\n" + "=" * 70)
    print("All tests completed!")
    print("=" * 70)

if __name__ == "__main__":
    test_memory_diagnostics()
