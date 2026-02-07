// PyBatch.cpp
// PyAE - Python for After Effects
// バッチ操作とパフォーマンス最適化 (Phase 5)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PluginState.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "Logger.h"
#include "WinSync.h"

#include <vector>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <optional>

namespace py = pybind11;

namespace PyAE {

// =============================================================
// BatchOperation - バッチ操作マネージャー
// =============================================================
class BatchOperation {
public:
    static BatchOperation& Instance() {
        static BatchOperation instance;
        return instance;
    }

    // バッチモードの開始
    void Begin() {
        WinLockGuard lock(m_mutex);
        if (m_batchActive) {
            throw std::runtime_error("Batch operation already active");
        }
        m_batchActive = true;
        m_pendingOperations.clear();
        m_startTime = std::chrono::steady_clock::now();
        PYAE_LOG_DEBUG("Batch", "Batch operation started");
    }

    // バッチモードの終了
    void End() {
        WinLockGuard lock(m_mutex);
        if (!m_batchActive) {
            return;
        }

        // 全操作を一括実行
        CommitInternal();

        m_batchActive = false;
        PYAE_LOG_DEBUG("Batch", "Batch operation ended");
    }

    // バッチモードが有効か
    bool IsActive() const {
        WinLockGuard lock(m_mutex);
        return m_batchActive;
    }

    // 操作のキューイング
    void QueueOperation(std::function<void()> operation) {
        WinLockGuard lock(m_mutex);
        if (m_batchActive) {
            m_pendingOperations.push_back(std::move(operation));
        } else {
            // バッチモードでない場合は即座に実行
            operation();
        }
    }

    // コミット（全操作を一括実行）
    void Commit() {
        WinLockGuard lock(m_mutex);
        CommitInternal();
    }

    // ロールバック（操作を破棄）
    void Rollback() {
        WinLockGuard lock(m_mutex);
        m_pendingOperations.clear();
        PYAE_LOG_DEBUG("Batch", "Batch operation rolled back");
    }

    // 保留中の操作数
    size_t GetPendingCount() const {
        WinLockGuard lock(m_mutex);
        return m_pendingOperations.size();
    }

private:
    BatchOperation() = default;

    void CommitInternal() {
        if (m_pendingOperations.empty()) {
            return;
        }

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        // アンドゥグループを開始
        if (suites.utilitySuite) {
            suites.utilitySuite->AEGP_StartUndoGroup("Batch Operation");
        }

        size_t successCount = 0;
        size_t failCount = 0;

        // 全操作を実行
        for (auto& op : m_pendingOperations) {
            try {
                op();
                ++successCount;
            } catch (const std::exception& e) {
                PYAE_LOG_WARNING("Batch", std::string("Operation failed: ") + e.what());
                ++failCount;
            }
        }

        // アンドゥグループを終了
        if (suites.utilitySuite) {
            suites.utilitySuite->AEGP_EndUndoGroup();
        }

        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime);

        PYAE_LOG_INFO("Batch",
            "Committed " + std::to_string(successCount) + " operations in " +
            std::to_string(duration.count()) + "ms (" +
            std::to_string(failCount) + " failed)");

        m_pendingOperations.clear();
    }

    mutable WinMutex m_mutex;
    bool m_batchActive = false;
    std::vector<std::function<void()>> m_pendingOperations;
    std::chrono::steady_clock::time_point m_startTime;
};

// =============================================================
// ScopedBatchOperation - RAIIヘルパー
// =============================================================
class ScopedBatchOperation {
public:
    ScopedBatchOperation() : m_committed(false) {
        BatchOperation::Instance().Begin();
    }

    ~ScopedBatchOperation() {
        if (!m_committed) {
            try {
                BatchOperation::Instance().Commit();
            } catch (...) {
                // デストラクタでは例外を投げない
            }
        }
        BatchOperation::Instance().End();
    }

    void Commit() {
        BatchOperation::Instance().Commit();
        m_committed = true;
    }

    void Rollback() {
        BatchOperation::Instance().Rollback();
        m_committed = true;
    }

    // コピー禁止
    ScopedBatchOperation(const ScopedBatchOperation&) = delete;
    ScopedBatchOperation& operator=(const ScopedBatchOperation&) = delete;

private:
    bool m_committed;
};

// =============================================================
// PropertyCache - プロパティキャッシュ
// =============================================================
template<typename Key, typename Value>
class PropertyCache {
public:
    void Set(const Key& key, const Value& value) {
        WinLockGuard lock(m_mutex);
        m_cache[key] = {value, std::chrono::steady_clock::now()};
    }

    std::optional<Value> Get(const Key& key) const {
        WinLockGuard lock(m_mutex);
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            auto age = std::chrono::steady_clock::now() - it->second.timestamp;
            if (age < m_ttl) {
                return it->second.value;
            }
        }
        return std::nullopt;
    }

    void Invalidate(const Key& key) {
        WinLockGuard lock(m_mutex);
        m_cache.erase(key);
    }

    void Clear() {
        WinLockGuard lock(m_mutex);
        m_cache.clear();
    }

    void SetTTL(std::chrono::milliseconds ttl) {
        m_ttl = ttl;
    }

    size_t Size() const {
        WinLockGuard lock(m_mutex);
        return m_cache.size();
    }

private:
    struct CacheEntry {
        Value value;
        std::chrono::steady_clock::time_point timestamp;
    };

    mutable WinMutex m_mutex;
    std::unordered_map<Key, CacheEntry> m_cache;
    std::chrono::milliseconds m_ttl{100};  // デフォルト100ms
};

// グローバルキャッシュインスタンス
static PropertyCache<std::string, py::object> g_propertyCache;

// =============================================================
// PerformanceStats - パフォーマンス統計
// =============================================================
class PerformanceStats {
public:
    static PerformanceStats& Instance() {
        static PerformanceStats instance;
        return instance;
    }

    void RecordOperation(const std::string& name, std::chrono::microseconds duration) {
        WinLockGuard lock(m_mutex);
        auto& stats = m_stats[name];
        stats.count++;
        stats.totalDuration += duration;
        stats.maxDuration = (std::max)(stats.maxDuration, duration);
        stats.minDuration = (std::min)(stats.minDuration, duration);
    }

    py::dict GetStats() const {
        WinLockGuard lock(m_mutex);
        py::dict result;

        for (const auto& [name, stats] : m_stats) {
            py::dict opStats;
            opStats["count"] = stats.count;
            opStats["total_ms"] = stats.totalDuration.count() / 1000.0;
            opStats["avg_ms"] = (stats.count > 0) ?
                (stats.totalDuration.count() / stats.count / 1000.0) : 0.0;
            opStats["max_ms"] = stats.maxDuration.count() / 1000.0;
            opStats["min_ms"] = (stats.minDuration.count() < LLONG_MAX) ?
                (stats.minDuration.count() / 1000.0) : 0.0;
            result[name.c_str()] = opStats;
        }

        return result;
    }

    void Reset() {
        WinLockGuard lock(m_mutex);
        m_stats.clear();
    }

private:
    PerformanceStats() = default;

    struct OpStats {
        int64_t count = 0;
        std::chrono::microseconds totalDuration{0};
        std::chrono::microseconds maxDuration{0};
        std::chrono::microseconds minDuration{LLONG_MAX};
    };

    mutable WinMutex m_mutex;
    std::unordered_map<std::string, OpStats> m_stats;
};

// =============================================================
// ScopedTimer - 操作の計測ヘルパー
// =============================================================
class ScopedTimer {
public:
    explicit ScopedTimer(const std::string& name)
        : m_name(name)
        , m_start(std::chrono::steady_clock::now())
    {}

    ~ScopedTimer() {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start);
        PerformanceStats::Instance().RecordOperation(m_name, duration);
    }

private:
    std::string m_name;
    std::chrono::steady_clock::time_point m_start;
};

} // namespace PyAE

void init_batch(py::module_& m) {
    // バッチ操作サブモジュール
    py::module_ batch = m.def_submodule("batch", "Batch operations and performance optimization");

    // バッチ操作
    batch.def("begin", []() {
        PyAE::BatchOperation::Instance().Begin();
    }, "Begin a batch operation");

    batch.def("end", []() {
        PyAE::BatchOperation::Instance().End();
    }, "End the current batch operation");

    batch.def("commit", []() {
        PyAE::BatchOperation::Instance().Commit();
    }, "Commit all pending operations");

    batch.def("rollback", []() {
        PyAE::BatchOperation::Instance().Rollback();
    }, "Rollback all pending operations");

    batch.def("is_active", []() {
        return PyAE::BatchOperation::Instance().IsActive();
    }, "Check if batch mode is active");

    batch.def("pending_count", []() {
        return PyAE::BatchOperation::Instance().GetPendingCount();
    }, "Get number of pending operations");

    // ScopedBatchOperation をコンテキストマネージャーとして公開
    py::class_<PyAE::ScopedBatchOperation>(batch, "Operation")
        .def(py::init<>())
        .def("__enter__", [](PyAE::ScopedBatchOperation& self) -> PyAE::ScopedBatchOperation& {
            return self;
        })
        .def("__exit__", [](PyAE::ScopedBatchOperation& self, py::object, py::object, py::object) {
            // デストラクタで自動コミット＆終了
        })
        .def("commit", &PyAE::ScopedBatchOperation::Commit,
             "Commit operations before context exit")
        .def("rollback", &PyAE::ScopedBatchOperation::Rollback,
             "Rollback operations before context exit");

    // キャッシュ操作
    py::module_ cache = m.def_submodule("cache", "Property caching");

    cache.def("clear", []() {
        PyAE::g_propertyCache.Clear();
    }, "Clear the property cache");

    cache.def("size", []() {
        return PyAE::g_propertyCache.Size();
    }, "Get number of cached entries");

    cache.def("set_ttl", [](int milliseconds) {
        PyAE::g_propertyCache.SetTTL(std::chrono::milliseconds(milliseconds));
    }, "Set cache TTL in milliseconds",
    py::arg("milliseconds"));

    // パフォーマンス統計
    py::module_ perf = m.def_submodule("perf", "Performance monitoring");

    perf.def("stats", []() {
        return PyAE::PerformanceStats::Instance().GetStats();
    }, "Get performance statistics");

    perf.def("reset", []() {
        PyAE::PerformanceStats::Instance().Reset();
    }, "Reset performance statistics");

    // ユーティリティ関数
    m.def("batch_operation", []() {
        return std::make_unique<PyAE::ScopedBatchOperation>();
    }, "Create a batch operation context manager");
}
