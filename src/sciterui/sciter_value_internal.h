#pragma once
#include <sciter-x-value.h>
#include <sciter_value.h>

SciterValue ConvertToSciterValue(const sciter::value & rv);
sciter::value ConvertFromSciterValue(const SciterValue & rv);
