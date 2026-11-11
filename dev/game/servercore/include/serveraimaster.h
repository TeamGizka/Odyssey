#ifndef __SERVERAIMASTER_H__
#define __SERVERAIMASTER_H__

class CExoLinkedListInternal;
#include "exobase.h"
#include "nwseffectlist.h"
#include "nwsitemproperty.h"
#include "nwscript.h"
#include "math3d.h"

extern CVirtualMachine* g_pVirtualMachine;

class CResGFF;
class CResStruct;
class CSWSObject;
class CServerExoAppInternal;
class CSWSItem;
class CSWItemProperty;
class CSWSCreature;
class CGameEffect;
class CScriptEvent;

extern int g_bLogEventAdd;

class CServerAIBodyBagInfo
{
public:
    CServerAIBodyBagInfo()
    {
    }

    BOOL SaveBodyBag(CResGFF* pRes, CResStruct* pStruct);
    BOOL LoadBodyBag(CResGFF* pRes, CResStruct* pStruct);

    unsigned long m_nBodyBagId;

    Vector m_vPosition;
};

class CServerAIEventNode
{
public:
    BOOL SaveNode(CResGFF* pRes, CResStruct* pStruct);
    BOOL LoadNode(CResGFF* pRes, CResStruct* pStruct);

    unsigned long m_nCalendarDay;
    unsigned long m_nTimeOfDay;
    unsigned long m_oidCaller;
    unsigned long m_oidObject;
    unsigned long m_nEventId;
    void* m_pEventData;
};

class CServerAIList : public CExoArrayList<unsigned long>
{
public:
    CServerAIList()  // PC 0x004B0770  Xbox 0x00160190
    {
        m_nCurrent = 0;
    }

    BOOL RemoveObject(unsigned long oidObject);

    CSWSObject* GetNextObject();
    CSWSObject* GetObjectAtPosition(int nPosition);

    int IndexOf(unsigned long oidObject)
    {
        for (int i = 0; i < num; i++)
        {
            if (element[i] == oidObject)
            {
                return i;
            }
        }

        return -1;
    }

    int m_nCurrent;
};

class CServerAIMaster
{
public:

    enum { LAST_EVENT_PERCEPTION = 2 };

    enum { LAST_EVENT_ACTION = 5 };

    enum { AI_LEVEL_ITEM = 0 };

    enum { AI_LEVEL_AREA_OF_EFFECT = 0 };


    enum { AI_LEVEL_DIALOG = 1 };

    enum { AI_LEVEL_NONPARTY_CREATURE = 2, AI_LEVEL_PARTY_CREATURE = 4 };


    enum { AI_LEVEL_EMPTY_AREA_CREATURE = 0, AI_LEVEL_OCCUPIED_AREA_CREATURE = 1 };




    static const unsigned __int64 AI_UPDATE_TIME_HI_RES;
    static const unsigned long EVENT_TIMED_EVENT;
    static const unsigned long EVENT_ENTERED_TRIGGER;
    static const unsigned long EVENT_LEFT_TRIGGER;
    static const unsigned long EVENT_REMOVE_FROM_AREA;
    static const unsigned long EVENT_APPLY_EFFECT;
    static const unsigned long EVENT_CLOSE_OBJECT;
    static const unsigned long EVENT_OPEN_OBJECT;
    static const unsigned long EVENT_SPELL_IMPACT;
    static const unsigned long EVENT_PLAY_ANIMATION;
    static const unsigned long EVENT_SIGNAL_EVENT;
    static const unsigned long EVENT_DESTROY_OBJECT;
    static const unsigned long EVENT_UNLOCK_OBJECT;
    static const unsigned long EVENT_LOCK_OBJECT;
    static const unsigned long EVENT_REMOVE_EFFECT;
    static const unsigned long EVENT_ON_MELEE_ATTACKED;
    static const unsigned long EVENT_DECREMENT_STACKSIZE;
    static const unsigned long EVENT_SPAWN_BODY_BAG;
    static const unsigned long EVENT_FORCED_ACTION;
    static const unsigned long EVENT_ITEM_ON_HIT_SPELL_IMPACT;
    static const unsigned long EVENT_BROADCAST_AOO;
    static const unsigned long EVENT_BROADCAST_SAFE_PROJECTILE;
    static const unsigned long EVENT_FEEDBACK_MESSAGE;
    static const unsigned long EVENT_ABILITY_EFFECT_APPLIED;
    static const unsigned long EVENT_SUMMON_CREATURE;
    static const unsigned long EVENT_ACQUIRE_ITEM;
    static const unsigned long EVENT_AREA_TRANSITION;
    static const unsigned long EVENT_CONTROLLER_RUMBLE;

    enum { CONTROLLER_RUMBLE_CRITICAL_HIT = 20 };
    void AdjustTargetAndWitnessReputations(unsigned long oidTarget,
                                          unsigned long oidActor, int nAction);
    enum { REPUTATION_ACTION_DEATH = 2 };

    CServerAIMaster();
    ~CServerAIMaster();
    void SetExoAppInternal(CServerExoAppInternal* pExoAppInternal);

    BOOL AddObject(CSWSObject* pObject, int nAILevel);
    BOOL RemoveObject(CSWSObject* pObject);
    BOOL SetAILevel(CSWSObject* pObject, int nAILevel);
    BOOL AddEventAbsoluteTime(unsigned long nCalendarDay, unsigned long nTimeOfDay,
                     unsigned long oidCaller, unsigned long oidObject,
                     unsigned long nEventId, void* pEventData);
    BOOL AddEventDeltaTime(unsigned long nDayDelta, unsigned long nTimeDelta,
                           unsigned long oidCaller, unsigned long oidObject,
                           unsigned long nEventId, void* pEventData);
    BOOL SaveEventQueue(CResGFF* pRes, CResStruct* pStruct);
    BOOL LoadEventQueue(CResGFF* pRes, CResStruct* pStruct);
    void ClearEventQueue();
    void DeleteEventData(unsigned long nEventId, void* pEventData);

    BOOL GetPendingEvent(unsigned long* pnCalendarDay,
                         unsigned long* pnTimeOfDay,
                         unsigned long* poidCaller, unsigned long* poidObject,
                         unsigned long* pnEventId, void** ppEventData);
    BOOL EventPending(unsigned long nCalendarDay,
                      unsigned long nTimeOfDay);
    void UpdateState();
    int OnEffectApplied(CSWSObject* pObject, CGameEffect* pEffect,
                        int bLoadingModule);
    int OnEffectRemoved(CSWSObject* pObject, CGameEffect* pEffect);
    int OnItemPropertyApplied(CSWSItem* pItem, CSWItemProperty* pProperty,
                              CSWSCreature* pCreature, unsigned long nIndex,
                              int bLoading);
    int OnItemPropertyRemoved(CSWSItem* pItem, CSWItemProperty* pProperty,
                              CSWSCreature* pCreature, unsigned long nIndex);
    void AppendToScriptsRun(const CExoString& sScript);

    CServerExoAppInternal* m_pExoAppInternal;
    CServerAIList m_aAILists[5];
    CExoLinkedList<CServerAIEventNode> m_lstEvents;
    CSWSEffectListHandler* m_pEffectHandler;
    CSWSItemPropertyHandler* m_pItemPropertyHandler;
    CExoString m_sLastEvent;
    int m_nLastEvent[6];
    CExoArrayList<unsigned long>* m_pUnused80;
    CExoArrayList<unsigned long>* m_pUnused84;
    CExoArrayList<unsigned long>* m_pUnused88;
    CExoArrayList<unsigned long>* m_pUnused8C;
};

#endif
