# PyAEのテストについて

PyAEのテストシステムは、After Effects内でPythonテストを自動実行する仕組みを提供します。C++製のテストランナーがAfter Effectsプロセスを管理し、環境変数によるAuto Testモードでプラグイン内部からテストを実行します。

---

## このドキュメントで学べること

- テストの実行方法（全テスト、特定テスト、グループテスト）
- テスト結果の確認方法
- 新しいテストの書き方
- テストシステムのアーキテクチャ

**前提**: [ビルドノート](build-notes.md) を理解し、ビルドとデプロイができること

---

## クイックスタート: テストを実行してみる

### 1. 全テストの実行

```bash
./run_tests.bat
```

### 2. 結果の確認

```bash
./run_tests.bat --results
```

成功した場合、以下のような出力が表示されます：

```
Total: 841 tests
Passed: 841
Failed: 0
Memory Leaks: None
```

### 3. 特定のテストのみ実行

```bash
# Compositionテストのみ
./run_tests.bat --target Composition

# 複数指定
./run_tests.bat --target "Composition,Layer"
```

---

## アーキテクチャ

### システム構成

```
[run_tests.bat]                   バッチエントリポイント
       │
       │  環境変数設定 + プロセス起動
       ▼
[TestRunner.exe]                  C++テストランナー
       │  - After Effects プロセス管理
       │  - OutputDebugString キャプチャ
       │  - タイムアウト制御
       │  - テストファイルデプロイ
       ▼
[AfterFX.exe]                     After Effects (Auto Test Mode)
       │
       │  環境変数検出 → IdleHandler 起動
       ▼
[PyAEBridge.dll / PyAECore.dll]   PyAE プラグイン
       │
       │  Python インタプリタ実行
       ▼
[test_runner.py]                  Python テストランナー
       │  - テストモジュール読み込み
       │  - メモリリーク検出
       │  - 結果集計
       ▼
[test_*.py]                       Python テストファイル群
       │
       ▼
[test_results.json]               結果出力（JSON）
```

### コンポーネント一覧

| コンポーネント | 言語 | 役割 |
|---------------|------|------|
| `run_tests.bat` | Batch | エントリポイント、環境設定 |
| `TestRunner.exe` | C++ | プロセス管理、デバッグキャプチャ |
| `IdleHandler.cpp` | C++ | Auto Testモード検出、Python実行 |
| `test_runner.py` | Python | テスト実行制御、結果集計 |
| `test_utils.py` | Python | TestSuiteクラス、アサーション関数 |
| `test_*.py` | Python | 各機能のテストコード |

---

## Auto Test モード

### 動作原理

Auto Testモードは、環境変数を使用してAfter Effects起動時に自動的にテストを実行する仕組みです。

```
1. TestRunner.exe が環境変数を設定して AfterFX.exe を起動
2. PyAEプラグインの IdleHandler が環境変数 PYAE_AUTO_TEST=1 を検出
3. IdleHandler が指定されたPythonモジュール・関数を実行
4. テスト完了後、After Effects を自動終了
```

### 環境変数

| 変数名 | 値 | 説明 |
|--------|-----|------|
| `PYAE_AUTO_TEST` | `1` | Auto Testモードを有効化 |
| `PYAE_TEST_MODULE` | `test_runner` | 実行するPythonモジュール名 |
| `PYAE_TEST_FUNCTION` | `run_all_tests` / `run_specific_tests` | 実行する関数名 |
| `PYAE_TEST_TARGET` | `Composition,Layer` | 特定テスト指定（カンマ区切り） |
| `PYAE_TEST_REPEAT` | `3` | テスト繰り返し回数 |
| `PYAE_TEST_OUTPUT` | パス | 結果JSONファイルパス |
| `PYAE_TEST_EXIT` | `1` | テスト完了後に自動終了 |
| `PYAE_TEST_TIMEOUT` | `600` | タイムアウト秒数 |

### C++ IdleHandler の実装

```cpp
// IdleHandler.cpp (概要)
void IdleHandler::CheckAutoTest() {
    // 環境変数チェック
    if (getenv("PYAE_AUTO_TEST") == "1") {
        const char* module = getenv("PYAE_TEST_MODULE");    // "test_runner"
        const char* function = getenv("PYAE_TEST_FUNCTION"); // "run_all_tests"
        const char* target = getenv("PYAE_TEST_TARGET");     // optional

        // Python実行
        ExecutePythonFunction(module, function, target);

        // 自動終了
        if (getenv("PYAE_TEST_EXIT") == "1") {
            QuitAfterEffects();
        }
    }
}
```

---

## テストランナー (C++)

### 主要機能

`TestRunner.exe` は以下の機能を提供します：

1. **テストファイルデプロイ**: `scripts/` と `tests/` をプラグインディレクトリにコピー
2. **After Effects起動**: 環境変数を設定してプロセスを開始
3. **OutputDebugStringキャプチャ**: DBWINプロトコルでデバッグログを収集
4. **タイムアウト管理**: 指定時間内にプロセスが終了しない場合は強制終了
5. **結果収集**: JSON結果ファイルをコピー

### コマンドライン引数

```
TestRunner.exe [options]

Options:
  --ae-path PATH        After Effects 実行ファイルパス
  --plugin-dir PATH     PyAE プラグインディレクトリ
  --pyae-dir PATH       PyAE ソースディレクトリ
  --timeout SECONDS     タイムアウト秒数（デフォルト: 600）
  --test-target MODULE  テストモジュール指定
  --test-repeat N       繰り返し回数
  --debug-log PATH      デバッグログファイルパス
```

### OutputDebugString キャプチャ

TestRunner.exe は Windows の DBWIN プロトコルを使用して `OutputDebugString` 出力をキャプチャします。これにより、After Effects 内のログメッセージをリアルタイムで収集できます。

```cpp
// DBWINプロトコルの共有メモリ構造
// - DBWIN_BUFFER_READY: バッファ準備完了イベント
// - DBWIN_DATA_READY: データ準備完了イベント
// - DBWIN_BUFFER: 4096バイトの共有メモリ
//   - 最初の4バイト: PID
//   - 残り: メッセージ文字列
```

---

## 使用方法

### 全テスト実行

```bash
./run_tests.bat
```

### 特定テストのみ実行

```bash
# 単一モジュール
./run_tests.bat --target Composition

# 複数モジュール
./run_tests.bat --target "Composition,Layer,Effect"

# テストグループ
./run_tests.bat --target core      # Core Tests
./run_tests.bat --target animation  # Animation Tests
./run_tests.bat --target sdk        # SDK Suite Tests
```

### テストファイルのみデプロイ

```bash
./run_tests.bat --deploy
```

### 最後のテスト結果表示

```bash
./run_tests.bat --results
```

### 繰り返し実行（メモリリーク検出用）

```bash
./run_tests.bat --target Composition --repeat 5
```

---

## テストフレームワーク

### TestSuite クラス

`test_utils.py` で定義される `TestSuite` クラスがテストの基盤です。

```python
from test_utils import TestSuite, assert_true, assert_equal

suite = TestSuite("My Feature")

@suite.setup
def setup():
    """テスト前の準備"""
    pass

@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    pass

@suite.test
def test_something():
    """テストケース"""
    result = some_function()
    assert_equal(result, expected_value, "結果が期待値と一致すること")

def run():
    """エントリポイント（必須）"""
    return suite.run()
```

### アサーション関数

| 関数 | 説明 |
|------|------|
| `assert_true(condition, message)` | 条件がTrueであること |
| `assert_false(condition, message)` | 条件がFalseであること |
| `assert_equal(expected, actual, message)` | 値が等しいこと |
| `assert_not_equal(expected, actual, message)` | 値が異なること |
| `assert_not_none(value, message)` | 値がNoneでないこと |
| `assert_none(value, message)` | 値がNoneであること |
| `assert_isinstance(obj, type, message)` | 型が一致すること |
| `assert_close(expected, actual, tolerance, message)` | 浮動小数点が近いこと |
| `assert_in(item, container, message)` | 要素が含まれること |
| `assert_raises(exception, func, *args)` | 例外が発生すること |

### メモリリーク検出

テストランナーは各テストモジュール実行前後でメモリ状態を記録し、リークを検出します。

```python
# test_runner.py での実装
mem_before = ae.get_memory_stats()
results = module.run()
mem_after = ae.get_memory_stats()

has_leak = ae.check_memory_leak(
    mem_before, mem_after,
    tolerance_handles=5,    # 許容ハンドル増加数
    tolerance_size=10*1024  # 許容サイズ増加（10KB）
)
```

---

## テストグループとモジュール

### グループ定義

| グループ | 含まれるテスト |
|----------|--------------|
| `core` | Project, Composition, Layer, Effect, Mask, Property, Command, Utility, Marker 等 |
| `animation` | Keyframe Operations, Interpolation, Animation Roundtrip, Motion Path, Easing |
| `sdk` | Camera/Light, I/O, Footage, Collection, Data/Settings, Render 関連 Suite |
| `shape` | Shape Layer Basic, Shape Properties, Trim Paths |
| `other` | Parent Linking, FootageItem, Folder Operations, Expression Links 等 |
| `all` | 全テストモジュール |

### モジュール別エイリアス

```
comp      → Composition
layer     → Layer
effect    → Effect
keyframe  → Keyframe Operations
camera    → Camera/Light Suite
io        → I/O Suite
data      → Data/Settings Suite
memory    → Memory Diagnostics
```

---

## ファイル構成

```
AfterFX/                             # プロジェクトルート
├── run_tests.bat                    # エントリポイント
├── build/TestRunner/
│   └── Debug/TestRunner.exe         # C++テストランナー
├── logs/
│   ├── test_results.json            # テスト結果（JSON）
│   └── test_debug.log               # デバッグログ
├── PyAE/scripts/
│   └── run_tests.py                 # Pythonエントリポイント
└── PyAE/tests/
    ├── __init__.py
    ├── test_runner.py               # メインテストランナー
    ├── test_utils.py                # TestSuiteクラス、アサーション
    ├── core/                        # コアテスト
    │   ├── test_project.py
    │   ├── test_comp.py
    │   ├── test_layer.py
    │   └── ...
    ├── animation/                   # アニメーションテスト
    ├── shape/                       # シェイプレイヤーテスト
    ├── render_queue/               # レンダーキューテスト
    └── ...
```

---

## 結果ファイル

### JSON形式 (`test_results.json`)

```json
{
  "success": true,
  "result": {
    "all_results": {
      "Composition": {
        "results": [
          {"name": "test_create_comp", "passed": true, "message": "", "error": ""}
        ],
        "passed": 48,
        "failed": 0,
        "total": 48,
        "memory_leak": false
      }
    },
    "total_passed": 841,
    "total_failed": 0,
    "memory_leaks": [],
    "overall_memory_leak": false
  }
}
```

### デバッグログ (`test_debug.log`)

```
[21:26:33.949] [ 8084] [PyAE] [INFO ] [Python] Creating new project...
[21:26:34.185] [ 8084] [PyAE] [INFO ] [Python] >>> Starting Project...
[21:26:34.189] [ 8084] [PyAE] [INFO ] [Python] [PASS] test_project_exists
...
```

---

## トラブルシューティング

### After Effects が起動しない

1. パスを確認:
   ```bash
   # run_tests.bat 内の AE_PATH を確認
   set "AE_PATH=C:\Program Files\Adobe\Adobe After Effects 2025\Support Files\AfterFX.exe"
   ```

2. プラグインがインストールされているか確認:
   ```bash
   dir "C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\PyAEBridge.dll"
   ```

### テストがタイムアウトする

1. タイムアウト値を増やす:
   ```bash
   # run_tests.bat 内で変更
   set PYAE_TEST_TIMEOUT=900
   ```

2. 特定テストのみ実行して問題を特定:
   ```bash
   ./run_tests.bat --target Composition
   ```

### デバッグログが出力されない

1. 別のデバッグビューア（DebugView等）が起動していないか確認
2. Visual Studio デバッガがアタッチされていないか確認

### 結果ファイルが生成されない

1. テストが正常に完了していない可能性 → デバッグログを確認
2. 書き込み権限を確認

---

## テストを書いてみる

新しい機能を実装したら、テストを書いてその動作を検証しましょう。ここでは、テストファイルの作成からテスト実行までの流れを説明します。

### ステップ1: テストファイルを作成

`PyAE/tests/` ディレクトリに新しいテストファイルを作成します。

```python
# tests/core/test_my_feature.py

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_equal,
        assert_not_none,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_equal,
        assert_not_none,
    )

suite = TestSuite("My Feature")

# テスト用グローバル変数
_test_comp = None

@suite.setup
def setup():
    """テスト前の準備"""
    global _test_comp
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_TestComp", 1920, 1080, 1.0, 10.0, 30.0)

@suite.teardown
def teardown():
    """テスト後のクリーンアップ（必ず実行）"""
    global _test_comp
    if _test_comp:
        try:
            _test_comp.delete()
        except:
            pass

@suite.test
def test_feature_works():
    """機能が正しく動作すること"""
    global _test_comp
    result = _test_comp.some_method()
    assert_not_none(result, "結果がNoneでないこと")

def run():
    """エントリポイント（必須）"""
    return suite.run()

if __name__ == "__main__":
    run()
```

### テスト登録

`test_runner.py` にインポートとモジュール登録を追加:

```python
# インポート追加
from .my_folder import test_my_feature

# test_modules リストに追加
test_modules = [
    ...
    ("My Feature", test_my_feature),
]

# エイリアス追加（オプション）
aliases = {
    ...
    "myfeature": "My Feature",
}
```

---

## 次のステップ

テストシステムを理解したら、以下のドキュメントに進んでください：

1. **[アーキテクチャ](architecture.md)** - PyAEの内部設計を理解する
2. **[AEGP Suite アーキテクチャ](aegp-suite-architecture.md)** - After Effects SDKの構造

## 関連項目

- **前のドキュメント**: [ビルドノート](build-notes.md)
- [高レベルAPI](../api/high-level/index.rst) - Python APIリファレンス
