# cmake/FindPython310.cmake
# 同梱Python 3.10検出モジュール

function(find_python310 PYTHON_ROOT)
    if(NOT EXISTS "${PYTHON_ROOT}")
        message(FATAL_ERROR "Python310 directory not found: ${PYTHON_ROOT}")
    endif()

    # 必須ファイルの確認
    set(PYTHON_DLL "${PYTHON_ROOT}/python310.dll")
    set(PYTHON_LIB "${PYTHON_ROOT}/libs/python310.lib")
    set(PYTHON_INCLUDE "${PYTHON_ROOT}/include")

    if(NOT EXISTS "${PYTHON_DLL}")
        message(FATAL_ERROR "python310.dll not found: ${PYTHON_DLL}")
    endif()

    if(NOT EXISTS "${PYTHON_LIB}")
        message(FATAL_ERROR "python310.lib not found: ${PYTHON_LIB}")
    endif()

    if(NOT EXISTS "${PYTHON_INCLUDE}")
        message(FATAL_ERROR "Python include directory not found: ${PYTHON_INCLUDE}")
    endif()

    # インポートライブラリとして定義
    add_library(Python310 SHARED IMPORTED GLOBAL)

    set_target_properties(Python310 PROPERTIES
        IMPORTED_LOCATION "${PYTHON_DLL}"
        IMPORTED_IMPLIB "${PYTHON_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE}"
    )

    # pybind11用の変数を設定
    set(PYTHON_EXECUTABLE "${PYTHON_ROOT}/python.exe" PARENT_SCOPE)
    set(PYTHON_INCLUDE_DIRS "${PYTHON_INCLUDE}" PARENT_SCOPE)
    set(PYTHON_LIBRARIES "${PYTHON_LIB}" PARENT_SCOPE)
    set(Python310_ROOT "${PYTHON_ROOT}" PARENT_SCOPE)

    message(STATUS "Found Python 3.10: ${PYTHON_ROOT}")
endfunction()
