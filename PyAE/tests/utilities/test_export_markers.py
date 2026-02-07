# test_export_markers.py
# マーカーコメントのエクスポートをテスト

import ae

def test_export_markers():
    """マーカーコメントがエクスポートできるかテスト"""
    print("=" * 80)
    print("マーカーコメント エクスポートテスト")
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
            print(f"  in_point: {layer.in_point}秒")

            # ソースアイテムコメント
            try:
                comment = layer.comment
                if comment:
                    print(f"  ✓ ソースコメント: '{comment}'")
                else:
                    print(f"  - ソースコメント: なし")
            except Exception as e:
                print(f"  ✗ ソースコメント取得エラー: {e}")

            # マーカーコメント(in_point)
            try:
                marker_comment = layer.get_marker_comment(layer.in_point)
                if marker_comment:
                    print(f"  ✓ マーカーコメント(in_point={layer.in_point}): '{marker_comment}'")
                else:
                    print(f"  - マーカーコメント(in_point): なし")
            except Exception as e:
                print(f"  ✗ マーカーコメント取得エラー: {e}")

            # マーカーコメント(0.0秒)
            try:
                marker_comment_0 = layer.get_marker_comment(0.0)
                if marker_comment_0:
                    print(f"  ✓ マーカーコメント(0.0秒): '{marker_comment_0}'")
                else:
                    print(f"  - マーカーコメント(0.0秒): なし")
            except Exception as e:
                print(f"  ✗ マーカーコメント(0.0秒)取得エラー: {e}")

            print()

        except Exception as e:
            print(f"[{i}] エラー: {e}\n")

    print("=" * 80)
    print("テスト完了")
    print("=" * 80)
    print("\n【次のステップ】")
    print("1. 元のプロジェクト(コメント付き)を開く")
    print("2. コメントが表示されているコンポジションをアクティブにする")
    print("3. このスクリプトを再実行")
    print("4. マーカーコメントが取得できていればOK")
    print("=" * 80)

if __name__ == "__main__":
    test_export_markers()
