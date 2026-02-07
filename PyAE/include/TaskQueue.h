// TaskQueue.h
// PyAE - Python for After Effects
// スレッドセーフなタスクキュー
// Uses Windows CRITICAL_SECTION to avoid C++ runtime mutex compatibility issues

#pragma once

#include <queue>
#include <functional>
#include <future>
#include <atomic>
#include <optional>
#include <chrono>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

namespace PyAE {

// Forward declaration - CriticalSection and CSLockGuard are defined in Logger.h
// But we need to define them here as well to avoid circular dependencies

// Windows CRITICAL_SECTION wrapper (avoids C++ runtime mutex issues)
class TaskQueueCS {
public:
    TaskQueueCS() {
        InitializeCriticalSection(&m_cs);
        InitializeConditionVariable(&m_cv);
    }
    ~TaskQueueCS() {
        DeleteCriticalSection(&m_cs);
        // CONDITION_VARIABLE does not need cleanup
    }
    void lock() {
        EnterCriticalSection(&m_cs);
    }
    void unlock() {
        LeaveCriticalSection(&m_cs);
    }

    // Wait indefinitely
    void wait() {
        SleepConditionVariableCS(&m_cv, &m_cs, INFINITE);
    }

    // Wait with timeout (milliseconds), returns true if signaled, false if timeout
    bool wait_for(DWORD milliseconds) {
        return SleepConditionVariableCS(&m_cv, &m_cs, milliseconds) != 0;
    }

    void notify_one() {
        WakeConditionVariable(&m_cv);
    }

    void notify_all() {
        WakeAllConditionVariable(&m_cv);
    }

    CRITICAL_SECTION* get_cs() { return &m_cs; }

private:
    CRITICAL_SECTION m_cs;
    CONDITION_VARIABLE m_cv;
    TaskQueueCS(const TaskQueueCS&) = delete;
    TaskQueueCS& operator=(const TaskQueueCS&) = delete;
};

// RAII lock guard for TaskQueueCS
class TaskQueueLock {
public:
    explicit TaskQueueLock(TaskQueueCS& cs) : m_cs(cs) {
        m_cs.lock();
    }
    ~TaskQueueLock() {
        m_cs.unlock();
    }
private:
    TaskQueueCS& m_cs;
    TaskQueueLock(const TaskQueueLock&) = delete;
    TaskQueueLock& operator=(const TaskQueueLock&) = delete;
};

// タスクの優先度
enum class TaskPriority {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

// タスク構造体
struct Task {
    std::function<void()> func;
    TaskPriority priority = TaskPriority::Normal;
    std::string description;  // タスクの説明
    std::string sourceLocation;  // タスクがエンキューされた場所

    // 優先度比較（高い方が先）
    bool operator<(const Task& other) const {
        return static_cast<int>(priority) < static_cast<int>(other.priority);
    }
};

// スレッドセーフなタスクキュー
class TaskQueue {
public:
    TaskQueue() : m_shutdown(false) {}
    ~TaskQueue() {
        Shutdown();
    }

    // シャットダウン
    void Shutdown() {
        {
            TaskQueueLock lock(m_cs);
            m_shutdown = true;
        }
        m_cs.notify_all();
    }

    bool IsShutdown() const {
        TaskQueueLock lock(const_cast<TaskQueueCS&>(m_cs));
        return m_shutdown;
    }

    // タスク追加
    void Push(std::function<void()> func, TaskPriority priority = TaskPriority::Normal,
              const std::string& description = "", const std::string& sourceLocation = "") {
        {
            TaskQueueLock lock(m_cs);
            if (m_shutdown) return;
            m_queue.push(Task{std::move(func), priority, description, sourceLocation});
        }
        m_cs.notify_one();
    }

    // 結果を返すタスク追加
    template<typename F, typename R = std::invoke_result_t<F>>
    std::future<R> PushWithResult(F&& func, TaskPriority priority = TaskPriority::Normal,
                                  const std::string& description = "", const std::string& sourceLocation = "") {
        auto promise = std::make_shared<std::promise<R>>();
        auto future = promise->get_future();

        Push([promise, func = std::forward<F>(func)]() mutable {
            try {
                if constexpr (std::is_void_v<R>) {
                    func();
                    promise->set_value();
                } else {
                    promise->set_value(func());
                }
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        }, priority, description, sourceLocation);

        return future;
    }

    // タスク取得（ブロッキング）
    std::optional<Task> Pop() {
        m_cs.lock();

        // Wait until queue is not empty or shutdown
        while (m_queue.empty() && !m_shutdown) {
            m_cs.wait();
        }

        if (m_shutdown && m_queue.empty()) {
            m_cs.unlock();
            return std::nullopt;
        }

        Task task = m_queue.top();
        m_queue.pop();
        m_cs.unlock();
        return task;
    }

    // タスク取得（タイムアウト付き）
    std::optional<Task> PopWithTimeout(std::chrono::milliseconds timeout) {
        m_cs.lock();

        auto deadline = std::chrono::steady_clock::now() + timeout;

        while (m_queue.empty() && !m_shutdown) {
            auto now = std::chrono::steady_clock::now();
            if (now >= deadline) {
                m_cs.unlock();
                return std::nullopt;  // タイムアウト
            }

            auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now);
            if (!m_cs.wait_for(static_cast<DWORD>(remaining.count()))) {
                // Timeout or spurious wakeup, check condition again
                continue;
            }
        }

        if (m_shutdown && m_queue.empty()) {
            m_cs.unlock();
            return std::nullopt;
        }

        Task task = m_queue.top();
        m_queue.pop();
        m_cs.unlock();
        return task;
    }

    // タスク取得（非ブロッキング）
    std::optional<Task> TryPop() {
        TaskQueueLock lock(m_cs);

        if (m_queue.empty()) {
            return std::nullopt;
        }

        Task task = m_queue.top();
        m_queue.pop();
        return task;
    }

    // キューのサイズ
    size_t Size() const {
        TaskQueueLock lock(const_cast<TaskQueueCS&>(m_cs));
        return m_queue.size();
    }

    // 空かどうか
    bool Empty() const {
        TaskQueueLock lock(const_cast<TaskQueueCS&>(m_cs));
        return m_queue.empty();
    }

    // クリア
    void Clear() {
        TaskQueueLock lock(m_cs);
        while (!m_queue.empty()) {
            m_queue.pop();
        }
    }

private:
    mutable TaskQueueCS m_cs;
    std::priority_queue<Task> m_queue;
    bool m_shutdown;
};

// 結果待機用ヘルパー
template<typename T>
class FutureWaiter {
public:
    explicit FutureWaiter(std::future<T>&& future)
        : m_future(std::move(future))
    {}

    // タイムアウト付き待機
    std::optional<T> Wait(std::chrono::milliseconds timeout) {
        if (m_future.wait_for(timeout) == std::future_status::ready) {
            return m_future.get();
        }
        return std::nullopt;
    }

    // 無限待機
    T Get() {
        return m_future.get();
    }

    bool IsReady() const {
        return m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
    }

private:
    std::future<T> m_future;
};

} // namespace PyAE
