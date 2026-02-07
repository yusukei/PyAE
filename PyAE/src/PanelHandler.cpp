// PanelHandler.cpp
// PyAE - Python for After Effects
// パネルUI管理の実装

#include "PanelHandler.h"
#include "PluginState.h"
#include "Logger.h"
#include "ErrorHandling.h"

#ifdef _WIN32
#include "PanelUI_Win.h"
#endif

namespace PyAE {

bool PanelHandler::Initialize() {
    if (m_initialized) {
        PYAE_LOG_WARNING("PanelHandler", "Already initialized");
        return true;
    }

    PYAE_LOG_INFO("PanelHandler", "Initializing PanelHandler...");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // PanelSuiteが利用可能か確認
    if (suites.panelSuite == nullptr) {
        PYAE_LOG_WARNING("PanelHandler", "PanelSuite not available - panel UI will be disabled");
        m_panelSupported = false;
        m_initialized = true;
        return true;
    }

    m_panelSupported = true;
    m_initialized = true;

    PYAE_LOG_INFO("PanelHandler", "PanelHandler initialized (panel supported: " +
                  std::string(m_panelSupported ? "yes" : "no") + ")");

    return true;
}

void PanelHandler::Shutdown() {
    if (!m_initialized) {
        return;
    }

    PYAE_LOG_INFO("PanelHandler", "Shutting down PanelHandler...");

    if (m_panelRegistered) {
        UnregisterPanel();
    }

    m_initialized = false;

    PYAE_LOG_INFO("PanelHandler", "PanelHandler shutdown complete");
}

bool PanelHandler::RegisterPanel() {
    if (!m_initialized || !m_panelSupported) {
        PYAE_LOG_ERROR("PanelHandler", "Cannot register panel: not initialized or not supported");
        return false;
    }

    if (m_panelRegistered) {
        PYAE_LOG_WARNING("PanelHandler", "Panel already registered");
        return true;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.panelSuite || !suites.commandSuite || !suites.registerSuite) {
        PYAE_LOG_ERROR("PanelHandler", "Required suites not available");
        return false;
    }

    A_Err err = A_Err_NONE;

    try {
        // パネル表示切り替えコマンドを登録
        err = suites.commandSuite->AEGP_GetUniqueCommand(&m_cmdTogglePanel);
        if (err != A_Err_NONE) {
            PYAE_LOG_ERROR("PanelHandler", "Failed to get unique command for panel toggle");
            return false;
        }

        // メニューにコマンドを追加
        err = suites.commandSuite->AEGP_InsertMenuCommand(
            m_cmdTogglePanel,
            PANEL_DISPLAY_NAME,
            AEGP_Menu_WINDOW,
            AEGP_MENU_INSERT_SORTED);
        if (err != A_Err_NONE) {
            PYAE_LOG_ERROR("PanelHandler", "Failed to insert menu command for panel");
            return false;
        }

        // パネル作成フックを登録
        err = suites.panelSuite->AEGP_RegisterCreatePanelHook(
            state.GetPluginID(),
            reinterpret_cast<const A_u_char*>(PANEL_MATCH_NAME),
            &PanelHandler::S_CreatePanelHook,
            reinterpret_cast<AEGP_CreatePanelRefcon>(this),
            TRUE  // AEに背景描画を任せる
        );
        if (err != A_Err_NONE) {
            PYAE_LOG_ERROR("PanelHandler", "Failed to register panel create hook");
            return false;
        }

        m_panelRegistered = true;
        PYAE_LOG_INFO("PanelHandler", "Panel registered successfully");
        return true;

    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PanelHandler", std::string("Exception during panel registration: ") + e.what());
        return false;
    }
}

void PanelHandler::UnregisterPanel() {
    if (!m_panelRegistered) {
        return;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (suites.panelSuite) {
        suites.panelSuite->AEGP_UnRegisterCreatePanelHook(
            reinterpret_cast<const A_u_char*>(PANEL_MATCH_NAME));
    }

    m_panelRegistered = false;
    PYAE_LOG_INFO("PanelHandler", "Panel unregistered");
}

void PanelHandler::ToggleVisibility() {
    if (!m_panelRegistered || !m_panelSupported) {
        PYAE_LOG_WARNING("PanelHandler", "Cannot toggle visibility: panel not registered or not supported");
        return;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (suites.panelSuite) {
        A_Err err = suites.panelSuite->AEGP_ToggleVisibility(
            reinterpret_cast<const A_u_char*>(PANEL_MATCH_NAME));
        if (err != A_Err_NONE) {
            PYAE_LOG_WARNING("PanelHandler", "Failed to toggle panel visibility");
        }
    }
}

bool PanelHandler::IsShown() const {
    if (!m_panelRegistered || !m_panelSupported) {
        return false;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (suites.panelSuite) {
        A_Boolean tabShown = FALSE;
        A_Boolean frontmost = FALSE;
        A_Err err = suites.panelSuite->AEGP_IsShown(
            reinterpret_cast<const A_u_char*>(PANEL_MATCH_NAME),
            &tabShown,
            &frontmost);
        if (err == A_Err_NONE) {
            return tabShown && frontmost;
        }
    }

    return false;
}

A_Err PanelHandler::S_CreatePanelHook(
    AEGP_GlobalRefcon plugin_refconP,
    AEGP_CreatePanelRefcon refconP,
    AEGP_PlatformViewRef container,
    AEGP_PanelH panelH,
    AEGP_PanelFunctions1* outFunctionTable,
    AEGP_PanelRefcon* outRefcon)
{
    try {
        PanelHandler* handler = reinterpret_cast<PanelHandler*>(refconP);
        if (handler) {
            handler->CreatePanelHook(container, panelH, outFunctionTable, outRefcon);
        }
        return A_Err_NONE;
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PanelHandler", std::string("Exception in CreatePanelHook: ") + e.what());
        return A_Err_GENERIC;
    }
}

void PanelHandler::CreatePanelHook(
    AEGP_PlatformViewRef container,
    AEGP_PanelH panelH,
    AEGP_PanelFunctions1* outFunctionTable,
    AEGP_PanelRefcon* outRefcon)
{
    PYAE_LOG_INFO("PanelHandler", "Creating panel UI...");

#ifdef _WIN32
    // Windows版パネルUIを作成
    // OWNERSHIP: After Effects owns the panel lifecycle. AE will call this function
    // to create the panel and will destroy it when the panel is closed.
    // The PanelUI_Win object is allocated with 'new' and passed back as refcon.
    // AE does NOT automatically delete this object - we must handle deletion in
    // response to panel destruction events (e.g., WM_DESTROY or panel close callback).
    auto& state = PluginState::Instance();
    PanelUI_Win* panelUI = new PanelUI_Win(
        state.GetBasicSuite(),
        panelH,
        container,
        outFunctionTable);
    *outRefcon = reinterpret_cast<AEGP_PanelRefcon>(panelUI);
#else
    // 他のプラットフォームは未サポート
    *outRefcon = nullptr;
#endif

    PYAE_LOG_INFO("PanelHandler", "Panel UI created");
}

// コマンドハンドラ
A_Err PanelHandler_OnCommand(AEGP_Command command) {
    auto& handler = PanelHandler::Instance();

    if (command == handler.GetToggleCommand()) {
        handler.ToggleVisibility();
        return A_Err_NONE;
    }

    return A_Err_NONE;
}

A_Err PanelHandler_OnUpdateMenu() {
    auto& handler = PanelHandler::Instance();
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!handler.IsPanelSupported()) {
        return A_Err_NONE;
    }

    AEGP_Command cmd = handler.GetToggleCommand();
    if (cmd != 0 && suites.commandSuite) {
        // コマンドを有効化
        suites.commandSuite->AEGP_EnableCommand(cmd);

        // パネルが表示中の場合はチェックマークを付ける
        A_Boolean checked = handler.IsShown() ? TRUE : FALSE;
        suites.commandSuite->AEGP_CheckMarkMenuCommand(cmd, checked);
    }

    return A_Err_NONE;
}

} // namespace PyAE
