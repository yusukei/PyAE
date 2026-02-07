/*******************************************************************/
/*                                                                 */
/*                 TestEffect_Strings.cpp                          */
/*                                                                 */
/*  String table implementation for TestEffect                     */
/*  Implements GetStringPtr() declared in String_Utils.h           */
/*                                                                 */
/*******************************************************************/

#include "TestEffect.h"

typedef struct {
    unsigned long   index;
    char            str[256];
} TableString;

TableString g_strs[StrID_NUMTYPES] = {
    StrID_NONE,                 "",
    StrID_Name,                 "PyAE Test Effect",
    StrID_Description,          "Test effect for PyAE custom property validation.\rVersion 1.0",

    // Basic Types Group
    StrID_Group_Basic,          "Basic Types",
    StrID_Slider_Int,           "Integer Slider",
    StrID_Slider_Float,         "Float Slider",
    StrID_Slider_Percent,       "Percent",
    StrID_Checkbox,             "Checkbox",
    StrID_Checkbox_Desc,        "Enable test option",
    StrID_Group_Basic_End,      "Basic Types",

    // Spatial Types Group
    StrID_Group_Spatial,        "Spatial Types",
    StrID_Angle,                "Angle",
    StrID_Color,                "Color",
    StrID_Point_2D,             "Point 2D",
    StrID_Point_3D,             "Point 3D",
    StrID_Group_Spatial_End,    "Spatial Types",

    // Reference Types Group
    StrID_Group_Reference,      "Reference Types",
    StrID_Popup,                "Popup Menu",
    StrID_Popup_Choices,        "Option 1|Option 2|Option 3|Option 4|Option 5",
    StrID_Layer_Ref,            "Layer Reference",
    StrID_Group_Reference_End,  "Reference Types",

    // Advanced Types Group
    StrID_Group_Advanced,       "Advanced",
    StrID_Button,               "Test Action",
    StrID_Button_Label,         "Click Me",
    StrID_Button_Message,       "Button clicked! TestEffect is working correctly.",
    StrID_Arbitrary,            "Custom Data",
    StrID_Group_Advanced_End,   "Advanced",
};

char* GetStringPtr(int strNum)
{
    return g_strs[strNum].str;
}
