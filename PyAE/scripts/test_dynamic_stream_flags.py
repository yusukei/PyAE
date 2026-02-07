"""
Dynamic Stream Flags API テスト

新しく実装した get_dynamic_stream_flags / set_dynamic_stream_flag が
正しく動作するかを確認するシンプルなテストスクリプト
"""

import ae

def test_dynamic_stream_flags_api():
    """Dynamic Stream Flags API の動作確認"""
    print("=" * 60)
    print("Dynamic Stream Flags API テスト")
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
        print("使用方法: レイヤーを選択してからこのスクリプトを実行してください")
        return

    layer = layers[0]
    print(f"\nレイヤー: {layer.name}")

    # Transform プロパティグループでテスト
    try:
        transform = layer.property("ADBE Transform Group")
        if not transform:
            print("エラー: Transform グループが見つかりません")
            return

        print(f"\n✓ Transform グループを取得")
        print(f"  match_name: {transform.match_name}")
        print(f"  is_group: {transform.is_group}")

        # get_dynamic_stream_flags をテスト
        try:
            flags = transform.get_dynamic_stream_flags()
            print(f"\n✓ get_dynamic_stream_flags() 成功")
            print(f"  flags: 0x{flags:02x}")
            print(f"  ACTIVE_EYEBALL (bit 0): {bool(flags & 0x01)}")
            print(f"  HIDDEN (bit 1): {bool(flags & 0x02)}")
            print(f"  DISABLED (bit 2): {bool(flags & 0x04)}")
        except Exception as e:
            print(f"\n✗ get_dynamic_stream_flags() 失敗: {e}")

        # レイヤースタイルの確認（存在する場合）
        print("\n--- レイヤースタイル確認 ---")

        # まず、レイヤーの全プロパティを確認
        try:
            num_props = layer.num_properties
            print(f"レイヤーのトップレベルプロパティ数: {num_props}")

            # レイヤースタイルを探す
            layer_styles = None
            for i in range(1, num_props + 1):
                try:
                    prop = layer.property(i)
                    if prop.match_name == "ADBE Layer Styles":
                        layer_styles = prop
                        break
                except:
                    pass

            if layer_styles:
                print(f"\n✓ レイヤースタイルグループを検出")
                print(f"  name: {layer_styles.name}")
                print(f"  match_name: {layer_styles.match_name}")
                print(f"  子プロパティ数: {layer_styles.num_properties}")

                # 各レイヤースタイル効果を確認
                for i in range(1, min(layer_styles.num_properties + 1, 6)):  # 最初の5つまで
                    try:
                        prop = layer_styles.property(i)
                        flags = prop.get_dynamic_stream_flags()
                        is_enabled = bool(flags & 0x01)
                        print(f"\n  [{i}] {prop.name}")
                        print(f"      match_name: {prop.match_name}")
                        print(f"      flags: 0x{flags:02x}, enabled: {is_enabled}")
                    except Exception as e:
                        print(f"  [{i}] フラグ取得失敗: {e}")
            else:
                print("\n※ レイヤースタイルは未適用です")
                print("   レイヤースタイルをテストする場合は、")
                print("   レイヤーメニュー → レイヤースタイル から効果を追加してください")

        except Exception as e:
            print(f"\n※ レイヤースタイル確認エラー: {e}")
            import traceback
            traceback.print_exc()

        print("\n" + "=" * 60)
        print("✓ API テスト完了")
        print("=" * 60)

    except Exception as e:
        print(f"\nエラー: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    test_dynamic_stream_flags_api()
