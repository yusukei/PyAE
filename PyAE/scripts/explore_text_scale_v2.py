# explore_text_scale_v2.py
# threeDPerChar 仮説の検証スクリプト
# 報告書 Section 6.1 に基づく実装

import ae
import os

# ログファイル
log_path = os.path.join(ae.get_documents_folder(), "scale_context_investigation.txt")
log_file = open(log_path, "w", encoding="utf-8")

def log(msg):
    """ログ出力"""
    ae.log_info(f"[SCALE_CONTEXT] {msg}")
    log_file.write(msg + "\n")
    log_file.flush()

def investigate_scale_with_context():
    """threeDPerChar コンテキストによる Scale 挙動の検証"""
    log("=" * 60)
    log("Scale Z-axis Behavior with threeDPerChar Context")
    log("=" * 60)

    project = ae.get_project()
    if not project:
        log("ERROR: プロジェクトが開いていません")
        return "ERROR"

    # テストコンポジション作成
    comp = project.create_comp("Scale_Context_Test", 1920, 1080, 1.0, 5.0, 30.0)
    text_layer = comp.add_text("Context Test")
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

    target_val = [150.0, 150.0, 100.0]

    log("\n" + "=" * 60)
    log("TEST 1: Default 2D Context (threeDPerChar = false)")
    log("=" * 60)

    # デフォルト状態（2D）での設定テスト
    scale_prop.value = target_val
    actual_1 = list(scale_prop.value)

    log(f"  設定値: {target_val}")
    log(f"  実際値: {actual_1}")

    if actual_1 == [150.0, 150.0, 150.0]:
        log("  ✅ CONFIRMED: Z軸が2Dモードでクランプされました")
        test1_passed = True
    elif actual_1 == target_val:
        log("  ❓ UNEXPECTED: Z軸がクランプされていません（既に3D?）")
        test1_passed = False
    else:
        log(f"  ❓ UNEXPECTED: 予期しない値: {actual_1}")
        test1_passed = False

    log("\n" + "=" * 60)
    log("TEST 2: Switching Context to 3D (threeDPerChar = true)")
    log("=" * 60)

    # ExtendScript で threeDPerChar を有効化
    # コンポジションとレイヤーのインデックスを取得
    try:
        # コンポジション内のレイヤーインデックスは1始まり
        layer_index = text_layer.index

        # ExtendScript を実行
        # app.project.activeItem を使用するため、先にコンポをアクティブにする
        toggle_script = f"""
        (function() {{
            var comp = null;
            for (var i = 1; i <= app.project.numItems; i++) {{
                if (app.project.item(i).name === "{comp.name}") {{
                    comp = app.project.item(i);
                    break;
                }}
            }}
            if (comp) {{
                var layer = comp.layer({layer_index});
                var before = layer.threeDPerChar;
                layer.threeDPerChar = true;
                var after = layer.threeDPerChar;
                return "threeDPerChar: " + before + " -> " + after;
            }}
            return "ERROR: Comp not found";
        }})();
        """

        result = ae.execute_extendscript(toggle_script)
        log(f"  ExtendScript結果: {result}")
        log("  レイヤーを3Dモードに切り替えました")

    except Exception as e:
        log(f"  ❌ ExtendScript実行エラー: {e}")
        log("  Note: execute_script が利用できない場合があります")

    log("\n" + "=" * 60)
    log("TEST 3: 3D Context Assignment (threeDPerChar = true)")
    log("=" * 60)

    # 3D状態での設定テスト（再設定が必要）
    scale_prop.value = target_val
    actual_2 = list(scale_prop.value)

    log(f"  設定値: {target_val}")
    log(f"  実際値: {actual_2}")

    if actual_2 == [150.0, 150.0, 100.0]:
        log("  ✅ SUCCESS: 3DモードでZ軸が保持されました!")
        test3_passed = True
    else:
        log("  ❌ FAILURE: 3DモードでもZ軸がクランプされています")
        test3_passed = False

    log("\n" + "=" * 60)
    log("TEST 4: 追加検証 - X ≠ Y のケース")
    log("=" * 60)

    # X ≠ Y でもZ軸が保持されるか（3Dモード時）
    test_val_2 = [100.0, 150.0, 200.0]
    scale_prop.value = test_val_2
    actual_3 = list(scale_prop.value)

    log(f"  設定値: {test_val_2}")
    log(f"  実際値: {actual_3}")

    if actual_3 == test_val_2:
        log("  ✅ SUCCESS: X≠YでもZ軸が独立して保持されました!")
        test4_passed = True
    else:
        log("  ⚠️  部分的成功: X≠Y時はZ軸に制約がある可能性")
        test4_passed = False

    log("\n" + "=" * 60)
    log("総合結論")
    log("=" * 60)

    if test3_passed:
        log("🎉 仮説検証成功: threeDPerChar = true で Scale Z軸が保持されます")
        log("📋 推奨アクション:")
        log("   1. export_scene.py に threeDPerChar エクスポートを追加")
        log("   2. import_scene.py で threeDPerChar を先に適用")
        log("   3. その後 Animator プロパティを設定")
    else:
        log("❌ 仮説検証失敗: threeDPerChar では解決しない可能性")
        log("   追加調査が必要です")

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
    investigate_scale_with_context()
