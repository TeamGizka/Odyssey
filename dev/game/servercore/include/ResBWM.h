#ifndef __RESBWM_H__
#define __RESBWM_H__

#include "exoresman.h"
#include "math3d.h"

class _CSWWalkMeshHeader
{
public:
    char m_sVersion[8];
    int m_nType;
    Vector m_avRelativeUsePoints[2];
    Vector m_avAbsoluteUsePoints[2];
    Vector m_vPosition;
    int m_nVertexCount;
    int m_nVertexOffset;
    int m_nFaceCount;
    int m_nFaceOffset;
    int m_nMaterialOffset;
    int m_nNormalOffset;
    int m_nDistanceOffset;
    int m_nAABBNodeCount;
    int m_nAABBNodeOffset;
    int m_nAABBRootNode;
    int m_nAdjacencyCount;
    int m_nAdjacencyOffset;
    int m_nEdgeCount;
    int m_nEdgeOffset;
    int m_nPerimeterCount;
    int m_nPerimeterOffset;
    // PC 0x005808E0
    _CSWWalkMeshHeader()
    {
    }
};

class CResBWM : public CRes
{
public:
    int m_bServiced;
    unsigned long m_nBWMSize;
    unsigned char* m_pBWMData;
    _CSWWalkMeshHeader* m_pBWMHeader;
    unsigned char* GetBWMDataPtr();
    CResBWM();
    ~CResBWM();
    virtual int OnResourceFreed();
    virtual int OnResourceServiced();
    int IsBinary();
    unsigned long GetBWMSize();
    // PC 0x005CE010
    Vector GetAbsoluteUsePoint(int nIndex)
    {
        return m_pBWMHeader ? m_pBWMHeader->m_avAbsoluteUsePoints[nIndex]
                            : Vector(0.0f, 0.0f, 0.0f);
    }
};

#endif
