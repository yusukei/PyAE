// PySidePanelUI_Win.cpp
// PyAE - Python for After Effects
// PySide6埋め込み用パネルUI (Windows) - 複数インスタンス対応

#ifdef _WIN32

#include "PySidePanelUI_Win.h"
#include "PluginState.h"
#include "IdleHandler.h"
#include "Logger.h"

#include <algorithm>

namespace PyAE {

// 静的メンバー初期化
std::unordered_map<std::string, PySidePanelUI_Win*> PySidePanelUI_Win::s_panelRegistry;
PySidePanelUI_Win* PySidePanelUI_Win::s_activePanel = nullptr;

// ウィンドウプロパティ名
static const char* const PYSIDE_PANEL_WINDOW_PROPERTY = "PyAE_PySidePanelUI_Win_Ptr";

// AEダークテーマ用カラー
static constexpr COLORREF COLOR_BG = RGB(46, 46, 46);  // 背景色（ダークグレー）

PySidePanelUI_Win::PySidePanelUI_Win(
    SPBasicSuite* spbP,
    AEGP_PanelH panelH,
    AEGP_PlatformViewRef platformWindowRef,
    AEGP_PanelFunctions1* outFunctionTable,
    const std::string& matchName)
    : m_basicSuite(spbP)
    , m_panelH(panelH)
    , m_containerHwnd(platformWindowRef)
    , m_matchName(matchName)
{
    PYAE_LOG_INFO("PySidePanelUI_Win", "Creating PySide panel UI for: " + matchName);

    // コールバック関数テーブルを設定
    outFunctionTable->GetSnapSizes = S_GetSnapSizes;
    outFunctionTable->PopulateFlyout = S_PopulateFlyout;
    outFunctionTable->DoFlyoutCommand = S_DoFlyoutCommand;

    // ウィンドウプロシージャをフック
    m_prevWindowProc = reinterpret_cast<WindowProc>(
        GetWindowLongPtr(platformWindowRef, GWLP_WNDPROC));

    SetWindowLongPtrA(platformWindowRef, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(StaticOSWindowWndProc));

    // このオブジェクトへのポインタをウィンドウプロパティに保存
    SetPropA(platformWindowRef, PYSIDE_PANEL_WINDOW_PROPERTY, static_cast<HANDLE>(this));

    // 背景ブラシを作成（AEダークテーマ用）
    m_bgBrush = CreateSolidBrush(COLOR_BG);

    // パネルレジストリに登録
    if (!matchName.empty()) {
        s_panelRegistry[matchName] = this;
    }

    // アクティブパネルとして設定
    s_activePanel = this;

    PYAE_LOG_INFO("PySidePanelUI_Win", "s_activePanel set to: " + std::to_string(reinterpret_cast<uintptr_t>(this)));
    PYAE_LOG_INFO("PySidePanelUI_Win", "Container HWND: " + std::to_string(reinterpret_cast<uintptr_t>(m_containerHwnd)));

    PYAE_LOG_INFO("PySidePanelUI_Win", "PySide panel UI created successfully for: " + matchName);
}

PySidePanelUI_Win::~PySidePanelUI_Win() {
    PYAE_LOG_INFO("PySidePanelUI_Win", "Destroying PySide panel UI for: " + m_matchName);

    // タイマーをキャンセル
    if (m_containerHwnd) {
        KillTimer(m_containerHwnd, RESIZE_TIMER_ID);
    }

    // 埋め込みウィジェットへの参照をクリア（ウィジェット自体はPython/Qtが管理）
    m_embeddedWidgetHwnd = nullptr;

    // パネルレジストリから削除
    if (!m_matchName.empty()) {
        s_panelRegistry.erase(m_matchName);
    }

    // アクティブパネルをクリア
    if (s_activePanel == this) {
        s_activePanel = nullptr;
        // 他にパネルがあれば最初のものをアクティブに
        if (!s_panelRegistry.empty()) {
            s_activePanel = s_panelRegistry.begin()->second;
        }
    }

    // ウィンドウプロシージャを復元
    if (m_containerHwnd && m_prevWindowProc) {
        SetWindowLongPtrA(m_containerHwnd, GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(m_prevWindowProc));
    }

    // プロパティを削除
    if (m_containerHwnd) {
        RemovePropA(m_containerHwnd, PYSIDE_PANEL_WINDOW_PROPERTY);
    }

    // 背景ブラシを削除
    if (m_bgBrush) {
        DeleteObject(m_bgBrush);
        m_bgBrush = nullptr;
    }
}

PySidePanelUI_Win* PySidePanelUI_Win::GetPanel(const std::string& matchName) {
    auto it = s_panelRegistry.find(matchName);
    if (it != s_panelRegistry.end()) {
        return it->second;
    }
    return nullptr;
}

PySidePanelUI_Win* PySidePanelUI_Win::GetActivePanel() {
    return s_activePanel;
}

void PySidePanelUI_Win::SetActivePanel(PySidePanelUI_Win* panel) {
    s_activePanel = panel;
}

void PySidePanelUI_Win::SetEmbeddedWidget(HWND widgetHwnd) {
    m_embeddedWidgetHwnd = widgetHwnd;
    PYAE_LOG_INFO("PySidePanelUI_Win", "Embedded widget HWND set to: " +
        std::to_string(reinterpret_cast<uintptr_t>(widgetHwnd)) +
        " for panel: " + m_matchName);

    // ウィジェットをリサイズ（MoveWindowを使用）
    if (widgetHwnd && m_containerHwnd && IsWindow(widgetHwnd)) {
        RECT rect;
        GetClientRect(m_containerHwnd, &rect);
        MoveWindow(widgetHwnd, 0, 0,
            rect.right - rect.left, rect.bottom - rect.top, TRUE);
    }
}

// ウィンドウプロシージャ
LRESULT CALLBACK PySidePanelUI_Win::StaticOSWindowWndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    PySidePanelUI_Win* panel = reinterpret_cast<PySidePanelUI_Win*>(
        GetPropA(hWnd, PYSIDE_PANEL_WINDOW_PROPERTY));

    if (panel) {
        return panel->OSWindowWndProc(hWnd, message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT PySidePanelUI_Win::OSWindowWndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message) {
    case WM_SIZE:
        // SIZE_MINIMIZED (wParam == 1) の場合はリサイズしない
        if (wParam != SIZE_MINIMIZED) {
            OnSize(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_TIMER:
        if (wParam == RESIZE_TIMER_ID) {
            KillTimer(hWnd, RESIZE_TIMER_ID);
            DoDelayedResize();
            return 0;
        }
        break;

    case WM_ERASEBKGND:
        // 背景をダークグレーで塗りつぶす
        {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(hdc, &rect, m_bgBrush);
            return TRUE;
        }

    case WM_SETFOCUS:
    case WM_ACTIVATE:
        // このパネルがアクティブになった
        s_activePanel = this;
        break;

    case WM_DESTROY:
        // クリーンアップ
        break;
    }

    // 元のウィンドウプロシージャを呼び出す
    if (m_prevWindowProc) {
        return CallWindowProc(m_prevWindowProc, hWnd, message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void PySidePanelUI_Win::OnSize(int width, int height) {
    // サイズが0以下の場合はスキップ（クラッシュ防止）
    if (width <= 0 || height <= 0) {
        return;
    }

    // 最小サイズを設定（極端に小さいサイズでのクラッシュ防止）
    static constexpr int MIN_SIZE = 50;
    if (width < MIN_SIZE || height < MIN_SIZE) {
        // 埋め込みウィジェットがある場合は非表示にする
        if (m_embeddedWidgetHwnd && IsWindow(m_embeddedWidgetHwnd)) {
            ShowWindow(m_embeddedWidgetHwnd, SW_HIDE);
        }
        return;
    }

    // 埋め込みウィジェットがない場合はスキップ
    if (!m_embeddedWidgetHwnd || !IsWindow(m_embeddedWidgetHwnd)) {
        return;
    }

    // 非表示だった場合は表示する
    if (!IsWindowVisible(m_embeddedWidgetHwnd)) {
        ShowWindow(m_embeddedWidgetHwnd, SW_SHOW);
    }

    // 遅延リサイズをスケジュール（タイマーで処理）
    m_pendingWidth = width;
    m_pendingHeight = height;
    m_resizePending = true;

    // 既存のタイマーをキャンセルして新しいタイマーをセット（16ms = 約60fps）
    KillTimer(m_containerHwnd, RESIZE_TIMER_ID);
    SetTimer(m_containerHwnd, RESIZE_TIMER_ID, 16, nullptr);
}

void PySidePanelUI_Win::DoDelayedResize() {
    if (!m_resizePending) {
        return;
    }
    m_resizePending = false;

    int width = m_pendingWidth;
    int height = m_pendingHeight;

    // 最小サイズの再確認
    static constexpr int MIN_SIZE = 50;
    if (width < MIN_SIZE || height < MIN_SIZE) {
        return;
    }

    // 埋め込まれたウィジェットをリサイズ
    if (m_embeddedWidgetHwnd && IsWindow(m_embeddedWidgetHwnd)) {
        if (IsWindowVisible(m_embeddedWidgetHwnd)) {
            // MoveWindowはWM_SIZEを内部で発行し、Qtがそれを処理する
            MoveWindow(m_embeddedWidgetHwnd, 0, 0, width, height, TRUE);
        }
    }

    // コールバックを呼び出し
    if (m_resizeCallback) {
        m_resizeCallback(width, height);
    }
}

// スナップサイズ
void PySidePanelUI_Win::GetSnapSizes(A_LPoint* snapSizes, A_long* numSizesP) {
    snapSizes[0].x = 300;
    snapSizes[0].y = 200;
    snapSizes[1].x = 500;
    snapSizes[1].y = 400;
    *numSizesP = 2;
}

// フライアウトメニュー
void PySidePanelUI_Win::PopulateFlyout(AEGP_FlyoutMenuItem* itemsP, A_long* in_out_numItemsP) {
    AEGP_FlyoutMenuItem menu[] = {
        {1, AEGP_FlyoutMenuMarkType_NORMAL, TRUE,
         static_cast<A_long>(PySidePanelFlyoutCmd::About),
         reinterpret_cast<const A_u_char*>("About PyAE Panel...")},
    };

    A_long menuSize = sizeof(menu) / sizeof(AEGP_FlyoutMenuItem);

    if (menuSize <= *in_out_numItemsP) {
        std::copy(&menu[0], &menu[menuSize], itemsP);
    } else {
        std::copy(&menu[0], &menu[*in_out_numItemsP], itemsP);
    }

    *in_out_numItemsP = menuSize;
}

void PySidePanelUI_Win::DoFlyoutCommand(AEGP_FlyoutMenuCmdID commandID) {
    auto cmd = static_cast<PySidePanelFlyoutCmd>(commandID);

    switch (cmd) {
    case PySidePanelFlyoutCmd::About:
        PYAE_LOG_INFO("PySidePanelUI_Win", "About PyAE Panel: " + m_matchName);
        break;
    }
}

// 静的コールバックアダプター
A_Err PySidePanelUI_Win::S_GetSnapSizes(AEGP_PanelRefcon refcon, A_LPoint* snapSizes, A_long* numSizesP) {
    try {
        reinterpret_cast<PySidePanelUI_Win*>(refcon)->GetSnapSizes(snapSizes, numSizesP);
        return A_Err_NONE;
    } catch (...) {
        return A_Err_GENERIC;
    }
}

A_Err PySidePanelUI_Win::S_PopulateFlyout(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuItem* itemsP, A_long* in_out_numItemsP) {
    try {
        reinterpret_cast<PySidePanelUI_Win*>(refcon)->PopulateFlyout(itemsP, in_out_numItemsP);
        return A_Err_NONE;
    } catch (...) {
        return A_Err_GENERIC;
    }
}

A_Err PySidePanelUI_Win::S_DoFlyoutCommand(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuCmdID commandID) {
    try {
        reinterpret_cast<PySidePanelUI_Win*>(refcon)->DoFlyoutCommand(commandID);
        return A_Err_NONE;
    } catch (...) {
        return A_Err_GENERIC;
    }
}

} // namespace PyAE

#endif // _WIN32
