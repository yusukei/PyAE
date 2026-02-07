# debug_threeDPerChar.py
# threeDPerChar のエクスポート問題をデバッグ

import ae
import os

LOG_PATH = os.path.join(os.path.expanduser("~"), "Documents", "debug_3dpc.txt")

def log(msg):
    ae.log_info(f"[DEBUG] {msg}")
    with open(LOG_PATH, "a", encoding="utf-8") as f:
        f.write(f"{msg}\n")


def run_debug():
    # ログファイルを初期化
    with open(LOG_PATH, "w", encoding="utf-8") as f:
        f.write("=== threeDPerChar Debug ===\n")

    project = ae.get_project()
    if not project:
        log("ERROR: No project open")
        return "ERROR"

    # テスト用コンポジション作成
    comp = project.create_comp("Debug_3DPC_Test", 1920, 1080, 1.0, 10.0, 30.0)
    log(f"Comp created: {comp.name}")

    # テキストレイヤー作成
    text_layer = comp.add_text("Test Text")
    text_layer.name = "Debug_Text"
    log(f"Text layer created: {text_layer.name}")

    comp_name = comp.name
    layer_index = text_layer.index
    log(f"  PyAE layer.index = {layer_index}")

    layer_index_1based = text_layer.index + 1
    log(f"  1-based layer index = {layer_index_1based}")

    # 設定前の threeDPerChar を確認
    check_script = f'''
    (function() {{
        for (var i = 1; i <= app.project.numItems; i++) {{
            if (app.project.item(i).name === "{comp_name}") {{
                var layer = app.project.item(i).layer({layer_index_1based});
                if (layer) {{
                    return "BEFORE: threeDPerChar=" + layer.threeDPerChar + ", layerName=" + layer.name;
                }}
                return "BEFORE: Layer not found at index {layer_index_1based}";
            }}
        }}
        return "BEFORE: Comp not found";
    }})();
    '''
    result = ae.execute_extendscript(check_script)
    log(f"  {result}")

    # threeDPerChar を true に設定
    set_script = f'''
    (function() {{
        for (var i = 1; i <= app.project.numItems; i++) {{
            if (app.project.item(i).name === "{comp_name}") {{
                var layer = app.project.item(i).layer({layer_index_1based});
                if (layer) {{
                    layer.threeDPerChar = true;
                    return "SET: OK, threeDPerChar=" + layer.threeDPerChar;
                }}
                return "SET: Layer not found";
            }}
        }}
        return "SET: Comp not found";
    }})();
    '''
    result = ae.execute_extendscript(set_script)
    log(f"  {result}")

    # 設定後の threeDPerChar を確認
    check_script2 = f'''
    (function() {{
        for (var i = 1; i <= app.project.numItems; i++) {{
            if (app.project.item(i).name === "{comp_name}") {{
                var layer = app.project.item(i).layer({layer_index_1based});
                if (layer) {{
                    return "AFTER: threeDPerChar=" + layer.threeDPerChar + ", layerName=" + layer.name;
                }}
                return "AFTER: Layer not found";
            }}
        }}
        return "AFTER: Comp not found";
    }})();
    '''
    result = ae.execute_extendscript(check_script2)
    log(f"  {result}")

    # AE状態を更新
    ae.refresh()

    # 更新後に再度確認
    result = ae.execute_extendscript(check_script2)
    log(f"  After refresh: {result}")

    # エクスポートで使用するコード（export_scene.py と同じ）をテスト
    log("Testing export code path:")

    # export_scene.py と同じ方法で取得
    export_script = f'''
    (function() {{
        var comp = null;
        for (var i = 1; i <= app.project.numItems; i++) {{
            if (app.project.item(i).name === "{comp_name}") {{
                comp = app.project.item(i);
                break;
            }}
        }}
        if (comp && comp.layer({layer_index_1based})) {{
            return comp.layer({layer_index_1based}).threeDPerChar ? "true" : "false";
        }}
        return "false";
    }})();
    '''
    result = ae.execute_extendscript(export_script)
    log(f"  Export-style result: '{result}'")
    log(f"  Parsed as: {result.strip().lower() == 'true'}")

    # クリーンアップ
    try:
        for item in project.items:
            if hasattr(item, 'name') and item.name == "Debug_3DPC_Test":
                item.delete()
                log("Cleanup: Comp deleted")
                break
    except Exception as e:
        log(f"Cleanup error: {e}")

    log("Debug complete")
    return "OK"


if __name__ == "__main__":
    run_debug()
