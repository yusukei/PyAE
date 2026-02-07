# インストール

PyAEをAfter Effectsにインストールする方法を説明します。

## システム要件

- Adobe After Effects 2023, 2024, または 2025
- Windows 10/11 (64-bit)
- Python 3.10 (プラグインに組み込み済み)

## インストール手順

### 1. プラグインファイルの配置

PyAEプラグインを以下のディレクトリに配置します：

```
C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\
```

配置するファイル：
- `PyAE.aex` - メインプラグイン
- `PyAECore.dll` - コアライブラリ
- `Python310/` - Pythonランタイム（埋め込み済み）

### 2. After Effectsの起動

After Effectsを起動すると、PyAEプラグインが自動的に読み込まれます。

### 3. インストールの確認

Pythonスクリプトで以下を実行して確認：

```python
import ae
print(ae.version())
```

正しくインストールされている場合、バージョン番号が表示されます。

## トラブルシューティング

### プラグインが読み込まれない

- After Effectsを管理者権限で実行してみてください
- プラグインディレクトリのパスが正しいか確認してください

### Pythonモジュールが見つからない

- `Python310/`ディレクトリが`PyAE.aex`と同じ場所にあることを確認してください

## 次のステップ

インストールが完了したら、[クイックスタート](quickstart.md)に進んでください。
