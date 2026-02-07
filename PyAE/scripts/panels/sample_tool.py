"""
PyAE Sample Tool Panel

ae.panel_ui.set_widget() を使ってパネルにウィジェットを設定する例

使い方:
    1. Window > Sample Tool でパネルを開く
    2. PyAE Consoleで以下を実行:
       import ae
       ae.execute_script_file(r"C:\\path\\to\\sample_tool.py")
"""

from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton,
    QListWidget, QFrame
)
from PySide6.QtCore import Qt
import ae


class SampleToolWidget(QWidget):
    """サンプルツールパネルウィジェット"""

    def __init__(self):
        super().__init__()
        self._init_ui()

    def _init_ui(self):
        """UI初期化"""
        self.setStyleSheet("""
            QWidget {
                background-color: #2e2e2e;
                color: #dcdcdc;
                font-size: 12px;
            }
            QPushButton {
                background-color: #0078d4;
                color: white;
                border: none;
                padding: 8px 16px;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #106ebe;
            }
            QPushButton:pressed {
                background-color: #005a9e;
            }
            QListWidget {
                background-color: #232323;
                border: 1px solid #3e3e3e;
                border-radius: 4px;
            }
            QListWidget::item {
                padding: 4px;
            }
            QListWidget::item:selected {
                background-color: #0078d4;
            }
        """)

        layout = QVBoxLayout()
        layout.setContentsMargins(8, 8, 8, 8)
        layout.setSpacing(8)

        # タイトル
        title = QLabel("Sample Tool Panel")
        title.setAlignment(Qt.AlignCenter)
        title.setStyleSheet("font-size: 16px; font-weight: bold; color: #00aaff;")
        layout.addWidget(title)

        # 説明
        desc = QLabel("ae.panel_ui.set_widget() で設定されたウィジェット")
        desc.setAlignment(Qt.AlignCenter)
        desc.setStyleSheet("color: #888888; font-size: 11px;")
        layout.addWidget(desc)

        # 区切り線
        line = QFrame()
        line.setFrameShape(QFrame.HLine)
        line.setStyleSheet("background-color: #3e3e3e;")
        layout.addWidget(line)

        # コンポジションリスト
        list_label = QLabel("Compositions:")
        list_label.setStyleSheet("font-weight: bold;")
        layout.addWidget(list_label)

        self.comp_list = QListWidget()
        layout.addWidget(self.comp_list)

        # ボタン行
        btn_layout = QHBoxLayout()

        btn_refresh = QPushButton("Refresh")
        btn_refresh.clicked.connect(self._on_refresh)
        btn_layout.addWidget(btn_refresh)

        btn_info = QPushButton("Show Info")
        btn_info.clicked.connect(self._on_show_info)
        btn_layout.addWidget(btn_info)

        layout.addLayout(btn_layout)

        # フッター
        footer = QLabel("Set via ae.panel_ui.set_widget()")
        footer.setAlignment(Qt.AlignCenter)
        footer.setStyleSheet("color: #666666; font-size: 10px;")
        layout.addWidget(footer)

        self.setLayout(layout)

        # 初期データ読み込み
        self._on_refresh()

    def _on_refresh(self):
        """コンポジションリストを更新"""
        self.comp_list.clear()

        try:
            project = ae.get_project()
            if project:
                for item in project.items:
                    if hasattr(item, 'frame_rate'):
                        self.comp_list.addItem(item.name)

                if self.comp_list.count() == 0:
                    self.comp_list.addItem("(No compositions)")
            else:
                self.comp_list.addItem("(No project open)")
        except Exception as e:
            self.comp_list.addItem(f"(Error: {e})")

    def _on_show_info(self):
        """AE情報を表示"""
        info = ae.get_ae_info()
        ae.log_info(f"AE Version: {info.get('ae_version', 'unknown')}")
        ae.log_info(f"PyAE Version: {info.get('pyae_version', 'unknown')}")


# グローバル参照
_widget = None


def main():
    """メイン関数"""
    global _widget

    # ウィジェットを作成
    _widget = SampleToolWidget()

    # パネルにウィジェットを設定
    # パネル名はpanels_config.pyのmatch_nameと一致させる
    panel_name = "PyAE_SampleTool"

    if ae.panel_ui.set_widget(panel_name, _widget):
        ae.log_info(f"Widget set to panel '{panel_name}' successfully!")
    else:
        ae.log_error(f"Failed to set widget to panel '{panel_name}'")
        ae.log_info("Make sure the panel is open (Window > Sample Tool)")


if __name__ == "__main__":
    main()
