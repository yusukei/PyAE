// PyMenuClasses.h
// PyAE - Python for After Effects
// Menu callback bridge for Python integration

#pragma once

#include <functional>
#include <unordered_map>
#include <mutex>
#include <optional>

#include "AE_GeneralPlug.h"

namespace PyAE {

// Forward declaration for pybind11 callback type
using MenuCallback = std::function<void()>;
using UpdateMenuCallback = std::function<bool()>;  // Returns whether command should be enabled

// =============================================================
// MenuCallbackBridge - Bridges C++ menu events to Python callbacks
// =============================================================
class MenuCallbackBridge {
public:
    static MenuCallbackBridge& Instance() {
        static MenuCallbackBridge instance;
        return instance;
    }

    // Register a callback for a command ID
    void RegisterCallback(AEGP_Command cmdId, MenuCallback callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callbacks[cmdId] = std::move(callback);
    }

    // Register an update menu callback for a command ID
    void RegisterUpdateCallback(AEGP_Command cmdId, UpdateMenuCallback callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_updateCallbacks[cmdId] = std::move(callback);
    }

    // Unregister a callback
    void UnregisterCallback(AEGP_Command cmdId) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callbacks.erase(cmdId);
        m_updateCallbacks.erase(cmdId);
    }

    // Clear all callbacks
    void ClearAllCallbacks() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callbacks.clear();
        m_updateCallbacks.clear();
    }

    // Check if a callback is registered for a command
    bool HasCallback(AEGP_Command cmdId) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_callbacks.find(cmdId) != m_callbacks.end();
    }

    // Invoke callback for a command (called from MenuHandler)
    // Returns true if callback was invoked
    bool InvokeCallback(AEGP_Command cmdId);

    // Invoke update menu callback (returns enabled state)
    std::optional<bool> InvokeUpdateCallback(AEGP_Command cmdId);

    // Get all registered command IDs
    std::vector<AEGP_Command> GetRegisteredCommands() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AEGP_Command> result;
        for (const auto& pair : m_callbacks) {
            result.push_back(pair.first);
        }
        return result;
    }

private:
    MenuCallbackBridge() = default;

    mutable std::mutex m_mutex;
    std::unordered_map<AEGP_Command, MenuCallback> m_callbacks;
    std::unordered_map<AEGP_Command, UpdateMenuCallback> m_updateCallbacks;
};

} // namespace PyAE
