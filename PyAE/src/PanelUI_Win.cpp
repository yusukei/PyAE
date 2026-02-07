// PanelUI_Win.cpp
// PyAE - Python for After Effects
// Windows版パネルUI - Mayaスクリプトエディタ風レイアウト
// 上部: コンソール出力 / 下部: マルチラインスクリプトエディタ

#ifdef _WIN32

#include "PanelUI_Win.h"
#include "PluginState.h"
#include "PythonHost.h"
#include "IdleHandler.h"
#include "Logger.h"

#include <algorithm>
#include <sstream>
#include <uxtheme.h>
#include <vssym32.h>
#include <dwmapi.h>

#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "dwmapi.lib")

// Windows 10 1809+ ダークモードAPI（非公開）
// これらはuxtheme.dllからordinalでエクスポートされている
using fnSetWindowTheme = HRESULT(WINAPI *)(HWND, LPCWSTR, LPCWSTR);
using fnAllowDarkModeForWindow = bool(WINAPI *)(HWND, bool);
using fnSetPreferredAppMode = void(WINAPI *)(int);

// ダークモードを有効化するヘルパー関数
static void EnableDarkModeForControl(HWND hwnd)
{
    // SetWindowThemeでダークモードを適用（Windows 10 1809+）
    SetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr);

    // Windows 10 1903以降の非公開API（ordinal 133: AllowDarkModeForWindow）
    static fnAllowDarkModeForWindow allowDarkMode = nullptr;
    static bool initialized = false;

    if (!initialized)
    {
        HMODULE hUxTheme = GetModuleHandleW(L"uxtheme.dll");
        if (hUxTheme)
        {
            allowDarkMode = reinterpret_cast<fnAllowDarkModeForWindow>(
                GetProcAddress(hUxTheme, MAKEINTRESOURCEA(133)));
        }
        initialized = true;
    }

    if (allowDarkMode)
    {
        allowDarkMode(hwnd, true);
    }
}

namespace PyAE
{

    // 静的メンバー初期化
    PanelUI_Win *PanelUI_Win::s_activePanel = nullptr;

    // ウィンドウプロパティ名
    static const char *const PANEL_WINDOW_PROPERTY = "PyAE_PanelUI_Win_Ptr";
    static const char *const EDITOR_SUBCLASS_PROPERTY = "PyAE_EditorSubclass_Ptr";

    // UIサイズ定義
    static constexpr int MARGIN = 4;
    static constexpr int SPLITTER_HEIGHT = 6;
    static constexpr double DEFAULT_SPLIT_RATIO = 0.5; // 上下の分割比率

    // AEダークテーマ用カラー
    static constexpr COLORREF COLOR_BG = RGB(46, 46, 46);       // 背景色（ダークグレー）
    static constexpr COLORREF COLOR_TEXT = RGB(220, 220, 220);  // テキスト色（明るいグレー）
    static constexpr COLORREF COLOR_ERROR = RGB(255, 100, 100); // エラーテキスト色（赤）
    static constexpr COLORREF COLOR_BORDER = RGB(30, 30, 30);   // 境界線色（より暗いグレー）

    PanelUI_Win::PanelUI_Win(
        SPBasicSuite *spbP,
        AEGP_PanelH panelH,
        AEGP_PlatformViewRef platformWindowRef,
        AEGP_PanelFunctions1 *outFunctionTable)
        : m_basicSuite(spbP), m_panelH(panelH), m_containerHwnd(platformWindowRef), m_splitRatio(DEFAULT_SPLIT_RATIO),
          m_aliveFlag(std::make_shared<std::atomic<bool>>(true))
    {
        PYAE_LOG_INFO("PanelUI_Win", "Creating panel UI (Maya-style layout)...");

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
        SetPropA(platformWindowRef, PANEL_WINDOW_PROPERTY, static_cast<HANDLE>(this));

        // フォントを作成
        m_font = CreateFontW(
            14,                      // 高さ
            0,                       // 幅
            0,                       // 角度
            0,                       // 向き
            FW_NORMAL,               // 太さ
            FALSE,                   // 斜体
            FALSE,                   // 下線
            FALSE,                   // 打消し線
            DEFAULT_CHARSET,         // 文字セット
            OUT_DEFAULT_PRECIS,      // 出力精度
            CLIP_DEFAULT_PRECIS,     // クリッピング精度
            CLEARTYPE_QUALITY,       // 品質
            FIXED_PITCH | FF_MODERN, // ピッチとファミリー
            L"Consolas"              // フォント名
        );

        // 背景ブラシを作成（AEダークテーマ用）
        m_bgBrush = CreateSolidBrush(COLOR_BG);

        // コントロールを作成
        CreateControls();

        // レイアウト
        LayoutControls();

        // Store main thread ID for direct callback invocation
        m_mainThreadId = GetCurrentThreadId();

        // 初期メッセージを表示
        AppendLog("PyAE Console - Python for After Effects\n");
        AppendLog("Ctrl+Enter: Execute selected code (or all if no selection)\n");
        AppendLog("----------------------------------------\n");

        // Python出力コールバックを登録
        // SAFETY: Capture m_aliveFlag to prevent Use-After-Free if callbacks execute after destruction
        auto aliveFlag = m_aliveFlag;
        PythonHost::Instance().SetOutputCallback(
            [this, aliveFlag](const std::string &text, bool isError)
            {
                if (GetCurrentThreadId() == m_mainThreadId)
                {
                    // Already on main thread (during Python execution) - call directly
                    // Check if still alive before using 'this'
                    if (aliveFlag && aliveFlag->load())
                    {
                        AppendLog(text + "\n", isError);
                    }
                }
                else
                {
                    // From another thread (e.g. Python threading) - enqueue for main thread
                    IdleHandler::Instance().EnqueueTask([this, aliveFlag, text, isError]()
                    {
                        // Check if panel is still alive before accessing 'this'
                        if (aliveFlag && aliveFlag->load())
                        {
                            AppendLog(text + "\n", isError);
                        }
                    });
                }
            });

        // アクティブパネルとして登録
        s_activePanel = this;
        PYAE_LOG_INFO("PanelUI_Win", "s_activePanel set to: " + std::to_string(reinterpret_cast<uintptr_t>(this)));
        PYAE_LOG_INFO("PanelUI_Win", "Container HWND: " + std::to_string(reinterpret_cast<uintptr_t>(m_containerHwnd)));

        PYAE_LOG_INFO("PanelUI_Win", "Panel UI created successfully");
    }

    PanelUI_Win::~PanelUI_Win()
    {
        PYAE_LOG_INFO("PanelUI_Win", "Destroying panel UI...");

        // Mark this object as destroyed to prevent Use-After-Free in delayed callbacks
        if (m_aliveFlag)
        {
            m_aliveFlag->store(false);
        }

        // アクティブパネルをクリア
        if (s_activePanel == this)
        {
            s_activePanel = nullptr;
        }

        // Python出力コールバックをクリア
        PythonHost::Instance().ClearOutputCallback();

        // エディタのサブクラスを解除
        if (m_scriptEdit && m_prevEditorProc)
        {
            RemovePropA(m_scriptEdit, EDITOR_SUBCLASS_PROPERTY);
            SetWindowLongPtrA(m_scriptEdit, GWLP_WNDPROC,
                              reinterpret_cast<LONG_PTR>(m_prevEditorProc));
        }

        // ウィンドウプロシージャを復元
        if (m_containerHwnd && m_prevWindowProc)
        {
            SetWindowLongPtrA(m_containerHwnd, GWLP_WNDPROC,
                              reinterpret_cast<LONG_PTR>(m_prevWindowProc));
        }

        // プロパティを削除
        if (m_containerHwnd)
        {
            RemovePropA(m_containerHwnd, PANEL_WINDOW_PROPERTY);
        }

        // フォントを削除
        if (m_font)
        {
            DeleteObject(m_font);
            m_font = nullptr;
        }

        // 背景ブラシを削除
        if (m_bgBrush)
        {
            DeleteObject(m_bgBrush);
            m_bgBrush = nullptr;
        }

        // コントロールは親ウィンドウと共に破棄される
    }

    void PanelUI_Win::CreateControls()
    {
        HINSTANCE hInstance = nullptr;

        // ===== 上部: コンソール出力エリア（読み取り専用） =====
        // WS_EX_CLIENTEDGEを使わず、フラットなスタイルで作成（境界線は別途描画）
        m_logEdit = CreateWindowExW(
            0, // 拡張スタイルなし（WS_EX_CLIENTEDGEの白い境界線を避ける）
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
                ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            0, 0, 100, 100,
            m_containerHwnd,
            reinterpret_cast<HMENU>(static_cast<UINT_PTR>(static_cast<WORD>(PanelControlID::LogEdit))),
            hInstance,
            nullptr);

        if (m_logEdit && m_font)
        {
            SendMessage(m_logEdit, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
            // Increase text limit (default 30,000 may cause issues with EM_REPLACESEL)
            SendMessage(m_logEdit, EM_SETLIMITTEXT, static_cast<WPARAM>(1024 * 1024), 0);
            // ダークモードを適用（スクロールバーも暗くなる）
            EnableDarkModeForControl(m_logEdit);
        }

        // ===== 下部: スクリプトエディタ（マルチライン、編集可能） =====
        // WS_EX_CLIENTEDGEを使わず、フラットなスタイルで作成（境界線は別途描画）
        m_scriptEdit = CreateWindowExW(
            0, // 拡張スタイルなし（WS_EX_CLIENTEDGEの白い境界線を避ける）
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
                ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN,
            0, 0, 100, 100,
            m_containerHwnd,
            reinterpret_cast<HMENU>(static_cast<UINT_PTR>(static_cast<WORD>(PanelControlID::ScriptEdit))),
            hInstance,
            nullptr);

        if (m_scriptEdit && m_font)
        {
            SendMessage(m_scriptEdit, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
            // ダークモードを適用（スクロールバーも暗くなる）
            EnableDarkModeForControl(m_scriptEdit);

            // エディタをサブクラス化してCtrl+Enterをキャッチ
            m_prevEditorProc = reinterpret_cast<WindowProc>(
                GetWindowLongPtr(m_scriptEdit, GWLP_WNDPROC));
            SetPropA(m_scriptEdit, EDITOR_SUBCLASS_PROPERTY, static_cast<HANDLE>(this));
            SetWindowLongPtrA(m_scriptEdit, GWLP_WNDPROC,
                              reinterpret_cast<LONG_PTR>(StaticEditorWndProc));
        }

        // ボタンなし - Ctrl+Enterで実行、フライアウトメニューでクリア
    }

    void PanelUI_Win::LayoutControls()
    {
        RECT clientRect;
        GetClientRect(m_containerHwnd, &clientRect);

        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        // 有効な描画領域（マージンを除く）
        int contentHeight = height - MARGIN * 2;

        // スプリッター位置
        int splitterY = MARGIN + static_cast<int>(contentHeight * m_splitRatio);

        // ===== 上部: コンソール出力 =====
        int logHeight = splitterY - MARGIN;
        if (m_logEdit)
        {
            SetWindowPos(m_logEdit, nullptr,
                         MARGIN, MARGIN,
                         width - MARGIN * 2, logHeight,
                         SWP_NOZORDER);
        }

        // ===== 下部: スクリプトエディタ =====
        int editorY = splitterY + SPLITTER_HEIGHT;
        int editorHeight = height - editorY - MARGIN;
        if (m_scriptEdit)
        {
            SetWindowPos(m_scriptEdit, nullptr,
                         MARGIN, editorY,
                         width - MARGIN * 2, editorHeight,
                         SWP_NOZORDER);
        }
    }

    void PanelUI_Win::AppendLog(const std::string &text, bool isError)
    {
        if (!m_logEdit)
            return;

        // \n を \r\n に変換（Windows Editコントロール用）
        std::string normalizedText;
        normalizedText.reserve(text.size() * 2);
        for (size_t i = 0; i < text.size(); ++i)
        {
            if (text[i] == '\n')
            {
                // \r\n でない場合のみ \r を追加
                if (i == 0 || text[i - 1] != '\r')
                {
                    normalizedText += '\r';
                }
            }
            normalizedText += text[i];
        }

        // Temporarily disable read-only for EM_REPLACESEL
        // (EM_REPLACESEL does not work reliably with ES_READONLY)
        SendMessage(m_logEdit, EM_SETREADONLY, FALSE, 0);

        // Move cursor to the very end of the text.
        // Use large constant instead of GetWindowTextLengthW which can return
        // inaccurate values with mixed ANSI/Unicode text, causing mid-text insertion.
        SendMessage(m_logEdit, EM_SETSEL, static_cast<WPARAM>(0x7FFFFFFF), static_cast<LPARAM>(0x7FFFFFFF));

        // UTF-8からワイド文字列に変換
        int wideLen = MultiByteToWideChar(CP_UTF8, 0, normalizedText.c_str(), -1, nullptr, 0);
        if (wideLen > 0)
        {
            std::wstring wideText(wideLen - 1, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, normalizedText.c_str(), -1, &wideText[0], wideLen);

            // テキストを追加
            SendMessageW(m_logEdit, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(wideText.c_str()));
        }

        // Restore read-only
        SendMessage(m_logEdit, EM_SETREADONLY, TRUE, 0);

        // スクロールして最新の行を表示
        SendMessage(m_logEdit, EM_SCROLLCARET, 0, 0);
    }

    void PanelUI_Win::ClearLog()
    {
        if (m_logEdit)
        {
            SetWindowTextW(m_logEdit, L"");
        }
    }

    void PanelUI_Win::ClearEditor()
    {
        if (m_scriptEdit)
        {
            SetWindowTextW(m_scriptEdit, L"");
        }
    }

    std::string PanelUI_Win::GetSelectedTextOrAll()
    {
        if (!m_scriptEdit)
            return "";

        // 選択範囲を取得
        DWORD selStart = 0, selEnd = 0;
        SendMessage(m_scriptEdit, EM_GETSEL, reinterpret_cast<WPARAM>(&selStart), reinterpret_cast<LPARAM>(&selEnd));

        std::wstring wideText;

        if (selStart != selEnd)
        {
            // 選択範囲がある場合、選択テキストを取得
            int textLen = GetWindowTextLengthW(m_scriptEdit);
            if (textLen > 0)
            {
                std::wstring fullText(textLen + 1, L'\0');
                GetWindowTextW(m_scriptEdit, &fullText[0], textLen + 1);
                wideText = fullText.substr(selStart, selEnd - selStart);
            }
        }
        else
        {
            // 選択がない場合、全テキストを取得
            int textLen = GetWindowTextLengthW(m_scriptEdit);
            if (textLen > 0)
            {
                wideText.resize(textLen + 1);
                GetWindowTextW(m_scriptEdit, &wideText[0], textLen + 1);
                wideText.resize(textLen); // null終端を除去
            }
        }

        // UTF-16からUTF-8に変換
        if (wideText.empty())
            return "";

        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideText.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (utf8Len > 0)
        {
            std::string utf8Text(utf8Len - 1, '\0');
            WideCharToMultiByte(CP_UTF8, 0, wideText.c_str(), -1, &utf8Text[0], utf8Len, nullptr, nullptr);
            return utf8Text;
        }

        return "";
    }

    void PanelUI_Win::ExecuteSelectedCode()
    {
        std::string code = GetSelectedTextOrAll();
        if (code.empty())
            return;

        // Build the entire prompt display as a single string
        // (avoids multiple EM_REPLACESEL calls which can cause text corruption)
        std::string display;
        std::istringstream stream(code);
        std::string line;
        bool firstLine = true;
        while (std::getline(stream, line))
        {
            // Remove trailing \r (Windows line endings from EDIT control)
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }
            if (firstLine)
            {
                display += ">>> " + line + "\n";
                firstLine = false;
            }
            else
            {
                display += "... " + line + "\n";
            }
        }
        AppendLog(display);

        // Execute Python code (on main thread via IdleHandler)
        // SAFETY: Capture m_aliveFlag to prevent Use-After-Free if execution occurs after destruction
        auto aliveFlag = m_aliveFlag;
        IdleHandler::Instance().EnqueueTask([this, aliveFlag, code]()
                                            {
                                                // Check if panel is still alive before accessing 'this'
                                                if (!aliveFlag || !aliveFlag->load())
                                                {
                                                    return;
                                                }

                                                std::string error;
                                                bool success = PythonHost::Instance().ExecuteString(code, error);

                                                if (!success && !error.empty())
                                                {
                                                    // Display traceback/error info
                                                    AppendLog(error + "\n", true);
                                                }
                                            });
    }

    void PanelUI_Win::InvalidateAll()
    {
        if (m_containerHwnd)
        {
            InvalidateRect(m_containerHwnd, nullptr, FALSE);
        }
    }

    // エディタ用サブクラスプロシージャ（Ctrl+Enterを処理）
    LRESULT CALLBACK PanelUI_Win::StaticEditorWndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam)
    {
        PanelUI_Win *panel = reinterpret_cast<PanelUI_Win *>(
            GetPropA(hWnd, EDITOR_SUBCLASS_PROPERTY));

        if (panel)
        {
            // Ctrl+Enterをチェック（WM_KEYDOWN）
            if (message == WM_KEYDOWN && wParam == VK_RETURN)
            {
                if (GetKeyState(VK_CONTROL) & 0x8000)
                {
                    // Ctrl+Enter: コードを実行
                    panel->ExecuteSelectedCode();
                    return 0; // メッセージを処理済み
                }
            }

            // Ctrl+Enterの後の改行文字を無視（WM_CHAR）
            if (message == WM_CHAR && (wParam == '\r' || wParam == '\n'))
            {
                if (GetKeyState(VK_CONTROL) & 0x8000)
                {
                    return 0; // Ctrl押下中の改行は無視
                }
            }

            // 元のプロシージャを呼び出す
            if (panel->m_prevEditorProc)
            {
                return CallWindowProc(panel->m_prevEditorProc, hWnd, message, wParam, lParam);
            }
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    // ウィンドウプロシージャ
    LRESULT CALLBACK PanelUI_Win::StaticOSWindowWndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam)
    {
        PanelUI_Win *panel = reinterpret_cast<PanelUI_Win *>(
            GetPropA(hWnd, PANEL_WINDOW_PROPERTY));

        if (panel)
        {
            return panel->OSWindowWndProc(hWnd, message, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LRESULT PanelUI_Win::OSWindowWndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam)
    {
        bool handled = false;

        switch (message)
        {
        case WM_SIZE:
            OnSize(LOWORD(lParam), HIWORD(lParam));
            // Forward to AE's default handler - don't consume the message
            // handled = true;
            break;

        case WM_COMMAND:
            OnCommand(LOWORD(wParam), HIWORD(wParam));
            // Forward to AE's default handler - don't consume the message
            // handled = true;
            break;

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
            // Editコントロールの色をAEダークテーマに合わせる
            {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                SetTextColor(hdc, COLOR_TEXT);
                SetBkColor(hdc, COLOR_BG);
                return reinterpret_cast<LRESULT>(m_bgBrush);
            }

        case WM_ERASEBKGND:
            // 親ウィンドウの背景をダークに塗りつぶす
            {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                RECT rect;
                GetClientRect(hWnd, &rect);
                // 境界線色でパネル全体の背景を塗りつぶす
                // （Editコントロールとの間に暗い隙間ができて境界線のように見える）
                HBRUSH borderBrush = CreateSolidBrush(COLOR_BORDER);
                FillRect(hdc, &rect, borderBrush);
                DeleteObject(borderBrush);
                return TRUE; // 背景描画完了
            }

        case WM_DESTROY:
            // クリーンアップ
            break;
        }

        // 元のウィンドウプロシージャを呼び出す
        if (m_prevWindowProc && !handled)
        {
            return CallWindowProc(m_prevWindowProc, hWnd, message, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void PanelUI_Win::OnSize(int width, int height)
    {
        LayoutControls();
        InvalidateAll();
    }

    void PanelUI_Win::OnCommand(WORD controlId, WORD notifyCode)
    {
        // ボタンなし - フライアウトメニューで処理
        (void)controlId;
        (void)notifyCode;
    }

    void PanelUI_Win::OnPaint(HDC hdc)
    {
        // 必要に応じてカスタム描画
    }

    // スナップサイズ
    void PanelUI_Win::GetSnapSizes(A_LPoint *snapSizes, A_long *numSizesP)
    {
        snapSizes[0].x = 300;
        snapSizes[0].y = 200;
        snapSizes[1].x = 500;
        snapSizes[1].y = 400;
        *numSizesP = 2;
    }

    // フライアウトメニュー
    void PanelUI_Win::PopulateFlyout(AEGP_FlyoutMenuItem *itemsP, A_long *in_out_numItemsP)
    {
        AEGP_FlyoutMenuItem menu[] = {
            {1, AEGP_FlyoutMenuMarkType_NORMAL, TRUE,
             static_cast<A_long>(PanelFlyoutCmd::ExecuteCode),
             reinterpret_cast<const A_u_char *>("Execute (Ctrl+Enter)")},
            {1, AEGP_FlyoutMenuMarkType_SEPARATOR, TRUE, AEGP_FlyoutMenuCmdID_NONE, nullptr},
            {1, AEGP_FlyoutMenuMarkType_NORMAL, TRUE,
             static_cast<A_long>(PanelFlyoutCmd::ClearLog),
             reinterpret_cast<const A_u_char *>("Clear Console")},
            {1, AEGP_FlyoutMenuMarkType_NORMAL, TRUE,
             static_cast<A_long>(PanelFlyoutCmd::ClearEditor),
             reinterpret_cast<const A_u_char *>("Clear Editor")},
            {1, AEGP_FlyoutMenuMarkType_SEPARATOR, TRUE, AEGP_FlyoutMenuCmdID_NONE, nullptr},
            {1, AEGP_FlyoutMenuMarkType_NORMAL, TRUE,
             static_cast<A_long>(PanelFlyoutCmd::CopyLog),
             reinterpret_cast<const A_u_char *>("Copy Console to Clipboard")},
        };

        A_long menuSize = sizeof(menu) / sizeof(AEGP_FlyoutMenuItem);

        if (menuSize <= *in_out_numItemsP)
        {
            std::copy(&menu[0], &menu[menuSize], itemsP);
        }
        else
        {
            std::copy(&menu[0], &menu[*in_out_numItemsP], itemsP);
        }

        *in_out_numItemsP = menuSize;
    }

    void PanelUI_Win::DoFlyoutCommand(AEGP_FlyoutMenuCmdID commandID)
    {
        auto cmd = static_cast<PanelFlyoutCmd>(commandID);

        switch (cmd)
        {
        case PanelFlyoutCmd::ExecuteCode:
            ExecuteSelectedCode();
            break;

        case PanelFlyoutCmd::ClearLog:
            ClearLog();
            break;

        case PanelFlyoutCmd::ClearEditor:
            ClearEditor();
            break;

        case PanelFlyoutCmd::CopyLog:
            if (m_logEdit)
            {
                // ログのテキストをクリップボードにコピー
                int textLen = GetWindowTextLengthW(m_logEdit);
                if (textLen > 0)
                {
                    std::wstring text(textLen + 1, L'\0');
                    GetWindowTextW(m_logEdit, &text[0], textLen + 1);

                    if (OpenClipboard(m_containerHwnd))
                    {
                        EmptyClipboard();
                        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (textLen + 1) * sizeof(wchar_t));
                        if (hMem)
                        {
                            wchar_t *pMem = static_cast<wchar_t *>(GlobalLock(hMem));
                            if (pMem)
                            {
                                wcscpy_s(pMem, textLen + 1, text.c_str());
                                GlobalUnlock(hMem);
                                SetClipboardData(CF_UNICODETEXT, hMem);
                            }
                        }
                        CloseClipboard();
                    }
                }
            }
            break;

        case PanelFlyoutCmd::WordWrap:
            m_wordWrap = !m_wordWrap;
            // ワードラップの切り替えは再作成が必要
            break;
        }

        InvalidateAll();
    }

    // 静的コールバックアダプター
    A_Err PanelUI_Win::S_GetSnapSizes(AEGP_PanelRefcon refcon, A_LPoint *snapSizes, A_long *numSizesP)
    {
        try
        {
            reinterpret_cast<PanelUI_Win *>(refcon)->GetSnapSizes(snapSizes, numSizesP);
            return A_Err_NONE;
        }
        catch (...)
        {
            return A_Err_GENERIC;
        }
    }

    A_Err PanelUI_Win::S_PopulateFlyout(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuItem *itemsP, A_long *in_out_numItemsP)
    {
        try
        {
            reinterpret_cast<PanelUI_Win *>(refcon)->PopulateFlyout(itemsP, in_out_numItemsP);
            return A_Err_NONE;
        }
        catch (...)
        {
            return A_Err_GENERIC;
        }
    }

    A_Err PanelUI_Win::S_DoFlyoutCommand(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuCmdID commandID)
    {
        try
        {
            reinterpret_cast<PanelUI_Win *>(refcon)->DoFlyoutCommand(commandID);
            return A_Err_NONE;
        }
        catch (...)
        {
            return A_Err_GENERIC;
        }
    }

} // namespace PyAE

#endif // _WIN32
