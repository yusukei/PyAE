// pch.h
// Precompiled Header for PyAECore

#pragma once

// Windows API
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// Standard Library
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <stdexcept>
#include <sstream>

// Pybind11
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>

// After Effects SDK
#include "AE_GeneralPlug.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "AE_GeneralPlugPanels.h"
#include "AEGP_SuiteHandler.h"

// Project Common Headers
// (These headers are stable enough to include in PCH.
//  Headers that change frequently should not be included here.)
#include "Logger.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"

// Note: PluginState.h includes many other headers, so it might be good candidate,
// but be careful about circular dependencies if PluginState changes often.
// For now, let's keep it minimal to stable SDK/STL headers and utility classes.
