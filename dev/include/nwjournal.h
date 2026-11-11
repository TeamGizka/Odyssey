#ifndef __NWJOURNAL_H__
#define __NWJOURNAL_H__

#include "exobase.h"

class CWorldJournalEntry
{
public:

    CExoString m_sText;
    CExoString m_sTag;
    unsigned long m_nCalendarDay;
    unsigned long m_nTimeOfDay;
    unsigned long m_nStringRef;
    int m_nIndex;
    unsigned long m_bStringRef : 1;
};

class SJournalEntry
{
public:
    CExoLocString m_lsName;
    CExoLocString m_lsText;
    unsigned long m_nDate;
    unsigned long m_nTime;
    CExoString m_sPlotID;
    unsigned long m_nState;
    unsigned long m_nPriority;
    unsigned short m_nPicture;
    unsigned int : 0;
    unsigned int m_bCompleted : 1;
    unsigned int m_bChanged : 1;
    unsigned int m_bNew : 1;
    unsigned int m_nUnusedFlags : 29;
    int m_nPlotIndex;
    int m_nPlanetID;
    int m_nXP;
    // PC 0x005C53E0  Xbox 0x00070A50
    SJournalEntry()
        : m_nDate(-1),
          m_nTime(-1),
          m_nState(-1),
          m_nPriority(-1)
    {
        m_bCompleted = 0;
        m_bChanged = 0;
        m_bNew = 0;
        m_nPlanetID = -1;
        m_nPicture = -1;
        m_nPlotIndex = 0;
        m_nXP = 0;
    }
    SJournalEntry* operator=(const SJournalEntry& entry)  // PC 0x005C5470  Xbox 0x00205CE0
    {
        m_lsName = entry.m_lsName;
        m_lsText = entry.m_lsText;
        m_nDate = entry.m_nDate;
        m_nTime = entry.m_nTime;
        m_sPlotID = entry.m_sPlotID;
        m_nState = entry.m_nState;
        m_nPriority = entry.m_nPriority;
        m_nPicture = entry.m_nPicture;
        m_bCompleted = entry.m_bCompleted;
        m_bChanged = entry.m_bChanged;
        m_bNew = entry.m_bNew;
        m_nPlotIndex = entry.m_nPlotIndex;
        m_nPlanetID = entry.m_nPlanetID;
        m_nXP = entry.m_nXP;
        return this;
    }
    ~SJournalEntry()  // PC 0x005C5500  Xbox 0x00023670
    {
    }
    // PC 0x005C5560  Xbox 0x00077630
    SJournalEntry(const SJournalEntry& entry)
        : m_lsName(entry.m_lsName),
          m_lsText(entry.m_lsText),
          m_nDate(entry.m_nDate),
          m_nTime(entry.m_nTime),
          m_sPlotID(entry.m_sPlotID),
          m_nState(entry.m_nState),
          m_nPriority(entry.m_nPriority),
          m_nPicture(entry.m_nPicture)
    {
        m_bCompleted = entry.m_bCompleted;
        m_bChanged = entry.m_bChanged;
        m_bNew = entry.m_bNew;
        m_nPlotIndex = entry.m_nPlotIndex;
        m_nPlanetID = entry.m_nPlanetID;
        m_nXP = entry.m_nXP;
    }

    void Update(SJournalEntry entry, unsigned short nFlags);
};

class CSWSJournalEntry
{
public:
    unsigned char m_aUnknown00[16];
    unsigned long m_nDate;
    unsigned long m_nTime;
    CExoString m_sPlotID;
    unsigned long m_nState;
    unsigned char m_aUnknown24[24];
};

class CSWSJournal
{
public:
    CSWSJournal();
    ~CSWSJournal();
    int GetQuestEntryXP(const CExoString& sName);
    int GetState(const CExoString& sName);
    void SetState(const CExoString& sName, unsigned long nState, int nFlag);
    void SetDate(const CExoString& sName, unsigned long nDate, int nFlag);
    void SetTime(const CExoString& sName, unsigned long nTime, int nFlag);
    void SetPicture(const CExoString& sName, int nPicture, int nFlag);
    void Destroy(const CExoString& sName);

    CExoArrayList<SJournalEntry> m_lstEntries;
};

extern int g_nJournalSortType;

#endif
