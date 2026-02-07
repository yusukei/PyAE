# import_scene.py
# PyAE - Python for After Effects
# JSONからAfter Effectsシーンを構築（空のシーン前提）

import ae
import json
import os
import struct

# ログレベル: 0=エラーのみ, 1=警告+エラー, 2=情報+警告+エラー, 3=デバッグ(全て)
LOG_LEVEL = 3  # デバッグモード: 全てのログを出力


def _safe_log(msg):
    """
    ログ出力（UTF-8で直接出力）
    C++側でUTF-8を正しく処理するため、変換不要
    """
    try:
        ae.log_info(msg)
    except Exception as e:
        # エラーの場合、エラーメッセージを含めてログ出力
        try:
            ae.log_info(f"[LOG ERROR] {str(e)}: {repr(msg)}")
        except:
            ae.log_info("[LOG ERROR] Failed to log message")


def log_error(msg):
    """エラーログ"""
    _safe_log(f"[ERROR] {msg}")


def log_warning(msg):
    """警告ログ"""
    if LOG_LEVEL >= 1:
        _safe_log(f"[WARNING] {msg}")


def log_info(msg):
    """情報ログ"""
    if LOG_LEVEL >= 2:
        _safe_log(f"[INFO] {msg}")


def log_debug(msg):
    """デバッグログ"""
    if LOG_LEVEL >= 3:
        _safe_log(f"[DEBUG] {msg}")


def debug_log(msg):
    """後方互換性のため残す（log_debugのエイリアス）"""
    log_debug(msg)


def bdata_to_value(bdata_hex):
    """
    bdata形式（IEEE 754 big-endian hex）からPython値へ変換
    """
    if not bdata_hex or bdata_hex == "":
        return None

    try:
        bdata_bytes = bytes.fromhex(bdata_hex)
        num_doubles = len(bdata_bytes) // 8

        if num_doubles == 0:
            return None
        elif num_doubles == 1:
            return struct.unpack('>d', bdata_bytes[:8])[0]
        else:
            values = []
            for i in range(num_doubles):
                offset = i * 8
                value = struct.unpack('>d', bdata_bytes[offset:offset+8])[0]
                values.append(value)
            return values
    except Exception as e:
        debug_log(f"bdata変換エラー: {bdata_hex[:20]}... -> {e}")
        return None


def bdata_to_path_vertices(bdata_hex):
    """
    Path/Mask の bdata 形式から頂点配列へ変換

    bdata 構造:
    [4 bytes] セグメント数 (big-endian int32)
    [8 bytes] vertex[0].x
    [8 bytes] vertex[0].y
    [8 bytes] vertex[0].tan_in_x
    [8 bytes] vertex[0].tan_in_y
    [8 bytes] vertex[0].tan_out_x
    [8 bytes] vertex[0].tan_out_y
    [repeat for each vertex...]
    """
    if not bdata_hex or bdata_hex == "":
        return None

    try:
        bdata_bytes = bytes.fromhex(bdata_hex)

        # 最初の4バイトからセグメント数を取得
        if len(bdata_bytes) < 4:
            return None

        num_segs = struct.unpack('>I', bdata_bytes[0:4])[0]

        # 各頂点は 6 doubles (x, y, tan_in_x, tan_in_y, tan_out_x, tan_out_y)
        expected_size = 4 + (num_segs * 6 * 8)
        if len(bdata_bytes) < expected_size:
            debug_log(f"Path bdata サイズ不足: expected {expected_size}, got {len(bdata_bytes)}")
            return None

        vertices = []
        for i in range(num_segs):
            offset = 4 + (i * 6 * 8)
            x = struct.unpack('>d', bdata_bytes[offset:offset+8])[0]
            y = struct.unpack('>d', bdata_bytes[offset+8:offset+16])[0]
            tan_in_x = struct.unpack('>d', bdata_bytes[offset+16:offset+24])[0]
            tan_in_y = struct.unpack('>d', bdata_bytes[offset+24:offset+32])[0]
            tan_out_x = struct.unpack('>d', bdata_bytes[offset+32:offset+40])[0]
            tan_out_y = struct.unpack('>d', bdata_bytes[offset+40:offset+48])[0]

            vertices.append({
                "x": x,
                "y": y,
                "tan_in_x": tan_in_x,
                "tan_in_y": tan_in_y,
                "tan_out_x": tan_out_x,
                "tan_out_y": tan_out_y,
            })

        return vertices

    except Exception as e:
        debug_log(f"Path bdata変換エラー: {e}")
        return None


class SceneImporter:
    """シーンインポーター（空のシーン前提）"""

    def __init__(self, json_path):
        self.json_path = json_path
        self.data = None
        self.project = None
        self.id_to_item = {}  # JSONのID -> AEアイテム

    def run(self):
        """インポート実行"""
        if not os.path.exists(self.json_path):
            log_error(f"ファイルが見つかりません: {self.json_path}")
            return False

        log_info(f"JSONファイル読み込み中: {self.json_path}")

        with open(self.json_path, "r", encoding="utf-8") as f:
            self.data = json.load(f)

        self.project = ae.get_project()
        self.id_to_item[0] = None  # ルートフォルダはNone

        ae.begin_undo_group("JSONからシーンをインポート")

        try:
            items = self.data.get("items", [])
            log_info(f"総アイテム数: {len(items)}")

            # Phase 1: フォルダ作成
            self._create_folders(items)

            # Phase 2: コンポジション作成
            self._create_comps(items)

            # Phase 3: フッテージ作成
            self._create_footages(items)

            # Phase 4: レイヤー復元
            self._restore_layers(items)

            log_info("=" * 60)
            log_info("インポート完了")
            log_info("=" * 60)
            return True

        except Exception as e:
            import traceback
            log_error(f"インポート失敗: {e}\n{traceback.format_exc()}")
            return False

        finally:
            ae.end_undo_group()
            ae.refresh()

    def _get_parent_folder(self, parent_id):
        """親フォルダを取得 (None=ルート, Folder=フォルダオブジェクト)"""
        return self.id_to_item.get(parent_id)

    def _create_folders(self, items):
        """フォルダを階層順に作成"""
        folder_items = [i for i in items if i.get("type") == "Folder"]
        log_info(f"[Phase 1] フォルダ作成 ({len(folder_items)}個)")

        remaining = folder_items[:]
        max_iterations = len(folder_items) + 1
        iteration = 0

        while remaining and iteration < max_iterations:
            created = []
            for item in remaining:
                item_id = item.get("id", -1)
                item_name = item.get("name", "")
                parent_id = item.get("parent_folder_id", 0)

                # ルートフォルダはスキップ (id=0 または親がルート自身)
                if item_id == 0 or item_name == "ルート":
                    self.id_to_item[item_id] = None  # ルートはNone
                    created.append(item)
                    continue

                # 親フォルダが作成済みかチェック
                if parent_id not in self.id_to_item:
                    continue

                parent_folder = self.id_to_item[parent_id]  # None (ルート) または Folder オブジェクト

                try:
                    # フォルダ作成
                    folder = self.project.create_folder(item_name, parent_folder)

                    # デバッグ: フォルダの型を確認
                    debug_log(f"    作成: {item_name} (id:{item_id}, 親id:{parent_id}) -> type={type(folder)}")

                    # id_to_item に格納
                    self.id_to_item[item_id] = folder

                    # デバッグ: 格納されたハンドルを確認
                    folder_handle = folder._get_handle_ptr() if hasattr(folder, '_get_handle_ptr') else 'N/A'
                    debug_log(f"    id_to_item[{item_id}] = {hex(folder_handle) if isinstance(folder_handle, int) else folder_handle}")

                    parent_name = parent_folder.name if parent_folder else "ルート"
                    log_info(f"  + フォルダ: {item_name} (親: {parent_name})")

                except Exception as e:
                    import traceback
                    log_warning(f"  ! フォルダ作成失敗: {item_name}: {e}")
                    debug_log(traceback.format_exc())

                created.append(item)

            for c in created:
                remaining.remove(c)
            iteration += 1

    def _create_comps(self, items):
        """コンポジションを作成"""
        comp_items = [i for i in items if i.get("type") == "Comp"]
        log_info(f"[Phase 2] コンポジション作成 ({len(comp_items)}個)")

        for item in comp_items:
            item_id = item.get("id", -1)
            item_name = item.get("name", "")
            parent_id = item.get("parent_folder_id", 0)
            comp_data = item.get("comp_data", {})

            if not comp_data:
                log_warning(f"  ! comp_dataなし: {item_name}")
                continue

            parent_folder = self._get_parent_folder(parent_id)

            # デバッグ: 親フォルダのハンドルを確認
            if parent_folder:
                parent_handle = parent_folder._get_handle_ptr() if hasattr(parent_folder, '_get_handle_ptr') else 'N/A'
                debug_log(f"    Comp作成: {item_name} (id:{item_id}, 親id:{parent_id}) -> parent_handle={hex(parent_handle) if isinstance(parent_handle, int) else parent_handle}")
            else:
                debug_log(f"    Comp作成: {item_name} (id:{item_id}, 親id:{parent_id}) -> parent=ルート")

            try:
                comp = self.project.create_comp(
                    item_name,
                    int(comp_data.get("width", 1920)),
                    int(comp_data.get("height", 1080)),
                    float(comp_data.get("pixel_aspect", 1.0)),
                    float(comp_data.get("duration", 10.0)),
                    float(comp_data.get("frame_rate", 30.0)),
                    parent_folder  # 親フォルダを作成時に指定
                )
                log_info(f"  + コンポ: {item_name} ({comp_data.get('width')}x{comp_data.get('height')})")

                # Phase 1.2: Apply composition settings
                self._apply_comp_settings(comp, comp_data)

            except Exception as e:
                log_warning(f"  ! コンポ作成失敗: {item_name}: {e}")
                continue

            self.id_to_item[item_id] = comp

    def _apply_comp_settings(self, comp, comp_data):
        """コンポジション設定を適用（Phase 1.2）"""
        try:
            # Background color
            if "background_color" in comp_data:
                try:
                    bg = comp_data["background_color"]
                    if len(bg) >= 3:
                        comp.bg_color = (bg[0], bg[1], bg[2])
                        debug_log(f"      Background color set: {bg}")
                except Exception as e:
                    debug_log(f"      Failed to set bg_color: {e}")

            # Work area
            if "work_area_start" in comp_data and "work_area_duration" in comp_data:
                try:
                    comp.work_area_start = comp_data["work_area_start"]
                    comp.work_area_duration = comp_data["work_area_duration"]
                    debug_log(f"      Work area set: {comp_data['work_area_start']} - {comp_data['work_area_duration']}")
                except Exception as e:
                    debug_log(f"      Failed to set work_area: {e}")

        except Exception as e:
            debug_log(f"      Comp settings apply error: {e}")

    def _create_footages(self, items):
        """フッテージを作成"""
        footage_items = [i for i in items if i.get("type") == "Footage"]
        log_info(f"[Phase 3] フッテージ処理 ({len(footage_items)}個)")

        for item in footage_items:
            item_id = item.get("id", -1)
            item_name = item.get("name", "")
            parent_id = item.get("parent_folder_id", 0)
            footage_data = item.get("footage_data", {})

            parent_folder = self._get_parent_folder(parent_id)
            footage_type = footage_data.get("footage_type", "")

            # ファイルベースのフッテージをインポート
            if footage_type == "File":
                file_path = footage_data.get("file_path", "")
                if file_path and os.path.exists(file_path):
                    try:
                        seq_opts = footage_data.get("sequence_options")
                        footage = self.project.import_file(file_path, parent_folder, seq_opts)
                        if footage:
                            log_info(f"  + ファイル: {item_name}")
                            self.id_to_item[item_id] = footage
                    except Exception as e:
                        log_warning(f"  ! インポート失敗: {item_name}: {e}")
                else:
                    debug_log(f"  ファイルなし: {file_path}")

            # Solidはレイヤー作成時に処理
            elif footage_type == "Solid":
                debug_log(f"  Solidはレイヤー作成時に処理: {item_name}")

    def _restore_layers(self, items):
        """レイヤーを復元"""
        comp_items = [i for i in items if i.get("type") == "Comp"]
        log_info(f"[Phase 4] レイヤー復元")

        for item in comp_items:
            item_id = item.get("id", -1)
            comp = self.id_to_item.get(item_id)
            if not comp:
                continue

            comp_data = item.get("comp_data", {})
            layers_data = comp_data.get("layers", [])

            if not layers_data:
                continue

            log_info(f"  コンポ: {comp.name} ({len(layers_data)}レイヤー)")

            # インデックス降順で作成（AEでは後から追加したレイヤーが上になる）
            sorted_layers = sorted(layers_data, key=lambda x: x.get("index", 999), reverse=True)

            for layer_data in sorted_layers:
                try:
                    self._restore_layer(comp, layer_data)
                except Exception as e:
                    import traceback
                    log_error(f"    ! レイヤー復元エラー: {layer_data.get('name')}: {e}")
                    debug_log(traceback.format_exc())

            # Phase 1.1 & 1.4: Restore layer hierarchy and track mattes after all layers are created
            try:
                self._restore_layer_hierarchy(comp, layers_data)
                self._restore_track_mattes(comp, layers_data)
            except Exception as e:
                import traceback
                log_error(f"    ! 親子関係/トラックマット復元エラー: {e}")
                debug_log(traceback.format_exc())

    def _restore_layer(self, comp, layer_data):
        """単一レイヤーを復元"""
        layer_name = layer_data.get("name", "Layer")
        layer_type = layer_data.get("type", "").lower()

        duration = layer_data.get("out_point", comp.duration) - layer_data.get("in_point", 0.0)

        layer = self._create_layer(comp, layer_data, duration)

        if not layer:
            log_warning(f"    ! レイヤー作成失敗: {layer_name}")
            return

        log_info(f"    + レイヤー: {layer_name} ({layer_type})")

        # Solidレイヤー、またはSolidソースを持つAVレイヤーの場合、ソースfootageの親フォルダを修正
        if layer_type == "solid" or (layer_type == "av" and self._is_solid_source(layer_data)):
            self._fix_solid_parent_folder(layer, layer_data)

        # 基本設定を適用
        self._apply_layer_settings(layer, layer_data)

        # テキストレイヤーの場合、ソーステキストを設定
        if layer_type == "text":
            source_text = layer_data.get("source_text")
            if source_text:
                expression = layer_data.get("source_text_expression")
                expression_enabled = layer_data.get("source_text_expression_enabled", True)
                self._set_source_text(layer, source_text, expression, expression_enabled)

            # Phase 9: threeDPerChar を先に適用（Scale Z軸の復元に必要）
            threeDPerChar = layer_data.get("threeDPerChar", False)
            if threeDPerChar:
                try:
                    comp_name = comp.name
                    layer_index = layer.index + 1  # AE ExtendScript は 1-based インデックス
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
                    debug_log(f"    threeDPerChar = true を適用: {result}")
                except Exception as e:
                    debug_log(f"    threeDPerChar 適用失敗: {e}")

            # Phase 9: Text Animators を復元
            text_animators = layer_data.get("text_animators", [])
            if text_animators:
                self._restore_text_animators(layer, text_animators)

        # エフェクト復元
        effects_data = layer_data.get("effects", [])
        if effects_data:
            self._restore_effects(layer, effects_data)

        # マスク復元 (Phase 2)
        masks_data = layer_data.get("masks", [])
        if masks_data:
            self._restore_masks(layer, masks_data)

        # Camera/Light options復元 (Phase 3.3)
        if layer_type == "camera":
            camera_options = layer_data.get("camera_options", {})
            if camera_options:
                self._restore_camera_options(layer, camera_options)

        if layer_type == "light":
            light_options = layer_data.get("light_options", {})
            if light_options:
                self._restore_light_options(layer, light_options)

        # プロパティ復元
        properties_data = layer_data.get("properties", {})
        if properties_data:
            self._restore_properties(layer, properties_data)

    def _create_layer(self, comp, layer_data, duration):
        """レイヤーを新規作成"""
        layer_name = layer_data.get("name", "Layer")
        layer_type = layer_data.get("type", "").lower()
        is_null = layer_data.get("is_null", False)

        try:
            # Nullレイヤー
            if is_null or layer_type == "null":
                return comp.add_null(layer_name, duration)

            # Solidレイヤー
            if layer_type == "solid":
                width = layer_data.get("width", comp.width)
                height = layer_data.get("height", comp.height)
                color = layer_data.get("solid_color", [0.0, 0.0, 0.0])
                if len(color) < 3:
                    color = [0.0, 0.0, 0.0]
                return comp.add_solid(layer_name, width, height, color, duration)

            # Textレイヤー
            if layer_type == "text":
                layer = comp.add_text("")
                layer.name = layer_name
                return layer

            # Cameraレイヤー
            if layer_type == "camera":
                return comp.add_camera(layer_name, (comp.width / 2, comp.height / 2))

            # Lightレイヤー
            if layer_type == "light":
                return comp.add_light(layer_name, (comp.width / 2, comp.height / 2))

            # Shapeレイヤー
            if layer_type == "shape":
                layer = comp.add_shape()
                layer.name = layer_name
                return layer

            # AVレイヤー（フッテージ/コンポ参照）
            if layer_type == "av":
                source_item_id = layer_data.get("source_item_id")
                source_item = self.id_to_item.get(source_item_id) if source_item_id else None

                if source_item:
                    return comp.add_layer(source_item, duration)
                else:
                    # ソースが見つからない場合、JSONからフッテージ情報を取得
                    footage_info = self._get_footage_info(source_item_id)
                    if footage_info and footage_info.get("footage_data", {}).get("footage_type") == "Solid":
                        # Solidの場合、色情報も取得（将来的な拡張のため）
                        width = footage_info.get("footage_data", {}).get("width", comp.width)
                        height = footage_info.get("footage_data", {}).get("height", comp.height)
                        # 注: このSolidレイヤーも_restore_layer()で親フォルダが修正される
                        return comp.add_solid(layer_name, width, height, (0.0, 0.0, 0.0), duration)

                    log_warning(f"      ソースなし: {layer_name} (id: {source_item_id})")
                    return None

            log_warning(f"      未対応タイプ: {layer_type}")
            return None

        except Exception as e:
            log_warning(f"      レイヤー作成エラー: {layer_name}: {e}")
            return None

    def _get_footage_info(self, item_id):
        """JSONデータからフッテージ情報を取得"""
        if not self.data or "items" not in self.data:
            return None
        for item in self.data["items"]:
            if item.get("id") == item_id and item.get("type") == "Footage":
                return item
        return None

    def _is_solid_source(self, layer_data):
        """レイヤーのソースがSolidかどうかを判定"""
        source_item_id = layer_data.get("source_item_id")
        if not source_item_id:
            return False
        footage_info = self._get_footage_info(source_item_id)
        if footage_info and footage_info.get("footage_data", {}).get("footage_type") == "Solid":
            return True
        return False

    def _fix_solid_parent_folder(self, layer, layer_data):
        """Solidレイヤーのソースfootageの親フォルダを正しく設定"""
        try:
            # レイヤーからソースアイテムIDを取得
            source_item_id = layer_data.get("source_item_id")
            if not source_item_id:
                debug_log(f"      source_item_id not found in layer_data")
                return

            # JSONからソースfootageの情報を取得
            footage_info = self._get_footage_info(source_item_id)
            if not footage_info:
                debug_log(f"      footage_info not found for id:{source_item_id}")
                return

            # 親フォルダIDを取得
            parent_folder_id = footage_info.get("parent_folder_id", 0)
            parent_folder = self._get_parent_folder(parent_folder_id)

            # レイヤーの実際のソースアイテムIDを取得
            actual_source_id = layer.source_item_id
            if actual_source_id == 0:
                debug_log(f"      layer has no source_item_id")
                return

            # プロジェクト内のすべてのアイテムから該当するものを検索
            for item in self.project.items:
                if hasattr(item, 'id') and item.id == actual_source_id:
                    # 現在の親フォルダと目的の親フォルダが同じ場合はスキップ
                    current_parent = item.parent_folder
                    if current_parent == parent_folder:
                        debug_log(f"      Solid footage '{footage_info.get('name')}' already in correct folder")
                        return

                    # 親フォルダを設定
                    item.parent_folder = parent_folder
                    parent_name = parent_folder.name if parent_folder else "ルート"

                    # デバッグログに親フォルダの詳細情報を出力
                    if parent_folder:
                        # 親フォルダのパス情報を構築
                        path_parts = [parent_name]
                        temp_parent = parent_folder.parent_folder
                        while temp_parent:
                            path_parts.insert(0, temp_parent.name)
                            temp_parent = temp_parent.parent_folder
                        folder_path = "/".join(path_parts)
                        debug_log(f"      Solid footage '{footage_info.get('name')}' moved to '{folder_path}' (id:{parent_folder_id})")
                    else:
                        debug_log(f"      Solid footage '{footage_info.get('name')}' moved to 'ルート' (id:{parent_folder_id})")
                    return

            debug_log(f"      Could not find item with id:{actual_source_id} in project.items")

        except Exception as e:
            debug_log(f"      _fix_solid_parent_folder error: {e}")

    def _apply_layer_settings(self, layer, layer_data):
        """レイヤー基本設定を適用"""
        try:
            if "active" in layer_data:
                layer.enabled = layer_data["active"]

            if layer_data.get("is_adjustment", False):
                layer.is_adjustment = True

            if layer_data.get("is_3d", False):
                layer.is_3d = True

            # Time Remapping: フラグを先に有効化（プロパティ復元より前に設定が必要）
            if layer_data.get("time_remapping_enabled", False):
                try:
                    layer.time_remapping_enabled = True
                    debug_log(f"      time_remapping_enabled = True")
                except Exception as e:
                    debug_log(f"      time_remapping_enabled設定失敗: {e}")

            if "in_point" in layer_data:
                layer.in_point = layer_data["in_point"]

            if "out_point" in layer_data:
                layer.out_point = layer_data["out_point"]

            if "start_time" in layer_data:
                layer.start_time = layer_data["start_time"]

            if "label" in layer_data:
                try:
                    layer.label = layer_data["label"]
                except Exception as e:
                    debug_log(f"      label設定失敗: {e}")

            # Phase 1.3: Layer flags restoration
            if "solo" in layer_data:
                try:
                    layer.solo = layer_data["solo"]
                except Exception as e:
                    debug_log(f"      solo設定失敗: {e}")

            if "locked" in layer_data:
                try:
                    layer.locked = layer_data["locked"]
                except Exception as e:
                    debug_log(f"      locked設定失敗: {e}")

            if "shy" in layer_data:
                try:
                    layer.shy = layer_data["shy"]
                except Exception as e:
                    debug_log(f"      shy設定失敗: {e}")

            if "quality" in layer_data:
                try:
                    layer.quality = layer_data["quality"]
                except Exception as e:
                    debug_log(f"      quality設定失敗: {e}")

            if "blend_mode" in layer_data:
                try:
                    layer.blend_mode = layer_data["blend_mode"]
                except Exception as e:
                    debug_log(f"      blend_mode設定失敗: {e}")

        except Exception as e:
            debug_log(f"      設定適用エラー: {e}")

    def _set_source_text(self, layer, text, expression=None, expression_enabled=True):
        """テキストレイヤーのソーステキストを設定（エクスプレッション対応）"""
        try:
            root = layer.properties
            if not root or not root.is_group:
                debug_log(f"      レイヤーのプロパティルートが取得できません")
                return

            # ADBE Text Properties > ADBE Text Document を取得
            text_props = root.property("ADBE Text Properties")
            if not text_props or not text_props.is_group:
                debug_log(f"      ADBE Text Properties が見つかりません")
                return

            source_text_prop = text_props.property("ADBE Text Document")
            if not source_text_prop:
                debug_log(f"      ADBE Text Document が見つかりません")
                return

            # set_source_textメソッドを使ってテキストを設定
            source_text_prop.set_source_text(text)
            debug_log(f"      ソーステキスト設定: {text[:30]}..." if len(str(text)) > 30 else f"      ソーステキスト設定: {text}")

            # エクスプレッションを設定
            if expression:
                try:
                    source_text_prop.expression = expression
                    debug_log(f"      ソーステキストエクスプレッション設定: {expression[:50]}...")
                    # エクスプレッションの有効/無効を設定
                    if hasattr(source_text_prop, 'expression_enabled'):
                        source_text_prop.expression_enabled = expression_enabled
                        debug_log(f"      エクスプレッション有効: {expression_enabled}")
                except Exception as e:
                    debug_log(f"      ソーステキストエクスプレッション設定エラー: {e}")

        except Exception as e:
            debug_log(f"      ソーステキスト設定エラー: {e}")

    def _restore_layer_hierarchy(self, comp, layers_data):
        """レイヤーの親子関係を復元（Phase 1.1）"""
        try:
            # Get all layers in composition (0-based indexing)
            layers = []
            for i in range(comp.num_layers):
                layers.append(comp.layer(i))

            # Sort layers_data by index to match layer order
            sorted_layers_data = sorted(layers_data, key=lambda x: x.get("index", 999))

            for i, layer_data in enumerate(sorted_layers_data):
                parent_index = layer_data.get("parent_index")
                # parent_index は 0-based (layer.index が 0-based になったため)
                if parent_index is not None and parent_index >= 0 and parent_index < len(layers):
                    try:
                        layers[i].parent = layers[parent_index]
                        debug_log(f"      Set parent for layer {i} ({layer_data.get('name')}) to layer {parent_index}")
                    except Exception as e:
                        debug_log(f"      Failed to set parent for layer {i}: {e}")

        except Exception as e:
            debug_log(f"      Layer hierarchy restoration error: {e}")

    def _restore_track_mattes(self, comp, layers_data):
        """トラックマット関係を復元（Phase 1.4）"""
        try:
            # Get all layers in composition (0-based indexing)
            layers = []
            for i in range(comp.num_layers):
                layers.append(comp.layer(i))

            # Sort layers_data by index to match layer order
            sorted_layers_data = sorted(layers_data, key=lambda x: x.get("index", 999))

            for i, layer_data in enumerate(sorted_layers_data):
                if layer_data.get("has_track_matte") and "track_matte_type" in layer_data:
                    try:
                        track_matte_type = layer_data["track_matte_type"]
                        layers[i].track_matte_type = track_matte_type
                        debug_log(f"      Set track matte for layer {i} ({layer_data.get('name')}) to type {track_matte_type}")
                    except Exception as e:
                        debug_log(f"      Failed to set track matte for layer {i}: {e}")

        except Exception as e:
            debug_log(f"      Track matte restoration error: {e}")

    def _restore_effects(self, layer, effects_data):
        """エフェクトを復元"""
        if effects_data:
            log_debug(f"      エフェクト復元: {len(effects_data)}個")

        for effect_data in effects_data:
            # 新形式: stream_name + match_name
            # 旧形式: name + match_name（後方互換性のため）
            stream_name = effect_data.get("stream_name")  # カスタム名（Noneの場合あり）
            match_name = effect_data.get("match_name", "")
            old_name = effect_data.get("name")  # 旧形式用（非推奨）
            enabled = effect_data.get("enabled", True)

            try:
                # match_nameでエフェクトを追加
                effect = None
                if match_name:
                    effect = layer.add_effect(match_name)
                elif old_name:
                    # 旧形式のフォールバック
                    effect = layer.add_effect(old_name)

                if effect:
                    # カスタム名が設定されている場合のみset_name()を呼び出す
                    if stream_name:
                        try:
                            effect.set_name(stream_name)
                            # 設定後の名前を確認
                            actual_stream_name = effect.stream_name if hasattr(effect, 'stream_name') else None
                            if actual_stream_name == stream_name:
                                log_info(f"        [OK] エフェクト: {stream_name}")
                            else:
                                log_warning(f"        エフェクト名設定警告: 期待={stream_name}, 実際={actual_stream_name}")
                        except Exception as e:
                            log_error(f"        エフェクト名設定失敗: {stream_name}: {e}")
                    else:
                        # カスタム名なし（デフォルト名のまま）
                        log_info(f"        エフェクト追加: {match_name}")

                    # エフェクトの有効/無効を設定
                    if not enabled:
                        try:
                            effect.enabled = enabled
                        except Exception as e:
                            debug_log(f"        enabled設定失敗: {e}")

                    params_data = effect_data.get("params", [])
                    self._restore_effect_params(effect, params_data)

            except Exception as e:
                debug_log(f"        ! エフェクト追加失敗: {match_name or old_name}: {e}")

    def _restore_effect_params(self, effect, params_data):
        """エフェクトパラメータを復元"""
        for param_data in params_data:
            param_name = param_data.get("name", "")
            param_index = param_data.get("index", -1)
            bdata = param_data.get("bdata", "")

            if not bdata:
                continue

            value = bdata_to_value(bdata)
            if value is None:
                continue

            try:
                param = None
                if param_index >= 0:
                    param = effect.property_by_index(param_index)
                if not param and param_name:
                    param = effect.property(param_name)

                if param and param.valid:
                    param.value = value
                    debug_log(f"          パラメータ: {param_name} = {value}")
            except Exception as e:
                debug_log(f"          パラメータ設定失敗: {param_name}: {e}")

    def _restore_masks(self, layer, masks_data):
        """マスクを復元 (Phase 2)"""
        debug_log(f"      マスク復元: {len(masks_data)}個")

        try:
            root = layer.properties
            if not root or not root.is_group:
                return

            masks_group = root.property("ADBE Mask Parade")
            if not masks_group or not masks_group.is_group:
                debug_log(f"        ! Mask Paradeグループが見つかりません")
                return

            # 既存のマスクに対してプロパティを復元
            # 注意: 新規マスク作成は Phase 6 で実装予定
            num_existing_masks = masks_group.num_properties
            debug_log(f"        既存マスク数: {num_existing_masks}")

            for i, mask_data in enumerate(masks_data):
                mask_name = mask_data.get("name", "")
                properties_data = mask_data.get("properties", {})

                # インデックスでマスクを取得（1-based）
                if i + 1 <= num_existing_masks:
                    try:
                        mask = masks_group.property(i + 1)
                        debug_log(f"          + マスク復元: {mask_name}")

                        # プロパティを復元
                        for prop_name, prop_data in properties_data.items():
                            try:
                                self._restore_property_recursive(mask, prop_name, prop_data)
                            except Exception as e:
                                debug_log(f"            マスクプロパティスキップ: {prop_name}: {e}")

                    except Exception as e:
                        debug_log(f"          ! マスク[{i+1}]復元失敗: {e}")
                else:
                    debug_log(f"          ! マスク[{i+1}]はエクスポート時に存在したが、現在は存在しません（Phase 6で新規作成実装予定）")

        except Exception as e:
            debug_log(f"        ! マスク復元エラー: {e}")

    def _restore_camera_options(self, layer, camera_options):
        """カメラオプションを復元 (Phase 8: 改善版)"""
        debug_log(f"      カメラオプション復元: {len(camera_options)}個")

        try:
            root = layer.properties
            if not root or not root.is_group:
                return

            camera_opts_group = root.property("ADBE Camera Options Group")
            if not camera_opts_group or not camera_opts_group.is_group:
                debug_log(f"        ! Camera Options Groupが見つかりません")
                return

            # 各プロパティを復元
            for prop_name, prop_data in camera_options.items():
                try:
                    # Phase 8: 新形式（dict）と旧形式（直接値）の両方に対応
                    if isinstance(prop_data, dict):
                        match_name = prop_data.get("match_name")
                        prop_value = prop_data.get("value")
                    else:
                        # 旧形式: 直接値が入っている
                        match_name = None
                        prop_value = prop_data

                    if prop_value is None:
                        continue

                    # match_name で検索（優先）、なければ名前で検索
                    prop = None
                    if match_name:
                        prop = camera_opts_group.property(match_name)

                    if not prop or not prop.valid:
                        # 名前で検索
                        for i in range(camera_opts_group.num_properties):
                            p = camera_opts_group.property(i)
                            if p and p.name == prop_name:
                                prop = p
                                break

                    if prop and prop.valid and not prop.is_group:
                        prop.value = prop_value
                        debug_log(f"          + {prop_name} = {prop_value}")
                    else:
                        debug_log(f"          ! {prop_name} が見つかりません")

                except Exception as e:
                    debug_log(f"          ! {prop_name} 設定失敗: {e}")

        except Exception as e:
            debug_log(f"        ! カメラオプション復元エラー: {e}")

    def _restore_light_options(self, layer, light_options):
        """ライトオプションを復元 (Phase 8: 改善版)"""
        debug_log(f"      ライトオプション復元: {len(light_options)}個")

        try:
            root = layer.properties
            if not root or not root.is_group:
                return

            light_opts_group = root.property("ADBE Light Options Group")
            if not light_opts_group or not light_opts_group.is_group:
                debug_log(f"        ! Light Options Groupが見つかりません")
                return

            # 各プロパティを復元
            for prop_name, prop_data in light_options.items():
                try:
                    # Phase 8: 新形式（dict）と旧形式（直接値）の両方に対応
                    if isinstance(prop_data, dict):
                        match_name = prop_data.get("match_name")
                        prop_value = prop_data.get("value")
                    else:
                        # 旧形式: 直接値が入っている
                        match_name = None
                        prop_value = prop_data

                    if prop_value is None:
                        continue

                    # match_name で検索（優先）、なければ名前で検索
                    prop = None
                    if match_name:
                        prop = light_opts_group.property(match_name)

                    if not prop or not prop.valid:
                        # 名前で検索
                        for i in range(light_opts_group.num_properties):
                            p = light_opts_group.property(i)
                            if p and p.name == prop_name:
                                prop = p
                                break

                    if prop and prop.valid and not prop.is_group:
                        prop.value = prop_value
                        debug_log(f"          + {prop_name} = {prop_value}")
                    else:
                        debug_log(f"          ! {prop_name} が見つかりません")

                except Exception as e:
                    debug_log(f"          ! {prop_name} 設定失敗: {e}")

        except Exception as e:
            debug_log(f"        ! ライトオプション復元エラー: {e}")

    def _restore_text_animators(self, layer, text_animators_data):
        """Text Animatorsを復元 (Phase 9)"""
        debug_log(f"      Text Animators復元: {len(text_animators_data)}個")

        try:
            root = layer.properties
            if not root or not root.is_group:
                return

            # ADBE Text Properties を取得
            text_props = root.property("ADBE Text Properties")
            if not text_props or not text_props.is_group:
                debug_log(f"        ! Text Propertiesが見つかりません")
                return

            # ADBE Text Animators グループを取得
            animators_group = text_props.property("ADBE Text Animators")
            if not animators_group or not animators_group.is_group:
                debug_log(f"        ! Text Animatorsグループが見つかりません")
                return

            # 各 Animator を復元
            for animator_data in text_animators_data:
                try:
                    animator_name = animator_data.get("name", "Animator")
                    self._create_text_animator(animators_group, animator_data, animator_name)
                except Exception as e:
                    debug_log(f"        ! Animator復元失敗: {animator_name}: {e}")

        except Exception as e:
            debug_log(f"        ! Text Animators復元エラー: {e}")

    def _create_text_animator(self, animators_group, animator_data, animator_name):
        """単一のText Animatorを作成"""
        # add_stream で Animator を追加
        if not hasattr(animators_group, 'can_add_stream'):
            debug_log(f"          ! can_add_streamメソッドがありません")
            return

        if not animators_group.can_add_stream("ADBE Text Animator"):
            debug_log(f"          ! Animatorを追加できません")
            return

        animator = animators_group.add_stream("ADBE Text Animator")
        if not animator:
            debug_log(f"          ! Animator追加失敗")
            return

        # 名前を設定
        try:
            if hasattr(animator, 'set_name'):
                animator.set_name(animator_name)
                debug_log(f"          + Animator作成: {animator_name}")
        except Exception as e:
            debug_log(f"          Animator名設定失敗: {e}")

        # Selectors を復元
        selectors_data = animator_data.get("selectors", [])
        if selectors_data:
            self._restore_animator_selectors(animator, selectors_data)

        # Properties を復元
        properties_data = animator_data.get("properties", {})
        if properties_data:
            self._restore_animator_properties(animator, properties_data)

    def _restore_animator_selectors(self, animator, selectors_data):
        """Animator Selectors を復元"""
        try:
            # ADBE Text Selectors グループを取得
            selectors_group = animator.property("ADBE Text Selectors")
            if not selectors_group or not selectors_group.is_group:
                debug_log(f"            ! Selectorsグループが見つかりません")
                return

            for selector_data in selectors_data:
                try:
                    selector_type = selector_data.get("type", "Range Selector")

                    # 新規 Selector を追加
                    if hasattr(selectors_group, 'can_add_stream') and selectors_group.can_add_stream("ADBE Text Selector"):
                        selector = selectors_group.add_stream("ADBE Text Selector")
                        if selector:
                            debug_log(f"            + Selector作成: {selector_type}")
                            self._restore_selector_properties(selector, selector_data)
                    else:
                        # 既存の Selector を使用（デフォルトで1つ存在する場合がある）
                        if selectors_group.num_properties > 0:
                            selector = selectors_group.property(0)
                            if selector:
                                self._restore_selector_properties(selector, selector_data)

                except Exception as e:
                    debug_log(f"            ! Selector復元失敗: {e}")

        except Exception as e:
            debug_log(f"            ! Selectors復元エラー: {e}")

    def _restore_selector_properties(self, selector, selector_data):
        """Selector のプロパティを復元"""
        properties = selector_data.get("properties", {})

        for prop_name, prop_data in properties.items():
            try:
                if not isinstance(prop_data, dict):
                    continue

                match_name = prop_data.get("match_name")
                prop_value = prop_data.get("value")

                if match_name is None or prop_value is None:
                    continue

                prop = selector.property(match_name)
                if prop and prop.valid and not prop.is_group:
                    prop.value = prop_value
                    debug_log(f"              + Selector.{prop_name} = {prop_value}")

                    # キーフレームを復元
                    keyframes = prop_data.get("keyframes", [])
                    if keyframes:
                        self._restore_keyframes(prop, keyframes)

                    # Expression を復元
                    expression = prop_data.get("expression")
                    if expression:
                        try:
                            prop.expression = expression
                            expression_enabled = prop_data.get("expression_enabled", True)
                            if hasattr(prop, 'expression_enabled'):
                                prop.expression_enabled = expression_enabled
                        except Exception as e:
                            debug_log(f"              ! Expression設定失敗: {e}")

            except Exception as e:
                debug_log(f"              ! Selectorプロパティ設定失敗: {prop_name}: {e}")

    def _restore_animator_properties(self, animator, properties_data):
        """Animator Properties を復元"""
        try:
            # ADBE Text Animator Properties グループを取得
            anim_props = animator.property("ADBE Text Animator Properties")
            if not anim_props or not anim_props.is_group:
                debug_log(f"            ! Animator Propertiesが見つかりません")
                return

            for prop_name, prop_data in properties_data.items():
                try:
                    if not isinstance(prop_data, dict):
                        continue

                    match_name = prop_data.get("match_name")
                    prop_value = prop_data.get("value")

                    if match_name is None:
                        continue

                    prop = anim_props.property(match_name)
                    if prop and prop.valid and not prop.is_group:
                        if prop_value is not None:
                            prop.value = prop_value
                            debug_log(f"            + {prop_name} = {prop_value}")

                        # キーフレームを復元
                        keyframes = prop_data.get("keyframes", [])
                        if keyframes:
                            self._restore_keyframes(prop, keyframes)

                        # Expression を復元
                        expression = prop_data.get("expression")
                        if expression:
                            try:
                                prop.expression = expression
                                expression_enabled = prop_data.get("expression_enabled", True)
                                if hasattr(prop, 'expression_enabled'):
                                    prop.expression_enabled = expression_enabled
                            except Exception as e:
                                debug_log(f"            ! Expression設定失敗: {e}")
                    else:
                        debug_log(f"            ! プロパティが見つかりません: {match_name}")

                except Exception as e:
                    debug_log(f"            ! プロパティ設定失敗: {prop_name}: {e}")

        except Exception as e:
            debug_log(f"            ! Animator Properties復元エラー: {e}")

    def _restore_keyframes(self, prop, keyframes_data):
        """キーフレームを復元"""
        try:
            for kf in keyframes_data:
                time = kf.get("time")
                value = kf.get("value")
                if time is not None and value is not None:
                    prop.set_value_at_time(time, value)
                    debug_log(f"              + キーフレーム: t={time}, v={value}")
        except Exception as e:
            debug_log(f"              ! キーフレーム復元失敗: {e}")

    def _restore_properties(self, layer, properties_data):
        """プロパティツリーを復元"""
        try:
            root = layer.properties
            if not root or not root.is_group:
                return

            for prop_name, prop_data in properties_data.items():
                try:
                    self._restore_property_recursive(root, prop_name, prop_data)
                except Exception as e:
                    debug_log(f"      プロパティスキップ: {prop_name}: {e}")

        except Exception as e:
            debug_log(f"      プロパティ復元エラー: {e}")

    def _restore_property_recursive(self, parent, prop_name, prop_data):
        """プロパティを再帰的に復元"""
        if not isinstance(prop_data, dict):
            return

        match_name = prop_data.get("match_name")
        if not match_name:
            return

        # スキップするプロパティ
        skip_props = [
            "ADBE Marker",           # マーカーは _restore_markers() で別途処理
            # "ADBE Time Remapping", # time_remapping_enabled フラグ設定後に復元可能
            "ADBE MTrackers",        # TrackerSuiteがAEGPで使用不可のため復元不可
            "ADBE Mask Parade",      # マスクは _restore_masks() で別途処理
            "ADBE Effect Parade",    # エフェクトは _restore_effects() で別途処理
            # "ADBE Layer Styles",   # レイヤースタイル復元を有効化（INDEXED_GROUP として自動処理）
            # "ADBE Audio Group",    # 子プロパティ（Audio Levels, Audio Pan）で復元可能
            # "ADBE Material Options Group",  # is_3d設定後に復元可能
            # "ADBE Extrsn Options Group",    # is_3d設定後に復元可能
            # "ADBE Plane Options Group",     # is_3d設定後に復元可能
            "ADBE Text Document",    # ソーステキストは set_source_text で処理済み
        ]
        if match_name in skip_props:
            log_debug(f"        スキップ: {match_name}")
            return

        prop_type = prop_data.get("type", "property")

        # シェイプコンテンツの処理を詳しくログ
        if match_name == "ADBE Root Vectors Group":
            log_debug(f"        [Shape] Processing Contents: {prop_name}, type={prop_type}")
            log_debug(f"        [Shape] Parent: {parent.match_name if hasattr(parent, 'match_name') else 'unknown'}")
            children = prop_data.get("children", {})
            log_debug(f"        [Shape] Number of children: {len(children)}")

        # Phase 3.2: allowed_groups allowlistを削除
        # すべてのプロパティグループを処理対象とする
        # ただし、skip_propsに含まれるものは除外

        prop = None

        if prop_type == "group":
            # グループはmatch_nameで取得
            try:
                prop = parent.property(match_name)
            except Exception as e:
                debug_log(f"        グループ取得失敗 (match_name={match_name}): {e}")

            if not prop or not prop.valid:
                try:
                    prop = parent.property(prop_name)
                except Exception as e:
                    debug_log(f"        グループ取得失敗 (prop_name={prop_name}): {e}")

            # INDEXED_GROUPの場合、プロパティが存在しない場合は add_stream で追加
            if not prop or not prop.valid:
                # 親が INDEXED_GROUP かチェック
                try:
                    parent_grouping = parent.grouping_type if hasattr(parent, 'grouping_type') else None
                    log_debug(f"        グループ未発見: {match_name}, 親grouping_type={parent_grouping}")

                    # grouping_typeの値をチェック (enumの場合と整数の場合に対応)
                    is_indexed_group = False
                    if hasattr(parent, 'grouping_type'):
                        gt = parent.grouping_type
                        # 整数比較と文字列比較の両方を試す
                        is_indexed_group = (gt == 2 or str(gt) == 'StreamGroupingType.IndexedGroup' or
                                          (hasattr(gt, 'value') and gt.value == 2))
                        log_debug(f"        is_indexed_group check: gt={gt}, type={type(gt)}, result={is_indexed_group}")

                    if is_indexed_group:
                        # add_stream で新規追加を試みる
                        if hasattr(parent, 'can_add_stream') and parent.can_add_stream(match_name):
                            prop = parent.add_stream(match_name)
                            log_debug(f"        [OK] 新しいストリームを追加: {match_name}")

                            # カスタム名を設定（prop_nameがmatch_nameと異なる場合のみ）
                            # prop_name: JSONのキー名（ユーザー設定名またはmatch_name）
                            # match_name: SDK識別子
                            if prop_name != match_name:
                                try:
                                    if hasattr(prop, 'set_name'):
                                        prop.set_name(prop_name)
                                        log_debug(f"        [OK] グループ名を設定: '{prop_name}'")
                                    else:
                                        log_debug(f"        [WARNING] set_name() メソッドが見つかりません")
                                except Exception as e:
                                    log_debug(f"        [ERROR] グループ名設定失敗: {e}")
                        else:
                            log_debug(f"        [ERROR] ストリームを追加できません: {match_name}")
                            return
                    else:
                        log_debug(f"        [SKIP] グループが見つかりません (親はIndexedGroupではない): {match_name}")
                        return
                except Exception as e:
                    log_debug(f"        [ERROR] add_stream 失敗: {match_name}: {e}")
                    import traceback
                    log_debug(traceback.format_exc())
                    return

            # グループの有効/無効フラグを復元（レイヤースタイルなど）
            if "enabled" in prop_data:
                try:
                    enabled = prop_data["enabled"]
                    # AEGP_DynStreamFlag_ACTIVE_EYEBALL (0x01) を設定
                    prop.set_dynamic_stream_flag(
                        0x01,     # AEGP_DynStreamFlag_ACTIVE_EYEBALL
                        True,     # undoable
                        enabled   # set_flag
                    )
                    log_debug(f"        Group {match_name} enabled set to: {enabled}")
                except (AttributeError, RuntimeError) as e:
                    log_debug(f"        Dynamic stream flag設定失敗 for {match_name}: {e}")

            # HIDDENフラグを復元（UIの表示/非表示）
            if "hidden" in prop_data:
                try:
                    hidden = prop_data["hidden"]
                    # AEGP_DynStreamFlag_HIDDEN (0x02) を設定
                    prop.set_dynamic_stream_flag(
                        0x02,     # AEGP_DynStreamFlag_HIDDEN
                        True,     # undoable
                        hidden    # set_flag
                    )
                    log_debug(f"        Group {match_name} hidden set to: {hidden}")
                except (AttributeError, RuntimeError) as e:
                    log_debug(f"        Dynamic stream HIDDEN flag設定失敗 for {match_name}: {e}")

            children = prop_data.get("children", {})
            if children:
                log_debug(f"        グループ {match_name} の子要素を処理: {len(children)}個")
            for child_name, child_data in children.items():
                try:
                    log_debug(f"          子要素: {child_name}")
                    self._restore_property_recursive(prop, child_name, child_data)
                except Exception as e:
                    log_debug(f"          [ERROR] 子プロパティ処理失敗 ({child_name}): {e}")
        else:
            # リーフプロパティはmatch_nameで直接取得
            try:
                prop = parent.property(match_name)
            except Exception as e:
                debug_log(f"        リーフ取得失敗 (match_name={match_name}): {e}")

            # match_nameで見つからない場合はprop_nameで試す
            if not prop or not prop.valid:
                try:
                    prop = parent.property(prop_name)
                except Exception as e:
                    debug_log(f"        リーフ取得失敗 (prop_name={prop_name}): {e}")

            if prop and prop.valid:
                self._restore_property_value(prop, prop_data)
            else:
                debug_log(f"        リーフプロパティが見つかりません: {match_name} (親: {parent.match_name if hasattr(parent, 'match_name') else 'unknown'})")

    def _restore_property_value(self, prop, prop_data):
        """プロパティ値を復元（Phase 3: expression対応、Phase 7: Path対応）"""
        match_name = prop_data.get("match_name", "")

        # Phase 3.1: Expression復元
        expression = prop_data.get("expression")
        if expression:
            try:
                prop.expression = expression
                debug_log(f"          Expression設定: {expression[:50]}..." if len(expression) > 50 else f"          Expression設定: {expression}")
            except Exception as e:
                debug_log(f"          Expression設定失敗 ({match_name}): {e}")

        # Phase 7.2: Shape Path データの特別処理
        if match_name == "ADBE Vector Shape - Group":
            # Path データは bdata から頂点配列に変換して set_shape_path_vertices() を使用
            bdata = prop_data.get("bdata", "")
            if bdata:
                vertices = bdata_to_path_vertices(bdata)
                if vertices:
                    try:
                        prop.set_shape_path_vertices(vertices, 0.0)
                        debug_log(f"          Shape Path 頂点設定: {len(vertices)} vertices")
                    except Exception as e:
                        debug_log(f"          Shape Path 頂点設定失敗: {e}")
            # キーフレームがある場合も処理
            keyframes = prop_data.get("keyframes", [])
            if keyframes:
                for kf_data in keyframes:
                    time = kf_data.get("time", 0.0)
                    bdata_kf = kf_data.get("bdata", "")
                    if bdata_kf:
                        vertices_kf = bdata_to_path_vertices(bdata_kf)
                        if vertices_kf:
                            try:
                                prop.set_shape_path_vertices(vertices_kf, time)
                                debug_log(f"          Shape Path キーフレーム設定 (t={time}): {len(vertices_kf)} vertices")
                            except Exception as e:
                                debug_log(f"          Shape Path キーフレーム設定失敗 (t={time}): {e}")
            return

        # 通常のプロパティ値復元
        value = prop_data.get("_value")
        if value is None:
            bdata = prop_data.get("bdata", "")
            if bdata:
                value = bdata_to_value(bdata)

        if value is None:
            debug_log(f"          値なし: {match_name}")
            return

        # 静的値を設定
        try:
            prop.value = value
            debug_log(f"          値設定: {match_name} = {value}")
        except Exception as e:
            debug_log(f"          値設定失敗 ({match_name}): {e}")

        # キーフレーム復元
        keyframes = prop_data.get("keyframes", [])
        if keyframes:
            debug_log(f"          キーフレーム復元開始: {match_name} ({len(keyframes)}個)")
            self._restore_keyframes(prop, keyframes)

    def _restore_keyframes(self, prop, keyframes_data):
        """キーフレームを復元"""
        if not prop.can_have_keyframes:
            debug_log(f"          プロパティ {prop.match_name} はキーフレーム非対応")
            return

        for kf_data in keyframes_data:
            time = kf_data.get("time", 0.0)
            bdata = kf_data.get("bdata", "")

            value = bdata_to_value(bdata)
            if value is None:
                debug_log(f"          キーフレーム値変換失敗 (t={time}): bdata={bdata[:20]}...")
                continue

            try:
                # add_keyframe(time, value) - 2引数必須、インデックス(int)を返す
                kf_index = prop.add_keyframe(time, value)
                debug_log(f"          キーフレーム追加 (t={time}): index={kf_index}, value={value}")

                # 補間タイプを設定
                in_interp = kf_data.get("in_interpolation")
                out_interp = kf_data.get("out_interpolation")
                if in_interp is not None and out_interp is not None:
                    try:
                        prop.set_keyframe_interpolation(kf_index, in_interp, out_interp)
                        debug_log(f"          補間設定: {in_interp}/{out_interp}")
                    except Exception as e:
                        debug_log(f"          補間設定失敗 (index={kf_index}): {e}")

            except Exception as e:
                debug_log(f"          キーフレーム追加失敗 (t={time}, value={value}): {e}")


def import_project(input_path):
    """JSONファイルからシーンをインポート"""
    importer = SceneImporter(input_path)
    return importer.run()


if __name__ == "__main__":
    docs_path = ae.get_documents_folder()
    in_file = os.path.join(docs_path, "scene_dump.json")

    try:
        success = import_project(in_file)
        if success:
            ae.alert("インポート完了しました。")
        else:
            ae.alert("インポートに失敗しました。")
    except Exception as e:
        import traceback
        ae.alert(f"インポートエラー:\n{str(e)}")
        log_error(f"インポートエラー:\n{traceback.format_exc()}")
