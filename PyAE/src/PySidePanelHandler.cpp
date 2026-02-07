// PySidePanelHandler.cpp
// PyAE - Python for After Effects
// PySide6パネル管理の実装（複数パネル対応）

#include "PySidePanelHandler.h"
#include "PluginState.h"
#include "PythonHost.h"
#include "IdleHandler.h"
#include "Logger.h"
#include "ErrorHandling.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#ifdef _WIN32
#include "PySidePanelUI_Win.h"
#endif

namespace py = pybind11;

namespace PyAE {

// コールバック用構造体（matchNameを保持）
struct PanelCallbackData {
    std::string matchName;
    PySidePanelHandler* handler;
};

// コールバックデータを保持（パネルごとに1つ）
static std::vector<std::unique_ptr<PanelCallbackData>> s_callbackDataList;

bool PySidePanelHandler::Initialize() {
    if (m_initialized) {
        PYAE_LOG_WARNING("PySidePanelHandler", "Already initialized");
        return true;
    }

    PYAE_LOG_INFO("PySidePanelHandler", "Initializing PySidePanelHandler...");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // PanelSuiteが利用可能か確認
    if (suites.panelSuite == nullptr) {
        PYAE_LOG_WARNING("PySidePanelHandler", "PanelSuite not available - PySide panel will be disabled");
        m_panelSupported = false;
        m_initialized = true;
        return true;
    }

    m_panelSupported = true;
    m_initialized = true;

    PYAE_LOG_INFO("PySidePanelHandler", "PySidePanelHandler initialized (panel supported: " +
                  std::string(m_panelSupported ? "yes" : "no") + ")");

    return true;
}

void PySidePanelHandler::Shutdown() {
    if (!m_initialized) {
        return;
    }

    PYAE_LOG_INFO("PySidePanelHandler", "Shutting down PySidePanelHandler...");

    // 登録解除
    UnregisterPanel();

    // クリーンアップ
    m_panelDefinitions.clear();
    m_matchNameToIndex.clear();
    m_panelInstances.clear();
    s_callbackDataList.clear();

    m_initialized = false;
    m_configLoaded = false;

    PYAE_LOG_INFO("PySidePanelHandler", "PySidePanelHandler shutdown complete");
}

bool PySidePanelHandler::LoadPanelConfig() {
    if (m_configLoaded) {
        return true;
    }

    PYAE_LOG_INFO("PySidePanelHandler", "Loading panel configuration...");

    // Pythonから設定を読み込む
    try {
        py::gil_scoped_acquire gil;

        // panels_config モジュールをインポート
        py::module_ config = py::module_::import("panels_config");

        // バリデーション
        py::object validate = config.attr("validate_panels");
        py::tuple result = validate();
        bool valid = result[0].cast<bool>();
        if (!valid) {
            std::string errorMsg = result[1].cast<std::string>();
            PYAE_LOG_ERROR("PySidePanelHandler", "Panel config validation failed: " + errorMsg);
            return false;
        }

        // パネル定義を取得
        py::object getPanels = config.attr("get_panels");
        py::list panels = getPanels();

        m_panelDefinitions.clear();
        m_matchNameToIndex.clear();

        for (size_t i = 0; i < panels.size(); ++i) {
            py::dict panelDict = panels[i].cast<py::dict>();

            PanelDefinition def;
            def.name = panelDict["name"].cast<std::string>();
            def.matchName = panelDict["match_name"].cast<std::string>();
            def.factoryPath = panelDict["factory"].cast<std::string>();
            def.toggleCommand = 0;
            def.registered = false;

            m_matchNameToIndex[def.matchName] = m_panelDefinitions.size();
            m_panelDefinitions.push_back(def);

            PYAE_LOG_INFO("PySidePanelHandler", "Loaded panel definition: " + def.name +
                          " (" + def.matchName + ")");
        }

        PYAE_LOG_INFO("PySidePanelHandler", "Loaded " + std::to_string(m_panelDefinitions.size()) +
                      " panel definitions");

    } catch (const py::error_already_set& e) {
        PYAE_LOG_WARNING("PySidePanelHandler", std::string("Failed to load panels_config: ") + e.what());
        // 設定ファイルがなくてもエラーにはしない
    }

    // 設定が空の場合、デフォルトパネルを追加
    if (m_panelDefinitions.empty()) {
        PYAE_LOG_INFO("PySidePanelHandler", "No panel definitions found, adding default panel");

        PanelDefinition def;
        def.name = DEFAULT_PANEL_DISPLAY_NAME;
        def.matchName = DEFAULT_PANEL_MATCH_NAME;
        def.factoryPath = "";  // ファクトリーなし（手動埋め込み用）
        def.toggleCommand = 0;
        def.registered = false;

        m_matchNameToIndex[def.matchName] = 0;
        m_panelDefinitions.push_back(def);
    }

    m_configLoaded = true;
    return true;
}

bool PySidePanelHandler::RegisterPanel() {
    if (!m_initialized || !m_panelSupported) {
        PYAE_LOG_ERROR("PySidePanelHandler", "Cannot register panel: not initialized or not supported");
        return false;
    }

    // 設定を読み込み
    if (!LoadPanelConfig()) {
        PYAE_LOG_ERROR("PySidePanelHandler", "Failed to load panel config");
        return false;
    }

    // 各パネルを登録
    bool anyRegistered = false;
    for (auto& panel : m_panelDefinitions) {
        if (RegisterSinglePanel(panel)) {
            anyRegistered = true;
        }
    }

    return anyRegistered;
}

bool PySidePanelHandler::RegisterSinglePanel(PanelDefinition& panel) {
    if (panel.registered) {
        PYAE_LOG_WARNING("PySidePanelHandler", "Panel already registered: " + panel.matchName);
        return true;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.panelSuite || !suites.commandSuite || !suites.registerSuite) {
        PYAE_LOG_ERROR("PySidePanelHandler", "Required suites not available");
        return false;
    }

    A_Err err = A_Err_NONE;

    try {
        // パネル表示切り替えコマンドを登録
        err = suites.commandSuite->AEGP_GetUniqueCommand(&panel.toggleCommand);
        if (err != A_Err_NONE) {
            PYAE_LOG_ERROR("PySidePanelHandler", "Failed to get unique command for panel: " + panel.name);
            return false;
        }

        // メニューにコマンドを追加
        err = suites.commandSuite->AEGP_InsertMenuCommand(
            panel.toggleCommand,
            panel.name.c_str(),
            AEGP_Menu_WINDOW,
            AEGP_MENU_INSERT_SORTED);
        if (err != A_Err_NONE) {
            PYAE_LOG_ERROR("PySidePanelHandler", "Failed to insert menu command for panel: " + panel.name);
            return false;
        }

        // コールバックデータを作成
        auto callbackData = std::make_unique<PanelCallbackData>();
        callbackData->matchName = panel.matchName;
        callbackData->handler = this;
        PanelCallbackData* rawPtr = callbackData.get();
        s_callbackDataList.push_back(std::move(callbackData));

        // パネル作成フックを登録
        err = suites.panelSuite->AEGP_RegisterCreatePanelHook(
            state.GetPluginID(),
            reinterpret_cast<const A_u_char*>(panel.matchName.c_str()),
            &PySidePanelHandler::S_CreatePanelHook,
            reinterpret_cast<AEGP_CreatePanelRefcon>(rawPtr),
            TRUE  // AEに背景描画を任せる
        );
        if (err != A_Err_NONE) {
            PYAE_LOG_ERROR("PySidePanelHandler", "Failed to register panel create hook: " + panel.matchName);
            return false;
        }

        panel.registered = true;
        PYAE_LOG_INFO("PySidePanelHandler", "Panel registered: " + panel.name + " (" + panel.matchName + ")");
        return true;

    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PySidePanelHandler", std::string("Exception during panel registration: ") + e.what());
        return false;
    }
}

void PySidePanelHandler::UnregisterPanel() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    for (auto& panel : m_panelDefinitions) {
        if (panel.registered && suites.panelSuite) {
            suites.panelSuite->AEGP_UnRegisterCreatePanelHook(
                reinterpret_cast<const A_u_char*>(panel.matchName.c_str()));
            panel.registered = false;
            PYAE_LOG_INFO("PySidePanelHandler", "Panel unregistered: " + panel.matchName);
        }
    }
}

void PySidePanelHandler::ToggleVisibility(const std::string& matchName) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (suites.panelSuite) {
        A_Err err = suites.panelSuite->AEGP_ToggleVisibility(
            reinterpret_cast<const A_u_char*>(matchName.c_str()));
        if (err != A_Err_NONE) {
            PYAE_LOG_WARNING("PySidePanelHandler", "Failed to toggle panel visibility: " + matchName);
        }
    }
}

bool PySidePanelHandler::IsShown(const std::string& matchName) const {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (suites.panelSuite) {
        A_Boolean tabShown = FALSE;
        A_Boolean frontmost = FALSE;
        A_Err err = suites.panelSuite->AEGP_IsShown(
            reinterpret_cast<const A_u_char*>(matchName.c_str()),
            &tabShown,
            &frontmost);
        if (err == A_Err_NONE) {
            return tabShown && frontmost;
        }
    }

    return false;
}

// 後方互換性：デフォルトパネルの操作
void PySidePanelHandler::ToggleVisibility() {
    if (!m_panelDefinitions.empty()) {
        ToggleVisibility(m_panelDefinitions[0].matchName);
    }
}

bool PySidePanelHandler::IsShown() const {
    if (!m_panelDefinitions.empty()) {
        return IsShown(m_panelDefinitions[0].matchName);
    }
    return false;
}

AEGP_Command PySidePanelHandler::GetToggleCommand() const {
    if (!m_panelDefinitions.empty()) {
        return m_panelDefinitions[0].toggleCommand;
    }
    return 0;
}

PanelDefinition* PySidePanelHandler::GetPanelByCommand(AEGP_Command command) {
    for (auto& panel : m_panelDefinitions) {
        if (panel.toggleCommand == command) {
            return &panel;
        }
    }
    return nullptr;
}

void PySidePanelHandler::RegisterPanelInstance(const std::string& matchName, AEGP_PanelH panelH, PySidePanelUI_Win* panelUI) {
    PanelInstance instance;
    instance.matchName = matchName;
    instance.panelH = panelH;
    instance.panelUI = panelUI;
    m_panelInstances[matchName] = instance;

    PYAE_LOG_INFO("PySidePanelHandler", "Panel instance registered: " + matchName);
}

void PySidePanelHandler::UnregisterPanelInstance(const std::string& matchName) {
    m_panelInstances.erase(matchName);
    PYAE_LOG_INFO("PySidePanelHandler", "Panel instance unregistered: " + matchName);
}

PanelInstance* PySidePanelHandler::GetPanelInstance(const std::string& matchName) {
    auto it = m_panelInstances.find(matchName);
    if (it != m_panelInstances.end()) {
        return &it->second;
    }
    return nullptr;
}

bool PySidePanelHandler::CreateWidgetForPanel(const std::string& matchName) {
    PYAE_LOG_INFO("PySidePanelHandler", "Creating widget for panel: " + matchName);

    // パネル定義を取得
    auto it = m_matchNameToIndex.find(matchName);
    if (it == m_matchNameToIndex.end()) {
        PYAE_LOG_ERROR("PySidePanelHandler", "Panel definition not found: " + matchName);
        return false;
    }

    const auto& panel = m_panelDefinitions[it->second];

    // ファクトリーパスが空の場合はスキップ（手動埋め込み用）
    if (panel.factoryPath.empty()) {
        PYAE_LOG_INFO("PySidePanelHandler", "No factory defined for panel (manual embedding): " + matchName);
        return true;
    }

    // パネルインスタンスを取得
    auto* instance = GetPanelInstance(matchName);
    if (!instance || !instance->panelUI) {
        PYAE_LOG_ERROR("PySidePanelHandler", "Panel instance not ready: " + matchName);
        return false;
    }

    try {
        py::gil_scoped_acquire gil;

        // ファクトリーパスをパース（"module.path:function_name"）
        size_t colonPos = panel.factoryPath.find(':');
        if (colonPos == std::string::npos) {
            PYAE_LOG_ERROR("PySidePanelHandler", "Invalid factory path format: " + panel.factoryPath);
            return false;
        }

        std::string modulePath = panel.factoryPath.substr(0, colonPos);
        std::string functionName = panel.factoryPath.substr(colonPos + 1);

        PYAE_LOG_INFO("PySidePanelHandler", "Importing module: " + modulePath);

        // モジュールをインポート
        py::module_ factoryModule = py::module_::import(modulePath.c_str());

        // ファクトリー関数を取得
        py::object factoryFunc = factoryModule.attr(functionName.c_str());

        // ウィジェットを生成
        PYAE_LOG_INFO("PySidePanelHandler", "Calling factory function: " + functionName);
        py::object widget = factoryFunc();

        // FramelessWindowHint を設定
        py::module_ qtCore = py::module_::import("PySide6.QtCore");
        py::object Qt = qtCore.attr("Qt");
        py::object FramelessWindowHint = Qt.attr("FramelessWindowHint");
        widget.attr("setWindowFlag")(FramelessWindowHint);

        // ae.panel_ui.embed_widget を使用して埋め込み
        py::module_ ae = py::module_::import("ae");
        py::object panel_ui = ae.attr("panel_ui");
        py::object embed_widget = panel_ui.attr("embed_widget");

        bool result = embed_widget(widget).cast<bool>();

        if (result) {
            PYAE_LOG_INFO("PySidePanelHandler", "Widget embedded successfully for panel: " + matchName);
        } else {
            PYAE_LOG_ERROR("PySidePanelHandler", "Failed to embed widget for panel: " + matchName);
        }

        return result;

    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("PySidePanelHandler", std::string("Python error creating widget: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PySidePanelHandler", std::string("Exception creating widget: ") + e.what());
        return false;
    }
}

A_Err PySidePanelHandler::S_CreatePanelHook(
    AEGP_GlobalRefcon plugin_refconP,
    AEGP_CreatePanelRefcon refconP,
    AEGP_PlatformViewRef container,
    AEGP_PanelH panelH,
    AEGP_PanelFunctions1* outFunctionTable,
    AEGP_PanelRefcon* outRefcon)
{
    try {
        PanelCallbackData* data = reinterpret_cast<PanelCallbackData*>(refconP);
        if (data && data->handler) {
            data->handler->CreatePanelHook(data->matchName, container, panelH, outFunctionTable, outRefcon);
        }
        return A_Err_NONE;
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PySidePanelHandler", std::string("Exception in CreatePanelHook: ") + e.what());
        return A_Err_GENERIC;
    }
}

void PySidePanelHandler::CreatePanelHook(
    const std::string& matchName,
    AEGP_PlatformViewRef container,
    AEGP_PanelH panelH,
    AEGP_PanelFunctions1* outFunctionTable,
    AEGP_PanelRefcon* outRefcon)
{
    PYAE_LOG_INFO("PySidePanelHandler", "Creating panel UI for: " + matchName);

#ifdef _WIN32
    // Windows版パネルUIを作成
    auto& state = PluginState::Instance();
    PySidePanelUI_Win* panelUI = new PySidePanelUI_Win(
        state.GetBasicSuite(),
        panelH,
        container,
        outFunctionTable,
        matchName);  // matchNameを渡す
    *outRefcon = reinterpret_cast<AEGP_PanelRefcon>(panelUI);

    // パネルインスタンスを登録
    RegisterPanelInstance(matchName, panelH, panelUI);

    // ファクトリー関数を使ってウィジェットを生成・埋め込み
    // AEコールバック内でPythonを直接呼び出すとクラッシュするため、IdleHandlerで遅延実行
    // SAFETY: The lambda may execute after the panel is destroyed.
    // CreateWidgetForPanel checks if the panel instance still exists before proceeding.
    std::string matchNameCopy = matchName;  // キャプチャ用にコピー
    IdleHandler::Instance().EnqueueTask([matchNameCopy]() {
        PYAE_LOG_INFO("PySidePanelHandler", "Delayed widget creation for: " + matchNameCopy);
        // CreateWidgetForPanel will check if panel instance is still valid
        PySidePanelHandler::Instance().CreateWidgetForPanel(matchNameCopy);
    });
#else
    // 他のプラットフォームは未サポート
    *outRefcon = nullptr;
#endif

    PYAE_LOG_INFO("PySidePanelHandler", "Panel UI created for: " + matchName);
}

// コマンドハンドラ
A_Err PySidePanelHandler_OnCommand(AEGP_Command command) {
    auto& handler = PySidePanelHandler::Instance();

    // コマンドに対応するパネルを検索
    PanelDefinition* panel = handler.GetPanelByCommand(command);
    if (panel) {
        handler.ToggleVisibility(panel->matchName);
        return A_Err_NONE;
    }

    return A_Err_NONE;
}

A_Err PySidePanelHandler_OnUpdateMenu() {
    auto& handler = PySidePanelHandler::Instance();
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!handler.IsPanelSupported() || !suites.commandSuite) {
        return A_Err_NONE;
    }

    // 各パネルのメニュー状態を更新
    for (const auto& panel : handler.GetPanelDefinitions()) {
        if (panel.toggleCommand != 0) {
            // コマンドを有効化
            suites.commandSuite->AEGP_EnableCommand(panel.toggleCommand);

            // パネルが表示中の場合はチェックマークを付ける
            A_Boolean checked = handler.IsShown(panel.matchName) ? TRUE : FALSE;
            suites.commandSuite->AEGP_CheckMarkMenuCommand(panel.toggleCommand, checked);
        }
    }

    return A_Err_NONE;
}

} // namespace PyAE
