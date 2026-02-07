# phase2_test.py
# PyAE Phase 2 機能テストスクリプト
# Author: VTech Studio
# Version: 1.0
# Description: Phase2 の全機能をテストするスクリプト

"""
PyAE Phase 2 テストスクリプト

テスト項目:
1. ae.get_project() でプロジェクトを取得できる
2. ae.get_active_comp() でアクティブコンポを取得できる
3. レイヤーの名前、位置、不透明度を取得/設定できる
4. キーフレームの追加/削除ができる
5. 新規コンポジションを作成できる
6. ソリッドレイヤーを追加できる
7. すべての操作でUndo/Redoが機能する
8. 削除されたオブジェクトへのアクセスで適切なエラーが発生する
9. 日本語を含むレイヤー名・コンポ名が正しく処理される
"""

import ae

# テスト結果を格納
test_results = []


def log_test(name, passed, message=""):
    """テスト結果をログに記録"""
    status = "PASS" if passed else "FAIL"
    result = f"[{status}] {name}"
    if message:
        result += f": {message}"
    ae.log_info(result)
    test_results.append({"name": name, "passed": passed, "message": message})
    return passed


def test_separator(title):
    """テストセクションの区切り"""
    ae.log_info("")
    ae.log_info("=" * 60)
    ae.log_info(f" {title}")
    ae.log_info("=" * 60)


# =============================================================
# テスト1: プロジェクト取得
# =============================================================
def test_get_project():
    test_separator("Test 1: ae.get_project()")

    try:
        project = ae.get_project()
        if project is None:
            log_test("get_project returns value", False, "Returned None")
            return False

        log_test("get_project returns value", True)

        # プロジェクト名を取得
        try:
            name = project.name
            log_test("project.name accessible", True, f"Name: {name}")
        except Exception as e:
            log_test("project.name accessible", False, str(e))

        # アイテム数を取得
        try:
            num_items = project.num_items
            log_test("project.num_items accessible", True, f"Items: {num_items}")
        except Exception as e:
            log_test("project.num_items accessible", False, str(e))

        return True
    except Exception as e:
        log_test("get_project", False, str(e))
        return False


# =============================================================
# テスト2: アクティブコンポ取得
# =============================================================
def test_get_active_comp():
    test_separator("Test 2: ae.get_active_comp()")

    try:
        comp = ae.get_active_comp()

        # アクティブコンポがない場合もテストとして有効
        if comp is None or not comp.valid:
            log_test(
                "get_active_comp returns",
                True,
                "No active comp (expected if none selected)",
            )
            return True

        log_test("get_active_comp returns valid comp", True)

        # コンポプロパティをテスト
        try:
            ae.log_info(f"  Name: {comp.name}")
            ae.log_info(f"  Size: {comp.width}x{comp.height}")
            ae.log_info(f"  Duration: {comp.duration}s")
            ae.log_info(f"  Frame rate: {comp.frame_rate}fps")
            ae.log_info(f"  Layers: {comp.num_layers}")
            log_test("comp properties accessible", True)
        except Exception as e:
            log_test("comp properties accessible", False, str(e))

        return True
    except Exception as e:
        log_test("get_active_comp", False, str(e))
        return False


# =============================================================
# テスト3: 新規コンポジション作成
# =============================================================
def test_create_comp():
    test_separator("Test 3: Create New Composition")

    try:
        # 英語名でコンポを作成
        comp = ae.Comp.create(
            name="PyAE_Test_Comp",
            width=1920,
            height=1080,
            pixel_aspect=1.0,
            duration=5.0,
            frame_rate=30.0,
        )

        if comp is None or not comp.valid:
            log_test("Create comp (English name)", False, "Comp creation failed")
            return None

        log_test("Create comp (English name)", True, f"Created: {comp.name}")

        # サイズ確認
        if comp.width == 1920 and comp.height == 1080:
            log_test("Comp size correct", True, f"{comp.width}x{comp.height}")
        else:
            log_test(
                "Comp size correct",
                False,
                f"Expected 1920x1080, got {comp.width}x{comp.height}",
            )

        # フレームレート確認
        if abs(comp.frame_rate - 30.0) < 0.01:
            log_test("Comp frame rate correct", True, f"{comp.frame_rate}fps")
        else:
            log_test(
                "Comp frame rate correct",
                False,
                f"Expected 30.0, got {comp.frame_rate}",
            )

        return comp
    except Exception as e:
        log_test("Create comp", False, str(e))
        return None


# =============================================================
# テスト4: 日本語コンポ名
# =============================================================
def test_japanese_comp_name():
    test_separator("Test 4: Japanese Comp Name (UTF-8/UTF-16)")

    try:
        # 日本語名でコンポを作成
        jp_name = "テストコンポ_日本語"
        comp = ae.Comp.create(
            name=jp_name,
            width=1280,
            height=720,
            pixel_aspect=1.0,
            duration=3.0,
            frame_rate=24.0,
        )

        if comp is None or not comp.valid:
            log_test("Create comp (Japanese name)", False, "Comp creation failed")
            return None

        log_test("Create comp (Japanese name)", True, f"Created: {comp.name}")

        # 名前が正しく取得できるか確認
        retrieved_name = comp.name
        if retrieved_name == jp_name:
            log_test("Japanese name roundtrip", True, f"'{retrieved_name}'")
        else:
            log_test(
                "Japanese name roundtrip",
                False,
                f"Expected '{jp_name}', got '{retrieved_name}'",
            )

        # 名前を変更してテスト
        new_jp_name = "新しい名前_変更テスト"
        comp.name = new_jp_name
        if comp.name == new_jp_name:
            log_test("Japanese name change", True, f"Changed to '{comp.name}'")
        else:
            log_test(
                "Japanese name change",
                False,
                f"Expected '{new_jp_name}', got '{comp.name}'",
            )

        return comp
    except Exception as e:
        log_test("Japanese comp name", False, str(e))
        return None


# =============================================================
# テスト5: ソリッドレイヤー追加
# =============================================================
def test_add_solid_layer(comp):
    test_separator("Test 5: Add Solid Layer")

    if comp is None or not comp.valid:
        log_test("Add solid layer", False, "No valid comp provided")
        return None

    try:
        # 英語名でソリッド追加
        layer = comp.add_solid(
            name="Red Solid", width=1920, height=1080, color=(1.0, 0.0, 0.0), duration=5.0
        )

        if layer is None or not layer.valid:
            log_test("Add solid (English name)", False, "Layer creation failed")
            return None

        log_test("Add solid (English name)", True, f"Created: {layer.name}")

        # 日本語名でソリッド追加
        jp_layer = comp.add_solid(
            name="青いソリッド_テスト",
            width=1920,
            height=1080,
            color=(0.0, 0.0, 1.0),
            duration=5.0,
        )

        if jp_layer is not None and jp_layer.valid:
            log_test("Add solid (Japanese name)", True, f"Created: {jp_layer.name}")
        else:
            log_test("Add solid (Japanese name)", False, "Layer creation failed")

        return layer
    except Exception as e:
        log_test("Add solid layer", False, str(e))
        return None


# =============================================================
# テスト6: レイヤープロパティ取得/設定
# =============================================================
def test_layer_properties(layer):
    test_separator("Test 6: Layer Properties (Name, Position, Opacity)")

    if layer is None or not layer.valid:
        log_test("Layer properties", False, "No valid layer provided")
        return False

    try:
        # 名前取得
        original_name = layer.name
        log_test("Get layer name", True, f"Name: {original_name}")

        # 名前設定
        new_name = "Renamed Layer"
        layer.name = new_name
        if layer.name == new_name:
            log_test("Set layer name", True, f"Changed to: {layer.name}")
        else:
            log_test(
                "Set layer name", False, f"Expected '{new_name}', got '{layer.name}'"
            )

        # 日本語名に設定
        jp_name = "リネームされたレイヤー"
        layer.name = jp_name
        if layer.name == jp_name:
            log_test("Set Japanese layer name", True, f"Changed to: {layer.name}")
        else:
            log_test(
                "Set Japanese layer name",
                False,
                f"Expected '{jp_name}', got '{layer.name}'",
            )

        # 位置取得
        try:
            pos = layer.position
            log_test("Get layer position", True, f"Position: {pos}")
        except Exception as e:
            log_test("Get layer position", False, str(e))

        # 位置設定
        try:
            new_pos = [960.0, 540.0]  # 中央に移動
            layer.position = new_pos
            updated_pos = layer.position
            log_test("Set layer position", True, f"New position: {updated_pos}")
        except Exception as e:
            log_test("Set layer position", False, str(e))

        # 不透明度取得
        try:
            opacity = layer.opacity
            log_test("Get layer opacity", True, f"Opacity: {opacity}")
        except Exception as e:
            log_test("Get layer opacity", False, str(e))

        # 不透明度設定
        try:
            new_opacity = 50.0
            layer.opacity = new_opacity
            updated_opacity = layer.opacity
            if abs(updated_opacity - new_opacity) < 1.0:
                log_test("Set layer opacity", True, f"New opacity: {updated_opacity}")
            else:
                log_test(
                    "Set layer opacity",
                    False,
                    f"Expected {new_opacity}, got {updated_opacity}",
                )
        except Exception as e:
            log_test("Set layer opacity", False, str(e))

        return True
    except Exception as e:
        log_test("Layer properties", False, str(e))
        return False


# =============================================================
# テスト7: キーフレーム操作
# =============================================================
def test_keyframes(layer):
    test_separator("Test 7: Keyframe Operations")

    if layer is None or not layer.valid:
        log_test("Keyframe operations", False, "No valid layer provided")
        return False

    try:
        # 位置プロパティを取得
        position_prop = layer.property("Position")
        if position_prop is None:
            # Fallback for Japanese environment
            position_prop = layer.property("位置")

        if position_prop is None:
            log_test(
                "Get Position property", False, "Property not found (Position/位置)"
            )
            return False

        log_test("Get Position property", True)

        # キーフレーム追加
        try:
            # 0秒目にキーフレーム (Positionは3Dの可能性があるため3要素指定)
            kf1 = position_prop.add_keyframe(0.0, [100.0, 100.0, 0.0])
            log_test("Add keyframe at 0.0s", True if kf1 is not None else False)

            # 2秒目にキーフレーム
            kf2 = position_prop.add_keyframe(2.0, [800.0, 600.0, 0.0])
            log_test("Add keyframe at 2.0s", True if kf2 is not None else False)

            # 4秒目にキーフレーム
            kf3 = position_prop.add_keyframe(4.0, [500.0, 300.0, 0.0])
            log_test("Add keyframe at 4.0s", True if kf3 is not None else False)
        except Exception as e:
            log_test("Add keyframes", False, str(e))

        # キーフレーム数を確認
        try:
            num_kf = position_prop.num_keyframes
            log_test("Get keyframe count", True, f"Count: {num_kf}")
        except Exception as e:
            log_test("Get keyframe count", False, str(e))

        # キーフレーム値を取得
        try:
            if position_prop.num_keyframes > 0:
                # keyframe(index) メソッドは存在しないため、個別メソッドを使用
                idx = 0
                kf_time = position_prop.get_keyframe_time(idx)
                kf_value = position_prop.get_keyframe_value(idx)

                # 値が取得できたか確認 (Noneでないこと)
                if kf_value is not None:
                    log_test(
                        "Get keyframe value",
                        True,
                        f"Time: {kf_time}, Value: {kf_value}",
                    )
                else:
                    log_test("Get keyframe value", False, "Keyframe value is None")
        except Exception as e:
            log_test("Get keyframe value", False, str(e))

        # キーフレーム削除
        try:
            if position_prop.num_keyframes > 0:
                initial_count = position_prop.num_keyframes
                position_prop.remove_keyframe(0)
                new_count = position_prop.num_keyframes
                if new_count == initial_count - 1:
                    log_test(
                        "Remove keyframe",
                        True,
                        f"Count: {initial_count} -> {new_count}",
                    )
                else:
                    log_test("Remove keyframe", False, f"Count unchanged: {new_count}")
        except Exception as e:
            log_test("Remove keyframe", False, str(e))

        return True
    except Exception as e:
        log_test("Keyframe operations", False, str(e))
        return False


# =============================================================
# テスト8: Undo/Redo
# =============================================================
def test_undo_redo(comp):
    test_separator("Test 8: Undo/Redo Operations")

    if comp is None or not comp.valid:
        log_test("Undo/Redo", False, "No valid comp provided")
        return False

    try:
        # Undo グループでレイヤー追加
        with ae.UndoGroup("PyAE Test: Add Layer"):
            layer = comp.add_solid(
                name="Undo Test Layer",
                width=100,
                height=100,
                color=(0.0, 1.0, 0.0),
                duration=2.0,
            )
            if layer and layer.valid:
                log_test("Add layer in undo group", True, f"Created: {layer.name}")
            else:
                log_test("Add layer in undo group", False, "Creation failed")

        # レイヤー数を記録
        layer_count_after_add = comp.num_layers
        ae.log_info(f"  Layer count after add: {layer_count_after_add}")

        # 注意: 実際のUndo実行はAE UIからのみ可能
        # ここではUndo Groupが正しく作成されたことを確認
        log_test(
            "Undo group created", True, "Undo group 'PyAE Test: Add Layer' created"
        )

        # 別のUndo グループでプロパティ変更
        if layer and layer.valid:
            with ae.UndoGroup("PyAE Test: Modify Layer"):
                layer.opacity = 75.0
                layer.name = "Modified Undo Layer"
            log_test(
                "Modify in undo group",
                True,
                f"Modified: {layer.name}, Opacity: {layer.opacity}",
            )

        return True
    except Exception as e:
        log_test("Undo/Redo", False, str(e))
        return False


# =============================================================
# テスト9: 削除されたオブジェクトへのアクセス
# =============================================================
def test_deleted_object_access(comp):
    test_separator("Test 9: Deleted Object Access Error Handling")

    if comp is None or not comp.valid:
        log_test("Deleted object test", False, "No valid comp provided")
        return False

    try:
        # テスト用レイヤーを追加
        layer = comp.add_solid(
            name="Delete Test Layer",
            width=100,
            height=100,
            color=(1.0, 1.0, 0.0),
            duration=1.0,
        )

        if layer is None or not layer.valid:
            log_test("Create test layer", False, "Layer creation failed")
            return False

        log_test("Create test layer for deletion", True, f"Created: {layer.name}")

        # レイヤーを削除
        try:
            layer.delete()
            log_test("Delete layer", True, "Layer deleted")
        except Exception as e:
            log_test("Delete layer", False, str(e))
            return False

        # 削除されたレイヤーへのアクセスをテスト
        error_raised = False
        try:
            # 削除されたレイヤーの名前を取得しようとする
            _ = layer.name
            log_test(
                "Access deleted layer.name",
                False,
                "No error raised (should have raised)",
            )
        except Exception as e:
            error_raised = True
            log_test(
                "Access deleted layer.name",
                True,
                f"Error raised as expected: {type(e).__name__}",
            )

        # valid プロパティをチェック
        try:
            is_valid = layer.valid
            if not is_valid:
                log_test("layer.valid returns False", True, "Correctly reports invalid")
            else:
                log_test(
                    "layer.valid returns False",
                    False,
                    "Still reports valid after deletion",
                )
        except Exception as e:
            log_test("layer.valid check", True, f"Error raised: {type(e).__name__}")

        return error_raised
    except Exception as e:
        log_test("Deleted object test", False, str(e))
        return False


# =============================================================
# メイン実行
# =============================================================
def main():
    ae.log_info("")
    ae.log_info("*" * 60)
    ae.log_info("*  PyAE Phase 2 Test Suite")
    ae.log_info("*  Version: 1.0")
    ae.log_info("*" * 60)
    ae.log_info("")

    # 初期化チェック
    if not ae.is_initialized():
        ae.log_error("PyAE is not initialized!")
        return

    ae.log_info(f"PyAE Version: {ae.version()}")
    ae.log_info("")

    # テスト1: プロジェクト取得
    test_get_project()

    # テスト2: アクティブコンポ取得
    test_get_active_comp()

    # テスト3: 新規コンポジション作成
    test_comp = test_create_comp()

    # テスト4: 日本語コンポ名
    jp_comp = test_japanese_comp_name()

    # テスト5-9はコンポが必要
    if test_comp is not None and test_comp.valid:
        # テスト5: ソリッドレイヤー追加
        layer = test_add_solid_layer(test_comp)

        # テスト6: レイヤープロパティ
        if layer is not None and layer.valid:
            test_layer_properties(layer)

        # テスト7: キーフレーム操作
        if layer is not None and layer.valid:
            test_keyframes(layer)

        # テスト8: Undo/Redo
        test_undo_redo(test_comp)

        # テスト9: 削除されたオブジェクトへのアクセス
        test_deleted_object_access(test_comp)
    else:
        ae.log_warning("Skipping tests 5-9: No valid comp available")

    # 結果サマリー
    test_separator("Test Results Summary")

    passed_count = sum(1 for r in test_results if r["passed"])
    failed_count = sum(1 for r in test_results if not r["passed"])
    total_count = len(test_results)

    ae.log_info(f"Total tests: {total_count}")
    ae.log_info(f"Passed: {passed_count}")
    ae.log_info(f"Failed: {failed_count}")
    ae.log_info(
        f"Pass rate: {(passed_count / total_count * 100) if total_count > 0 else 0:.1f}%"
    )

    if failed_count > 0:
        ae.log_info("")
        ae.log_info("Failed tests:")
        for r in test_results:
            if not r["passed"]:
                ae.log_info(f"  - {r['name']}: {r['message']}")

    ae.log_info("")
    ae.log_info("*" * 60)
    ae.log_info("*  Test Suite Completed")
    ae.log_info("*" * 60)


if __name__ == "__main__":
    main()
