# check_comments.py
# すべてのレイヤーのコメントを確認するスクリプト

import ae

def check_all_comments():
    """すべてのコンポジションのすべてのレイヤーのコメントをチェック"""
    print("=" * 80)
    print("全レイヤーのコメントチェック")
    print("=" * 80)

    project = ae.get_project()
    if not project or not project.valid:
        print("エラー: プロジェクトが無効です")
        return

    found_comments = []

    # すべてのアイテムをチェック
    for item in project.items:
        if item.type != ae.ItemType.Comp:
            continue

        comp = item
        print(f"\nコンポジション: {comp.name} ({comp.num_layers} layers)")

        for layer_idx in range(1, comp.num_layers + 1):
            try:
                layer = comp.layer(layer_idx)

                # ソースアイテムのコメントをチェック
                source_comment = layer.comment if hasattr(layer, 'comment') else ""

                # マーカーコメントをチェック（in_pointで）
                marker_comment = ""
                try:
                    marker_comment = layer.get_marker_comment(layer.in_point)
                except:
                    pass

                if source_comment or marker_comment:
                    info = {
                        'comp': comp.name,
                        'layer': layer.name,
                        'index': layer_idx,
                        'source_comment': source_comment,
                        'marker_comment': marker_comment
                    }
                    found_comments.append(info)
                    print(f"  [{layer_idx}] {layer.name}")
                    if source_comment:
                        print(f"      ソースコメント: {source_comment}")
                    if marker_comment:
                        print(f"      マーカーコメント: {marker_comment}")
            except Exception as e:
                print(f"  エラー (layer {layer_idx}): {e}")

    print("\n" + "=" * 80)
    print(f"コメント付きレイヤー総数: {len(found_comments)}")
    print("=" * 80)

    # 「タイムリマップ」または「矢印」を含むコメントを探す
    target_comments = [c for c in found_comments
                      if 'タイムリマップ' in c['source_comment'] or
                         'タイムリマップ' in c['marker_comment'] or
                         '矢印' in c['source_comment'] or
                         '矢印' in c['marker_comment']]

    if target_comments:
        print("\n【ターゲットコメント発見】")
        for c in target_comments:
            print(f"\nコンポ: {c['comp']}")
            print(f"  レイヤー: {c['layer']} (index: {c['index']})")
            if c['source_comment']:
                print(f"  ソースコメント: {c['source_comment']}")
            if c['marker_comment']:
                print(f"  マーカーコメント: {c['marker_comment']}")
    else:
        print("\n【注意】「タイムリマップ」または「矢印」を含むコメントが見つかりませんでした")
        print("元のプロジェクトにこのコメントが設定されているか確認してください")

if __name__ == "__main__":
    check_all_comments()
