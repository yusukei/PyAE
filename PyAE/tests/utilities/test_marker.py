# test_marker.py
# マーカーコメントのテストスクリプト

import ae

def test_marker():
    """マーカー機能をテスト"""
    print("=" * 60)
    print("マーカーテスト開始")
    print("=" * 60)

    # アクティブなコンポジションを取得
    comp = ae.get_active_comp()
    if not comp or not comp.valid:
        print("エラー: アクティブなコンポジションがありません")
        return

    print(f"コンポジション: {comp.name}")
    print(f"レイヤー数: {comp.num_layers}")

    if comp.num_layers == 0:
        print("エラー: レイヤーがありません")
        return

    # 最初のレイヤーを取得
    layer = comp.layer(1)
    print(f"\nレイヤー: {layer.name} (index: {layer.index})")
    print(f"in_point: {layer.in_point}")

    # マーカーを追加
    test_time = layer.in_point
    test_comment = "テストマーカーコメント"

    print(f"\nマーカー追加:")
    print(f"  時間: {test_time}")
    print(f"  コメント: {test_comment}")

    try:
        layer.add_marker(test_time, test_comment)
        print("  ✓ マーカー追加成功")
    except Exception as e:
        print(f"  ✗ マーカー追加エラー: {e}")
        return

    # マーカーコメントを取得
    print(f"\nマーカーコメント取得:")
    try:
        retrieved_comment = layer.get_marker_comment(test_time)
        print(f"  取得したコメント: '{retrieved_comment}'")
        if retrieved_comment == test_comment:
            print("  ✓ コメント一致")
        else:
            print(f"  ✗ コメント不一致 (期待: '{test_comment}')")
    except Exception as e:
        print(f"  ✗ コメント取得エラー: {e}")

    # 追加のマーカーを複数追加
    print(f"\n複数のマーカーを追加:")
    test_markers = [
        (0.5, "0.5秒のマーカー"),
        (1.0, "1秒のマーカー"),
        (2.0, "2秒のマーカー"),
    ]

    for time, comment in test_markers:
        try:
            layer.add_marker(time, comment)
            print(f"  ✓ {time}秒: '{comment}'")
        except Exception as e:
            print(f"  ✗ {time}秒: エラー - {e}")

    print("\n" + "=" * 60)
    print("テスト完了")
    print("=" * 60)
    print("\n【確認方法】")
    print("1. タイムラインパネルを確認")
    print("2. 'Comment' 列が表示されているか確認")
    print("   (表示されていない場合は、パネル下部のアイコンから追加)")
    print("3. レイヤーのマーカー位置にコメントが表示されているか確認")
    print("4. レイヤーを展開して 'Markers' プロパティを確認")

if __name__ == "__main__":
    test_marker()
