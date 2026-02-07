// PluginState.cpp
// PyAE - Python for After Effects
// プラグイン状態管理の実装

#include "PluginState.h"
#include "ErrorHandling.h"
#include "IdleHandler.h"

#include <cstdlib>

namespace PyAE {

bool PluginState::Initialize(SPBasicSuite* basicSuite, AEGP_PluginID pluginId) {
    if (m_initialized.load()) {
        PYAE_LOG_WARNING("PluginState", "Already initialized");
        return true;
    }

    m_basicSuite = basicSuite;
    m_pluginId = pluginId;

    // PathManager: プラグインディレクトリ検出
    if (!m_pathManager.DetectPluginDirectory()) {
        PYAE_LOG_ERROR("PluginState", "Failed to detect plugin directory");
        return false;
    }

    // SuiteManager: 作成とスイート取得
    try {
        m_suiteManager = std::make_unique<SuiteManager>(basicSuite);
        if (!m_suiteManager->AcquireSuites()) {
            PYAE_LOG_ERROR("PluginState", "Failed to acquire suites");
            return false;
        }
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PluginState", std::string("Failed to initialize SuiteManager: ") + e.what());
        return false;
    }

    m_initialized.store(true);
    PYAE_LOG_INFO("PluginState", "Initialized successfully");

    return true;
}

void PluginState::Shutdown() {
    if (!m_initialized.load()) {
        return;
    }

    PYAE_LOG_INFO("PluginState", "Shutting down...");

    m_shuttingDown.store(true);

    // SuiteManager: スイート解放とクリーンアップ
    if (m_suiteManager) {
        m_suiteManager->ReleaseSuites();
        m_suiteManager.reset();
    }

    m_initialized.store(false);
}

void PluginState::CheckAndQueueAutoTest() {
    // 環境変数をチェック
    const char* autoTest = std::getenv("PYAE_AUTO_TEST");

    // Debug: Log environment variable check
    PYAE_LOG_INFO("PluginState", "Checking PYAE_AUTO_TEST: " +
        std::string(autoTest ? autoTest : "(null)"));

    if (!autoTest || std::string(autoTest) != "1") {
        // 自動テストが有効になっていない
        return;
    }

    PYAE_LOG_INFO("PluginState", "Auto test mode detected");

    // 必須パラメータを取得
    const char* testModule = std::getenv("PYAE_TEST_MODULE");
    const char* testFunction = std::getenv("PYAE_TEST_FUNCTION");

    if (!testModule || !testFunction) {
        PYAE_LOG_ERROR("PluginState", "PYAE_TEST_MODULE and PYAE_TEST_FUNCTION must be set");
        return;
    }

    // オプションパラメータを取得
    const char* scriptPath = std::getenv("PYAE_TEST_SCRIPT");
    const char* outputPath = std::getenv("PYAE_TEST_OUTPUT");
    const char* autoExitStr = std::getenv("PYAE_TEST_EXIT");
    const char* timeoutStr = std::getenv("PYAE_TEST_TIMEOUT");
    const char* delayStr = std::getenv("PYAE_TEST_DELAY");

    // デフォルト値
    std::string output = outputPath ? outputPath : "test_result.json";
    bool autoExit = autoExitStr ? (std::string(autoExitStr) == "1") : true;
    int timeout = timeoutStr ? std::atoi(timeoutStr) : 300;  // デフォルト 5分
    int delay = delayStr ? std::atoi(delayStr) : 2;  // デフォルト 2秒

    PYAE_LOG_INFO("PluginState", "Queueing auto test:");
    PYAE_LOG_INFO("PluginState", "  Module: " + std::string(testModule));
    PYAE_LOG_INFO("PluginState", "  Function: " + std::string(testFunction));
    if (scriptPath) {
        PYAE_LOG_INFO("PluginState", "  Script: " + std::string(scriptPath));
    }
    PYAE_LOG_INFO("PluginState", "  Output: " + output);
    PYAE_LOG_INFO("PluginState", "  Auto Exit: " + std::string(autoExit ? "Yes" : "No"));
    PYAE_LOG_INFO("PluginState", "  Timeout: " + std::to_string(timeout) + "s");
    PYAE_LOG_INFO("PluginState", "  Delay: " + std::to_string(delay) + "s");

    // IdleHandler にテストをキュー
    IdleHandler::Instance().QueueAutoTest(
        scriptPath ? scriptPath : "",
        testModule,
        testFunction,
        output,
        autoExit,
        timeout,
        delay
    );
}

} // namespace PyAE
