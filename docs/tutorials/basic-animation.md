# 基本アニメーションチュートリアル

このチュートリアルでは、PyAEを使って基本的なアニメーションを作成する方法を学びます。

## 目標

- コンポジションとレイヤーを作成
- キーフレームアニメーションを追加
- エフェクトを適用

## ステップ1: プロジェクトとコンポジションの準備

```python
import ae

# 現在のプロジェクトを取得
project = ae.Project.get_current()

# 1920x1080、30fps、10秒のコンポジションを作成
comp = project.create_comp(
    name="Basic Animation",
    width=1920,
    height=1080,
    pixel_aspect=1.0,
    duration=10.0,
    frame_rate=30.0
)

print(f"コンポジション作成: {comp.name}")
```

## ステップ2: レイヤーの追加

```python
# 背景用の平面レイヤーを追加
background = comp.add_solid("Background", 1920, 1080, 0.1, 0.1, 0.2)

# アニメーション対象のテキストレイヤーを追加
text_layer = comp.add_text("PyAE")
text_layer.name = "Animated Text"

# テキストを画面中央に配置
text_layer.position = (960, 540)

# テキストサイズを拡大
text_layer.scale = (200, 200)
```

## ステップ3: 位置アニメーション

```python
# 位置プロパティを取得
position_prop = text_layer.property("Transform/Position")

# 左端から右端へ移動するアニメーション
position_prop.add_keyframe(0.0, (100, 540))   # 開始位置（左端）
position_prop.add_keyframe(5.0, (1820, 540))  # 終了位置（右端）

# イージングを追加
position_prop.set_keyframe_interpolation(0, "ease_in", "ease_out")
position_prop.set_keyframe_interpolation(1, "ease_in", "ease_out")

print("位置アニメーション追加完了")
```

## ステップ4: 回転アニメーション

```python
# 回転プロパティを取得
rotation_prop = text_layer.property("Transform/Rotation")

# 2回転するアニメーション
rotation_prop.add_keyframe(0.0, 0.0)      # 開始角度
rotation_prop.add_keyframe(5.0, 720.0)    # 2回転 (360度 × 2)

print("回転アニメーション追加完了")
```

## ステップ5: スケールアニメーション

```python
# スケールプロパティを取得
scale_prop = text_layer.property("Transform/Scale")

# 拡大縮小アニメーション
scale_prop.add_keyframe(0.0, (50, 50))     # 小さい
scale_prop.add_keyframe(2.5, (300, 300))   # 大きい
scale_prop.add_keyframe(5.0, (200, 200))   # 中間

# スムーズなイージング
for i in range(3):
    scale_prop.set_keyframe_interpolation(i, "ease_in", "ease_out")

print("スケールアニメーション追加完了")
```

## ステップ6: 不透明度アニメーション

```python
# 不透明度プロパティを取得
opacity_prop = text_layer.property("Transform/Opacity")

# フェードイン・フェードアウト
opacity_prop.add_keyframe(0.0, 0.0)      # 透明
opacity_prop.add_keyframe(1.0, 100.0)    # 完全に表示
opacity_prop.add_keyframe(9.0, 100.0)    # 完全に表示を維持
opacity_prop.add_keyframe(10.0, 0.0)     # 透明にフェードアウト

print("不透明度アニメーション追加完了")
```

## ステップ7: エフェクトの追加

```python
# ブラーエフェクトを追加
blur = text_layer.add_effect("ADBE Gaussian Blur 2")

# ブラー量をアニメーション
blurriness = blur.property("Blurriness")
blurriness.add_keyframe(0.0, 0.0)
blurriness.add_keyframe(2.5, 50.0)
blurriness.add_keyframe(5.0, 0.0)

# グローエフェクトを追加
glow = text_layer.add_effect("ADBE Glo2")
glow.property("Glow Threshold").value = 50.0
glow.property("Glow Radius").value = 100.0
glow.property("Glow Intensity").value = 1.5

print("エフェクト追加完了")
```

## ステップ8: アンドゥグループでまとめる

すべての操作を1つのアンドゥグループにまとめることで、1回のアンドゥで全体を元に戻せます：

```python
import ae

with ae.UndoGroup("Create Basic Animation"):
    # すべてのステップをここに記述
    project = ae.Project.get_current()
    comp = project.create_comp("Basic Animation", 1920, 1080, duration=10.0)
    # ... 以下すべての操作

print("アニメーション作成完了！")
```

## 完全なコード

```python
import ae

with ae.UndoGroup("Create Basic Animation"):
    # プロジェクトとコンポジション
    project = ae.Project.get_current()
    comp = project.create_comp(
        name="Basic Animation",
        width=1920,
        height=1080,
        duration=10.0,
        frame_rate=30.0
    )

    # レイヤー追加
    background = comp.add_solid("Background", 1920, 1080, 0.1, 0.1, 0.2)
    text_layer = comp.add_text("PyAE")
    text_layer.name = "Animated Text"
    text_layer.position = (960, 540)
    text_layer.scale = (200, 200)

    # 位置アニメーション
    position = text_layer.property("Transform/Position")
    position.add_keyframe(0.0, (100, 540))
    position.add_keyframe(5.0, (1820, 540))

    # 回転アニメーション
    rotation = text_layer.property("Transform/Rotation")
    rotation.add_keyframe(0.0, 0.0)
    rotation.add_keyframe(5.0, 720.0)

    # スケールアニメーション
    scale = text_layer.property("Transform/Scale")
    scale.add_keyframe(0.0, (50, 50))
    scale.add_keyframe(2.5, (300, 300))
    scale.add_keyframe(5.0, (200, 200))

    # 不透明度アニメーション
    opacity = text_layer.property("Transform/Opacity")
    opacity.add_keyframe(0.0, 0.0)
    opacity.add_keyframe(1.0, 100.0)
    opacity.add_keyframe(9.0, 100.0)
    opacity.add_keyframe(10.0, 0.0)

    # エフェクト追加
    blur = text_layer.add_effect("ADBE Gaussian Blur 2")
    blur.property("Blurriness").add_keyframe(0.0, 0.0)
    blur.property("Blurriness").add_keyframe(2.5, 50.0)
    blur.property("Blurriness").add_keyframe(5.0, 0.0)

    glow = text_layer.add_effect("ADBE Glo2")
    glow.property("Glow Threshold").value = 50.0
    glow.property("Glow Radius").value = 100.0

print("基本アニメーション作成完了！")
```

## 次のステップ

- [レイヤー操作チュートリアル](layer-manipulation.md)
- [エフェクト自動化チュートリアル](effect-automation.md)
