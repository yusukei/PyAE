# PyAE Documentation

PyAE (Python for After Effects) の公式ドキュメントです。

## ドキュメントのビルド

### 前提条件

- **uv** がインストールされていること ([uv installation](https://github.com/astral-sh/uv))

### 必要なパッケージのインストール

```bash
cd docs
uv pip install -r requirements.txt
```

### HTMLドキュメントのビルド

```bash
# Windows（プロジェクトルートから実行）
build_docs.bat

# または直接Sphinxを実行
cd docs
uv run sphinx-build -M html . _build
```

### クリーンビルド

```bash
build_docs.bat --clean
```

### 生成されたドキュメント

ビルドが成功すると、以下の場所にHTMLドキュメントが生成されます：

```
docs/_build/html/index.html
```

ブラウザで開いて確認してください。

## ドキュメント構成

構造は `index.rst` の toctree で定義されています。`[セクション名]` は toctree での分類を示します。

```
docs/
├── index.rst                          # トップページ（toctree で構造定義）
├── conf.py                            # Sphinx設定
│
├── getting-started/                   # [はじめに] ユーザー向け
│   ├── installation.md
│   └── quickstart.md
│
├── tutorials/                         # [チュートリアル] ユーザー向け
│   ├── basic-animation.md
│   ├── layer-manipulation.md
│   ├── effect-automation.md
│   └── footage-management.md
│
├── guides/                            # ガイド
│   ├── best-practices.md              #   [チュートリアル] ベストプラクティス
│   ├── getting-started.md             #   [プラグイン開発] 開発環境セットアップ
│   ├── build-notes.md                 #   [プラグイン開発] ビルドシステム詳細
│   ├── test-guide.md                  #   [プラグイン開発] テストガイド
│   ├── architecture.md                #   [プラグイン開発] PyAEアーキテクチャ
│   ├── aegp-suite-architecture.md     #   [プラグイン開発] AEGP Suiteアーキテクチャ
│   ├── effects-plugin-development.md  #   [TestEffect] Effects Plugin開発ガイド
│   └── testeffect.md                  #   [TestEffect] TestEffectリファレンス
│
├── api/                               # [APIリファレンス]
│   ├── high-level/                    #   高レベルPython API (.rst)
│   └── low-level/                     #   低レベルAEGP Suite API (.rst)
│
└── _build/                            # ビルド出力（git無視）
```

## docstringの記述ルール

Google Style docstringを使用しています：

```python
def create_comp(
    name: str,
    width: int,
    height: int,
    duration: float = 10.0
) -> CompItem:
    """Create a new composition.

    Args:
        name: Composition name
        width: Width in pixels
        height: Height in pixels
        duration: Duration in seconds (default: 10.0)

    Returns:
        CompItem: The created composition

    Raises:
        RuntimeError: If composition creation fails

    Example:
        >>> comp = project.create_comp("MyComp", 1920, 1080)
        >>> print(comp.name)
        MyComp
    """
```

## スタブファイルへのdocstring追加

`.pyi`ファイルにdocstringを追加することで、ドキュメントが自動生成されます：

```python
# PyAE/ae-stubs/project.pyi

class Project:
    """After Effects project interface.

    Represents the currently open After Effects project.
    """

    @property
    def name(self) -> str:
        """Get the project name.

        Returns:
            str: Project name or "Untitled" if not saved
        """
        ...
```

## トラブルシューティング

### uvがインストールされていない

```bash
# uvのインストール
# Windows (PowerShell)
irm https://astral.sh/uv/install.ps1 | iex

# または公式サイトからダウンロード
# https://github.com/astral-sh/uv
```

### Sphinxがインストールされていない

```bash
cd docs
uv pip install -r requirements.txt
```

### ビルドエラー

```bash
# クリーンビルドを試す
build_docs.bat --clean

# 詳細なエラーメッセージを表示
uv run sphinx-build -M html . _build -v
```

### スタブファイルが見つからない

`conf.py`のパス設定を確認：

```python
sys.path.insert(0, str(Path(__file__).parent.parent / 'PyAE' / 'ae-stubs'))
```

## 公開

生成されたHTMLドキュメントは、GitHub Pages、Read the Docs、または任意のWebサーバーで公開できます。

### GitHub Pagesへの公開

```bash
# _build/html の内容を gh-pages ブランチにプッシュ
git checkout --orphan gh-pages
git rm -rf .
cp -r _build/html/* .
git add .
git commit -m "Update documentation"
git push origin gh-pages
```

### Read the Docsへの公開

1. https://readthedocs.org/ でプロジェクトを登録
2. GitHubリポジトリを連携
3. 自動的にビルド・公開される

## ライセンス

PyAE Documentation is licensed under the same license as PyAE.
