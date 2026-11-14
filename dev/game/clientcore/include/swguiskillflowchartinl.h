#ifndef __SWGUISKILLFLOWCHARTINL_H__
#define __SWGUISKILLFLOWCHARTINL_H__

#include "swguiskillflowchart.h"

// PC 0x00736BE0 GOG 0x00736940
inline int CSWGuiSkillFlowChart::GetSelected()
{
    if (m_cFlows.num > 0)
    {
        return static_cast<CSWGuiSkillFlow*>(m_cFlows.element[m_nSelectedRow])
            ->m_aSkills[m_nSelectedCol]
            .m_nSkill;
    }
    return -1;
}

#endif
