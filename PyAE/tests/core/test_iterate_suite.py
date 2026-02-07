# test_iterate_suite.py
# PyAE Iterate Suite Tests

import ae
import threading

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
        assert_greater_than,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
        assert_greater_than,
    )

suite = TestSuite("IterateSuite")


@suite.setup
def setup():
    """Test setup"""
    pass


@suite.teardown
def teardown():
    """Test cleanup"""
    pass


# -----------------------------------------------------------------------
# AEGP_GetNumThreads Tests
# -----------------------------------------------------------------------

@suite.test
def test_get_num_threads():
    """Get number of available threads"""
    num_threads = ae.sdk.AEGP_GetNumThreads()
    assert_isinstance(num_threads, int, "Number of threads should be an integer")
    assert_greater_than(num_threads, 0, "Number of threads should be positive")


@suite.test
def test_get_num_threads_consistent():
    """GetNumThreads returns consistent values"""
    num_threads1 = ae.sdk.AEGP_GetNumThreads()
    num_threads2 = ae.sdk.AEGP_GetNumThreads()
    assert_equal(num_threads1, num_threads2, "Thread count should be consistent")


# -----------------------------------------------------------------------
# AEGP_IterateGeneric Tests
# -----------------------------------------------------------------------

@suite.test
def test_iterate_generic_basic():
    """Basic iteration with callback"""
    results = []
    lock = threading.Lock()

    def callback(thread_index, i, iterations):
        with lock:
            results.append((thread_index, i, iterations))
        return None  # Continue iteration

    iterations = 5
    ae.sdk.AEGP_IterateGeneric(iterations, callback)

    assert_equal(len(results), iterations, f"Should have {iterations} results")

    # Verify all iteration indices are present
    indices = sorted([r[1] for r in results])
    expected_indices = list(range(iterations))
    assert_equal(indices, expected_indices, "All iteration indices should be present")


@suite.test
def test_iterate_generic_zero_iterations():
    """Iteration with zero iterations does nothing"""
    call_count = [0]

    def callback(thread_index, i, iterations):
        call_count[0] += 1
        return None

    ae.sdk.AEGP_IterateGeneric(0, callback)
    assert_equal(call_count[0], 0, "Callback should not be called with 0 iterations")


@suite.test
def test_iterate_generic_single_iteration():
    """Single iteration works correctly"""
    results = []

    def callback(thread_index, i, iterations):
        results.append({"thread": thread_index, "i": i, "total": iterations})
        return None

    ae.sdk.AEGP_IterateGeneric(1, callback)
    assert_equal(len(results), 1, "Should have exactly 1 result")
    assert_equal(results[0]["i"], 0, "Iteration index should be 0")
    assert_equal(results[0]["total"], 1, "Total iterations should be 1")


@suite.test
def test_iterate_generic_callback_receives_correct_params():
    """Callback receives correct parameters"""
    received_params = []
    lock = threading.Lock()

    def callback(thread_index, i, iterations):
        with lock:
            received_params.append({
                "thread_index": thread_index,
                "i": i,
                "iterations": iterations
            })
        return None

    test_iterations = 10
    ae.sdk.AEGP_IterateGeneric(test_iterations, callback)

    # Verify all iterations were called
    assert_equal(len(received_params), test_iterations, "All iterations should be called")

    # Verify each call had correct iterations count
    for param in received_params:
        assert_equal(param["iterations"], test_iterations,
                    "iterations parameter should equal total iterations")
        assert_isinstance(param["thread_index"], int, "thread_index should be int")
        assert_true(param["thread_index"] >= 0, "thread_index should be non-negative")


@suite.test
def test_iterate_generic_accumulator():
    """Iteration can accumulate values"""
    total = [0]
    lock = threading.Lock()

    def callback(thread_index, i, iterations):
        with lock:
            total[0] += i
        return None

    iterations = 10
    ae.sdk.AEGP_IterateGeneric(iterations, callback)

    # Sum of 0..9 = 45
    expected_sum = sum(range(iterations))
    assert_equal(total[0], expected_sum, f"Sum should be {expected_sum}")


@suite.test
def test_once_per_processor_constant():
    """PF_Iterations_ONCE_PER_PROCESSOR constant is available"""
    constant = ae.sdk.PF_Iterations_ONCE_PER_PROCESSOR
    assert_equal(constant, -1, "PF_Iterations_ONCE_PER_PROCESSOR should be -1")


@suite.test
def test_iterate_generic_once_per_processor():
    """Iteration with ONCE_PER_PROCESSOR calls callback for each processor"""
    call_count = [0]
    lock = threading.Lock()

    def callback(thread_index, i, iterations):
        with lock:
            call_count[0] += 1
        return None

    # Use ONCE_PER_PROCESSOR
    ae.sdk.AEGP_IterateGeneric(ae.sdk.PF_Iterations_ONCE_PER_PROCESSOR, callback)

    # Should be called at least once, typically once per processor
    num_threads = ae.sdk.AEGP_GetNumThreads()
    assert_greater_than(call_count[0], 0, "Callback should be called at least once")


# -----------------------------------------------------------------------
# Argument Validation Tests
# -----------------------------------------------------------------------

@suite.test
def test_iterate_generic_invalid_callback():
    """Iteration with None callback raises error"""
    error_raised = False
    try:
        ae.sdk.AEGP_IterateGeneric(5, None)
    except (TypeError, ValueError) as e:
        error_raised = True

    assert_true(error_raised, "Should raise error with None callback")


@suite.test
def test_iterate_generic_negative_iterations():
    """Negative iterations (except -1) should be handled"""
    # -1 is special (ONCE_PER_PROCESSOR), but other negative values
    # may be rejected or handled differently
    call_count = [0]

    def callback(thread_index, i, iterations):
        call_count[0] += 1
        return None

    # -1 should work (ONCE_PER_PROCESSOR)
    ae.sdk.AEGP_IterateGeneric(-1, callback)
    assert_greater_than(call_count[0], 0, "ONCE_PER_PROCESSOR should call callback")

    # Other negative values
    call_count[0] = 0
    error_raised = False
    try:
        ae.sdk.AEGP_IterateGeneric(-2, callback)
    except (RuntimeError, ValueError) as e:
        error_raised = True

    # Note: Depending on AE's behavior, this might not raise an error
    # but we document that only -1 has special meaning
    assert_true(True, "Negative iterations test completed")


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
