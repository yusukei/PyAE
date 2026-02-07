# PyAE Test Suite
# テストモジュールパッケージ
#
# 使用方法:
#   from tests import run_all_tests
#   results = run_all_tests()

from .test_utils import (
    TestResult,
    TestSuite,
    run_test,
    assert_true,
    assert_false,
    assert_equal,
    assert_not_equal,
    assert_not_none,
    assert_none,
    assert_raises,
    assert_in,
    assert_isinstance,
    assert_close,
    assert_greater_than,
    assert_less_than,
)

# テストモジュール
from . import test_project
from . import test_item
from . import test_comp
from . import test_layer
from . import test_effect
from . import test_mask
from . import test_render_queue
from . import test_3d

# テストランナー
from .test_runner import run_all_tests, run_quick_tests

__all__ = [
    # Utilities
    "TestResult",
    "TestSuite",
    "run_test",
    "assert_true",
    "assert_false",
    "assert_equal",
    "assert_not_equal",
    "assert_not_none",
    "assert_none",
    "assert_raises",
    "assert_in",
    "assert_isinstance",
    "assert_close",
    "assert_greater_than",
    "assert_less_than",
    # Test modules
    "test_project",
    "test_item",
    "test_comp",
    "test_layer",
    "test_effect",
    "test_mask",
    "test_render_queue",
    "test_3d",
    # Runners
    "run_all_tests",
    "run_quick_tests",
]
