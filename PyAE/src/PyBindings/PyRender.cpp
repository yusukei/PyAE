// PyRender.cpp
// PyAE - Python for After Effects
// レンダリング関連クラスの実装
//
// RenderOptionsSuite4, RenderSuite5 の高レベルAPI

#include "PyRenderClasses.h"
#include "PluginState.h"

namespace PyAE {

// =============================================================
// PyRenderOptions Implementation
// =============================================================

PyRenderOptions::PyRenderOptions()
    : m_optionsH(nullptr)
    , m_owned(false)
{
}

PyRenderOptions::PyRenderOptions(AEGP_RenderOptionsH optionsH, bool owned)
    : m_optionsH(optionsH)
    , m_owned(owned)
{
}

PyRenderOptions::PyRenderOptions(PyRenderOptions&& other) noexcept
    : m_optionsH(other.m_optionsH)
    , m_owned(other.m_owned)
{
    other.m_optionsH = nullptr;
    other.m_owned = false;
}

PyRenderOptions& PyRenderOptions::operator=(PyRenderOptions&& other) noexcept
{
    if (this != &other) {
        Dispose();
        m_optionsH = other.m_optionsH;
        m_owned = other.m_owned;
        other.m_optionsH = nullptr;
        other.m_owned = false;
    }
    return *this;
}

PyRenderOptions::~PyRenderOptions()
{
    Dispose();
}

void PyRenderOptions::Dispose()
{
    if (m_optionsH && m_owned) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.renderOptionsSuite) {
            suites.renderOptionsSuite->AEGP_Dispose(m_optionsH);
        }
        m_optionsH = nullptr;
        m_owned = false;
    }
}

bool PyRenderOptions::IsValid() const
{
    return m_optionsH != nullptr;
}

double PyRenderOptions::GetTime() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Time time = {0, 1};
    A_Err err = suites.renderOptionsSuite->AEGP_GetTime(m_optionsH, &time);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetTime failed");
    }

    return static_cast<double>(time.value) / static_cast<double>(time.scale);
}

void PyRenderOptions::SetTime(double seconds)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    // Convert seconds to A_Time with high precision
    A_Time time;
    time.scale = 1000000;  // Microsecond precision
    time.value = static_cast<A_long>(seconds * time.scale);

    A_Err err = suites.renderOptionsSuite->AEGP_SetTime(m_optionsH, time);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetTime failed");
    }
}

double PyRenderOptions::GetTimeStep() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Time timeStep = {0, 1};
    A_Err err = suites.renderOptionsSuite->AEGP_GetTimeStep(m_optionsH, &timeStep);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetTimeStep failed");
    }

    return static_cast<double>(timeStep.value) / static_cast<double>(timeStep.scale);
}

void PyRenderOptions::SetTimeStep(double seconds)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Time timeStep;
    timeStep.scale = 1000000;
    timeStep.value = static_cast<A_long>(seconds * timeStep.scale);

    A_Err err = suites.renderOptionsSuite->AEGP_SetTimeStep(m_optionsH, timeStep);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetTimeStep failed");
    }
}

WorldType PyRenderOptions::GetWorldType() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    AEGP_WorldType type = AEGP_WorldType_NONE;
    A_Err err = suites.renderOptionsSuite->AEGP_GetWorldType(m_optionsH, &type);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetWorldType failed");
    }

    return static_cast<WorldType>(type);
}

void PyRenderOptions::SetWorldType(WorldType type)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Err err = suites.renderOptionsSuite->AEGP_SetWorldType(
        m_optionsH, static_cast<AEGP_WorldType>(type));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetWorldType failed");
    }
}

FieldRender PyRenderOptions::GetFieldRender() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    PF_Field field = PF_Field_FRAME;
    A_Err err = suites.renderOptionsSuite->AEGP_GetFieldRender(m_optionsH, &field);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetFieldRender failed");
    }

    return static_cast<FieldRender>(field);
}

void PyRenderOptions::SetFieldRender(FieldRender field)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Err err = suites.renderOptionsSuite->AEGP_SetFieldRender(
        m_optionsH, static_cast<PF_Field>(field));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetFieldRender failed");
    }
}

std::tuple<int, int> PyRenderOptions::GetDownsampleFactor() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_short x = 1, y = 1;
    A_Err err = suites.renderOptionsSuite->AEGP_GetDownsampleFactor(m_optionsH, &x, &y);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetDownsampleFactor failed");
    }

    return std::make_tuple(static_cast<int>(x), static_cast<int>(y));
}

void PyRenderOptions::SetDownsampleFactor(int x, int y)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }
    if (x <= 0 || y <= 0) {
        throw std::runtime_error("Downsample factor must be positive");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Err err = suites.renderOptionsSuite->AEGP_SetDownsampleFactor(
        m_optionsH, static_cast<A_short>(x), static_cast<A_short>(y));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetDownsampleFactor failed");
    }
}

py::dict PyRenderOptions::GetRegionOfInterest() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_LRect roi = {0, 0, 0, 0};
    A_Err err = suites.renderOptionsSuite->AEGP_GetRegionOfInterest(m_optionsH, &roi);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetRegionOfInterest failed");
    }

    py::dict d;
    d["left"] = roi.left;
    d["top"] = roi.top;
    d["right"] = roi.right;
    d["bottom"] = roi.bottom;
    return d;
}

void PyRenderOptions::SetRegionOfInterest(int left, int top, int right, int bottom)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_LRect roi;
    roi.left = left;
    roi.top = top;
    roi.right = right;
    roi.bottom = bottom;

    A_Err err = suites.renderOptionsSuite->AEGP_SetRegionOfInterest(m_optionsH, &roi);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetRegionOfInterest failed");
    }
}

MatteMode PyRenderOptions::GetMatteMode() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    AEGP_MatteMode mode = AEGP_MatteMode_STRAIGHT;
    A_Err err = suites.renderOptionsSuite->AEGP_GetMatteMode(m_optionsH, &mode);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetMatteMode failed");
    }

    return static_cast<MatteMode>(mode);
}

void PyRenderOptions::SetMatteMode(MatteMode mode)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Err err = suites.renderOptionsSuite->AEGP_SetMatteMode(
        m_optionsH, static_cast<AEGP_MatteMode>(mode));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetMatteMode failed");
    }
}

ChannelOrder PyRenderOptions::GetChannelOrder() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    AEGP_ChannelOrder order = AEGP_ChannelOrder_ARGB;
    A_Err err = suites.renderOptionsSuite->AEGP_GetChannelOrder(m_optionsH, &order);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetChannelOrder failed");
    }

    return static_cast<ChannelOrder>(order);
}

void PyRenderOptions::SetChannelOrder(ChannelOrder order)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Err err = suites.renderOptionsSuite->AEGP_SetChannelOrder(
        m_optionsH, static_cast<AEGP_ChannelOrder>(order));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetChannelOrder failed");
    }
}

bool PyRenderOptions::GetRenderGuideLayers() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Boolean render = FALSE;
    A_Err err = suites.renderOptionsSuite->AEGP_GetRenderGuideLayers(m_optionsH, &render);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetRenderGuideLayers failed");
    }

    return render != FALSE;
}

void PyRenderOptions::SetRenderGuideLayers(bool render)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Err err = suites.renderOptionsSuite->AEGP_SetRenderGuideLayers(
        m_optionsH, render ? TRUE : FALSE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetRenderGuideLayers failed");
    }
}

RenderQuality PyRenderOptions::GetRenderQuality() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    AEGP_ItemQuality quality = AEGP_ItemQuality_BEST;
    A_Err err = suites.renderOptionsSuite->AEGP_GetRenderQuality(m_optionsH, &quality);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetRenderQuality failed");
    }

    return static_cast<RenderQuality>(quality);
}

void PyRenderOptions::SetRenderQuality(RenderQuality quality)
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    A_Err err = suites.renderOptionsSuite->AEGP_SetRenderQuality(
        m_optionsH, static_cast<AEGP_ItemQuality>(quality));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetRenderQuality failed");
    }
}

std::shared_ptr<PyRenderOptions> PyRenderOptions::Duplicate() const
{
    if (!m_optionsH) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    AEGP_RenderOptionsH copyH = nullptr;
    A_Err err = suites.renderOptionsSuite->AEGP_Duplicate(
        state.GetPluginID(), m_optionsH, &copyH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_Duplicate failed");
    }

    return std::make_shared<PyRenderOptions>(copyH, true);
}

AEGP_RenderOptionsH PyRenderOptions::GetHandle() const
{
    return m_optionsH;
}

AEGP_RenderOptionsH PyRenderOptions::Release()
{
    AEGP_RenderOptionsH h = m_optionsH;
    m_optionsH = nullptr;
    m_owned = false;
    return h;
}

bool PyRenderOptions::IsOwned() const
{
    return m_owned;
}

std::shared_ptr<PyRenderOptions> PyRenderOptions::FromItem(uintptr_t itemH_ptr)
{
    if (itemH_ptr == 0) {
        throw std::runtime_error("Invalid item handle");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderOptionsSuite) {
        throw std::runtime_error("RenderOptions Suite not available");
    }

    AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);
    AEGP_RenderOptionsH optionsH = nullptr;

    A_Err err = suites.renderOptionsSuite->AEGP_NewFromItem(
        state.GetPluginID(), itemH, &optionsH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewFromItem failed");
    }

    return std::make_shared<PyRenderOptions>(optionsH, true);
}

// =============================================================
// PyFrameReceipt Implementation
// =============================================================

PyFrameReceipt::PyFrameReceipt()
    : m_receiptH(nullptr)
    , m_checkedIn(true)
{
}

PyFrameReceipt::PyFrameReceipt(AEGP_FrameReceiptH receiptH)
    : m_receiptH(receiptH)
    , m_checkedIn(false)
{
}

PyFrameReceipt::PyFrameReceipt(PyFrameReceipt&& other) noexcept
    : m_receiptH(other.m_receiptH)
    , m_checkedIn(other.m_checkedIn)
{
    other.m_receiptH = nullptr;
    other.m_checkedIn = true;
}

PyFrameReceipt& PyFrameReceipt::operator=(PyFrameReceipt&& other) noexcept
{
    if (this != &other) {
        Checkin();
        m_receiptH = other.m_receiptH;
        m_checkedIn = other.m_checkedIn;
        other.m_receiptH = nullptr;
        other.m_checkedIn = true;
    }
    return *this;
}

PyFrameReceipt::~PyFrameReceipt()
{
    try {
        Checkin();
    } catch (...) {
        // Suppress exceptions in destructor
    }
}

bool PyFrameReceipt::IsValid() const
{
    return m_receiptH != nullptr && !m_checkedIn;
}

std::shared_ptr<PyWorld> PyFrameReceipt::GetWorld() const
{
    if (!IsValid()) {
        throw std::runtime_error("Invalid or checked-in frame receipt");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite) {
        throw std::runtime_error("Render Suite not available");
    }

    AEGP_WorldH worldH = nullptr;
    A_Err err = suites.renderSuite->AEGP_GetReceiptWorld(m_receiptH, &worldH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetReceiptWorld failed");
    }

    // Note: The world is NOT owned by us - it's owned by the receipt
    // We pass owned=false so that PyWorld won't try to dispose it
    return std::make_shared<PyWorld>(worldH, false);
}

py::dict PyFrameReceipt::GetRenderedRegion() const
{
    if (!IsValid()) {
        throw std::runtime_error("Invalid or checked-in frame receipt");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite) {
        throw std::runtime_error("Render Suite not available");
    }

    A_LRect region = {0, 0, 0, 0};
    A_Err err = suites.renderSuite->AEGP_GetRenderedRegion(m_receiptH, &region);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetRenderedRegion failed");
    }

    py::dict d;
    d["left"] = region.left;
    d["top"] = region.top;
    d["right"] = region.right;
    d["bottom"] = region.bottom;
    return d;
}

std::string PyFrameReceipt::GetGuid() const
{
    if (!IsValid()) {
        throw std::runtime_error("Invalid or checked-in frame receipt");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite || !suites.memorySuite) {
        throw std::runtime_error("Required suites not available");
    }

    AEGP_MemHandle guidMH = nullptr;
    A_Err err = suites.renderSuite->AEGP_GetReceiptGuid(m_receiptH, &guidMH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetReceiptGuid failed");
    }

    if (!guidMH) {
        return "";
    }

    void* guidData = nullptr;
    err = suites.memorySuite->AEGP_LockMemHandle(guidMH, &guidData);
    if (err != A_Err_NONE) {
        suites.memorySuite->AEGP_FreeMemHandle(guidMH);
        throw std::runtime_error("Failed to lock memory handle");
    }

    std::string result;
    if (guidData) {
        result = std::string(reinterpret_cast<const char*>(guidData));
    }

    suites.memorySuite->AEGP_UnlockMemHandle(guidMH);
    suites.memorySuite->AEGP_FreeMemHandle(guidMH);

    return result;
}

void PyFrameReceipt::Checkin()
{
    if (m_receiptH && !m_checkedIn) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.renderSuite) {
            suites.renderSuite->AEGP_CheckinFrame(m_receiptH);
        }
        m_receiptH = nullptr;
        m_checkedIn = true;
    }
}

PyFrameReceipt* PyFrameReceipt::Enter()
{
    return this;
}

void PyFrameReceipt::Exit(py::object /*exc_type*/, py::object /*exc_val*/, py::object /*exc_tb*/)
{
    Checkin();
}

AEGP_FrameReceiptH PyFrameReceipt::GetHandle() const
{
    return m_receiptH;
}

// =============================================================
// PyRenderer Implementation
// =============================================================

std::shared_ptr<PyFrameReceipt> PyRenderer::RenderFrame(
    const std::shared_ptr<PyRenderOptions>& options)
{
    if (!options || !options->IsValid()) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite) {
        throw std::runtime_error("Render Suite not available");
    }

    AEGP_FrameReceiptH receiptH = nullptr;
    A_Err err = suites.renderSuite->AEGP_RenderAndCheckoutFrame(
        options->GetHandle(),
        nullptr,  // cancel_functionP0 - not available from Python
        nullptr,  // cancel_function_refconP0
        &receiptH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_RenderAndCheckoutFrame failed with error: " + std::to_string(err));
    }

    return std::make_shared<PyFrameReceipt>(receiptH);
}

std::shared_ptr<PyFrameReceipt> PyRenderer::RenderLayerFrame(uintptr_t layerOptionsH_ptr)
{
    if (layerOptionsH_ptr == 0) {
        throw std::runtime_error("Invalid layer render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite) {
        throw std::runtime_error("Render Suite not available");
    }

    AEGP_LayerRenderOptionsH optionsH = reinterpret_cast<AEGP_LayerRenderOptionsH>(layerOptionsH_ptr);
    AEGP_FrameReceiptH receiptH = nullptr;

    A_Err err = suites.renderSuite->AEGP_RenderAndCheckoutLayerFrame(
        optionsH,
        nullptr,  // cancel_functionP0
        nullptr,  // cancel_function_refconP0
        &receiptH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_RenderAndCheckoutLayerFrame failed with error: " + std::to_string(err));
    }

    return std::make_shared<PyFrameReceipt>(receiptH);
}

std::tuple<int, int, int, int> PyRenderer::GetCurrentTimestamp()
{
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite) {
        throw std::runtime_error("Render Suite not available");
    }

    AEGP_TimeStamp timestamp;
    memset(&timestamp, 0, sizeof(timestamp));

    A_Err err = suites.renderSuite->AEGP_GetCurrentTimestamp(&timestamp);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCurrentTimestamp failed");
    }

    return std::make_tuple(
        static_cast<int>(timestamp.a[0]),
        static_cast<int>(timestamp.a[1]),
        static_cast<int>(timestamp.a[2]),
        static_cast<int>(timestamp.a[3]));
}

bool PyRenderer::HasItemChangedSinceTimestamp(
    uintptr_t itemH_ptr,
    double startTime,
    double duration,
    const std::tuple<int, int, int, int>& timestamp)
{
    if (itemH_ptr == 0) {
        throw std::runtime_error("Invalid item handle");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite) {
        throw std::runtime_error("Render Suite not available");
    }

    AEGP_ItemH itemH = reinterpret_cast<AEGP_ItemH>(itemH_ptr);

    A_Time start;
    start.scale = 1000000;
    start.value = static_cast<A_long>(startTime * start.scale);

    A_Time dur;
    dur.scale = 1000000;
    dur.value = static_cast<A_long>(duration * dur.scale);

    AEGP_TimeStamp ts;
    ts.a[0] = static_cast<A_char>(std::get<0>(timestamp));
    ts.a[1] = static_cast<A_char>(std::get<1>(timestamp));
    ts.a[2] = static_cast<A_char>(std::get<2>(timestamp));
    ts.a[3] = static_cast<A_char>(std::get<3>(timestamp));

    A_Boolean hasChanged = FALSE;
    A_Err err = suites.renderSuite->AEGP_HasItemChangedSinceTimestamp(
        itemH, &start, &dur, &ts, &hasChanged);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_HasItemChangedSinceTimestamp failed");
    }

    return hasChanged != FALSE;
}

bool PyRenderer::IsItemWorthwhileToRender(
    const std::shared_ptr<PyRenderOptions>& options,
    const std::tuple<int, int, int, int>& timestamp)
{
    if (!options || !options->IsValid()) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite) {
        throw std::runtime_error("Render Suite not available");
    }

    AEGP_TimeStamp ts;
    ts.a[0] = static_cast<A_char>(std::get<0>(timestamp));
    ts.a[1] = static_cast<A_char>(std::get<1>(timestamp));
    ts.a[2] = static_cast<A_char>(std::get<2>(timestamp));
    ts.a[3] = static_cast<A_char>(std::get<3>(timestamp));

    A_Boolean isWorthwhile = FALSE;
    A_Err err = suites.renderSuite->AEGP_IsItemWorthwhileToRender(
        options->GetHandle(), &ts, &isWorthwhile);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_IsItemWorthwhileToRender failed");
    }

    return isWorthwhile != FALSE;
}

bool PyRenderer::IsRenderedFrameSufficient(
    const std::shared_ptr<PyRenderOptions>& renderedOptions,
    const std::shared_ptr<PyRenderOptions>& proposedOptions)
{
    if (!renderedOptions || !renderedOptions->IsValid() ||
        !proposedOptions || !proposedOptions->IsValid()) {
        throw std::runtime_error("Invalid render options");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.renderSuite) {
        throw std::runtime_error("Render Suite not available");
    }

    A_Boolean isSufficient = FALSE;
    A_Err err = suites.renderSuite->AEGP_IsRenderedFrameSufficient(
        renderedOptions->GetHandle(),
        proposedOptions->GetHandle(),
        &isSufficient);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_IsRenderedFrameSufficient failed");
    }

    return isSufficient != FALSE;
}

// =============================================================
// Module initialization
// =============================================================

void init_render(py::module_& m)
{
    // Enums
    py::enum_<MatteMode>(m, "MatteMode", "Matte mode for rendering")
        .value("STRAIGHT", MatteMode::STRAIGHT, "Straight alpha")
        .value("PREMUL_BLACK", MatteMode::PREMUL_BLACK, "Premultiplied with black")
        .value("PREMUL_BG_COLOR", MatteMode::PREMUL_BG_COLOR, "Premultiplied with background color")
        .export_values();

    py::enum_<ChannelOrder>(m, "ChannelOrder", "Channel order for rendered frames")
        .value("ARGB", ChannelOrder::ARGB, "Alpha, Red, Green, Blue")
        .value("BGRA", ChannelOrder::BGRA, "Blue, Green, Red, Alpha")
        .export_values();

    py::enum_<FieldRender>(m, "FieldRender", "Field rendering mode")
        .value("FRAME", FieldRender::FRAME, "Full frame (no field)")
        .value("UPPER", FieldRender::UPPER, "Upper field")
        .value("LOWER", FieldRender::LOWER, "Lower field")
        .export_values();

    py::enum_<RenderQuality>(m, "RenderQuality", "Render quality level")
        .value("DRAFT", RenderQuality::DRAFT, "Draft quality (faster)")
        .value("BEST", RenderQuality::BEST, "Best quality")
        .export_values();

    // RenderOptions class
    py::class_<PyRenderOptions, std::shared_ptr<PyRenderOptions>>(m, "RenderOptions",
        "Render options for frame rendering.\n\n"
        "Create from an item using RenderOptions.from_item(item_handle).\n\n"
        "Example::\n\n"
        "    # Create options from a composition\n"
        "    options = ae.RenderOptions.from_item(comp._handle)\n"
        "    options.time = 2.5  # Set render time to 2.5 seconds\n"
        "    options.world_type = ae.WorldType.BIT16  # 16bpc\n")
        .def_property_readonly("valid", &PyRenderOptions::IsValid,
            "Check if options are valid")
        .def_property("time", &PyRenderOptions::GetTime, &PyRenderOptions::SetTime,
            "Render time in seconds")
        .def_property("time_step", &PyRenderOptions::GetTimeStep, &PyRenderOptions::SetTimeStep,
            "Time step in seconds (for motion blur)")
        .def_property("world_type", &PyRenderOptions::GetWorldType, &PyRenderOptions::SetWorldType,
            "World type (bit depth)")
        .def_property("field_render", &PyRenderOptions::GetFieldRender, &PyRenderOptions::SetFieldRender,
            "Field rendering mode")
        .def_property("downsample_factor", &PyRenderOptions::GetDownsampleFactor,
            [](PyRenderOptions& self, py::tuple factor) {
                self.SetDownsampleFactor(factor[0].cast<int>(), factor[1].cast<int>());
            },
            "Downsample factor as (x, y) tuple")
        .def_property("region_of_interest", &PyRenderOptions::GetRegionOfInterest,
            [](PyRenderOptions& self, const py::dict& roi) {
                self.SetRegionOfInterest(
                    roi["left"].cast<int>(),
                    roi["top"].cast<int>(),
                    roi["right"].cast<int>(),
                    roi["bottom"].cast<int>());
            },
            "Region of interest as dict {left, top, right, bottom}")
        .def_property("matte_mode", &PyRenderOptions::GetMatteMode, &PyRenderOptions::SetMatteMode,
            "Matte mode")
        .def_property("channel_order", &PyRenderOptions::GetChannelOrder, &PyRenderOptions::SetChannelOrder,
            "Channel order (ARGB or BGRA)")
        .def_property("render_guide_layers", &PyRenderOptions::GetRenderGuideLayers, &PyRenderOptions::SetRenderGuideLayers,
            "Whether to render guide layers")
        .def_property("render_quality", &PyRenderOptions::GetRenderQuality, &PyRenderOptions::SetRenderQuality,
            "Render quality")
        .def("duplicate", &PyRenderOptions::Duplicate,
            "Create a duplicate of this render options")
        .def_property_readonly("_handle", [](const PyRenderOptions& self) {
            return reinterpret_cast<uintptr_t>(self.GetHandle());
        }, "Internal: raw handle")
        .def_static("from_item", &PyRenderOptions::FromItem,
            py::arg("item_handle"),
            "Create render options from an item (composition or footage)")
        .def("__repr__", [](const PyRenderOptions& self) {
            if (!self.IsValid()) {
                return std::string("<RenderOptions: invalid>");
            }
            return std::string("<RenderOptions: valid>");
        });

    // FrameReceipt class
    py::class_<PyFrameReceipt, std::shared_ptr<PyFrameReceipt>>(m, "FrameReceipt",
        "Receipt for a rendered frame.\n\n"
        "Use as a context manager for automatic checkin::\n\n"
        "    with ae.Renderer.render_frame(options) as receipt:\n"
        "        world = receipt.world\n"
        "        pixels = world.get_pixels()\n"
        "    # Frame is automatically checked in after the with block\n")
        .def_property_readonly("valid", &PyFrameReceipt::IsValid,
            "Check if receipt is valid (not checked in)")
        .def_property_readonly("world", &PyFrameReceipt::GetWorld,
            "Get the rendered world (frame buffer). Read-only.")
        .def_property_readonly("rendered_region", &PyFrameReceipt::GetRenderedRegion,
            "Get the rendered region as dict {left, top, right, bottom}")
        .def_property_readonly("guid", &PyFrameReceipt::GetGuid,
            "Get the GUID of this rendered frame")
        .def("checkin", &PyFrameReceipt::Checkin,
            "Explicitly check in the frame (release resources)")
        .def("__enter__", &PyFrameReceipt::Enter, py::return_value_policy::reference)
        .def("__exit__", &PyFrameReceipt::Exit)
        .def("__repr__", [](const PyFrameReceipt& self) {
            if (!self.IsValid()) {
                return std::string("<FrameReceipt: checked in>");
            }
            return std::string("<FrameReceipt: valid>");
        });

    // Renderer class (static methods only)
    py::class_<PyRenderer>(m, "Renderer",
        "Static methods for rendering frames.\n\n"
        "Example::\n\n"
        "    # Create render options\n"
        "    options = ae.RenderOptions.from_item(comp._handle)\n"
        "    options.time = 1.0\n\n"
        "    # Render and get the frame\n"
        "    with ae.Renderer.render_frame(options) as receipt:\n"
        "        world = receipt.world\n"
        "        print(f'Rendered: {world.width}x{world.height}')\n")
        .def_static("render_frame", &PyRenderer::RenderFrame,
            py::arg("options"),
            "Render a frame using RenderOptions.\n\n"
            "Returns a FrameReceipt. Use as context manager for automatic cleanup.")
        .def_static("render_layer_frame", &PyRenderer::RenderLayerFrame,
            py::arg("layer_options_handle"),
            "Render a layer frame using LayerRenderOptions handle.\n\n"
            "Returns a FrameReceipt. Use as context manager for automatic cleanup.")
        .def_static("get_current_timestamp", &PyRenderer::GetCurrentTimestamp,
            "Get current project timestamp for cache validation.\n\n"
            "Returns tuple of 4 integers.")
        .def_static("has_item_changed_since_timestamp", &PyRenderer::HasItemChangedSinceTimestamp,
            py::arg("item_handle"), py::arg("start_time"), py::arg("duration"), py::arg("timestamp"),
            "Check if an item has changed since a timestamp.\n\n"
            "Returns True if the item's video has changed.")
        .def_static("is_item_worthwhile_to_render", &PyRenderer::IsItemWorthwhileToRender,
            py::arg("options"), py::arg("timestamp"),
            "Check if an item is worthwhile to render.\n\n"
            "Returns True if rendering is recommended.")
        .def_static("is_rendered_frame_sufficient", &PyRenderer::IsRenderedFrameSufficient,
            py::arg("rendered_options"), py::arg("proposed_options"),
            "Check if a rendered frame meets proposed requirements.\n\n"
            "Returns True if existing render is sufficient.");
}

} // namespace PyAE
