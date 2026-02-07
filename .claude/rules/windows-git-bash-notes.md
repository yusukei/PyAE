---
description: Windows環境でGit Bash経由でコマンドを実行する際の注意点
globs: ["**/*"]
alwaysApply: true
---

# Windows + Git Bash コマンド実行の注意点

このドキュメントは、Windows環境でGit Bashを使用してPythonの`subprocess`モジュールからコマンドを実行する際の注意点をまとめたものです。

## 1. パス処理

### 1.1 パス形式の統一

**推奨: `pathlib.Path` を使用**

```python
from pathlib import Path

# OS非依存のパス処理
script_dir = Path(__file__).parent
build_bat = script_dir.parent / "build.bat"
```

### 1.2 Git Bashのパス自動変換を無効化

Git BashはPOSIX→Windowsパス自動変換を行うため、意図しない動作を引き起こす場合があります。

```python
import subprocess
import os

# パス変換を無効化
env = os.environ.copy()
env['MSYS_NO_PATHCONV'] = '1'

subprocess.run(['command', '/path/arg'], env=env)
```

### 1.3 スペースを含むパス

```python
# リスト形式なら自動的にエスケープされる（推奨）
ae_path = r"C:\Program Files\Adobe\Adobe After Effects 2025\Support Files\AfterFX.exe"
subprocess.run([ae_path, "-r", script_path])

# shell=True の場合は手動で引用符が必要（非推奨）
subprocess.run(f'"{ae_path}" -r "{script_path}"', shell=True)
```

---

## 2. コマンド実行方式

### 2.1 バッチファイル (.bat) の実行

**重要:** Windowsでは`.bat`ファイルは`cmd.exe`で解釈されます。

```python
# shell=True が必要な場合（内部コマンド: dir, copy, cd など）
subprocess.run("dir", shell=True)

# shell=False でも実行可能（.batファイル、.exeファイル）
subprocess.run(["build.bat", "--clean"], shell=False)
```

**PyAEでの実装パターン:**

```python
result = subprocess.run(
    ["build.bat", "--clean"],
    cwd=pyae_dir,
    shell=True,              # バッチファイルのため shell=True
    capture_output=True,
    text=True,
    encoding='utf-8',
    errors='replace',
    timeout=300
)
```

### 2.2 実行可能ファイル (.exe) の実行

```python
# .exeファイルは shell=False で実行（推奨）
ae_path = r"C:\Program Files\Adobe\Adobe After Effects 2025\Support Files\AfterFX.exe"
result = subprocess.run(
    [ae_path, "-r", script_path],
    capture_output=True,
    text=True
)
```

### 2.3 バッチファイルから他のバッチファイルを呼び出す

**重要:** バッチファイル内では `call` コマンドを使用する必要があります。

```batch
REM 良い例：call を使用
call build.bat --clean

REM 悪い例：call なし（呼び出し元のバッチが終了してしまう）
build.bat --clean
```

---

## 3. エンコーディングとタイムアウト

### 3.1 文字エンコーディングの明示

**必須:** Windows日本語環境では`encoding`と`errors`を明示的に指定

```python
result = subprocess.run(
    ["command"],
    capture_output=True,
    text=True,
    encoding='utf-8',        # UTF-8を明示
    errors='replace'         # デコードエラーを置換
)
```

### 3.2 タイムアウトの設定

**推奨:** 長時間実行される可能性のあるコマンドには必ず`timeout`を設定

```python
try:
    result = subprocess.run(
        ["command"],
        timeout=300,  # 5分でタイムアウト
        capture_output=True
    )
except subprocess.TimeoutExpired:
    print("コマンドがタイムアウトしました")
```

---

## 4. 環境変数

### 4.1 環境変数の設定

```python
import subprocess
import os

# 環境変数を設定して実行
env = os.environ.copy()
env['PYAE_AUTO_TEST'] = '1'
env['PYAE_TEST_TIMEOUT'] = '600'

subprocess.run(['command'], env=env)
```

### 4.2 Git Bash特有の環境変数

| 変数 | 説明 |
|------|------|
| `MSYS_NO_PATHCONV` | パス自動変換を無効化 |
| `MSYSTEM` | MSYS2のサブシステム（MINGW64, MSYS等） |
| `HOME` | ユーザーのホームディレクトリ |

---

## 5. 改行コードとGit設定

### 5.1 改行コードの問題

**問題:** Git Bashで作成したスクリプトがCRLFだと、Bash実行時にエラーになる場合がある

```
/bin/bash^M: bad interpreter: No such file or directory
```

### 5.2 Git設定での対処

```bash
# Windows環境での推奨設定（LFを強制）
git config --global core.autocrlf input
git config --global core.eol lf
```

---

## 6. ベストプラクティス

### 6.1 推奨される実装パターン

```python
from pathlib import Path
import subprocess
import os

def run_build():
    """ビルドを実行する例"""
    script_dir = Path(__file__).parent
    pyae_dir = script_dir.parent
    build_bat = pyae_dir / "build.bat"

    try:
        result = subprocess.run(
            [str(build_bat), "--clean"],
            cwd=str(pyae_dir),
            shell=True,  # Windows .bat ファイルのため
            capture_output=True,
            text=True,
            encoding='utf-8',
            errors='replace',
            timeout=300
        )

        if result.returncode != 0:
            print(f"Build failed: {result.stderr}")
            return False

        return True

    except subprocess.TimeoutExpired:
        print("Build timed out")
        return False
    except Exception as e:
        print(f"Error: {e}")
        return False
```

### 6.2 避けるべきパターン

```python
# 悪い例：シェルコマンドに依存
subprocess.run("ls -la", shell=True)  # Bashがないと動かない

# 悪い例：パス変換の問題を無視
subprocess.run(["/c/Program Files/tool.exe"])  # Git Bash特有のパス

# 悪い例：エンコーディング指定なし
subprocess.run(["command"], capture_output=True, text=True)  # 日本語でエラー
```

### 6.3 PyAEプロジェクトでの制約

**許可されたコマンドのみ使用:**

| コマンド | 説明 |
|---------|------|
| `python` | Python実行（subprocess経由） |
| `git` | バージョン管理操作 |
| `cmd` | Windows command prompt |
| `powershell` / `pwsh` | PowerShell |
| `build.bat` | ビルド実行 |
| `rebuild-and-deploy.bat` | リビルド＆デプロイ |
| `setup_all.bat` | マルチSDKビルド＆デプロイ |
| `run_tests.bat` | テスト実行 |

**禁止されたコマンド:**

- `cmake` - 直接実行禁止（必ず`build.bat`経由）

---

## 7. チェックリスト

コマンド実行前に以下を確認:

- [ ] パス処理に `pathlib.Path` を使用しているか
- [ ] `encoding='utf-8', errors='replace'` を指定しているか
- [ ] `timeout` を設定しているか（長時間実行の場合）
- [ ] 環境変数が必要な場合、`env=os.environ.copy()` を使用しているか
- [ ] エラーハンドリング (`try-except`) を実装しているか
- [ ] バッチファイルから他のバッチファイルを呼び出す場合、`call` を使用しているか
- [ ] スペースを含むパスをリスト形式で渡しているか

---

## まとめ

### 重要なポイント

1. **パス処理**: `pathlib.Path`を使用してOS非依存のコード書く
2. **バッチファイル実行**: `shell=True`の使用は慎重に。リスト形式の引数を推奨
3. **エンコーディング**: `encoding='utf-8', errors='replace'`を明示的に指定
4. **タイムアウト**: 長時間実行コマンドには必ず`timeout`を設定
5. **エラーハンドリング**: `try-except`で`TimeoutExpired`と一般例外を捕捉
6. **環境変数**: `env=os.environ.copy()`で環境変数を設定
7. **改行コード**: Git設定で`core.autocrlf=input`、`core.eol=lf`を推奨
