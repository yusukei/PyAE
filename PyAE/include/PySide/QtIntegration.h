// QtIntegration.h (PySide版)
// PyAE - Python for After Effects
// Qt6/PySide6統合管理（PyAEPySide.dll用）

#pragma once

#include <atomic>
#include <memory>
#include <vector>
#include "WinSync.h"

// Qt includes (必要最小限)
#include <QApplication>
class QWidget;

namespace PyAE {
namespace PySide {

/**
 * @brief Qt統合管理クラス（PySide専用）
 *
 * QApplicationのライフサイクル管理とイベント処理統合を行う。
 * PyAEPySide.dll内でのみ使用される。
 */
class QtIntegration {
public:
    /**
     * @brief シングルトンインスタンス取得
     */
    static QtIntegration& Instance() {
        static QtIntegration instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize();
    void Shutdown();
    bool IsInitialized() const { return m_initialized.load(); }

    // イベント処理
    void ProcessEvents(int maxTimeMs = 10);

    // ウィジェット管理
    void RegisterWidget(QWidget* widget);
    void UnregisterWidget(QWidget* widget);
    bool HasVisibleWidgets() const;
    size_t GetWidgetCount() const;

    // テーマ設定
    void ApplyDarkTheme();

private:
    QtIntegration() = default;
    ~QtIntegration() = default;

    // コピー禁止
    QtIntegration(const QtIntegration&) = delete;
    QtIntegration& operator=(const QtIntegration&) = delete;

    // メンバー変数
    std::unique_ptr<QApplication> m_app;
    std::vector<QWidget*> m_widgets;
    mutable WinMutex m_widgetsMutex;
    std::atomic<bool> m_initialized{false};
};

} // namespace PySide
} // namespace PyAE
