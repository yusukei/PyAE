# cmake/PiPLCompiler.cmake
# PiPLリソースコンパイル
#
# 重要: PiPLToolはCMakeとの統合が困難なため、
# プリコンパイル済みリソースの使用を推奨します。

# PiPLToolの検出
function(find_pipl_tool)
    # SDKに含まれるPiPLToolを探す
    find_program(PIPL_TOOL
        NAMES PiPLTool.exe PiPLtool.exe
        PATHS
            "${AESDK_ROOT}/Resources"
            "${AESDK_ROOT}/Tools"
            "$ENV{AESDK_ROOT}/Resources"
        DOC "Adobe PiPL compiler tool"
    )

    if(NOT PIPL_TOOL)
        message(WARNING "PiPLTool not found. Will use pre-compiled resources.")
    else()
        message(STATUS "Found PiPLTool: ${PIPL_TOOL}")
    endif()

    set(PIPL_TOOL "${PIPL_TOOL}" PARENT_SCOPE)
endfunction()

# 推奨方法: プリコンパイル済みPiPLリソースを使用
function(add_precompiled_pipl TARGET RES_FILE)
    if(WIN32)
        if(NOT EXISTS "${RES_FILE}")
            message(FATAL_ERROR "Pre-compiled PiPL resource not found: ${RES_FILE}")
        endif()

        # .resファイルを直接リンク
        target_link_options(${TARGET} PRIVATE "${RES_FILE}")
        message(STATUS "Using pre-compiled PiPL: ${RES_FILE}")
    endif()
endfunction()

# 簡易版: Windowsリソースファイルを直接使用
function(add_pipl_resource TARGET RC_FILE)
    if(WIN32)
        target_sources(${TARGET} PRIVATE "${RC_FILE}")
    endif()
endfunction()
