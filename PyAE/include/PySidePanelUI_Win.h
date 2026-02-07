// PySidePanelUI_Win.h
// PyAE - Python for After Effects
// PySide6埋め込み用パネルUI (Windows) - 複数インスタンス対応

#pragma once

#ifdef _WIN32

#include <Windows.h>
#include <string>
#include <functional>
#include <unordered_map>

#include "AE_GeneralPlug.h"
#include "AE_GeneralPlugPanels.h"
#include "SPBasic.h"

namespace PyAE {

// フライアウトメニューコマンドID
enum class PySidePanelFlyoutCmd : A_long {
    About = 100
};

/**
 * @brief PySide6ウィジェット埋め込み用パネルUI
 *
 * このパネルは空のコンテナとして機能し、
 * PySide6ウィジェットを埋め込むためのHWNDを提供します。
 *
 * 複数インスタンスをサポートし、各パネルはmatchNameで識別されます。
 */
class PySidePanelUI_Win {
public:
    explicit PySidePanelUI_Win(
        SPBasicSuite* spbP,
        AEGP_PanelH panelH,
        AEGP_PlatformViewRef platformWindowRef,
        AEGP_PanelFunctions1* outFunctionTable,
        const std::string& matchName = "");

    ~PySidePanelUI_Win();

    // HWND取得（PySide6統合用）
    HWND GetContainerHWND() const { return m_containerHwnd; }

    // パネル識別名
    const std::string& GetMatchName() const { return m_matchName; }

    // 埋め込まれたウィジェットのHWND管理
    void SetEmbeddedWidget(HWND widgetHwnd);
    HWND GetEmbeddedWidget() const { return m_embeddedWidgetHwnd; }

    // パネルインスタンス管理（matchNameで検索）
    static PySidePanelUI_Win* GetPanel(const std::string& matchName);
    static PySidePanelUI_Win* GetActivePanel();
    static void SetActivePanel(PySidePanelUI_Win* panel);

    // リサイズコールバック（埋め込みウィジェットのリサイズ用）
    using ResizeCallback = std::function<void(int width, int height)>;
    void SetResizeCallback(ResizeCallback callback) { m_resizeCallback = callback; }

protected:
    // コールバック関数テーブル
    void GetSnapSizes(A_LPoint* snapSizes, A_long* numSizesP);
    void PopulateFlyout(AEGP_FlyoutMenuItem* itemsP, A_long* in_out_numItemsP);
    void DoFlyoutCommand(AEGP_FlyoutMenuCmdID commandID);

private:
    // コピー禁止
    PySidePanelUI_Win(const PySidePanelUI_Win&) = delete;
    PySidePanelUI_Win& operator=(const PySidePanelUI_Win&) = delete;

    // ウィンドウプロシージャ
    typedef LRESULT(CALLBACK* WindowProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    WindowProc m_prevWindowProc = nullptr;

    static LRESULT CALLBACK StaticOSWindowWndProc(
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
    void DoDelayedResize();

    // タイマーID
    static constexpr UINT_PTR RESIZE_TIMER_ID = 1001;

    // 静的コールバックアダプター
    static A_Err S_GetSnapSizes(AEGP_PanelRefcon refcon, A_LPoint* snapSizes, A_long* numSizesP);
    static A_Err S_PopulateFlyout(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuItem* itemsP, A_long* in_out_numItemsP);
    static A_Err S_DoFlyoutCommand(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuCmdID commandID);

    // メンバー変数
    SPBasicSuite* m_basicSuite = nullptr;
    AEGP_PanelH m_panelH = nullptr;
    AEGP_PlatformViewRef m_containerHwnd = nullptr;

    // パネル識別名
    std::string m_matchName;

    // 埋め込まれたPySide6ウィジェットのHWND
    HWND m_embeddedWidgetHwnd = nullptr;

    // 保留中のリサイズ
    int m_pendingWidth = 0;
    int m_pendingHeight = 0;
    bool m_resizePending = false;

    // 背景ブラシ（ダークテーマ用）
    HBRUSH m_bgBrush = nullptr;

    // リサイズコールバック
    ResizeCallback m_resizeCallback;

    // パネルインスタンスレジストリ（matchName -> PySidePanelUI_Win*）
    static std::unordered_map<std::string, PySidePanelUI_Win*> s_panelRegistry;

    // 最後にアクティブになったパネル（後方互換性用）
    static PySidePanelUI_Win* s_activePanel;
};

} // namespace PyAE

#endif // _WIN32
