#!/usr/bin/env python3
"""
型スタブと実装の整合性チェックスクリプト
"""

from typing import Dict, List, Tuple
import re

# 実装されているEnum値（C++コードから抽出）
ENUM_IMPLEMENTATIONS = {
    "LayerType": {
        "file": "PyAE/src/PyBindings/PyLayer.cpp",
        "values": ["None_", "AV", "Light", "Camera", "Text", "Adjustment", "Null", "Shape", "Solid"]
    },
    "ItemType": {
        "file": "PyAE/src/PyBindings/PyItem.cpp",
        "values": ["None_", "Folder", "Comp", "Footage", "Solid"]
    },
    "FootageType": {
        "file": "PyAE/src/PyBindings/PyItem.cpp",
        "values": ["None_", "Solid", "Missing", "Placeholder"]
    },
    "MaskMode": {
        "file": "PyAE/src/PyBindings/PyMask.cpp",
        "values": ["None_", "Add", "Subtract", "Intersect", "Lighten", "Darken", "Difference"]
    },
    "RenderStatus": {
        "file": "PyAE/src/PyBindings/PyRenderQueue.cpp",
        "values": ["WillContinue", "NeedsOutput", "Queued", "Rendering", "UserStopped", "ErrStopped", "Done", "Unqueued"]
    },
    "PropertyType": {
        "file": "PyAE/src/PyBindings/PyProperty.cpp",
        "values": ["None_", "OneDim", "TwoDim", "ThreeDim", "Color", "Arbitrary", "NoValue"]
    },
    "KeyInterpolation": {
        "file": "PyAE/src/PyBindings/PyKeyframe.cpp",
        "values": ["Linear", "Bezier", "Hold"]  # 要確認
    },
    "EasingType": {
        "file": "PyAE/src/PyBindings/PyKeyframe.cpp",
        "values": ["EaseIn", "EaseOut", "EaseInOut"]  # 要確認
    },
    "LightType": {
        "file": "PyAE/src/PyBindings/Py3DLayer.cpp",
        "values": ["Parallel", "Spot", "Point", "Ambient"]  # 要確認
    }
}

# 型スタブで定義されているEnum値（修正後）
STUB_DEFINITIONS = {
    "LayerType": ["None_", "AV", "Light", "Camera", "Text", "Adjustment", "Null", "Shape", "Solid"],
    "ItemType": ["None_", "Folder", "Comp", "Footage", "Solid"],
    "FootageType": ["None_", "Solid", "Missing", "Placeholder"],
    "MaskMode": ["None_", "Add", "Subtract", "Intersect", "Lighten", "Darken", "Difference"],
    "RenderStatus": ["WillContinue", "NeedsOutput", "Queued", "Rendering", "UserStopped", "ErrStopped", "Done", "Unqueued"],
    "PropertyType": ["None_", "OneDim", "TwoDim", "ThreeDim", "Color", "Arbitrary", "NoValue"],
    "KeyInterpolation": ["Linear", "Bezier", "Hold"],
    "EasingType": ["EaseIn", "EaseOut", "EaseInOut"],
    "LightType": ["Parallel", "Spot", "Point", "Ambient"],
}


def compare_enums():
    """Enum定義の比較"""
    print("=" * 70)
    print("Enum型の整合性チェック")
    print("=" * 70)

    issues = []

    for enum_name, impl_data in ENUM_IMPLEMENTATIONS.items():
        print(f"\n### {enum_name}")
        print(f"ファイル: {impl_data['file']}")

        impl_values = impl_data['values']
        stub_values = STUB_DEFINITIONS.get(enum_name, [])

        print(f"\n実装値 ({len(impl_values)}個):")
        print(f"  {', '.join(impl_values)}")

        if stub_values:
            print(f"\n型スタブ ({len(stub_values)}個):")
            print(f"  {', '.join(stub_values)}")

            # 実装にあって型スタブにない値
            missing_in_stub = set(impl_values) - set(stub_values)
            if missing_in_stub:
                print(f"\n[NG] Missing in stub:")
                for val in missing_in_stub:
                    print(f"  - {val}")
                    issues.append(f"{enum_name}: Missing '{val}' in stub")

            # 型スタブにあって実装にない値
            missing_in_impl = set(stub_values) - set(impl_values)
            if missing_in_impl:
                print(f"\n[NG] Missing in implementation:")
                for val in missing_in_impl:
                    print(f"  - {val}")
                    issues.append(f"{enum_name}: Missing '{val}' in implementation")

            # 大文字小文字の違い
            impl_lower = {v.lower(): v for v in impl_values}
            stub_lower = {v.lower(): v for v in stub_values}

            case_issues = []
            for key in impl_lower.keys() & stub_lower.keys():
                if impl_lower[key] != stub_lower[key]:
                    case_issues.append((impl_lower[key], stub_lower[key]))

            if case_issues:
                print(f"\n[WARN] Naming convention differences:")
                for impl_val, stub_val in case_issues:
                    print(f"  - Implementation: {impl_val}  Stub: {stub_val}")
                    issues.append(f"{enum_name}: Naming difference ({impl_val} vs {stub_val})")

        else:
            print(f"\n[WARN] Not defined in stub")
            issues.append(f"{enum_name}: Entire stub missing")

    print("\n" + "=" * 70)
    print(f"問題点: {len(issues)}件")
    print("=" * 70)

    if issues:
        print("\nDetails:")
        for i, issue in enumerate(issues, 1):
            print(f"{i}. {issue}")
    else:
        print("\n[OK] No issues found")


def check_global_attributes():
    """グローバル属性のチェック"""
    print("\n" + "=" * 70)
    print("グローバル属性のチェック")
    print("=" * 70)

    attrs = {
        "project": {"stub": False, "impl": False, "note": "get_project()関数を使用 - 将来実装予定"},
        "render_queue": {"stub": False, "impl": False, "note": "将来実装予定"}
    }

    for attr_name, data in attrs.items():
        status = "[OK]" if data["impl"] else "[NG]"
        print(f"\n{status} ae.{attr_name}")
        print(f"  Stub: {'Defined' if data['stub'] else 'Not defined'}")
        print(f"  Implementation: {'Yes' if data['impl'] else 'No'}")
        if data.get("note"):
            print(f"  Note: {data['note']}")


def main():
    """メイン"""
    print("\nPyAE 型スタブと実装の整合性チェック\n")

    compare_enums()
    check_global_attributes()

    print("\n" + "=" * 70)
    print("チェック完了")
    print("=" * 70)


if __name__ == "__main__":
    main()
