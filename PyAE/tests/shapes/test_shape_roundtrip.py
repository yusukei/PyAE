"""
test_shape_roundtrip.py
Shape Layer の完全なラウンドトリップ検証スクリプト

Phase 7.3: Build Verification

使用方法:
1. After Effects で空のプロジェクトを開く
2. このスクリプトを実行
3. ログ出力で結果を確認

テストケース:
- Rectangle Shape (パラメトリックシェイプ)
- Ellipse Shape (パラメトリックシェイプ)
- Custom Path Shape (ベジェパス)
- Gradient Fill
- Repeater
"""

import ae
import json
import os
import struct
import traceback

# 出力先
DOCS_PATH = ae.get_documents_folder()
EXPORT_JSON = os.path.join(DOCS_PATH, "shape_roundtrip_test.json")

# ログレベル
LOG_LEVEL = 3  # 3=DEBUG

def log(msg, level="INFO"):
    """ログ出力"""
    levels = {"ERROR": 0, "WARNING": 1, "INFO": 2, "DEBUG": 3}
    if levels.get(level, 2) <= LOG_LEVEL:
        ae.log_info(f"[{level}] {msg}")

def log_error(msg): log(msg, "ERROR")
def log_warning(msg): log(msg, "WARNING")
def log_info(msg): log(msg, "INFO")
def log_debug(msg): log(msg, "DEBUG")


class ShapeRoundtripTest:
    """Shape Layer ラウンドトリップテスト"""

    def __init__(self):
        self.project = ae.Project.get_current()
        self.test_comp = None
        self.results = {
            "total": 0,
            "passed": 0,
            "failed": 0,
            "details": []
        }

    def run(self):
        """テスト実行"""
        log_info("=" * 60)
        log_info("Shape Layer Roundtrip Test - Phase 7.3")
        log_info("=" * 60)

        ae.begin_undo_group("Shape Roundtrip Test")

        try:
            # Step 1: テスト用コンポジション作成
            self._create_test_comp()

            # Step 2: テストシェイプを作成
            self._create_test_shapes()

            # Step 3: エクスポート
            self._export_scene()

            # Step 4: 新しいコンポジションにインポート
            self._import_scene()

            # Step 5: 比較検証
            self._verify_roundtrip()

            # 結果サマリー
            self._print_summary()

        except Exception as e:
            log_error(f"テスト失敗: {e}")
            log_error(traceback.format_exc())

        finally:
            ae.end_undo_group()
            ae.refresh()

        return self.results

    def _create_test_comp(self):
        """テスト用コンポジション作成"""
        log_info("Step 1: Creating test composition...")
        self.test_comp = self.project.create_comp(
            "Shape_Roundtrip_Test",
            1920, 1080,
            1.0,  # pixel aspect
            10.0, # duration
            30.0  # fps
        )
        log_info(f"  Created: {self.test_comp.name}")

    def _create_test_shapes(self):
        """テストシェイプを作成"""
        log_info("Step 2: Creating test shape layers...")

        # Test 1: Rectangle
        self._create_rectangle_shape()

        # Test 2: Ellipse
        self._create_ellipse_shape()

        # Test 3: Custom Path (Star)
        self._create_custom_path_shape()

        # Test 4: Gradient Fill
        self._create_gradient_shape()

        # Test 5: Repeater
        self._create_repeater_shape()

        log_info(f"  Created {self.test_comp.num_layers} shape layers")

    def _create_rectangle_shape(self):
        """Rectangle シェイプレイヤー作成"""
        log_debug("  Creating Rectangle shape...")
        layer = self.test_comp.add_shape()
        layer.name = "Test_Rectangle"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                # Rectangle Group を追加
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Rectangle Group")

                    # Rectangle Shape を追加
                    if group.can_add_stream("ADBE Vector Shape - Rect"):
                        rect = group.add_stream("ADBE Vector Shape - Rect")
                        # サイズ設定
                        size_prop = rect.property("ADBE Vector Rect Size")
                        if size_prop:
                            size_prop.value = [200.0, 150.0]
                        log_debug("    Rectangle shape added")

                    # Fill を追加
                    if group.can_add_stream("ADBE Vector Graphic - Fill"):
                        fill = group.add_stream("ADBE Vector Graphic - Fill")
                        color_prop = fill.property("ADBE Vector Fill Color")
                        if color_prop:
                            color_prop.value = [1.0, 0.0, 0.0, 1.0]  # Red
                        log_debug("    Fill added (Red)")

                    # Stroke を追加
                    if group.can_add_stream("ADBE Vector Graphic - Stroke"):
                        stroke = group.add_stream("ADBE Vector Graphic - Stroke")
                        stroke_color = stroke.property("ADBE Vector Stroke Color")
                        if stroke_color:
                            stroke_color.value = [0.0, 0.0, 0.0, 1.0]  # Black
                        stroke_width = stroke.property("ADBE Vector Stroke Width")
                        if stroke_width:
                            stroke_width.value = 3.0
                        log_debug("    Stroke added (Black, 3px)")
        except Exception as e:
            log_warning(f"    Rectangle creation error: {e}")

    def _create_ellipse_shape(self):
        """Ellipse シェイプレイヤー作成"""
        log_debug("  Creating Ellipse shape...")
        layer = self.test_comp.add_shape()
        layer.name = "Test_Ellipse"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Ellipse Group")

                    # Ellipse Shape を追加
                    if group.can_add_stream("ADBE Vector Shape - Ellipse"):
                        ellipse = group.add_stream("ADBE Vector Shape - Ellipse")
                        size_prop = ellipse.property("ADBE Vector Ellipse Size")
                        if size_prop:
                            size_prop.value = [180.0, 120.0]
                        log_debug("    Ellipse shape added")

                    # Fill を追加
                    if group.can_add_stream("ADBE Vector Graphic - Fill"):
                        fill = group.add_stream("ADBE Vector Graphic - Fill")
                        color_prop = fill.property("ADBE Vector Fill Color")
                        if color_prop:
                            color_prop.value = [0.0, 1.0, 0.0, 1.0]  # Green
                        log_debug("    Fill added (Green)")
        except Exception as e:
            log_warning(f"    Ellipse creation error: {e}")

    def _create_custom_path_shape(self):
        """Custom Path (Star) シェイプレイヤー作成"""
        log_debug("  Creating Custom Path (Star) shape...")
        layer = self.test_comp.add_shape()
        layer.name = "Test_CustomPath"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Star Group")

                    # Path Shape を追加
                    if group.can_add_stream("ADBE Vector Shape - Group"):
                        path = group.add_stream("ADBE Vector Shape - Group")

                        # スター型のパスを作成（5点）
                        import math
                        vertices = []
                        num_points = 10  # 5 outer + 5 inner
                        outer_radius = 100.0
                        inner_radius = 40.0

                        for i in range(num_points):
                            angle = (i * 36 - 90) * math.pi / 180  # 36度ずつ
                            radius = outer_radius if i % 2 == 0 else inner_radius
                            x = radius * math.cos(angle)
                            y = radius * math.sin(angle)
                            vertices.append({
                                "x": x, "y": y,
                                "tan_in_x": 0, "tan_in_y": 0,
                                "tan_out_x": 0, "tan_out_y": 0
                            })

                        path_prop = path.property("ADBE Vector Shape")
                        if path_prop and hasattr(path_prop, 'set_shape_path_vertices'):
                            path_prop.set_shape_path_vertices(vertices, 0.0)
                            log_debug(f"    Custom path added ({len(vertices)} vertices)")

                    # Fill を追加
                    if group.can_add_stream("ADBE Vector Graphic - Fill"):
                        fill = group.add_stream("ADBE Vector Graphic - Fill")
                        color_prop = fill.property("ADBE Vector Fill Color")
                        if color_prop:
                            color_prop.value = [1.0, 1.0, 0.0, 1.0]  # Yellow
                        log_debug("    Fill added (Yellow)")
        except Exception as e:
            log_warning(f"    Custom path creation error: {e}")

    def _create_gradient_shape(self):
        """Gradient Fill シェイプレイヤー作成"""
        log_debug("  Creating Gradient Fill shape...")
        layer = self.test_comp.add_shape()
        layer.name = "Test_GradientFill"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Gradient Group")

                    # Rectangle を追加
                    if group.can_add_stream("ADBE Vector Shape - Rect"):
                        rect = group.add_stream("ADBE Vector Shape - Rect")
                        size_prop = rect.property("ADBE Vector Rect Size")
                        if size_prop:
                            size_prop.value = [250.0, 150.0]

                    # Gradient Fill を追加
                    if group.can_add_stream("ADBE Vector Graphic - G-Fill"):
                        gfill = group.add_stream("ADBE Vector Graphic - G-Fill")
                        log_debug("    Gradient Fill added")
        except Exception as e:
            log_warning(f"    Gradient creation error: {e}")

    def _create_repeater_shape(self):
        """Repeater シェイプレイヤー作成"""
        log_debug("  Creating Repeater shape...")
        layer = self.test_comp.add_shape()
        layer.name = "Test_Repeater"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Repeater Group")

                    # Ellipse を追加
                    if group.can_add_stream("ADBE Vector Shape - Ellipse"):
                        ellipse = group.add_stream("ADBE Vector Shape - Ellipse")
                        size_prop = ellipse.property("ADBE Vector Ellipse Size")
                        if size_prop:
                            size_prop.value = [50.0, 50.0]

                    # Fill を追加
                    if group.can_add_stream("ADBE Vector Graphic - Fill"):
                        fill = group.add_stream("ADBE Vector Graphic - Fill")
                        color_prop = fill.property("ADBE Vector Fill Color")
                        if color_prop:
                            color_prop.value = [0.0, 0.5, 1.0, 1.0]  # Blue

                    # Repeater を追加
                    if group.can_add_stream("ADBE Vector Filter - Repeater"):
                        repeater = group.add_stream("ADBE Vector Filter - Repeater")
                        copies_prop = repeater.property("ADBE Vector Repeater Copies")
                        if copies_prop:
                            copies_prop.value = 5.0
                        log_debug("    Repeater added (5 copies)")
        except Exception as e:
            log_warning(f"    Repeater creation error: {e}")

    def _export_scene(self):
        """シーンをエクスポート"""
        log_info("Step 3: Exporting scene...")

        # export_scene.py と同じ形式でエクスポート
        from export_scene import export_project
        export_project(EXPORT_JSON)

        if os.path.exists(EXPORT_JSON):
            size = os.path.getsize(EXPORT_JSON)
            log_info(f"  Exported to: {EXPORT_JSON}")
            log_info(f"  File size: {size:,} bytes")
        else:
            log_error("  Export failed: file not created")

    def _import_scene(self):
        """シーンをインポート（新しいコンポジションとして）"""
        log_info("Step 4: Importing scene to new composition...")

        # JSONを読み込んでコンポジション名を変更
        with open(EXPORT_JSON, 'r', encoding='utf-8') as f:
            data = json.load(f)

        # テストコンポジションを探して名前を変更
        for item in data.get("items", []):
            if item.get("type") == "Comp" and item.get("name") == "Shape_Roundtrip_Test":
                item["name"] = "Shape_Roundtrip_Imported"
                break

        # 変更したJSONを保存
        import_json = os.path.join(DOCS_PATH, "shape_roundtrip_import.json")
        with open(import_json, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)

        # インポート実行
        from import_scene import import_project
        import_project(import_json)

        log_info("  Import completed")

    def _verify_roundtrip(self):
        """ラウンドトリップ検証"""
        log_info("Step 5: Verifying roundtrip...")

        # インポートされたコンポジションを検索
        imported_comp = None
        for item in self.project.items:
            if item.type == ae.ItemType.Comp and item.name == "Shape_Roundtrip_Imported":
                imported_comp = item
                break

        if not imported_comp:
            log_error("  Imported composition not found!")
            self.results["failed"] += 1
            return

        log_info(f"  Original: {self.test_comp.name} ({self.test_comp.num_layers} layers)")
        log_info(f"  Imported: {imported_comp.name} ({imported_comp.num_layers} layers)")

        # レイヤー数の比較
        self._verify_layer_count(imported_comp)

        # 各レイヤーの詳細比較
        for i in range(self.test_comp.num_layers):
            orig_layer = self.test_comp.layer(i)

            # 同名のレイヤーを検索
            imp_layer = None
            for j in range(imported_comp.num_layers):
                l = imported_comp.layer(j)
                if l.name == orig_layer.name:
                    imp_layer = l
                    break

            if imp_layer:
                self._verify_layer(orig_layer, imp_layer)
            else:
                self._add_result(
                    f"Layer: {orig_layer.name}",
                    False,
                    "Imported layer not found"
                )

    def _verify_layer_count(self, imported_comp):
        """レイヤー数の検証"""
        orig_count = self.test_comp.num_layers
        imp_count = imported_comp.num_layers

        self._add_result(
            "Layer count",
            orig_count == imp_count,
            f"Original: {orig_count}, Imported: {imp_count}"
        )

    def _verify_layer(self, orig_layer, imp_layer):
        """レイヤーの検証"""
        layer_name = orig_layer.name
        log_debug(f"  Verifying: {layer_name}")

        # レイヤータイプの検証
        type_match = orig_layer.layer_type == imp_layer.layer_type
        self._add_result(
            f"{layer_name} - Type",
            type_match,
            f"Original: {orig_layer.layer_type}, Imported: {imp_layer.layer_type}"
        )

        # Shape Layer の場合、コンテンツを検証
        if orig_layer.layer_type == ae.LayerType.Shape:
            self._verify_shape_contents(orig_layer, imp_layer)

    def _verify_shape_contents(self, orig_layer, imp_layer):
        """シェイプコンテンツの検証"""
        layer_name = orig_layer.name

        try:
            orig_root = orig_layer.properties
            imp_root = imp_layer.properties

            orig_contents = orig_root.property("ADBE Root Vectors Group")
            imp_contents = imp_root.property("ADBE Root Vectors Group")

            if not orig_contents or not imp_contents:
                self._add_result(
                    f"{layer_name} - Contents",
                    False,
                    "Contents group not found"
                )
                return

            # グループ数の比較
            orig_groups = self._count_shape_groups(orig_contents)
            imp_groups = self._count_shape_groups(imp_contents)

            self._add_result(
                f"{layer_name} - Groups",
                orig_groups == imp_groups,
                f"Original: {orig_groups}, Imported: {imp_groups}"
            )

            # Custom Path の場合、頂点数を検証
            if "CustomPath" in layer_name:
                self._verify_path_vertices(orig_contents, imp_contents, layer_name)

        except Exception as e:
            self._add_result(
                f"{layer_name} - Contents",
                False,
                f"Error: {e}"
            )

    def _count_shape_groups(self, contents):
        """シェイプグループ数をカウント"""
        count = 0
        try:
            for i in range(contents.num_properties):
                prop = contents.property(i)
                if prop and prop.match_name == "ADBE Vector Group":
                    count += 1
        except:
            pass
        return count

    def _verify_path_vertices(self, orig_contents, imp_contents, layer_name):
        """パス頂点の検証"""
        try:
            # 最初のグループの Path Shape を取得
            orig_path = self._find_path_property(orig_contents)
            imp_path = self._find_path_property(imp_contents)

            if orig_path and imp_path:
                orig_vertices = orig_path.get_shape_path_vertices(0.0) if hasattr(orig_path, 'get_shape_path_vertices') else None
                imp_vertices = imp_path.get_shape_path_vertices(0.0) if hasattr(imp_path, 'get_shape_path_vertices') else None

                if orig_vertices and imp_vertices:
                    vertices_match = len(orig_vertices) == len(imp_vertices)
                    self._add_result(
                        f"{layer_name} - Vertices",
                        vertices_match,
                        f"Original: {len(orig_vertices)}, Imported: {len(imp_vertices)}"
                    )
                else:
                    self._add_result(
                        f"{layer_name} - Vertices",
                        False,
                        "Could not get vertices"
                    )
            else:
                self._add_result(
                    f"{layer_name} - Path",
                    False,
                    "Path property not found"
                )
        except Exception as e:
            self._add_result(
                f"{layer_name} - Vertices",
                False,
                f"Error: {e}"
            )

    def _find_path_property(self, contents):
        """パスプロパティを検索"""
        try:
            for i in range(contents.num_properties):
                group = contents.property(i)
                if group and group.match_name == "ADBE Vector Group":
                    for j in range(group.num_properties):
                        prop = group.property(j)
                        if prop and prop.match_name == "ADBE Vector Shape - Group":
                            return prop.property("ADBE Vector Shape")
        except:
            pass
        return None

    def _add_result(self, name, passed, detail):
        """テスト結果を追加"""
        self.results["total"] += 1
        if passed:
            self.results["passed"] += 1
            status = "PASS"
        else:
            self.results["failed"] += 1
            status = "FAIL"

        self.results["details"].append({
            "name": name,
            "passed": passed,
            "detail": detail
        })

        log_info(f"  [{status}] {name}: {detail}")

    def _print_summary(self):
        """結果サマリーを表示"""
        log_info("")
        log_info("=" * 60)
        log_info("TEST SUMMARY")
        log_info("=" * 60)
        log_info(f"Total:  {self.results['total']}")
        log_info(f"Passed: {self.results['passed']}")
        log_info(f"Failed: {self.results['failed']}")

        if self.results["failed"] == 0:
            log_info("")
            log_info("All tests PASSED!")
            ae.alert(f"Shape Roundtrip Test PASSED!\n\nTotal: {self.results['total']}\nPassed: {self.results['passed']}")
        else:
            log_info("")
            log_info("Some tests FAILED:")
            for r in self.results["details"]:
                if not r["passed"]:
                    log_info(f"  - {r['name']}: {r['detail']}")
            ae.alert(f"Shape Roundtrip Test FAILED!\n\nTotal: {self.results['total']}\nPassed: {self.results['passed']}\nFailed: {self.results['failed']}")


def main():
    """メイン処理"""
    test = ShapeRoundtripTest()
    results = test.run()
    return results


if __name__ == "__main__":
    main()
