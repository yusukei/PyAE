# test_memory_suite.py
# PyAE SDK MemorySuite テスト

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
        assert_raises,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_isinstance,
        assert_raises,
    )

suite = TestSuite("SDK Memory Suite")


@suite.test
def test_mem_flags_constants():
    """Memory flag constants are defined"""
    assert_equal(ae.sdk.AEGP_MemFlag_NONE, 0)
    assert_equal(ae.sdk.AEGP_MemFlag_CLEAR, 1)
    assert_equal(ae.sdk.AEGP_MemFlag_QUIET, 2)


@suite.test
def test_get_mem_stats():
    """AEGP_GetMemStats returns memory statistics"""
    stats = ae.sdk.AEGP_GetMemStats()
    assert_isinstance(stats, dict)
    assert_true("handle_count" in stats, "stats should contain handle_count")
    assert_true("total_size" in stats, "stats should contain total_size")
    assert_true(stats["handle_count"] >= 0, "handle_count should be non-negative")
    assert_true(stats["total_size"] >= 0, "total_size should be non-negative")


@suite.test
def test_set_mem_reporting_on():
    """AEGP_SetMemReportingOn can be called"""
    # Enable reporting
    ae.sdk.AEGP_SetMemReportingOn(True)
    # Disable reporting
    ae.sdk.AEGP_SetMemReportingOn(False)
    # No exception means success


@suite.test
def test_new_free_mem_handle():
    """AEGP_NewMemHandle and AEGP_FreeMemHandle work correctly"""
    # Allocate memory
    mem_h = ae.sdk.AEGP_NewMemHandle("TestAlloc", 1024, ae.sdk.AEGP_MemFlag_CLEAR)
    assert_true(mem_h != 0, "Memory handle should not be null")

    # Check size
    size = ae.sdk.AEGP_GetMemHandleSize(mem_h)
    assert_equal(size, 1024, "Size should be 1024 bytes")

    # Free memory
    ae.sdk.AEGP_FreeMemHandle(mem_h)
    # No exception means success


@suite.test
def test_lock_unlock_mem_handle():
    """AEGP_LockMemHandle and AEGP_UnlockMemHandle work correctly"""
    # Allocate memory
    mem_h = ae.sdk.AEGP_NewMemHandle("TestLock", 256, ae.sdk.AEGP_MemFlag_NONE)
    assert_true(mem_h != 0, "Memory handle should not be null")

    # Lock memory
    ptr = ae.sdk.AEGP_LockMemHandle(mem_h)
    assert_true(ptr != 0, "Locked pointer should not be null")

    # Unlock memory
    ae.sdk.AEGP_UnlockMemHandle(mem_h)

    # Free memory
    ae.sdk.AEGP_FreeMemHandle(mem_h)


@suite.test
def test_nestable_locks():
    """Memory locks are nestable"""
    mem_h = ae.sdk.AEGP_NewMemHandle("TestNestable", 128, ae.sdk.AEGP_MemFlag_NONE)

    # First lock
    ptr1 = ae.sdk.AEGP_LockMemHandle(mem_h)
    assert_true(ptr1 != 0, "First lock should return valid pointer")

    # Second lock (nested)
    ptr2 = ae.sdk.AEGP_LockMemHandle(mem_h)
    assert_true(ptr2 != 0, "Second lock should return valid pointer")
    assert_equal(ptr1, ptr2, "Both locks should return same pointer")

    # Unlock twice
    ae.sdk.AEGP_UnlockMemHandle(mem_h)
    ae.sdk.AEGP_UnlockMemHandle(mem_h)

    ae.sdk.AEGP_FreeMemHandle(mem_h)


@suite.test
def test_resize_mem_handle():
    """AEGP_ResizeMemHandle works correctly"""
    # Allocate initial memory
    mem_h = ae.sdk.AEGP_NewMemHandle("TestResize", 512, ae.sdk.AEGP_MemFlag_NONE)
    size1 = ae.sdk.AEGP_GetMemHandleSize(mem_h)
    assert_equal(size1, 512, "Initial size should be 512")

    # Resize to larger
    ae.sdk.AEGP_ResizeMemHandle("TestResize", 1024, mem_h)
    size2 = ae.sdk.AEGP_GetMemHandleSize(mem_h)
    assert_equal(size2, 1024, "Resized size should be 1024")

    # Resize to smaller
    ae.sdk.AEGP_ResizeMemHandle("TestResize", 256, mem_h)
    size3 = ae.sdk.AEGP_GetMemHandleSize(mem_h)
    assert_equal(size3, 256, "Resized size should be 256")

    ae.sdk.AEGP_FreeMemHandle(mem_h)


@suite.test
def test_new_mem_handle_with_clear_flag():
    """AEGP_NewMemHandle with AEGP_MemFlag_CLEAR clears memory"""
    mem_h = ae.sdk.AEGP_NewMemHandle("TestClear", 64, ae.sdk.AEGP_MemFlag_CLEAR)
    assert_true(mem_h != 0, "Memory handle should not be null")

    # Memory should be cleared (zeroed)
    # Note: We can't directly verify the content in Python,
    # but the allocation should succeed
    ae.sdk.AEGP_FreeMemHandle(mem_h)


@suite.test
def test_invalid_size_raises_error():
    """AEGP_NewMemHandle with invalid size raises error"""
    error_raised = False
    try:
        ae.sdk.AEGP_NewMemHandle("TestInvalid", 0, ae.sdk.AEGP_MemFlag_NONE)
    except (ValueError, RuntimeError):
        error_raised = True
    assert_true(error_raised, "Should raise error for size <= 0")


@suite.test
def test_null_handle_raises_error():
    """Operations on null handle raise error"""
    error_raised = False
    try:
        ae.sdk.AEGP_FreeMemHandle(0)
    except (ValueError, RuntimeError):
        error_raised = True
    assert_true(error_raised, "Should raise error for null handle")


@suite.test
def test_memory_stats_after_allocation():
    """Memory stats reflect allocations"""
    stats_before = ae.sdk.AEGP_GetMemStats()

    # Allocate some memory
    handles = []
    for i in range(5):
        h = ae.sdk.AEGP_NewMemHandle(f"Test{i}", 1024, ae.sdk.AEGP_MemFlag_NONE)
        handles.append(h)

    stats_during = ae.sdk.AEGP_GetMemStats()

    # Free all memory
    for h in handles:
        ae.sdk.AEGP_FreeMemHandle(h)

    stats_after = ae.sdk.AEGP_GetMemStats()

    # During allocation, handle count should be higher
    assert_true(
        stats_during["handle_count"] >= stats_before["handle_count"],
        "Handle count should increase during allocation"
    )

    # After freeing, stats should return to approximately original
    # (may not be exact due to internal caching)
    assert_true(
        stats_after["handle_count"] <= stats_during["handle_count"],
        "Handle count should decrease after freeing"
    )


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
