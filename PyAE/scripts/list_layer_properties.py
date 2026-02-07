"""
レイヤーの全プロパティを列挙

選択されたレイヤーの全プロパティを表示して、
レイヤースタイルがどのように存在するかを確認します。
"""

import ae

def list_properties(prop, depth=0):
    """プロパティツリーを再帰的に表示"""
    indent = "  " * depth

    try:
        name = prop.name if hasattr(prop, 'name') else "???"
        match_name = prop.match_name if hasattr(prop, 'match_name') else "???"
        is_group = prop.is_group if hasattr(prop, 'is_group') else False

        print(f"{indent}[{depth}] {name}")
        print(f"{indent}    match_name: {match_name}")
        print(f"{indent}    is_group: {is_group}")

        if is_group:
            num_props = prop.num_properties if hasattr(prop, 'num_properties') else 0
            print(f"{indent}    num_properties: {num_props}")

            # 子プロパティを列挙
            for i in range(1, num_props + 1):
                try:
                    child = prop.property(i)
                    list_properties(child, depth + 1)
                except Exception as e:
                    print(f"{indent}    [ERROR] property({i}): {e}")

    except Exception as e:
        print(f"{indent}[ERROR] {e}")


def main():
    """メイン処理"""
    print("=" * 60)
    print("レイヤープロパティ一覧")
    print("=" * 60)

    # アクティブなコンポジションを取得
    comp = ae.get_active_comp()
    if not comp:
        print("エラー: アクティブなコンポジションがありません")
        return

    # 選択されたレイヤーを取得
    layers = comp.selected_layers
    if not layers or len(layers) == 0:
        print("エラー: レイヤーが選択されていません")
        print("使用方法: レイヤーを選択してからこのスクリプトを実行してください")
        return

    layer = layers[0]
    print(f"\nレイヤー: {layer.name}")
    print(f"インデックス: {layer.index}")
    print(f"レイヤータイプ: {layer.layer_type}")

    print("\n" + "=" * 60)
    print("全プロパティ:")
    print("=" * 60)

    # レイヤーをプロパティグループとして扱う
    try:
        num_props = layer.num_properties if hasattr(layer, 'num_properties') else 0
        print(f"\nトップレベルプロパティ数: {num_props}")

        for i in range(1, num_props + 1):
            try:
                prop = layer.property(i)
                list_properties(prop, 0)
                print()  # 空行
            except Exception as e:
                print(f"[ERROR] property({i}): {e}")
                print()

    except Exception as e:
        print(f"エラー: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
