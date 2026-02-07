パネルUI（PySide統合）
======================

.. currentmodule:: ae.panel_ui

``ae.panel_ui`` モジュールは、PySide/Qt を使用したカスタムUIパネルを After Effects に統合するためのAPIを提供します。

概要
----

このモジュールを使用すると、Qt/PySide で作成したウィジェットを After Effects のパネルとして
登録・表示できます。パネルは After Effects のウィンドウメニューからアクセスでき、
ドッキング可能なパネルとして動作します。

**主な機能**:

- PySide パネルの登録と管理
- Qt ウィジェットの After Effects への埋め込み
- パネルのウィンドウハンドル取得
- 登録状態の確認

.. important::
   この機能を使用するには、PySide6 がインストールされている必要があります。

   .. code-block:: bash

      pip install PySide6

基本的な使い方
--------------

PySide利用可能性の確認
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   if ae.panel_ui.is_pyside_available():
       print("PySide is available")
   else:
       print("PySide is not installed")

シンプルなパネルの作成
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from PySide6.QtWidgets import QWidget, QVBoxLayout, QPushButton, QLabel

   class MyPanel(QWidget):
       def __init__(self):
           super().__init__()
           self.setup_ui()

       def setup_ui(self):
           layout = QVBoxLayout(self)

           # ラベル
           self.label = QLabel("Hello from PySide!")
           layout.addWidget(self.label)

           # ボタン
           self.button = QPushButton("Click Me")
           self.button.clicked.connect(self.on_click)
           layout.addWidget(self.button)

       def on_click(self):
           comp = ae.Comp.get_active()
           if comp:
               self.label.setText(f"Active: {comp.name}")
           else:
               self.label.setText("No active composition")

   # パネルを登録
   panel_widget = MyPanel()
   ae.panel_ui.register_pyside_panel(
       "my_panel",           # パネルID
       "My Custom Panel",    # 表示名
       panel_widget          # ウィジェット
   )

API リファレンス
----------------

PySide 確認
~~~~~~~~~~~

.. function:: is_pyside_available() -> bool

   PySide が利用可能かどうかを確認します。

   :return: PySide が利用可能な場合は True
   :rtype: bool

   **例**:

   .. code-block:: python

      if ae.panel_ui.is_pyside_available():
          from PySide6.QtWidgets import QWidget
          # PySide を使用した処理

パネル登録
~~~~~~~~~~

.. function:: register_pyside_panel(panel_id: str, name: str, widget: QWidget) -> bool

   PySide ウィジェットをパネルとして登録します。

   :param panel_id: パネルの一意識別子
   :type panel_id: str
   :param name: ウィンドウメニューに表示される名前
   :type name: str
   :param widget: 表示する Qt ウィジェット
   :type widget: QWidget
   :return: 登録が成功した場合は True
   :rtype: bool

   .. note::
      - panel_id は一意である必要があります
      - 既に登録されている panel_id を指定するとエラーになります

.. function:: unregister_panel(panel_id: str) -> bool

   パネルの登録を解除します。

   :param panel_id: 登録解除するパネルのID
   :type panel_id: str
   :return: 解除が成功した場合は True
   :rtype: bool

.. function:: is_panel_registered(panel_id: str) -> bool

   パネルが登録されているかどうかを確認します。

   :param panel_id: 確認するパネルのID
   :type panel_id: str
   :return: 登録されている場合は True
   :rtype: bool

ウィンドウハンドル
~~~~~~~~~~~~~~~~~~

.. function:: get_pyside_panel_hwnd(panel_id: str) -> int

   PySide パネルのウィンドウハンドルを取得します。

   :param panel_id: パネルID
   :type panel_id: str
   :return: ウィンドウハンドル（HWND）
   :rtype: int

.. function:: get_ae_panel_hwnd() -> int

   After Effects のメインパネルのウィンドウハンドルを取得します。

   :return: After Effects のウィンドウハンドル（HWND）
   :rtype: int

ウィジェット操作
~~~~~~~~~~~~~~~~

.. function:: embed_widget(panel_id: str, widget: QWidget) -> bool

   ウィジェットをパネルに埋め込みます。

   :param panel_id: 対象のパネルID
   :type panel_id: str
   :param widget: 埋め込むウィジェット
   :type widget: QWidget
   :return: 成功した場合は True
   :rtype: bool

.. function:: set_widget(panel_id: str, widget: QWidget) -> bool

   パネルのウィジェットを設定または置換します。

   :param panel_id: 対象のパネルID
   :type panel_id: str
   :param widget: 設定するウィジェット
   :type widget: QWidget
   :return: 成功した場合は True
   :rtype: bool

実用例
------

コンポジション情報パネル
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from PySide6.QtWidgets import (
       QWidget, QVBoxLayout, QHBoxLayout, QLabel,
       QPushButton, QTableWidget, QTableWidgetItem
   )
   from PySide6.QtCore import QTimer

   class CompInfoPanel(QWidget):
       """アクティブなコンポジションの情報を表示するパネル"""

       def __init__(self):
           super().__init__()
           self.setup_ui()
           self.setup_timer()

       def setup_ui(self):
           layout = QVBoxLayout(self)

           # コンポジション名
           self.comp_label = QLabel("No active composition")
           layout.addWidget(self.comp_label)

           # レイヤーテーブル
           self.layer_table = QTableWidget()
           self.layer_table.setColumnCount(3)
           self.layer_table.setHorizontalHeaderLabels(["Index", "Name", "Type"])
           layout.addWidget(self.layer_table)

           # 更新ボタン
           refresh_btn = QPushButton("Refresh")
           refresh_btn.clicked.connect(self.update_info)
           layout.addWidget(refresh_btn)

       def setup_timer(self):
           """定期的に情報を更新"""
           self.timer = QTimer(self)
           self.timer.timeout.connect(self.update_info)
           self.timer.start(1000)  # 1秒ごと

       def update_info(self):
           """コンポジション情報を更新"""
           comp = ae.Comp.get_active()

           if not comp or not comp.valid:
               self.comp_label.setText("No active composition")
               self.layer_table.setRowCount(0)
               return

           # コンポジション名を更新
           self.comp_label.setText(
               f"{comp.name} ({comp.width}x{comp.height}, {comp.frame_rate}fps)"
           )

           # レイヤー情報を更新
           layers = comp.layers
           self.layer_table.setRowCount(len(layers))

           for i, layer in enumerate(layers):
               self.layer_table.setItem(i, 0, QTableWidgetItem(str(i + 1)))
               self.layer_table.setItem(i, 1, QTableWidgetItem(layer.name))
               self.layer_table.setItem(i, 2, QTableWidgetItem(str(layer.layer_type)))

   # パネルを登録
   if ae.panel_ui.is_pyside_available():
       panel = CompInfoPanel()
       ae.panel_ui.register_pyside_panel(
           "comp_info_panel",
           "Composition Info",
           panel
       )

レイヤー操作パネル
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from PySide6.QtWidgets import (
       QWidget, QVBoxLayout, QHBoxLayout, QLabel,
       QPushButton, QLineEdit, QSpinBox, QDoubleSpinBox,
       QGroupBox, QFormLayout
   )

   class LayerToolsPanel(QWidget):
       """レイヤー操作ツールパネル"""

       def __init__(self):
           super().__init__()
           self.setup_ui()

       def setup_ui(self):
           layout = QVBoxLayout(self)

           # Null レイヤー作成
           null_group = QGroupBox("Null Layer")
           null_layout = QFormLayout(null_group)

           self.null_name = QLineEdit("Null 1")
           null_layout.addRow("Name:", self.null_name)

           null_btn = QPushButton("Create Null")
           null_btn.clicked.connect(self.create_null)
           null_layout.addRow(null_btn)

           layout.addWidget(null_group)

           # ソリッドレイヤー作成
           solid_group = QGroupBox("Solid Layer")
           solid_layout = QFormLayout(solid_group)

           self.solid_name = QLineEdit("Solid 1")
           solid_layout.addRow("Name:", self.solid_name)

           self.solid_width = QSpinBox()
           self.solid_width.setRange(1, 8192)
           self.solid_width.setValue(1920)
           solid_layout.addRow("Width:", self.solid_width)

           self.solid_height = QSpinBox()
           self.solid_height.setRange(1, 8192)
           self.solid_height.setValue(1080)
           solid_layout.addRow("Height:", self.solid_height)

           solid_btn = QPushButton("Create Solid")
           solid_btn.clicked.connect(self.create_solid)
           solid_layout.addRow(solid_btn)

           layout.addWidget(solid_group)

           layout.addStretch()

       def create_null(self):
           """Null レイヤーを作成"""
           comp = ae.Comp.get_active()
           if not comp:
               return

           with ae.UndoGroup("Create Null Layer"):
               layer = comp.add_null(self.null_name.text())
               print(f"Created null: {layer.name}")

       def create_solid(self):
           """ソリッドレイヤーを作成"""
           comp = ae.Comp.get_active()
           if not comp:
               return

           with ae.UndoGroup("Create Solid Layer"):
               layer = comp.add_solid(
                   self.solid_name.text(),
                   self.solid_width.value(),
                   self.solid_height.value()
               )
               print(f"Created solid: {layer.name}")

   # パネルを登録
   if ae.panel_ui.is_pyside_available():
       panel = LayerToolsPanel()
       ae.panel_ui.register_pyside_panel(
           "layer_tools",
           "Layer Tools",
           panel
       )

パネルの動的更新
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel
   from PySide6.QtCore import Signal, QObject

   class AEEventBridge(QObject):
       """After Effects イベントを Qt シグナルに変換"""
       selection_changed = Signal()
       comp_changed = Signal()

   class DynamicPanel(QWidget):
       def __init__(self):
           super().__init__()
           self.bridge = AEEventBridge()
           self.bridge.selection_changed.connect(self.on_selection_changed)

           self.setup_ui()

       def setup_ui(self):
           layout = QVBoxLayout(self)
           self.info_label = QLabel("Select a layer...")
           layout.addWidget(self.info_label)

       def on_selection_changed(self):
           """選択変更時の処理"""
           comp = ae.Comp.get_active()
           if not comp:
               self.info_label.setText("No active composition")
               return

           selected = comp.selected_layers
           if selected:
               layer = selected[0]
               self.info_label.setText(
                   f"Selected: {layer.name}\n"
                   f"In: {layer.in_point:.2f}s\n"
                   f"Out: {layer.out_point:.2f}s"
               )
           else:
               self.info_label.setText("No layer selected")

注意事項
--------

.. note::
   - PySide6 のインストールが必要です
   - パネルIDは一意である必要があります（プレフィックスを使用することを推奨）
   - After Effects を終了する前にパネルを正しくクリーンアップしてください

.. warning::
   - メインスレッド以外から Qt ウィジェットを操作しないでください
   - 大量の更新を行う場合は、タイマーの間隔を適切に設定してください
   - パネルのウィジェット参照を保持しないと、ガベージコレクションされる可能性があります

制限事項
--------

1. **Windows のみ**: 現在、この機能は Windows でのみ利用可能です
2. **PySide6 推奨**: PySide2 でも動作する可能性がありますが、PySide6 を推奨します
3. **シングルインスタンス**: 同じパネルIDで複数のパネルを登録することはできません

関連項目
--------

- :doc:`menu` - メニュー・コマンドAPI
- :doc:`persistent_data` - 設定の永続化
