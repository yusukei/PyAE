/**
 * @file test_runner.cpp
 * @brief High-performance test runner for PyAE
 *
 * Integrates OutputDebugString capture with After Effects process management
 * for automated testing. Replaces the slow Python-based implementation.
 *
 * ## 主要機能 (Main Features)
 * 1. After Effectsプロセスの起動と監視
 * 2. OutputDebugStringキャプチャ（デバッグログ収集）
 * 3. テスト結果の収集とレポート生成
 * 4. プロセス終了後の確実なクリーンアップ
 *
 * ## 重要な設計方針 (Critical Design Principles)
 * 1. **確実なプロセス終了**: After Effectsが終了したら、test_runnerも即座に終了
 * 2. **スレッド管理**: デバッグキャプチャスレッドを停止イベントで即座に終了
 * 3. **リソース管理**: プロセスハンドルを適切にクローズして残存プロセスを防ぐ
 * 4. **タイムアウト制御**: 長時間のハングを防ぐためのタイムアウト設定
 *
 * ## プロセス終了の流れ (Process Termination Flow)
 * 1. WaitForCompletion(): After Effectsプロセスの終了を検出
 * 2. StopDebugCapture(): デバッグキャプチャスレッドを停止（停止イベント使用）
 * 3. ForceQuitIfRunning(): 残っているAfterFX.exeを強制終了
 * 4. CheckAndDisplayResults(): テスト結果を表示
 * 5. test_runnerが終了
 *
 * ## 問題修正履歴 (Fix History)
 * - 2026-02-02: プロセス終了問題を修正
 *   - 停止イベント（m_stopEvent）を追加してスレッドを即座に終了
 *   - CREATE_NEW_PROCESS_GROUPフラグを削除してプロセス終了シグナルを正確に伝播
 *   - プロセスハンドルを即座にクローズして残存プロセスを防ぐ
 *   - タイムアウトを1000ms→200msに短縮して応答性を向上
 */

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

// DBWIN Protocol Constants
constexpr const char* BUFFER_READY_EVENT = "DBWIN_BUFFER_READY";
constexpr const char* DATA_READY_EVENT = "DBWIN_DATA_READY";
constexpr const char* BUFFER_NAME = "DBWIN_BUFFER";
constexpr size_t BUFFER_SIZE = 4096;

/**
 * @brief High-performance OutputDebugString capture using DBWIN protocol
 */
class DebugStringCapture {
public:
    DebugStringCapture(DWORD filterPID = 0, const std::string& outputFile = "")
        : m_outputFile(outputFile)
        , m_running(false)
        , m_bufferReady(nullptr)
        , m_dataReady(nullptr)
        , m_stopEvent(nullptr)
        , m_sharedMem(nullptr)
        , m_sharedBuffer(nullptr)
    {
        if (filterPID > 0) {
            m_filterPIDs.push_back(filterPID);
        }
    }

    void AddFilterPID(DWORD pid) {
        if (pid > 0) {
            m_filterPIDs.push_back(pid);
        }
    }

    ~DebugStringCapture() {
        Stop();
    }

    bool Start() {
        // CRITICAL: Create stop event to interrupt CaptureOne() waiting
        // Without this, thread may hang for up to 1000ms after m_running = false
        m_stopEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);
        if (!m_stopEvent) {
            std::cerr << "Failed to create STOP event: " << GetLastError() << "\n";
            return false;
        }

        // Create events
        m_bufferReady = CreateEventA(nullptr, FALSE, FALSE, BUFFER_READY_EVENT);
        if (!m_bufferReady) {
            DWORD err = GetLastError();
            if (err == ERROR_ACCESS_DENIED) {
                std::cerr << "Error: Access denied. Another debug viewer may be running.\n";
                std::cerr << "Close DebugView or Visual Studio debugger and try again.\n";
            } else {
                std::cerr << "Failed to create BUFFER_READY event: " << err << "\n";
            }
            return false;
        }

        m_dataReady = CreateEventA(nullptr, FALSE, FALSE, DATA_READY_EVENT);
        if (!m_dataReady) {
            std::cerr << "Failed to create DATA_READY event: " << GetLastError() << "\n";
            return false;
        }

        // Create shared memory
        m_sharedMem = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            nullptr,
            PAGE_READWRITE,
            0,
            BUFFER_SIZE,
            BUFFER_NAME
        );
        if (!m_sharedMem) {
            std::cerr << "Failed to create shared memory: " << GetLastError() << "\n";
            return false;
        }

        m_sharedBuffer = static_cast<char*>(MapViewOfFile(
            m_sharedMem,
            FILE_MAP_READ,
            0,
            0,
            BUFFER_SIZE
        ));
        if (!m_sharedBuffer) {
            std::cerr << "Failed to map shared memory: " << GetLastError() << "\n";
            return false;
        }

        // Open output file
        if (!m_outputFile.empty()) {
            m_logFile.open(m_outputFile, std::ios::out | std::ios::trunc);
            if (!m_logFile.is_open()) {
                std::cerr << "Failed to open log file: " << m_outputFile << "\n";
                return false;
            }
            Log("Debug capture started at " + GetTimestamp());
            if (!m_filterPIDs.empty()) {
                std::ostringstream oss;
                oss << "Filtering PIDs: ";
                for (size_t i = 0; i < m_filterPIDs.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << m_filterPIDs[i];
                }
                Log(oss.str());
            }
            Log(std::string(60, '-'));
        }

        m_running = true;
        std::cout << "[DebugCapture] Started (PID filter: ";
        if (m_filterPIDs.empty()) {
            std::cout << "none";
        } else {
            for (size_t i = 0; i < m_filterPIDs.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << m_filterPIDs[i];
            }
        }
        std::cout << ")\n";

        return true;
    }

    void Stop() {
        m_running = false;

        // CRITICAL: Signal stop event to interrupt CaptureOne() waiting
        // This allows immediate thread termination instead of waiting for timeout
        if (m_stopEvent) {
            SetEvent(m_stopEvent);
        }

        if (m_logFile.is_open()) {
            Log(std::string(60, '-'));
            Log("Debug capture stopped at " + GetTimestamp());
            m_logFile.close();
        }

        if (m_sharedBuffer) {
            UnmapViewOfFile(m_sharedBuffer);
            m_sharedBuffer = nullptr;
        }

        if (m_sharedMem) {
            CloseHandle(m_sharedMem);
            m_sharedMem = nullptr;
        }

        if (m_dataReady) {
            CloseHandle(m_dataReady);
            m_dataReady = nullptr;
        }

        if (m_bufferReady) {
            CloseHandle(m_bufferReady);
            m_bufferReady = nullptr;
        }

        if (m_stopEvent) {
            CloseHandle(m_stopEvent);
            m_stopEvent = nullptr;
        }

        std::cout << "[DebugCapture] Stopped\n";
    }

    bool CaptureOne(DWORD timeoutMs, DWORD& outPID, std::string& outMessage) {
        if (!m_running) {
            return false;
        }

        // Signal ready to receive
        SetEvent(m_bufferReady);

        // CRITICAL: Monitor both data and stop events
        // This allows immediate thread termination when Stop() is called
        // データ受信とスレッド停止の両方を監視することで、
        // Stop()呼び出し時にタイムアウトを待たずに即座に終了できる
        HANDLE handles[2] = { m_dataReady, m_stopEvent };
        DWORD result = WaitForMultipleObjects(2, handles, FALSE, timeoutMs);

        if (result == WAIT_OBJECT_0) {
            // Data received (データ受信)
            // Read PID (first 4 bytes)
            DWORD pid = *reinterpret_cast<DWORD*>(m_sharedBuffer);

            // Read message (remaining bytes, null-terminated)
            const char* messageStart = m_sharedBuffer + sizeof(DWORD);
            std::string message(messageStart);

            outPID = pid;
            outMessage = message;
            return true;
        } else if (result == WAIT_OBJECT_0 + 1) {
            // Stop signal received (停止シグナル受信 → 即座に終了)
            return false;
        }

        // Timeout or error
        return false;
    }

    void CaptureLoop() {
        // CRITICAL: Use short timeout (200ms) for better responsiveness
        // When Stop() is called, thread will exit within 200ms instead of 1000ms
        // 短いタイムアウト（200ms）で応答性を向上させる
        // Stop()呼び出し時、1000msではなく200ms以内にスレッドが終了する
        while (m_running) {
            DWORD pid;
            std::string message;

            if (CaptureOne(200, pid, message)) {
                // Apply PID filter (allow multiple PIDs)
                if (!m_filterPIDs.empty()) {
                    bool allowed = false;
                    for (DWORD filterPID : m_filterPIDs) {
                        if (pid == filterPID) {
                            allowed = true;
                            break;
                        }
                    }
                    if (!allowed) {
                        continue;
                    }
                }

                // Format output
                std::ostringstream oss;
                oss << "[" << GetTimeString() << "] ["
                    << std::setw(5) << pid << "] " << message;

                Log(oss.str());
            }
        }
    }

    bool IsRunning() const { return m_running; }

private:
    void Log(const std::string& message) {
        std::cout << message << "\n";
        if (m_logFile.is_open()) {
            m_logFile << message << "\n";
            m_logFile.flush();
        }
    }

    std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::string GetTimeString() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm;
        localtime_s(&tm, &time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S") << "."
            << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    std::vector<DWORD> m_filterPIDs;
    std::string m_outputFile;
    std::atomic<bool> m_running;
    std::ofstream m_logFile;

    HANDLE m_bufferReady;
    HANDLE m_dataReady;
    HANDLE m_stopEvent;      // Event to signal thread termination (スレッド終了通知用イベント)
    HANDLE m_sharedMem;
    char* m_sharedBuffer;
};

/**
 * @brief Test runner with integrated process and debug capture management
 */
class TestRunner {
public:
    TestRunner(
        const std::string& aePath,
        const std::string& pluginDir,
        const std::string& pyaeDir,
        const std::string& logsDir,
        const std::string& debugLog,
        const std::string& resultJson,
        const std::string& resultTxt,
        int timeoutSeconds,
        const std::string& testTarget = "",
        int testRepeat = 1
    )
        : m_aePath(aePath)
        , m_pluginDir(pluginDir)
        , m_pyaeDir(pyaeDir)
        , m_logsDir(logsDir)
        , m_debugLog(debugLog)
        , m_resultJson(resultJson)
        , m_resultTxt(resultTxt)
        , m_timeoutSeconds(timeoutSeconds)
        , m_testTarget(testTarget)
        , m_testRepeat(testRepeat)
        , m_aeProcess(nullptr)
        , m_running(false)
    {
    }

    ~TestRunner() {
        if (m_aeProcess) {
            CloseHandle(m_aeProcess);
        }
    }

    int Run() {
        PrintHeader();

        // Deploy test files
        if (!DeployTestFiles()) {
            return 1;
        }

        // Start After Effects
        DWORD aePID = StartAfterEffects();
        if (aePID == 0) {
            return 1;
        }

        // Small delay for process startup
        Sleep(1000);

        // Start debug capture in background thread
        m_capture = std::make_unique<DebugStringCapture>(aePID, m_debugLog);
        // Also capture test_runner.exe's own PID
        m_capture->AddFilterPID(GetCurrentProcessId());
        if (!m_capture->Start()) {
            std::cerr << "ERROR: Failed to start debug capture\n";
            std::cerr << "Continuing without debug capture...\n";
            m_capture.reset();
        }

        m_running = true;
        if (m_capture) {
            m_captureThread = std::thread([this]() {
                m_capture->CaptureLoop();
            });
        }

        // CRITICAL: Wait for After Effects process to complete
        // プロセスの完了を待機（タイムアウト付き）
        int returnCode = WaitForCompletion();

        // CRITICAL: Stop debug capture IMMEDIATELY after process exit
        // プロセス終了後、デバッグキャプチャを即座に停止
        StopDebugCapture();

        // Write exit summary to log file after capture stops
        WriteFinalSummary(returnCode);

        // CRITICAL: Force quit any remaining AfterFX.exe processes
        // 残っているAfterFX.exeプロセスを強制終了
        ForceQuitIfRunning();

        // Display results (no delay needed)
        CheckAndDisplayResults();

        PrintFooter();

        return returnCode;
    }

private:
    void PrintHeader() {
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "PyAE Test Runner with Debug Capture (C++)\n";
        std::cout << "Started at: " << GetTimestamp() << "\n";
        std::cout << std::string(70, '=') << "\n\n";
    }

    void PrintFooter() {
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "Finished at: " << GetTimestamp() << "\n";
        std::cout << std::string(70, '=') << "\n";
    }

    bool DeployTestFiles() {
        std::cout << "Deploying test scripts...\n";

        // Deploy scripts
        fs::path scriptsSrc = fs::path(m_pyaeDir) / "scripts";
        fs::path scriptsDst = fs::path(m_pluginDir) / "scripts";

        try {
            fs::create_directories(scriptsDst);

            for (const auto& entry : fs::directory_iterator(scriptsSrc)) {
                if (entry.path().extension() == ".py") {
                    fs::path dst = scriptsDst / entry.path().filename();
                    fs::copy_file(entry.path(), dst, fs::copy_options::overwrite_existing);
                }
            }
            std::cout << "  - Scripts deployed\n";
        } catch (const std::exception& e) {
            std::cerr << "  Warning: Failed to deploy scripts: " << e.what() << "\n";
        }

        // Deploy tests
        std::cout << "Deploying tests...\n";
        fs::path testsSrc = fs::path(m_pyaeDir) / "tests";
        fs::path testsDst = fs::path(m_pluginDir) / "tests";

        if (fs::exists(testsSrc)) {
            try {
                if (fs::exists(testsDst)) {
                    fs::remove_all(testsDst);
                }
                fs::copy(testsSrc, testsDst, fs::copy_options::recursive);
                std::cout << "  - Tests deployed\n";
            } catch (const std::exception& e) {
                std::cerr << "  Warning: Failed to deploy tests: " << e.what() << "\n";
            }
        }

        std::cout << "\n";
        return true;
    }

    DWORD StartAfterEffects() {
        std::cout << "Starting After Effects in Auto Test Mode...\n\n";

        std::cout << "Environment Variables Set:\n";
        // Display environment variables
        std::string testFunction = m_testTarget.empty() ? "run_all_tests" : "run_specific_tests";
        std::cout << "  PYAE_AUTO_TEST     = 1\n";
        std::cout << "  PYAE_TEST_MODULE   = test_runner\n";
        std::cout << "  PYAE_TEST_FUNCTION = " << testFunction << "\n";
        if (!m_testTarget.empty()) {
            std::cout << "  PYAE_TEST_TARGET   = " << m_testTarget << "\n";
        }
        if (m_testRepeat > 1) {
            std::cout << "  PYAE_TEST_REPEAT   = " << m_testRepeat << "\n";
        }
        std::cout << "  PYAE_TEST_OUTPUT   = " << m_resultJson << "\n\n";

        // Start process
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};

        std::wstring cmdLine = Utf8ToWide(m_aePath);
        // CreateProcessW modifies lpCommandLine, so we need a non-const buffer
        std::vector<wchar_t> cmdLineBuf(cmdLine.begin(), cmdLine.end());
        cmdLineBuf.push_back(L'\0');

        // Build environment block for child process
        // Use vector instead of wstring to properly handle embedded null characters
        std::vector<wchar_t> envBlock;

        // Helper lambda to append null-terminated string to envBlock
        auto appendEnv = [&envBlock](const std::wstring& str) {
            envBlock.insert(envBlock.end(), str.begin(), str.end());
            envBlock.push_back(L'\0');
        };

        // Get current environment variables
        wchar_t* parentEnv = GetEnvironmentStringsW();
        if (parentEnv) {
            // Copy all parent environment variables
            wchar_t* p = parentEnv;
            while (*p) {
                std::wstring var(p);
                // Skip variables we're going to override and empty variables
                if (!var.empty() && var.find(L"PYAE_") != 0) {
                    appendEnv(var);
                }
                p += wcslen(p) + 1;
            }
            FreeEnvironmentStringsW(parentEnv);
        }

        // Add PyAE test configuration variables
        appendEnv(L"PYAE_AUTO_TEST=1");
        appendEnv(L"PYAE_TEST_MODULE=test_runner");

        if (!m_testTarget.empty()) {
            appendEnv(L"PYAE_TEST_FUNCTION=run_specific_tests");
            appendEnv(L"PYAE_TEST_TARGET=" + Utf8ToWide(m_testTarget));
        } else {
            appendEnv(L"PYAE_TEST_FUNCTION=run_all_tests");
        }

        if (m_testRepeat > 1) {
            appendEnv(L"PYAE_TEST_REPEAT=" + std::to_wstring(m_testRepeat));
        }

        appendEnv(L"PYAE_TEST_OUTPUT=" + Utf8ToWide(m_resultJson));
        appendEnv(L"PYAE_TEST_EXIT=1");
        appendEnv(L"PYAE_TEST_TIMEOUT=" + std::to_wstring(m_timeoutSeconds));
        appendEnv(L"PYAE_TEST_DELAY=2");

        // Terminate environment block with double null
        envBlock.push_back(L'\0');

        // CRITICAL: Remove CREATE_NEW_PROCESS_GROUP flag
        // This flag can prevent proper process exit signal propagation
        // Without it, WaitForSingleObject(m_aeProcess) will reliably detect process exit
        // CREATE_NEW_PROCESS_GROUPフラグを削除
        // このフラグはプロセス終了シグナルの正確な伝播を阻害する可能性がある
        // これがないと、WaitForSingleObject(m_aeProcess)でプロセス終了を確実に検出できる
        BOOL success = CreateProcessW(
            nullptr,
            cmdLineBuf.data(),
            nullptr,
            nullptr,
            FALSE,
            CREATE_UNICODE_ENVIRONMENT,  // Only this flag (このフラグのみ)
            envBlock.data(),  // Pass environment block as vector
            nullptr,
            &si,
            &pi
        );

        if (!success) {
            std::cerr << "ERROR: Failed to start After Effects: " << GetLastError() << "\n";
            return 0;
        }

        m_aeProcess = pi.hProcess;
        CloseHandle(pi.hThread);

        std::cout << "After Effects started with PID: " << pi.dwProcessId << "\n";
        std::cout << "Timeout: " << m_timeoutSeconds << " seconds\n\n";

        return pi.dwProcessId;
    }

    int WaitForCompletion() {
        if (!m_aeProcess) {
            return -1;
        }

        auto startTime = std::chrono::steady_clock::now();
        DWORD timeoutMs = m_timeoutSeconds * 1000;

        // CRITICAL: Wait for After Effects process to exit
        // プロセス終了を待機（タイムアウト付き）
        DWORD result = WaitForSingleObject(m_aeProcess, timeoutMs);

        auto elapsed = std::chrono::steady_clock::now() - startTime;
        auto elapsedSec = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();

        if (result == WAIT_OBJECT_0) {
            // Process exited normally (プロセスが正常終了)
            DWORD exitCode;
            GetExitCodeProcess(m_aeProcess, &exitCode);
            m_exitCode = static_cast<int>(exitCode);

            // Log exit code - both to stdout and debug log file
            std::ostringstream exitMsg;
            exitMsg << "[TestRunner] After Effects exited with code " << exitCode
                    << " (0x" << std::hex << std::uppercase << exitCode << std::dec << ")";
            m_exitMsg = exitMsg.str();
            LogToDebugCapture(m_exitMsg);

            // Explain common exit codes
            std::string explanation = GetExitCodeExplanation(exitCode);
            if (!explanation.empty()) {
                std::string explainMsg = "[TestRunner] Exit code meaning: " + explanation;
                LogToDebugCapture(explainMsg);
            }

            std::ostringstream timeMsg;
            timeMsg << "[TestRunner] Elapsed time: " << elapsedSec << " seconds";
            LogToDebugCapture(timeMsg.str());

            // CRITICAL: Give capture thread time to process all messages before stopping
            // キャプチャスレッドが全メッセージを処理する時間を確保
            Sleep(500);

            // CRITICAL: Close process handle immediately after exit
            // プロセス終了後、すぐにハンドルをクローズ
            CloseHandle(m_aeProcess);
            m_aeProcess = nullptr;

            return m_exitCode;
        } else {
            // Timeout (タイムアウト → 強制終了)
            m_exitCode = -1;
            m_exitMsg = "[TestRunner] ERROR: Timeout";
            LogToDebugCapture(m_exitMsg);
            std::ostringstream timeoutMsg;
            timeoutMsg << "[TestRunner] Timeout after " << m_timeoutSeconds << " seconds";
            LogToDebugCapture(timeoutMsg.str());
            LogToDebugCapture("[TestRunner] Terminating After Effects...");

            TerminateProcess(m_aeProcess, 1);
            WaitForSingleObject(m_aeProcess, 5000);

            // CRITICAL: Close handle after forced termination
            // 強制終了後もハンドルをクローズ
            CloseHandle(m_aeProcess);
            m_aeProcess = nullptr;

            return m_exitCode;
        }
    }

    std::string GetExitCodeExplanation(DWORD exitCode) {
        switch (exitCode) {
            case 0:
                return "Success (normal exit)";
            case 1:
                return "General error";
            case 0xC0000005:
                return "ACCESS_VIOLATION (memory access error)";
            case 0xC000001D:
                return "ILLEGAL_INSTRUCTION";
            case 0xC0000094:
                return "INTEGER_DIVIDE_BY_ZERO";
            case 0xC0000096:
                return "PRIVILEGED_INSTRUCTION";
            case 0xC00000FD:
                return "STACK_OVERFLOW";
            case 0x40000005:
                return "STATUS_FLOAT_INEXACT_RESULT";
            case 0x40000015:
                return "STATUS_FATAL_APP_EXIT (application-requested exit)";
            case 0x80000003:
                return "STATUS_BREAKPOINT (debugger breakpoint)";
            case 0xC0000409:
                return "STATUS_STACK_BUFFER_OVERRUN (stack buffer overrun detected)";
            case 0xC0000374:
                return "STATUS_HEAP_CORRUPTION (heap corruption detected)";
            default:
                if (exitCode >= 0xC0000000) {
                    return "NTSTATUS error (likely crash)";
                } else if (exitCode >= 0x80000000) {
                    return "NTSTATUS warning";
                }
                return "";
        }
    }

    void LogToDebugCapture(const std::string& message) {
        // Write directly to debug log file first (most reliable)
        WriteToDebugLog(message);

        // Output to stdout
        std::cout << message << "\n";

        // Output via OutputDebugString so it gets captured
        OutputDebugStringA(message.c_str());

        // Ensure file operations complete before continuing
        Sleep(100);
    }

    void WriteToDebugLog(const std::string& message) {
        if (m_debugLog.empty()) {
            return;
        }

        try {
            std::ofstream logFile(m_debugLog, std::ios::app);
            if (logFile.is_open()) {
                logFile << "[" << GetTimeString() << "] [" << std::setw(5)
                        << GetCurrentProcessId() << "] " << message << "\n";
                logFile.flush();
                logFile.close();
            }
        } catch (...) {
            // Ignore errors - this is best-effort logging
        }
    }

    void StopDebugCapture() {
        // CRITICAL: Ensure thread terminates reliably
        // スレッドを確実に終了させる
        m_running = false;

        if (m_capture) {
            // Signal stop event to interrupt CaptureOne() immediately
            // 停止シグナルを送信して、CaptureOne()の待機を即座に中断
            m_capture->Stop();
        }

        // CRITICAL: Wait for thread termination with timeout
        // タイムアウト付きでスレッド終了を待機
        if (m_captureThread.joinable()) {
            auto startTime = std::chrono::steady_clock::now();
            const int timeoutSeconds = 3;
            bool joined = false;

            // Try to join with polling (実際にはjoinableの間待機)
            // std::threadにはtimed_joinがないため、detach前に短時間待機
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            if (m_captureThread.joinable()) {
                m_captureThread.join();
                joined = true;
            }

            if (!joined) {
                std::cerr << "WARNING: Capture thread did not terminate within timeout. Detaching...\n";
                if (m_captureThread.joinable()) {
                    m_captureThread.detach();
                }
            }
        }

        if (m_capture) {
            m_capture.reset();
        }

        std::cout << "[TestRunner] Debug capture stopped\n";
    }

    void WriteFinalSummary(int exitCode) {
        if (m_debugLog.empty()) {
            return;
        }

        try {
            std::ofstream logFile(m_debugLog, std::ios::app);
            if (logFile.is_open()) {
                logFile << std::string(60, '=') << "\n";
                logFile << "Test Runner Summary\n";
                logFile << std::string(60, '=') << "\n";
                logFile << "Exit Code: " << exitCode;
                if (exitCode >= 0) {
                    logFile << " (0x" << std::hex << std::uppercase << exitCode << std::dec << ")";
                }
                logFile << "\n";

                if (exitCode >= 0) {
                    std::string explanation = GetExitCodeExplanation(static_cast<DWORD>(exitCode));
                    if (!explanation.empty()) {
                        logFile << "Meaning: " << explanation << "\n";
                    }
                } else if (exitCode == -1) {
                    logFile << "Meaning: Timeout or terminated\n";
                }

                logFile << "Timestamp: " << GetTimestamp() << "\n";
                logFile << std::string(60, '=') << "\n";
                logFile.flush();
                logFile.close();
            }
        } catch (...) {
            // Ignore errors
        }
    }

    void ForceQuitIfRunning() {
        // Check if AfterFX.exe is still running
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return;
        }

        PROCESSENTRY32W pe = { sizeof(pe) };
        if (Process32FirstW(snapshot, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, L"AfterFX.exe") == 0) {
                    std::cout << "WARNING: After Effects did not exit properly. Force quitting...\n";
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                    if (hProcess) {
                        TerminateProcess(hProcess, 1);
                        CloseHandle(hProcess);
                    }
                    Sleep(1000);
                    break;
                }
            } while (Process32NextW(snapshot, &pe));
        }

        CloseHandle(snapshot);
    }

    void CheckAndDisplayResults() {
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "Test Execution Complete\n";
        std::cout << std::string(70, '=') << "\n";

        // Check text results
        if (fs::exists(m_resultTxt)) {
            std::cout << "\nResults Summary:\n";
            std::cout << std::string(70, '-') << "\n";

            std::ifstream file(m_resultTxt);
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    std::cout << line << "\n";
                }
                file.close();
            }
            std::cout << std::string(70, '-') << "\n";
        }

        // Check JSON results
        if (fs::exists(m_resultJson)) {
            std::cout << "\nJSON results: " << m_resultJson << "\n";

            // Copy to logs directory
            fs::path dst = fs::path(m_logsDir) / "test_results.json";
            fs::path src = fs::absolute(m_resultJson);
            fs::path dstAbs = fs::absolute(dst);

            if (src != dstAbs) {
                try {
                    fs::create_directories(dstAbs.parent_path());
                    fs::copy_file(src, dstAbs, fs::copy_options::overwrite_existing);
                    std::cout << "Results copied to: " << dstAbs.string() << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to copy results: " << e.what() << "\n";
                }
            }
        } else {
            std::cout << "\nWARNING: Result file not found.\n";
            std::cout << "  Expected: " << m_resultJson << "\n";
        }

        // Debug log info
        if (fs::exists(m_debugLog)) {
            std::cout << "\nDebug log: " << m_debugLog << "\n";

            // Copy to logs directory
            fs::path dst = fs::path(m_logsDir) / "test_debug.log";
            fs::path src = fs::absolute(m_debugLog);
            fs::path dstAbs = fs::absolute(dst);

            if (src != dstAbs) {
                try {
                    fs::create_directories(dstAbs.parent_path());
                    fs::copy_file(src, dstAbs, fs::copy_options::overwrite_existing);
                    std::cout << "Debug log copied to: " << dstAbs.string() << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to copy debug log: " << e.what() << "\n";
                }
            }
        }
    }

    std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::string GetTimeString() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm;
        localtime_s(&tm, &time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S") << "."
            << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    std::wstring Utf8ToWide(const std::string& utf8) {
        if (utf8.empty()) return std::wstring();

        // Get size including NULL terminator
        int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
        if (size <= 0) return std::wstring();

        // Allocate buffer and convert
        std::wstring result(size - 1, 0);  // Exclude NULL terminator from string length
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &result[0], size);
        return result;
    }

    std::string m_aePath;
    std::string m_pluginDir;
    std::string m_pyaeDir;
    std::string m_logsDir;
    std::string m_debugLog;
    std::string m_resultJson;
    std::string m_resultTxt;
    std::string m_testTarget;
    int m_testRepeat;
    int m_timeoutSeconds;

    HANDLE m_aeProcess;
    std::unique_ptr<DebugStringCapture> m_capture;
    std::thread m_captureThread;
    std::atomic<bool> m_running;
    int m_exitCode = -1;
    std::string m_exitMsg;
};

/**
 * @brief Parse command line arguments
 */
struct Config {
    std::string aePath = R"(C:\Program Files\Adobe\Adobe After Effects 2025\Support Files\AfterFX.exe)";
    std::string pluginDir = R"(C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE)";
    std::string pyaeDir;
    std::string logsDir;     // Logs output directory (default: project_root/logs/)
    std::string debugLog;
    std::string testTarget;  // Test target module(s)
    int testRepeat = 1;      // Test repeat count (within same AE process)
    int timeout = 600;
};

void PrintUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --ae-path PATH        Path to After Effects executable\n";
    std::cout << "  --plugin-dir PATH     PyAE plugin directory\n";
    std::cout << "  --pyae-dir PATH       PyAE source directory (auto-detected if not specified)\n";
    std::cout << "  --timeout SECONDS     Timeout in seconds (default: 600)\n";
    std::cout << "  --debug-log PATH      Debug log file path\n";
    std::cout << "  --test-target MODULE  Test module(s) to run (e.g., \"Composition\" or \"Composition,Layer\")\n";
    std::cout << "  --test-repeat N       Repeat tests N times within the same AE process (default: 1)\n";
    std::cout << "  --help                Show this help message\n";
}

bool ParseArgs(int argc, char* argv[], Config& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            PrintUsage(argv[0]);
            return false;
        } else if (arg == "--ae-path" && i + 1 < argc) {
            config.aePath = argv[++i];
        } else if (arg == "--plugin-dir" && i + 1 < argc) {
            config.pluginDir = argv[++i];
        } else if (arg == "--pyae-dir" && i + 1 < argc) {
            config.pyaeDir = argv[++i];
            // Remove trailing backslash if present
            if (!config.pyaeDir.empty() && config.pyaeDir.back() == '\\') {
                config.pyaeDir.pop_back();
            }
        } else if (arg == "--timeout" && i + 1 < argc) {
            try {
                config.timeout = std::stoi(argv[++i]);
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Invalid timeout value: " << argv[i] << "\n";
                return false;
            }
        } else if (arg == "--debug-log" && i + 1 < argc) {
            config.debugLog = argv[++i];
        } else if (arg == "--test-target" && i + 1 < argc) {
            config.testTarget = argv[++i];
        } else if (arg == "--test-repeat" && i + 1 < argc) {
            try {
                config.testRepeat = std::stoi(argv[++i]);
                if (config.testRepeat < 1) {
                    std::cerr << "ERROR: --test-repeat must be at least 1\n";
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Invalid test-repeat value: " << argv[i] << "\n";
                return false;
            }
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            return false;
        }
    }

    // Auto-detect PyAE directory if not specified
    if (config.pyaeDir.empty()) {
        // TestRunner.exe is at build/TestRunner/<config>/TestRunner.exe
        // Go up 3 levels to reach project root, then look for PyAE/
        fs::path exePath = fs::path(argv[0]).parent_path();
        fs::path projectRoot = exePath.parent_path().parent_path().parent_path();
        fs::path pyaeDir = projectRoot / "PyAE";
        if (fs::exists(pyaeDir / "src")) {
            config.pyaeDir = pyaeDir.string();
        } else {
            std::cerr << "ERROR: Could not auto-detect PyAE directory\n";
            std::cerr << "Please specify --pyae-dir\n";
            return false;
        }
    }

    // Set logs directory (project_root/logs/)
    if (config.logsDir.empty()) {
        fs::path projectRoot = fs::path(config.pyaeDir).parent_path();
        config.logsDir = (projectRoot / "logs").string();
    }

    // Create logs directory if it doesn't exist
    fs::create_directories(config.logsDir);

    // Set debug log path if not specified
    if (config.debugLog.empty()) {
        config.debugLog = (fs::path(config.logsDir) / "test_debug.log").string();
    }

    return true;
}

int main(int argc, char* argv[]) {
    try {
        Config config;

        if (!ParseArgs(argc, argv, config)) {
            return 1;
        }

        // Verify After Effects exists
        if (!fs::exists(config.aePath)) {
            std::cerr << "ERROR: After Effects not found at: " << config.aePath << "\n";
            return 1;
        }

        // Verify plugin exists
        fs::path pluginPath = fs::path(config.pluginDir) / "PyAEBridge.dll";
        if (!fs::exists(pluginPath)) {
            std::cerr << "ERROR: PyAEBridge.dll not found at: " << config.pluginDir << "\n";
            std::cerr << "Please deploy the plugin first.\n";
            return 1;
        }

        // Set result paths
        std::string resultJson = (fs::path(config.pluginDir) / "scripts" / "test_results.json").string();
        std::string resultTxt = (fs::path(config.pluginDir) / "scripts" / "test_results.txt").string();

        // Create and run test runner
        TestRunner runner(
            config.aePath,
            config.pluginDir,
            config.pyaeDir,
            config.logsDir,
            config.debugLog,
            resultJson,
            resultTxt,
            config.timeout,
            config.testTarget,
            config.testRepeat
        );

        return runner.Run();

    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception\n";
        return 1;
    }
}
