# ae.async_render - Async Render API
# PyAE - Python for After Effects

def render_item_frame(
    async_manager: int, purpose_id: int, render_options: int
) -> int:
    """
    アイテムフレームを非同期にレンダリング

    PF_Event_DRAWコンテキスト内でのみ使用可能。
    PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGERフラグが設定されている必要があります。

    Args:
        async_manager: PF_AsyncManagerPポインタ
        purpose_id: リクエストの一意ID（同じIDで古いリクエストを自動キャンセル）
        render_options: AEGP_RenderOptionsHハンドル

    Returns:
        AEGP_FrameReceiptHハンドル
    """
    ...

def render_layer_frame(
    async_manager: int, purpose_id: int, layer_render_options: int
) -> int:
    """
    レイヤーフレームを非同期にレンダリング

    PF_Event_DRAWコンテキスト内でのみ使用可能。
    PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGERフラグが設定されている必要があります。

    Args:
        async_manager: PF_AsyncManagerPポインタ
        purpose_id: リクエストの一意ID（同じIDで古いリクエストを自動キャンセル）
        layer_render_options: AEGP_LayerRenderOptionsHハンドル

    Returns:
        AEGP_FrameReceiptHハンドル
    """
    ...

__all__ = [
    "render_item_frame",
    "render_layer_frame",
]
