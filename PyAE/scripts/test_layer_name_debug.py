# test_layer_name_debug.py
# Debug script for layer name issue

import ae

print("\n=== Layer Name Debug ===\n")

proj = ae.get_project()
comp = proj.create_comp("_NameDebugComp", 1920, 1080, 1.0, 10.0, 30.0)

print("Creating solid layer with name '_TestSolid'...")
layer = comp.add_solid("_TestSolid", 200, 200, (1.0, 0.0, 0.0), 5.0)

print(f"Layer created. layer.name = '{layer.name}'")
print(f"Layer name length = {len(layer.name)}")
print(f"Layer name repr = {repr(layer.name)}")

print("\nSetting layer name to '_RenamedSolid'...")
try:
    layer.name = "_RenamedSolid"
    print(f"After setting: layer.name = '{layer.name}'")
except Exception as e:
    print(f"Error setting name: {e}")

print("\nCreating null layer with name '_TestNull'...")
null_layer = comp.add_null("_TestNull", 10.0)
print(f"Null layer created. null_layer.name = '{null_layer.name}'")

print("\n=== End Debug ===\n")
