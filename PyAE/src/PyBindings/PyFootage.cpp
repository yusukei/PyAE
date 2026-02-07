// PyFootage.cpp
// PyAE - Python for After Effects
// フッテージクラスの実装（統合版）
//
// FootageSuite5の高レベルAPI
//
// Note: 統合版 - プロジェクト追加前(AEGP_FootageH)と追加後(AEGP_ItemH)を
//       単一のクラスで管理し、状態に応じたAPIを提供する

#include "PyFootageClasses.h"
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"

namespace PyAE {

// =============================================================
// PyFootage Implementation - Constructors & Destructor
// =============================================================

PyFootage::PyFootage()
    : m_footageH(nullptr)
    , m_itemH(nullptr)
    , m_owned(false)
{
}

PyFootage::PyFootage(AEGP_FootageH footageH, bool owned)
    : m_footageH(footageH)
    , m_itemH(nullptr)
    , m_owned(owned)
{
}

PyFootage::PyFootage(AEGP_ItemH itemH)
    : m_footageH(nullptr)
    , m_itemH(itemH)
    , m_owned(false)
{
    // Validate that this is a footage item
    if (m_itemH) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.itemSuite) {
            AEGP_ItemType type;
            A_Err err = suites.itemSuite->AEGP_GetItemType(m_itemH, &type);
            if (err != A_Err_NONE || type != AEGP_ItemType_FOOTAGE) {
                m_itemH = nullptr;
            }
        }
    }
}

PyFootage::PyFootage(PyFootage&& other) noexcept
    : m_footageH(other.m_footageH)
    , m_itemH(other.m_itemH)
    , m_owned(other.m_owned)
{
    other.m_footageH = nullptr;
    other.m_itemH = nullptr;
    other.m_owned = false;
}

PyFootage& PyFootage::operator=(PyFootage&& other) noexcept
{
    if (this != &other) {
        DisposeFootage();
        m_footageH = other.m_footageH;
        m_itemH = other.m_itemH;
        m_owned = other.m_owned;
        other.m_footageH = nullptr;
        other.m_itemH = nullptr;
        other.m_owned = false;
    }
    return *this;
}

PyFootage::~PyFootage()
{
    DisposeFootage();
}

void PyFootage::DisposeFootage()
{
    if (m_footageH && m_owned) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.footageSuite) {
            suites.footageSuite->AEGP_DisposeFootage(m_footageH);
        }
    }
    m_footageH = nullptr;
    m_owned = false;
    // m_itemH is not owned, so we don't dispose it
}

// =============================================================
// State Properties
// =============================================================

bool PyFootage::IsValid() const
{
    return m_footageH != nullptr || m_itemH != nullptr;
}

bool PyFootage::IsInProject() const
{
    return m_itemH != nullptr;
}

// =============================================================
// Helper Methods
// =============================================================

AEGP_FootageH PyFootage::GetMainFootageFromItem() const
{
    if (!m_itemH) return nullptr;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) return nullptr;

    AEGP_FootageH footageH = nullptr;
    A_Err err = suites.footageSuite->AEGP_GetMainFootageFromItem(m_itemH, &footageH);
    if (err != A_Err_NONE) return nullptr;

    return footageH;
}

// =============================================================
// Common Properties (work in both states)
// =============================================================

std::string PyFootage::GetPath() const
{
    AEGP_FootageH footageH = m_footageH;
    if (!footageH && m_itemH) {
        footageH = GetMainFootageFromItem();
    }
    if (!footageH) {
        throw std::runtime_error("Invalid footage");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite || !suites.memorySuite) {
        throw std::runtime_error("Required suites not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_MemHandle unicode_pathH = nullptr;

    A_Err err = suites.footageSuite->AEGP_GetFootagePath(
        footageH, 0, AEGP_FOOTAGE_MAIN_FILE_INDEX, &unicode_pathH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetFootagePath failed");
    }

    if (!unicode_pathH) {
        return "";
    }

    ScopedMemHandle scoped(plugin_id, suites.memorySuite, unicode_pathH);
    ScopedMemLock lock(suites.memorySuite, unicode_pathH);

    if (!lock.IsValid()) {
        throw std::runtime_error("Failed to lock memory handle");
    }

    return StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
}

py::dict PyFootage::GetLayerKey() const
{
    AEGP_FootageH footageH = m_footageH;
    if (!footageH && m_itemH) {
        footageH = GetMainFootageFromItem();
    }
    if (!footageH) {
        throw std::runtime_error("Invalid footage");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    AEGP_FootageLayerKey layerKey = {};
    A_Err err = suites.footageSuite->AEGP_GetFootageLayerKey(footageH, &layerKey);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetFootageLayerKey failed");
    }

    py::dict result;
    result["layer_id"] = static_cast<int>(layerKey.layer_idL);
    result["layer_index"] = static_cast<int>(layerKey.layer_indexL);
    result["name"] = std::string(layerKey.nameAC);
    result["layer_draw_style"] = static_cast<int>(layerKey.layer_draw_style);
    return result;
}

py::dict PyFootage::GetSoundFormat() const
{
    AEGP_FootageH footageH = m_footageH;
    if (!footageH && m_itemH) {
        footageH = GetMainFootageFromItem();
    }
    if (!footageH) {
        throw std::runtime_error("Invalid footage");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    AEGP_SoundDataFormat format = {};
    A_Err err = suites.footageSuite->AEGP_GetFootageSoundDataFormat(footageH, &format);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetFootageSoundDataFormat failed");
    }

    py::dict result;
    result["sample_rate"] = format.sample_rateF;
    result["encoding"] = static_cast<int>(format.encoding);
    result["bytes_per_sample"] = static_cast<int>(format.bytes_per_sampleL);
    result["num_channels"] = static_cast<int>(format.num_channelsL);
    return result;
}

py::dict PyFootage::GetSequenceOptions() const
{
    AEGP_FootageH footageH = m_footageH;
    if (!footageH && m_itemH) {
        footageH = GetMainFootageFromItem();
    }
    if (!footageH) {
        return py::dict();
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    AEGP_FileSequenceImportOptions options = {};
    A_Err err = suites.footageSuite->AEGP_GetFootageSequenceImportOptions(footageH, &options);
    if (err != A_Err_NONE) {
        return py::dict();
    }

    py::dict result;
    result["all_in_folder"] = options.all_in_folderB != 0;
    result["force_alphabetical"] = options.force_alphabeticalB != 0;
    result["start_frame"] = options.start_frameL;
    result["end_frame"] = options.end_frameL;
    return result;
}

// =============================================================
// Pre-Project Properties (only when is_in_project == false)
// =============================================================

std::tuple<int, int> PyFootage::GetNumFiles() const
{
    if (!m_footageH) {
        throw std::runtime_error("Footage not available (already in project?)");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    A_long num_main_files = 0;
    A_long files_per_frame = 0;
    A_Err err = suites.footageSuite->AEGP_GetFootageNumFiles(
        m_footageH, &num_main_files, &files_per_frame);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetFootageNumFiles failed");
    }

    return std::make_tuple(static_cast<int>(num_main_files), static_cast<int>(files_per_frame));
}

FootageSignature PyFootage::GetSignature() const
{
    AEGP_FootageH footageH = m_footageH;
    if (!footageH && m_itemH) {
        footageH = GetMainFootageFromItem();
    }
    if (!footageH) {
        throw std::runtime_error("Invalid footage");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    AEGP_FootageSignature sig = AEGP_FootageSignature_NONE;
    A_Err err = suites.footageSuite->AEGP_GetFootageSignature(footageH, &sig);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetFootageSignature failed");
    }

    // Map SDK FourCC values to our simplified enum
    if (sig == AEGP_FootageSignature_SOLID) {
        return FootageSignature::SOLID;
    } else if (sig == AEGP_FootageSignature_MISSING) {
        return FootageSignature::MISSING;
    }
    return FootageSignature::NONE;
}

// =============================================================
// Project Operations
// =============================================================

uintptr_t PyFootage::AddToProject(uintptr_t folderH)
{
    if (!m_footageH) {
        if (m_itemH) {
            throw std::runtime_error("Footage already in project");
        }
        throw std::runtime_error("Invalid footage");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    // If no folder specified, use project root folder
    AEGP_ItemH folder = reinterpret_cast<AEGP_ItemH>(folderH);
    if (!folder) {
        if (!suites.projSuite) {
            throw std::runtime_error("Project Suite not available");
        }
        AEGP_ProjectH projH = nullptr;
        A_Err projErr = suites.projSuite->AEGP_GetProjectByIndex(0, &projH);
        if (projErr != A_Err_NONE || !projH) {
            throw std::runtime_error("Failed to get current project");
        }
        projErr = suites.projSuite->AEGP_GetProjectRootFolder(projH, &folder);
        if (projErr != A_Err_NONE || !folder) {
            throw std::runtime_error("Failed to get project root folder");
        }
    }

    AEGP_ItemH newItemH = nullptr;

    A_Err err = suites.footageSuite->AEGP_AddFootageToProject(m_footageH, folder, &newItemH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_AddFootageToProject failed");
    }

    // After adding to project, the footage handle is managed by AE
    m_owned = false;
    m_footageH = nullptr;  // Clear footageH as it's now managed by AE
    m_itemH = newItemH;    // Store the item handle for future operations

    return reinterpret_cast<uintptr_t>(newItemH);
}

// =============================================================
// In-Project Properties (only when is_in_project == true)
// =============================================================

FootageType PyFootage::GetFootageType() const
{
    if (!m_itemH) {
        return FootageType::None;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.footageSuite || !suites.itemSuite) {
        throw std::runtime_error("Footage/Item suite not available");
    }

    // Check missing flag
    AEGP_ItemFlags flags;
    A_Err err = suites.itemSuite->AEGP_GetItemFlags(m_itemH, &flags);
    if (err == A_Err_NONE && (flags & AEGP_ItemFlag_MISSING)) {
        return FootageType::Missing;
    }

    // Check solid
    AEGP_FootageH footageH = GetMainFootageFromItem();
    if (!footageH) {
        return FootageType::None;
    }

    AEGP_FootageSignature sig;
    err = suites.footageSuite->AEGP_GetFootageSignature(footageH, &sig);
    if (err == A_Err_NONE) {
        if (sig == AEGP_FootageSignature_SOLID) {
            return FootageType::Solid;
        }
    }

    return FootageType::None;
}

py::dict PyFootage::GetInterpretation() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.footageSuite) {
        throw std::runtime_error("Footage suite not available");
    }

    AEGP_FootageInterp interp;
    A_Err err = suites.footageSuite->AEGP_GetFootageInterpretation(m_itemH, false, &interp);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get footage interpretation");
    }

    py::dict result;

    // Alpha mode
    std::string alphaMode = "straight";
    if (interp.al.flags & AEGP_AlphaIgnore) {
        alphaMode = "ignore";
    } else if (interp.al.flags & AEGP_AlphaPremul) {
        alphaMode = "premultiplied";
    }
    result["alpha_mode"] = alphaMode;

    // Premul color
    if (interp.al.flags & AEGP_AlphaPremul) {
        result["premul_color"] = py::make_tuple(
            interp.al.redCu / 255.0,
            interp.al.greenCu / 255.0,
            interp.al.blueCu / 255.0
        );
    }

    // Frame rates
    if (interp.native_fpsF > 0) {
        result["native_fps"] = interp.native_fpsF;
    }
    if (interp.conform_fpsF > 0) {
        result["conform_fps"] = interp.conform_fpsF;
    }

    return result;
}

void PyFootage::SetInterpretation(const std::string& alphaMode, double fps)
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.footageSuite) {
        throw std::runtime_error("Footage suite not available");
    }

    // Get current settings
    AEGP_FootageInterp interp;
    A_Err err = suites.footageSuite->AEGP_GetFootageInterpretation(m_itemH, false, &interp);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get current footage interpretation");
    }

    // Set alpha mode
    if (!alphaMode.empty()) {
        interp.al.flags = 0;
        if (alphaMode == "premultiplied") {
            interp.al.flags = AEGP_AlphaPremul;
        } else if (alphaMode == "straight") {
            interp.al.flags = 0;
        } else if (alphaMode == "ignore") {
            interp.al.flags = AEGP_AlphaIgnore;
        }
    }

    // Set frame rate
    if (fps > 0.0) {
        interp.conform_fpsF = fps;
    }

    err = suites.footageSuite->AEGP_SetFootageInterpretation(m_itemH, false, &interp);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set footage interpretation");
    }
}

py::tuple PyFootage::GetSolidColor(bool proxy) const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.footageSuite) {
        throw std::runtime_error("Footage suite not available");
    }

    AEGP_ColorVal color;
    A_Err err = suites.footageSuite->AEGP_GetSolidFootageColor(m_itemH, proxy ? TRUE : FALSE, &color);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get solid footage color (item may not be solid)");
    }

    return py::make_tuple((float)color.redF, (float)color.greenF, (float)color.blueF, (float)color.alphaF);
}

void PyFootage::SetSolidColor(const py::tuple& color_rgba, bool proxy)
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.footageSuite) {
        throw std::runtime_error("Footage suite not available");
    }

    AEGP_ColorVal c;
    if (py::len(color_rgba) >= 3) {
        c.redF = color_rgba[0].cast<float>();
        c.greenF = color_rgba[1].cast<float>();
        c.blueF = color_rgba[2].cast<float>();
        if (py::len(color_rgba) >= 4) {
            c.alphaF = color_rgba[3].cast<float>();
        } else {
            c.alphaF = 1.0f;
        }
    } else {
        throw std::invalid_argument("Color tuple must be (r,g,b) or (r,g,b,a)");
    }

    A_Err err = suites.footageSuite->AEGP_SetSolidFootageColor(m_itemH, proxy ? TRUE : FALSE, &c);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set solid footage color (item may not be solid)");
    }
}

void PyFootage::SetSolidDimensions(int width, int height, bool proxy)
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.footageSuite) {
        throw std::runtime_error("Footage suite not available");
    }

    if (width < 1 || width > 30000 || height < 1 || height > 30000) {
        throw std::invalid_argument("Width and height must be between 1 and 30000");
    }

    A_Err err = suites.footageSuite->AEGP_SetSolidFootageDimensions(
        m_itemH, proxy ? TRUE : FALSE, (A_long)width, (A_long)height);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set solid footage dimensions (item may not be solid)");
    }
}

// =============================================================
// Item Properties (delegated to underlying ItemH)
// =============================================================

std::string PyFootage::GetName() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite || !suites.memorySuite) {
        throw std::runtime_error("Item/Memory suite not available");
    }

    AEGP_MemHandle nameH = nullptr;
    A_Err err = suites.itemSuite->AEGP_GetItemName(state.GetPluginID(), m_itemH, &nameH);
    if (err != A_Err_NONE || !nameH) {
        return "";
    }

    ScopedMemHandle scoped(state.GetPluginID(), suites.memorySuite, nameH);
    ScopedMemLock lock(suites.memorySuite, nameH);

    if (!lock.IsValid()) {
        return "";
    }

    return StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
}

void PyFootage::SetName(const std::string& name)
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    std::wstring wideName = StringUtils::Utf8ToWide(name);
    A_Err err = suites.itemSuite->AEGP_SetItemName(
        m_itemH, reinterpret_cast<const A_UTF16Char*>(wideName.c_str()));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set item name");
    }
}

std::string PyFootage::GetComment() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite || !suites.memorySuite) {
        throw std::runtime_error("Item/Memory suite not available");
    }

    AEGP_MemHandle commentH = nullptr;
    A_Err err = suites.itemSuite->AEGP_GetItemComment(m_itemH, &commentH);
    if (err != A_Err_NONE || !commentH) {
        return "";
    }

    ScopedMemHandle scoped(state.GetPluginID(), suites.memorySuite, commentH);
    ScopedMemLock lock(suites.memorySuite, commentH);

    if (!lock.IsValid()) {
        return "";
    }

    return StringUtils::Utf16ToUtf8(lock.As<A_UTF16Char>());
}

void PyFootage::SetComment(const std::string& comment)
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    std::wstring wideComment = StringUtils::Utf8ToWide(comment);
    A_Err err = suites.itemSuite->AEGP_SetItemComment(
        m_itemH, reinterpret_cast<const A_UTF16Char*>(wideComment.c_str()));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set item comment");
    }
}

int PyFootage::GetLabel() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    AEGP_LabelID label;
    A_Err err = suites.itemSuite->AEGP_GetItemLabel(m_itemH, &label);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get item label");
    }

    return static_cast<int>(label);
}

void PyFootage::SetLabel(int label)
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    A_Err err = suites.itemSuite->AEGP_SetItemLabel(m_itemH, static_cast<AEGP_LabelID>(label));
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set item label");
    }
}

bool PyFootage::IsSelected() const
{
    if (!m_itemH) {
        return false;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        return false;
    }

    A_Boolean selected = FALSE;
    A_Err err = suites.itemSuite->AEGP_IsItemSelected(m_itemH, &selected);
    if (err != A_Err_NONE) {
        return false;
    }

    return selected != FALSE;
}

void PyFootage::SetSelected(bool selected)
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    A_Err err = suites.itemSuite->AEGP_SelectItem(m_itemH, selected ? TRUE : FALSE, TRUE);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to set item selection");
    }
}

int PyFootage::GetWidth() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    A_long width = 0, height = 0;
    A_Err err = suites.itemSuite->AEGP_GetItemDimensions(m_itemH, &width, &height);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get item dimensions");
    }

    return static_cast<int>(width);
}

int PyFootage::GetHeight() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    A_long width = 0, height = 0;
    A_Err err = suites.itemSuite->AEGP_GetItemDimensions(m_itemH, &width, &height);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get item dimensions");
    }

    return static_cast<int>(height);
}

double PyFootage::GetDuration() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    A_Time duration = {0, 1};
    A_Err err = suites.itemSuite->AEGP_GetItemDuration(m_itemH, &duration);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get item duration");
    }

    return AETypeUtils::TimeToSeconds(duration);
}

double PyFootage::GetFrameRate() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    // For footage, get interpretation fps
    py::dict interp = GetInterpretation();
    if (interp.contains("native_fps")) {
        return interp["native_fps"].cast<double>();
    }
    if (interp.contains("conform_fps")) {
        return interp["conform_fps"].cast<double>();
    }

    return 0.0;
}

bool PyFootage::IsMissing() const
{
    if (!m_itemH) {
        return false;
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        return false;
    }

    AEGP_ItemFlags flags;
    A_Err err = suites.itemSuite->AEGP_GetItemFlags(m_itemH, &flags);
    if (err != A_Err_NONE) {
        return false;
    }

    return (flags & AEGP_ItemFlag_MISSING) != 0;
}

int PyFootage::GetItemId() const
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    A_long id = 0;
    A_Err err = suites.itemSuite->AEGP_GetItemID(m_itemH, &id);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get item ID");
    }

    return static_cast<int>(id);
}

void PyFootage::Delete()
{
    if (!m_itemH) {
        throw std::runtime_error("Footage not in project");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.itemSuite) {
        throw std::runtime_error("Item suite not available");
    }

    A_Err err = suites.itemSuite->AEGP_DeleteItem(m_itemH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to delete item");
    }

    m_itemH = nullptr;
}

// =============================================================
// Handle access
// =============================================================

AEGP_FootageH PyFootage::GetFootageHandle() const
{
    return m_footageH;
}

AEGP_ItemH PyFootage::GetItemHandle() const
{
    return m_itemH;
}

AEGP_FootageH PyFootage::ReleaseFootageHandle()
{
    AEGP_FootageH handle = m_footageH;
    m_footageH = nullptr;
    m_owned = false;
    return handle;
}

bool PyFootage::IsOwned() const
{
    return m_owned;
}

// =============================================================
// Static factory methods
// =============================================================

std::shared_ptr<PyFootage> PyFootage::FromFile(const std::string& path, InterpretationStyle style)
{
    if (path.empty()) {
        throw std::runtime_error("Path cannot be empty");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();

    std::wstring widePath = StringUtils::Utf8ToWide(path);
    AEGP_FootageH footageH = nullptr;

    A_Err err = suites.footageSuite->AEGP_NewFootage(
        plugin_id,
        reinterpret_cast<const A_UTF16Char*>(widePath.c_str()),
        nullptr,
        nullptr,
        static_cast<AEGP_InterpretationStyle>(style),
        nullptr,
        &footageH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewFootage failed");
    }

    return std::make_shared<PyFootage>(footageH, true);
}

std::shared_ptr<PyFootage> PyFootage::FromItem(AEGP_ItemH itemH)
{
    if (!itemH) {
        throw std::runtime_error("Invalid item handle");
    }

    return std::make_shared<PyFootage>(itemH);
}

std::shared_ptr<PyFootage> PyFootage::Solid(
    const std::string& name,
    int width,
    int height,
    const std::tuple<float, float, float, float>& color)
{
    if (name.empty()) {
        throw std::runtime_error("Name cannot be empty");
    }
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Dimensions must be positive");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    AEGP_ColorVal colorVal = {};
    colorVal.redF = static_cast<A_FpLong>(std::get<0>(color));
    colorVal.greenF = static_cast<A_FpLong>(std::get<1>(color));
    colorVal.blueF = static_cast<A_FpLong>(std::get<2>(color));
    colorVal.alphaF = static_cast<A_FpLong>(std::get<3>(color));

    AEGP_FootageH footageH = nullptr;

    A_Err err = suites.footageSuite->AEGP_NewSolidFootage(
        name.c_str(),
        static_cast<A_long>(width),
        static_cast<A_long>(height),
        &colorVal,
        &footageH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewSolidFootage failed");
    }

    return std::make_shared<PyFootage>(footageH, true);
}

std::shared_ptr<PyFootage> PyFootage::Placeholder(
    const std::string& name,
    int width,
    int height,
    float duration)
{
    if (name.empty()) {
        throw std::runtime_error("Name cannot be empty");
    }
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Dimensions must be positive");
    }
    if (duration <= 0) {
        throw std::runtime_error("Duration must be positive");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();

    A_Time durationTime = AETypeUtils::SecondsToTime(duration);

    AEGP_FootageH footageH = nullptr;

    A_Err err = suites.footageSuite->AEGP_NewPlaceholderFootage(
        plugin_id,
        name.c_str(),
        static_cast<A_long>(width),
        static_cast<A_long>(height),
        &durationTime,
        &footageH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewPlaceholderFootage failed");
    }

    return std::make_shared<PyFootage>(footageH, true);
}

std::shared_ptr<PyFootage> PyFootage::PlaceholderWithPath(
    const std::string& path,
    int width,
    int height,
    float duration)
{
    if (path.empty()) {
        throw std::runtime_error("Path cannot be empty");
    }
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Dimensions must be positive");
    }
    if (duration <= 0) {
        throw std::runtime_error("Duration must be positive");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.footageSuite) {
        throw std::runtime_error("Footage Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();

    std::wstring widePath = StringUtils::Utf8ToWide(path);

    A_Time durationTime = AETypeUtils::SecondsToTime(duration);

    AEGP_FootageH footageH = nullptr;

    A_Err err = suites.footageSuite->AEGP_NewPlaceholderFootageWithPath(
        plugin_id,
        reinterpret_cast<const A_UTF16Char*>(widePath.c_str()),
        static_cast<AEGP_Platform>(0),
        static_cast<AEIO_FileType>(0),
        static_cast<A_long>(width),
        static_cast<A_long>(height),
        &durationTime,
        &footageH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewPlaceholderFootageWithPath failed");
    }

    return std::make_shared<PyFootage>(footageH, true);
}

// =============================================================
// Python Bindings
// =============================================================

void init_footage(py::module_& m)
{
    // FootageSignature enum
    py::enum_<FootageSignature>(m, "FootageSignature",
        "Footage signature (source state).\n\n"
        "Values:\n"
        "    NONE: No footage / invalid\n"
        "    MISSING: Source file is missing\n"
        "    SOLID: Solid color footage")
        .value("NONE", FootageSignature::NONE, "No footage / invalid")
        .value("MISSING", FootageSignature::MISSING, "Source file is missing")
        .value("SOLID", FootageSignature::SOLID, "Solid color footage")
        .export_values();

    // FootageType enum
    py::enum_<FootageType>(m, "FootageType",
        "Footage type (in-project state).\n\n"
        "Values:\n"
        "    None: Invalid or unknown\n"
        "    Solid: Solid color footage\n"
        "    Missing: Source file is missing\n"
        "    Placeholder: Placeholder footage")
        .value("None_", FootageType::None, "Invalid or unknown")
        .value("Solid", FootageType::Solid, "Solid color footage")
        .value("Missing", FootageType::Missing, "Source file is missing")
        .value("Placeholder", FootageType::Placeholder, "Placeholder footage")
        .export_values();

    // InterpretationStyle enum
    py::enum_<InterpretationStyle>(m, "InterpretationStyle",
        "Footage interpretation style.\n\n"
        "Values:\n"
        "    NO_DIALOG_NO_GUESS: Use stored interpretation\n"
        "    DIALOG_OK: Show dialog if needed\n"
        "    NO_DIALOG_GUESS: Guess without dialog (default)")
        .value("NO_DIALOG_NO_GUESS", InterpretationStyle::NO_DIALOG_NO_GUESS,
            "Use stored interpretation")
        .value("DIALOG_OK", InterpretationStyle::DIALOG_OK,
            "Show dialog if needed")
        .value("NO_DIALOG_GUESS", InterpretationStyle::NO_DIALOG_GUESS,
            "Guess without dialog (default)")
        .export_values();

    // Footage class (unified)
    py::class_<PyFootage, std::shared_ptr<PyFootage>>(m, "Footage",
        "Unified footage class for both pre-project and in-project states.\n\n"
        "A Footage object can be in one of two states:\n"
        "- Before project: Created via factory methods (from_file, solid, etc.)\n"
        "- In project: After add_to_project() or retrieved from project.items\n\n"
        "Use is_in_project property to check the current state.\n\n"
        "Example::\n\n"
        "    # Create and add footage\n"
        "    footage = ae.Footage.from_file('/path/to/video.mp4')\n"
        "    print(footage.is_in_project)  # False\n"
        "    \n"
        "    footage.add_to_project()\n"
        "    print(footage.is_in_project)  # True\n"
        "    \n"
        "    # Now you can access in-project properties\n"
        "    footage.name = 'My Video'\n"
        "    footage.set_interpretation('premultiplied', fps=29.97)\n"
        "    \n"
        "    # Create a solid\n"
        "    solid = ae.Footage.solid('Red', 1920, 1080, (1.0, 0.0, 0.0, 1.0))\n"
        "    solid.add_to_project()\n"
        "    solid.set_solid_color((0.5, 0.5, 0.5, 1.0))")

        // State properties
        .def_property_readonly("valid", &PyFootage::IsValid,
            "Check if footage is valid")

        .def_property_readonly("is_in_project", &PyFootage::IsInProject,
            "Check if footage has been added to project")

        // Item type (always Footage - for compatibility with PyItem interface)
        .def_property_readonly("type", [](const PyFootage&) -> py::object {
            return py::module_::import("ae").attr("ItemType").attr("Footage");
        }, "Get item type (always ItemType.Footage)")

        // Common properties (both states)
        .def_property_readonly("path", &PyFootage::GetPath,
            "Get file path")

        .def_property_readonly("layer_key", &PyFootage::GetLayerKey,
            "Get layer key info (for layered formats like PSD)")

        .def_property_readonly("sound_format", &PyFootage::GetSoundFormat,
            "Get sound data format info")

        .def_property_readonly("sequence_options", &PyFootage::GetSequenceOptions,
            "Get sequence import options")

        // Pre-project properties
        .def_property_readonly("num_files", &PyFootage::GetNumFiles,
            "Get number of files as (total_files, files_per_frame) tuple.\n"
            "Only available before adding to project.")

        .def_property_readonly("signature", &PyFootage::GetSignature,
            "Get footage signature (NONE, MISSING, or SOLID)")

        // Project operation
        .def("add_to_project", [](PyFootage& self, py::object folder) -> uintptr_t {
            uintptr_t folderH = 0;
            if (folder.is_none()) {
                folderH = 0;  // root folder
            } else if (py::isinstance<py::int_>(folder)) {
                folderH = folder.cast<uintptr_t>();
            } else if (py::hasattr(folder, "_handle")) {
                folderH = folder.attr("_handle").cast<uintptr_t>();
            } else {
                throw py::type_error("folder must be None, int, or Folder object");
            }
            return self.AddToProject(folderH);
        },
            "Add footage to project.\n\n"
            "Args:\n"
            "    folder: Parent folder (None for root, or Folder object)\n\n"
            "Returns:\n"
            "    Created item handle\n\n"
            "After calling, is_in_project becomes True.",
            py::arg("folder") = py::none())

        // In-project properties
        .def_property_readonly("footage_type", &PyFootage::GetFootageType,
            "Get footage type (Solid, Missing, Placeholder, None).\n"
            "Only available after adding to project.")

        .def_property_readonly("interpretation", &PyFootage::GetInterpretation,
            "Get interpretation rules.\n"
            "Only available after adding to project.")

        .def("set_interpretation", &PyFootage::SetInterpretation,
            "Set interpretation rules.\n\n"
            "Args:\n"
            "    alpha_mode: 'straight', 'premultiplied', or 'ignore'\n"
            "    fps: Conform frame rate (0.0 = no change)\n\n"
            "Only available after adding to project.",
            py::arg("alpha_mode") = "", py::arg("fps") = 0.0)

        .def("get_solid_color", &PyFootage::GetSolidColor,
            "Get solid color (RGBA, 0.0-1.0).\n"
            "Only available for solid footage in project.",
            py::arg("proxy") = false)

        .def("set_solid_color", &PyFootage::SetSolidColor,
            "Set solid color.\n\n"
            "Args:\n"
            "    color_rgba: (r, g, b, a) tuple (0.0-1.0)\n"
            "    proxy: Use proxy instead of main\n\n"
            "Only available for solid footage in project.",
            py::arg("color_rgba"), py::arg("proxy") = false)

        .def("set_solid_dimensions", &PyFootage::SetSolidDimensions,
            "Set solid dimensions.\n\n"
            "Args:\n"
            "    width: Width in pixels (1-30000)\n"
            "    height: Height in pixels (1-30000)\n"
            "    proxy: Use proxy instead of main\n\n"
            "Only available for solid footage in project.",
            py::arg("width"), py::arg("height"), py::arg("proxy") = false)

        // Item properties (in-project only)
        .def_property("name", &PyFootage::GetName, &PyFootage::SetName,
            "Get or set the name. Only available in project.")

        .def_property("comment", &PyFootage::GetComment, &PyFootage::SetComment,
            "Get or set the comment. Only available in project.")

        .def_property("label", &PyFootage::GetLabel, &PyFootage::SetLabel,
            "Get or set the label. Only available in project.")

        .def_property("selected", &PyFootage::IsSelected, &PyFootage::SetSelected,
            "Get or set selection state. Only available in project.")

        .def_property_readonly("width", &PyFootage::GetWidth,
            "Get width. Only available in project.")

        .def_property_readonly("height", &PyFootage::GetHeight,
            "Get height. Only available in project.")

        .def_property_readonly("duration", &PyFootage::GetDuration,
            "Get duration in seconds. Only available in project.")

        .def_property_readonly("frame_rate", &PyFootage::GetFrameRate,
            "Get frame rate. Only available in project.")

        .def_property_readonly("missing", &PyFootage::IsMissing,
            "Check if source file is missing. Only available in project.")

        .def_property_readonly("item_id", &PyFootage::GetItemId,
            "Get item ID. Only available in project.")

        // Alias for compatibility with PyItem interface
        .def_property_readonly("id", &PyFootage::GetItemId,
            "Get item ID (alias for item_id). Only available in project.")

        // Parent folder (for compatibility with PyItem interface)
        .def_property_readonly("parent_folder", [](const PyFootage& self) -> py::object {
            if (!self.IsInProject()) return py::none();
            AEGP_ItemH itemH = self.GetItemHandle();
            if (!itemH) return py::none();

            auto& state = PluginState::Instance();
            const auto& suites = state.GetSuites();
            if (!suites.itemSuite) return py::none();

            AEGP_ItemH parentH = nullptr;
            A_Err err = suites.itemSuite->AEGP_GetItemParentFolder(itemH, &parentH);
            if (err != A_Err_NONE || !parentH) return py::none();

            // Get parent folder's item ID
            A_long parentId = 0;
            err = suites.itemSuite->AEGP_GetItemID(parentH, &parentId);
            if (err != A_Err_NONE) return py::none();

            // Return dict with id attribute for lightweight access
            py::dict result;
            result["id"] = parentId;
            result["_handle"] = reinterpret_cast<uintptr_t>(parentH);
            return result;
        }, "Get parent folder. Only available in project.")

        .def("delete", &PyFootage::Delete,
            "Delete from project. Only available in project.")

        // Handle access
        .def_property_readonly("_handle", [](const PyFootage& self) {
            // Return item handle if in project, otherwise footage handle
            if (self.IsInProject()) {
                return reinterpret_cast<uintptr_t>(self.GetItemHandle());
            }
            return reinterpret_cast<uintptr_t>(self.GetFootageHandle());
        }, "Internal: Get raw handle as integer")

        .def_property_readonly("_item_handle", [](const PyFootage& self) {
            return reinterpret_cast<uintptr_t>(self.GetItemHandle());
        }, "Internal: Get item handle as integer")

        .def_property_readonly("_footage_handle", [](const PyFootage& self) {
            return reinterpret_cast<uintptr_t>(self.GetFootageHandle());
        }, "Internal: Get footage handle as integer")

        // Factory methods
        .def_static("from_file", &PyFootage::FromFile,
            "Create footage from file.\n\n"
            "Args:\n"
            "    path: File path\n"
            "    style: Interpretation style (default: NO_DIALOG_GUESS)\n\n"
            "Returns:\n"
            "    New Footage instance (not yet in project)",
            py::arg("path"),
            py::arg("style") = InterpretationStyle::NO_DIALOG_GUESS)

        .def_static("solid", &PyFootage::Solid,
            "Create solid color footage.\n\n"
            "Args:\n"
            "    name: Solid name\n"
            "    width: Width in pixels\n"
            "    height: Height in pixels\n"
            "    color: RGBA tuple (0.0-1.0)\n\n"
            "Returns:\n"
            "    New Footage instance (not yet in project)",
            py::arg("name"), py::arg("width"), py::arg("height"), py::arg("color"))

        .def_static("placeholder", &PyFootage::Placeholder,
            "Create placeholder footage.\n\n"
            "Args:\n"
            "    name: Placeholder name\n"
            "    width: Width in pixels\n"
            "    height: Height in pixels\n"
            "    duration: Duration in seconds\n\n"
            "Returns:\n"
            "    New Footage instance (not yet in project)",
            py::arg("name"), py::arg("width"), py::arg("height"), py::arg("duration"))

        .def_static("placeholder_with_path", &PyFootage::PlaceholderWithPath,
            "Create placeholder footage with file path.\n\n"
            "Args:\n"
            "    path: Placeholder file path\n"
            "    width: Width in pixels\n"
            "    height: Height in pixels\n"
            "    duration: Duration in seconds\n\n"
            "Returns:\n"
            "    New Footage instance (not yet in project)",
            py::arg("path"), py::arg("width"), py::arg("height"), py::arg("duration"))

        .def("__repr__", [](const PyFootage& self) {
            if (!self.IsValid()) {
                return std::string("<Footage: invalid>");
            }
            try {
                std::string status = self.IsInProject() ? "in project" : "not in project";
                std::string path = self.GetPath();
                if (!path.empty()) {
                    size_t pos = path.find_last_of("/\\");
                    if (pos != std::string::npos) {
                        path = path.substr(pos + 1);
                    }
                    return std::string("<Footage: ") + path + " (" + status + ")>";
                }
                FootageSignature sig = self.GetSignature();
                if (sig == FootageSignature::SOLID) {
                    return std::string("<Footage: solid (" + status + ")>");
                }
                return std::string("<Footage: " + status + ">");
            } catch (...) {
                return std::string("<Footage: valid>");
            }
        });
}

} // namespace PyAE
