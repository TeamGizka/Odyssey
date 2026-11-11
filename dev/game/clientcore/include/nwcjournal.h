#ifndef __NWCJOURNAL_H__
#define __NWCJOURNAL_H__

#include "exobase.h"
#include "nwjournal.h"

class CSWJournalEntry
{
public:
    CExoLocString m_lsName;
    CExoLocString m_lsText;
    unsigned char m_aUnknown10[0x1C];
    unsigned long m_nJournalFlagsUnknown : 2;
    unsigned long m_bNew : 1;
    unsigned long m_nJournalFlagsUnused : 29;
    unsigned char m_aUnknown30[4];
    int m_nQuestRow;
    unsigned char m_aUnknown38[4];
};

class CSWCJournalPendingUpdate
{
public:
    CExoString m_sTag;
    int m_bActive;
};

class CSWCSortedJournalArrayList
{
public:

    enum SortBy
    {
        SORT_BY_DATE = 0,
        SORT_BY_NAME = 1,
        SORT_BY_PRIORITY = 2,
        SORT_BY_PLANET = 3
    };

    CSWCSortedJournalArrayList();

    void SortList();
    int SortItemsByDate(int nFirst, int nSecond);
    int SortItemsByName(int nFirst, int nSecond);
    int SortItemsByPriority(int nFirst, int nSecond);
    int SortItemsByPlanet(int nFirst, int nSecond);

    CExoArrayList<SJournalEntry> m_lstEntries;
    CExoArrayList<int> m_lstOrder;
    int m_nSortBy;
    int m_bHasChanged;
};

class CSWCJournal
{
public:
    static const unsigned long STRREF_CSWCJOURNAL_QUESTENTRYADDED;
    static const unsigned long STRREF_CSWCJOURNAL_WORLDENTRYADDED;

    CSWCJournal();
    ~CSWCJournal();

    void EnableSort();
    void DisableSort();
    void Add(SJournalEntry entry, unsigned short nFlags);
    void Remove(const CExoString& sTag);
    void SetJournalPicture(const CExoString& sTag, int nPicture);
    void ClearAll();
    void CallFullUpdateComplete();
    int HasNewEntries();
    void JournalUpdated(int bFromServer, int bActive,
                        const CExoString& sMessage);
    CSWJournalEntry* GetActiveEntryAt(int nIndex);
    CSWJournalEntry* GetDoneEntryAt(int nIndex);
    void ClearActiveHasChanged();
    void ClearDoneHasChanged();
    int GetNumDoneEntries();
    int GetNumActiveEntries();
    int GetDoneSortBy();
    int GetActiveSortBy()  // PC 0x00712FC0
    {
        return m_lstActive.m_nSortBy;
    }
    void SetDoneSortBy(int nSortBy);
    void SetActiveSortBy(int nSortBy);

    CSWCSortedJournalArrayList m_lstDone;
    CSWCSortedJournalArrayList m_lstActive;
    unsigned long m_bSortEnabled : 1;
    CExoArrayList<CSWCJournalPendingUpdate> m_lstPendingUpdates;
};

class CSWCWorldJournal
{
public:
    CSWCWorldJournal();
    ~CSWCWorldJournal();

    void SortList();
    int SortItemsByName(int nFirst, int nSecond, int bNoTie);
    int SortItemsByDate(int nFirst, int nSecond, int bNoTie);
    void DeleteEntryStrRef(unsigned long nId);
    void DeleteEntry(unsigned long nId);
    void DeleteAllEntries();
    void AddEntryStrRef(unsigned long nName, unsigned long nText,
                        unsigned long nCalendarDay, unsigned long nTimeOfDay);
    void AddEntry(unsigned long nId, const CExoString& sText,
                  const CExoString& sName, unsigned long nCalendarDay,
                  unsigned long nTimeOfDay);

    CExoArrayList<CWorldJournalEntry> m_lstEntries;
    CExoArrayList<int> m_lstOrder;
    unsigned char m_nSortBy;
    unsigned long m_bChanged : 1;
};

#endif
