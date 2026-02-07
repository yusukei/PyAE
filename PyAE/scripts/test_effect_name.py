"""
エフェクト名設定のテストスクリプト
"""
import ae

def main():
    project = ae.get_project()
    if not project:
        print("No project open")
        return

    # テストコンポジションを作成
    comp = project.create_comp("Test Effect Name", 1920, 1080, 1.0, 10.0, 30.0)

    # Solidレイヤーを作成
    layer = comp.add_solid("Test Solid", 1920, 1080, (1.0, 0.0, 0.0), 10.0)

    # エフェクトを追加
    effect = layer.add_effect("ADBE Fill")
    print(f"Added effect: {effect.name}, match_name: {effect.match_name}")
    print(f"Effect index: {effect.index}")
    print(f"Layer num_effects: {layer.num_effects}")

    # レイヤーのエフェクトを直接確認
    print("\n=== Testing Direct Effect Access ===")
    try:
        # effectsプロパティから直接アクセス
        print(f"layer.effects type: {type(layer.effects)}")
        print(f"layer.effects: {layer.effects}")

        if layer.effects:
            for idx, eff in enumerate(layer.effects):
                print(f"Effect {idx}: name={eff.name}, match_name={eff.match_name}")

                # エフェクト自体のnameを変更できるか
                try:
                    old_name = eff.name
                    eff.name = "Custom Name"
                    print(f"  -> Effect name changed: {old_name} -> {eff.name}")
                except AttributeError as e:
                    print(f"  -> Effect.name is read-only (expected): {type(e).__name__}")
                except Exception as e:
                    print(f"  -> Cannot change effect.name: {e}")

    except Exception as e:
        print(f"Error with direct effect access: {e}")

    # 別の方法: layer.property() を使う
    print("\n=== Testing layer.property() ===")
    match_names_to_try = [
        "ADBE Effect Parade",
        "Effects",
        "エフェクト",
    ]

    for match_name in match_names_to_try:
        try:
            print(f"Trying: {match_name}")
            effects_prop = layer.property(match_name)
            if effects_prop:
                print(f"  -> Found! type={type(effects_prop)}, valid={effects_prop.valid if hasattr(effects_prop, 'valid') else 'N/A'}")

                # サブプロパティを確認
                if hasattr(effects_prop, 'num_properties'):
                    num_sub = effects_prop.num_properties
                    print(f"  -> Number of sub-properties: {num_sub}")

                    if num_sub > 0:
                        # 最初のサブプロパティ（エフェクト）を取得
                        sub_prop = effects_prop.property_by_index(1)
                        if sub_prop:
                            print(f"  -> Sub-property 1: name={sub_prop.name if hasattr(sub_prop, 'name') else 'N/A'}")

                            # 名前を変更できるか
                            try:
                                old_name = sub_prop.name
                                sub_prop.name = "Custom Effect Name"
                                print(f"  -> Name changed successfully: {old_name} -> {sub_prop.name}")
                                sub_prop.name = old_name  # 元に戻す
                            except Exception as e:
                                print(f"  -> Cannot change name: {e}")
            else:
                print(f"  -> Not found")
        except Exception as e:
            print(f"  -> Error: {e}")

    print("\n=== Done ===")

if __name__ == "__main__":
    main()
