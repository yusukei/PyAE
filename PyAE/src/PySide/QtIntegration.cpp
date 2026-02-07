// QtIntegration.cpp (PySide版)
// PyAE - Python for After Effects
// Qt6/PySide6統合管理の実装（PyAEPySide.dll用）

#include "PySide/QtIntegration.h"
#include "Logger.h"

#include <QApplication>
#include <QWidget>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QCoreApplication>
#include <QEventLoop>

#include <stdexcept>

namespace PyAE {
namespace PySide {

bool QtIntegration::Initialize() {
    if (m_initialized.load()) {
        PYAE_LOG_WARNING("QtIntegration", "Already initialized");
        return true;
    }

    PYAE_LOG_INFO("QtIntegration", "Initializing Qt integration (lazy mode)...");

    // NOTE: QApplicationの作成はここでは行わない。
    // PySide6がPythonスクリプト内で import された時点で、
    // PySide6が提供するQt環境を使用する。
    // そのため、ここではフラグを立てるだけで、
    // 実際のQApplication作成はPySide6に任せる。

    // 初期化フラグのみ設定
    m_initialized.store(true);
    PYAE_LOG_INFO("QtIntegration", "Qt integration ready (waiting for PySide6)");

    return true;
}

void QtIntegration::Shutdown() {
    if (!m_initialized.load()) {
        return;
    }

    PYAE_LOG_INFO("QtIntegration", "Shutting down Qt integration...");

    // 全ウィジェットを削除
    // WARNING: During plugin shutdown, Python may have already cleaned up.
    // Calling deleteLater() on Python-owned QWidget objects can cause crashes
    // because the Python side may have already destroyed the object.
    // For now, we clear the tracking list without calling deleteLater().
    // If widgets need explicit cleanup, this should be done before Python shutdown.
    {
        WinLockGuard lock(m_widgetsMutex);
        // Do NOT call deleteLater() - Python may have already cleaned up these objects
        // for (auto* widget : m_widgets) {
        //     if (widget) {
        //         widget->deleteLater();
        //     }
        // }
        m_widgets.clear();
    }

    // イベント処理（deleteLater対応）- PySide6のQApplicationが存在する場合のみ
    // NOTE: Commented out deleteLater() calls above, so this processEvents is not strictly needed
    // if (QCoreApplication::instance()) {
    //     QCoreApplication::processEvents(QEventLoop::AllEvents);
    // }

    // NOTE: QApplicationはPySide6が管理しているため、ここでは削除しない
    // m_app.reset();  // 不要

    m_initialized.store(false);
    PYAE_LOG_INFO("QtIntegration", "Qt integration shutdown complete");
}

void QtIntegration::ProcessEvents(int maxTimeMs) {
    if (!m_initialized.load()) {
        return;
    }

    // PySide6がQApplicationを作成している場合のみ処理
    if (!QCoreApplication::instance()) {
        return;
    }

    // イベント処理（最大maxTimeMs）
    QCoreApplication::processEvents(
        QEventLoop::AllEvents,
        maxTimeMs
    );
}

void QtIntegration::RegisterWidget(QWidget* widget) {
    if (!widget) {
        PYAE_LOG_ERROR("QtIntegration", "Attempted to register null widget");
        return;
    }

    WinLockGuard lock(m_widgetsMutex);

    // 既に登録されているか確認
    auto it = std::find(m_widgets.begin(), m_widgets.end(), widget);
    if (it != m_widgets.end()) {
        PYAE_LOG_WARNING("QtIntegration", "Widget already registered");
        return;
    }

    m_widgets.push_back(widget);

    PYAE_LOG_DEBUG("QtIntegration", "Widget registered (total: " + std::to_string(m_widgets.size()) + ")");
}

void QtIntegration::UnregisterWidget(QWidget* widget) {
    if (!widget) {
        return;
    }

    WinLockGuard lock(m_widgetsMutex);
    auto it = std::find(m_widgets.begin(), m_widgets.end(), widget);
    if (it != m_widgets.end()) {
        m_widgets.erase(it);
        PYAE_LOG_DEBUG("QtIntegration", "Widget unregistered (remaining: " + std::to_string(m_widgets.size()) + ")");
    }
}

bool QtIntegration::HasVisibleWidgets() const {
    WinLockGuard lock(m_widgetsMutex);

    for (auto* widget : m_widgets) {
        if (widget && widget->isVisible()) {
            return true;
        }
    }

    return false;
}

size_t QtIntegration::GetWidgetCount() const {
    WinLockGuard lock(m_widgetsMutex);
    return m_widgets.size();
}

void QtIntegration::ApplyDarkTheme() {
    // PySide6が作成したQApplicationが存在する場合のみ適用
    auto* app = qobject_cast<QApplication*>(QCoreApplication::instance());
    if (!app) {
        PYAE_LOG_ERROR("QtIntegration", "Cannot apply theme: QApplication not available");
        return;
    }

    // Fusionスタイル適用（クロスプラットフォームで一貫した外観）
    app->setStyle(QStyleFactory::create("Fusion"));

    // After Effectsのダークテーマに合わせたパレット
    QPalette darkPalette;

    // 背景色
    darkPalette.setColor(QPalette::Window, QColor(46, 46, 46));
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::AlternateBase, QColor(46, 46, 46));

    // テキスト色
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);

    // ボタン
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);

    // ツールチップ
    darkPalette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);

    // リンクとハイライト
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    // 無効状態
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(128, 128, 128));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(128, 128, 128));

    app->setPalette(darkPalette);

    // スタイルシートで細かい調整
    app->setStyleSheet(R"(
        QToolTip {
            color: white;
            background-color: #353535;
            border: 1px solid #2e2e2e;
            padding: 2px;
        }
        QScrollBar:vertical {
            background: #2e2e2e;
            width: 12px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #555555;
            min-height: 20px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background: #666666;
        }
        QScrollBar:horizontal {
            background: #2e2e2e;
            height: 12px;
            margin: 0px;
        }
        QScrollBar::handle:horizontal {
            background: #555555;
            min-width: 20px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #666666;
        }
    )");

    PYAE_LOG_INFO("QtIntegration", "Dark theme applied successfully");
}

} // namespace PySide
} // namespace PyAE
