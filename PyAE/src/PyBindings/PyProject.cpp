// PyProject.cpp
// PyAE - Python for After Effects
// プロジェクトAPIバインディング (Phase 2 拡充版)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PluginState.h"
#include "IdleHandler.h"
#include "ErrorHandling.h"
#include "ScopedHandles.h"
#include "StringUtils.h"
#include "AETypeUtils.h"
#include "Logger.h"
#include "PyRefTypes.h"

namespace py = pybind11;

namespace PyAE {

// プロジェクトラッパークラス
class PyProject {
public:
    PyProject() : m_projectH(nullptr) {
        // 現在のプロジェクトを取得
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (suites.projSuite) {
            suites.projSuite->AEGP_GetProjectByIndex(0, &m_projectH);
        }
    }

    bool IsValid() const { return m_projectH != nullptr; }

    // ==========================================
    // 基本情報
    // ==========================================
    std::string GetName() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            return "";
        }

        A_char name[AEGP_MAX_PROJ_NAME_SIZE] = {0};
        A_Err err = suites.projSuite->AEGP_GetProjectName(m_projectH, name);
        if (err != A_Err_NONE) {
            return "";
        }

        // A_char はローカルエンコーディング (Shift-JIS等) なので UTF-8 に変換
        return StringUtils::LocalToUtf8(name);
    }

    std::string GetPath() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            return "";
        }

        AEGP_MemHandle pathH;
        A_Err err = suites.projSuite->AEGP_GetProjectPath(m_projectH, &pathH);
        if (err != A_Err_NONE || !pathH) {
            return "";
        }

        ScopedMemHandle scopedPath(state.GetPluginID(), suites.memorySuite, pathH);
        ScopedMemLock lock(suites.memorySuite, pathH);
        A_UTF16Char* pathPtr = lock.As<A_UTF16Char>();
        if (!pathPtr) {
            return "";
        }

        std::string result = StringUtils::Utf16ToUtf8(pathPtr);

        return result;
    }

    int GetBitDepth() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            return 8;
        }

        AEGP_ProjBitDepth bitDepth;
        A_Err err = suites.projSuite->AEGP_GetProjectBitDepth(m_projectH, &bitDepth);
        if (err != A_Err_NONE) {
            return 8;
        }

        switch (bitDepth) {
            case AEGP_ProjBitDepth_8:  return 8;
            case AEGP_ProjBitDepth_16: return 16;
            case AEGP_ProjBitDepth_32: return 32;
            default:                   return 8;
        }
    }

    void SetBitDepth(int depth) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            throw std::runtime_error("Invalid project");
        }

        AEGP_ProjBitDepth bitDepth;
        switch (depth) {
            case 8:  bitDepth = AEGP_ProjBitDepth_8; break;
            case 16: bitDepth = AEGP_ProjBitDepth_16; break;
            case 32: bitDepth = AEGP_ProjBitDepth_32; break;
            default:
                throw std::runtime_error("Invalid bit depth (must be 8, 16, or 32)");
        }

        A_Err err = suites.projSuite->AEGP_SetProjectBitDepth(m_projectH, bitDepth);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set bit depth");
        }
    }

    // ==========================================
    // アイテム操作
    // ==========================================
    int GetNumItems() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !suites.itemSuite || !m_projectH) {
            return 0;
        }

        int count = 0;
        AEGP_ItemH itemH = nullptr;
        A_Err err = suites.itemSuite->AEGP_GetFirstProjItem(m_projectH, &itemH);
        while (err == A_Err_NONE && itemH != nullptr) {
            count++;
            AEGP_ItemH nextItemH = nullptr;
            err = suites.itemSuite->AEGP_GetNextProjItem(m_projectH, itemH, &nextItemH);
            itemH = nextItemH;
        }

        return count;
    }

    PyItemRef GetRootFolder() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            return PyItemRef();
        }

        AEGP_ItemH rootFolder;
        A_Err err = suites.projSuite->AEGP_GetProjectRootFolder(m_projectH, &rootFolder);
        if (err != A_Err_NONE) {
            return PyItemRef();
        }

        return PyItemRef(rootFolder);
    }

    PyItemRef GetActiveItem() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.itemSuite) {
            return PyItemRef();
        }

        AEGP_ItemH itemH;
        A_Err err = suites.itemSuite->AEGP_GetActiveItem(&itemH);
        if (err != A_Err_NONE || !itemH) {
            return PyItemRef();
        }

        return PyItemRef(itemH);
    }

    std::vector<PyItemRef> GetItems() const {
        std::vector<PyItemRef> items;

        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !suites.itemSuite || !m_projectH) {
            return items;
        }

        AEGP_ItemH itemH = nullptr;
        A_Err err = suites.itemSuite->AEGP_GetFirstProjItem(m_projectH, &itemH);
        while (err == A_Err_NONE && itemH != nullptr) {
            items.emplace_back(itemH);
            AEGP_ItemH nextItemH = nullptr;
            err = suites.itemSuite->AEGP_GetNextProjItem(m_projectH, itemH, &nextItemH);
            itemH = nextItemH;
        }

        return items;
    }

    PyItemRef GetItemByName(const std::string& name) const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !suites.itemSuite || !m_projectH) {
            throw std::runtime_error("Invalid project");
        }

        AEGP_ItemH itemH = nullptr;
        A_Err err = suites.itemSuite->AEGP_GetFirstProjItem(m_projectH, &itemH);
        while (err == A_Err_NONE && itemH != nullptr) {
            AEGP_MemHandle nameH;
            err = suites.itemSuite->AEGP_GetItemName(state.GetPluginID(), itemH, &nameH);
            if (err == A_Err_NONE && nameH) {
                ScopedMemHandle scopedName(state.GetPluginID(), suites.memorySuite, nameH);

                ScopedMemLock lock(suites.memorySuite, nameH);
                A_UTF16Char* namePtr = lock.As<A_UTF16Char>();
                if (namePtr) {
                    std::string itemName = StringUtils::Utf16ToUtf8(namePtr);

                    if (itemName == name) {
                        return PyItemRef(itemH);
                    }
                }
            }

            AEGP_ItemH nextItemH = nullptr;
            err = suites.itemSuite->AEGP_GetNextProjItem(m_projectH, itemH, &nextItemH);
            itemH = nextItemH;
        }

        // Item not found: return invalid ref (Python None)
        return PyItemRef();
    }

    // ==========================================
    // フォルダ作成
    // ==========================================
    PyItemRef CreateFolder(const std::string& name, py::object parentFolder = py::none()) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !suites.itemSuite || !m_projectH) {
            throw std::runtime_error("Invalid project");
        }

        // 親フォルダを決定
        AEGP_ItemH parentH = nullptr;
        if (parentFolder.is_none()) {
            // ルートフォルダを使用
            A_Err err = suites.projSuite->AEGP_GetProjectRootFolder(m_projectH, &parentH);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to get root folder");
            }
        } else {
            // PyItemRef, PyFolder, その他のアイテムオブジェクトに対応
            if (py::hasattr(parentFolder, "_get_handle_ptr")) {
                uintptr_t ptr = parentFolder.attr("_get_handle_ptr")().cast<uintptr_t>();
                parentH = reinterpret_cast<AEGP_ItemH>(ptr);
            } else {
                try {
                    PyItemRef parentRef = parentFolder.cast<PyItemRef>();
                    parentH = parentRef.GetHandle();
                } catch (const py::cast_error&) {
                    throw std::runtime_error("Invalid parent folder type. Expected Folder or Item object.");
                }
            }
        }

        // フォルダを作成
        std::wstring wname = StringUtils::Utf8ToWide(name);

        // デバッグログ
        char logMsg[512];
        snprintf(logMsg, sizeof(logMsg), "CreateFolder: name='%s', parentH=%p", name.c_str(), parentH);
        Logger::Instance().Log(LogLevel::Info, "PyProject", logMsg);

        AEGP_ItemH newFolderH;
        A_Err err = suites.itemSuite->AEGP_CreateNewFolder(
            reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
            parentH,
            &newFolderH
        );

        if (err != A_Err_NONE || !newFolderH) {
            snprintf(logMsg, sizeof(logMsg), "CreateFolder failed: name='%s', error=%d", name.c_str(), static_cast<int>(err));
            Logger::Instance().Log(LogLevel::Error, "PyProject", logMsg);
            throw std::runtime_error("Failed to create folder");
        }

        // 作成後の親フォルダを確認
        AEGP_ItemH actualParentH = nullptr;
        suites.itemSuite->AEGP_GetItemParentFolder(newFolderH, &actualParentH);
        snprintf(logMsg, sizeof(logMsg), "CreateFolder result: name='%s', newFolderH=%p, actualParentH=%p",
                 name.c_str(), newFolderH, actualParentH);
        Logger::Instance().Log(LogLevel::Info, "PyProject", logMsg);

        return PyItemRef(newFolderH);
    }

    // ==========================================
    // コンポジション作成
    // ==========================================
    PyCompRef CreateComp(const std::string& name, int width, int height,
                         double pixelAspect, double duration, double frameRate,
                         py::object parentFolder = py::none()) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.compSuite || !suites.projSuite || !m_projectH) {
            throw std::runtime_error("Required suites not available");
        }

        // 親フォルダを決定
        AEGP_ItemH parentH = nullptr;
        if (parentFolder.is_none()) {
            A_Err err = suites.projSuite->AEGP_GetProjectRootFolder(m_projectH, &parentH);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to get root folder");
            }
        } else {
            // PyItemRef, PyFolder, その他のアイテムオブジェクトに対応
            if (py::hasattr(parentFolder, "_get_handle_ptr")) {
                uintptr_t ptr = parentFolder.attr("_get_handle_ptr")().cast<uintptr_t>();
                parentH = reinterpret_cast<AEGP_ItemH>(ptr);
            } else {
                try {
                    PyItemRef parentRef = parentFolder.cast<PyItemRef>();
                    parentH = parentRef.GetHandle();
                } catch (const py::cast_error&) {
                    throw std::runtime_error("Invalid parent folder type. Expected Folder or Item object.");
                }
            }
        }

        A_Ratio pixelAspectRatio = AETypeUtils::DoubleToRatio(pixelAspect);
        A_Time durationTime = AETypeUtils::SecondsToTimeWithFps(duration, frameRate);
        A_Ratio frameRateRatio = AETypeUtils::DoubleToRatio(frameRate, 1000);

        std::wstring wname = StringUtils::Utf8ToWide(name);

        // デバッグログ
        Logger::Instance().Log(LogLevel::Info, "PyProject",
            "CreateComp: name='" + name + "', parentH=" +
            (parentH ? std::to_string(reinterpret_cast<uintptr_t>(parentH)) : "nullptr"));

        AEGP_CompH newCompH;
        // AEGP_CreateCompの親フォルダ指定が機能しないため、ルートに作成
        A_Err err = suites.compSuite->AEGP_CreateComp(
            nullptr,  // 一旦ルートに作成
            reinterpret_cast<const A_UTF16Char*>(wname.c_str()),
            static_cast<A_long>(width),
            static_cast<A_long>(height),
            &pixelAspectRatio,
            &durationTime,
            &frameRateRatio,
            &newCompH
        );

        if (err != A_Err_NONE || !newCompH) {
            throw std::runtime_error("Failed to create comp");
        }

        // ItemHを取得
        AEGP_ItemH itemH = nullptr;
        err = suites.compSuite->AEGP_GetItemFromComp(newCompH, &itemH);
        if (err != A_Err_NONE || !itemH) {
            throw std::runtime_error("Failed to get item from comp");
        }

        // 親フォルダが指定されていれば移動
        if (parentH && suites.itemSuite) {
            err = suites.itemSuite->AEGP_SetItemParentFolder(itemH, parentH);
            if (err != A_Err_NONE) {
                Logger::Instance().Log(LogLevel::Warning, "PyProject",
                    "Failed to set parent folder for comp '" + name + "'");
            }
        }

        // 作成後のログ
        Logger::Instance().Log(LogLevel::Info, "PyProject",
            "CreateComp result: name='" + name + "', newCompH=" +
            std::to_string(reinterpret_cast<uintptr_t>(newCompH)) +
            ", itemH=" + std::to_string(reinterpret_cast<uintptr_t>(itemH)) +
            ", moved to parent=" + (parentH ? "yes" : "no"));

        return PyCompRef(newCompH);
    }

    // ==========================================
    // ファイルインポート
    // ==========================================
    PyItemRef ImportFile(const std::string& filePath, py::object parentFolder = py::none(),
                         py::object sequenceOptions = py::none()) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.footageSuite || !suites.projSuite || !m_projectH) {
            throw std::runtime_error("Required suites not available");
        }

        // 親フォルダを決定
        AEGP_ItemH parentH = nullptr;
        if (parentFolder.is_none()) {
            A_Err err = suites.projSuite->AEGP_GetProjectRootFolder(m_projectH, &parentH);
            if (err != A_Err_NONE) {
                throw std::runtime_error("Failed to get root folder");
            }
        } else {
            // PyItemRef, PyFolder, その他のアイテムオブジェクトに対応
            // _get_handle_ptr メソッドを持つオブジェクトからハンドルを取得
            if (py::hasattr(parentFolder, "_get_handle_ptr")) {
                uintptr_t ptr = parentFolder.attr("_get_handle_ptr")().cast<uintptr_t>();
                parentH = reinterpret_cast<AEGP_ItemH>(ptr);
            } else {
                // PyItemRef として直接キャストを試みる
                try {
                    PyItemRef parentRef = parentFolder.cast<PyItemRef>();
                    parentH = parentRef.GetHandle();
                } catch (const py::cast_error&) {
                    throw std::runtime_error("Invalid parent folder type. Expected Folder or Item object.");
                }
            }
        }

        // ファイルパスをUTF-16に変換
        std::wstring wpath = StringUtils::Utf8ToWide(filePath);

        // デバッグログ
        Logger::Instance().Log(LogLevel::Info, "PyProject",
            "ImportFile: path='" + filePath + "', parentH=" +
            (parentH ? std::to_string(reinterpret_cast<uintptr_t>(parentH)) : "nullptr"));

        // シーケンスオプションの設定
        AEGP_FileSequenceImportOptions seqOpts;
        AEGP_FileSequenceImportOptions* seqOptsPtr = nullptr;

        if (!sequenceOptions.is_none()) {
            py::dict seqDict = sequenceOptions.cast<py::dict>();

            // is_sequence が True の場合のみシーケンスとしてインポート
            if (seqDict.contains("is_sequence") && seqDict["is_sequence"].cast<bool>()) {
                seqOpts.all_in_folderB = TRUE;

                // force_alphabetical (デフォルト: FALSE)
                if (seqDict.contains("force_alphabetical")) {
                    seqOpts.force_alphabeticalB = seqDict["force_alphabetical"].cast<bool>() ? TRUE : FALSE;
                } else {
                    seqOpts.force_alphabeticalB = FALSE;
                }

                // start_frame (-1 = AEGP_ANY_FRAME = 自動検出)
                if (seqDict.contains("start_frame")) {
                    seqOpts.start_frameL = seqDict["start_frame"].cast<A_long>();
                } else {
                    seqOpts.start_frameL = AEGP_ANY_FRAME;
                }

                // end_frame (-1 = AEGP_ANY_FRAME = 自動検出)
                if (seqDict.contains("end_frame")) {
                    seqOpts.end_frameL = seqDict["end_frame"].cast<A_long>();
                } else {
                    seqOpts.end_frameL = AEGP_ANY_FRAME;
                }

                seqOptsPtr = &seqOpts;
            }
        }

        // フッテージを作成
        AEGP_FootageH footageH;
        A_Err err = suites.footageSuite->AEGP_NewFootage(
            state.GetPluginID(),
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str()),
            nullptr,  // フッテージレイヤー
            seqOptsPtr,  // シーケンスオプション
            AEGP_InterpretationStyle_NO_DIALOG_GUESS,
            nullptr,  // インタープリットオプション
            &footageH
        );

        if (err != A_Err_NONE || !footageH) {
            // 見つからない場合は無効な参照を返す（Python側でNoneになる）
            return PyItemRef();
        }

        // プロジェクトに追加
        AEGP_ItemH itemH;
        err = suites.footageSuite->AEGP_AddFootageToProject(footageH, parentH, &itemH);
        if (err != A_Err_NONE || !itemH) {
            suites.footageSuite->AEGP_DisposeFootage(footageH);
            throw std::runtime_error("Failed to add footage to project");
        }

        // 作成後のログ
        Logger::Instance().Log(LogLevel::Info, "PyProject",
            "ImportFile result: itemH=" + std::to_string(reinterpret_cast<uintptr_t>(itemH)));

        return PyItemRef(itemH);
    }

    // ==========================================
    // プロジェクト状態
    // ==========================================
    bool IsDirty() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            return false;
        }

        A_Boolean isDirty = false;
        A_Err err = suites.projSuite->AEGP_ProjectIsDirty(m_projectH, &isDirty);
        return (err == A_Err_NONE) && isDirty;
    }

    void SetDirty() {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.advAppSuite) {
            throw std::runtime_error("AdvApp Suite not available");
        }

        A_Err err = suites.advAppSuite->PF_SetProjectDirty();
        if (err != A_Err_NONE) {
            throw std::runtime_error("PF_SetProjectDirty failed: error " + std::to_string(err));
        }
    }

    // ==========================================
    // 時間表示設定
    // ==========================================
    py::dict GetTimeDisplay() const {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            return py::dict();
        }

        AEGP_TimeDisplay3 timeDisplay;
        A_Err err = suites.projSuite->AEGP_GetProjectTimeDisplay(m_projectH, &timeDisplay);
        if (err != A_Err_NONE) {
            return py::dict();
        }

        py::dict result;
        result["display_mode"] = static_cast<int>(timeDisplay.display_mode);
        result["footage_display_mode"] = static_cast<int>(timeDisplay.footage_display_mode);
        result["display_dropframe"] = static_cast<bool>(timeDisplay.display_dropframeB);
        result["use_feet_frames"] = static_cast<bool>(timeDisplay.use_feet_framesB);
        result["timebase"] = static_cast<int>(timeDisplay.timebaseC);
        result["frames_per_foot"] = static_cast<int>(timeDisplay.frames_per_footC);
        result["frames_display_mode"] = static_cast<int>(timeDisplay.frames_display_mode);
        return result;
    }

    void SetTimeDisplay(py::dict timeDisplay) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            throw std::runtime_error("Invalid project");
        }

        AEGP_TimeDisplay3 td;
        td.display_mode = static_cast<AEGP_TimeDisplayMode>(timeDisplay["display_mode"].cast<int>());
        td.footage_display_mode = static_cast<AEGP_SourceTimecodeDisplayMode>(timeDisplay["footage_display_mode"].cast<int>());
        td.display_dropframeB = timeDisplay["display_dropframe"].cast<bool>() ? TRUE : FALSE;
        td.use_feet_framesB = timeDisplay["use_feet_frames"].cast<bool>() ? TRUE : FALSE;
        td.timebaseC = static_cast<A_char>(timeDisplay["timebase"].cast<int>());
        td.frames_per_footC = static_cast<A_char>(timeDisplay["frames_per_foot"].cast<int>());
        td.frames_display_mode = static_cast<AEGP_FramesDisplayMode>(timeDisplay["frames_display_mode"].cast<int>());

        A_Err err = suites.projSuite->AEGP_SetProjectTimeDisplay(m_projectH, &td);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to set time display");
        }
    }

    // ==========================================
    // 保存
    // ==========================================
    bool Save() {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            return false;
        }

        A_Err err = suites.projSuite->AEGP_SaveProjectToPath(m_projectH, nullptr);
        return err == A_Err_NONE;
    }

    bool SaveAs(const std::string& filePath) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite || !m_projectH) {
            return false;
        }

        std::wstring wpath = StringUtils::Utf8ToWide(filePath);

        A_Err err = suites.projSuite->AEGP_SaveProjectToPath(
            m_projectH,
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str())
        );
        return err == A_Err_NONE;
    }

    // ==========================================
    // 静的メソッド
    // ==========================================
    static int GetNumProjects() {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite) {
            return 0;
        }

        A_long numProjs = 0;
        A_Err err = suites.projSuite->AEGP_GetNumProjects(&numProjs);
        return (err == A_Err_NONE) ? static_cast<int>(numProjs) : 0;
    }

    static PyProject GetCurrent() {
        return PyProject();
    }

    static PyProject GetProjectByIndex(int index) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite) {
            throw std::runtime_error("Project Suite not available");
        }

        AEGP_ProjectH projH = nullptr;
        A_Err err = suites.projSuite->AEGP_GetProjectByIndex(index, &projH);
        if (err != A_Err_NONE || !projH) {
            throw std::runtime_error("Failed to get project by index");
        }

        PyProject proj;
        proj.m_projectH = projH;
        return proj;
    }

    static bool IsProjectOpen() {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite) {
            return false;
        }

        AEGP_ProjectH projH;
        A_Err err = suites.projSuite->AEGP_GetProjectByIndex(0, &projH);
        return err == A_Err_NONE && projH != nullptr;
    }

    static PyProject NewProject() {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite) {
            throw std::runtime_error("Project Suite not available");
        }

        AEGP_ProjectH newProjH = nullptr;
        A_Err err = suites.projSuite->AEGP_NewProject(&newProjH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to create new project");
        }

        PyProject proj;
        proj.m_projectH = newProjH;
        return proj;
    }

    static PyProject OpenProject(const std::string& filePath) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();

        if (!suites.projSuite) {
            throw std::runtime_error("Project Suite not available");
        }

        std::wstring wpath = StringUtils::Utf8ToWide(filePath);

        AEGP_ProjectH projH = nullptr;
        A_Err err = suites.projSuite->AEGP_OpenProjectFromPath(
            reinterpret_cast<const A_UTF16Char*>(wpath.c_str()), &projH);
        if (err != A_Err_NONE) {
            throw std::runtime_error("Failed to open project from path");
        }

        PyProject proj;
        proj.m_projectH = projH;
        return proj;
    }

    // Internal method for low-level SDK access
    AEGP_ProjectH GetProjectHandle() const {
        return m_projectH;
    }

private:
    AEGP_ProjectH m_projectH;
};

} // namespace PyAE

void init_project(py::module_& m) {
    // ヘルパー関数: PyItemRefからresolveするラムダ
    // ResolveItemHandle関数はPyItem.cppで定義されている（PyRefTypes.hで宣言）
    auto resolve_item = [](const PyAE::PyItemRef& ref) -> py::object {
        if (!ref.IsValid()) return py::none();
        return PyAE::ResolveItemHandle(ref.GetHandle());
    };

    // Projectクラス
    py::class_<PyAE::PyProject>(m, "Project")
        .def(py::init<>())
        // 基本情報
        .def_property_readonly("valid", &PyAE::PyProject::IsValid,
                              "Check if project is valid")
        .def_property_readonly("name", &PyAE::PyProject::GetName,
                              "Project name")
        .def_property_readonly("path", &PyAE::PyProject::GetPath,
                              "Project file path (empty if not saved)")
        .def_property("bit_depth", &PyAE::PyProject::GetBitDepth, &PyAE::PyProject::SetBitDepth,
                     "Project bit depth (8, 16, or 32)")
        .def_property_readonly("is_dirty", &PyAE::PyProject::IsDirty,
                              "Check if project has been modified since last save")
        .def("set_dirty", &PyAE::PyProject::SetDirty,
             "Mark the project as having unsaved changes")
        .def("get_time_display", &PyAE::PyProject::GetTimeDisplay,
             "Get time display settings")
        .def("set_time_display", &PyAE::PyProject::SetTimeDisplay,
             "Set time display settings (UNDOABLE)",
             py::arg("time_display"))
        // アイテム操作 - 自動resolve
        .def_property_readonly("num_items", &PyAE::PyProject::GetNumItems,
                              "Number of items in project")
        .def_property_readonly("root_folder", [resolve_item](const PyAE::PyProject& self) {
            return resolve_item(self.GetRootFolder());
        }, "Project root folder")
        .def_property_readonly("active_item", [resolve_item](const PyAE::PyProject& self) {
            return resolve_item(self.GetActiveItem());
        }, "Currently active item")
        .def_property_readonly("items", [resolve_item](const PyAE::PyProject& self) {
            auto refs = self.GetItems();
            py::list result;
            for (const auto& ref : refs) {
                result.append(resolve_item(ref));
            }
            return result;
        }, "List of all items in project")
        .def("item_by_name", [resolve_item](const PyAE::PyProject& self, const std::string& name) {
            return resolve_item(self.GetItemByName(name));
        }, "Get item by name", py::arg("name"))
        // 作成 - 自動resolve
        .def("create_folder", [resolve_item](PyAE::PyProject& self, const std::string& name, py::object parentFolder) {
            return resolve_item(self.CreateFolder(name, parentFolder));
        }, "Create a new folder",
           py::arg("name"),
           py::arg("parent_folder") = py::none())
        .def("create_comp", [resolve_item](PyAE::PyProject& self,
                                            const std::string& name, int width, int height,
                                            double pixelAspect, double duration, double frameRate,
                                            py::object parentFolder) -> py::object {
            auto compRef = self.CreateComp(name, width, height, pixelAspect, duration, frameRate, parentFolder);
            if (!compRef.IsValid()) return py::none();

            // CompH から ItemH を取得
            auto& state = PyAE::PluginState::Instance();
            const auto& suites = state.GetSuites();
            if (!suites.compSuite) return py::none();

            AEGP_ItemH itemH;
            A_Err err = suites.compSuite->AEGP_GetItemFromComp(compRef.GetHandle(), &itemH);
            if (err != A_Err_NONE || !itemH) return py::none();

            // ItemRef経由でresolve
            return resolve_item(PyAE::PyItemRef(itemH));
        }, "Create a new composition",
           py::arg("name"),
           py::arg("width"),
           py::arg("height"),
           py::arg("pixel_aspect") = 1.0,
           py::arg("duration") = 10.0,
           py::arg("frame_rate") = 30.0,
           py::arg("parent_folder") = py::none())
        .def("import_file", [resolve_item](PyAE::PyProject& self, const std::string& filePath,
                                            py::object parentFolder, py::object sequenceOptions) {
            return resolve_item(self.ImportFile(filePath, parentFolder, sequenceOptions));
        }, "Import a file into the project. For image sequences, pass sequence_options dict with "
           "is_sequence=True, start_frame, end_frame.",
           py::arg("file_path"),
           py::arg("parent_folder") = py::none(),
           py::arg("sequence_options") = py::none())
        // 保存
        .def("save", &PyAE::PyProject::Save,
             "Save the project")
        .def("save_as", &PyAE::PyProject::SaveAs,
             "Save the project to a new location",
             py::arg("file_path"))
        // Internal methods for low-level SDK access
        .def("_get_handle_ptr", [](const PyAE::PyProject& self) -> uintptr_t {
            return reinterpret_cast<uintptr_t>(self.GetProjectHandle());
        }, "Internal: Get project handle as uintptr_t for low-level SDK access")
        // 静的メソッド
        .def_static("num_projects", &PyAE::PyProject::GetNumProjects,
                   "Get the number of open projects")
        .def_static("get_current", &PyAE::PyProject::GetCurrent,
                   "Get the current project")
        .def_static("get_by_index", &PyAE::PyProject::GetProjectByIndex,
                   "Get project by index",
                   py::arg("index"))
        .def_static("is_open", &PyAE::PyProject::IsProjectOpen,
                   "Check if a project is open")
        .def_static("new_project", &PyAE::PyProject::NewProject,
                   "Create a new project")
        .def_static("open_project", &PyAE::PyProject::OpenProject,
                   "Open a project from file path (WARNING: Will close any open projects)",
                   py::arg("file_path"))

        // Serialization methods
        .def("to_dict", [](PyAE::PyProject& self) -> py::dict {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                return serialize.attr("project_to_dict")(self).cast<py::dict>();
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to serialize project: ") + e.what());
            }
        }, "Export project to dictionary")

        .def_static("from_dict", [](const py::dict& data) -> PyAE::PyProject {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                serialize.attr("project_from_dict")(data);
                return PyAE::PyProject();  // Return current project
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to import project from dict: ") + e.what());
            }
        }, "Import project items from dictionary into current project",
           py::arg("data"))

        .def("update_from_dict", [](PyAE::PyProject& self, const py::dict& data) {
            try {
                py::module_ serialize = py::module_::import("ae_serialize");
                serialize.attr("project_update_from_dict")(self, data);
            } catch (const py::error_already_set& e) {
                throw std::runtime_error(std::string("Failed to update project from dict: ") + e.what());
            }
        }, "Update existing project from dictionary",
           py::arg("data"));
}
