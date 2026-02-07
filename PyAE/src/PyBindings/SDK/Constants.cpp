#include <pybind11/pybind11.h>

// AE SDK Headers
#include "AE_GeneralPlug.h"
#include "AE_EffectCB.h"

// SDK Version Compatibility
#include "SDKVersionCompat.h"

namespace py = pybind11;

namespace PyAE {
namespace SDK {

void init_Constants(py::module_& sdk) {
    // -----------------------------------------------------------------------
    // AEGP_LayerStream Constants (62 constants for complete Export/Import)
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_LayerStream_NONE") = (int)AEGP_LayerStream_NONE;
    sdk.attr("AEGP_LayerStream_ANCHORPOINT") = (int)AEGP_LayerStream_ANCHORPOINT;
    sdk.attr("AEGP_LayerStream_POSITION") = (int)AEGP_LayerStream_POSITION;
    sdk.attr("AEGP_LayerStream_SCALE") = (int)AEGP_LayerStream_SCALE;
    sdk.attr("AEGP_LayerStream_ROTATION") = (int)AEGP_LayerStream_ROTATION;
    sdk.attr("AEGP_LayerStream_ROTATE_Z") = (int)AEGP_LayerStream_ROTATE_Z;
    sdk.attr("AEGP_LayerStream_OPACITY") = (int)AEGP_LayerStream_OPACITY;
    sdk.attr("AEGP_LayerStream_AUDIO") = (int)AEGP_LayerStream_AUDIO;
    sdk.attr("AEGP_LayerStream_MARKER") = (int)AEGP_LayerStream_MARKER;
    sdk.attr("AEGP_LayerStream_TIME_REMAP") = (int)AEGP_LayerStream_TIME_REMAP;
    sdk.attr("AEGP_LayerStream_ROTATE_X") = (int)AEGP_LayerStream_ROTATE_X;
    sdk.attr("AEGP_LayerStream_ROTATE_Y") = (int)AEGP_LayerStream_ROTATE_Y;
    sdk.attr("AEGP_LayerStream_ORIENTATION") = (int)AEGP_LayerStream_ORIENTATION;
    sdk.attr("AEGP_LayerStream_ZOOM") = (int)AEGP_LayerStream_ZOOM;
    sdk.attr("AEGP_LayerStream_DEPTH_OF_FIELD") = (int)AEGP_LayerStream_DEPTH_OF_FIELD;
    sdk.attr("AEGP_LayerStream_FOCUS_DISTANCE") = (int)AEGP_LayerStream_FOCUS_DISTANCE;
    sdk.attr("AEGP_LayerStream_APERTURE") = (int)AEGP_LayerStream_APERTURE;
    sdk.attr("AEGP_LayerStream_BLUR_LEVEL") = (int)AEGP_LayerStream_BLUR_LEVEL;
    sdk.attr("AEGP_LayerStream_INTENSITY") = (int)AEGP_LayerStream_INTENSITY;
    sdk.attr("AEGP_LayerStream_COLOR") = (int)AEGP_LayerStream_COLOR;
    sdk.attr("AEGP_LayerStream_CONE_ANGLE") = (int)AEGP_LayerStream_CONE_ANGLE;
    sdk.attr("AEGP_LayerStream_CONE_FEATHER") = (int)AEGP_LayerStream_CONE_FEATHER;
    sdk.attr("AEGP_LayerStream_SHADOW_DARKNESS") = (int)AEGP_LayerStream_SHADOW_DARKNESS;
    sdk.attr("AEGP_LayerStream_SHADOW_DIFFUSION") = (int)AEGP_LayerStream_SHADOW_DIFFUSION;
    sdk.attr("AEGP_LayerStream_ACCEPTS_SHADOWS") = (int)AEGP_LayerStream_ACCEPTS_SHADOWS;
    sdk.attr("AEGP_LayerStream_ACCEPTS_LIGHTS") = (int)AEGP_LayerStream_ACCEPTS_LIGHTS;
    sdk.attr("AEGP_LayerStream_AMBIENT_COEFF") = (int)AEGP_LayerStream_AMBIENT_COEFF;
    sdk.attr("AEGP_LayerStream_DIFFUSE_COEFF") = (int)AEGP_LayerStream_DIFFUSE_COEFF;
    sdk.attr("AEGP_LayerStream_SPECULAR_INTENSITY") = (int)AEGP_LayerStream_SPECULAR_INTENSITY;
    sdk.attr("AEGP_LayerStream_SPECULAR_SHININESS") = (int)AEGP_LayerStream_SPECULAR_SHININESS;
    sdk.attr("AEGP_LayerStream_CASTS_SHADOWS") = (int)AEGP_LayerStream_CASTS_SHADOWS;
    sdk.attr("AEGP_LayerStream_LIGHT_TRANSMISSION") = (int)AEGP_LayerStream_LIGHT_TRANSMISSION;
    sdk.attr("AEGP_LayerStream_METAL") = (int)AEGP_LayerStream_METAL;
    sdk.attr("AEGP_LayerStream_SOURCE_TEXT") = (int)AEGP_LayerStream_SOURCE_TEXT;
    sdk.attr("AEGP_LayerStream_IRIS_SHAPE") = (int)AEGP_LayerStream_IRIS_SHAPE;
    sdk.attr("AEGP_LayerStream_IRIS_ROTATION") = (int)AEGP_LayerStream_IRIS_ROTATION;
    sdk.attr("AEGP_LayerStream_IRIS_ROUNDNESS") = (int)AEGP_LayerStream_IRIS_ROUNDNESS;
    sdk.attr("AEGP_LayerStream_IRIS_ASPECT_RATIO") = (int)AEGP_LayerStream_IRIS_ASPECT_RATIO;
    sdk.attr("AEGP_LayerStream_IRIS_DIFFRACTION_FRINGE") = (int)AEGP_LayerStream_IRIS_DIFFRACTION_FRINGE;
    sdk.attr("AEGP_LayerStream_IRIS_HIGHLIGHT_GAIN") = (int)AEGP_LayerStream_IRIS_HIGHLIGHT_GAIN;
    sdk.attr("AEGP_LayerStream_IRIS_HIGHLIGHT_THRESHOLD") = (int)AEGP_LayerStream_IRIS_HIGHLIGHT_THRESHOLD;
    sdk.attr("AEGP_LayerStream_IRIS_HIGHLIGHT_SATURATION") = (int)AEGP_LayerStream_IRIS_HIGHLIGHT_SATURATION;
    sdk.attr("AEGP_LayerStream_LIGHT_FALLOFF_TYPE") = (int)AEGP_LayerStream_LIGHT_FALLOFF_TYPE;
    sdk.attr("AEGP_LayerStream_LIGHT_FALLOFF_START") = (int)AEGP_LayerStream_LIGHT_FALLOFF_START;
    sdk.attr("AEGP_LayerStream_LIGHT_FALLOFF_DISTANCE") = (int)AEGP_LayerStream_LIGHT_FALLOFF_DISTANCE;
    sdk.attr("AEGP_LayerStream_REFLECTION_INTENSITY") = (int)AEGP_LayerStream_REFLECTION_INTENSITY;
    sdk.attr("AEGP_LayerStream_REFLECTION_SHARPNESS") = (int)AEGP_LayerStream_REFLECTION_SHARPNESS;
    sdk.attr("AEGP_LayerStream_REFLECTION_ROLLOFF") = (int)AEGP_LayerStream_REFLECTION_ROLLOFF;
    sdk.attr("AEGP_LayerStream_TRANSPARENCY_COEFF") = (int)AEGP_LayerStream_TRANSPARENCY_COEFF;
    sdk.attr("AEGP_LayerStream_TRANSPARENCY_ROLLOFF") = (int)AEGP_LayerStream_TRANSPARENCY_ROLLOFF;
    sdk.attr("AEGP_LayerStream_INDEX_OF_REFRACTION") = (int)AEGP_LayerStream_INDEX_OF_REFRACTION;
    sdk.attr("AEGP_LayerStream_EXTRUSION_BEVEL_STYLE") = (int)AEGP_LayerStream_EXTRUSION_BEVEL_STYLE;
    sdk.attr("AEGP_LayerStream_EXTRUSION_BEVEL_DIRECTION") = (int)AEGP_LayerStream_EXTRUSION_BEVEL_DIRECTION;
    sdk.attr("AEGP_LayerStream_EXTRUSION_BEVEL_DEPTH") = (int)AEGP_LayerStream_EXTRUSION_BEVEL_DEPTH;
    sdk.attr("AEGP_LayerStream_EXTRUSION_HOLE_BEVEL_DEPTH") = (int)AEGP_LayerStream_EXTRUSION_HOLE_BEVEL_DEPTH;
    sdk.attr("AEGP_LayerStream_EXTRUSION_DEPTH") = (int)AEGP_LayerStream_EXTRUSION_DEPTH;
    sdk.attr("AEGP_LayerStream_PLANE_CURVATURE") = (int)AEGP_LayerStream_PLANE_CURVATURE;
    sdk.attr("AEGP_LayerStream_PLANE_SUBDIVISION") = (int)AEGP_LayerStream_PLANE_SUBDIVISION;
#if defined(kAEGPLightSuiteVersion3)
    // AE 24.4+ only: Environment light background properties
    sdk.attr("AEGP_LayerStream_LIGHT_BACKGROUND_VISIBLE") = (int)AEGP_LayerStream_LIGHT_BACKGROUND_VISIBLE;
    sdk.attr("AEGP_LayerStream_LIGHT_BACKGROUND_OPACITY") = (int)AEGP_LayerStream_LIGHT_BACKGROUND_OPACITY;
    sdk.attr("AEGP_LayerStream_LIGHT_BACKGROUND_BLUR") = (int)AEGP_LayerStream_LIGHT_BACKGROUND_BLUR;
    sdk.attr("AEGP_LayerStream_SHADOW_COLOR") = (int)AEGP_LayerStream_SHADOW_COLOR;
#endif
    sdk.attr("AEGP_LayerStream_NUMTYPES") = (int)AEGP_LayerStream_NUMTYPES;

    // -----------------------------------------------------------------------
    // AEGP_MarkerString Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_MarkerString_NONE") = (int)AEGP_MarkerString_NONE;
    sdk.attr("AEGP_MarkerString_COMMENT") = (int)AEGP_MarkerString_COMMENT;
    sdk.attr("AEGP_MarkerString_CHAPTER") = (int)AEGP_MarkerString_CHAPTER;
    sdk.attr("AEGP_MarkerString_URL") = (int)AEGP_MarkerString_URL;
    sdk.attr("AEGP_MarkerString_FRAME_TARGET") = (int)AEGP_MarkerString_FRAME_TARGET;
    sdk.attr("AEGP_MarkerString_CUE_POINT_NAME") = (int)AEGP_MarkerString_CUE_POINT_NAME;

    // -----------------------------------------------------------------------
    // AEGP_LTimeMode Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_LTimeMode_LayerTime") = (int)AEGP_LTimeMode_LayerTime;
    sdk.attr("AEGP_LTimeMode_CompTime") = (int)AEGP_LTimeMode_CompTime;

    // -----------------------------------------------------------------------
    // AEGP_LayerQuality Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_LayerQual_NONE") = (int)AEGP_LayerQual_NONE;
    sdk.attr("AEGP_LayerQual_WIREFRAME") = (int)AEGP_LayerQual_WIREFRAME;
    sdk.attr("AEGP_LayerQual_DRAFT") = (int)AEGP_LayerQual_DRAFT;
    sdk.attr("AEGP_LayerQual_BEST") = (int)AEGP_LayerQual_BEST;

    // -----------------------------------------------------------------------
    // AEGP_LayerSamplingQuality Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_LayerSamplingQual_BILINEAR") = (int)AEGP_LayerSamplingQual_BILINEAR;
    sdk.attr("AEGP_LayerSamplingQual_BICUBIC") = (int)AEGP_LayerSamplingQual_BICUBIC;

    // -----------------------------------------------------------------------
    // AEGP_LabelID Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_Label_NONE") = (int)AEGP_Label_NONE;
    sdk.attr("AEGP_Label_NO_LABEL") = (int)AEGP_Label_NO_LABEL;
    sdk.attr("AEGP_Label_1") = (int)AEGP_Label_1;
    sdk.attr("AEGP_Label_2") = (int)AEGP_Label_2;
    sdk.attr("AEGP_Label_3") = (int)AEGP_Label_3;
    sdk.attr("AEGP_Label_4") = (int)AEGP_Label_4;
    sdk.attr("AEGP_Label_5") = (int)AEGP_Label_5;
    sdk.attr("AEGP_Label_6") = (int)AEGP_Label_6;
    sdk.attr("AEGP_Label_7") = (int)AEGP_Label_7;
    sdk.attr("AEGP_Label_8") = (int)AEGP_Label_8;
    sdk.attr("AEGP_Label_9") = (int)AEGP_Label_9;
    sdk.attr("AEGP_Label_10") = (int)AEGP_Label_10;
    sdk.attr("AEGP_Label_11") = (int)AEGP_Label_11;
    sdk.attr("AEGP_Label_12") = (int)AEGP_Label_12;
    sdk.attr("AEGP_Label_13") = (int)AEGP_Label_13;
    sdk.attr("AEGP_Label_14") = (int)AEGP_Label_14;
    sdk.attr("AEGP_Label_15") = (int)AEGP_Label_15;
    sdk.attr("AEGP_Label_16") = (int)AEGP_Label_16;

    // -----------------------------------------------------------------------
    // PF_TransferMode (Blend Mode) Constants
    // -----------------------------------------------------------------------
    sdk.attr("PF_Xfer_NONE") = (int)PF_Xfer_NONE;
    sdk.attr("PF_Xfer_COPY") = (int)PF_Xfer_COPY;
    sdk.attr("PF_Xfer_BEHIND") = (int)PF_Xfer_BEHIND;
    sdk.attr("PF_Xfer_IN_FRONT") = (int)PF_Xfer_IN_FRONT;
    sdk.attr("PF_Xfer_DISSOLVE") = (int)PF_Xfer_DISSOLVE;
    sdk.attr("PF_Xfer_ADD") = (int)PF_Xfer_ADD;
    sdk.attr("PF_Xfer_MULTIPLY") = (int)PF_Xfer_MULTIPLY;
    sdk.attr("PF_Xfer_SCREEN") = (int)PF_Xfer_SCREEN;
    sdk.attr("PF_Xfer_OVERLAY") = (int)PF_Xfer_OVERLAY;
    sdk.attr("PF_Xfer_SOFT_LIGHT") = (int)PF_Xfer_SOFT_LIGHT;
    sdk.attr("PF_Xfer_HARD_LIGHT") = (int)PF_Xfer_HARD_LIGHT;
    sdk.attr("PF_Xfer_DARKEN") = (int)PF_Xfer_DARKEN;
    sdk.attr("PF_Xfer_LIGHTEN") = (int)PF_Xfer_LIGHTEN;
    sdk.attr("PF_Xfer_DIFFERENCE") = (int)PF_Xfer_DIFFERENCE;
    sdk.attr("PF_Xfer_HUE") = (int)PF_Xfer_HUE;
    sdk.attr("PF_Xfer_SATURATION") = (int)PF_Xfer_SATURATION;
    sdk.attr("PF_Xfer_COLOR") = (int)PF_Xfer_COLOR;
    sdk.attr("PF_Xfer_LUMINOSITY") = (int)PF_Xfer_LUMINOSITY;
    sdk.attr("PF_Xfer_MULTIPLY_ALPHA") = (int)PF_Xfer_MULTIPLY_ALPHA;
    sdk.attr("PF_Xfer_MULTIPLY_ALPHA_LUMA") = (int)PF_Xfer_MULTIPLY_ALPHA_LUMA;
    sdk.attr("PF_Xfer_MULTIPLY_NOT_ALPHA") = (int)PF_Xfer_MULTIPLY_NOT_ALPHA;
    sdk.attr("PF_Xfer_MULTIPLY_NOT_ALPHA_LUMA") = (int)PF_Xfer_MULTIPLY_NOT_ALPHA_LUMA;
    sdk.attr("PF_Xfer_ADDITIVE_PREMUL") = (int)PF_Xfer_ADDITIVE_PREMUL;
    sdk.attr("PF_Xfer_ALPHA_ADD") = (int)PF_Xfer_ALPHA_ADD;
    sdk.attr("PF_Xfer_COLOR_DODGE") = (int)PF_Xfer_COLOR_DODGE;
    sdk.attr("PF_Xfer_COLOR_BURN") = (int)PF_Xfer_COLOR_BURN;
    sdk.attr("PF_Xfer_EXCLUSION") = (int)PF_Xfer_EXCLUSION;
    sdk.attr("PF_Xfer_DIFFERENCE2") = (int)PF_Xfer_DIFFERENCE2;
    sdk.attr("PF_Xfer_COLOR_DODGE2") = (int)PF_Xfer_COLOR_DODGE2;
    sdk.attr("PF_Xfer_COLOR_BURN2") = (int)PF_Xfer_COLOR_BURN2;
    sdk.attr("PF_Xfer_LINEAR_DODGE") = (int)PF_Xfer_LINEAR_DODGE;
    sdk.attr("PF_Xfer_LINEAR_BURN") = (int)PF_Xfer_LINEAR_BURN;
    sdk.attr("PF_Xfer_LINEAR_LIGHT") = (int)PF_Xfer_LINEAR_LIGHT;
    sdk.attr("PF_Xfer_VIVID_LIGHT") = (int)PF_Xfer_VIVID_LIGHT;
    sdk.attr("PF_Xfer_PIN_LIGHT") = (int)PF_Xfer_PIN_LIGHT;
    sdk.attr("PF_Xfer_HARD_MIX") = (int)PF_Xfer_HARD_MIX;
    sdk.attr("PF_Xfer_LIGHTER_COLOR") = (int)PF_Xfer_LIGHTER_COLOR;
    sdk.attr("PF_Xfer_DARKER_COLOR") = (int)PF_Xfer_DARKER_COLOR;
    sdk.attr("PF_Xfer_SUBTRACT") = (int)PF_Xfer_SUBTRACT;
    sdk.attr("PF_Xfer_DIVIDE") = (int)PF_Xfer_DIVIDE;

    // トラックマット定数
    sdk.attr("TRACK_MATTE_NO_TRACK_MATTE") = (int)AEGP_TrackMatte_NO_TRACK_MATTE;
    sdk.attr("TRACK_MATTE_ALPHA") = (int)AEGP_TrackMatte_ALPHA;
    sdk.attr("TRACK_MATTE_NOT_ALPHA") = (int)AEGP_TrackMatte_NOT_ALPHA;
    sdk.attr("TRACK_MATTE_LUMA") = (int)AEGP_TrackMatte_LUMA;
    sdk.attr("TRACK_MATTE_NOT_LUMA") = (int)AEGP_TrackMatte_NOT_LUMA;

    // -----------------------------------------------------------------------
    // AEGP_CameraType Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_CameraType_NONE") = (int)AEGP_CameraType_NONE;
    sdk.attr("AEGP_CameraType_PERSPECTIVE") = (int)AEGP_CameraType_PERSPECTIVE;
    sdk.attr("AEGP_CameraType_ORTHOGRAPHIC") = (int)AEGP_CameraType_ORTHOGRAPHIC;

    // -----------------------------------------------------------------------
    // AEGP_FilmSizeUnits Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_FilmSizeUnits_NONE") = (int)AEGP_FilmSizeUnits_NONE;
    sdk.attr("AEGP_FilmSizeUnits_HORIZONTAL") = (int)AEGP_FilmSizeUnits_HORIZONTAL;
    sdk.attr("AEGP_FilmSizeUnits_VERTICAL") = (int)AEGP_FilmSizeUnits_VERTICAL;
    sdk.attr("AEGP_FilmSizeUnits_DIAGONAL") = (int)AEGP_FilmSizeUnits_DIAGONAL;

    // -----------------------------------------------------------------------
    // AEGP_LightType Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_LightType_NONE") = (int)AEGP_LightType_NONE;
    sdk.attr("AEGP_LightType_PARALLEL") = (int)AEGP_LightType_PARALLEL;
    sdk.attr("AEGP_LightType_SPOT") = (int)AEGP_LightType_SPOT;
    sdk.attr("AEGP_LightType_POINT") = (int)AEGP_LightType_POINT;
    sdk.attr("AEGP_LightType_AMBIENT") = (int)AEGP_LightType_AMBIENT;
#if defined(kAEGPLightSuiteVersion3)
    // AE 24.4+ only: Environment light type
    sdk.attr("AEGP_LightType_ENVIRONMENT") = (int)AEGP_LightType_ENVIRONMENT;
#endif

    // -----------------------------------------------------------------------
    // AEGP_KeyframeInterpolationType Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_KeyInterp_NONE") = (int)AEGP_KeyInterp_NONE;
    sdk.attr("AEGP_KeyInterp_LINEAR") = (int)AEGP_KeyInterp_LINEAR;
    sdk.attr("AEGP_KeyInterp_BEZIER") = (int)AEGP_KeyInterp_BEZIER;
    sdk.attr("AEGP_KeyInterp_HOLD") = (int)AEGP_KeyInterp_HOLD;

    // -----------------------------------------------------------------------
    // AEGP_QueryXformType Constants
    // -----------------------------------------------------------------------
    sdk.attr("AEGP_Query_Xform_LAYER") = (int)AEGP_Query_Xform_LAYER;
    sdk.attr("AEGP_Query_Xform_WORLD") = (int)AEGP_Query_Xform_WORLD;
    sdk.attr("AEGP_Query_Xform_VIEW") = (int)AEGP_Query_Xform_VIEW;
    sdk.attr("AEGP_Query_Xform_SCREEN") = (int)AEGP_Query_Xform_SCREEN;

    // -----------------------------------------------------------------------
    // SDK Feature Flags - for runtime feature detection
    // -----------------------------------------------------------------------
    sdk.attr("HAS_LIGHT_SUITE_V3") = PyAE::SDKCompat::HasLightSuite3;
    sdk.attr("HAS_COMP_SUITE_V12") = PyAE::SDKCompat::HasCompSuite12;
    sdk.attr("SDK_DESCRIPTION") = PyAE::SDKCompat::SDKInfo::GetSDKDescription();
}

} // namespace SDK
} // namespace PyAE
