高レベルAPI
============

PyAEの高レベルAPIは、Pythonらしい直感的なインターフェースを提供します。

このAPIは以下の特徴があります：

- **Pythonicなインターフェース**: Pythonの慣習に従った設計
- **型ヒント完備**: IDEの補完とtype checkingをサポート
- **自動リソース管理**: C++リソースの自動管理
- **エラーハンドリング**: 適切な例外処理

トップレベルAPI
---------------

``ae`` モジュール直下で利用できる関数、列挙型、定数です。

.. toctree::
   :maxdepth: 2

   functions
   enums

プロジェクト構造
----------------

プロジェクトとその中のアイテム（コンポジション、フッテージ、フォルダ）を操作するAPIです。

.. toctree::
   :maxdepth: 2

   project
   item
   folder
   comp
   footage

レイヤー操作
------------

レイヤーとその構成要素（エフェクト、プロパティ、マスク、マーカー）を操作するAPIです。

.. toctree::
   :maxdepth: 2

   layer
   effect
   property
   mask
   marker
   three_d

レンダリング
------------

レンダーキュー、レンダリング実行、フレームバッファを操作するAPIです。

.. toctree::
   :maxdepth: 2

   render_queue
   render_monitor
   render
   layer_render_options
   async_render
   world
   sound_data

カラーマネジメント
------------------

カラープロファイルとOCIO設定を操作するAPIです。

.. toctree::
   :maxdepth: 2

   color_profile
   color_settings

ユーティリティ
--------------

メニュー操作、シリアライゼーション、パフォーマンス最適化などの補助機能です。

.. toctree::
   :maxdepth: 2

   menu
   serialize
   performance

UI拡張
------

カスタムUIパネルと設定の永続化機能です。

.. toctree::
   :maxdepth: 2

   panel_ui
   persistent_data

クラス一覧
----------

プロジェクト構造
~~~~~~~~~~~~~~~~

- :class:`Project <project.Project>` - After Effectsプロジェクト
- :class:`Item <item.Item>` - アイテム基底クラス
- :class:`Folder <folder.Folder>` - フォルダ
- :class:`Comp <comp.Comp>` - コンポジション
- :class:`CompItem <comp.CompItem>` - コンポジションアイテム
- :class:`Footage <footage.Footage>` - フッテージ（統合版：追加前/追加後両対応）

レイヤー操作
~~~~~~~~~~~~

- :class:`Layer <layer.Layer>` - レイヤー
- :class:`Effect <effect.Effect>` - エフェクト
- :class:`Property <property.Property>` - プロパティ
- :class:`Mask <mask.Mask>` - マスク
- :class:`Marker <marker.Marker>` - マーカー
- :doc:`three_d` - 3Dレイヤー・カメラ・ライト

レンダリング
~~~~~~~~~~~~

- :class:`RenderQueueItem <render_queue.RenderQueueItem>` - レンダーキューアイテム
- :doc:`render_monitor` - レンダリング監視（コールバック）
- :class:`RenderOptions <render.RenderOptions>` - レンダリング設定
- :class:`FrameReceipt <render.FrameReceipt>` - レンダリング結果
- :class:`Renderer <render.Renderer>` - レンダリング実行
- :class:`LayerRenderOptions <layer_render_options.LayerRenderOptions>` - レイヤーレンダリング設定
- :doc:`async_render` - 非同期レンダリング
- :class:`World <world.World>` - フレームバッファ（画像データ）
- :class:`SoundData <sound_data.SoundData>` - サウンドデータ

カラーマネジメント
~~~~~~~~~~~~~~~~~~

- :class:`ColorProfile <color_profile.ColorProfile>` - カラープロファイル
- :doc:`color_settings` - カラー設定（OCIO）

ユーティリティ
~~~~~~~~~~~~~~

- :doc:`menu` - メニュー・コマンド
- :mod:`ae_serialize <serialize>` - シリアライゼーション
- :doc:`performance` - パフォーマンス最適化（batch/cache/perf）

UI拡張
~~~~~~

- :doc:`panel_ui` - PySide統合によるカスタムUIパネル
- :doc:`persistent_data` - 永続データ（設定保存）
