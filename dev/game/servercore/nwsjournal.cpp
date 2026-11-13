#include "exobase.h"
#include "exoresman.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwjournal.h"
#include "nwsobject.h"
#include "nwsplayer.h"
#include "nwmessage.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "twodimarrays.h"

static inline void NwsjournalTrace1();
// PC 0x00600DB0 GOG 0x00600DC0
CSWSJournal::CSWSJournal()
{
}
// PC 0x00600DC0 GOG 0x00600C80
CSWSJournal::~CSWSJournal()
{
}
// PC 0x006009E0 GOG 0x006009C0
void CSWSJournal::SetPicture(const CExoString& sName, int nPicture, int nFlag)
{
}
// PC 0x00600AF0 GOG 0x00600AD0
void SJournalEntry::Update(SJournalEntry entry, unsigned short nFlags)
{
    NwsjournalTrace1();
    m_sPlotID = entry.m_sPlotID;
    if ((nFlags & 1) || (nFlags & 0x8000))
        m_lsName = entry.m_lsName;
    if ((nFlags & 2) || (nFlags & 0x8000))
        m_lsText = entry.m_lsText;
    if ((nFlags & 4) || (nFlags & 0x8000))
        m_nDate = entry.m_nDate;
    if ((nFlags & 8) || (nFlags & 0x8000))
        m_nTime = entry.m_nTime;
    if ((nFlags & 0x10) || (nFlags & 0x8000))
        m_nState = entry.m_nState;
    if ((nFlags & 0x20) || (nFlags & 0x8000))
        m_nPriority = entry.m_nPriority;
    if ((nFlags & 0x80) || (nFlags & 0x8000))
        m_bCompleted = entry.m_bCompleted;
    if (nFlags & 0x8200)
        m_nPlotIndex = entry.m_nPlotIndex;
    if (nFlags & 0x8400)
        m_nPlanetID = entry.m_nPlanetID;
    if (nFlags & 0x8800)
        m_nXP = entry.m_nXP;
    m_bNew = 1;
}
// PC 0x00600C00 GOG 0x00600BE0
int CSWSJournal::GetQuestEntryXP(const CExoString& sName)
{
    int nNum = m_lstEntries.num;
    for (int nIndex = 0; nIndex < nNum; nIndex++)
    {
        if (m_lstEntries[nIndex].m_sPlotID.CompareNoCase(sName))
        {
            return m_lstEntries[nIndex].m_nXP;
        }
    }
    return 0;
}
// PC 0x00600C50 GOG 0x00600C30
int CSWSJournal::GetState(const CExoString& sName)
{
    int nNum = m_lstEntries.num;
    for (int nIndex = 0; nIndex < nNum; nIndex++)
    {
        if (m_lstEntries[nIndex].m_sPlotID.CompareNoCase(sName))
        {
            return m_lstEntries[nIndex].m_nState;
        }
    }
    return 0;
}
// PC 0x00600DF0 GOG 0x00600DD0
void CSWSJournal::SetState(const CExoString& sName, unsigned long nState,
                           int nFlag)
{
    SJournalEntry entry;
    int nNum = m_lstEntries.num;
    int bFound = 0;
    int bSuccess = 1;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CSWSPlayerJournalQuestUpdates update;
    CExoLocString lsString;
    CExoString sTag;
    unsigned short nFlags = 0;
    int bCategoryMissing = 1;
    int nEntry;

    if (nFlag != 0)
    {
        nFlags = CSWSPlayerJournalQuestUpdates::QUEST_QUIET;
    }

    int nIndex = 0;
    for (; nIndex < nNum; nIndex++)
    {
        if (m_lstEntries.element[nIndex].m_sPlotID.CompareNoCase(sName))
        {
            nEntry = nIndex;
            bFound = 1;
            break;
        }
    }
    if (bFound == 0)
    {
        entry.m_sPlotID = sName.LowerCase();
        entry.m_nState = nState;
        m_lstEntries.Add(entry);
        nEntry = m_lstEntries.num - 1;
        nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_NEW;
    }

    CResGFF* pGFF = new CResGFF(RESTYPE_JRL, "JRL ", "Global");
    if (pGFF->m_bLoaded == 0)
    {
        delete pGFF;
        return;
    }

    CResStruct cRoot;
    CResList cCategoryList;
    CResStruct cCategory;
    CResList cEntryList;
    CResStruct cEntry;

    pGFF->GetTopLevelStruct(&cRoot);
    pGFF->GetList(&cCategoryList, &cRoot, "Categories");
    int nCategories = pGFF->GetListCount(&cCategoryList);
    for (int nCategory = 0; nCategory < nCategories; nCategory++)
    {
        pGFF->GetListElement(&cCategory, &cCategoryList, nCategory);
        {
            CExoString sDefault("");
            sTag = pGFF->ReadFieldCExoString(&cCategory, "Tag", bSuccess,
                                             sDefault);
        }
        if (bSuccess && sTag != "" && sName.CompareNoCase(sTag))
        {
            bCategoryMissing = 0;

            if ((lsString = pGFF->ReadFieldCExoLocString(
                     &cCategory, "Name", bSuccess, CExoLocString())) !=
                m_lstEntries.element[nEntry].m_lsName)
            {
                nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_NAME;
                m_lstEntries.element[nEntry].m_lsName = lsString;
            }

            unsigned long nPriority =
                pGFF->ReadFieldDWORD(&cCategory, "Priority", bSuccess, 0);
            if (nPriority != m_lstEntries.element[nEntry].m_nPriority)
            {
                nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_PRIORITY;
                m_lstEntries.element[nEntry].m_nPriority = nPriority;
            }

            if (m_lstEntries.element[nEntry].m_nState != nState)
            {
                nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_STATE;
                m_lstEntries.element[nEntry].m_nState = nState;
            }

            unsigned short nPicture =
                pGFF->ReadFieldWORD(&cCategory, "Picture", bSuccess, 0);
            if (nPicture != m_lstEntries.element[nEntry].m_nPicture)
            {
                nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_PICTURE;
                m_lstEntries.element[nEntry].m_nPicture = nPicture;
            }

            int nPlotIndex =
                pGFF->ReadFieldINT(&cCategory, "PlotIndex", bSuccess, 0);
            if (nPlotIndex != m_lstEntries.element[nEntry].m_nPlotIndex)
            {
                nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_PLOTINDEX;
                m_lstEntries.element[nEntry].m_nPlotIndex = nPlotIndex;
            }

            int nPlanetID =
                pGFF->ReadFieldINT(&cCategory, "PlanetID", bSuccess, 0);
            if (nPlanetID != m_lstEntries.element[nEntry].m_nPlanetID)
            {
                nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_PLANETID;
                m_lstEntries.element[nEntry].m_nPlanetID = nPlanetID;
            }

            pGFF->GetList(&cEntryList, &cCategory, "EntryList");
            int nEntries = pGFF->GetListCount(&cEntryList);
            for (int nEntryIndex = 0; nEntryIndex < nEntries; nEntryIndex++)
            {
                pGFF->GetListElement(&cEntry, &cEntryList, nEntryIndex);
                if (pGFF->ReadFieldDWORD(&cEntry, "ID", bSuccess, 0) == nState)
                {
                    if ((lsString = pGFF->ReadFieldCExoLocString(
                             &cEntry, "Text", bSuccess, CExoLocString())) !=
                        m_lstEntries.element[nEntry].m_lsText)
                    {
                        nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_TEXT;
                        m_lstEntries.element[nEntry].m_lsText = lsString;
                    }

                    float fXPPercentage = pGFF->ReadFieldFLOAT(
                        &cEntry, "XP_Percentage", bSuccess, 0.0f);
                    if (bSuccess)
                    {
                        int nPlotXP = 0;
                        CTwoDimArrays*& p2DArrays = g_pRules->m_p2DArrays;
#ifdef _XBOX
                        CTwoDimArrays* pArraysForPlotXP = g_pRules->Get2DArrays();
                        EXOXBOXASSERT(pArraysForPlotXP->m_pPlotXP);
                        C2DA* pPlotXP = pArraysForPlotXP->m_pPlotXP;
#else
                        C2DA* pPlotXP = g_pRules->Get2DArrays()->m_pPlotXP;
#endif
                        EXOLOG("CSWSJournal::SetState: plot XP table %p, flag %p",
                               p2DArrays, &nFlag);
                        pPlotXP->GetINTEntry(
                            m_lstEntries.element[nEntry].m_nPlotIndex, "XP",
                            &nPlotXP);
                        int nXP = (int)(nPlotXP * fXPPercentage + 0.5f);
                        if (nXP != m_lstEntries.element[nEntry].m_nXP)
                        {
                            nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_XP;
                            m_lstEntries.element[nEntry].m_nXP = nXP;
                        }
                    }

                    int nEnd =
                        pGFF->ReadFieldWORD(&cEntry, "End", bSuccess, 0);
                    if (nEnd != m_lstEntries.element[nEntry].m_bCompleted)
                    {
                        m_lstEntries[nEntry].m_bCompleted = nEnd;
                    }
                    nFlags |= CSWSPlayerJournalQuestUpdates::QUEST_COMPLETED;
                    break;
                }
            }

            m_lstEntries.element[nEntry].m_bNew = (nFlag == 0);

            CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
            int nPlayerCreatureId = pExoApp->GetPlayerCreatureId();
            CSWSPlayer* pPlayer = pExoApp->GetClientObjectByObjectId(
                nPlayerCreatureId);
            if (pPlayer != NULL)
            {
                if (pPlayer->m_pJournalGUI->GetFullUpdate() != 0)
                {
                    pMessage->SendServerToPlayerJournalAddQuest(
                        pPlayer, sName.LowerCase(), nState,
                        m_lstEntries.element[nEntry].m_nPriority,
                        m_lstEntries.element[nEntry].m_nPicture,
                        m_lstEntries.element[nEntry].m_bCompleted,
                        m_lstEntries.element[nEntry].m_nDate,
                        m_lstEntries.element[nEntry].m_nTime,
                        m_lstEntries.element[nEntry].m_lsName,
                        m_lstEntries.element[nEntry].m_lsText,
                        m_lstEntries.element[nEntry].m_nPlotIndex,
                        m_lstEntries.element[nEntry].m_nPlanetID,
                        m_lstEntries.element[nEntry].m_nXP);
                }
                else if (pPlayer->m_pJournalGUI->m_bUpdateRequired == 0)
                {
                    CExoArrayList<CSWSPlayerJournalQuestUpdates>* pUpdates =
                        &pPlayer->m_pJournalGUI->m_lstQuestUpdates;
                    if (pUpdates != NULL)
                    {
                        int bQueued = 0;
                        int nCount = pUpdates->num;
                        int nUpdate = 0;
                        for (; nUpdate < nCount; nUpdate++)
                        {
                            if (pUpdates->element[nUpdate].m_sTag.CompareNoCase(
                                    sName))
                            {
                                if (bFound == 0)
                                {
                                    pUpdates->element[nUpdate].m_nQuest =
                                        CSWSPlayerJournalQuestUpdates::
                                            QUEST_NEW;
                                }
                                else
                                {
                                    pUpdates->element[nUpdate].m_nQuest &=
                                        ~CSWSPlayerJournalQuestUpdates::
                                            QUEST_CLEAR_REMOVED;
                                    pUpdates->element[nUpdate].m_nQuest |=
                                        nFlags;
                                }
                                bQueued = 1;
                                break;
                            }
                        }
                        if (bQueued == 0)
                        {
                            if (bFound == 0)
                            {
                                update.m_nQuest |=
                                    CSWSPlayerJournalQuestUpdates::QUEST_NEW;
                            }
                            else
                            {
                                update.m_nQuest = nFlags;
                            }
                            update.m_sTag = sName.LowerCase();
                            pUpdates->Add(update);
                        }
                    }
                }
                if (nFlag == 0)
                {
                    pMessage->SendServerToPlayerJournalUpdated(
                        pPlayer, 1, m_lstEntries.element[nEntry].m_bCompleted,
                        m_lstEntries.element[nEntry].m_lsName);
                }
            }
            break;
        }
    }

    delete pGFF;

    if (bCategoryMissing != 0)
    {
        CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
        int nPlayerCreatureId = pExoApp->GetPlayerCreatureId();
        CSWSPlayer* pPlayer = pExoApp->GetClientObjectByObjectId(
            nPlayerCreatureId);
        if (pPlayer != NULL && pPlayer->m_pJournalGUI != NULL &&
            pPlayer->m_pJournalGUI->m_bFullUpdate != 0)
        {
            pMessage->SendServerToPlayerJournalAddQuest(
                pPlayer, sName.LowerCase(), nState,
                m_lstEntries.element[nEntry].m_nPriority,
                m_lstEntries.element[nEntry].m_nPicture,
                m_lstEntries.element[nEntry].m_bCompleted,
                m_lstEntries.element[nEntry].m_nDate,
                m_lstEntries.element[nEntry].m_nTime,
                m_lstEntries.element[nEntry].m_lsName,
                m_lstEntries.element[nEntry].m_lsText,
                m_lstEntries.element[nEntry].m_nPlotIndex,
                m_lstEntries.element[nEntry].m_nPlanetID,
                m_lstEntries.element[nEntry].m_nXP);
        }
    }
}
// PC 0x006017A0 GOG 0x00601780
void CSWSJournal::SetDate(const CExoString& sName, unsigned long nDate,
                          int nFlag)
{
    SJournalEntry entry;
    CSWSPlayerJournalQuestUpdates update;
    int bFound = 0;
    int nIndex = 0;
    int nNum = m_lstEntries.num;

    for (; nIndex < nNum; nIndex++)
    {
        if (m_lstEntries[nIndex].m_sPlotID.CompareNoCase(sName))
        {
            m_lstEntries[nIndex].m_nDate = nDate;
            bFound = 1;
            break;
        }
    }
    if (bFound == 0)
    {
        entry.m_sPlotID = sName.LowerCase();
        entry.m_nDate = nDate;
        m_lstEntries.Add(entry);
    }

    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer =
        pExoApp->GetClientObjectByObjectId(pExoApp->GetPlayerCreatureId());
    if (pPlayer != NULL)
    {
        if (pPlayer->m_pJournalGUI->m_bUpdateRequired == 0)
        {
            CExoArrayList<CSWSPlayerJournalQuestUpdates>* pUpdates =
                &pPlayer->m_pJournalGUI->m_lstQuestUpdates;
            if (pUpdates != NULL)
            {
                int bQueued = 0;
                int nUpdate = 0;
                int nCount = pUpdates->num;
                for (; nUpdate < nCount; nUpdate++)
                {
                    if ((*pUpdates)[nUpdate].m_sTag.CompareNoCase(sName))
                    {
                        if (bFound == 0)
                        {
                            (*pUpdates)[nUpdate].m_nQuest =
                                CSWSPlayerJournalQuestUpdates::QUEST_NEW;
                        }
                        else
                        {
                            (*pUpdates)[nUpdate].m_nQuest &=
                                ~CSWSPlayerJournalQuestUpdates::
                                    QUEST_CLEAR_REMOVED;
                            (*pUpdates)[nUpdate].m_nQuest |=
                                CSWSPlayerJournalQuestUpdates::QUEST_DATE;
                        }
                        if (nFlag != 0)
                        {
                            (*pUpdates)[nUpdate].m_nQuest |=
                                CSWSPlayerJournalQuestUpdates::QUEST_QUIET;
                        }
                        bQueued = 1;
                        break;
                    }
                }
                if (bQueued == 0)
                {
                    update.m_sTag = sName.LowerCase();
                    update.m_nQuest =
                        bFound != 0 ? CSWSPlayerJournalQuestUpdates::QUEST_DATE
                                    : CSWSPlayerJournalQuestUpdates::QUEST_NEW;
                    if (nFlag != 0)
                    {
                        update.m_nQuest |=
                            CSWSPlayerJournalQuestUpdates::QUEST_QUIET;
                    }
                    pUpdates->Add(update);
                }
            }
        }
    }
}
// PC 0x00601A00 GOG 0x006019E0
void CSWSJournal::SetTime(const CExoString& sName, unsigned long nTime,
                          int nFlag)
{
    SJournalEntry entry;
    CSWSPlayerJournalQuestUpdates update;
    int bFound = 0;
    int nIndex = 0;
    int nNum = m_lstEntries.num;

    for (; nIndex < nNum; nIndex++)
    {
        if (m_lstEntries[nIndex].m_sPlotID.CompareNoCase(sName))
        {
            m_lstEntries[nIndex].m_nTime = nTime;
            bFound = 1;
            break;
        }
    }
    if (bFound == 0)
    {
        entry.m_sPlotID = sName.LowerCase();
        entry.m_nTime = nTime;
        m_lstEntries.Add(entry);
    }

    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer =
        pExoApp->GetClientObjectByObjectId(pExoApp->GetPlayerCreatureId());
    if (pPlayer != NULL)
    {
        if (pPlayer->m_pJournalGUI->m_bUpdateRequired == 0)
        {
            CExoArrayList<CSWSPlayerJournalQuestUpdates>* pUpdates =
                &pPlayer->m_pJournalGUI->m_lstQuestUpdates;
            if (pUpdates != NULL)
            {
                int bQueued = 0;
                int nUpdate = 0;
                int nCount = pUpdates->num;
                for (; nUpdate < nCount; nUpdate++)
                {
                    if ((*pUpdates)[nUpdate].m_sTag.CompareNoCase(sName))
                    {
                        if (bFound == 0)
                        {
                            (*pUpdates)[nUpdate].m_nQuest =
                                CSWSPlayerJournalQuestUpdates::QUEST_NEW;
                        }
                        else
                        {
                            (*pUpdates)[nUpdate].m_nQuest &=
                                ~CSWSPlayerJournalQuestUpdates::
                                    QUEST_CLEAR_REMOVED;
                            (*pUpdates)[nUpdate].m_nQuest |=
                                CSWSPlayerJournalQuestUpdates::QUEST_TIME;
                        }
                        if (nFlag != 0)
                        {
                            (*pUpdates)[nUpdate].m_nQuest |=
                                CSWSPlayerJournalQuestUpdates::QUEST_QUIET;
                        }
                        bQueued = 1;
                        break;
                    }
                }
                if (bQueued == 0)
                {
                    update.m_sTag = sName.LowerCase();
                    update.m_nQuest =
                        bFound != 0 ? CSWSPlayerJournalQuestUpdates::QUEST_TIME
                                    : CSWSPlayerJournalQuestUpdates::QUEST_NEW;
                    if (nFlag != 0)
                    {
                        update.m_nQuest |=
                            CSWSPlayerJournalQuestUpdates::QUEST_QUIET;
                    }
                    pUpdates->Add(update);
                }
            }
        }
    }
}
// PC 0x00601C60 GOG 0x00601C40
void CSWSJournal::Destroy(const CExoString& sName)
{
    int nNum = m_lstEntries.num;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CSWSPlayerJournalQuestUpdates update;
    CExoLocString lsName;

    for (int nIndex = nNum - 1; nIndex >= 0; nIndex--)
    {
        if (m_lstEntries[nIndex].m_sPlotID.CompareNoCase(sName))
        {
            int bCompleted = m_lstEntries[nIndex].m_bCompleted;
            lsName = m_lstEntries[nIndex].m_lsName;
            m_lstEntries.DelIndex(nIndex);

            CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
            CSWSPlayer* pPlayer = pExoApp->GetClientObjectByObjectId(
                pExoApp->GetPlayerCreatureId());
            if (pPlayer == NULL)
            {
                return;
            }
            int bFullUpdate = pPlayer->m_pJournalGUI->m_bFullUpdate;
            if (bFullUpdate != 0)
            {
                pMessage->SendServerToPlayerJournalRemoveQuest(
                    pPlayer, sName.LowerCase());
            }
            else if (pPlayer->m_pJournalGUI->m_bUpdateRequired == 0)
            {
                CExoArrayList<CSWSPlayerJournalQuestUpdates>* pUpdates =
                    &pPlayer->m_pJournalGUI->m_lstQuestUpdates;
                if (pUpdates != NULL)
                {
                    int bQueued = 0;
                    int nUpdate = 0;
                    int nCount = pUpdates->num;
                    for (; nUpdate < nCount; nUpdate++)
                    {
                        if ((*pUpdates)[nUpdate].m_sTag.CompareNoCase(
                                sName))
                        {
                            (*pUpdates)[nUpdate].m_nQuest =
                                CSWSPlayerJournalQuestUpdates::QUEST_REMOVED;
                            bQueued = 1;
                            break;
                        }
                    }
                    if (bQueued == 0)
                    {
                        update.m_nQuest =
                            CSWSPlayerJournalQuestUpdates::QUEST_REMOVED;
                        update.m_sTag = sName.LowerCase();
                        pUpdates->Add(update);
                    }
                }
            }
            if (bFullUpdate == 0)
            {
                pMessage->SendServerToPlayerJournalUpdated(pPlayer, 1,
                                                           bCompleted, lsName);
            }
            return;
        }
    }
}

static inline void NwsjournalTrace1()
{
}
