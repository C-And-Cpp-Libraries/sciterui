#include "sciter_value_internal.h"
#include "std_string.h"

#include <sciter-x-api.h>

SciterValue ConvertToSciterValue(const sciter::value & rv)
{
    UINT Type, Units;
    ValueType(&rv, &Type, &Units);

    if (Type == T_BOOL)
    {
        int v;
        if (ValueIntData(&rv, &v) == HV_OK)
        {
            return SciterValue(v != 0);
        }
    }
    else if (Type == T_INT)
    {
        int v;
        if (ValueIntData(&rv, &v) == HV_OK)
        {
            return SciterValue(v);
        }
    }
    else if (Type == T_STRING)
    {
        LPCWSTR c;
        UINT l;
        if (ValueStringData(&rv, &c, &l) == HV_OK)
        {
            if (l == 0)
            {
                return SciterValue();
            }
            return SciterValue(SciterUI::stdstr().FromUTF16(c));
        }
    }
    __debugbreak();
    return SciterValue();
}