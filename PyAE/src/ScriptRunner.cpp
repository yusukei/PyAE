// ScriptRunner.cpp
// PyAE - Python for After Effects
// スクリプト実行管理の実装

#include "ScriptRunner.h"
#include "PythonHost.h"
#include "PluginState.h"
#include "Logger.h"

#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#endif

namespace PyAE {

bool ScriptRunner::Initialize() {
    if (m_initialized) {
        PYAE_LOG_WARNING("ScriptRunner", "Already initialized");
        return true;
    }

    PYAE_LOG_INFO("ScriptRunner", "Initializing ScriptRunner...");

    // デフォルトのスクリプトディレクトリを追加
    auto scriptsDir = PluginState::Instance().GetScriptsDir();
    if (!scriptsDir.empty()) {
        AddScriptDirectory(scriptsDir);
    }

    m_initialized = true;

    PYAE_LOG_INFO("ScriptRunner", "ScriptRunner initialized");
    return true;
}

void ScriptRunner::Shutdown() {
    if (!m_initialized) {
        return;
    }

    PYAE_LOG_INFO("ScriptRunner", "Shutting down ScriptRunner...");

    m_scriptDirs.clear();
    m_recentScripts.clear();

    m_initialized = false;

    PYAE_LOG_INFO("ScriptRunner", "ScriptRunner shutdown complete");
}

ScriptResult ScriptRunner::RunFile(const std::filesystem::path& scriptPath,
                                   const ScriptRunOptions& options) {
    ScriptResult result;
    result.success = false;

    if (!m_initialized) {
        result.error = "ScriptRunner not initialized";
        return result;
    }

    if (m_isRunning) {
        result.error = "Another script is already running";
        return result;
    }

    if (!std::filesystem::exists(scriptPath)) {
        result.error = "Script file not found: " + scriptPath.string();
        return result;
    }

    PYAE_LOG_INFO("ScriptRunner", "Running script: " + scriptPath.string());

    m_isRunning = true;

    auto startTime = std::chrono::steady_clock::now();

    // 進捗コールバック
    if (m_progressCallback) {
        m_progressCallback("Running script...", 0);
    }

    // スクリプト実行
    std::string errorOut;
    result.success = PythonHost::Instance().ExecuteFile(scriptPath, errorOut);
    result.error = errorOut;

    auto endTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    m_isRunning = false;

    if (result.success) {
        PYAE_LOG_INFO("ScriptRunner", "Script completed in " +
                     std::to_string(result.executionTime.count()) + "ms");
        AddToRecentScripts(scriptPath);
    } else {
        PYAE_LOG_ERROR("ScriptRunner", "Script failed: " + result.error);
    }

    // 進捗コールバック
    if (m_progressCallback) {
        m_progressCallback(result.success ? "Complete" : "Failed", 100);
    }

    return result;
}

ScriptResult ScriptRunner::RunString(const std::string& code,
                                     const ScriptRunOptions& options) {
    ScriptResult result;
    result.success = false;

    if (!m_initialized) {
        result.error = "ScriptRunner not initialized";
        return result;
    }

    if (m_isRunning) {
        result.error = "Another script is already running";
        return result;
    }

    PYAE_LOG_DEBUG("ScriptRunner", "Running code string");

    m_isRunning = true;

    auto startTime = std::chrono::steady_clock::now();

    std::string errorOut;
    result.success = PythonHost::Instance().ExecuteString(code, errorOut);
    result.error = errorOut;

    auto endTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    m_isRunning = false;

    return result;
}

std::vector<ScriptInfo> ScriptRunner::GetAvailableScripts() const {
    std::vector<ScriptInfo> scripts;

    for (const auto& dir : m_scriptDirs) {
        ScanScriptDirectory(dir, scripts);
    }

    // 名前でソート
    std::sort(scripts.begin(), scripts.end(),
              [](const ScriptInfo& a, const ScriptInfo& b) {
                  return a.name < b.name;
              });

    return scripts;
}

std::optional<ScriptInfo> ScriptRunner::GetScriptInfo(const std::filesystem::path& path) const {
    if (!std::filesystem::exists(path)) {
        return std::nullopt;
    }

    return ParseScriptMetadata(path);
}

void ScriptRunner::AddScriptDirectory(const std::filesystem::path& dir) {
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        // 重複チェック
        auto it = std::find(m_scriptDirs.begin(), m_scriptDirs.end(), dir);
        if (it == m_scriptDirs.end()) {
            m_scriptDirs.push_back(dir);
            PYAE_LOG_INFO("ScriptRunner", "Added script directory: " + dir.string());
        }
    }
}

void ScriptRunner::RemoveScriptDirectory(const std::filesystem::path& dir) {
    auto it = std::find(m_scriptDirs.begin(), m_scriptDirs.end(), dir);
    if (it != m_scriptDirs.end()) {
        m_scriptDirs.erase(it);
        PYAE_LOG_INFO("ScriptRunner", "Removed script directory: " + dir.string());
    }
}

std::optional<std::filesystem::path> ScriptRunner::ShowOpenScriptDialog() {
#ifdef _WIN32
    OPENFILENAMEW ofn = {};
    wchar_t szFile[MAX_PATH] = {};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Python Scripts (*.py)\0*.py\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;

    // 初期ディレクトリをスクリプトディレクトリに設定
    std::wstring initialDir;
    if (!m_scriptDirs.empty()) {
        initialDir = m_scriptDirs[0].wstring();
        ofn.lpstrInitialDir = initialDir.c_str();
    }

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrTitle = L"Select Python Script";

    if (GetOpenFileNameW(&ofn)) {
        return std::filesystem::path(szFile);
    }
#endif

    return std::nullopt;
}

void ScriptRunner::ScanScriptDirectory(const std::filesystem::path& dir,
                                       std::vector<ScriptInfo>& outScripts) const {
    if (!std::filesystem::exists(dir)) {
        return;
    }

    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".py") {
                // __で始まるファイルはスキップ
                std::string filename = entry.path().filename().string();
                if (filename.substr(0, 2) != "__") {
                    outScripts.push_back(ParseScriptMetadata(entry.path()));
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        PYAE_LOG_ERROR("ScriptRunner", std::string("Error scanning directory: ") + e.what());
    }
}

ScriptInfo ScriptRunner::ParseScriptMetadata(const std::filesystem::path& path) const {
    ScriptInfo info;
    info.path = path;
    info.name = path.stem().string();

    try {
        info.lastModified = std::chrono::system_clock::now();  // 簡略化

        // スクリプトファイルを読み込んでメタデータを抽出
        std::ifstream file(path);
        if (!file.is_open()) {
            return info;
        }

        std::string line;
        bool inDocstring = false;
        std::string docstring;

        // 最初の数行からメタデータを抽出
        int lineCount = 0;
        while (std::getline(file, line) && lineCount < 50) {
            lineCount++;

            // docstring検出
            if (line.find("\"\"\"") != std::string::npos || line.find("'''") != std::string::npos) {
                inDocstring = !inDocstring;
                continue;
            }

            if (inDocstring) {
                docstring += line + "\n";
            }

            // # Author: または # Version: パターン
            static const std::regex authorRegex(R"(#\s*Author:\s*(.+))");
            static const std::regex versionRegex(R"(#\s*Version:\s*(.+))");
            static const std::regex descRegex(R"(#\s*Description:\s*(.+))");

            std::smatch match;
            if (std::regex_search(line, match, authorRegex)) {
                info.author = match[1].str();
            } else if (std::regex_search(line, match, versionRegex)) {
                info.version = match[1].str();
            } else if (std::regex_search(line, match, descRegex)) {
                info.description = match[1].str();
            }
        }

        // docstringを説明として使用
        if (info.description.empty() && !docstring.empty()) {
            // 最初の行だけ使用
            size_t pos = docstring.find('\n');
            if (pos != std::string::npos) {
                info.description = docstring.substr(0, pos);
            } else {
                info.description = docstring;
            }
        }

    } catch (const std::exception& e) {
        PYAE_LOG_WARNING("ScriptRunner",
                        "Error parsing metadata for " + path.string() + ": " + e.what());
    }

    return info;
}

void ScriptRunner::AddToRecentScripts(const std::filesystem::path& path) {
    // 既存のエントリを削除
    auto it = std::find(m_recentScripts.begin(), m_recentScripts.end(), path);
    if (it != m_recentScripts.end()) {
        m_recentScripts.erase(it);
    }

    // 先頭に追加
    m_recentScripts.insert(m_recentScripts.begin(), path);

    // 最大数を超えたら古いものを削除
    while (m_recentScripts.size() > MAX_RECENT_SCRIPTS) {
        m_recentScripts.pop_back();
    }
}

} // namespace PyAE
