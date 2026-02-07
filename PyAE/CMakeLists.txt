# CMakeLists.txt
# PyAE - Python integration plugin for Adobe After Effects
cmake_minimum_required(VERSION 3.21)

# vcpkg統合（オプション）
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(PyAE
    VERSION 1.0.0
    DESCRIPTION "Python integration plugin for Adobe After Effects"
    LANGUAGES CXX
)

# C++17を要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Windows専用
if(NOT WIN32)
    message(FATAL_ERROR "PyAE currently supports Windows only")
endif()

# 64-bit専用
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "PyAE requires 64-bit build")
endif()

# CMakeモジュールパス
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ===============================================
# 設定オプション
# ===============================================
option(PYAE_BUILD_TESTS "Build unit tests" OFF)
option(PYAE_ENABLE_REPL "Enable REPL server" OFF)
option(PYAE_USE_VCPKG_PYBIND11 "Use vcpkg for pybind11" OFF)

# プロジェクトルート（PyAEの親ディレクトリ）
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

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

# AESDK_ROOT as alias for compatibility
set(AESDK_ROOT "${AESDK_PATH}")

# ===============================================
# Multi-SDK Output Naming
# ===============================================
# PYAE_CORE_SUFFIX: Suffix for PyAECore DLL name (e.g., "_2022", "_25_6")
# This allows building multiple versions of PyAECore for different SDK versions
# Example: -DPYAE_CORE_SUFFIX=_25_6 produces PyAECore_25_6.dll
set(PYAE_CORE_SUFFIX "" CACHE STRING "Suffix for PyAECore output name (e.g., _2022, _2023, _25_2, _25_6)")

# Python310パス
set(Python310_ROOT "" CACHE PATH "Path to embedded Python 3.10")
if(NOT Python310_ROOT OR Python310_ROOT STREQUAL "")
    set(Python310_ROOT "${PROJECT_ROOT}/Python310")
endif()

message(STATUS "========================================")
message(STATUS "SDK Configuration:")
message(STATUS "  SDK Path: ${AESDK_PATH}")
message(STATUS "  Python310: ${Python310_ROOT}")
message(STATUS "========================================")

# ===============================================
# 依存関係
# ===============================================

# AE SDK検出
include(AdobeAESDK)
find_aesdk(${AESDK_ROOT})

# Python 3.10検出
include(FindPython310)
find_python310(${Python310_ROOT})

# pybind11
if(PYAE_USE_VCPKG_PYBIND11)
    find_package(pybind11 CONFIG REQUIRED)
else()
    # FetchContentで取得
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG        v2.11.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# PiPLコンパイラ
include(PiPLCompiler)

# ===============================================
# メインターゲット
# ===============================================
add_subdirectory(src)

# PySide6統合（オプション - Qt6が見つかった場合のみ）
# Qt6パスのヒント（Windowsのデフォルトインストール場所）
if(NOT DEFINED Qt6_DIR AND NOT DEFINED CMAKE_PREFIX_PATH)
    # 一般的なQt6インストールパスを検索
    set(_QT6_SEARCH_PATHS
        "C:/Qt/6.10.2/msvc2022_64/lib/cmake/Qt6"
        "C:/Qt/6.8.0/msvc2022_64/lib/cmake/Qt6"
        "C:/Qt/6.7.0/msvc2022_64/lib/cmake/Qt6"
        "C:/Qt/6.6.0/msvc2022_64/lib/cmake/Qt6"
        "$ENV{QT_DIR}/lib/cmake/Qt6"
    )
    foreach(_path ${_QT6_SEARCH_PATHS})
        if(EXISTS "${_path}/Qt6Config.cmake")
            set(Qt6_DIR "${_path}" CACHE PATH "Qt6 CMake directory")
            message(STATUS "Auto-detected Qt6_DIR: ${Qt6_DIR}")
            break()
        endif()
    endforeach()
endif()

find_package(Qt6 COMPONENTS Core Widgets Gui QUIET)
if(Qt6_FOUND)
    message(STATUS "Qt6 found - building PyAEPySide.dll")
    add_subdirectory(src/PySide)
else()
    message(STATUS "Qt6 not found - PyAEPySide.dll will not be built")
    message(STATUS "To enable PySide6 support, install Qt6 and set Qt6_DIR")
endif()

# ===============================================
# Tools
# ===============================================
# TestRunner is now a standalone project at project root (TestRunner/)
# Build with: build.bat --project TestRunner

# ===============================================
# インストール設定
# ===============================================
include(GNUInstallDirs)

# AEプラグインディレクトリへのインストール
set(AE_PLUGIN_DIR "$ENV{APPDATA}/Adobe/Common/Plug-ins/7.0/MediaCore/PyAE"
    CACHE PATH "After Effects plugin installation directory")

install(TARGETS PyAE
    RUNTIME DESTINATION "${AE_PLUGIN_DIR}"
    LIBRARY DESTINATION "${AE_PLUGIN_DIR}"
)

# Python310ディストリビューションをコピー
install(DIRECTORY "${Python310_ROOT}/"
    DESTINATION "${AE_PLUGIN_DIR}/Python310"
    PATTERN "*.pdb" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    PATTERN "test" EXCLUDE
    PATTERN "idlelib" EXCLUDE
    PATTERN "tkinter" EXCLUDE
)

# サンプルスクリプト
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/scripts/"
    DESTINATION "${AE_PLUGIN_DIR}/scripts"
)

# ===============================================
# パッケージング（CPack）
# ===============================================
set(CPACK_PACKAGE_NAME "PyAE")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "VTechStudio")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Python for After Effects")

# NSIS インストーラ（Windowsの場合）
set(CPACK_GENERATOR "ZIP")

include(CPack)
