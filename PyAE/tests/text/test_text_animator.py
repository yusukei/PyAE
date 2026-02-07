# test_text_animator.py
# Phase 9: Text Animator のエクスポート/インポートテスト
# After Effects で実行してください

import ae
import json
import os
import traceback

# テスト結果
test_results = {
    "total": 0,
    "passed": 0,
    "failed": 0,
    "details": []
}

# ファイルベースのログ
LOG_PATH = os.path.join(os.path.expanduser("~"), "Documents", "text_animator_test_log.txt")
_log_file = None


def init_log():
    """ログファイルを初期化"""
    global _log_file
    try:
        _log_file = open(LOG_PATH, "w", encoding="utf-8")
        _log_file.write("=== Text Animator Test Log ===\n")
        _log_file.flush()
    except Exception as e:
        ae.log_info(f"ログファイル初期化失敗: {e}")


def close_log():
    """ログファイルを閉じる"""
    global _log_file
    if _log_file:
        try:
            _log_file.close()
        except:
            pass


def log(msg):
    """ログ出力"""
    ae.log_info(f"[TEST] {msg}")
    if _log_file:
        try:
            _log_file.write(f"{msg}\n")
            _log_file.flush()
        except:
            pass


def test_pass(name, detail=""):
    """テスト成功"""
    test_results["total"] += 1
    test_results["passed"] += 1
    test_results["details"].append({"name": name, "status": "PASS", "detail": detail})
    log(f"[PASS] {name}")


def test_fail(name, detail=""):
    """テスト失敗"""
    test_results["total"] += 1
    test_results["failed"] += 1
    test_results["details"].append({"name": name, "status": "FAIL", "detail": detail})
    log(f"[FAIL] {name}: {detail}")


def compare_values(original, imported, tolerance=0.01):
    """値の比較（浮動小数点の誤差を考慮）"""
    if isinstance(original, (list, tuple)) and isinstance(imported, (list, tuple)):
        if len(original) != len(imported):
            return False
        return all(abs(o - i) < tolerance for o, i in zip(original, imported))
    elif isinstance(original, (int, float)) and isinstance(imported, (int, float)):
        return abs(original - imported) < tolerance
    else:
        return original == imported


def create_test_scene():
    """テスト用シーンを作成"""
    log("=" * 60)
    log("Phase 9: Text Animator テスト開始")
    log("=" * 60)

    project = ae.Project.get_current()
    if not project:
        test_fail("プロジェクト取得", "プロジェクトが開いていません")
        return None, None

    # テスト用コンポジション作成
    comp = project.create_comp("TextAnimator_Test", 1920, 1080, 1.0, 10.0, 30.0)
    log(f"テストコンポ作成: {comp.name}")

    # テキストレイヤー作成
    text_layer = comp.add_text("Hello World")
    text_layer.name = "Test_Text"
    log(f"テキストレイヤー追加: {text_layer.name}")

    # Animator を追加して設定値を記録
    animator_values = {}

    log("Text Animator を追加中...")

    # threeDPerChar を有効化（Scale Z 軸の独立制御に必要）
    try:
        comp_name = comp.name
        layer_index = text_layer.index + 1  # AE ExtendScript は 1-based インデックス
        three_d_script = f'''
        (function() {{
            var comp = null;
            for (var i = 1; i <= app.project.numItems; i++) {{
                if (app.project.item(i).name === "{comp_name}") {{
                    comp = app.project.item(i);
                    break;
                }}
            }}
            if (comp && comp.layer({layer_index})) {{
                comp.layer({layer_index}).threeDPerChar = true;
                return "OK";
            }}
            return "ERROR: Layer not found";
        }})();
        '''
        result = ae.execute_extendscript(three_d_script)
        log(f"  threeDPerChar 設定結果: {result}")

        # 設定後の状態を確認
        verify_script = f'''
        (function() {{
            for (var i = 1; i <= app.project.numItems; i++) {{
                if (app.project.item(i).name === "{comp_name}") {{
                    return "threeDPerChar=" + app.project.item(i).layer({layer_index}).threeDPerChar;
                }}
            }}
            return "Comp not found";
        }})();
        '''
        verify_result = ae.execute_extendscript(verify_script)
        log(f"  threeDPerChar 確認: {verify_result}")

        # AE の状態を更新
        ae.refresh()
    except Exception as e:
        log(f"  WARNING: threeDPerChar 適用失敗 (Scale Z復元に影響する可能性): {e}")
        import traceback
        log(f"  {traceback.format_exc()}")

    try:
        root = text_layer.properties
        text_props = root.property("ADBE Text Properties")
        if not text_props:
            test_fail("Text Properties取得", "Text Propertiesが見つかりません")
            return comp, None

        animators_group = text_props.property("ADBE Text Animators")
        if not animators_group:
            test_fail("Text Animators取得", "Text Animatorsが見つかりません")
            return comp, None

        # Animator を追加
        if not animators_group.can_add_stream("ADBE Text Animator"):
            test_fail("Animator追加可能チェック", "Animatorを追加できません")
            return comp, None

        animator = animators_group.add_stream("ADBE Text Animator")
        if not animator:
            test_fail("Animator追加", "Animator追加失敗")
            return comp, None

        animator.set_name("Test_Animator_1")
        log(f"  Animator追加: {animator.name}")

        # Animator Properties を設定
        anim_props = animator.property("ADBE Text Animator Properties")
        if anim_props and anim_props.is_group:
            log(f"  Animator Properties: {anim_props.num_properties} properties")

            # Position を設定
            pos_prop = anim_props.property("ADBE Text Position 3D")
            if pos_prop and not pos_prop.is_group:
                pos_prop.value = [50.0, 100.0, 0.0]
                animator_values["Position"] = {
                    "match_name": "ADBE Text Position 3D",
                    "value": [50.0, 100.0, 0.0]
                }
                log(f"    Position = {pos_prop.value}")

            # Opacity を設定
            opacity_prop = anim_props.property("ADBE Text Opacity")
            if opacity_prop and not opacity_prop.is_group:
                opacity_prop.value = 50.0
                animator_values["Opacity"] = {
                    "match_name": "ADBE Text Opacity",
                    "value": 50.0
                }
                log(f"    Opacity = {opacity_prop.value}")

            # Scale を設定
            scale_prop = anim_props.property("ADBE Text Scale 3D")
            if scale_prop and not scale_prop.is_group:
                scale_prop.value = [150.0, 150.0, 100.0]
                animator_values["Scale"] = {
                    "match_name": "ADBE Text Scale 3D",
                    "value": [150.0, 150.0, 100.0]
                }
                log(f"    Scale = {scale_prop.value}")

            # Rotation を設定
            rotation_prop = anim_props.property("ADBE Text Rotation")
            if rotation_prop and not rotation_prop.is_group:
                rotation_prop.value = 45.0
                animator_values["Rotation"] = {
                    "match_name": "ADBE Text Rotation",
                    "value": 45.0
                }
                log(f"    Rotation = {rotation_prop.value}")

        # Selector を設定
        selectors_group = animator.property("ADBE Text Selectors")
        if selectors_group and selectors_group.is_group:
            log(f"  Selectors: {selectors_group.num_properties} selectors")

            # デフォルトでRange Selectorが存在するか、または追加
            selector = None
            if selectors_group.num_properties > 0:
                selector = selectors_group.property(0)
            elif selectors_group.can_add_stream("ADBE Text Selector"):
                selector = selectors_group.add_stream("ADBE Text Selector")

            if selector:
                log(f"    Selector: {selector.name}")

                # 高度設定にアクセスして「単位」を確認
                advanced = selector.property("ADBE Text Range Advanced")
                if advanced and advanced.is_group:
                    units_prop = advanced.property("ADBE Text Range Units")
                    if units_prop:
                        log(f"      現在の単位: {units_prop.value} (1=パーセント, 2=インデックス)")

                # Index ベースのプロパティを設定 (これらは常に存在)
                start_prop = selector.property("ADBE Text Index Start")
                if start_prop and not start_prop.is_group:
                    start_prop.value = 2.0  # 2文字目から
                    animator_values["Selector_Start"] = {
                        "match_name": "ADBE Text Index Start",
                        "value": 2.0
                    }
                    log(f"      Index Start = {start_prop.value}")

                end_prop = selector.property("ADBE Text Index End")
                if end_prop and not end_prop.is_group:
                    end_prop.value = 8.0  # 8文字目まで
                    animator_values["Selector_End"] = {
                        "match_name": "ADBE Text Index End",
                        "value": 8.0
                    }
                    log(f"      Index End = {end_prop.value}")

                offset_prop = selector.property("ADBE Text Index Offset")
                if offset_prop and not offset_prop.is_group:
                    offset_prop.value = 1.0
                    animator_values["Selector_Offset"] = {
                        "match_name": "ADBE Text Index Offset",
                        "value": 1.0
                    }
                    log(f"      Index Offset = {offset_prop.value}")

        test_pass("Animator作成", f"{len(animator_values)} プロパティ設定")

    except Exception as e:
        test_fail("Animator設定", f"{e}\n{traceback.format_exc()}")
        return comp, None

    return comp, animator_values


def run_export_import_test(comp, original_values):
    """エクスポート/インポートテスト"""
    log("=" * 60)
    log("エクスポート/インポートテスト")
    log("=" * 60)

    docs_path = ae.get_documents_folder()
    export_file = os.path.join(docs_path, "text_animator_test.json")

    # export_scene.py を使ってエクスポート
    log("エクスポート実行中...")
    try:
        from export_scene import export_project
        export_project(export_file)
        log(f"  エクスポート完了: {export_file}")
        test_pass("エクスポート完了")
    except Exception as e:
        test_fail("エクスポート", f"{e}\n{traceback.format_exc()}")
        return

    # JSONを確認
    log("エクスポートJSON確認中...")
    exported_animators = []

    try:
        with open(export_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        for item in data.get("items", []):
            if item.get("type") == "Comp" and item.get("name") == "TextAnimator_Test":
                comp_data = item.get("comp_data", {})
                for layer_data in comp_data.get("layers", []):
                    if layer_data.get("type", "").lower() == "text":
                        exported_animators = layer_data.get("text_animators", [])
                        if exported_animators:
                            test_pass("Text Animators エクスポート", f"{len(exported_animators)} animators")
                            log(f"    エクスポートされた Animator 数: {len(exported_animators)}")

                            for anim in exported_animators:
                                log(f"      Animator: {anim.get('name')}")
                                props = anim.get("properties", {})
                                for pname, pdata in props.items():
                                    if isinstance(pdata, dict):
                                        log(f"        {pname} = {pdata.get('value')}")
                        else:
                            test_fail("Text Animators エクスポート", "text_animators が空です")

    except Exception as e:
        test_fail("JSON確認", f"{e}\n{traceback.format_exc()}")
        return

    # 元のコンポジションを削除
    log("元のテストコンポを削除中...")
    try:
        project = ae.Project.get_current()
        for item in project.items:
            if hasattr(item, 'name') and item.name == "TextAnimator_Test":
                item.delete()
                log("  削除完了")
                break
    except Exception as e:
        log(f"  WARNING: 元コンポ削除失敗: {e}")

    # インポート
    log("インポート実行中...")
    try:
        from import_scene import import_project
        success = import_project(export_file)
        if success:
            test_pass("インポート完了")
        else:
            test_fail("インポート", "インポート処理がFalseを返しました")
            return
    except Exception as e:
        test_fail("インポート", f"{e}\n{traceback.format_exc()}")
        return

    # インポート後の値を確認
    log("インポート後の値確認中...")

    project = ae.Project.get_current()
    imported_comp = None
    for item in project.items:
        if hasattr(item, 'name') and item.name == "TextAnimator_Test":
            imported_comp = item
            break

    if not imported_comp:
        test_fail("インポートコンポ検索", "TextAnimator_Test が見つかりません")
        return

    log(f"インポートされたコンポ: {imported_comp.name} ({imported_comp.num_layers} layers)")

    # テキストレイヤーを探す
    for i in range(imported_comp.num_layers):
        layer = imported_comp.layer(i)
        layer_type_str = str(layer.layer_type).lower()

        if "text" in layer_type_str:
            log(f"  テキストレイヤー検出: {layer.name}")
            try:
                root = layer.properties
                text_props = root.property("ADBE Text Properties")
                if not text_props:
                    test_fail("Text Properties確認", "Text Propertiesが見つかりません")
                    continue

                animators_group = text_props.property("ADBE Text Animators")
                if not animators_group or not animators_group.is_group:
                    test_fail("Text Animators確認", "Text Animatorsが見つかりません")
                    continue

                log(f"    Animators数: {animators_group.num_properties}")

                if animators_group.num_properties == 0:
                    test_fail("Animator復元", "Animatorが0個です")
                    continue

                # 最初のAnimatorを確認
                animator = animators_group.property(0)
                if animator:
                    log(f"    Animator: {animator.name}")

                    # Animator Properties を確認
                    anim_props = animator.property("ADBE Text Animator Properties")
                    if anim_props and anim_props.is_group:
                        # Position を確認
                        if "Position" in original_values:
                            expected = original_values["Position"]["value"]
                            pos_prop = anim_props.property("ADBE Text Position 3D")
                            if pos_prop and not pos_prop.is_group:
                                imported_val = list(pos_prop.value) if hasattr(pos_prop.value, '__iter__') else pos_prop.value
                                if compare_values(expected, imported_val):
                                    test_pass("Position復元", f"Expected: {expected}, Got: {imported_val}")
                                else:
                                    test_fail("Position復元", f"Expected: {expected}, Got: {imported_val}")
                            else:
                                test_fail("Position取得", "Positionプロパティが見つかりません")

                        # Opacity を確認
                        if "Opacity" in original_values:
                            expected = original_values["Opacity"]["value"]
                            opacity_prop = anim_props.property("ADBE Text Opacity")
                            if opacity_prop and not opacity_prop.is_group:
                                imported_val = opacity_prop.value
                                if compare_values(expected, imported_val):
                                    test_pass("Opacity復元", f"Expected: {expected}, Got: {imported_val}")
                                else:
                                    test_fail("Opacity復元", f"Expected: {expected}, Got: {imported_val}")
                            else:
                                test_fail("Opacity取得", "Opacityプロパティが見つかりません")

                        # Scale を確認
                        if "Scale" in original_values:
                            expected = original_values["Scale"]["value"]
                            scale_prop = anim_props.property("ADBE Text Scale 3D")
                            if scale_prop and not scale_prop.is_group:
                                imported_val = list(scale_prop.value) if hasattr(scale_prop.value, '__iter__') else scale_prop.value
                                if compare_values(expected, imported_val):
                                    test_pass("Scale復元", f"Expected: {expected}, Got: {imported_val}")
                                else:
                                    test_fail("Scale復元", f"Expected: {expected}, Got: {imported_val}")
                            else:
                                test_fail("Scale取得", "Scaleプロパティが見つかりません")

                        # Rotation を確認
                        if "Rotation" in original_values:
                            expected = original_values["Rotation"]["value"]
                            rotation_prop = anim_props.property("ADBE Text Rotation")
                            if rotation_prop and not rotation_prop.is_group:
                                imported_val = rotation_prop.value
                                if compare_values(expected, imported_val):
                                    test_pass("Rotation復元", f"Expected: {expected}, Got: {imported_val}")
                                else:
                                    test_fail("Rotation復元", f"Expected: {expected}, Got: {imported_val}")
                            else:
                                test_fail("Rotation取得", "Rotationプロパティが見つかりません")

                    # Selector を確認
                    selectors_group = animator.property("ADBE Text Selectors")
                    if selectors_group and selectors_group.is_group and selectors_group.num_properties > 0:
                        selector = selectors_group.property(0)
                        if selector:
                            # Start を確認 (Index ベース)
                            if "Selector_Start" in original_values:
                                expected = original_values["Selector_Start"]["value"]
                                match_name = original_values["Selector_Start"]["match_name"]
                                start_prop = selector.property(match_name)
                                if start_prop and not start_prop.is_group:
                                    imported_val = start_prop.value
                                    if compare_values(expected, imported_val):
                                        test_pass("Selector Start復元", f"Expected: {expected}, Got: {imported_val}")
                                    else:
                                        test_fail("Selector Start復元", f"Expected: {expected}, Got: {imported_val}")

                            # End を確認 (Index ベース)
                            if "Selector_End" in original_values:
                                expected = original_values["Selector_End"]["value"]
                                match_name = original_values["Selector_End"]["match_name"]
                                end_prop = selector.property(match_name)
                                if end_prop and not end_prop.is_group:
                                    imported_val = end_prop.value
                                    if compare_values(expected, imported_val):
                                        test_pass("Selector End復元", f"Expected: {expected}, Got: {imported_val}")
                                    else:
                                        test_fail("Selector End復元", f"Expected: {expected}, Got: {imported_val}")

                            # Offset を確認 (Index ベース)
                            if "Selector_Offset" in original_values:
                                expected = original_values["Selector_Offset"]["value"]
                                match_name = original_values["Selector_Offset"]["match_name"]
                                offset_prop = selector.property(match_name)
                                if offset_prop and not offset_prop.is_group:
                                    imported_val = offset_prop.value
                                    if compare_values(expected, imported_val):
                                        test_pass("Selector Offset復元", f"Expected: {expected}, Got: {imported_val}")
                                    else:
                                        test_fail("Selector Offset復元", f"Expected: {expected}, Got: {imported_val}")

            except Exception as e:
                test_fail("値確認", f"{e}\n{traceback.format_exc()}")


def run_tests():
    """テスト実行"""
    init_log()  # ファイルログ初期化
    log(f"Log file: {LOG_PATH}")

    ae.begin_undo_group("Text Animator Test")

    try:
        # テストシーン作成
        comp, animator_values = create_test_scene()
        if not comp or not animator_values:
            return

        # エクスポート/インポートテスト
        run_export_import_test(comp, animator_values)

    except Exception as e:
        test_fail("テスト実行", f"{e}\n{traceback.format_exc()}")

    finally:
        ae.end_undo_group()

    # 結果サマリー
    log("=" * 60)
    log("テスト結果サマリー")
    log("=" * 60)
    log(f"Total:  {test_results['total']}")
    log(f"Passed: {test_results['passed']}")
    log(f"Failed: {test_results['failed']}")
    success_rate = test_results['passed'] / max(test_results['total'], 1) * 100
    log(f"Success Rate: {success_rate:.0f}%")
    log("=" * 60)

    # 結果をJSONに保存
    docs_path = ae.get_documents_folder()
    result_file = os.path.join(docs_path, "phase9_test_results.json")
    with open(result_file, "w", encoding="utf-8") as f:
        json.dump(test_results, f, indent=2, ensure_ascii=False)
    log(f"結果保存: {result_file}")

    # 失敗があれば詳細を表示
    if test_results["failed"] > 0:
        log("失敗したテスト:")
        for detail in test_results["details"]:
            if detail["status"] == "FAIL":
                log(f"  - {detail['name']}: {detail['detail']}")

    log(f"Log file saved to: {LOG_PATH}")
    close_log()  # ファイルログ閉じる

    return "OK"


if __name__ == "__main__":
    run_tests()
