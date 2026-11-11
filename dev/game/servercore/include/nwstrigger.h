#ifndef __NWSTRIGGER_H__
#define __NWSTRIGGER_H__

class CResGFF;
class CResList;
class CResRef;
class CResStruct;
class CSWSArea;

#include "nwsobject.h"

class CSWSTrigger : public CSWSObject
{
public:

    enum
    {
        TRIGGER_TYPE_AREA_TRANSITION = 1
    };

    enum { CURSOR_MINE = 10 };

    enum
    {
        TRIGGER_LINK_WAYPOINT = 2
    };

    enum
    {
        SCRIPT_HEARTBEAT = 0,

        SCRIPT_ONENTER = 1,
        SCRIPT_ONEXIT = 2,
        SCRIPT_USERDEFINE = 3,
        SCRIPT_ONTRAPTRIGGERED = 4,
        SCRIPT_ONDISARM = 5,
        SCRIPT_ONCLICK = 6
    };

    static const unsigned long HEARTBEAT_INTERVAL = 6000;

    static const unsigned long TRANSITION_FADE_DELAY = 500;

    enum
    {
        TRAP_ANIMATION_ACTIVE = 10143,
        TRAP_ANIMATION_INACTIVE = 10144
    };
    // PC 0x00698BD0
    virtual CSWSTrigger* AsSWSTrigger()
    {
        return this;
    }
    virtual CExoLocString* GetFirstName()  // PC 0x00587F40
    {
        return &m_lsName;
    }
    virtual ~CSWSTrigger();
    virtual void AIUpdate();
    virtual void EventHandler(unsigned long, unsigned long, void*, unsigned long,
                              unsigned long);

    int GetGeometryPointCount() { return m_nGeometryPointCount; }
    Vector* GetGeometry() { return m_pGeometry; }

    int GetFactionId() { return m_nFaction; }
    unsigned char GetCursor() { return m_nCursor; }
    unsigned char GetTrapType() { return m_nTrapType; }
    float GetHighlightHeight() { return m_fHighlightHeight; }
    CExoLocString& GetTransitionDestination()
    {
        return m_lsTransitionDestination;
    }

    int GetTrapFlag() { return m_bMineTrigger; }
    int GetDisarmDC() { return m_nTrapDisarmDC; }
    unsigned long GetCreator() { return m_oidCreator; }
    void SetFlagged(int bFlagged) { m_bTrapFlagged = bFlagged; }
    int GetFlagged() { return m_bTrapFlagged; }
    CExoArrayList<unsigned long>* GetCreatureList() { return &m_lstTrapTargets; }

    CExoString* GetScriptName(int nScript);
    int GetTargetArea();
    int CalculateNearestPoint(Vector const&, Vector*);
    void ComputeBoundingBox(float*, float*, float*, float*);
    int LineSegmentIntersectTrigger(Vector const&, Vector const&);
    int InTrigger(Vector const&);
    void LoadTriggerGeometry(CResGFF*, CResList&);
    void CreateNewGeometry(float, Vector const&, CSWSArea*);
    int GetCanFireMineOnObject(unsigned long, int);
    void OnEnterMine(int);
    void SetCreator(unsigned long);
    int GetClosestPointWithinTrigger(Vector const&, Vector*);
    int LoadTrigger(CResGFF*, CResStruct*);
    int SaveTrigger(CResGFF*, CResStruct*);
    CSWSTrigger(unsigned long);
    int LoadFromTemplate(CResRef const&);
    void RemoveFromArea();
    void AddToArea(CSWSArea*, float, float, float, int);

    CExoLocString m_lsName;
    CExoString m_sLinkedTo;
    CExoString m_sLinkedToModule;
    unsigned char m_nLinkedToFlags;
    CExoString m_asScripts[7];
    CExoString m_sKeyName;
    int m_nGeometryPointCount;
    Vector* m_pGeometry;

    int m_nGeometryInitialValue1;
    int m_nGeometryInitialValue2;
    int m_nPolygonPointCount;
    unsigned long* m_pPolygonPointOrder;
    unsigned long m_oidLastEntered;
    unsigned long m_oidLastLeft;
    unsigned long m_oidLastDisarmedBy;
    CExoArrayList<unsigned long> m_lstTrapTargets;
    int m_nTriggerType;
    int m_nFaction;
    int m_bMineTrigger;
    int m_bTrapDetectable;
    int m_bTrapDisarmable;
    int m_bTrapFlagged;
    int m_bAutoRemoveKey;
    int m_bTrapOneShot;
    unsigned char m_nTrapType;
    int m_nTrapDetectDC;
    int m_nTrapDisarmDC;
    float m_fHighlightHeight;
    unsigned long m_oidCreator;
    float m_fBoundingMinX;
    float m_fBoundingMaxX;
    float m_fBoundingMinY;
    float m_fBoundingMaxY;

    int m_nCustomScriptEventId;
    unsigned char m_nCursor;
    unsigned long m_nLastHeartbeatTime;
    unsigned long m_nLastHeartbeatDay;
    unsigned short m_nLoadScreenId;

    unsigned short m_nObjectIterator;
    CExoLocString m_lsTransitionDestination;
    int m_bSetByPlayerParty;

    static float m_fTrapRadiusRange;

public:

    unsigned long GetLastEntered()
    {
        return m_oidLastEntered;
    }
    void SetLastEntered(unsigned long nLastEntered)
    {
        m_oidLastEntered = nLastEntered;
    }
    unsigned long GetLastLeft()
    {
        return m_oidLastLeft;
    }
    void SetLastLeft(unsigned long nLastLeft)
    {
        m_oidLastLeft = nLastLeft;
    }
    void SetLinkedTo(const CExoString& sLinkedTo)
    {
        m_sLinkedTo = sLinkedTo;
    }
    CExoString GetLinkedTo()
    {
        return m_sLinkedTo;
    }
    void SetKeyName(const CExoString& sKeyName)
    {
        m_sKeyName = sKeyName;
    }
    CExoString* GetKeyName()
    {
        return &m_sKeyName;
    }
    void SetAutoRemoveKey(int nAutoRemoveKey)
    {
        m_bAutoRemoveKey = nAutoRemoveKey;
    }
    int GetAutoRemoveKey()
    {
        return m_bAutoRemoveKey;
    }
    int GetCustomScriptEventId()
    {
        return m_nCustomScriptEventId;
    }
    void SetCustomScriptEventId(int nCustomScriptEventId)
    {
        m_nCustomScriptEventId = nCustomScriptEventId;
    }
};

#endif
