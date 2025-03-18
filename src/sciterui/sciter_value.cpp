#include <sciter_value.h>

SciterValue::SciterValue() :
    m_type(TYPE_UNDEFINED),
    m_valueInt(0)
{
}

SciterValue::~SciterValue()
{
}

SciterValue::SciterValue(bool Value) :
    m_type(TYPE_BOOL),
    m_valueInt(Value)
{
}

SciterValue::SciterValue(int32_t Value) :
    m_type(TYPE_INT32_T),
    m_valueInt(Value)
{
}

SciterValue::SciterValue(std::string Value) :
    m_type(TYPE_STRING),
    m_valueStr(std::move(Value))
{
}

int32_t SciterValue::GetValueInt(void) const
{
    if (m_type != TYPE_INT32_T)
    {
        __debugbreak();
        return 0;
    }
    return m_valueInt;
}
