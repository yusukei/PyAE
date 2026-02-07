# test_project.py
# PyAE Project テスト

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
    )

suite = TestSuite("Project")

# テスト用変数
_test_folder = None
_test_comp = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_folder, _test_comp
    proj = ae.Project.get_current()
    # テスト用フォルダ作成
    _test_folder = proj.create_folder("_TestFolder")
    # テスト用コンポジション作成
    _test_comp = proj.create_comp("_TestComp", 1920, 1080, 1.0, 5.0, 30.0)


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_folder, _test_comp
    if _test_comp is not None:
        _test_comp.delete()
    if _test_folder is not None:
        _test_folder.delete()


@suite.test
def test_project_exists():
    """プロジェクトオブジェクトが存在する"""
    proj = ae.Project.get_current()
    assert_not_none(proj, "ae.project should not be None")


@suite.test
def test_project_name():
    """プロジェクト名が取得できる"""
    proj = ae.Project.get_current()
    name = proj.name
    assert_isinstance(name, str, "Project name should be a string")


@suite.test
def test_project_path():
    """プロジェクトパスが取得できる"""
    proj = ae.Project.get_current()
    path = proj.path
    # パスは文字列または空文字列
    assert_true(isinstance(path, str), "Project path should be a string")


@suite.test
def test_project_root_folder():
    """ルートフォルダが取得できる"""
    proj = ae.Project.get_current()
    root = proj.root_folder
    assert_not_none(root, "Root folder should not be None")


@suite.test
def test_project_items():
    """プロジェクトアイテム一覧が取得できる"""
    proj = ae.Project.get_current()
    items = proj.items
    assert_isinstance(items, list, "items should be a list")
    assert_true(len(items) >= 2, "Should have at least test folder and comp")


@suite.test
def test_create_folder():
    """フォルダを作成できる"""
    proj = ae.Project.get_current()
    folder = proj.create_folder("_TestFolder2")
    assert_not_none(folder, "Created folder should not be None")
    assert_equal("_TestFolder2", folder.name)
    # クリーンアップ
    folder.delete()


@suite.test
def test_create_comp():
    """コンポジションを作成できる"""
    proj = ae.Project.get_current()
    comp = proj.create_comp("_TestComp2", 1280, 720, 1.0, 10.0, 24.0)
    assert_not_none(comp, "Created comp should not be None")
    assert_equal("_TestComp2", comp.name)
    assert_equal(1280, comp.width)
    assert_equal(720, comp.height)
    # クリーンアップ
    comp.delete()


@suite.test
def test_item_by_name():
    """名前でアイテムを検索できる"""
    proj = ae.Project.get_current()
    item = proj.item_by_name("_TestFolder")
    assert_not_none(item, "Should find test folder by name")
    assert_equal("_TestFolder", item.name)


@suite.test
def test_item_by_name_not_found():
    """存在しないアイテム名でNoneが返る"""
    proj = ae.Project.get_current()
    item = proj.item_by_name("_NonExistent_XYZ_123")
    assert_none(item, "Should return None for non-existent item")


@suite.test
def test_active_item():
    """アクティブアイテムが取得できる（または None）"""
    proj = ae.Project.get_current()
    # active_item は None またはアイテム
    active = proj.active_item
    # 型チェックのみ（選択状態に依存）
    assert_true(
        active is None or hasattr(active, "name"),
        "active_item should be None or have name",
    )


@suite.test
def test_project_bit_depth():
    """プロジェクトのビット深度を取得・設定できる"""
    proj = ae.Project.get_current()

    # 初期値を保存
    original_depth = proj.bit_depth
    assert_true(original_depth in [8, 16, 32], "Bit depth should be 8, 16, or 32")

    # 異なるビット深度に設定
    test_depth = 16 if original_depth != 16 else 32
    proj.bit_depth = test_depth
    assert_equal(test_depth, proj.bit_depth, f"Bit depth should be {test_depth}")

    # 元に戻す
    proj.bit_depth = original_depth
    assert_equal(original_depth, proj.bit_depth, f"Bit depth should be restored to {original_depth}")


@suite.test
def test_project_is_dirty():
    """プロジェクトの編集済みフラグを確認できる"""
    proj = ae.Project.get_current()

    # is_dirty はブール値
    is_dirty = proj.is_dirty
    assert_isinstance(is_dirty, bool, "is_dirty should be a boolean")


@suite.test
def test_project_time_display():
    """プロジェクトの時間表示設定を取得・設定できる"""
    proj = ae.Project.get_current()

    # 時間表示設定を取得
    time_display = proj.get_time_display()
    assert_isinstance(time_display, dict, "time_display should be a dict")

    # 必須キーの存在確認
    required_keys = [
        "display_mode", "footage_display_mode", "display_dropframe",
        "use_feet_frames", "timebase", "frames_per_foot",
        "frames_display_mode"
    ]
    for key in required_keys:
        assert_true(key in time_display, f"time_display should have key '{key}'")

    # 設定を変更（frames_display_modeのみ変更）
    modified_display = time_display.copy()
    modified_display["frames_display_mode"] = 0
    proj.set_time_display(modified_display)

    # 変更が反映されているか確認
    new_display = proj.get_time_display()
    assert_equal(0, new_display["frames_display_mode"], "frames_display_mode should be 0")

    # 元に戻す
    proj.set_time_display(time_display)


@suite.test
def test_num_projects():
    """オープンしているプロジェクト数を取得できる"""
    num_projects = ae.Project.num_projects()
    assert_isinstance(num_projects, int, "num_projects should be an integer")
    assert_true(num_projects >= 1, "Should have at least one open project")


@suite.test
def test_get_by_index():
    """インデックスでプロジェクトを取得できる"""
    proj = ae.Project.get_by_index(0)
    assert_not_none(proj, "Project at index 0 should not be None")
    assert_true(proj.valid, "Project should be valid")


@suite.test
def test_is_project_open():
    """プロジェクトが開いているか確認できる"""
    is_open = ae.Project.is_open()
    assert_true(is_open, "A project should be open")


# Low-level SDK API tests
@suite.test
def test_sdk_get_num_projects():
    """SDK: プロジェクト数を取得できる"""
    num_projects = ae.sdk.AEGP_GetNumProjects()
    assert_isinstance(num_projects, int, "num_projects should be an integer")
    assert_true(num_projects >= 1, "Should have at least one open project")


@suite.test
def test_sdk_get_project_by_index():
    """SDK: インデックスからプロジェクトハンドルを取得できる"""
    projH = ae.sdk.AEGP_GetProjectByIndex(0)
    assert_true(projH > 0, "Project handle should be non-zero")


@suite.test
def test_sdk_get_project_name():
    """SDK: プロジェクト名を取得できる"""
    projH = ae.sdk.AEGP_GetProjectByIndex(0)
    name = ae.sdk.AEGP_GetProjectName(projH)
    assert_isinstance(name, str, "Project name should be a string")


@suite.test
def test_sdk_get_project_path():
    """SDK: プロジェクトパスを取得できる"""
    plugin_id = ae.sdk.AEGP_GetPluginID()
    projH = ae.sdk.AEGP_GetProjectByIndex(0)
    path = ae.sdk.AEGP_GetProjectPath(plugin_id, projH)
    assert_isinstance(path, str, "Project path should be a string")


@suite.test
def test_sdk_get_project_root_folder():
    """SDK: プロジェクトのルートフォルダを取得できる"""
    projH = ae.sdk.AEGP_GetProjectByIndex(0)
    rootFolderH = ae.sdk.AEGP_GetProjectRootFolder(projH)
    assert_true(rootFolderH > 0, "Root folder handle should be non-zero")


@suite.test
def test_sdk_project_is_dirty():
    """SDK: プロジェクトの編集済みフラグを確認できる"""
    projH = ae.sdk.AEGP_GetProjectByIndex(0)
    is_dirty = ae.sdk.AEGP_ProjectIsDirty(projH)
    assert_isinstance(is_dirty, bool, "is_dirty should be a boolean")


@suite.test
def test_sdk_get_project_bit_depth():
    """SDK: プロジェクトのビット深度を取得できる"""
    projH = ae.sdk.AEGP_GetProjectByIndex(0)
    bit_depth = ae.sdk.AEGP_GetProjectBitDepth(projH)
    assert_true(bit_depth in [8, 16, 32], "Bit depth should be 8, 16, or 32")


@suite.test
def test_sdk_set_project_bit_depth():
    """SDK: プロジェクトのビット深度を設定できる"""
    projH = ae.sdk.AEGP_GetProjectByIndex(0)

    # 初期値を保存
    original_depth = ae.sdk.AEGP_GetProjectBitDepth(projH)

    # 異なるビット深度に設定
    test_depth = 16 if original_depth != 16 else 32
    ae.sdk.AEGP_SetProjectBitDepth(projH, test_depth)

    # 確認
    current_depth = ae.sdk.AEGP_GetProjectBitDepth(projH)
    assert_equal(test_depth, current_depth, f"Bit depth should be {test_depth}")

    # 元に戻す
    ae.sdk.AEGP_SetProjectBitDepth(projH, original_depth)


@suite.test
def test_sdk_get_project_time_display():
    """SDK: プロジェクトの時間表示設定を取得できる"""
    projH = ae.sdk.AEGP_GetProjectByIndex(0)
    time_display = ae.sdk.AEGP_GetProjectTimeDisplay(projH)

    assert_isinstance(time_display, dict, "time_display should be a dict")

    # 必須キーの存在確認
    required_keys = [
        "display_mode", "footage_display_mode", "display_dropframe",
        "use_feet_frames", "timebase", "frames_per_foot",
        "frames_display_mode"
    ]
    for key in required_keys:
        assert_true(key in time_display, f"time_display should have key '{key}'")


@suite.test
def test_sdk_set_project_time_display():
    """SDK: プロジェクトの時間表示設定を設定できる"""
    projH = ae.sdk.AEGP_GetProjectByIndex(0)

    # 初期値を取得
    original_display = ae.sdk.AEGP_GetProjectTimeDisplay(projH)

    # 変更（frames_display_modeのみ変更）
    modified_display = original_display.copy()
    modified_display["frames_display_mode"] = 0
    ae.sdk.AEGP_SetProjectTimeDisplay(projH, modified_display)

    # 確認
    new_display = ae.sdk.AEGP_GetProjectTimeDisplay(projH)
    assert_equal(0, new_display["frames_display_mode"], "frames_display_mode should be 0")

    # 元に戻す
    ae.sdk.AEGP_SetProjectTimeDisplay(projH, original_display)


# -----------------------------------------------------------------------
# Project Save Tests
# -----------------------------------------------------------------------

@suite.test
def test_sdk_save_project_to_path():
    """SDK: プロジェクトを指定パスに保存できる"""
    import os
    import tempfile

    projH = ae.sdk.AEGP_GetProjectByIndex(0)

    # 一時ファイルパスを作成（.aepファイル）
    temp_dir = tempfile.gettempdir()
    temp_path = os.path.join(temp_dir, "_PyAE_TestSave.aep")

    # 保存を実行
    ae.sdk.AEGP_SaveProjectToPath(projH, temp_path)

    # ファイルが作成されたか確認
    assert_true(os.path.exists(temp_path), f"Project file should exist at {temp_path}")

    # クリーンアップ
    if os.path.exists(temp_path):
        os.remove(temp_path)


@suite.test
def test_sdk_save_project_as():
    """SDK: プロジェクトを新規パスに保存できる（SaveAs）"""
    import os
    import tempfile

    projH = ae.sdk.AEGP_GetProjectByIndex(0)

    # 一時ファイルパスを作成
    temp_dir = tempfile.gettempdir()
    temp_path = os.path.join(temp_dir, "_PyAE_TestSaveAs.aep")

    # 保存を実行
    ae.sdk.AEGP_SaveProjectAs(projH, temp_path)

    # ファイルが作成されたか確認
    assert_true(os.path.exists(temp_path), f"Project file should exist at {temp_path}")

    # クリーンアップ
    if os.path.exists(temp_path):
        os.remove(temp_path)


@suite.test
def test_project_save_as_method():
    """Project.save_as(): プロジェクトを新規パスに保存できる"""
    import os
    import tempfile

    proj = ae.Project.get_current()

    # 一時ファイルパスを作成
    temp_dir = tempfile.gettempdir()
    temp_path = os.path.join(temp_dir, "_PyAE_TestHighLevelSaveAs.aep")

    # 高レベルAPIで保存
    result = proj.save_as(temp_path)
    assert_true(result, "save_as should return True on success")

    # ファイルが作成されたか確認
    assert_true(os.path.exists(temp_path), f"Project file should exist at {temp_path}")

    # クリーンアップ
    if os.path.exists(temp_path):
        os.remove(temp_path)


@suite.test
def test_project_save_unicode_path():
    """Unicode文字を含むパスに保存できる"""
    import os
    import tempfile

    projH = ae.sdk.AEGP_GetProjectByIndex(0)

    # 日本語を含むファイル名
    temp_dir = tempfile.gettempdir()
    temp_path = os.path.join(temp_dir, "_PyAE_テスト保存.aep")

    # 保存を実行
    ae.sdk.AEGP_SaveProjectToPath(projH, temp_path)

    # ファイルが作成されたか確認
    assert_true(os.path.exists(temp_path), f"Project file should exist at {temp_path}")

    # クリーンアップ
    if os.path.exists(temp_path):
        os.remove(temp_path)


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
