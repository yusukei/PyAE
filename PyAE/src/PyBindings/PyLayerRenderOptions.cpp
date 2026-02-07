// PyLayerRenderOptions.cpp
// PyAE - Python for After Effects
// レイヤーレンダリングオプションクラスの実装
//
// LayerRenderOptionsSuite2 の高レベルAPI

#include "PyLayerRenderOptionsClasses.h"
#include "PluginState.h"
#include "AETypeUtils.h"
#include "ValidationUtils.h"

namespace PyAE {

// =============================================================
// PyLayerRenderOptions Implementation
// =============================================================

PyLayerRenderOptions::PyLayerRenderOptions()
    : m_optionsH(nullptr), m_owned(false) {}

PyLayerRenderOptions::PyLayerRenderOptions(AEGP_LayerRenderOptionsH optionsH, bool owned)
    : m_optionsH(optionsH), m_owned(owned) {}

PyLayerRenderOptions::PyLayerRenderOptions(PyLayerRenderOptions&& other) noexcept
    : m_optionsH(other.m_optionsH), m_owned(other.m_owned)
{
    other.m_optionsH = nullptr;
    other.m_owned = false;
}

PyLayerRenderOptions& PyLayerRenderOptions::operator=(PyLayerRenderOptions&& other) noexcept
{
    if (this != &other) {
        Dispose();
        m_optionsH = other.m_optionsH;
        m_owned = other.m_owned;
        other.m_optionsH = nullptr;
        other.m_owned = false;
    }
    return *this;
}

PyLayerRenderOptions::~PyLayerRenderOptions()
{
    Dispose();
}

void PyLayerRenderOptions::Dispose()
{
    if (m_optionsH && m_owned) {
        auto& state = PyAE::PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.layerRenderOptionsSuite) {
            suites.layerRenderOptionsSuite->AEGP_Dispose(m_optionsH);
        }
    }
    m_optionsH = nullptr;
    m_owned = false;
}

bool PyLayerRenderOptions::IsValid() const
{
    return m_optionsH != nullptr;
}

// =============================================================
// Time properties
// =============================================================

double PyLayerRenderOptions::GetTime() const
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    A_Time time = {0, 1};
    A_Err err = suites.layerRenderOptionsSuite->AEGP_GetTime(m_optionsH, &time);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetTime failed (error code: " + std::to_string(err) + ")");
    }

    return AETypeUtils::TimeToSeconds(time);
}

void PyLayerRenderOptions::SetTime(double seconds)
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    A_Time time = AETypeUtils::SecondsToTime(seconds);
    A_Err err = suites.layerRenderOptionsSuite->AEGP_SetTime(m_optionsH, time);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetTime failed (error code: " + std::to_string(err) + ")");
    }
}

double PyLayerRenderOptions::GetTimeStep() const
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    A_Time timeStep = {0, 1};
    A_Err err = suites.layerRenderOptionsSuite->AEGP_GetTimeStep(m_optionsH, &timeStep);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetTimeStep failed (error code: " + std::to_string(err) + ")");
    }

    return AETypeUtils::TimeToSeconds(timeStep);
}

void PyLayerRenderOptions::SetTimeStep(double seconds)
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    A_Time timeStep = AETypeUtils::SecondsToTime(seconds);
    A_Err err = suites.layerRenderOptionsSuite->AEGP_SetTimeStep(m_optionsH, timeStep);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetTimeStep failed (error code: " + std::to_string(err) + ")");
    }
}

// =============================================================
// Rendering properties
// =============================================================

WorldType PyLayerRenderOptions::GetWorldType() const
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    AEGP_WorldType type = AEGP_WorldType_NONE;
    A_Err err = suites.layerRenderOptionsSuite->AEGP_GetWorldType(m_optionsH, &type);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetWorldType failed (error code: " + std::to_string(err) + ")");
    }

    return static_cast<WorldType>(type);
}

void PyLayerRenderOptions::SetWorldType(WorldType type)
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    A_Err err = suites.layerRenderOptionsSuite->AEGP_SetWorldType(m_optionsH, static_cast<AEGP_WorldType>(type));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetWorldType failed (error code: " + std::to_string(err) + ")");
    }
}

std::tuple<int, int> PyLayerRenderOptions::GetDownsampleFactor() const
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    A_short x = 1, y = 1;
    A_Err err = suites.layerRenderOptionsSuite->AEGP_GetDownsampleFactor(m_optionsH, &x, &y);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetDownsampleFactor failed (error code: " + std::to_string(err) + ")");
    }

    return std::make_tuple(static_cast<int>(x), static_cast<int>(y));
}

void PyLayerRenderOptions::SetDownsampleFactor(int x, int y)
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");
    Validation::RequirePositive(x, "x");
    Validation::RequirePositive(y, "y");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    A_Err err = suites.layerRenderOptionsSuite->AEGP_SetDownsampleFactor(
        m_optionsH, static_cast<A_short>(x), static_cast<A_short>(y));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetDownsampleFactor failed (error code: " + std::to_string(err) + ")");
    }
}

MatteMode PyLayerRenderOptions::GetMatteMode() const
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    AEGP_MatteMode mode = AEGP_MatteMode_STRAIGHT;
    A_Err err = suites.layerRenderOptionsSuite->AEGP_GetMatteMode(m_optionsH, &mode);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetMatteMode failed (error code: " + std::to_string(err) + ")");
    }

    return static_cast<MatteMode>(mode);
}

void PyLayerRenderOptions::SetMatteMode(MatteMode mode)
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    A_Err err = suites.layerRenderOptionsSuite->AEGP_SetMatteMode(m_optionsH, static_cast<AEGP_MatteMode>(mode));
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_SetMatteMode failed (error code: " + std::to_string(err) + ")");
    }
}

// =============================================================
// Duplication
// =============================================================

std::shared_ptr<PyLayerRenderOptions> PyLayerRenderOptions::Duplicate() const
{
    if (!m_optionsH) throw std::runtime_error("Invalid LayerRenderOptions");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    AEGP_LayerRenderOptionsH copyH = nullptr;
    A_Err err = suites.layerRenderOptionsSuite->AEGP_Duplicate(state.GetPluginID(), m_optionsH, &copyH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_Duplicate failed (error code: " + std::to_string(err) + ")");
    }

    return std::make_shared<PyLayerRenderOptions>(copyH, true);
}

// =============================================================
// Handle access
// =============================================================

AEGP_LayerRenderOptionsH PyLayerRenderOptions::GetHandle() const
{
    return m_optionsH;
}

AEGP_LayerRenderOptionsH PyLayerRenderOptions::Release()
{
    AEGP_LayerRenderOptionsH h = m_optionsH;
    m_optionsH = nullptr;
    m_owned = false;
    return h;
}

bool PyLayerRenderOptions::IsOwned() const
{
    return m_owned;
}

// =============================================================
// Static factory methods
// =============================================================

std::shared_ptr<PyLayerRenderOptions> PyLayerRenderOptions::FromLayer(uintptr_t layerH_ptr)
{
    Validation::RequireNonNull(layerH_ptr, "layerH");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    AEGP_LayerH layerH = reinterpret_cast<AEGP_LayerH>(layerH_ptr);
    AEGP_LayerRenderOptionsH optionsH = nullptr;

    A_Err err = suites.layerRenderOptionsSuite->AEGP_NewFromLayer(
        state.GetPluginID(), layerH, &optionsH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewFromLayer failed (error code: " + std::to_string(err) + ")");
    }

    return std::make_shared<PyLayerRenderOptions>(optionsH, true);
}

std::shared_ptr<PyLayerRenderOptions> PyLayerRenderOptions::FromUpstreamOfEffect(uintptr_t effectH_ptr)
{
    Validation::RequireNonNull(effectH_ptr, "effectH");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
    AEGP_LayerRenderOptionsH optionsH = nullptr;

    A_Err err = suites.layerRenderOptionsSuite->AEGP_NewFromUpstreamOfEffect(
        state.GetPluginID(), effectH, &optionsH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewFromUpstreamOfEffect failed (error code: " + std::to_string(err) + ")");
    }

    return std::make_shared<PyLayerRenderOptions>(optionsH, true);
}

std::shared_ptr<PyLayerRenderOptions> PyLayerRenderOptions::FromDownstreamOfEffect(uintptr_t effectH_ptr)
{
    Validation::RequireNonNull(effectH_ptr, "effectH");

    auto& state = PyAE::PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.layerRenderOptionsSuite) throw std::runtime_error("LayerRenderOptions Suite not available");

    AEGP_EffectRefH effectH = reinterpret_cast<AEGP_EffectRefH>(effectH_ptr);
    AEGP_LayerRenderOptionsH optionsH = nullptr;

    A_Err err = suites.layerRenderOptionsSuite->AEGP_NewFromDownstreamOfEffect(
        state.GetPluginID(), effectH, &optionsH);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_NewFromDownstreamOfEffect failed (error code: " + std::to_string(err) + ")");
    }

    return std::make_shared<PyLayerRenderOptions>(optionsH, true);
}

// =============================================================
// Module init
// =============================================================

void init_layer_render_options(py::module_& m)
{
    // LayerRenderOptions class
    py::class_<PyLayerRenderOptions, std::shared_ptr<PyLayerRenderOptions>>(m, "LayerRenderOptions",
        R"doc(Layer rendering options for individual layer rendering.

Unlike RenderOptions which is for composition-level rendering,
LayerRenderOptions focuses on individual layer render control.

Example::

    # Create options from a layer
    layer = comp.layers[0]
    options = ae.LayerRenderOptions.from_layer(layer._handle)
    options.time = 2.5
    options.world_type = ae.WorldType.BIT16

    # Render the layer
    receipt = ae.Renderer.render_layer_frame(options._handle)
)doc")
        .def(py::init<>())
        .def_property_readonly("valid", &PyLayerRenderOptions::IsValid,
            "Check if the options handle is valid.")

        // Time properties
        .def_property("time",
            &PyLayerRenderOptions::GetTime,
            &PyLayerRenderOptions::SetTime,
            "Render time in seconds.")
        .def_property("time_step",
            &PyLayerRenderOptions::GetTimeStep,
            &PyLayerRenderOptions::SetTimeStep,
            "Time step for motion blur in seconds.")

        // Rendering properties
        .def_property("world_type",
            &PyLayerRenderOptions::GetWorldType,
            &PyLayerRenderOptions::SetWorldType,
            "World type (bit depth): NONE, BIT8, BIT16, or BIT32.")
        .def_property("downsample_factor",
            &PyLayerRenderOptions::GetDownsampleFactor,
            [](PyLayerRenderOptions& self, std::tuple<int, int> factor) {
                self.SetDownsampleFactor(std::get<0>(factor), std::get<1>(factor));
            },
            "Downsample factor as (x, y) tuple. 1=100%, 2=50%, etc.")
        .def_property("matte_mode",
            &PyLayerRenderOptions::GetMatteMode,
            &PyLayerRenderOptions::SetMatteMode,
            "Matte mode: STRAIGHT, PREMUL_BLACK, or PREMUL_BG_COLOR.")

        // Duplication
        .def("duplicate", &PyLayerRenderOptions::Duplicate,
            "Create a duplicate of these options.")

        // Handle access
        .def_property_readonly("_handle",
            [](const PyLayerRenderOptions& self) { return reinterpret_cast<uintptr_t>(self.GetHandle()); },
            "Internal: Get raw handle as integer.")

        // Static factory methods
        .def_static("from_layer", &PyLayerRenderOptions::FromLayer,
            py::arg("layer_handle"),
            R"doc(Create render options from a layer.

Args:
    layer_handle: Layer handle (int)

Returns:
    New LayerRenderOptions instance

Initial values:
    - Time: Layer's current time
    - TimeStep: Layer's frame duration
    - EffectsToRender: "all"
)doc")
        .def_static("from_upstream_of_effect", &PyLayerRenderOptions::FromUpstreamOfEffect,
            py::arg("effect_handle"),
            R"doc(Create render options from upstream of an effect.

Args:
    effect_handle: Effect handle (int)

Returns:
    New LayerRenderOptions instance

Note:
    EffectsToRender is set to the specified effect's index.
)doc")
        .def_static("from_downstream_of_effect", &PyLayerRenderOptions::FromDownstreamOfEffect,
            py::arg("effect_handle"),
            R"doc(Create render options from downstream of an effect.

Args:
    effect_handle: Effect handle (int)

Returns:
    New LayerRenderOptions instance

Warning:
    This function can only be called from the UI thread.
)doc");
}

} // namespace PyAE
