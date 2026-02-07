// MenuHandler.cpp
// PyAE - Python for After Effects
// メニューハンドラの実装

#include "MenuHandler.h"
#include "PyMenuClasses.h"
#include "PluginState.h"
#include "ScriptRunner.h"
#include "PanelHandler.h"
#include "PySidePanelHandler.h"
#include "Logger.h"
#include "ErrorHandling.h"

#include <Windows.h>
#include <shellapi.h>

namespace PyAE {

// UTF-8からUTF-16への変換
static std::wstring Utf8ToUtf16(const std::string& utf8) {
    if (utf8.empty()) return L"";

    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (size <= 0) return L"";

    std::wstring result(size - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &result[0], size);
    return result;
}

bool MenuHandler::Initialize() {
    if (m_initialized) {
        PYAE_LOG_WARNING("MenuHandler", "Already initialized");
        return true;
    }

    PYAE_LOG_INFO("MenuHandler", "Initializing MenuHandler...");

    m_initialized = true;

    PYAE_LOG_INFO("MenuHandler", "MenuHandler initialized");
    return true;
}

void MenuHandler::Shutdown() {
    if (!m_initialized) {
        return;
    }

    PYAE_LOG_INFO("MenuHandler", "Shutting down MenuHandler...");

    m_dynamicItems.clear();
    m_commandToIndex.clear();
    m_scriptMenuItems.clear();

    m_initialized = false;

    PYAE_LOG_INFO("MenuHandler", "MenuHandler shutdown complete");
}

bool MenuHandler::CreatePyAEMenu() {
    PYAE_LOG_INFO("MenuHandler", "Creating PyAE menu...");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    PYAE_LOG_DEBUG("MenuHandler", std::string("commandSuite: ") + (suites.commandSuite ? "OK" : "NULL"));
    PYAE_LOG_DEBUG("MenuHandler", std::string("registerSuite: ") + (suites.registerSuite ? "OK" : "NULL"));

    if (!suites.commandSuite || !suites.registerSuite) {
        PYAE_LOG_ERROR("MenuHandler", "Required suites not available");
        return false;
    }

    A_Err err = A_Err_NONE;

    try {
        // メインメニューコマンドを登録
        // Run Script...
        err = suites.commandSuite->AEGP_GetUniqueCommand(&m_cmdRunScript);
        PYAE_LOG_DEBUG("MenuHandler", "GetUniqueCommand RunScript: err=" + std::to_string(err) + ", cmd=" + std::to_string(m_cmdRunScript));
        PYAE_CHECK_ERR(err, "GetUniqueCommand RunScript");

        // Open Scripts Folder
        err = suites.commandSuite->AEGP_GetUniqueCommand(&m_cmdOpenScriptsFolder);
        PYAE_LOG_DEBUG("MenuHandler", "GetUniqueCommand OpenScriptsFolder: err=" + std::to_string(err));
        PYAE_CHECK_ERR(err, "GetUniqueCommand OpenScriptsFolder");

        // Open Console (REPL有効時のみ)
#ifdef PYAE_ENABLE_REPL
        err = suites.commandSuite->AEGP_GetUniqueCommand(&m_cmdOpenConsole);
        PYAE_CHECK_ERR(err, "GetUniqueCommand OpenConsole");
#endif

        // Reload Modules
        err = suites.commandSuite->AEGP_GetUniqueCommand(&m_cmdReloadModules);
        PYAE_CHECK_ERR(err, "GetUniqueCommand ReloadModules");

        // About
        err = suites.commandSuite->AEGP_GetUniqueCommand(&m_cmdAbout);
        PYAE_LOG_DEBUG("MenuHandler", "GetUniqueCommand About: err=" + std::to_string(err));
        PYAE_CHECK_ERR(err, "GetUniqueCommand About");

        // メニューにコマンドを追加
        err = suites.commandSuite->AEGP_InsertMenuCommand(
            m_cmdRunScript,
            "Run Script...",
            AEGP_Menu_WINDOW,
            AEGP_MENU_INSERT_SORTED
        );
        PYAE_LOG_DEBUG("MenuHandler", "InsertMenuCommand result: " + std::to_string(err));
        PYAE_CHECK_ERR(err, "InsertMenuCommand RunScript");

        PYAE_LOG_INFO("MenuHandler", "PyAE menu created successfully");
        return true;

    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("MenuHandler", std::string("Failed to create menu: ") + e.what());
        return false;
    }
}

bool MenuHandler::OnCommand(AEGP_Command command) {
    if (!m_initialized) {
        return false;  // 初期化されていない場合は処理しない
    }

    // Run Script
    if (command == m_cmdRunScript) {
        OnRunScript();
        return true;  // 処理した
    }

    // Open Scripts Folder
    if (command == m_cmdOpenScriptsFolder) {
        OnOpenScriptsFolder();
        return true;  // 処理した
    }

#ifdef PYAE_ENABLE_REPL
    // Open Console
    if (command == m_cmdOpenConsole) {
        OnOpenConsole();
        return true;  // 処理した
    }
#endif

    // Reload Modules
    if (command == m_cmdReloadModules) {
        OnReloadModules();
        return true;  // 処理した
    }

    // About
    if (command == m_cmdAbout) {
        OnAbout();
        return true;  // 処理した
    }

    // パネルコマンド (PyAE Console)
    if (command == PanelHandler::Instance().GetToggleCommand()) {
        PanelHandler_OnCommand(command);
        return true;  // 処理した
    }

    // PySide パネルコマンド（複数パネル対応）
    if (PySidePanelHandler::Instance().GetPanelByCommand(command)) {
        PySidePanelHandler_OnCommand(command);
        return true;  // 処理した
    }

    // 動的メニュー項目
    auto it = m_commandToIndex.find(command);
    if (it != m_commandToIndex.end() && it->second < m_dynamicItems.size()) {
        const auto& item = m_dynamicItems[it->second];
        if (item.callback) {
            item.callback();
        }
        return true;  // 処理した
    }

    // Python メニューコールバック (MenuCallbackBridge)
    if (MenuCallbackBridge::Instance().InvokeCallback(command)) {
        return true;  // Pythonコールバックが処理した
    }

    return false;  // このプラグインのコマンドではない
}

A_Err MenuHandler::OnUpdateMenu(AEGP_Command command) {
    // メニュー項目の有効/無効を更新
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.commandSuite) {
        return A_Err_NONE;
    }

    // 全てのコマンドを有効に
    A_Boolean enabled = TRUE;
    A_Boolean checked = FALSE;

    if (command == m_cmdRunScript) {
        suites.commandSuite->AEGP_EnableCommand(command);
    } else if (command == m_cmdOpenScriptsFolder) {
        suites.commandSuite->AEGP_EnableCommand(command);
    }
#ifdef PYAE_ENABLE_REPL
    else if (command == m_cmdOpenConsole) {
        suites.commandSuite->AEGP_EnableCommand(command);
    }
#endif
    else if (command == m_cmdReloadModules) {
        suites.commandSuite->AEGP_EnableCommand(command);
    } else if (command == m_cmdAbout) {
        suites.commandSuite->AEGP_EnableCommand(command);
    }

    return A_Err_NONE;
}

void MenuHandler::OnRunScript() {
    PYAE_LOG_INFO("MenuHandler", "Run Script command invoked");

    auto scriptPath = ScriptRunner::Instance().ShowOpenScriptDialog();
    if (scriptPath) {
        auto result = ScriptRunner::Instance().RunFile(*scriptPath);
        if (!result.success) {
            PYAE_LOG_ERROR("MenuHandler", "Script execution failed: " + result.error);
            // TODO: エラーダイアログ表示
        }
    }
}

void MenuHandler::OnOpenScriptsFolder() {
    PYAE_LOG_INFO("MenuHandler", "Open Scripts Folder command invoked");

    auto scriptsDir = PluginState::Instance().GetScriptsDir();
    if (std::filesystem::exists(scriptsDir)) {
        ShellExecuteW(nullptr, L"explore", scriptsDir.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

void MenuHandler::OnOpenConsole() {
#ifdef PYAE_ENABLE_REPL
    PYAE_LOG_INFO("MenuHandler", "Open Console command invoked");
    // TODO: コンソールウィンドウを開く
#endif
}

void MenuHandler::OnReloadModules() {
    PYAE_LOG_INFO("MenuHandler", "Reload Modules command invoked");
    // TODO: Pythonモジュールのリロード
}

void MenuHandler::OnAbout() {
    PYAE_LOG_INFO("MenuHandler", "About command invoked");

    std::wstring message =
        L"PyAE - Python for After Effects\n"
        L"Version " PYAE_VERSION L"\n\n"
        L"Python integration plugin for Adobe After Effects.\n\n"
        L"© 2024 VTech Studio";

    MessageBoxW(nullptr, message.c_str(), L"About PyAE", MB_OK | MB_ICONINFORMATION);
}

bool MenuHandler::AddMenuItem(const std::string& name, std::function<void()> callback) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.commandSuite) {
        return false;
    }

    AEGP_Command command;
    A_Err err = suites.commandSuite->AEGP_GetUniqueCommand(&command);
    if (err != A_Err_NONE) {
        return false;
    }

    MenuItem item;
    item.name = name;
    item.commandId = command;
    item.callback = std::move(callback);

    size_t index = m_dynamicItems.size();
    m_dynamicItems.push_back(std::move(item));
    m_commandToIndex[command] = index;

    return true;
}

bool MenuHandler::RemoveMenuItem(const std::string& name) {
    // TODO: メニュー項目の削除
    return false;
}

void MenuHandler::RefreshScriptMenu() {
    // TODO: スクリプトメニューの更新
}

// C関数: コマンドフックコールバック
A_Err PyAE_CommandHook(
    AEGP_GlobalRefcon plugin_refconP,
    AEGP_CommandRefcon refconP,
    AEGP_Command command,
    AEGP_HookPriority hook_priority,
    A_Boolean already_handledB,
    A_Boolean* handledPB)
{
    if (already_handledB) {
        *handledPB = FALSE;
        return A_Err_NONE;
    }

    // MenuHandlerに処理を委譲
    // OnCommandは自分のコマンドを処理した場合のみtrueを返す
    bool handled = MenuHandler::Instance().OnCommand(command);

    // このプラグインのコマンドを処理した場合のみTRUEに
    // 処理しなかった場合はFALSEにして、AE本体や他のプラグインに委ねる
    *handledPB = handled ? TRUE : FALSE;

    return A_Err_NONE;
}

A_Err PyAE_UpdateMenuHook(
    AEGP_GlobalRefcon plugin_refconP,
    AEGP_UpdateMenuRefcon refconP,
    AEGP_WindowType active_window)
{
    // 全メニューコマンドを有効化
    auto& handler = MenuHandler::Instance();
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (suites.commandSuite) {
        // Run Script コマンドを有効化
        if (handler.m_cmdRunScript != 0) {
            suites.commandSuite->AEGP_EnableCommand(handler.m_cmdRunScript);
        }
        // Open Scripts Folder コマンドを有効化
        if (handler.m_cmdOpenScriptsFolder != 0) {
            suites.commandSuite->AEGP_EnableCommand(handler.m_cmdOpenScriptsFolder);
        }
#ifdef PYAE_ENABLE_REPL
        // Open Console コマンドを有効化
        if (handler.m_cmdOpenConsole != 0) {
            suites.commandSuite->AEGP_EnableCommand(handler.m_cmdOpenConsole);
        }
#endif
        // Reload Modules コマンドを有効化
        if (handler.m_cmdReloadModules != 0) {
            suites.commandSuite->AEGP_EnableCommand(handler.m_cmdReloadModules);
        }
        // About コマンドを有効化
        if (handler.m_cmdAbout != 0) {
            suites.commandSuite->AEGP_EnableCommand(handler.m_cmdAbout);
        }
    }

    // パネルメニューの更新
    PanelHandler_OnUpdateMenu();
    PySidePanelHandler_OnUpdateMenu();

    // Python メニューコマンドを有効化
    auto registeredCmds = MenuCallbackBridge::Instance().GetRegisteredCommands();
    for (AEGP_Command cmd : registeredCmds) {
        auto enabledState = MenuCallbackBridge::Instance().InvokeUpdateCallback(cmd);
        if (enabledState.has_value()) {
            if (*enabledState) {
                suites.commandSuite->AEGP_EnableCommand(cmd);
            } else {
                suites.commandSuite->AEGP_DisableCommand(cmd);
            }
        } else {
            // デフォルトで有効化
            suites.commandSuite->AEGP_EnableCommand(cmd);
        }
    }

    return A_Err_NONE;
}

} // namespace PyAE
