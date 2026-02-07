PyAE Documentation
==================

**PyAE (Python for After Effects)** は、Adobe After Effectsの自動化とスクリプティングのためのPythonバインディングを提供するプラグインです。

PyAEを使用すると、Pythonの強力な機能を活用してAfter Effectsのワークフローを自動化できます。

主な機能
--------

- **高レベルAPI**: Pythonらしい直感的なインターフェース
- **低レベルAPI**: AEGP SDKへの直接アクセス
- **型ヒント**: 完全な型アノテーションでIDEサポート
- **アンドゥ対応**: すべての操作がアンドゥ可能
- **パフォーマンス**: C++ネイティブ実装による高速処理

クイックスタート
----------------

.. code-block:: python

   import ae

   # プロジェクトを取得
   project = ae.Project.get_current()

   # コンポジションを作成
   comp = project.create_comp("MyComp", 1920, 1080, duration=10.0)

   # レイヤーを追加
   layer = comp.add_solid("Red Solid", 1920, 1080, color=(1, 0, 0))
   layer.position = (960, 540)

   # エフェクトを追加
   effect = layer.add_effect("ADBE Gaussian Blur 2")
   effect.property("Blurriness").value = 50.0

.. toctree::
   :maxdepth: 2
   :caption: はじめに

   getting-started/installation
   getting-started/quickstart

.. toctree::
   :maxdepth: 2
   :caption: チュートリアル

   tutorials/basic-animation
   tutorials/layer-manipulation
   tutorials/effect-automation
   tutorials/footage-management
   guides/best-practices

.. toctree::
   :maxdepth: 2
   :caption: API リファレンス

   api/high-level/index
   api/low-level/index

.. toctree::
   :maxdepth: 1
   :caption: PyAE開発ガイド

   guides/getting-started
   guides/build-notes
   guides/test-guide
   guides/architecture
   guides/aegp-suite-architecture

.. toctree::
   :maxdepth: 2
   :caption: TestEffect開発ガイド

   guides/effects-plugin-development
   guides/testeffect

索引
----

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
