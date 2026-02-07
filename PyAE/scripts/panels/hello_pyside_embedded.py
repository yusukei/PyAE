"""
PyAE - PySide6 Embedded Panel Test

AEパネル内にPySide6ウィジェットを埋め込むテスト

使い方:
    1. まずWindowメニューから「PyAE Panel」を開く
    2. PyAE Consoleで以下を実行:
       import ae
       ae.execute_script_file(r"C:\\Program Files\\Adobe\\Common\\Plug-ins\\7.0\\MediaCore\\PyAE\\scripts\\panels\\hello_pyside_embedded.py")

注意:
    - PyAE Console（スクリプトエディタ）とPyAE Panel（PySide6埋め込み用）は別のパネルです
    - PySide6ウィジェットはPyAE Panelに埋め込まれます
"""

from PySide6.QtWidgets import (
    QApplication, QWidget, QPushButton, QVBoxLayout, QLabel,
    QHBoxLayout, QFrame, QTextEdit
)
from PySide6.QtCore import Qt
from PySide6.QtGui import QFont
import ae

# グローバル参照を保持
_embedded_widget = None


class EmbeddedPanel(QWidget):
    """AEパネルに埋め込むウィジェット"""

    def __init__(self, frameless=False):
        super().__init__()
        self.click_count = 0

        # フレームレスモード（AEパネル埋め込み用）
        # Qtが内部的にフレーム分のオフセットを計算しないようにする
        if frameless:
            self.setWindowFlag(Qt.FramelessWindowHint)

        self.init_ui()

    def init_ui(self):
        """UI初期化"""
        self.setStyleSheet("""
            QWidget {
                background-color: #2e2e2e;
                color: #dcdcdc;
            }
            QPushButton {
                background-color: #0078d4;
                color: white;
                border: none;
                padding: 8px 16px;
                border-radius: 4px;
                font-size: 12px;
            }
            QPushButton:hover {
                background-color: #106ebe;
            }
            QPushButton:pressed {
                background-color: #005a9e;
            }
            QLabel {
                color: #dcdcdc;
            }
            QTextEdit {
                background-color: #232323;
                color: #dcdcdc;
                border: 1px solid #3e3e3e;
                border-radius: 4px;
            }
        """)

        main_layout = QVBoxLayout()
        main_layout.setContentsMargins(8, 8, 8, 8)
        main_layout.setSpacing(8)

        # ヘッダー
        header = QLabel("PySide6 Embedded Panel")
        header.setAlignment(Qt.AlignCenter)
        header.setStyleSheet("font-size: 16px; font-weight: bold; color: #00aaff;")
        main_layout.addWidget(header)

        # 情報ラベル
        info = QLabel("This widget is embedded in the AE panel!")
        info.setAlignment(Qt.AlignCenter)
        info.setStyleSheet("color: #888888; font-size: 11px;")
        main_layout.addWidget(info)

        # 区切り線
        line = QFrame()
        line.setFrameShape(QFrame.HLine)
        line.setStyleSheet("background-color: #3e3e3e;")
        main_layout.addWidget(line)

        # ボタン行
        btn_layout = QHBoxLayout()

        btn_click = QPushButton("Click Me!")
        btn_click.clicked.connect(self.on_click)
        btn_layout.addWidget(btn_click)

        btn_ae_info = QPushButton("Get AE Info")
        btn_ae_info.clicked.connect(self.on_ae_info)
        btn_layout.addWidget(btn_ae_info)

        main_layout.addLayout(btn_layout)

        # カウンターラベル
        self.counter_label = QLabel("Clicks: 0")
        self.counter_label.setAlignment(Qt.AlignCenter)
        self.counter_label.setStyleSheet("font-size: 14px;")
        main_layout.addWidget(self.counter_label)

        # 出力テキストエリア
        self.output = QTextEdit()
        self.output.setReadOnly(True)
        self.output.setPlaceholderText("Output will appear here...")
        self.output.setMaximumHeight(150)
        main_layout.addWidget(self.output)

        # ストレッチ
        main_layout.addStretch()

        # フッター
        ae_info = ae.get_ae_info()
        footer = QLabel(f"PyAE v{ae_info.get('pyae_version', '?')}")
        footer.setAlignment(Qt.AlignCenter)
        footer.setStyleSheet("color: #666666; font-size: 10px;")
        main_layout.addWidget(footer)

        self.setLayout(main_layout)

    def on_click(self):
        """クリックボタン"""
        self.click_count += 1
        self.counter_label.setText(f"Clicks: {self.click_count}")
        self.append_output(f"Button clicked {self.click_count} times!")

    def on_ae_info(self):
        """AE情報取得"""
        info = ae.get_ae_info()
        self.append_output(f"AE Version: {info.get('ae_version', 'unknown')}")
        self.append_output(f"PyAE Version: {info.get('pyae_version', 'unknown')}")

        # プロジェクト情報
        try:
            proj = ae.get_project()
            if proj:
                self.append_output(f"Project: {proj.name}")
        except:
            self.append_output("No project open")

    def append_output(self, text):
        """出力追加"""
        self.output.append(text)
        ae.log_info(text)


def main():
    """メイン関数"""
    global _embedded_widget

    # QApplicationがなければ作成
    app = QApplication.instance()
    if not app:
        app = QApplication([])

    # PySide用パネルのHWNDを確認
    # get_pyside_panel_hwnd() はPyAE Panel専用（PyAE Consoleではない）
    hwnd = ae.panel_ui.get_pyside_panel_hwnd()
    if not hwnd:
        ae.log_warning("PyAE Panel not found. Please open Window > PyAE Panel first.")
        ae.log_info("Showing as standalone window instead...")
        # スタンドアロンウィンドウとして表示
        _embedded_widget = EmbeddedPanel()
        _embedded_widget.resize(350, 400)
        _embedded_widget.show()
        ae.log_info("PySide6 panel displayed as standalone window")
        return

    ae.log_info(f"Found PyAE Panel HWND: {hwnd}")

    # パネル作成（埋め込み用なのでフレームレスで作成）
    # これによりQtがフレーム分のオフセットを計算しなくなる
    _embedded_widget = EmbeddedPanel(frameless=True)

    # PyAE Panelに埋め込み
    if ae.panel_ui.embed_widget(_embedded_widget):
        ae.log_info("PySide6 panel embedded in PyAE Panel successfully!")
        # 念のため明示的にshow()を呼び出す
        _embedded_widget.show()
        _embedded_widget.raise_()
        _embedded_widget.activateWindow()
    else:
        ae.log_error("Failed to embed panel. Showing as standalone window.")
        _embedded_widget.resize(350, 400)
        _embedded_widget.show()


if __name__ == "__main__":
    main()
