#pragma once

#include <string>
#include <memory>
#include <pybind11/pybind11.h>
#include "AE_GeneralPlug.h"

namespace PyAE {

class PyMarker {
public:
    PyMarker();
    explicit PyMarker(AEGP_MarkerValP markerP, bool ownsHandle = true);
    ~PyMarker();

    // Copy/Move semantics
    PyMarker(const PyMarker&) = delete;
    PyMarker& operator=(const PyMarker&) = delete;
    PyMarker(PyMarker&& other) noexcept;
    PyMarker& operator=(PyMarker&& other) noexcept;

    // Handle access
    AEGP_MarkerValP GetHandle() const { return m_markerP; }
    bool IsValid() const { return m_markerP != nullptr; }

    // Properties
    std::string GetComment() const;
    void SetComment(const std::string& val);

    std::string GetChapter() const;
    void SetChapter(const std::string& val);

    std::string GetURL() const;
    void SetURL(const std::string& val);

    std::string GetFrameTarget() const;
    void SetFrameTarget(const std::string& val);

    std::string GetCuePointName() const;
    void SetCuePointName(const std::string& val);

    double GetDuration() const;
    void SetDuration(double val);

    int GetLabel() const;
    void SetLabel(int val); // 0-16 for color

    // Flags
    bool IsNavigation() const;
    void SetNavigation(bool val);

    bool IsProtected() const;
    void SetProtected(bool val);

    // Helpers
    static std::shared_ptr<PyMarker> Create();

private:
    std::string GetString(AEGP_MarkerStringType type) const;
    void SetString(AEGP_MarkerStringType type, const std::string& val);
    
    bool GetFlag(AEGP_MarkerFlagType flag) const;
    void SetFlag(AEGP_MarkerFlagType flag, bool val);

    AEGP_MarkerValP m_markerP;
    bool m_ownsHandle;
};

void init_marker(pybind11::module_& m);

} // namespace PyAE
