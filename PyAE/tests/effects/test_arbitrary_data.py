# test_arbitrary_data.py
# PyAE TestEffect Arbitrary Data テスト

import struct
import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_equal,
        assert_not_none,
        assert_isinstance,
        assert_close,
        skip,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_equal,
        assert_not_none,
        assert_isinstance,
        assert_close,
        skip,
    )

suite = TestSuite("Arbitrary Data")

# TestFX_ArbData binary format (little-endian, Windows x64)
# 4 doubles (A_FpLong) + 1 int32 (A_long) + 1 byte (A_Boolean) + 3 padding
ARB_FORMAT = '<4diB3x'
ARB_SIZE = struct.calcsize(ARB_FORMAT)  # 40 bytes

# Default values from TestEffect_Arb_Handler.cpp
DEFAULT_VALUES = (1.0, 2.0, 3.0, 4.0)
DEFAULT_MODE = 1
DEFAULT_ENABLED = 1  # TRUE

# Test state
_test_comp = None
_test_layer = None
_test_effect = None


def _skip_if_no_effect():
    """TestEffect not available - skip test"""
    if _test_effect is None:
        skip("TestEffect plugin not installed")


@suite.setup
def setup():
    """Create test comp with TestEffect applied"""
    global _test_comp, _test_layer, _test_effect
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_ArbDataTestComp", 1920, 1080, 1.0, 5.0, 30.0)
    _test_layer = _test_comp.add_solid(
        "_ArbDataTestLayer", 1920, 1080, (1.0, 0.0, 0.0), 5.0
    )
    # List installed effects to check if TestEffect is loaded
    try:
        effects_list = ae.get_installed_effects()
        pyae_effects = [e for e in effects_list if 'PYAE' in e.get('match_name', '') or 'Test' in e.get('match_name', '')]
        print(f"[DEBUG] Found {len(effects_list)} installed effects, PyAE-related: {pyae_effects}")
    except Exception as e:
        print(f"[DEBUG] Could not list effects: {e}")

    # Add TestEffect by match_name
    try:
        effect = _test_layer.add_effect("PYAE TestEffect")
        # Verify the correct effect was added
        if effect.match_name == "PYAE TestEffect":
            _test_effect = effect
        else:
            print(f"[DEBUG] Wrong effect: match_name='{effect.match_name}', name='{effect.name}'")
            effect.delete()
            _test_effect = None
    except Exception as e:
        print(f"[DEBUG] add_effect failed: {type(e).__name__}: {e}")
        _test_effect = None


@suite.teardown
def teardown():
    """Clean up test comp"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except Exception:
        pass


@suite.test
def test_testeffect_added():
    """TestEffectプラグインが追加できる"""
    _skip_if_no_effect()
    assert_not_none(_test_effect, "TestEffect should be added")
    assert_true(
        "Test" in _test_effect.name or "PyAE" in _test_effect.name,
        f"Effect name should contain 'Test' or 'PyAE', got: {_test_effect.name}"
    )


@suite.test
def test_arbitrary_param_exists():
    """Arbitrary Dataパラメータが取得できる"""
    _skip_if_no_effect()
    # Diagnostic: list all params by index
    num = _test_effect.num_properties
    print(f"[DEBUG] num_params={num}")
    arb_param = None
    for i in range(num):
        p = _test_effect.property_by_index(i)
        pname = p.name
        print(f"[DEBUG] param[{i}]: name='{pname}', valid={p.valid}")
        if pname == "Custom Data":
            arb_param = p
    if arb_param is not None:
        assert_true(True, "Found Custom Data by index scan")
    else:
        # Try by name (may throw)
        param = _test_effect.property("Custom Data")
        assert_not_none(param, "Custom Data parameter should exist")


@suite.test
def test_arbitrary_value_is_bytes():
    """Arbitrary Dataの値がbytes型で返る"""
    _skip_if_no_effect()
    param = _test_effect.property("Custom Data")
    value = param.value
    assert_isinstance(value, bytes, "Arbitrary data value should be bytes")


@suite.test
def test_arbitrary_data_size():
    """Arbitrary Dataのサイズが正しい（40バイト）"""
    _skip_if_no_effect()
    param = _test_effect.property("Custom Data")
    value = param.value
    assert_equal(
        len(value), ARB_SIZE,
        f"Arbitrary data size should be {ARB_SIZE} bytes, got {len(value)}"
    )


@suite.test
def test_arbitrary_default_values():
    """Arbitrary Dataのデフォルト値が正しい"""
    _skip_if_no_effect()
    param = _test_effect.property("Custom Data")
    raw = param.value
    assert_equal(len(raw), ARB_SIZE, "Data size mismatch")

    data = struct.unpack(ARB_FORMAT, raw)
    # data = (v0, v1, v2, v3, mode, enabled)
    for i in range(4):
        assert_close(
            data[i], DEFAULT_VALUES[i], tolerance=0.0001,
            message=f"values[{i}] should be {DEFAULT_VALUES[i]}, got {data[i]}"
        )
    assert_equal(data[4], DEFAULT_MODE, f"mode should be {DEFAULT_MODE}, got {data[4]}")
    assert_equal(data[5], DEFAULT_ENABLED, f"enabled should be {DEFAULT_ENABLED}, got {data[5]}")



@suite.test
def test_arbitrary_set_value():
    """Arbitrary Dataの値を設定できる"""
    _skip_if_no_effect()
    param = _test_effect.property("Custom Data")

    # Save original
    original = param.value

    # Create new data: different values
    new_values = (10.0, 20.0, 30.0, 40.0)
    new_mode = 2
    new_enabled = 0  # FALSE
    new_data = struct.pack(ARB_FORMAT, *new_values, new_mode, new_enabled)

    # Set new value
    param.value = new_data

    # Read back
    read_back = param.value
    assert_equal(len(read_back), ARB_SIZE, "Read-back size should match")

    parsed = struct.unpack(ARB_FORMAT, read_back)
    for i in range(4):
        assert_close(
            parsed[i], new_values[i], tolerance=0.0001,
            message=f"Set values[{i}] should be {new_values[i]}, got {parsed[i]}"
        )
    assert_equal(parsed[4], new_mode, f"Set mode should be {new_mode}, got {parsed[4]}")
    assert_equal(parsed[5], new_enabled, f"Set enabled should be {new_enabled}, got {parsed[5]}")

    # Restore original
    param.value = original


@suite.test
def test_arbitrary_roundtrip():
    """Arbitrary Dataの読み書きラウンドトリップ"""
    _skip_if_no_effect()
    param = _test_effect.property("Custom Data")

    # Read original
    original = param.value
    original_parsed = struct.unpack(ARB_FORMAT, original)

    # Modify
    modified_values = (99.9, -1.5, 0.0, 42.0)
    modified_mode = 7
    modified_enabled = 1
    modified = struct.pack(ARB_FORMAT, *modified_values, modified_mode, modified_enabled)
    param.value = modified

    # Verify modification took effect
    read_modified = param.value
    assert_equal(read_modified, modified, "Modified data should persist")

    # Restore original
    param.value = original

    # Verify restoration
    read_restored = param.value
    restored_parsed = struct.unpack(ARB_FORMAT, read_restored)
    for i in range(4):
        assert_close(
            restored_parsed[i], original_parsed[i], tolerance=0.0001,
            message=f"Restored values[{i}] should match original"
        )
    assert_equal(restored_parsed[4], original_parsed[4], "Restored mode should match original")
    assert_equal(restored_parsed[5], original_parsed[5], "Restored enabled should match original")


@suite.test
def test_arbitrary_export_json():
    """TestEffectデータをJSON形式でエクスポート"""
    import json
    import os
    _skip_if_no_effect()

    # Build effect param data
    params_data = []
    num_params = _test_effect.num_properties
    for i in range(num_params):
        param = _test_effect.property_by_index(i)
        if param and param.valid:
            val = param.value
            if isinstance(val, bytes):
                bdata = val.hex()
            elif isinstance(val, (int, float)):
                bdata = struct.pack('>d', float(val)).hex()
            elif isinstance(val, (list, tuple)):
                bdata = b''.join(struct.pack('>d', float(v)) for v in val).hex()
            elif val is None:
                bdata = ""
            else:
                bdata = str(val)
            params_data.append({
                "name": param.name,
                "index": param.index,
                "bdata": bdata,
                "value_repr": repr(val),
            })

    export_json = {
        "version": 1,
        "description": "TestEffect with Arbitrary Data",
        "comp": {
            "name": _test_comp.name,
            "width": _test_comp.width,
            "height": _test_comp.height,
            "frame_rate": _test_comp.frame_rate,
            "duration": _test_comp.duration,
        },
        "layer": {
            "name": _test_layer.name,
            "index": _test_layer.index,
        },
        "effect": {
            "name": _test_effect.name,
            "match_name": _test_effect.match_name,
            "enabled": _test_effect.enabled,
            "num_params": num_params,
            "params": params_data,
        },
    }

    json_str = json.dumps(export_json, indent=2, ensure_ascii=False)

    # Write to file
    docs_path = ae.get_documents_folder()
    output_path = os.path.join(docs_path, "testeffect_arb_export.json")
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(json_str)

    # Print JSON to log
    print(f"\n{'='*60}")
    print(f"TestEffect ARB Export JSON:")
    print(f"{'='*60}")
    print(json_str)
    print(f"{'='*60}")
    print(f"Output file: {output_path}")
    print(f"{'='*60}")
    assert_true(True, "JSON exported successfully")


def run():
    """Run tests"""
    return suite.run()


if __name__ == "__main__":
    run()
