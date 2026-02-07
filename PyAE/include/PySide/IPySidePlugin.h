// IPySidePlugin.h
// PyAE - Python for After Effects
// PySide6プラグインインターフェース

#pragma once

#include <string>

namespace PyAE {

/**
 * @brief PySide6プラグインインターフェース
 *
 * PyAEPySide.dllが実装するインターフェース。
 * PyAECore.dllから動的にロードされる。
 */
class IPySidePlugin {
public:
    virtual ~IPySidePlugin() = default;

    /**
     * @brief プラグインの初期化
     * @return 成功した場合true
     */
    virtual bool Initialize() = 0;

    /**
     * @brief プラグインの終了
     */
    virtual void Shutdown() = 0;

    /**
     * @brief 初期化状態を確認
     * @return 初期化されている場合true
     */
    virtual bool IsInitialized() const = 0;

    /**
     * @brief Qtイベントループを処理
     * @param maxTimeMs 最大処理時間（ミリ秒）
     */
    virtual void ProcessEvents(int maxTimeMs = 10) = 0;

    /**
     * @brief PySide6パネルを登録
     * @param panelName パネル名（一意）
     * @param qwidgetPtr PySide6 QWidgetへのポインタ（void*）
     * @return 成功した場合true
     */
    virtual bool RegisterPySidePanel(
        const std::string& panelName,
        void* qwidgetPtr) = 0;

    /**
     * @brief PySide6パネルの登録を解除
     * @param panelName パネル名
     */
    virtual void UnregisterPySidePanel(const std::string& panelName) = 0;

    /**
     * @brief パネルが登録されているか確認
     * @param panelName パネル名
     * @return 登録されている場合true
     */
    virtual bool IsPanelRegistered(const std::string& panelName) const = 0;
};

/**
 * @brief プラグイン作成関数の型定義
 *
 * PyAEPySide.dllからエクスポートされる関数。
 * 関数名: "CreatePySidePlugin"
 */
using CreatePySidePluginFunc = IPySidePlugin* (*)();

/**
 * @brief プラグイン破棄関数の型定義
 *
 * PyAEPySide.dllからエクスポートされる関数。
 * 関数名: "DestroyPySidePlugin"
 */
using DestroyPySidePluginFunc = void (*)(IPySidePlugin*);

} // namespace PyAE

/**
 * @brief C言語エクスポート関数（DLL境界用）
 *
 * これらの関数はPyAEPySide.dllによって実装される必要があります。
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief プラグインインスタンスを作成
 * @return IPySidePluginインスタンスへのポインタ
 */
__declspec(dllexport) PyAE::IPySidePlugin* CreatePySidePlugin();

/**
 * @brief プラグインインスタンスを破棄
 * @param plugin 破棄するプラグインインスタンス
 */
__declspec(dllexport) void DestroyPySidePlugin(PyAE::IPySidePlugin* plugin);

#ifdef __cplusplus
}
#endif
