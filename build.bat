@echo off
setlocal enabledelayedexpansion

REM ============================================
REM Unified Build Script (PyAE / TestEffect)
REM ============================================

set "SCRIPT_DIR=%~dp0"
REM Remove trailing backslash to avoid quote escaping issues
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
set "PROJECT=PyAE"
set "SDK_PATH="
set "BUILD_TYPE=Debug"
set "GENERATOR=Visual Studio 17 2022"
set "CLEAN_BUILD=0"
set "QT6_DIR="
set "OUTPUT_SUFFIX="

REM Parse arguments
:parse_args
if "%~1"=="" goto :check_required
if /i "%~1"=="--project" (
    set "PROJECT=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--sdk-path" (
    set "SDK_PATH=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--config" (
    set "BUILD_TYPE=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--clean" (
    set "CLEAN_BUILD=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--qt6-dir" (
    set "QT6_DIR=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--output-suffix" (
    set "OUTPUT_SUFFIX=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--help" (
    goto :show_help
)
if /i "%~1"=="-h" (
    goto :show_help
)
echo Unknown option: %~1
goto :show_help

:check_required
REM TestRunner does not require SDK
if /i "%PROJECT%"=="TestRunner" goto :no_sdk_project

if not "%SDK_PATH%"=="" goto :after_args

REM Auto-detect SDK path from SDKs/ directory (prefer latest)
if exist "%SCRIPT_DIR%\SDKs\ae25.6_61.64bit.AfterEffectsSDK\Examples" (
    set "SDK_PATH=%SCRIPT_DIR%\SDKs\ae25.6_61.64bit.AfterEffectsSDK\Examples"
    goto :sdk_found
)
if exist "%SCRIPT_DIR%\SDKs\ae25.2_20.64bit.AfterEffectsSDK\Examples" (
    set "SDK_PATH=%SCRIPT_DIR%\SDKs\ae25.2_20.64bit.AfterEffectsSDK\Examples"
    goto :sdk_found
)
if exist "%SCRIPT_DIR%\SDKs\AfterEffectsSDK 2023\Examples" (
    set "SDK_PATH=%SCRIPT_DIR%\SDKs\AfterEffectsSDK 2023\Examples"
    goto :sdk_found
)
if exist "%SCRIPT_DIR%\SDKs\AfterEffectsSDK 2022\Examples" (
    set "SDK_PATH=%SCRIPT_DIR%\SDKs\AfterEffectsSDK 2022\Examples"
    goto :sdk_found
)
echo Error: No SDK found in SDKs/ directory.
echo Please specify --sdk-path or place an SDK in the SDKs/ directory.
exit /b 1

:sdk_found
echo Auto-detected SDK: %SDK_PATH%
goto :after_args

:no_sdk_project
REM Projects that do not require AE SDK (e.g., TestRunner)
set "SOURCE_DIR=%SCRIPT_DIR%\%PROJECT%"
if not exist "%SOURCE_DIR%\CMakeLists.txt" (
    echo Error: Project "%PROJECT%" not found at %SOURCE_DIR%
    exit /b 1
)
if not "%BUILD_TYPE%"=="Debug" if not "%BUILD_TYPE%"=="Release" (
    echo Error: Invalid BUILD_TYPE "%BUILD_TYPE%". Supported: Debug, Release
    exit /b 1
)
set "BUILD_DIR=%SCRIPT_DIR%\build\%PROJECT%"
set "SDK_NAME="
set "CMAKE_OPTS="
goto :do_build

:after_args

REM Validate project directory exists
set "SOURCE_DIR=%SCRIPT_DIR%\%PROJECT%"
if not exist "%SOURCE_DIR%\CMakeLists.txt" (
    echo Error: Project "%PROJECT%" not found at %SOURCE_DIR%
    exit /b 1
)

REM Validate SDK_PATH exists
if not exist "%SDK_PATH%" (
    echo Error: SDK path does not exist: %SDK_PATH%
    exit /b 1
)

REM Validate BUILD_TYPE
if not "%BUILD_TYPE%"=="Debug" if not "%BUILD_TYPE%"=="Release" (
    echo Error: Invalid BUILD_TYPE "%BUILD_TYPE%". Supported: Debug, Release
    exit /b 1
)

REM Determine build directory name from SDK path
for %%I in ("%SDK_PATH%\..") do set "SDK_NAME=%%~nxI"

REM Auto-detect output suffix for PyAE from SDK name (always required)
if /i not "%PROJECT%"=="PyAE" goto :skip_suffix_detect
if not "%OUTPUT_SUFFIX%"=="" goto :skip_suffix_detect

echo %SDK_NAME% | findstr /i "25.6 25_6" >nul
if not errorlevel 1 (
    set "OUTPUT_SUFFIX=_25_6"
    goto :skip_suffix_detect
)
echo %SDK_NAME% | findstr /i "25.2 25_2" >nul
if not errorlevel 1 (
    set "OUTPUT_SUFFIX=_25_2"
    goto :skip_suffix_detect
)
echo %SDK_NAME% | findstr /i "2023" >nul
if not errorlevel 1 (
    set "OUTPUT_SUFFIX=_2023"
    goto :skip_suffix_detect
)
echo %SDK_NAME% | findstr /i "2022" >nul
if not errorlevel 1 (
    set "OUTPUT_SUFFIX=_2022"
    goto :skip_suffix_detect
)
echo Error: Could not determine SDK version from "%SDK_NAME%"
echo Please specify --output-suffix explicitly.
exit /b 1

:skip_suffix_detect

REM Build directory: PyAE -> build/<SDK>, others -> build/<PROJECT>/<SDK>
if /i "%PROJECT%"=="PyAE" (
    set "BUILD_DIR=%SCRIPT_DIR%\build\%SDK_NAME%"
) else (
    set "BUILD_DIR=%SCRIPT_DIR%\build\%PROJECT%\%SDK_NAME%"
)

REM Auto-detect Qt6 if not specified (PyAE only)
if /i "%PROJECT%"=="PyAE" (
    if "%QT6_DIR%"=="" (
        REM Search common Qt6 installation paths
        for %%V in (6.10.2 6.9.0 6.8.0 6.7.0 6.6.0 6.5.3) do (
            if exist "C:\Qt\%%V\msvc2022_64\lib\cmake\Qt6\Qt6Config.cmake" (
                set "QT6_DIR=C:\Qt\%%V\msvc2022_64\lib\cmake\Qt6"
                goto :qt6_found
            )
        )
    )
)
:qt6_found

REM Build cmake options for SDK-dependent projects
set "CMAKE_OPTS=-DAESDK_PATH="%SDK_PATH%""
if not "%QT6_DIR%"=="" set "CMAKE_OPTS=%CMAKE_OPTS% -DQt6_DIR="%QT6_DIR%""
if not "%OUTPUT_SUFFIX%"=="" set "CMAKE_OPTS=%CMAKE_OPTS% -DPYAE_CORE_SUFFIX=%OUTPUT_SUFFIX%"

:do_build

echo ============================================
echo %PROJECT% Build
echo ============================================
echo Project:           %PROJECT%
echo Source Directory:   %SOURCE_DIR%
if not "%SDK_PATH%"=="" echo SDK Path:          %SDK_PATH%
if not "%SDK_NAME%"=="" echo SDK Name:          %SDK_NAME%
echo Build Type:        %BUILD_TYPE%
echo Build Directory:   %BUILD_DIR%
if not "%QT6_DIR%"=="" echo Qt6 Directory:     %QT6_DIR%
if not "%OUTPUT_SUFFIX%"=="" echo Output Suffix:     %OUTPUT_SUFFIX%
echo ============================================

REM Clean build if requested
if "%CLEAN_BUILD%"=="1" (
    echo Cleaning build directory...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Run CMake configure
echo.
echo [1/2] Configuring CMake...

cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 %CMAKE_OPTS%
if errorlevel 1 (
    echo CMake configuration failed!
    exit /b 1
)

REM Run CMake build
echo.
echo [2/2] Building...
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE%
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo ============================================
echo Build completed successfully!
if /i "%PROJECT%"=="PyAE" (
    echo Output: %BUILD_DIR%\src\%BUILD_TYPE%\PyAE.aex
    echo Core:   %BUILD_DIR%\src\%BUILD_TYPE%\PyAECore%OUTPUT_SUFFIX%.dll
    if exist "%BUILD_DIR%\src\PySide\%BUILD_TYPE%\PyAEPySide.dll" (
        echo PySide: %BUILD_DIR%\src\PySide\%BUILD_TYPE%\PyAEPySide.dll
    ) else (
        echo PySide: Not built [Qt6 not found]
    )
) else if /i "%PROJECT%"=="TestRunner" (
    echo Output: %BUILD_DIR%\%BUILD_TYPE%\TestRunner.exe
) else (
    echo Output: %BUILD_DIR%\%BUILD_TYPE%\%PROJECT%.aex
)
echo ============================================

exit /b 0

:show_help
echo Usage: build.bat [options]
echo.
echo Options:
echo   --sdk-path ^<path^>     Path to SDK Examples directory
echo                            Auto-detected from SDKs/ directory if not specified
echo   --project ^<name^>      Project to build (default: PyAE)
echo                            Available: PyAE, TestEffect, TestRunner
echo   --config ^<type^>       Build configuration (Debug, Release)
echo                            Default: Debug
echo   --clean                 Clean build directory before building
echo   --qt6-dir ^<path^>      Path to Qt6 CMake config (auto-detected if not set)
echo   --output-suffix ^<sfx^> Override auto-detected suffix for PyAECore DLL name
echo                            Normally auto-detected from SDK path (e.g., _2022, _25_6)
echo   --help, -h              Show this help message
echo.
echo PyAE builds always produce versioned DLLs (e.g., PyAECore_25_6.dll).
echo The version suffix is auto-detected from the SDK path.
echo.
echo Examples:
echo   build.bat --sdk-path "../SDKs/ae25.6_61.64bit.AfterEffectsSDK/Examples"
echo     -^> PyAECore_25_6.dll
echo   build.bat --sdk-path "../SDKs/AfterEffectsSDK 2022/Examples"
echo     -^> PyAECore_2022.dll
echo   build.bat --project TestEffect --sdk-path "../SDKs/ae25.6_61.64bit.AfterEffectsSDK/Examples"
echo     -^> TestEffect.aex
echo   build.bat --sdk-path "../SDKs/ae25.6_61.64bit.AfterEffectsSDK/Examples" --clean
echo   build.bat --sdk-path "../SDKs/ae25.6_61.64bit.AfterEffectsSDK/Examples" --config Release
exit /b 1
