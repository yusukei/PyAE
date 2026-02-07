"""
test_shape_layer_export_import.py
Shape Layer の Export/Import を検証するテストスクリプト

Phase 7.2: Export/Import 検証
"""

import ae
import json
import os


def create_test_shape_layers(comp):
    """包括的な Shape Layer テストケースを作成"""
    print("\n" + "=" * 80)
    print("Creating Shape Layer Test Cases")
    print("=" * 80)

    # 注: After Effects では Shape Layer の作成後、手動でシェイプを追加する必要があります
    # このスクリプトは Shape Layer の「枠」を作成します

    # Test 1: 基本シェイプ（Rectangle）
    print("\n1. Creating Rectangle Shape Layer...")
    rect_layer = comp.create_shape_layer("Rectangle Shape")
    print("   -> Created. Please add a Rectangle shape manually.")

    # Test 2: 基本シェイプ（Ellipse）
    print("\n2. Creating Ellipse Shape Layer...")
    ellipse_layer = comp.create_shape_layer("Ellipse Shape")
    print("   -> Created. Please add an Ellipse shape manually.")

    # Test 3: カスタムパス
    print("\n3. Creating Custom Path Shape Layer...")
    path_layer = comp.create_shape_layer("Custom Path Shape")
    print("   -> Created. Please draw a custom path with pen tool.")

    # Test 4: Stroke + Fill
    print("\n4. Creating Stroke+Fill Shape Layer...")
    stroke_fill_layer = comp.create_shape_layer("Stroke+Fill Shape")
    print("   -> Created. Please add a shape with Stroke and Fill.")

    # Test 5: Gradient Fill
    print("\n5. Creating Gradient Fill Shape Layer...")
    gradient_layer = comp.create_shape_layer("Gradient Fill Shape")
    print("   -> Created. Please add a shape with Gradient Fill.")

    # Test 6: Merge Paths
    print("\n6. Creating Merge Paths Shape Layer...")
    merge_layer = comp.create_shape_layer("Merge Paths Shape")
    print("   -> Created. Please add shapes with Merge Paths operation.")

    # Test 7: Repeater
    print("\n7. Creating Repeater Shape Layer...")
    repeater_layer = comp.create_shape_layer("Repeater Shape")
    print("   -> Created. Please add a shape with Repeater.")

    # Test 8: 入れ子グループ
    print("\n8. Creating Nested Groups Shape Layer...")
    nested_layer = comp.create_shape_layer("Nested Groups Shape")
    print("   -> Created. Please create nested shape groups.")

    print("\n" + "=" * 80)
    print("Shape Layer Creation Complete")
    print("=" * 80)
    print("\nIMPORTANT: Add shapes manually in After Effects, then run export_scene.py")


def analyze_exported_json(json_path):
    """Export された JSON を分析"""
    print("\n" + "=" * 80)
    print("Analyzing Exported JSON")
    print("=" * 80)

    if not os.path.exists(json_path):
        print(f"\nError: JSON file not found: {json_path}")
        print("Please run export_scene.py first.")
        return

    try:
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except Exception as e:
        print(f"\nError loading JSON: {e}")
        return

    # コンポジションを検索
    project = data.get("project", {})
    items = project.get("items", [])

    shape_comp = None
    for item in items:
        if item.get("type") == "composition" and "Shape" in item.get("name", ""):
            shape_comp = item
            break

    if not shape_comp:
        print("\nNo Shape Layer composition found in JSON.")
        return

    print(f"\nFound composition: {shape_comp['name']}")

    # Shape Layer を検索
    layers = shape_comp.get("layers", [])
    shape_layers = [l for l in layers if l.get("type") == "Shape"]

    print(f"\nFound {len(shape_layers)} Shape Layers")

    # 各 Shape Layer を分析
    for i, layer in enumerate(shape_layers):
        print(f"\n--- Shape Layer {i+1}: {layer.get('name')} ---")
        analyze_shape_layer_data(layer)


def analyze_shape_layer_data(layer_data):
    """Shape Layer のデータ構造を分析"""
    properties = layer_data.get("properties", {})

    # "Contents" または "ADBE Root Vectors Group" を検索
    contents = None
    for key, value in properties.items():
        if isinstance(value, dict):
            match_name = value.get("match_name", "")
            if "Root Vectors" in match_name or "Contents" in key:
                contents = value
                print(f"  Found contents: {key} ({match_name})")
                break

    if not contents:
        print("  No shape contents found!")
        return

    # 子要素を分析
    children = contents.get("children", {})
    print(f"  Number of child groups: {len(children)}")

    for child_name, child_data in children.items():
        if isinstance(child_data, dict):
            match_name = child_data.get("match_name", "")
            child_type = child_data.get("type", "")
            print(f"    - {child_name} ({match_name}, type: {child_type})")

            # さらに詳細を表示
            if child_type == "group":
                sub_children = child_data.get("children", {})
                print(f"      └─ {len(sub_children)} sub-properties")
                for sub_name, sub_data in list(sub_children.items())[:5]:  # 最初の5つのみ
                    if isinstance(sub_data, dict):
                        sub_match = sub_data.get("match_name", "")
                        sub_type = sub_data.get("type", "")
                        print(f"         - {sub_name} ({sub_match}, {sub_type})")


def check_import_results(original_comp, imported_comp):
    """Import 結果を比較"""
    print("\n" + "=" * 80)
    print("Comparing Original vs Imported")
    print("=" * 80)

    print(f"\nOriginal Composition: {original_comp.name}")
    print(f"  Layers: {original_comp.num_layers}")

    print(f"\nImported Composition: {imported_comp.name}")
    print(f"  Layers: {imported_comp.num_layers}")

    if original_comp.num_layers != imported_comp.num_layers:
        print("\n⚠️ WARNING: Layer count mismatch!")
        return

    # 各レイヤーを比較
    for i in range(1, original_comp.num_layers + 1):
        orig_layer = original_comp.layer(i)
        imp_layer = imported_comp.layer(i)

        print(f"\n--- Layer {i}: {orig_layer.name} ---")
        print(f"  Original type: {orig_layer.layer_type}")
        print(f"  Imported type: {imp_layer.layer_type}")

        if orig_layer.layer_type != imp_layer.layer_type:
            print("  ⚠️ Type mismatch!")

        # Shape Layer の場合、プロパティ数を比較
        if orig_layer.layer_type == ae.LayerType.Shape:
            try:
                orig_root = orig_layer.properties
                imp_root = imp_layer.properties

                # Contents を検索
                orig_contents = None
                imp_contents = None

                for j in range(1, orig_root.num_properties + 1):
                    prop = orig_root.property(j)
                    if "Root Vectors" in prop.match_name:
                        orig_contents = prop
                        break

                for j in range(1, imp_root.num_properties + 1):
                    prop = imp_root.property(j)
                    if "Root Vectors" in prop.match_name:
                        imp_contents = prop
                        break

                if orig_contents and imp_contents:
                    print(f"  Original contents: {orig_contents.num_properties} groups")
                    print(f"  Imported contents: {imp_contents.num_properties} groups")

                    if orig_contents.num_properties != imp_contents.num_properties:
                        print("  ⚠️ Shape group count mismatch!")
                else:
                    if not orig_contents:
                        print("  ⚠️ Original contents not found!")
                    if not imp_contents:
                        print("  ⚠️ Imported contents not found!")

            except Exception as e:
                print(f"  Error comparing properties: {e}")


def main():
    """メイン処理"""
    print("=" * 80)
    print("PyAE - Shape Layer Export/Import Verification")
    print("Phase 7.2: Export/Import Validation")
    print("=" * 80)

    app = ae.get_application()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("Shape Layer Export Test", 1920, 1080, 1.0, 10.0, 30)

    # テストケース作成
    create_test_shape_layers(comp)

    print("\n" + "=" * 80)
    print("Next Steps:")
    print("=" * 80)
    print("\n1. Manually add shapes to each layer in After Effects:")
    print("   - Rectangle Shape: Add Rectangle")
    print("   - Ellipse Shape: Add Ellipse")
    print("   - Custom Path Shape: Draw custom path")
    print("   - Stroke+Fill Shape: Add shape with both stroke and fill")
    print("   - Gradient Fill Shape: Add shape with gradient")
    print("   - Merge Paths Shape: Add multiple shapes with Merge Paths")
    print("   - Repeater Shape: Add shape with Repeater")
    print("   - Nested Groups Shape: Create nested groups")
    print("\n2. Save the project")
    print("\n3. Run export_scene.py:")
    print("   python PyAE/scripts/export_scene.py")
    print("\n4. Run this script again with --analyze flag:")
    print("   python PyAE/scripts/test_shape_layer_export_import.py --analyze")
    print("\n5. Run import_scene.py:")
    print("   python PyAE/scripts/import_scene.py")
    print("\n6. Compare original and imported projects visually")


if __name__ == "__main__":
    import sys

    if "--analyze" in sys.argv:
        # JSON 分析モード
        json_path = r"d:\works\VTechStudio\AfterFX\export\scene.json"
        analyze_exported_json(json_path)
    elif "--compare" in sys.argv:
        # 比較モード（Import 後に実行）
        app = ae.get_application()
        project = app.project

        # オリジナルとインポートされたコンポジションを検索
        original_comp = None
        imported_comp = None

        for i in range(1, project.num_items + 1):
            item = project.item(i)
            if hasattr(item, 'num_layers'):
                if "Export Test" in item.name and "Imported" not in item.name:
                    original_comp = item
                elif "Imported" in item.name:
                    imported_comp = item

        if original_comp and imported_comp:
            check_import_results(original_comp, imported_comp)
        else:
            print("Error: Could not find original and imported compositions")
            print(f"Original found: {original_comp is not None}")
            print(f"Imported found: {imported_comp is not None}")
    else:
        # テストケース作成モード
        main()
