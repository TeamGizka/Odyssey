#ifndef __AURTEXPTR_H__
#define __AURTEXPTR_H__

#include "aurpointer.h"

class CAurTexture;

static inline CAurTexture* AsTexture(SafePointer<CAurTexture>& pPointer)
{
    return (CAurTexture*)pPointer.m_pObject;
}

#endif
