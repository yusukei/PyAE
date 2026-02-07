// PyAEBridge.cpp
// ExtendScript ExternalObject Bridge for PyAE
// This is a separate DLL that communicates with PyAECore.dll

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdio>
#include <cstdint>
#include <cstddef>

// Debug logging to file and OutputDebugString
static void DebugLog(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    OutputDebugStringA("[PyAEBridge] ");
    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");

    // Also write to file for easier debugging
    FILE* f = fopen("C:\\Program Files\\Adobe\\Common\\Plug-ins\\7.0\\MediaCore\\PyAE\\logs\\bridge_debug.log", "a");
    if (f) {
        fprintf(f, "[PyAEBridge] %s\n", buffer);
        fclose(f);
    }
}

// =============================================
// ExtendScript ExternalObject Interface
// =============================================

#ifdef _WIN32
#define ES_EXPORT __declspec(dllexport)
#else
#define ES_EXPORT __attribute__((visibility("default")))
#endif

// ExtendScript data types
// Based on Adobe ExtendScript SDK SoSharedLibDefs.h
// IMPORTANT: ExtendScript uses 32-bit pointers even on 64-bit Windows!
// Structure layout: data union (8 bytes) + type (4 bytes) + filler (4 bytes) = 16 bytes

// ExtendScript TaggedData structure
// Based on Adobe SDK documentation
// Using 4-byte packing to match ExtendScript's 32-bit origins
#pragma pack(push, 4)
typedef struct TaggedData_s {
    union {
        long intval;              // Integer value (4 bytes)
        double fltval;            // Floating point value (8 bytes)
        char* string;             // String pointer (8 bytes on 64-bit)
        struct TaggedData_s* hObject;  // Object handle (8 bytes on 64-bit)
    } data;                       // 8 bytes (aligned to 8 for double)
    long type;                    // Data type (4 bytes)
    long filler;                  // Padding (4 bytes)
} TaggedData;
#pragma pack(pop)

enum ESDataType {
    kTypeUndefined = 0,
    kTypeBool      = 1,
    kTypeDouble    = 2,
    kTypeString    = 3,
    kTypeScript    = 4,
    kTypeInteger   = 5,
    kTypeUInteger  = 6
};

enum ESErrorCode {
    kESErrOK              = 0,
    kESErrException       = 1,
    kESErrBadArgumentList = 5
};

// =============================================
// PyAECore.dll Interface (via GetProcAddress)
// =============================================

// Function pointer types for PyAECore.dll exports
typedef bool (*PyAECore_IsReadyFunc)();
typedef bool (*PyAECore_ExecuteStringFunc)(const char* code, char* errorBuf, size_t errorBufSize);
typedef bool (*PyAECore_ExecuteFileFunc)(const char* path, char* errorBuf, size_t errorBufSize);

// Global state
static HMODULE g_pyaeCoreDll = nullptr;
static PyAECore_IsReadyFunc g_isReadyFunc = nullptr;
static PyAECore_ExecuteStringFunc g_executeStringFunc = nullptr;
static PyAECore_ExecuteFileFunc g_executeFileFunc = nullptr;
static bool g_initialized = false;

// =============================================
// Helper Functions
// =============================================

// Store this DLL's module handle
static HMODULE g_thisDllModule = nullptr;

namespace {
    char* DuplicateString(const char* str) {
        if (!str) return nullptr;
        size_t len = strlen(str) + 1;
        char* result = static_cast<char*>(malloc(len));
        if (result) {
            memcpy(result, str, len);
        }
        return result;
    }

    char* DuplicateString(const std::string& str) {
        return DuplicateString(str.c_str());
    }

    // Find PyAECore.dll in the same directory as this DLL
    std::wstring GetPyAECorePath() {
        wchar_t modulePath[MAX_PATH];

        if (g_thisDllModule && GetModuleFileNameW(g_thisDllModule, modulePath, MAX_PATH)) {
            std::wstring path(modulePath);
            size_t lastSlash = path.find_last_of(L"\\/");
            if (lastSlash != std::wstring::npos) {
                return path.substr(0, lastSlash + 1) + L"PyAECore.dll";
            }
        }

        return L"PyAECore.dll";
    }

    bool InitializePyAECore() {
        DebugLog("InitializePyAECore called, g_initialized=%d", g_initialized);
        if (g_initialized) return true;

        // Try to get handle to already-loaded PyAECore.dll
        DebugLog("Trying GetModuleHandleW for PyAECore.dll...");
        g_pyaeCoreDll = GetModuleHandleW(L"PyAECore.dll");
        DebugLog("GetModuleHandleW returned: %p", g_pyaeCoreDll);

        if (!g_pyaeCoreDll) {
            // Not loaded yet, try to load it
            std::wstring dllPath = GetPyAECorePath();
            DebugLog("PyAECore.dll not loaded, trying LoadLibraryW...");
            g_pyaeCoreDll = LoadLibraryW(dllPath.c_str());
            DebugLog("LoadLibraryW returned: %p", g_pyaeCoreDll);
        }

        if (!g_pyaeCoreDll) {
            DebugLog("ERROR: Failed to get PyAECore.dll handle");
            return false;
        }

        // Get function pointers
        DebugLog("Getting function pointers from PyAECore.dll...");
        g_isReadyFunc = reinterpret_cast<PyAECore_IsReadyFunc>(
            GetProcAddress(g_pyaeCoreDll, "PyAEBridge_IsReady"));
        DebugLog("PyAEBridge_IsReady: %p", g_isReadyFunc);

        g_executeStringFunc = reinterpret_cast<PyAECore_ExecuteStringFunc>(
            GetProcAddress(g_pyaeCoreDll, "PyAEBridge_ExecuteString"));
        DebugLog("PyAEBridge_ExecuteString: %p", g_executeStringFunc);

        g_executeFileFunc = reinterpret_cast<PyAECore_ExecuteFileFunc>(
            GetProcAddress(g_pyaeCoreDll, "PyAEBridge_ExecuteFile"));
        DebugLog("PyAEBridge_ExecuteFile: %p", g_executeFileFunc);

        g_initialized = (g_isReadyFunc != nullptr);
        DebugLog("InitializePyAECore completed, g_initialized=%d", g_initialized);
        return g_initialized;
    }
}

// =============================================
// ExtendScript Entry Points
// =============================================

extern "C" ES_EXPORT char* ESInitialize(TaggedData* argv, long argc) {
    DebugLog("ESInitialize called - argv=%p, argc=%ld", argv, argc);

    (void)argv;
    (void)argc;

    // Don't initialize PyAECore here - wait until actually called
    // This avoids potential issues during DLL load

    // Return function signatures
    // Format: "functionName_argTypes,..." (no suffix for no-argument functions)
    // Argument type suffixes: _s = string, _d = double, _b = bool
    // Functions with no arguments have no underscore suffix
    const char* funcs = "isPyAEReady,evalPython_s,runPythonFile_s,debugLog_s";
    DebugLog("ESInitialize returning: %s", funcs);
    return DuplicateString(funcs);
}

extern "C" ES_EXPORT long ESGetVersion(void) {
    DebugLog("ESGetVersion called, returning 1");
    return 1;
}

extern "C" ES_EXPORT void ESFreeMem(void* p) {
    DebugLog("ESFreeMem called - p=%p", p);
    if (p) {
        free(p);
    }
}

extern "C" ES_EXPORT void ESTerminate(void) {
    DebugLog("ESTerminate called");
    // Don't unload PyAECore.dll - it's managed by PyAE.aex
    g_pyaeCoreDll = nullptr;
    g_isReadyFunc = nullptr;
    g_executeStringFunc = nullptr;
    g_executeFileFunc = nullptr;
    g_initialized = false;
    DebugLog("ESTerminate completed");
}

// =============================================
// Custom Functions
// =============================================

extern "C" ES_EXPORT long isPyAEReady(TaggedData* argv, long argc, TaggedData* retval) {
    DebugLog("isPyAEReady called - argv=%p, argc=%ld, retval=%p", argv, argc, retval);

    (void)argv;
    (void)argc;

    bool ready = false;

    DebugLog("isPyAEReady: calling InitializePyAECore...");
    if (InitializePyAECore() && g_isReadyFunc) {
        DebugLog("isPyAEReady: calling g_isReadyFunc...");
        ready = g_isReadyFunc();
        DebugLog("isPyAEReady: g_isReadyFunc returned %d", ready);
    } else {
        DebugLog("isPyAEReady: InitializePyAECore failed or g_isReadyFunc is null");
    }

    // Debug: Log structure layout
    DebugLog("isPyAEReady: sizeof(TaggedData)=%zu, offsetof(data)=%zu, offsetof(type)=%zu, offsetof(filler)=%zu",
             sizeof(TaggedData), offsetof(TaggedData, data), offsetof(TaggedData, type), offsetof(TaggedData, filler));
    DebugLog("isPyAEReady: retval addr=%p, &retval->data=%p, &retval->type=%p, &retval->data.string=%p",
             retval, &retval->data, &retval->type, &retval->data.string);

    // Use kTypeScript - ExtendScript evaluates the returned string as JavaScript
    // This worked in previous session
    retval->type = kTypeScript;
    retval->data.string = DuplicateString(ready ? "true" : "false");
    retval->filler = 0;

    DebugLog("isPyAEReady: retval set - type=%d (kTypeScript), string=%s (ptr=%p)", retval->type, retval->data.string, retval->data.string);
    DebugLog("isPyAEReady returning kESErrOK");
    return kESErrOK;
}

extern "C" ES_EXPORT long evalPython(TaggedData* argv, long argc, TaggedData* retval) {
    DebugLog("evalPython called - argv=%p, argc=%ld, retval=%p", argv, argc, retval);

    // Argument check
    if (argc < 1) {
        DebugLog("evalPython: argc < 1, returning error");
        retval->type = kTypeScript;
        retval->data.string = DuplicateString("'Error: evalPython requires a string argument'");
        return kESErrBadArgumentList;
    }

    DebugLog("evalPython: argv[0].type=%d", argv[0].type);

    // Accept both kTypeString (3) and kTypeScript (4) as valid string arguments
    if ((argv[0].type != kTypeString && argv[0].type != kTypeScript) || argv[0].data.string == nullptr) {
        DebugLog("evalPython: argv[0] is not a string (type=%d), returning error", argv[0].type);
        retval->type = kTypeScript;
        retval->data.string = DuplicateString("'Error: evalPython requires a string argument'");
        retval->filler = 0;
        return kESErrBadArgumentList;
    }

    // Initialize if needed
    if (!InitializePyAECore() || !g_executeStringFunc) {
        DebugLog("evalPython: PyAECore not available");
        retval->type = kTypeScript;
        retval->data.string = DuplicateString("'Error: PyAECore.dll not available'");
        return kESErrException;
    }

    const char* code = argv[0].data.string;
    DebugLog("evalPython: executing code: %s", code);

    char errorBuf[4096] = {0};
    bool success = g_executeStringFunc(code, errorBuf, sizeof(errorBuf));

    DebugLog("evalPython: success=%d, errorBuf=%s", success, errorBuf);

    // Use kTypeScript - return JavaScript code that evaluates to a string
    retval->type = kTypeScript;
    if (success) {
        retval->data.string = DuplicateString("'OK'");
    } else {
        // Escape single quotes and special chars in error message
        std::string escaped = "'";
        const char* err = errorBuf[0] ? errorBuf : "Unknown error";
        for (const char* p = err; *p; ++p) {
            if (*p == '\'') escaped += "\\'";
            else if (*p == '\\') escaped += "\\\\";
            else if (*p == '\n') escaped += "\\n";
            else if (*p == '\r') escaped += "\\r";
            else escaped += *p;
        }
        escaped += "'";
        retval->data.string = DuplicateString(escaped);
        DebugLog("evalPython: returning error string: %s", retval->data.string);
    }
    retval->filler = 0;

    DebugLog("evalPython returning %s", success ? "kESErrOK" : "kESErrException");
    return success ? kESErrOK : kESErrException;
}

extern "C" ES_EXPORT long runPythonFile(TaggedData* argv, long argc, TaggedData* retval) {
    DebugLog("runPythonFile called - argv=%p, argc=%ld, retval=%p", argv, argc, retval);

    // Argument check - accept both kTypeString (3) and kTypeScript (4)
    if (argc < 1 || (argv[0].type != kTypeString && argv[0].type != kTypeScript) || argv[0].data.string == nullptr) {
        DebugLog("runPythonFile: invalid argument (type=%d)", argc > 0 ? argv[0].type : -1);
        retval->type = kTypeScript;
        retval->data.string = DuplicateString("'Error: runPythonFile requires a file path argument'");
        retval->filler = 0;
        return kESErrBadArgumentList;
    }

    // Initialize if needed
    if (!InitializePyAECore() || !g_executeFileFunc) {
        DebugLog("runPythonFile: PyAECore not available");
        retval->type = kTypeScript;
        retval->data.string = DuplicateString("'Error: PyAECore.dll not available'");
        return kESErrException;
    }

    const char* filePath = argv[0].data.string;
    DebugLog("runPythonFile: executing file: %s", filePath);

    char errorBuf[4096] = {0};
    bool success = g_executeFileFunc(filePath, errorBuf, sizeof(errorBuf));

    DebugLog("runPythonFile: success=%d, errorBuf=%s", success, errorBuf);

    // Use kTypeScript - return JavaScript code that evaluates to a string
    retval->type = kTypeScript;
    if (success) {
        retval->data.string = DuplicateString("'OK'");
    } else {
        // Escape single quotes and special chars in error message
        std::string escaped = "'";
        const char* err = errorBuf[0] ? errorBuf : "Unknown error";
        for (const char* p = err; *p; ++p) {
            if (*p == '\'') escaped += "\\'";
            else if (*p == '\\') escaped += "\\\\";
            else if (*p == '\n') escaped += "\\n";
            else if (*p == '\r') escaped += "\\r";
            else escaped += *p;
        }
        escaped += "'";
        retval->data.string = DuplicateString(escaped);
        DebugLog("runPythonFile: returning error string: %s", retval->data.string);
    }
    retval->filler = 0;

    DebugLog("runPythonFile returning %s", success ? "kESErrOK" : "kESErrException");
    return success ? kESErrOK : kESErrException;
}

extern "C" ES_EXPORT long debugLog(TaggedData* argv, long argc, TaggedData* retval) {
    // Argument check
    if (argc < 1 || (argv[0].type != kTypeString && argv[0].type != kTypeScript) || argv[0].data.string == nullptr) {
        retval->type = kTypeScript;
        retval->data.string = DuplicateString("'Error: debugLog requires a string argument'");
        retval->filler = 0;
        return kESErrBadArgumentList;
    }

    const char* message = argv[0].data.string;

    // Output to OutputDebugString with [ExtendScript] prefix
    OutputDebugStringA("[ExtendScript] ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");

    // Also write to file for easier debugging
    FILE* f = fopen("C:\\Program Files\\Adobe\\Common\\Plug-ins\\7.0\\MediaCore\\PyAE\\logs\\extendscript_debug.log", "a");
    if (f) {
        fprintf(f, "[ExtendScript] %s\n", message);
        fclose(f);
    }

    // Return success
    retval->type = kTypeScript;
    retval->data.string = DuplicateString("'OK'");
    retval->filler = 0;
    return kESErrOK;
}

// =============================================
// DLL Entry Point
// =============================================

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    (void)lpReserved;

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        g_thisDllModule = hModule;
        DebugLog("DLL_PROCESS_ATTACH - Module handle: %p", hModule);
        break;
    case DLL_PROCESS_DETACH:
        DebugLog("DLL_PROCESS_DETACH");
        g_thisDllModule = nullptr;
        break;
    }
    return TRUE;
}
#endif
