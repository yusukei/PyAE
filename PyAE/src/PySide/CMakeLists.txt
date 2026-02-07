# src/PySide/CMakeLists.txt
# PyAEPySide.dll - PySide6統合DLL

cmake_minimum_required(VERSION 3.16)

# ===============================================
# Qt6を検索
# ===============================================

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 is required for PyAEPySide.dll but not found")
endif()

message(STATUS "Building PyAEPySide.dll with Qt6")
message(STATUS "Qt6_DIR: ${Qt6_DIR}")
message(STATUS "Qt6 Core version: ${Qt6Core_VERSION}")

# ===============================================
# ソースファイル
# ===============================================

set(PYAEPYSIDE_SOURCES
    QtIntegration.cpp
    PanelUI_PySide.cpp
    PySidePlugin.cpp
)

set(PYAEPYSIDE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/PySide/IPySidePlugin.h
    ${CMAKE_SOURCE_DIR}/include/PySide/QtIntegration.h
    ${CMAKE_SOURCE_DIR}/include/PySide/PanelUI_PySide.h
    PySidePlugin.h
)

# ===============================================
# PyAEPySide.dll ターゲット
# ===============================================

add_library(PyAEPySide SHARED
    ${PYAEPYSIDE_SOURCES}
    ${PYAEPYSIDE_HEADERS}
)

set_target_properties(PyAEPySide PROPERTIES
    OUTPUT_NAME "PyAEPySide"
    SUFFIX ".dll"
    PREFIX ""
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# インクルードディレクトリ
target_include_directories(PyAEPySide PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/PySide
)

# Qt6ライブラリをリンク
target_link_libraries(PyAEPySide PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

# コンパイル定義
target_compile_definitions(PyAEPySide PRIVATE
    PYAE_PYSIDE_DLL_BUILD
)

# MSVC設定
if(MSVC)
    target_compile_options(PyAEPySide PRIVATE
        /W4          # 警告レベル4
        /WX-         # 警告をエラーとして扱わない
        /utf-8       # UTF-8エンコーディング
        /permissive- # 標準準拠モード
        /Zc:__cplusplus # __cplusplusマクロを正しく設定
        /MP          # マルチプロセスコンパイル
    )

    target_compile_options(PyAEPySide PRIVATE
        $<$<CONFIG:Release>:/O2>  # 最適化
        $<$<CONFIG:Release>:/GL>  # プログラム全体の最適化
    )

    target_link_options(PyAEPySide PRIVATE
        $<$<CONFIG:Release>:/LTCG>  # リンク時コード生成
    )
endif()

# ===============================================
# Post-build処理
# ===============================================

# PyAEPySide.dllをPyAECore.dllと同じディレクトリにコピー
add_custom_command(TARGET PyAEPySide POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:PyAEPySide>"
        "$<TARGET_FILE_DIR:PyAECore>"
    COMMENT "Copying PyAEPySide.dll to PyAECore directory"
)

# AE_PLUGIN_DIRが設定されている場合、そこにもコピー
if(AE_PLUGIN_DIR AND NOT AE_PLUGIN_DIR STREQUAL "")
    add_custom_command(TARGET PyAEPySide POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:PyAEPySide>"
            "${AE_PLUGIN_DIR}/"
        COMMENT "Copying PyAEPySide.dll to AE plugin directory"
    )
endif()

# Qt6 DLLもコピー（デバッグ用）
if(WIN32)
    # Qt6Core.dll, Qt6Widgets.dll, Qt6Gui.dll
    get_target_property(QT6_CORE_DLL Qt6::Core LOCATION)
    get_target_property(QT6_WIDGETS_DLL Qt6::Widgets LOCATION)
    get_target_property(QT6_GUI_DLL Qt6::Gui LOCATION)

    add_custom_command(TARGET PyAEPySide POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT6_CORE_DLL}"
            "$<TARGET_FILE_DIR:PyAECore>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT6_WIDGETS_DLL}"
            "$<TARGET_FILE_DIR:PyAECore>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT6_GUI_DLL}"
            "$<TARGET_FILE_DIR:PyAECore>"
        COMMENT "Copying Qt6 DLLs to build directory"
    )
endif()

message(STATUS "PyAEPySide.dll configuration complete")
