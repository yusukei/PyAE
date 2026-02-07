// PyComp_Core.cpp
// PyAE - Python for After Effects
// PyComp 実装
//
// PYAE-008: PyComp.cppから分割

#include "PyCompClasses.h"
#include "PyLayerClasses.h"
#include "PyColorProfile.h"
#include "PluginState.h"
#include "AETypeUtils.h"
#include "ValidationUtils.h"
#include "StringUtils.h"
#include "ScopedHandles.h"
#include "SDKVersionCompat.h"
#include <vector>

namespace PyAE {

// =============================================================
// PyComp 実装
// =============================================================

PyComp::PyComp() : m_compH(nullptr), m_itemH(nullptr) {}

PyComp::PyComp(AEGP_CompH compH) : m_compH(compH), m_itemH(nullptr) {
    if (compH) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.compSuite) {
            suites.compSuite->AEGP_GetItemFromComp(compH, &m_itemH);
        }
    }
}

bool PyComp::IsValid() const {
    return m_compH != nullptr;
}

// 名前
std::string PyComp::GetName() const {
    if (!m_itemH) return "";

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_MemHandle nameH;
    A_Err err = suites.itemSuite->AEGP_GetItemName(
        state.GetPluginID(), m_itemH, &nameH);
    if (err != A_Err_NONE || !nameH) {
        throw std::runtime_error("AEGP_GetItemName failed");
    }

    ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);

    ScopedMemLock lock(suites.memorySuite, nameH);
    A_UTF16Char* namePtr = lock.As<A_UTF16Char>();
    if (!namePtr) return "";

    std::string result = StringUtils::Utf16ToUtf8(namePtr);

    return result;
}

void PyComp::SetName(const std::string& name) {
    if (!m_itemH) {
        throw std::runtime_error("Invalid comp");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    std::wstring wname = StringUtils::Utf8ToWide(name);

    A_Err err = suites.itemSuite->AEGP_SetItemName(
        m_itemH, reinterpret_cast<const A_UTF16Char*>(wname.c_str()));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set comp name");
    }
}

// サイズ
int PyComp::GetWidth() const {
    if (!m_itemH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long width, height;
    A_Err err = suites.itemSuite->AEGP_GetItemDimensions(m_itemH, &width, &height);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetItemDimensions failed");
    }
    return static_cast<int>(width);
}

int PyComp::GetHeight() const {
    if (!m_itemH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long width, height;
    A_Err err = suites.itemSuite->AEGP_GetItemDimensions(m_itemH, &width, &height);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetItemDimensions failed");
    }
    return static_cast<int>(height);
}

py::tuple PyComp::GetResolution() const {
    return py::make_tuple(GetWidth(), GetHeight());
}

// 時間関連
double PyComp::GetDuration() const {
    if (!m_itemH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time duration;
    A_Err err = suites.itemSuite->AEGP_GetItemDuration(m_itemH, &duration);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetItemDuration failed");
    }
    if (duration.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(duration);
}

void PyComp::SetDuration(double seconds) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    // Validate duration
    Validation::RequireValidDuration(seconds);

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    A_FpLong fps;
    A_Err err = suites.compSuite->AEGP_GetCompFramerate(m_compH, &fps);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get composition framerate");
    }
    A_Time duration = AETypeUtils::SecondsToTimeWithFps(seconds, fps);
    if (suites.compSuite->AEGP_SetCompDuration(m_compH, &duration) != A_Err_NONE)
        throw std::runtime_error("Failed to set duration");
}

double PyComp::GetFrameRate() const {
    if (!m_compH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_FpLong fps;
    A_Err err = suites.compSuite->AEGP_GetCompFramerate(m_compH, &fps);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompFramerate failed");
    }

    return static_cast<double>(fps);
}

void PyComp::SetFrameRate(double fps) {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_FpLong fpsVal = static_cast<A_FpLong>(fps);
    // AEGP_CompSuite11 does not expose AEGP_SetCompFramerate?
    // Commenting out to allow build.
    // if (suites.compSuite->AEGP_SetCompFramerate(m_compH, &fpsVal) != A_Err_NONE)
    throw std::runtime_error("SetFrameRate not supported in this AE version");
}

double PyComp::GetCTITime() const {
    if (!m_compH) return 0.0;
    // CTI support tentative
    return 0.0;
}

void PyComp::SetCTITime(double seconds) {
    throw std::runtime_error("SetCTITime is not implemented");
}

double PyComp::GetWorkAreaStart() const {
    if (!m_compH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time workAreaStart;
    A_Err err = suites.compSuite->AEGP_GetCompWorkAreaStart(m_compH, &workAreaStart);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompWorkAreaStart failed");
    }
    if (workAreaStart.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(workAreaStart);
}

void PyComp::SetWorkAreaStart(double time) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // 現在のワークエリア duration を取得
    A_Time currentDuration;
    if (suites.compSuite->AEGP_GetCompWorkAreaDuration(m_compH, &currentDuration) != A_Err_NONE) {
        throw std::runtime_error("Failed to get work area duration");
    }

    A_FpLong fps;
    A_Err err = suites.compSuite->AEGP_GetCompFramerate(m_compH, &fps);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get composition framerate");
    }
    A_Time newStart = AETypeUtils::SecondsToTimeWithFps(time, fps);

    if (suites.compSuite->AEGP_SetCompWorkAreaStartAndDuration(m_compH, &newStart, &currentDuration) != A_Err_NONE) {
        throw std::runtime_error("Failed to set work area start");
    }
}

double PyComp::GetWorkAreaDuration() const {
    if (!m_compH) return 0.0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_Time workAreaDuration;
    A_Err err = suites.compSuite->AEGP_GetCompWorkAreaDuration(m_compH, &workAreaDuration);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompWorkAreaDuration failed");
    }
    if (workAreaDuration.scale == 0) return 0.0;

    return AETypeUtils::TimeToSeconds(workAreaDuration);
}

void PyComp::SetWorkAreaDuration(double seconds) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // 現在のワークエリア start を取得
    A_Time currentStart;
    if (suites.compSuite->AEGP_GetCompWorkAreaStart(m_compH, &currentStart) != A_Err_NONE) {
        throw std::runtime_error("Failed to get work area start");
    }

    A_FpLong fps;
    A_Err err = suites.compSuite->AEGP_GetCompFramerate(m_compH, &fps);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get composition framerate");
    }
    A_Time newDuration = AETypeUtils::SecondsToTimeWithFps(seconds, fps);

    if (suites.compSuite->AEGP_SetCompWorkAreaStartAndDuration(m_compH, &currentStart, &newDuration) != A_Err_NONE) {
        throw std::runtime_error("Failed to set work area duration");
    }
}

// ==========================================
// プロパティ
// ==========================================

double PyComp::GetPixelAspect() const {
    if (!m_itemH) return 1.0;
    auto& suites = PluginState::Instance().GetSuites();
    A_Ratio ratio;
    A_Err err = suites.itemSuite->AEGP_GetItemPixelAspectRatio(m_itemH, &ratio);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetItemPixelAspectRatio failed");
    }
    if (ratio.den == 0) return 1.0;
    return (double)ratio.num / (double)ratio.den;
}

void PyComp::SetPixelAspect(double aspect) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    // Validate aspect ratio
    Validation::RequireValidAspectRatio(aspect);

    auto& suites = PluginState::Instance().GetSuites();

    A_Ratio ratio;
    ratio.num = static_cast<A_long>(aspect * 1000);
    ratio.den = 1000;

    A_Err err = suites.compSuite->AEGP_SetCompPixelAspectRatio(m_compH, &ratio);
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set pixel aspect ratio");
}

py::tuple PyComp::GetBgColor() const {
    if (!m_compH) return py::make_tuple(0,0,0);
    AEGP_ColorVal color;
    A_Err err = PluginState::Instance().GetSuites().compSuite->AEGP_GetCompBGColor(m_compH, &color);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompBGColor failed");
    }
    return py::make_tuple(color.redF, color.greenF, color.blueF);
}

void PyComp::SetBgColor(py::tuple color) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    // Validate tuple length
    if (py::len(color) < 3) {
        throw std::invalid_argument(
            "Color tuple must have at least 3 elements (RGB), got " +
            std::to_string(py::len(color)));
    }

    AEGP_ColorVal val;
    val.redF = color[0].cast<float>();
    val.greenF = color[1].cast<float>();
    val.blueF = color[2].cast<float>();
    val.alphaF = 1.0;

    // Validate color component ranges
    Validation::RequireColorRange(val.redF, "Red");
    Validation::RequireColorRange(val.greenF, "Green");
    Validation::RequireColorRange(val.blueF, "Blue");

    A_Err err = PluginState::Instance().GetSuites().compSuite->AEGP_SetCompBGColor(m_compH, &val);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set background color");
    }
}

// 新規追加プロパティ
void PyComp::SetDimensions(int width, int height) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    // Validate dimensions
    Validation::RequireValidCompDimensions(width, height);

    auto& suites = PluginState::Instance().GetSuites();
    A_Err err = suites.compSuite->AEGP_SetCompDimensions(m_compH, static_cast<A_long>(width), static_cast<A_long>(height));
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set comp dimensions");
}

double PyComp::GetDisplayStartTime() const {
    if (!m_compH) return 0.0;
    auto& suites = PluginState::Instance().GetSuites();
    A_Time startTime = {0, 1};
    A_Err err = suites.compSuite->AEGP_GetCompDisplayStartTime(m_compH, &startTime);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompDisplayStartTime failed");
    }
    if (startTime.scale == 0) return 0.0;
    return static_cast<double>(startTime.value) / static_cast<double>(startTime.scale);
}

void PyComp::SetDisplayStartTime(double seconds) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    // Validate non-negative time value
    Validation::RequireNonNegative(seconds, "Display start time");

    auto& suites = PluginState::Instance().GetSuites();
    A_FpLong fps = 30.0;
    A_Err err = suites.compSuite->AEGP_GetCompFramerate(m_compH, &fps);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get composition framerate");
    }

    // Use AETypeUtils for proper time conversion with fps
    A_Time startTime = AETypeUtils::SecondsToTimeWithFps(seconds, fps);

    err = suites.compSuite->AEGP_SetCompDisplayStartTime(m_compH, &startTime);
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set display start time");
}

bool PyComp::GetShowLayerNames() const {
    if (!m_compH) return true;
    auto& suites = PluginState::Instance().GetSuites();
    A_Boolean showLayerNames = TRUE;
    A_Err err = suites.compSuite->AEGP_GetShowLayerNameOrSourceName(m_compH, &showLayerNames);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetShowLayerNameOrSourceName failed");
    }
    return showLayerNames != FALSE;
}

void PyComp::SetShowLayerNames(bool show) {
    if (!m_compH) throw std::runtime_error("Invalid composition");
    auto& suites = PluginState::Instance().GetSuites();
    A_Err err = suites.compSuite->AEGP_SetShowLayerNameOrSourceName(m_compH, show ? TRUE : FALSE);
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set show layer names");
}

bool PyComp::GetShowBlendModes() const {
    if (!m_compH) return false;
    auto& suites = PluginState::Instance().GetSuites();
    A_Boolean showBlendModes = FALSE;
    A_Err err = suites.compSuite->AEGP_GetShowBlendModes(m_compH, &showBlendModes);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetShowBlendModes failed");
    }
    return showBlendModes != FALSE;
}

void PyComp::SetShowBlendModes(bool show) {
    if (!m_compH) throw std::runtime_error("Invalid composition");
    auto& suites = PluginState::Instance().GetSuites();
    A_Err err = suites.compSuite->AEGP_SetShowBlendModes(m_compH, show ? TRUE : FALSE);
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set show blend modes");
}

bool PyComp::GetDisplayDropFrame() const {
    if (!m_compH) return false;
    auto& suites = PluginState::Instance().GetSuites();
    A_Boolean dropFrame = FALSE;
    A_Err err = suites.compSuite->AEGP_GetCompDisplayDropFrame(m_compH, &dropFrame);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompDisplayDropFrame failed");
    }
    return dropFrame != FALSE;
}

void PyComp::SetDisplayDropFrame(bool dropFrame) {
    if (!m_compH) throw std::runtime_error("Invalid composition");
    auto& suites = PluginState::Instance().GetSuites();
    A_Err err = suites.compSuite->AEGP_SetCompDisplayDropFrame(m_compH, dropFrame ? TRUE : FALSE);
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set display drop frame");
}

double PyComp::GetFrameDuration() const {
    if (!m_compH) return 0.0;
    auto& suites = PluginState::Instance().GetSuites();
    A_Time frameTime = {0, 1};
    A_Err err = suites.compSuite->AEGP_GetCompFrameDuration(m_compH, &frameTime);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompFrameDuration failed");
    }
    if (frameTime.scale == 0) return 0.0;
    return static_cast<double>(frameTime.value) / static_cast<double>(frameTime.scale);
}

py::tuple PyComp::GetShutterAnglePhase() const {
    if (!m_compH) return py::make_tuple(0.0, 0.0);
    auto& suites = PluginState::Instance().GetSuites();
    A_Ratio angle = {0, 1};
    A_Ratio phase = {0, 1};
    A_Err err = suites.compSuite->AEGP_GetCompShutterAnglePhase(m_compH, &angle, &phase);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompShutterAnglePhase failed");
    }
    double angleVal = (angle.den != 0) ? static_cast<double>(angle.num) / angle.den : 0.0;
    double phaseVal = (phase.den != 0) ? static_cast<double>(phase.num) / phase.den : 0.0;
    return py::make_tuple(angleVal, phaseVal);
}

int PyComp::GetMotionBlurSamples() const {
    if (!m_compH) return 0;
    auto& suites = PluginState::Instance().GetSuites();
    A_long samples = 0;
    A_Err err = suites.compSuite->AEGP_GetCompSuggestedMotionBlurSamples(m_compH, &samples);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompSuggestedMotionBlurSamples failed");
    }
    return static_cast<int>(samples);
}

void PyComp::SetMotionBlurSamples(int samples) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    // Validate positive sample count
    Validation::RequirePositive(samples, "Motion blur samples");

    auto& suites = PluginState::Instance().GetSuites();
    A_Err err = suites.compSuite->AEGP_SetCompSuggestedMotionBlurSamples(m_compH, static_cast<A_long>(samples));
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set motion blur samples");
}

int PyComp::GetMotionBlurAdaptiveSampleLimit() const {
    if (!m_compH) return 0;
    auto& suites = PluginState::Instance().GetSuites();
    A_long samples = 0;
    A_Err err = suites.compSuite->AEGP_GetCompMotionBlurAdaptiveSampleLimit(m_compH, &samples);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompMotionBlurAdaptiveSampleLimit failed");
    }
    return static_cast<int>(samples);
}

void PyComp::SetMotionBlurAdaptiveSampleLimit(int samples) {
    if (!m_compH) throw std::runtime_error("Invalid composition");

    // Validate positive sample count
    Validation::RequirePositive(samples, "Motion blur adaptive sample limit");

    auto& suites = PluginState::Instance().GetSuites();
    A_Err err = suites.compSuite->AEGP_SetCompMotionBlurAdaptiveSampleLimit(m_compH, static_cast<A_long>(samples));
    if (err != A_Err_NONE) throw std::runtime_error("Failed to set motion blur adaptive sample limit");
}

// レイヤー操作
int PyComp::GetNumLayers() const {
    if (!m_compH) return 0;
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    A_long count = 0;
    A_Err err = suites.layerSuite->AEGP_GetCompNumLayers(m_compH, &count);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetCompNumLayers failed");
    }
    return static_cast<int>(count);
}

PyLayer PyComp::GetLayerByIndex(int index) const {
    if (!m_compH) return PyLayer();

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    // Get layer count for bounds checking
    A_long numLayers = 0;
    if (suites.layerSuite->AEGP_GetCompNumLayers(m_compH, &numLayers) != A_Err_NONE) {
        throw std::runtime_error("Failed to get layer count");
    }

    // Python-side bounds checking (0-based indexing)
    if (index < 0 || index >= numLayers) {
        throw std::out_of_range("Layer index out of range: " + std::to_string(index) +
                                " (valid range: 0-" + std::to_string(numLayers - 1) + ")");
    }

    AEGP_LayerH layerH = nullptr;
    // AEGP_GetCompLayerByIndex は 0-based（Pythonic）
    if (suites.layerSuite->AEGP_GetCompLayerByIndex(m_compH, index, &layerH) != A_Err_NONE) {
        throw std::runtime_error("Failed to get layer by index");
    }

    return PyLayer(layerH);
}

PyLayer PyComp::GetLayerByName(const std::string& name) const {
    int num = GetNumLayers();
    for (int i = 0; i < num; ++i) {
        PyLayer layer = GetLayerByIndex(i);
        if (layer.IsValid() && layer.GetName() == name) {
            return layer;
        }
    }
    return PyLayer();
}

PyLayer PyComp::GetLayer(const py::object& key) const {
    if (py::isinstance<py::int_>(key)) {
        return GetLayerByIndex(key.cast<int>());
    } else if (py::isinstance<py::str>(key)) {
        return GetLayerByName(key.cast<std::string>());
    }
    throw std::runtime_error("Key must be int or string");
}

std::vector<PyLayer> PyComp::GetLayers() const {
    std::vector<PyLayer> layers;

    if (!m_compH) return layers;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long numLayers;
    A_Err err = suites.layerSuite->AEGP_GetCompNumLayers(m_compH, &numLayers);
    if (err != A_Err_NONE) return layers;

    layers.reserve(numLayers);
    for (A_long i = 0; i < numLayers; ++i) {
        AEGP_LayerH layerH;
        err = suites.layerSuite->AEGP_GetCompLayerByIndex(m_compH, i, &layerH);
        if (err == A_Err_NONE && layerH) {
            layers.emplace_back(layerH);
        }
    }

    return layers;
}

std::vector<PyLayer> PyComp::GetSelectedLayers() const {
    std::vector<PyLayer> selectedLayers;

    if (!m_compH) return selectedLayers;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.collectionSuite) {
        return selectedLayers;
    }

    AEGP_Collection2H collectionH;
    A_Err err = suites.collectionSuite->AEGP_NewCollection(
        state.GetPluginID(), &collectionH);
    if (err != A_Err_NONE) {
        return selectedLayers;
    }

    err = suites.compSuite->AEGP_GetNewCollectionFromCompSelection(
        state.GetPluginID(), m_compH, &collectionH);
    if (err != A_Err_NONE) {
        suites.collectionSuite->AEGP_DisposeCollection(collectionH);
        return selectedLayers;
    }

    A_u_long numItems;
    err = suites.collectionSuite->AEGP_GetCollectionNumItems(collectionH, &numItems);
    if (err != A_Err_NONE) {
        suites.collectionSuite->AEGP_DisposeCollection(collectionH);
        return selectedLayers;
    }

    for (A_u_long i = 0; i < numItems; ++i) {
        AEGP_CollectionItemV2 item;
        err = suites.collectionSuite->AEGP_GetCollectionItemByIndex(collectionH, i, &item);
        if (err == A_Err_NONE && item.type == AEGP_CollectionItemType_LAYER) {
            selectedLayers.emplace_back(item.u.layer.layerH);
        }
    }

    suites.collectionSuite->AEGP_DisposeCollection(collectionH);
    return selectedLayers;
}

// レイヤー追加
PyLayer PyComp::AddSolid(const std::string& name, int width, int height,
                         double r, double g, double b, double duration) {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }

    // Validate dimensions
    Validation::RequireValidCompDimensions(width, height);

    // Validate color ranges
    Validation::RequireColorRange(r, "Red");
    Validation::RequireColorRange(g, "Green");
    Validation::RequireColorRange(b, "Blue");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_ColorVal color;
    color.redF = static_cast<A_FpShort>(r);
    color.greenF = static_cast<A_FpShort>(g);
    color.blueF = static_cast<A_FpShort>(b);
    color.alphaF = 1.0;

    double actualDuration = duration;
    if (actualDuration < 0) {
        actualDuration = GetDuration();
    }

    std::wstring wname = StringUtils::Utf8ToWide(name);

    // Get composition framerate to calculate duration
    A_FpLong fps;
    A_Err err = suites.compSuite->AEGP_GetCompFramerate(m_compH, &fps);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get composition framerate");
    }

    // Calculate duration in A_Time format
    A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(actualDuration, fps);

    // Initialize layer handle
    AEGP_LayerH newLayerH = nullptr;
    err = suites.compSuite->AEGP_CreateSolidInComp(
        reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
        static_cast<A_long>(width),
        static_cast<A_long>(height),
        &color,
        m_compH,
        &durationTime,
        &newLayerH
    );

    if (err != A_Err_NONE || !newLayerH) {
        throw std::runtime_error("Failed to create solid layer");
    }

    // NOTE: AEGP_CreateSolidInComp should set the layer name automatically
    // Testing without explicit AEGP_SetLayerName call

    return PyLayer(newLayerH);
}

PyLayer PyComp::AddNull(const std::string& name, double duration) {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    double actualDuration = duration;
    if (actualDuration < 0) {
        actualDuration = GetDuration();
    }

    A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(actualDuration, GetFrameRate());

    std::wstring wname = StringUtils::Utf8ToWide(name);

    AEGP_LayerH newLayerH;
    A_Err err = suites.compSuite->AEGP_CreateNullInComp(
        reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
        m_compH,
        &durationTime,
        &newLayerH
    );

    if (err != A_Err_NONE || !newLayerH) {
        throw std::runtime_error("Failed to create null layer");
    }

    return PyLayer(newLayerH);
}

PyLayer PyComp::AddCamera(const std::string& name, double x, double y) {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_FloatPoint center;
    center.x = static_cast<A_FpLong>(x == 0.0 ? GetWidth() / 2.0 : x);
    center.y = static_cast<A_FpLong>(y == 0.0 ? GetHeight() / 2.0 : y);

    std::wstring wname = StringUtils::Utf8ToWide(name);

    AEGP_LayerH newLayerH;
    A_Err err = suites.compSuite->AEGP_CreateCameraInComp(
        reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
        center,
        m_compH,
        &newLayerH
    );

    if (err != A_Err_NONE || !newLayerH) {
        throw std::runtime_error("Failed to create camera layer");
    }

    return PyLayer(newLayerH);
}

PyLayer PyComp::AddLight(const std::string& name, double x, double y) {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_FloatPoint center;
    center.x = static_cast<A_FpLong>(x == 0.0 ? GetWidth() / 2.0 : x);
    center.y = static_cast<A_FpLong>(y == 0.0 ? GetHeight() / 2.0 : y);

    std::wstring wname = StringUtils::Utf8ToWide(name);

    AEGP_LayerH newLayerH;
    A_Err err = suites.compSuite->AEGP_CreateLightInComp(
        reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
        center,
        m_compH,
        &newLayerH
    );

    if (err != A_Err_NONE || !newLayerH) {
        throw std::runtime_error("Failed to create light layer");
    }

    return PyLayer(newLayerH);
}

PyLayer PyComp::AddLayer(AEGP_ItemH itemH, double duration) {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }
    if (!itemH) {
        throw std::runtime_error("Invalid item");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    double actualDuration = duration;
    if (actualDuration < 0) {
        actualDuration = GetDuration();
    }

    A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(actualDuration, GetFrameRate());

    AEGP_LayerH newLayerH;
    A_Err err = suites.layerSuite->AEGP_AddLayer(
        itemH,
        m_compH,
        &newLayerH
    );

    if (err != A_Err_NONE || !newLayerH) {
        throw std::runtime_error("Failed to add layer from item");
    }

    return PyLayer(newLayerH);
}

PyLayer PyComp::AddText(const std::string& text) {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerH newLayerH = nullptr;
#if defined(kAEGPCompSuiteVersion12)
    // CompSuite12 (AE 25.2+): has horzB parameter
    A_Err err = suites.compSuite->AEGP_CreateTextLayerInComp(
        m_compH,         // parent_compH
        FALSE,           // select_new_layerB
        TRUE,            // horzB (horizontal text)
        &newLayerH       // new_text_layerPH
    );
#else
    // CompSuite11 (AE 2022-2025.1): no horzB parameter
    A_Err err = suites.compSuite->AEGP_CreateTextLayerInComp(
        m_compH,         // parent_compH
        FALSE,           // select_new_layerB
        &newLayerH       // new_text_layerPH
    );
#endif

    if (err != A_Err_NONE || !newLayerH) {
        throw std::runtime_error("Failed to create text layer");
    }

    // Set text content if provided
    if (!text.empty()) {
        // Get SOURCE_TEXT stream
        AEGP_StreamRefH streamH = nullptr;
        err = suites.streamSuite->AEGP_GetNewLayerStream(
            state.GetPluginID(),
            newLayerH,
            AEGP_LayerStream_SOURCE_TEXT,
            &streamH
        );

        if (err == A_Err_NONE && streamH) {
            // Get stream value at time 0
            A_Time time = {0, 1};

            // CRITICAL: Use heap allocation to match Python Bindings pattern
            // This prevents memory leak caused by SDK internal pointer tracking
            AEGP_StreamValue2* streamValue = new AEGP_StreamValue2();
            memset(streamValue, 0, sizeof(AEGP_StreamValue2));

            err = suites.streamSuite->AEGP_GetNewStreamValue(
                state.GetPluginID(),
                streamH,
                AEGP_LTimeMode_LayerTime,
                &time,
                FALSE,  // pre_expression
                streamValue
            );

            // Only proceed if GetNewStreamValue succeeded
            if (err == A_Err_NONE) {
                if (streamValue->val.text_documentH) {
                    // Convert UTF-8 to UTF-16
                    std::wstring utf16_text = StringUtils::Utf8ToUtf16(text);

                    // Set text content
                    err = suites.textDocSuite->AEGP_SetText(
                        streamValue->val.text_documentH,
                        (const A_u_short*)utf16_text.c_str(),
                        (A_long)utf16_text.length()
                    );

                    if (err == A_Err_NONE) {
                        // Set the modified value back to the stream
                        err = suites.streamSuite->AEGP_SetStreamValue(
                            state.GetPluginID(),
                            streamH,
                            streamValue
                        );
                    }
                }

                // CRITICAL: Only dispose if GetNewStreamValue succeeded
                // Adobe SDK always disposes, but only if the value was actually created
                suites.streamSuite->AEGP_DisposeStreamValue(streamValue);
            }

            // Delete the heap-allocated struct
            delete streamValue;

            // Dispose stream
            suites.streamSuite->AEGP_DisposeStream(streamH);
        }
    }

    return PyLayer(newLayerH);
}

PyLayer PyComp::AddShape() {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_LayerH newLayerH;
    A_Err err = suites.compSuite->AEGP_CreateVectorLayerInComp(
        m_compH,
        &newLayerH
    );

    if (err != A_Err_NONE || !newLayerH) {
        throw std::runtime_error("Failed to create shape layer");
    }

    return PyLayer(newLayerH);
}

PyLayer PyComp::AddBoxText(double boxWidth, double boxHeight, bool horizontal) {
    if (!m_compH) {
        throw std::runtime_error("Invalid comp");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_FloatPoint boxDimensions;
    boxDimensions.x = static_cast<A_FpLong>(boxWidth);
    boxDimensions.y = static_cast<A_FpLong>(boxHeight);

    AEGP_LayerH newLayerH = nullptr;
#if defined(kAEGPCompSuiteVersion12)
    // CompSuite12 (AE 25.2+): has horzB parameter
    A_Err err = suites.compSuite->AEGP_CreateBoxTextLayerInComp(
        m_compH,                          // parent_compH
        FALSE,                            // select_new_layerB
        boxDimensions,                    // box_dimensions
        horizontal ? TRUE : FALSE,        // horzB
        &newLayerH                        // new_text_layerPH
    );
#else
    // CompSuite11 (AE 2022-2025.1): no horzB parameter
    (void)horizontal;  // Unused in this SDK version
    A_Err err = suites.compSuite->AEGP_CreateBoxTextLayerInComp(
        m_compH,                          // parent_compH
        FALSE,                            // select_new_layerB
        boxDimensions,                    // box_dimensions
        &newLayerH                        // new_text_layerPH
    );
#endif

    if (err != A_Err_NONE || !newLayerH) {
        throw std::runtime_error("Failed to create box text layer");
    }

    return PyLayer(newLayerH);
}

// コンポジション操作
PyComp PyComp::Duplicate() const {
    if (!m_compH) {
        throw std::runtime_error("Invalid composition");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    AEGP_CompH newCompH = nullptr;
    A_Err err = suites.compSuite->AEGP_DuplicateComp(m_compH, &newCompH);
    if (err != A_Err_NONE || !newCompH) {
        throw std::runtime_error("Failed to duplicate composition");
    }

    return PyComp(newCompH);
}

// ハンドルアクセス
AEGP_CompH PyComp::GetHandle() const {
    return m_compH;
}

AEGP_ItemH PyComp::GetItemHandle() const {
    return m_itemH;
}

// 静的メソッド
PyComp PyComp::GetActive() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite || !suites.compSuite) {
        return PyComp();
    }

    AEGP_ItemH itemH;
    A_Err err = suites.itemSuite->AEGP_GetActiveItem(&itemH);
    if (err != A_Err_NONE || !itemH) {
        return PyComp();
    }

    AEGP_ItemType type;
    err = suites.itemSuite->AEGP_GetItemType(itemH, &type);
    if (err != A_Err_NONE || type != AEGP_ItemType_COMP) {
        return PyComp();
    }

    AEGP_CompH compH;
    err = suites.compSuite->AEGP_GetCompFromItem(itemH, &compH);
    if (err != A_Err_NONE) {
        return PyComp();
    }

    return PyComp(compH);
}

PyComp PyComp::GetMostRecentlyUsed() {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.compSuite) {
        return PyComp();
    }

    AEGP_CompH compH = nullptr;
    A_Err err = suites.compSuite->AEGP_GetMostRecentlyUsedComp(&compH);
    if (err != A_Err_NONE || !compH) {
        return PyComp();
    }

    return PyComp(compH);
}

PyComp PyComp::Create(const std::string& name, int width, int height,
                      double pixelAspect, double duration, double frameRate) {
    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.compSuite || !suites.projSuite) {
        throw std::runtime_error("Required suites not available");
    }

    AEGP_ProjectH projH;
    A_Err err = suites.projSuite->AEGP_GetProjectByIndex(0, &projH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get project");
    }

    AEGP_ItemH rootFolder;
    err = suites.projSuite->AEGP_GetProjectRootFolder(projH, &rootFolder);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get root folder");
    }

    A_Ratio pixelAspectRatio = AETypeUtils::DoubleToRatio(pixelAspect);
    A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(duration, frameRate);
    A_Ratio frameRateRatio = AETypeUtils::DoubleToRatio(frameRate, 1000);

    std::wstring wname = StringUtils::Utf8ToWide(name);

    AEGP_CompH newCompH;
    err = suites.compSuite->AEGP_CreateComp(
        rootFolder,
        reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
        static_cast<A_long>(width),
        static_cast<A_long>(height),
        &pixelAspectRatio,
        &durationTime,
        &frameRateRatio,
        &newCompH
    );

    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to create comp");
    }

    return PyComp(newCompH);
}

// カラープロファイル
py::object PyComp::GetColorProfile() const {
    if (!m_compH) {
        throw std::runtime_error("Invalid composition");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    AEGP_ColorProfileP colorProfileP = nullptr;
    A_Err err = suites.colorSettingsSuite->AEGP_GetNewWorkingSpaceColorProfile(
        state.GetPluginID(), m_compH, &colorProfileP);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetNewWorkingSpaceColorProfile failed");
    }

    auto profile = std::make_shared<PyColorProfile>(colorProfileP);
    return py::cast(profile);
}

void PyComp::SetColorProfile(py::object profileObj) {
    if (!m_compH) {
        throw std::runtime_error("Invalid composition");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.colorSettingsSuite) {
        throw std::runtime_error("ColorSettings Suite not available");
    }

    // Handle ColorProfile object
    if (py::isinstance<PyColorProfile>(profileObj)) {
        auto& profile = profileObj.cast<PyColorProfile&>();
        if (!profile.IsValid()) {
            throw std::runtime_error("Invalid color profile");
        }

        A_Err err = suites.colorSettingsSuite->AEGP_SetWorkingColorSpace(
            state.GetPluginID(), m_compH, profile.GetHandle());
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetWorkingColorSpace failed");
        }
    }
    // Handle shared_ptr<PyColorProfile>
    else if (py::isinstance<std::shared_ptr<PyColorProfile>>(profileObj)) {
        auto profile = profileObj.cast<std::shared_ptr<PyColorProfile>>();
        if (!profile || !profile->IsValid()) {
            throw std::runtime_error("Invalid color profile");
        }

        A_Err err = suites.colorSettingsSuite->AEGP_SetWorkingColorSpace(
            state.GetPluginID(), m_compH, profile->GetHandle());
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetWorkingColorSpace failed");
        }
    }
    // Handle bytes (ICC data)
    else if (py::isinstance<py::bytes>(profileObj)) {
        py::bytes iccData = profileObj.cast<py::bytes>();
        auto profile = PyColorProfile::FromICCData(iccData);
        if (!profile || !profile->IsValid()) {
            throw std::runtime_error("Failed to create color profile from ICC data");
        }

        A_Err err = suites.colorSettingsSuite->AEGP_SetWorkingColorSpace(
            state.GetPluginID(), m_compH, profile->GetHandle());
        if (err != A_Err_NONE) {
            throw std::runtime_error("AEGP_SetWorkingColorSpace failed");
        }
    }
    else {
        throw std::runtime_error("Expected ColorProfile object or ICC data (bytes)");
    }
}

} // namespace PyAE
