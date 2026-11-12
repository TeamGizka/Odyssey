#ifndef __NWSDOOR_H__
#define __NWSDOOR_H__

#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwsobject.h"

class CResGFF;
class CResStruct;
class CSWCollisionMesh;
class CSWSArea;
class CWalkHitInfo;
class Quaternion;
class Vector;
class CSWDoorSurfaceMesh;

#define DOOR_ACTION_POINT_INITIAL_DISTANCE_SQUARED 1000.0f

class CSWSDoor : public CSWSObject
{
public:

    enum OpenState
    {
        OPEN_STATE_CLOSED = 0,
        OPEN_STATE_OPEN1 = 1,
        OPEN_STATE_OPEN2 = 2,
        OPEN_STATE_DESTROYED = 3
    };

    enum ScriptIndex
    {
        SCRIPT_ON_OPEN,
        SCRIPT_ON_CLOSED,
        SCRIPT_ON_DAMAGED,
        SCRIPT_ON_DEATH,
        SCRIPT_ON_DISARM,
        SCRIPT_ON_HEARTBEAT,
        SCRIPT_ON_LOCK,
        SCRIPT_ON_MELEE_ATTACKED,
        SCRIPT_ON_SPELL_CAST_AT,
        SCRIPT_ON_TRAP_TRIGGERED,
        SCRIPT_ON_UNLOCK,
        SCRIPT_ON_USER_DEFINED,
        SCRIPT_ON_CLICK,
        SCRIPT_ON_DIALOG,
        SCRIPT_ON_FAIL_TO_OPEN,
        NUM_SCRIPTS
    };

    enum DoorLinkType
    {
        DOOR_LINK_DOOR = 1,
        DOOR_LINK_WAYPOINT = 2
    };

    enum { TRAP_TYPE_NONE = -1 };

    CSWSDoor(unsigned long oidSelf);

    void SetLocked(int bLocked) { m_bLocked = bLocked; }

    int GetFactionId() { return m_nFactionId; }
    unsigned char GetOpenLockDC();
    unsigned char GetDetectDC();
    unsigned char GetDisarmDC();

    void SetDetectDC(unsigned char nDC) { m_nTrapDetectDC = nDC; }
    void SetDisarmDC(unsigned char nDC) { m_nDisarmDC = nDC; }
    void SetFlagged(unsigned char bFlagged) { m_bTrapFlagged = bFlagged; }
    unsigned char GetFlagged() { return m_bTrapFlagged; }

    int GetTrapped() { return m_bTrapFlag; }
    int GetDisarmable() { return m_bTrapDisarmable; }
    CExoArrayList<unsigned long>* GetCreatureList() { return &m_lstTrapDetectedBy; }
    CExoLocString& GetTransitionDestination()
    {
        return m_locTransitionDestination;
    }
    virtual ~CSWSDoor();
    virtual CSWSDoor* AsSWSDoor()  // PC 0x00698BD0
    {
        return this;
    }
    virtual void AIUpdate();
    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    // PC 0x00580E10
    virtual CResRef GetDialogResref()
    {
        return m_crConversation;
    }

    void SetDialogResref(CResRef cResRef) { m_crConversation = cResRef; }
    // PC 0x00580E40
    virtual CExoLocString* GetFirstName()
    {
        return &m_locName;
    }
    virtual int GetDead();
    virtual void DoDamage(int nDamage);

    CExoString m_asScripts[NUM_SCRIPTS];
    char m_nAppearanceType;
    unsigned char m_nGenericType;
    CResRef m_crConversation;
    char gap_2B2[2];
    float m_fBearing;
    int m_nFactionId;
    unsigned char m_nFortSave;
    unsigned char m_nReflexSave;
    unsigned char m_nWillSave;
    unsigned char m_nOpenLockDC;
    unsigned char m_nCloseLockDC;
    char m_nSecretDoorDC;
    unsigned char m_nHardness;
    char gap_2C3[1];
    int m_bLocked;
    int m_bLockable;
    unsigned char m_nOpenLockDiff;
    char m_nOpenLockDiffMod;
    char m_aUnmapped30E[0x2];
    int m_nOwnerDemolitionsSkill;
    unsigned char m_bUseTweakColor;
    char m_aUnmapped315[0x3];
    unsigned long m_nTweakColor;
    char m_nOpenState;
    char m_nTargetOpenState;
    char gap_2CE[2];
    CExoString m_sKeyName;
    int m_bKeyRequired;

    CExoArrayList<unsigned long> m_lstTrapDetectedBy;
    int m_bTrapFlag;

    unsigned char m_nDisarmDC;
    unsigned char m_nTrapDetectDC;
    char gap_2EE[2];

    int m_bAutoRemoveKey;

    int m_bTrapFlagged;
    unsigned char m_nTrapType;
    char gap_2F9[3];
    int m_bTrapDisarmable;
    int m_bTrapDetectable;
    int m_bTrapOneShot;
    int m_bAnimatable;
    int m_nLastUpdateSeconds;
    int m_nLastUpdateMilliseconds;
    unsigned long m_oidLastLockedBy;

    unsigned long m_oidLastUnlockedBy;
    unsigned long m_oidLastOpener;
    unsigned long m_oidLastCloser;

    unsigned long m_oidLastTriggeredBy;
    unsigned long m_oidLastDisarmedBy;
    CSWDoorSurfaceMesh* m_apCollisionMesh[3];
    Vector m_vMoveToPosition;
    Vector m_vMoveToNormal;

    Vector m_avOutline[4];
    int m_nOutlineVertices;
    char m_IsLinked;
    char gap_385[2];
    CExoString m_sLinkedTo;
    CExoString m_sLinkedToModule;
    unsigned long m_oidLinkedObject;
    CExoLocString m_locName;
    CExoLocString m_locDescription;
    short m_nLoadScreenId;
    char gap_3AE[2];
    Quaternion m_qOrientation;
    int m_bStatic;

    int m_bPreciseUse;
    CExoLocString m_locTransitionDestination;
    int m_n420;

    int GetMeshOutline(Vector, Vector, Vector&, Vector&, Vector&, Vector&);
    int SaveDoor(CResGFF*, CResStruct*);
    int GetIsLinked();
    int NoNonWalkPolysInDoor(Vector const&, Vector const&, float const&,
                             float const&, float const&, CWalkHitInfo*);
    Vector GetActionPoint(int, unsigned char);
    Vector GetNearestActionPoint(const Vector& vPosition, int bClosed);
    CSWSObject* GetLinkedObject();
    void SetOpenState(unsigned char, int);
    void MoveToNextOpenState();
    void SetOrientation(Quaternion);
    int InDoor(Vector);
    int GetMeshBoundingBox(Vector, Vector, Vector&, Vector&);
    int OpenDoor(unsigned long);
    void PostProcess();
    int LoadDoor(CResGFF*, CResStruct*, int);
    int LoadFromTemplate(CResRef const&);
    void RemoveFromArea();
    void AddToArea(CSWSArea*, float, float, float, int);
    int LoadDoorExternal(CResGFF*, CResStruct*, int);
};

#endif
