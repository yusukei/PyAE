# PyAE Phase 2 テストスクリプト

## 概要

このディレクトリには、PyAE Phase 2 の機能をテストするためのスクリプトが含まれています。

## ファイル一覧

- `phase2_test.py` - 包括的なテストスイート（全機能を自動テスト）
- `quick_test.py` - クイックテスト（個別機能を対話的にテスト）

## 実行方法

### 1. After Effects でテストを実行

1. After Effects を起動
2. 任意のプロジェクトを開く（または新規作成）
3. PyAE メニュー → Run Script を選択
4. テストスクリプトを選択して実行

### 2. PyAE REPL からテストを実行

```python
# REPL を開いている場合
exec(open(r"D:\works\VTechStudio\AfterFX\PyAE\scripts\test_scripts\phase2_test.py").read())
```

または quick_test.py をインポートして個別に実行:

```python
import sys
sys.path.append(r"D:\works\VTechStudio\AfterFX\PyAE\scripts\test_scripts")
import quick_test

# 基本情報表示
quick_test.info()

# 個別テスト実行
quick_test.test_project()
quick_test.test_active_comp()
quick_test.test_create_comp("MyTestComp")
quick_test.test_japanese_name()

# 全テスト実行
quick_test.run_all()
```

## テスト項目

### Phase 2 テストスイート (phase2_test.py)

| # | テスト項目 | 説明 |
|---|-----------|------|
| 1 | プロジェクト取得 | `ae.get_project()` が正しく動作するか |
| 2 | アクティブコンポ取得 | `ae.get_active_comp()` が正しく動作するか |
| 3 | コンポ作成 | 新規コンポジションを作成できるか |
| 4 | 日本語名 | UTF-8/UTF-16 変換が正しく動作するか |
| 5 | ソリッド追加 | ソリッドレイヤーを追加できるか |
| 6 | レイヤープロパティ | 名前、位置、不透明度の取得/設定 |
| 7 | キーフレーム | キーフレームの追加/取得/削除 |
| 8 | Undo/Redo | UndoGroup が正しく作成されるか |
| 9 | エラーハンドリング | 削除されたオブジェクトへのアクセス |

## 期待される出力

### 成功時

```
[PASS] get_project returns value
[PASS] project.name accessible: MyProject
[PASS] Create comp (English name): Created: PyAE_Test_Comp
[PASS] Japanese name roundtrip: 'テストコンポ_日本語'
...

Test Results Summary
====================
Total tests: 25
Passed: 25
Failed: 0
Pass rate: 100.0%
```

### 失敗時

```
[FAIL] Set layer position: Failed to set property value
...

Failed tests:
  - Set layer position: Failed to set property value
```

## トラブルシューティング

### プラグインが読み込まれない

- AE プラグインフォルダに `PyAE.aex` と `PyAECore.dll` がコピーされているか確認
- Python 3.10 の DLL (`python310.dll`) が同じフォルダにあるか確認

### 日本語が文字化けする

- UTF-8/UTF-16 変換に問題がある可能性
- ログファイルを確認してエラーを特定

### クラッシュする

- Debug ビルドで実行してスタックトレースを確認
- AE のバージョンと SDK のバージョンが一致しているか確認

## ログファイルの場所

ログファイルは以下の場所に出力されます:

```
%USERPROFILE%\Documents\PyAE\logs\
```

または

```
C:\Users\<ユーザー名>\Documents\PyAE\logs\
```
