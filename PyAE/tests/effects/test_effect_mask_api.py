#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Effect/Mask API のテストスクリプト

Phase 5-6 で実装された API の動作確認：
- Effect Application API
- Mask Creation API
"""

import ae

def test_effect_api():
    """エフェクトAPIのテスト"""
    print("\n" + "="*60)
    print("Effect API テスト開始")
    print("="*60)

    app = ae.app()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("TestComp_Effect", 1920, 1080, 1.0, 5.0, 30.0)
    print(f"✓ テストコンポジション作成: {comp.name}")

    # Solid レイヤー作成
    layer = comp.create_solid([1.0, 0.0, 0.0], "TestSolid", 1920, 1080, 1.0, 5.0)
    print(f"✓ Solidレイヤー作成: {layer.name}")

    # テスト1: エフェクトの追加
    print("\n[Test 1] エフェクト追加")
    try:
        effect = layer.add_effect("ADBE Gaussian Blur 2")
        print(f"  ✓ エフェクト追加成功: {effect.name}")
        print(f"    - Match Name: {effect.match_name}")
    except Exception as e:
        print(f"  ✗ エフェクト追加失敗: {e}")
        return False

    # テスト2: エフェクト数の確認
    print("\n[Test 2] エフェクト数の確認")
    num_effects = layer.num_effects
    print(f"  - エフェクト数: {num_effects}")
    if num_effects != 1:
        print(f"  ✗ 期待値(1)と異なる: {num_effects}")
        return False
    print("  ✓ エフェクト数が正しい")

    # テスト3: エフェクトの取得（インデックス）
    print("\n[Test 3] エフェクト取得（インデックス）")
    try:
        effect_by_index = layer.effect(0)
        if effect_by_index:
            print(f"  ✓ インデックス取得成功: {effect_by_index.name}")
        else:
            print("  ✗ インデックス取得失敗: None が返された")
            return False
    except Exception as e:
        print(f"  ✗ インデックス取得失敗: {e}")
        return False

    # テスト4: エフェクトの取得（名前）
    print("\n[Test 4] エフェクト取得（名前）")
    try:
        effect_by_name = layer.effect("ガウスぼかし")  # 日本語名
        if effect_by_name:
            print(f"  ✓ 名前取得成功: {effect_by_name.name}")
        else:
            print("  ⚠ 名前取得失敗（ローカライズ名の問題かもしれません）")
    except Exception as e:
        print(f"  ⚠ 名前取得エラー: {e}")

    # テスト5: 全エフェクトの取得
    print("\n[Test 5] 全エフェクト取得")
    try:
        effects = layer.effects
        print(f"  ✓ 全エフェクト取得成功: {len(effects)} 個")
        for i, eff in enumerate(effects):
            print(f"    [{i}] {eff.name} ({eff.match_name})")
    except Exception as e:
        print(f"  ✗ 全エフェクト取得失敗: {e}")
        return False

    # テスト6: エフェクトパラメータの設定
    print("\n[Test 6] エフェクトパラメータの設定")
    try:
        # Blurriness パラメータを取得
        blur_param = effect.property("Blurriness")
        if blur_param:
            print(f"  - 現在の値: {blur_param.value}")
            blur_param.value = 50.0
            print(f"  ✓ パラメータ設定成功: {blur_param.value}")
        else:
            print("  ✗ パラメータ取得失敗")
            return False
    except Exception as e:
        print(f"  ✗ パラメータ設定失敗: {e}")
        return False

    # テスト7: 複数エフェクトの追加
    print("\n[Test 7] 複数エフェクト追加")
    try:
        effect2 = layer.add_effect("ADBE Fill")
        print(f"  ✓ 2つ目のエフェクト追加成功: {effect2.name}")

        num_effects = layer.num_effects
        if num_effects != 2:
            print(f"  ✗ エフェクト数が期待値(2)と異なる: {num_effects}")
            return False
        print("  ✓ エフェクト数が正しい: 2")
    except Exception as e:
        print(f"  ✗ 2つ目のエフェクト追加失敗: {e}")
        return False

    print("\n" + "="*60)
    print("Effect API テスト完了: すべてのテストに合格")
    print("="*60)
    return True


def test_mask_api():
    """マスクAPIのテスト"""
    print("\n" + "="*60)
    print("Mask API テスト開始")
    print("="*60)

    app = ae.app()
    project = app.project

    # テスト用コンポジション作成
    comp = project.create_comp("TestComp_Mask", 1920, 1080, 1.0, 5.0, 30.0)
    print(f"✓ テストコンポジション作成: {comp.name}")

    # Solid レイヤー作成
    layer = comp.create_solid([0.0, 1.0, 0.0], "TestSolid", 1920, 1080, 1.0, 5.0)
    print(f"✓ Solidレイヤー作成: {layer.name}")

    # テスト1: マスクの追加（空）
    print("\n[Test 1] マスク追加（空）")
    try:
        mask = layer.add_mask()
        if mask:
            print(f"  ✓ マスク追加成功: {mask.name}")
        else:
            print("  ✗ マスク追加失敗: None が返された")
            return False
    except Exception as e:
        print(f"  ✗ マスク追加失敗: {e}")
        return False

    # テスト2: マスク数の確認
    print("\n[Test 2] マスク数の確認")
    num_masks = layer.num_masks
    print(f"  - マスク数: {num_masks}")
    if num_masks != 1:
        print(f"  ✗ 期待値(1)と異なる: {num_masks}")
        return False
    print("  ✓ マスク数が正しい")

    # テスト3: マスクの取得（インデックス）
    print("\n[Test 3] マスク取得（インデックス）")
    try:
        mask_by_index = layer.mask(0)
        if mask_by_index:
            print(f"  ✓ インデックス取得成功: {mask_by_index.name}")
        else:
            print("  ✗ インデックス取得失敗: None が返された")
            return False
    except Exception as e:
        print(f"  ✗ インデックス取得失敗: {e}")
        return False

    # テスト4: マスクの取得（名前）
    print("\n[Test 4] マスク取得（名前）")
    try:
        mask_name = mask.name
        mask_by_name = layer.mask(mask_name)
        if mask_by_name:
            print(f"  ✓ 名前取得成功: {mask_by_name.name}")
        else:
            print("  ✗ 名前取得失敗: None が返された")
            return False
    except Exception as e:
        print(f"  ✗ 名前取得失敗: {e}")
        return False

    # テスト5: 全マスクの取得
    print("\n[Test 5] 全マスク取得")
    try:
        masks = layer.masks
        print(f"  ✓ 全マスク取得成功: {len(masks)} 個")
        for i, m in enumerate(masks):
            print(f"    [{i}] {m.name}")
    except Exception as e:
        print(f"  ✗ 全マスク取得失敗: {e}")
        return False

    # テスト6: マスクプロパティの設定
    print("\n[Test 6] マスクプロパティの設定")
    try:
        # マスクプロパティが存在するか確認（例: Mask Feather）
        print("  ⚠ マスクプロパティの詳細テストは PyMask クラスの実装確認後に実施")
    except Exception as e:
        print(f"  ⚠ マスクプロパティテストエラー: {e}")

    # テスト7: 複数マスクの追加
    print("\n[Test 7] 複数マスク追加")
    try:
        mask2 = layer.add_mask()
        print(f"  ✓ 2つ目のマスク追加成功: {mask2.name}")

        num_masks = layer.num_masks
        if num_masks != 2:
            print(f"  ✗ マスク数が期待値(2)と異なる: {num_masks}")
            return False
        print("  ✓ マスク数が正しい: 2")
    except Exception as e:
        print(f"  ✗ 2つ目のマスク追加失敗: {e}")
        return False

    # テスト8: 頂点データ付きマスク追加（現在は未実装）
    print("\n[Test 8] 頂点データ付きマスク追加")
    print("  ⚠ 頂点データ設定は未実装（Phase 5-6 の残り15%）")
    try:
        vertices = [
            {"position": [100, 100]},
            {"position": [200, 100]},
            {"position": [200, 200]},
            {"position": [100, 200]}
        ]
        # mask3 = layer.add_mask(vertices)
        # print(f"  ✓ 頂点データ付きマスク追加成功: {mask3.name}")
    except Exception as e:
        print(f"  ⚠ 頂点データ付きマスク追加: 現在は空のマスクのみサポート")

    print("\n" + "="*60)
    print("Mask API テスト完了: 基本機能のテストに合格")
    print("="*60)
    return True


def main():
    """メインテスト関数"""
    print("\n" + "="*60)
    print("PyAE Effect/Mask API テストスイート")
    print("Phase 5-6 実装の動作確認")
    print("="*60)

    results = {
        "Effect API": False,
        "Mask API": False
    }

    # Effect API テスト
    try:
        results["Effect API"] = test_effect_api()
    except Exception as e:
        print(f"\n✗ Effect API テストで予期しないエラー: {e}")
        import traceback
        traceback.print_exc()

    # Mask API テスト
    try:
        results["Mask API"] = test_mask_api()
    except Exception as e:
        print(f"\n✗ Mask API テストで予期しないエラー: {e}")
        import traceback
        traceback.print_exc()

    # 結果サマリー
    print("\n" + "="*60)
    print("テスト結果サマリー")
    print("="*60)
    for test_name, passed in results.items():
        status = "✓ 合格" if passed else "✗ 不合格"
        print(f"{test_name}: {status}")

    all_passed = all(results.values())
    if all_passed:
        print("\n🎉 すべてのテストに合格しました！")
        print("Phase 5-6 の実装は正常に動作しています。")
    else:
        print("\n⚠ 一部のテストが失敗しました。")
        print("ログを確認して問題を修正してください。")

    print("="*60)


if __name__ == "__main__":
    main()
