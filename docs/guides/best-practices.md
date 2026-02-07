# ベストプラクティス

PyAEスクリプト作成時の推奨パターンを解説します。アンドゥグループによる操作のまとめ方、プロパティアクセスのキャッシュによるパフォーマンス向上、エラーハンドリング、型ヒントの活用など、保守性と実行効率を高めるコーディング手法を紹介します。

## アンドゥグループの使用

### 常にアンドゥグループを使用する

```python
# ✅ 良い例
import ae

with ae.UndoGroup("Create Animation"):
    comp = project.create_comp("MyComp", 1920, 1080)
    layer = comp.add_text("Hello")
    layer.position = (960, 540)

# ❌ 悪い例
comp = project.create_comp("MyComp", 1920, 1080)
layer = comp.add_text("Hello")
layer.position = (960, 540)
# → 3回のアンドゥ操作が必要
```

### 意味のある名前を付ける

```python
# ✅ 良い例
with ae.UndoGroup("Apply Glow Effect"):
    glow = layer.add_effect("ADBE Glo2")
    glow.property("Glow Radius").value = 100.0

# ❌ 悪い例
with ae.UndoGroup("Edit"):
    glow = layer.add_effect("ADBE Glo2")
```

## エラーハンドリング

### 適切な例外処理

```python
# ✅ 良い例
try:
    layer = comp.layers[0]
    layer.position = (960, 540)
except IndexError:
    print("レイヤーが存在しません")
except RuntimeError as e:
    print(f"After Effectsエラー: {e}")

# ❌ 悪い例
layer = comp.layers[0]  # IndexErrorが発生する可能性
```

### バリデーション

```python
# ✅ 良い例
def set_layer_position(layer, x, y):
    if not layer.valid:
        raise ValueError("無効なレイヤーです")

    if x < 0 or y < 0:
        raise ValueError("座標は0以上である必要があります")

    layer.position = (x, y)

# ❌ 悪い例
def set_layer_position(layer, x, y):
    layer.position = (x, y)  # バリデーションなし
```

## パフォーマンス最適化

### バッチ処理

```python
# ✅ 良い例
with ae.UndoGroup("Batch Update"):
    for layer in comp.layers:
        layer.opacity = 50.0

# ❌ 悪い例
for layer in comp.layers:
    with ae.UndoGroup("Update Opacity"):  # 毎回アンドゥグループを作成
        layer.opacity = 50.0
```

### プロパティアクセスのキャッシュ

```python
# ✅ 良い例
position_prop = layer.property("Transform/Position")
for i in range(100):
    position_prop.add_keyframe(i * 0.1, (i * 10, 540))

# ❌ 悪い例
for i in range(100):
    layer.property("Transform/Position").add_keyframe(i * 0.1, (i * 10, 540))
    # 毎回プロパティを検索
```

### 不要な操作を避ける

```python
# ✅ 良い例
if layer.opacity != 100.0:
    layer.opacity = 100.0

# ❌ 悪い例
layer.opacity = 100.0  # 既に100.0でも再設定
```

## コードの可読性

### 関数に分割

```python
# ✅ 良い例
def create_title_layer(comp, text, position):
    """タイトルレイヤーを作成"""
    layer = comp.add_text(text)
    layer.position = position
    apply_title_style(layer)
    return layer

def apply_title_style(layer):
    """タイトルスタイルを適用"""
    glow = layer.add_effect("ADBE Glo2")
    glow.property("Glow Radius").value = 100.0
    shadow = layer.add_effect("ADBE Drop Shadow")

# 使用
with ae.UndoGroup("Create Title"):
    title = create_title_layer(comp, "Hello", (960, 200))

# ❌ 悪い例
with ae.UndoGroup("Create Title"):
    layer = comp.add_text("Hello")
    layer.position = (960, 200)
    glow = layer.add_effect("ADBE Glo2")
    glow.property("Glow Radius").value = 100.0
    shadow = layer.add_effect("ADBE Drop Shadow")
    # すべてがベタ書き
```

### 定数を定義

```python
# ✅ 良い例
COMP_WIDTH = 1920
COMP_HEIGHT = 1080
COMP_FPS = 30.0
COMP_DURATION = 10.0

comp = project.create_comp(
    "MyComp",
    COMP_WIDTH,
    COMP_HEIGHT,
    frame_rate=COMP_FPS,
    duration=COMP_DURATION
)

# ❌ 悪い例
comp = project.create_comp("MyComp", 1920, 1080, frame_rate=30.0, duration=10.0)
# マジックナンバー
```

## 型ヒントの活用

```python
# ✅ 良い例
from typing import List, Optional
import ae

def get_text_layers(comp: ae.Comp) -> List[ae.Layer]:
    """テキストレイヤーのリストを取得"""
    return [layer for layer in comp.layers if "text" in layer.name.lower()]

def find_layer_by_name(comp: ae.Comp, name: str) -> Optional[ae.Layer]:
    """名前でレイヤーを検索"""
    for layer in comp.layers:
        if layer.name == name:
            return layer
    return None

# ❌ 悪い例
def get_text_layers(comp):  # 型ヒントなし
    return [layer for layer in comp.layers if "text" in layer.name.lower()]
```

## リソース管理

### 有効性チェック

```python
# ✅ 良い例
def safe_delete_layer(layer):
    if layer.valid:
        layer.delete()
    else:
        print("レイヤーは既に削除されています")

# ❌ 悪い例
def delete_layer(layer):
    layer.delete()  # 既に削除されている場合エラー
```

### コンテキストマネージャーの活用

```python
# ✅ 良い例
with ae.UndoGroup("Complex Operation"):
    try:
        # 複雑な操作
        create_complex_animation(comp)
    except Exception as e:
        print(f"エラー: {e}")
        # アンドゥグループは自動的にクローズ

# ❌ 悪い例
ae.begin_undo_group("Complex Operation")
try:
    create_complex_animation(comp)
finally:
    ae.end_undo_group()  # 手動管理
```

## デバッグ

### ログ出力

```python
# ✅ 良い例
import ae

ae.log_info(f"コンポジション作成: {comp.name}")
ae.log_debug(f"レイヤー数: {comp.num_layers}")

try:
    layer.position = (960, 540)
    ae.log_info("位置設定成功")
except Exception as e:
    ae.log_error(f"位置設定失敗: {e}")

# ❌ 悪い例
print(f"コンポジション作成: {comp.name}")  # ログに残らない
```

### アサーション

```python
# ✅ 良い例
def add_keyframe_animation(layer, start_pos, end_pos, duration):
    assert layer.valid, "レイヤーが無効です"
    assert duration > 0, "デュレーションは0より大きい必要があります"

    position = layer.property("Transform/Position")
    position.add_keyframe(0.0, start_pos)
    position.add_keyframe(duration, end_pos)

# ❌ 悪い例
def add_keyframe_animation(layer, start_pos, end_pos, duration):
    # バリデーションなし
    position = layer.property("Transform/Position")
    position.add_keyframe(0.0, start_pos)
    position.add_keyframe(duration, end_pos)
```

## テストとデバッグ

### 小さく始める

```python
# ✅ 良い例
# まず1つのレイヤーでテスト
layer = comp.layers[0]
apply_effect(layer)

# 動作確認後、全レイヤーに適用
for layer in comp.layers:
    apply_effect(layer)

# ❌ 悪い例
# いきなり全レイヤーに適用
for layer in comp.layers:
    apply_effect(layer)  # 問題があると全レイヤーで失敗
```

### スクリプトをモジュール化

```python
# ✅ 良い例: my_tools.py
"""PyAE用カスタムツール"""
import ae

def create_standard_comp(name):
    """標準コンポジションを作成"""
    return ae.Project.get_current().create_comp(
        name, 1920, 1080, duration=10.0, frame_rate=30.0
    )

def apply_standard_glow(layer):
    """標準グローを適用"""
    glow = layer.add_effect("ADBE Glo2")
    glow.property("Glow Threshold").value = 50.0
    glow.property("Glow Radius").value = 100.0

# 使用
import my_tools
comp = my_tools.create_standard_comp("Test")

# ❌ 悪い例: すべてをスクリプトファイルに記述
```

## まとめ

- **アンドゥグループを常に使用**
- **適切なエラーハンドリング**
- **バッチ処理でパフォーマンス向上**
- **コードを関数に分割**
- **型ヒントを活用**
- **ログ出力でデバッグ**

## 関連項目

- [高レベルAPI](../api/high-level/index.rst) - Python APIリファレンス
- [アーキテクチャ](architecture.md) - PyAEの内部設計（プラグイン開発者向け）
