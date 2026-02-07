# ae.menu - Menu Command API
# PyAE - Python for After Effects

from typing import Optional, Callable, List

class Command:
    """
    メニューコマンドを表すクラス

    register_command() でメニューアイテムを作成し、
    Pythonコールバックでクリックを処理できます。

    Example:
        def on_click():
            print("Menu item clicked!")

        cmd = ae.menu.register_command("My Script", ae.menu.WINDOW, on_click)
        cmd.enabled = True
        cmd.checked = False
    """

    @property
    def enabled(self) -> bool:
        """コマンドが有効（クリック可能）かどうか"""
        ...
    @enabled.setter
    def enabled(self, value: bool) -> None: ...

    @property
    def checked(self) -> bool:
        """コマンドにチェックマークがあるかどうか"""
        ...
    @checked.setter
    def checked(self, value: bool) -> None: ...

    @property
    def name(self) -> str:
        """コマンドの表示名"""
        ...
    @name.setter
    def name(self, value: str) -> None: ...

    @property
    def command_id(self) -> int:
        """一意のコマンドID"""
        ...

    @property
    def menu_id(self) -> int:
        """このコマンドが配置されているメニューID"""
        ...

    def set_on_click(self, callback: Optional[Callable[[], None]]) -> None:
        """
        メニューアイテムがクリックされたときのコールバック関数を設定

        Args:
            callback: 引数なしの関数、またはクリアするにはNone
        """
        ...

    def set_on_update(self, callback: Optional[Callable[[], bool]]) -> None:
        """
        メニュー更新コールバック（有効状態の制御）を設定

        Args:
            callback: boolを返す関数（True=有効）、またはクリアするにはNone
        """
        ...

    def execute(self) -> None:
        """コマンドをプログラムから実行"""
        ...

    def remove(self) -> None:
        """メニューからコマンドを削除"""
        ...

    def __repr__(self) -> str: ...

# Menu ID constants
NONE: int
APPLE: int
FILE: int
EDIT: int
COMPOSITION: int
LAYER: int
EFFECT: int
WINDOW: int
FLOATERS: int
KF_ASSIST: int
IMPORT: int
SAVE_FRAME_AS: int
PREFS: int
EXPORT: int
ANIMATION: int
PURGE: int
NEW: int

def register_command(
    name: str,
    menu_id: int,
    callback: Optional[Callable[[], None]] = None,
    after_item: int = -1,
) -> Command:
    """
    新しいメニューコマンドを登録

    Args:
        name: メニューアイテムの表示名
        menu_id: 追加先のメニュー（例: ae.menu.WINDOW）
        callback: クリック時に呼ばれる関数（省略可）
        after_item: 位置ヒント（-1でソート順）

    Returns:
        作成されたメニューコマンドオブジェクト
    """
    ...

def execute(command_id: int) -> None:
    """コマンドIDでコマンドを実行"""
    ...

def get_unique_id() -> int:
    """一意のコマンドIDを取得"""
    ...

def add_to_file(
    name: str,
    callback: Optional[Callable[[], None]] = None,
    after_item: int = -1,
) -> Command:
    """Fileメニューにコマンドを追加"""
    ...

def add_to_edit(
    name: str,
    callback: Optional[Callable[[], None]] = None,
    after_item: int = -1,
) -> Command:
    """Editメニューにコマンドを追加"""
    ...

def add_to_composition(
    name: str,
    callback: Optional[Callable[[], None]] = None,
    after_item: int = -1,
) -> Command:
    """Compositionメニューにコマンドを追加"""
    ...

def add_to_layer(
    name: str,
    callback: Optional[Callable[[], None]] = None,
    after_item: int = -1,
) -> Command:
    """Layerメニューにコマンドを追加"""
    ...

def add_to_effect(
    name: str,
    callback: Optional[Callable[[], None]] = None,
    after_item: int = -1,
) -> Command:
    """Effectメニューにコマンドを追加"""
    ...

def add_to_window(
    name: str,
    callback: Optional[Callable[[], None]] = None,
    after_item: int = -1,
) -> Command:
    """Windowメニューにコマンドを追加"""
    ...

def add_to_animation(
    name: str,
    callback: Optional[Callable[[], None]] = None,
    after_item: int = -1,
) -> Command:
    """Animationメニューにコマンドを追加"""
    ...

def clear_all_callbacks() -> None:
    """登録されたすべてのメニューコールバックをクリア"""
    ...

def get_registered_command_ids() -> List[int]:
    """コールバックが登録されたコマンドIDのリストを取得"""
    ...

__all__ = [
    "Command",
    "NONE",
    "APPLE",
    "FILE",
    "EDIT",
    "COMPOSITION",
    "LAYER",
    "EFFECT",
    "WINDOW",
    "FLOATERS",
    "KF_ASSIST",
    "IMPORT",
    "SAVE_FRAME_AS",
    "PREFS",
    "EXPORT",
    "ANIMATION",
    "PURGE",
    "NEW",
    "register_command",
    "execute",
    "get_unique_id",
    "add_to_file",
    "add_to_edit",
    "add_to_composition",
    "add_to_layer",
    "add_to_effect",
    "add_to_window",
    "add_to_animation",
    "clear_all_callbacks",
    "get_registered_command_ids",
]
