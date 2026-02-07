// PySoundData.cpp
// PyAE - Python for After Effects
// サウンドデータクラスの実装
//
// SoundDataSuite1 の高レベルAPI

#include "PySoundDataClasses.h"
#include "PluginState.h"
#include "ValidationUtils.h"

#include <cstring>

namespace PyAE {

// =============================================================
// PySoundData Implementation
// =============================================================

PySoundData::PySoundData()
    : m_soundH(nullptr), m_owned(false), m_locked(false) {}

PySoundData::PySoundData(AEGP_SoundDataH soundH, bool owned)
    : m_soundH(soundH), m_owned(owned), m_locked(false) {}

PySoundData::PySoundData(PySoundData&& other) noexcept
    : m_soundH(other.m_soundH), m_owned(other.m_owned), m_locked(other.m_locked)
{
    other.m_soundH = nullptr;
    other.m_owned = false;
    other.m_locked = false;
}

PySoundData& PySoundData::operator=(PySoundData&& other) noexcept
{
    if (this != &other) {
        Dispose();
        m_soundH = other.m_soundH;
        m_owned = other.m_owned;
        m_locked = other.m_locked;
        other.m_soundH = nullptr;
        other.m_owned = false;
        other.m_locked = false;
    }
    return *this;
}

PySoundData::~PySoundData()
{
    Dispose();
}

void PySoundData::Dispose()
{
    if (m_soundH) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.soundDataSuite) {
            // Unlock if still locked
            if (m_locked) {
                suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);
                m_locked = false;
            }
            // Dispose if owned
            if (m_owned) {
                suites.soundDataSuite->AEGP_DisposeSoundData(m_soundH);
            }
        }
    }
    m_soundH = nullptr;
    m_owned = false;
    m_locked = false;
}

bool PySoundData::IsValid() const
{
    return m_soundH != nullptr;
}

// =============================================================
// Format properties
// =============================================================

double PySoundData::GetSampleRate() const
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    AEGP_SoundDataFormat format;
    std::memset(&format, 0, sizeof(format));

    A_Err err = suites.soundDataSuite->AEGP_GetSoundDataFormat(m_soundH, &format);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetSoundDataFormat failed (error code: " + std::to_string(err) + ")");
    }

    return static_cast<double>(format.sample_rateF);
}

SoundEncoding PySoundData::GetEncoding() const
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    AEGP_SoundDataFormat format;
    std::memset(&format, 0, sizeof(format));

    A_Err err = suites.soundDataSuite->AEGP_GetSoundDataFormat(m_soundH, &format);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetSoundDataFormat failed (error code: " + std::to_string(err) + ")");
    }

    return static_cast<SoundEncoding>(format.encoding);
}

int PySoundData::GetBytesPerSample() const
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    AEGP_SoundDataFormat format;
    std::memset(&format, 0, sizeof(format));

    A_Err err = suites.soundDataSuite->AEGP_GetSoundDataFormat(m_soundH, &format);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetSoundDataFormat failed (error code: " + std::to_string(err) + ")");
    }

    // For FLOAT encoding, bytes_per_sample is always 4
    if (format.encoding == AEGP_SoundEncoding_FLOAT) {
        return 4;
    }

    return static_cast<int>(format.bytes_per_sampleL);
}

int PySoundData::GetNumChannels() const
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    AEGP_SoundDataFormat format;
    std::memset(&format, 0, sizeof(format));

    A_Err err = suites.soundDataSuite->AEGP_GetSoundDataFormat(m_soundH, &format);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetSoundDataFormat failed (error code: " + std::to_string(err) + ")");
    }

    return static_cast<int>(format.num_channelsL);
}

int PySoundData::GetNumSamples() const
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    A_long numSamples = 0;
    A_Err err = suites.soundDataSuite->AEGP_GetNumSamples(m_soundH, &numSamples);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetNumSamples failed (error code: " + std::to_string(err) + ")");
    }

    return static_cast<int>(numSamples);
}

double PySoundData::GetDuration() const
{
    double sampleRate = GetSampleRate();
    int numSamples = GetNumSamples();

    if (sampleRate <= 0) return 0.0;
    return static_cast<double>(numSamples) / sampleRate;
}

py::dict PySoundData::GetFormat() const
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    AEGP_SoundDataFormat format;
    std::memset(&format, 0, sizeof(format));

    A_Err err = suites.soundDataSuite->AEGP_GetSoundDataFormat(m_soundH, &format);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetSoundDataFormat failed (error code: " + std::to_string(err) + ")");
    }

    int bytesPerSample = static_cast<int>(format.bytes_per_sampleL);
    if (format.encoding == AEGP_SoundEncoding_FLOAT) {
        bytesPerSample = 4;
    }

    py::dict result;
    result["sample_rate"] = static_cast<double>(format.sample_rateF);
    result["encoding"] = static_cast<int>(format.encoding);
    result["bytes_per_sample"] = bytesPerSample;
    result["num_channels"] = static_cast<int>(format.num_channelsL);
    return result;
}

// =============================================================
// Sample data access
// =============================================================

void* PySoundData::LockSamples()
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");
    if (m_locked) throw std::runtime_error("SoundData already locked");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    void* samples = nullptr;
    A_Err err = suites.soundDataSuite->AEGP_LockSoundDataSamples(m_soundH, &samples);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_LockSoundDataSamples failed (error code: " + std::to_string(err) + ")");
    }

    m_locked = true;
    return samples;
}

void PySoundData::UnlockSamples()
{
    if (!m_soundH || !m_locked) return;

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (suites.soundDataSuite) {
        suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);
    }
    m_locked = false;
}

py::bytes PySoundData::GetSamples() const
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    // Lock samples
    void* samples = nullptr;
    A_Err err = suites.soundDataSuite->AEGP_LockSoundDataSamples(m_soundH, &samples);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_LockSoundDataSamples failed (error code: " + std::to_string(err) + ")");
    }

    // Get format
    AEGP_SoundDataFormat format;
    std::memset(&format, 0, sizeof(format));
    err = suites.soundDataSuite->AEGP_GetSoundDataFormat(m_soundH, &format);
    if (err != A_Err_NONE) {
        suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);
        throw std::runtime_error("AEGP_GetSoundDataFormat failed (error code: " + std::to_string(err) + ")");
    }

    // Get number of samples
    A_long numSamples = 0;
    err = suites.soundDataSuite->AEGP_GetNumSamples(m_soundH, &numSamples);
    if (err != A_Err_NONE) {
        suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);
        throw std::runtime_error("AEGP_GetNumSamples failed (error code: " + std::to_string(err) + ")");
    }

    // Calculate buffer size
    int bytesPerSample = static_cast<int>(format.bytes_per_sampleL);
    if (format.encoding == AEGP_SoundEncoding_FLOAT) {
        bytesPerSample = 4;
    }
    size_t bufferSize = static_cast<size_t>(numSamples) *
                        static_cast<size_t>(format.num_channelsL) *
                        static_cast<size_t>(bytesPerSample);

    // Copy data
    py::bytes result(reinterpret_cast<const char*>(samples), bufferSize);

    // Unlock
    suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);

    return result;
}

void PySoundData::SetSamples(const py::bytes& data)
{
    if (!m_soundH) throw std::runtime_error("Invalid SoundData");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    // Lock samples
    void* samples = nullptr;
    A_Err err = suites.soundDataSuite->AEGP_LockSoundDataSamples(m_soundH, &samples);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_LockSoundDataSamples failed (error code: " + std::to_string(err) + ")");
    }

    // Get format
    AEGP_SoundDataFormat format;
    std::memset(&format, 0, sizeof(format));
    err = suites.soundDataSuite->AEGP_GetSoundDataFormat(m_soundH, &format);
    if (err != A_Err_NONE) {
        suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);
        throw std::runtime_error("AEGP_GetSoundDataFormat failed (error code: " + std::to_string(err) + ")");
    }

    // Get number of samples
    A_long numSamples = 0;
    err = suites.soundDataSuite->AEGP_GetNumSamples(m_soundH, &numSamples);
    if (err != A_Err_NONE) {
        suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);
        throw std::runtime_error("AEGP_GetNumSamples failed (error code: " + std::to_string(err) + ")");
    }

    // Calculate expected buffer size
    int bytesPerSample = static_cast<int>(format.bytes_per_sampleL);
    if (format.encoding == AEGP_SoundEncoding_FLOAT) {
        bytesPerSample = 4;
    }
    size_t expectedSize = static_cast<size_t>(numSamples) *
                          static_cast<size_t>(format.num_channelsL) *
                          static_cast<size_t>(bytesPerSample);

    // Get data from Python bytes
    std::string dataStr = static_cast<std::string>(data);
    if (dataStr.size() != expectedSize) {
        suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);
        throw std::runtime_error("Data size mismatch: expected " + std::to_string(expectedSize) +
                                 " bytes, got " + std::to_string(dataStr.size()));
    }

    // Copy data
    std::memcpy(samples, dataStr.data(), expectedSize);

    // Unlock
    suites.soundDataSuite->AEGP_UnlockSoundDataSamples(m_soundH);
}

// =============================================================
// Context manager support
// =============================================================

PySoundData* PySoundData::Enter()
{
    LockSamples();
    return this;
}

void PySoundData::Exit(py::object /*exc_type*/, py::object /*exc_val*/, py::object /*exc_tb*/)
{
    UnlockSamples();
}

// =============================================================
// Handle access
// =============================================================

AEGP_SoundDataH PySoundData::GetHandle() const
{
    return m_soundH;
}

AEGP_SoundDataH PySoundData::Release()
{
    // Unlock if locked
    if (m_locked) {
        UnlockSamples();
    }

    AEGP_SoundDataH h = m_soundH;
    m_soundH = nullptr;
    m_owned = false;
    return h;
}

bool PySoundData::IsOwned() const
{
    return m_owned;
}

// =============================================================
// Static factory method
// =============================================================

std::shared_ptr<PySoundData> PySoundData::Create(
    double sampleRate,
    SoundEncoding encoding,
    int bytesPerSample,
    int numChannels)
{
    Validation::RequirePositive(sampleRate, "sampleRate");
    Validation::RequireRange(static_cast<int>(encoding), 3, 5, "encoding");
    Validation::RequireRange(bytesPerSample, 1, 4, "bytesPerSample");
    Validation::RequireRange(numChannels, 1, 2, "numChannels");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.soundDataSuite) throw std::runtime_error("SoundData Suite not available");

    AEGP_SoundDataFormat format;
    format.sample_rateF = static_cast<A_FpLong>(sampleRate);
    format.encoding = static_cast<AEGP_SoundEncoding>(encoding);
    format.bytes_per_sampleL = static_cast<A_long>(bytesPerSample);
    format.num_channelsL = static_cast<A_long>(numChannels);

    AEGP_SoundDataH soundH = nullptr;
    A_Err err = suites.soundDataSuite->AEGP_NewSoundData(&format, &soundH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewSoundData failed (error code: " + std::to_string(err) + ")");
    }

    return std::make_shared<PySoundData>(soundH, true);
}

// =============================================================
// Module init
// =============================================================

void init_sound_data(py::module_& m)
{
    // SoundEncoding enum
    py::enum_<SoundEncoding>(m, "SoundEncoding",
        "Sound encoding format.")
        .value("UNSIGNED_PCM", SoundEncoding::UNSIGNED_PCM, "Unsigned PCM")
        .value("SIGNED_PCM", SoundEncoding::SIGNED_PCM, "Signed PCM")
        .value("FLOAT", SoundEncoding::FLOAT, "Floating point");

    // SoundData class
    py::class_<PySoundData, std::shared_ptr<PySoundData>>(m, "SoundData",
        R"doc(Sound data buffer for audio processing.

Provides access to audio sample data with support for multiple
formats including PCM and floating point.

Example::

    # Create sound data
    sound = ae.SoundData.create(
        sample_rate=48000.0,
        encoding=ae.SoundEncoding.FLOAT,
        bytes_per_sample=4,
        num_channels=2
    )

    # Get format info
    print(f"Duration: {sound.duration}s")
    print(f"Sample rate: {sound.sample_rate}")

    # Access samples
    samples = sound.get_samples()

    # Or use context manager for lock/unlock
    with sound:
        # Sound is locked here
        pass
)doc")
        .def(py::init<>())
        .def_property_readonly("valid", &PySoundData::IsValid,
            "Check if the sound data handle is valid.")

        // Format properties
        .def_property_readonly("sample_rate", &PySoundData::GetSampleRate,
            "Sample rate in Hz (e.g., 44100.0, 48000.0).")
        .def_property_readonly("encoding", &PySoundData::GetEncoding,
            "Sound encoding format.")
        .def_property_readonly("bytes_per_sample", &PySoundData::GetBytesPerSample,
            "Bytes per sample (1, 2, or 4).")
        .def_property_readonly("num_channels", &PySoundData::GetNumChannels,
            "Number of channels (1 for mono, 2 for stereo).")
        .def_property_readonly("num_samples", &PySoundData::GetNumSamples,
            "Number of samples per channel.")
        .def_property_readonly("duration", &PySoundData::GetDuration,
            "Duration in seconds.")
        .def_property_readonly("format", &PySoundData::GetFormat,
            "Get format as dictionary with sample_rate, encoding, bytes_per_sample, num_channels.")

        // Sample data access
        .def("get_samples", &PySoundData::GetSamples,
            R"doc(Get sample data as Python bytes.

Returns:
    bytes: Raw sample data

Note:
    The data is automatically copied and the buffer is unlocked after.
    For stereo audio, samples are interleaved (L, R, L, R, ...).
)doc")
        .def("set_samples", &PySoundData::SetSamples,
            py::arg("data"),
            R"doc(Set sample data from Python bytes.

Args:
    data: Raw sample data

Note:
    The data size must match exactly: num_samples * num_channels * bytes_per_sample.
    For stereo audio, samples should be interleaved (L, R, L, R, ...).
)doc")

        // Context manager
        .def("__enter__", &PySoundData::Enter)
        .def("__exit__", &PySoundData::Exit)

        // Handle access
        .def_property_readonly("_handle",
            [](const PySoundData& self) { return reinterpret_cast<uintptr_t>(self.GetHandle()); },
            "Internal: Get raw handle as integer.")

        // Static factory method
        .def_static("create", &PySoundData::Create,
            py::arg("sample_rate"),
            py::arg("encoding"),
            py::arg("bytes_per_sample"),
            py::arg("num_channels"),
            R"doc(Create a new sound data buffer.

Args:
    sample_rate: Sample rate in Hz (e.g., 44100.0, 48000.0)
    encoding: SoundEncoding value (UNSIGNED_PCM, SIGNED_PCM, or FLOAT)
    bytes_per_sample: 1, 2, or 4 (ignored if encoding is FLOAT)
    num_channels: 1 for mono, 2 for stereo

Returns:
    New SoundData instance
)doc");
}

} // namespace PyAE
