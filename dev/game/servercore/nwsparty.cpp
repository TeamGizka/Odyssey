#include "exobase.h"
#include "math.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "serverexoappinternal.h"
#include "factionmanager.h"
#include "nwsobject.h"
#include "nwsarea.h"
#include "nwmessage.h"
#include "nwsplayer.h"
#include "swguiabilitieschargen.h"
#include "nwsparty.h"
// PC 0x00601E60
int CSWSFaction::GetMemberList(unsigned long** ppMembers)
{
    *ppMembers = m_lstMembers.element;
    return m_lstMembers.num;
}
// PC 0x00601E70
int CSWSFaction::InviteMember(unsigned long oidCreature,
                              unsigned long oidInvitedBy)
{
    g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        pCreature->SetInvitedToParty(1);
        pCreature->m_oidInvitedBy = oidInvitedBy;
    }
    CSWSCreature* pInviter =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidInvitedBy);
    if (pInviter != NULL)
    {
        pInviter->AddToInvitationsOffered(oidCreature);
        return 1;
    }
    return 0;
}
// PC 0x00601EE0
int CSWSFaction::GetCreatureInParty(unsigned long oidCreature)
{
    unsigned long* pElement = m_lstMembers.element;
    int nNumMembers = m_lstMembers.num;
    int i;
    for (i = 0; i < nNumMembers; ++i)
    {
        if (pElement[i] == oidCreature)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00601F10
void CSWSFaction::Initialise()
{
    {
        CExoString sName("");

        m_sName = sName;
    }
    m_nLastFactionMember = 0;
    m_bGlobal = 1;
}
// PC 0x00601F80
CSWSFaction::~CSWSFaction()
{
}
// PC 0x00601FE0
void CSWSFaction::SendChatMessage(unsigned long nVolume,
                                  const CExoString& sMessage)
{
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSMessage* pMessage = pExoApp->GetSWSMessage();
    int i;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        CSWSPlayer* pPlayer =
            pExoApp->GetClientObjectByObjectId(m_lstMembers[i]);
        if (pPlayer != NULL)
        {
            pMessage->SendServerToPlayerChat_Party(pPlayer->m_nPlayerId,
                                                    nVolume, sMessage);
        }
    }
}
// PC 0x00602040
int CSWSFaction::SendFactionUpdateList()
{
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSMessage* pMessage = pExoApp->GetSWSMessage();
    int nResult = m_lstMembers.num;
    for (int i = 0; i < nResult; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        CSWSPlayer* pPlayer =
            pExoApp->GetClientObjectByObjectId(m_lstMembers[i]);
        if (pPlayer != NULL)
        {
            pMessage->SendServerToPlayerParty_List(
                pPlayer->m_nPlayerId, m_lstMembers.num, m_lstMembers.element, 1,
                m_lstMembers[i]);
        }
        nResult = m_lstMembers.num;
    }
    return nResult;
}
// PC 0x006020A0
void CSWSFaction::SendFactionUpdateAdd(unsigned long oidCreature)
{
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSMessage* pMessage = pExoApp->GetSWSMessage();
    int i;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        CSWSPlayer* pPlayer =
            pExoApp->GetClientObjectByObjectId(m_lstMembers[i]);
        if (pPlayer != NULL)
        {
            pMessage->SendServerToPlayerParty_List(
                pPlayer->m_nPlayerId, 1, &oidCreature, 3,
                m_lstMembers[i]);
        }
    }
}
// PC 0x00602100
void CSWSFaction::SendFactionUpdateRemove(unsigned long oidCreature)
{
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSMessage* pMessage = pExoApp->GetSWSMessage();
    int i;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        CSWSPlayer* pPlayer =
            pExoApp->GetClientObjectByObjectId(m_lstMembers[i]);
        if (pPlayer != NULL)
        {
            pMessage->SendServerToPlayerParty_List(
                pPlayer->m_nPlayerId, 1, &oidCreature, 4, OBJECT_INVALID);
        }
    }
}
// PC 0x00602160
unsigned long CSWSFaction::GetWeakestMember(unsigned long oidSource,
                                            int bMustBeVisible)
{
    int i;
    unsigned char nLevel;
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pSource;
    int nLowestLevel = 999;
    unsigned long oidWeakest = OBJECT_INVALID;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL)
        {
            nLevel = pCreature->m_pStats->GetLevel(0);
            if (nLevel < nLowestLevel)
            {
                if (oidSource != OBJECT_INVALID && bMustBeVisible)
                {
                    Vector vHit(0.0f, 0.0f, 0.0f);
                    unsigned long oidHit;
                    pExoApp = g_pAppManager->m_pServerExoApp;
                    pSource = pExoApp->GetCreatureByGameObjectID(oidSource);
                    if (pSource != NULL && pCreature->GetArea() != NULL &&
                        pCreature->GetArea()->ClearLineOfSight(
                            pCreature->m_vPosition, pSource->m_vPosition,
                            &vHit, &oidHit, OBJECT_INVALID, OBJECT_INVALID, 0,
                            0) == 1)
                    {
                        nLowestLevel = nLevel;
                        oidWeakest = m_lstMembers[i];
                    }
                }
                else
                {
                    nLowestLevel = nLevel;
                    oidWeakest = m_lstMembers[i];
                }
            }
        }
    }
    return oidWeakest;
}
// PC 0x006022A0
unsigned long CSWSFaction::GetStrongestMember(unsigned long oidSource,
                                              int bMustBeVisible)
{
    int i;
    unsigned char nLevel;
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pSource;
    int nHighestLevel = 0;
    unsigned long oidStrongest = OBJECT_INVALID;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL)
        {
            nLevel = pCreature->m_pStats->GetLevel(0);
            if (nLevel > nHighestLevel)
            {
                if (oidSource != OBJECT_INVALID && bMustBeVisible)
                {
                    Vector vHit(0.0f, 0.0f, 0.0f);
                    unsigned long oidHit;
                    pExoApp = g_pAppManager->m_pServerExoApp;
                    pSource = pExoApp->GetCreatureByGameObjectID(oidSource);
                    if (pSource != NULL && pCreature->GetArea() != NULL &&
                        pCreature->GetArea()->ClearLineOfSight(
                            pCreature->m_vPosition, pSource->m_vPosition,
                            &vHit, &oidHit, OBJECT_INVALID, OBJECT_INVALID, 0,
                            0) == 1)
                    {
                        nHighestLevel = nLevel;
                        oidStrongest = m_lstMembers[i];
                    }
                }
                else
                {
                    nHighestLevel = nLevel;
                    oidStrongest = m_lstMembers[i];
                }
            }
        }
    }
    return oidStrongest;
}
// PC 0x006023E0
unsigned long CSWSFaction::GetMostDamagedMember(unsigned long oidSource,
                                                 int bMustBeVisible)
{
    int i;
    int nDamage;
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pSource;
    int nMostDamage = -1;
    unsigned long oidMostDamaged = OBJECT_INVALID;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL && !pCreature->GetDead() &&
            !pCreature->GetDeadTemp())
        {
            nDamage = pCreature->GetMaxHitPoints(1);
            nDamage -= pCreature->GetCurrentHitPoints(0);
            if (nDamage > nMostDamage)
            {
                if (oidSource != OBJECT_INVALID && bMustBeVisible)
                {
                    Vector vHit(0.0f, 0.0f, 0.0f);
                    unsigned long oidHit;
                    pExoApp = g_pAppManager->m_pServerExoApp;
                    pSource = pExoApp->GetCreatureByGameObjectID(oidSource);
                    if (pSource != NULL && pCreature->GetArea() != NULL &&
                        pCreature->GetArea()->ClearLineOfSight(
                            pCreature->m_vPosition, pSource->m_vPosition,
                            &vHit, &oidHit, OBJECT_INVALID, OBJECT_INVALID, 0,
                            0) == 1)
                    {
                        oidMostDamaged = m_lstMembers[i];
                        nMostDamage = nDamage;
                    }
                }
                else
                {
                    oidMostDamaged = m_lstMembers[i];
                    nMostDamage = nDamage;
                }
            }
        }
    }
    return oidMostDamaged;
}
// PC 0x00602570
unsigned long CSWSFaction::GetLeastDamagedMember(unsigned long oidSource,
                                                 int bMustBeVisible)
{
    int i;
    int nDamage;
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pSource;
    int nLeastDamage = 999;
    unsigned long oidLeastDamaged = OBJECT_INVALID;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (!pCreature->GetDead() && !pCreature->GetDeadTemp())
        {
            nDamage = pCreature->GetMaxHitPoints(1);
            nDamage -= pCreature->GetCurrentHitPoints(0);
            if (nDamage < nLeastDamage)
            {
                if (oidSource != OBJECT_INVALID && bMustBeVisible)
                {
                    Vector vHit(0.0f, 0.0f, 0.0f);
                    unsigned long oidHit;
                    pExoApp = g_pAppManager->m_pServerExoApp;
                    pSource = pExoApp->GetCreatureByGameObjectID(oidSource);
                    if (pSource != NULL && pCreature->GetArea() != NULL &&
                        pCreature->GetArea()->ClearLineOfSight(
                            pCreature->m_vPosition, pSource->m_vPosition,
                            &vHit, &oidHit, OBJECT_INVALID, OBJECT_INVALID, 0,
                            0) == 1)
                    {
                        oidLeastDamaged = m_lstMembers[i];
                        nLeastDamage = nDamage;
                    }
                }
                else
                {
                    oidLeastDamaged = m_lstMembers[i];
                    nLeastDamage = nDamage;
                }
            }
        }
    }
    return oidLeastDamaged;
}
// PC 0x006026F0
int CSWSFaction::GetGold()
{
    for (int i = 0; i < m_lstMembers.num; ++i)
    {
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            m_lstMembers[i]);
    }
    return 0;
}
// PC 0x00602730
int CSWSFaction::GetAverageReputation(unsigned long oidCreature)
{
    int nTotalReputation = 0;
    CSWSObject* pCreature;
    int i;
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;

    pCreature = (CSWSObject*)pExoApp->GetCreatureByGameObjectID(oidCreature);

    if (pCreature != NULL)
    {
        for (i = 0; i < m_lstMembers.num; ++i)
        {
            nTotalReputation +=
                pCreature->GetReputation(m_lstMembers[i], 0);
        }
    }
    EXOXBOXASSERT(m_lstMembers.num > 0);
    nTotalReputation /= m_lstMembers.num;
    if (nTotalReputation < 0)
    {
        return 0;
    }
    if (nTotalReputation > 100)
    {
        return 100;
    }
    return nTotalReputation;
}
// PC 0x006027A0
int CSWSFaction::GetAverageGoodEvilAlignment()
{
    int nTotalAlignment = 0;
    int i;
    for (i = 0; i < m_lstMembers.num; ++i)
    {
        CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature =
            pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL)
        {
            nTotalAlignment += pCreature->m_pStats->GetSimpleAlignmentGoodEvil();
        }
    }
    if (m_lstMembers.num > 0)
    {
        if (m_lstMembers.num == 1)
        {
            return nTotalAlignment;
        }
        float fAverageAlignment = nTotalAlignment / m_lstMembers.num;
        if (fAverageAlignment == 2.5f)
        {
            return CSWSCreatureStats::ALIGNMENT_NEUTRAL;
        }
        if (fabs(fAverageAlignment - CSWSCreatureStats::ALIGNMENT_GOOD) <
            fabs(fAverageAlignment - CSWSCreatureStats::ALIGNMENT_EVIL))
        {
            return CSWSCreatureStats::ALIGNMENT_GOOD;
        }
        return CSWSCreatureStats::ALIGNMENT_EVIL;
    }
    return CSWSCreatureStats::ALIGNMENT_NEUTRAL;
}
// PC 0x00602860
int CSWSFaction::GetAverageLevel()
{
    int nTotalLevel = 0;
    int i;
    for (i = 0; i < m_lstMembers.num; ++i)
    {
        CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature =
            pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL)
        {
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            nTotalLevel += pStats->GetLevel(0);
        }
    }
    if (m_lstMembers.num > 0)
    {
        return nTotalLevel / m_lstMembers.num;
    }
    return 0;
}
// PC 0x006028C0
int CSWSFaction::GetAverageXP()
{
    int nTotalXP = 0;
    int i;
    for (i = 0; i < m_lstMembers.num; ++i)
    {
        CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature =
            pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL)
        {
            nTotalXP += pCreature->m_pStats->m_nExperience;
        }
    }
    if (m_lstMembers.num > 0)
    {
        return nTotalXP / m_lstMembers.num;
    }
    return 0;
}
// PC 0x00602910
int CSWSFaction::GetMostFrequentClass()
{
    int i;
    int j;
    int k;
    int m;
    int nBestClass;
    int nBestCount;
    int nCount;
    int nClass;
    CSWSCreature* pCreature;
    CSWSCreature* pOther;
    CServerExoApp* pExoApp;

    nBestClass = 0;
    nBestCount = 0;
    nCount = 0;
    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL)
        {
            for (j = 0; j < pCreature->m_pStats->m_nClassCount; ++j)
            {
                nClass = (unsigned char)pCreature->m_pStats->GetClass(j);
                for (k = 0; k < m_lstMembers.num; ++k)
                {
                    pExoApp = g_pAppManager->m_pServerExoApp;
                    pOther = pExoApp->GetCreatureByGameObjectID(
                        m_lstMembers[k]);
                    if (pOther != NULL)
                    {
                        for (m = 0; m < pOther->m_pStats->m_nClassCount; ++m)
                        {
                            if ((unsigned char)pOther->m_pStats->GetClass(m) ==
                                nClass)
                            {
                                ++nCount;
                            }
                        }
                    }
                }
                if (nCount > nBestCount)
                {
                    nBestCount = nCount;
                    nBestClass = nClass;
                }
            }
        }
    }
    return nBestClass;
}
// PC 0x00602A60
unsigned long CSWSFaction::GetWorstAC(unsigned long oidSource,
                                      int bMustBeVisible)
{
    int i;
    short nAC;
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pSource;
    short nWorstAC = 999;
    unsigned long oidWeakest = OBJECT_INVALID;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL)
        {
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            nAC = pStats->GetArmorClassVersus(NULL, 0);
            if (nAC < nWorstAC)
            {
                if (oidSource != OBJECT_INVALID && bMustBeVisible)
                {
                    Vector vHit(0.0f, 0.0f, 0.0f);
                    unsigned long oidHit;
                    pExoApp = g_pAppManager->m_pServerExoApp;
                    pSource = pExoApp->GetCreatureByGameObjectID(oidSource);
                    if (pSource != NULL && pCreature->GetArea() != NULL &&
                        pCreature->GetArea()->ClearLineOfSight(
                            pCreature->m_vPosition, pSource->m_vPosition,
                            &vHit, &oidHit, OBJECT_INVALID, OBJECT_INVALID, 0,
                            0) == 1)
                    {
                        nWorstAC = nAC;
                        oidWeakest = m_lstMembers[i];
                    }
                }
                else
                {
                    nWorstAC = nAC;
                    oidWeakest = m_lstMembers[i];
                }
            }
        }
    }
    return oidWeakest;
}
// PC 0x00602BA0
unsigned long CSWSFaction::GetBestAC(unsigned long oidSource,
                                     int bMustBeVisible)
{
    int i;
    short nAC;
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pSource;
    short nBestAC = 0;
    unsigned long oidBest = OBJECT_INVALID;

    for (i = 0; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL)
        {
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            nAC = pStats->GetArmorClassVersus(NULL, 0);
            if (nAC > nBestAC)
            {
                if (oidSource != OBJECT_INVALID && bMustBeVisible)
                {
                    Vector vHit(0.0f, 0.0f, 0.0f);
                    unsigned long oidHit;
                    pExoApp = g_pAppManager->m_pServerExoApp;
                    pSource = pExoApp->GetCreatureByGameObjectID(oidSource);
                    if (pSource != NULL && pCreature->GetArea() != NULL &&
                        pCreature->GetArea()->ClearLineOfSight(
                            pCreature->m_vPosition, pSource->m_vPosition,
                            &vHit, &oidHit, OBJECT_INVALID, OBJECT_INVALID, 0,
                            0) == 1)
                    {
                        nBestAC = nAC;
                        oidBest = m_lstMembers[i];
                    }
                }
                else
                {
                    nBestAC = nAC;
                    oidBest = m_lstMembers[i];
                }
            }
        }
    }
    return oidBest;
}
// PC 0x00602E70
CSWSFaction::CSWSFaction(unsigned long oidCreature)
{
    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    EXOXBOXASSERT(pCreature);
    if (pCreature->GetFaction() != NULL)
    {
        pCreature->GetFaction()->RemoveMember(oidCreature);
    }
    if (m_lstMembers.num == m_lstMembers.array_size)
    {
        m_lstMembers.Allocate(
            m_lstMembers.array_size != 0 ? 2 * m_lstMembers.array_size : 16);
    }
    m_lstMembers.element[m_lstMembers.num++] = oidCreature;
    CFactionManager* pFactionManager = GetFactionManager();
    if (pFactionManager != NULL)
    {
        CExoArrayList<CSWSFaction*>* pFactions =
            pFactionManager->m_pFactions;
        if (pFactions->num == pFactions->array_size)
        {
            pFactions->Allocate(pFactions->array_size != 0
                                    ? 2 * pFactions->array_size
                                    : 16);
        }
        pFactions->element[pFactions->num++] = this;
        m_nIndex = pFactionManager->m_pFactions->num - 1;
        pCreature->SetFactionId(m_nIndex);
    }
    Initialise();
}
// PC 0x00602F80
CSWSFaction::CSWSFaction()
{
    CFactionManager* pFactionManager = GetFactionManager();
    if (pFactionManager != NULL)
    {
        CExoArrayList<CSWSFaction*>* pFactions =
            pFactionManager->m_pFactions;
        if (pFactions->num == pFactions->array_size)
        {
            pFactions->Allocate(pFactions->array_size != 0
                                    ? 2 * pFactions->array_size
                                    : 16);
        }
        pFactions->element[pFactions->num++] = this;
        m_nIndex = pFactionManager->m_pFactions->num - 1;
        if (m_nIndex > 0)
        {
            ++pFactionManager->m_nNumFactions;
        }
    }
    Initialise();
}

CFactionManager* CSWSFaction::GetFactionManager()
{
    CServerExoAppInternal* pServerMode =
        g_pAppManager->m_pServerExoApp->m_pcExoAppInternal;
    if (pServerMode != NULL)
    {
        return pServerMode->m_pFactionManager;
    }
    return NULL;
}
// PC 0x00603040
void CSWSFaction::RemoveMember(unsigned long oidCreature)
{
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature == NULL)
    {
        return;
    }
    if (!m_lstMembers.Contains(oidCreature))
    {
        return;
    }
    m_lstMembers.Remove(oidCreature);
    int nAreaWaitState = pCreature->m_nAreaWaitState;
    if (nAreaWaitState == 1)
    {
        CSWSFaction* pFaction = new CSWSFaction(oidCreature);
        pFaction->SendFactionUpdateList();
    }
    if (m_lstMembers.num == 0)
    {
        CFactionManager* pFactionManager = GetFactionManager();
        if (pFactionManager != NULL)
        {
            int nFactionIndex = m_nIndex;
            if (!pFactionManager->GetIsNPCFaction(nFactionIndex) &&
                pCreature->m_pStats->m_bIsPC == 1)
            {
                pFactionManager->DeleteFaction(m_nIndex);
            }
        }
    }
    else
    {
        SendFactionUpdateRemove(oidCreature);
    }
}
// PC 0x00603150
int CSWSFaction::TransferLeadership(unsigned long oidCreature,
                                    unsigned long oidLeader)
{
    if (GetLeader() == oidLeader)
    {
        m_lstMembers.Remove(oidCreature);
        m_lstMembers.Insert(oidCreature, 0);
        return 1;
    }
    return 0;
}
// PC 0x00602CE0
unsigned long CSWSFaction::GetLeader()
{
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;
    int i;
    unsigned long oidLeader = OBJECT_INVALID;
    CFactionManager* pFactionManager = GetFactionManager();
    if (pFactionManager != NULL)
    {
        int nFactionIndex = m_nIndex;
        if (pFactionManager->GetIsNPCFaction(nFactionIndex))
        {
            float fBestChallengeRating = 0.0f;
            for (i = 0; i < m_lstMembers.num; ++i)
            {
                pExoApp = g_pAppManager->m_pServerExoApp;
                pCreature = pExoApp->GetCreatureByGameObjectID(
                    m_lstMembers[i]);
                if (pCreature != NULL &&
                    pCreature->m_pStats->m_fChallengeRating >
                        fBestChallengeRating)
                {
                    fBestChallengeRating =
                        pCreature->m_pStats->m_fChallengeRating;
                    oidLeader = m_lstMembers[i];
                }
            }
        }
        else
        {
            for (i = 0; i < m_lstMembers.num; ++i)
            {
                pExoApp = g_pAppManager->m_pServerExoApp;
                pCreature = pExoApp->GetCreatureByGameObjectID(
                    m_lstMembers[i]);
                if (pCreature != NULL && pCreature->m_nAreaWaitState != 0)
                {
                    return pCreature->m_nId;
                }
            }
        }
    }
    return oidLeader;
}
// PC 0x00602DB0
int CSWSFaction::GetSingletonParty()
{
    int i;
    int nCount;
    CSWSCreature* pCreature;
    CServerExoApp* pExoApp;

    i = 0;
    nCount = 0;
    for (; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL && pCreature->m_nAreaWaitState != 0)
        {
            ++nCount;
        }
        if (nCount > 1)
        {
            return 0;
        }
    }
    return 1;
}
#ifndef _XBOX
unsigned long CSWSFaction::GetFactionMember(int nMember, int nFaction)  // PC 0x00602E00
{
    int i;
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;

    if (m_lstMembers.num < 1 || nMember >= m_lstMembers.num)
    {
        return OBJECT_INVALID;
    }
    for (i = nMember; i < m_lstMembers.num; ++i)
    {
        pExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
        if (pCreature != NULL && pCreature->m_pStats->m_bIsPC == nFaction)
        {
            m_nLastFactionMember = i + 1;
            return m_lstMembers[i];
        }
    }
    return OBJECT_INVALID;
}
#endif
void CSWSFaction::AddMember(unsigned long oidCreature, int bSilent)  // PC 0x00603190
{
    CSWSCreature* pCreature;
    CSWSFaction* pOldFaction;
    CSWSMessage* pMessage;
    CSWSPlayer* pPlayer;
    CServerExoApp* pExoApp;
    unsigned long* pElement;
    int nNumMembers;
    int i;

    pExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        pOldFaction = pCreature->GetFaction();
        nNumMembers = m_lstMembers.num;
        pElement = m_lstMembers.element;
        for (i = 0; i < nNumMembers; ++i)
        {
            if (pElement[i] == oidCreature)
            {
                return;
            }
        }
        if (pOldFaction != NULL)
        {
            pOldFaction->RemoveMember(oidCreature);
        }
        if (bSilent == 0)
        {
            SendFactionUpdateAdd(oidCreature);
        }
        m_lstMembers.AddUnique(oidCreature);
        pCreature->SetFactionId(m_nIndex);
        pExoApp = g_pAppManager->m_pServerExoApp;
        pMessage = pExoApp->GetSWSMessage();
        pExoApp = g_pAppManager->m_pServerExoApp;
        pPlayer = pExoApp->GetClientObjectByObjectId(oidCreature);
        if (pPlayer != NULL)
        {
            pMessage->SendServerToPlayerParty_List(
                pPlayer->m_nPlayerId, m_lstMembers.num, m_lstMembers.element, 1,
                oidCreature);
        }
    }
}
// PC 0x00603240
int CSWSFaction::AttemptRemove(unsigned long oidCreature,
                               unsigned long oidLeader)
{
    if (m_lstMembers.num <= 1 ||
        (oidCreature != oidLeader && GetLeader() != oidLeader))
    {
        return 0;
    }
    RemoveMember(oidCreature);
    return 1;
}

#ifdef _XBOX

unsigned long CSWSFaction::GetFactionMember(int nMember, int nFaction)
{
    int i;
    CServerExoApp* pExoApp;
    CSWSCreature* pCreature;

    if (m_lstMembers.num >= 1 && nMember < m_lstMembers.num)
    {
        for (i = nMember; i < m_lstMembers.num; ++i)
        {
            pExoApp = g_pAppManager->m_pServerExoApp;
            pCreature = pExoApp->GetCreatureByGameObjectID(m_lstMembers[i]);
            if (pCreature != NULL && pCreature->m_pStats->m_bIsPC == nFaction)
            {
                m_nLastFactionMember = i + 1;
                return m_lstMembers[i];
            }
        }
    }
    return OBJECT_INVALID;
}
#endif
