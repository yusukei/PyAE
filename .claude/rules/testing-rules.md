---
description: Testing and completion criteria
globs: ["**/*.py", "**/test_*.py", "**/*_test.py"]
alwaysApply: true
---

# Testing Rules

## Testing and Completion Criteria

1. **Tests must pass for completion** - A task or phase is only considered complete when all associated tests pass successfully.
2. **No completion reports without passing tests** - Never report a task as complete if tests have not been run and passed.
3. **Meaningful tests only** - Tests must validate actual functionality. Suppressing errors or modifying tests to artificially pass is strictly prohibited.
4. **Test integrity** - If a test fails, fix the underlying issue, not the test (unless the test itself is incorrect).
5. **Verification is mandatory** - All implementations must be verified through proper testing before being marked complete.

## Test Code Guidelines

1. **No try-except in test code** - Test code must NOT use try-except blocks, except in tests that specifically verify error handling behavior.
2. **Let tests fail naturally** - Tests should fail with clear error messages, not be suppressed by try-except.
3. **Exception testing only** - Only use try-except when testing that a specific error is raised as expected.

**Examples:**

```python
# ❌ Bad: Suppressing errors in tests
def test_something():
    try:
        result = api.do_something()
        assert result == expected
    except Exception as e:
        print(f"Error: {e}")  # Tests should fail, not print errors

# ✅ Good: Let the test fail naturally
def test_something():
    result = api.do_something()
    assert result == expected

# ✅ Good: Testing error handling
def test_error_handling():
    with pytest.raises(ValueError):
        api.do_invalid_operation()
```

## Debugging Guidelines

1. **Follow established procedures** - Always adhere to the procedures defined in guides when troubleshooting issues.
2. **Temporary workarounds are not solutions** - Bypassing procedures for debugging is acceptable temporarily, but only solutions that follow the established procedures are considered successful.
3. **Document deviations** - If a procedure needs modification, update the guide rather than silently ignoring it.
