// PyPersistentData.cpp
// PyAE - Python for After Effects
// High-level API for persistent data (preferences)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "Logger.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// PyPersistentSection - Section-level access (dict-like)
// =============================================================
class PyPersistentSection {
public:
    PyPersistentSection(AEGP_PersistentBlobH blobH, const std::string& section)
        : m_blobH(blobH), m_section(section) {}

    // Get a string value
    std::string GetString(const std::string& key, const std::string& defaultValue = "") const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite || !m_blobH) {
            return defaultValue;
        }

        A_Boolean exists = FALSE;
        A_Err err = suites.persistentDataSuite->AEGP_DoesKeyExist(
            m_blobH, m_section.c_str(), key.c_str(), &exists);
        if (err != A_Err_NONE || !exists) {
            return defaultValue;
        }

        std::vector<A_char> buffer(4096, 0);
        A_u_long actualSize = 0;
        err = suites.persistentDataSuite->AEGP_GetString(
            m_blobH, m_section.c_str(), key.c_str(),
            defaultValue.c_str(), static_cast<A_u_long>(buffer.size()),
            buffer.data(), &actualSize);
        if (err != A_Err_NONE) {
            return defaultValue;
        }

        return std::string(buffer.data());
    }

    // Set a string value
    void SetString(const std::string& key, const std::string& value) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite || !m_blobH) {
            throw std::runtime_error("PersistentData Suite not available");
        }

        A_Err err = suites.persistentDataSuite->AEGP_SetString(
            m_blobH, m_section.c_str(), key.c_str(), value.c_str());
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set string value");
        }
    }

    // Get an integer value
    int GetInt(const std::string& key, int defaultValue = 0) const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite || !m_blobH) {
            return defaultValue;
        }

        A_long value = 0;
        A_Err err = suites.persistentDataSuite->AEGP_GetLong(
            m_blobH, m_section.c_str(), key.c_str(),
            static_cast<A_long>(defaultValue), &value);
        if (err != A_Err_NONE) {
            return defaultValue;
        }

        return static_cast<int>(value);
    }

    // Set an integer value
    void SetInt(const std::string& key, int value) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite || !m_blobH) {
            throw std::runtime_error("PersistentData Suite not available");
        }

        A_Err err = suites.persistentDataSuite->AEGP_SetLong(
            m_blobH, m_section.c_str(), key.c_str(), static_cast<A_long>(value));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set integer value");
        }
    }

    // Get a float value
    double GetFloat(const std::string& key, double defaultValue = 0.0) const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite || !m_blobH) {
            return defaultValue;
        }

        A_FpLong value = 0.0;
        A_Err err = suites.persistentDataSuite->AEGP_GetFpLong(
            m_blobH, m_section.c_str(), key.c_str(),
            static_cast<A_FpLong>(defaultValue), &value);
        if (err != A_Err_NONE) {
            return defaultValue;
        }

        return static_cast<double>(value);
    }

    // Set a float value
    void SetFloat(const std::string& key, double value) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite || !m_blobH) {
            throw std::runtime_error("PersistentData Suite not available");
        }

        A_Err err = suites.persistentDataSuite->AEGP_SetFpLong(
            m_blobH, m_section.c_str(), key.c_str(), static_cast<A_FpLong>(value));
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set float value");
        }
    }

    // Check if a key exists
    bool Contains(const std::string& key) const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite || !m_blobH) {
            return false;
        }

        A_Boolean exists = FALSE;
        A_Err err = suites.persistentDataSuite->AEGP_DoesKeyExist(
            m_blobH, m_section.c_str(), key.c_str(), &exists);
        if (err != A_Err_NONE) {
            return false;
        }

        return exists != FALSE;
    }

    // Delete a key
    void Delete(const std::string& key) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite || !m_blobH) {
            throw std::runtime_error("PersistentData Suite not available");
        }

        A_Err err = suites.persistentDataSuite->AEGP_DeleteEntry(
            m_blobH, m_section.c_str(), key.c_str());
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to delete entry");
        }
    }

    // Get all keys in this section
    std::vector<std::string> Keys() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        std::vector<std::string> result;

        if (!suites.persistentDataSuite || !m_blobH) {
            return result;
        }

        A_long numKeys = 0;
        A_Err err = suites.persistentDataSuite->AEGP_GetNumKeys(
            m_blobH, m_section.c_str(), &numKeys);
        if (err != A_Err_NONE) {
            return result;
        }

        for (A_long i = 0; i < numKeys; ++i) {
            std::vector<A_char> buffer(256, 0);
            err = suites.persistentDataSuite->AEGP_GetValueKeyByIndex(
                m_blobH, m_section.c_str(), i, 256, buffer.data());
            if (err == A_Err_NONE) {
                result.push_back(buffer.data());
            }
        }

        return result;
    }

    std::string GetSectionName() const { return m_section; }

private:
    AEGP_PersistentBlobH m_blobH;
    std::string m_section;
};

// =============================================================
// PyPersistentBlob - Blob-level access
// =============================================================
class PyPersistentBlob {
public:
    explicit PyPersistentBlob(AEGP_PersistentType type) : m_type(type), m_blobH(nullptr) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (!suites.persistentDataSuite) {
            throw std::runtime_error("PersistentData Suite not available");
        }

        A_Err err = suites.persistentDataSuite->AEGP_GetApplicationBlob(type, &m_blobH);
        if (err != A_Err_NONE || !m_blobH) {
            throw std::runtime_error("Failed to get application blob");
        }
    }

    // Get a section by name
    PyPersistentSection GetSection(const std::string& section) {
        return PyPersistentSection(m_blobH, section);
    }

    // Typed accessors at blob level
    std::string GetString(const std::string& section, const std::string& key,
                          const std::string& defaultValue = "") const {
        PyPersistentSection sec(m_blobH, section);
        return sec.GetString(key, defaultValue);
    }

    void SetString(const std::string& section, const std::string& key,
                   const std::string& value) {
        PyPersistentSection sec(m_blobH, section);
        sec.SetString(key, value);
    }

    int GetInt(const std::string& section, const std::string& key,
               int defaultValue = 0) const {
        PyPersistentSection sec(m_blobH, section);
        return sec.GetInt(key, defaultValue);
    }

    void SetInt(const std::string& section, const std::string& key, int value) {
        PyPersistentSection sec(m_blobH, section);
        sec.SetInt(key, value);
    }

    double GetFloat(const std::string& section, const std::string& key,
                    double defaultValue = 0.0) const {
        PyPersistentSection sec(m_blobH, section);
        return sec.GetFloat(key, defaultValue);
    }

    void SetFloat(const std::string& section, const std::string& key, double value) {
        PyPersistentSection sec(m_blobH, section);
        sec.SetFloat(key, value);
    }

    bool Contains(const std::string& section, const std::string& key) const {
        PyPersistentSection sec(m_blobH, section);
        return sec.Contains(key);
    }

    void Delete(const std::string& section, const std::string& key) {
        PyPersistentSection sec(m_blobH, section);
        sec.Delete(key);
    }

    // Get all sections
    std::vector<std::string> Sections() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        std::vector<std::string> result;

        if (!suites.persistentDataSuite || !m_blobH) {
            return result;
        }

        A_long numSections = 0;
        A_Err err = suites.persistentDataSuite->AEGP_GetNumSections(m_blobH, &numSections);
        if (err != A_Err_NONE) {
            return result;
        }

        for (A_long i = 0; i < numSections; ++i) {
            std::vector<A_char> buffer(256, 0);
            err = suites.persistentDataSuite->AEGP_GetSectionKeyByIndex(
                m_blobH, i, 256, buffer.data());
            if (err == A_Err_NONE) {
                // Filter out invalid characters
                std::string sectionName;
                for (size_t j = 0; j < 256 && buffer[j] != '\0'; ++j) {
                    unsigned char c = static_cast<unsigned char>(buffer[j]);
                    if ((c >= 0x20 && c <= 0x7E) || c == '\t' || c == '\n' || c == '\r') {
                        sectionName.push_back(buffer[j]);
                    }
                }
                if (!sectionName.empty()) {
                    result.push_back(sectionName);
                }
            }
        }

        return result;
    }

    AEGP_PersistentBlobH GetHandle() const { return m_blobH; }

private:
    AEGP_PersistentType m_type;
    AEGP_PersistentBlobH m_blobH;
};

// =============================================================
// Get preferences directory
// =============================================================
std::string GetPrefsDirectory() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.persistentDataSuite || !suites.memorySuite) {
        throw std::runtime_error("Required suites not available");
    }

    AEGP_MemHandle pathH = nullptr;
    A_Err err = suites.persistentDataSuite->AEGP_GetPrefsDirectory(&pathH);
    if (err != A_Err_NONE || !pathH) {
        throw std::runtime_error("Failed to get prefs directory");
    }

    ScopedMemHandle scopedPath(state.GetPluginID(), suites.memorySuite, pathH);

    void* ptr = nullptr;
    err = suites.memorySuite->AEGP_LockMemHandle(pathH, &ptr);
    if (err != A_Err_NONE || !ptr) {
        throw std::runtime_error("Failed to lock prefs directory path");
    }

    // Path is UTF-16 encoded
    std::string result = StringUtils::Utf16ToUtf8(static_cast<A_UTF16Char*>(ptr));
    suites.memorySuite->AEGP_UnlockMemHandle(pathH);

    return result;
}

} // namespace PyAE

// =============================================================
// Python bindings
// =============================================================
void init_persistent_data(py::module_& m) {
    // Create submodule
    py::module_ pd = m.def_submodule("persistent_data", "Persistent data (preferences) storage");

    // PersistentSection class
    py::class_<PyAE::PyPersistentSection>(pd, "Section",
        "A section within persistent data storage")
        .def("get_string", &PyAE::PyPersistentSection::GetString,
             "Get a string value",
             py::arg("key"), py::arg("default") = "")
        .def("set_string", &PyAE::PyPersistentSection::SetString,
             "Set a string value",
             py::arg("key"), py::arg("value"))
        .def("get_int", &PyAE::PyPersistentSection::GetInt,
             "Get an integer value",
             py::arg("key"), py::arg("default") = 0)
        .def("set_int", &PyAE::PyPersistentSection::SetInt,
             "Set an integer value",
             py::arg("key"), py::arg("value"))
        .def("get_float", &PyAE::PyPersistentSection::GetFloat,
             "Get a float value",
             py::arg("key"), py::arg("default") = 0.0)
        .def("set_float", &PyAE::PyPersistentSection::SetFloat,
             "Set a float value",
             py::arg("key"), py::arg("value"))
        .def("__contains__", &PyAE::PyPersistentSection::Contains,
             "Check if key exists")
        .def("delete", &PyAE::PyPersistentSection::Delete,
             "Delete a key",
             py::arg("key"))
        .def("keys", &PyAE::PyPersistentSection::Keys,
             "Get all keys in this section")
        .def_property_readonly("name", &PyAE::PyPersistentSection::GetSectionName,
             "Section name");

    // PersistentBlob class
    py::class_<PyAE::PyPersistentBlob>(pd, "Blob",
        "Persistent data blob (container for sections)")
        .def("__getitem__", &PyAE::PyPersistentBlob::GetSection,
             "Get a section by name",
             py::arg("section"))
        .def("get_string", &PyAE::PyPersistentBlob::GetString,
             "Get a string value",
             py::arg("section"), py::arg("key"), py::arg("default") = "")
        .def("set_string", &PyAE::PyPersistentBlob::SetString,
             "Set a string value",
             py::arg("section"), py::arg("key"), py::arg("value"))
        .def("get_int", &PyAE::PyPersistentBlob::GetInt,
             "Get an integer value",
             py::arg("section"), py::arg("key"), py::arg("default") = 0)
        .def("set_int", &PyAE::PyPersistentBlob::SetInt,
             "Set an integer value",
             py::arg("section"), py::arg("key"), py::arg("value"))
        .def("get_float", &PyAE::PyPersistentBlob::GetFloat,
             "Get a float value",
             py::arg("section"), py::arg("key"), py::arg("default") = 0.0)
        .def("set_float", &PyAE::PyPersistentBlob::SetFloat,
             "Set a float value",
             py::arg("section"), py::arg("key"), py::arg("value"))
        .def("contains", &PyAE::PyPersistentBlob::Contains,
             "Check if key exists",
             py::arg("section"), py::arg("key"))
        .def("delete", &PyAE::PyPersistentBlob::Delete,
             "Delete a key",
             py::arg("section"), py::arg("key"))
        .def("sections", &PyAE::PyPersistentBlob::Sections,
             "Get all section names");

    // Blob type constants
    pd.attr("MACHINE_SPECIFIC") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC);
    pd.attr("MACHINE_INDEPENDENT") = static_cast<int>(AEGP_PersistentType_MACHINE_INDEPENDENT);
    pd.attr("MACHINE_INDEPENDENT_RENDER") = static_cast<int>(AEGP_PersistentType_MACHINE_INDEPENDENT_RENDER);
    pd.attr("MACHINE_INDEPENDENT_OUTPUT") = static_cast<int>(AEGP_PersistentType_MACHINE_INDEPENDENT_OUTPUT);
    pd.attr("MACHINE_INDEPENDENT_COMPOSITION") = static_cast<int>(AEGP_PersistentType_MACHINE_INDEPENDENT_COMPOSITION);
    pd.attr("MACHINE_SPECIFIC_TEXT") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC_TEXT);
    pd.attr("MACHINE_SPECIFIC_PAINT") = static_cast<int>(AEGP_PersistentType_MACHINE_SPECIFIC_PAINT);

    // Module-level accessors for common blob types
    pd.def("get_machine_specific", []() {
        return PyAE::PyPersistentBlob(AEGP_PersistentType_MACHINE_SPECIFIC);
    }, "Get machine-specific preferences blob");

    pd.def("get_machine_independent", []() {
        return PyAE::PyPersistentBlob(AEGP_PersistentType_MACHINE_INDEPENDENT);
    }, "Get machine-independent preferences blob");

    pd.def("get_prefs_directory", []() {
        return PyAE::GetPrefsDirectory();
    }, "Get the preferences directory path");

    // Convenience property-like access via module attributes
    pd.attr("machine_specific") = py::cpp_function([]() {
        return PyAE::PyPersistentBlob(AEGP_PersistentType_MACHINE_SPECIFIC);
    });

    pd.attr("machine_independent") = py::cpp_function([]() {
        return PyAE::PyPersistentBlob(AEGP_PersistentType_MACHINE_INDEPENDENT);
    });

    // Factory function for custom blob types
    pd.def("get_blob", [](int blobType) {
        return PyAE::PyPersistentBlob(static_cast<AEGP_PersistentType>(blobType));
    }, "Get a preferences blob by type",
       py::arg("blob_type"));
}
