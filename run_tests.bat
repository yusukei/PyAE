@echo off
setlocal enabledelayedexpansion

REM ========================================
REM PyAE Test Runner (Auto Test Mode)
REM Environment: Variables trigger internal test runner
REM ========================================

REM Batch directory
set "BATCH_DIR=%~dp0"
cd /d "%BATCH_DIR%"

REM Configuration
set "AE_PATH=C:\Program Files\Adobe\Adobe After Effects 2025\Support Files\AfterFX.exe"
set "PLUGIN_DIR=C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE"
set "LOGS_DIR=%BATCH_DIR%logs"
set "RESULT_JSON=%LOGS_DIR%\test_results.json"
set "RESULT_TXT=%LOGS_DIR%\test_results.txt"

echo.
echo ========================================
echo PyAE Test Runner (Auto Test)
echo ========================================
echo.

REM Usage Arguments
if "%1"=="--help" goto :help
if "%1"=="-h" goto :help
if "%1"=="--deploy" goto :deploy
if "%1"=="--results" goto :show_results
if "%1"=="--target" goto :run_with_target

REM Verify After Effects
if not exist "%AE_PATH%" (
    echo ERROR: After Effects not found at:
    echo   %AE_PATH%
    echo.
    echo Please install After Effects 2025 or update AE_PATH in this script.
    exit /b 1
)

REM Verify Plugin
if not exist "%PLUGIN_DIR%\PyAEBridge.dll" (
    echo ERROR: PyAEBridge.dll not found at:
    echo   %PLUGIN_DIR%
    echo.
    echo Please run: setup.bat
    exit /b 1
)

REM Auto Test Configuration
set PYAE_TEST_TIMEOUT=600

REM Find test_runner.exe (standalone build, Release over Debug)
set "TEST_RUNNER="
if exist "%BATCH_DIR%build\TestRunner\Release\TestRunner.exe" (
    set "TEST_RUNNER=%BATCH_DIR%build\TestRunner\Release\TestRunner.exe"
) else if exist "%BATCH_DIR%build\TestRunner\Debug\TestRunner.exe" (
    set "TEST_RUNNER=%BATCH_DIR%build\TestRunner\Debug\TestRunner.exe"
)

if not defined TEST_RUNNER (
    echo ERROR: TestRunner.exe not found.
    echo Please build it first with: build.bat --project TestRunner
    exit /b 1
)

REM Run tests with debug capture using C++ test runner
echo Running tests with OutputDebugString capture (C++)...
echo Test Runner: %TEST_RUNNER%
echo.

"%TEST_RUNNER%" --ae-path "%AE_PATH%" --plugin-dir "%PLUGIN_DIR%" --pyae-dir "%BATCH_DIR%PyAE" --timeout "%PYAE_TEST_TIMEOUT%"

if errorlevel 1 (
    echo.
    echo Test execution failed or timed out.
    exit /b 1
)

echo.
echo Test execution completed successfully.

goto :end

:deploy
echo Deploying test files only (no test execution)...
if not exist "%PLUGIN_DIR%\scripts" mkdir "%PLUGIN_DIR%\scripts"
copy /Y "%BATCH_DIR%PyAE\scripts\*.py" "%PLUGIN_DIR%\scripts\" >nul 2>&1
if not exist "%PLUGIN_DIR%\tests" mkdir "%PLUGIN_DIR%\tests"
xcopy /Y /E /I "%BATCH_DIR%PyAE\tests" "%PLUGIN_DIR%\tests" >nul 2>&1
echo Done.
goto :end

:show_results
if exist "%RESULT_JSON%" (
    type "%RESULT_JSON%"
) else (
    echo No results file found.
)
goto :end

:run_with_target
if "%~2"=="" (
    echo ERROR: --target requires a module name
    echo Example: run_tests.bat --target Composition
    echo Example: run_tests.bat --target "Composition,Layer"
    exit /b 1
)

REM Store all arguments after --target (handles comma-separated values from Git Bash)
REM Git Bash may split "Project,Composition,Layer" into separate arguments: Project, Composition, Layer
REM We need to rejoin them with commas
set "TEST_TARGET=%~2"
set "TEST_REPEAT=1"

REM Check for --repeat option in various positions
if "%~3"=="--repeat" (
    set "TEST_REPEAT=%~4"
) else if "%~4"=="--repeat" (
    set "TEST_REPEAT=%~5"
    set "TEST_TARGET=%TEST_TARGET%,%~3"
) else if "%~5"=="--repeat" (
    set "TEST_REPEAT=%~6"
    set "TEST_TARGET=%TEST_TARGET%,%~3,%~4"
) else if "%~6"=="--repeat" (
    set "TEST_REPEAT=%~7"
    set "TEST_TARGET=%TEST_TARGET%,%~3,%~4,%~5"
) else if "%~7"=="--repeat" (
    set "TEST_REPEAT=%~8"
    set "TEST_TARGET=%TEST_TARGET%,%~3,%~4,%~5,%~6"
) else (
    REM No --repeat, just rejoin targets
    if not "%~3"=="" set "TEST_TARGET=%TEST_TARGET%,%~3"
    if not "%~4"=="" set "TEST_TARGET=%TEST_TARGET%,%~4"
    if not "%~5"=="" set "TEST_TARGET=%TEST_TARGET%,%~5"
    if not "%~6"=="" set "TEST_TARGET=%TEST_TARGET%,%~6"
    if not "%~7"=="" set "TEST_TARGET=%TEST_TARGET%,%~7"
    if not "%~8"=="" set "TEST_TARGET=%TEST_TARGET%,%~8"
    if not "%~9"=="" set "TEST_TARGET=%TEST_TARGET%,%~9"
)

REM Auto Test Configuration
set PYAE_TEST_TIMEOUT=600

REM Verify After Effects
if not exist "%AE_PATH%" (
    echo ERROR: After Effects not found at:
    echo   %AE_PATH%
    exit /b 1
)

REM Verify Plugin
if not exist "%PLUGIN_DIR%\PyAEBridge.dll" (
    echo ERROR: PyAEBridge.dll not found at:
    echo   %PLUGIN_DIR%
    exit /b 1
)

REM Find test_runner.exe (standalone build, Release over Debug)
set "TEST_RUNNER="
if exist "%BATCH_DIR%build\TestRunner\Release\TestRunner.exe" (
    set "TEST_RUNNER=%BATCH_DIR%build\TestRunner\Release\TestRunner.exe"
) else if exist "%BATCH_DIR%build\TestRunner\Debug\TestRunner.exe" (
    set "TEST_RUNNER=%BATCH_DIR%build\TestRunner\Debug\TestRunner.exe"
)

if not defined TEST_RUNNER (
    echo ERROR: TestRunner.exe not found.
    echo Please build it first with: build.bat --project TestRunner
    exit /b 1
)

REM Run tests with specific target and repeat count
REM Note: Repeat logic is now handled inside Python test runner (same AE process)
echo Running tests for target: %TEST_TARGET%
echo Repeat count: %TEST_REPEAT% (within same After Effects process)
echo Test Runner: %TEST_RUNNER%
echo.

"%TEST_RUNNER%" --ae-path "%AE_PATH%" --plugin-dir "%PLUGIN_DIR%" --pyae-dir "%BATCH_DIR%PyAE" --timeout "%PYAE_TEST_TIMEOUT%" --test-target "%TEST_TARGET%" --test-repeat "%TEST_REPEAT%"

if errorlevel 1 (
    echo.
    echo [ERROR] Test execution failed or timed out.
    exit /b 1
)

echo.
echo [SUCCESS] Test execution completed successfully.
goto :end

:help
echo.
echo PyAE Auto Test Runner
echo.
echo Usage:
echo   run_tests.bat                           Run all tests via Auto Test Mode
echo   run_tests.bat --target MODULE           Run specific test module(s) or group(s)
echo   run_tests.bat --target MODULE --repeat N  Run tests N times
echo   run_tests.bat --deploy                  Deploy test files only
echo   run_tests.bat --results                 Show last test results
echo   run_tests.bat --help                    Show this help
echo.
echo Examples:
echo   run_tests.bat --target Composition      Run Composition tests only
echo   run_tests.bat --target "Composition,Layer"  Run Composition and Layer tests
echo   run_tests.bat --target core             Run all Core tests
echo   run_tests.bat --target "core,animation" Run Core and Animation groups
echo   run_tests.bat --target core --repeat 3  Run Core tests 3 times
echo.
echo Test Groups:
echo   core        - All Core tests (Project, Composition, Layer, Effect, etc.)
echo   animation   - Animation tests (Keyframe Operations, Interpolation, etc.)
echo   sdk         - SDK Suite tests (Camera/Light, I/O, Data/Settings, etc.)
echo   shape       - Shape Layer tests (Basic, Properties, Trim Paths)
echo   other       - Other tests (Parent Linking, Footage, Folder, etc.)
echo   all         - All available tests
echo.
echo Available modules:
echo.
echo   Core Tests:
echo     Project, Item/Folder, Composition, Layer, Layer Quality/Label,
echo     Layer Advanced, Effect, Mask, Property, Property Advanced,
echo     StreamSuite Low-level, DynamicProperty, RenderQueue, 3D Layer,
echo     Command, Utility, Marker
echo.
echo   Animation Tests:
echo     Keyframe Operations, Keyframe Interpolation, Animation Roundtrip,
echo     Motion Path Bezier, Easing Functions
echo.
echo   Other Tests:
echo     Parent Linking, FootageItem, Folder Operations, Batch Operations,
echo     Special Layers, Render Queue Details, Shape Layer Basic,
echo     Shape Properties, Trim Paths, Expression Links, Text Outline,
echo     Memory Diagnostics
echo.
echo   SDK Suite Tests:
echo     OutputModule Suite, RQItem Suite, Camera/Light Suite, I/O Suite,
echo     Footage Suite, Collection Suite, TextDocument Suite,
echo     RenderQueue Suite, Data/Settings Suite
echo.
echo Note: This script sets PYAE_AUTO_TEST=1 and launches After Effects.
echo       The plugin will automatically run the specified tests.
echo.
goto :end

:end
endlocal
