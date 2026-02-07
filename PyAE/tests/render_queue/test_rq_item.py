#!/usr/bin/env python
# -*- coding: utf-8 -*-
# tests/render_queue/test_rq_item.py

"""
RQItemSuite4のテスト

このモジュールはAEGP_RQItemSuite4の新規実装関数をテストします。

テスト対象関数:
- AEGP_GetNextRQItem
- AEGP_GetStartedTime
- AEGP_GetElapsedTime
- AEGP_GetLogType / AEGP_SetLogType
- AEGP_GetComment / AEGP_SetComment
- AEGP_RemoveOutputModule
"""

import ae

try:
    from ..test_utils import TestSuite, assert_true, assert_isinstance, assert_equal
except ImportError:
    from test_utils import TestSuite, assert_true, assert_isinstance, assert_equal

# =============================================================================
# グローバル変数
# =============================================================================

suite = TestSuite("RQItemSuite4")

_test_comp1 = None
_test_comp2 = None
_test_rq_item1 = None
_test_rq_item2 = None

# =============================================================================
# セットアップ
# =============================================================================

@suite.setup
def setup():
    """テスト環境のセットアップ"""
    global _test_comp1, _test_comp2, _test_rq_item1, _test_rq_item2

    # プロジェクト取得
    proj = ae.Project.get_current()

    # テスト用コンポジション作成
    _test_comp1 = proj.create_comp("RQItemTest_Comp1", 1920, 1080, 1.0, 5.0, 30.0)
    _test_comp2 = proj.create_comp("RQItemTest_Comp2", 1920, 1080, 1.0, 5.0, 30.0)

    # RQアイテム追加
    _test_rq_item1 = ae.render_queue.add_comp(_test_comp1)
    _test_rq_item2 = ae.render_queue.add_comp(_test_comp2)

@suite.teardown
def teardown():
    """テスト環境のクリーンアップ"""
    global _test_comp1, _test_comp2, _test_rq_item1, _test_rq_item2

    # コンポジションを削除すると、関連するRQアイテムも自動削除される
    # そのため、先にコンポジションを削除し、RQアイテムの明示的削除は行わない
    if _test_comp1:
        _test_comp1.delete()
    if _test_comp2:
        _test_comp2.delete()

    # RQアイテムはコンポジション削除時に自動削除されるため、明示的削除は不要
    # (明示的に削除すると、すでに削除されたハンドルにアクセスしてメモリエラーが発生する可能性がある)

# =============================================================================
# RQItem Tests
# =============================================================================

@suite.test
def test_get_next_rq_item():
    """次のRQアイテムの取得"""
    global _test_rq_item1

    try:
        # Get first RQ item (pass 0)
        first_item = ae.sdk.AEGP_GetNextRQItem(0)
        assert_isinstance(first_item, int, "First RQ item should be an integer handle")
        assert_true(first_item != 0, "First RQ item should not be null")

        # Get next item
        next_item = ae.sdk.AEGP_GetNextRQItem(first_item)
        # next_item can be 0 if there's only one item, or a valid handle if there are more

    except Exception as e:
        print(f"Get next RQ item test failed: {e}")
        raise

@suite.test
def test_started_time():
    """レンダー開始時刻の取得"""
    global _test_rq_item1

    try:
        # Get started time (should be 0 if not started)
        started_time = ae.sdk.AEGP_GetStartedTime(_test_rq_item1._handle)
        assert_isinstance(started_time, float, "Started time should be a float")
        assert_equal(started_time, 0.0, "Started time should be 0.0 if not started")

    except Exception as e:
        print(f"Started time test failed: {e}")
        raise

@suite.test
def test_elapsed_time():
    """レンダー経過時間の取得"""
    global _test_rq_item1

    try:
        # Get elapsed time (should be 0 if not rendered)
        elapsed_time = ae.sdk.AEGP_GetElapsedTime(_test_rq_item1._handle)
        assert_isinstance(elapsed_time, float, "Elapsed time should be a float")
        assert_equal(elapsed_time, 0.0, "Elapsed time should be 0.0 if not rendered")

    except Exception as e:
        print(f"Elapsed time test failed: {e}")
        raise

@suite.test
def test_log_type():
    """ログタイプの取得・設定"""
    global _test_rq_item1

    try:
        # Get default log type
        log_type = ae.sdk.AEGP_GetLogType(_test_rq_item1._handle)
        assert_isinstance(log_type, int, "Log type should be an integer")

        # Set log type
        ae.sdk.AEGP_SetLogType(_test_rq_item1._handle, 1)

        # Verify
        new_log_type = ae.sdk.AEGP_GetLogType(_test_rq_item1._handle)
        assert_equal(new_log_type, 1, "Log type should be set to 1")

    except Exception as e:
        print(f"Log type test failed: {e}")
        raise

@suite.test
def test_comment():
    """コメントの取得・設定"""
    global _test_rq_item1

    try:
        # Get default comment
        comment = ae.sdk.AEGP_GetComment(_test_rq_item1._handle)
        assert_isinstance(comment, str, "Comment should be a string")

        # Set comment
        test_comment = "Test Comment for RQ Item"
        ae.sdk.AEGP_SetComment(_test_rq_item1._handle, test_comment)

        # Verify
        new_comment = ae.sdk.AEGP_GetComment(_test_rq_item1._handle)
        assert_equal(new_comment, test_comment, f"Comment should be '{test_comment}'")

    except Exception as e:
        print(f"Comment test failed: {e}")
        raise

@suite.test
def test_remove_output_module():
    """Output Moduleの削除"""
    global _test_rq_item1

    try:
        # Add output module
        outmod = ae.sdk.AEGP_AddDefaultOutputModule(_test_rq_item1._handle)
        assert_isinstance(outmod, int, "Output module should be an integer handle")

        # Get initial count
        initial_count = ae.sdk.AEGP_GetNumOutputModulesForRQItem(_test_rq_item1._handle)

        # Remove output module
        ae.sdk.AEGP_RemoveOutputModule(_test_rq_item1._handle, outmod)

        # Verify count decreased
        new_count = ae.sdk.AEGP_GetNumOutputModulesForRQItem(_test_rq_item1._handle)
        assert_equal(new_count, initial_count - 1, "Output module count should decrease by 1")

    except Exception as e:
        print(f"Remove output module test failed: {e}")
        raise

# =============================================================================
# テスト実行
# =============================================================================

def run():
    """すべてのテストを実行"""
    return suite.run()
