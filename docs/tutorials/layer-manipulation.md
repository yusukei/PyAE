# レイヤー操作チュートリアル

このチュートリアルでは、PyAEを使ってレイヤーを操作する様々な方法を学びます。

## 目標

- レイヤーの作成と複製
- レイヤーの並び替えと削除
- 親子関係の設定
- レイヤープロパティの一括変更

## 準備

```python
import ae

# コンポジションを作成
project = ae.Project.get_current()
comp = project.create_comp("Layer Manipulation", 1920, 1080, duration=10.0)
```

## 1. 複数レイヤーの作成

```python
# 異なる色の平面レイヤーを5つ作成
colors = [
    ([1.0, 0.0, 0.0], "Red"),
    ([0.0, 1.0, 0.0], "Green"),
    ([0.0, 0.0, 1.0], "Blue"),
    ([1.0, 1.0, 0.0], "Yellow"),
    ([1.0, 0.0, 1.0], "Magenta"),
]

layers = []
for i, (color, name) in enumerate(colors):
    layer = comp.add_solid(name, 400, 400, color[0], color[1], color[2])
    # 横に並べて配置
    layer.position = (300 + i * 350, 540)
    layers.append(layer)

print(f"{len(layers)}個のレイヤーを作成しました")
```

## 2. レイヤーの並び替え

```python
# 最初のレイヤーを最前面に移動
layers[0].move_to(1)

# 最後のレイヤーを最背面に移動
layers[-1].index = comp.num_layers

# インデックスでソート（降順）
print("レイヤー順序:")
for layer in comp.layers:
    print(f"  {layer.index}: {layer.name}")
```

## 3. レイヤーの複製

```python
# Redレイヤーを複製
red_layer = layers[0]
duplicated = red_layer.duplicate()
duplicated.name = "Red Copy"
duplicated.position = (960, 200)

# 複数回複製して円形に配置
import math

num_copies = 8
radius = 400
center = (960, 540)

for i in range(num_copies):
    copy = red_layer.duplicate()
    copy.name = f"Copy {i+1}"

    # 円形配置
    angle = (2 * math.pi / num_copies) * i
    x = center[0] + radius * math.cos(angle)
    y = center[1] + radius * math.sin(angle)
    copy.position = (x, y)

    # 回転
    copy.rotation = math.degrees(angle)

print(f"{num_copies}個の複製を作成しました")
```

## 4. 親子関係の設定

```python
# ヌルオブジェクトをコントローラーとして作成
controller = comp.add_null("Controller")
controller.position = (960, 540)

# すべてのレイヤーをコントローラーの子にする
for layer in comp.layers:
    if layer != controller and "Copy" in layer.name:
        layer.parent = controller

# コントローラーを回転させると、すべての子レイヤーも回転する
rotation_prop = controller.property("Transform/Rotation")
rotation_prop.add_keyframe(0.0, 0.0)
rotation_prop.add_keyframe(10.0, 360.0)

print("親子関係を設定しました")
```

## 5. レイヤープロパティの一括変更

```python
# 特定の名前パターンに一致するレイヤーを検索
def find_layers_by_pattern(comp, pattern):
    return [layer for layer in comp.layers if pattern in layer.name]

# "Copy"を含むすべてのレイヤーを取得
copy_layers = find_layers_by_pattern(comp, "Copy")

# プロパティを一括変更
for layer in copy_layers:
    # スケールを変更
    layer.scale = (150, 150)

    # 不透明度をアニメーション
    opacity = layer.property("Transform/Opacity")
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(2.0, 100.0)

print(f"{len(copy_layers)}個のレイヤーのプロパティを変更しました")
```

## 6. レイヤーフラグの一括設定

```python
# すべてのレイヤーに対してシャイフラグを設定
for layer in comp.layers:
    if "Copy" in layer.name:
        layer.shy = True

# 選択レイヤーをロック
selected = comp.selected_layers
for layer in selected:
    layer.locked = True
    print(f"{layer.name} をロックしました")
```

## 7. 条件付きレイヤー削除

```python
# 名前に"Temp"を含むレイヤーを削除
layers_to_delete = find_layers_by_pattern(comp, "Temp")
for layer in layers_to_delete:
    print(f"{layer.name} を削除します")
    layer.delete()

# 不透明度が0のレイヤーを削除
for layer in list(comp.layers):  # リストのコピーでイテレート
    if layer.opacity == 0.0:
        print(f"{layer.name} (不透明度0) を削除します")
        layer.delete()
```

## 8. レイヤーの時間オフセット

```python
# レイヤーを順番に表示する（カスケード効果）
delay = 0.5  # 0.5秒ごとに表示

for i, layer in enumerate(comp.layers):
    # インポイントをずらす
    layer.in_point = i * delay

    # フェードイン
    opacity = layer.property("Transform/Opacity")
    opacity.add_keyframe(layer.in_point, 0.0)
    opacity.add_keyframe(layer.in_point + 0.5, 100.0)

print("カスケード効果を追加しました")
```

## 完全な例: グリッドレイアウト

```python
import ae

with ae.UndoGroup("Create Grid Layout"):
    project = ae.Project.get_current()
    comp = project.create_comp("Grid Layout", 1920, 1080, duration=10.0)

    # グリッドパラメータ
    rows = 4
    cols = 5
    cell_width = 200
    cell_height = 200
    spacing = 50

    # 開始位置（中央揃え）
    grid_width = cols * cell_width + (cols - 1) * spacing
    grid_height = rows * cell_height + (rows - 1) * spacing
    start_x = (1920 - grid_width) / 2 + cell_width / 2
    start_y = (1080 - grid_height) / 2 + cell_height / 2

    # グリッドレイヤーを作成
    import random

    for row in range(rows):
        for col in range(cols):
            # ランダムな色
            color = [random.random(), random.random(), random.random()]

            # レイヤー作成
            layer = comp.add_solid(f"Cell_{row}_{col}", cell_width, cell_height, color[0], color[1], color[2])

            # 位置
            x = start_x + col * (cell_width + spacing)
            y = start_y + row * (cell_height + spacing)
            layer.position = (x, y)

            # アニメーション（ランダムな遅延）
            delay = random.uniform(0, 3)
            opacity = layer.property("Transform/Opacity")
            opacity.add_keyframe(delay, 0.0)
            opacity.add_keyframe(delay + 1.0, 100.0)

    print(f"{rows * cols}個のグリッドレイヤーを作成しました")
```

## 次のステップ

- [エフェクト自動化チュートリアル](effect-automation.md)
- [API リファレンス - Layer](../api/high-level/layer)
