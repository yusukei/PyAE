# animate_layer.py
# PyAE サンプルスクリプト
# Author: VTech Studio
# Version: 1.0
# Description: レイヤーにアニメーションを追加するサンプル

"""
レイヤーアニメーションスクリプト

このスクリプトはレイヤーのプロパティにキーフレームを
追加してアニメーションを作成する方法を示します。
"""

import ae

def animate_position(layer, start_pos, end_pos, start_time, end_time):
    """
    レイヤーの位置をアニメーションします。

    Args:
        layer: アニメーションするレイヤー
        start_pos: 開始位置 (x, y) または (x, y, z)
        end_pos: 終了位置
        start_time: 開始時間（秒）
        end_time: 終了時間（秒）
    """
    # 注: この関数は実装例です
    # 実際のプロパティアクセスはレイヤーからストリームを取得する必要があります
    ae.log_info(f"Animating layer position from {start_pos} to {end_pos}")
    ae.log_info(f"Time range: {start_time}s to {end_time}s")

def animate_opacity(layer, start_opacity, end_opacity, start_time, end_time):
    """
    レイヤーの不透明度をアニメーションします。

    Args:
        layer: アニメーションするレイヤー
        start_opacity: 開始不透明度 (0-100)
        end_opacity: 終了不透明度 (0-100)
        start_time: 開始時間（秒）
        end_time: 終了時間（秒）
    """
    ae.log_info(f"Animating opacity from {start_opacity}% to {end_opacity}%")

def create_fade_in(layer, duration=1.0):
    """
    フェードインアニメーションを作成します。

    Args:
        layer: レイヤー
        duration: フェードの長さ（秒）
    """
    animate_opacity(layer, 0, 100, 0, duration)

def create_fade_out(layer, start_time, duration=1.0):
    """
    フェードアウトアニメーションを作成します。

    Args:
        layer: レイヤー
        start_time: 開始時間（秒）
        duration: フェードの長さ（秒）
    """
    animate_opacity(layer, 100, 0, start_time, start_time + duration)

def main():
    # アクティブなアイテムを取得
    item = ae.Item.get_active()
    if not item.valid:
        ae.log_warning("No item selected")
        return

    ae.log_info(f"Selected item: {item.name}")
    ae.log_info(f"Type: {item.type}")

    # コンポジションの場合のみ処理
    if item.type != ae.ItemType.Comp:
        ae.log_warning("Please select a composition")
        return

    # デモメッセージ
    ae.log_info("Animation demo complete!")
    ae.log_info("This script demonstrates the animation API structure.")

if __name__ == "__main__":
    main()
