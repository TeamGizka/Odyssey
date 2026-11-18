#ifndef __AURMATERIALINL_H__
#define __AURMATERIALINL_H__

#include "aurmaterial.h"

// PC 0x00489860 GOG 0x00489930 Xbox 0x002CF9F0
inline Vector Material::GetBumpMapSpecularColor()
{
    CAurTexture* pBumpMap = (CAurTexture*)m_pBumpMap.m_pObject;
    return pBumpMap->GetSpecularColor();
}

#endif
