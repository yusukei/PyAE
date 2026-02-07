# Effects Plugin 開発ガイド

本ドキュメントは、AE SDK を使用した Effects Plugin（.aex）の開発方法をまとめたものです。
TestEffect プラグインの開発経験から得られた知見を含みます。

:::{note}
TestEffect のパラメータ一覧や Python からの使い方については [TestEffect リファレンス](testeffect.md) を参照してください。
:::

---

## 1. アーキテクチャ概要

### 1.1 Effects Plugin とは

After Effects の Effects Plugin は、レイヤーに適用されるエフェクト処理を提供する DLL（.aex）です。

```
AE Host <--> .aex Plugin (DLL)
  |             |
  |  PF_Cmd_*   |
  | ----------> | EffectMain()
  |             |   +-- PF_Cmd_GLOBAL_SETUP
  |             |   +-- PF_Cmd_PARAMS_SETUP
  |             |   +-- PF_Cmd_RENDER
  |             |   +-- PF_Cmd_EVENT
  |             |   +-- PF_Cmd_ARBITRARY_CALLBACK
  |             |   +-- etc.
```

### 1.2 プラグインの構成要素

| 要素 | 説明 |
|------|------|
| PiPL リソース | プラグインの静的メタデータ（名前、カテゴリ、フラグ等） |
| EffectMain 関数 | 全コマンドのディスパッチャー |
| パラメータ定義 | PF_Cmd_PARAMS_SETUP で登録 |
| レンダリング処理 | PF_Cmd_RENDER で画像処理 |
| Arbitrary Data | カスタムデータ型（オプション） |

### 1.3 ファイル構成（TestEffect の例）

```
TestEffect/
+-- TestEffect.h                  # ヘッダ（パラメータ enum、定数、構造体）
+-- TestEffect.cpp                # メインロジック（EffectMain、ParamsSetup、Render）
+-- TestEffect_Strings.h          # 文字列ID enum
+-- TestEffect_Strings.cpp        # 文字列テーブル（GetStringPtr 実装）
+-- TestEffect_Arb_Handler.cpp    # Arbitrary Data コールバック
+-- TestEffectPiPL.r              # PiPL リソース定義（.r 形式）
+-- resources/
|   +-- TestEffectPiPL.rc         # フォールバック用手書き PiPL（PiPLTool 不在時）
+-- compile_pipl.cmake            # PiPL 3段階コンパイルスクリプト
+-- CMakeLists.txt                # ビルド定義
```

---

## 2. PiPL リソースシステム

### 2.1 PiPL とは

PiPL（Plug-in Property List）は、AE がプラグインを読み込む前に静的に取得するメタデータです。
プラグインの名前、カテゴリ、エントリポイント、OutFlags 等を含みます。

### 2.2 PiPL の .r ファイル形式

```text
#include "AEConfig.h"
#include "AE_EffectVers.h"
#ifndef AE_OS_WIN
    #include "AE_General.r"
#endif

resource 'PiPL' (16000) {
    {
        Kind { AEEffect },

        Name { "PyAE Test Effect" },

        Category { "PyAE Test" },

        // Platform-specific code entry point
#ifdef AE_OS_WIN
    #if defined(AE_PROC_INTELx64)
        CodeWin64X86 {"EffectMain"},
    #elif defined(AE_PROC_ARM64)
        CodeWinARM64 {"EffectMain"},
    #endif
#elif defined(AE_OS_MAC)
        CodeMacIntel64 {"EffectMain"},
        CodeMacARM64 {"EffectMain"},
#endif

        AE_PiPL_Version { 2, 0 },

        AE_Effect_Spec_Version {
            PF_PLUG_IN_VERSION,     // = 13
            PF_PLUG_IN_SUBVERS      // = 29
        },

        AE_Effect_Version {
            524289    // PF_VERSION(1,0,0,PF_Stage_DEVELOP,1) = 0x80001
        },

        AE_Effect_Info_Flags { 0 },

        // CRITICAL: C++ GlobalSetup の out_flags と一致させること
        AE_Effect_Global_OutFlags {
            0x2000000    // PF_OutFlag_DEEP_COLOR_AWARE
        },

        AE_Effect_Global_OutFlags_2 {
            0x0          // PF_OutFlag2_NONE
        },

        AE_Effect_Match_Name { "PYAE TestEffect" },

        AE_Reserved_Info { 0 },

        AE_Effect_Support_URL { "https://github.com/anthropics/pyae" }
    }
};
```

### 2.3 PiPLTool 3段階パイプライン

PiPL の `.r` ファイルは、AE SDK の PiPLTool を使って `.rc` リソースファイルに変換されます。

```
Step 1: cl.exe /I <SDK_Headers> /EP input.r  -->  output.rr    (C プリプロセッサ)
Step 2: PiPLTool.exe output.rr output.rrc                      (PiPL 変換)
Step 3: cl.exe /D "MSWindows" /EP output.rrc  -->  output.rc   (最終プリプロセス)
```

CMake での自動化（`compile_pipl.cmake`）:

```cmake
find_program(PIPL_TOOL
    NAMES PiPLTool.exe PiPLtool.exe
    PATHS "${AESDK_ROOT}/Resources"
)

if(PIPL_TOOL)
    add_custom_command(
        OUTPUT "${PIPL_RC}"
        COMMAND "${CMAKE_COMMAND}"
            "-DSDK=${AESDK_HEADERS_DIR}"
            "-DTOOL=${PIPL_TOOL}"
            "-DR=${PIPL_SOURCE}"
            "-DRC=${PIPL_RC}"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/compile_pipl.cmake"
        DEPENDS "${PIPL_SOURCE}" "${CMAKE_CURRENT_SOURCE_DIR}/compile_pipl.cmake"
    )
    target_sources(TestEffect PRIVATE "${PIPL_RC}")
endif()
```

### 2.4 PiPL と GlobalSetup の OutFlags 整合性

:::{warning}
PiPL の `AE_Effect_Global_OutFlags` と GlobalSetup の `out_data->out_flags` は**必ず一致**させてください。
不一致はエラーの原因になります。
:::

| フラグ | 16進値 | 説明 |
|--------|--------|------|
| `PF_OutFlag_DEEP_COLOR_AWARE` | `0x2000000` | 16bpc 対応 |
| `PF_OutFlag_CUSTOM_UI` | `0x8000` | カスタム UI（EVENT ハンドラ必須） |
| `PF_OutFlag_I_DO_DIALOG` | `0x20` (1L << 5) | About ダイアログ |

### 2.5 手書き PiPL RC（フォールバック）

PiPLTool が利用できない場合（SDK に含まれていない、パスが見つからない等）、`.rc` ファイルを手書きで作成できます。
CMake が PiPLTool を検出できない場合にフォールバック用の `.rc` が自動的に使用されます。
ただし、PiPLTool による `.r` → `.rc` 変換の方が正確であるため、**PiPLTool の使用を強く推奨**します。

---

## 3. エントリポイントとコマンド処理

### 3.1 PluginDataEntryFunction（V1 / V2）

AE がプラグインを最初に発見した際に呼び出されるエントリポイントです。

```cpp
// SDK 25.2+ (AE 2025.2+): V2 エントリポイント
#ifdef PF_REGISTER_EFFECT_EXT2
extern "C" DllExport
PF_Err PluginDataEntryFunction2(
    PF_PluginDataPtr    inPtr,
    PF_PluginDataCB2    inPluginDataCallBackPtr,
    SPBasicSuite*       inSPBasicSuitePtr,
    const char*         inHostName,
    const char*         inHostVersion)
{
    return PF_REGISTER_EFFECT_EXT2(
        inPtr,
        inPluginDataCallBackPtr,
        "PyAE Test Effect",       // Display Name
        "PYAE TestEffect",        // Match Name (一意識別子)
        "PyAE Test",              // Category
        AE_RESERVED_INFO,
        "EffectMain",             // Entry point function name
        "https://...");           // Support URL
}
#else
// Fallback for older SDKs
extern "C" DllExport
PF_Err PluginDataEntryFunction(
    PF_PluginDataPtr    inPtr,
    PF_PluginDataCB     inPluginDataCallBackPtr,
    SPBasicSuite*       inSPBasicSuitePtr,
    const char*         inHostName,
    const char*         inHostVersion)
{
    return PF_REGISTER_EFFECT(
        inPtr,
        inPluginDataCallBackPtr,
        "PyAE Test Effect",
        "PYAE TestEffect",
        "PyAE Test",
        AE_RESERVED_INFO);
}
#endif
```

### 3.2 EffectMain（コマンドディスパッチャー）

全コマンドのハンドラです。必ず `PF_Err` を返します。

```cpp
PF_Err EffectMain(
    PF_Cmd          cmd,
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ParamDef     *params[],
    PF_LayerDef     *output,
    void            *extra)
{
    PF_Err err = PF_Err_NONE;

    try {
        switch (cmd) {
            case PF_Cmd_ABOUT:          err = About(in_data, out_data, params, output); break;
            case PF_Cmd_GLOBAL_SETUP:   err = GlobalSetup(in_data, out_data, params, output); break;
            case PF_Cmd_PARAMS_SETUP:   err = ParamsSetup(in_data, out_data, params, output); break;
            case PF_Cmd_RENDER:         err = Render(in_data, out_data, params, output); break;
            case PF_Cmd_USER_CHANGED_PARAM:
                err = UserChangedParam(in_data, out_data, params,
                    reinterpret_cast<const PF_UserChangedParamExtra*>(extra));
                break;
            case PF_Cmd_EVENT:
                if (extra) {
                    PF_EventExtra* event_extra = reinterpret_cast<PF_EventExtra*>(extra);
                    if (event_extra->e_type == PF_Event_DRAW) {
                        event_extra->evt_out_flags = PF_EO_HANDLED_EVENT;
                    }
                }
                break;
            case PF_Cmd_ARBITRARY_CALLBACK:
                err = TestFX_HandleArbitrary(in_data, out_data, params, output,
                    reinterpret_cast<PF_ArbParamsExtra*>(extra));
                break;
            default: break;
        }
    } catch (PF_Err& thrown_err) {
        err = thrown_err;
    }

    return err;
}
```

### 3.3 主要コマンド一覧

| コマンド | タイミング | 用途 |
|----------|------------|------|
| `PF_Cmd_ABOUT` | Effect > About 時 | プラグイン情報表示 |
| `PF_Cmd_GLOBAL_SETUP` | プラグイン初回読み込み | バージョン、OutFlags 設定 |
| `PF_Cmd_PARAMS_SETUP` | パラメータ登録 | `PF_ADD_*` マクロでパラメータ追加 |
| `PF_Cmd_RENDER` | 各フレーム描画 | 画像処理（入力 → 出力） |
| `PF_Cmd_EVENT` | UI イベント | DRAW, CLICK 等（CUSTOM_UI 時） |
| `PF_Cmd_USER_CHANGED_PARAM` | ユーザー操作 | パラメータ変更通知 |
| `PF_Cmd_ARBITRARY_CALLBACK` | ARB データ操作 | New/Copy/Flatten/Unflatten 等 |

---

## 4. パラメータの定義

### 4.1 パラメータ ID Enum

パラメータは enum で管理します。`TESTFX_INPUT = 0` は入力レイヤー（自動追加）です。

```cpp
enum {
    TESTFX_INPUT = 0,        // Input layer (auto)
    TESTFX_SLIDER_INT,       // PF_Param_SLIDER
    TESTFX_SLIDER_FLOAT,     // PF_Param_FLOAT_SLIDER
    TESTFX_SLIDER_PERCENT,   // PF_Param_FIX_SLIDER
    TESTFX_CHECKBOX,         // PF_Param_CHECKBOX
    TESTFX_ANGLE,            // PF_Param_ANGLE
    TESTFX_COLOR,            // PF_Param_COLOR
    TESTFX_POINT_2D,         // PF_Param_POINT
    TESTFX_POPUP,            // PF_Param_POPUP
    TESTFX_POINT_3D,         // PF_Param_POINT_3D
    TESTFX_BUTTON,           // PF_Param_BUTTON
    TESTFX_ARBITRARY,        // PF_Param_ARBITRARY_DATA
    TESTFX_NUM_PARAMS
};
```

### 4.2 パラメータ追加マクロ

`PF_Cmd_PARAMS_SETUP` 内で使用します。各マクロの前に `AEFX_CLR_STRUCT(def)` で初期化してください。

#### 整数スライダー

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_SLIDER(
    "Integer Slider",  // Name
    0,                 // Valid min
    100,               // Valid max
    0,                 // Slider min
    100,               // Slider max
    50,                // Default
    TESTFX_SLIDER_INT  // Disk ID
);
```

#### 浮動小数点スライダー

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_FLOAT_SLIDERX(
    "Float Slider",            // Name
    0.0,                       // Valid min
    100.0,                     // Valid max
    0.0,                       // Slider min
    100.0,                     // Slider max
    3.14159,                   // Default
    PF_Precision_HUNDREDTHS,   // Precision
    0,                         // Display flags
    0,                         // Param flags
    TESTFX_SLIDER_FLOAT        // Disk ID
);
```

#### パーセントスライダー（FIX_SLIDER）

固定小数点（16.16形式）を使用します。手動でフィールド設定が必要です。

```cpp
AEFX_CLR_STRUCT(def);
def.param_type = PF_Param_FIX_SLIDER;
PF_STRCPY(def.PF_DEF_NAME, "Percent");
def.u.fd.value_str[0] = '\0';
def.u.fd.value_desc[0] = '\0';
def.u.fd.valid_min = 0;
def.u.fd.valid_max = static_cast<PF_Fixed>(100 << 16);
def.u.fd.slider_min = 0;
def.u.fd.slider_max = static_cast<PF_Fixed>(100 << 16);
def.u.fd.value = static_cast<PF_Fixed>(static_cast<A_long>(75.0 * 65536.0));
def.u.fd.precision = 1;
def.flags = 0;
def.ui_flags = 0;
if (err = PF_ADD_PARAM(in_data, TESTFX_SLIDER_PERCENT, &def)) return err;
```

#### チェックボックス

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_CHECKBOXX("Checkbox", TRUE, 0, TESTFX_CHECKBOX);
```

#### 角度

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_ANGLE("Angle", 45.0, TESTFX_ANGLE);
```

#### カラー

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_COLOR("Color", 255, 128, 64, TESTFX_COLOR);
```

#### 2D ポイント

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_POINT("Point 2D",
    static_cast<A_long>(50.0),  // X default (%)
    static_cast<A_long>(50.0),  // Y default (%)
    0,                          // Restrict bounds
    TESTFX_POINT_2D);
```

#### 3D ポイント

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_POINT_3D("Point 3D", 100.0, 100.0, 100.0, TESTFX_POINT_3D);
```

#### ポップアップメニュー

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_POPUP(
    "Popup Menu",
    5,                                          // Number of choices
    2,                                          // Default (1-based)
    "Option 1|Option 2|Option 3|Option 4|Option 5",  // Pipe-separated
    TESTFX_POPUP);
```

#### ボタン

```cpp
AEFX_CLR_STRUCT(def);
PF_ADD_BUTTON(
    "Test Action",          // Param name
    "Click Me",             // Button label
    0,                      // PUI flags
    PF_ParamFlag_SUPERVISE, // Param flags (USER_CHANGED_PARAM で通知)
    TESTFX_BUTTON);
```

### 4.3 パラメータ数の設定

`PF_Cmd_PARAMS_SETUP` の最後に必ず設定してください。

```cpp
out_data->num_params = TESTFX_NUM_PARAMS;
```

---

## 5. Arbitrary Data（カスタムデータ型）

### 5.1 概要

`PF_Param_ARBITRARY_DATA` は、プラグイン独自のバイナリデータをパラメータとして格納する機能です。
AE は構造を知らないため、各種コールバック（New, Copy, Flatten, Unflatten, Compare 等）を実装する必要があります。

:::{note}
TestEffect での Arbitrary Data のデータ構造やデフォルト値、Python からの読み書き方法については [TestEffect リファレンス](testeffect.md) を参照してください。
:::

### 5.2 パラメータ登録

:::{warning}
`PF_Param_ARBITRARY_DATA` には `PF_PUI_TOPIC`、`PF_PUI_CONTROL`、または `PF_PUI_NO_ECW_UI` の指定が**必須**です。`PF_PUI_NONE` はエラーになります。
:::

```cpp
// Arbitrary Data (Custom Data)
{
    PF_ArbitraryH dephault = NULL;
    err = TestFX_CreateDefaultArb(in_data, out_data, &dephault);
    if (err) return err;

    AEFX_CLR_STRUCT(def);
    def.param_type = PF_Param_ARBITRARY_DATA;
    def.flags = 0;
    PF_STRCPY(def.PF_DEF_NAME, "Custom Data");
    def.ui_width = 0;           // PF_PUI_NO_ECW_UI 時は 0。PF_PUI_CONTROL 時はカスタム UI サイズ
    def.ui_height = 0;          // ui_width/ui_height > 0 は PF_OutFlag_CUSTOM_UI 必須
    def.ui_flags = PF_PUI_NO_ECW_UI;  // ECW から非表示
    def.u.arb_d.value = NULL;
    def.u.arb_d.pad = 0;
    def.u.arb_d.dephault = dephault;
    def.uu.id = def.u.arb_d.id = TESTFX_ARBITRARY;
    def.u.arb_d.refconPV = nullptr;

    PF_Err priv_err = PF_ADD_PARAM(in_data, -1, &def);
    if (priv_err != PF_Err_NONE) return priv_err;
}
```

`PF_ADD_ARBITRARY2` マクロを使用する場合:

```cpp
PF_ADD_ARBITRARY2("Custom Data", 0, 0, 0, PF_PUI_NO_ECW_UI, dephault, TESTFX_ARBITRARY, nullptr);
```

#### PUI フラグの選択

| フラグ | 値 | 用途 | 備考 |
|--------|-----|------|------|
| `PF_PUI_TOPIC` | `1L << 0` (0x1) | トピックヘッダ | `PF_OutFlag_CUSTOM_UI` + EVENT ハンドラ必須 |
| `PF_PUI_CONTROL` | `1L << 1` (0x2) | カスタム UI | `PF_OutFlag_CUSTOM_UI` + EVENT ハンドラ必須 |
| `PF_PUI_NO_ECW_UI` | `1L << 3` (0x8) | ECW 非表示 | **推奨**: CUSTOM_UI 不要 |
| `PF_PUI_INVISIBLE` | `1L << 9` | 完全非表示 | ECW + Timeline 両方から非表示 |

### 5.3 デフォルト値の作成

`PF_HandleSuite1` を使用してメモリ確保・初期化します。

```cpp
PF_Err TestFX_CreateDefaultArb(
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ArbitraryH   *dephault)
{
    PF_Err err = PF_Err_NONE;

    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // PF_HandleSuite1 で確保
    PF_Handle arbH = suites.HandleSuite1()->host_new_handle(sizeof(TestFX_ArbData));
    if (!arbH) return PF_Err_OUT_OF_MEMORY;

    TestFX_ArbData* arbP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(arbH));
    if (!arbP) {
        suites.HandleSuite1()->host_dispose_handle(arbH);
        return PF_Err_OUT_OF_MEMORY;
    }

    AEFX_CLR_STRUCT(*arbP);
    arbP->values[0] = 1.0;
    arbP->values[1] = 2.0;
    arbP->values[2] = 3.0;
    arbP->values[3] = 4.0;
    arbP->mode    = 1;
    arbP->enabled = TRUE;

    suites.HandleSuite1()->host_unlock_handle(arbH);
    *dephault = arbH;

    return err;
}
```

### 5.4 Arbitrary Callback ハンドラ

`PF_Cmd_ARBITRARY_CALLBACK` で呼び出されます。全コールバックを処理する必要があります。

```cpp
PF_Err TestFX_HandleArbitrary(
    PF_InData           *in_data,
    PF_OutData          *out_data,
    PF_ParamDef         *params[],
    PF_LayerDef         *output,
    PF_ArbParamsExtra   *extra)
{
    if (!extra) return PF_Err_BAD_CALLBACK_PARAM;

    switch (extra->which_function) {
        case PF_Arbitrary_NEW_FUNC:
            return TestFX_CreateDefaultArb(in_data, out_data, extra->u.new_func_params.arbPH);

        case PF_Arbitrary_DISPOSE_FUNC:
            if (extra->u.dispose_func_params.arbH)
                PF_DISPOSE_HANDLE(extra->u.dispose_func_params.arbH);
            return PF_Err_NONE;

        case PF_Arbitrary_COPY_FUNC:
            // CRITICAL: 先に destination を確保してから copy する
            err = TestFX_CreateDefaultArb(in_data, out_data, extra->u.copy_func_params.dst_arbPH);
            if (!err) {
                err = Arb_Copy(in_data,
                    &extra->u.copy_func_params.src_arbH,
                    extra->u.copy_func_params.dst_arbPH);
            }
            return err;

        case PF_Arbitrary_FLAT_SIZE_FUNC:
            *(extra->u.flat_size_func_params.flat_data_sizePLu) = sizeof(TestFX_ArbData);
            return PF_Err_NONE;

        case PF_Arbitrary_FLATTEN_FUNC:
            return Arb_Flatten(...);

        case PF_Arbitrary_UNFLATTEN_FUNC:
            return Arb_Unflatten(...);

        case PF_Arbitrary_INTERP_FUNC:
            return Arb_Interpolate(...);

        case PF_Arbitrary_COMPARE_FUNC:
            return Arb_Compare(...);

        case PF_Arbitrary_PRINT_SIZE_FUNC:
            *(extra->u.print_size_func_params.print_sizePLu) = TESTFX_ARB_MAX_PRINT_SIZE;
            return PF_Err_NONE;

        case PF_Arbitrary_PRINT_FUNC:
            return Arb_Print(...);

        case PF_Arbitrary_SCAN_FUNC:
            return PF_Err_NONE;

        default:
            return PF_Err_NONE;
    }
}
```

(copy-callback-warning)=
### 5.5 COPY コールバックの重要な注意点

:::{warning}
AE は `PF_ADD_PARAM` 呼び出し中にデフォルト値のコピーのため `COPY_FUNC` を呼び出します。
この時、**destination ハンドルは未割り当て**（NULL または無効値）の場合があります。
:::

```cpp
case PF_Arbitrary_COPY_FUNC:
    // NG: destination が既に割り当て済みと仮定
    err = Arb_Copy(in_data,
        &extra->u.copy_func_params.src_arbH,
        extra->u.copy_func_params.dst_arbPH);  // dst が NULL --> クラッシュ

    // OK: まず destination を確保してからコピー
    err = TestFX_CreateDefaultArb(in_data, out_data, extra->u.copy_func_params.dst_arbPH);
    if (!err) {
        err = Arb_Copy(in_data,
            &extra->u.copy_func_params.src_arbH,
            extra->u.copy_func_params.dst_arbPH);
    }
```

このパターンは SDK サンプル ColorGrid で使用されている正規のパターンです。

---

## 6. ハンドル型とメモリ管理

### 6.1 3つのハンドル型

AE SDK には3種類のハンドル型があり、**混同すると致命的なエラー**が発生します。

| 型 | 定義 | 用途 | 管理スイート |
|----|------|------|-------------|
| `PF_Handle` | `void**` (Windows) | Effect パラメータ、ARB データ | `PF_HandleSuite1` |
| `AEGP_MemHandle` | `struct _Up_OpaqueMem**` | AEGP プラグイン用メモリ | `AEGP_MemorySuite1` |
| `A_Handle` | `void*` | 汎用ハンドル（AEGP StreamValue の arbH） | 状況による |

### 6.2 PF_HandleSuite1（Effect パラメータ用）

:::{warning}
`PF_HandleSuite1` の関数は C スタイルの**直接戻り値**パターンです。
AEGP スイートの「エラーコード + 出力パラメータ」パターンとは全く異なります。
:::

```c
// PF_HandleSuite1 の定義（AE_EffectCBSuites.h）
typedef struct PF_HandleSuite1 {
    PF_Handle    (*host_new_handle)(A_HandleSize size);           // 直接 PF_Handle を返す
    void*        (*host_lock_handle)(PF_Handle pf_handle);        // 直接 void* を返す
    void         (*host_unlock_handle)(PF_Handle pf_handle);      // 戻り値なし
    void         (*host_dispose_handle)(PF_Handle pf_handle);     // 戻り値なし
    A_HandleSize (*host_get_handle_size)(PF_Handle pf_handle);    // 直接サイズを返す
    PF_Err       (*host_resize_handle)(A_HandleSize new_sizeL, PF_Handle *handlePH);
} PF_HandleSuite1;
```

正しい使用例:

```cpp
AEGP_SuiteHandler suites(in_data->pica_basicP);

// 確保
PF_Handle h = suites.HandleSuite1()->host_new_handle(sizeof(MyData));

// ロック
MyData* ptr = reinterpret_cast<MyData*>(suites.HandleSuite1()->host_lock_handle(h));

// サイズ取得
A_HandleSize size = suites.HandleSuite1()->host_get_handle_size(h);

// アンロック
suites.HandleSuite1()->host_unlock_handle(h);

// 解放
suites.HandleSuite1()->host_dispose_handle(h);
```

### 6.3 AEGP_MemorySuite1（AEGP プラグイン用）

```cpp
// AEGP_MemorySuite1 の関数は「エラーコード + 出力パラメータ」パターン
A_Err AEGP_NewMemHandle(AEGP_PluginID id, const A_char *nameZ, AEGP_MemSize size,
                        AEGP_MemFlag flags, AEGP_MemHandle *memPH);
A_Err AEGP_LockMemHandle(AEGP_MemHandle memH, void **ptr_to_ptr);
A_Err AEGP_UnlockMemHandle(AEGP_MemHandle memH);
A_Err AEGP_FreeMemHandle(AEGP_MemHandle memH);
A_Err AEGP_GetMemHandleSize(AEGP_MemHandle memH, AEGP_MemSize *sizeP);
```

(arb-stream-value-handle)=
### 6.4 ARB ストリーム値のハンドル型

AEGP API（`AEGP_GetNewStreamValue` 等）から取得した ARB ストリーム値は `A_Handle` 型です。
これは `PF_Handle` にキャストして `PF_HandleSuite1` で操作します。

```cpp
// StreamValue の arbH は A_Handle 型
PF_Handle pfH = reinterpret_cast<PF_Handle>(value.val.arbH);

// PF_HandleSuite1 で操作
void* ptr = handleSuite->host_lock_handle(pfH);
A_HandleSize size = handleSuite->host_get_handle_size(pfH);
```

:::{danger}
AEGP_MemorySuite1 で ARB ハンドルを操作すると**クラッシュ**します:

```cpp
// NG: AEGP_MemorySuite1 で ARB ハンドルを操作 --> "不当なトラックメモリID" エラー
AEGP_MemHandle memH = reinterpret_cast<AEGP_MemHandle>(value.val.arbH);
suites.memorySuite->AEGP_LockMemHandle(memH, &ptr);  // クラッシュ！
```
:::

---

## 7. ビルドシステム

### 7.1 CMake 構成

```cmake
cmake_minimum_required(VERSION 3.21)
project(TestEffect VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SDK パス（必須）
set(AESDK_PATH "" CACHE PATH "Path to SDK Examples directory (required)")

# SDK 構造の自動検出
if(EXISTS "${AESDK_PATH}/Examples/Headers")
    set(AESDK_ROOT "${AESDK_PATH}/Examples")
elseif(EXISTS "${AESDK_PATH}/Headers")
    set(AESDK_ROOT "${AESDK_PATH}")
else()
    message(FATAL_ERROR "Cannot find AE SDK Headers directory in ${AESDK_PATH}")
endif()

set(AESDK_HEADERS_DIR "${AESDK_ROOT}/Headers")
set(AESDK_UTIL_DIR "${AESDK_ROOT}/Util")

# AE SDK インターフェースライブラリ
add_library(AdobeAESDK INTERFACE)
target_include_directories(AdobeAESDK INTERFACE
    "${AESDK_HEADERS_DIR}"
    "${AESDK_HEADERS_DIR}/SP"
    "${AESDK_UTIL_DIR}"
)
target_compile_definitions(AdobeAESDK INTERFACE WIN32 _WINDOWS _USRDLL MSWindows)

# プラグイン（.aex = DLL）
add_library(TestEffect SHARED
    TestEffect.cpp
    TestEffect_Strings.cpp
    TestEffect_Arb_Handler.cpp
    ${AESDK_UTIL_DIR}/AEGP_SuiteHandler.cpp
    ${AESDK_UTIL_DIR}/MissingSuiteError.cpp
)

set_target_properties(TestEffect PROPERTIES
    OUTPUT_NAME "TestEffect"
    SUFFIX ".aex"     # AE は .aex 拡張子のみ認識
    PREFIX ""
)

# MSVC コンパイルオプション
if(MSVC)
    target_compile_options(TestEffect PRIVATE
        /W3 /WX- /utf-8 /permissive- /Zc:__cplusplus /MP /EHsc
    )
endif()
```

### 7.2 ビルドコマンド

```bash
# プロジェクトルートで実行（SDKは自動検出）
./build.bat --project TestEffect

# クリーンビルド
./build.bat --project TestEffect --clean

# 特定のSDKを指定する場合
./build.bat --project TestEffect --sdk-path "SDKs/ae25.6_61.64bit.AfterEffectsSDK/Examples"
```

### 7.3 デプロイ

```bash
# プロジェクトルートで実行（管理者権限が必要、PyAEとTestEffect両方をデプロイ）
./setup.bat

# アンインストール
./setup.bat --uninstall
```

デプロイ先: `C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\TestEffect\`

---

## 8. 文字列テーブル

### 8.1 String_Utils パターン

SDK の `String_Utils.h` は `GetStringPtr()` 関数と `STR()` マクロを提供します。

```cpp
// TestEffect_Strings.h - 文字列 ID
typedef enum {
    StrID_NONE = 0,
    StrID_Name,
    StrID_Description,
    StrID_Slider_Int,
    StrID_Slider_Float,
    // ...
    StrID_NUMTYPES
} StrIDType;

// TestEffect_Strings.cpp - 文字列テーブル
typedef struct {
    unsigned long   index;
    char            str[256];
} TableString;

TableString g_strs[StrID_NUMTYPES] = {
    StrID_NONE,        "",
    StrID_Name,        "PyAE Test Effect",
    StrID_Description, "Test effect for PyAE custom property validation.",
    StrID_Slider_Int,  "Integer Slider",
    // ...
};

char* GetStringPtr(int strNum) {
    return g_strs[strNum].str;
}
```

使用方法:

```cpp
PF_ADD_SLIDER(STR(StrID_Slider_Int), ...);
```

---

## 9. よくある問題と解決方法

### 9.1 "cannot initialize" エラー (25, 3)

**原因**: `PF_Param_ARBITRARY_DATA` に `PF_PUI_NONE (=0)` を指定。

**解決**: `PF_PUI_TOPIC`、`PF_PUI_CONTROL`、または `PF_PUI_NO_ECW_UI` のいずれかを使用する。

### 9.2 "不当なトラックメモリID" エラー (23, 34)

**原因**: ARB ストリーム値の `A_Handle` を `AEGP_MemHandle` として扱った。

**解決**: `PF_HandleSuite1` を使用する（[6.4 ARB ストリーム値のハンドル型](arb-stream-value-handle) 参照）。

### 9.3 Error 516 (PF_Err_BAD_CALLBACK_PARAM)

**原因**: `COPY_FUNC` コールバックで destination ハンドルが未割り当て。

**解決**: コピー前に destination を確保する（[5.5 COPY コールバックの重要な注意点](copy-callback-warning) 参照）。

### 9.4 AE がハングする（EVENT 未処理）

**原因**: `PF_Cmd_EVENT` の `PF_Event_DRAW` を処理せず、AE が無限リトライ。

**解決**: DRAW イベントに `PF_EO_HANDLED_EVENT` を設定する。

```cpp
case PF_Cmd_EVENT:
    if (extra) {
        PF_EventExtra* event_extra = reinterpret_cast<PF_EventExtra*>(extra);
        if (event_extra->e_type == PF_Event_DRAW) {
            event_extra->evt_out_flags = PF_EO_HANDLED_EVENT;
        }
    }
    break;
```

### 9.5 AE クラッシュ回復ダイアログ

テスト中に AE を強制終了すると、次回起動時にクラッシュ回復ダイアログが表示されます。
自動テスト環境では誰もダイアログを閉じないためテストがタイムアウトします。

**対策**: 強制終了後は手動で AE を起動し、回復ダイアログを閉じてからテストを実行する。

:::{tip}
[TestEffect リファレンス](testeffect.md) のトラブルシューティングには、`AEGP_StreamType_NO_DATA` エラーや Custom UI フラグの不整合エラーについても記載されています。
:::

---

## 10. SDK サンプルリファレンス

### 10.1 参考になる SDK サンプル

| サンプル | 場所 | 特徴 |
|----------|------|------|
| **Skeleton** | `Effect/Skeleton/` | 最小構成のエフェクトプラグイン |
| **ColorGrid** | `Effect/ColorGrid/` | ARBITRARY_DATA + CUSTOM_UI の完全な実装例 |
| **Paramarama** | `Effect/Paramarama/` | 全パラメータタイプの網羅的な実装例 |
| **Checkout** | `Effect/Checkout/` | パラメータ値の読み取り |

### 10.2 ColorGrid の ARB パターン（SDK リファレンス実装）

ColorGrid は ARBITRARY_DATA の正式な実装例として最も参考になります。
特に COPY コールバックでの destination 事前確保パターンを採用しています。

### 10.3 Paramarama の PF_PUI_NO_ECW_UI パターン

Paramarama は `PF_PUI_NO_ECW_UI` フラグを使って ARB パラメータを ECW から非表示にしています。
CUSTOM_UI を必要とせず、よりシンプルな構成が可能です。

---

## 11. Python からの ARB データアクセス（PyAE 内部実装）

### 11.1 読み取り（StreamValueToPython）

AEGP API の `AEGP_GetNewStreamValue` で取得した値を Python の `bytes` に変換します。

```cpp
case AEGP_StreamType_ARB: {
    if (!value.val.arbH) return py::bytes("");

    PF_HandleSuite1* handleSuite = state.GetSuiteHandler().HandleSuite1();
    PF_Handle pfH = reinterpret_cast<PF_Handle>(value.val.arbH);
    void* ptr = handleSuite->host_lock_handle(pfH);
    if (!ptr) return py::none();

    A_HandleSize size = handleSuite->host_get_handle_size(pfH);
    py::object result = py::bytes(static_cast<const char*>(ptr), static_cast<size_t>(size));

    handleSuite->host_unlock_handle(pfH);
    return result;
}
```

### 11.2 書き込み（PythonToStreamValue）

Python の `bytes` から新しいハンドルを確保し、ストリーム値に設定します。

```cpp
case AEGP_StreamType_ARB: {
    std::string data;
    if (py::isinstance<py::bytes>(value)) {
        data = value.cast<std::string>();
    } else {
        py::buffer buffer = value.cast<py::buffer>();
        py::buffer_info info = buffer.request();
        data.assign(static_cast<const char*>(info.ptr), info.size * info.itemsize);
    }

    PF_HandleSuite1* handleSuite = state.GetSuiteHandler().HandleSuite1();
    PF_Handle pfH = handleSuite->host_new_handle(static_cast<A_HandleSize>(data.size()));
    if (!pfH) throw std::runtime_error("Failed to allocate handle for arbitrary data");

    void* ptr = handleSuite->host_lock_handle(pfH);
    if (ptr) {
        memcpy(ptr, data.data(), data.size());
        handleSuite->host_unlock_handle(pfH);
        streamValue.val.arbH = pfH;
    } else {
        handleSuite->host_dispose_handle(pfH);
        throw std::runtime_error("Failed to lock handle");
    }
    break;
}
```

---

## 付録: TestEffect 試行錯誤ログ

### ARBITRARY_DATA 追加の試行結果

| 試行 | 設定 | 結果 |
|------|------|------|
| 1 | `PF_PUI_NONE`, w=0, h=0 | Error (25, 3) "cannot initialize" |
| 2 | `PF_PUI_NO_ECW_UI`, w=1, h=1, CUSTOM_UI なし | Error (25, 37) "no custom ui outflag" |
| 3 | `PF_PUI_NO_ECW_UI`, w=0, h=0 | AE クラッシュ（COPY コールバック未修正が原因） |
| 4 | `PF_PUI_CONTROL`, `PF_OutFlag_CUSTOM_UI` | AE ハング（EVENT 未処理） |
| 5 | `PF_PUI_NO_ECW_UI`, w=1, h=1, COPY 修正 | 動作するがエラー (25, 37) が出る |
| 6 | `PF_PUI_NO_ECW_UI`, w=0, h=0, COPY 修正済み | **成功** |

最終的な成功構成:
- `PF_PUI_NO_ECW_UI` + `ui_width=0, ui_height=0`
- `PF_OutFlag_DEEP_COLOR_AWARE` のみ（CUSTOM_UI 不要）
- COPY コールバックで destination を事前確保（試行3のクラッシュ原因はCOPY未修正）
- `PF_HandleSuite1` の C スタイル直接戻り値パターンを使用
- PiPL OutFlags: `0x2000000`（DEEP_COLOR_AWARE のみ）

---

## 関連項目

- [TestEffect リファレンス](testeffect.md) - パラメータ一覧、Python からの使い方
- [Effect API リファレンス](../api/high-level/effect.rst) - エフェクトクラス
- [Property API リファレンス](../api/high-level/property.rst) - プロパティクラス
- [ビルドガイド](build-notes.md) - ビルドシステムの詳細
