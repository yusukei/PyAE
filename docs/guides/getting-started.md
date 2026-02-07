# 開発環境セットアップ

PyAEの開発に必要な環境構築手順を説明します。このガイドに従えば、リポジトリのクローンから最初のビルド・動作確認まで完了できます。

---

## このドキュメントで学べること

- PyAE開発に必要なツールのインストール
- リポジトリのクローンとディレクトリ構成の理解
- 最初のビルドの実行方法
- After Effectsへのデプロイと動作確認

**所要時間の目安**: 初回セットアップは環境により異なります。ツールが既にインストールされている場合は、ビルドからデプロイまでスムーズに進められます。

---

## 必要なツール

以下のツールをインストールしてください。

| ツール | バージョン | 用途 |
|--------|-----------|------|
| **Visual Studio 2022** | v143 toolset | C++コンパイラ |
| **CMake** | 3.21以上 | ビルドシステム |
| **Git** | 最新版 | バージョン管理 |
| **After Effects** | 2022以降 | 動作確認 |

### Visual Studio 2022

[Visual Studio ダウンロードページ](https://visualstudio.microsoft.com/ja/downloads/)からインストール。

**必須コンポーネント:**
- 「C++によるデスクトップ開発」ワークロード
- Windows 10/11 SDK

### CMake

[CMake ダウンロードページ](https://cmake.org/download/)からインストール。

インストール時に「Add CMake to the system PATH」を選択してください。

### After Effects SDK

1. [Adobe Developer Console](https://developer.adobe.com/console/) にログイン
2. 「Downloads」→「After Effects」からSDKをダウンロード
3. ダウンロードしたZIPを `SDKs/` ディレクトリに展開

```
AfterFX/
└── SDKs/
    ├── ae25.6_61.64bit.AfterEffectsSDK/    # AE 2025.6用（推奨）
    ├── ae25.2_20.64bit.AfterEffectsSDK/    # AE 2025.0-2025.5用
    ├── AfterEffectsSDK 2023/               # AE 2023/2024用
    └── AfterEffectsSDK 2022/               # AE 2022用
```

```{tip}
開発対象のAfter Effectsバージョンに対応するSDKをダウンロードしてください。複数バージョンに対応する場合は、複数のSDKを配置できます。
```

### Python 3.10 Embedded

`Python310/` ディレクトリには埋め込み用Pythonランタイムが配置されています。

- **リポジトリに含まれている場合**: そのまま使用可能
- **含まれていない場合**: [Python公式サイト](https://www.python.org/downloads/release/python-3100/)から「Windows embeddable package (64-bit)」をダウンロードし、`Python310/` に展開

---

## リポジトリのクローン

```bash
git clone <repository-url>
cd AfterFX
```

### ディレクトリ構成の確認

```
AfterFX/
├── build.bat       # ビルドスクリプト
├── setup.bat       # マルチSDKビルド＆デプロイ
├── run_tests.bat   # テスト実行
├── build_docs.bat  # ドキュメントビルド
├── PyAE/           # メインのプラグインソースコード
│   ├── src/        # C++ソースコード
│   ├── include/    # C++ヘッダファイル
│   ├── tests/      # Pythonテストファイル
│   └── scripts/    # Pythonスクリプト
├── TestEffect/     # テスト用エフェクトプラグイン
├── Python310/      # 埋め込みPythonランタイム
├── SDKs/           # After Effects SDK（手動配置）
└── docs/           # ドキュメント
```

---

## 最初のビルド

Git Bashまたはコマンドプロンプトで、プロジェクトルートから以下を実行：

```bash
./build.bat
```

SDKパスとDLLサフィックスは `SDKs/` ディレクトリから自動検出されます。ビルドが成功すると `build/` ディレクトリに `PyAE.aex` と `PyAECore_*.dll` が生成されます。

```{note}
ビルドオプション、マルチSDK対応、出力ディレクトリの詳細は [ビルドノート](build-notes.md) を参照してください。
```

---

## After Effectsへのデプロイ

管理者権限でコマンドプロンプトを開き、以下を実行：

```bash
./setup.bat --skip-build
```

`C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\` にプラグインファイルがデプロイされます。デプロイされるファイルの詳細は [ビルドノート](build-notes.md) を参照してください。

---

## 動作確認

### 1. After Effectsを起動

After Effectsを起動し、以下を確認します：

1. **メニューの確認**: メニューバーに「**PyAE**」メニューが表示される
2. **Console確認**: 「PyAE」→「Console」でPythonコンソールが開く

```{note}
PyAEメニューが表示されない場合は、「トラブルシューティング」セクションを参照してください。
```

### 2. 簡単な動作テスト

PyAEコンソールで以下を実行し、エラーなく動作することを確認：

```python
import ae
project = ae.Project.get_current()
print(f"Project: {project}")
```

### 3. 自動テストの実行

より詳細な動作確認のため、自動テストを実行します：

```bash
./run_tests.bat --target core
```

テスト結果の確認：

```bash
./run_tests.bat --results
```

すべてのテストがパスすれば、環境構築は完了です。

---

## トラブルシューティング

### After Effectsでプラグインが表示されない

1. `setup.bat` を再実行
2. After Effectsを再起動
3. `C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\` にファイルが存在するか確認

### ビルドやデプロイでエラーが発生する

ビルドエラー（SDK not found、CMake not found等）やデプロイエラーの詳細な対処法は [ビルドノート](build-notes.md) を参照してください。

---

## 次のステップ

開発環境が整ったら、以下のドキュメントに進んでください：

1. **[ビルドノート](build-notes.md)** - ビルドシステムの詳細、マルチSDKビルド
2. **[テストガイド](test-guide.md)** - テストの実行方法、テストの書き方
3. **[アーキテクチャ](architecture.md)** - PyAEの内部設計
4. **[AEGP Suite アーキテクチャ](aegp-suite-architecture.md)** - After Effects SDKの構造
