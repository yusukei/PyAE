// PathManager.h
// PyAE - Python for After Effects
// プラグイン・スクリプトディレクトリのパス管理

#pragma once

#include <filesystem>
#include <string>

namespace PyAE {

// =============================================================
// PathManager クラス
// プラグインとスクリプトのパス管理を担当
// =============================================================
class PathManager {
public:
    PathManager() = default;

    // パス検出（Windows専用の実装）
    bool DetectPluginDirectory();

    // パスアクセス（読み取り専用）
    const std::filesystem::path& GetPluginDir() const { return m_pluginDir; }
    const std::filesystem::path& GetScriptsDir() const { return m_scriptsDir; }

    // パス設定（テスト用）
    void SetPluginDir(const std::filesystem::path& path) { m_pluginDir = path; }
    void SetScriptsDir(const std::filesystem::path& path) { m_scriptsDir = path; }

private:
    std::filesystem::path m_pluginDir;
    std::filesystem::path m_scriptsDir;
};

} // namespace PyAE
