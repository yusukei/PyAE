低レベルAPI (SDK)
=================

PyAEの低レベルAPIは、Adobe After Effects AEGP SDKへの直接アクセスを提供します。

このAPIは以下の場合に使用します：

- 高レベルAPIでサポートされていない機能へのアクセス
- パフォーマンスが重要な場合
- SDKの詳細な制御が必要な場合

.. warning::
   低レベルAPIは高度なユーザー向けです。不適切な使用はAfter Effectsのクラッシュを引き起こす可能性があります。

主要モジュール
--------------

.. toctree::
   :maxdepth: 2

   sdk
   AEGP_ProjSuite6
   AEGP_CompSuite12
   AEGP_LayerSuite9
   AEGP_EffectSuite5
   AEGP_StreamSuite6
   AEGP_KeyframeSuite5
   AEGP_ItemSuite9
   AEGP_MaskSuite6
   AEGP_FootageSuite5
   AEGP_IOInSuite7
   AEGP_UtilitySuite6
   AEGP_MarkerSuite3
   AEGP_MaskOutlineSuite3
   AEGP_CollectionSuite2
   AEGP_CameraSuite2
   AEGP_LightSuite3
   AEGP_CommandSuite1
   AEGP_TextLayerSuite1
   AEGP_TextDocumentSuite1
   AEGP_DynamicStreamSuite4
   AEGP_SoundDataSuite1
   AEGP_RenderQueueSuite1
   AEGP_RQItemSuite4
   AEGP_OutputModuleSuite4
   AEGP_PersistentDataSuite4
   AEGP_ColorSettingsSuite6
   AEGP_MemorySuite1
   AEGP_WorldSuite3
   AEGP_RenderOptionsSuite4
   AEGP_LayerRenderOptionsSuite2
   AEGP_RenderSuite5
   AEGP_RenderQueueMonitorSuite1
   AEGP_IOOutSuite6
   AEGP_CompositeSuite2
   AEGP_MathSuite1
   AEGP_ItemViewSuite1
   AEGP_IterateSuite2
   AEGP_RenderAsyncManagerSuite1
   PF_AdvAppSuite2
   PF_AdvItemSuite1

モジュール概要
--------------

``ae.sdk`` モジュールは、AEGP SDKへの低レベルアクセスを提供します。
詳細は :doc:`sdk` を参照してください。

使用例
------

基本的な使用方法
~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae
   import ae.sdk as sdk

   # プロジェクト数を取得
   num_projects = sdk.AEGP_GetNumProjects()
   print(f"開いているプロジェクト数: {num_projects}")

   # プロジェクトハンドルを取得
   projH = sdk.AEGP_GetProjectByIndex(0)

   # プロジェクト名を取得
   project_name = sdk.AEGP_GetProjectName(projH)
   print(f"プロジェクト名: {project_name}")

   # ルートフォルダを取得
   rootFolderH = sdk.AEGP_GetProjectRootFolder(projH)
   print(f"ルートフォルダハンドル: {rootFolderH}")

高レベルAPIとの組み合わせ
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import ae

   # 高レベルAPIでレイヤーを取得
   layer = ae.get_active_comp().layers[0]

   # レイヤーハンドルを取得
   layer_h = layer._handle

   # 低レベルAPIで操作
   # （注意: ハンドルの直接操作は慎重に行う）

注意事項
--------

- **ハンドルの寿命管理**: ハンドルは適切に管理する必要があります
- **エラーチェック**: すべてのSDK呼び出しの戻り値を確認してください
- **アンドゥグループ**: 操作はアンドゥグループで囲んでください
- **スレッドセーフティ**: SDKはメインスレッドでのみ使用可能です

関連項目
--------

- :doc:`../high-level/index` - 高レベルAPI
- Adobe After Effects SDK Documentation
