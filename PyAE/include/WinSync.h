// WinSync.h
// PyAE - Python for After Effects
// Windows同期プリミティブラッパー
// std::mutexの代わりにWindows CRITICAL_SECTIONを使用
// （AE 2024などでC++ランタイム互換性問題を回避するため）

#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

namespace PyAE {

// =============================================================
// WinMutex - Windows CRITICAL_SECTION wrapper
// std::mutex の代替
// =============================================================
class WinMutex {
public:
    WinMutex() {
        InitializeCriticalSection(&m_cs);
    }
    ~WinMutex() {
        DeleteCriticalSection(&m_cs);
    }

    void lock() {
        EnterCriticalSection(&m_cs);
    }

    void unlock() {
        LeaveCriticalSection(&m_cs);
    }

    bool try_lock() {
        return TryEnterCriticalSection(&m_cs) != 0;
    }

    // コピー禁止
    WinMutex(const WinMutex&) = delete;
    WinMutex& operator=(const WinMutex&) = delete;

private:
    CRITICAL_SECTION m_cs;
};

// =============================================================
// WinLockGuard - RAII lock guard for WinMutex
// std::lock_guard の代替
// =============================================================
class WinLockGuard {
public:
    explicit WinLockGuard(WinMutex& mutex) : m_mutex(mutex) {
        m_mutex.lock();
    }
    ~WinLockGuard() {
        m_mutex.unlock();
    }

    // コピー禁止
    WinLockGuard(const WinLockGuard&) = delete;
    WinLockGuard& operator=(const WinLockGuard&) = delete;

private:
    WinMutex& m_mutex;
};

// =============================================================
// WinSharedMutex - Windows SRWLOCK wrapper
// std::shared_mutex の代替（読み書きロック）
// =============================================================
class WinSharedMutex {
public:
    WinSharedMutex() {
        InitializeSRWLock(&m_srw);
    }
    ~WinSharedMutex() {
        // SRWLOCKは明示的な破棄不要
    }

    // 排他ロック（書き込み用）
    void lock() {
        AcquireSRWLockExclusive(&m_srw);
    }

    void unlock() {
        ReleaseSRWLockExclusive(&m_srw);
    }

    bool try_lock() {
        return TryAcquireSRWLockExclusive(&m_srw) != 0;
    }

    // 共有ロック（読み取り用）
    void lock_shared() {
        AcquireSRWLockShared(&m_srw);
    }

    void unlock_shared() {
        ReleaseSRWLockShared(&m_srw);
    }

    bool try_lock_shared() {
        return TryAcquireSRWLockShared(&m_srw) != 0;
    }

    // コピー禁止
    WinSharedMutex(const WinSharedMutex&) = delete;
    WinSharedMutex& operator=(const WinSharedMutex&) = delete;

private:
    SRWLOCK m_srw;
};

// =============================================================
// WinUniqueLock - RAII unique lock for WinSharedMutex
// std::unique_lock の代替
// =============================================================
class WinUniqueLock {
public:
    explicit WinUniqueLock(WinSharedMutex& mutex) : m_mutex(mutex) {
        m_mutex.lock();
    }
    ~WinUniqueLock() {
        m_mutex.unlock();
    }

    // コピー禁止
    WinUniqueLock(const WinUniqueLock&) = delete;
    WinUniqueLock& operator=(const WinUniqueLock&) = delete;

private:
    WinSharedMutex& m_mutex;
};

// =============================================================
// WinSharedLock - RAII shared lock for WinSharedMutex
// std::shared_lock の代替
// =============================================================
class WinSharedLock {
public:
    explicit WinSharedLock(WinSharedMutex& mutex) : m_mutex(mutex) {
        m_mutex.lock_shared();
    }
    ~WinSharedLock() {
        m_mutex.unlock_shared();
    }

    // コピー禁止
    WinSharedLock(const WinSharedLock&) = delete;
    WinSharedLock& operator=(const WinSharedLock&) = delete;

private:
    WinSharedMutex& m_mutex;
};

} // namespace PyAE
