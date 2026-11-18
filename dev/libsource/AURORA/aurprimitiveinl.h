#ifndef __AURPRIMITIVEINL_H__
#define __AURPRIMITIVEINL_H__

#include "aurprimitive.h"

// PC 0x0044A340 GOG 0x0044A480 Xbox 0x002ABE00
inline NodeVertexAnim::~NodeVertexAnim()
{
    m_lstPositions.SetSize(0);
    m_lstNormals.SetSize(0);
    m_lstTexCoords.SetSize(0);
}

#endif
