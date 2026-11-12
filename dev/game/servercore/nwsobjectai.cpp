#include "worldtimer.h"

#include "serveraimaster.h"
#include "nwsobject.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwmessage.h"
#include "nwscript.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "exotlktable.h"
#include "nwsplayer.h"
#include "exobase.h"
#include "gameobjectarray.h"
#include "nwsitem.h"
#include "nwsarea.h"
#include "serverexoappinternal.h"
#include "factionmanager.h"
#include "nwsspellimpact.h"
#include "nwsplaceable.h"
#include "guiingame.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "nwcobject.h"
#include "nwcplaceable.h"
#include "nwcanimbase.h"
#include "nwrules.h"
#include "swparty.h"
#include "nwccreature.h"
#include "nwcitem.h"
#include "nwsscriptevent.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "exoinput.h"

#include "clientexoappinternal.h"
#include "nwscombatround.h"
#include "swmain.h"
#include "nwsmodule.h"
#include "pathfindinfo.h"
#include "nwsdoor.h"

#include "nwskill.h"

static inline CServerExoApp* GetServerExoApp();

int RenderAi = 0;

int OverrideReputation = CSWSObject::REPUTATION_NO_OVERRIDE;
// PC 0x005DACE0
void CSWSCreature::SetPreDialogPosition(Vector vPosition)
{
    m_v1208 = vPosition;
}
// PC 0x005DAD00
void CSWSCreature::SetPreDialogOrientation(Vector vOrientation)
{
    m_v1214 = vOrientation;
}
// PC 0x005DAD20
unsigned long CSWSObject::GetAcceptableAction(unsigned long nActionType)
{
    int nResult;

    switch (nActionType)
    {
    case 1:
    case 7:
    case 9:
    case 12:
    case 15:
    case 20:
    case 21:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 46:
    case 50:
    case 54:
    case 55:
    case 56:
    case 61:
    case 63:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    {
        nResult = 1;
        break;
    }
    default:
    {
        nResult = 0;
        break;
    }
    }

    return nResult;
}
// PC 0x005DADA0
unsigned short CSWSObject::GetAQActionIDByID(unsigned short nActionType,
                                  int bSpecialAction)
{

    if (bSpecialAction != 0)
    {
        int nAction = nActionType;
        if (nAction != ACTION_UNEQUIP_ITEM)
            return nAction != ACTION_WAIT ? ::ACTION_INVALID : AQ_ACTION_SPECIAL_WAIT;
        return AQ_ACTION_UNEQUIP_ITEM;
    }

    switch (nActionType)
    {
    case ACTION_MOVE_TO_POINT:
        return ::ACTION_MOVETOPOINT;
    case ACTION_PICK_UP_ITEM:
        return ::ACTION_PICKUPITEM;
    case ACTION_DROP_ITEM:
        return ::ACTION_DROPITEM;
    case ACTION_PHYSICAL_ATTACKS:
        return ::ACTION_ATTACKOBJECT;
    case ACTION_COMBAT:
        return AQ_ACTION_COMBAT;
    case ACTION_CAST_SPELL:
        return ::ACTION_CASTSPELL;
    case ACTION_OPEN_DOOR:
        return ::ACTION_OPENDOOR;
    case ACTION_CLOSE_DOOR:
        return ::ACTION_CLOSEDOOR;
    case ACTION_DIALOG_OBJECT:
        return ::ACTION_DIALOGOBJECT;
    case ACTION_DISARM_MINE:
        return ::ACTION_DISABLETRAP;
    case ACTION_RECOVER_MINE:
        return ::ACTION_RECOVERTRAP;
    case ACTION_FLAG_MINE:
        return ::ACTION_FLAGTRAP;
    case ACTION_EXAMINE_MINE:
        return ::ACTION_EXAMINETRAP;
    case ACTION_SET_MINE:
        return ::ACTION_SETTRAP;
    case ACTION_UNLOCK_OBJECT:
        return ::ACTION_OPENLOCK;
    case ACTION_LOCK_OBJECT:
        return ::ACTION_LOCK;
    case ACTION_USE_OBJECT:
        return ::ACTION_USEOBJECT;
    case ACTION_ANIMAL_EMPATHY:
        return ::ACTION_ANIMALEMPATHY;
    case ACTION_REST:
        return ::ACTION_REST;
    case ACTION_TAUNT:
        return ::ACTION_TAUNT;
    case ACTION_ITEM_CAST_SPELL:
        return ::ACTION_ITEMCASTSPELL;
    case ACTION_COUNTER_SPELL:
        return ::ACTION_COUNTERSPELL;
    case ACTION_HEAL:
        return ::ACTION_HEAL;
    case ACTION_PICK_POCKET:
        return ::ACTION_PICKPOCKET;
    case ACTION_FORCE_FOLLOW_OBJECT:
        return ::ACTION_FOLLOW;
    case ACTION_FOLLOW_LEADER:
        return ::ACTION_FOLLOWLEADER;
    case ACTION_FOLLOW_OWNER:
        return ::ACTION_FOLLOWOWNER;
    case ACTION_WAIT:
        return ::ACTION_WAIT;
    case ACTION_DOOR_SABER:
        return ::ACTION_DOORSABER;
    case ACTION_SWITCH_WEAPONS:
        return ::ACTION_SWITCHWEAPONS;
    case ACTION_STANCE_TOTAL_DEFENSE:
        return ::ACTION_TOTALDEFENSE;
    case ACTION_STANCE_MEDITATIVE:
        return ::ACTION_MEDITATIVE;
    default:
        return ::ACTION_INVALID;
    }

}

unsigned short CSWSObject::GetIDByAQActionID(unsigned short nAQActionID)
{
    switch (nAQActionID)
    {
    case ::ACTION_MOVETOPOINT:
        return ACTION_MOVE_TO_POINT;
    case ::ACTION_PICKUPITEM:
        return ACTION_PICK_UP_ITEM;
    case ::ACTION_DROPITEM:
        return ACTION_DROP_ITEM;
    case ::ACTION_ATTACKOBJECT:
        return ACTION_PHYSICAL_ATTACKS;
    case AQ_ACTION_COMBAT:
        return ACTION_COMBAT;
    case ::ACTION_CASTSPELL:
        return ACTION_CAST_SPELL;
    case ::ACTION_OPENDOOR:
        return ACTION_OPEN_DOOR;
    case ::ACTION_CLOSEDOOR:
        return ACTION_CLOSE_DOOR;
    case ::ACTION_DIALOGOBJECT:
        return ACTION_DIALOG_OBJECT;
    case ::ACTION_DISABLETRAP:
        return ACTION_DISARM_MINE;
    case ::ACTION_RECOVERTRAP:
        return ACTION_RECOVER_MINE;
    case ::ACTION_FLAGTRAP:
        return ACTION_FLAG_MINE;
    case ::ACTION_EXAMINETRAP:
        return ACTION_EXAMINE_MINE;
    case ::ACTION_SETTRAP:
        return ACTION_SET_MINE;
    case ::ACTION_OPENLOCK:
        return ACTION_UNLOCK_OBJECT;
    case ::ACTION_LOCK:
        return ACTION_LOCK_OBJECT;
    case ::ACTION_USEOBJECT:
        return ACTION_USE_OBJECT;
    case ::ACTION_ANIMALEMPATHY:
        return ACTION_ANIMAL_EMPATHY;
    case ::ACTION_REST:
        return ACTION_REST;
    case ::ACTION_TAUNT:
        return ACTION_TAUNT;
    case ::ACTION_ITEMCASTSPELL:
        return ACTION_ITEM_CAST_SPELL;
    case AQ_ACTION_UNEQUIP_ITEM:
        return ACTION_UNEQUIP_ITEM;
    case ::ACTION_COUNTERSPELL:
        return ACTION_COUNTER_SPELL;
    case ::ACTION_HEAL:
        return ACTION_HEAL;
    case ::ACTION_FOLLOW:
        return ACTION_FORCE_FOLLOW_OBJECT;
    case ::ACTION_FOLLOWLEADER:
        return ACTION_FOLLOW_LEADER;
    case AQ_ACTION_SPECIAL_WAIT:
    case ::ACTION_WAIT:
        return ACTION_WAIT;
    default:
        return ACTION_TYPE_INVALID;
    }
}
// PC 0x005DAFA0
int CSWSObject::AIActionDialogObject(CSWSObjectActionNode* pNode)
{
    if (GetDead() == 1 || GetIsPCDying() == 1)
    {
        SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
        return ACTION_FAILED;
    }

    unsigned long oidTarget = (unsigned long)pNode->m_apParam[0];
    CExoString sDialog;
    sDialog = *(CExoString*)pNode->m_apParam[1];
    int bPrivateConversation = (int)pNode->m_apParam[2];
    int nDialogType = (int)pNode->m_apParam[3];
    int bIgnoreStartRange = (int)pNode->m_apParam[4];
    unsigned long oidOriginalLeader = (unsigned long)pNode->m_apParam[5];

    int nCharacters = g_pAppManager->GetClientExoApp()->GetSWParty()->m_nCharacters;
    if (nCharacters > 0)
    {
        int nCharacter = 0;
        while (1)
        {
            CSWCCreature* pCharacter =
                g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(nCharacter);
            if (pCharacter != NULL && pCharacter->GetServerCreature() != NULL &&
                pCharacter->GetServerCreature()->GetDeadTemp() == 0)
                break;
            if (++nCharacter >= nCharacters)
            {
                SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID);
                g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
                return ACTION_FAILED;
            }
        }
    }

    if (oidOriginalLeader == OBJECT_INVALID)
    {
        CGameObject* pGameObject;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidTarget, &pGameObject) != CGameObjectArray::SUCCESS)
        {
            SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
            return ACTION_FAILED;
        }
        CSWSObject* pTarget = pGameObject->AsSWSObject();
        if (pTarget->m_bCommandable == 0 || pTarget->GetDead() == 1)
        {
            SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
            return ACTION_FAILED;
        }

        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
        CSWSCreature* pPlayerCreature = g_pAppManager->GetServerExoApp()->GetPlayerCreature();
        if (pPlayerCreature == NULL)
            return ACTION_IN_PROGRESS;
        unsigned long oidPlayer = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(
            pPlayerCreature->GetId());
        CSWCObject* pClientPlayer = g_pAppManager->GetClientExoApp()->GetGameObject(oidPlayer);
        if (pClientPlayer == NULL || pClientPlayer->AsSWCObject() == NULL)
            return ACTION_IN_PROGRESS;

        int bDelayDialog = g_pAppManager->GetClientExoApp()->GetAreaNotReadyToDisplay() ||
            g_pAppManager->GetClientExoApp()->GetPausedByCombat() ||
            g_pAppManager->GetClientExoApp()->GetAutoPaused();
        if (bDelayDialog)
        {
            AddActionToFront(ACTION_DIALOG_OBJECT, pNode->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_STRING, &sDialog,
                ACTION_PARAM_INTEGER, &bPrivateConversation,
                ACTION_PARAM_INTEGER, &nDialogType,
                ACTION_PARAM_INTEGER, &bIgnoreStartRange,
                ACTION_PARAM_OBJECT, &oidOriginalLeader,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            return ACTION_COMPLETE;
        }

        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(1);
        if (AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = AsSWSCreature();
            pCreature->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);
            pCreature->SetActivity(CSWSCreature::ACTIVITY_DIALOG, 0, 0);
            if (bIgnoreStartRange == 0)
            {
                if (nDialogType == 0)
                {
                    Vector vDelta = m_vPosition - pTarget->m_vPosition;
                    if (vDelta * vDelta > CSWSArea::MAX_OBJECT_START_CONVERSATION_DISTANCE *
                        CSWSArea::MAX_OBJECT_START_CONVERSATION_DISTANCE)
                    {
                        unsigned long oidArea = pGameObject->AsSWSObject()->m_oidArea;
                        Vector vPosition;
                        float fRange;
                        AsSWSCreature()->GetUseRange(oidTarget, vPosition, fRange, 0);
                        fRange += 1.0f;
                        vPosition = (vPosition + m_vPosition) * 0.5f;

                        AddActionToFront(ACTION_DIALOG_OBJECT, pNode->m_nGroupId,
                            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_STRING, &sDialog,
                            ACTION_PARAM_INTEGER, &bPrivateConversation,
                            ACTION_PARAM_INTEGER, &nDialogType,
                            ACTION_PARAM_INTEGER, &bIgnoreStartRange,
                            ACTION_PARAM_INTEGER, &oidOriginalLeader,
                            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                        pCreature->AddMoveToPointActionToFront(pNode->m_nGroupId,
                            vPosition, oidArea, oidTarget, 1, fRange, 0.0f,
                            0, 0, 0, 0, 0, 0, Vector(), 0, 0, 0);
                        return ACTION_COMPLETE;
                    }
                }
                else
                {
                    nDialogType = 0;
                    if (!AsSWSCreature()->GetIsInUseRange(oidTarget, 1.0f, 0))
                    {
                        unsigned long oidArea = pGameObject->AsSWSObject()->m_oidArea;
                        Vector vPosition;
                        float fRange;
                        AsSWSCreature()->GetUseRange(oidTarget, vPosition, fRange, 0);
                        fRange += 1.0f;
                        vPosition = (vPosition + m_vPosition) * 0.5f;
                        AddActionToFront(ACTION_DIALOG_OBJECT, pNode->m_nGroupId,
                            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_STRING, &sDialog,
                            ACTION_PARAM_INTEGER, &bPrivateConversation,
                            ACTION_PARAM_INTEGER, &nDialogType,
                            ACTION_PARAM_INTEGER, &bIgnoreStartRange,
                            ACTION_PARAM_OBJECT, &oidOriginalLeader,
                            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                        pCreature->AddMoveToPointActionToFront(pNode->m_nGroupId,
                            vPosition, oidArea, oidTarget, 1, fRange, 0.0f,
                            0, 0, 0, 0, 0, 0, Vector(), 0, 0, 0);
                        return ACTION_COMPLETE;
                    }
                }
            }
        }

        CSWSCreature* pTargetCreature = pTarget->AsSWSCreature();
        unsigned long oidPlayerCreature = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
        CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayerCreature);
        if (pPlayer != NULL && GetClientObject() != NULL &&
            GetClientObject()->AsSWCCreature() != NULL &&
            GetClientObject()->AsSWCCreature()->m_bInParty && GetId() != pPlayer->m_oidPCObject)
        {
            if (pTarget->m_bPartyInteract != 0 || !pTarget->GetDialogResref().IsValid() ||
                pTarget->GetId() == pPlayer->m_oidPCObject)
            {
                if (pTarget->GetId() == pPlayer->m_oidPCObject && pTarget->GetClientObject() != NULL &&
                    pTarget->GetClientObject()->AsSWCCreature() != NULL &&
                    pTarget->GetClientObject()->AsSWCCreature()->m_bInParty)
                {
                    g_pAppManager->GetClientExoApp()->ChangeCharacter(CSWParty::LEADER_PLAYER, 0);
                }
            }
            else
            {
                CSWSCreature* pOriginalPlayer = g_pAppManager->GetServerExoApp()->GetGameObject(
                    pPlayer->m_oidPCObject)->AsSWSCreature();
                if (pOriginalPlayer != NULL && pOriginalPlayer->m_bInParty)
                {
                    if (pOriginalPlayer->m_bCombatState && pOriginalPlayer->m_nItemUseState == CSWSCreature::COMBAT_STATE_NOTIFY)
                    {
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
                        return ACTION_FAILED;
                    }
                    CSWCCreature* pLeader = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
                    unsigned long oidServerLeader = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pLeader->GetId());
                    CSWSCreature* pServerLeader = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidServerLeader);
                    Vector vLeaderPosition = pServerLeader->m_vPosition;
                    pLeader->ClearAllActions();
                    pServerLeader->ClearAllActions(1);
                    for (int nMember = 1; nMember < CSWParty::MAX_MEMBERS; nMember++)
                    {
                        CSWCCreature* pMember =
                            g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(nMember);
                        if (pMember != NULL)
                        {
                            unsigned long oidServerMember = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pMember->GetId());
                            CGameObject* pServerMember = g_pAppManager->GetServerExoApp()->GetGameObject(oidServerMember);
                            if (pServerMember != NULL && pServerMember->AsSWSCreature() != NULL)
                            {
                                pServerMember->AsSWSCreature()->ClearAllActions(1);
                                pMember->ClearAllActions();
                            }
                            Vector vDelta = vLeaderPosition - pMember->m_vPosition;
                            if (vDelta * vDelta >= DIALOG_FOLLOWER_RELOCATE_DISTANCE_SQUARED)
                            {
                                CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
                                Vector vOffset = pParty->m_aPartyMemberData[nMember].m_vFormationOffset;
                                Vector vFollow = pParty->m_aPartyMemberData[nMember].m_vFormationPosition;
                                int nPathIndex = pParty->m_aPartyMemberData[nMember].m_nFormationPathIndex;
                                float fFacing = pParty->m_cFollowPaths.GetPathData(nPathIndex)->m_fFacing;
                                Vector vAxis(0.0f, 0.0f, 1.0f);
                                Vector vPosition = vFollow + Quaternion(vAxis, DegToRad(fFacing)) * vOffset;
                                g_pAppManager->GetServerExoApp()->ComputeSafeLocation(
                                    vPosition, 10.0f, pServerMember->GetId(), 0, &vPosition);
                                pMember->SetPosition(vPosition);
                            }
                        }
                    }
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
                        0, 0.0f, 0.75f, Vector(0.0f, 0.0f, 0.0f));
                    g_pExoInput->SetActive(0);
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog = 1;
                    oidOriginalLeader = pServerLeader->GetId();
                    if (pOriginalPlayer->GetDeadTemp())
                    {
                        CGameEffect* pEffect = new CGameEffect(1);
                        pEffect->m_nType = CSWSEffectListHandler::EFFECT_RESURRECTION;
                        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                        pOriginalPlayer->ApplyEffect(pEffect, 0, 0);
                        pOriginalPlayer->m_nF0 = 1;
                    }
                    pOriginalPlayer->AddActionToFront(ACTION_DIALOG_OBJECT, pNode->m_nGroupId,
                        ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_STRING, &sDialog,
                        ACTION_PARAM_INTEGER, &bPrivateConversation,
                        ACTION_PARAM_INTEGER, &nDialogType,
                        ACTION_PARAM_INTEGER, &bIgnoreStartRange,
                        ACTION_PARAM_OBJECT, &oidOriginalLeader,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                    return ACTION_FAILED;
                }
            }

        }
        pTargetCreature = pTarget->AsSWSCreature();
        if (pTargetCreature != NULL && pTargetCreature->m_bInParty)
        {
            pTargetCreature->m_n1204 = 1;
            pTargetCreature->SetPreDialogPosition(pTargetCreature->m_vPosition);
            pTargetCreature->SetPreDialogOrientation(pTargetCreature->m_vOrientation);
        }
    }
    else
    {
        if (!g_pAppManager->GetClientExoApp()->GetInGameGui()->IsGlobalFading())
        {
            CGameObject* pOriginalLeader = g_pAppManager->GetServerExoApp()->GetGameObject(oidOriginalLeader);
            if (pOriginalLeader != NULL && pOriginalLeader->AsSWSCreature() != NULL)
            {
                CSWSCreature* pLeaderCreature = pOriginalLeader->AsSWSCreature();
                Vector vPosition = m_vPosition;
                Vector vLeaderPosition = pLeaderCreature->m_vPosition;
                Vector vOrientation = pLeaderCreature->m_vOrientation;
                if (pLeaderCreature->m_bInParty)
                {
                    pLeaderCreature->m_n1204 = 1;
                    pLeaderCreature->SetPreDialogPosition(vLeaderPosition);
                    pLeaderCreature->SetPreDialogOrientation(vOrientation);
                }
                CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
                if (pTarget != NULL && pTarget->AsSWSObject() != NULL)
                {
                    const Vector& vTargetPosition = pTarget->AsSWSObject()->m_vPosition;
                    vOrientation = vTargetPosition - vLeaderPosition;
                }
                SetPosition(vLeaderPosition, 1, 1, 0);
                SetOrientation(vOrientation);
                pOriginalLeader->AsSWSCreature()->SetPosition(vPosition, 1, 1, 0);
                if (GetClientObject() != NULL && GetClientObject()->AsSWCCreature() != NULL)
                {
                    GetClientObject()->AsSWCCreature()->SetPosition(vLeaderPosition);
                    GetClientObject()->AsSWCCreature()->SetOrientation(vOrientation);
                }
                unsigned long oidServerLeader = pOriginalLeader->AsSWSCreature()->GetId();
                unsigned long oidClientLeader = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidServerLeader);
                g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidClientLeader);
                if (pOriginalLeader->AsSWSCreature() != NULL &&
                    pOriginalLeader->AsSWSCreature()->GetClientCreature() != NULL)
                    pOriginalLeader->AsSWSCreature()->GetClientCreature()->SetPosition(vPosition);
                unsigned long oidPlayerCreature = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayerCreature);
                g_pAppManager->GetClientExoApp()->ChangeCharacter(CSWParty::LEADER_PLAYER, 0);
                unsigned long oidPostDialogLeader = oidOriginalLeader;
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_oidPostDialogCharacterSwitch = oidPostDialogLeader;
                g_pExoInput->SetActive(1);
            }
        }
        else
        {
            AddActionToFront(ACTION_DIALOG_OBJECT, pNode->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_STRING, &sDialog,
                ACTION_PARAM_INTEGER, &bPrivateConversation,
                ACTION_PARAM_INTEGER, &nDialogType,
                ACTION_PARAM_INTEGER, &bIgnoreStartRange,
                ACTION_PARAM_OBJECT, &oidOriginalLeader,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            return ACTION_FAILED;
        }
    }

    CScriptEvent* pEvent = new CScriptEvent;
    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DIALOGUE;
    pEvent->SetInteger(0, -1);
    pEvent->SetInteger(1, -1);
    pEvent->SetInteger(2, bPrivateConversation);
    pEvent->SetInteger(3, bIgnoreStartRange);
    pEvent->SetString(0, sDialog);
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
        0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
    return ACTION_COMPLETE;
}
// PC 0x005DC030
int CSWSObject::AIActionPauseDialog(CSWSObjectActionNode* pNode)
{
    CSWSCreature* pCreature;
    int nDeadTemp = 0;

    if (AsSWSCreature() != NULL)
    {
        pCreature = AsSWSCreature();
        nDeadTemp = pCreature->GetDeadTemp();
    }

    if (GetDead() == 1 || GetIsPCDying() == 1 || nDeadTemp == 1)
    {
        SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID);
        return ACTION_FAILED;
    }

    m_nDialogPending = 1;
    if (AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature2 = AsSWSCreature();
        pCreature2->m_bInDialog |= CSWSCreature::DIALOG_PAUSED;
    }
    return ACTION_COMPLETE;
}
// PC 0x005DC0D0
int CSWSObject::AIActionResumeDialog(CSWSObjectActionNode* pNode)
{
    int nDeadTemp = 0;

    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (GetDead() == 1 || GetIsPCDying() == 1 || nDeadTemp == 1)
    {
        SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID);
        return ACTION_FAILED;
    }
    else
    {
        m_nDialogPending = 0;
        if (AsSWSCreature() != NULL)
        {
            AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);
            CSWSCreature* pCreature = AsSWSCreature();
            pCreature->m_bInDialog &= ~CSWSCreature::DIALOG_PAUSED;
            AsSWSCreature()->SetActivity(CSWSCreature::ACTIVITY_STEALTH, 0, 0);
        }
        return ACTION_COMPLETE;
    }
}
// PC 0x005DC190
int CSWSObject::AIActionSpeakStrRef(CSWSObjectActionNode* pNode)
{
    if (AsSWSCreature() != NULL)
    {
        AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);
        AsSWSCreature()->SetActivity(CSWSCreature::ACTIVITY_STEALTH, 0, 0);
    }

    unsigned long nPlayer = (unsigned long)pNode->m_apParam[0];
    unsigned long nStrRef = (unsigned long)pNode->m_apParam[1];
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    pServer->GetSWSMessage()->SendServerToPlayerChatStrRefMessage(
        nStrRef, m_nId, nPlayer);
    return ACTION_COMPLETE;
}
// PC 0x005DC1F0
int CSWSObject::AIActionSpeak(CSWSObjectActionNode* pNode)
{
    CExoString sMessage;

    if (AsSWSCreature() != NULL)
    {
        AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);
        AsSWSCreature()->SetActivity(CSWSCreature::ACTIVITY_STEALTH, 0, 0);
    }

    sMessage = *(CExoString*)pNode->m_apParam[0];
    unsigned long nType = (unsigned long)pNode->m_apParam[1];
    {
        CExoString sTargetName("");
        g_pAppManager->GetServerExoApp()->GetSWSMessage()
            ->SendServerToPlayerChatMessage(nType, m_nId, sMessage, -1,
                                            sTargetName);
    }
    return ACTION_COMPLETE;
}
// PC 0x005DC2F0
int CSWSObject::AIActionDoCommand(CSWSObjectActionNode* pNode)
{
    int nDeadTemp = 0;

    if (AsSWSCreature() != NULL)
    {
        nDeadTemp = AsSWSCreature()->GetDeadTemp();
    }

    if (GetDead() == 1 || GetIsPCDying() == 1 || nDeadTemp == 1)
    {
        return ACTION_FAILED;
    }

    g_pVirtualMachine->RunScriptSituation(pNode->m_apParam[0], m_nId, 1);
    pNode->m_apParam[0] = NULL;
    return ACTION_COMPLETE;
}
// PC 0x005DC380
int CSWSObject::AIActionWait(CSWSObjectActionNode* pNode)
{
    int nDeadTemp = 0;

    if (AsSWSCreature() != NULL)
    {
        nDeadTemp = AsSWSCreature()->GetDeadTemp();
    }

    if (GetDead() == 1 || GetIsPCDying() == 1 || nDeadTemp != 0)
    {
        return ACTION_FAILED;
    }
    float fWaitDuration = *(float*)&pNode->m_apParam[0];
    unsigned long nCurrentDay;
    unsigned long nCurrentTime;
    unsigned long nElapsedDay;
    unsigned long nElapsedTime;

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    pServer->GetWorldTimer()->GetWorldTime(
        &nCurrentDay, &nCurrentTime);
    pServer = g_pAppManager->m_pServerExoApp;
    pServer->GetWorldTimer()->SubtractWorldTimes(
        nCurrentDay, nCurrentTime, m_nActionStartDay, m_nActionStartTime,
        &nElapsedDay, &nElapsedTime);
    if (nElapsedTime < (unsigned long)(fWaitDuration * 1000.0f))
    {
        return ACTION_IN_PROGRESS;
    }
    return ACTION_COMPLETE;
}
// PC 0x005E08F0
int CSWSObject::AIActionGiveItem(CSWSObjectActionNode* pNode)
{
    CSWSItem* pItem = NULL;
    if (pNode->m_nNumParams < 2)
        return ACTION_FAILED;

    unsigned long oidItem = (unsigned long)pNode->m_apParam[0];
    unsigned long oidTarget = (unsigned long)pNode->m_apParam[1];
    int nQuantity = (int)pNode->m_apParam[2];
    int bAddToQueue = (int)pNode->m_apParam[3];
    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
        return ACTION_FAILED;

    if (AsSWSCreature() != NULL &&
        !AsSWSCreature()->GetIsInUseRange(oidTarget, 0.0f, 0))
    {
        Vector vPosition;
        float fRange;
        int bRun = 0;
        int bCheckRange = 1;
        AsSWSCreature()->GetUseRange(oidTarget, vPosition, fRange, 0);
        Vector vDelta = m_vPosition - vPosition;
        if (vDelta * vDelta > GIVE_ITEM_RUN_DISTANCE_SQUARED)
            bRun = 1;

        AddActionToFront(ACTION_GIVE_ITEM, pNode->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_INTEGER, &nQuantity, ACTION_PARAM_INTEGER, &bAddToQueue,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        AddActionToFront(ACTION_CHANGE_FACING_POINT, pNode->m_nGroupId,
            ACTION_PARAM_FLOAT, &vPosition.m_fX,
            ACTION_PARAM_FLOAT, &vPosition.m_fY,
            ACTION_PARAM_FLOAT, &vPosition.m_fZ,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, pNode->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
            ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_FLOAT, &fRange,
            ACTION_PARAM_INTEGER, &bCheckRange,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        unsigned long oidArea = m_oidArea;
        Vector vOrientation;
        AsSWSCreature()->AddMoveToPointActionToFront(
            pNode->m_nGroupId, vPosition, oidArea, oidTarget,
            bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0, vOrientation, 0, 0, 0);
        return ACTION_COMPLETE;
    }

    pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
        return ACTION_FAILED;
    if (pItem->GetBaseItem()->GetItemType() == CSWBaseItem::ITEM_TYPE_CREDITS)
    {
        if (AsSWSCreature() != NULL)
        {
            int nGold = pItem->m_nStackSize;
            AsSWSCreature()->RemoveGold(nGold, 1);
        }
    }
    else if (pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_PAZAAK_CARD &&
             AsSWSCreature() != NULL)
        AsSWSCreature()->RemovePazaakCard(pItem);

    int bDisplayFeedback = 1;
    if (pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_CREDITS)
        bDisplayFeedback = 0;
    if (nQuantity != GIVE_ITEM_QUANTITY_ALL)
    {
        CSWSItem* pSplitItem = pItem->SplitItem(nQuantity);
        if (pSplitItem != NULL)
            pItem = pSplitItem;
    }

    if (pTarget->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
    {
        {
            CExoLocString sName;
            if (pItem != NULL)
            {
                sName = pItem->m_sLocalizedName;
            }
            unsigned long oidPossessor = pItem->m_oidPossessor;
            pTarget->AsSWSCreature()->AcquireItem(&pItem, oidPossessor,
                                                 OBJECT_INVALID, bDisplayFeedback);
            if (bAddToQueue && pTarget->AsSWSCreature()->m_bInParty)
            {
                g_pAppManager->GetClientExoApp()->GetInGameGui()->AddItemReceived(sName);
                g_pAppManager->GetClientExoApp()->GetInGameGui()
                    ->UpdateStatus(GUI_STATUS_ITEM_RECEIVED, 0);
            }
        }
    }
    else if (pTarget->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
    {
        {
            unsigned long oidPossessor = pItem->m_oidPossessor;
            pTarget->AsSWSPlaceable()->AcquireItem(&pItem, oidPossessor,
                                                  bDisplayFeedback);
        }
    }
    else if (pTarget->m_nObjectType == SW_OBJECT_TYPE_ITEM)
    {
        {
            int nBaseItem = pTarget->AsSWSItem()->m_nBaseItem;
            if (g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_bIsContainer == 1)
            {
                unsigned long oidPossessor = pItem->m_oidPossessor;
                pTarget->AsSWSItem()->AcquireItem(&pItem, oidPossessor,
                                                 bDisplayFeedback);
            }
        }
    }
    return ACTION_COMPLETE;
}
// PC 0x005DC480
int CSWSObject::AIActionTakeItem(CSWSObjectActionNode* pNode)
{
    if (pNode->m_nNumParams < 1)
    {
        return ACTION_FAILED;
    }

    unsigned long oidItem = (unsigned long)pNode->m_apParam[0];
    unsigned long oidPossessor = (unsigned long)pNode->m_apParam[1];
    int bAddToQueue = (int)pNode->m_apParam[2];
    CGameObject* pGameObject;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CGameObjectArray* pObjectArray = pServer->GetObjectArray();
    CSWSItem* pItem = NULL;
    if (pObjectArray->GetGameObject(
            oidItem, &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pItem = pGameObject->AsSWSItem();
    }
    if (pItem == NULL)
    {
        return ACTION_FAILED;
    }
    if (m_nObjectType == SW_OBJECT_TYPE_CREATURE)
    {
        if (AsSWSCreature()->AcquireItem(&pItem, oidPossessor,
                                        OBJECT_INVALID, bAddToQueue) &&
            bAddToQueue)
        {
            CSWSCreature* pPossessor = g_pAppManager->m_pServerExoApp
                ->GetCreatureByGameObjectID(oidPossessor);
            if (pPossessor != NULL && pPossessor->m_bInParty)
            {
                g_pAppManager->m_pClientExoApp->GetInGameGui()
                    ->UpdateStatus(GUI_STATUS_ITEM_LOST, 0);
                return ACTION_COMPLETE;
            }
        }
    }
    else if (m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
    {
        AsSWSPlaceable()->AcquireItem(&pItem, oidPossessor, 1);
    }
    return ACTION_COMPLETE;
}
// PC 0x005DC590
int CSWSObject::AIActionCloseDoor(CSWSObjectActionNode* pNode)
{
    float fDelay;
    float fRange;
    int nDeadTemp = 0;
    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (GetDead() != 1 && GetIsPCDying() != 1 && nDeadTemp != 1)
    {
        if (AsSWSCreature() != NULL)
            AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);

        int bRun = 0;
        unsigned long oidDoor = (unsigned long)pNode->m_apParam[0];
        if (pNode->m_nNumParams > 1 && pNode->m_apParam[1] != NULL)
            bRun = (int)pNode->m_apParam[1];

        CGameObject* pObject;
        Vector vPosition;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        if (pServer->GetObjectArray()->GetGameObject(
                oidDoor, &pObject) == CGameObjectArray::SUCCESS)
        {
            if (AsSWSCreature() != NULL)
            {
                if (pObject->AsSWSDoor() == NULL && pObject->AsSWSPlaceable() == NULL)
                    return ACTION_FAILED;

                CSWSArea* pArea = ((CSWSObject*)pObject)->GetArea();
                AsSWSCreature()->GetUseRange(oidDoor, vPosition, fRange, 0);
                if (GetArea() != pObject->AsSWSObject()->GetArea() ||
                    !AsSWSCreature()->GetIsInUseRange(oidDoor, 0.0f, 0))
                {
                    AsSWSCreature()->AddActionToFront(
                        ACTION_CLOSE_DOOR, ACTION_GROUP_NEW,
                        ACTION_PARAM_OBJECT, &oidDoor, ACTION_PARAM_INTEGER, &bRun,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    Vector vOrientation;
                    AsSWSCreature()->AddMoveToPointActionToFront(
                        pNode->m_nGroupId, vPosition,
                        pObject->AsSWSObject()->m_oidArea, OBJECT_INVALID,
                        bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0, vOrientation, 0, 0, 0);
                    return ACTION_COMPLETE;
                }
            }

            SetLockOrientationToObject(oidDoor);
            pServer = g_pAppManager->m_pServerExoApp;
            pServer->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, m_nId, oidDoor, CServerAIMaster::EVENT_CLOSE_OBJECT, NULL);
            fDelay = 0.5f;
            AddActionToFront(ACTION_WAIT, pNode->m_nGroupId,
                             ACTION_PARAM_FLOAT, &fDelay,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            return ACTION_COMPLETE;
        }
    }
    return ACTION_FAILED;
}
// PC 0x005DC870
int CSWSObject::AIActionLockObject(CSWSObjectActionNode* pNode)
{
    int bSuccess = 0;
    int bTake20 = 0;
    int nDeadTemp = 0;
    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (GetDead() != 1 && GetIsPCDying() != 1 && nDeadTemp != 1)
    {
        if (AsSWSCreature() != NULL)
            AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);

        unsigned long oidObject = (unsigned long)pNode->m_apParam[0];
        CGameObject* pGameObject = g_pAppManager->m_pServerExoApp->GetGameObject(oidObject);
        CSWSObject* pObject = pGameObject->AsSWSObject();
        if (pObject == NULL)
        {
            if (AsSWSCreature() != NULL)
            {
                AsSWSCreature()->StopGuiTimingBar();
                AsSWSCreature()->m_bLockActionStarted = 0;
            }
            return ACTION_FAILED;
        }

        if (AsSWSCreature() != NULL)
        {
            Vector vPosition;
            float fRange;
            if (!AsSWSCreature()->GetIsInUseRange(oidObject, 0.0f, 0))
            {
                AsSWSCreature()->GetUseRange(oidObject, vPosition, fRange, 0);
                AddActionToFront(ACTION_LOCK_OBJECT, pNode->m_nGroupId,
                    ACTION_PARAM_OBJECT, &oidObject,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
                    pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                {
                    AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pNode->m_nGroupId,
                        ACTION_PARAM_OBJECT, &oidObject,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                }
                else
                {
                    AddActionToFront(ACTION_CHANGE_FACING_POINT, pNode->m_nGroupId,
                        ACTION_PARAM_FLOAT, &vPosition.m_fX,
                        ACTION_PARAM_FLOAT, &vPosition.m_fY,
                        ACTION_PARAM_FLOAT, &vPosition.m_fZ,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                }
                unsigned long oidArea = pObject->m_oidArea;
                Vector vOrientation;
                AsSWSCreature()->AddMoveToPointActionToFront(
                    pNode->m_nGroupId, vPosition, oidArea, oidObject,
                    1, fRange, 0.0f, 0, 0, 0, 0, 0, 0, vOrientation, 0, 0, 0);
                return ACTION_COMPLETE;
            }
        }

        if (AsSWSCreature() != NULL && !AsSWSCreature()->m_bLockActionStarted)
        {
            AsSWSCreature()->m_bLockActionStarted = 1;
            int nAnimation;
            int bLooping = 1;
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                nAnimation = CREATURE_ANIMATION_LOCKPICK_DOOR;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                nAnimation = CREATURE_ANIMATION_LOCKPICK_PLACEABLE;
            float fSpeed = 1.0f;
            float fDuration = 1.5f;
            AddActionToFront(ACTION_LOCK_OBJECT, pNode->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidObject,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            AddActionToFront(ACTION_PLAY_ANIMATION, pNode->m_nGroupId,
                ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
                ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLooping,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            AsSWSCreature()->StartGuiTimingBar(CSWSCreature::LOCK_ACTION_TIME_MS,
                                               CSWSCreature::GUI_TIMING_LOCK);
            return ACTION_COMPLETE;
        }

        if (AsSWSCreature() != NULL && AsSWSCreature()->EvaluateLock(pObject, 1) == 1)
        {
            AsSWSCreature()->StopGuiTimingBar();
            AsSWSCreature()->m_bLockActionStarted = 0;
            return ACTION_COMPLETE;
        }

        if (AsSWSCreature() == NULL ||
            AsSWSCreature()->GetIsInUseRange(oidObject, 0.0f, 0))
        {
            if (pObject->AsSWSDoor() != NULL)
            {
                if (pObject->AsSWSDoor()->m_bLocked)
                {
                    if (AsSWSCreature() != NULL)
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetObjectID(0, pObject->GetId());
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_LOCKED, pData);

                        if (pData != NULL)
                        {
                            AsSWSCreature()->StopGuiTimingBar();
                            AsSWSCreature()->m_bLockActionStarted = 0;
                        }
                        else
                        {
                            AsSWSCreature()->StopGuiTimingBar();
                            AsSWSCreature()->m_bLockActionStarted = 0;
                        }
                    }
                    return ACTION_FAILED;
                }
                if (pObject->AsSWSDoor()->m_bKeyRequired)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidObject, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    if (AsSWSCreature() != NULL)
                    {
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_KEY_REQUIRED, NULL);
                        AsSWSCreature()->StopGuiTimingBar();
                        AsSWSCreature()->m_bLockActionStarted = 0;
                    }
                    return ACTION_FAILED;
                }
                if (pObject->GetId() == GetId())
                {
                    pObject->AsSWSDoor()->m_bLocked = 1;
                    bSuccess = 1;
                }
            }
            else if (pObject->AsSWSPlaceable() != NULL)
            {
                if (pObject->AsSWSPlaceable()->m_bLocked)
                {
                    if (AsSWSCreature() != NULL)
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetObjectID(0, pObject->GetId());
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_LOCKED, pData);
                        if (pData != NULL)
                        {
                            AsSWSCreature()->StopGuiTimingBar();
                            AsSWSCreature()->m_bLockActionStarted = 0;
                        }
                        else
                        {
                            AsSWSCreature()->StopGuiTimingBar();
                            AsSWSCreature()->m_bLockActionStarted = 0;
                        }
                    }
                    return ACTION_FAILED;
                }
                if (pObject->AsSWSPlaceable()->m_bKeyRequired)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidObject, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    if (AsSWSCreature() != NULL)
                    {
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_KEY_REQUIRED, NULL);
                        AsSWSCreature()->StopGuiTimingBar();
                        AsSWSCreature()->m_bLockActionStarted = 0;
                    }
                    return ACTION_FAILED;
                }
                if (pObject->GetId() == GetId())
                {
                    pObject->AsSWSPlaceable()->m_bLocked = 1;
                    bSuccess = 1;
                }
            }

            int nRoll;
            int nSkillRank;

            int nDC;
            if (AsSWSCreature() != NULL)
            {
                if (AsSWSCreature()->m_bCombatState)
                    nRoll = rand() % 20 + 1;
                else
                {
                    nRoll = 20;
                    bTake20 = 1;
                }
                nSkillRank = AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_SECURITY, NULL, 0);
                if (pObject->AsSWSDoor() != NULL)
                {
                    if (pObject->AsSWSDoor()->m_nCloseLockDC < 1)
                        nDC = 1;
                    else
                        nDC = pObject->AsSWSDoor()->m_nCloseLockDC;
                    if (nRoll + nSkillRank >= nDC)
                    {
                        pObject->AsSWSDoor()->m_bLocked = 1;
                        bSuccess = 1;
                    }
                }
                else if (pObject->AsSWSPlaceable() != NULL)
                {
                    if (pObject->AsSWSPlaceable()->m_nCloseLockDC < 1)
                        nDC = 1;
                    else
                        nDC = pObject->AsSWSPlaceable()->m_nCloseLockDC;
                    if (nRoll + nSkillRank >= nDC)
                    {
                        pObject->AsSWSPlaceable()->m_bLocked = 1;
                        bSuccess = 1;
                    }
                }
            }

            if (bSuccess)
            {
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidObject, CServerAIMaster::EVENT_LOCK_OBJECT, NULL);
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_LOCKED;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), pObject->GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
            if (AsSWSCreature() != NULL)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, GetId());
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, CSWCCMessageData::SKILL_DATA_SECURITY_STRREF);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, nRoll);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, nSkillRank);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, nDC);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, bTake20);
                if (nRoll + nSkillRank >= nDC)
                    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_SUCCESS);
                else if (bTake20)
                    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_RETRY);
                else
                    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_FAILED);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_SECURITY_STRREF);
                AsSWSCreature()->BroadcastSkillData(pData);
            }
        }
        if (AsSWSCreature() != NULL)
        {
            AsSWSCreature()->StopGuiTimingBar();
            AsSWSCreature()->m_bLockActionStarted = 0;
        }
        return ACTION_COMPLETE;
    }
    if (AsSWSCreature() != NULL)
    {
        AsSWSCreature()->StopGuiTimingBar();
        AsSWSCreature()->m_bLockActionStarted = 0;
    }
    return ACTION_FAILED;
}
// PC 0x005DD170
int CSWSObject::AIActionSwitchWeapons(CSWSObjectActionNode* pNode)
{
    CSWSCreature* pCreature = AsSWSCreature();
    ClearAllActions(1);
    if (pCreature != NULL)
    {
        CSWSItem* pRight = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        CSWSItem* pLeft = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
        CSWSItem* pRight2 = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND2);
        CSWSItem* pLeft2 = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND2);
        pCreature->ClearAllActions(1);
        pCreature->m_pcCombatRound->RemoveAllActions();
        int bInstant = 1;
        if (pRight2 == NULL && pLeft2 == NULL)
        {
            bInstant = 0;
        }
        if (g_pAppManager->m_pClientExoApp->GetPlayerMovementInput())
        {
            bInstant = 1;
        }
        if (pLeft2 != NULL)
        {
            pCreature->RunUnequip(pLeft2->m_nId, CGameObjectArray::INVALIDOBJID, 0, NULL, 0, bInstant);
        }
        if (pRight2 != NULL)
        {
            pCreature->RunUnequip(pRight2->m_nId, CGameObjectArray::INVALIDOBJID, 0, NULL, 0, bInstant);
        }
        if (pLeft != NULL)
        {
            pCreature->RunUnequip(pLeft->m_nId, CGameObjectArray::INVALIDOBJID, 0, NULL, 0, bInstant);
        }
        if (pRight != NULL)
        {
            pCreature->RunUnequip(pRight->m_nId, CGameObjectArray::INVALIDOBJID, 0, NULL, 0, bInstant);
        }
        if (pRight2 != NULL)
        {
            pCreature->RunEquip(pRight2->m_nId, INVENTORY_SLOT_RIGHTHAND, 0);
        }
        if (pLeft2 != NULL)
        {
            pCreature->RunEquip(pLeft2->m_nId, INVENTORY_SLOT_LEFTHAND, 0);
        }
        if (pRight != NULL)
        {
            pCreature->RunEquip(pRight->m_nId, INVENTORY_SLOT_RIGHTHAND2, 0);
        }
        if (pLeft != NULL)
        {
            pCreature->RunEquip(pLeft->m_nId, INVENTORY_SLOT_LEFTHAND2, 0);
        }
        if (pCreature->m_bCombatState && pCreature->m_nItemUseState == 1)
        {
            unsigned long oidTarget = pCreature->m_pcCombatRound->m_oidUnknown9CC;
            if (oidTarget != OBJECT_INVALID)
            {
                pCreature->AddAttackActions(oidTarget, 0, 0, 0, 0, 0, 0, 1, 10009,
                                            1500, 1, 0, 0, 4, 0);
            }
        }
    }
    return ACTION_COMPLETE;
}
// PC 0x005DD350
int CSWSObject::AIActionSetCommandable(CSWSObjectActionNode* pNode)
{
    int nDeadTemp = 0;

    if (AsSWSCreature() != NULL)
    {
        nDeadTemp = AsSWSCreature()->GetDeadTemp();
    }

    if (GetDead() || GetIsPCDying() || nDeadTemp)
    {
        return ACTION_FAILED;
    }

    int bCommandable;
    memcpy(&bCommandable, &pNode->m_apParam[0], sizeof(bCommandable));
    m_bCommandable = bCommandable;
    return ACTION_COMPLETE;
}
// PC 0x005DD3C0
int CSWSObject::AddUseObjectAction(unsigned long oidObject)
{
    if (m_bCommandable == 0)
    {
        return 0;
    }

    AddAction(0x28u, 0xffffu, 3u, &oidObject, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
#ifndef _XBOX
int CSWSObject::AddGiveItemActions(unsigned long oidItem, unsigned long oidTarget,  // PC 0x005DD420
                                  int nQuantity, int bAddToQueue)
{
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pGameObject == NULL)
    {
        return 0;
    }
    if (pGameObject->AsSWSObject() == NULL)
    {
        return 0;
    }

    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        CSWSCreature* pCreature = AsSWSCreature();
        unsigned long oidPossessor = pItem->m_oidPossessor;
        CSWSCreature* pPossessor =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPossessor);
        if ((pPossessor == NULL || pCreature == NULL ||
             pPossessor->m_bInParty == 0 || pCreature->m_bInParty == 0) &&
            pItem->m_oidPossessor != GetId())
        {
            return 0;
        }

        if (m_bCommandable != 0)
        {
            int nStackSize = pItem->m_nStackSize;
            if (nQuantity > nStackSize)
            {
                nQuantity = nStackSize;
            }
            AddAction(ACTION_GIVE_ITEM, ACTION_GROUP_LAST,
                      ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidTarget,
                      ACTION_PARAM_INTEGER, &nQuantity, ACTION_PARAM_INTEGER, &bAddToQueue,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif
int CSWSObject::AddTakeItemActions(unsigned long oidItem, unsigned long oidTarget,  // PC 0x005DD530
                                  int bAddToQueue)
{
    int bRun = 0;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServer->GetGameObject(oidTarget);
    if (pGameObject == NULL)
    {
        return 0;
    }
    CSWSObject* pTarget = pGameObject->AsSWSObject();
    if (pTarget == NULL)
    {
        return 0;
    }
    pServer = g_pAppManager->m_pServerExoApp;
    CSWSItem* pItem = pServer->GetItemByGameObjectID(oidItem);
    if (pItem == NULL || pItem->m_oidPossessor == m_nId || m_bCommandable == 0)
    {
        return 0;
    }
    if (AsSWSCreature() != NULL)
    {
        Vector vDifference = m_vPosition - pTarget->m_vPosition;
        if (vDifference.m_fX * vDifference.m_fX +
            vDifference.m_fY * vDifference.m_fY +
            vDifference.m_fZ * vDifference.m_fZ > TAKE_ITEM_RUN_DISTANCE_SQUARED)
        {
            bRun = 1;
        }
        Vector vOrientation;
        unsigned long oidArea = pTarget->GetArea()->GetId();
        AsSWSCreature()->AddMoveToPointAction(
            ACTION_GROUP_NEW, pTarget->m_vPosition, oidArea,
            oidTarget, bRun, 1.0f, 0.0f, 0, 0, 0, 0, 0, 0, vOrientation, 0, 0, 0);
    }
    AddAction(ACTION_TAKE_ITEM, ACTION_GROUP_NEW,
              ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidTarget,
              ACTION_PARAM_INTEGER, &bAddToQueue,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x005DD6C0
int CSWSObject::AddDoCommandAction(void* pScriptSituation)
{
    if (m_bCommandable == 0)
    {
        g_pVirtualMachine->DeleteScriptSituation(pScriptSituation);
        return 0;
    }

    AddAction(0x25u, 0xffffu, 5u, pScriptSituation, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
#ifndef _XBOX
int CSWSObject::GetReputation(unsigned long oidObject, int bUnknown)  // PC 0x005DD730
{
    if (OverrideReputation != REPUTATION_NO_OVERRIDE)
    {
        return OverrideReputation;
    }

    CGameObject* pObject;
    CGameObject* pCreator;
    int nTargetFaction;
    int nSourceFaction;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    int nReputation = REPUTATION_MIN;
    if (pServer->GetObjectArray()->GetGameObject(
            oidObject, &pObject) == CGameObjectArray::SUCCESS)
    {
        CFactionManager* pFactionManager =
            g_pAppManager->m_pServerExoApp->m_pcExoAppInternal->m_pFactionManager;
        if (pFactionManager != NULL)
        {
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT &&
                pObject->AsSWSAreaOfEffectObject() != NULL)
            {
                oidObject = pObject->AsSWSAreaOfEffectObject()->m_oidCreator;
            }

            if (AsSWSAreaOfEffectObject() != NULL)
            {
                unsigned long oidCreator = AsSWSAreaOfEffectObject()->m_oidCreator;
                pServer = g_pAppManager->m_pServerExoApp;
                if (pServer->GetObjectArray()->GetGameObject(
                        oidCreator, &pCreator) == CGameObjectArray::SUCCESS)
                {
                    return pCreator->AsSWSObject()->GetReputation(oidObject, bUnknown);
                }
            }
            else
            {
                pServer = g_pAppManager->m_pServerExoApp;
                if (pServer->GetFactionOfObject(
                        oidObject, &nTargetFaction))
                {
                    unsigned long oidSource = m_nId;
                    pServer = g_pAppManager->m_pServerExoApp;
                    if (pServer->GetFactionOfObject(oidSource, &nSourceFaction))
                    {
                        if (oidObject == m_nId)
                        {
                            return REPUTATION_MAX;
                        }
                        if (pFactionManager->GetIsNPCFaction(nTargetFaction) == 0 &&
                            pFactionManager->GetIsNPCFaction(nSourceFaction) == 1)
                        {
                            unsigned long oidSelf = m_nId;
                            return pObject->AsSWSObject()->GetReputation(oidSelf, 0);
                        }
                        if (AsSWSCreature() != NULL)
                        {
                            nReputation = AsSWSCreature()->GetCreatureReputation(
                                oidObject, nTargetFaction);
                        }
                        else if (AsSWSPlaceable() != NULL || AsSWSEncounter() != NULL ||
                                 AsSWSDoor() != NULL || AsSWSTrigger() != NULL)
                        {
                            nReputation = pFactionManager->GetNPCFactionReputation(
                                nTargetFaction, nSourceFaction);
                        }
                        if (nReputation < REPUTATION_MIN)
                        {
                            nReputation = REPUTATION_MIN;
                        }
                        else if (nReputation > REPUTATION_MAX)
                        {
                            nReputation = REPUTATION_MAX;
                        }
                        return nReputation;
                    }
                }
            }
        }
    }
    return REPUTATION_DEFAULT;
}
#endif
unsigned char CSWSObject::GetAIStateReputation(unsigned long oidObject)  // PC 0x005DD940
{
    CServerExoApp* pServer;
    int nReputation;
    CGameObject* pGameObject;

    GetArea();
    pServer = g_pAppManager->m_pServerExoApp;
    pServer->GetObjectArray()->GetGameObject(oidObject, &pGameObject);
    if (pGameObject != NULL)
    {
        if (pGameObject->AsSWSObject() != NULL)
        {
            unsigned long oidSelf = m_nId;
            nReputation = pGameObject->AsSWSObject()->GetReputation(oidSelf, 0);
            if (nReputation >= 90)
            {
                return 1;
            }
            if (nReputation <= 10)
            {
                return 2;
            }
        }
    }
    return 0;
}
// PC 0x005DD9B0
int CSWSObject::AIActionBarkString(CSWSObjectActionNode* pNode)
{
    unsigned long nStrRef = (unsigned long)pNode->m_apParam[0];
    STR_RES cResult;
    CClientExoApp* pClientExoApp;
    unsigned long oidClientObject;
    unsigned long oidServerObject;

    g_pTlkTable->Fetch(nStrRef, cResult, CTlkTable::GENDER_MALE);
    CResRef resSound(cResult.m_resSound);
    CExoString sBarkString(cResult.m_sString);
    oidServerObject = m_nId;
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    oidClientObject = pClientExoApp->ServerToClientObjectId(oidServerObject);
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->BarkString(oidClientObject, sBarkString, resSound, 0);
    return ACTION_COMPLETE;
}
// PC 0x005DDAB0
int CSWSObject::AIActionPlaySound(CSWSObjectActionNode* pNode)
{
    CExoLinkedListNode* pPosition;
    CSWSPlayer* pPlayer;
    CSWSObject* pPlayerObject;
    CSWSMessage* pMessage;
    CExoString sSound;
    CExoLinkedList<CSWSClient>* pPlayerList;

    sSound = *(CExoString*)pNode->m_apParam[0];
    pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    pPlayerList = g_pAppManager->GetServerExoApp()->GetPlayerList();
    for (pPosition = pPlayerList->GetHeadPos(); pPosition != NULL;
         pPlayerList->GetNext(pPosition))
    {
        pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer->GetGameObject() != NULL)
        {
            pPlayerObject = pPlayer->GetGameObject();
            if (pPlayerObject != NULL)
            {
                CSWSArea* pArea = pPlayerObject->GetArea();
                Vector vPosition = pPlayerObject->m_vPosition;
                if (pArea == GetArea())
                {
                    Vector vDistance = vPosition - m_vPosition;
                    if (vDistance * vDistance <=
                            CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE *
                                CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE &&
                        pMessage != NULL)
                    {
                        pMessage->SendServerToPlayerAIActionPlaySound(
                            pPlayer->GetPlayerId(), m_nId, sSound);
                    }
                }
            }
        }
    }
    return ACTION_COMPLETE;
}

#include "nwsdoor.h"
#include "nwsscriptevent.h"
// PC 0x005DDC30
int CSWSObject::AIActionPlayAnimation(CSWSObjectActionNode* pNode)
{
    int nDeadTemp = 0;
    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (GetDead() == 1 || GetIsPCDying() == 1 || nDeadTemp == 1)
    {
        return ACTION_FAILED;
    }

    int nAnimation = (int)pNode->m_apParam[0];
    float fSpeed = *(float*)&pNode->m_apParam[1];
    float fDuration = *(float*)&pNode->m_apParam[2];
    int bFirstUpdate = (int)pNode->m_apParam[3];
    int bSoundPlayed = (int)pNode->m_apParam[4];
    unsigned long nSoundDelay = (unsigned long)pNode->m_apParam[5];
    unsigned long nCurrentDay;
    unsigned long nCurrentTime;
    unsigned long nElapsedDay;
    unsigned long nElapsedTime;

    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    pServer->GetWorldTimer()->GetWorldTime(&nCurrentDay, &nCurrentTime);
    if (bFirstUpdate == 1)
    {
        m_nC4 = nCurrentDay;
        m_nC8 = nCurrentTime;
        pNode->m_apParam[3] = (void*)0;
        pServer = g_pAppManager->GetServerExoApp();
        if (GetId() == pServer->GetPlayerCreatureId())
        {
            switch (nAnimation)
            {
            case CREATURE_ANIMATION_LOCKPICK_DOOR:
                nSoundDelay = 250;
                break;
            case CREATURE_ANIMATION_LOCKPICK_PLACEABLE:
                nSoundDelay = 250;
                break;
            case CREATURE_ANIMATION_MINE_DISARM_1:
                nSoundDelay = 750;
                break;
            case CREATURE_ANIMATION_MINE_DISARM_2:
            case CREATURE_ANIMATION_MINE_DISARM_3:
                nSoundDelay = 750;
                break;
            case CREATURE_ANIMATION_MINE_ARM:
                nSoundDelay = 750;
                break;
            case CREATURE_ANIMATION_MINE_DISARM_4:
                nSoundDelay = 750;
                break;
            case CREATURE_ANIMATION_DOOR_SABER:
                nSoundDelay = 0;
                break;
            default:
                bSoundPlayed = 1;
                pNode->m_apParam[4] = (void*)bSoundPlayed;
                break;
            }
            pNode->m_apParam[5] = (void*)nSoundDelay;
        }
    }
    pServer = g_pAppManager->GetServerExoApp();
    pServer->GetWorldTimer()->SubtractWorldTimes(
        nCurrentDay, nCurrentTime, m_nC4, m_nC8,
        &nElapsedDay, &nElapsedTime);

    int bDone = 0;
    if (fDuration >= 30.0)
    {
        while (!m_lstActions.IsEmpty() && bDone == 0)
        {
            bDone = 1;
            CSWSObjectActionNode* pAction = m_lstActions.RemoveHead();
            if (pAction != NULL && pAction->m_nActionType == ACTION_PLAY_ANIMATION)
            {
                int nActionAnimation = (int)pAction->m_apParam[0];
                void* pActionSpeed = pAction->m_apParam[1];
                if (nActionAnimation == nAnimation &&
                    *(float*)&pActionSpeed == fSpeed && fDuration >= 30.0)
                {
                    delete pAction;
                    bDone = 0;
                    continue;
                }
            }
            m_lstActions.AddHead(pAction);
        }
    }

    float fAnimationLength = 1000.0f;
    float fAbsSpeed = (float)fabs(fSpeed);
    int bFireAndForget = 0;
    if (GetClientObject() != NULL)
    {
        float fLength = GetClientObject()->GetAnimationLength(nAnimation);
        if (fLength > 0.0f)
        {
            fAnimationLength = fLength;
        }
        bFireAndForget = GetClientObject()->AnimationFireAndForget(nAnimation);
    }
    if (fAbsSpeed != 0.0f)
    {
        fAnimationLength = fAnimationLength / fAbsSpeed;
    }
    if (fDuration > 0.0f && bFireAndForget == 0)
    {
        fAnimationLength = fDuration * 1000.0f;
    }

    if (bSoundPlayed == 0 && nSoundDelay <= nElapsedTime)
    {
        pNode->m_apParam[4] = (void*)1;
        CExoString sSound;
        int bPlaySound = 1;
        switch (nAnimation)
        {
        case CREATURE_ANIMATION_LOCKPICK_DOOR:
        case CREATURE_ANIMATION_LOCKPICK_PLACEABLE:
            sSound = "gui_lockpick";
            break;
        case CREATURE_ANIMATION_MINE_DISARM_1:
        case CREATURE_ANIMATION_MINE_DISARM_2:
        case CREATURE_ANIMATION_MINE_DISARM_3:
        case CREATURE_ANIMATION_MINE_DISARM_4:
            sSound = "gui_minedisarm";
            break;
        case CREATURE_ANIMATION_MINE_ARM:
            sSound = "gui_minearm";
            break;
        case CREATURE_ANIMATION_DOOR_SABER:
            sSound = "gui_doorsaber";
            break;
        default:
            bPlaySound = 0;
            break;
        }
        if (bPlaySound)
        {
            CResRef cSound((char*)sSound.CStr());
            g_pExoSound->PlayOneShotSound(cSound, CExoSound::PRIORITY_GROUP_ACTION_ANIMATION, 0, 0, 0.0f, 0.0f);
        }
    }

    if (nElapsedTime < (unsigned long)fAnimationLength)
    {
        m_fSyncHeight = fSpeed;
        SetAnimation((unsigned short)nAnimation);
        return ACTION_IN_PROGRESS;
    }
    if (AsSWSCreature() != NULL)
    {
        m_fSyncHeight = 1.0f;
        SetAnimation(ANIMATION_COMBAT_READY);
    }
    return ACTION_COMPLETE;
}
#ifndef _XBOX
int CSWSObject::AIActionOpenDoor(CSWSObjectActionNode* pNode)  // PC 0x005DE080
{
    int nDeadTemp = 0;
    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (GetDead() != 1 && GetIsPCDying() != 1 && nDeadTemp != 1)
    {
        if (AsSWSCreature() != NULL)
            AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);

        int bRun = 1;
        unsigned long oidDoor = (unsigned long)pNode->m_apParam[0];
        if (pNode->m_nNumParams > 1 && pNode->m_apParam[1] != NULL)
            bRun = (int)pNode->m_apParam[1];

        CGameObject* pObject;
        Vector vPosition;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidDoor, &pObject) == CGameObjectArray::SUCCESS)
        {
            if (AsSWSCreature() != NULL)
            {
                CSWSDoor* pDoor = pObject->AsSWSDoor();
                if (pDoor == NULL)
                {
                    m_bOpenDoorStarted = 0;
                    return ACTION_FAILED;
                }
                CSWSArea* pArea = ((CSWSObject*)pObject)->GetArea();
                float fRange;
                AsSWSCreature()->GetUseRange(oidDoor, vPosition, fRange, 0);
                if (pDoor->m_nAnimation != DOOR_ANIMATION_CLOSED)
                    return ACTION_COMPLETE;

                if (GetArea() != pObject->AsSWSObject()->GetArea() ||
                    !AsSWSCreature()->GetIsInUseRange(oidDoor, 0.0f, 0))
                {
                    AsSWSCreature()->AddActionToFront(
                        ACTION_OPEN_DOOR, ACTION_GROUP_NEW,
                        ACTION_PARAM_OBJECT, &oidDoor, ACTION_PARAM_INTEGER, &bRun,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    if (pDoor->m_bPreciseUse && pDoor->m_bLocked)
                    {
                        float fWait = 0.5f;
                        AsSWSCreature()->AddActionToFront(
                            ACTION_WAIT, ACTION_GROUP_LAST, ACTION_PARAM_FLOAT, &fWait,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        AsSWSCreature()->AddActionToFront(
                            ACTION_CHANGE_FACING_OBJECT, ACTION_GROUP_LAST,
                            ACTION_PARAM_OBJECT, &oidDoor,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    }
                    Vector vOrientation;
                    AsSWSCreature()->AddMoveToPointActionToFront(
                        ACTION_GROUP_LAST, vPosition,
                        pObject->AsSWSObject()->m_oidArea, OBJECT_INVALID,
                        bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0, vOrientation, 0, 0, 0);
                    m_bOpenDoorStarted = 1;
                    return ACTION_COMPLETE;
                }
                else
                {
                    if (pDoor->m_bTrapFlag &&
                        pDoor->GetReputation(GetId(), 0) < MIN_FRIENDLY_REPUTATION)
                    {
                        int nDoorFaction = pDoor->m_nFactionId;
                        if (nDoorFaction != AsSWSCreature()->GetFactionId())
                        {
                            CScriptEvent* pEvent = new CScriptEvent;
                            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                            unsigned long oidSelf = m_nId;
                            GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                                0, 0, oidSelf, oidDoor, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                            return ACTION_FAILED;
                        }
                    }
                    if (!AsSWSCreature()->EvaluateLock(pObject->AsSWSObject(), 0))
                    {
                        CScriptEvent* pEvent = new CScriptEvent;
                        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN;
                        unsigned long oidSelf = m_nId;
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, oidSelf, oidDoor, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                        m_bOpenDoorStarted = 0;
                        return ACTION_FAILED;
                    }
                    SetLockOrientationToObject(oidDoor);
                    if (!m_bOpenDoorStarted)
                    {
                        AsSWSCreature()->AddActionToFront(
                            ACTION_OPEN_DOOR, ACTION_GROUP_NEW,
                            ACTION_PARAM_OBJECT, &oidDoor, ACTION_PARAM_INTEGER, &bRun,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        m_bOpenDoorStarted = 1;
                        return ACTION_COMPLETE;
                    }
                }
            }
            m_bOpenDoorStarted = 0;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, m_nId, oidDoor, CServerAIMaster::EVENT_OPEN_OBJECT, NULL);
            float fDelay = 0.5f;
            AddActionToFront(ACTION_WAIT, pNode->m_nGroupId,
                ACTION_PARAM_FLOAT, &fDelay,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            return ACTION_COMPLETE;
        }
    }
    m_bOpenDoorStarted = 0;
    return ACTION_FAILED;
}
#endif
// PC 0x005DE5D0
int CSWSObject::AIActionUnlockObject(CSWSObjectActionNode* pNode)
{
    int bSuccess = 0;
    int bTake20 = 0;
    int nDeadTemp = 0;
    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (GetDead() != 1 && GetIsPCDying() != 1 && nDeadTemp != 1)
    {
        if (AsSWSCreature() != NULL)
            AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);

        unsigned long oidTarget = (unsigned long)pNode->m_apParam[0];
        unsigned long oidItem = (unsigned long)pNode->m_apParam[1];
        int nProperty = (int)pNode->m_apParam[2];
        CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
        if (pObject != NULL)
        {
            CSWSObject* pTarget = pObject->AsSWSObject();
            if (pTarget == NULL)
            {
                if (AsSWSCreature() != NULL)
                {
                    AsSWSCreature()->StopGuiTimingBar();
                    AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                    AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                }
                return ACTION_FAILED;
            }

            if (AsSWSCreature() != NULL)
            {
                Vector vPosition;
                float fRange;
                if (!AsSWSCreature()->GetIsInUseRange(oidTarget, 0.0f, 0))
                {
                    AsSWSCreature()->GetUseRange(oidTarget, vPosition, fRange, 0);
                    AddActionToFront(ACTION_UNLOCK_OBJECT, pNode->m_nGroupId,
                        ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_OBJECT, &oidItem,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
                        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pNode->m_nGroupId,
                            ACTION_PARAM_OBJECT, &oidTarget,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    else
                        AddActionToFront(ACTION_CHANGE_FACING_POINT, pNode->m_nGroupId,
                            ACTION_PARAM_FLOAT, &vPosition.m_fX, ACTION_PARAM_FLOAT, &vPosition.m_fY,
                            ACTION_PARAM_FLOAT, &vPosition.m_fZ,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    unsigned long oidArea = pTarget->m_oidArea;
                    Vector vOrientation;
                    AsSWSCreature()->AddMoveToPointActionToFront(pNode->m_nGroupId,
                        vPosition, oidArea, oidTarget, 1, fRange, 0.0f,
                        0, 0, 0, 0, 0, 0, vOrientation, 0, 0, 0);
                    return ACTION_COMPLETE;
                }
                if (!AsSWSCreature()->m_bUnlockObjectFacingSet)
                {
                    AsSWSCreature()->m_bUnlockObjectFacingSet = 1;
                    AsSWSCreature()->GetUseRange(oidTarget, vPosition, fRange, 0);
                    AddActionToFront(ACTION_UNLOCK_OBJECT, pNode->m_nGroupId,
                        ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_OBJECT, &oidItem,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
                        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pNode->m_nGroupId,
                            ACTION_PARAM_OBJECT, &oidTarget,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    else
                        AddActionToFront(ACTION_CHANGE_FACING_POINT, pNode->m_nGroupId,
                            ACTION_PARAM_FLOAT, &vPosition.m_fX, ACTION_PARAM_FLOAT, &vPosition.m_fY,
                            ACTION_PARAM_FLOAT, &vPosition.m_fZ,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    if (rand() % 100 < 20)
                        AsSWSCreature()->BroadcastVoiceChat(CSWSCreature::VOICE_CHAT_PICK_LOCK);
                    return ACTION_COMPLETE;
                }
            }

            if (AsSWSCreature() != NULL && !AsSWSCreature()->m_bUnlockObjectAnimationPlayed)
            {
                AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 1;
                SetLockOrientationToObject(oidTarget);
                int nAnimation = CREATURE_ANIMATION_LOCKPICK_PLACEABLE;
                int bLoopAnimation = 1;
                if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                    nAnimation = CREATURE_ANIMATION_LOCKPICK_DOOR;
                else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                    nAnimation = CREATURE_ANIMATION_LOCKPICK_PLACEABLE;
                float fSpeed = 1.0f;
                float fDuration = 1.5f;
                AddActionToFront(ACTION_UNLOCK_OBJECT, pNode->m_nGroupId,
                    ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_OBJECT, &oidItem,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                AddActionToFront(ACTION_PLAY_ANIMATION, pNode->m_nGroupId,
                    ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
                    ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLoopAnimation,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                AsSWSCreature()->StartGuiTimingBar(CSWSCreature::UNLOCK_OBJECT_DURATION,
                                                 CSWSCreature::GUI_TIMING_UNLOCK_OBJECT);
                return ACTION_COMPLETE;
            }

            CSWSDoor* pDoor = pTarget->AsSWSDoor();
            CSWSPlaceable* pPlaceable = pTarget->AsSWSPlaceable();
            if (pDoor != NULL && pDoor->m_bTrapFlag && AsSWSCreature() != NULL)
            {
                int nReputation = pDoor->GetReputation(GetId(), 0);
                if (nReputation < MIN_FRIENDLY_REPUTATION)
                {
                    int nFaction = pDoor->m_nFactionId;
                    if (nFaction != AsSWSCreature()->GetFactionId())
                    {
                        CScriptEvent* pEvent = new CScriptEvent;
                        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                        GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                        AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                        AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                        return ACTION_FAILED;
                    }
                }
            }
            else if (pPlaceable != NULL && pPlaceable->m_nTrapFlag && AsSWSCreature() != NULL &&
                     pPlaceable->GetReputation(GetId(), 0) < MIN_FRIENDLY_REPUTATION)
            {
                int nFaction = pPlaceable->m_nFaction;
                if (nFaction != AsSWSCreature()->GetFactionId())
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                    GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                    AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                    return ACTION_FAILED;
                }
            }
            if (AsSWSCreature() != NULL && AsSWSCreature()->EvaluateLock(pTarget, 0) == 1)
            {
                if (pDoor != NULL)
                    pDoor->OpenDoor(GetId());
                else if (pPlaceable != NULL)
                {
                    unsigned long oidPlaceable = pPlaceable->GetId();
                    AddActionToFront(ACTION_USE_OBJECT, pNode->m_nGroupId,
                        ACTION_PARAM_OBJECT, &oidPlaceable,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                }
                AsSWSCreature()->StopGuiTimingBar();
                AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                return ACTION_COMPLETE;
            }
            if (AsSWSCreature() == NULL || AsSWSCreature()->GetIsInUseRange(oidTarget, 0.0f, 0))
            {
            if (pDoor != NULL)
            {
                if (!pDoor->m_bLocked)
                {
                    if (AsSWSCreature() != NULL)
                    {
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_ALREADY_UNLOCKED, NULL);
                        AsSWSCreature()->StopGuiTimingBar();
                        AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                        AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                    }
                    return ACTION_FAILED;
                }
                if (pDoor->m_bKeyRequired)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    if (AsSWSCreature() != NULL)
                    {
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_KEY_REQUIRED, NULL);
                        AsSWSCreature()->StopGuiTimingBar();
                        AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                        AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                    }
                    if (AsSWSCreature() != NULL)
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, GetId());
                        pData->SetObjectID(CSWCCMessageData::SKILL_DATA_TARGET, oidTarget);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, CSWCCMessageData::SKILL_DATA_SECURITY_STRREF);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, 0);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, 0);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, 0);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, 0);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_IMPOSSIBLE);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_SECURITY_STRREF);
                        AsSWSCreature()->BroadcastSkillData(pData);
                        AsSWSCreature()->BroadcastVoiceChat(CSWSCreature::VOICE_CHAT_PICK_LOCK_FAILED);
                    }
                    return ACTION_FAILED;
                }
                if (pTarget->GetId() == GetId())
                {
                    pDoor->m_bLocked = 0;
                    pDoor->OpenDoor(GetId());
                    bSuccess = 1;
                }
            }
            else if (pPlaceable != NULL)
            {
                if (!pPlaceable->m_bLocked)
                {
                    if (AsSWSCreature() != NULL)
                    {
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_ALREADY_UNLOCKED, NULL);
                        AsSWSCreature()->StopGuiTimingBar();
                        AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                        AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                    }
                    return ACTION_FAILED;
                }
                if (pPlaceable->m_bKeyRequired)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    if (AsSWSCreature() != NULL)
                    {
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_KEY_REQUIRED, NULL);
                        AsSWSCreature()->StopGuiTimingBar();
                        AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                        AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                    }
                    if (AsSWSCreature() != NULL)
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, GetId());
                        pData->SetObjectID(CSWCCMessageData::SKILL_DATA_TARGET, oidTarget);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, CSWCCMessageData::SKILL_DATA_SECURITY_STRREF);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, 0);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, 0);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, 0);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, 0);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_IMPOSSIBLE);
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_SECURITY_STRREF);
                        AsSWSCreature()->BroadcastSkillData(pData);
                        AsSWSCreature()->BroadcastVoiceChat(CSWSCreature::VOICE_CHAT_PICK_LOCK_FAILED);
                    }
                    return ACTION_FAILED;
                }
                if (pTarget->GetId() == GetId())
                {
                    pPlaceable->m_bLocked = 0;
                    bSuccess = 1;
                }
            }

            int nRoll;
            int nSkill;
            int nDC;
            CSWSItem* pItem;
            if (AsSWSCreature() != NULL)
            {
                if (AsSWSCreature()->m_bCombatState)
                    nRoll = rand() % 20 + 1;
                else
                {
                    nRoll = 20;
                    bTake20 = 1;
                }
                nSkill = AsSWSCreature()->GetStats()->GetSkillRank(CSWRules::SKILL_SECURITY, NULL, 0);
                int nBonus = 0;
                pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
                if (pItem != NULL)
                {
                    CSWItemProperty* pProperty = pItem->GetActiveProperty(nProperty);
                    if (pProperty != NULL)
                    {
                        unsigned char nUpgradeType = pProperty->m_nUpgradeType;
                        int bActive = pItem->GetIsUpgradeActive(nUpgradeType);
                        if (bActive)
                            nBonus = pProperty->GetCostValue();
                    }
                }
                nSkill += nBonus;
                if (pTarget->AsSWSDoor() != NULL)
                {
                    nDC = pTarget->AsSWSDoor()->GetOpenLockDC() < 1 ? 1 : pTarget->AsSWSDoor()->GetOpenLockDC();
                    if (nRoll + nSkill >= nDC)
                    {
                        pTarget->AsSWSDoor()->m_bLocked = 0;
                        pTarget->AsSWSDoor()->OpenDoor(GetId());
                        bSuccess = 1;
                    }
                }
                else if (pTarget->AsSWSPlaceable() != NULL)
                {
                    nDC = pTarget->AsSWSPlaceable()->GetOpenLockDC() < 1 ? 1 : pTarget->AsSWSPlaceable()->GetOpenLockDC();
                    if (nRoll + nSkill >= nDC)
                    {
                        pTarget->AsSWSPlaceable()->m_bLocked = 0;
                        if (AsSWSCreature()->GetStats()->GetIsPC() ||
                            g_pAppManager->GetServerExoApp()->GetPartyTable()->GetIsLeader(GetId()))
                        {
                            unsigned long oidPlaceable = pTarget->GetId();
                            AddActionToFront(ACTION_USE_OBJECT, pNode->m_nGroupId,
                                ACTION_PARAM_OBJECT, &oidPlaceable,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        }
                        bSuccess = 1;
                    }
                }
            }
            if (bSuccess)
            {
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_UNLOCK_OBJECT, NULL);
                CScriptEvent* pUnlockEvent = new CScriptEvent;
                pUnlockEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_UNLOCKED;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), pTarget->GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pUnlockEvent);
                if (AsSWSCreature() != NULL && rand() % 100 < 20)
                    AsSWSCreature()->BroadcastVoiceChat(CSWSCreature::VOICE_CHAT_PICK_LOCK_SUCCESS);
                if (AsSWSCreature() != NULL && AsSWSCreature()->GetInParty())
                {
                    CSWSCreature* pPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreature();
                    int nExperience;
                    if (nDC < pPlayer->GetStats()->GetLevel(0) + 20)
                        nExperience = 5 * pPlayer->GetStats()->GetLevel(0);
                    else
                        nExperience = 10 * pPlayer->GetStats()->GetLevel(0);
                    g_pAppManager->GetServerExoApp()->GetPartyTable()->DistributeExperience(nExperience, 1);
                }
            }
            else
            {
                if (AsSWSCreature() != NULL)
                    AsSWSCreature()->BroadcastVoiceChat(CSWSCreature::VOICE_CHAT_PICK_LOCK_FAILED);
                CScriptEvent* pFailEvent = new CScriptEvent;
                pFailEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN;
                pFailEvent->SetInteger(0, 1);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventAbsoluteTime(
                    0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pFailEvent);
            }

            if (AsSWSCreature() != NULL)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, GetId());
                pData->SetObjectID(CSWCCMessageData::SKILL_DATA_TARGET, oidTarget);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, CSWCCMessageData::SKILL_DATA_SECURITY_STRREF);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, nRoll);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, nSkill);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, nDC);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, bTake20);
                if (nRoll + nSkill >= nDC && nDC < CSWSCreature::UNLOCK_OBJECT_IMPOSSIBLE_DC)
                    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_SUCCESS);
                else if (bTake20 && nRoll + nSkill < nDC)
                {
                    if (nDC >= CSWSCreature::UNLOCK_OBJECT_IMPOSSIBLE_DC)
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_IMPOSSIBLE);
                    else
                        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_RETRY);
                }
                else if (nRoll + nSkill < nDC)
                    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_FAILED);
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_SECURITY_STRREF);
                AsSWSCreature()->BroadcastSkillData(pData);
                if (pItem != NULL && AsSWSCreature()->GetItemRepository(1)->GetItemInRepository(pItem, 1))
                {
                    if (pItem->m_nStackSize > 1)
                        --pItem->m_nStackSize;
                    else if (AsSWSCreature()->RemoveItem(pItem, 1, 1, NULL))
                    {
                        CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
                        if (pPlayer != NULL)
                            g_pAppManager->GetServerExoApp()->GetSWSMessage()->SendServerPlayerItemUpdate_DestroyItem(pPlayer, pItem->GetId());
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, GetId(), pItem->GetId(), CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                    }
                }
            }
            }
            if (AsSWSCreature() != NULL)
            {
                AsSWSCreature()->StopGuiTimingBar();
                AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
                AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
            }
            return ACTION_COMPLETE;
        }
    }
    if (AsSWSCreature() != NULL)
    {
        AsSWSCreature()->StopGuiTimingBar();
        AsSWSCreature()->m_bUnlockObjectAnimationPlayed = 0;
        AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
    }
    return ACTION_FAILED;
}
// PC 0x005DF660
int CSWSObject::AIActionDoorSaber(CSWSObjectActionNode* pNode)
{
    int nDeadTemp = 0;
    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (!GetDead() && !GetIsPCDying() && !nDeadTemp)
    {
        if (AsSWSCreature() != NULL)
            AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);

        unsigned long oidTarget = (unsigned long)pNode->m_apParam[0];
        CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
        if (pObject != NULL)
        {
            CSWSObject* pTarget = pObject->AsSWSObject();
            if (pTarget != NULL)
            {
                CSWSItem* pItem = AsSWSCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                if (pItem != NULL && pItem->GetBaseItem() != NULL &&
                    pItem->GetBaseItem()->m_bPoweredItem)
                    pItem->GetClientItem()->PowerItem(1, 0, 1, 0);
                pItem = AsSWSCreature()->GetInventory()->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
                if (pItem != NULL && pItem->GetBaseItem() != NULL &&
                    pItem->GetBaseItem()->m_bPoweredItem)
                    pItem->GetClientItem()->PowerItem(1, 0, 1, 0);
                AsSWSCreature()->SetCombatState(1, CSWSCreature::COMBAT_STATE_EQUIPMENT);

                if (AsSWSCreature() != NULL)
                {
                    Vector vPosition;
                    float fRange = 0.0f;
                    if (!AsSWSCreature()->GetIsInUseRange(oidTarget, 0.0f, 0, 1))
                    {
                        AsSWSCreature()->GetUseRange(oidTarget, vPosition, fRange, 0, 1);
                        AddActionToFront(ACTION_DOOR_SABER, pNode->m_nGroupId,
                            ACTION_PARAM_OBJECT, &oidTarget,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pNode->m_nGroupId,
                            ACTION_PARAM_OBJECT, &oidTarget,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        Vector vOrientation;
                        AsSWSCreature()->AddMoveToPointActionToFront(pNode->m_nGroupId,
                            vPosition, pTarget->m_oidArea, oidTarget, 1, fRange, 0.0f,
                            0, 0, 0, 0, 0, 0, vOrientation, 0, 0, 0);
                        return ACTION_COMPLETE;
                    }
                    if (!AsSWSCreature()->m_bUnlockObjectFacingSet)
                    {
                        AsSWSCreature()->m_bUnlockObjectFacingSet = 1;
                        AsSWSCreature()->GetUseRange(oidTarget, vPosition, fRange, 0);
                        AddActionToFront(ACTION_DOOR_SABER, pNode->m_nGroupId,
                            ACTION_PARAM_OBJECT, &oidTarget,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pNode->m_nGroupId,
                            ACTION_PARAM_OBJECT, &oidTarget,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        return ACTION_COMPLETE;
                    }
                }

                if (AsSWSCreature() != NULL && !AsSWSCreature()->m_bDoorSaberAnimationPlayed)
                {
                    AsSWSCreature()->m_bDoorSaberAnimationPlayed = 1;
                    SetLockOrientationToObject(oidTarget);
                    int nAnimation = CSWSCreature::ANIMATION_DOOR_SABER;
                    int bLoopAnimation = 1;
                    float fSpeed = 1.0f;
                    float fDuration = 2.0f;
                    AddActionToFront(ACTION_DOOR_SABER, pNode->m_nGroupId,
                        ACTION_PARAM_OBJECT, &oidTarget,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    AddActionToFront(ACTION_PLAY_ANIMATION, pNode->m_nGroupId,
                        ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
                        ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLoopAnimation,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    AsSWSCreature()->StartGuiTimingBar(CSWSCreature::DOOR_SABER_DURATION,
                                                     CSWSCreature::GUI_TIMING_DOOR_SABER);
                    return ACTION_COMPLETE;
                }

                CSWSDoor* pDoor = pTarget->AsSWSDoor();
                if (pDoor != NULL && pDoor->m_bTrapFlag && AsSWSCreature() != NULL &&
                    pDoor->GetReputation(GetId(), 0) < MIN_FRIENDLY_REPUTATION)
                {
                    int nFaction = pDoor->m_nFactionId;
                    if (nFaction != AsSWSCreature()->GetFactionId())
                    {
                        CScriptEvent* pEvent = new CScriptEvent;
                        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                        AsSWSCreature()->StopGuiTimingBar();
                        AsSWSCreature()->m_bDoorSaberAnimationPlayed = 0;
                        AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                        return ACTION_FAILED;
                    }
                }
                if (AsSWSCreature() == NULL || AsSWSCreature()->GetIsInUseRange(oidTarget, 0.0f, 0, 1))
                {
                    if (pDoor != NULL && !pDoor->m_bLocked)
                    {
                        if (AsSWSCreature() != NULL)
                        {
                            AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_ALREADY_UNLOCKED, NULL);
                            AsSWSCreature()->StopGuiTimingBar();
                            AsSWSCreature()->m_bDoorSaberAnimationPlayed = 0;
                            AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                        }
                        return ACTION_FAILED;
                    }
                    if (AsSWSCreature() != NULL)
                    {
                        pTarget->AsSWSDoor()->m_bLocked = 0;
                        pTarget->AsSWSDoor()->OpenDoor(GetId());
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_UNLOCK_OBJECT, NULL);
                        if (AsSWSCreature() != NULL && rand() % 100 < 20)
                            AsSWSCreature()->BroadcastVoiceChat(CSWSCreature::VOICE_CHAT_PICK_LOCK_SUCCESS);
                    }
                    else if (AsSWSCreature() != NULL)
                        AsSWSCreature()->BroadcastVoiceChat(CSWSCreature::VOICE_CHAT_PICK_LOCK_FAILED);
                }
                if (AsSWSCreature() != NULL)
                {
                    AsSWSCreature()->StopGuiTimingBar();
                    AsSWSCreature()->m_bDoorSaberAnimationPlayed = 0;
                    AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
                }
                return ACTION_COMPLETE;
            }
        }
    }
    if (AsSWSCreature() != NULL)
    {
        AsSWSCreature()->StopGuiTimingBar();
        AsSWSCreature()->m_bDoorSaberAnimationPlayed = 0;
        AsSWSCreature()->m_bUnlockObjectFacingSet = 0;
    }
    return ACTION_FAILED;
}
// PC 0x005DFD00
int CSWSObject::AIActionUseObject(CSWSObjectActionNode* pNode)
{
    int nDeadTemp = 0;
    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (GetDead() == 1 || GetIsPCDying() == 1 || nDeadTemp == 1)
        return ACTION_FAILED;

    if (AsSWSCreature() != NULL && AsSWSCreature()->GetInParty() &&
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState)
        return ACTION_FAILED;

    unsigned long oidObject = (unsigned long)pNode->m_apParam[0];
    CGameObject* pGameObject;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidObject, &pGameObject) != CGameObjectArray::SUCCESS)
        return ACTION_FAILED;

    CSWSObject* pObject = pGameObject->AsSWSObject();
    int nTargetDeadTemp = 0;
    if (pObject->AsSWSCreature() != NULL)
        nTargetDeadTemp = pObject->AsSWSCreature()->GetDeadTemp();
    if (pObject->GetDead() == 1 || pObject->GetIsPCDying() == 1 ||
        nTargetDeadTemp == 1)
        return ACTION_FAILED;

    if (pObject->GetArea() != NULL && AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = AsSWSCreature();
        pCreature->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);
        pCreature->SetActivity(CSWSCreature::ACTIVITY_INDEX_USE_OBJECT, 0, 0);
        if (!pCreature->GetIsInUseRange(oidObject, 0.0f, 0))
        {
            Vector vPosition;
            float fRange;
            int bUseRange = 1;
            pCreature->GetUseRange(oidObject, vPosition, fRange, 0);
            int bRun = 1;
            AddActionToFront(ACTION_USE_OBJECT, pNode->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidObject,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            float fWait = 0.5f;
            AsSWSCreature()->AddActionToFront(ACTION_WAIT, ACTION_GROUP_LAST,
                ACTION_PARAM_FLOAT, &fWait,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, ACTION_GROUP_LAST,
                ACTION_PARAM_OBJECT, &oidObject,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, pNode->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidObject, ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_FLOAT, &fRange,
                ACTION_PARAM_INTEGER, &bUseRange,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            Vector vOrientation;
            pCreature->AddMoveToPointActionToFront(pNode->m_nGroupId,
                vPosition, m_oidArea, oidObject, bRun, fRange, 0.0f,
                0, 0, 0, 0, 0, 1, vOrientation, 0, 0, 0);
            return ACTION_COMPLETE;
        }
    }

    SetLockOrientationToObject(oidObject);
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
        int bUseable = pPlaceable->m_bUseable;
        if (!bUseable)
            return ACTION_FAILED;

        if (pPlaceable->m_nTrapFlag && AsSWSCreature() != NULL &&
            pPlaceable->GetReputation(GetId(), 0) < MIN_FRIENDLY_REPUTATION)
        {
            int nFaction = pPlaceable->m_nFaction;
            if (nFaction != AsSWSCreature()->GetFactionId())
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidObject, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                return ACTION_FAILED;
            }
        }

        int bHasInventory = pPlaceable->m_bHasInventory;
        if (bHasInventory)
        {
            int bLocked = pPlaceable->m_bLocked;
            if (bLocked)
            {
                if (AsSWSCreature() != NULL &&
                    !AsSWSCreature()->EvaluateLock(pPlaceable, 0))
                {
                    CSWCCMessageData* pData = new CSWCCMessageData;
                    pData->SetObjectID(0, pPlaceable->GetId());
                    AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_LOCKED, pData);
                }
                else
                {
                    CExoString sOpened("");
                    int nSoundAppType;
                    int bSoundAppType;
                    {
                        CExoString sColumn("SoundAppType");
                        unsigned long nAppearance = pPlaceable->m_nAppearance;
                        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
                        bSoundAppType = pPlaceables->GetINTEntry(nAppearance, sColumn, &nSoundAppType);
                    }
                    if (bSoundAppType)
                    {
                        bool bFailed;
                        {
                            CExoString sColumn("Opened");
                            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                            C2DA* pSounds = p2DArrays->m_pPlaceableSounds;
                            bFailed = !pSounds->GetCExoStringEntry(nSoundAppType, sColumn, &sOpened);
                        }
                        if (bFailed)
                            sOpened = "";
                    }
                    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
                    if (pPlayer != NULL)
                    {
                        int bOpen = pPlaceable->m_bOpen;
                        if (!bOpen)
                        {
                            int bOpening = pPlaceable->m_bInventoryOpening;
                            if (bOpening)
                            {
                                pPlaceable->OpenInventory(GetId(), 0);
                                pPlaceable->m_bInventoryOpening = 0;
                                pPlaceable->m_fSyncHeight = 1.0f;
                            }
                            else
                            {
                                AddActionToFront(ACTION_USE_OBJECT, pNode->m_nGroupId,
                                    ACTION_PARAM_OBJECT, &oidObject,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                                float fDelay;
                                CSWCPlaceable* pClientPlaceable = pPlaceable->GetClientPlaceable();
                                if (pClientPlaceable != NULL)
                                {
                                    fDelay = pClientPlaceable->GetAnimationLength(PLACEABLE_ANIMATION_CLIENT_OPEN) * 0.001f;
                                    CResRef cSound(sOpened);
                                    unsigned long nSoundLength = g_pExoSound->Play3DOneShotSound(
                                        cSound, pPlaceable->m_vPosition, 0.0f,
                                        CExoSound::PRIORITY_GROUP_PLACEABLE, 0, 0, 0.0f, 0.0f);
                                    if (nSoundLength * 0.001f > fDelay && fDelay != 0.0f)
                                        fDelay = nSoundLength * 0.001f;
                                }
                                else
                                    fDelay = 0.5f;
                                AddActionToFront(ACTION_WAIT, pNode->m_nGroupId,
                                    ACTION_PARAM_FLOAT, &fDelay,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                                pPlaceable->SetAnimation(PLACEABLE_ANIMATION_OPEN);
                                pPlaceable->m_bInventoryOpening = 1;
                                return ACTION_COMPLETE;
                            }
                        }
                        else
                            return ACTION_COMPLETE;
                    }
                    else
                    {
                        int bOpening = pPlaceable->m_bInventoryOpening;
                        if (bOpening)
                            pPlaceable->m_bInventoryOpening = 0;
                        SetAnimation(PLACEABLE_ANIMATION_OPEN);
                        CResRef cSound(sOpened);
                        g_pExoSound->Play3DOneShotSound(cSound,
                            pPlaceable->m_vPosition, 0.0f, CExoSound::PRIORITY_GROUP_PLACEABLE,
                            0, 0, 0.0f, 0.0f);
                    }
                }
            }
            else
            {
                CExoString sOpened("");
                int nSoundAppType;
                int bSoundAppType;
                {
                    CExoString sColumn("SoundAppType");
                    unsigned long nAppearance = pPlaceable->m_nAppearance;
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    C2DA* pPlaceables = p2DArrays->m_pPlaceables;
                    bSoundAppType = pPlaceables->GetINTEntry(nAppearance, sColumn, &nSoundAppType);
                }
                if (bSoundAppType)
                {
                    bool bFailed;
                    {
                        CExoString sColumn("Opened");
                        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                        C2DA* pSounds = p2DArrays->m_pPlaceableSounds;
                        bFailed = !pSounds->GetCExoStringEntry(nSoundAppType, sColumn, &sOpened);
                    }
                    if (bFailed)
                        sOpened = "";
                }
                CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
                if (pPlayer != NULL)
                {
                    int bOpen = pPlaceable->m_bOpen;
                    if (!bOpen)
                    {
                        CSWCPlaceable* pClientPlaceable = pPlaceable->GetClientPlaceable();
                        if (pPlaceable->m_bInventoryOpening)
                        {
                            pPlaceable->OpenInventory(GetId(), 0);
                            pPlaceable->m_bInventoryOpening = 0;
                            pPlaceable->m_fSyncHeight = 1.0f;
                        }
                        else
                        {
                            AddActionToFront(ACTION_USE_OBJECT, pNode->m_nGroupId,
                                ACTION_PARAM_OBJECT, &oidObject,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                            float fDelay;
                            if (pClientPlaceable != NULL)
                            {
                                fDelay = pClientPlaceable->GetAnimationLength(PLACEABLE_ANIMATION_CLIENT_OPEN) * 0.001f;
                                CResRef cSound(sOpened);
                                unsigned long nSoundLength = g_pExoSound->Play3DOneShotSound(
                                    cSound, pPlaceable->m_vPosition, 0.0f,
                                    CExoSound::PRIORITY_GROUP_PLACEABLE, 0, 0, 0.0f, 0.0f);
                                if (nSoundLength * 0.001f > fDelay && fDelay != 0.0)
                                    fDelay = nSoundLength * 0.001f;
                            }
                            else
                                fDelay = 0.5f;
                            AddActionToFront(ACTION_WAIT, pNode->m_nGroupId,
                                ACTION_PARAM_FLOAT, &fDelay,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                            pPlaceable->SetAnimation(PLACEABLE_ANIMATION_OPEN);
                            pPlaceable->m_bInventoryOpening = 1;
                            return ACTION_COMPLETE;
                        }
                    }
                    else
                        return ACTION_COMPLETE;
                }
                else
                {
                    if (pPlaceable->m_bInventoryOpening)
                        pPlaceable->m_bInventoryOpening = 0;
                    SetAnimation(PLACEABLE_ANIMATION_OPEN);
                    CResRef cSound(sOpened);
                    g_pExoSound->Play3DOneShotSound(cSound,
                        pPlaceable->m_vPosition, 0.0f, CExoSound::PRIORITY_GROUP_PLACEABLE,
                        0, 0, 0.0f, 0.0f);
                }
            }
        }
        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_USED;
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, GetId(), oidObject, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        return ACTION_COMPLETE;
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        CSWSItem* pItem = pObject->AsSWSItem();
        if (g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_bIsContainer)
        {
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
            if (pPlayer != NULL)
            {
                if (pPlayer->m_pContainerGUI->m_oidContainer != oidObject)
                {
                    pItem->OpenInventory(GetId());
                    return ACTION_COMPLETE;
                }
                pItem->CloseInventory(GetId(), 1);
            }
        }
    }
    return ACTION_COMPLETE;
}

inline int CSWSObject::TerminateAISliceAfterAction(unsigned long nAction)
{
    if (nAction == ACTION_JUMP_TO_POINT || nAction == ACTION_JUMP_TO_OBJECT ||
        nAction == ACTION_MOVE_TO_POINT || nAction == ACTION_DRIVE)
    {
        return 1;
    }
    return 0;
}
// PC 0x005E0D90
void CSWSObject::RunActions(unsigned long nDay, unsigned long nTime,
                            unsigned __int64 nTimer)
{
    int nResult = ACTION_FAILED;
    int bDone = 0;
    int nAction = 0;
    int nObjectType = m_nObjectType;

    if (RenderCombat)
    {
        CExoString sActions;
        m_sTag.CStr();

        if (AsSWSCreature() != NULL)
        {

            AsSWSCreature()->m_oidAttackTarget;
            AsSWSCreature()->m_oidAttemptedAttackTarget;
            AsSWSCreature()->m_oidAttemptedMovementTarget;
            AsSWSCreature()->m_oidSpellTarget;
            AsSWSCreature()->m_nCombatMode;
            AsSWSCreature()->m_nLastAttackAction;
            AsSWSCreature()->m_nAnimation;
            AsSWSCreature()->m_nAnimationState;
            if (AsSWSCreature()->GetCombatRound()->m_bRoundPaused)
                AsSWSCreature()->GetCombatRound()->m_nPauseTimer;
            AsSWSCreature()->GetCombatRound()->m_nTimer;

        }
        if (!m_lstActions.IsEmpty())
        {
            CExoLinkedListNode* pPosition = m_lstActions.GetHeadPos();
            sActions = " ";
            while (pPosition != NULL)
            {
                CExoString sAction;
                CSWSObjectActionNode* pNode = m_lstActions.GetAtPos(pPosition);
                sAction.Format(" %d(%d)", pNode->m_nActionType, pNode->m_nGroupId);
                sActions = sActions + sAction;
                m_lstActions.GetNext(pPosition);
            }
            sActions.CStr();

        }
    }
    if (!m_lstActions.IsEmpty())
    {
        unsigned long oidSelf = GetId();
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        unsigned long oidLeader = pClient->GetSWParty()->m_aPartyMemberData[0].m_oidCreature;
        if (pClient->ServerToClientObjectId(oidSelf) == oidLeader &&
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode ==
                CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
        {
            g_bShowIndicator = 1;
            g_pAppManager->GetClientExoApp()->GetModule()->RestoreCamera();
            g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME, 1);
        }
    }
    do
    {
        if (!m_lstActions.IsEmpty())
        {
            CSWSObjectActionNode* pNode = m_lstActions.RemoveHead();
            if (m_nActionStartDay == 0 && m_nActionStartTime == 0)
            {
                m_nActionStartDay = nDay;
                m_nActionStartTime = nTime;
            }
            m_pCurrentActionNode = pNode;
            nAction = pNode->m_nActionType;
            m_nCurrentAction = pNode->m_nActionType;
            nResult = ACTION_FAILED;
            switch (nAction)
            {
            case ACTION_MOVE_TO_POINT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionMoveToPoint(pNode);
                break;
            case ACTION_FOLLOW_LEADER:
                if (g_pAppManager->GetClientExoApp()->GetPlayerCreatureId() == OBJECT_INVALID)
                    nResult = ACTION_IN_PROGRESS;
                else if (AsSWSCreature() != NULL && AsSWSCreature()->GetIsPCDying())
                    nResult = ACTION_IN_PROGRESS;
                else if (nObjectType == SW_OBJECT_TYPE_CREATURE &&
                         AsSWSCreature() != NULL && AsSWSCreature()->GetInParty())
                    nResult = AsSWSCreature()->AIActionPartyFollowLeader(pNode);
                break;
            case ACTION_FOLLOW_OWNER:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE &&
                    AsSWSCreature() != NULL && AsSWSCreature()->m_n11FC)
                    nResult = AsSWSCreature()->AIActionFollowOwner(pNode);
                break;
            case ACTION_RANDOM_WALK:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionRandomWalk(pNode);
                break;
            case ACTION_FORCE_FOLLOW_OBJECT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionForceFollowObject(pNode);
                break;
            case ACTION_CHECK_FORCE_FOLLOW_OBJECT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCheckForceFollowObject(pNode);
                break;
            case ACTION_PLAY_SOUND:
                nResult = AIActionPlaySound(pNode);
                break;
            case ACTION_CAST_SPELL:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                {
                    nResult = AsSWSCreature()->AIActionCastSpell(pNode);
                    if (nResult == ACTION_FAILED || nResult == ACTION_COMPLETE)
                        AsSWSCreature()->m_pcCombatRound->GetSpellActionPending();
                }
                else if (nObjectType == SW_OBJECT_TYPE_PLACEABLE)
                    nResult = AsSWSPlaceable()->AIActionCastSpell(pNode);
                break;
            case ACTION_JUMP_TO_POINT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionJumpToPoint(pNode);
                break;
            case ACTION_JUMP_TO_OBJECT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionJumpToObject(pNode);
                break;
            case ACTION_CHECK_MOVE_TO_OBJECT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCheckMoveToObject(pNode);
                break;
            case ACTION_CHECK_MOVE_TO_OBJECT_RADIUS:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCheckMoveToObjectRadius(pNode);
                break;
            case ACTION_ITEM_CAST_SPELL:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                {
                    nResult = AsSWSCreature()->AIActionItemCastSpell(pNode);
                    if (nResult != ACTION_IN_PROGRESS)
                    {
                        AsSWSCreature()->m_oidAttemptedSpellTarget = OBJECT_INVALID;
                        AsSWSCreature()->m_oidSpellTarget = OBJECT_INVALID;
                    }
                }
                break;
            case ACTION_SET_COMMANDABLE:
                nResult = AIActionSetCommandable(pNode);
                break;
            case ACTION_CHECK_MOVE_AWAY_FROM_OBJECT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCheckMoveAwayFromObject(pNode);
                break;
            case ACTION_CHECK_MOVE_AWAY_FROM_LOCATION:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCheckMoveAwayFromLocation(pNode);
                break;
            case ACTION_CHECK_INTER_AREA_PATHFINDING:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCheckInterAreaPathfinding(pNode);
                break;
            case ACTION_CHANGE_FACING_POINT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionChangeFacingPoint(pNode);
                break;
            case ACTION_CHANGE_FACING_OBJECT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionChangeFacingObject(pNode);
                break;
            case ACTION_ORIENT_CAMERA:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionOrientCamera(pNode);
                break;
            case ACTION_PLAY_ANIMATION:
                nResult = AIActionPlayAnimation(pNode);
                break;
            case ACTION_PICK_UP_ITEM:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionPickUpItem(pNode);
                break;
            case ACTION_GIVE_ITEM:
                nResult = AIActionGiveItem(pNode);
                break;
            case ACTION_TAKE_ITEM:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE ||
                    nObjectType == SW_OBJECT_TYPE_PLACEABLE)
                    nResult = AIActionTakeItem(pNode);
                break;
            case ACTION_EQUIP_ITEM:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionEquipItem(pNode);
                break;
            case ACTION_DROP_ITEM:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionDropItem(pNode);
                break;
            case ACTION_UNEQUIP_ITEM:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionUnequipItem(pNode);
                break;
            case ACTION_CHECK_MOVE_TO_POINT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCheckMoveToPoint(pNode);
                break;
            case ACTION_CHECK_MOVE_TO_POINT_RADIUS:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCheckMoveToPointRadius(pNode);
                break;
            case ACTION_CHECK_MOVE_TO_FOLLOW_RADIUS:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE &&
                    AsSWSCreature() != NULL && AsSWSCreature()->GetInParty())
                    nResult = AsSWSCreature()->AIActionCheckMoveToFollowRadius(pNode);
                break;
            case ACTION_PHYSICAL_ATTACKS:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                {
                    m_nUnknown84 = (unsigned long)pNode->m_apParam[0];
                    nResult = AsSWSCreature()->AIActionPhysicalAttacks(pNode);
                    m_nUnknown84 = OBJECT_INVALID;
                    if (nResult != ACTION_IN_PROGRESS)
                    {
                        unsigned long nCurrentDay, nCurrentTime;
                        g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
                            &nCurrentDay, &nCurrentTime);
                    }
                }
                break;
            case ACTION_COMBAT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCombat(pNode);
                break;
            case ACTION_SPEAK_STR_REF:
                nResult = AIActionSpeakStrRef(pNode);
                break;
            case ACTION_SPEAK_STRING:
                nResult = AIActionSpeak(pNode);
                break;
            case ACTION_WAIT_FOR_END_OF_ROUND:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionWaitForEndOfRound(pNode);
                break;
            case ACTION_CLOSE_DOOR:
                nResult = AIActionCloseDoor(pNode);
                break;
            case ACTION_OPEN_DOOR:
                nResult = AIActionOpenDoor(pNode);
                break;
            case ACTION_DIALOG_OBJECT:
                nResult = AIActionDialogObject(pNode);
                break;
            case ACTION_PAUSE_DIALOG:
                nResult = AIActionPauseDialog(pNode);
                break;
            case ACTION_RESUME_DIALOG:
                nResult = AIActionResumeDialog(pNode);
                break;
            case ACTION_BARK_STRING:
                nResult = AIActionBarkString(pNode);
                break;
            case ACTION_FLAG_MINE:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionFlagMine(pNode);
                break;
            case ACTION_DISARM_MINE:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionDisarmMine(pNode);
                break;
            case ACTION_RECOVER_MINE:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionRecoverMine(pNode);
                break;
            case ACTION_EXAMINE_MINE:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionExamineMine(pNode);
                break;
            case ACTION_WAIT:
                nResult = AIActionWait(pNode);
                break;
            case ACTION_SET_MINE:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionSetMine(pNode);
                break;
            case ACTION_ENCOUNTER_CREATURE_DESTROY_SELF:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionEncounterCreatureDestroySelf();
                break;
            case ACTION_DO_COMMAND:
                nResult = AIActionDoCommand(pNode);
                break;
            case ACTION_UNLOCK_OBJECT:
                nResult = AIActionUnlockObject(pNode);
                break;
            case ACTION_LOCK_OBJECT:
                nResult = AIActionLockObject(pNode);
                break;
            case ACTION_USE_OBJECT:
                nResult = AIActionUseObject(pNode);
                break;
            case ACTION_ANIMAL_EMPATHY:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionAnimalEmpathy(pNode);
                break;
            case ACTION_REST:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionRest(pNode);
                break;
            case ACTION_TAUNT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionTaunt(pNode);
                break;
            case ACTION_COUNTER_SPELL:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionCounterSpell(pNode);
                break;
            case ACTION_DRIVE:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionDrive(pNode);
                break;
            case ACTION_APPEAR:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionAppear(pNode);
                break;
            case ACTION_DISAPPEAR:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionDisappear(pNode);
                break;
            case ACTION_PICK_POCKET:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionPickPocket(pNode);
                break;
            case ACTION_HEAL:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionHeal(pNode);
                break;
            case ACTION_AREA_WAIT:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionAreaWait(pNode);
                break;
            case ACTION_SURRENDER_TO_ENEMIES:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionSurrenderToEnemies(pNode);
                break;
            case ACTION_DOOR_SABER:
                nResult = AIActionDoorSaber(pNode);
                break;
            case ACTION_SWITCH_WEAPONS:
                nResult = AIActionSwitchWeapons(pNode);
                break;
            case ACTION_STANCE_TOTAL_DEFENSE:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionStanceTotalDefense(pNode);
                break;
            case ACTION_STANCE_MEDITATIVE:
                if (nObjectType == SW_OBJECT_TYPE_CREATURE)
                    nResult = AsSWSCreature()->AIActionStanceMeditative(pNode);
                break;
#ifdef _XBOX
            default:
                EXOXBOXASSERT(0);
                break;
#endif
            }
            if (RenderCombat && AsSWSCreature() != NULL && AsSWSCreature()->GetInParty())
            {
                AsSWSCreature()->GetCombatRound()->DEBUGDumpCombatRoundToScreen(
                    10, AsSWSCreature()->GetTag());
                unsigned long oidTarget = AsSWSCreature()->m_oidAttemptedAttackTarget;
                if (oidTarget != OBJECT_INVALID)
                {
                    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
                    if (pTarget != NULL && pTarget->AsSWSCreature() != NULL)
                    {
                        CSWSCreature* pCreature = pTarget->AsSWSCreature();
                        pCreature->m_pcCombatRound->DEBUGDumpCombatRoundToScreen(30, pCreature->m_sTag);
                    }
                }
            }
            m_pCurrentActionNode = NULL;
            {
                CServerAIMaster* pAIMaster = g_pAppManager->GetServerExoApp()->GetServerAIMaster();
                pAIMaster->m_nLastEvent[CServerAIMaster::LAST_EVENT_ACTION] = m_nCurrentAction;
            }
            m_nCurrentAction = ACTION_TYPE_INVALID;
            m_nLastUpdateDay = nDay;
            m_nLastUpdateTime = nTime;
            if (nResult == ACTION_IN_DELAY)
                m_lstActions.AddTail(pNode);
            else if (nResult == ACTION_IN_PROGRESS)
                m_lstActions.AddHead(pNode);
            else
            {
                if ((unsigned long)GetLockOrientationToObject() != OBJECT_INVALID &&
                    nAction != ACTION_PHYSICAL_ATTACKS)
                {
                    GetLockOrientationToObject();
                    SetLockOrientationToObject(OBJECT_INVALID);
                }
                delete pNode;
                m_nActionStartDay = 0;
                m_nActionStartTime = 0;
            }
        }
        else if (AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = AsSWSCreature();
            if (pCreature->m_oidAttemptedAttackTarget != OBJECT_INVALID)
                pCreature->SetAttemptedAttackTarget(OBJECT_INVALID);
            if (pCreature->m_oidAttackTarget != OBJECT_INVALID)
                pCreature->m_oidAttackTarget = OBJECT_INVALID;
            if (pCreature->m_oidAttemptedSpellTarget != OBJECT_INVALID)
                pCreature->m_oidAttemptedSpellTarget = OBJECT_INVALID;
            if (pCreature->m_oidSpellTarget != OBJECT_INVALID)
                pCreature->m_oidSpellTarget = OBJECT_INVALID;
            AsSWSCreature()->m_oidAttemptedMovementTarget = OBJECT_INVALID;
            CPathfindInformation* pInfo = AsSWSCreature()->m_pPathfindInformation;
            pInfo->m_oidBlockingCreature = OBJECT_INVALID;
            pInfo->m_vBlockingCreaturePosition = Vector(0.0f, 0.0f, 0.0f);
        }
        if (nResult == ACTION_IN_PROGRESS ||
            g_pExoBase->m_pExoTimers->GetHighResolutionTimer() - nTimer > AI_TIME_SLICE ||
            TerminateAISliceAfterAction(nAction) == 1)
            bDone = 1;
    } while (!m_lstActions.IsEmpty() && !bDone);

    if ((unsigned long)m_lstActions.Count() > ACTION_QUEUE_WARNING_LIMIT)
    {
        ReportOverflow(CSWSCreature::FEEDBACK_ACTION_LIST_OVERFLOW, m_lstActions.Count(),
                       ACTION_QUEUE_WARNING_LIMIT, 0);
        if ((unsigned long)m_lstActions.Count() > ACTION_QUEUE_CLEAR_LIMIT)
        {
            ReportOverflow(CSWSCreature::FEEDBACK_ACTION_LIST_ERASED, m_lstActions.Count(),
                           ACTION_QUEUE_CLEAR_LIMIT, 1);
            int bClear = 1;
            while (!m_lstActions.IsEmpty() && bClear == 1)
            {
                CSWSObjectActionNode* pNode = m_lstActions.RemoveHead();
                if (ClearAction(pNode, 1) == 1)
                    delete pNode;
                else
                {
                    bClear = 0;
                    m_lstActions.AddHead(pNode);
                }
            }
            if ((unsigned long)m_lstActions.Count() > ACTION_QUEUE_DELETE_LIMIT)
            {
                while (!m_lstActions.IsEmpty())
                    delete m_lstActions.RemoveHead();
            }
        }
    }
    if (m_lstEffects.num > EFFECT_WARNING_LIMIT)
    {
        ReportOverflow(CSWSCreature::FEEDBACK_EFFECT_LIST_OVERFLOW, m_lstEffects.num, EFFECT_WARNING_LIMIT, 0);
        if (m_lstEffects.num > EFFECT_REMOVE_LIMIT)
        {
            ReportOverflow(CSWSCreature::FEEDBACK_EFFECT_LIST_ERASED, m_lstEffects.num, EFFECT_REMOVE_LIMIT, 1);
            RemoveSomeEffectsOfDurationType(CGameEffect::DURATION_TYPE_TEMPORARY);
            RemoveSomeEffectsOfDurationType(CGameEffect::DURATION_TYPE_PERMANENT);
        }
    }
}

static inline CServerExoApp* GetServerExoApp()
{
    return g_pAppManager->m_pServerExoApp;
}

#ifdef _XBOX

int CSWSObject::GetReputation(unsigned long oidObject, int bUnknown)
{
    if (OverrideReputation != REPUTATION_NO_OVERRIDE)
    {
        return OverrideReputation;
    }

    CGameObject* pObject;
    CGameObject* pCreator;
    int nTargetFaction;
    int nSourceFaction;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    int nReputation = REPUTATION_MIN;
    if (pServer->GetObjectArray()->GetGameObject(
            oidObject, &pObject) != CGameObjectArray::SUCCESS)
    {
        return REPUTATION_DEFAULT;
    }
    {
        CFactionManager* pFactionManager =
            g_pAppManager->m_pServerExoApp->m_pcExoAppInternal->m_pFactionManager;
        if (pFactionManager != NULL)
        {
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT &&
                pObject->AsSWSAreaOfEffectObject() != NULL)
            {
                oidObject = pObject->AsSWSAreaOfEffectObject()->m_oidCreator;
            }

            if (AsSWSAreaOfEffectObject() != NULL)
            {
                unsigned long oidCreator = AsSWSAreaOfEffectObject()->m_oidCreator;
                pServer = g_pAppManager->m_pServerExoApp;
                if (pServer->GetObjectArray()->GetGameObject(
                        oidCreator, &pCreator) == CGameObjectArray::SUCCESS)
                {
                    return pCreator->AsSWSObject()->GetReputation(oidObject, bUnknown);
                }
            }
            else
            {
                pServer = g_pAppManager->m_pServerExoApp;
                if (pServer->GetFactionOfObject(
                        oidObject, &nTargetFaction))
                {
                    unsigned long oidSource = m_nId;
                    pServer = g_pAppManager->m_pServerExoApp;
                    if (pServer->GetFactionOfObject(oidSource, &nSourceFaction))
                    {
                        if (oidObject == m_nId)
                        {
                            return REPUTATION_MAX;
                        }
                        if (pFactionManager->GetIsNPCFaction(nTargetFaction) == 0 &&
                            pFactionManager->GetIsNPCFaction(nSourceFaction) == 1)
                        {
                            unsigned long oidSelf = m_nId;
                            return pObject->AsSWSObject()->GetReputation(oidSelf, 0);
                        }
                        if (AsSWSCreature() != NULL)
                        {
                            nReputation = AsSWSCreature()->GetCreatureReputation(
                                oidObject, nTargetFaction);
                        }
                        else if (AsSWSPlaceable() != NULL || AsSWSEncounter() != NULL ||
                                 AsSWSDoor() != NULL || AsSWSTrigger() != NULL)
                        {
                            nReputation = pFactionManager->GetNPCFactionReputation(
                                nTargetFaction, nSourceFaction);
                        }
                        if (nReputation < REPUTATION_MIN)
                        {
                            nReputation = REPUTATION_MIN;
                        }
                        else if (nReputation > REPUTATION_MAX)
                        {
                            nReputation = REPUTATION_MAX;
                        }
                        return nReputation;
                    }
                }
            }
        }
    }
    return REPUTATION_DEFAULT;
}

int CSWSObject::AIActionOpenDoor(CSWSObjectActionNode* pNode)
{
    int nDeadTemp = 0;
    if (AsSWSCreature() != NULL)
        nDeadTemp = AsSWSCreature()->GetDeadTemp();

    if (GetDead() != 1 && GetIsPCDying() != 1 && nDeadTemp != 1)
    {
        if (AsSWSCreature() != NULL)
            AsSWSCreature()->ActivityManager(CSWSCreature::ACTIVITY_ACTION_OBJECT_INTERACTION);

        int bRun = 1;
        unsigned long oidDoor = (unsigned long)pNode->m_apParam[0];
        if (pNode->m_nNumParams > 1 && pNode->m_apParam[1] != NULL)
            bRun = (int)pNode->m_apParam[1];

        CGameObject* pObject;
        Vector vPosition;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidDoor, &pObject) != CGameObjectArray::SUCCESS)
        {
            m_bOpenDoorStarted = 0;
            return ACTION_FAILED;
        }
        {
            if (AsSWSCreature() != NULL)
            {
                CSWSDoor* pDoor = pObject->AsSWSDoor();
                if (pDoor == NULL)
                {
                    m_bOpenDoorStarted = 0;
                    return ACTION_FAILED;
                }
                CSWSArea* pArea = ((CSWSObject*)pObject)->GetArea();
                float fRange;
                AsSWSCreature()->GetUseRange(oidDoor, vPosition, fRange, 0);
                if (pDoor->m_nAnimation != DOOR_ANIMATION_CLOSED)
                    return ACTION_COMPLETE;

                if (GetArea() != pObject->AsSWSObject()->GetArea() ||
                    !AsSWSCreature()->GetIsInUseRange(oidDoor, 0.0f, 0))
                {
                    AsSWSCreature()->AddActionToFront(
                        ACTION_OPEN_DOOR, ACTION_GROUP_NEW,
                        ACTION_PARAM_OBJECT, &oidDoor, ACTION_PARAM_INTEGER, &bRun,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    if (pDoor->m_bPreciseUse && pDoor->m_bLocked)
                    {
                        float fWait = 0.5f;
                        AsSWSCreature()->AddActionToFront(
                            ACTION_WAIT, ACTION_GROUP_LAST, ACTION_PARAM_FLOAT, &fWait,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        AsSWSCreature()->AddActionToFront(
                            ACTION_CHANGE_FACING_OBJECT, ACTION_GROUP_LAST,
                            ACTION_PARAM_OBJECT, &oidDoor,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    }
                    Vector vOrientation;
                    AsSWSCreature()->AddMoveToPointActionToFront(
                        ACTION_GROUP_LAST, vPosition,
                        pObject->AsSWSObject()->m_oidArea, OBJECT_INVALID,
                        bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0, vOrientation, 0, 0, 0);
                    m_bOpenDoorStarted = 1;
                    return ACTION_COMPLETE;
                }
                else
                {
                    if (pDoor->m_bTrapFlag &&
                        pDoor->GetReputation(GetId(), 0) < MIN_FRIENDLY_REPUTATION)
                    {
                        int nDoorFaction = pDoor->m_nFactionId;
                        if (nDoorFaction != AsSWSCreature()->GetFactionId())
                        {
                            CScriptEvent* pEvent = new CScriptEvent;
                            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                            unsigned long oidSelf = m_nId;
                            GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                                0, 0, oidSelf, oidDoor, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                            return ACTION_FAILED;
                        }
                    }
                    if (!AsSWSCreature()->EvaluateLock(pObject->AsSWSObject(), 0))
                    {
                        CScriptEvent* pEvent = new CScriptEvent;
                        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN;
                        unsigned long oidSelf = m_nId;
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, oidSelf, oidDoor, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                        m_bOpenDoorStarted = 0;
                        return ACTION_FAILED;
                    }
                    SetLockOrientationToObject(oidDoor);
                    if (!m_bOpenDoorStarted)
                    {
                        AsSWSCreature()->AddActionToFront(
                            ACTION_OPEN_DOOR, ACTION_GROUP_NEW,
                            ACTION_PARAM_OBJECT, &oidDoor, ACTION_PARAM_INTEGER, &bRun,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                        m_bOpenDoorStarted = 1;
                        return ACTION_COMPLETE;
                    }
                }
            }
            m_bOpenDoorStarted = 0;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, m_nId, oidDoor, CServerAIMaster::EVENT_OPEN_OBJECT, NULL);
            float fDelay = 0.5f;
            AddActionToFront(ACTION_WAIT, pNode->m_nGroupId,
                ACTION_PARAM_FLOAT, &fDelay,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            return ACTION_COMPLETE;
        }
    }
    m_bOpenDoorStarted = 0;
    return ACTION_FAILED;
}

int CSWSObject::AddGiveItemActions(unsigned long oidItem, unsigned long oidTarget,
                                  int nQuantity, int bAddToQueue)
{
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pGameObject == NULL)
    {
        return 0;
    }
    if (pGameObject->AsSWSObject() == NULL)
    {
        return 0;
    }

    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        CSWSCreature* pCreature = AsSWSCreature();
        unsigned long oidPossessor = pItem->m_oidPossessor;
        CSWSCreature* pPossessor =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPossessor);
        if ((pPossessor->m_bInParty == 0 || pCreature->m_bInParty == 0) &&
            pItem->m_oidPossessor != GetId())
        {
            return 0;
        }

        if (m_bCommandable != 0)
        {
            int nStackSize = pItem->m_nStackSize;
            if (nQuantity > nStackSize)
            {
                nQuantity = nStackSize;
            }
            AddAction(ACTION_GIVE_ITEM, ACTION_GROUP_LAST,
                      ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidTarget,
                      ACTION_PARAM_INTEGER, &nQuantity, ACTION_PARAM_INTEGER, &bAddToQueue,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif
