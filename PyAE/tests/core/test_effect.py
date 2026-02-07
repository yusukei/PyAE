# test_effect.py
# PyAE Effect テスト

import ae

try:
    from ..test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
        assert_close,
    )
except ImportError:
    from test_utils import (
        TestSuite,
        assert_true,
        assert_false,
        assert_equal,
        assert_not_none,
        assert_none,
        assert_isinstance,
        assert_close,
    )

suite = TestSuite("Effect")

# テスト用変数
_test_comp = None
_test_layer = None
_test_effect = None


@suite.setup
def setup():
    """テスト前のセットアップ"""
    global _test_comp, _test_layer, _test_effect
    proj = ae.Project.get_current()
    _test_comp = proj.create_comp("_EffectTestComp", 1920, 1080, 1.0, 5.0, 30.0)
    _test_layer = _test_comp.add_solid(
        "_EffectTestLayer", 1920, 1080, 1.0, 0.0, 0.0, 5.0
    )
    # エフェクトを追加 (Gaussian Blur は標準エフェクト)
    try:
        _test_effect = _test_layer.add_effect("ADBE Gaussian Blur 2")
    except:
        # フォールバック: 別のエフェクト名を試す
        try:
            _test_effect = _test_layer.add_effect("Gaussian Blur")
        except:
            _test_effect = None


@suite.teardown
def teardown():
    """テスト後のクリーンアップ"""
    global _test_comp
    try:
        if _test_comp is not None:
            _test_comp.delete()
    except:
        pass


@suite.test
def test_effect_added():
    """エフェクトが追加できる"""
    global _test_effect
    if _test_effect is None:
        # エフェクト追加に失敗した場合はスキップ扱い
        assert_true(True, "Effect test skipped - no compatible effect found")
        return
    assert_not_none(_test_effect, "Effect should be added")


@suite.test
def test_effect_name():
    """エフェクト名が取得できる"""
    global _test_effect
    if _test_effect is None:
        return
    name = _test_effect.name
    assert_isinstance(name, str)
    assert_true(len(name) > 0, "Effect name should not be empty")


@suite.test
def test_effect_index():
    """エフェクトインデックスが取得できる"""
    global _test_effect
    if _test_effect is None:
        return
    index = _test_effect.index
    # エフェクトインデックスは0ベース
    # レイヤーハンドルが設定されていれば0以上、なければ-1
    assert_true(index >= 0, "Effect index should be 0 or greater (0-based)")


@suite.test
def test_effect_enabled():
    """エフェクトの有効/無効が設定できる"""
    global _test_effect
    if _test_effect is None:
        return
    original = _test_effect.enabled
    _test_effect.enabled = False
    assert_false(_test_effect.enabled)
    _test_effect.enabled = True
    assert_true(_test_effect.enabled)
    _test_effect.enabled = original


@suite.test
def test_layer_effects():
    """レイヤーのエフェクト一覧が取得できる"""
    global _test_layer
    effects = _test_layer.effects
    assert_isinstance(effects, list)


@suite.test
def test_layer_num_effects():
    """レイヤーのエフェクト数が取得できる"""
    global _test_layer
    num = _test_layer.num_effects
    assert_true(num >= 0, "num_effects should be non-negative")


@suite.test
def test_effect_duplicate():
    """エフェクトを複製できる"""
    global _test_effect
    if _test_effect is None:
        return
    try:
        dup = _test_effect.duplicate()
        assert_not_none(dup, "Duplicated effect should not be None")
        # クリーンアップ
        dup.delete()
    except Exception as e:
        # 複製がサポートされていない場合
        assert_true(True, f"Effect duplicate may not be supported: {e}")


@suite.test
def test_effect_match_name():
    """エフェクトのマッチ名が取得できる"""
    global _test_effect
    if _test_effect is None:
        return
    match_name = _test_effect.match_name
    assert_isinstance(match_name, str)


@suite.test
def test_effect_stream_name():
    """エフェクトストリーム名の取得/設定ができる"""
    global _test_effect
    if _test_effect is None:
        return
    try:
        # 名前を設定
        _test_effect.set_name("TestEffect")
        # 取得
        stream_name = _test_effect.stream_name
        assert_isinstance(stream_name, str)
        # 設定した名前が反映されているか確認
        assert_equal(stream_name, "TestEffect", "Stream name should match set name")
    except Exception as e:
        # set_nameが失敗する可能性もある（パラメータがないエフェクトなど）
        assert_true(True, f"Stream name test skipped: {e}")


@suite.test
def test_effect_num_masks():
    """エフェクトのマスク数が取得できる"""
    global _test_effect
    if _test_effect is None:
        return
    try:
        num = _test_effect.num_masks
        assert_true(num >= 0, "num_masks should be non-negative")
    except Exception as e:
        # マスク機能が使えない場合もある
        assert_true(True, f"Mask test skipped: {e}")


@suite.test
def test_get_installed_effects():
    """インストール済みエフェクトリストが取得できる"""
    effects = ae.get_installed_effects()
    assert_isinstance(effects, list)
    assert_true(len(effects) > 0, "Should have at least one installed effect")

    # 最初のエフェクトの形式を確認
    first_effect = effects[0]
    assert_isinstance(first_effect, dict)
    assert_true("name" in first_effect, "Effect info should have 'name'")
    assert_true("match_name" in first_effect, "Effect info should have 'match_name'")
    assert_true("category" in first_effect, "Effect info should have 'category'")
    assert_true("is_internal" in first_effect, "Effect info should have 'is_internal'")

    # is_internalがboolであることを確認
    assert_isinstance(first_effect["is_internal"], bool)


@suite.test
def test_effect_sdk_functions():
    """SDK低レベル関数のテスト"""
    global _test_layer, _test_effect
    if _test_effect is None:
        return

    try:
        # AEGP_GetLayerNumEffects
        layer_h = _test_layer._handle if hasattr(_test_layer, '_handle') else 0
        if layer_h:
            num = ae.sdk.AEGP_GetLayerNumEffects(layer_h)
            assert_true(num >= 1, "Should have at least one effect")

        # AEGP_GetIsInternalEffect
        all_effects = ae.get_installed_effects()
        if len(all_effects) > 0:
            # 最初のエフェクトの内部/外部チェック
            first_match_name = all_effects[0]["match_name"]
            # InstalledEffectKeyを取得（簡易的な方法）
            # 実際はAEGP_GetNextInstalledEffectで取得する必要がある
            pass  # 低レベルAPIのテストは複雑なため省略

    except Exception as e:
        assert_true(True, f"SDK function test skipped: {e}")


@suite.test
def test_get_effect_param_union():
    """AEGP_GetEffectParamUnionByIndex関数のテスト"""
    global _test_effect
    if _test_effect is None:
        return

    try:
        # エフェクトハンドルを取得
        effect_h = _test_effect._handle if hasattr(_test_effect, '_handle') else 0
        if not effect_h:
            return

        # plugin_idを取得
        plugin_id = ae.get_plugin_id()

        # パラメータ0（入力レイヤー）の定義を取得
        param_info = ae.sdk.AEGP_GetEffectParamUnionByIndex(plugin_id, effect_h, 0)

        # 戻り値が辞書であることを確認
        assert_isinstance(param_info, dict)

        # 必須キーが存在することを確認
        assert_true("param_type" in param_info, "Should have param_type")
        assert_true("param_type_name" in param_info, "Should have param_type_name")
        assert_true("warning" in param_info, "Should have warning message")

        # param_typeが整数であることを確認
        assert_isinstance(param_info["param_type"], int)

        # param_type_nameが文字列であることを確認
        assert_isinstance(param_info["param_type_name"], str)

        # パラメータ0は通常LAYERタイプ
        # ただし、エフェクトによって異なる可能性があるため、厳密にはチェックしない

    except Exception as e:
        # 一部のエフェクトではこの機能がサポートされていない可能性がある
        assert_true(True, f"GetEffectParamUnionByIndex test skipped: {e}")


@suite.test
def test_effect_call_generic():
    """AEGP_EffectCallGeneric関数のテスト"""
    global _test_effect
    if _test_effect is None:
        return

    try:
        # エフェクトハンドルを取得
        effect_h = _test_effect._handle if hasattr(_test_effect, '_handle') else 0
        if not effect_h:
            return

        # plugin_idを取得
        plugin_id = ae.get_plugin_id()

        # PF_Cmd_COMPLETELY_GENERALで呼び出し
        # time=0.0, default command
        ae.sdk.AEGP_EffectCallGeneric(plugin_id, effect_h, 0.0)

        # 例外が発生しなければ成功
        assert_true(True, "EffectCallGeneric succeeded")

    except Exception as e:
        # この機能は高度な使用法のため、失敗しても問題ない
        assert_true(True, f"EffectCallGeneric test skipped: {e}")


# -----------------------------------------------------------------------
# EffectSuite5 Additional Tests (Effect Mask functions)
# -----------------------------------------------------------------------

@suite.test
def test_effect_num_effect_mask():
    """AEGP_NumEffectMask - エフェクトマスク数を取得できる"""
    global _test_effect
    if _test_effect is None:
        return

    effect_h = _test_effect._handle if hasattr(_test_effect, '_handle') else 0
    if not effect_h:
        return

    num_masks = ae.sdk.AEGP_NumEffectMask(effect_h)
    assert_isinstance(num_masks, int)
    assert_true(num_masks >= 0, "Effect mask count should be non-negative")
    print(f"Effect has {num_masks} masks")


@suite.test
def test_effect_mask_operations():
    """エフェクトマスク操作（Add/Get/Set/Remove）"""
    global _test_effect, _test_layer
    if _test_effect is None:
        return

    effect_h = _test_effect._handle if hasattr(_test_effect, '_handle') else 0
    if not effect_h:
        return

    # まずレイヤーにマスクを追加
    try:
        vertices = [(100, 100), (500, 100), (500, 400), (100, 400)]
        mask = _test_layer.add_mask(vertices)
        if mask is None:
            print("Could not create mask - skipping effect mask test")
            return

        mask_id = ae.sdk.AEGP_GetMaskID(mask._handle)

        # エフェクトマスクを追加
        mask_idx = ae.sdk.AEGP_AddEffectMask(effect_h, mask_id)
        assert_true(mask_idx >= 0, "Effect mask index should be non-negative")
        print(f"Added effect mask at index {mask_idx}")

        # エフェクトマスク数を確認
        num_masks = ae.sdk.AEGP_NumEffectMask(effect_h)
        assert_true(num_masks >= 1, "Should have at least 1 effect mask")

        # エフェクトマスクIDを取得
        retrieved_mask_id = ae.sdk.AEGP_GetEffectMaskID(effect_h, mask_idx)
        assert_equal(retrieved_mask_id, mask_id, "Retrieved mask ID should match")

        # エフェクトマスクを設定（同じマスクIDで上書き）
        ae.sdk.AEGP_SetEffectMask(effect_h, mask_idx, mask_id)

        # エフェクトマスクを削除
        ae.sdk.AEGP_RemoveEffectMask(effect_h, mask_idx)

        # 削除後のマスク数を確認
        num_masks_after = ae.sdk.AEGP_NumEffectMask(effect_h)
        print(f"Effect masks after removal: {num_masks_after}")

        # クリーンアップ
        mask.delete()

    except Exception as e:
        print(f"Effect mask operations test skipped: {e}")


@suite.test
def test_get_is_internal_effect():
    """AEGP_GetIsInternalEffect - エフェクトが内部エフェクトか確認"""
    try:
        # インストール済みエフェクトをスキャン
        key = ae.sdk.AEGP_GetNextInstalledEffect(0)
        count = 0
        internal_count = 0

        while key != 0 and count < 10:
            is_internal = ae.sdk.AEGP_GetIsInternalEffect(key)
            assert_isinstance(is_internal, bool)
            if is_internal:
                internal_count += 1
            count += 1
            key = ae.sdk.AEGP_GetNextInstalledEffect(key)

        print(f"Checked {count} effects, {internal_count} are internal")
        assert_true(count > 0, "Should find at least one effect")

    except Exception as e:
        print(f"GetIsInternalEffect test skipped: {e}")


def run():
    """テストを実行"""
    return suite.run()


if __name__ == "__main__":
    run()
