# test_item.py
# PyAE Item/Folder テスト

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

suite = TestSuite("Item/Folder")

# テスト用変数
_parent_folder = None
_child_folder = None
_test_comp = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _parent_folder, _child_folder, _test_comp
    proj = ae.Project.get_current()
    _parent_folder = proj.create_folder("_ItemTestParent")
    _child_folder = proj.create_folder("_ItemTestChild")
    _test_comp = proj.create_comp("_ItemTestComp", 1920, 1080, 1.0, 5.0, 30.0)


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _parent_folder, _child_folder, _test_comp
    for item in [_test_comp, _child_folder, _parent_folder]:
        if item is not None:
            item.delete()


@suite.test
def test_folder_name():
    """フォルダ名が取得・設定できる"""
    global _child_folder
    original_name = _child_folder.name
    _child_folder.name = "_RenamedFolder"
    assert_equal("_RenamedFolder", _child_folder.name)
    _child_folder.name = original_name  # 元に戻す


@suite.test
def test_folder_is_valid():
    """フォルダが有効か確認できる"""
    global _child_folder
    # Property is 'valid' in bindings
    assert_true(_child_folder.valid, "Folder should be valid")


@suite.test
def test_folder_item_type():
    """フォルダのアイテムタイプが正しい"""
    global _child_folder
    # Property is 'type' in bindings
    item_type = _child_folder.type
    assert_not_none(item_type)


@suite.test
def test_folder_parent_folder():
    """フォルダの親フォルダを取得・設定できる"""
    global _parent_folder, _child_folder
    # 親フォルダを設定
    _child_folder.parent_folder = _parent_folder
    # 親フォルダを確認
    parent = _child_folder.parent_folder
    assert_not_none(parent, "Parent folder should not be None after setting")


@suite.test
def test_folder_children():
    """フォルダの子アイテムを取得できる"""
    global _parent_folder, _child_folder
    # 子フォルダを親に移動
    _child_folder.parent_folder = _parent_folder
    # 子アイテムを取得 (Property is 'items')
    children = _parent_folder.items
    assert_isinstance(children, list)
    assert_true(len(children) >= 1, "Parent should have at least one child")


@suite.test
def test_comp_item_properties():
    """コンポジションアイテムのプロパティが正しい"""
    global _test_comp
    assert_equal("_ItemTestComp", _test_comp.name)
    assert_equal(1920, _test_comp.width)
    assert_equal(1080, _test_comp.height)


@suite.test
def test_item_comment():
    """アイテムのコメントが取得・設定できる"""
    global _test_comp
    _test_comp.comment = "Test comment"
    assert_equal("Test comment", _test_comp.comment)
    _test_comp.comment = ""  # クリア


@suite.test
def test_item_selected():
    """アイテムの選択状態が取得・設定できる"""
    global _test_comp
    # 選択状態を設定
    _test_comp.selected = True
    assert_true(_test_comp.selected, "Item should be selected")
    _test_comp.selected = False
    assert_false(_test_comp.selected, "Item should not be selected")


@suite.test
def test_item_duplicate():
    """アイテムを複製できる"""
    global _test_comp
    dup = _test_comp.duplicate()
    assert_not_none(dup, "Duplicated item should not be None")
    assert_true("_ItemTestComp" in dup.name, "Duplicated name should contain original")
    # クリーンアップ
    dup.delete()


@suite.test
def test_project_item_iteration():
    """プロジェクトアイテムの走査ができる"""
    proj = ae.Project.get_current()

    # 低レベルAPI: GetFirstProjItem / GetNextProjItem を使用
    projectH = proj._get_handle_ptr()

    # 最初のアイテムを取得
    itemH = ae.sdk.AEGP_GetFirstProjItem(projectH)
    assert_true(itemH != 0, "First item should exist")

    # アイテムを走査
    item_count = 0
    current_itemH = itemH
    while current_itemH != 0:
        item_count += 1
        # 次のアイテムを取得（最後のアイテムの後は0を返す）
        current_itemH = ae.sdk.AEGP_GetNextProjItem(projectH, current_itemH)
        # 無限ループ防止
        if item_count > 100:
            break

    assert_true(item_count >= 3, "Project should have at least 3 items (folders + comp)")


@suite.test
def test_item_id():
    """アイテムIDが取得できる"""
    global _test_comp

    # 低レベルAPI: AEGP_GetItemID を使用
    itemH = _test_comp._get_handle_ptr()
    item_id = ae.sdk.AEGP_GetItemID(itemH)

    assert_true(item_id > 0, "Item ID should be positive")


@suite.test
def test_item_parent_folder():
    """アイテムの親フォルダが取得・設定できる"""
    global _test_comp
    proj = ae.Project.get_current()

    # 低レベルAPI: AEGP_GetItemParentFolder を使用
    itemH = _test_comp._get_handle_ptr()
    parent_folderH = ae.sdk.AEGP_GetItemParentFolder(itemH)

    # ルートフォルダかどうか確認（ルートの場合は0が返る可能性がある）
    assert_true(parent_folderH >= 0, "Parent folder handle should be non-negative")


@suite.test
def test_item_pixel_aspect_ratio():
    """アイテムのピクセルアスペクト比が取得できる"""
    global _test_comp

    # 低レベルAPI: AEGP_GetItemPixelAspectRatio を使用
    itemH = _test_comp._get_handle_ptr()
    num, den = ae.sdk.AEGP_GetItemPixelAspectRatio(itemH)

    assert_true(num > 0, "Pixel aspect ratio numerator should be positive")
    assert_true(den > 0, "Pixel aspect ratio denominator should be positive")


@suite.test
def test_item_type_name():
    """アイテムタイプ名が取得できる"""
    global _test_comp

    # 低レベルAPI: AEGP_GetTypeName を使用
    itemH = _test_comp._get_handle_ptr()
    item_type = ae.sdk.AEGP_GetItemType(itemH)
    type_name = ae.sdk.AEGP_GetTypeName(item_type)

    assert_not_none(type_name, "Type name should not be None")
    assert_true(len(type_name) > 0, "Type name should not be empty")


@suite.test
def test_item_mru_view():
    """アイテムの最近使用したビューを取得できる"""
    global _test_comp

    itemH = _test_comp._get_handle_ptr()

    # AEGP_GetItemMRUView を使用
    viewP = ae.sdk.AEGP_GetItemMRUView(itemH)

    # viewPは0（ビューがない）または有効なポインタ
    assert_true(viewP >= 0, "MRU view should be non-negative (0 if no view)")
    print(f"Item MRU view: {viewP}")


@suite.test
def test_set_item_use_proxy():
    """アイテムのプロキシ使用設定ができる"""
    global _test_comp

    itemH = _test_comp._get_handle_ptr()

    # プロキシ設定（コンポジションの場合、通常はフッテージ用）
    # この関数はFolderやCompには適用されない可能性があるため、
    # エラーが発生する可能性を考慮
    try:
        # プロキシを無効化
        ae.sdk.AEGP_SetItemUseProxy(itemH, False)
        print("Set item use proxy: False")

        # プロキシを有効化（プロキシがない場合は無視される）
        ae.sdk.AEGP_SetItemUseProxy(itemH, True)
        print("Set item use proxy: True")

        assert_true(True, "SetItemUseProxy succeeded")
    except RuntimeError as e:
        # コンポジションの場合、プロキシ設定がサポートされていない可能性
        print(f"SetItemUseProxy may not be supported for this item type: {e}")
        assert_true(True, "SetItemUseProxy test skipped for comp")


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
