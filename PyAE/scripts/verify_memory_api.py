#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Verify Memory Diagnostics API
メモリ診断APIが正しく利用可能か検証

Usage:
  python verify_memory_api.py
"""

def verify_memory_api():
    """メモリ診断APIが利用可能か確認"""
    print("Verifying Memory Diagnostics API...")

    try:
        import ae
        print("✓ ae module imported")

        # Check if memory diagnostics functions exist
        functions = [
            'get_memory_stats',
            'log_memory_stats',
            'check_memory_leak',
            'log_memory_leak_details',
            'enable_memory_reporting',
        ]

        for func_name in functions:
            if hasattr(ae, func_name):
                print(f"✓ ae.{func_name} exists")
            else:
                print(f"✗ ae.{func_name} NOT FOUND")
                return False

        # Check if MemStats class exists
        if hasattr(ae, 'MemStats'):
            print("✓ ae.MemStats class exists")

            # Check MemStats attributes
            stats = ae.get_memory_stats()
            if hasattr(stats, 'handle_count'):
                print(f"✓ MemStats.handle_count = {stats.handle_count}")
            else:
                print("✗ MemStats.handle_count NOT FOUND")
                return False

            if hasattr(stats, 'total_size'):
                print(f"✓ MemStats.total_size = {stats.total_size}")
            else:
                print("✗ MemStats.total_size NOT FOUND")
                return False
        else:
            print("✗ ae.MemStats class NOT FOUND")
            return False

        # Try basic functionality
        print("\nTesting basic functionality...")
        ae.log_memory_stats("Verification test")
        print("✓ log_memory_stats() works")

        before = ae.get_memory_stats()
        after = ae.get_memory_stats()
        has_leak = ae.check_memory_leak(before, after)
        print(f"✓ check_memory_leak() works (result: {has_leak})")

        ae.log_memory_leak_details(before, after, "Verification")
        print("✓ log_memory_leak_details() works")

        ae.enable_memory_reporting(False)
        print("✓ enable_memory_reporting() works")

        print("\n" + "=" * 50)
        print("SUCCESS: All Memory Diagnostics APIs are available!")
        print("=" * 50)
        return True

    except Exception as e:
        print(f"\n✗ ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    import sys
    success = verify_memory_api()
    sys.exit(0 if success else 1)
