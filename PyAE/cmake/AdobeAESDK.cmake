# cmake/AdobeAESDK.cmake
# After Effects SDK検出モジュール

function(find_aesdk SDK_ROOT)
    if(NOT EXISTS "${SDK_ROOT}")
        message(FATAL_ERROR "AESDK_ROOT does not exist: ${SDK_ROOT}")
    endif()

    # SDK構造の検出（Examples内にある場合とルートにある場合の両方に対応）
    if(EXISTS "${SDK_ROOT}/Examples/Headers")
        set(SDK_EXAMPLES_ROOT "${SDK_ROOT}/Examples")
    elseif(EXISTS "${SDK_ROOT}/Headers")
        set(SDK_EXAMPLES_ROOT "${SDK_ROOT}")
    else()
        message(FATAL_ERROR "Cannot find AE SDK Headers directory in ${SDK_ROOT}")
    endif()

    # 必須ディレクトリの確認
    set(AESDK_HEADERS_DIR "${SDK_EXAMPLES_ROOT}/Headers")
    set(AESDK_HEADERS_SP_DIR "${SDK_EXAMPLES_ROOT}/Headers/SP")
    set(AESDK_UTIL_DIR "${SDK_EXAMPLES_ROOT}/Util")
    set(AESDK_RESOURCES_DIR "${SDK_EXAMPLES_ROOT}/Resources")

    foreach(dir ${AESDK_HEADERS_DIR} ${AESDK_HEADERS_SP_DIR} ${AESDK_UTIL_DIR})
        if(NOT EXISTS "${dir}")
            message(FATAL_ERROR "AE SDK directory not found: ${dir}")
        endif()
    endforeach()

    # インターフェースライブラリとして定義
    add_library(AdobeAESDK INTERFACE)

    target_include_directories(AdobeAESDK INTERFACE
        "${AESDK_HEADERS_DIR}"
        "${AESDK_HEADERS_SP_DIR}"
        "${AESDK_UTIL_DIR}"
    )

    # Windows固有の設定
    if(WIN32)
        target_compile_definitions(AdobeAESDK INTERFACE
            WIN32
            _WINDOWS
            _USRDLL
            MSWindows
        )
    endif()

    # グローバル変数として公開
    set(AESDK_HEADERS_DIR "${AESDK_HEADERS_DIR}" PARENT_SCOPE)
    set(AESDK_UTIL_DIR "${AESDK_UTIL_DIR}" PARENT_SCOPE)
    set(AESDK_RESOURCES_DIR "${AESDK_RESOURCES_DIR}" PARENT_SCOPE)

    message(STATUS "Found Adobe AE SDK: ${SDK_ROOT}")
endfunction()
