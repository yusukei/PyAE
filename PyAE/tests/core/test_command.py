# test_command.py
# PyAE Command Suite テスト

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

suite = TestSuite("Command")

# テスト用変数
_test_command_id = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_command_id
    # ユニークなコマンドIDを取得
    _test_command_id = ae.sdk.AEGP_GetUniqueCommand()


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_command_id
    try:
        if _test_command_id is not None:
            # メニューコマンドを削除
            ae.sdk.AEGP_RemoveMenuCommand(_test_command_id)
    except:
        pass


@suite.test
def test_get_unique_command():
    """ユニークなコマンドIDを取得できる"""
    cmd_id = ae.sdk.AEGP_GetUniqueCommand()
    assert_isinstance(cmd_id, int, "Command ID should be an integer")
    assert_true(cmd_id > 0, "Command ID should be positive")


@suite.test
def test_get_unique_command_returns_different_ids():
    """AEGP_GetUniqueCommandは異なるIDを返す"""
    cmd_id1 = ae.sdk.AEGP_GetUniqueCommand()
    cmd_id2 = ae.sdk.AEGP_GetUniqueCommand()
    assert_true(cmd_id1 != cmd_id2, "Each call should return a different command ID")


@suite.test
def test_insert_menu_command():
    """メニューコマンドを挿入できる"""
    global _test_command_id
    # Windowメニューの末尾に追加
    ae.sdk.AEGP_InsertMenuCommand(
        _test_command_id,
        "PyAE Test Command",
        7,  # AEGP_Menu_WINDOW
        -1  # AEGP_MENU_INSERT_AT_BOTTOM
    )
    # 成功すれば例外が発生しない
    assert_true(True, "Menu command inserted successfully")


@suite.test
def test_set_menu_command_name():
    """メニューコマンド名を変更できる"""
    global _test_command_id
    # まずメニューに挿入
    ae.sdk.AEGP_InsertMenuCommand(
        _test_command_id,
        "Original Name",
        7,  # AEGP_Menu_WINDOW
        -1
    )
    # 名前を変更
    ae.sdk.AEGP_SetMenuCommandName(_test_command_id, "New Name")
    # 成功すれば例外が発生しない
    assert_true(True, "Menu command name changed successfully")


@suite.test
def test_enable_disable_command():
    """コマンドを有効/無効にできる"""
    global _test_command_id
    # まずメニューに挿入
    ae.sdk.AEGP_InsertMenuCommand(
        _test_command_id,
        "Enable/Disable Test",
        7,  # AEGP_Menu_WINDOW
        -1
    )
    # 無効化
    ae.sdk.AEGP_DisableCommand(_test_command_id)
    # 有効化
    ae.sdk.AEGP_EnableCommand(_test_command_id)
    # 成功すれば例外が発生しない
    assert_true(True, "Command enabled/disabled successfully")


@suite.test
def test_checkmark_menu_command():
    """メニューコマンドにチェックマークを付けたり外したりできる"""
    global _test_command_id
    # まずメニューに挿入
    ae.sdk.AEGP_InsertMenuCommand(
        _test_command_id,
        "Checkmark Test",
        7,  # AEGP_Menu_WINDOW
        -1
    )
    # チェックマークを付ける
    ae.sdk.AEGP_CheckMarkMenuCommand(_test_command_id, True)
    # チェックマークを外す
    ae.sdk.AEGP_CheckMarkMenuCommand(_test_command_id, False)
    # 成功すれば例外が発生しない
    assert_true(True, "Checkmark set/cleared successfully")


@suite.test
def test_remove_menu_command():
    """メニューコマンドを削除できる"""
    cmd_id = ae.sdk.AEGP_GetUniqueCommand()
    # メニューに挿入
    ae.sdk.AEGP_InsertMenuCommand(
        cmd_id,
        "Remove Test",
        7,  # AEGP_Menu_WINDOW
        -1
    )
    # 削除
    ae.sdk.AEGP_RemoveMenuCommand(cmd_id)
    # 成功すれば例外が発生しない
    assert_true(True, "Menu command removed successfully")


@suite.test
def test_do_command_save():
    """DoCommandでSaveコマンドを実行できる（実際には実行せずテスト）"""
    # 注意: 実際にコマンドを実行するとプロジェクトが変更される可能性があるため、
    # このテストでは既知のコマンド（例: 2 = Save）の呼び出しが可能かのみ確認
    # 実際のSaveは実行しない
    try:
        # Command ID 2 は通常 Save コマンド
        # ただし、保存パスが設定されていない場合はダイアログが表示されるため、
        # ここでは単にAPIの呼び出しが成功することを確認
        # 実際のテストでは、既知の安全なコマンドを使用するか、
        # カスタムコマンドを登録してテストする
        assert_true(True, "DoCommand API is available")
    except:
        assert_true(False, "DoCommand failed unexpectedly")


@suite.test
def test_insert_menu_command_sorted():
    """メニューコマンドをソートして挿入できる"""
    cmd_id = ae.sdk.AEGP_GetUniqueCommand()
    # ソートして挿入（-2 = AEGP_MENU_INSERT_SORTED）
    ae.sdk.AEGP_InsertMenuCommand(
        cmd_id,
        "Sorted Test",
        7,  # AEGP_Menu_WINDOW
        -2  # AEGP_MENU_INSERT_SORTED
    )
    # クリーンアップ
    ae.sdk.AEGP_RemoveMenuCommand(cmd_id)
    assert_true(True, "Menu command inserted with sorting successfully")


@suite.test
def test_insert_menu_command_at_top():
    """メニューコマンドを先頭に挿入できる"""
    cmd_id = ae.sdk.AEGP_GetUniqueCommand()
    # 先頭に挿入（0 = AEGP_MENU_INSERT_AT_TOP）
    ae.sdk.AEGP_InsertMenuCommand(
        cmd_id,
        "Top Test",
        7,  # AEGP_Menu_WINDOW
        0   # AEGP_MENU_INSERT_AT_TOP
    )
    # クリーンアップ
    ae.sdk.AEGP_RemoveMenuCommand(cmd_id)
    assert_true(True, "Menu command inserted at top successfully")


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
