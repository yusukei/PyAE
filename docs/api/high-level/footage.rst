Footage
=======

.. currentmodule:: ae

Footageクラスは、After Effectsのフッテージ（メディアソース）を表します。

概要
----

``Footage`` クラスは、After Effectsフッテージへの統合的なアクセスを提供します。プロジェクト追加前とプロジェクト追加後の両方の状態を1つのクラスで扱えます。

**特徴**:

- 統合されたAPI（追加前/追加後の両状態に対応）
- ファクトリメソッドによる作成
- 型ヒント完備
- 自動リソース管理

**基本的なインポート**:

.. code-block:: python

   import ae
   from typing import Optional, Tuple, Dict

   # ファイルからフッテージを作成
   footage = ae.Footage.from_file("/path/to/video.mp4")

.. note::
   **APIの統合について**

   以前は ``Footage`` （追加前）と ``FootageItem`` （追加後）の2つのクラスに
   分かれていましたが、現在は ``Footage`` クラスに統合されています。

   ``FootageItem`` は後方互換性のためのエイリアスとして残っていますが、
   新しいコードでは ``Footage`` を使用してください。

クラスリファレンス
------------------

.. class:: Footage

   After Effectsフッテージを表す統合クラス。

   フッテージは2つの状態を持ちます：

   - **追加前** (``is_in_project == False``): ファクトリメソッドで作成直後
   - **追加後** (``is_in_project == True``): プロジェクトに追加後

プロパティ一覧
~~~~~~~~~~~~~~

状態プロパティ
^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 25 25 50

   * - プロパティ
     - 型
     - 説明
   * - ``valid``
     - bool
     - フッテージが有効かどうか（読み取り専用）
   * - ``is_in_project``
     - bool
     - プロジェクトに追加済みかどうか（読み取り専用）

共通プロパティ（両状態）
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 25 25 50

   * - プロパティ
     - 型
     - 説明
   * - ``path``
     - str
     - ファイルパス（読み取り専用）
   * - ``layer_key``
     - Dict
     - レイヤー情報（PSD等）（読み取り専用）
   * - ``sound_format``
     - Dict
     - サウンドフォーマット情報（読み取り専用）
   * - ``sequence_options``
     - Dict
     - シーケンスインポートオプション（読み取り専用）

追加前のみのプロパティ
^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 25 25 50

   * - プロパティ
     - 型
     - 説明
   * - ``num_files``
     - Tuple[int, int]
     - (総ファイル数, フレームあたりファイル数)（読み取り専用）
   * - ``signature``
     - FootageSignature
     - フッテージの署名/タイプ（読み取り専用）

追加後のみのプロパティ
^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 25 25 50

   * - プロパティ
     - 型
     - 説明
   * - ``footage_type``
     - FootageType
     - フッテージの種類（読み取り専用）
   * - ``interpretation``
     - Dict
     - 解釈ルール（読み取り専用）
   * - ``name``
     - str
     - アイテム名（読み書き可能）
   * - ``comment``
     - str
     - コメント（読み書き可能）
   * - ``label``
     - int
     - ラベル色（読み書き可能）
   * - ``selected``
     - bool
     - 選択状態（読み書き可能）
   * - ``width``
     - int
     - 幅（ピクセル）（読み取り専用）
   * - ``height``
     - int
     - 高さ（ピクセル）（読み取り専用）
   * - ``duration``
     - float
     - デュレーション（秒）（読み取り専用）
   * - ``frame_rate``
     - float
     - フレームレート（読み取り専用）
   * - ``missing``
     - bool
     - ソースが見つからないか（読み取り専用）
   * - ``item_id``
     - int
     - アイテムID（読み取り専用）

メソッド一覧
~~~~~~~~~~~~

静的メソッド（ファクトリ）
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 55 45

   * - メソッド
     - 説明
   * - ``Footage.from_file(path, style) -> Footage``
     - ファイルからフッテージを作成
   * - ``Footage.solid(name, width, height, color) -> Footage``
     - ソリッドを作成
   * - ``Footage.placeholder(name, width, height, duration) -> Footage``
     - プレースホルダーを作成
   * - ``Footage.placeholder_with_path(path, ...) -> Footage``
     - パス付きプレースホルダーを作成

インスタンスメソッド
^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 55 45

   * - メソッド
     - 説明
   * - ``add_to_project(folder=None) -> int``
     - プロジェクトに追加（追加前のみ）
   * - ``delete()``
     - プロジェクトから削除（追加後のみ）
   * - ``set_interpretation(alpha_mode, fps)``
     - 解釈ルールを設定（追加後のみ）
   * - ``get_solid_color(proxy=False) -> Tuple``
     - ソリッドの色を取得（追加後のみ）
   * - ``set_solid_color(color_rgba, proxy=False)``
     - ソリッドの色を設定（追加後のみ）
   * - ``set_solid_dimensions(width, height, proxy=False)``
     - ソリッドのサイズを設定（追加後のみ）

プロパティ
~~~~~~~~~~

.. attribute:: Footage.valid
   :type: bool

   フッテージが有効かどうか。

   削除されたフッテージや無効なフッテージの場合は ``False`` になります。

   **例**:

   .. code-block:: python

      footage = ae.Footage.from_file("/path/to/video.mp4")
      if footage.valid:
          print("フッテージは有効です")

.. attribute:: Footage.is_in_project
   :type: bool

   フッテージがプロジェクトに追加済みかどうか。

   - ``False``: ファクトリメソッドで作成直後（追加前）
   - ``True``: ``add_to_project()`` 呼び出し後（追加後）

   **例**:

   .. code-block:: python

      footage = ae.Footage.from_file("/path/to/video.mp4")
      print(footage.is_in_project)  # False

      footage.add_to_project()
      print(footage.is_in_project)  # True

.. attribute:: Footage.path
   :type: str

   フッテージのファイルパス。

   ソリッドの場合は空文字列が返されます。

   **例**:

   .. code-block:: python

      footage = ae.Footage.from_file("/path/to/video.mp4")
      print(f"パス: {footage.path}")

.. attribute:: Footage.signature
   :type: FootageSignature

   フッテージの署名（ソース状態）。追加前のみ有効。

   - ``NONE`` (0): フッテージなし/無効
   - ``MISSING`` (1): ソースファイルが見つからない
   - ``SOLID`` (2): ソリッドカラー

   **例**:

   .. code-block:: python

      solid = ae.Footage.solid("Red", 100, 100, (1.0, 0.0, 0.0, 1.0))
      print(f"署名: {solid.signature}")  # 2 (SOLID)

.. attribute:: Footage.footage_type
   :type: FootageType

   フッテージの種類。追加後のみ有効。

   - ``None_`` (0): なし/無効
   - ``Solid`` (1): ソリッドカラー
   - ``Missing`` (2): ソースファイルが見つからない
   - ``Placeholder`` (3): プレースホルダー

   **例**:

   .. code-block:: python

      for item in project.items:
          if hasattr(item, 'footage_type'):
              if item.footage_type == ae.FootageType.Solid:
                  print(f"ソリッド: {item.name}")

.. attribute:: Footage.name
   :type: str

   アイテム名。追加後のみ有効。読み書き可能。

   **例**:

   .. code-block:: python

      footage = ae.Footage.from_file("/path/to/video.mp4")
      footage.add_to_project()
      footage.name = "My Video"
      print(f"名前: {footage.name}")

.. attribute:: Footage.interpretation
   :type: Dict

   解釈ルール。追加後のみ有効。

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``alpha_mode``
        - str
        - アルファモード ('straight', 'premultiplied', 'ignore')
      * - ``premul_color``
        - Tuple
        - プリマルチプライの色（premultipliedの場合）
      * - ``native_fps``
        - float
        - ネイティブフレームレート
      * - ``conform_fps``
        - float
        - コンフォームフレームレート

   **例**:

   .. code-block:: python

      footage.add_to_project()
      interp = footage.interpretation
      print(f"アルファモード: {interp.get('alpha_mode')}")
      print(f"FPS: {interp.get('native_fps')}")

.. attribute:: Footage.width
   :type: int

   幅（ピクセル）。追加後のみ有効。

   **例**:

   .. code-block:: python

      footage.add_to_project()
      print(f"サイズ: {footage.width}x{footage.height}")

.. attribute:: Footage.height
   :type: int

   高さ（ピクセル）。追加後のみ有効。

.. attribute:: Footage.duration
   :type: float

   デュレーション（秒）。追加後のみ有効。

   **例**:

   .. code-block:: python

      footage.add_to_project()
      print(f"長さ: {footage.duration}秒")

.. attribute:: Footage.frame_rate
   :type: float

   フレームレート。追加後のみ有効。

.. attribute:: Footage.missing
   :type: bool

   ソースファイルが見つからないかどうか。追加後のみ有効。

   **例**:

   .. code-block:: python

      if footage.missing:
          print("警告: ソースファイルが見つかりません")

メソッド
~~~~~~~~

.. method:: Footage.add_to_project(folder: Optional[Folder] = None) -> int

   フッテージをプロジェクトに追加します。

   :param folder: 親フォルダ（Folderオブジェクト）。Noneの場合ルートフォルダ
   :type folder: Optional[Folder]
   :return: 作成されたアイテムハンドル
   :rtype: int
   :raises RuntimeError: 既にプロジェクトに追加済みの場合

   .. note::
      このメソッドを呼び出すと ``is_in_project`` が ``True`` になり、
      追加後専用のプロパティとメソッドが使用可能になります。

   **例**:

   .. code-block:: python

      footage = ae.Footage.from_file("/path/to/video.mp4")
      footage.add_to_project()

      # 追加後はnameプロパティにアクセス可能
      footage.name = "My Video"

      # Folder オブジェクトを指定
      my_folder = ae.get_project().root_folder[0]
      footage2 = ae.Footage.from_file("/path/to/image.png")
      footage2.add_to_project(my_folder)

      # 引数なしでルートに追加
      footage3 = ae.Footage.from_file("/path/to/audio.wav")
      footage3.add_to_project()

.. method:: Footage.delete()

   フッテージをプロジェクトから削除します。

   :raises RuntimeError: プロジェクトに追加されていない場合

   .. warning::
      削除後、このFootageオブジェクトは無効になります。

   **例**:

   .. code-block:: python

      footage.delete()
      print(footage.valid)  # False

.. method:: Footage.set_interpretation(alpha_mode: str = "", fps: float = 0.0)

   フッテージの解釈ルールを設定します。追加後のみ有効。

   :param alpha_mode: アルファモード ('straight', 'premultiplied', 'ignore'、空文字列で変更なし)
   :type alpha_mode: str
   :param fps: コンフォームフレームレート（0.0 で変更なし）
   :type fps: float
   :raises RuntimeError: プロジェクトに追加されていない場合

   **例**:

   .. code-block:: python

      footage.add_to_project()

      # アルファモードを設定
      footage.set_interpretation(alpha_mode="premultiplied")

      # フレームレートを設定
      footage.set_interpretation(fps=29.97)

      # 両方同時に設定
      footage.set_interpretation(alpha_mode="straight", fps=24.0)

.. method:: Footage.get_solid_color(proxy: bool = False) -> Tuple[float, float, float, float]

   ソリッドの色を取得します。追加後のソリッドのみ有効。

   :param proxy: ``True`` でプロキシの色を取得
   :type proxy: bool
   :return: RGBA色値のタプル（各値 0.0〜1.0）
   :rtype: Tuple[float, float, float, float]
   :raises RuntimeError: ソリッドでない場合

   **例**:

   .. code-block:: python

      solid = ae.Footage.solid("Red", 100, 100, (1.0, 0.0, 0.0, 1.0))
      solid.add_to_project()

      color = solid.get_solid_color()
      print(f"R={color[0]}, G={color[1]}, B={color[2]}, A={color[3]}")

.. method:: Footage.set_solid_color(color_rgba: Tuple[float, float, float, float], proxy: bool = False)

   ソリッドの色を設定します。追加後のソリッドのみ有効。

   :param color_rgba: RGBA色値のタプル（各値 0.0〜1.0）
   :type color_rgba: Tuple[float, float, float, float]
   :param proxy: ``True`` でプロキシの色を設定
   :type proxy: bool
   :raises RuntimeError: ソリッドでない場合

   **例**:

   .. code-block:: python

      solid.set_solid_color((0.0, 0.0, 1.0, 1.0))  # 青に変更

.. method:: Footage.set_solid_dimensions(width: int, height: int, proxy: bool = False)

   ソリッドのサイズを設定します。追加後のソリッドのみ有効。

   :param width: 幅（ピクセル、1〜30000）
   :type width: int
   :param height: 高さ（ピクセル、1〜30000）
   :type height: int
   :param proxy: ``True`` でプロキシのサイズを設定
   :type proxy: bool
   :raises RuntimeError: ソリッドでない場合

   **例**:

   .. code-block:: python

      solid.set_solid_dimensions(1920, 1080)
      print(f"新サイズ: {solid.width}x{solid.height}")

静的メソッド
~~~~~~~~~~~~

.. staticmethod:: Footage.from_file(path: str, style: InterpretationStyle = InterpretationStyle.NO_DIALOG_GUESS) -> Footage

   ファイルからフッテージを作成します。

   :param path: ファイルパス
   :type path: str
   :param style: 解釈スタイル（デフォルト: ``NO_DIALOG_GUESS``）
   :type style: InterpretationStyle
   :return: 新しいFootageインスタンス（プロジェクト未追加）
   :rtype: Footage
   :raises RuntimeError: ファイルが存在しないか読み込めない場合

   **例**:

   .. code-block:: python

      # 基本的な使用法
      footage = ae.Footage.from_file("/path/to/video.mp4")

      # 解釈スタイルを指定
      footage = ae.Footage.from_file(
          "/path/to/video.mp4",
          style=ae.InterpretationStyle.DIALOG_OK
      )

.. staticmethod:: Footage.solid(name: str, width: int, height: int, color: Tuple[float, float, float, float]) -> Footage

   ソリッドカラーのフッテージを作成します。

   :param name: ソリッドの名前
   :type name: str
   :param width: 幅（ピクセル、1〜30000）
   :type width: int
   :param height: 高さ（ピクセル、1〜30000）
   :type height: int
   :param color: RGBA色値のタプル（各値 0.0〜1.0）
   :type color: Tuple[float, float, float, float]
   :return: 新しいFootageインスタンス（プロジェクト未追加）
   :rtype: Footage

   **例**:

   .. code-block:: python

      # 赤いソリッドを作成
      red_solid = ae.Footage.solid(
          name="Red Solid",
          width=1920,
          height=1080,
          color=(1.0, 0.0, 0.0, 1.0)
      )
      red_solid.add_to_project()

.. staticmethod:: Footage.placeholder(name: str, width: int, height: int, duration: float) -> Footage

   プレースホルダーフッテージを作成します。

   :param name: プレースホルダーの名前
   :type name: str
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int
   :param duration: デュレーション（秒）
   :type duration: float
   :return: 新しいFootageインスタンス（プロジェクト未追加）
   :rtype: Footage

   **例**:

   .. code-block:: python

      placeholder = ae.Footage.placeholder(
          name="Missing Footage",
          width=1920,
          height=1080,
          duration=10.0
      )
      placeholder.add_to_project()

.. staticmethod:: Footage.placeholder_with_path(path: str, width: int, height: int, duration: float) -> Footage

   ファイルパス付きのプレースホルダーフッテージを作成します。

   :param path: プレースホルダーのファイルパス
   :type path: str
   :param width: 幅（ピクセル）
   :type width: int
   :param height: 高さ（ピクセル）
   :type height: int
   :param duration: デュレーション（秒）
   :type duration: float
   :return: 新しいFootageインスタンス（プロジェクト未追加）
   :rtype: Footage

   **例**:

   .. code-block:: python

      placeholder = ae.Footage.placeholder_with_path(
          path="/path/to/missing/file.mov",
          width=1920,
          height=1080,
          duration=5.0
      )

列挙型
------

FootageSignature
~~~~~~~~~~~~~~~~

プロジェクト追加前のフッテージソースの状態を表します。

.. list-table::
   :header-rows: 1
   :widths: 25 10 65

   * - 値
     - 数値
     - 説明
   * - ``NONE``
     - 0
     - フッテージなし/無効
   * - ``MISSING``
     - 1
     - ソースファイルが見つからない
   * - ``SOLID``
     - 2
     - ソリッドカラー

FootageType
~~~~~~~~~~~

プロジェクト追加後のフッテージの種類を表します。

.. list-table::
   :header-rows: 1
   :widths: 25 10 65

   * - 値
     - 数値
     - 説明
   * - ``None_``
     - 0
     - なし/無効
   * - ``Solid``
     - 1
     - ソリッドカラー
   * - ``Missing``
     - 2
     - ソースファイルが見つからない
   * - ``Placeholder``
     - 3
     - プレースホルダー

InterpretationStyle
~~~~~~~~~~~~~~~~~~~

ファイルインポート時の解釈スタイルを指定します。

.. list-table::
   :header-rows: 1
   :widths: 30 10 60

   * - 値
     - 数値
     - 説明
   * - ``NO_DIALOG_NO_GUESS``
     - 0
     - 保存された解釈設定を使用
   * - ``DIALOG_OK``
     - 1
     - 必要に応じてダイアログを表示
   * - ``NO_DIALOG_GUESS``
     - 2
     - ダイアログなしで推測（デフォルト）

使用例
------

フッテージの作成と追加
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # ファイルからフッテージを作成
   footage = ae.Footage.from_file("/path/to/video.mp4")
   print(f"is_in_project: {footage.is_in_project}")  # False

   # プロジェクトに追加
   footage.add_to_project()
   print(f"is_in_project: {footage.is_in_project}")  # True

   # 追加後のプロパティにアクセス
   footage.name = "My Video"
   print(f"名前: {footage.name}")
   print(f"サイズ: {footage.width}x{footage.height}")
   print(f"長さ: {footage.duration}秒")

ソリッドの作成と操作
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # 赤いソリッドを作成
   solid = ae.Footage.solid(
       name="Red Solid",
       width=1920,
       height=1080,
       color=(1.0, 0.0, 0.0, 1.0)
   )

   # プロジェクトに追加
   solid.add_to_project()

   # 色を変更
   solid.set_solid_color((0.0, 0.0, 1.0, 1.0))  # 青に変更

   # サイズを変更
   solid.set_solid_dimensions(3840, 2160)

解釈ルールの設定
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   footage = ae.Footage.from_file("/path/to/video.mp4")
   footage.add_to_project()

   # 解釈ルールを取得
   interp = footage.interpretation
   print(f"アルファモード: {interp.get('alpha_mode')}")
   print(f"FPS: {interp.get('native_fps')}")

   # 解釈ルールを設定
   footage.set_interpretation(
       alpha_mode="premultiplied",
       fps=29.97
   )

複数ファイルの一括インポート
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import os

   def import_folder(folder_path, extensions=None):
       """フォルダ内のメディアファイルをインポート"""
       if extensions is None:
           extensions = ['.mp4', '.mov', '.avi', '.jpg', '.png', '.psd']

       imported = []

       for filename in os.listdir(folder_path):
           ext = os.path.splitext(filename)[1].lower()
           if ext in extensions:
               filepath = os.path.join(folder_path, filename)

               footage = ae.Footage.from_file(filepath)
               if footage.valid:
                   footage.add_to_project()
                   print(f"Imported: {footage.name}")
                   imported.append(footage)

       return imported

   # 使用例
   footage_list = import_folder("C:/media/assets")
   print(f"Imported {len(footage_list)} files")

カラーパレットからソリッド作成
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def create_color_palette(colors, width=1920, height=1080):
       """カラーパレットからソリッドを作成"""
       solids = []

       for name, color in colors.items():
           # RGBからRGBAに変換
           if len(color) == 3:
               color = (*color, 1.0)

           solid = ae.Footage.solid(
               name=f"Solid - {name}",
               width=width,
               height=height,
               color=color
           )
           solid.add_to_project()
           solids.append(solid)

       return solids

   # 使用例
   palette = {
       "Primary Red": (0.9, 0.1, 0.1),
       "Primary Blue": (0.1, 0.1, 0.9),
       "White": (1.0, 1.0, 1.0),
       "Black": (0.0, 0.0, 0.0),
   }
   create_color_palette(palette)

プロジェクト内のフッテージ一覧
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def list_all_footage():
       """プロジェクト内のすべてのフッテージを一覧"""
       project = ae.Project.get_current()

       for item in project.items:
           if hasattr(item, 'footage_type'):
               print(f"名前: {item.name}")
               print(f"  タイプ: {item.footage_type}")
               print(f"  パス: {item.path}")
               print(f"  サイズ: {item.width}x{item.height}")
               print()

   list_all_footage()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **状態の確認**

   ``is_in_project`` プロパティで現在の状態を確認してください。

   .. code-block:: python

      if footage.is_in_project:
          # 追加後の操作
          footage.name = "New Name"
      else:
          # 追加前の操作
          footage.add_to_project()

2. **追加前/追加後の制限**

   追加前専用のプロパティを追加後にアクセスするとエラーになります。
   追加後専用のメソッドを追加前に呼び出すとエラーになります。

   .. code-block:: python

      footage = ae.Footage.from_file("/path/to/video.mp4")

      # 追加前: signatureは使用可能
      print(footage.signature)

      # 追加前: nameはエラー
      # print(footage.name)  # RuntimeError

      footage.add_to_project()

      # 追加後: nameは使用可能
      print(footage.name)

3. **有効性の確認**

   操作前に ``valid`` プロパティを確認してください。

   .. code-block:: python

      if footage.valid:
          footage.add_to_project()

ベストプラクティス
~~~~~~~~~~~~~~~~~~

エラーハンドリング
^^^^^^^^^^^^^^^^^^

ファイル操作は失敗する可能性があるため、エラーハンドリングを実装してください。

.. code-block:: python

   import os

   file_path = "/path/to/video.mp4"
   if os.path.exists(file_path):
       footage = ae.Footage.from_file(file_path)
       if footage.valid:
           footage.add_to_project()
           print(f"インポート成功: {footage.name}")

ソリッドの色指定
^^^^^^^^^^^^^^^^

色はRGBA形式（各値 0.0〜1.0）で指定します。

.. code-block:: python

   # 完全な赤（不透明）
   red = (1.0, 0.0, 0.0, 1.0)

   # 半透明の青
   blue_transparent = (0.0, 0.0, 1.0, 0.5)

   # グレー
   gray = (0.5, 0.5, 0.5, 1.0)

後方互換性
~~~~~~~~~~

``FootageItem`` は ``Footage`` のエイリアスとして残っています：

.. code-block:: python

   import ae

   # 両方とも同じクラスを参照
   print(ae.FootageItem is ae.Footage)  # True

関連項目
--------

- :class:`Item` - プロジェクトアイテム基底クラス
- :class:`Project` - プロジェクト
- :class:`Comp` - コンポジション
- :doc:`/api/low-level/AEGP_FootageSuite5` - 低レベルAPI
