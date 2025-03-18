#pragma once
#include <stdint.h>
#include <string>

class SciterValue
{
    enum VALUE_TYPE
    {
        TYPE_UNDEFINED = 0,
        TYPE_BOOL = 1,
        TYPE_INT32_T = 2,
        TYPE_STRING = 3,
    };

public:
    SciterValue();
    SciterValue(bool Value);
    SciterValue(int32_t Value);
    SciterValue(std::string Value);
    ~SciterValue();

    int32_t GetValueInt(void) const;

private:
    uint32_t m_type;
    uint32_t m_valueInt;
    std::string m_valueStr;
};
