# create_comp.py
# PyAE サンプルスクリプト
# Author: VTech Studio
# Version: 1.0
# Description: 新しいコンポジションを作成するサンプル

"""
新規コンポジション作成スクリプト

このスクリプトは新しいコンポジションを作成する方法を示します。
"""

import ae

def create_hd_comp(name="New Comp", duration=10.0, frame_rate=30.0):
    """
    HD（1920x1080）コンポジションを作成します。

    Args:
        name: コンポジション名
        duration: 長さ（秒）
        frame_rate: フレームレート

    Returns:
        作成されたコンポジション
    """
    comp = ae.Comp.create(
        name=name,
        width=1920,
        height=1080,
        pixel_aspect=1.0,
        duration=duration,
        frame_rate=frame_rate
    )

    ae.log_info(f"Created comp: {comp.name}")
    return comp

def create_4k_comp(name="4K Comp", duration=10.0, frame_rate=24.0):
    """
    4K（3840x2160）コンポジションを作成します。
    """
    comp = ae.Comp.create(
        name=name,
        width=3840,
        height=2160,
        pixel_aspect=1.0,
        duration=duration,
        frame_rate=frame_rate
    )

    ae.log_info(f"Created 4K comp: {comp.name}")
    return comp

def main():
    if not ae.Project.is_open():
        ae.log_error("Please open a project first")
        return

    # HDコンポジションを作成
    hd_comp = create_hd_comp("PyAE Test Comp", 5.0, 30.0)

    ae.log_info(f"Comp created successfully!")
    ae.log_info(f"  Name: {hd_comp.name}")
    ae.log_info(f"  Size: {hd_comp.width}x{hd_comp.height}")
    ae.log_info(f"  Duration: {hd_comp.duration}s")

if __name__ == "__main__":
    main()
