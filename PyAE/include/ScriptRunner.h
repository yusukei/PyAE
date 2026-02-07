// ScriptRunner.h
// PyAE - Python for After Effects
// スクリプト実行管理

#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <functional>
#include <optional>
#include <chrono>
#include <atomic>

namespace PyAE {

// スクリプト実行結果
struct ScriptResult {
    bool success = false;
    std::string output;
    std::string error;
    std::chrono::milliseconds executionTime{0};
};

// スクリプト情報
struct ScriptInfo {
    std::filesystem::path path;
    std::string name;
    std::string description;
    std::string author;
    std::string version;
    std::chrono::system_clock::time_point lastModified;
};

// スクリプト実行オプション
struct ScriptRunOptions {
    bool captureOutput = true;          // stdout/stderrをキャプチャ
    bool showProgressDialog = false;     // 進捗ダイアログを表示
    std::optional<std::chrono::seconds> timeout;  // タイムアウト
    std::unordered_map<std::string, std::string> environment;  // 追加環境変数
};

class ScriptRunner {
public:
    static ScriptRunner& Instance() {
        static ScriptRunner instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize();
    void Shutdown();

    // スクリプト実行
    ScriptResult RunFile(const std::filesystem::path& scriptPath,
                        const ScriptRunOptions& options = {});
    ScriptResult RunString(const std::string& code,
                          const ScriptRunOptions& options = {});

    // スクリプト検索
    std::vector<ScriptInfo> GetAvailableScripts() const;
    std::optional<ScriptInfo> GetScriptInfo(const std::filesystem::path& path) const;

    // スクリプトディレクトリ管理
    void AddScriptDirectory(const std::filesystem::path& dir);
    void RemoveScriptDirectory(const std::filesystem::path& dir);
    const std::vector<std::filesystem::path>& GetScriptDirectories() const { return m_scriptDirs; }

    // ファイルダイアログ
    std::optional<std::filesystem::path> ShowOpenScriptDialog();

    // 最近使用したスクリプト
    const std::vector<std::filesystem::path>& GetRecentScripts() const { return m_recentScripts; }
    void ClearRecentScripts() { m_recentScripts.clear(); }

    // コールバック
    using ProgressCallback = std::function<void(const std::string& message, int percent)>;
    void SetProgressCallback(ProgressCallback callback) { m_progressCallback = std::move(callback); }

    // 状態
    bool IsInitialized() const { return m_initialized; }
    bool IsRunning() const { return m_isRunning; }

private:
    ScriptRunner() = default;
    ~ScriptRunner() = default;

    ScriptRunner(const ScriptRunner&) = delete;
    ScriptRunner& operator=(const ScriptRunner&) = delete;

    void ScanScriptDirectory(const std::filesystem::path& dir, std::vector<ScriptInfo>& outScripts) const;
    ScriptInfo ParseScriptMetadata(const std::filesystem::path& path) const;
    void AddToRecentScripts(const std::filesystem::path& path);

    std::atomic<bool> m_initialized = false;
    std::atomic<bool> m_isRunning = false;

    std::vector<std::filesystem::path> m_scriptDirs;
    std::vector<std::filesystem::path> m_recentScripts;
    static constexpr size_t MAX_RECENT_SCRIPTS = 10;

    ProgressCallback m_progressCallback;
};

} // namespace PyAE
