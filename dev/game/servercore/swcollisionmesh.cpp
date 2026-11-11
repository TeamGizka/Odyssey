#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "aurora.h"
#include "exobase.h"
#include "ResBWM.h"
#include "exoresman.h"
#include "SWCollisionMesh.h"
// PC 0x00556FF0
CSWCollisionMesh::~CSWCollisionMesh()
{
    if (m_bBinary != 0)
    {
        m_pVertices = NULL;
        m_pFaces = NULL;
        m_pFaceMaterials = NULL;
        m_pFaceDistances = NULL;
        m_pFaceNormals = NULL;
        m_pRes->Release();
        if (g_pExoResMan->ReleaseResObject(m_pRes) == 0)
        {
            if (m_pRes != NULL)
            {
                delete m_pRes;
            }
        }
        m_pRes = NULL;
    }
    else
    {
        delete m_pVertices;
        m_pVertices = NULL;
        delete m_pFaces;
        m_pFaces = NULL;
        delete m_pFaceMaterials;
        m_pFaceMaterials = NULL;
        delete m_pFaceNormals;
        m_pFaceNormals = NULL;
        delete m_pFaceDistances;
        m_pFaceDistances = NULL;
    }
}
// PC 0x005571B0
int CSWCollisionMesh::LoadMesh(int nType)
{
    int nResult = 1;
    CResRef cResRef;

    GetResourceForBinaryWalkMesh(cResRef);
    if (g_pExoResMan->Exists(cResRef, m_nBinaryResType, (unsigned long*)NULL))
    {
        CResBWM* pRes =
            (CResBWM*)g_pExoResMan->GetResObject(cResRef, m_nBinaryResType);
        if (pRes == NULL)
        {
            pRes = new CResBWM;
            g_pExoResMan->SetResObject(cResRef, m_nBinaryResType, pRes);
        }
        if (pRes->Demand())
        {
            if (pRes->IsBinary())
            {
                if (nType)
                {
                    pRes->Release();
                    pRes->Dump(1);
                    return 0;
                }
                m_bBinary = 1;
                nResult = LoadMeshBinary(pRes);
                m_pRes = pRes;
            }
            else
            {
                m_bBinary = 0;
                nResult = LoadMeshText(pRes->GetBWMDataPtr(),
                                       pRes->GetBWMSize());
                pRes->Release();
                pRes->Dump(1);
            }
        }
    }
    else
    {
        GetResourceForTextWalkMesh(cResRef);
        if (g_pExoResMan->Exists(cResRef, m_nTextResType, (unsigned long*)NULL))
        {
            CResMDL* pRes =
                (CResMDL*)g_pExoResMan->GetResObject(cResRef, m_nTextResType);
            if (pRes == NULL)
            {
                pRes = new CResMDL;
                g_pExoResMan->SetResObject(cResRef, m_nTextResType, pRes);
            }
            if (pRes->Demand())
            {
                m_bBinary = 0;
                nResult = LoadMeshText(pRes->GetMDLDataPtr(),
                                       pRes->GetMDLSize());
                pRes->Release();
                pRes->Dump(1);
            }
        }
    }
    return nResult;
}
// PC 0x00557560
int CSWCollisionMesh::LoadMeshText(unsigned char* pData, unsigned long nSize)
{
    m_bBinary = 0;
    return 1;
}
// PC 0x005575B0
int CSWCollisionMesh::LoadMeshString(unsigned char** ppData,
                                     unsigned long* pnSize,
                                     unsigned char* pBuffer,
                                     unsigned long nBufferSize)
{
    unsigned long nIndex = -1;
    if (*pnSize == 0)
    {
        return 0;
    }
    do
    {
        nIndex++;
        pBuffer[nIndex] = *(*ppData)++;
        (*pnSize)--;
    } while (nIndex < nBufferSize - 1 && pBuffer[nIndex] != 10 && *pnSize > 0);
    if (nIndex >= nBufferSize)
    {
        return 0;
    }
    pBuffer[nIndex] = 0;
    return 1;
}
// PC 0x005576B0
void CSWCollisionMesh::SetTriangleCount(int nCount)
{
    m_nFaceCount = nCount;
    if (m_bBinary == 0)
    {
        m_pFaces = new int[3 * nCount];
        m_pFaceMaterials = new int[nCount];
        m_pFaceDistances = new float[nCount];
        m_pFaceNormals = new Vector[nCount];
    }
}
// PC 0x00557750
void CSWCollisionMesh::SetVertexCount(int nCount)
{
    m_nVertexCount = nCount;
    m_pVertices = new Vector[nCount];
}
// PC 0x005577B0
Vector CSWCollisionMesh::WorldToLocal(const Vector& vPosition)
{
    if (m_nType != 0)
    {
        return vPosition;
    }
    return -m_qOrientation * (vPosition - m_vWorldPosition) - m_vPosition;
}
// PC 0x00557980
Vector CSWCollisionMesh::LocalToWorld(const Vector& vPosition)
{
    if (m_nType != 0)
    {
        return vPosition;
    }
    return m_vWorldPosition + m_qOrientation * (m_vPosition + vPosition);
}

Vector CSWCollisionMesh::NormalLocalToWorld(const Vector& vLocal)
{
    if (m_nType != 0)
    {
        return vLocal;
    }
    return m_qOrientation * vLocal;
}
// PC 0x00557C90
CSWCollisionMesh::CSWCollisionMesh(unsigned short nTextResType,
                                   unsigned short nBinaryResType)
{
    m_nBinaryResType = nBinaryResType;
    m_nTextResType = nTextResType;
    m_bBinary = 0;
    m_nType = 0;
    m_pRes = NULL;
    m_nVertexCount = 0;
    m_pVertices = NULL;
    m_nFaceCount = 0;
    m_nAdjacencyCount = 0;
    m_pFaces = NULL;
    m_pFaceMaterials = NULL;
    m_pFaceNormals = NULL;
    m_pFaceDistances = NULL;
}
// PC 0x005570B0
void CSWCollisionMesh::ComputeMeshNormals()
{
    if (m_bBinary == 0)
    {
        for (int i = 0; i < m_nFaceCount; i++)
        {
            Vector vFirst = m_pVertices[m_pFaces[3 * i]];
            Vector vSecond = m_pVertices[m_pFaces[3 * i + 1]];
            Vector vThird = m_pVertices[m_pFaces[3 * i + 2]];
            Vector vNormal = NormalOf(vFirst, vSecond, vThird);
            m_pFaceNormals[i] = vNormal;
            m_pFaceDistances[i] = -(vNormal * vFirst);
        }
    }
}
// PC 0x00557D20
int CSWCollisionMesh::GetMeshBoundingBox(Vector& vMin, Vector& vMax)
{
    if (m_pVertices == NULL)
    {
        return 0;
    }
    int bFirst = 0;
    Vector vVertex;
    for (int i = 0; i < m_nVertexCount; i++)
    {
        vVertex = LocalToWorld(m_pVertices[i]);
        if (bFirst == 0)
        {
            vMax = vVertex;
            vMin = vVertex;
            bFirst = 1;
        }
        else
        {
            if (vVertex.m_fX > vMax.m_fX)
            {
                vMax.m_fX = vVertex.m_fX;
            }
            if (vVertex.m_fX < vMin.m_fX)
            {
                vMin.m_fX = vVertex.m_fX;
            }
            if (vVertex.m_fY > vMax.m_fY)
            {
                vMax.m_fY = vVertex.m_fY;
            }
            if (vVertex.m_fY < vMin.m_fY)
            {
                vMin.m_fY = vVertex.m_fY;
            }
            if (vVertex.m_fZ > vMax.m_fZ)
            {
                vMax.m_fZ = vVertex.m_fZ;
            }
            if (vVertex.m_fZ < vMin.m_fZ)
            {
                vMin.m_fZ = vVertex.m_fZ;
            }
        }
    }
    return bFirst;
}
// PC 0x00557E50
int CSWCollisionMesh::GetMeshOutline(Vector& vFirst, Vector& vSecond,
                                     Vector& vThird, Vector& vFourth)
{
    if (m_pVertices == NULL)
    {
        return 0;
    }
    int bFirst = 0;
    Vector vMin;
    Vector vMax;
    for (int i = 0; i < m_nVertexCount; i++)
    {
        Vector vVertex = m_pVertices[i];
        if (bFirst == 0)
        {
            vMax = vVertex;
            vMin = vVertex;
            bFirst = 1;
        }
        else
        {
            if (vVertex.m_fX > vMax.m_fX)
            {
                vMax.m_fX = vVertex.m_fX;
            }
            if (vVertex.m_fX < vMin.m_fX)
            {
                vMin.m_fX = vVertex.m_fX;
            }
            if (vVertex.m_fY > vMax.m_fY)
            {
                vMax.m_fY = vVertex.m_fY;
            }
            if (vVertex.m_fY < vMin.m_fY)
            {
                vMin.m_fY = vVertex.m_fY;
            }
            if (vVertex.m_fZ > vMax.m_fZ)
            {
                vMax.m_fZ = vVertex.m_fZ;
            }
            if (vVertex.m_fZ < vMin.m_fZ)
            {
                vMin.m_fZ = vVertex.m_fZ;
            }
        }
    }
    vFirst = vMin;
    vFirst.m_fY = vMax.m_fY;
    vFirst.m_fZ = vMin.m_fZ;
    vSecond = vMin;
    vSecond.m_fZ = vMin.m_fZ;
    vThird = vMax;
    vThird.m_fY = vMin.m_fY;
    vThird.m_fZ = vMin.m_fZ;
    vFourth = vMax;
    vFourth.m_fZ = vMin.m_fZ;
    vFirst = LocalToWorld(vFirst);
    vSecond = LocalToWorld(vSecond);
    vThird = LocalToWorld(vThird);
    vFourth = LocalToWorld(vFourth);
    return bFirst;
}
// PC 0x005580A0
void CSWCollisionMesh::GetVertex(int nVertex, float* pfX, float* pfY,
                                 float* pfZ)
{
    if (m_pVertices != NULL && nVertex < m_nVertexCount)
    {
        Vector vVertex = LocalToWorld(m_pVertices[nVertex]);
        *pfX = vVertex.m_fX;
        *pfY = vVertex.m_fY;
        *pfZ = vVertex.m_fZ;
    }
}
// PC 0x005580F0
void CSWCollisionMesh::GetVertex(int nVertex, Vector* pvVertex)
{
    if (m_pVertices != NULL && nVertex < m_nVertexCount)
    {
        *pvVertex = LocalToWorld(m_pVertices[nVertex]);
    }
}
// PC 0x005573D0
int CSWCollisionMesh::LoadMeshBinary(CResBWM* pMesh)
{
    m_bBinary = 1;
    m_nType = pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_nType : 0;
    m_avUsePoints[0] = pMesh->m_pBWMHeader
                           ? pMesh->m_pBWMHeader->m_avRelativeUsePoints[0]
                           : Vector(0.0f, 0.0f, 0.0f);
    m_avUsePoints[1] = pMesh->m_pBWMHeader
                           ? pMesh->m_pBWMHeader->m_avRelativeUsePoints[1]
                           : Vector(0.0f, 0.0f, 0.0f);
    m_nVertexCount =
        pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_nVertexCount : 0;
    m_vPosition = pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_vPosition
                                      : Vector(0.0f, 0.0f, 0.0f);
    m_pVertices = pMesh->m_pBWMHeader
                      ? (Vector*)(pMesh->m_pBWMData +
                                  pMesh->m_pBWMHeader->m_nVertexOffset)
                      : NULL;
    m_nFaceCount = pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_nFaceCount : 0;
    m_nAdjacencyCount =
        pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_nAdjacencyCount : 0;
    m_pFaces = pMesh->m_pBWMHeader
                   ? (int*)(pMesh->m_pBWMData +
                            pMesh->m_pBWMHeader->m_nFaceOffset)
                   : NULL;
    m_pFaceMaterials = pMesh->m_pBWMHeader
                           ? (int*)(pMesh->m_pBWMData +
                                    pMesh->m_pBWMHeader->m_nMaterialOffset)
                           : NULL;
    m_pFaceNormals = pMesh->m_pBWMHeader
                         ? (Vector*)(pMesh->m_pBWMData +
                                     pMesh->m_pBWMHeader->m_nNormalOffset)
                         : NULL;
    m_pFaceDistances = pMesh->m_pBWMHeader
                           ? (float*)(pMesh->m_pBWMData +
                                      pMesh->m_pBWMHeader->m_nDistanceOffset)
                           : NULL;
    return 1;
}

static int WalkTestMaterial = 1;
static int rendercollisionboxes = 0;

int bDumpData = 0;

inline void CSWCollisionMesh::GetFaceVertex(int nIndex, int nCorner,
                                            Vector& vVertex)
{
    int* pFaces = m_pFaces;
    if (pFaces[nIndex + nCorner] >= 0)
        vVertex = m_pVertices[pFaces[nIndex + nCorner]];
}
// PC 0x00558390
int CSWCollisionMesh::NoNonWalkPolys(const Vector& vStart,
    const Vector& vEnd, float fMinHeight, float fMaxHeight,
    float fRadius, CWalkHitInfo* pHitInfo)
{
    Vector vLocalStart = WorldToLocal(vStart);
    Vector vLocalEnd = WorldToLocal(vEnd);
    if (bDumpData)
        EXOLOG("CSWCollisionMesh::NoNonWalkPolys: %s %s %s %s %s %s %s %s %s\n", "face", "material", "outside0", "outside1", "outside2", "edge0", "edge1", "edge2", "hit");
    float fBottom;
    float fTop;
    Vector vMin, vMax;
    if (m_nType)
        fBottom = fMinHeight;
    else
        fBottom = fMinHeight - m_vWorldPosition.m_fZ - m_vPosition.m_fZ;
    if (m_nType)
        fTop = fMaxHeight;
    else
        fTop = fMaxHeight - m_vWorldPosition.m_fZ - m_vPosition.m_fZ;
    if (vLocalEnd.m_fX == vLocalStart.m_fX &&
        vLocalEnd.m_fY == vLocalStart.m_fY)
        return 1;

    Vector avCorridor[4];
    Vector vIntersection;
    Vector vMove;
    vMove.m_fX = vLocalEnd.m_fX - vLocalStart.m_fX;
    vMove.m_fY = vLocalEnd.m_fY - vLocalStart.m_fY;
    vMove.m_fZ = 0.0f;
    float fLength = magnitude(vMove);
    Vector vDirection = vMove / fLength;
    vDirection = normalize(vMove);
    Vector vSide(vDirection.m_fY, -vDirection.m_fX, 0.0f);
    Vector vReach = vDirection * fRadius;
    Vector vOffset(-vReach.m_fY, vReach.m_fX, 0.0f);
    avCorridor[0] = vLocalStart + vOffset;
    Vector vReachEnd = vLocalEnd + vReach;
    avCorridor[1] = vReachEnd + vOffset;
    avCorridor[2] = vLocalEnd + vReach - vOffset;
    avCorridor[3] = vLocalStart - vOffset;
    float fWidth = fRadius + fRadius;
    int nCorner;
    if (vLocalEnd.m_fX >= vLocalStart.m_fX)
        nCorner = (vLocalEnd.m_fY < vLocalStart.m_fY) ? 0 : 1;
    else
        nCorner = (vLocalEnd.m_fY < vLocalStart.m_fY) ? 3 : 2;
    vMin.m_fX = avCorridor[(nCorner + 3) % 4].m_fX - 0.01f;
    vMin.m_fY = avCorridor[(nCorner + 2) % 4].m_fY - 0.01f;
    vMax.m_fX = avCorridor[(nCorner + 1) % 4].m_fX + 0.01f;
    vMax.m_fY = avCorridor[nCorner % 4].m_fY + 0.01f;
    Vector vPush = normalize(avCorridor[0] - avCorridor[1]) * fRadius;
    avCorridor[1] = Vector(vPush.m_fX + avCorridor[1].m_fX, avCorridor[1].m_fY + vPush.m_fY, avCorridor[1].m_fZ + vPush.m_fZ);
    Vector vPush2 = normalize(avCorridor[3] - avCorridor[2]) * fRadius;
    avCorridor[2] = Vector(vPush2.m_fX + avCorridor[2].m_fX, (avCorridor[2].m_fY + vPush2.m_fY), avCorridor[2].m_fZ + vPush2.m_fZ);

    if (rendercollisionboxes == 1)
    {
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[0]),
                LocalToWorld(avCorridor[1]),
                LocalToWorld(avCorridor[2]),
                LocalToWorld(avCorridor[3])
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF00FFF);
        }
        Vector vLevel0 = avCorridor[0];
        vLevel0.m_fZ = fBottom;
        Vector vLevel1 = avCorridor[1];
        vLevel1.m_fZ = fBottom;
        Vector vLevel2 = avCorridor[2];
        vLevel2.m_fZ = fBottom;
        Vector vLevel3 = avCorridor[3];
        vLevel3.m_fZ = fBottom;
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[0]),
                LocalToWorld(avCorridor[1]),
                LocalToWorld(vLevel0),
                LocalToWorld(vLevel1)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF0FFF0);
        }
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[1]),
                LocalToWorld(avCorridor[2]),
                LocalToWorld(vLevel1),
                LocalToWorld(vLevel2)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF0FFF0);
        }
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[2]),
                LocalToWorld(avCorridor[3]),
                LocalToWorld(vLevel2),
                LocalToWorld(vLevel3)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF0FFF0);
        }
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[3]),
                LocalToWorld(avCorridor[0]),
                LocalToWorld(vLevel3),
                LocalToWorld(vLevel0)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF0FFF0);
        }
        vLevel0.m_fZ = fTop;
        vLevel1.m_fZ = fTop;
        vLevel2.m_fZ = fTop;
        vLevel3.m_fZ = fTop;
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[0]),
                LocalToWorld(avCorridor[1]),
                LocalToWorld(vLevel0),
                LocalToWorld(vLevel1)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF0F0F0);
        }
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[1]),
                LocalToWorld(avCorridor[2]),
                LocalToWorld(vLevel1),
                LocalToWorld(vLevel2)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF0F0F0);
        }
        if (bDumpData)
            EXOLOG("  start z %f\n", vLocalStart.m_fZ);
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[2]),
                LocalToWorld(avCorridor[3]),
                LocalToWorld(vLevel2),
                LocalToWorld(vLevel3)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF0F0F0);
        }
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(avCorridor[3]),
                LocalToWorld(avCorridor[0]),
                LocalToWorld(vLevel3),
                LocalToWorld(vLevel0)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF0F0F0);
        }
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(vLocalEnd),
                LocalToWorld(vLocalEnd + vReach),
                LocalToWorld(vLocalEnd + vReach),
                LocalToWorld(vLocalEnd)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, AUR_DRAWABLE_COLOR_WHITE);
        }
        {
            static int REC_NUM = -1;
            Vector vTop1 = avCorridor[1];
            Vector vTop2 = avCorridor[2];
            vTop1.m_fZ = fTop;
            vTop2.m_fZ = fTop;
            Vector avSquare[4] =
            {
                LocalToWorld(vTop1),
                LocalToWorld(vTop1),
                LocalToWorld(vTop2),
                LocalToWorld(vTop2)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, AUR_DRAWABLE_COLOR_WHITE);
        }
    }
    if (bDumpData)
        EXOLOG("  end z %f\n", vLocalEnd.m_fZ);
    Vector avTriangle[3];
    int bVertexHit = 0;
    int bHit = 0;
    Vector vClosestHit;
    Vector vFirst;
    Vector vSecond;
    float fClosest = FLT_MAX;
    int nHitFace;
    int nHitEdge;
    int nMaterialMask = -1;

    int bScan = TRUE;
    if (WalkTestMaterial)
        nMaterialMask = GetWalkCheckMaterialMask();
    for (int nFace = m_nAdjacencyCount; nFace < m_nFaceCount && bScan; ++nFace)
    {
        int i;
        if (!bScan)
            break;
        int nIndex = nFace * 3;
        for (i = 0; i < 3; ++i)
            GetFaceVertex(nIndex, i, avTriangle[i]);
        if (!(fBottom <= fTop) ||
            !((1 << m_pFaceMaterials[nFace]) & nMaterialMask) ||
            !(avTriangle[0].m_fZ <= fTop || avTriangle[1].m_fZ <= fTop || avTriangle[2].m_fZ <= fTop) ||
            !(avTriangle[0].m_fZ >= fBottom || avTriangle[1].m_fZ >= fBottom || avTriangle[2].m_fZ >= fBottom) ||
            !(avTriangle[0].m_fX <= vMax.m_fX || avTriangle[1].m_fX <= vMax.m_fX || avTriangle[2].m_fX <= vMax.m_fX) ||
            !(avTriangle[0].m_fY <= vMax.m_fY || avTriangle[1].m_fY <= vMax.m_fY || avTriangle[2].m_fY <= vMax.m_fY) ||
            !(avTriangle[0].m_fX >= vMin.m_fX || avTriangle[1].m_fX >= vMin.m_fX || avTriangle[2].m_fX >= vMin.m_fX) ||
            !(avTriangle[0].m_fY >= vMin.m_fY || avTriangle[1].m_fY >= vMin.m_fY || avTriangle[2].m_fY >= vMin.m_fY))
            continue;

        int abOutside[3];
        int abTestEdge[3] = { 1, 1, 1 };
        for (i = 0; i < 3; ++i)
        {
            vFirst = avTriangle[i];
            Vector vDelta = vFirst - avCorridor[0];
            if (!(vDelta * vDirection <= fLength &&
                  vDelta * vDirection >= 0.0f))
            {
                Vector vEndDelta;
                vEndDelta.m_fX = vFirst.m_fX - vLocalEnd.m_fX;
                vEndDelta.m_fY = vFirst.m_fY - vLocalEnd.m_fY;
                vEndDelta.m_fZ = vFirst.m_fZ - vLocalEnd.m_fZ;
                if (vEndDelta * vEndDelta <= fRadius * fRadius)
                {
                    abOutside[i] = 0;
                }
                else
                {
                    abOutside[i] = 1;
                    continue;
                }
                if (vFirst.m_fZ >= fBottom && vFirst.m_fZ <= fTop)
                {
                    if (pHitInfo == NULL)
                        return 0;
                    Vector vToFirst = vFirst - vLocalStart;
                    float fDistance = vToFirst * vDirection;
                    if (fDistance < fClosest)
                    {
                        fClosest = fDistance;
                        bHit = 1;
                        bVertexHit = 1;
                        vClosestHit = vFirst;
                        nHitFace = nFace;
                        nHitEdge = i;
                    }
                }
            }
            else
            {
                if (!(vDelta * vSide <= fWidth &&
                      vDelta * vSide >= 0.0f))
                {
                    abOutside[i] = 1;
                    continue;
                }
                abOutside[i] = 0;
                if (vFirst.m_fZ >= fBottom && vFirst.m_fZ <= fTop)
                {
                    if (pHitInfo == NULL)
                        return 0;
                    Vector vToFirst = vFirst - vLocalStart;
                    float fDistance = vToFirst * vDirection;
                    if (fDistance < fClosest)
                    {
                        fClosest = fDistance;
                        bHit = 1;
                        bVertexHit = 1;
                        vClosestHit = vFirst;
                        nHitFace = nFace;
                        nHitEdge = i;
                    }
                }
            }
        }
        if (!abOutside[0] && !abOutside[1])
            abTestEdge[0] = 0;
        if (!abOutside[1] && !abOutside[2])
            abTestEdge[1] = 0;
        if (!abOutside[2] && !abOutside[0])
            abTestEdge[2] = 0;

        for (i = 0; i < 3; ++i)
        {
            vFirst = avTriangle[i];
            vSecond = avTriangle[(i + 1) % 3];
            if (abTestEdge[i] == 1)
            {
                for (int j = 0; j < 4; ++j)
                {
                    if (j == 1)
                        continue;
                    if (IntersectLineSegments2d(vFirst, vSecond,
                        avCorridor[j], avCorridor[(j + 1) % 4], vIntersection) == 1)
                    {
                        if (pHitInfo == NULL)
                            return 0;
                        Vector vToIntersection(vIntersection.m_fX - vLocalStart.m_fX, vIntersection.m_fY - vLocalStart.m_fY, vIntersection.m_fZ - vLocalStart.m_fZ);
                        float fDistance = vToIntersection * vDirection;
                        if (fDistance < fClosest)
                        {
                            if (rendercollisionboxes == 1)
                            {
                                static int REC_NUM = -1;
                                Vector avSquare[4] =
                                {
                                    LocalToWorld(avCorridor[j]),
                                    LocalToWorld(avCorridor[j]),
                                    LocalToWorld(avCorridor[(j + 1) % 4]),
                                    LocalToWorld(avCorridor[(j + 1) % 4])
                                };
                                if (REC_NUM != -1)
                                    AurRemoveDrawableSquare(REC_NUM);
                                REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, AUR_DRAWABLE_COLOR_WHITE);
                            }
                            bHit = 1;
                            bVertexHit = 0;
                            vClosestHit = vIntersection;
                            nHitFace = nFace;
                            nHitEdge = i;
                            fClosest = fDistance;
                        }
                    }
                }
                Vector vCircleFirst = vFirst;
                Vector vCircleSecond = vSecond;
                Vector vCircleCenter = vLocalEnd;
                Vector vIntersection1;
                Vector vIntersection2;
                vCircleFirst.m_fZ = 0.0f;
                vCircleSecond.m_fZ = 0.0f;
                vCircleCenter.m_fZ = 0.0f;
                int nIntersections = IntersectCircle2d(vCircleCenter, fRadius + 0.001f,
                    vCircleFirst, vCircleSecond, vIntersection1, vIntersection2,
                    pHitInfo != NULL);
                if (pHitInfo != NULL)
                {
                    switch (nIntersections)
                    {
                    case 2:
                    {
                        Vector vToIntersection(vIntersection2.m_fX - vLocalStart.m_fX, vIntersection2.m_fY - vLocalStart.m_fY, vIntersection2.m_fZ - vLocalStart.m_fZ);
                        float fDistance = vToIntersection * vDirection;
                        if (fDistance >= 0.0f && fDistance < fClosest)
                        {
                            fClosest = fDistance;
                            vClosestHit = vIntersection2;
                            nHitFace = nFace;
                            bHit = 1;
                            bVertexHit = 0;
                            nHitEdge = i % 3;
                        }
                    }
                    case 1:
                    {
                        Vector vToIntersection(vIntersection1.m_fX - vLocalStart.m_fX, vIntersection1.m_fY - vLocalStart.m_fY, vIntersection1.m_fZ - vLocalStart.m_fZ);
                        float fDistance = vToIntersection * vDirection;
                        if (fDistance >= 0.0f && fDistance < fClosest)
                        {
                            fClosest = fDistance;
                            vClosestHit = vIntersection1;
                            nHitFace = nFace;
                            bHit = 1;
                            bVertexHit = 0;
                            nHitEdge = i % 3;
                        }
                    }
                    }
                }
                else if (nIntersections > 0)
                    return 0;
            }
        }
        Vector vPerpendicular;
        Vector vEdge = avTriangle[1] - avTriangle[0];
        vPerpendicular = cross(zaxis, avTriangle[0] - vLocalStart);
        if (!(vEdge * vPerpendicular >= 0.0f))
            continue;
        vEdge = avTriangle[2] - avTriangle[1];
        vPerpendicular = cross(zaxis, avTriangle[1] - vLocalStart);
        if (!(vEdge * vPerpendicular >= 0.0f))
            continue;
        vEdge = avTriangle[0] - avTriangle[2];
        vPerpendicular = cross(zaxis, avTriangle[2] - vLocalStart);
        if (!(vEdge * vPerpendicular >= 0.0f))
            continue;
        if (pHitInfo != NULL)
        {
            pHitInfo->m_nUnknown00 = CWalkHitInfo::FAILURE_INSIDE_TRIANGLE;
            pHitInfo->m_nUnknown44 = CWalkHitInfo::INTERSECTION_TRIANGLE;
        }
        return 0;
    }
    if (pHitInfo == NULL || bHit != 1)
        return 1;
    int nIndex = nHitFace * 3;
    int k = 0;
    while (k < 3)
    {
        GetFaceVertex(nIndex, k, avTriangle[k]);
        ++k;
    }
    if (bDumpData)
        EXOLOG("  hit: face %d, edge %d, vertex %d, reach end %f %f %f\n", nHitFace, nHitEdge, bVertexHit, vReachEnd.m_fX, vReachEnd.m_fY, vReachEnd.m_fZ);
    vFirst = avTriangle[nHitEdge];
    vSecond = avTriangle[(nHitEdge + 1) % 3];
    Vector vThird = avTriangle[(nHitEdge + 2) % 3];
    if (rendercollisionboxes == 1)
    {
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(vFirst),
                LocalToWorld(vSecond),
                LocalToWorld(vThird),
                LocalToWorld(vFirst)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, AUR_DRAWABLE_COLOR_WHITE);
        }
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(vFirst),
                LocalToWorld(vFirst),
                LocalToWorld(vSecond),
                LocalToWorld(vSecond)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFFF0F0F);
        }
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(vClosestHit),
                LocalToWorld(vClosestHit),
                LocalToWorld(vClosestHit + m_pFaceNormals[nHitFace] * 0.5f),
                LocalToWorld(vClosestHit + m_pFaceNormals[nHitFace] * 0.5f)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFF00FF00);
        }
        {
            static int REC_NUM = -1;
            Vector vCentroid = (vFirst + vSecond + vThird) * 0.33333f;
            Vector avSquare[4] =
            {
                LocalToWorld(vCentroid),
                LocalToWorld(vCentroid),
                LocalToWorld(vCentroid + m_pFaceNormals[nHitFace] * 0.5f),
                LocalToWorld(vCentroid + m_pFaceNormals[nHitFace] * 0.5f)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFF00FF00);
        }
    }
    if ((vFirst.m_fX == vSecond.m_fX && vFirst.m_fY == vSecond.m_fY &&
         vFirst.m_fZ != vSecond.m_fZ) || bVertexHit)
    {
        Vector vToHit = vClosestHit - vLocalStart;
        float fDistance = vToHit * vDirection + 0.001f;
        Vector vNormal = normalize(normalize(cross(zaxis, vFirst - vSecond)) +
                                   normalize(cross(zaxis, vThird - vFirst)));
        Vector vCorrection = vDirection * (fDistance - fRadius);
        Vector vFaceNormal = m_pFaceNormals[nHitFace];
        if ((zaxis * vFaceNormal) <= 0.0f && vNormal * vFaceNormal < 0.0f)
            vNormal = -vNormal;
        pHitInfo->SetHitInfo(CWalkHitInfo::FAILURE_BLOCKED, LocalToWorld(vLocalStart + vCorrection),
            NormalLocalToWorld(vNormal), LocalToWorld(vClosestHit), LocalToWorld(vClosestHit),
            CWalkHitInfo::INTERSECTION_CORNER, vFaceNormal);
        if (rendercollisionboxes == 1)
        {
            static int REC_NUM = -1;
            Vector avSquare[4] =
            {
                LocalToWorld(vClosestHit),
                LocalToWorld(vClosestHit),
                LocalToWorld(vClosestHit + vNormal),
                LocalToWorld(vClosestHit + vNormal)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF00FFF);
        }
        return 0;
    }
    else
    {
        Vector vRayStart;
        vRayStart.m_fX = vLocalStart.m_fX;
        vRayStart.m_fY = vLocalStart.m_fY;
        vRayStart.m_fZ = 0.0f;
        Vector vRayEnd(vLocalEnd.m_fX, vLocalEnd.m_fY, 0.0f);
        Vector vEdgeDirection = normalize(Vector(vFirst.m_fX, vFirst.m_fY, 0.0f) -
            Vector(vSecond.m_fX, vSecond.m_fY, 0.0f));
        Vector vNormal(-vEdgeDirection.m_fY, vEdgeDirection.m_fX, 0.0f);
        float fShear = (((vEdgeDirection * vLocalEnd)) - (vEdgeDirection * vLocalStart)) /
            ((vNormal * vLocalEnd) - (vNormal * vLocalStart)) * fRadius;
        float fRadiusSquared = fRadius * fRadius;
        float fAdjustment = sqrtf(fShear * fShear + fRadiusSquared) + 0.03f;
        Vector vEdgeStart(vFirst.m_fX, vFirst.m_fY, 0.0f);
        Vector vEdgeEnd(vSecond.m_fX, vSecond.m_fY, 0.0f);
        Vector vCross = cross(vRayEnd - vRayStart, vEdgeEnd - vEdgeStart);
        if (vCross.IsZero())
        {
            vClosestHit = vLocalStart;
            pHitInfo->SetHitInfo(CWalkHitInfo::FAILURE_BLOCKED, LocalToWorld(vLocalStart), NormalLocalToWorld(m_pFaceNormals[nHitFace]),
                LocalToWorld(vLocalStart), LocalToWorld(vLocalStart), CWalkHitInfo::INTERSECTION_CORNER, m_pFaceNormals[nHitFace]);
            if (rendercollisionboxes == 1)
            {
                static int REC_NUM = -1;
                Vector avSquare[4] =
                {
                    LocalToWorld(vClosestHit),
                    LocalToWorld(vClosestHit),
                    LocalToWorld(vClosestHit + vNormal),
                    LocalToWorld(vClosestHit + vNormal)
                };
                if (REC_NUM != -1)
                    AurRemoveDrawableSquare(REC_NUM);
                REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFFF00FFF);
            }
            return 0;
        }
        DistanceBetweenLines(vRayStart, Vector(vRayEnd.m_fX - vRayStart.m_fX, vRayEnd.m_fY - vRayStart.m_fY, vRayEnd.m_fZ - vRayStart.m_fZ),
            vEdgeStart, vEdgeEnd - vEdgeStart, &vClosestHit, NULL);
        if (rendercollisionboxes == 1)
        {
            static int REC_NUM = -1;
            Vector vNormalEnd = vClosestHit + vNormal;
            Vector avSquare[4] =
            {
                LocalToWorld(vClosestHit),
                LocalToWorld(vClosestHit),
                LocalToWorld(vNormalEnd),
                LocalToWorld(vNormalEnd)
            };
            if (REC_NUM != -1)
                AurRemoveDrawableSquare(REC_NUM);
            REC_NUM = AurAddDrawableSquare(&avSquare[0].m_fX, 0xFF55FF55);
        }
        Vector vAdjusted = vClosestHit + normalize(vLocalStart - vLocalEnd) * fAdjustment;
        vAdjusted.m_fZ = vLocalStart.m_fZ;
        Vector vTotalMove = vLocalEnd - vLocalStart;
        Vector vAdjustedDistance = vAdjusted - vLocalStart;
        float fMoveSquared = vTotalMove * vTotalMove;
        float fAdjustedSquared = vAdjustedDistance * vAdjustedDistance;
        if (fAdjustedSquared > fMoveSquared)
        {
            vEdgeDirection = -vEdgeDirection;
            Vector vPerpendicular(-vEdgeDirection.m_fY,
                                  vEdgeDirection.m_fX, 0.0f);
            fShear = (vEdgeDirection * vLocalEnd - vEdgeDirection * vLocalStart) /
                (vPerpendicular * vLocalEnd - vPerpendicular * vLocalStart) *
                fRadius + 0.03f;
            float fAdjustment = sqrt(fShear * fShear + fRadiusSquared);
            vAdjusted = vClosestHit + normalize(vLocalStart - vLocalEnd) * fAdjustment;
        }
        float fDeltaZ = vLocalStart.m_fZ - vLocalEnd.m_fZ;
        vAdjusted.m_fZ = fDeltaZ / fMoveSquared * fAdjustedSquared + vLocalStart.m_fZ;
        if (vFirst.m_fX == vSecond.m_fX && vFirst.m_fY == vSecond.m_fY)
        {
            pHitInfo->SetHitInfo(CWalkHitInfo::FAILURE_BLOCKED, LocalToWorld(vAdjusted), NormalLocalToWorld(m_pFaceNormals[nHitFace]),
                LocalToWorld(vFirst), LocalToWorld(vSecond), CWalkHitInfo::INTERSECTION_CORNER, m_pFaceNormals[nHitFace]);
        }
        else
        {
            pHitInfo->SetHitInfo(CWalkHitInfo::FAILURE_BLOCKED, LocalToWorld(vAdjusted), NormalLocalToWorld(vNormal),
                LocalToWorld(vFirst), LocalToWorld(vSecond), CWalkHitInfo::INTERSECTION_LINE, m_pFaceNormals[nHitFace]);
        }
        return 0;
    }
}
// PC 0x00557610
void CSWCollisionMesh::TransformToWorld(const Vector& vPosition)
{
    if (m_bBinary == 0)
    {
        m_vWorldPosition = vPosition;
        m_nType = 1;
        for (int i = 0; i < m_nVertexCount; i++)
        {
            m_pVertices[i] = m_pVertices[i] + m_vPosition + m_vWorldPosition;
        }
    }
}
// PC 0x0055AF60
int CSWCollisionMesh::RenderMesh(int nFlag, unsigned long nColor,
                                 unsigned long nMask)
{
    GetWalkableMaterialMask();
    int nFaceCount = m_nFaceCount;
    if (nFaceCount != 0)
    {
        int nFace = 0;
        if (nFaceCount > 0)
        {
            int nOffset = 0;
            do
            {
                if (nFace < nFaceCount)
                {
                    int nFirst = m_pFaces[nOffset];
                    int nSecond = m_pFaces[nOffset + 1];
                    int nThird = m_pFaces[nOffset + 2];
                    if (nFirst != -1 && nSecond != -1 && nThird != -1)
                    {
                        Vector vFirst;
                        Vector vSecond;
                        Vector vThird;
                        if (m_pVertices != NULL && nFirst < m_nVertexCount)
                        {
                            vFirst = LocalToWorld(m_pVertices[nFirst]);
                        }
                        if (m_pVertices != NULL && nSecond < m_nVertexCount)
                        {
                            vSecond = LocalToWorld(m_pVertices[nSecond]);
                        }
                        if (m_pVertices != NULL && nThird < m_nVertexCount)
                        {
                            vThird = LocalToWorld(m_pVertices[nThird]);
                        }
                    }
                }
                nFaceCount = m_nFaceCount;
                nFace++;
                nOffset += 3;
            } while (nFace < nFaceCount);
        }
    }
    return nFaceCount;
}
// PC 0x00558130
int CSWCollisionMesh::SaveBinaryMesh(const CExoString& sFileName)
{
    if (m_bBinary != 0)
    {
        return -1;
    }
    FILE* pFile = fopen(sFileName.CStr(), "r");
    if (pFile != NULL)
    {
        CExoString sError;
        sError.Format("ERROR: opening a Binary walkmesh file for writeing that "
                      "already exists (File: %s)",
                      sFileName.CStr());
        fclose(pFile);
        return -1;
    }
    pFile = fopen(sFileName.CStr(), "wb+");
    if (pFile == NULL)
    {
        return 0;
    }
    _CSWWalkMeshHeader header;
    memset(&header, 0, sizeof(header));
    strcpy(header.m_sVersion, "BWM V1.0");
    header.m_nType = m_nType;
    header.m_avRelativeUsePoints[0] = m_avUsePoints[0];
    header.m_avRelativeUsePoints[1] = m_avUsePoints[1];
    header.m_vPosition = m_vPosition;
    header.m_nVertexCount = m_nVertexCount;
    header.m_nFaceCount = m_nFaceCount;
    header.m_nAdjacencyCount = m_nAdjacencyCount;
    fwrite(&header, sizeof(header), 1, pFile);
    header.m_nVertexOffset = ftell(pFile);
    fwrite(m_pVertices, 12, m_nVertexCount, pFile);
    header.m_nFaceOffset = ftell(pFile);
    fwrite(m_pFaces, 4, 3 * m_nFaceCount, pFile);
    header.m_nMaterialOffset = ftell(pFile);
    fwrite(m_pFaceMaterials, 4, m_nFaceCount, pFile);
    header.m_nNormalOffset = ftell(pFile);
    fwrite(m_pFaceNormals, 12, m_nFaceCount, pFile);
    header.m_nDistanceOffset = ftell(pFile);
    fwrite(m_pFaceDistances, 4, m_nFaceCount, pFile);
    fseek(pFile, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, pFile);
    fclose(pFile);
    return 1;
}

#ifdef _XBOX
#include "nwsarea.h"
#include "swsroom.h"
#include "nwdoorsurfacemesh.h"
#include "aurconsole.h"

int ConvertRoomWalkMeshes(const CResRef& cLayout, const CExoString& sDirectory)
{
    CExoString sLayout;
    CExoString sPath;
    char szLayout[17];
    cLayout.CopyToString(szLayout);
    sPath = sDirectory;
    if (sPath.Right(1) != "\\")
    {
        sPath = sPath + "\\";
    }
    CLYT* pLayout = new CLYT;
    if (pLayout->LoadLayout(cLayout))
    {
        int nRooms = pLayout->GetRoomCount();
        CSWSRoom* pRooms = CSWSArea::LoadRooms(pLayout);
        for (int nRoom = 0; nRoom < nRooms; nRoom++)
        {
            CExoString sFile;
            CExoString sRoom;
            pLayout->GetRoomName(nRoom).CopyToString(sRoom);
            sFile.Format("%s%s.WOK", sPath.CStr(), sRoom.CStr());
            if (!pRooms[nRoom].m_pWalkMesh->SaveBinaryMesh(sFile))
            {
                delete[] pRooms;
                pLayout->UnloadLayout();
                delete pLayout;
                return 0;
            }
        }
        delete[] pRooms;
        pLayout->UnloadLayout();
        delete pLayout;
        return 1;
    }
    pLayout->UnloadLayout();
    delete pLayout;
    return 0;
}

static char* ConvertRooms(char* szArgs)
{
    CExoString sLayout;
    CExoString sArgs(szArgs);
    int nSpace = sArgs.Find(' ', 0);
    sLayout = sArgs.Left(nSpace);
    sArgs = sArgs.Right(sArgs.GetLength() - nSpace - 1);
    if (ConvertRoomWalkMeshes(CResRef(sLayout), sArgs) == 1)
    {
        return "SUSSESS";
    }
    return "FAILURE";
}

int ConvertDoorWalkMeshes(const CResRef& cDoor, const CExoString& sDirectory)
{
    CExoString sFile;
    CExoString sDoor;
    CExoString sPath;
    cDoor.CopyToString(sDoor);
    sPath = sDirectory;
    if (sPath.Right(1) != "\\")
    {
        sPath = sPath + "\\";
    }
    int bResult = 1;
    for (int nState = CSWDoorSurfaceMesh::DOOR_STATE_CLOSED; nState <= CSWDoorSurfaceMesh::DOOR_STATE_OPEN2; nState++)
    {
        CSWDoorSurfaceMesh* pMesh = new CSWDoorSurfaceMesh;
        pMesh->SetDoorType(nState);
        pMesh->m_cResRef = cDoor;
        int nLoaded = pMesh->LoadMesh(1);
        if (nLoaded <= 0)
        {
            bResult = nLoaded;
        }
        pMesh->ComputeMeshNormals();
        sFile.Format("%s%s%d.DWK", sPath.CStr(), sDoor.CStr(), nState);
        if (!pMesh->SaveBinaryMesh(sFile))
        {
            delete pMesh;
            return 0;
        }
        delete pMesh;
    }
    return bResult;
}

static char* ConvertDoor(char* szArgs)
{
    CExoString sDoor;
    CExoString sArgs(szArgs);
    int nSpace = sArgs.Find(' ', 0);
    sDoor = sArgs.Left(nSpace);
    sArgs = sArgs.Right(sArgs.GetLength() - nSpace - 1);
    if (ConvertDoorWalkMeshes(CResRef(sDoor), sArgs) == 1)
    {
        return "SUSSESS";
    }
    return "FAILURE";
}

int ConvertPlaceableWalkMesh(const CResRef& cPlaceable, const CExoString& sDirectory)
{
    CExoString sFile;
    CExoString sPlaceable;
    CExoString sPath;
    cPlaceable.CopyToString(sPlaceable);
    sPath = sDirectory;
    if (sPath.Right(1) != "\\")
    {
        sPath = sPath + "\\";
    }
    CSWPlaceableSurfaceMesh* pMesh = new CSWPlaceableSurfaceMesh;
    pMesh->m_cResRef = cPlaceable;
    if (pMesh->LoadMesh(1) <= 0)
    {
        return -1;
    }
    sFile.Format("%s%s.PWK", sPath.CStr(), sPlaceable.CStr());
    if (!pMesh->SaveBinaryMesh(sFile))
    {
        delete pMesh;
        return 0;
    }
    delete pMesh;
    return 1;
}

static char* ConvertPlaceable(char* szArgs)
{
    CExoString sPlaceable;
    CExoString sArgs(szArgs);
    int nSpace = sArgs.Find(' ', 0);
    sPlaceable = sArgs.Left(nSpace);
    sArgs = sArgs.Right(sArgs.GetLength() - nSpace - 1);
    if (ConvertPlaceableWalkMesh(CResRef(sPlaceable), sArgs) == 1)
    {
        return "SUSSESS";
    }
    return "FAILURE";
}

ConsoleFunc ConVrendercollisionboxes("rendercollisionboxes", &rendercollisionboxes);
ConsoleFunc ConVWalkTestMaterial("WalkTestMaterial", &WalkTestMaterial);
ConsoleFunc ConFConvertDoor("ConvertDoor", ConvertDoor);
ConsoleFunc ConFConvertPlaceable("ConvertPlaceable", ConvertPlaceable);
ConsoleFunc ConFConvertRooms("ConvertRooms", ConvertRooms);
#endif
