// SuiteManager.cpp
// PyAE - Python for After Effects
// AE SDKスイート管理クラスの実装

#include "SuiteManager.h"
#include "Logger.h"
#include <stdexcept>

namespace PyAE {

SuiteManager::SuiteManager(SPBasicSuite* basicSuite)
    : m_basicSuite(basicSuite)
{
    if (!m_basicSuite) {
        throw std::runtime_error("SuiteManager: basicSuite is null");
    }

    // SuiteHandlerを作成
    try {
        m_suiteHandler = std::make_unique<AEGP_SuiteHandler>(m_basicSuite);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("SuiteManager: Failed to create SuiteHandler: ") + e.what());
    }
}

SuiteManager::~SuiteManager() {
    ReleaseSuites();
}

bool SuiteManager::AcquireSuites() {
    WinUniqueLock lock(m_mutex);

    if (!m_suiteHandler) {
        PYAE_LOG_ERROR("SuiteManager", "SuiteHandler not initialized");
        return false;
    }

    try {
        // 必須スイートを取得
        m_suites.memorySuite = m_suiteHandler->MemorySuite1();
        m_suites.projSuite = m_suiteHandler->ProjSuite6();
        m_suites.itemSuite = m_suiteHandler->ItemSuite9();
        // Comp Suite (SDK version-dependent)
        const void* tempCompSuite = nullptr;
        A_Err compErr = m_basicSuite->AcquireSuite(
            kAEGPCompSuite,
            SDKCompat::CompSuiteVersion,
            &tempCompSuite);
        if (compErr == A_Err_NONE && tempCompSuite) {
            m_suites.compSuite = const_cast<SDKCompat::CompSuiteType*>(
                static_cast<const SDKCompat::CompSuiteType*>(tempCompSuite));
        } else {
            m_suites.compSuite = nullptr;
        }
        // Layer Suite (SDK version-dependent)
        {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPLayerSuite,
                SDKCompat::LayerSuiteVersion,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.layerSuite = const_cast<SDKCompat::LayerSuiteType*>(
                    static_cast<const SDKCompat::LayerSuiteType*>(tempSuite));
            } else {
                m_suites.layerSuite = nullptr;
            }
        }

        // Stream Suite (SDK version-dependent)
        {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPStreamSuite,
                SDKCompat::StreamSuiteVersion,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.streamSuite = const_cast<SDKCompat::StreamSuiteType*>(
                    static_cast<const SDKCompat::StreamSuiteType*>(tempSuite));
            } else {
                m_suites.streamSuite = nullptr;
            }
        }

        // Dynamic Stream Suite (Manual Acquire)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPDynamicStreamSuite,
                kAEGPDynamicStreamSuiteVersion4,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.dynamicStreamSuite = const_cast<AEGP_DynamicStreamSuite4*>(
                    static_cast<const AEGP_DynamicStreamSuite4*>(tempSuite));
            } else {
                m_suites.dynamicStreamSuite = nullptr;
            }
        } catch (...) {
            m_suites.dynamicStreamSuite = nullptr;
        }

        // Keyframe Suite (SDK version-dependent)
        {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPKeyframeSuite,
                SDKCompat::KeyframeSuiteVersion,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.keyframeSuite = const_cast<SDKCompat::KeyframeSuiteType*>(
                    static_cast<const SDKCompat::KeyframeSuiteType*>(tempSuite));
            } else {
                m_suites.keyframeSuite = nullptr;
            }
        }

        // Marker Suite (Manual Acquire)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPMarkerSuite,
                kAEGPMarkerSuiteVersion3,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.markerSuite = const_cast<AEGP_MarkerSuite3*>(
                    static_cast<const AEGP_MarkerSuite3*>(tempSuite));
            } else {
                m_suites.markerSuite = nullptr;
            }
        } catch (...) {
            m_suites.markerSuite = nullptr;
        }

        m_suites.utilitySuite = m_suiteHandler->UtilitySuite6();
        m_suites.registerSuite = m_suiteHandler->RegisterSuite5();
        m_suites.commandSuite = m_suiteHandler->CommandSuite1();
        m_suites.worldSuite = m_suiteHandler->WorldSuite3();
        m_suites.footageSuite = m_suiteHandler->FootageSuite5();
        m_suites.collectionSuite = m_suiteHandler->CollectionSuite2();
        // Effect Suite (SDK version-dependent)
        {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPEffectSuite,
                SDKCompat::EffectSuiteVersion,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.effectSuite = const_cast<SDKCompat::EffectSuiteType*>(
                    static_cast<const SDKCompat::EffectSuiteType*>(tempSuite));
            } else {
                m_suites.effectSuite = nullptr;
            }
        }
        m_suites.maskSuite = m_suiteHandler->MaskSuite6();

        // オプショナルなスイート（取得失敗してもエラーにしない）
        try {
            m_suites.maskOutlineSuite = m_suiteHandler->MaskOutlineSuite3();
        } catch (...) {
            m_suites.maskOutlineSuite = nullptr;
        }

        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kPFPathDataSuite,
                kPFPathDataSuiteVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.pathDataSuite = const_cast<PF_PathDataSuite1*>(
                    static_cast<const PF_PathDataSuite1*>(tempSuite));
            } else {
                m_suites.pathDataSuite = nullptr;
            }
        } catch (...) {
            m_suites.pathDataSuite = nullptr;
        }

        try {
            m_suites.renderSuite = m_suiteHandler->RenderSuite5();
        } catch (...) {
            m_suites.renderSuite = nullptr;
        }

        try {
            m_suites.textDocSuite = m_suiteHandler->TextDocumentSuite1();
        } catch (...) {
            m_suites.textDocSuite = nullptr;
        }

        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPTextLayerSuite,
                kAEGPTextLayerSuiteVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.textLayerSuite = const_cast<AEGP_TextLayerSuite1*>(
                    static_cast<const AEGP_TextLayerSuite1*>(tempSuite));
            } else {
                m_suites.textLayerSuite = nullptr;
            }
        } catch (...) {
            m_suites.textLayerSuite = nullptr;
        }

        // Phase 5: Render Queue Suites
        try {
            m_suites.renderQueueSuite = m_suiteHandler->RenderQueueSuite1();
        } catch (...) {
            m_suites.renderQueueSuite = nullptr;
        }

        try {
            m_suites.outputModuleSuite = m_suiteHandler->OutputModuleSuite4();
        } catch (...) {
            m_suites.outputModuleSuite = nullptr;
        }

        // RQItemSuite4 を手動取得
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPRQItemSuite,
                kAEGPRQItemSuiteVersion4,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.rqItemSuite = const_cast<AEGP_RQItemSuite4*>(
                    static_cast<const AEGP_RQItemSuite4*>(tempSuite));
            } else {
                m_suites.rqItemSuite = nullptr;
            }
        } catch (...) {
            m_suites.rqItemSuite = nullptr;
        }

        // Camera Suite (Manual Acquire) - Testing individually
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPCameraSuite,
                kAEGPCameraSuiteVersion2,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.cameraSuite = const_cast<AEGP_CameraSuite2*>(
                    static_cast<const AEGP_CameraSuite2*>(tempSuite));
            } else {
                m_suites.cameraSuite = nullptr;
            }
        } catch (...) {
            m_suites.cameraSuite = nullptr;
        }

        // Light Suite (SDK version-dependent)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPLightSuite,
                SDKCompat::LightSuiteVersion,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.lightSuite = const_cast<SDKCompat::LightSuiteType*>(
                    static_cast<const SDKCompat::LightSuiteType*>(tempSuite));
            } else {
                m_suites.lightSuite = nullptr;
            }
        } catch (...) {
            m_suites.lightSuite = nullptr;
        }

        // IO In Suite (SDK version-dependent)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPIOInSuite,
                SDKCompat::IOInSuiteVersion,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.ioInSuite = const_cast<SDKCompat::IOInSuiteType*>(
                    static_cast<const SDKCompat::IOInSuiteType*>(tempSuite));
            } else {
                m_suites.ioInSuite = nullptr;
            }
        } catch (...) {
            m_suites.ioInSuite = nullptr;
        }

        // IO Out Suite (SDK version-dependent)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPIOOutSuite,
                SDKCompat::IOOutSuiteVersion,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.ioOutSuite = const_cast<SDKCompat::IOOutSuiteType*>(
                    static_cast<const SDKCompat::IOOutSuiteType*>(tempSuite));
            } else {
                m_suites.ioOutSuite = nullptr;
            }
        } catch (...) {
            m_suites.ioOutSuite = nullptr;
        }

        // Note: QueryXformSuite excluded - Artisan plugin API only
        // See docs/development/QueryXformSuite_exclusion_rationale.md

        // Sound Data Suite (Manual Acquire - Version 1)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPSoundDataSuite,
                kAEGPSoundDataVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.soundDataSuite = const_cast<AEGP_SoundDataSuite1*>(
                    static_cast<const AEGP_SoundDataSuite1*>(tempSuite));
            } else {
                m_suites.soundDataSuite = nullptr;
            }
        } catch (...) {
            m_suites.soundDataSuite = nullptr;
        }

        // Color Settings Suite (SDK version-dependent)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPColorSettingsSuite,
                SDKCompat::ColorSettingsSuiteVersion,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.colorSettingsSuite = const_cast<SDKCompat::ColorSettingsSuiteType*>(
                    static_cast<const SDKCompat::ColorSettingsSuiteType*>(tempSuite));
            } else {
                m_suites.colorSettingsSuite = nullptr;
            }
        } catch (...) {
            m_suites.colorSettingsSuite = nullptr;
        }

        // Persistent Data Suite (Manual Acquire - Version 4)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPPersistentDataSuite,
                kAEGPPersistentDataSuiteVersion4,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.persistentDataSuite = const_cast<AEGP_PersistentDataSuite4*>(
                    static_cast<const AEGP_PersistentDataSuite4*>(tempSuite));
            } else {
                m_suites.persistentDataSuite = nullptr;
            }
        } catch (...) {
            m_suites.persistentDataSuite = nullptr;
        }

        // Math Suite (Manual Acquire - Version 1)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPMathSuite,
                kAEGPMathSuiteVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.mathSuite = const_cast<AEGP_MathSuite1*>(
                    static_cast<const AEGP_MathSuite1*>(tempSuite));
            } else {
                m_suites.mathSuite = nullptr;
            }
        } catch (...) {
            m_suites.mathSuite = nullptr;
        }

        // PanelSuiteはSimpleSuiteHelperパターンで手動取得
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPPanelSuite,
                kAEGPPanelSuiteVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.panelSuite = const_cast<AEGP_PanelSuite1*>(
                    static_cast<const AEGP_PanelSuite1*>(tempSuite));
            } else {
                m_suites.panelSuite = nullptr;
                PYAE_LOG_WARNING("SuiteManager", "PanelSuite not available (may not be supported in this AE version)");
            }
        } catch (...) {
            m_suites.panelSuite = nullptr;
        }

        // Iterate Suite (Manual Acquire - Version 2)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPIterateSuite,
                kAEGPIterateSuiteVersion2,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.iterateSuite = const_cast<AEGP_IterateSuite2*>(
                    static_cast<const AEGP_IterateSuite2*>(tempSuite));
            } else {
                m_suites.iterateSuite = nullptr;
            }
        } catch (...) {
            m_suites.iterateSuite = nullptr;
        }

        // Item View Suite (Manual Acquire - Version 1, frozen in AE 13.6)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPItemViewSuite,
                kAEGPItemViewSuiteVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.itemViewSuite = const_cast<AEGP_ItemViewSuite1*>(
                    static_cast<const AEGP_ItemViewSuite1*>(tempSuite));
            } else {
                m_suites.itemViewSuite = nullptr;
            }
        } catch (...) {
            m_suites.itemViewSuite = nullptr;
        }

        // Render Async Manager Suite (Manual Acquire - Version 1, frozen in AE 13.5)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPRenderAsyncManagerSuite,
                kAEGPRenderAsyncManagerSuiteVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.renderAsyncManagerSuite = const_cast<AEGP_RenderAsyncManagerSuite1*>(
                    static_cast<const AEGP_RenderAsyncManagerSuite1*>(tempSuite));
            } else {
                m_suites.renderAsyncManagerSuite = nullptr;
            }
        } catch (...) {
            m_suites.renderAsyncManagerSuite = nullptr;
        }

        // Composite Suite (Manual Acquire - Version 2, frozen in AE 10.0)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPCompositeSuite,
                kAEGPCompositeSuiteVersion2,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.compositeSuite = const_cast<AEGP_CompositeSuite2*>(
                    static_cast<const AEGP_CompositeSuite2*>(tempSuite));
            } else {
                m_suites.compositeSuite = nullptr;
            }
        } catch (...) {
            m_suites.compositeSuite = nullptr;
        }

        // Render Options Suite (Manual Acquire - Version 4, frozen in AE 10.5)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPRenderOptionsSuite,
                kAEGPRenderOptionsSuiteVersion4,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.renderOptionsSuite = const_cast<AEGP_RenderOptionsSuite4*>(
                    static_cast<const AEGP_RenderOptionsSuite4*>(tempSuite));
            } else {
                m_suites.renderOptionsSuite = nullptr;
            }
        } catch (...) {
            m_suites.renderOptionsSuite = nullptr;
        }

        // Layer Render Options Suite (Manual Acquire - Version 2, frozen in AE 13.5)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPLayerRenderOptionsSuite,
                kAEGPLayerRenderOptionsSuiteVersion2,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.layerRenderOptionsSuite = const_cast<AEGP_LayerRenderOptionsSuite2*>(
                    static_cast<const AEGP_LayerRenderOptionsSuite2*>(tempSuite));
            } else {
                m_suites.layerRenderOptionsSuite = nullptr;
            }
        } catch (...) {
            m_suites.layerRenderOptionsSuite = nullptr;
        }

        // Render Queue Monitor Suite (Manual Acquire - Version 1, frozen in AE 11.0)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kAEGPRenderQueueMonitorSuite,
                kAEGPRenderQueueMonitorSuiteVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.renderQueueMonitorSuite = const_cast<AEGP_RenderQueueMonitorSuite1*>(
                    static_cast<const AEGP_RenderQueueMonitorSuite1*>(tempSuite));
            } else {
                m_suites.renderQueueMonitorSuite = nullptr;
            }
        } catch (...) {
            m_suites.renderQueueMonitorSuite = nullptr;
        }

        // PF Adv App Suite (Manual Acquire - Version 2, frozen in AE 6.0)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kPFAdvAppSuite,
                kPFAdvAppSuiteVersion2,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.advAppSuite = const_cast<PF_AdvAppSuite2*>(
                    static_cast<const PF_AdvAppSuite2*>(tempSuite));
            } else {
                m_suites.advAppSuite = nullptr;
            }
        } catch (...) {
            m_suites.advAppSuite = nullptr;
        }

        // PF Adv Item Suite (Manual Acquire - Version 1, frozen in AE 5.0)
        try {
            const void* tempSuite = nullptr;
            A_Err err = m_basicSuite->AcquireSuite(
                kPFAdvItemSuite,
                kPFAdvItemSuiteVersion1,
                &tempSuite);
            if (err == A_Err_NONE && tempSuite) {
                m_suites.advItemSuite = const_cast<PF_AdvItemSuite1*>(
                    static_cast<const PF_AdvItemSuite1*>(tempSuite));
            } else {
                m_suites.advItemSuite = nullptr;
            }
        } catch (...) {
            m_suites.advItemSuite = nullptr;
        }

        PYAE_LOG_INFO("SuiteManager", "All suites acquired successfully");
        return true;

    } catch (const std::exception& e) {
        PYAE_LOG_ERROR("SuiteManager", std::string("Failed to acquire suites: ") + e.what());
        return false;
    }
}

void SuiteManager::ReleaseSuites() {
    WinUniqueLock lock(m_mutex);

    // スイートポインタをクリア（実際の解放はSuiteHandlerが行う）
    m_suites = SuiteCache{};

    PYAE_LOG_INFO("SuiteManager", "Suites released");
}

AEGP_SuiteHandler& SuiteManager::GetSuiteHandler() {
    WinSharedLock lock(m_mutex);
    if (!m_suiteHandler) {
        throw std::runtime_error("SuiteHandler not initialized");
    }
    return *m_suiteHandler;
}

} // namespace PyAE
