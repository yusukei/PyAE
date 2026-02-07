# test_async_render.py
# Tests for ae.async_render high-level API
#
# IMPORTANT LIMITATION:
# These functions can ONLY be used inside PF_Event_DRAW context
# with PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER flag set.
# Therefore, unit tests for the actual rendering functions are limited.

import ae

try:
    from ..test_utils import (
        TestSuite, assert_true, assert_false, assert_equal,
        assert_not_none, assert_none, assert_isinstance,
        assert_close, assert_raises, skip,
    )
except ImportError:
    from test_utils import (
        TestSuite, assert_true, assert_false, assert_equal,
        assert_not_none, assert_none, assert_isinstance,
        assert_close, assert_raises, skip,
    )

suite = TestSuite("AsyncRender API")


# -----------------------------------------------------------------------
# Module Availability Tests
# -----------------------------------------------------------------------

@suite.test
def test_module_exists():
    """Test that async_render module exists"""
    assert_true(hasattr(ae, 'async_render'),
                "ae should have 'async_render' module")


@suite.test
def test_module_docstring():
    """Test that module has documentation"""
    assert_not_none(ae.async_render.__doc__)
    assert_true(len(ae.async_render.__doc__) > 0,
                "Module docstring should not be empty")


@suite.test
def test_render_item_frame_exists():
    """Test that render_item_frame function exists"""
    assert_true(hasattr(ae.async_render, 'render_item_frame'),
                "async_render should have 'render_item_frame'")


@suite.test
def test_render_layer_frame_exists():
    """Test that render_layer_frame function exists"""
    assert_true(hasattr(ae.async_render, 'render_layer_frame'),
                "async_render should have 'render_layer_frame'")


# -----------------------------------------------------------------------
# Input Validation Tests
#
# Note: These tests verify that the API validates inputs correctly.
# Actual rendering requires a valid async_manager from PF_Event_DRAW.
# -----------------------------------------------------------------------

@suite.test
def test_render_item_frame_null_manager():
    """Test that null async_manager is rejected"""
    # async_manager=0 should raise error
    assert_raises(
        RuntimeError,
        ae.async_render.render_item_frame,
        async_manager=0,
        purpose_id=1,
        render_options=0,
    )


@suite.test
def test_render_layer_frame_null_manager():
    """Test that null async_manager is rejected"""
    # async_manager=0 should raise error
    assert_raises(
        RuntimeError,
        ae.async_render.render_layer_frame,
        async_manager=0,
        purpose_id=1,
        layer_render_options=0,
    )


# -----------------------------------------------------------------------
# Documentation Tests
# -----------------------------------------------------------------------

@suite.test
def test_item_frame_doc():
    """Test render_item_frame has useful docstring"""
    doc = ae.async_render.render_item_frame.__doc__
    assert_not_none(doc)
    # Should mention the PF_Event_DRAW requirement
    assert_true("PF_Event_DRAW" in doc or "Event" in doc,
                "Docstring should mention PF_Event_DRAW or Event context")


@suite.test
def test_layer_frame_doc():
    """Test render_layer_frame has useful docstring"""
    doc = ae.async_render.render_layer_frame.__doc__
    assert_not_none(doc)
    # Should mention the PF_Event_DRAW requirement
    assert_true("PF_Event_DRAW" in doc or "Event" in doc,
                "Docstring should mention PF_Event_DRAW or Event context")


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
