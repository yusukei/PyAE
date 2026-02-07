# debug_test_effect.py
# エフェクトテストのデバッグ

import ae
import traceback

def main():
    try:
        proj = ae.get_project()
        comp = proj.create_comp("_DebugEffectTest", 1920, 1080, 1.0, 5.0, 30.0)
        layer = comp.add_solid("_TestLayer", 1920, 1080, (1.0, 0.0, 0.0), 5.0)

        print("=== Effect Test Debug ===")

        # エフェクトを追加
        print("Adding effect...")
        effect = layer.add_effect("ADBE Gaussian Blur 2")
        print(f"Effect added: {effect}")
        print(f"Effect name: {effect.name}")
        print(f"Effect match_name: {effect.match_name}")

        # インデックスを取得
        print(f"Getting effect index...")
        index = effect.index
        print(f"Effect index: {index}")

        # 同じ種類のエフェクトを2つ追加してテスト
        print("\nAdding second effect of same type...")
        effect2 = layer.add_effect("ADBE Gaussian Blur 2")
        print(f"Effect2 added: {effect2}")

        index1 = effect.index
        index2 = effect2.index
        print(f"Effect 1 index: {index1}")
        print(f"Effect 2 index: {index2}")

        if index1 == index2:
            print("ERROR: Both effects have the same index!")
        else:
            print("SUCCESS: Effects have different indices")

        # クリーンアップ
        comp.delete()

        print("\n=== Test Complete ===")
        return "SUCCESS"

    except Exception as e:
        print(f"ERROR: {e}")
        traceback.print_exc()
        return f"FAILED: {e}"

if __name__ == "__main__":
    result = main()
    print(f"\nResult: {result}")
