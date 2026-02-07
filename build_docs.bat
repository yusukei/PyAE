@echo off
REM PyAE Documentation Build Script
REM
REM Uses uv to manage dependencies and build Sphinx documentation.
REM Requires: uv (https://github.com/astral-sh/uv)

setlocal

set SCRIPT_DIR=%~dp0
set DOCS_DIR=%SCRIPT_DIR%docs

echo ========================================
echo PyAE Documentation Build
echo ========================================
echo.

REM Check if uv is installed
where uv >NUL 2>&1
if errorlevel 1 (
    echo Error: uv is not installed
    echo Please install uv: https://github.com/astral-sh/uv
    exit /b 1
)

REM Parse command line options
set BUILD_TYPE=html
set CLEAN=0

:parse_args
if "%1"=="" goto end_parse
if /i "%1"=="--clean" set CLEAN=1
if /i "%1"=="--pdf" set BUILD_TYPE=latexpdf
if /i "%1"=="--help" goto show_help
if /i "%1"=="-h" goto show_help
shift
goto parse_args
:end_parse

REM Move into docs directory
cd /d "%DOCS_DIR%"
if errorlevel 1 (
    echo Error: docs directory not found: %DOCS_DIR%
    exit /b 1
)

REM Install/sync dependencies
echo Syncing dependencies...
uv sync
if errorlevel 1 (
    echo Failed to sync dependencies
    exit /b 1
)

REM Clean build directory if requested
if %CLEAN%==1 (
    echo Cleaning build directory...
    if exist "_build" rmdir /s /q "_build"
)

REM Build documentation
echo.
echo Building %BUILD_TYPE% documentation...
echo Source: %DOCS_DIR%
echo.

uv run python -m sphinx -M %BUILD_TYPE% . _build
if errorlevel 1 (
    echo.
    echo ========================================
    echo Build FAILED
    echo ========================================
    exit /b 1
)

echo.
echo ========================================
echo Build Successful
echo ========================================
echo.
echo Output: %DOCS_DIR%\_build\%BUILD_TYPE%
echo.

if "%BUILD_TYPE%"=="html" (
    echo To view the documentation, open:
    echo   %DOCS_DIR%\_build\html\index.html
)

exit /b 0

:show_help
echo Usage: build_docs.bat [options]
echo.
echo Options:
echo   --clean        Clean build directory before building
echo   --pdf          Build PDF documentation (requires LaTeX)
echo   --help, -h     Show this help message
echo.
echo Examples:
echo   build_docs.bat                 Build HTML documentation
echo   build_docs.bat --clean         Clean build and rebuild
echo   build_docs.bat --pdf           Build PDF documentation
echo.
echo Requirements:
echo   - uv (https://github.com/astral-sh/uv)
exit /b 0
