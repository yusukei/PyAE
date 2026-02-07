#!/usr/bin/env python3
# quick_deploy.py
# ビルド済みプラグインを素早くデプロイ（rebuild-and-deployの簡易版）

import shutil
import os
import sys

def main():
    # パス設定
    script_dir = os.path.dirname(os.path.abspath(__file__))
    pyae_dir = os.path.dirname(script_dir)
    build_dir = os.path.join(pyae_dir, "build", "ae2025", "src", "Debug")
    plugin_dir = r"C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE"

    print("Quick Deploy PyAE Plugin")
    print("=" * 50)
    print(f"Source:      {build_dir}")
    print(f"Destination: {plugin_dir}")
    print()

    # ビルド済みファイルの確認
    pyae_aex = os.path.join(build_dir, "PyAE.aex")
    pyaecore_dll = os.path.join(build_dir, "PyAECore.dll")

    if not os.path.exists(pyae_aex):
        print(f"ERROR: PyAE.aex not found at {pyae_aex}")
        print("Please run build.bat first")
        return 1

    if not os.path.exists(pyaecore_dll):
        print(f"ERROR: PyAECore.dll not found at {pyaecore_dll}")
        print("Please run build.bat first")
        return 1

    # プラグインディレクトリ作成
    if not os.path.exists(plugin_dir):
        print(f"Creating plugin directory: {plugin_dir}")
        os.makedirs(plugin_dir)

    # ファイルコピー
    try:
        print("Copying PyAE.aex...", end=" ")
        shutil.copy2(pyae_aex, plugin_dir)
        print("OK")

        print("Copying PyAECore.dll...", end=" ")
        shutil.copy2(pyaecore_dll, plugin_dir)
        print("OK")

        print()
        print("=" * 50)
        print("Deployment completed successfully!")
        print("=" * 50)
        return 0

    except PermissionError as e:
        print(f"\nERROR: Permission denied - {e}")
        print("Please run this script as Administrator")
        return 1
    except Exception as e:
        print(f"\nERROR: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
