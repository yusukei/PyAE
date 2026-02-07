/*******************************************************************/
/*                                                                 */
/*                   TestEffect_Strings.h                          */
/*                                                                 */
/*  String identifiers for TestEffect                              */
/*  Note: GetStringPtr and STR macro are provided by String_Utils.h*/
/*                                                                 */
/*******************************************************************/

#pragma once

typedef enum {
    StrID_NONE = 0,
    StrID_Name,
    StrID_Description,

    // Basic Types Group
    StrID_Group_Basic,
    StrID_Slider_Int,
    StrID_Slider_Float,
    StrID_Slider_Percent,
    StrID_Checkbox,
    StrID_Checkbox_Desc,
    StrID_Group_Basic_End,

    // Spatial Types Group
    StrID_Group_Spatial,
    StrID_Angle,
    StrID_Color,
    StrID_Point_2D,
    StrID_Point_3D,
    StrID_Group_Spatial_End,

    // Reference Types Group
    StrID_Group_Reference,
    StrID_Popup,
    StrID_Popup_Choices,
    StrID_Layer_Ref,
    StrID_Group_Reference_End,

    // Advanced Types Group
    StrID_Group_Advanced,
    StrID_Button,
    StrID_Button_Label,
    StrID_Button_Message,
    StrID_Arbitrary,
    StrID_Group_Advanced_End,

    StrID_NUMTYPES
} StrIDType;
