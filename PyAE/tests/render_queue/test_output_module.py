#!/usr/bin/env python
# -*- coding: utf-8 -*-
# tests/render_queue/test_output_module.py

"""
OutputModuleSuite4のテスト

このモジュールはAEGP_OutputModuleSuite4の新規実装関数をテストします。

テスト対象関数:
- AEGP_GetEmbedOptions / AEGP_SetEmbedOptions
- AEGP_GetPostRenderAction / AEGP_SetPostRenderAction
- AEGP_GetEnabledOutputs / AEGP_SetEnabledOutputs
- AEGP_GetOutputChannels / AEGP_SetOutputChannels
- AEGP_GetStretchInfo / AEGP_SetStretchInfo
- AEGP_GetCropInfo / AEGP_SetCropInfo
- AEGP_GetSoundFormatInfo / AEGP_SetSoundFormatInfo
- AEGP_GetExtraOutputModuleInfo

重要: インデックス規約とハンドルペアリング
========================================
1. AEGP_GetOutputModuleByIndex は 0ベースインデックス を使用（AEGP SDKの例外）
2. rq_itemH と outmodH は一貫したペアとして使用する必要がある:
   - (rq_itemH, outmodH): 両方とも同じRQアイテムから取得した実際のハンドル ✅
   - (0, 0): Queuebertパターン（最初のRQアイテム、最初の出力モジュール）✅
   - (0, outmodH) や (rq_itemH, 0): 混合パターンはエラー ❌

NOTE: このテストは Queuebert SDK sample に従い、(0, 0) パターンを使用。
実際のハンドルペアを使用する場合は rq_item._handle と
ae.sdk.AEGP_GetOutputModuleByIndex(rq_itemH, 0) で取得した outmodH を使用。
"""

import ae

try:
    from ..test_utils import TestSuite, assert_true, assert_isinstance, assert_equal
except ImportError:
    from test_utils import TestSuite, assert_true, assert_isinstance, assert_equal

# =============================================================================
# グローバル変数
# =============================================================================

suite = TestSuite("OutputModuleSuite4")

_test_rq_item = None
_test_outmod = None
_test_comp = None

# =============================================================================
# セットアップ
# =============================================================================

@suite.setup
def setup():
    """テスト環境のセットアップ"""
    global _test_rq_item, _test_outmod, _test_comp

    # プロジェクト取得
    proj = ae.Project.get_current()

    # テスト用コンポジション作成
    _test_comp = proj.create_comp("OutputModuleTest_Comp", 1920, 1080, 1.0, 5.0, 30.0)

    # RQアイテム追加
    # NOTE: 0, 0 パターンは「最初のRQアイテム」を参照するため、
    # RQアイテムが存在する必要がある
    _test_rq_item = ae.render_queue.add_comp(_test_comp)

    # Output Module追加
    # NOTE: Queuebert sample pattern - use 0 for first RQ item
    _test_outmod = ae.sdk.AEGP_AddDefaultOutputModule(0)

@suite.teardown
def teardown():
    """テスト環境のクリーンアップ"""
    global _test_comp, _test_rq_item

    # コンポジションを削除すると、関連するRQアイテムも自動削除される
    # そのため、先にコンポジションを削除し、RQアイテムの明示的削除は行わない
    if _test_comp:
        _test_comp.delete()

    # RQアイテムはコンポジション削除時に自動削除されるため、明示的削除は不要
    # (明示的に削除すると、すでに削除されたハンドルにアクセスしてメモリエラーが発生する可能性がある)

# =============================================================================
# Embed Options Tests
# =============================================================================

@suite.test
def test_embed_options():
    """埋め込みオプションの取得・設定"""
    # NOTE: Using 0, 0 pattern like Queuebert SDK sample
    # 0, 0 means "first RQ item, first output module"

    # Get default value
    embed_options = ae.sdk.AEGP_GetEmbedOptions(0, 0)
    assert_isinstance(embed_options, int, "Embed options should be an integer")

    # Set to LINK (1)
    ae.sdk.AEGP_SetEmbedOptions(0, 0, 1)

    # Verify
    new_value = ae.sdk.AEGP_GetEmbedOptions(0, 0)
    assert_equal(new_value, 1, "Embed options should be set to LINK (1)")

@suite.test
def test_post_render_action():
    """ポストレンダーアクションの取得・設定"""
    # NOTE: Using 0, 0 pattern like Queuebert SDK sample

    # Get default value
    action = ae.sdk.AEGP_GetPostRenderAction(0, 0)
    assert_isinstance(action, int, "Post render action should be an integer")

    # Set to IMPORT (0)
    ae.sdk.AEGP_SetPostRenderAction(0, 0, 0)

    # Verify
    new_value = ae.sdk.AEGP_GetPostRenderAction(0, 0)
    assert_equal(new_value, 0, "Post render action should be set to IMPORT (0)")

@suite.test
def test_enabled_outputs():
    """有効な出力タイプの取得・設定"""
    # NOTE: Using 0, 0 pattern like Queuebert SDK sample

    # Get default value
    enabled = ae.sdk.AEGP_GetEnabledOutputs(0, 0)
    assert_isinstance(enabled, int, "Enabled outputs should be an integer")

    # Set to VIDEO + AUDIO (3)
    ae.sdk.AEGP_SetEnabledOutputs(0, 0, 3)

    # Verify
    new_value = ae.sdk.AEGP_GetEnabledOutputs(0, 0)
    assert_equal(new_value, 3, "Enabled outputs should be set to VIDEO+AUDIO (3)")

@suite.test
def test_output_channels():
    """出力チャンネルの取得・設定"""
    # NOTE: Using 0, 0 pattern like Queuebert SDK sample

    # Get default value
    channels = ae.sdk.AEGP_GetOutputChannels(0, 0)
    assert_isinstance(channels, int, "Output channels should be an integer")

    # Set to RGBA (1)
    ae.sdk.AEGP_SetOutputChannels(0, 0, 1)

    # Verify
    new_value = ae.sdk.AEGP_GetOutputChannels(0, 0)
    assert_equal(new_value, 1, "Output channels should be set to RGBA (1)")

@suite.test
def test_stretch_info():
    """ストレッチ情報の取得・設定"""
    # NOTE: Using 0, 0 pattern like Queuebert SDK sample

    # Get stretch info
    info = ae.sdk.AEGP_GetStretchInfo(0, 0)
    assert_isinstance(info, dict, "Stretch info should be a dictionary")
    assert_true("is_enabled" in info, "Stretch info should have 'is_enabled'")
    assert_true("stretch_quality" in info, "Stretch info should have 'stretch_quality'")
    assert_true("locked" in info, "Stretch info should have 'locked'")

    # Set stretch info
    ae.sdk.AEGP_SetStretchInfo(0, 0, True, 1)  # HIGH quality

    # Verify
    new_info = ae.sdk.AEGP_GetStretchInfo(0, 0)
    assert_equal(new_info["is_enabled"], True, "Stretch should be enabled")
    assert_equal(new_info["stretch_quality"], 1, "Stretch quality should be HIGH (1)")

@suite.test
def test_crop_info():
    """クロップ情報の取得・設定"""
    # NOTE: Using 0, 0 pattern like Queuebert SDK sample

    # Get crop info
    info = ae.sdk.AEGP_GetCropInfo(0, 0)
    assert_isinstance(info, dict, "Crop info should be a dictionary")
    assert_true("is_enabled" in info, "Crop info should have 'is_enabled'")
    assert_true("left" in info, "Crop info should have 'left'")
    assert_true("top" in info, "Crop info should have 'top'")
    assert_true("right" in info, "Crop info should have 'right'")
    assert_true("bottom" in info, "Crop info should have 'bottom'")

    # Set crop info
    crop_rect = {"left": 10, "top": 20, "right": 1910, "bottom": 1060}
    ae.sdk.AEGP_SetCropInfo(0, 0, True, crop_rect)

    # Verify
    new_info = ae.sdk.AEGP_GetCropInfo(0, 0)
    assert_equal(new_info["is_enabled"], True, "Crop should be enabled")
    assert_equal(new_info["left"], 10, "Crop left should be 10")
    assert_equal(new_info["top"], 20, "Crop top should be 20")

@suite.test
def test_sound_format_info():
    """サウンドフォーマット情報の取得・設定"""
    # NOTE: Using 0, 0 pattern like Queuebert SDK sample
    # Queuebert uses 0, 0 for rq_itemH and outmodH
    # This relies on internal AE fallback behavior

    # Get sound format info
    info = ae.sdk.AEGP_GetSoundFormatInfo(0, 0)
    assert_isinstance(info, dict, "Sound format info should be a dictionary")
    assert_true("sample_rate" in info, "Sound format should have 'sample_rate'")
    assert_true("encoding" in info, "Sound format should have 'encoding'")
    assert_true("bytes_per_sample" in info, "Sound format should have 'bytes_per_sample'")
    assert_true("num_channels" in info, "Sound format should have 'num_channels'")
    assert_true("audio_enabled" in info, "Sound format should have 'audio_enabled'")

    # Set sound format info (like Queuebert sample)
    # Only set if audio not enabled or sample_rate is 0
    if not info["audio_enabled"] or info["sample_rate"] == 0:
        sound_format = {
            "sample_rate": 44100.0,
            "encoding": 2,  # AEGP_SoundEncoding_FLOAT
            "bytes_per_sample": 4,
            "num_channels": 1
        }
        ae.sdk.AEGP_SetSoundFormatInfo(0, 0, sound_format, True)

        # Verify
        new_info = ae.sdk.AEGP_GetSoundFormatInfo(0, 0)
        assert_equal(new_info["sample_rate"], 44100.0, "Sample rate should be 44100")

@suite.test
def test_extra_output_module_info():
    """追加のOutput Module情報の取得"""
    # NOTE: Using 0, 0 pattern like Queuebert SDK sample

    # Get extra info
    info = ae.sdk.AEGP_GetExtraOutputModuleInfo(0, 0)
    assert_isinstance(info, dict, "Extra info should be a dictionary")
    assert_true("format" in info, "Extra info should have 'format'")
    assert_true("info" in info, "Extra info should have 'info'")
    assert_true("is_sequence" in info, "Extra info should have 'is_sequence'")
    assert_true("multi_frame" in info, "Extra info should have 'multi_frame'")

    # Format and info should be strings
    assert_isinstance(info["format"], str, "Format should be a string")
    assert_isinstance(info["info"], str, "Info should be a string")

# =============================================================================
# Handle Pattern Investigation Results (2026-02-03)
# =============================================================================
# 検証済みパターン:
# - (rq_itemH, outmodH): ✅ 正常動作 - 同じRQアイテムからの実際のハンドルペア
# - (0, 0): ✅ 正常動作 - Queuebert SDKパターン（最初のRQアイテム、最初のOM）
# - (0, outmodH): ❌ エラー - 混合パターン（ハンドルペアリング違反）
# - (rq_itemH, 0): ❌ エラー - 混合パターン（ハンドルペアリング違反）
# - (1, 0): ❌ クラッシュ - 数値をポインタ値として解釈（無効なメモリアクセス）
#
# 結論:
# 1. SDKは引数をポインタ値として扱う（インデックスではない）
# 2. (0, 0) は「最初のアイテム」を参照する特別なパターン
# 3. 実際のハンドルを使用する場合は、同じRQアイテムから取得したペアを使用
# 4. AEGP_GetOutputModuleByIndex は 0ベースインデックス（AEGP SDKの例外）
# =============================================================================

# =============================================================================
# テスト実行
# =============================================================================

def run():
    """すべてのテストを実行"""
    return suite.run()
