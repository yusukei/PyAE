"""
ExtendScript経由でエフェクト名を変更するテスト
"""
import ae

def main():
    project = ae.get_project()
    if not project:
        print("No project open")
        return

    # テストコンポジションを作成
    comp = project.create_comp("Test Effect Name ExtendScript", 1920, 1080, 1.0, 10.0, 30.0)

    # Solidレイヤーを作成
    layer = comp.add_solid("Test Solid", 1920, 1080, (1.0, 0.0, 0.0), 10.0)

    # エフェクトを追加
    effect = layer.add_effect("ADBE Fill")
    print(f"Added effect: {effect.name}, match_name: {effect.match_name}")

    # ExtendScriptを使ってエフェクト名を変更
    extendscript = """
    (function() {
        var comp = app.project.activeItem;
        if (!comp || !(comp instanceof CompItem)) {
            return "No active comp";
        }

        var layer = comp.layer(1);
        if (!layer) {
            return "No layer found";
        }

        // エフェクトにアクセス
        var effect = layer.effect(1);
        if (!effect) {
            return "No effect found";
        }

        // エフェクト名を取得
        var oldName = effect.name;

        // エフェクト名を変更
        try {
            effect.name = "Custom Effect Name";
            return "Success: " + oldName + " -> " + effect.name;
        } catch (e) {
            return "Error: " + e.toString();
        }
    })();
    """

    print("\n=== Executing ExtendScript ===")
    try:
        result = ae.execute_script(extendscript)
        print(f"Result: {result}")

        # 変更後のエフェクト名を確認
        print(f"\nAfter script execution:")
        print(f"Effect name: {effect.name}")

        # レイヤーのエフェクトを再取得
        effects = layer.effects
        if effects:
            for idx, eff in enumerate(effects):
                print(f"Effect {idx}: name={eff.name}, match_name={eff.match_name}")
    except AttributeError as e:
        print(f"ExtendScript execution not supported: {e}")
        print("\nNote: ae.execute_script() may not be implemented yet.")
    except Exception as e:
        print(f"Error executing ExtendScript: {e}")

    print("\n=== Done ===")

if __name__ == "__main__":
    main()
