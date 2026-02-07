---
description: Build process rules
globs: ["**/*.cpp", "**/*.h", "**/CMakeLists.txt"]
alwaysApply: true
---

# Build Rules

## Build Commands

**Use batch files only. Direct cmake invocation is STRICTLY FORBIDDEN.**

All batch files are located at the project root and executed from there.
All options are optional - sensible defaults are auto-detected.

| Command | Description |
|---------|-------------|
| `build.bat` | Build a plugin (PyAE or TestEffect) |
| `setup.bat` | Multi-SDK build and deploy (PyAE + TestEffect) |
| `run_tests.bat` | Run tests |
| `build_docs.bat` | Build Sphinx documentation |

## build.bat Options

```
build.bat [options]

Options:
  --sdk-path <path>     Path to SDK Examples directory
                        Auto-detected from SDKs/ directory if not specified
  --project <name>      Project to build (PyAE, TestEffect)
                        Default: PyAE
  --config <type>       Build configuration (Debug, Release)
                        Default: Debug
  --clean               Clean build directory before building
  --output-suffix <sfx> Override auto-detected suffix for PyAECore DLL name
  --qt6-dir <path>      Path to Qt6 CMake config (auto-detected if not set)
```

PyAE builds always produce versioned DLLs (e.g., PyAECore_25_6.dll).
The version suffix is auto-detected from the SDK path.

### Examples

```batch
# No arguments - auto-detect latest SDK, build PyAE
build.bat

# Specify SDK explicitly
build.bat --sdk-path "SDKs/ae25.6_61.64bit.AfterEffectsSDK/Examples"

# TestEffect build (auto-detect SDK)
build.bat --project TestEffect

# Clean Release build
build.bat --clean --config Release
```

## setup.bat

Performs full multi-SDK build and deploys both PyAE and TestEffect:
1. Build PyAE for all SDK versions (2022, 2023, 25.2, 25.6)
2. Build TestEffect (latest SDK)
3. Deploy PyAE to `C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\`
4. Deploy TestEffect to `C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\TestEffect\`

No arguments required. Requires administrator privileges.

## Strictly Forbidden

- ❌ `cmake` - Do not invoke directly
- ❌ `cmake --build` - Do not invoke directly
- ❌ Manual file copying for deployment - Use `setup.bat`

## Output Location

Build outputs are placed in:
- `build/<SDK_NAME>/src/<config>/PyAE.aex` (PyAE)
- `build/<SDK_NAME>/src/<config>/PyAECore_<version>.dll` (PyAE)
- `build/TestEffect/<SDK_NAME>/<config>/TestEffect.aex` (TestEffect)

Where `<SDK_NAME>` is derived from the SDK directory name (e.g., `ae25.6_61.64bit.AfterEffectsSDK`).
