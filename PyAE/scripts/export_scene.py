import ae
import json
import os
import binascii
import traceback

# デバッグモード（詳細ログを出力）
# テスト時はTrue、本番ではFalseに設定
DEBUG_MODE = True

def debug_log(message):
    """デバッグログ出力"""
    if DEBUG_MODE:
        ae.log_info(f"[DEBUG] {message}")


def item_type_to_string(item_type):
    """ItemTypeを文字列に変換"""
    type_map = {
        ae.ItemType.Comp: "Comp",
        ae.ItemType.Folder: "Folder",
        ae.ItemType.Footage: "Footage",
    }
    return type_map.get(item_type, str(item_type))


def layer_type_to_string(layer_type):
    """LayerTypeを文字列に変換"""
    type_map = {
        ae.LayerType.AV: "AV",
        ae.LayerType.Camera: "Camera",
        ae.LayerType.Light: "Light",
        ae.LayerType.Text: "Text",
        ae.LayerType.Shape: "Shape",
        ae.LayerType.Adjustment: "Adjustment",
        ae.LayerType.Null: "Null",
        ae.LayerType.Solid: "Solid",
    }
    return type_map.get(layer_type, str(layer_type))


def export_project(output_path):
    project = ae.get_project()
    if not project:
        raise RuntimeError("No project open")

    debug_log(f"プロジェクト内のアイテム数: {len(project.items)}")

    data = {"version": 1, "items": []}

    # プロジェクト内のアイテムを走査
    for idx, item in enumerate(project.items, 1):
        debug_log(f"アイテム {idx}/{len(project.items)}: {item.name} ({item_type_to_string(item.type)})")
        item_data = {
            "name": item.name,
            "type": item_type_to_string(item.type),
            "id": item.id,
        }

        # 親フォルダIDを保存（オプショナル）
        try:
            parent_folder = item.parent_folder
            if parent_folder and hasattr(parent_folder, "id"):
                item_data["parent_folder_id"] = parent_folder.id
        except AttributeError as e:
            debug_log(f"  親フォルダID取得失敗（属性なし）: {e}")

        if item.type == ae.ItemType.Comp:
            debug_log(f"  Comp処理開始: {item.name}")
            item_data["comp_data"] = export_comp(item)
            debug_log(f"  Comp処理完了")
        elif item.type == ae.ItemType.Footage:
            # Footageの詳細情報を出力
            item_data["footage_data"] = export_footage(item)

        data["items"].append(item_data)

    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)

    ae.log_info(f"Exported scene to {output_path}")


def export_footage(item):
    """
    Footageアイテムの詳細情報をエクスポート

    Args:
        item: Footageアイテム

    Returns:
        dict: Footage詳細情報
    """
    footage_data = {
        "width": item.width,
        "height": item.height,
        "duration": item.duration,
    }

    try:
        # FootageItemのプロパティに直接アクセス（resolve()は不要）
        # FootageTypeを取得
        if hasattr(item, "footage_type"):
            ft = item.footage_type
            if ft == ae.FootageType.Solid:
                footage_data["footage_type"] = "Solid"
                debug_log(f"  Footage: {item.name} (Solid)")
            elif ft == ae.FootageType.Missing:
                footage_data["footage_type"] = "Missing"
                debug_log(f"  Footage: {item.name} (Missing)")
            elif ft == ae.FootageType.Placeholder:
                footage_data["footage_type"] = "Placeholder"
                debug_log(f"  Footage: {item.name} (Placeholder)")
            else:
                footage_data["footage_type"] = "File"
                debug_log(f"  Footage: {item.name} (File)")

        # ファイルパスを取得
        if hasattr(item, "file_path"):
            file_path = item.file_path
            if file_path:
                footage_data["file_path"] = file_path
                debug_log(f"    file_path: {file_path}")

        # シーケンスインポートオプションを取得
        if hasattr(item, "sequence_options"):
            try:
                seq_opts = item.sequence_options
                if seq_opts and seq_opts.get("is_sequence", False):
                    footage_data["sequence_options"] = seq_opts
                    debug_log(f"    sequence_options: {seq_opts}")
            except Exception as e:
                debug_log(f"    sequence_options取得エラー: {e}")

    except Exception as e:
        debug_log(f"  Footage情報取得エラー: {e}")

    return footage_data


def export_comp(comp):
    debug_log(f"Comp: {comp.name} ({comp.width}x{comp.height}, {comp.frame_rate}fps)")

    comp_data = {
        "width": comp.width,
        "height": comp.height,
        "pixel_aspect": comp.pixel_aspect,
        "frame_rate": comp.frame_rate,
        "duration": comp.duration,
        "layers": [],
    }

    # 背景色を取得（オプショナル）
    try:
        bg_color = comp.bg_color
        if bg_color:
            comp_data["background_color"] = list(bg_color)
            debug_log(f"  背景色: {bg_color}")
    except AttributeError as e:
        debug_log(f"  背景色取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  背景色取得失敗（SDKエラー）: {e}")

    # ワークエリア設定を取得（オプショナル）
    try:
        comp_data["work_area_start"] = comp.work_area_start
        comp_data["work_area_duration"] = comp.work_area_duration
        debug_log(f"  ワークエリア: {comp.work_area_start}s ~ {comp.work_area_duration}s")
    except AttributeError as e:
        debug_log(f"  ワークエリア取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  ワークエリア取得失敗（SDKエラー）: {e}")

    debug_log(f"  レイヤー数: {len(comp.layers)}")
    for idx, layer in enumerate(comp.layers, 1):
        debug_log(f"  レイヤー {idx}/{len(comp.layers)}: {layer.name}")
        comp_data["layers"].append(export_layer(layer, comp))

    return comp_data


def _export_text_animator(animator):
    """Phase 9: Text Animator をエクスポート"""
    try:
        animator_data = {
            "name": animator.name,
            "match_name": animator.match_name,
            "selectors": [],
            "properties": {}
        }

        # Selectors をエクスポート
        selectors_group = animator.property("ADBE Text Selectors")
        if selectors_group and selectors_group.is_group:
            for sel_idx in range(selectors_group.num_properties):
                selector = selectors_group.property(sel_idx)
                if selector:
                    selector_data = _export_text_selector(selector)
                    if selector_data:
                        animator_data["selectors"].append(selector_data)

        # Animator Properties をエクスポート
        props_group = animator.property("ADBE Text Animator Properties")
        if props_group and props_group.is_group:
            for prop_idx in range(props_group.num_properties):
                prop = props_group.property(prop_idx)
                if prop and not prop.is_group:
                    # デフォルト値でないプロパティのみエクスポート
                    try:
                        value = prop.value
                        # デフォルト値かどうかをチェック（簡易版）
                        is_default = _is_default_animator_value(prop.match_name, value)
                        if not is_default or prop.num_keyframes > 0:
                            prop_data = {
                                "match_name": prop.match_name,
                                "value": _convert_value_for_json(value),
                                "num_keys": prop.num_keyframes
                            }
                            # キーフレームがあれば取得
                            if prop.num_keyframes > 0:
                                prop_data["keyframes"] = _export_keyframes(prop)
                            # エクスプレッションがあれば取得
                            try:
                                if prop.can_have_expression and prop.has_expression:
                                    prop_data["expression"] = prop.expression
                                    prop_data["expression_enabled"] = prop.expression_enabled
                            except:
                                pass
                            animator_data["properties"][prop.name] = prop_data
                    except:
                        pass

        return animator_data
    except Exception as e:
        debug_log(f"    Animator エクスポート失敗: {e}")
        return None


def _export_text_selector(selector):
    """Phase 9: Text Selector をエクスポート"""
    try:
        selector_data = {
            "name": selector.name,
            "match_name": selector.match_name,
            "properties": {}
        }

        # Selector のプロパティをエクスポート
        if selector.is_group:
            for prop_idx in range(selector.num_properties):
                prop = selector.property(prop_idx)
                if prop and not prop.is_group:
                    try:
                        value = prop.value
                        prop_data = {
                            "match_name": prop.match_name,
                            "value": _convert_value_for_json(value),
                            "num_keys": prop.num_keyframes
                        }
                        if prop.num_keyframes > 0:
                            prop_data["keyframes"] = _export_keyframes(prop)
                        selector_data["properties"][prop.name] = prop_data
                    except:
                        pass

        return selector_data
    except Exception as e:
        debug_log(f"    Selector エクスポート失敗: {e}")
        return None


def _is_default_animator_value(match_name, value):
    """Animator Property がデフォルト値かどうかを判定"""
    # 主要なプロパティのデフォルト値
    defaults = {
        "ADBE Text Position 3D": (0.0, 0.0, 0.0),
        "ADBE Text Anchor Point 3D": (0.0, 0.0, 0.0),
        "ADBE Text Scale 3D": (100.0, 100.0, 100.0),
        "ADBE Text Rotation": 0.0,
        "ADBE Text Rotation X": 0.0,
        "ADBE Text Rotation Y": 0.0,
        "ADBE Text Opacity": 100.0,
        "ADBE Text Skew": 0.0,
        "ADBE Text Skew Axis": 0.0,
        "ADBE Text Fill Opacity": 100.0,
        "ADBE Text Stroke Opacity": 100.0,
        "ADBE Text Tracking Amount": 0.0,
        "ADBE Text Line Spacing": (0.0, 0.0),
        "ADBE Text Blur": (0.0, 0.0),
    }

    default_value = defaults.get(match_name)
    if default_value is None:
        return False

    # 値を比較
    if isinstance(value, (list, tuple)) and isinstance(default_value, (list, tuple)):
        if len(value) != len(default_value):
            return False
        return all(abs(v - d) < 0.001 for v, d in zip(value, default_value))
    elif isinstance(value, (int, float)) and isinstance(default_value, (int, float)):
        return abs(value - default_value) < 0.001
    return False


def _convert_value_for_json(value):
    """値をJSON互換形式に変換"""
    if isinstance(value, (list, tuple)):
        return [_convert_value_for_json(v) for v in value]
    elif isinstance(value, (int, float)):
        return value
    elif value is None:
        return None
    else:
        return str(value)


def _export_keyframes(prop):
    """キーフレームをエクスポート"""
    keyframes = []
    try:
        for i in range(prop.num_keyframes):
            kf = {
                "time": prop.get_keyframe_time(i),
                "value": _convert_value_for_json(prop.get_keyframe_value(i))
            }
            # 補間タイプを取得
            try:
                in_interp, out_interp = prop.get_keyframe_interpolation(i)
                kf["in_interpolation"] = in_interp
                kf["out_interpolation"] = out_interp
            except:
                pass
            keyframes.append(kf)
    except:
        pass
    return keyframes


def export_layer(layer, comp=None):
    """レイヤーをエクスポート

    Args:
        layer: エクスポート対象のレイヤー
        comp: レイヤーの親コンポジション（threeDPerChar取得に必要）
    """
    layer_type = layer_type_to_string(layer.layer_type)
    debug_log(f"レイヤー処理開始: {layer.name} (type: {layer_type})")

    layer_data = {
        "name": layer.name,
        "index": layer.index,
        "type": layer_type,
        "active": layer.enabled,
        "is_null": layer.is_null,
        "is_adjustment": layer.is_adjustment,
        "is_3d": layer.is_3d,
        "time_remapping_enabled": layer.time_remapping_enabled,
        "in_point": layer.in_point,
        "out_point": layer.out_point,
        "start_time": layer.start_time,
        "properties": {},
        "effects": [],
    }

    # PyLayerRefで実装済みの追加プロパティを取得
    try:
        layer_data["solo"] = layer.solo
        debug_log(f"  solo: {layer.solo}")
    except (AttributeError, RuntimeError) as e:
        debug_log(f"  solo取得失敗: {e}")

    try:
        layer_data["locked"] = layer.locked
        debug_log(f"  locked: {layer.locked}")
    except (AttributeError, RuntimeError) as e:
        debug_log(f"  locked取得失敗: {e}")

    try:
        layer_data["shy"] = layer.shy
        debug_log(f"  shy: {layer.shy}")
    except (AttributeError, RuntimeError) as e:
        debug_log(f"  shy取得失敗: {e}")

    try:
        layer_data["collapse_transformation"] = layer.collapse_transformation
        debug_log(f"  collapse_transformation: {layer.collapse_transformation}")
    except (AttributeError, RuntimeError) as e:
        debug_log(f"  collapse_transformation取得失敗: {e}")

    # Phase 2: 追加のレイヤープロパティ
    try:
        blend_mode = layer.blend_mode
        layer_data["blend_mode"] = blend_mode
        debug_log(f"  blend_mode: {blend_mode}")
    except AttributeError as e:
        debug_log(f"  blend_mode取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  blend_mode取得失敗（SDKエラー）: {e}")
        if DEBUG_MODE:
            traceback.print_exc()

    try:
        quality = layer.quality_string
        layer_data["quality_string"] = quality
        debug_log(f"  quality_string: {quality}")
    except AttributeError as e:
        debug_log(f"  quality_string取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  quality_string取得失敗（SDKエラー）: {e}")
        if DEBUG_MODE:
            traceback.print_exc()

    try:
        label = layer.label
        layer_data["label"] = label
        debug_log(f"  label: {label}")
    except AttributeError as e:
        debug_log(f"  label取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  label取得失敗（SDKエラー）: {e}")
        if DEBUG_MODE:
            traceback.print_exc()

    try:
        comment = layer.comment
        if comment:
            layer_data["comment"] = comment
            debug_log(f"  comment: {comment}")
    except AttributeError as e:
        debug_log(f"  comment取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  comment取得失敗（SDKエラー）: {e}")
        if DEBUG_MODE:
            traceback.print_exc()

    # マーカーコメントを取得（タイムラインのComment列に表示されるもの）
    try:
        # レイヤーの開始位置でマーカーコメントを確認
        marker_comment = layer.get_marker_comment(layer.in_point)
        if marker_comment:
            layer_data["marker_comment"] = marker_comment
            layer_data["marker_time"] = layer.in_point
            debug_log(f"  marker_comment (at in_point={layer.in_point}): {marker_comment}")
    except AttributeError as e:
        debug_log(f"  marker_comment取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  marker_comment取得失敗（SDKエラー）: {e}")
        if DEBUG_MODE:
            traceback.print_exc()

    try:
        track_matte = layer.track_matte
        if track_matte is not None:
            layer_data["track_matte"] = track_matte
            debug_log(f"  track_matte: {track_matte}")
        else:
            debug_log("  track_matte: None")
    except AttributeError as e:
        debug_log(f"  track_matte取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  track_matte取得失敗（SDKエラー）: {e}")
        if DEBUG_MODE:
            traceback.print_exc()

    # ソースアイテムIDを取得（重要: FootageとCompの紐付け）
    try:
        source_id = layer.source_item_id
        if source_id:
            layer_data["source_item_id"] = source_id
            debug_log(f"  source_item_id: {source_id}")
    except AttributeError as e:
        debug_log(f"  source_item_id取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  source_item_id取得失敗（SDKエラー）: {e}")

    # 親レイヤーのインデックスを取得（レイヤー階層の復元に必須）
    try:
        parent = layer.parent
        if parent:
            layer_data["parent_index"] = parent.index
            debug_log(f"  parent_index: {parent.index}")
    except AttributeError as e:
        debug_log(f"  parent_index取得失敗（属性なし）: {e}")
    except RuntimeError as e:
        debug_log(f"  parent_index取得失敗（SDKエラー）: {e}")

    # レイヤータイプ別の追加情報
    # Note: layer_typeは大文字小文字が混在する可能性があるため、lower()で比較
    layer_type_lower = layer_type.lower()
    if layer_type_lower in ("camera", "light"):
        debug_log(f"  {layer_type}固有プロパティ取得開始")
        try:
            root = layer.properties
            if root and root.is_group:
                # Camera固有プロパティ
                if layer_type_lower == "camera":
                    try:
                        camera_opts = root.property("ADBE Camera Options Group")
                        if camera_opts and camera_opts.is_group:
                            layer_data["camera_options"] = {}
                            debug_log(f"    Camera Optionsグループ検出")
                            for cam_prop in camera_opts:
                                try:
                                    prop_name = cam_prop.name if cam_prop.name else cam_prop.match_name
                                    if not cam_prop.is_group:
                                        # match_name と value を保存（インポート時の確実な復元のため）
                                        layer_data["camera_options"][prop_name] = {
                                            "match_name": cam_prop.match_name,
                                            "value": cam_prop.value,
                                            "num_keys": cam_prop.num_keyframes
                                        }
                                        debug_log(f"      {prop_name} ({cam_prop.match_name}): {cam_prop.value}")
                                except (AttributeError, RuntimeError) as e:
                                    debug_log(f"      {prop_name}取得失敗: {e}")
                        else:
                            debug_log("    Camera Optionsグループが見つかりません")
                    except RuntimeError as e:
                        debug_log(f"    Camera Options取得失敗: {e}")

                # Light固有プロパティ
                if layer_type_lower == "light":
                    try:
                        light_opts = root.property("ADBE Light Options Group")
                        if light_opts and light_opts.is_group:
                            layer_data["light_options"] = {}
                            debug_log(f"    Light Optionsグループ検出")
                            for light_prop in light_opts:
                                try:
                                    prop_name = light_prop.name if light_prop.name else light_prop.match_name
                                    if not light_prop.is_group:
                                        # match_name と value を保存（インポート時の確実な復元のため）
                                        layer_data["light_options"][prop_name] = {
                                            "match_name": light_prop.match_name,
                                            "value": light_prop.value,
                                            "num_keys": light_prop.num_keyframes
                                        }
                                        debug_log(f"      {prop_name} ({light_prop.match_name}): {light_prop.value}")
                                except (AttributeError, RuntimeError) as e:
                                    debug_log(f"      {prop_name}取得失敗: {e}")
                        else:
                            debug_log("    Light Optionsグループが見つかりません")
                    except RuntimeError as e:
                        debug_log(f"    Light Options取得失敗: {e}")
        except RuntimeError as e:
            debug_log(f"  {layer_type}固有プロパティ取得失敗: {e}")

    elif layer_type_lower == "text":
        layer_data["has_text"] = True

        # threeDPerChar 属性を取得 (ExtendScript 経由)
        # これは Text Animator の Scale Z 軸復元に必要
        try:
            debug_log(f"  threeDPerChar 取得開始...")
            # execute_extendscript が利用可能か確認
            if not hasattr(ae, 'execute_extendscript'):
                debug_log(f"  WARNING: ae.execute_extendscript は利用できません")
                layer_data["threeDPerChar"] = False
            elif comp is None:
                debug_log(f"  WARNING: comp パラメータが渡されていません")
                layer_data["threeDPerChar"] = False
            else:
                comp_name = comp.name
                layer_index = layer.index + 1  # AE ExtendScript は 1-based インデックス
                debug_log(f"    comp: {comp_name}, layer index (1-based): {layer_index}")
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
                        return comp.layer({layer_index}).threeDPerChar ? "true" : "false";
                    }}
                    return "false";
                }})();
                '''
                debug_log(f"    ExtendScript 実行中...")
                result = ae.execute_extendscript(three_d_script)
                debug_log(f"    ExtendScript 結果: '{result}'")
                layer_data["threeDPerChar"] = result.strip().lower() == "true"
                debug_log(f"  threeDPerChar: {layer_data['threeDPerChar']}")
        except Exception as e:
            debug_log(f"  threeDPerChar 取得失敗: {e}")
            import traceback
            debug_log(f"  トレースバック: {traceback.format_exc()}")
            layer_data["threeDPerChar"] = False

        # ソーステキストを取得
        try:
            root = layer.properties
            if root and root.is_group:
                text_props = root.property("ADBE Text Properties")
                debug_log(f"  text_props found: {text_props is not None}")
                if text_props and text_props.is_group:
                    source_text_prop = text_props.property("ADBE Text Document")
                    debug_log(f"  source_text_prop found: {source_text_prop is not None}")
                    if source_text_prop:
                        # デバッグ: プロパティのタイプを確認
                        debug_log(f"  stream_value_type: {source_text_prop.stream_value_type}")
                        debug_log(f"  is_leaf: {source_text_prop.is_leaf}")
                        debug_log(f"  can_set_value: {source_text_prop.can_set_value}")
                        # GetValue()でテキスト文字列を取得
                        source_text = source_text_prop.value
                        debug_log(f"  value type: {type(source_text)}")
                        debug_log(f"  value repr: {repr(source_text)}")
                        if source_text is not None:
                            layer_data["source_text"] = source_text
                            debug_log(f"  source_text: {source_text[:50]}..." if len(str(source_text)) > 50 else f"  source_text: {source_text}")
                        else:
                            debug_log(f"  source_text is None!")

                        # ソーステキストのエクスプレッションを取得
                        try:
                            if source_text_prop.can_have_expression:
                                debug_log(f"  can_have_expression: True")
                                if source_text_prop.has_expression:
                                    layer_data["source_text_expression"] = source_text_prop.expression
                                    layer_data["source_text_expression_enabled"] = source_text_prop.expression_enabled
                                    debug_log(f"  source_text_expression: {source_text_prop.expression[:50]}...")
                            else:
                                debug_log(f"  can_have_expression: False")
                        except (AttributeError, RuntimeError) as e:
                            debug_log(f"  ソーステキストエクスプレッション取得失敗: {e}")

                    # Phase 9: Text Animators をエクスポート
                    animators_group = text_props.property("ADBE Text Animators")
                    if animators_group and animators_group.is_group and animators_group.num_properties > 0:
                        layer_data["text_animators"] = []
                        debug_log(f"  Text Animators: {animators_group.num_properties} 個")
                        for anim_idx in range(animators_group.num_properties):
                            animator = animators_group.property(anim_idx)
                            if animator:
                                animator_data = _export_text_animator(animator)
                                if animator_data:
                                    layer_data["text_animators"].append(animator_data)
                                    debug_log(f"    Animator '{animator_data.get('name')}' エクスポート完了")
        except (AttributeError, RuntimeError) as e:
            debug_log(f"  ソーステキスト取得失敗: {e}")
            import traceback
            traceback.print_exc()

    elif layer_type_lower == "solid":
        # Solidの幅・高さと色を取得
        try:
            layer_data["width"] = layer.width
            layer_data["height"] = layer.height
        except (AttributeError, RuntimeError) as e:
            debug_log(f"  Solid幅・高さ取得失敗: {e}")

        # Solid Colorを取得（Effects > Solid Settings > Color）
        try:
            for effect in layer.effects:
                if "Solid" in effect.match_name or "Solid" in effect.name:
                    for param in effect.properties:
                        if "Color" in param.name or param.index == 0:
                            color_value = param.value
                            if isinstance(color_value, (list, tuple)) and len(color_value) >= 3:
                                layer_data["solid_color"] = list(color_value[:3])
                            break
                    break
        except (AttributeError, RuntimeError) as e:
            debug_log(f"  Solid Color取得失敗: {e}")

    elif layer_type_lower == "av" and not layer.is_null:
        # AVレイヤーの幅・高さを取得
        try:
            layer_data["width"] = layer.width
            layer_data["height"] = layer.height
        except (AttributeError, RuntimeError) as e:
            debug_log(f"  AVレイヤー幅・高さ取得失敗: {e}")

    # Dynamic Stream を使ってプロパティを動的に取得
    try:
        root = layer.properties
        if root and root.is_group:
            for child in root:
                try:
                    name = child.name if child.name else child.match_name
                    layer_data["properties"][name] = export_property_tree(child)
                except RuntimeError as e:
                    debug_log(f"  プロパティ '{name}' 取得失敗: {e}")

    except RuntimeError as e:
        debug_log(f"  プロパティツリー取得失敗: {e}")

    # エフェクト
    for effect in layer.effects:
        try:
            effect_data = export_effect(effect)
            layer_data["effects"].append(effect_data)
        except RuntimeError as e:
            debug_log(f"  エフェクト '{effect.name}' 取得失敗: {e}")

    return layer_data


def value_to_bdata(value):
    """Python値をbdata形式のhex文字列に変換"""
    import struct

    if value is None:
        return ""

    if isinstance(value, bytes):
        # 既にバイナリデータ
        return value.hex()

    if isinstance(value, (int, float)):
        # スカラー値: IEEE 754 double ビッグエンディアン
        return struct.pack('>d', float(value)).hex()

    if isinstance(value, (list, tuple)):
        # 配列: 各要素をdoubleとして連結
        data = b''
        for v in value:
            if isinstance(v, (int, float)):
                data += struct.pack('>d', float(v))
        return data.hex()

    if isinstance(value, str):
        # 文字列: UTF-16LE
        return value.encode('utf-16-le').hex()

    # その他は文字列化してUTF-8
    return str(value).encode('utf-8').hex()


def export_property_tree(prop):
    """プロパティツリーを再帰的にエクスポート（bdata形式）"""
    result = {
        "match_name": prop.match_name,
    }

    if prop.is_group:
        result["type"] = "group"

        # グループの有効/無効フラグを取得（レイヤースタイルなど）
        try:
            flags = prop.get_dynamic_stream_flags()
            # AEGP_DynStreamFlag_ACTIVE_EYEBALL (0x01) と HIDDEN (0x02) をチェック
            is_enabled = bool(flags & 0x01)
            is_hidden = bool(flags & 0x02)
            result["enabled"] = is_enabled
            result["hidden"] = is_hidden
            debug_log(f"      Group {prop.match_name} enabled: {is_enabled}, hidden: {is_hidden} (flags: 0x{flags:02x})")
        except (AttributeError, RuntimeError) as e:
            # DynamicStreamフラグが取得できない場合は無視（通常のグループ）
            debug_log(f"      Dynamic stream flags not available for {prop.match_name}: {e}")

        result["children"] = {}
        for child in prop:
            try:
                name = child.name if child.name else child.match_name
                result["children"][name] = export_property_tree(child)
            except RuntimeError as e:
                debug_log(f"      子プロパティ '{name}' 取得失敗: {e}")

    else:
        result["type"] = "property"
        result["num_keys"] = prop.num_keyframes

        # メタデータ取得（tdb4, tdum, tduM相当）
        try:
            metadata = prop.get_stream_metadata()
            if metadata:
                # tdum/tduM: 最小/最大値をbdata形式で出力
                if "min_value" in metadata:
                    result["tdum"] = value_to_bdata(metadata["min_value"])
                if "max_value" in metadata:
                    result["tduM"] = value_to_bdata(metadata["max_value"])

                # tdb4相当: フラグ情報
                if "flags" in metadata:
                    result["flags"] = metadata["flags"]

                # 追加メタデータ（デバッグ用）
                if DEBUG_MODE:
                    result["_metadata"] = metadata
        except (AttributeError, RuntimeError) as e:
            debug_log(f"      メタデータ取得失敗: {e}")

        # bdata形式で値を取得
        try:
            # get_raw_bytes()でバイナリデータを取得
            raw_bytes = prop.get_raw_bytes(0.0)
            if raw_bytes and len(raw_bytes) > 0:
                result["bdata"] = raw_bytes.hex()
            else:
                # フォールバック: 通常の値をbdata形式に変換
                value = prop.value
                result["bdata"] = value_to_bdata(value)

            # デバッグ用に可読値も保持（オプション）
            if DEBUG_MODE:
                try:
                    result["_value"] = prop.value
                except (RuntimeError, AttributeError) as e:
                    debug_log(f"      デバッグ値取得失敗 ({prop.match_name}): {type(e).__name__}: {e}")
                    result["_value"] = None

        except (AttributeError, RuntimeError) as e:
            debug_log(f"      bdata取得失敗: {e}")
            # フォールバック
            try:
                result["bdata"] = value_to_bdata(prop.value)
            except (RuntimeError, AttributeError, ValueError) as e:
                ae.log_warning(f"      bdata変換失敗 ({prop.match_name}): {type(e).__name__}: {e}")
                result["bdata"] = None  # 空文字列ではなくNoneで明示的に失敗を示す

        # エクスプレッションの取得
        try:
            if prop.can_have_expression:
                if prop.has_expression:
                    result["expression"] = prop.expression
                    result["expression_enabled"] = True
                    debug_log(f"      エクスプレッション検出: {prop.match_name}")
        except (AttributeError, RuntimeError) as e:
            debug_log(f"      エクスプレッション取得失敗: {e}")

        # キーフレーム詳細の取得（bdata形式 + 補間タイプ）
        if prop.can_have_keyframes and prop.num_keyframes > 0:
            result["keyframes"] = []
            debug_log(f"      キーフレーム数: {prop.num_keyframes} ({prop.match_name})")
            try:
                num_keys = prop.num_keyframes
                for i in range(num_keys):
                    try:
                        # 時間と値
                        time = prop.get_keyframe_time(i)
                        value = prop.get_keyframe_value(i)

                        kf_data = {
                            "time": time,
                            "bdata": value_to_bdata(value),
                        }

                        # 補間タイプを直接取得
                        try:
                            in_interp, out_interp = prop.get_keyframe_interpolation(i)
                            kf_data["in_interpolation"] = in_interp
                            kf_data["out_interpolation"] = out_interp
                        except (AttributeError, RuntimeError) as e:
                            debug_log(f"        キーフレーム[{i}]補間タイプ取得失敗: {e}")

                        result["keyframes"].append(kf_data)
                    except (RuntimeError, IndexError) as e:
                        debug_log(f"        キーフレーム[{i}]取得失敗: {e}")

                debug_log(f"        {len(result['keyframes'])}個のキーフレーム詳細を取得")
            except (AttributeError, RuntimeError) as e:
                debug_log(f"        キーフレーム詳細取得失敗: {e}")
                if DEBUG_MODE:
                    traceback.print_exc()

    return result


def export_effect(effect):
    # stream_name: カスタム設定された名前（ユーザーが変更した名前）
    # match_name: プラグイン識別子（"ADBE Fill"など、エフェクト追加に使用）
    effect_stream_name = effect.stream_name if hasattr(effect, 'stream_name') else None
    plugin_name = effect.name

    # プラグイン名とstream_nameが同じ場合はカスタム名なし
    # （デフォルト名のままなのでNoneを保存）
    # 注意: 文字エンコーディングの違いを考慮して比較
    if effect_stream_name and plugin_name:
        try:
            # 両方を正規化して比較（空白削除、大文字小文字無視）
            normalized_stream = effect_stream_name.strip()
            normalized_plugin = plugin_name.strip()
            if normalized_stream == normalized_plugin:
                effect_stream_name = None
        except Exception:
            # 比較失敗時は安全のためstream_nameを保持
            pass

    effect_data = {
        "stream_name": effect_stream_name,  # カスタム名（なければNone）
        "match_name": effect.match_name,    # 必須: エフェクト追加に使用
        "enabled": effect.enabled,
        "params": [],
    }

    # effect.paramsの代わりにparam_by_indexを使用
    try:
        num_params = effect.num_properties
        for i in range(num_params):
            try:
                param = effect.property_by_index(i)
                if param and param.valid:
                    param_data = export_param(param)
                    if param_data:
                        effect_data["params"].append(param_data)
            except RuntimeError as e:
                debug_log(f"    エフェクトパラメータ[{i}]取得失敗: {e}")
    except (AttributeError, RuntimeError) as e:
        debug_log(f"    エフェクトパラメータ取得失敗: {e}")

    return effect_data


def export_param(param):
    if not param.valid:
        return None

    val = param.value

    # すべてbdata形式で出力
    if isinstance(val, bytes):
        bdata = val.hex()
    else:
        bdata = value_to_bdata(val)

    return {
        "name": param.name,
        "index": param.index,
        "bdata": bdata,
        "keyframes": [],
    }


if __name__ == "__main__":
    # 出力先はドキュメントフォルダ
    docs_path = ae.get_documents_folder()
    out_file = os.path.join(docs_path, "scene_dump.json")

    ae.log_info("=" * 60)
    ae.log_info("PyAE Export Scene - 改善版テスト実行")
    ae.log_info(f"Debug Mode: {DEBUG_MODE}")
    ae.log_info("=" * 60)

    try:
        export_project(out_file)

        # ファイルサイズを確認
        file_size = os.path.getsize(out_file)
        ae.log_info(f"エクスポート成功: {out_file}")
        ae.log_info(f"ファイルサイズ: {file_size:,} bytes ({file_size/1024:.1f} KB)")

        # JSONの内容をサマリー
        with open(out_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        total_items = len(data.get("items", []))
        total_layers = 0
        for item in data.get("items", []):
            if "comp_data" in item:
                total_layers += len(item["comp_data"].get("layers", []))

        ae.log_info(f"総アイテム数: {total_items}")
        ae.log_info(f"総レイヤー数: {total_layers}")
        ae.log_info("=" * 60)

        ae.alert(f"Export Complete!\n\nFile: {out_file}\nSize: {file_size/1024:.1f} KB\nItems: {total_items}, Layers: {total_layers}")

    except Exception as e:
        error_msg = f"Export Failed: {str(e)}"
        ae.log_error(error_msg)
        ae.log_error("=" * 60)
        ae.log_error("エラー詳細:")
        ae.log_error(traceback.format_exc())
        ae.log_error("=" * 60)
        ae.alert(error_msg)
