// Logger.h
// PyAE - Python for After Effects
// ロギングシステム

#pragma once

#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <filesystem>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

namespace PyAE {

// Windows CRITICAL_SECTION wrapper (avoids C++ runtime mutex issues)
class CriticalSection {
public:
    CriticalSection() {
        InitializeCriticalSection(&m_cs);
    }
    ~CriticalSection() {
        DeleteCriticalSection(&m_cs);
    }
    void lock() {
        EnterCriticalSection(&m_cs);
    }
    void unlock() {
        LeaveCriticalSection(&m_cs);
    }
private:
    CRITICAL_SECTION m_cs;
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
};

// RAII lock guard for CriticalSection
class CSLockGuard {
public:
    explicit CSLockGuard(CriticalSection& cs) : m_cs(cs) {
        m_cs.lock();
    }
    ~CSLockGuard() {
        m_cs.unlock();
    }
private:
    CriticalSection& m_cs;
    CSLockGuard(const CSLockGuard&) = delete;
    CSLockGuard& operator=(const CSLockGuard&) = delete;
};

// OutputDebugString への出力（初期化前でも使用可能）
// UTF-8文字列を受け取り、UTF-16に変換してOutputDebugStringWを使用
inline void DebugOutput(const std::string& message) {
#ifdef _WIN32
    std::string prefixed = "[PyAE] " + message + "\n";

    // UTF-8 → UTF-16 変換
    int wideSize = MultiByteToWideChar(CP_UTF8, 0, prefixed.c_str(), -1, nullptr, 0);
    if (wideSize > 0) {
        std::wstring wide(wideSize - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, prefixed.c_str(), -1, &wide[0], wideSize);
        OutputDebugStringW(wide.c_str());
    } else {
        // フォールバック: ASCIIのみ出力
        OutputDebugStringA(prefixed.c_str());
    }
#endif
}

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
public:
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    // 初期化（プラグインパスを基準にログディレクトリを設定）
    // ファイル出力はデフォルトOff。SetFileOutputEnabled(true)で有効化。
    bool Initialize(const std::filesystem::path& pluginDir) {
        DebugOutput("Logger::Initialize - start");
        {
            CSLockGuard lock(m_cs);

            m_logDir = pluginDir / "logs";
            m_initialized = true;
            DebugOutput("Logger::Initialize - log dir set: " + m_logDir.string());
        }

        Log(LogLevel::Info, "Logger", "PyAE Logger initialized (file output: off)");

        DebugOutput("Logger::Initialize - complete");
        return true;
    }

    // ファイル出力の有効/無効を切り替え
    void SetFileOutputEnabled(bool enabled) {
        CSLockGuard lock(m_cs);
        if (enabled == m_fileOutputEnabled) {
            return;
        }
        m_fileOutputEnabled = enabled;

        if (enabled) {
            // Create log directory and open file
            try {
                std::filesystem::create_directories(m_logDir);
            } catch (const std::exception& e) {
                DebugOutput("Logger: Failed to create log dir: " + std::string(e.what()));
                m_fileOutputEnabled = false;
                return;
            }

            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::tm tm_buf;
            localtime_s(&tm_buf, &time_t);

            std::ostringstream filename;
            filename << "pyae_" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S") << ".log";
            m_logPath = m_logDir / filename.str();

            m_logFile.open(m_logPath, std::ios::out | std::ios::app | std::ios::binary);
            if (!m_logFile.is_open()) {
                DebugOutput("Logger: Failed to open log file: " + m_logPath.string());
                m_fileOutputEnabled = false;
                return;
            }

            // UTF-8 BOM
            if (m_logFile.tellp() == 0) {
                const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
                m_logFile.write(reinterpret_cast<const char*>(bom), sizeof(bom));
            }

            DebugOutput("Logger: File output enabled: " + m_logPath.string());

            // Cleanup old logs
            CleanupOldLogs(7);
        } else {
            if (m_logFile.is_open()) {
                m_logFile.close();
            }
            m_logPath.clear();
            DebugOutput("Logger: File output disabled");
        }
    }

    bool IsFileOutputEnabled() const {
        return m_fileOutputEnabled;
    }

    void Shutdown() {
        CSLockGuard lock(m_cs);
        if (m_logFile.is_open()) {
            m_logFile.close();
        }
        m_fileOutputEnabled = false;
        m_initialized = false;
    }

    void SetMinLevel(LogLevel level) {
        CSLockGuard lock(m_cs);
        m_minLevel = level;
    }

    void Log(LogLevel level, const std::string& component, const std::string& message) {
        CSLockGuard lock(m_cs);

        // 常にDebugViewに出力（初期化前でも）
        std::ostringstream debugMsg;
        debugMsg << "[" << LevelToString(level) << "] "
                 << "[" << component << "] "
                 << message;
        DebugOutput(debugMsg.str());

        if (!m_initialized || level < m_minLevel) {
            return;
        }

        if (!m_fileOutputEnabled || !m_logFile.is_open()) {
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_s(&tm_buf, &time_t);

        m_logFile << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S")
                  << " [" << LevelToString(level) << "] "
                  << "[" << component << "] "
                  << message << "\n";
        m_logFile.flush();
    }

    // 便利メソッド
    void Debug(const std::string& component, const std::string& message) {
        Log(LogLevel::Debug, component, message);
    }

    void Info(const std::string& component, const std::string& message) {
        Log(LogLevel::Info, component, message);
    }

    void Warning(const std::string& component, const std::string& message) {
        Log(LogLevel::Warning, component, message);
    }

    void Error(const std::string& component, const std::string& message) {
        Log(LogLevel::Error, component, message);
    }

    void Fatal(const std::string& component, const std::string& message) {
        Log(LogLevel::Fatal, component, message);
    }

    const std::filesystem::path& GetLogPath() const {
        return m_logPath;
    }

private:
    Logger() = default;
    ~Logger() { Shutdown(); }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static const char* LevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO ";
            case LogLevel::Warning: return "WARN ";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
            default:                return "?????";
        }
    }

    void CleanupOldLogs(int daysToKeep) {
        auto now = std::filesystem::file_time_type::clock::now();
        auto threshold = now - std::chrono::hours(24 * daysToKeep);

        try {
            for (const auto& entry : std::filesystem::directory_iterator(m_logDir)) {
                if (entry.is_regular_file() &&
                    entry.path().extension() == ".log" &&
                    entry.last_write_time() < threshold) {
                    std::filesystem::remove(entry.path());
                }
            }
        } catch (const std::filesystem::filesystem_error&) {
            // クリーンアップ失敗は無視
        }
    }

    CriticalSection m_cs;
    std::ofstream m_logFile;
    std::filesystem::path m_logDir;
    std::filesystem::path m_logPath;
    LogLevel m_minLevel = LogLevel::Info;
    bool m_fileOutputEnabled = false;
    bool m_initialized = false;
};

// 便利マクロ
#define PYAE_LOG_DEBUG(component, msg)   PyAE::Logger::Instance().Debug(component, msg)
#define PYAE_LOG_INFO(component, msg)    PyAE::Logger::Instance().Info(component, msg)
#define PYAE_LOG_WARNING(component, msg) PyAE::Logger::Instance().Warning(component, msg)
#define PYAE_LOG_ERROR(component, msg)   PyAE::Logger::Instance().Error(component, msg)
#define PYAE_LOG_FATAL(component, msg)   PyAE::Logger::Instance().Fatal(component, msg)

} // namespace PyAE
