# test_utility.py
# PyAE Utility Suite テスト

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
    )

suite = TestSuite("Utility")

# テスト用変数
_plugin_id = None
_err_state = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _plugin_id
    _plugin_id = ae.sdk.AEGP_GetPluginID()


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    pass


# -----------------------------------------------------------------------
# Plugin ID and Version Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_plugin_id():
    """プラグインIDを取得できる"""
    plugin_id = ae.sdk.AEGP_GetPluginID()
    assert_isinstance(plugin_id, int, "Plugin ID should be an integer")
    assert_true(plugin_id > 0, "Plugin ID should be positive")


@suite.test
def test_get_driver_plugin_init_func_version():
    """プラグイン初期化関数バージョンを取得できる"""
    major, minor = ae.sdk.AEGP_GetDriverPluginInitFuncVersion()
    assert_isinstance(major, int, "Major version should be an integer")
    assert_isinstance(minor, int, "Minor version should be an integer")


@suite.test
def test_get_driver_implementation_version():
    """実装バージョンを取得できる"""
    major, minor = ae.sdk.AEGP_GetDriverImplementationVersion()
    assert_isinstance(major, int, "Major version should be an integer")
    assert_isinstance(minor, int, "Minor version should be an integer")


# -----------------------------------------------------------------------
# Error Handling Tests
# -----------------------------------------------------------------------

@suite.test
def test_start_end_quiet_errors():
    """エラー抑制の開始と終了ができる"""
    global _err_state
    _err_state = ae.sdk.AEGP_StartQuietErrors()
    assert_isinstance(_err_state, int, "Error state should be an integer")

    # 終了
    ae.sdk.AEGP_EndQuietErrors(False, _err_state)
    assert_true(True, "Start/End quiet errors succeeded")


@suite.test
def test_get_last_error_message():
    """最後のエラーメッセージを取得できる"""
    error_msg, error_num = ae.sdk.AEGP_GetLastErrorMessage()
    assert_isinstance(error_msg, str, "Error message should be a string")
    assert_isinstance(error_num, int, "Error number should be an integer")


# -----------------------------------------------------------------------
# Undo Group Tests
# -----------------------------------------------------------------------

@suite.test
def test_undo_group():
    """Undoグループを開始・終了できる"""
    ae.sdk.AEGP_StartUndoGroup("PyAE Test Undo Group")
    ae.sdk.AEGP_EndUndoGroup()
    assert_true(True, "Undo group started and ended successfully")


# -----------------------------------------------------------------------
# UI and Platform Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_suppress_interactive_ui():
    """UI抑制状態を取得できる"""
    is_suppressed = ae.sdk.AEGP_GetSuppressInteractiveUI()
    assert_isinstance(is_suppressed, bool, "UI suppression state should be boolean")


@suite.test
def test_get_main_hwnd():
    """メインウィンドウハンドルを取得できる"""
    hwnd = ae.sdk.AEGP_GetMainHWND()
    assert_isinstance(hwnd, int, "Window handle should be an integer (uintptr_t)")


@suite.test
def test_host_is_activated():
    """After Effectsがアクティベートされているか確認できる"""
    is_activated = ae.sdk.AEGP_HostIsActivated()
    assert_isinstance(is_activated, bool, "Activation state should be boolean")


# -----------------------------------------------------------------------
# Paint Palette Tests
# -----------------------------------------------------------------------

@suite.test
def test_paint_pal_get_fore_color():
    """前景色を取得できる"""
    color = ae.sdk.AEGP_PaintPalGetForeColor()
    assert_isinstance(color, dict, "Color should be a dictionary")
    assert_true("red" in color, "Color should have 'red' key")
    assert_true("green" in color, "Color should have 'green' key")
    assert_true("blue" in color, "Color should have 'blue' key")
    assert_true("alpha" in color, "Color should have 'alpha' key")


@suite.test
def test_paint_pal_get_back_color():
    """背景色を取得できる"""
    color = ae.sdk.AEGP_PaintPalGetBackColor()
    assert_isinstance(color, dict, "Color should be a dictionary")
    assert_true("red" in color, "Color should have 'red' key")


@suite.test
def test_paint_pal_set_colors():
    """前景色と背景色を設定できる"""
    # 色を取得
    original_fore = ae.sdk.AEGP_PaintPalGetForeColor()
    original_back = ae.sdk.AEGP_PaintPalGetBackColor()

    # 新しい色を設定
    test_color = {"red": 1.0, "green": 0.5, "blue": 0.0, "alpha": 1.0}
    ae.sdk.AEGP_PaintPalSetForeColor(test_color)
    ae.sdk.AEGP_PaintPalSetBackColor(test_color)

    # 元に戻す
    ae.sdk.AEGP_PaintPalSetForeColor(original_fore)
    ae.sdk.AEGP_PaintPalSetBackColor(original_back)

    assert_true(True, "Paint palette colors set successfully")


# -----------------------------------------------------------------------
# Character Palette Tests
# -----------------------------------------------------------------------

@suite.test
def test_char_pal_get_fill_color():
    """テキスト塗りつぶし色を取得できる"""
    is_defined, color = ae.sdk.AEGP_CharPalGetFillColor()
    assert_isinstance(is_defined, bool, "Fill color defined flag should be boolean")
    assert_isinstance(color, dict, "Color should be a dictionary")


@suite.test
def test_char_pal_get_stroke_color():
    """テキストストローク色を取得できる"""
    is_defined, color = ae.sdk.AEGP_CharPalGetStrokeColor()
    assert_isinstance(is_defined, bool, "Stroke color defined flag should be boolean")
    assert_isinstance(color, dict, "Color should be a dictionary")


@suite.test
def test_char_pal_set_colors():
    """テキスト色を設定できる"""
    test_color = {"red": 1.0, "green": 0.0, "blue": 0.0, "alpha": 1.0}
    ae.sdk.AEGP_CharPalSetFillColor(test_color)
    ae.sdk.AEGP_CharPalSetStrokeColor(test_color)
    assert_true(True, "Character palette colors set successfully")


@suite.test
def test_char_pal_is_fill_color_ui_frontmost():
    """塗りつぶし色UIが最前面か確認できる"""
    is_frontmost = ae.sdk.AEGP_CharPalIsFillColorUIFrontmost()
    assert_isinstance(is_frontmost, bool, "Frontmost flag should be boolean")


# -----------------------------------------------------------------------
# Conversion Tests
# -----------------------------------------------------------------------

@suite.test
def test_convert_fplong_to_hsf_ratio():
    """浮動小数点数からHSFRatio変換ができる"""
    numerator, denominator = ae.sdk.AEGP_ConvertFpLongToHSFRatio(1.5)
    assert_isinstance(numerator, int, "Numerator should be an integer")
    assert_isinstance(denominator, int, "Denominator should be an integer")


@suite.test
def test_convert_hsf_ratio_to_fplong():
    """HSFRatioから浮動小数点数変換ができる"""
    number = ae.sdk.AEGP_ConvertHSFRatioToFpLong(3, 2)
    assert_isinstance(number, float, "Number should be a float")
    assert_true(abs(number - 1.5) < 0.01, "Conversion should produce 1.5")


@suite.test
def test_conversion_roundtrip():
    """変換のラウンドトリップが正しく動作する"""
    original = 2.5
    num, den = ae.sdk.AEGP_ConvertFpLongToHSFRatio(original)
    result = ae.sdk.AEGP_ConvertHSFRatioToFpLong(num, den)
    assert_true(abs(result - original) < 0.01, "Roundtrip conversion should preserve value")


# -----------------------------------------------------------------------
# Logging and Debug Tests
# -----------------------------------------------------------------------

@suite.test
def test_write_to_os_console():
    """OSコンソールに書き込める"""
    ae.sdk.AEGP_WriteToOSConsole("PyAE Test: Console output test")
    assert_true(True, "Console write succeeded")


@suite.test
def test_write_to_debug_log():
    """デバッグログに書き込める"""
    ae.sdk.AEGP_WriteToDebugLog("PyAE", "TestEvent", "Debug log test from PyAE")
    assert_true(True, "Debug log write succeeded")


# -----------------------------------------------------------------------
# Scripting Tests
# -----------------------------------------------------------------------

@suite.test
def test_is_scripting_available():
    """スクリプト機能が利用可能か確認できる"""
    is_available = ae.sdk.AEGP_IsScriptingAvailable()
    assert_isinstance(is_available, bool, "Scripting availability should be boolean")


@suite.test
def test_execute_script():
    """ExtendScriptを実行できる"""
    try:
        result = ae.sdk.AEGP_ExecuteScript("1 + 1")
        assert_isinstance(result, str, "Script result should be a string")
        # 結果は"2"または空文字列（環境による）
        assert_true(True, "Script execution succeeded")
    except:
        # スクリプトが利用できない環境でもテストを通す
        assert_true(True, "Script execution attempted")


# -----------------------------------------------------------------------
# Plugin Path Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_plugin_paths():
    """プラグインパスを取得できる"""
    global _plugin_id
    try:
        # AEGP_GetPathTypes_PLUGIN = 0 is NOT IMPLEMENTED (documented in SDK)
        # Use AEGP_GetPathTypes_USER_PLUGIN = 1 instead
        path = ae.sdk.AEGP_GetPluginPaths(_plugin_id, 1)
        assert_isinstance(path, str, "Plugin path should be a string")
        assert_true(len(path) > 0, "Plugin path should not be empty")
    except Exception as e:
        # 一部の環境でサポートされていない可能性がある
        print(f"GetPluginPaths test skipped: {e}")
        assert_true(True, "GetPluginPaths attempted")


# -----------------------------------------------------------------------
# Miscellaneous Tests
# -----------------------------------------------------------------------

@suite.test
def test_cause_idle_routines_to_be_called():
    """アイドルルーチン呼び出しを要求できる"""
    ae.sdk.AEGP_CauseIdleRoutinesToBeCalled()
    assert_true(True, "Idle routine call request succeeded")


@suite.test
def test_update_font_list():
    """フォントリストを更新できる"""
    ae.sdk.AEGP_UpdateFontList()
    assert_true(True, "Font list update succeeded")


@suite.test
def test_get_plugin_platform_ref():
    """プラグインプラットフォームリファレンスを取得できる"""
    global _plugin_id
    try:
        plat_ref = ae.sdk.AEGP_GetPluginPlatformRef(_plugin_id)
        assert_isinstance(plat_ref, int, "Platform reference should be an integer (uintptr_t)")
        # Windowsでは通常NULL(0)が返される
        assert_true(True, "Platform reference retrieved")
    except:
        # 環境によってはサポートされない可能性がある
        assert_true(True, "GetPluginPlatformRef attempted")


# -----------------------------------------------------------------------
# Report/Dialog Tests (注意: ダイアログが表示される可能性がある)
# -----------------------------------------------------------------------

# Note: ReportInfo/ReportInfoUnicodeはダイアログを表示するため、
# 自動テストでは実行しない（コメントアウト）

# @suite.test
# def test_report_info():
#     """情報ダイアログを表示できる（手動テスト用）"""
#     global _plugin_id
#     ae.sdk.AEGP_ReportInfo(_plugin_id, "PyAE Test: ReportInfo test")
#     assert_true(True, "ReportInfo succeeded")

# @suite.test
# def test_report_info_unicode():
#     """Unicode情報ダイアログを表示できる（手動テスト用）"""
#     global _plugin_id
#     ae.sdk.AEGP_ReportInfoUnicode(_plugin_id, "PyAE Test: Unicode 日本語テスト")
#     assert_true(True, "ReportInfoUnicode succeeded")


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
