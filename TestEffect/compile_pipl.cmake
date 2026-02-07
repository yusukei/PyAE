# compile_pipl.cmake
# PiPL resource compilation script (3-step pipeline)
#
# Usage: cmake -DSDK=<headers_dir> -DTOOL=<pipltool>
#              -DR=<input.r> -DRC=<output.rc> -DWORK=<build_dir>
#              -P compile_pipl.cmake
#
# Pipeline: .r -> cl /EP -> .rr -> PiPLtool -> .rrc -> cl /D MSWindows /EP -> .rc
# Note: cl.exe is found via PATH (VS environment must be active)

cmake_minimum_required(VERSION 3.21)

# Validate required variables
foreach(var SDK TOOL R RC WORK)
    if(NOT DEFINED ${var})
        message(FATAL_ERROR "Required variable ${var} is not defined")
    endif()
endforeach()

# Find cl.exe on PATH (relies on VS environment being active)
find_program(CL_EXE NAMES cl.exe)
if(NOT CL_EXE)
    message(FATAL_ERROR "cl.exe not found on PATH. Ensure Visual Studio environment is active.")
endif()

set(RR_FILE "${WORK}/pipl.rr")
set(RRC_FILE "${WORK}/pipl.rrc")

# Step 1: Preprocess .r with C preprocessor
# cl /I <SDK_Headers> /EP input.r > output.rr
message(STATUS "PiPL Step 1: Preprocessing ${R} (using ${CL_EXE})")
execute_process(
    COMMAND "${CL_EXE}" /I "${SDK}" /EP "${R}"
    OUTPUT_FILE "${RR_FILE}"
    ERROR_QUIET
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "PiPL Step 1 failed (cl /EP .r): exit code ${res}")
endif()

# Step 2: Run PiPLTool to convert .rr to .rrc
message(STATUS "PiPL Step 2: Running PiPLTool (${TOOL})")
execute_process(
    COMMAND "${TOOL}" "${RR_FILE}" "${RRC_FILE}"
    RESULT_VARIABLE res
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "PiPL Step 2 failed (PiPLTool): exit code ${res}\nstdout: ${out}\nstderr: ${err}")
endif()

# Step 3: Final preprocessing with MSWindows define
# cl /D "MSWindows" /EP output.rrc > output.rc
message(STATUS "PiPL Step 3: Generating final .rc")
execute_process(
    COMMAND "${CL_EXE}" /D "MSWindows" /EP "${RRC_FILE}"
    OUTPUT_FILE "${RC}"
    ERROR_QUIET
    RESULT_VARIABLE res
)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "PiPL Step 3 failed (cl /EP .rrc): exit code ${res}")
endif()

message(STATUS "PiPL compiled successfully: ${RC}")
