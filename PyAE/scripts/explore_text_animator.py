# explore_text_animator.py
# Phase 9: Text Animator 構造調査スクリプト
# After Effects で実行して、Text Animator の正確な構造を確認する

import ae
import traceback
import os
import json

# 結果を保存するリスト
exploration_results = []


def log(msg):
    """ログ出力"""
    ae.log_info(f"[EXPLORE] {msg}")
    exploration_results.append(msg)


def explore_property(prop, indent=0):
    """プロパティを再帰的に探索"""
    prefix = "  " * indent
    try:
        name = prop.name if prop.name else "(no name)"
        match_name = prop.match_name
        is_group = prop.is_group
        value_type = str(prop.stream_value_type) if hasattr(prop, 'stream_value_type') else "N/A"
        grouping_type = str(prop.grouping_type) if hasattr(prop, 'grouping_type') else "N/A"

        log(f"{prefix}[{name}]")
        log(f"{prefix}  match_name: {match_name}")
        log(f"{prefix}  is_group: {is_group}")
        log(f"{prefix}  grouping_type: {grouping_type}")
        log(f"{prefix}  stream_value_type: {value_type}")

        if not is_group:
            # 葉プロパティの場合、値を取得
            try:
                value = prop.value
                log(f"{prefix}  value: {value}")
            except Exception as e:
                log(f"{prefix}  value: (error: {e})")

            # can_add_stream を確認
            try:
                if hasattr(prop, 'can_add_stream'):
                    log(f"{prefix}  can_add_stream available: True")
            except:
                pass
        else:
            # グループの場合、子プロパティを探索
            num_props = prop.num_properties
            log(f"{prefix}  num_properties: {num_props}")

            # can_add_stream を確認（INDEXED_GROUP用）
            if hasattr(prop, 'can_add_stream'):
                try:
                    # いくつかの候補 match_name を試す
                    candidates = [
                        "ADBE Text Animator",
                        "ADBE Text Selector",
                        "ADBE Text Range Selector",
                        "ADBE Text Range Selector2",
                        "ADBE Text Wiggly Selector",
                        "ADBE Text Expression Selector"
                    ]
                    for candidate in candidates:
                        can_add = prop.can_add_stream(candidate)
                        if can_add:
                            log(f"{prefix}  can_add_stream('{candidate}'): {can_add}")
                except Exception as e:
                    pass

            for i in range(num_props):
                try:
                    child = prop.property(i)
                    if child:
                        explore_property(child, indent + 1)
                except Exception as e:
                    log(f"{prefix}  child[{i}] error: {e}")

    except Exception as e:
        log(f"{prefix}ERROR: {e}")


def create_test_text_layer():
    """テスト用テキストレイヤーを作成し、Animator を追加"""
    log("=" * 60)
    log("Phase 9: Text Animator 構造調査")
    log("=" * 60)

    project = ae.get_project()
    if not project:
        log("ERROR: プロジェクトが開いていません")
        return None

    # テスト用コンポジション作成
    comp = project.create_comp("Text_Animator_Test", 1920, 1080, 1.0, 10.0, 30.0)
    log(f"テストコンポ作成: {comp.name}")

    # テキストレイヤー作成
    text_layer = comp.add_text("Test Text")
    log(f"テキストレイヤー作成: {text_layer.name}")

    return text_layer


def explore_text_layer(text_layer):
    """テキストレイヤーの構造を探索"""
    log("=" * 60)
    log("テキストレイヤー構造探索")
    log("=" * 60)

    try:
        root = text_layer.properties
        if not root:
            log("ERROR: プロパティルートが取得できません")
            return

        # ADBE Text Properties を探索
        text_props = root.property("ADBE Text Properties")
        if text_props:
            log("ADBE Text Properties 発見")
            explore_property(text_props, 0)
        else:
            log("ERROR: ADBE Text Properties が見つかりません")

    except Exception as e:
        log(f"ERROR: {e}")
        log(traceback.format_exc())


def test_add_animator(text_layer):
    """Animator 追加をテスト"""
    log("=" * 60)
    log("Animator 追加テスト")
    log("=" * 60)

    try:
        root = text_layer.properties
        text_props = root.property("ADBE Text Properties")
        if not text_props:
            log("ERROR: ADBE Text Properties が見つかりません")
            return

        # ADBE Text Animators グループを取得
        animators_group = text_props.property("ADBE Text Animators")
        if not animators_group:
            log("ERROR: ADBE Text Animators が見つかりません")
            return

        log(f"ADBE Text Animators 発見")
        log(f"  is_group: {animators_group.is_group}")
        log(f"  grouping_type: {animators_group.grouping_type}")
        log(f"  num_properties: {animators_group.num_properties}")

        # Animator 追加を試みる
        log("\nAnimator 追加を試行...")

        # can_add_stream で追加可能か確認
        if hasattr(animators_group, 'can_add_stream'):
            can_add = animators_group.can_add_stream("ADBE Text Animator")
            log(f"  can_add_stream('ADBE Text Animator'): {can_add}")

            if can_add:
                log("  add_stream を実行...")
                new_animator = animators_group.add_stream("ADBE Text Animator")
                if new_animator:
                    log(f"  Animator 追加成功!")
                    log(f"    name: {new_animator.name}")
                    log(f"    match_name: {new_animator.match_name}")

                    # 追加された Animator の構造を探索
                    log("\n追加された Animator の構造:")
                    explore_property(new_animator, 1)

                    # Animator Properties を探索
                    anim_props = new_animator.property("ADBE Text Animator Properties")
                    if anim_props:
                        log("\n  ADBE Text Animator Properties 発見")

                        # Position を追加してみる
                        if hasattr(anim_props, 'can_add_stream'):
                            for prop_name in ["ADBE Text Animator Position", "ADBE Text Position 3D"]:
                                can_add_pos = anim_props.can_add_stream(prop_name)
                                log(f"    can_add_stream('{prop_name}'): {can_add_pos}")

                                if can_add_pos:
                                    pos_prop = anim_props.add_stream(prop_name)
                                    if pos_prop:
                                        log(f"    Position 追加成功!")
                                        log(f"      name: {pos_prop.name}")
                                        log(f"      match_name: {pos_prop.match_name}")

                                        # 値を設定
                                        try:
                                            pos_prop.value = [100.0, 50.0]
                                            log(f"      value set: [100.0, 50.0]")
                                            log(f"      value get: {pos_prop.value}")
                                        except Exception as e:
                                            log(f"      value set error: {e}")
                                    break
                else:
                    log("  ERROR: Animator 追加失敗")
            else:
                log("  can_add_stream が False - 追加不可")
        else:
            log("  can_add_stream メソッドがありません")

    except Exception as e:
        log(f"ERROR: {e}")
        log(traceback.format_exc())


def run_exploration():
    """構造調査を実行"""
    ae.begin_undo_group("Text Animator Exploration")

    try:
        # テキストレイヤー作成
        text_layer = create_test_text_layer()
        if not text_layer:
            return

        # 初期状態の構造を探索
        log("\n--- 初期状態の構造 ---")
        explore_text_layer(text_layer)

        # Animator 追加をテスト
        log("\n--- Animator 追加テスト ---")
        test_add_animator(text_layer)

        # 追加後の構造を探索
        log("\n--- Animator 追加後の構造 ---")
        explore_text_layer(text_layer)

    except Exception as e:
        log(f"ERROR: {e}")
        log(traceback.format_exc())

    finally:
        ae.end_undo_group()

    log("=" * 60)
    log("構造調査完了")
    log("=" * 60)

    # 結果をファイルに保存
    docs_path = ae.get_documents_folder()
    result_file = os.path.join(docs_path, "text_animator_structure.txt")
    with open(result_file, "w", encoding="utf-8") as f:
        f.write("\n".join(exploration_results))
    log(f"結果保存: {result_file}")

    return "OK"


if __name__ == "__main__":
    run_exploration()
