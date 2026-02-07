// PanelUI_Win.h
// PyAE - Python for After Effects
// Windows版パネルUI - Mayaスクリプトエディタ風レイアウト

#pragma once

#ifdef _WIN32

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include "AE_GeneralPlug.h"
#include "AE_GeneralPlugPanels.h"
#include "SPBasic.h"

namespace PyAE {

// コントロールID
enum class PanelControlID : WORD {
    LogEdit = 1001,           // コンソール出力（上部）
    ScriptEdit = 1002         // スクリプトエディタ（下部）
};

// フライアウトメニューコマンドID
enum class PanelFlyoutCmd : A_long {
    ExecuteCode = 100,
    ClearLog = 101,
    ClearEditor = 102,
    CopyLog = 103,
    WordWrap = 104
};

class PanelUI_Win {
public:
    explicit PanelUI_Win(
        SPBasicSuite* spbP,
        AEGP_PanelH panelH,
        AEGP_PlatformViewRef platformWindowRef,
        AEGP_PanelFunctions1* outFunctionTable);

    ~PanelUI_Win();

    // ログ出力
    void AppendLog(const std::string& text, bool isError = false);
    void ClearLog();
    void ClearEditor();

    // HWND取得（PySide6統合用）
    HWND GetContainerHWND() const { return m_containerHwnd; }

    // シングルトンアクセス（アクティブなパネルを取得）
    static PanelUI_Win* GetActivePanel() { return s_activePanel; }
    static void SetActivePanel(PanelUI_Win* panel) { s_activePanel = panel; }

    // コード実行
    void ExecuteSelectedCode();
    std::string GetSelectedTextOrAll();

protected:
    // コールバック関数テーブル
    void GetSnapSizes(A_LPoint* snapSizes, A_long* numSizesP);
    void PopulateFlyout(AEGP_FlyoutMenuItem* itemsP, A_long* in_out_numItemsP);
    void DoFlyoutCommand(AEGP_FlyoutMenuCmdID commandID);

    // ウィンドウ再描画
    void InvalidateAll();

private:
    // コピー禁止
    PanelUI_Win(const PanelUI_Win&) = delete;
    PanelUI_Win& operator=(const PanelUI_Win&) = delete;

    // コントロール作成
    void CreateControls();
    void LayoutControls();

    // ウィンドウプロシージャ
    typedef LRESULT(CALLBACK* WindowProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    WindowProc m_prevWindowProc = nullptr;
    WindowProc m_prevEditorProc = nullptr;  // エディタ用サブクラスプロシージャ

    static LRESULT CALLBACK StaticOSWindowWndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    static LRESULT CALLBACK StaticEditorWndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    LRESULT OSWindowWndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    // メッセージハンドラ
    void OnSize(int width, int height);
    void OnCommand(WORD controlId, WORD notifyCode);
    void OnPaint(HDC hdc);

    // 静的コールバックアダプター
    static A_Err S_GetSnapSizes(AEGP_PanelRefcon refcon, A_LPoint* snapSizes, A_long* numSizesP);
    static A_Err S_PopulateFlyout(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuItem* itemsP, A_long* in_out_numItemsP);
    static A_Err S_DoFlyoutCommand(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuCmdID commandID);

    // メンバー変数
    SPBasicSuite* m_basicSuite = nullptr;
    AEGP_PanelH m_panelH = nullptr;
    AEGP_PlatformViewRef m_containerHwnd = nullptr;

    // コントロールハンドル
    HWND m_logEdit = nullptr;           // コンソール出力（上部、読み取り専用）
    HWND m_scriptEdit = nullptr;        // スクリプトエディタ（下部、マルチライン）

    // フォント
    HFONT m_font = nullptr;

    // 背景ブラシ（ダークテーマ用）
    HBRUSH m_bgBrush = nullptr;

    // 設定
    bool m_wordWrap = false;
    double m_splitRatio = 0.5;  // 上下の分割比率

    // Main thread ID (for direct output callback on main thread)
    DWORD m_mainThreadId = 0;

    // Validity flag to prevent Use-After-Free in delayed callbacks
    // Lambdas that capture 'this' and are passed to IdleHandler must also
    // capture a copy of this shared_ptr and check it before using 'this'
    std::shared_ptr<std::atomic<bool>> m_aliveFlag;

    // コマンド履歴（将来の拡張用）
    std::vector<std::string> m_commandHistory;
    int m_historyIndex = -1;

    // シングルトン（最後に作成されたパネル）
    static PanelUI_Win* s_activePanel;
};

} // namespace PyAE

#endif // _WIN32
