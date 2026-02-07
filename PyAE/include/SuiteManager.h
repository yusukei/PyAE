// SuiteManager.h
// PyAE - Python for After Effects
// AE SDKスイート管理クラス

#pragma once

#include <memory>
#include <functional>
#include "WinSync.h"

#include "AE_GeneralPlug.h"
#include "AE_EffectSuites.h"
#include "AE_GeneralPlugPanels.h"
#include "AEGP_SuiteHandler.h"
#include "AE_AdvEffectSuites.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace PyAE {

// =============================================================
// スイートキャッシュ構造体
// After Effectsの各種API (Suite) へのポインタをキャッシュ
// SDKバージョン依存のSuiteは SDKCompat:: の互換型を使用
// =============================================================
struct SuiteCache {
    // Version-independent Suites
    AEGP_MemorySuite1*      memorySuite = nullptr;
    AEGP_ProjSuite6*        projSuite = nullptr;
    AEGP_ItemSuite9*        itemSuite = nullptr;
    SDKCompat::LayerSuiteType* layerSuite = nullptr;     // SDK version-dependent (V9 or V8)
    SDKCompat::StreamSuiteType* streamSuite = nullptr;   // SDK version-dependent (V6 or V5)
    AEGP_DynamicStreamSuite4* dynamicStreamSuite = nullptr;
    SDKCompat::KeyframeSuiteType* keyframeSuite = nullptr; // SDK version-dependent (V5 or V4)
    AEGP_MarkerSuite3*      markerSuite = nullptr;
    AEGP_UtilitySuite6*     utilitySuite = nullptr;
    AEGP_RegisterSuite5*    registerSuite = nullptr;
    AEGP_CommandSuite1*     commandSuite = nullptr;
    AEGP_WorldSuite3*       worldSuite = nullptr;
    AEGP_FootageSuite5*     footageSuite = nullptr;
    AEGP_RenderSuite5*      renderSuite = nullptr;
    AEGP_CollectionSuite2*  collectionSuite = nullptr;
    AEGP_MaskSuite6*        maskSuite = nullptr;
    AEGP_MaskOutlineSuite3* maskOutlineSuite = nullptr;
    PF_PathDataSuite1*      pathDataSuite = nullptr;
    AEGP_TextDocumentSuite1* textDocSuite = nullptr;
    AEGP_TextLayerSuite1*   textLayerSuite = nullptr;
    AEGP_PanelSuite1*       panelSuite = nullptr;
    AEGP_RenderQueueSuite1* renderQueueSuite = nullptr;
    AEGP_RQItemSuite4*      rqItemSuite = nullptr;
    AEGP_OutputModuleSuite4* outputModuleSuite = nullptr;
    AEGP_CameraSuite2*      cameraSuite = nullptr;
    AEGP_SoundDataSuite1*   soundDataSuite = nullptr;
    AEGP_PersistentDataSuite4* persistentDataSuite = nullptr;
    AEGP_MathSuite1*        mathSuite = nullptr;
    AEGP_IterateSuite2*     iterateSuite = nullptr;
    AEGP_ItemViewSuite1*    itemViewSuite = nullptr;
    AEGP_RenderAsyncManagerSuite1* renderAsyncManagerSuite = nullptr;
    AEGP_CompositeSuite2*   compositeSuite = nullptr;
    AEGP_RenderOptionsSuite4* renderOptionsSuite = nullptr;
    AEGP_LayerRenderOptionsSuite2* layerRenderOptionsSuite = nullptr;
    AEGP_RenderQueueMonitorSuite1* renderQueueMonitorSuite = nullptr;

    // PF Advanced Suites (Effect helper suites accessible from AEGP context)
    PF_AdvAppSuite2*    advAppSuite = nullptr;
    PF_AdvItemSuite1*   advItemSuite = nullptr;

    // Version-dependent Suites (use SDKCompat types)
    SDKCompat::CompSuiteType*          compSuite = nullptr;
    SDKCompat::LightSuiteType*         lightSuite = nullptr;
    SDKCompat::IOInSuiteType*          ioInSuite = nullptr;
    SDKCompat::IOOutSuiteType*         ioOutSuite = nullptr;
    SDKCompat::ColorSettingsSuiteType* colorSettingsSuite = nullptr;
    SDKCompat::EffectSuiteType*        effectSuite = nullptr;

    // Note: QueryXformSuite excluded - Artisan API only, incompatible with AEGP
};

// =============================================================
// SuiteManager クラス
// AE SDKのスイートを一元管理
// =============================================================
class SuiteManager {
public:
    explicit SuiteManager(SPBasicSuite* basicSuite);
    ~SuiteManager();

    // スイートの取得・解放
    bool AcquireSuites();
    void ReleaseSuites();

    // スイートキャッシュへのアクセス（読み取り専用、スレッドセーフ）
    const SuiteCache& GetSuites() const {
        WinSharedLock lock(m_mutex);
        return m_suites;
    }

    // スイートキャッシュの更新（書き込み、スレッドセーフ）
    void UpdateSuites(const std::function<void(SuiteCache&)>& updater) {
        WinUniqueLock lock(m_mutex);
        updater(m_suites);
    }

    // SuiteHandlerへのアクセス
    AEGP_SuiteHandler& GetSuiteHandler();

private:
    SPBasicSuite* m_basicSuite;
    mutable WinSharedMutex m_mutex;
    SuiteCache m_suites;
    std::unique_ptr<AEGP_SuiteHandler> m_suiteHandler;
};

} // namespace PyAE
