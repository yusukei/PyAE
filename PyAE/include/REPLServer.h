// REPLServer.h
// PyAE - Python for After Effects
// REPLサーバー（デバッグ・開発用）

#pragma once

#ifdef PYAE_ENABLE_REPL

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include "WinSync.h"
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

namespace PyAE {

// REPL設定
struct REPLConfig {
    int port = 9999;
    std::string bindAddress = "127.0.0.1";  // ローカルのみ（セキュリティ）
    bool requireAuth = true;                 // 認証を要求
    std::string authToken;                   // 認証トークン
    int maxConnections = 1;                  // 最大同時接続数
    int timeoutSeconds = 300;                // タイムアウト（秒）
    size_t maxMessageSize = 1024 * 1024;     // 最大メッセージサイズ（1MB）
};

// REPLセッション
class REPLSession {
public:
    REPLSession(int socket, const REPLConfig& config);
    ~REPLSession();

    void Start();
    void Stop();
    bool IsRunning() const { return m_running.load(); }
    bool IsAuthenticated() const { return m_authenticated; }

private:
    void SessionLoop();
    std::string ProcessCommand(const std::string& command);
    bool Authenticate(const std::string& token);
    void Send(const std::string& message);
    std::string Receive();

    int m_socket;
    REPLConfig m_config;
    std::thread m_thread;
    std::atomic<bool> m_running{false};
    bool m_authenticated = false;
};

class REPLServer {
public:
    static REPLServer& Instance() {
        static REPLServer instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize(const REPLConfig& config);
    void Shutdown();

    // サーバー制御
    bool Start();
    void Stop();

    // 状態
    bool IsInitialized() const { return m_initialized; }
    bool IsRunning() const { return m_running.load(); }
    int GetPort() const { return m_config.port; }
    int GetActiveConnections() const;

    // 認証トークン生成
    static std::string GenerateAuthToken();

private:
    REPLServer() = default;
    ~REPLServer() = default;

    REPLServer(const REPLServer&) = delete;
    REPLServer& operator=(const REPLServer&) = delete;

    void AcceptLoop();
    void CleanupSessions();

#ifdef _WIN32
    SOCKET m_serverSocket = INVALID_SOCKET;
#else
    int m_serverSocket = -1;
#endif

    REPLConfig m_config;
    std::thread m_acceptThread;
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_running{false};

    mutable WinMutex m_sessionsMutex;
    std::vector<std::unique_ptr<REPLSession>> m_sessions;
};

} // namespace PyAE

#endif // PYAE_ENABLE_REPL
