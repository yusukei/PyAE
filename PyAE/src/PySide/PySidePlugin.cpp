// PySidePlugin.cpp
// PyAE - Python for After Effects
// PySide6プラグイン実装

#include "PySidePlugin.h"
#include "PySide/QtIntegration.h"
#include "PySide/PanelUI_PySide.h"
#include "Logger.h"
#include <QWidget>

namespace PyAE {
namespace PySide {

PySidePlugin::~PySidePlugin() {
    Shutdown();
}

bool PySidePlugin::Initialize() {
    if (m_initialized) {
        PYAE_LOG_WARNING("PySidePlugin", "Already initialized");
        return true;
    }

    PYAE_LOG_INFO("PySidePlugin", "Initializing...");

    // QtIntegration初期化
    if (!QtIntegration::Instance().Initialize()) {
        PYAE_LOG_ERROR("PySidePlugin", "Failed to initialize QtIntegration");
        return false;
    }

    m_initialized = true;
    PYAE_LOG_INFO("PySidePlugin", "Initialized successfully");

    return true;
}

void PySidePlugin::Shutdown() {
    if (!m_initialized) {
        return;
    }

    PYAE_LOG_INFO("PySidePlugin", "Shutting down...");

    // 全パネルを削除
    {
        WinLockGuard lock(m_panelsMutex);
        m_panels.clear();
    }

    // QtIntegration終了
    QtIntegration::Instance().Shutdown();

    m_initialized = false;
    PYAE_LOG_INFO("PySidePlugin", "Shutdown complete");
}

bool PySidePlugin::IsInitialized() const {
    return m_initialized;
}

bool PySidePlugin::RegisterPySidePanel(
    const std::string& panelName,
    void* qwidgetPtr)
{
    if (!m_initialized) {
        PYAE_LOG_ERROR("PySidePlugin", "Not initialized");
        return false;
    }

    if (!qwidgetPtr) {
        PYAE_LOG_ERROR("PySidePlugin", "Null QWidget pointer");
        return false;
    }

    WinLockGuard lock(m_panelsMutex);

    // 既に登録されているか確認
    if (m_panels.find(panelName) != m_panels.end()) {
        PYAE_LOG_ERROR("PySidePlugin", "Panel already registered: " + panelName);
        return false;
    }

    try {
        // QWidgetポインタをキャスト
        QWidget* widget = static_cast<QWidget*>(qwidgetPtr);

        // ウィジェット登録
        QtIntegration::Instance().RegisterWidget(widget);

        // TODO: Create PanelUI_PySide from widget and add to m_panels for tracking
        // m_panels requires unique_ptr<PanelUI_PySide>, needs HWND from PanelHandler

        // 注: 実際のAEパネル統合にはHWNDが必要
        // 現在は仮実装として、ウィジェット登録のみ
        // TODO: PanelHandlerからHWNDを取得してPanelUI_PySideを作成

        PYAE_LOG_INFO("PySidePlugin", "Panel registered: " + panelName);

        return true;

    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PySidePlugin", "Exception while registering panel: " + std::string(e.what()));
        return false;
    }
}

void PySidePlugin::UnregisterPySidePanel(const std::string& panelName) {
    if (!m_initialized) {
        return;
    }

    WinLockGuard lock(m_panelsMutex);

    auto it = m_panels.find(panelName);
    if (it != m_panels.end()) {
        m_panels.erase(it);
        PYAE_LOG_INFO("PySidePlugin", "Panel unregistered: " + panelName);
    }
}

bool PySidePlugin::IsPanelRegistered(const std::string& panelName) const {
    WinLockGuard lock(m_panelsMutex);
    return m_panels.find(panelName) != m_panels.end();
}

void PySidePlugin::ProcessEvents(int maxTimeMs) {
    if (m_initialized) {
        QtIntegration::Instance().ProcessEvents(maxTimeMs);
    }
}

} // namespace PySide
} // namespace PyAE

// ===============================================
// DLLエクスポート関数
// ===============================================

extern "C" {

__declspec(dllexport) PyAE::IPySidePlugin* CreatePySidePlugin() {
    try {
        return new PyAE::PySide::PySidePlugin();
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PySidePlugin", "CreatePySidePlugin: Exception: " + std::string(e.what()));
        return nullptr;
    }
}

__declspec(dllexport) void DestroyPySidePlugin(PyAE::IPySidePlugin* plugin) {
    if (plugin) {
        delete plugin;
    }
}

} // extern "C"
