/*******************************************************************/
/*                                                                 */
/*                      TestEffect.cpp                             */
/*                                                                 */
/*  Test effect plugin for PyAE custom property validation         */
/*  This effect does nothing to the image (pass-through) but       */
/*  provides various parameter types for testing PyAE's ability    */
/*  to read and write effect parameters.                           */
/*                                                                 */
/*******************************************************************/

#include "TestEffect.h"

static PF_Err
About(
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ParamDef     *params[],
    PF_LayerDef     *output)
{
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    suites.ANSICallbacksSuite1()->sprintf(
        out_data->return_msg,
        "%s v%d.%d\r%s",
        STR(StrID_Name),
        MAJOR_VERSION,
        MINOR_VERSION,
        STR(StrID_Description));

    return PF_Err_NONE;
}

static PF_Err
GlobalSetup(
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ParamDef     *params[],
    PF_LayerDef     *output)
{
    out_data->my_version = PF_VERSION(
        MAJOR_VERSION,
        MINOR_VERSION,
        BUG_VERSION,
        STAGE_VERSION,
        BUILD_VERSION);

    out_data->out_flags =
        PF_OutFlag_DEEP_COLOR_AWARE;

    out_data->out_flags2 = PF_OutFlag2_NONE;

    return PF_Err_NONE;
}

static PF_Err
ParamsSetup(
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ParamDef     *params[],
    PF_LayerDef     *output)
{
    PF_Err      err = PF_Err_NONE;
    PF_ParamDef def;

    // --- Basic Types ---

    // Integer Slider
    AEFX_CLR_STRUCT(def);
    PF_ADD_SLIDER(
        STR(StrID_Slider_Int),
        TESTFX_INT_MIN,
        TESTFX_INT_MAX,
        TESTFX_INT_MIN,
        TESTFX_INT_MAX,
        TESTFX_INT_DFLT,
        TESTFX_SLIDER_INT);

    // Float Slider
    AEFX_CLR_STRUCT(def);
    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_Slider_Float),
        TESTFX_FLOAT_MIN,
        TESTFX_FLOAT_MAX,
        TESTFX_FLOAT_MIN,
        TESTFX_FLOAT_MAX,
        TESTFX_FLOAT_DFLT,
        PF_Precision_HUNDREDTHS,
        0,
        0,
        TESTFX_SLIDER_FLOAT);

    // Percent Slider
    AEFX_CLR_STRUCT(def);
    def.param_type = PF_Param_FIX_SLIDER;
    PF_STRCPY(def.PF_DEF_NAME, STR(StrID_Slider_Percent));
    def.u.fd.value_str[0] = '\0';
    def.u.fd.value_desc[0] = '\0';
    def.u.fd.valid_min = 0;
    def.u.fd.valid_max = static_cast<PF_Fixed>(100 << 16);
    def.u.fd.slider_min = 0;
    def.u.fd.slider_max = static_cast<PF_Fixed>(100 << 16);
    def.u.fd.value = static_cast<PF_Fixed>(static_cast<A_long>(TESTFX_PERCENT_DFLT * 65536.0));
    def.u.fd.precision = 1;
    def.flags = 0;
    def.ui_flags = 0;
    if (err = PF_ADD_PARAM(in_data, TESTFX_SLIDER_PERCENT, &def)) return err;

    // Checkbox
    AEFX_CLR_STRUCT(def);
    PF_ADD_CHECKBOXX(
        STR(StrID_Checkbox),
        TRUE,
        0,
        TESTFX_CHECKBOX);

    // --- Spatial Types ---

    // Angle
    AEFX_CLR_STRUCT(def);
    PF_ADD_ANGLE(
        STR(StrID_Angle),
        TESTFX_ANGLE_DFLT,
        TESTFX_ANGLE);

    // Color
    AEFX_CLR_STRUCT(def);
    PF_ADD_COLOR(
        STR(StrID_Color),
        TESTFX_COLOR_R,
        TESTFX_COLOR_G,
        TESTFX_COLOR_B,
        TESTFX_COLOR);

    // Point 2D
    AEFX_CLR_STRUCT(def);
    PF_ADD_POINT(
        STR(StrID_Point_2D),
        static_cast<A_long>(TESTFX_POINT2D_X),
        static_cast<A_long>(TESTFX_POINT2D_Y),
        0,
        TESTFX_POINT_2D);

    // --- Reference Types ---

    // Popup Menu
    AEFX_CLR_STRUCT(def);
    PF_ADD_POPUP(
        STR(StrID_Popup),
        TESTFX_POPUP_CHOICES,
        TESTFX_POPUP_DFLT,
        STR(StrID_Popup_Choices),
        TESTFX_POPUP);

    // Point 3D
    AEFX_CLR_STRUCT(def);
    PF_ADD_POINT_3D(
        STR(StrID_Point_3D),
        TESTFX_POINT3D_X,
        TESTFX_POINT3D_Y,
        TESTFX_POINT3D_Z,
        TESTFX_POINT_3D);

    // Button
    AEFX_CLR_STRUCT(def);
    PF_ADD_BUTTON(
        STR(StrID_Button),
        STR(StrID_Button_Label),
        0,
        PF_ParamFlag_SUPERVISE,
        TESTFX_BUTTON);

    // Arbitrary Data (Custom Data)
    // Uses PF_PUI_NO_ECW_UI to hide from Effect Controls Window (matching Paramarama SDK pattern)
    {
        PF_ArbitraryH dephault = NULL;
        err = TestFX_CreateDefaultArb(in_data, out_data, &dephault);
        if (err) return err;

        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_ARBITRARY_DATA;
        def.flags = 0;
        PF_STRCPY(def.PF_DEF_NAME, STR(StrID_Arbitrary));
        def.ui_width = 0;
        def.ui_height = 0;
        def.ui_flags = PF_PUI_NO_ECW_UI;
        def.u.arb_d.value = NULL;
        def.u.arb_d.pad = 0;
        def.u.arb_d.dephault = dephault;
        def.uu.id = def.u.arb_d.id = TESTFX_ARBITRARY;
        def.u.arb_d.refconPV = TESTFX_ARB_REFCON;

        PF_Err priv_err = PF_ADD_PARAM(in_data, -1, &def);
        if (priv_err != PF_Err_NONE) {
            return priv_err;
        }
    }

    out_data->num_params = TESTFX_NUM_PARAMS;

    return err;
}

static PF_Err
Render(
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ParamDef     *params[],
    PF_LayerDef     *output)
{
    PF_Err err = PF_Err_NONE;

    // Pass-through: simply copy input to output
    // This effect doesn't modify the image at all
    if (in_data->appl_id != 'PrMr') {  // Not Premiere
        AEGP_SuiteHandler suites(in_data->pica_basicP);

        if (PF_Quality_HI == in_data->quality) {
            err = suites.WorldTransformSuite1()->copy_hq(
                in_data->effect_ref,
                &params[TESTFX_INPUT]->u.ld,
                output,
                NULL,
                NULL);
        } else {
            err = suites.WorldTransformSuite1()->copy(
                in_data->effect_ref,
                &params[TESTFX_INPUT]->u.ld,
                output,
                NULL,
                NULL);
        }
    } else {
        // Premiere path
        err = PF_COPY(&params[TESTFX_INPUT]->u.ld, output, NULL, NULL);
    }

    return err;
}

static PF_Err
UserChangedParam(
    PF_InData                       *in_data,
    PF_OutData                      *out_data,
    PF_ParamDef                     *params[],
    const PF_UserChangedParamExtra  *which_hitP)
{
    PF_Err err = PF_Err_NONE;

    if (which_hitP->param_index == TESTFX_BUTTON) {
        // Show a message when button is clicked
        AEGP_SuiteHandler suites(in_data->pica_basicP);
        suites.ANSICallbacksSuite1()->strcpy(
            out_data->return_msg,
            STR(StrID_Button_Message));

        if (in_data->appl_id != 'PrMr') {
            out_data->out_flags |= PF_OutFlag_DISPLAY_ERROR_MESSAGE;
        }
    }

    return err;
}

// SDK version compatibility: Use V2 entry point if available (SDK 25.2+)
#ifdef PF_REGISTER_EFFECT_EXT2

extern "C" DllExport
PF_Err PluginDataEntryFunction2(
    PF_PluginDataPtr    inPtr,
    PF_PluginDataCB2    inPluginDataCallBackPtr,
    SPBasicSuite*       inSPBasicSuitePtr,
    const char*         inHostName,
    const char*         inHostVersion)
{
    PF_Err result = PF_Err_INVALID_CALLBACK;

    result = PF_REGISTER_EFFECT_EXT2(
        inPtr,
        inPluginDataCallBackPtr,
        "PyAE Test Effect",      // Name
        "PYAE TestEffect",       // Match Name
        "PyAE Test",             // Category
        AE_RESERVED_INFO,        // Reserved Info
        "EffectMain",            // Entry point
        "https://github.com/yusukei/PyAE");  // Support URL

    return result;
}

#else

// Fallback for older SDKs (2022, 2023) that don't have V2 entry point
extern "C" DllExport
PF_Err PluginDataEntryFunction(
    PF_PluginDataPtr    inPtr,
    PF_PluginDataCB     inPluginDataCallBackPtr,
    SPBasicSuite*       inSPBasicSuitePtr,
    const char*         inHostName,
    const char*         inHostVersion)
{
    PF_Err result = PF_Err_INVALID_CALLBACK;

    result = PF_REGISTER_EFFECT(
        inPtr,
        inPluginDataCallBackPtr,
        "PyAE Test Effect",      // Name
        "PYAE TestEffect",       // Match Name
        "PyAE Test",             // Category
        AE_RESERVED_INFO);       // Reserved Info

    return result;
}

#endif

PF_Err
EffectMain(
    PF_Cmd          cmd,
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ParamDef     *params[],
    PF_LayerDef     *output,
    void            *extra)
{
    PF_Err err = PF_Err_NONE;

    try {
        switch (cmd) {
            case PF_Cmd_ABOUT:
                err = About(in_data, out_data, params, output);
                break;

            case PF_Cmd_GLOBAL_SETUP:
                err = GlobalSetup(in_data, out_data, params, output);
                break;

            case PF_Cmd_PARAMS_SETUP:
                err = ParamsSetup(in_data, out_data, params, output);
                break;

            case PF_Cmd_RENDER:
                err = Render(in_data, out_data, params, output);
                break;

            case PF_Cmd_USER_CHANGED_PARAM:
                err = UserChangedParam(
                    in_data,
                    out_data,
                    params,
                    reinterpret_cast<const PF_UserChangedParamExtra*>(extra));
                break;

            case PF_Cmd_EVENT:
                // Handle draw events to prevent AE from retrying
                if (extra) {
                    PF_EventExtra* event_extra = reinterpret_cast<PF_EventExtra*>(extra);
                    if (event_extra->e_type == PF_Event_DRAW) {
                        event_extra->evt_out_flags = PF_EO_HANDLED_EVENT;
                    }
                }
                break;

            case PF_Cmd_ARBITRARY_CALLBACK:
                err = TestFX_HandleArbitrary(
                    in_data,
                    out_data,
                    params,
                    output,
                    reinterpret_cast<PF_ArbParamsExtra*>(extra));
                break;

            default:
                break;
        }
    } catch (PF_Err& thrown_err) {
        err = thrown_err;
    }

    return err;
}
