---
description: File path and deployment rules
globs: ["**/*"]
alwaysApply: true
---

# Deployment Rules

## File Path Policy

- **NEVER use AppData paths** - AppData directory (`%APPDATA%`, `%LOCALAPPDATA%`) is STRICTLY FORBIDDEN.
- **Use Program Files only** - All plugin files must be in `C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE\`
- **No user-specific paths** - Plugin deployment must not involve user-specific directories.
- **CMake INSTALL target** - Do not use CMake INSTALL target if it deploys to AppData.

## Deployment Policy

**CRITICAL: Manual file copying is strictly prohibited**

1. **Use deployment scripts only** - All file deployments MUST be performed using the designated deployment scripts (`deploy.bat`, `deploy.ps1`, etc.)
2. **No manual copying** - Manually copying files (via Explorer, `copy`, `xcopy`, `robocopy`, etc.) is STRICTLY FORBIDDEN.
3. **Benefits of script-based deployment:**
   - Ensures consistency across deployments
   - Prevents missing dependencies
   - Tracks deployment history
   - Reduces human error
   - Maintains proper file permissions
4. **Emergency exception** - Only in extreme emergencies where deployment scripts are broken may manual copying be temporarily used, but the deployment script MUST be fixed immediately afterward.
5. **Script maintenance** - Keep deployment scripts up-to-date with all required files and dependencies.
