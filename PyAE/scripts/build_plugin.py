#!/usr/bin/env python3
# build_plugin.py
# ビルドを実行するヘルパースクリプト

import subprocess
import sys
import os

def main():
    # スクリプトのディレクトリを取得
    script_dir = os.path.dirname(os.path.abspath(__file__))
    pyae_dir = os.path.dirname(script_dir)
    build_bat = os.path.join(pyae_dir, "build.bat")

    # ビルド実行
    print("Building PyAE plugin...")
    print(f"Build script: {build_bat}")

    try:
        result = subprocess.run(
            [build_bat, "--clean"],
            cwd=pyae_dir,
            shell=True,
            capture_output=True,
            text=True,
            encoding='utf-8',
            errors='replace',
            timeout=300
        )

        print(result.stdout)
        if result.stderr:
            print(result.stderr, file=sys.stderr)

        if result.returncode != 0:
            print(f"Build failed with exit code {result.returncode}", file=sys.stderr)
            return result.returncode

        print("\nBuild completed successfully!")
        return 0

    except subprocess.TimeoutExpired:
        print("Build timed out after 300 seconds", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Build failed with error: {e}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())
