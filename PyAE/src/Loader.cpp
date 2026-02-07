// Loader.cpp
// PyAE - Python for After Effects
// Lightweight loader plugin (no Python/pybind11 dependencies)
// This plugin loads PyAECore.dll after setting up DLL search path

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// After Effects SDK
#include "AE_GeneralPlug.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"

// Forward declarations
#ifdef _WIN32
#define PLUGIN_EXPORT __declspec(dllexport)
#else
#define PLUGIN_EXPORT
#endif

extern "C" PLUGIN_EXPORT A_Err EntryPointFunc(SPBasicSuite*, A_long, A_long, AEGP_PluginID, AEGP_GlobalRefcon*);

// Core DLL function types
typedef A_Err (*CoreInitFunc)(SPBasicSuite*, A_long, A_long, AEGP_PluginID, AEGP_GlobalRefcon*);
typedef void (*CoreShutdownFunc)();

// Global state
static HMODULE g_hCoreDll = nullptr;
static CoreInitFunc g_coreInit = nullptr;
static CoreShutdownFunc g_coreShutdown = nullptr;
static wchar_t g_pluginDir[MAX_PATH] = {0};

// Debug output helper
static void DebugOutput(const char* message) {
#ifdef _WIN32
    OutputDebugStringA("[PyAE Loader] ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
#endif
}

static void DebugOutputW(const wchar_t* message) {
#ifdef _WIN32
    OutputDebugStringW(L"[PyAE Loader] ");
    OutputDebugStringW(message);
    OutputDebugStringW(L"\n");
#endif
}

// Get plugin directory from module handle
static bool GetPluginDirectory(wchar_t* outPath, size_t maxLen) {
#ifdef _WIN32
    HMODULE hModule = nullptr;
    if (GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&EntryPointFunc),
        &hModule) == 0) {
        return false;
    }

    if (GetModuleFileNameW(hModule, outPath, static_cast<DWORD>(maxLen)) == 0) {
        return false;
    }

    // Remove filename to get directory
    wchar_t* lastSlash = wcsrchr(outPath, L'\\');
    if (lastSlash) {
        *lastSlash = L'\0';
    }
    return true;
#else
    return false;
#endif
}

// Try to load a specific DLL by name
static HMODULE TryLoadDll(const wchar_t* dllName) {
    wchar_t dllPath[MAX_PATH];
    swprintf_s(dllPath, L"%s\\%s", g_pluginDir, dllName);

    // Check if file exists first
    if (GetFileAttributesW(dllPath) == INVALID_FILE_ATTRIBUTES) {
        return nullptr;
    }

    DebugOutputW(L"Trying to load:");
    DebugOutputW(dllPath);

    HMODULE hDll = LoadLibraryW(dllPath);
    if (hDll != nullptr) {
        char msg[128];
        sprintf_s(msg, "Successfully loaded: %ls", dllName);
        DebugOutput(msg);
    }
    return hDll;
}

// Determine and load appropriate core DLL based on AE version
// DLL naming: PyAECore_2022.dll, PyAECore_2023.dll, PyAECore_25_2.dll, PyAECore_25_6.dll
// Fallback order: version-specific → older versions → generic PyAECore.dll
static bool LoadCoreDll(A_long major_versionL, A_long minor_versionL) {
#ifdef _WIN32
    if (g_hCoreDll != nullptr) {
        return true;  // Already loaded
    }

    char versionMsg[256];
    sprintf_s(versionMsg, "LoadCoreDll called with: major=%ld, minor=%ld", major_versionL, minor_versionL);
    DebugOutput(versionMsg);

    // Build candidate list based on AE version (best match first, then fallbacks)
    // major_versionL format: 122=AE2022, 123=AE2023, 124=AE2024, 125=AE2025
    // Formula: 100 + (year - 2000) = 100 + last 2 digits of year
    const wchar_t* candidates[6] = { nullptr };
    int candidateCount = 0;

    // Convert to year for clearer logic
    A_long aeYear = (major_versionL >= 100) ? (2000 + (major_versionL - 100)) : major_versionL;
    sprintf_s(versionMsg, "Detected AE Year: %ld (from major=%ld)", aeYear, major_versionL);
    DebugOutput(versionMsg);

    if (aeYear >= 2025 || major_versionL >= 125) {
        DebugOutput("Version branch: AE 2025+");
        // AE 2025+: Try SDK 25.6 first (for AE 25.6+), then 25.2
        if (minor_versionL >= 6) {
            DebugOutput("  -> Adding PyAECore_25_6.dll (minor >= 6)");
            candidates[candidateCount++] = L"PyAECore_25_6.dll";
        }
        if (minor_versionL >= 2 || candidateCount == 0) {
            DebugOutput("  -> Adding PyAECore_25_2.dll (minor >= 2 or no 25_6)");
            candidates[candidateCount++] = L"PyAECore_25_2.dll";
        }
        DebugOutput("  -> Adding PyAECore_2023.dll (fallback)");
        candidates[candidateCount++] = L"PyAECore_2023.dll";
        DebugOutput("  -> Adding PyAECore_2022.dll (fallback)");
        candidates[candidateCount++] = L"PyAECore_2022.dll";
    }
    else if (aeYear >= 2023 || major_versionL >= 123) {
        DebugOutput("Version branch: AE 2023/2024");
        // AE 2023/2024: Try SDK 2023 first
        DebugOutput("  -> Adding PyAECore_2023.dll");
        candidates[candidateCount++] = L"PyAECore_2023.dll";
        DebugOutput("  -> Adding PyAECore_2022.dll (fallback)");
        candidates[candidateCount++] = L"PyAECore_2022.dll";
    }
    else if (aeYear >= 2022 || major_versionL >= 122) {
        DebugOutput("Version branch: AE 2022");
        // AE 2022: Try SDK 2022
        candidates[candidateCount++] = L"PyAECore_2022.dll";
    }
    else {
        sprintf_s(versionMsg, "Version branch: Unknown/Old (year=%ld, major=%ld)", aeYear, major_versionL);
        DebugOutput(versionMsg);
    }

    // Always add generic fallback
    DebugOutput("  -> Adding PyAECore.dll (generic fallback)");
    candidates[candidateCount++] = L"PyAECore.dll";

    sprintf_s(versionMsg, "Total DLL candidates: %d", candidateCount);
    DebugOutput(versionMsg);

    // Try each candidate in order
    for (int i = 0; i < candidateCount && candidates[i] != nullptr; i++) {
        g_hCoreDll = TryLoadDll(candidates[i]);
        if (g_hCoreDll != nullptr) {
            break;
        }
    }

    if (g_hCoreDll == nullptr) {
        DWORD error = GetLastError();
        char errorMsg[256];
        sprintf_s(errorMsg, "Failed to load any PyAECore DLL, last error code: %lu", error);
        DebugOutput(errorMsg);
        return false;
    }

    // Get function pointers
    g_coreInit = reinterpret_cast<CoreInitFunc>(
        GetProcAddress(g_hCoreDll, "PyAECore_Init"));
    g_coreShutdown = reinterpret_cast<CoreShutdownFunc>(
        GetProcAddress(g_hCoreDll, "PyAECore_Shutdown"));

    if (g_coreInit == nullptr) {
        DebugOutput("Failed to get PyAECore_Init function");
        FreeLibrary(g_hCoreDll);
        g_hCoreDll = nullptr;
        return false;
    }

    DebugOutput("Core DLL loaded and initialized successfully");
    return true;
#else
    return false;
#endif
}

// Unload core DLL
static void UnloadCoreDll() {
#ifdef _WIN32
    if (g_coreShutdown != nullptr) {
        g_coreShutdown();
    }

    if (g_hCoreDll != nullptr) {
        FreeLibrary(g_hCoreDll);
        g_hCoreDll = nullptr;
    }

    g_coreInit = nullptr;
    g_coreShutdown = nullptr;
#endif
}

// ===============================================
// Entry Point
// ===============================================

extern "C" PLUGIN_EXPORT A_Err EntryPointFunc(
    SPBasicSuite* pica_basicP,
    A_long major_versionL,
    A_long minor_versionL,
    AEGP_PluginID aegp_plugin_id,
    AEGP_GlobalRefcon* global_refconP)
{
    DebugOutput("=== PyAE Loader EntryPointFunc called ===");

    // Log version info immediately
    char versionInfo[256];
    sprintf_s(versionInfo, "AE Version from EntryPoint: major=%ld, minor=%ld", major_versionL, minor_versionL);
    DebugOutput(versionInfo);

#ifdef _WIN32
    // Step 1: Get plugin directory
    if (!GetPluginDirectory(g_pluginDir, MAX_PATH)) {
        DebugOutput("ERROR: Failed to get plugin directory");
        return A_Err_GENERIC;
    }
    DebugOutputW(L"Plugin directory:");
    DebugOutputW(g_pluginDir);

    // Step 2: Set DLL search path to plugin directory
    // This allows python310.dll to be found when PyAECore.dll loads
    if (!SetDllDirectoryW(g_pluginDir)) {
        DebugOutput("ERROR: Failed to set DLL directory");
        return A_Err_GENERIC;
    }
    DebugOutput("DLL search path set to plugin directory");

    // Step 3: Load appropriate PyAECore DLL based on AE version
    if (!LoadCoreDll(major_versionL, minor_versionL)) {
        SetDllDirectoryW(nullptr);  // Restore default
        DebugOutput("ERROR: Failed to load core DLL");
        return A_Err_GENERIC;
    }

    // Step 4: Call core initialization
    A_Err err = A_Err_NONE;
    if (g_coreInit != nullptr) {
        DebugOutput("Calling PyAECore_Init...");
        err = g_coreInit(pica_basicP, major_versionL, minor_versionL, aegp_plugin_id, global_refconP);

        char msg[64];
        sprintf_s(msg, "PyAECore_Init returned: %d", static_cast<int>(err));
        DebugOutput(msg);
    }

    // Step 5: Restore default DLL search path
    SetDllDirectoryW(nullptr);
    DebugOutput("DLL search path restored to default");

    DebugOutput("=== PyAE Loader EntryPointFunc done ===");
    return err;
#else
    return A_Err_GENERIC;
#endif
}

// DLL unload handler
#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_PROCESS_DETACH:
        if (lpReserved == nullptr) {
            // Only cleanup on explicit unload, not process termination
            UnloadCoreDll();
        }
        break;
    }
    return TRUE;
}
#endif
