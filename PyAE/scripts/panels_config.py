"""
PyAE Panel Configuration

プラグイン初期化時に読み込まれ、定義されたパネルがAEに登録されます。

各パネル定義:
    name: str       - AEのWindowメニューに表示される名前
    match_name: str - AE内部識別名（英数字・アンダースコアのみ、一意である必要あり）
    factory: str    - ウィジェットファクトリー関数（空の場合は手動でset_widget()を使用）

使い方:
    1. ここにパネルを定義
    2. AEを起動するとWindowメニューにパネルが表示される
    3. パネルを開いた後、スクリプトから ae.panel_ui.set_widget() でウィジェットを設定

例:
    import ae
    from PySide6.QtWidgets import QPushButton

    widget = QPushButton("Hello!")
    ae.panel_ui.set_widget("PyAE_SampleTool", widget)
"""

# パネル定義リスト
PANELS = [
    # Sample Tool パネル（手動でウィジェットを設定）
    {
        "name": "Sample Tool",
        "match_name": "PyAE_SampleTool",
        "factory": "",  # 空 = ae.panel_ui.set_widget()で手動設定
    },

    # パネルを追加する場合:
    # {
    #     "name": "My Panel",           # Windowメニューに表示される名前
    #     "match_name": "PyAE_MyPanel", # 内部ID（英数字とアンダースコアのみ）
    #     "factory": "",                # 空にして手動でset_widget()を使用
    # },
]


def get_panels():
    """
    パネル定義リストを取得

    C++側から呼び出されます。
    動的にパネルを追加したい場合はこの関数をオーバーライドしてください。

    Returns:
        list[dict]: パネル定義のリスト
    """
    return PANELS


def validate_panels():
    """
    パネル定義のバリデーション

    Returns:
        tuple[bool, str]: (成功フラグ, エラーメッセージ)
    """
    seen_names = set()
    seen_match_names = set()

    for i, panel in enumerate(PANELS):
        # 必須フィールドのチェック
        if "name" not in panel:
            return False, f"Panel {i}: 'name' is required"
        if "match_name" not in panel:
            return False, f"Panel {i}: 'match_name' is required"
        if "factory" not in panel:
            return False, f"Panel {i}: 'factory' is required"

        # match_nameのフォーマットチェック
        match_name = panel["match_name"]
        if not match_name.replace("_", "").isalnum():
            return False, f"Panel {i}: 'match_name' must contain only alphanumeric and underscore"

        # 一意性チェック
        if panel["name"] in seen_names:
            return False, f"Panel {i}: duplicate name '{panel['name']}'"
        if match_name in seen_match_names:
            return False, f"Panel {i}: duplicate match_name '{match_name}'"

        seen_names.add(panel["name"])
        seen_match_names.add(match_name)

        # ファクトリー形式チェック（空でない場合のみ）
        factory = panel["factory"]
        if factory and ":" not in factory:
            return False, f"Panel {i}: 'factory' must be in 'module:function' format or empty"

    return True, ""
