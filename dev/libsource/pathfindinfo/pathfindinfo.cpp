#include "pathfindinfo.h"
#include "appmanager.h"
#include "nwccreature.h"
#include "serverexoapp.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "nwsobject.h"
#include "nwscript.h"

int nPlotPathAroundCreatureCount = 0;

int nPathfindInfoUnused = 0;
float fPlotAroundCreatureAditionalSpace = 0.2f;

int bRenderHexTest;
int RenderHex;
#include <float.h>
#include <math.h>
#include <string.h>

const int CPathfindInformation::PATH_POINT_STARTPOINTS = 1;
const int CPathfindInformation::PATH_POINT_GRIDSTART = 2;
const int CPathfindInformation::PATH_POINT_GRIDEND = 3;
const int CPathfindInformation::PATH_POINT_PATHFIND = 4;
const int CPathfindInformation::PATH_POINT_SMOOTH = 5;
const int CPathfindInformation::SMOOTHING_START = 0;
const int CPathfindInformation::SMOOTHING_STEP_1 = 1;
const int CPathfindInformation::SMOOTHING_STEP_2 = 2;
const int CPathfindInformation::SMOOTHING_STEP_3 = 3;
const int CPathfindInformation::SMOOTHING_STEP_4 = 4;

Vector cross(const Vector& vLeft, const Vector& vRight);
Vector normalize(const Vector& vVector);
// PC 0x0061BC10 GOG 0x0061BBF0
void CPathfindInformation::ResetInterTileSearchData()
{
    if (m_pTileOpen != 0)
    {
        delete m_pTileOpen;
    }
    if (m_pTileClosed != 0)
    {
        delete m_pTileClosed;
    }
    if (m_pTileParent != 0)
    {
        delete m_pTileParent;
    }
    if (m_pTileCost != 0)
    {
        delete m_pTileCost;
    }
    m_nField_C0 = 0;
    m_pTileOpen = 0;
    m_pTileClosed = 0;
    m_nField_CC = 0;
    m_pTileParent = 0;
    m_pTileCost = 0;
    ResetIntraTileSearchData();
}
// PC 0x0061BCD0 GOG 0x0061BCB0
int CAvoidCreature::FindEntryAndExit(CPathfindInformation* pInfo)
{
    int nIntersections = 0;
    Vector vFrom;
    Vector vTo;
    if (pInfo == NULL || pInfo->m_pWayPoints == NULL || pInfo->m_nWayPointCount == 0)
    {
        return 0;
    }
    float* pWayPoints = pInfo->m_pWayPoints;
    float* pPoint = pWayPoints;
    vFrom = Vector(pPoint[0], pPoint[1], 0.0f);
    for (int nPoint = 2; nPoint < pInfo->m_nWayPointCount; nPoint += 2)
    {
        pPoint = &pWayPoints[nPoint];
        vTo = Vector(pPoint[0], pPoint[1], 0.0f);
        Vector vIntersection;
        for (int nSide = 0; nSide < 6; ++nSide)
        {
            Vector vFromFirst = vFrom - m_vHexPoints[nSide];
            Vector vFromSecond = vFrom - m_vHexPoints[(nSide + 1) % 6];
            Vector vToFirst = vTo - m_vHexPoints[nSide];
            Vector vToSecond = vTo - m_vHexPoints[(nSide + 1) % 6];
            int bFromFirst = vFromFirst * vFromFirst < AVOID_CREATURE_POINT_TOLERANCE_SQUARED;
            int bFromSecond = vFromSecond * vFromSecond < AVOID_CREATURE_POINT_TOLERANCE_SQUARED;
            int bToFirst = vToFirst * vToFirst < AVOID_CREATURE_POINT_TOLERANCE_SQUARED;
            int bToSecond = vToSecond * vToSecond < AVOID_CREATURE_POINT_TOLERANCE_SQUARED;
            if (bFromFirst + bFromSecond + bToFirst + bToSecond >= 2)
            {
                if (nIntersections == 0 && (bFromFirst || bFromSecond))
                {
                    m_nExitSide = nSide;
                    m_nExitSegment = nPoint / 2;
                    m_vExitPoint = vFrom;
                    nIntersections = 1;
                }
                if (bToFirst || bToSecond)
                {
                    m_nExitSide = nSide;
                    m_nExitSegment = nPoint / 2;
                    m_vExitPoint = vTo;
                    ++nIntersections;
                }
            }
            if (IntersectLineSegments2d(vFrom, vTo, m_vHexPoints[nSide],
                                       m_vHexPoints[(nSide + 1) % 6], vIntersection))
            {
                if (nIntersections == 0)
                {
                    m_nExitSide = nSide;
                    m_nExitSegment = nPoint / 2;
                    m_vExitPoint = vIntersection;
                    nIntersections = 1;
                    break;
                }
                else if (nIntersections >= 1)
                {
                    m_nEntrySide = m_nExitSide;
                    m_nEntrySegment = m_nExitSegment;
                    m_vEntryPoint = m_vExitPoint;
                    m_nExitSide = nSide;
                    m_nExitSegment = nPoint / 2;
                    m_vExitPoint = vIntersection;
                    ++nIntersections;
                    break;
                }
            }
        }
        vFrom = vTo;
    }
    int bFound;
    if (m_nEntrySide == -1 && m_nExitSide == -1)
    {
        bFound = 0;
    }
    else
    {
        if (m_nEntrySide == -1)
        {
            Vector vPoint(pWayPoints[max(0, pInfo->m_nField_9C - 2)],
                          pWayPoints[max(1, pInfo->m_nField_9C - 1)], 0.0f);
            m_nEntrySide = GetClosestPointTo(vPoint);
            m_nEntrySegment = max((pInfo->m_nField_9C - 2) / 2, 0);
            m_vEntryPoint = m_vHexPoints[m_nEntrySide];
            ++nIntersections;
        }
        if (m_nExitSide == -1)
        {
            Vector vPoint(pWayPoints[pInfo->m_nWayPointCount - 2],
                          pWayPoints[pInfo->m_nWayPointCount - 1], 0.0f);
            m_nExitSide = GetClosestPointTo(vPoint);
            m_nExitSegment = pInfo->m_nWayPointCount / 2;
            m_vExitPoint = m_vHexPoints[m_nExitSide];
            ++nIntersections;
        }
        bFound = nIntersections >= 2;
        equal(m_vExitPoint, m_vEntryPoint);
    }
    return bFound;
}
// PC 0x0061C200 GOG 0x0061C1E0
Vector* CAvoidCreature::FindPath(int& nPoints)
{
    int nEntrySide = m_nEntrySide;
    int nExitSide = m_nExitSide;
    if (nEntrySide == nExitSide)
    {
        Vector* pPath = new Vector[2];
        pPath[0] = m_vEntryPoint;
        pPath[1] = m_vExitPoint;
        nPoints = 2;
        return pPath;
    }
    if (m_nUnused != 0)
    {
        return FindPath_Left(nPoints);
    }
    return FindPath_Right(nPoints);
}
// PC 0x0061C2E0 GOG 0x0061C2C0
int CAvoidCreature::IsPathClear(unsigned long oidCreature, Vector* pPoints,
                                int nPoints, unsigned long& oidBlocker)
{
    oidBlocker = OBJECT_INVALID;
    Vector vFrom = pPoints[0];
    Vector vTo;
    for (int i = 1; i < nPoints; i++)
    {
        memcpy(&vTo, &pPoints[i], sizeof(vTo));
        if (!SideClearOfObstructions(oidCreature, vFrom, vTo, oidBlocker))
        {
            return 0;
        }
        vFrom = vTo;
    }
    return 1;
}
// PC 0x006049D0 GOG 0x006049B0
void CPathfindInformation::HandlePathfindError(int nError, char* szMessage)
{
}
// PC 0x0061A130 GOG 0x0061A110
void CPathfindInformation::DeleteFirstTileFValueAlternatives()
{
    if (m_ppFirstTileFValueAlternatives != 0)
    {
        for (int i = 0; i < m_nFirstTileCount; i++)
        {
            if (m_ppFirstTileFValueAlternatives[i] != 0)
            {
                delete m_ppFirstTileFValueAlternatives[i];
                m_ppFirstTileFValueAlternatives[i] = 0;
            }
        }
        if (m_pFirstTileFValue != 0)
        {
            delete m_pFirstTileFValue;
            m_pFirstTileFValue = 0;
        }
        if (m_pFirstTileFValue2 != 0)
        {
            delete m_pFirstTileFValue2;
            m_pFirstTileFValue2 = 0;
        }
        m_nFirstTileCount = 0;
        delete m_ppFirstTileFValueAlternatives;
        m_ppFirstTileFValueAlternatives = 0;
    }
}

const float CAvoidCreature::LINEFACTOR = 0.86602497f;
const int CAvoidCreature::AVOIDCREATURE_SUCSESS = 1;
const int CAvoidCreature::AVOIDCREATURE_FAILED = 0;
const int CAvoidCreature::AVOIDCREATURE_STARTINHEX = -1;
const int CAvoidCreature::AVOIDCREATURE_ENDINHEX = -2;

void CPathfindInformation::ResetIntraTileSearchData()
{
    if (m_ppFirstTileFValueAlternatives != 0)
    {
        DeleteFirstTileFValueAlternatives();
    }
    m_nField_D8 = 0;
    m_nField_DC = 0;
    m_nField_E0 = 0;
    if (m_pField_F4 != 0)
    {
        delete m_pField_F4;
    }
    m_pField_F4 = 0;
}
// PC 0x0061A1D0 GOG 0x0061A1B0
void CPathfindInformation::FlipStartEndPoints()
{
    float fTemp = m_fStartX;
    m_fStartX = m_fEndX;
    m_fEndX = fTemp;
    fTemp = m_fStartY;
    m_fStartY = m_fEndY;
    m_fEndY = fTemp;
    fTemp = m_fStartZ;
    m_fStartZ = m_fEndZ;
    m_fEndZ = fTemp;

    int nArea = m_nStartArea;
    m_nStartArea = m_nEndArea;
    m_nEndArea = nArea;
}
// PC 0x0061A210 GOG 0x0061A1F0
void CPathfindInformation::FlipTempEndPoints()
{
    float fTemp = m_fStartX;
    m_fStartX = m_fTempStartX;
    m_fTempStartX = fTemp;
    fTemp = m_fStartY;
    m_fStartY = m_fTempStartY;
    m_fTempStartY = fTemp;
    fTemp = m_fStartZ;
    m_fStartZ = m_fTempStartZ;
    m_fTempStartZ = fTemp;
    fTemp = m_fStartDistance;
    m_fStartDistance = m_fTempDistance;
    m_fTempDistance = fTemp;

    int nArea = m_nStartArea;
    m_nStartArea = m_nTempArea;
    m_nTempArea = nArea;
}
// PC 0x0061A250 GOG 0x0061A230
void CPathfindInformation::ComputeStepTolerance()
{
    if (m_fStepScale != 0.0f)
    {
        m_nStepTolerance = (int)(m_fStartDistance / m_fStepScale);
    }
    else
    {
        m_nStepTolerance = (int)m_fStartDistance;
    }
}
// PC 0x0061A280 GOG 0x0061A260
void CPathfindInformation::Shutdown()
{
    if (m_pWayPoints != 0)
    {
        delete m_pWayPoints;
    }
    if (m_pWayPointsTemp != 0)
    {
        delete m_pWayPointsTemp;
    }
    if (m_pAreaPath != 0)
    {
        delete m_pAreaPath;
    }
    if (m_pAreaPathCost != 0)
    {
        delete m_pAreaPathCost;
    }
    if (m_pAreaList != 0)
    {
        delete m_pAreaList;
    }
    if (m_pTileOpen != 0)
    {
        delete m_pTileOpen;
    }
    if (m_pTileClosed != 0)
    {
        delete m_pTileClosed;
    }
    if (m_pTileParent != 0)
    {
        delete m_pTileParent;
    }
    if (m_pTileCost != 0)
    {
        delete m_pTileCost;
    }
    if (m_ppFirstTileFValueAlternatives != 0)
    {
        DeleteFirstTileFValueAlternatives();
    }
    if (m_pFirstTileFValue != 0)
    {
        delete m_pFirstTileFValue;
        m_pFirstTileFValue = 0;
    }
    if (m_pFirstTileFValue2 != 0)
    {
        delete m_pFirstTileFValue2;
        m_pFirstTileFValue2 = 0;
    }
    if (m_pField_16C != 0)
    {
        delete m_pField_16C;
        m_pField_16C = 0;
    }
    if (m_pfSmoothIntermediatePoints != 0)
    {
        delete m_pfSmoothIntermediatePoints;
        m_pfSmoothIntermediatePoints = 0;
    }
    if (m_pPathPointDFSPath != 0)
    {
        delete m_pPathPointDFSPath;
        m_pPathPointDFSPath = 0;
    }
    m_pPathPointDFSPath = 0;
    if (m_pfSmoothFirstPoints != 0)
    {
        m_nSmoothFirstCount = 0;
        delete m_pfSmoothFirstPoints;
        m_pfSmoothFirstPoints = 0;
    }
    m_pfSmoothFirstPoints = 0;
    if (m_pfSmoothSecondPoints != 0)
    {
        m_nSmoothSecondCount = 0;
        delete m_pfSmoothSecondPoints;
        m_pfSmoothSecondPoints = 0;
    }
    m_pfSmoothSecondPoints = 0;
    if (m_pfSmoothFinalPoints != 0)
    {
        m_nSmoothFinalCount = 0;
        delete m_pfSmoothFinalPoints;
        m_pfSmoothFinalPoints = 0;
    }
    m_pfSmoothFinalPoints = 0;
}
// PC 0x0061A440 GOG 0x0061A420
void CPathfindInformation::ResetGridSearchData()
{
    Vector vGridStart;
    vGridStart.m_fX = 0.0f;
    vGridStart.m_fY = 0.0f;
    vGridStart.m_fZ = 0.0f;
    m_vGridStart = vGridStart;
    Vector vGridEnd;
    vGridEnd.m_fX = 0.0f;
    vGridEnd.m_fY = 0.0f;
    vGridEnd.m_fZ = 0.0f;
    m_vGridEnd = vGridEnd;
    m_fField_164 = 700.0f;
    m_nField_168 = 0;
    m_nGridDFSBestPathLength = 0;
    if (m_pGridDFSBestPath != 0)
    {
        delete m_pGridDFSBestPath;
    }
    m_pGridDFSBestPath = 0;
    m_nGridDFSBestDistance = 700;
    m_fPathPointDFSBestDistance = 100000.0f;
}
// PC 0x0061A500 GOG 0x0061A4E0
void CPathfindInformation::ResetWayPointData()
{
    Vector vOrigin;
    Vector vDirection;
    if (m_pWayPoints != 0)
    {
        delete m_pWayPoints;
    }
    if (m_pWayPointsTemp != 0)
    {
        delete m_pWayPointsTemp;
    }
    m_nWayPointCount = 0;
    m_pWayPoints = 0;
    m_nField_94 = 0;
    m_pWayPointsTemp = 0;
    m_nField_9C = 0;
    m_nField_F8 = 0;
    m_nField_104 = 0;
    vOrigin.m_fX = 0.0f;
    vOrigin.m_fY = 0.0f;
    vOrigin.m_fZ = 0.0f;
    m_vWayPointOrigin = vOrigin;
    vDirection.m_fX = 1.0f;
    vDirection.m_fY = 0.0f;
    vDirection.m_fZ = 0.0f;
    m_vWayPointDirection = vDirection;
}
// PC 0x0061A5D0 GOG 0x0061A5B0
void CPathfindInformation::ResetPathPointSearchData()
{
    Vector vStart;
    Vector vEnd;
    vStart.m_fX = 0.0f;
    vStart.m_fY = 0.0f;
    vStart.m_fZ = 0.0f;
    m_vPathPointStart = vStart;
    vEnd.m_fX = 0.0f;
    vEnd.m_fY = 0.0f;
    vEnd.m_fZ = 0.0f;
    m_vPathPointEnd = vEnd;
    m_fPathPointSearchBound = 0;
    m_nPathPointDFSBestPathLength = 0;
    if (m_pPathPointDFSBestPath != 0)
    {
        delete m_pPathPointDFSBestPath;
    }
    m_pPathPointDFSBestPath = 0;
    m_fPathPointDFSBestScore = 700.0f;
    m_fPathPointDFSBestDistance = 3.402823466e+38f;
}
// PC 0x0061A690 GOG 0x0061A670
void CAvoidCreature::ComputeHexPoints(const Vector& vCenter,
                                     const Vector& vDirection, float fRadius)
{
    Reset();
    Vector vRadius = normalize(-vDirection);
    float fScale = fRadius / LINEFACTOR;
    vRadius *= fScale;
    Quaternion qRotation(Vector(0.0f, 0.0f, 1.0f), -1.0471976f);

    Vector vPoint;
    vPoint = vRadius + vCenter;
    m_vHexPoints[0] = vPoint;

    vRadius = qRotation * vRadius;
    vPoint = vRadius + vCenter;
    vPoint.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
        vPoint, 1);
    m_vHexPoints[1] = vPoint;

    vRadius = qRotation * vRadius;
    vPoint = vRadius + vCenter;
    vPoint.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
        vPoint, 1);
    m_vHexPoints[2] = vPoint;

    vRadius = qRotation * vRadius;
    vPoint = vRadius + vCenter;
    vPoint.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
        vPoint, 1);
    m_vHexPoints[3] = vPoint;

    vRadius = qRotation * vRadius;
    vPoint = vRadius + vCenter;
    vPoint.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
        vPoint, 1);
    m_vHexPoints[4] = vPoint;

    vRadius = qRotation * vRadius;
    vPoint = vRadius + vCenter;
    vPoint.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
        vPoint, 1);
    m_vHexPoints[5] = vPoint;
}
// PC 0x0061B0B0 GOG 0x0061B090
Vector* CAvoidCreature::FindPath_Left(int& nPoints)
{
    Vector aPoints[10];
    nPoints = 0;
    int nSide = m_nEntrySide;
    aPoints[nPoints] = m_vEntryPoint;
    nPoints = 1;
    aPoints[nPoints] = m_vHexPoints[nSide];
    nPoints = 2;
    if (nSide != (m_nExitSide + 1) % 6)
    {
        do
        {
            nSide = (nSide - 1) % 6;
            if (nSide < 0)
            {
                nSide = 5;
            }
            aPoints[nPoints] = m_vHexPoints[nSide];
            nPoints++;
        } while (nSide != (m_nExitSide + 1) % 6);
    }
    aPoints[nPoints] = m_vExitPoint;
    nPoints++;
    Vector* pPath = new Vector[nPoints];
    memcpy(pPath, aPoints, sizeof(Vector) * nPoints);
    return pPath;
}
// PC 0x0061B200 GOG 0x0061B1E0
Vector* CAvoidCreature::FindPath_Right(int& nPoints)
{
    Vector aPoints[10];
    nPoints = 0;
    int nSide = (m_nEntrySide + 1) % 6;
    aPoints[nPoints] = m_vEntryPoint;
    nPoints = 1;
    aPoints[nPoints] = m_vHexPoints[nSide];
    nPoints = 2;
    while (nSide != m_nExitSide)
    {
        nSide = (nSide + 1) % 6;
        aPoints[nPoints] = m_vHexPoints[nSide];
        nPoints++;
    }
    aPoints[nPoints] = m_vExitPoint;
    nPoints++;
    Vector* pPath = new Vector[nPoints];
    memcpy(pPath, aPoints, sizeof(Vector) * nPoints);
    return pPath;
}
// PC 0x0061B330 GOG 0x0061B310
int CAvoidCreature::SideClearOfObstructions(unsigned long oidCreature,
                                            const Vector& vFrom,
                                            const Vector& vTo,
                                            unsigned long& oidBlocker)
{
    oidBlocker = OBJECT_INVALID;
    CWalkHitInfo cHitInfo;
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    int nResult = -4;
    if (pCreature != NULL)
    {
        float fDistance = pCreature->m_pPathfindInformation->m_fStepScale;
        float fWidth = pCreature->m_pPathfindInformation->m_fHalfStep;
        nResult = g_pAppManager->GetServerExoApp()->TestDirectLine(
            oidCreature,
            g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
            vFrom, vTo, fDistance, fWidth, 1, &cHitInfo);
    }
    if (bRenderHexTest)
    {
        Vector vStart = vFrom;
        Vector vEnd = vTo;
        vStart.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
            g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
            vStart, 0) + 0.2f;
        vEnd.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
            g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
            vEnd, 0) + 0.2f;
    }
    switch (nResult)
    {
    case 1:
        return 1;
    case -3:
        oidBlocker = cHitInfo.m_oidBlocker;
        return 0;
    case -2:
    case -1:
    case 0:
        return 0;
    default:
        return 0;
    }
}

// PC 0x0061B540 GOG 0x0061B520
int CAvoidCreature::InsertPathSegmentBetween(int nFirstSegment,
                                             int nLastSegment,
                                             Vector* pNewPoints, int nNewPoints,
                                             CPathfindInformation* pInfo)
{
    float* pOldPoints = pInfo->m_pWayPoints;
    int nDst = 0;
    int nSkip;
    int nTotal;
    if (nFirstSegment == nLastSegment)
    {
        nTotal = pInfo->m_nWayPointCount + 2 * nNewPoints - 2;
        nSkip = 1;
    }
    else
    {
        nTotal = pInfo->m_nWayPointCount +
                 2 * (nNewPoints + nFirstSegment - nLastSegment);
        nSkip = 0;
    }

    float* pPoints = (float*)operator new(4 * nTotal);
    for (int i = 0; i < nFirstSegment; i++)
    {
        pPoints[nDst++] = pOldPoints[2 * i];
        pPoints[nDst++] = pOldPoints[2 * i + 1];
    }
    for (int j = 0; j < nNewPoints; j++)
    {
        pPoints[nDst++] = pNewPoints->m_fX;
        pPoints[nDst++] = pNewPoints->m_fY;
        pNewPoints++;
    }
    for (int k = nSkip + nLastSegment; k < pInfo->m_nWayPointCount / 2; k++)
    {
        pPoints[nDst++] = pOldPoints[2 * k];
        pPoints[nDst++] = pOldPoints[2 * k + 1];
    }
    pInfo->m_pWayPoints = 0;
    pInfo->m_nWayPointCount = 0;
    delete pOldPoints;
    pInfo->m_pWayPoints = pPoints;
    pInfo->m_nWayPointCount = nTotal;
    return 1;
}
// PC 0x0061B660 GOG 0x0061B640
int CAvoidCreature::GetClosestPointTo(Vector& vPoint)
{
    float fBest = FLT_MAX;
    int nBest = -1;
    for (int i = 0; i < 6; i++)
    {
        Vector vDifference = m_vHexPoints[i] - vPoint;
        if (vDifference * vDifference < fBest)
        {
            nBest = i;
            fBest = vDifference * vDifference;
        }
    }
    return nBest;
}
// PC 0x0061B7F0 GOG 0x0061B7D0
int CAvoidCreature::IsPointInHex(const Vector& vPoint)
{
    for (int i = 0; i < 6; i++)
    {
        const Vector& vStart = m_vHexPoints[i];
        const Vector& vEnd = m_vHexPoints[(i + 1) % 6];
        Vector vNormal = normalize(-cross(Vector(0.0f, 0.0f, 1.0f),
                                           vEnd - vStart));
        Vector vDirection = normalize(vPoint - vStart);
        if (vNormal * vDirection < 0.0f)
        {
            return 0;
        }
    }
    return 1;
}
// PC 0x0061B900 GOG 0x0061B8E0
CPathfindInformation::~CPathfindInformation()
{
    Shutdown();
}
// PC 0x0061C3B0 GOG 0x0061C390
int CAvoidCreature::PlotPathAroundCreature(CSWSCreature* pCreature,
                                           CSWSCreature* pAvoidCreature)
{
    ++nPlotPathAroundCreatureCount;
    pCreature->GetPathfindInformation()->SetBlockingCreature(pAvoidCreature->GetId());
    pCreature->GetPathfindInformation()->m_vBlockingCreaturePosition = pAvoidCreature->m_vPosition;

    unsigned long oidFirstBlocker = OBJECT_INVALID;
    unsigned long oidSecondBlocker = OBJECT_INVALID;
    int nFirstPoints = 0;
    int nSecondPoints = 0;
    CExoString sFailureScript("k_def_pathfail01");

    for (int i = 0; i < pCreature->GetPathfindInformation()->m_nWayPointCount; ++i)
    {
    }
    if (pAvoidCreature->GetReputation(pCreature->GetId(), 0) <= CSWSObject::MAX_HOSTILE_REPUTATION)
    {
        pCreature->GetPathfindInformation()->m_nField_264 = pAvoidCreature->GetId();
        g_pVirtualMachine->RunScript(&sFailureScript, pCreature->GetId(),
                                    pCreature->GetId() != OBJECT_INVALID);
        if (pCreature->m_bCommandable)
        {
            return AVOIDCREATURE_FAILED;
        }
    }

    float fRadius;
    {
        CPathfindInformation* pCreaturePathInfo = pCreature->GetPathfindInformation();
        CPathfindInformation* pAvoidPathInfo = pAvoidCreature->GetPathfindInformation();
        fRadius = pCreaturePathInfo->GetCreaturePersonalSpace() +
                  pAvoidPathInfo->GetCreaturePersonalSpace() +
                  fPlotAroundCreatureAditionalSpace;
    }
    ComputeHexPoints(pAvoidCreature->m_vPosition, pCreature->m_vOrientation, fRadius);
    float* pWayPoints = pCreature->GetPathfindInformation()->m_pWayPoints;
    IsPointInHex(Vector(pWayPoints[0], pWayPoints[1], 0.0f));
    if (IsPointInHex(Vector(pWayPoints[pCreature->GetPathfindInformation()->GetWayPointCount() - 2],
                            pWayPoints[pCreature->GetPathfindInformation()->GetWayPointCount() - 1], 0.0f)))
    {
        return AVOIDCREATURE_ENDINHEX;
    }
    if (!FindEntryAndExit(pCreature->GetPathfindInformation()))
    {
        return AVOIDCREATURE_FAILED;
    }

    Vector* pFirstPath = FindPath(nFirstPoints);
    int bFirstBlocked = !IsPathClear(pCreature->GetId(), pFirstPath, nFirstPoints, oidFirstBlocker);
    int bLeft = m_nUnused;
    m_nUnused = !bLeft;
    Vector* pSecondPath = FindPath(nSecondPoints);
    int bSecondBlocked = !IsPathClear(pCreature->GetId(), pSecondPath, nSecondPoints, oidSecondBlocker);
    int nFirstQuality = PATH_BLOCKED;
    int nSecondQuality = PATH_BLOCKED;
    if (!bFirstBlocked)
    {
        nFirstQuality = PATH_CLEAR;
    }
    if (bFirstBlocked && oidFirstBlocker != OBJECT_INVALID)
    {
        nFirstQuality = PATH_BLOCKED_BY_CREATURE;
    }
    if (bFirstBlocked && oidFirstBlocker == OBJECT_INVALID)
    {
        nFirstQuality = PATH_BLOCKED;
    }
    if (!bSecondBlocked)
    {
        nSecondQuality = PATH_CLEAR;
    }
    if (bSecondBlocked && oidSecondBlocker != OBJECT_INVALID)
    {
        nSecondQuality = PATH_BLOCKED_BY_CREATURE;
    }
    if (bSecondBlocked && oidSecondBlocker == OBJECT_INVALID)
    {
        nSecondQuality = PATH_BLOCKED;
    }

    int nBestPath = 0;
    if (nFirstQuality >= nSecondQuality)
    {
        if (nFirstQuality > PATH_BLOCKED)
        {
            nBestPath = 1;
        }
    }
    else if (nSecondQuality > PATH_BLOCKED)
    {
        nBestPath = 2;
    }
    int bResult;
    if (nBestPath == 0)
    {
        pCreature->GetPathfindInformation()->m_nField_264 = OBJECT_INVALID;
        g_pVirtualMachine->RunScript(&sFailureScript, pCreature->GetId(),
                                    pCreature->GetId() != OBJECT_INVALID);
        bResult = AVOIDCREATURE_FAILED;
    }
    else if (nBestPath == 1)
    {
        InsertPathSegmentBetween(GetEntrySegment(), GetExitSegment(), pFirstPath,
                                 nFirstPoints, pCreature->GetPathfindInformation());
        if (pCreature->GetPathfindInformation()->GetCurrentWayPoint() > m_nEntrySegment * 2)
        {
            int nSegment = m_nEntrySegment;
            if ((nSegment << 1) < 0)
            {
                nSegment = 0;
            }
            else
            {
                nSegment *= 2;
            }
            pCreature->GetPathfindInformation()->m_nField_9C = nSegment;
        }
        if (oidFirstBlocker != OBJECT_INVALID)
        {
            CSWSCreature* pBlocker = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidFirstBlocker);
            if (pBlocker != NULL && pBlocker->GetReputation(pCreature->GetId(), 0) <= CSWSObject::MAX_HOSTILE_REPUTATION)
            {
                pCreature->GetPathfindInformation()->m_nField_264 = oidFirstBlocker;
                g_pVirtualMachine->RunScript(&sFailureScript, pCreature->GetId(),
                                            pCreature->GetId() != OBJECT_INVALID);
            }
        }
        bResult = AVOIDCREATURE_SUCSESS;
    }
    else if (nBestPath == 2)
    {
        InsertPathSegmentBetween(GetEntrySegment(), GetExitSegment(), pSecondPath,
                                 nSecondPoints, pCreature->GetPathfindInformation());
        if (pCreature->GetPathfindInformation()->GetCurrentWayPoint() > m_nEntrySegment * 2)
        {
            int nSegment = m_nEntrySegment;
            if ((nSegment << 1) < 0)
            {
                nSegment = 0;
            }
            else
            {
                nSegment *= 2;
            }
            pCreature->GetPathfindInformation()->m_nField_9C = nSegment;
        }
        if (oidSecondBlocker != OBJECT_INVALID)
        {
            CSWSCreature* pBlocker = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidSecondBlocker);
            if (pBlocker != NULL && pBlocker->GetReputation(pCreature->GetId(), 0) <= CSWSObject::MAX_HOSTILE_REPUTATION)
            {
                pCreature->GetPathfindInformation()->m_nField_264 = oidSecondBlocker;
                g_pVirtualMachine->RunScript(&sFailureScript, pCreature->GetId(),
                                            pCreature->GetId() != OBJECT_INVALID);
            }
        }
        bResult = AVOIDCREATURE_SUCSESS;
    }
    for (int j = 0; j < pCreature->GetPathfindInformation()->m_nWayPointCount; ++j)
    {
    }
    if (pFirstPath != NULL)
    {
        delete pFirstPath;
    }
    if (pSecondPath != NULL)
    {
        delete pSecondPath;
    }
    return bResult;
}
// PC 0x0061C850 GOG 0x0061C830
CPathfindInformation::CPathfindInformation()
{
    m_pGridDFSBestPath = 0;
    m_nField_34 = OBJECT_INVALID;
    Initialize();
    return;
}
// PC 0x0061C970 GOG 0x0061C950
void CPathfindInformation::ResetInterAreaPathSearchData()
{
    if (m_pAreaPath != 0)
    {
        delete m_pAreaPath;
    }
    if (m_pAreaPathCost != 0)
    {
        delete m_pAreaPathCost;
    }
    m_nField_A0 = 0;
    m_nField_A4 = 0;
    m_nField_A8 = 0;
    m_pAreaPath = 0;
    m_pAreaPathCost = 0;
    ResetGridSearchData();
    ResetWayPointData();
    ResetInterTileSearchData();
    ResetPathPointSearchData();
}
// PC 0x0061C9E0 GOG 0x0061C9C0
void CPathfindInformation::ResetInterAreaSearchData()
{
    if (m_pAreaList != 0)
    {
        delete m_pAreaList;
    }
    m_nField_84 = 0;
    m_nField_1C = 0;
    m_nField_20 = 0;
    m_nField_24 = 0;
    m_nField_B4 = 0;
    m_pAreaList = 0;
    m_nField_88 = 1;
    ResetInterAreaPathSearchData();
}
// PC 0x0061CA30 GOG 0x0061CA10
void CPathfindInformation::ResetAtEndOfPath()
{
    m_nField_194 = 0;
    m_nField_40 = 0;
    m_fTempStartX = 0;
    m_fTempStartY = 0;
    m_fTempStartZ = 0;
    m_nTempArea = 0;
    m_nField_58 = 0;
    m_fStartX = 0;
    m_fStartY = 0;
    m_fStartZ = 0;
    m_fStartDistance = 0;
    m_nStartArea = 0;
    m_fEndX = 0;
    m_fEndY = 0;
    m_fEndZ = 0;
    m_nEndArea = 0;
    ResetInterAreaSearchData();
}
// PC 0x0061B910 GOG 0x0061B8F0
void CPathfindInformation::Initialize()
{
    m_fStepScale = 0.6f;
    m_fStepScaleY = 0.6f;
    m_fStepScaleZ = 0.6f;
    m_nPathFlags = 0;
    m_fHalfStep = 0.5f;
    m_nField_1C = 0;
    m_nField_20 = 0;
    m_nField_24 = 0;
    m_nField_194 = 0;
    m_oidPathfindCreature = OBJECT_INVALID;
    m_oidPathfindTargetCreature = OBJECT_INVALID;
    m_nField_40 = 0;
    m_fTempStartX = 0.0f;
    m_fTempStartY = 0.0f;
    m_fTempStartZ = 0.0f;
    m_nTempArea = 0;
    m_nField_58 = 0;
    m_fStartX = 0.0f;
    m_fStartY = 0.0f;
    m_fStartZ = 0.0f;
    m_fStartDistance = 0.0f;
    m_nStepTolerance = 0;
    m_nStartArea = 0;
    m_fEndX = 0.0f;
    m_fEndY = 0.0f;
    m_fEndZ = 0.0f;
    m_nEndArea = 0;
    m_nField_84 = 0;
    m_nField_88 = 1;
    m_nWayPointCount = 0;
    m_pWayPoints = 0;
    m_nField_94 = 0;
    m_pWayPointsTemp = 0;
    m_nField_9C = 0;
    m_nField_A0 = 0;
    m_nField_A4 = 0;
    m_nField_A8 = 0;
    m_pAreaPath = 0;
    m_pAreaPathCost = 0;
    m_nField_B4 = 0;
    m_pAreaList = 0;
    m_nField_C0 = 0;
    m_pTileOpen = 0;
    m_pTileClosed = 0;
    m_nField_CC = 0;
    m_pTileParent = 0;
    m_pTileCost = 0;
    m_nField_D8 = 0;
    m_nField_DC = 0;
    m_nField_E0 = 0;
    m_nFirstTileCount = 0;
    m_ppFirstTileFValueAlternatives = 0;
    m_pField_F4 = 0;
    m_pFirstTileFValue = 0;
    m_pFirstTileFValue2 = 0;
    m_nField_28 = 0;
    m_nField_F8 = 0;
    m_bField_100 = 0;
    m_nField_104 = 0;
    m_vWayPointOrigin = Vector();
    m_vWayPointDirection = Vector(1.0f, 0.0f, 0.0f);
    m_fField_164 = 700.0f;
    m_nField_168 = 0;
    m_pField_16C = 0;
    m_vGridStart = Vector();
    m_vGridEnd = Vector();
    m_nGridDFSBestPathLength = 0;
    if (m_pGridDFSBestPath != 0)
    {
        delete m_pGridDFSBestPath;
    }
    m_nField_1A4 = -1;
    m_nField_1A8 = -1;
    m_pGridDFSBestPath = 0;
    m_nGridDFSBestDistance = 700;
    m_fPathPointDFSBestDistance = 100000.0f;
    m_nField_BC = OBJECT_INVALID;
    m_pPathPointDFSPath = 0;
    m_nPathPointDFSDepth = 0;
    m_pPathPointTransTable = 0;
    m_fPathPointSearchBound = 0;
    m_nPathPointDFSBestPathLength = 0;
    m_pPathPointDFSBestPath = 0;
    m_nField_1E0 = 1;
    m_nField_194 = 0;
    m_nField_198 = 0;
    m_bField_1A0 = 0;
    m_nField_210 = 0;
    m_pfSmoothIntermediatePoints = 0;
    m_pfPathPointStartPoints = 0;
    m_pfPathPointEndPoints = 0;
    m_nField_23C = 0;
    m_nField_240 = 0;
    m_nField_244 = 0;
    SetBlockingCreature(OBJECT_INVALID);
    m_nField_264 = OBJECT_INVALID;
    m_nSmoothFirstCount = 0;
    m_pfSmoothFirstPoints = 0;
    m_nSmoothSecondCount = 0;
    m_nSmoothFinalCount = 0;
    m_pfSmoothSecondPoints = 0;
    m_pfSmoothFinalPoints = 0;
    m_fTargetUseRange = 0.0f;
    m_oidUseTarget = OBJECT_INVALID;
    m_nField_26C = 0;
    m_nField_270 = 0;
    m_nField_274 = 0;
}
