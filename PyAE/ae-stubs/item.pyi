# ae.item - Item/Folder/CompItem API
# PyAE - Python for After Effects
#
# Note: FootageItem has been unified with the Footage class.
# Use ae.Footage instead of ae.FootageItem.
# The FootageItem alias is provided for backward compatibility.

from typing import Optional, Union, List, Dict, Tuple, Any, Iterator
from .types import ItemType
from .layer import Layer


class Item:
    """プロジェクトアイテムの基底クラス"""

    def __init__(self) -> None: ...

    # ========================================
    # 基本プロパティ
    # ========================================
    @property
    def valid(self) -> bool:
        """アイテムが有効かどうか"""
        ...

    @property
    def type(self) -> ItemType:
        """アイテムタイプ"""
        ...

    @property
    def id(self) -> int:
        """アイテムID"""
        ...

    @property
    def name(self) -> str:
        """アイテム名"""
        ...
    @name.setter
    def name(self, value: str) -> None: ...
    @property
    def comment(self) -> str:
        """コメント"""
        ...
    @comment.setter
    def comment(self, value: str) -> None: ...
    @property
    def label(self) -> int:
        """ラベルカラー番号 (0-16)"""
        ...
    @label.setter
    def label(self, value: int) -> None: ...
    @property
    def selected(self) -> bool:
        """選択状態"""
        ...
    @selected.setter
    def selected(self, value: bool) -> None: ...

    # ========================================
    # フラグ
    # ========================================
    @property
    def use_proxy(self) -> bool:
        """プロキシを使用しているか"""
        ...
    @use_proxy.setter
    def use_proxy(self, value: bool) -> None: ...
    @property
    def missing(self) -> bool:
        """フッテージが欠落しているか"""
        ...

    # ========================================
    # 寸法・時間
    # ========================================
    @property
    def width(self) -> int:
        """幅（ピクセル）"""
        ...

    @property
    def height(self) -> int:
        """高さ（ピクセル）"""
        ...

    @property
    def duration(self) -> float:
        """デュレーション（秒）"""
        ...

    @property
    def frame_rate(self) -> float:
        """フレームレート（fps） - コンポジションのみ有効"""
        ...

    # ========================================
    # 階層
    # ========================================
    @property
    def parent_folder(self) -> Optional["Folder"]:
        """親フォルダ"""
        ...
    @parent_folder.setter
    def parent_folder(self, value: Optional[Union["Folder", "Item"]]) -> None: ...

    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...
    def __eq__(self, other: object) -> bool: ...
    def __hash__(self) -> int: ...

    # ========================================
    # メソッド
    # ========================================
    def delete(self) -> None:
        """削除"""
        ...

    def duplicate(self) -> "Item":
        """複製 (コンポジションのみサポート)"""
        ...

    @staticmethod
    def get_active() -> Optional["Item"]:
        """アクティブなアイテムを取得"""
        ...

    @property
    def _handle(self) -> int:
        """内部ハンドル (int)"""
        ...

    def _get_handle_ptr(self) -> int: ...


class Folder(Item):
    """フォルダアイテム"""

    def __init__(self) -> None: ...
    @property
    def num_items(self) -> int:
        """フォルダ内のアイテム数"""
        ...

    @property
    def items(self) -> List[Item]:
        """フォルダ内のアイテムリスト"""
        ...

    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...
    def __len__(self) -> int: ...
    def __iter__(self) -> Iterator[Item]: ...
    def __getitem__(self, index: int) -> Item: ...
    def __contains__(self, name: str) -> bool: ...


class CompItem(Item):
    """コンポジションアイテム"""

    def __init__(self) -> None: ...

    # ========================================
    # コンポジション設定
    # ========================================
    @property
    def pixel_aspect(self) -> float:
        """ピクセルアスペクト比"""
        ...
    @pixel_aspect.setter
    def pixel_aspect(self, value: float) -> None: ...
    @property
    def duration(self) -> float:
        """デュレーション（秒）"""
        ...
    @duration.setter
    def duration(self, value: float) -> None: ...
    @property
    def frame_rate(self) -> float:
        """フレームレート（fps）"""
        ...
    @frame_rate.setter
    def frame_rate(self, value: float) -> None: ...
    @property
    def bg_color(self) -> Tuple[float, float, float]:
        """背景色 (r, g, b)"""
        ...
    @bg_color.setter
    def bg_color(self, value: Tuple[float, float, float]) -> None: ...
    @property
    def work_area_start(self) -> float:
        """ワークエリア開始時間"""
        ...
    @work_area_start.setter
    def work_area_start(self, value: float) -> None: ...
    @property
    def work_area_duration(self) -> float:
        """ワークエリア長さ"""
        ...
    @work_area_duration.setter
    def work_area_duration(self, value: float) -> None: ...

    # ========================================
    # レイヤーアクセス
    # ========================================
    @property
    def num_layers(self) -> int:
        """レイヤー数"""
        ...

    @property
    def layers(self) -> List[Layer]:
        """全レイヤーリスト"""
        ...

    @property
    def selected_layers(self) -> List[Layer]:
        """選択レイヤーリスト"""
        ...

    def layer(self, key: Union[int, str]) -> Optional[Layer]:
        """インデックス(0-based)または名前でレイヤーを取得"""
        ...

    # ========================================
    # レイヤー作成
    # ========================================
    def add_solid(
        self,
        name: str,
        width: int,
        height: int,
        color: Tuple[float, float, float] = (1.0, 1.0, 1.0),
        duration: float = -1.0,
    ) -> Layer:
        """ソリッドレイヤーを追加"""
        ...

    def add_null(self, name: str = "Null", duration: float = -1.0) -> Layer:
        """ヌルレイヤーを追加"""
        ...

    def add_camera(self, name: str = "Camera", center: Tuple[float, float] = (0.0, 0.0)) -> Layer:
        """カメラレイヤーを追加"""
        ...

    def add_light(self, name: str = "Light", center: Tuple[float, float] = (0.0, 0.0)) -> Layer:
        """ライトレイヤーを追加"""
        ...

    def add_text(self, text: str = "") -> Layer:
        """テキストレイヤーを追加"""
        ...

    def add_shape(self) -> Layer:
        """シェイプレイヤーを追加"""
        ...

    def add_layer(self, item: Item, duration: float = -1.0) -> Layer:
        """アイテムからレイヤーを作成"""
        ...

    @property
    def _handle(self) -> int: ...
    @property
    def _comp_handle(self) -> int:
        """内部コンポジションハンドル (int)"""
        ...

    def _get_comp_handle_ptr(self) -> int: ...

    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...
    def __eq__(self, other: object) -> bool: ...
    def __hash__(self) -> int: ...
    def __len__(self) -> int: ...
    def __iter__(self) -> Iterator[Layer]: ...
    def __getitem__(self, key: Union[int, str]) -> Layer: ...
    def __contains__(self, name: str) -> bool: ...

    # ========================================
    # シリアライゼーション
    # ========================================
    def to_dict(self) -> Dict[str, Any]:
        """コンポジションを辞書にエクスポート"""
        ...

    def update_from_dict(self, data: Dict[str, Any]) -> None:
        """辞書からコンポジション設定を更新"""
        ...


class SequenceOptions:
    """シーケンスインポートオプション"""

    is_sequence: bool
    force_alphabetical: bool
    start_frame: int
    end_frame: int


# Backward compatibility: FootageItem is now unified with Footage
# Import Footage from footage module and use it as FootageItem
from .footage import Footage as FootageItem


__all__ = ["Item", "Folder", "CompItem", "FootageItem", "SequenceOptions"]
