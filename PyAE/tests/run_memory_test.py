#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Memory Diagnostics Test Runner
メモリ診断機能のテストを実行するスクリプト
"""

import ae
import sys

# テストモジュールをインポート
try:
    from test_memory_diagnostics import run
except ImportError:
    import os
    script_dir = os.path.dirname(os.path.abspath(__file__))
    if script_dir not in sys.path:
        sys.path.insert(0, script_dir)
    from test_memory_diagnostics import run

def main():
    """メモリ診断テストを実行"""
    print("=" * 70)
    print("PyAE Memory Diagnostics Test")
    print("=" * 70)

    # メモリレポートを有効化（デバッグ用）
    # ae.enable_memory_reporting(True)

    # テスト前のメモリ統計
    print("\n[Before Tests]")
    ae.log_memory_stats("Before all tests")
    mem_before = ae.get_memory_stats()

    # テスト実行
    print("\n[Running Tests]")
    results = run()

    # テスト後のメモリ統計
    print("\n[After Tests]")
    ae.log_memory_stats("After all tests")
    mem_after = ae.get_memory_stats()

    # メモリリークをチェック
    print("\n[Memory Leak Check]")
    ae.log_memory_leak_details(mem_before, mem_after, "Memory Diagnostics Tests")

    # メモリレポートを無効化
    # ae.enable_memory_reporting(False)

    # 結果サマリー
    passed = sum(1 for r in results if r.passed)
    failed = len(results) - passed

    print("\n" + "=" * 70)
    print(f"Tests: {len(results)} total, {passed} passed, {failed} failed")
    print("=" * 70)

    return failed == 0

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
