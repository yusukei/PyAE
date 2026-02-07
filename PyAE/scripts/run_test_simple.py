#!/usr/bin/env python3
# run_test_simple.py
# シンプルなテスト実行ヘルパー

import subprocess
import sys
import os

def main():
    # スクリプトのディレクトリを取得
    script_dir = os.path.dirname(os.path.abspath(__file__))
    pyae_dir = os.path.dirname(script_dir)
    run_tests_bat = os.path.join(pyae_dir, "run_tests.bat")

    print("Running PyAE tests...")
    print(f"Test script: {run_tests_bat}")
    print()

    # After Effectsが起動しているか確認
    result = subprocess.run(
        ["tasklist", "/FI", "IMAGENAME eq AfterFX.exe"],
        capture_output=True,
        text=True
    )

    if "AfterFX.exe" in result.stdout:
        print("WARNING: After Effects is already running!")
        print("Please close After Effects before running tests.")
        return 1

    try:
        # テスト実行（タイムアウト10分）
        process = subprocess.Popen(
            [run_tests_bat],
            cwd=pyae_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            encoding='utf-8',
            errors='replace',
            bufsize=1,
            universal_newlines=True
        )

        # リアルタイムで出力を表示
        for line in process.stdout:
            print(line, end='')

        process.wait(timeout=600)  # 10分タイムアウト

        return process.returncode

    except subprocess.TimeoutExpired:
        print("\nERROR: Test execution timed out after 10 minutes")
        process.kill()
        return 1
    except Exception as e:
        print(f"\nERROR: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
