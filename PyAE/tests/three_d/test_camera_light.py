# test_camera_light.py
# Phase 8: Camera/Light 固有プロパティのテスト
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


def log(msg):
    """ログ出力"""
    ae.log_info(f"[TEST] {msg}")


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
    log("Phase 8: Camera/Light テスト開始")
    log("=" * 60)

    project = ae.Project.get_current()
    if not project:
        test_fail("プロジェクト取得", "プロジェクトが開いていません")
        return None, None, None

    # テスト用コンポジション作成
    comp = project.create_comp("Camera_Light_Test", 1920, 1080, 1.0, 10.0, 30.0)
    log(f"テストコンポ作成: {comp.name}")

    # カメラレイヤー作成（カメラは常に3D）
    camera = comp.add_camera("Test_Camera", (960, 540))
    log(f"カメラ追加: {camera.name}")

    # ライトレイヤー作成
    light = comp.add_light("Test_Light", (960, 540))
    log(f"ライト追加: {light.name}")

    # カメラオプションを設定
    camera_values = {}
    log("カメラオプション設定中...")
    try:
        root = camera.properties
        camera_opts = root.property("ADBE Camera Options Group")
        if camera_opts and camera_opts.is_group:
            log(f"  Camera Options Group found: is_group={camera_opts.is_group}")

            # Zoom を設定
            zoom_prop = camera_opts.property("ADBE Camera Zoom")
            if zoom_prop:
                zoom_prop.value = 2000.0
                camera_values["Zoom"] = 2000.0
                log(f"  Zoom 設定: {zoom_prop.value}")

            # Focus Distance を設定
            focus_prop = camera_opts.property("ADBE Camera Focus Distance")
            if focus_prop:
                focus_prop.value = 1500.0
                camera_values["Focus Distance"] = 1500.0
                log(f"  Focus Distance 設定: {focus_prop.value}")

            # Aperture を設定
            aperture_prop = camera_opts.property("ADBE Camera Aperture")
            if aperture_prop:
                aperture_prop.value = 50.0
                camera_values["Aperture"] = 50.0
                log(f"  Aperture 設定: {aperture_prop.value}")
        else:
            log("  WARNING: Camera Options Group が見つかりません")
    except Exception as e:
        log(f"  ERROR: カメラオプション設定失敗: {e}")
        log(traceback.format_exc())

    # ライトオプションを設定
    light_values = {}
    log("ライトオプション設定中...")
    try:
        root = light.properties
        light_opts = root.property("ADBE Light Options Group")
        if light_opts and light_opts.is_group:
            log(f"  Light Options Group found: is_group={light_opts.is_group}")

            # Intensity を設定
            intensity_prop = light_opts.property("ADBE Light Intensity")
            if intensity_prop:
                intensity_prop.value = 150.0
                light_values["Intensity"] = 150.0
                log(f"  Intensity 設定: {intensity_prop.value}")

            # Shadow Darkness を設定
            shadow_prop = light_opts.property("ADBE Light Shadow Darkness")
            if shadow_prop:
                shadow_prop.value = 75.0
                light_values["Shadow Darkness"] = 75.0
                log(f"  Shadow Darkness 設定: {shadow_prop.value}")
        else:
            log("  WARNING: Light Options Group が見つかりません")
    except Exception as e:
        log(f"  ERROR: ライトオプション設定失敗: {e}")
        log(traceback.format_exc())

    return comp, camera_values, light_values


def run_export_import_test(comp, original_camera_values, original_light_values):
    """エクスポート/インポートテスト"""
    log("=" * 60)
    log("エクスポート/インポートテスト")
    log("=" * 60)

    docs_path = ae.get_documents_folder()
    export_file = os.path.join(docs_path, "camera_light_test.json")

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
    exported_camera_options = {}
    exported_light_options = {}

    try:
        with open(export_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        for item in data.get("items", []):
            if item.get("type") == "Comp" and item.get("name") == "Camera_Light_Test":
                comp_data = item.get("comp_data", {})
                for layer_data in comp_data.get("layers", []):
                    layer_type = layer_data.get("type", "").lower()

                    if layer_type == "camera":
                        exported_camera_options = layer_data.get("camera_options", {})
                        if exported_camera_options:
                            test_pass("Camera Options エクスポート", f"{len(exported_camera_options)} properties")
                            for k, v in exported_camera_options.items():
                                if isinstance(v, dict):
                                    log(f"    camera.{k} = {v.get('value')} (match_name: {v.get('match_name')})")
                                else:
                                    log(f"    camera.{k} = {v}")
                        else:
                            test_fail("Camera Options エクスポート", "camera_options が空です")

                    if layer_type == "light":
                        exported_light_options = layer_data.get("light_options", {})
                        if exported_light_options:
                            test_pass("Light Options エクスポート", f"{len(exported_light_options)} properties")
                            for k, v in exported_light_options.items():
                                if isinstance(v, dict):
                                    log(f"    light.{k} = {v.get('value')} (match_name: {v.get('match_name')})")
                                else:
                                    log(f"    light.{k} = {v}")
                        else:
                            test_fail("Light Options エクスポート", "light_options が空です")

    except Exception as e:
        test_fail("JSON確認", f"{e}\n{traceback.format_exc()}")
        return

    # 元のコンポジションを削除
    log("元のテストコンポを削除中...")
    try:
        project = ae.Project.get_current()
        for item in project.items:
            if hasattr(item, 'name') and item.name == "Camera_Light_Test":
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
        if hasattr(item, 'name') and item.name == "Camera_Light_Test":
            imported_comp = item
            break

    if not imported_comp:
        test_fail("インポートコンポ検索", "Camera_Light_Test が見つかりません")
        return

    log(f"インポートされたコンポ: {imported_comp.name} ({imported_comp.num_layers} layers)")

    # カメラの値を確認
    for i in range(imported_comp.num_layers):
        layer = imported_comp.layer(i)
        layer_type_str = str(layer.layer_type).lower()
        log(f"  Layer {i}: {layer.name} (type: {layer_type_str})")

        if "camera" in layer_type_str:
            log(f"  カメラレイヤー検出: {layer.name}")
            try:
                root = layer.properties
                camera_opts = root.property("ADBE Camera Options Group")
                if camera_opts and camera_opts.is_group:
                    for prop_name, expected_value in original_camera_values.items():
                        # match_nameで取得を試みる
                        match_names = {
                            "Zoom": "ADBE Camera Zoom",
                            "Focus Distance": "ADBE Camera Focus Distance",
                            "Aperture": "ADBE Camera Aperture"
                        }
                        match_name = match_names.get(prop_name)
                        prop = camera_opts.property(match_name) if match_name else None

                        if prop and not prop.is_group:
                            imported_value = prop.value
                            if compare_values(expected_value, imported_value):
                                test_pass(f"Camera {prop_name} 復元",
                                         f"Expected: {expected_value}, Imported: {imported_value}")
                            else:
                                test_fail(f"Camera {prop_name} 復元",
                                         f"Expected: {expected_value}, Imported: {imported_value}")
                        else:
                            test_fail(f"Camera {prop_name} 取得", f"プロパティが見つかりません: {match_name}")
            except Exception as e:
                test_fail("Camera Options 確認", f"{e}\n{traceback.format_exc()}")

        elif "light" in layer_type_str:
            log(f"  ライトレイヤー検出: {layer.name}")
            try:
                root = layer.properties
                light_opts = root.property("ADBE Light Options Group")
                if light_opts and light_opts.is_group:
                    for prop_name, expected_value in original_light_values.items():
                        match_names = {
                            "Intensity": "ADBE Light Intensity",
                            "Shadow Darkness": "ADBE Light Shadow Darkness"
                        }
                        match_name = match_names.get(prop_name)
                        prop = light_opts.property(match_name) if match_name else None

                        if prop and not prop.is_group:
                            imported_value = prop.value
                            if compare_values(expected_value, imported_value):
                                test_pass(f"Light {prop_name} 復元",
                                         f"Expected: {expected_value}, Imported: {imported_value}")
                            else:
                                test_fail(f"Light {prop_name} 復元",
                                         f"Expected: {expected_value}, Imported: {imported_value}")
                        else:
                            test_fail(f"Light {prop_name} 取得", f"プロパティが見つかりません: {match_name}")
            except Exception as e:
                test_fail("Light Options 確認", f"{e}\n{traceback.format_exc()}")


def run_tests():
    """テスト実行"""
    ae.begin_undo_group("Camera/Light Test")

    try:
        # テストシーン作成
        comp, camera_values, light_values = create_test_scene()
        if not comp:
            return

        # エクスポート/インポートテスト
        run_export_import_test(comp, camera_values, light_values)

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
    result_file = os.path.join(docs_path, "phase8_test_results.json")
    with open(result_file, "w", encoding="utf-8") as f:
        json.dump(test_results, f, indent=2, ensure_ascii=False)
    log(f"結果保存: {result_file}")

    # 失敗があれば詳細を表示
    if test_results["failed"] > 0:
        log("失敗したテスト:")
        for detail in test_results["details"]:
            if detail["status"] == "FAIL":
                log(f"  - {detail['name']}: {detail['detail']}")

    return "OK"


if __name__ == "__main__":
    run_tests()
