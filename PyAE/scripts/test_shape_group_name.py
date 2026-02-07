"""
シェイプグループの名前設定テスト
"""
import ae

def main():
    project = ae.get_project()
    if not project:
        print("No project open")
        return

    # テストコンポジションを作成
    comp = project.create_comp("Test Shape Group Name", 1920, 1080, 1.0, 10.0, 30.0)

    # シェイプレイヤーを作成
    print("\n=== Creating Shape Layer ===")
    shape_layer = comp.add_shape()
    shape_layer.name = "Test Shape"

    # コンテンツプロパティを取得
    contents = shape_layer.properties.property("ADBE Root Vectors Group")

    # Vector Groupを追加
    print("\n=== Adding Vector Group ===")
    group = contents.add_stream("ADBE Vector Group")
    print(f"Added group: {group}")
    print(f"Group name: {group.name}")
    print(f"Group match_name: {group.match_name}")

    # 名前を変更してみる
    print("\n=== Testing Name Change ===")
    new_name = "Custom Group Name"

    # 方法1: nameプロパティに直接代入
    print(f"Method 1: group.name = '{new_name}'")
    try:
        group.name = new_name
        print(f"[OK] Name changed: {group.name}")
    except Exception as e:
        print(f"[ERROR] Failed: {e}")

    # 方法2: set_nameメソッド (Effectと同じ)
    print(f"\nMethod 2: group.set_name('{new_name}')")
    try:
        if hasattr(group, 'set_name'):
            group.set_name(new_name)
            print(f"[OK] Name changed: {group.name}")
        else:
            print(f"[ERROR] set_name() method not found")
    except Exception as e:
        print(f"[ERROR] Failed: {e}")

    # stream_nameプロパティを確認
    print(f"\n=== Checking stream_name ===")
    if hasattr(group, 'stream_name'):
        print(f"stream_name: {group.stream_name}")
    else:
        print("stream_name property not found")

    # 親ストリームから名前を設定 (Effectと同じ方法)
    print(f"\n=== Method 3: Using parent stream ===")
    try:
        # 親ストリーム(Contents)を取得
        parent_stream = contents
        print(f"Parent: {parent_stream.match_name}")

        # インデックスを確認
        if hasattr(group, 'index'):
            group_index = group.index
            print(f"Group index: {group_index}")

            # 親から子要素を取得
            child_stream = parent_stream.property_by_index(group_index)
            if child_stream:
                print(f"Child stream: {child_stream.match_name}")

                # 名前を設定
                if hasattr(child_stream, 'set_name'):
                    child_stream.set_name("Test Name via Parent")
                    print(f"[OK] Name set via parent: {child_stream.name}")
                elif hasattr(parent_stream, 'set_stream_name'):
                    parent_stream.set_stream_name(group_index, "Test Name via set_stream_name")
                    print(f"[OK] Name set via set_stream_name: {group.name}")
                else:
                    print("[INFO] No set_name or set_stream_name method")
    except Exception as e:
        print(f"[ERROR] Failed: {e}")
        import traceback
        traceback.print_exc()

    print("\n=== Test Complete ===")

if __name__ == "__main__":
    main()
