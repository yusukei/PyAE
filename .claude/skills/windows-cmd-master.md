# Windows CMD Master

Windowsコマンドプロンプト (cmd.exe) の完全マスタースキル。構文解析メカニズム、エスケープ処理の深層、ストリーム管理、変数展開のパラドックスまで、CMD の内部挙動を徹底的に理解した上でのスクリプティング支援を提供します。

## コアコンセプト

### パーサーの多段階処理を理解する

cmd.exe は入力を以下の段階で処理します:

1. **解析時展開** (Parse-Time): ブロック読み込み時に %VAR% を展開
2. **特殊文字の解釈**: リダイレクト、パイプ、セパレータの認識
3. **エスケープ処理**: ^ によるリテラル化
4. **実行時展開** (Execution-Time): 遅延展開有効時に !VAR! を展開

この順序を理解することが、予期せぬ動作を防ぐ鍵となります。

## 1. エスケープ処理の完全マスター

### 1.1 予約文字とエスケープの基本

**予約文字一覧:**
- `&` - コマンドセパレータ (無条件実行)
- `&&` - AND演算子 (前のコマンドが成功時のみ実行)
- `||` - OR演算子 (前のコマンドが失敗時のみ実行)
- `|` - パイプ (STDOUT を次のコマンドの STDIN へ)
- `<` - 入力リダイレクト
- `>` - 出力リダイレクト (上書き)
- `>>` - 出力リダイレクト (追記)
- `^` - エスケープ文字
- `%` - 変数展開識別子
- `!` - 遅延展開識別子 (EnableDelayedExpansion時)
- `"` - 文字列・パス区切り
- `()` - コマンドグループ化

### 1.2 キャレット (^) によるエスケープ

**基本原則:** 予約文字の直前に ^ を配置することで、その文字をリテラルとして扱わせる。

```batch
REM 誤り - & がコマンドセパレータと解釈される
echo R&D
REM → "R" のみ出力され、"'D' は内部コマンドまたは外部コマンドとして認識されていません" エラー

REM 正しい - & をエスケープ
echo R^&D
REM → "R&D" と出力

REM リダイレクト記号のエスケープ
echo ^<tag^>
REM → "<tag>" と出力

REM キャレット自体のエスケープ
echo This is a caret: ^^
REM → "This is a caret: ^"
```

### 1.3 行継続としてのキャレット

キャレットが**行末**にある場合、改行をエスケープし、次の行を継続として扱います。

```batch
copy c:\source\very_long_path\file.txt ^
     d:\destination\backup_folder\
REM → 内部的に1行のコマンドとして処理される

dir /s /b ^
    /a-d ^
    *.txt
REM → 長いコマンドを可読性高く記述可能
```

### 1.4 ダブルクォート内の特殊な挙動

**重要:** ダブルクォート内では、大半の予約文字が自動的にリテラル化されます。

```batch
echo "Research & Development"
REM → "Research & Development" (エラーなし、& は文字として扱われる)

echo "Redirect symbols: < > |"
REM → 正常に出力される
```

**例外:**
- `%` (変数展開) はクォート内でも有効
- `!` (遅延展開) もクォート内で有効
- クォート内では `^` のエスケープ機能自体が無効化される

```batch
REM クォート内でのエスケープ無効化
echo "Symbol ^&"
REM → "Symbol ^&" (^ がそのまま表示される)
```

### 1.5 エクスクラメーション (!) のエスケープ地獄

遅延環境変数展開が有効な場合、`!` のエスケープは極めて複雑です。

```batch
SETLOCAL EnableDelayedExpansion

REM 1回のエスケープでは不十分な場合がある
echo ^!
REM → 場合により ! が変数展開の開始と解釈される

REM 多段階パースのため、3連キャレットが必要なケースも
echo ^^^!
REM → 第1パス: ^^^ → ^, ^! → !
REM → 第2パス: ^ が残った ! をエスケープ
```

**推奨される回避策:**
- 遅延展開が不要な部分では `SETLOCAL DisableDelayedExpansion`
- `!` を含むファイル名は FOR ループ変数 (%%I) で扱う (FOR変数展開は遅延展開の影響を受けない)

## 2. ストリーム管理とリダイレクトの完全制御

### 2.1 標準ストリームの定義

| ハンドルID | 名称 | デフォルト接続先 |
|-----------|------|-----------------|
| 0 | STDIN (標準入力) | キーボード (CON) |
| 1 | STDOUT (標準出力) | コンソール画面 |
| 2 | STDERR (標準エラー出力) | コンソール画面 |

### 2.2 リダイレクト演算子

```batch
REM 上書き出力
dir > filelist.txt

REM 追記出力 (ログファイル作成に最適)
echo [%DATE% %TIME%] Backup started >> log.txt

REM 入力リダイレクト
sort < unsorted.txt

REM エラー出力のみをファイルへ
dir non_existent_file 2> error.log
```

### 2.3 ストリーム結合の順序依存性 (超重要!)

**2>&1 の意味:** "ハンドル2 (STDERR) の出力先を、ハンドル1 (STDOUT) の**現在の**出力先と同じ場所に設定せよ"

**正しい順序 (成功例):**
```batch
command > file.txt 2>&1
REM 1. > file.txt でSTDOUTがファイルに向く
REM 2. 2>&1 でSTDERRもSTDOUTの向き先(=ファイル)に向く
REM 結果: 両方の出力がファイルに保存される ✓
```

**誤った順序 (失敗例):**
```batch
command 2>&1 > file.txt
REM 1. 2>&1 が先に処理される。この時点でSTDOUTは画面に向いているため、STDERRも画面に向く
REM 2. > file.txt でSTDOUTだけがファイルに変更される
REM 結果: STDERRは画面に残り、STDOUTだけがファイルに保存される ✗
```

### 2.4 NULデバイスによる出力抑制

```batch
REM すべての出力を捨てる (静かな実行)
command > NUL 2>&1

REM エラーメッセージのみ捨てる
command 2> NUL
```

## 3. パイプラインの深層と変数スコープの罠

### 3.1 子プロセス生成による変数消失

**重大な仕様:** パイプ演算子 (|) の左右は、それぞれ**新しいcmd.exeプロセス**で実行されます。

```batch
REM 失敗例
set COUNT=0
dir /b | (
    set /a COUNT+=1
    echo Inside pipe: %COUNT%
)
echo Outside pipe: %COUNT%
REM 出力:
REM   Inside pipe: 1
REM   Outside pipe: 0  ← 変数が反映されていない!
```

**原因:** パイプ内部は子プロセスで実行されるため、変数変更は親プロセスに伝わりません。

**回避策:**
1. FOR /F でコマンド出力をキャプチャ
2. 一時ファイルを使用

### 3.2 パイプとエラー出力

デフォルトでは、パイプは**STDOUTのみ**を渡します。STDERRは画面に出力されます。

```batch
REM エラーメッセージもパイプで渡す
command 2>&1 | findstr "ERROR"
```

## 4. 変数展開のパラドックスと遅延展開

### 4.1 解析時展開の問題

cmd.exe はブロック全体を読み込んだ時点で `%VAR%` を展開します。

```batch
REM 失敗例
set VAR=0
FOR /L %%I IN (1,1,5) DO (
    set /a VAR=%VAR%+1
    echo %VAR%
)
REM 出力: 0, 0, 0, 0, 0 (すべて0!)
```

**原因:** FORブロック解析時に `%VAR%` が `0` に展開され、`set /a VAR=0+1` と `echo 0` に固定される。

### 4.2 遅延環境変数展開による解決

```batch
SETLOCAL EnableDelayedExpansion
set VAR=0
FOR /L %%I IN (1,1,5) DO (
    set /a VAR=!VAR!+1
    echo !VAR!
)
REM 出力: 1, 2, 3, 4, 5 ✓
```

**原理:** `!VAR!` は実行時 (ループが回るたび) に展開されます。

### 4.3 ファイル名に含まれる ! の罠

```batch
SETLOCAL EnableDelayedExpansion

REM ファイル名が "Hello!World.txt" の場合
FOR %%F IN (*!*.txt) DO (
    echo %%F
    REM → ファイル名の一部が欠落したり、変数と誤認される
)
```

**対策:** FOR変数 (%%F) は遅延展開の影響を受けないため、ループ内では `!` を使わず `%%F` を直接使用する。

## 5. 高度なエスケープ戦略

### 5.1 cmd /c における多重クォート

外部からコマンドを呼び出す際、cmd.exe は独自のクォート剥奪ルールを適用します。

**Microsoft仕様:** 以下の条件を**すべて**満たす場合のみクォートを保持:
1. /S スイッチが指定されていない
2. クォート文字が正確に2つだけ
3. クォート間に特殊文字 (&, <, >, (, ), ^, |) がない
4. クォート間に少なくとも1つの空白がある
5. クォート内の文字列が実行可能ファイル名である

**失敗例:**
```batch
cmd /c "C:\Program Files\App\bin.exe" "Argument"
REM クォートが4つあるため条件2を満たさず、先頭と末尾が削除される
REM 結果: C:\Program Files\App\bin.exe" "Argument (不正)
```

**成功例 (ベストプラクティス):**
```batch
cmd /c ""C:\Program Files\App\bin.exe" "Argument""
REM 全体をさらに外側のクォートで囲む
REM シェルは外側を削除し、内側の正しいコマンドが実行される ✓
```

### 5.2 変数設定時のホワイトスペース汚染対策

```batch
REM 悪い例 - 行末にスペースがあると変数値に含まれる
set PATH_ROOT=C:\App
REM → 変数値は "C:\App " となり、"%PATH_ROOT%\bin" が "C:\App \bin" となる

REM 推奨される記法
set "VAR=Value"
REM クォート自体は変数値に含まれず、余分な空白を確実に排除
```

## 6. FOR /F によるコマンド出力のキャプチャ

### 6.1 基本構文

```batch
FOR /F "tokens=*" %%I IN ('command') DO SET RESULT=%%I
REM コマンドの出力を変数に格納 (最後の行のみ)
```

### 6.2 usebackq オプション

**usebackq なし:**
- `IN ("string")` → 文字列として処理
- `IN ('command')` → コマンド実行

**usebackq あり:**
- `IN ("file path with spaces")` → ファイルとして読み込み
- `IN (`command`)` → コマンド実行 (バッククォート)

```batch
REM スペースを含むファイルパスを扱う場合は usebackq が必須
FOR /F "usebackq tokens=*" %%I IN ("C:\My Documents\data.txt") DO (
    echo %%I
)
```

### 6.3 複数行の出力を取得

```batch
SETLOCAL EnableDelayedExpansion
set OUTPUT=
FOR /F "tokens=*" %%I IN ('dir /b') DO (
    set OUTPUT=!OUTPUT!%%I,
)
echo !OUTPUT!
REM → すべてのファイル名をカンマ区切りで結合
```

## 7. 条件付き実行とワンライナー構築

### 7.1 論理演算子

```batch
REM && (AND) - 前のコマンドが成功したら実行
cd Project && code .

REM || (OR) - 前のコマンドが失敗したら実行
ping 192.168.1.1 || echo Host Unreachable

REM & (Separator) - 無条件で実行
cls & dir
```

### 7.2 グループ化とリダイレクトのスコープ

```batch
REM グループ全体の出力をリダイレクト
(echo Start & type log.txt & echo End) > report.txt
REM → 3つのコマンドすべての出力が report.txt に書き込まれる
```

## 8. 国際化とエンコーディング

### 8.1 UTF-8 対応

```batch
REM コードページをUTF-8に変更
chcp 65001

REM 注意: コンソールフォントをTrueTypeフォント
REM (MS ゴシック、Consolas など) に設定する必要がある
REM ラスターフォントではUTF-8文字が正しく描画されない
```

## 9. 生産性向上テクニック

### 9.1 ヒストリー機能

- **F7** - コマンド履歴をポップアップ一覧表示
- **F8** - 入力中の文字列に一致する履歴を前方一致検索
- **F9** - 履歴番号を指定して呼び出し

### 9.2 クリップボード連携

```batch
REM コマンド出力を直接クリップボードへ
ipconfig | clip

REM ファイル内容をクリップボードへ
type config.txt | clip
```

### 9.3 エクスプローラー連携

```batch
REM 現在のディレクトリをエクスプローラーで開く
start .

REM または
explorer .
```

## 10. 堅牢なバッチファイルテンプレート

```batch
@ECHO OFF
SETLOCAL EnableDelayedExpansion EnableExtensions

REM ===========================================
REM スクリプト名: example.bat
REM 説明:
REM Author:
REM Date:
REM ===========================================

REM スクリプト自身の情報を取得
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_NAME=%~nx0"
set "LOGFILE=%SCRIPT_DIR%log_%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%.txt"

REM メイン処理を呼び出し
CALL :Main %*
EXIT /B %ERRORLEVEL%

REM ===========================================
REM メイン処理
REM ===========================================
:Main
    CALL :LogMessage "スクリプト開始"

    REM 引数チェック
    IF "%~1"=="" (
        ECHO 使用方法: %SCRIPT_NAME% [引数]
        EXIT /B 1
    )

    REM エラーハンドリング付きコマンド実行
    XCOPY "source\*" "dest\" /E /I /Y || (
        CALL :LogMessage "エラー: コピーに失敗しました"
        EXIT /B 1
    )

    CALL :LogMessage "スクリプト完了"
    EXIT /B 0

REM ===========================================
REM ログメッセージ出力
REM ===========================================
:LogMessage
    set "MSG=%~1"
    set "TIMESTAMP=%DATE% %TIME%"
    echo [!TIMESTAMP!] !MSG!
    echo [!TIMESTAMP!] !MSG! >> "%LOGFILE%"
    EXIT /B 0

REM ===========================================
REM クリーンアップ処理 (必要に応じて)
REM ===========================================
:Cleanup
    REM 一時ファイルの削除など
    IF EXIST "%TEMP%\temp_file.tmp" DEL /F /Q "%TEMP%\temp_file.tmp"
    EXIT /B 0
```

## 11. デバッグテクニック

### 11.1 段階的デバッグ

```batch
REM エコーを有効にして各行の実行を確認
@ECHO ON
command1
command2
@ECHO OFF

REM 特定の箇所で一時停止
PAUSE

REM 変数の値を確認
SET VAR
ECHO %VAR%
```

### 11.2 エラーレベルの追跡

```batch
command
IF ERRORLEVEL 1 (
    ECHO コマンドが失敗しました。エラーコード: %ERRORLEVEL%
    EXIT /B %ERRORLEVEL%
)
```

## 重要な注意事項とベストプラクティス

1. **変数展開のタイミングを常に意識する**
   - ループ内で変数を更新する場合は `EnableDelayedExpansion` が必須
   - `%VAR%` vs `!VAR!` の使い分けを理解する

2. **パスの引用を徹底する**
   - スペースを含む可能性のあるパスは必ず `"%variable%"` で囲む
   - 変数設定時は `set "VAR=Value"` 形式を使用

3. **エラーレベルの即時チェック**
   - コマンド実行後は速やかに `ERRORLEVEL` または `|| (...)` でチェック
   - 他のコマンドを挟むとエラーレベルが上書きされる

4. **ストリームリダイレクトの順序厳守**
   - `2>&1` は必ずリダイレクト先の指定**後**に記述

5. **cmd /c の多重クォート**
   - 外部呼び出し時は `cmd /c ""command" "arg""` 形式を使用

6. **文字コードの明示**
   - 日本語を含む場合は `chcp 932` (Shift-JIS) または `chcp 65001` (UTF-8)
   - UTF-8使用時はフォント設定も確認

7. **管理者権限の確認**
   - システム変更を伴う操作は管理者権限での実行が必要
   - スクリプト冒頭で権限チェックを実装することを推奨

## このスキルを使用するタイミング

- バッチファイルの作成・編集・最適化・デバッグ
- エスケープ処理やクォート処理に関する問題解決
- ストリームリダイレクトやパイプラインの複雑な使用
- 遅延展開や変数スコープに関する問題のトラブルシューティング
- コマンドプロンプト特有の仕様や内部挙動の理解
- 堅牢で保守性の高いバッチスクリプトの設計

## 参考資料

このスキルは、Windows コマンドプロンプトの公式仕様、実証実験、および以下のトピックに関する深い理解に基づいています:

- 構文解析エンジンの多段階処理メカニズム
- 予約文字とエスケープシーケンスの完全な動作仕様
- 標準入出力ストリームの内部実装
- 変数展開のパースタイムとランタイムの違い
- cmd.exe のプロセスモデルとスコープ管理
- Microsoft公式ドキュメントおよびコミュニティの実証実験結果

---

**使用方法:**

このスキルを有効化すると、コマンドプロンプトに関するあらゆる質問や作業において、内部メカニズムを理解した上での正確で堅牢なソリューションを提供します。

例:
- "このバッチファイルのエスケープ処理を修正して"
- "FOR ループ内で変数が更新されない問題を解決して"
- "stdout と stderr を同じファイルに保存する方法は?"
- "cmd /c でスペースを含むパスを正しく渡すには?"
- "遅延展開と ! を含むファイル名の競合を解決して"
