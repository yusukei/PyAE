// PySideLoader.h
// PyAE - Python for After Effects
// PySide6プラグイン動的ローダー

#pragma once

#include "PySide/IPySidePlugin.h"
#include <string>
#include <memory>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace PyAE {

/**
 * @brief PySide6プラグイン動的ローダー
 *
 * PyAEPySide.dllを動的にロード・アンロードする。
 */
class PySideLoader {
public:
    /**
     * @brief シングルトンインスタンス取得
     */
    static PySideLoader& Instance() {
        static PySideLoader instance;
        return instance;
    }

    /**
     * @brief PyAEPySide.dllをロード
     * @return 成功した場合true
     */
    bool LoadPlugin();

    /**
     * @brief PyAEPySide.dllをアンロード
     */
    void UnloadPlugin();

    /**
     * @brief プラグインがロードされているか確認
     * @return ロードされている場合true
     */
    bool IsLoaded() const { return m_plugin != nullptr; }

    /**
     * @brief プラグインインスタンスを取得
     * @return IPySidePluginインスタンス（nullptrの場合あり）
     */
    IPySidePlugin* GetPlugin() const { return m_plugin; }

private:
    PySideLoader() = default;
    ~PySideLoader();

    // コピー禁止
    PySideLoader(const PySideLoader&) = delete;
    PySideLoader& operator=(const PySideLoader&) = delete;

    // メンバー変数
#ifdef _WIN32
    HMODULE m_dllHandle = nullptr;
#endif
    IPySidePlugin* m_plugin = nullptr;
    DestroyPySidePluginFunc m_destroyFunc = nullptr;
};

} // namespace PyAE
