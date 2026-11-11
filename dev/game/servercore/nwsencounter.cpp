#include "exobase.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "serveraimaster.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "nwscript.h"
#include "nwsscriptevent.h"
#include "worldtimer.h"
#include "nwarea.h"
#include "nwsarea.h"
#include "nwsmodule.h"
#include "partytables.h"
#include <math.h>
#include "nwsencounter.h"

#include "factionmanager.h"

static inline void NwsencounterTrace1();
#define NWSENCOUNTER_TRACE() NwsencounterTrace1()
// PC 0x0056B6C0
CEncounterSpawnPoint::CEncounterSpawnPoint()
    : m_fOrientation(0.0f)
{
}
// PC 0x0056B6D0
int CSWSEncounter::LineSegmentIntersectActivateArea(const Vector& vStart,
                                                   const Vector& vEnd)
{
    CSWSArea* pArea = GetArea();
    int nIndex;
    float fX;
    float fY;

    if (pArea != NULL)
    {
        for (nIndex = 0; nIndex < m_nActivateAreaVertexCount; nIndex++)
        {
            int nFirst = m_pActivateAreaIndices[nIndex];
            int nSecond = m_pActivateAreaIndices[
                nIndex != m_nActivateAreaVertexCount - 1 ? nIndex + 1 : 0];

            if (pArea->IntersectLineSegments(
                    vStart.m_fX, vStart.m_fY, vEnd.m_fX, vEnd.m_fY,
                    m_pActivateAreaVertices[nFirst].m_fX,
                    m_pActivateAreaVertices[nFirst].m_fY,
                    m_pActivateAreaVertices[nSecond].m_fX,
                    m_pActivateAreaVertices[nSecond].m_fY, &fX, &fY) == 1)
            {
                return 1;
            }
        }
    }
    return 0;
}
#ifndef _XBOX
int CSWSEncounter::GetInActivateArea(const Vector& vPosition)  // PC 0x0056B790
{
    int nIntersections = 0;
    int nVertexCount = m_nActivateAreaVertexCount;

    for (int nIndex = 0; nIndex < nVertexCount; nIndex++)
    {
        int nFirst = m_pActivateAreaIndices[nIndex];
        int nSecond = m_pActivateAreaIndices[
            nIndex != nVertexCount - 1 ? nIndex + 1 : 0];
        Vector* pSecond = &m_pActivateAreaVertices[nSecond];
        Vector* pFirst = &m_pActivateAreaVertices[nFirst];

        if (pSecond->m_fX != pFirst->m_fX)
        {
            float fY = pFirst->m_fY;
            float fX = vPosition.m_fX;
            float fDeltaY = pSecond->m_fY - fY;
            float fDeltaX = pSecond->m_fX - pFirst->m_fX;
            fY = fY -
                (pFirst->m_fX - vPosition.m_fX) * (fDeltaY / fDeltaX);

            if (fY > vPosition.m_fY)
            {
                if (fabs(fDeltaY) > fabs(fDeltaX))
                {
                    if (pSecond->m_fY > pFirst->m_fY)
                    {
                        if (fY >= pFirst->m_fY && fY < pSecond->m_fY)
                            nIntersections++;
                    }
                    else if (pFirst->m_fY > pSecond->m_fY &&
                             fY > pSecond->m_fY && fY <= pFirst->m_fY)
                    {
                        nIntersections++;
                    }
                }
                else
                {
                    if (pSecond->m_fX > pFirst->m_fX)
                    {
                        if (fX >= pFirst->m_fX && fX < pSecond->m_fX)
                            nIntersections++;
                    }
                    else if (pFirst->m_fX > pSecond->m_fX &&
                             fX > pSecond->m_fX && fX <= pFirst->m_fX)
                    {
                        nIntersections++;
                    }
                }
            }
        }

        if (pSecond->m_fX == vPosition.m_fX &&
            (pSecond->m_fY > vPosition.m_fY ||
             pFirst->m_fY >= vPosition.m_fY))
        {
            nIntersections++;
        }
    }

    return nIntersections % 2 == 1;
}
#endif
CEncounterListEntry& CEncounterListEntry::operator=(
    const CEncounterListEntry& other)
{
    m_cResRef = other.m_cResRef;
    m_fChallengeRating = other.m_fChallengeRating;
    m_fSpawnPoolCost = other.m_fSpawnPoolCost;
    m_bSpawned = other.m_bSpawned;
    m_bSelected = other.m_bSelected;
    m_bSingleSpawn = other.m_bSingleSpawn;
    return *this;
}
// PC 0x0056B950
int CSWSEncounter::GetInActivateAreaList(unsigned long oidObject)
{
    int nIndex;

    for (nIndex = 0; nIndex < m_nActivateAreaListSize; nIndex++)
    {
        if (m_pActivateAreaList[nIndex] == oidObject)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x0056B990
void CSWSEncounter::AddToActivateAreaList(unsigned long oidObject)
{
    int nIndex;
    unsigned long* pOldList;
    unsigned long* pNewList;

    for (nIndex = 0; nIndex < m_nActivateAreaListSize; nIndex++)
    {
        if (m_pActivateAreaList[nIndex] == oidObject)
        {
            return;
        }
    }
    if (m_nActivateAreaListSize < m_nAreaListMaxSize)
    {
        m_pActivateAreaList[m_nActivateAreaListSize] = oidObject;
        m_nActivateAreaListSize++;
    }
    else
    {
        pOldList = m_pActivateAreaList;
        m_nAreaListMaxSize += 16;
        pNewList = new unsigned long[m_nAreaListMaxSize];
        for (nIndex = 0; nIndex <= m_nActivateAreaListSize; nIndex++)
        {
            pNewList[nIndex] = pOldList[nIndex];
        }
        pNewList[m_nActivateAreaListSize++] = oidObject;
        m_pActivateAreaList = pNewList;
        delete[] pOldList;
    }
}
// PC 0x0056BA70
int CSWSEncounter::RemoveFromActivateAreaList(unsigned long oidObject)
{
    int nRemoved;
    int nIndex;

    nRemoved = 0;
    for (nIndex = 0; nIndex < m_nActivateAreaListSize; nIndex++)
    {
        if (m_pActivateAreaList[nIndex] == oidObject)
        {
            nRemoved++;
        }
        else if (nRemoved != 0)
        {
            m_pActivateAreaList[nIndex - nRemoved] =
                m_pActivateAreaList[nIndex];
        }
    }
    m_nActivateAreaListSize -= nRemoved;
    return nIndex;
}
// PC 0x0056BC40
int CSWSEncounter::LoadEncounterGeometry(CResGFF* pRes, CResList& lstGeometry)
{
    CResStruct cStruct;
    int bSuccess;
    int nIndex;
    int bFirst;

    m_nActivateAreaVertexDataSize = pRes->GetListCount(&lstGeometry);
    m_nActivateAreaVertexCount = m_nActivateAreaVertexDataSize;
    if (m_pActivateAreaVertices != NULL)
    {
        delete m_pActivateAreaVertices;
    }
    if (m_nActivateAreaVertexDataSize == 0)
    {
        CExoString sError;

        sError.Format("Problem loading encounter with tag '%s'.  It has "
                      "geometry, but no vertices.  Skipping.",
                      m_sTag.CStr());
        return 0;
    }
    m_pActivateAreaVertices = new Vector[m_nActivateAreaVertexDataSize];
    if (m_pActivateAreaIndices != NULL)
    {
        delete m_pActivateAreaIndices;
    }
    m_pActivateAreaIndices = new unsigned long[m_nActivateAreaVertexCount];
    bFirst = 1;
    for (nIndex = 0; nIndex < m_nActivateAreaVertexDataSize; nIndex++)
    {
        if (pRes->GetListElement(&cStruct, &lstGeometry, nIndex) != 0)
        {
            m_pActivateAreaVertices[nIndex].m_fX =
                pRes->ReadFieldFLOAT(&cStruct, "X", bSuccess, 0.0f) +
                m_vPosition.m_fX;
            m_pActivateAreaVertices[nIndex].m_fY =
                pRes->ReadFieldFLOAT(&cStruct, "Y", bSuccess, 0.0f) +
                m_vPosition.m_fY;
            m_pActivateAreaVertices[nIndex].m_fZ =
                pRes->ReadFieldFLOAT(&cStruct, "Z", bSuccess, 0.0f) +
                m_vPosition.m_fZ;
            m_pActivateAreaIndices[nIndex] = nIndex;
            if (bFirst != 0)
            {
                m_fMinX = m_pActivateAreaVertices[nIndex].m_fX;
                m_fMaxX = m_pActivateAreaVertices[nIndex].m_fX;
                m_fMinY = m_pActivateAreaVertices[nIndex].m_fY;
                m_fMaxY = m_pActivateAreaVertices[nIndex].m_fY;
                bFirst = 0;
            }
            else
            {
                if (m_pActivateAreaVertices[nIndex].m_fX < m_fMinX)
                {
                    m_fMinX = m_pActivateAreaVertices[nIndex].m_fX;
                }
                if (m_pActivateAreaVertices[nIndex].m_fX > m_fMaxX)
                {
                    m_fMaxX = m_pActivateAreaVertices[nIndex].m_fX;
                }
                if (m_pActivateAreaVertices[nIndex].m_fY < m_fMinY)
                {
                    m_fMinY = m_pActivateAreaVertices[nIndex].m_fY;
                }
                if (m_pActivateAreaVertices[nIndex].m_fY > m_fMaxY)
                {
                    m_fMaxY = m_pActivateAreaVertices[nIndex].m_fY;
                }
            }
        }
    }
    return 1;
}
// PC 0x0056BAD0
void CSWSEncounter::LoadEncounterSpawnPoints(CResGFF* pRes,
                                             CResList& lstSpawnPoints)
{
    CResStruct cStruct;
    int bSuccess;
    int nIndex;

    m_nSpawnPointCount = pRes->GetListCount(&lstSpawnPoints);
    if (m_nSpawnPointCount > 0)
    {
        if (m_pSpawnPoints != NULL)
        {
            delete m_pSpawnPoints;
        }
        m_pSpawnPoints = new CEncounterSpawnPoint[m_nSpawnPointCount];
        for (nIndex = 0; nIndex < m_nSpawnPointCount; nIndex++)
        {
            if (pRes->GetListElement(&cStruct, &lstSpawnPoints, nIndex) != 0)
            {
                m_pSpawnPoints[nIndex].m_vPosition.m_fX =
                    pRes->ReadFieldFLOAT(&cStruct, "X", bSuccess, 0.0f);
                m_pSpawnPoints[nIndex].m_vPosition.m_fY =
                    pRes->ReadFieldFLOAT(&cStruct, "Y", bSuccess, 0.0f);
                m_pSpawnPoints[nIndex].m_vPosition.m_fZ =
                    pRes->ReadFieldFLOAT(&cStruct, "Z", bSuccess, 0.0f);
                m_pSpawnPoints[nIndex].m_fOrientation = pRes->ReadFieldFLOAT(
                    &cStruct, "Orientation", bSuccess, 0.0f);
            }
        }
    }
}
// PC 0x0056BEE0
void CSWSEncounter::ReadEncounterScriptsFromGff(CResGFF* pRes,
                                               CResStruct* pStruct)
{
    NWSENCOUNTER_TRACE();
    int bSuccess;

    {
        CResRef cDefault(m_asScripts[0]);
        CExoString sScript((char*)pRes->ReadFieldCResRef(
            pStruct, "OnEntered", bSuccess, cDefault).GetResRef());

        m_asScripts[0] = sScript;
    }
    {
        CResRef cDefault(m_asScripts[1]);
        CExoString sScript((char*)pRes->ReadFieldCResRef(
            pStruct, "OnExit", bSuccess, cDefault).GetResRef());

        m_asScripts[1] = sScript;
    }
    {
        CResRef cDefault(m_asScripts[2]);
        CExoString sScript((char*)pRes->ReadFieldCResRef(
            pStruct, "OnHeartbeat", bSuccess, cDefault).GetResRef());

        m_asScripts[2] = sScript;
    }
    {
        CResRef cDefault(m_asScripts[3]);
        CExoString sScript((char*)pRes->ReadFieldCResRef(
            pStruct, "OnExhausted", bSuccess, cDefault).GetResRef());

        m_asScripts[3] = sScript;
    }
    {
        CResRef cDefault(m_asScripts[4]);
        CExoString sScript((char*)pRes->ReadFieldCResRef(
            pStruct, "OnUserDefined", bSuccess, cDefault).GetResRef());

        m_asScripts[4] = sScript;
    }
}
// PC 0x0056C120
void CSWSEncounter::SetDifficulty(int nDifficulty)
{
    NWSENCOUNTER_TRACE();
    float fDifficulty = 0.0f;
#ifdef _XBOX
    CTwoDimArrays* pArraysForEncounterDifficulty = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(pArraysForEncounterDifficulty->m_pEncounterDifficulty);
    C2DA* pDifficulty = pArraysForEncounterDifficulty->m_pEncounterDifficulty;
#else
    C2DA* pDifficulty = g_pRules->m_p2DArrays->m_pEncounterDifficulty;
#endif

    if (pDifficulty != NULL)
    {
        CTwoDimArrays* pTables = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pTables->m_pEncounterDifficulty);
        C2DA* pTable = pTables->m_pEncounterDifficulty;
        int nRows = pTable->m_nNumRows;

        if (nDifficulty >= nRows || nDifficulty < 0)
        {
            return;
        }
        pDifficulty->GetFLOATEntry(nDifficulty, "VALUE", &fDifficulty);
    }
    m_nDifficulty = (int)fDifficulty;
}

void CSWSEncounter::PadOutCreatureCount(CEncounterListEntry* pEntries,
                                        int& nCount)
{
    NWSENCOUNTER_TRACE();
    int nRemaining;
    float fCost;
    int nFirst;
    int nSingles;
    int nEntry;
    int nIndex;
    int nLast;
    int nPicked;

    nRemaining = m_nRecommendedCreatures - m_nNumberSpawned - nCount;
    fCost = 0.0f;
    while (nRemaining > 0)
    {
        nEntry = m_nSpawnEntryCount - 1;
        if (nEntry < 0)
        {
            break;
        }
        while (1)
        {
            {
                CExoString sResRef;

                m_pSpawnEntries[nEntry].m_cResRef.CopyToString(sResRef);
                if (m_pSpawnEntries[nEntry].m_fSpawnPoolCost > fCost &&
                    (m_pSpawnEntries[nEntry].m_bSingleSpawn == 0 ||
                     m_pSpawnEntries[nEntry].m_bSpawned == 0))
                {
                    fCost = m_pSpawnEntries[nEntry].m_fSpawnPoolCost;
                    break;
                }
            }
            nEntry--;
            if (nEntry < 0)
            {
                return;
            }
        }
        int bFirst;

        nLast = 0;
        nFirst = 0;
        bFirst = 1;
        nSingles = 0;
        for (nIndex = 0; nIndex < m_nSpawnEntryCount; nIndex++)
        {
            if (m_pSpawnEntries[nIndex].m_fSpawnPoolCost == fCost)
            {
                if (bFirst == 1)
                {
                    nFirst = nIndex;
                    bFirst = 0;
                }
                nLast = nIndex;
                if (m_pSpawnEntries[nIndex].m_bSingleSpawn == 1)
                {
                    nSingles++;
                }
                m_pSpawnEntries[nIndex].m_bSelected = 0;
            }
        }
        int nRange = nLast - nFirst + 1;
        int nUsed = 0;

        while (1)
        {
            nPicked = nFirst + rand() % nRange;
            if (m_pSpawnEntries[nPicked].m_bSingleSpawn == 0 ||
                m_pSpawnEntries[nPicked].m_bSpawned == 0)
            {
                pEntries[nCount].m_cResRef = m_pSpawnEntries[nPicked].m_cResRef;
                pEntries[nCount].m_fSpawnPoolCost =
                    m_pSpawnEntries[nPicked].m_fSpawnPoolCost;
                pEntries[nCount++].m_fChallengeRating =
                    m_pSpawnEntries[nPicked].m_fChallengeRating;
                m_pSpawnEntries[nPicked].m_bSpawned = 1;
                nRemaining--;
            }
            if (m_pSpawnEntries[nPicked].m_bSelected == 0)
            {
                nUsed++;
                m_pSpawnEntries[nPicked].m_bSelected = 1;
            }
            if (nRange == nSingles && nUsed == nSingles)
            {
                break;
            }
            if (nRemaining <= 0)
            {
                return;
            }
        }
    }
    return;
}
// PC 0x0056C1D0
void CSWSEncounter::UpdateActivateAreaList(unsigned long oidObject)
{
    NWSENCOUNTER_TRACE();
    CSWSArea* pArea = GetArea();
    int nIndex;
    int nAreaIndex;
    CSWSCreature* pCreature;
    unsigned long oidCreature;

    if (pArea != NULL)
    {
        for (nIndex = 0; nIndex < m_nActivateAreaListSize; nIndex++)
        {
            CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
            pCreature = pServerExoApp->GetCreatureByGameObjectID(
                m_pActivateAreaList[nIndex]);
            if (pCreature != NULL && !GetInActivateArea(pCreature->m_vPosition))
            {
                RemoveFromActivateAreaList(m_pActivateAreaList[nIndex]);
            }
        }
        if (pArea->GetFirstObjectIndiceByX(&nAreaIndex, m_fMinX))
        {
            while (nAreaIndex < pArea->m_lstObjectIds.num)
            {
                oidCreature = pArea->m_lstObjectIds.element[nAreaIndex];
                pCreature = g_pAppManager->GetServerExoApp()->
                    GetCreatureByGameObjectID(oidCreature);
                if (pCreature == NULL)
                {
                    nAreaIndex++;
                    continue;
                }
                Vector vPosition = pCreature->m_vPosition;
                if (vPosition.m_fX > m_fMaxX)
                {
                    break;
                }
                if (pCreature->GetAIStateReputation(GetId()) ==
                        AI_STATE_REPUTATION_ENEMY &&
                    !pCreature->GetDead() && !pCreature->GetIsPCDying() &&
                    (oidCreature == oidObject ||
                     (vPosition.m_fY >= m_fMinY &&
                      vPosition.m_fY <= m_fMaxY &&
                      GetInActivateArea(vPosition))))
                {
                    if (!GetInActivateAreaList(oidCreature))
                    {
                        AddToActivateAreaList(oidCreature);
                    }
                    if (!m_bPlayerOnly || pCreature->m_pStats->m_bIsPC)
                    {
                        m_bSpawnTriggered = 1;
                    }
                    else
                    {
                        CSWPartyTable* pPartyTable =
                            g_pAppManager->GetServerExoApp()->GetPartyTable();
                        if (pPartyTable != NULL &&
                            pPartyTable->GetIsMember(oidCreature) == 1)
                        {
                            m_bSpawnTriggered = 1;
                        }
                    }
                }
                nAreaIndex++;
            }
        }
    }
}
// PC 0x0056C3A0
void CSWSEncounter::BuildInitialSpawnList(CEncounterListEntry* pEntries,
                                         int& nCount)
{
    NWSENCOUNTER_TRACE();
    float fMargin = (m_fSpawnPool - m_fSpawnPoolActive) /
                    ENCOUNTER_SPAWN_POOL_ALLOWANCE_DIVISOR;
    float fRemaining = m_fSpawnPool - m_fSpawnPoolActive;
    int nIndex = 0;
    CEncounterListEntry cEntry;

    while (nIndex < m_nSpawnEntryCount)
    {
        if (!(nCount < m_nMaxCreatures - m_nNumberSpawned &&
              fRemaining > 0.0f))
        {
            break;
        }
        cEntry = m_pSpawnEntries[nIndex];
        if (cEntry.m_bSingleSpawn == 1 && cEntry.m_bSpawned == 0)
        {
            pEntries[nCount].m_cResRef = cEntry.m_cResRef;
            pEntries[nCount].m_fSpawnPoolCost = cEntry.m_fSpawnPoolCost;
            pEntries[nCount++].m_fChallengeRating = cEntry.m_fChallengeRating;
            cEntry.m_bSpawned = 1;
            nIndex++;
            continue;
        }
        if (cEntry.m_fChallengeRating != 0.0f &&
                 !(cEntry.m_fSpawnPoolCost > fRemaining + fMargin))
        {
            if (cEntry.m_fChallengeRating ==
                m_pSpawnEntries[nIndex + 1].m_fChallengeRating)
            {
                int nRange = 1;
                int nSingles = 1;
                int nLast = nIndex + 1;
                int nUsed = 0;
                cEntry.m_bSelected = 0;
                for (; nLast < m_nSpawnEntryCount; nLast++)
                {
                    if (m_pSpawnEntries[nLast].m_fChallengeRating !=
                        m_pSpawnEntries[nIndex].m_fChallengeRating)
                    {
                        nLast--;
                        break;
                    }
                    m_pSpawnEntries[nLast].m_bSelected = 0;
                    if (m_pSpawnEntries[nLast].m_bSingleSpawn)
                    {
                        nSingles++;
                    }
                    nRange++;
                }
                int nPicked = -1;
                while (m_pSpawnEntries[nPicked].m_fSpawnPoolCost <=
                           (fRemaining + fMargin) &&
                       nCount < m_nMaxCreatures - m_nNumberSpawned)
                {
                    nPicked = nIndex + rand() % nRange;
                    if (m_pSpawnEntries[nPicked].m_bSingleSpawn == 0 ||
                        m_pSpawnEntries[nPicked].m_bSpawned == 0)
                    {
                        pEntries[nCount].m_cResRef = m_pSpawnEntries[nPicked].m_cResRef;
                        pEntries[nCount].m_fSpawnPoolCost =
                            m_pSpawnEntries[nPicked].m_fSpawnPoolCost;
                        pEntries[nCount++].m_fChallengeRating =
                            m_pSpawnEntries[nPicked].m_fChallengeRating;
                        m_pSpawnEntries[nPicked].m_bSpawned = 1;
                        fRemaining -= m_pSpawnEntries[nPicked].m_fSpawnPoolCost;
                    }
                    if (m_pSpawnEntries[nPicked].m_bSelected == 0)
                    {
                        m_pSpawnEntries[nPicked].m_bSelected = 1;
                        nUsed++;
                    }
                    if (nRange == nSingles && nUsed == nRange)
                    {
                        break;
                    }
                }
                nIndex = nLast + 1;
                continue;
            }
            else
            {
                while (cEntry.m_fSpawnPoolCost <= fRemaining + fMargin &&
                       nCount < m_nMaxCreatures - m_nNumberSpawned &&
                       (cEntry.m_bSingleSpawn == 0 || cEntry.m_bSpawned == 0))
                {
                    pEntries[nCount].m_cResRef = cEntry.m_cResRef;
                    pEntries[nCount].m_fSpawnPoolCost = cEntry.m_fSpawnPoolCost;
                    pEntries[nCount++].m_fChallengeRating = cEntry.m_fChallengeRating;
                    fRemaining -= cEntry.m_fSpawnPoolCost;
                    cEntry.m_bSpawned = 1;
                }
            }
        }
        nIndex++;
    }
}
// PC 0x0056C810
int CSWSEncounter::SaveEncounter(CResGFF* pRes, CResStruct* pStruct)
{
    CResStruct* pTopStruct;
    Vector vVertex;
    CResList lstList;
    int nIndex;

    pTopStruct = pStruct;
    pRes->WriteFieldBYTE(pTopStruct, m_bActive, "Active");
    pRes->WriteFieldBYTE(pTopStruct, m_bReset, "Reset");
    pRes->WriteFieldINT(pTopStruct, m_nResetTime, "ResetTime");
    pRes->WriteFieldINT(pTopStruct, m_nRespawns, "Respawns");
    pRes->WriteFieldINT(pTopStruct, m_nSpawnOption, "SpawnOption");
    pRes->WriteFieldINT(pTopStruct, m_nMaxCreatures, "MaxCreatures");
    pRes->WriteFieldINT(pTopStruct, m_nRecommendedCreatures, "RecCreatures");
    pRes->WriteFieldBYTE(pTopStruct, m_bPlayerOnly, "PlayerOnly");
    pRes->WriteFieldDWORD(pTopStruct, m_nFaction, "Faction");
    pRes->WriteFieldINT(pTopStruct, m_nDifficultyIndex, "DifficultyIndex");
    pRes->WriteFieldINT(pTopStruct, m_nDifficulty, "Difficulty");
    pRes->WriteFieldFLOAT(pTopStruct, m_vPosition.m_fX, "XPosition");
    pRes->WriteFieldFLOAT(pTopStruct, m_vPosition.m_fY, "YPosition");
    pRes->WriteFieldFLOAT(pTopStruct, m_vPosition.m_fZ, "ZPosition");
    pRes->WriteFieldCExoLocString(pTopStruct, m_lsFirstName, "LocalizedName");
    pRes->WriteFieldCExoString(pTopStruct, m_sTag, "Tag");
    {
        CResRef cResRef(m_asScripts[0]);

        pRes->WriteFieldCResRef(pTopStruct, cResRef, "OnEntered");
    }
    {
        CResRef cResRef(m_asScripts[1]);

        pRes->WriteFieldCResRef(pTopStruct, cResRef, "OnExit");
    }
    {
        CResRef cResRef(m_asScripts[2]);

        pRes->WriteFieldCResRef(pTopStruct, cResRef, "OnHeartbeat");
    }
    {
        CResRef cResRef(m_asScripts[3]);

        pRes->WriteFieldCResRef(pTopStruct, cResRef, "OnExhausted");
    }
    {
        CResRef cResRef(m_asScripts[4]);

        pRes->WriteFieldCResRef(pTopStruct, cResRef, "OnUserDefined");
    }
    if (m_nActivateAreaVertexDataSize > 0)
    {
        pRes->AddList(&lstList, pTopStruct, "Geometry");
        for (nIndex = 0; nIndex < m_nActivateAreaVertexDataSize; nIndex++)
        {
            vVertex = m_pActivateAreaVertices[nIndex];
            vVertex -= m_vPosition;
            pRes->AddListElement((CResStruct*)&pStruct, &lstList, nIndex);
            pRes->WriteFieldFLOAT((CResStruct*)&pStruct, vVertex.m_fX, "X");
            pRes->WriteFieldFLOAT((CResStruct*)&pStruct, vVertex.m_fY, "Y");
            pRes->WriteFieldFLOAT((CResStruct*)&pStruct, vVertex.m_fZ, "Z");
        }
    }
    if (m_nSpawnEntryCount > 0)
    {
        pRes->AddList(&lstList, pTopStruct, "CreatureList");
        for (nIndex = 0; nIndex < m_nSpawnEntryCount; nIndex++)
        {
            pRes->AddListElement((CResStruct*)&pStruct, &lstList, nIndex);
            pRes->WriteFieldCResRef((CResStruct*)&pStruct,
                                    m_pSpawnEntries[nIndex].m_cResRef,
                                    "ResRef");
            pRes->WriteFieldFLOAT((CResStruct*)&pStruct,
                                  m_pSpawnEntries[nIndex].m_fChallengeRating,
                                  "CR");
            pRes->WriteFieldBYTE((CResStruct*)&pStruct,
                                 m_pSpawnEntries[nIndex].m_bSingleSpawn,
                                 "SingleSpawn");
        }
    }
    if (m_nSpawnPointCount > 0)
    {
        pRes->AddList(&lstList, pTopStruct, "SpawnPointList");
        for (nIndex = 0; nIndex < m_nSpawnPointCount; nIndex++)
        {
            pRes->AddListElement((CResStruct*)&pStruct, &lstList, nIndex);
            pRes->WriteFieldFLOAT((CResStruct*)&pStruct,
                                  m_pSpawnPoints[nIndex].m_vPosition.m_fX, "X");
            pRes->WriteFieldFLOAT((CResStruct*)&pStruct,
                                  m_pSpawnPoints[nIndex].m_vPosition.m_fY, "Y");
            pRes->WriteFieldFLOAT((CResStruct*)&pStruct,
                                  m_pSpawnPoints[nIndex].m_vPosition.m_fZ, "Z");
            pRes->WriteFieldFLOAT((CResStruct*)&pStruct,
                                  m_pSpawnPoints[nIndex].m_fOrientation,
                                  "Orientation");
        }
    }
    pRes->WriteFieldINT(pTopStruct, m_nNumberSpawned, "NumberSpawned");
    pRes->WriteFieldDWORD(pTopStruct, m_nHeartbeatDay, "HeartbeatDay");
    pRes->WriteFieldDWORD(pTopStruct, m_nHeartbeatTime, "HeartbeatTime");
    pRes->WriteFieldDWORD(pTopStruct, m_nLastSpawnDay, "LastSpawnDay");
    pRes->WriteFieldDWORD(pTopStruct, m_nLastSpawnTime, "LastSpawnTime");
    pRes->WriteFieldBYTE(pTopStruct, m_bStarted, "Started");
    pRes->WriteFieldBYTE(pTopStruct, m_bExhausted, "Exhausted");
    pRes->WriteFieldINT(pTopStruct, m_nCurrentSpawns, "CurrentSpawns");
    pRes->WriteFieldFLOAT(pTopStruct, m_fSpawnPoolActive, "SpawnPoolActive");
    pRes->WriteFieldDWORD(pTopStruct, m_oidLastEntered, "LastEntered");
    pRes->WriteFieldDWORD(pTopStruct, m_oidLastLeft, "LastLeft");
    pRes->WriteFieldINT(pTopStruct, m_nCustomScriptId, "CustomScriptId");
    pRes->WriteFieldINT(pTopStruct, m_nAreaListMaxSize, "AreaListMaxSize");
    pRes->WriteFieldFLOAT(pTopStruct, m_fAreaPoints, "AreaPoints");
    if (m_nActivateAreaListSize > 0)
    {
        pRes->WriteFieldINT(pTopStruct, m_nActivateAreaListSize,
                            "AreaListSize");
        pRes->AddList(&lstList, pTopStruct, "AreaList");
        for (nIndex = 0; nIndex < m_nActivateAreaListSize; nIndex++)
        {
            pRes->AddListElement((CResStruct*)&pStruct, &lstList, 3);
            pRes->WriteFieldDWORD((CResStruct*)&pStruct,
                                  m_pActivateAreaList[nIndex], "AreaObject");
        }
    }
    if (m_lstSpawnList.num > 0)
    {
        pRes->AddList(&lstList, pTopStruct, "SpawnList");
        for (nIndex = 0; nIndex < m_lstSpawnList.num; nIndex++)
        {
            pRes->AddListElement((CResStruct*)&pStruct, &lstList, nIndex);
            pRes->WriteFieldCResRef((CResStruct*)&pStruct,
                                    m_lstSpawnList[nIndex]->m_cResRef,
                                    "SpawnResRef");
            pRes->WriteFieldFLOAT(
                (CResStruct*)&pStruct,
                m_lstSpawnList[nIndex]->m_fChallengeRating, "SpawnCR");
        }
    }
    return 1;
}

void CSWSEncounter::AddCreaturesToSpawnList(CEncounterListEntry* pEntries,
                                          int& nCount, int nIndex,
                                          CEncounterListEntry cEntry)
{
    float fPoints = CalculatePointsFromCR(cEntry.m_fChallengeRating);

    if (fPoints != m_fMinSpawnPoolCost)
    {
        if (nCount == 1)
        {
            nCount = 0;
        }
        else
        {
            int nRemoved = 0;
            for (int i = 0; i < nCount; i++)
            {
                if (i == nIndex)
                {
                    nRemoved++;
                }
                else if (nRemoved != 0)
                {
                    pEntries[i - nRemoved] = pEntries[i];
                }
            }
            nCount -= nRemoved;
        }

        float fMaxCost = fPoints - ENCOUNTER_SPAWN_COST_TOLERANCE;
        for (int i = 0; i < m_nSpawnEntryCount; i++)
        {
            if (nCount > m_nRecommendedCreatures - m_nNumberSpawned)
            {
                break;
            }
            while (m_pSpawnEntries[i].m_fSpawnPoolCost <= fPoints &&
                   m_pSpawnEntries[i].m_fSpawnPoolCost <= fMaxCost &&
                   nCount < m_nMaxCreatures &&
                   (!m_pSpawnEntries[i].m_bSingleSpawn ||
                    !m_pSpawnEntries[i].m_bSpawned))
            {
                pEntries[nCount].m_cResRef = m_pSpawnEntries[i].m_cResRef;
                pEntries[nCount].m_fSpawnPoolCost =
                    m_pSpawnEntries[i].m_fSpawnPoolCost;
                pEntries[nCount].m_fChallengeRating =
                    m_pSpawnEntries[i].m_fChallengeRating;
                m_pSpawnEntries[i].m_bSpawned = 1;
                nCount++;
                fPoints -= m_pSpawnEntries[i].m_fSpawnPoolCost;
            }
        }
    }
}
// PC 0x0056D640
int CSWSEncounter::ReadEncounterFromGff(CResGFF* pRes, CResStruct* pStruct,
                                        int bLoadPosition, Vector* pvPosition)
{
    int bSuccess;
    CResStruct stCreature;
    CResStruct stArea;
    CResStruct stSpawn;

    m_bActive = pRes->ReadFieldBYTE(pStruct, "Active", bSuccess, m_bActive);
    m_bReset = pRes->ReadFieldBYTE(pStruct, "Reset", bSuccess, m_bReset);
    m_nResetTime = pRes->ReadFieldINT(pStruct, "ResetTime", bSuccess, m_nResetTime);
    m_nRespawns = pRes->ReadFieldINT(pStruct, "Respawns", bSuccess, m_nRespawns);
    m_nSpawnOption = pRes->ReadFieldINT(pStruct, "SpawnOption", bSuccess, m_nSpawnOption);
    m_nMaxCreatures = pRes->ReadFieldINT(pStruct, "MaxCreatures", bSuccess, m_nMaxCreatures);
    m_nRecommendedCreatures = pRes->ReadFieldINT(pStruct, "RecCreatures", bSuccess, m_nRecommendedCreatures);
    m_bPlayerOnly = pRes->ReadFieldBYTE(pStruct, "PlayerOnly", bSuccess, m_bPlayerOnly);
    m_nFaction = pRes->ReadFieldDWORD(pStruct, "Faction", bSuccess, m_nFaction);
    m_nDifficultyIndex = pRes->ReadFieldINT(pStruct, "DifficultyIndex", bSuccess, -1);
    EXOXBOXASSERT(m_nDifficultyIndex != -1);

#ifdef _XBOX
    CTwoDimArrays* pArraysForEncounterDifficulty = g_pRules->Get2DArrays();
    EXOXBOXASSERT(pArraysForEncounterDifficulty->m_pEncounterDifficulty);
    C2DA* pDifficulty = pArraysForEncounterDifficulty->m_pEncounterDifficulty;
#else
    C2DA* pDifficulty = g_pRules->Get2DArrays()->m_pEncounterDifficulty;
#endif
    if (pDifficulty != NULL)
    {
        pDifficulty->GetINTEntry(m_nDifficultyIndex, "VALUE", &m_nDifficulty);
    }
    else
    {
        m_nDifficulty = pRes->ReadFieldINT(pStruct, "Difficulty", bSuccess, m_nDifficulty);
    }

    Vector vPosition;
    vPosition.m_fX = pRes->ReadFieldFLOAT(pStruct, "XPosition", bSuccess, m_vPosition.m_fX);
    vPosition.m_fY = pRes->ReadFieldFLOAT(pStruct, "YPosition", bSuccess, m_vPosition.m_fY);
    vPosition.m_fZ = pRes->ReadFieldFLOAT(pStruct, "ZPosition", bSuccess, m_vPosition.m_fZ);
    SetPosition(vPosition, 1, 1, 0);
    if (pvPosition != NULL)
    {
        SetPosition(*pvPosition, 1, 1, 0);
    }

    m_lsFirstName = pRes->ReadFieldCExoLocString(pStruct, "LocalizedName", bSuccess, m_lsFirstName);
    SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, m_sTag));
    g_pAppManager->GetServerExoApp()->GetModule()->AddObjectToLookupTable(m_sTag, GetId());

    CResList lstGeometry;
    if (pRes->GetList(&lstGeometry, pStruct, "Geometry"))
    {
        if (!LoadEncounterGeometry(pRes, lstGeometry))
        {
            return 0;
        }
    }
    else if (bLoadPosition)
    {
        m_nActivateAreaVertexDataSize = 4;
        m_nActivateAreaVertexCount = 4;
        if (m_pActivateAreaVertices != NULL)
        {
            delete[] m_pActivateAreaVertices;
        }
        m_pActivateAreaVertices = new Vector[m_nActivateAreaVertexDataSize];
        if (m_pActivateAreaIndices != NULL)
        {
            delete[] m_pActivateAreaIndices;
        }
        m_pActivateAreaIndices = new unsigned long[m_nActivateAreaVertexCount];

        Vector vCenter = m_vPosition;
        float fMinX = vCenter.m_fX - ENCOUNTER_DEFAULT_AREA_HALF_EXTENT;
        m_fMinX = fMinX;
        float fMinY = vCenter.m_fY - ENCOUNTER_DEFAULT_AREA_HALF_EXTENT;
        m_fMinY = fMinY;
        float fMaxX = vCenter.m_fX + ENCOUNTER_DEFAULT_AREA_HALF_EXTENT;
        m_fMaxX = fMaxX;
        float fMaxY = vCenter.m_fY + ENCOUNTER_DEFAULT_AREA_HALF_EXTENT;
        m_fMaxY = fMaxY;

        m_pActivateAreaVertices[0].m_fX = fMinX;
        m_pActivateAreaVertices[0].m_fY = fMinY;
        m_pActivateAreaVertices[0].m_fZ = vCenter.m_fZ;
        m_pActivateAreaIndices[0] = 0;
        m_pActivateAreaVertices[1].m_fX = fMaxX;
        m_pActivateAreaVertices[1].m_fY = fMinY;
        m_pActivateAreaVertices[1].m_fZ = vCenter.m_fZ;
        m_pActivateAreaIndices[1] = 1;
        m_pActivateAreaVertices[2].m_fX = fMaxX;
        m_pActivateAreaVertices[2].m_fY = fMaxY;
        m_pActivateAreaVertices[2].m_fZ = vCenter.m_fZ;
        m_pActivateAreaIndices[2] = 2;
        m_pActivateAreaVertices[3].m_fX = fMinX;
        m_pActivateAreaVertices[3].m_fY = fMaxY;
        m_pActivateAreaVertices[3].m_fZ = vCenter.m_fZ;
        m_pActivateAreaIndices[3] = 3;
    }

    CResList lstCreatures;
    if (pRes->GetList(&lstCreatures, pStruct, "CreatureList"))
    {
        m_nSpawnEntryCount = pRes->GetListCount(&lstCreatures);
        if (m_pSpawnEntries != NULL)
        {
            delete[] m_pSpawnEntries;
        }
        m_pSpawnEntries = new CEncounterListEntry[m_nSpawnEntryCount];
        for (int nCreature = 0; nCreature < m_nSpawnEntryCount; nCreature++)
        {
            if (pRes->GetListElement(&stCreature, &lstCreatures, nCreature))
            {
                CResRef cDefault("");
                m_pSpawnEntries[nCreature].m_cResRef = pRes->ReadFieldCResRef(&stCreature, "ResRef", bSuccess, cDefault);
                m_pSpawnEntries[nCreature].m_fChallengeRating = pRes->ReadFieldFLOAT(&stCreature, "CR", bSuccess, 0.0f);
                m_pSpawnEntries[nCreature].m_fSpawnPoolCost = CalculatePointsFromCR(m_pSpawnEntries[nCreature].m_fChallengeRating);
                m_pSpawnEntries[nCreature].m_bSingleSpawn = pRes->ReadFieldBYTE(&stCreature, "SingleSpawn", bSuccess, 0);
                m_pSpawnEntries[nCreature].m_bSpawned = 0;
            }
        }
    }

    CEncounterListEntry cTemp;
    int nIndex;
    int nSortIndex;
    for (nIndex = 0; nIndex < m_nSpawnEntryCount; nIndex++)
    {
        for (nSortIndex = 0; nSortIndex < m_nSpawnEntryCount - 1; nSortIndex++)
        {
            if (m_pSpawnEntries[nSortIndex].m_fChallengeRating <
                m_pSpawnEntries[nSortIndex + 1].m_fChallengeRating)
            {
                cTemp.m_fChallengeRating = m_pSpawnEntries[nSortIndex].m_fChallengeRating;
                cTemp.m_cResRef = m_pSpawnEntries[nSortIndex].m_cResRef;
                cTemp.m_fSpawnPoolCost = m_pSpawnEntries[nSortIndex].m_fSpawnPoolCost;
                cTemp.m_bSingleSpawn = m_pSpawnEntries[nSortIndex].m_bSingleSpawn;

                m_pSpawnEntries[nSortIndex].m_fChallengeRating = m_pSpawnEntries[nSortIndex + 1].m_fChallengeRating;
                m_pSpawnEntries[nSortIndex].m_cResRef = m_pSpawnEntries[nSortIndex + 1].m_cResRef;
                m_pSpawnEntries[nSortIndex].m_fSpawnPoolCost = m_pSpawnEntries[nSortIndex + 1].m_fSpawnPoolCost;
                m_pSpawnEntries[nSortIndex].m_bSingleSpawn = m_pSpawnEntries[nSortIndex + 1].m_bSingleSpawn;
                m_pSpawnEntries[nSortIndex].m_bSelected = 0;
                m_pSpawnEntries[nSortIndex].m_bSpawned = 0;

                m_pSpawnEntries[nSortIndex + 1].m_fChallengeRating = cTemp.m_fChallengeRating;
                m_pSpawnEntries[nSortIndex + 1].m_cResRef = cTemp.m_cResRef;
                m_pSpawnEntries[nSortIndex + 1].m_fSpawnPoolCost = cTemp.m_fSpawnPoolCost;
                m_pSpawnEntries[nSortIndex + 1].m_bSingleSpawn = cTemp.m_bSingleSpawn;
                m_pSpawnEntries[nSortIndex + 1].m_bSelected = 0;
                m_pSpawnEntries[nSortIndex + 1].m_bSpawned = 0;
            }
        }
    }

    CExoString sResRef;
    for (nIndex = 0; nIndex < m_nSpawnEntryCount; nIndex++)
    {
        m_pSpawnEntries[nIndex].m_cResRef.CopyToString(sResRef);
        sResRef.CStr();
    }
    if (m_nSpawnEntryCount > 0)
    {
        m_fMinSpawnPoolCost = m_pSpawnEntries[m_nSpawnEntryCount - 1].m_fSpawnPoolCost;
    }

    CResList lstSpawnPoints;
    if (pRes->GetList(&lstSpawnPoints, pStruct, "SpawnPointList"))
    {
        LoadEncounterSpawnPoints(pRes, lstSpawnPoints);
    }
    m_nNumberSpawned = pRes->ReadFieldINT(pStruct, "NumberSpawned", bSuccess, m_nNumberSpawned);
    m_nHeartbeatDay = pRes->ReadFieldDWORD(pStruct, "HeartbeatDay", bSuccess, m_nHeartbeatDay);
    m_nHeartbeatTime = pRes->ReadFieldDWORD(pStruct, "HeartbeatTime", bSuccess, m_nHeartbeatTime);
    m_nLastSpawnDay = pRes->ReadFieldDWORD(pStruct, "LastSpawnDay", bSuccess, m_nLastSpawnDay);
    m_nLastSpawnTime = pRes->ReadFieldDWORD(pStruct, "LastSpawnTime", bSuccess, m_nLastSpawnTime);
    m_oidLastEntered = pRes->ReadFieldDWORD(pStruct, "LastEntered", bSuccess, m_oidLastEntered);
    m_oidLastLeft = pRes->ReadFieldDWORD(pStruct, "LastLeft", bSuccess, m_oidLastLeft);
    m_bStarted = pRes->ReadFieldBYTE(pStruct, "Started", bSuccess, m_bStarted);
    m_bExhausted = pRes->ReadFieldBYTE(pStruct, "Exhausted", bSuccess, m_bExhausted);
    m_nCurrentSpawns = pRes->ReadFieldINT(pStruct, "CurrentSpawns", bSuccess, m_nCurrentSpawns);
    m_nCustomScriptId = pRes->ReadFieldINT(pStruct, "CustomScriptId", bSuccess, m_nCustomScriptId);
    m_nAreaListMaxSize = pRes->ReadFieldINT(pStruct, "AreaListMaxSize", bSuccess, m_nAreaListMaxSize);
    m_fSpawnPoolActive = pRes->ReadFieldFLOAT(pStruct, "SpawnPoolActive", bSuccess, m_fSpawnPoolActive);
    m_fAreaPoints = pRes->ReadFieldFLOAT(pStruct, "AreaPoints", bSuccess, m_fAreaPoints);

    CResList lstArea;
    if (pRes->GetList(&lstArea, pStruct, "AreaList"))
    {
        m_nActivateAreaListSize = pRes->GetListCount(&lstArea);
        if (m_nActivateAreaListSize > 0)
        {
            if (m_pActivateAreaList != NULL)
            {
                delete[] m_pActivateAreaList;
            }
            m_pActivateAreaList = new unsigned long[m_nAreaListMaxSize];
            for (nIndex = 0; nIndex < m_nActivateAreaListSize; nIndex++)
            {
                if (pRes->GetListElement(&stArea, &lstArea, 3))
                {
                    m_pActivateAreaList[nIndex] = pRes->ReadFieldDWORD(&stArea, "AreaObject", bSuccess, 0);
                }
            }
        }
    }

    CResList lstSpawn;
    if (pRes->GetList(&lstSpawn, pStruct, "SpawnList"))
    {
        if (m_lstSpawnList.num > 0)
        {
            ClearSpawnList();
        }
        int nSpawnCount = pRes->GetListCount(&lstSpawn);
        for (nIndex = 0; nIndex < nSpawnCount; nIndex++)
        {
            if (pRes->GetListElement(&stSpawn, &lstSpawn, nIndex))
            {
                CEncounterListEntry* pEntry = new CEncounterListEntry;
                CResRef cDefault("");
                pEntry->m_cResRef = pRes->ReadFieldCResRef(&stSpawn, "SpawnResRef", bSuccess, cDefault);
                pEntry->m_fChallengeRating = pRes->ReadFieldFLOAT(&stSpawn, "SpawnCR", bSuccess, 0.0f);
                m_lstSpawnList.Add(pEntry);
            }
        }
    }
    return 1;
}

void CSWSEncounter::CalculateSpawnPool(float fChallengeRating)
{
    if (fChallengeRating == -1.0f)
    {
        TallyEnemyRadiusPoints();
    }
    else
    {
        m_fAreaPoints = CalculatePointsFromCR(fChallengeRating);
    }
    if (m_fAreaPoints != 0.0f)
    {
        m_nSpawnLevel =
            m_nDifficulty + (int)(log(m_fAreaPoints * 0.25) / log(1.5) + 0.5);
        m_fSpawnPool += CalculatePointsFromCR((float)(m_nSpawnLevel - 1));
    }
}
// PC 0x0056E470
void CSWSEncounter::LoadFractionalCRData()
{
#ifdef _XBOX
    CTwoDimArrays* pArraysForFractionalCR = g_pRules->Get2DArrays();
    EXOXBOXASSERT(pArraysForFractionalCR->m_pFractionalCR);
    C2DA* pFractionalCR = pArraysForFractionalCR->m_pFractionalCR;
#else
    C2DA* pFractionalCR = g_pRules->Get2DArrays()->m_pFractionalCR;
#endif

    if (pFractionalCR != NULL)
    {
        m_plstFractionalCR = new CExoArrayList<float>(0);

        for (int nRow = 0; nRow < 5; nRow++)
        {
            float fMin;
            pFractionalCR->GetFLOATEntry(nRow, "Min", &fMin);
            m_plstFractionalCR->Add(fMin);
        }
    }
}
// PC 0x0056E5D0
CSWSEncounter::CSWSEncounter(unsigned long oidEncounter)
    : CSWSObject(SW_OBJECT_TYPE_ENCOUNTER, oidEncounter, 0)
{
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddObject(this, 0);

    unsigned long nCalendarDay, nTimeOfDay;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    pServer->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
    m_nLastUpdateDay = nCalendarDay;
    m_nLastUpdateTime = nTimeOfDay;

    m_nLastSpawnDay = 0;
    m_nLastSpawnTime = 0;
    m_nHeartbeatTime = 0;
    m_nHeartbeatDay = 0;
    m_nSpawnEntryCount = 0;
    m_pSpawnEntries = NULL;
    m_nAreaListMaxSize = 16;
    m_pActivateAreaList = new unsigned long[m_nAreaListMaxSize];
    m_nActivateAreaListSize = 0;
    m_bSpawnTriggered = 0;
    m_pActivateAreaVertices = NULL;
    m_pActivateAreaIndices = NULL;
    m_nActivateAreaVertexDataSize = 0;
    m_nActivateAreaVertexCount = 0;
    m_oidLastEntered = OBJECT_INVALID;
    m_nSpawnLevel = 0;
    m_nSpawnPointCount = 0;
    m_nCurrentSpawns = 0;
    m_nNumberSpawned = 0;
    m_fSpawnPool = 0.0f;
    m_fSpawnPoolActive = 0.0f;
    m_fCreaturePoints = 0.0f;
    m_fMinSpawnPoolCost = 0.0f;
    m_oidLastLeft = OBJECT_INVALID;
    m_pSpawnPoints = NULL;
    m_bStarted = 0;

    for (int nLevel = 0; nLevel <= 50; nLevel++)
    {
        m_sPowerLookup.Add((float)pow(1.5, nLevel));
    }
    LoadFractionalCRData();
    SetActive(1);

    m_nSpawnOption = SPAWN_OPTION_ONCE;
    m_bReset = 0;
    m_nResetTime = 60;
    m_nFaction = STANDARD_FACTION_HOSTILE_1;
    m_nDifficulty = 0;
    Vector vPosition;
    SetPosition(vPosition, 1, 1, 0);
    m_nMaxCreatures = 8;
    m_nRecommendedCreatures = 2;
    m_nRespawns = 0;
    m_bPlayerOnly = 0;
}
// PC 0x0056E8F0
CSWSEncounter::~CSWSEncounter()
{
    RemoveFromArea();

    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->RemoveObject(this);

    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        unsigned long oidObject = m_nId;
        pModule->RemoveObjectFromLookupTable(m_sTag, oidObject);
    }

    if (m_pSpawnEntries != NULL)
    {
        delete[] m_pSpawnEntries;
        m_pSpawnEntries = NULL;
    }
    if (m_pActivateAreaList != NULL)
    {
        delete[] m_pActivateAreaList;
        m_pActivateAreaList = NULL;
    }
    if (m_pActivateAreaVertices != NULL)
    {
        delete[] m_pActivateAreaVertices;
        m_pActivateAreaVertices = NULL;
    }
    if (m_pActivateAreaIndices != NULL)
    {
        delete[] m_pActivateAreaIndices;
        m_pActivateAreaIndices = NULL;
    }
    if (m_pSpawnPoints != NULL)
    {
        delete[] m_pSpawnPoints;
        m_pSpawnPoints = NULL;
    }

    ClearSpawnList();
    if (m_plstFractionalCR != NULL)
    {
        delete m_plstFractionalCR;
    }
}
// PC 0x0056EDD0
void CSWSEncounter::AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                              int nFlags)
{
    RemoveFromArea();
    if (pArea != NULL)
    {
        SetArea(pArea);
        Vector vPosition(fX, fY, fZ);

        SetPosition(vPosition, 0, 1, 0);
        GetArea()->AddObjectToArea(m_nId, nFlags);
        CSWSArea* pCurrentArea = GetArea();

        pCurrentArea->m_lstTriggerIds.Add(m_nId);
    }
}
// PC 0x0056D490
void CSWSEncounter::BuildSpawnList()
{
    CEncounterListEntry* pEntries = new CEncounterListEntry[m_nMaxCreatures];
    int nCount = 0;

    BuildInitialSpawnList(pEntries, nCount);
    if (nCount < m_nRecommendedCreatures - m_nNumberSpawned &&
        m_fSpawnPool > m_fMinSpawnPoolCost)
    {
        int nIndex;
        do
        {
            nIndex = nCount - 1;
            while (nIndex >= 0 && nCount < m_nRecommendedCreatures - m_nNumberSpawned)
            {
                AddCreaturesToSpawnList(pEntries, nCount, nIndex, pEntries[nIndex]);
                nIndex--;
            }
        } while (nIndex >= 0 && nCount < m_nRecommendedCreatures - m_nNumberSpawned);
    }
    if (nCount < m_nRecommendedCreatures - m_nNumberSpawned)
    {
        PadOutCreatureCount(pEntries, nCount);
    }
    if (m_fSpawnPool < m_fMinSpawnPoolCost)
    {
        m_fSpawnPool = 0.0f;
    }
    if (nCount != 0)
    {
        for (int nIndex = 0; nIndex < nCount; nIndex++)
        {
            CEncounterListEntry* pEntry = new CEncounterListEntry;
            pEntry->m_cResRef = pEntries[nIndex].m_cResRef;
            pEntry->m_fChallengeRating = pEntries[nIndex].m_fChallengeRating;
            m_lstSpawnList.Add(pEntry);
        }
        delete[] pEntries;
    }
}
// PC 0x0056EFB0
int CSWSEncounter::LoadEncounter(CResGFF* pRes, CResStruct* pStruct)
{
    if (!ReadEncounterFromGff(pRes, pStruct, 0, NULL))
    {
        return 0;
    }

    ReadEncounterScriptsFromGff(pRes, pStruct);

    CEncounterListEntry cTemp;
    int nPass;
    int nIndex;

    for (nPass = 0; nPass < m_nSpawnEntryCount; nPass++)
    {
        for (nIndex = 0; nIndex < m_nSpawnEntryCount - 1; nIndex++)
        {
            if (m_pSpawnEntries[nIndex].m_fChallengeRating <
                m_pSpawnEntries[nIndex + 1].m_fChallengeRating)
            {
                cTemp.m_fChallengeRating =
                    m_pSpawnEntries[nIndex].m_fChallengeRating;
                cTemp.m_cResRef = m_pSpawnEntries[nIndex].m_cResRef;
                cTemp.m_fSpawnPoolCost = m_pSpawnEntries[nIndex].m_fSpawnPoolCost;
                cTemp.m_bSingleSpawn = m_pSpawnEntries[nIndex].m_bSingleSpawn;

                m_pSpawnEntries[nIndex].m_fChallengeRating =
                    m_pSpawnEntries[nIndex + 1].m_fChallengeRating;
                m_pSpawnEntries[nIndex].m_cResRef =
                    m_pSpawnEntries[nIndex + 1].m_cResRef;
                m_pSpawnEntries[nIndex].m_fSpawnPoolCost =
                    m_pSpawnEntries[nIndex + 1].m_fSpawnPoolCost;
                m_pSpawnEntries[nIndex].m_bSingleSpawn =
                    m_pSpawnEntries[nIndex + 1].m_bSingleSpawn;
                m_pSpawnEntries[nIndex].m_bSelected = 0;
                m_pSpawnEntries[nIndex].m_bSpawned = 0;

                m_pSpawnEntries[nIndex + 1].m_fChallengeRating =
                    cTemp.m_fChallengeRating;
                m_pSpawnEntries[nIndex + 1].m_cResRef = cTemp.m_cResRef;
                m_pSpawnEntries[nIndex + 1].m_fSpawnPoolCost =
                    cTemp.m_fSpawnPoolCost;
                m_pSpawnEntries[nIndex + 1].m_bSingleSpawn =
                    cTemp.m_bSingleSpawn;
                m_pSpawnEntries[nIndex + 1].m_bSelected = 0;
                m_pSpawnEntries[nIndex + 1].m_bSpawned = 0;
            }
        }
    }

    CExoString sResRef;
    for (nIndex = 0; nIndex < m_nSpawnEntryCount; nIndex++)
    {
        m_pSpawnEntries[nIndex].m_cResRef.CopyToString(sResRef);
        sResRef.CStr();
    }

    if (m_nSpawnEntryCount > 0)
    {
        m_fMinSpawnPoolCost =
            m_pSpawnEntries[m_nSpawnEntryCount - 1].m_fSpawnPoolCost;
    }

    return 1;
}
// PC 0x0056F240
int CSWSEncounter::LoadFromTemplate(const CResRef& cResRef, int bLoadPosition,
                                    Vector* pvPosition)
{
    CResGFF* pRes = new CResGFF(2040, "UTE ", cResRef);

    if (pRes->m_bLoaded == 0)
    {
        delete pRes;
        CExoString sMessage;
        CExoString sResRef;
        cResRef.CopyToString(sResRef);
        sMessage.Format("Encounter template %s doesn't exist.\n",
                        sResRef.CStr());
        return 0;
    }

    CResStruct stTop;
    pRes->GetTopLevelStruct(&stTop);
    if (bLoadPosition != 0)
    {
        ReadEncounterFromGff(pRes, &stTop, bLoadPosition, pvPosition);
    }
    else
    {
        ReadEncounterFromGff(pRes, &stTop, 0, NULL);
    }
    ReadEncounterScriptsFromGff(pRes, &stTop);
    delete pRes;
    return 1;
}
// PC 0x0056EA80
void CSWSEncounter::AIUpdate()
{
    SpawnCreatureFromSpawnList();

    unsigned __int64 nTimer =
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    unsigned long nNowDay, nNowTime;
    unsigned long nDiffDay, nDiffTime;
    unsigned long nHeartbeatDay, nHeartbeatTime;
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetWorldTimer()->GetWorldTime(&nNowDay, &nNowTime);
    }
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetWorldTimer()->SubtractWorldTimes(
            nNowDay, nNowTime, m_nLastUpdateDay, m_nLastUpdateTime,
            &nDiffDay, &nDiffTime);
    }
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetWorldTimer()->SubtractWorldTimes(
            nNowDay, nNowTime, m_nHeartbeatDay, m_nHeartbeatTime,
            &nHeartbeatDay, &nHeartbeatTime);
    }
    m_nLastUpdateDay = nNowDay;
    m_nLastUpdateTime = nNowTime;
    m_nElapsedTime = nDiffTime;

    if (m_nSpawnOption == SPAWN_OPTION_CONTINUOUS && m_bReset == 1 &&
        m_nLastSpawnDay != 0 && m_nLastSpawnTime != 0)
    {
        unsigned long nResetDay, nResetTime;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetWorldTimer()->SubtractWorldTimes(
            nNowDay, nNowTime, m_nLastSpawnDay, m_nLastSpawnTime,
            &nResetDay, &nResetTime);
        if (nResetTime >= m_nResetTime * 1000 || nResetDay >= 1)
        {
            if (m_nCurrentSpawns < m_nRespawns || m_nRespawns == RESPAWNS_UNLIMITED)
            {
                m_nCurrentSpawns++;
                m_nLastSpawnDay = 0;
                m_nLastSpawnTime = 0;
                m_bActive = 1;
                SpawnIfAppropriate(OBJECT_INVALID, -1.0f);
                m_fSpawnPool = 0.0f;
                m_fSpawnPoolActive = 0.0f;
            }
        }
    }
    if (m_bActive != 0)
    {
        if (nHeartbeatTime >= HEARTBEAT_INTERVAL)
        {
            if (m_nHeartbeatTime != 0)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_HEARTBEAT;
                pEvent->SetObjectID(0, GetId());
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), GetId(),
                    CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
            m_nHeartbeatTime = nNowTime;
            m_nHeartbeatDay = nNowDay;
        }
        RunActions(nNowDay, nNowTime, nTimer);
    }
}
// PC 0x0056ECE0
void CSWSEncounter::EventHandler(unsigned long nEventId,
                                 unsigned long oidCaller, void* pEventData,
                                 unsigned long nCalendarDay,
                                 unsigned long nTimeOfDay)
{
    CScriptEvent* pEvent;

    if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        pEvent = (CScriptEvent*)pEventData;
        if (pEvent->m_nType == 12)
        {
            m_oidLastEntered = pEvent->GetObjectID(0);
            SpawnIfAppropriate(pEvent->GetObjectID(0), -1.0f);
            g_pVirtualMachine->RunScript(&m_asScripts[0], m_nId, 1);
        }
        else if (pEvent->m_nType == 13)
        {
            m_oidLastLeft = pEvent->GetObjectID(0);
            UpdateActivateAreaList(OBJECT_INVALID);
            g_pVirtualMachine->RunScript(&m_asScripts[1], m_nId, 1);
        }
        else if (pEvent->m_nType == 0)
        {
            g_pVirtualMachine->RunScript(&m_asScripts[2], m_nId, 1);
        }
        else if (pEvent->m_nType == 21)
        {
            g_pVirtualMachine->RunScript(&m_asScripts[3], m_nId, 1);
        }
        else if (pEvent->m_nType == 11)
        {
            m_nCustomScriptId = pEvent->GetInteger(0);
            g_pVirtualMachine->RunScript(&m_asScripts[4], m_nId, 1);
        }
        delete pEvent;
    }
}
// PC 0x0056E420
void CSWSEncounter::SpawnIfAppropriate(unsigned long oidObject,
                                       float fChallengeRating)
{
    if (m_bActive != 0 && m_bStarted != 1)
    {
        m_bSpawnTriggered = 0;
        UpdateActivateAreaList(oidObject);
        if (m_bSpawnTriggered == 1)
        {
            CalculateSpawnPool(fChallengeRating);
            if (m_fSpawnPool > 0.0f)
            {
                BuildSpawnList();
                m_bStarted = 1;
            }
        }
    }
}
// PC 0x0056EE80
void CSWSEncounter::RemoveFromActiveCreatureCount(float fChallengeRating,
                                                  int bAddToPool)
{
    int nIndex;
    CServerExoApp* pExoApp;

    if (m_nNumberSpawned != 0)
    {
        m_fSpawnPoolActive -= CalculatePointsFromCR(fChallengeRating);
        m_nNumberSpawned--;
        if (m_fSpawnPoolActive < 0.0f)
        {
            m_fSpawnPoolActive = 0.0f;
        }
        if (bAddToPool == 1)
        {
            m_fSpawnPool += CalculatePointsFromCR(fChallengeRating);
        }
        else if (m_nNumberSpawned == 0)
        {
            CScriptEvent* pEvent = new CScriptEvent;

            pEvent->m_nType = 0x15;
            pEvent->SetObjectID(0, m_nId);
            pExoApp = g_pAppManager->m_pServerExoApp;
            pExoApp->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, m_nId, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT,
                pEvent);
            if (m_bReset == 1)
            {
                pExoApp = g_pAppManager->m_pServerExoApp;
                pExoApp->GetWorldTimer()->GetWorldTime(&m_nLastSpawnDay,
                                                       &m_nLastSpawnTime);
            }
            for (nIndex = 0; nIndex < m_nSpawnEntryCount; nIndex++)
            {
                m_pSpawnEntries[nIndex].m_bSelected = 0;
                m_pSpawnEntries[nIndex].m_bSpawned = 0;
            }
            SetActive(0);
            m_bStarted = 0;
        }
    }
}

float CSWSEncounter::CalculatePointsFromCR(float fChallengeRating)
{
    if (fChallengeRating > 0.0f && fChallengeRating < 1.0f)
    {
        if (fChallengeRating >= m_plstFractionalCR->element[0])
        {
            return 1.0f;
        }
        if (fChallengeRating >= m_plstFractionalCR->element[1])
        {
            return (float)pow(1.5, -1.0);
        }
        if (fChallengeRating >= m_plstFractionalCR->element[2])
        {
            return (float)pow(1.5, -2.0);
        }
        if (fChallengeRating >= m_plstFractionalCR->element[3])
        {
            return (float)pow(1.5, -3.0);
        }
        return (float)pow(1.5, -4.0);
    }
    if (fChallengeRating >= 0.0f && fChallengeRating <= 50.0f)
    {
        return m_sPowerLookup[(int)fChallengeRating];
    }
    return (float)pow(1.5f, fChallengeRating);
}

void CSWSEncounter::TallyEnemyRadiusPoints()
{
    CSWSArea* pArea = GetArea();
    int nIndex;
    int nLevel;
    Vector vOrigin;
    Vector vPosition;

    if (pArea != NULL)
    {
        m_fAreaPoints = 0.0f;
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                m_oidLastEntered);
        if (pCreature != NULL)
        {
            vOrigin = pCreature->m_vPosition;
            if (pArea->GetFirstObjectIndiceByX(
                    &nIndex, vOrigin.m_fX - ENCOUNTER_ENEMY_SEARCH_RADIUS))
            {
                while (nIndex < pArea->m_lstObjectIds.num)
                {
                    unsigned long oidCreature = pArea->m_lstObjectIds[nIndex];
                    pCreature =
                        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
                    if (pCreature == NULL)
                    {
                        nIndex++;
                        continue;
                    }
                    vPosition = pCreature->m_vPosition;
                    if (vPosition.m_fX > vOrigin.m_fX + ENCOUNTER_ENEMY_SEARCH_RADIUS)
                    {
                        break;
                    }
                    if (!(pCreature->GetAIStateReputation(GetId()) ==
                            CSWSObject::AI_STATE_REPUTATION_ENEMY &&
                        pCreature->GetDead() != 1 &&
                        pCreature->GetIsPCDying() != 1 &&
                        vPosition.m_fY >= vOrigin.m_fY - ENCOUNTER_ENEMY_SEARCH_RADIUS &&
                        vPosition.m_fY <= vOrigin.m_fY + ENCOUNTER_ENEMY_SEARCH_RADIUS))
                    {
                        nIndex++;
                        continue;
                    }
                    nLevel = pCreature->m_pStats->GetLevel(0);
                    if (pCreature->m_nAreaWaitState == 0)
                    {
                        nLevel /= 2;
                    }
                    m_fAreaPoints += CalculatePointsFromCR((float)nLevel);
                    nIndex++;
                }
            }
        }
    }
}
// PC 0x0056CEC0
void CSWSEncounter::SpawnCreatureFromSpawnList()
{
    if (m_lstSpawnList.num == 0)
        return;

    CSWSArea* pArea = GetArea();
    if (pArea == NULL)
        return;

    int nSpawnPoint = 0;
    int bUseSpawnPoint = 0;
    Vector vSpawnPoint;
    Vector vOrientation(1.0f, 0.0f, 0.0f);
    if (m_nSpawnPointCount > 0)
    {
        bUseSpawnPoint = 1;
        float fFarthestDistance = 0.0f;
        unsigned long oidEntered = m_oidLastEntered;
        CSWSCreature* pEntered =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                oidEntered);
        if (pEntered != NULL)
        {
            Vector vEnteredPosition = pEntered->m_vPosition;
            for (int i = 0; i < m_nSpawnPointCount; i++)
            {
                Vector vDifference = m_pSpawnPoints[i].m_vPosition -
                                     vEnteredPosition;
                float fDistanceSquared = vDifference * vDifference;
                if (fDistanceSquared > fFarthestDistance * fFarthestDistance)
                {
                    fFarthestDistance = (float)sqrt(fDistanceSquared);
                    nSpawnPoint = i;
                }
            }
            vSpawnPoint = m_pSpawnPoints[nSpawnPoint].m_vPosition;
            float fAngle = ENCOUNTER_SPAWN_ORIENTATION_OFFSET -
                           m_pSpawnPoints[nSpawnPoint].m_fOrientation;

            normalize(Vector((float)cos(fAngle), (float)sin(fAngle), 0.0f));
        }
    }

    Vector vLocation;
    CSWSCreature* pCreature = new CSWSCreature(OBJECT_INVALID, 0);
    if (!pCreature->LoadFromTemplate(m_lstSpawnList[0]->m_cResRef, 0))
    {
        delete pCreature;
        return;
    }
    pCreature->m_oidSpawnedBy = GetId();
    pCreature->m_nAreaWaitState = 0;

    int bFoundLocation = 0;
    float fX, fY, fZ;
    if (bUseSpawnPoint)
    {
        if (pArea->ComputeSafeLocation(vSpawnPoint,
                ENCOUNTER_SAFE_SPAWN_RADIUS, pCreature->GetPathfindInformation(),
                0, &vLocation) == 1)
        {
            fX = vLocation.m_fX;
            fY = vLocation.m_fY;
            fZ = pArea->ComputeHeight(vLocation, 1, NULL);
            CExoString sResRef;
            m_lstSpawnList[0]->m_cResRef.CopyToString(sResRef);
            sResRef.CStr();
            bFoundLocation = 1;
        }
        else
        {
            CExoString sResRef;
            m_lstSpawnList[0]->m_cResRef.CopyToString(sResRef);
            sResRef.CStr();
        }
    }
    if (!bUseSpawnPoint || !bFoundLocation)
    {
        if (pArea->ComputeNonVisibleLocation(m_vPosition,
                pCreature->GetPathfindInformation(), 0, &vLocation,
                ENCOUNTER_NON_VISIBLE_SPAWN_RADIUS) == 1)
        {
            fX = vLocation.m_fX;
            fY = vLocation.m_fY;
            fZ = pArea->ComputeHeight(vLocation, 1, NULL);
            CExoString sResRef;
            m_lstSpawnList[0]->m_cResRef.CopyToString(sResRef);
            sResRef.CStr();
            bFoundLocation = 1;
        }
        else if (vLocation.m_fX == AREA_NON_VISIBLE_INVALID_COORDINATE &&
                 vLocation.m_fY == AREA_NON_VISIBLE_INVALID_COORDINATE &&
                 vLocation.m_fZ == AREA_NON_VISIBLE_INVALID_COORDINATE)
        {
            CExoString sResRef;
            m_lstSpawnList[0]->m_cResRef.CopyToString(sResRef);
            sResRef.CStr();
        }
        else
        {
            fX = vLocation.m_fX;
            fY = vLocation.m_fY;
            fZ = pArea->ComputeHeight(vLocation, 1, NULL);
            CExoString sResRef;
            m_lstSpawnList[0]->m_cResRef.CopyToString(sResRef);
            sResRef.CStr();
            bFoundLocation = 1;
        }
    }
    if (bFoundLocation)
    {
        pCreature->AddToArea(pArea, fX, fY, fZ, 0, 0);
        if (pArea->m_nPlayersInArea > 0)
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->SetAILevel(
                pCreature, CServerAIMaster::AI_LEVEL_OCCUPIED_AREA_CREATURE);
        else
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->SetAILevel(
                pCreature, CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE);
        if (bUseSpawnPoint)
            pCreature->SetOrientation(vOrientation);
        m_nNumberSpawned++;
        m_fSpawnPoolActive += m_lstSpawnList[0]->m_fSpawnPoolCost;
        m_fSpawnPool -= m_lstSpawnList[0]->m_fSpawnPoolCost;
        if (m_fSpawnPool < 0.0f)
            m_fSpawnPool = 0.0f;
        m_lstSpawnList.DelIndex(0);
    }
    else
    {
        delete pCreature;
        CExoString sResRef;
        m_lstSpawnList[0]->m_cResRef.CopyToString(sResRef);
        sResRef.CStr();
    }
}
// PC 0x0056D3B0
void CSWSEncounter::ClearSpawnList()
{
    int nIndex;
    CEncounterListEntry* pEntry;

    if (m_lstSpawnList.num > 0)
    {
        for (nIndex = m_lstSpawnList.num - 1; nIndex >= 0; nIndex--)
        {
            pEntry = m_lstSpawnList.element[nIndex];
            m_lstSpawnList.DelIndex(nIndex);
            if (pEntry != NULL)
            {
                delete pEntry;
            }
        }
    }
}
// PC 0x0056D420
void CSWSEncounter::RemoveFromArea()
{
    if (GetArea() != NULL)
    {
        GetArea()->RemoveObjectFromArea(m_nId);
        GetArea()->RemoveSubArea(this);
    }

    SetArea(NULL);

    Vector vPosition;

    SetPosition(vPosition, 1, 1, 0);
}

CExoArrayList<float> CSWSEncounter::m_sPowerLookup;

static inline void NwsencounterTrace1()
{
}
// PC 0x0056E580
void CSWSEncounter::SetActive(int bActive)
{
    int nActive;

    nActive = bActive;
    m_bActive = bActive;
    if (bActive != 0)
    {
        SpawnIfAppropriate(OBJECT_INVALID, -1.0f);
        return;
    }
}

#ifdef _XBOX

int CSWSEncounter::GetInActivateArea(const Vector& vPosition)
{
    int nIntersections = 0;
    int nIndex = 0;
    int nLast = m_nActivateAreaVertexCount - 1;

    for (nIndex = 0; nIndex < m_nActivateAreaVertexCount; nIndex++)
    {
        int nFirst = m_pActivateAreaIndices[nIndex];
        int nSecond = m_pActivateAreaIndices[nIndex != nLast ? nIndex + 1 : 0];

        if (m_pActivateAreaVertices[nSecond].m_fX != m_pActivateAreaVertices[nFirst].m_fX)
        {
            float fY = m_pActivateAreaVertices[nFirst].m_fY;
            fY = fY - (m_pActivateAreaVertices[nFirst].m_fX - vPosition.m_fX) *
                ((m_pActivateAreaVertices[nSecond].m_fY - fY) /
                 (m_pActivateAreaVertices[nSecond].m_fX - m_pActivateAreaVertices[nFirst].m_fX));

            if (fY > vPosition.m_fY)
            {
                float fX = vPosition.m_fX;
                if (fabs(m_pActivateAreaVertices[nSecond].m_fY - m_pActivateAreaVertices[nFirst].m_fY) >
                    fabs(m_pActivateAreaVertices[nSecond].m_fX - m_pActivateAreaVertices[nFirst].m_fX))
                {
                    if (m_pActivateAreaVertices[nSecond].m_fY > m_pActivateAreaVertices[nFirst].m_fY)
                    {
                        if (fY >= m_pActivateAreaVertices[nFirst].m_fY && fY < m_pActivateAreaVertices[nSecond].m_fY)
                            nIntersections++;
                    }
                    else if (m_pActivateAreaVertices[nFirst].m_fY > m_pActivateAreaVertices[nSecond].m_fY &&
                             fY > m_pActivateAreaVertices[nSecond].m_fY && fY <= m_pActivateAreaVertices[nFirst].m_fY)
                    {
                        nIntersections++;
                    }
                }
                else
                {
                    if (m_pActivateAreaVertices[nSecond].m_fX > m_pActivateAreaVertices[nFirst].m_fX)
                    {
                        if (fX >= m_pActivateAreaVertices[nFirst].m_fX && fX < m_pActivateAreaVertices[nSecond].m_fX)
                            nIntersections++;
                    }
                    else if (m_pActivateAreaVertices[nFirst].m_fX > m_pActivateAreaVertices[nSecond].m_fX &&
                             fX > m_pActivateAreaVertices[nSecond].m_fX && fX <= m_pActivateAreaVertices[nFirst].m_fX)
                    {
                        nIntersections++;
                    }
                }
            }
        }

        if (m_pActivateAreaVertices[nSecond].m_fX == vPosition.m_fX &&
            (m_pActivateAreaVertices[nSecond].m_fY > vPosition.m_fY ||
             m_pActivateAreaVertices[nFirst].m_fY >= vPosition.m_fY))
        {
            nIntersections++;
        }
    }

    return nIntersections % 2 == 1;
}
#endif
