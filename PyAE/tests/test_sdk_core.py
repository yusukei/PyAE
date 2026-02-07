import ae
import ae.sdk
import pytest


def test_sdk_item_comp():
    """Test Item and Comp Suite bindings"""
    # Create Comp
    comp = ae.Comp.create("SDK Core Test", 1280, 720, 1.0, 5.0, 24.0)

    # 1. Item Suite Basics
    active_item_h = ae.sdk.AEGP_GetActiveItem()
    assert active_item_h != 0
    assert active_item_h == comp._handle

    plugin_id = ae.sdk.AEGP_GetPluginID()
    item_name = ae.sdk.AEGP_GetItemName(plugin_id, active_item_h)
    assert item_name == "SDK Core Test"

    item_type = ae.sdk.AEGP_GetItemType(active_item_h)
    # AEGP_ItemType_COMP is 4
    assert item_type == 4

    # Duration (5.0s)
    duration = ae.sdk.AEGP_GetItemDuration(active_item_h)
    assert abs(duration - 5.0) < 0.001

    # 2. Comp Suite Basics
    # Get Comp from Item
    comp_h = ae.sdk.AEGP_GetCompFromItem(active_item_h)
    assert comp_h != 0

    # Get Item from Comp
    item_h_back = ae.sdk.AEGP_GetItemFromComp(comp_h)
    assert item_h_back == active_item_h

    # Downsample Factor
    dsf = ae.sdk.AEGP_GetCompDownSampleFactor(comp_h)
    assert dsf == (1, 1)  # Full Res

    # Set Downsample (Half)
    ae.sdk.AEGP_SetCompDownSampleFactor(comp_h, 2, 2)
    dsf_new = ae.sdk.AEGP_GetCompDownSampleFactor(comp_h)
    assert dsf_new == (2, 2)

    # Set Frame Duration (Convert 24fps -> 60fps)
    # 1/60 = 0.016666...
    ae.sdk.AEGP_SetCompFrameDuration(comp_h, 1.0 / 60.0)
    # Duration check won't change, but internally fps changed.
    # Verification of FPS change is tricky without GetCompFramerate exposed,
    # but we can rely on no error being thrown.

    print("SDK Item/Comp Tests Passed")


if __name__ == "__main__":
    try:
        test_sdk_item_comp()
    except Exception as e:
        print(f"FAILED: {e}")
        import traceback

        traceback.print_exc()
