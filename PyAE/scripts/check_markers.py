# check_markers.py
# マーカーの存在を詳細に確認するスクリプト

import ae

def check_markers():
    """すべてのレイヤーのマーカーを詳細にチェック"""
    print("=" * 80)
    print("マーカー詳細チェック")
    print("=" * 80)

    comp = ae.get_active_comp()
    if not comp or not comp.valid:
        print("エラー: アクティブなコンポジションがありません")
        return

    print(f"\nコンポジション: {comp.name}")
    print(f"レイヤー数: {comp.num_layers}\n")

    for i in range(1, comp.num_layers + 1):
        try:
            layer = comp.layer(i)
            print(f"[{i}] {layer.name}")
            print("-" * 60)

            # in_point, 0.0秒, 1.0秒でマーカーをチェック
            test_times = [0.0, layer.in_point, 1.0, 2.0]

            for time in test_times:
                try:
                    marker_comment = layer.get_marker_comment(time)
                    if marker_comment:
                        print(f"  ✓ マーカー発見 (time={time}秒): '{marker_comment}'")
                except Exception as e:
                    pass  # マーカーなしは正常

            # ソースアイテムコメントも確認
            try:
                source_comment = layer.comment
                if source_comment:
                    print(f"  ソースコメント: '{source_comment}'")
            except:
                pass

            print()

        except Exception as e:
            print(f"[{i}] エラー: {e}\n")

    print("=" * 80)
    print("チェック完了")
    print("=" * 80)

if __name__ == "__main__":
    check_markers()
