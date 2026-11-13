#include "swsroom.h"

inline void CSWSRoom::DestroyWalkMesh()
{
    if (m_pWalkMesh != NULL)
    {
        delete m_pWalkMesh;
        m_pWalkMesh = NULL;
    }
}
// PC 0x0055B030
CSWSRoom::CSWSRoom()
{
    m_pWalkMesh = new CSWRoomSurfaceMesh;
}
// PC 0x0055B0A0
int CSWSRoom::LoadWalkMesh(int nType)
{
    if (m_bNoWalkMesh)
    {
        return 0;
    }
    m_pWalkMesh->m_cResRef = m_cResRef;
    return m_pWalkMesh->LoadMesh(nType);
}
// PC 0x0055B0E0
int CSWSRoom::CheckAABBWalkable(const Vector& vMin, const Vector& vMax,
                                CSWRoomSurfaceMeshHitInfo& hitInfo)
{
    int bResult = 0;
    CSWRoomSurfaceMesh* pMesh = m_pWalkMesh;
    if (pMesh->CheckAABBWalkable(vMin, vMax, hitInfo))
    {
        bResult = 1;
        hitInfo.m_vHitPosition =
            hitInfo.m_vHitPosition + m_vPosition + m_pWalkMesh->m_vWorldPosition;
    }
    return bResult;
}
// PC 0x0055B1A0
int CSWSRoom::CheckAABBAll(Vector vMin, Vector vMax,
                           CSWRoomSurfaceMeshHitInfo& hitInfo)
{
    int bResult = 0;
    if (m_pWalkMesh->CheckAABBAll(vMin, vMax, hitInfo))
    {
        bResult = 1;
    }
    return bResult;
}
// PC 0x0055B200
int CSWSRoom::ClippedLineSegmentWalkable(const Vector& vStart,
                                         const Vector& vEnd,
                                         Vector& vClippedStart,
                                         Vector& vClippedEnd, float& fStartT,
                                         float& fEndT, int& nFace)
{
    return m_pWalkMesh->ClippedLineSegmentWalkable(
        vStart, vEnd, vClippedStart, vClippedEnd, fStartT, fEndT, nFace);
}
// PC 0x0055B210
int CSWSRoom::NoNonWalkPolysOnRoom(const Vector& vStart, const Vector& vEnd,
                                   const float& fA, const float& fB,
                                   const float& fC, CWalkHitInfo* pHitInfo)
{
    CSWCollisionMesh* pMesh = m_pWalkMesh;
    return pMesh->NoNonWalkPolys(vStart, vEnd, fA, fB, fC, pHitInfo);
}
// PC 0x0055B240
void CSWSRoom::TransformToWorld()
{
    if (!m_bNoWalkMesh)
    {
        m_pWalkMesh->TransformToWorld(m_vPosition);
    }
}
// PC 0x0055B260
int CSWSRoom::GetAdjacentRoom(int nEdge)
{
    return m_pWalkMesh->GetAdjacentRoom(nEdge);
}
// PC 0x0055B270
int CSWSRoom::WeldRoom(CSWSRoom& other, float fTolerance)
{
    if (m_bNoWalkMesh || other.m_bNoWalkMesh)
    {
        return 0;
    }
    Vector vOtherPosition = other.m_vPosition;
    return m_pWalkMesh->WeldEdgeVertices(other.m_pWalkMesh, fTolerance,
                                         &m_vPosition, &vOtherPosition);
}
// PC 0x0055B2D0
void CSWSRoom::ComputeMeshNormals()
{
    if (!m_bNoWalkMesh)
    {
        m_pWalkMesh->ComputeMeshNormals();
    }
}
// PC 0x0055B2E0
int CSWSRoom::SetRoomAdjacency(CSWSRoom& other, int nEdge, int nOtherEdge)
{
    if (m_bNoWalkMesh || other.m_bNoWalkMesh)
    {
        return 0;
    }
    Vector vOtherPosition = other.m_vPosition;
    return m_pWalkMesh->SetRoomAdjacency(other.m_pWalkMesh, nEdge, nOtherEdge,
                                         m_vPosition, vOtherPosition);
}
// PC 0x0055B340
int CSWSRoom::ClearLineOfSight(Vector vStart, Vector vEnd, Vector* pvHit,
                               unsigned long* pnFace)
{
    if (m_bNoWalkMesh)
    {
        return 1;
    }
    CSWRoomSurfaceMeshHitInfo hitInfo(0xFFFFFFFF);
    hitInfo.ClearAll(0xFFFFFFFF);
    int bHit = m_pWalkMesh->CheckAABBLineOfSight(vStart, vEnd, hitInfo);
    if (bHit != 0 && pvHit != NULL)
    {
        *pvHit = hitInfo.m_vHitPosition;
    }
    return bHit == 0;
}
// PC 0x0055B3E0
float CSWSRoom::ComputeHeight(Vector vPosition, int nFace)
{
    float fHeight = 0.0f;
    if (m_pWalkMesh != NULL)
    {
        fHeight = m_pWalkMesh->ComputeHeight(vPosition, nFace);
    }
    return fHeight;
}
// PC 0x0055B420
float CSWSRoom::ComputeHeight(int nFace, Vector vPosition)
{
    float fHeight = 0.0f;
    if (m_pWalkMesh != NULL)
    {
        fHeight = m_pWalkMesh->ComputeHeight(nFace, vPosition);
    }
    return fHeight;
}
// PC 0x0055B460
Vector CSWSRoom::GetNormalOfPoint(int nFace)
{
    if (nFace >= 0)
    {
        Vector* pNormals = m_pWalkMesh->m_pFaceNormals;
        if (pNormals != NULL)
        {
            return pNormals[nFace];
        }
        return Vector(0.0f, 0.0f, 1.0f);
    }
    return Vector(0.0f, 0.0f, 1.0f);
}
// PC 0x0055B4B0
CSWSRoom::~CSWSRoom()
{
    if (m_pWalkMesh != NULL)
    {
        delete m_pWalkMesh;
    }
    m_pWalkMesh = NULL;
}
