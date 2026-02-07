// PathManager.cpp
// PyAE - Python for After Effects
// パス管理クラスの実装

#include "PathManager.h"
#include "Logger.h"

#ifdef _WIN32
#include <Windows.h>
#include "AE_GeneralPlug.h"
#endif

// 前方宣言 (PyAECore.dllのエントリーポイント)
extern "C" A_Err PyAECore_Init(SPBasicSuite*, A_long, A_long, AEGP_PluginID, AEGP_GlobalRefcon*);

namespace PyAE {

bool PathManager::DetectPluginDirectory() {
#ifdef _WIN32
    wchar_t modulePath[MAX_PATH];
    HMODULE hModule = nullptr;

    // 自分自身のDLLのモジュールハンドルを取得
    if (GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&PyAECore_Init),
            &hModule)) {

        if (GetModuleFileNameW(hModule, modulePath, MAX_PATH) > 0) {
            m_pluginDir = std::filesystem::path(modulePath).parent_path();
            m_scriptsDir = m_pluginDir / "scripts";

            // scriptsディレクトリがなければ作成
            std::error_code ec;
            std::filesystem::create_directories(m_scriptsDir, ec);

            PYAE_LOG_INFO("PathManager", "Plugin directory: " + m_pluginDir.string());
            PYAE_LOG_INFO("PathManager", "Scripts directory: " + m_scriptsDir.string());

            return true;
        }
    }
#endif

    // フォールバック
    m_pluginDir = std::filesystem::current_path();
    m_scriptsDir = m_pluginDir / "scripts";

    PYAE_LOG_WARNING("PathManager", "Using fallback plugin directory: " + m_pluginDir.string());
    return true;
}

} // namespace PyAE
