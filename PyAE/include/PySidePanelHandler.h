// PySidePanelHandler.h
// PyAE - Python for After Effects
// PySide6パネル管理（複数パネル対応）

#pragma once

#include <string>
#include <memory>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <functional>

#include "AE_GeneralPlug.h"
#include "AE_GeneralPlugPanels.h"

namespace PyAE {

// 前方宣言
class PySidePanelUI_Win;

/**
 * @brief パネル定義
 */
struct PanelDefinition {
    std::string name;           // 表示名
    std::string matchName;      // AE内部識別名
    std::string factoryPath;    // Pythonファクトリー関数パス（"module:function"）
    AEGP_Command toggleCommand = 0;  // メニューコマンドID
    bool registered = false;
};

/**
 * @brief パネルインスタンス情報
 */
struct PanelInstance {
    std::string matchName;      // どのパネル定義か
    AEGP_PanelH panelH;         // AEパネルハンドル
    PySidePanelUI_Win* panelUI; // パネルUI
};

/**
 * @brief PySide6埋め込み用パネルハンドラー（複数パネル対応）
 *
 * panels_config.py から読み込んだ設定に基づいて
 * 複数のPySide6パネルを登録・管理します。
 */
class PySidePanelHandler {
public:
    static PySidePanelHandler& Instance() {
        static PySidePanelHandler instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize();
    void Shutdown();

    // パネル設定の読み込み
    bool LoadPanelConfig();

    // パネル登録（全定義を登録）
    bool RegisterPanel();
    void UnregisterPanel();

    // 個別パネルの表示切り替え
    void ToggleVisibility(const std::string& matchName);
    bool IsShown(const std::string& matchName) const;

    // デフォルトパネル用（後方互換性）
    void ToggleVisibility();
    bool IsShown() const;
    AEGP_Command GetToggleCommand() const;

    // コマンドIDからパネルを特定
    PanelDefinition* GetPanelByCommand(AEGP_Command command);

    // パネル定義の取得
    const std::vector<PanelDefinition>& GetPanelDefinitions() const { return m_panelDefinitions; }

    // 状態
    bool IsInitialized() const { return m_initialized; }
    bool IsPanelSupported() const { return m_panelSupported; }

    // デフォルトパネル（設定ファイルに定義がない場合に使用）
    static constexpr const char* DEFAULT_PANEL_MATCH_NAME = "PyAE_PySide";
    static constexpr const char* DEFAULT_PANEL_DISPLAY_NAME = "PyAE Panel";

    // パネルインスタンスの登録（パネルUI作成時に呼ばれる）
    void RegisterPanelInstance(const std::string& matchName, AEGP_PanelH panelH, PySidePanelUI_Win* panelUI);
    void UnregisterPanelInstance(const std::string& matchName);

    // パネルインスタンスの取得
    PanelInstance* GetPanelInstance(const std::string& matchName);

    // Pythonファクトリー関数を呼び出してウィジェットを生成・埋め込み
    bool CreateWidgetForPanel(const std::string& matchName);

private:
    PySidePanelHandler() = default;
    ~PySidePanelHandler() = default;

    PySidePanelHandler(const PySidePanelHandler&) = delete;
    PySidePanelHandler& operator=(const PySidePanelHandler&) = delete;

    // 個別パネルの登録
    bool RegisterSinglePanel(PanelDefinition& panel);

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
        const std::string& matchName,
        AEGP_PlatformViewRef container,
        AEGP_PanelH panelH,
        AEGP_PanelFunctions1* outFunctionTable,
        AEGP_PanelRefcon* outRefcon);

    bool m_initialized = false;
    bool m_panelSupported = false;
    bool m_configLoaded = false;

    // パネル定義（設定ファイルから読み込み）
    std::vector<PanelDefinition> m_panelDefinitions;

    // matchName -> PanelDefinitionのインデックス
    std::unordered_map<std::string, size_t> m_matchNameToIndex;

    // パネルインスタンス（matchName -> PanelInstance）
    std::unordered_map<std::string, PanelInstance> m_panelInstances;
};

// コマンドハンドラ用コールバック
A_Err PySidePanelHandler_OnCommand(AEGP_Command command);
A_Err PySidePanelHandler_OnUpdateMenu();

} // namespace PyAE
