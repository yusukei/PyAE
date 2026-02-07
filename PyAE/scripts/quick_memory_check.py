#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Quick Memory Check
テスト実行前後のメモリ状態を比較
"""

import ae
import sys

def main():
    """クイックメモリチェック"""
    print("=" * 70)
    print("PyAE Quick Memory Check")
    print("=" * 70)

    # 初期メモリ状態
    print("\n[Before]")
    mem_before = ae.get_memory_stats()
    ae.log_memory_stats("Before operations")
    print(f"  Handles: {mem_before.handle_count}")
    print(f"  Size:    {mem_before.total_size} bytes ({mem_before.total_size / 1024:.2f} KB)")

    # 簡単な操作を実行
    print("\n[Operations]")
    proj = ae.get_project()
    comp = proj.create_comp("MemTest", 1920, 1080, 1.0, 10.0, 30.0)

    # レイヤー作成と削除を繰り返す
    for i in range(100):
        layer = comp.add_solid(f"Test_{i}", 100, 100, (1.0, 0.0, 0.0))
        layer.delete()

    print(f"  Created and deleted 100 layers")

    # コンポジションを削除
    comp.delete()
    print(f"  Deleted composition")

    # 最終メモリ状態
    print("\n[After]")
    mem_after = ae.get_memory_stats()
    ae.log_memory_stats("After operations")
    print(f"  Handles: {mem_after.handle_count}")
    print(f"  Size:    {mem_after.total_size} bytes ({mem_after.total_size / 1024:.2f} KB)")

    # メモリ差分
    print("\n[Difference]")
    diff_handles = mem_after.handle_count - mem_before.handle_count
    diff_size = mem_after.total_size - mem_before.total_size
    print(f"  Handles: {diff_handles:+d}")
    print(f"  Size:    {diff_size:+d} bytes ({diff_size / 1024:+.2f} KB)")

    # メモリリーク詳細
    ae.log_memory_leak_details(mem_before, mem_after, "Quick Memory Check")

    # 判定
    has_leak = ae.check_memory_leak(mem_before, mem_after, tolerance_handles=5, tolerance_size=10*1024)

    print("\n[Result]")
    if has_leak:
        print("  ⚠️  Potential memory leak detected")
    else:
        print("  ✅ No significant memory leak")

    print("=" * 70)
    return not has_leak

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
