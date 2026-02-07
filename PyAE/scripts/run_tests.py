# run_tests.py
# PyAE テスト実行スクリプト
#
# After Effects の Python コンソールで実行:
#   exec(open(r"D:\works\VTechStudio\AfterFX\PyAE\scripts\run_tests.py").read())
#
# または PyAE メニューから「Run Script」で選択して実行

import sys
import os

# パスを設定
# __file__ が定義されていない場合（exec()で実行された場合）の対応
try:
    script_dir = os.path.dirname(os.path.abspath(__file__))
except NameError:
    # exec()で実行された場合、カレントディレクトリを使用
    script_dir = os.getcwd()
    # scriptsディレクトリに移動
    if not os.path.exists(os.path.join(script_dir, "tests")):
        # おそらく PyAE ルートディレクトリにいるので、scripts に移動
        script_dir = os.path.join(script_dir, "scripts")
        if not os.path.exists(script_dir):
            # 最後の手段：絶対パスを使用
            script_dir = r"D:\works\VTechStudio\AfterFX\PyAE\scripts"

if script_dir not in sys.path:
    sys.path.insert(0, script_dir)

# テストを実行
from tests.test_runner import run_all_tests

if __name__ == "__main__":
    results = run_all_tests()

    # 結果を表示
    print(f"\nTotal: {results['total_passed']} passed, {results['total_failed']} failed")
else:
    # exec() で実行された場合もテストを実行
    results = run_all_tests()
    print(f"\nTotal: {results['total_passed']} passed, {results['total_failed']} failed")
