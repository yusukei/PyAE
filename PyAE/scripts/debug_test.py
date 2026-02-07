#!/usr/bin/env python
# debug_test.py
# Import デバッグスクリプト

import sys
import os

print("=== Python Import Debug ===")
print(f"Python version: {sys.version}")
print(f"Python executable: {sys.executable}")
print()

print("=== sys.path ===")
for i, path in enumerate(sys.path):
    print(f"  [{i}] {path}")
print()

# Test 1: Import ae
print("--- Test 1: Import ae ---")
try:
    import ae
    print("SUCCESS: ae module imported")
    print(f"  ae module: {ae}")
except Exception as e:
    print(f"FAILED: {type(e).__name__}: {e}")
print()

# Test 2: Import test_utils
print("--- Test 2: Import test_utils from tests ---")
try:
    from tests.test_utils import TestSuite
    print("SUCCESS: test_utils imported")
    print(f"  TestSuite: {TestSuite}")
except Exception as e:
    print(f"FAILED: {type(e).__name__}: {e}")
    import traceback
    traceback.print_exc()
print()

# Test 3: Import test_project
print("--- Test 3: Import test_project from tests.core ---")
try:
    from tests.core import test_project
    print("SUCCESS: test_project imported")
    print(f"  test_project: {test_project}")
except Exception as e:
    print(f"FAILED: {type(e).__name__}: {e}")
    import traceback
    traceback.print_exc()
print()

# Test 4: Check if tests directory is a package
print("--- Test 4: Check tests package structure ---")
tests_dir = os.path.join(os.path.dirname(__file__), "..", "tests")
print(f"Tests directory: {tests_dir}")
print(f"  Exists: {os.path.exists(tests_dir)}")
if os.path.exists(tests_dir):
    init_file = os.path.join(tests_dir, "__init__.py")
    print(f"  __init__.py exists: {os.path.exists(init_file)}")
    core_dir = os.path.join(tests_dir, "core")
    print(f"  core/ exists: {os.path.exists(core_dir)}")
    if os.path.exists(core_dir):
        core_init = os.path.join(core_dir, "__init__.py")
        print(f"  core/__init__.py exists: {os.path.exists(core_init)}")
print()

# Test 5: Manual path setup and import
print("--- Test 5: Manual sys.path setup ---")
plugin_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
tests_path = os.path.join(plugin_dir, "tests")
if tests_path not in sys.path:
    sys.path.insert(0, tests_path)
    print(f"Added to sys.path: {tests_path}")

try:
    from test_utils import TestSuite
    print("SUCCESS: test_utils imported after path setup")
except Exception as e:
    print(f"FAILED: {type(e).__name__}: {e}")
print()

# Test 6: Try importing with different approaches
print("--- Test 6: Different import approaches ---")
try:
    # Approach 1: Direct import from current directory
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from test_utils import TestSuite
    print("Approach 1 SUCCESS: Direct import")
except Exception as e:
    print(f"Approach 1 FAILED: {e}")

try:
    # Approach 2: Package import
    import tests.test_utils
    print("Approach 2 SUCCESS: Package import")
except Exception as e:
    print(f"Approach 2 FAILED: {e}")

print()
print("=== Debug Complete ===")
print("'OK'")
