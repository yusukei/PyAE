# PyAEのビルドについて

PyAEはAfter EffectsのPythonバインディングプラグインです。複数のAfter Effectsバージョンに対応するため、マルチSDKビルドシステムを採用しています。

---

## このドキュメントで学べること

- PyAEのビルドアーキテクチャ（ローダー方式、DLL構成）
- 各ビルドコマンドの使い方とオプション
- マルチSDKビルドの仕組み
- 日常的な開発ワークフロー

**前提**: [開発環境セットアップ](getting-started.md) が完了していること

---

## 日常的な開発ワークフロー

開発中によく使うコマンドパターンを紹介します。

### コード修正 → ビルド → テスト

```bash
# 1. コード修正後、ビルド（プロジェクトルートで実行）
./build.bat

# 2. デプロイ（After Effectsを閉じてから実行）
./setup.bat --skip-build

# 3. テスト実行
./run_tests.bat --target core
```

### クリーンビルド（問題発生時）

```bash
./build.bat --clean
```

### Releaseビルド（配布用）

```bash
./setup.bat --release
```

---

## アーキテクチャ

### ローダー方式

PyAEは2段階のローディングアーキテクチャを採用しています：

```
PyAE.aex (ローダー)
    ↓
    AEバージョン検出
    ↓
PyAECore_XXX.dll (実際の実装)
    ↓
python310.dll, pybind11, etc.
```

**理由:**
- After Effectsのバージョンごとに異なるSDK APIを使用
- 実行時にAEバージョンを検出し、適切なDLLを動的にロード
- SDKバージョン間の互換性問題を回避

### DLLファイル構成

| ファイル | 説明 |
|----------|------|
| `PyAE.aex` | ローダープラグイン（AEバージョン検出・DLLロード） |
| `PyAECore_2022.dll` | AE 2022用の実装 |
| `PyAECore_2023.dll` | AE 2023/2024用の実装 |
| `PyAECore_25_2.dll` | AE 2025.0〜2025.5用の実装 |
| `PyAECore_25_6.dll` | AE 2025.6以降用の実装 |
| `PyAEBridge.dll` | Python側からC++側を呼び出すブリッジ |
| `PyAEPySide.dll` | PySide6/Qt6統合（オプション） |

### バージョン検出ロジック

`Loader.cpp`内で、AEのメジャー/マイナーバージョンを基にDLLを選択：

```
AE 2025.6+  → PyAECore_25_6.dll → PyAECore_25_2.dll → PyAECore_2023.dll
AE 2025.x   → PyAECore_25_2.dll → PyAECore_2023.dll → PyAECore_2022.dll
AE 2023/24  → PyAECore_2023.dll → PyAECore_2022.dll
AE 2022     → PyAECore_2022.dll
```

---

## 対応SDKバージョン

| SDK名 | 対応AEバージョン | 出力DLL |
|-------|------------------|---------|
| AfterEffectsSDK 2022 | AE 2022 | PyAECore_2022.dll |
| AfterEffectsSDK 2023 | AE 2023, 2024 | PyAECore_2023.dll |
| ae25.2_20.64bit.AfterEffectsSDK | AE 2025.0〜2025.5 | PyAECore_25_2.dll |
| ae25.6_61.64bit.AfterEffectsSDK | AE 2025.6+ | PyAECore_25_6.dll |

---

## ビルドコマンド

```{important}
直接CMakeを実行することは禁止です。必ずバッチファイルを使用してください。
```

### build.bat - 単一SDKビルド

```bash
# プロジェクトルートで実行（引数なしでOK）
./build.bat
```

SDKパスは `SDKs/` ディレクトリから自動検出されます（最新SDK優先: 25.6 > 25.2 > 2023 > 2022）。
PyAEビルド時のDLLサフィックス（`_25_6` 等）もSDK名から自動検出されます。

**オプション:**

| オプション | 説明 | デフォルト |
|------------|------|-----------|
| `--sdk-path <path>` | SDKのExamplesディレクトリ | 自動検出 |
| `--project <name>` | ビルド対象（PyAE / TestEffect / TestRunner） | PyAE |
| `--config <type>` | ビルド構成（Debug/Release） | Debug |
| `--clean` | クリーンビルド | - |
| `--output-suffix <sfx>` | DLL名のサフィックス（例: `_25_6`） | 自動検出 |
| `--qt6-dir <path>` | Qt6のCMake設定パス | 自動検出 |

**使用例:**

```bash
# 引数なし（SDKとサフィックスを自動検出）
./build.bat

# TestEffectのビルド
./build.bat --project TestEffect

# 特定のSDKを指定
./build.bat --sdk-path "SDKs/AfterEffectsSDK 2022/Examples"

# Releaseビルド
./build.bat --config Release

# クリーンビルド
./build.bat --clean

# TestRunnerのビルド（SDK不要）
./build.bat --project TestRunner
```

### setup.bat - マルチSDKビルド＆デプロイ

```bash
# プロジェクトルートで実行（管理者権限が必要）
./setup.bat
```

**オプション:**

| オプション | 説明 |
|------------|------|
| `--release` | Releaseビルド（デフォルト） |
| `--debug` | Debugビルド |
| `--skip-build` | ビルドをスキップ、デプロイのみ |
| `--uninstall` | PyAE + TestEffectをアンインストール |

**実行内容:**
1. 4つのSDK（2022, 2023, 25.2, 25.6）でPyAEを順次ビルド
2. TestEffectをビルド
3. プラグインディレクトリにデプロイ
4. Python310ディストリビューションをコピー
5. スクリプト・テストファイルをコピー

### run_tests.bat - テスト実行

```bash
# 全テスト実行
./run_tests.bat

# 特定モジュールのテスト
./run_tests.bat --target Composition

# 複数モジュール
./run_tests.bat --target "Composition,Layer"

# テストグループ
./run_tests.bat --target core

# テストファイルのみデプロイ
./run_tests.bat --deploy

# 結果表示
./run_tests.bat --results
```

---

## 出力ディレクトリ構造

```
build/
├── ae25.6_61.64bit.AfterEffectsSDK/    # SDK 25.6ビルド（PyAE）
│   └── src/
│       ├── Debug/
│       │   ├── PyAE.aex
│       │   ├── PyAECore_25_6.dll
│       │   └── PyAEBridge.dll
│       └── Release/
│           └── ...
├── ae25.2_20.64bit.AfterEffectsSDK/    # SDK 25.2ビルド（PyAE）
│   └── ...
├── AfterEffectsSDK 2023/               # SDK 2023ビルド（PyAE）
│   └── ...
├── AfterEffectsSDK 2022/               # SDK 2022ビルド（PyAE）
│   └── ...
├── TestEffect/
│   └── ae25.6_61.64bit.AfterEffectsSDK/ # TestEffectビルド
│       └── ...
└── TestRunner/                          # TestRunnerビルド（SDK不要）
    └── Debug/
        └── TestRunner.exe
```

---

## インストール先

```
C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\
├── PyAE.aex
├── PyAECore_2022.dll
├── PyAECore_2023.dll
├── PyAECore_25_2.dll
├── PyAECore_25_6.dll
├── PyAEBridge.dll
├── PyAEPySide.dll        # Qt6がある場合
├── python310.dll
├── python310.zip
├── Lib/                  # Pythonライブラリ
├── scripts/              # Pythonスクリプト
├── tests/                # テストファイル
└── logs/                 # ログ出力
```

---

## 依存関係

### 必須

- **Visual Studio 2022** (v143 toolset)
- **CMake 3.21以上**
- **After Effects SDK** (2022, 2023, 25.2, 25.6のいずれか)
- **Python 3.10 Embedded** (`Python310/`ディレクトリに配置)
- **pybind11** (FetchContentで自動取得)

### オプション

- **Qt6** (PySide6統合用、自動検出)
- **vcpkg** (追加依存関係管理)

---

## SDK別機能対応状況

### SDK 2022/2023での制限

旧SDKでは一部のAPIが利用できません：

| 機能 | SDK 2022 | SDK 2023 | SDK 25.2+ |
|------|----------|----------|-----------|
| 基本レイヤー操作 | ✅ | ✅ | ✅ |
| BlendMode完全対応 | ⚠️ 制限あり | ✅ | ✅ |
| TrackMatte | ⚠️ 制限あり | ⚠️ 制限あり | ✅ |
| 新API (2025+) | ❌ | ❌ | ✅ |

### フォールバック動作

新しいAEで古いDLLがロードされた場合：
- 非対応APIは例外をスローまたはデフォルト値を返す
- アプリケーションはクラッシュせず継続動作

---

## トラブルシューティング

### ビルドエラー: SDKが見つからない

```
Error: SDK path does not exist
```

**解決:** `SDKs/`ディレクトリにSDKが配置されているか確認。または `--sdk-path`オプションで明示的に指定

### ビルドエラー: Qt6が見つからない

```
Qt6 not found - PyAEPySide.dll will not be built
```

**解決:** Qt6をインストールするか、`--qt6-dir`で明示的にパスを指定

### 実行時エラー: DLLが見つからない

```
Failed to load any PyAECore DLL
```

**解決:** `setup.bat`を再実行してデプロイ

### 実行時エラー: Python310.dllが見つからない

```
python310.dll not found
```

**解決:** `Python310/`ディレクトリがプラグインディレクトリにコピーされているか確認

---

## 次のステップ

ビルドシステムを理解したら、以下のドキュメントに進んでください：

1. **[テストガイド](test-guide.md)** - テストの実行方法、テストの書き方
2. **[アーキテクチャ](architecture.md)** - PyAEの内部設計を理解する

## 関連項目

- **前のドキュメント**: [開発環境セットアップ](getting-started.md)
- [低レベルAPI](../api/low-level/index.rst) - AEGP Suite リファレンス
