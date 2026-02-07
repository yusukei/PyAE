# add_test_marker.py
# テストマーカーを追加して、タイムラインに表示されるか確認

import ae

def add_test_marker():
    """アクティブコンポジションの最初のレイヤーにテストマーカーを追加"""
    print("=" * 80)
    print("テストマーカー追加")
    print("=" * 80)

    comp = ae.get_active_comp()
    if not comp or not comp.valid:
        print("エラー: アクティブなコンポジションがありません")
        return

    if comp.num_layers < 1:
        print("エラー: レイヤーがありません")
        return

    layer = comp.layer(1)
    print(f"\nコンポジション: {comp.name}")
    print(f"レイヤー: {layer.name}")
    print(f"レイヤー in_point: {layer.in_point}秒")

    # 複数の時間にマーカーを追加
    test_markers = [
        (0.0, "0秒のマーカー"),
        (layer.in_point, "in_pointのマーカー"),
        (1.0, "1秒のマーカー"),
        (2.0, "2秒のマーカー"),
    ]

    print("\nマーカーを追加中...")
    for time, comment in test_markers:
        try:
            layer.add_marker(time, comment)
            print(f"  ✓ マーカー追加成功 (time={time}秒): '{comment}'")
        except Exception as e:
            print(f"  ✗ マーカー追加失敗 (time={time}秒): {e}")

    print("\n追加したマーカーを確認中...")
    for time, expected_comment in test_markers:
        try:
            actual_comment = layer.get_marker_comment(time)
            if actual_comment == expected_comment:
                print(f"  ✓ マーカー確認成功 (time={time}秒): '{actual_comment}'")
            else:
                print(f"  ✗ マーカーコメント不一致 (time={time}秒)")
                print(f"      期待値: '{expected_comment}'")
                print(f"      実際値: '{actual_comment}'")
        except Exception as e:
            print(f"  ✗ マーカー取得失敗 (time={time}秒): {e}")

    print("\n" + "=" * 80)
    print("完了")
    print("=" * 80)
    print("\n【確認事項】")
    print("1. After Effectsのタイムラインを確認してください")
    print("2. タイムラインの左側のカラムヘッダーを右クリック")
    print("3. 'Comment'列が表示されているか確認")
    print("4. レイヤーにマーカーアイコンが表示されているか確認")
    print("=" * 80)

if __name__ == "__main__":
    add_test_marker()
