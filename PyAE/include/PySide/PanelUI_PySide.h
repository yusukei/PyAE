// PanelUI_PySide.h
// PyAE - Python for After Effects
// AE HWNDとPySide6 QWidgetの橋渡し

#pragma once

#include <string>
#include <memory>

#ifdef _WIN32
#include <Windows.h>
#endif

// Forward declarations
class QWindow;
class QWidget;
class QVBoxLayout;

namespace PyAE {
namespace PySide {

/**
 * @brief AEパネルHWNDとPySide6 QWidgetの橋渡しクラス
 *
 * QWindow::fromWinId()とQWidget::createWindowContainer()を使用して
 * After EffectsのパネルHWND内にPySide6ウィジェットを表示する。
 */
class PanelUI_PySide {
public:
    /**
     * @brief コンストラクタ
     * @param hwnd After Effectsパネルのウィンドウハンドル
     * @param panelName パネル名
     */
    explicit PanelUI_PySide(HWND hwnd, const std::string& panelName);

    /**
     * @brief デストラクタ
     */
    ~PanelUI_PySide();

    // コピー禁止
    PanelUI_PySide(const PanelUI_PySide&) = delete;
    PanelUI_PySide& operator=(const PanelUI_PySide&) = delete;

    /**
     * @brief PySideウィジェットを設定
     * @param widget PySide6 QWidgetインスタンス
     * @return 成功した場合true
     */
    bool SetPySideWidget(QWidget* widget);

    /**
     * @brief 現在のPySideウィジェットを取得
     * @return QWidgetポインタ（nullptr可）
     */
    QWidget* GetPySideWidget() const { return m_pysideWidget; }

    /**
     * @brief パネル名を取得
     */
    const std::string& GetPanelName() const { return m_panelName; }

    /**
     * @brief HWNDを取得
     */
    HWND GetHWND() const { return m_hwnd; }

    /**
     * @brief コンテナウィジェットを取得
     */
    QWidget* GetContainer() const { return m_container; }

    /**
     * @brief 初期化が完了しているか
     */
    bool IsInitialized() const { return m_initialized; }

    /**
     * @brief パネルサイズを更新
     * @param width 幅
     * @param height 高さ
     */
    void OnResize(int width, int height);

    /**
     * @brief パネルを表示
     */
    void Show();

    /**
     * @brief パネルを非表示
     */
    void Hide();

private:
    // メンバー変数
    HWND m_hwnd = nullptr;
    std::string m_panelName;
    bool m_initialized = false;

    // Qtオブジェクト
    QWindow* m_qwindow = nullptr;
    QWidget* m_container = nullptr;
    QVBoxLayout* m_layout = nullptr;
    QWidget* m_pysideWidget = nullptr;
};

} // namespace PySide
} // namespace PyAE
