#include "exobase.h"
#include "resgff.h"
#include "twodimarray.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "nwsparty.h"
#include "factionmanager.h"
#include "nwrules.h"
// PC 0x005AD1E0 GOG 0x005AD250
CFactionManager::CFactionManager()
{
    m_pPlayer = NULL;
    m_pHostile1 = NULL;
    m_pHostile2 = NULL;
    m_pFriendly1 = NULL;
    m_pFriendly2 = NULL;
    m_pInsane = NULL;
    m_pNeutral = NULL;
    m_pFactions = new CExoArrayList<CSWSFaction*>(0);
    m_nNumFactions = 0;
    m_pReputations = NULL;
}
// PC 0x005AD250 GOG 0x005AD2C0
CFactionManager::~CFactionManager()
{
    if (m_pFactions != NULL)
    {
        for (int i = 0; i < m_pFactions->num; i++)
        {
            CSWSFaction* pFaction = m_pFactions->element[i];
            if (pFaction != NULL)
            {
                delete pFaction;
            }
        }
        delete m_pFactions;
    }
    if (m_pReputations != NULL)
    {
        delete m_pReputations;
        m_pReputations = NULL;
    }
}
// PC 0x005ACB80 GOG 0x005ACBF0
int CFactionManager::GetIsNPCFaction(int nFaction)
{
    return nFaction >= 1 && nFaction <= m_nNumFactions;
}
// PC 0x005ACBA0 GOG 0x005ACC10
void CFactionManager::ResolveFactionName(const CExoString& sName,
                                         CSWSFaction* pFaction)
{
    if (sName.CompareNoCase("player"))
    {
        m_pPlayer = pFaction;
    }
    else if (sName.CompareNoCase("hostile_1"))
    {
        m_pHostile1 = pFaction;
    }
    else if (sName.CompareNoCase("friendly_1"))
    {
        m_pFriendly1 = pFaction;
    }
    else if (sName.CompareNoCase("hostile_2"))
    {
        m_pHostile2 = pFaction;
    }
    else if (sName.CompareNoCase("friendly_2"))
    {
        m_pFriendly2 = pFaction;
    }
    else if (sName.CompareNoCase("neutral"))
    {
        m_pNeutral = pFaction;
    }
    else if (sName.CompareNoCase("insane"))
    {
        m_pInsane = pFaction;
    }
}
// PC 0x005ACC70 GOG 0x005ACCE0
void CFactionManager::DeleteFaction(int nFaction)
{
    if (m_pFactions != NULL)
    {
        CSWSFaction* pFaction = m_pFactions->element[nFaction];
        if (pFaction != NULL)
        {
            delete pFaction;
            m_pFactions->element[nFaction] = NULL;
        }
    }
}
// PC 0x005AD130 GOG 0x005AD1A0
int CFactionManager::SaveReputations(CResGFF* pRes, CResList* pList)
{
    CResStruct stRep;
    unsigned long nElement = 0;
    for (int i = 0; i <= m_nNumFactions; i++)
    {
        for (int j = 0; j <= m_nNumFactions; j++)
        {
            int nReputation = GetNPCFactionReputation(j, i);
            if (nReputation != 100)
            {
                pRes->AddListElement(&stRep, pList, nElement);
                pRes->WriteFieldDWORD(&stRep, i, "FactionID1");
                pRes->WriteFieldDWORD(&stRep, j, "FactionID2");
                pRes->WriteFieldDWORD(&stRep, nReputation, "FactionRep");
                nElement++;
            }
        }
    }
    return 1;
}
// PC 0x005ACCB0 GOG 0x005ACD20
CSWSFaction* CFactionManager::GetFaction(int nFaction)
{
    if (nFaction < 0 || nFaction >= m_pFactions->num)
    {
        return NULL;
    }
    return m_pFactions->element[nFaction];
}
// PC 0x005ACCD0 GOG 0x005ACD40
void CFactionManager::SetNPCFactionReputation(int nFaction1, int nFaction2,
                                              int nReputation)
{
    if (nFaction2 >= 1 && nFaction2 <= m_nNumFactions && nFaction1 >= 0 &&
        nFaction1 <= m_nNumFactions)
    {
        if (nReputation > 100)
        {
            nReputation = 100;
        }
        else if (nReputation < 0)
        {
            nReputation = 0;
        }
        int nIndex = nFaction1 * m_nNumFactions + nFaction2 - 1;
        if (nIndex < m_pReputations->num)
        {
            m_pReputations->element[nIndex] = nReputation;
        }
    }
}
// PC 0x005ACD20 GOG 0x005ACD90
int CFactionManager::GetNPCFactionReputation(int nFaction1, int nFaction2)
{
    if (!GetIsNPCFaction(nFaction1) && !GetIsNPCFaction(nFaction2))
    {
        return 50;
    }
    if (GetIsNPCFaction(nFaction1) || !GetIsNPCFaction(nFaction2))
    {
        int nIndex = nFaction2 * m_nNumFactions + nFaction1 - 1;
        if (nIndex >= 0 && nIndex < m_pReputations->num)
        {
            return m_pReputations->element[nIndex];
        }
        return 50;
    }
    return GetNPCFactionReputation(nFaction2, nFaction1);
}
// PC 0x005ACD90 GOG 0x005ACE00
int CFactionManager::LoadFactions(int bAppend)
{
    CExoString sName;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pRepute);
    C2DA* pFactions = p2DArrays->m_pRepute;
    int nNumRows = pFactions->m_nNumRows;
    int i = 0;

    if (bAppend)
    {
        CExoArrayList<CSWSFaction*>* pFactionList = m_pFactions;
        i = pFactionList->num;
    }
    for (; i < nNumRows; ++i)
    {
        {
            CExoString sLabel("LABEL");
            pFactions->GetCExoStringEntry(i, sLabel, &sName);
        }
        CSWSFaction* pFaction = new CSWSFaction;
        pFaction->m_sName = sName;
        pFaction->m_bGlobal = 1;
        pFaction->m_nParentID = -1;
        ResolveFactionName(sName, pFaction);
    }
    return 1;
}
// PC 0x005ACEC0 GOG 0x005ACF30
int CFactionManager::LoadFactionsFromSaveGame(CResGFF* pRes, CResList* pList)
{
    CResStruct stFaction;
    CExoString sName;
    int bSuccess = 1;
    int nCount = pRes->GetListCount(pList);

    for (int i = 0; i < nCount && bSuccess; i++)
    {
        pRes->GetListElement(&stFaction, pList, i);
        {
            CExoString sDefault("");
            sName = pRes->ReadFieldCExoString(&stFaction, "FactionName",
                                              bSuccess, sDefault);
        }
        int nParentID =
            pRes->ReadFieldDWORD(&stFaction, "FactionParentID", bSuccess, 0);
        int bGlobal =
            pRes->ReadFieldWORD(&stFaction, "FactionGlobal", bSuccess, 0);
        if (!bSuccess)
        {
            bGlobal = 1;
            bSuccess = 1;
        }
        CSWSFaction* pFaction = new CSWSFaction;
        pFaction->m_sName = sName;
        pFaction->m_bGlobal = bGlobal;
        pFaction->m_nParentID = nParentID;
        ResolveFactionName(sName, pFaction);
    }
    int bLoaded = LoadFactions(1);
    return bSuccess && bLoaded;
}
// PC 0x005AD090 GOG 0x005AD100
int CFactionManager::SaveFactions(CResGFF* pRes, CResList* pList)
{
    CResStruct stFaction;
    for (int i = 0; i < m_nNumFactions + 1; i++)
    {
        CSWSFaction* pFaction = GetFaction(i);
        pRes->AddListElement(&stFaction, pList, i);
        pRes->WriteFieldCExoString(&stFaction, pFaction->m_sName,
                                   "FactionName");
        pRes->WriteFieldDWORD(&stFaction, pFaction->m_nParentID,
                              "FactionParentID");
        pRes->WriteFieldWORD(&stFaction, pFaction->m_bGlobal, "FactionGlobal");
    }
    return 1;
}
// PC 0x005AD2E0 GOG 0x005AD350
int CFactionManager::LoadReputations()
{
    int nEntries = m_nNumFactions * (m_nNumFactions + 1);
    int bSuccess = 1;

    if (m_pReputations != NULL)
    {
        delete m_pReputations;
    }
    m_pReputations = new CExoArrayList<int>(0);
    for (int i = 0; i < nEntries; i++)
    {
        m_pReputations->Add(100);
    }
    m_pReputations->SetSize(m_pReputations->num);

#ifdef _XBOX
    CTwoDimArrays* pArraysForRepute = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(pArraysForRepute->m_pRepute);
    C2DA* pRepute = pArraysForRepute->m_pRepute;
#else
    C2DA* pRepute = g_pRules->m_p2DArrays->m_pRepute;
#endif
    int nNumRows = pRepute->m_nNumRows;
    int nNumColumns = pRepute->m_nNumColumns;
    int nReputation;

    for (int nRow = 0; bSuccess && nRow <= nNumRows; nRow++)
    {
        for (int nColumn = 0; bSuccess && nColumn < nNumColumns; nColumn++)
        {
            bSuccess = pRepute->GetINTEntry(nRow, nColumn, &nReputation);
            SetNPCFactionReputation(nRow, nColumn, nReputation);
        }
    }
    return bSuccess;
}
// PC 0x005AD4E0 GOG 0x005AD550
int CFactionManager::LoadReputationsFromSaveGame(CResGFF* pRes, CResList* pList)
{
    int bSuccess;
    CResStruct stRep;

    LoadReputations();
    bSuccess = 1;
    int nCount = pRes->GetListCount(pList);
    for (int i = 0; bSuccess && i <= nCount; i++)
    {
        pRes->GetListElement(&stRep, pList, i);
        int nFaction1 = pRes->ReadFieldDWORD(&stRep, "FactionID1", bSuccess, 0);
        int nFaction2 = pRes->ReadFieldDWORD(&stRep, "FactionID2", bSuccess, 0);
        int nReputation =
            pRes->ReadFieldDWORD(&stRep, "FactionRep", bSuccess, 0);
        SetNPCFactionReputation(nFaction1, nFaction2, nReputation);
    }
    return bSuccess;
}
// PC 0x005AD5E0 GOG 0x005AD650
void CFactionManager::CreateDefaultFactions()
{
    m_pPlayer = new CSWSFaction;
    m_pHostile1 = new CSWSFaction;
    m_pHostile2 = new CSWSFaction;
    m_pFriendly1 = new CSWSFaction;
    m_pFriendly2 = new CSWSFaction;
    m_pNeutral = new CSWSFaction;
    m_pInsane = new CSWSFaction;

    int nEntries = m_nNumFactions * (m_nNumFactions + 1);
    m_pReputations = new CExoArrayList<int>(0);
    m_pReputations->Add(0);
    for (int i = 1; i < nEntries; i++)
    {
        m_pReputations->Add(100);
    }
    m_pReputations->SetSize(m_pReputations->num);
}
