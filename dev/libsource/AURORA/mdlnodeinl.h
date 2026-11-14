#ifndef __MDLNODEINL_H__
#define __MDLNODEINL_H__

#include "aurmodel.h"

// PC 0x00448F30 GOG 0x00449080
inline char* MdlNode::TranslateNodeName()
{
    if (m_pModel->AsModel())
    {
        return m_pModel->AsModel()->m_lNames[m_nIndex];
    }
    else
    {
        return ((Model*)m_pModel->AsAnimation()->m_pBinaryData)->m_lNames[m_nIndex];
    }
}

#endif
