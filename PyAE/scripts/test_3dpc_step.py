# test_3dpc_step.py
# threeDPerChar 段階的テスト

import ae
import os

log_path = os.path.join(ae.get_documents_folder(), "3dpc_step_test.txt")
log_file = open(log_path, "w", encoding="utf-8")

def log(msg):
    ae.log_info(f"[3DPC] {msg}")
    log_file.write(msg + "\n")
    log_file.flush()

log("=== threeDPerChar Step Test ===")

# 1. コンポ作成
log("\n1. Creating comp...")
project = ae.get_project()
comp = project.create_comp("StepTest", 1920, 1080, 1.0, 5.0, 30.0)
log(f"   Comp: {comp.name}")

# 2. テキストレイヤー作成
log("\n2. Creating text layer...")
text_layer = comp.add_text("Test")
text_layer.name = "TestLayer"
log(f"   Layer: {text_layer.name}, index: {text_layer.index}")

# 3. 単純な ExtendScript: コンポを探す
log("\n3. Finding comp via ExtendScript...")
try:
    script = f'app.project.item(1).name'
    result = ae.execute_extendscript(script)
    log(f"   First item: {result}")
except Exception as e:
    log(f"   ERROR: {e}")

# 4. IIFE でコンポを探す
log("\n4. IIFE to find comp...")
try:
    comp_name = comp.name
    script = f'''(function() {{
        for (var i = 1; i <= app.project.numItems; i++) {{
            if (app.project.item(i).name === "{comp_name}") {{
                return "Found at index " + i;
            }}
        }}
        return "Not found";
    }})()'''
    log(f"   Script:\n{script}")
    result = ae.execute_extendscript(script)
    log(f"   Result: {result}")
except Exception as e:
    log(f"   ERROR: {e}")

# 5. レイヤーにアクセス
log("\n5. Accessing layer...")
try:
    comp_name = comp.name
    layer_index = text_layer.index + 1  # AE は 1-based index
    script = f'''(function() {{
        for (var i = 1; i <= app.project.numItems; i++) {{
            if (app.project.item(i).name === "{comp_name}") {{
                var c = app.project.item(i);
                var l = c.layer({layer_index});
                return "Layer name: " + l.name;
            }}
        }}
        return "Not found";
    }})()'''
    result = ae.execute_extendscript(script)
    log(f"   Result: {result}")
except Exception as e:
    log(f"   ERROR: {e}")

# 6. threeDPerChar を取得
log("\n6. Getting threeDPerChar...")
try:
    comp_name = comp.name
    layer_index = text_layer.index + 1
    script = f'''(function() {{
        for (var i = 1; i <= app.project.numItems; i++) {{
            if (app.project.item(i).name === "{comp_name}") {{
                var c = app.project.item(i);
                var l = c.layer({layer_index});
                return "threeDPerChar: " + l.threeDPerChar;
            }}
        }}
        return "Not found";
    }})()'''
    result = ae.execute_extendscript(script)
    log(f"   Result: {result}")
except Exception as e:
    log(f"   ERROR: {e}")

# 7. threeDPerChar を設定
log("\n7. Setting threeDPerChar = true...")
try:
    comp_name = comp.name
    layer_index = text_layer.index + 1
    script = f'''(function() {{
        for (var i = 1; i <= app.project.numItems; i++) {{
            if (app.project.item(i).name === "{comp_name}") {{
                var c = app.project.item(i);
                var l = c.layer({layer_index});
                var before = l.threeDPerChar;
                l.threeDPerChar = true;
                var after = l.threeDPerChar;
                return "before=" + before + ", after=" + after;
            }}
        }}
        return "Not found";
    }})()'''
    result = ae.execute_extendscript(script)
    log(f"   Result: {result}")
except Exception as e:
    log(f"   ERROR: {e}")

# クリーンアップ
log("\n8. Cleanup...")
try:
    comp.delete()
    log("   Comp deleted")
except:
    pass

log("\n=== Complete ===")
log(f"Log: {log_path}")
log_file.close()
print("OK")
