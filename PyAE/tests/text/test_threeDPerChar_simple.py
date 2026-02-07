# test_threeDPerChar_simple.py
# threeDPerChar の簡易テスト

import ae
import os
import json

log_path = os.path.join(ae.get_documents_folder(), "threeDPerChar_simple_test.txt")
log_file = open(log_path, "w", encoding="utf-8")

def log(msg):
    ae.log_info(f"[3DPC_TEST] {msg}")
    log_file.write(msg + "\n")
    log_file.flush()

def run_test():
    log("=== threeDPerChar Simple Test ===")
    log("")

    project = ae.Project.get_current()
    if not project:
        log("ERROR: No project open")
        return "ERROR"

    # テストコンポジション作成
    log("Creating test composition...")
    comp = project.create_comp("ThreeDPerChar_Test", 1920, 1080, 1.0, 5.0, 30.0)
    log(f"  Created: {comp.name}")

    # テキストレイヤー追加
    log("Adding text layer...")
    text_layer = comp.add_text("Test Text")
    text_layer.name = "Test_Text_Layer"
    log(f"  Layer: {text_layer.name}, index: {text_layer.index}")

    # 1. デフォルト状態で Scale を設定（2D モード）
    log("")
    log("--- TEST 1: Scale in 2D mode ---")
    try:
        root = text_layer.properties
        text_props = root.property("ADBE Text Properties")
        animators = text_props.property("ADBE Text Animators")
        animator = animators.add_stream("ADBE Text Animator")
        animator.set_name("Test_Animator")
        log(f"  Animator added: {animator.name}")

        anim_props = animator.property("ADBE Text Animator Properties")
        scale_prop = anim_props.property("ADBE Text Scale 3D")

        target_val = [150.0, 150.0, 100.0]
        scale_prop.value = target_val
        actual_val = list(scale_prop.value)
        log(f"  Set: {target_val}")
        log(f"  Got: {actual_val}")

        if actual_val[2] == 150.0:
            log("  RESULT: Z-axis clamped (as expected in 2D mode)")
        elif actual_val[2] == 100.0:
            log("  RESULT: Z-axis preserved (unexpected!)")

    except Exception as e:
        log(f"  ERROR: {e}")
        import traceback
        log(traceback.format_exc())

    # 2. threeDPerChar = true を設定
    log("")
    log("--- TEST 2: Setting threeDPerChar = true ---")
    try:
        comp_name = comp.name
        layer_index = text_layer.index
        script = f'''
        (function() {{
            var comp = null;
            for (var i = 1; i <= app.project.numItems; i++) {{
                if (app.project.item(i).name === "{comp_name}") {{
                    comp = app.project.item(i);
                    break;
                }}
            }}
            if (comp && comp.layer({layer_index})) {{
                var before = comp.layer({layer_index}).threeDPerChar;
                comp.layer({layer_index}).threeDPerChar = true;
                var after = comp.layer({layer_index}).threeDPerChar;
                return "before=" + before + ", after=" + after;
            }}
            return "ERROR: Layer not found";
        }})();
        '''
        log(f"  Calling execute_extendscript...")
        result = ae.execute_extendscript(script)
        log(f"  Result: {result}")

    except Exception as e:
        log(f"  ERROR: {e}")
        import traceback
        log(traceback.format_exc())

    # 3. 3D モードで Scale を再設定
    log("")
    log("--- TEST 3: Scale in 3D mode ---")
    try:
        target_val = [150.0, 150.0, 100.0]
        scale_prop.value = target_val
        actual_val = list(scale_prop.value)
        log(f"  Set: {target_val}")
        log(f"  Got: {actual_val}")

        if actual_val[2] == 100.0:
            log("  ✅ SUCCESS: Z-axis preserved in 3D mode!")
        else:
            log(f"  ❌ FAIL: Z-axis still clamped to {actual_val[2]}")

    except Exception as e:
        log(f"  ERROR: {e}")
        import traceback
        log(traceback.format_exc())

    # クリーンアップ
    log("")
    log("Cleanup...")
    try:
        comp.delete()
        log("  Test comp deleted")
    except:
        log("  Could not delete test comp")

    log("")
    log("=== Test Complete ===")
    log(f"Log saved to: {log_path}")
    log_file.close()
    return "OK"

if __name__ == "__main__":
    run_test()
