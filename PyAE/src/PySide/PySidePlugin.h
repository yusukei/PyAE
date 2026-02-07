// PySidePlugin.h
// PyAE - Python for After Effects
// PySide6プラグイン実装クラス（PyAEPySide.dll内部用）

#pragma once

#include "PySide/IPySidePlugin.h"
#include <map>
#include <memory>
#include <string>
#include "WinSync.h"

// 前方宣言
class QWidget;

namespace PyAE {
namespace PySide {

// 前方宣言
class PanelUI_PySide;

/**
 * @brief PySide6プラグイン実装クラス
 *
 * IPySidePluginインターフェースの具体的な実装。
 * PyAEPySide.dll内でのみ使用される。
 */
class PySidePlugin : public IPySidePlugin {
public:
    PySidePlugin() = default;
    virtual ~PySidePlugin() override;

    // IPySidePluginインターフェース実装
    virtual bool Initialize() override;
    virtual void Shutdown() override;
    virtual bool IsInitialized() const override;

    virtual bool RegisterPySidePanel(
        const std::string& panelName,
        void* qwidgetPtr) override;

    virtual void UnregisterPySidePanel(const std::string& panelName) override;

    virtual bool IsPanelRegistered(const std::string& panelName) const override;

    // イベント処理（Core.cppのIdleHandlerから呼ばれる想定）
    virtual void ProcessEvents(int maxTimeMs = 10) override;

private:
    // コピー禁止
    PySidePlugin(const PySidePlugin&) = delete;
    PySidePlugin& operator=(const PySidePlugin&) = delete;

    // メンバー変数
    bool m_initialized = false;

    // パネル管理
    std::map<std::string, std::unique_ptr<PanelUI_PySide>> m_panels;
    mutable WinMutex m_panelsMutex;
};

} // namespace PySide
} // namespace PyAE
