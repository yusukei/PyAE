AEGP_MathSuite1
===============

.. currentmodule:: ae.sdk

AEGP_MathSuite1は、行列演算とベクトル変換を行うためのSDK APIです。3D空間での変換、回転、スケーリングなど、幾何学的な計算を効率的に実行できます。

概要
----

**実装状況**: 5/5関数実装 ✅ + 8個のヘルパー関数

AEGP_MathSuite1は以下の機能を提供します:

- 4×4行列の演算（単位行列生成、行列乗算）
- 3×3行列と4×4行列の相互変換
- 行列の分解（位置、スケール、シアー、回転の抽出）
- 変換行列の生成ヘルパー関数（平行移動、回転、スケール）

基本概念
--------

行列の表現形式
~~~~~~~~~~~~~~

PyAEでは、行列は **行優先（row-major）順序** のフラットなリストとして表現されます。

**4×4行列の例**:

.. code-block:: python

   # 4×4単位行列
   matrix4 = [
       1.0, 0.0, 0.0, 0.0,  # 第1行
       0.0, 1.0, 0.0, 0.0,  # 第2行
       0.0, 0.0, 1.0, 0.0,  # 第3行
       0.0, 0.0, 0.0, 1.0   # 第4行
   ]
   # 全16要素をフラットリストで表現

**3×3行列の例**:

.. code-block:: python

   # 3×3単位行列
   matrix3 = [
       1.0, 0.0, 0.0,  # 第1行
       0.0, 1.0, 0.0,  # 第2行
       0.0, 0.0, 1.0   # 第3行
   ]
   # 全9要素をフラットリストで表現

.. important::
   - 4×4行列は **16要素のリスト**
   - 3×3行列は **9要素のリスト**
   - インデックス ``[row * columns + col]`` でアクセス
   - 要素は ``float`` 型（倍精度）

行列の用途
~~~~~~~~~~

After Effectsでは、3D空間での変換を行列で表現します:

**変換の種類**:

.. list-table::
   :header-rows: 1

   * - 変換
     - 用途
     - 行列サイズ
   * - 平行移動
     - レイヤーの位置変更
     - 4×4
   * - 回転
     - レイヤーの回転
     - 4×4 または 3×3
   * - スケール
     - レイヤーのサイズ変更
     - 4×4 または 3×3
   * - シアー
     - レイヤーの歪み
     - 4×4
   * - 複合変換
     - 複数の変換を組み合わせ
     - 4×4

.. tip::
   複数の変換を適用する場合、行列の乗算によって1つの変換行列にまとめることができます。
   これにより、計算回数を減らし、パフォーマンスを向上できます。

座標系とベクトル
~~~~~~~~~~~~~~~~

**3D座標系**:

- **X軸**: 左右（右が正）
- **Y軸**: 上下（上が正）
- **Z軸**: 奥行き（手前が正）

**ベクトル表現**:

.. code-block:: python

   # 3Dベクトルはタプルで表現
   position = (100.0, 200.0, 0.0)  # (x, y, z)
   scale = (1.5, 1.5, 1.0)         # (sx, sy, sz)
   rotation = (0.0, 0.0, 45.0)     # (rx, ry, rz) 度数法

.. note::
   ``AEGP_MatrixDecompose4`` から返される回転値は **ラジアン** です。
   度数法に変換する場合は ``degrees = radians * (180 / π)`` を使用してください。

API リファレンス
----------------

行列生成
~~~~~~~~

.. function:: AEGP_IdentityMatrix4() -> list[float]

   4×4単位行列を生成します。

   :return: 16要素のリスト（4×4単位行列、行優先順序）
   :rtype: list[float]

   **単位行列**:

   .. code-block:: text

      [ 1  0  0  0 ]
      [ 0  1  0  0 ]
      [ 0  0  1  0 ]
      [ 0  0  0  1 ]

   **例**:

   .. code-block:: python

      identity = ae.sdk.AEGP_IdentityMatrix4()
      print(identity)
      # [1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0]

行列演算
~~~~~~~~

.. function:: AEGP_MultiplyMatrix4(matA: list[float], matB: list[float]) -> list[float]

   2つの4×4行列を乗算します。

   :param matA: 左辺の4×4行列（16要素のリスト）
   :type matA: list[float]
   :param matB: 右辺の4×4行列（16要素のリスト）
   :type matB: list[float]
   :return: 結果の4×4行列（16要素のリスト）
   :rtype: list[float]
   :raises ValueError: 行列のサイズが16要素でない場合

   .. important::
      行列の乗算は **非可換** です。``A × B ≠ B × A`` であることに注意してください。
      変換の適用順序は右から左です: ``result = A × B`` は「B変換を適用してからA変換を適用」となります。

   **例**:

   .. code-block:: python

      # 平行移動行列とスケール行列を乗算
      translate = ae.sdk.CreateTranslationMatrix4((100, 200, 0))
      scale = ae.sdk.CreateScaleMatrix4((2.0, 2.0, 1.0))

      # スケールしてから平行移動
      result = ae.sdk.AEGP_MultiplyMatrix4(translate, scale)

.. function:: AEGP_MultiplyMatrix4by3(matA: list[float], matB: list[float]) -> list[float]

   4×4行列と3×3行列を乗算します。

   :param matA: 左辺の4×4行列（16要素のリスト）
   :type matA: list[float]
   :param matB: 右辺の3×3行列（9要素のリスト）
   :type matB: list[float]
   :return: 結果の4×4行列（16要素のリスト）
   :rtype: list[float]
   :raises ValueError: 行列のサイズが不正な場合（matA: 16要素、matB: 9要素が必須）

   .. note::
      3×3行列は内部で4×4行列に変換されてから乗算されます。
      変換時、右端と下端の要素は0、右下隅は1が設定されます。

   **例**:

   .. code-block:: python

      mat4 = ae.sdk.AEGP_IdentityMatrix4()
      mat3 = ae.sdk.CreateRotationMatrix3(3.14159 / 4)  # 45度回転

      result = ae.sdk.AEGP_MultiplyMatrix4by3(mat4, mat3)

行列変換
~~~~~~~~

.. function:: AEGP_Matrix3ToMatrix4(mat3: list[float]) -> list[float]

   3×3行列を4×4行列に変換します。

   :param mat3: 3×3行列（9要素のリスト）
   :type mat3: list[float]
   :return: 4×4行列（16要素のリスト）
   :rtype: list[float]
   :raises ValueError: 行列のサイズが9要素でない場合

   **変換ルール**:

   .. code-block:: text

      入力 (3×3):           出力 (4×4):
      [ a  b  c ]          [ a  b  c  0 ]
      [ d  e  f ]    =>    [ d  e  f  0 ]
      [ g  h  i ]          [ g  h  i  0 ]
                           [ 0  0  0  1 ]

   **例**:

   .. code-block:: python

      mat3 = ae.sdk.CreateRotationMatrix3(1.57)  # 90度回転
      mat4 = ae.sdk.AEGP_Matrix3ToMatrix4(mat3)

行列分解
~~~~~~~~

.. function:: AEGP_MatrixDecompose4(mat4: list[float]) -> dict

   4×4変換行列を位置、スケール、シアー、回転の成分に分解します。

   :param mat4: 4×4行列（16要素のリスト）
   :type mat4: list[float]
   :return: 分解結果の辞書
   :rtype: dict
   :raises ValueError: 行列のサイズが16要素でない場合

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``position``
        - tuple[float, float, float]
        - 平行移動成分 (x, y, z)
      * - ``scale``
        - tuple[float, float, float]
        - スケール成分 (sx, sy, sz)
      * - ``shear``
        - tuple[float, float, float]
        - シアー成分 (xy, xz, yz)
      * - ``rotation``
        - tuple[float, float, float]
        - 回転成分 (rx, ry, rz) **ラジアン**

   .. warning::
      回転値は **ラジアン** で返されます。度数法に変換する場合:

      .. code-block:: python

         import math
         degrees = math.degrees(radians)  # または radians * (180 / math.pi)

   **例**:

   .. code-block:: python

      import math

      # 複合変換行列を作成
      matrix = ae.sdk.AEGP_IdentityMatrix4()

      # 行列を分解
      decomposed = ae.sdk.AEGP_MatrixDecompose4(matrix)

      pos_x, pos_y, pos_z = decomposed['position']
      scale_x, scale_y, scale_z = decomposed['scale']
      rot_x, rot_y, rot_z = decomposed['rotation']

      # 回転を度数法に変換
      rot_x_deg = math.degrees(rot_x)
      rot_y_deg = math.degrees(rot_y)
      rot_z_deg = math.degrees(rot_z)

      print(f"位置: ({pos_x}, {pos_y}, {pos_z})")
      print(f"スケール: ({scale_x}, {scale_y}, {scale_z})")
      print(f"回転（度）: ({rot_x_deg}, {rot_y_deg}, {rot_z_deg})")

ヘルパー関数
------------

PyAEは、よく使用される変換行列を生成するヘルパー関数を提供しています。
これらはPython側で実装されており、直接使用できます。

4×4行列の生成
~~~~~~~~~~~~~~

.. function:: CreateTranslationMatrix4(translation: tuple) -> list[float]

   4×4平行移動行列を生成します。

   :param translation: (tx, ty, tz) のタプル
   :type translation: tuple[float, float, float]
   :return: 16要素のリスト（4×4平行移動行列）
   :rtype: list[float]

   **例**:

   .. code-block:: python

      # (100, 200, 0) に移動
      translate = ae.sdk.CreateTranslationMatrix4((100, 200, 0))

.. function:: CreateScaleMatrix4(scale: tuple) -> list[float]

   4×4スケール行列を生成します。

   :param scale: (sx, sy, sz) のタプル
   :type scale: tuple[float, float, float]
   :return: 16要素のリスト（4×4スケール行列）
   :rtype: list[float]

   **例**:

   .. code-block:: python

      # 2倍に拡大（Z軸は変更なし）
      scale = ae.sdk.CreateScaleMatrix4((2.0, 2.0, 1.0))

.. function:: CreateRotationXMatrix4(angle: float) -> list[float]

   X軸周りの回転行列を生成します。

   :param angle: 回転角度（**ラジアン**）
   :type angle: float
   :return: 16要素のリスト（4×4回転行列）
   :rtype: list[float]

   **例**:

   .. code-block:: python

      import math

      # X軸周りに45度回転
      rot_x = ae.sdk.CreateRotationXMatrix4(math.radians(45))

.. function:: CreateRotationYMatrix4(angle: float) -> list[float]

   Y軸周りの回転行列を生成します。

   :param angle: 回転角度（**ラジアン**）
   :type angle: float
   :return: 16要素のリスト（4×4回転行列）
   :rtype: list[float]

   **例**:

   .. code-block:: python

      import math

      # Y軸周りに90度回転
      rot_y = ae.sdk.CreateRotationYMatrix4(math.radians(90))

.. function:: CreateRotationZMatrix4(angle: float) -> list[float]

   Z軸周りの回転行列を生成します。

   :param angle: 回転角度（**ラジアン**）
   :type angle: float
   :return: 16要素のリスト（4×4回転行列）
   :rtype: list[float]

   **例**:

   .. code-block:: python

      import math

      # Z軸周りに180度回転
      rot_z = ae.sdk.CreateRotationZMatrix4(math.radians(180))

3×3行列の生成
~~~~~~~~~~~~~~

.. function:: CreateIdentityMatrix3() -> list[float]

   3×3単位行列を生成します。

   :return: 9要素のリスト（3×3単位行列）
   :rtype: list[float]

   **例**:

   .. code-block:: python

      identity3 = ae.sdk.CreateIdentityMatrix3()

.. function:: CreateScaleMatrix3(scale: tuple) -> list[float]

   3×3スケール行列を生成します（2D）。

   :param scale: (sx, sy) のタプル
   :type scale: tuple[float, float]
   :return: 9要素のリスト（3×3スケール行列）
   :rtype: list[float]

   **例**:

   .. code-block:: python

      # 横に2倍、縦に1.5倍
      scale3 = ae.sdk.CreateScaleMatrix3((2.0, 1.5))

.. function:: CreateRotationMatrix3(angle: float) -> list[float]

   3×3回転行列を生成します（2D、Z軸周り）。

   :param angle: 回転角度（**ラジアン**）
   :type angle: float
   :return: 9要素のリスト（3×3回転行列）
   :rtype: list[float]

   **例**:

   .. code-block:: python

      import math

      # 45度回転
      rot3 = ae.sdk.CreateRotationMatrix3(math.radians(45))

.. function:: CreateTranslationMatrix3(translation: tuple) -> list[float]

   3×3平行移動行列を生成します（2D、同次座標系）。

   :param translation: (tx, ty) のタプル
   :type translation: tuple[float, float]
   :return: 9要素のリスト（3×3平行移動行列）
   :rtype: list[float]

   .. note::
      3×3行列での平行移動は **同次座標系** を使用します。
      点 ``(x, y)`` は ``(x, y, 1)`` として扱われます。

   **例**:

   .. code-block:: python

      # (50, 100) に移動
      translate3 = ae.sdk.CreateTranslationMatrix3((50, 100))

使用例
------

基本的な変換行列の作成
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import math

   def create_transform_matrix():
       """複合変換行列を作成"""

       # 1. 各変換行列を作成
       translate = ae.sdk.CreateTranslationMatrix4((100, 200, 0))
       scale = ae.sdk.CreateScaleMatrix4((2.0, 2.0, 1.0))
       rotate = ae.sdk.CreateRotationZMatrix4(math.radians(45))

       # 2. 行列を合成（右から左に適用: スケール → 回転 → 平行移動）
       temp = ae.sdk.AEGP_MultiplyMatrix4(rotate, scale)
       final_matrix = ae.sdk.AEGP_MultiplyMatrix4(translate, temp)

       # 3. 結果を分解して確認
       decomposed = ae.sdk.AEGP_MatrixDecompose4(final_matrix)

       pos = decomposed['position']
       scl = decomposed['scale']
       rot = decomposed['rotation']

       print(f"位置: {pos}")
       print(f"スケール: {scl}")
       print(f"回転（度）: {tuple(math.degrees(r) for r in rot)}")

       return final_matrix

   # 実行
   matrix = create_transform_matrix()

レイヤー変換の取得と解析
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import math

   def analyze_layer_transform(layer):
       """レイヤーの変換行列を解析"""

       # レイヤーの変換行列を取得（この関数は例示用）
       # 実際のAPI関数は別途実装が必要
       # matrix = layer.get_transform_matrix()

       # デモ用の行列を作成
       translate = ae.sdk.CreateTranslationMatrix4((500, 300, 0))
       scale = ae.sdk.CreateScaleMatrix4((1.5, 1.5, 1.0))
       rotate = ae.sdk.CreateRotationZMatrix4(math.radians(30))

       temp = ae.sdk.AEGP_MultiplyMatrix4(rotate, scale)
       matrix = ae.sdk.AEGP_MultiplyMatrix4(translate, temp)

       # 行列を分解
       decomposed = ae.sdk.AEGP_MatrixDecompose4(matrix)

       # 結果を表示
       print("=" * 50)
       print("レイヤー変換の解析")
       print("=" * 50)

       pos_x, pos_y, pos_z = decomposed['position']
       print(f"\n位置:")
       print(f"  X: {pos_x:.2f}")
       print(f"  Y: {pos_y:.2f}")
       print(f"  Z: {pos_z:.2f}")

       scale_x, scale_y, scale_z = decomposed['scale']
       print(f"\nスケール:")
       print(f"  X: {scale_x:.2f}x")
       print(f"  Y: {scale_y:.2f}x")
       print(f"  Z: {scale_z:.2f}x")

       rot_x, rot_y, rot_z = decomposed['rotation']
       print(f"\n回転:")
       print(f"  X: {math.degrees(rot_x):.2f}°")
       print(f"  Y: {math.degrees(rot_y):.2f}°")
       print(f"  Z: {math.degrees(rot_z):.2f}°")

       shear_xy, shear_xz, shear_yz = decomposed['shear']
       print(f"\nシアー:")
       print(f"  XY: {shear_xy:.4f}")
       print(f"  XZ: {shear_xz:.4f}")
       print(f"  YZ: {shear_yz:.4f}")

       print("=" * 50)

   # 実行
   analyze_layer_transform(None)

親子関係の変換行列計算
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import math

   def calculate_world_transform(parent_matrix, child_matrix):
       """親レイヤーの変換を考慮した子レイヤーのワールド変換を計算"""

       # 親の変換 × 子の変換 = ワールド変換
       world_matrix = ae.sdk.AEGP_MultiplyMatrix4(parent_matrix, child_matrix)

       # ワールド座標での位置・回転・スケールを取得
       decomposed = ae.sdk.AEGP_MatrixDecompose4(world_matrix)

       return world_matrix, decomposed

   # 例: 親レイヤーの変換
   parent_translate = ae.sdk.CreateTranslationMatrix4((200, 100, 0))
   parent_rotate = ae.sdk.CreateRotationZMatrix4(math.radians(45))
   parent_matrix = ae.sdk.AEGP_MultiplyMatrix4(parent_translate, parent_rotate)

   # 例: 子レイヤーの変換（親からの相対位置）
   child_translate = ae.sdk.CreateTranslationMatrix4((100, 0, 0))
   child_scale = ae.sdk.CreateScaleMatrix4((0.5, 0.5, 1.0))
   child_matrix = ae.sdk.AEGP_MultiplyMatrix4(child_translate, child_scale)

   # ワールド変換を計算
   world_matrix, world_transform = calculate_world_transform(parent_matrix, child_matrix)

   # 結果を表示
   pos = world_transform['position']
   print(f"ワールド座標での位置: ({pos[0]:.2f}, {pos[1]:.2f}, {pos[2]:.2f})")

行列の補間（アニメーション用）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import math

   def interpolate_matrices(mat_a, mat_b, t):
       """2つの行列間を線形補間（簡易版）"""

       # 各行列を分解
       decomp_a = ae.sdk.AEGP_MatrixDecompose4(mat_a)
       decomp_b = ae.sdk.AEGP_MatrixDecompose4(mat_b)

       # 各成分を線形補間
       def lerp_vec3(a, b, t):
           return tuple(a[i] + (b[i] - a[i]) * t for i in range(3))

       pos = lerp_vec3(decomp_a['position'], decomp_b['position'], t)
       scale = lerp_vec3(decomp_a['scale'], decomp_b['scale'], t)
       rotation = lerp_vec3(decomp_a['rotation'], decomp_b['rotation'], t)

       # 補間した値から行列を再構築
       translate = ae.sdk.CreateTranslationMatrix4(*pos)
       scale_mat = ae.sdk.CreateScaleMatrix4(*scale)

       # 回転行列を作成（XYZ順）
       rot_x = ae.sdk.CreateRotationXMatrix4(rotation[0])
       rot_y = ae.sdk.CreateRotationYMatrix4(rotation[1])
       rot_z = ae.sdk.CreateRotationZMatrix4(rotation[2])

       # 合成: スケール → 回転(Z) → 回転(Y) → 回転(X) → 平行移動
       result = scale_mat
       result = ae.sdk.AEGP_MultiplyMatrix4(rot_z, result)
       result = ae.sdk.AEGP_MultiplyMatrix4(rot_y, result)
       result = ae.sdk.AEGP_MultiplyMatrix4(rot_x, result)
       result = ae.sdk.AEGP_MultiplyMatrix4(translate, result)

       return result

   # 開始と終了の変換行列
   start_matrix = ae.sdk.CreateTranslationMatrix4((0, 0, 0))
   end_matrix = ae.sdk.CreateTranslationMatrix4((500, 300, 0))

   # 中間フレーム（50%）の行列を計算
   interpolated = interpolate_matrices(start_matrix, end_matrix, 0.5)

   # 確認
   decomposed = ae.sdk.AEGP_MatrixDecompose4(interpolated)
   pos = decomposed['position']
   print(f"補間位置: ({pos[0]:.2f}, {pos[1]:.2f}, {pos[2]:.2f})")

3×3行列と4×4行列の変換
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import math

   def demo_matrix_conversion():
       """3×3行列と4×4行列の変換デモ"""

       # 1. 3×3回転行列を作成
       mat3 = ae.sdk.CreateRotationMatrix3(math.radians(30))
       print("3×3回転行列:")
       for i in range(3):
           row = mat3[i*3:(i+1)*3]
           print(f"  {row}")

       # 2. 4×4行列に変換
       mat4 = ae.sdk.AEGP_Matrix3ToMatrix4(mat3)
       print("\n4×4行列に変換:")
       for i in range(4):
           row = mat4[i*4:(i+1)*4]
           print(f"  {row}")

       # 3. 4×4行列として処理
       translate = ae.sdk.CreateTranslationMatrix4((100, 50, 0))
       result = ae.sdk.AEGP_MultiplyMatrix4(translate, mat4)

       print("\n平行移動を追加:")
       decomposed = ae.sdk.AEGP_MatrixDecompose4(result)
       print(f"  位置: {decomposed['position']}")
       print(f"  回転（度）: {tuple(math.degrees(r) for r in decomposed['rotation'])}")

   # 実行
   demo_matrix_conversion()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **行列の乗算順序**

   行列の乗算は非可換です。変換の適用順序に注意してください。

   .. code-block:: python

      # 正しい順序: スケール → 回転 → 平行移動
      result = ae.sdk.AEGP_MultiplyMatrix4(translate,
                   ae.sdk.AEGP_MultiplyMatrix4(rotate, scale))

2. **回転の単位**

   .. warning::
      - ヘルパー関数の回転引数は **ラジアン**
      - ``AEGP_MatrixDecompose4`` の戻り値も **ラジアン**
      - 度数法との変換に ``math.radians()`` と ``math.degrees()`` を使用

3. **行列のサイズ検証**

   .. code-block:: python

      if len(matrix) != 16:
          raise ValueError("4×4行列は16要素が必要です")

4. **浮動小数点の精度**

   行列演算では浮動小数点の丸め誤差が累積することがあります。

   .. code-block:: python

      # 完全一致ではなく、許容誤差を使用
      epsilon = 1e-6
      if abs(value - expected) < epsilon:
          # ほぼ等しい

ベストプラクティス
~~~~~~~~~~~~~~~~~~

変換の合成順序
^^^^^^^^^^^^^^

.. code-block:: python

   # 推奨: スケール → 回転 → 平行移動の順
   scale_mat = ae.sdk.CreateScaleMatrix4(sx, sy, sz)
   rotate_mat = ae.sdk.CreateRotationZMatrix4(angle)
   translate_mat = ae.sdk.CreateTranslationMatrix4(tx, ty, tz)

   # 右から左に適用
   temp = ae.sdk.AEGP_MultiplyMatrix4(rotate_mat, scale_mat)
   final = ae.sdk.AEGP_MultiplyMatrix4(translate_mat, temp)

行列の再利用
^^^^^^^^^^^^

.. code-block:: python

   # 単位行列をキャッシュ
   _identity_cache = None

   def get_identity_matrix():
       global _identity_cache
       if _identity_cache is None:
           _identity_cache = ae.sdk.AEGP_IdentityMatrix4()
       return _identity_cache.copy()  # コピーを返す

度数法とラジアンの変換
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   import math

   # 度からラジアンへ
   radians = math.radians(degrees)

   # ラジアンから度へ
   degrees = math.degrees(radians)

行列の視覚化
^^^^^^^^^^^^

.. code-block:: python

   def print_matrix4(matrix, name="Matrix"):
       """4×4行列を見やすく表示"""
       print(f"{name}:")
       for i in range(4):
           row = matrix[i*4:(i+1)*4]
           print(f"  [{row[0]:8.4f} {row[1]:8.4f} {row[2]:8.4f} {row[3]:8.4f}]")

パフォーマンスの最適化
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 複数の変換を事前に1つの行列にまとめる
   transform = ae.sdk.AEGP_MultiplyMatrix4(
       ae.sdk.AEGP_MultiplyMatrix4(translate, rotate),
       scale
   )

   # この変換を多くのポイントに適用
   # (実際の適用処理は別のAPIで実行)

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       result = ae.sdk.AEGP_MultiplyMatrix4(matA, matB)
   except ValueError as e:
       print(f"行列のサイズエラー: {e}")
   except RuntimeError as e:
       print(f"API呼び出しエラー: {e}")

よくある間違い
~~~~~~~~~~~~~~

❌ **間違い1: 行列サイズの不一致**

.. code-block:: python

   # 間違い: 15要素しかない
   matrix = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0]
   result = ae.sdk.AEGP_MatrixDecompose4(matrix)  # エラー!

✅ **正しい実装**:

.. code-block:: python

   # 正しい: 16要素
   matrix = ae.sdk.AEGP_IdentityMatrix4()
   result = ae.sdk.AEGP_MatrixDecompose4(matrix)

❌ **間違い2: 度数法とラジアンの混同**

.. code-block:: python

   # 間違い: 度数法を直接渡す
   rotate = ae.sdk.CreateRotationZMatrix4(45)  # 45度ではなく45ラジアン!

✅ **正しい実装**:

.. code-block:: python

   import math

   # 正しい: ラジアンに変換
   rotate = ae.sdk.CreateRotationZMatrix4(math.radians(45))

❌ **間違い3: 行列乗算の順序**

.. code-block:: python

   # 間違い: スケールが平行移動に影響してしまう
   result = ae.sdk.AEGP_MultiplyMatrix4(scale, translate)

✅ **正しい実装**:

.. code-block:: python

   # 正しい: 平行移動 × スケール
   result = ae.sdk.AEGP_MultiplyMatrix4(translate, scale)

関連項目
--------

- :doc:`AEGP_LayerSuite9` - レイヤーの変換プロパティ取得
- :doc:`AEGP_StreamSuite6` - プロパティストリームの操作
- :doc:`index` - 低レベルAPI概要

参考資料
--------

**行列とベクトルの基礎**:

- `線形代数の基本 <https://ja.wikipedia.org/wiki/%E7%B7%9A%E5%BD%A2%E4%BB%A3%E6%95%B0%E5%AD%A6>`_
- `変換行列 <https://ja.wikipedia.org/wiki/%E5%A4%89%E6%8F%9B%E8%A1%8C%E5%88%97>`_
- `同次座標系 <https://ja.wikipedia.org/wiki/%E5%90%8C%E6%AC%A1%E5%BA%A7%E6%A8%99>`_

**After Effects座標系**:

- After Effects Help - 3D layers
- After Effects SDK Guide - Math Utilities
