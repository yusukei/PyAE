# ae_serialize - Serialization API for PyAE
# PyAE - Python for After Effects
#
# This module provides high-level serialization API for AE objects.
# Usage: import ae_serialize

from typing import Any, Dict, List, Optional, Union
from .item import CompItem, FolderItem
from .layer import Layer
from .property import Property
from .effect import Effect
from .footage import Footage


class SerializationContext:
    """
    シリアライズ/デシリアライズ操作のコンテキスト管理クラス

    IDからオブジェクトへのマッピングを管理し、参照解決に使用されます。
    """

    id_to_item: Dict[int, Any]
    project: Any

    def __init__(self) -> None:
        """コンテキストを初期化"""
        ...

    def register(self, json_id: int, ae_item: Any) -> None:
        """
        AEアイテムをJSON IDで登録（後の参照解決用）

        Args:
            json_id: JSONデータ内のID
            ae_item: After Effectsオブジェクト
        """
        ...

    def resolve(self, json_id: int) -> Optional[Any]:
        """
        JSON IDから対応するAEアイテムを解決

        Args:
            json_id: 解決するJSON ID

        Returns:
            対応するAEアイテム、見つからない場合はNone
        """
        ...

    def generate_id(self) -> int:
        """
        新規アイテム用のユニークIDを生成

        Returns:
            新しいユニークID
        """
        ...


# =============================================================================
# Project Serialization
# =============================================================================

def project_to_dict(project: Any = None) -> Dict[str, Any]:
    """
    プロジェクトを辞書にエクスポート

    Args:
        project: プロジェクトオブジェクト（省略時は現在のプロジェクト）

    Returns:
        プロジェクトデータ（version, items を含む）

    Raises:
        RuntimeError: プロジェクトが開かれていない場合

    Example:
        ```python
        import ae_serialize
        import json

        data = ae_serialize.project_to_dict()
        with open("project.json", "w") as f:
            json.dump(data, f, indent=2)
        ```
    """
    ...


def project_from_dict(
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> SerializationContext:
    """
    辞書から現在のプロジェクトにアイテムをインポート

    データに基づいて現在のプロジェクトに新規アイテムを作成します。

    Args:
        data: プロジェクト辞書（project_to_dict()の戻り値）
        context: SerializationContext（省略時は新規作成）

    Returns:
        IDマッピングを含むSerializationContext

    Raises:
        RuntimeError: プロジェクトが開かれていない場合
    """
    ...


def project_update_from_dict(
    project: Any,
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> None:
    """
    辞書から既存プロジェクトを更新

    IDまたは名前が一致する既存アイテムを更新します。

    Args:
        project: 更新するプロジェクト
        data: プロジェクト辞書
        context: SerializationContext（省略時は新規作成）
    """
    ...


# =============================================================================
# Comp Serialization
# =============================================================================

def comp_to_dict(comp: CompItem) -> Dict[str, Any]:
    """
    コンポジションを辞書にエクスポート

    Args:
        comp: コンポジションオブジェクト

    Returns:
        コンポジションデータ（レイヤーを含む）

    Example:
        ```python
        import ae_serialize
        import ae

        comp = ae.Project.get_current().items[0]
        data = ae_serialize.comp_to_dict(comp)
        ```
    """
    ...


def comp_from_dict(
    data: Dict[str, Any],
    parent_folder: Optional[FolderItem] = None,
    context: Optional[SerializationContext] = None
) -> CompItem:
    """
    辞書から新規コンポジションを作成

    Args:
        data: コンポジション辞書（comp_to_dict()の戻り値）
        parent_folder: 親フォルダ（省略時はルート）
        context: SerializationContext（省略時は新規作成）

    Returns:
        新規作成されたコンポジション

    Raises:
        RuntimeError: プロジェクトが開かれていない場合

    Example:
        ```python
        import ae_serialize
        import json

        with open("comp.json", "r") as f:
            data = json.load(f)
        new_comp = ae_serialize.comp_from_dict(data)
        ```
    """
    ...


def comp_update_from_dict(
    comp: CompItem,
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> None:
    """
    辞書から既存コンポジションを更新

    Args:
        comp: 更新するコンポジション
        data: コンポジション辞書
        context: SerializationContext（省略時は新規作成）
    """
    ...


# =============================================================================
# Layer Serialization
# =============================================================================

def layer_to_dict(layer: Layer, comp: Optional[CompItem] = None) -> Dict[str, Any]:
    """
    レイヤーを辞書にエクスポート

    Args:
        layer: レイヤーオブジェクト
        comp: 親コンポジション（省略可）

    Returns:
        レイヤーデータ（プロパティ、エフェクトを含む）
    """
    ...


def layer_from_dict(
    comp: CompItem,
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> Optional[Layer]:
    """
    辞書からコンポジションに新規レイヤーを作成

    Args:
        comp: 親コンポジション
        data: レイヤー辞書（layer_to_dict()の戻り値）
        context: SerializationContext（省略時は新規作成）

    Returns:
        新規作成されたレイヤー、失敗時はNone
    """
    ...


def layer_update_from_dict(
    layer: Layer,
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> None:
    """
    辞書から既存レイヤーを更新

    Args:
        layer: 更新するレイヤー
        data: レイヤー辞書
        context: SerializationContext（省略時は新規作成）
    """
    ...


# =============================================================================
# Property Serialization
# =============================================================================

def property_to_dict(prop: Property) -> Dict[str, Any]:
    """
    プロパティを辞書にエクスポート

    Args:
        prop: プロパティオブジェクト

    Returns:
        プロパティデータ（値、キーフレーム、エクスプレッションを含む）
    """
    ...


def property_update_from_dict(
    prop: Property,
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> None:
    """
    辞書からプロパティ値を復元

    Args:
        prop: 更新するプロパティ
        data: プロパティ辞書（property_to_dict()の戻り値）
        context: SerializationContext（省略時は新規作成）
    """
    ...


# =============================================================================
# Effect Serialization
# =============================================================================

def effect_to_dict(effect: Effect) -> Dict[str, Any]:
    """
    エフェクトを辞書にエクスポート

    Args:
        effect: エフェクトオブジェクト

    Returns:
        エフェクトデータ（パラメータを含む）
    """
    ...


def effect_from_dict(
    layer: Layer,
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> Optional[Effect]:
    """
    辞書からレイヤーにエフェクトを追加

    Args:
        layer: エフェクトを追加するレイヤー
        data: エフェクト辞書（effect_to_dict()の戻り値）
        context: SerializationContext（省略時は新規作成）

    Returns:
        新規作成されたエフェクト、失敗時はNone
    """
    ...


def effect_update_from_dict(
    effect: Effect,
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> None:
    """
    辞書からエフェクトパラメータを更新

    Args:
        effect: 更新するエフェクト
        data: エフェクト辞書
        context: SerializationContext（省略時は新規作成）
    """
    ...


# =============================================================================
# Footage Serialization
# =============================================================================

def footage_to_dict(footage: Footage) -> Dict[str, Any]:
    """
    フッテージを辞書にエクスポート

    Args:
        footage: フッテージオブジェクト

    Returns:
        フッテージデータ（タイプ、寸法、設定を含む）

    Example:
        ```python
        import ae_serialize
        import ae

        footage = ae.Project.get_current().items[0]
        data = ae_serialize.footage_to_dict(footage)
        # data = {
        #     "name": "my_video.mp4",
        #     "footage_type": "File",
        #     "width": 1920,
        #     "height": 1080,
        #     ...
        # }
        ```
    """
    ...


def footage_from_dict(
    data: Dict[str, Any],
    parent_folder: Optional[FolderItem] = None,
    context: Optional[SerializationContext] = None
) -> Optional[Footage]:
    """
    辞書から新規フッテージを作成

    Args:
        data: フッテージ辞書（footage_to_dict()の戻り値）
        parent_folder: 親フォルダ（省略時はルート）
        context: SerializationContext（省略時は新規作成）

    Returns:
        新規作成されたフッテージ、失敗時はNone

    Example:
        ```python
        import ae_serialize

        # ソリッドを作成
        data = {
            "footage_type": "Solid",
            "name": "Red Solid",
            "width": 1920,
            "height": 1080,
            "solid_color": [1.0, 0.0, 0.0, 1.0]
        }
        footage = ae_serialize.footage_from_dict(data)

        # ファイルから作成
        data = {
            "footage_type": "File",
            "file_path": "/path/to/video.mp4"
        }
        footage = ae_serialize.footage_from_dict(data)
        ```
    """
    ...


def footage_update_from_dict(
    footage: Footage,
    data: Dict[str, Any],
    context: Optional[SerializationContext] = None
) -> None:
    """
    辞書から既存フッテージを更新

    注意: footage_typeなど一部のプロパティは作成後に変更できません。
    この関数は変更可能なプロパティのみを更新します。

    Args:
        footage: 更新するフッテージ
        data: フッテージ辞書
        context: SerializationContext（省略時は新規作成）

    Raises:
        RuntimeError: フッテージがプロジェクトに追加されていない場合

    Example:
        ```python
        import ae_serialize

        footage = project.items[0]
        ae_serialize.footage_update_from_dict(footage, {
            "name": "New Name",
            "comment": "Updated comment",
            "solid_color": [0.5, 0.5, 0.5, 1.0]  # ソリッドの場合のみ
        })
        ```
    """
    ...


# =============================================================================
# Utility Functions (re-exported from export_scene/import_scene)
# =============================================================================

def value_to_bdata(value: Any) -> str:
    """
    値をbdata形式（IEEE 754 big-endian hex）に変換

    Args:
        value: 変換する値（float、list、tupleなど）

    Returns:
        bdata形式の16進数文字列

    Example:
        ```python
        import ae_serialize
        bdata = ae_serialize.value_to_bdata([960.0, 540.0])
        # -> "408e0000000000004080e00000000000"
        ```
    """
    ...


def bdata_to_value(bdata: str) -> Optional[Any]:
    """
    bdata形式（IEEE 754 big-endian hex）を値に変換

    Args:
        bdata: 16進数文字列

    Returns:
        変換された値（float、listなど）、変換失敗時はNone

    Example:
        ```python
        import ae_serialize
        value = ae_serialize.bdata_to_value("44700000442a0000")
        # -> [960.0, 540.0]
        ```
    """
    ...


def bdata_to_path_vertices(bdata: str) -> Optional[Dict[str, Any]]:
    """
    bdata形式からシェイプパス頂点データに変換

    Args:
        bdata: 16進数文字列

    Returns:
        パス頂点データ（vertices, in_tangents, out_tangents, closed）
    """
    ...


def item_type_to_string(item_type: int) -> str:
    """
    アイテムタイプを文字列に変換

    Args:
        item_type: ItemType値

    Returns:
        文字列表現（"Comp", "Folder", "Footage"など）
    """
    ...


def layer_type_to_string(layer_type: int) -> str:
    """
    レイヤータイプを文字列に変換

    Args:
        layer_type: LayerType値

    Returns:
        文字列表現（"AV", "Text", "Shape"など）
    """
    ...


__all__ = [
    "SerializationContext",
    # Project
    "project_to_dict",
    "project_from_dict",
    "project_update_from_dict",
    # Comp
    "comp_to_dict",
    "comp_from_dict",
    "comp_update_from_dict",
    # Layer
    "layer_to_dict",
    "layer_from_dict",
    "layer_update_from_dict",
    # Property
    "property_to_dict",
    "property_update_from_dict",
    # Effect
    "effect_to_dict",
    "effect_from_dict",
    "effect_update_from_dict",
    # Footage
    "footage_to_dict",
    "footage_from_dict",
    "footage_update_from_dict",
    # Utilities
    "value_to_bdata",
    "bdata_to_value",
    "bdata_to_path_vertices",
    "item_type_to_string",
    "layer_type_to_string",
]
