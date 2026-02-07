// SoundDataSuite.cpp
// PyAE - Python for After Effects
// AEGP_SoundDataSuite1 Python Bindings
//
// Provides Python bindings for After Effects Sound Data Suite.
// This suite manages sound data buffers for audio processing.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_SoundDataSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_SoundDataSuite1 - Sound Data Management Functions
    // -----------------------------------------------------------------------

    // 1. AEGP_NewSoundData
    // Creates a new sound data buffer with the specified format
    sdk.def("AEGP_NewSoundData", [](double sample_rate, int encoding, int bytes_per_sample, int num_channels) -> uintptr_t {
        // Validate arguments
        Validation::RequirePositive(sample_rate, "sample_rate");
        Validation::RequireRange(encoding, 3, 5, "encoding");  // UNSIGNED_PCM(3), SIGNED_PCM(4), FLOAT(5)
        Validation::RequireRange(bytes_per_sample, 1, 4, "bytes_per_sample");
        Validation::RequireRange(num_channels, 1, 2, "num_channels");  // 1 for mono, 2 for stereo

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

        AEGP_SoundDataFormat format;
        format.sample_rateF = static_cast<A_FpLong>(sample_rate);
        format.encoding = static_cast<AEGP_SoundEncoding>(encoding);
        format.bytes_per_sampleL = static_cast<A_long>(bytes_per_sample);
        format.num_channelsL = static_cast<A_long>(num_channels);

        AEGP_SoundDataH soundH = nullptr;
        A_Err err = suites.soundDataSuite->AEGP_NewSoundData(&format, &soundH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_NewSoundData failed with error: " + std::to_string(err));

        // Note: soundH can be NULL if no audio
        return reinterpret_cast<uintptr_t>(soundH);
    }, py::arg("sample_rate"), py::arg("encoding"), py::arg("bytes_per_sample"), py::arg("num_channels"),
       "Create a new sound data buffer. Returns AEGP_SoundDataH (0 if no audio). "
       "Must be disposed with AEGP_DisposeSoundData.\n"
       "sample_rate: Sample rate in Hz (e.g., 44100.0, 48000.0)\n"
       "encoding: AEGP_SoundEncoding_UNSIGNED_PCM(3), AEGP_SoundEncoding_SIGNED_PCM(4), or AEGP_SoundEncoding_FLOAT(5)\n"
       "bytes_per_sample: 1, 2, or 4 (ignored if encoding is FLOAT)\n"
       "num_channels: 1 for mono, 2 for stereo");

    // 2. AEGP_DisposeSoundData
    // Disposes a sound data buffer
    sdk.def("AEGP_DisposeSoundData", [](uintptr_t sound_dataH_ptr) {
        if (sound_dataH_ptr == 0) return; // Silently ignore null handles

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

        AEGP_SoundDataH soundH = reinterpret_cast<AEGP_SoundDataH>(sound_dataH_ptr);

        A_Err err = suites.soundDataSuite->AEGP_DisposeSoundData(soundH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DisposeSoundData failed with error: " + std::to_string(err));
    }, py::arg("sound_dataH"),
       "Dispose a sound data buffer created with AEGP_NewSoundData");

    // 3. AEGP_GetSoundDataFormat
    // Gets the format of a sound data buffer
    sdk.def("AEGP_GetSoundDataFormat", [](uintptr_t soundH_ptr) -> py::tuple {
        Validation::RequireNonNull(soundH_ptr, "soundH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

        AEGP_SoundDataH soundH = reinterpret_cast<AEGP_SoundDataH>(soundH_ptr);
        AEGP_SoundDataFormat format;
        memset(&format, 0, sizeof(format));

        A_Err err = suites.soundDataSuite->AEGP_GetSoundDataFormat(soundH, &format);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetSoundDataFormat failed with error: " + std::to_string(err));

        return py::make_tuple(
            static_cast<double>(format.sample_rateF),
            static_cast<int>(format.encoding),
            static_cast<int>(format.bytes_per_sampleL),
            static_cast<int>(format.num_channelsL)
        );
    }, py::arg("soundH"),
       "Get the format of a sound data buffer.\n"
       "Returns tuple: (sample_rate, encoding, bytes_per_sample, num_channels)");

    // 4. AEGP_LockSoundDataSamples
    // Locks and returns the sample data as bytes
    sdk.def("AEGP_LockSoundDataSamples", [](uintptr_t soundH_ptr) -> py::tuple {
        Validation::RequireNonNull(soundH_ptr, "soundH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

        AEGP_SoundDataH soundH = reinterpret_cast<AEGP_SoundDataH>(soundH_ptr);
        void* samples = nullptr;

        A_Err err = suites.soundDataSuite->AEGP_LockSoundDataSamples(soundH, &samples);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_LockSoundDataSamples failed with error: " + std::to_string(err));

        // Get the format to calculate buffer size
        AEGP_SoundDataFormat format;
        memset(&format, 0, sizeof(format));
        err = suites.soundDataSuite->AEGP_GetSoundDataFormat(soundH, &format);
        if (err != A_Err_NONE) {
            suites.soundDataSuite->AEGP_UnlockSoundDataSamples(soundH);
            throw std::runtime_error("AEGP_GetSoundDataFormat failed during lock");
        }

        // Get number of samples
        A_long num_samples = 0;
        err = suites.soundDataSuite->AEGP_GetNumSamples(soundH, &num_samples);
        if (err != A_Err_NONE) {
            suites.soundDataSuite->AEGP_UnlockSoundDataSamples(soundH);
            throw std::runtime_error("AEGP_GetNumSamples failed during lock");
        }

        // Calculate total buffer size
        // For FLOAT encoding, bytes_per_sample is ignored and always 4 (sizeof FpShort)
        int bytes_per_sample = format.bytes_per_sampleL;
        if (format.encoding == AEGP_SoundEncoding_FLOAT) {
            bytes_per_sample = 4; // FpShort is 4 bytes
        }
        size_t buffer_size = static_cast<size_t>(num_samples) * static_cast<size_t>(format.num_channelsL) * static_cast<size_t>(bytes_per_sample);

        // Return pointer as uintptr_t and buffer size
        // Caller must call AEGP_UnlockSoundDataSamples when done
        return py::make_tuple(
            reinterpret_cast<uintptr_t>(samples),
            static_cast<uint64_t>(buffer_size)
        );
    }, py::arg("soundH"),
       "Lock sound data samples for reading/writing.\n"
       "Returns tuple: (samples_ptr, buffer_size_bytes)\n"
       "IMPORTANT: Must call AEGP_UnlockSoundDataSamples when done!");

    // 4b. AEGP_LockSoundDataSamplesAsBytes
    // Locks and returns a copy of the sample data as Python bytes
    sdk.def("AEGP_LockSoundDataSamplesAsBytes", [](uintptr_t soundH_ptr) -> py::bytes {
        Validation::RequireNonNull(soundH_ptr, "soundH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

        AEGP_SoundDataH soundH = reinterpret_cast<AEGP_SoundDataH>(soundH_ptr);
        void* samples = nullptr;

        A_Err err = suites.soundDataSuite->AEGP_LockSoundDataSamples(soundH, &samples);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_LockSoundDataSamples failed with error: " + std::to_string(err));

        // Get the format to calculate buffer size
        AEGP_SoundDataFormat format;
        memset(&format, 0, sizeof(format));
        err = suites.soundDataSuite->AEGP_GetSoundDataFormat(soundH, &format);
        if (err != A_Err_NONE) {
            suites.soundDataSuite->AEGP_UnlockSoundDataSamples(soundH);
            throw std::runtime_error("AEGP_GetSoundDataFormat failed during lock");
        }

        // Get number of samples
        A_long num_samples = 0;
        err = suites.soundDataSuite->AEGP_GetNumSamples(soundH, &num_samples);
        if (err != A_Err_NONE) {
            suites.soundDataSuite->AEGP_UnlockSoundDataSamples(soundH);
            throw std::runtime_error("AEGP_GetNumSamples failed during lock");
        }

        // Calculate total buffer size
        int bytes_per_sample = format.bytes_per_sampleL;
        if (format.encoding == AEGP_SoundEncoding_FLOAT) {
            bytes_per_sample = 4;
        }
        size_t buffer_size = static_cast<size_t>(num_samples) * static_cast<size_t>(format.num_channelsL) * static_cast<size_t>(bytes_per_sample);

        // Copy data to Python bytes
        py::bytes result(reinterpret_cast<const char*>(samples), buffer_size);

        // Unlock immediately after copying
        suites.soundDataSuite->AEGP_UnlockSoundDataSamples(soundH);

        return result;
    }, py::arg("soundH"),
       "Lock sound data samples and return as Python bytes.\n"
       "The samples are automatically unlocked after copying.\n"
       "Use this for safe read-only access to audio data.");

    // 5. AEGP_UnlockSoundDataSamples
    // Unlocks the sample data
    sdk.def("AEGP_UnlockSoundDataSamples", [](uintptr_t soundH_ptr) {
        Validation::RequireNonNull(soundH_ptr, "soundH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

        AEGP_SoundDataH soundH = reinterpret_cast<AEGP_SoundDataH>(soundH_ptr);

        A_Err err = suites.soundDataSuite->AEGP_UnlockSoundDataSamples(soundH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_UnlockSoundDataSamples failed with error: " + std::to_string(err));
    }, py::arg("soundH"),
       "Unlock sound data samples after reading/writing");

    // 6. AEGP_GetNumSamples
    // Gets the number of samples in the sound data buffer
    sdk.def("AEGP_GetNumSamples", [](uintptr_t soundH_ptr) -> int {
        Validation::RequireNonNull(soundH_ptr, "soundH");

        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

        AEGP_SoundDataH soundH = reinterpret_cast<AEGP_SoundDataH>(soundH_ptr);
        A_long num_samples = 0;

        A_Err err = suites.soundDataSuite->AEGP_GetNumSamples(soundH, &num_samples);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumSamples failed with error: " + std::to_string(err));

        return static_cast<int>(num_samples);
    }, py::arg("soundH"),
       "Get the number of samples in the sound data buffer.\n"
       "For stereo, this is per-channel (total samples = num_samples * num_channels)");

    // -----------------------------------------------------------------------
    // Constants for AEGP_SoundEncoding
    // -----------------------------------------------------------------------

    // AEGP_SoundEncoding values (from AE SDK)
    // Note: Values start at 3, not 1!
    sdk.attr("AEGP_SoundEncoding_UNSIGNED_PCM") = py::int_(static_cast<int>(AEGP_SoundEncoding_UNSIGNED_PCM));  // 3
    sdk.attr("AEGP_SoundEncoding_SIGNED_PCM") = py::int_(static_cast<int>(AEGP_SoundEncoding_SIGNED_PCM));      // 4
    sdk.attr("AEGP_SoundEncoding_FLOAT") = py::int_(static_cast<int>(AEGP_SoundEncoding_FLOAT));                // 5
    sdk.attr("AEGP_SoundEncoding_BEGIN") = py::int_(static_cast<int>(AEGP_SoundEncoding_BEGIN));                // 3
    sdk.attr("AEGP_SoundEncoding_END") = py::int_(static_cast<int>(AEGP_SoundEncoding_END));                    // 6
}

} // namespace SDK
} // namespace PyAE
