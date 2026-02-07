@echo off
setlocal enabledelayedexpansion

REM ============================================
REM PyAE & TestEffect Multi-SDK Build and Deploy
REM Builds PyAECore for all supported SDK versions,
REM builds TestEffect, and deploys to plugin directories
REM ============================================

set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

set "BUILD_TYPE=Release"
set "SKIP_BUILD=0"
set "UNINSTALL=0"
set "PLUGIN_DIR=C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE"
set "TESTEFFECT_PLUGIN_DIR=C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\TestEffect"
set "PYTHON_SRC=%SCRIPT_DIR%\Python310"

REM Parse arguments
:parse_args
if "%~1"=="" goto :after_args
if /i "%~1"=="--debug" (
    set "BUILD_TYPE=Debug"
    shift
    goto :parse_args
)
if /i "%~1"=="--release" (
    set "BUILD_TYPE=Release"
    shift
    goto :parse_args
)
if /i "%~1"=="--skip-build" (
    set "SKIP_BUILD=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--uninstall" (
    set "UNINSTALL=1"
    shift
    goto :parse_args
)
if /i "%~1"=="--help" goto :show_help
if /i "%~1"=="-h" goto :show_help
shift
goto :parse_args

:after_args

REM Handle uninstall
if "%UNINSTALL%"=="1" goto :do_uninstall

echo ============================================
echo PyAE ^& TestEffect Multi-SDK Build and Deploy
echo ============================================
echo Build Type: %BUILD_TYPE%
echo PyAE Plugin Dir: %PLUGIN_DIR%
echo TestEffect Plugin Dir: %TESTEFFECT_PLUGIN_DIR%
echo ============================================
echo.

REM ============================================
REM Step 1: Build all SDK versions (PyAE)
REM ============================================
if "%SKIP_BUILD%"=="1" (
    echo [Step 1] Skipping builds...
    goto :deploy
)

echo [Step 1] Building PyAE for all SDK versions...
echo.

REM SDK 2022
echo --- Building PyAE (SDK 2022) ---
call "%SCRIPT_DIR%\build.bat" --project PyAE --sdk-path "%SCRIPT_DIR%\SDKs\AfterEffectsSDK 2022\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: PyAE SDK 2022 build failed!
    exit /b 1
)
echo.

REM SDK 2023
echo --- Building PyAE (SDK 2023) ---
call "%SCRIPT_DIR%\build.bat" --project PyAE --sdk-path "%SCRIPT_DIR%\SDKs\AfterEffectsSDK 2023\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: PyAE SDK 2023 build failed!
    exit /b 1
)
echo.

REM SDK 25.2
echo --- Building PyAE (SDK 25.2) ---
call "%SCRIPT_DIR%\build.bat" --project PyAE --sdk-path "%SCRIPT_DIR%\SDKs\ae25.2_20.64bit.AfterEffectsSDK\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: PyAE SDK 25.2 build failed!
    exit /b 1
)
echo.

REM SDK 25.6
echo --- Building PyAE (SDK 25.6) ---
call "%SCRIPT_DIR%\build.bat" --project PyAE --sdk-path "%SCRIPT_DIR%\SDKs\ae25.6_61.64bit.AfterEffectsSDK\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: PyAE SDK 25.6 build failed!
    exit /b 1
)
echo.

REM ============================================
REM Step 2: Build TestEffect (latest SDK only)
REM ============================================
echo [Step 2] Building TestEffect (SDK 25.6)...
echo.

call "%SCRIPT_DIR%\build.bat" --project TestEffect --sdk-path "%SCRIPT_DIR%\SDKs\ae25.6_61.64bit.AfterEffectsSDK\Examples" --config %BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: TestEffect build failed!
    exit /b 1
)
echo.

REM ============================================
REM Step 3: Deploy to plugin directories
REM ============================================
:deploy
echo [Step 3] Deploying to plugin directories...
echo.

REM --- PyAE Deploy ---
echo --- Deploying PyAE ---

REM Create plugin directory
if not exist "%PLUGIN_DIR%" (
    echo Creating PyAE plugin directory...
    mkdir "%PLUGIN_DIR%"
    if errorlevel 1 (
        echo ERROR: Failed to create plugin directory!
        echo Make sure you have administrator privileges.
        exit /b 1
    )
)

REM Copy PyAE.aex (use latest build - SDK 25.6)
set "AEX_SRC=%SCRIPT_DIR%\build\ae25.6_61.64bit.AfterEffectsSDK\src\%BUILD_TYPE%\PyAE.aex"
if not exist "%AEX_SRC%" (
    echo ERROR: PyAE.aex not found at %AEX_SRC%
    exit /b 1
)
copy /Y "%AEX_SRC%" "%PLUGIN_DIR%\"
echo   - PyAE.aex copied

REM Copy PyAECore DLLs for each SDK version
echo   - Copying PyAECore DLLs...

set "CORE_2022=%SCRIPT_DIR%\build\AfterEffectsSDK 2022\src\%BUILD_TYPE%\PyAECore_2022.dll"
if exist "%CORE_2022%" (
    copy /Y "%CORE_2022%" "%PLUGIN_DIR%\"
    echo     - PyAECore_2022.dll copied
) else (
    echo     - PyAECore_2022.dll not found, skipping
)

set "CORE_2023=%SCRIPT_DIR%\build\AfterEffectsSDK 2023\src\%BUILD_TYPE%\PyAECore_2023.dll"
if exist "%CORE_2023%" (
    copy /Y "%CORE_2023%" "%PLUGIN_DIR%\"
    echo     - PyAECore_2023.dll copied
) else (
    echo     - PyAECore_2023.dll not found, skipping
)

set "CORE_25_2=%SCRIPT_DIR%\build\ae25.2_20.64bit.AfterEffectsSDK\src\%BUILD_TYPE%\PyAECore_25_2.dll"
if exist "%CORE_25_2%" (
    copy /Y "%CORE_25_2%" "%PLUGIN_DIR%\"
    echo     - PyAECore_25_2.dll copied
) else (
    echo     - PyAECore_25_2.dll not found, skipping
)

set "CORE_25_6=%SCRIPT_DIR%\build\ae25.6_61.64bit.AfterEffectsSDK\src\%BUILD_TYPE%\PyAECore_25_6.dll"
if exist "%CORE_25_6%" (
    copy /Y "%CORE_25_6%" "%PLUGIN_DIR%\"
    echo     - PyAECore_25_6.dll copied
) else (
    echo     - PyAECore_25_6.dll not found, skipping
)

REM Copy PyAEBridge.dll (use latest build)
set "BRIDGE_SRC=%SCRIPT_DIR%\build\ae25.6_61.64bit.AfterEffectsSDK\src\%BUILD_TYPE%\PyAEBridge.dll"
if exist "%BRIDGE_SRC%" (
    copy /Y "%BRIDGE_SRC%" "%PLUGIN_DIR%\"
    echo   - PyAEBridge.dll copied
)

REM Copy PyAEPySide.dll (use latest build)
set "PYSIDE_SRC=%SCRIPT_DIR%\build\ae25.6_61.64bit.AfterEffectsSDK\src\PySide\%BUILD_TYPE%\PyAEPySide.dll"
if exist "%PYSIDE_SRC%" (
    copy /Y "%PYSIDE_SRC%" "%PLUGIN_DIR%\"
    echo   - PyAEPySide.dll copied
)

REM Copy Python embedded distribution
echo   - Copying Python310...
if exist "%PYTHON_SRC%\python310.dll" (
    robocopy "%PYTHON_SRC%" "%PLUGIN_DIR%" /E /NFL /NDL /NJH /NJS /NC /NS /NP /XD __pycache__ Scripts
    if %ERRORLEVEL% GEQ 8 (
        echo ERROR: Failed to copy Python directory!
        exit /b 1
    )
    echo   - Python310 copied
) else (
    echo ERROR: Python310 not found at %PYTHON_SRC%
    exit /b 1
)

REM Copy scripts
set "SCRIPTS_SRC=%SCRIPT_DIR%\PyAE\scripts"
if exist "%SCRIPTS_SRC%" (
    if not exist "%PLUGIN_DIR%\scripts" mkdir "%PLUGIN_DIR%\scripts"
    xcopy /E /I /Y /Q "%SCRIPTS_SRC%" "%PLUGIN_DIR%\scripts"
    echo   - Scripts copied
)

REM Copy tests
set "TESTS_SRC=%SCRIPT_DIR%\PyAE\tests"
if exist "%TESTS_SRC%" (
    if not exist "%PLUGIN_DIR%\tests" mkdir "%PLUGIN_DIR%\tests"
    xcopy /E /I /Y /Q "%TESTS_SRC%" "%PLUGIN_DIR%\tests"
    echo   - Tests copied
)

REM Create logs directory
if not exist "%PLUGIN_DIR%\logs" mkdir "%PLUGIN_DIR%\logs"

echo.

REM --- TestEffect Deploy ---
echo --- Deploying TestEffect ---

set "TESTEFFECT_AEX=%SCRIPT_DIR%\build\TestEffect\ae25.6_61.64bit.AfterEffectsSDK\%BUILD_TYPE%\TestEffect.aex"
if exist "%TESTEFFECT_AEX%" (
    if not exist "%TESTEFFECT_PLUGIN_DIR%" (
        echo Creating TestEffect plugin directory...
        mkdir "%TESTEFFECT_PLUGIN_DIR%"
        if errorlevel 1 (
            echo ERROR: Failed to create TestEffect plugin directory!
            echo Make sure you have administrator privileges.
            exit /b 1
        )
    )
    copy /Y "%TESTEFFECT_AEX%" "%TESTEFFECT_PLUGIN_DIR%\"
    echo   - TestEffect.aex copied
) else (
    echo   - TestEffect.aex not found, skipping
)

echo.
echo ============================================
echo Build and Deploy completed!
echo ============================================
echo.
echo PyAE installed to: %PLUGIN_DIR%
echo.
echo PyAE contents:
dir /B "%PLUGIN_DIR%\PyAE*" 2>nul
echo.
if exist "%TESTEFFECT_PLUGIN_DIR%\TestEffect.aex" (
    echo TestEffect installed to: %TESTEFFECT_PLUGIN_DIR%
) else (
    echo TestEffect: not deployed
)
echo.
echo PyAE will automatically select the appropriate
echo PyAECore DLL based on your After Effects version.
echo ============================================

exit /b 0

:do_uninstall
echo ============================================
echo Uninstalling PyAE and TestEffect...
echo ============================================
if exist "%PLUGIN_DIR%" (
    rmdir /S /Q "%PLUGIN_DIR%"
    if errorlevel 1 (
        echo Failed to remove PyAE plugin directory!
        echo Make sure you have administrator privileges.
        exit /b 1
    )
    echo PyAE has been uninstalled from:
    echo   %PLUGIN_DIR%
) else (
    echo PyAE is not installed.
)
if exist "%TESTEFFECT_PLUGIN_DIR%" (
    rmdir /S /Q "%TESTEFFECT_PLUGIN_DIR%"
    if errorlevel 1 (
        echo Failed to remove TestEffect plugin directory!
        echo Make sure you have administrator privileges.
        exit /b 1
    )
    echo TestEffect has been uninstalled from:
    echo   %TESTEFFECT_PLUGIN_DIR%
) else (
    echo TestEffect is not installed.
)
exit /b 0

:show_help
echo PyAE ^& TestEffect Multi-SDK Build and Deploy Script
echo.
echo Builds PyAECore for all SDK versions (2022, 2023, 25.2, 25.6),
echo builds TestEffect, and deploys to plugin directories.
echo.
echo Usage: setup.bat [options]
echo.
echo Options:
echo   --debug       Build Debug configuration
echo   --release     Build Release configuration (default)
echo   --skip-build  Skip building, only deploy existing files
echo   --uninstall   Remove PyAE and TestEffect from After Effects
echo   --help, -h    Show this help message
echo.
echo Examples:
echo   setup.bat                  Build and deploy all versions
echo   setup.bat --debug          Build Debug and deploy
echo   setup.bat --skip-build     Deploy without rebuilding
echo   setup.bat --uninstall      Uninstall PyAE and TestEffect
echo.
echo Installation directories:
echo   PyAE:       C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE
echo   TestEffect: C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\TestEffect
exit /b 0
