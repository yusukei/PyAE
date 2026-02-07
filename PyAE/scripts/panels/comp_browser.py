"""
PyAE - Composition Browser Panel

プロジェクト内のコンポジション一覧を表示し、選択・操作できるパネル

使い方:
    import ae
    ae.execute_script_file("path/to/comp_browser.py")
"""

try:
    from PySide6.QtWidgets import (
        QWidget, QVBoxLayout, QHBoxLayout,
        QListWidget, QPushButton, QLabel,
        QListWidgetItem, QFrame, QSplitter
    )
    from PySide6.QtCore import Qt
except ImportError:
    print("Error: PySide6 is not installed.")
    print("Install with: pip install PySide6")
    raise

import ae


class CompBrowserPanel(QWidget):
    """コンポジションブラウザパネル"""

    def __init__(self):
        super().__init__()
        self.init_ui()
        self.refresh_compositions()

    def init_ui(self):
        """UI初期化"""
        main_layout = QVBoxLayout()
        main_layout.setContentsMargins(8, 8, 8, 8)
        main_layout.setSpacing(8)

        # ヘッダー
        header = QLabel("Compositions")
        header.setStyleSheet("""
            font-size: 14px;
            font-weight: bold;
            color: #00aaff;
            padding: 4px 0;
        """)
        main_layout.addWidget(header)

        # 水平線
        line = QFrame()
        line.setFrameShape(QFrame.HLine)
        line.setStyleSheet("background-color: #444444;")
        main_layout.addWidget(line)

        # コンポジションリスト
        self.comp_list = QListWidget()
        self.comp_list.setStyleSheet("""
            QListWidget {
                background-color: #2d2d2d;
                border: 1px solid #3d3d3d;
                border-radius: 4px;
                color: white;
            }
            QListWidget::item {
                padding: 8px;
                border-bottom: 1px solid #3d3d3d;
            }
            QListWidget::item:selected {
                background-color: #0078d4;
            }
            QListWidget::item:hover {
                background-color: #3d3d3d;
            }
        """)
        self.comp_list.itemDoubleClicked.connect(self.on_comp_double_clicked)
        self.comp_list.itemSelectionChanged.connect(self.on_selection_changed)
        main_layout.addWidget(self.comp_list)

        # 情報ラベル
        self.info_label = QLabel("")
        self.info_label.setStyleSheet("color: #888888; font-size: 11px;")
        main_layout.addWidget(self.info_label)

        # ボタン行
        button_layout = QHBoxLayout()

        refresh_btn = QPushButton("Refresh")
        refresh_btn.setStyleSheet(self._button_style())
        refresh_btn.clicked.connect(self.refresh_compositions)
        button_layout.addWidget(refresh_btn)

        open_btn = QPushButton("Open")
        open_btn.setStyleSheet(self._button_style())
        open_btn.clicked.connect(self.open_selected_comp)
        button_layout.addWidget(open_btn)

        main_layout.addLayout(button_layout)

        # ステータス
        self.status_label = QLabel("Ready")
        self.status_label.setStyleSheet("color: #666666; font-size: 10px;")
        main_layout.addWidget(self.status_label)

        self.setLayout(main_layout)

    def _button_style(self):
        return """
            QPushButton {
                background-color: #3d3d3d;
                color: white;
                border: 1px solid #4d4d4d;
                padding: 6px 12px;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #4d4d4d;
            }
            QPushButton:pressed {
                background-color: #2d2d2d;
            }
        """

    def refresh_compositions(self):
        """コンポジション一覧を更新"""
        try:
            self.comp_list.clear()
            self.info_label.setText("")

            project = ae.get_project()
            if not project:
                self.status_label.setText("No project open")
                return

            comps = project.compositions
            for comp in comps:
                # フォーマット: "名前 (幅x高さ, fps, 長さ)"
                duration_str = f"{comp.duration:.2f}s"
                text = f"{comp.name}"
                subtext = f"  {comp.width}x{comp.height} | {comp.fps:.2f}fps | {duration_str}"

                item = QListWidgetItem(text)
                item.setToolTip(subtext)
                item.setData(Qt.UserRole, comp)
                self.comp_list.addItem(item)

            self.status_label.setText(f"Found {len(comps)} compositions")

        except Exception as e:
            self.status_label.setText(f"Error: {str(e)}")
            ae.log_error(f"CompBrowser error: {e}")

    def on_selection_changed(self):
        """選択変更時"""
        item = self.comp_list.currentItem()
        if item:
            comp = item.data(Qt.UserRole)
            if comp:
                info = f"{comp.width}x{comp.height} | {comp.fps:.2f}fps | {comp.duration:.2f}s"
                self.info_label.setText(info)

    def on_comp_double_clicked(self, item):
        """コンポジションダブルクリック時"""
        self.open_comp(item)

    def open_selected_comp(self):
        """選択されたコンポジションを開く"""
        item = self.comp_list.currentItem()
        if item:
            self.open_comp(item)

    def open_comp(self, item):
        """コンポジションを開く"""
        try:
            comp = item.data(Qt.UserRole)
            if comp:
                comp.set_active()
                self.status_label.setText(f"Opened: {comp.name}")
                ae.log_info(f"Opened composition: {comp.name}")
        except Exception as e:
            self.status_label.setText(f"Error: {str(e)}")
            ae.log_error(f"Failed to open comp: {e}")


def main():
    """メイン関数"""
    if not ae.panel_ui.is_pyside_available():
        ae.log_error("PySide6 support is not available")
        ae.alert("PySide6 support is not available.")
        return

    # パネル作成
    panel = CompBrowserPanel()
    panel.setMinimumSize(250, 300)

    # パネル登録
    try:
        ae.panel_ui.register_pyside_panel("Comp Browser", panel)
        ae.log_info("Comp Browser panel registered!")
        panel.show()
    except Exception as e:
        ae.log_error(f"Failed to register panel: {e}")


if __name__ == "__main__":
    main()
