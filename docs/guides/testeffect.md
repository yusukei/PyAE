# TestEffect リファレンス

TestEffect は、エフェクトパラメータのシリアライズ・デシリアライズの開発とテストを目的とした専用プラグインです。
エフェクトとしての画像処理は行わず（パススルー）、各パラメータ型の読み書きと Arbitrary Data のシリアライズ・デシリアライズが正しく動作することを検証します。

---

## このドキュメントで学べること

- TestEffect のパラメータ一覧と各型のデフォルト値
- Arbitrary Data（カスタムバイナリデータ）の構造と Python での扱い方
- TestEffect を使ったパラメータ操作の実例
- ビルドとデプロイの方法

**前提**: PyAE プラグインがインストール済みであること。[テストガイド](test-guide.md) を参照。

:::{tip}
Effects Plugin の C++ 実装の詳細（PiPL、エントリポイント、ハンドル型、Arbitrary Data コールバック等）については [Effects Plugin 開発ガイド](effects-plugin-development.md) を参照してください。
:::

---

## 基本情報

| 項目 | 値 |
|------|-----|
| マッチ名 | `PYAE TestEffect` |
| 表示名 | `PyAE Test Effect` |
| カテゴリ | `PyAE Test` |
| バージョン | 1.0.0 (Development) |
| ソースディレクトリ | `TestEffect/` |

```python
import ae

comp = ae.Comp.get_active()
layer = comp.layers[0]

# TestEffect を追加
effect = layer.add_effect("PYAE TestEffect")
print(f"name: {effect.name}, match_name: {effect.match_name}")
```

---

## パラメータ一覧

TestEffect は 12 個のパラメータを持ち、主要な AE パラメータ型を網羅しています。

| Index | Disk ID | 名前 | 型 | デフォルト値 | 備考 |
|-------|---------|------|-----|-------------|------|
| 0 | `TESTFX_INPUT` | Input Layer | `PF_Param_LAYER` | - | AE が自動追加 |
| 1 | `TESTFX_SLIDER_INT` | Integer Slider | `PF_Param_SLIDER` | 50 | 範囲: 0〜100 |
| 2 | `TESTFX_SLIDER_FLOAT` | Float Slider | `PF_Param_FLOAT_SLIDER` | 3.14159 | 範囲: 0.0〜100.0 |
| 3 | `TESTFX_SLIDER_PERCENT` | Percent | `PF_Param_FIX_SLIDER` | 75.0 | 範囲: 0〜100（16.16固定小数点） |
| 4 | `TESTFX_CHECKBOX` | Checkbox | `PF_Param_CHECKBOX` | TRUE | |
| 5 | `TESTFX_ANGLE` | Angle | `PF_Param_ANGLE` | 45.0° | |
| 6 | `TESTFX_COLOR` | Color | `PF_Param_COLOR` | RGB(255, 128, 64) | |
| 7 | `TESTFX_POINT_2D` | Point 2D | `PF_Param_POINT` | (50, 50) | |
| 8 | `TESTFX_POPUP` | Popup Menu | `PF_Param_POPUP` | Choice 2 | 5 選択肢 |
| 9 | `TESTFX_POINT_3D` | Point 3D | `PF_Param_POINT_3D` | (100, 100, 100) | |
| 10 | `TESTFX_BUTTON` | Test Action | `PF_Param_BUTTON` | - | クリックでメッセージ表示 |
| 11 | `TESTFX_ARBITRARY` | Custom Data | `PF_Param_ARBITRARY_DATA` | 後述 | バイナリデータ（40バイト） |

---

## Arbitrary Data（カスタムバイナリデータ）

TestEffect の主要なテスト対象である Arbitrary Data パラメータは、エフェクトプラグインが独自に定義するバイナリデータ型です。

### C++ 構造体定義

```cpp
typedef struct {
    A_FpLong    values[4];  // 4 doubles (32 bytes)
    A_long      mode;       // Integer mode selector (4 bytes)
    A_Boolean   enabled;    // Boolean flag (1 byte)
    A_char      pad[3];     // Alignment padding (3 bytes)
} TestFX_ArbData;           // Total: 40 bytes
```

### デフォルト値

| フィールド | デフォルト値 |
|-----------|------------|
| `values[0]` | 1.0 |
| `values[1]` | 2.0 |
| `values[2]` | 3.0 |
| `values[3]` | 4.0 |
| `mode` | 1 |
| `enabled` | TRUE (1) |

### バイナリフォーマット

Python の `struct` モジュールで読み書きする場合のフォーマット文字列:

```python
import struct

ARB_FORMAT = '<4diB3x'
# <    = little-endian
# 4d   = 4 doubles (各 8 bytes)
# i    = signed int32 (4 bytes)
# B    = unsigned byte (1 byte)
# 3x   = 3 bytes padding

ARB_SIZE = struct.calcsize(ARB_FORMAT)  # 40 bytes
```

### Python での読み書き

```python
import struct
import ae

ARB_FORMAT = '<4diB3x'

comp = ae.Comp.get_active()
layer = comp.layers[0]
effect = layer.add_effect("PYAE TestEffect")

# Arbitrary Data パラメータを取得
param = effect.property("Custom Data")

# 値の読み取り（bytes として返る）
raw = param.value
data = struct.unpack(ARB_FORMAT, raw)
# data = (v0, v1, v2, v3, mode, enabled)

print(f"values: {data[0]}, {data[1]}, {data[2]}, {data[3]}")
print(f"mode: {data[4]}, enabled: {data[5]}")

# 値の書き込み
new_data = struct.pack(ARB_FORMAT, 10.0, 20.0, 30.0, 40.0, 2, 0)
param.value = new_data

# 読み戻して確認
readback = struct.unpack(ARB_FORMAT, param.value)
print(f"updated values: {readback}")
```

### Arbitrary Data コールバック

TestEffect は以下の Arbitrary Data コールバックを実装しています:

| コールバック | 機能 |
|------------|------|
| `NEW_FUNC` | デフォルト値で新規インスタンス作成 |
| `DISPOSE_FUNC` | メモリ解放 |
| `COPY_FUNC` | データ複製 |
| `FLAT_SIZE_FUNC` | シリアライズサイズ取得（40 bytes） |
| `FLATTEN_FUNC` | バイナリにシリアライズ |
| `UNFLATTEN_FUNC` | バイナリからデシリアライズ |
| `INTERP_FUNC` | キーフレーム補間（double は線形、int/bool はステップ） |
| `COMPARE_FUNC` | 2 つのインスタンスを比較 |
| `PRINT_FUNC` | テキスト表現を生成（`V0=1.0 V1=2.0 ...`） |

---

## 使用例

### 全パラメータの列挙

```python
import ae

comp = ae.Comp.get_active()
layer = comp.layers[0]
effect = layer.add_effect("PYAE TestEffect")

for i in range(effect.num_properties):
    prop = effect.property_by_index(i)
    print(f"[{i}] {prop.name}: {prop.value}")
```

### JSON エクスポート

```python
import ae
import struct
import json

effect = layer.effects[0]  # TestEffect

params_data = []
for i in range(effect.num_properties):
    prop = effect.property_by_index(i)
    if prop and prop.valid:
        val = prop.value
        if isinstance(val, bytes):
            bdata = val.hex()
        elif isinstance(val, (int, float)):
            bdata = struct.pack('>d', float(val)).hex()
        elif isinstance(val, (list, tuple)):
            bdata = b''.join(struct.pack('>d', float(v)) for v in val).hex()
        else:
            bdata = str(val) if val is not None else ""
        params_data.append({
            "name": prop.name,
            "index": prop.index,
            "bdata": bdata,
        })

export = {
    "effect": {
        "name": effect.name,
        "match_name": effect.match_name,
        "num_params": effect.num_properties,
        "params": params_data,
    }
}
print(json.dumps(export, indent=2, ensure_ascii=False))
```

---

## ビルドとデプロイ

```bash
./build.bat --project TestEffect   # ビルド
./setup.bat                        # デプロイ（PyAE + TestEffect）
```

デプロイ先: `C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\TestEffect\`

ビルドシステムの詳細は [ビルドノート](build-notes.md) 、CMake構成やPiPLの詳細は [Effects Plugin 開発ガイド](effects-plugin-development.md) を参照してください。

---

## トラブルシューティング

TestEffect の開発中に遭遇したエラーと対処法をまとめます。
AE エフェクトプラグイン全般に適用できる知見です。

### エラー 5027 (247): AEGP_StreamType_NO_DATA

```
After Effects エラー: 内部確認に失敗しました。
{Cannot get AEGP_StreamValue2 for AEGP_StreamType_NO_DATA streams} ( 5027 , 247 )
```

**原因**: データを持たない構造的ストリーム（プロパティグループ、トピックヘッダー等）に対して `AEGP_GetNewStreamValue` を呼び出した。

AE では「トランスフォーム」のようなプロパティグループも `AEGP_StreamRefH` を持ちますが、型は `AEGP_StreamType_NO_DATA` であり、値の取得は禁止されています。
エフェクトパラメータを走査する場合、トピック（グループ）ヘッダーも含まれるため、型チェックなしにすべてのストリームから値を取得しようとするとこのエラーが発生します。

**対処法**: 値を取得する前に `AEGP_GetStreamType` で型を確認する。

```cpp
AEGP_StreamType stream_type = AEGP_StreamType_NO_DATA;
err = stream_suite->AEGP_GetStreamType(stream_handle, &stream_type);

// NO_DATA 型は値を持たないのでスキップ
if (!err && stream_type != AEGP_StreamType_NO_DATA) {
    // 安全に値を取得できる
    err = stream_suite->AEGP_GetNewStreamValue(
        plugin_id, stream_handle, AEGP_LTimeMode_LayerTime,
        &time, TRUE, &stream_val);
    // ... 使用後は必ず AEGP_DisposeStreamValue で解放
}

// ストリームハンドルは型に関係なく必ず解放
stream_suite->AEGP_DisposeStream(stream_handle);
```

**主な `AEGP_StreamType` と値の有無**:

| ストリーム型 | 値の取得 | 代表的な用途 |
|------------|---------|------------|
| `NO_DATA` | 不可 | プロパティグループ、トピックヘッダー |
| `OneD` | 可 | 不透明度、回転、スライダー |
| `TwoD_SPATIAL` | 可 | 位置、アンカーポイント |
| `ThreeD_SPATIAL` | 可 | 3D レイヤーの位置 |
| `COLOR` | 可 | 色 |
| `ARB` | 可 | Arbitrary Data（カーブ等） |

**メモリ管理の注意点**: `AEGP_DisposeStream(streamH)` は `NO_DATA` であっても必ず呼ぶ必要があります。
値の解放 (`AEGP_DisposeStreamValue`) は値の取得に成功した場合のみ呼びます。

---

### エラー 25 :: 37: Custom UI フラグの不整合

```
After Effects エラー: effect: no custom ui outflag, but param has ui_width or
ui_height or PF_PUI_TOPIC/CONTROL flags. ( 25 :: 37 )
```

`PF_OutFlag_CUSTOM_UI` を宣言していないのに、パラメータに Custom UI 関連の設定がある場合に発生します。TestEffect では `PF_PUI_NO_ECW_UI` + `ui_width=0, ui_height=0` で解決しています。

OutFlags の整合性、PUI フラグの選択肢、Arbitrary Data パラメータの注意点については [Effects Plugin 開発ガイド](effects-plugin-development.md) を参照してください。

---

## 関連項目

- [Effect API リファレンス](../api/high-level/effect.rst) - エフェクトクラス
- [Property API リファレンス](../api/high-level/property.rst) - プロパティクラス
- [テストガイド](test-guide.md) - テストの実行方法
- [エフェクト自動化チュートリアル](../tutorials/effect-automation.md) - エフェクト操作の実例
