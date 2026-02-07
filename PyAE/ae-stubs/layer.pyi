# ae.layer - Layer API
# PyAE - Python for After Effects

from typing import Optional, Union, List, Tuple, Any, overload, Dict, TypedDict
from .types import LayerType, LayerQuality, SamplingQuality, BlendMode, TrackMatteMode
from .effect import Effect
from .mask import Mask
from .property import Property
from .marker import Marker

class TransferMode(TypedDict, total=False):
    """レイヤーの転送モード（ブレンドモード）情報"""
    mode: BlendMode
    flags: int
    track_matte: TrackMatteMode

class Layer:
    """レイヤークラス"""

    def __init__(self) -> None: ...

    # ========================================
    # 基本プロパティ
    # ========================================

    @property
    def valid(self) -> bool:
        """レイヤーが有効かどうか"""
        ...

    @property
    def name(self) -> str:
        """レイヤー名"""
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
    def quality(self) -> LayerQuality:
        """レイヤー品質"""
        ...
    @quality.setter
    def quality(self, value: LayerQuality) -> None: ...

    @property
    def sampling_quality(self) -> SamplingQuality:
        """サンプリング品質"""
        ...
    @sampling_quality.setter
    def sampling_quality(self, value: SamplingQuality) -> None: ...

    @property
    def label(self) -> int:
        """ラベルカラーID (-1=NONE, 0=NO_LABEL, 1-16=カラーラベル)"""
        ...
    @label.setter
    def label(self, value: int) -> None: ...

    @overload
    def add_marker(self, time: float, comment: str = "") -> None: ...
    @overload
    def add_marker(self, time: float, marker: Marker) -> None: ...
    def add_marker(
        self, time: float, comment_or_marker: Union[str, Marker] = ""
    ) -> None:
        """
        マーカーを追加します。

        Args:
            time: 時間（秒）
            comment_or_marker: コメント文字列、または Marker オブジェクト
        """
        ...

    def get_marker_comment(self, time: float) -> str:
        """指定時間のマーカーコメントを取得"""
        ...

    @property
    def index(self) -> int:
        """レイヤーインデックス（0から開始）"""
        ...
    @index.setter
    def index(self, value: int) -> None: ...

    # ========================================
    # 時間プロパティ
    # ========================================

    @property
    def in_point(self) -> float:
        """インポイント（秒）"""
        ...
    @in_point.setter
    def in_point(self, value: float) -> None: ...
    @property
    def out_point(self) -> float:
        """アウトポイント（秒）"""
        ...
    @out_point.setter
    def out_point(self, value: float) -> None: ...
    @property
    def start_time(self) -> float:
        """スタートタイム（秒）"""
        ...
    @start_time.setter
    def start_time(self, value: float) -> None: ...
    @property
    def duration(self) -> float:
        """デュレーション（秒）"""
        ...

    @property
    def stretch(self) -> Tuple[int, int]:
        """タイムストレッチ比率 (numerator, denominator)"""
        ...
    @stretch.setter
    def stretch(self, value: Tuple[int, int]) -> None: ...

    @property
    def current_time(self) -> float:
        """現在の時刻（秒）- 読み取り専用"""
        ...

    # ========================================
    # フラグプロパティ
    # ========================================

    @property
    def enabled(self) -> bool:
        """有効/無効 (ビデオスイッチ)"""
        ...
    @enabled.setter
    def enabled(self, value: bool) -> None: ...
    @property
    def audio_enabled(self) -> bool:
        """オーディオ有効/無効"""
        ...
    @audio_enabled.setter
    def audio_enabled(self, value: bool) -> None: ...
    @property
    def solo(self) -> bool:
        """ソロ状態"""
        ...
    @solo.setter
    def solo(self, value: bool) -> None: ...
    @property
    def locked(self) -> bool:
        """ロック状態"""
        ...
    @locked.setter
    def locked(self, value: bool) -> None: ...
    @property
    def shy(self) -> bool:
        """シャイ状態"""
        ...
    @shy.setter
    def shy(self, value: bool) -> None: ...
    @property
    def collapse_transformation(self) -> bool:
        """コラップス/連続ラスタライズ"""
        ...
    @collapse_transformation.setter
    def collapse_transformation(self, value: bool) -> None: ...

    @property
    def selected(self) -> bool:
        """選択状態"""
        ...
    @selected.setter
    def selected(self, value: bool) -> None: ...

    # ========================================
    # トランスフォームプロパティ
    # ========================================

    @property
    def position(self) -> tuple[float, ...]:
        """位置"""
        ...
    @position.setter
    def position(self, value: tuple[float, ...]) -> None: ...
    @property
    def scale(self) -> tuple[float, ...]:
        """スケール"""
        ...
    @scale.setter
    def scale(self, value: tuple[float, ...]) -> None: ...
    @property
    def rotation(self) -> float:
        """回転"""
        ...
    @rotation.setter
    def rotation(self, value: float) -> None: ...
    @property
    def anchor_point(self) -> tuple[float, ...]:
        """アンカーポイント"""
        ...
    @anchor_point.setter
    def anchor_point(self, value: tuple[float, ...]) -> None: ...
    @property
    def opacity(self) -> float:
        """不透明度"""
        ...
    @opacity.setter
    def opacity(self, value: float) -> None: ...

    @property
    def transfer_mode(self) -> TransferMode:
        """転送モード（ブレンドモード）

        Returns:
            TransferMode dict with keys:
                - mode (int): ブレンドモード (PF_Xfer_*)
                - flags (int): 転送フラグ
                - track_matte (int): トラックマットモード (TRACK_MATTE_*)
        """
        ...
    @transfer_mode.setter
    def transfer_mode(self, value: TransferMode) -> None: ...

    def to_world_xform(self, comp_time: float) -> List[List[float]]:
        """
        指定されたコンポジション時間でのレイヤーのワールド変換行列を取得

        Args:
            comp_time: コンポジション時間（秒）

        Returns:
            4x4変換行列（リストのリスト）
        """
        ...

    # ========================================
    # キーフレーム設定（ハイレベルAPI）
    # ========================================

    def set_position_keyframe(self, time: float, value: tuple[float, ...]) -> None:
        """指定時間にポジションキーフレームを追加"""
        ...

    def set_scale_keyframe(self, time: float, value: tuple[float, ...]) -> None:
        """指定時間にスケールキーフレームを追加"""
        ...

    def set_rotation_keyframe(self, time: float, value: float) -> None:
        """指定時間にローテーションキーフレームを追加"""
        ...

    def set_opacity_keyframe(self, time: float, value: float) -> None:
        """指定時間にオパシティキーフレームを追加"""
        ...

    # ========================================
    # 親子関係・タイプ・3D
    # ========================================

    @property
    def parent(self) -> Optional["Layer"]:
        """親レイヤー"""
        ...
    @parent.setter
    def parent(self, value: Optional["Layer"]) -> None: ...
    @property
    def layer_type(self) -> LayerType:
        """レイヤータイプ"""
        ...

    @property
    def is_adjustment_layer(self) -> bool:
        """調整レイヤーかどうか（読み取り専用）"""
        ...

    @property
    def is_adjustment(self) -> bool:
        """調整レイヤーフラグ（読み書き可能）"""
        ...
    @is_adjustment.setter
    def is_adjustment(self, value: bool) -> None: ...

    @property
    def is_null_layer(self) -> bool:
        """ヌルレイヤーかどうか"""
        ...

    @property
    def is_null(self) -> bool:
        """ヌルレイヤーかどうか（is_null_layer のエイリアス）"""
        ...

    @property
    def is_3d_layer(self) -> bool:
        """3Dレイヤーかどうか（読み取り専用）"""
        ...

    # ========================================
    # ID関連
    # ========================================

    @property
    def id(self) -> int:
        """レイヤーID（読み取り専用）"""
        ...

    @property
    def source_item_id(self) -> int:
        """ソースアイテムID（読み取り専用）"""
        ...

    @property
    def object_type(self) -> int:
        """オブジェクトタイプ（読み取り専用）"""
        ...

    @property
    def is_2d(self) -> bool:
        """2Dレイヤーかどうか（読み取り専用）"""
        ...

    @property
    def three_d(self) -> bool:
        """3Dレイヤーフラグ（読み書き可能、is_3d のエイリアス）"""
        ...
    @three_d.setter
    def three_d(self, value: bool) -> None: ...

    @property
    def is_3d(self) -> bool:
        """3Dレイヤーかどうか"""
        ...
    @is_3d.setter
    def is_3d(self, value: bool) -> None: ...

    @property
    def time_remapping_enabled(self) -> bool:
        """タイムリマップが有効かどうか"""
        ...
    @time_remapping_enabled.setter
    def time_remapping_enabled(self, value: bool) -> None: ...

    # ========================================
    # トラックマット
    # ========================================

    @property
    def has_track_matte(self) -> bool:
        """トラックマットを持つかどうか"""
        ...

    @property
    def is_used_as_track_matte(self) -> bool:
        """トラックマットとして使用されているかどうか"""
        ...

    @property
    def track_matte_layer(self) -> Optional['Layer']:
        """トラックマットレイヤー（Noneの場合、トラックマットなし）"""
        ...

    def set_track_matte(self, matte_layer: 'Layer', matte_mode: TrackMatteMode) -> None:
        """
        トラックマットを設定します。

        Args:
            matte_layer: トラックマットとして使用するレイヤー
            matte_mode: トラックマットモード
                - TRACK_MATTE_NO_TRACK_MATTE (0): トラックマットなし
                - TRACK_MATTE_ALPHA (1): アルファマット
                - TRACK_MATTE_NOT_ALPHA (2): アルファ反転マット
                - TRACK_MATTE_LUMA (3): ルミナンスマット
                - TRACK_MATTE_NOT_LUMA (4): ルミナンス反転マット
        """
        ...

    def remove_track_matte(self) -> None:
        """トラックマットを削除します"""
        ...

    # ========================================
    # エフェクト・マスク
    # ========================================

    @property
    def num_effects(self) -> int:
        """エフェクト数"""
        ...

    @property
    def effects(self) -> List[Effect]:
        """エフェクトのリスト"""
        ...

    def effect(self, index_or_name: Union[int, str]) -> Optional[Effect]:
        """エフェクトを取得"""
        ...

    def add_effect(self, match_name: str) -> Effect:
        """エフェクトを追加"""
        ...

    @property
    def num_masks(self) -> int:
        """マスク数"""
        ...

    @property
    def masks(self) -> List[Mask]:
        """マスクのリスト"""
        ...

    def mask(self, index_or_name: Union[int, str]) -> Optional[Mask]:
        """マスクを取得"""
        ...

    def add_mask(self, vertices: List[Tuple[float, float]] = ...) -> Mask:
        """マスクを追加"""
        ...

    # ========================================
    # プロパティアクセス (Dynamic)
    # ========================================

    @property
    def properties(self) -> Property:
        """ルートプロパティグループ (動的探索用)"""
        ...

    def property(self, name: str) -> Property:
        """名前でプロパティを取得"""
        ...

    def get_property(self, name: str) -> Property:
        """名前でプロパティを取得（property()のエイリアス）"""
        ...

    # ========================================
    # メソッド
    # ========================================

    def delete(self) -> None:
        """削除"""
        ...

    def duplicate(self) -> "Layer":
        """複製"""
        ...

    def move_to(self, index: int) -> None:
        """指定インデックスへ移動"""
        ...

    @property
    def _handle(self) -> int:
        """内部ハンドル (ae.three_dなどで使用)"""
        ...

    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...
    def __eq__(self, other: object) -> bool: ...
    def __hash__(self) -> int: ...

    # ========================================
    # Serialization
    # ========================================

    def to_dict(self) -> dict:
        """
        Export layer to dictionary.

        Returns:
            dict: Layer data including properties and effects
        """
        ...

    def update_from_dict(self, data: dict) -> None:
        """
        Update existing layer from dictionary.

        Args:
            data: Layer dictionary
        """
        ...

__all__ = ["Layer", "TransferMode"]
