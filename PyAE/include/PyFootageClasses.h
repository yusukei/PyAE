// PyFootageClasses.h
// PyAE - Python for After Effects
// フッテージクラスの宣言（統合版）
//
// FootageSuite5の高レベルAPI
//
// Note: 統合版 - プロジェクト追加前(AEGP_FootageH)と追加後(AEGP_ItemH)を
//       単一のクラスで管理し、状態に応じたAPIを提供する

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <tuple>
#include <memory>

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

// =============================================================
// FootageSignature - フッテージの署名（ソース状態）
// プロジェクト追加前のフッテージソースの状態
// =============================================================
enum class FootageSignature {
    NONE = 0,    // AEGP_FootageSignature_NONE - No footage / invalid sig
    MISSING = 1, // AEGP_FootageSignature_MISSING - Placeholder
    SOLID = 2    // AEGP_FootageSignature_SOLID - Solid footage
};

// =============================================================
// FootageType - フッテージタイプ（プロジェクト内）
// プロジェクト追加後のフッテージアイテムの種類
// =============================================================
enum class FootageType {
    None = 0,        // Invalid or unknown
    Solid = 1,       // Solid color
    Missing = 2,     // Source file missing
    Placeholder = 3  // Placeholder
};

// =============================================================
// InterpretationStyle - フッテージ解釈スタイル
// =============================================================
enum class InterpretationStyle {
    NO_DIALOG_NO_GUESS = 0,  // Use stored interpretation
    DIALOG_OK = 1,           // Show dialog if needed
    NO_DIALOG_GUESS = 2      // Guess without dialog
};

// =============================================================
// PyFootage - フッテージクラス（統合版）
//
// プロジェクト追加前(AEGP_FootageH)と追加後(AEGP_ItemH)の
// 両方の状態を1つのクラスで管理する
//
// 状態:
// - is_in_project == false: footageHが有効、追加前
// - is_in_project == true:  itemHが有効、追加後
// =============================================================
class PyFootage {
public:
    // =============================================================
    // Constructors
    // =============================================================

    // Default constructor (invalid footage)
    PyFootage();

    // Constructor from footage handle (before adding to project)
    explicit PyFootage(AEGP_FootageH footageH, bool owned = true);

    // Constructor from item handle (after adding to project)
    explicit PyFootage(AEGP_ItemH itemH);

    // Copy constructor (disabled)
    PyFootage(const PyFootage&) = delete;
    PyFootage& operator=(const PyFootage&) = delete;

    // Move constructor
    PyFootage(PyFootage&& other) noexcept;
    PyFootage& operator=(PyFootage&& other) noexcept;

    // Destructor - disposes the footage if owned and not added to project
    ~PyFootage();

    // =============================================================
    // State Properties
    // =============================================================

    // Check if footage is valid (either footageH or itemH is valid)
    bool IsValid() const;

    // Check if footage has been added to project (itemH is valid)
    bool IsInProject() const;

    // =============================================================
    // Common Properties (work in both states)
    // =============================================================

    // Get file path
    std::string GetPath() const;

    // Get layer key info (for layered formats like PSD)
    py::dict GetLayerKey() const;

    // Get sound data format info
    py::dict GetSoundFormat() const;

    // Get sequence import options
    py::dict GetSequenceOptions() const;

    // =============================================================
    // Pre-Project Properties (only when is_in_project == false)
    // =============================================================

    // Get number of files: (total_files, files_per_frame)
    std::tuple<int, int> GetNumFiles() const;

    // Get footage signature (NONE, MISSING, SOLID)
    FootageSignature GetSignature() const;

    // =============================================================
    // Project Operations
    // =============================================================

    // Add footage to project
    // folder: parent folder (0 for root)
    // Returns the created item handle
    // Note: After adding, is_in_project becomes true
    uintptr_t AddToProject(uintptr_t folderH = 0);

    // =============================================================
    // In-Project Properties (only when is_in_project == true)
    // =============================================================

    // Get footage type (Solid, Missing, Placeholder, None)
    FootageType GetFootageType() const;

    // Get interpretation rules
    py::dict GetInterpretation() const;

    // Set interpretation rules
    void SetInterpretation(const std::string& alphaMode, double fps = 0.0);

    // Get solid color (RGBA, 0.0-1.0)
    py::tuple GetSolidColor(bool proxy = false) const;

    // Set solid color
    void SetSolidColor(const py::tuple& color_rgba, bool proxy = false);

    // Set solid dimensions
    void SetSolidDimensions(int width, int height, bool proxy = false);

    // =============================================================
    // Item Properties (delegated to underlying ItemH)
    // Only available when is_in_project == true
    // =============================================================

    // Get/Set name
    std::string GetName() const;
    void SetName(const std::string& name);

    // Get/Set comment
    std::string GetComment() const;
    void SetComment(const std::string& comment);

    // Get/Set label
    int GetLabel() const;
    void SetLabel(int label);

    // Get/Set selected
    bool IsSelected() const;
    void SetSelected(bool selected);

    // Get dimensions
    int GetWidth() const;
    int GetHeight() const;

    // Get duration (seconds)
    double GetDuration() const;

    // Get frame rate
    double GetFrameRate() const;

    // Check if missing
    bool IsMissing() const;

    // Get item ID
    int GetItemId() const;

    // Delete from project
    void Delete();

    // =============================================================
    // Handle access
    // =============================================================

    // Get raw footage handle (for internal use)
    AEGP_FootageH GetFootageHandle() const;

    // Get raw item handle (for internal use)
    AEGP_ItemH GetItemHandle() const;

    // Release footage handle ownership (caller takes ownership)
    AEGP_FootageH ReleaseFootageHandle();

    // Check if this footage owns its footage handle
    bool IsOwned() const;

    // =============================================================
    // Static factory methods
    // =============================================================

    // Create footage from file
    static std::shared_ptr<PyFootage> FromFile(
        const std::string& path,
        InterpretationStyle style = InterpretationStyle::NO_DIALOG_GUESS);

    // Create footage from existing item handle (for ResolveItemHandle)
    static std::shared_ptr<PyFootage> FromItem(AEGP_ItemH itemH);

    // Create solid footage
    static std::shared_ptr<PyFootage> Solid(
        const std::string& name,
        int width,
        int height,
        const std::tuple<float, float, float, float>& color);

    // Create placeholder footage
    static std::shared_ptr<PyFootage> Placeholder(
        const std::string& name,
        int width,
        int height,
        float duration);

    // Create placeholder footage with path
    static std::shared_ptr<PyFootage> PlaceholderWithPath(
        const std::string& path,
        int width,
        int height,
        float duration);

private:
    AEGP_FootageH m_footageH;  // Source handle (before project)
    AEGP_ItemH m_itemH;        // Item handle (after project)
    bool m_owned;              // Ownership of footageH

    // Internal dispose helper for footageH
    void DisposeFootage();

    // Helper to get main footage handle from item
    AEGP_FootageH GetMainFootageFromItem() const;
};

// =============================================================
// Module init function declaration
// =============================================================
void init_footage(py::module_& m);

} // namespace PyAE
