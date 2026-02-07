# src/CMakeLists.txt

# ===============================================
# PyAECore.dll - Core functionality with Python/pybind11
# ===============================================

# Core sources (all Python-related code)
set(PYAE_CORE_SOURCES
    Core.cpp
    PluginState.cpp
    SuiteManager.cpp
    PathManager.cpp
    PythonHost.cpp
    TaskQueue.cpp
    IdleHandler.cpp
    ErrorHandling.cpp
    Logger.cpp
    MenuHandler.cpp
    ScriptRunner.cpp
    PanelHandler.cpp
    PanelUI_Win.cpp
    PySidePanelHandler.cpp
    PySidePanelUI_Win.cpp
    PySideLoader.cpp
    PyAEBridgeExports.cpp
    ${AESDK_ROOT}/Util/AEGP_SuiteHandler.cpp
    ${AESDK_ROOT}/Util/MissingSuiteError.cpp
    # Memory diagnostics
    PyBindings/MemoryDiagnostics.cpp
)

# REPL server (optional)
if(PYAE_ENABLE_REPL)
    list(APPEND PYAE_CORE_SOURCES REPLServer.cpp)
endif()

# PyBindings subdirectory
set(PYBINDINGS_SOURCES
    PyBindings/AEModule.cpp
    PyBindings/PyLayer.cpp
    PyBindings/PyLayer_Core.cpp
    PyBindings/PyLayer_Marker.cpp
    PyBindings/PyLayer_Time.cpp
    PyBindings/PyLayer_Flags.cpp
    PyBindings/PyLayer_Relations.cpp
    PyBindings/PyLayer_Transform.cpp
    PyBindings/PyLayer_Effect.cpp
    PyBindings/PyLayer_Mask.cpp
    PyBindings/PyLayer_Matte.cpp
    PyBindings/PyProperty.cpp
    PyBindings/PyComp.cpp
    PyBindings/PyComp_Core.cpp
    PyBindings/PyProject.cpp
    PyBindings/PyItem.cpp
    PyBindings/PyKeyframe.cpp
    # Phase 5: Advanced Features
    PyBindings/PyEffect.cpp
    PyBindings/PyMask.cpp
    PyBindings/PyRenderQueue.cpp
    PyBindings/PyBatch.cpp
    PyBindings/Py3DLayer.cpp
    PyBindings/PyMarker.cpp
    PyBindings/PySDK.cpp
    PyBindings/PyPanelUI.cpp
    # Color Profile (High-level API)
    PyBindings/PyColorProfile.cpp
    # High-level APIs (new)
    PyBindings/PyPersistentData.cpp
    PyBindings/PyMenu.cpp
    PyBindings/PyRenderMonitor.cpp
    PyBindings/PyAsyncRender.cpp
    # World and Footage (High-level API)
    PyBindings/PyWorld.cpp
    PyBindings/PyFootage.cpp
    PyBindings/PyRender.cpp
    PyBindings/PyLayerRenderOptions.cpp
    PyBindings/PySoundData.cpp
    # SDK Suites (Low-level API)
    PyBindings/SDK/Constants.cpp
    PyBindings/SDK/ProjSuite.cpp
    PyBindings/SDK/ItemSuite.cpp
    PyBindings/SDK/ItemViewSuite.cpp
    PyBindings/SDK/CompSuite.cpp
    PyBindings/SDK/LayerSuite.cpp
    PyBindings/SDK/MarkerSuite.cpp
    PyBindings/SDK/CommandSuite.cpp
    PyBindings/SDK/UtilitySuite.cpp
    PyBindings/SDK/StreamValueHelpers.cpp
    PyBindings/SDK/FootageSuite.cpp
    PyBindings/SDK/EffectSuite.cpp
    PyBindings/SDK/MaskSuite.cpp
    PyBindings/SDK/RenderQueueSuite.cpp
    PyBindings/SDK/CollectionSuite.cpp
    PyBindings/SDK/TextLayerSuite.cpp
    PyBindings/SDK/MaskOutlineSuite.cpp
    PyBindings/SDK/RQItemSuite.cpp
    PyBindings/SDK/OutputModuleSuite.cpp
    # SDK APIバージョン問題は2026-02-01に解決済み（plugin_id追加、MemHandle処理実装完了）
    PyBindings/SDK/KeyframeSuite.cpp
    PyBindings/SDK/StreamSuite.cpp
    # Camera & Light
    PyBindings/SDK/CameraSuite.cpp
    # I/O
    PyBindings/SDK/IOInSuite.cpp
    PyBindings/SDK/IOOutSuite.cpp
    # Sound Data
    PyBindings/SDK/SoundDataSuite.cpp
    # Color Settings
    PyBindings/SDK/ColorSettingsSuite.cpp
    # Persistent Data (Preferences)
    PyBindings/SDK/PersistentDataSuite.cpp
    # Math (Matrix operations)
    PyBindings/SDK/MathSuite.cpp
    # Iterate Suite (Multi-threaded iteration)
    PyBindings/SDK/IterateSuite.cpp
    # World Suite (frame buffers)
    PyBindings/SDK/WorldSuite.cpp
    # Composite Suite (image compositing)
    PyBindings/SDK/CompositeSuite.cpp
    # Render Async Manager Suite
    PyBindings/SDK/RenderAsyncManagerSuite.cpp
    # Render Options Suite
    PyBindings/SDK/RenderOptionsSuite.cpp
    # Layer Render Options Suite
    PyBindings/SDK/LayerRenderOptionsSuite.cpp
    # Render Queue Monitor Suite
    PyBindings/SDK/RenderQueueMonitorSuite.cpp
    # Render Suite (frame/sound rendering)
    PyBindings/SDK/RenderSuite.cpp
    # Memory Suite (memory management)
    PyBindings/SDK/MemorySuite.cpp
    # PF Advanced Suites (Refresh/Recalculation)
    PyBindings/SDK/AdvAppSuite.cpp
    PyBindings/SDK/AdvItemSuite.cpp
    # Note: QueryXformSuite excluded - Artisan API only
)

# Headers (for IDE)
set(PYAE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/PluginState.h
    ${CMAKE_SOURCE_DIR}/include/SuiteManager.h
    ${CMAKE_SOURCE_DIR}/include/PathManager.h
    ${CMAKE_SOURCE_DIR}/include/PythonHost.h
    ${CMAKE_SOURCE_DIR}/include/TaskQueue.h
    ${CMAKE_SOURCE_DIR}/include/IdleHandler.h
    ${CMAKE_SOURCE_DIR}/include/ErrorHandling.h
    ${CMAKE_SOURCE_DIR}/include/Logger.h
    ${CMAKE_SOURCE_DIR}/include/ScopedHandles.h
    ${CMAKE_SOURCE_DIR}/include/MenuHandler.h
    ${CMAKE_SOURCE_DIR}/include/ScriptRunner.h
    ${CMAKE_SOURCE_DIR}/include/PanelHandler.h
    ${CMAKE_SOURCE_DIR}/include/PanelUI_Win.h
    ${CMAKE_SOURCE_DIR}/include/PySidePanelHandler.h
    ${CMAKE_SOURCE_DIR}/include/PySidePanelUI_Win.h
    ${CMAKE_SOURCE_DIR}/include/PySideLoader.h
    ${CMAKE_SOURCE_DIR}/include/PySide/IPySidePlugin.h
    ${CMAKE_SOURCE_DIR}/include/MemoryDiagnostics.h
    ${CMAKE_SOURCE_DIR}/include/PyWorldClasses.h
    ${CMAKE_SOURCE_DIR}/include/PyFootageClasses.h
    ${CMAKE_SOURCE_DIR}/include/PyRenderClasses.h
    ${CMAKE_SOURCE_DIR}/include/PyLayerRenderOptionsClasses.h
    ${CMAKE_SOURCE_DIR}/include/PySoundDataClasses.h
)

if(PYAE_ENABLE_REPL)
    list(APPEND PYAE_HEADERS ${CMAKE_SOURCE_DIR}/include/REPLServer.h)
endif()

# Create PyAECore.dll
add_library(PyAECore SHARED
    ${PYAE_CORE_SOURCES}
    ${PYBINDINGS_SOURCES}
    ${PYAE_HEADERS}
)

# Apply version suffix if specified (for multi-SDK builds)
# Example: PYAE_CORE_SUFFIX="_25_6" produces "PyAECore_25_6.dll"
set(PYAE_CORE_OUTPUT_NAME "PyAECore${PYAE_CORE_SUFFIX}")
message(STATUS "PyAECore output name: ${PYAE_CORE_OUTPUT_NAME}.dll")

set_target_properties(PyAECore PROPERTIES
    OUTPUT_NAME "${PYAE_CORE_OUTPUT_NAME}"
    SUFFIX ".dll"
    PREFIX ""
)

target_include_directories(PyAECore PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries for Core (includes Python and pybind11)
target_link_libraries(PyAECore PRIVATE
    AdobeAESDK
    Python310
    pybind11::embed
)

if(WIN32)
    target_link_libraries(PyAECore PRIVATE
        ws2_32      # Winsock (for REPL)
        comdlg32    # Common dialogs
        shell32     # Shell functions
    )
endif()

target_compile_definitions(PyAECore PRIVATE
    PYAE_VERSION="${PROJECT_VERSION}"
    $<$<BOOL:${PYAE_ENABLE_REPL}>:PYAE_ENABLE_REPL>
)

if(MSVC)
    target_compile_options(PyAECore PRIVATE
        /W4
        /WX-
        /utf-8
        /permissive-
        /Zc:__cplusplus
        /MP
    )

    target_compile_options(PyAECore PRIVATE
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/GL>
    )

    target_link_options(PyAECore PRIVATE
        $<$<CONFIG:Release>:/LTCG>
    )
    target_link_options(PyAECore PRIVATE
        $<$<CONFIG:Release>:/LTCG>
    )
endif()

# Precompiled Headers
if(POLICY CMP0105) # Check if CMake supports PCH (3.16+)
    cmake_policy(SET CMP0105 NEW)
    target_precompile_headers(PyAECore PRIVATE "pch.h")
    target_sources(PyAECore PRIVATE "pch.h") # Ensure it shows up in IDE
endif()

# ===============================================
# PyAE.aex - Lightweight loader (no Python dependency)
# ===============================================

set(PYAE_LOADER_SOURCES
    Loader.cpp
    ${AESDK_ROOT}/Util/AEGP_SuiteHandler.cpp
    ${AESDK_ROOT}/Util/MissingSuiteError.cpp
)

add_library(PyAE SHARED
    ${PYAE_LOADER_SOURCES}
)

set_target_properties(PyAE PROPERTIES
    OUTPUT_NAME "PyAE"
    SUFFIX ".aex"
    PREFIX ""

    # Visual Studio debugger settings
    VS_DEBUGGER_COMMAND "$ENV{ProgramFiles}/Adobe/Adobe After Effects ${AE_VERSION}/Support Files/AfterFX.exe"
    VS_DEBUGGER_WORKING_DIRECTORY "$ENV{ProgramFiles}/Adobe/Adobe After Effects ${AE_VERSION}/Support Files"
)

target_include_directories(PyAE PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Loader only links to AE SDK (no Python!)
target_link_libraries(PyAE PRIVATE
    AdobeAESDK
)

if(MSVC)
    target_compile_options(PyAE PRIVATE
        /W4
        /WX-
        /utf-8
        /permissive-
        /Zc:__cplusplus
        /MP
    )

    target_compile_options(PyAE PRIVATE
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/GL>
    )

    target_link_options(PyAE PRIVATE
        $<$<CONFIG:Release>:/LTCG>
    )
endif()

# ===============================================
# PiPL Resource
# ===============================================

if(EXISTS "${CMAKE_SOURCE_DIR}/resources/PyAE.rc")
    target_sources(PyAE PRIVATE "${CMAKE_SOURCE_DIR}/resources/PyAE.rc")
endif()

# ===============================================
# Post-build processing
# ===============================================

# Copy python310.dll to build directory (for debugging)
add_custom_command(TARGET PyAECore POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Python310_ROOT}/python310.dll"
        "$<TARGET_FILE_DIR:PyAECore>"
    COMMENT "Copying python310.dll to build directory"
)

# Copy PyAECore.dll to same directory as PyAE.aex
add_custom_command(TARGET PyAECore POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:PyAECore>"
        "$<TARGET_FILE_DIR:PyAE>"
    COMMENT "Copying PyAECore.dll to PyAE.aex directory"
)

# Development: Copy to AE plugin directory (if AE_PLUGIN_DIR is set)
if(AE_PLUGIN_DIR AND NOT AE_PLUGIN_DIR STREQUAL "")
    add_custom_command(TARGET PyAE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${AE_PLUGIN_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:PyAE>"
            "${AE_PLUGIN_DIR}/"
        COMMENT "Copying PyAE.aex to AE plugin directory"
    )

    add_custom_command(TARGET PyAECore POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:PyAECore>"
            "${AE_PLUGIN_DIR}/"
        COMMENT "Copying PyAECore.dll to AE plugin directory"
    )
else()
    message(STATUS "AE_PLUGIN_DIR not set, skipping post-build copy")
endif()

# ===============================================
# PyAEBridge.dll - ExtendScript ExternalObject Bridge
# ===============================================

add_library(PyAEBridge SHARED
    Bridge/PyAEBridge.cpp
)

set_target_properties(PyAEBridge PROPERTIES
    OUTPUT_NAME "PyAEBridge"
    SUFFIX ".dll"
    PREFIX ""
)

if(MSVC)
    target_compile_options(PyAEBridge PRIVATE
        /W4
        /WX-
        /utf-8
        /permissive-
        /Zc:__cplusplus
        /MP
    )

    target_compile_options(PyAEBridge PRIVATE
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/GL>
    )

    target_link_options(PyAEBridge PRIVATE
        $<$<CONFIG:Release>:/LTCG>
    )
endif()

# Copy PyAEBridge.dll to PyAE.aex directory
add_custom_command(TARGET PyAEBridge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:PyAEBridge>"
        "$<TARGET_FILE_DIR:PyAE>"
    COMMENT "Copying PyAEBridge.dll to PyAE.aex directory"
)

# Copy to AE plugin directory if set
if(AE_PLUGIN_DIR AND NOT AE_PLUGIN_DIR STREQUAL "")
    add_custom_command(TARGET PyAEBridge POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:PyAEBridge>"
            "${AE_PLUGIN_DIR}/"
        COMMENT "Copying PyAEBridge.dll to AE plugin directory"
    )
endif()
