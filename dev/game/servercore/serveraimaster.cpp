#include "exobase.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwsobject.h"
#include "nwsscriptevent.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "gameobjectarray.h"
#include "nwcobject.h"
#include "nwscombatround.h"
#include "nwmessage.h"
#include "nwseffect.h"
#include "nwsforcedaction.h"
#include "serverexoappinternal.h"
#include "worldtimer.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "serveraimaster.h"



int g_nServerAIUnused = 0;


float g_AIModuleTime = 0;

int g_bLogEventAdd = 0;


const unsigned __int64 CServerAIMaster::AI_UPDATE_TIME_HI_RES = 10000;
const unsigned long CServerAIMaster::EVENT_TIMED_EVENT = 1;

const unsigned long CServerAIMaster::EVENT_ENTERED_TRIGGER = 2;
const unsigned long CServerAIMaster::EVENT_LEFT_TRIGGER = 3;
const unsigned long CServerAIMaster::EVENT_REMOVE_FROM_AREA = 4;
const unsigned long CServerAIMaster::EVENT_APPLY_EFFECT = 5;
const unsigned long CServerAIMaster::EVENT_CLOSE_OBJECT = 6;
const unsigned long CServerAIMaster::EVENT_OPEN_OBJECT = 7;
const unsigned long CServerAIMaster::EVENT_SPELL_IMPACT = 8;
const unsigned long CServerAIMaster::EVENT_PLAY_ANIMATION = 9;
const unsigned long CServerAIMaster::EVENT_SIGNAL_EVENT = 10;
const unsigned long CServerAIMaster::EVENT_DESTROY_OBJECT = 11;
const unsigned long CServerAIMaster::EVENT_UNLOCK_OBJECT = 12;
const unsigned long CServerAIMaster::EVENT_LOCK_OBJECT = 13;
const unsigned long CServerAIMaster::EVENT_REMOVE_EFFECT = 14;
const unsigned long CServerAIMaster::EVENT_ON_MELEE_ATTACKED = 15;
const unsigned long CServerAIMaster::EVENT_DECREMENT_STACKSIZE = 16;
const unsigned long CServerAIMaster::EVENT_SPAWN_BODY_BAG = 17;
const unsigned long CServerAIMaster::EVENT_FORCED_ACTION = 18;
const unsigned long CServerAIMaster::EVENT_ITEM_ON_HIT_SPELL_IMPACT = 19;
const unsigned long CServerAIMaster::EVENT_BROADCAST_AOO = 20;
const unsigned long CServerAIMaster::EVENT_BROADCAST_SAFE_PROJECTILE = 21;
const unsigned long CServerAIMaster::EVENT_FEEDBACK_MESSAGE = 22;
const unsigned long CServerAIMaster::EVENT_ABILITY_EFFECT_APPLIED = 23;
const unsigned long CServerAIMaster::EVENT_SUMMON_CREATURE = 24;
const unsigned long CServerAIMaster::EVENT_ACQUIRE_ITEM = 25;
const unsigned long CServerAIMaster::EVENT_AREA_TRANSITION = 26;

inline void CreateHandlers(CSWSEffectListHandler*& pEffectHandler, CSWSItemPropertyHandler*& pItemPropertyHandler)
{
    pEffectHandler = new CSWSEffectListHandler;
    pItemPropertyHandler = new CSWSItemPropertyHandler;
}
const unsigned long CServerAIMaster::EVENT_CONTROLLER_RUMBLE = 27;
// PC 0x00650770 GOG 0x00650620 Xbox 0x00171F80
void CServerAIMaster::SetExoAppInternal(CServerExoAppInternal* pExoAppInternal)
{
    m_pExoAppInternal = pExoAppInternal;
}




// Dead code: not in the PC executable
CSWSObject* CServerAIList::GetObjectAtPosition(int nPosition)
{
    if (num == 0)
    {
        return NULL;
    }

    unsigned long oidObject = element[nPosition];
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidObject);

    if (pGameObject != NULL &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        return (CSWSObject*)pGameObject;
    }

    return NULL;
}

// Dead code: not in the PC executable
CSWSObject* CServerAIList::GetNextObject()
{
    if (num == 0)
    {
        return NULL;
    }

    m_nCurrent++;

    if (m_nCurrent >= num)
    {
        m_nCurrent = 0;
    }

    unsigned long oidObject = element[m_nCurrent];
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidObject);

    if (pGameObject != NULL &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        return (CSWSObject*)pGameObject;
    }

    return NULL;
}

// Dead code: not in the PC executable
BOOL CServerAIMaster::GetPendingEvent(unsigned long* pnCalendarDay,
                                      unsigned long* pnTimeOfDay,
                                      unsigned long* poidCaller,
                                      unsigned long* poidObject,
                                      unsigned long* pnEventId,
                                      void** ppEventData)
{
    if (m_lstEvents.IsEmpty())
    {
        return FALSE;
    }

    CServerAIEventNode* pNode = m_lstEvents.RemoveHead();
    *pnCalendarDay = pNode->m_nCalendarDay;
    *pnTimeOfDay = pNode->m_nTimeOfDay;
    *poidCaller = pNode->m_oidCaller;
    *poidObject = pNode->m_oidObject;
    *pnEventId = pNode->m_nEventId;
    *ppEventData = pNode->m_pEventData;
    pNode->m_pEventData = NULL;
    delete pNode;
    return TRUE;
}

// Dead code: not in the PC executable
BOOL CServerAIMaster::EventPending(unsigned long nCalendarDay,
                                   unsigned long nTimeOfDay)
{
    if (m_lstEvents.IsEmpty())
    {
        return FALSE;
    }

    CServerAIEventNode* pNode = m_lstEvents.GetHead();

    if (m_pExoAppInternal->GetWorldTimer()->CompareWorldTimes(
            pNode->m_nCalendarDay, pNode->m_nTimeOfDay, nCalendarDay,
            nTimeOfDay) > 0)
    {
        return FALSE;
    }

    return TRUE;
}

// PC 0x004DC9A0 GOG 0x004DCD90 Xbox 0x001721D0
void LogEventAddGetObject(unsigned long oidObject, CExoString& sName,
                          CExoString& sType)
{
    CSWSObject* pObject = NULL;
    CExoString* pTag;
    CGameObject* pGameObject =
        g_pAppManager->m_pServerExoApp->GetGameObject(oidObject);

    if (pGameObject != NULL)
    {
        pObject = pGameObject->AsSWSObject();
    }

    if (pObject != NULL && (pTag = &pObject->m_sTag) != NULL && *pTag != "")
    {
        sName.Format("%x,%s", oidObject, *pTag);
    }
    else
    {
        sName.Format("%x", oidObject);
    }

    if (pObject != NULL)
    {
        if (pObject->AsSWSAreaOfEffectObject())
        {
            sType = "AOE";
            return;
        }
        else if (pObject->AsSWSCreature())
        {
            sType = "Creature";
            return;
        }
        else if (pObject->AsSWSDoor())
        {
            sType = "Door";
            return;
        }
        else if (pObject->AsSWSPlaceable())
        {
            sType = "Placeable";
            return;
        }
        else if (pObject->AsSWSEncounter())
        {
            sType = "Encounter";
            return;
        }
        else if (pObject->AsSWSTrigger())
        {
            sType = "Trigger";
            return;
        }
        else if (pObject->AsSWSWaypoint())
        {
            sType = "Waypoint";
            return;
        }
        else if (pObject->AsSWSItem())
        {
            sType = "Item";
            return;
        }
        else if (pObject->AsSWSSoundObject())
        {
            sType = "Sound";
            return;
        }
        else if (pObject->AsSWSStore())
        {
            sType = "Store";
            return;
        }
        else
        {
            sType = "Object";
            return;
        }
    }
    else if (pGameObject != NULL)
    {
        if (pGameObject->AsSWSArea())
        {
            sType = "Area";
            return;
        }
        else if (pGameObject->AsSWSModule())
        {
            sType = "Module";
            return;
        }
        else
        {
            sType = "GameObject";
            return;
        }
    }
    else
    {
        sType = "Unknown";
        return;
    }
}


// PC 0x004DCBC0 GOG 0x004DCFB0 Xbox 0x001723F0
static void LogEventAdd(unsigned long oidCaller, unsigned long oidObject,
                        unsigned long nEventId, void* pEventData)
{
    CScriptEvent* pScriptEvent = (CScriptEvent*)pEventData;
    CExoString sCallerName;
    CExoString sCallerType;
    CExoString sObjectName;
    CExoString sObjectType;
    CExoString sEventName;
    CExoString sScriptEventName;
    CExoString sMessage;

    LogEventAddGetObject(oidObject, sObjectName, sObjectType);
    LogEventAddGetObject(oidCaller, sCallerName, sCallerType);

    switch (nEventId)
    {
    case CServerAIMaster::EVENT_TIMED_EVENT:
        sEventName = "EVENT_TIMED_EVENT";
        break;
    case CServerAIMaster::EVENT_ENTERED_TRIGGER:
        sEventName = "EVENT_ENTERED_TRIGGER";
        break;
    case CServerAIMaster::EVENT_LEFT_TRIGGER:
        sEventName = "EVENT_LEFT_TRIGGER";
        break;
    case CServerAIMaster::EVENT_REMOVE_FROM_AREA:
        sEventName = "EVENT_REMOVE_FROM_AREA";
        break;
    case CServerAIMaster::EVENT_APPLY_EFFECT:
        sEventName = "EVENT_APPLY_EFFECT";
        break;
    case CServerAIMaster::EVENT_CLOSE_OBJECT:
        sEventName = "EVENT_CLOSE_OBJECT";
        break;
    case CServerAIMaster::EVENT_OPEN_OBJECT:
        sEventName = "EVENT_OPEN_OBJECT";
        break;
    case CServerAIMaster::EVENT_SPELL_IMPACT:
        sEventName = "EVENT_SPELL_IMPACT";
        break;
    case CServerAIMaster::EVENT_PLAY_ANIMATION:
        sEventName = "EVENT_PLAY_ANIMATION";
        break;
    case CServerAIMaster::EVENT_SIGNAL_EVENT:
        sEventName = "EVENT_SIGNAL_EVENT";
        break;
    case CServerAIMaster::EVENT_DESTROY_OBJECT:
        sEventName = "EVENT_DESTROY_OBJECT";
        break;
    case CServerAIMaster::EVENT_UNLOCK_OBJECT:
        sEventName = "EVENT_UNLOCK_OBJECT";
        break;
    case CServerAIMaster::EVENT_LOCK_OBJECT:
        sEventName = "EVENT_LOCK_OBJECT";
        break;
    case CServerAIMaster::EVENT_REMOVE_EFFECT:
        sEventName = "EVENT_REMOVE_EFFECT";
        break;
    case CServerAIMaster::EVENT_ON_MELEE_ATTACKED:
        sEventName = "EVENT_ON_MELEE_ATTACKED";
        break;
    case CServerAIMaster::EVENT_DECREMENT_STACKSIZE:
        sEventName = "EVENT_DECREMENT_STACKSIZE";
        break;
    case CServerAIMaster::EVENT_SPAWN_BODY_BAG:
        sEventName = "EVENT_SPAWN_BODY_BAG";
        break;
    case CServerAIMaster::EVENT_FORCED_ACTION:
        sEventName = "EVENT_FORCED_ACTION";
        break;
    case CServerAIMaster::EVENT_ITEM_ON_HIT_SPELL_IMPACT:
        sEventName = "EVENT_ITEM_ON_HIT_SPELL_IMPACT";
        break;
    case CServerAIMaster::EVENT_BROADCAST_AOO:
        sEventName = "EVENT_BROADCAST_AOO";
        break;
    case CServerAIMaster::EVENT_BROADCAST_SAFE_PROJECTILE:
        sEventName = "EVENT_BROADCAST_SAFE_PROJECTILE";
        break;
    case CServerAIMaster::EVENT_FEEDBACK_MESSAGE:
        sEventName = "EVENT_FEEDBACK_MESSAGE";
        break;
    case CServerAIMaster::EVENT_ABILITY_EFFECT_APPLIED:
        sEventName = "EVENT_ABILITY_EFFECT_APPLIED";
        break;
    case CServerAIMaster::EVENT_SUMMON_CREATURE:
        sEventName = "EVENT_SUMMON_CREATURE";
        break;
    case CServerAIMaster::EVENT_ACQUIRE_ITEM:
        sEventName = "EVENT_ACQUIRE_ITEM";
        break;
    case CServerAIMaster::EVENT_AREA_TRANSITION:
        sEventName = "EVENT_AREA_TRANSITION";
        break;
    case CServerAIMaster::EVENT_CONTROLLER_RUMBLE:
        sEventName = "EVENT_CONTROLLER_RUMBLE";
        break;
    default:
        sEventName.Format("Event(%d)", nEventId);
        break;
    }

    if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        switch (pScriptEvent->GetType())
        {
        case 0:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_HEARTBEAT";
            break;
        case 1:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_PERCEPTION";
            break;
        case 2:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_SPELLCASTAT";
            break;
        case 4:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_DAMAGED";
            break;
        case 5:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_DISTURBED";
            break;
        case 7:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_DIALOGUE";
            break;
        case 8:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_SPAWN_IN";
            break;
        case 9:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_RESTED";
            break;
        case 10:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_DEATH";
            break;
        case 11:
            sScriptEventName =
                "CSWSSCRIPTEVENT_EVENTTYPE_ON_USER_DEFINED_EVENT";
            break;
        case 12:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER";
            break;
        case 13:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT";
            break;
        case 14:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_ENTER";
            break;
        case 15:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_EXIT";
            break;
        case 16:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_MODULE_START";
            break;
        case 17:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_MODULE_LOAD";
            break;
        case 18:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_ACTIVATE_ITEM";
            break;
        case 19:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_ACQUIRE_ITEM";
            break;
        case 20:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_LOSE_ITEM";
            break;
        case 21:
            sScriptEventName =
                "CSWSSCRIPTEVENT_EVENTTYPE_ON_ENCOUNTER_EXHAUSTED";
            break;
        case 22:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_OPEN";
            break;
        case 23:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_CLOSE";
            break;
        case 24:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM";
            break;
        case 25:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_USED";
            break;
        case 26:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED";
            break;
        case 27:
            sScriptEventName =
                "CSWSSCRIPTEVENT_EVENTTYPE_ON_INVENTORY_DISTURBED";
            break;
        case 28:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_LOCKED";
            break;
        case 29:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_UNLOCKED";
            break;
        case 30:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICKED";
            break;
        case 31:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED";
            break;
        case 32:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_DYING";
            break;
        case 33:
            sScriptEventName =
                "CSWSSCRIPTEVENT_EVENTTYPE_ON_RESPAWN_BUTTON_PRESSED";
            break;
        case 34:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN";
            break;
        case 35:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_REST";
            break;
        case 36:
            sScriptEventName =
                "CSWSSCRIPTEVENT_EVENTTYPE_ON_DESTROYPLAYERCREATURE";
            break;
        case 37:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_LEVEL_UP";
            break;
        case 38:
            sScriptEventName = "CSWSSCRIPTEVENT_EVENTTYPE_ON_EQUIP_ITEM";
            break;
        default:
            sScriptEventName.Format("EventType(%d)", pScriptEvent->GetType());
            break;
        }
    }

    sMessage.Format("DRF Event Added: %s(%s) %s(%s) %s %s\n",
                    sCallerType.CStr(), sCallerName.CStr(), sObjectType.CStr(),
                    sObjectName.CStr(), sEventName.CStr(),
                    sScriptEventName.CStr());
    sMessage.CStr();
}
// PC 0x004DD340 GOG 0x004DCA10 Xbox 0x00171F90
int CServerAIMaster::OnEffectApplied(CSWSObject* pObject, CGameEffect* pEffect,
                                     int bLoadingModule)
{
    return m_pEffectHandler->OnEffectApplied(pObject, pEffect, bLoadingModule);
}
// PC 0x004DD350 GOG 0x004DCA20 Xbox 0x00171FA0
int CServerAIMaster::OnEffectRemoved(CSWSObject* pObject, CGameEffect* pEffect)
{
    return m_pEffectHandler->OnEffectRemoved(pObject, pEffect);
}
// PC 0x004DD360 GOG 0x004DCA30 Xbox 0x00171FB0
int CServerAIMaster::OnItemPropertyApplied(CSWSItem* pItem,
                                           CSWItemProperty* pProperty,
                                           CSWSCreature* pCreature,
                                           unsigned long nIndex, int bLoading)
{
    return m_pItemPropertyHandler->OnItemPropertyApplied(
        pItem, pProperty, pCreature, nIndex, bLoading);
}
// PC 0x004DD370 GOG 0x004DCA40 Xbox 0x00171FC0
int CServerAIMaster::OnItemPropertyRemoved(CSWSItem* pItem,
                                           CSWItemProperty* pProperty,
                                           CSWSCreature* pCreature,
                                           unsigned long nIndex)
{
    return m_pItemPropertyHandler->OnItemPropertyRemoved(pItem, pProperty,
                                                         pCreature, nIndex);
}
// PC 0x004DD570 GOG 0x004DCCB0 Xbox 0x001720F0
BOOL CServerAIList::RemoveObject(unsigned long oidObject)
{
    if (Contains(oidObject) != 0)
    {
        int nIndex = CExoArrayList<unsigned long>::IndexOf(oidObject);

        if (nIndex != -1)
        {
            DelIndex(nIndex);
        }
    }

    return TRUE;
}
// PC 0x004DD5D0 GOG 0x004DCD50 Xbox 0x00172190
BOOL CServerAIMaster::RemoveObject(CSWSObject* pObject)
{
    int nAILevel = pObject->m_nAILevel;

    if (nAILevel <= 4 && nAILevel >= 0)
    {
        BOOL bRemoved = m_aAILists[nAILevel].RemoveObject(pObject->m_nId);

        if (bRemoved == TRUE)
        {
            pObject->m_nAILevel = -1;
        }

        return bRemoved;
    }

    return FALSE;
}

void LogEventAddGetObject(unsigned long oidObject, CExoString& sName,
                          CExoString& sType);
// PC 0x004DD610 GOG 0x004DD730 Xbox 0x00172AF0
BOOL CServerAIMaster::AddEventAbsoluteTime(unsigned long nCalendarDay,
                                  unsigned long nTimeOfDay,
                                  unsigned long oidCaller,
                                  unsigned long oidObject,
                                  unsigned long nEventId, void* pEventData)
{
    if (g_bLogEventAdd != 0)
    {
        LogEventAdd(oidCaller, oidObject, nEventId, pEventData);
    }

    CServerAIEventNode* pNode = new CServerAIEventNode;
    pNode->m_nCalendarDay = nCalendarDay;
    pNode->m_nTimeOfDay = nTimeOfDay;
    pNode->m_oidCaller = oidCaller;
    pNode->m_oidObject = oidObject;
    pNode->m_nEventId = nEventId;
    pNode->m_pEventData = pEventData;

    BOOL bInserted = FALSE;
    CExoLinkedListNode* pPos = m_lstEvents.GetHeadPos();

    while (pPos != NULL && !bInserted)
    {
        CServerAIEventNode* pOther = m_lstEvents.GetAtPos(pPos);
        CWorldTimer* pWorldTimer = m_pExoAppInternal->m_pWorldTimer;

        if (pWorldTimer->CompareWorldTimes(
                pOther->m_nCalendarDay, pOther->m_nTimeOfDay, nCalendarDay,
                nTimeOfDay) == 1)
        {
            m_lstEvents.AddBefore(pNode, pPos);
            bInserted = TRUE;
        }
        else
        {
            m_lstEvents.GetNext(pPos);
        }
    }

    if (!bInserted)
    {
        m_lstEvents.AddTail(pNode);
    }

    return TRUE;
}
// PC 0x004DD700 GOG 0x004DD820 Xbox 0x00172BE0
BOOL CServerAIEventNode::SaveNode(CResGFF* pRes, CResStruct* pStruct)
{
    CResStruct sEventData;

    pRes->WriteFieldDWORD(pStruct, m_nCalendarDay, "Day");
    pRes->WriteFieldDWORD(pStruct, m_nTimeOfDay, "Time");
    pRes->WriteFieldDWORD(pStruct, m_oidObject, "ObjectId");
    pRes->WriteFieldDWORD(pStruct, m_oidCaller, "CallerId");
    pRes->WriteFieldDWORD(pStruct, m_nEventId, "EventId");

    switch (m_nEventId)
    {
    case CServerAIMaster::EVENT_APPLY_EFFECT:
    case CServerAIMaster::EVENT_REMOVE_EFFECT:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x1111);
            ((CGameEffect*)m_pEventData)->SaveGameEffect(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_ON_MELEE_ATTACKED:
    case CServerAIMaster::EVENT_BROADCAST_SAFE_PROJECTILE:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x2222);
            ((CSWSCombatAttackData*)m_pEventData)->SaveData(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_REMOVE_FROM_AREA:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x9999);
            pRes->WriteFieldBYTE(&sEventData, (unsigned char)m_pEventData, "Value");
        }
        return TRUE;
    case CServerAIMaster::EVENT_BROADCAST_AOO:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x3333);
            pRes->WriteFieldDWORD(&sEventData, (unsigned long)m_pEventData,
                                  "Value");
        }
        return TRUE;
    case CServerAIMaster::EVENT_FEEDBACK_MESSAGE:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0xCCCC);
            ((CSWCCMessageData*)m_pEventData)->SaveData(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_PLAY_ANIMATION:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x3333);
            pRes->WriteFieldINT(&sEventData, (int)m_pEventData, "Value");
        }
        return TRUE;
    case CServerAIMaster::EVENT_SIGNAL_EVENT:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x4444);
            ((CScriptEvent*)m_pEventData)->SaveEvent(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_FORCED_ACTION:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x8888);
            ((CSWSForcedAction*)m_pEventData)->SaveForcedAction(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_SPAWN_BODY_BAG:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x5555);
            ((CServerAIBodyBagInfo*)m_pEventData)->SaveBodyBag(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_SPELL_IMPACT:
    case CServerAIMaster::EVENT_ITEM_ON_HIT_SPELL_IMPACT:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData",
                                    CSWSSpellScriptData::GFF_STRUCT_ID);
            ((CSWSSpellScriptData*)m_pEventData)->SaveData(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_TIMED_EVENT:
        pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0x7777);
        g_pVirtualMachine->SaveScriptSituation(m_pEventData, pRes, &sEventData);
        return TRUE;
    case CServerAIMaster::EVENT_SUMMON_CREATURE:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0xDDDD);
            ((CScriptEvent*)m_pEventData)->SaveEvent(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_AREA_TRANSITION:
        if (m_pEventData != NULL)
        {
            pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0xEEEE);
            ((CScriptEvent*)m_pEventData)->SaveEvent(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_CONTROLLER_RUMBLE:
        pRes->AddStructToStruct(&sEventData, pStruct, "EventData", 0xFFFF);
        pRes->WriteFieldINT(&sEventData, (int)m_pEventData, "Value");
        break;
#ifdef _XBOX
    case CServerAIMaster::EVENT_ENTERED_TRIGGER: case CServerAIMaster::EVENT_LEFT_TRIGGER: case CServerAIMaster::EVENT_CLOSE_OBJECT: case CServerAIMaster::EVENT_OPEN_OBJECT:
    case CServerAIMaster::EVENT_DESTROY_OBJECT: case CServerAIMaster::EVENT_UNLOCK_OBJECT: case CServerAIMaster::EVENT_LOCK_OBJECT: case CServerAIMaster::EVENT_DECREMENT_STACKSIZE: return TRUE;
#endif
    }
    return TRUE;
}
// PC 0x004DDAF0 GOG 0x004DDC10 Xbox 0x00173DE0
BOOL CServerAIEventNode::LoadNode(CResGFF* pRes, CResStruct* pStruct)
{
    CResStruct sEventData;
    int bSuccess;

    m_nCalendarDay = pRes->ReadFieldDWORD(pStruct, "Day", bSuccess, 0);
    m_nTimeOfDay = pRes->ReadFieldDWORD(pStruct, "Time", bSuccess, 0);
    m_oidObject = pRes->ReadFieldDWORD(pStruct, "ObjectId", bSuccess, 0);
    m_oidCaller = pRes->ReadFieldDWORD(pStruct, "CallerId", bSuccess, 0);
    m_nEventId = pRes->ReadFieldDWORD(pStruct, "EventId", bSuccess, 0);

    switch (m_nEventId)
    {
    case CServerAIMaster::EVENT_APPLY_EFFECT:
    case CServerAIMaster::EVENT_REMOVE_EFFECT:
    {
        m_pEventData = new CGameEffect(1);
        if (m_pEventData != NULL)
        {
            pRes->GetStructFromStruct(&sEventData, pStruct, "EventData");
            ((CGameEffect*)m_pEventData)->LoadGameEffect(pRes, &sEventData);
        }
    }
        return TRUE;
    case CServerAIMaster::EVENT_ENTERED_TRIGGER:
    case CServerAIMaster::EVENT_LEFT_TRIGGER:
    case CServerAIMaster::EVENT_CLOSE_OBJECT:
    case CServerAIMaster::EVENT_OPEN_OBJECT:
    case CServerAIMaster::EVENT_DESTROY_OBJECT:
    case CServerAIMaster::EVENT_UNLOCK_OBJECT:
    case CServerAIMaster::EVENT_LOCK_OBJECT:
    case CServerAIMaster::EVENT_DECREMENT_STACKSIZE:
        m_pEventData = NULL;
        return TRUE;
    case CServerAIMaster::EVENT_ON_MELEE_ATTACKED:
    case CServerAIMaster::EVENT_BROADCAST_SAFE_PROJECTILE:
        m_pEventData = NULL;
        if (pRes->GetStructFromStruct(&sEventData, pStruct, "EventData") != 0)
        {
            CSWSCombatAttackData* pAttackData = new CSWSCombatAttackData;
            m_pEventData = pAttackData;
            pAttackData->LoadData(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_REMOVE_FROM_AREA:
        m_pEventData = NULL;
        if (pRes->GetStructFromStruct(&sEventData, pStruct, "EventData") != 0)
        {
            m_pEventData =
                (void*)pRes->ReadFieldBYTE(&sEventData, "Value", bSuccess, 0);
        }
        return TRUE;
    case CServerAIMaster::EVENT_BROADCAST_AOO:
        m_pEventData = NULL;
        if (pRes->GetStructFromStruct(&sEventData, pStruct, "EventData") != 0)
        {
            m_pEventData =
                (void*)pRes->ReadFieldDWORD(&sEventData, "Value", bSuccess, 0);
        }
        return TRUE;
    case CServerAIMaster::EVENT_FEEDBACK_MESSAGE:
        m_pEventData = NULL;
        if (pRes->GetStructFromStruct(&sEventData, pStruct, "EventData") != 0)
        {
            CSWCCMessageData* pMessageData = new CSWCCMessageData;
            m_pEventData = pMessageData;
            pMessageData->LoadData(pRes, &sEventData);
        }
        return TRUE;
    case CServerAIMaster::EVENT_PLAY_ANIMATION:
        m_pEventData = NULL;
        if (pRes->GetStructFromStruct(&sEventData, pStruct, "EventData") != 0)
        {
            m_pEventData =
                (void*)pRes->ReadFieldINT(&sEventData, "Value", bSuccess, 0);
        }
        return TRUE;
    case CServerAIMaster::EVENT_SIGNAL_EVENT:
    {
        m_pEventData = new CScriptEvent;
        if (m_pEventData != NULL)
        {
            pRes->GetStructFromStruct(&sEventData, pStruct, "EventData");
            ((CScriptEvent*)m_pEventData)->LoadEvent(pRes, &sEventData);
        }
    }
        return TRUE;
    case CServerAIMaster::EVENT_FORCED_ACTION:
        return FALSE;
    case CServerAIMaster::EVENT_SPAWN_BODY_BAG:
    {
        m_pEventData = new CServerAIBodyBagInfo;
        if (m_pEventData != NULL)
        {
            pRes->GetStructFromStruct(&sEventData, pStruct, "EventData");
            ((CServerAIBodyBagInfo*)m_pEventData)
                ->LoadBodyBag(pRes, &sEventData);
        }
    }
        return TRUE;
    case CServerAIMaster::EVENT_SPELL_IMPACT:
    {
        m_pEventData = new CSWSSpellScriptData;
        if (m_pEventData != NULL)
        {
            pRes->GetStructFromStruct(&sEventData, pStruct, "EventData");
            ((CSWSSpellScriptData*)m_pEventData)->LoadData(pRes, &sEventData);
        }
    }
        return TRUE;
    case CServerAIMaster::EVENT_TIMED_EVENT:
        pRes->GetStructFromStruct(&sEventData, pStruct, "EventData");
        g_pVirtualMachine->LoadScriptSituation(&m_pEventData, pRes,
                                               &sEventData);
        return TRUE;
    case CServerAIMaster::EVENT_SUMMON_CREATURE:
    {
        m_pEventData = new CScriptEvent;
        if (m_pEventData != NULL)
        {
            pRes->GetStructFromStruct(&sEventData, pStruct, "EventData");
            ((CScriptEvent*)m_pEventData)->LoadEvent(pRes, &sEventData);
        }
    }
        return TRUE;
    case CServerAIMaster::EVENT_AREA_TRANSITION:
    {
        m_pEventData = new CScriptEvent;
        if (m_pEventData != NULL)
        {
            pRes->GetStructFromStruct(&sEventData, pStruct, "EventData");
            ((CScriptEvent*)m_pEventData)->LoadEvent(pRes, &sEventData);
        }
    }
        return TRUE;
    case CServerAIMaster::EVENT_CONTROLLER_RUMBLE:
        m_pEventData = NULL;
        if (pRes->GetStructFromStruct(&sEventData, pStruct, "EventData") != 0)
        {
            m_pEventData =
                (void*)pRes->ReadFieldINT(&sEventData, "Value", bSuccess, 0);
        }
        return TRUE;
    }

    return TRUE;
}
// PC 0x004DD3A0 GOG 0x004DCA80 Xbox 0x00171FD0
BOOL CServerAIBodyBagInfo::SaveBodyBag(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldDWORD(pStruct, m_nBodyBagId, "BodyBagId");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fX, "PositionX");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fY, "PositionY");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fZ, "PositionZ");
    return TRUE;
}
// PC 0x004DD400 GOG 0x004DCAE0 Xbox 0x00172030
BOOL CServerAIBodyBagInfo::LoadBodyBag(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;

    m_nBodyBagId = pRes->ReadFieldDWORD(pStruct, "BodyBagId", bSuccess, 0);
    m_vPosition.m_fX = pRes->ReadFieldFLOAT(pStruct, "PositionX", bSuccess, 0.0f);
    m_vPosition.m_fY = pRes->ReadFieldFLOAT(pStruct, "PositionY", bSuccess, 0.0f);
    m_vPosition.m_fZ = pRes->ReadFieldFLOAT(pStruct, "PositionZ", bSuccess, 0.0f);
    return TRUE;
}
template <> inline CExoLinkedList<CServerAIEventNode>::~CExoLinkedList()
{
    delete m_pInternal;
}
// PC 0x004DDFD0 GOG 0x004DE0F0
CServerAIList::CServerAIList()
{
    m_nCurrent = 0;
}

// Dead code: not in the PC executable
BOOL CServerAIList::AddObject(unsigned long oidObject)  // Xbox 0x002C2710
{
    AddUnique(oidObject);
    return TRUE;
}
// PC 0x004DDFE0 GOG 0x004DE100 Xbox 0x00173000
CServerAIMaster::CServerAIMaster()
{
    m_pEffectHandler = new CSWSEffectListHandler;
    m_pEffectHandler->InitializeEffects();
    m_pItemPropertyHandler = new CSWSItemPropertyHandler;
    m_pItemPropertyHandler->InitializeItemProperties();
    m_pAttackRepAdjustments = NULL;
    m_pTheftRepAdjustments = NULL;
    m_pKillRepAdjustments = NULL;
    m_pHelpRepAdjustments = NULL;
}
// PC 0x004DE0B0 GOG 0x004DE1D0 Xbox 0x001730E0
BOOL CServerAIMaster::AddObject(CSWSObject* pObject, int nAILevel)
{
    if (nAILevel > 4 || nAILevel < 0 || pObject == NULL)
    {
        return FALSE;
    }

    if (pObject->m_nAILevel != -1)
    {
        RemoveObject(pObject);
    }

    m_aAILists[nAILevel].AddUnique(pObject->m_nId);
    pObject->m_nAILevel = nAILevel;
    return TRUE;
}
// PC 0x004DE100 GOG 0x004DE220 Xbox 0x00173130
BOOL CServerAIMaster::SetAILevel(CSWSObject* pObject, int nAILevel)
{
    if (pObject->m_nAILevel == nAILevel)
    {
        return TRUE;
    }

    if (nAILevel <= 4 && nAILevel >= 0)
    {
        return AddObject(pObject, nAILevel);
    }

    return FALSE;
}
// PC 0x004DE130 GOG 0x004DE250 Xbox 0x00173160
BOOL CServerAIMaster::AddEventDeltaTime(
    unsigned long nDayDelta, unsigned long nTimeDelta, unsigned long oidCaller,
    unsigned long oidObject, unsigned long nEventId, void* pEventData)
{
    unsigned long nNowTime;
    unsigned long nNowDay;
    unsigned long nEventTime;
    unsigned long nEventDay;

    g_pAppManager->m_pServerExoApp->GetActivePauseState();
    m_pExoAppInternal->m_pWorldTimer->GetWorldTime(&nNowDay, &nNowTime);

    CWorldTimer* pWorldTimer = m_pExoAppInternal->m_pWorldTimer;
    if (pWorldTimer->AddWorldTimes(nNowDay, nNowTime, nDayDelta, nTimeDelta,
                                   &nEventDay, &nEventTime) == 0)
    {
        return AddEventAbsoluteTime(nEventDay, nEventTime, oidCaller, oidObject,
                           nEventId, pEventData);
    }

    delete pEventData;
    return FALSE;
}
// PC 0x004DE1D0 GOG 0x004DE2F0 Xbox 0x00173200
BOOL CServerAIMaster::SaveEventQueue(CResGFF* pRes, CResStruct* pStruct)
{
    CResList lstEvents;
    CResStruct sEvent;

    pRes->AddList(&lstEvents, pStruct, "EventQueue");

    if ((unsigned long)m_lstEvents.Count() > 0)
    {
        CExoLinkedListNode* pPos = m_lstEvents.GetHeadPos();

        while (pPos != NULL)
        {
            CServerAIEventNode* pNode = m_lstEvents.GetAtPos(pPos);

            pRes->AddListElement(&sEvent, &lstEvents, 0xABCD);
            pNode->SaveNode(pRes, &sEvent);
            m_lstEvents.GetNext(pPos);
        }
    }

    return TRUE;
}
// PC 0x004DE260 GOG 0x004DE380 Xbox 0x00174A00
BOOL CServerAIMaster::LoadEventQueue(CResGFF* pRes, CResStruct* pStruct)
{
    CResList lstEvents;
    CResStruct sEvent;

    if (pRes->GetList(&lstEvents, pStruct, "EventQueue") != 0)
    {
        for (unsigned int i = 0; i < pRes->GetListCount(&lstEvents); i++)
        {
#ifdef _XBOX
            CServerAIEventNode* pNode = new CServerAIEventNode();
#else
            CServerAIEventNode* pNode = new CServerAIEventNode;
#endif

            pRes->GetListElement(&sEvent, &lstEvents, i);

            if (pNode->LoadNode(pRes, &sEvent) != 0)
            {
                m_lstEvents.AddTail(pNode);
            }
            else
            {
                delete pNode;
            }
        }
    }

    return TRUE;
}
static inline C2DA* GetReputationAdjustmentTable()
{
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pRepAdjustments);
    return p2DArrays->m_pRepAdjustments;
}
// PC 0x004DE310 GOG 0x004DE430 Xbox 0x00173290
void CServerAIMaster::LoadReputationAdjustments()
{
    C2DA* pRepAdjustments = GetReputationAdjustmentTable();
    if (pRepAdjustments != NULL)
    {
        for (int nRow = 0; nRow < 4; nRow++)
        {
            int nPersonalRep;
            int nFactionRep;
            int nWitnessFriendA;
            int nWitnessFriendB;
            int nWitnessFriendC;
            int nWitnessNeutralA;
            int nWitnessNeutralB;
            int nWitnessNeutralC;
            int nWitnessEnemyA;
            int nWitnessEnemyB;
            int nWitnessEnemyC;
            pRepAdjustments->GetINTEntry(nRow, "PERSONALREP", &nPersonalRep);
            pRepAdjustments->GetINTEntry(nRow, "FACTIONREP", &nFactionRep);
            pRepAdjustments->GetINTEntry(nRow, "WITFRIA", &nWitnessFriendA);
            pRepAdjustments->GetINTEntry(nRow, "WITFRIB", &nWitnessFriendB);
            pRepAdjustments->GetINTEntry(nRow, "WITFRIC", &nWitnessFriendC);
            pRepAdjustments->GetINTEntry(nRow, "WITNEUA", &nWitnessNeutralA);
            pRepAdjustments->GetINTEntry(nRow, "WITNEUB", &nWitnessNeutralB);
            pRepAdjustments->GetINTEntry(nRow, "WITNEUC", &nWitnessNeutralC);
            pRepAdjustments->GetINTEntry(nRow, "WITENEA", &nWitnessEnemyA);
            pRepAdjustments->GetINTEntry(nRow, "WITENEB", &nWitnessEnemyB);
            pRepAdjustments->GetINTEntry(nRow, "WITENEC", &nWitnessEnemyC);
            switch (nRow)
            {
            case 0:
                m_pAttackRepAdjustments = new CExoArrayList<unsigned long>;
                m_pAttackRepAdjustments->Add(nPersonalRep);
                m_pAttackRepAdjustments->Add(nFactionRep);
                m_pAttackRepAdjustments->Add(nWitnessFriendA);
                m_pAttackRepAdjustments->Add(nWitnessFriendB);
                m_pAttackRepAdjustments->Add(nWitnessFriendC);
                m_pAttackRepAdjustments->Add(nWitnessNeutralA);
                m_pAttackRepAdjustments->Add(nWitnessNeutralB);
                m_pAttackRepAdjustments->Add(nWitnessNeutralC);
                m_pAttackRepAdjustments->Add(nWitnessEnemyA);
                m_pAttackRepAdjustments->Add(nWitnessEnemyB);
                m_pAttackRepAdjustments->Add(nWitnessEnemyC);
                break;
            case 1:
                m_pTheftRepAdjustments = new CExoArrayList<unsigned long>;
                m_pTheftRepAdjustments->Add(nPersonalRep);
                m_pTheftRepAdjustments->Add(nFactionRep);
                m_pTheftRepAdjustments->Add(nWitnessFriendA);
                m_pTheftRepAdjustments->Add(nWitnessFriendB);
                m_pTheftRepAdjustments->Add(nWitnessFriendC);
                m_pTheftRepAdjustments->Add(nWitnessNeutralA);
                m_pTheftRepAdjustments->Add(nWitnessNeutralB);
                m_pTheftRepAdjustments->Add(nWitnessNeutralC);
                m_pTheftRepAdjustments->Add(nWitnessEnemyA);
                m_pTheftRepAdjustments->Add(nWitnessEnemyB);
                m_pTheftRepAdjustments->Add(nWitnessEnemyC);
                break;
            case 2:
                m_pKillRepAdjustments = new CExoArrayList<unsigned long>;
                m_pKillRepAdjustments->Add(nPersonalRep);
                m_pKillRepAdjustments->Add(nFactionRep);
                m_pKillRepAdjustments->Add(nWitnessFriendA);
                m_pKillRepAdjustments->Add(nWitnessFriendB);
                m_pKillRepAdjustments->Add(nWitnessFriendC);
                m_pKillRepAdjustments->Add(nWitnessNeutralA);
                m_pKillRepAdjustments->Add(nWitnessNeutralB);
                m_pKillRepAdjustments->Add(nWitnessNeutralC);
                m_pKillRepAdjustments->Add(nWitnessEnemyA);
                m_pKillRepAdjustments->Add(nWitnessEnemyB);
                m_pKillRepAdjustments->Add(nWitnessEnemyC);
                break;
            case 3:
                m_pHelpRepAdjustments = new CExoArrayList<unsigned long>;
                m_pHelpRepAdjustments->Add(nPersonalRep);
                m_pHelpRepAdjustments->Add(nFactionRep);
                m_pHelpRepAdjustments->Add(nWitnessFriendA);
                m_pHelpRepAdjustments->Add(nWitnessFriendB);
                m_pHelpRepAdjustments->Add(nWitnessFriendC);
                m_pHelpRepAdjustments->Add(nWitnessNeutralA);
                m_pHelpRepAdjustments->Add(nWitnessNeutralB);
                m_pHelpRepAdjustments->Add(nWitnessNeutralC);
                m_pHelpRepAdjustments->Add(nWitnessEnemyA);
                m_pHelpRepAdjustments->Add(nWitnessEnemyB);
                m_pHelpRepAdjustments->Add(nWitnessEnemyC);
                break;
            }
        }
    }
}
#ifndef _XBOX
void CServerAIMaster::DeleteEventData(unsigned long nEventId, void* pEventData)  // PC 0x004DEF60 GOG 0x004DF080
{
    if (pEventData != NULL)
    {
        switch (nEventId)
        {
        case EVENT_BROADCAST_SAFE_PROJECTILE:
            delete (CSWSCombatAttackData*)pEventData;
            return;
        case EVENT_FEEDBACK_MESSAGE:
            delete (CSWCCMessageData*)pEventData;
            return;
        case EVENT_SPELL_IMPACT:
        case EVENT_ITEM_ON_HIT_SPELL_IMPACT:
            delete (CSWSSpellScriptData*)pEventData;
            return;
        case EVENT_TIMED_EVENT:
            g_pVirtualMachine->DeleteScriptSituation(pEventData);
            return;
        case EVENT_SIGNAL_EVENT:
        case EVENT_SUMMON_CREATURE:
        case EVENT_AREA_TRANSITION:
            delete (CScriptEvent*)pEventData;
            return;
        }
    }
}
#endif

#ifdef _XBOX


void CServerAIMaster::DeleteEventData(unsigned long nEventId, void* pEventData)  // Xbox 0x001742B0
{
    if (pEventData != NULL)
    {
        switch (nEventId)
        {
        case EVENT_BROADCAST_SAFE_PROJECTILE:
            delete (CSWSCombatAttackData*)pEventData;
            return;
        case EVENT_FEEDBACK_MESSAGE:
            delete (CSWCCMessageData*)pEventData;
            return;
        case EVENT_SPELL_IMPACT:
        case EVENT_ITEM_ON_HIT_SPELL_IMPACT:
            delete (CSWSSpellScriptData*)pEventData;
            return;
        case EVENT_TIMED_EVENT:
            g_pVirtualMachine->DeleteScriptSituation(pEventData);
            return;
        case EVENT_SIGNAL_EVENT:
        case EVENT_SUMMON_CREATURE:
        case EVENT_AREA_TRANSITION:
            delete (CScriptEvent*)pEventData;
            return;
        case EVENT_ENTERED_TRIGGER:
        case EVENT_LEFT_TRIGGER:
        case EVENT_REMOVE_FROM_AREA:
        case EVENT_APPLY_EFFECT:
        case EVENT_CLOSE_OBJECT:
        case EVENT_OPEN_OBJECT:
        case EVENT_PLAY_ANIMATION:
        case EVENT_DESTROY_OBJECT:
        case EVENT_UNLOCK_OBJECT:
        case EVENT_LOCK_OBJECT:
        case EVENT_REMOVE_EFFECT:
        case EVENT_ON_MELEE_ATTACKED:
        case EVENT_DECREMENT_STACKSIZE:
        case EVENT_SPAWN_BODY_BAG:
        case EVENT_FORCED_ACTION:
        case EVENT_BROADCAST_AOO:
        case EVENT_ABILITY_EFFECT_APPLIED:
        case EVENT_ACQUIRE_ITEM:
        case EVENT_CONTROLLER_RUMBLE:
            return;
        }
    }
}
#endif
// PC 0x004DF020 GOG 0x004DF140 Xbox 0x00174370
void CServerAIMaster::UpdateState()
{
    unsigned long nCurrentTime;
    unsigned long nCurrentDay;
    CGameObject* pTarget;
    CSWSObject* pFirst;
    unsigned __int64 nRemaining = 10000;
    int nLevel = 4;

    m_pExoAppInternal->GetWorldTimer()->GetWorldTime(&nCurrentDay,
                                                     &nCurrentTime);

    while (nLevel >= 0)
    {
        unsigned __int64 nSlice;

        if (nLevel > 0)
        {
            nSlice = 60 * nRemaining / 100;
        }
        else
        {
            nSlice = nRemaining;
        }

        unsigned __int64 nStart =
            g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        unsigned __int64 nNow = nStart;
        int bFirst = 1;
        int bIdle = 0;

        do
        {
            for (int i = 0; i < m_aAILists[nLevel].num; i++)
            {
                CSWSObject* pObject = m_aAILists[nLevel].GetObjectAtPosition(i);

                if (pObject == NULL || pObject->m_nClientState == 0 ||
                    pObject->AsSWSCreature() == NULL)
                {
                    continue;
                }

                CSWSCreature* pCreature = pObject->AsSWSCreature();
                CSWSObjectActionNode* pAction = pCreature->m_lstActions.GetHead();

                if ((pAction != NULL &&
                     (pAction->m_nActionType ==
                          CSWSObject::ACTION_MOVE_TO_POINT ||
                      pAction->m_nActionType ==
                          CSWSObject::ACTION_FOLLOW_LEADER ||
                      pAction->m_nActionType ==
                          CSWSObject::ACTION_FOLLOW_OWNER)) ||
                    pObject->AsSWSCreature()->m_nAnimationState ==
                        CSWSCreature::ANIMATION_STATE_FORCE_PUSH ||
                    pObject->AsSWSCreature()->m_nAnimationState ==
                        CSWSCreature::ANIMATION_STATE_FORCE_JUMP ||
                    pObject->AsSWSCreature()->m_nAnimationState ==
                        CSWSCreature::ANIMATION_STATE_FORCE_JUMP_TARGET)
                {
                    if (pObject->AsSWSCreature()->WalkUpdateLocation_QuickWalk(
                            0) == CSWSObject::ACTION_COMPLETE)
                    {
                        pObject->AsSWSCreature()->m_nAnimationState = 1;
                    }
                }
            }

            while (EventPending(nCurrentDay, nCurrentTime) == TRUE)
            {
                unsigned long nCalendarDay;
                unsigned long nTimeOfDay;
                unsigned long oidCaller;
                unsigned long oidObject;
                unsigned long nEventId;
                void* pEventData;

                if (GetPendingEvent(&nCalendarDay, &nTimeOfDay, &oidCaller,
                                    &oidObject, &nEventId,
                                    &pEventData) == TRUE)
                {
                    if (m_pExoAppInternal->GetObjectArray()->GetGameObject(
                            oidObject, &pTarget) == CGameObjectArray::SUCCESS)
                    {
                        if (pTarget->m_nObjectType >
                            CGameObject::OBJECT_TYPE_AREA)
                        {
                            ((CSWSObject*)pTarget)->EventHandler(
                                nEventId, oidCaller, pEventData,
                                nCalendarDay, nTimeOfDay);
                        }
                        else if (pTarget->m_nObjectType ==
                                 CGameObject::OBJECT_TYPE_AREA)
                        {
                            pTarget->AsSWSArea()->EventHandler(
                                nEventId, oidCaller, pEventData,
                                nCalendarDay, nTimeOfDay);
                        }
                        else if (pTarget->m_nObjectType ==
                                 CGameObject::OBJECT_TYPE_MODULE)
                        {
                            pTarget->AsSWSModule()->EventHandler(
                                nEventId, oidCaller, pEventData,
                                nCalendarDay, nTimeOfDay);
                        }
                        else
                        {
                            DeleteEventData(nEventId, pEventData);
                        }
                    }
                    else
                    {
                        DeleteEventData(nEventId, pEventData);
                    }
                }
            }

            if (nNow - nStart <= nSlice)
            {
                CSWSObject* pObject = m_aAILists[nLevel].GetNextObject();

                if (bFirst == 1)
                {
                    pFirst = pObject;
                }

                nNow =
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();

                m_sLastEvent = "";
                m_nLastEvent[0] = 0;
                m_nLastEvent[1] = 0;
                m_nLastEvent[2] = 0;
                m_nLastEvent[3] = 0;
                m_nLastEvent[4] = 0;
                m_nLastEvent[5] = -1;

                if (pObject != NULL && (pObject != pFirst || bFirst == 1))
                {
                    pObject->AIUpdate();
                }
                else
                {
                    bIdle = 1;
                }

                bFirst = 0;

                unsigned long nElapsed =
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer() -
                    nNow;

                if (nElapsed >= 75000 && pObject != NULL)
                {
                    pObject->ReportOverflow(
                        CSWSCreature::FEEDBACK_AI_UPDATE_TIME_OVERFLOW,
                        nElapsed, 75000, 0);
                }

                nNow = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
            }
        } while (EventPending(nCurrentDay, nCurrentTime) == FALSE &&
                 bIdle == 0 && nNow - nStart <= nSlice);

        if (nNow - nStart < nSlice)
        {
            nRemaining = nRemaining - (nNow - nStart);
        }
        else
        {
            nRemaining = nRemaining - nSlice;
        }

        nLevel--;
    }

    for (int nList = 4; nList >= 0; nList--)
    {
        for (int i = 0; i < m_aAILists[nList].num; i++)
        {
            CSWSObject* pObject = m_aAILists[nList].GetObjectAtPosition(i);

            if (pObject != NULL && pObject->m_nClientState != 0)
            {
                CSWCObject* pClient = pObject->GetClientObject();

                if (pClient != NULL && pClient->AsSWCObject() != NULL)
                {
                    if (pObject->IsDirtyFlagSet(0) != 0)
                    {
                        pClient->AsSWCObject()
                            ->ClientServerSyncHandler_Animation(
                                pObject->m_nAnimation,
                                pObject->GetAnimationSpeed());
                        pObject->ClearDirtyFlag(CSWSObject::DIRTY_ANIMATION_INDEX);
                    }

                    if (pObject->IsDirtyFlagSet(1) != 0)
                    {
                        pClient->AsSWCObject()
                            ->ClientServerSyncHandler_Position(
                                pObject->m_vPosition, pObject->m_nAnimation,
                                pObject->GetMovementRate());
                        pObject->ClearDirtyFlag(1);
                    }

                    if (pObject->IsDirtyFlagSet(2) != 0)
                    {
                        pClient->AsSWCObject()
                            ->ClientServerSyncHandler_Orientation(
                                pObject->m_vOrientation);
                        pObject->ClearDirtyFlag(CSWSObject::DIRTY_ORIENTATION_INDEX);
                    }
                }
            }
        }
    }

    if (m_pExoAppInternal != NULL)
    {
        if (m_pExoAppInternal->GetModule() != NULL)
        {
            m_pExoAppInternal->GetModule()->DoUpdate();
        }
    }
}
// PC 0x004DF670 GOG 0x004DF790 Xbox 0x001749C0
void CServerAIMaster::ClearEventQueue()
{
    while (!m_lstEvents.IsEmpty())
    {
        CServerAIEventNode* pNode =
            (CServerAIEventNode*)m_lstEvents.RemoveHead();

        DeleteEventData(pNode->m_nEventId, pNode->m_pEventData);
        delete pNode;
    }
}
// PC 0x004DF6B0 GOG 0x004DF7D0 Xbox 0x00174AE0
CServerAIMaster::~CServerAIMaster()
{
    ClearEventQueue();

    if (m_pEffectHandler != NULL)
    {
        delete m_pEffectHandler;
        m_pEffectHandler = NULL;
    }

    if (m_pItemPropertyHandler != NULL)
    {
        delete m_pItemPropertyHandler;
        m_pItemPropertyHandler = NULL;
    }

    if (m_pAttackRepAdjustments != NULL)
    {
        delete m_pAttackRepAdjustments;
        m_pAttackRepAdjustments = NULL;
    }

    if (m_pTheftRepAdjustments != NULL)
    {
        delete m_pTheftRepAdjustments;
        m_pTheftRepAdjustments = NULL;
    }

    if (m_pKillRepAdjustments != NULL)
    {
        delete m_pKillRepAdjustments;
        m_pKillRepAdjustments = NULL;
    }

    if (m_pHelpRepAdjustments != NULL)
    {
        delete m_pHelpRepAdjustments;
        m_pHelpRepAdjustments = NULL;
    }
}


// PC 0x006009E0 GOG 0x006009C0 Xbox 0x0021F550
void CServerAIMaster::AdjustTargetAndWitnessReputations(
    unsigned long oidTarget, unsigned long oidActor, int nAction)
{
}

// Dead code: not in the PC executable
int IsCreatureDeadTemp(CSWSCreature* pCreature)
{
    return pCreature->GetDeadTemp();
}
