#include <math.h>
#include <string.h>
#include "exobase.h"
#include "nwarea.h"

const int CSWArea::MAX_INTER_TILE_SUCCESSORS = 8;
const float CSWArea::TILE_SIZE = 10.0f;
const float CSWArea::MAX_TILE_EPSILON = 9.9999f;
const float CSWArea::MIN_TILE_EPSILON = 0.0001f;
const float CSWArea::EPSILON = 0.0001f;
const float CSWArea::SPLINE_COSINE_LIMIT = 0.9f;
// PC 0x005A1540 GOG 0x005A15B0
CSWArea::CSWArea()
{
    Vector vColor;
    m_vFogColor = vColor;
    m_nFlags = 0;
    m_nHeight = 0;
    m_nWidth = 0;
    m_nMoonAmbientColor = 0;
    m_nMoonDiffuseColor = 0;
    m_nMoonFogColor = 0;
    m_fMoonFogNear = 0;
    m_fMoonFogFar = 0;
    m_bMoonShadows = 0;
    m_nSunAmbientColor = 0;
    m_nSunDiffuseColor = 0;
    m_nSunFogColor = 0;
    m_fSunFogNear = 0;
    m_fSunFogFar = 0;
    m_bSunShadows = 0;
    m_bIsNight = 0;
    m_nChanceRain = 0;
    m_nChanceSnow = 0;
    m_nChanceLightning = 0;
    m_nWind = 0;
    m_nDynAmbientColor = 0;
    m_bNoRest = 0;
    m_pStaticObjectTriangleStarts = 0;
    m_pStaticObjectTriangleCounts = 0;
    m_pStaticObjectVertexStarts = 0;
    m_pStaticObjectVertexCounts = 0;
    m_pStaticObjectIds = 0;
    m_pStaticTriangles = 0;
    m_pStaticVertices = 0;
    m_pBoundingBoxes = 0;
    m_pBoundingBoxIds = 0;
    m_nTurnRateRow = 0;
    m_bDayNightCycle = 1;
    m_nShadowOpacity = 50;
}
// PC 0x005A1630 GOG 0x005A16A0
CSWArea::~CSWArea()
{
    delete m_pStaticObjectTriangleStarts;
    delete m_pStaticObjectTriangleCounts;
    delete m_pStaticObjectVertexStarts;
    delete m_pStaticObjectVertexCounts;
    delete m_pStaticObjectIds;
    delete m_pStaticTriangles;
    delete m_pStaticVertices;
    delete m_pBoundingBoxes;
    delete m_pBoundingBoxIds;
}

int CSWArea::AddStaticObject(unsigned long nId, const Vector& vOrientation,
                             const Vector& vPosition, int nVertices,
                             Vector* pVertices, int nTriangles, int* pTriangles)
{
    if (m_nStaticObjectAlloc == m_nStaticObjectCount)
    {
        GrowStaticObjectArray();
    }
    int nVertexStart;
    int nTriangleStart;
    if (m_nStaticObjectCount == 0)
    {
        nVertexStart = 0;
        nTriangleStart = 0;
    }
    else
    {
        nVertexStart = m_nStaticObjectVertexCount;
        nTriangleStart = m_nStaticTriangleCount;
    }
    if (nVertexStart + nVertices >= m_nStaticVertexAlloc)
    {
        GrowStaticObjectVertices(nVertices);
    }
    if ((nTriangleStart + nTriangles) * 3 >= m_nStaticTriangleAlloc)
    {
        GrowStaticObjectTriangles(nTriangles);
    }
    m_pStaticObjectVertexStarts[m_nStaticObjectCount] = nVertexStart;
    m_pStaticObjectVertexCounts[m_nStaticObjectCount] = nVertices;
    m_pStaticObjectTriangleStarts[m_nStaticObjectCount] = nTriangleStart;
    m_pStaticObjectTriangleCounts[m_nStaticObjectCount] = nTriangles;
    m_pStaticObjectIds[m_nStaticObjectCount++] = nId;
    int i;
    for (i = 0; i < nVertices; i++)
    {
        Vector& vVertex = m_pStaticVertices[nVertexStart + i];
        vVertex.m_fX = vOrientation.m_fY * pVertices[i].m_fX + vOrientation.m_fX * pVertices[i].m_fY;
        vVertex.m_fY = vOrientation.m_fY * pVertices[i].m_fY - vOrientation.m_fX * pVertices[i].m_fX;
        vVertex.m_fZ = pVertices[i].m_fZ;
        vVertex += vPosition;
    }
    for (i = 0; i < nTriangles * 3; i++)
    {
        m_pStaticTriangles[nTriangleStart * 3 + i] = nVertexStart + pTriangles[i];
    }
    m_nStaticTriangleCount += nTriangles;
    m_nStaticObjectVertexCount += nVertices;
    return m_nStaticObjectCount - 1;
}

void CSWArea::GrowStaticObjectArray()
{
    if (m_nStaticObjectAlloc == 0)
    {
        m_nStaticObjectAlloc = 16;
    }
    else
    {
        m_nStaticObjectAlloc *= 2;
    }
    int* pTriangleStarts = new int[m_nStaticObjectAlloc];
    int* pTriangleCounts = new int[m_nStaticObjectAlloc];
    int* pVertexStarts = new int[m_nStaticObjectAlloc];
    int* pVertexCounts = new int[m_nStaticObjectAlloc];
    unsigned long* pIds = new unsigned long[m_nStaticObjectAlloc];
    for (int i = 0; i < m_nStaticObjectCount; i++)
    {
        pTriangleStarts[i] = m_pStaticObjectTriangleStarts[i];
        pTriangleCounts[i] = m_pStaticObjectTriangleCounts[i];
        pVertexStarts[i] = m_pStaticObjectVertexStarts[i];
        pVertexCounts[i] = m_pStaticObjectVertexCounts[i];
        pIds[i] = m_pStaticObjectIds[i];
    }
    delete[] m_pStaticObjectTriangleStarts;
    delete[] m_pStaticObjectTriangleCounts;
    delete[] m_pStaticObjectVertexStarts;
    delete[] m_pStaticObjectVertexCounts;
    delete[] m_pStaticObjectIds;
    m_pStaticObjectTriangleStarts = pTriangleStarts;
    m_pStaticObjectTriangleCounts = pTriangleCounts;
    m_pStaticObjectVertexStarts = pVertexStarts;
    m_pStaticObjectVertexCounts = pVertexCounts;
    m_pStaticObjectIds = pIds;
}

void CSWArea::GrowStaticObjectVertices(int nAdditional)
{
    int nNewAlloc = m_nStaticVertexAlloc;
    if (nNewAlloc == 0)
    {
        nNewAlloc = 16;
    }
    while (nNewAlloc <= m_nStaticObjectVertexCount + nAdditional)
    {
        nNewAlloc *= 2;
    }
    Vector* pVertices = new Vector[nNewAlloc];
    for (int i = 0; i < m_nStaticObjectVertexCount; i++)
    {
        pVertices[i] = m_pStaticVertices[i];
    }
    delete[] m_pStaticVertices;
    m_nStaticVertexAlloc = nNewAlloc;
    m_pStaticVertices = pVertices;
}

void CSWArea::GrowStaticObjectTriangles(int nAdditional)
{
    int nNewAlloc = m_nStaticTriangleAlloc;
    if (nNewAlloc == 0)
    {
        nNewAlloc = 48;
    }
    while (nNewAlloc <= (m_nStaticTriangleCount + nAdditional) * 3)
    {
        nNewAlloc *= 2;
    }
    int* pTriangles = new int[nNewAlloc];
    for (int i = 0; i < m_nStaticTriangleCount * 3; i++)
    {
        pTriangles[i] = m_pStaticTriangles[i];
    }
    delete[] m_pStaticTriangles;
    m_nStaticTriangleAlloc = nNewAlloc;
    m_pStaticTriangles = pTriangles;
}
// PC 0x005A1690 GOG 0x005A1700
int CSWArea::InitializeStaticObjects()
{
    m_nStaticObjectVertexCount = 0;
    m_pStaticTriangles = 0;
    m_nStaticTriangleCount = 0;
    m_pStaticVertices = 0;
    m_nStaticObjectAlloc = 0;
    m_nStaticObjectCount = 0;
    m_pStaticObjectTriangleStarts = 0;
    m_pStaticObjectTriangleCounts = 0;
    m_pStaticObjectVertexStarts = 0;
    m_pStaticObjectVertexCounts = 0;
    m_pStaticObjectIds = 0;
    m_nStaticVertexAlloc = 0;
    m_nStaticTriangleAlloc = 0;
    m_nBoundingBoxAlloc = 0;
    m_nBoundingBoxCount = 0;
    m_pBoundingBoxes = 0;
    m_pBoundingBoxIds = 0;
    return 0;
}
// PC 0x005A16D0 GOG 0x005A1740
int CSWArea::IntersectLineSegments(float fX1, float fY1, float fX2, float fY2,
                                   float fX3, float fY3, float fX4, float fY4,
                                   float* pfX, float* pfY)
{

    double fLength1 = sqrt((fY2 - fY1) * (fY2 - fY1) + (fX2 - fX1) * (fX2 - fX1));
    float fLength2 = sqrt((fY4 - fY3) * (fY4 - fY3) + (fX4 - fX3) * (fX4 - fX3));
    float fDot = ((fY4 - fY3) * (fY2 - fY1) + (fX4 - fX3) * (fX2 - fX1));
    if (fDot / (float)fLength1 / fLength2 == 1.0)
    {
        return 0;
    }

    int bHit = 0;
    if (fX1 == fX2)
    {
        float fSlope2 = (fY4 - fY3) / (fX4 - fX3);
        *pfX = fX1;
        *pfY = (fY3 - fX3 * fSlope2) + fSlope2 * fX1;
        if (fY4 >= fY3 && *pfY >= fY3 && *pfY <= fY4)
        {
            bHit = 1;
        }
        if (fY3 >= fY4 && *pfY >= fY4 && *pfY <= fY3)
        {
            bHit = 1;
        }
        if (bHit == 1)
        {
            if (fX4 >= fX3 && !(*pfX >= fX3 && *pfX <= fX4))
            {
                bHit = 0;
            }
            if (fX3 >= fX4 && !(*pfX >= fX4 && *pfX <= fX3))
            {
                bHit = 0;
            }
            if (bHit == 1)
            {
                if (fY2 >= fY1 && !(*pfY >= fY1 && *pfY <= fY2))
                {
                    bHit = 0;
                }
                if (fY1 >= fY2 && !(*pfY >= fY2 && *pfY <= fY1))
                {
                    bHit = 0;
                }
            }
        }
    }
    else if (fX3 == fX4)
    {
        float fSlope1 = (fY2 - fY1) / (fX2 - fX1);
        *pfX = fX3;
        *pfY = (fY1 - fX1 * fSlope1) + fSlope1 * fX3;
        if (fY2 >= fY1 && *pfY >= fY1 && *pfY <= fY2)
        {
            bHit = 1;
        }
        if (fY1 >= fY2 && *pfY >= fY2 && *pfY <= fY1)
        {
            bHit = 1;
        }
        if (bHit == 1)
        {
            if (fX2 >= fX1 && !(*pfX >= fX1 && *pfX <= fX2))
            {
                bHit = 0;
            }
            if (fX1 >= fX2 && !(*pfX >= fX2 && *pfX <= fX1))
            {
                bHit = 0;
            }
            if (bHit == 1)
            {
                if (fY4 >= fY3 && !(*pfY >= fY3 && *pfY <= fY4))
                {
                    bHit = 0;
                }
                if (fY3 >= fY4 && !(*pfY >= fY4 && *pfY <= fY3))
                {
                    bHit = 0;
                }
            }
        }
    }
    else
    {
        float fSlope1 = (fY2 - fY1) / (fX2 - fX1);
        float fSlope2 = (fY4 - fY3) / (fX4 - fX3);
        if (fSlope2 - fSlope1 == 0.0)
        {
            return 0;
        }
        float fIntercept2 = fY3 - fX3 * fSlope2;
        float fX = (fIntercept2 - (fY1 - fSlope1 * fX1)) / (fSlope1 - fSlope2);
        *pfX = fX;
        *pfY = fX * fSlope2 + fIntercept2;
        if (fX4 >= fX3 && *pfX >= fX3 && *pfX <= fX4)
        {
            bHit = 1;
        }
        if (fX3 >= fX4 && *pfX >= fX4 && *pfX <= fX3)
        {
            bHit = 1;
        }
        if (bHit == 1)
        {
            if (fX2 >= fX1 && !(*pfX >= fX1 && *pfX <= fX2))
            {
                bHit = 0;
            }
            if (fX1 >= fX2 && !(*pfX >= fX2 && *pfX <= fX1))
            {
                bHit = 0;
            }
        }
    }
    return bHit;
}

void CSWArea::RemoveStaticObject(int nIndex)
{
    if (nIndex == -1 || nIndex >= m_nStaticObjectCount)
    {
        return;
    }
    int nVertexStart = m_pStaticObjectVertexStarts[nIndex];
    int nVertices = m_pStaticObjectVertexCounts[nIndex];
    int nTriangleStart = m_pStaticObjectTriangleStarts[nIndex];
    int nTriangles = m_pStaticObjectTriangleCounts[nIndex];
    m_pStaticObjectVertexStarts[nIndex] = 0;
    m_pStaticObjectVertexCounts[nIndex] = 0;
    m_pStaticObjectTriangleStarts[nIndex] = 0;
    m_pStaticObjectTriangleCounts[nIndex] = 0;
    m_pStaticObjectIds[nIndex] = AREA_INVALID_OBJECT_ID;
    int i;
    for (i = nVertexStart + nVertices; i < m_nStaticObjectVertexCount; i++)
    {
        m_pStaticVertices[i - nVertices] = m_pStaticVertices[i];
    }
    for (i = (nTriangleStart + nTriangles) * 3; i < m_nStaticTriangleCount * 3; i++)
    {
        m_pStaticTriangles[i - nTriangles * 3] = m_pStaticTriangles[i] - nVertices;
    }
    m_nStaticTriangleCount -= nTriangles;
    m_nStaticObjectVertexCount -= nVertices;
    for (i = nIndex + 1; i < m_nStaticObjectCount; i++)
    {
        m_pStaticObjectVertexStarts[i] -= nVertices;
        m_pStaticObjectTriangleStarts[i] -= nTriangles;
    }
}

int CSWArea::GetIsNight()
{
    return m_bIsNight;
}
// PC 0x005A1B70 GOG 0x005A1BE0
int CSWArea::SetFog(float fNear, float fFar, Vector vColor)
{
    m_vFogColor = vColor;
    m_fFogNear = fNear;
    m_fFogFar = fFar;
    return 1;
}
// PC 0x005A1BA0 GOG 0x005A1C10
int CSWArea::SetIsNight(int bIsNight)
{
    m_bIsNight = bIsNight;
    return 1;
}
// PC 0x005A1BC0 GOG 0x005A1C30
int CSWArea::SetWind(unsigned char nWind)
{
    m_nWind = nWind;
    return 1;
}
// PC 0x005A1BE0 GOG 0x005A1C50
int CSWArea::AddStaticBoundingBox(unsigned long nId, const Vector& vMin,
                                  const Vector& vMax)
{
    EXOXBOXASSERT(m_nBoundingBoxCount <= m_nBoundingBoxAlloc);
    if (m_nBoundingBoxCount == m_nBoundingBoxAlloc)
    {
        int nNewAlloc = m_nBoundingBoxAlloc * 2;
        if (nNewAlloc <= 8)
        {
            nNewAlloc = 8;
        }
        Vector* pBoxes = new Vector[nNewAlloc * 2];
        unsigned long* pIds = new unsigned long[nNewAlloc];
        if (m_pBoundingBoxes != NULL)
        {
            memcpy(pBoxes, m_pBoundingBoxes,
                   m_nBoundingBoxCount * 2 * sizeof(Vector));
            delete m_pBoundingBoxes;
        }
        if (m_pBoundingBoxIds != NULL)
        {
            memcpy(pIds, m_pBoundingBoxIds,
                   m_nBoundingBoxCount * sizeof(unsigned long));
            delete m_pBoundingBoxIds;
        }
        m_nBoundingBoxAlloc = nNewAlloc;
        m_pBoundingBoxes = pBoxes;
        m_pBoundingBoxIds = pIds;
    }
    m_pBoundingBoxes[m_nBoundingBoxCount * 2] = vMin;
    m_pBoundingBoxes[m_nBoundingBoxCount * 2 + 1] = vMax;
    m_pBoundingBoxIds[m_nBoundingBoxCount] = nId;
    return ++m_nBoundingBoxCount;
}
// PC 0x005A1D50 GOG 0x005A1DC0
int CSWArea::RemoveStaticBoundingBox(unsigned long nId)
{
    int i;
    int nIndex = -1;
    for (i = 0; i < m_nBoundingBoxCount; i++)
    {
        if (m_pBoundingBoxIds[i] == nId)
        {
            nIndex = i;
            break;
        }
    }
    if (nIndex < 0 || nIndex >= m_nBoundingBoxCount)
    {
        return 0;
    }
    m_nBoundingBoxCount--;
    for (i = nIndex; i < m_nBoundingBoxCount; i++)
    {
        m_pBoundingBoxIds[i] = m_pBoundingBoxIds[i + 1];
        m_pBoundingBoxes[i * 2] = m_pBoundingBoxes[i * 2 + 2];
        m_pBoundingBoxes[i * 2 + 1] = m_pBoundingBoxes[i * 2 + 3];
    }
    return 1;
}
