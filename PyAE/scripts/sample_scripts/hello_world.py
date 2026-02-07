# hello_world.py
# PyAE サンプルスクリプト
# Author: VTech Studio
# Version: 1.0
# Description: PyAE の基本的な使用方法を示すサンプルスクリプト

"""
PyAE Hello World スクリプト

このスクリプトは PyAE の基本的な機能をデモンストレーションします。
"""

import ae

def main():
    # PyAE バージョン表示
    ae.log_info(f"PyAE Version: {ae.version()}")

    # プラグインの初期化状態を確認
    if not ae.is_initialized():
        ae.log_error("PyAE is not initialized!")
        return

    # プロジェクトの確認
    if not ae.Project.is_open():
        ae.log_warning("No project is open")
        return

    # 現在のプロジェクトを取得
    project = ae.Project.get_current()
    ae.log_info(f"Project name: {project.name}")
    ae.log_info(f"Number of items: {project.num_items}")

    # アクティブなコンポジションを取得
    comp = ae.Comp.get_active()
    if comp.valid:
        ae.log_info(f"Active comp: {comp.name}")
        ae.log_info(f"  Size: {comp.width}x{comp.height}")
        ae.log_info(f"  Duration: {comp.duration} seconds")
        ae.log_info(f"  Frame rate: {comp.frame_rate} fps")
        ae.log_info(f"  Layers: {comp.num_layers}")
    else:
        ae.log_info("No active composition")

    ae.log_info("Hello World script completed!")

if __name__ == "__main__":
    main()
