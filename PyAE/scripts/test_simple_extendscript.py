# test_simple_extendscript.py
# 最小限の ExtendScript テスト

import ae
import os

log_path = os.path.join(ae.get_documents_folder(), "simple_extendscript_test.txt")
log_file = open(log_path, "w", encoding="utf-8")

def log(msg):
    ae.log_info(f"[ES_TEST] {msg}")
    log_file.write(msg + "\n")
    log_file.flush()

log("=== Simple ExtendScript Test ===")

# Test 1: 簡単な計算
log("\nTest 1: Simple math (1+1)")
try:
    result = ae.execute_extendscript("1 + 1")
    log(f"  Result: {result}")
except Exception as e:
    log(f"  ERROR: {e}")

# Test 2: 文字列
log("\nTest 2: String return")
try:
    result = ae.execute_extendscript("'hello'")
    log(f"  Result: {result}")
except Exception as e:
    log(f"  ERROR: {e}")

# Test 3: app.project
log("\nTest 3: app.project.numItems")
try:
    result = ae.execute_extendscript("app.project.numItems")
    log(f"  Result: {result}")
except Exception as e:
    log(f"  ERROR: {e}")

# Test 4: 複数行スクリプト
log("\nTest 4: Multi-line IIFE")
try:
    script = """
    (function() {
        return "IIFE works";
    })();
    """
    result = ae.execute_extendscript(script)
    log(f"  Result: {result}")
except Exception as e:
    log(f"  ERROR: {e}")

# Test 5: 変数を使う
log("\nTest 5: Variable usage")
try:
    script = """
    (function() {
        var x = 10;
        var y = 20;
        return x + y;
    })();
    """
    result = ae.execute_extendscript(script)
    log(f"  Result: {result}")
except Exception as e:
    log(f"  ERROR: {e}")

# Test 6: app.project.item へのアクセス
log("\nTest 6: Access project items")
try:
    script = """
    (function() {
        var count = app.project.numItems;
        if (count > 0) {
            return "Items: " + count + ", First: " + app.project.item(1).name;
        }
        return "No items";
    })();
    """
    result = ae.execute_extendscript(script)
    log(f"  Result: {result}")
except Exception as e:
    log(f"  ERROR: {e}")

# Test 7: layer へのアクセス（if comp exists）
log("\nTest 7: Layer access")
try:
    script = """
    (function() {
        for (var i = 1; i <= app.project.numItems; i++) {
            var item = app.project.item(i);
            if (item instanceof CompItem && item.numLayers > 0) {
                var layer = item.layer(1);
                return "Layer: " + layer.name;
            }
        }
        return "No layers found";
    })();
    """
    result = ae.execute_extendscript(script)
    log(f"  Result: {result}")
except Exception as e:
    log(f"  ERROR: {e}")

log("\n=== All Tests Complete ===")
log(f"Log: {log_path}")
log_file.close()

print("OK")
