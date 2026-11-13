#include "swparty.h"

#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "serverexoapp.h"
#include "partytables.h"
#include "nwsobject.h"
#include "nwsarea.h"
#include "nwscreaturestats.h"
#include "nwmessage.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "pathfindinfo.h"
#include "nwscript.h"
#include <math.h>
#include "nwsplayer.h"
#include "swguimaininterfacepc2.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "aurora.h"

#define VIDEO_EFFECT_FORCESIGHT 4
#define VIDEO_EFFECT_FURY_1 7
#define VIDEO_EFFECT_FURY_2 8
#define VIDEO_EFFECT_FURY_3 9

int g_nPartyUnused = 0;

int FollowMode = -1;
int g_bEnablePartySmoothing = 1;
// PC 0x004015A0
CSWParty::~CSWParty()
{
}
// PC 0x00689E30
void CSWPartyMemberData::SetCombatMessage(unsigned long nMessage)
{
    m_nCombatMessage = nMessage;
    if (m_oidCreature ==
        g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetCombatMessage(
            m_nCombatMessage);
    }
}
// PC 0x00689E70
int CSWParty::IsPartyMember(unsigned long oidCreature)
{
    int nIndex;

    for (nIndex = 0; nIndex < m_nCharacters; ++nIndex)
    {
        if (m_aPartyMemberData[nIndex].m_oidCreature == oidCreature)
        {
            return 1;
        }
    }

    return 0;
}
// PC 0x00689EA0
CSWCCreature* CSWParty::RemoveCharacter(int nIndex)
{
    if (g_pAppManager == NULL)
    {
        return NULL;
    }

    unsigned long oidCreature = m_aPartyMemberData[nIndex].m_oidCreature;
    CSWCCreature* pCreature = g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
        oidCreature);

    for (int nMember = nIndex; nMember < MAX_MEMBERS - 1; ++nMember)
    {
        m_aPartyMemberData[nMember].m_oidCreature =
            m_aPartyMemberData[nMember + 1].m_oidCreature;
    }

    m_aPartyMemberData[MAX_MEMBERS - 1].m_oidCreature = OBJECT_INVALID;
    --m_nCharacters;
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMainInterface->ResetPartyMemberItemUse();
    return pCreature;
}
// PC 0x00689F30
CSWCCreature* CSWParty::GetCharacter(int nIndex)
{
    unsigned long oidCreature;
    CClientExoApp* pClientExoApp;

    if (nIndex >= 0 && nIndex < m_nCharacters)
    {
        oidCreature = m_aPartyMemberData[nIndex].m_oidCreature;
        pClientExoApp = g_pAppManager->m_pClientExoApp;

        return pClientExoApp->GetCreatureByGameObjectID(oidCreature);
    }

    return NULL;
}
// PC 0x00689F60
void CSWParty::InvertFormation()
{
    Vector vOffset;

    vOffset = m_aPartyMemberData[1].m_vFormationOffset;
    vOffset.m_fX = -vOffset.m_fX;
    m_aPartyMemberData[1].m_vFormationOffset = vOffset;
    vOffset = m_aPartyMemberData[2].m_vFormationOffset;
    vOffset.m_fX = -vOffset.m_fX;
    m_aPartyMemberData[2].m_vFormationOffset = vOffset;
    m_bFormationInverted = !m_bFormationInverted;
}
// PC 0x00689FD0
void CSWParty::SetFormation(int nFormation)
{
    int bChanged = FALSE;

    if (nFormation == m_nFormation || nFormation >= 3)
    {
        return;
    }

    if (nFormation >= 0)
    {
        m_aPartyMemberData[1].m_vFormationOffset = m_aRightFormation[nFormation][0];
        m_aPartyMemberData[2].m_vFormationOffset = m_aRightFormation[nFormation][1];
        m_fFormationAngle = m_fFormationRange[nFormation];
        bChanged = TRUE;
    }
    else if (nFormation == FORMATION_NEXT)
    {
        nFormation = m_nFormation + 1;
        if (nFormation >= 3)
        {
            nFormation = -3;
        }
        while (nFormation < 0 && !m_bLeftFormationAvailable[-nFormation - 1])
        {
            ++nFormation;
        }
        SetFormation(nFormation);
    }
    else if (nFormation >= -3 && m_bLeftFormationAvailable[-nFormation - 1])
    {
        m_aPartyMemberData[1].m_vFormationOffset = m_aLeftFormation[-nFormation - 1][0];
        m_aPartyMemberData[2].m_vFormationOffset = m_aLeftFormation[-nFormation - 1][1];
        m_fFormationAngle = m_fFormationRange[-nFormation - 1];
        bChanged = TRUE;
    }

    if (bChanged)
    {
        m_nFormation = nFormation;
        if (m_bFormationInverted)
        {
            Vector vOffset = m_aPartyMemberData[1].m_vFormationOffset;
            m_aPartyMemberData[1].m_vFormationOffset = m_aPartyMemberData[2].m_vFormationOffset;
            m_aPartyMemberData[2].m_vFormationOffset = vOffset;
        }
    }
}
// PC 0x0068A130
Vector CSWParty::GetWorldFormationPosition(const Vector& vPosition,
                                          const Vector& vFacing, int nMember)
{
    Vector vWorldPosition;

    if (nMember == 0)
    {
        vWorldPosition = vPosition;
    }
    else if (nMember > 0 && nMember < 3)
    {
        Quaternion qRotation = RotationArc(Vector(0.0f, 1.0f, 0.0f), vFacing);
        vWorldPosition = vPosition +
            qRotation * m_aPartyMemberData[nMember].m_vFormationOffset;
    }

    return vWorldPosition;
}
// PC 0x0068A340
int CSWParty::GetIndex(unsigned long oidCreature)
{
    int nIndex;

    if (oidCreature != OBJECT_INVALID)
    {
        for (nIndex = 0; nIndex < m_nCharacters; ++nIndex)
        {
            if (m_aPartyMemberData[nIndex].m_oidCreature == oidCreature)
            {
                return nIndex;
            }
        }
    }

    return -1;
}
// PC 0x0068A370
int CSWParty::RemoveCharacter(unsigned long oidCreature)
{
    int nIndex;

    nIndex = GetIndex(oidCreature);
    if (nIndex != -1 && RemoveCharacter(nIndex))
    {
        return 1;
    }

    return 0;
}
// PC 0x0068A3C0
void CSWParty::ClearParty()
{
    CSWPartyMemberData* pPartyMember;
    int nIndex;

    nIndex = 0;
    if (m_nCharacters > 0)
    {
        pPartyMember = m_aPartyMemberData;
        do
        {
            pPartyMember->m_oidCreature = OBJECT_INVALID;
            ++nIndex;
            ++pPartyMember;
        } while (nIndex < m_nCharacters);
    }

    m_nCharacters = 0;
}
// PC 0x0068A3F0
float CSWParty::GetFollowRange()
{
    float fRange;
    int nFollowState;

    fRange = 5.0f;
    nFollowState = g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nFollowState;
    if (FollowMode != -1)
    {
        nFollowState = FollowMode;
    }

    switch (nFollowState)
    {
        case 0:
        {
            CExoString sColumn("PrimaryRange");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            C2DA* pRanges = p2DArrays->m_pRanges;
            pRanges->GetFLOATEntry(14, sColumn, &fRange);
            break;
        }

        case 1:
        {
            CExoString sColumn("PrimaryRange");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            C2DA* pRanges = p2DArrays->m_pRanges;
            pRanges->GetFLOATEntry(15, sColumn, &fRange);
            break;
        }
    }

    return fRange;
}
// PC 0x0068A4E0
int CSWParty::PartyHasRace(unsigned short nRace)
{
    for (int nIndex = 0; nIndex < m_nCharacters; ++nIndex)
    {
        if (GetCharacter(nIndex)->GetStats()->m_nRace == nRace)
            return 1;
    }
    return 0;
}
// PC 0x0068A550
int CSWPartyFollowDataArray::Reset(const Vector& vPosition, float fFacing)
{
    int nIndex;

    for (nIndex = 0; nIndex < 100; ++nIndex)
    {
        m_aPathData[nIndex].Reset(vPosition, fFacing);
    }

    m_nPathDataCount = 0;
    m_nCurrentPathData = 0;
    return 0;
}
// PC 0x0068A6B0
CSWPartyFollowData* CSWPartyFollowDataArray::GetPathData(int nIndex)
{
    if (nIndex < 0 || nIndex >= 100)
    {
        CExoString sError;
        sError.Format("PathFollowData requesting bad data position %d", nIndex);
    EXOXBOXASSERT(nIndex >= 0 && nIndex < 100);
    }

    return &m_aPathData[nIndex];
}
// PC 0x0068A730
int CSWPartyFollowDataArray::FindClosestPartyMemeber_Current()
{
    int nFirstPathIndex = -1;
    int nSecondPathIndex = -1;
    int nCurrentPathIndex;

    if (m_pParty->m_aPartyMemberData[1].m_oidCreature != OBJECT_INVALID)
    {
        nFirstPathIndex = m_pParty->m_aPartyMemberData[1].m_nFollowPathIndex;
    }

    if (m_pParty->m_aPartyMemberData[2].m_oidCreature != OBJECT_INVALID)
    {
        nSecondPathIndex = m_pParty->m_aPartyMemberData[2].m_nFollowPathIndex;
    }

    nCurrentPathIndex = m_nCurrentPathData;
    if (nCurrentPathIndex > nFirstPathIndex && nFirstPathIndex != -1)
    {
        nFirstPathIndex += 100;
    }

    if (nCurrentPathIndex > nSecondPathIndex && nSecondPathIndex != -1)
    {
        nSecondPathIndex += 100;
    }

    return (nFirstPathIndex <= nSecondPathIndex) + 1;
}
// PC 0x0068A790
int CSWPartyFollowDataArray::FindFurthestPartyMemeber_Current()
{
    int nFirstPathIndex = -1;
    int nSecondPathIndex = -1;
    int nCurrentPathIndex;

    if (m_pParty->m_aPartyMemberData[1].m_oidCreature != OBJECT_INVALID)
    {
        nFirstPathIndex = m_pParty->m_aPartyMemberData[1].m_nFollowPathIndex;
    }

    if (m_pParty->m_aPartyMemberData[2].m_oidCreature != OBJECT_INVALID)
    {
        nSecondPathIndex = m_pParty->m_aPartyMemberData[2].m_nFollowPathIndex;
    }

    nCurrentPathIndex = m_nCurrentPathData;
    if (nCurrentPathIndex > nFirstPathIndex && nFirstPathIndex != -1)
    {
        nFirstPathIndex += 100;
    }

    if (nCurrentPathIndex > nSecondPathIndex && nSecondPathIndex != -1)
    {
        nSecondPathIndex += 100;
    }

    return (nFirstPathIndex >= nSecondPathIndex) + 1;
}
// PC 0x0068A7F0
float CSWPartyFollowDataArray::PointOnLineDistanceFromLeader(int nMember,
                                                             int bSquared)
{
    CSWCCreature* pLeader = m_pParty->GetCharacter(0);
    Vector vDelta;
#ifdef _XBOX
    vDelta = m_pParty->GetMemberData(nMember)->m_vFollowPosition -
                    pLeader->m_vPosition;
#else
    vDelta = m_pParty->m_aPartyMemberData[nMember].m_vFollowPosition -
                    pLeader->m_vPosition;
#endif
    float fDistanceSquared = vDelta.m_fZ * vDelta.m_fZ + vDelta.m_fX * vDelta.m_fX +
                             vDelta.m_fY * vDelta.m_fY;

    if (bSquared)
    {
        return fDistanceSquared;
    }

    return (float)sqrt(fDistanceSquared);
}
// PC 0x0068A870
float CSWPartyFollowDataArray::AdvancePointOnPath(float fDistance, int nPathIndex,
                                                const Vector& vStart, int nStartIndex,
                                                const Vector& vEnd, int nEndIndex,
                                                Vector& vPosition, int& nPositionIndex)
{

    float fDistanceLeft = fDistance;
    int nCurrentIndex = nStartIndex;
    Vector vCurrent = vStart;
    int bReachedEnd = 0;
    vPosition = vStart;
    while (fDistanceLeft > 0.0f && !bReachedEnd)
    {
        Vector vDelta = GetPathData(nCurrentIndex)->m_vPosition - vCurrent;
        float fSegmentLength = magnitude(vDelta);
        vDelta = normalize(vDelta);
        float fStep = fSegmentLength < fDistanceLeft ? fSegmentLength : fDistanceLeft;
        vPosition += fStep * vDelta;

        Vector vEndDelta = vEnd - vStart;
        if (vEndDelta * vEndDelta <= fDistance * fDistance)
        {
            vPosition = vEnd;
            nPositionIndex = nEndIndex;
            magnitude(vEnd - vStart);
            nCurrentIndex = nEndIndex;
            break;
        }

        if (fDistanceLeft >= fSegmentLength)
        {
            vCurrent = GetPathData(nCurrentIndex)->m_vPosition;
            ++nCurrentIndex;
            if (nCurrentIndex >= 100)
            {
                nCurrentIndex = 0;
            }
            if (nCurrentIndex == m_nCurrentPathData - 1)
            {
                bReachedEnd = 1;
            }
            if (nCurrentIndex == 99 && m_nCurrentPathData == 0)
            {
                bReachedEnd = 1;
            }
        }
        fDistanceLeft -= fStep;
    }
    nPositionIndex = nCurrentIndex;
    return fDistanceLeft;
}
// PC 0x0068AEC0
void CSWParty::SetLeader(int nIndex)
{
    unsigned long oidOldLeader = m_aPartyMemberData[0].m_oidCreature;
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    unsigned long oidLeader = OBJECT_INVALID;
    float fLeaderPrimaryRange;
    float fLeaderSecondaryRange;
    float fMemberPrimaryRange;
    float fMemberSecondaryRange;

    if (m_nCharacters <= 1 || nIndex == 0)
    {
        return;
    }

    if (nIndex == LEADER_NEXT)
    {
        oidLeader = m_aPartyMemberData[1].m_oidCreature;
    }
    else if (nIndex == LEADER_PLAYER)
    {
        for (int nMember = 0; nMember < MAX_MEMBERS; ++nMember)
        {
            unsigned long oidMember = m_aPartyMemberData[nMember].m_oidCreature;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                pServer->ClientToServerObjectId(oidMember));
            if (pCreature != NULL && pCreature->GetStats()->m_bIsPC)
            {
                oidLeader = m_aPartyMemberData[nMember].m_oidCreature;
                break;
            }
        }
    }
    else
    {
        if (nIndex <= 0 || nIndex >= m_nCharacters)
        {
            return;
        }
        oidLeader = m_aPartyMemberData[nIndex].m_oidCreature;
    }

    if (oidLeader == OBJECT_INVALID)
    {
        return;
    }

    unsigned long oidCreature = OBJECT_INVALID;
    if (GetCharacter(0) != NULL && GetCharacter(0)->GetServerCreature() != NULL)
    {
        oidCreature = GetCharacter(0)->GetServerCreature()->m_oidAttackTarget;
    }
    m_aPartyMemberData[0].m_oidCombatTarget = oidCreature;
    g_pAppManager->GetClientExoApp()->SetLastTarget(oidCreature, 0);

    while ((oidCreature = m_aPartyMemberData[0].m_oidCreature) != oidLeader)
    {
        unsigned long oidFirst = m_aPartyMemberData[0].m_oidCreature;
        unsigned long oidFirstTarget = m_aPartyMemberData[0].m_oidCombatTarget;
        int nMember;
        for (nMember = 0; nMember < MAX_MEMBERS - 1; ++nMember)
        {
            unsigned long oidNext = m_aPartyMemberData[nMember + 1].m_oidCreature;
            m_aPartyMemberData[nMember].m_oidCreature = oidNext;
            m_aPartyMemberData[nMember].m_oidCombatTarget =
                m_aPartyMemberData[nMember + 1].m_oidCombatTarget;
            if (oidNext == OBJECT_INVALID)
            {
                break;
            }
        }
        m_aPartyMemberData[nMember].m_oidCreature = oidFirst;
        m_aPartyMemberData[nMember].m_oidCombatTarget = oidFirstTarget;
    }

    CSWCCreature* pLeader = pClient->GetCreatureByGameObjectID(oidLeader);
    if (pLeader != NULL)
    {
        pClient->UpdateRoom(pLeader->m_vPosition);
    }

    if (m_bApplyLeaderRanges)
    {
        {
            CExoString sColumn("PrimaryRange");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            C2DA* pRanges = p2DArrays->m_pRanges;
            pRanges->GetFLOATEntry(CSWRules::RANGE_PARTY_LEADER, sColumn, &fLeaderPrimaryRange);
        }
        {
            CExoString sColumn("SecondaryRange");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            C2DA* pRanges = p2DArrays->m_pRanges;
            pRanges->GetFLOATEntry(CSWRules::RANGE_PARTY_LEADER, sColumn, &fLeaderSecondaryRange);
        }
        {
            CExoString sColumn("PrimaryRange");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            C2DA* pRanges = p2DArrays->m_pRanges;
            pRanges->GetFLOATEntry(CSWRules::RANGE_PARTY_MEMBER, sColumn, &fMemberPrimaryRange);
        }
        {
            CExoString sColumn("SecondaryRange");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            C2DA* pRanges = p2DArrays->m_pRanges;
            pRanges->GetFLOATEntry(CSWRules::RANGE_PARTY_MEMBER, sColumn, &fMemberSecondaryRange);
        }

        CSWSPlayer* pPlayer = (CSWSPlayer*)pServer->GetPlayerList()->GetHead();
        unsigned long oidFirstCreature = m_aPartyMemberData[0].m_oidCreature;
        CSWSCreature* pServerLeader = pServer->GetCreatureByGameObjectID(
            pServer->ClientToServerObjectId(oidFirstCreature));
        pServer->GetPartyTable()->SetLeader(pServer->ClientToServerObjectId(oidLeader));
        pPlayer->SetGameObject((CSWSObject*)pServerLeader);
        pServerLeader->m_fSpotCheckDistance = fLeaderPrimaryRange;
        pServerLeader->m_fListenCheckDistance = fLeaderSecondaryRange;

        CSWCCreature* pOldLeader =
            g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidOldLeader);
        if (pOldLeader != NULL && oidOldLeader != oidLeader)
        {
            CSWSCreature* pServerOldLeader = pOldLeader->GetServerCreature();
            if (pServerOldLeader != NULL)
            {
                CExoArrayList<CSWVisibilityNode*>* pVisibility =
                    &pServerLeader->m_lstVisibilityList;
                CExoArrayList<CSWVisibilityNode*>* pOldVisibility =
                    &pServerOldLeader->m_lstVisibilityList;
                if (pVisibility->num > 0)
                {
                    for (int nNode = pVisibility->num - 1; nNode >= 0; nNode--)
                    {
                        CSWVisibilityNode* pNode = pVisibility->element[nNode];
                        pVisibility->DelIndex(nNode);
                        if (pNode != NULL)
                        {
                            delete pNode;
                        }
                    }
                }
                pServerLeader->SetVisibilityList(pOldVisibility);
                pServerOldLeader->m_fSpotCheckDistance = fMemberPrimaryRange;
                pServerOldLeader->m_fListenCheckDistance = fMemberSecondaryRange;
                pServerOldLeader->SetVisibilityList(NULL);
                pServerOldLeader->UpdateVisibleList(0);
            }
        }

        if (oidOldLeader != m_aPartyMemberData[0].m_oidCreature)
        {
            CExoString sScript("k_hen_leadchng");
            unsigned long oidClientObject = m_aPartyMemberData[0].m_oidCreature;
            unsigned long oidScriptObject = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(
                oidClientObject);
            g_pVirtualMachine->RunScript(&sScript, oidScriptObject,
                                         oidScriptObject != OBJECT_INVALID);
            m_aPartyMemberData[0].m_bWaitingForLeader = 1;
            m_aPartyMemberData[1].m_bWaitingForLeader = 1;
            m_aPartyMemberData[2].m_bWaitingForLeader = 1;
        }

        unsigned long oidCombatTarget = m_aPartyMemberData[0].m_oidCombatTarget;
        g_pAppManager->GetClientExoApp()->SetLastTarget(oidCombatTarget, 0);
        if (pLeader != NULL)
        {
            g_pAppManager->GetClientExoApp()->SetCombatMode(pLeader->m_bCombatMode);
            if (pLeader->m_bCombatMode)
            {
                unsigned long nCombatMessage = m_aPartyMemberData[0].m_nCombatMessage;
                if (nCombatMessage != CSWGuiMainInterface::STRREF_COMBAT_MODE_ENGAGED)
                {
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetCombatMessage(
                        nCombatMessage);
                }
            }
        }
    }
}
// PC 0x0068B400
void CSWParty::RecaulateFollowPoint()
{
    Vector vPosition;
    int nPositionIndex;

    if (m_cFollowPaths.GetPointBackFromStart(
            GetFollowRange() - m_fFormationAngle, vPosition, nPositionIndex, 1))
    {
        m_aPartyMemberData[1].m_vFormationPosition = vPosition;
        m_aPartyMemberData[1].m_nFormationPathIndex = nPositionIndex;
    }

    if (m_cFollowPaths.GetPointBackFromStart(
            GetFollowRange() + m_fFormationAngle, vPosition, nPositionIndex, 2))
    {
        m_aPartyMemberData[2].m_vFormationPosition = vPosition;
        m_aPartyMemberData[2].m_nFormationPathIndex = nPositionIndex;
    }
}
// PC 0x0068AAE0
int CSWPartyFollowDataArray::GetPointBackFromStart(float fDistance,
                                                 Vector& vPosition,
                                                 int& nPositionIndex,
                                                 int nMember)
{

    int nPathIndex = m_nCurrentPathData - 1;
    if (nPathIndex < 0)
    {
        nPathIndex += 100;
    }

    if (m_nPathDataCount < 3)
    {
        nPositionIndex = 0;
        vPosition = Vector(0.0f, 0.0f, 0.0f);
        return 0;
    }

    Vector vCurrent = GetPathData(nPathIndex)->m_vPosition;
    int nPointsLeft = m_nPathDataCount;
    int nFollowPathIndex = -1;
    if (nMember != 0)
    {
        EXOXBOXASSERT(nMember >= 0 && nMember < 3);
        nFollowPathIndex = m_pParty->m_aPartyMemberData[nMember].m_nFollowPathIndex;
        EXOXBOXASSERT(nMember >= 0 && nMember < 3);
    }

    float fDistanceLeft = fDistance;
    nPositionIndex = nPathIndex;
    while (nPointsLeft >= 0 && fDistanceLeft > 0.0f)
    {
        if (nMember != 0 && nPathIndex == nFollowPathIndex)
        {
            nPositionIndex = nPathIndex;
            vPosition = vCurrent;
            return 1;
        }

        nPositionIndex = nPathIndex;
        --nPathIndex;
        if (nPathIndex < 0)
        {
            nPathIndex += 100;
        }
        --nPointsLeft;

        Vector vDelta = GetPathData(nPathIndex)->m_vPosition - vCurrent;
        float fLengthSquared = vDelta * vDelta;
        if (!(fLengthSquared >= fDistanceLeft * fDistanceLeft))
        {
            fDistanceLeft -= (float)sqrt(fLengthSquared);
            vCurrent = GetPathData(nPathIndex)->m_vPosition;
        }
        else
        {
            vPosition = vCurrent + fDistanceLeft * normalize(vDelta);
            return 1;
        }
    }

    if (nPointsLeft >= 0)
    {
        return 1;
    }

    vPosition = Vector(0.0f, 0.0f, 0.0f);
    return 0;
}
// PC 0x0068AD40
int CSWParty::IsPartyTogether()
{
    CSWSCreature* pLeader;
    CSWSCreature* pMember;
    int nIndex;

    pLeader = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
        g_pAppManager->m_pServerExoApp->GetFirstPCObject());
    if (pLeader == NULL)
    {
        return 0;
    }
    if (pLeader->GetCurrentHitPoints(0) <= 0)
    {
        return 0;
    }

    nIndex = 0;
    if (m_nCharacters > 0)
    {
        CSWPartyMemberData* pMemberData = m_aPartyMemberData;
        for (; nIndex < m_nCharacters; ++nIndex, ++pMemberData)
        {
            EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
            pMember = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                g_pAppManager->m_pServerExoApp->ClientToServerObjectId(
                    pMemberData->m_oidCreature));
            if (pMember != NULL && pMember->GetCurrentHitPoints(0) <= 0)
            {
                return 0;
            }
        }
    }

    return 1;
}
// PC 0x0068ADE0
int CSWParty::IsPartyInCombat()
{
    CSWSCreature* pLeader;
    CSWSCreature* pMember;
    int nIndex;

    unsigned long oidPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreatureId();
    unsigned long oidServer = g_pAppManager->m_pServerExoApp->ClientToServerObjectId(oidPlayer);
    pLeader = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidServer);
    if (pLeader == NULL)
    {
        return 0;
    }

    if (pLeader->GetCombatState() && pLeader->GetItemUseState() == 1)
    {
        return 1;
    }

    nIndex = 0;
    if (m_nCharacters > 0)
    {
        CSWPartyMemberData* pMemberData = m_aPartyMemberData;
        for (; nIndex < m_nCharacters; ++nIndex, ++pMemberData)
        {
            pMember = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                g_pAppManager->m_pServerExoApp->ClientToServerObjectId(
                    pMemberData->m_oidCreature));
            if (pMember != NULL && pMember->GetCombatState() &&
                pMember->GetItemUseState() == 1)
            {
                return 1;
            }
        }
    }

    return 0;
}
// PC 0x0068AEA0
CSWCCreature* CSWParty::GetPlayerCharacter()
{
    return GetCharacter(0);
}
// PC 0x0068B4D0
CSWPartyFollowDataArray::CSWPartyFollowDataArray()
{
    Vector vPosition;

    Reset(vPosition, 0.0f);
}
#define SWPARTY_DEBUG_LINE_HEIGHT_STEP 0.01f
static inline void SwpartyDebugLine(const Vector&, const Vector&, int)
{
}
// PC 0x0068B530
void CSWPartyFollowDataArray::SmoothFollowPath()
{
    static int OFF = 0;

    Vector aSmoothPath[201];
    int nEndIndex = m_nCurrentPathData - 1;
    if (nEndIndex < 0)
    {
        nEndIndex += 100;
    }

    unsigned long oidLeader = m_pParty->m_aPartyMemberData[0].m_oidCreature;
    unsigned long oidServerLeader =
        g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidLeader);
    CSWSCreature* pLeader =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidServerLeader);
    if (pLeader == NULL)
    {
        return;
    }

    CResRef cMemberAreaResRef;
    CWalkHitInfo cMemberWalkHitInfo;
    unsigned long oidServerMember1 =
        g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidLeader);
    CSWSCreature* pMember1 =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidServerLeader);
    unsigned long oidServerMember2 =
        g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidLeader);
    CSWSCreature* pMember2 =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidServerLeader);
    if (pMember1 != NULL)
    {
        CPathfindInformation* pPathfind = pLeader->GetPathfindInformation();
        float fDistance = pPathfind->m_fStepScale;
        float fWidth = pPathfind->m_fHalfStep;
        int nResult = g_pAppManager->GetServerExoApp()->TestDirectLine(
            oidLeader, cMemberAreaResRef, pMember1->m_vPosition,
            m_aPathData[nEndIndex].m_vPosition, fDistance, fWidth, 1,
            &cMemberWalkHitInfo);
        unsigned long oidBlocker =
            g_pAppManager->GetClientExoApp()->ServerToClientObjectId(
                cMemberWalkHitInfo.m_oidBlocker);
        if (nResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
        {
            if (m_pParty->IsPartyMember(oidBlocker))
            {
                nResult = CSWSArea::DIRECT_LINE_CLEAR;
            }
        }
        if (nResult == CSWSArea::DIRECT_LINE_CLEAR)
        {
            m_pParty->m_aPartyMemberData[1].m_nFormationPathIndex = nEndIndex;
        }
    }

    if (pMember2 != NULL)
    {
        CPathfindInformation* pPathfind = pLeader->GetPathfindInformation();
        float fDistance = pPathfind->m_fStepScale;
        float fWidth = pPathfind->m_fHalfStep;
        int nResult = g_pAppManager->GetServerExoApp()->TestDirectLine(
            oidLeader, cMemberAreaResRef, pMember2->m_vPosition,
            m_aPathData[nEndIndex].m_vPosition, fDistance, fWidth, 1,
            &cMemberWalkHitInfo);
        unsigned long oidBlocker =
            g_pAppManager->GetClientExoApp()->ServerToClientObjectId(
                cMemberWalkHitInfo.m_oidBlocker);
        if (nResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
        {
            if (m_pParty->IsPartyMember(oidBlocker))
            {
                nResult = CSWSArea::DIRECT_LINE_CLEAR;
            }
        }
        if (nResult == CSWSArea::DIRECT_LINE_CLEAR)
        {
            m_pParty->m_aPartyMemberData[2].m_nFormationPathIndex = nEndIndex;
        }
    }

#ifdef _XBOX
    int nClosestMember = FindClosestPartyMemeber_Current();
    CSWParty* pXboxParty = m_pParty;
    EXOXBOXASSERT(nClosestMember >= 0 && nClosestMember < 3);
    int nStartIndex = pXboxParty->m_aPartyMemberData[nClosestMember].m_nFollowPathIndex;
#else
    int nStartIndex =
        m_pParty->m_aPartyMemberData[FindClosestPartyMemeber_Current()].m_nFollowPathIndex;
#endif
    if (nStartIndex >= 100 || nStartIndex < 0)
    {
        nStartIndex = 0;
    }

    aSmoothPath[0] = GetPathData(nStartIndex)->m_vPosition;
    int nCount = 1;
    int nIndex = nStartIndex + 1;
    if (nIndex >= 100)
    {
        nIndex = 0;
    }

    CResRef cAreaResRef;
    int nLastIndex = nIndex;
    int nLineResult = 1;

    while (nLineResult != 0 && nIndex != nEndIndex && nEndIndex != nStartIndex &&
           nCount < 200)
    {
        CWalkHitInfo cWalkHitInfo;
        Vector vDelta = aSmoothPath[nCount - 1] - GetPathData(nIndex)->m_vPosition;
        if (vDelta * (aSmoothPath[nCount - 1] - GetPathData(nIndex)->m_vPosition) > 0.00001)
        {
            CPathfindInformation* pPathfind = pLeader->GetPathfindInformation();
            float fDistance = pPathfind->m_fStepScale;
            float fWidth = pPathfind->m_fHalfStep;
            CSWPartyFollowData* pPathPoint = GetPathData(nIndex);
            nLineResult = g_pAppManager->GetServerExoApp()->TestDirectLine(
                oidLeader, cAreaResRef, aSmoothPath[nCount - 1],
                pPathPoint->m_vPosition, fDistance, fWidth, 1, &cWalkHitInfo);
        }
        else
        {
            nLineResult = CSWSArea::DIRECT_LINE_CLEAR;
        }

        unsigned long oidBlocker =
            g_pAppManager->GetClientExoApp()->ServerToClientObjectId(cWalkHitInfo.m_oidBlocker);
        if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
        {
            if (m_pParty->IsPartyMember(oidBlocker))
            {
                nLineResult = CSWSArea::DIRECT_LINE_CLEAR;
            }
        }

        if (nLineResult == CSWSArea::DIRECT_LINE_CLEAR)
        {
            nLastIndex = nIndex;
            if (++nIndex >= 100)
            {
                nIndex = 0;
            }
            continue;
        }
        else if (equal(aSmoothPath[nCount - 1], GetPathData(nLastIndex)->m_vPosition))
        {
            aSmoothPath[nCount] = GetPathData(nIndex)->m_vPosition;
            ++nCount;
            if (++nIndex >= 100)
            {
                nIndex = 0;
            }
        }
        else
        {
            aSmoothPath[nCount] = GetPathData(nLastIndex)->m_vPosition;
            ++nCount;
        }

        if (nCount >= 200)
        {
            break;
        }
    }

    if (nCount < 200)
    {
        CPathfindInformation* pPathfind = pLeader->GetPathfindInformation();
        float fDistance = pPathfind->m_fStepScale;
        float fWidth = pPathfind->m_fHalfStep;
        CSWPartyFollowData* pPathPoint = GetPathData(nIndex);
        if (g_pAppManager->GetServerExoApp()->TestDirectLine(
                oidLeader, cAreaResRef, aSmoothPath[nCount - 1],
                pPathPoint->m_vPosition, fDistance, fWidth, 1, NULL) ==
            CSWSArea::DIRECT_LINE_CLEAR)
        {
            aSmoothPath[nCount] = GetPathData(nIndex)->m_vPosition;
        }
        else
        {
            aSmoothPath[nCount] = GetPathData(nLastIndex)->m_vPosition;
            ++nCount;
            aSmoothPath[nCount] = GetPathData(nIndex)->m_vPosition;
        }
        ++nCount;
    }

    SwpartyDebugLine(
        m_aPathData[nStartIndex].m_vPosition + Vector(0.0f, 0.0f, OFF * SWPARTY_DEBUG_LINE_HEIGHT_STEP),
        m_aPathData[nEndIndex].m_vPosition + Vector(0.0f, 0.0f, OFF * SWPARTY_DEBUG_LINE_HEIGHT_STEP), OFF);
    SwpartyDebugLine(aSmoothPath[0] + Vector(0.0f, 0.0f, OFF * SWPARTY_DEBUG_LINE_HEIGHT_STEP),
                     aSmoothPath[nCount - 1] + Vector(0.0f, 0.0f, OFF * SWPARTY_DEBUG_LINE_HEIGHT_STEP), OFF);

    int nPoint = 0;
    Vector vLast = GetPathData(nStartIndex)->m_vPosition;
    m_nCurrentPathData = nStartIndex;
    while (nPoint < nCount)
    {
        Vector vDelta = aSmoothPath[nPoint] - vLast;
        Vector vNew;
        if (vDelta * vDelta > 1.0f)
        {
            Vector vDirection = normalize(vDelta);
            vNew = vDirection * 1.0f + vLast;
        }
        else
        {
            vNew = aSmoothPath[nPoint];
            ++nPoint;
        }

        m_aPathData[m_nCurrentPathData].m_vPosition = vNew;
        m_aPathData[m_nCurrentPathData].m_fFacing = vDelta.IsZero() ?
            m_aPathData[m_nCurrentPathData].m_fFacing : Yaw(normalize(vNew - vLast));
        m_nCurrentPathData = (m_nCurrentPathData >= 99) ? 0 : ++m_nCurrentPathData;
        vLast = vNew;
    }

    OFF += 6;
    if (OFF > 40)
    {
        OFF = 0;
    }

    m_pParty->RecaulateFollowPoint();
}
// PC 0x0068BDB0
CSWParty::CSWParty()
{
    m_fFormationRange[0] = 1.5f;
    m_fFormationRange[1] = 1.5f;
    m_fFormationRange[2] = 1.5f;
    m_aRightFormation[0][0] = Vector(1.5f, -0.7f, 0.0f);
    m_aRightFormation[0][1] = Vector(-1.5f, 0.8f, 0.0f);
    m_aRightFormation[1][0] = Vector(0.0f, 0.0f, 0.0f);
    m_aRightFormation[1][1] = Vector(0.0f, 0.0f, 0.0f);
    m_aRightFormation[2][0] = Vector(-2.0f, 1.5f, 0.0f);
    m_aRightFormation[2][1] = Vector(2.0f, 4.5f, 0.0f);
    m_bFormationInverted = 0;
    m_nFormation = FORMATION_NEXT;
    SetFormation(0);
    m_fFormationAngle = 1.0f;
    m_bLeftFormationAvailable[0] = 0;
    m_bLeftFormationAvailable[1] = 0;
    m_bLeftFormationAvailable[2] = 0;
    for (int nMember = 0; nMember < 3; ++nMember)
    {
        m_aPartyMemberData[nMember].m_oidCreature = OBJECT_INVALID;
    }
    m_vPartyPosition = Vector(0.0f, 0.0f, 0.0f);
    m_vLastLeaderPosition = Vector(0.0f, 0.0f, 0.0f);
    m_nCharacters = 0;
    m_nPartyState = 0;
    m_bPartyChanged = 0;
    m_nPartyFlags = 0;
    m_fRetreatTimer = 0.0f;
    m_aPartyMemberData[0].m_fFollowDistance = 0.0f;
    m_aPartyMemberData[1].m_fFollowDistance = 1.5f;
    m_bApplyLeaderRanges = 1;
    m_fUpdateInterval = 0.5f;
    m_cFollowPaths.m_pParty = this;
    m_aPartyMemberData[2].m_fFollowDistance = 3.0f;
}
// PC 0x0068C100
int CSWParty::AddCharacter(unsigned long oidCreature)
{
    int nAddedIndex = -1;
    if (oidCreature == OBJECT_INVALID)
    {
        return -1;
    }

    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    CSWSCreature* pCreature = pServer != NULL ?
        pServer->GetCreatureByGameObjectID(pServer->ClientToServerObjectId(oidCreature)) : NULL;
    if (pCreature == NULL)
    {
        return -1;
    }

    for (int nMember = 0; nMember < MAX_MEMBERS; ++nMember)
    {
        if (m_aPartyMemberData[nMember].m_oidCreature == oidCreature)
        {
            return nMember;
        }
        if (m_aPartyMemberData[nMember].m_oidCreature == OBJECT_INVALID)
        {
            m_aPartyMemberData[nMember].m_oidCreature = oidCreature;
            m_aPartyMemberData[nMember].m_nCharacterIndex =
                CSWPartyMemberData::FOLLOW_UNINITIALIZED;
            if (nMember == 0)
            {
                float fPrimaryRange;
                float fSecondaryRange;
                {
                    CExoString sColumn("PrimaryRange");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    EXOXBOXASSERT(p2DArrays->m_pRanges);
                    C2DA* pRanges = p2DArrays->m_pRanges;
                    pRanges->GetFLOATEntry(CSWRules::RANGE_PARTY_LEADER, sColumn, &fPrimaryRange);
                }
                {
                    CExoString sColumn("SecondaryRange");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    EXOXBOXASSERT(p2DArrays->m_pRanges);
                    C2DA* pRanges = p2DArrays->m_pRanges;
                    pRanges->GetFLOATEntry(CSWRules::RANGE_PARTY_LEADER, sColumn, &fSecondaryRange);
                }
                pClient->GetSWCMessage()->SendPlayerToServerGuiInventory_Status(1);
                pClient->GetSWCMessage()->SendPlayerToServerJournalMessage_QuestScreenOpen();
                pCreature->m_fSpotCheckDistance = fPrimaryRange;
                pCreature->m_fListenCheckDistance = fSecondaryRange;
            }
            else
            {
                float fPrimaryRange;
                float fSecondaryRange;
                {
                    CExoString sColumn("PrimaryRange");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    EXOXBOXASSERT(p2DArrays->m_pRanges);
                    C2DA* pRanges = p2DArrays->m_pRanges;
                    pRanges->GetFLOATEntry(CSWRules::RANGE_PARTY_MEMBER, sColumn, &fPrimaryRange);
                }
                {
                    CExoString sColumn("SecondaryRange");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    EXOXBOXASSERT(p2DArrays->m_pRanges);
                    C2DA* pRanges = p2DArrays->m_pRanges;
                    pRanges->GetFLOATEntry(CSWRules::RANGE_PARTY_MEMBER, sColumn, &fSecondaryRange);
                }
                pCreature->m_fSpotCheckDistance = fPrimaryRange;
                pCreature->m_fListenCheckDistance = fSecondaryRange;
                pCreature->AddAction(CSWSObject::ACTION_FOLLOW_LEADER,
                    CSWSObject::ACTION_GROUP_NEW,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL);
            }
            ++m_nCharacters;
            nAddedIndex = nMember;
            break;
        }
    }

    if (oidCreature == pClient->GetPlayerCreatureId())
    {
        pServer->GetPartyTable()->TransferInventory(pCreature);
    }
    if (pClient->GetAreaNotReadyToDisplay())
    {
        if (pServer->GetPartyTable()->GetIsLeader(
                pServer->ClientToServerObjectId(oidCreature)))
        {
            pClient->SetFutureLeader(oidCreature);
        }
        else
        {
            unsigned long oidFirstCreature = m_aPartyMemberData[0].m_oidCreature;
            if (!pServer->GetPartyTable()->GetIsLeader(
                    pServer->ClientToServerObjectId(oidFirstCreature)) &&
                pCreature->GetStats()->m_bIsPC)
            {
                SetLeader(nAddedIndex);
            }
        }
    }
    else
    {
        if (pServer->GetPartyTable()->GetIsLeader(
                pServer->ClientToServerObjectId(oidCreature)))
        {
            SetLeader(nAddedIndex);
        }
        else
        {
            unsigned long oidFirstCreature = m_aPartyMemberData[0].m_oidCreature;
            if (!pServer->GetPartyTable()->GetIsLeader(
                    pServer->ClientToServerObjectId(oidFirstCreature)) &&
                pCreature->GetStats()->m_bIsPC)
            {
                SetLeader(nAddedIndex);
            }
        }
    }
    m_bPartyChanged = 1;
    return nAddedIndex;
}
// PC 0x0068C480
int CSWParty::TestAndResetPath()
{
    int nPathIndex = m_cFollowPaths.m_nCurrentPathData - 1;
    if (nPathIndex < 0)
    {
        nPathIndex += 100;
    }

    float fFacing = m_cFollowPaths.GetPathData(nPathIndex)->m_fFacing;
    Vector vForward(0.0f, 1.0f, 0.0f);
    Vector vPathFacing = Quaternion(Vector(0.0f, 0.0f, 1.0f),
                                    DegToRad(fFacing)) * vForward;
#ifdef _XBOX
    int nClosestMember = m_cFollowPaths.FindClosestPartyMemeber_Current();
    EXOXBOXASSERT(nClosestMember >= 0 && nClosestMember < 3);
    Vector vMemberFacing = m_aPartyMemberData[nClosestMember].m_vFacing;
#else
    Vector vMemberFacing = m_aPartyMemberData[
        m_cFollowPaths.FindClosestPartyMemeber_Current()].m_vFacing;
#endif

    if (vPathFacing * vMemberFacing < 0.0f)
    {
        RecaulateFollowPoint();
        for (int nMember = 1; nMember <= 2; ++nMember)
        {
#ifdef _XBOX
            if (GetCharacter(nMember) != NULL)
            {
                if (GetMemberData(nMember)->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_UNINITIALIZED &&
                    GetMemberData(nMember)->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_FIND_PATH &&
                    GetMemberData(nMember)->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_WAITING_FOR_PATH &&
                    GetMemberData(nMember)->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_OFFSET_WAIT_DEST &&
                    GetMemberData(nMember)->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_PATH_WAIT_DEST)
                {
                    int nFormationPathIndex = GetMemberData(nMember)->m_nFormationPathIndex;
                    GetMemberData(nMember)->m_nFollowPathIndex = nFormationPathIndex;
                    GetMemberData(nMember)->m_vFollowPosition = GetMemberData(nMember)->m_vFormationPosition;
                    GetMemberData(nMember)->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_OFFSET;
                }
            }
#else
            if (GetCharacter(nMember) != NULL)
            {
                CSWPartyMemberData* pMember = &m_aPartyMemberData[nMember];
                if (pMember->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_UNINITIALIZED &&
                    pMember->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_FIND_PATH &&
                    pMember->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_WAITING_FOR_PATH &&
                    pMember->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_OFFSET_WAIT_DEST &&
                    pMember->m_nCharacterIndex != CSWPartyMemberData::FOLLOW_PATH_WAIT_DEST)
                {
                    pMember->m_nFollowPathIndex = pMember->m_nFormationPathIndex;
                    pMember->m_vFollowPosition = pMember->m_vFormationPosition;
                    pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_OFFSET;
                }
            }
#endif
        }
        InvertFormation();
        return 1;
    }
    return 0;
}
// PC 0x0068C700
void CSWParty::FollowStateChanged()
{
    int nIndex;

    RecaulateFollowPoint();
    for (nIndex = 0; nIndex < 3; ++nIndex)
    {
        EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
        switch (m_aPartyMemberData[nIndex].m_nCharacterIndex)
        {
            case 1:
                EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
                m_aPartyMemberData[nIndex].m_nCharacterIndex = 6;
                break;

            case 2:
                EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
                m_aPartyMemberData[nIndex].m_nCharacterIndex = 9;
                break;

            case 3:
                EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
                m_aPartyMemberData[nIndex].m_nCharacterIndex = 7;
                break;

            case 4:
                EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
                m_aPartyMemberData[nIndex].m_nCharacterIndex = 8;
                break;
        }
    }
}
// PC 0x0068C760
void CSWPartyFollowDataArray::ProcessPoint(const Vector& vPosition, float fFacing,
                                           float fTime)
{

    int nLastIndex = m_nCurrentPathData - 1;
    if (nLastIndex < 0)
    {
        nLastIndex += 100;
    }

    static Vector m_vLastGoodProcessedPoint(0.0f, 0.0f, 0.0f);
    static float m_fLastGoodProcessedBearing;

    Vector vDelta = vPosition - GetPathData(nLastIndex)->m_vPosition;
    float fDistanceSquared = vDelta * vDelta;
    SwpartyDebugLine(vPosition + Vector(0.0f, 0.0f, SWPARTY_DEBUG_LINE_HEIGHT_STEP),
                     vPosition - vDelta + Vector(0.0f, 0.0f, SWPARTY_DEBUG_LINE_HEIGHT_STEP), 0);

    unsigned long oidLeader = m_pParty->m_aPartyMemberData[0].m_oidCreature;
    unsigned long oidServerLeader =
        g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidLeader);
    CSWSCreature* pLeader =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidServerLeader);
    CResRef cAreaResRef;

    if (m_nPathDataCount == 0)
    {
        m_vLastGoodProcessedPoint = vPosition;
    }

    if (pLeader == NULL)
    {
        return;
    }

    CWalkHitInfo cWalkHitInfo;
    int nLineResult;
    {
        CPathfindInformation* pPathfind = pLeader->GetPathfindInformation();
        float fDistance = pPathfind->m_fStepScale;
        float fWidth = pPathfind->m_fHalfStep;
        CSWPartyFollowData* pLastPoint = GetPathData(nLastIndex);
        nLineResult = g_pAppManager->GetServerExoApp()->TestDirectLine(
            oidLeader, cAreaResRef, pLastPoint->m_vPosition, vPosition,
            fDistance, fWidth, 1, &cWalkHitInfo);
    }

    if (nLineResult == CSWSArea::DIRECT_LINE_CLEAR ||
        (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE &&
         cWalkHitInfo.m_oidBlocker != OBJECT_INVALID &&
         m_pParty->IsPartyMember(cWalkHitInfo.m_oidBlocker)))
    {
        m_vLastGoodProcessedPoint = vPosition;
        m_fLastGoodProcessedBearing = fFacing;
    }
    else
    {
        m_aPathData[m_nCurrentPathData].m_vPosition = m_vLastGoodProcessedPoint;
        m_nCurrentPathData = (m_nCurrentPathData >= 99) ? 0 : ++m_nCurrentPathData;
        m_vLastGoodProcessedPoint = vPosition;
        m_fLastGoodProcessedBearing = fFacing;
    }

    if (fDistanceSquared >= 0.25 || nLineResult != CSWSArea::DIRECT_LINE_CLEAR)
    {
        m_aPathData[m_nCurrentPathData].m_vPosition = vPosition;
        m_aPathData[m_nCurrentPathData].m_fFacing = fFacing;
        m_nCurrentPathData = (m_nCurrentPathData >= 99) ? 0 : ++m_nCurrentPathData;
        if (m_nPathDataCount < 100)
        {
            ++m_nPathDataCount;
        }

        if (m_nPathDataCount >= 3)
        {
            int nIndex = m_nCurrentPathData - 1;
            if (nIndex < 0)
            {
                nIndex += 100;
            }
            float fCurrentFacing = m_aPathData[nIndex].m_fFacing;
            --nIndex;
            if (nIndex < 0)
            {
                nIndex += 100;
            }
            CSWPartyFollowData* pPreviousPoint = &m_aPathData[nIndex];
            float fPreviousFacing = pPreviousPoint->m_fFacing;
            Vector vPreviousPosition = pPreviousPoint->m_vPosition;

            for (int nMember = 1; nMember < 3; ++nMember)
            {
                CSWParty* pParty = m_pParty;
                EXOXBOXASSERT(nMember >= 0 && nMember < 3);
                Vector vOffset = pParty->m_aPartyMemberData[nMember].m_vFormationOffset;
                Vector vPreviousOffset =
                    Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fPreviousFacing)) * vOffset;
                Vector vPreviousMemberPosition = vPreviousPosition + vPreviousOffset;
                Vector vCurrentOffset =
                    Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fCurrentFacing)) * vOffset;
                Vector vMemberPosition = vPreviousPosition + vCurrentOffset;
                SwpartyDebugLine(vPreviousMemberPosition + Vector(0.0f, 0.0f, SWPARTY_DEBUG_LINE_HEIGHT_STEP),
                                 vMemberPosition + Vector(0.0f, 0.0f, SWPARTY_DEBUG_LINE_HEIGHT_STEP), nMember);
                if (nMember == 1)
                {
                    pPreviousPoint->m_vFirstMemberPosition = vMemberPosition;
                }
                else
                {
                    pPreviousPoint->m_vSecondMemberPosition = vMemberPosition;
                }
            }

            if (g_bEnablePartySmoothing && m_pParty->m_nCharacters > 1)
            {
                if (PointOnLineDistanceFromLeader(FindFurthestPartyMemeber_Current(), 1) >= 49.0f ||
                    (m_pParty->m_nCharacters == 2 &&
                     m_pParty->m_aPartyMemberData[1].m_fSpeed >= 100.0f) ||
                    (m_pParty->m_nCharacters == 3 &&
                     m_pParty->m_aPartyMemberData[1].m_fSpeed >= 100.0f &&
                     m_pParty->m_aPartyMemberData[2].m_fSpeed >= 100.0f))
                {
                    SmoothFollowPath();
                }
            }

            unsigned long oidMember = m_pParty->m_aPartyMemberData[1].m_oidCreature;
            if (oidMember != OBJECT_INVALID)
            {
                unsigned long oidServerMember =
                    g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidMember);
                CSWSCreature* pMember =
                    g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidServerMember);
                if (pMember != NULL && !pMember->HasAction(CSWSObject::ACTION_FOLLOW_LEADER))
                {
                    CSWPartyMemberData* pMemberData = &m_pParty->m_aPartyMemberData[1];
                    pMemberData->m_nFollowPathIndex = pMemberData->m_nFormationPathIndex;
                    pMemberData->m_vFollowPosition = pMemberData->m_vFormationPosition;
                }
            }

            oidMember = m_pParty->m_aPartyMemberData[2].m_oidCreature;
            if (oidMember != OBJECT_INVALID)
            {
                unsigned long oidServerMember =
                    g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidMember);
                CSWSCreature* pMember =
                    g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidServerMember);
                if (pMember != NULL && !pMember->HasAction(CSWSObject::ACTION_FOLLOW_LEADER))
                {
                    CSWPartyMemberData* pMemberData = &m_pParty->m_aPartyMemberData[2];
                    pMemberData->m_nFollowPathIndex = pMemberData->m_nFormationPathIndex;
                    pMemberData->m_vFollowPosition = pMemberData->m_vFormationPosition;
                }
            }
        }
    }
}
// PC 0x0068CE70
void CSWParty::Update(float fTime)
{
    Vector vLeaderFollowPosition = m_vLeaderFollowPosition;
    CSWCCreature* pLeader = GetCharacter(0);
    int nDialogState =
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pLeader != NULL && pServerExoApp != NULL && nDialogState == 0)
    {
        CSWSCreature* pServerLeader = pServerExoApp->GetCreatureByGameObjectID(
            pServerExoApp->ClientToServerObjectId(pLeader->m_nId));
        int bFury = 0;
        int bForceSight = 0;
        for (int nEffect = 0; nEffect < pServerLeader->m_lstEffects.num; ++nEffect)
        {
            CGameEffect* pEffect = pServerLeader->m_lstEffects.element[nEffect];
            int nType = pEffect->GetEffectType();
            if (nType == CSWSEffectListHandler::EFFECT_FURY)
            {
                bFury = 1;
            }
            else if (pEffect->GetEffectType() ==
                     CSWSEffectListHandler::EFFECT_FORCESIGHT)
            {
                bForceSight = 1;
                break;
            }
        }

        if (AurIsSpeedBlurEnabled() && !bForceSight)
        {
            g_pAppManager->m_pClientExoApp->DisableVideoEffect();
        }
        else if (bFury || bForceSight)
        {
            if (bForceSight)
            {
                g_pAppManager->m_pClientExoApp->EnableVideoEffect(
                    VIDEO_EFFECT_FORCESIGHT);
            }
            else
            {
                g_pAppManager->m_pClientExoApp->EnableVideoEffect(
                    pServerLeader->m_pStats->m_n124);
            }
        }
        else if (g_pAppManager->m_pClientExoApp->GetVideoEffectType() == VIDEO_EFFECT_FORCESIGHT ||
                 g_pAppManager->m_pClientExoApp->GetVideoEffectType() == VIDEO_EFFECT_FURY_1 ||
                 g_pAppManager->m_pClientExoApp->GetVideoEffectType() == VIDEO_EFFECT_FURY_2 ||
                 g_pAppManager->m_pClientExoApp->GetVideoEffectType() == VIDEO_EFFECT_FURY_3)
        {
            g_pAppManager->m_pClientExoApp->DisableVideoEffect();
        }
    }
    else if (nDialogState != 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetVideoEffectType() == VIDEO_EFFECT_FORCESIGHT ||
            g_pAppManager->m_pClientExoApp->GetVideoEffectType() == VIDEO_EFFECT_FURY_1 ||
            g_pAppManager->m_pClientExoApp->GetVideoEffectType() == VIDEO_EFFECT_FURY_2 ||
            g_pAppManager->m_pClientExoApp->GetVideoEffectType() == VIDEO_EFFECT_FURY_3)
        {
            g_pAppManager->m_pClientExoApp->DisableVideoEffect();
        }
    }

    if (pLeader != NULL)
    {
        Quaternion qLeaderFacing(zaxis,
                                 Yaw(pLeader->m_vOrientation) * DEG_TO_RAD);
        Vector vDelta = m_vLastLeaderPosition - pLeader->m_vPosition;
        float fMovedSquared = vDelta * vDelta;
        Vector vFollowPosition = pLeader->m_vPosition -
            pLeader->m_vOrientation * (m_fFormationAngle * 0.5f);
        vDelta = m_vLeaderFollowPosition - vFollowPosition;
        if (vDelta * vDelta >= 100.0f)
        {
            m_vLeaderFollowPosition = vFollowPosition;
            vLeaderFollowPosition = vFollowPosition;
            m_qLeaderFacing = qLeaderFacing;
        }
        if (fMovedSquared >= 1.0f)
        {
            Vector vDirection = vFollowPosition - vLeaderFollowPosition;
            m_vLastLeaderPosition = pLeader->m_vPosition;
            m_vLastLeaderPosition = pLeader->m_vPosition;
            float fMoved = (float)sqrt(fMovedSquared);
            m_vLeaderFollowPosition = normalize(vDirection) * fMoved +
                                      vLeaderFollowPosition;
            m_qLeaderFacing = qLeaderFacing;
        }
    }

    int nFurthest = m_cFollowPaths.FindFurthestPartyMemeber_Current();
    if (GetCharacter(nFurthest) != NULL &&
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState == 0 &&
        m_cFollowPaths.PointOnLineDistanceFromLeader(nFurthest, 1) >= 49.0f &&
        !TestAndResetPath())
    {
        RecaulateFollowPoint();
    }

    m_fRetreatTimer += fTime;
    CExoString sRetreatScript("k_hen_retreat");
    float fPrimaryRange;
    {
        CExoString sColumn("PrimaryRange");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pRanges);
        C2DA* pRanges = p2DArrays->m_pRanges;
        pRanges->GetFLOATEntry(CTwoDimArrays::RANGES_PARTY_RETREAT_ROW,
                              sColumn, &fPrimaryRange);
    }
    fPrimaryRange = fPrimaryRange * fPrimaryRange;
    float fSecondaryRange;
    {
        CExoString sColumn("SecondaryRange");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pRanges);
        C2DA* pRanges = p2DArrays->m_pRanges;
        pRanges->GetFLOATEntry(CTwoDimArrays::RANGES_PARTY_RETREAT_ROW,
                              sColumn, &fSecondaryRange);
    }

    int bRetreat = 0;
    for (int nMember = 1; nMember < m_nCharacters; ++nMember)
    {
        unsigned long oidClientCreature = m_aPartyMemberData[nMember].m_oidCreature;
        unsigned long oidCreature =
            g_pAppManager->m_pServerExoApp->ClientToServerObjectId(oidClientCreature);
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidCreature);
        if (pCreature == NULL || pCreature->m_pFollowInfo == NULL)
        {
            continue;
        }

        int bInSafetyRange = pCreature->m_pFollowInfo->m_bInSafetyRange;
        CSWCCreature* pMember = GetCharacter(nMember);
        Vector vSeparation = pLeader->m_vPosition - pMember->m_vPosition;
        float fDistanceSquared = vSeparation * vSeparation;
        if (m_fRetreatTimer >= fSecondaryRange && fDistanceSquared >= fPrimaryRange)
        {
            bRetreat = 1;
            g_pVirtualMachine->RunScript(&sRetreatScript, oidCreature,
                                         oidCreature != OBJECT_INVALID);
        }

        int bInRange = fDistanceSquared <= 25.0;
        if (!bInRange && bInSafetyRange)
        {
            CExoString sScript("k_hen_exit5m");
            g_pVirtualMachine->RunScript(&sScript, oidCreature,
                                         oidCreature != OBJECT_INVALID);
            CSWSCreaturePartyFollowInfo* pFollowInfo = pCreature->m_pFollowInfo;
            pFollowInfo->m_bInSafetyRange = 0;
        }
        else if (bInRange && !bInSafetyRange)
        {
            CExoString sScript("k_hen_enter5m");
            g_pVirtualMachine->RunScript(&sScript, oidCreature,
                                         oidCreature != OBJECT_INVALID);
            CSWSCreaturePartyFollowInfo* pFollowInfo = pCreature->m_pFollowInfo;
            pFollowInfo->m_bInSafetyRange = 1;
        }
    }

    if (bRetreat)
    {
        m_fRetreatTimer = 0.0f;
    }
}
static inline int ComputeSafeLocation(CServerExoApp* pServerExoApp, CSWSCreature* pCreature, const Vector& vPosition, Vector* pvSafePosition)
{
    CPathfindInformation* pPathfind = pCreature->m_pPathfindInformation;
    unsigned long oidArea = pCreature->m_oidArea;
    return pServerExoApp->GetAreaByGameObjectID(oidArea)->ComputeSafeLocation(vPosition, 2.0f, pPathfind, 1, pvSafePosition);
}
// PC 0x0068D610
void CSWParty::Reset(const Vector& vPosition, float fFacing, int bFlag)
{
    m_cFollowPaths.Reset(vPosition, fFacing);
    if (bFlag)
    {
        Quaternion qFacing(zaxis, fFacing * DEG_TO_RAD);
        Vector vOffset(0.0f, 1.0f, 0.0f);
        vOffset = -(qFacing * vOffset);
        vOffset *= 1.5f;

        Vector vSafePosition(0.0f, 0.0f, 0.0f);
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        unsigned long oidCreature = m_aPartyMemberData[0].m_oidCreature;
        CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(
            pServerExoApp->ClientToServerObjectId(oidCreature));
        if (pCreature != NULL)
        {
            if (ComputeSafeLocation(pServerExoApp, pCreature, vOffset + vPosition, &vSafePosition))
            {
                m_aPartyMemberData[1].m_vFormationPosition = vSafePosition;
                m_aPartyMemberData[1].m_nFormationPathIndex = 0;
                m_cFollowPaths.ProcessPoint(vSafePosition, fFacing, 0.5f);
            }

            vOffset += vOffset;
            if (ComputeSafeLocation(pServerExoApp, pCreature, vOffset + vPosition, &vSafePosition))
            {
                m_aPartyMemberData[2].m_vFormationPosition = vSafePosition;
                m_aPartyMemberData[2].m_nFormationPathIndex = 0;
                m_cFollowPaths.ProcessPoint(vSafePosition, fFacing, 0.5f);
            }

        }
        m_cFollowPaths.ProcessPoint(vPosition, fFacing, 0.5f);
    }
}
// PC 0x0068D990
void CSWParty::ResetAllFollowData(const Vector& vPosition, float fFacing, int bFlag)
{
    Reset(vPosition, fFacing, bFlag);
    ResetFollowData(0, vPosition);
    ResetFollowData(1, vPosition);
    ResetFollowData(2, vPosition);

    for (int nCreature = 0; nCreature < MAX_MEMBERS; ++nCreature)
    {
        EXOXBOXASSERT(nCreature >= 0 && nCreature < 3);
        unsigned long oidCreature = m_aPartyMemberData[nCreature].m_oidCreature;
        CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
        CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(
            pServerExoApp->ClientToServerObjectId(oidCreature));
        if (pCreature != NULL)
        {
            pCreature->m_pPathfindInformation->m_nField_210 = 0;
            pCreature->m_pPathfindInformation->m_nField_34 = OBJECT_INVALID;
            if (!g_pAppManager->GetServerExoApp()->TestSafeLocation(
                    pCreature->GetId(), pCreature->m_vPosition))
            {
                Vector vSafePosition(0.0f, 0.0f, 0.0f);
                g_pAppManager->GetServerExoApp()->ComputeSafeLocation(
                    pCreature->m_vPosition, 5.0f, pCreature->GetId(), 1,
                    &vSafePosition);
                pCreature->SetPosition(vSafePosition, 1, 1, 0);
            }
        }
    }
}
