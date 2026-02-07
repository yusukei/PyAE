// MenuHandler.h
// PyAE - Python for After Effects
// メニュー管理

#pragma once

// C++標準ライブラリを先にインクルード
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

// After Effects SDK
#include "AE_GeneralPlug.h"

namespace PyAE {

// メニュー項目情報
struct MenuItem {
    std::string name;
    std::string shortcut;
    AEGP_Command commandId = 0;
    std::function<void()> callback;
    bool enabled = true;
    bool checked = false;
};

class MenuHandler {
public:
    static MenuHandler& Instance() {
        static MenuHandler instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize();
    void Shutdown();

    // メニュー作成
    bool CreatePyAEMenu();

    // コマンドハンドラ
    // OnCommand: コマンドを処理した場合はtrueを返す
    bool OnCommand(AEGP_Command command);
    A_Err OnUpdateMenu(AEGP_Command command);

    // 動的メニュー項目
    bool AddMenuItem(const std::string& name, std::function<void()> callback);
    bool RemoveMenuItem(const std::string& name);

    // スクリプトメニュー更新
    void RefreshScriptMenu();

    // 状態
    bool IsInitialized() const { return m_initialized; }

    // コマンドID (UpdateMenuHookからアクセス)
    AEGP_Command m_cmdRunScript = 0;
    AEGP_Command m_cmdOpenScriptsFolder = 0;
    AEGP_Command m_cmdOpenConsole = 0;
    AEGP_Command m_cmdReloadModules = 0;
    AEGP_Command m_cmdAbout = 0;

private:
    MenuHandler() = default;
    ~MenuHandler() = default;

    MenuHandler(const MenuHandler&) = delete;
    MenuHandler& operator=(const MenuHandler&) = delete;

    // メニューアクション
    void OnRunScript();
    void OnOpenScriptsFolder();
    void OnOpenConsole();
    void OnReloadModules();
    void OnAbout();

    // ヘルパー
    bool RegisterCommand(const std::string& name, AEGP_Command* outCommand);
    bool AddToMenu(AEGP_MenuID menuId, AEGP_Command command, const std::string& name);

    bool m_initialized = false;

    // 動的メニュー項目
    std::vector<MenuItem> m_dynamicItems;
    std::unordered_map<AEGP_Command, size_t> m_commandToIndex;

    // スクリプトメニュー
    std::vector<std::pair<std::string, AEGP_Command>> m_scriptMenuItems;
};

// コマンドフックコールバック関数
extern "C" {
    A_Err PyAE_CommandHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_CommandRefcon refconP,
        AEGP_Command command,
        AEGP_HookPriority hook_priority,
        A_Boolean already_handledB,
        A_Boolean* handledPB
    );

    A_Err PyAE_UpdateMenuHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_UpdateMenuRefcon refconP,
        AEGP_WindowType active_window
    );
}

} // namespace PyAE
