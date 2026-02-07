"""
エフェクト名設定のシンプルなテストスクリプト
effect.set_name() 機能をテスト
"""
import ae

def main():
    project = ae.get_project()
    if not project:
        print("No project open")
        return

    # テストコンポジションを作成
    comp = project.create_comp("Test Effect SetName", 1920, 1080, 1.0, 10.0, 30.0)

    # Solidレイヤーを作成
    layer = comp.add_solid("Test Solid", 1920, 1080, (1.0, 0.0, 0.0), 10.0)

    # エフェクトを追加
    print("\n=== Adding Effect ===")
    effect = layer.add_effect("ADBE Fill")
    print(f"Added effect: {effect.name}")
    print(f"Match name: {effect.match_name}")

    # エフェクト名を変更
    print("\n=== Testing set_name() ===")
    new_name = "Custom Fill Effect"
    print(f"Calling effect.set_name('{new_name}')...")

    try:
        effect.set_name(new_name)
        print(f"[OK] set_name() succeeded!")

        # 名前が変更されたか確認
        plugin_name = effect.name  # プラグイン名(ローカライズ済み)
        stream_name = effect.stream_name  # ストリーム名(ユーザー設定)

        print(f"Plugin name (effect.name): {plugin_name}")
        print(f"Stream name (effect.stream_name): {stream_name}")

        if stream_name == new_name:
            print(f"[OK] Stream name changed successfully: '{new_name}'")
            print(f"  (Plugin name remains: '{plugin_name}')")
        else:
            print(f"[ERROR] Stream name not changed. Expected: '{new_name}', Got: '{stream_name}'")

    except Exception as e:
        print(f"[ERROR] set_name() failed: {e}")
        import traceback
        traceback.print_exc()

    print("\n=== Test Complete ===")

if __name__ == "__main__":
    main()
