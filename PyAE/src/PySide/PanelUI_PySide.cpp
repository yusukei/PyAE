// PanelUI_PySide.cpp
// PyAE - Python for After Effects
// AE HWNDとPySide6 QWidgetの橋渡し実装

#include "PySide/PanelUI_PySide.h"
#include "PySide/QtIntegration.h"
#include "Logger.h"

#include <QWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QApplication>

#include <stdexcept>

namespace PyAE {
namespace PySide {

PanelUI_PySide::PanelUI_PySide(HWND hwnd, const std::string& panelName)
    : m_hwnd(hwnd)
    , m_panelName(panelName)
{
    PYAE_LOG_INFO("PanelUI_PySide", "Creating panel UI for '" + panelName + "'...");

    if (!hwnd) {
        throw std::runtime_error("PanelUI_PySide: Invalid HWND (null)");
    }

    // QApplicationが初期化されていることを確認
    if (!QApplication::instance()) {
        throw std::runtime_error("PanelUI_PySide: QApplication not initialized. Call QtIntegration::Initialize() first.");
    }

    try {
        // 1. HWNDをWIdに変換してQWindowを作成
        WId winId = reinterpret_cast<WId>(hwnd);
        m_qwindow = QWindow::fromWinId(winId);

        if (!m_qwindow) {
            throw std::runtime_error("QWindow::fromWinId() failed - platform may not support foreign windows");
        }

        PYAE_LOG_DEBUG("PanelUI_PySide", "QWindow created from HWND");

        // 2. QWindowをQWidgetコンテナに変換
        m_container = QWidget::createWindowContainer(m_qwindow);

        if (!m_container) {
            throw std::runtime_error("QWidget::createWindowContainer() failed");
        }

        PYAE_LOG_DEBUG("PanelUI_PySide", "Window container created");

        // 3. コンテナの設定
        m_container->setObjectName(QString::fromStdString("PyAE_Panel_" + panelName));
        m_container->setAttribute(Qt::WA_DeleteOnClose, false);
        m_container->setFocusPolicy(Qt::StrongFocus);

        // 4. レイアウト作成
        m_layout = new QVBoxLayout(m_container);
        m_layout->setContentsMargins(0, 0, 0, 0);
        m_layout->setSpacing(0);

        // 5. QtIntegrationに登録
        QtIntegration::Instance().RegisterWidget(m_container);

        m_initialized = true;

        PYAE_LOG_INFO("PanelUI_PySide", "Panel UI '" + panelName + "' initialized successfully");

    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PanelUI_PySide", "Initialization failed: " + std::string(e.what()));

        // クリーンアップ
        if (m_container) {
            delete m_container;
            m_container = nullptr;
        }
        // m_qwindowはfromWinId()で作成されたので、コンテナと一緒に削除される

        throw;
    }
}

PanelUI_PySide::~PanelUI_PySide() {
    PYAE_LOG_INFO("PanelUI_PySide", "Destroying panel UI '" + m_panelName + "'...");

    // QtIntegrationから登録解除
    if (m_container) {
        QtIntegration::Instance().UnregisterWidget(m_container);
    }

    // PySideウィジェットは外部で管理されているので削除しない
    // （Pythonガベージコレクターが管理）
    m_pysideWidget = nullptr;

    // コンテナを削除（レイアウトも一緒に削除される）
    if (m_container) {
        m_container->deleteLater();
        m_container = nullptr;
    }

    // QWindowはfromWinId()で作成されたので、明示的に削除しない
    // （親HWNDが所有している）
    m_qwindow = nullptr;

    m_layout = nullptr;
    m_initialized = false;

    PYAE_LOG_INFO("PanelUI_PySide", "Panel UI '" + m_panelName + "' destroyed");
}

bool PanelUI_PySide::SetPySideWidget(QWidget* widget) {
    if (!m_initialized) {
        PYAE_LOG_ERROR("PanelUI_PySide", "Cannot set widget - not initialized");
        return false;
    }

    if (!widget) {
        PYAE_LOG_ERROR("PanelUI_PySide", "Cannot set null widget");
        return false;
    }

    if (!m_layout) {
        PYAE_LOG_ERROR("PanelUI_PySide", "Layout not available");
        return false;
    }

    PYAE_LOG_DEBUG("PanelUI_PySide", "Setting PySide widget for '" + m_panelName + "'...");

    // 既存のウィジェットがあれば削除
    if (m_pysideWidget) {
        m_layout->removeWidget(m_pysideWidget);
        // 注: PySideウィジェットはPythonが所有しているので、deleteLaterしない
        m_pysideWidget = nullptr;
    }

    // 新しいウィジェットを追加
    m_pysideWidget = widget;
    m_layout->addWidget(m_pysideWidget);

    // ウィジェットをストレッチして全体に広げる
    m_pysideWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 表示
    m_pysideWidget->show();

    PYAE_LOG_INFO("PanelUI_PySide", "PySide widget set successfully");
    return true;
}

void PanelUI_PySide::OnResize(int width, int height) {
    if (!m_initialized || !m_container) {
        return;
    }

    // コンテナのサイズを更新
    m_container->resize(width, height);

    // PySideウィジェットも更新
    if (m_pysideWidget) {
        m_pysideWidget->resize(width, height);
    }
}

void PanelUI_PySide::Show() {
    if (m_container) {
        m_container->show();
    }
    if (m_pysideWidget) {
        m_pysideWidget->show();
    }
}

void PanelUI_PySide::Hide() {
    if (m_pysideWidget) {
        m_pysideWidget->hide();
    }
    if (m_container) {
        m_container->hide();
    }
}

} // namespace PySide
} // namespace PyAE
