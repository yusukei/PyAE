AEGP_MemorySuite1
==================

.. currentmodule:: ae.sdk

AEGP_MemorySuite1は、After Effects SDKにおけるメモリ管理を行うための低レベルAPIです。

概要
----

**実装状況**: 8/8関数実装、テスト済み ✅

AEGP_MemorySuite1は以下の機能を提供します:

- メモリハンドルの割り当てと解放
- メモリハンドルのロック/アンロック
- メモリサイズの取得とリサイズ
- メモリ統計情報の取得
- メモリレポーティングの制御

.. warning::
   これは低レベルなメモリ操作APIです。通常のPythonスクリプトでは、``ae.get_memory_stats()`` や ``ae.enable_memory_reporting()`` などの高レベルAPIを使用してください。

   メモリハンドルの直接操作は、特別な理由がある場合のみ使用してください。

基本概念
--------

メモリハンドル (AEGP_MemHandle)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Effectsのメモリ管理システムで使用される、メモリブロックへの参照。PyAEでは整数値 (``int``) として扱われます。

.. important::
   - メモリハンドルは ``AEGP_NewMemHandle()`` で割り当て、``AEGP_FreeMemHandle()`` で必ず解放する必要があります
   - メモリリークを防ぐため、try-finallyブロックを使用して確実に解放してください
   - メモリハンドルの使用後は、必ずアンロックして解放してください

ロック機構
~~~~~~~~~~

メモリハンドルにアクセスするには、以下の手順が必要です:

1. **Lock** - ``AEGP_LockMemHandle()`` でメモリをロックし、ポインタを取得
2. **Access** - メモリ内容にアクセス（読み書き）
3. **Unlock** - ``AEGP_UnlockMemHandle()`` でメモリをアンロック

.. note::
   ロックは入れ子可能です。ロック回数とアンロック回数は必ず一致させてください。

メモリフラグ
~~~~~~~~~~~~

メモリ割り当て時に以下のフラグを指定できます:

.. list-table::
   :header-rows: 1

   * - フラグ
     - 値
     - 説明
   * - ``AEGP_MemFlag_NONE``
     - 0
     - フラグなし（デフォルト）
   * - ``AEGP_MemFlag_CLEAR``
     - 1
     - メモリをゼロクリアして割り当て
   * - ``AEGP_MemFlag_QUIET``
     - 2
     - エラーメッセージを抑制

API リファレンス
----------------

メモリハンドルの割り当てと解放
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_NewMemHandle(what: str, size: int, flags: int = 0) -> int

   新しいメモリハンドルを割り当てます。

   :param what: デバッグ用の説明文字列（メモリリーク調査時に使用）
   :type what: str
   :param size: 割り当てるサイズ（バイト単位、正の整数）
   :type size: int
   :param flags: メモリフラグ（``AEGP_MemFlag_NONE``, ``AEGP_MemFlag_CLEAR``, ``AEGP_MemFlag_QUIET``）
   :type flags: int
   :return: メモリハンドル（整数値）
   :rtype: int

   :raises ValueError: ``size`` が0以下の場合
   :raises RuntimeError: メモリ割り当てに失敗した場合

   .. warning::
      割り当てたメモリハンドルは、``AEGP_FreeMemHandle()`` で必ず解放してください。
      解放し忘れるとメモリリークが発生します。

   **例**:

   .. code-block:: python

      # 1024バイトのメモリを割り当て（ゼロクリア）
      memH = ae.sdk.AEGP_NewMemHandle("MyData", 1024, ae.sdk.AEGP_MemFlag_CLEAR)

      try:
          # メモリハンドルを使用
          size = ae.sdk.AEGP_GetMemHandleSize(memH)
          print(f"割り当てサイズ: {size} bytes")
      finally:
          # 必ず解放
          ae.sdk.AEGP_FreeMemHandle(memH)

.. function:: AEGP_FreeMemHandle(memH: int) -> None

   メモリハンドルを解放します。

   :param memH: 解放するメモリハンドル（``AEGP_NewMemHandle()`` で取得）
   :type memH: int

   :raises ValueError: ``memH`` が0（NULL）の場合
   :raises RuntimeError: メモリ解放に失敗した場合

   .. important::
      解放後のメモリハンドルは無効になります。再利用しないでください。

   **例**:

   .. code-block:: python

      memH = ae.sdk.AEGP_NewMemHandle("TempBuffer", 512)

      try:
          # メモリハンドル使用
          pass
      finally:
          ae.sdk.AEGP_FreeMemHandle(memH)

メモリハンドルのロック/アンロック
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_LockMemHandle(memH: int) -> int

   メモリハンドルをロックし、メモリポインタを取得します。

   :param memH: ロックするメモリハンドル
   :type memH: int
   :return: メモリポインタ（整数値として返される）
   :rtype: int

   :raises ValueError: ``memH`` が0（NULL）の場合
   :raises RuntimeError: ロックに失敗した場合

   .. note::
      ロックは入れ子可能です。各 ``AEGP_LockMemHandle()`` に対して、必ず ``AEGP_UnlockMemHandle()`` を呼び出してください。

   .. warning::
      Pythonから直接ポインタを操作することは推奨されません。
      この関数は主に内部的な使用や、特殊なケースでのみ使用してください。

   **例**:

   .. code-block:: python

      memH = ae.sdk.AEGP_NewMemHandle("Data", 256)

      try:
          # ロック
          ptr = ae.sdk.AEGP_LockMemHandle(memH)

          try:
              # メモリ操作（Pythonでは直接操作は困難）
              pass
          finally:
              # 必ずアンロック
              ae.sdk.AEGP_UnlockMemHandle(memH)
      finally:
          ae.sdk.AEGP_FreeMemHandle(memH)

.. function:: AEGP_UnlockMemHandle(memH: int) -> None

   メモリハンドルをアンロックします。

   :param memH: アンロックするメモリハンドル
   :type memH: int

   :raises ValueError: ``memH`` が0（NULL）の場合
   :raises RuntimeError: アンロックに失敗した場合

   .. important::
      各 ``AEGP_LockMemHandle()`` 呼び出しに対して、必ず ``AEGP_UnlockMemHandle()`` を呼び出してください。
      アンロック忘れは、メモリリークやクラッシュの原因になります。

   **例**:

   .. code-block:: python

      ptr = ae.sdk.AEGP_LockMemHandle(memH)
      try:
          # メモリアクセス
          pass
      finally:
          ae.sdk.AEGP_UnlockMemHandle(memH)

メモリサイズの取得とリサイズ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetMemHandleSize(memH: int) -> int

   メモリハンドルのサイズを取得します。

   :param memH: サイズを取得するメモリハンドル
   :type memH: int
   :return: メモリサイズ（バイト単位）
   :rtype: int

   :raises ValueError: ``memH`` が0（NULL）の場合
   :raises RuntimeError: サイズ取得に失敗した場合

   **例**:

   .. code-block:: python

      memH = ae.sdk.AEGP_NewMemHandle("Buffer", 1024)

      try:
          size = ae.sdk.AEGP_GetMemHandleSize(memH)
          print(f"メモリサイズ: {size} bytes")
      finally:
          ae.sdk.AEGP_FreeMemHandle(memH)

.. function:: AEGP_ResizeMemHandle(what: str, new_size: int, memH: int) -> None

   メモリハンドルのサイズを変更します。

   :param what: デバッグ用の説明文字列
   :type what: str
   :param new_size: 新しいサイズ（バイト単位、0以上）
   :type new_size: int
   :param memH: リサイズするメモリハンドル
   :type memH: int

   :raises ValueError: ``memH`` が0（NULL）または ``new_size`` が負の場合
   :raises RuntimeError: リサイズに失敗した場合

   .. warning::
      リサイズ時、内部的にメモリが再割り当てされる可能性があります。
      古いポインタは無効になるため、リサイズ後は必ず再度ロックしてください。

   **例**:

   .. code-block:: python

      memH = ae.sdk.AEGP_NewMemHandle("DynamicBuffer", 512)

      try:
          # サイズを2倍に拡張
          new_size = 1024
          ae.sdk.AEGP_ResizeMemHandle("Resize to 1KB", new_size, memH)

          # 新しいサイズを確認
          size = ae.sdk.AEGP_GetMemHandleSize(memH)
          print(f"新しいサイズ: {size} bytes")
      finally:
          ae.sdk.AEGP_FreeMemHandle(memH)

メモリ統計とレポーティング
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. function:: AEGP_GetMemStats() -> dict

   プラグインのメモリ統計情報を取得します。

   :return: メモリ統計情報の辞書
   :rtype: dict

   :raises RuntimeError: 統計情報の取得に失敗した場合

   **戻り値の構造**:

   .. list-table::
      :header-rows: 1

      * - キー
        - 型
        - 説明
      * - ``handle_count``
        - int
        - 現在割り当てられているメモリハンドルの数
      * - ``total_size``
        - int
        - 割り当てられているメモリの合計サイズ（バイト単位）

   .. note::
      高レベルAPIである ``ae.get_memory_stats()`` を使用することを推奨します。

   **例**:

   .. code-block:: python

      stats = ae.sdk.AEGP_GetMemStats()
      print(f"メモリハンドル数: {stats['handle_count']}")
      print(f"合計メモリサイズ: {stats['total_size']} bytes")

.. function:: AEGP_SetMemReportingOn(enable: bool) -> None

   詳細なメモリレポーティングを有効/無効にします。

   :param enable: ``True`` で有効化、``False`` で無効化
   :type enable: bool

   :raises RuntimeError: レポーティング設定に失敗した場合

   .. warning::
      メモリレポーティングを有効にすると、すべてのメモリ割り当て/解放がログに記録されます。
      **パフォーマンスに影響するため、デバッグ時のみ使用してください。**

   .. note::
      高レベルAPIである ``ae.enable_memory_reporting()`` を使用することを推奨します。

   **例**:

   .. code-block:: python

      # メモリレポーティングを有効化（デバッグ用）
      ae.sdk.AEGP_SetMemReportingOn(True)

      try:
          # メモリリークを調査したい処理
          memH = ae.sdk.AEGP_NewMemHandle("DebugTest", 1024)
          ae.sdk.AEGP_FreeMemHandle(memH)
      finally:
          # レポーティングを無効化
          ae.sdk.AEGP_SetMemReportingOn(False)

定数
----

メモリフラグ
~~~~~~~~~~~~

.. data:: AEGP_MemFlag_NONE
   :value: 0

   フラグなし（デフォルト）

.. data:: AEGP_MemFlag_CLEAR
   :value: 1

   メモリをゼロクリアして割り当て

.. data:: AEGP_MemFlag_QUIET
   :value: 2

   エラーメッセージを抑制

使用例
------

基本的なメモリハンドルの使用
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def allocate_and_use_memory():
       """メモリハンドルの基本的な使用例"""
       # メモリを割り当て（1024バイト、ゼロクリア）
       memH = ae.sdk.AEGP_NewMemHandle(
           "MyBuffer",
           1024,
           ae.sdk.AEGP_MemFlag_CLEAR
       )

       try:
           # サイズを確認
           size = ae.sdk.AEGP_GetMemHandleSize(memH)
           print(f"割り当てサイズ: {size} bytes")

           # メモリをロック
           ptr = ae.sdk.AEGP_LockMemHandle(memH)

           try:
               # メモリ操作（Pythonでは制限あり）
               print(f"メモリポインタ: 0x{ptr:X}")
           finally:
               # 必ずアンロック
               ae.sdk.AEGP_UnlockMemHandle(memH)

       finally:
           # 必ずメモリを解放
           ae.sdk.AEGP_FreeMemHandle(memH)

   # 実行
   allocate_and_use_memory()

動的にサイズを変更するバッファ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def dynamic_buffer_example():
       """動的にサイズを変更するメモリバッファの例"""
       # 初期サイズ512バイトで割り当て
       memH = ae.sdk.AEGP_NewMemHandle("DynamicBuffer", 512)

       try:
           print(f"初期サイズ: {ae.sdk.AEGP_GetMemHandleSize(memH)} bytes")

           # サイズを2倍に拡張
           ae.sdk.AEGP_ResizeMemHandle("Expand to 1KB", 1024, memH)
           print(f"拡張後: {ae.sdk.AEGP_GetMemHandleSize(memH)} bytes")

           # さらに拡張
           ae.sdk.AEGP_ResizeMemHandle("Expand to 4KB", 4096, memH)
           print(f"最終サイズ: {ae.sdk.AEGP_GetMemHandleSize(memH)} bytes")

       finally:
           ae.sdk.AEGP_FreeMemHandle(memH)

   # 実行
   dynamic_buffer_example()

メモリリークの検出
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   def detect_memory_leaks():
       """メモリリークを検出する例"""
       # メモリレポーティングを有効化
       ae.sdk.AEGP_SetMemReportingOn(True)

       try:
           # 初期状態の統計を取得
           stats_before = ae.sdk.AEGP_GetMemStats()
           print("実行前:")
           print(f"  ハンドル数: {stats_before['handle_count']}")
           print(f"  メモリサイズ: {stats_before['total_size']} bytes")

           # 複数のメモリハンドルを割り当て・解放
           handles = []
           for i in range(5):
               memH = ae.sdk.AEGP_NewMemHandle(f"Test{i}", 1024)
               handles.append(memH)

           # すべて解放
           for memH in handles:
               ae.sdk.AEGP_FreeMemHandle(memH)

           # 実行後の統計を取得
           stats_after = ae.sdk.AEGP_GetMemStats()
           print("\n実行後:")
           print(f"  ハンドル数: {stats_after['handle_count']}")
           print(f"  メモリサイズ: {stats_after['total_size']} bytes")

           # メモリリークのチェック
           if stats_after['handle_count'] > stats_before['handle_count']:
               print("\n警告: メモリリークの可能性があります！")
           else:
               print("\nOK: メモリリークは検出されませんでした")

       finally:
           # レポーティングを無効化
           ae.sdk.AEGP_SetMemReportingOn(False)

   # 実行
   detect_memory_leaks()

安全なメモリ管理（コンテキストマネージャー風）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   class MemoryHandle:
       """メモリハンドルを安全に管理するラッパークラス"""

       def __init__(self, what, size, flags=0):
           self.what = what
           self.memH = ae.sdk.AEGP_NewMemHandle(what, size, flags)

       def __enter__(self):
           return self.memH

       def __exit__(self, exc_type, exc_val, exc_tb):
           if self.memH:
               ae.sdk.AEGP_FreeMemHandle(self.memH)
               self.memH = None
           return False

       def size(self):
           return ae.sdk.AEGP_GetMemHandleSize(self.memH)

       def resize(self, new_size):
           ae.sdk.AEGP_ResizeMemHandle(self.what, new_size, self.memH)

   def safe_memory_usage():
       """コンテキストマネージャーを使った安全なメモリ管理"""
       # with文で自動的に解放される
       with MemoryHandle("SafeBuffer", 2048, ae.sdk.AEGP_MemFlag_CLEAR) as memH:
           print(f"メモリサイズ: {ae.sdk.AEGP_GetMemHandleSize(memH)} bytes")

           # メモリをロック
           ptr = ae.sdk.AEGP_LockMemHandle(memH)
           try:
               # メモリ操作
               print(f"ポインタ: 0x{ptr:X}")
           finally:
               ae.sdk.AEGP_UnlockMemHandle(memH)

       # with文を抜けた時点で自動的にFreeMemHandleが呼ばれる
       print("メモリは自動的に解放されました")

   # 実行
   safe_memory_usage()

注意事項とベストプラクティス
----------------------------

重要な注意事項
~~~~~~~~~~~~~~

1. **メモリリークの防止**

   割り当てたメモリハンドルは、``AEGP_FreeMemHandle()`` で必ず解放してください。
   try-finallyブロックを使用して、エラー発生時でも確実に解放されるようにしてください。

2. **ロック/アンロックの対応**

   ``AEGP_LockMemHandle()`` と ``AEGP_UnlockMemHandle()`` は必ずペアで呼び出してください。
   アンロック忘れは、メモリリークやクラッシュの原因になります。

3. **リサイズ後のポインタ無効化**

   ``AEGP_ResizeMemHandle()`` 実行後、古いポインタは無効になります。
   リサイズ後は必ず再度ロックして新しいポインタを取得してください。

4. **メモリレポーティングのパフォーマンス影響**

   ``AEGP_SetMemReportingOn(True)`` は、すべてのメモリ操作をログに記録するため、パフォーマンスに大きく影響します。
   デバッグ時のみ使用し、本番環境では無効化してください。

5. **Pythonからの直接ポインタ操作の制限**

   ``AEGP_LockMemHandle()`` で取得したポインタは、Pythonから直接操作することは困難です。
   通常は、C++拡張やctypesなどを使用する必要があります。

ベストプラクティス
~~~~~~~~~~~~~~~~~~

try-finallyによる確実な解放
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   memH = ae.sdk.AEGP_NewMemHandle("Data", 1024)
   try:
       # メモリハンドル使用
       pass
   finally:
       ae.sdk.AEGP_FreeMemHandle(memH)

ロック/アンロックの入れ子
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   memH = ae.sdk.AEGP_NewMemHandle("Data", 512)
   try:
       ptr = ae.sdk.AEGP_LockMemHandle(memH)
       try:
           # メモリアクセス
           pass
       finally:
           ae.sdk.AEGP_UnlockMemHandle(memH)
   finally:
       ae.sdk.AEGP_FreeMemHandle(memH)

高レベルAPIの優先使用
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 推奨: 高レベルAPI
   stats = ae.get_memory_stats()

   # 非推奨: 低レベルAPI（特別な理由がない限り避ける）
   stats = ae.sdk.AEGP_GetMemStats()

メモリリークの定期チェック
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: python

   # 開発中は定期的にメモリ統計を確認
   stats = ae.sdk.AEGP_GetMemStats()
   if stats['handle_count'] > 100:
       print("警告: メモリハンドル数が多すぎます")

エラーハンドリング
^^^^^^^^^^^^^^^^^^

.. code-block:: python

   try:
       memH = ae.sdk.AEGP_NewMemHandle("Buffer", 1024)
   except RuntimeError as e:
       print(f"メモリ割り当て失敗: {e}")
       return

関連項目
--------

- 高レベルメモリAPI: ``ae.get_memory_stats()``, ``ae.enable_memory_reporting()``
- :doc:`index` - 低レベルAPI概要
