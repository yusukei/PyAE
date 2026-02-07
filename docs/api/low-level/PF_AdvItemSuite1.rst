PF_AdvItemSuite1
=================

アクティブアイテムの操作と再評価をトリガーする機能を提供するスイートです。

.. note::
   このスイートはPF（プラグインエフェクト）スイートですが、一部の関数はAEGP文脈からも使用可能です。
   ``PF_MoveTimeStep`` と ``PF_ForceRerender`` は ``PF_InData*`` が必要なため、AEGP文脈からは使用できません。

定数
----

.. data:: PF_Step_FORWARD

   時間ステップの方向: 前方（値: 0）

.. data:: PF_Step_BACKWARD

   時間ステップの方向: 後方（値: 1）

関数
----

.. function:: PF_TouchActiveItem() -> None

   アクティブアイテムを「変更済み」としてマークし、依存するエフェクトやエクスプレッションの再評価をトリガーします。

   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

   .. code-block:: python

      import ae.sdk as sdk

      # プロパティ変更後に再評価をトリガー
      sdk.PF_TouchActiveItem()

   .. tip::
      高レベルAPIの :func:`ae.touch_active_item` を使用することを推奨します。

.. function:: PF_MoveTimeStepActiveItem(time_dir: int, num_steps: int) -> None

   アクティブアイテムの時間ステップを移動します。

   :param time_dir: 方向（``PF_Step_FORWARD`` または ``PF_Step_BACKWARD``）
   :param num_steps: 移動するステップ数
   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

   .. code-block:: python

      import ae.sdk as sdk

      # 1フレーム進める
      sdk.PF_MoveTimeStepActiveItem(sdk.PF_Step_FORWARD, 1)

      # 5フレーム戻る
      sdk.PF_MoveTimeStepActiveItem(sdk.PF_Step_BACKWARD, 5)

.. function:: PF_EffectIsActiveOrEnabled(contextH: int) -> bool

   エフェクトがアクティブまたは有効かどうかを確認します。

   :param contextH: エフェクトコンテキストハンドル
   :returns: エフェクトが有効な場合 ``True``
   :rtype: bool
   :raises RuntimeError: スイートが利用不可または呼び出しに失敗した場合

   .. note::
      この関数は ``PF_ContextH`` が必要です。通常はエフェクトプラグインの文脈でのみ使用されます。

AEGP文脈から使用不可の関数
---------------------------

以下の関数は ``PF_InData*`` / ``PF_EffectWorld*`` が必要なため、
AEGP（Pythonスクリプト）文脈からは使用できません：

- ``PF_MoveTimeStep`` - 特定のエフェクトの時間ステップを移動
- ``PF_ForceRerender`` - エフェクトの再レンダリングを強制

関連項目
--------

- :func:`ae.touch_active_item` - 高レベル再評価トリガーAPI
- :func:`ae.refresh` - 高レベルリフレッシュAPI
- :doc:`PF_AdvAppSuite2` - アプリケーションレベルユーティリティ
