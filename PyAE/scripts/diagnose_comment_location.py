# diagnose_comment_location.py
# コメントの保存場所を詳細に調査するスクリプト

import ae

def diagnose_layer_comments():
    """レイヤーのコメントがどこに保存されているか診断"""
    print("=" * 80)
    print("レイヤーコメント保存場所診断")
    print("=" * 80)

    comp = ae.get_active_comp()
    if not comp or not comp.valid:
        print("エラー: アクティブなコンポジションがありません")
        print("診断したいコンポジション（例: yazirusi）を開いてから実行してください")
        return

    print(f"\nコンポジション: {comp.name}")
    print(f"レイヤー数: {comp.num_layers}\n")

    for i in range(1, comp.num_layers + 1):
        try:
            layer = comp.layer(i)
            print(f"[{i}] {layer.name}")
            print("-" * 60)

            # 1. ソースアイテムのコメント
            try:
                source_comment = layer.comment
                print(f"  ソースアイテムコメント: '{source_comment}'")
            except Exception as e:
                print(f"  ソースアイテムコメント取得エラー: {e}")

            # 2. マーカーコメント（in_pointで）
            try:
                marker_comment_at_in = layer.get_marker_comment(layer.in_point)
                print(f"  マーカーコメント(in_point={layer.in_point}): '{marker_comment_at_in}'")
            except Exception as e:
                print(f"  マーカーコメント取得エラー: {e}")

            # 3. マーカーコメント（0.0秒で）
            try:
                marker_comment_at_zero = layer.get_marker_comment(0.0)
                print(f"  マーカーコメント(0.0秒): '{marker_comment_at_zero}'")
            except Exception as e:
                print(f"  マーカーコメント(0秒)取得エラー: {e}")

            # 4. レイヤープロパティを試す
            try:
                # "ADBE Marker"プロパティが存在するか
                markers_prop = layer.property("ADBE Marker")
                if markers_prop:
                    print(f"  ✓ ADBE Markerプロパティ: 存在します")
                    # マーカーの数を確認
                    try:
                        num_keys = markers_prop.num_keys if hasattr(markers_prop, 'num_keys') else 0
                        print(f"    キー数: {num_keys}")
                    except:
                        pass
                else:
                    print(f"  ADBE Markerプロパティ: 存在しません")
            except Exception as e:
                print(f"  ADBE Markerプロパティアクセスエラー: {e}")

            # 5. Commentプロパティが存在するか
            try:
                comment_prop = layer.property("Comment")
                if comment_prop:
                    print(f"  ✓ Commentプロパティ: 存在します")
                    try:
                        value = comment_prop.value if hasattr(comment_prop, 'value') else None
                        print(f"    値: '{value}'")
                    except:
                        pass
                else:
                    print(f"  Commentプロパティ: 存在しません")
            except Exception as e:
                print(f"  Commentプロパティアクセスエラー: {e}")

            print()

        except Exception as e:
            print(f"[{i}] エラー: {e}\n")

    print("=" * 80)
    print("診断完了")
    print("=" * 80)
    print("\n【次のステップ】")
    print("1. スクリーンショットでコメントが表示されていたプロジェクトを開く")
    print("2. コメントが表示されているコンポジション（例: yazirusi）をアクティブにする")
    print("3. このスクリプトを再実行して、コメントがどこに保存されているか確認")

if __name__ == "__main__":
    diagnose_layer_comments()
