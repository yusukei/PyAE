// PySoundDataClasses.h
// PyAE - Python for After Effects
// サウンドデータクラスの宣言
//
// SoundDataSuite1 の高レベルAPI

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <tuple>
#include <memory>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {

// Forward declarations
class PySoundData;

// =============================================================
// SoundEncoding - サウンドエンコーディング
// =============================================================
enum class SoundEncoding {
    UNSIGNED_PCM = 3,  // Unsigned PCM
    SIGNED_PCM = 4,    // Signed PCM
    FLOAT = 5          // Floating point
};

// =============================================================
// PySoundData - サウンドデータクラス
// AEGP_SoundDataH のラッパー
// =============================================================
class PySoundData {
public:
    // Default constructor (invalid sound data)
    PySoundData();

    // Constructor from raw handle (takes ownership if owned=true)
    explicit PySoundData(AEGP_SoundDataH soundH, bool owned = true);

    // Copy constructor (disabled)
    PySoundData(const PySoundData&) = delete;
    PySoundData& operator=(const PySoundData&) = delete;

    // Move constructor
    PySoundData(PySoundData&& other) noexcept;
    PySoundData& operator=(PySoundData&& other) noexcept;

    // Destructor - disposes the sound data if owned
    ~PySoundData();

    // =============================================================
    // State check
    // =============================================================

    bool IsValid() const;

    // =============================================================
    // Format properties (read-only)
    // =============================================================

    double GetSampleRate() const;
    SoundEncoding GetEncoding() const;
    int GetBytesPerSample() const;
    int GetNumChannels() const;
    int GetNumSamples() const;

    // Get duration in seconds
    double GetDuration() const;

    // Get format as dictionary
    py::dict GetFormat() const;

    // =============================================================
    // Sample data access
    // =============================================================

    // Get samples as Python bytes (copies data, automatically unlocks)
    py::bytes GetSamples() const;

    // Set samples from Python bytes
    void SetSamples(const py::bytes& data);

    // =============================================================
    // Context manager support (lock/unlock)
    // =============================================================

    PySoundData* Enter();
    void Exit(py::object exc_type, py::object exc_val, py::object exc_tb);

    // =============================================================
    // Handle access
    // =============================================================

    AEGP_SoundDataH GetHandle() const;
    AEGP_SoundDataH Release();
    bool IsOwned() const;

    // =============================================================
    // Static factory method
    // =============================================================

    // Create new sound data with specified format
    static std::shared_ptr<PySoundData> Create(
        double sampleRate,
        SoundEncoding encoding,
        int bytesPerSample,
        int numChannels);

private:
    AEGP_SoundDataH m_soundH;
    bool m_owned;
    bool m_locked;

    void Dispose();
    void* LockSamples();
    void UnlockSamples();
};

// =============================================================
// Module init function declaration
// =============================================================
void init_sound_data(py::module_& m);

} // namespace PyAE
