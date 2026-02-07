"""
analyze_shape_layer.py
Shape Layer のプロパティ構造を分析するスクリプト

Phase 7.1: Shape Layer プロパティの分析
"""

import ae
import json


def analyze_property(prop, depth=0, max_depth=10):
    """プロパティを再帰的に分析"""
    if depth > max_depth:
        return {"error": "Max depth reached"}

    indent = "  " * depth
    result = {
        "name": prop.name,
        "match_name": prop.match_name,
        "type": None,
        "value": None,
        "num_keys": 0,
        "expression": None,
    }

    try:
        # プロパティタイプを判定
        if prop.is_group:
            result["type"] = "group"
            result["num_properties"] = prop.num_properties
            result["children"] = []

            print(f"{indent}[GROUP] {prop.name} ({prop.match_name})")
            print(f"{indent}  └─ Properties: {prop.num_properties}")

            # 子プロパティを再帰的に分析
            for i in range(1, prop.num_properties + 1):
                try:
                    child = prop.property(i)
                    child_result = analyze_property(child, depth + 1, max_depth)
                    result["children"].append(child_result)
                except Exception as e:
                    print(f"{indent}  └─ Error accessing property {i}: {e}")

        else:
            # 値を持つプロパティ
            result["type"] = "value"
            try:
                value = prop.value
                result["value"] = str(value) if value is not None else None
                print(f"{indent}[VALUE] {prop.name} ({prop.match_name})")
                print(f"{indent}  └─ Value: {value}")
            except Exception as e:
                print(f"{indent}[VALUE] {prop.name} ({prop.match_name})")
                print(f"{indent}  └─ Error getting value: {e}")

            # キーフレーム数
            try:
                result["num_keys"] = prop.num_keys
                if prop.num_keys > 0:
                    print(f"{indent}  └─ Keyframes: {prop.num_keys}")
            except:
                pass

            # Expression
            try:
                if prop.expression:
                    result["expression"] = prop.expression
                    print(f"{indent}  └─ Expression: {prop.expression}")
            except:
                pass

    except Exception as e:
        result["error"] = str(e)
        print(f"{indent}[ERROR] {prop.name}: {e}")

    return result


def analyze_shape_layer(layer):
    """Shape Layer の完全な分析"""
    print("=" * 80)
    print(f"Analyzing Shape Layer: {layer.name}")
    print("=" * 80)

    result = {
        "layer_name": layer.name,
        "layer_index": layer.index,
        "properties": {}
    }

    try:
        root = layer.properties
        print(f"\nRoot Properties: {root.num_properties}")

        # ルートプロパティをすべて分析
        for i in range(1, root.num_properties + 1):
            try:
                prop = root.property(i)
                print(f"\n--- Root Property {i}: {prop.name} ---")
                prop_result = analyze_property(prop, depth=0)
                result["properties"][prop.name] = prop_result
            except Exception as e:
                print(f"Error accessing root property {i}: {e}")

    except Exception as e:
        print(f"Error analyzing layer: {e}")
        result["error"] = str(e)

    return result


def create_test_shapes(comp):
    """テスト用のシェイプを作成"""
    print("\n" + "=" * 80)
    print("Creating Test Shapes")
    print("=" * 80)

    shapes = []

    # 1. Rectangle
    print("\n1. Creating Rectangle Shape Layer...")
    rect_layer = comp.create_shape_layer("Rectangle Shape")
    shapes.append(("Rectangle", rect_layer))

    # 2. Ellipse
    print("2. Creating Ellipse Shape Layer...")
    ellipse_layer = comp.create_shape_layer("Ellipse Shape")
    shapes.append(("Ellipse", ellipse_layer))

    # 3. Custom Path (後で手動で追加)
    print("3. Creating Custom Path Shape Layer...")
    custom_layer = comp.create_shape_layer("Custom Path Shape")
    shapes.append(("Custom Path", custom_layer))

    return shapes


def main():
    """メイン処理"""
    print("=" * 80)
    print("PyAE - Shape Layer Property Analysis")
    print("Phase 7.1: Shape Layer Property Structure Analysis")
    print("=" * 80)

    app = ae.get_application()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("Shape Layer Analysis", 1920, 1080, 1.0, 5.0, 30)

    # テストシェイプを作成
    shapes = create_test_shapes(comp)

    print("\n" + "=" * 80)
    print("IMPORTANT: Please add the following shapes manually in After Effects:")
    print("=" * 80)
    print("1. Rectangle Shape: Add a Rectangle shape")
    print("2. Ellipse Shape: Add an Ellipse shape")
    print("3. Custom Path Shape: Draw a custom path with pen tool")
    print("\nAfter adding shapes, run this script again to analyze them.")
    print("=" * 80)

    # 各シェイプレイヤーを分析
    all_results = {}

    for shape_type, layer in shapes:
        print(f"\n\n{'=' * 80}")
        print(f"Analyzing: {shape_type}")
        print(f"{'=' * 80}")

        result = analyze_shape_layer(layer)
        all_results[shape_type] = result

    # 結果を JSON ファイルに保存
    output_file = r"d:\works\VTechStudio\AfterFX\logs\shape_layer_analysis.json"
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(all_results, f, indent=2, ensure_ascii=False)
        print(f"\n\n{'=' * 80}")
        print(f"Analysis results saved to: {output_file}")
        print(f"{'=' * 80}")
    except Exception as e:
        print(f"\nError saving results: {e}")

    print("\n" + "=" * 80)
    print("Analysis Complete")
    print("=" * 80)
    print("\nNext Steps:")
    print("1. Manually add shapes to the layers in After Effects")
    print("2. Run this script again to see the complete property structure")
    print("3. Review the JSON output file for detailed analysis")


if __name__ == "__main__":
    main()
