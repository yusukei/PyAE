"""
シェイプグループの名前設定エクスポート/インポートテスト
shape groupの名前が正しくエクスポート/インポートされるか確認
"""
import ae
import json
import os

def main():
    project = ae.get_project()
    if not project:
        print("No project open")
        return

    # Phase 1: エクスポート用シーンを作成
    print("\n=== Phase 1: Creating Test Scene ===")
    comp = project.create_comp("Test Shape Group Export", 1920, 1080, 1.0, 10.0, 30.0)

    # シェイプレイヤーを作成
    layer = comp.add_shape_layer("Shape Layer 1")
    print(f"Created shape layer: {layer.name}")

    # Contents プロパティを取得
    root = layer.properties
    contents = root.property("ADBE Root Vectors Group")
    if not contents:
        print("[ERROR] Contents property not found")
        return

    print(f"Contents: {contents.name}, type={contents.grouping_type}")

    # Vector Groupを2つ追加して名前を設定
    print("\n=== Adding Vector Groups ===")

    # Group 1: "Custom Group A"
    group1 = contents.add_stream("ADBE Vector Group")
    print(f"Added group1: {group1.name}")
    group1.set_name("Custom Group A")
    print(f"[OK] Set name to: {group1.name}")

    # Group 2: "Custom Group B"
    group2 = contents.add_stream("ADBE Vector Group")
    print(f"Added group2: {group2.name}")
    group2.set_name("Custom Group B")
    print(f"[OK] Set name to: {group2.name}")

    # Phase 2: エクスポート
    print("\n=== Phase 2: Exporting Scene ===")
    export_path = "d:/works/VTechStudio/AfterFX/test_shape_group.json"

    try:
        # export_scene.py の関数を使用
        import sys
        sys.path.insert(0, os.path.dirname(__file__))
        from export_scene import export_project_to_dict

        project_data = export_project_to_dict(project)

        with open(export_path, 'w', encoding='utf-8') as f:
            json.dump(project_data, f, indent=2, ensure_ascii=False)

        print(f"[OK] Exported to: {export_path}")

        # エクスポートされたJSONを確認
        with open(export_path, 'r', encoding='utf-8') as f:
            data = json.load(f)

        # Shape Layerのプロパティを確認
        items = data.get("items", [])
        comp_item = None
        for item in items:
            if item.get("name") == "Test Shape Group Export":
                comp_item = item
                break

        if comp_item:
            layers = comp_item.get("comp_data", {}).get("layers", [])
            if layers:
                layer_data = layers[0]
                properties = layer_data.get("properties", {})

                print("\n=== Checking Exported JSON ===")
                # Contents配下を確認
                for prop_name, prop_data in properties.items():
                    if prop_data.get("match_name") == "ADBE Root Vectors Group":
                        print(f"Found Contents: {prop_name}")
                        children = prop_data.get("children", {})
                        print(f"Children: {list(children.keys())}")

                        # 各子グループの名前を確認
                        for child_name, child_data in children.items():
                            match_name = child_data.get("match_name")
                            print(f"  - Child: '{child_name}' (match_name: {match_name})")

    except Exception as e:
        print(f"[ERROR] Export failed: {e}")
        import traceback
        traceback.print_exc()
        return

    # Phase 3: 新規プロジェクトを作成してインポート
    print("\n=== Phase 3: Creating New Project for Import ===")

    # 現在のプロジェクトを閉じて新規プロジェクトを作成
    # 注意: ae.new_project() がない場合は手動で新規プロジェクトを作成
    print("[INFO] Please create a new project manually, then run import_scene.py")
    print(f"[INFO] Import file: {export_path}")

if __name__ == "__main__":
    main()
