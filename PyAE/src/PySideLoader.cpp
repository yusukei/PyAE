// PySideLoader.cpp
// PyAE - Python for After Effects
// PySide6プラグイン動的ローダーの実装

#include "PySideLoader.h"
#include "Logger.h"
#include <filesystem>

namespace PyAE {

namespace fs = std::filesystem;

// PySide6のQt6 DLLディレクトリを検索パスに追加する
// Windows 8以降で利用可能なAddDllDirectory APIを使用
static std::vector<DLL_DIRECTORY_COOKIE> s_dllDirectoryCookies;

// 現在のDLLのディレクトリを取得
static fs::path GetCurrentDllDirectory() {
#ifdef _WIN32
    HMODULE hModule = nullptr;
    // 自分のDLL（PyAECore.dll）のハンドルを取得
    if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&GetCurrentDllDirectory),
            &hModule)) {
        char path[MAX_PATH];
        if (GetModuleFileNameA(hModule, path, MAX_PATH)) {
            fs::path dllPath(path);
            return dllPath.parent_path();
        }
    }
#endif
    return fs::path();
}

static void AddPySide6DllDirectories() {
#ifdef _WIN32
    // 現在のDLLディレクトリを取得
    fs::path pluginDir = GetCurrentDllDirectory();
    if (pluginDir.empty()) {
        PYAE_LOG_WARNING("PySideLoader", "Could not detect plugin directory");
        return;
    }

    PYAE_LOG_INFO("PySideLoader", "Plugin directory: " + pluginDir.string());

    // PySide6/shiboken6 DLLディレクトリを検索
    std::vector<fs::path> searchPaths;

    // 方法1: プラグインディレクトリ内のLib/site-packages (インストール時)
    searchPaths.push_back(pluginDir / "Lib" / "site-packages" / "PySide6");
    searchPaths.push_back(pluginDir / "Lib" / "site-packages" / "shiboken6");

    // 方法2: 環境変数PYAE_PYTHON_HOMEから
    const char* pythonHome = std::getenv("PYAE_PYTHON_HOME");
    if (pythonHome) {
        fs::path pythonHomePath(pythonHome);
        searchPaths.push_back(pythonHomePath / "Lib" / "site-packages" / "PySide6");
        searchPaths.push_back(pythonHomePath / "Lib" / "site-packages" / "shiboken6");
    }

    for (const auto& path : searchPaths) {
        if (fs::exists(path) && fs::is_directory(path)) {
            std::wstring wpath = path.wstring();
            DLL_DIRECTORY_COOKIE cookie = AddDllDirectory(wpath.c_str());
            if (cookie) {
                s_dllDirectoryCookies.push_back(cookie);
                PYAE_LOG_INFO("PySideLoader",
                    "Added DLL directory: " + path.string());
            }
        }
    }

    if (s_dllDirectoryCookies.empty()) {
        PYAE_LOG_WARNING("PySideLoader",
            "No PySide6/shiboken6 directories found - Qt6 DLLs may not be available");
    }
#endif
}

static void RemovePySide6DllDirectories() {
#ifdef _WIN32
    for (auto cookie : s_dllDirectoryCookies) {
        RemoveDllDirectory(cookie);
    }
    s_dllDirectoryCookies.clear();
#endif
}

PySideLoader::~PySideLoader() {
    UnloadPlugin();
}

bool PySideLoader::LoadPlugin() {
    if (m_plugin) {
        PYAE_LOG_WARNING("PySideLoader", "Plugin already loaded");
        return true;
    }

    PYAE_LOG_INFO("PySideLoader", "Loading PyAEPySide.dll...");

#ifdef _WIN32
    // プラグインディレクトリを取得
    fs::path pluginDir = GetCurrentDllDirectory();
    if (pluginDir.empty()) {
        PYAE_LOG_ERROR("PySideLoader", "Could not detect plugin directory");
        return false;
    }

    // PyAEPySide.dllのフルパスを構築
    fs::path pysideDllPath = pluginDir / "PyAEPySide.dll";
    PYAE_LOG_INFO("PySideLoader", "PyAEPySide.dll path: " + pysideDllPath.string());

    if (!fs::exists(pysideDllPath)) {
        PYAE_LOG_INFO("PySideLoader",
            "PyAEPySide.dll not found at: " + pysideDllPath.string() +
            " - PySide6 features will be disabled");
        return false;
    }

    // PySide6のDLLディレクトリを検索パスに追加
    AddPySide6DllDirectories();

    // フルパスでロード（LOAD_LIBRARY_SEARCH_DEFAULT_DIRSでAddDllDirectoryが有効になる）
    std::string dllPathStr = pysideDllPath.string();
    m_dllHandle = LoadLibraryExA(dllPathStr.c_str(), NULL,
        LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);

    if (!m_dllHandle) {
        DWORD error1 = GetLastError();
        PYAE_LOG_WARNING("PySideLoader",
            "LoadLibraryExA failed (error: " + std::to_string(error1) + "), trying LoadLibraryA...");

        // フォールバック: 従来の方法でロードを試みる
        m_dllHandle = LoadLibraryA(dllPathStr.c_str());
    }

    if (!m_dllHandle) {
        DWORD error = GetLastError();
        PYAE_LOG_INFO("PySideLoader",
            "PyAEPySide.dll could not be loaded (error: " + std::to_string(error) +
            ") - PySide6 features will be disabled");
        RemovePySide6DllDirectories();
        return false;
    }

    PYAE_LOG_INFO("PySideLoader", "PyAEPySide.dll loaded");

    // 関数ポインタを取得
    auto createFunc = reinterpret_cast<CreatePySidePluginFunc>(
        GetProcAddress(m_dllHandle, "CreatePySidePlugin"));

    m_destroyFunc = reinterpret_cast<DestroyPySidePluginFunc>(
        GetProcAddress(m_dllHandle, "DestroyPySidePlugin"));

    if (!createFunc || !m_destroyFunc) {
        PYAE_LOG_ERROR("PySideLoader",
            "Failed to get exported functions from PyAEPySide.dll");
        FreeLibrary(m_dllHandle);
        m_dllHandle = nullptr;
        return false;
    }

    // プラグインインスタンスを作成
    m_plugin = createFunc();
    if (!m_plugin) {
        PYAE_LOG_ERROR("PySideLoader", "Failed to create plugin instance");
        FreeLibrary(m_dllHandle);
        m_dllHandle = nullptr;
        m_destroyFunc = nullptr;
        return false;
    }

    // プラグイン初期化
    if (!m_plugin->Initialize()) {
        PYAE_LOG_ERROR("PySideLoader", "Failed to initialize plugin");
        m_destroyFunc(m_plugin);
        m_plugin = nullptr;
        FreeLibrary(m_dllHandle);
        m_dllHandle = nullptr;
        m_destroyFunc = nullptr;
        return false;
    }

    PYAE_LOG_INFO("PySideLoader", "PyAEPySide.dll loaded and initialized successfully");
    return true;

#else
    PYAE_LOG_ERROR("PySideLoader", "Dynamic loading not supported on this platform");
    return false;
#endif
}

void PySideLoader::UnloadPlugin() {
    if (!m_plugin) {
        return;
    }

    PYAE_LOG_INFO("PySideLoader", "Unloading PyAEPySide.dll...");

    // プラグインをシャットダウン
    m_plugin->Shutdown();

    // プラグインインスタンスを破棄
    if (m_destroyFunc) {
        m_destroyFunc(m_plugin);
    }
    m_plugin = nullptr;
    m_destroyFunc = nullptr;

#ifdef _WIN32
    // DLLをアンロード
    if (m_dllHandle) {
        FreeLibrary(m_dllHandle);
        m_dllHandle = nullptr;
    }

    // DLLディレクトリを削除
    RemovePySide6DllDirectories();
#endif

    PYAE_LOG_INFO("PySideLoader", "PyAEPySide.dll unloaded");
}

} // namespace PyAE
