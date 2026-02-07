import ae
import ae.sdk
import pytest


def test_sdk_constants():
    """SDK定数が正しくエクスポートされているか確認"""
    assert ae.sdk.AEGP_LayerStream_MARKER is not None
    assert ae.sdk.AEGP_MarkerString_COMMENT is not None
    assert ae.sdk.AEGP_LTimeMode_LayerTime is not None


def test_sdk_stream_and_marker():
    """低レベルAPIを使用したマーカー操作のテスト"""
    comp = ae.Comp.create("SDK Test Comp", 1920, 1080, 1.0, 10.0, 30.0)
    layer = comp.add_solid("Solid", 1920, 1080, (1.0, 0.0, 0.0), 10.0)

    # 1. プラグインID取得
    plugin_id = ae.sdk.AEGP_GetPluginID()
    assert plugin_id != 0

    # 2. ストリーム取得
    # Layer._handle プロパティを使用
    stream_h = ae.sdk.AEGP_GetNewLayerStream(
        plugin_id, layer._handle, ae.sdk.AEGP_LayerStream_MARKER
    )
    assert stream_h != 0

    try:
        # 3. マーカー作成
        marker_p = ae.sdk.AEGP_NewMarker()
        assert marker_p != 0

        # 4. コメント設定
        comment_text = "Low Level Marker"
        ae.sdk.AEGP_SetMarkerString(
            marker_p, ae.sdk.AEGP_MarkerString_COMMENT, comment_text
        )

        # 5. キーフレーム挿入 (1.0秒地点)
        time = 1.0
        key_index = ae.sdk.AEGP_InsertKeyframe(
            stream_h, ae.sdk.AEGP_LTimeMode_LayerTime, time
        )
        # key_index は 0以上の整数のみ有効とは限らないが(AEGP_InsertKeyframeはindexを返す)、
        # 通常は有効なインデックスが返る。

        # 6. 値設定
        ae.sdk.AEGP_SetKeyframeValue_Marker(stream_h, key_index, marker_p)

        # マーカーはストリームに値として設定されたので、ここでのローカル変数は用済みだが、
        # SDKの仕様として AEGP_NewMarker で作ったものは AEGP_DisposeMarker すべきか？
        # AEGP_SetKeyframeValue はディープコピーを行うか？
        # 通常、AEGP_SetKeyframeValue は値をコピーする。
        # AEGP_NewMarker で確保したメモリは呼び出し元が責任を持つ必要がある。
        ae.sdk.AEGP_DisposeMarker(marker_p)

        # 検証: 高レベルAPIで確認
        # layer.get_marker_comment は今回実装したものを利用
        # ただし、マーカーキーフレームが正しく設定されていれば取得できるはず
        read_comment = layer.get_marker_comment(time)
        assert read_comment == comment_text

    finally:
        # ストリーム解放
        ae.sdk.AEGP_DisposeStream(stream_h)


def test_marker_high_level():
    """高レベルMarkerクラスのテスト"""
    comp = ae.Comp.create("Marker Test Comp", 1920, 1080, 1.0, 10.0, 30.0)
    layer = comp.add_solid("Solid", 1920, 1080, (1.0, 0.0, 0.0), 10.0)

    # 1. Markerオブジェクト作成とプロパティ設定
    m = ae.Marker()
    m.comment = "High Level Comment"
    m.label = 1  # Red usually
    m.chapter = "Chapter 1"
    m.url = "http://example.com"
    m.frame_target = "_blank"
    m.protected = True

    assert m.comment == "High Level Comment"
    assert m.label == 1
    assert m.chapter == "Chapter 1"
    assert m.protected == True

    # 2. レイヤーに追加
    time = 2.0
    layer.add_marker(time, m)

    # 3. 確認 (単純なコメント取得)
    read_comment = layer.get_marker_comment(time)
    assert read_comment == "High Level Comment"


if __name__ == "__main__":
    # 手動実行用
    try:
        test_sdk_constants()
        test_sdk_stream_and_marker()
        test_marker_high_level()
        print("SDK Tests Passed!")
    except Exception as e:
        print(f"SDK Tests Failed: {e}")
        import traceback

        traceback.print_exc()
