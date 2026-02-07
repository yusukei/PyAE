# PyAE サンプルスクリプト

このディレクトリには PyAE の使用方法を示すサンプルスクリプトが含まれています。

## スクリプト一覧

### hello_world.py
基本的な PyAE の使用方法を示します。
- プラグインのバージョン確認
- プロジェクト情報の取得
- アクティブコンポジションの情報表示

### create_comp.py
新しいコンポジションを作成する方法を示します。
- HD（1920x1080）コンポジションの作成
- 4K（3840x2160）コンポジションの作成
- コンポジションプロパティの設定

### animate_layer.py
レイヤーのアニメーション方法を示します。
- レイヤー情報の取得
- プロパティ値の設定
- キーフレームの追加（デモ）

## 使用方法

### After Effects から実行

1. After Effects を起動
2. プロジェクトを開く（または新規作成）
3. メニュー: **ウィンドウ > PyAE > Run Script...**
4. スクリプトファイル（.py）を選択
5. 実行結果はログファイルに出力されます

### ログの確認

ログファイルは以下に出力されます：
```
[プラグインディレクトリ]/logs/pyae_YYYYMMDD_HHMMSS.log
```

## API リファレンス

### ae モジュール

```python
import ae

# ユーティリティ
ae.version()           # バージョン文字列
ae.is_initialized()    # 初期化状態

# ロギング
ae.log_debug(msg)      # デバッグログ
ae.log_info(msg)       # 情報ログ
ae.log_warning(msg)    # 警告ログ
ae.log_error(msg)      # エラーログ
```

### Project クラス

```python
# 現在のプロジェクト取得
project = ae.Project.get_current()

# プロパティ
project.name           # プロジェクト名
project.path           # ファイルパス
project.num_items      # アイテム数

# メソッド
project.save()         # 保存

# 静的メソッド
ae.Project.is_open()   # プロジェクトが開いているか
```

### Comp クラス

```python
# アクティブコンポジション取得
comp = ae.Comp.get_active()

# プロパティ
comp.valid             # 有効かどうか
comp.name              # 名前（読み書き可）
comp.width             # 幅
comp.height            # 高さ
comp.duration          # 長さ（秒）
comp.frame_rate        # フレームレート（読み書き可）
comp.num_layers        # レイヤー数

# 静的メソッド：新規作成
comp = ae.Comp.create(
    name="New Comp",
    width=1920,
    height=1080,
    pixel_aspect=1.0,
    duration=10.0,
    frame_rate=30.0
)
```

### Item クラス

```python
# アクティブアイテム取得
item = ae.Item.get_active()

# プロパティ
item.valid             # 有効かどうか
item.type              # アイテムタイプ (ItemType enum)
item.name              # 名前（読み書き可）
item.width             # 幅
item.height            # 高さ
item.duration          # 長さ
item.frame_rate        # フレームレート
item.selected          # 選択状態（読み書き可）

# アイテムタイプ
ae.ItemType.None
ae.ItemType.Folder
ae.ItemType.Comp
ae.ItemType.Footage
```

### Layer クラス

```python
# プロパティ
layer.valid            # 有効かどうか
layer.name             # 名前（読み書き可）
layer.index            # インデックス（読み書き可）
layer.in_point         # イン点（読み書き可）
layer.out_point        # アウト点
layer.enabled          # 表示/非表示（読み書き可）
layer.solo             # ソロ状態（読み書き可）
layer.locked           # ロック状態（読み書き可）

# メソッド
layer.delete()         # レイヤー削除
new_layer = layer.duplicate()  # 複製
```

### Property クラス

```python
# プロパティ
prop.valid             # 有効かどうか
prop.name              # プロパティ名
prop.type              # 値タイプ (PropertyType enum)
prop.can_set_value     # 値設定可能か
prop.num_keyframes     # キーフレーム数
prop.is_time_varying   # キーフレームがあるか

# メソッド
value = prop.get_value(time=0.0)  # 値取得
prop.set_value(value, time=0.0)   # 値設定

# プロパティタイプ
ae.PropertyType.None
ae.PropertyType.OneDim     # 1次元（不透明度等）
ae.PropertyType.TwoDim     # 2次元（位置XY等）
ae.PropertyType.ThreeDim   # 3次元（位置XYZ等）
ae.PropertyType.Color      # 色
ae.PropertyType.NoValue    # 値なし（グループ等）
```

### Keyframe クラス

```python
# プロパティ
keyframe.valid             # 有効かどうか
keyframe.time              # 時間（秒）
keyframe.value             # 値
keyframe.index             # インデックス
keyframe.in_interpolation  # 入り補間タイプ
keyframe.out_interpolation # 出補間タイプ

# メソッド
keyframe.set_interpolation(in_type, out_type)

# 補間タイプ
ae.KeyInterpolation.Linear
ae.KeyInterpolation.Bezier
ae.KeyInterpolation.Hold
```

## トラブルシューティング

### プラグインが読み込まれない

1. `python312.dll` がプラグインと同じディレクトリにあるか確認
2. Visual C++ Redistributable 2015-2022 がインストールされているか確認
3. After Effects のバージョンが 2024 以降か確認

### スクリプトエラー

1. ログファイルを確認
2. `ae.is_initialized()` で初期化状態を確認
3. プロジェクトが開いているか確認

### パフォーマンス

- 大量のレイヤー操作は `ae.Comp` のアンドゥグループ内で実行
- 頻繁な UI 更新が必要な場合は進捗コールバックを使用
