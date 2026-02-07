# test_serialize_testeffect.py
# TestEffectを含むコンプをシリアライズしてJSONに保存

import ae
import json
import os
import struct
import traceback

from export_scene import export_comp, export_effect, export_param


def main():
    ae.log_info("=" * 60)
    ae.log_info("TestEffect Serialize Test")
    ae.log_info("=" * 60)

    proj = ae.Project.get_current()
    comp = proj.create_comp("_SerializeTestComp", 1920, 1080, 1.0, 5.0, 30.0)
    layer = comp.add_solid("_TestSolid", 1920, 1080, (1.0, 0.0, 0.0), 5.0)

    # Add TestEffect
    try:
        effect = layer.add_effect("PYAE TestEffect")
        if effect.match_name != "PYAE TestEffect":
            ae.log_error(f"Wrong effect: {effect.match_name}")
            effect.delete()
            comp.delete()
            return
    except Exception as e:
        ae.log_error(f"TestEffect not available: {e}")
        comp.delete()
        return

    ae.log_info(f"Effect added: {effect.name} ({effect.match_name})")
    ae.log_info(f"Num params: {effect.num_properties}")

    # Serialize the entire comp
    comp_data = export_comp(comp)

    # Also serialize just the effect
    effect_data = export_effect(effect)

    # Arbitrary Data param details
    arb_detail = {}
    try:
        param = effect.property("Custom Data")
        raw = param.value
        arb_detail["raw_type"] = str(type(raw))
        arb_detail["raw_hex"] = raw.hex() if isinstance(raw, bytes) else str(raw)
        arb_detail["raw_len"] = len(raw) if isinstance(raw, bytes) else 0

        # Parse the struct
        ARB_FORMAT = '<4diB3x'
        if len(raw) == struct.calcsize(ARB_FORMAT):
            data = struct.unpack(ARB_FORMAT, raw)
            arb_detail["parsed"] = {
                "values": list(data[:4]),
                "mode": data[4],
                "enabled": data[5],
            }

        # get_raw_bytes
        raw_bytes = param.get_raw_bytes(0.0)
        arb_detail["get_raw_bytes_hex"] = raw_bytes.hex() if isinstance(raw_bytes, bytes) else str(raw_bytes)
        arb_detail["get_raw_bytes_len"] = len(raw_bytes) if isinstance(raw_bytes, bytes) else 0
    except Exception as e:
        arb_detail["error"] = str(e)
        ae.log_error(f"Arb detail error: {e}")

    # Build output
    output = {
        "comp_serialized": comp_data,
        "effect_serialized": effect_data,
        "arbitrary_data_detail": arb_detail,
    }

    # Save to file
    docs_path = ae.get_documents_folder()
    out_file = os.path.join(docs_path, "testeffect_serialize.json")

    with open(out_file, "w", encoding="utf-8") as f:
        json.dump(output, f, indent=2, ensure_ascii=False, default=str)

    ae.log_info(f"Saved to: {out_file}")
    file_size = os.path.getsize(out_file)
    ae.log_info(f"File size: {file_size} bytes")

    # Cleanup
    comp.delete()
    ae.log_info("Cleanup done")
    ae.log_info("=" * 60)

    return out_file


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        ae.log_error(f"FATAL: {e}")
        ae.log_error(traceback.format_exc())
