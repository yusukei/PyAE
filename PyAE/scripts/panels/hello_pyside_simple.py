"""
PyAE - Simple PySide6 Test (no PyAEPySide.dll required)

PyAEPySide.dllに依存しない、純粋なPySide6テスト

使い方:
    import ae
    ae.execute_script_file(r"C:\\Program Files\\Adobe\\Common\\Plug-ins\\7.0\\MediaCore\\PyAE\\scripts\\panels\\hello_pyside_simple.py")
"""

from PySide6.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout, QLabel
from PySide6.QtCore import Qt
import ae

# グローバル参照を保持してガベージコレクションを防ぐ
_active_panels = []

class HelloPanel(QWidget):
    """Hello Worldパネル"""

    def __init__(self):
        super().__init__()
        self.click_count = 0
        self.init_ui()

    def init_ui(self):
        """UI初期化"""
        self.setWindowTitle("PyAE PySide6 Test")
        self.setStyleSheet("background-color: #2e2e2e;")

        layout = QVBoxLayout()

        # タイトル
        title = QLabel("Hello from PySide6!")
        title.setAlignment(Qt.AlignCenter)
        title.setStyleSheet("font-size: 18px; font-weight: bold; color: #00aaff;")
        layout.addWidget(title)

        # 説明
        desc = QLabel("PyAEでPySide6 GUIが動作しています")
        desc.setAlignment(Qt.AlignCenter)
        desc.setStyleSheet("color: #888888;")
        layout.addWidget(desc)

        layout.addSpacing(20)

        # ボタン
        button = QPushButton("Click Me!")
        button.setStyleSheet("""
            QPushButton {
                background-color: #0078d4;
                color: white;
                border: none;
                padding: 10px 20px;
                border-radius: 5px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #106ebe;
            }
            QPushButton:pressed {
                background-color: #005a9e;
            }
        """)
        button.clicked.connect(self.on_button_clicked)
        layout.addWidget(button)

        layout.addSpacing(10)

        # カウンターラベル
        self.counter_label = QLabel("Clicks: 0")
        self.counter_label.setAlignment(Qt.AlignCenter)
        self.counter_label.setStyleSheet("color: white; font-size: 16px;")
        layout.addWidget(self.counter_label)

        # ストレッチを追加
        layout.addStretch()

        # AE情報
        ae_info = ae.get_ae_info()
        info_text = f"PyAE v{ae_info.get('pyae_version', 'unknown')}"
        info_label = QLabel(info_text)
        info_label.setAlignment(Qt.AlignCenter)
        info_label.setStyleSheet("color: #666666; font-size: 10px;")
        layout.addWidget(info_label)

        self.setLayout(layout)
        self.resize(300, 250)

    def on_button_clicked(self):
        """ボタンクリック時"""
        self.click_count += 1
        self.counter_label.setText(f"Clicks: {self.click_count}")
        ae.log_info(f"Button clicked {self.click_count} times!")


def main():
    """メイン関数"""
    global _active_panels

    # QApplicationがなければ作成
    app = QApplication.instance()
    if not app:
        app = QApplication([])

    # パネル作成・表示
    panel = HelloPanel()

    # ウィンドウを閉じた時にリストから削除
    def on_close():
        if panel in _active_panels:
            _active_panels.remove(panel)
        ae.log_info("PySide6 panel closed")

    panel.destroyed.connect(on_close)

    # グローバルリストに保持してGCを防ぐ
    _active_panels.append(panel)

    panel.show()
    ae.log_info(f"PySide6 panel displayed! (active panels: {len(_active_panels)})")


if __name__ == "__main__":
    main()
