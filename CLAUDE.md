# AfterFX Project Rules

## Project Overview

PyAE (Python for After Effects) - After Effects plugin providing Python bindings for automation and scripting.
After Effects のすべてのAEGP APIをPythonから利用可能にするプラグイン。プロジェクトは完成済み。

## Work Process

**CRITICAL: 調査と計画を優先し、安易な修正を禁止する**

1. **調査優先** - 修正を始める前に、必ず関連コードを十分に調査する
2. **計画立案** - 調査結果に基づき、修正計画を立ててから実装する
3. **安易な修正禁止** - 試行錯誤的な修正を繰り返さない
4. **根本原因の特定** - 表面的な対処ではなく、問題の根本原因を特定する
5. **一度で正しく** - 何度も修正を繰り返すのではなく、一度で正しい修正を行う

**CRITICAL: コードをクリーンに保つ**

6. **作業終了時のクリーンアップ必須** - 作業終了前に必ず不要なコードを削除する
7. **デバッグコードの削除** - 一時的なprint文、ログ出力、テスト用コードは必ず削除
8. **未使用コードの削除** - コメントアウトされたコード、未使用の変数・関数は削除
9. **テストファイルの整理** - 一時的なテストスクリプトは作業完了後に削除
10. **散乱禁止** - 修正コードやテストコードを散乱させない

## Coding Guidelines

1. **Use English for code comments** - Keeps code accessible
2. **Japanese for documentation** - User-facing docs in Japanese
3. **Consistent naming** - Follow existing patterns in codebase
4. **ExtendScript禁止** - ExtendScript（.jsx）の使用は禁止。すべてPython + AEGP APIで実装する

## Git Commit Guidelines

- Use Japanese for commit messages
- Format: `type: description`
  - `feat:` - New feature
  - `fix:` - Bug fix
  - `docs:` - Documentation
  - `refactor:` - Code refactoring
  - `test:` - Test additions

## Project Components

| Directory | Description |
|-----------|-------------|
| `PyAE/` | Core plugin source code |
| `TestEffect/` | Test effect plugin for parameter testing |
| `docs/` | Sphinx documentation |
| `Python310/` | Embedded Python runtime |
| `SDKs/` | Adobe After Effects SDK |
| `logs/` | Debug/test logs |

## Build Commands

All batch files run from project root. No arguments required (auto-detected).
Details: `.claude/rules/build-rules.md`

```bash
./build.bat                       # PyAE build (latest SDK auto-detected)
./build.bat --project TestEffect  # TestEffect build
./build.bat --clean               # Clean build
./setup.bat                       # Multi-SDK build + deploy (admin required)
./run_tests.bat                   # Run tests
./build_docs.bat                  # Build Sphinx docs
```

## Troubleshooting

### AEGP API - NULL Pointer Issues

AEGP APIの多くの関数は**NULLポインタを受け付けない**。値を変更したくない場合でも、現在の値を取得してから渡す必要がある。

```cpp
// NG: AEGP_SetLayerInPointAndDuration に nullptr を渡すとクラッシュ
err = suites.layerSuite->AEGP_SetLayerInPointAndDuration(
    m_layerH, AEGP_LTimeMode_CompTime, &inPoint, nullptr);

// OK: 現在の値を取得してから渡す
A_Time currentDuration = {0, 1};
err = suites.layerSuite->AEGP_GetLayerDuration(m_layerH, AEGP_LTimeMode_CompTime, &currentDuration);
err = suites.layerSuite->AEGP_SetLayerInPointAndDuration(
    m_layerH, AEGP_LTimeMode_CompTime, &inPoint, &currentDuration);
```

`A_Time` の初期化は `{0, 1}` を使用（`scale=0` はエラーの原因になる）。

## Rules

Detailed rules are defined in `.claude/rules/`:

| File | Description |
|------|-------------|
| `mindset-rules.md` | **CRITICAL: Development mindset and self-checking principles** |
| `shell-commands.md` | Shell command restrictions |
| `build-rules.md` | Build process rules |
| `docs-rules.md` | Documentation management |
| `deployment-rules.md` | File path and deployment policy |
| `testing-rules.md` | Testing and completion criteria |
| `windows-git-bash-notes.md` | Windows + Git Bash execution notes |
