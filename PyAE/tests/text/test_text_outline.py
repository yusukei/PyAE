# test_text_outline.py
# PyAE Text Outline テスト

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
        assert_close,
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
        assert_close,
    )

suite = TestSuite("Text Outline")

# テスト用変数
_test_comp = None
_test_text_layer = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp, _test_text_layer
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_TextOutlineTestComp", 1920, 1080, 1.0, 5.0, 30.0)
    _test_text_layer = _test_comp.add_text("Hello World")


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except:
        pass


@suite.test
def test_text_layer_created():
    """テキストレイヤーが作成できる"""
    global _test_text_layer
    assert_not_none(_test_text_layer, "Text layer should be created")


@suite.test
def test_get_text_outlines():
    """テキストアウトラインが取得できる"""
    global _test_text_layer
    try:
        # 低レベルAPIでテキストアウトラインを取得
        layer_handle = _test_text_layer._handle
        outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layer_handle)
        assert_not_none(outlinesH, "Text outlines handle should be returned")

        # アウトライン数を取得
        num_outlines = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)
        assert_true(num_outlines >= 0, "Number of outlines should be non-negative")

        # アウトラインを破棄
        ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

    except Exception as e:
        assert_true(False, f"Failed to get text outlines: {e}")


@suite.test
def test_get_indexed_text_outline():
    """インデックス指定でテキストアウトラインが取得できる"""
    global _test_text_layer
    try:
        # 低レベルAPIでテキストアウトラインを取得
        layer_handle = _test_text_layer._handle
        outlinesH = ae.sdk.AEGP_GetNewTextOutlines(layer_handle)

        # アウトライン数を取得
        num_outlines = ae.sdk.AEGP_GetNumTextOutlines(outlinesH)

        if num_outlines > 0:
            # 最初のアウトラインパスを取得
            pathH = ae.sdk.AEGP_GetIndexedTextOutline(outlinesH, 0)
            assert_not_none(pathH, "Text outline path should be returned")

            # 範囲外のインデックスでエラーになることを確認
            try:
                ae.sdk.AEGP_GetIndexedTextOutline(outlinesH, num_outlines)
                assert_true(False, "Should raise error for out of range index")
            except (RuntimeError, IndexError):
                # 期待通りエラーが発生
                pass

        # アウトラインを破棄
        ae.sdk.AEGP_DisposeTextOutlines(outlinesH)

    except Exception as e:
        assert_true(False, f"Failed to get indexed text outline: {e}")


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
