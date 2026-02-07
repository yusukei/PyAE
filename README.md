# PyAE - Python for After Effects

> **This plugin is currently under development. APIs and features are subject to change without notice.**

Adobe After Effects の AEGP API に Python バインディングを提供するプラグインです。
Python の豊富なエコシステムを活用して、After Effects のワークフローを自動化できます。

## 主な機能

- **高レベル API** - Python らしい直感的なインターフェースでコンポジション、レイヤー、エフェクトを操作
- **低レベル API** - AEGP SDK への直接アクセスによる高度な制御
- **型ヒント** - 完全な型アノテーションによる IDE サポート（補完・型チェック）
- **アンドゥ対応** - すべての操作がアンドゥグループでまとめて取り消し可能
- **C++ ネイティブ実装** - pybind11 による高速な Python-C++ ブリッジ

## システム要件

- Windows 10/11 (64-bit)
- Adobe After Effects 2023 / 2024 / 2025
- Python 3.10（プラグインに組み込み済み）

## クイックスタート

```python
import ae

# プロジェクトを取得
project = ae.Project.get_current()

# コンポジションを作成
comp = project.create_comp("MyComp", 1920, 1080, duration=10.0)

# レイヤーを追加
layer = comp.add_solid("Red Solid", 1920, 1080, color=(1, 0, 0))
layer.position = (960, 540)

# エフェクトを追加
effect = layer.add_effect("ADBE Gaussian Blur 2")
effect.property("Blurriness").value = 50.0
```

## インストール

プラグインファイルを以下のディレクトリに配置します：

```
C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\
```

配置するファイル：
- `PyAE.aex` - メインプラグイン
- `PyAECore.dll` - コアライブラリ
- `Python310/` - Python ランタイム（埋め込み済み）

詳細は[インストールガイド](docs/getting-started/installation.md)を参照してください。

## ドキュメント

| ドキュメント | 内容 |
|---|---|
| [クイックスタート](docs/getting-started/quickstart.md) | 基本的な使い方 |
| [チュートリアル](docs/tutorials/) | 実践的なチュートリアル |
| [API リファレンス](docs/api/) | 高レベル/低レベル API |
| [サンプルスクリプト](PyAE/scripts/sample_scripts/) | 実行可能なサンプル集 |

ドキュメントのビルド：

```bash
build_docs.bat
```

## プロジェクト構成

```
PyAE/           コアプラグインソースコード
TestEffect/     パラメータテスト用エフェクトプラグイン
docs/           Sphinx ドキュメント
Python310/      組み込み Python ランタイム
SDKs/           Adobe After Effects SDK
```

## ビルド

```bash
# PyAE をビルド（SDK 自動検出）
build.bat

# TestEffect をビルド
build.bat --project TestEffect

# クリーンビルド
build.bat --clean

# テスト実行
run_tests.bat
```

詳細は[ビルドガイド](docs/guides/build-notes.md)を参照してください。

## ライセンス

[MIT License](LICENSE)
