// PyAEBridgeExports.cpp
// Export functions for PyAEBridge.dll to call
// These are C-style exports that PyAEBridge.dll can call via GetProcAddress

#include "PythonHost.h"
#include "PluginState.h"
#include "Logger.h"

#include <cstring>

#ifdef _WIN32
#define BRIDGE_EXPORT extern "C" __declspec(dllexport)
#else
#define BRIDGE_EXPORT extern "C" __attribute__((visibility("default")))
#endif

BRIDGE_EXPORT bool PyAEBridge_IsReady() {
    return PyAE::PluginState::Instance().IsInitialized() &&
           PyAE::PythonHost::Instance().IsInitialized();
}

BRIDGE_EXPORT bool PyAEBridge_ExecuteString(const char* code, char* errorBuf, size_t errorBufSize) {
    if (!code || !errorBuf || errorBufSize == 0) {
        return false;
    }

    errorBuf[0] = '\0';

    if (!PyAE::PluginState::Instance().IsInitialized()) {
        strncpy(errorBuf, "PyAE is not initialized", errorBufSize - 1);
        errorBuf[errorBufSize - 1] = '\0';
        return false;
    }

    PYAE_LOG_INFO("PyAEBridge", "ExecuteString called");

    std::string errorOut;
    bool success = PyAE::PythonHost::Instance().ExecuteString(code, errorOut);

    if (!success && !errorOut.empty()) {
        strncpy(errorBuf, errorOut.c_str(), errorBufSize - 1);
        errorBuf[errorBufSize - 1] = '\0';
    }

    return success;
}

BRIDGE_EXPORT bool PyAEBridge_ExecuteFile(const char* filePath, char* errorBuf, size_t errorBufSize) {
    if (!filePath || !errorBuf || errorBufSize == 0) {
        return false;
    }

    errorBuf[0] = '\0';

    if (!PyAE::PluginState::Instance().IsInitialized()) {
        strncpy(errorBuf, "PyAE is not initialized", errorBufSize - 1);
        errorBuf[errorBufSize - 1] = '\0';
        return false;
    }

    PYAE_LOG_INFO("PyAEBridge", "ExecuteFile called");

    std::string errorOut;
    bool success = PyAE::PythonHost::Instance().ExecuteFile(
        std::filesystem::path(filePath), errorOut);

    if (!success && !errorOut.empty()) {
        strncpy(errorBuf, errorOut.c_str(), errorBufSize - 1);
        errorBuf[errorBufSize - 1] = '\0';
    }

    return success;
}
