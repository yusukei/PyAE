// PanelHandler.h
// PyAE - Python for After Effects
// パネルUI管理

#pragma once

// C++標準ライブラリを先にインクルード
#include <string>
#include <memory>
#include <atomic>

// After Effects SDK
#include "AE_GeneralPlug.h"
#include "AE_GeneralPlugPanels.h"

namespace PyAE {

// 前方宣言
class PanelUI_Win;

class PanelHandler {
public:
    static PanelHandler& Instance() {
        static PanelHandler instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize();
    void Shutdown();

    // パネル登録
    bool RegisterPanel();
    void UnregisterPanel();

    // パネル表示切り替え
    void ToggleVisibility();
    bool IsShown() const;

    // パネルコマンドID
    AEGP_Command GetToggleCommand() const { return m_cmdTogglePanel; }

    // 状態
    bool IsInitialized() const { return m_initialized; }
    bool IsPanelSupported() const { return m_panelSupported; }

    // パネル名（内部識別用）
    static constexpr const char* PANEL_MATCH_NAME = "PyAE_Console";
    static constexpr const char* PANEL_DISPLAY_NAME = "PyAE Console";

private:
    PanelHandler() = default;
    ~PanelHandler() = default;

    PanelHandler(const PanelHandler&) = delete;
    PanelHandler& operator=(const PanelHandler&) = delete;

    // パネル作成コールバック（静的）
    static A_Err S_CreatePanelHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_CreatePanelRefcon refconP,
        AEGP_PlatformViewRef container,
        AEGP_PanelH panelH,
        AEGP_PanelFunctions1* outFunctionTable,
        AEGP_PanelRefcon* outRefcon);

    // パネル作成（インスタンスメソッド）
    void CreatePanelHook(
        AEGP_PlatformViewRef container,
        AEGP_PanelH panelH,
        AEGP_PanelFunctions1* outFunctionTable,
        AEGP_PanelRefcon* outRefcon);

    bool m_initialized = false;
    bool m_panelSupported = false;
    bool m_panelRegistered = false;

    AEGP_Command m_cmdTogglePanel = 0;

    // パネルUIインスタンス（複数のパネルインスタンスが存在する可能性があるため）
    // 実際にはAEが管理するため、ここでは作成のみ行う
};

// コマンドハンドラ用コールバック
A_Err PanelHandler_OnCommand(AEGP_Command command);
A_Err PanelHandler_OnUpdateMenu();

} // namespace PyAE
