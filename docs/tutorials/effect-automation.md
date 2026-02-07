# エフェクト自動化チュートリアル

このチュートリアルでは、PyAEを使ってエフェクトを自動的に追加し、パラメータをコントロールする方法を学びます。

## 目標

- 複数レイヤーへのエフェクト一括追加
- エフェクトパラメータの自動設定
- エフェクトのアニメーション
- カスタムエフェクトプリセットの作成

## 準備

```python
import ae

project = ae.Project.get_current()
comp = project.create_comp("Effect Automation", 1920, 1080, duration=10.0)

# テスト用レイヤーを作成
layer = comp.add_solid("Base Layer", 1920, 1080, (0.5, 0.5, 0.5))
```

## 1. 基本的なエフェクト追加

```python
# Gaussian Blur
blur = layer.add_effect("ADBE Gaussian Blur 2")
blur.property("Blurriness").value = 30.0

# Glow
glow = layer.add_effect("ADBE Glo2")
glow.property("Glow Threshold").value = 50.0
glow.property("Glow Radius").value = 100.0
glow.property("Glow Intensity").value = 1.5

# Color Correction
levels = layer.add_effect("ADBE Pro Levels2")
levels.property("Gamma").value = 1.2

print(f"{layer.num_effects}個のエフェクトを追加しました")
```

## 2. エフェクトのプリセット作成

```python
def apply_glow_preset(layer, intensity="medium"):
    """グロー効果のプリセットを適用"""
    glow = layer.add_effect("ADBE Glo2")

    presets = {
        "soft": {"threshold": 70, "radius": 50, "intensity": 0.5},
        "medium": {"threshold": 50, "radius": 100, "intensity": 1.0},
        "strong": {"threshold": 30, "radius": 150, "intensity": 2.0},
    }

    params = presets.get(intensity, presets["medium"])
    glow.property("Glow Threshold").value = params["threshold"]
    glow.property("Glow Radius").value = params["radius"]
    glow.property("Glow Intensity").value = params["intensity"]

    return glow

# プリセットを使用
glow = apply_glow_preset(layer, "strong")
print("強いグロー効果を適用しました")
```

## 3. 複数レイヤーへのエフェクト一括適用

```python
# 複数のレイヤーを作成
layers = []
for i in range(5):
    color = [i / 4, 0.5, 1.0 - i / 4]
    l = comp.add_solid(f"Layer {i+1}", 300, 300, (color[0], color[1], color[2]))
    l.position = (300 + i * 350, 540)
    layers.append(l)

# すべてのレイヤーにエフェクトを追加
for i, layer in enumerate(layers):
    # ブラー（徐々に強く）
    blur = layer.add_effect("ADBE Gaussian Blur 2")
    blur.property("Blurriness").value = (i + 1) * 10

    # ドロップシャドウ
    shadow = layer.add_effect("ADBE Drop Shadow")
    shadow.property("Opacity").value = 50.0
    shadow.property("Distance").value = 20.0

print(f"{len(layers)}個のレイヤーにエフェクトを追加しました")
```

## 4. エフェクトパラメータのアニメーション

```python
# ブラーのアニメーション
blur = layer.add_effect("ADBE Gaussian Blur 2")
blurriness = blur.property("Blurriness")

blurriness.add_keyframe(0.0, 0.0)
blurriness.add_keyframe(5.0, 100.0)
blurriness.add_keyframe(10.0, 0.0)

# ベジェ補間に変更
blurriness.set_keyframe_interpolation(0, "bezier", "bezier")
blurriness.set_keyframe_interpolation(1, "bezier", "bezier")

print("ブラーアニメーションを追加しました")
```

## 5. カラーコレクションの自動化

```python
def apply_color_grade(layer, preset="warm"):
    """カラーグレーディングを適用"""

    presets = {
        "warm": {
            "hue": 10,
            "saturation": 20,
            "lightness": 5,
        },
        "cool": {
            "hue": -10,
            "saturation": 10,
            "lightness": -5,
        },
        "vibrant": {
            "hue": 0,
            "saturation": 50,
            "lightness": 10,
        },
    }

    params = presets.get(preset, presets["warm"])

    # Hue/Saturation
    hue_sat = layer.add_effect("ADBE HUE SATURATION")
    hue_sat.property("Master Hue").value = params["hue"]
    hue_sat.property("Master Saturation").value = params["saturation"]
    hue_sat.property("Master Lightness").value = params["lightness"]

    return hue_sat

# カラーグレーディングを適用
apply_color_grade(layer, "warm")
print("ウォームトーンのカラーグレーディングを適用しました")
```

## 6. エフェクトチェーンの構築

```python
def create_stylized_look(layer):
    """スタイライズドルックを作成"""

    # 1. ブラー（軽く）
    blur = layer.add_effect("ADBE Gaussian Blur 2")
    blur.property("Blurriness").value = 5.0

    # 2. シャープ
    sharpen = layer.add_effect("ADBE Unsharp Mask")
    sharpen.property("Amount").value = 100.0

    # 3. カラーコレクション
    curves = layer.add_effect("ADBE CurvesCustom")
    # カーブ設定はSDK経由で行う必要がある

    # 4. グロー
    glow = layer.add_effect("ADBE Glo2")
    glow.property("Glow Threshold").value = 60.0
    glow.property("Glow Radius").value = 80.0

    # 5. ビネット（4隅を暗く）
    levels = layer.add_effect("ADBE Pro Levels2")

    print(f"スタイライズドルック適用: {layer.num_effects}個のエフェクト")

# スタイライズドルックを適用
create_stylized_look(layer)
```

## 7. 条件付きエフェクト適用

```python
def apply_effects_based_on_layer_type(comp):
    """レイヤータイプに応じてエフェクトを適用"""

    for layer in comp.layers:
        # テキストレイヤーの場合
        if "text" in layer.name.lower():
            # ドロップシャドウとグロー
            shadow = layer.add_effect("ADBE Drop Shadow")
            shadow.property("Opacity").value = 75.0
            glow = layer.add_effect("ADBE Glo2")

        # ソリッドレイヤーの場合
        elif "solid" in layer.name.lower():
            # ブラーとカラーコレクション
            blur = layer.add_effect("ADBE Gaussian Blur 2")
            hue_sat = layer.add_effect("ADBE HUE SATURATION")

        print(f"{layer.name}: {layer.num_effects}個のエフェクトを追加")

# 条件付きエフェクト適用
apply_effects_based_on_layer_type(comp)
```

## 8. エフェクトのランダム化

```python
import random

def randomize_effect_parameters(layer, effect_name, param_ranges):
    """エフェクトパラメータをランダム化"""

    # エフェクトを取得または追加
    effect = layer.effect(effect_name)
    if not effect:
        effect = layer.add_effect(effect_name)

    # パラメータをランダム化
    for param_name, (min_val, max_val) in param_ranges.items():
        value = random.uniform(min_val, max_val)
        effect.property(param_name).value = value
        print(f"  {param_name}: {value:.2f}")

    return effect

# ブラーをランダム化
for layer in comp.layers:
    randomize_effect_parameters(
        layer,
        "ADBE Gaussian Blur 2",
        {"Blurriness": (10, 50)}
    )
```

## 完全な例: バッチエフェクト処理

```python
import ae

def batch_process_layers(comp, effects_config):
    """複数レイヤーに対してエフェクトをバッチ処理"""

    processed = 0

    for layer in comp.layers:
        # スキップ条件
        if layer.locked or not layer.enabled:
            continue

        # エフェクトを適用
        for effect_name, params in effects_config.items():
            effect = layer.add_effect(effect_name)

            # パラメータを設定
            for param_name, value in params.items():
                if isinstance(value, dict):
                    # アニメーション設定
                    prop = effect.property(param_name)
                    for time, val in value.items():
                        prop.add_keyframe(time, val)
                else:
                    # 固定値
                    effect.property(param_name).value = value

        processed += 1
        print(f"処理完了: {layer.name}")

    return processed

# 使用例
with ae.UndoGroup("Batch Effect Processing"):
    comp = ae.get_active_comp()

    effects_config = {
        "ADBE Gaussian Blur 2": {
            "Blurriness": {0.0: 0.0, 3.0: 50.0, 6.0: 0.0}
        },
        "ADBE Glo2": {
            "Glow Threshold": 50.0,
            "Glow Radius": 100.0,
            "Glow Intensity": 1.5
        }
    }

    count = batch_process_layers(comp, effects_config)
    print(f"{count}個のレイヤーを処理しました")
```

## よく使うエフェクトマッチ名リファレンス

```python
COMMON_EFFECTS = {
    # Blur & Sharpen
    "Gaussian Blur": "ADBE Gaussian Blur 2",
    "Fast Blur": "ADBE Fast Blur",
    "Sharpen": "ADBE Sharpen",
    "Unsharp Mask": "ADBE Unsharp Mask",

    # Color Correction
    "Levels": "ADBE Pro Levels2",
    "Curves": "ADBE CurvesCustom",
    "Hue/Saturation": "ADBE HUE SATURATION",
    "Color Correction": "ADBE Color Correction",

    # Stylize
    "Glow": "ADBE Glo2",
    "Drop Shadow": "ADBE Drop Shadow",
    "Stroke": "ADBE Stroke",
    "Fill": "ADBE Fill",

    # Distort
    "Transform": "ADBE Geometry2",
    "Warp": "ADBE Warp",

    # Generate
    "Solid": "ADBE Solid Composite",
}

# 使用例
for name, match_name in COMMON_EFFECTS.items():
    print(f"{name}: {match_name}")
```

## 次のステップ

- [API リファレンス - Effect](../api/high-level/effect)
- [API リファレンス - Property](../api/high-level/property)
