#!/usr/bin/env python3
# deploy_plugin.py
# プラグインをデプロイするヘルパースクリプト

import subprocess
import sys
import os

def main():
    # スクリプトのディレクトリを取得
    script_dir = os.path.dirname(os.path.abspath(__file__))
    pyae_dir = os.path.dirname(script_dir)
    deploy_bat = os.path.join(pyae_dir, "rebuild-and-deploy.bat")

    # デプロイ実行
    print("Deploying PyAE plugin...")
    print(f"Deploy script: {deploy_bat}")

    try:
        result = subprocess.run(
            [deploy_bat],
            cwd=pyae_dir,
            shell=True,
            encoding='utf-8',
            errors='replace',
            timeout=300
        )

        if result.returncode != 0:
            print(f"Deploy failed with exit code {result.returncode}", file=sys.stderr)
            return result.returncode

        print("\nDeploy completed successfully!")
        return 0

    except subprocess.TimeoutExpired:
        print("Deploy timed out after 300 seconds", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Deploy failed with error: {e}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())
