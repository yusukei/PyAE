"""
レイヤースタイルのエクスポート/インポートテスト

このスクリプトは、レイヤースタイルの有効/無効状態が
正しくエクスポート/インポートされることをテストします。

使用方法:
1. After Effectsで実行
2. レイヤースタイルを適用したレイヤーを選択
3. エクスポート実行
4. JSONの内容を確認
5. 新規プロジェクトでインポート実行
6. レイヤースタイルの状態を確認
"""

import ae
import json
import os
import sys

def export_layer_styles_test():
    """レイヤースタイルのエクスポートテスト"""
    print("=" * 60)
    print("レイヤースタイル エクスポートテスト")
    print("=" * 60)

    # アクティブなコンポジションを取得
    comp = ae.get_active_comp()
    if not comp:
        print("エラー: アクティブなコンポジションがありません")
        return

    # 選択されたレイヤーを取得
    layers = comp.selected_layers
    if not layers or len(layers) == 0:
        print("エラー: レイヤーが選択されていません")
        return

    layer = layers[0]
    print(f"\nレイヤー: {layer.name}")
    print(f"インデックス: {layer.index}")

    # レイヤースタイルグループを取得
    try:
        layer_styles = layer.property("ADBE Layer Styles")
        if not layer_styles:
            print("エラー: レイヤースタイルが見つかりません")
            return

        print(f"\nレイヤースタイル グループ: {layer_styles.name}")
        print(f"  match_name: {layer_styles.match_name}")
        print(f"  grouping_type: {layer_styles.grouping_type}")
        print(f"  子プロパティ数: {layer_styles.num_properties}")

        # 各レイヤースタイル効果を確認
        print("\nレイヤースタイル効果:")
        for i in range(1, layer_styles.num_properties + 1):
            try:
                prop = layer_styles.property(i)

                # Dynamic Stream フラグを取得
                try:
                    flags = prop.get_dynamic_stream_flags()
                    is_enabled = bool(flags & 0x01)  # AEGP_DynStreamFlag_ACTIVE_EYEBALL

                    print(f"\n  [{i}] {prop.name}")
                    print(f"      match_name: {prop.match_name}")
                    print(f"      flags: 0x{flags:02x}")
                    print(f"      enabled: {is_enabled}")

                except (AttributeError, RuntimeError) as e:
                    print(f"\n  [{i}] {prop.name}")
                    print(f"      match_name: {prop.match_name}")
                    print(f"      Dynamic flags取得失敗: {e}")

            except Exception as e:
                print(f"  プロパティ {i} 取得失敗: {e}")

    except Exception as e:
        print(f"エラー: {e}")
        import traceback
        traceback.print_exc()


def test_layer_styles_export_json():
    """レイヤースタイルのJSONエクスポートテスト"""
    print("=" * 60)
    print("レイヤースタイル JSONエクスポートテスト")
    print("=" * 60)

    # アクティブなコンポジションを取得
    comp = ae.get_active_comp()
    if not comp:
        print("エラー: アクティブなコンポジションがありません")
        return

    # 選択されたレイヤーを取得
    layers = comp.selected_layers
    if not layers or len(layers) == 0:
        print("エラー: レイヤーが選択されていません")
        return

    layer = layers[0]
    print(f"\nレイヤー: {layer.name}")

    # レイヤースタイルグループを取得
    try:
        layer_styles = layer.property("ADBE Layer Styles")
        if not layer_styles:
            print("エラー: レイヤースタイルが見つかりません")
            return

        # export_property_tree 相当の処理
        def export_property_tree(prop, depth=0):
            indent = "  " * depth
            result = {
                "match_name": prop.match_name,
            }

            if prop.is_group:
                result["type"] = "group"

                # グループの有効/無効フラグを取得
                try:
                    flags = prop.get_dynamic_stream_flags()
                    is_enabled = bool(flags & 0x01)
                    result["enabled"] = is_enabled
                    print(f"{indent}Group {prop.match_name} enabled: {is_enabled} (flags: 0x{flags:02x})")
                except (AttributeError, RuntimeError) as e:
                    print(f"{indent}Dynamic flags not available for {prop.match_name}: {e}")

                result["children"] = {}
                for i in range(1, prop.num_properties + 1):
                    child = prop.property(i)
                    name = child.name if child.name else child.match_name
                    result["children"][name] = export_property_tree(child, depth + 1)
            else:
                result["type"] = "leaf"
                try:
                    value = prop.value
                    result["value"] = str(value)
                except:
                    result["value"] = None

            return result

        print("\nレイヤースタイル エクスポート:")
        layer_styles_data = export_property_tree(layer_styles, 0)

        # JSONを保存
        output_path = os.path.join(os.path.dirname(__file__), "layer_styles_test.json")
        with open(output_path, "w", encoding="utf-8") as f:
            json.dump(layer_styles_data, f, indent=2, ensure_ascii=False)

        print(f"\nJSON保存完了: {output_path}")
        print("\nJSON内容:")
        print(json.dumps(layer_styles_data, indent=2, ensure_ascii=False))

    except Exception as e:
        print(f"エラー: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    # エクスポートテスト
    export_layer_styles_test()
    print("\n")
    test_layer_styles_export_json()
