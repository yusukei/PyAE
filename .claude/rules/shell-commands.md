---
description: Shell command restrictions for this project
globs: ["**/*"]
alwaysApply: true
---

# Shell Command Restrictions

## Allowed Commands

Only the following commands are permitted:

| Command | Description |
|---------|-------------|
| `python` | Python interpreter (use for process execution via subprocess) |
| `git` | Version control operations |
| `cmd` | Windows command prompt |
| `powershell` / `pwsh` | PowerShell |
| `build.bat` | Build the plugin |
| `rebuild-and-deploy.bat` | Clean build and deploy |
| `setup_all.bat` | Multi-SDK build and deploy |
| `run_tests.bat` | Test execution script |

## Strictly Forbidden

- ❌ `cmake` - Do not invoke directly

## Process Execution

Use Python's `subprocess` module within Python scripts:

```python
import subprocess
subprocess.run(["path/to/executable", "arg1", "arg2"], env=env_vars)
```

## No Restrictions On

- File operations (read, write, edit, delete)
- Document management and cleanup
- Test file generation
- Code generation and editing
