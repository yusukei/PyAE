---
description: Documentation management rules
globs: ["docs/**/*.md", "docs/**/*.rst", "**/*.md"]
alwaysApply: true
---

# Documentation Rules

## Overview

`docs/` は Sphinx ドキュメントプロジェクト。`index.rst` の toctree がドキュメント構造の正式な定義。

## Documentation Management

1. **Keep docs clean** - 完了した計画ドキュメントは積極的に削除する
2. **Preserve reference materials** - 将来の実装に価値のある分析ドキュメントは保持する
3. **index.rst is the source of truth** - ドキュメント構造は `docs/index.rst` の toctree で管理する

## Location Restrictions

- ドキュメントファイル (.md, .rst) は `docs/` ディレクトリ内に配置する
- 新規ドキュメントは `index.rst` の toctree に追加する

## Documentation Structure

```
docs/
├── index.rst                   # トップページ（toctree で構造定義）
├── conf.py                     # Sphinx 設定
├── README.md                   # ビルド手順
│
├── getting-started/            # はじめに（ユーザー向け）
│   ├── installation.md
│   └── quickstart.md
│
├── tutorials/                  # チュートリアル（ユーザー向け）
│   ├── basic-animation.md
│   ├── layer-manipulation.md
│   ├── effect-automation.md
│   └── footage-management.md
│
├── guides/                     # ガイド（ユーザー向け + 開発者向け）
│   ├── best-practices.md       # ユーザー向けベストプラクティス
│   ├── getting-started.md      # 開発環境セットアップ
│   ├── build-notes.md          # ビルドシステム詳細
│   ├── test-guide.md           # テストガイド
│   ├── architecture.md         # PyAE アーキテクチャ
│   ├── aegp-suite-architecture.md  # AEGP Suite アーキテクチャ
│   ├── effects-plugin-development.md  # Effects Plugin 開発ガイド
│   └── testeffect.md           # TestEffect リファレンス
│
├── api/                        # API リファレンス（自動生成 + 手動 .rst）
│   ├── high-level/             # 高レベル Python API
│   └── low-level/              # 低レベル AEGP Suite API
│
└── _build/                     # ビルド出力（git 無視）
```

## Toctree Categories (index.rst)

| セクション | 対象読者 | 内容 |
|-----------|---------|------|
| はじめに | ユーザー | インストール、クイックスタート |
| チュートリアル | ユーザー | 実践的な使い方、ベストプラクティス |
| API リファレンス | ユーザー/開発者 | 高レベル/低レベル API |
| プラグイン開発ガイド | 開発者 | 環境構築、ビルド、テスト、アーキテクチャ |
| TestEffect | 開発者 | Effects Plugin 開発、TestEffect リファレンス |
