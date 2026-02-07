"""
PyAE - Layer Tools Panel

アクティブコンポジションのレイヤーを操作するツールパネル

使い方:
    import ae
    ae.execute_script_file("path/to/layer_tools.py")
"""

try:
    from PySide6.QtWidgets import (
        QWidget, QVBoxLayout, QHBoxLayout, QGridLayout,
        QListWidget, QPushButton, QLabel,
        QListWidgetItem, QFrame, QCheckBox, QSpinBox,
        QDoubleSpinBox, QGroupBox, QSlider, QTabWidget
    )
    from PySide6.QtCore import Qt
except ImportError:
    print("Error: PySide6 is not installed.")
    raise

import ae


class LayerToolsPanel(QWidget):
    """レイヤーツールパネル"""

    def __init__(self):
        super().__init__()
        self.init_ui()
        self.refresh_layers()

    def init_ui(self):
        """UI初期化"""
        main_layout = QVBoxLayout()
        main_layout.setContentsMargins(8, 8, 8, 8)
        main_layout.setSpacing(8)

        # ヘッダー
        header = QLabel("Layer Tools")
        header.setStyleSheet("""
            font-size: 14px;
            font-weight: bold;
            color: #00aaff;
        """)
        main_layout.addWidget(header)

        # タブウィジェット
        tabs = QTabWidget()
        tabs.setStyleSheet("""
            QTabWidget::pane {
                border: 1px solid #3d3d3d;
                border-radius: 4px;
            }
            QTabBar::tab {
                background-color: #2d2d2d;
                color: white;
                padding: 6px 12px;
                border: 1px solid #3d3d3d;
            }
            QTabBar::tab:selected {
                background-color: #0078d4;
            }
        """)

        # レイヤー一覧タブ
        layers_tab = self._create_layers_tab()
        tabs.addTab(layers_tab, "Layers")

        # ツールタブ
        tools_tab = self._create_tools_tab()
        tabs.addTab(tools_tab, "Tools")

        main_layout.addWidget(tabs)

        # ステータス
        self.status_label = QLabel("Ready")
        self.status_label.setStyleSheet("color: #666666; font-size: 10px;")
        main_layout.addWidget(self.status_label)

        self.setLayout(main_layout)

    def _create_layers_tab(self):
        """レイヤー一覧タブを作成"""
        widget = QWidget()
        layout = QVBoxLayout(widget)

        # レイヤーリスト
        self.layer_list = QListWidget()
        self.layer_list.setStyleSheet("""
            QListWidget {
                background-color: #2d2d2d;
                border: 1px solid #3d3d3d;
                color: white;
            }
            QListWidget::item:selected {
                background-color: #0078d4;
            }
        """)
        self.layer_list.setSelectionMode(QListWidget.ExtendedSelection)
        layout.addWidget(self.layer_list)

        # ボタン
        btn_layout = QHBoxLayout()

        refresh_btn = QPushButton("Refresh")
        refresh_btn.clicked.connect(self.refresh_layers)
        btn_layout.addWidget(refresh_btn)

        select_btn = QPushButton("Select All")
        select_btn.clicked.connect(lambda: self.layer_list.selectAll())
        btn_layout.addWidget(select_btn)

        layout.addLayout(btn_layout)

        return widget

    def _create_tools_tab(self):
        """ツールタブを作成"""
        widget = QWidget()
        layout = QVBoxLayout(widget)

        # レイヤー操作グループ
        ops_group = QGroupBox("Layer Operations")
        ops_group.setStyleSheet("""
            QGroupBox {
                color: white;
                border: 1px solid #3d3d3d;
                border-radius: 4px;
                margin-top: 8px;
                padding-top: 8px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 8px;
            }
        """)
        ops_layout = QGridLayout()

        # Shy/Solo/Lockボタン
        shy_btn = QPushButton("Toggle Shy")
        shy_btn.clicked.connect(lambda: self.toggle_flag("shy"))
        ops_layout.addWidget(shy_btn, 0, 0)

        solo_btn = QPushButton("Toggle Solo")
        solo_btn.clicked.connect(lambda: self.toggle_flag("solo"))
        ops_layout.addWidget(solo_btn, 0, 1)

        lock_btn = QPushButton("Toggle Lock")
        lock_btn.clicked.connect(lambda: self.toggle_flag("locked"))
        ops_layout.addWidget(lock_btn, 1, 0)

        visible_btn = QPushButton("Toggle Visible")
        visible_btn.clicked.connect(lambda: self.toggle_flag("video_active"))
        ops_layout.addWidget(visible_btn, 1, 1)

        ops_group.setLayout(ops_layout)
        layout.addWidget(ops_group)

        # 順序操作グループ
        order_group = QGroupBox("Layer Order")
        order_layout = QHBoxLayout()

        up_btn = QPushButton("Move Up")
        up_btn.clicked.connect(lambda: self.move_layers(-1))
        order_layout.addWidget(up_btn)

        down_btn = QPushButton("Move Down")
        down_btn.clicked.connect(lambda: self.move_layers(1))
        order_layout.addWidget(down_btn)

        order_group.setLayout(order_layout)
        order_group.setStyleSheet(ops_group.styleSheet())
        layout.addWidget(order_group)

        layout.addStretch()

        return widget

    def refresh_layers(self):
        """レイヤー一覧を更新"""
        try:
            self.layer_list.clear()

            comp = ae.get_active_comp()
            if not comp:
                self.status_label.setText("No active composition")
                return

            layers = comp.layers
            for layer in layers:
                # フォーマット: [index] 名前 (タイプ)
                flags = []
                if layer.shy:
                    flags.append("S")
                if layer.solo:
                    flags.append("O")
                if layer.locked:
                    flags.append("L")
                if not layer.video_active:
                    flags.append("H")

                flag_str = f" [{','.join(flags)}]" if flags else ""
                text = f"[{layer.index}] {layer.name}{flag_str}"

                item = QListWidgetItem(text)
                item.setData(Qt.UserRole, layer)
                self.layer_list.addItem(item)

            self.status_label.setText(f"Comp: {comp.name} ({len(layers)} layers)")

        except Exception as e:
            self.status_label.setText(f"Error: {str(e)}")

    def toggle_flag(self, flag_name):
        """選択されたレイヤーのフラグをトグル"""
        try:
            items = self.layer_list.selectedItems()
            if not items:
                return

            ae.begin_undo_group(f"Toggle {flag_name}")

            for item in items:
                layer = item.data(Qt.UserRole)
                if layer:
                    current_value = getattr(layer, flag_name)
                    setattr(layer, flag_name, not current_value)

            ae.end_undo_group()
            self.refresh_layers()
            self.status_label.setText(f"Toggled {flag_name} on {len(items)} layers")

        except Exception as e:
            ae.log_error(f"toggle_flag error: {e}")
            self.status_label.setText(f"Error: {str(e)}")

    def move_layers(self, direction):
        """選択されたレイヤーを移動"""
        try:
            items = self.layer_list.selectedItems()
            if not items:
                return

            ae.begin_undo_group("Move Layers")

            for item in items:
                layer = item.data(Qt.UserRole)
                if layer:
                    new_index = layer.index + direction
                    if new_index >= 1:
                        layer.move_to(new_index)

            ae.end_undo_group()
            self.refresh_layers()

        except Exception as e:
            ae.log_error(f"move_layers error: {e}")
            self.status_label.setText(f"Error: {str(e)}")


def main():
    """メイン関数"""
    if not ae.panel_ui.is_pyside_available():
        ae.log_error("PySide6 support is not available")
        return

    panel = LayerToolsPanel()
    panel.setMinimumSize(300, 400)

    try:
        ae.panel_ui.register_pyside_panel("Layer Tools", panel)
        ae.log_info("Layer Tools panel registered!")
        panel.show()
    except Exception as e:
        ae.log_error(f"Failed to register panel: {e}")


if __name__ == "__main__":
    main()
