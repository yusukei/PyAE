// MemoryDiagnostics.cpp
// PyAE - Python for After Effects
// メモリ診断ユーティリティ実装

#include "../../include/MemoryDiagnostics.h"
#include "../../include/PluginState.h"
#include "../../include/Logger.h"
#include <sstream>
#include <iomanip>

namespace PyAE {

MemoryDiagnostics::MemStats MemoryDiagnostics::GetMemoryStats() {
    auto& state = PluginState::Instance();
    if (!state.IsInitialized()) {
        PYAE_LOG_WARNING("Memory", "PluginState not initialized, cannot get memory stats");
        return MemStats();
    }

    const auto& suites = state.GetSuites();
    if (!suites.memorySuite) {
        PYAE_LOG_WARNING("Memory", "MemorySuite not available");
        return MemStats();
    }

    A_long count = 0;
    A_long size = 0;
    A_Err err = suites.memorySuite->AEGP_GetMemStats(
        state.GetPluginID(),
        &count,
        &size
    );

    if (err != A_Err_NONE) {
        PYAE_LOG_ERROR("Memory", "Failed to get memory stats: " + std::to_string(err));
        return MemStats();
    }

    // Debug: Log retrieved memory stats
    PYAE_LOG_INFO("Memory", "GetMemStats succeeded: handles=" + std::to_string(count) +
                            ", size=" + std::to_string(size));

    return MemStats(count, size);
}

void MemoryDiagnostics::EnableMemoryReporting(bool enable) {
    auto& state = PluginState::Instance();
    if (!state.IsInitialized()) {
        PYAE_LOG_WARNING("Memory", "PluginState not initialized, cannot enable memory reporting");
        return;
    }

    const auto& suites = state.GetSuites();
    if (!suites.memorySuite) {
        PYAE_LOG_WARNING("Memory", "MemorySuite not available");
        return;
    }

    A_Err err = suites.memorySuite->AEGP_SetMemReportingOn(enable ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        PYAE_LOG_ERROR("Memory", "Failed to set memory reporting: " + std::to_string(err));
    } else {
        PYAE_LOG_INFO("Memory", std::string("Memory reporting ") + (enable ? "enabled" : "disabled"));
    }
}

void MemoryDiagnostics::LogMemoryStats(const std::string& context) {
    MemStats stats = GetMemoryStats();

    std::ostringstream oss;
    oss << context << " - "
        << "Handles: " << stats.handleCount << ", "
        << "Size: " << std::fixed << std::setprecision(2)
        << (stats.totalSize / 1024.0) << " KB";

    PYAE_LOG_INFO("Memory", oss.str());
}

bool MemoryDiagnostics::CheckMemoryLeak(
    const MemStats& before,
    const MemStats& after,
    A_long toleranceHandles,
    A_long toleranceSize)
{
    MemStats diff = after - before;

    // 許容範囲内かチェック
    bool handleLeak = diff.handleCount > toleranceHandles;
    bool sizeLeak = diff.totalSize > toleranceSize;

    return handleLeak || sizeLeak;
}

void MemoryDiagnostics::LogMemoryLeakDetails(
    const MemStats& before,
    const MemStats& after,
    const std::string& context)
{
    MemStats diff = after - before;

    std::ostringstream oss;
    oss << context << "\n"
        << "  Before: Handles=" << before.handleCount
        << ", Size=" << before.totalSize << " bytes\n"
        << "  After:  Handles=" << after.handleCount
        << ", Size=" << after.totalSize << " bytes\n"
        << "  Diff:   Handles=" << std::showpos << diff.handleCount
        << ", Size=" << diff.totalSize << std::noshowpos << " bytes";

    if (diff.HasIncreased()) {
        PYAE_LOG_WARNING("MemoryLeak", oss.str());
    } else {
        PYAE_LOG_INFO("MemoryLeak", oss.str() + " (OK)");
    }
}

AEGP_MemSize MemoryDiagnostics::GetMemHandleSize(AEGP_MemHandle memH) {
    if (!memH) {
        return 0;
    }

    auto& state = PluginState::Instance();
    if (!state.IsInitialized()) {
        return 0;
    }

    const auto& suites = state.GetSuites();
    if (!suites.memorySuite) {
        return 0;
    }

    AEGP_MemSize size = 0;
    A_Err err = suites.memorySuite->AEGP_GetMemHandleSize(memH, &size);
    if (err != A_Err_NONE) {
        PYAE_LOG_ERROR("Memory", "Failed to get mem handle size: " + std::to_string(err));
        return 0;
    }

    return size;
}

} // namespace PyAE
