// IdleHandler.h
// PyAE - Python for After Effects
// AEアイドルフック処理

#pragma once

// C++標準ライブラリを先にインクルード
#include <atomic>
#include <chrono>
#include <functional>
#include <string>
#include <memory>

// After Effects SDK
#include "AE_GeneralPlug.h"

// PyAE
#include "TaskQueue.h"

namespace PyAE {

class IdleHandler {
public:
    // 自動テストタスク構造体
    struct AutoTestTask {
        std::string scriptPath;      // Pythonファイルパス
        std::string moduleName;      // Pythonモジュール名
        std::string functionName;    // 実行する関数名
        std::string outputPath;      // 結果出力パス
        bool autoExit;               // 完了後に終了するか
        int timeout;                 // タイムアウト（秒）
        int delay;                   // 実行前の待機時間（秒）

        std::chrono::steady_clock::time_point queuedTime;
        bool executed = false;
        bool completed = false;
    };

    static IdleHandler& Instance() {
        static IdleHandler instance;
        return instance;
    }

    // 初期化・終了
    bool Initialize();
    void Shutdown();

    // アイドルフックコールバック（AEから呼ばれる）
    A_Err OnIdle(AEGP_GlobalRefcon plugin_refconP,
                 AEGP_IdleRefcon refconP,
                 A_long* max_sleepPL);

    // タスク追加
    void EnqueueTask(std::function<void()> task, TaskPriority priority = TaskPriority::Normal,
                     const std::string& description = "", const std::string& sourceLocation = "");

    // 結果を返すタスク追加
    template<typename F, typename R = std::invoke_result_t<F>>
    std::future<R> EnqueueTaskWithResult(F&& func, TaskPriority priority = TaskPriority::Normal,
                                         const std::string& description = "", const std::string& sourceLocation = "") {
        return m_taskQueue.PushWithResult(std::forward<F>(func), priority, description, sourceLocation);
    }

    // 自動テスト
    void QueueAutoTest(
        const std::string& scriptPath,
        const std::string& moduleName,
        const std::string& functionName,
        const std::string& outputPath,
        bool autoExit,
        int timeout,
        int delay
    );

    // 設定
    void SetMaxTasksPerIdle(size_t count) { m_maxTasksPerIdle = count; }
    void SetIdleInterval(std::chrono::milliseconds interval) { m_idleInterval = interval; }
    void SetSuspended(bool suspended) { m_suspended.store(suspended); }

    // 状態
    bool IsInitialized() const { return m_initialized.load(); }
    size_t GetPendingTaskCount() const { return m_taskQueue.Size(); }

private:
    IdleHandler() = default;
    ~IdleHandler() = default;

    IdleHandler(const IdleHandler&) = delete;
    IdleHandler& operator=(const IdleHandler&) = delete;

    void ProcessPendingTasks();
    void ProcessAutoTest();
    void ExecuteAutoTest();
    void HandleTestCompletion(bool success);
    void ExecuteQuitCommand();

    TaskQueue m_taskQueue;
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_suspended{false};

    // 自動テスト
    std::unique_ptr<AutoTestTask> m_autoTestTask;

    // 設定
    size_t m_maxTasksPerIdle = 10;  // 1回のアイドルで処理する最大タスク数
    std::chrono::milliseconds m_idleInterval{16};  // アイドル間隔（約60FPS）

    // 統計
    std::atomic<uint64_t> m_totalTasksProcessed{0};
    std::atomic<uint64_t> m_totalIdleCalls{0};
};

// アイドルフックコールバック関数（C関数としてAEに登録）
extern "C" {
    A_Err PyAE_IdleHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_IdleRefcon refconP,
        A_long* max_sleepPL
    );
}

} // namespace PyAE
