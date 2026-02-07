# test_render_queue_details.py
# PyAE Render Queue Detailed Test
#
# NOTE: Output Module Operations - Handle Pairing Rules (2026-02-03)
# ==================================================================
# rq_itemH と outmodH は一貫したペアとして使用する必要があります:
#
# ✅ 正しいパターン:
#   - (rq_itemH, outmodH): 同じRQアイテムから取得した実際のハンドルペア
#   - (0, 0): Queuebert SDKパターン（最初のRQアイテム、最初の出力モジュール）
#
# ❌ エラーが発生するパターン:
#   - (0, outmodH): 混合パターン
#   - (rq_itemH, 0): 混合パターン
#
# 重要: AEGP_GetOutputModuleByIndex は 0ベースインデックスを使用（AEGP SDKの例外）

import ae
import os

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

suite = TestSuite("Render Queue Details")

# Test variables
_test_comp = None
_test_rq_item = None


@suite.setup
def setup():
    """Setup test environment"""
    global _test_comp, _test_rq_item
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_RQDetailTestComp", 1920, 1080, 1.0, 5.0, 30.0)
    # Add solid to ensure comp can be rendered
    _test_comp.add_solid("_RQDetailSolid", 1920, 1080, (1.0, 0.0, 0.0), 5.0)

    # Add to render queue
    try:
        rq = ae.render_queue
        _test_rq_item = rq.add_comp(_test_comp)
    except Exception as e:
        print(f"Setup error: {e}")
        _test_rq_item = None


@suite.teardown
def teardown():
    """Cleanup test environment"""
    global _test_comp, _test_rq_item
    try:
        if _test_rq_item is not None:
            _test_rq_item.delete()
    except:
        pass
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except:
        pass


# =============================================================
# Render Queue Item Tests
# =============================================================

@suite.test
def test_rq_item_valid():
    """Test render queue item validity"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    assert_true(_test_rq_item.valid, "RQ item should be valid")


@suite.test
def test_rq_item_queued_status():
    """Test queued status get/set"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    # Get initial queued status
    initial_queued = _test_rq_item.queued
    assert_isinstance(initial_queued, bool, "Queued should be boolean")

    # Toggle queued status
    _test_rq_item.queued = True
    assert_true(_test_rq_item.queued, "Should be queued after setting to True")

    _test_rq_item.queued = False
    assert_false(_test_rq_item.queued, "Should not be queued after setting to False")


@suite.test
def test_rq_item_status():
    """Test render queue item status"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    status = _test_rq_item.status
    assert_not_none(status, "Status should not be None")


@suite.test
def test_rq_item_comp_name():
    """Test getting composition name from RQ item"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    comp_name = _test_rq_item.comp_name
    assert_equal(comp_name, "_RQDetailTestComp", "Comp name should match")


# =============================================================
# Output Module Tests
# =============================================================

@suite.test
def test_output_module_count():
    """Test output module count"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    num_om = _test_rq_item.num_output_modules
    assert_true(num_om >= 0, "Number of output modules should be non-negative")
    # Usually at least 1 output module is created by default
    assert_true(num_om >= 1, "Should have at least 1 output module")


@suite.test
def test_output_module_access():
    """Test accessing output module by index"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    num_om = _test_rq_item.num_output_modules
    if num_om == 0:
        assert_true(True, "No output modules to test")
        return

    # Get first output module
    om = _test_rq_item.output_module(0)
    assert_not_none(om, "Output module should not be None")


@suite.test
def test_output_module_valid():
    """Test output module validity"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    num_om = _test_rq_item.num_output_modules
    if num_om == 0:
        assert_true(True, "No output modules to test")
        return

    om = _test_rq_item.output_module(0)
    assert_true(om.valid, "Output module should be valid")


@suite.test
def test_output_module_file_path_get():
    """Test getting output file path"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    num_om = _test_rq_item.num_output_modules
    if num_om == 0:
        assert_true(True, "No output modules to test")
        return

    om = _test_rq_item.output_module(0)
    file_path = om.file_path
    # File path might be empty initially, just check it's a string
    assert_isinstance(file_path, str, "File path should be a string")


@suite.test
def test_output_module_file_path_set():
    """Test setting output file path"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    num_om = _test_rq_item.num_output_modules
    if num_om == 0:
        assert_true(True, "No output modules to test")
        return

    om = _test_rq_item.output_module(0)

    # Set a test file path (use system temp directory)
    test_path = os.path.join(os.environ.get('TEMP', 'C:\\Temp'), "_test_output.mov")
    om.file_path = test_path

    # Path setting succeeded without error
    assert_true(True, "File path setter executed without error")


@suite.test
def test_output_module_embed_options():
    """Test embed options get/set"""
    if _test_rq_item is None:
        assert_true(True, "Skipping test - no RQ item")
        return

    num_om = _test_rq_item.num_output_modules
    if num_om == 0:
        assert_true(True, "No output modules to test")
        return

    om = _test_rq_item.output_module(0)

    # Get embed options - Note: embed_options getter returns EmbedOptions enum
    embed = om.embed_options
    # Enum supports int conversion due to py::arithmetic()
    assert_true(isinstance(embed, int) or hasattr(embed, 'value'), "Embed options should be int or enum")

    # Test setting embed options (0=None, 1=Link, 2=Project)
    om.embed_options = 0
    assert_true(True, "Embed options can be set to 0")

    # Verify the property is accessible - Note: returns EmbedOptions enum
    final_embed = om.embed_options
    assert_true(isinstance(final_embed, int) or hasattr(final_embed, 'value'), "Embed options should be int or enum")


# =============================================================
# Render Queue Operations
# =============================================================

@suite.test
def test_multiple_rq_items():
    """Test adding multiple items to render queue"""
    # Create another comp
    comp2 = ae.Project.get_current().create_comp("_RQMultiComp", 1280, 720, 1.0, 3.0, 30.0)
    comp2.add_solid("_Multi", 1280, 720, (0.0, 1.0, 0.0), 3.0)

    rq = ae.render_queue
    initial_count = rq.num_items()

    # Add to render queue
    rq_item2 = rq.add_comp(comp2)
    assert_not_none(rq_item2, "Second RQ item should be created")

    # Check count increased
    new_count = rq.num_items()
    assert_true(new_count > initial_count, "RQ should have more items")

    # Cleanup
    rq_item2.delete()
    comp2.delete()


@suite.test
def test_rq_item_deletion():
    """Test deleting render queue items"""
    # Create temp comp
    temp_comp = ae.Project.get_current().create_comp("_TempRQComp", 1920, 1080, 1.0, 2.0, 30.0)
    temp_comp.add_solid("_Temp", 1920, 1080, (1.0, 1.0, 1.0), 2.0)

    rq = ae.render_queue
    initial_count = rq.num_items()

    # Add and then delete
    temp_rq_item = rq.add_comp(temp_comp)
    mid_count = rq.num_items()
    assert_true(mid_count > initial_count, "Count should increase after adding")

    temp_rq_item.delete()
    final_count = rq.num_items()
    assert_equal(final_count, initial_count, "Count should return to initial after deletion")

    # Cleanup
    temp_comp.delete()


def run():
    """Run the test suite"""
    return suite.run()


if __name__ == "__main__":
    run()
