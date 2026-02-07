# PyAE Type Stubs

After Effects Python API 用の型スタブファイル（.pyi）パッケージです。

## 概要

このパッケージは、PyAE（Python for After Effects）の型情報を提供し、以下をサポートします：

- **IDE自動補完**: VSCode、PyCharmなどのIDEで型情報に基づいた補完が利用できます
- **型チェック**: mypy、pyrightなどの型チェッカーで静的型チェックが可能です
- **ドキュメント**: 各APIの引数と戻り値の型情報がdocstringで確認できます

## 使用方法

### 1. IDE設定

VSCodeの場合、以下の設定を`.vscode/settings.json`に追加：

```json
{
    "python.analysis.stubPath": "path/to/PyAE",
    "python.analysis.typeCheckingMode": "basic"
}
```

### 2. mypy設定

`mypy.ini`または`pyproject.toml`に以下を追加：

```ini
[mypy]
mypy_path = path/to/PyAE
```

### 3. コード例

```python
import ae
from ae import Layer, Comp

# 型ヒント付きコード
def process_layers(comp: Comp) -> list[Layer]:
    layers: list[Layer] = comp.layers
    return [layer for layer in layers if layer.enabled]

# 自動補完が効く
project = ae.project
comp = project.create_comp("MyComp", 1920, 1080, 1.0, 10.0, 30.0)
layer = comp.add_solid("Red", 1920, 1080, (1.0, 0.0, 0.0, 1.0), 10.0)
layer.position = (960.0, 540.0, 0.0)  # 型チェックされる
```

## ファイル構成

```
ae-stubs/
├── __init__.pyi          # メインモジュール
├── types.pyi             # 型定義（Enum）
├── project.pyi           # Project API
├── item.pyi              # Item/Comp/Folder/Footage API
├── layer.pyi             # Layer API
├── effect.pyi            # Effect API
├── mask.pyi              # Mask API
├── property.pyi          # Property/Keyframe API
├── render_queue.pyi      # RenderQueue API
├── py.typed              # PEP 561 マーカー
└── README.md             # このファイル
```

## API概要

### モジュールレベル

- `ae.project` - 現在のプロジェクト
- `ae.render_queue` - レンダーキュー
- `ae.UndoGroup` - アンドゥグループ（コンテキストマネージャ）

### 主要クラス

- `Project` - プロジェクト管理
- `Item`, `Comp`, `Folder`, `Footage` - プロジェクトアイテム
- `Layer` - レイヤー操作
- `Effect` - エフェクト
- `Mask` - マスク
- `Property`, `Keyframe` - プロパティとキーフレーム
- `RenderQueue`, `RQItem`, `OutputModule` - レンダリング

### Enum型

- `LayerType` - レイヤータイプ
- `ItemType` - アイテムタイプ
- `MaskMode` - マスクモード
- `RenderStatus` - レンダーステータス
- `PropertyType` - プロパティタイプ

## PEP 561準拠

このパッケージはPEP 561に準拠しており、`py.typed`マーカーファイルを含んでいます。
これにより、型チェッカーがこのパッケージの型情報を自動的に認識します。

## 参考資料

- [Python API設計ドキュメント](../docs/Python_API_Design.md)
- [PEP 561 - Distributing and Packaging Type Information](https://www.python.org/dev/peps/pep-0561/)
