// ScopedHandles.h
// PyAE - Python for After Effects
// RAIIによるAEハンドル管理

#pragma once

// C++標準ライブラリを先にインクルード
#include <utility>

// After Effects SDK
#include "AE_GeneralPlug.h"
#include "AEGP_SuiteHandler.h"

// PyAE
#include "Logger.h"
#include "SDKVersionCompat.h"

namespace PyAE {

// =============================================================
// ScopedMemHandle - AEGP_MemHandle のRAIIラッパー
// =============================================================
class ScopedMemHandle {
public:
    ScopedMemHandle() = default;

    ScopedMemHandle(AEGP_PluginID pluginId, AEGP_MemorySuite1* memorySuite, AEGP_MemHandle handle)
        : m_pluginId(pluginId)
        , m_memorySuite(memorySuite)
        , m_handle(handle)
    {}

    ~ScopedMemHandle() {
        Release();
    }

    // ムーブのみ許可
    ScopedMemHandle(ScopedMemHandle&& other) noexcept
        : m_pluginId(other.m_pluginId)
        , m_memorySuite(other.m_memorySuite)
        , m_handle(std::exchange(other.m_handle, nullptr))
    {}

    ScopedMemHandle& operator=(ScopedMemHandle&& other) noexcept {
        if (this != &other) {
            Release();
            m_pluginId = other.m_pluginId;
            m_memorySuite = other.m_memorySuite;
            m_handle = std::exchange(other.m_handle, nullptr);
        }
        return *this;
    }

    // コピー禁止
    ScopedMemHandle(const ScopedMemHandle&) = delete;
    ScopedMemHandle& operator=(const ScopedMemHandle&) = delete;

    // ハンドル取得
    AEGP_MemHandle Get() const { return m_handle; }
    AEGP_MemHandle* GetPtr() { return &m_handle; }

    // 所有権を手放す（解放せずに返す、呼び出し元が解放責任を持つ）
    AEGP_MemHandle Detach() {
        AEGP_MemHandle handle = m_handle;
        m_handle = nullptr;
        return handle;
    }

    // メモリを解放
    void Release() {
        if (m_handle && m_memorySuite) {
            m_memorySuite->AEGP_FreeMemHandle(m_handle);
        }
        m_handle = nullptr;
    }

    // 注意: Lock/Unlockは例外安全ではないため削除されました。
    // メモリのロックが必要な場合は ScopedMemLock を使用してください。

    // 有効性チェック
    bool IsValid() const { return m_handle != nullptr; }
    explicit operator bool() const { return IsValid(); }

private:
    AEGP_PluginID m_pluginId = 0;
    AEGP_MemorySuite1* m_memorySuite = nullptr;
    AEGP_MemHandle m_handle = nullptr;
};

// =============================================================
// ScopedMemLock - メモリロックのRAIIラッパー
// =============================================================
class ScopedMemLock {
public:
    ScopedMemLock(AEGP_MemorySuite1* memorySuite, AEGP_MemHandle handle)
        : m_memorySuite(memorySuite)
        , m_handle(handle)
    {
        if (m_memorySuite && m_handle) {
            m_memorySuite->AEGP_LockMemHandle(m_handle, &m_ptr);
        }
    }

    ~ScopedMemLock() {
        Unlock();
    }

    // ムーブのみ許可
    ScopedMemLock(ScopedMemLock&& other) noexcept
        : m_memorySuite(other.m_memorySuite)
        , m_handle(other.m_handle)
        , m_ptr(std::exchange(other.m_ptr, nullptr))
    {
        other.m_handle = nullptr;
    }

    ScopedMemLock& operator=(ScopedMemLock&& other) noexcept {
        if (this != &other) {
            Unlock();
            m_memorySuite = other.m_memorySuite;
            m_handle = other.m_handle;
            m_ptr = std::exchange(other.m_ptr, nullptr);
            other.m_handle = nullptr;
        }
        return *this;
    }

    // コピー禁止
    ScopedMemLock(const ScopedMemLock&) = delete;
    ScopedMemLock& operator=(const ScopedMemLock&) = delete;

    void* Get() const { return m_ptr; }

    template<typename T>
    T* As() const { return static_cast<T*>(m_ptr); }

    void Unlock() {
        if (m_ptr && m_memorySuite && m_handle) {
            m_memorySuite->AEGP_UnlockMemHandle(m_handle);
            m_ptr = nullptr;
        }
    }

    bool IsValid() const { return m_ptr != nullptr; }
    explicit operator bool() const { return IsValid(); }

private:
    AEGP_MemorySuite1* m_memorySuite = nullptr;
    AEGP_MemHandle m_handle = nullptr;
    void* m_ptr = nullptr;
};

// =============================================================
// ScopedStreamRef - AEGP_StreamRefH のRAIIラッパー
// =============================================================
class ScopedStreamRef {
public:
    ScopedStreamRef() = default;

    ScopedStreamRef(SDKCompat::StreamSuiteType* streamSuite, AEGP_StreamRefH stream)
        : m_streamSuite(streamSuite)
        , m_stream(stream)
    {}

    ~ScopedStreamRef() {
        Release();
    }

    // ムーブのみ許可
    ScopedStreamRef(ScopedStreamRef&& other) noexcept
        : m_streamSuite(other.m_streamSuite)
        , m_stream(std::exchange(other.m_stream, nullptr))
    {}

    ScopedStreamRef& operator=(ScopedStreamRef&& other) noexcept {
        if (this != &other) {
            Release();
            m_streamSuite = other.m_streamSuite;
            m_stream = std::exchange(other.m_stream, nullptr);
        }
        return *this;
    }

    // コピー禁止
    ScopedStreamRef(const ScopedStreamRef&) = delete;
    ScopedStreamRef& operator=(const ScopedStreamRef&) = delete;

    AEGP_StreamRefH Get() const { return m_stream; }
    AEGP_StreamRefH* GetPtr() { return &m_stream; }

    void Release() {
        if (m_stream && m_streamSuite) {
            m_streamSuite->AEGP_DisposeStream(m_stream);
        }
        m_stream = nullptr;
    }

    bool IsValid() const { return m_stream != nullptr; }
    explicit operator bool() const { return IsValid(); }

private:
    SDKCompat::StreamSuiteType* m_streamSuite = nullptr;
    AEGP_StreamRefH m_stream = nullptr;
};

// =============================================================
// ScopedUndoGroup - アンドゥグループのRAIIラッパー
// =============================================================
class ScopedUndoGroup {
public:
    ScopedUndoGroup(AEGP_UtilitySuite6* utilitySuite,
                   AEGP_PluginID pluginId,
                   const A_char* undoName)
        : m_utilitySuite(utilitySuite)
    {
        if (m_utilitySuite) {
            m_utilitySuite->AEGP_StartUndoGroup(undoName);
            m_started = true;
        }
    }

    ~ScopedUndoGroup() {
        End();
    }

    // ムーブのみ許可
    ScopedUndoGroup(ScopedUndoGroup&& other) noexcept
        : m_utilitySuite(other.m_utilitySuite)
        , m_started(std::exchange(other.m_started, false))
    {}

    ScopedUndoGroup& operator=(ScopedUndoGroup&& other) noexcept {
        if (this != &other) {
            End();
            m_utilitySuite = other.m_utilitySuite;
            m_started = std::exchange(other.m_started, false);
        }
        return *this;
    }

    // コピー禁止
    ScopedUndoGroup(const ScopedUndoGroup&) = delete;
    ScopedUndoGroup& operator=(const ScopedUndoGroup&) = delete;

    void End() {
        if (m_started && m_utilitySuite) {
            m_utilitySuite->AEGP_EndUndoGroup();
            m_started = false;
        }
    }

private:
    AEGP_UtilitySuite6* m_utilitySuite = nullptr;
    bool m_started = false;
};

// =============================================================
// ScopedWorldHandle - AEGP_WorldH のRAIIラッパー
// =============================================================
class ScopedWorldHandle {
public:
    ScopedWorldHandle() = default;

    ScopedWorldHandle(AEGP_WorldSuite3* worldSuite, AEGP_WorldH world)
        : m_worldSuite(worldSuite)
        , m_world(world)
    {}

    ~ScopedWorldHandle() {
        Release();
    }

    // ムーブのみ許可
    ScopedWorldHandle(ScopedWorldHandle&& other) noexcept
        : m_worldSuite(other.m_worldSuite)
        , m_world(std::exchange(other.m_world, nullptr))
    {}

    ScopedWorldHandle& operator=(ScopedWorldHandle&& other) noexcept {
        if (this != &other) {
            Release();
            m_worldSuite = other.m_worldSuite;
            m_world = std::exchange(other.m_world, nullptr);
        }
        return *this;
    }

    // コピー禁止
    ScopedWorldHandle(const ScopedWorldHandle&) = delete;
    ScopedWorldHandle& operator=(const ScopedWorldHandle&) = delete;

    AEGP_WorldH Get() const { return m_world; }
    AEGP_WorldH* GetPtr() { return &m_world; }

    void Release() {
        if (m_world && m_worldSuite) {
            m_worldSuite->AEGP_Dispose(m_world);
        }
        m_world = nullptr;
    }

    bool IsValid() const { return m_world != nullptr; }
    explicit operator bool() const { return IsValid(); }

private:
    AEGP_WorldSuite3* m_worldSuite = nullptr;
    AEGP_WorldH m_world = nullptr;
};

// =============================================================
// ScopedStreamValue - AEGP_StreamValue2 のRAIIラッパー
// =============================================================
class ScopedStreamValue {
public:
    ScopedStreamValue() = default;

    ScopedStreamValue(SDKCompat::StreamSuiteType* streamSuite, AEGP_StreamValue2 value)
        : m_streamSuite(streamSuite)
        , m_value(value)
        , m_initialized(true)
    {}

    ~ScopedStreamValue() {
        Release();
    }

    // ムーブのみ許可
    ScopedStreamValue(ScopedStreamValue&& other) noexcept
        : m_streamSuite(other.m_streamSuite)
        , m_value(other.m_value)
        , m_initialized(std::exchange(other.m_initialized, false))
    {}

    ScopedStreamValue& operator=(ScopedStreamValue&& other) noexcept {
        if (this != &other) {
            Release();
            m_streamSuite = other.m_streamSuite;
            m_value = other.m_value;
            m_initialized = std::exchange(other.m_initialized, false);
        }
        return *this;
    }

    // コピー禁止
    ScopedStreamValue(const ScopedStreamValue&) = delete;
    ScopedStreamValue& operator=(const ScopedStreamValue&) = delete;

    AEGP_StreamValue2& Get() { return m_value; }
    const AEGP_StreamValue2& Get() const { return m_value; }
    AEGP_StreamValue2* GetPtr() { return &m_value; }

    void Release() {
        if (m_initialized && m_streamSuite) {
            m_streamSuite->AEGP_DisposeStreamValue(&m_value);
            m_initialized = false;
        }
    }

    bool IsValid() const { return m_initialized; }
    explicit operator bool() const { return IsValid(); }

private:
    SDKCompat::StreamSuiteType* m_streamSuite = nullptr;
    AEGP_StreamValue2 m_value = {};
    bool m_initialized = false;
};

} // namespace PyAE
