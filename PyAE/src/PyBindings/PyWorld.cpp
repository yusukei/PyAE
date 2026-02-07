// PyWorld.cpp
// PyAE - Python for After Effects
// フレームバッファ（World）クラスの実装
//
// WorldSuite3の高レベルAPI

#include "PyWorldClasses.h"
#include "PluginState.h"
#include "ScopedHandles.h"

namespace PyAE {

// Helper function to clamp a value between 0 and 1
// (avoids Windows min/max macro conflicts)
static inline float ClampNormalized(float val)
{
    if (val < 0.0f) return 0.0f;
    if (val > 1.0f) return 1.0f;
    return val;
}

// =============================================================
// PyWorld Implementation
// =============================================================

PyWorld::PyWorld()
    : m_worldH(nullptr)
    , m_owned(false)
{
}

PyWorld::PyWorld(AEGP_WorldH worldH, bool owned)
    : m_worldH(worldH)
    , m_owned(owned)
{
}

PyWorld::PyWorld(PyWorld&& other) noexcept
    : m_worldH(other.m_worldH)
    , m_owned(other.m_owned)
{
    other.m_worldH = nullptr;
    other.m_owned = false;
}

PyWorld& PyWorld::operator=(PyWorld&& other) noexcept
{
    if (this != &other) {
        Dispose();
        m_worldH = other.m_worldH;
        m_owned = other.m_owned;
        other.m_worldH = nullptr;
        other.m_owned = false;
    }
    return *this;
}

PyWorld::~PyWorld()
{
    Dispose();
}

void PyWorld::Dispose()
{
    if (m_worldH && m_owned) {
        auto& state = PluginState::Instance();
        const auto& suites = state.GetSuites();
        if (suites.worldSuite) {
            suites.worldSuite->AEGP_Dispose(m_worldH);
        }
        m_worldH = nullptr;
        m_owned = false;
    }
}

bool PyWorld::IsValid() const
{
    return m_worldH != nullptr;
}

WorldType PyWorld::GetType() const
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.worldSuite) {
        throw std::runtime_error("World Suite not available");
    }

    AEGP_WorldType type = AEGP_WorldType_NONE;
    A_Err err = suites.worldSuite->AEGP_GetType(m_worldH, &type);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetType failed");
    }

    return static_cast<WorldType>(type);
}

int PyWorld::GetWidth() const
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.worldSuite) {
        throw std::runtime_error("World Suite not available");
    }

    A_long width = 0;
    A_long height = 0;
    A_Err err = suites.worldSuite->AEGP_GetSize(m_worldH, &width, &height);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetSize failed");
    }

    return static_cast<int>(width);
}

int PyWorld::GetHeight() const
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.worldSuite) {
        throw std::runtime_error("World Suite not available");
    }

    A_long width = 0;
    A_long height = 0;
    A_Err err = suites.worldSuite->AEGP_GetSize(m_worldH, &width, &height);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetSize failed");
    }

    return static_cast<int>(height);
}

std::tuple<int, int> PyWorld::GetSize() const
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.worldSuite) {
        throw std::runtime_error("World Suite not available");
    }

    A_long width = 0;
    A_long height = 0;
    A_Err err = suites.worldSuite->AEGP_GetSize(m_worldH, &width, &height);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetSize failed");
    }

    return std::make_tuple(static_cast<int>(width), static_cast<int>(height));
}

int PyWorld::GetRowBytes() const
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.worldSuite) {
        throw std::runtime_error("World Suite not available");
    }

    A_u_long rowbytes = 0;
    A_Err err = suites.worldSuite->AEGP_GetRowBytes(m_worldH, &rowbytes);
    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_GetRowBytes failed");
    }

    return static_cast<int>(rowbytes);
}

void* PyWorld::GetBaseAddr() const
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.worldSuite) {
        throw std::runtime_error("World Suite not available");
    }

    WorldType type = GetType();
    void* baseAddr = nullptr;
    A_Err err = A_Err_NONE;

    switch (type) {
        case WorldType::BIT8: {
            PF_Pixel8* addr8 = nullptr;
            err = suites.worldSuite->AEGP_GetBaseAddr8(m_worldH, &addr8);
            baseAddr = addr8;
            break;
        }
        case WorldType::BIT16: {
            PF_Pixel16* addr16 = nullptr;
            err = suites.worldSuite->AEGP_GetBaseAddr16(m_worldH, &addr16);
            baseAddr = addr16;
            break;
        }
        case WorldType::BIT32: {
            PF_PixelFloat* addr32 = nullptr;
            err = suites.worldSuite->AEGP_GetBaseAddr32(m_worldH, &addr32);
            baseAddr = addr32;
            break;
        }
        default:
            throw std::runtime_error("Unsupported world type");
    }

    if (err != A_Err_NONE) {
        throw std::runtime_error("Failed to get base address");
    }

    return baseAddr;
}

py::bytes PyWorld::GetPixels() const
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    void* baseAddr = GetBaseAddr();
    if (!baseAddr) {
        throw std::runtime_error("Failed to get base address");
    }

    int rowBytes = GetRowBytes();
    int height = GetHeight();
    size_t totalBytes = static_cast<size_t>(rowBytes) * static_cast<size_t>(height);

    return py::bytes(static_cast<const char*>(baseAddr), totalBytes);
}

void PyWorld::SetPixels(const py::bytes& data)
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    void* baseAddr = GetBaseAddr();
    if (!baseAddr) {
        throw std::runtime_error("Failed to get base address");
    }

    int rowBytes = GetRowBytes();
    int height = GetHeight();
    size_t totalBytes = static_cast<size_t>(rowBytes) * static_cast<size_t>(height);

    std::string dataStr = data;
    if (dataStr.size() != totalBytes) {
        throw std::runtime_error("Data size mismatch. Expected " +
            std::to_string(totalBytes) + " bytes, got " +
            std::to_string(dataStr.size()));
    }

    std::memcpy(baseAddr, dataStr.data(), totalBytes);
}

std::tuple<float, float, float, float> PyWorld::GetPixel(int x, int y) const
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    int width = GetWidth();
    int height = GetHeight();
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::runtime_error("Pixel coordinates out of range");
    }

    void* baseAddr = GetBaseAddr();
    if (!baseAddr) {
        throw std::runtime_error("Failed to get base address");
    }

    int rowBytes = GetRowBytes();
    WorldType type = GetType();

    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

    switch (type) {
        case WorldType::BIT8: {
            // PF_Pixel8: A, R, G, B (each 8-bit)
            PF_Pixel8* row = reinterpret_cast<PF_Pixel8*>(
                static_cast<char*>(baseAddr) + y * rowBytes);
            PF_Pixel8& pixel = row[x];
            a = pixel.alpha / 255.0f;
            r = pixel.red / 255.0f;
            g = pixel.green / 255.0f;
            b = pixel.blue / 255.0f;
            break;
        }
        case WorldType::BIT16: {
            // PF_Pixel16: A, R, G, B (each 16-bit, max 32768)
            PF_Pixel16* row = reinterpret_cast<PF_Pixel16*>(
                static_cast<char*>(baseAddr) + y * rowBytes);
            PF_Pixel16& pixel = row[x];
            a = pixel.alpha / 32768.0f;
            r = pixel.red / 32768.0f;
            g = pixel.green / 32768.0f;
            b = pixel.blue / 32768.0f;
            break;
        }
        case WorldType::BIT32: {
            // PF_PixelFloat: A, R, G, B (each float)
            PF_PixelFloat* row = reinterpret_cast<PF_PixelFloat*>(
                static_cast<char*>(baseAddr) + y * rowBytes);
            PF_PixelFloat& pixel = row[x];
            a = pixel.alpha;
            r = pixel.red;
            g = pixel.green;
            b = pixel.blue;
            break;
        }
        default:
            throw std::runtime_error("Unsupported world type");
    }

    return std::make_tuple(r, g, b, a);
}

void PyWorld::SetPixel(int x, int y, float r, float g, float b, float a)
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    int width = GetWidth();
    int height = GetHeight();
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::runtime_error("Pixel coordinates out of range");
    }

    void* baseAddr = GetBaseAddr();
    if (!baseAddr) {
        throw std::runtime_error("Failed to get base address");
    }

    int rowBytes = GetRowBytes();
    WorldType type = GetType();

    switch (type) {
        case WorldType::BIT8: {
            PF_Pixel8* row = reinterpret_cast<PF_Pixel8*>(
                static_cast<char*>(baseAddr) + y * rowBytes);
            PF_Pixel8& pixel = row[x];
            pixel.alpha = static_cast<A_u_char>(ClampNormalized(a) * 255.0f);
            pixel.red = static_cast<A_u_char>(ClampNormalized(r) * 255.0f);
            pixel.green = static_cast<A_u_char>(ClampNormalized(g) * 255.0f);
            pixel.blue = static_cast<A_u_char>(ClampNormalized(b) * 255.0f);
            break;
        }
        case WorldType::BIT16: {
            PF_Pixel16* row = reinterpret_cast<PF_Pixel16*>(
                static_cast<char*>(baseAddr) + y * rowBytes);
            PF_Pixel16& pixel = row[x];
            pixel.alpha = static_cast<A_u_short>(ClampNormalized(a) * 32768.0f);
            pixel.red = static_cast<A_u_short>(ClampNormalized(r) * 32768.0f);
            pixel.green = static_cast<A_u_short>(ClampNormalized(g) * 32768.0f);
            pixel.blue = static_cast<A_u_short>(ClampNormalized(b) * 32768.0f);
            break;
        }
        case WorldType::BIT32: {
            PF_PixelFloat* row = reinterpret_cast<PF_PixelFloat*>(
                static_cast<char*>(baseAddr) + y * rowBytes);
            PF_PixelFloat& pixel = row[x];
            pixel.alpha = a;
            pixel.red = r;
            pixel.green = g;
            pixel.blue = b;
            break;
        }
        default:
            throw std::runtime_error("Unsupported world type");
    }
}

void PyWorld::FastBlur(float radius, int flags, int quality)
{
    if (!m_worldH) {
        throw std::runtime_error("Invalid world");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.worldSuite) {
        throw std::runtime_error("World Suite not available");
    }

    if (radius < 0) {
        throw std::runtime_error("Blur radius cannot be negative");
    }
    if (quality < 0 || quality > 5) {
        throw std::runtime_error("Quality must be between 0 and 5");
    }

    A_Err err = suites.worldSuite->AEGP_FastBlur(
        static_cast<A_FpLong>(radius),
        static_cast<PF_ModeFlags>(flags),
        static_cast<PF_Quality>(quality),
        m_worldH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_FastBlur failed");
    }
}

AEGP_WorldH PyWorld::GetHandle() const
{
    return m_worldH;
}

AEGP_WorldH PyWorld::Release()
{
    AEGP_WorldH handle = m_worldH;
    m_worldH = nullptr;
    m_owned = false;
    return handle;
}

bool PyWorld::IsOwned() const
{
    return m_owned;
}

// Static factory methods

std::shared_ptr<PyWorld> PyWorld::Create(WorldType type, int width, int height)
{
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Dimensions must be positive");
    }
    if (width > 30000 || height > 30000) {
        throw std::runtime_error("Dimensions exceed maximum (30000)");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();
    if (!suites.worldSuite) {
        throw std::runtime_error("World Suite not available");
    }

    AEGP_PluginID plugin_id = state.GetPluginID();
    AEGP_WorldH worldH = nullptr;

    A_Err err = suites.worldSuite->AEGP_New(
        plugin_id,
        static_cast<AEGP_WorldType>(type),
        static_cast<A_long>(width),
        static_cast<A_long>(height),
        &worldH);

    if (err != A_Err_NONE) {
        throw std::runtime_error("AEGP_New failed");
    }

    return std::make_shared<PyWorld>(worldH, true);
}

// =============================================================
// Python Bindings
// =============================================================

void init_world(py::module_& m)
{
    // WorldType enum
    py::enum_<WorldType>(m, "WorldType",
        "Pixel bit depth for World (frame buffer).\n\n"
        "Values:\n"
        "    NONE: Invalid/no world\n"
        "    BIT8: 8 bits per channel\n"
        "    BIT16: 16 bits per channel\n"
        "    BIT32: 32 bits per channel (float)")
        .value("NONE", WorldType::NONE, "Invalid/no world")
        .value("BIT8", WorldType::BIT8, "8 bits per channel")
        .value("BIT16", WorldType::BIT16, "16 bits per channel")
        .value("BIT32", WorldType::BIT32, "32 bits per channel (float)")
        .export_values();

    // World class
    py::class_<PyWorld, std::shared_ptr<PyWorld>>(m, "World",
        "Frame buffer for image data.\n\n"
        "World represents a pixel buffer that can be used for rendering\n"
        "and image processing operations.\n\n"
        "Example::\n\n"
        "    # Create a new 8-bit world\n"
        "    world = ae.World.create(ae.WorldType.BIT8, 1920, 1080)\n"
        "    \n"
        "    # Get pixel data\n"
        "    pixels = world.get_pixels()\n"
        "    \n"
        "    # Get individual pixel\n"
        "    r, g, b, a = world.get_pixel(100, 100)\n"
        "    \n"
        "    # Set pixel\n"
        "    world.set_pixel(100, 100, 1.0, 0.0, 0.0, 1.0)  # Red\n"
        "    \n"
        "    # Apply blur\n"
        "    world.fast_blur(5.0)")

        .def_property_readonly("valid", &PyWorld::IsValid,
            "Check if world is valid")

        .def_property_readonly("type", &PyWorld::GetType,
            "Get world type (bit depth)")

        .def_property_readonly("width", &PyWorld::GetWidth,
            "Get width in pixels")

        .def_property_readonly("height", &PyWorld::GetHeight,
            "Get height in pixels")

        .def_property_readonly("size", &PyWorld::GetSize,
            "Get size as (width, height) tuple")

        .def_property_readonly("row_bytes", &PyWorld::GetRowBytes,
            "Get bytes per row (row stride)")

        .def("get_pixels", &PyWorld::GetPixels,
            "Get all pixel data as bytes.\n\n"
            "Returns raw pixel data in ARGB format.\n"
            "The size depends on the world type:\n"
            "- BIT8: 4 bytes per pixel\n"
            "- BIT16: 8 bytes per pixel\n"
            "- BIT32: 16 bytes per pixel (float)")

        .def("set_pixels", &PyWorld::SetPixels,
            "Set all pixel data from bytes.\n\n"
            "Data size must match row_bytes * height.",
            py::arg("data"))

        .def("get_pixel", &PyWorld::GetPixel,
            "Get pixel value at (x, y).\n\n"
            "Returns (R, G, B, A) tuple normalized to 0.0-1.0.",
            py::arg("x"), py::arg("y"))

        .def("set_pixel", &PyWorld::SetPixel,
            "Set pixel value at (x, y).\n\n"
            "Values should be normalized 0.0-1.0.",
            py::arg("x"), py::arg("y"),
            py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a"))

        .def("fast_blur", &PyWorld::FastBlur,
            "Apply fast blur effect.\n\n"
            "Args:\n"
            "    radius: Blur radius in pixels\n"
            "    flags: Quality flags (0 = default)\n"
            "    quality: Quality level 0-5 (default 1)",
            py::arg("radius"),
            py::arg("flags") = 0,
            py::arg("quality") = 1)

        .def_property_readonly("_handle", [](const PyWorld& self) {
            return reinterpret_cast<uintptr_t>(self.GetHandle());
        }, "Internal: Get raw handle as integer")

        .def_static("create", &PyWorld::Create,
            "Create a new world with specified dimensions.\n\n"
            "Args:\n"
            "    type: WorldType (BIT8, BIT16, or BIT32)\n"
            "    width: Width in pixels (1-30000)\n"
            "    height: Height in pixels (1-30000)\n\n"
            "Returns:\n"
            "    New World instance",
            py::arg("type"), py::arg("width"), py::arg("height"))

        .def("__repr__", [](const PyWorld& self) {
            if (!self.IsValid()) {
                return std::string("<World: invalid>");
            }
            try {
                int w = self.GetWidth();
                int h = self.GetHeight();
                WorldType type = self.GetType();
                const char* typeStr = "unknown";
                switch (type) {
                    case WorldType::BIT8: typeStr = "8bpc"; break;
                    case WorldType::BIT16: typeStr = "16bpc"; break;
                    case WorldType::BIT32: typeStr = "32bpc"; break;
                    default: break;
                }
                return std::string("<World: ") +
                    std::to_string(w) + "x" + std::to_string(h) +
                    " " + typeStr + ">";
            } catch (...) {
                return std::string("<World: valid>");
            }
        });
}

} // namespace PyAE
