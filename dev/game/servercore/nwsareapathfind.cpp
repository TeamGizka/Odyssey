#include "exobase.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "serveraimaster.h"
#include "gameobjectarray.h"
#include "nwsobject.h"
#include "pathfindinfo.h"
#include "nwccreature.h"
#include "nwsdoor.h"
#include "nwstrigger.h"
#include "nwsplaceable.h"
#include "swsroom.h"
#include "swroomsurfacemesh.h"
#include "nwsarea.h"
#include "nwsmodule.h"
#include "aurora.h"

#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

#define AREA_DIRECT_LINE_MAX_ROOM_STEPS 100
#define AREA_PATH_POINT_BLOCKER_POST_COLUMN 7
#define AREA_PATH_POINT_BLOCKER_POST_ROW 15
#define AREA_PATH_POINT_BLOCKER_POST_DURATION 5.0f

int nPathfindUnused1 = 0;
int DumpRoomBBox = 0;
int RenderTest = 0;
int SmoothTime = 10000;

int RenderDFS = 0;
int bInitnRenderDEFLines = 1;
int nRenderDEFLines[CSWSArea::PATH_POINT_DFS_RENDER_LINES];
int nRenderDFSCount = 0;

// PC 0x00651540 GOG 0x006513F0
int CSWSArea::PlotSoundPath(CPathfindInformation* pPathfindInformation)
{
    return 1;
}
// PC 0x004FC180 GOG 0x004F4260
CSWSRoom* CSWSArea::GetRoom(const Vector& vPosition, int* pnFace,
                            int* pnRoomIndex)
{
    Vector vTop(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ + 1000.0f);
    Vector vBottom(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ - 1000.0f);
    CSWRoomSurfaceMeshHitInfo hitInfo(0xFFFFFFFF);
    CSWSRoom* pRoom = NULL;

    if (pnFace != NULL) { *pnFace = -1; }

    int nLastRoom = -1;
    if (pnRoomIndex != NULL && (nLastRoom = *pnRoomIndex) >= 0 &&
        nLastRoom < m_nRoomCount &&
        m_pRooms[nLastRoom].CheckAABBWalkable(vTop, vBottom, hitInfo))
    {
        pRoom = &m_pRooms[nLastRoom];
        if (pnFace != NULL)
        {
            *pnFace = hitInfo.m_nField0C;
        }
    }
    else
    {
        for (int nRoom = 0; nRoom < m_nRoomCount; nRoom++)
        {
#ifdef _XBOX
            if (DumpRoomBBox) { g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory); g_pExoBase->m_pExoDebug->FlushScriptLogFile(); }
#endif
            if (nRoom != nLastRoom &&
                m_pRooms[nRoom].CheckAABBWalkable(vTop, vBottom, hitInfo))
            {
                pRoom = &m_pRooms[nRoom];
                if (pnFace != NULL)
                {
                    *pnFace = hitInfo.m_nField0C;
                }
                if (pnRoomIndex != NULL)
                {
                    *pnRoomIndex = nRoom;
                }
                break;
            }
        }
    }
    return pRoom;
}
// PC 0x004FC2B0 GOG 0x004F4390
CSWSRoom* CSWSArea::GetRoom(int nRoomIndex)
{
    if (nRoomIndex < 0 || nRoomIndex >= m_nRoomCount)
    {
        return NULL;
    }
    return &m_pRooms[nRoomIndex];
}
// PC 0x004FC2E0 GOG 0x004F43C0
int CSWSArea::GetRoomIndex(const Vector& vPosition, int bAllMaterials)
{
    int nResult = -1;
    Vector vTop(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ + 1000.0f);
    Vector vBottom(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ - 1000.0f);
    CSWRoomSurfaceMeshHitInfo hitInfo(0xFFFFFFFF);

    for (int nRoom = 0; nRoom < m_nRoomCount; nRoom++)
    {
        if (bAllMaterials != 0)
        {
            if (m_pRooms[nRoom].CheckAABBAll(vTop, vBottom, hitInfo))
            {
                nResult = nRoom;
                break;
            }
        }
        else
        {
            if (m_pRooms[nRoom].CheckAABBWalkable(vTop, vBottom, hitInfo))
            {
                nResult = nRoom;
                break;
            }
        }
    }
    return nResult;
}
// PC 0x004FC3E0 GOG 0x004F44C0
int CSWSArea::GetRoomForceRating(const Vector& vPosition)
{
    if (m_pnRoomForceRating == NULL)
    {
        return 0;
    }
    int nRoom = GetRoomIndex(vPosition, 0);
    return m_pnRoomForceRating[m_pnRoomAudioIndex[nRoom]];
}
// PC 0x004FC420 GOG 0x004F4500
unsigned char CSWSArea::GetRoomDisableWeather(const Vector& vPosition)
{
    if (m_pbRoomDisableWeather == NULL)
    {
        return 0;
    }
    int nRoom = GetRoomIndex(vPosition, 0);
    return m_pbRoomDisableWeather[m_pnRoomAudioIndex[nRoom]];
}
// PC 0x004FC460 GOG 0x004F4540
float CSWSArea::ComputePathDistance(int nPoints, float* pfPoints)
{
    float fDistance = 0.0f;

    for (int i = 0; i < nPoints - 2; i += 2)
    {
        float fY = pfPoints[i + 3] - pfPoints[i + 1];
        float fX = pfPoints[i + 2] - pfPoints[i];
        fDistance += (float)sqrt(fX * fX + fY * fY);
    }

    return fDistance;
}
#ifndef _XBOX
int CSWSArea::CountVisibleToPlayers(const Vector& vPosition, int nMaxCount)  // PC 0x004FC4B0 GOG 0x004F4590
{
    unsigned long oidObject;
    int nCount = 0;
    int nLimit = nMaxCount;

    if (GetFirstObjectInArea(oidObject) == 1)
    {
        Vector vStart = vPosition;
        do
        {
            if (nCount >= nLimit)
            {
                return nCount;
            }
            CGameObject* pObject;
            CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
            if (pExoApp->GetObjectArray()->GetGameObject(oidObject, &pObject) ==
                    CGameObjectArray::SUCCESS &&
                pObject->AsSWSCreature() != NULL)
            {
                unsigned long oidOwner = m_nId;
                CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                if (pServer->GetClientObjectByObjectId(oidOwner) != NULL)
                {
                    Vector vEnd = pObject->AsSWSCreature()->m_vPosition;
                    float fX = vStart.m_fX - vEnd.m_fX;
                    float fY = vStart.m_fY - vEnd.m_fY;
                    float fZ = vStart.m_fZ - vEnd.m_fZ;
                    if (fX * fX + fY * fY + fZ * fZ <= 900.0f)
                    {
                        vStart.m_fZ += 1.5f;
                        vEnd.m_fZ += 1.5f;
                        Vector vHit;
                        unsigned long oidHit;
                        if (ClearLineOfSight(vStart, vEnd, &vHit, &oidHit,
                                             OBJECT_INVALID, OBJECT_INVALID, 0, 0) == 1)
                        {
                            nCount++;
                        }
                    }
                }
            }
        } while (GetNextObjectInArea(oidObject) == 1);
    }
    return nCount;
}
#endif
int CSWSArea::EvaluateOverlappingTargets(CPathfindInformation* pPathfind,  // PC 0x004FC640 GOG 0x004F4720
                                        const Vector& vPosition,
                                        unsigned long oidTarget,
                                        const Vector& vTargetPosition,
                                        float fDistance, int nFlags,
                                        int bCheckSize,
                                        unsigned long* poidOverlap)
{
    if (RenderAi)
    {
        CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
        unsigned long oidCreature = pPathfind->m_oidPathfindCreature;
        CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
        Vector vTarget = pTarget->m_vPosition;
        Vector vCreature = pCreature->m_vPosition;
        Vector vDirection = vTarget - vCreature;
        vDirection = normalize(vDirection);
        vDirection = vCreature - vTarget;
        vDirection = normalize(vDirection);
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
    }

    Vector vDifference = vPosition - vTargetPosition;
    float fDistanceSquared = vDifference * vDifference;
    if (fDistanceSquared <= fDistance * fDistance)
    {
        return 0;
    }
    if (nFlags && fDistanceSquared <= fDistance * fDistance)
    {
        return 1;
    }
    if (poidOverlap != NULL)
    {
        *poidOverlap = oidTarget;
    }
    if (bCheckSize == 1 && pPathfind != NULL)
    {
        unsigned long oidPathTarget = pPathfind->m_oidPathfindTargetCreature;
        CSWSCreature* pPathTarget = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPathTarget);
        CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
        if (pPathTarget != NULL && pTarget != NULL)
        {
            Vector vSeparation = pPathTarget->m_vPosition - pTarget->m_vPosition;
            float fRadius = pTarget->m_pPathfindInformation->GetCreaturePersonalSpace() +
                            pPathTarget->m_pPathfindInformation->GetCreaturePersonalSpace();
            if (vSeparation.m_fX * vSeparation.m_fX +
                vSeparation.m_fY * vSeparation.m_fY < fRadius * fRadius)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x004FC850 GOG 0x004F4930
void CSWSArea::GridDFSTransTableInitialize()
{
    if (m_pGridTransTable == NULL)
    {
        m_pGridTransTable = new CSWSAreaGridTransEntry[512];
    }

    for (int i = 0; i < 512; i++)
    {
        m_pGridTransTable[i].m_nX = 0;
        m_pGridTransTable[i].m_nY = 0;
        m_pGridTransTable[i].m_nScore = 0x80000000;
        m_pGridTransTable[i].m_nTieBreak = 0;
    }
}
// PC 0x004FC8C0 GOG 0x004F49A0
int CSWSArea::GridDFSTransTableGet(int nX, int nY, int* pnScore,
                                   int* pnTieBreak)
{
    int nSlot = (17 * nX + 23 * nY + 6000) % 256;

    if (m_pGridTransTable[nSlot].m_nX == nX &&
        m_pGridTransTable[nSlot].m_nY == nY)
    {
        *pnScore = m_pGridTransTable[nSlot].m_nScore;
        *pnTieBreak = m_pGridTransTable[nSlot].m_nTieBreak;
        return 1;
    }

    nSlot += 256;
    if (m_pGridTransTable[nSlot].m_nX == nX &&
        m_pGridTransTable[nSlot].m_nY == nY)
    {
        *pnScore = m_pGridTransTable[nSlot].m_nScore;
        *pnTieBreak = m_pGridTransTable[nSlot].m_nTieBreak;
        return 1;
    }

    *pnScore = 0;
    *pnTieBreak = 0;
    return 0;
}
// PC 0x004FC980 GOG 0x004F4A60
void CSWSArea::GridDFSTransTablePut(int nX, int nY, int nScore, int nTieBreak)
{
    int nSlot = (17 * nX + 23 * nY + 6000) % 256;
    CSWSAreaGridTransEntry* pEntry = &m_pGridTransTable[nSlot];

    if (pEntry->m_nX != nX || pEntry->m_nY != nY)
    {
        if (pEntry->m_nScore <= nScore &&
            (pEntry->m_nScore != nScore || pEntry->m_nTieBreak <= nTieBreak))
        {
            nSlot += 256;
        }
    }

    m_pGridTransTable[nSlot].m_nX = nX;
    m_pGridTransTable[nSlot].m_nY = nY;
    m_pGridTransTable[nSlot].m_nScore = nScore;
    m_pGridTransTable[nSlot].m_nTieBreak = nTieBreak;
}
// PC 0x004FCA20 GOG 0x004F4B00
int CSWSArea::NoNoneWalkPolysInStaticObject(
    unsigned long oidObject, const Vector& vStart, const Vector& vEnd,
    float fDistance, float fWidth, float fHeight, unsigned long oidSource,
    int bCheckDoors, CWalkHitInfo* pWalkHitInfo)
{
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSObject* pObject = (CSWSObject*)pExoApp->GetGameObject(oidObject);

    if (pObject == NULL)
    {
        return 0;
    }

    switch (((CGameObject*)pObject)->m_nObjectType)
    {
    case 10:
    {
        CSWSDoor* pDoor = pObject->AsSWSDoor();
        return pDoor->NoNonWalkPolysInDoor(vStart, vEnd, fDistance, fWidth,
                                           fHeight, pWalkHitInfo);
    }
    case 9:
        return pObject->AsSWSPlaceable()->NoNonWalkPolys(
            vStart, vEnd, fDistance, fWidth, fHeight, pWalkHitInfo);
    }
    return 0;
}
// PC 0x004FCAB0 GOG 0x004F4B90
void CSWSArea::PathPointDFSTransTableInitialize()
{
    if (m_pPathPointTransTable == NULL ||
        IsBadWritePtr(m_pPathPointTransTable,
                      512 * sizeof(CSWSAreaPathPointTransEntry)))
    {
        m_pPathPointTransTable = new CSWSAreaPathPointTransEntry[512];
    }

    for (int i = 0; i < 512; i++)
    {
        m_pPathPointTransTable[i].m_fX = 0.0f;
        m_pPathPointTransTable[i].m_fY = 0.0f;
        m_pPathPointTransTable[i].m_fScore = FLT_MAX;
        m_pPathPointTransTable[i].m_fTieBreak = 0.0f;
    }
}
// PC 0x004FCB30 GOG 0x004F4C10
int CSWSArea::PathPointDFSTransTableGet(float fX, float fY, float* pfScore,
                                        float* pfTieBreak)
{
    int nSlot = ((int)(17 * (__int64)fX + 23 * (__int64)fY) + 6000) % 256;

    if (m_pPathPointTransTable[nSlot].m_fX == fX &&
        m_pPathPointTransTable[nSlot].m_fY == fY)
    {
        *pfScore = m_pPathPointTransTable[nSlot].m_fScore;
        *pfTieBreak = m_pPathPointTransTable[nSlot].m_fTieBreak;
        return 1;
    }

    nSlot += 256;
    if (m_pPathPointTransTable[nSlot].m_fX == fX &&
        m_pPathPointTransTable[nSlot].m_fY == fY)
    {
        *pfScore = m_pPathPointTransTable[nSlot].m_fScore;
        *pfTieBreak = m_pPathPointTransTable[nSlot].m_fTieBreak;
        return 1;
    }

    *pfScore = 0.0f;
    *pfTieBreak = 0.0f;
    return 0;
}
// PC 0x004FCC20 GOG 0x004F4D00
void CSWSArea::PathPointDFSTransTablePut(float fX, float fY, float fScore,
                                         float fTieBreak)
{
    int nSlot = ((int)(17 * (__int64)fX + 23 * (__int64)fY) + 6000) % 256;

    if (m_pPathPointTransTable[nSlot].m_fX != fX ||
        m_pPathPointTransTable[nSlot].m_fY != fY)
    {
        if (!(m_pPathPointTransTable[nSlot].m_fScore > fScore) &&
            (m_pPathPointTransTable[nSlot].m_fScore != fScore ||
             !(m_pPathPointTransTable[nSlot].m_fTieBreak > fTieBreak)))
        {
            nSlot += 256;
        }
    }

    m_pPathPointTransTable[nSlot].m_fX = fX;
    m_pPathPointTransTable[nSlot].m_fY = fY;
    m_pPathPointTransTable[nSlot].m_fScore = fScore;
    m_pPathPointTransTable[nSlot].m_fTieBreak = fTieBreak;
}
// PC 0x004FCCF0 GOG 0x004F4DD0
int CSWSArea::SetHitInfoForCreature(const Vector& vCenter, float fRadius,
                                   const Vector& vStart, const Vector& vEnd,
                                   CWalkHitInfo* pWalkHitInfo)
{
    if (pWalkHitInfo == NULL)
    {
        return 1;
    }

    Vector vHit1;
    Vector vHit2;
    int nHits = IntersectCircle2d(vCenter, fRadius, vStart, vEnd,
                                 vHit1, vHit2, true);
    if (nHits == 2)
    {
        Vector vDifference = vHit1 - vStart;
        float fDistance1 = vDifference * vDifference;
        vDifference = vHit2 - vStart;
        float fDistance2 = vDifference * vDifference;
        if (fDistance2 < fDistance1)
        {
            Vector vNormal = normalize(vHit2 - vCenter);
            Vector vTangent(-vNormal.m_fY, vNormal.m_fX, vNormal.m_fZ);
            pWalkHitInfo->SetHitInfo(CWalkHitInfo::FAILURE_BLOCKED,
                vHit2, vNormal, vHit2 - vTangent, vHit2 + vTangent,
                CWalkHitInfo::INTERSECTION_LINE, vNormal);
        }
        else
        {
            Vector vNormal = normalize(vHit1 - vCenter);
            Vector vTangent(-vNormal.m_fY, vNormal.m_fX, vNormal.m_fZ);
            pWalkHitInfo->SetHitInfo(CWalkHitInfo::FAILURE_BLOCKED,
                vHit1, vNormal, vHit1 - vTangent, vHit1 + vTangent,
                CWalkHitInfo::INTERSECTION_LINE, vNormal);
        }
    }
    else if (nHits == 1)
    {
        Vector vNormal = normalize(vHit1 - vCenter);
        Vector vTangent(-vNormal.m_fY, vNormal.m_fX, vNormal.m_fZ);
        pWalkHitInfo->SetHitInfo(CWalkHitInfo::FAILURE_BLOCKED,
            vHit1, vNormal, vHit1 - vTangent, vHit1 + vTangent,
            CWalkHitInfo::INTERSECTION_LINE, vNormal);
    }
    return nHits;
}
// PC 0x004FCF90 GOG 0x004F5070
float CSWSArea::ComputeHeight(const Vector& vPosition, int bUseFace,
                              CSWSRoom** ppRoom, int* pnRoomIndex)
{
    float fHeight = 0.0f;
    int nFace;
    CSWSRoom* pRoom = GetRoom(vPosition, &nFace, pnRoomIndex);

    if (ppRoom != NULL)
    {
        *ppRoom = pRoom;
    }

    if (pRoom != NULL)
    {
        if (bUseFace != 0)
        {
            fHeight = pRoom->ComputeHeight(nFace, vPosition);
        }
        else
        {
            fHeight = pRoom->ComputeHeight(vPosition, 0);
        }
    }
    return fHeight;
}
// PC 0x004FD020 GOG 0x004F5100
int CSWSArea::CountAreaTransitionTriggers(unsigned long* poidTriggers,
                                        int nCount,
                                        unsigned long* poidExcluded)
{
    int nTransitions = 0;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    CGameObject* pObject;
    CGameObject* pAreaObject;
    int nObjects = m_lstObjectIds.num;
    for (int i = 0; i < nObjects; i++)
    {
        unsigned long oidObject = m_lstObjectIds[i];
        if (pObjectArray->GetGameObject(oidObject, &pObject) !=
            CGameObjectArray::SUCCESS)
        {
            continue;
        }
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
        {
            CSWSTrigger* pTrigger = pObject->AsSWSTrigger();
            if (pTrigger->m_nTriggerType !=
                CSWSTrigger::TRIGGER_TYPE_AREA_TRANSITION)
            {
                continue;
            }
            if (pObjectArray->GetGameObject(pTrigger->GetTargetArea(),
                                           &pAreaObject) !=
                CGameObjectArray::SUCCESS)
            {
                continue;
            }
            CSWSArea* pArea = pAreaObject->AsSWSArea();
            if (pArea == NULL || pArea->m_bTransitionCounted != 0)
            {
                continue;
            }
            int bExcluded = 0;
            for (int j = 0; j < nCount; j++)
            {
                if (poidExcluded[j] == oidObject)
                {
                    bExcluded = 1;
                }
            }
            if (!bExcluded)
            {
                if (poidTriggers != NULL)
                {
                    poidTriggers[nTransitions] = oidObject;
                    pArea->m_bTransitionCounted = 1;
                }
                nTransitions++;
            }
        }
        else if (pObject->m_nObjectType == SW_OBJECT_TYPE_DOOR)
        {
            CSWSDoor* pDoor = pObject->AsSWSDoor();
            if (!pDoor->GetIsLinked())
            {
                continue;
            }
            CSWSObject* pLinkedObject = pDoor->GetLinkedObject();
            if (pLinkedObject == NULL)
            {
                continue;
            }
            CSWSArea* pArea = pLinkedObject->GetArea();
            if (pArea == NULL || pArea->m_bTransitionCounted != 0)
            {
                continue;
            }
            int bExcluded = 0;
            for (int j = 0; j < nCount; j++)
            {
                if (poidExcluded[j] == oidObject)
                {
                    bExcluded = 1;
                }
            }
            if (!bExcluded)
            {
                if (poidTriggers != NULL)
                {
                    poidTriggers[nTransitions] = oidObject;
                    pArea->m_bTransitionCounted = 1;
                }
                nTransitions++;
            }
        }
    }
    return nTransitions;
}
// PC 0x004FD730 GOG 0x004F5810
int CSWSArea::TestDirectLine(const Vector& vStart, const Vector& vEnd,
                             const float& fRadius, float fHeight, int nFlags,
                             CWalkHitInfo* pWalkHitInfo)
{
    int nResult = DIRECT_LINE_CLEAR;
    int bFinished = 0;
    Vector vFrom = vStart;
    Vector vClippedStart;
    Vector vClippedEnd;
#ifdef _XBOX
    if (DumpRoomBBox) { g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory); g_pExoBase->m_pExoDebug->FlushScriptLogFile(); }
#endif
    CSWSRoom* pRoom = GetRoom(vFrom, NULL);
    if (DumpRoomBBox)
    {
#ifdef _XBOX
        g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory); g_pExoBase->m_pExoDebug->FlushScriptLogFile();
#endif
        DumpRoomBBox = 0;
    }
    float fMinHeight = 0.0f;
    float fMaxHeight = 0.0f;
    int nEdge;
    unsigned long oidHit;
    int nSteps = 0;
    while (nResult == DIRECT_LINE_CLEAR && !bFinished)
    {
        if (pRoom != NULL)
        {
            int bRoomClear = 0;
            if (pRoom->ClippedLineSegmentWalkable(vFrom, vEnd, vClippedStart,
                    vClippedEnd, fMinHeight, fMaxHeight, nEdge) == 1)
            {
                fMinHeight -= AREA_DIRECT_LINE_HEIGHT_MARGIN;
                fMaxHeight += fHeight + AREA_DIRECT_LINE_HEIGHT_MARGIN;
                bRoomClear = pRoom->NoNonWalkPolysOnRoom(vClippedStart, vClippedEnd,
                    fMinHeight, fMaxHeight, fRadius, pWalkHitInfo);
            }
            if (bRoomClear)
            {
                unsigned long oidObject = m_pPathfindInformation->m_nField_BC;
                float fObjectMinHeight = fMinHeight - AREA_DIRECT_LINE_OBJECT_HEIGHT_RANGE;
                float fObjectMaxHeight = fMaxHeight + AREA_DIRECT_LINE_OBJECT_HEIGHT_RANGE;
                if (NoNonWalkPolysDetailed(vClippedStart, vClippedEnd, fObjectMinHeight, fObjectMaxHeight, fRadius, oidObject, oidHit, 0, NULL, 0, pWalkHitInfo, 0))
                {
                    if (m_pPathfindInformation != NULL && (m_pPathfindInformation->m_nField_210 || (fRadius > 0.0f && NoCreaturesOnLine(vClippedStart, vClippedEnd, m_pPathfindInformation, 1, 0, NULL, 1, pWalkHitInfo))))
                    {
                        if (vEnd.m_fX - EPSILON <= vClippedEnd.m_fX &&
                            EPSILON + vEnd.m_fX >= vClippedEnd.m_fX &&
                            vEnd.m_fY - EPSILON <= vClippedEnd.m_fY &&
                            EPSILON + vEnd.m_fY >= vClippedEnd.m_fY)
                        {
                            bFinished = 1;
                        }
                        else
                        {
                            int nRoom = pRoom->GetAdjacentRoom(nEdge);
                            if (nRoom == -1 || m_pRooms[nRoom].m_bNoWalkMesh)
                            {
                                nResult = DIRECT_LINE_NO_ROOM;
                            }
                            else
                            {
                                equal(vClippedStart, vClippedEnd);
                                pRoom = &m_pRooms[nRoom];
                                vFrom = vClippedEnd;
                            }
                        }
                    }
                    else
                    {
                        nResult = DIRECT_LINE_BLOCKED_CREATURE;
                    }
                }
                else
                {
                    nResult = DIRECT_LINE_BLOCKED_OBJECT;
                }
            }
            else
            {
                nResult = DIRECT_LINE_BLOCKED_ROOM;
            }
        }
        else
        {
            return DIRECT_LINE_NO_ROOM;
        }
        if (nSteps >= AREA_DIRECT_LINE_MAX_ROOM_STEPS)
        {
            nResult = DIRECT_LINE_NO_ROOM;
            break;
        }
        nSteps++;
    }

    if (!RenderTest)
    {
        return nResult;
    }
    {
        Vector vRenderStart = vStart;
        Vector vRenderEnd = vEnd;
        vRenderStart.m_fZ = ComputeHeight(vRenderStart, 0, NULL);
        vRenderEnd.m_fZ = ComputeHeight(vRenderEnd, 0, NULL);
    }
    return nResult;
}
// PC 0x004FDAD0 GOG 0x004F5BB0
int CSWSArea::GridDFSGenerateSuccessors(int nCost, int nRemainingCost,
                                       int nX, int nY, int nZ,
                                       CSWSAreaGridSuccessors** ppSuccessors)
{
    int nSuccessors = 0;
    Vector vStart;
    Vector vEnd;
    float fPersonalSpace = m_pPathfindInformation->m_fStepScale;
    float fHalfStep = m_pPathfindInformation->m_fHalfStep;

    *ppSuccessors = new CSWSAreaGridSuccessors[9];
    if (nCost == 0)
    {
        vStart = Vector(m_pPathfindInformation->m_fEndX,
                        m_pPathfindInformation->m_fEndY, 0.0f);
    }
    else
    {
        vStart = m_pPathfindInformation->m_vField_12C +
                 m_pPathfindInformation->m_vField_138 * nX +
                 m_pPathfindInformation->m_vField_144 * nY +
                 m_pPathfindInformation->m_vField_150 * nZ;
    }

    int bFound = 0;
    if (nRemainingCost == GRID_DIRECT_LINE_BUDGET ||
        (nRemainingCost > GRID_DIRECT_LINE_BUDGET &&
         nCost % GRID_DIRECT_LINE_COST_INTERVAL == GRID_DIRECT_LINE_COST_PHASE))
    {
        vEnd = m_pPathfindInformation->m_vField_12C;
        int nEndX = 0;
        int nEndY = 0;
        int nTolerance = m_pPathfindInformation->m_nStepTolerance;
        if (nTolerance <= 1)
        {
            if (TestDirectLine(vStart, vEnd, fPersonalSpace, fHalfStep, 1, NULL) == DIRECT_LINE_CLEAR)
            {
                (*ppSuccessors)[0].m_nCost = abs(nX) + abs(nY);
                (*ppSuccessors)[0].m_nX = 0;
                (*ppSuccessors)[0].m_nY = 0;
                (*ppSuccessors)[0].m_nZ = 0;
                return 1;
            }
        }
        else
        {
            float fDistanceSquared = nX * nX + nY * nY + nZ * nZ;
            float fDistance = (float)sqrt(fDistanceSquared);
            nEndX = (int)(nTolerance * (nX / fDistance) / AREA_GRID_DIRECT_LINE_SCALE);
            nEndY = (int)(nTolerance * (nY / fDistance) / AREA_GRID_DIRECT_LINE_SCALE);
            if (fDistanceSquared >= nTolerance * nTolerance)
            {
                vEnd = m_pPathfindInformation->m_vField_12C +
                       m_pPathfindInformation->m_vField_138 * nEndX +
                       m_pPathfindInformation->m_vField_144 * nEndY +
                       m_pPathfindInformation->m_vField_150 * 0;
                if (TestDirectLine(vStart, vEnd, fPersonalSpace, fHalfStep, 1, NULL) == DIRECT_LINE_CLEAR)
                {
                    (*ppSuccessors)[0].m_nCost = abs(nX - nEndX) + abs(nY - nEndY);
                    (*ppSuccessors)[0].m_nX = nEndX;
                    (*ppSuccessors)[0].m_nY = nEndY;
                    (*ppSuccessors)[0].m_nZ = 0;
                    return 1;
                }
            }
        }

        float fDeltaX = nEndX - nX;
        float fDeltaY = nEndY - nY;
        float fFraction = 0.5f;
        float fIncrement = 0.25f;
        do
        {
            int nTestX = nX + (int)(fFraction * fDeltaX);
            int nTestY = nY + (int)(fFraction * fDeltaY);
            if (nTestX != nX || nTestY != nY)
            {
                vEnd = m_pPathfindInformation->m_vField_12C +
                       m_pPathfindInformation->m_vField_138 * nTestX +
                       m_pPathfindInformation->m_vField_144 * nTestY;
                if (TestDirectLine(vStart, vEnd, fPersonalSpace, fHalfStep, 1, NULL) == DIRECT_LINE_CLEAR)
                {
                    (*ppSuccessors)[0].m_nCost = abs(nX - nTestX) + abs(nY - nTestY);
                    (*ppSuccessors)[0].m_nX = nTestX;
                    (*ppSuccessors)[0].m_nY = nTestY;
                    (*ppSuccessors)[0].m_nZ = 0;
                    bFound = 1;
                    fFraction += fIncrement;
                }
                else
                {
                    fFraction -= fIncrement;
                }
            }
            fIncrement *= 0.5f;
        } while (fIncrement > AREA_GRID_MINIMUM_FRACTION);
    }

    if (bFound == 1)
    {
        return 1;
    }

    for (int nDeltaY = -1; nDeltaY <= 1; ++nDeltaY)
    {
        for (int nDeltaX = -1; nDeltaX <= 1; ++nDeltaX)
        {
            if (nDeltaY != 0 || nDeltaX != 0)
            {
                vEnd = m_pPathfindInformation->m_vField_12C +
                       m_pPathfindInformation->m_vField_138 * (nDeltaX + nX) +
                       m_pPathfindInformation->m_vField_144 * (nDeltaY + nY) +
                       m_pPathfindInformation->m_vField_150 * nZ;
                if (TestDirectLine(vStart, vEnd, fPersonalSpace, fHalfStep, 1, NULL) == DIRECT_LINE_CLEAR)
                {
                    (*ppSuccessors)[nSuccessors].m_nCost = abs(nDeltaY) + abs(nDeltaX);
                    (*ppSuccessors)[nSuccessors].m_nX = nDeltaX + nX;
                    (*ppSuccessors)[nSuccessors].m_nY = nDeltaY + nY;
                    (*ppSuccessors)[nSuccessors].m_nZ = 0;
                    ++nSuccessors;
                    for (int nSort = nSuccessors - 1; nSort > 0; --nSort)
                    {
                        if (abs((*ppSuccessors)[nSort].m_nX) + abs((*ppSuccessors)[nSort].m_nY) <=
                            abs((*ppSuccessors)[nSort - 1].m_nX) + abs((*ppSuccessors)[nSort - 1].m_nY))
                        {
                            int nTemp = (*ppSuccessors)[nSort].m_nCost;
                            (*ppSuccessors)[nSort].m_nCost = (*ppSuccessors)[nSort - 1].m_nCost;
                            (*ppSuccessors)[nSort - 1].m_nCost = nTemp;
                            nTemp = (*ppSuccessors)[nSort].m_nX;
                            (*ppSuccessors)[nSort].m_nX = (*ppSuccessors)[nSort - 1].m_nX;
                            (*ppSuccessors)[nSort - 1].m_nX = nTemp;
                            nTemp = (*ppSuccessors)[nSort].m_nY;
                            (*ppSuccessors)[nSort].m_nY = (*ppSuccessors)[nSort - 1].m_nY;
                            (*ppSuccessors)[nSort - 1].m_nY = nTemp;
                            nTemp = (*ppSuccessors)[nSort].m_nZ;
                            (*ppSuccessors)[nSort].m_nZ = (*ppSuccessors)[nSort - 1].m_nZ;
                            (*ppSuccessors)[nSort - 1].m_nZ = nTemp;
                        }
                    }
                }
            }
        }
    }
    return nSuccessors;
}
// PC 0x004FE360 GOG 0x004F6440
int CSWSArea::ClosestPathPoint(const Vector& vPosition, unsigned long& nPathPoint)
{
    if (m_pPathPoints != NULL && m_pPathfindInformation != NULL)
    {
        float fMinX = vPosition.m_fX - AREA_PATH_POINT_SEARCH_STEP;
        float fMaxX = vPosition.m_fX + AREA_PATH_POINT_SEARCH_STEP;
        float fMinY = vPosition.m_fY - AREA_PATH_POINT_SEARCH_STEP;
        float fMaxY = vPosition.m_fY + AREA_PATH_POINT_SEARCH_STEP;
        float fOldMinX = vPosition.m_fX + AREA_PATH_POINT_EXCLUSION_MARGIN;
        float fOldMaxX = vPosition.m_fX - AREA_PATH_POINT_EXCLUSION_MARGIN;
        float fOldMinY = vPosition.m_fY + AREA_PATH_POINT_EXCLUSION_MARGIN;
        float fOldMaxY = vPosition.m_fY - AREA_PATH_POINT_EXCLUSION_MARGIN;
        float fFirstX = m_pPathPoints[0].m_fX;
        float fLastX = m_pPathPoints[m_nPathPointCount - 1].m_fX;
        unsigned long nClosest = PATH_POINT_INVALID;
        float fClosestDistance = FLT_MAX;
        int nStart = 0;
        int nPasses = PATH_POINT_SEARCH_PASSES;
        float fRadius = m_pPathfindInformation->m_fStepScale;
        while (nClosest == PATH_POINT_INVALID && nPasses-- > 0)
        {
            for (unsigned long i = nStart; i < m_nPathPointCount; i++)
            {
                CSWSAreaPathPoint* pPoint = &m_pPathPoints[i];
                if (pPoint->m_fX < fMinX)
                {
                    nStart = i;
                }
                else if (!(pPoint->m_fX > fOldMinX &&
                           pPoint->m_fX < fOldMaxX &&
                           pPoint->m_fY > fOldMinY &&
                           pPoint->m_fY < fOldMaxY))
                {
                    if (pPoint->m_fX > fMaxX)
                    {
                        break;
                    }
                    if (!(pPoint->m_fY < fMinY || pPoint->m_fY > fMaxY))
                    {
                        float fX = pPoint->m_fX - vPosition.m_fX;
                        float fY = pPoint->m_fY - vPosition.m_fY;
                        float fDistance = fY * fY + fX * fX;
                        if (fDistance < fClosestDistance)
                        {
                            Vector vStart = vPosition;
                            vStart.m_fZ = 0.0f;
                            Vector vEnd(pPoint->m_fX, pPoint->m_fY, 0.0f);
                            CWalkHitInfo cHitInfo;
                            float fHeight = m_pPathfindInformation->m_fHalfStep;
                            int nResult = TestDirectLine(vStart, vEnd, fRadius,
                                                       fHeight, 1, &cHitInfo);
                            if (nResult == DIRECT_LINE_CLEAR ||
                                nResult == DIRECT_LINE_BLOCKED_CREATURE)
                            {
                                fClosestDistance = fDistance;
                                nClosest = i;
                            }
                        }
                    }
                }
            }
            if (nClosest != PATH_POINT_INVALID)
            {
                break;
            }

            fMinX -= AREA_PATH_POINT_SEARCH_STEP;
            if (!(fMinX > fFirstX))
                fMinX = fFirstX;
            fMaxX += AREA_PATH_POINT_SEARCH_STEP;
            if (!(fMaxX < fLastX))
                fMaxX = fLastX;
            fMinY -= AREA_PATH_POINT_SEARCH_STEP;
            fMaxY += AREA_PATH_POINT_SEARCH_STEP;
            fOldMinX -= AREA_PATH_POINT_SEARCH_STEP;
            if (!(fOldMinX > fFirstX))
                fOldMinX = fFirstX;
            fOldMaxX += AREA_PATH_POINT_SEARCH_STEP;
            if (!(fOldMaxX < fLastX))
                fOldMaxX = fLastX;
            fOldMinY -= AREA_PATH_POINT_SEARCH_STEP;
            fOldMaxY += AREA_PATH_POINT_SEARCH_STEP;
            while (nStart > 0 && m_pPathPoints[nStart].m_fX > fMinX)
            {
                nStart--;
            }
        }
        nPathPoint = nClosest;
        return nClosest != PATH_POINT_INVALID;
    }
    else
    {
        nPathPoint = 0;
        return 1;
    }
}
// PC 0x004FE6F0 GOG 0x004F67D0
int CSWSArea::PathPointDFSGenerateSuccessors(
    float fDistance, int nDepth, unsigned long nPathPoint,
    CSWSPathPointSuccessors** ppSuccessors)
{
    int nSuccessors = 0;
    float fRadius = m_pPathfindInformation->m_fStepScale;
    EXOXBOXASSERT(nPathPoint < m_nPathPointCount);
    unsigned long nFirstConnection = m_pPathPoints[nPathPoint].m_nFirstConnection;
    int nConnections = m_pPathPoints[nPathPoint].m_nConnectionCount;
    *ppSuccessors = new CSWSPathPointSuccessors[nConnections];
    Vector vStart(m_pPathPoints[nPathPoint].m_fX,
                  m_pPathPoints[nPathPoint].m_fY, 0.0f);
    Vector vEnd;

    for (unsigned long nConnection = nFirstConnection;
         nConnection < nFirstConnection + nConnections; nConnection++)
    {
        vEnd.m_fX = m_pPathPoints[m_pPathConnections[nConnection]].m_fX;
        vEnd.m_fY = m_pPathPoints[m_pPathConnections[nConnection]].m_fY;
        float fDetourDistance = 0.0f;
        unsigned long oidCreature;
        if (!NoCreaturesOnLine(vStart, vEnd, m_pActivePathfindInformation,
                               1, 0, &oidCreature, 0, NULL))
        {
            Vector vSide = vEnd - vStart;
            float fSideY = -vSide.m_fY;
            vSide.m_fY = vSide.m_fX;
            vSide.m_fX = fSideY;
            vSide = normalize(vSide);
            CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID(oidCreature);
            if (pCreature == NULL)
                continue;
            float fAvoidRadius = pCreature->GetPathfindInformation()->m_fStepScale;
            fAvoidRadius += fRadius;
            Vector vDetour = vSide * fAvoidRadius;
            vDetour += pCreature->m_vPosition;
            vDetour.m_fZ = 0.0f;
            fDetourDistance = magnitude(vDetour - vStart) +
                              magnitude(vEnd - vDetour);
        }

        float fMinHeight = (vStart.m_fZ < vEnd.m_fZ ?
                            vStart.m_fZ : vEnd.m_fZ) - AREA_PATH_POINT_HEIGHT_MARGIN;
        float fMaxHeight = (vStart.m_fZ > vEnd.m_fZ ?
                            vStart.m_fZ : vEnd.m_fZ) + AREA_PATH_POINT_HEIGHT_MARGIN;
        unsigned long oidHitObject;
        if (!NoNonWalkPolysDetailed(vStart, vEnd, fMinHeight, fMaxHeight,
                                    fRadius, OBJECT_INVALID, oidHitObject,
                                    1, NULL, 0, NULL, 0))
        {
            CGameObject* pObject = g_pAppManager->GetServerExoApp()
                ->GetGameObject(oidHitObject);
            if (pObject != NULL && pObject->AsSWSObject() != NULL)
            {
                CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
                CSWSDoor* pDoor = pObject->AsSWSDoor();
                int bPlaceable = 0;
                if (pPlaceable != NULL)
                {
                    char szBlocker[256];
                    Vector vPosition = pPlaceable->m_vPosition;
                    sprintf(szBlocker, "\"%s\" at ( %.1f, %.1f, %.1f )", pPlaceable->m_sTag.CStr(),
                            vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ);
                    AurPostString(szBlocker, AREA_PATH_POINT_BLOCKER_POST_COLUMN, AREA_PATH_POINT_BLOCKER_POST_ROW,
                                  AREA_PATH_POINT_BLOCKER_POST_DURATION);
                    bPlaceable = 1;
                }
                if ((pDoor != NULL && pDoor->m_bStatic) || bPlaceable)
                    continue;
            }
        }

        int nGoalPathPoint = m_pPathfindInformation->m_nField_1A8;
        Vector vGoal(m_pPathPoints[nGoalPathPoint].m_fX,
                     m_pPathPoints[nGoalPathPoint].m_fY,
                     0.0f);
        (*ppSuccessors)[nSuccessors].m_nPathPoint = m_pPathConnections[nConnection];
        if (fDetourDistance != 0.0f)
            (*ppSuccessors)[nSuccessors].m_fDistance = fDetourDistance;
        else
            (*ppSuccessors)[nSuccessors].m_fDistance = magnitude(vEnd - vStart);
        (*ppSuccessors)[nSuccessors].m_fDistanceToGoal = magnitude(vEnd - vGoal);
        nSuccessors++;
        for (int nSort = nSuccessors - 1; nSort > 0; nSort--)
        {
            if ((*ppSuccessors)[nSort].m_fDistanceToGoal <=
                (*ppSuccessors)[nSort - 1].m_fDistanceToGoal)
            {
                unsigned long nSwap = (*ppSuccessors)[nSort].m_nPathPoint;
                (*ppSuccessors)[nSort].m_nPathPoint = (*ppSuccessors)[nSort - 1].m_nPathPoint;
                (*ppSuccessors)[nSort - 1].m_nPathPoint = nSwap;
                float fSwap = (*ppSuccessors)[nSort].m_fDistance;
                (*ppSuccessors)[nSort].m_fDistance = (*ppSuccessors)[nSort - 1].m_fDistance;
                (*ppSuccessors)[nSort - 1].m_fDistance = fSwap;
                fSwap = (*ppSuccessors)[nSort].m_fDistanceToGoal;
                (*ppSuccessors)[nSort].m_fDistanceToGoal = (*ppSuccessors)[nSort - 1].m_fDistanceToGoal;
                (*ppSuccessors)[nSort - 1].m_fDistanceToGoal = fSwap;
            }
        }
    }
    return nSuccessors;
}
// PC 0x004FEBD0 GOG 0x004F6CB0
Vector CSWSArea::ComputeAwayVector(const Vector& vPosition, const Vector& vAway,
                                  float fDistance, CPathfindInformation* pPathfind)
{
    Vector vDifference = vAway - vPosition;
    float fOriginalDistanceSquared = vDifference.m_fZ * vDifference.m_fZ +
                                     vDifference.m_fX * vDifference.m_fX +
                                     vDifference.m_fY * vDifference.m_fY;
    vDifference.m_fZ = 0.0f;
    Vector vDirection = normalize(vDifference);
    float fDistanceSquared = fDistance * fDistance;
    float fBestScore = 0.0f;
    Vector vBest = vPosition;

    for (int nDirection = AWAY_FORWARD; nDirection < AWAY_DIRECTION_COUNT; ++nDirection)
    {
        float fAngle = nDirection * AREA_AWAY_ANGLE_STEP;
        double fCos = cos(fAngle);
        double fSin = sin(fAngle);
        Vector vRotated(vDirection.m_fY * fSin + vDirection.m_fX * fCos,
                     fCos * vDirection.m_fY - vDirection.m_fX * fSin, 0.0f);
        Vector vCandidate = vPosition + vRotated * fDistance;
        Vector vSeparation = vAway - vCandidate;
        if (vSeparation * vSeparation < fDistanceSquared)
        {
            vCandidate = vCandidate + vRotated * fDistance;
        }

        if (pPathfind != NULL)
        {
            m_pPathfindInformation = pPathfind;
            float fRadius = pPathfind->m_fStepScale;
            float fHeight = pPathfind->m_fHalfStep;
            if (TestDirectLine(vPosition, vCandidate, fRadius, fHeight, 1, NULL) != DIRECT_LINE_CLEAR)
            {
                int bFound = 0;
                float fFraction = 1.0f;
                while (fFraction >= AREA_AWAY_MINIMUM_FRACTION)
                {
                    if (bFound != 0)
                    {
                        break;
                    }
                    vCandidate = vPosition + (vRotated * fDistance) * fFraction;
                    float fTestRadius = pPathfind->m_fStepScale;
                    float fTestHeight = pPathfind->m_fHalfStep;
                    if (TestDirectLine(vPosition, vCandidate, fTestRadius, fTestHeight, 1, NULL) == DIRECT_LINE_CLEAR)
                    {
                        bFound = 1;
                    }
                    fFraction *= 0.5f;
                }
                if (bFound != 1)
                {
                    continue;
                }
                Vector vNewSeparation = vAway - vCandidate;
                if (!(vNewSeparation * vNewSeparation >= fOriginalDistanceSquared))
                {
                    continue;
                }
            }

            float fScore = magnitude(vCandidate - vAway);
            if (nDirection == AWAY_BACK)
                fScore *= AREA_AWAY_REVERSE_WEIGHT;
            else if (nDirection == AWAY_BACK_RIGHT || nDirection == AWAY_BACK_LEFT)
                fScore *= AREA_AWAY_REVERSE_DIAGONAL_WEIGHT;
            else if (nDirection == AWAY_RIGHT || nDirection == AWAY_LEFT)
                fScore *= AREA_AWAY_SIDE_WEIGHT;
            else if (nDirection == AWAY_FORWARD_RIGHT || nDirection == AWAY_FORWARD_LEFT)
                fScore *= AREA_AWAY_FORWARD_DIAGONAL_WEIGHT;
            if (fScore >= fBestScore)
            {
                fBestScore = fScore;
                vBest = vCandidate;
            }
        }
    }
    return vBest;
}

#include "nwsspellimpact.h"

#include "nwsencounter.h"
// PC 0x004FEFF0 GOG 0x004F70D0
int CSWSArea::ComputeBestCorner(float fX1, float fY1, float fX2, float fY2,
                               float fX3, float fY3, float fRadius, float fWidth,
                               float* pfX1, float* pfY1, float* pfX2, float* pfY2)
{
    int bFound = 0;
    float fLength2 = (float)sqrt((fY3 - fY2) * (fY3 - fY2) + (fX3 - fX2) * (fX3 - fX2));
    float fLength1 = (float)sqrt((fY2 - fY1) * (fY2 - fY1) + (fX2 - fX1) * (fX2 - fX1));

    if (fLength1 < 0.0001f || fLength2 < 0.0001f)
    {
        return 0;
    }
    if (((fX2 - fX1) * (fX3 - fX2) + (fY2 - fY1) * (fY3 - fY2)) / (fLength1 * fLength2) >=
        SPLINE_COSINE_LIMIT)
    {
        return 0;
    }

    float fLength = fLength1;
    if (fLength2 < fLength1)
    {
        fLength = fLength2;
    }
    float fFraction = 0.5f;
    float fStep = 0.25f;
    for (int bFirst = 1; bFirst == 1 || fStep * fLength > 0.1f; bFirst = 0)
    {
        float fStartX = (fX1 - fX2) * (fFraction * fLength) / fLength1 + fX2;
        float fStartY = (fY1 - fY2) * (fFraction * fLength) / fLength1 + fY2;
        float fEndX = (fX3 - fX2) * (fFraction * fLength) / fLength2 + fX2;
        float fEndY = (fY3 - fY2) * (fFraction * fLength) / fLength2 + fY2;
        if (TestDirectLine(Vector(fStartX, fStartY, 0.0f),
                           Vector(fEndX, fEndY, 0.0f), fRadius, fWidth, 1, NULL) == 1)
        {
            *pfX1 = fStartX;
            *pfX2 = fEndX;
            *pfY1 = fStartY;
            *pfY2 = fEndY;
            bFound = 1;
            fFraction += fStep;
        }
        else
        {
            fFraction -= fStep;
        }
        fStep *= 0.5f;
    }
    return bFound;
}
// PC 0x004FF230 GOG 0x004F7310
int CSWSArea::TestSafeLocationPoint(const Vector& vPosition,
                                   CPathfindInformation* pPathfind)
{
    m_pPathfindInformation = pPathfind;
    float fRadius = pPathfind->m_fStepScale;
    int bFinished = 0;
    Vector vStart = vPosition - Vector(AREA_SAFE_LOCATION_HORIZONTAL_RADIUS,
                                       AREA_SAFE_LOCATION_HORIZONTAL_RADIUS,
                                       AREA_SAFE_LOCATION_VERTICAL_RADIUS);
    Vector vEnd = vPosition + Vector(AREA_SAFE_LOCATION_HORIZONTAL_RADIUS,
                                     AREA_SAFE_LOCATION_HORIZONTAL_RADIUS,
                                     AREA_SAFE_LOCATION_VERTICAL_RADIUS);
    float fMinHeight = vPosition.m_fZ;
    fMinHeight -= AREA_SAFE_LOCATION_VERTICAL_RADIUS;
    float fMaxHeight = pPathfind->m_fHalfStep;
    fMaxHeight += fMinHeight;
    fMaxHeight += AREA_SAFE_LOCATION_VERTICAL_RADIUS;
    CSWSRoom* pRoom = GetRoom(vStart, NULL);
    if (pRoom == NULL)
    {
        return 0;
    }
    do
    {
        Vector vClippedStart;
        Vector vClippedEnd;
        int nEdge;
        pRoom->m_pWalkMesh->ComputeClippedLineSegment(
            vStart, vEnd, vClippedStart, vClippedEnd, nEdge);
        if (!pRoom->NoNonWalkPolysOnRoom(vClippedStart, vClippedEnd,
                                        fMinHeight, fMaxHeight, fRadius, NULL))
        {
            return 0;
        }
        if (!(vEnd.m_fX - EPSILON <= vClippedEnd.m_fX &&
              EPSILON + vEnd.m_fX >= vClippedEnd.m_fX &&
              vEnd.m_fY - EPSILON <= vClippedEnd.m_fY &&
              EPSILON + vEnd.m_fY >= vClippedEnd.m_fY))
        {
            int nRoom = pRoom->GetAdjacentRoom(nEdge);
            if (nRoom == -1)
            {
                return 0;
            }
            if (m_pRooms[nRoom].m_bNoWalkMesh)
            {
                return 0;
            }
            pRoom = &m_pRooms[nRoom];
            if (equal(vClippedStart, vClippedEnd))
            {
                bFinished = 1;
            }
            vStart = vClippedEnd;
        }
        else
        {
            break;
        }
    } while (!bFinished);

    unsigned long oidObject = pPathfind->m_nField_BC;
    if (!NoNonWalkPolys(vStart, vEnd, fMinHeight, fMaxHeight, fRadius,
                        oidObject, NULL, 0))
    {
        return 0;
    }
    if (NoCreaturesOnLine(vStart, vEnd, pPathfind, 1, 0, NULL, 0, NULL))
    {
        return 1;
    }
    return 0;
}
// PC 0x004FF4B0 GOG 0x004F7590
int CSWSArea::ComputeSafeLocation(const Vector& vPosition, float fRadius,
                                  CPathfindInformation* pPathfind,
                                  int bCheckLine, Vector* pSafePosition)
{
    *pSafePosition = vPosition;
    m_pPathfindInformation = pPathfind;
    float fMinHeight;
    float fStep = pPathfind->m_fStepScale;
    Vector vTest = vPosition;
    vTest.m_fZ = ComputeHeight(vTest, 1, NULL);
    if (TestSafeLocationPoint(vTest, pPathfind) == 1)
    {
        *pSafePosition = vTest;
        return 1;
    }

    unsigned long oidHit;
    if (PositionWalkable(vTest))
    {
        float fHeight = pPathfind->m_fHalfStep;
        float fWidth = pPathfind->m_fStepScale;
        Vector vMax = vTest + Vector(AREA_SAFE_LOCATION_HORIZONTAL_RADIUS,
                                     AREA_SAFE_LOCATION_HORIZONTAL_RADIUS, 0.0f);
        Vector vMin = vTest + Vector(-AREA_SAFE_LOCATION_HORIZONTAL_RADIUS,
                                     -AREA_SAFE_LOCATION_HORIZONTAL_RADIUS, 0.0f);
        if (!NoNonWalkPolysInDoors(vMin, vMax, vTest.m_fZ,
                fHeight + vTest.m_fZ, fWidth, OBJECT_INVALID, oidHit, NULL, 1, 0))
        {
            bCheckLine = 0;
        }
    }
    else
    {
        bCheckLine = 0;
    }

    float fX;
    float fY;
    float fMaxHeight;
    if (fRadius > 0.0f)
    {
        for (float fDistance = AREA_SAFE_LOCATION_INITIAL_RADIUS;
             fDistance < fRadius; fDistance += fStep)
        {
            float fMinX = vPosition.m_fX - fDistance;
            float fMaxX = fDistance + vPosition.m_fX;
            float fMinY = vPosition.m_fY - fDistance;
            float fMaxY = fDistance + vPosition.m_fY;
            for (fX = fMinX; fX <= fMaxX; fX += fStep)
            {
                for (fY = fMinY; fY <= fMaxY; fY += fMaxY - fMinY)
                {
                    vTest.m_fX = fX;
                    vTest.m_fY = fY;
                    vTest.m_fZ = ComputeHeight(vTest, 0, NULL);
                    if (TestSafeLocationPoint(vTest, pPathfind) == 1)
                    {
                        if (vTest.m_fZ < vPosition.m_fZ)
                        {
                            fMinHeight = vTest.m_fZ;
                            fMaxHeight = vPosition.m_fZ;
                        }
                        else
                        {
                            fMaxHeight = vTest.m_fZ;
                            fMinHeight = vPosition.m_fZ;
                        }
                        fMinHeight -= AREA_SAFE_LOCATION_VERTICAL_RADIUS;
                        fMaxHeight += pPathfind->m_fHalfStep;
                        if (bCheckLine)
                        {
                            float fWidth = pPathfind->m_fStepScale;
                            float fHeight = pPathfind->m_fHalfStep;
                            int nResult = TestDirectLine(vTest, vPosition, fWidth,
                                                         fHeight, 0, NULL);
                            if (nResult != DIRECT_LINE_CLEAR &&
                                nResult != DIRECT_LINE_BLOCKED_CREATURE)
                            {
                                continue;
                            }
                            if (!NoNonWalkPolysInDoors(vTest, vPosition, fMinHeight,
                                    fMaxHeight, fStep, OBJECT_INVALID, oidHit, NULL, 1, 0))
                            {
                                continue;
                            }
                        }
                        *pSafePosition = vTest;
                        return 1;
                    }
                }
            }
            for (fX = fMinX; fX <= fMaxX; fX += fMaxX - fMinX)
            {
                for (fY = fMinY + fStep; fY <= fMaxY - fStep; fY += fStep)
                {
                    vTest.m_fX = fX;
                    vTest.m_fY = fY;
                    vTest.m_fZ = ComputeHeight(vTest, 0, NULL);
                    if (TestSafeLocationPoint(vTest, pPathfind) == 1)
                    {
                        if (vTest.m_fZ < vPosition.m_fZ)
                        {
                            fMinHeight = vTest.m_fZ;
                            fMaxHeight = vPosition.m_fZ;
                        }
                        else
                        {
                            fMaxHeight = vTest.m_fZ;
                            fMinHeight = vPosition.m_fZ;
                        }
                        float fHeight = pPathfind->m_fHalfStep;
                        if (bCheckLine)
                        {
                            float fWidth = pPathfind->m_fStepScale;
                            float fLineHeight = pPathfind->m_fHalfStep;
                            if (!TestDirectLine(vTest, vPosition, fWidth,
                                                fLineHeight, 0, NULL))
                            {
                                continue;
                            }
                            fMinHeight -= AREA_SAFE_LOCATION_VERTICAL_RADIUS;
                            fMaxHeight += fHeight;
                            if (!NoNonWalkPolysInDoors(vTest, vPosition,
                                    fMinHeight, fMaxHeight, fStep,
                                    OBJECT_INVALID, oidHit, NULL, 1, 0))
                            {
                                continue;
                            }
                        }
                        *pSafePosition = vTest;
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    float fMaxDistance = fRadius * -1.0f;
    if (fMaxDistance < fStep)
        fMaxDistance = fStep;
    for (float fDistance = fStep; fDistance <= fMaxDistance; fDistance += fStep)
    {
        float fMinX = vPosition.m_fX - fDistance;
        float fMaxX = fDistance + vPosition.m_fX;
        float fMinY = vPosition.m_fY - fDistance;
        float fMaxY = fDistance + vPosition.m_fY;
        for (fX = fMinX; fX <= fMaxX; fX += fStep)
        {
            for (fY = fMinY; fY <= fMaxY; fY += fMaxY - fMinY)
            {
                vTest.m_fX = fX;
                vTest.m_fY = fY;
                vTest.m_fZ = ComputeHeight(vTest, 0, NULL);
                if (TestSafeLocationPoint(vTest, pPathfind) == 1)
                {
                    if (vTest.m_fZ < vPosition.m_fZ)
                    {
                        fMinHeight = vTest.m_fZ;
                        fMaxHeight = vPosition.m_fZ;
                    }
                    else
                    {
                        fMaxHeight = vTest.m_fZ;
                        fMinHeight = vPosition.m_fZ;
                    }
                    fMinHeight -= AREA_SAFE_LOCATION_VERTICAL_RADIUS;
                    fMaxHeight += pPathfind->m_fHalfStep;
                    if (bCheckLine)
                    {
                        float fWidth = pPathfind->m_fStepScale;
                        float fHeight = pPathfind->m_fHalfStep;
                        int nResult = TestDirectLine(vTest, vPosition, fWidth,
                                                     fHeight, 0, NULL);
                        if (nResult != DIRECT_LINE_CLEAR &&
                            nResult != DIRECT_LINE_BLOCKED_CREATURE)
                        {
                            continue;
                        }
                        if (!NoNonWalkPolysInDoors(vTest, vPosition, fMinHeight,
                                fMaxHeight, fStep, OBJECT_INVALID, oidHit, NULL, 1, 0))
                        {
                            continue;
                        }
                    }
                    *pSafePosition = vTest;
                    return 1;
                }
            }
        }
        for (fX = fMinX; fX <= fMaxX; fX += fMaxX - fMinX)
        {
            for (fY = fMinY + fStep; fY <= fMaxY - fStep; fY += fStep)
            {
                vTest.m_fX = fX;
                vTest.m_fY = fY;
                vTest.m_fZ = ComputeHeight(vTest, 0, NULL);
                if (TestSafeLocationPoint(vTest, pPathfind) == 1)
                {
                    if (vTest.m_fZ < vPosition.m_fZ)
                    {
                        fMinHeight = vTest.m_fZ;
                        fMaxHeight = vPosition.m_fZ;
                    }
                    else
                    {
                        fMaxHeight = vTest.m_fZ;
                        fMinHeight = vPosition.m_fZ;
                    }
                    float fHeight = pPathfind->m_fHalfStep;
                    if (bCheckLine)
                    {
                        float fWidth = pPathfind->m_fStepScale;
                        float fLineHeight = pPathfind->m_fHalfStep;
                        if (!TestDirectLine(vTest, vPosition, fWidth,
                                            fLineHeight, 0, NULL))
                        {
                            continue;
                        }
                        fMinHeight -= AREA_SAFE_LOCATION_VERTICAL_RADIUS;
                        fMaxHeight += fHeight;
                        if (!NoNonWalkPolysInDoors(vTest, vPosition,
                                fMinHeight, fMaxHeight, fStep,
                                OBJECT_INVALID, oidHit, NULL, 1, 0))
                        {
                            continue;
                        }
                    }
                    *pSafePosition = vTest;
                    return 1;
                }
            }
        }
    }
    return 0;
}
// PC 0x004FFE80 GOG 0x004F7F60
int CSWSArea::ComputeNonVisibleLocation(const Vector& vPosition,
                                       CPathfindInformation* pPathfind,
                                       int nFlags, Vector* pvLocation,
                                       float fDistance)
{
    Vector vCandidate;
    int nBestCount = AREA_NON_VISIBLE_PLAYER_LIMIT;
    if (pPathfind != NULL)
    {
        *pvLocation = Vector(-1.0f, -1.0f, -1.0f);
        m_pPathfindInformation = pPathfind;
        float fStep = pPathfind->m_fStepScale;
        vCandidate = vPosition;
        vCandidate.m_fZ = ComputeHeight(vCandidate, 0, NULL);
        if (TestSafeLocationPoint(vCandidate, pPathfind) == 1)
        {
            int nCount = CountVisibleToPlayers(vPosition, AREA_NON_VISIBLE_PLAYER_LIMIT);
            if (nCount < AREA_NON_VISIBLE_PLAYER_LIMIT)
            {
                *pvLocation = vCandidate;
                nBestCount = nCount;
            }
            if (nCount == 0)
            {
                return 1;
            }
        }

        Vector vHit;
        unsigned long oidHit;
        for (float fRadius = 1.0f; fRadius < AREA_NON_VISIBLE_SEARCH_RADIUS;
             fRadius += fStep)
        {
            float fMinX = vPosition.m_fX - fRadius;
            float fMaxX = vPosition.m_fX + fRadius;
            float fMinY = vPosition.m_fY - fRadius;
            float fMaxY = vPosition.m_fY + fRadius;
            float fX;
            float fY;
            for (fX = fMinX; fX <= fMaxX; fX += fStep)
            {
                for (fY = fMinY; fY <= fMaxY; fY += fMaxY - fMinY)
                {
                    vCandidate.m_fX = fX;
                    vCandidate.m_fY = fY;
                    vCandidate.m_fZ = ComputeHeight(vCandidate, 0, NULL);
                    if (TestSafeLocationPoint(vCandidate, pPathfind) == 1 &&
                        magnitude(vPosition - vCandidate) <= AREA_NON_VISIBLE_SIGHT_DISTANCE &&
                        ClearLineOfSight(vPosition, vCandidate, &vHit, &oidHit,
                                         OBJECT_INVALID, OBJECT_INVALID, 0, 0) == 1)
                    {
                        int nCount = CountVisibleToPlayers(vPosition, nBestCount);
                        if (nBestCount > nCount)
                        {
                            *pvLocation = vCandidate;
                            nBestCount = nCount;
                        }
                        if (nCount == 0)
                        {
                            return 1;
                        }
                    }
                }
            }
            for (fX = fMinX; fX <= fMaxX; fX += fMaxX - fMinX)
            {
                for (fY = fMinY + fStep; fY <= fMaxY - fStep; fY += fStep)
                {
                    vCandidate.m_fX = fX;
                    vCandidate.m_fY = fY;
                    vCandidate.m_fZ = ComputeHeight(vCandidate, 0, NULL);
                    if (TestSafeLocationPoint(vCandidate, pPathfind) == 1 &&
                        magnitude(vPosition - vCandidate) <= AREA_NON_VISIBLE_SIGHT_DISTANCE &&
                        ClearLineOfSight(vPosition, vCandidate, &vHit, &oidHit,
                                         OBJECT_INVALID, OBJECT_INVALID, 0, 0) == 1)
                    {
                        int nCount = CountVisibleToPlayers(vPosition, nBestCount);
                        if (nBestCount > nCount)
                        {
                            *pvLocation = vCandidate;
                            nBestCount = nCount;
                        }
                        if (nCount == 0)
                        {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x00500320 GOG 0x004F8400
int CSWSArea::IntersectingLineSegment(const Vector& vStart, const Vector& vEnd,
                                      CExoArrayList<unsigned long>* pObjects)
{
    int bIntersecting = 0;
    for (int i = 0; i < m_lstTriggerIds.num; i++)
    {
        CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(
            m_lstTriggerIds[i]);
        if (pObject != NULL)
        {
            if (!pObjects->Contains(m_lstTriggerIds[i]))
            {
                if (pObject->AsSWSTrigger() != NULL)
                {
                    if (pObject->AsSWSTrigger()->LineSegmentIntersectTrigger(
                            vStart, vEnd))
                    {
                        bIntersecting = 1;
                        pObjects->Add(m_lstTriggerIds[i]);
                    }
                }
                else if (pObject->AsSWSAreaOfEffectObject() != NULL)
                {
                    if (pObject->AsSWSAreaOfEffectObject()->LineSegmentIntersectAreaOfEffect(
                            vStart, vEnd))
                    {
                        bIntersecting = 1;
                        pObjects->Add(m_lstTriggerIds[i]);
                    }
                }
                else if (pObject->AsSWSEncounter() != NULL)
                {
                    if (pObject->AsSWSEncounter()->LineSegmentIntersectActivateArea(
                            vStart, vEnd))
                    {
                        bIntersecting = 1;
                        pObjects->Add(m_lstTriggerIds[i]);
                    }
                }
            }
        }
    }
    return bIntersecting;
}
// PC 0x005004D0 GOG 0x004F85B0
int CSWSArea::InSubAreas(const Vector& vPosition,
                        CExoArrayList<unsigned long>* pObjects)
{
    int bInSubArea = 0;
    unsigned long oidInvalid = OBJECT_INVALID;
    for (int i = 0; i < m_lstTriggerIds.num; i++)
    {
        if (!pObjects->Contains(m_lstTriggerIds[i]))
        {
            CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(
                m_lstTriggerIds[i]);

            if (pObject == NULL)
            {
                oidInvalid = m_lstTriggerIds[i];
            }
            else if (pObject != NULL)
            {
                if (pObject->AsSWSTrigger() != NULL)
                {
                    if (pObject->AsSWSTrigger()->InTrigger(vPosition))
                    {
                        bInSubArea = 1;
                        pObjects->Add(m_lstTriggerIds[i]);
                    }
                }
                else if (pObject->AsSWSAreaOfEffectObject() != NULL)
                {
                    if (pObject->AsSWSAreaOfEffectObject()->InAreaOfEffect(vPosition))
                    {
                        bInSubArea = 1;
                        pObjects->Add(m_lstTriggerIds[i]);
                    }
                }
                else if (pObject->AsSWSEncounter() != NULL)
                {
                    if (pObject->AsSWSEncounter()->GetInActivateArea(vPosition))
                    {
                        bInSubArea = 1;
                        pObjects->Add(m_lstTriggerIds[i]);
                    }
                }
                if (pObject->AsSWSDoor() != NULL)
                {
                    if (pObject->AsSWSDoor()->InDoor(vPosition))
                    {
                        bInSubArea = 1;
                        pObjects->Add(m_lstTriggerIds[i]);
                    }
                }
            }
        }
    }
    if (oidInvalid != OBJECT_INVALID)
    {
        m_lstTriggerIds.Remove(oidInvalid);
    }
    return bInSubArea;
}
// PC 0x005006B0 GOG 0x004F8790
int CSWSArea::ShrinkLineByCreatures(const Vector& vStart, const Vector& vEnd,
                                  Vector& vResult)
{
    CExoArrayList<CSWSCreature*> lstCreatures;
    float fMinX = (vStart.m_fX < vEnd.m_fX ? vStart.m_fX : vEnd.m_fX) -
                  AREA_SHRINK_LINE_SEARCH_MARGIN;
    float fMaxX = (vStart.m_fX > vEnd.m_fX ? vStart.m_fX : vEnd.m_fX) +
                  AREA_SHRINK_LINE_SEARCH_MARGIN;
    float fMinY = (vStart.m_fY < vEnd.m_fY ? vStart.m_fY : vEnd.m_fY) -
                  AREA_SHRINK_LINE_SEARCH_MARGIN;
    float fMaxY = (vStart.m_fY > vEnd.m_fY ? vStart.m_fY : vEnd.m_fY) +
                  AREA_SHRINK_LINE_SEARCH_MARGIN;
    int nIndex;
    int nLast = m_lstObjectIds.num - 1;
    if (GetFirstObjectIndiceByX(&nIndex, fMaxX))
    {
        nLast = nIndex;
    }
    int nFirst = 0;
    if (GetFirstObjectIndiceByX(&nIndex, fMinX))
    {
        nFirst = nIndex;
    }
    for (; nFirst <= nLast; nFirst++)
    {
        CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
            m_lstObjectIds.element[nFirst]);
        if (pCreature != NULL)
        {
            Vector vFrom = vStart;
            Vector vPosition = pCreature->m_vPosition;
            Vector vDifference = vPosition - vFrom;
            vDifference.m_fZ = 0.0f;
            if (vPosition.m_fY >= fMinY && vPosition.m_fY <= fMaxY)
            {
                if (vDifference * vDifference >
                    pCreature->GetPathfindInformation()->GetStepScale() *
                    pCreature->GetPathfindInformation()->GetStepScale())
                {
                    lstCreatures.Add(pCreature);
                }
            }
        }
    }

    vResult = vEnd;
    if (lstCreatures.num == 0)
    {
        return 0;
    }
    int bShrunk = 0;
    int bRepeat;
    do
    {
        bRepeat = 0;
        for (int i = 0; i < lstCreatures.num; i++)
        {
            Vector vPosition = lstCreatures[i]->m_vPosition;
            Vector vEndpoint = vResult;
            float fRadius = lstCreatures[i]->GetPathfindInformation()->m_fStepScaleZ;
            Vector vDifference = vPosition - vEndpoint;
            vDifference.m_fZ = 0.0f;
            if (vDifference * vDifference < fRadius * fRadius)
            {
                Vector vFrom = vStart;
                Vector vTo = vResult;
                Vector vHit1;
                Vector vHit2;
                vFrom.m_fZ = 0.0f;
                vTo.m_fZ = 0.0f;
                Vector vDirection = vTo - vFrom;
                vDirection.m_fZ = 0.0f;
                Vector vBackoff = normalize(vDirection) * AREA_SHRINK_LINE_HIT_BACKOFF;
                Vector vCenter = lstCreatures[i]->m_vPosition;
                vCenter.m_fZ = 0.0f;
                int nHits = IntersectCircle2d(vCenter, fRadius, vFrom, vTo,
                                             vHit1, vHit2, true);
                if (nHits == 2)
                {
                    Vector vDistance1 = vStart - vHit1;
                    Vector vDistance2 = vStart - vHit2;
                    if (vDistance1 * vDistance1 < vDistance2 * vDistance2)
                    {
                        vResult = vHit1 - vBackoff;
                    }
                    else
                    {
                        vResult = vHit2 - vBackoff;
                    }
                    bShrunk = 1;
                    bRepeat = 1;
                    float fLengthSquared = (vFrom - vTo) * (vFrom - vTo);
                    if (fLengthSquared != 0.0f)
                    {
                        float fDistanceSquared = (vFrom - vResult) * (vFrom - vResult);
                        float fFraction = fDistanceSquared / fLengthSquared;
                        float fMinZ = vStart.m_fZ < vEnd.m_fZ ? vStart.m_fZ : vEnd.m_fZ;
                        float fMaxZ = vStart.m_fZ > vEnd.m_fZ ? vStart.m_fZ : vEnd.m_fZ;
                        vResult.m_fZ = (fMaxZ - fMinZ) * fFraction -
                                      AREA_SHRINK_LINE_HEIGHT_BIAS + fMinZ;
                    }
                }
                else if (nHits == 1)
                {
                    if (vResult.m_fX != vHit1.m_fX || vResult.m_fY != vHit1.m_fY)
                    {
                        vResult = vHit1 - vBackoff;
                        bShrunk = 1;
                        bRepeat = 1;
                    }
                    float fLengthSquared = (vFrom - vTo) * (vFrom - vTo);
                    if (bRepeat && fLengthSquared != 0.0f)
                    {
                        float fDistanceSquared = (vFrom - vResult) * (vFrom - vResult);
                        float fFraction = fDistanceSquared / fLengthSquared;
                        float fMinZ = vStart.m_fZ < vEnd.m_fZ ? vStart.m_fZ : vEnd.m_fZ;
                        float fMaxZ = vStart.m_fZ > vEnd.m_fZ ? vStart.m_fZ : vEnd.m_fZ;
                        vResult.m_fZ = (fMaxZ - fMinZ) * fFraction -
                                      AREA_SHRINK_LINE_HEIGHT_BIAS + fMinZ;
                    }
                }
            }
        }
    } while (bRepeat == 1);
    return bShrunk;
}
// PC 0x00500E80 GOG 0x004F8F60
int CSWSArea::SmoothCornerOptimize(int nPoints, float* pfPoints,
                                    int* pnResultCount, float** ppfResult)
{
    float fWidth = m_pPathfindInformation->m_fStepScale;
    float fHalfStep = m_pPathfindInformation->m_fHalfStep;
    *ppfResult = new float[4 * nPoints];
    *pnResultCount = 0;
    int nPoint = 0;
    (*ppfResult)[*pnResultCount] = pfPoints[nPoint];
    (*ppfResult)[*pnResultCount + 1] = pfPoints[nPoint + 1];
    *pnResultCount += 2;
    nPoint += 2;
    while (nPoint < nPoints - 2)
    {
        float fFirstX, fFirstY, fSecondX, fSecondY;
        if (ComputeBestCorner(
                (*ppfResult)[*pnResultCount - 2],
                (*ppfResult)[*pnResultCount - 1],
                pfPoints[nPoint], pfPoints[nPoint + 1],
                pfPoints[nPoint + 2], pfPoints[nPoint + 3],
                fWidth, fHalfStep, &fFirstX, &fFirstY,
                &fSecondX, &fSecondY) == 1)
        {
            float fLeftFirstX, fLeftFirstY, fLeftSecondX, fLeftSecondY;
            float fRightFirstX, fRightFirstY, fRightSecondX, fRightSecondY;
            if (ComputeBestCorner(
                    (*ppfResult)[*pnResultCount - 2],
                    (*ppfResult)[*pnResultCount - 1],
                    fFirstX, fFirstY, fSecondX, fSecondY,
                    fWidth, fHalfStep, &fLeftFirstX, &fLeftFirstY,
                    &fLeftSecondX, &fLeftSecondY) == 1)
            {
                if (ComputeBestCorner(
                        fLeftSecondX, fLeftSecondY, fSecondX, fSecondY,
                        pfPoints[nPoint + 2], pfPoints[nPoint + 3],
                        fWidth, fHalfStep, &fRightFirstX, &fRightFirstY,
                        &fRightSecondX, &fRightSecondY) == 1)
                {
                    (*ppfResult)[*pnResultCount] = fLeftFirstX;
                    (*ppfResult)[*pnResultCount + 1] = fLeftFirstY;
                    *pnResultCount += 2;
                    (*ppfResult)[*pnResultCount] = fLeftSecondX;
                    (*ppfResult)[*pnResultCount + 1] = fLeftSecondY;
                    *pnResultCount += 2;
                    (*ppfResult)[*pnResultCount] = fRightFirstX;
                    (*ppfResult)[*pnResultCount + 1] = fRightFirstY;
                    *pnResultCount += 2;
                    (*ppfResult)[*pnResultCount] = fRightSecondX;
                    (*ppfResult)[*pnResultCount + 1] = fRightSecondY;
                }
                else
                {
                    (*ppfResult)[*pnResultCount] = fLeftFirstX;
                    (*ppfResult)[*pnResultCount + 1] = fLeftFirstY;
                    *pnResultCount += 2;
                    (*ppfResult)[*pnResultCount] = fLeftSecondX;
                    (*ppfResult)[*pnResultCount + 1] = fLeftSecondY;
                    *pnResultCount += 2;
                    (*ppfResult)[*pnResultCount] = fSecondX;
                    (*ppfResult)[*pnResultCount + 1] = fSecondY;
                }
                *pnResultCount += 2;
                nPoint += 2;
            }
            else
            {
                if (ComputeBestCorner(
                        fFirstX, fFirstY, fSecondX, fSecondY,
                        pfPoints[nPoint + 2], pfPoints[nPoint + 3],
                        fWidth, fHalfStep, &fLeftFirstX, &fLeftFirstY,
                        &fLeftSecondX, &fLeftSecondY) == 1)
                {
                    (*ppfResult)[*pnResultCount] = fFirstX;
                    (*ppfResult)[*pnResultCount + 1] = fFirstY;
                    *pnResultCount += 2;
                    (*ppfResult)[*pnResultCount] = fLeftFirstX;
                    (*ppfResult)[*pnResultCount + 1] = fLeftFirstY;
                    *pnResultCount += 2;
                    (*ppfResult)[*pnResultCount] = fLeftSecondX;
                    (*ppfResult)[*pnResultCount + 1] = fLeftSecondY;
                }
                else
                {
                    (*ppfResult)[*pnResultCount] = fFirstX;
                    (*ppfResult)[*pnResultCount + 1] = fFirstY;
                    *pnResultCount += 2;
                    (*ppfResult)[*pnResultCount] = fSecondX;
                    (*ppfResult)[*pnResultCount + 1] = fSecondY;
                }
                *pnResultCount += 2;
                nPoint += 2;
            }
        }
        else
        {
            (*ppfResult)[*pnResultCount] = pfPoints[nPoint];
            (*ppfResult)[*pnResultCount + 1] = pfPoints[nPoint + 1];
            *pnResultCount += 2;
            nPoint += 2;
        }
    }
    if (nPoints <= 2)
    {
        return *pnResultCount;
    }
    (*ppfResult)[*pnResultCount] = pfPoints[nPoint];
    (*ppfResult)[*pnResultCount + 1] = pfPoints[nPoint + 1];
    *pnResultCount += 2;
}
// PC 0x005011E0 GOG 0x004F92C0
void CSWSArea::SmoothSelectNodes(int nPoints, float* pfPoints,
                                 int* pnResultCount, float** ppfResult,
                                 int bSubdivide, int bSelect)
{
    float fRadius = m_pPathfindInformation->m_fStepScale;
    float fHeight = m_pPathfindInformation->m_fHalfStep;
    int nDistance = (int)ComputePathDistance(nPoints, pfPoints);
    int nPending = -1;
    float* pfPending = NULL;
    EXOXBOXASSERT(nDistance <= 10000000);
    *pnResultCount = 0;
    *ppfResult = new float[nPoints + 2 * nDistance];
    int nNode = 0;
    (*ppfResult)[*pnResultCount] = pfPoints[2 * nNode];
    (*ppfResult)[*pnResultCount + 1] = pfPoints[2 * nNode + 1];
    *pnResultCount += 2;

    while (nNode < ((nPoints - 1) >> 1) || nPending > 0)
    {
        int nAdvance = 0;
        if (bSelect == 1)
        {
            int nTest = (nPoints >> 1) - nNode - 1;
            while (nTest >= 1)
            {
                Vector vEnd(pfPoints[2 * (nNode + nTest)],
                            pfPoints[2 * (nNode + nTest) + 1], 0.0f);
                Vector vStart((*ppfResult)[*pnResultCount - 2],
                              (*ppfResult)[*pnResultCount - 1], 0.0f);
                if (TestDirectLine(vStart, vEnd, fRadius, fHeight, 1, NULL) ==
                    DIRECT_LINE_CLEAR)
                {
                    nAdvance += nTest;
                    nTest = 0;
                }
                else if (nTest > AREA_SMOOTH_LINEAR_SEARCH_LIMIT)
                {
                    nTest >>= 1;
                    continue;
                }
                --nTest;
            }
        }
        else if (nPending <= 0)
        {
            nAdvance = 1;
        }

        nNode += nAdvance;
        float fX = pfPoints[2 * nNode];
        float fY = pfPoints[2 * nNode + 1];
        if (nAdvance == 0)
        {
            if (nPending > 0)
            {
                fX = pfPending[2 * (nPending - 1)];
                --nPending;
                fY = pfPending[2 * nPending + 1];
            }
            else
            {
                fX = pfPoints[2 * nNode + 2];
                ++nNode;
                fY = pfPoints[2 * nNode + 1];
            }
        }
        else
        {
            if (pfPending != NULL)
            {
                delete[] pfPending;
                pfPending = NULL;
            }
            nPending = 0;
        }

        if (bSubdivide)
        {
            Vector vDifference((*ppfResult)[*pnResultCount - 2] - fX,
                               (*ppfResult)[*pnResultCount - 1] - fY, 0.0f);
            float fDistance = magnitude(vDifference);
            if (fDistance >= AREA_SMOOTH_SUBDIVISION_THRESHOLD)
            {
                float fStartX = (*ppfResult)[*pnResultCount - 2];
                float fStartY = (*ppfResult)[*pnResultCount - 1];
                if (pfPending != NULL)
                {
                    delete[] pfPending;
                }
                pfPending = new float[2 * ((int)fDistance + 1)];
                nPending = 0;
                int nCoordinate = 0;
                float fRemaining = fDistance;
                while (fRemaining > 0.0)
                {
                    pfPending[nCoordinate] = fRemaining * (fX - fStartX) / fDistance + fStartX;
                    pfPending[nCoordinate + 1] = fRemaining * (fY - fStartY) / fDistance + fStartY;
                    nCoordinate += 2;
                    ++nPending;
                    fRemaining -= 1.0;
                }
                if (nPending > 0)
                {
                    fX = pfPending[2 * (nPending - 1)];
                    --nPending;
                    fY = pfPending[2 * nPending + 1];
                }
            }
        }
        (*ppfResult)[*pnResultCount] = fX;
        (*ppfResult)[*pnResultCount + 1] = fY;
        *pnResultCount += 2;
    }
    if (pfPending != NULL)
    {
        delete[] pfPending;
    }
}
// PC 0x00501540 GOG 0x004F9620
int CSWSArea::SmoothSelectNodesRestartable(int nPoints, float* pfPoints,
                                 int* pnResultCount, float** ppfResult,
                                 int bSubdivide, int bSelect, float fTolerance)
{
    unsigned __int64 nStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    float fRadius = m_pPathfindInformation->m_fStepScale;
    fRadius += AREA_SMOOTH_RESTART_RADIUS_MARGIN;
    float fHeight = m_pPathfindInformation->m_fHalfStep;
    int nDistance = (int)ComputePathDistance(nPoints, pfPoints);
    int nPending = m_pPathfindInformation->m_nSmoothIntermediateCount;
    float* pfPending = m_pPathfindInformation->m_pfSmoothIntermediatePoints;
    EXOXBOXASSERT(nDistance <= 10000000);
    int nNode = m_pPathfindInformation->m_nSmoothCurrentNode;
    if (*ppfResult == NULL)
    {
        *pnResultCount = 0;
        *ppfResult = new float[nPoints + 2 * nDistance];
        (*ppfResult)[*pnResultCount] = pfPoints[2 * nNode];
        (*ppfResult)[*pnResultCount + 1] = pfPoints[2 * nNode + 1];
        *pnResultCount += 2;
    }

    while (nNode < ((nPoints - 1) >> 1) || nPending > 0)
    {
        int nAdvance = 0;
        if (bSelect == 1)
        {
            int nTest = (nPoints >> 1) - nNode - 1;
            while (nTest >= 1)
            {
                Vector vStart((*ppfResult)[*pnResultCount - 2],
                              (*ppfResult)[*pnResultCount - 1], 0.0f);
                Vector vEnd(pfPoints[2 * (nNode + nTest)],
                            pfPoints[2 * (nNode + nTest) + 1], 0.0f);
                int nClear = TestDirectLine(vStart, vEnd, fRadius, fHeight, 1, NULL);
                bDumpData = 0;
                g_bRenderClipWalkable = 0;
                if (nClear == DIRECT_LINE_CLEAR)
                {
                    nAdvance += nTest;
                    nTest = 0;
                }
                else if (nTest > AREA_SMOOTH_LINEAR_SEARCH_LIMIT)
                {
                    nTest >>= 1;
                    continue;
                }
                --nTest;
            }
        }
        else if (nPending <= 0)
        {
            nAdvance = 1;
        }

        nNode += nAdvance;
        float fX = pfPoints[2 * nNode];
        float fY = pfPoints[2 * nNode + 1];
        if (nAdvance == 0)
        {
            if (nPending > 0)
            {
                fX = pfPending[2 * (nPending - 1)];
                --nPending;
                fY = pfPending[2 * nPending + 1];
            }
            else
            {
                fX = pfPoints[2 * nNode + 2];
                ++nNode;
                fY = pfPoints[2 * nNode + 1];
            }
        }
        else
        {
            if (pfPending != NULL)
            {
                delete[] pfPending;
                pfPending = NULL;
                m_pPathfindInformation->m_pfSmoothIntermediatePoints = NULL;
            }
            nPending = 0;
        }

        if (bSubdivide)
        {
            Vector vDifference((*ppfResult)[*pnResultCount - 2] - fX,
                               (*ppfResult)[*pnResultCount - 1] - fY, 0.0f);
            float fDistance = magnitude(vDifference);
            if (fDistance >= fTolerance)
            {
                float fStartX = (*ppfResult)[*pnResultCount - 2];
                float fStartY = (*ppfResult)[*pnResultCount - 1];
                if (pfPending != NULL)
                {
                    delete[] pfPending;
                    m_pPathfindInformation->m_pfSmoothIntermediatePoints = NULL;
                }
                pfPending = new float[2 * ((int)fDistance + 1)];
                m_pPathfindInformation->m_pfSmoothIntermediatePoints = pfPending;
                nPending = 0;
                int nCoordinate = 0;
                float fRemaining = fDistance;
                while (fRemaining > 0.0)
                {
                    pfPending[nCoordinate] = ((fX - fStartX) / fDistance) * fRemaining + fStartX;
                    pfPending[nCoordinate + 1] = ((fY - fStartY) / fDistance) * fRemaining + fStartY;
                    nCoordinate += 2;
                    ++nPending;
                    fRemaining -= 1.0;
                }
                if (nPending > 0)
                {
                    fX = pfPending[2 * (nPending - 1)];
                    --nPending;
                    fY = pfPending[2 * nPending + 1];
                }
            }
        }
        (*ppfResult)[*pnResultCount] = fX;
        (*ppfResult)[*pnResultCount + 1] = fY;
        *pnResultCount += 2;
        if (g_pExoBase->m_pExoTimers->GetHighResolutionTimer() - nStartTime >
            (unsigned __int64)SmoothTime)
        {
            m_pPathfindInformation->m_nSmoothIntermediateCount = nPending;
            m_pPathfindInformation->m_nSmoothCurrentNode = nNode;
            return 1;
        }
    }
    if (pfPending != NULL)
    {
        delete[] pfPending;
        m_pPathfindInformation->m_pfSmoothIntermediatePoints = NULL;
    }
    m_pPathfindInformation->m_nSmoothIntermediateCount = -1;
    m_pPathfindInformation->m_nSmoothCurrentNode = 0;
    return 0;
}
// PC 0x00501990 GOG 0x004F9A70
int CSWSArea::GridDFSearch(int nDepth, int nCost, int nRemainingCost,
                          int nX, int nY, int nZ)
{
    int* pPath = m_pPathfindInformation->m_pGridDFSPath;
    int nNextDepth = nDepth + 1;
    m_pPathfindInformation->m_nGridDFSDepth = nNextDepth;
    pPath[2 * nDepth] = nX;
    pPath[2 * nDepth + 1] = nY;

    if (m_pPathfindInformation->m_nGridDFSBestDistance > abs(nX) + abs(nY))
    {
        m_pPathfindInformation->m_nGridDFSBestDistance = abs(nX) + abs(nY);
        memcpy(m_pPathfindInformation->m_pGridDFSBestPath,
               m_pPathfindInformation->m_pGridDFSPath,
               (2 * nDepth + 2) * sizeof(int));
        m_pPathfindInformation->m_nGridDFSBestPathLength = nNextDepth;
    }

    if (nX == 0 && nY == 0 && nZ == 0)
        return 1;

    int nTolerance = m_pPathfindInformation->m_nStepTolerance;
    if (nX * nX + nY * nY + nZ * nZ <=
        nTolerance * nTolerance && nDepth > 0)
        return 1;

    if (nRemainingCost < abs(nX) + abs(nY) - nTolerance)
        return 0;

    int nStoredCost;
    int nStoredRemainingCost;
    if (GridDFSTransTableGet(nX, nY, &nStoredCost, &nStoredRemainingCost) &&
        (nCost > nStoredCost || nStoredRemainingCost >= nRemainingCost))
        return 0;

    if (++m_nGridSearchNodes >= MAX_GRID_DFS_SEARCH_NODES)
        m_bGridSearchAborted = 1;

    {
        CSWSAreaGridSuccessors* pSuccessors;
        int nSuccessors = GridDFSGenerateSuccessors(nCost, nRemainingCost,
                                                   nX, nY, nZ, &pSuccessors);
        int bFound = 0;
        CSWSAreaGridSuccessors* pSuccessor = pSuccessors;
        for (int i = 0; i < nSuccessors && !bFound && !m_bGridSearchAborted; i++, pSuccessor++)
        {
            bFound = GridDFSearch(nNextDepth, nCost + pSuccessor->m_nCost,
                                 nRemainingCost - pSuccessor->m_nCost,
                                 pSuccessor->m_nX, pSuccessor->m_nY,
                                 pSuccessor->m_nZ);
        }
        delete[] pSuccessors;
        GridDFSTransTablePut(nX, nY, nCost, nRemainingCost);
        return bFound;
    }
}
// PC 0x00501B90 GOG 0x004F9C70
int CSWSArea::PathPointDFSearch(int nDepth, float fCost, float fRemaining,
                               unsigned long nPoint, unsigned long nGoal)
{
    if (RenderDFS && bInitnRenderDEFLines &&
        m_pPathfindInformation->m_oidPathfindCreature == LockRenderDev)
    {
        memset(nRenderDEFLines, -1, sizeof(nRenderDEFLines));
        bInitnRenderDEFLines = 0;
        nRenderDFSCount = 0;
    }

    unsigned long* pPath = m_pPathfindInformation->m_pPathPointDFSPath;
    int nNextDepth = nDepth + 1;
    m_pPathfindInformation->m_nPathPointDFSDepth = nNextDepth;
    pPath[nDepth] = nPoint;
    Vector vPoint(m_pPathPoints[nPoint].m_fX, m_pPathPoints[nPoint].m_fY, 0.0f);
    double fDistance = magnitude(
        Vector(m_pPathPoints[nGoal].m_fX, m_pPathPoints[nGoal].m_fY, 0.0f) - vPoint);
    if (m_pPathfindInformation->m_fPathPointDFSBestDistance > fDistance)
    {
        m_pPathfindInformation->m_fPathPointDFSBestScore = fDistance;
        memcpy(m_pPathfindInformation->m_pPathPointDFSBestPath,
               m_pPathfindInformation->m_pPathPointDFSPath,
               (nDepth + 1) * sizeof(unsigned long));
        m_pPathfindInformation->m_nPathPointDFSBestPathLength = nNextDepth;
    }
    if (nPoint == nGoal)
        return 1;
    if (fRemaining < fDistance)
        return 0;

    float fOldCost, fOldRemaining;
    if (PathPointDFSTransTableGet(vPoint.m_fX, vPoint.m_fY,
                                 &fOldCost, &fOldRemaining))
    {
        if (fCost > fOldCost || fOldRemaining >= fRemaining)
            return 0;
    }
    if (nDepth > PATH_POINT_DFS_MAX_REVISIT_DEPTH)
    {
        m_bPathPointDFSAborted = 1;
        return 0;
    }
    if (++m_nPathPointDFSExpansions >= PATH_POINT_DFS_MAX_EXPANSIONS)
        m_bPathPointDFSAborted = 1;

    CSWSPathPointSuccessors* pSuccessors;
    int nSuccessors = PathPointDFSGenerateSuccessors(fCost, (int)fRemaining,
                                                    nPoint, &pSuccessors);
    int bFound = 0;
    PathPointDFSTransTablePut(vPoint.m_fX, vPoint.m_fY, fCost, fRemaining);
    for (int i = 0; i < nSuccessors && !bFound && !m_bPathPointDFSAborted; ++i)
    {
        CSWSPathPointSuccessors* pSuccessor = pSuccessors + i;
        float fStepCost = pSuccessor->m_fDistance;
        if (RenderDFS && m_pPathfindInformation->m_oidPathfindCreature == LockRenderDev)
        {
            Vector vNext(m_pPathPoints[pSuccessor->m_nPathPoint].m_fX,
                         m_pPathPoints[pSuccessor->m_nPathPoint].m_fY, 0.0f);
            Vector vSquare[4] = { vPoint, vPoint, vNext, vNext };
            vSquare[0].m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
                g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
                vSquare[0], 1);
            vSquare[1].m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
                g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
                vSquare[1], 1);
            vSquare[2].m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
                g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
                vSquare[2], 1);
            vSquare[3].m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
                g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
                vSquare[3], 1);
            nRenderDEFLines[nRenderDFSCount] = (AurAddDrawableSquare)((float*)vSquare, AUR_DRAWABLE_COLOR_WHITE,
                "nwsareapathfind.cpp", 7703);
            ++nRenderDFSCount;
        }
        bFound = PathPointDFSearch(nNextDepth, fCost + fStepCost,
                                   fRemaining - fStepCost,
                                   pSuccessor->m_nPathPoint, nGoal);
        if (RenderDFS && m_pPathfindInformation->m_oidPathfindCreature == LockRenderDev)
        {
            AurRemoveDrawableSquare(nRenderDEFLines[nRenderDFSCount]);
            nRenderDEFLines[nRenderDFSCount] = -1;
            --nRenderDFSCount;
        }
    }
    delete pSuccessors;
    return bFound;
}
// PC 0x00502020 GOG 0x004FA100
int CSWSArea::ComputeSafeLocationInDirection(const Vector& vPosition,
                                            const Vector& vDirection,
                                            float fDistance,
                                            CPathfindInformation* pPathfind,
                                            int nFlags, Vector* pvLocation)
{
    m_pPathfindInformation = pPathfind;
    float fStep = pPathfind->m_fStepScale;
    Vector vCandidate;
    vCandidate = vPosition + vDirection;
    vCandidate.m_fZ = ComputeHeight(vCandidate, 0, NULL);
    if (TestSafeLocationPoint(vCandidate, pPathfind) == 1)
    {
        *pvLocation = vCandidate;
        return 1;
    }

    unsigned long oidHit;
    if (PositionWalkable(vCandidate))
    {
        float fHeight = pPathfind->m_fHalfStep;
        float fProbeRadius = pPathfind->m_fStepScale;
        if (!NoNonWalkPolysInDoors(
            vCandidate + Vector(-AREA_SAFE_DIRECTION_PROBE_RADIUS,
                                -AREA_SAFE_DIRECTION_PROBE_RADIUS, 0.0f),
            vCandidate + Vector(AREA_SAFE_DIRECTION_PROBE_RADIUS,
                                AREA_SAFE_DIRECTION_PROBE_RADIUS, 0.0f),
            vCandidate.m_fZ, vCandidate.m_fZ + fHeight,
            fProbeRadius, OBJECT_INVALID, oidHit, NULL, 1, 0))
        {
            nFlags = 0;
        }
    }
    else
    {
        nFlags = 0;
    }

    Vector vForward = normalize(Vector(vDirection.m_fX, vDirection.m_fY, 0.0f));
    Vector vSide(vForward.m_fY, -vForward.m_fX, 0.0f);
    for (float fRadius = fStep; fRadius < fDistance; fRadius += fStep)
    {
        int nSteps = (int)((fRadius + AREA_SAFE_DIRECTION_STEP_ALLOWANCE) / fStep);
        for (int nSide = -nSteps; nSide <= nSteps; nSide++)
        {
            vCandidate = vPosition + vForward * (float)nSteps + vSide * (float)nSide;
            vCandidate.m_fZ = ComputeHeight(vCandidate, 0, NULL);
            if (TestSafeLocationPoint(vCandidate, pPathfind) == 1)
            {
                float fMinHeight;
                float fMaxHeight;
                if (vCandidate.m_fZ < vPosition.m_fZ)
                {
                    fMinHeight = vCandidate.m_fZ - AREA_SAFE_DIRECTION_HEIGHT_MARGIN;
                    fMaxHeight = pPathfind->m_fHalfStep;
                    fMaxHeight += vCandidate.m_fZ;
                }
                else
                {
                    fMinHeight = vPosition.m_fZ - AREA_SAFE_DIRECTION_HEIGHT_MARGIN;
                    fMaxHeight = pPathfind->m_fHalfStep;
                    fMaxHeight += vPosition.m_fZ;
                }
                if (nFlags == 0 ||
                    NoNonWalkPolysInDoors(vPosition, vCandidate, fMinHeight,
                        fMaxHeight, fStep, OBJECT_INVALID, oidHit, NULL, 1, 0) == 1)
                {
                    *pvLocation = vCandidate;
                    return 1;
                }
            }
        }
    }
    return ComputeSafeLocation(vPosition, fDistance, pPathfind, nFlags, pvLocation);
}
// PC 0x00502460 GOG 0x004FA540
int CSWSArea::SmoothSelectionRestartable(int nPoints, float* pfPoints,
                                         int* pnResultCount, float** ppfResult)
{
    int nFirstCount = m_pPathfindInformation->m_nSmoothFirstCount;
    float* pfFirst = m_pPathfindInformation->m_pfSmoothFirstPoints;
    int nSecondCount = m_pPathfindInformation->m_nSmoothSecondCount;
    float* pfSecond = m_pPathfindInformation->m_pfSmoothSecondPoints;

    if (m_pPathfindInformation->m_nSmoothingStep == CPathfindInformation::SMOOTHING_STEP_2)
    {
        int nResult = SmoothSelectNodesRestartable(nPoints, pfPoints,
            &nFirstCount, &pfFirst, 1, 0, 5.0f);
        m_pPathfindInformation->SetSmoothFirstPoints(pfFirst);
        m_pPathfindInformation->m_nSmoothFirstCount = nFirstCount;
        if (nResult == 1)
            return 1;
        m_pPathfindInformation->m_nSmoothIntermediateCount = -1;
        m_pPathfindInformation->m_nSmoothCurrentNode = 0;
        m_pPathfindInformation->m_nSmoothingStep = CPathfindInformation::SMOOTHING_STEP_3;
    }
    if (m_pPathfindInformation->m_nSmoothingStep == CPathfindInformation::SMOOTHING_STEP_3)
    {
        int nResult = SmoothSelectNodesRestartable(nFirstCount, pfFirst,
            &nSecondCount, &pfSecond, 1, 1, 1.1f);
        m_pPathfindInformation->SetSmoothSecondPoints(pfSecond);
        m_pPathfindInformation->m_nSmoothSecondCount = nSecondCount;
        if (nResult == 1)
            return 1;
        m_pPathfindInformation->m_nSmoothIntermediateCount = -1;
        m_pPathfindInformation->m_nSmoothCurrentNode = 0;
        m_pPathfindInformation->m_nSmoothingStep = CPathfindInformation::SMOOTHING_STEP_4;
    }
    if (m_pPathfindInformation->m_nSmoothingStep == CPathfindInformation::SMOOTHING_STEP_4)
    {
        if (SmoothSelectNodesRestartable(nSecondCount, pfSecond,
            pnResultCount, ppfResult, 0, 1, 1.1f) == 1)
            return 1;
    }
    m_pPathfindInformation->SetSmoothFirstPoints(NULL);
    m_pPathfindInformation->SetSmoothSecondPoints(NULL);
    return 0;
}
// PC 0x00502670 GOG 0x004FA750
void CSWSArea::SmoothSelection(int nPoints, float* pfPoints,
                               int* pnResultCount, float** ppfResult)
{
    float* pfFirst;
    float* pfSecond;
    {
        int nCount;
        float* pfResult;
        SmoothSelectNodes(nPoints, pfPoints, &nCount, &pfResult, 1, 0);
        nPoints = nCount;
        pfFirst = pfResult;
    }
    {
        int nCount;
        float* pfResult;
        SmoothSelectNodes(nPoints, pfFirst, &nCount, &pfResult, 1, 1);
        nPoints = nCount;
        pfSecond = pfResult;
    }
    SmoothSelectNodes(nPoints, pfSecond, pnResultCount, ppfResult, 0, 1);
    delete[] pfFirst;
    delete[] pfSecond;
}
// PC 0x005026F0 GOG 0x004FA7D0
int CSWSArea::SmoothPointsOnPath()
{
    if (m_pPathfindInformation->m_pWayPoints != NULL)
    {
        delete[] m_pPathfindInformation->m_pWayPoints;
        m_pPathfindInformation->m_pWayPoints = NULL;
        m_pPathfindInformation->m_nWayPointCount = 0;
    }

    int nPoints = m_pPathfindInformation->m_nField_94;
    float* pfPoints = m_pPathfindInformation->m_pWayPointsTemp;

    if (nPoints == 0)
    {
        m_pPathfindInformation->m_nWayPointCount = 0;
        m_pPathfindInformation->m_pWayPoints = NULL;
        return 1;
    }

    if (m_pPathfindInformation->m_nField_1C == 1)
    {
        float* pfCopy = new float[nPoints];
        for (int i = 0; i < nPoints; i++)
        {
            pfCopy[i] = pfPoints[i];
        }
        m_pPathfindInformation->m_pWayPoints = pfCopy;
        m_pPathfindInformation->m_nWayPointCount = nPoints;
        return 1;
    }

    int nCount;
    float* pfSmoothed;
    {
        int nA;
        float* pfA;
        SmoothSelection(nPoints, pfPoints, &nA, &pfA);
        nCount = nA;
        pfSmoothed = pfA;
    }
    {
        int nB;
        float* pfB;
        SmoothCornerOptimize(nCount, pfSmoothed, &nB, &pfB);
        m_pPathfindInformation->m_pWayPoints = pfB;
        m_pPathfindInformation->m_nWayPointCount = nB;
    }
    delete[] pfSmoothed;
    return 1;
}
// PC 0x00502820 GOG 0x004FA900
int CSWSArea::SmoothPointsOnPathRestartable()
{
    int nPoints;
    float* pfPoints;
    int nCount;
    int bRestart;
    float* pfSelection;
    {
        int nInputCount = m_pPathfindInformation->m_nField_94;
        float* pfInputPoints = m_pPathfindInformation->m_pWayPointsTemp;
        if (nInputCount == 0)
        {
            m_pPathfindInformation->m_nWayPointCount = 0;
            m_pPathfindInformation->m_pWayPoints = NULL;
            return 0;
        }

        if (m_pPathfindInformation->m_nSmoothingStep == CPathfindInformation::SMOOTHING_STEP_1)
        {
            if (m_pPathfindInformation->m_pWayPoints != NULL)
            {
                delete[] m_pPathfindInformation->m_pWayPoints;
                m_pPathfindInformation->m_pWayPoints = NULL;
                m_pPathfindInformation->m_nWayPointCount = 0;
            }
            if (m_pPathfindInformation->m_nField_1C == 1)
            {
                float* pfCopy = new float[nInputCount];
                for (int i = 0; i < nInputCount; i++)
                {
                    pfCopy[i] = pfInputPoints[i];
                }
                m_pPathfindInformation->m_pWayPoints = pfCopy;
                m_pPathfindInformation->m_nWayPointCount = nInputCount;
                return 0;
            }
            m_pPathfindInformation->m_nSmoothIntermediateCount = -1;
            m_pPathfindInformation->m_nSmoothCurrentNode = 0;
            m_pPathfindInformation->m_nSmoothingStep = CPathfindInformation::SMOOTHING_STEP_2;
            m_pPathfindInformation->SetSmoothFirstPoints(NULL);
            m_pPathfindInformation->SetSmoothSecondPoints(NULL);
            m_pPathfindInformation->SetSmoothFinalPoints(NULL);
        }

        nPoints = nInputCount;
        pfPoints = pfInputPoints;
    }
    {
        float* pfResult;
        {
            int nPreviousCount = m_pPathfindInformation->m_nSmoothFinalCount;
            float* pfPreviousPoints = m_pPathfindInformation->m_pfSmoothFinalPoints;
            int nSelectionCount = nPreviousCount;
            float* pfSelectionResult = pfPreviousPoints;
            pfResult = NULL;
            bRestart = SmoothSelectionRestartable(nPoints, pfPoints, &nSelectionCount, &pfSelectionResult);
            nCount = nSelectionCount;
            pfSelection = pfSelectionResult;
        }
        m_pPathfindInformation->m_nSmoothFinalCount = nCount;
        m_pPathfindInformation->SetSmoothFinalPoints(pfSelection);
        if (bRestart == 1)
        {
            return 1;
        }
        {
            int nResultCount;
            SmoothCornerOptimize(nCount, pfSelection, &nResultCount, &pfResult);
            m_pPathfindInformation->m_pWayPoints = pfResult;
            m_pPathfindInformation->m_nWayPointCount = nResultCount;
        }
        m_pPathfindInformation->m_nSmoothFinalCount = 0;
        m_pPathfindInformation->SetSmoothFinalPoints(NULL);
        return 0;
    }
}
// PC 0x00502A80 GOG 0x004FAB60
int CSWSArea::PlotGridPath(CPathfindInformation* pPathfind,
                         unsigned __int64 nTimeLimit)
{
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->m_nLastEvent[1] = 1;
    int nSearches = 0;
    unsigned __int64 nStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    m_pPathfindInformation = pPathfind;

    float fEndX = pPathfind->m_fEndX;
    float fEndY = pPathfind->m_fEndY;
    float fStartX = pPathfind->m_fStartX;
    float fStartY = pPathfind->m_fStartY;
    float fStep = (pPathfind->m_fStepScale);
    Vector vStart(fStartX, fStartY, 0.0f);
    Vector vEnd(fEndX, fEndY, 0.0f);
    if (fStep <= 0.1f)
        fStep = 0.1f;

    Vector vUp(0.0f, 0.0f, 1.0f);
    Vector vStep;
    vStep = fStep * normalize(vEnd - vStart);
    Vector vSide(vStep.m_fY, -vStep.m_fX, 0.0f);
    int nDistance = (int)(magnitude(vEnd - vStart) / fStep + 0.5);
    pPathfind->m_nField_120 = 0;
    pPathfind->m_nField_124 = nDistance;
    pPathfind->m_nField_128 = 0;
    pPathfind->m_vField_12C = vStart;
    pPathfind->m_vField_138 = vSide;
    pPathfind->m_vField_144 = vStep;
    pPathfind->m_vField_150 = vUp;
    pPathfind->m_nField_24 = 1;
    int nMaxDepth = nDistance + CPathfindInformation::GRID_SEARCH_DEPTH_ALLOWANCE;
    int* pGridPath = new int[2 * nMaxDepth + 4];
    pPathfind->m_pGridDFSPath = pGridPath;
    int* pBestPath = pPathfind->m_pGridDFSBestPath;
    if (pBestPath == NULL)
    {
        pBestPath = new int[2 * nMaxDepth + 4];
        pPathfind->m_nGridDFSBestPathLength = 0;
        pPathfind->m_pGridDFSBestPath = pBestPath;
    }

    int nDepth;
    Vector vEndDifference = Vector(fEndX, fEndY, 0.0f) - pPathfind->m_vGridStart;
    Vector vStartDifference = vStart - pPathfind->m_vGridEnd;
    if (vEndDifference * vEndDifference >= 0.0001f ||
        vStartDifference * vStartDifference >= 0.0001f)
    {
        if (pPathfind->m_pField_16C != NULL)
            m_pGridTransTable = (CSWSAreaGridTransEntry*)pPathfind->m_pField_16C;
        GridDFSTransTableInitialize();
        nDepth = nDistance + 2;
    }
    else
    {
        m_pGridTransTable = (CSWSAreaGridTransEntry*)pPathfind->m_pField_16C;
        nDepth = pPathfind->m_nField_168 + 2;
    }

    pPathfind->m_fField_164 = 10.0f;
    unsigned __int64 nElapsed = g_pExoBase->m_pExoTimers->GetHighResolutionTimer() - nStartTime;
    int nResult = CSWSObject::ACTION_IN_PROGRESS;
    if (pPathfind->m_nField_24 == 1 && pPathfind->m_nField_84 == 1)
    {
        pPathfind->HandlePathfindError(2, "we can not return a closest result if points are reversed.");
        nResult = CSWSObject::ACTION_FAILED;
    }
    m_nGridSearchNodes = 0;
    m_bGridSearchAborted = 0;
    if (pPathfind->m_nGridDFSBestDistance > nDistance + 1)
        pPathfind->m_nGridDFSBestDistance = nDistance + 1;
    int nBestDistance = pPathfind->m_nGridDFSBestDistance;
    while (nDepth < nMaxDepth && nResult == CSWSObject::ACTION_IN_PROGRESS &&
           nElapsed < nTimeLimit)
    {
        ++nSearches;
        nDepth += 2;
        m_nGridSearchNodes = 0;
        m_bGridSearchAborted = 0;
        if (GridDFSearch(0, 0, nDepth, 0, nDistance, 0) == 1)
            nResult = CSWSObject::ACTION_COMPLETE;
        nElapsed = g_pExoBase->m_pExoTimers->GetHighResolutionTimer() - nStartTime;
        if (pPathfind->m_nField_24 == 1)
        {
            int nBestLength = pPathfind->m_nGridDFSBestPathLength;
            if (nBestLength > 1 && nResult != CSWSObject::ACTION_COMPLETE && nSearches == 1 &&
                (nElapsed >= 4 * nTimeLimit ||
                 (nElapsed >= nTimeLimit && nBestDistance == pPathfind->m_nGridDFSBestDistance)))
            {
                if (m_bGridSearchAborted != 1)
                    nResult = CSWSObject::ACTION_COMPLETE;
                pPathfind->m_nGridDFSDepth = nBestLength;
                memcpy(pPathfind->m_pGridDFSPath, pPathfind->m_pGridDFSBestPath, 8 * nBestLength);
            }
        }
        nMaxDepth = nDistance + CPathfindInformation::GRID_SEARCH_DEPTH_ALLOWANCE;
    }
    pPathfind->m_fField_164 = 700.0f;
    nMaxDepth = nDistance + CPathfindInformation::GRID_SEARCH_DEPTH_ALLOWANCE;
    if (nResult != CSWSObject::ACTION_COMPLETE)
    {
        if (nDepth >= nMaxDepth ||
            (nSearches == 1 && (nElapsed >= 40 * nTimeLimit ||
             (nElapsed >= 10 * nTimeLimit && nBestDistance == pPathfind->m_nGridDFSBestDistance))))
        {
            if (nDepth >= nMaxDepth)
                pPathfind->HandlePathfindError(2, "Exceeded limits:Max search deapth");
            if (nSearches == 1)
            {
                if (nElapsed >= 40 * nTimeLimit)
                    pPathfind->HandlePathfindError(2, "Exceeded limits:Time limit");
                if (nElapsed >= 10 * nTimeLimit && nBestDistance == pPathfind->m_nGridDFSBestDistance)
                    pPathfind->HandlePathfindError(2, "Exceeded limits:Distance to goal");
            }
            nResult = CSWSObject::ACTION_FAILED;
        }
    }

    if (nResult == CSWSObject::ACTION_IN_PROGRESS)
    {
        pPathfind->m_vGridStart = Vector(fEndX, fEndY, 0.0f);
        pPathfind->m_vGridEnd = Vector(fStartX, fStartY, 0.0f);
        pPathfind->m_nField_168 = nDepth;
        pPathfind->m_pField_16C = (int*)m_pGridTransTable;
        m_pGridTransTable = NULL;
    }
    else
    {
        delete[] m_pGridTransTable;
        m_pGridTransTable = NULL;
        pPathfind->m_vGridStart = Vector(0.0f, 0.0f, 0.0f);
        pPathfind->m_vGridEnd = Vector(0.0f, 0.0f, 0.0f);
        pPathfind->m_nField_168 = 0;
        pPathfind->m_pField_16C = NULL;
    }
    if (nResult == CSWSObject::ACTION_COMPLETE)
    {
        int nLength = pPathfind->m_nGridDFSDepth;
        float* pfPoints = new float[2 * nLength];
        int* pPath = pPathfind->m_pGridDFSPath;
        pfPoints[0] = fEndX;
        pfPoints[1] = fEndY;
        for (int i = 1; i < nLength; ++i)
        {
            Vector vPoint = vStep * (float)pPath[2 * i + 1];
            Vector vPosition = vSide * (float)pPath[2 * i] + vStart + vPoint + vUp * 0;
            pfPoints[2 * i] = vPosition.m_fX;
            pfPoints[2 * i + 1] = vPosition.m_fY;
        }
        pPathfind->m_nField_94 = 2 * nLength;
        pPathfind->m_pWayPointsTemp = pfPoints;
    }
    if (nResult != CSWSObject::ACTION_IN_PROGRESS)
    {
        delete[] pBestPath;
        pPathfind->m_pGridDFSBestPath = NULL;
    }
    delete[] pGridPath;
    pPathfind->m_pGridDFSPath = NULL;
    if (nResult == CSWSObject::ACTION_COMPLETE)
        SmoothPointsOnPath();
    return nResult;
}
// PC 0x00503280 GOG 0x004FB360
int CSWSArea::PlotPathPointPath(CPathfindInformation* pPathfind,
                               unsigned __int64 nTimeLimit)
{
    Vector vStart(pPathfind->m_fEndX, pPathfind->m_fEndY, pPathfind->m_fEndZ);
    Vector vEnd(pPathfind->m_fStartX, pPathfind->m_fStartY, pPathfind->m_fStartZ);
    int nSearches = 0;
    unsigned __int64 nStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    unsigned long nStartPoint;
    unsigned long nEndPoint;
    int nResult;

    if (pPathfind->m_nField_1E0 == CPathfindInformation::PATH_POINT_STARTPOINTS)
    {
        int bStartSafe = TestSafeLocationPoint(vStart, pPathfind);
        int bEndSafe = TestSafeLocationPoint(vEnd, pPathfind);
        if (!bStartSafe || !bEndSafe)
        {
            if (!bStartSafe)
            {
                Vector vOldStart = vStart;
                ComputeSafeLocation(vStart, AREA_PATH_POINT_SAFE_RADIUS, pPathfind, 1, &vStart);
                if (!equal(vStart, vOldStart))
                {
                    unsigned long oidCreature = pPathfind->m_oidPathfindCreature;
                    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
                    if (pCreature != NULL)
                        pCreature->SetPosition(vStart, 1, 1, 0);
                }
                pPathfind->m_fEndX = vStart.m_fX;
                pPathfind->m_fEndY = vStart.m_fY;
                pPathfind->m_fEndZ = vStart.m_fZ;
                PositionWalkable(vStart);
            }
            if (!bEndSafe)
            {
                ComputeSafeLocation(vEnd, AREA_PATH_POINT_SAFE_RADIUS, pPathfind, 1, &vEnd);
                pPathfind->m_fStartX = vEnd.m_fX;
                pPathfind->m_fStartY = vEnd.m_fY;
                pPathfind->m_fStartZ = vEnd.m_fZ;
                PositionWalkable(vEnd);
            }
        }
        if (!ClosestPathPoint(vStart, nStartPoint) || !ClosestPathPoint(vEnd, nEndPoint))
        {
            pPathfind->HandlePathfindError(2, "Error could not find closest path node to start or end");
            pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_STARTPOINTS;
            return CSWSObject::ACTION_FAILED;
        }
        pPathfind->m_nField_1A4 = nStartPoint;
        pPathfind->m_nField_1A8 = nEndPoint;
        pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_PATHFIND;
        if (g_pExoBase->m_pExoTimers->GetHighResolutionTimer() - nStartTime > nTimeLimit)
            return CSWSObject::ACTION_IN_PROGRESS;
    }
    else
    {
        nStartPoint = pPathfind->m_nField_1A4;
        nEndPoint = pPathfind->m_nField_1A8;
    }

    if (nStartPoint == nEndPoint && pPathfind->m_nField_1E0 == CPathfindInformation::PATH_POINT_PATHFIND)
    {
        pPathfind->m_nPathPointDFSDepth = 1;
        unsigned long* pPath = new unsigned long[1];
        pPath[0] = nStartPoint;
        pPathfind->SetPathPointPath(pPath);
        pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_GRIDSTART;
    }

    if (pPathfind->m_nField_1E0 == CPathfindInformation::PATH_POINT_PATHFIND)
    {
        pPathfind->m_nField_1A4 = nStartPoint;
        pPathfind->m_nField_1A8 = nEndPoint;
        float fDistance = magnitude(vEnd - vStart);
        m_nPathPointPathCapacity = m_nPathPointCount;
        pPathfind->SetPathPointPath(new unsigned long[m_nPathPointCount]);
        unsigned long* pBestPath = pPathfind->m_pPathPointDFSBestPath;
        if (pBestPath == NULL)
        {
            pBestPath = new unsigned long[m_nPathPointCount];
            pPathfind->m_pPathPointDFSBestPath = pBestPath;
        }
        Vector vStartChange = vStart - pPathfind->m_vPathPointStart;
        Vector vEndChange = vEnd - pPathfind->m_vPathPointEnd;
        float fIncrement = fDistance * AREA_PATH_POINT_SEARCH_BOUND_FRACTION;
        float fBound;
        if (vStartChange * vStartChange >= AREA_PATH_POINT_RESTART_TOLERANCE_SQUARED ||
            vEndChange * vEndChange >= AREA_PATH_POINT_RESTART_TOLERANCE_SQUARED)
        {
            if (pPathfind->m_pPathPointTransTable != 0)
                m_pPathPointTransTable = (CSWSAreaPathPointTransEntry*)pPathfind->m_pPathPointTransTable;
            PathPointDFSTransTableInitialize();
            fBound = fDistance * AREA_PATH_POINT_SEARCH_BOUND_FRACTION + AREA_PATH_POINT_INITIAL_SEARCH_BOUND;
            pPathfind->m_fPathPointDFSBestScore = fBound;
        }
        else
        {
            m_pPathPointTransTable = (CSWSAreaPathPointTransEntry*)pPathfind->m_pPathPointTransTable;
            if (m_pPathPointTransTable == NULL)
                PathPointDFSTransTableInitialize();
            fBound = pPathfind->m_fPathPointSearchBound;
            fBound += fIncrement;
        }
        unsigned __int64 nElapsed = g_pExoBase->m_pExoTimers->GetHighResolutionTimer() - nStartTime;
        nResult = CSWSObject::ACTION_IN_PROGRESS;
        m_nPathPointDFSExpansions = 0;
        m_bPathPointDFSAborted = 0;
        if (pPathfind->m_fPathPointDFSBestDistance > fDistance)
            pPathfind->m_fPathPointDFSBestDistance = fDistance;

        while (nSearches == 0 || (nResult == CSWSObject::ACTION_IN_PROGRESS && nElapsed < nTimeLimit))
        {
            nSearches++;
            fBound += fIncrement;
            m_nPathPointDFSExpansions = 0;
            m_bPathPointDFSAborted = 0;
            m_pActivePathfindInformation = pPathfind;
            if (PathPointDFSearch(0, 0.0f, fBound, nStartPoint, nEndPoint) == 1)
                nResult = CSWSObject::ACTION_COMPLETE;
            if (m_bPathPointDFSAborted)
            {
                m_pActivePathfindInformation->HandlePathfindError(0, "we exceeded 4096 levels of searching.. ");
                return CSWSObject::ACTION_FAILED;
            }
            m_pActivePathfindInformation = NULL;
            nElapsed = g_pExoBase->m_pExoTimers->GetHighResolutionTimer() - nStartTime;
        }
        if (nResult == CSWSObject::ACTION_IN_PROGRESS)
        {
            pPathfind->m_vPathPointStart = vStart;
            pPathfind->m_vPathPointEnd = vEnd;
            pPathfind->m_fPathPointSearchBound = fBound;
            pPathfind->m_pPathPointTransTable = m_pPathPointTransTable;
            m_pPathPointTransTable = NULL;
        }
        else
        {
            delete[] m_pPathPointTransTable;
            m_pPathPointTransTable = NULL;
            pPathfind->m_vPathPointStart = Vector(0.0f, 0.0f, 0.0f);
            pPathfind->m_vPathPointEnd = Vector(0.0f, 0.0f, 0.0f);
            pPathfind->m_fPathPointSearchBound = 0.0f;
            pPathfind->m_pPathPointTransTable = 0;
        }
        if (nResult != CSWSObject::ACTION_IN_PROGRESS)
        {
            delete[] pBestPath;
            pPathfind->m_pPathPointDFSBestPath = NULL;
        }
        if (nResult == CSWSObject::ACTION_COMPLETE)
        {
            pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_GRIDSTART;
            return CSWSObject::ACTION_IN_PROGRESS;
        }
    }

    if (pPathfind->m_nField_1E0 == CPathfindInformation::PATH_POINT_GRIDSTART)
    {
        Vector vPoint(m_pPathPoints[nStartPoint].m_fX, m_pPathPoints[nStartPoint].m_fY, 0.0f);
        int nGridResult = CSWSObject::ACTION_COMPLETE;
        pPathfind->m_nField_210 = nStartPoint != nEndPoint;
        float fRadius = pPathfind->m_fStepScale;
        float fHeight = pPathfind->m_fHalfStep;
        if (TestDirectLine(vStart, vPoint, fRadius, fHeight, 1, NULL) != 1)
        {
            pPathfind->SetPathPointEndpoints(vStart, vEnd);
            pPathfind->SetStartX(m_pPathPoints[nStartPoint].m_fX);
            pPathfind->SetStartY(m_pPathPoints[nStartPoint].m_fY);
            pPathfind->m_nField_1C = 1;
            nGridResult = PlotGridPath(pPathfind, nTimeLimit);
            pPathfind->m_nField_1C = 0;
            pPathfind->GetPathPointEndpoints(vStart, vEnd);
            pPathfind->m_fStartX = vEnd.m_fX;
            pPathfind->m_fStartY = vEnd.m_fY;
            if (nGridResult == CSWSObject::ACTION_COMPLETE)
            {
                pPathfind->SetPathPointStartPath(m_pPathfindInformation->m_nField_94,
                                                m_pPathfindInformation->m_pWayPointsTemp);
                m_pPathfindInformation->m_nField_94 = 0;
                m_pPathfindInformation->m_pWayPointsTemp = NULL;
            }
        }
        else
        {
            float* pfPoints = new float[4];
            pfPoints[0] = vStart.m_fX;
            pfPoints[1] = vStart.m_fY;
            pfPoints[2] = m_pPathPoints[nStartPoint].m_fX;
            pfPoints[3] = m_pPathPoints[nStartPoint].m_fY;
            pPathfind->SetPathPointStartPath(4, pfPoints);
        }
        pPathfind->m_nField_210 = 0;
        if (nGridResult == CSWSObject::ACTION_COMPLETE)
        {
            unsigned __int64 nNow = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
            pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_GRIDEND;
            if (nNow - nStartTime > nTimeLimit)
                return CSWSObject::ACTION_IN_PROGRESS;
        }
        if (nGridResult == CSWSObject::ACTION_FAILED)
        {
            int nCount = pPathfind->m_nPathPointDFSDepth;
            if (nCount > 1)
            {
                unsigned long* pPath = pPathfind->m_pPathPointDFSPath;
                for (int i = 1; i < nCount; i++)
                    pPath[i - 1] = pPath[i];
                pPathfind->m_nPathPointDFSDepth = nCount - 1;
                pPathfind->m_nField_1A4 = pPath[0];
                pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_GRIDSTART;
                return CSWSObject::ACTION_IN_PROGRESS;
            }
            pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_STARTPOINTS;
            pPathfind->HandlePathfindError(0, "    failed to grid based pathfind from the creatures position to the starting path point.");
            return CSWSObject::ACTION_FAILED;
        }
        if (nGridResult == CSWSObject::ACTION_IN_PROGRESS)
            return CSWSObject::ACTION_IN_PROGRESS;
    }

    if (pPathfind->m_nField_1E0 == CPathfindInformation::PATH_POINT_GRIDEND)
    {
        Vector vPoint(m_pPathPoints[nEndPoint].m_fX, m_pPathPoints[nEndPoint].m_fY, 0.0f);
        int nGridResult = CSWSObject::ACTION_COMPLETE;
        pPathfind->m_nField_210 = nStartPoint != nEndPoint;
        float fRadius = pPathfind->m_fStepScale;
        float fHeight = pPathfind->m_fHalfStep;
        int nLineResult = TestDirectLine(vEnd, vPoint, fRadius, fHeight, 1, NULL);
        if (nLineResult != 1 && nLineResult != DIRECT_LINE_BLOCKED_CREATURE)
        {
            pPathfind->SetPathPointEndpoints(vStart, vEnd);
            pPathfind->SetEndX(m_pPathPoints[nEndPoint].m_fX);
            pPathfind->SetEndY(m_pPathPoints[nEndPoint].m_fY);
            pPathfind->m_nField_1C = 1;
            nGridResult = PlotGridPath(pPathfind, nTimeLimit);
            pPathfind->m_nField_1C = 0;
            pPathfind->GetPathPointEndpoints(vStart, vEnd);
            pPathfind->m_fEndX = vStart.m_fX;
            pPathfind->m_fEndY = vStart.m_fY;
            if (nGridResult == CSWSObject::ACTION_COMPLETE)
            {
                pPathfind->SetPathPointEndPath(m_pPathfindInformation->m_nField_94,
                                              m_pPathfindInformation->m_pWayPointsTemp);
                m_pPathfindInformation->m_nField_94 = 0;
                m_pPathfindInformation->m_pWayPointsTemp = NULL;
            }
        }
        else
        {
            float* pfPoints = new float[4];
            pfPoints[0] = m_pPathPoints[nEndPoint].m_fX;
            pfPoints[1] = m_pPathPoints[nEndPoint].m_fY;
            pfPoints[2] = vEnd.m_fX;
            pfPoints[3] = vEnd.m_fY;
            pPathfind->SetPathPointEndPath(4, pfPoints);
        }
        pPathfind->m_nField_210 = 0;
        if (nGridResult == CSWSObject::ACTION_COMPLETE)
        {
            unsigned __int64 nNow = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
            pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_SMOOTH;
            pPathfind->m_nSmoothingStep = CPathfindInformation::SMOOTHING_START;
            if (nNow - nStartTime > nTimeLimit)
                return CSWSObject::ACTION_IN_PROGRESS;
        }
        if (nGridResult == CSWSObject::ACTION_FAILED)
        {
            int nCount = pPathfind->m_nPathPointDFSDepth;
            if (nCount > 1)
            {
                unsigned long* pPath = pPathfind->m_pPathPointDFSPath;
                --nCount;
                pPathfind->m_nPathPointDFSDepth = nCount;
                pPathfind->m_nField_1A8 = pPath[nCount - 1];
                pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_GRIDEND;
                return CSWSObject::ACTION_IN_PROGRESS;
            }
            pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_STARTPOINTS;
            pPathfind->HandlePathfindError(0, "    failed to grid based pathfind from the ending path point ot the destiantion.");
            return CSWSObject::ACTION_FAILED;
        }
        if (nGridResult == CSWSObject::ACTION_IN_PROGRESS)
            return CSWSObject::ACTION_IN_PROGRESS;
    }

    if (pPathfind->m_nField_1E0 == CPathfindInformation::PATH_POINT_SMOOTH)
    {
        if (pPathfind->m_nSmoothingStep == CPathfindInformation::SMOOTHING_START)
        {
            nStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
            int nPathCount = pPathfind->m_nPathPointDFSDepth;
            unsigned long* pPath = pPathfind->m_pPathPointDFSPath;
            int nStartCount = pPathfind->m_nPathPointStartCount;
            int nEndCount = pPathfind->m_nPathPointEndCount;
            float* pfStart = pPathfind->m_pfPathPointStartPoints;
            float* pfEnd = pPathfind->m_pfPathPointEndPoints;
            int nCount = nEndCount + 2 * nPathCount - 4 + nStartCount;
            float* pfPoints = new float[nCount];

            memset(pfPoints, nCount, sizeof(float));
            int nIndex = 0;
            for (int i = 0; i < nStartCount - 2; i++)
                pfPoints[nIndex++] = pfStart[i];
            for (int j = 0; j < nPathCount; j++)
            {
                pfPoints[nIndex++] = m_pPathPoints[pPath[j]].m_fX;
                pfPoints[nIndex++] = m_pPathPoints[pPath[j]].m_fY;
            }
            float*& pfEndRef = pfEnd;
            for (int k = 2; k < nEndCount; k++)
                pfPoints[nIndex++] = pfEndRef[k];
            pPathfind->m_nPathPointStartCount = 0;
            if (pPathfind->m_pfPathPointStartPoints != NULL)
                delete[] pPathfind->m_pfPathPointStartPoints;
            pPathfind->m_pfPathPointStartPoints = NULL;
            pPathfind->m_nPathPointEndCount = 0;
            if (pPathfind->m_pfPathPointEndPoints != NULL)
                delete[] pPathfind->m_pfPathPointEndPoints;
            pPathfind->m_pfPathPointEndPoints = NULL;
            pPathfind->m_nField_94 = nCount;
            pPathfind->m_pWayPointsTemp = pfPoints;
            pPathfind->SetPathPointPath(NULL);
            pPathfind->m_nSmoothingStep = CPathfindInformation::SMOOTHING_STEP_1;
            unsigned __int64 nNow = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
            if (nNow - nStartTime > (unsigned __int64)SmoothTime)
                return 1;
        }
        if (!SmoothPointsOnPathRestartable())
        {
            pPathfind->m_nField_1E0 = CPathfindInformation::PATH_POINT_STARTPOINTS;
            return CSWSObject::ACTION_COMPLETE;
        }
        return CSWSObject::ACTION_IN_PROGRESS;
    }
    return nResult;
}
// PC 0x004FD1B0 GOG 0x004F5290
int CSWSArea::NoCreaturesOnLine(const Vector& vStart, const Vector& vEnd,
                               CPathfindInformation* pPathfind, int nFlags,
                               int bBumpCreatures, unsigned long* poidOverlap,
                               int bCheckSize, CWalkHitInfo* pWalkHitInfo)
{
    if (equal(vEnd, vStart))
    {
        return 1;
    }
    if (pPathfind == NULL)
    {
        return 0;
    }

    float fPersonalSpace = pPathfind->GetCreaturePersonalSpace() +
                           AREA_CREATURE_LINE_RADIUS_MARGIN;
    unsigned long oidCreature = pPathfind->m_oidPathfindCreature;
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
        oidCreature);
    if (pCreature == NULL)
    {
        bBumpCreatures = 0;
    }
    unsigned long oidTarget = pPathfind->m_oidPathfindTargetCreature;
    unsigned long oidIgnore = pPathfind->m_nField_34;
    float fDeltaY = vEnd.m_fY - vStart.m_fY;
    float fDeltaX = vEnd.m_fX - vStart.m_fX;
    float fMaxX = vEnd.m_fX > vStart.m_fX ? vEnd.m_fX : vStart.m_fX;
    float fMinX = vEnd.m_fX < vStart.m_fX ? vEnd.m_fX : vStart.m_fX;
    float fMaxY = vEnd.m_fY > vStart.m_fY ? vEnd.m_fY : vStart.m_fY;
    float fMinY = vEnd.m_fY < vStart.m_fY ? vEnd.m_fY : vStart.m_fY;
    float fSlope;
    float fIntercept;
    float fScale;
    if (fDeltaX == 0.0f)
    {
        fSlope = 0.0f;
        fIntercept = 0.0f;
    }
    else
    {
        fSlope = fDeltaY / fDeltaX;
        fIntercept = vStart.m_fY - fSlope * vStart.m_fX;
        fScale = (float)sqrt(1.0f / (fSlope * fSlope + 1.0f));
    }

    int nIndex;
    int nLast = m_lstObjectIds.num - 1;
    if (GetFirstObjectIndiceByX(&nIndex, fMaxX + AREA_CREATURE_LINE_SEARCH_MARGIN))
    {
        nLast = nIndex;
    }
    int nFirst = 0;
    if (GetFirstObjectIndiceByX(&nIndex, fMinX - AREA_CREATURE_LINE_SEARCH_MARGIN))
    {
        nFirst = nIndex;
    }
    float fMaxYBound = fMaxY + AREA_CREATURE_LINE_SEARCH_MARGIN;
    float fMinYBound = fMinY - AREA_CREATURE_LINE_SEARCH_MARGIN;
    for (; nFirst <= nLast; nFirst++)
    {
        unsigned long oidOther = m_lstObjectIds[nFirst];
        CSWSCreature* pOther = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
            oidOther);
        if (pOther != NULL && oidOther != oidCreature && oidOther != oidTarget &&
            oidOther != oidIgnore && !pOther->GetDead() && !pOther->GetDeadTemp() &&
            (!bBumpCreatures || !pCreature->GetIsCreatureBumpable(pOther)) &&
            !pOther->m_c26A && !pPathfind->m_nField_210)
        {
            float fRadius = pOther->m_pPathfindInformation->GetCreaturePersonalSpace() +
                            fPersonalSpace;
            Vector vCenter = pOther->m_vPosition;
            vCenter.m_fZ = 0.0f;
            if (vCenter.m_fY > fMaxYBound || vCenter.m_fY < fMinYBound)
                continue;
            if (fDeltaX == 0.0f)
            {
                if (vCenter.m_fY <= fMinY)
                {
                    float fX = fMinX - vCenter.m_fX;
                    float fY = fMinY - vCenter.m_fY;
                    if (fY * fY + fX * fX <= fRadius * fRadius &&
                        !EvaluateOverlappingTargets(pPathfind, vCenter, oidOther,
                                                     vStart, fRadius, nFlags,
                                                     bCheckSize, poidOverlap))
                    {
                        return 0;
                    }
                }
                else if (vCenter.m_fY >= fMaxY)
                {
                    float fX = fMaxX - vCenter.m_fX;
                    float fY = fMaxY - vCenter.m_fY;
                    if (fY * fY + fX * fX <= fRadius * fRadius &&
                        !EvaluateOverlappingTargets(pPathfind, vCenter, oidOther,
                                                     vStart, fRadius, nFlags,
                                                     bCheckSize, poidOverlap))
                    {
                        return 0;
                    }
                }
                else if (fMinX - fRadius <= vCenter.m_fX &&
                         fRadius + fMaxX >= vCenter.m_fX &&
                         !EvaluateOverlappingTargets(pPathfind, vCenter, oidOther,
                                                      vStart, fRadius, nFlags,
                                                      bCheckSize, poidOverlap))
                {
                    return 0;
                }
            }
            else
            {
                float fLineY = vCenter.m_fX * fSlope + fIntercept;
                float fDistance = (float)fabs(vCenter.m_fY - fLineY) * fScale;
                if (fDistance < fRadius)
                {
                    float fSide = 1.0f;
                    if (vCenter.m_fY < fLineY)
                    {
                        fSide = -1.0f;
                    }
                    float fClosestX = fSide * fDistance * fScale * fSlope + vCenter.m_fX;
                    float fHalfWidth = (float)sqrt(fRadius * fRadius - fDistance * fDistance) * fScale;
                    if ((fClosestX - fHalfWidth >= fMinX || fHalfWidth + fClosestX >= fMinX) &&
                        (fClosestX - fHalfWidth <= fMaxX || fHalfWidth + fClosestX <= fMaxX) &&
                        !EvaluateOverlappingTargets(pPathfind, vCenter, oidOther,
                                                     vStart, fRadius, nFlags,
                                                     bCheckSize, poidOverlap))
                    {
                        SetHitInfoForCreature(pOther->m_vPosition, fRadius, vStart,
                                               vEnd, pWalkHitInfo);
                        if (poidOverlap != NULL)
                        {
                            *poidOverlap = oidOther;
                        }
                        if (pWalkHitInfo != NULL)
                        {
                            pWalkHitInfo->m_oidBlocker = oidOther;
                        }
                        return 0;
                    }
                }
            }
        }
    }
    if (poidOverlap != NULL)
    {
        *poidOverlap = OBJECT_INVALID;
    }
    return 1;
}

#ifdef _XBOX

int CSWSArea::CountVisibleToPlayers(const Vector& vPosition, int nMaxCount)
{
    unsigned long oidObject;
    int nCount = 0;
    int nLimit = nMaxCount;

    if (GetFirstObjectInArea(oidObject) == 1)
    {
        Vector vStart = vPosition;
        do
        {
            if (nCount >= nLimit)
            {
                break;
            }
            CGameObject* pObject;
            CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
            if (pExoApp->GetObjectArray()->GetGameObject(oidObject, &pObject) ==
                    CGameObjectArray::SUCCESS &&
                pObject->AsSWSCreature() != NULL)
            {
                unsigned long oidOwner = m_nId;
                CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                if (pServer->GetClientObjectByObjectId(oidOwner) != NULL)
                {
                    Vector vEnd = pObject->AsSWSCreature()->m_vPosition;
                    float fX = vStart.m_fX - vEnd.m_fX;
                    float fY = vStart.m_fY - vEnd.m_fY;
                    float fZ = vStart.m_fZ - vEnd.m_fZ;
                    if (fX * fX + fY * fY + fZ * fZ <= 900.0f)
                    {
                        Vector vHit;
                        vStart.m_fZ += 1.5f;
                        vEnd.m_fZ += 1.5f;
                        unsigned long oidHit;
                        if (ClearLineOfSight(vStart, vEnd, &vHit, &oidHit,
                                             OBJECT_INVALID, OBJECT_INVALID, 0, 0) == 1)
                        {
                            nCount++;
                        }
                    }
                }
            }
        } while (GetNextObjectInArea(oidObject) == 1);
        return nCount;
    }
    return nCount;
}
#endif
