#include "swroomsurfacemesh.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

template void CExoArrayList<CSWRoomSurfaceMeshAABBNode>::Allocate(int);
template void CExoArrayList<CSWRoomSurfaceMeshAABBNode>::Add(CSWRoomSurfaceMeshAABBNode);

#include "exobase.h"

#include "ResBWM.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "swmain.h"

static inline void RoomSurfaceMeshTrace1();
#define SWROOMSURFACEMESH_TRACE() RoomSurfaceMeshTrace1()

int g_bRenderClipWalkable = 0;

float checkaabbtime;
// PC 0x00580920  Xbox 0x001E1760
int CSWRoomSurfaceMesh::CheckAABBNode(int nNode, const Vector& vStart, Vector& vEnd,
                                     CSWRoomSurfaceMeshHitInfo& hitInfo)
{
    const Vector& vMinimum = m_lstAABBNodes[nNode].m_vMin;
    const Vector& vMaximum = m_lstAABBNodes[nNode].m_vMax;
#ifdef _XBOX
    extern int DumpRoomBBox; if (DumpRoomBBox) { g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory); g_pExoBase->m_pExoDebug->FlushScriptLogFile(); }
#endif
    if (!BoxIntersect(vStart, vEnd, vMinimum, vMaximum))
    {
        return 0;
    }

    if (m_lstAABBNodes[nNode].m_nTriangle != -1)
    {
        int nTriangle = m_lstAABBNodes[nNode].m_nTriangle;
        int nMaterialMask = 1 << m_pFaceMaterials[nTriangle];
        int nHits = 0;
        if (!(hitInfo.m_nField00 & nMaterialMask))
        {
            return 0;
        }

        Vector avVertices[3];
        Vector vNormal;
        for (int i = 0; i < 3; ++i)
        {
            avVertices[i] = m_pVertices[m_pFaces[nTriangle * 3 + i]];
        }

        if (polyhit(avVertices, 3, vStart, vEnd, &vEnd, &vNormal))
        {
            hitInfo.m_nField0C = nTriangle;
            nHits = 1;
            hitInfo.m_nField1C = nMaterialMask;
        }
        return nHits;
    }
    else
    {
        if (m_lstAABBNodes[nNode].m_nTraversalMask & hitInfo.m_nField04)
        {
            return CheckAABBNode(m_lstAABBNodes[nNode].m_nLeftChild, vStart, vEnd, hitInfo) + CheckAABBNode(m_lstAABBNodes[nNode].m_nRightChild, vStart, vEnd, hitInfo);
        }
        else
        {
            return CheckAABBNode(m_lstAABBNodes[nNode].m_nRightChild, vStart, vEnd, hitInfo) + CheckAABBNode(m_lstAABBNodes[nNode].m_nLeftChild, vStart, vEnd, hitInfo);
        }
    }
}
// PC 0x00580AF0  Xbox 0x001E1990
void CSWRoomSurfaceMesh::TransformAABBToWorld(int nNode, const Vector& vOffset)
{
    while (nNode != -1)
    {
        m_lstAABBNodes[nNode].m_vMax =
            m_lstAABBNodes[nNode].m_vMax + m_vPosition + vOffset;
        m_lstAABBNodes[nNode].m_vMin =
            m_lstAABBNodes[nNode].m_vMin + m_vPosition + vOffset;
        TransformAABBToWorld(m_lstAABBNodes[nNode].m_nLeftChild,
                             vOffset);
        nNode = m_lstAABBNodes[nNode].m_nRightChild;
    }
}
// PC 0x00580340  Xbox 0x001E1090
static __int64 QueryTimer()
{
    LARGE_INTEGER nFrequency;
    LARGE_INTEGER nCount;
    static __int64 performanceFrequency =
        (QueryPerformanceFrequency(&nFrequency), nFrequency.QuadPart);

    QueryPerformanceCounter(&nCount);
    return 1000000 * nCount.QuadPart / performanceFrequency;
}
// PC 0x005803C0  Xbox 0x001E1110
void CSWRoomSurfaceMesh::ComputeTriangleAdjacency()
{
    if (m_nAdjacencyCount != 0)
    {
        m_pTriangleAdjacency = new int[m_nAdjacencyCount * 3];
        int* pVertexIndices = new int[m_nAdjacencyCount * 7];

        for (int nTriangle = 0; nTriangle < m_nAdjacencyCount; ++nTriangle)
        {
            m_pTriangleAdjacency[nTriangle * 3] =
                m_pTriangleAdjacency[nTriangle * 3 + 1] =
                m_pTriangleAdjacency[nTriangle * 3 + 2] = -1;
            pVertexIndices[nTriangle * 7] =
                pVertexIndices[nTriangle * 7 + 3] =
                pVertexIndices[nTriangle * 7 + 6] = m_pFaces[nTriangle * 3];
            pVertexIndices[nTriangle * 7 + 1] =
                pVertexIndices[nTriangle * 7 + 5] = m_pFaces[nTriangle * 3 + 1];
            pVertexIndices[nTriangle * 7 + 2] =
                pVertexIndices[nTriangle * 7 + 4] = m_pFaces[nTriangle * 3 + 2];
        }

        int bFoundEdge = 0;
        for (int nFirst = 0; nFirst < m_nAdjacencyCount; ++nFirst)
        {
            for (int nFirstEdge = 1; nFirstEdge < 7; ++nFirstEdge)
            {
                for (int nSecond = nFirst + 1; nSecond < m_nAdjacencyCount; ++nSecond)
                {
                    for (int nSecondEdge = 1; nSecondEdge < 7; ++nSecondEdge)
                    {
                        if (pVertexIndices[nFirst * 7 + nFirstEdge - 1] ==
                                pVertexIndices[nSecond * 7 + nSecondEdge - 1] &&
                            pVertexIndices[nFirst * 7 + nFirstEdge] ==
                                pVertexIndices[nSecond * 7 + nSecondEdge])
                        {
                            if (bFoundEdge == 0)
                            {
                                bFoundEdge = 1;
                            }
                            else
                            {
                                int nFirstIndex = nFirst * 3 +
                                    (nFirstEdge < 4 ? nFirstEdge % 4 - 1 : 6 - nFirstEdge);
                                int nSecondIndex = nSecond * 3 +
                                    (nSecondEdge < 4 ? nSecondEdge % 4 - 1 : 6 - nSecondEdge);
                                m_pTriangleAdjacency[nFirstIndex] = nSecondIndex;
                                m_pTriangleAdjacency[nSecondIndex] = nFirstIndex;
                                bFoundEdge = 0;
                            }
                        }
                    }
                }
            }
        }
        delete[] pVertexIndices;
    }
}
// PC 0x005805E0  Xbox 0x001E20A0
float CSWRoomSurfaceMesh::ComputeHeight(int nFace, Vector vPosition)
{
    Vector vPoint(vPosition.m_fX, vPosition.m_fY, -9.0f);

    return PlaneHeightAtPoint(m_pFaceNormals[nFace], m_pFaceDistances[nFace],
                              vPoint);
}
// PC 0x00580620  Xbox 0x001E1420
void CSWRoomSurfaceMesh::GetTriangleAdjacency(int nUnused, int nTriangle,
                                              int* pnFirst, int* pnSecond,
                                              int* pnThird)
{
    if (nTriangle < m_nFaceCount && m_pTriangleAdjacency != NULL)
    {
        int nIndex = nTriangle * 3;
        int nFirst = m_pTriangleAdjacency[nIndex];
        nFirst = (nFirst == -1) ? -1 : nFirst / 3;
        *pnFirst = nFirst;
        int nSecond = m_pTriangleAdjacency[nIndex + 1];
        nSecond = (nSecond == -1) ? -1 : nSecond / 3;
        *pnSecond = nSecond;
        int nThird = m_pTriangleAdjacency[nIndex + 2];
        nThird = (nThird == -1) ? -1 : nThird / 3;
        *pnThird = nThird;
    }
}
// PC 0x005806C0  Xbox 0x001E14C0
void CSWRoomSurfaceMesh::GetTriangleVertices(int nUnused, int nTriangle,
                                             int* pnFirst, int* pnSecond,
                                             int* pnThird)
{
    if (nTriangle < m_nFaceCount)
    {
        int nVertexOffset = nTriangle * 3;

        *pnFirst = m_pFaces[nVertexOffset];
        *pnSecond = m_pFaces[nVertexOffset + 1];
        *pnThird = m_pFaces[nVertexOffset + 2];
    }
}
// PC 0x00580700  Xbox 0x001E1550
void CSWRoomSurfaceMesh::SetOrientation(const Quaternion& qOrientation)
{
    m_qOrientation = qOrientation;
}

inline CSWRoomSurfaceExternalEdge::CSWRoomSurfaceExternalEdge()
{
    m_nEdge = -1;
    m_nAdjacentRoom = -1;
    SWROOMSURFACEMESH_TRACE();
}

inline void AddExternalEdge(
    CExoStaticArrayList<CSWRoomSurfaceExternalEdge>& lstEdges,
    CSWRoomSurfaceExternalEdge edge)
{
    if (lstEdges.m_bStatic == 0)
    {
        lstEdges.CExoArrayList<CSWRoomSurfaceExternalEdge>::Add(edge);
    }
}
// PC 0x00580720  Xbox 0x001E1570
inline CSWRoomSurfaceMeshAABBNode::CSWRoomSurfaceMeshAABBNode()
{
    m_nLeftChild = -1;
    m_nRightChild = -1;
    m_nTriangle = -1;
    m_nNodeType = 4;
    m_nTraversalMask = 0;
}
// PC 0x00580750  Xbox 0x001E15B0
int CSWRoomSurfaceMeshHitInfo::ClearOutput()
{
    m_nField08 = 0;
    m_nField1C = 0;
    m_vHitPosition = Vector(0.0f, 0.0f, 0.0f);
    return 0;
}
// PC 0x00580780  Xbox 0x001E15F0
void CSWRoomSurfaceMeshHitInfo::ClearAll(unsigned long nMask)
{
    m_nField0C = -1;
    m_nField00 = nMask;
    m_nField04 = 0;
    m_nField08 = 0;
    m_nField1C = 0;
    m_vHitPosition = Vector(0.0f, 0.0f, 0.0f);
}
// PC 0x005807C0  Xbox 0x001E1640
int CSWRoomSurfaceMesh::LoadMeshBinary(CResBWM* pMesh)
{
    int nResult = CSWCollisionMesh::LoadMeshBinary(pMesh);

    if (nResult == 0)
    {
        return nResult;
    }

    m_pTriangleAdjacency = pMesh->m_pBWMHeader
                               ? (int*)(pMesh->m_pBWMData +
                                        pMesh->m_pBWMHeader->m_nAdjacencyOffset)
                               : NULL;
    m_nAABBRootNode =
        pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_nAABBRootNode : 0;

    int nEdgeCount =
        pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_nEdgeCount : 0;
    CSWRoomSurfaceExternalEdge* pEdges =
        pMesh->m_pBWMHeader
            ? (CSWRoomSurfaceExternalEdge*)(pMesh->m_pBWMData +
                                            pMesh->m_pBWMHeader->m_nEdgeOffset)
            : NULL;
    if (m_lstExternalEdges.num == 0)
    {
        m_lstExternalEdges.element = pEdges;
        m_lstExternalEdges.num = nEdgeCount;
        m_lstExternalEdges.array_size = nEdgeCount;
        m_lstExternalEdges.m_bStatic = 1;
    }

    int nAABBCount =
        pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_nAABBNodeCount : 0;
    CSWRoomSurfaceMeshAABBNode* pAABBNodes =
        pMesh->m_pBWMHeader
            ? (CSWRoomSurfaceMeshAABBNode*)(pMesh->m_pBWMData +
                                            pMesh->m_pBWMHeader
                                                ->m_nAABBNodeOffset)
            : NULL;
    if (m_lstAABBNodes.num == 0)
    {
        m_lstAABBNodes.element = pAABBNodes;
        m_lstAABBNodes.num = nAABBCount;
        m_lstAABBNodes.array_size = nAABBCount;
        m_lstAABBNodes.m_bStatic = 1;
    }

    int nPerimeterCount =
        pMesh->m_pBWMHeader ? pMesh->m_pBWMHeader->m_nPerimeterCount : 0;
    int* pPerimeters = pMesh->m_pBWMHeader
                           ? (int*)(pMesh->m_pBWMData +
                                    pMesh->m_pBWMHeader->m_nPerimeterOffset)
                           : NULL;
    if (m_lstPerimeters.num == 0)
    {
        m_lstPerimeters.element = pPerimeters;
        m_lstPerimeters.num = nPerimeterCount;
        m_lstPerimeters.array_size = nPerimeterCount;
        m_lstPerimeters.m_bStatic = 1;
    }

    return 1;
}
// PC 0x00580BE0  Xbox 0x001E1AB0
int CSWRoomSurfaceMesh::GetEdgeVertex(int nEdge, Vector& vVertex)
{
    if (nEdge < m_lstExternalEdges.num)
    {
        vVertex =
            m_pVertices[m_pFaces[m_lstExternalEdges[nEdge].m_nEdge]];
        return 1;
    }

    vVertex = Vector(0.0f, 0.0f, 0.0f);
    return 0;
}
// PC 0x00580C60  Xbox 0x001E1B30
int CSWRoomSurfaceMesh::GetEdgeVertices(int nEdge, Vector& vFirst,
                                        Vector& vSecond)
{
    int nEdgeCount = m_lstExternalEdges.num;

    if (nEdge < nEdgeCount)
    {
        vFirst =
            m_pVertices[m_pFaces[m_lstExternalEdges[nEdge].m_nEdge]];

        vSecond = m_pVertices
            [m_pFaces[m_lstExternalEdges[nEdge + 1 == nEdgeCount ? 0 : nEdge + 1]
                          .m_nEdge]];
        return 1;
    }

    vFirst = Vector(0.0f, 0.0f, 0.0f);
    vSecond = Vector(0.0f, 0.0f, 0.0f);
    return 0;
}

int CSWRoomSurfaceMesh::SetEdgeVertex(int nEdge, Vector& vVertex)
{
    if (nEdge < m_lstExternalEdges.num)
    {
        m_pVertices[m_pFaces[m_lstExternalEdges[nEdge].m_nEdge]] = vVertex;
        return 1;
    }
    return 0;
}
// PC 0x00580D50  Xbox 0x001E1C20
int CSWRoomSurfaceMesh::WeldEdgeVertices(CSWRoomSurfaceMesh* pOther,
                                         float fTolerance, Vector* pvPosition,
                                         Vector* pvOtherPosition)
{
    if (m_bBinary != 0)
    {
        return 0;
    }

    int nWelded = 0;
    Vector vVertex;
    Vector vOtherVertex;

    for (int i = 0; i < m_lstExternalEdges.num; i++)
    {
        GetEdgeVertex(i, vVertex);
        for (int j = 0; j < pOther->m_lstExternalEdges.num; j++)
        {
            pOther->GetEdgeVertex(j, vOtherVertex);

            float fDeltaX = vVertex.m_fX - vOtherVertex.m_fX;
            float fDeltaY = vVertex.m_fY - vOtherVertex.m_fY;
            float fDeltaZ = vVertex.m_fZ - vOtherVertex.m_fZ;

            if (fDeltaZ * fDeltaZ + fDeltaY * fDeltaY + fDeltaX * fDeltaX <
                fTolerance)
            {
                pOther->SetEdgeVertex(j, vVertex);
                nWelded++;
            }
        }
    }

    return nWelded;
}
// PC 0x00580E90  Xbox 0x001E1DB0
int CSWRoomSurfaceMesh::SetRoomAdjacency(CSWRoomSurfaceMesh* pOther,
                                          int nRoom, int nOtherRoom,
                                          const Vector& vPosition,
                                          const Vector& vOtherPosition)
{
    int nPerimeter = 0;
    if (m_bBinary != 0)
    {
        return 0;
    }

    int nAdjacent = 0;
    Vector vFirst;
    Vector vSecond;
    Vector vOtherFirst;
    Vector vOtherSecond;
    Vector vPerimeterStart;

    for (int i = 0; i < m_lstExternalEdges.num; i++)
    {
        GetEdgeVertices(i, vFirst, vSecond);
        if (i == 0)
        {
            vPerimeterStart = vFirst;
        }
        if (i == m_lstPerimeters[nPerimeter] - 1)
        {
            nPerimeter++;
            Vector vTemp = vSecond;
            vSecond = vPerimeterStart;
            vPerimeterStart = vTemp;
        }

        Vector vOtherPerimeterStart;
        int nOtherPerimeter = 0;
        for (int j = 0; j < pOther->m_lstExternalEdges.num; j++)
        {
            pOther->GetEdgeVertices(j, vOtherFirst, vOtherSecond);
            if (j == 0)
            {
                vOtherPerimeterStart = vOtherFirst;
            }
            if (j == pOther->m_lstPerimeters[nOtherPerimeter] - 1)
            {
                nOtherPerimeter++;
                Vector vTemp = vOtherSecond;
                vOtherSecond = vOtherPerimeterStart;
                vOtherPerimeterStart = vTemp;
            }

            float fDeltaX = vFirst.m_fX - vOtherSecond.m_fX;
            float fDeltaY = vFirst.m_fY - vOtherSecond.m_fY;
            float fDeltaZ = vFirst.m_fZ - vOtherSecond.m_fZ;
            float fOtherDeltaX = vSecond.m_fX - vOtherFirst.m_fX;
            float fOtherDeltaY = vSecond.m_fY - vOtherFirst.m_fY;
            float fOtherDeltaZ = vSecond.m_fZ - vOtherFirst.m_fZ;

            if (fDeltaZ * fDeltaZ + fDeltaY * fDeltaY + fDeltaX * fDeltaX <
                    SWROOMSURFACEMESH_ADJACENCY_TOLERANCE_SQUARED &&
                fOtherDeltaZ * fOtherDeltaZ + fOtherDeltaY * fOtherDeltaY +
                        fOtherDeltaX * fOtherDeltaX <
                    SWROOMSURFACEMESH_ADJACENCY_TOLERANCE_SQUARED)
            {
                m_lstExternalEdges[i].m_nAdjacentRoom = nOtherRoom;
                pOther->m_lstExternalEdges[j].m_nAdjacentRoom = nRoom;
                nAdjacent++;
            }
        }
    }

    return nAdjacent;
}
// PC 0x00581150
int CSWRoomSurfaceMesh::GetAdjacentRoom(int nEdge)
{
    return m_lstExternalEdges[nEdge].m_nAdjacentRoom;
}
// PC 0x00582AE0  Xbox 0x001E3B50
CSWRoomSurfaceMesh::CSWRoomSurfaceMesh()
    : CSWCollisionMesh(RESTYPE_MDL, RESTYPE_WOK)
{
    m_pTriangleAdjacency = NULL;
    m_lstAABBNodes = CExoStaticArrayList<CSWRoomSurfaceMeshAABBNode>();
    m_nLOSMaterialMask = 0;
    m_nWalkableMaterialMask = 0;
    m_nWalkCheckMaterialMask = 0;
    m_nAllMaterialsMask = 0;

    for (int nMaterial = 0; nMaterial < 32; ++nMaterial)
    {
        CExoString sLabel;
        int nValue;
        {
            CExoString sColumn("Label");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pSurfaceMaterial = p2DArrays->m_pSurfaceMaterial;
            pSurfaceMaterial->GetCExoStringEntry(
                nMaterial, sColumn, &sLabel);
        }
        {
            CExoString sColumn("LineOfSight");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pSurfaceMaterial = p2DArrays->m_pSurfaceMaterial;
            pSurfaceMaterial->GetINTEntry(
                nMaterial, sColumn, &nValue);
        }
        if (nValue == 1)
        {
            m_nLOSMaterialMask += 1 << nMaterial;
        }
        {
            CExoString sColumn("Walk");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pSurfaceMaterial = p2DArrays->m_pSurfaceMaterial;
            pSurfaceMaterial->GetINTEntry(
                nMaterial, sColumn, &nValue);
        }
        if (nValue == 1)
        {
            m_nWalkableMaterialMask += 1 << nMaterial;
        }
        {
            CExoString sColumn("WalkCheck");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pSurfaceMaterial = p2DArrays->m_pSurfaceMaterial;
            pSurfaceMaterial->GetINTEntry(
                nMaterial, sColumn, &nValue);
        }
        if (nValue == 1)
        {
            m_nWalkCheckMaterialMask += 1 << nMaterial;
        }
        m_nAllMaterialsMask += 1 << nMaterial;
    }
}

#ifndef _XBOX
template <> CExoArrayList<CSWRoomSurfaceMeshAABBNode>::~CExoArrayList()
{
    delete[] element;
    element = NULL;
}
#endif
// PC 0x00582D70  Xbox 0x001E3DE0
int CSWRoomSurfaceMesh::LoadMeshText(unsigned char* pData,
                                     unsigned long nDataSize)
{
    if (m_pFaces != NULL)
        return 1;
    char* pLine = new char[SWROOMSURFACEMESH_LINE_LENGTH];
    if (pLine == NULL)
        return 0;
    unsigned char* pCurrentData = pData;
    unsigned long nRemaining = nDataSize;
    int bInAABB = 0;
    int bFoundAABB = 0;
    float fPositionX = 0.0f;
    float fPositionY = 0.0f;
    float fPositionZ = 0.0f;
    int* pnOriginalFaces = NULL;
    int* pnWalkableMap = NULL;
    int* pnNonWalkableMap = NULL;
    while (nRemaining > 0)
    {
        if (LoadMeshString(&pCurrentData, &nRemaining,
                           (unsigned char*)pLine,
                           SWROOMSURFACEMESH_LINE_LENGTH) != 1)
            break;
        char* pText = pLine;
        while (*pText == ' ')
            pText++;
        if (strncmp(pText, "node", 4) == 0)
        {
            pText += 4;
            bInAABB = 0;
            if (strncmp(pText, " aabb", 5) == 0)
            {
                bInAABB = 1;
                bFoundAABB = 1;
            }
        }
        if (strncmp(pText, "endnode", 7) == 0)
        {
            bInAABB = 0;
            continue;
        }
        if (bInAABB != 1)
            continue;
        if (strncmp(pText, "position", 8) == 0)
        {
            pText += 8;
            sscanf(pText, "%f %f %f", &fPositionX,
                   &fPositionY, &fPositionZ);
            m_vPosition = Vector(fPositionX, fPositionY, fPositionZ);
        }
        if (strncmp(pText, "orientation", 11) == 0)
        {
            Vector vAxis;
            float fAngle;
            pText += 11;
            sscanf(pText, "%f %f %f %f", &vAxis.m_fX,
                   &vAxis.m_fY, &vAxis.m_fZ, &fAngle);
            if (vAxis.m_fX == 0.0f && vAxis.m_fY == 0.0f &&
                vAxis.m_fZ == 0.0f)
                SetOrientation(Quaternion());
            else
                m_qOrientation = Quaternion(vAxis, fAngle);
        }
        if (strncmp(pText, "verts", 5) == 0)
        {
            pText += 5;
            sscanf(pText, "%d", &m_nVertexCount);
            SetVertexCount(m_nVertexCount);
            for (int nVertex = 0; nVertex < m_nVertexCount; nVertex++)
            {
                if (LoadMeshString(&pCurrentData, &nRemaining,
                                   (unsigned char*)pLine,
                                   SWROOMSURFACEMESH_LINE_LENGTH) == 0)
                {
                    LoadDefaultMesh();
                    delete[] pLine;
                    return 1;
                }
                sscanf(pLine, "%f %f %f", &m_pVertices[nVertex].m_fX,
                       &m_pVertices[nVertex].m_fY, &m_pVertices[nVertex].m_fZ);
                int nGridX =
                    (int)(m_pVertices[nVertex].m_fX * SWROOMSURFACEMESH_VERTEX_GRID + 0.5f);
                if ((int)nGridX % SWROOMSURFACEMESH_VERTEX_GRID == 0)
                    m_pVertices[nVertex].m_fX = (float)((int)nGridX >> 9);
                int nGridY =
                    (int)(m_pVertices[nVertex].m_fY * SWROOMSURFACEMESH_VERTEX_GRID + 0.5f);
                if ((int)nGridY % SWROOMSURFACEMESH_VERTEX_GRID == 0)
                    m_pVertices[nVertex].m_fY = (float)((int)nGridY >> 9);
            }
        }
        if (strncmp(pText, "faces", 5) == 0)
        {
            pText += 5;
            sscanf(pText, "%d", &m_nFaceCount);
            SetTriangleCount(m_nFaceCount);
            pnOriginalFaces = new int[3 * m_nFaceCount];
            int* pnMaterials = new int[m_nFaceCount];
            int nFace;
            for (nFace = 0; nFace < m_nFaceCount; nFace++)
            {
                if (LoadMeshString(&pCurrentData, &nRemaining,
                                   (unsigned char*)pLine,
                                   SWROOMSURFACEMESH_LINE_LENGTH) == 0)
                {
                    LoadDefaultMesh();
                    delete[] pLine;
                    return 1;
                }
                int nSmoothingGroup, nTexture0, nTexture1, nTexture2;
                sscanf(pLine, "%d %d %d %d %d %d %d %d",
                       &pnOriginalFaces[3 * nFace],
                       &pnOriginalFaces[3 * nFace + 1],
                       &pnOriginalFaces[3 * nFace + 2],
                       &nSmoothingGroup, &nTexture0, &nTexture1,
                       &nTexture2, &pnMaterials[nFace]);
            }
            int nWalkable = 0;
            int nNonWalkable = 0;
            int* pnWalkableFaces = new int[m_nFaceCount];
            int* pnNonWalkableFaces = new int[m_nFaceCount];
            pnWalkableMap = new int[m_nFaceCount];
            pnNonWalkableMap = new int[m_nFaceCount];
            for (nFace = 0; nFace < m_nFaceCount; nFace++)
            {
                int bWalkable;
                {
                    CExoString sColumn("Walk");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    C2DA* pSurfaceMaterial = p2DArrays->m_pSurfaceMaterial;
                    pSurfaceMaterial->GetINTEntry(
                        pnMaterials[nFace], sColumn, &bWalkable);
                }
                if (bWalkable != 0)
                {
                    pnWalkableMap[nFace] = nWalkable;
                    pnNonWalkableMap[nFace] = -1;
                    pnWalkableFaces[nWalkable++] = nFace;
                }
                else
                {
                    pnWalkableMap[nFace] = -1;
                    pnNonWalkableMap[nFace] = nNonWalkable;
                    pnNonWalkableFaces[nNonWalkable++] = nFace;
                }
            }
            for (nFace = 0; nFace < nWalkable; nFace++)
            {
                m_pFaces[3 * nFace] = pnOriginalFaces[3 * pnWalkableFaces[nFace]];
                m_pFaces[3 * nFace + 1] = pnOriginalFaces[3 * pnWalkableFaces[nFace] + 1];
                m_pFaces[3 * nFace + 2] = pnOriginalFaces[3 * pnWalkableFaces[nFace] + 2];
                m_pFaceMaterials[nFace] = pnMaterials[pnWalkableFaces[nFace]];
            }
            for (nFace = 0; nFace < nNonWalkable; nFace++)
            {
                m_pFaces[3 * (nWalkable + nFace)] = pnOriginalFaces[3 * pnNonWalkableFaces[nFace]];
                m_pFaces[3 * (nWalkable + nFace) + 1] = pnOriginalFaces[3 * pnNonWalkableFaces[nFace] + 1];
                m_pFaces[3 * (nWalkable + nFace) + 2] = pnOriginalFaces[3 * pnNonWalkableFaces[nFace] + 2];
                m_pFaceMaterials[nWalkable + nFace] = pnMaterials[pnNonWalkableFaces[nFace]];
            }
            m_nAdjacencyCount = nWalkable;
            delete[] pnNonWalkableFaces;
            delete[] pnWalkableFaces;
            delete[] pnMaterials;
        }
        if (strncmp(pText, "aabb", 4) == 0)
        {
            pText += 4;
            CExoArrayList<CSWRoomSurfaceMeshAABBNode*> lstParents;
            CExoArrayList<int> lstParentIndices;
            CExoStaticArrayList<CSWRoomSurfaceMeshAABBNode*> lstNodes;
            int nCurrent;
            do
            {
                Vector vMin, vMax;
                int nTriangle;
#ifdef _XBOX
                int nFields = sscanf(pText, "%f %f %f %f %f %f %d",
                       &vMin.m_fX, &vMin.m_fY, &vMin.m_fZ,
                       &vMax.m_fX, &vMax.m_fY, &vMax.m_fZ, &nTriangle);
#else
                sscanf(pText, "%f %f %f %f %f %f %d",
                       &vMin.m_fX, &vMin.m_fY, &vMin.m_fZ,
                       &vMax.m_fX, &vMax.m_fY, &vMax.m_fZ, &nTriangle);
#endif
                if (nTriangle != -1)
                    nTriangle = pnWalkableMap[nTriangle] != -1 ?
                        pnWalkableMap[nTriangle] :
                        m_nAdjacencyCount + pnNonWalkableMap[nTriangle];
                EXOXBOXASSERT(nFields == 7);
                float fTemp;
                if (vMin.m_fX > vMax.m_fX)
                {
                    fTemp = vMin.m_fX; vMin.m_fX = vMax.m_fX; vMax.m_fX = fTemp;
                }
                if (vMin.m_fY > vMax.m_fY)
                {
                    fTemp = vMin.m_fY; vMin.m_fY = vMax.m_fY; vMax.m_fY = fTemp;
                }
                if (vMin.m_fZ > vMax.m_fZ)
                {
                    fTemp = vMin.m_fZ; vMin.m_fZ = vMax.m_fZ; vMax.m_fZ = fTemp;
                }
                CSWRoomSurfaceMeshAABBNode* pNode = new CSWRoomSurfaceMeshAABBNode;
                lstNodes.Add(pNode);
                pNode->m_nTriangle = nTriangle;
                pNode->m_vMin = vMin - Vector(SWROOMSURFACEMESH_AABB_MARGIN,
                                             SWROOMSURFACEMESH_AABB_MARGIN,
                                             SWROOMSURFACEMESH_AABB_MARGIN);
                pNode->m_vMax = vMax + Vector(SWROOMSURFACEMESH_AABB_MARGIN,
                                             SWROOMSURFACEMESH_AABB_MARGIN,
                                             SWROOMSURFACEMESH_AABB_MARGIN);
                nCurrent = lstNodes.num - 1;
                if (pNode->m_nTriangle == -1)
                {
                    lstParents.Add(pNode);
                    lstParentIndices.Add(nCurrent);
                    if (LoadMeshString(&pCurrentData, &nRemaining,
                                       (unsigned char*)pLine,
                                       SWROOMSURFACEMESH_LINE_LENGTH) == 0)
                    {
                        LoadDefaultMesh();
                        delete[] pLine;
                        delete[] pnWalkableMap;
                        delete[] pnNonWalkableMap;
                        delete[] pnOriginalFaces;
                        return 1;
                    }
                    pText = pLine;
                }
                else
                {
                    int bClimbing;
                    do
                    {
                        bClimbing = 0;
                        if (lstParents.num > 0)
                        {
                            CSWRoomSurfaceMeshAABBNode* pParent = lstParents[lstParents.num - 1];
                            int nParent = lstParentIndices[lstParentIndices.num - 1];
                            lstParents[lstParents.num - 1] = NULL;
                            lstParents.DelIndex(lstParents.num - 1);
                            lstParentIndices.DelIndex(lstParentIndices.num - 1);
                            if (pParent->m_nLeftChild == -1)
                            {
                                pParent->m_nLeftChild = nCurrent;
                                lstParents.Add(pParent);
                                lstParentIndices.Add(nParent);
                                if (LoadMeshString(&pCurrentData, &nRemaining,
                                                   (unsigned char*)pLine,
                                                   SWROOMSURFACEMESH_LINE_LENGTH) == 0)
                                {
                                    LoadDefaultMesh();
                                    delete[] pLine;
                                    delete[] pnWalkableMap;
                                    delete[] pnNonWalkableMap;
                                    delete[] pnOriginalFaces;
                                    return 1;
                                }
                                pText = pLine;
                            }
                            else
                            {
                                bClimbing = 1;
                                pParent->m_nRightChild = nCurrent;
                                Vector vDifference =
                                    (lstNodes[pParent->m_nRightChild]->m_vMin +
                                     lstNodes[pParent->m_nRightChild]->m_vMax) -
                                    (lstNodes[pParent->m_nLeftChild]->m_vMin +
                                     lstNodes[pParent->m_nLeftChild]->m_vMax);
                                if (fabs(vDifference.m_fX) > fabs(vDifference.m_fY) &&
                                    fabs(vDifference.m_fX) > fabs(vDifference.m_fZ))
                                    pParent->m_nTraversalMask = vDifference.m_fX > 0.0f ?
                                        CSWRoomSurfaceMeshAABBNode::TRAVERSAL_POSITIVE_X :
                                        CSWRoomSurfaceMeshAABBNode::TRAVERSAL_NEGATIVE_X;
                                else if (fabs(vDifference.m_fY) > fabs(vDifference.m_fZ))
                                    pParent->m_nTraversalMask = vDifference.m_fY > 0.0f ?
                                        CSWRoomSurfaceMeshAABBNode::TRAVERSAL_POSITIVE_Y :
                                        CSWRoomSurfaceMeshAABBNode::TRAVERSAL_NEGATIVE_Y;
                                else
                                    pParent->m_nTraversalMask = vDifference.m_fZ > 0.0f ?
                                        CSWRoomSurfaceMeshAABBNode::TRAVERSAL_POSITIVE_Z :
                                        CSWRoomSurfaceMeshAABBNode::TRAVERSAL_NEGATIVE_Z;
                                nCurrent = nParent;
                            }
                        }
                    } while (bClimbing == 1);
                }
            } while (lstParents.num > 0);
            m_nAABBRootNode = nCurrent;
            for (int nNode = 0; nNode < lstNodes.num; nNode++)
            {
                m_lstAABBNodes.Add(*lstNodes[nNode]);
                delete lstNodes[nNode];
                lstNodes[nNode] = NULL;
            }
        }
    }
    delete[] pLine;
    delete[] pnWalkableMap;
    delete[] pnNonWalkableMap;
    delete[] pnOriginalFaces;
    if (bFoundAABB == 0)
        LoadDefaultMesh();
    else
    {
        ComputeTriangleAdjacency();
        ComputeRoomBorder();
    }
    return 1;
}
// PC 0x00581320  Xbox 0x001E22A0
CSWRoomSurfaceMeshHitInfo::CSWRoomSurfaceMeshHitInfo(unsigned long nMask)
{
    RoomSurfaceMeshTrace1();
    ClearAll(nMask);
}
// PC 0x00581370  Xbox 0x001E2300
int CSWRoomSurfaceMesh::SaveBinaryMesh(const CExoString& sFileName)
{
    SWROOMSURFACEMESH_TRACE();
    int nResult = CSWCollisionMesh::SaveBinaryMesh(sFileName);

    if (nResult == 1)
    {
        FILE* pFile = fopen(sFileName.CStr(), "rb+");
        _CSWWalkMeshHeader header;

        fread(&header, sizeof(header), 1, pFile);
        fseek(pFile, 0, SEEK_END);
        header.m_nAABBNodeCount = m_lstAABBNodes.num;
        header.m_nAABBNodeOffset = ftell(pFile);
        header.m_nAABBRootNode = m_nAABBRootNode;
        fwrite(m_lstAABBNodes.element, 44, header.m_nAABBNodeCount, pFile);
        header.m_nAdjacencyOffset = ftell(pFile);
        fwrite(m_pTriangleAdjacency, 4, 3 * header.m_nAdjacencyCount, pFile);
        header.m_nEdgeCount = m_lstExternalEdges.num;
        header.m_nEdgeOffset = ftell(pFile);
        fwrite(m_lstExternalEdges.element, 8, header.m_nEdgeCount, pFile);
        header.m_nPerimeterCount = m_lstPerimeters.num;
        header.m_nPerimeterOffset = ftell(pFile);
        fwrite(m_lstPerimeters.element, 4, header.m_nPerimeterCount, pFile);
        fseek(pFile, 0, SEEK_SET);
        fwrite(&header, sizeof(header), 1, pFile);
        fclose(pFile);
        return 1;
    }

    return nResult;
}
// PC 0x005814D0  Xbox 0x001E24B0
int CSWRoomSurfaceMesh::CheckAABBLineOfSight(Vector vStart, Vector vEnd,
                                             CSWRoomSurfaceMeshHitInfo& hitInfo)
{
    if (m_lstAABBNodes.num == 0)
    {
        return 0;
    }

    hitInfo.m_nField00 = m_nLOSMaterialMask;
    if (CheckAABBNode(m_nAABBRootNode, vStart, vEnd, hitInfo) > 0)
    {
        hitInfo.m_nField08 = 1;
        hitInfo.m_vHitPosition = vEnd;
    }

    return hitInfo.m_nField08;
}
// PC 0x00581530  Xbox 0x001E2510
int CSWRoomSurfaceMesh::CheckAABBWalkable(Vector vStart, Vector vEnd,
                                          CSWRoomSurfaceMeshHitInfo& hitInfo)
{
    if (m_lstAABBNodes.num == 0)
    {
        return 0;
    }

    hitInfo.m_nField00 = m_nWalkCheckMaterialMask;

    int nHits = CheckAABBNode(m_nAABBRootNode, vStart, vEnd, hitInfo);

    if (hitInfo.m_nField0C == -1)
    {
        Vector vOffset = normalize(Vector(1.0f, 1.0f, 0.0f));

        vOffset *= 0.001f;
        vStart += vOffset;
        vEnd += vOffset;
        nHits = CheckAABBNode(m_nAABBRootNode, vStart, vEnd, hitInfo);
    }

    if (nHits > 0)
    {
        hitInfo.m_nField08 = 1;
        hitInfo.m_vHitPosition = vEnd;
    }

    return hitInfo.m_nField08;
}
// PC 0x00581610  Xbox 0x001E2600
int CSWRoomSurfaceMesh::CheckAABBAll(Vector vStart, Vector vEnd,
                                     CSWRoomSurfaceMeshHitInfo& hitInfo)
{
    if (m_lstAABBNodes.num == 0)
    {
        return 0;
    }

    __int64 nStartTime = QueryTimer();

    hitInfo.m_nField00 = m_nAllMaterialsMask;

    int nHits = CheckAABBNode(m_nAABBRootNode, vStart, vEnd, hitInfo);

    if (hitInfo.m_nField0C == -1)
    {
        Vector vOffset = normalize(Vector(1.0f, 1.0f, 0.0f));

        vOffset *= 0.001f;
        vStart += vOffset;
        vEnd += vOffset;
        nHits = CheckAABBNode(m_nAABBRootNode, vStart, vEnd, hitInfo);
    }

    if (nHits > 0)
    {
        hitInfo.m_nField08 = 1;
        hitInfo.m_vHitPosition = vEnd;
    }

    checkaabbtime = (float)(QueryTimer() - nStartTime);
    return hitInfo.m_nField08;
}
// PC 0x00581720  Xbox 0x001E2710
int CSWRoomSurfaceMesh::FindTriangle(const Vector& vPosition, int bWalkable,
                                     Vector vDirection)
{
    Vector vLocal = WorldToLocal(vPosition);
    Vector vTop(vLocal.m_fX, vLocal.m_fY, 1000.0f);
    Vector vBottom(vLocal.m_fX, vLocal.m_fY, -1000.0f);
    CSWRoomSurfaceMeshHitInfo hitInfo(0xFFFFFFFF);

    if (bWalkable != 0)
    {
        CheckAABBWalkable(vTop, vBottom, hitInfo);
    }
    else
    {
        CheckAABBAll(vTop, vBottom, hitInfo);
    }

    if (hitInfo.m_nField0C == -1)
    {
        Vector vOffset = normalize(vDirection);

        vOffset *= 0.001f;
        vTop += vOffset;
        vBottom += vOffset;
        if (bWalkable != 0)
        {
            CheckAABBWalkable(vTop, vBottom, hitInfo);
        }
        else
        {
            CheckAABBAll(vTop, vBottom, hitInfo);
        }
    }

    return hitInfo.m_nField0C;
}
// PC 0x00581930  Xbox 0x001E2930
int CSWRoomSurfaceMesh::GetSurfaceMaterial(Vector vPosition)
{
    Vector vLocal = WorldToLocal(vPosition);
    CSWRoomSurfaceMeshHitInfo hitInfo(0xFFFFFFFF);
    Vector vStart(vLocal.m_fX, vLocal.m_fY, 1000.0f);
    Vector vEnd(vLocal.m_fX, vLocal.m_fY, -1000.0f);

    CheckAABBAll(vStart, vEnd, hitInfo);
    if (hitInfo.m_nField0C == -1)
    {
        return 0;
    }

    return m_pFaceMaterials[hitInfo.m_nField0C];
}
// PC 0x00581A10  Xbox 0x001E2A30
int CSWRoomSurfaceMesh::GetSurfaceMaterialWalkCheckOnly(Vector vPosition)
{
    Vector vLocal = WorldToLocal(vPosition);
    CSWRoomSurfaceMeshHitInfo hitInfo(0xFFFFFFFF);
    Vector vStart(vLocal.m_fX, vLocal.m_fY, 1000.0f);
    Vector vEnd(vLocal.m_fX, vLocal.m_fY, -1000.0f);

    CheckAABBWalkable(vStart, vEnd, hitInfo);
    if (hitInfo.m_nField0C == -1)
    {
        return 0;
    }

    return m_pFaceMaterials[hitInfo.m_nField0C];
}
// PC 0x00581AF0  Xbox 0x001E2B30
void CSWRoomSurfaceMesh::TransformToWorld(const Vector& vPosition)
{
    if (m_bBinary == 0)
    {
        CSWCollisionMesh::TransformToWorld(vPosition);
        if (m_lstAABBNodes.num != 0)
        {
            TransformAABBToWorld(m_nAABBRootNode, vPosition);
        }
    }
}
// PC 0x00581B30  Xbox 0x001E2B70
float CSWRoomSurfaceMesh::ComputeHeight(Vector vPosition, int bWalkable)
{
    int nFace = FindTriangle(vPosition, bWalkable, Vector(1.0f, 1.0f, 0.0f));

    if (nFace == -1)
    {
        return -9.0f;
    }

    Vector vPoint(vPosition.m_fX, vPosition.m_fY, -9.0f);

    return PlaneHeightAtPoint(m_pFaceNormals[nFace], m_pFaceDistances[nFace],
                              vPoint);
}
// PC 0x00581FE0  Xbox 0x001E3070
CSWRoomSurfaceMesh::~CSWRoomSurfaceMesh()
{
    if (m_bBinary != 0)
    {
        m_pTriangleAdjacency = NULL;
        m_lstAABBNodes.element = NULL;
        m_lstAABBNodes.num = 0;
        m_lstAABBNodes.array_size = 0;
        m_lstAABBNodes.m_bStatic = 0;
        m_lstExternalEdges.element = NULL;
        m_lstExternalEdges.num = 0;
        m_lstExternalEdges.array_size = 0;
        m_lstExternalEdges.m_bStatic = 0;
        m_lstPerimeters.m_bStatic = 0;
        m_lstPerimeters.element = NULL;
        m_lstPerimeters.array_size = 0;
        m_lstPerimeters.num = 0;
    }
    else
    {
        int*& pAdjacency = m_pTriangleAdjacency;
        delete[] pAdjacency;
        pAdjacency = NULL;
        if (m_lstExternalEdges.m_bStatic == 0)
        {
            m_lstExternalEdges.SetSize(0);
        }
        if (m_lstPerimeters.m_bStatic == 0)
        {
            m_lstPerimeters.SetSize(0);
        }
    }
}
// PC 0x005821E0  Xbox 0x001E3250
void CSWRoomSurfaceMesh::ComputeRoomBorder()
{
    if (m_nAdjacencyCount == 0)
        return;

    CExoArrayList<CSWRoomSurfaceExternalEdge*> lstEdges;
    CExoArrayList<CSWRoomSurfaceExternalEdge*> lstOrderedEdges;
    int nEdge;
    for (nEdge = 0; nEdge < m_nAdjacencyCount * 3; nEdge++)
    {
        if (m_pTriangleAdjacency[nEdge] == -1)
        {
            lstEdges.Add(new CSWRoomSurfaceExternalEdge(nEdge));
        }
    }

    int* pnEdges = new int[lstEdges.num];
    for (int i = 0; i < lstEdges.num; i++)
    {
        pnEdges[i] = lstEdges[i]->m_nEdge;
    }

    int nProcessed = 0;
    while (nProcessed < lstEdges.num)
    {
        int nIndex = 0;
        while (pnEdges[nIndex] == -1)
            nIndex++;

        lstOrderedEdges.Add(lstEdges[nIndex]);
        nProcessed++;
        pnEdges[nIndex] = -1;
        nEdge = lstEdges[nIndex]->m_nEdge;
        int nStartVertex = m_pFaces[nEdge];
        int nNextVertex;
        int bComplete = 0;
        while (!bComplete)
        {
            nNextVertex = m_pFaces[nEdge % 3 != 2 ? nEdge + 1 : nEdge - 2];
            for (nIndex = 0; nIndex < lstEdges.num; nIndex++)
            {
                if (pnEdges[nIndex] != -1 &&
                    nNextVertex == m_pFaces[lstEdges[nIndex]->m_nEdge])
                {
                    lstOrderedEdges.Add(lstEdges[nIndex]);
                    nEdge = lstEdges[nIndex]->m_nEdge;
                    pnEdges[nIndex] = -1;
                    nProcessed++;
                    if (nStartVertex == m_pFaces[nEdge % 3 != 2 ? nEdge + 1 : nEdge - 2])
                    {
                        bComplete = 1;
                        m_lstPerimeters.Add(nProcessed);
                    }
                    break;
                }
            }
        }
    }

    for (int j = 0; j < lstOrderedEdges.num; j++)
    {
        AddExternalEdge(m_lstExternalEdges, *lstOrderedEdges[j]);
        delete lstOrderedEdges[j];
    }
    delete[] pnEdges;
}
// PC 0x00583E30  Xbox 0x001E4EB0
int CSWRoomSurfaceMesh::ComputeClippedLineSegment(
    Vector vStart, Vector vEnd, Vector& vClippedStart, Vector& vClippedEnd,
    int& nFace)
{
    Vector vStart2D(vStart.m_fX, vStart.m_fY, 0.0f);
    Vector vEnd2D(vEnd.m_fX, vEnd.m_fY, 0.0f);
    Vector vCrossings[2];

    if (equal(vStart2D, vEnd2D))
    {
        vEnd2D.m_fY += 1.0f;
        if (IsPointInPolygon(vStart2D, vEnd2D, nFace) % 2)
        {
            vClippedStart = vStart;
            vClippedEnd = vEnd;
            return 1;
        }
        vClippedStart = vStart;
        vClippedEnd = vEnd;
        return 0;
    }

    int nCrossings = IsPointInPolygon(vStart2D, vEnd2D, nFace);
    float fLengthSquared = (vEnd2D - vStart2D) * (vEnd2D - vStart2D);

    if (nCrossings == 0)
    {
        vClippedStart = vStart;
        vClippedEnd = vEnd;
        return 0;
    }
    if (nCrossings % 2)
    {
        vCrossings[0] = Vector(m_lstCrossings[0].m_fX,
                               m_lstCrossings[0].m_fY, 0.0f);
        if (fLengthSquared <
            (vCrossings[0] - vStart2D) * (vCrossings[0] - vStart2D))
        {
            vClippedStart = vStart;
            vClippedEnd = vEnd;
            return 1;
        }
        vClippedStart = vStart;
        vClippedEnd = m_lstCrossings[0];
        return 1;
    }

    vCrossings[0] = Vector(m_lstCrossings[0].m_fX,
                           m_lstCrossings[0].m_fY, 0.0f);
    vCrossings[1] = Vector(m_lstCrossings[1].m_fX,
                           m_lstCrossings[1].m_fY, 0.0f);
    if (fLengthSquared <
        (vCrossings[0] - vStart2D) * (vCrossings[0] - vStart2D))
    {
        vClippedStart = vStart;
        vClippedEnd = vEnd;
        return 0;
    }
    if (fLengthSquared <
        (vCrossings[1] - vStart2D) * (vCrossings[1] - vStart2D))
    {
        vClippedStart = m_lstCrossings[0];
        vClippedEnd = vEnd;
        return 1;
    }
    vClippedStart = m_lstCrossings[0];
    vClippedEnd = m_lstCrossings[1];
    return 1;
}
// PC 0x00582630  Xbox 0x001E36A0
int CSWRoomSurfaceMesh::IsPointInPolygon(const Vector& vStart,
                                         const Vector& vEnd, int& nFace)
{
    m_lstCrossings.num = 0;
    m_lstCrossingFaces.num = 0;
    int nCrossings = 0;
    EXOXBOXASSERT(vEnd.m_fX != vStart.m_fX || vEnd.m_fY != vStart.m_fY);
    Vector vDirection = normalize(Vector(vEnd.m_fX - vStart.m_fX,
                                         vEnd.m_fY - vStart.m_fY, 0.0f));
    Vector vNormal(-vDirection.m_fY, vDirection.m_fX, 0.0f);
    float fStartDistance = vDirection * vStart;
    float fStartSide = vNormal * vStart;
    float fPreviousSide = fStartSide;
    int bFoundFace = 0;
    int nFirst = 0;

    for (int nPerimeter = 0; nPerimeter < m_lstPerimeters.num; nPerimeter++)
    {
        int nLast = m_lstPerimeters[nPerimeter];
        for (int nEdge = nFirst; nEdge < nLast; nEdge++)
        {
            const Vector& vPrevious =
                m_pVertices[m_pFaces[m_lstExternalEdges[
                    (nEdge - 1 < nFirst) ? nLast - 1 : nEdge - 1].m_nEdge]];
            const Vector& vCurrent =
                m_pVertices[m_pFaces[m_lstExternalEdges[nEdge].m_nEdge]];
            int nNext = (nEdge + 1 == nLast) ? nFirst : nEdge + 1;
            const Vector& vNext =
                m_pVertices[m_pFaces[m_lstExternalEdges[nNext].m_nEdge]];
            float fDistance = vDirection * vCurrent;
            float fCurrentSide = vNormal * vCurrent;
            float fNextSide = vNormal * vNext;

            if ((fCurrentSide < fStartSide && fStartSide < fNextSide) ||
                (fNextSide < fStartSide && fStartSide < fCurrentSide))
            {
                float fFraction =
                    (fStartSide - fNextSide) / (fCurrentSide - fNextSide);
                Vector vCrossing =
                    fFraction * vCurrent + (1.0f - fFraction) * vNext;
                if (vCrossing * vDirection >= fStartDistance)
                {
                    AddCrossing(vStart, vDirection, vCrossing,
                                nCrossings, nEdge);
                    if (!bFoundFace)
                    {
                        nFace = nEdge;
                        bFoundFace = 1;
                    }
                }
            }

            if (fCurrentSide == fStartSide && fDistance >= fStartDistance)
            {
                float fPreviousVertexSide = vNormal * vPrevious;
                if (fPreviousVertexSide != fCurrentSide ||
                    fCurrentSide != fNextSide)
                {
                    if (fPreviousSide == fStartSide)
                    {
                        if (fPreviousVertexSide != fStartSide)
                            fPreviousSide = fPreviousVertexSide;
                    }
                    else
                    {
                        int bSameSide =
                            (fPreviousSide < fStartSide && fNextSide < fStartSide) ||
                            (fPreviousSide > fStartSide && fNextSide > fStartSide);
                        fPreviousSide = fStartSide;
                        if (!bSameSide)
                            continue;
                    }
                    AddCrossing(vStart, vDirection, vCurrent,
                                nCrossings, nEdge);
                    if (!bFoundFace)
                    {
                        nFace = nEdge;
                        bFoundFace = 1;
                    }
                    if (fDistance != fStartDistance &&
                        ((fPreviousVertexSide < fStartSide && fNextSide < fStartSide) ||
                         (fPreviousVertexSide > fStartSide && fNextSide > fStartSide)))
                    {
                        AddCrossing(vStart, vDirection, vCurrent,
                                    nCrossings, nEdge);
                        if (!bFoundFace)
                        {
                            nFace = nEdge;
                            bFoundFace = 1;
                        }
                    }
                }
            }
        }
        nFirst = nLast;
    }
    if (nCrossings > 1)
        nFace = m_lstCrossingFaces[0];
    return nCrossings;
}
// PC 0x00584220  Xbox 0x001E5260
int CSWRoomSurfaceMesh::ClippedLineSegmentWalkable(
    const Vector& vStart, const Vector& vEnd, Vector& vClippedStart,
    Vector& vClippedEnd, float& fMinHeight, float& fMaxHeight, int& nFace)
{
    if (!ComputeClippedLineSegment(vStart, vEnd, vClippedStart,
                                   vClippedEnd, nFace))
    {
        return 0;
    }

    int nTriangles = 0;
    Vector avVertices[3];
    Vector vPosition = vStart;
    Vector vEndPosition = vEnd;
    Vector vDirection = vEnd - vStart;
    Vector vHit;
    Vector vNormal;
    int anVertices[3];
    int anAdjacent[3];
    float afX[3], afY[3], afZ[3];
    int abInside[3];
    int nTriangle = FindTriangle(vPosition, 1, vDirection);
    if (nTriangle == -1)
    {
        return 0;
    }
    if (nTriangle >= 0)
    {
        GetTriangleVertices(0, nTriangle, &anVertices[0], &anVertices[1],
                            &anVertices[2]);
        for (int i = 0; i < 3; ++i)
        {
            GetVertex(anVertices[i], &avVertices[i]);
        }
        {
            Vector vTop = vPosition;
            Vector vBottom = vPosition;
            vTop.m_fZ = SWROOMSURFACEMESH_HEIGHT_PROBE;
            vBottom.m_fZ = -SWROOMSURFACEMESH_HEIGHT_PROBE;
            if (polyhit(avVertices, 3, vTop, vBottom, &vHit, &vNormal))
            {
                vPosition.m_fZ = vHit.m_fZ;
                vClippedStart.m_fZ = vHit.m_fZ;
            }
        }
        while (nTriangle >= 0 &&
               nTriangles < SWROOMSURFACEMESH_MAX_WALK_TRIANGLES)
        {
            ++nTriangles;
            GetTriangleVertices(0, nTriangle, &anVertices[0], &anVertices[1],
                                &anVertices[2]);
            GetTriangleAdjacency(0, nTriangle, &anAdjacent[0], &anAdjacent[1],
                                 &anAdjacent[2]);
            for (int i = 0; i < 3; ++i)
            {
                GetVertex(anVertices[i], &afX[i], &afY[i], &afZ[i]);
                GetVertex(anVertices[i], &avVertices[i]);
            }
            if (nTriangles == 1)
            {
                fMaxHeight = vPosition.m_fZ;
                fMinHeight = vPosition.m_fZ;
            }
            else if (vPosition.m_fZ >= fMaxHeight)
            {
                fMaxHeight = vPosition.m_fZ;
            }
            else if (vPosition.m_fZ <= fMinHeight)
            {
                fMinHeight = vPosition.m_fZ;
            }
            int nInside = 0;
            for (int i = 0; i < 3; ++i)
            {
                int nNext = (i + 1) % 3;
                if ((afY[i] - afY[nNext]) * (vEnd.m_fX - afX[i]) +
                    (afX[nNext] - afX[i]) * (vEnd.m_fY - afY[i]) >= 0.0)
                {
                    abInside[i] = 1;
                    ++nInside;
                }
                else
                {
                    abInside[i] = 0;
                }
            }
            if (nInside == 0)
            {
                return 0;
            }
            if (nInside == 3)
            {
                Vector vTop = vEndPosition;
                Vector vBottom = vEndPosition;
                vTop.m_fZ = SWROOMSURFACEMESH_HEIGHT_PROBE;
                vBottom.m_fZ = -SWROOMSURFACEMESH_HEIGHT_PROBE;
                if (polyhit(avVertices, 3, vTop, vBottom, &vHit, &vNormal))
                {
                    vEndPosition.m_fZ = vHit.m_fZ;
                    vClippedEnd.m_fZ = vHit.m_fZ;
                }
                if (vEndPosition.m_fZ >= fMaxHeight)
                    fMaxHeight = vEndPosition.m_fZ;
                else if (vEndPosition.m_fZ <= fMinHeight)
                    fMinHeight = vEndPosition.m_fZ;
                return 1;
            }
            if (nInside == 2)
            {
                Vector vCrossing;
                for (int i = 2; i >= 0; --i)
                {
                    int nNext = (i + 1) % 3;
                    if (!abInside[i])
                    {
                        if (!IntersectLineSegments2d(vPosition, vEndPosition,
                            avVertices[i], avVertices[nNext], vCrossing))
                            return 0;
                        nTriangle = anAdjacent[i];
                        vPosition = vCrossing;
                    }
                }
            }
            else if (nInside == 1)
            {
                int bFound = 0;
                Vector vCrossing;
                for (int i = 2; i >= 0 && !bFound; --i)
                {
                    int nNext = (i + 1) % 3;
                    if (!abInside[i] &&
                        IntersectLineSegments2d(vPosition, vEndPosition,
                            avVertices[i], avVertices[nNext], vCrossing) == 1)
                    {
                        bFound = 1;
                        nTriangle = anAdjacent[i];
                        vPosition = vCrossing;
                    }
                }
                if (!bFound)
                    return 0;
            }
        }
        if (nTriangles >= SWROOMSURFACEMESH_MAX_WALK_TRIANGLES)
        {
            CExoString sResRef; m_cResRef.CopyToString(sResRef);
            EXOLOG("ERROR!\n This tile is crapped out.  %s\n ERROR!\n", sResRef.CStr()); EXOLOG("ERROR!\n This tile is crapped out.  %s\n ERROR!\n", sResRef.CStr());
#ifdef _XBOX
            g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory); g_pExoBase->m_pExoDebug->FlushScriptLogFile();
#endif
            return 0;
        }
    }
    if (nTriangle == -1)
    {
        if (vPosition.m_fZ >= fMaxHeight)
            fMaxHeight = vPosition.m_fZ;
        else if (vPosition.m_fZ <= fMinHeight)
            fMinHeight = vPosition.m_fZ;
        return 1;
    }
    return 0;
}
// PC 0x00581D90  Xbox 0x001E2DE0
void CSWRoomSurfaceMesh::AddCrossing(const Vector& vStart,
                                    const Vector& vDirection,
                                    const Vector& vCrossing,
                                    int& nCrossings, int nFace)
{
    int nIndex = 0;
    int nCount = m_lstCrossings.num;
    for (; nIndex < nCount; nIndex++)
    {
        if ((((vDirection * vCrossing))) < vDirection * m_lstCrossings[nIndex])
            break;
    }
    m_lstCrossings.Insert(vCrossing, nIndex);
    m_lstCrossingFaces.Insert(nFace, nIndex);
    nCrossings++;
}

static inline void RoomSurfaceMeshTrace1()
{
}

#ifdef _XBOX
#include "aurconsole.h"

ConsoleFunc ConVcheckaabbtime("checkaabbtime", &checkaabbtime);
#endif
