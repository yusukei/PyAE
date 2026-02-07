# ae.property - Property/Keyframe API
# PyAE - Python for After Effects

from typing import Any, Optional, Union, Iterator, List, Tuple, Dict, Sequence
from builtins import property as _property
from .types import (
    PropertyType,
    StreamValueType,
    StreamGroupingType,
    KeyInterpolation,
    EasingType,
)

class Property:
    """プロパティクラス"""

    # ========================================
    # 基本プロパティ
    # ========================================

    @_property
    def valid(self) -> bool:
        """プロパティが有効かどうか"""
        ...

    @_property
    def name(self) -> str:
        """プロパティ名"""
        ...

    def set_name(self, name: str) -> None:
        """
        プロパティ名を変更

        Args:
            name: 新しいプロパティ名
        """
        ...

    @_property
    def match_name(self) -> str:
        """マッチ名（プロパティの固有識別子）"""
        ...

    @_property
    def depth(self) -> int:
        """プロパティ階層の深さ"""
        ...

    @_property
    def _handle(self) -> int:
        """内部ハンドル (int, AEGP_StreamRefH)"""
        ...

    # ========================================
    # タイプ情報
    # ========================================

    @_property
    def stream_value_type(self) -> StreamValueType:
        """
        ストリーム値タイプ

        Returns:
            StreamValueType: NoData, ThreeD_Spatial, ThreeD, TwoD_Spatial, TwoD, OneD, Color, Arbitrary, Marker, LayerID, MaskID, Mask, TextDocument
        """
        ...

    @_property
    def grouping_type(self) -> StreamGroupingType:
        """
        グルーピングタイプ

        Returns:
            StreamGroupingType: Invalid, Leaf, NamedGroup, IndexedGroup
        """
        ...

    @_property
    def type(self) -> PropertyType:
        """プロパティタイプ（レガシー互換）"""
        ...

    @_property
    def is_group(self) -> bool:
        """プロパティグループかどうか"""
        ...

    @_property
    def is_leaf(self) -> bool:
        """リーフプロパティ（値を持つ）かどうか"""
        ...

    @_property
    def is_primitive(self) -> bool:
        """プリミティブプロパティかどうか"""
        ...

    @_property
    def is_effect_param(self) -> bool:
        """エフェクトパラメータかどうか"""
        ...

    @_property
    def index(self) -> int:
        """エフェクトパラメータインデックス (0-based, エフェクトパラメータでない場合は -1)"""
        ...

    # ========================================
    # 能力フラグ
    # ========================================

    @_property
    def can_vary_over_time(self) -> bool:
        """時間によって変化できるかどうか"""
        ...

    @_property
    def can_have_keyframes(self) -> bool:
        """キーフレームを持てるかどうか"""
        ...

    @_property
    def can_have_expression(self) -> bool:
        """エクスプレッションを持てるかどうか"""
        ...

    @_property
    def can_set_value(self) -> bool:
        """値を設定できるかどうか"""
        ...

    # ========================================
    # 値アクセス
    # ========================================

    @_property
    def value(self) -> Any:
        """プロパティ値（時間0.0での値、または現在の静的値）"""
        ...

    @value.setter
    def value(self, val: Any) -> None: ...
    def get_value(self, time: float = 0.0) -> Any:
        """
        指定時間の値を取得

        Args:
            time: 時間（秒）。デフォルトは0.0
        """
        ...

    def set_value(self, value: Any) -> None:
        """
        値を設定（キーフレームがない場合は全時間に適用）

        Args:
            value: 設定する値
        """
        ...

    def get_value_at_time(self, time: float) -> Any:
        """
        指定時間の値を取得

        Args:
            time: 時間（秒）
        """
        ...

    def set_value_at_time(self, time: float, value: Any) -> None:
        """
        指定時間に値を設定（キーフレームを作成/更新）

        Args:
            time: 時間（秒）
            value: 設定する値
        """
        ...

    def get_raw_bytes(self, time: float = 0.0) -> bytes:
        """
        生のバイナリデータを取得 (bdataフォーマット)

        Args:
            time: 時間（秒）
        """
        ...

    def get_stream_metadata(self) -> Dict[str, Any]:
        """
        ストリームメタデータを取得（flags, min/max, types）
        """
        ...

    # ========================================
    # TextDocument 専用
    # ========================================

    def set_source_text(self, text: str, time: float = 0.0) -> None:
        """
        テキストレイヤーのソーステキストを設定

        TextDocument タイプのプロパティ（ADBE Text Document）でのみ使用可能。
        他のタイプのプロパティで呼び出すとエラーになります。

        Args:
            text: 設定するテキスト（UTF-8）
            time: 時間（秒）。デフォルトは0.0

        Raises:
            RuntimeError: TextDocument タイプでない場合

        Example:
            ```python
            # テキストレイヤーのソーステキストを設定
            text_layer = comp.layer(1)
            text_props = text_layer.properties.property("ADBE Text Properties")
            source_text = text_props.property("ADBE Text Document")
            source_text.set_source_text("Hello, World!")
            ```
        """
        ...

    # ========================================
    # キーフレーム
    # ========================================

    @_property
    def num_keyframes(self) -> int:
        """キーフレーム数"""
        ...

    @_property
    def is_time_varying(self) -> bool:
        """時間によって変化するかどうか"""
        ...

    @_property
    def keyframes(self) -> List["Keyframe"]:
        """キーフレームオブジェクトのリスト"""
        ...

    def add_keyframe(self, time: float, value: Any) -> int:
        """
        キーフレームを追加

        Args:
            time: 時間（秒）
            value: 値

        Returns:
            int: 追加されたキーフレームのインデックス
        """
        ...

    def remove_keyframe(self, index: int) -> None:
        """
        キーフレームを削除

        Args:
            index: キーフレームインデックス
        """
        ...

    def remove_all_keyframes(self) -> None:
        """全キーフレームを削除"""
        ...

    def get_keyframe_time(self, index: int) -> float:
        """キーフレーム時間を取得"""
        ...

    def get_keyframe_value(self, index: int) -> Any:
        """キーフレーム値を取得"""
        ...

    def set_keyframe_value(self, index: int, value: Any) -> None:
        """キーフレーム値を設定"""
        ...

    def get_keyframe_interpolation(self, index: int) -> Tuple[str, str]:
        """キーフレーム補間タイプを取得 (in, out) -> ("linear", "linear") など"""
        ...

    def set_keyframe_interpolation(
        self, index: int, in_type: str, out_type: str
    ) -> None:
        """キーフレーム補間タイプを設定 (linear, bezier, hold)"""
        ...

    def keyframe_time(self, index: int) -> float:
        """キーフレーム時間を取得 (get_keyframe_time のエイリアス)"""
        ...

    def keyframe_in_interpolation(self, index: int) -> str:
        """キーフレーム入力補間タイプを取得"""
        ...

    def keyframe_out_interpolation(self, index: int) -> str:
        """キーフレーム出力補間タイプを取得"""
        ...

    def set_temporal_ease(
        self, index: int, ease_in: Sequence[float], ease_out: Sequence[float]
    ) -> None:
        """
        キーフレームの時間的イーズを設定

        Args:
            index: キーフレームインデックス
            ease_in: [speed, influence] 入力イーズ
            ease_out: [speed, influence] 出力イーズ
        """
        ...

    # ========================================
    # エクスプレッション
    # ========================================

    @_property
    def has_expression(self) -> bool:
        """エクスプレッションの有無"""
        ...

    @_property
    def expression(self) -> str:
        """エクスプレッション文字列"""
        ...

    @expression.setter
    def expression(self, value: str) -> None: ...
    def enable_expression(self, enable: bool) -> None:
        """エクスプレッションの有効/無効切り替え"""
        ...

    # ========================================
    # 子プロパティ (Dynamic Stream)
    # ========================================

    @_property
    def num_properties(self) -> int:
        """子プロパティ数（グループの場合）"""
        ...

    @_property
    def properties(self) -> List["Property"]:
        """子プロパティのリスト"""
        ...

    def property(self, key: Union[int, str]) -> Optional["Property"]:
        """
        子プロパティを取得

        Args:
            key: インデックス(int) または マッチ名(str)

        Returns:
            Property: 子プロパティ、見つからない場合は None
        """
        ...

    def __getitem__(self, key: Union[int, str]) -> "Property":
        """子プロパティを取得"""
        ...

    def __iter__(self) -> Iterator["Property"]:
        """子プロパティのイテレータ"""
        ...

    def __len__(self) -> int: ...
    def __contains__(self, name: str) -> bool: ...
    def __bool__(self) -> bool: ...

    # ========================================
    # Dynamic Stream (Indexed Group)
    # ========================================

    def can_add_stream(self, match_name: str) -> bool:
        """
        指定タイプのストリームを追加できるか確認

        Args:
            match_name: 追加するストリームのマッチ名
                       例: "ADBE Text Animator" (Text Animator追加)
                           "ADBE Mask Atom" (マスク追加)

        Returns:
            bool: 追加可能ならTrue
        """
        ...

    def add_stream(self, match_name: str) -> Optional["Property"]:
        """
        ダイナミックストリーム（子プロパティ）を追加

        Indexed Group (Text Animators, Masks, Effects等) に新しい
        ストリームを追加します。

        Args:
            match_name: 追加するストリームのマッチ名

        Returns:
            Property: 追加されたプロパティ、失敗時はNone

        Example:
            ```python
            # Text Animator を追加
            animators = text_props.property("ADBE Text Animators")
            if animators.can_add_stream("ADBE Text Animator"):
                new_animator = animators.add_stream("ADBE Text Animator")
                new_animator.set_name("My Animator")
            ```
        """
        ...

    def get_dynamic_stream_flags(self) -> int:
        """ダイナミックストリームフラグを取得"""
        ...

    def set_dynamic_stream_flag(self, flag: int, value: bool) -> None:
        """ダイナミックストリームフラグを設定"""
        ...

    # ========================================
    # Shape Path 専用
    # ========================================

    def set_shape_path_vertices(
        self,
        vertices: List[List[float]],
        in_tangents: List[List[float]],
        out_tangents: List[List[float]],
        closed: bool = True
    ) -> None:
        """
        シェイプパスの頂点を設定

        Args:
            vertices: 頂点座標のリスト [[x1,y1], [x2,y2], ...]
            in_tangents: 入力タンジェントのリスト
            out_tangents: 出力タンジェントのリスト
            closed: パスを閉じるかどうか

        Example:
            ```python
            # 三角形を作成
            path_prop.set_shape_path_vertices(
                vertices=[[0, 0], [100, 0], [50, 100]],
                in_tangents=[[0, 0], [0, 0], [0, 0]],
                out_tangents=[[0, 0], [0, 0], [0, 0]],
                closed=True
            )
            ```
        """
        ...

    # ========================================
    # Serialization
    # ========================================

    def to_dict(self) -> Dict[str, Any]:
        """
        Export property to dictionary.

        Returns:
            dict: Property data including value, keyframes, expression
        """
        ...

    def update_from_dict(self, data: Dict[str, Any]) -> None:
        """
        Update property from dictionary.

        Args:
            data: Property dictionary
        """
        ...

class BezierTangent:
    """ベジェタンジェント"""

    speed: float
    influence: float
    def __init__(self, speed: float = 0.0, influence: float = 33.33) -> None: ...
    def __repr__(self) -> str: ...

class SpatialTangent:
    """空間タンジェント"""

    x: float
    y: float
    z: float
    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0) -> None: ...
    def __repr__(self) -> str: ...

class Keyframe:
    """キーフレームクラス"""

    @_property
    def valid(self) -> bool: ...
    @_property
    def time(self) -> float: ...
    @time.setter
    def time(self, value: float) -> None: ...
    @_property
    def value(self) -> Any: ...
    @value.setter
    def value(self, val: Any) -> None: ...
    @_property
    def index(self) -> int: ...
    @_property
    def in_interpolation(self) -> KeyInterpolation: ...
    @_property
    def out_interpolation(self) -> KeyInterpolation: ...
    def set_interpolation(
        self, in_type: KeyInterpolation, out_type: KeyInterpolation
    ) -> None: ...
    def set_easing(self, easing_type: EasingType) -> None: ...
    @_property
    def temporal_ease_in(self) -> BezierTangent: ...
    @_property
    def temporal_ease_out(self) -> BezierTangent: ...
    def set_temporal_ease_in(self, ease: BezierTangent) -> None: ...
    def set_temporal_ease_out(self, ease: BezierTangent) -> None: ...
    @_property
    def spatial_tangent_in(self) -> SpatialTangent: ...
    @_property
    def spatial_tangent_out(self) -> SpatialTangent: ...
    def set_spatial_tangents(
        self, tangent_in: SpatialTangent, tangent_out: SpatialTangent
    ) -> None: ...
    @_property
    def roving(self) -> bool: ...
    @roving.setter
    def roving(self, value: bool) -> None: ...
    @_property
    def temporal_continuous(self) -> bool: ...
    @temporal_continuous.setter
    def temporal_continuous(self, value: bool) -> None: ...
    @_property
    def temporal_auto_bezier(self) -> bool: ...
    @temporal_auto_bezier.setter
    def temporal_auto_bezier(self, value: bool) -> None: ...
    def __init__(self) -> None: ...
    def __repr__(self) -> str: ...
    def __bool__(self) -> bool: ...

__all__ = ["Property", "Keyframe", "BezierTangent", "SpatialTangent"]
