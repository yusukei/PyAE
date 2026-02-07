#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "../ValidationUtils.h"

// AE SDK Headers
#include "AE_GeneralPlug.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_PersistentDataSuite(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_PersistentDataSuite4 - Application Preferences and Persistent Data
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Blob and Section Management
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetApplicationBlob", [](int blob_type) -> uintptr_t {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        AEGP_PersistentBlobH blobH = nullptr;
        A_Err err = suites.persistentDataSuite->AEGP_GetApplicationBlob(
            static_cast<AEGP_PersistentType>(blob_type), &blobH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetApplicationBlob failed");

        return reinterpret_cast<uintptr_t>(blobH);
    }, py::arg("blob_type"),
       "Get application blob handle. blob_type: AEGP_PersistentType_* constant");

    sdk.def("AEGP_GetNumSections", [](uintptr_t blobH_ptr) -> int {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_long num_sections = 0;
        A_Err err = suites.persistentDataSuite->AEGP_GetNumSections(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr), &num_sections);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumSections failed");

        return static_cast<int>(num_sections);
    }, py::arg("blobH"),
       "Get the number of sections in a blob");

    sdk.def("AEGP_GetSectionKeyByIndex", [](uintptr_t blobH_ptr, int section_index, int max_size) -> std::string {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonNegative(section_index, "section_index");
        PyAE::Validation::RequirePositive(max_size, "max_size");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        std::vector<A_char> buffer(max_size + 1, 0);
        A_Err err = suites.persistentDataSuite->AEGP_GetSectionKeyByIndex(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            static_cast<A_long>(section_index),
            static_cast<A_long>(max_size),
            buffer.data());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetSectionKeyByIndex failed");

        // Filter out invalid UTF-8 sequences by keeping only valid ASCII characters
        // Section keys should typically be ASCII-only identifiers
        std::string result;
        for (size_t i = 0; i < max_size && buffer[i] != '\0'; ++i) {
            unsigned char c = static_cast<unsigned char>(buffer[i]);
            // Only keep printable ASCII characters (0x20-0x7E) plus common control chars
            if ((c >= 0x20 && c <= 0x7E) || c == '\t' || c == '\n' || c == '\r') {
                result.push_back(buffer[i]);
            }
            // Skip invalid or non-ASCII bytes to avoid UTF-8 decode errors
        }
        return result;
    }, py::arg("blobH"), py::arg("section_index"), py::arg("max_size") = 256,
       "Get section key by index");

    sdk.def("AEGP_DoesKeyExist", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key) -> bool {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_Boolean exists = FALSE;
        A_Err err = suites.persistentDataSuite->AEGP_DoesKeyExist(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            &exists);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DoesKeyExist failed");

        return exists != FALSE;
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"),
       "Check if a key exists in the blob");

    sdk.def("AEGP_GetNumKeys", [](uintptr_t blobH_ptr, const std::string& section_key) -> int {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_long num_keys = 0;
        A_Err err = suites.persistentDataSuite->AEGP_GetNumKeys(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            &num_keys);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetNumKeys failed");

        return static_cast<int>(num_keys);
    }, py::arg("blobH"), py::arg("section_key"),
       "Get the number of keys in a section");

    sdk.def("AEGP_GetValueKeyByIndex", [](uintptr_t blobH_ptr, const std::string& section_key, int key_index, int max_size) -> std::string {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonNegative(key_index, "key_index");
        PyAE::Validation::RequirePositive(max_size, "max_size");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        std::vector<A_char> buffer(max_size + 1, 0);
        A_Err err = suites.persistentDataSuite->AEGP_GetValueKeyByIndex(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            static_cast<A_long>(key_index),
            static_cast<A_long>(max_size),
            buffer.data());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetValueKeyByIndex failed");

        return std::string(buffer.data());
    }, py::arg("blobH"), py::arg("section_key"), py::arg("key_index"), py::arg("max_size") = 256,
       "Get value key by index within a section");

    // -----------------------------------------------------------------------
    // Data Getters
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetDataHandle", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, py::object default_data) -> py::bytes {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID pluginId = state.GetPluginID();

        // Handle default data
        AEGP_MemHandle defaultH = nullptr;
        PyAE::ScopedMemHandle scopedDefault;
        if (!default_data.is_none()) {
            py::bytes defaultBytes = default_data.cast<py::bytes>();
            std::string defaultStr = defaultBytes;
            A_Err allocErr = suites.memorySuite->AEGP_NewMemHandle(
                pluginId, "Default", static_cast<AEGP_MemSize>(defaultStr.size()), AEGP_MemFlag_CLEAR, &defaultH);
            if (allocErr == A_Err_NONE && defaultH) {
                scopedDefault = PyAE::ScopedMemHandle(pluginId, suites.memorySuite, defaultH);
                void* ptr = nullptr;
                A_Err lockErr = suites.memorySuite->AEGP_LockMemHandle(defaultH, &ptr);
                if (lockErr == A_Err_NONE && ptr) {
                    memcpy(ptr, defaultStr.data(), defaultStr.size());
                    suites.memorySuite->AEGP_UnlockMemHandle(defaultH);
                }
            }
        }

        AEGP_MemHandle valueH = nullptr;
        A_Err err = suites.persistentDataSuite->AEGP_GetDataHandle(
            pluginId,
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            defaultH,
            &valueH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetDataHandle failed");

        if (!valueH) {
            return py::bytes();
        }

        // Read data from handle
        PyAE::ScopedMemHandle scopedValue(pluginId, suites.memorySuite, valueH);
        AEGP_MemSize size = 0;
        suites.memorySuite->AEGP_GetMemHandleSize(valueH, &size);

        PyAE::ScopedMemLock lock(suites.memorySuite, valueH);
        if (!lock.IsValid()) {
            throw std::runtime_error("Failed to lock memory handle");
        }

        return py::bytes(static_cast<const char*>(lock.Get()), size);
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("default_data") = py::none(),
       "Get binary data as bytes. Returns default if key not found.");

    sdk.def("AEGP_GetData", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, int data_size, py::object default_data) -> py::bytes {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        PyAE::Validation::RequirePositive(data_size, "data_size");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        std::vector<char> buffer(data_size, 0);
        const void* defaultPtr = nullptr;
        std::string defaultStr;

        if (!default_data.is_none()) {
            py::bytes defaultBytes = default_data.cast<py::bytes>();
            defaultStr = defaultBytes;
            defaultPtr = defaultStr.data();
        }

        A_Err err = suites.persistentDataSuite->AEGP_GetData(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            static_cast<A_u_long>(data_size),
            defaultPtr,
            buffer.data());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetData failed");

        return py::bytes(buffer.data(), data_size);
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("data_size"), py::arg("default_data") = py::none(),
       "Get fixed-size binary data. Returns default if key not found or size mismatch.");

    sdk.def("AEGP_GetString", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, const std::string& default_value, int buf_size) -> std::string {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        PyAE::Validation::RequirePositive(buf_size, "buf_size");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        std::vector<A_char> buffer(buf_size + 1, 0);
        A_u_long actual_size = 0;

        A_Err err = suites.persistentDataSuite->AEGP_GetString(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            default_value.empty() ? nullptr : default_value.c_str(),
            static_cast<A_u_long>(buf_size),
            buffer.data(),
            &actual_size);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetString failed");

        return std::string(buffer.data());
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("default_value") = "", py::arg("buf_size") = 1024,
       "Get string value. Returns default if key not found.");

    sdk.def("AEGP_GetLong", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, int default_value) -> int {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_long value = 0;
        A_Err err = suites.persistentDataSuite->AEGP_GetLong(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            static_cast<A_long>(default_value),
            &value);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetLong failed");

        return static_cast<int>(value);
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("default_value") = 0,
       "Get integer value. Returns default if key not found.");

    sdk.def("AEGP_GetFpLong", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, double default_value) -> double {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_FpLong value = 0.0;
        A_Err err = suites.persistentDataSuite->AEGP_GetFpLong(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            static_cast<A_FpLong>(default_value),
            &value);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetFpLong failed");

        return static_cast<double>(value);
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("default_value") = 0.0,
       "Get floating-point value. Returns default if key not found.");

    sdk.def("AEGP_GetTime", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, py::object default_time) -> py::tuple {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_Time defaultT = {0, 1};
        const A_Time* defaultPtr = nullptr;
        if (!default_time.is_none()) {
            py::tuple t = default_time.cast<py::tuple>();
            defaultT.value = t[0].cast<A_long>();
            defaultT.scale = t[1].cast<A_u_long>();
            defaultPtr = &defaultT;
        }

        A_Time value = {0, 1};
        A_Err err = suites.persistentDataSuite->AEGP_GetTime(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            defaultPtr,
            &value);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetTime failed");

        return py::make_tuple(value.value, static_cast<int>(value.scale));
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("default_time") = py::none(),
       "Get time value as (value, scale) tuple. Returns default if key not found.");

    sdk.def("AEGP_GetARGB", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, py::object default_color) -> py::tuple {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        PF_PixelFloat defaultC = {1.0f, 0.0f, 0.0f, 0.0f}; // alpha, red, green, blue
        const PF_PixelFloat* defaultPtr = nullptr;
        if (!default_color.is_none()) {
            py::tuple c = default_color.cast<py::tuple>();
            defaultC.alpha = c[0].cast<float>();
            defaultC.red = c[1].cast<float>();
            defaultC.green = c[2].cast<float>();
            defaultC.blue = c[3].cast<float>();
            defaultPtr = &defaultC;
        }

        PF_PixelFloat value = {0.0f, 0.0f, 0.0f, 0.0f};
        A_Err err = suites.persistentDataSuite->AEGP_GetARGB(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            defaultPtr,
            &value);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetARGB failed");

        return py::make_tuple(
            static_cast<float>(value.alpha),
            static_cast<float>(value.red),
            static_cast<float>(value.green),
            static_cast<float>(value.blue)
        );
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("default_color") = py::none(),
       "Get ARGB color as (alpha, red, green, blue) tuple. Returns default if key not found.");

    // -----------------------------------------------------------------------
    // Data Setters
    // -----------------------------------------------------------------------

    sdk.def("AEGP_SetDataHandle", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, py::bytes data) {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID pluginId = state.GetPluginID();
        std::string dataStr = data;

        AEGP_MemHandle valueH = nullptr;
        A_Err allocErr = suites.memorySuite->AEGP_NewMemHandle(
            pluginId, "SetData", static_cast<AEGP_MemSize>(dataStr.size()), AEGP_MemFlag_CLEAR, &valueH);
        if (allocErr != A_Err_NONE || !valueH) {
            throw std::runtime_error("Failed to allocate memory handle");
        }

        PyAE::ScopedMemHandle scopedValue(pluginId, suites.memorySuite, valueH);
        {
            PyAE::ScopedMemLock lock(suites.memorySuite, valueH);
            if (!lock.IsValid()) {
                throw std::runtime_error("Failed to lock memory handle");
            }
            memcpy(lock.Get(), dataStr.data(), dataStr.size());
        }

        A_Err err = suites.persistentDataSuite->AEGP_SetDataHandle(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            valueH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetDataHandle failed");
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("data"),
       "Set binary data from bytes");

    sdk.def("AEGP_SetData", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, py::bytes data) {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        std::string dataStr = data;

        A_Err err = suites.persistentDataSuite->AEGP_SetData(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            static_cast<A_u_long>(dataStr.size()),
            dataStr.data());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetData failed");
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("data"),
       "Set fixed-size binary data from bytes");

    sdk.def("AEGP_SetString", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, const std::string& value) {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_Err err = suites.persistentDataSuite->AEGP_SetString(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            value.c_str());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetString failed");
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("value"),
       "Set string value");

    sdk.def("AEGP_SetLong", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, int value) {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_Err err = suites.persistentDataSuite->AEGP_SetLong(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            static_cast<A_long>(value));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetLong failed");
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("value"),
       "Set integer value");

    sdk.def("AEGP_SetFpLong", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, double value) {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_Err err = suites.persistentDataSuite->AEGP_SetFpLong(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            static_cast<A_FpLong>(value));
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetFpLong failed");
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("value"),
       "Set floating-point value");

    sdk.def("AEGP_SetTime", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, py::tuple time_tuple) {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_Time value;
        value.value = time_tuple[0].cast<A_long>();
        value.scale = time_tuple[1].cast<A_u_long>();

        A_Err err = suites.persistentDataSuite->AEGP_SetTime(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            &value);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetTime failed");
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("time_tuple"),
       "Set time value from (value, scale) tuple");

    sdk.def("AEGP_SetARGB", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key, py::tuple color_tuple) {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        PF_PixelFloat value;
        value.alpha = color_tuple[0].cast<float>();
        value.red = color_tuple[1].cast<float>();
        value.green = color_tuple[2].cast<float>();
        value.blue = color_tuple[3].cast<float>();

        A_Err err = suites.persistentDataSuite->AEGP_SetARGB(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str(),
            &value);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_SetARGB failed");
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"), py::arg("color_tuple"),
       "Set ARGB color from (alpha, red, green, blue) tuple");

    // -----------------------------------------------------------------------
    // Entry Management
    // -----------------------------------------------------------------------

    sdk.def("AEGP_DeleteEntry", [](uintptr_t blobH_ptr, const std::string& section_key, const std::string& value_key) {
        if (blobH_ptr == 0) throw std::runtime_error("Invalid blob handle (null)");
        PyAE::Validation::RequireNonEmpty(section_key, "section_key");
        PyAE::Validation::RequireNonEmpty(value_key, "value_key");
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");

        A_Err err = suites.persistentDataSuite->AEGP_DeleteEntry(
            reinterpret_cast<AEGP_PersistentBlobH>(blobH_ptr),
            section_key.c_str(),
            value_key.c_str());
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_DeleteEntry failed");
    }, py::arg("blobH"), py::arg("section_key"), py::arg("value_key"),
       "Delete an entry from the blob. No error if entry not found.");

    // -----------------------------------------------------------------------
    // Preferences Directory
    // -----------------------------------------------------------------------

    sdk.def("AEGP_GetPrefsDirectory", []() -> std::string {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) throw std::runtime_error("PersistentData Suite not available");
        if (!suites.memorySuite) throw std::runtime_error("Memory Suite not available");

        AEGP_PluginID pluginId = state.GetPluginID();
        AEGP_MemHandle pathH = nullptr;

        A_Err err = suites.persistentDataSuite->AEGP_GetPrefsDirectory(&pathH);
        if (err != A_Err_NONE) throw std::runtime_error("AEGP_GetPrefsDirectory failed");

        if (!pathH) {
            return "";
        }

        PyAE::ScopedMemHandle scopedPath(pluginId, suites.memorySuite, pathH);
        PyAE::ScopedMemLock lock(suites.memorySuite, pathH);

        if (!lock.IsValid()) {
            return "";
        }

        return PyAE::StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
    }, "Get the preferences directory path");

    // -----------------------------------------------------------------------
    // AEGP_PersistentType Constants
    // -----------------------------------------------------------------------

    sdk.attr("AEGP_PersistentType_MACHINE_SPECIFIC") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC);
    sdk.attr("AEGP_PersistentType_MACHINE_INDEPENDENT") = static_cast<int>(AEGP_PersistentType_MACHINE_INDEPENDENT);
    sdk.attr("AEGP_PersistentType_MACHINE_INDEPENDENT_RENDER") = static_cast<int>(AEGP_PersistentType_MACHINE_INDEPENDENT_RENDER);
    sdk.attr("AEGP_PersistentType_MACHINE_INDEPENDENT_OUTPUT") = static_cast<int>(AEGP_PersistentType_MACHINE_INDEPENDENT_OUTPUT);
    sdk.attr("AEGP_PersistentType_MACHINE_INDEPENDENT_COMPOSITION") = static_cast<int>(AEGP_PersistentType_MACHINE_INDEPENDENT_COMPOSITION);
    sdk.attr("AEGP_PersistentType_MACHINE_SPECIFIC_TEXT") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC_TEXT);
    sdk.attr("AEGP_PersistentType_MACHINE_SPECIFIC_PAINT") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC_PAINT);
#if defined(kAEGPCompSuiteVersion12)
    // AE 24.0+ only: Additional persistent type constants
    sdk.attr("AEGP_PersistentType_MACHINE_SPECIFIC_EFFECTS") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC_EFFECTS);
    sdk.attr("AEGP_PersistentType_MACHINE_SPECIFIC_EXPRESSION_SNIPPETS") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC_EXPRESSION_SNIPPETS);
    sdk.attr("AEGP_PersistentType_MACHINE_SPECIFIC_SCRIPT_SNIPPETS") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC_SCRIPT_SNIPPETS);
#endif
}

} // namespace SDK
} // namespace PyAE
