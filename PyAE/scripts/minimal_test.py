# minimal_test.py
# 最小限のテスト

import ae
import os

try:
    log_path = os.path.join(ae.get_documents_folder(), "minimal_test.txt")
    with open(log_path, "w", encoding="utf-8") as f:
        f.write("Minimal test started\n")
        f.write(f"ae module loaded: {ae}\n")

        # 単純なテスト
        try:
            result = ae.execute_extendscript("42")
            f.write(f"execute_extendscript('42') = {result}\n")
        except Exception as e:
            f.write(f"ERROR: {e}\n")

        f.write("Test complete\n")
    print("OK")
except Exception as e:
    ae.log_error(f"Minimal test failed: {e}")
    print(f"ERROR: {e}")
