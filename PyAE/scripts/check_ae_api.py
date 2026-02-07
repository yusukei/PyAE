# check_ae_api.py
# ae モジュールの API を確認

import ae

ae.log_info("=== AE Module API Check ===")

# execute_extendscript の有無を確認
if hasattr(ae, 'execute_extendscript'):
    ae.log_info("✅ ae.execute_extendscript is available")
    try:
        result = ae.execute_extendscript("'test'")
        ae.log_info(f"  Test result: {result}")
    except Exception as e:
        ae.log_error(f"  ERROR calling execute_extendscript: {e}")
else:
    ae.log_error("❌ ae.execute_extendscript is NOT available")

# すべての ae モジュールの属性をリスト
ae.log_info("\nAll ae module attributes:")
attrs = [attr for attr in dir(ae) if not attr.startswith('_')]
for attr in sorted(attrs):
    ae.log_info(f"  - {attr}")

print("Check complete")
