/*******************************************************************/
/*                                                                 */
/*                  TestEffect_Arb_Handler.cpp                     */
/*                                                                 */
/*  Arbitrary data handler implementation for TestEffect           */
/*  Provides New/Dispose/Copy/Flatten/Unflatten/Interpolate/       */
/*  Compare/Print callbacks for PF_Param_ARBITRARY_DATA            */
/*                                                                 */
/*******************************************************************/

#include "TestEffect.h"
#include <cstdio>
#include <cstring>

// Default values for TestFX_ArbData
static const A_FpLong kDefaultValues[TESTFX_ARB_NUM_VALUES] = {
    1.0, 2.0, 3.0, 4.0
};
static const A_long    kDefaultMode    = 1;
static const A_Boolean kDefaultEnabled = TRUE;


PF_Err
TestFX_CreateDefaultArb(
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ArbitraryH   *dephault)
{
    PF_Err err = PF_Err_NONE;

    AEGP_SuiteHandler suites(in_data->pica_basicP);

    PF_Handle arbH = suites.HandleSuite1()->host_new_handle(sizeof(TestFX_ArbData));
    if (!arbH) {
        return PF_Err_OUT_OF_MEMORY;
    }

    TestFX_ArbData* arbP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(arbH));
    if (!arbP) {
        suites.HandleSuite1()->host_dispose_handle(arbH);
        return PF_Err_OUT_OF_MEMORY;
    }

    AEFX_CLR_STRUCT(*arbP);

    for (A_long i = 0; i < TESTFX_ARB_NUM_VALUES; ++i) {
        arbP->values[i] = kDefaultValues[i];
    }
    arbP->mode    = kDefaultMode;
    arbP->enabled = kDefaultEnabled;

    suites.HandleSuite1()->host_unlock_handle(arbH);
    *dephault = arbH;

    return err;
}


static PF_Err
Arb_Copy(
    PF_InData               *in_data,
    const PF_ArbitraryH     *src,
    PF_ArbitraryH           *dst)
{
    PF_Err err = PF_Err_NONE;

    PF_Handle srcH = *src;
    PF_Handle dstH = *dst;

    if (!srcH || !dstH) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    AEGP_SuiteHandler suites(in_data->pica_basicP);

    TestFX_ArbData* srcP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(srcH));
    TestFX_ArbData* dstP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(dstH));

    if (!srcP || !dstP) {
        if (srcP) suites.HandleSuite1()->host_unlock_handle(srcH);
        if (dstP) suites.HandleSuite1()->host_unlock_handle(dstH);
        return PF_Err_OUT_OF_MEMORY;
    }

    memcpy(dstP, srcP, sizeof(TestFX_ArbData));

    suites.HandleSuite1()->host_unlock_handle(srcH);
    suites.HandleSuite1()->host_unlock_handle(dstH);

    return err;
}


static PF_Err
Arb_Flatten(
    PF_InData               *in_data,
    const PF_ArbitraryH     *arbH,
    A_u_long                buf_size,
    void                    *flat_buf)
{
    PF_Err err = PF_Err_NONE;

    PF_Handle h = *arbH;
    if (!h) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    AEGP_SuiteHandler suites(in_data->pica_basicP);

    TestFX_ArbData* arbP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(h));
    if (!arbP) {
        return PF_Err_OUT_OF_MEMORY;
    }

    if (buf_size >= sizeof(TestFX_ArbData)) {
        memcpy(flat_buf, arbP, sizeof(TestFX_ArbData));
    } else {
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    suites.HandleSuite1()->host_unlock_handle(h);
    return err;
}


static PF_Err
Arb_Unflatten(
    PF_InData       *in_data,
    A_u_long        buf_size,
    const void      *flat_buf,
    PF_ArbitraryH   *arbPH)
{
    PF_Err err = PF_Err_NONE;

    if (buf_size < sizeof(TestFX_ArbData)) {
        return PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    AEGP_SuiteHandler suites(in_data->pica_basicP);

    PF_Handle arbH = suites.HandleSuite1()->host_new_handle(sizeof(TestFX_ArbData));
    if (!arbH) {
        return PF_Err_OUT_OF_MEMORY;
    }

    TestFX_ArbData* arbP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(arbH));
    if (!arbP) {
        suites.HandleSuite1()->host_dispose_handle(arbH);
        return PF_Err_OUT_OF_MEMORY;
    }

    memcpy(arbP, flat_buf, sizeof(TestFX_ArbData));

    suites.HandleSuite1()->host_unlock_handle(arbH);
    *arbPH = arbH;

    return err;
}


static PF_Err
Arb_Interpolate(
    PF_InData               *in_data,
    double                  tF,
    const PF_ArbitraryH     *l_arbH,
    const PF_ArbitraryH     *r_arbH,
    PF_ArbitraryH           *result_arbH)
{
    PF_Err err = PF_Err_NONE;

    PF_Handle lH = *l_arbH;
    PF_Handle rH = *r_arbH;
    PF_Handle resH = *result_arbH;

    if (!lH || !rH || !resH) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    AEGP_SuiteHandler suites(in_data->pica_basicP);

    TestFX_ArbData* lP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(lH));
    TestFX_ArbData* rP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(rH));
    TestFX_ArbData* resP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(resH));

    if (!lP || !rP || !resP) {
        if (lP)   suites.HandleSuite1()->host_unlock_handle(lH);
        if (rP)   suites.HandleSuite1()->host_unlock_handle(rH);
        if (resP) suites.HandleSuite1()->host_unlock_handle(resH);
        return PF_Err_OUT_OF_MEMORY;
    }

    // Interpolate floating-point values
    for (A_long i = 0; i < TESTFX_ARB_NUM_VALUES; ++i) {
        resP->values[i] = lP->values[i] + (rP->values[i] - lP->values[i]) * tF;
    }

    // Discrete values: use left value for t < 0.5, right for t >= 0.5
    if (tF < 0.5) {
        resP->mode    = lP->mode;
        resP->enabled = lP->enabled;
    } else {
        resP->mode    = rP->mode;
        resP->enabled = rP->enabled;
    }

    suites.HandleSuite1()->host_unlock_handle(lH);
    suites.HandleSuite1()->host_unlock_handle(rH);
    suites.HandleSuite1()->host_unlock_handle(resH);

    return err;
}


static PF_Err
Arb_Compare(
    PF_InData               *in_data,
    const PF_ArbitraryH     *a_arbH,
    const PF_ArbitraryH     *b_arbH,
    PF_ArbCompareResult     *resultP)
{
    PF_Err err = PF_Err_NONE;

    PF_Handle aH = *a_arbH;
    PF_Handle bH = *b_arbH;

    *resultP = PF_ArbCompare_EQUAL;

    if (!aH || !bH) {
        if (!aH && !bH) {
            *resultP = PF_ArbCompare_EQUAL;
        } else {
            *resultP = aH ? PF_ArbCompare_MORE : PF_ArbCompare_LESS;
        }
        return err;
    }

    AEGP_SuiteHandler suites(in_data->pica_basicP);

    TestFX_ArbData* aP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(aH));
    TestFX_ArbData* bP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(bH));

    if (!aP || !bP) {
        if (aP) suites.HandleSuite1()->host_unlock_handle(aH);
        if (bP) suites.HandleSuite1()->host_unlock_handle(bH);
        return PF_Err_OUT_OF_MEMORY;
    }

    // Compare by memcmp for exact equality, then by sum for ordering
    if (memcmp(aP, bP, sizeof(TestFX_ArbData)) == 0) {
        *resultP = PF_ArbCompare_EQUAL;
    } else {
        A_FpLong sumA = 0, sumB = 0;
        for (A_long i = 0; i < TESTFX_ARB_NUM_VALUES; ++i) {
            sumA += aP->values[i];
            sumB += bP->values[i];
        }
        *resultP = (sumA > sumB) ? PF_ArbCompare_MORE : PF_ArbCompare_LESS;
    }

    suites.HandleSuite1()->host_unlock_handle(aH);
    suites.HandleSuite1()->host_unlock_handle(bH);

    return err;
}


static PF_Err
Arb_Print(
    PF_InData       *in_data,
    PF_ArbitraryH   arbH,
    A_u_long        print_size,
    A_char          *print_buf)
{
    PF_Err err = PF_Err_NONE;

    if (!arbH || !print_buf || print_size == 0) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    AEGP_SuiteHandler suites(in_data->pica_basicP);

    TestFX_ArbData* arbP = reinterpret_cast<TestFX_ArbData*>(
        suites.HandleSuite1()->host_lock_handle(arbH));
    if (!arbP) {
        return PF_Err_OUT_OF_MEMORY;
    }

    // Format: "V0=1.0 V1=2.0 V2=3.0 V3=4.0 M=1 E=1"
    _snprintf_s(print_buf, print_size, _TRUNCATE,
        "V0=%.4f V1=%.4f V2=%.4f V3=%.4f M=%ld E=%d",
        arbP->values[0], arbP->values[1],
        arbP->values[2], arbP->values[3],
        arbP->mode, static_cast<int>(arbP->enabled));

    suites.HandleSuite1()->host_unlock_handle(arbH);
    return err;
}


PF_Err
TestFX_HandleArbitrary(
    PF_InData           *in_data,
    PF_OutData          *out_data,
    PF_ParamDef         *params[],
    PF_LayerDef         *output,
    PF_ArbParamsExtra   *extra)
{
    PF_Err err = PF_Err_NONE;

    if (!extra) {
        return PF_Err_BAD_CALLBACK_PARAM;
    }

    switch (extra->which_function) {

        case PF_Arbitrary_NEW_FUNC:
            err = TestFX_CreateDefaultArb(
                in_data, out_data,
                extra->u.new_func_params.arbPH);
            break;

        case PF_Arbitrary_DISPOSE_FUNC:
            if (extra->u.dispose_func_params.arbH) {
                PF_DISPOSE_HANDLE(extra->u.dispose_func_params.arbH);
            }
            break;

        case PF_Arbitrary_COPY_FUNC:
            // Must allocate destination handle first, then copy
            // (matches ColorGrid SDK pattern)
            err = TestFX_CreateDefaultArb(
                in_data, out_data,
                extra->u.copy_func_params.dst_arbPH);
            if (!err) {
                err = Arb_Copy(
                    in_data,
                    &extra->u.copy_func_params.src_arbH,
                    extra->u.copy_func_params.dst_arbPH);
            }
            break;

        case PF_Arbitrary_FLAT_SIZE_FUNC:
            *(extra->u.flat_size_func_params.flat_data_sizePLu) = sizeof(TestFX_ArbData);
            break;

        case PF_Arbitrary_FLATTEN_FUNC:
            err = Arb_Flatten(
                in_data,
                &extra->u.flatten_func_params.arbH,
                extra->u.flatten_func_params.buf_sizeLu,
                extra->u.flatten_func_params.flat_dataPV);
            break;

        case PF_Arbitrary_UNFLATTEN_FUNC:
            err = Arb_Unflatten(
                in_data,
                extra->u.unflatten_func_params.buf_sizeLu,
                extra->u.unflatten_func_params.flat_dataPV,
                extra->u.unflatten_func_params.arbPH);
            break;

        case PF_Arbitrary_INTERP_FUNC:
            err = Arb_Interpolate(
                in_data,
                extra->u.interp_func_params.tF,
                &extra->u.interp_func_params.left_arbH,
                &extra->u.interp_func_params.right_arbH,
                extra->u.interp_func_params.interpPH);
            break;

        case PF_Arbitrary_COMPARE_FUNC:
            err = Arb_Compare(
                in_data,
                &extra->u.compare_func_params.a_arbH,
                &extra->u.compare_func_params.b_arbH,
                extra->u.compare_func_params.compareP);
            break;

        case PF_Arbitrary_PRINT_SIZE_FUNC:
            *(extra->u.print_size_func_params.print_sizePLu) = TESTFX_ARB_MAX_PRINT_SIZE;
            break;

        case PF_Arbitrary_PRINT_FUNC:
            err = Arb_Print(
                in_data,
                extra->u.print_func_params.arbH,
                extra->u.print_func_params.print_sizeLu,
                extra->u.print_func_params.print_bufferPC);
            break;

        case PF_Arbitrary_SCAN_FUNC:
            // Scan (text parsing) is not required for serialization testing
            break;

        default:
            break;
    }

    return err;
}
