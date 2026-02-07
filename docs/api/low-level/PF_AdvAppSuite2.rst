PF_AdvAppSuite2
===============

アプリケーションレベルのユーティリティ関数を提供するスイートです。
ウィンドウのリフレッシュ、プロジェクトの保存、情報パネルへのテキスト表示などの機能を含みます。

.. note::
   このスイートはPF（プラグインエフェクト）スイートですが、AEGP文脈からも使用可能です。

ウィンドウ・プロジェクト管理
----------------------------

.. function:: PF_RefreshAllWindows() -> None

   すべてのウィンドウ（コンプビューア含む）を強制リフレッシュします。

   プロパティ変更後にUIを更新するための主要な関数です。

   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

   .. code-block:: python

      import ae.sdk as sdk

      # プロパティ変更後にリフレッシュ
      sdk.PF_RefreshAllWindows()

   .. tip::
      高レベルAPIの :func:`ae.refresh` を使用することを推奨します。

.. function:: PF_SetProjectDirty() -> None

   プロジェクトを「未保存の変更あり」としてマークします。

   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

.. function:: PF_SaveProject() -> None

   現在のプロジェクトを保存します。

   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

.. function:: PF_ForceForeground() -> None

   After Effectsをフォアグラウンドに移動します。

   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

.. function:: PF_SaveBackgroundState() -> None

   バックグラウンド状態を保存します。

   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

.. function:: PF_RestoreBackgroundState() -> None

   バックグラウンド状態を復元します。

   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

情報パネル表示
--------------

.. function:: PF_InfoDrawText(line1: str, line2: str) -> None

   情報パネルにテキストを2行表示します。

   :param line1: 1行目のテキスト
   :param line2: 2行目のテキスト
   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

.. function:: PF_InfoDrawText3(line1: str, line2: str, line3: str) -> None

   情報パネルにテキストを3行表示します。

   :param line1: 1行目のテキスト
   :param line2: 2行目のテキスト
   :param line3: 3行目のテキスト
   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

.. function:: PF_InfoDrawText3Plus(line1: str, line2_jr: str, line2_jl: str, line3_jr: str, line3_jl: str) -> None

   情報パネルにフォーマット付きテキストを表示します（右寄せ・左寄せ対応）。

   :param line1: 1行目のテキスト
   :param line2_jr: 2行目右寄せテキスト
   :param line2_jl: 2行目左寄せテキスト
   :param line3_jr: 3行目右寄せテキスト
   :param line3_jl: 3行目左寄せテキスト
   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

.. function:: PF_AppendInfoText(text: str) -> None

   情報パネルにテキストを追加します。

   :param text: 追加するテキスト
   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

.. function:: PF_InfoDrawColor(red: int, green: int, blue: int, alpha: int = 255) -> None

   情報パネルにカラーを表示します。

   :param red: 赤チャンネル（0-255）
   :param green: 緑チャンネル（0-255）
   :param blue: 青チャンネル（0-255）
   :param alpha: アルファチャンネル（0-255、デフォルト: 255）
   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

関連項目
--------

- :func:`ae.refresh` - 高レベルリフレッシュAPI
- :func:`ae.set_project_dirty` - 高レベルプロジェクトダーティ設定
- :doc:`PF_AdvItemSuite1` - アクティブアイテム操作
