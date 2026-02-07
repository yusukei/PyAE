// PyLayer_Effect.cpp
// PyAE - Python for After Effects
// PyLayer エフェクト操作実装

#include "PyLayerClasses.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// PyLayer エフェクト操作実装
// =============================================================

int PyLayer::GetNumEffects() const {
    if (!m_layerH) return 0;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    A_long numEffects;
    A_Err err = suites.effectSuite->AEGP_GetLayerNumEffects(m_layerH, &numEffects);
    if (err != A_Err_NONE) return 0;

    return static_cast<int>(numEffects);
}

py::list PyLayer::GetAllEffects() {
    py::list effects;

    if (!m_layerH) return effects;

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    int numEffects = GetNumEffects();
    for (int i = 0; i < numEffects; ++i) {
        try {
            AEGP_EffectRefH effectH;
            A_Err err = suites.effectSuite->AEGP_GetLayerEffectByIndex(
                state.GetPluginID(), m_layerH, static_cast<A_long>(i), &effectH);
            if (err == A_Err_NONE && effectH) {
                // Pythonモジュールからae.Effectクラスを取得してインスタンス化
                py::module_ ae = py::module_::import("ae");
                py::object effectClass = ae.attr("Effect");
                // _create_from_handleという内部メソッドを使用（後で追加）
                // または、EffectRefHをuintptr_tとして渡す
                py::object effect = effectClass.attr("_create_from_handle")(
                    reinterpret_cast<uintptr_t>(effectH),
                    reinterpret_cast<uintptr_t>(m_layerH),
                    i);  // Pass the index
                effects.append(effect);
            }
        } catch (const std::exception& e) {
            PYAE_LOG_WARNING("Layer", std::string("Failed to get effect at index ") + std::to_string(i) + ": " + e.what());
            throw;  // Re-throw to propagate error
        }
    }

    return effects;
}

py::object PyLayer::GetEffectByIndex(int index) {
    if (!m_layerH) {
        return py::none();
    }

    int numEffects = GetNumEffects();
    if (index < 0 || index >= numEffects) {
        return py::none();
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    try {
        AEGP_EffectRefH effectH;
        A_Err err = suites.effectSuite->AEGP_GetLayerEffectByIndex(
            state.GetPluginID(), m_layerH, static_cast<A_long>(index), &effectH);
        if (err == A_Err_NONE && effectH) {
            py::module_ ae = py::module_::import("ae");
            py::object effectClass = ae.attr("Effect");
            return effectClass.attr("_create_from_handle")(
                reinterpret_cast<uintptr_t>(effectH),
                reinterpret_cast<uintptr_t>(m_layerH),
                index);  // Pass the index
        }
    } catch (const std::exception& e) {
        PYAE_LOG_WARNING("Layer", std::string("Failed to get effect by index: ") + e.what());
    }

    return py::none();
}

py::object PyLayer::GetEffectByName(const std::string& name) {
    if (!m_layerH) {
        return py::none();
    }

    // 全エフェクトを取得して名前で検索
    py::list effects = GetAllEffects();
    for (auto effect : effects) {
        try {
            std::string effectName = effect.attr("name").cast<std::string>();
            if (effectName == name) {
                return py::reinterpret_borrow<py::object>(effect);
            }
        } catch (const std::exception&) {
            continue;
        }
    }

    return py::none();
}

py::object PyLayer::AddEffect(const std::string& matchName) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    // Use the helper function from PyEffect.cpp
    return AddEffectToLayer(m_layerH, matchName);
}

} // namespace PyAE
