"""
シェイプレイヤーのコンテンツ復元テスト
"""
import ae

def main():
    project = ae.Project.get_current()
    if not project:
        print("No project open")
        return

    # テストコンポジションを作成
    comp = project.create_comp("Test Shape Content", 1920, 1080, 1.0, 10.0, 30.0)

    # シェイプレイヤーを作成
    print("\n=== Creating Shape Layer ===")
    shape_layer = comp.add_shape()
    shape_layer.name = "Test Shape"
    print(f"Created layer: {shape_layer.name}")

    # プロパティツリーを確認
    print("\n=== Layer Properties ===")
    props = shape_layer.properties
    print(f"Root properties: {props}")
    print(f"Is group: {props.is_group}")
    print(f"Num properties: {props.num_properties if hasattr(props, 'num_properties') else 'N/A'}")

    # コンテンツプロパティを取得
    print("\n=== Contents Property ===")
    try:
        contents = props.property("ADBE Root Vectors Group")
        if contents:
            print(f"Contents found: {contents}")
            print(f"Match name: {contents.match_name}")
            print(f"Is group: {contents.is_group}")
            print(f"Num properties: {contents.num_properties if hasattr(contents, 'num_properties') else 'N/A'}")

            # グループタイプを確認
            if hasattr(contents, 'grouping_type'):
                print(f"Grouping type: {contents.grouping_type}")

            # add_streamが使えるか
            if hasattr(contents, 'can_add_stream'):
                print(f"Can add stream (ADBE Vector Group): {contents.can_add_stream('ADBE Vector Group')}")

                # 実際に追加してみる
                print("\n=== Adding Vector Group ===")
                try:
                    group = contents.add_stream("ADBE Vector Group")
                    print(f"[OK] Added vector group: {group}")
                    print(f"Group name: {group.name if hasattr(group, 'name') else 'N/A'}")
                    print(f"Group match_name: {group.match_name}")
                except Exception as e:
                    print(f"[ERROR] Failed to add vector group: {e}")
                    import traceback
                    traceback.print_exc()
        else:
            print("[ERROR] Contents not found")
    except Exception as e:
        print(f"[ERROR] Failed to get contents: {e}")
        import traceback
        traceback.print_exc()

    print("\n=== Test Complete ===")

if __name__ == "__main__":
    main()
