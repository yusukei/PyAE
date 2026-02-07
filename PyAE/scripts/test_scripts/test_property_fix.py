"""
Test script to verify Property type registration fix
"""

import ae

print("=" * 60)
print("  Property Type Registration Test")
print("=" * 60)
print()

# Create a test composition
print("Creating test composition...")
comp = ae.Comp.create("Test", 1920, 1080, 1.0, 10.0)
print(f"[PASS] Created composition: {comp.name}")

# Add a test layer
print("Adding test layer...")
solid_layer = comp.add_solid("Test Layer", 100, 100, (1.0, 0.0, 0.0))
print(f"[PASS] Added layer: {solid_layer.name}")

# Test property access
print()
print("Testing property access...")
try:
    # Get Position property
    pos_prop = solid_layer.property("Position")
    print(f"Property object type: {type(pos_prop)}")
    print(f"Property object class name: {pos_prop.__class__.__name__}")
    print(f"Property object module: {pos_prop.__class__.__module__}")

    # Test if it has the expected attributes
    if hasattr(pos_prop, 'name'):
        print(f"[PASS] Property has 'name' attribute: {pos_prop.name}")
    else:
        print(f"[FAIL] Property missing 'name' attribute")
        print(f"Available attributes: {dir(pos_prop)}")

    if hasattr(pos_prop, 'value'):
        print(f"[PASS] Property has 'value' attribute: {pos_prop.value}")
    else:
        print(f"[FAIL] Property missing 'value' attribute")

    if hasattr(pos_prop, 'type'):
        print(f"[PASS] Property has 'type' attribute: {pos_prop.type}")
    else:
        print(f"[FAIL] Property missing 'type' attribute")

    # Check if it's the correct type
    if pos_prop.__class__.__name__ == "Property":
        print(f"[PASS] Property is correctly typed as 'Property'")
    else:
        print(f"[FAIL] Property has unexpected type: {pos_prop.__class__.__name__}")

except Exception as e:
    print(f"[FAIL] Error accessing property: {e}")
    import traceback
    traceback.print_exc()

# Test other properties
print()
print("Testing other properties...")
try:
    opacity = solid_layer.property("Opacity")
    print(f"Opacity property type: {type(opacity).__name__}")
    print(f"Opacity name: {opacity.name}")
    print(f"Opacity value: {opacity.value}")
    print(f"[PASS] Opacity property works correctly")
except Exception as e:
    print(f"[FAIL] Opacity property error: {e}")

try:
    scale = solid_layer.property("Scale")
    print(f"Scale property type: {type(scale).__name__}")
    print(f"Scale name: {scale.name}")
    print(f"Scale value: {scale.value}")
    print(f"[PASS] Scale property works correctly")
except Exception as e:
    print(f"[FAIL] Scale property error: {e}")

print()
print("=" * 60)
print("  Test Complete")
print("=" * 60)
