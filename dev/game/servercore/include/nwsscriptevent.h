#ifndef __NWSSCRIPTEVENT_H__
#define __NWSSCRIPTEVENT_H__

#include "exobase.h"

class CResGFF;
class CResStruct;



const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_ACQUIRE_ITEM = 19;
const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_LOSE_ITEM = 20;
const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_INVENTORY_DISTURBED = 27;


const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_HEARTBEAT = 0;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION = 1;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_SPELLCASTAT = 2;
const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_LOCKED = 28;
const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_UNLOCKED = 29;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_DAMAGED = 4;
const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_DEATH = 10;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_USER_DEFINED_EVENT = 11;


const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_DIALOGUE = 7;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_OPEN = 22;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_CLOSE = 23;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM = 24;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_USED = 25;


const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED = 26;
const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN = 34;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER = 12;
const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT = 13;


const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_CLIENT_ENTER = 14;


const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_USER_DEFINED = 11;
const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK = 30;


const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED = 31;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICKED = 30;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_SPAWN_BUTTON_DOWN = 33;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_LEVEL_UP = 37;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_EQUIP_ITEM = 38;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_DYING = 32;

const unsigned short CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_REST = 35;

class CScriptEvent
{
public:

    enum PerceptionEvent { PERCEPTION_EVENT_GAINED_HEARING = 0,
                           PERCEPTION_EVENT_LOST_HEARING = 1,
                           PERCEPTION_EVENT_GAINED_SIGHT = 2,
                           PERCEPTION_EVENT_LOST_SIGHT = 3 };


    enum { REST_EVENT_STARTED = 1, REST_EVENT_CANCELLED = 3 };


    enum { EVENT_ACTIVATE_ITEM = 18 };


    enum AreaTransitionInteger { TRANSITION_INTEGER_ID = 0 };
    enum AreaTransitionString
    {
        TRANSITION_STRING_MODULE = 0,
        TRANSITION_STRING_WAYPOINT = 1,
        TRANSITION_STRING_FIRST_MOVIE = 2,
        NUM_TRANSITION_STRINGS = 8
    };

    CScriptEvent();
    ~CScriptEvent();


    unsigned short GetType() { return m_nType; }

    int operator==(const CScriptEvent& sev) const;
    float GetFloat(int nIndex);
    int GetInteger(int nIndex);
    CExoString GetString(int nIndex);
    unsigned long GetObjectID(int nIndex);
    void SetInteger(int nIndex, int nValue);
    void SetFloat(int nIndex, float fValue);
    void SetString(int nIndex, const CExoString& sValue);
    void SetObjectID(int nIndex, unsigned long oidObject);


    int SaveEvent(CResGFF* pRes, CResStruct* pStruct);
    int LoadEvent(CResGFF* pRes, CResStruct* pStruct);
    void CopyScriptEvent(CScriptEvent* pEvent);

    unsigned short m_nType;
    CExoArrayList<int> m_lstIntegers;
    CExoArrayList<float> m_lstFloats;
    CExoArrayList<CExoString> m_lstStrings;
    CExoArrayList<unsigned long> m_lstObjectIDs;
};

#endif
