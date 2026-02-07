# run_test_interpolation.py
# T302 Keyframe Interpolation テスト実行スクリプト
#
# After Effects の Python コンソールで実行:
#   exec(open(r"D:\works\VTechStudio\AfterFX\PyAE\scripts\run_test_interpolation.py").read())

import sys
import os

# パスを設定
try:
    script_dir = os.path.dirname(os.path.abspath(__file__))
except NameError:
    script_dir = r"D:\works\VTechStudio\AfterFX\PyAE\scripts"

tests_dir = os.path.join(os.path.dirname(script_dir), "tests")
if tests_dir not in sys.path:
    sys.path.insert(0, tests_dir)
if script_dir not in sys.path:
    sys.path.insert(0, script_dir)

# テストを実行
from animation import test_keyframe_interpolation

if __name__ == "__main__":
    results = test_keyframe_interpolation.run()
else:
    results = test_keyframe_interpolation.run()

# サマリー
passed = sum(1 for r in results if r.passed)
failed = len(results) - passed
print(f"\nT302 Keyframe Interpolation: {passed} passed, {failed} failed")
