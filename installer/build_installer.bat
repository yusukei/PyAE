@echo off
setlocal enabledelayedexpansion

REM ============================================
REM PyAE MSI Installer Build Script
REM Builds all SDK versions and generates MSI
REM ============================================

set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
set "PROJECT_DIR=%SCRIPT_DIR%\.."
set "STAGING=%SCRIPT_DIR%\staging"
set "BUILD_TYPE=Release"
set "SKIP_BUILD=0"
set "VERSION=1.0.0"

REM Parse arguments
:parse_args
if "%~1"=="" goto :after_args
if /i "%~1"=="--skip-build" (
    set "SKIP_BUILD=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--debug" (
    set "BUILD_TYPE=Debug"
    shift
    goto :parse_args
)
if /i "%~1"=="--version" (
    set "VERSION=%~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--help" goto :show_help
if /i "%~1"=="-h" goto :show_help
shift
goto :parse_args

:after_args

echo ============================================
echo PyAE MSI Installer Build
echo ============================================
echo Build Type: %BUILD_TYPE%
echo Version:    %VERSION%
echo ============================================
echo.

REM ============================================
REM Step 1: Build all SDK versions
REM ============================================
if "%SKIP_BUILD%"=="1" (
    echo [Step 1] Skipping builds...
    goto :staging
)

echo [Step 1] Building PyAE for all SDK versions...
echo.

REM SDK 2022
echo --- Building PyAE (SDK 2022) ---
call "%PROJECT_DIR%\build.bat" --project PyAE --sdk-path "%PROJECT_DIR%\SDKs\AfterEffectsSDK 2022\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: PyAE SDK 2022 build failed!
    exit /b 1
)
echo.

REM SDK 2023
echo --- Building PyAE (SDK 2023) ---
call "%PROJECT_DIR%\build.bat" --project PyAE --sdk-path "%PROJECT_DIR%\SDKs\AfterEffectsSDK 2023\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: PyAE SDK 2023 build failed!
    exit /b 1
)
echo.

REM SDK 25.2
echo --- Building PyAE (SDK 25.2) ---
call "%PROJECT_DIR%\build.bat" --project PyAE --sdk-path "%PROJECT_DIR%\SDKs\ae25.2_20.64bit.AfterEffectsSDK\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: PyAE SDK 25.2 build failed!
    exit /b 1
)
echo.

REM SDK 25.6
echo --- Building PyAE (SDK 25.6) ---
call "%PROJECT_DIR%\build.bat" --project PyAE --sdk-path "%PROJECT_DIR%\SDKs\ae25.6_61.64bit.AfterEffectsSDK\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: PyAE SDK 25.6 build failed!
    exit /b 1
)
echo.

REM ============================================
REM Step 2: Prepare staging directory
REM ============================================
:staging
echo [Step 2] Preparing staging directory...

REM Clean staging
if exist "%STAGING%" rmdir /S /Q "%STAGING%"
mkdir "%STAGING%"

REM Copy PyAE.aex (latest SDK build)
set "AEX_SRC=%PROJECT_DIR%\build\ae25.6_61.64bit.AfterEffectsSDK\src\%BUILD_TYPE%\PyAE.aex"
if not exist "%AEX_SRC%" (
    echo ERROR: PyAE.aex not found at %AEX_SRC%
    exit /b 1
)
copy /Y "%AEX_SRC%" "%STAGING%\"
echo   - PyAE.aex

REM Copy PyAECore DLLs
set "CORE_2022=%PROJECT_DIR%\build\AfterEffectsSDK 2022\src\%BUILD_TYPE%\PyAECore_2022.dll"
if exist "%CORE_2022%" (
    copy /Y "%CORE_2022%" "%STAGING%\"
    echo   - PyAECore_2022.dll
) else (
    echo ERROR: PyAECore_2022.dll not found!
    exit /b 1
)

set "CORE_2023=%PROJECT_DIR%\build\AfterEffectsSDK 2023\src\%BUILD_TYPE%\PyAECore_2023.dll"
if exist "%CORE_2023%" (
    copy /Y "%CORE_2023%" "%STAGING%\"
    echo   - PyAECore_2023.dll
) else (
    echo ERROR: PyAECore_2023.dll not found!
    exit /b 1
)

set "CORE_25_2=%PROJECT_DIR%\build\ae25.2_20.64bit.AfterEffectsSDK\src\%BUILD_TYPE%\PyAECore_25_2.dll"
if exist "%CORE_25_2%" (
    copy /Y "%CORE_25_2%" "%STAGING%\"
    echo   - PyAECore_25_2.dll
) else (
    echo ERROR: PyAECore_25_2.dll not found!
    exit /b 1
)

set "CORE_25_6=%PROJECT_DIR%\build\ae25.6_61.64bit.AfterEffectsSDK\src\%BUILD_TYPE%\PyAECore_25_6.dll"
if exist "%CORE_25_6%" (
    copy /Y "%CORE_25_6%" "%STAGING%\"
    echo   - PyAECore_25_6.dll
) else (
    echo ERROR: PyAECore_25_6.dll not found!
    exit /b 1
)

REM Copy PyAEBridge.dll
set "BRIDGE_SRC=%PROJECT_DIR%\build\ae25.6_61.64bit.AfterEffectsSDK\src\%BUILD_TYPE%\PyAEBridge.dll"
if exist "%BRIDGE_SRC%" (
    copy /Y "%BRIDGE_SRC%" "%STAGING%\"
    echo   - PyAEBridge.dll
) else (
    echo ERROR: PyAEBridge.dll not found!
    exit /b 1
)

REM Copy PyAEPySide.dll (optional)
set "PYSIDE_SRC=%PROJECT_DIR%\build\ae25.6_61.64bit.AfterEffectsSDK\src\PySide\%BUILD_TYPE%\PyAEPySide.dll"
if exist "%PYSIDE_SRC%" (
    copy /Y "%PYSIDE_SRC%" "%STAGING%\"
    echo   - PyAEPySide.dll
) else (
    echo   - PyAEPySide.dll not found, skipping (optional)
)

REM Copy Python310 runtime
echo   - Copying Python310...
set "PYTHON_SRC=%PROJECT_DIR%\Python310"
if not exist "%PYTHON_SRC%\python310.dll" (
    echo ERROR: Python310 not found at %PYTHON_SRC%
    exit /b 1
)
robocopy "%PYTHON_SRC%" "%STAGING%\Python310" /E /NFL /NDL /NJH /NJS /NC /NS /NP /XD __pycache__ Scripts test idlelib tkinter
if %ERRORLEVEL% GEQ 8 (
    echo ERROR: Failed to copy Python310!
    exit /b 1
)
echo   - Python310 copied

echo.
echo Staging complete.
echo.

REM ============================================
REM Step 3: Build MSI
REM ============================================
echo [Step 3] Building MSI...
echo.

REM Check wix is available
where wix >nul 2>nul
if errorlevel 1 (
    echo ERROR: WiX toolset not found!
    echo Install with: dotnet tool install --global wix
    exit /b 1
)

REM Build MSI
set "MSI_OUTPUT=%SCRIPT_DIR%\PyAE-%VERSION%.msi"
wix build "%SCRIPT_DIR%\PyAE.wxs" -o "%MSI_OUTPUT%" -arch x64
if errorlevel 1 (
    echo ERROR: MSI build failed!
    exit /b 1
)

echo.
echo ============================================
echo MSI build successful!
echo Output: %MSI_OUTPUT%
echo ============================================

REM ============================================
REM Step 4: Cleanup staging
REM ============================================
echo.
echo Cleaning up staging directory...
rmdir /S /Q "%STAGING%"

exit /b 0

:show_help
echo PyAE MSI Installer Build Script
echo.
echo Builds PyAE for all SDK versions and generates an MSI installer.
echo.
echo Usage: build_installer.bat [options]
echo.
echo Options:
echo   --skip-build     Skip building, use existing build outputs
echo   --debug          Use Debug build configuration (default: Release)
echo   --version ^<ver^>  Set MSI version (default: 1.0.0)
echo   --help, -h       Show this help message
echo.
echo Prerequisites:
echo   - .NET SDK (for WiX toolset)
echo   - WiX toolset: dotnet tool install --global wix
echo.
echo Examples:
echo   build_installer.bat                    Full build + MSI
echo   build_installer.bat --skip-build       MSI from existing builds
echo   build_installer.bat --version 1.1.0    Custom version
exit /b 0
