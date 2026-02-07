# 親子関係循環参照テスト実行手順

## 前提条件

- PyAEプラグインがビルドされ、デプロイ済みであること
- After Effects 2025が起動していること

## テスト実行方法

### 1. After Effectsを起動

### 2. Python コンソールを開く

メニューから: `ウィンドウ > PyAE Console` を選択

### 3. 以下のコードを実行

```python
# テストスクリプトのパスを設定
exec(open(r"D:\works\VTechStudio\AfterFX\PyAE\scripts\run_test_parenting.py").read())
```

## 期待される結果

以下のテストがすべてPASSすること:

- `test_no_parent_initially` - 初期状態で親なし
- `test_set_parent` - 親レイヤーの設定
- `test_clear_parent` - 親レイヤーのクリア
- `test_parent_position_affects_child` - 親の位置が子に影響
- `test_multiple_children` - 複数の子レイヤー
- `test_parent_chain` - 親子チェーン
- **`test_cannot_parent_to_self`** - ✨ 自己参照エラー
- **`test_cannot_create_circular_reference_child_to_parent`** - ✨ 子→親の循環参照エラー
- **`test_cannot_create_circular_reference_grandchild`** - ✨ 孫→祖父の循環参照エラー
- **`test_cannot_create_circular_reference_ancestor`** - ✨ 複数階層の循環参照エラー
- `test_parent_by_index` - インデックスによる親の取得
- `test_parent_by_name` - 名前による親の取得
- `test_change_parent` - 親の変更

## テスト内容

新しく追加された循環参照テスト(✨マーク):

1. **自己参照の検出**: `layer.parent = layer` を実行すると、C++側でエラーを検出し、適切なエラーメッセージを返す

2. **子→親の循環参照検出**:
   - `child.parent = parent` で親子関係を設定
   - `parent.parent = child` を実行すると循環参照エラー

3. **孫→祖父の循環参照検出**:
   - `child.parent = parent`, `grandchild.parent = child` で階層を作成
   - `parent.parent = grandchild` を実行すると循環参照エラー

4. **複数階層の循環参照検出**:
   - `parent.parent = grandparent`, `child.parent = parent` で階層を作成
   - `grandparent.parent = child` を実行すると循環参照エラー

## トラブルシューティング

### テストが実行できない

- PyAEプラグインが正しくロードされているか確認
- Pythonコンソールでエラーが表示されていないか確認

### テストが失敗する

- エラーメッセージを確認
- 循環参照エラーが適切に検出されているか確認
- エラーメッセージに "circular", "descendant", "ancestor" などのキーワードが含まれているか確認
