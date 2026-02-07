# PyAEのアーキテクチャについて

PyAEの多層構造（AEGP Plugin → Python Interpreter → pybind11 → High-Level API）と、ハンドル管理・エラー変換・アンドゥシステムなど内部設計の仕組みを解説します。PyAEの拡張や低レベルAPIの理解に役立ちます。

---

## このドキュメントで学べること

- PyAEの4層アーキテクチャ
- C++とPythonの連携方法
- ハンドル管理とエラーハンドリング
- アンドゥシステムとプロパティシステム

## なぜこの知識が必要か

- **新しいAPI追加時**: C++バインディングの書き方を理解する必要がある
- **バグ調査時**: どの層で問題が発生しているか特定できる
- **低レベルAPI使用時**: `ae.sdk.*` の挙動を理解できる
- **パフォーマンス最適化時**: オーバーヘッドの原因を特定できる

## 関連するソースコード

| コンポーネント | ファイル |
|---------------|----------|
| AEGP Plugin Entry | `PyAE/src/Main.cpp` |
| Suite Manager | `PyAE/src/SuiteManager.cpp` |
| Python Bindings | `PyAE/src/PyBindings/*.cpp` |
| 高レベルAPI | `PyAE/src/PyBindings/PyComp.cpp`, `PyLayer.cpp` など |

---

## 全体構成

PyAEは、After EffectsのC++ APIとPythonの間をつなぐブリッジとして設計されています。以下の図は、各コンポーネントの関係を示しています。

```
PyAE
├── C++ プラグイン (PyAE.aex)
│   ├── AEGP Plugin Entry Point
│   ├── Python Interpreter (埋め込み)
│   └── pybind11 Bindings
├── Python Bindings
│   ├── 高レベルAPI (ae module)
│   └── 低レベルAPI (ae.sdk module)
└── Type Stubs (ae-stubs)
    └── .pyi files
```

**設計の意図**: After Effectsは外部プラグインに対してC言語ベースのAPIのみを提供しています。Pythonから直接このAPIを呼び出すことはできないため、C++でラッパーを作成し、pybind11を使ってPythonに公開しています。

**効果**: ユーザーはC++の知識がなくても、Pythonの文法でAfter Effectsを操作できます。

---

## レイヤー構造

PyAEは4層構造になっており、各層が明確な責務を持っています。この分離により、保守性と拡張性が確保されています。

### 1. AEGP Plugin Layer

After Effectsと直接やり取りする最下層です。

- After Effects Plugin API (AEGP) との通信
- プラグインのライフサイクル管理
- メニューコマンド、アイドルフック

**意図**: After Effectsの内部に最も近い層として、SDKの仕様変更に対応しやすくしています。この層を変更しても、上位層への影響を最小限に抑えられます。

### 2. Python Interpreter Layer

Python実行環境を管理する層です。

- 埋め込みPython 3.10ランタイム
- スクリプト実行環境
- モジュール管理

**意図**: After Effects内部でPythonコードを実行可能にします。埋め込みPythonを使用することで、ユーザーがシステムにPythonをインストールする必要がなく、バージョンの不整合も防げます。

### 3. pybind11 Binding Layer

C++とPythonの橋渡しをする層です。

- C++ ↔ Python 型変換
- オブジェクト寿命管理
- エラーハンドリング

**意図**: pybind11を使用することで、C++のクラスや関数を自然なPythonオブジェクトとして公開できます。手動でPython C APIを書く必要がなく、型安全性も保たれます。

### 4. High-Level API Layer

ユーザーが直接使用するPython APIです。

- Pythonicなインターフェース
- リソース自動管理
- 型ヒント完備

**意図**: AEGP APIの複雑さを隠蔽し、Pythonらしい直感的なAPIを提供します。例えば、ハンドルの管理やエラーチェックを自動化し、ユーザーはビジネスロジックに集中できます。

## 主要クラスの設計

PyAEの各クラスは、AEGP APIの「ハンドル」をラップしています。ハンドルとは、After Effects内部のオブジェクトを参照するための不透明なポインタです。

**設計原則**:
- 各クラスは対応するAEGPハンドル（`AEGP_ProjectH`, `AEGP_LayerH`など）を内部に保持
- C++側でAEGP APIを呼び出し、結果をPythonフレンドリーな形式に変換
- Python側では、ハンドルの存在を意識せずに直感的な操作が可能

### Project

Projectクラスは、After Effectsプロジェクト全体を表します。

```cpp
// C++ 実装
class PyProject {
    AEGP_ProjectH m_projectH;
    SuiteManager* m_suites;
public:
    py::list GetItems();
    PyCompItem* CreateComp(...);
};
```

```python
# Python API
class Project:
    @property
    def items(self) -> List[Item]: ...
    def create_comp(...) -> CompItem: ...
```

**意図**: C++側では`AEGP_ProjectH`ハンドルと、Suite APIへのアクセスを管理する`SuiteManager`を保持しています。Python側では、`items`プロパティでプロジェクト内のアイテムにアクセスでき、`create_comp()`で新しいコンポジションを作成できます。

### Layer

Layerクラスは、コンポジション内の各レイヤーを表します。

```cpp
// C++ 実装
class PyLayer {
    AEGP_LayerH m_layerH;
    AEGP_CompH m_compH;
public:
    void SetPosition(py::tuple pos);
    py::object GetProperty(std::string name);
};
```

```python
# Python API
class Layer:
    @property
    def position(self) -> Tuple[float, ...]: ...
    def property(self, name: str) -> Property: ...
```

**意図**: レイヤーは親コンポジションとの関係が重要なため、`m_layerH`と`m_compH`の両方を保持しています。Python側では`layer.position = (x, y)`のようなプロパティアクセスで、内部的にはSetPosition()が呼ばれます。

## ハンドル管理

After Effects AEGP APIはハンドルベースです。ハンドルは「借りた」リソースであり、使用後に適切に解放する必要があります。

**問題**: AEGP APIから取得したハンドルは、使用後に対応する`Dispose`関数を呼び出さないとメモリリークが発生します。手動管理はエラーの温床となります。

**解決策**: PyAEはRAII（Resource Acquisition Is Initialization）パターンを使用して、ハンドルの寿命を自動管理します。

```cpp
// ハンドルの取得と解放
class HandleGuard {
    AEGP_ItemH handle;
public:
    HandleGuard(AEGP_ItemH h) : handle(h) {}
    ~HandleGuard() {
        // 自動解放
        suites->itemSuite->AEGP_DisposeItem(handle);
    }
};
```

**効果**: Pythonオブジェクトがガベージコレクションされると、C++のデストラクタが呼ばれ、ハンドルが自動的に解放されます。ユーザーはリソース管理を意識する必要がありません。

## エラーハンドリング

AEGP APIは、すべての関数がエラーコード（`A_Err`）を返す設計になっています。これをPythonらしい例外処理に変換します。

**問題**: AEGP APIは成功/失敗をエラーコードで返しますが、Pythonでは例外を使うのが一般的です。エラーコードを毎回チェックするのは煩雑で、チェック漏れによるバグの原因になります。

**解決策**: C++側でエラーコードをチェックし、エラーがあればC++例外をスローします。pybind11がこれをPython例外に自動変換します。

```cpp
void CheckError(A_Err err) {
    if (err != A_Err_NONE) {
        throw std::runtime_error(GetErrorString(err));
    }
}
```

```python
try:
    layer.position = (960, 540)
except RuntimeError as e:
    print(f"Error: {e}")
```

**効果**: ユーザーは標準的なPythonのtry-except構文でエラーを処理できます。エラーコードを意識する必要がなく、エラーが発生した場所で即座に例外が発生するため、問題の特定が容易になります。

## アンドゥシステム

After Effectsはアンドゥ/リドゥ機能を持っており、PyAEからの操作もこのシステムに統合されています。

**問題**: スクリプトで100個のレイヤーを操作した場合、アンドゥグループを使わないと、Ctrl+Zを100回押す必要があります。また、アンドゥグループを開始したら必ず終了しないと、After Effectsが不安定になります。

**解決策**: Python側でコンテキストマネージャー（`with`文）を使用し、アンドゥグループの開始と終了を自動化します。例外が発生しても確実にグループが閉じられます。

```cpp
// C++でアンドゥグループを開始
suites->undoSuite->AEGP_StartUndoGroup("Set Position");
// 操作実行
suites->undoSuite->AEGP_EndUndoGroup();
```

```python
# Pythonでアンドゥグループを使用
with ae.UndoGroup("Set Position"):
    layer.position = (960, 540)
```

**効果**: `with`ブロック内のすべての操作が1回のアンドゥで元に戻せます。スクリプトの実行結果が期待と異なった場合、1回のCtrl+Zで完全に元の状態に戻せるため、安心してスクリプトを実行できます。

## プロパティシステム

After Effectsのプロパティは階層構造になっています（例: Layer → Transform → Position）。PyAEはこの階層を複数の方法でアクセスできるようにしています。

**設計意図**: エフェクトやカスタムプロパティなど、事前に名前がわからないプロパティにもアクセスできる必要があります。同時に、よく使うプロパティには簡潔にアクセスしたいという要求もあります。

```python
# 階層的アクセス（動的なプロパティ探索に最適）
transform = layer.property("Transform")
position = transform.property("Position")

# パス文字列でアクセス（一発でアクセスしたい場合）
position = layer.property("Transform/Position")

# 属性アクセス（IDEの補完が効く）
position = layer.properties.Transform.Position
```

**効果**:
- **階層的アクセス**: プロパティツリーを動的に探索する場合に便利
- **パス文字列**: 深い階層のプロパティに一発でアクセス
- **属性アクセス**: IDEの自動補完が効き、タイプミスを防げる

## スレッドセーフティ

After Effectsのプラグインアーキテクチャには、スレッドに関する重要な制約があります。

**制約**: AEGP APIはメインスレッド（UIスレッド）からのみ呼び出し可能です。バックグラウンドスレッドから呼び出すと、クラッシュや予期しない動作が発生します。

**PyAEの対応**:
- AEGP APIはメインスレッドでのみ使用可能
- Python GIL (Global Interpreter Lock) を適切に管理
- アイドルフックで非同期タスクをスケジュール

**効果**: 通常のPyAEスクリプトはメインスレッドで実行されるため、この制約を意識する必要はありません。ただし、マルチスレッドを使用する場合は、AEGP APIの呼び出しをメインスレッドに委譲する必要があります。

## パフォーマンス最適化

PyAEはC++ → Python間のオーバーヘッドを最小化するため、いくつかの最適化を行っています。

### No Cache（キャッシュなし）方針

PyAEでは **プロパティ値やオブジェクト状態のキャッシュは使用しません**。毎回AEGP APIを呼び出して最新値を取得します。

**理由**:
- ユーザーがUIで直接レイヤーを追加/削除する可能性
- 他のスクリプト/エクスプレッションが変更を加える可能性
- Undo/Redoでシーン状態が巻き戻る可能性
- キャッシュは stale data（古いデータ）の原因となり、バグの温床になる

**Python側での最適化**: プロパティオブジェクトの再利用でAPI呼び出し回数を削減できます。

```python
# ✅ 良い例: プロパティオブジェクトを変数に保持
position_prop = layer.property("Transform/Position")
for i in range(100):
    position_prop.add_keyframe(i * 0.1, (i * 10, 540))

# ❌ 悪い例: 毎回プロパティを検索
for i in range(100):
    layer.property("Transform/Position").add_keyframe(i * 0.1, (i * 10, 540))
```

### バッチ操作

**問題**: 多数のレイヤーに対して個別に操作を行うと、アンドゥ履歴が膨大になり、After Effectsの動作が重くなります。

**解決策**: 複数の操作をアンドゥグループでまとめることで、内部的な最適化が働きます。

```python
with ae.UndoGroup("Batch Update"):
    for layer in comp.layers:
        layer.position = calculate_position(layer)
```

**効果**: アンドゥ履歴が1つにまとまり、メモリ使用量が削減されます。また、After Effects内部での再描画も最適化されます。

## 将来の拡張性

PyAEのアーキテクチャは、将来的な機能拡張を見据えて設計されています。以下は検討中の機能です。

### プラグインシステム

**構想**: ユーザーが独自のPythonモジュールをPyAEのプラグインとして登録し、メニューから呼び出せるようにする機能です。

```python
# 将来的な実装
ae.register_plugin("my_tools", MyToolsModule)
```

**メリット**: チーム内で共通のツールを配布したり、スタジオ固有のワークフローを自動化したりできます。

### リモート実行

**構想**: After Effectsを外部プロセスからリモートで制御する機能です。

```python
# 将来的な実装
import ae.remote
ae.remote.connect("localhost:7000")
```

**メリット**: パイプラインツールやWebアプリケーションから、After Effectsを制御できるようになります。レンダーファームの自動化などに活用できます。

## 次のステップ

PyAEの内部設計を理解したら、以下のドキュメントに進んでください：

1. **[AEGP Suite アーキテクチャ](aegp-suite-architecture.md)** - After Effects SDKの全体像と構造

## 関連項目

- **前のドキュメント**: [テストガイド](test-guide.md)
- [高レベルAPI](../api/high-level/index.rst) - Python APIリファレンス
- [低レベルAPI](../api/low-level/index.rst) - AEGP Suite リファレンス
