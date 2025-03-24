#pragma once
#include "sciter_ui.h"
#include <stdint.h>

__interface IClickSink
{
    bool OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t reason);
};
static const char * IID_ICLICKSINK = "591A7458-0F80-4A52-A68E-89B1E4F16FB3";