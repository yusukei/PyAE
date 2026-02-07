# AEGP Suiteのアーキテクチャについて

After Effects SDK の AEGP Suite は、プラグインから After Effects の内部データにアクセスするための C API 群です。このドキュメントでは、各 Suite 間の関係性と PyAE での実装アーキテクチャについて説明します。

---

## このドキュメントで学べること

- 38 の実装済み AEGP Suite の役割と階層構造
- Suite 間の依存関係（データフロー階層図）
- ハンドルの変換パターン（どの Suite で何が取得できるか）
- Python API から After Effects SDK までの呼び出し階層
- オブジェクトモデルと AEGP ハンドルの対応関係

**前提**: [PyAE のアーキテクチャ](architecture.md) を理解していると、より深く理解できます。

```{note}
このドキュメントには **Mermaid 図** が含まれています。ブラウザで正しく表示されない場合は、Mermaid 対応のマークダウンビューアを使用するか、[Mermaid Live Editor](https://mermaid.live/) にコードを貼り付けて確認してください。
```

---

## ドキュメントの目的

1. **AEGP Suite の全体像把握**: 38 の実装済み Suite の役割と依存関係を理解する
2. **ハンドル管理の理解**: 不透明ハンドルの変換・転送パターンを習得する
3. **API設計の理解**: Python バインディングの設計原則と実装パターンを把握する

## 想定読者

- PyAE の API を使用する開発者
- After Effects SDK の構造を理解したい Python バインディング開発者
- AEGP Suite の依存関係や階層構造を把握したいアーキテクト

---

## Suite 全体構造

### 実装状況サマリー

| カテゴリ | 数値 |
|---------|------|
| 実装済み Suite 数 | **38/46** |
| 実装済み関数数 | **609/719** |
| 除外 Suite 数 | 8（技術的制約により実装不可） |
| 統合テスト数 | **841** |
| テスト成功率 | **100%** |

### 実装済み Suite 一覧

#### コア Suite

| Suite 名 | バージョン | 関数数 | 役割 |
|----------|-----------|--------|------|
| `AEGP_MemorySuite` | 1 | 8/8 | メモリハンドル管理（全 Suite の基盤） |
| `AEGP_ProjSuite` | 6 | 14/14 | プロジェクト情報取得、保存 |
| `AEGP_ItemSuite` | 9 | 26/26 | アイテム（Comp/Footage/Folder）管理 |
| `AEGP_CompSuite` | 12 | 45/45 | コンポジション作成・設定、全種類のレイヤー作成 |
| `AEGP_LayerSuite` | 9 | 47/47 | レイヤー管理、時間制御、親子関係、トラックマット |
| `AEGP_UtilitySuite` | 6 | 33/33 | Undo管理、カラーパレット、スクリプト実行 |
| `AEGP_CommandSuite` | 1 | 8/8 | メニューコマンド管理 |
| `AEGP_CollectionSuite` | 2 | 6/6 | コレクション操作 |

#### プロパティ・アニメーション Suite

| Suite 名 | バージョン | 関数数 | 役割 |
|----------|-----------|--------|------|
| `AEGP_StreamSuite` | 6 | 37/37 | プロパティ値操作、Expression、ストリーム管理 |
| `AEGP_DynamicStreamSuite` | 4 | 6/6 | プロパティツリー走査（シリアライゼーション対応） |
| `AEGP_KeyframeSuite` | 5 | 22/22 | キーフレーム操作、補間、イージング、モーションパス |
| `AEGP_MarkerSuite` | 3 | 16/16 | マーカー管理、キューポイント |

#### エフェクト・マスク Suite

| Suite 名 | バージョン | 関数数 | 役割 |
|----------|-----------|--------|------|
| `AEGP_EffectSuite` | 5 | 23/23 | エフェクト管理、パラメータアクセス |
| `AEGP_MaskSuite` | 6 | 21/21 | マスク作成・管理、色・RotoBezier設定 |
| `AEGP_MaskOutlineSuite` | 3 | 12/12 | マスクアウトライン、頂点、ぼかし |

#### メディア・テキスト Suite

| Suite 名 | バージョン | 関数数 | 役割 |
|----------|-----------|--------|------|
| `AEGP_FootageSuite` | 5 | 21/21 | フッテージ管理、Solid操作、解釈ルール |
| `AEGP_TextLayerSuite` | 1 | 6/6 | テキストアウトライン取得 |
| `AEGP_TextDocumentSuite` | 1 | 2/2 | テキストドキュメント取得・設定 |
| `AEGP_CameraSuite` | 2 | 5/5 | カメラタイプ、FOV、ズーム、フォーカス距離 |
| `AEGP_LightSuite` | 3 | 4/4 | ライトタイプ、強度、円錐角度、フォールオフ |
| `AEGP_SoundDataSuite` | 1 | 6/6 | サウンドデータ（PCM/Float形式、サンプル管理） |

#### I/O Suite

| Suite 名 | バージョン | 関数数 | 役割 |
|----------|-----------|--------|------|
| `AEGP_IOInSuite` | 7 | 39/39 | 入力仕様管理（フッテージメタデータ、FPS等） |
| `AEGP_IOOutSuite` | 6 | 35/36 | 出力仕様管理（レンダー出力メタデータ、FPS等） |

#### レンダリング Suite

| Suite 名 | バージョン | 関数数 | 役割 |
|----------|-----------|--------|------|
| `AEGP_RenderQueueSuite` | 1 | 3/3 | レンダーキュー管理 |
| `AEGP_RQItemSuite` | 4 | 15/15 | レンダーキューアイテム管理 |
| `AEGP_OutputModuleSuite` | 4 | 19/19 | 出力モジュール設定、クロップ、ストレッチ |
| `AEGP_RenderSuite` | 5 | 14/14 | レンダリング実行、フレームチェックアウト |
| `AEGP_RenderOptionsSuite` | 4 | 23/23 | レンダリングオプション設定 |
| `AEGP_LayerRenderOptionsSuite` | 2 | 15/15 | レイヤーレンダーオプション |
| `AEGP_RenderAsyncManagerSuite` | 1 | 2/2 | 非同期レンダリング管理 |
| `AEGP_RenderQueueMonitorSuite` | 1 | 18/18 | レンダーキューモニタリング |

#### ワールド・カラー・数学 Suite

| Suite 名 | バージョン | 関数数 | 役割 |
|----------|-----------|--------|------|
| `AEGP_WorldSuite` | 3 | 13/13 | ワールド（フレームバッファ）管理、ブラー適用 |
| `AEGP_ColorSettingsSuite` | 6 | 19/20 | カラー設定（ICCプロファイル、OCIO、LUT） |
| `AEGP_MathSuite` | 1 | 5/5 | 行列演算（単位行列、乗算、分解） |
| `AEGP_IterateSuite` | 2 | 2/2 | マルチスレッドイテレーション処理 |

#### データ管理 Suite

| Suite 名 | バージョン | 関数数 | 役割 |
|----------|-----------|--------|------|
| `AEGP_PersistentDataSuite` | 4 | 22/22 | 永続データ保存（セクション/キー管理、各種型対応） |
| `AEGP_ItemViewSuite` | 1 | 1/1 | アイテムビュー再生時間取得 |

### 除外 Suite（技術的制約）

以下の Suite は技術的制約により Python バインディングを実装できません。

| Suite | 関数数 | 除外理由 |
|-------|--------|---------|
| `AEGP_RegisterSuite5` | 12 | C コールバック必須、初期化時のみ |
| `AEGP_TrackerSuite1` | 9 | トラッキングプラグイン専用 |
| `AEGP_CanvasSuite8` | 49 | Artisan レンダラー専用 |
| `AEGP_QueryXformSuite2` | 18 | Artisan 専用座標変換 |
| `AEGP_ArtisanUtilSuite1` | 6 | Artisan ユーティリティ |
| `AEGP_ComputeCacheSuite1` | 6 | C コールバック必須 |
| `AEGP_FIMSuite4` | 5 | C コールバック必須 |
| `AEGP_PFInterfaceSuite1` | 5 | Effect Plug-in 専用 |

> **注**: これらは After Effects の内部プラグイン開発用 API であり、一般的なスクリプティング用途では使用しません。

---

## Suite 間の関係

### データフロー階層図

以下の図は、各 Suite がどのように階層化され、依存し合っているかを示しています。

```{mermaid}
graph TD
    subgraph "基盤層"
        MEM[MemorySuite1<br/>メモリ管理]
        UTIL[UtilitySuite6<br/>ユーティリティ]
    end

    subgraph "プロジェクト層"
        PROJ[ProjSuite6<br/>プロジェクト]
        ITEM[ItemSuite9<br/>アイテム管理]
    end

    subgraph "コンポジション層"
        COMP[CompSuite12<br/>コンポジション]
        FOOT[FootageSuite5<br/>フッテージ]
        LAYER[LayerSuite9<br/>レイヤー]
    end

    subgraph "プロパティ層"
        STREAM[StreamSuite6<br/>ストリーム/プロパティ]
        KF[KeyframeSuite5<br/>キーフレーム]
        DYNSTREAM[DynamicStreamSuite4<br/>動的ストリーム]
    end

    subgraph "装飾層"
        EFFECT[EffectSuite5<br/>エフェクト]
        MASK[MaskSuite6<br/>マスク]
        MASKOUT[MaskOutlineSuite3<br/>マスクパス]
        MARKER[MarkerSuite3<br/>マーカー]
        TEXT[TextLayerSuite1<br/>テキスト]
        TEXTDOC[TextDocumentSuite1<br/>テキスト編集]
    end

    subgraph "レンダー層"
        RQ[RenderQueueSuite1<br/>レンダーキュー]
        RQITEM[RQItemSuite4<br/>RQアイテム]
        OUTMOD[OutputModuleSuite4<br/>出力モジュール]
        RENDER[RenderSuite5<br/>レンダリング]
    end

    %% 依存関係
    MEM --> PROJ & ITEM & COMP & LAYER & STREAM & EFFECT & MASK & RQ

    PROJ --> ITEM
    ITEM <--> COMP
    ITEM --> FOOT
    COMP --> LAYER

    LAYER --> STREAM
    LAYER --> EFFECT
    LAYER --> MASK
    LAYER --> TEXT

    STREAM --> KF
    STREAM --> DYNSTREAM

    EFFECT --> STREAM
    MASK --> STREAM
    MASK --> MASKOUT
    TEXT --> TEXTDOC
    TEXTDOC --> STREAM

    COMP --> RQ
    RQ --> RQITEM
    RQITEM --> OUTMOD
    RQITEM --> RENDER

    style MEM fill:#f9f,stroke:#333
    style COMP fill:#bbf,stroke:#333
    style LAYER fill:#bfb,stroke:#333
    style STREAM fill:#fbb,stroke:#333
```

**図の読み方**:
- **矢印**: 依存関係の方向を示します（例: LayerSuite は CompSuite に依存）
- **色分け**: ピンク=基盤、青=コンポジション、緑=レイヤー、赤=プロパティ
- **双方向矢印**: Item ↔ Comp のように相互変換が可能な関係

### Suite 依存グラフ（テキスト版）

Mermaid 図が表示できない環境向けのテキスト版です。

```
MemorySuite (基盤)
    ↑
    └─── すべてのSuite

ProjSuite ──→ ItemSuite ──→ CompSuite ←→ FootageSuite
                              │
                              ↓
                          LayerSuite
                              │
              ┌───────────────┼───────────────┐
              ↓               ↓               ↓
        EffectSuite      MaskSuite      TextLayerSuite
              │               │               │
              ↓               ↓               ↓
        StreamSuite ←─────────┴───────────────┘
              │
              ↓
        KeyframeSuite
```

### オブジェクト階層構造

以下のクラス図は、PyAE のオブジェクトモデルと AEGP ハンドルの対応関係を示しています。

```{mermaid}
classDiagram
    class Project {
        +ProjectH handle
        +name: string
        +path: string
        +items: Item[]
    }

    class Item {
        +ItemH handle
        +name: string
        +type: ItemType
        +duration: A_Time
        +dimensions: (w, h)
    }

    class Comp {
        +CompH handle
        +framerate: float
        +bgColor: RGBA
        +layers: Layer[]
    }

    class Footage {
        +filePath: string
        +source: any
    }

    class Folder {
        +children: Item[]
    }

    class Layer {
        +LayerH handle
        +name: string
        +index: int
        +parent: Layer
        +sourceItem: Item
        +effects: Effect[]
        +masks: Mask[]
        +streams: Stream[]
    }

    class Stream {
        +StreamRefH handle
        +name: string
        +type: StreamType
        +value: StreamValue
        +keyframes: Keyframe[]
    }

    class Effect {
        +EffectRefH handle
        +matchName: string
        +enabled: bool
        +properties: Stream[]
    }

    class Mask {
        +MaskRefH handle
        +mode: MaskMode
        +inverted: bool
        +outline: MaskOutline
    }

    class Keyframe {
        +index: int
        +time: A_Time
        +value: any
        +flags: KeyframeFlags
    }

    Project "1" --> "*" Item : contains
    Item <|-- Comp : is-a
    Item <|-- Footage : is-a
    Item <|-- Folder : is-a
    Comp "1" --> "*" Layer : contains
    Comp "1" <--> "1" Item : bidirectional
    Layer "1" --> "*" Effect : has
    Layer "1" --> "*" Mask : has
    Layer "1" --> "*" Stream : has
    Layer "0..1" --> "0..1" Layer : parent-child
    Layer --> Item : sourceItem
    Effect "1" --> "*" Stream : properties
    Mask "1" --> "*" Stream : properties
    Stream "1" --> "*" Keyframe : contains
```

**設計のポイント**:
- 各クラスは対応する AEGP ハンドル（`ProjectH`, `LayerH` など）を内部に保持
- `Comp` は `Item` を継承し、`ItemH` と `CompH` の双方向変換が可能
- `Layer` は複数の `Effect`, `Mask`, `Stream` を持ち、それぞれが Suite を通じて操作される

---

## ハンドル変換

### ハンドル変換フロー図

以下の図は、各種ハンドル型と、それらを取得・変換するための Suite の関係を示しています。

```{mermaid}
flowchart LR
    subgraph "ハンドル型"
        PH[ProjectH]
        IH[ItemH]
        CH[CompH]
        LH[LayerH]
        SH[StreamRefH]
        EH[EffectRefH]
        MH[MaskRefH]
    end

    subgraph "Suite"
        PS[ProjSuite]
        IS[ItemSuite]
        CS[CompSuite]
        LS[LayerSuite]
        SS[StreamSuite]
        ES[EffectSuite]
        MS[MaskSuite]
    end

    PS -->|GetProjectByIndex| PH
    IS -->|GetActiveItem| IH

    IH -->|GetCompFromItem| CH
    CH -->|GetItemFromComp| IH

    CH -->|GetCompLayerByIndex| LH
    LH -->|GetLayerSourceItem| IH

    LH -->|GetLayerEffectByIndex| EH
    LH -->|GetLayerMaskByIndex| MH

    EH & MH & LH -->|GetNewStreamRef| SH

    style IH fill:#ffd,stroke:#333
    style CH fill:#dff,stroke:#333
    style LH fill:#dfd,stroke:#333
```

**図の読み方**:
- **左側のボックス**: AEGP ハンドル型（不透明ポインタ）
- **右側のボックス**: ハンドルを取得するための Suite
- **矢印のラベル**: 使用する Suite メソッド名

### 主要な変換パターン

| 変換元 | 変換先 | Suite | メソッド |
|--------|--------|-------|----------|
| index | ProjectH | ProjSuite | `AEGP_GetProjectByIndex(index)` |
| - | ItemH | ItemSuite | `AEGP_GetActiveItem()` |
| ItemH | CompH | CompSuite | `AEGP_GetCompFromItem(itemH)` |
| CompH | ItemH | CompSuite | `AEGP_GetItemFromComp(compH)` |
| CompH + index | LayerH | LayerSuite | `AEGP_GetCompLayerByIndex(compH, index)` |
| LayerH | ItemH | LayerSuite | `AEGP_GetLayerSourceItem(layerH)` |
| LayerH | LayerH | LayerSuite | `AEGP_GetLayerParent(layerH)` |
| LayerH + index | EffectRefH | EffectSuite | `AEGP_GetLayerEffectByIndex(plugin_id, layerH, index)` |
| LayerH + index | MaskRefH | MaskSuite | `AEGP_GetLayerMaskByIndex(layerH, index)` |

### 双方向変換

`ItemH` と `CompH` は双方向変換が可能です。これにより、コンポジションをアイテムとして扱うコンテキストと、コンポジション固有の操作を行うコンテキストを柔軟に切り替えられます。

### Python へのハンドル転送

C++ の不透明ハンドル（`void*`）は Python に `uintptr_t` として転送されます。

```cpp
// C++ → Python
uintptr_t handle_ptr = reinterpret_cast<uintptr_t>(compH);
return handle_ptr;

// Python → C++
AEGP_CompH compH = reinterpret_cast<AEGP_CompH>(handle_ptr);
```

### AEGP_MemHandle の仕組み

After Effects SDK では、文字列やバイナリデータは直接ポインタで返されず、`AEGP_MemHandle` という不透明ハンドルで返されます。

**MemHandle を使う理由:**

| 理由 | 説明 |
|------|------|
| プラグイン帰属 | メモリがどのプラグインに属するか追跡できる |
| 自動解放 | プラグインアンロード時に未解放メモリを回収 |
| サイズ管理 | `AEGP_GetMemHandleSize` でサイズ取得可能 |
| ロック制御 | 複数箇所からのアクセス競合を防止 |

**使用パターン:**

```cpp
AEGP_MemHandle nameH = nullptr;
suites.layerSuite->AEGP_GetLayerName(plugin_id, layerH, &nameH, nullptr);

// Lock してポインタ取得
A_UTF16Char* namePtr = nullptr;
suites.memorySuite->AEGP_LockMemHandle(nameH, (void**)&namePtr);
std::string name = UTF16ToUTF8(namePtr);

// Unlock & Free（必須）
suites.memorySuite->AEGP_UnlockMemHandle(nameH);
suites.memorySuite->AEGP_FreeMemHandle(nameH);
```

**RAII パターンでの自動管理:**

PyAE では `ScopedMemHandle` / `ScopedMemLock` で自動解放します。

```cpp
AEGP_MemHandle nameH = nullptr;
suites.layerSuite->AEGP_GetLayerName(plugin_id, layerH, &nameH, nullptr);

// ScopedMemHandle: スコープ終了時に自動 Free
PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, nameH);

// ScopedMemLock: スコープ終了時に自動 Unlock
{
    PyAE::ScopedMemLock lock(suites.memorySuite, nameH);
    A_UTF16Char* ptr = lock.Get<A_UTF16Char>();
    // ptr を使用
}  // ここで自動 Unlock
```

### plugin_id の役割

多くの Suite メソッドは `plugin_id` パラメータを必要とします。

```cpp
AEGP_GetItemName(plugin_id, itemH, &nameH);
AEGP_GetLayerName(plugin_id, layerH, &nameH, &sourceNameH);
AEGP_GetLayerEffectByIndex(plugin_id, layerH, index, &effectH);
```

**目的:**
1. **メモリ帰属**: メモリハンドルをプラグインに紐付け
2. **権限管理**: プラグインの権限レベルを確認
3. **リソース追跡**: プラグイン終了時のリソース解放

---

## API 設計パターン

### API 階層構造

以下の図は、Python API から After Effects SDK までの呼び出し階層を示しています。

```{mermaid}
graph TB
    subgraph "Python High-Level API"
        PY_PROJ[ae.Project]
        PY_COMP[ae.Comp]
        PY_LAYER[ae.Layer]
        PY_EFFECT[ae.Effect]
        PY_PROP[ae.Property]
    end

    subgraph "Python Low-Level API"
        SDK[ae.sdk.*<br/>AEGP_* 関数]
    end

    subgraph "C++ Binding Layer"
        BIND[PyBindings<br/>pybind11]
    end

    subgraph "C++ Suite Layer"
        SUITE[SuiteManager<br/>Suite Cache]
    end

    subgraph "After Effects SDK"
        AE[AEGP Suites]
    end

    PY_PROJ & PY_COMP & PY_LAYER --> SDK
    SDK --> BIND
    BIND --> SUITE
    SUITE --> AE

    style PY_PROJ fill:#9f9,stroke:#333
    style PY_COMP fill:#9f9,stroke:#333
    style PY_LAYER fill:#9f9,stroke:#333
```

**各層の役割**:
- **High-Level API**: ユーザーが直接使用する Pythonic なインターフェース
- **Low-Level API**: AEGP Suite 関数への直接アクセス（`ae.sdk.*`）
- **C++ Binding Layer**: pybind11 による C++ ↔ Python の型変換
- **Suite Layer**: Suite ポインタのキャッシュとスレッドセーフな管理
- **AEGP Suites**: After Effects SDK が提供するネイティブ C API

### 設計原則

#### ハンドルのカプセル化

```python
# 低レベル API
comp_handle = ae.sdk.AEGP_GetCompFromItem(item_handle)
num_layers = ae.sdk.AEGP_GetCompNumLayers(comp_handle)

# 高レベル API
comp = ae.Comp(item)  # 内部でハンドル変換
layers = comp.layers  # 自動的にレイヤーオブジェクト群を返す
```

#### 常に最新データを取得（No Cache）

After Effects ではユーザーが UI から操作したり、他のスクリプトが変更を加えたり、Undo/Redo が発生する可能性があるため、**キャッシュは使用しません**。

**理由:**
- ユーザーが UI で直接レイヤーを追加/削除する可能性
- 他のスクリプト/エクスプレッションが変更を加える可能性
- Undo/Redo でシーン状態が巻き戻る可能性
- キャッシュは stale data（古いデータ）の原因となり、バグの温床になる

#### 統一的なプロパティアクセス

```python
# Suite 間の違いを吸収
layer.transform.position.value  # StreamSuite
layer.transform.position.keyframes  # KeyframeSuite
layer.effects[0].opacity.value  # EffectSuite → StreamSuite

# 全て同じ Property インターフェースで統一
class Property:
    value: Any
    keyframes: list[Keyframe]

    def set_value(self, value, time=None): ...
    def add_keyframe(self, time, value): ...
```

#### クラス階層

```
ae.Project
├── ae.Comp(ae.Item)
│   ├── layers: list[ae.Layer]
│   ├── markers: list[ae.Marker]
│   └── work_area: WorkArea
│
├── ae.Footage(ae.Item)
│   └── source: FootageSource
│
└── ae.Folder(ae.Item)
    └── children: list[ae.Item]

ae.Layer
├── transform: TransformGroup
│   ├── position: ae.Property
│   ├── scale: ae.Property
│   ├── rotation: ae.Property
│   └── ...
├── effects: list[ae.Effect]
├── masks: list[ae.Mask]
├── markers: list[ae.Marker]
└── parent: ae.Layer | None

ae.Effect
├── enabled: bool
├── match_name: str
└── properties: PropertyGroup

ae.Mask
├── mode: MaskMode
├── path: ae.Property  # MaskOutlineSuite
├── opacity: ae.Property
└── feather: ae.Property

ae.Property
├── value: Any
├── expression: str | None
├── keyframes: list[ae.Keyframe]
└── can_vary_over_time: bool
```

#### エラーハンドリング

**低レベル API**: A_Err コードを例外に変換

```python
try:
    comp_h = ae.sdk.AEGP_GetCompFromItem(item_h)
except ae.SDKError as e:
    print(f"Error code: {e.error_code}")  # A_Err_INVALID_INDEX など
    print(f"Message: {e.message}")
```

**高レベル API**: ドメイン例外に変換

```python
# 例外階層
class PyAEError(Exception):
    """PyAE 基底例外"""
    pass

class LayerError(PyAEError):
    """レイヤー関連エラー"""
    pass

class LayerNotFoundError(LayerError):
    """レイヤーが見つからない（削除済み）"""
    pass
```

---

## Suite 操作のベストプラクティス

### 安全な Suite 使用

```cpp
// Suite の存在確認
const auto& suites = PluginState::Instance().GetSuites();
if (!suites.compSuite) {
    throw std::runtime_error("CompSuite not available");
}

// エラーチェック
A_Err err = suites.compSuite->AEGP_GetCompFromItem(itemH, &compH);
if (err != A_Err_NONE) {
    throw std::runtime_error("AEGP_GetCompFromItem failed: " + std::to_string(err));
}
```

### メモリ管理（RAII パターン）

```cpp
// ScopedMemHandle で自動解放
AEGP_MemHandle nameH = nullptr;
suites.layerSuite->AEGP_GetLayerName(plugin_id, layerH, &nameH, nullptr);
PyAE::ScopedMemHandle scoped(plugin_id, suites.memorySuite, nameH);
// スコープ終了時に自動解放
```

### ハンドル有効性の確認

```python
class Layer:
    def is_valid(self) -> bool:
        """ハンドルが有効かチェック"""
        try:
            _ = ae.sdk.AEGP_GetLayerIndex(self._handle)
            return True
        except:
            return False
```

### スレッドセーフ性

`SuiteManager` は `std::shared_mutex` を使用してスレッドセーフに実装されています。

```cpp
class SuiteManager {
    mutable std::shared_mutex m_mutex;
    SuiteCache m_suites;

public:
    const SuiteCache& GetSuites() const {
        std::shared_lock lock(m_mutex);  // 読み取りは複数スレッド同時OK
        return m_suites;
    }

    void AcquireSuites() {
        std::unique_lock lock(m_mutex);  // 書き込みは排他ロック
        // ...
    }
};
```

:::{warning}
`SuiteManager` 経由での Suite ポインタ取得はスレッドセーフですが、**個々の Suite 関数自体は After Effects のメインスレッドからのみ呼び出し可能**です。

```python
# NG: バックグラウンドスレッドからの呼び出し
import threading
def worker():
    layer.name  # クラッシュの可能性

# OK: メインスレッドから呼び出す
# After Effects のコールバック内で実行
```
:::

---

## 実装ファイルマッピング

| 概念 | C++ 実装 | Python バインディング |
|------|----------|----------------------|
| Suite 管理 | `PyAE/src/SuiteManager.cpp` | - |
| Project | `PyAE/src/PyBindings/SDK/ProjSuite.cpp` | `ae.sdk.AEGP_GetProject*` |
| Item | `PyAE/src/PyBindings/SDK/ItemSuite.cpp` | `ae.sdk.AEGP_GetItem*` |
| Comp | `PyAE/src/PyBindings/SDK/CompSuite.cpp` | `ae.sdk.AEGP_GetComp*` |
| Layer | `PyAE/src/PyBindings/SDK/LayerSuite.cpp` | `ae.sdk.AEGP_GetLayer*` |
| Stream | `PyAE/src/PyBindings/SDK/StreamSuite.cpp` | `ae.sdk.AEGP_GetStream*` |
| Keyframe | `PyAE/src/PyBindings/SDK/KeyframeSuite.cpp` | `ae.sdk.AEGP_*Keyframe*` |
| Effect | `PyAE/src/PyBindings/SDK/EffectSuite.cpp` | `ae.sdk.AEGP_*Effect*` |
| Mask | `PyAE/src/PyBindings/SDK/MaskSuite.cpp` | `ae.sdk.AEGP_*Mask*` |

---

## まとめ

### Suite アーキテクチャの特徴

1. **階層的構造**: Project → Item → Comp → Layer → Stream/Effect/Mask
2. **不透明ハンドル**: 内部実装を隠蔽し、API の安定性を確保
3. **Suite 分離**: 機能ごとに Suite が分かれ、必要なものだけ取得可能
4. **双方向変換**: Item ↔ Comp のような相互変換が可能

### PyAE API 設計のポイント

1. **ハンドルの隠蔽**: ユーザーは `uintptr_t` を意識しない
2. **Suite 選択の自動化**: オブジェクトが適切な Suite を選択
3. **統一的インターフェース**: Property クラスで全プロパティを統一
4. **メモリ管理の自動化**: Python オブジェクト破棄時に解放

### 実装完了状況

| 項目 | 状況 |
|------|------|
| 実装済み Suite | 38/46（83%） |
| 実装済み関数 | 609/719（85%） |
| テスト成功率 | 100%（841テスト） |
| メモリリーク | 0件 |

技術的に実装可能なすべての AEGP Suite の実装が完了しています。

---

## 関連項目

- **前のドキュメント**: [アーキテクチャ](architecture.md)
- [低レベルAPI](../api/low-level/index.rst) - AEGP Suite の詳細リファレンス
- [高レベルAPI](../api/high-level/index.rst) - Python APIリファレンス

---

これでプラグイン開発ガイドは完了です。実際の開発では、[低レベルAPI](../api/low-level/index.rst) と [高レベルAPI](../api/high-level/index.rst) のリファレンスを参照してください。
