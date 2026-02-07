"""
PyAE - Hello World PySide Panel

最もシンプルなPySide6パネルのサンプル
PySide6とQt6がインストールされている必要があります。

使い方:
    import ae
    ae.execute_script_file("path/to/hello_pyside.py")
"""

try:
    from PySide6.QtWidgets import QWidget, QPushButton, QVBoxLayout, QLabel
    from PySide6.QtCore import Qt
except ImportError:
    print("Error: PySide6 is not installed.")
    print("Install with: pip install PySide6")
    raise

import ae


class HelloPanel(QWidget):
    """Hello Worldパネル"""

    def __init__(self):
        super().__init__()
        self.click_count = 0
        self.init_ui()

    def init_ui(self):
        """UI初期化"""
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

    def on_button_clicked(self):
        """ボタンクリック時"""
        self.click_count += 1
        self.counter_label.setText(f"Clicks: {self.click_count}")

        # AEコンソールにも出力
        ae.log_info(f"Button clicked {self.click_count} times!")


def main():
    """メイン関数"""
    # PySideサポートが利用可能か確認
    if not ae.panel_ui.is_pyside_available():
        ae.log_error("PySide6 support is not available")
        ae.alert("PySide6 support is not available.\n"
                 "Make sure PyAEPySide.dll and Qt6 are installed.")
        return

    # パネル作成
    panel = HelloPanel()
    panel.setMinimumSize(300, 200)

    # パネル登録
    try:
        ae.panel_ui.register_pyside_panel("Hello PySide", panel)
        ae.log_info("Hello PySide panel registered successfully!")

        # スタンドアロンで表示（デバッグ用）
        panel.show()

    except Exception as e:
        ae.log_error(f"Failed to register panel: {e}")
        ae.alert(f"Failed to register panel:\n{e}")


if __name__ == "__main__":
    main()
