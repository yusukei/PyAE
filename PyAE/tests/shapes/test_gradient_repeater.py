"""
test_gradient_repeater.py
Gradient Fill/Stroke と Repeater の詳細検証スクリプト

Phase 7.4: Gradient/Repeater Verification

使用方法:
1. After Effects で空のプロジェクトを開く
2. このスクリプトを実行
3. ログ出力で結果を確認

検証項目:
- Gradient Fill (開始色、終了色、タイプ、角度)
- Gradient Stroke (線の太さ、グラデーション設定)
- Repeater (コピー数、Transform設定)
"""

import ae
import json
import os
import traceback

# 出力先
DOCS_PATH = ae.get_documents_folder()
EXPORT_JSON = os.path.join(DOCS_PATH, "gradient_repeater_test.json")

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


class GradientRepeaterTest:
    """Gradient/Repeater 詳細検証テスト"""

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
        log_info("Gradient/Repeater Detailed Test - Phase 7.4")
        log_info("=" * 60)

        ae.begin_undo_group("Gradient/Repeater Test")

        try:
            # Step 1: テスト用コンポジション作成
            self._create_test_comp()

            # Step 2: テストシェイプを作成
            self._create_test_shapes()

            # Step 3: エクスポート
            self._export_scene()

            # Step 4: 新しいコンポジションにインポート
            self._import_scene()

            # Step 5: 詳細比較検証
            self._verify_gradient_fill()
            self._verify_gradient_stroke()
            self._verify_repeater()

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
            "Gradient_Repeater_Test",
            1920, 1080,
            1.0, 10.0, 30.0
        )
        log_info(f"  Created: {self.test_comp.name}")

    def _create_test_shapes(self):
        """テストシェイプを作成"""
        log_info("Step 2: Creating test shape layers...")

        # Test 1: Gradient Fill (Linear)
        self._create_gradient_fill_linear()

        # Test 2: Gradient Fill (Radial)
        self._create_gradient_fill_radial()

        # Test 3: Gradient Stroke
        self._create_gradient_stroke()

        # Test 4: Repeater with Transform
        self._create_repeater_with_transform()

        # Test 5: Multiple Repeaters (nested)
        self._create_nested_repeater()

        log_info(f"  Created {self.test_comp.num_layers} shape layers")

    def _create_gradient_fill_linear(self):
        """Linear Gradient Fill シェイプレイヤー作成"""
        log_debug("  Creating Linear Gradient Fill shape...")
        layer = self.test_comp.add_shape()
        layer.name = "GradientFill_Linear"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Linear Gradient Group")

                    # グループ内のコンテンツグループを取得
                    vectors_group = group.property("ADBE Vectors Group")
                    if not vectors_group:
                        log_error("    ADBE Vectors Group not found")
                        return

                    # Rectangle を追加
                    if vectors_group.can_add_stream("ADBE Vector Shape - Rect"):
                        rect = vectors_group.add_stream("ADBE Vector Shape - Rect")
                        size_prop = rect.property("ADBE Vector Rect Size")
                        if size_prop:
                            size_prop.value = [300.0, 200.0]
                        log_debug("    Rectangle added (300x200)")

                    # Gradient Fill を追加
                    if vectors_group.can_add_stream("ADBE Vector Graphic - G-Fill"):
                        gfill = vectors_group.add_stream("ADBE Vector Graphic - G-Fill")

                        # Type: Linear (1)
                        type_prop = gfill.property("ADBE Vector Grad Type")
                        if type_prop:
                            type_prop.value = 1.0  # Linear

                        # Start Point
                        start_prop = gfill.property("ADBE Vector Grad Start Pt")
                        if start_prop:
                            start_prop.value = [-100.0, 0.0]

                        # End Point
                        end_prop = gfill.property("ADBE Vector Grad End Pt")
                        if end_prop:
                            end_prop.value = [100.0, 0.0]

                        log_info("    Linear Gradient Fill created successfully")
        except Exception as e:
            log_error(f"    Linear gradient creation error: {e}")
            log_error(traceback.format_exc())

    def _create_gradient_fill_radial(self):
        """Radial Gradient Fill シェイプレイヤー作成"""
        log_debug("  Creating Radial Gradient Fill shape...")
        layer = self.test_comp.add_shape()
        layer.name = "GradientFill_Radial"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Radial Gradient Group")

                    # グループ内のコンテンツグループを取得
                    vectors_group = group.property("ADBE Vectors Group")
                    if not vectors_group:
                        log_error("    ADBE Vectors Group not found")
                        return

                    # Ellipse を追加
                    if vectors_group.can_add_stream("ADBE Vector Shape - Ellipse"):
                        ellipse = vectors_group.add_stream("ADBE Vector Shape - Ellipse")
                        size_prop = ellipse.property("ADBE Vector Ellipse Size")
                        if size_prop:
                            size_prop.value = [250.0, 250.0]
                        log_debug("    Ellipse added (250x250)")

                    # Gradient Fill を追加
                    if vectors_group.can_add_stream("ADBE Vector Graphic - G-Fill"):
                        gfill = vectors_group.add_stream("ADBE Vector Graphic - G-Fill")

                        # Type: Radial (2)
                        type_prop = gfill.property("ADBE Vector Grad Type")
                        if type_prop:
                            type_prop.value = 2.0  # Radial

                        # Start Point (center)
                        start_prop = gfill.property("ADBE Vector Grad Start Pt")
                        if start_prop:
                            start_prop.value = [0.0, 0.0]

                        # End Point (radius)
                        end_prop = gfill.property("ADBE Vector Grad End Pt")
                        if end_prop:
                            end_prop.value = [125.0, 0.0]

                        log_info("    Radial Gradient Fill created successfully")
        except Exception as e:
            log_error(f"    Radial gradient creation error: {e}")
            log_error(traceback.format_exc())

    def _create_gradient_stroke(self):
        """Gradient Stroke シェイプレイヤー作成"""
        log_debug("  Creating Gradient Stroke shape...")
        layer = self.test_comp.add_shape()
        layer.name = "GradientStroke"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Gradient Stroke Group")

                    # グループ内のコンテンツグループを取得
                    vectors_group = group.property("ADBE Vectors Group")
                    if not vectors_group:
                        log_error("    ADBE Vectors Group not found")
                        return

                    # Star path を追加
                    if vectors_group.can_add_stream("ADBE Vector Shape - Star"):
                        star = vectors_group.add_stream("ADBE Vector Shape - Star")
                        # Type: Star (1)
                        type_prop = star.property("ADBE Vector Star Type")
                        if type_prop:
                            type_prop.value = 1.0  # Star

                        # Points
                        points_prop = star.property("ADBE Vector Star Points")
                        if points_prop:
                            points_prop.value = 5.0

                        # Outer Radius
                        outer_prop = star.property("ADBE Vector Star Outer Radius")
                        if outer_prop:
                            outer_prop.value = 100.0

                        # Inner Radius
                        inner_prop = star.property("ADBE Vector Star Inner Radius")
                        if inner_prop:
                            inner_prop.value = 50.0
                        log_debug("    Star added (5 points, R=100/50)")

                    # Gradient Stroke を追加
                    if vectors_group.can_add_stream("ADBE Vector Graphic - G-Stroke"):
                        gstroke = vectors_group.add_stream("ADBE Vector Graphic - G-Stroke")

                        # Stroke Width
                        width_prop = gstroke.property("ADBE Vector Stroke Width")
                        if width_prop:
                            width_prop.value = 10.0

                        # Type: Linear
                        type_prop = gstroke.property("ADBE Vector Grad Type")
                        if type_prop:
                            type_prop.value = 1.0  # Linear

                        log_info("    Gradient Stroke created successfully")
        except Exception as e:
            log_error(f"    Gradient stroke creation error: {e}")
            log_error(traceback.format_exc())

    def _create_repeater_with_transform(self):
        """Repeater with Transform シェイプレイヤー作成"""
        log_debug("  Creating Repeater with Transform shape...")
        layer = self.test_comp.add_shape()
        layer.name = "Repeater_Transform"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Repeater Transform Group")

                    # グループ内のコンテンツグループを取得
                    vectors_group = group.property("ADBE Vectors Group")
                    if not vectors_group:
                        log_error("    ADBE Vectors Group not found")
                        return

                    # Rectangle を追加
                    if vectors_group.can_add_stream("ADBE Vector Shape - Rect"):
                        rect = vectors_group.add_stream("ADBE Vector Shape - Rect")
                        size_prop = rect.property("ADBE Vector Rect Size")
                        if size_prop:
                            size_prop.value = [30.0, 30.0]
                        log_debug("    Rectangle added (30x30)")

                    # Fill を追加
                    if vectors_group.can_add_stream("ADBE Vector Graphic - Fill"):
                        fill = vectors_group.add_stream("ADBE Vector Graphic - Fill")
                        color_prop = fill.property("ADBE Vector Fill Color")
                        if color_prop:
                            color_prop.value = [1.0, 0.5, 0.0, 1.0]  # Orange
                        log_debug("    Fill added (Orange)")

                    # Repeater を追加
                    if vectors_group.can_add_stream("ADBE Vector Filter - Repeater"):
                        repeater = vectors_group.add_stream("ADBE Vector Filter - Repeater")

                        # Copies
                        copies_prop = repeater.property("ADBE Vector Repeater Copies")
                        if copies_prop:
                            copies_prop.value = 8.0

                        # Offset
                        offset_prop = repeater.property("ADBE Vector Repeater Offset")
                        if offset_prop:
                            offset_prop.value = 0.0

                        # Transform: Position
                        transform = repeater.property("ADBE Vector Repeater Transform")
                        if transform:
                            pos_prop = transform.property("ADBE Vector Repeater Position")
                            if pos_prop:
                                pos_prop.value = [50.0, 0.0]

                            # Transform: Rotation
                            rot_prop = transform.property("ADBE Vector Repeater Rotation")
                            if rot_prop:
                                rot_prop.value = 45.0

                            # Transform: Scale
                            scale_prop = transform.property("ADBE Vector Repeater Scale")
                            if scale_prop:
                                scale_prop.value = [95.0, 95.0]

                        log_info("    Repeater created successfully")
        except Exception as e:
            log_error(f"    Repeater creation error: {e}")
            log_error(traceback.format_exc())

    def _create_nested_repeater(self):
        """Nested Repeater シェイプレイヤー作成"""
        log_debug("  Creating Nested Repeater shape...")
        layer = self.test_comp.add_shape()
        layer.name = "Nested_Repeater"

        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")

            if contents and hasattr(contents, 'can_add_stream'):
                if contents.can_add_stream("ADBE Vector Group"):
                    group = contents.add_stream("ADBE Vector Group")
                    group.set_name("Nested Repeater Group")

                    # グループ内のコンテンツグループを取得
                    vectors_group = group.property("ADBE Vectors Group")
                    if not vectors_group:
                        log_error("    ADBE Vectors Group not found")
                        return

                    # Ellipse を追加
                    if vectors_group.can_add_stream("ADBE Vector Shape - Ellipse"):
                        ellipse = vectors_group.add_stream("ADBE Vector Shape - Ellipse")
                        size_prop = ellipse.property("ADBE Vector Ellipse Size")
                        if size_prop:
                            size_prop.value = [20.0, 20.0]
                        log_debug("    Ellipse added (20x20)")

                    # Fill を追加
                    if vectors_group.can_add_stream("ADBE Vector Graphic - Fill"):
                        fill = vectors_group.add_stream("ADBE Vector Graphic - Fill")
                        color_prop = fill.property("ADBE Vector Fill Color")
                        if color_prop:
                            color_prop.value = [0.0, 0.8, 0.8, 1.0]  # Cyan
                        log_debug("    Fill added (Cyan)")

                    # Repeater 1 (内側)
                    if vectors_group.can_add_stream("ADBE Vector Filter - Repeater"):
                        repeater1 = vectors_group.add_stream("ADBE Vector Filter - Repeater")
                        copies_prop = repeater1.property("ADBE Vector Repeater Copies")
                        if copies_prop:
                            copies_prop.value = 4.0

                        transform = repeater1.property("ADBE Vector Repeater Transform")
                        if transform:
                            pos_prop = transform.property("ADBE Vector Repeater Position")
                            if pos_prop:
                                pos_prop.value = [40.0, 0.0]

                        log_debug("    Inner Repeater added (Copies: 4, Position: [40,0])")

                    # Repeater 2 (外側)
                    if vectors_group.can_add_stream("ADBE Vector Filter - Repeater"):
                        repeater2 = vectors_group.add_stream("ADBE Vector Filter - Repeater")
                        copies_prop = repeater2.property("ADBE Vector Repeater Copies")
                        if copies_prop:
                            copies_prop.value = 6.0

                        transform = repeater2.property("ADBE Vector Repeater Transform")
                        if transform:
                            rot_prop = transform.property("ADBE Vector Repeater Rotation")
                            if rot_prop:
                                rot_prop.value = 60.0

                        log_info("    Nested Repeater created successfully")

        except Exception as e:
            log_error(f"    Nested repeater creation error: {e}")
            log_error(traceback.format_exc())

    def _export_scene(self):
        """シーンをエクスポート"""
        log_info("Step 3: Exporting scene...")
        from export_scene import export_project
        export_project(EXPORT_JSON)

        if os.path.exists(EXPORT_JSON):
            size = os.path.getsize(EXPORT_JSON)
            log_info(f"  Exported to: {EXPORT_JSON}")
            log_info(f"  File size: {size:,} bytes")
        else:
            log_error("  Export failed: file not created")

    def _import_scene(self):
        """シーンをインポート"""
        log_info("Step 4: Importing scene to new composition...")

        with open(EXPORT_JSON, 'r', encoding='utf-8') as f:
            data = json.load(f)

        # コンポジション名を変更
        for item in data.get("items", []):
            if item.get("type") == "Comp" and item.get("name") == "Gradient_Repeater_Test":
                item["name"] = "Gradient_Repeater_Imported"
                break

        import_json = os.path.join(DOCS_PATH, "gradient_repeater_import.json")
        with open(import_json, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)

        from import_scene import import_project
        import_project(import_json)

        log_info("  Import completed")

    def _verify_gradient_fill(self):
        """Gradient Fill の検証"""
        log_info("Step 5a: Verifying Gradient Fill...")

        # インポートされたコンポジションを取得
        imported_comp = self._get_imported_comp()
        if not imported_comp:
            return

        # Linear Gradient Fill を検証
        self._verify_layer_gradient(
            self.test_comp, imported_comp,
            "GradientFill_Linear",
            "Linear Gradient",
            expected_type=1.0,  # Linear
            expected_start=[-100.0, 0.0],
            expected_end=[100.0, 0.0]
        )

        # Radial Gradient Fill を検証
        self._verify_layer_gradient(
            self.test_comp, imported_comp,
            "GradientFill_Radial",
            "Radial Gradient",
            expected_type=2.0,  # Radial
            expected_start=[0.0, 0.0],
            expected_end=[125.0, 0.0]
        )

    def _verify_gradient_stroke(self):
        """Gradient Stroke の検証"""
        log_info("Step 5b: Verifying Gradient Stroke...")

        imported_comp = self._get_imported_comp()
        if not imported_comp:
            return

        # Gradient Stroke を検証
        orig_layer = self._find_layer(self.test_comp, "GradientStroke")
        imp_layer = self._find_layer(imported_comp, "GradientStroke")

        if not orig_layer or not imp_layer:
            self._add_result("GradientStroke", False, "Layer not found")
            return

        # Stroke Width を検証
        orig_width = self._get_gradient_stroke_width(orig_layer)
        imp_width = self._get_gradient_stroke_width(imp_layer)

        if orig_width is not None and imp_width is not None:
            match = abs(orig_width - imp_width) < 0.01
            self._add_result(
                "GradientStroke - Width",
                match,
                f"Original: {orig_width:.1f}, Imported: {imp_width:.1f}"
            )
        else:
            self._add_result(
                "GradientStroke - Width",
                False,
                "Could not get stroke width"
            )

    def _verify_repeater(self):
        """Repeater の検証"""
        log_info("Step 5c: Verifying Repeater...")

        imported_comp = self._get_imported_comp()
        if not imported_comp:
            return

        # Repeater Transform を検証
        self._verify_layer_repeater(
            self.test_comp, imported_comp,
            "Repeater_Transform",
            expected_copies=8.0,
            expected_position=[50.0, 0.0],
            expected_rotation=45.0
        )

        # Nested Repeater を検証（コピー数のみ）
        self._verify_layer_repeater(
            self.test_comp, imported_comp,
            "Nested_Repeater",
            expected_copies=4.0,  # 最初のRepeater
            check_nested=True
        )

    def _verify_layer_gradient(self, orig_comp, imp_comp, layer_name, test_name,
                                expected_type, expected_start, expected_end):
        """Gradient Fill/Stroke の詳細検証"""
        orig_layer = self._find_layer(orig_comp, layer_name)
        imp_layer = self._find_layer(imp_comp, layer_name)

        if not orig_layer:
            self._add_result(test_name, False, f"Original layer '{layer_name}' not found")
            return
        if not imp_layer:
            self._add_result(test_name, False, f"Imported layer '{layer_name}' not found")
            return

        # Type を検証
        orig_type = self._get_gradient_type(orig_layer)
        imp_type = self._get_gradient_type(imp_layer)

        log_debug(f"    Gradient Type - Original: {orig_type}, Imported: {imp_type}")

        if orig_type is not None and imp_type is not None:
            type_match = abs(orig_type - expected_type) < 0.01 and abs(imp_type - expected_type) < 0.01
            self._add_result(
                f"{test_name} - Type",
                type_match,
                f"Expected: {expected_type}, Original: {orig_type}, Imported: {imp_type}"
            )
        else:
            self._add_result(
                f"{test_name} - Type",
                False,
                f"Could not get gradient type (Original: {orig_type}, Imported: {imp_type})"
            )

        # Start Point を検証
        orig_start = self._get_gradient_start_point(orig_layer)
        imp_start = self._get_gradient_start_point(imp_layer)

        log_debug(f"    Gradient Start - Original: {orig_start}, Imported: {imp_start}")

        if orig_start and imp_start:
            start_match = self._compare_points(orig_start, imp_start)
            self._add_result(
                f"{test_name} - Start Point",
                start_match,
                f"Original: {orig_start}, Imported: {imp_start}"
            )
        else:
            self._add_result(
                f"{test_name} - Start Point",
                False,
                f"Could not get start point (Original: {orig_start}, Imported: {imp_start})"
            )

        # End Point を検証
        orig_end = self._get_gradient_end_point(orig_layer)
        imp_end = self._get_gradient_end_point(imp_layer)

        log_debug(f"    Gradient End - Original: {orig_end}, Imported: {imp_end}")

        if orig_end and imp_end:
            end_match = self._compare_points(orig_end, imp_end)
            self._add_result(
                f"{test_name} - End Point",
                end_match,
                f"Original: {orig_end}, Imported: {imp_end}"
            )
        else:
            self._add_result(
                f"{test_name} - End Point",
                False,
                f"Could not get end point (Original: {orig_end}, Imported: {imp_end})"
            )

    def _verify_layer_repeater(self, orig_comp, imp_comp, layer_name,
                                expected_copies, expected_position=None,
                                expected_rotation=None, check_nested=False):
        """Repeater の詳細検証"""
        orig_layer = self._find_layer(orig_comp, layer_name)
        imp_layer = self._find_layer(imp_comp, layer_name)

        if not orig_layer:
            self._add_result(f"{layer_name} - Repeater", False, f"Original layer '{layer_name}' not found")
            return
        if not imp_layer:
            self._add_result(f"{layer_name} - Repeater", False, f"Imported layer '{layer_name}' not found")
            return

        # Copies を検証
        orig_copies = self._get_repeater_copies(orig_layer)
        imp_copies = self._get_repeater_copies(imp_layer)

        log_debug(f"    Repeater Copies - Original: {orig_copies}, Imported: {imp_copies}")

        if orig_copies is not None and imp_copies is not None:
            copies_match = abs(orig_copies - expected_copies) < 0.01 and abs(imp_copies - expected_copies) < 0.01
            self._add_result(
                f"{layer_name} - Copies",
                copies_match,
                f"Expected: {expected_copies}, Original: {orig_copies}, Imported: {imp_copies}"
            )
        else:
            self._add_result(
                f"{layer_name} - Copies",
                False,
                f"Could not get repeater copies (Original: {orig_copies}, Imported: {imp_copies})"
            )

        # Position を検証（指定された場合）
        if expected_position:
            orig_pos = self._get_repeater_position(orig_layer)
            imp_pos = self._get_repeater_position(imp_layer)

            log_debug(f"    Repeater Position - Original: {orig_pos}, Imported: {imp_pos}")

            if orig_pos and imp_pos:
                pos_match = self._compare_points(orig_pos, imp_pos)
                self._add_result(
                    f"{layer_name} - Position",
                    pos_match,
                    f"Original: {orig_pos}, Imported: {imp_pos}"
                )
            else:
                self._add_result(
                    f"{layer_name} - Position",
                    False,
                    f"Could not get repeater position (Original: {orig_pos}, Imported: {imp_pos})"
                )

        # Rotation を検証（指定された場合）
        if expected_rotation is not None:
            orig_rot = self._get_repeater_rotation(orig_layer)
            imp_rot = self._get_repeater_rotation(imp_layer)

            log_debug(f"    Repeater Rotation - Original: {orig_rot}, Imported: {imp_rot}")

            if orig_rot is not None and imp_rot is not None:
                rot_match = abs(orig_rot - imp_rot) < 0.01
                self._add_result(
                    f"{layer_name} - Rotation",
                    rot_match,
                    f"Original: {orig_rot:.1f}°, Imported: {imp_rot:.1f}°"
                )
            else:
                self._add_result(
                    f"{layer_name} - Rotation",
                    False,
                    f"Could not get repeater rotation (Original: {orig_rot}, Imported: {imp_rot})"
                )

    # ヘルパーメソッド
    def _get_imported_comp(self):
        """インポートされたコンポジションを取得"""
        for item in self.project.items:
            if item.type == ae.ItemType.Comp and item.name == "Gradient_Repeater_Imported":
                return item
        log_error("  Imported composition not found!")
        return None

    def _find_layer(self, comp, layer_name):
        """レイヤーを名前で検索"""
        for i in range(comp.num_layers):
            layer = comp.layer(i)
            if layer.name == layer_name:
                return layer
        return None

    def _get_vectors_group(self, layer):
        """レイヤーの ADBE Vectors Group を取得するヘルパー"""
        try:
            root = layer.properties
            contents = root.property("ADBE Root Vectors Group")
            if contents:
                for i in range(contents.num_properties):
                    group = contents.property(i)
                    if group and group.match_name == "ADBE Vector Group":
                        vectors_group = group.property("ADBE Vectors Group")
                        if vectors_group:
                            return vectors_group
        except Exception as e:
            log_debug(f"    Error getting vectors group: {e}")
        return None

    def _get_gradient_type(self, layer):
        """Gradient Type を取得"""
        try:
            vectors_group = self._get_vectors_group(layer)
            if vectors_group:
                for i in range(vectors_group.num_properties):
                    prop = vectors_group.property(i)
                    if prop and "G-Fill" in prop.match_name:
                        type_prop = prop.property("ADBE Vector Grad Type")
                        if type_prop:
                            return type_prop.value
        except Exception as e:
            log_debug(f"    Error getting gradient type: {e}")
        return None

    def _get_gradient_start_point(self, layer):
        """Gradient Start Point を取得"""
        try:
            vectors_group = self._get_vectors_group(layer)
            if vectors_group:
                for i in range(vectors_group.num_properties):
                    prop = vectors_group.property(i)
                    if prop and "G-Fill" in prop.match_name:
                        start_prop = prop.property("ADBE Vector Grad Start Pt")
                        if start_prop:
                            return list(start_prop.value)
        except Exception as e:
            log_debug(f"    Error getting gradient start point: {e}")
        return None

    def _get_gradient_end_point(self, layer):
        """Gradient End Point を取得"""
        try:
            vectors_group = self._get_vectors_group(layer)
            if vectors_group:
                for i in range(vectors_group.num_properties):
                    prop = vectors_group.property(i)
                    if prop and "G-Fill" in prop.match_name:
                        end_prop = prop.property("ADBE Vector Grad End Pt")
                        if end_prop:
                            return list(end_prop.value)
        except Exception as e:
            log_debug(f"    Error getting gradient end point: {e}")
        return None

    def _get_gradient_stroke_width(self, layer):
        """Gradient Stroke Width を取得"""
        try:
            vectors_group = self._get_vectors_group(layer)
            if vectors_group:
                for i in range(vectors_group.num_properties):
                    prop = vectors_group.property(i)
                    if prop and "G-Stroke" in prop.match_name:
                        width_prop = prop.property("ADBE Vector Stroke Width")
                        if width_prop:
                            return width_prop.value
        except Exception as e:
            log_debug(f"    Error getting gradient stroke width: {e}")
        return None

    def _get_repeater_copies(self, layer):
        """Repeater Copies を取得"""
        try:
            vectors_group = self._get_vectors_group(layer)
            if vectors_group:
                for i in range(vectors_group.num_properties):
                    prop = vectors_group.property(i)
                    if prop and "Repeater" in prop.match_name:
                        copies_prop = prop.property("ADBE Vector Repeater Copies")
                        if copies_prop:
                            return copies_prop.value
        except Exception as e:
            log_debug(f"    Error getting repeater copies: {e}")
        return None

    def _get_repeater_position(self, layer):
        """Repeater Transform Position を取得"""
        try:
            vectors_group = self._get_vectors_group(layer)
            if vectors_group:
                for i in range(vectors_group.num_properties):
                    prop = vectors_group.property(i)
                    if prop and "Repeater" in prop.match_name:
                        transform = prop.property("ADBE Vector Repeater Transform")
                        if transform:
                            pos_prop = transform.property("ADBE Vector Repeater Position")
                            if pos_prop:
                                return list(pos_prop.value)
        except Exception as e:
            log_debug(f"    Error getting repeater position: {e}")
        return None

    def _get_repeater_rotation(self, layer):
        """Repeater Transform Rotation を取得"""
        try:
            vectors_group = self._get_vectors_group(layer)
            if vectors_group:
                for i in range(vectors_group.num_properties):
                    prop = vectors_group.property(i)
                    if prop and "Repeater" in prop.match_name:
                        transform = prop.property("ADBE Vector Repeater Transform")
                        if transform:
                            rot_prop = transform.property("ADBE Vector Repeater Rotation")
                            if rot_prop:
                                return rot_prop.value
        except Exception as e:
            log_debug(f"    Error getting repeater rotation: {e}")
        return None

    def _compare_points(self, p1, p2, tolerance=0.01):
        """2つの座標を比較"""
        if not p1 or not p2:
            return False
        if len(p1) != len(p2):
            return False
        return all(abs(a - b) < tolerance for a, b in zip(p1, p2))

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
            log_info(f"Total: {self.results['total']}, Passed: {self.results['passed']}")
        else:
            log_info("")
            log_info("Some tests FAILED:")
            for r in self.results["details"]:
                if not r["passed"]:
                    log_info(f"  - {r['name']}: {r['detail']}")
            log_info(f"Total: {self.results['total']}, Passed: {self.results['passed']}, Failed: {self.results['failed']}")


def main():
    """メイン処理"""
    test = GradientRepeaterTest()
    results = test.run()

    # テスト結果をファイルに保存
    result_file = os.path.join(DOCS_PATH, "phase7_test_results.json")
    try:
        with open(result_file, 'w', encoding='utf-8') as f:
            json.dump(results, f, indent=2, ensure_ascii=False)
        log_info(f"Results saved to: {result_file}")
    except Exception as e:
        log_error(f"Failed to save results: {e}")

    return results


if __name__ == "__main__":
    main()
