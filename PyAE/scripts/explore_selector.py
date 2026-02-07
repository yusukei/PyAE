# explore_selector.py
# Range Selector の詳細構造を調査

import ae
import os

# ログファイル
log_path = os.path.join(ae.get_documents_folder(), "selector_structure_detail.txt")
log_file = open(log_path, "w", encoding="utf-8")

def log(msg):
    ae.log_info(f"[EXPLORE] {msg}")
    log_file.write(msg + "\n")
    log_file.flush()

def explore_property(prop, indent=0):
    """プロパティを再帰的に探索"""
    prefix = "  " * indent
    try:
        log(f"{prefix}[{prop.name}]")
        log(f"{prefix}  match_name: {prop.match_name}")
        log(f"{prefix}  is_group: {prop.is_group}")

        if not prop.is_group:
            try:
                log(f"{prefix}  value: {prop.value}")
            except:
                log(f"{prefix}  value: (cannot read)")

        if prop.is_group:
            log(f"{prefix}  num_properties: {prop.num_properties}")
            for i in range(prop.num_properties):
                try:
                    child = prop.property(i)
                    if child:
                        explore_property(child, indent + 1)
                except Exception as e:
                    log(f"{prefix}  Error getting property {i}: {e}")
    except Exception as e:
        log(f"{prefix}Error exploring: {e}")

def run():
    log("=== Range Selector 構造調査 ===")

    project = ae.get_project()
    if not project:
        log("プロジェクトが開いていません")
        return "ERROR"

    # テスト用コンポジション作成
    comp = project.create_comp("Selector_Test", 1920, 1080, 1.0, 5.0, 30.0)
    text_layer = comp.add_text("Test")
    log(f"テキストレイヤー作成: {text_layer.name}")

    root = text_layer.properties
    text_props = root.property("ADBE Text Properties")
    animators_group = text_props.property("ADBE Text Animators")

    # Animator を追加
    animator = animators_group.add_stream("ADBE Text Animator")
    log(f"Animator追加: {animator.name}")

    # Selectors グループを取得
    selectors_group = animator.property("ADBE Text Selectors")
    log(f"Selectors数: {selectors_group.num_properties}")

    # Selector を追加
    if selectors_group.num_properties == 0:
        if selectors_group.can_add_stream("ADBE Text Selector"):
            selector = selectors_group.add_stream("ADBE Text Selector")
            log(f"Selector追加: {selector.name}")

    # 再度確認
    log(f"Selectors数 (追加後): {selectors_group.num_properties}")

    if selectors_group.num_properties > 0:
        selector = selectors_group.property(0)
        log(f"\n=== Selector構造 ===")
        explore_property(selector)

    # コンポを削除
    try:
        for item in project.items:
            if hasattr(item, 'name') and item.name == "Selector_Test":
                item.delete()
                break
    except:
        pass

    log("=== 調査完了 ===")
    log_file.close()
    return "OK"

if __name__ == "__main__":
    run()
