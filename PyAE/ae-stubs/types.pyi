# ae.types - Type definitions
# PyAE - Python for After Effects

from enum import IntEnum

class LayerType(IntEnum):
    """レイヤータイプ"""

    None_ = 0  # Python予約語のため末尾に_
    AV = 1
    Light = 2
    Camera = 3
    Text = 4
    Adjustment = 5
    Null = 6
    Shape = 7
    Solid = 8

class ItemType(IntEnum):
    """アイテムタイプ"""

    None_ = 0
    Folder = 1
    Comp = 2
    Footage = 3
    Solid = 4

class FootageType(IntEnum):
    """フッテージタイプ"""

    None_ = 0
    Solid = 1
    Missing = 2
    Placeholder = 3

class MaskMode(IntEnum):
    """マスクモード"""

    None_ = 0
    Add = 1
    Subtract = 2
    Intersect = 3
    Lighten = 4
    Darken = 5
    Difference = 6

class RenderStatus(IntEnum):
    """レンダーステータス"""

    WillContinue = 0
    NeedsOutput = 1
    Queued = 2
    Rendering = 3
    UserStopped = 4
    ErrStopped = 5
    Done = 6
    Unqueued = 7

class PropertyType(IntEnum):
    """プロパティタイプ"""

    None_ = 0
    OneDim = 1
    TwoDim = 2
    ThreeDim = 3
    Color = 4
    Arbitrary = 5
    NoValue = 6

class StreamValueType(IntEnum):
    """ストリーム値タイプ"""

    NoData = 0
    ThreeD_Spatial = 1
    ThreeD = 2
    TwoD_Spatial = 3
    TwoD = 4
    OneD = 5
    Color = 6
    ARB = 7
    Marker = 8
    LayerID = 9
    MaskID = 10
    Mask = 11
    TextDocument = 12

class StreamGroupingType(IntEnum):
    """ストリームグルーピングタイプ"""

    Invalid = 0
    Leaf = 1
    NamedGroup = 2
    IndexedGroup = 3

class KeyInterpolation(IntEnum):
    """キーフレーム補間タイプ"""

    Linear = 0
    Bezier = 1
    Hold = 2

class EasingType(IntEnum):
    """イージングタイプ"""

    Linear = 0
    EaseIn = 1
    EaseOut = 2
    EaseInOut = 3
    Custom = 4

class LayerQuality(IntEnum):
    """レイヤー品質"""

    None_ = 0
    Wireframe = 1
    Draft = 2
    Best = 3

class SamplingQuality(IntEnum):
    """サンプリング品質"""

    Bilinear = 0
    Bicubic = 1

class BlendMode(IntEnum):
    """ブレンドモード"""

    None_ = 0
    Copy = 1
    Behind = 2
    InFront = 3
    Dissolve = 4
    Add = 5
    Multiply = 6
    Screen = 7
    Overlay = 8
    SoftLight = 9
    HardLight = 10
    Darken = 11
    Lighten = 12
    Difference = 13
    Hue = 14
    Saturation = 15
    Color = 16
    Luminosity = 17
    MultiplyAlpha = 18
    MultiplyAlphaLuma = 19
    MultiplyNotAlpha = 20
    MultiplyNotAlphaLuma = 21
    AdditivePremul = 22
    AlphaAdd = 23
    ColorDodge = 24
    ColorBurn = 25
    Exclusion = 26
    Difference2 = 27
    ColorDodge2 = 28
    ColorBurn2 = 29
    LinearDodge = 30
    LinearBurn = 31
    LinearLight = 32
    VividLight = 33
    PinLight = 34
    HardMix = 35
    LighterColor = 36
    DarkerColor = 37
    Subtract = 38
    Divide = 39

class TrackMatteMode(IntEnum):
    """トラックマットモード"""

    NoTrackMatte = 0
    Alpha = 1
    NotAlpha = 2
    Luma = 3
    NotLuma = 4

class EmbedOptions(IntEnum):
    """埋め込みオプション"""

    None_ = 0
    Link = 1
    All = 2

class LightType(IntEnum):
    """ライトタイプ"""

    Parallel = 0
    Spot = 1
    Point = 2
    Ambient = 3

__all__ = [
    "LayerType",
    "ItemType",
    "FootageType",
    "MaskMode",
    "RenderStatus",
    "PropertyType",
    "StreamValueType",
    "StreamGroupingType",
    "KeyInterpolation",
    "EasingType",
    "LayerQuality",
    "SamplingQuality",
    "BlendMode",
    "TrackMatteMode",
    "EmbedOptions",
    "LightType",
]
