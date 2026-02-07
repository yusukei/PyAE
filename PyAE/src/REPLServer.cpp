// REPLServer.cpp
// PyAE - Python for After Effects
// REPLサーバーの実装

#ifdef PYAE_ENABLE_REPL

#include "REPLServer.h"
#include "PythonHost.h"
#include "Logger.h"

#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace PyAE {

// ===============================================
// 定数定義
// ===============================================

// REPLセッションのバッファサイズ
static constexpr size_t REPL_RECV_BUFFER_SIZE = 4096;

// 認証トークンの長さ（16進数文字数）
static constexpr int AUTH_TOKEN_LENGTH = 32;

// ===============================================
// REPLSession実装
// ===============================================

REPLSession::REPLSession(int socket, const REPLConfig& config)
    : m_socket(socket)
    , m_config(config)
    , m_authenticated(!config.requireAuth)  // 認証不要なら最初から認証済み
{
}

REPLSession::~REPLSession() {
    Stop();
}

void REPLSession::Start() {
    if (m_running.load()) {
        return;
    }

    m_running.store(true);
    m_thread = std::thread(&REPLSession::SessionLoop, this);
}

void REPLSession::Stop() {
    m_running.store(false);

    // ソケットを閉じる
#ifdef _WIN32
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
#else
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
#endif

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void REPLSession::SessionLoop() {
    PYAE_LOG_INFO("REPLSession", "Session started");

    // ウェルカムメッセージ
    if (m_config.requireAuth) {
        Send("PyAE REPL Server\r\nPlease authenticate with: AUTH <token>\r\n>>> ");
    } else {
        Send("PyAE REPL Server\r\n>>> ");
    }

    while (m_running.load()) {
        std::string input = Receive();
        if (input.empty()) {
            break;  // 接続切断
        }

        // 末尾の改行を削除
        while (!input.empty() && (input.back() == '\r' || input.back() == '\n')) {
            input.pop_back();
        }

        if (input.empty()) {
            Send(">>> ");
            continue;
        }

        // 認証処理
        if (!m_authenticated) {
            if (input.substr(0, 5) == "AUTH ") {
                std::string token = input.substr(5);
                if (Authenticate(token)) {
                    Send("Authentication successful\r\n>>> ");
                } else {
                    Send("Authentication failed\r\n>>> ");
                }
            } else {
                Send("Please authenticate first: AUTH <token>\r\n>>> ");
            }
            continue;
        }

        // 終了コマンド
        if (input == "exit" || input == "quit") {
            Send("Goodbye!\r\n");
            break;
        }

        // コマンド実行
        std::string result = ProcessCommand(input);
        Send(result + "\r\n>>> ");
    }

    PYAE_LOG_INFO("REPLSession", "Session ended");
}

std::string REPLSession::ProcessCommand(const std::string& command) {
    try {
        std::string error;
        bool success = PythonHost::Instance().ExecuteString(command, error);

        if (!success) {
            return "Error: " + error;
        }

        return "OK";

    } catch (const std::exception& e) {
        return std::string("Exception: ") + e.what();
    }
}

bool REPLSession::Authenticate(const std::string& token) {
    if (token == m_config.authToken) {
        m_authenticated = true;
        PYAE_LOG_INFO("REPLSession", "Client authenticated");
        return true;
    }

    PYAE_LOG_WARNING("REPLSession", "Authentication failed");
    return false;
}

void REPLSession::Send(const std::string& message) {
    if (message.size() > m_config.maxMessageSize) {
        return;
    }

#ifdef _WIN32
    send(m_socket, message.c_str(), static_cast<int>(message.size()), 0);
#else
    send(m_socket, message.c_str(), message.size(), 0);
#endif
}

std::string REPLSession::Receive() {
    char buffer[REPL_RECV_BUFFER_SIZE];
    int bytesReceived;

#ifdef _WIN32
    bytesReceived = recv(m_socket, buffer, sizeof(buffer) - 1, 0);
#else
    bytesReceived = recv(m_socket, buffer, sizeof(buffer) - 1, 0);
#endif

    if (bytesReceived <= 0) {
        return "";
    }

    buffer[bytesReceived] = '\0';
    return std::string(buffer);
}

// ===============================================
// REPLServer実装
// ===============================================

bool REPLServer::Initialize(const REPLConfig& config) {
    if (m_initialized.load()) {
        PYAE_LOG_WARNING("REPLServer", "Already initialized");
        return true;
    }

    m_config = config;

    PYAE_LOG_INFO("REPLServer", "Initializing REPL server...");

#ifdef _WIN32
    // Winsock初期化
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        PYAE_LOG_ERROR("REPLServer", "WSAStartup failed: " + std::to_string(result));
        return false;
    }
#endif

    m_initialized.store(true);

    PYAE_LOG_INFO("REPLServer", "REPL server initialized");
    PYAE_LOG_INFO("REPLServer", "Auth token generated (length: " + std::to_string(config.authToken.length()) + ")");

    return true;
}

void REPLServer::Shutdown() {
    if (!m_initialized.load()) {
        return;
    }

    PYAE_LOG_INFO("REPLServer", "Shutting down REPL server...");

    Stop();

#ifdef _WIN32
    WSACleanup();
#endif

    m_initialized.store(false);

    PYAE_LOG_INFO("REPLServer", "REPL server shutdown complete");
}

bool REPLServer::Start() {
    if (!m_initialized.load()) {
        return false;
    }

    if (m_running.load()) {
        return true;
    }

    PYAE_LOG_INFO("REPLServer", "Starting REPL server on port " + std::to_string(m_config.port));

    // ソケット作成
#ifdef _WIN32
    m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_serverSocket == INVALID_SOCKET) {
        PYAE_LOG_ERROR("REPLServer", "Failed to create socket");
        return false;
    }
#else
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0) {
        PYAE_LOG_ERROR("REPLServer", "Failed to create socket");
        return false;
    }
#endif

    // SO_REUSEADDR設定
    int opt = 1;
#ifdef _WIN32
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    // バインド
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(m_config.port));

    // ローカルホストのみにバインド（セキュリティ）
    inet_pton(AF_INET, m_config.bindAddress.c_str(), &addr.sin_addr);

    if (bind(m_serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        PYAE_LOG_ERROR("REPLServer", "Failed to bind socket");
#ifdef _WIN32
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
#else
        close(m_serverSocket);
        m_serverSocket = -1;
#endif
        return false;
    }

    // リッスン
    if (listen(m_serverSocket, m_config.maxConnections) < 0) {
        PYAE_LOG_ERROR("REPLServer", "Failed to listen on socket");
#ifdef _WIN32
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
#else
        close(m_serverSocket);
        m_serverSocket = -1;
#endif
        return false;
    }

    m_running.store(true);
    m_acceptThread = std::thread(&REPLServer::AcceptLoop, this);

    PYAE_LOG_INFO("REPLServer", "REPL server started");
    return true;
}

void REPLServer::Stop() {
    if (!m_running.load()) {
        return;
    }

    PYAE_LOG_INFO("REPLServer", "Stopping REPL server...");

    m_running.store(false);

    // サーバーソケットを閉じてacceptをunblock
#ifdef _WIN32
    if (m_serverSocket != INVALID_SOCKET) {
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
    }
#else
    if (m_serverSocket >= 0) {
        close(m_serverSocket);
        m_serverSocket = -1;
    }
#endif

    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }

    // 全セッションを停止
    CleanupSessions();

    PYAE_LOG_INFO("REPLServer", "REPL server stopped");
}

void REPLServer::AcceptLoop() {
    PYAE_LOG_INFO("REPLServer", "Accept loop started");

    while (m_running.load()) {
        sockaddr_in clientAddr = {};
        socklen_t clientAddrLen = sizeof(clientAddr);

#ifdef _WIN32
        SOCKET clientSocket = accept(m_serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            if (m_running.load()) {
                PYAE_LOG_ERROR("REPLServer", "Accept failed");
            }
            continue;
        }
#else
        int clientSocket = accept(m_serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocket < 0) {
            if (m_running.load()) {
                PYAE_LOG_ERROR("REPLServer", "Accept failed");
            }
            continue;
        }
#endif

        PYAE_LOG_INFO("REPLServer", "New connection accepted");

        // セッション数チェック
        {
            WinLockGuard lock(m_sessionsMutex);

            // 終了したセッションをクリーンアップ
            m_sessions.erase(
                std::remove_if(m_sessions.begin(), m_sessions.end(),
                              [](const std::unique_ptr<REPLSession>& s) { return !s->IsRunning(); }),
                m_sessions.end()
            );

            if (static_cast<int>(m_sessions.size()) >= m_config.maxConnections) {
                PYAE_LOG_WARNING("REPLServer", "Max connections reached, rejecting");
#ifdef _WIN32
                closesocket(clientSocket);
#else
                close(clientSocket);
#endif
                continue;
            }

            // 新しいセッションを作成
            auto session = std::make_unique<REPLSession>(static_cast<int>(clientSocket), m_config);
            session->Start();
            m_sessions.push_back(std::move(session));
        }
    }

    PYAE_LOG_INFO("REPLServer", "Accept loop ended");
}

void REPLServer::CleanupSessions() {
    WinLockGuard lock(m_sessionsMutex);

    for (auto& session : m_sessions) {
        session->Stop();
    }
    m_sessions.clear();
}

int REPLServer::GetActiveConnections() const {
    WinLockGuard lock(m_sessionsMutex);
    return static_cast<int>(std::count_if(m_sessions.begin(), m_sessions.end(),
                           [](const std::unique_ptr<REPLSession>& s) { return s->IsRunning(); }));
}

std::string REPLServer::GenerateAuthToken() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::ostringstream oss;
    oss << std::hex;
    for (int i = 0; i < AUTH_TOKEN_LENGTH; ++i) {
        oss << dis(gen);
    }

    return oss.str();
}

} // namespace PyAE

#endif // PYAE_ENABLE_REPL
