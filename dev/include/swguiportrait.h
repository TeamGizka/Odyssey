#ifndef __SWGUIPORTRAIT_H__
#define __SWGUIPORTRAIT_H__

#include "swgui.h"

static inline void SetPortraitImage(CSWGuiLabel* pLabel,
                                    const CResRef& cPortrait)
{
    pLabel->m_cBorder.m_cParams.SetFillImage(cPortrait, 0);
}

#endif
