# test_runner.py
# PyAE 統合テストランナー
#
# 使用方法:
#   After Effects のスクリプトコンソールで実行:
#   exec(open("path/to/scripts/tests/test_runner.py").read())
#
#   または、特定のテストモジュールのみ実行:
#   exec(open("path/to/scripts/tests/test_project.py").read())

import sys
import os
from typing import List, Dict
from datetime import datetime

# スクリプトディレクトリをパスに追加
# __file__ が定義されていない場合（exec()で実行された場合）の対応
try:
    script_dir = os.path.dirname(os.path.abspath(__file__))
except NameError:
    # exec()で実行された場合、現在のディレクトリから推測
    import inspect

    script_dir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda: None)))

parent_dir = os.path.dirname(script_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
if script_dir not in sys.path:
    sys.path.insert(0, script_dir)

# テストモジュールをインポート
try:
    # 相対インポートを試す（パッケージとして実行された場合）
    from .core import test_project
    from .core import test_item
    from .core import test_comp
    from .core import test_layer
    from .core import test_layer_quality_label
    from .core import test_layer_advanced
    from .core import test_effect
    from .core import test_mask
    from .core import test_property
    from .core import test_property_advanced
    from .core import test_stream_suite  # Enabled: SDK API version issues resolved (2026-02-01)
    from .core import test_render_queue
    from .core import test_3d
    from .core import test_dynamic_property
    from .core import test_command
    from .core import test_utility
    from .core import test_marker
    from .animation import test_keyframe_operations
    from .animation import test_keyframe_interpolation
    from .animation import test_animation_roundtrip
    from .animation import test_motion_path
    from .animation import test_easing
    from .parenting import test_parent_linking
    from .footage import test_footage_item
    from .folder import test_folder_operations
    from .batch import test_batch_operations
    from .special_layers import test_special_layers
    from .render import test_render_queue_details
    from .shape import test_shape_layer_basic
    from .shape import test_shape_properties
    from .shape import test_trim_paths
    from .expressions import test_expression_links
    from .text import test_text_outline
    from .render_queue import test_output_module
    from .render_queue import test_rq_item
    from .effects import test_arbitrary_data
    from . import test_camera_light_suite
    from . import test_io_suite
    from . import test_footage_suite
    from . import test_collection_suite
    from . import test_textdocument_suite
    from . import test_renderqueue_suite
    from . import test_data_suite
    from . import test_memory_diagnostics
    from .core import test_math_suite
    from .core import test_world_suite
    from .core import test_iterate_suite
    from .core import test_composite_suite
    from .core import test_render_async_manager_suite
    from .core import test_render_suite
    from .core import test_render_options_suite
    from .core import test_layer_render_options_suite
    from .core import test_render_queue_monitor_suite
    from .serialization import test_serialization
    from . import test_colorsettings_suite
    from . import test_sound_data_suite
    from .SDK import test_memory_suite
    from .core import test_item_view_suite
    from .high_level import test_menu
    from .high_level import test_persistent_data
    from .high_level import test_async_render
    from .high_level import test_render_monitor
    from .effects import test_effect_param
except ImportError:
    # 絶対インポート（exec()で実行された場合）
    from core import test_project
    from core import test_item
    from core import test_comp
    from core import test_layer
    from core import test_layer_quality_label
    from core import test_layer_advanced
    from core import test_effect
    from core import test_mask
    from core import test_property
    from core import test_property_advanced
    from core import test_stream_suite  # Enabled: SDK API version issues resolved (2026-02-01)
    from core import test_render_queue
    from core import test_3d
    from core import test_dynamic_property
    from core import test_command
    from core import test_utility
    from core import test_marker
    from animation import test_keyframe_operations
    from animation import test_keyframe_interpolation
    from animation import test_animation_roundtrip
    from animation import test_motion_path
    from animation import test_easing
    from parenting import test_parent_linking
    from footage import test_footage_item
    from folder import test_folder_operations
    from batch import test_batch_operations
    from special_layers import test_special_layers
    from render import test_render_queue_details
    from shape import test_shape_layer_basic
    from shape import test_shape_properties
    from shape import test_trim_paths
    from expressions import test_expression_links
    from text import test_text_outline
    from render_queue import test_output_module
    from render_queue import test_rq_item
    from effects import test_arbitrary_data
    import test_camera_light_suite
    import test_io_suite
    import test_footage_suite
    import test_collection_suite
    import test_textdocument_suite
    import test_renderqueue_suite
    import test_data_suite
    import test_memory_diagnostics
    from core import test_math_suite
    from core import test_world_suite
    from core import test_iterate_suite
    from core import test_composite_suite
    from core import test_render_async_manager_suite
    from core import test_render_suite
    from core import test_render_options_suite
    from core import test_layer_render_options_suite
    from core import test_render_queue_monitor_suite
    from serialization import test_serialization
    import test_colorsettings_suite
    import test_sound_data_suite
    from SDK import test_memory_suite
    from core import test_item_view_suite
    from high_level import test_menu
    from high_level import test_persistent_data
    from high_level import test_async_render
    from high_level import test_render_monitor
    from effects import test_effect_param


def run_all_tests() -> Dict:
    """
    全テストを実行して結果を返す（メモリリークチェック付き）
    """
    import ae

    print("\n" + "=" * 70)
    print("PyAE Test Suite - Full Run (with Memory Leak Detection)")
    print(f"Started at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 70)

    # Check if memory diagnostics are available
    has_memory_diagnostics = (hasattr(ae, 'get_memory_stats') and
                              hasattr(ae, 'log_memory_stats') and
                              hasattr(ae, 'check_memory_leak'))

    # 全テスト開始前のメモリ状態を記録 (optional)
    mem_initial = None
    if has_memory_diagnostics:
        try:
            mem_initial = ae.get_memory_stats()
            ae.log_memory_stats("Initial state (before all tests)")
            print(f"\nInitial Memory: Handles={mem_initial.handle_count}, Size={mem_initial.total_size} bytes")
        except Exception as e:
            print(f"Memory stats not available: {e}")
            has_memory_diagnostics = False

    all_results = {}
    total_passed = 0
    total_failed = 0
    memory_leaks = []

    # テストモジュール一覧
    test_modules = [
        ("Project", test_project),
        ("Item/Folder", test_item),
        ("Composition", test_comp),
        ("Layer", test_layer),
        ("Layer Quality/Label", test_layer_quality_label),
        ("Layer Advanced", test_layer_advanced),
        ("Effect", test_effect),
        ("Mask", test_mask),
        ("Property", test_property),
        ("Property Advanced", test_property_advanced),
        ("StreamSuite Low-level", test_stream_suite),  # Enabled: SDK API version issues resolved (2026-02-01)
        ("DynamicProperty", test_dynamic_property),
        ("RenderQueue", test_render_queue),
        ("3D Layer", test_3d),
        ("Command", test_command),
        ("Utility", test_utility),
        ("Marker", test_marker),
        ("Keyframe Operations", test_keyframe_operations),
        ("Keyframe Interpolation", test_keyframe_interpolation),
        ("Animation Roundtrip", test_animation_roundtrip),
        ("Motion Path Bezier", test_motion_path),
        ("Easing Functions", test_easing),
        ("Parent Linking", test_parent_linking),
        ("FootageItem", test_footage_item),
        ("Folder Operations", test_folder_operations),
        ("Batch Operations", test_batch_operations),
        ("Special Layers", test_special_layers),
        ("Render Queue Details", test_render_queue_details),
        ("Shape Layer Basic", test_shape_layer_basic),
        ("Shape Properties", test_shape_properties),
        ("Trim Paths", test_trim_paths),
        ("Expression Links", test_expression_links),
        ("Text Outline", test_text_outline),
        ("OutputModule Suite", test_output_module),
        ("RQItem Suite", test_rq_item),
        ("Camera/Light Suite", test_camera_light_suite),
        ("I/O Suite", test_io_suite),
        ("Footage Suite", test_footage_suite),
        ("Collection Suite", test_collection_suite),
        ("TextDocument Suite", test_textdocument_suite),
        ("RenderQueue Suite", test_renderqueue_suite),
        ("Data/Settings Suite", test_data_suite),
        ("Math Suite", test_math_suite),
        ("World Suite", test_world_suite),
        ("Iterate Suite", test_iterate_suite),
        # ("Composite Suite", test_composite_suite),  # Disabled: GPU world not supported yet (AE error 37,48)
        ("RenderAsyncManager Suite", test_render_async_manager_suite),
        ("Render Suite", test_render_suite),
        ("RenderOptions Suite", test_render_options_suite),
        ("LayerRenderOptions Suite", test_layer_render_options_suite),
        ("RenderQueueMonitor Suite", test_render_queue_monitor_suite),
        ("Memory Diagnostics", test_memory_diagnostics),
        ("Arbitrary Data", test_arbitrary_data),
        ("Serialization API", test_serialization),
        ("ColorSettings Suite", test_colorsettings_suite),
        ("SoundData Suite", test_sound_data_suite),
        ("Memory Suite", test_memory_suite),
        ("ItemView Suite", test_item_view_suite),
        ("Menu API", test_menu),
        ("PersistentData API", test_persistent_data),
        ("AsyncRender API", test_async_render),
        ("RenderMonitor API", test_render_monitor),
        ("EffectParam", test_effect_param),
    ]

    for name, module in test_modules:
        print(f"\n>>> Starting {name}...")

        # テストモジュール実行前のメモリ状態
        mem_before = None
        if has_memory_diagnostics:
            mem_before = ae.get_memory_stats()

        # テスト実行
        results = module.run()
        passed = sum(1 for r in results if r.passed)
        failed = len(results) - passed

        # テストモジュール実行後のメモリ状態
        mem_after = None
        if has_memory_diagnostics:
            mem_after = ae.get_memory_stats()

        # メモリリークチェック（許容範囲: 5ハンドル、10KB）
        has_leak = False
        if has_memory_diagnostics and mem_before and mem_after:
            has_leak = ae.check_memory_leak(mem_before, mem_after,
                                            tolerance_handles=5,
                                            tolerance_size=10*1024)

        # Convert TestResult objects to dictionaries for JSON serialization
        results_as_dicts = [r.to_dict() for r in results]
        result_dict = {
            "results": results_as_dicts,
            "passed": passed,
            "failed": failed,
            "total": len(results),
            "memory_leak": has_leak,
        }
        if mem_before and mem_after:
            result_dict["memory_before"] = {"handles": mem_before.handle_count, "size": mem_before.total_size}
            result_dict["memory_after"] = {"handles": mem_after.handle_count, "size": mem_after.total_size}
        all_results[name] = result_dict
        total_passed += passed
        total_failed += failed

        # メモリリーク検出時の警告
        if has_leak:
            memory_leaks.append(name)
            if hasattr(ae, 'log_memory_leak_details'):
                ae.log_memory_leak_details(mem_before, mem_after, f"Test Module: {name}")
            print(f"⚠️  MEMORY LEAK DETECTED in {name}")

        print(f"<<< Finished {name}: {passed}/{len(results)} passed")

    # 全テスト終了後のメモリ状態
    mem_final = None
    if has_memory_diagnostics:
        mem_final = ae.get_memory_stats()
        ae.log_memory_stats("Final state (after all tests)")
        print(f"\nFinal Memory: Handles={mem_final.handle_count}, Size={mem_final.total_size} bytes")

    # 全体のメモリリークチェック
    overall_leak = False
    if has_memory_diagnostics and mem_initial and mem_final:
        overall_leak = ae.check_memory_leak(mem_initial, mem_final,
                                            tolerance_handles=10,
                                        tolerance_size=100*1024)

    # 最終サマリー
    print("\n" + "=" * 70)
    print("FINAL SUMMARY")
    print("=" * 70)
    print(f"\n{'Test Suite':<25} {'Passed':<10} {'Failed':<10} {'Total':<10} {'Leak':<6}")
    print("-" * 61)

    for name, data in all_results.items():
        status = "✓" if data["failed"] == 0 else "✗"
        leak_status = "⚠️" if data.get("memory_leak", False) else "✓"
        print(
            f"{name:<25} {data['passed']:<10} {data['failed']:<10} {data['total']:<10} {leak_status:<6} {status}"
        )

    print("-" * 61)
    print(
        f"{'TOTAL':<25} {total_passed:<10} {total_failed:<10} {total_passed + total_failed:<10}"
    )
    print()

    if total_failed == 0:
        print("✅ All tests PASSED!")
    else:
        print(f"❌ {total_failed} test(s) FAILED")

    # メモリリークサマリー
    print("\n" + "=" * 70)
    print("MEMORY LEAK SUMMARY")
    print("=" * 70)

    mem_diff_handles = mem_final.handle_count - mem_initial.handle_count
    mem_diff_size = mem_final.total_size - mem_initial.total_size

    print(f"\nOverall Memory Change:")
    print(f"  Handles: {mem_diff_handles:+d} ({mem_initial.handle_count} → {mem_final.handle_count})")
    print(f"  Size:    {mem_diff_size:+d} bytes ({mem_diff_size/1024:+.2f} KB)")

    if memory_leaks:
        print(f"\n⚠️  Memory leaks detected in {len(memory_leaks)} module(s):")
        for module_name in memory_leaks:
            print(f"  - {module_name}")

    if overall_leak:
        print(f"\n⚠️  OVERALL MEMORY LEAK DETECTED")
        print(f"  Total memory growth exceeds acceptable threshold")
    else:
        print(f"\n✅ No significant overall memory leak")
        print(f"  Memory growth is within acceptable range")

    print(f"\nFinished at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 70)

    return {
        "all_results": all_results,
        "total_passed": total_passed,
        "total_failed": total_failed,
        "memory_leaks": memory_leaks,
        "overall_memory_leak": overall_leak,
        "memory_initial": {"handles": mem_initial.handle_count, "size": mem_initial.total_size},
        "memory_final": {"handles": mem_final.handle_count, "size": mem_final.total_size},
    }


def run_quick_tests() -> Dict:
    """
    クイックテスト（Project, Comp, Layer のみ）
    """
    print("\n" + "=" * 70)
    print("PyAE Test Suite - Quick Run")
    print("=" * 70)

    all_results = {}
    total_passed = 0
    total_failed = 0

    test_modules = [
        ("Project", test_project),
        ("Composition", test_comp),
        ("Layer", test_layer),
    ]

    for name, module in test_modules:
        try:
            results = module.run()
            passed = sum(1 for r in results if r.passed)
            failed = len(results) - passed
            all_results[name] = {
                "passed": passed,
                "failed": failed,
                "total": len(results),
            }
            total_passed += passed
            total_failed += failed
        except Exception as e:
            print(f"ERROR running {name}: {e}")
            total_failed += 1

    print(f"\nQuick Test: {total_passed} passed, {total_failed} failed")
    return {"passed": total_passed, "failed": total_failed}


def run_specific_tests(target: str) -> Dict:
    """
    指定されたテストモジュールのみ実行

    Args:
        target: テストモジュール名（例: "Composition", "Layer", "Project"）
                カンマ区切りで複数指定可能（例: "Composition,Layer"）

    Environment Variables:
        PYAE_TEST_REPEAT: 同一プロセス内でテストを繰り返す回数（デフォルト: 1）
    """
    import ae

    # 繰り返し回数を環境変数から取得
    repeat_count = int(os.getenv("PYAE_TEST_REPEAT", "1"))

    # 全テストモジュールのマッピング
    all_test_modules = {
        # Core tests
        "Project": test_project,
        "Item/Folder": test_item,
        "Composition": test_comp,
        "Layer": test_layer,
        "Layer Quality/Label": test_layer_quality_label,
        "Layer Advanced": test_layer_advanced,
        "Effect": test_effect,
        "Mask": test_mask,
        "Property": test_property,
        "Property Advanced": test_property_advanced,
        "StreamSuite Low-level": test_stream_suite,
        "DynamicProperty": test_dynamic_property,
        "RenderQueue": test_render_queue,
        "3D Layer": test_3d,
        "Command": test_command,
        "Utility": test_utility,
        "Marker": test_marker,
        # Animation tests
        "Keyframe Operations": test_keyframe_operations,
        "Keyframe Interpolation": test_keyframe_interpolation,
        "Animation Roundtrip": test_animation_roundtrip,
        "Motion Path Bezier": test_motion_path,
        "Easing Functions": test_easing,
        # Parenting tests
        "Parent Linking": test_parent_linking,
        # Footage tests
        "FootageItem": test_footage_item,
        # Folder tests
        "Folder Operations": test_folder_operations,
        # Batch tests
        "Batch Operations": test_batch_operations,
        # Special layer tests
        "Special Layers": test_special_layers,
        # Render tests
        "Render Queue Details": test_render_queue_details,
        # Shape tests
        "Shape Layer Basic": test_shape_layer_basic,
        "Shape Properties": test_shape_properties,
        "Trim Paths": test_trim_paths,
        # Expression tests
        "Expression Links": test_expression_links,
        # Text tests
        "Text Outline": test_text_outline,
        # RenderQueue module tests
        "OutputModule Suite": test_output_module,
        "RQItem Suite": test_rq_item,
        # SDK Suite tests
        "Camera/Light Suite": test_camera_light_suite,
        "I/O Suite": test_io_suite,
        "Footage Suite": test_footage_suite,
        "Collection Suite": test_collection_suite,
        "TextDocument Suite": test_textdocument_suite,
        "RenderQueue Suite": test_renderqueue_suite,
        "Data/Settings Suite": test_data_suite,
        "Math Suite": test_math_suite,
        "World Suite": test_world_suite,
        "Iterate Suite": test_iterate_suite,
        # "Composite Suite": test_composite_suite,  # Disabled: GPU world not supported yet
        "RenderAsyncManager Suite": test_render_async_manager_suite,
        "Render Suite": test_render_suite,
        "RenderOptions Suite": test_render_options_suite,
        "LayerRenderOptions Suite": test_layer_render_options_suite,
        "RenderQueueMonitor Suite": test_render_queue_monitor_suite,
        "Memory Diagnostics": test_memory_diagnostics,
        "Arbitrary Data": test_arbitrary_data,
        # Serialization tests
        "Serialization API": test_serialization,
        # New test modules
        "ColorSettings Suite": test_colorsettings_suite,
        "SoundData Suite": test_sound_data_suite,
        "Memory Suite": test_memory_suite,
        "ItemView Suite": test_item_view_suite,
        "Menu API": test_menu,
        "PersistentData API": test_persistent_data,
        "AsyncRender API": test_async_render,
        "RenderMonitor API": test_render_monitor,
        "EffectParam": test_effect_param,
    }

    # Short aliases for common suite names
    aliases = {
        # Core short names
        "project": "Project",
        "item": "Item/Folder",
        "comp": "Composition",
        "layer": "Layer",
        "effect": "Effect",
        "mask": "Mask",
        "property": "Property",
        "dynamic": "DynamicProperty",
        "renderqueue": "RenderQueue",
        "render": "RenderQueue",
        "3d": "3D Layer",
        "command": "Command",
        "utility": "Utility",
        "marker": "Marker",
        # Animation short names
        "keyframe": "Keyframe Operations",
        "interpolation": "Keyframe Interpolation",
        "animation": "Animation Roundtrip",
        "motion": "Motion Path Bezier",
        "easing": "Easing Functions",
        # Other short names
        "parent": "Parent Linking",
        "footage_item": "FootageItem",
        "folder": "Folder Operations",
        "batch": "Batch Operations",
        "special": "Special Layers",
        "shape": "Shape Layer Basic",
        "trim": "Trim Paths",
        "expression": "Expression Links",
        "text": "Text Outline",
        # SDK Suite short names
        "output": "OutputModule Suite",
        "rqitem": "RQItem Suite",
        "camera": "Camera/Light Suite",
        "light": "Camera/Light Suite",
        "io": "I/O Suite",
        "footage": "Footage Suite",
        "collection": "Collection Suite",
        "textdoc": "TextDocument Suite",
        "rq": "RenderQueue Suite",
        "data": "Data/Settings Suite",
        "settings": "Data/Settings Suite",
        "stream": "StreamSuite Low-level",
        "math": "Math Suite",
        "world": "World Suite",
        "iterate": "Iterate Suite",
        "composite": "Composite Suite",
        "renderasync": "RenderAsyncManager Suite",
        "asyncrender": "RenderAsyncManager Suite",
        "render": "Render Suite",
        "renderoptions": "RenderOptions Suite",
        "layerrenderoptions": "LayerRenderOptions Suite",
        "rqmonitor": "RenderQueueMonitor Suite",
        "memory": "Memory Diagnostics",
        "arbitrary": "Arbitrary Data",
        "arb": "Arbitrary Data",
        "serialize": "Serialization API",
        "serialization": "Serialization API",
        # New module short names
        "colorsettings": "ColorSettings Suite",
        "color": "ColorSettings Suite",
        "sounddata": "SoundData Suite",
        "sound": "SoundData Suite",
        "memsuite": "Memory Suite",
        "itemview": "ItemView Suite",
        "menu": "Menu API",
        "persistent": "PersistentData API",
        "prefs": "PersistentData API",
        "asyncapi": "AsyncRender API",
        "rendermonitor": "RenderMonitor API",
        "effectparam": "EffectParam",
    }

    # Test group definitions
    groups = {
        "core": [
            "Project", "Item/Folder", "Composition", "Layer",
            "Layer Quality/Label", "Layer Advanced", "Effect", "Mask",
            "Property", "Property Advanced", "StreamSuite Low-level",
            "DynamicProperty", "RenderQueue", "3D Layer",
            "Command", "Utility", "Marker",
            "ItemView Suite", "EffectParam"
        ],
        "animation": [
            "Keyframe Operations", "Keyframe Interpolation",
            "Animation Roundtrip", "Motion Path Bezier", "Easing Functions"
        ],
        "sdk": [
            "Camera/Light Suite", "I/O Suite", "Footage Suite",
            "Collection Suite", "TextDocument Suite", "RenderQueue Suite",
            "Data/Settings Suite", "OutputModule Suite", "RQItem Suite",
            "Math Suite", "World Suite", "Iterate Suite",
            "RenderAsyncManager Suite", "Render Suite", "RenderOptions Suite",
            "LayerRenderOptions Suite", "RenderQueueMonitor Suite",
            "ColorSettings Suite", "SoundData Suite", "Memory Suite"
            # Note: "Composite Suite" disabled - GPU world not supported yet
            # Note: "Register Suite" excluded - Effect plugin registration API, requires C callbacks
        ],
        "shape": [
            "Shape Layer Basic", "Shape Properties", "Trim Paths"
        ],
        "other": [
            "Parent Linking", "FootageItem", "Folder Operations",
            "Batch Operations", "Special Layers", "Render Queue Details",
            "Expression Links", "Text Outline", "Memory Diagnostics",
            "Arbitrary Data", "Serialization API",
            "Menu API", "PersistentData API",
            "AsyncRender API", "RenderMonitor API"
        ],
        "all": list(all_test_modules.keys())
    }

    # ターゲットをカンマ区切りで分割
    targets = [t.strip() for t in target.split(",")]

    # テストモジュールを選択
    test_modules = []
    for t in targets:
        # Check if it's a group name
        if t.lower() in groups:
            # Expand group to individual modules
            group_modules = groups[t.lower()]
            for module_name in group_modules:
                if module_name in all_test_modules:
                    # Avoid duplicates
                    if (module_name, all_test_modules[module_name]) not in test_modules:
                        test_modules.append((module_name, all_test_modules[module_name]))
        else:
            # Resolve alias if exists
            resolved = aliases.get(t.lower(), t)
            if resolved in all_test_modules:
                test_modules.append((resolved, all_test_modules[resolved]))
            elif t in all_test_modules:
                test_modules.append((t, all_test_modules[t]))
            else:
                print(f"WARNING: Unknown test module or group '{t}', skipping...")

    if not test_modules:
        print(f"ERROR: No valid test modules found for target: {target}")
        return {
            "all_results": {},
            "total_passed": 0,
            "total_failed": 1,
        }

    print("\n" + "=" * 70)
    print(f"PyAE Test Suite - Specific Tests: {', '.join(targets)} (with Memory Leak Detection)")
    if repeat_count > 1:
        print(f"Repeat Count: {repeat_count} iterations in the same process")
    print(f"Started at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 70)

    # テスト開始前のメモリ状態を記録
    mem_initial = ae.get_memory_stats()
    ae.log_memory_stats("Initial state (before specific tests)")
    print(f"\nInitial Memory: Handles={mem_initial.handle_count}, Size={mem_initial.total_size} bytes")

    # 繰り返し実行用の変数
    all_iterations_results = {}
    all_iterations_passed = 0
    all_iterations_failed = 0
    failed_iterations = 0

    # 繰り返しループ
    for iteration in range(1, repeat_count + 1):
        if repeat_count > 1:
            print("\n" + "=" * 70)
            print(f"ITERATION {iteration} of {repeat_count}")
            print("=" * 70)

        all_results = {}
        total_passed = 0
        total_failed = 0
        memory_leaks = []

        for name, module in test_modules:
            print(f"\n>>> Starting {name}...")

            # テストモジュール実行前のメモリ状態
            mem_before = ae.get_memory_stats()

            # テスト実行
            results = module.run()
            passed = sum(1 for r in results if r.passed)
            failed = len(results) - passed

            # テストモジュール実行後のメモリ状態
            mem_after = ae.get_memory_stats()

            # メモリリークチェック（許容範囲: 5ハンドル、10KB）
            has_leak = ae.check_memory_leak(mem_before, mem_after,
                                            tolerance_handles=5,
                                            tolerance_size=10*1024)

            # Convert TestResult objects to dictionaries for JSON serialization
            results_as_dicts = [r.to_dict() for r in results]
            all_results[name] = {
                "results": results_as_dicts,
                "passed": passed,
                "failed": failed,
                "total": len(results),
                "memory_leak": has_leak,
                "memory_before": {"handles": mem_before.handle_count, "size": mem_before.total_size},
                "memory_after": {"handles": mem_after.handle_count, "size": mem_after.total_size},
            }
            total_passed += passed
            total_failed += failed

            # メモリリーク検出時の警告
            if has_leak:
                memory_leaks.append(name)
                ae.log_memory_leak_details(mem_before, mem_after, f"Test Module: {name}")
                print(f"⚠️  MEMORY LEAK DETECTED in {name}")

            print(f"<<< Finished {name}: {passed}/{len(results)} passed")

        # テスト終了後のメモリ状態（イテレーション終了時）
        mem_iteration_final = ae.get_memory_stats()
        ae.log_memory_stats(f"Final state (after iteration {iteration})")
        print(f"\nIteration {iteration} Final Memory: Handles={mem_iteration_final.handle_count}, Size={mem_iteration_final.total_size} bytes")

        # イテレーション毎のメモリリークチェック
        iteration_leak = ae.check_memory_leak(mem_initial, mem_iteration_final,
                                              tolerance_handles=10,
                                              tolerance_size=100*1024)

        # サマリー
        print("\n" + "=" * 70)
        print(f"ITERATION {iteration} SUMMARY")
        print("=" * 70)
        print(f"\n{'Test Suite':<25} {'Passed':<10} {'Failed':<10} {'Total':<10} {'Leak':<6}")
        print("-" * 61)

        for name, data in all_results.items():
            status = "✓" if data["failed"] == 0 else "✗"
            leak_status = "⚠️" if data.get("memory_leak", False) else "✓"
            print(
                f"{name:<25} {data['passed']:<10} {data['failed']:<10} {data['total']:<10} {leak_status:<6} {status}"
            )

        print("-" * 61)
        print(
            f"{'TOTAL':<25} {total_passed:<10} {total_failed:<10} {total_passed + total_failed:<10}"
        )
        print()

        if total_failed == 0:
            print(f"✅ Iteration {iteration}: All tests PASSED!")
        else:
            print(f"❌ Iteration {iteration}: {total_failed} test(s) FAILED")
            failed_iterations += 1

        # メモリリークサマリー（イテレーション毎）
        if repeat_count > 1:
            print("\n" + "=" * 70)
            print(f"ITERATION {iteration} MEMORY LEAK SUMMARY")
            print("=" * 70)

            mem_diff_handles = mem_iteration_final.handle_count - mem_initial.handle_count
            mem_diff_size = mem_iteration_final.total_size - mem_initial.total_size

            print(f"\nIteration {iteration} Memory Change:")
            print(f"  Handles: {mem_diff_handles:+d} ({mem_initial.handle_count} → {mem_iteration_final.handle_count})")
            print(f"  Size:    {mem_diff_size:+d} bytes ({mem_diff_size/1024:+.2f} KB)")

            if memory_leaks:
                print(f"\n⚠️  Memory leaks detected in {len(memory_leaks)} module(s):")
                for module_name in memory_leaks:
                    print(f"  - {module_name}")

            if iteration_leak:
                print(f"\n⚠️  ITERATION {iteration} MEMORY LEAK DETECTED")
                print(f"  Memory growth exceeds acceptable threshold")
            else:
                print(f"\n✅ Iteration {iteration}: No significant memory leak")
                print(f"  Memory growth is within acceptable range")

        # 繰り返し実行の累積結果を更新
        all_iterations_results[f"iteration_{iteration}"] = all_results
        all_iterations_passed += total_passed
        all_iterations_failed += total_failed

    # 全イテレーション終了後の最終メモリ状態
    mem_final = ae.get_memory_stats()
    ae.log_memory_stats("Final state (after all iterations)")
    print(f"\nFinal Memory (after all iterations): Handles={mem_final.handle_count}, Size={mem_final.total_size} bytes")

    # 全体のメモリリークチェック
    overall_leak = ae.check_memory_leak(mem_initial, mem_final,
                                        tolerance_handles=10,
                                        tolerance_size=100*1024)

    # 全イテレーションのサマリー
    if repeat_count > 1:
        print("\n" + "=" * 70)
        print("ALL ITERATIONS SUMMARY")
        print("=" * 70)
        print(f"\nTotal iterations: {repeat_count}")
        passed_iterations = repeat_count - failed_iterations
        print(f"Passed iterations: {passed_iterations}")
        print(f"Failed iterations: {failed_iterations}")
        print(f"\nTotal tests passed: {all_iterations_passed}")
        print(f"Total tests failed: {all_iterations_failed}")

        if failed_iterations == 0:
            print(f"\n✅ All {repeat_count} iterations PASSED!")
        else:
            print(f"\n❌ {failed_iterations} iteration(s) FAILED")

    # メモリリークサマリー
    print("\n" + "=" * 70)
    print("OVERALL MEMORY LEAK SUMMARY")
    print("=" * 70)

    mem_diff_handles = mem_final.handle_count - mem_initial.handle_count
    mem_diff_size = mem_final.total_size - mem_initial.total_size

    print(f"\nOverall Memory Change (Initial → Final):")
    print(f"  Handles: {mem_diff_handles:+d} ({mem_initial.handle_count} → {mem_final.handle_count})")
    print(f"  Size:    {mem_diff_size:+d} bytes ({mem_diff_size/1024:+.2f} KB)")

    if overall_leak:
        print(f"\n⚠️  OVERALL MEMORY LEAK DETECTED")
        print(f"  Total memory growth exceeds acceptable threshold")
    else:
        print(f"\n✅ No significant overall memory leak")
        print(f"  Memory growth is within acceptable range")

    print(f"\nFinished at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 70)

    return {
        "all_iterations_results": all_iterations_results,
        "all_iterations_passed": all_iterations_passed,
        "all_iterations_failed": all_iterations_failed,
        "repeat_count": repeat_count,
        "failed_iterations": failed_iterations,
        "overall_memory_leak": overall_leak,
        "memory_initial": {"handles": mem_initial.handle_count, "size": mem_initial.total_size},
        "memory_final": {"handles": mem_final.handle_count, "size": mem_final.total_size},
    }


# メイン実行
if __name__ == "__main__":
    import os
    # 環境変数でターゲットを指定可能
    target = os.environ.get("PYAE_TEST_TARGET", "")
    if target:
        run_specific_tests(target)
    else:
        run_all_tests()
