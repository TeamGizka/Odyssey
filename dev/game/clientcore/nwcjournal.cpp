#include "exobase.h"
#include "nwcjournal.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "exotlktable.h"
#include "worldtimer.h"

#include "clientexoappinternal.h"
#include "guiingame.h"
#include "nwmessage.h"
#include "swguiingame.h"

inline CSWCJournal::~CSWCJournal()
{
}

const unsigned long CSWCJournal::STRREF_CSWCJOURNAL_QUESTENTRYADDED = 32296;
const unsigned long CSWCJournal::STRREF_CSWCJOURNAL_WORLDENTRYADDED = 0;
// PC 0x006CBC00
void CSWCSortedJournalArrayList::SortList()
{
    int (CSWCSortedJournalArrayList::*pfnCompare)(int, int);
    if (m_nSortBy == SORT_BY_DATE)
    {
        pfnCompare = &CSWCSortedJournalArrayList::SortItemsByDate;
    }
    else if (m_nSortBy == SORT_BY_NAME)
    {
        pfnCompare = &CSWCSortedJournalArrayList::SortItemsByName;
    }
    else if (m_nSortBy == SORT_BY_PRIORITY)
    {
        pfnCompare = &CSWCSortedJournalArrayList::SortItemsByPriority;
    }
    else
    {
        pfnCompare = &CSWCSortedJournalArrayList::SortItemsByPlanet;
    }

    int nCount = m_lstEntries.num;
    for (int i = 0; i < nCount; i++)
    {
        int nBest = i;
        for (int j = i + 1; j < nCount; j++)
        {
            if ((this->*pfnCompare)(m_lstOrder.element[nBest],
                                    m_lstOrder.element[j]) < 0)
            {
                nBest = j;
            }
        }
        int* pOrder = m_lstOrder.element;
        int nSwap = pOrder[i];
        pOrder[i] = pOrder[nBest];
        m_lstOrder.element[nBest] = nSwap;
    }
    m_bHasChanged = 1;
}

CWorldJournalEntry::~CWorldJournalEntry()
{
}
// PC 0x006CB480
void CSWCJournal::EnableSort()
{
    m_bSortEnabled = 1;
}
// PC 0x006CB490
void CSWCJournal::DisableSort()
{
    m_bSortEnabled = 0;
}
// PC 0x006CB5A0
int CSWCSortedJournalArrayList::SortItemsByDate(int nFirst, int nSecond)
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    return pApp->GetWorldTimer()->CompareWorldTimes(
        m_lstEntries[nFirst].m_nDate,
        m_lstEntries[nFirst].m_nTime,
        m_lstEntries[nSecond].m_nDate,
        m_lstEntries[nSecond].m_nTime);
}
// PC 0x006CB5E0
int CSWCSortedJournalArrayList::SortItemsByName(int nFirst, int nSecond)
{
    CExoString sFirst;
    CExoString sSecond;
    int nLanguage = g_pAppManager->m_pClientExoApp->GetClientLanguage();
    m_lstEntries[nFirst].m_lsName.GetString(nLanguage, &sFirst, 0);
    m_lstEntries[nSecond].m_lsName.GetString(nLanguage, &sSecond, 0);
    if (sFirst < sSecond)
    {
        return 1;
    }
    if (sFirst > sSecond)
    {
        return -1;
    }
    return 0;
}
// PC 0x006CB720
int CSWCSortedJournalArrayList::SortItemsByPriority(int nFirst, int nSecond)
{
    unsigned long nFirstPriority = m_lstEntries.element[nFirst].m_nPriority;
    unsigned long nSecondPriority = m_lstEntries.element[nSecond].m_nPriority;
    if (nFirstPriority > nSecondPriority)
    {
        return -1;
    }
    if (nFirstPriority < nSecondPriority)
    {
        return 1;
    }
    return SortItemsByDate(nFirst, nSecond);
}
// PC 0x006CB770
int CSWCSortedJournalArrayList::SortItemsByPlanet(int nFirst, int nSecond)
{
    int nFirstPlanet = m_lstEntries.element[nFirst].m_nPlanetID;
    int nSecondPlanet = m_lstEntries.element[nSecond].m_nPlanetID;
    if (nFirstPlanet > nSecondPlanet)
    {
        return 1;
    }
    if (nFirstPlanet < nSecondPlanet)
    {
        return -1;
    }
    return SortItemsByDate(nFirst, nSecond);
}
// PC 0x006CB7C0
void CSWCJournal::Remove(const CExoString& sTag)
{
    int nActive = m_lstActive.m_lstEntries.num;
    for (int i = 0; i < nActive; i++)
    {
        if (m_lstActive.m_lstEntries.element[i].m_sPlotID == sTag)
        {
            m_lstActive.m_lstEntries.DelIndex(i);
            for (int j = 0; j < nActive; j++)
            {
                if (m_lstActive.m_lstOrder.element[j] == i)
                {
                    m_lstActive.m_lstOrder.DelIndex(j);
                    nActive = m_lstActive.m_lstOrder.num;
                }
                if (j != nActive)
                {
                    if (m_lstActive.m_lstOrder.element[j] > i)
                    {
                        m_lstActive.m_lstOrder.element[j]--;
                    }
                }
            }
            m_lstActive.m_bHasChanged = 1;
            return;
        }
    }
    int nDone = m_lstDone.m_lstEntries.num;
    for (int k = 0; k < nDone; k++)
    {
        if (m_lstDone.m_lstEntries.element[k].m_sPlotID == sTag)
        {
            m_lstDone.m_lstEntries.DelIndex(k);
            for (int m = 0; m < nDone; m++)
            {
                if (m_lstDone.m_lstOrder.element[m] == k)
                {
                    m_lstDone.m_lstOrder.DelIndex(m);
                    nDone = m_lstDone.m_lstOrder.num;
                }
                if (m != nDone)
                {
                    if (m_lstDone.m_lstOrder.element[m] > k)
                    {
                        int& nEntry = m_lstDone.m_lstOrder.element[m];
                        nEntry--;
                    }
                }
            }
            m_lstDone.m_bHasChanged = 1;
            return;
        }
    }
}
// PC 0x006CB930
void CSWCJournal::SetJournalPicture(const CExoString& sTag, int nPicture)
{
    int nDone = m_lstDone.m_lstEntries.num;
    for (int i = 0; i < nDone; i++)
    {
        if (m_lstDone.m_lstEntries[i].m_sPlotID == sTag)
        {
            m_lstDone.m_lstEntries[i].m_nPicture =
                (unsigned short)nPicture;
            m_lstDone.m_lstEntries[i].m_bNew = 1;
            m_lstDone.m_bHasChanged = 1;
            return;
        }
    }
    CExoArrayList<SJournalEntry>* pActive = &m_lstActive.m_lstEntries;
    int nActive = pActive->num;
    for (int j = 0; j < nActive; j++)
    {
        if (pActive->element[j].m_sPlotID == sTag)
        {
            pActive->element[j].m_nPicture = (unsigned short)nPicture;
            pActive->element[j].m_bNew = 1;
            m_lstActive.m_bHasChanged = 1;
            return;
        }
    }
}
// PC 0x006CB9E0
CSWJournalEntry* CSWCJournal::GetActiveEntryAt(int nIndex)
{
    if (nIndex >= 0 && nIndex < m_lstActive.m_lstEntries.num)
    {
        return (CSWJournalEntry*)&m_lstActive.m_lstEntries
            [m_lstActive.m_lstOrder[nIndex]];
    }
    return 0;
}
// PC 0x006CBA10
CSWJournalEntry* CSWCJournal::GetDoneEntryAt(int nIndex)
{
    if (nIndex >= 0 && nIndex < m_lstDone.m_lstEntries.num)
    {
        return (CSWJournalEntry*)&m_lstDone.m_lstEntries
            [m_lstDone.m_lstOrder[nIndex]];
    }
    return 0;
}
// PC 0x005ED200
int CSWCJournal::GetNumActiveEntries()
{
    return m_lstActive.m_lstEntries.num;
}
// PC 0x006CB4A0
int CSWCJournal::GetNumDoneEntries()
{
    return m_lstDone.m_lstEntries.num;
}
// PC 0x006CBCE0
void CSWCJournal::Add(SJournalEntry entry, unsigned short nFlags)
{
    int nDoneCount = m_lstDone.m_lstEntries.num;
    int i = 0;
    int bDone = entry.m_bCompleted;
    int bFound = 0;
    int nActiveCount = m_lstActive.m_lstEntries.num;
    SJournalEntry existing;
    CExoString sMessage;

    if (bDone == 1)
    {
        for (i = 0; i < nActiveCount && bFound == 0; i++)
        {
            if (m_lstActive.m_lstEntries.element[i].m_sPlotID ==
                entry.m_sPlotID)
            {
                existing = m_lstActive.m_lstEntries.element[i];
                Remove(entry.m_sPlotID);
                bFound = 1;
            }
        }
    }
    else
    {
        for (i = 0; i < nDoneCount && bFound == 0; i++)
        {
            if (m_lstDone.m_lstEntries.element[i].m_sPlotID == entry.m_sPlotID)
            {
                existing = m_lstDone.m_lstEntries.element[i];
                Remove(entry.m_sPlotID);
                bFound = 1;
            }
        }
    }
    if (bFound != 0)
    {
        existing.Update(entry, nFlags);
        entry = existing;
    }

    if (bDone == 1)
    {
        for (i = 0; i < nDoneCount; i++)
        {
            if (m_lstDone.m_lstEntries.element[i].m_sPlotID == entry.m_sPlotID)
            {
                m_lstDone.m_lstEntries[i].Update(entry, nFlags);
                if (m_bSortEnabled)
                {
                    m_lstDone.SortList();
                }
                return;
            }
        }
    }
    else
    {
        for (i = 0; i < nActiveCount; i++)
        {
            if (m_lstActive.m_lstEntries.element[i].m_sPlotID ==
                entry.m_sPlotID)
            {
                m_lstActive.m_lstEntries[i].Update(entry, nFlags);
                if (m_bSortEnabled)
                {
                    m_lstActive.SortList();
                }
                return;
            }
        }
    }

    if (bFound == 0 && (nFlags & 0x8000) == 0)
    {
        sMessage.Format(
            "Journal Crash! Partial Update on non-existant entry; plot:%s "
            "flags:%i",
            entry.m_sPlotID.CStr(), nFlags);
        EXOXBOXASSERT(0);
    }

    if (bDone == 1)
    {
        int nIndex = m_lstDone.m_lstEntries.num;
        m_lstDone.m_lstEntries.Add(entry);
        m_lstDone.m_lstOrder.Add(nIndex);
        if (m_bSortEnabled)
        {
            m_lstDone.SortList();
        }
    }
    else
    {
        int nIndex = m_lstActive.m_lstEntries.num;
        m_lstActive.m_lstEntries.Add(entry);
        m_lstActive.m_lstOrder.Add(nIndex);
        if (m_bSortEnabled)
        {
            m_lstActive.SortList();
        }
    }
}
// PC 0x006CC1D0
void CSWCJournal::ClearAll()
{
    m_lstDone.m_lstEntries.SetSize(0);
    m_lstDone.m_lstOrder.SetSize(0);
    m_lstActive.m_lstEntries.SetSize(0);
    m_lstActive.m_lstOrder.SetSize(0);
}
// PC 0x006CC270
void CSWCJournal::CallFullUpdateComplete()
{
    m_lstActive.SortList();
    m_lstDone.SortList();
}
// PC 0x006CC290
void CSWCJournal::SetActiveSortBy(int nSortBy)
{
    if (nSortBy == CSWCSortedJournalArrayList::SORT_BY_DATE ||
        nSortBy == CSWCSortedJournalArrayList::SORT_BY_NAME ||
        nSortBy == CSWCSortedJournalArrayList::SORT_BY_PRIORITY ||
        nSortBy == CSWCSortedJournalArrayList::SORT_BY_PLANET)
    {
        m_lstActive.m_nSortBy = nSortBy;
        if (m_bSortEnabled)
        {
            m_lstActive.SortList();
        }
    }
}
// PC 0x006CC2C0
void CSWCJournal::SetDoneSortBy(int nSortBy)
{
    if (nSortBy == CSWCSortedJournalArrayList::SORT_BY_DATE ||
        nSortBy == CSWCSortedJournalArrayList::SORT_BY_NAME ||
        nSortBy == CSWCSortedJournalArrayList::SORT_BY_PRIORITY ||
        nSortBy == CSWCSortedJournalArrayList::SORT_BY_PLANET)
    {
        m_lstDone.m_nSortBy = nSortBy;
        if (m_bSortEnabled)
        {
            m_lstDone.SortList();
        }
    }
}
// PC 0x006CC2F0
void CSWCJournal::JournalUpdated(int bFromServer, int bActive,
                                 const CExoString& sMessage)
{
    if (g_pAppManager->m_pClientExoApp->GetLoadBarState() != 0 &&
        bFromServer != 0)
    {
        CSWCJournalPendingUpdate update;
        update.m_sTag = sMessage;
        update.m_bActive = bActive;
        m_lstPendingUpdates.Add(update);
    }
    else
    {
        CSWCCMessageData* pData = new CSWCCMessageData;
        pData->SetString(0, sMessage);
        {
            CExoString sEmpty("");
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
            pClient->AddFeedbackMessage(FEEDBACK_MESSAGE_JOURNAL_UPDATED, pData, sEmpty);
        }
        g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(0, 0);
        if (bActive != 0)
        {
            g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(102, 0);
        }
        else
        {
            g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(101, 0);
        }
        g_bOverrideJournalSortType = 1;
    }
}
// PC 0x006CC470
CSWCWorldJournal::CSWCWorldJournal() : m_lstEntries(0), m_lstOrder(0)
{
    m_nSortBy = 0;
    m_bChanged = 0;
}
// PC 0x006CC4B0
CSWCWorldJournal::~CSWCWorldJournal()
{
}
// PC 0x006CC530
void CSWCWorldJournal::DeleteEntryStrRef(unsigned long nStrRef)
{
    int nCount = m_lstEntries.num;
    for (int i = 0; i < nCount; i++)
    {
        if (m_lstEntries.element[i].m_bStringRef &&
            m_lstEntries.element[i].m_nIndex == nStrRef)
        {
            m_lstEntries.DelIndex(i);
            for (int j = 0; j < nCount; j++)
            {
                if (m_lstOrder.element[j] == i)
                {
                    m_lstOrder.DelIndex(j);
                    nCount = m_lstOrder.num;
                }
                if (j != nCount)
                {
                    if (m_lstOrder.element[j] > i)
                    {
                        m_lstOrder.element[j]--;
                    }
                }
            }
        }
    }
    m_bChanged = 1;
}
// PC 0x006CC5D0
void CSWCWorldJournal::DeleteEntry(unsigned long nId)
{
    int nCount = m_lstEntries.num;
    for (int i = 0; i < nCount; i++)
    {
        if (!m_lstEntries.element[m_lstOrder.element[i]].m_bStringRef &&
            m_lstEntries.element[m_lstOrder.element[i]].m_nIndex == nId)
        {
            m_lstEntries.DelIndex(m_lstOrder.element[i]);
            int nRemoved = m_lstOrder.element[i];
            m_lstOrder.DelIndex(i);
            nCount = m_lstOrder.num;
            for (int j = 0; j < nCount; j++)
            {
                if (m_lstOrder[j] > nRemoved)
                {
                    m_lstOrder[j]--;
                }
            }
        }
    }
    m_bChanged = 1;
}
// PC 0x006CC670
void CSWCWorldJournal::DeleteAllEntries()
{
    m_lstEntries.SetSize(0);
    m_lstOrder.SetSize(0);
    m_bChanged = 1;
}

CSWCSortedJournalArrayList::CSWCSortedJournalArrayList()
    : m_lstEntries(0), m_lstOrder(0)
{
    m_nSortBy = SORT_BY_DATE;
    m_bHasChanged = 0;
}
// PC 0x006CC6D0
void CSWCWorldJournal::SortList()
{
    int (CSWCWorldJournal::*pfnCompare)(int, int, int);
    if (m_nSortBy == 0)
    {
        pfnCompare = &CSWCWorldJournal::SortItemsByDate;
    }
    else
    {
        pfnCompare = &CSWCWorldJournal::SortItemsByName;
    }

    int nCount = m_lstOrder.num;
    for (int i = 0; i < nCount; i++)
    {
        int nBest = i;
        for (int j = i + 1; j < nCount; j++)
        {
            if ((this->*pfnCompare)(m_lstOrder.element[nBest],
                                    m_lstOrder.element[j], 0) < 0)
            {
                nBest = j;
            }
        }
        int* pOrder = m_lstOrder.element;
        int nSwap = pOrder[i];
        pOrder[i] = pOrder[nBest];
        m_lstOrder.element[nBest] = nSwap;
    }
}
// PC 0x006CC790
CSWCJournal::CSWCJournal()
    : m_lstPendingUpdates(0)
{
    m_bSortEnabled = 1;
}
// PC 0x006CBA40
int CSWCWorldJournal::SortItemsByDate(int nFirst, int nSecond, int bNoTie)
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CWorldJournalEntry* pSecond = &m_lstEntries[nSecond];
    CWorldJournalEntry* pFirst = &m_lstEntries[nFirst];
    int nResult = pApp->GetWorldTimer()->CompareWorldTimes(
        pFirst->m_nCalendarDay, pFirst->m_nTimeOfDay, pSecond->m_nCalendarDay,
        pSecond->m_nTimeOfDay);
    if (nResult == 0 && bNoTie == 0)
    {
        return SortItemsByName(nFirst, nSecond, 1);
    }
    return nResult;
}
// PC 0x006CBAA0
int CSWCWorldJournal::SortItemsByName(int nFirst, int nSecond, int bNoTie)
{
    if (m_lstEntries[nFirst].m_sTag <
        m_lstEntries[nSecond].m_sTag)
    {
        return 1;
    }
    if (m_lstEntries[nFirst].m_sTag >
        m_lstEntries[nSecond].m_sTag)
    {
        return -1;
    }
    if (bNoTie == 0)
    {
        return SortItemsByDate(nFirst, nSecond, 1);
    }
    return 0;
}
// PC 0x006CBB40
int CSWCJournal::HasNewEntries()
{
    int nCount = m_lstActive.m_lstEntries.num;
    for (int i = 0; i < nCount; i++)
    {
        if (m_lstActive.m_lstEntries[i].m_bNew)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x006CC800
void CSWCWorldJournal::AddEntryStrRef(unsigned long nNameStrRef,
                                      unsigned long nTextStrRef,
                                      unsigned long nCalendarDay,
                                      unsigned long nTimeOfDay)
{
    STR_RES strName;
    STR_RES strText;
    if (g_pTlkTable->Fetch(nNameStrRef, strName, CTlkTable::GENDER_MALE) == 0)
    {
        strName.m_sString = "";
    }
    if (g_pTlkTable->Fetch(nTextStrRef, strText, CTlkTable::GENDER_MALE) == 0)
    {
        strText.m_sString = "";
    }

    CWorldJournalEntry entry;
    entry.m_nIndex = nNameStrRef;
    entry.m_sText = strName.m_sString;
    entry.m_sTag = strText.m_sString;
    entry.m_nCalendarDay = nCalendarDay;
    entry.m_nTimeOfDay = nTimeOfDay;
    entry.m_bStringRef = 1;
    int nIndex = m_lstEntries.num;
    m_lstEntries.Add(entry);
    m_lstOrder.Add(nIndex);
    SortList();
    m_bChanged = 1;
}
// PC 0x006CC9D0
void CSWCWorldJournal::AddEntry(unsigned long nId, const CExoString& sText,
                                const CExoString& sName,
                                unsigned long nCalendarDay,
                                unsigned long nTimeOfDay)
{
    CWorldJournalEntry entry;
    entry.m_nIndex = nId;
    entry.m_sText = sText;
    entry.m_sTag = sName;
    int nIndex = m_lstEntries.num;
    entry.m_nCalendarDay = nCalendarDay;
    entry.m_nTimeOfDay = nTimeOfDay;
    entry.m_bStringRef = 0;
    m_lstEntries.Add(entry);
    m_lstOrder.Add(nIndex);
    SortList();
    m_bChanged = 1;
}
