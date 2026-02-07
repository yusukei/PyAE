# クイックスタート

PyAEの基本的な使い方を学びましょう。

## 最初のスクリプト

### プロジェクトとコンポジションの作成

```python
import ae

# 現在のプロジェクトを取得
project = ae.Project.get_current()

# 新しいコンポジションを作成
comp = project.create_comp(
    name="MyFirstComp",
    width=1920,
    height=1080,
    pixel_aspect=1.0,
    duration=10.0,  # 10秒
    frame_rate=30.0
)

print(f"作成したコンポジション: {comp.name}")
```

### レイヤーの追加

```python
# 平面レイヤーを追加
red_layer = comp.add_solid("Red Background", 1920, 1080, (1.0, 0.0, 0.0))

# テキストレイヤーを追加
text_layer = comp.add_text("Hello PyAE!")
text_layer.position = (960, 540)  # 中央に配置
text_layer.scale = (200, 200)     # 2倍に拡大
```

### プロパティのアニメーション

```python
# 位置アニメーションを作成
text_layer.property("Transform/Position").add_keyframe(0.0, (100, 540))
text_layer.property("Transform/Position").add_keyframe(5.0, (1820, 540))

# 不透明度アニメーション
opacity_prop = text_layer.property("Transform/Opacity")
opacity_prop.add_keyframe(0.0, 0.0)
opacity_prop.add_keyframe(1.0, 100.0)
opacity_prop.add_keyframe(9.0, 100.0)
opacity_prop.add_keyframe(10.0, 0.0)
```

### エフェクトの追加

```python
# ブラーエフェクトを追加
blur = text_layer.add_effect("ADBE Gaussian Blur 2")
blur.property("Blurriness").value = 20.0

# グローエフェクトを追加
glow = text_layer.add_effect("ADBE Glo2")
glow.property("Glow Threshold").value = 50.0
glow.property("Glow Radius").value = 100.0
```

## アンドゥグループの使用

複数の操作を1つのアンドゥ単位にまとめることができます：

```python
import ae

# アンドゥグループを使用
with ae.UndoGroup("Create Animation"):
    comp = project.create_comp("AnimComp", 1920, 1080)
    layer = comp.add_text("Animated Text")
    layer.property("Transform/Position").add_keyframe(0.0, (100, 540))
    layer.property("Transform/Position").add_keyframe(3.0, (1820, 540))

# すべての操作が1回のアンドゥで取り消せる
```

## ファイルのインポート

```python
import ae

project = ae.Project.get_current()

# 画像ファイルをインポート
footage = project.import_file("C:/path/to/image.png")

# コンポジションに追加
comp = ae.get_active_comp()
layer = comp.add_layer(footage)
layer.position = (960, 540)
```

## レンダリング

```python
import ae

comp = ae.get_active_comp()

# レンダリングオプションを作成
options = ae.RenderOptions.from_item(comp._handle)
options.time = 0.0  # 0秒の位置
options.world_type = ae.WorldType.BIT8

# フレームをレンダリング
with ae.Renderer.render_frame(options) as receipt:
    if receipt and receipt.valid:
        world = receipt.world
        print(f"Rendered: {world.width}x{world.height}")
```

## 次のステップ

- [基本アニメーションチュートリアル](../tutorials/basic-animation.md)
- [レイヤー操作チュートリアル](../tutorials/layer-manipulation.md)
- [API リファレンス](../api/high-level/index)
