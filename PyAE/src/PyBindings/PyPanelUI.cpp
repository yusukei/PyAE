// PyPanelUI.cpp
// PyAE - Python for After Effects
// PySide6パネル統合のPythonバインディング

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PySideLoader.h"
#include "Logger.h"

#ifdef _WIN32
#include "PanelUI_Win.h"
#include "PySidePanelUI_Win.h"
#endif

namespace py = pybind11;

namespace PyAE {

/**
 * @brief PySide6 QWidgetのC++ポインタを取得
 *
 * PySide6/Shibokenの内部構造を利用して、
 * Python QWidgetオブジェクトからC++ポインタを抽出する。
 */
void* GetQWidgetPointer(py::object pyWidget) {
    // PySide6のShibokenラッパーからC++ポインタを取得する方法:
    // 1. shiboken6.getCppPointer() を使用
    // 2. または内部の __int__ を使用

    try {
        // 方法1: shiboken6.getCppPointer()
        py::module_ shiboken = py::module_::import("shiboken6");
        py::object getCppPointer = shiboken.attr("getCppPointer");
        py::tuple result = getCppPointer(pyWidget);

        if (py::len(result) > 0) {
            // タプルの最初の要素がポインタ
            uintptr_t ptr = result[0].cast<uintptr_t>();
            return reinterpret_cast<void*>(ptr);
        }
    } catch (const py::error_already_set& e) {
        PYAE_LOG_WARNING("PyPanelUI",
            std::string("shiboken6.getCppPointer failed: ") + e.what());
    }

    try {
        // 方法2: wrapInstance/unwrapInstanceパターン（PySide2互換）
        // __int__でメモリアドレスを取得
        if (py::hasattr(pyWidget, "__int__")) {
            uintptr_t ptr = pyWidget.attr("__int__")().cast<uintptr_t>();
            return reinterpret_cast<void*>(ptr);
        }
    } catch (const py::error_already_set& e) {
        PYAE_LOG_WARNING("PyPanelUI",
            std::string("__int__ method failed: ") + e.what());
    }

    PYAE_LOG_ERROR("PyPanelUI", "Failed to extract C++ pointer from PySide6 QWidget");
    return nullptr;
}

/**
 * @brief PySide6パネルを登録
 */
bool RegisterPySidePanel(const std::string& panelName, py::object pyWidget) {
    PYAE_LOG_INFO("PyPanelUI", "Registering PySide panel: " + panelName);

    // PySideLoaderからプラグインを取得
    auto& loader = PySideLoader::Instance();

    if (!loader.IsLoaded()) {
        // プラグインがまだロードされていない場合、ロードを試みる
        if (!loader.LoadPlugin()) {
            PYAE_LOG_ERROR("PyPanelUI",
                "PyAEPySide.dll not available. PySide6 panels require Qt6 support.");
            throw std::runtime_error(
                "PySide6 panel support not available. "
                "Make sure PyAEPySide.dll and Qt6 are installed.");
        }
    }

    auto* plugin = loader.GetPlugin();
    if (!plugin || !plugin->IsInitialized()) {
        PYAE_LOG_ERROR("PyPanelUI", "PySide plugin not initialized");
        throw std::runtime_error("PySide plugin not initialized");
    }

    // PySide6 QWidgetからC++ポインタを取得
    void* widgetPtr = GetQWidgetPointer(pyWidget);
    if (!widgetPtr) {
        throw std::runtime_error(
            "Failed to get C++ pointer from PySide6 QWidget. "
            "Make sure you're passing a valid QWidget instance.");
    }

    // プラグインに登録
    bool result = plugin->RegisterPySidePanel(panelName, widgetPtr);

    if (result) {
        PYAE_LOG_INFO("PyPanelUI", "PySide panel registered: " + panelName);
    } else {
        PYAE_LOG_ERROR("PyPanelUI", "Failed to register panel: " + panelName);
    }

    return result;
}

/**
 * @brief PySide6パネルの登録を解除
 */
void UnregisterPySidePanel(const std::string& panelName) {
    PYAE_LOG_INFO("PyPanelUI", "Unregistering PySide panel: " + panelName);

    auto& loader = PySideLoader::Instance();
    if (!loader.IsLoaded()) {
        return;
    }

    auto* plugin = loader.GetPlugin();
    if (plugin) {
        plugin->UnregisterPySidePanel(panelName);
    }
}

/**
 * @brief パネルが登録されているか確認
 */
bool IsPanelRegistered(const std::string& panelName) {
    auto& loader = PySideLoader::Instance();
    if (!loader.IsLoaded()) {
        return false;
    }

    auto* plugin = loader.GetPlugin();
    if (!plugin) {
        return false;
    }

    return plugin->IsPanelRegistered(panelName);
}

/**
 * @brief PySide6サポートが利用可能か確認
 */
bool IsPySideAvailable() {
    auto& loader = PySideLoader::Instance();

    // まだロードされていない場合、ロードを試みる
    if (!loader.IsLoaded()) {
        loader.LoadPlugin();
    }

    return loader.IsLoaded();
}

/**
 * @brief PySide用AEパネルのHWNDを取得（整数として返す）
 *
 * PyAE Panelが開いている場合はそのHWNDを返します。
 */
uintptr_t GetPySidePanelHWND() {
#ifdef _WIN32
    PYAE_LOG_DEBUG("PyPanelUI", "GetPySidePanelHWND called");
    auto* panel = PySidePanelUI_Win::GetActivePanel();
    if (panel) {
        HWND hwnd = panel->GetContainerHWND();
        PYAE_LOG_INFO("PyPanelUI", "PySide panel found, HWND: " + std::to_string(reinterpret_cast<uintptr_t>(hwnd)));
        return reinterpret_cast<uintptr_t>(hwnd);
    } else {
        PYAE_LOG_WARNING("PyPanelUI", "No active PySide panel - use Window > PyAE Panel to open it");
    }
#endif
    return 0;
}

/**
 * @brief PyAE Console パネルのHWNDを取得（整数として返す）
 *
 * 後方互換性のため残しています。
 */
uintptr_t GetAEPanelHWND() {
#ifdef _WIN32
    PYAE_LOG_DEBUG("PyPanelUI", "GetAEPanelHWND called (deprecated, use get_pyside_panel_hwnd)");
    // まずPySide用パネルを確認
    auto* pysidePanel = PySidePanelUI_Win::GetActivePanel();
    if (pysidePanel) {
        HWND hwnd = pysidePanel->GetContainerHWND();
        PYAE_LOG_INFO("PyPanelUI", "PySide panel found, HWND: " + std::to_string(reinterpret_cast<uintptr_t>(hwnd)));
        return reinterpret_cast<uintptr_t>(hwnd);
    }

    // フォールバック: Consoleパネル
    auto* consolePanel = PanelUI_Win::GetActivePanel();
    if (consolePanel) {
        HWND hwnd = consolePanel->GetContainerHWND();
        PYAE_LOG_INFO("PyPanelUI", "Console panel found, HWND: " + std::to_string(reinterpret_cast<uintptr_t>(hwnd)));
        return reinterpret_cast<uintptr_t>(hwnd);
    }

    PYAE_LOG_WARNING("PyPanelUI", "No active panel - open Window > PyAE Panel");
#endif
    return 0;
}

/**
 * @brief 登録済みパネル名のリストを取得
 */
std::vector<std::string> GetRegisteredPanelNames() {
#ifdef _WIN32
    std::vector<std::string> names;
    // PySidePanelUI_Winのレジストリから取得（staticメソッドを追加する必要あり）
    // 今はPySidePanelHandlerから取得
    // 簡易実装: 空リストを返す（後で実装）
    return names;
#else
    return {};
#endif
}

/**
 * @brief 指定したパネルにPySide6ウィジェットを埋め込む
 *
 * @param panelMatchName パネルのmatch_name（panels_config.pyで定義）
 * @param pyWidget 埋め込むQWidgetインスタンス
 * @return 成功時true
 */
bool SetWidgetToPanel(const std::string& panelMatchName, py::object pyWidget) {
#ifdef _WIN32
    PYAE_LOG_INFO("PyPanelUI", "SetWidgetToPanel called for: " + panelMatchName);

    // 指定されたパネルを取得
    auto* panel = PySidePanelUI_Win::GetPanel(panelMatchName);
    if (!panel) {
        PYAE_LOG_ERROR("PyPanelUI",
            "Panel not found: " + panelMatchName + ". Make sure the panel is open.");
        return false;
    }

    HWND hwnd = panel->GetContainerHWND();
    if (!hwnd) {
        PYAE_LOG_ERROR("PyPanelUI", "Panel HWND is null for: " + panelMatchName);
        return false;
    }
    PYAE_LOG_INFO("PyPanelUI", "Panel HWND: " + std::to_string(reinterpret_cast<uintptr_t>(hwnd)));

    try {
        // FramelessWindowHint を設定
        py::module_ qtCore = py::module_::import("PySide6.QtCore");
        py::object Qt = qtCore.attr("Qt");
        py::object FramelessWindowHint = Qt.attr("FramelessWindowHint");
        pyWidget.attr("setWindowFlag")(FramelessWindowHint);

        // winId()を呼び出してネイティブウィンドウを確実に作成
        py::object winIdObj = pyWidget.attr("winId")();
        uintptr_t widgetHwndVal = winIdObj.cast<uintptr_t>();
        HWND widgetHwnd = reinterpret_cast<HWND>(widgetHwndVal);

        PYAE_LOG_INFO("PyPanelUI", "Widget HWND: " + std::to_string(widgetHwndVal));

        if (!widgetHwnd) {
            PYAE_LOG_ERROR("PyPanelUI", "Failed to get widget HWND");
            return false;
        }

        // パネルのクライアント領域サイズを取得
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        // SetParent
        SetParent(widgetHwnd, hwnd);

        // ウィンドウスタイルを変更
        LONG style = GetWindowLong(widgetHwnd, GWL_STYLE);
        style &= ~(WS_POPUP | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME |
                   WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        style |= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        SetWindowLong(widgetHwnd, GWL_STYLE, style);

        // 拡張スタイルもクリア
        LONG exStyle = GetWindowLong(widgetHwnd, GWL_EXSTYLE);
        exStyle &= ~(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME |
                     WS_EX_TOOLWINDOW | WS_EX_APPWINDOW);
        SetWindowLong(widgetHwnd, GWL_EXSTYLE, exStyle);

        // 位置とサイズを設定
        SetWindowPos(widgetHwnd, HWND_TOP, 0, 0, width, height,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOACTIVATE);

        // Qtジオメトリを設定
        pyWidget.attr("move")(0, 0);
        pyWidget.attr("resize")(width, height);
        pyWidget.attr("show")();

        // MoveWindowで最終的な位置を設定
        MoveWindow(widgetHwnd, 0, 0, width, height, TRUE);

        // パネルに登録
        panel->SetEmbeddedWidget(widgetHwnd);

        // 再描画
        pyWidget.attr("update")();
        InvalidateRect(hwnd, nullptr, TRUE);
        UpdateWindow(hwnd);

        PYAE_LOG_INFO("PyPanelUI", "Widget set to panel successfully: " + panelMatchName);
        return true;

    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("PyPanelUI", std::string("Python error: ") + e.what());
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PyPanelUI", std::string("Exception: ") + e.what());
    }
#endif
    return false;
}

/**
 * @brief PySide6ウィジェットをPyAE Panelに埋め込む
 *
 * この関数はPySide6ウィジェットをPyAE Panelの子ウィンドウとして設定します。
 * PyAE Console（スクリプトエディタ）には影響しません。
 */
bool EmbedWidgetInAEPanel(py::object pyWidget) {
#ifdef _WIN32
    PYAE_LOG_INFO("PyPanelUI", "EmbedWidgetInAEPanel called");

    // PySide専用パネルを取得
    auto* panel = PySidePanelUI_Win::GetActivePanel();
    if (!panel) {
        PYAE_LOG_ERROR("PyPanelUI",
            "No active PyAE Panel found. Please open Window > PyAE Panel first.");
        return false;
    }
    PYAE_LOG_INFO("PyPanelUI", "PySide panel found at: " +
        std::to_string(reinterpret_cast<uintptr_t>(panel)));

    HWND hwnd = panel->GetContainerHWND();
    if (!hwnd) {
        PYAE_LOG_ERROR("PyPanelUI", "Panel HWND is null");
        return false;
    }
    PYAE_LOG_INFO("PyPanelUI", "Container HWND: " + std::to_string(reinterpret_cast<uintptr_t>(hwnd)));

    try {
        // まずwinId()を呼び出してネイティブウィンドウを確実に作成
        PYAE_LOG_INFO("PyPanelUI", "Creating native window handle...");
        py::object winIdObj = pyWidget.attr("winId")();
        uintptr_t widgetHwndVal = winIdObj.cast<uintptr_t>();
        HWND widgetHwnd = reinterpret_cast<HWND>(widgetHwndVal);

        PYAE_LOG_INFO("PyPanelUI", "Widget HWND: " + std::to_string(widgetHwndVal));

        if (!widgetHwnd) {
            PYAE_LOG_ERROR("PyPanelUI", "Failed to get widget HWND - winId() returned 0");
            return false;
        }

        // パネルのクライアント領域サイズを取得
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        PYAE_LOG_INFO("PyPanelUI", "Panel client rect: " +
            std::to_string(width) + "x" + std::to_string(height));

        // ======== 検証用ログ: SetParent前のウィジェット位置 ========
        RECT widgetRectBefore;
        GetWindowRect(widgetHwnd, &widgetRectBefore);
        PYAE_LOG_INFO("PyPanelUI", "[BEFORE SetParent] Widget WindowRect: (" +
            std::to_string(widgetRectBefore.left) + ", " +
            std::to_string(widgetRectBefore.top) + ") - (" +
            std::to_string(widgetRectBefore.right) + ", " +
            std::to_string(widgetRectBefore.bottom) + ")");

        // Qt側のgeometry（SetParent前）
        try {
            py::object geom = pyWidget.attr("geometry")();
            int qx = geom.attr("x")().cast<int>();
            int qy = geom.attr("y")().cast<int>();
            int qw = geom.attr("width")().cast<int>();
            int qh = geom.attr("height")().cast<int>();
            PYAE_LOG_INFO("PyPanelUI", "[BEFORE SetParent] Qt geometry(): (" +
                std::to_string(qx) + ", " + std::to_string(qy) + ", " +
                std::to_string(qw) + ", " + std::to_string(qh) + ")");

            py::object frameGeom = pyWidget.attr("frameGeometry")();
            int fx = frameGeom.attr("x")().cast<int>();
            int fy = frameGeom.attr("y")().cast<int>();
            int fw = frameGeom.attr("width")().cast<int>();
            int fh = frameGeom.attr("height")().cast<int>();
            PYAE_LOG_INFO("PyPanelUI", "[BEFORE SetParent] Qt frameGeometry(): (" +
                std::to_string(fx) + ", " + std::to_string(fy) + ", " +
                std::to_string(fw) + ", " + std::to_string(fh) + ")");
        } catch (...) {
            PYAE_LOG_DEBUG("PyPanelUI", "Failed to get Qt geometry before SetParent");
        }

        // ======== Step 1: SetParent を最初に呼ぶ ========
        // Qt が座標計算を正しく行えるようにするため
        HWND oldParent = SetParent(widgetHwnd, hwnd);
        PYAE_LOG_INFO("PyPanelUI", "SetParent result (old parent): " +
            std::to_string(reinterpret_cast<uintptr_t>(oldParent)));

        // ======== 検証用ログ: SetParent直後のウィジェット位置 ========
        RECT widgetRectAfterParent;
        GetWindowRect(widgetHwnd, &widgetRectAfterParent);
        PYAE_LOG_INFO("PyPanelUI", "[AFTER SetParent] Widget WindowRect: (" +
            std::to_string(widgetRectAfterParent.left) + ", " +
            std::to_string(widgetRectAfterParent.top) + ") - (" +
            std::to_string(widgetRectAfterParent.right) + ", " +
            std::to_string(widgetRectAfterParent.bottom) + ")");

        // ======== Step 2: ウィンドウスタイルを変更 ========
        LONG style = GetWindowLong(widgetHwnd, GWL_STYLE);
        PYAE_LOG_DEBUG("PyPanelUI", "Original window style: " + std::to_string(style));
        // WS_POPUP, WS_OVERLAPPED などを削除し、WS_CHILD を設定
        style &= ~(WS_POPUP | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME |
                   WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        style |= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        SetWindowLong(widgetHwnd, GWL_STYLE, style);
        PYAE_LOG_DEBUG("PyPanelUI", "New window style: " + std::to_string(style));

        // 拡張スタイルもクリア
        LONG exStyle = GetWindowLong(widgetHwnd, GWL_EXSTYLE);
        exStyle &= ~(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME |
                     WS_EX_TOOLWINDOW | WS_EX_APPWINDOW);
        SetWindowLong(widgetHwnd, GWL_EXSTYLE, exStyle);

        // ======== Step 3: Win32 で位置とサイズを設定 ========
        // SWP_FRAMECHANGED でスタイル変更を反映
        SetWindowPos(widgetHwnd, HWND_TOP, 0, 0, width, height,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOACTIVATE);

        // ======== Step 4: Qt に通知 ========
        // Qt の内部状態を更新するために windowHandle() を呼び出す
        PYAE_LOG_INFO("PyPanelUI", "Notifying Qt about reparenting...");
        try {
            // QWidget.windowHandle() を取得
            py::object windowHandle = pyWidget.attr("windowHandle")();
            if (!windowHandle.is_none()) {
                // QWindow が存在する場合、位置を (0,0) に設定
                windowHandle.attr("setPosition")(0, 0);
            }
        } catch (...) {
            // windowHandle() が失敗しても続行
            PYAE_LOG_DEBUG("PyPanelUI", "windowHandle() not available, skipping");
        }

        // ======== Step 5: Qt ウィジェットのジオメトリを設定 ========
        PYAE_LOG_INFO("PyPanelUI", "Setting Qt geometry...");
        pyWidget.attr("move")(0, 0);
        pyWidget.attr("resize")(width, height);
        pyWidget.attr("show")();

        // ======== Step 6: Win32 で最終的な位置を強制 ========
        // Qt が内部で座標を変更する可能性があるため、最後に MoveWindow で確定
        MoveWindow(widgetHwnd, 0, 0, width, height, TRUE);

        // ======== Step 7: パネルに登録 ========
        panel->SetEmbeddedWidget(widgetHwnd);

        // 再描画を強制
        pyWidget.attr("update")();
        InvalidateRect(hwnd, nullptr, TRUE);
        UpdateWindow(hwnd);

        // ======== 検証用ログ: 最終的な位置 ========
        RECT widgetRectFinal;
        GetWindowRect(widgetHwnd, &widgetRectFinal);
        PYAE_LOG_INFO("PyPanelUI", "[FINAL] Widget WindowRect (screen): (" +
            std::to_string(widgetRectFinal.left) + ", " +
            std::to_string(widgetRectFinal.top) + ") - (" +
            std::to_string(widgetRectFinal.right) + ", " +
            std::to_string(widgetRectFinal.bottom) + ")");

        // 親ウィンドウのスクリーン座標
        RECT parentRectScreen;
        GetWindowRect(hwnd, &parentRectScreen);
        PYAE_LOG_INFO("PyPanelUI", "[FINAL] Parent WindowRect (screen): (" +
            std::to_string(parentRectScreen.left) + ", " +
            std::to_string(parentRectScreen.top) + ") - (" +
            std::to_string(parentRectScreen.right) + ", " +
            std::to_string(parentRectScreen.bottom) + ")");

        // 子ウィンドウの位置（親に対する相対座標）
        POINT childPos = { widgetRectFinal.left, widgetRectFinal.top };
        ScreenToClient(hwnd, &childPos);
        PYAE_LOG_INFO("PyPanelUI", "[FINAL] Widget position relative to parent: (" +
            std::to_string(childPos.x) + ", " + std::to_string(childPos.y) + ")");

        // Qt側の最終状態
        try {
            py::object geom = pyWidget.attr("geometry")();
            int qx = geom.attr("x")().cast<int>();
            int qy = geom.attr("y")().cast<int>();
            int qw = geom.attr("width")().cast<int>();
            int qh = geom.attr("height")().cast<int>();
            PYAE_LOG_INFO("PyPanelUI", "[FINAL] Qt geometry(): (" +
                std::to_string(qx) + ", " + std::to_string(qy) + ", " +
                std::to_string(qw) + ", " + std::to_string(qh) + ")");

            py::object frameGeom = pyWidget.attr("frameGeometry")();
            int fx = frameGeom.attr("x")().cast<int>();
            int fy = frameGeom.attr("y")().cast<int>();
            int fw = frameGeom.attr("width")().cast<int>();
            int fh = frameGeom.attr("height")().cast<int>();
            PYAE_LOG_INFO("PyPanelUI", "[FINAL] Qt frameGeometry(): (" +
                std::to_string(fx) + ", " + std::to_string(fy) + ", " +
                std::to_string(fw) + ", " + std::to_string(fh) + ")");

            // mapToGlobal
            py::module_ qtCore = py::module_::import("PySide6.QtCore");
            py::object QPoint = qtCore.attr("QPoint");
            py::object origin = QPoint(0, 0);
            py::object globalPos = pyWidget.attr("mapToGlobal")(origin);
            int gx = globalPos.attr("x")().cast<int>();
            int gy = globalPos.attr("y")().cast<int>();
            PYAE_LOG_INFO("PyPanelUI", "[FINAL] Qt mapToGlobal(0,0): (" +
                std::to_string(gx) + ", " + std::to_string(gy) + ")");
        } catch (const std::exception& e) {
            PYAE_LOG_DEBUG("PyPanelUI", std::string("Failed to get Qt geometry: ") + e.what());
        }

        PYAE_LOG_INFO("PyPanelUI", "Widget embedded in PyAE Panel successfully");
        return true;

    } catch (const py::error_already_set& e) {
        PYAE_LOG_ERROR("PyPanelUI",
            std::string("Failed to embed widget: ") + e.what());
    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("PyPanelUI",
            std::string("C++ exception during embedding: ") + e.what());
    }
#endif
    return false;
}

/**
 * @brief Pythonバインディング初期化
 */
void init_panel_ui(py::module_& m) {
    py::module_ panel_ui = m.def_submodule("panel_ui",
        "PySide6 Panel Integration for After Effects");

    // PySideサポート確認
    panel_ui.def("is_pyside_available",
        &IsPySideAvailable,
        R"pbdoc(
        Check if PySide6 panel support is available.

        Returns:
            bool: True if PySide6 support is available (PyAEPySide.dll loaded)

        Note:
            PySide6 support requires:
            - PyAEPySide.dll in the plugin directory
            - Qt6 DLLs (Qt6Core.dll, Qt6Widgets.dll, Qt6Gui.dll)
            - PySide6 Python package installed
        )pbdoc");

    // パネル登録
    panel_ui.def("register_pyside_panel",
        &RegisterPySidePanel,
        py::arg("panel_name"),
        py::arg("widget"),
        R"pbdoc(
        Register a PySide6 QWidget as an After Effects panel.

        Parameters:
            panel_name (str): Unique name for the panel
            widget (QWidget): PySide6 QWidget instance

        Returns:
            bool: True if registration succeeded

        Raises:
            RuntimeError: If PySide6 support is not available or registration fails

        Example:
            >>> from PySide6.QtWidgets import QWidget, QPushButton, QVBoxLayout
            >>> import pyae
            >>>
            >>> class MyPanel(QWidget):
            ...     def __init__(self):
            ...         super().__init__()
            ...         layout = QVBoxLayout()
            ...         layout.addWidget(QPushButton("Click Me"))
            ...         self.setLayout(layout)
            >>>
            >>> panel = MyPanel()
            >>> pyae.panel_ui.register_pyside_panel("My Panel", panel)
        )pbdoc");

    // パネル登録解除
    panel_ui.def("unregister_panel",
        &UnregisterPySidePanel,
        py::arg("panel_name"),
        R"pbdoc(
        Unregister a PySide6 panel.

        Parameters:
            panel_name (str): Name of the panel to unregister
        )pbdoc");

    // パネル登録確認
    panel_ui.def("is_panel_registered",
        &IsPanelRegistered,
        py::arg("panel_name"),
        R"pbdoc(
        Check if a panel is registered.

        Parameters:
            panel_name (str): Name of the panel to check

        Returns:
            bool: True if the panel is registered
        )pbdoc");

    // PySideパネルHWND取得（推奨）
    panel_ui.def("get_pyside_panel_hwnd",
        &GetPySidePanelHWND,
        R"pbdoc(
        Get the HWND of the PyAE Panel (for PySide6 embedding).

        Returns:
            int: HWND as an integer (0 if no panel is active)

        Note:
            Open Window > PyAE Panel before calling this function.
            This panel is separate from PyAE Console (script editor).
        )pbdoc");

    // AEパネルHWND取得（後方互換性）
    panel_ui.def("get_ae_panel_hwnd",
        &GetAEPanelHWND,
        R"pbdoc(
        Get the HWND of an active PyAE panel (deprecated, use get_pyside_panel_hwnd).

        Returns:
            int: HWND as an integer (0 if no panel is active)

        Note:
            This function first checks PyAE Panel, then falls back to PyAE Console.
            For new code, use get_pyside_panel_hwnd() instead.
        )pbdoc");

    // AEパネルにウィジェットを埋め込む（アクティブパネルに）
    panel_ui.def("embed_widget",
        &EmbedWidgetInAEPanel,
        py::arg("widget"),
        R"pbdoc(
        Embed a PySide6 QWidget into the active PyAE panel.

        Parameters:
            widget (QWidget): PySide6 QWidget instance to embed

        Returns:
            bool: True if embedding succeeded

        Note:
            This embeds into the currently active panel.
            Use set_widget() to specify the panel by name.
        )pbdoc");

    // 指定したパネルにウィジェットを埋め込む
    panel_ui.def("set_widget",
        &SetWidgetToPanel,
        py::arg("panel_name"),
        py::arg("widget"),
        R"pbdoc(
        Set a PySide6 QWidget to a specific panel by name.

        Parameters:
            panel_name (str): The match_name of the panel (defined in panels_config.py)
            widget (QWidget): PySide6 QWidget instance to embed

        Returns:
            bool: True if embedding succeeded

        Example:
            >>> from PySide6.QtWidgets import QPushButton
            >>> import ae
            >>>
            >>> # Create widget
            >>> widget = QPushButton("Hello Panel!")
            >>>
            >>> # Embed into specific panel (must be open)
            >>> ae.panel_ui.set_widget("PyAE_SampleTool", widget)

        Note:
            The panel must be open (visible) before calling this function.
            The panel_name should match the match_name in panels_config.py.
        )pbdoc");
}

} // namespace PyAE
