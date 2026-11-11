#ifndef __NWSWAYPOINT_H__
#define __NWSWAYPOINT_H__

#include "nwsobject.h"

class CResGFF;
class CResRef;
class CResStruct;
class CSWSArea;

class CSWSWaypoint : public CSWSObject
{
public:
    CSWSWaypoint(unsigned long oidWaypoint);
    virtual ~CSWSWaypoint();

    virtual void AIUpdate();
    // PC 0x00698BD0
    virtual CSWSWaypoint* AsSWSWaypoint() { return this; }

    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    virtual CExoLocString* GetFirstName()  // PC 0x0056F440
    {
        return &m_lsLocalizedName;
    }

    int GetMapNoteEnabled() { return m_bMapNoteEnabled; }

    int LoadWaypoint(CResGFF* pRes, CResStruct* pStruct);
    int SaveWaypoint(CResGFF* pRes, CResStruct* pStruct);
    void RemoveFromArea();
    int LoadFromTemplate(const CResRef& cResRef);
    void AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                   int bLoadingGame);

    int m_bHasMapNote;
    int m_bMapNoteEnabled;
    CExoLocString m_lsMapNote;
    CExoLocString m_lsLocalizedName;
};

#endif
