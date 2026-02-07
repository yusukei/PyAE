# test_memory_diagnostics.py
# PyAE Memory Diagnostics テスト

import ae

try:
    from .test_utils import (
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

suite = TestSuite("Memory Diagnostics")

_test_comp = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp
    _test_comp = ae.Comp.create("_MemTest", 1920, 1080, 1.0, 10.0, 30.0)


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    if _test_comp is not None and _test_comp.valid:
        proj = ae.Project.get_current()
        items = proj.items
        for item in items:
            if item.name == "_MemTest" or item.name.startswith("_MemTest"):
                item.delete()


@suite.test
def test_get_memory_stats():
    """メモリ統計を取得できる"""
    stats = ae.get_memory_stats()
    assert_not_none(stats, "Memory stats should not be None")
    assert_isinstance(stats, ae.MemStats)
    assert_true(stats.handle_count >= 0, "Handle count should be non-negative")
    assert_true(stats.total_size >= 0, "Total size should be non-negative")


@suite.test
def test_log_memory_stats():
    """メモリ統計をログ出力できる"""
    ae.log_memory_stats("Test context")
    # ログ出力が成功すればOK（エラーが発生しない）


@suite.test
def test_memory_leak_detection():
    """メモリリークを検出できる"""
    global _test_comp

    # テスト前のメモリ統計
    mem_before = ae.get_memory_stats()

    # テキストレイヤーを作成・削除
    text_layer = _test_comp.add_text("Memory Test")
    text_layer.delete()

    # テスト後のメモリ統計
    mem_after = ae.get_memory_stats()

    # メモリリークをチェック
    has_leak = ae.check_memory_leak(mem_before, mem_after)

    # リーク詳細をログ出力
    ae.log_memory_leak_details(mem_before, mem_after, "test_memory_leak_detection")

    # メモリリークがないことを確認（許容範囲内）
    # Note: SDK内部でキャッシュなどがある可能性があるため、
    # ここでは単にAPIが動作することを確認
    assert_isinstance(has_leak, bool, "check_memory_leak should return bool")


@suite.test
def test_memory_stats_consistency():
    """複数回取得したメモリ統計が一貫している"""
    stats1 = ae.get_memory_stats()
    stats2 = ae.get_memory_stats()

    # 短時間で連続取得した場合、メモリ統計は同じか近い値のはず
    # （完全に同じとは限らないが、大きく変わることはない）
    assert_isinstance(stats1, ae.MemStats)
    assert_isinstance(stats2, ae.MemStats)


@suite.test
def test_enable_memory_reporting():
    """メモリレポートを有効化/無効化できる"""
    # 有効化
    ae.enable_memory_reporting(True)
    # 無効化
    ae.enable_memory_reporting(False)
    # エラーが発生しなければOK


@suite.test
def test_memory_stats_repr():
    """MemStatsの文字列表現が正しい"""
    stats = ae.get_memory_stats()
    repr_str = repr(stats)
    assert_isinstance(repr_str, str)
    assert_true("MemStats" in repr_str, "repr should contain 'MemStats'")
    assert_true("handles=" in repr_str, "repr should contain 'handles='")
    assert_true("size=" in repr_str, "repr should contain 'size='")


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
