"""
debug_property_access.py
Shape Layer のプロパティアクセスをデバッグ
"""

import ae
import json
import os

DOCS_PATH = ae.get_documents_folder()
LOG_FILE = os.path.join(DOCS_PATH, "property_debug.txt")

def log(msg):
    ae.log_info(msg)
    with open(LOG_FILE, 'a', encoding='utf-8') as f:
        f.write(msg + '\n')

def debug_layer_properties(layer):
    """レイヤーのプロパティ構造をデバッグ"""
    log(f"\n=== Layer: {layer.name} ===")

    root = layer.properties
    log(f"Root properties: {root}")
    log(f"Root match_name: {root.match_name if hasattr(root, 'match_name') else 'N/A'}")

    # ADBE Root Vectors Group を探す
    contents = root.property("ADBE Root Vectors Group")
    log(f"Contents: {contents}")

    if not contents:
        log("  ERROR: ADBE Root Vectors Group not found")
        return

    log(f"Contents match_name: {contents.match_name}")

    # num_properties を確認
    if hasattr(contents, 'num_properties'):
        num = contents.num_properties
        log(f"Contents num_properties: {num}")

        for i in range(num):
            try:
                prop = contents.property(i)
                if prop:
                    log(f"  [{i}] name: {prop.name}, match_name: {prop.match_name}")

                    # Vector Group の中身を調べる
                    if prop.match_name == "ADBE Vector Group":
                        if hasattr(prop, 'num_properties'):
                            inner_num = prop.num_properties
                            log(f"      Inner properties count: {inner_num}")
                            for j in range(inner_num):
                                try:
                                    inner_prop = prop.property(j)
                                    if inner_prop:
                                        log(f"        [{j}] name: {inner_prop.name}, match_name: {inner_prop.match_name}")

                                        # ADBE Vectors Group の中身をさらに調べる (ここにシェイプがある)
                                        if inner_prop.match_name == "ADBE Vectors Group":
                                            if hasattr(inner_prop, 'num_properties'):
                                                deep_num = inner_prop.num_properties
                                                log(f"          >>> ADBE Vectors Group contents ({deep_num} items):")
                                                for k in range(deep_num):
                                                    try:
                                                        deep_prop = inner_prop.property(k)
                                                        if deep_prop:
                                                            log(f"            [{k}] name: {deep_prop.name}, match_name: {deep_prop.match_name}")
                                                    except Exception as e:
                                                        log(f"            [{k}] Error: {e}")
                                except Exception as e:
                                    log(f"        [{j}] Error: {e}")
                else:
                    log(f"  [{i}] None")
            except Exception as e:
                log(f"  [{i}] Error: {e}")
    else:
        log("  ERROR: num_properties not available")

def main():
    # ログファイルを初期化
    with open(LOG_FILE, 'w', encoding='utf-8') as f:
        f.write("=== Property Debug Log ===\n")

    project = ae.get_project()

    # 既存のテストコンポジションを探す
    test_comp = None
    for item in project.items:
        if item.type == ae.ItemType.Comp and "Gradient_Repeater" in item.name:
            test_comp = item
            log(f"Found test comp: {item.name}")
            break

    if not test_comp:
        log("No test composition found. Creating one...")
        ae.begin_undo_group("Debug Test")

        test_comp = project.create_comp("Debug_Test", 1920, 1080, 1.0, 10.0, 30.0)
        layer = test_comp.add_shape()
        layer.name = "DebugShape"

        # Simple shape を追加
        root = layer.properties
        contents = root.property("ADBE Root Vectors Group")
        if contents and hasattr(contents, 'can_add_stream'):
            if contents.can_add_stream("ADBE Vector Group"):
                group = contents.add_stream("ADBE Vector Group")
                group.set_name("Test Group")

                # Rectangle
                if group.can_add_stream("ADBE Vector Shape - Rect"):
                    rect = group.add_stream("ADBE Vector Shape - Rect")
                    log("Rectangle added")

                # Gradient Fill
                if group.can_add_stream("ADBE Vector Graphic - G-Fill"):
                    gfill = group.add_stream("ADBE Vector Graphic - G-Fill")
                    log("Gradient Fill added")

        ae.end_undo_group()
        ae.refresh()

    # レイヤーをデバッグ
    for i in range(test_comp.num_layers):
        layer = test_comp.layer(i)
        debug_layer_properties(layer)

    log(f"\nLog saved to: {LOG_FILE}")

if __name__ == "__main__":
    main()
