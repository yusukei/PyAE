# test_expression.py
# Phase 10: Expression のエクスポート/インポートテスト

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
LOG_PATH = os.path.join(os.path.expanduser("~"), "Documents", "expression_test_log.txt")
_log_file = None


def init_log():
    """ログファイルを初期化"""
    global _log_file
    try:
        _log_file = open(LOG_PATH, "w", encoding="utf-8")
        _log_file.write("=== Expression Test Log ===\n")
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
    """値の比較"""
    if isinstance(original, (list, tuple)) and isinstance(imported, (list, tuple)):
        if len(original) != len(imported):
            return False
        return all(abs(o - i) < tolerance for o, i in zip(original, imported))
    elif isinstance(original, (int, float)) and isinstance(imported, (int, float)):
        return abs(original - imported) < tolerance
    else:
        return original == imported


def create_test_scene():
    """テスト用シーンを作成（様々な Expression を設定）"""
    log("=" * 60)
    log("Phase 10: Expression テスト開始")
    log("=" * 60)

    project = ae.Project.get_current()
    if not project:
        test_fail("プロジェクト取得", "プロジェクトが開いていません")
        return None, None

    # テスト用コンポジション作成
    comp = project.create_comp("Expression_Test", 1920, 1080, 1.0, 10.0, 30.0)
    log(f"テストコンポ作成: {comp.name}")

    # テスト用のExpression設定値を記録
    expression_values = {}

    # --- ソリッドレイヤー作成 ---
    # add_solid(name, width, height, r, g, b, duration)
    solid_layer = comp.add_solid("Expression_Solid", 1920, 1080, (1.0, 0.5, 0.0), 10.0)
    log(f"ソリッドレイヤー追加: {solid_layer.name}")

    try:
        root = solid_layer.properties
        transform = root.property("ADBE Transform Group")
        if transform and transform.is_group:
            log(f"  Transform Group 取得成功")

            # 1. Position に単純な Expression を設定
            position_prop = transform.property("ADBE Position")
            if position_prop and not position_prop.is_group:
                log(f"    Position: can_have_expression = {position_prop.can_have_expression}")
                if position_prop.can_have_expression:
                    expr1 = "[thisComp.width/2, thisComp.height/2]"
                    position_prop.expression = expr1
                    expression_values["Position"] = {
                        "match_name": "ADBE Position",
                        "expression": expr1
                    }
                    log(f"    Position Expression 設定: {expr1}")

            # 2. Rotation に time ベースの Expression を設定
            rotation_prop = transform.property("ADBE Rotate Z")
            if rotation_prop and not rotation_prop.is_group:
                log(f"    Rotation: can_have_expression = {rotation_prop.can_have_expression}")
                if rotation_prop.can_have_expression:
                    expr2 = "time * 90"
                    rotation_prop.expression = expr2
                    expression_values["Rotation"] = {
                        "match_name": "ADBE Rotate Z",
                        "expression": expr2
                    }
                    log(f"    Rotation Expression 設定: {expr2}")

            # 3. Opacity に wiggle Expression を設定
            opacity_prop = transform.property("ADBE Opacity")
            if opacity_prop and not opacity_prop.is_group:
                log(f"    Opacity: can_have_expression = {opacity_prop.can_have_expression}")
                if opacity_prop.can_have_expression:
                    expr3 = "wiggle(2, 20) + 80"
                    opacity_prop.expression = expr3
                    expression_values["Opacity"] = {
                        "match_name": "ADBE Opacity",
                        "expression": expr3
                    }
                    log(f"    Opacity Expression 設定: {expr3}")

            # 4. Scale に条件付き Expression を設定
            scale_prop = transform.property("ADBE Scale")
            if scale_prop and not scale_prop.is_group:
                log(f"    Scale: can_have_expression = {scale_prop.can_have_expression}")
                if scale_prop.can_have_expression:
                    expr4 = "s = 100 + Math.sin(time * 3) * 20; [s, s]"
                    scale_prop.expression = expr4
                    expression_values["Scale"] = {
                        "match_name": "ADBE Scale",
                        "expression": expr4
                    }
                    log(f"    Scale Expression 設定: {expr4}")

        # Expression 設定確認
        if len(expression_values) >= 4:
            test_pass("Expression設定", f"{len(expression_values)} プロパティに設定")
        else:
            test_fail("Expression設定", f"設定数が不足: {len(expression_values)}")

    except Exception as e:
        test_fail("Expression設定", f"{e}\n{traceback.format_exc()}")
        return comp, None

    # --- テキストレイヤーを追加 ---
    text_layer = comp.add_text("Expression Test Text")
    text_layer.name = "Expression_Text"
    log(f"テキストレイヤー追加: {text_layer.name}")

    # ソーステキストに Expression を設定
    try:
        text_root = text_layer.properties
        text_props = text_root.property("ADBE Text Properties")
        if text_props and text_props.is_group:
            source_text_prop = text_props.property("ADBE Text Document")
            if source_text_prop:
                log(f"    Source Text: can_have_expression = {source_text_prop.can_have_expression}")
                # Note: テキストドキュメントは通常 Expression を持てない
                # ただし確認のためにテストする
                if source_text_prop.can_have_expression:
                    try:
                        expr5 = '"Frame: " + Math.floor(time * thisComp.frameRate)'
                        source_text_prop.expression = expr5
                        expression_values["SourceText"] = {
                            "match_name": "ADBE Text Document",
                            "expression": expr5
                        }
                        log(f"    Source Text Expression 設定: {expr5}")
                    except Exception as e:
                        log(f"    Source Text Expression 設定失敗（予期される動作）: {e}")
                else:
                    log(f"    Source Text は Expression を持てません（予期される動作）")

    except Exception as e:
        log(f"  テキストレイヤー処理エラー: {e}")

    return comp, expression_values


def run_export_import_test(comp, original_values):
    """エクスポート → インポート → 値比較テスト"""
    log("=" * 60)
    log("エクスポート/インポートテスト")
    log("=" * 60)

    # エクスポート
    docs_path = ae.get_documents_folder()
    export_file = os.path.join(docs_path, "expression_test.json")

    log("エクスポート実行中...")
    try:
        from export_scene import export_project
        export_project(export_file)
        test_pass("エクスポート完了")
        log(f"  エクスポート完了: {export_file}")
    except Exception as e:
        test_fail("エクスポート", f"{e}\n{traceback.format_exc()}")
        return

    # JSONを確認
    log("エクスポートJSON確認中...")
    try:
        with open(export_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        # Expression がエクスポートされているか再帰的に確認
        def count_expressions(obj, path=""):
            """再帰的に Expression を数える"""
            count = 0
            if isinstance(obj, dict):
                if "expression" in obj:
                    expr = obj["expression"]
                    log(f"    Expression検出: {path} = {expr[:50] if len(expr) > 50 else expr}...")
                    count += 1
                for key, value in obj.items():
                    count += count_expressions(value, f"{path}/{key}")
            elif isinstance(obj, list):
                for i, item in enumerate(obj):
                    count += count_expressions(item, f"{path}[{i}]")
            return count

        expression_count = count_expressions(data)

        if expression_count >= len(original_values):
            test_pass("Expressionエクスポート", f"{expression_count} expressions")
        else:
            test_fail("Expressionエクスポート", f"Expected >= {len(original_values)}, Got {expression_count}")

    except Exception as e:
        test_fail("JSON確認", f"{e}")
        return

    # 元のコンポジションを削除
    log("元のテストコンポを削除中...")
    try:
        project = ae.Project.get_current()
        for item in project.items:
            if hasattr(item, 'name') and item.name == "Expression_Test":
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

    # インポート後の Expression を確認
    log("インポート後の値確認中...")

    project = ae.Project.get_current()
    imported_comp = None
    for item in project.items:
        if hasattr(item, 'name') and item.name == "Expression_Test":
            imported_comp = item
            break

    if not imported_comp:
        test_fail("インポートコンポ検索", "Expression_Test が見つかりません")
        return

    log(f"インポートされたコンポ: {imported_comp.name} ({imported_comp.num_layers} layers)")

    # ソリッドレイヤーを探して Expression を確認
    for i in range(imported_comp.num_layers):
        layer = imported_comp.layer(i)
        if layer.name == "Expression_Solid":
            log(f"  ソリッドレイヤー検出: {layer.name}")
            try:
                root = layer.properties
                transform = root.property("ADBE Transform Group")
                if not transform or not transform.is_group:
                    test_fail("Transform取得", "Transform Groupが見つかりません")
                    continue

                # Position Expression を確認
                if "Position" in original_values:
                    expected_expr = original_values["Position"]["expression"]
                    pos_prop = transform.property("ADBE Position")
                    if pos_prop and pos_prop.has_expression:
                        imported_expr = pos_prop.expression
                        if expected_expr == imported_expr:
                            test_pass("Position Expression復元", f"'{expected_expr}'")
                        else:
                            test_fail("Position Expression復元", f"Expected: '{expected_expr}', Got: '{imported_expr}'")
                    else:
                        test_fail("Position Expression復元", "Expression が見つかりません")

                # Rotation Expression を確認
                if "Rotation" in original_values:
                    expected_expr = original_values["Rotation"]["expression"]
                    rot_prop = transform.property("ADBE Rotate Z")
                    if rot_prop and rot_prop.has_expression:
                        imported_expr = rot_prop.expression
                        if expected_expr == imported_expr:
                            test_pass("Rotation Expression復元", f"'{expected_expr}'")
                        else:
                            test_fail("Rotation Expression復元", f"Expected: '{expected_expr}', Got: '{imported_expr}'")
                    else:
                        test_fail("Rotation Expression復元", "Expression が見つかりません")

                # Opacity Expression を確認
                if "Opacity" in original_values:
                    expected_expr = original_values["Opacity"]["expression"]
                    opa_prop = transform.property("ADBE Opacity")
                    if opa_prop and opa_prop.has_expression:
                        imported_expr = opa_prop.expression
                        if expected_expr == imported_expr:
                            test_pass("Opacity Expression復元", f"'{expected_expr}'")
                        else:
                            test_fail("Opacity Expression復元", f"Expected: '{expected_expr}', Got: '{imported_expr}'")
                    else:
                        test_fail("Opacity Expression復元", "Expression が見つかりません")

                # Scale Expression を確認
                if "Scale" in original_values:
                    expected_expr = original_values["Scale"]["expression"]
                    scale_prop = transform.property("ADBE Scale")
                    if scale_prop and scale_prop.has_expression:
                        imported_expr = scale_prop.expression
                        if expected_expr == imported_expr:
                            test_pass("Scale Expression復元", f"'{expected_expr}'")
                        else:
                            test_fail("Scale Expression復元", f"Expected: '{expected_expr}', Got: '{imported_expr}'")
                    else:
                        test_fail("Scale Expression復元", "Expression が見つかりません")

            except Exception as e:
                test_fail("値確認", f"{e}\n{traceback.format_exc()}")


def run_tests():
    """テスト実行"""
    init_log()
    log(f"Log file: {LOG_PATH}")

    ae.begin_undo_group("Expression Test")

    try:
        # テストシーン作成
        comp, expression_values = create_test_scene()
        if not comp or not expression_values:
            return

        # エクスポート/インポートテスト
        run_export_import_test(comp, expression_values)

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
    result_file = os.path.join(docs_path, "phase10_test_results.json")
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
    close_log()

    return "OK"


if __name__ == "__main__":
    run_tests()
