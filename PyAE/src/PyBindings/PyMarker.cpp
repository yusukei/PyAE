#include "PyMarker.h"
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"

namespace PyAE {

namespace py = pybind11;

// =============================================================
// Implementation
// =============================================================

PyMarker::PyMarker() 
    : m_markerP(nullptr), m_ownsHandle(false) 
{
}

PyMarker::PyMarker(AEGP_MarkerValP markerP, bool ownsHandle)
    : m_markerP(markerP), m_ownsHandle(ownsHandle)
{
}

PyMarker::~PyMarker() {
    if (m_ownsHandle && m_markerP) {
        auto& state = PluginState::Instance();
        // Check if suites are still valid before disposing (e.g. shutdown)
        // Note: PluginState might be destroyed at exit, handling that is complex,
        // but generally safer to check pointer.
        // Also accessing suites requires Instance()
        if (state.GetSuites().markerSuite) {
            state.GetSuites().markerSuite->AEGP_DisposeMarker(m_markerP);
        }
    }
}

PyMarker::PyMarker(PyMarker&& other) noexcept
    : m_markerP(other.m_markerP), m_ownsHandle(other.m_ownsHandle)
{
    other.m_markerP = nullptr;
    other.m_ownsHandle = false;
}

PyMarker& PyMarker::operator=(PyMarker&& other) noexcept {
    if (this != &other) {
        if (m_ownsHandle && m_markerP) {
            auto& state = PluginState::Instance();
            if (state.GetSuites().markerSuite) {
                state.GetSuites().markerSuite->AEGP_DisposeMarker(m_markerP);
            }
        }
        m_markerP = other.m_markerP;
        m_ownsHandle = other.m_ownsHandle;
        other.m_markerP = nullptr;
        other.m_ownsHandle = false;
    }
    return *this;
}

std::shared_ptr<PyMarker> PyMarker::Create() {
    auto& state = PluginState::Instance();
    AEGP_MarkerValP newMarker = nullptr;
    A_Err err = state.GetSuites().markerSuite->AEGP_NewMarker(&newMarker);
    if (err != A_Err_NONE || !newMarker) {
        throw std::runtime_error("Failed to create marker");
    }
    return std::make_shared<PyMarker>(newMarker, true);
}

// -------------------------------------------------------------
// Helpers
// -------------------------------------------------------------

std::string PyMarker::GetString(AEGP_MarkerStringType type) const {
    if (!m_markerP) return "";
    auto& state = PluginState::Instance();
    AEGP_MemHandle memH = nullptr;
    A_Err err = state.GetSuites().markerSuite->AEGP_GetMarkerString(
        state.GetPluginID(), m_markerP, type, &memH);
    
    if (err != A_Err_NONE || !memH) return "";
    
    ScopedMemHandle scoped(state.GetPluginID(), state.GetSuites().memorySuite, memH);
    ScopedMemLock lock(state.GetSuites().memorySuite, memH);
    A_UTF16Char* chars = lock.As<A_UTF16Char>();
    if (!chars) return "";
    
    return StringUtils::Utf16ToUtf8(chars);
}

void PyMarker::SetString(AEGP_MarkerStringType type, const std::string& val) {
    if (!m_markerP) throw std::runtime_error("Invalid marker");
    auto& state = PluginState::Instance();
    std::wstring wstr = StringUtils::Utf8ToWide(val);
    A_Err err = state.GetSuites().markerSuite->AEGP_SetMarkerString(
        m_markerP, type, reinterpret_cast<const A_u_short*>(wstr.c_str()), (A_long)wstr.length());
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set marker string");
}

bool PyMarker::GetFlag(AEGP_MarkerFlagType flag) const {
    if (!m_markerP) return false;
    auto& state = PluginState::Instance();
    A_Boolean val = FALSE;
    state.GetSuites().markerSuite->AEGP_GetMarkerFlag(m_markerP, flag, &val);
    return val != FALSE;
}

void PyMarker::SetFlag(AEGP_MarkerFlagType flag, bool val) {
    if (!m_markerP) throw std::runtime_error("Invalid marker");
    auto& state = PluginState::Instance();
    state.GetSuites().markerSuite->AEGP_SetMarkerFlag(m_markerP, flag, val ? TRUE : FALSE);
}

// -------------------------------------------------------------
// Accessors
// -------------------------------------------------------------

std::string PyMarker::GetComment() const { return GetString(AEGP_MarkerString_COMMENT); }
void PyMarker::SetComment(const std::string& val) { SetString(AEGP_MarkerString_COMMENT, val); }

std::string PyMarker::GetChapter() const { return GetString(AEGP_MarkerString_CHAPTER); }
void PyMarker::SetChapter(const std::string& val) { SetString(AEGP_MarkerString_CHAPTER, val); }

std::string PyMarker::GetURL() const { return GetString(AEGP_MarkerString_URL); }
void PyMarker::SetURL(const std::string& val) { SetString(AEGP_MarkerString_URL, val); }

std::string PyMarker::GetFrameTarget() const { return GetString(AEGP_MarkerString_FRAME_TARGET); }
void PyMarker::SetFrameTarget(const std::string& val) { SetString(AEGP_MarkerString_FRAME_TARGET, val); }

std::string PyMarker::GetCuePointName() const { return GetString(AEGP_MarkerString_CUE_POINT_NAME); }
void PyMarker::SetCuePointName(const std::string& val) { SetString(AEGP_MarkerString_CUE_POINT_NAME, val); }

double PyMarker::GetDuration() const {
    if (!m_markerP) return 0.0;
    auto& state = PluginState::Instance();
    A_Time durationT;
    state.GetSuites().markerSuite->AEGP_GetMarkerDuration(m_markerP, &durationT);
    return AETypeUtils::TimeToSeconds(durationT);
}

void PyMarker::SetDuration(double val) {
    if (!m_markerP) throw std::runtime_error("Invalid marker");
    auto& state = PluginState::Instance();
    A_Time durationT = AETypeUtils::SecondsToTime(val);
    state.GetSuites().markerSuite->AEGP_SetMarkerDuration(m_markerP, &durationT);
}

int PyMarker::GetLabel() const {
    if (!m_markerP) return 0;
    auto& state = PluginState::Instance();
    A_long val = 0;
    state.GetSuites().markerSuite->AEGP_GetMarkerLabel(m_markerP, &val);
    return (int)val;
}

void PyMarker::SetLabel(int val) {
    if (!m_markerP) throw std::runtime_error("Invalid marker");
    auto& state = PluginState::Instance();
    state.GetSuites().markerSuite->AEGP_SetMarkerLabel(m_markerP, (A_long)val);
}

bool PyMarker::IsNavigation() const { return GetFlag(AEGP_MarkerFlag_NAVIGATION); }
void PyMarker::SetNavigation(bool val) { SetFlag(AEGP_MarkerFlag_NAVIGATION, val); }

bool PyMarker::IsProtected() const { return GetFlag(AEGP_MarkerFlag_PROTECT_REGION); }
void PyMarker::SetProtected(bool val) { SetFlag(AEGP_MarkerFlag_PROTECT_REGION, val); }


// -------------------------------------------------------------
// Initialization
// -------------------------------------------------------------

} // namespace PyAE

void init_marker(pybind11::module_& m) {
    using namespace PyAE;
    py::class_<PyMarker, std::shared_ptr<PyMarker>>(m, "Marker")
        .def(py::init(&PyMarker::Create))
        .def_property("comment", &PyMarker::GetComment, &PyMarker::SetComment, "Marker Comment")
        .def_property("chapter", &PyMarker::GetChapter, &PyMarker::SetChapter, "Chapter")
        .def_property("url", &PyMarker::GetURL, &PyMarker::SetURL, "URL")
        .def_property("frame_target", &PyMarker::GetFrameTarget, &PyMarker::SetFrameTarget, "Frame Target")
        .def_property("cue_point_name", &PyMarker::GetCuePointName, &PyMarker::SetCuePointName, "Cue Point Name")
        .def_property("duration", &PyMarker::GetDuration, &PyMarker::SetDuration, "Duration in seconds")
        .def_property("label", &PyMarker::GetLabel, &PyMarker::SetLabel, "Label color index")
        .def_property("protected", &PyMarker::IsProtected, &PyMarker::SetProtected, "Protected Region flag")
        .def_property("navigation", &PyMarker::IsNavigation, &PyMarker::SetNavigation, "Navigation flag")
        .def_property_readonly("_handle", [](const PyMarker& self) -> uintptr_t {
            return reinterpret_cast<uintptr_t>(self.GetHandle());
        }, "Internal handle")
        ;
}
