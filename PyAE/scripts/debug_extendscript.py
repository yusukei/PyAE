# debug_extendscript.py
# execute_extendscript の詳細デバッグ

import ae
import os

# ログファイルに出力
log_path = os.path.join(ae.get_documents_folder(), "debug_extendscript.txt")
log_file = open(log_path, "w", encoding="utf-8")

def log(msg):
    ae.log_info(f"[DEBUG] {msg}")
    log_file.write(msg + "\n")
    log_file.flush()

log("=== ExtendScript Debug Start ===")
log(f"ae module: {ae}")
log(f"ae module type: {type(ae)}")

# ae モジュールの属性をチェック
log("\n--- ae module attributes ---")
for attr in sorted(dir(ae)):
    if not attr.startswith('_'):
        log(f"  {attr}")

# execute_extendscript の有無を確認
log("\n--- execute_extendscript check ---")
log(f"hasattr(ae, 'execute_extendscript'): {hasattr(ae, 'execute_extendscript')}")

if hasattr(ae, 'execute_extendscript'):
    log("execute_extendscript is available")
    log(f"  type: {type(ae.execute_extendscript)}")
    log(f"  repr: {repr(ae.execute_extendscript)}")

    # 実際に呼び出してみる
    log("\n--- Calling execute_extendscript ---")
    try:
        result = ae.execute_extendscript("1 + 1")
        log(f"  Result of '1 + 1': {result}")
        log(f"  Result type: {type(result)}")
    except Exception as e:
        log(f"  ERROR: {e}")
        import traceback
        log(f"  Traceback:\n{traceback.format_exc()}")
else:
    log("execute_extendscript is NOT available")

    # SDK モジュールを確認
    log("\n--- Checking ae.sdk module ---")
    if hasattr(ae, 'sdk'):
        log(f"ae.sdk: {ae.sdk}")
        log("ae.sdk attributes:")
        for attr in sorted(dir(ae.sdk)):
            if not attr.startswith('_'):
                log(f"  {attr}")

        if hasattr(ae.sdk, 'AEGP_ExecuteScript'):
            log("\nae.sdk.AEGP_ExecuteScript is available")
            try:
                result = ae.sdk.AEGP_ExecuteScript("1 + 1")
                log(f"  Result: {result}")
            except Exception as e:
                log(f"  ERROR: {e}")
    else:
        log("ae.sdk is NOT available")

log("\n=== Debug Complete ===")
log(f"Log saved to: {log_path}")
log_file.close()

print(f"Debug log: {log_path}")
