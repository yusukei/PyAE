# PyAE プロジェクトルール

## ビルドとデプロイ

**重要: ビルド、デプロイは必ずスクリプトを使用する**

### ビルドスクリプト（プロジェクトルートで実行）

- **通常ビルド**: `./build.bat`
  - SDKは自動検出（SDKs/ディレクトリから最新優先）
  - 例: `./build.bat --config Release --clean`

- **TestEffectビルド**: `./build.bat --project TestEffect`

- **TestRunnerビルド**: `./build.bat --project TestRunner`
  - SDK不要

- **マルチSDKビルド＆デプロイ**: `./setup.bat`
  - 管理者権限が必要
  - 全SDKバージョン（2022, 2023, 25.2, 25.6）をビルド
  - プラグインディレクトリへ自動デプロイ

### テスト実行（プロジェクトルートで実行）

- **全テスト**: `./run_tests.bat`
- **特定テスト**: `./run_tests.bat --target Composition`
- **テストグループ**: `./run_tests.bat --target core`
- **結果表示**: `./run_tests.bat --results`

テスト結果は `logs/` ディレクトリに出力されます。

### デプロイ先

```
C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\
```

### ビルド・デプロイ時の注意事項

1. After Effectsが実行中の場合、ファイルがロックされてデプロイできない
2. デプロイ後は必ずAfter Effectsを再起動する
3. 手動でのファイルコピーは禁止 - 必ずスクリプトを使用する
