// Core.cpp
// PyAE - Python for After Effects
// Core DLL with Python/pybind11 functionality
// Loaded by PyAE.aex (Loader) after DLL search path is configured

// C++ standard library (include before AE SDK to avoid extern "C" issues)
#include <filesystem>
#include <string>
#include <memory>

// Windows SDK (define WIN32_LEAN_AND_MEAN before winsock2.h)
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <Windows.h>
#endif

// After Effects SDK
#include "AE_GeneralPlug.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"

// PyAE headers
#include "PluginState.h"
#include "PythonHost.h"
#include "IdleHandler.h"
#include "MenuHandler.h"
#include "PanelHandler.h"
#include "PySidePanelHandler.h"
#include "ScriptRunner.h"
#include "Logger.h"
#include "ErrorHandling.h"
#include "PySideLoader.h"

#ifdef PYAE_ENABLE_REPL
#include "REPLServer.h"
#endif

// Export macros
#ifdef _WIN32
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT
#endif

// Plugin info
static constexpr const char* PLUGIN_NAME = "PyAE";
static constexpr const char* PLUGIN_MATCH_NAME = "com.vtechstudio.pyae";
static constexpr A_long PLUGIN_MAJOR_VERSION = 1;
static constexpr A_long PLUGIN_MINOR_VERSION = 0;

// Global plugin ID
static AEGP_PluginID g_pluginId = 0;

// Forward declaration for address reference
extern "C" CORE_EXPORT A_Err PyAECore_Init(SPBasicSuite*, A_long, A_long, AEGP_PluginID, AEGP_GlobalRefcon*);

// Detect plugin directory
static std::filesystem::path DetectPluginDirectory(SPBasicSuite* basicSuite, AEGP_PluginID pluginId) {
#ifdef _WIN32
    wchar_t modulePath[MAX_PATH];
    HMODULE hModule = nullptr;

    // Get our own module handle
    if (GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&PyAECore_Init),
        &hModule) == 0) {
        return std::filesystem::current_path();
    }

    if (hModule && GetModuleFileNameW(hModule, modulePath, MAX_PATH) > 0) {
        std::filesystem::path path(modulePath);
        return path.parent_path();
    }
#endif

    // Fallback: current directory
    return std::filesystem::current_path();
}

// Plugin initialization
static A_Err PluginInit(
    SPBasicSuite* basicSuite,
    AEGP_PluginID pluginId,
    AEGP_GlobalRefcon* globalRefconP)
{
    A_Err err = A_Err_NONE;

    g_pluginId = pluginId;

    try {
        // Detect plugin directory
        PyAE::DebugOutput("Detecting plugin directory...");
        std::filesystem::path pluginDir = DetectPluginDirectory(basicSuite, pluginId);
        PyAE::DebugOutput("Plugin directory: " + pluginDir.string());

        // Initialize logger
        PyAE::DebugOutput("Initializing Logger...");
        if (!PyAE::Logger::Instance().Initialize(pluginDir)) {
            PyAE::DebugOutput("ERROR: Logger initialization failed!");
            return A_Err_GENERIC;
        }
        PyAE::DebugOutput("Logger initialized OK");

        PYAE_LOG_INFO("Core", "PyAE Core initializing...");
        PYAE_LOG_INFO("Core", "Plugin directory: " + pluginDir.string());

        // Initialize plugin state
        PyAE::DebugOutput("Initializing PluginState...");
        if (!PyAE::PluginState::Instance().Initialize(basicSuite, pluginId)) {
            PyAE::DebugOutput("ERROR: PluginState initialization failed!");
            PYAE_LOG_ERROR("Core", "Failed to initialize PluginState");
            return A_Err_GENERIC;
        }
        PyAE::DebugOutput("PluginState initialized OK");

        // Initialize Python
        PyAE::DebugOutput("Initializing PythonHost...");
        PyAE::PythonConfig pythonConfig;
        // Python home is the plugin directory itself (embedded Python)
        pythonConfig.pythonHome = pluginDir;
        pythonConfig.scriptsDir = pluginDir / "scripts";
        PyAE::DebugOutput("Python home: " + pythonConfig.pythonHome.string());
        PyAE::DebugOutput("Scripts dir: " + pythonConfig.scriptsDir.string());

#ifdef PYAE_ENABLE_REPL
        pythonConfig.enableREPL = true;
        pythonConfig.replPort = 9999;
#endif

        if (!PyAE::PythonHost::Instance().Initialize(pythonConfig)) {
            PyAE::DebugOutput("ERROR: PythonHost initialization failed!");
            PYAE_LOG_ERROR("Core", "Failed to initialize PythonHost");
            return A_Err_GENERIC;
        }
        PyAE::DebugOutput("PythonHost initialized OK");

        // Load PySide plugin (optional - will gracefully fail if not available)
        PyAE::DebugOutput("Loading PySideLoader...");
        if (PyAE::PySideLoader::Instance().LoadPlugin()) {
            PyAE::DebugOutput("PySideLoader loaded OK - PySide6 features enabled");
            PYAE_LOG_INFO("Core", "PySide6 plugin loaded successfully");
        } else {
            PyAE::DebugOutput("PySideLoader not available - PySide6 features disabled");
            PYAE_LOG_INFO("Core", "PySide6 plugin not available - continuing without PySide6 features");
        }

        // Initialize idle handler
        PyAE::DebugOutput("Initializing IdleHandler...");
        if (!PyAE::IdleHandler::Instance().Initialize()) {
            PyAE::DebugOutput("ERROR: IdleHandler initialization failed!");
            PYAE_LOG_ERROR("Core", "Failed to initialize IdleHandler");
            return A_Err_GENERIC;
        }
        PyAE::DebugOutput("IdleHandler initialized OK");

        // Check and queue auto test (if environment variables are set)
        PyAE::DebugOutput("Checking for auto test configuration...");
        PyAE::PluginState::Instance().CheckAndQueueAutoTest();

        // Initialize menu handler
        PyAE::DebugOutput("Initializing MenuHandler...");
        if (!PyAE::MenuHandler::Instance().Initialize()) {
            PyAE::DebugOutput("ERROR: MenuHandler initialization failed!");
            PYAE_LOG_ERROR("Core", "Failed to initialize MenuHandler");
            return A_Err_GENERIC;
        }
        PyAE::DebugOutput("MenuHandler initialized OK");

        // Initialize panel handler
        PyAE::DebugOutput("Initializing PanelHandler...");
        if (!PyAE::PanelHandler::Instance().Initialize()) {
            PyAE::DebugOutput("WARNING: PanelHandler initialization failed (panel UI will be disabled)");
            PYAE_LOG_WARNING("Core", "Failed to initialize PanelHandler - panel UI disabled");
        } else {
            PyAE::DebugOutput("PanelHandler initialized OK");
        }

        // Initialize script runner
        PyAE::DebugOutput("Initializing ScriptRunner...");
        if (!PyAE::ScriptRunner::Instance().Initialize()) {
            PyAE::DebugOutput("ERROR: ScriptRunner initialization failed!");
            PYAE_LOG_ERROR("Core", "Failed to initialize ScriptRunner");
            return A_Err_GENERIC;
        }
        PyAE::DebugOutput("ScriptRunner initialized OK");

        // Get suite handler
        PyAE::DebugOutput("Getting AEGP_SuiteHandler...");
        AEGP_SuiteHandler suites(basicSuite);

        // Register idle hook
        PyAE::DebugOutput("Registering IdleHook...");
        err = suites.RegisterSuite5()->AEGP_RegisterIdleHook(
            pluginId,
            PyAE::PyAE_IdleHook,
            nullptr
        );
        PyAE::DebugOutput("RegisterIdleHook result: " + std::to_string(err));
        PYAE_CHECK_ERR(err, "RegisterIdleHook");

        // Register command hook (hook all commands)
        PyAE::DebugOutput("Registering CommandHook...");
        err = suites.RegisterSuite5()->AEGP_RegisterCommandHook(
            pluginId,
            AEGP_HP_BeforeAE,
            AEGP_Command_ALL,
            PyAE::PyAE_CommandHook,
            nullptr
        );
        PyAE::DebugOutput("RegisterCommandHook result: " + std::to_string(err));
        PYAE_CHECK_ERR(err, "RegisterCommandHook");

        // Register menu update hook
        PyAE::DebugOutput("Registering UpdateMenuHook...");
        err = suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(
            pluginId,
            PyAE::PyAE_UpdateMenuHook,
            nullptr
        );
        PyAE::DebugOutput("RegisterUpdateMenuHook result: " + std::to_string(err));
        PYAE_CHECK_ERR(err, "RegisterUpdateMenuHook");

        // Create menu
        PyAE::DebugOutput("Creating PyAE menu...");
        if (!PyAE::MenuHandler::Instance().CreatePyAEMenu()) {
            PyAE::DebugOutput("WARNING: Failed to create PyAE menu");
            PYAE_LOG_WARNING("Core", "Failed to create PyAE menu");
        } else {
            PyAE::DebugOutput("PyAE menu created successfully");
        }

        // Register PyAE Console panel
        if (PyAE::PanelHandler::Instance().IsPanelSupported()) {
            PyAE::DebugOutput("Registering PyAE Console panel...");
            if (!PyAE::PanelHandler::Instance().RegisterPanel()) {
                PyAE::DebugOutput("WARNING: Failed to register PyAE Console panel");
                PYAE_LOG_WARNING("Core", "Failed to register PyAE Console panel");
            } else {
                PyAE::DebugOutput("PyAE Console panel registered successfully");
            }
        } else {
            PyAE::DebugOutput("Panel not supported in this AE version");
        }

        // Initialize and register PySide panel handler
        PyAE::DebugOutput("Initializing PySidePanelHandler...");
        if (PyAE::PySidePanelHandler::Instance().Initialize()) {
            PyAE::DebugOutput("PySidePanelHandler initialized OK");
            if (PyAE::PySidePanelHandler::Instance().IsPanelSupported()) {
                PyAE::DebugOutput("Registering PyAE Panel (for PySide6)...");
                if (!PyAE::PySidePanelHandler::Instance().RegisterPanel()) {
                    PyAE::DebugOutput("WARNING: Failed to register PyAE Panel");
                    PYAE_LOG_WARNING("Core", "Failed to register PyAE Panel");
                } else {
                    PyAE::DebugOutput("PyAE Panel (for PySide6) registered successfully");
                }
            }
        } else {
            PyAE::DebugOutput("WARNING: PySidePanelHandler initialization failed");
            PYAE_LOG_WARNING("Core", "Failed to initialize PySidePanelHandler");
        }

#ifdef PYAE_ENABLE_REPL
        // Initialize and start REPL server
        PyAE::REPLConfig replConfig;
        replConfig.port = pythonConfig.replPort;
        replConfig.authToken = PyAE::REPLServer::GenerateAuthToken();

        if (PyAE::REPLServer::Instance().Initialize(replConfig)) {
            PyAE::REPLServer::Instance().Start();
            PYAE_LOG_INFO("Core", "REPL server started on port " + std::to_string(replConfig.port));
        }
#endif

        PYAE_LOG_INFO("Core", "PyAE Core initialized successfully");

    } catch (const std::exception& e) {
        PYAE_LOG_FATAL("Core", std::string("Exception during init: ") + e.what());
        return A_Err_GENERIC;
    }

    return err;
}

// Plugin shutdown
static void PluginShutdown() {
    PYAE_LOG_INFO("Core", "PyAE Core shutting down...");

    PyAE::PluginState::Instance().SetShuttingDown(true);

#ifdef PYAE_ENABLE_REPL
    PyAE::REPLServer::Instance().Shutdown();
#endif

    PyAE::PySidePanelHandler::Instance().Shutdown();
    PyAE::PanelHandler::Instance().Shutdown();
    PyAE::ScriptRunner::Instance().Shutdown();
    PyAE::MenuHandler::Instance().Shutdown();
    PyAE::IdleHandler::Instance().Shutdown();

    // Unload PySide plugin before Python shutdown
    PyAE::PySideLoader::Instance().UnloadPlugin();

    PyAE::PythonHost::Instance().Shutdown();
    PyAE::PluginState::Instance().Shutdown();

    PYAE_LOG_INFO("Core", "PyAE Core shutdown complete");

    PyAE::Logger::Instance().Shutdown();
}

// ===============================================
// Exported Functions
// ===============================================

extern "C" CORE_EXPORT A_Err PyAECore_Init(
    SPBasicSuite* pica_basicP,
    A_long major_versionL,
    A_long minor_versionL,
    AEGP_PluginID aegp_plugin_id,
    AEGP_GlobalRefcon* global_refconP)
{
    PyAE::DebugOutput("=== PyAECore_Init called ===");
    PyAE::DebugOutput("AE SDK version: " + std::to_string(major_versionL) + "." + std::to_string(minor_versionL));
    PyAE::DebugOutput("Plugin ID: " + std::to_string(aegp_plugin_id));

    A_Err err = A_Err_NONE;
    g_pluginId = aegp_plugin_id;

    try {
        PyAE::DebugOutput("Calling PluginInit...");
        err = PluginInit(pica_basicP, aegp_plugin_id, global_refconP);
        PyAE::DebugOutput("PluginInit returned: " + std::to_string(err));
    } catch (const std::exception& e) {
        PyAE::DebugOutput("Exception in PluginInit: " + std::string(e.what()));
        if (PyAE::Logger::Instance().GetLogPath().empty() == false) {
            PYAE_LOG_FATAL("Core", std::string("Unhandled exception: ") + e.what());
        }
        err = A_Err_GENERIC;
    }

    PyAE::DebugOutput("=== PyAECore_Init done ===");
    return err;
}

extern "C" CORE_EXPORT void PyAECore_Shutdown() {
    PyAE::DebugOutput("=== PyAECore_Shutdown called ===");
    PluginShutdown();
    PyAE::DebugOutput("=== PyAECore_Shutdown done ===");
}
