# test_utils.py
# PyAE テストユーティリティ

import traceback
from typing import List, Callable, Any, Optional
from functools import wraps


# =============================================================
# Skip Test Support
# =============================================================

class SkipTestError(Exception):
    """テストスキップ用の例外"""
    pass


def skip(reason: str):
    """テストをスキップする"""
    raise SkipTestError(reason)


def skip_if(condition: bool, reason: str):
    """条件付きでテストをスキップする"""
    if condition:
        raise SkipTestError(reason)


def requires_light_suite_v3(func: Callable) -> Callable:
    """LightSuite V3が必要なテストのデコレータ"""
    @wraps(func)
    def wrapper(*args, **kwargs):
        try:
            import ae.sdk as sdk
            if not getattr(sdk, 'HAS_LIGHT_SUITE_V3', False):
                skip("Requires LightSuite V3 (AE 24.4+)")
        except ImportError:
            skip("SDK module not available")
        return func(*args, **kwargs)
    return wrapper


def requires_comp_suite_v12(func: Callable) -> Callable:
    """CompSuite V12が必要なテストのデコレータ"""
    @wraps(func)
    def wrapper(*args, **kwargs):
        try:
            import ae.sdk as sdk
            if not getattr(sdk, 'HAS_COMP_SUITE_V12', False):
                skip("Requires CompSuite V12 (AE 24.0+)")
        except ImportError:
            skip("SDK module not available")
        return func(*args, **kwargs)
    return wrapper


class TestResult:
    """テスト結果を保持するクラス"""

    def __init__(self, name: str, passed: bool, message: str = "", error: str = "", skipped: bool = False):
        self.name = name
        self.passed = passed
        self.message = message
        self.error = error
        self.skipped = skipped

    def __str__(self):
        if self.skipped:
            status = "SKIP"
        elif self.passed:
            status = "PASS"
        else:
            status = "FAIL"
        result = f"[{status}] {self.name}"
        if self.message:
            result += f": {self.message}"
        if self.error:
            result += f"\n  Error: {self.error}"
        return result

    def to_dict(self):
        """JSON シリアライズ可能な辞書に変換"""
        return {
            "name": self.name,
            "passed": self.passed,
            "skipped": self.skipped,
            "message": self.message,
            "error": self.error,
        }


class TestSuite:
    """テストスイートクラス"""

    def __init__(self, name: str):
        self.name = name
        self.tests: List[Callable] = []
        self.results: List[TestResult] = []
        self.setup_func: Optional[Callable] = None
        self.teardown_func: Optional[Callable] = None

    def setup(self, func: Callable):
        """セットアップ関数を登録"""
        self.setup_func = func
        return func

    def teardown(self, func: Callable):
        """ティアダウン関数を登録"""
        self.teardown_func = func
        return func

    def test(self, func: Callable):
        """テスト関数を登録するデコレータ"""
        self.tests.append(func)
        return func

    def run(self) -> List[TestResult]:
        """全テストを実行"""
        self.results = []
        print(f"\n{'='*60}")
        print(f"Test Suite: {self.name}")
        print(f"{'='*60}")

        # Setup
        if self.setup_func:
            try:
                self.setup_func()
                print(f"Setup completed")
            except Exception as e:
                print(f"Setup failed: {e}")
                return self.results

        # Run tests
        for test_func in self.tests:
            result = run_test(test_func)
            self.results.append(result)
            print(result)

        # Teardown
        if self.teardown_func:
            try:
                self.teardown_func()
                print(f"Teardown completed")
            except Exception as e:
                print(f"Teardown failed: {e}")

        # Summary
        passed = sum(1 for r in self.results if r.passed and not r.skipped)
        skipped = sum(1 for r in self.results if r.skipped)
        failed = sum(1 for r in self.results if not r.passed)
        total = len(self.results)
        print(f"\n{'-'*60}")
        summary_parts = [f"{passed} passed"]
        if skipped > 0:
            summary_parts.append(f"{skipped} skipped")
        if failed > 0:
            summary_parts.append(f"{failed} failed")
        print(f"Results: {', '.join(summary_parts)} / {total} total")

        return self.results


def run_test(test_func: Callable) -> TestResult:
    """単一のテスト関数を実行"""
    name = test_func.__name__
    try:
        test_func()
        return TestResult(name, True)
    except SkipTestError as e:
        return TestResult(name, True, message=str(e), skipped=True)
    except AssertionError as e:
        return TestResult(name, False, str(e))
    except Exception as e:
        return TestResult(name, False, error=f"{type(e).__name__}: {e}\n{traceback.format_exc()}")


def assert_true(condition: bool, message: str = ""):
    """条件がTrueであることを確認"""
    if not condition:
        raise AssertionError(message or "Expected True but got False")


def assert_false(condition: bool, message: str = ""):
    """条件がFalseであることを確認"""
    if condition:
        raise AssertionError(message or "Expected False but got True")


def assert_equal(expected: Any, actual: Any, message: str = ""):
    """値が等しいことを確認"""
    if expected != actual:
        raise AssertionError(message or f"Expected {expected!r} but got {actual!r}")


def assert_not_equal(expected: Any, actual: Any, message: str = ""):
    """値が等しくないことを確認"""
    if expected == actual:
        raise AssertionError(message or f"Expected values to differ but both are {expected!r}")


def assert_not_none(value: Any, message: str = ""):
    """値がNoneでないことを確認"""
    if value is None:
        raise AssertionError(message or "Expected non-None value but got None")


def assert_none(value: Any, message: str = ""):
    """値がNoneであることを確認"""
    if value is not None:
        raise AssertionError(message or f"Expected None but got {value!r}")


def assert_raises(exception_type: type, func: Callable, *args, **kwargs):
    """例外が発生することを確認"""
    try:
        func(*args, **kwargs)
        raise AssertionError(f"Expected {exception_type.__name__} but no exception was raised")
    except exception_type:
        pass  # Expected


def assert_in(item: Any, container: Any, message: str = ""):
    """アイテムがコンテナに含まれることを確認"""
    if item not in container:
        raise AssertionError(message or f"Expected {item!r} in {container!r}")


def assert_isinstance(obj: Any, expected_type: type, message: str = ""):
    """オブジェクトが指定した型であることを確認"""
    if not isinstance(obj, expected_type):
        raise AssertionError(
            message or f"Expected instance of {expected_type.__name__} but got {type(obj).__name__}"
        )


def assert_close(expected: float, actual: float, tolerance: float = 0.001, message: str = ""):
    """浮動小数点値が近いことを確認"""
    if abs(expected - actual) > tolerance:
        raise AssertionError(
            message or f"Expected {expected} (±{tolerance}) but got {actual}"
        )


def assert_greater_than(value: Any, threshold: Any, message: str = ""):
    """値がしきい値より大きいことを確認"""
    if not value > threshold:
        raise AssertionError(
            message or f"Expected {value} > {threshold}"
        )


def assert_less_than(value: Any, threshold: Any, message: str = ""):
    """値がしきい値より小さいことを確認"""
    if not value < threshold:
        raise AssertionError(
            message or f"Expected {value} < {threshold}"
        )
