# explore_text_scale.py
# Text Animator Scale プロパティの詳細調査
# Phase 9: Scale 復元問題の原因特定

import ae
import os

# ログファイル
log_path = os.path.join(ae.get_documents_folder(), "text_scale_investigation.txt")
log_file = open(log_path, "w", encoding="utf-8")

def log(msg):
    """ログ出力"""
    ae.log_info(f"[SCALE_INVESTIGATE] {msg}")
    log_file.write(msg + "\n")
    log_file.flush()

def investigate_scale_property():
    """Scale プロパティの詳細調査"""
    log("=" * 60)
    log("Text Animator Scale Property Investigation")
    log("=" * 60)

    project = ae.get_project()
    if not project:
        log("ERROR: プロジェクトが開いていません")
        return "ERROR"

    comp = project.create_comp("Scale_Investigation", 1920, 1080, 1.0, 5.0, 30.0)
    text_layer = comp.add_text("Test")
    log(f"テストコンポ作成: {comp.name}")
    log(f"テキストレイヤー作成: {text_layer.name}")

    # Animator を追加
    root = text_layer.properties
    text_props = root.property("ADBE Text Properties")
    animators = text_props.property("ADBE Text Animators")
    animator = animators.add_stream("ADBE Text Animator")
    log(f"Animator追加: {animator.name}")

    # Scale プロパティを取得
    anim_props = animator.property("ADBE Text Animator Properties")
    scale_prop = anim_props.property("ADBE Text Scale 3D")

    log("\n" + "=" * 60)
    log("Scale Property Metadata")
    log("=" * 60)

    # 基本情報
    log(f"match_name: {scale_prop.match_name}")
    log(f"name: {scale_prop.name}")
    log(f"is_group: {scale_prop.is_group}")

    # プロパティタイプ情報
    try:
        if hasattr(scale_prop, 'value_type'):
            log(f"value_type: {scale_prop.value_type}")
    except Exception as e:
        log(f"value_type: (取得失敗) {e}")

    try:
        if hasattr(scale_prop, 'grouping_type'):
            log(f"grouping_type: {scale_prop.grouping_type}")
    except Exception as e:
        log(f"grouping_type: (取得失敗) {e}")

    # デフォルト値
    default_value = list(scale_prop.value)
    log(f"default_value: {default_value}")

    # 制約情報
    try:
        if hasattr(scale_prop, 'min_value'):
            log(f"min_value: {scale_prop.min_value}")
    except Exception as e:
        log(f"min_value: (なし)")

    try:
        if hasattr(scale_prop, 'max_value'):
            log(f"max_value: {scale_prop.max_value}")
    except Exception as e:
        log(f"max_value: (なし)")

    # キーフレーム関連
    try:
        log(f"can_vary_over_time: {scale_prop.can_vary_over_time}")
    except:
        pass

    try:
        log(f"can_have_keyframes: {scale_prop.can_have_keyframes}")
    except:
        pass

    try:
        log(f"can_have_expression: {scale_prop.can_have_expression}")
    except:
        pass

    log("\n" + "=" * 60)
    log("Scale Value Test")
    log("=" * 60)

    # テストケース
    test_cases = [
        ([150.0, 150.0, 100.0], "Different Z (X=Y, Z≠X)"),
        ([100.0, 150.0, 200.0], "All Different (X≠Y≠Z)"),
        ([50.0, 100.0, 150.0], "Ascending (X<Y<Z)"),
        ([100.0, 100.0, 100.0], "All Same (X=Y=Z)"),
        ([200.0, 50.0, 125.0], "Random Values")
    ]

    for test_val, description in test_cases:
        try:
            log(f"\n[Test] {description}")
            log(f"  設定値: {test_val}")

            # 値を設定
            scale_prop.value = test_val

            # 即座に読み取り
            actual_val = list(scale_prop.value)
            log(f"  実際値: {actual_val}")

            # 比較
            matches = test_val == actual_val
            status = "✅ MATCH" if matches else "❌ DIFFER"
            log(f"  結果: {status}")

            if not matches:
                # 差分を詳細表示
                for i, axis in enumerate(['X', 'Y', 'Z']):
                    if abs(test_val[i] - actual_val[i]) > 0.001:
                        log(f"    >>> {axis} axis: {test_val[i]} → {actual_val[i]}")
                        if i == 2:  # Z軸
                            if abs(actual_val[2] - actual_val[0]) < 0.001:
                                log(f"        注: Z軸がX軸と同じ値に変更されました")
                            elif abs(actual_val[2] - actual_val[1]) < 0.001:
                                log(f"        注: Z軸がY軸と同じ値に変更されました")
                            elif abs(actual_val[2] - ((actual_val[0] + actual_val[1]) / 2)) < 0.001:
                                log(f"        注: Z軸がX/Yの平均値に変更されました")

        except Exception as e:
            log(f"  ❌ ERROR: {e}")

    log("\n" + "=" * 60)
    log("Position 3D との比較テスト")
    log("=" * 60)

    # Position 3D も同様にテスト（比較のため）
    try:
        pos_prop = anim_props.property("ADBE Text Position 3D")
        if pos_prop:
            log(f"Position 3D match_name: {pos_prop.match_name}")
            log(f"Position 3D default: {list(pos_prop.value)}")

            test_val = [50.0, 100.0, 0.0]
            pos_prop.value = test_val
            actual_val = list(pos_prop.value)

            log(f"Position Test: {test_val} → {actual_val}")
            if test_val == actual_val:
                log("  ✅ Position は正常に動作（3軸独立）")
            else:
                log("  ❌ Position も問題あり")
    except Exception as e:
        log(f"Position テスト失敗: {e}")

    log("\n" + "=" * 60)
    log("結論")
    log("=" * 60)

    # 全テストケースで Z軸が強制されているかチェック
    z_forced_count = 0
    for test_val, description in test_cases:
        scale_prop.value = test_val
        actual_val = list(scale_prop.value)
        if test_val != actual_val and abs(actual_val[2] - actual_val[0]) < 0.001:
            z_forced_count += 1

    if z_forced_count == len(test_cases):
        log("📊 全テストケースで Z軸が X軸と同じ値に強制されました")
        log("📋 判定: After Effects の仕様による制限の可能性が高い")
    elif z_forced_count > 0:
        log(f"📊 {z_forced_count}/{len(test_cases)} ケースで Z軸が変更されました")
        log("📋 判定: 条件付きで制約がある可能性")
    else:
        log("📊 Z軸の値が保持されました")
        log("📋 判定: PyAE の実装に問題がある可能性")

    # クリーンアップ
    try:
        comp.delete()
        log("\nテストコンポを削除しました")
    except:
        pass

    log("\n" + "=" * 60)
    log("調査完了")
    log("=" * 60)
    log(f"結果保存: {log_path}")

    log_file.close()
    return "OK"

if __name__ == "__main__":
    investigate_scale_property()
