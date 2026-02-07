AEGP_ItemViewSuite1
===================

.. currentmodule:: ae.sdk

AEGP_ItemViewSuite1は、After Effectsのアイテムビュー（プレビュー）の再生時間情報を取得するためのSDK APIです。

概要
----

**実装状況**: 1/1関数実装、1/1関数テスト済み ✅

AEGP_ItemViewSuite1は以下の機能を提供します:

- アイテムビューの再生時間取得
- プレビュー状態の確認

基本概念
--------

アイテムビューハンドル (AEGP_ItemViewP)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

アイテムビュー（Footage, Composition, Layer Sourceのプレビューウィンドウ）を識別するためのハンドル。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - アイテムビューハンドルは、``AEGP_GetItemMRUView`` から取得します
   - アイテムビューが閉じられると、ハンドルは無効になります
   - 各アイテムは複数のビューを持つ可能性があります（最も最近使用されたビューがMRU）

プレビュー時間とデフォルト時間
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

アイテムビューには2種類の時間があります:

1. **プレビュー再生時間** - プレビューが実行中のときの現在再生位置
2. **デフォルトアイテム時間** - プレビューが停止中のときの現在位置

``AEGP_GetItemViewPlaybackTime`` は、プレビュー状態に応じて適切な時間を返します。

API リファレンス
----------------

アイテムビュー時間取得
~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetItemViewPlaybackTime(item_viewP: int) -> dict

   アイテムビューの再生時間を取得します。

   :param item_viewP: アイテムビューハンドル（``AEGP_GetItemMRUView`` から取得）
   :type item_viewP: int
   :return: 時間情報とプレビュー状態を含む辞書
   :rtype: dict
   :raises ValueError: item_viewPがnull（0）の場合
   :raises RuntimeError: ItemView Suiteが利用できない場合、またはAPI呼び出しが失敗した場合

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``is_previewing``
        - bool
        - ``True``: プレビュー再生中、``False``: プレビュー停止中
      * - ``time_seconds``
        - float
        - 現在時間（秒単位）
      * - ``time_value``
        - int
        - A_Time構造体のvalue値（生データ）
      * - ``time_scale``
        - int
        - A_Time構造体のscale値（時間の分解能）

   .. note::
      ``time_seconds`` は ``time_value / time_scale`` で計算されます。
      通常は ``time_seconds`` を使用すれば十分です。

   **例1: 基本的な使い方**

   .. code-block:: python

      import ae

      # アイテムビューハンドルを取得
      item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)

      # 再生時間を取得
      time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)

      print(f"現在時間: {time_info['time_seconds']:.3f}秒")
      print(f"プレビュー中: {time_info['is_previewing']}")

   **例2: プレビュー状態に応じた処理**

   .. code-block:: python

      import ae

      item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
      time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)

      if time_info['is_previewing']:
          print(f"プレビュー再生中 - 再生位置: {time_info['time_seconds']:.2f}秒")
      else:
          print(f"プレビュー停止中 - カレント位置: {time_info['time_seconds']:.2f}秒")

   **例3: 時間の精密な取得（A_Time構造体の使用）**

   .. code-block:: python

      import ae

      item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
      time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)

      # 精密な時間計算（整数演算で丸め誤差なし）
      time_value = time_info['time_value']
      time_scale = time_info['time_scale']
      frame_number = time_value // (time_scale // 30)  # 30fpsと仮定

      print(f"フレーム番号: {frame_number}")
      print(f"正確な時間: {time_value}/{time_scale}秒")

使用例
------

プレビュー進行状況の監視
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import time

   def monitor_preview_playback(itemH, duration_seconds=10):
       """プレビュー再生の進行状況を監視"""
       item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)

       print(f"プレビュー監視開始（{duration_seconds}秒間）")
       start = time.time()

       while time.time() - start < duration_seconds:
           time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)

           status = "▶ 再生中" if time_info['is_previewing'] else "⏸ 停止中"
           print(f"{status} - {time_info['time_seconds']:.3f}秒", end='\r')

           time.sleep(0.1)  # 100ms間隔でチェック

       print("\n監視終了")

   # 実行例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetNextProjItem(projH, 0)  # 最初のアイテム
   monitor_preview_playback(itemH)

フッテージのサムネイル時間取得
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def get_current_thumbnail_time(itemH):
       """フッテージまたはコンポのサムネイル表示時間を取得"""
       try:
           item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
       except:
           print("アイテムビューが開かれていません")
           return None

       time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)

       return {
           'time_seconds': time_info['time_seconds'],
           'is_previewing': time_info['is_previewing'],
           'formatted_time': format_timecode(time_info['time_seconds'])
       }

   def format_timecode(seconds):
       """秒をタイムコード形式に変換"""
       h = int(seconds // 3600)
       m = int((seconds % 3600) // 60)
       s = int(seconds % 60)
       f = int((seconds % 1) * 30)  # 30fps想定
       return f"{h:02d}:{m:02d}:{s:02d}:{f:02d}"

   # 使用例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetNextProjItem(projH, 0)
   thumb_info = get_current_thumbnail_time(itemH)

   if thumb_info:
       print(f"サムネイル時間: {thumb_info['formatted_time']}")
       print(f"再生状態: {thumb_info['is_previewing']}")

プレビュー同期チェック
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def check_preview_sync(comp_itemH, layer_itemH):
       """コンポジションとレイヤーソースのプレビュー時間を比較"""
       # コンポジションのビュー時間を取得
       comp_viewP = ae.sdk.AEGP_GetItemMRUView(comp_itemH)
       comp_time = ae.sdk.AEGP_GetItemViewPlaybackTime(comp_viewP)

       # レイヤーソースのビュー時間を取得
       layer_viewP = ae.sdk.AEGP_GetItemMRUView(layer_itemH)
       layer_time = ae.sdk.AEGP_GetItemViewPlaybackTime(layer_viewP)

       # 同期状態を比較
       time_diff = abs(comp_time['time_seconds'] - layer_time['time_seconds'])
       is_synced = time_diff < 0.001  # 1ms以下の誤差を許容

       print("=" * 60)
       print("プレビュー同期チェック")
       print("-" * 60)
       print(f"コンポジション時間: {comp_time['time_seconds']:.4f}秒")
       print(f"  プレビュー中: {comp_time['is_previewing']}")
       print(f"レイヤーソース時間: {layer_time['time_seconds']:.4f}秒")
       print(f"  プレビュー中: {layer_time['is_previewing']}")
       print(f"時間差: {time_diff:.4f}秒")
       print(f"同期状態: {'✓ 同期' if is_synced else '✗ 非同期'}")
       print("=" * 60)

       return is_synced

   # 使用例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   comp_itemH = ae.sdk.AEGP_GetNextProjItem(projH, 0)
   layer_itemH = ae.sdk.AEGP_GetNextProjItem(projH, comp_itemH)
   check_preview_sync(comp_itemH, layer_itemH)

アイテムビューの時間レンジ分析
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def analyze_item_view_time_range(itemH, num_samples=10):
       """プレビューウィンドウで表示可能な時間レンジを分析"""
       try:
           item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
       except:
           print("アイテムビューが開かれていません")
           return

       # アイテムの詳細情報を取得
       item_name = ae.sdk.AEGP_GetItemName(itemH)
       item_type = ae.sdk.AEGP_GetItemType(itemH)

       # 現在の時間を取得
       time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)

       print("=" * 60)
       print(f"アイテムビュー時間分析: {item_name}")
       print("-" * 60)
       print(f"アイテムタイプ: {item_type}")
       print(f"現在時間: {time_info['time_seconds']:.4f}秒")
       print(f"プレビュー状態: {'再生中' if time_info['is_previewing'] else '停止中'}")
       print(f"時間精度:")
       print(f"  Value: {time_info['time_value']}")
       print(f"  Scale: {time_info['time_scale']}")
       print(f"  精度: {1.0 / time_info['time_scale']:.6f}秒/tick")
       print("=" * 60)

   # 使用例
   projH = ae.sdk.AEGP_GetProjectByIndex(0)
   itemH = ae.sdk.AEGP_GetNextProjItem(projH, 0)
   analyze_item_view_time_range(itemH)

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **アイテムビューハンドルの取得**

   ``AEGP_GetItemViewPlaybackTime`` を使用する前に、必ず ``AEGP_GetItemMRUView`` でアイテムビューハンドルを取得してください。

   .. code-block:: python

      item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
      time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)

2. **ビューが開かれていない場合**

   アイテムにビューが開かれていない場合、``AEGP_GetItemMRUView`` は例外を発生させる可能性があります。

   .. code-block:: python

      try:
          item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
      except:
          print("ビューが開かれていません")

3. **プレビュー状態の判定**

   ``is_previewing`` は現在の再生状態を示します。スクラブ中（手動でタイムインジケータを移動中）は ``False`` になります。

4. **時間精度**

   高精度な時間計算が必要な場合は、``time_value`` と ``time_scale`` を直接使用してください。浮動小数点の ``time_seconds`` は丸め誤差が発生する可能性があります。

   .. code-block:: python

      # 高精度計算
      exact_seconds = time_info['time_value'] / time_info['time_scale']

      # フレーム単位の計算（30fps想定）
      fps = 30
      frame_number = (time_info['time_value'] * fps) // time_info['time_scale']

ベストプラクティス
~~~~~~~~~~~~~~~~~~

ビューの存在確認
^^^^^^^^^^^^^^^^

.. code-block:: python

   def get_item_view_time_safe(itemH):
       """安全にアイテムビューの時間を取得"""
       try:
           item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
           return ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)
       except Exception as e:
           print(f"アイテムビュー時間取得エラー: {e}")
           return None

プレビュー状態の活用
^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)

   if time_info['is_previewing']:
       # プレビュー再生中の処理
       print("プレビュー再生中")
   else:
       # プレビュー停止中の処理
       print("プレビュー停止中")

時間フォーマットの統一
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   def format_time(time_info):
       """時間情報を読みやすい形式に変換"""
       seconds = time_info['time_seconds']
       return f"{seconds:.3f}秒 (Value: {time_info['time_value']}, Scale: {time_info['time_scale']})"

   time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)
   print(format_time(time_info))

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       item_viewP = ae.sdk.AEGP_GetItemMRUView(itemH)
       time_info = ae.sdk.AEGP_GetItemViewPlaybackTime(item_viewP)
   except ValueError as e:
       print(f"無効なハンドル: {e}")
   except RuntimeError as e:
       print(f"API呼び出しエラー: {e}")
   except Exception as e:
       print(f"予期しないエラー: {e}")

パフォーマンス考慮事項
~~~~~~~~~~~~~~~~~~~~~~

1. **頻繁な呼び出し**

   ``AEGP_GetItemViewPlaybackTime`` は軽量な関数ですが、リアルタイム監視では適切な間隔（100ms以上）を設けてください。

2. **複数ビューの監視**

   複数のアイテムビューを同時に監視する場合、各ビューのハンドルをキャッシュしてください。

   .. code-block:: python

      # ビューハンドルをキャッシュ
      view_handles = {
          'comp': ae.sdk.AEGP_GetItemMRUView(comp_itemH),
          'footage': ae.sdk.AEGP_GetItemMRUView(footage_itemH)
      }

      # 繰り返し使用
      comp_time = ae.sdk.AEGP_GetItemViewPlaybackTime(view_handles['comp'])
      footage_time = ae.sdk.AEGP_GetItemViewPlaybackTime(view_handles['footage'])

関連項目
--------

- :doc:`AEGP_ItemSuite9` - アイテム管理（``AEGP_GetItemMRUView`` を含む）
- :doc:`AEGP_CompSuite12` - コンポジション管理
- :doc:`AEGP_FootageSuite5` - フッテージ管理
- :doc:`index` - 低レベルAPI概要
