# CMakeLists.txt
# TestEffect - PyAE Custom Property Test Plugin
# A pass-through effect plugin for testing PyAE's effect parameter read/write functionality.

cmake_minimum_required(VERSION 3.21)

project(TestEffect
    VERSION 1.0.0
    DESCRIPTION "Test Effect Plugin for PyAE parameter testing"
    LANGUAGES CXX
)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Windows only
if(NOT WIN32)
    message(FATAL_ERROR "TestEffect currently supports Windows only")
endif()

# 64-bit only
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "TestEffect requires 64-bit build")
endif()

# ===============================================
# SDK Path (Required)
# ===============================================
set(AESDK_PATH "" CACHE PATH "Path to SDK Examples directory (required)")

if(NOT AESDK_PATH OR AESDK_PATH STREQUAL "")
    message(FATAL_ERROR "AESDK_PATH is required. Use -DAESDK_PATH=<path> to specify the SDK Examples directory.")
endif()

if(NOT EXISTS "${AESDK_PATH}")
    message(FATAL_ERROR "SDK path does not exist: ${AESDK_PATH}")
endif()

# SDK structure detection
if(EXISTS "${AESDK_PATH}/Examples/Headers")
    set(AESDK_ROOT "${AESDK_PATH}/Examples")
elseif(EXISTS "${AESDK_PATH}/Headers")
    set(AESDK_ROOT "${AESDK_PATH}")
else()
    message(FATAL_ERROR "Cannot find AE SDK Headers directory in ${AESDK_PATH}")
endif()

# SDK directories
set(AESDK_HEADERS_DIR "${AESDK_ROOT}/Headers")
set(AESDK_UTIL_DIR "${AESDK_ROOT}/Util")

message(STATUS "========================================")
message(STATUS "TestEffect Configuration:")
message(STATUS "  SDK Path: ${AESDK_PATH}")
message(STATUS "  SDK Root: ${AESDK_ROOT}")
message(STATUS "========================================")

# ===============================================
# AE SDK Interface Library
# ===============================================
add_library(AdobeAESDK INTERFACE)

target_include_directories(AdobeAESDK INTERFACE
    "${AESDK_HEADERS_DIR}"
    "${AESDK_HEADERS_DIR}/SP"
    "${AESDK_UTIL_DIR}"
)

target_compile_definitions(AdobeAESDK INTERFACE
    WIN32
    _WINDOWS
    _USRDLL
    MSWindows
)

# ===============================================
# Source files
# ===============================================
set(TESTEFFECT_SOURCES
    TestEffect.cpp
    TestEffect_Strings.cpp
    TestEffect_Arb_Handler.cpp
    # SDK utilities
    ${AESDK_UTIL_DIR}/AEGP_SuiteHandler.cpp
    ${AESDK_UTIL_DIR}/MissingSuiteError.cpp
)

# ===============================================
# TestEffect.aex - Effect Plugin
# ===============================================
add_library(TestEffect SHARED
    ${TESTEFFECT_SOURCES}
)

set_target_properties(TestEffect PROPERTIES
    OUTPUT_NAME "TestEffect"
    SUFFIX ".aex"
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)

# Include directories
target_include_directories(TestEffect PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AESDK_HEADERS_DIR}
    ${AESDK_HEADERS_DIR}/SP
    ${AESDK_HEADERS_DIR}/Win
    ${AESDK_UTIL_DIR}
)

# Link to AE SDK
target_link_libraries(TestEffect PRIVATE
    AdobeAESDK
)

# Windows compile definitions
target_compile_definitions(TestEffect PRIVATE
    MSWindows
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
)

# MSVC compile options
if(MSVC)
    target_compile_options(TestEffect PRIVATE
        /W3
        /WX-
        /utf-8
        /permissive-
        /Zc:__cplusplus
        /MP
        /EHsc
    )
endif()

# ===============================================
# PiPL Resource (via PiPLTool pipeline)
# ===============================================
# SDK standard pipeline: .r -> cl /EP -> .rr -> PiPLtool -> .rrc -> cl /EP -> .rc
find_program(PIPL_TOOL
    NAMES PiPLTool.exe PiPLtool.exe
    PATHS "${AESDK_ROOT}/Resources"
)

if(PIPL_TOOL)
    set(PIPL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/TestEffectPiPL.r")
    set(PIPL_RC "${CMAKE_CURRENT_BINARY_DIR}/TestEffectPiPL.rc")

    add_custom_command(
        OUTPUT "${PIPL_RC}"
        COMMAND "${CMAKE_COMMAND}"
            "-DSDK=${AESDK_HEADERS_DIR}"
            "-DTOOL=${PIPL_TOOL}"
            "-DR=${PIPL_SOURCE}"
            "-DRC=${PIPL_RC}"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/compile_pipl.cmake"
        DEPENDS "${PIPL_SOURCE}" "${CMAKE_CURRENT_SOURCE_DIR}/compile_pipl.cmake"
        COMMENT "Compiling PiPL resource via PiPLTool"
    )

    target_sources(TestEffect PRIVATE "${PIPL_RC}")
    message(STATUS "PiPL: using PiPLTool (${PIPL_TOOL})")
else()
    target_sources(TestEffect PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/TestEffectPiPL.rc"
    )
    message(WARNING "PiPLTool not found - using hand-crafted PiPL resource")
endif()

message(STATUS "TestEffect plugin configured successfully")
