/*******************************************************************/
/*                                                                 */
/*                      TestEffect.h                               */
/*                                                                 */
/*  Test effect plugin for PyAE custom property validation         */
/*                                                                 */
/*******************************************************************/

#pragma once

#ifndef TESTEFFECT_H
#define TESTEFFECT_H

#include "AEConfig.h"
#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEGP_SuiteHandler.h"
#include "AE_EffectUI.h"

#include "TestEffect_Strings.h"

// Version
#define MAJOR_VERSION       1
#define MINOR_VERSION       0
#define BUG_VERSION         0
#define STAGE_VERSION       PF_Stage_DEVELOP
#define BUILD_VERSION       1

// Parameter IDs (Disk IDs for serialization)
// No TOPIC groups needed for this test plugin
enum {
    TESTFX_INPUT = 0,
    TESTFX_SLIDER_INT,
    TESTFX_SLIDER_FLOAT,
    TESTFX_SLIDER_PERCENT,
    TESTFX_CHECKBOX,
    TESTFX_ANGLE,
    TESTFX_COLOR,
    TESTFX_POINT_2D,
    TESTFX_POPUP,
    TESTFX_POINT_3D,
    TESTFX_BUTTON,
    TESTFX_ARBITRARY,
    TESTFX_NUM_PARAMS
};

// Layer Ref not yet added
#define TESTFX_LAYER_REF    100

// Parameter defaults
#define TESTFX_INT_MIN          0
#define TESTFX_INT_MAX          100
#define TESTFX_INT_DFLT         50

#define TESTFX_FLOAT_MIN        0.0
#define TESTFX_FLOAT_MAX        100.0
#define TESTFX_FLOAT_DFLT       3.14159

#define TESTFX_PERCENT_DFLT     75.0

#define TESTFX_ANGLE_DFLT       45.0

#define TESTFX_COLOR_R          255
#define TESTFX_COLOR_G          128
#define TESTFX_COLOR_B          64

#define TESTFX_POINT2D_X        50.0
#define TESTFX_POINT2D_Y        50.0

#define TESTFX_POINT3D_X        100.0
#define TESTFX_POINT3D_Y        100.0
#define TESTFX_POINT3D_Z        100.0

#define TESTFX_POPUP_CHOICES    5
#define TESTFX_POPUP_DFLT       2

// Arbitrary data definitions
#define TESTFX_ARB_DISK_ID      100
#define TESTFX_ARB_REFCON       nullptr
#define TESTFX_ARB_MAX_PRINT_SIZE   256
#define TESTFX_ARB_NUM_VALUES   4

// Custom arbitrary data structure for testing serialization
typedef struct {
    A_FpLong    values[TESTFX_ARB_NUM_VALUES];  // 4 floating-point values
    A_long      mode;                            // Integer mode selector
    A_Boolean   enabled;                         // Boolean flag
    A_char      pad[3];                          // Padding for alignment
} TestFX_ArbData;

// Arbitrary data handler functions
PF_Err TestFX_CreateDefaultArb(PF_InData* in_data, PF_OutData* out_data, PF_ArbitraryH* dephault);
PF_Err TestFX_HandleArbitrary(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], PF_LayerDef* output, PF_ArbParamsExtra* extra);

// Entry point
#ifdef __cplusplus
extern "C" {
#endif

DllExport PF_Err EffectMain(
    PF_Cmd          cmd,
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ParamDef     *params[],
    PF_LayerDef     *output,
    void            *extra);

#ifdef __cplusplus
}
#endif

#endif // TESTEFFECT_H
