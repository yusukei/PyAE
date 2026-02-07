// PluginState.h
// PyAE - Python for After Effects
// プラグイン状態管理（シングルトン）

#pragma once

// C++標準ライブラリを先にインクルード
#include <atomic>
#include <filesystem>
#include <string>
#include <functional>
#include <memory>

// After Effects SDK
#include "AE_GeneralPlug.h"
#include "AE_GeneralPlugPanels.h"
#include "AEGP_SuiteHandler.h"

// PyAE
#include "Logger.h"
#include "SuiteManager.h"
#include "PathManager.h"
#include "WinSync.h"

namespace PyAE {

class PluginState {
public:
    // シングルトンアクセス
    static PluginState& Instance() {
        static PluginState instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize(SPBasicSuite* basicSuite, AEGP_PluginID pluginId);
    void Shutdown();

    // 状態チェック
    bool IsInitialized() const { return m_initialized.load(); }
    bool IsShuttingDown() const { return m_shuttingDown.load(); }
    void SetShuttingDown(bool value) { m_shuttingDown.store(value); }

    // プラグイン情報（読み取り専用、ロック不要）
    AEGP_PluginID GetPluginID() const { return m_pluginId; }
    SPBasicSuite* GetBasicSuite() const { return m_basicSuite; }

    // スイート管理（SuiteManagerへ委譲）
    const SuiteCache& GetSuites() const { return m_suiteManager->GetSuites(); }
    void UpdateSuites(const std::function<void(SuiteCache&)>& updater) {
        m_suiteManager->UpdateSuites(updater);
    }
    AEGP_SuiteHandler& GetSuiteHandler() { return m_suiteManager->GetSuiteHandler(); }

    // パス管理（PathManagerへ委譲）
    const std::filesystem::path& GetPluginDir() const { return m_pathManager.GetPluginDir(); }
    const std::filesystem::path& GetScriptsDir() const { return m_pathManager.GetScriptsDir(); }

    // 自動テスト関連
    void CheckAndQueueAutoTest();

    // メニューコマンドID管理
    void SetMenuCommandID(AEGP_Command cmd) {
        WinUniqueLock lock(m_stateMutex);
        m_menuCommandId = cmd;
    }
    AEGP_Command GetMenuCommandID() const {
        WinSharedLock lock(m_stateMutex);
        return m_menuCommandId;
    }

private:
    PluginState() = default;
    ~PluginState() = default;

    PluginState(const PluginState&) = delete;
    PluginState& operator=(const PluginState&) = delete;

    // 基本情報
    SPBasicSuite* m_basicSuite = nullptr;
    AEGP_PluginID m_pluginId = 0;

    // 管理クラス（ファサードパターン）
    std::unique_ptr<SuiteManager> m_suiteManager;
    PathManager m_pathManager;

    // 状態関連
    mutable WinSharedMutex m_stateMutex;
    AEGP_Command m_menuCommandId = 0;

    // 原子的フラグ
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_shuttingDown{false};
};

} // namespace PyAE
