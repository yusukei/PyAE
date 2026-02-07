# フッテージ管理チュートリアル

このチュートリアルでは、PyAEを使ってフッテージ（素材ファイル）を管理する方法を学びます。ファイルパスの確認・変更、フッテージの置換、バッチ処理などを解説します。

## 目標

- プロジェクト内のフッテージを取得・一覧表示
- フッテージのファイルパスを確認・変更
- 欠落フッテージの検出と修復
- 複数フッテージの一括処理

## ステップ1: プロジェクト内のフッテージを取得

```python
import ae

# 現在のプロジェクトを取得
project = ae.Project.get_current()

# プロジェクト内のすべてのアイテムを取得
items = project.items

# フッテージアイテムのみをフィルタリング
footage_items = [item for item in items if item.type == ae.ItemType.Footage]

print(f"フッテージ数: {len(footage_items)}")
for footage in footage_items:
    print(f"  - {footage.name}")
```

## ステップ2: フッテージの詳細情報を取得

```python
# フッテージの詳細を表示
for footage in footage_items:
    print(f"\n=== {footage.name} ===")
    print(f"  タイプ: {footage.type}")
    print(f"  幅: {footage.width}")
    print(f"  高さ: {footage.height}")
    print(f"  デュレーション: {footage.duration}秒")
    print(f"  フレームレート: {footage.frame_rate}")

    # ファイルパスを取得（ファイルベースのフッテージの場合）
    if hasattr(footage, 'path') and footage.path:
        print(f"  パス: {footage.path}")
```

## ステップ3: ファイルパスの確認と変更

```python
import ae
import os

# 特定のフッテージを取得
footage = project.item_by_name("my_video.mp4")

if footage:
    # 現在のパスを確認
    current_path = footage.path
    print(f"現在のパス: {current_path}")

    # ファイルが存在するか確認
    if os.path.exists(current_path):
        print("ファイルは存在します")
    else:
        print("ファイルが見つかりません（欠落）")

    # パスを変更（新しいフッテージをインポートして置換）
    new_path = r"D:\projects\assets\new_video.mp4"
    if os.path.exists(new_path):
        # 新しいフッテージをインポート
        new_footage = ae.Footage.from_file(new_path)
        new_footage.add_to_project()
        new_footage.name = footage.name + "_new"
        print(f"新しいフッテージをインポートしました: {new_footage.name}")
        # 注意: 既存のフッテージを使用しているレイヤーは手動で更新が必要です
```

## ステップ4: 欠落フッテージの検出

```python
def find_missing_footage(project):
    """プロジェクト内の欠落フッテージを検出"""
    missing = []

    for item in project.items:
        if item.type != ae.ItemType.Footage:
            continue

        # ファイルパスがあるフッテージのみチェック
        if hasattr(item, 'path') and item.path:
            if not os.path.exists(item.path):
                missing.append({
                    'name': item.name,
                    'path': item.path
                })

    return missing

# 欠落フッテージを検出
missing_footage = find_missing_footage(project)

if missing_footage:
    print(f"\n欠落フッテージが {len(missing_footage)} 件見つかりました:")
    for item in missing_footage:
        print(f"  - {item['name']}")
        print(f"    パス: {item['path']}")
else:
    print("欠落フッテージはありません")
```

## ステップ5: フッテージパスの一括変更

プロジェクトのフォルダを移動した場合など、複数のフッテージパスを一括で変更する例です：

```python
def relink_footage_by_prefix(project, old_prefix, new_prefix):
    """フッテージパスのプレフィックスに基づいて再リンク情報を表示

    注意: PyAEでは直接的なフッテージ置換APIは提供されていません。
    この関数は再リンクが必要なフッテージを特定し、情報を表示します。
    """
    needs_relink = []

    for item in project.items:
        if item.type != ae.ItemType.Footage:
            continue

        if not hasattr(item, 'path') or not item.path:
            continue

        current_path = item.path

        # パスがold_prefixで始まる場合
        if current_path.startswith(old_prefix):
            new_path = current_path.replace(old_prefix, new_prefix, 1)

            needs_relink.append({
                'name': item.name,
                'old_path': current_path,
                'new_path': new_path,
                'new_exists': os.path.exists(new_path)
            })

    return needs_relink

# 使用例: D:\old_project から E:\new_project にパスを変更
old_prefix = r"D:\old_project\assets"
new_prefix = r"E:\new_project\assets"

relink_info = relink_footage_by_prefix(project, old_prefix, new_prefix)
print(f"\n{len(relink_info)} 件のフッテージの再リンクが必要です:")
for info in relink_info:
    status = "✓" if info['new_exists'] else "✗"
    print(f"  [{status}] {info['name']}")
    print(f"      旧: {info['old_path']}")
    print(f"      新: {info['new_path']}")
```

## ステップ6: フッテージのインポート

```python
def import_footage_file(project, file_path, folder=None):
    """フッテージファイルをインポート"""
    if not os.path.exists(file_path):
        print(f"ファイルが見つかりません: {file_path}")
        return None

    with ae.UndoGroup("Import Footage"):
        footage = project.import_file(file_path)

        # フォルダが指定されていれば移動
        if folder:
            footage.parent_folder = folder

        print(f"インポート完了: {footage.name}")
        return footage

# 使用例
new_footage = import_footage_file(project, r"D:\assets\background.png")
```

## ステップ7: フォルダ内のファイルを一括インポート

```python
def import_folder(project, folder_path, extensions=None):
    """フォルダ内のファイルを一括インポート"""
    if extensions is None:
        extensions = ['.png', '.jpg', '.jpeg', '.mp4', '.mov', '.wav', '.mp3']

    imported = []

    with ae.UndoGroup("Import Folder"):
        # プロジェクト内にフォルダを作成
        folder_name = os.path.basename(folder_path)
        project_folder = project.create_folder(folder_name)

        # フォルダ内のファイルを走査
        for filename in os.listdir(folder_path):
            ext = os.path.splitext(filename)[1].lower()

            if ext in extensions:
                file_path = os.path.join(folder_path, filename)
                footage = project.import_file(file_path)
                footage.parent_folder = project_folder
                imported.append(footage)
                print(f"インポート: {filename}")

    print(f"\n合計 {len(imported)} ファイルをインポートしました")
    return imported

# 使用例
imported_files = import_folder(project, r"D:\assets\images", ['.png', '.jpg'])
```

## ステップ8: フッテージの使用状況を確認

```python
def find_footage_usage(project, footage):
    """フッテージがどのコンポジションで使用されているか確認"""
    usage = []

    # フッテージのアイテムIDを取得
    footage_id = footage.item_id

    for item in project.items:
        if item.type != ae.ItemType.Comp:
            continue

        comp = item
        for layer in comp.layers:
            # レイヤーのソースアイテムIDと比較
            if hasattr(layer, 'source_item_id') and layer.source_item_id == footage_id:
                usage.append({
                    'comp': comp.name,
                    'layer': layer.name
                })

    return usage

# 特定のフッテージの使用状況を確認
footage = project.item_by_name("background.png")
if footage:
    usage = find_footage_usage(project, footage)

    if usage:
        print(f"'{footage.name}' の使用箇所:")
        for u in usage:
            print(f"  - {u['comp']} > {u['layer']}")
    else:
        print(f"'{footage.name}' は使用されていません")
```

## 完全なコード: フッテージ管理ユーティリティ

```python
import ae
import os

class FootageManager:
    """フッテージ管理ユーティリティクラス"""

    def __init__(self):
        self.project = ae.Project.get_current()

    def list_all(self):
        """すべてのフッテージを一覧表示"""
        footage_items = [item for item in self.project.items if item.type == ae.ItemType.Footage]

        for footage in footage_items:
            status = "OK" if self._file_exists(footage) else "MISSING"
            print(f"[{status}] {footage.name}")
            if hasattr(footage, 'path') and footage.path:
                print(f"       {footage.path}")

        return footage_items

    def find_missing(self):
        """欠落フッテージを検出"""
        missing = []
        for item in self.project.items:
            if item.type == ae.ItemType.Footage and not self._file_exists(item):
                missing.append(item)
        return missing

    def check_relink(self, footage, new_path):
        """フッテージの再リンク可能性をチェック

        注意: PyAEでは直接的なフッテージ置換APIは提供されていません。
        この関数は再リンクの可否を確認します。
        """
        if os.path.exists(new_path):
            print(f"再リンク可能: {footage.name}")
            print(f"  新パス: {new_path}")
            return True
        return False

    def find_relink_candidates(self, old_prefix, new_prefix):
        """パスプレフィックスに基づいて再リンク候補を検索

        注意: PyAEでは直接的なフッテージ置換APIは提供されていません。
        この関数は再リンクが必要なフッテージのリストを返します。
        """
        candidates = []
        for item in self.project.items:
            if item.type != ae.ItemType.Footage:
                continue
            if hasattr(item, 'path') and item.path:
                if item.path.startswith(old_prefix):
                    new_path = item.path.replace(old_prefix, new_prefix, 1)
                    candidates.append({
                        'item': item,
                        'old_path': item.path,
                        'new_path': new_path,
                        'exists': os.path.exists(new_path)
                    })
        return candidates

    def _file_exists(self, footage):
        """フッテージファイルが存在するか確認"""
        if hasattr(footage, 'path') and footage.path:
            return os.path.exists(footage.path)
        return True  # パスがない場合（Solidなど）はOKとみなす

# 使用例
manager = FootageManager()

print("=== フッテージ一覧 ===")
manager.list_all()

print("\n=== 欠落フッテージ ===")
missing = manager.find_missing()
for m in missing:
    print(f"  - {m.name}: {m.path}")

print("\n=== 再リンク候補 ===")
candidates = manager.find_relink_candidates(
    r"D:\old_project",
    r"E:\new_project"
)
for c in candidates:
    status = "✓" if c['exists'] else "✗"
    print(f"  [{status}] {c['item'].name}")
print(f"{len(candidates)} 件の再リンク候補が見つかりました")
```

## 次のステップ

- [基本アニメーションチュートリアル](basic-animation.md)
- [レイヤー操作チュートリアル](layer-manipulation.md)
- [エフェクト自動化チュートリアル](effect-automation.md)
- [ベストプラクティス](../guides/best-practices.md)
