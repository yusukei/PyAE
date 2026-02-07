# test_extendscript.py
# execute_extendscript API の動作確認

import ae

def test_extendscript():
    """ExtendScript 実行をテスト"""
    ae.log_info("=== ExtendScript API Test ===")

    try:
        # シンプルなテスト
        result = ae.execute_extendscript("1 + 1")
        ae.log_info(f"Test 1 (1+1): {result}")

        # AE情報取得
        result = ae.execute_extendscript("app.version")
        ae.log_info(f"Test 2 (app.version): {result}")

        # プロジェクト名取得
        result = ae.execute_extendscript("app.project.file ? app.project.file.name : 'Untitled'")
        ae.log_info(f"Test 3 (project name): {result}")

        ae.log_info("✅ All ExtendScript tests passed")
        return "OK"

    except Exception as e:
        ae.log_error(f"❌ ExtendScript test failed: {e}")
        import traceback
        traceback.print_exc()
        return "ERROR"

if __name__ == "__main__":
    test_extendscript()
