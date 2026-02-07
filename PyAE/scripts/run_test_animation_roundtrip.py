# run_test_animation_roundtrip.py
# PyAE Animation Roundtrip Test Runner

import sys
import os

# Add parent directory to path
script_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(script_dir)
tests_dir = os.path.join(parent_dir, "tests")

if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
if tests_dir not in sys.path:
    sys.path.insert(0, tests_dir)

# Import and run the test
from animation import test_animation_roundtrip

print("\n" + "=" * 70)
print("PyAE Animation Roundtrip Test")
print("=" * 70 + "\n")

results = test_animation_roundtrip.run()

# Print summary
passed = sum(1 for r in results if r.passed)
failed = len(results) - passed

print(f"\n{'=' * 70}")
print(f"Results: {passed} passed, {failed} failed out of {len(results)} tests")
print("=" * 70)

if failed == 0:
    print("✓ All tests PASSED!")
else:
    print(f"✗ {failed} test(s) FAILED")
