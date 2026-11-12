#include "nwsobject.h"
#include "clientexoapp.h"
#include "gameobjectarray.h"
#include "nwsarea.h"
#include "nwsencounter.h"
#include "serveraimaster.h"
#include "nwscombatround.h"
#include "nwitem.h"
#include "nwsitem.h"
#include "nwsdoor.h"
#include "nwsplaceable.h"
#include "pathfindinfo.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "serverexoappinternal.h"
#include "nwmessage.h"
#include "factionmanager.h"
#include "nwscreaturestats.h"
#include "nwclass.h"
#include "nwfeat.h"
#include "nwitem.h"
#include "worldtimer.h"
#include "swparty.h"
#include "nwccreature.h"
#include "nwsscriptevent.h"
#include "nwsplayer.h"
#include "nwsparty.h"
#include "nwseffect.h"
#include "nwsspellimpact.h"
#include "nwsmodule.h"
#include "nwrules.h"
#include "serverinfo.h"
#include "nwfeat.h"
#include "nwstrigger.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "swmain.h"
#include "nwsforcedaction.h"
#include "nwscript.h"
#include "guiingame.h"
#include <math.h>
#include "nwspell.h"
#include "nwrace.h"

static inline void NwscreatureaiTrace1();

unsigned long AiTimeSlice = CSWSObject::AI_TIME_SLICE;
float GrenadeThrowThreshold = 10.0f;
int bNoForceMoves = 0;
int g_bRunSubAreaScripts = 1;

int ShowFailedPathfinds = 0;
int NPCTurbo = 0;
int FollowLeaderFullLogging = 0;
int RenderPartyFollow = 0;
int RenderParallelograms = 0;
float fSpringFactor = 0.5f;

int CreaturePartyIndex;
char sRet[255] = "                                                  ";

static Vector* AllocateVectorArray(int nCount)
{
    return new Vector[nCount];
}
// PC 0x00542060
int CSWSCreature::AIActionFlagMine(CSWSObjectActionNode* pAction)
{
    Vector vPosition;
    float fRange;
    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
    {
        StopGuiTimingBar();
        m_bMineActionStarted = FALSE;
        return ACTION_FAILED;
    }
    if (!GetIsInUseRange(oidTarget, 0.0f, FALSE))
    {
        GetUseRange(oidTarget, vPosition, fRange, FALSE);
        AddActionToFront(ACTION_FLAG_MINE, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        Vector vOrientation;
        AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition,
            m_oidArea, OBJECT_INVALID, TRUE, fRange, 0.0f, 0, 0, 0, 0, 0, 0,
            vOrientation, 0, 0, 0);
        return ACTION_COMPLETE;
    }
    if (!m_bMineActionStarted)
    {
        m_bMineActionStarted = TRUE;
        int nAnimation;
        if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
            pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
            nAnimation = CREATURE_ANIMATION_LOOPING_GET_MID;
        else
            nAnimation = CREATURE_ANIMATION_LOOPING_GET_LOW;
        int bLooping = TRUE;
        float fSpeed = 1.0f;
        float fDuration = 4.5f;
        AddActionToFront(ACTION_FLAG_MINE, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_PLAY_ANIMATION, pAction->m_nGroupId,
            ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
            ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLooping,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        StartGuiTimingBar(FLAG_MINE_DURATION, TRUE);
        return ACTION_COMPLETE;
    }

    int nSkillRank = m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, FALSE);
    int bOwnMine = FALSE;
    int bTakeTwenty = FALSE;
    int nDC;
    int nRoll = 0;
    if (m_bCombatState)
        nRoll = rand() % 20 + 1;
    else
    {
        nRoll = 20;
        bTakeTwenty = TRUE;
    }
    if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        if (pTarget->AsSWSTrigger()->GetCreator() == m_nId)
        {
            pTarget->AsSWSTrigger()->m_bTrapFlagged = TRUE;
            bOwnMine = TRUE;
        }
        else
        {
            nDC = pTarget->AsSWSTrigger()->GetDisarmDC() - FLAG_MINE_DC_REDUCTION < 1
                ? 1 : pTarget->AsSWSTrigger()->GetDisarmDC() - FLAG_MINE_DC_REDUCTION;
            if (nSkillRank + nRoll >= nDC)
                pTarget->AsSWSTrigger()->m_bTrapFlagged = TRUE;
        }
    }
    else if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        nDC = pTarget->AsSWSDoor()->m_nDisarmDC - FLAG_MINE_DC_REDUCTION < 1
            ? 1 : pTarget->AsSWSDoor()->m_nDisarmDC - FLAG_MINE_DC_REDUCTION;
        if (nSkillRank + nRoll >= nDC)
            pTarget->AsSWSDoor()->m_bTrapFlagged = TRUE;
    }
    else if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        nDC = pTarget->AsSWSPlaceable()->m_nDisarmDC - FLAG_MINE_DC_REDUCTION < 1
            ? 1 : pTarget->AsSWSPlaceable()->m_nDisarmDC - FLAG_MINE_DC_REDUCTION;
        if (nSkillRank + nRoll >= nDC)
            pTarget->AsSWSPlaceable()->m_bTrapFlagged = TRUE;
    }

    CSWCCMessageData* pData = new CSWCCMessageData;
    pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, m_nId);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, 0);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, nRoll);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, nSkillRank);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, nDC);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, bTakeTwenty);
    if (bOwnMine == TRUE)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_OWN_MINE);
    else if (nSkillRank + nRoll >= nDC)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_SUCCESS);
    else if (bTakeTwenty)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_RETRY);
    else
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_FAILED);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_DEMOLITIONS_STRREF);
    BroadcastSkillData(pData);
    StopGuiTimingBar();
    m_bMineActionStarted = FALSE;
    return ACTION_COMPLETE;
}
// PC 0x00542570
int CSWSCreature::AIActionExamineMine(CSWSObjectActionNode* pAction)
{
    Vector vTarget;
    float fRange;
    unsigned long oidTarget;

    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);
    oidTarget = (unsigned long)pAction->m_apParam[0];
    CGameObject* pTarget =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
    {
        StopGuiTimingBar();
        m_bMineActionStarted = FALSE;
        return ACTION_FAILED;
    }

    if (!GetIsInUseRange(oidTarget, 0.0f, FALSE))
    {
        GetUseRange(oidTarget, vTarget, fRange, FALSE);
        AddActionToFront(ACTION_EXAMINE_MINE, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddMoveToPointActionToFront(pAction->m_nGroupId, vTarget, m_oidArea,
            OBJECT_INVALID, TRUE, fRange, 0.0f, 0, 0, 0, 0, 0, 0,
            Vector(), 0, 0, 0);
    }
    else
    {
        if (!m_bMineActionStarted)
        {
            m_bMineActionStarted = TRUE;
            int nAnimation;
            if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
                pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                nAnimation = CREATURE_ANIMATION_LOOPING_GET_MID;
            else
                nAnimation = CREATURE_ANIMATION_LOOPING_GET_LOW;
            int bLooping = TRUE;
            float fSpeed = 1.0f;
            float fDuration = 4.5f;
            AddActionToFront(ACTION_EXAMINE_MINE, pAction->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            AddActionToFront(ACTION_PLAY_ANIMATION, pAction->m_nGroupId,
                ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
                ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLooping,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL);
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            StartGuiTimingBar(EXAMINE_MINE_DURATION, TIMING_BAR_EXAMINE_MINE);
        }
        else
        {
            int nRoll;
            int nDC;
            char nRank = GetStats()->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, FALSE);
            int nSkill = nRank;
            int bTakeTwenty = FALSE;
            int bRecoverable = FALSE;
            int bOwnMine = FALSE;
            if (m_bCombatState)
                nRoll = rand() % 20 + 1;
            else
            {
                nRoll = 20;
                bTakeTwenty = TRUE;
            }

            if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
            {
                if (pTarget->AsSWSTrigger()->GetCreator() == m_nId)
                {
                    bRecoverable = TRUE;
                    bOwnMine = TRUE;
                }
                else
                {
                    nDC = pTarget->AsSWSTrigger()->GetDisarmDC() - EXAMINE_MINE_DC_ADJUSTMENT < 1
                        ? 1 : pTarget->AsSWSTrigger()->GetDisarmDC() - EXAMINE_MINE_DC_ADJUSTMENT;
                    if (nRoll + nSkill >= nDC)
                        bRecoverable = TRUE;
                }
            }
            else if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
            {
                nDC = pTarget->AsSWSDoor()->m_nDisarmDC - EXAMINE_MINE_DC_ADJUSTMENT < 1
                    ? 1 : pTarget->AsSWSDoor()->m_nDisarmDC - EXAMINE_MINE_DC_ADJUSTMENT;
                if (nRoll + nSkill >= nDC)
                    bRecoverable = TRUE;
            }
            else if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
            {
                nDC = pTarget->AsSWSPlaceable()->m_nDisarmDC - EXAMINE_MINE_DC_ADJUSTMENT < 1
                    ? 1 : pTarget->AsSWSPlaceable()->m_nDisarmDC - EXAMINE_MINE_DC_ADJUSTMENT;
                if (nRoll + nSkill >= nDC)
                    bRecoverable = TRUE;
            }

            CSWCCMessageData* pData = new CSWCCMessageData;
            pData->SetObjectID(0, GetId());
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME,
                CSWCCMessageData::SKILL_DATA_EXAMINE_STRREF);
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, nRoll);
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, nSkill);
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, nDC);
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, bTakeTwenty);
            if (bOwnMine == TRUE)
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT,
                    CSWCCMessageData::SKILL_RESULT_OWN_MINE);
            else if (nRoll + nSkill >= nDC)
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT,
                    CSWCCMessageData::SKILL_RESULT_SUCCESS);
            else if (bTakeTwenty)
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT,
                    CSWCCMessageData::SKILL_RESULT_RETRY);
            else
                pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT,
                    CSWCCMessageData::SKILL_RESULT_FAILED);
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION,
                CSWCCMessageData::SKILL_DATA_DEMOLITIONS_STRREF);
            BroadcastSkillData(pData);
            CSWSPlayer* pPlayer =
                g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
            if (pPlayer != NULL)
                g_pAppManager->GetServerExoApp()->GetSWSMessage()->
                    SendServerToPlayerExamineGui_MineData(pPlayer, oidTarget,
                        this, bRecoverable);
            StopGuiTimingBar();
            m_bMineActionStarted = FALSE;
        }
    }

    return ACTION_COMPLETE;
}
// PC 0x00542AB0
void CSWSCreature::SetDetectMode(unsigned char nDetectMode)
{
    if (nDetectMode != m_nDetectMode)
    {
        m_nDetectMode = nDetectMode;
        ComputeModifiedMovementRate();
    }
}
// PC 0x00542AD0
void CSWSCreature::SetStealthMode(unsigned char nStealthMode, int nValue)
{
    if (nStealthMode != m_nStealthMode)
    {
        m_nStealthMode = nStealthMode;
        m_nStealthModeValue = nValue;
        ComputeModifiedMovementRate();
    }
}
// PC 0x00542B00
void CSWSCreature::SetCombatMode(unsigned char nMode, int bUpdate)
{
    unsigned char nOldMode;
    if (bUpdate == TRUE)
    {
        nOldMode = m_nActiveCombatMode;
        if (nMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS)
        {
            if (m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) != NULL)
            {
                SendFeedbackMessage(FEEDBACK_COMBAT_MODE_WEAPON_INVALID, NULL);
                return;
            }
        }
        else if (nMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT)
        {
            CSWSItem* pWeapon = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
            if (pWeapon == NULL)
            {
                SendFeedbackMessage(FEEDBACK_COMBAT_MODE_WEAPON_INVALID, NULL);
                return;
            }
            if (g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nRangedWeapon <= 0)
            {
                SendFeedbackMessage(FEEDBACK_COMBAT_MODE_WEAPON_INVALID, NULL);
                return;
            }
        }
        else if (nMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK ||
                 nMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK ||
                 nMode == SWSOBJECT_COMBAT_MODE_PARRY)
        {
            CSWSItem* pWeapon = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
            if (pWeapon != NULL &&
                g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nRangedWeapon > 0)
            {
                SendFeedbackMessage(FEEDBACK_COMBAT_MODE_WEAPON_INVALID, NULL);
                return;
            }
        }
        m_nActiveCombatMode = nMode;
    }
    else if (nMode == SWSOBJECT_COMBAT_MODE_COUNTERSPELL &&
             m_nActiveCombatMode != nMode)
    {
        nOldMode = m_nActiveCombatMode;
        m_nActiveCombatMode = nMode;
    }
    else
    {
        if (nMode == m_nActiveCombatMode)
            return;
        if (m_pcCombatRound->m_bRoundStarted == TRUE)
        {
            m_nDesiredCombatMode = nMode;
            return;
        }
        if (nMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS)
        {
            if (m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) != NULL)
            {
                SendFeedbackMessage(FEEDBACK_COMBAT_MODE_WEAPON_INVALID, NULL);
                return;
            }
        }
        else if (nMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT)
        {
            CSWSItem* pWeapon = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
            if (pWeapon == NULL)
            {
                SendFeedbackMessage(FEEDBACK_COMBAT_MODE_WEAPON_INVALID, NULL);
                return;
            }
            if (g_pRules->m_pBaseItems->GetBaseItem(pWeapon->GetBaseItemType())->m_nRangedWeapon <= 0)
            {
                SendFeedbackMessage(FEEDBACK_COMBAT_MODE_WEAPON_INVALID, NULL);
                return;
            }
        }
        nOldMode = m_nActiveCombatMode;
        m_nActiveCombatMode = nMode;
    }

    if (nMode == SWSOBJECT_COMBAT_MODE_PARRY || nOldMode == SWSOBJECT_COMBAT_MODE_PARRY)
    {
        if (nMode == SWSOBJECT_COMBAT_MODE_PARRY)
            SetActivity(ACTIVITY_BIT_10, TRUE, 0);
        else
            SetActivity(ACTIVITY_BIT_10, FALSE, 0);
    }
    else if (nMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK || nOldMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK)
    {
        if (nMode == SWSOBJECT_COMBAT_MODE_POWER_ATTACK)
            SetActivity(ACTIVITY_BIT_8, TRUE, 0);
        else
            SetActivity(ACTIVITY_BIT_8, FALSE, 0);
    }
    else if (nMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK || nOldMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK)
    {
        if (nMode == SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK)
            SetActivity(ACTIVITY_BIT_9, TRUE, 0);
        else
            SetActivity(ACTIVITY_BIT_9, FALSE, 0);
    }
    else if (nMode == SWSOBJECT_COMBAT_MODE_COUNTERSPELL || nOldMode == SWSOBJECT_COMBAT_MODE_COUNTERSPELL)
    {
        if (nMode == SWSOBJECT_COMBAT_MODE_COUNTERSPELL)
            SetActivity(ACTIVITY_BIT_11, TRUE, 0);
        else
            SetActivity(ACTIVITY_BIT_11, FALSE, 0);
    }
    else if (nMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS || nOldMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS)
    {
        if (nMode == SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS)
            SetActivity(ACTIVITY_BIT_12, TRUE, 0);
        else
            SetActivity(ACTIVITY_BIT_12, FALSE, 0);
    }
    else if (nMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT || nOldMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT)
    {
        if (nMode == SWSOBJECT_COMBAT_MODE_RAPID_SHOT)
            SetActivity(ACTIVITY_BIT_13, TRUE, 0);
        else
            SetActivity(ACTIVITY_BIT_13, FALSE, 0);
    }
}
// PC 0x00542DF0
int CSWSCreature::AIActionItemCastSpell(CSWSObjectActionNode* pAction)
{
    Vector vTarget;
    unsigned long oidItem = (unsigned long)pAction->m_apParam[0];
    int nPropertyIndex = (int)pAction->m_apParam[1];
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[3];
    float fX = *(float*)&pAction->m_apParam[4];
    float fY = *(float*)&pAction->m_apParam[5];
    float fZ = *(float*)&pAction->m_apParam[6];
    unsigned long nStartDay, nStartTime;
    if (pAction->m_nNumParams < 9)
    {
        g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId())->GetWorldTime(
            &nStartDay, &nStartTime);
        pAction->m_apParam[7] = (void*)nStartDay;
        pAction->m_apParam[8] = (void*)nStartTime;
        pAction->m_nNumParams = 9;
    }
    else
    {
        nStartDay = (unsigned long)pAction->m_apParam[7];
        nStartTime = (unsigned long)pAction->m_apParam[8];
    }

    int bCastComplete = FALSE;
    unsigned long nEndTime = 0;
    unsigned short nEndAnimation = 0;
    if (pAction->m_nNumParams > 9)
    {
        bCastComplete = (int)pAction->m_apParam[9];
        nEndAnimation = (unsigned short)pAction->m_apParam[10];
        nEndTime = (unsigned long)pAction->m_apParam[11];
    }
    if (GetInParty())
    {
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        int nIndex = pClient->GetSWParty()->GetIndex(
            pClient->ServerToClientObjectId(GetId()));
        if (nIndex >= 0)
#ifdef _XBOX
        {
            CSWParty* pMemberParty = pClient->GetSWParty();

            pMemberParty->GetMemberData(nIndex)->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_ENGAGED);
        }
#else
            pClient->GetSWParty()->GetMemberData(nIndex)->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_ENGAGED);
#endif
    }
    m_nAttackMovementTimer = ITEM_CAST_AI_DELAY;
    if (GetDead() == TRUE || GetIsPCDying() == TRUE || GetDeadTemp() == TRUE)
    {
        SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
        m_bItemCastStarted = FALSE;
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }

    unsigned long nCurrentDay, nCurrentTime, nElapsedDay, nElapsedTime;
    g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId())->GetWorldTime(
        &nCurrentDay, &nCurrentTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nCurrentDay, nCurrentTime, nStartDay, nStartTime,
        &nElapsedDay, &nElapsedTime);
    if (bCastComplete)
    {
        if (nElapsedTime < nEndTime)
        {
            SetAnimation(nEndAnimation);
            return ACTION_IN_PROGRESS;
        }
        SetAnimation(GetPauseReadyAnimation());
        m_bItemCastStarted = FALSE;
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_COMPLETE;
    }

    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (oidTarget != OBJECT_INVALID && pTarget == NULL)
    {
        SetAnimation(GetPauseReadyAnimation());
        m_bItemCastStarted = FALSE;
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }
    if (pTarget != NULL && pTarget->AsSWSCreature() != NULL &&
        (pTarget->AsSWSCreature()->GetDead() ||
         pTarget->AsSWSCreature()->GetDeadTemp()))
    {
        SetAnimation(GetPauseReadyAnimation());
        m_bItemCastStarted = FALSE;
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }
    if (oidTarget == OBJECT_INVALID)
        m_oidSpellTarget = GetAreaId();
    else
        m_oidSpellTarget = oidTarget;
    CSWSCreature* pTargetCreature = NULL;
    if (pTarget != NULL)
        pTargetCreature = pTarget->AsSWSCreature();

    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
    {
        SetAnimation(GetPauseReadyAnimation());
        m_bItemCastStarted = FALSE;
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }
    CSWItemProperty* pProperty = pItem->GetActiveProperty(nPropertyIndex);
    if (pProperty == NULL)
    {
        SetAnimation(GetPauseReadyAnimation());
        m_bItemCastStarted = FALSE;
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }
    unsigned long nSpellId = pProperty->m_nSubtype;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
    if (pSpell == NULL)
    {
        SetAnimation(GetPauseReadyAnimation());
        m_bItemCastStarted = FALSE;
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }
    int bHostile = pSpell->m_bHostileSetting;
    if (bHostile)
    {
        ActivityManager(ACTIVITY_ACTION_CLEAR_MODES);
        SetCombatState(TRUE, COMBAT_STATE_NOTIFY);
    }
    else
        ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);

    vTarget = Vector(fX, fY, fZ);
    CExoString sModelType;
    g_pRules->Get2DArrays()->GetAppearance()->GetCExoStringEntry(
        m_cAppearanceInfo.m_nAppearance, "MODELTYPE", &sModelType);
    int bHumanoid = sModelType.CompareNoCase("B") || sModelType.CompareNoCase("F");
    pAction->m_nField70 = 0;
    int bForceMaster = FALSE;
    unsigned short nAnimation;
    unsigned long nCastTime, nRoundTime;
    switch (pItem->GetBaseItem()->m_nItemType)
    {
    case CSWBaseItem::ITEM_TYPE_STIMULANT:
    case CSWBaseItem::ITEM_TYPE_MEDPAC:
    case CSWBaseItem::ITEM_TYPE_REPAIR_KIT:
        nAnimation = bHumanoid ? CREATURE_ANIMATION_FIREFORGET_INJECT : CREATURE_ANIMATION_COMBAT_READY;
        nCastTime = ITEM_CAST_USE_TIME;
        nRoundTime = ITEM_CAST_ROUND_TIME;
        break;
    case CSWBaseItem::ITEM_TYPE_SQUAD_MEDPAC:
        nAnimation = bHumanoid ? CREATURE_ANIMATION_STEALTH_TRANSITION : CREATURE_ANIMATION_COMBAT_READY;
        nCastTime = ITEM_CAST_USE_TIME;
        nRoundTime = ITEM_CAST_ROUND_TIME;
        break;
    case CSWBaseItem::ITEM_TYPE_HOSTILE_USABLE:
        if (bHumanoid)
        {
            Vector vDelta = vTarget - m_vPosition;
            if (vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
                vDelta.m_fZ * vDelta.m_fZ >= GrenadeThrowThreshold * GrenadeThrowThreshold)
            {
                nAnimation = CREATURE_ANIMATION_FIREFORGET_THROW_HIGH;
                nCastTime = ITEM_CAST_GRENADE_THROW_TIME;
            }
            else
            {
                nAnimation = CREATURE_ANIMATION_FIREFORGET_THROW_LOW;
                nCastTime = ITEM_CAST_GRENADE_TOSS_TIME;
            }
        }
        else
        {
            nAnimation = CREATURE_ANIMATION_COMBAT_READY;
            nCastTime = ITEM_CAST_GRENADE_THROW_TIME;
        }
        nRoundTime = ITEM_CAST_ROUND_TIME;
        bForceMaster = TRUE;
        break;
    case CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_20:
        nAnimation = bHumanoid ? CREATURE_ANIMATION_STEALTH_TRANSITION : CREATURE_ANIMATION_COMBAT_READY;
        nCastTime = ITEM_CAST_EQUIPPED_USE_TIME;
        nRoundTime = ITEM_CAST_EQUIPPED_ROUND_TIME;
        break;
    case CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_12:
    {
        CSWCCreature* pClientCreature = GetClientCreature();
        nRoundTime = ITEM_CAST_ROUND_TIME;
        if (pClientCreature != NULL)
            nCastTime = (unsigned long)pClientCreature->GetAnimationLength(
                CREATURE_ANIMATION_ACTIVATE_ITEM) - ITEM_CAST_ANIMATION_MARGIN;
        else
            nCastTime = ITEM_CAST_EQUIPPED_FALLBACK_TIME;
        nAnimation = nElapsedTime < nCastTime ? CREATURE_ANIMATION_ACTIVATE_ITEM : CREATURE_ANIMATION_ACTIVATE_ITEM_END;
        break;
    }
    default:
    {
        nAnimation = CREATURE_ANIMATION_SPELLCAST_1;
        nCastTime = 1;
        int nSpellCastTime = pSpell->m_nCastTime;
        nRoundTime = nSpellCastTime + 1;
        break;
    }
    }

    if (!GetCombatRound()->m_bRoundStarted)
    {
        int bEngaged = FALSE;
        int bMaster;
        if (bForceMaster)
            bEngaged = bMaster = TRUE;
        else
        {
            bEngaged = GetCombatRound()->ResolveEngagedState(oidTarget);
            bMaster = GetCombatRound()->ResolveMasterState(oidTarget, bEngaged);
        }
        GetCombatRound()->StartCombatRound(oidTarget, bEngaged, bMaster, 0, 0);
        GetCombatRound()->SetSpellCastRound(0, 0);
        GetCombatRound()->SetRoundPaused(TRUE, GetId());
        GetCombatRound()->SetPauseTimer(nRoundTime, FALSE);
        GetCombatRound()->DecrementRoundLength(nRoundTime, FALSE);
    }
    if (!m_bItemCastStarted)
    {

        if ((pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_STIMULANT ||
             pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_STIMULANT) &&
            (!m_nAreaWaitState || !g_pAppManager->GetServerExoApp()->GetDifficultyOption(2)))
            BroadcastAttackOfOpportunity(OBJECT_INVALID);
        m_bItemCastStarted = TRUE;
    }
    if (nElapsedTime < nCastTime)
    {
        if (pItem->GetUsedActivePropertyUsesLeft(nPropertyIndex) < 1 &&
            pProperty->m_nCostValue != CSWItemProperty::ACTIVATE_ITEM_UNLIMITED_ALT &&
            pProperty->m_nCostValue != CSWItemProperty::ACTIVATE_ITEM_UNLIMITED)
        {
            SetAnimation(GetPauseReadyAnimation());
            m_bItemCastStarted = FALSE;
            RemoveSpellActionFromRound();
            ProcessPendingCombatActions();
            SendFeedbackMessage(FEEDBACK_ITEM_NO_USES, NULL);
            return ACTION_FAILED;
        }
        m_nLastSpellId = nSpellId;
        m_oidSpellTargetObject = oidTarget;
        m_vSpellTargetPosition = vTarget;
        m_oidSpellCasterItem = oidItem;
        m_bSafeProjectileBroadcast = FALSE;
        m_bCasterLevelOverride = FALSE;
        CalculateLastSpellProjectileTime(0);
        int nLevel = m_pStats->GetClassLevel(m_pStats->GetNumClasses() - 1, FALSE);
        m_nCasterLevelOverride = nLevel;
        if (pTargetCreature != NULL &&
            (pItem->GetBaseItem()->GetItemType() == CSWBaseItem::ITEM_TYPE_STIMULANT ||
             pItem->GetBaseItem()->GetItemType() == CSWBaseItem::ITEM_TYPE_REPAIR_KIT ||
             pItem->GetBaseItem()->GetItemType() == CSWBaseItem::ITEM_TYPE_SQUAD_MEDPAC))
            pTargetCreature->SetAnimation(nAnimation);
        else
            SetAnimation(nAnimation);
        return ACTION_IN_PROGRESS;
    }

    if (!m_bSafeProjectileBroadcast)
    {
        m_vSpellTargetPosition = vTarget;
        m_nLastSpellId = nSpellId;
        m_oidSpellTargetObject = oidTarget;
        m_bSafeProjectileBroadcast = FALSE;
        m_oidSpellCasterItem = oidItem;
        m_bCasterLevelOverride = TRUE;
        CalculateLastSpellProjectileTime(0);
        int nLevel = m_pStats->GetClassLevel(m_pStats->GetNumClasses() - 1, FALSE);
        m_nCasterLevelOverride = nLevel;
        if (pTargetCreature != NULL &&
            (pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_STIMULANT ||
             pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_REPAIR_KIT ||
             pItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_SQUAD_MEDPAC))
            pTargetCreature->SetAnimation(nAnimation);
        else
            SetAnimation(nAnimation);
        GetCombatRound()->IncrementNumActionsTaken(1);
        SpellCastAndImpact(nSpellId, vTarget, oidTarget, CASTING_CLASS_INNATE_FF, oidItem, 0, 0, 0, 0);
        if (GetCombatRound()->GetSpellActionPending() == TRUE)
            GetCombatRound()->RemoveSpellAction();
        pItem->DecrementUses(nPropertyIndex, GetId());
        if (pItem->GetBaseItem()->GetItemType() == CSWBaseItem::ITEM_TYPE_EQUIPPED_ACTIVATION_12)
        {
            pAction->m_apParam[9] = (void*)TRUE;
            *(unsigned short*)&pAction->m_apParam[10] = CREATURE_ANIMATION_ACTIVATE_ITEM_END;
            pAction->m_apParam[11] = (void*)nRoundTime;
            pAction->m_nNumParams = 12;
        }
    }
    if (nElapsedTime < nRoundTime)
        return ACTION_IN_PROGRESS;
    SetAnimation(GetPauseReadyAnimation());
    m_bItemCastStarted = FALSE;
    RemoveSpellActionFromRound();
    ProcessPendingCombatActions();
    return ACTION_COMPLETE;
}
// PC 0x00543920
int CSWSCreature::AIActionChangeFacingObject(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
    {
        return CSWSObject::ACTION_FAILED;
    }

    CGameObject* pObject;
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidTarget, &pObject) != CGameObjectArray::SUCCESS ||
        pObject->m_nObjectType <= CGameObject::OBJECT_TYPE_AREA ||
        ((CSWSObject*)pObject)->GetArea() != GetArea())
    {
        return CSWSObject::ACTION_FAILED;
    }

    Vector vPoint = pObject->AsSWSObject()->m_vPosition;
    Vector vDirection = vPoint - m_vPosition;
    vDirection.m_fZ = 0.0f;
    if (vDirection.m_fX * vDirection.m_fX +
        vDirection.m_fY * vDirection.m_fY >= SWSCREATURE_FACING_EPSILON_SQUARED)
    {
        SetOrientation(normalize(vDirection));
    }

    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00543A50
int CSWSCreature::AIActionChangeFacingPoint(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
    {
        return CSWSObject::ACTION_FAILED;
    }

    float fX;
    float fY;
    float fZ;
    memcpy(&fX, &pAction->m_apParam[0], sizeof(float));
    memcpy(&fY, &pAction->m_apParam[1], sizeof(float));
    memcpy(&fZ, &pAction->m_apParam[2], sizeof(float));
    Vector vPoint(fX, fY, fZ);
    Vector vDirection = vPoint - m_vPosition;
    vDirection.m_fZ = 0.0f;
    if (vDirection * vDirection >= SWSCREATURE_FACING_EPSILON_SQUARED)
    {
        SetOrientation(normalize(vDirection));
    }

    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00543B20
int CSWSCreature::AIActionCheckMoveAwayFromLocation(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
        return ACTION_FAILED;

    Vector vAway;
    CSWSArea* pArea = GetArea();
    vAway.m_fX = *(float*)&pAction->m_apParam[0];
    vAway.m_fY = *(float*)&pAction->m_apParam[1];
    vAway.m_fZ = *(float*)&pAction->m_apParam[2];
    int bRun = (int)pAction->m_apParam[3];
    int nDistance = (int)pAction->m_apParam[4];
    int bOption = (int)pAction->m_apParam[5];
    Vector vDelta = m_vPosition - vAway;
    if (vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY + vDelta.m_fZ * vDelta.m_fZ <= *(float*)&nDistance * *(float*)&nDistance)
    {
        Vector vDestination = pArea->ComputeAwayVector(
            m_vPosition, vAway, *(float*)&nDistance, m_pPathfindInformation);
        unsigned long oidArea = pArea->GetId();
        AddActionToFront(ACTION_CHECK_MOVE_AWAY_FROM_LOCATION, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &vAway.m_fX, ACTION_PARAM_FLOAT, &vAway.m_fY,
            ACTION_PARAM_FLOAT, &vAway.m_fZ, ACTION_PARAM_INTEGER, &bRun,
            ACTION_PARAM_FLOAT, &nDistance, ACTION_PARAM_INTEGER, &bOption,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        Vector vOrientation;
        AddMoveToPointActionToFront(pAction->m_nGroupId, vDestination,
            oidArea, OBJECT_INVALID, bRun, 0.0f, 0.0f,
            0, 0, 0, 0, 1, 0, vOrientation, 0, 0, 0);
        AddPathfindingWaitActionToFront(pAction->m_nGroupId);
    }
    return ACTION_COMPLETE;
}
// PC 0x00543CF0
int CSWSCreature::AIActionCheckInterAreaPathfinding(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
        return ACTION_FAILED;
    if (GetArea() == NULL)
        return ACTION_IN_PROGRESS;

    int nAreas = m_pPathfindInformation->m_nField_A8;
    if (nAreas > 1)
    {
        int* pAreaPath = new int[nAreas - 1];
        int* pAreaPathCost = NULL;
        if (nAreas > 2)
        {
            pAreaPathCost = new int[nAreas - 2];
            for (int i = 0; i < nAreas - 2; ++i)
                pAreaPathCost[i] = m_pPathfindInformation->m_pAreaPath[i + 1];
        }
        for (int i = 0; i < nAreas - 1; ++i)
            pAreaPath[i] = m_pPathfindInformation->m_pAreaPath[i + 1];

        if (m_pPathfindInformation->m_pAreaPath != NULL)
            delete[] m_pPathfindInformation->m_pAreaPath;
        if (m_pPathfindInformation->m_pAreaPathCost != NULL)
            delete[] m_pPathfindInformation->m_pAreaPathCost;
        m_pPathfindInformation->m_pAreaPathCost = pAreaPathCost;
        m_pPathfindInformation->m_pAreaPath = pAreaPath;
        m_pPathfindInformation->m_nField_A8 = nAreas - 1;

        if (GetArea() != NULL)
        {
            g_pAppManager->GetServerExoApp()->GetModule()->SetIntraAreaGoal(
                m_pPathfindInformation);
            float fX = m_pPathfindInformation->m_fStartX;
            float fY = m_pPathfindInformation->m_fStartY;
            float fZ = m_pPathfindInformation->m_fStartZ;
            unsigned long oidArea = m_pPathfindInformation->m_nStartArea;
            m_pPathfindInformation->m_fStartX = fX - 5.0f;
            m_pPathfindInformation->m_nField_A4 = 1;
            AddMoveToPointActionToFront(pAction->m_nGroupId, Vector(fX, fY, fZ),
                oidArea, OBJECT_INVALID, 0, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0,
                Vector(), 0, 0, 0);
            AddPathfindingWaitActionToFront(pAction->m_nGroupId);
        }
    }
    else
    {
        m_pPathfindInformation->ResetInterAreaSearchData();
    }
    float fWait = 0.75f;
    AddActionToFront(ACTION_WAIT, pAction->m_nGroupId,
        ACTION_PARAM_FLOAT, &fWait, 0, NULL, 0, NULL, 0, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
        0, NULL, 0, NULL, 0, NULL);
    return ACTION_COMPLETE;
}
// PC 0x00543F80
int CSWSCreature::AIActionCheckMoveToObject(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
        return ACTION_FAILED;

    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    int bRun = (int)pAction->m_apParam[1];
    Vector vPosition;
    float fRange = 0.0f;
    CGameObject* pObject;

    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidTarget, &pObject) != CGameObjectArray::SUCCESS)
        return ACTION_FAILED;

    CSWSArea* pArea = ((CSWSObject*)pObject)->GetArea();
    GetUseRange(oidTarget, vPosition, fRange, 0);
    if (pArea == NULL)
    {
        if (pObject->AsSWSCreature() == NULL)
            return ACTION_FAILED;
        pArea = g_pAppManager->GetServerExoApp()->GetModule()->GetArea();
        vPosition = pObject->AsSWSCreature()->m_vDisappearPosition;
        if (pArea == NULL)
            return ACTION_FAILED;
    }

    if (pArea != GetArea() || !GetIsInUseRange(oidTarget, 0.0f, 0))
    {
        unsigned long oidArea = pArea->GetId();
        AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        Vector vOrientation;
        AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition,
            oidArea, oidTarget, bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0,
            vOrientation, 0, 0, 0);
        AddPathfindingWaitActionToFront(pAction->m_nGroupId);
    }
    SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID, 0);
    return ACTION_COMPLETE;
}
// PC 0x005441A0
int CSWSCreature::AIActionCheckMoveToObjectRadius(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
        return ACTION_FAILED;

    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    int bRun = (int)pAction->m_apParam[1];
    float fRange = *(float*)&pAction->m_apParam[2];
    float fRadius = *(float*)&pAction->m_apParam[3];
    int bCheckRange = (int)pAction->m_apParam[4];
    int bUseRange = 0;
    if (pAction->m_apParam[5] != NULL)
        bUseRange = (int)pAction->m_apParam[5];
    int bOption = (int)pAction->m_apParam[6];
    Vector vPosition;
    float fUseRange;
    CGameObject* pObject;

    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidTarget, &pObject) != CGameObjectArray::SUCCESS ||
            pObject == NULL || pObject->AsSWSObject() == NULL)
        return ACTION_FAILED;

    GetUseRange(oidTarget, vPosition, fUseRange, 0);
    CSWSArea* pArea = pObject->AsSWSObject()->GetArea();
    if (pObject->AsSWSObject()->GetArea() == NULL)
    {
        if (pObject->AsSWSCreature() == NULL)
            return ACTION_FAILED;
        pArea = g_pAppManager->GetServerExoApp()->GetModule()->GetArea();
        vPosition = pObject->AsSWSCreature()->m_vDisappearPosition;
    }
    if (pArea == NULL || GetArea() == NULL)
        return ACTION_FAILED;

    int bDifferentArea = pArea != GetArea();
    float fExtraRange = max(0.0f, fRadius - fUseRange);
    int bInRange = GetIsInUseRange(oidTarget, fExtraRange, bUseRange);
    if (bDifferentArea || !bInRange)
    {
        unsigned long oidArea = pArea->GetId();
        AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
            ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_FLOAT, &fRadius,
            ACTION_PARAM_INTEGER, &bCheckRange, ACTION_PARAM_INTEGER, &bUseRange,
            ACTION_PARAM_INTEGER, &bOption, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        Vector vOrientation;
        AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition,
            oidArea, oidTarget, bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 1,
            vOrientation, 0, 0, bOption);
        AddPathfindingWaitActionToFront(pAction->m_nGroupId);
    }
    return ACTION_COMPLETE;
}
// PC 0x00544460
int CSWSCreature::AIActionCheckMoveToPoint(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1 ||
        !(m_nAIStateFlags & AI_STATE_CAN_MOVE))
        return ACTION_FAILED;

    float fX = *(float*)&pAction->m_apParam[0];
    float fY = *(float*)&pAction->m_apParam[1];
    float fZ = *(float*)&pAction->m_apParam[2];
    unsigned long oidArea = (unsigned long)pAction->m_apParam[3];
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[4];
    int bRun = (int)pAction->m_apParam[5];
    CGameObject* pObject;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidArea, &pObject) != CGameObjectArray::SUCCESS)
        return ACTION_FAILED;

    float fRange = m_pPathfindInformation->m_fStepScale;
    int bDifferentArea = GetArea() != NULL && oidArea != GetArea()->GetId();
    Vector vDelta = m_vPosition - Vector(fX, fY, fZ);
    if (bDifferentArea ||
        vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
            vDelta.m_fZ * vDelta.m_fZ >= (fRange * 2.0f) * (fRange * 2.0f))
    {
        AddActionToFront(ACTION_CHECK_MOVE_TO_POINT, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY,
            ACTION_PARAM_FLOAT, &fZ, ACTION_PARAM_OBJECT, &oidArea,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, (void*)bRun,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        Vector vPosition(fX, fY, fZ);
        Vector vOrientation;
        AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition,
            oidArea, oidTarget, bRun, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0,
            vOrientation, 0, 0, 0);
        AddPathfindingWaitActionToFront(pAction->m_nGroupId);
    }
    return ACTION_COMPLETE;
}
// PC 0x00544690
int CSWSCreature::AIActionCheckMoveToPointRadius(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
        return ACTION_FAILED;

    float fX = *(float*)&pAction->m_apParam[0];
    float fY = *(float*)&pAction->m_apParam[1];
    float fZ = *(float*)&pAction->m_apParam[2];
    unsigned long oidArea = (unsigned long)pAction->m_apParam[3];
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[4];
    int bRun = (int)pAction->m_apParam[5];
    float fRange = *(float*)&pAction->m_apParam[6];
    int bOption = (int)pAction->m_apParam[7];
    CGameObject* pObject;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidArea, &pObject) != CGameObjectArray::SUCCESS)
        return ACTION_FAILED;

    int bDifferentArea = GetArea() != NULL && oidArea != GetArea()->GetId();
    Vector vDelta = m_vPosition - Vector(fX, fY, fZ);
    if (bDifferentArea || vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY >
            (fRange + NWSCREATURE_POINT_RADIUS_TOLERANCE) *
            (fRange + NWSCREATURE_POINT_RADIUS_TOLERANCE))
    {
        AddActionToFront(ACTION_CHECK_MOVE_TO_POINT_RADIUS, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY,
            ACTION_PARAM_FLOAT, &fZ, ACTION_PARAM_OBJECT, &oidArea,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
            ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_INTEGER, &bOption,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        Vector vPosition(fX, fY, fZ);
        Vector vOrientation;
        AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition,
            oidArea, oidTarget, bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0,
            vOrientation, 0, 0, bOption);
        AddPathfindingWaitActionToFront(pAction->m_nGroupId);
    }
    return ACTION_COMPLETE;
}
// PC 0x005448B0
int CSWSCreature::AIActionCheckMoveToFollowRadius(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
        return ACTION_FAILED;

    float fX = *(float*)&pAction->m_apParam[0];
    float fY = *(float*)&pAction->m_apParam[1];
    float fZ = *(float*)&pAction->m_apParam[2];
    unsigned long oidArea = (unsigned long)pAction->m_apParam[3];

    unsigned long oidTarget = (unsigned long)pAction->m_apParam[4];
    int nMovementFlags = (int)pAction->m_apParam[5];
    float fRadius = *(float*)&pAction->m_apParam[6];
    CGameObject* pObject;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidArea, &pObject) != CGameObjectArray::SUCCESS)
        return ACTION_FAILED;

    int bDifferentArea = GetArea() != NULL && oidArea != GetArea()->GetId();
    Vector vDelta(m_vPosition.m_fX - fX, m_vPosition.m_fY - fY,
        m_vPosition.m_fZ - fZ);
    if (bDifferentArea || vDelta * vDelta > (fRadius + NWSCREATURE_FOLLOW_RADIUS_TOLERANCE) *
            (fRadius + NWSCREATURE_FOLLOW_RADIUS_TOLERANCE))
    {
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        unsigned long oidCreature = pClient->ServerToClientObjectId(GetId());
        int nIndex = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(oidCreature);
        if (nIndex >= 0)
        {

#ifdef _XBOX
            CSWPartyMemberData* pMember = g_pAppManager->GetClientExoApp()->GetSWParty()->GetMemberData(nIndex); pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_FIND_PATH; pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_WAITING_FOR_PATH;
#else
            CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty(); pParty->m_aPartyMemberData[nIndex].m_nCharacterIndex = CSWPartyMemberData::FOLLOW_FIND_PATH;
            pParty->m_aPartyMemberData[nIndex].m_nCharacterIndex = CSWPartyMemberData::FOLLOW_WAITING_FOR_PATH;
#endif
        }
    }
    return ACTION_COMPLETE;
}
// PC 0x00544A30
int CSWSCreature::AIActionEncounterCreatureDestroySelf()
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
    {
        return CSWSObject::ACTION_FAILED;
    }

    CSWSEncounter* pEncounter = g_pAppManager->GetServerExoApp()->GetEncounterByGameObjectID(m_oidSpawnedBy);
    if (pEncounter != NULL)
    {
        pEncounter->RemoveFromActiveCreatureCount(m_pStats->GetChallengeRating(), 1);
        m_bEncounterCountRemoved = 1;
    }

    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    pServer->GetServerAIMaster()->AddEventDeltaTime(
        0, ENCOUNTER_CREATURE_DESTROY_DELAY, m_nId, m_nId,
        CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
    m_bCommandable = 0;
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00544AF0
unsigned char CSWSCreature::CanEquipWeapon(CSWSItem* pItem,
                                         unsigned long* pnSlot,
                                         int bFeedback, int bUnknown)
{
    CSWSItem* pRightItem;
    CSWSItem* pLeftItem;
    CSWBaseItem* pRightBaseItem = NULL;
    CSWBaseItem* pLeftBaseItem = NULL;
    CSWBaseItem* pBaseItem;

    if (*pnSlot == INVENTORY_SLOT_RIGHTHAND ||
        *pnSlot == INVENTORY_SLOT_LEFTHAND)
    {
        pRightItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        pLeftItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
        if (pRightItem != NULL)
            pRightBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                pRightItem->GetBaseItemType());
        if (pLeftItem != NULL)
            pLeftBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                pLeftItem->GetBaseItemType());

        if (pItem != NULL)
        {
            pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                pItem->GetBaseItemType());
            if (pRightItem == NULL && pLeftItem == NULL)
            {
                if (*pnSlot == INVENTORY_SLOT_LEFTHAND)
                    *pnSlot = INVENTORY_SLOT_RIGHTHAND;
                return 1;
            }
            else if (pRightItem != NULL && pLeftItem == NULL)
            {
                if (pBaseItem->GetIsTwoHanded() == 1)
                {
                    *pnSlot = INVENTORY_SLOT_RIGHTHAND;
                    return 3;
                }
                if ((pRightBaseItem->m_nRangedWeapon > 0) ==
                    (pBaseItem->m_nRangedWeapon > 0))
                {
                    if (pRightBaseItem->GetIsOneHanded() == 1)
                    {
                        if (*pnSlot == INVENTORY_SLOT_LEFTHAND)
                            return 1;
                        return 2;
                    }
                    *pnSlot = INVENTORY_SLOT_RIGHTHAND;
                    return 2;
                }
                *pnSlot = INVENTORY_SLOT_RIGHTHAND;
                return 2;
            }
            else if (pRightItem != NULL && pLeftItem != NULL)
            {
                if (pBaseItem->GetIsTwoHanded() == 1)
                {
                    *pnSlot = INVENTORY_SLOT_RIGHTHAND;
                    return 3;
                }
                if ((pRightBaseItem->m_nRangedWeapon > 0) ==
                    (pBaseItem->m_nRangedWeapon > 0))
                    return 2;
                *pnSlot = INVENTORY_SLOT_RIGHTHAND;
                return 3;
            }
        }
    }
    return 0;
}
// PC 0x00544C50
unsigned char CSWSCreature::CanEquipWeapon2(CSWSItem* pItem,
                                          unsigned long* pnSlot,
                                          int bFeedback, int bUnknown)
{
    CSWSItem* pRightItem;
    CSWSItem* pLeftItem;
    CSWBaseItem* pRightBaseItem = NULL;
    CSWBaseItem* pLeftBaseItem = NULL;
    CSWBaseItem* pBaseItem;

    if (*pnSlot == INVENTORY_SLOT_RIGHTHAND2 ||
        *pnSlot == INVENTORY_SLOT_LEFTHAND2)
    {
        pRightItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND2);
        pLeftItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND2);
        if (pRightItem != NULL)
            pRightBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                pRightItem->GetBaseItemType());
        if (pLeftItem != NULL)
            pLeftBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                pLeftItem->GetBaseItemType());

        if (pItem != NULL)
        {
            pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                pItem->GetBaseItemType());
            if (pRightItem == NULL && pLeftItem == NULL)
            {
                if (*pnSlot == INVENTORY_SLOT_LEFTHAND2)
                    *pnSlot = INVENTORY_SLOT_RIGHTHAND2;
                return 1;
            }
            else if (pRightItem != NULL && pLeftItem == NULL)
            {
                if (pBaseItem->GetIsTwoHanded() == 1)
                {
                    *pnSlot = INVENTORY_SLOT_RIGHTHAND2;
                    return 4;
                }
                if ((pRightBaseItem->m_nRangedWeapon > 0) ==
                    (pBaseItem->m_nRangedWeapon > 0))
                {
                    if (pRightBaseItem->GetIsOneHanded() == 1)
                    {
                        if (*pnSlot == INVENTORY_SLOT_LEFTHAND2)
                            return 1;
                        return 2;
                    }
                    *pnSlot = INVENTORY_SLOT_RIGHTHAND2;
                    return 2;
                }
                *pnSlot = INVENTORY_SLOT_RIGHTHAND2;
                return 2;
            }
            else if (pRightItem != NULL && pLeftItem != NULL)
            {
                if (pBaseItem->GetIsTwoHanded() == 1)
                {
                    *pnSlot = INVENTORY_SLOT_RIGHTHAND2;
                    return 4;
                }
                if ((pRightBaseItem->m_nRangedWeapon > 0) ==
                    (pBaseItem->m_nRangedWeapon > 0))
                    return 2;
                *pnSlot = INVENTORY_SLOT_RIGHTHAND2;
                return 4;
            }
        }
    }
    return 0;
}
// PC 0x0054AD10
int CSWSCreature::AIActionCheckMoveAwayFromObject(CSWSObjectActionNode* pAction)
{

    if (GetDead() == TRUE || CSWSObject::GetIsPCDying() == TRUE ||
        GetDeadTemp() == TRUE)
    {
        return CSWSObject::ACTION_FAILED;
    }

    unsigned long oidFleeFrom = (unsigned long)pAction->m_apParam[0];
    int bRun = (int)pAction->m_apParam[1];
    float fMoveAwayRange = *(float*)&pAction->m_apParam[2];
    int nRemainingChecks = (int)pAction->m_apParam[3];
    Vector vFleeFrom;
    CGameObject* pTarget;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if (pServer->GetObjectArray()->GetGameObject(oidFleeFrom, &pTarget) !=
        CGameObjectArray::SUCCESS)
    {
        return CSWSObject::ACTION_FAILED;
    }

    vFleeFrom = ((CSWSObject*)pTarget)->m_vPosition;
    CSWSArea* pArea = ((CSWSObject*)pTarget)->GetArea();
    CExoLinkedList<CSWSObjectActionNode>* pActions = &m_lstActions;
    int bCancelled;
    do
    {
        bCancelled = FALSE;
        if (pActions != NULL)
        {
            CSWSObjectActionNode* pQueuedAction = pActions->GetHead();
            CExoLinkedListNode* pPos = pActions->GetHeadPos();
            while (pQueuedAction != NULL && !bCancelled)
            {
                if (pQueuedAction->m_nActionType ==
                        CSWSObject::ACTION_CHECK_MOVE_AWAY_FROM_OBJECT)
                {
                    unsigned long oidQueued = (unsigned long)pQueuedAction->m_apParam[0];
                    if (pQueuedAction == NULL && oidQueued != 0)
                        return CSWSObject::ACTION_FAILED;
                    if (oidQueued == oidFleeFrom)
                    {
                        CancelAction(0, pQueuedAction->m_nGroupId);
                        bCancelled = TRUE;
                    }
                    else
                    {
                        pQueuedAction = pActions->GetNext(pPos);
                    }
                }
                else
                {
                    pQueuedAction = pActions->GetNext(pPos);
                }
            }
        }
    }
    while (bCancelled == TRUE);

    Vector vDelta = m_vPosition - vFleeFrom;
    if (pArea == CSWSObject::GetArea() &&
        vDelta * vDelta <= fMoveAwayRange * fMoveAwayRange &&
        nRemainingChecks >= 1)
    {
        --nRemainingChecks;
        Vector vDestination = pArea->ComputeAwayVector(
            m_vPosition, vFleeFrom, fMoveAwayRange, m_pPathfindInformation);
        unsigned long oidArea = pArea->m_nId;
        CSWSObject::AddActionToFront(
            CSWSObject::ACTION_CHECK_MOVE_AWAY_FROM_OBJECT, pAction->m_nGroupId,
            CSWSObject::ACTION_PARAM_OBJECT, &oidFleeFrom,
            CSWSObject::ACTION_PARAM_INTEGER, &bRun,
            CSWSObject::ACTION_PARAM_FLOAT, &fMoveAwayRange,
            CSWSObject::ACTION_PARAM_INTEGER, &nRemainingChecks,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        Vector vOrientation;
        AddMoveToPointActionToFront(
            pAction->m_nGroupId, vDestination, oidArea, OBJECT_INVALID, bRun,
            0.0f, 0.0f, FALSE, 0, FALSE, FALSE, TRUE, FALSE,
            vOrientation, 0, 0, FALSE);
        AddPathfindingWaitActionToFront(pAction->m_nGroupId);
    }
    return CSWSObject::ACTION_COMPLETE;
}
enum
{
    SET_MINE_DIFFICULTY_EASY = 0,
    SET_MINE_DIFFICULTY_HARD = 2,
    SET_MINE_DIFFICULTY_DC_MODIFIER = 5,
    SET_MINE_STRENGTH_MINOR = 0,
    SET_MINE_STRENGTH_AVERAGE = 3,
    SET_MINE_STRENGTH_DEADLY = 9,
    SET_MINE_STRENGTH_DEVASTATING = 12,
    SET_MINE_TRIGGER_DELAY = 3000
};
// PC 0x0054F440
int CSWSCreature::AIActionSetMine(CSWSObjectActionNode* pAction)
{
    CExoString sTrapScript;
    CSWSTrigger* pTrigger = NULL;
    Vector vTarget;
    Vector vMoveTo;
    int nDetectDCMod;
    int nDisarmDCMod;

    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);

    void* pItemParam = pAction > NULL ? pAction->m_apParam[0] : pAction->m_apParam[0];
    unsigned long oidItem = (unsigned long)pItemParam;
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[1];
    float fX, fY, fZ;
    memcpy(&fX, &pAction->m_apParam[2], sizeof(float));
    memcpy(&fY, &pAction->m_apParam[3], sizeof(float));
    memcpy(&fZ, &pAction->m_apParam[4], sizeof(float));
    Vector vPosition(fX, fY, fZ);
    vTarget = vPosition;

    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
    {
        StopGuiTimingBar();
        m_bMineActionStarted = FALSE;
        return ACTION_FAILED;
    }

    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
    {
        if (equal(vTarget, Vector()))
        {
            m_bMineActionStarted = FALSE;
            return ACTION_FAILED;
        }
    }
    else
        vTarget = pTarget->AsSWSObject()->m_vPosition;

    if (GetArea() != NULL && !GetArea()->GetPlayerCanSetMines())
    {
        StopGuiTimingBar();
        m_bMineActionStarted = FALSE;
        return ACTION_FAILED;
    }

    float fRange;
    int bInRange;
    if (pTarget == NULL)
    {
        vMoveTo = vTarget - normalize(m_vPosition - vTarget);
        Vector vDifference = m_vPosition - vMoveTo;
        bInRange = vDifference * vDifference < 2.25f;
    }
    else
    {
        GetUseRange(oidTarget, vMoveTo, fRange, FALSE);
        bInRange = GetIsInUseRange(oidTarget, 0.5f, FALSE);
    }

    if (!bInRange)
    {
        AddActionToFront(ACTION_SET_MINE, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY, ACTION_PARAM_FLOAT, &fZ,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        if (pTarget == NULL)
        {
            AddActionToFront(ACTION_CHANGE_FACING_POINT, pAction->m_nGroupId,
                ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY, ACTION_PARAM_FLOAT, &fZ,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
        else
        {
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
        if (pTarget == NULL)
        {
            Vector vOrientation;
            AddMoveToPointActionToFront(pAction->m_nGroupId, vMoveTo, GetAreaId(),
                OBJECT_INVALID, TRUE, 1.0f, 0.0f, FALSE, 0, FALSE, FALSE, FALSE, FALSE,
                vOrientation, 0, 0, FALSE);
        }
        else
        {
            Vector vOrientation;
            AddMoveToPointActionToFront(pAction->m_nGroupId, vMoveTo, GetAreaId(),
                oidTarget, TRUE, fRange, 0.0f, FALSE, 0, FALSE, FALSE, FALSE, FALSE,
                vOrientation, 0, 0, FALSE);
        }
        m_bUnlockObjectFacingSet = TRUE;
        return ACTION_COMPLETE;
    }

    if (!m_bUnlockObjectFacingSet)
    {
        AddActionToFront(ACTION_SET_MINE, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY, ACTION_PARAM_FLOAT, &fZ,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        m_bUnlockObjectFacingSet = TRUE;
        return ACTION_COMPLETE;
    }

    if (!m_bMineActionStarted)
    {
        m_bMineActionStarted = TRUE;
        int nAnimation = CREATURE_ANIMATION_MINE_ARM;
        int bWait = TRUE;
        float fSpeed = 1.0f;
        float fDuration = 2.0f;
        AddActionToFront(ACTION_SET_MINE, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY, ACTION_PARAM_FLOAT, &fZ,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_PLAY_ANIMATION, pAction->m_nGroupId,
            ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
            ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bWait,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        StartGuiTimingBar(SET_MINE_TIMING_DURATION, GUI_TIMING_SET_MINE);
        return ACTION_COMPLETE;
    }

    int nTrapType = pItem->GetActiveProperty(0)->m_nSubtype;
    int nMineStrength = SET_MINE_STRENGTH_MINOR;
    switch (nTrapType)
    {
    case 3:
    case 6:
    case 14:
        nMineStrength = SET_MINE_STRENGTH_MINOR;
        break;
    case 4:
    case 7:
    case 15:
        nMineStrength = SET_MINE_STRENGTH_AVERAGE;
        break;
    case 5:
    case 8:
    case 16:
    case 19:
    case 21:
    case 25:
        nMineStrength = SET_MINE_STRENGTH_DEADLY;
        break;
    case 20:
    case 22:
    case 26:
        nMineStrength = SET_MINE_STRENGTH_DEVASTATING;
        break;
    }
    int nSetDC;
    g_pRules->Get2DArrays()->GetTraps()->GetINTEntry(nTrapType, "SetDC", &nSetDC);
    int nDifficultyModifier = 0;
    switch (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nDifficulty)
    {
    case SET_MINE_DIFFICULTY_EASY:
        nDifficultyModifier = -SET_MINE_DIFFICULTY_DC_MODIFIER;
        break;
    case SET_MINE_DIFFICULTY_HARD:
        nDifficultyModifier = SET_MINE_DIFFICULTY_DC_MODIFIER;
        break;
    }
    nSetDC += nDifficultyModifier;
    if (nSetDC < 1)
        nSetDC = 1;
    int nSkill = m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, FALSE);
    int bTakeTwenty = FALSE;
    int nRoll;
    if (m_bCombatState && pTarget == NULL)
        nRoll = rand() % 20 + 1;
    else
    {
        nRoll = 20;
        bTakeTwenty = TRUE;
    }
    nRoll += nSkill;
    int bCriticalFailure = FALSE;
    int bSuccess = FALSE;
    if (nRoll >= nSetDC)
        bSuccess = TRUE;
    else if (!bTakeTwenty && nRoll < nSetDC - SET_MINE_CRITICAL_FAILURE_MARGIN)
        bCriticalFailure = TRUE;
    if (bSuccess || bCriticalFailure)
    {
        g_pRules->Get2DArrays()->GetTraps()->GetINTEntry(nTrapType, "DetectDCMod", &nDetectDCMod);
        g_pRules->Get2DArrays()->GetTraps()->GetINTEntry(nTrapType, "DisarmDCMod", &nDisarmDCMod);
        g_pRules->Get2DArrays()->GetTraps()->GetCExoStringEntry(nTrapType, "TrapScript", &sTrapScript);
        if (pTarget != NULL && pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            CSWSDoor* pDoor = pTarget->AsSWSDoor();
            if (!pDoor->m_bTrapFlag)
            {
                pDoor->m_bTrapFlag = TRUE;
                pDoor->m_nTrapType = nTrapType;
                pDoor->m_asScripts[CSWSDoor::SCRIPT_ON_TRAP_TRIGGERED] = sTrapScript;
                pDoor->SetDetectDC(nDetectDCMod + nRoll);
                pDoor->SetDisarmDC(nRoll + nDisarmDCMod);
                pDoor->m_bTrapDetectable = TRUE;
                pDoor->m_bTrapDisarmable = FALSE;
                pDoor->m_bTrapOneShot = TRUE;
                pDoor->m_nOwnerDemolitionsSkill =
                    m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, FALSE);
                unsigned long oidDoor = pDoor->GetId();
                CSWSArea* pArea = GetArea();
                pArea->m_lstTrapObjectIds.Add(oidDoor);

                pTrigger = new CSWSTrigger(OBJECT_INVALID);
                pTrigger->SetCreator(GetId());
                pTrigger->m_nTrapType = nTrapType;
                pTrigger->m_bMineTrigger = TRUE;
                pTrigger->SetPosition(pDoor->m_vPosition, TRUE, TRUE, FALSE);
                pTrigger->m_nFaction = GetFactionId();
                pTrigger->m_bTrapDisarmable = FALSE;
                pTrigger->m_bTrapDetectable = FALSE;
                pTrigger->m_bPartyInteract = FALSE;
                int nTrapName = 0;
                g_pRules->Get2DArrays()->GetTraps()->GetINTEntry(nTrapType, "TrapName", &nTrapName);
                pTrigger->m_lsName.m_dwStringRef = nTrapName;
                unsigned long oidTrigger = pTrigger->GetId();
                CSWSArea* pTriggerArea = GetArea();
                pTriggerArea->m_lstTrapObjectIds.Add(oidTrigger);
                CSWSArea* pGeometryArea = GetArea();
                pTrigger->CreateNewGeometry(2.0f, vTarget, pGeometryArea);
                pTrigger->AddToArea(GetArea(), pDoor->m_vPosition.m_fX,
                    pDoor->m_vPosition.m_fY, pDoor->m_vPosition.m_fZ, TRUE);
                pDoor->m_pTrapTrigger = pTrigger;
                if (!bCriticalFailure)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                    pEvent->SetInteger(0, TRUE);
                    pEvent->SetInteger(1, FALSE);
                    pEvent->SetInteger(2, TRUE);
                    pEvent->SetInteger(3, nMineStrength);
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, SET_MINE_TRIGGER_DELAY, GetId(), oidTarget,
                        CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
            }
        }
        else if (pTarget != NULL && pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            CSWSPlaceable* pPlaceable = pTarget->AsSWSPlaceable();
            if (!pPlaceable->m_nTrapFlag)
            {
                pPlaceable->m_nTrapType = nTrapType;
                pPlaceable->m_nTrapFlag = TRUE;
                pPlaceable->m_sScripts[CSWSPlaceable::SCRIPT_ONTRAPTRIGGERED] = sTrapScript;
                pPlaceable->m_nTrapDetectDC = nDetectDCMod + nRoll;
                pPlaceable->m_nDisarmDC = nRoll + nDisarmDCMod;
                pPlaceable->m_bTrapDetectable = TRUE;
                pPlaceable->m_bTrapDisarmable = FALSE;
                pPlaceable->m_bTrapOneShot = TRUE;
                pPlaceable->m_nOwnerDemolitionsSkill =
                    m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, FALSE);
                unsigned long oidPlaceable = pPlaceable->GetId();
                CSWSArea* pArea = GetArea();
                pArea->m_lstTrapObjectIds.Add(oidPlaceable);

                pTrigger = new CSWSTrigger(OBJECT_INVALID);
                pTrigger->SetCreator(GetId());
                pTrigger->m_nTrapType = nTrapType;
                pTrigger->m_bMineTrigger = TRUE;
                pTrigger->SetPosition(pPlaceable->m_vPosition, TRUE, TRUE, FALSE);
                pTrigger->m_nFaction = GetFactionId();
                pTrigger->m_bTrapDisarmable = FALSE;
                pTrigger->m_bTrapDetectable = FALSE;
                pTrigger->m_bPartyInteract = FALSE;
                int nTrapName = 0;
                g_pRules->Get2DArrays()->GetTraps()->GetINTEntry(nTrapType, "TrapName", &nTrapName);
                unsigned long oidTrigger = pTrigger->GetId();
                pTrigger->m_lsName.m_dwStringRef = nTrapName;
                CSWSArea* pTriggerArea = GetArea();
                pTriggerArea->m_lstTrapObjectIds.Add(oidTrigger);
                CSWSArea* pGeometryArea = GetArea();
                pTrigger->CreateNewGeometry(2.0f, vTarget, pGeometryArea);
                pTrigger->AddToArea(GetArea(), pPlaceable->m_vPosition.m_fX,
                    pPlaceable->m_vPosition.m_fY, pPlaceable->m_vPosition.m_fZ, TRUE);
                pPlaceable->m_pTrapTrigger = pTrigger;
                if (!bCriticalFailure)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                    pEvent->SetInteger(0, TRUE);
                    pEvent->SetInteger(1, FALSE);
                    pEvent->SetInteger(2, TRUE);
                    pEvent->SetInteger(3, nMineStrength);
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, SET_MINE_TRIGGER_DELAY, GetId(), oidTarget,
                        CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
            }
        }
        else
        {
            pTrigger = new CSWSTrigger(OBJECT_INVALID);
            pTrigger->SetCreator(GetId());
            pTrigger->m_nTrapType = nTrapType;
            pTrigger->m_asScripts[CSWSTrigger::SCRIPT_ONTRAPTRIGGERED] = sTrapScript;
            pTrigger->m_nTrapDetectDC = nDetectDCMod + nRoll;
            pTrigger->m_nTrapDisarmDC = nRoll + nDisarmDCMod;
            pTrigger->m_bMineTrigger = TRUE;
            pTrigger->m_bTrapDetectable = TRUE;
            pTrigger->m_bTrapDisarmable = TRUE;
            pTrigger->SetPosition(m_vPosition, TRUE, TRUE, FALSE);

            pTrigger->m_nFaction = GetFactionId();
            pTrigger->m_nOwnerDemolitionsSkill =
                m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, FALSE);
            int nTrapName = 0;
            g_pRules->Get2DArrays()->GetTraps()->GetINTEntry(nTrapType, "TrapName", &nTrapName);
            pTrigger->m_lsName.m_dwStringRef = nTrapName;
            CSWSArea* pGeometryArea = GetArea();
            pTrigger->CreateNewGeometry(2.0f, vTarget, pGeometryArea);
            pTrigger->AddToArea(GetArea(), m_vPosition.m_fX, m_vPosition.m_fY, m_vPosition.m_fZ, TRUE);
            unsigned long oidTrigger = pTrigger->GetId();
            CSWSArea* pArea = GetArea();
            pArea->m_lstTrapObjectIds.Add(oidTrigger);
        }
        if (GetItemRepository(TRUE)->GetItemInRepository(pItem, TRUE))
        {
            if (pItem->m_nStackSize > 1)
                --pItem->m_nStackSize;
            else if (RemoveItem(pItem, TRUE, TRUE, NULL) == TRUE)
            {
                CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
                if (pPlayer != NULL)
                    g_pAppManager->GetServerExoApp()->GetSWSMessage()->SendServerPlayerItemUpdate_DestroyItem(pPlayer, pItem->GetId());
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), pItem->GetId(), CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
            }
        }
        CScriptEvent* pLoseEvent = new CScriptEvent;
        pLoseEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_LOSE_ITEM;
        pLoseEvent->SetObjectID(0, pItem->GetId());
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, GetId(), g_pAppManager->GetServerExoApp()->GetModule()->GetId(),
            CServerAIMaster::EVENT_SIGNAL_EVENT, pLoseEvent);

        if (bCriticalFailure)
        {
            if (pTarget == NULL && pTrigger != NULL)
            {
                pTrigger->m_bMineTrigger = TRUE;
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
                pEvent->SetObjectID(0, GetId());
                pEvent->SetInteger(0, TRUE);
                pEvent->SetInteger(1, TRUE);
                pEvent->SetInteger(2, TRUE);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), pTrigger->GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
            else if (pTarget->AsSWSPlaceable() != NULL || pTarget->AsSWSDoor() != NULL)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                pEvent->SetInteger(0, TRUE);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
    }

    CSWCCMessageData* pData = new CSWCCMessageData;
    pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, GetId());
    if (pTarget != NULL && (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE ||
                           pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR))
        pData->SetObjectID(CSWCCMessageData::SKILL_DATA_TARGET, pTarget->m_nId);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, CSWCCMessageData::SKILL_DATA_SET_STRREF);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, nRoll - nSkill);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, nSkill);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, nSetDC);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, bTakeTwenty);
    if (bSuccess)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_SUCCESS);
    else if (bTakeTwenty)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_RETRY);
    else if (bCriticalFailure)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_CRITICAL_FAILURE);
    else
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_FAILED);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_DEMOLITIONS_STRREF);
    BroadcastSkillData(pData);
    BroadcastVoiceChat(bSuccess ? VOICE_SET_MINE_SUCCESS : VOICE_SET_MINE_FAILURE);
    ClearActivities(ACTIVITY_DETECT | ACTIVITY_DIALOG | ACTIVITY_REST);
    RemoveCombatInvisibilityEffects();
    StopGuiTimingBar();
    m_bMineActionStarted = FALSE;
    m_bUnlockObjectFacingSet = FALSE;
    return ACTION_COMPLETE;
}
// PC 0x00550620
unsigned char CSWSCreature::CanEquipItem(CSWSItem* pItem,
                                        unsigned long* pnSlot,
                                        int bCheckRestrictions,
                                        int bLoading, int bFeedback)
{
    if (bCheckRestrictions == 1 && m_pStats->m_bIsPC == 1 && !bLoading)
    {
        unsigned char nLevel = m_pStats->GetLevel(0);
        if (nLevel < pItem->GetMinEquipLevel() &&
            g_pAppManager->GetServerExoApp()->GetServerInfo()
                ->m_bItemLevelRestrictions)
        {
            if (bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_LEVEL_RESTRICTION, NULL);
            }
            return 0;
        }
    }

    unsigned char nResult;
    if (!pItem->m_bNonEquippable && CheckProficiencies(pItem, *pnSlot))
    {
        if (!CheckItemAlignmentRestrictions(pItem))
        {
            if (bCheckRestrictions == 1 && bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_ALIGNMENT_RESTRICTION, NULL);
            }
            return 0;
        }
        else if (!CheckItemClassRestrictions(pItem))
        {
            if (bCheckRestrictions == 1 && bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_CLASS_RESTRICTION, NULL);
            }
            return 0;
        }
        else if (!CheckItemRaceRestrictions(pItem, 1))
        {
            if (bCheckRestrictions == 1 && bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_RACE_RESTRICTION, NULL);
            }
            return 0;
        }
        else if (!CheckItemFeatRestrictions(pItem))
        {
            if (bCheckRestrictions == 1 && bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_FEAT_RESTRICTION, NULL);
            }
            return 0;
        }
        else if (!CheckItemGenderRestrictions(pItem))
        {
            if (bCheckRestrictions == 1 && bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_GENDER_RESTRICTION, NULL);
            }
            return 0;
        }
        else if (!CheckItemPCRestrictions(pItem))
        {
            if (bCheckRestrictions == 1 && bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_PC_RESTRICTION, NULL);
            }
            return 0;
        }
        else if (!CheckItemAttributeRestrictions(pItem))
        {
            if (bCheckRestrictions == 1 && bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_ATTRIBUTE_RESTRICTION, NULL);
            }
            return 0;
        }
        else if (!CheckItemCNPCRestrictions(pItem))
        {
            if (bCheckRestrictions == 1 && bFeedback == 1)
            {
                SendFeedbackMessage(FEEDBACK_ITEM_CNPC_RESTRICTION, NULL);
            }
            return 0;
        }
        else
        {
            switch (*pnSlot)
            {
            case INVENTORY_SLOT_RIGHTHAND:
            case INVENTORY_SLOT_LEFTHAND:
                nResult = CanEquipWeapon(pItem, pnSlot, bCheckRestrictions, bFeedback);
                break;
            case INVENTORY_SLOT_RIGHTHAND2:
            case INVENTORY_SLOT_LEFTHAND2:
                nResult = CanEquipWeapon2(pItem, pnSlot, bCheckRestrictions, bFeedback);
                break;
            default:
                nResult = CanEquipMiscellaneous(pItem, pnSlot,
                                               bCheckRestrictions, bFeedback);
                break;
            }
            unsigned long nSlot = *pnSlot;
            if (nSlot == INVENTORY_SLOT_RIGHTHAND2)
            {
                *pnSlot = INVENTORY_SLOT_RIGHTHAND2 | INVENTORY_SLOT_RIGHTHAND;
            }
            else if (nSlot == INVENTORY_SLOT_LEFTHAND2)
            {
                *pnSlot = INVENTORY_SLOT_LEFTHAND2 | INVENTORY_SLOT_LEFTHAND;
            }
            int bCanEquip = FALSE;
            CSWBaseItem* pBaseItem =
                g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType());
            if (pBaseItem != NULL)
            {
                bCanEquip = (pBaseItem->GetEquipableSlots() & *pnSlot) != 0;
            }
            *pnSlot = nSlot;
            if (bCanEquip != FALSE)
            {
                return nResult;
            }
        }
    }
    return 0;
}
// PC 0x0054AFD0
unsigned char CSWSCreature::CanEquipMiscellaneous(CSWSItem* pItem,
                                                  unsigned long* pnSlot,
                                                  int bFeedback, int bUnknown)
{
    if (CheckProficiencies(pItem, *pnSlot) == 0)
    {
        if (bFeedback == 1 && bUnknown == 1)
        {
            SendFeedbackMessage(FEEDBACK_ITEM_PROFICIENCY_REQUIRED, NULL);
        }
        return 0;
    }

    if (m_pInventory->GetItemInSlot(*pnSlot) != 0)
    {
        if (bFeedback == 1 && bUnknown == 1)
        {
            SendFeedbackMessage(FEEDBACK_EQUIPPED_ITEM_SWAPPED, NULL);
        }
        return 2;
    }

    return 1;
}
// PC 0x00544DC0
int CSWSCreature::CheckProficiencies(CSWSItem* pItem, unsigned long nSlot)
{
    int i;

    if (g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->GetEquipableSlots() == 0)
    {
        return 0;
    }

    if (m_pStats->HasFeat(CSWFeat::FEAT_PROFICIENCY_ALL))
    {
        return 1;
    }

    for (i = 0; i < g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nRequiredFeatCount; i++)
    {
        if (m_pStats->HasFeat(g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->GetRequiredFeat(i)) == 0)
        {
            return 0;
        }
    }

    return 1;
}
// PC 0x0054B050
int CSWSCreature::AcquireItem(CSWSItem** ppItem, unsigned long oidCaller,
                              unsigned long oidRepository, int bFlag)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CSWSItem* pContainer = NULL;
    CItemRepository* pRepository = NULL;
    CGameObject* pPossessor;
    int bResult;

    if ((*ppItem)->m_oidPossessor != oidCaller)
    {
        pContainer = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(
            (*ppItem)->m_oidPossessor);
        if (pContainer == NULL || pContainer->m_oidPossessor != oidCaller)
        {
            return 0;
        }
        oidCaller = (*ppItem)->m_oidPossessor;
    }

    if (oidRepository == OBJECT_INVALID)
    {
        pRepository = GetItemRepository(1);
    }
    else
    {
        pContainer =
            g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidRepository);
        if (pContainer != NULL)
        {
            pRepository = pContainer->m_pRepository;
        }
    }

    pPossessor = g_pAppManager->m_pServerExoApp->GetGameObject(oidCaller);
    if (pPossessor != NULL)
    {
        if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
        {
            pPossessor->AsSWSCreature()->RemoveItem(*ppItem, 0, 1, NULL);
        }
        else if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
        {
            pPossessor->AsSWSPlaceable()->RemoveItem(*ppItem);
        }
        else if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_ITEM &&
                 pPossessor->AsSWSItem()->m_pRepository != NULL)
        {
            CSWSItem* pItem = pPossessor->AsSWSItem();
            pItem->m_pRepository->RemoveItem(*ppItem);
        }
    }
    else if (pContainer != NULL &&
             g_pRules->m_pBaseItems->GetBaseItem((*ppItem)->GetBaseItemType())
                 ->m_bIsContainer &&
             g_pRules->m_pBaseItems->GetBaseItem(pContainer->GetBaseItemType())
                 ->m_bIsContainer)
    {
        return 0;
    }

    bResult = pRepository->AddItem(ppItem, 1, bFlag, 1);
    if (bResult == 1)
    {
        if (pPossessor == NULL && *ppItem != NULL)
        {
            (*ppItem)->RemoveFromArea();
        }
        if (oidRepository == CGameObjectArray::INVALIDOBJID)
        {
            if (*ppItem != NULL)
            {
                (*ppItem)->SetPossessor(m_nId, 1, bFlag, OBJECT_INVALID);
            }
        }
        else if (*ppItem != NULL)
        {
            (*ppItem)->SetPossessor(oidRepository, 1, bFlag, OBJECT_INVALID);
        }
    }
    return bResult;
}
// PC 0x0054B230
int CSWSCreature::AIActionRandomWalk(CSWSObjectActionNode* pAction)
{
    float fWait = NWSCREATURE_RANDOM_WALK_WAIT;
    float fX = *(float*)&pAction->m_apParam[0];
    float fY = *(float*)&pAction->m_apParam[1];
    float fZ = *(float*)&pAction->m_apParam[2];
    unsigned long oidArea = (unsigned long)pAction->m_apParam[3];
    CSWSArea* pArea = g_pAppManager->GetServerExoApp()->GetAreaByGameObjectID(oidArea);
    if (pArea == NULL)
        return ACTION_FAILED;

    while (!m_lstActions.IsEmpty())
        delete m_lstActions.RemoveHead();

    if (m_nAILevel > CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE)
    {
        AddActionToFront(ACTION_RANDOM_WALK, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY,
            ACTION_PARAM_FLOAT, &fZ, ACTION_PARAM_OBJECT, &oidArea,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_WAIT, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &fWait, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL);

        Vector vDestination(fX, fY, 0.0f);
        Vector vDirection;
        vDestination.m_fX += rand() % NWSCREATURE_RANDOM_WALK_RANGE -
            NWSCREATURE_RANDOM_WALK_OFFSET;
        vDestination.m_fY += rand() % NWSCREATURE_RANDOM_WALK_RANGE -
            NWSCREATURE_RANDOM_WALK_OFFSET;
        vDestination.m_fZ = pArea->ComputeHeight(vDestination, 1, NULL);
        vDirection = vDestination - m_vPosition;
        float fDistance = magnitude(vDirection);
        float fOriginalDistance = fDistance;
        float fMinimumDistance = fDistance * NWSCREATURE_RANDOM_WALK_MINIMUM_FRACTION;
        vDirection = normalize(vDirection);
        pArea->m_pPathfindInformation = m_pPathfindInformation;
        float fRadius = m_pPathfindInformation->m_fStepScale;
        float fWidth = m_pPathfindInformation->m_fHalfStep;
        int nAttempts = NWSCREATURE_RANDOM_WALK_MAX_ATTEMPTS;
        while (pArea->TestDirectLine(m_vPosition, vDestination,
                   fRadius, fWidth, 0, NULL) != CSWSArea::DIRECT_LINE_CLEAR)
        {
            fDistance *= NWSCREATURE_RANDOM_WALK_SHORTEN_FACTOR;
            if (fDistance < fMinimumDistance)
            {

                rand();
                vDirection = cross(Vector(0.0f, 0.0f, 1.0f), vDirection);
                vDirection = cross(Vector(0.0f, 0.0f, 1.0f), vDirection);
                vDirection = -vDirection;
                fDistance = fOriginalDistance;
            }
            vDestination = m_vPosition + vDirection * fDistance;
            if (fDistance < NWSCREATURE_RANDOM_WALK_MINIMUM_DISTANCE || nAttempts-- < 0)
                return ACTION_COMPLETE;
            fRadius = m_pPathfindInformation->m_fStepScale;
            fWidth = m_pPathfindInformation->m_fHalfStep;
        }
        AddMoveToPointActionToFront(pAction->m_nGroupId, vDestination,
            oidArea, OBJECT_INVALID, 0, 0.0f, 0.0f,
            0, 0, 0, 0, 1, 0, Vector(), 0, 0, 0);
        return ACTION_COMPLETE;
    }
    else
    {
        fWait = NWSCREATURE_RANDOM_WALK_IDLE_WAIT;
        AddActionToFront(ACTION_RANDOM_WALK, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &fX, ACTION_PARAM_FLOAT, &fY,
            ACTION_PARAM_FLOAT, &fZ, ACTION_PARAM_OBJECT, &oidArea,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_WAIT, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &fWait, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL);
    }
    return ACTION_COMPLETE;
}
// PC 0x0054B740
int CSWSCreature::HandleSubAreaStateChanges(
    CExoArrayList<unsigned long>* pNewSubAreas,
    CExoArrayList<unsigned long>* pIntersectingSubAreas,
    int bCheckTriggerType, int bRunScripts)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    int bChanged = 0;
    int i;
    for (i = 0; i < m_lstSubAreas.num; i++)
    {
        CGameObject* pObject;
        if (!pNewSubAreas->Contains(m_lstSubAreas[i]))
        {
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                    m_lstSubAreas[i], &pObject) == CGameObjectArray::SUCCESS)
            {
                if (pObject->AsSWSDoor() == NULL && bRunScripts != 0)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT;
                    pEvent->SetObjectID(0, m_nId);
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, m_nId, m_lstSubAreas[i],
                        CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
                bChanged = 1;
            }
        }
    }
    for (i = 0; i < pNewSubAreas->num; i++)
    {
        unsigned long oidSubArea = (*pNewSubAreas)[i];
        if (!m_lstSubAreas.Contains(oidSubArea))
        {
            int bEntered = 1;
            CGameObject* pObject;
            if (pObjectArray->GetGameObject(oidSubArea, &pObject) ==
                    CGameObjectArray::SUCCESS && bRunScripts != 0)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                if ((bCheckTriggerType != 0 && pObject->AsSWSTrigger() != NULL &&
                     pObject->AsSWSTrigger()->m_nTriggerType != 0) ||
                    pObject->AsSWSDoor() != NULL)
                {
                    if (m_bInParty != 0 &&
                        g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(
                            g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId())) == 0)
                    {
                        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK;
                    }
                    else if (m_bInParty != 0)
                    {
                        bEntered = 0;
                    }
                }
                else
                {
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
                }
                if (bEntered != 0)
                {
                    pEvent->SetObjectID(0, m_nId);
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, m_nId, oidSubArea, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
                else
                {
                    delete pEvent;
                }
            }
            bChanged = bEntered;
        }
    }
    if (pIntersectingSubAreas != NULL)
    {
        for (i = 0; i < pIntersectingSubAreas->num; i++)
        {
            unsigned long oidSubArea = (*pIntersectingSubAreas)[i];
            if (!m_lstSubAreas.Contains(oidSubArea))
            {
                if (pNewSubAreas->Contains(oidSubArea))
                    continue;
                CGameObject* pObject;
                if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                        oidSubArea, &pObject) != CGameObjectArray::SUCCESS ||
                    bRunScripts == 0)
                    continue;
                CScriptEvent* pEnterEvent = new CScriptEvent;
                if ((bCheckTriggerType != 0 && pObject->AsSWSTrigger() != NULL &&
                     pObject->AsSWSTrigger()->m_nTriggerType != 0) ||
                    pObject->AsSWSDoor() != NULL)
                    pEnterEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK;
                else
                    pEnterEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
                pEnterEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidSubArea, CServerAIMaster::EVENT_SIGNAL_EVENT, pEnterEvent);
                CScriptEvent* pExitEvent = new CScriptEvent;
                pExitEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT;
                pExitEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidSubArea, CServerAIMaster::EVENT_SIGNAL_EVENT, pExitEvent);
            }
            else
            {
                if (!pNewSubAreas->Contains(oidSubArea))
                    continue;
                CGameObject* pObject;
                if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                        oidSubArea, &pObject) != CGameObjectArray::SUCCESS ||
                    bRunScripts == 0)
                    continue;
                CScriptEvent* pExitEvent = new CScriptEvent;
                pExitEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT;
                pExitEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidSubArea, CServerAIMaster::EVENT_SIGNAL_EVENT, pExitEvent);
                CScriptEvent* pEnterEvent = new CScriptEvent;
                if ((bCheckTriggerType != 0 && pObject->AsSWSTrigger() != NULL &&
                     pObject->AsSWSTrigger()->m_nTriggerType != 0) ||
                    pObject->AsSWSDoor() != NULL)
                    pEnterEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK;
                else
                    pEnterEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
                pEnterEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidSubArea, CServerAIMaster::EVENT_SIGNAL_EVENT, pEnterEvent);
            }
        }
    }
    return bChanged;
}
// PC 0x0054BD50
int CSWSCreature::WalkUpdateLocationDistance(float fDistance, Vector* pvPosition,
    Vector* pvDirection, CExoArrayList<unsigned long>* pIntersectingSubAreas)
{
    CSWSArea* pArea = GetArea();
    if (pArea == NULL)
        return 0;
    int nPoints = m_pPathfindInformation->m_nWayPointCount;
    int nCurrentPoint = m_pPathfindInformation->m_nField_9C;
    float fDistanceLeft = fDistance;
    Vector vPosition = m_vPosition;
    Vector vNext;
    Vector vDifference;
    Vector vStart;
    Vector vEnd;
    int bAtEnd = 0;
    Vector vOldDirection = m_vOrientation;
    *pvDirection = vOldDirection;
    int nPoint = nCurrentPoint;
    if (nCurrentPoint == nPoints || nPoints == 0)
        bAtEnd = 1;
    if (m_pPathfindInformation->m_oidPathfindTargetCreature != OBJECT_INVALID)
    {
        unsigned long oidTarget = m_pPathfindInformation->m_oidPathfindTargetCreature;
        Vector vTarget;
        float fUseRange = 0.0f;
        if (GetUseRange(oidTarget, vTarget, fUseRange, 0) != 0)
        {
            Vector vDelta = m_vPosition - vTarget;
            if (vDelta * vDelta <= fUseRange * fUseRange)
            {
                SetAnimation(GetPauseReadyAnimation());
                *pvPosition = m_vPosition;
                return 1;
            }
        }
    }
    if (m_pPathfindInformation->m_nField_240 != 0)
    {
        Vector vTarget;
        float fUseRange = 0.0f;
        if (m_pPathfindInformation->m_oidPathfindTargetCreature != OBJECT_INVALID &&
            GetUseRange(m_pPathfindInformation->m_oidPathfindTargetCreature,
                        vTarget, fUseRange, 0) != 0 &&
            m_pPathfindInformation->m_fStartDistance > fUseRange)
        {
            Vector vDelta = m_vPosition - vTarget;
            Vector vEye = m_vPosition;
            vEye.m_fZ += 1.5f;
            Vector vTargetEye = vTarget;
            vTargetEye.m_fZ += 1.5f;
            Vector vHit;
            unsigned long oidHit;
            if (vDelta * vDelta <= m_pPathfindInformation->m_fStartDistance *
                                   m_pPathfindInformation->m_fStartDistance)
            {
                unsigned long oidTarget = m_pPathfindInformation->m_oidPathfindTargetCreature;
                if (GetArea()->ClearLineOfSight(vEye, vTargetEye, &vHit, &oidHit, GetId(),
                        oidTarget, 0, 0) != 0 &&
                    GetArea()->NoNonWalkPolysDetailed(vEye, vTargetEye,
                        vEye.m_fZ - 0.01f, vEye.m_fZ + 0.01f,
                        m_pPathfindInformation->GetStepScale(), OBJECT_INVALID,
                        oidHit, 1, NULL, 0, NULL, 0) == 1)
                {
                    SetAnimation(GetPauseReadyAnimation());
                    *pvPosition = m_vPosition;
                    return 1;
                }
            }
        }
        if (nPoints > 2)
        {
            Vector vLast(m_pPathfindInformation->m_pWayPoints[nPoints - 2],
                         m_pPathfindInformation->m_pWayPoints[nPoints - 1], 0.0f);
            Vector vCurrent = m_vPosition;
            Vector vDelta;
            vDelta = vLast - vCurrent;
            vDelta.m_fZ = 0.0f;
            float fRange = m_pPathfindInformation->m_fStartDistance;
            if (vDelta * vDelta <= fRange * fRange)
            {
                Vector vEye = m_vPosition;
                vEye.m_fZ += 1.5f;
                Vector vLastEye = vLast;
                vLastEye.m_fZ = GetArea()->ComputeHeight(vLastEye, 0, NULL) + 1.5f;
                Vector vHit;
                unsigned long oidHit;
                if (GetArea()->ClearLineOfSight(vEye, vLastEye, &vHit, &oidHit,
                                                GetId(), OBJECT_INVALID, 0, 0) != 0 &&
                    GetArea()->NoNonWalkPolysDetailed(vEye, vLastEye,
                        vEye.m_fZ - 0.01f, vEye.m_fZ + 0.01f,
                        m_pPathfindInformation->GetStepScale(), OBJECT_INVALID,
                        oidHit, 1, NULL, 0, NULL, 0) == 1)
                {
                    SetAnimation(GetPauseReadyAnimation());
                    *pvPosition = m_vPosition;
                    return 1;
                }
            }
        }
    }

    unsigned long oidBlockingCreature;
    int bBlocked = 0;
    while (!bBlocked && nPoint < nPoints)
    {
        vNext = Vector(m_pPathfindInformation->m_pWayPoints[nPoint],
                       m_pPathfindInformation->m_pWayPoints[nPoint + 1], 0.0f);
        vDifference = vNext - vPosition;
        vDifference.m_fZ = 0.0f;
        float fSegmentSquared = vDifference * vDifference;
        float fDistanceSquared = fDistanceLeft * fDistanceLeft;
        float fStep;
        if (fDistanceSquared > fSegmentSquared && fSegmentSquared != 0.0f)
            fStep = sqrt(fSegmentSquared);
        else
            fStep = fDistanceLeft;
        vStart = vPosition;
        vStart.m_fZ = pArea->ComputeHeight(vStart, 1, NULL);
        vEnd = vPosition + fStep * normalize(vDifference);
        vEnd.m_fZ = pArea->ComputeHeight(vEnd, 1, NULL);
        Vector vHit;
        if (fSegmentSquared > 0.0f && fStep > 0.0f &&
            WalkUpdateLocationTestDistance(vStart, vEnd, &vHit, &oidBlockingCreature) == 0)
        {
            bBlocked = 1;
        }
        else
        {
            if (fDistanceSquared <= fSegmentSquared)
            {
                if (fSegmentSquared != 0.0f)
                {
                    Vector vNewPosition = vPosition + fDistanceLeft * normalize(vDifference);
                    pArea->IntersectingLineSegment(vPosition, vNewPosition, pIntersectingSubAreas);
                    *pvDirection = normalize(vDifference);
                    vPosition = vNewPosition;
                    vPosition.m_fZ = GetArea()->ComputeHeight(vPosition, 1, NULL);
                }
                bAtEnd = 0;
                break;
            }
            pArea->IntersectingLineSegment(vPosition, vNext, pIntersectingSubAreas);
            fDistanceLeft -= magnitude(vDifference);
            vPosition = vNext;
            vPosition.m_fZ = GetArea()->ComputeHeight(vPosition, 1, NULL);
            nPoint += 2;
            m_pPathfindInformation->m_nField_9C = nPoint;
            if (nPoint == nPoints)
            {
                if (vDifference * vDifference != 0.0f)
                    *pvDirection = normalize(vDifference);
                bAtEnd = 1;
                break;
            }
        }
    }
    if (bBlocked)
    {
        int nBumpCount = m_pPathfindInformation->m_nBumpCount + 1;
        m_pPathfindInformation->m_nBumpCount = nBumpCount;
        if (nBumpCount > 5)
        {
            m_pPathfindInformation->m_nWayPointCount = 0;
            delete[] m_pPathfindInformation->m_pWayPoints;
            m_pPathfindInformation->m_pWayPoints = NULL;
            SetAnimation(GetPauseReadyAnimation());
            m_pPathfindInformation->HandlePathfindError(1,
                "aborted walking, Maximum number of bumps happened");
            *pvPosition = m_vPosition;
            return 1;
        }
        if (m_pPathfindInformation->m_oidPathfindTargetCreature != OBJECT_INVALID &&
            oidBlockingCreature == m_pPathfindInformation->m_oidPathfindTargetCreature)
        {
            m_pPathfindInformation->m_nWayPointCount = 0;
            delete[] m_pPathfindInformation->m_pWayPoints;
            m_pPathfindInformation->m_pWayPoints = NULL;
            SetAnimation(GetPauseReadyAnimation());
            *pvPosition = m_vPosition;
            return 1;
        }
        CSWSCreature* pBlockingCreature = g_pAppManager->GetServerExoApp()
            ->GetCreatureByGameObjectID(oidBlockingCreature);
        if (pBlockingCreature != NULL &&
            (oidBlockingCreature != m_pPathfindInformation->m_oidBlockingCreature ||
             (m_pPathfindInformation->m_vBlockingCreaturePosition - pBlockingCreature->m_vPosition) *
             (m_pPathfindInformation->m_vBlockingCreaturePosition - pBlockingCreature->m_vPosition) >
                0.00001f))
        {
            *pvPosition = m_vPosition;
            CAvoidCreature avoidCreature;
            avoidCreature.m_nUnused = m_pPathfindInformation->m_nField_244;
            int nResult = avoidCreature.PlotPathAroundCreature(this, pBlockingCreature);
            if (nResult == CAvoidCreature::AVOIDCREATURE_FAILED)
            {
                m_pPathfindInformation->SetBlockingCreature(OBJECT_INVALID);
                SetAnimation(GetPauseReadyAnimation());
                m_pPathfindInformation->HandlePathfindError(1,
                    "aborted walking, we are totaly blocked. can't get around this creature at all.");
                *pvPosition = m_vPosition;
                if (GetId() == g_pAppManager->GetServerExoApp()->GetPlayerCreatureId())
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetCombatMessage(47859);
                return 1;
            }
            if (m_pPathfindInformation->m_nField_244 != avoidCreature.m_nUnused)
                m_pPathfindInformation->m_nField_244 = avoidCreature.m_nUnused;
            if (m_vOrientation * pBlockingCreature->m_vOrientation < 0.0f)
                pBlockingCreature->m_pPathfindInformation->m_nField_244 =
                    m_pPathfindInformation->m_nField_244;
            else
                pBlockingCreature->m_pPathfindInformation->m_nField_244 =
                    m_pPathfindInformation->m_nField_244 == 0;
            if (nResult == CAvoidCreature::AVOIDCREATURE_ENDINHEX)
            {
                SetAnimation(GetPauseReadyAnimation());
                return 1;
            }
            return 0;
        }
        SetAnimation(GetPauseReadyAnimation());
        m_pPathfindInformation->HandlePathfindError(1,
            "aborted walking, Bumped into this creature at this position already.");
        *pvPosition = m_vPosition;
        return 1;
    }
    if (nPoints == 4)
    {
        vNext = Vector(m_pPathfindInformation->m_pWayPoints[2],
                       m_pPathfindInformation->m_pWayPoints[3], 0.0f);
        vDifference = vNext - m_vPosition;
        vDifference.m_fZ = 0.0f;
        if (AnimationWalking(GetAnimation()) && vDifference * vDifference <= 4.0f &&
            vOldDirection.m_fX * pvDirection->m_fX +
            vOldDirection.m_fY * pvDirection->m_fY < -0.707f)
        {
            *pvDirection = Vector(-pvDirection->m_fX, -pvDirection->m_fY, 0.0f);
            if (m_nAnimation != CREATURE_ANIMATION_WALK_BACKWARD)
                SetAnimation(CREATURE_ANIMATION_WALK_BACKWARD);
        }
    }
    *pvPosition = vPosition;
    return bAtEnd;
}
// PC 0x0054C9B0
int CSWSCreature::AIActionPickUpItem(CSWSObjectActionNode* pAction)
{
    Vector vDestination;
    Vector vSafePosition;
    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);

    unsigned long oidItem = (unsigned long)pAction->m_apParam[0];
    unsigned long oidRepository = (unsigned long)pAction->m_apParam[1];
    unsigned char bUnknown = (unsigned char)pAction->m_apParam[3];
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    CServerExoAppInternal* pAppInternal = g_pAppManager->GetServerExoApp()->m_pcExoAppInternal;
    CSWSPlayer* pPlayer = pAppInternal->GetClientObjectByObjectId(m_nId);
    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);

    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1 || pItem == NULL)
    {
        if (pMessage != NULL && pPlayer != NULL)
            pMessage->SendServerToPlayerInventory_PickupCancel(pPlayer->m_nPlayerId, oidItem);
        m_bPickupAnimationPlayed = 0;
    }
    else if (pItem->m_oidPossessor == OBJECT_INVALID)
    {
        Vector vDelta = m_vPosition - pItem->m_vPosition;
        if (vDelta * vDelta > 1.21f)
        {
            int bRun;
            if (vDelta * vDelta >= 4.0f)
                bRun = 1;
            AddActionToFront(ACTION_PICK_UP_ITEM, pAction->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidRepository,
                ACTION_PARAM_INTEGER, &bUnknown, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidItem, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL);
            vDelta = normalize(m_vPosition - pItem->m_vPosition);
            vDestination = pItem->m_vPosition + vDelta * 1.0f;
            if (GetArea()->ComputeSafeLocation(vDestination, 0.5f,
                    GetPathfindInformation(), 1, &vSafePosition) == 1)
                vDestination = vSafePosition;
            else
                vDestination = pItem->m_vPosition;
            AddMoveToPointActionToFront(pAction->m_nGroupId, vDestination,
                m_oidArea, OBJECT_INVALID, bRun, 0.0f, 0.0f,
                0, 0, 0, 0, 0, 0, Vector(0.0f, 0.0f, 0.0f), 0, 0, 0);
            return ACTION_COMPLETE;
        }
        else if (!m_bPickupAnimationPlayed)
        {
            m_bPickupAnimationPlayed = 1;
            int bLoop = 1;
            float fSpeed = 1.0f;
            float fDuration = 1.5f;
            int nAnimation = vDestination.m_fZ > 1.0f ?
                CREATURE_ANIMATION_LOOPING_GET_MID : CREATURE_ANIMATION_LOOPING_GET_LOW;
            AddActionToFront(ACTION_PICK_UP_ITEM, pAction->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_OBJECT, &oidRepository,
                ACTION_PARAM_INTEGER, &bUnknown, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            AddActionToFront(ACTION_PLAY_ANIMATION, pAction->m_nGroupId,
                ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
                ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLoop,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL);
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
                ACTION_PARAM_OBJECT, &oidItem, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL);
            return ACTION_COMPLETE;
        }
        else if (AcquireItem(&pItem, OBJECT_INVALID,
                     oidRepository, bUnknown) == 1)
        {
            if (pPlayer != NULL && pMessage != NULL)
                pMessage->SendServerToPlayerInventory_Pickup(pPlayer->m_nPlayerId, oidItem);
            m_bPickupAnimationPlayed = 0;
            return ACTION_COMPLETE;
        }
        else
        {
            if (pPlayer != NULL && pMessage != NULL)
                pMessage->SendServerToPlayerInventory_PickupCancel(pPlayer->m_nPlayerId, oidItem);
            m_bPickupAnimationPlayed = 0;
            return ACTION_FAILED;
        }
    }
    return ACTION_FAILED;
}
// PC 0x0054CE90
void CSWSCreature::ClearVisibleList()
{
    int i;
    CSWVisibilityNode* pNode;

    for (i = m_lstVisibilityList.num - 1; i >= 0; i--)
    {
        if (m_lstVisibilityList[i]->m_oidObject != m_nId)
        {
            pNode = m_lstVisibilityList[i];
            m_lstVisibilityList.DelIndex(i);
            if (pNode != NULL)
            {
                delete pNode;
            }
        }
    }
}
// PC 0x00551610
int CSWSCreature::UpdateSubareasOnMoveTo(
    const Vector& vFrom, const Vector& vTo, int bUseIntersectList,
    CExoArrayList<unsigned long>* pIntersectList, int bFlag)
{
    CExoArrayList<unsigned long> lstNewSubAreas;
    g_pAppManager->GetServerExoApp()->GetObjectArray();
    CSWSArea* pArea = GetArea();
    if (pArea == NULL)
        return FALSE;

    int bChanged = FALSE;
    int bWasInSubAreas = m_lstSubAreas.num > 0;
    int bInSubAreas = pArea->InSubAreas(vTo, &lstNewSubAreas);
    if (!bUseIntersectList)
    {
        CExoArrayList<unsigned long> lstIntersectingSubAreas;
        pArea->IntersectingLineSegment(vFrom, vTo, &lstIntersectingSubAreas);
        if ((bWasInSubAreas || bInSubAreas) &&
            HandleSubAreaStateChanges(&lstNewSubAreas, &lstIntersectingSubAreas,
                                      bFlag, TRUE) == TRUE)
        {
            m_lstSubAreas = lstNewSubAreas;
            bChanged = TRUE;
        }
    }
    else
    {
        if ((bWasInSubAreas || bInSubAreas) &&
            HandleSubAreaStateChanges(&lstNewSubAreas, pIntersectList,
                                      bFlag, TRUE) == TRUE)
        {
            m_lstSubAreas = lstNewSubAreas;
            bChanged = TRUE;
        }
    }
    return bChanged;
}
// PC 0x005517A0
int CSWSCreature::UpdateSubareasOnJumpPosition(const Vector& vPosition,
                                               unsigned long oidArea,
                                               int bRunScripts)
{
    CExoArrayList<unsigned long> lstNewSubAreas;
    CGameObjectArray* pObjectArray;
    CGameObject* pObject;
    CSWSArea* pArea;
    CSWSAreaOfEffectObject* pAreaOfEffect;
    int bReturn;
    int bWasInSubAreas;
    int bInSubAreas;

    pObjectArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
    if (g_bRunSubAreaScripts)
    {
        bRunScripts = 1;
    }

    if (oidArea != CGameObjectArray::INVALIDOBJID)
    {
        pObjectArray->GetGameObject(oidArea, &pObject);
        pArea = pObject->AsSWSArea();
    }
    else
    {
        pArea = NULL;
    }

    bReturn = 0;
    bWasInSubAreas = m_lstSubAreas.num > 0;
    if (pArea != NULL)
    {
        bInSubAreas = pArea->InSubAreas(vPosition, &lstNewSubAreas);
    }
    else
    {
        bInSubAreas = 0;
    }

    for (int i = 0; i < m_lstAreaOfEffectObjects.num; i++)
    {
        pAreaOfEffect = g_pAppManager->GetServerExoApp()
            ->GetAreaOfEffectByGameObjectID(m_lstAreaOfEffectObjects[i]);
        if (pAreaOfEffect != NULL)
        {
            pAreaOfEffect->JumpToPoint(pArea, vPosition);
            lstNewSubAreas.AddUnique(m_lstAreaOfEffectObjects[i]);
        }
    }

    if (bWasInSubAreas || bInSubAreas)
    {
        if (HandleSubAreaStateChanges(&lstNewSubAreas, NULL, 0, bRunScripts) == 1)
        {
            m_lstSubAreas = lstNewSubAreas;
            bReturn = 1;
        }
    }
    return bReturn;
}
// PC 0x00551970
int CSWSCreature::WalkUpdateLocation()
{
    if (m_nElapsedTime == 0)
        return CSWSObject::ACTION_IN_PROGRESS;

    float fRate;
    float fAnimationDistance;
    if (m_nAnimation == CREATURE_ANIMATION_WALK ||
        m_nAnimation == CREATURE_ANIMATION_WALK_BACKWARD)
    {
        fRate = GetWalkRate();
        C2DA* pAppearance = g_pRules->Get2DArrays()->m_pAppearance;
        pAppearance->GetFLOATEntry(
            m_cAppearanceInfo.m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_WALKDIST,
            &fAnimationDistance);
    }
    else if (m_nAnimation == CREATURE_ANIMATION_RUN)
    {
        fRate = GetRunRate();
        C2DA* pAppearance = g_pRules->Get2DArrays()->m_pAppearance;
        pAppearance->GetFLOATEntry(
            m_cAppearanceInfo.m_nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_RUNDIST,
            &fAnimationDistance);
    }
    fAnimationDistance *= 1000.0f;

    float fSpeedFactor = 1.0f;
    float fWalkDistance = ComputeTotalWalkDistance();
    float fDistanceScale = fRate * 1000.0f / 2000000.0f;
    if (fWalkDistance <= fDistanceScale)
    {
        float fFactor = fWalkDistance / fDistanceScale;
        float fNewSpeedFactor;
        if (fFactor >= 0.01f)
            fNewSpeedFactor = sqrt(fFactor);
        else
            fNewSpeedFactor = 0.1f;
        fSpeedFactor = fNewSpeedFactor - m_nElapsedTime * 0.001f;
    }
    if (m_fMovementRate <= 1.0f)
    {
        float fMaximumSpeedFactor = m_nElapsedTime * 0.001f + m_fMovementRate;
        if (fSpeedFactor >= fMaximumSpeedFactor)
            fSpeedFactor = fMaximumSpeedFactor;
    }
    if (fSpeedFactor <= 0.1f)
        fSpeedFactor = 0.1f;

    fRate = (fSpeedFactor + m_fMovementRate) * m_nElapsedTime * fRate *
            0.5f;
    fRate *= 0.000001f;
    m_fMovementRate = fSpeedFactor;
    if (NPCTurbo)
        fRate *= 15.0;

    Vector vPosition;
    Vector vDirection;
    CExoArrayList<unsigned long> lstIntersectingSubAreas;
    int nWayPoint = m_pPathfindInformation->m_nField_9C;
    int bAtEnd = WalkUpdateLocationDistance(fRate, &vPosition, &vDirection,
                                           &lstIntersectingSubAreas);
    float fHeight = vPosition.m_fZ - m_vPosition.m_fZ;
    if (fHeight <= -0.001f || fHeight >= 0.001f)
    {
        Vector vSlope(fRate, 0.0f, fHeight);
        Vector vSlopeDisplacement = fRate * normalize(vSlope);
        CPathfindInformation* pPathfindInformation = m_pPathfindInformation;
        float fSlopeDistance = vSlopeDisplacement.m_fX;
        pPathfindInformation->m_nField_9C = nWayPoint;
        bAtEnd = WalkUpdateLocationDistance(fSlopeDistance, &vPosition,
                                            &vDirection, &lstIntersectingSubAreas);
    }
    AutoCloseGUIPanels(TRUE);
    UpdateSubareasOnMoveTo(m_vPosition, vPosition, TRUE,
                          &lstIntersectingSubAreas, FALSE);
    SetPosition(vPosition, TRUE, TRUE, FALSE);
    vDirection.m_fZ = 0.0f;
    vDirection.Normalize();
    SetOrientation(vDirection);

    if (GetInParty() &&
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0) != NULL)
    {
        CSWCCreature* pCharacter =
            g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
        if (GetId() == g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pCharacter->GetId()))
        {
            CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
            pParty->m_cFollowPaths.ProcessPoint(vPosition, Yaw(vDirection), 0.5f);
        }
    }

    unsigned long oidTarget = m_pPathfindInformation->m_oidPathfindTargetCreature;
    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget != NULL)
    {
        if (pTarget->m_nObjectType == SW_OBJECT_TYPE_TRIGGER)
        {
            if (!pTarget->AsSWSTrigger()->GetTrapFlag() &&
                GetIsInUseRange(oidTarget, 0.0f, FALSE))
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICKED;
                pEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
                return CSWSObject::ACTION_COMPLETE;
            }
        }
        else if (pTarget->m_nObjectType == SW_OBJECT_TYPE_DOOR)
        {
            CSWSDoor* pDoor = pTarget->AsSWSDoor();
            if (pDoor->GetIsLinked() && GetIsInUseRange(oidTarget, 0.0f, FALSE))
            {
                if (pDoor->m_nOpenState == CSWSDoor::OPEN_STATE_CLOSED)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, oidTarget, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    return CSWSObject::ACTION_IN_PROGRESS;
                }
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICKED;
                pEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
                return CSWSObject::ACTION_COMPLETE;
            }
        }
    }
    if (bAtEnd == TRUE)
    {
        m_pPathfindInformation->ResetAtEndOfPath();
        SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
        return CSWSObject::ACTION_COMPLETE;
    }
    return CSWSObject::ACTION_IN_PROGRESS;
}
// PC 0x00551F30
int CSWSCreature::WalkUpdateLocation_QuickWalk_ForcePushed()
{
    int nResult = ACTION_IN_PROGRESS;
    float fLength, fRate;
    if (m_nAnimationState == ANIMATION_STATE_FORCE_PUSH)
    {
        fLength = 5.0f;
        fRate = 25.0f;
    }
    else
    {
        float* pfPath = GetPathfindInformation()->m_pWayPoints;
        Vector vStart(pfPath[0], pfPath[1], 0.0f);
        Vector vEnd(pfPath[2], pfPath[3], 0.0f);
        Vector vDifference = vEnd - vStart;
        fLength = sqrtf(vDifference * vDifference);
        fRate = fLength / 0.33f;
    }
    float fDistance = fRate * m_nWalkElapsedTime / 1000.0f;
    if (fDistance != 0.0f)
    {
        Vector vPosition, vDirection;
        CExoArrayList<unsigned long> lstIntersectingSubAreas;
        int bAtEnd = WalkUpdateLocationDistance(fDistance, &vPosition, &vDirection,
                                                &lstIntersectingSubAreas);
        if (bAtEnd == 0)
        {
            UpdateSubareasOnMoveTo(m_vPosition, vPosition, 1, &lstIntersectingSubAreas, 0);
            float fOldDistance = m_fForcedMovementDistance;
            float fFraction = fOldDistance / fLength;

            float fPeakHeight = 0.0f;
            float fHeight = fPeakHeight * fFraction * fFraction * -4.0f;
            fHeight += fPeakHeight * fFraction * 4.0f;
            if (GetId() == g_pAppManager->GetServerExoApp()->GetPlayerCreatureId())
            {
                float fStepScale = GetPathfindInformation()->m_fStepScale;
                CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
                pParty->m_cFollowPaths.ProcessPoint(vPosition, Yaw(m_vOrientation), fStepScale);
            }
            SetPosition(vPosition, 1, 1, 0);
            m_fForcedMovementHeight = fHeight;
            m_fForcedMovementDistance = fOldDistance + fDistance;
        }
        else
        {
            if (GetId() == g_pAppManager->GetServerExoApp()->GetPlayerCreatureId())
            {
                float fStepScale = GetPathfindInformation()->m_fStepScale;
                CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
                pParty->m_cFollowPaths.ProcessPoint(vPosition, Yaw(m_vOrientation), fStepScale);
            }
            SetPosition(vPosition, 1, 1, 0);
            m_fForcedMovementDistance = 0.0f;
            nResult = ACTION_COMPLETE;
        }
    }
    return nResult;
}
// PC 0x005521C0
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader()
{
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    unsigned long oidClient = pClient->ServerToClientObjectId(GetId());
    int nMember = g_pAppManager->m_pClientExoApp->GetSWParty()->GetIndex(oidClient);
    if (nMember == -1)
        return ACTION_IN_PROGRESS;
    if (nMember == 0)
        return ACTION_COMPLETE;
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    EXOXBOXASSERT(nMember >= 0 && nMember < 3);
    CSWPartyMemberData* pMember = &pParty->m_aPartyMemberData[nMember];
    CSWPartyFollowDataArray* pFollowPaths = &pParty->m_cFollowPaths;
    if (pMember->m_bUpdateLastPosition)
    {
        pMember->m_vLastPosition = m_vPosition;
        pMember->m_bUpdateLastPosition = 0;
    }
    if (RenderPartyFollow)
    {
        g_pAppManager->m_pClientExoApp->GetSWParty();
        g_pAppManager->m_pClientExoApp->GetSWParty();
        CreaturePartyIndex = nMember;
        m_sTag.CStr();
        magnitude(pMember->m_vFormationPosition - pMember->m_vFollowPosition);
    }
    if (m_nWalkElapsedTime == 0.0f)
        return ACTION_IN_PROGRESS;
    Vector vDifference = pMember->m_vFormationPosition - pMember->m_vFollowPosition;
    float fDistanceSquared = vDifference.m_fX * vDifference.m_fX + vDifference.m_fY * vDifference.m_fY +
                             vDifference.m_fZ * vDifference.m_fZ;
    Vector vFromLast = pMember->m_vFormationPosition - pMember->m_vLastPosition;
    float fRate;
    if (!(fDistanceSquared >= 16.0) && !(vFromLast.m_fX * vFromLast.m_fX + vFromLast.m_fY * vFromLast.m_fY +
          vFromLast.m_fZ * vFromLast.m_fZ >= 100.0f))
    {
        if (pClient->GetSWParty()->GetCharacter(0)->GetSWAnimationSpeed() > 0.1f)
        {
            fRate = pClient->GetSWParty()->GetCharacter(0)->GetSWAnimationSpeed() * 1000.0f;
        }
        else
        {
            Vector vBack = pMember->m_vFollowPosition - pMember->m_vFormationPosition;
            if (vBack.m_fX * vBack.m_fX + vBack.m_fY * vBack.m_fY + vBack.m_fZ * vBack.m_fZ >= 0.001f)
            {
                float fSpeed = pMember->m_fSpeed;
                if (fSpeed > GetWalkRate() * 0.9f)
                    fRate = pMember->m_fSpeed;
                else
                    fRate = GetWalkRate() * 0.9f;
                if (FollowLeaderFullLogging)
                {
                    pClient->GetSWParty()->GetCharacter(0);
                    GetClientCreature();
                }
            }
            else
            {
                if (fDistanceSquared <= 4.0f)
                    fRate = GetRunRate() * DriveWalkThreshold;
                else
                    fRate = GetRunRate();
                if (FollowLeaderFullLogging)
                {
                    pClient->GetSWParty()->GetCharacter(0);
                    GetClientCreature();
                }
            }
        }
    }
    else if (pClient->GetSWParty()->GetCharacter(0)->GetSWAnimationSpeed() > 0.1f)
    {
        fRate = max(GetRunRate(), pClient->GetSWParty()->GetCharacter(0)->GetSWAnimationSpeed() * 1000.0f);
    }
    else
        fRate = GetRunRate();
    if (FollowLeaderFullLogging)
    {
        pClient->GetSWParty()->GetCharacter(0);
        GetClientCreature();
    }
    float fMaximumSpeed = fRate;
    if (fDistanceSquared <= 1.0f)
        fRate *= 0.9f;
    else if (fDistanceSquared <= 225.0f)
        fRate *= 1.2f;
    else
        fRate *= 1.5f;
    float fDistance = (m_nWalkElapsedTime * fRate) * 0.000001f;
    Vector vOldPosition = m_vPosition;
    int nOtherMember = 0;
    if (nMember == 1)
        nOtherMember = 2;
    else if (nMember == 2)
        nOtherMember = 1;
#ifdef _XBOX
    CSWParty* pOtherParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    EXOXBOXASSERT(nOtherMember >= 0 && nOtherMember < 3);
    g_pAppManager->m_pServerExoApp->ClientToServerObjectId(
        pOtherParty->m_aPartyMemberData[nOtherMember].m_oidCreature);
#else
    g_pAppManager->m_pServerExoApp->ClientToServerObjectId(
        g_pAppManager->m_pClientExoApp->GetSWParty()->m_aPartyMemberData[nOtherMember].m_oidCreature);
#endif
    m_pPathfindInformation->m_nField_34 = OBJECT_INVALID;
    GetArea()->m_pPathfindInformation = GetPathfindInformation();
    pMember->m_fOffsetScale = 1.0f;
    int nResult = ACTION_IN_PROGRESS;
    while (nResult == ACTION_IN_PROGRESS && fDistance >= 0.0f)
    {
        pMember->m_nStepCount++;
        switch (pMember->m_nCharacterIndex)
        {
        case CSWPartyMemberData::FOLLOW_UNINITIALIZED:
            pMember->m_vLastPosition = m_vPosition;
            pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_WAITING_FOR_PATH;
            break;
        case CSWPartyMemberData::FOLLOW_FIND_PATH:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_FindPath(pMember, pFollowPaths, fDistance, nMember);
            break;
        case CSWPartyMemberData::FOLLOW_OFFSET:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_Offset(pMember, pFollowPaths, fDistance, nMember);
            break;
        case CSWPartyMemberData::MOVING_TO_PATH:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_Moving_To_Path(pMember, pFollowPaths, fDistance, nMember);
            break;
        case CSWPartyMemberData::FOLLOW_PATH:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_Following_Path(pMember, pFollowPaths, fDistance, nMember);
            break;
        case CSWPartyMemberData::MOVING_TO_OFFSET:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_Moving_To_Offset(pMember, pFollowPaths, fDistance, nMember);
            break;
        case CSWPartyMemberData::FOLLOW_WAITING_FOR_PATH:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_WaitingForPath(pMember, pFollowPaths, fDistance, nMember);
            break;
        case CSWPartyMemberData::FOLLOW_OFFSET_WAIT_DEST:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_OffsetWaitDest(pMember, pFollowPaths, fDistance, nMember);
            pMember->m_nStepCount = 0;
            break;
        case CSWPartyMemberData::FOLLOW_PATH_WAIT_DEST:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_PathWaitDest(pMember, pFollowPaths, fDistance, nMember);
            pMember->m_nStepCount = 0;
            break;
        case CSWPartyMemberData::FOLLOW_TO_OFFSET_WAIT_DEST:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_ToOffsetWaitDest(pMember, pFollowPaths, fDistance, nMember);
            pMember->m_nStepCount = 0;
            break;
        case CSWPartyMemberData::FOLLOW_TO_PATH_WAIT_DEST:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_ToPathWaitDest(pMember, pFollowPaths, fDistance, nMember);
            pMember->m_nStepCount = 0;
            break;
        case CSWPartyMemberData::FOLLOW_TELEPORT_FORWARD:
            nResult = WalkUpdateLocation_QuickWalk_FollowLeader_TeleportForward(pMember, pFollowPaths, fDistance, nMember);
            pMember->m_nStepCount = 0;
            break;
        }
        if (pMember->m_nStepCount > 50)
        {
            pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_UNINITIALIZED;
            pMember->m_nStepCount = 0;
        }
    }
    Vector vCorrection = pMember->m_vLastPosition - m_vPosition;
    if (vCorrection.m_fX * vCorrection.m_fX + vCorrection.m_fY * vCorrection.m_fY +
        vCorrection.m_fZ * vCorrection.m_fZ >= 0.001f)
    {
        float fRadius = m_pPathfindInformation->m_fStepScale;
        float fHalfStep = m_pPathfindInformation->m_fHalfStep;
        if (GetArea()->TestDirectLine(pMember->m_vLastPosition, m_vPosition,
                                      fRadius, fHalfStep, 0, NULL) == 1)
        {
            vCorrection *= fSpringFactor;
            vCorrection.m_fZ = 0.0f;
            Vector vPosition = m_vPosition + vCorrection;
            vPosition.m_fZ = GetArea()->ComputeHeight(vPosition, 1, NULL);
            SetPosition(vPosition, 1, 1, 0);
        }
        else if (GetArea()->TestSafeLocationPoint(pMember->m_vLastPosition, m_pPathfindInformation) == 1)
            SetPosition(pMember->m_vLastPosition, 1, 1, 0);
        else
            pMember->m_nStepCount++;
    }
    else
    {
        float fRadius = m_pPathfindInformation->m_fStepScale;
        float fHalfStep = m_pPathfindInformation->m_fHalfStep;
        if (GetArea()->TestDirectLine(pMember->m_vLastPosition, m_vPosition,
                                      fRadius, fHalfStep, 0, NULL) == 1)
            SetPosition(pMember->m_vLastPosition, 1, 1, 0);
    }
    g_pAppManager->GetServerExoApp()->UpdateSubareasOnMoveTo(GetId(), vOldPosition, m_vPosition);
    Vector vNewPosition = m_vPosition;
    Vector vMovement(vNewPosition.m_fX - vOldPosition.m_fX,
                     vNewPosition.m_fY - vOldPosition.m_fY, 0.0f);
    float fMovement = magnitude(vMovement);
    Vector vLeaderDirection = normalize(pParty->GetCharacter(0)->m_vPosition - m_vPosition);
    if (vMovement.m_fX == 0.0f && vMovement.m_fY == 0.0f)
    {
        float fDot = vLeaderDirection * m_vOrientation;
        float fHeadLimit = cos(GetClientCreature()->GetHeadTurnHorizontal() * DEG_TO_RAD);
        if ((fDot < fHeadLimit && fDot >= 0.0f) || (-fHeadLimit < fDot && fDot <= 0.0f))
            pMember->m_vFacing = vLeaderDirection;
        GetClientCreature()->LookAt(pParty->GetCharacter(0)->GetId(), 8.0f);
    }
    else
    {
        GetClientCreature()->LookAt(OBJECT_INVALID, 8.0f);
        SetOrientation(normalize(vMovement));
        pMember->m_nStepCount = 0;
        pMember->m_vFacing = m_vOrientation;
    }
    float fNewSpeed = fMovement * 1000000.0f / m_nWalkElapsedTime;
    float fOldSpeed;
    memcpy(&fOldSpeed, &pMember->m_fSpeed, sizeof(float));
    double dSpeed = fNewSpeed;
    if (!(dSpeed <= 0.0001))
        dSpeed = (fOldSpeed + fNewSpeed) * 0.5f;
    float fSpeed = (float)dSpeed;
    if (fSpeed < 10.0f)
        fSpeed = 0.0f;
    fSpeed = min(fMaximumSpeed, fSpeed);
    pMember->m_fSpeed = fSpeed;
    return nResult;
}
// PC 0x005508D0
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_FindPath(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    g_pAppManager->GetClientExoApp()->GetSWParty()->RecaulateFollowPoint();
    Vector vPathPosition = pMember->m_vFormationPosition;
    int nPathIndex = pMember->m_nFormationPathIndex;
    float fFacing = pPaths->GetPathData(nPathIndex)->m_fFacing;
    Vector vOffsetPosition = vPathPosition +
        Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fFacing)) *
            pMember->m_vFormationOffset;
    vOffsetPosition.m_fZ = GetArea()->ComputeHeight(vOffsetPosition, 1, NULL);
    pMember->m_vFollowPosition = vPathPosition;
    pMember->m_nFollowPathIndex = nPathIndex;
    m_pPathfindInformation->SetBlockingCreature(OBJECT_INVALID);

    unsigned long oidTarget = OBJECT_INVALID;
    int bMoveRun = TRUE;
    float fRange = NWSCREATURE_FOLLOW_DESTINATION_RANGE;
    unsigned long oidArea = GetArea()->GetId();
    int bCommandable = m_bCommandable;
    m_bCommandable = TRUE;
    CSWSObjectActionNode* pAction = GetAction(ACTION_FOLLOW_LEADER);
    unsigned long nGroupId = ACTION_GROUP_NEW;
    if (pAction != NULL)
        nGroupId = pAction->m_nGroupId;

    if (GetArea()->TestSafeLocationPoint(vOffsetPosition, m_pPathfindInformation))
    {
        AddActionToFront(ACTION_CHECK_MOVE_TO_FOLLOW_RADIUS, nGroupId,
            ACTION_PARAM_FLOAT, &vOffsetPosition.m_fX,
            ACTION_PARAM_FLOAT, &vOffsetPosition.m_fY,
            ACTION_PARAM_FLOAT, &vOffsetPosition.m_fZ,
            ACTION_PARAM_OBJECT, &oidArea, ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_INTEGER, &bMoveRun, ACTION_PARAM_FLOAT, &fRange,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddMoveToPointActionToFront(ACTION_GROUP_LAST, vOffsetPosition,
            m_oidArea, OBJECT_INVALID, TRUE, 0.0f,
            NWSCREATURE_FOLLOW_PATH_TIMEOUT, 0, 0, 0, 0, 0, 0,
            Vector(), 0, 0, 0);
        pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_OFFSET_WAIT_DEST;
    }
    else
    {
        GetArea()->ComputeSafeLocation(vPathPosition,
            NWSCREATURE_FOLLOW_SAFE_SEARCH_RADIUS, m_pPathfindInformation,
            FALSE, &vPathPosition);
        AddActionToFront(ACTION_CHECK_MOVE_TO_FOLLOW_RADIUS, nGroupId,
            ACTION_PARAM_FLOAT, &vPathPosition.m_fX,
            ACTION_PARAM_FLOAT, &vPathPosition.m_fY,
            ACTION_PARAM_FLOAT, &vPathPosition.m_fZ,
            ACTION_PARAM_OBJECT, &oidArea, ACTION_PARAM_OBJECT, &oidTarget,
            ACTION_PARAM_INTEGER, &bMoveRun, ACTION_PARAM_FLOAT, &fRange,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddMoveToPointActionToFront(ACTION_GROUP_LAST, vPathPosition,
            m_oidArea, OBJECT_INVALID, TRUE, 0.0f,
            NWSCREATURE_FOLLOW_PATH_TIMEOUT, 0, 0, 0, 0, 0, 0,
            Vector(), 0, 0, 0);
        pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_PATH_WAIT_DEST;
    }
    m_nAnimationState = -1;
    m_bCommandable = bCommandable;
    unsigned long oidLeader = g_pAppManager->GetClientExoApp()->
        GetSWParty()->m_aPartyMemberData[0].m_oidCreature;
    oidLeader = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidLeader);
    CSWSCreature* pLeader = g_pAppManager->GetServerExoApp()->
        GetCreatureByGameObjectID(oidLeader);
    if (pLeader != NULL)
        pMember->m_vLeaderPosition = pLeader->m_vPosition;
    pMember->m_bUpdateLastPosition = TRUE;
    return ACTION_COMPLETE;
}
// PC 0x00550DA0
void CSWSCreature::SetVisibilityList(CExoArrayList<CSWVisibilityNode*>* pList)
{
    m_lstVisibilityList.SetSize(0);
    if (pList != NULL)
    {
        if (pList->num <= 0)
            return;
        for (int i = 0; i < pList->num; i++)
        {
            m_lstVisibilityList.Add(pList->element[i]);
        }
    }
}
// PC 0x00550E20
void CSWSCreature::UpdateVisibleList(unsigned char nUpdateType)
{
    int i;
    int nIndex;
    CSWParty* pParty;
    CServerExoApp* pServer;
    CSWSCreature* pCreature;
    unsigned long oidCreature;
    CSWSArea* pArea = GetArea();
    if (pArea != NULL)
    {
        pServer = g_pAppManager->m_pServerExoApp;
        CGameObjectArray* pObjectArray = pServer->GetObjectArray();
        pServer->GetServerAIMaster()->m_nLastEvent[CServerAIMaster::LAST_EVENT_PERCEPTION] = 1;

        if (m_nAreaWaitState == 0 && GetDead() == 1)
            return;

        float fDistance = (GetSpotCheckDistance() >= GetListenCheckDistance()) ?
            GetSpotCheckDistance() : GetListenCheckDistance();
        float fDistanceSquared = fDistance * fDistance;

        if (nUpdateType == VISIBLE_LIST_UPDATE_PARTY)
        {
            if (m_bInParty == 0)
            {
                pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
                if (pParty != NULL)
                {
                    for (i = 0; i < pParty->m_nCharacters; i++)
                    {
                        EXOXBOXASSERT(i >= 0 && i < 3);
                        oidCreature = pParty->m_aPartyMemberData[i].m_oidCreature;
                        oidCreature = pServer->ClientToServerObjectId(oidCreature);
                        pCreature =
                            pServer->GetCreatureByGameObjectID(oidCreature);
                        if (pCreature != NULL)
                        {
                            Vector vDifference = pCreature->m_vPosition - m_vPosition;
                            if (GetVisibleListElement(oidCreature) != NULL ||
                                vDifference * vDifference <= fDistanceSquared ||
                                pCreature->m_n260)
                            {
                                DoPerceptionUpdateOnCreature(pCreature);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if (m_lstVisibilityList.num > 0)
            {
                for (i = m_lstVisibilityList.num - 1; i >= 0; i--)
                {
                    oidCreature = m_lstVisibilityList[i]->m_oidObject;
                    if (oidCreature != m_nId)
                    {
                        pCreature =
                            pServer->GetCreatureByGameObjectID(oidCreature);
                        if (pCreature != NULL)
                        {
                            if (nUpdateType == VISIBLE_LIST_UPDATE_ALL ||
                                (nUpdateType == VISIBLE_LIST_UPDATE_NONPARTY &&
                                 pCreature->m_bInParty == 0))
                            {
                                DoPerceptionUpdateOnCreature(pCreature);
                            }
                        }
                        else
                        {
                            RemoveFromVisibleList(oidCreature);
                        }
                    }
                }
            }

            float fMinX = m_vPosition.m_fX - fDistance;
            float fMaxX = m_vPosition.m_fX + fDistance;
            if (!pArea->GetFirstObjectIndiceByX(&nIndex, fMinX))
                return;

            while (nIndex < pArea->m_lstObjectIds.num)
            {
                oidCreature = pArea->m_lstObjectIds[nIndex];
                if (oidCreature != m_nId)
                {
                    pCreature =
                        pServer->GetCreatureByGameObjectID(oidCreature);
                    if (pCreature != NULL)
                    {
                        if (pCreature->m_vPosition.m_fX > fMaxX)
                            break;

                        Vector vDifference = pCreature->m_vPosition - m_vPosition;
                        if (GetVisibleListElement(oidCreature) == NULL &&
                            (vDifference * vDifference <= fDistanceSquared ||
                             pCreature->m_n260) &&
                            (nUpdateType == VISIBLE_LIST_UPDATE_ALL ||
                             (nUpdateType == VISIBLE_LIST_UPDATE_NONPARTY &&
                              pCreature->m_bInParty == 0)))
                        {
                            DoPerceptionUpdateOnCreature(pCreature);
                        }
                    }
                }
                ++nIndex;
            }
        }
    }
    m_bVisibleListUpdatePending = 0;
}
// PC 0x0054CF00
void CSWSCreature::RemoveFromVisibleList(unsigned long oidObject)
{
    int i;
    CSWVisibilityNode* pNode;

    if (m_lstVisibilityList.num > 0)
    {
        for (i = m_lstVisibilityList.num - 1; i >= 0; i--)
        {
            if (m_lstVisibilityList[i]->m_oidObject == oidObject)
            {
                pNode = m_lstVisibilityList[i];
                m_lstVisibilityList.DelIndex(i);
                if (pNode != NULL)
                {
                    delete pNode;
                }
            }
        }
    }
}
// PC 0x0054CF80
int CSWSCreature::CheckVisibleList(unsigned long oidObject)
{
    int i;

    for (i = 0; i < m_lstVisibilityList.num; i++)
    {
        if (m_lstVisibilityList[i]->m_oidObject == oidObject)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x0054CFC0
CSWVisibilityNode* CSWSCreature::GetVisibleListElement(unsigned long oidObject)
{
    int i;

    for (i = 0; i < m_lstVisibilityList.num; i++)
    {
        if (m_lstVisibilityList[i]->m_oidObject == oidObject)
        {
            return m_lstVisibilityList[i];
        }
    }
    return NULL;
}
// PC 0x0054D000
int CSWSCreature::AIActionHeal(CSWSObjectActionNode* pAction)
{
    if ((GetDead() == TRUE && GetIsPCDying() == TRUE) || GetDeadTemp() == TRUE)
        return ACTION_FAILED;

    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    unsigned long oidItem = (unsigned long)pAction->m_apParam[1];
    int nHealOption = (int)pAction->m_apParam[2];
    int bApproach = (int)pAction->m_apParam[3];
    CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
    if (pTarget == NULL)
    {
        m_bHealAnimationPlayed = FALSE;
        return ACTION_FAILED;
    }
    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
    {
        m_bHealAnimationPlayed = FALSE;
        return ACTION_FAILED;
    }

    Vector vTarget;
    int bRun;
    int bCheckRange;
    float fRange;
    if (!GetIsInUseRange(oidTarget, 0.0f, FALSE) && bApproach)
    {
        bRun = TRUE;
        bCheckRange = TRUE;
        bApproach = FALSE;
        AsSWSCreature()->GetUseRange(oidTarget, vTarget, fRange, FALSE);
        AddActionToFront(ACTION_HEAL, pAction->m_nGroupId, ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_INTEGER, &nHealOption, ACTION_PARAM_INTEGER, &bApproach, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId, ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
            ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_FLOAT, &fRange,
            ACTION_PARAM_INTEGER, &bCheckRange, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddMoveToPointActionToFront(pAction->m_nGroupId, vTarget, m_oidArea,
            oidTarget, bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0,
            Vector(), 0, 0, 0);
        return ACTION_COMPLETE;
    }

    int nSkillRank = GetStats()->GetSkillRank(CSWRules::SKILL_TREAT_INJURY,
                                            pTarget->AsSWSObject(), FALSE);
    if (!m_bHealAnimationPlayed)
    {
        m_bHealAnimationPlayed = TRUE;
        int bLoop = TRUE;
        int nAnimation = CREATURE_ANIMATION_SPELLCAST_1;
        float fSpeed = 1.0f;
        float fDuration = 2.0f;
        AddActionToFront(ACTION_HEAL, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_OBJECT, &oidItem,
            ACTION_PARAM_INTEGER, &nHealOption, ACTION_PARAM_INTEGER, &bApproach,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_PLAY_ANIMATION, pAction->m_nGroupId,
            ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
            ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLoop,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        return ACTION_COMPLETE;
    }

    int nRoll = g_pRules->RollDice(1, 20);
    int bTakeTwenty;
    if (m_bCombatState == TRUE)
        bTakeTwenty = FALSE;
    else
    {
        bTakeTwenty = TRUE;
        nRoll = 20;
    }

    int i = 0;
    CGameEffect* pAffliction = NULL;
    for (; i < pTarget->m_lstEffects.num; ++i)
    {
        if (pTarget->m_lstEffects[i]->GetEffectType() > CSWSEffectListHandler::EFFECT_POISON)
            break;
        if (pTarget->m_lstEffects[i]->GetEffectType() == CSWSEffectListHandler::EFFECT_POISON)
        {
            pAffliction = pTarget->m_lstEffects[i];
            break;
        }
        else if (pTarget->m_lstEffects[i]->GetEffectType() == CSWSEffectListHandler::EFFECT_DISEASE)
        {
            pAffliction = pTarget->m_lstEffects[i];
            break;
        }
    }
    int nSaveDC = 0;
    if (pAffliction != NULL)
    {
        if (pAffliction->GetEffectType() == CSWSEffectListHandler::EFFECT_POISON)
        {
            int nPoison = pAffliction->GetInteger(0);
            CExoString sColumn("Save_DC");
#ifdef _XBOX
            g_pRules->Get2DArrays()->GetPoison()->GetINTEntry(nPoison, sColumn, &nSaveDC);
#else
            C2DA* pPoison = g_pRules->Get2DArrays()->m_pPoison;
            pPoison->GetINTEntry(nPoison, sColumn, &nSaveDC);
#endif
        }
        else if (pAffliction->GetEffectType() == CSWSEffectListHandler::EFFECT_DISEASE)
        {
            int nDisease = pAffliction->GetInteger(0);
            CExoString sColumn("Subs_Save");
#ifdef _XBOX
            g_pRules->Get2DArrays()->GetDisease()->GetINTEntry(nDisease, sColumn, &nSaveDC);
#else
            C2DA* pDisease = g_pRules->Get2DArrays()->m_pDisease;
            pDisease->GetINTEntry(nDisease, sColumn, &nSaveDC);
#endif
        }
        if (nSaveDC < 1)
            nSaveDC = 1;
    }
    if (pAffliction == NULL)
    {
        SendFeedbackMessage(FEEDBACK_HEAL_NO_AFFLICTION, NULL);
        if (pTarget->GetCurrentHitPoints(FALSE) >= pTarget->GetMaxHitPoints(TRUE))
        {
            SendFeedbackMessage(FEEDBACK_HEAL_ALREADY_FULL, NULL);
            return ACTION_FAILED;
        }
    }
    else
    {
        if (nSkillRank + nRoll >= nSaveDC)
        {
            unsigned __int64 nEffectId = pAffliction->m_nID;
            pTarget->RemoveEffectById(nEffectId);
        }
    }

    CSWCCMessageData* pData = new CSWCCMessageData;
    pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, GetId());
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, CSWCCMessageData::SKILL_DATA_TREAT_INJURY_STRREF);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, nRoll);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, nSkillRank);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, nSaveDC);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, bTakeTwenty);
    int nTotal = nSkillRank + nRoll;
    if (nTotal >= nSaveDC)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_SUCCESS);
    else if (bTakeTwenty)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_RETRY);
    else
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_FAILED);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_TREAT_INJURY_STRREF);
    BroadcastSkillData(pData);

    if (GetItemRepository(TRUE)->GetItemInRepository(pItem, TRUE) == TRUE)
    {
        if (pItem->m_nStackSize > 1)
            --pItem->m_nStackSize;
        else if (RemoveItem(pItem, TRUE, TRUE, NULL))
        {
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
            if (pPlayer != NULL)
                g_pAppManager->GetServerExoApp()->GetSWSMessage()->SendServerPlayerItemUpdate_DestroyItem(pPlayer, pItem->GetId());
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), pItem->GetId(), CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
        }
    }

    CGameEffect* pEffect = new CGameEffect(TRUE);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_HEAL;
    pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
    pEffect->SetCreator(GetId());
    pEffect->SetInteger(0, nTotal);
    pTarget->ApplyEffect(pEffect, FALSE, FALSE);
    pEffect = new CGameEffect(TRUE);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
    pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
    pEffect->SetCreator(GetId());
    pEffect->SetInteger(0, VFX_IMP_HEALING_SMALL);
    pTarget->ApplyEffect(pEffect, FALSE, FALSE);
    m_bHealAnimationPlayed = FALSE;
    return ACTION_COMPLETE;
}
// PC 0x00544E80
int CSWSCreature::RemoveItem(CSWSItem* pItem, int bUnknown1, int bUnknown2,
                             CSWSItem** ppItem)
{
    CGameObject* pObject;
    CSWSItem* pContainer;

    if (pItem == NULL)
    {
        return 0;
    }

    if (m_pInventory->GetItemInInventory(pItem) == 1)
    {
        RunUnequip(pItem->m_nId, OBJECT_INVALID, bUnknown1, ppItem, 0);
        if (ppItem != NULL)
        {
            pItem = *ppItem;
        }
    }

    int bInRepository = 0;
    if (pItem->m_oidPossessor == m_nId)
    {
        bInRepository = 1;
    }
    else
    {
        CGameObject* pPossessor =
            g_pAppManager->m_pServerExoApp->GetGameObject(pItem->m_oidPossessor);
        CGameObject* pSelf = g_pAppManager->m_pServerExoApp->GetGameObject(m_nId);
        if (pPossessor != NULL && pPossessor->AsSWSCreature() != NULL &&
            pPossessor->AsSWSCreature()->GetInParty() &&
            pSelf != NULL && pSelf->AsSWSCreature() != NULL &&
            pSelf->AsSWSCreature()->GetInParty())
        {
            bInRepository = 1;
        }
    }
    if (bInRepository)
    {
        if (GetItemRepository(1)->GetItemInRepository(pItem, 0) == 1)
        {
            GetItemRepository(1)->RemoveItem(pItem);
        }
    }
    else
    {
        pObject = g_pAppManager->m_pServerExoApp->GetGameObject(pItem->m_oidPossessor);
        if (pObject == NULL || pObject->m_nObjectType != CGameObject::OBJECT_TYPE_ITEM)
        {
            return 0;
        }
        pContainer = pObject->AsSWSItem();
        if (pContainer->m_oidPossessor != m_nId)
        {
            return 0;
        }
        pContainer->m_pRepository->RemoveItem(pItem);
    }

    pItem->SetPossessor(OBJECT_INVALID, 1, bUnknown2, OBJECT_INVALID);
    return 1;
}
// PC 0x00544FD0
int CSWSCreature::AIActionEquipItem(CSWSObjectActionNode* pAction)
{
    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);
    if (pAction->m_nNumParams < 3)
        return ACTION_FAILED;

    unsigned long oidItem =
        (unsigned long)pAction->m_apParam[CSWSObjectActionNode::EQUIP_ITEM_PARAMETER];
    unsigned long nInventorySlot =
        (unsigned long)pAction->m_apParam[CSWSObjectActionNode::EQUIP_SLOT_PARAMETER];
    int nFlags = (int)pAction->m_apParam[CSWSObjectActionNode::EQUIP_FLAGS_PARAMETER];
    if (!GetDead() && !GetIsPCDying() && !GetDeadTemp())
    {
        if (RunEquip(oidItem, nInventorySlot, nFlags) == TRUE)
            return ACTION_COMPLETE;
    }

    CSWSPlayer* pPlayer =
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    if (pMessage != NULL && pPlayer != NULL)
    {
        pMessage->SendServerToPlayerInventory_EquipCancel(
            pPlayer->m_nPlayerId, oidItem, nInventorySlot);
    }
    return ACTION_FAILED;
}
// PC 0x00545090
int CSWSCreature::AIActionAreaWait(CSWSObjectActionNode*)
{
    if (m_nAreaWaitState == 0)
    {
        return CSWSObject::ACTION_FAILED;
    }

    if (((CSWSObject*)this)->GetArea() == NULL)
    {
        return CSWSObject::ACTION_IN_PROGRESS;
    }

    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x005450C0
void CSWSCreature::TerminateClientSidePath(int nPathType)
{
    CServerExoAppInternal* pServer =
        g_pAppManager->m_pServerExoApp->m_pcExoAppInternal;
    CSWSPlayer* pPlayer = pServer->GetClientObjectByObjectId(m_nId);

    if (pPlayer != NULL)
    {
        CLastUpdateObject* pLastUpdateObject =
            pPlayer->GetLastUpdateObject(m_nId);
        if (pLastUpdateObject != NULL)
        {
            pLastUpdateObject->m_vOrientation = Vector(0.0f, 0.0f, 0.0f);
            pLastUpdateObject->m_nAnimation = 0xFFFF;
            pLastUpdateObject->m_nClientSidePathType = nPathType - 1;
        }
    }
}
// PC 0x00545140
int CSWSCreature::AIActionPartyFollowLeader(CSWSObjectActionNode*)
{
    if (m_bInParty != 0 && m_pFollowInfo != NULL &&
        GetDead() != 1 && GetIsPCDying() != 1 &&
        GetDeadTemp() != 1 && m_pPathfindInformation != NULL &&
        (m_nAIStateFlags & AI_STATE_CAN_MOVE) != 0)
    {
        if (GetArea() == NULL)
        {
            return CSWSObject::ACTION_IN_PROGRESS;
        }

        unsigned long oidLeader = g_pAppManager->m_pClientExoApp->GetSWParty()
                                      ->m_aPartyMemberData[0].m_oidCreature;
        if (g_pAppManager->m_pServerExoApp->ClientToServerObjectId(oidLeader) != GetId())
        {
            m_nAnimationState = 3;
            return CSWSObject::ACTION_IN_PROGRESS;
        }
    }

    return CSWSObject::ACTION_FAILED;
}
// PC 0x005451E0
int CSWSCreature::AIActionFollowOwner(CSWSObjectActionNode* pNode)
{
    if (m_n11FC != 0 && !GetDead() && !GetDeadTemp() &&
        (m_nAIStateFlags & AI_STATE_CAN_MOVE) != 0)
    {
        if (GetArea() == NULL)
        {
            return CSWSObject::ACTION_IN_PROGRESS;
        }
        float fFollowDistance = *(float*)&pNode->m_apParam[0];
        CSWPartyTable* pPartyTable = g_pAppManager->m_pServerExoApp->GetPartyTable();
        if (pPartyTable == NULL)
        {
            return CSWSObject::ACTION_IN_PROGRESS;
        }
        CSWSCreature* pOwner = pPartyTable->GetPUPOwner(pPartyTable->GetPUPID(GetId()));
        if (pOwner == NULL)
        {
            return CSWSObject::ACTION_IN_PROGRESS;
        }
        Vector vOwner = pOwner->m_vPosition;
        Vector vPosition = m_vPosition;
        Vector vDistance;
        vDistance.m_fX = vOwner.m_fX - vPosition.m_fX;
        vDistance.m_fY = vOwner.m_fY - vPosition.m_fY;
        vDistance.m_fZ = 0.0f;
        if (magnitude(vDistance) < fFollowDistance)
        {
            return CSWSObject::ACTION_IN_PROGRESS;
        }
        vPosition = pOwner->m_vPosition;
        AddMoveToPointAction(ACTION_GROUP_NEW, vPosition, pOwner->GetArea()->GetId(),
            pOwner->GetId(), 1, fFollowDistance, 0.0f, 0, 0, 0, 0, 0, 0,
            Vector(), 0, 0, 0);
        AddAction(ACTION_FOLLOW_OWNER, ACTION_GROUP_NEW, ACTION_PARAM_FLOAT,
            &fFollowDistance, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        return CSWSObject::ACTION_COMPLETE;
    }
    return CSWSObject::ACTION_FAILED;
}
// PC 0x005453D0
float CSWSCreature::ComputeTotalWalkDistance()
{
    int nFirstPoint = m_pPathfindInformation->m_nField_9C;
    int nPointCount = m_pPathfindInformation->m_nWayPointCount;
    float fDistance = 0.0f;
    int nPoint = nFirstPoint;
    BOOL bDone = FALSE;

    while (!bDone)
    {
        if (nPoint < nPointCount)
        {
            float* pPoints = m_pPathfindInformation->m_pWayPoints;
            float fX = pPoints[nPoint];
            float fY = pPoints[nPoint + 1];
            Vector vCurrent(fX, fY, 0.0f);
            if (nPoint == nFirstPoint)
            {
                fDistance += magnitude(vCurrent - m_vPosition);
            }
            else
            {
                fDistance += magnitude(vCurrent -
                    Vector(pPoints[nPoint - 2], pPoints[nPoint - 1], 0.0f));
            }
            nPoint += 2;
        }
        else
        {
            bDone = TRUE;
        }
    }
    return fDistance;
}
// PC 0x005454A0
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_Offset(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int nMemberIndex)
{
    CWalkHitInfo hitInfo;
    Vector vFollowPosition;
    Vector vPosition;
    int nPathIndex;
    int nFormationPathIndex = pMember->m_nFormationPathIndex;
    int nFollowPathIndex = pMember->m_nFollowPathIndex;
    pPaths->AdvancePointOnPath(fDistance, nMemberIndex, pMember->m_vFollowPosition,
        nFollowPathIndex, pMember->m_vFormationPosition,
        nFormationPathIndex, vFollowPosition, nPathIndex);
    {
        float fRadius = m_pPathfindInformation->m_fStepScale;
        float fWidth = m_pPathfindInformation->m_fHalfStep;
        GetArea()->TestDirectLine(pMember->m_vFollowPosition,
            vFollowPosition, fRadius, fWidth, 0, &hitInfo);
    }
    int nPreviousIndex = nPathIndex - 1;
    if (nPreviousIndex < 0)
        nPreviousIndex += 100;
    float fFacing = pPaths->GetPathData(nPathIndex)->m_fFacing;
    float fPreviousFacing = pPaths->GetPathData(nPreviousIndex)->m_fFacing;
    Vector vOffset = pMember->m_vFormationOffset;
    Vector vPathPosition = pPaths->GetPathData(nPathIndex)->m_vPosition;
    Vector vPreviousPosition = pPaths->GetPathData(nPreviousIndex)->m_vPosition;
    Vector vInterpolated = vFollowPosition;
    Vector vOffsetPosition = vPathPosition +
        Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fFacing)) * vOffset;
    Vector vPreviousOffsetPosition = vPreviousPosition +
        Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fPreviousFacing)) * vOffset;
    float fDistanceFromPrevious = magnitude(vInterpolated - vPreviousPosition);
    float fPathLength = magnitude(vPathPosition - vPreviousPosition);
    float fFraction = fDistanceFromPrevious / fPathLength;
    Vector vOffsetDelta = vOffsetPosition - vPreviousOffsetPosition;
    vOffsetDelta *= fFraction;
    vInterpolated = vOffsetDelta + vPreviousOffsetPosition;
    vPosition = vInterpolated;
    if (GetArea() != NULL)
        vPosition.m_fZ = GetArea()->ComputeHeight(vPosition, 1, NULL);
    float fPathLengthSquared =
        (vPreviousPosition - vPathPosition) * (vPreviousPosition - vPathPosition);
    pMember->m_fOffsetScale =
        ((vPreviousOffsetPosition - vOffsetPosition) *
         (vPreviousOffsetPosition - vOffsetPosition)) / fPathLengthSquared;
    {
        float fRadius = m_pPathfindInformation->m_fStepScale;
        float fWidth = m_pPathfindInformation->m_fHalfStep;
        if (GetArea()->TestDirectLine(pMember->m_vLastPosition, vPosition,
                fRadius, fWidth, 0, &hitInfo) != CSWSArea::DIRECT_LINE_CLEAR)
        {
            pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_PATH;
            return ACTION_IN_PROGRESS;
        }
    }
    int nLineResult;
    {
        float fRadius = m_pPathfindInformation->m_fStepScale;
        float fWidth = m_pPathfindInformation->m_fHalfStep;
        nLineResult = GetArea()->TestDirectLine(vPosition, vFollowPosition,
            fRadius, fWidth, 0, &hitInfo);
    }
    unsigned long oidBlocker = g_pAppManager->GetClientExoApp()->
        ServerToClientObjectId(hitInfo.m_oidBlocker);
    if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
    {
        if (!g_pAppManager->GetClientExoApp()->GetSWParty()->IsPartyMember(oidBlocker))
        {
            pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_PATH;
            return ACTION_IN_PROGRESS;
        }
    }
    else if (nLineResult != CSWSArea::DIRECT_LINE_CLEAR)
    {
        if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE && nFollowPathIndex == nFormationPathIndex)
            return ACTION_FAILED;
        pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_PATH;
        return ACTION_IN_PROGRESS;
    }
    pMember->m_vLastPosition = vPosition;
    pMember->m_nStepCount = 0;
    pMember->m_vFollowPosition = vFollowPosition;
    pMember->m_nFollowPathIndex = nPathIndex;
    return ACTION_COMPLETE;
}
// PC 0x00545C70
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_Moving_To_Path(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    float fMoveDistance = fDistance * NWSCREATURE_FOLLOW_PATH_SPEED_FACTOR;
    Vector vFollowPosition = pMember->m_vFollowPosition;
    Vector vDelta = vFollowPosition - pMember->m_vLastPosition;
    Vector vDirection = normalize(vDelta);
    Vector vPosition;
    int nFollowState = pMember->m_nCharacterIndex;
    int nResult;
    float fDistanceSquared = vDelta * vDelta;
    if (fDistanceSquared <= fMoveDistance * fMoveDistance)
    {
        vPosition = vFollowPosition;
        fDistance -= (float)sqrt(fDistanceSquared) /
            NWSCREATURE_FOLLOW_PATH_SPEED_FACTOR;
        nFollowState = CSWPartyMemberData::FOLLOW_PATH;
        nResult = CSWSObject::ACTION_IN_PROGRESS;
    }
    else
    {
        vPosition = pMember->m_vLastPosition + vDirection * fMoveDistance;
        fDistance = 0.0f;
        nResult = CSWSObject::ACTION_COMPLETE;
    }

    CWalkHitInfo hitInfo;
    float fRadius = m_pPathfindInformation->m_fStepScale -
        NWSCREATURE_FOLLOW_LINE_RADIUS_MARGIN;
    float fWidth = m_pPathfindInformation->m_fHalfStep;
    int nLineResult = CSWSObject::GetArea()->TestDirectLine(
        pMember->m_vLastPosition, vPosition, fRadius, fWidth, 0, &hitInfo);
    int bPartyMember = FALSE;
    if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        bPartyMember = pClient->GetSWParty()->
            IsPartyMember(pClient->
                ServerToClientObjectId(hitInfo.m_oidBlocker));
    }
    else if (nLineResult == CSWSArea::DIRECT_LINE_CLEAR)
    {
        pMember->m_nFollowPathElapsedTime = 0;
        pMember->m_vLastPosition = vPosition;
        pMember->m_nCharacterIndex = nFollowState;
        return nResult;
    }

    if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE && !bPartyMember)
    {
        pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_TELEPORT_FORWARD;
    }
    else
    {
        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, hitInfo.m_oidBlocker, m_nId,
            CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00545F70
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_Following_Path(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    Vector vPosition;
    int nPositionIndex;
    int nFormationIndex = pMember->m_nFormationPathIndex;
    int nFollowIndex = pMember->m_nFollowPathIndex;
    float fRemainingDistance = pPaths->AdvancePointOnPath(fDistance, bRun,
        pMember->m_vFollowPosition, nFollowIndex,
        pMember->m_vFormationPosition, nFormationIndex,
        vPosition, nPositionIndex);
    unsigned long nElapsedTime = m_nWalkElapsedTime;
    pMember->m_nFollowPathElapsedTime += nElapsedTime;

    CWalkHitInfo hitInfo;
    int nLineResult;
    {
        float fRadius = m_pPathfindInformation->m_fStepScale -
            NWSCREATURE_FOLLOW_LINE_RADIUS_MARGIN;
        float fWidth = m_pPathfindInformation->m_fHalfStep;
        nLineResult = CSWSObject::GetArea()->TestDirectLine(
            pMember->m_vFollowPosition, vPosition, fRadius, fWidth, 0, &hitInfo);
    }
    int bPartyMember = FALSE;
    if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
    {
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        bPartyMember = pClient->GetSWParty()->IsPartyMember(
            pClient->ServerToClientObjectId(hitInfo.m_oidBlocker));
    }
    if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE && !bPartyMember)
    {
        if (pMember->m_nFollowPathIndex != nPositionIndex)
        {
            int nEndIndex = nPositionIndex;
            nPositionIndex = pMember->m_nFollowPathIndex;
            vPosition = pPaths->GetPathData(nPositionIndex)->m_vPosition;
            if (equal(vPosition, m_vPosition))
            {
                if (nPositionIndex + 1 <= nEndIndex)
                {
                    ++nPositionIndex;
                    if (nPositionIndex >= CSWPartyFollowDataArray::MAX_PATH_POINTS)
                        nPositionIndex = 0;
                    vPosition = pPaths->GetPathData(nPositionIndex)->m_vPosition;
                }
            }
        }
        {
            float fRadius = m_pPathfindInformation->m_fStepScale -
                NWSCREATURE_FOLLOW_LINE_RADIUS_MARGIN;
            float fWidth = m_pPathfindInformation->m_fHalfStep;
            nLineResult = CSWSObject::GetArea()->TestDirectLine(
                pMember->m_vFollowPosition, vPosition, fRadius, fWidth, 0, &hitInfo);
        }
        if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
        {
            CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
            bPartyMember = pClient->GetSWParty()->IsPartyMember(
                pClient->ServerToClientObjectId(hitInfo.m_oidBlocker));
        }
    }

    if (nLineResult == CSWSArea::DIRECT_LINE_CLEAR)
    {
        pMember->m_vFollowPosition = vPosition;
        pMember->m_nFollowPathIndex = nPositionIndex;
        pMember->m_vLastPosition = vPosition;
        pMember->m_nStepCount = 0;
        fDistance = fRemainingDistance;

        float fFacing = pPaths->GetPathData(nPositionIndex)->m_fFacing;
        Vector vOffsetPosition = vPosition +
            Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fFacing)) *
                pMember->m_vFormationOffset;
        CWalkHitInfo offsetHitInfo;
        int nOffsetResult;
        {
            float fRadius = m_pPathfindInformation->m_fStepScale;
            float fWidth = GetPathfindInformation()->m_fHalfStep;
            nOffsetResult = CSWSObject::GetArea()->TestDirectLine(vPosition,
                vOffsetPosition, fRadius, fWidth, 0, &offsetHitInfo);
        }
        if (nOffsetResult == CSWSArea::DIRECT_LINE_CLEAR)
        {
            float fPathDistance = 0.0f;
            int bFinished = FALSE;
            int bClear = TRUE;
            int nPathIndex = pMember->m_nFollowPathIndex;
            Vector vPreviousPosition = vOffsetPosition;
            while (!bFinished && fPathDistance <= NWSCREATURE_FOLLOW_OFFSET_LOOKAHEAD)
            {
                CSWPartyFollowData* pPathPoint = pPaths->GetPathData(nPathIndex);
                float fPathFacing = pPathPoint->m_fFacing;
                Vector vNextPosition;
                if (nPathIndex == pMember->m_nFormationPathIndex)
                {
                    vNextPosition = pMember->m_vFormationPosition +
                        Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fPathFacing)) *
                            pMember->m_vFormationOffset;
                    vNextPosition = vPreviousPosition +
                        normalize(vNextPosition - vPreviousPosition) *
                            (NWSCREATURE_FOLLOW_OFFSET_LOOKAHEAD - fPathDistance);
                    bFinished = TRUE;
                }
                else
                {
                    vNextPosition =
                        Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fPathFacing)) *
                            pMember->m_vFormationOffset +
                        pPaths->GetPathData(nPathIndex)->m_vPosition;
                }
                if (equal(vPreviousPosition, vNextPosition))
                    return CSWSObject::ACTION_COMPLETE;
                float fPathRadius = m_pPathfindInformation->m_fStepScale;
                float fPathWidth = m_pPathfindInformation->m_fHalfStep;
                if (CSWSObject::GetArea()->TestDirectLine(vPreviousPosition,
                        vNextPosition, fPathRadius, fPathWidth, 0, NULL) !=
                        CSWSArea::DIRECT_LINE_CLEAR)
                {
                    bFinished = TRUE;
                    bClear = FALSE;
                }
                fPathDistance += magnitude(vNextPosition - vPreviousPosition);
                vPreviousPosition = vNextPosition;
                ++nPathIndex;
                if (nPathIndex >= CSWPartyFollowDataArray::MAX_PATH_POINTS)
                    nPathIndex = 0;
            }
            if (bClear == TRUE && pMember->m_nFollowPathElapsedTime >=
                    NWSCREATURE_FOLLOW_OFFSET_DELAY)
            {
                pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_OFFSET;
                fDistance = 0.0f;
            }
        }
    }
    else if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
    {
        if (!bPartyMember)
            pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_TELEPORT_FORWARD;
    }
    else
    {
        fDistance = 0.0f;
        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED;
        CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
        pServer->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, hitInfo.m_oidBlocker, m_nId,
            CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00546B40
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_Moving_To_Offset(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    CWalkHitInfo hitInfo;
    float fMoveDistance = fDistance * NWSCREATURE_FOLLOW_PATH_SPEED_FACTOR;
    int nPathIndex = pMember->m_nFollowPathIndex;
    float fFacing = pPaths->GetPathData(nPathIndex)->m_fFacing;
    Vector vAxis;
    vAxis.m_fZ = 1.0f;
    Vector vFollowPosition = pMember->m_vFollowPosition +
        Quaternion(vAxis, DegToRad(fFacing)) *
        pMember->m_vFormationOffset;
    Vector vDelta = vFollowPosition - pMember->m_vLastPosition;
    Vector vDirection = normalize(vDelta);
    int nFollowState = pMember->m_nCharacterIndex;
    Vector vPosition;
    int nResult;
    float fDistanceSquared = vDelta * vDelta;
    if (fDistanceSquared <= fMoveDistance * fMoveDistance)
    {
        vPosition = vFollowPosition;
        nFollowState = CSWPartyMemberData::FOLLOW_OFFSET;
        fDistance -= (float)sqrt(fDistanceSquared) /
            NWSCREATURE_FOLLOW_PATH_SPEED_FACTOR;
        nResult = CSWSObject::ACTION_IN_PROGRESS;
    }
    else
    {
        vPosition = pMember->m_vLastPosition + normalize(vDelta) * fMoveDistance;
        fDistance = 0.0f;
        nResult = CSWSObject::ACTION_COMPLETE;
    }

    {
        float fRadius = m_pPathfindInformation->m_fStepScale;
        fRadius -= NWSCREATURE_FOLLOW_LINE_RADIUS_MARGIN;
        float fWidth = m_pPathfindInformation->m_fHalfStep;
        if (CSWSObject::GetArea()->TestDirectLine(
                pMember->m_vLastPosition, vPosition, fRadius, fWidth, 0, &hitInfo) ==
            CSWSArea::DIRECT_LINE_CLEAR)
        {
            pMember->m_vLastPosition = vPosition;
            pMember->m_nCharacterIndex = nFollowState;
            return nResult;
        }
    }

    int nLineResult;
    {
        float fRadius = m_pPathfindInformation->m_fStepScale;
        fRadius -= NWSCREATURE_FOLLOW_LINE_RADIUS_MARGIN;
        float fWidth = m_pPathfindInformation->m_fHalfStep;
        nLineResult = CSWSObject::GetArea()->TestDirectLine(
            pMember->m_vLastPosition, pMember->m_vFollowPosition,
            fRadius, fWidth, 0, &hitInfo);
    }
    if (nLineResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
    {
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        if (pClient->GetSWParty()->IsPartyMember(
                pClient->ServerToClientObjectId(hitInfo.m_oidBlocker)))
            return CSWSObject::ACTION_COMPLETE;
    }
    else if (nLineResult == CSWSArea::DIRECT_LINE_CLEAR)
    {
        pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_PATH;
        return CSWSObject::ACTION_IN_PROGRESS;
    }
    pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_TELEPORT_FORWARD;
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00546FD0
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_WaitingForPath(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    if (pPaths->m_nPathDataCount >= 5)
    {
        float fFollowDistance = pMember->m_fFollowDistance;
        int nPathIndex;
        Vector vFollowPosition;
        if (pPaths->GetPointBackFromStart(fFollowDistance, vFollowPosition,
                                        nPathIndex, 0))
        {
            pMember->m_vFollowPosition = vFollowPosition;
            pMember->m_nFollowPathIndex = nPathIndex;
            pMember->m_vFormationPosition = vFollowPosition;
            pMember->m_nFormationPathIndex = nPathIndex;

            Vector vLeaderPosition;
            vLeaderPosition = g_pAppManager->GetClientExoApp()->
                GetSWParty()->GetCharacter(0)->m_vPosition;
            Vector vLeaderDelta = vLeaderPosition - m_vPosition;
            if (pMember->m_bWaitingForLeader &&
                vLeaderDelta * vLeaderDelta <
                    NWSCREATURE_FOLLOW_PATH_WAIT_DISTANCE_SQUARED)
            {
                return CSWSObject::ACTION_COMPLETE;
            }
            pMember->m_bWaitingForLeader = 0;

            CWalkHitInfo hitInfo;
            int nFollowIndex = pMember->m_nFollowPathIndex;
            float fFacing = pPaths->GetPathData(nFollowIndex)->m_fFacing;
            Vector vFormationPosition = pMember->m_vFollowPosition +
                Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fFacing)) *
                    pMember->m_vFormationOffset;
            {
                float fRadius = m_pPathfindInformation->m_fStepScale -
                    NWSCREATURE_FOLLOW_LINE_RADIUS_MARGIN;
                float fWidth = m_pPathfindInformation->m_fHalfStep;
                if (CSWSObject::GetArea()->TestDirectLine(pMember->m_vLastPosition,
                        vFormationPosition, fRadius, fWidth, 0, &hitInfo) ==
                        CSWSArea::DIRECT_LINE_CLEAR)
                {
                    pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_OFFSET;
                    return CSWSObject::ACTION_COMPLETE;
                }
            }
            {
                float fPathRadius = m_pPathfindInformation->m_fStepScale -
                    NWSCREATURE_FOLLOW_LINE_RADIUS_MARGIN;
                float fPathWidth = m_pPathfindInformation->m_fHalfStep;
                if (CSWSObject::GetArea()->TestDirectLine(pMember->m_vLastPosition,
                        pMember->m_vFollowPosition, fPathRadius, fPathWidth,
                        0, &hitInfo) == CSWSArea::DIRECT_LINE_CLEAR)
                {
                    pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_PATH;
                    return CSWSObject::ACTION_COMPLETE;
                }
            }
            pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_FIND_PATH;
            return CSWSObject::ACTION_IN_PROGRESS;
        }
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00547360
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_OffsetWaitDest(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    int nStartIndex = pMember->m_nFollowPathIndex;
    int nWaitIndex = pMember->m_nFormationPathIndex;
    int nPathIndex = nStartIndex;
    while (nPathIndex != pPaths->m_nCurrentPathData)
    {
        if (nPathIndex == nWaitIndex)
        {
            if (nWaitIndex == nStartIndex)
            {
                Vector vPath = pPaths->GetPathData(nStartIndex)->m_vPosition;
                Vector vFollow = vPath - pMember->m_vFollowPosition;
                Vector vFormation = vPath - pMember->m_vFormationPosition;
                if (vFollow * vFollow > vFormation * vFormation)
                {
                    pMember->m_nCharacterIndex = 1;
                    return CSWSObject::ACTION_IN_PROGRESS;
                }
                return CSWSObject::ACTION_COMPLETE;
            }
            pMember->m_nCharacterIndex = 1;
            return CSWSObject::ACTION_IN_PROGRESS;
        }
        nPathIndex++;
        if (nPathIndex >= 100)
        {
            nPathIndex = 0;
        }
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00547440
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_PathWaitDest(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    int nStartIndex = pMember->m_nFollowPathIndex;
    int nWaitIndex = pMember->m_nFormationPathIndex;
    int nPathIndex = nStartIndex;
    while (nPathIndex != pPaths->m_nCurrentPathData)
    {
        if (nPathIndex == nWaitIndex)
        {
            if (nWaitIndex == nStartIndex)
            {
                Vector vPath = pPaths->GetPathData(nStartIndex)->m_vPosition;
                Vector vFollow = vPath - pMember->m_vFollowPosition;
                Vector vFormation = vPath - pMember->m_vFormationPosition;
                if (vFollow * vFollow > vFormation * vFormation)
                {
                    pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_PATH;
                    return CSWSObject::ACTION_IN_PROGRESS;
                }
                return CSWSObject::ACTION_COMPLETE;
            }
            pMember->m_nCharacterIndex = CSWPartyMemberData::FOLLOW_PATH;
            return CSWSObject::ACTION_IN_PROGRESS;
        }
        nPathIndex++;
        if (nPathIndex >= 100)
        {
            nPathIndex = 0;
        }
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00547520
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_ToOffsetWaitDest(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    int nStartIndex = pMember->m_nFollowPathIndex;
    int nWaitIndex = pMember->m_nFormationPathIndex;
    int nPathIndex = nStartIndex;
    while (nPathIndex != pPaths->m_nCurrentPathData)
    {
        if (nPathIndex == nWaitIndex)
        {
            if (nWaitIndex == nStartIndex)
            {
                Vector vPath = pPaths->GetPathData(nStartIndex)->m_vPosition;
                Vector vFollow = vPath - pMember->m_vFollowPosition;
                Vector vFormation = vPath - pMember->m_vFormationPosition;
                if (vFollow * vFollow > vFormation * vFormation)
                {
                    pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_OFFSET;
                    return CSWSObject::ACTION_IN_PROGRESS;
                }
                return CSWSObject::ACTION_COMPLETE;
            }
            pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_OFFSET;
            return CSWSObject::ACTION_IN_PROGRESS;
        }
        nPathIndex++;
        if (nPathIndex >= 100)
        {
            nPathIndex = 0;
        }
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00547600
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_ToPathWaitDest(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    int nStartIndex = pMember->m_nFollowPathIndex;
    int nWaitIndex = pMember->m_nFormationPathIndex;
    int nPathIndex = nStartIndex;
    while (nPathIndex != pPaths->m_nCurrentPathData)
    {
        if (nPathIndex == nWaitIndex)
        {
            if (nWaitIndex == nStartIndex)
            {
                Vector vPath = pPaths->GetPathData(nStartIndex)->m_vPosition;
                Vector vFollow = vPath - pMember->m_vFollowPosition;
                Vector vFormation = vPath - pMember->m_vFormationPosition;
                if (vFollow * vFollow > vFormation * vFormation)
                {
                    pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_PATH;
                    return CSWSObject::ACTION_IN_PROGRESS;
                }
                return CSWSObject::ACTION_COMPLETE;
            }
            pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_PATH;
            return CSWSObject::ACTION_IN_PROGRESS;
        }
        nPathIndex++;
        if (nPathIndex >= 100)
        {
            nPathIndex = 0;
        }
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x005476E0
int CSWSCreature::WalkUpdateLocation_QuickWalk_FollowLeader_TeleportForward(
    CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
    float& fDistance, int bRun)
{
    int nPathIndex = pMember->m_nFollowPathIndex;
    int nCurrentPath = pPaths->m_nCurrentPathData;
    pPaths->GetPathData(pMember->m_nFormationPathIndex);
    Vector vPosition = pPaths->GetPathData(nPathIndex)->m_vPosition;
    if (++nPathIndex >= 100)
    {
        nPathIndex = 0;
    }
    vPosition = pPaths->GetPathData(nPathIndex)->m_vPosition;
    CPathfindInformation* pPathfind = m_pPathfindInformation;
    while (!GetArea()->TestSafeLocationPoint(vPosition, pPathfind) &&
        nPathIndex != nCurrentPath)
    {
        if (++nPathIndex >= 100)
        {
            nPathIndex = 0;
        }
        vPosition = pPaths->GetPathData(nPathIndex)->m_vPosition;
        pPathfind = m_pPathfindInformation;
    }
    if (nPathIndex != nCurrentPath)
    {
        pMember->m_nFollowPathIndex = nPathIndex;
        pMember->m_vFollowPosition = vPosition;
        pMember->m_vLastPosition = vPosition;
        fDistance = 0.0f;
        pMember->m_nCharacterIndex = CSWPartyMemberData::MOVING_TO_PATH;
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00547810
float CSWSCreature::ComputeSpeedFactor(float fDistance, int nTime)
{
    float fSpeedFactor;
    float fFactor;
    float fNewSpeedFactor;
    float fWalkDistance;
    float fDistanceScale;
    float fMaximumSpeedFactor;

    fWalkDistance = ComputeTotalWalkDistance();
    fDistanceScale = fDistance;
    fDistanceScale *= 1000.0f;
    fDistanceScale *= 0.0000005f;
    fSpeedFactor = 1.0f;

    if (fWalkDistance <= fDistanceScale)
    {
        fFactor = fWalkDistance / fDistanceScale;
        if (fFactor >= 0.01f)
        {
            fNewSpeedFactor = sqrt(fFactor);
        }
        else
        {
            fNewSpeedFactor = 0.1f;
        }
        fSpeedFactor = fNewSpeedFactor - nTime * 0.001f;
    }

    if (m_fMovementRate <= 1.0f)
    {
        fMaximumSpeedFactor = nTime * 0.001f + m_fMovementRate;
        if (fSpeedFactor >= fMaximumSpeedFactor)
        {
            fSpeedFactor = fMaximumSpeedFactor;
        }
    }

    if (fSpeedFactor <= 0.1f)
    {
        return 0.1f;
    }
    return fSpeedFactor;
}
// PC 0x005478D0
int CSWSCreature::WalkUpdateLocationTestDistance(
    const Vector& vStart, const Vector& vEnd,
    Vector* pvHitPosition, unsigned long* poidHitCreature)
{
    int bCanWalk = TRUE;
    if (GetArea() != NULL)
    {
        unsigned long oidOldTarget = m_pPathfindInformation->m_oidPathfindTargetCreature;
        m_pPathfindInformation->m_oidPathfindTargetCreature = CGameObjectArray::INVALIDOBJID;
        float fMinHeight;
        float fMaxHeight;
        if (vStart.m_fZ < vEnd.m_fZ)
        {
            fMinHeight = vStart.m_fZ;
            fMaxHeight = vEnd.m_fZ;
        }
        else
        {
            fMinHeight = vEnd.m_fZ;
            fMaxHeight = vStart.m_fZ;
        }
        fMinHeight -= NWSCREATURE_WALK_HEIGHT_MARGIN;
        float fHalfStep = m_pPathfindInformation->m_fHalfStep;
        float fTop = fHalfStep + fMaxHeight + NWSCREATURE_WALK_HEIGHT_MARGIN;
        unsigned long oidHitObject = OBJECT_INVALID;
        unsigned long oidHitCreature = OBJECT_INVALID;
        CWalkHitInfo hitInfo;
        unsigned long oidIgnore = m_pPathfindInformation->m_nField_BC;
        float fRadius = m_pPathfindInformation->m_fStepScale;
        int bNoNonWalkPolys = GetArea()->NoNonWalkPolysDetailed(
            vStart, vEnd, fMinHeight, fTop,
            fRadius,
            oidIgnore, oidHitObject,
            TRUE, NULL, FALSE, &hitInfo, FALSE);
        if (!bNoNonWalkPolys || !GetArea()->NoCreaturesOnLine(
                vStart, vEnd, m_pPathfindInformation, TRUE, 0,
                &oidHitCreature, TRUE, &hitInfo))
        {
            bCanWalk = FALSE;
            if (poidHitCreature != NULL)
                *poidHitCreature = oidHitCreature;
            if (pvHitPosition != NULL)
                *pvHitPosition = hitInfo.m_vHitPosition;
            if (oidHitCreature != OBJECT_INVALID)
            {
                CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->
                    GetCreatureByGameObjectID(oidHitCreature);
                if (pCreature != NULL && BumpFriends(pCreature, vEnd, vStart))
                {
                    unsigned long oidBlocker;
                    if (GetArea()->NoCreaturesOnLine(
                            vStart, vEnd, m_pPathfindInformation, TRUE, 0,
                            &oidBlocker, FALSE, NULL) == TRUE)
                        bCanWalk = TRUE;
                }
            }
            else if (!bNoNonWalkPolys && oidHitObject != OBJECT_INVALID)
            {
                CSWSDoor* pDoor = g_pAppManager->GetServerExoApp()->
                    GetDoorByGameObjectID(oidHitObject);
                if (pDoor != NULL && pDoor->m_nAnimation == DOOR_ANIMATION_CLOSED)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, oidHitObject, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
            }
        }
        m_pPathfindInformation->m_oidPathfindTargetCreature = oidOldTarget;
    }
    return bCanWalk;
}
// PC 0x00547BE0
int CSWSCreature::AIActionForceFollowObject(CSWSObjectActionNode* pAction)
{
    if (GetDead() || GetIsPCDying() || GetDeadTemp())
        return ACTION_FAILED;

    unsigned long oidTarget;
    int bRun;
    Vector vOffset;
    int bFollowing;
    Vector vLastPosition;
    float fWait = NWSCREATURE_FORCE_FOLLOW_WAIT;

    EXOXBOXASSERT(pAction->m_nNumParams == 7);
    oidTarget = (unsigned long)pAction->m_apParam[0];
    bRun = (int)pAction->m_apParam[1];
    vOffset.m_fX = *(float*)&pAction->m_apParam[2];
    vOffset.m_fY = *(float*)&pAction->m_apParam[3];
    bFollowing = (int)pAction->m_apParam[4];
    vLastPosition.m_fX = *(float*)&pAction->m_apParam[5];
    vLastPosition.m_fY = *(float*)&pAction->m_apParam[6];

    EXOXBOXASSERT(vOffset.m_fX <= 100000.0f && vOffset.m_fX >= -100000.0f && vOffset.m_fY <= 100000.0f && vOffset.m_fY >= -100000.0f);
    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pObject == NULL || pObject->AsSWSObject() == NULL)
        return ACTION_FAILED;

    Vector vLeaderPosition = g_pAppManager->GetClientExoApp()->GetSWParty()->m_vLeaderFollowPosition;
    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
    Vector vPosition = vLeaderPosition + pParty->m_qLeaderFacing * vOffset;
    unsigned long oidArea = pObject->AsSWSObject()->m_oidArea;
    Vector vDelta = vPosition - vLastPosition;

    if (!bFollowing)
    {
        bFollowing = vDelta * vDelta <=
            NWSCREATURE_FORCE_FOLLOW_POSITION_TOLERANCE *
            NWSCREATURE_FORCE_FOLLOW_POSITION_TOLERANCE;
        AddActionToFront(ACTION_FORCE_FOLLOW_OBJECT, ACTION_GROUP_NEW,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
            ACTION_PARAM_FLOAT, &vOffset.m_fX, ACTION_PARAM_FLOAT, &vOffset.m_fY,
            ACTION_PARAM_INTEGER, &bFollowing, ACTION_PARAM_FLOAT, &vPosition.m_fX,
            ACTION_PARAM_FLOAT, &vPosition.m_fY,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        if (m_bInParty)
            fWait = NWSCREATURE_FORCE_FOLLOW_PARTY_WAIT;
        AddActionToFront(ACTION_WAIT, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &fWait, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition,
            oidArea, oidTarget, bRun, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0,
            vOffset, 0, 0, 0);
        return ACTION_COMPLETE;
    }
    else
    {
        float fRange = g_pAppManager->GetClientExoApp()->GetSWParty()->GetFollowRange();
        if (vDelta * vDelta >= fRange * fRange)
        {
            bFollowing = FALSE;
            AddActionToFront(ACTION_FORCE_FOLLOW_OBJECT, ACTION_GROUP_NEW,
                ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
                ACTION_PARAM_FLOAT, &vOffset.m_fX, ACTION_PARAM_FLOAT, &vOffset.m_fY,
                ACTION_PARAM_INTEGER, &bFollowing, ACTION_PARAM_FLOAT, &vPosition.m_fX,
                ACTION_PARAM_FLOAT, &vPosition.m_fY,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            if (m_bInParty)
                fWait = NWSCREATURE_FORCE_FOLLOW_PARTY_WAIT;
            AddActionToFront(ACTION_WAIT, pAction->m_nGroupId,
                ACTION_PARAM_FLOAT, &fWait, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL);
            AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition,
                oidArea, oidTarget, bRun, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0,
                vOffset, 0, 0, 0);
            return ACTION_COMPLETE;
        }
    }
    return ACTION_IN_PROGRESS;
}
// PC 0x005480C0
int CSWSCreature::AIActionCheckForceFollowObject(CSWSObjectActionNode* pAction)
{
    Vector vPosition = m_vPosition;
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    int bRun = (int)pAction->m_apParam[1];
    Vector vDestination;
    memcpy(&vDestination.m_fX, &pAction->m_apParam[2], sizeof(float));
    memcpy(&vDestination.m_fY, &pAction->m_apParam[3], sizeof(float));
    int bStarted = 0;

    EXOXBOXASSERT(vDestination.m_fX <= 100000.0f && vDestination.m_fX >= -100000.0f && vDestination.m_fY <= 100000.0f && vDestination.m_fY >= -100000.0f);
    AddActionToFront(ACTION_CHECK_FORCE_FOLLOW_OBJECT, ACTION_GROUP_NEW,
        ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
        ACTION_PARAM_FLOAT, &vDestination.m_fX,
        ACTION_PARAM_FLOAT, &vDestination.m_fY,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
        0, NULL, 0, NULL, 0, NULL);
    AddActionToFront(ACTION_FORCE_FOLLOW_OBJECT, ACTION_GROUP_NEW,
        ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
        ACTION_PARAM_FLOAT, &vDestination.m_fX,
        ACTION_PARAM_FLOAT, &vDestination.m_fY,
        ACTION_PARAM_INTEGER, &bStarted,
        ACTION_PARAM_FLOAT, &vPosition.m_fX,
        ACTION_PARAM_FLOAT, &vPosition.m_fY,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);

    return ACTION_COMPLETE;
}
// PC 0x005481D0
int CSWSCreature::AIActionOrientCamera(CSWSObjectActionNode* pAction)
{
    int nX;
    int nY;
    int nUpdateMask;
    CSWSPlayer* pPlayer;
    CSWSMessage* pMessage;
    int nZ;
    int nMode;

    nUpdateMask = (int)pAction->m_apParam[0];
    nX = (int)pAction->m_apParam[1];
    nY = (int)pAction->m_apParam[2];
    nZ = (int)pAction->m_apParam[3];
    nMode = (int)pAction->m_apParam[4];
    pPlayer =
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(m_nId);

    if (pPlayer != NULL)
    {
        pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
        if (pMessage != NULL)
        {
            pMessage->SendServerToPlayerCamera_ChangeLocation(
                pPlayer, nUpdateMask, *(float*)&nX, *(float*)&nY,
                *(float*)&nZ, nMode);
        }
    }

    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00548240
int CSWSCreature::AIActionDropItem(CSWSObjectActionNode* pAction)
{
    CItemRepository* pRepository = NULL;
    Vector vPosition;
    int bItemOwned = 0;

    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);

    unsigned long oidItem = (unsigned long)pAction->m_apParam[0];
    float fX;
    float fY;
    float fZ;
    memcpy(&fX, &pAction->m_apParam[1], sizeof(float));
    memcpy(&fY, &pAction->m_apParam[2], sizeof(float));
    memcpy(&fZ, &pAction->m_apParam[3], sizeof(float));
    int bGold = (int)pAction->m_apParam[4];
    vPosition = Vector(fX, fY, fZ);

    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    CSWSPlayer* pPlayer =
        g_pAppManager->GetServerExoApp()->GetExoAppInternal()->GetClientObjectByObjectId(m_nId);

    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
    {
        if (pMessage != NULL && pPlayer != NULL)
            pMessage->SendServerToPlayerInventory_DropCancel(pPlayer->m_nPlayerId, oidItem);
        m_bUnlockObjectFacingSet = 0;
        return ACTION_FAILED;
    }

    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        if (pItem->m_oidPossessor == GetId())
        {
            bItemOwned = 1;
            pRepository = GetItemRepository(1);
        }
        else
        {
            CSWSItem* pContainer = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(
                pItem->m_oidPossessor);
            if (pContainer != NULL)
            {
                if (pContainer->m_oidPossessor == GetId())
                {
                    pRepository = pContainer->m_pRepository;
                    bItemOwned = 1;
                }
                else
                {
                    if (pMessage != NULL && pPlayer != NULL)
                        pMessage->SendServerToPlayerInventory_DropCancel(pPlayer->m_nPlayerId, oidItem);
                    m_bUnlockObjectFacingSet = 0;
                    return ACTION_FAILED;
                }
            }
            else
            {
                if (pMessage != NULL && pPlayer != NULL)
                    pMessage->SendServerToPlayerInventory_DropCancel(pPlayer->m_nPlayerId, oidItem);
                m_bUnlockObjectFacingSet = 0;
                return ACTION_FAILED;
            }
        }
    }

    Vector vDifference = m_vPosition - vPosition;
    float fDistanceSquared = vDifference * vDifference;
    if (fDistanceSquared > NWSCREATURE_DROP_ITEM_DISTANCE_SQUARED)
    {
        int bRun;
        if (fDistanceSquared >= NWSCREATURE_DROP_ITEM_RUN_DISTANCE_SQUARED)
            bRun = 1;
        AddActionToFront(ACTION_DROP_ITEM, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_FLOAT, &fX,
            ACTION_PARAM_FLOAT, &fY, ACTION_PARAM_FLOAT, &fZ,
            ACTION_PARAM_INTEGER, &bGold, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_POINT, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &vPosition.m_fX, ACTION_PARAM_FLOAT, &vPosition.m_fY,
            ACTION_PARAM_FLOAT, &vPosition.m_fZ, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        vDifference = normalize(m_vPosition - vPosition);
        vPosition = vPosition + vDifference * 1.0f;
        AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition, m_oidArea,
            OBJECT_INVALID, bRun, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0,
            Vector(0.0f, 0.0f, 0.0f), 0, 0, 0);
        return ACTION_COMPLETE;
    }

    if (m_bUnlockObjectFacingSet == 0)
    {
        int bWait = 1;
        float fSpeed = 1.0f;
        float fDuration = 1.5f;
        int nAnimation = vPosition.m_fZ > 1.0f ?
            CREATURE_ANIMATION_LOOPING_GET_MID : CREATURE_ANIMATION_LOOPING_GET_LOW;
        AddActionToFront(ACTION_DROP_ITEM, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidItem, ACTION_PARAM_FLOAT, &fX,
            ACTION_PARAM_FLOAT, &fY, ACTION_PARAM_FLOAT, &fZ,
            ACTION_PARAM_INTEGER, &bGold, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_PLAY_ANIMATION, pAction->m_nGroupId,
            ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
            ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bWait,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_POINT, pAction->m_nGroupId,
            ACTION_PARAM_FLOAT, &vPosition.m_fX, ACTION_PARAM_FLOAT, &vPosition.m_fY,
            ACTION_PARAM_FLOAT, &vPosition.m_fZ, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        m_bUnlockObjectFacingSet = 1;
        return ACTION_COMPLETE;
    }

    m_bUnlockObjectFacingSet = 0;
    if (pRepository != NULL)
    {
        if (bItemOwned == 1 && bGold == 0)
        {
            if (pItem->m_pRepository != NULL && pPlayer->m_pContainerGUI != NULL &&
                pPlayer->m_pContainerGUI->m_oidContainer == pItem->GetId())
                pItem->CloseInventory(GetId(), 1);
            if (RemoveItem(pItem, 1, 1, NULL) == 1)
            {
                pItem->AddToArea(GetArea(), fX, fY, fZ, 1);
                if (pPlayer != NULL && pMessage != NULL)
                    pMessage->SendServerToPlayerInventory_Drop(pPlayer->m_nPlayerId, pItem->GetId());
                return ACTION_COMPLETE;
            }
        }
        if (bGold == 1)
        {
            RemoveGold(pItem->m_nStackSize, 1);
            pItem->SetPossessor(OBJECT_INVALID, 0, 0, OBJECT_INVALID);
            pItem->AddToArea(GetArea(), fX, fY, fZ, 1);
            if (pPlayer != NULL && pMessage != NULL)
                pMessage->SendServerToPlayerInventory_Drop(pPlayer->m_nPlayerId, pItem->GetId());
            return ACTION_COMPLETE;
        }
    }
    if (pMessage != NULL && pPlayer != NULL)
        pMessage->SendServerToPlayerInventory_DropCancel(pPlayer->m_nPlayerId, oidItem);
    return ACTION_FAILED;
}
// PC 0x00548840
unsigned char CSWSCreature::CanUnEquipWeapon(CSWSItem* pItem)
{
    CSWSItem* pRightItem;
    CSWSItem* pLeftItem;
    CSWSPlayer* pPlayer;

    pRightItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    pLeftItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);

    if (pRightItem != NULL && pRightItem->m_nId == pItem->m_nId &&
        pLeftItem != NULL &&
        g_pRules->m_pBaseItems->GetBaseItem(pLeftItem->GetBaseItemType())
                ->m_nWeaponType != 0)
    {
        pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(
            m_nId);
        if (pPlayer != NULL)
        {
            pPlayer->m_pInventoryGUI->m_pLastUpdateObject->m_oidSlot[4] =
                OBJECT_INVALID;
            pPlayer->m_pInventoryGUI->m_pLastUpdateObject->m_oidSlot[5] =
                OBJECT_INVALID;
        }
        return 2;
    }

    return 1;
}
// PC 0x005488D0
unsigned char CSWSCreature::CanUnEquipWeapon2(CSWSItem* pItem)
{
    CSWSItem* pRightItem;
    CSWSItem* pLeftItem;
    CSWSPlayer* pPlayer;

    pRightItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND2);
    pLeftItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND2);

    if (pRightItem != NULL && pRightItem->m_nId == pItem->m_nId &&
        pLeftItem != NULL &&
        g_pRules->m_pBaseItems->GetBaseItem(pLeftItem->GetBaseItemType())
                ->m_nWeaponType != 0)
    {
        pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(
            m_nId);
        if (pPlayer != NULL)
        {
            pPlayer->m_pInventoryGUI->m_pLastUpdateObject->m_oidSlot[15] =
                OBJECT_INVALID;
            pPlayer->m_pInventoryGUI->m_pLastUpdateObject->m_oidSlot[16] =
                OBJECT_INVALID;
        }
        return 3;
    }

    return 1;
}
// PC 0x00548960
int CSWSCreature::AIActionUnequipItem(CSWSObjectActionNode* pAction)
{
    unsigned long oidItem;
    unsigned long oidTarget;
    int bUnknown;
    CSWSPlayer* pPlayer;
    CSWSMessage* pMessage;

    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);

    oidItem = (unsigned long)pAction->m_apParam[0];
    oidTarget = (unsigned long)pAction->m_apParam[1];
    bUnknown = (int)pAction->m_apParam[2];
    int nArg6 = (int)pAction->m_apParam[3];
    int bMerge = (int)pAction->m_apParam[4];

    if (!GetDead() && !GetIsPCDying() && !GetDeadTemp())
    {
        if (RunUnequip(oidItem, oidTarget, bMerge, NULL, bUnknown, nArg6) == 1)
        {
            return CSWSObject::ACTION_COMPLETE;
        }
    }

    pPlayer =
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(GetId());
    pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    if (pPlayer != NULL && pMessage != NULL)
    {
        pMessage->SendServerToPlayerInventory_UnequipCancel(
            pPlayer->m_nPlayerId, oidItem);
    }

    return CSWSObject::ACTION_FAILED;
}
// PC 0x00548A30
int CSWSCreature::AIActionWaitForEndOfRound(CSWSObjectActionNode*)
{
    if (((CSWSObject*)this)->GetDead() == 1 ||
        ((CSWSObject*)this)->GetIsPCDying() == 1 ||
        GetDeadTemp() == 1)
    {
        return CSWSObject::ACTION_FAILED;
    }

    if (m_pcCombatRound->m_bRoundStarted != 1 ||
        m_pcCombatRound->GetSpellActionPending() != 0)
    {
        return CSWSObject::ACTION_COMPLETE;
    }

    return CSWSObject::ACTION_IN_PROGRESS;
}
// PC 0x00548AA0
CSWSFaction* CSWSCreature::GetFaction()
{
    CFactionManager* pFactionManager;

#ifdef _XBOX
    CServerExoAppInternal* pInternal =
        g_pAppManager->m_pServerExoApp->m_pcExoAppInternal;
    EXOXBOXASSERT(pInternal);
    pFactionManager = pInternal->m_pFactionManager;
    EXOXBOXASSERT(pFactionManager);
#else
    pFactionManager =
        g_pAppManager->m_pServerExoApp->m_pcExoAppInternal->m_pFactionManager;
#endif
    return pFactionManager->GetFaction(m_pStats->m_nFactionId);
}
// PC 0x00548AD0
int CSWSCreature::GetCreatureReputation(unsigned long oidCreature, int nFaction)
{
    int nFaction1;
    int nFaction2;
    CSWSCreature* pCreature;
    CServerExoApp* pServer;

    nFaction1 = m_pStats->m_nFactionId;
    pServer = g_pAppManager->m_pServerExoApp;
    nFaction2 = nFaction;
    pCreature = pServer->GetCreatureByGameObjectID(oidCreature);
    if (m_bInParty != 0)
    {
        if (pCreature != NULL && pCreature->m_bInParty != 0)
        {
            return 100;
        }
    }
    if (m_bInParty != 0)
    {
        nFaction1 = 0;
    }
    if (pCreature != NULL && pCreature->m_bInParty != 0)
    {
        nFaction2 = 0;
    }
#ifdef _XBOX
    CServerExoAppInternal* pInternal = g_pAppManager->m_pServerExoApp->m_pcExoAppInternal;
    EXOXBOXASSERT(pInternal);
    return pInternal->m_pFactionManager
#else
    return g_pAppManager->m_pServerExoApp->m_pcExoAppInternal->m_pFactionManager
#endif
        ->GetNPCFactionReputation(nFaction2, nFaction1);
}
// PC 0x00548B50
void CSWSCreature::AdjustReputation(int nFaction, int nAdjustment)
{
    CFactionManager* pFactionManager;
    int nReputation;

#ifdef _XBOX
    CServerExoAppInternal* pInternal =
        g_pAppManager->m_pServerExoApp->m_pcExoAppInternal;
    EXOXBOXASSERT(pInternal);
    pFactionManager = pInternal->m_pFactionManager;
    EXOXBOXASSERT(pFactionManager);
#else
    pFactionManager =
        g_pAppManager->m_pServerExoApp->m_pcExoAppInternal->m_pFactionManager;
#endif
    if (pFactionManager->GetIsNPCFaction(nFaction) != 0)
    {
        int nFactionId = m_pStats->m_nFactionId;
        nReputation = pFactionManager->GetNPCFactionReputation(
            nFactionId, nFaction);
        if (m_pStats->m_nFactionId != nFaction)
        {
            pFactionManager->SetNPCFactionReputation(
                m_pStats->m_nFactionId, nFaction, nReputation + nAdjustment);
        }
    }
}

extern int g_bRunSubAreaScripts;
// PC 0x00548BB0
int CSWSCreature::AIActionPickPocket(CSWSObjectActionNode*)
{
    return CSWSObject::ACTION_FAILED;
}
// PC 0x00548BB0
int CSWSCreature::AIActionAnimalEmpathy(CSWSObjectActionNode*)
{
    return CSWSObject::ACTION_FAILED;
}
// PC 0x00548BB0
int CSWSCreature::AIActionTaunt(CSWSObjectActionNode*)
{
    return CSWSObject::ACTION_FAILED;
}
// PC 0x00548BC0
int CSWSCreature::AIActionRest(CSWSObjectActionNode*)
{
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00548BD0
int CSWSCreature::AIActionAppear(CSWSObjectActionNode*)
{
    CServerExoApp* pServerApp;
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nElapsedDays;
    unsigned long nElapsedTime;

    pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nActionStartDay, m_nActionStartTime, &nElapsedDays, &nElapsedTime);
    if (nElapsedTime < APPEAR_ANIMATION_DURATION)
    {
        m_fSyncHeight = 1.0f;
        SetAnimation(CREATURE_ANIMATION_APPEAR);
        return CSWSObject::ACTION_IN_PROGRESS;
    }

    if (AsSWSCreature() != NULL)
    {
        m_fSyncHeight = 1.0f;
        SetAnimation(CREATURE_ANIMATION_COMBAT_READY);
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00548C90
int CSWSCreature::AIActionDisappear(CSWSObjectActionNode*)
{
    CServerExoApp* pServerApp;
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nElapsedDays;
    unsigned long nElapsedTime;

    pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nActionStartDay, m_nActionStartTime, &nElapsedDays, &nElapsedTime);
    if (nElapsedTime < DISAPPEAR_ANIMATION_DURATION)
    {
        m_fSyncHeight = 1.0f;
        SetAnimation(CREATURE_ANIMATION_DISAPPEAR);
        return CSWSObject::ACTION_IN_PROGRESS;
    }

    if (AsSWSCreature() != NULL)
    {
        m_fSyncHeight = 1.0f;
        SetAnimation(CREATURE_ANIMATION_COMBAT_READY);
    }
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00548D50
int CSWSCreature::AIActionCounterSpell(CSWSObjectActionNode* pAction)
{
    unsigned long oidTarget;
    float fRange;
    int bRun;
    int bCheckRange;
    Vector vTarget;
    if ((GetDead() == TRUE && GetIsPCDying() == TRUE) || GetDeadTemp() == TRUE)
        return ACTION_FAILED;

    ActivityManager(ACTIVITY_ACTION_COUNTERSPELL);
    oidTarget = (unsigned long)pAction->m_apParam[0];
    CSWSCreature* pTarget =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
    if (pTarget == NULL || pTarget->GetDead() || pTarget->GetIsPCDying() ||
        pTarget->GetDeadTemp())
        return ACTION_FAILED;

    CSWSArea* pArea = pTarget->GetArea();
    if (pArea != NULL)
    {
        vTarget = pTarget->m_vPosition;
        fRange = g_pRules->m_afPrimaryRange[CSWRules::RANGE_LONG];
        bRun = TRUE;
        bCheckRange = TRUE;
        Vector vDelta = m_vPosition - vTarget;
        if (pArea->GetId() == GetArea()->GetId() &&
            !(vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
                vDelta.m_fZ * vDelta.m_fZ > fRange * fRange))
        {
            BroadcastCombatStateToParty(oidTarget);
            ResolveInitiative();
            SetCombatMode(SWSOBJECT_COMBAT_MODE_COUNTERSPELL, FALSE);
            if (m_oidCounterSpellTarget != oidTarget)
                m_oidCounterSpellTarget = oidTarget;
            if (GetLockOrientationToObject() != oidTarget)
                SetLockOrientationToObject(oidTarget, 0);
            return ACTION_IN_PROGRESS;
        }
        if (!(m_nAIStateFlags & AI_STATE_CAN_MOVE))
        {
            SendFeedbackMessage(FEEDBACK_CANNOT_MOVE, NULL);
            return ACTION_FAILED;
        }
        AddActionToFront(ACTION_COUNTER_SPELL, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
            ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_FLOAT, &fRange,
            ACTION_PARAM_INTEGER, &bCheckRange, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        AddMoveToPointActionToFront(pAction->m_nGroupId, vTarget, pArea->GetId(),
            oidTarget, bRun, fRange, 0.0f, 0, 0, 0, 0, 0, 0,
            Vector(), 0, 0, 0);
        AddPathfindingWaitActionToFront(pAction->m_nGroupId);
        return ACTION_COMPLETE;
    }
    return ACTION_IN_PROGRESS;
}
// PC 0x00549080
int CSWSCreature::AIActionStanceTotalDefense(CSWSObjectActionNode* pNode)
{
    m_nAttackMovementTimer = ATTACK_MOVEMENT_TIMEOUT;
    if (GetInParty())
    {
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        int nMember = pClient->GetSWParty()->GetIndex(pClient->ServerToClientObjectId(GetId()));
        if (nMember >= 0)
        {
            CSWParty* pParty = pClient->GetSWParty();
            pParty->m_aPartyMemberData[nMember].SetCombatMessage(
                CSWPartyMemberData::COMBAT_MESSAGE_ENGAGED);
        }
    }
    int bDeadTemp = GetDeadTemp();
    if (GetDead() || bDeadTemp || m_pcCombatRound == NULL)
    {
        CancelAllCombatModes(1);
        return CSWSObject::ACTION_FAILED;
    }
    if (g_pAppManager->GetServerExoApp()->GetPauseState(
            CServerExoAppInternal::PAUSE_TYPE_COMBAT_MASK) ||
        m_pcCombatRound->m_bRoundPaused)
    {
        return CSWSObject::ACTION_IN_PROGRESS;
    }
    unsigned long oidTarget = (unsigned long)pNode->m_apParam[0];
    if (m_n10B4 == 0 && m_n10B8 == 0)
    {
        SetAnimation(CREATURE_ANIMATION_COMBAT_READY);
        SetAttemptedAttackTarget(oidTarget);
        SetLockOrientationToObject(oidTarget, 0);
        ActivityManager(ACTIVITY_ACTION_COUNTERSPELL);
    }
    if (m_c110C != 0)
    {
        m_c110C = 0;
    }
    if (!m_pcCombatRound->m_bRoundStarted)
    {
        int bEngaged = GetCombatRound()->ResolveEngagedState(oidTarget);
        int bMaster = GetCombatRound()->ResolveMasterState(oidTarget, bEngaged);
        if (bEngaged)
        {
            if (bMaster)
            {
                g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
                    (unsigned long*)&m_n10B4, (unsigned long*)&m_n10B8);
                GetCombatRound()->StartCombatRound(oidTarget, bEngaged, bMaster, 0, 0);
                CSWSCreature* pTarget =
                    g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
                if (pTarget != NULL)
                {
                    pTarget->m_pcCombatRound->StartCombatRound(GetId(), bEngaged, 0, 0, 0);
                    pTarget->m_bResolveInitiative = 1;
                }
            }
        }
        else
        {
            g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
                (unsigned long*)&m_n10B4, (unsigned long*)&m_n10B8);
            GetCombatRound()->StartCombatRound(oidTarget, bEngaged, bMaster, 0, 0);
        }
    }
    if (!m_pcCombatRound->m_bRoundStarted)
    {
        return CSWSObject::ACTION_IN_PROGRESS;
    }
    unsigned long nDay, nTime;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    unsigned long nElapsedDays, nElapsedTime;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_n10B4, m_n10B8, &nElapsedDays, &nElapsedTime);
    if (nElapsedTime >= 1500)
    {
        m_n10B8 = 0;
        m_c110C = -1;
        m_n10B4 = 0;
        return CSWSObject::ACTION_COMPLETE;
    }
    return CSWSObject::ACTION_IN_PROGRESS;
}
// PC 0x00549350
int CSWSCreature::CheckItemAlignmentRestrictions(CSWSItem* pItem)
{
    if (!pItem->GetPropertyByTypeExists(
            CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_ALIGNMENT_GROUP, 0))
    {
        return 1;
    }
    if (m_pStats->HasFeat(CSWFeat::FEAT_PROFICIENCY_ALL))
    {
        return 1;
    }

    short nAlignment = m_pStats->m_nGoodEvilAlignment;
    for (int i = 0; i < pItem->m_nPassiveProperties; i++)
    {
        CSWItemProperty* pProperty = pItem->GetPassiveProperty(i);
        if (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
            (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType)))
        {
            if (pItem->GetPassiveProperty(i)->m_nPropertyType ==
                CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_ALIGNMENT_GROUP)
            {
                switch (pItem->GetPassiveProperty(i)->m_nSubtype)
                {
                case 0:
                    return 1;
                case 1:
                    if (nAlignment < 60 && nAlignment > 40)
                        return 1;
                    break;
                case 2:
                    if (nAlignment >= 60)
                        return 1;
                    break;
                case 3:
                    if (nAlignment <= 40)
                        return 1;
                    break;
                case 4:
                    if (nAlignment == 0)
                        return 1;
                    break;
                case 5:
                    if (nAlignment == 100)
                        return 1;
                    break;
                }
            }
        }
    }
    return 0;
}
// PC 0x00549630
int CSWSCreature::CheckItemFeatRestrictions(CSWSItem* pItem)
{
    if (!pItem->GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_FEAT, 0))
    {
        return 1;
    }
    if (m_pStats->HasFeat(CSWFeat::FEAT_PROFICIENCY_ALL))
    {
        return 1;
    }

    for (int i = 0; i < pItem->m_nPassiveProperties; i++)
    {
        CSWItemProperty* pProperty = pItem->GetPassiveProperty(i);
        if (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
            (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType)))
        {
            if (pItem->GetPassiveProperty(i)->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_FEAT)
            {
                unsigned short nFeat = pItem->GetPassiveProperty(i)->m_nSubtype;
                if (m_pStats->GetFeatRemainingUses(nFeat) <= 0)
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}
// PC 0x005496E0
int CSWSCreature::CheckItemGenderRestrictions(CSWSItem* pItem)
{
    if (!pItem->GetPropertyByTypeExists(
            CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_GENDER, 0))
    {
        return 1;
    }

    for (int i = 0; i < pItem->m_nPassiveProperties; i++)
    {
        CSWItemProperty* pProperty = pItem->GetPassiveProperty(i);
        if (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
            (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType)))
        {
            if (pItem->GetPassiveProperty(i)->m_nPropertyType ==
                CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_GENDER)
            {
                int nGender = pItem->GetPassiveProperty(i)->m_nSubtype;
                int nMyGender = GetGender();
                if (nGender != nMyGender &&
                    ((nGender != 0 && nGender != 1) || nMyGender != 2))
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}
// PC 0x00549790
int CSWSCreature::CheckItemPCRestrictions(CSWSItem* pItem)
{
    CExoString sExpectedTag;

    if (pItem->GetPropertyByTypeExists(
            CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_PC, 0))
    {
        unsigned char bAllowed = 0;
        int nRestrictions = 0;
        for (int i = 0; i < pItem->m_nPassiveProperties; i++)
        {
            CSWItemProperty* pProperty = pItem->GetPassiveProperty(i);
            if (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType)))
            {
                if (pItem->GetPassiveProperty(i)->m_nPropertyType ==
                    CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_PC)
                {
                    nRestrictions++;
                    CSWItemProperty* pRestriction = pItem->GetPassiveProperty(i);
                    int nSubtype = pRestriction->m_nSubtype;
                    if (nSubtype == 0)
                    {
                        if (m_bPlayerCreated != 0)
                        {
                            bAllowed = 1;
                        }
                    }
                    else
                    {
                        g_pRules->Get2DArrays()->m_pIPRPPC->GetCExoStringEntry(
                            nSubtype, CExoString("ExpectedTag"), &sExpectedTag);
                        if (stricmp(sExpectedTag.CStr(), m_sTag.CStr()) == 0)
                        {
                            bAllowed = 1;
                        }
                    }
                }
            }
        }
        if (nRestrictions > 0 && !bAllowed)
        {
            return 0;
        }
    }
    return 1;
}
// PC 0x00549930
int CSWSCreature::CheckItemAttributeRestrictions(CSWSItem* pItem)
{
    if (pItem->GetPropertyByTypeExists(
            CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_ATTRIBUTE, 0))
    {
        for (int i = 0; i < pItem->m_nPassiveProperties; i++)
        {
            CSWItemProperty* pProperty = pItem->GetPassiveProperty(i);
            if (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType)))
            {
                if (pItem->GetPassiveProperty(i)->m_nPropertyType ==
                    CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_ATTRIBUTE)
                {
                    int nAbility = pItem->GetPassiveProperty(i)->m_nSubtype;
                    int nRequired;
                    int nScore;
                    {
                        CExoString sColumn("Value");
                        unsigned short nCostValue =
                            pItem->GetPassiveProperty(i)->m_nCostValue;
                        g_pRules->Get2DArrays()->GetIPRPCostTable(26)->GetINTEntry(
                            nCostValue, sColumn, &nRequired);
                    }
                    switch (nAbility)
                    {
                    case CSWRules::ABILITY_STRENGTH:
                        nScore = m_pStats->m_nStrengthBase;
                        break;
                    case CSWRules::ABILITY_DEXTERITY:
                        nScore = m_pStats->m_nDexterityBase;
                        break;
                    case CSWRules::ABILITY_CONSTITUTION:
                        nScore = m_pStats->m_nConstitutionBase;
                        break;
                    case CSWRules::ABILITY_INTELLIGENCE:
                        nScore = m_pStats->m_nConstitutionBase;
                        break;
                    case CSWRules::ABILITY_WISDOM:
                        nScore = m_pStats->m_nWisdomBase;
                        break;
                    case CSWRules::ABILITY_CHARISMA:
                        nScore = m_pStats->m_nCharismaBase;
                        break;
                    }
                    if (nScore < nRequired)
                    {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}
// PC 0x00549AF0
int CSWSCreature::CheckItemCNPCRestrictions(CSWSItem* pItem)
{
    if (m_sTag.CompareNoCase("baodur"))
    {
        int nBaseItem = pItem->m_nBaseItem;
        if (nBaseItem == 35 || nBaseItem == 36 || nBaseItem == 37 ||
            nBaseItem == 97 || nBaseItem == 98 || nBaseItem == 99 ||
            nBaseItem == 100 || nBaseItem == 102)
        {
            return 0;
        }
    }
    return 1;
}
// PC 0x00549B40
void CSWSCreature::SetFormation(unsigned long oidLeader, int nFormation,
                                int nPosition)
{
    CSWSObject* pLeader = static_cast<CSWSObject*>(
        g_pAppManager->GetServerExoApp()->GetGameObject(oidLeader));
    if (pLeader == NULL)
    {
        return;
    }
    CExoString sAngleColumn;
    sAngleColumn.Format("ANGLE_%d", nPosition);
    CExoString sDistanceColumn;
    sDistanceColumn.Format("DISTANCE_%d", nPosition);
    int nAngle, nDistance;
    if (g_pRules->Get2DArrays()->GetFormations()->GetINTEntry(
            nFormation, sAngleColumn, &nAngle) == 0 ||
        g_pRules->Get2DArrays()->GetFormations()->GetINTEntry(
            nFormation, sDistanceColumn, &nDistance) == 0)
    {
        return;
    }
    Vector vPosition;
    Vector vOffset((float)nDistance, 0.0f, 0.0f);
    Quaternion qRotation(Vector(0.0f, 0.0f, 1.0f),
        (Yaw(pLeader->m_vOrientation) + nAngle) * DEG_TO_RAD);
    vPosition = pLeader->m_vPosition + qRotation * vOffset;
    float fFacingX = vPosition.m_fX + pLeader->m_vOrientation.m_fX;
    float fFacingY = vPosition.m_fY + pLeader->m_vOrientation.m_fY;
    float fFacingZ = vPosition.m_fZ + pLeader->m_vOrientation.m_fZ;
    AddMoveToPointAction(ACTION_GROUP_NEW, vPosition, pLeader->GetAreaId(),
        OBJECT_INVALID, 1, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0,
        Vector(), 0, 0, 0);
    AddAction(ACTION_CHANGE_FACING_POINT, ACTION_GROUP_LAST,
        ACTION_PARAM_FLOAT, &fFacingX,
        ACTION_PARAM_FLOAT, &fFacingY,
        ACTION_PARAM_FLOAT, &fFacingZ,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
}
// PC 0x00549F80
void CSWSCreature::UnsummonMyself()
{
    CExoString sScript;
    g_pAppManager->GetServerExoApp()->GetSWSMessage();
    ClearAllActions(TRUE);
    CSWSArea* pArea = GetArea();
    if (pArea != NULL)
    {
        CGameEffect* pEffect = new CGameEffect(TRUE);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pArea->ApplyEffect(pEffect, m_vPosition, m_vOrientation);
    }
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
        0, 0, m_nId, m_nId, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
}
// PC 0x0054D7B0
int CSWSCreature::AIActionStanceMeditative(CSWSObjectActionNode* pNode)
{
    int bDeadTemp = GetDeadTemp();
    if (GetDead() || bDeadTemp)
    {
        return CSWSObject::ACTION_FAILED;
    }
    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);
    if (m_c110C != 1)
    {
        m_c110C = 1;
        RecomputeAmbientAnimationState();
        return CSWSObject::ACTION_IN_PROGRESS;
    }
    if (m_lstActions.Count() > 1)
    {
        m_c110C = -1;
        RecomputeAmbientAnimationState();
        return CSWSObject::ACTION_COMPLETE;
    }
    return CSWSObject::ACTION_IN_PROGRESS;
}
// PC 0x0054D850
int CSWSCreature::CheckItemClassRestrictions(CSWSItem* pItem)
{
    if (!pItem->GetPropertyByTypeExists(
            CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_CLASS, 0))
    {
        return 1;
    }
    if (m_pStats->HasFeat(CSWFeat::FEAT_PROFICIENCY_ALL))
    {
        return 1;
    }

    for (int i = 0; i < pItem->m_nPassiveProperties; i++)
    {
        CSWItemProperty* pProperty = pItem->GetPassiveProperty(i);
        if (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
            (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType)))
        {
            if (pItem->GetPassiveProperty(i)->m_nPropertyType ==
                CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_CLASS)
            {
                unsigned short nClassRestriction =
                    pItem->GetPassiveProperty(i)->m_nSubtype;
                EXOLOG("Checking for property %d, subtype %d\n",
                       CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_CLASS, nClassRestriction);
                for (int nClass = 0; nClass < m_pStats->m_nClassCount; nClass++)
                {
                    if (nClassRestriction == m_pStats->GetClass(nClass))
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x00551150
void CSWSCreature::AddToVisibleList(unsigned long oidObject, int bSeen, int bHeard,
                                  unsigned char nSanctuary, int bInvisible)
{
    if (!CheckVisibleList(oidObject))
    {
        CSWVisibilityNode* pNode = new CSWVisibilityNode;
        pNode->m_oidObject = oidObject;
        pNode->m_bSeen = bSeen;
        pNode->m_bHeard = bHeard;
        pNode->m_nSanctuary = nSanctuary;
        pNode->m_bInvisible = bInvisible;
        m_lstVisibilityList.Add(pNode);
    }
}
// PC 0x00551200
int CSWSCreature::AIActionSurrenderToEnemies(CSWSObjectActionNode*)
{
    if (m_nAreaWaitState == 1)
    {
        return CSWSObject::ACTION_FAILED;
    }

    SurrenderToEnemies(0);
    return CSWSObject::ACTION_COMPLETE;
}
// PC 0x00551220
int CSWSCreature::CanUseItem(CSWSItem* pItem, int bUnknown,
                             int bCheckFullHealth)
{
    int bCanUse = TRUE;
    if (pItem == NULL)
    {
        return FALSE;
    }
    if (!CheckItemAlignmentRestrictions(pItem))
    {
        bCanUse = FALSE;
    }
    else if (!CheckItemClassRestrictions(pItem))
    {
        bCanUse = FALSE;
    }
    else if (!CheckItemRaceRestrictions(pItem, bCheckFullHealth))
    {
        bCanUse = FALSE;
    }
    else
    {
        int nItemType = pItem->GetBaseItem()->m_nItemType;
        if (bCheckFullHealth &&
            (nItemType == CSWBaseItem::ITEM_TYPE_REPAIR_KIT ||
             nItemType == CSWBaseItem::ITEM_TYPE_MEDPAC) &&
            GetCurrentHitPoints(0) == GetMaxHitPoints(1))
        {
            bCanUse = FALSE;
        }
        else if (!CheckItemFeatRestrictions(pItem))
        {
            bCanUse = FALSE;
        }
        else if (!CheckItemGenderRestrictions(pItem))
        {
            bCanUse = FALSE;
        }
        else if (!CheckItemPCRestrictions(pItem))
        {
            bCanUse = FALSE;
        }
        else if (!CheckItemAttributeRestrictions(pItem))
        {
            bCanUse = FALSE;
        }
        else if (!CheckItemCNPCRestrictions(pItem))
        {
            bCanUse = FALSE;
        }
    }
    if (m_nAreaWaitState == 1)
    {
        if (m_pStats->GetLevel(0) < pItem->GetMinEquipLevel() &&
            g_pAppManager->GetServerExoApp()->GetServerInfo()->m_bItemLevelRestrictions)
        {
            bCanUse = FALSE;
        }
    }
    if (bCanUse == TRUE)
    {
        CSWBaseItem* pBaseItem = g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType());
        if (pBaseItem != NULL &&
            pBaseItem->m_nWeaponWield != CSWBaseItem::WIELD_SIZE_UNRESTRICTED &&
            pBaseItem->m_nWeaponType != 0)
        {
            int nCreatureSize = m_nCreatureSize;
            if (g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType())->GetWeaponSize() -
                    nCreatureSize > 1)
            {
                bCanUse = FALSE;
            }
        }
        int nSlots = pBaseItem->GetEquipableSlots();
        for (int i = 0; i < CSWInventory::TOTAL_SLOT_COUNT; i++)
        {
            if (nSlots & (1 << i))
            {
                if (!CheckProficiencies(pItem, 1 << i))
                {
                    bCanUse = FALSE;
                }
                break;
            }
        }
    }
    return bCanUse;
}
// PC 0x00549450
int CSWSCreature::CheckItemRaceRestrictions(CSWSItem* pItem, int bEnforceRace)
{
    int bCheckDroidOrHuman = 1;
    if (pItem->GetPropertyByTypeExists(
            CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
    {
        CSWItemProperty* pProperty = NULL;
        pItem->GetPropertyByType(&pProperty,
                                 CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM,
                                 0x81);
        if (pProperty != NULL)
        {
            bCheckDroidOrHuman = 0;
        }
    }

    unsigned char nDroidOrHuman = pItem->GetBaseItem()->m_nDroidOrHuman;
    if (nDroidOrHuman != 0 && bCheckDroidOrHuman)
    {
        switch (nDroidOrHuman)
        {
        case 1:
            if (m_pStats->m_nRace != CSWRace::CRACE_HUMAN)
            {
                return bEnforceRace == 0;
            }
            break;
        case 2:
            if (m_pStats->m_nRace != CSWRace::CRACE_DROID)
            {
                return bEnforceRace == 0;
            }
            break;
        default:
            return 0;
        }
    }

    unsigned char nSubrace = m_pStats->m_nSubraceIndex;
    if (pItem->GetBaseItem()->m_nDenySubrace & (1 << nSubrace))
    {
        return 0;
    }

    if (!pItem->GetPropertyByTypeExists(
            CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_RACIAL_TYPE, 0) &&
        !pItem->GetPropertyByTypeExists(
            CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_SUBRACE, 0))
    {
        return 1;
    }

    for (int i = 0; i < pItem->m_nPassiveProperties; i++)
    {
        CSWItemProperty* pProperty = pItem->GetPassiveProperty(i);
        if (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
            (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType)))
        {
            if (pItem->GetPassiveProperty(i)->m_nPropertyType ==
                CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_RACIAL_TYPE)
            {
                unsigned short nRaceRestriction =
                    pItem->GetPassiveProperty(i)->m_nSubtype;
                EXOLOG("Checking for property %d, subtype %d\n",
                       CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_RACIAL_TYPE, nRaceRestriction);
                unsigned short nRace = m_pStats->m_nRace;
                if (nRace == pItem->GetPassiveProperty(i)->m_nSubtype)
                {
                    return 1;
                }
            }
        }
        pProperty = pItem->GetPassiveProperty(i);
        if (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
            (pItem->m_nUpgrades & (1 << pProperty->m_nUpgradeType)))
        {
            if (pItem->GetPassiveProperty(i)->m_nPropertyType ==
                CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_SUBRACE)
            {
                unsigned short nSubraceRestriction =
                    pItem->GetPassiveProperty(i)->m_nSubtype;
                EXOLOG("Checking for property %d, subtype %d\n",
                       CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_SUBRACE, nSubraceRestriction);
                if (m_pStats->m_nRace != CSWRace::CRACE_DROID)
                {
                    unsigned char nSubrace = m_pStats->m_nSubraceIndex;
                    if (nSubrace == pItem->GetPassiveProperty(i)->m_nSubtype)
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x0054D940
int CSWSCreature::GetIsInInvitationsOffered(unsigned long oidCreature)
{
    int i;
    CServerExoApp* pServerApp;
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nElapsedDays;
    unsigned long nElapsedTime;

    if (m_pInvitationsOffered != NULL)
    {
        for (i = m_pInvitationsOffered->num - 1; i >= 0; i--)
        {
            if ((*m_pInvitationsOffered)[i].m_oidCreature == oidCreature)
            {
                pServerApp = g_pAppManager->m_pServerExoApp;
                pServerApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
                pServerApp = g_pAppManager->m_pServerExoApp;
                pServerApp->GetWorldTimer()->SubtractWorldTimes(
                    nDay, nTime, (*m_pInvitationsOffered)[i].m_nDay,
                    (*m_pInvitationsOffered)[i].m_nTime, &nElapsedDays, &nElapsedTime);
                if (nElapsedTime > INVITATION_OFFER_TIMEOUT)
                {
                    m_pInvitationsOffered->DelIndex(i);
                }
                else
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
// PC 0x0054DA20
void CSWSCreature::RemoveFromInvitationsOffered(unsigned long oidCreature)
{
    int i;

    if (m_pInvitationsOffered != NULL)
    {
        for (i = m_pInvitationsOffered->num - 1; i >= 0; i--)
        {
            if ((*m_pInvitationsOffered)[i].m_oidCreature == oidCreature)
            {
                m_pInvitationsOffered->DelIndex(i);
            }
        }
    }
}
// PC 0x0054DA70
int CSWSCreature::GetIsInInvitationsIgnored(unsigned long oidCreature)
{
    int i;
    CServerExoApp* pServerApp;
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nElapsedDays;
    unsigned long nElapsedTime;

    if (m_pInvitationsIgnored != NULL)
    {
        for (i = m_pInvitationsIgnored->num - 1; i >= 0; i--)
        {
            if ((*m_pInvitationsIgnored)[i].m_oidCreature == oidCreature)
            {
                pServerApp = g_pAppManager->m_pServerExoApp;
                pServerApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
                pServerApp = g_pAppManager->m_pServerExoApp;
                pServerApp->GetWorldTimer()->SubtractWorldTimes(
                    nDay, nTime, (*m_pInvitationsIgnored)[i].m_nDay,
                    (*m_pInvitationsIgnored)[i].m_nTime, &nElapsedDays, &nElapsedTime);
                if (nElapsedTime > INVITATION_IGNORE_TIMEOUT)
                {
                    m_pInvitationsIgnored->DelIndex(i);
                }
                else
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
// PC 0x0054DB50
void CSWSCreature::CleanInvitationLists()
{
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nElapsedDays;
    unsigned long nElapsedTime;

    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    if (m_pInvitationsOffered != NULL)
    {
        int nInvited = m_pInvitationsOffered->num;
        for (int i = nInvited - 1; i >= 0; i--)
        {
            g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
                nDay, nTime, (*m_pInvitationsOffered)[i].m_nDay,
                (*m_pInvitationsOffered)[i].m_nTime, &nElapsedDays, &nElapsedTime);
            if (nElapsedTime > INVITATION_OFFER_TIMEOUT)
            {
                CSWSCreature* pInvitee = g_pAppManager->GetServerExoApp()
                    ->GetCreatureByGameObjectID((*m_pInvitationsOffered)[i].m_oidCreature);
                if (pInvitee != NULL)
                {
                    pInvitee->SetInvitedToParty(FALSE);
                }
                m_pInvitationsOffered->DelIndex(i);
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(CSWCCMessageData::INVITATION_INVITER, GetId());
                pInvitee->SendFeedbackMessage(FEEDBACK_INVITATION_EXPIRED, pData);
            }
        }
    }
    if (m_pInvitationsIgnored != NULL)
    {
        for (int i = m_pInvitationsIgnored->num - 1; i >= 0; i--)
        {
            g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
                nDay, nTime, (*m_pInvitationsIgnored)[i].m_nDay,
                (*m_pInvitationsIgnored)[i].m_nTime, &nElapsedDays, &nElapsedTime);
            if (nElapsedTime > INVITATION_IGNORE_TIMEOUT)
            {
                m_pInvitationsIgnored->DelIndex(i);
            }
        }
    }
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nInvitedToPartyDay, m_nInvitedToPartyTime,
        &nElapsedDays, &nElapsedTime);
    if (nElapsedTime > INVITATION_OFFER_TIMEOUT)
    {
        SetInvitedToParty(FALSE);
    }
}
// PC 0x0054A0D0
CAvoidCreature::CAvoidCreature()
{
    m_nUnused = 0;
    Reset();
}
// PC 0x0054A110
int CSWSCreature::AIActionCastSpell(CSWSObjectActionNode* pAction)
{
    Vector vTarget;
    CExoString sSpell;
    if (GetInParty())
    {
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        int nIndex = pClient->GetSWParty()->GetIndex(
            pClient->ServerToClientObjectId(GetId()));
        if (nIndex >= 0)
#ifdef _XBOX
        {
            CSWParty* pMemberParty = pClient->GetSWParty();

            pMemberParty->GetMemberData(nIndex)->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_ENGAGED);
        }
#else
            pClient->GetSWParty()->GetMemberData(nIndex)->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_ENGAGED);
#endif
    }
    m_nAttackMovementTimer = 3000;
    if (GetDead() == TRUE || GetIsPCDying() == TRUE || GetDeadTemp() == TRUE)
    {
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }

    unsigned long nSpellId = (unsigned long)pAction->m_apParam[0];
    int nMultiClass = (int)pAction->m_apParam[1];
    int nSpellLevel = (int)pAction->m_apParam[2];
    int nMetamagic = (int)pAction->m_apParam[3];
    int bOverride = (int)pAction->m_apParam[4];
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[5];
    int nX = (int)pAction->m_apParam[6];
    int nY = (int)pAction->m_apParam[7];
    int nZ = (int)pAction->m_apParam[8];
    vTarget = Vector(*(float*)&nX, *(float*)&nY, *(float*)&nZ);
    unsigned long nProjectilePath = (unsigned long)pAction->m_apParam[9];
    int nFeat = (int)pAction->m_apParam[10];
    int nSpellLikeClass = (int)pAction->m_apParam[11];
    int bFake = (nProjectilePath & SWSOBJECT_CAST_SPELL_FAKE_FLAG) != 0;
    int bInstant = (nProjectilePath & SWSOBJECT_CAST_SPELL_INSTANT_FLAG) != 0;
    unsigned char nProjectileType = (unsigned char)nProjectilePath ?
        SPELL_PROJECTILE_TYPE_CUSTOM_PATH : SPELL_PROJECTILE_TYPE_NORMAL;
    if (oidTarget != OBJECT_INVALID)
        SetLockOrientationToObject(oidTarget, 0);
    else
    {
        Vector vDirection = vTarget - m_vPosition;
        if (!vDirection.IsZero())
        {
            vDirection.Normalize();
            SetOrientation(vDirection);
        }
    }
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
    if (pSpell == NULL)
    {
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }
    int nForceItemMask = GetForceItemMask();
    if ((nForceItemMask & pSpell->m_nForbidItemMask) ||
        (nForceItemMask & pSpell->m_nRequireItemMask) != pSpell->m_nRequireItemMask ||
        (nFeat != -1 && m_pStats->GetFeatRemainingUses(nFeat) <= 0))
    {
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }
    if (oidTarget == OBJECT_INVALID)
        m_oidSpellTarget = GetAreaId();
    else
        m_oidSpellTarget = oidTarget;
    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if ((oidTarget != OBJECT_INVALID && pTarget == NULL) ||
        (pTarget != NULL && pTarget->AsSWSCreature() != NULL &&
         (pTarget->AsSWSCreature()->GetDead() || pTarget->AsSWSCreature()->GetDeadTemp())))
    {
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_FAILED;
    }
    if (!bFake)
        ActivityManager(ACTIVITY_ACTION_CLEAR_MODES);
    if (bInstant == TRUE)
    {
        m_pcCombatRound->StartCombatRound(oidTarget, 0, 0, 0, 0);
        m_pcCombatRound->SetRoundPaused(1, OBJECT_INVALID);
        m_pcCombatRound->SetPauseTimer(1500, 0);
        SetAnimation(10009);
        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack)->m_nAttackType = 28;
        ResolveAttack(oidTarget, 1, 1500, 0, 0, 4, 0);
        m_bSafeProjectileBroadcast = 0;
        SpellCastAndImpact(nSpellId, Vector(), oidTarget, nMultiClass,
            OBJECT_INVALID, 0, 0, 0, 0);
        m_bSafeProjectileBroadcast = 0;
        return ACTION_COMPLETE;
    }
    int nConjureTime = pSpell->m_nConjureTime;
    int nCastTime = pSpell->m_nCastTime;
    int nCatchTime = pSpell->m_nCatchTime;
    if (!m_pcCombatRound->m_bRoundStarted)
    {
        int bEngaged = m_pcCombatRound->ResolveEngagedState(oidTarget);
        int bMaster = GetCombatRound()->ResolveMasterState(oidTarget, bEngaged);
        GetCombatRound()->StartCombatRound(oidTarget, bEngaged, bMaster, 0, 0);
        GetCombatRound()->SetSpellCastRound(nSpellId, bFake);
        GetCombatRound()->SetRoundPaused(1, GetId());
        GetCombatRound()->SetPauseTimer(nCastTime + nCatchTime + nConjureTime, 0);
        int nRoundTime = nCastTime + nCatchTime + nConjureTime;
        GetCombatRound()->DecrementRoundLength(nRoundTime, 0);
        if (bEngaged && pTarget != NULL && pTarget->AsSWSCreature() != NULL &&
            m_nId != oidTarget && !bFake)
        {
            CSWSCreature* pCreature = pTarget->AsSWSCreature();
            pCreature->GetCombatRound()->StartCombatRound(GetId(), bEngaged, !bMaster, 0, 0);
            pCreature->m_pcCombatRound->SetRoundPaused(1, pCreature->GetId());
            pCreature->m_pcCombatRound->SetPauseTimer(nRoundTime, 0);
            pCreature->m_pcCombatRound->DecrementRoundLength(nRoundTime, 0);
        }
        if (!bFake)
            BroadcastSpellCast(nSpellId, nMetamagic, nMultiClass, nFeat, oidTarget);
    }
    else if (!m_pcCombatRound->m_bRoundPaused && m_pcCombatRound->m_nNumActionsTaken < 1)
    {
        m_pcCombatRound->SetSpellCastRound(nSpellId, bFake);
        m_pcCombatRound->SetRoundPaused(1, GetId());
        GetCombatRound()->SetPauseTimer(nCastTime + nCatchTime + nConjureTime, 0);
        int nRoundTime = nCastTime + nCatchTime + nConjureTime;
        GetCombatRound()->DecrementRoundLength(nRoundTime, 0);
        if (GetCombatRound()->m_bEngaged && pTarget != NULL && pTarget->AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = pTarget->AsSWSCreature();
            unsigned long nId = pCreature->m_nId;
            CSWSCombatRound* pcCombatRound = pCreature->m_pcCombatRound;
            pcCombatRound->SetRoundPaused(1, nId);
            pCreature->m_pcCombatRound->SetPauseTimer(nRoundTime, 0);
            pCreature->m_pcCombatRound->DecrementRoundLength(nRoundTime, 0);
        }
    }
    unsigned long nDay, nTime, nElapsedDays, nElapsedTime;
    unsigned long nId = m_nId;
    g_pAppManager->GetServerExoApp()->GetActiveTimer(nId)->GetWorldTime(&nDay, &nTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nActionStartDay, m_nActionStartTime, &nElapsedDays, &nElapsedTime);
    if (nElapsedTime == 0)
    {
        m_nLastSpellId = nSpellId;
        m_c1C4 = nSpellLevel;
        m_oidSpellTargetObject = oidTarget;
        m_vSpellTargetPosition = vTarget;
        m_bSafeProjectileBroadcast = 0;
    }
    if (nElapsedTime < nConjureTime)
    {
        short nConjureAnim = pSpell->m_nConjureAnim;
        if (nConjureAnim == 3)
            SetAnimation(CREATURE_ANIMATION_CONJURE_3);
        else if (nConjureAnim == 2)
            SetAnimation(CREATURE_ANIMATION_CONJURE_2);
        else if (nConjureAnim == 1)
            SetAnimation(CREATURE_ANIMATION_CONJURE_1);
        else if (nConjureAnim == 7)
            SetAnimation(10162);
        else
            SetAnimation(CREATURE_ANIMATION_CONJURE_1);
        return ACTION_IN_PROGRESS;
    }
    if (pAction->m_nField70 == 1)
    {
        int bEngaged = GetCombatRound()->m_bEngaged;
        pAction->m_nField70 = bEngaged != 0;
        if (!bFake)
        {
            if (GetArea() == NULL)
            {
                RemoveSpellActionFromRound();
                ProcessPendingCombatActions();
                return ACTION_FAILED;
            }
            if (nMultiClass != 255 && !m_bIgnoreSpellReadyCount)
            {
                if (!bOverride)
                {
                    if (!DecrementSpellReadyCount(nSpellId, nMultiClass, nSpellLevel,
                            nMetamagic, nSpellLikeClass))
                    {
                        RemoveSpellActionFromRound();
                        ProcessPendingCombatActions();
                        return ACTION_FAILED;
                    }
                }
                else
                {
                    if (nSpellLevel == 0)
                        pSpell->GetSpellLevel(m_pStats->GetClass(nMultiClass));
                    g_pRules->GetMetaMagicLevelCost(nMetamagic);
                }
                m_bIgnoreSpellReadyCount = 1;
            }
            for (int i = 0; i < m_lstEffects.num; ++i)
            {
                if (m_lstEffects.element[i]->m_nType == CSWSEffectListHandler::EFFECT_ENTANGLE)
                {
                    SetAnimation(CREATURE_ANIMATION_COMBAT_READY);
                    SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID, 0);
                    m_bIgnoreSpellReadyCount = 0;
                    if (!bInstant)
                        SendFeedbackMessage(FEEDBACK_UNKNOWN_065, NULL);
                    RemoveSpellActionFromRound();
                    ProcessPendingCombatActions();
                    return ACTION_FAILED;
                }
            }
        }
        m_c1D4 = nMultiClass;
        m_oidSpellCasterItem = CGameObjectArray::INVALIDOBJID;
        m_n1D0 = bOverride;
        m_c1D5 = nMetamagic;
        CalculateLastSpellProjectileTime(nProjectilePath);
    }
    if (nElapsedTime >= nConjureTime + nCastTime && m_bSafeProjectileBroadcast)
    {
        if (nElapsedTime < nConjureTime + nCastTime + nCatchTime)
        {
            if (pSpell->m_nCatchAnim == 0)
                SetAnimation(10161);
            m_bIgnoreSpellReadyCount = 0;
            ProcessPendingCombatActions();
            RemoveSpellActionFromRound();
            return ACTION_FAILED;
        }
        SetAnimation(GetPauseReadyAnimation());
        SetLockOrientationToObject(CGameObjectArray::INVALIDOBJID, 0);
        m_bIgnoreSpellReadyCount = 0;
        if (nFeat != -1)
            m_pStats->DecrementFeatRemainingUses(nFeat);
        RemoveSpellActionFromRound();
        ProcessPendingCombatActions();
        return ACTION_COMPLETE;
    }
    else
    {
        short nCastAnim = pSpell->m_nCastAnim;
        if (nCastAnim == 2)
            SetAnimation(CREATURE_ANIMATION_SPELLCAST_2);
        else if (nCastAnim == 1)
            SetAnimation(CREATURE_ANIMATION_SPELLCAST_1);
        else if (nCastAnim == 3)
            SetAnimation(CREATURE_ANIMATION_SPELLCAST_3);
        else if (nCastAnim == 4)
            SetAnimation(CREATURE_ANIMATION_SPELLCAST_4);
        else if (nCastAnim == 7)
            SetAnimation(CREATURE_ANIMATION_SPELLCAST_5);
        else if (nCastAnim != 8 && nCastAnim != 9)
            SetAnimation(CREATURE_ANIMATION_SPELLCAST_5);
        if (!m_bSafeProjectileBroadcast)
        {
            if (bFake)
            {
                CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
                CSWSObject* pServerObject;
                if (pObject != NULL && (pServerObject = pObject->AsSWSObject()) != NULL)
                    BroadcastSafeProjectile(GetId(), oidTarget, m_vPosition, pServerObject->m_vPosition,
                        m_nLastSpellProjectileTime, nProjectileType, nSpellId, 0, nProjectilePath, 0, 0);
                else
                    BroadcastSafeProjectile(GetId(), oidTarget, m_vPosition, m_vSpellTargetPosition,
                        m_nLastSpellProjectileTime, nProjectileType, nSpellId, 0, nProjectilePath, 0, 0);
                m_bSafeProjectileBroadcast = 1;
            }
            else
            {
                m_pcCombatRound->IncrementNumActionsTaken(1);
                SpellCastAndImpact(nSpellId, vTarget, oidTarget, nMultiClass,
                    OBJECT_INVALID, 0, 0, nProjectilePath, bInstant);
            }
        }
        return ACTION_IN_PROGRESS;
    }
}
// PC 0x0054DDB0
int CSWSCreature::GetNumInvited()
{
    NwscreatureaiTrace1();
    if (m_pInvitationsOffered != NULL)
    {
        return m_pInvitationsOffered->num;
    }
    return 0;
}
// PC 0x0054DDD0
int CSWSCreature::HasAction(unsigned long nAction)
{
    if (m_lstActions.GetHeadPos() != NULL)
    {
        CExoLinkedListNode* pPos = m_lstActions.GetHeadPos();

        while (pPos != NULL)
        {
            CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPos);

            if (pAction->m_nActionType == nAction)
            {
                return TRUE;
            }
            m_lstActions.GetNext(pPos);
        }
    }
    return FALSE;
}
// PC 0x0054DE30
CSWSObjectActionNode* CSWSCreature::GetAction(unsigned long nAction)
{
    if (m_lstActions.GetHeadPos() != NULL)
    {
        CExoLinkedListNode* pPos = m_lstActions.GetHeadPos();

        while (pPos != NULL)
        {
            CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPos);

            if (pAction->m_nActionType == nAction)
            {
                return pAction;
            }
            m_lstActions.GetNext(pPos);
        }
    }
    return NULL;
}
// PC 0x0054DE80
void CSWSCreature::SurrenderToEnemies(int bKeepOwnEffects)
{
    CancelCombat(0);
    ClearAllEffects(bKeepOwnEffects);

    Vector vPosition = m_vPosition;
    float fDistance = CSWSArea::MAX_OBJECT_VISIBLE_DISTANCE;
    float fMaxX = vPosition.m_fX + fDistance;
    CSWSArea* pArea = GetArea();
    int nIndex;
    if (pArea != NULL &&
        pArea->GetFirstObjectIndiceByX(&nIndex, vPosition.m_fX - fDistance) == 1)
    {
        int nObjectCount = pArea->m_lstObjectIds.num;
        while (nIndex < nObjectCount)
        {
            CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->
                GetCreatureByGameObjectID(pArea->m_lstObjectIds[nIndex]);
            if (pCreature != NULL && m_nId != pCreature->m_nId)
            {
                if (pCreature->m_vPosition.m_fX > fMaxX)
                    break;
                Vector vDifference = pCreature->m_vPosition - vPosition;
                if (vDifference * vDifference <= fDistance * fDistance &&
                    GetReputation(pCreature->m_nId, 0) <= MAX_HOSTILE_REPUTATION)
                {
                    pCreature->CancelCombat(0);
                    pCreature->ClearAllEffects(bKeepOwnEffects);
                }
            }
            nObjectCount = pArea->m_lstObjectIds.num;
            ++nIndex;
        }
    }

    if (g_pAppManager->GetServerExoApp()->GetFactionManager() != NULL &&
        g_pAppManager->GetServerExoApp()->GetFactionManager()->m_pNeutral != NULL)
    {
        g_pAppManager->GetServerExoApp()->GetFactionManager()->m_pNeutral->
            AddMember(GetId(), 0);
    }
}
// PC 0x0054E020
int CSWSCreature::GetUserActionsPending()
{
    CExoLinkedListNode* pPos = m_pcCombatRound->m_pScheduledActions->GetHeadPos();
    if (pPos == NULL)
    {
        return FALSE;
    }

    CSWSCombatRoundAction* pAction =
        m_pcCombatRound->m_pScheduledActions->GetAtPos(pPos);
    while (pAction != NULL)
    {
        CGameObject* pTarget = NULL;
        if (pAction->m_oidTarget != OBJECT_INVALID)
        {
            pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(pAction->m_oidTarget);
            if (pTarget != NULL && pTarget->AsSWSCreature() != NULL &&
                pTarget->AsSWSCreature()->GetDead())
            {
                pTarget = NULL;
            }
        }

        CGameObject* pSpellTarget = NULL;
        if (pAction->m_oidSpellTarget != OBJECT_INVALID)
        {
            pSpellTarget = g_pAppManager->GetServerExoApp()->GetGameObject(pAction->m_oidSpellTarget);
            if (pSpellTarget != NULL && pSpellTarget->AsSWSCreature() != NULL &&
                pSpellTarget->AsSWSCreature()->GetDead())
            {

                pTarget = NULL;
            }
        }

        if (pAction->m_nAttackFlags != 0 && pTarget != NULL && pSpellTarget != NULL)
        {
            return TRUE;
        }
        pAction = m_pcCombatRound->m_pScheduledActions->GetNext(pPos);
    }
    return FALSE;
}
// PC 0x0054E130
int CSWSCreature::AIActionRecoverMine(CSWSObjectActionNode* pAction)
{
    int nDC = 0;
    Vector vPosition;
    float fRange;
    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget == NULL)
    {
        StopGuiTimingBar();
        m_bMineActionStarted = FALSE;
        return ACTION_FAILED;
    }
    if (!GetIsInUseRange(oidTarget, NWSCREATURE_MINE_USE_RANGE_MARGIN, FALSE))
    {
        GetUseRange(oidTarget, vPosition, fRange, FALSE);
        fRange += NWSCREATURE_MINE_USE_RANGE_MARGIN;
        AddActionToFront(ACTION_RECOVER_MINE, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        Vector vOrientation;
        AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition,
            m_oidArea, OBJECT_INVALID, TRUE, fRange, 0.0f, 0, 0, 0, 0, 0, 0,
            vOrientation, 0, 0, 0);
        return ACTION_COMPLETE;
    }
    if (!m_bMineActionStarted)
    {
        m_bMineActionStarted = TRUE;
        int nAnimation;
        if (pTarget->m_nObjectType == OBJECT_TYPE_DOOR ||
            pTarget->m_nObjectType == OBJECT_TYPE_PLACEABLE)
            nAnimation = CREATURE_ANIMATION_LOOPING_GET_MID;
        else
            nAnimation = CREATURE_ANIMATION_MINE_DISARM_4;
        int bLooping = TRUE;
        float fSpeed = 1.0f;
        float fDuration = NWSCREATURE_MINE_ACTION_DURATION;
        AddActionToFront(ACTION_RECOVER_MINE, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_PLAY_ANIMATION, pAction->m_nGroupId,
            ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
            ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLooping,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
            ACTION_PARAM_OBJECT, &oidTarget, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
        StartGuiTimingBar(MINE_RECOVERY_TIME, MINE_RECOVERY_GUI_TYPE);
        return ACTION_COMPLETE;
    }

    int nRank = GetStats()->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, FALSE);
    int nRoll;
    int bSuccess = FALSE;
    int bAutoRoll = FALSE;
    int bOwnTrap = FALSE;
    if (m_bCombatState)
        nRoll = rand() % SKILL_CHECK_DIE_SIDES + 1;
    else
    {
        nRoll = SKILL_CHECK_DIE_SIDES;
        bAutoRoll = TRUE;
    }
    CSWSItem* pItem;
    unsigned char nTrapType;
    if (pTarget->m_nObjectType == OBJECT_TYPE_TRIGGER)
    {
        unsigned long oidCreator = pTarget->AsSWSTrigger()->GetCreator();
        CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreator);
        if (pTarget->AsSWSTrigger()->GetCreator() == m_nId ||
            ((g_pAppManager->m_pServerExoApp->GetPartyTable()->GetIsMember(
                pTarget->AsSWSTrigger()->GetCreator()) ||
              (pCreator != NULL && pCreator->GetStats()->GetIsPC())) &&
             (g_pAppManager->m_pServerExoApp->GetPartyTable()->GetIsMember(GetId()) ||
              GetStats()->GetIsPC())))
        {
            bSuccess = TRUE;
            bOwnTrap = TRUE;
            nTrapType = pTarget->AsSWSTrigger()->m_nTrapType;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
            pTarget->AsSWSTrigger()->SetFadeOutTime(MINE_RECOVERY_FADE_TIME);
        }
        else
        {
            nDC = pTarget->AsSWSTrigger()->GetDisarmDC() + MINE_RECOVERY_DC_INCREASE < 1
                ? 1 : pTarget->AsSWSTrigger()->GetDisarmDC() + MINE_RECOVERY_DC_INCREASE;
            if (nRoll + nRank >= nDC)
            {
                bSuccess = TRUE;
                nTrapType = pTarget->AsSWSTrigger()->m_nTrapType;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                pTarget->AsSWSTrigger()->SetFadeOutTime(MINE_RECOVERY_FADE_TIME);
            }
        }
    }
    else if (pTarget->m_nObjectType == OBJECT_TYPE_DOOR)
    {
        nDC = pTarget->AsSWSDoor()->m_nDisarmDC + MINE_RECOVERY_DC_INCREASE < 1
            ? 1 : pTarget->AsSWSDoor()->m_nDisarmDC + MINE_RECOVERY_DC_INCREASE;
        if (nRoll + nRank >= nDC)
        {
            bSuccess = TRUE;
            nTrapType = pTarget->AsSWSDoor()->m_nTrapType;
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        }
    }
    else if (pTarget->m_nObjectType == OBJECT_TYPE_PLACEABLE)
    {
        nDC = pTarget->AsSWSPlaceable()->m_nDisarmDC + MINE_RECOVERY_DC_INCREASE < 1
            ? 1 : pTarget->AsSWSPlaceable()->m_nDisarmDC + MINE_RECOVERY_DC_INCREASE;
        if (nRoll + nRank >= nDC)
        {
            bSuccess = TRUE;
            nTrapType = pTarget->AsSWSPlaceable()->m_nTrapType;
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        }
    }
    if (bSuccess)
    {
        CExoString sResRef;
        pItem = new CSWSItem(OBJECT_INVALID);
        if (g_pRules->Get2DArrays()->GetTraps()->GetCExoStringEntry(nTrapType, "ResRef", &sResRef))
        {
            CResRef cResRef(sResRef.CStr());
            pItem->LoadFromTemplate(cResRef);
            if (GetItemRepository(TRUE)->AddItem(&pItem, TRUE, FALSE, TRUE) && pItem != NULL)
                pItem->SetPossessor(GetId(), TRUE, TRUE, OBJECT_INVALID);
            CExoArrayList<unsigned long>* pTraps = GetArea() != NULL ? &GetArea()->m_lstTrapObjectIds : NULL;
            if (pTraps != NULL)
            {
                CSWSArea* pArea = GetArea();
                if (pArea->m_lstTrapObjectIds.Contains(oidTarget) == TRUE)
                {
                    CSWSArea* pTrapArea = GetArea();
                    pTrapArea->m_lstTrapObjectIds.Remove(oidTarget);
                }
            }
        }
    }
    else if (nRoll + nRank < nDC - MINE_TRIGGER_FAILURE_MARGIN)
    {
        if (pTarget->AsSWSTrigger() != NULL)
        {
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
            pEvent->SetObjectID(0, GetId());
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        }
        else if (pTarget->AsSWSPlaceable() != NULL || pTarget->AsSWSDoor() != NULL)
        {
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        }
    }
    CSWCCMessageData* pData = new CSWCCMessageData;
    pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, GetId());
    pData->SetObjectID(CSWCCMessageData::SKILL_DATA_TARGET, oidTarget);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, CSWCCMessageData::SKILL_DATA_RECOVER_STRREF);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, nRoll);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, nRank);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, nDC);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, bAutoRoll);
    if (bOwnTrap == TRUE)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_OWN_MINE);
    else if (bSuccess == TRUE)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_SUCCESS);
    else if (bAutoRoll && nRoll + nRank < nDC)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_RETRY);
    else if (nRoll + nRank < nDC - MINE_RECOVERY_FEEDBACK_FAILURE_MARGIN)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_CRITICAL_FAILURE);
    else if (nRoll + nRank < nDC)
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_FAILED);
    pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_DEMOLITIONS_STRREF);
    BroadcastSkillData(pData);
    StopGuiTimingBar();
    m_bMineActionStarted = FALSE;
    return ACTION_COMPLETE;
}
// PC 0x0054EB00
int CSWSCreature::AIActionDisarmMine(CSWSObjectActionNode* pAction)
{
    int nRoll;
    Vector vPosition;
    float fRange;
    ActivityManager(ACTIVITY_ACTION_OBJECT_INTERACTION);
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    int nResult;
    if (pObject == NULL)
    {
        StopGuiTimingBar();
        m_bMineActionStarted = FALSE;
        nResult = ACTION_FAILED;
    }
    else
    {
        if (!GetIsInUseRange(oidTarget, 0.25f, FALSE))
        {
            GetUseRange(oidTarget, vPosition, fRange, FALSE);
            fRange += 0.25f;
            AddActionToFront(ACTION_DISARM_MINE, pAction->m_nGroupId,
                             ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
                             ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            AddMoveToPointActionToFront(pAction->m_nGroupId, vPosition, m_oidArea,
                OBJECT_INVALID, TRUE, fRange, 0.0f, FALSE, 0, FALSE, FALSE, FALSE,
                FALSE, Vector(), 0, 0, FALSE);
            return ACTION_COMPLETE;
        }
        if (!m_bMineActionStarted)
        {
            m_bMineActionStarted = TRUE;
            int nAnimation;
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                nAnimation = CREATURE_ANIMATION_MINE_DISARM_2;
            else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                nAnimation = CREATURE_ANIMATION_MINE_DISARM_3;
            else
                nAnimation = CREATURE_ANIMATION_MINE_DISARM_1;
            int bLoop = TRUE;
            float fSpeed = 1.0f;
            float fDuration = 4.5f;
            AddActionToFront(ACTION_DISARM_MINE, pAction->m_nGroupId,
                             ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            AddActionToFront(ACTION_PLAY_ANIMATION, pAction->m_nGroupId,
                ACTION_PARAM_INTEGER, &nAnimation, ACTION_PARAM_FLOAT, &fSpeed,
                ACTION_PARAM_FLOAT, &fDuration, ACTION_PARAM_INTEGER, &bLoop,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL);
            AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
                             ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            StartGuiTimingBar(DISARM_TIMING_DURATION, DISARM_TIMING_TYPE);
            return ACTION_COMPLETE;
        }

        int nDC;
        int nRank = m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, FALSE);
        int bTakeTwenty = FALSE;
        int bSuccess = FALSE;
        int bOwnMine = FALSE;
        if (m_bCombatState)
            nRoll = rand() % SKILL_CHECK_DIE_SIDES + 1;
        else
        {
            nRoll = SKILL_CHECK_DIE_SIDES;
            bTakeTwenty = TRUE;
        }

        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
        {
            unsigned long oidCreator = pObject->AsSWSTrigger()->GetCreator();
            CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreator);
            if (pObject->AsSWSTrigger()->GetCreator() == m_nId ||
                ((g_pAppManager->m_pServerExoApp->GetPartyTable()->GetIsMember(
                    pObject->AsSWSTrigger()->GetCreator()) ||
                  (pCreator != NULL && pCreator->GetStats()->GetIsPC())) &&
                 (g_pAppManager->m_pServerExoApp->GetPartyTable()->GetIsMember(GetId()) ||
                  GetStats()->GetIsPC())))
            {
                bOwnMine = TRUE;
                bSuccess = TRUE;
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                pObject->AsSWSTrigger()->SetFadeOutTime(DISARM_FADE_OUT_TIME);
            }
            else
            {
                if (pObject->AsSWSTrigger()->m_nTrapDisarmDC >= DISARM_MINIMUM_DC)
                    nDC = pObject->AsSWSTrigger()->m_nTrapDisarmDC;
                else
                    nDC = DISARM_MINIMUM_DC;
                if (((pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER &&
                      pObject->m_nObjectType != CGameObject::OBJECT_TYPE_TRIGGER) ||
                     nDC <= DISARM_MAXIMUM_DC) && nRoll + nRank >= nDC)
                {
                    bSuccess = TRUE;
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                    pObject->AsSWSTrigger()->SetFadeOutTime(DISARM_FADE_OUT_TIME);
                }
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            if (pObject->AsSWSDoor()->m_nDisarmDC >= DISARM_MINIMUM_DC)
                nDC = pObject->AsSWSDoor()->m_nDisarmDC;
            else
                nDC = DISARM_MINIMUM_DC;
            if (nDC <= DISARM_MAXIMUM_DC && nRoll + nRank >= nDC)
            {
                bSuccess = TRUE;
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            if (pObject->AsSWSPlaceable()->m_nDisarmDC >= DISARM_MINIMUM_DC)
                nDC = pObject->AsSWSPlaceable()->m_nDisarmDC;
            else
                nDC = DISARM_MINIMUM_DC;
            if (nDC <= DISARM_MAXIMUM_DC && nRoll + nRank >= nDC)
            {
                bSuccess = TRUE;
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }

        if (bSuccess)
        {
            CExoArrayList<unsigned long>* pTraps = GetArea() != NULL ? &GetArea()->m_lstTrapObjectIds : NULL;
            if (pTraps != NULL)
            {
                CSWSArea* pArea = GetArea();
                if (pArea->m_lstTrapObjectIds.Contains(oidTarget) == TRUE)
                {
                    CSWSArea* pTrapArea = GetArea();
                    pTrapArea->m_lstTrapObjectIds.Remove(oidTarget);
                }
            }
        }
        else
        {
            GetClientCreature()->PlaySoundSetSound(DISARM_FAILED_SOUND, TRUE);
            if (nRoll + nRank < nDC - DISARM_CRITICAL_FAILURE_MARGIN)
            {
                if (pObject->AsSWSTrigger() != NULL)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
                    pEvent->SetObjectID(0, GetId());
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
                else if (pObject->AsSWSPlaceable() != NULL || pObject->AsSWSDoor() != NULL)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
            }
        }
        CSWCCMessageData* pData = new CSWCCMessageData;
        pData->SetObjectID(CSWCCMessageData::SKILL_DATA_CREATURE, GetId());
        pData->SetObjectID(CSWCCMessageData::SKILL_DATA_TARGET, oidTarget);
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_NAME, CSWCCMessageData::SKILL_DATA_DISARM_STRREF);
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_ROLL, nRoll);
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_RANK, nRank);
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_DC, nDC);
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_TAKE_20, bTakeTwenty);
        if (bOwnMine == TRUE)
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_OWN_MINE);
        else if (bSuccess == TRUE)
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_SUCCESS);
        else if (bTakeTwenty && nRoll + nRank < nDC)
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_RETRY);
        else if (nRoll + nRank < nDC - DISARM_CRITICAL_FAILURE_MARGIN)
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_CRITICAL_FAILURE);
        else if (nRoll + nRank < nDC)
            pData->SetInteger(CSWCCMessageData::SKILL_DATA_RESULT, CSWCCMessageData::SKILL_RESULT_FAILED);
        pData->SetInteger(CSWCCMessageData::SKILL_DATA_DESCRIPTION, CSWCCMessageData::SKILL_DATA_DEMOLITIONS_STRREF);
        BroadcastSkillData(pData);
        StopGuiTimingBar();
        m_bMineActionStarted = FALSE;
        nResult = ACTION_COMPLETE;
    }
    return nResult;
}
// PC 0x005513E0
void CSWSCreature::AddToInvitationsOffered(unsigned long oidCreature)
{
    unsigned long nDay;
    unsigned long nTime;
    CSWSInvitationDetails invitation;

    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    invitation.m_oidCreature = oidCreature;
    invitation.m_nDay = nDay;
    invitation.m_nTime = nTime;

    if (m_pInvitationsOffered == NULL)
    {
        m_pInvitationsOffered = new CExoArrayList<CSWSInvitationDetails>(0);
    }
    m_pInvitationsOffered->Add(invitation);
}
// PC 0x00552B90
int CSWSCreature::DriveUpdateLocation(int bRun)
{
    if (m_nElapsedTime == 0)
        return ACTION_IN_PROGRESS;
    if (m_pPathfindInformation->m_pWayPoints == NULL)
        return ACTION_FAILED;
    int nResult = ACTION_IN_PROGRESS;
    Vector vPosition;
    float fRate = bRun ? GetRunRate() : GetWalkRate();
    if (m_nAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10078 ||
        m_nAnimation == CREATURE_ANIMATION_FOOTSTEP_MOVE_10079)
        fRate = 1600.0f;
    float fPathTime = m_fClientPathTime;
    fRate *= fPathTime;
    Vector vTarget(m_pPathfindInformation->m_pWayPoints[0],
                   m_pPathfindInformation->m_pWayPoints[1], 0.0f);
    vTarget.m_fZ = GetArea()->ComputeHeight(vTarget, 0, NULL);
    Vector vDelta = vTarget - m_vPosition;
    Vector vDirection = normalize(vDelta);
    float fDistance = m_nElapsedTime * fRate / 1000000.0f;
    if (fDistance * fDistance >= vDelta * vDelta)
    {
        vPosition = vTarget;
        nResult = ACTION_COMPLETE;
    }
    else
        vPosition = m_vPosition + vDirection * fDistance;
    if (!GetArea()->NoCreaturesOnLine(m_vPosition, vPosition,
            GetPathfindInformation(), 1, 0, NULL, 0, NULL))
        return ACTION_FAILED;
    AutoCloseGUIPanels(1);
    UpdateSubareasOnMoveTo(m_vPosition, vPosition, 0, NULL, 1);
    unsigned long oidDoor = GetArea()->GetDoorCrossed(m_vPosition, vPosition);
    if (oidDoor != OBJECT_INVALID)
    {
        CSWSDoor* pDoor = g_pAppManager->GetServerExoApp()->GetDoorByGameObjectID(oidDoor);
        if (pDoor != NULL && pDoor->GetIsLinked() && pDoor->m_nOpenState != CSWSDoor::OPEN_STATE_CLOSED)
        {
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK;
            pEvent->SetObjectID(0, GetId());
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, m_nId, oidDoor, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            return ACTION_FAILED;
        }
    }
    vPosition.m_fZ = GetArea()->ComputeHeight(vPosition, 0, NULL);
    SetPosition(vPosition, 1, 1, 0);
    return nResult;
}
// PC 0x00552ED0
int CSWSCreature::ReplyToInvitation(int nReply, unsigned long oidInviter)
{
    CSWSCreature* pInviter;
    int bReturn;

    bReturn = 0;
    pInviter = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidInviter);
    if (pInviter != NULL && pInviter->GetIsInInvitationsOffered(GetId()) == 1 &&
        m_oidInvitedBy == oidInviter)
    {
        switch (nReply)
        {
        case 0:
            if (pInviter->GetFaction() != NULL)
            {
                pInviter->GetFaction()->AddMember(GetId(), 0);
                bReturn = 1;
            }
            break;
        case 1:
            bReturn = 1;
            break;
        case 2:
            AddToInvitationsIgnored(oidInviter);
            bReturn = 1;
            break;
        }
        if (bReturn)
        {
            pInviter->RemoveFromInvitationsOffered(GetId());
            SetInvitedToParty(0);
            m_oidInvitedBy = OBJECT_INVALID;
        }
    }
    return bReturn;
}
// PC 0x005514B0
void CSWSCreature::AddToInvitationsIgnored(unsigned long oidCreature)
{
    unsigned long nDay;
    unsigned long nTime;
    CSWSInvitationDetails invitation;

    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    invitation.m_oidCreature = oidCreature;
    invitation.m_nDay = nDay;
    invitation.m_nTime = nTime;

    if (m_pInvitationsIgnored == NULL)
    {
        m_pInvitationsIgnored = new CExoArrayList<CSWSInvitationDetails>(0);
    }
    m_pInvitationsIgnored->Add(invitation);
}
// PC 0x00552F80
int CSWSCreature::AIActionJumpToObject(CSWSObjectActionNode* pAction)
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1 ||
        m_pPathfindInformation == NULL)
        return ACTION_FAILED;

    unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
    int bCheckObjects = 1;
    if (pAction->m_nNumParams > 1)
        bCheckObjects = (int)pAction->m_apParam[1];

    CGameObject* pObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pObject == NULL || pObject->AsSWSObject() == NULL)
        return ACTION_FAILED;

    CSWSArea* pArea = pObject->AsSWSObject()->GetArea();
    Vector vPosition;
    Vector vOrientation;
    Vector vSafePosition;
    switch (pObject->m_nObjectType)
    {
    case SW_OBJECT_TYPE_DOOR:
        {
            CSWSDoor* pDoor = pObject->AsSWSDoor();
            CSWSDoor* pTargetDoor = pObject->AsSWSDoor();
            Vector vApproach = pDoor->m_vPosition - pTargetDoor->m_vOrientation;
            vPosition = pDoor->GetNearestActionPoint(vApproach, 1);
            vPosition = vPosition -
                (2.0f * m_pPathfindInformation->GetCreaturePersonalSpace()) *
                pDoor->m_vOrientation;
            vOrientation = normalize(vPosition - pDoor->m_vPosition);
        }
        break;
    case SW_OBJECT_TYPE_WAYPOINT:
        vPosition = pObject->AsSWSObject()->m_vPosition;
        vOrientation = pObject->AsSWSObject()->m_vOrientation;
        break;
    default:
        vPosition = ((CSWSObject*)pObject)->m_vPosition;
        {
            CSWSObject* pTarget = pObject->AsSWSObject();
            vOrientation = normalize(pTarget->m_vPosition - vPosition);
        }
        break;
    }

    if (pArea == NULL)
    {
        if (pObject->AsSWSCreature() == NULL)
            return ACTION_FAILED;
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        unsigned long oidArea = pCreature->m_oidDisappearTarget;
        pArea = g_pAppManager->GetServerExoApp()->GetAreaByGameObjectID(oidArea);
        vPosition = pCreature->m_vDisappearPosition;
        if (pArea == NULL)
            return ACTION_FAILED;
    }

    m_pPathfindInformation->ResetInterTileSearchData();
    m_pPathfindInformation->ResetGridSearchData();
    m_pPathfindInformation->m_oidPathfindCreature = GetId();
    if (pArea->ComputeSafeLocation(vPosition, JUMP_TO_OBJECT_SEARCH_RADIUS,
            m_pPathfindInformation, bCheckObjects, &vSafePosition) == 1)
    {
        AutoCloseGUIPanels(0);
        CutWaypointPath();
        float fX = m_vOrientation.m_fX;
        float fY = m_vOrientation.m_fY;
        unsigned long oidLeader = g_pAppManager->GetClientExoApp()->
            GetSWParty()->m_aPartyMemberData[0].m_oidCreature;
        if (GetId() == g_pAppManager->GetServerExoApp()->ClientToServerObjectId(
                oidLeader))
        {
            g_pAppManager->GetClientExoApp()->GetSWParty()->ResetAllFollowData(
                vSafePosition, Yaw(normalize(Vector(fX, fY, 0.0f))), 1);
        }
        CServerExoAppInternal* pServer = g_pAppManager->GetServerExoApp()->
            m_pcExoAppInternal;
        CSWSPlayer* pPlayer = pServer->GetClientObjectByObjectId(m_nId);
        SetOrientation(vOrientation);
        if (pArea == GetArea())
        {
            SetPosition(vSafePosition, 1, 1, 0);
            UpdateSubareasOnJumpPosition(vSafePosition, pArea->GetId(), 1);
        }
        else
        {
            if (pPlayer != NULL)
                UpdateSubareasOnJumpPosition(vSafePosition, OBJECT_INVALID, 1);
            AddToArea(pArea, vSafePosition.m_fX, vSafePosition.m_fY,
                vSafePosition.m_fZ, 0, 0);
            if (pPlayer == NULL)
                UpdateSubareasOnJumpPosition(vSafePosition, pArea->GetId(), 1);
            else
            {
                AddActionToFront(ACTION_AREA_WAIT, pAction->m_nGroupId,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL);
            }
        }
        AddActionToFront(ACTION_CHECK_INTER_AREA_PATHFINDING, pAction->m_nGroupId,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL);
        m_bJumped = 1;
        return ACTION_COMPLETE;
    }
    m_pPathfindInformation->ResetInterAreaSearchData();
    return ACTION_FAILED;
}
// PC 0x00553480
int CSWSCreature::AIActionJumpToPoint(CSWSObjectActionNode* pAction)
{
    if (GetDead() != 1 && GetIsPCDying() != 1 && GetDeadTemp() != 1 &&
        m_pPathfindInformation != NULL)
    {
        if (GetArea() == NULL &&
            g_pAppManager->GetServerExoApp()->GetModule()->IsObjectInLimbo(GetId()))
        {
            g_pAppManager->GetServerExoApp()->GetModule()->RemoveObjectFromLimbo(GetId());
        }

        int bUseSafeLocation = 1;
        float fRadius = JUMP_TO_OBJECT_SEARCH_RADIUS;
        float fOrientationX = m_vOrientation.m_fX;
        float fOrientationY = m_vOrientation.m_fY;
        float fX = *(float*)&pAction->m_apParam[0];
        float fY = *(float*)&pAction->m_apParam[1];
        float fZ = *(float*)&pAction->m_apParam[2];
        unsigned long oidArea = (unsigned long)pAction->m_apParam[3];
        if (pAction->m_nNumParams >= 5)
            bUseSafeLocation = (int)pAction->m_apParam[4];
        if (pAction->m_nNumParams >= 6)
            fRadius = *(float*)&pAction->m_apParam[5];
        if (pAction->m_nNumParams >= 7)
        {
            fOrientationX = *(float*)&pAction->m_apParam[6];
            fOrientationY = *(float*)&pAction->m_apParam[7];
        }
        EXOLOG("\nJumping to Point %f %f %f, Area %x\n", fX, fY, fZ, oidArea);

        CSWSArea* pArea = GetArea();
        CGameObject* pObject;
        unsigned char nResult = g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidArea, &pObject);
        if (nResult == CGameObjectArray::SUCCESS &&
            pObject != NULL && pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREA)
        {
            pArea = pObject->AsSWSArea();
            m_pPathfindInformation->ResetInterTileSearchData();
            m_pPathfindInformation->ResetGridSearchData();
            Vector vSafePosition;
            m_pPathfindInformation->m_oidPathfindCreature = GetId();
            if (pArea->ComputeSafeLocation(Vector(fX, fY, fZ), fRadius,
                    m_pPathfindInformation, bUseSafeLocation != 0, &vSafePosition) == 1)
            {
                EXOLOG("Jumping To Point:Found safe point at %f %f\n", vSafePosition.m_fX, vSafePosition.m_fY);
                AutoCloseGUIPanels(0);
                CutWaypointPath();
                unsigned long oidLeader = g_pAppManager->GetClientExoApp()->GetSWParty()->m_aPartyMemberData[0].m_oidCreature;
                if (GetId() == g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidLeader))
                {
                    g_pAppManager->GetClientExoApp()->GetSWParty()->ResetAllFollowData(
                        vSafePosition, Yaw(normalize(Vector(fOrientationX, fOrientationY, 0.0f))), 1);
                }
                CServerExoAppInternal* pServer = g_pAppManager->GetServerExoApp()->m_pcExoAppInternal;
                CSWSPlayer* pPlayer = pServer->GetClientObjectByObjectId(m_nId);
                if (pArea == GetArea())
                {
                    SetPosition(vSafePosition, 1, 1, 0);
                    UpdateSubareasOnJumpPosition(vSafePosition, pArea->GetId(), 1);
                }
                else
                {
                    if (pPlayer != NULL)
                        UpdateSubareasOnJumpPosition(vSafePosition, OBJECT_INVALID, 1);
                    AddToArea(pArea, vSafePosition.m_fX, vSafePosition.m_fY, vSafePosition.m_fZ, 0, 0);
                    if (pPlayer == NULL)
                        UpdateSubareasOnJumpPosition(vSafePosition, pArea->GetId(), 1);
                    else
                    {
                        AddActionToFront(ACTION_AREA_WAIT, pAction->m_nGroupId,
                            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                    }
                }
                SetOrientation(normalize(Vector(fOrientationX, fOrientationY, 0.0f)));
                AddActionToFront(ACTION_CHECK_INTER_AREA_PATHFINDING, pAction->m_nGroupId,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                m_bJumped = 1;
                return ACTION_COMPLETE;
            }
            EXOLOG("Jumping To Point:Cannot find safe point\n");
            m_pPathfindInformation->ResetInterAreaSearchData();
        }
    }
    return ACTION_FAILED;
}

int CSWSCreature::CheckUseMagicDeviceSkill(CSWSItem* pItem, int nRestrictionType)
{
    return 0;
}
// PC 0x00553850
int CSWSCreature::WalkUpdateLocation_QuickWalk(int bResetTime)
{
    if (m_nClientState == 0)
        return ACTION_COMPLETE;
    int nAnimationState = m_nAnimationState;
    if (nAnimationState == 1 ||
        GetDead() || GetIsPCDying() || GetDeadTemp())
        return ACTION_COMPLETE;
    unsigned long nDay = 0, nTime = 0, nElapsedDays = 0;
    g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId())->GetWorldTime(&nDay, &nTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastWalkDay, m_nLastWalkTime, &nElapsedDays, &m_nWalkElapsedTime);
    if (bResetTime != 0)
    {
        m_nWalkElapsedTime = 0;
        m_nLastWalkDay = nDay;
        m_nLastWalkTime = nTime;
    }
    if (m_nAnimationState == 3)
    {
        int nResult = WalkUpdateLocation_QuickWalk_FollowLeader();
        m_nLastWalkDay = nDay;
        m_nLastWalkTime = nTime;
        return nResult;
    }
    if (m_nAnimationState == ANIMATION_STATE_FORCE_PUSH ||
        m_nAnimationState == ANIMATION_STATE_FORCE_JUMP ||
        m_nAnimationState == ANIMATION_STATE_FORCE_JUMP_TARGET)
    {
        int nResult = WalkUpdateLocation_QuickWalk_ForcePushed();
        m_nLastWalkDay = nDay;
        m_nLastWalkTime = nTime;
        return nResult;
    }
    if (GetPathfindInformation()->m_nField_26C != 0)
    {
        unsigned long nCurrentDay, nCurrentTime;
        g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
            &nCurrentDay, &nCurrentTime);
        unsigned long nTimeoutTime = GetPathfindInformation()->m_nField_274;
        unsigned long nTimeoutDay = GetPathfindInformation()->m_nField_270;
        if (g_pAppManager->GetServerExoApp()->GetWorldTimer()->CompareWorldTimes(
                nCurrentDay, nCurrentTime, nTimeoutDay, nTimeoutTime) >= 0)
        {
            GetPathfindInformation()->HandlePathfindError(0,
                "Force Timeout, executing a forced move to point.");
            Vector vTarget(GetPathfindInformation()->m_fStartX,
                            GetPathfindInformation()->m_fStartY,
                            GetPathfindInformation()->m_fStartZ);
            unsigned long oidArea = GetPathfindInformation()->m_nStartArea;
            CSWSForcedAction forcedAction;
            forcedAction.m_oidTargetArea = oidArea;
            forcedAction.m_vTargetLocation = vTarget;
            forcedAction.m_oidTargetObject =
                GetPathfindInformation()->m_oidPathfindTargetCreature;
            m_nAnimationState = 1;
            ForceMoveToPoint(&forcedAction);
            CSWSObjectActionNode* pAction = m_lstActions.RemoveHead();
            if (pAction->m_nActionType == ACTION_MOVE_TO_POINT)
            {
                m_pCurrentActionNode = NULL;
                delete pAction;
                m_nActionStartDay = 0;
                m_nActionStartTime = 0;
            }
            else
                m_lstActions.AddHead(pAction);
            SetAnimation(GetPauseReadyAnimation());
            CPathfindInformation* pPathfind = GetPathfindInformation();
            pPathfind->m_nField_26C = 0;
            pPathfind->m_nField_270 = 0;
            pPathfind->m_nField_274 = 0;
            return ACTION_COMPLETE;
        }
    }
    if (m_nWalkElapsedTime == 0)
        return ACTION_IN_PROGRESS;
    m_nLastWalkDay = nDay;
    m_nLastWalkTime = nTime;
    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
    float fRate;
    if (GetActivity(1))
        fRate = GetWalkRate();
    else if (m_bRun == 0)
    {
        if (m_nAIStateFlags & AI_STATE_CAN_MOVE)
            SetAnimation(CREATURE_ANIMATION_WALK);
        fRate = GetWalkRate();
    }
    else
    {
        if (m_nAIStateFlags & AI_STATE_CAN_MOVE)
            SetAnimation(CREATURE_ANIMATION_RUN);
        fRate = GetRunRate();
    }
    if (fRate == 0.0f)
        return ACTION_IN_PROGRESS;
    if (GetInParty() && HasAction(ACTION_FOLLOW_LEADER))
    {
        int nMember = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(GetClientCreature()->GetId());
        if (nMember >= 0)
        {
#ifdef _XBOX
            CSWPartyMemberData* pMember = pParty->GetMemberData(nMember);
#else
            CSWPartyMemberData* pMember = &pParty->m_aPartyMemberData[nMember];
#endif
            Vector vDifference = pMember->m_vFollowPosition - pMember->m_vFormationPosition;
            float fSquaredDistance = vDifference * vDifference;
            float fFollowRate;
#ifdef _XBOX
            if (fSquaredDistance >= 0.001f)
            {
                fFollowRate = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)->GetSWAnimationSpeed();
                if (GetRunRate() > fFollowRate * 1000.0f) fRate = GetRunRate();
                else fRate = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)->GetSWAnimationSpeed() * 1000.0f;
            }
            if (fSquaredDistance <= 1.0f) fRate = fRate * 0.9f; else if (fSquaredDistance <= 225.0f) fRate = fRate * 1.2f; else fRate = fRate * 1.5f;
#else
            if (fSquaredDistance >= 0.001f)
            {
                fRate = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)->GetSWAnimationSpeed();
                if (GetRunRate() > fRate * 1000.0f) fFollowRate = GetRunRate(); else fFollowRate = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0)->GetSWAnimationSpeed() * 1000.0f;
            }
            else fFollowRate = fRate;
            if (fSquaredDistance <= 1.0f) fRate = fFollowRate * 0.9f; else if (fSquaredDistance <= 225.0f) fRate = fFollowRate * 1.2f; else fRate = fFollowRate * 1.5f;

#endif
        }
    }
    float fSpeedFactor = ComputeSpeedFactor(fRate, m_nWalkElapsedTime);
    fRate = (fSpeedFactor + m_fMovementRate) * m_nWalkElapsedTime * fRate * 0.5f;
    fRate *= 0.000001f;
    if (NPCTurbo)
        fRate *= 15.0;
    m_fMovementRate = fSpeedFactor;
    Vector vPosition, vDirection;
    CExoArrayList<unsigned long> lstIntersectingSubAreas;
    int nPoint = m_pPathfindInformation->m_nField_9C;
    int bAtEnd = WalkUpdateLocationDistance(fRate, &vPosition, &vDirection, &lstIntersectingSubAreas);
    float fHeight = vPosition.m_fZ - m_vPosition.m_fZ;
    if (fHeight <= -0.001f || fHeight >= 0.001f)
    {
        Vector vSlope(fRate, 0.0f, fHeight);
        Vector vSlopeDisplacement = fRate * normalize(vSlope);
        GetPathfindInformation()->m_nField_9C = nPoint;
        bAtEnd = WalkUpdateLocationDistance(vSlopeDisplacement.m_fX,
            &vPosition, &vDirection, &lstIntersectingSubAreas);
    }
    AutoCloseGUIPanels(1);
    UpdateSubareasOnMoveTo(m_vPosition, vPosition, 1, &lstIntersectingSubAreas, 0);
    SetPosition(vPosition, 1, 1, 0);
    if (GetInParty())
    {
        unsigned long oidClient = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId());
        int nMember = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(oidClient);
        if (nMember != -1)
        {
#ifdef _XBOX
            CSWParty* pCurrentParty = g_pAppManager->GetClientExoApp()->GetSWParty(); CSWPartyMemberData* pMember = pCurrentParty->GetMemberData(nMember);
#else
            CSWParty* pCurrentParty = g_pAppManager->GetClientExoApp()->GetSWParty(); CSWPartyMemberData* pMember = &pCurrentParty->m_aPartyMemberData[nMember];
#endif
            if (pMember != NULL)
                pMember->m_vLastPosition = vPosition;
        }
    }
    vDirection.m_fZ = 0.0f;
    vDirection.Normalize();
    SetOrientation(vDirection);
    if (m_bInParty != 0)
    {
        if (g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0) != NULL)
        {
            CSWCCreature* pLeader =
                g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
            if (GetId() == g_pAppManager->GetServerExoApp()
                    ->ClientToServerObjectId(pLeader->GetId()))
            {
                CSWParty* pLeaderParty = g_pAppManager->GetClientExoApp()->GetSWParty();
                pLeaderParty->m_cFollowPaths.ProcessPoint(vPosition, Yaw(vDirection), 0.5f);
                CSWCCreature* pClientCreature = GetClientCreature();
                if (pClientCreature != NULL)
                    pClientCreature->ResetCameraToFollow();
            }
        }
        if (m_bInParty != 0)
        {
            unsigned long oidClient = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId());
            int nMember = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(oidClient);
            if (nMember != -1)
            {
                CSWParty* pCurrentParty = g_pAppManager->GetClientExoApp()->GetSWParty();
#ifdef _XBOX
                CSWPartyMemberData* pMember = pCurrentParty->GetMemberData(nMember);
#else
                CSWPartyMemberData* pMember = &pCurrentParty->m_aPartyMemberData[nMember];
#endif
                CSWPartyFollowDataArray* pFollowPaths = &pCurrentParty->m_cFollowPaths;
                unsigned long oidLeaderClient = pCurrentParty->m_aPartyMemberData[0].m_oidCreature;
                unsigned long oidLeader = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidLeaderClient);
                CSWSCreature* pLeader = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidLeader);
                Vector vDifference;
                if (pLeader != NULL)
                {
                    vDifference = pLeader->m_vPosition - pMember->m_vLeaderPosition;
                    if (HasAction(ACTION_FOLLOW_LEADER) && bAtEnd == 0 && vDifference * vDifference >= NWSCREATURE_FOLLOW_PATH_WAIT_DISTANCE_SQUARED)
                    {
                        g_pAppManager->GetClientExoApp()->GetSWParty()->RecaulateFollowPoint();
                        Vector vFollowPosition = pMember->m_vFormationPosition;
                        int nFollowPoint = pMember->m_nFormationPathIndex;
                        float fFacing = pFollowPaths->GetPathData(nFollowPoint)->m_fFacing;
                        pMember->m_vFollowPosition = vFollowPosition;
                        pMember->m_nFollowPathIndex = nFollowPoint;
                        Vector vOffsetPosition = vFollowPosition + Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fFacing)) * pMember->m_vFormationOffset;
                        vOffsetPosition.m_fZ = GetArea()->ComputeHeight(vOffsetPosition, 1, NULL);
                        Vector vSafePosition;
                        int bSafe = 0;
                        if (pMember->m_nCharacterIndex == CSWPartyMemberData::FOLLOW_OFFSET_WAIT_DEST && GetArea()->TestSafeLocationPoint(vOffsetPosition, m_pPathfindInformation))
                        {
                            vSafePosition = vOffsetPosition;
                            bSafe = 1;
                        }
                        else if (pMember->m_nCharacterIndex == CSWPartyMemberData::FOLLOW_PATH_WAIT_DEST &&
                            GetArea()->TestSafeLocationPoint(vFollowPosition, m_pPathfindInformation))
                        {
                            vSafePosition = vFollowPosition;
                            bSafe = 1;
                        }
                        if (bSafe)
                        {
                            m_nAnimationState = -1;
                            GetPathfindInformation()->m_vField_248 = vSafePosition;
                            CSWSObjectActionNode* pAction = GetAction(ACTION_MOVE_TO_POINT);
                            if (pAction != NULL)
                            {
                                pAction->m_apParam[0] = *(void**)&vSafePosition.m_fX;
                                pAction->m_apParam[1] = *(void**)&vSafePosition.m_fY;
                                pAction->m_apParam[2] = *(void**)&vSafePosition.m_fZ;
                            }
                        }
                        return ACTION_IN_PROGRESS;
                    }
                }
            }
        }
    }
    unsigned long oidTarget = m_pPathfindInformation->m_oidPathfindTargetCreature;
    CGameObject* pTarget = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pTarget != NULL)
    {
        if (pTarget->m_nObjectType == SW_OBJECT_TYPE_TRIGGER)
        {
            if (pTarget->AsSWSTrigger()->m_bMineTrigger == 0 &&
                GetIsInUseRange(oidTarget, 0.0f, 0))
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK;
                pEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
                return ACTION_COMPLETE;
            }
        }
        else if (pTarget->m_nObjectType == SW_OBJECT_TYPE_DOOR)
        {
            CSWSDoor* pDoor = pTarget->AsSWSDoor();
            if (pDoor->GetIsLinked() && GetIsInUseRange(oidTarget, 0.0f, 0))
            {
                if (pDoor->m_nOpenState == CSWSDoor::OPEN_STATE_CLOSED)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, oidTarget, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
                    return ACTION_IN_PROGRESS;
                }
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK;
                pEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
                return ACTION_COMPLETE;
            }
        }
        else if (pTarget->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
        {
            Vector vTarget = pTarget->AsSWSCreature()->m_vPosition;
            Vector vLastTarget = GetPathfindInformation()->m_vField_248;
            Vector vTargetDifference = vTarget - vLastTarget;
            if (vTargetDifference * vTargetDifference >=
                NWSCREATURE_WALK_TARGET_REFRESH_DISTANCE_SQUARED)
            {
                m_nAnimationState = -1;
                GetPathfindInformation()->m_vField_248 = vTarget;
                return ACTION_IN_PROGRESS;
            }
        }
    }
    if (bAtEnd == 1 && m_nAnimationState != ANIMATION_STATE_MOVE_PENDING)
    {
        GetPathfindInformation()->ResetAtEndOfPath();
        m_nAnimationState = 1;
        if (m_lstActions.GetHeadPos() != NULL)
        {
            CSWSObjectActionNode* pAction = m_lstActions.RemoveHead();
            if (pAction->m_nActionType == ACTION_MOVE_TO_POINT)
            {
                m_pCurrentActionNode = NULL;
                delete pAction;
                m_nActionStartDay = 0;
                m_nActionStartTime = 0;
            }
            else
                m_lstActions.AddHead(pAction);
        }
        SetAnimation(GetPauseReadyAnimation());
        return ACTION_COMPLETE;
    }
    return ACTION_IN_PROGRESS;
}
// PC 0x00554710
int CSWSCreature::AIActionDrive(CSWSObjectActionNode* pAction)
{
    if (GetDead() == TRUE || GetIsPCDying() == TRUE || GetDeadTemp() == TRUE)
        return ACTION_FAILED;
    Vector vRequested;
    vRequested.m_fX = *(float*)&pAction->m_apParam[0];
    vRequested.m_fY = *(float*)&pAction->m_apParam[1];
    int nDirection = (int)pAction->m_apParam[2];
    int nDriveMode = (int)pAction->m_apParam[3];
    unsigned long nFlags = (unsigned long)pAction->m_apParam[4];
    int nRepeat = (int)pAction->m_apParam[5];
    int bNewPath = (int)pAction->m_apParam[6];
    CSWSArea* pArea = GetArea();
    if (pArea == NULL)
        return ACTION_FAILED;
    m_bUpdateMovingOrientation = TRUE;
    if (m_nUnknown8E8 == 1 && (nFlags & DRIVE_RUN) == DRIVE_RUN)
        nFlags = (nFlags & ~DRIVE_RUN) | DRIVE_FORWARD;
    Vector vDestination = m_vPosition;
    if (bNewPath)
    {
        m_fClientPathTime = 1.0f;
        pAction->m_apParam[6] = NULL;
        double fAngle = (nDirection / 10.0) * NWSCREATURE_DRIVE_RADIANS_PER_DEGREE;
        Vector vFacing(cos(fAngle), sin(fAngle), 0.0f);
        float fMovementFactor = 1.0f;
        float fDistance = 0.0f;
        Vector vMovement;
        if (nFlags & DRIVE_STRAFE_LEFT)
        {
            vMovement += Vector(-vFacing.m_fY, vFacing.m_fX, 0.0f);
            fDistance = 0.8f;
        }
        else if (nFlags & DRIVE_STRAFE_RIGHT)
        {
            vMovement += Vector(vFacing.m_fY, -vFacing.m_fX, 0.0f);
            fDistance = 0.8f;
        }
        if ((nFlags & DRIVE_RUN) == DRIVE_RUN)
        {
            vMovement += vFacing;
            fDistance = GetRunRate() * 0.0004f;
        }
        else if (nFlags & DRIVE_FORWARD)
        {
            vMovement += vFacing;
            fDistance = GetWalkRate() * 0.0004f;
        }
        else if (nFlags & DRIVE_BACKWARD)
        {
            vMovement -= vFacing;
            fDistance = GetWalkRate() * 0.0004f;
        }
        if (nFlags & DRIVE_TURN_LEFT)
        {
            Quaternion qTurn = YawPitchRoll(30.0f, 0.0f, 0.0f);
            vMovement = qTurn * vMovement;
            vFacing = qTurn * vFacing;
        }
        else if (nFlags & DRIVE_TURN_RIGHT)
        {
            Quaternion qTurn = YawPitchRoll(-30.0f, 0.0f, 0.0f);
            vMovement = qTurn * vMovement;
            vFacing = qTurn * vFacing;
        }
        vFacing.Normalize();
        SetOrientation(vFacing);
        vMovement.Normalize();
        Vector vSafePosition;
        Vector vDelta = vRequested - m_vPosition;
        if (vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
                vDelta.m_fZ * vDelta.m_fZ > 0.25f &&
            pArea->ComputeSafeLocation(
                vRequested, 0.5f, GetPathfindInformation(), 0, &vSafePosition))
            SetPosition(vSafePosition, 1, 1, 0);
        else
            vSafePosition = m_vPosition;
        float fStartX = m_vPosition.m_fX;
        float fStartY = m_vPosition.m_fY;
        float fRadius = GetPathfindInformation()->m_fStepScale;
        float fWidth = GetPathfindInformation()->m_fHalfStep;
        m_fClientPathTime = 1.0f;
        vSafePosition += vMovement * fDistance;
        vSafePosition.m_fZ = GetArea()->ComputeHeight(vSafePosition, 0, NULL);
        int bFound = FALSE;
        unsigned long oidBlocker;
        if (!pArea->NoCreaturesOnLine(m_vPosition, vSafePosition,
                GetPathfindInformation(), 1, 0, &oidBlocker, 0, NULL) &&
            oidBlocker != OBJECT_INVALID)
        {
            CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->
                GetCreatureByGameObjectID(oidBlocker);
            if (pCreature != NULL)
                BumpFriends(pCreature, m_vPosition, vSafePosition);
        }
        if (pArea->TestDirectLine(Vector(fStartX, fStartY, 0.0f), vSafePosition,
                fRadius, fWidth, 0, NULL) == 1)
        {
            vDestination = vSafePosition;
            bFound = TRUE;
        }
        else
        {
            float fFraction = 0.5f;
            float fStep = 0.25f;
            Vector vTrial;
            do
            {
                vTrial = m_vPosition + (vSafePosition - m_vPosition) * fFraction;
                if (pArea->TestDirectLine(Vector(fStartX, fStartY, 0.0f), vTrial,
                        fRadius, fWidth, 0, NULL) == 1)
                {
                    vDestination = vTrial;
                    fFraction += fStep;
                    bFound = TRUE;
                }
                else
                    fFraction -= fStep;
                fStep *= 0.5f;
            } while (fStep > 0.005f);
            if (bFound == TRUE && nRepeat > 1)
                pAction->m_apParam[5] = (void*)(nRepeat - 1);
            if (!bFound)
            {
                Vector vRight = normalize(Vector(vMovement.m_fY, -vMovement.m_fX, 0.0f));
                Vector vLeft = normalize(vRight * -1.0f);
                Vector vLeftBlocked = vMovement;
                Vector vRightBlocked = vMovement;
                int bSearchLeft = TRUE;
                int bSearchRight = TRUE;
                int bLeftClear;
                int bRightClear;
                Vector vLeftTrial;
                Vector vRightTrial;
                for (int nSearch = 0; nSearch < 15; nSearch++)
                {
                    if (!bSearchLeft && !bSearchRight)
                        break;
                    vLeftTrial = normalize(vLeft + vLeftBlocked);
                    vRightTrial = normalize(vRight + vRightBlocked);
                    if (bSearchLeft)
                    {
                        vTrial = m_vPosition + vLeftTrial * fDistance;
                        bLeftClear = pArea->TestDirectLine(Vector(fStartX, fStartY, 0.0f),
                            vTrial, fRadius, fWidth, 0, NULL) == 1;
                    }
                    if (bSearchRight)
                    {
                        vTrial = m_vPosition + vRightTrial * fDistance;
                        bRightClear = pArea->TestDirectLine(Vector(fStartX, fStartY, 0.0f),
                            vTrial, fRadius, fWidth, 0, NULL) == 1;
                    }
                    if (bSearchLeft)
                    {
                        if (bLeftClear)
                            vLeft = vLeftTrial;
                        else
                        {
                            bSearchLeft = !bRightClear;
                            vLeftBlocked = vLeftTrial;
                        }
                    }
                    if (bSearchRight)
                    {
                        if (bRightClear)
                            vRight = vRightTrial;
                        else
                        {
                            bSearchRight = !bLeftClear;
                            vRightBlocked = vRightTrial;
                        }
                    }
                }
                Vector vSlide;
                if (bSearchLeft == bSearchRight)
                {
                    if (vLeft * vMovement > vRight * vMovement)
                        vSlide = vLeft;
                    else
                        vSlide = vRight;
                }
                else if (bSearchLeft)
                    vSlide = vLeft;
                else if (bSearchRight)
                    vSlide = vRight;
                fMovementFactor = vSlide * vMovement;
                fMovementFactor = max(fMovementFactor, 0.25f);
                vDestination = m_vPosition + vSlide * (fDistance * fMovementFactor);
                bFound = pArea->TestDirectLine(Vector(fStartX, fStartY, 0.0f), vDestination,
                    fRadius, fWidth, 0, NULL) == 1;
            }
        }
        if (bFound == TRUE)
        {
            m_fClientPathTime = fMovementFactor;
            float* pfPath = new float[2];
            pfPath[0] = vDestination.m_fX;
            pfPath[1] = vDestination.m_fY;
            GetPathfindInformation()->m_nWayPointCount = 2;
            GetPathfindInformation()->m_pWayPoints = pfPath;
            GetPathfindInformation()->m_nPathFlags = 1;
            GetPathfindInformation()->m_oidPathfindCreature = GetId();
            GetPathfindInformation()->m_oidPathfindTargetCreature = OBJECT_INVALID;
            GetPathfindInformation()->m_nField_84 = 0;
            GetPathfindInformation()->m_nStartArea = m_oidArea;
            GetPathfindInformation()->m_fStartX = vDestination.m_fX;
            GetPathfindInformation()->m_fStartY = vDestination.m_fY;
            GetPathfindInformation()->m_fStartZ = vDestination.m_fZ;
            GetPathfindInformation()->m_fEndX = m_vPosition.m_fX;
            GetPathfindInformation()->m_fEndY = m_vPosition.m_fY;
            GetPathfindInformation()->SetEndZ(m_vPosition.m_fZ);
            GetPathfindInformation()->m_nField_9C = 0;
            GetPathfindInformation()->m_nField_28 = nDriveMode;
        }
        else
        {
            GetPathfindInformation()->m_nField_28 = nDriveMode;
            TerminateClientSidePath(nDriveMode);
            m_bUpdateMovingOrientation = FALSE;
            SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
            return ACTION_FAILED;
        }
    }
    Vector vTarget(GetPathfindInformation()->m_fStartX,
                   GetPathfindInformation()->m_fStartY,
                   GetPathfindInformation()->m_fStartZ);
    vDestination = vTarget;
    int bRun = FALSE;
    unsigned short nAnimation = CREATURE_ANIMATION_RUN;
    if (nFlags & DRIVE_STRAFE_LEFT)
        nAnimation = CREATURE_ANIMATION_FOOTSTEP_MOVE_10078;
    else if (nFlags & DRIVE_STRAFE_RIGHT)
        nAnimation = CREATURE_ANIMATION_FOOTSTEP_MOVE_10079;
    if ((nFlags & DRIVE_RUN) == DRIVE_RUN)
    {
        bRun = TRUE;
        if (nFlags & DRIVE_STRAFE_LEFT)
            nAnimation = CREATURE_ANIMATION_FOOTSTEP_MOVE_10086;
        else
            nAnimation = (nFlags & DRIVE_STRAFE_RIGHT) ?
                CREATURE_ANIMATION_FOOTSTEP_MOVE_10087 : CREATURE_ANIMATION_RUN;
    }
    else if (nFlags & DRIVE_FORWARD)
    {
        if (nFlags & DRIVE_STRAFE_LEFT)
            nAnimation = CREATURE_ANIMATION_FOOTSTEP_MOVE_10084;
        else
            nAnimation = (nFlags & DRIVE_STRAFE_RIGHT) ?
                CREATURE_ANIMATION_FOOTSTEP_MOVE_10085 : CREATURE_ANIMATION_WALK;
    }
    else if (nFlags & DRIVE_BACKWARD)
        nAnimation = CREATURE_ANIMATION_WALK_BACKWARD;
    SetAnimation(nAnimation);
    int nResult = DriveUpdateLocation(bRun);
    if (nResult != ACTION_IN_PROGRESS)
    {
        if (nRepeat > 1 && nResult == ACTION_COMPLETE)
        {
            double fDirection = atan2(m_vOrientation.m_fY, m_vOrientation.m_fX) * 57.29577951308232;
            if (fDirection < 0.0)
                fDirection += 360.0;
            AddDriveAction(pAction->m_nGroupId, m_vPosition,
                (unsigned short)(10 * (int)fDirection),
                nDriveMode, nFlags, nRepeat - 1);
        }
        else
        {
            m_bUpdateMovingOrientation = FALSE;
            SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
        }
    }
    return nResult;
}

#ifdef _XBOX
static __int64 QueryTimer();
#define AIACTIONMOVETOPOINT_RETURN(nValue) { m_fMoveToPointTime += (float)(int)(QueryTimer() - nStartTime) * 0.001f; return nValue; }
#else
#define AIACTIONMOVETOPOINT_RETURN(nValue) return nValue
#endif
int CSWSCreature::AIActionMoveToPoint(CSWSObjectActionNode* pAction)  // PC 0x005559F0
{
    if (GetDead() == 1 || GetIsPCDying() == 1 || GetDeadTemp() == 1)
        return ACTION_FAILED;

    int nAnimationState = m_nAnimationState;
    if (m_nClientState != 0)
    {
        if (nAnimationState == ANIMATION_STATE_MOVING || nAnimationState == ANIMATION_STATE_FORCE_PUSH || nAnimationState == ANIMATION_STATE_FORCE_JUMP || nAnimationState == ANIMATION_STATE_FORCE_JUMP_TARGET)
            return ACTION_IN_PROGRESS;
    }
    else
        m_nAnimationState = ANIMATION_STATE_MOVE_UNINITIALIZED;

    if (m_pPathfindInformation == NULL)
        return ACTION_FAILED;
#ifdef _XBOX
    if (m_nMoveToPointElapsed == -1)
    {
        m_fMoveToPointTime = 0.0f;
        m_nMoveToPointElapsed = 0;
        m_nMoveToPointCalls = 1;
    }
    else
    {
        m_nMoveToPointElapsed += m_nElapsedTime;
        m_nMoveToPointCalls++;
    }
    __int64 nStartTime = QueryTimer();
#endif

    float fX;
    float fY;
    float fZ;
    memcpy(&fX, &pAction->m_apParam[0], sizeof(float));
    memcpy(&fY, &pAction->m_apParam[1], sizeof(float));
    memcpy(&fZ, &pAction->m_apParam[2], sizeof(float));
    Vector vDestination(fX, fY, fZ);
    unsigned long oidArea = (unsigned long)pAction->m_apParam[3];
    unsigned long nForceDay = 0;
    unsigned long nForceTime = 0;
    Vector vOrientation;
    unsigned long oidTarget;
    if (pAction->m_nNumParams >= 5)
        oidTarget = (unsigned long)pAction->m_apParam[4];
    else
        oidTarget = OBJECT_INVALID;
    int nFlags = 0;
    if (pAction->m_nNumParams >= 6)
        nFlags = (int)pAction->m_apParam[5];
    int bRun = (nFlags & MOVE_RUN_MASK) > 0;
    int bUsePath = (nFlags >> MOVE_USE_PATH_SHIFT) & 1;
    int bTimeout = (nFlags >> MOVE_TIMEOUT_SHIFT) & 1;
    int nMovementOption = (nFlags >> MOVE_OPTION1_SHIFT) & MOVE_OPTION1_INPUT_MASK;
    int bClientMoving = (nFlags >> MOVE_CLIENT_SHIFT) & MOVE_CLIENT_INPUT_MASK;
    int bOption2 = (nFlags >> MOVE_OPTION2_SHIFT) & 1;
    int bUsePoint = (nFlags >> MOVE_OPTION3_SHIFT) & MOVE_OPTION3_INPUT_MASK;
    int bOption6 = (nFlags >> MOVE_OPTION6_SHIFT) & MOVE_OPTION6_INPUT_MASK;
    m_pPathfindInformation->m_nField_240 = bOption6;
    if (m_nUnknown8E8 == 1)
        bRun = 0;

    float fRange;
    if (pAction->m_nNumParams >= 7)
    {
        fRange = *(float*)&pAction->m_apParam[6];
        Vector vDelta = vDestination - m_vPosition;
        Vector vHit;
        unsigned long oidHit;
        if (vDelta * vDelta < fRange * fRange && GetArea()->ClearLineOfSight(m_vPosition, vDestination, &vHit, &oidHit, OBJECT_INVALID, OBJECT_INVALID, 0, 0) == 0)
            fRange = 0.0f;
    }
    else
        fRange = 0.0f;
    int nMovementRate;
    if (pAction->m_nNumParams >= 8)
        nMovementRate = (int)pAction->m_apParam[7];
    else
        nMovementRate = 0;
    float fTimeout;
    if (pAction->m_nNumParams >= 9)
        fTimeout = *(float*)&pAction->m_apParam[8];
    else
        fTimeout = 0.0f;
    if (pAction->m_nNumParams >= 10)
    {
        EXOXBOXASSERT(pAction->m_nNumParams >= 11);
        vOrientation.m_fX = *(float*)&pAction->m_apParam[9];
        vOrientation.m_fY = *(float*)&pAction->m_apParam[10];
    }
    if (pAction->m_nNumParams >= 12)
    {
        EXOXBOXASSERT(pAction->m_nNumParams >= 13);
        nForceDay = (unsigned long)pAction->m_apParam[11];
        nForceTime = (unsigned long)pAction->m_apParam[12];
    }
    unsigned long nDay;
    unsigned long nTime;
    if (nForceDay > 0 && nForceTime > 0)
    {
        g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
        if (g_pAppManager->GetServerExoApp()->GetWorldTimer()->CompareWorldTimes( nDay, nTime, nForceDay, nForceTime) >= 0)
        {
            m_pPathfindInformation->HandlePathfindError(0, "Force Timeout, executing a forced move to point.");
            CSWSForcedAction action;
            action.m_oidTargetArea = oidArea;
            action.m_vTargetLocation.m_fX = vDestination.m_fX;
            action.m_vTargetLocation.m_fY = vDestination.m_fY;
            action.m_vTargetLocation.m_fZ = vDestination.m_fZ;
            action.m_oidTargetObject = oidTarget;
            m_nAnimationState = ANIMATION_STATE_MOVE_COMPLETE;
            ForceMoveToPoint(&action);
            AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
        }
    }
    if (bTimeout == 1 && fTimeout > 0.0f)
    {
        g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
        g_pAppManager->GetServerExoApp()->GetWorldTimer()->AddWorldTimes( nDay, nTime, 0, (unsigned long)(fTimeout * 1000.0f), &nForceDay, &nForceTime);
        AddMoveToPointActionToFront(pAction->m_nGroupId, vDestination, oidArea, oidTarget, bRun, fRange, 0.0f, bUsePath, nMovementRate, nMovementOption, bClientMoving, bOption2, bUsePoint, vOrientation, nForceDay, nForceTime, bOption6);
        CPathfindInformation* pPathfind = m_pPathfindInformation;
        pPathfind->m_nField_26C = 0;
        pPathfind->m_nField_270 = 0;
        pPathfind->m_nField_274 = 0;
        m_nAnimationState = ANIMATION_STATE_MOVE_COMPLETE;
        AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
    }

    CGameObject* pObject;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject( oidTarget, &pObject) == CGameObjectArray::SUCCESS)
    {
        switch (pObject->m_nObjectType)
        {
        case SW_OBJECT_TYPE_TRIGGER:
        {
            Vector vNearest;
            m_pPathfindInformation->SetTargetPosition( pObject->AsSWSTrigger()->m_vPosition);
            if (pObject->AsSWSTrigger()->GetTrapFlag() != 0)
            {
                pObject->AsSWSTrigger()->CalculateNearestPoint(m_vPosition, &vNearest);
                vDestination = vNearest + normalize(m_vPosition - vNearest) * NWSCREATURE_MOVE_TRIGGER_OFFSET;
                AddMoveToPointActionToFront(pAction->m_nGroupId, vDestination, oidArea, OBJECT_INVALID, bRun, fRange, 0.0f, bUsePath, nMovementRate, nMovementOption, bClientMoving, bOption2, bUsePoint, vOrientation, nForceDay, nForceTime, 0);
                AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
            }
            if (GetIsInUseRange(oidTarget, 0.0f, 0))
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK;
                pEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime( 0, 0, m_nId, oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
            }
            break;
        }
        case SW_OBJECT_TYPE_CREATURE:
        {
            if (g_pAppManager->GetServerExoApp()->GetModule() == NULL)
                break;
            CSWSCreature* pTarget = pObject->AsSWSCreature();
            const Vector& vTargetPosition = pTarget->m_vPosition;
            m_pPathfindInformation->SetTargetPosition(vTargetPosition);
            if (pTarget->GetArea() == NULL)
            {
                Vector vTarget = pTarget->m_vDisappearPosition;
                Vector vDelta = vTarget - vDestination;
                if (oidArea != pTarget->m_oidDisappearTarget || vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
                        vDelta.m_fZ * vDelta.m_fZ > NWSCREATURE_MOVE_DISAPPEAR_TOLERANCE_SQUARED)
                {
                    AddMoveToPointActionToFront(pAction->m_nGroupId, vTarget,
                        pTarget->m_oidDisappearTarget, oidTarget, bRun, fRange,
                        0.0f, bUsePath, nMovementRate, nMovementOption,
                        bClientMoving, bOption2, bUsePoint, vOrientation,
                        nForceDay, nForceTime, 0);
                    AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
                }
            }
            else
            {
                unsigned long oidTargetArea = pTarget->GetArea()->GetId();
                Vector vTarget = vTargetPosition;
                if (vOrientation.m_fX != 0.0f || vOrientation.m_fY != 0.0f)
                {
                    Vector vLeader = g_pAppManager->GetClientExoApp()->GetSWParty()->m_vLeaderFollowPosition;
                    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
                    vTarget = vLeader + pParty->m_qLeaderFacing * vOrientation;
                    oidTarget = OBJECT_INVALID;
                }
                float fTargetDistance = magnitude(m_vPosition - vTarget) /
                    NWSCREATURE_MOVE_TARGET_DISTANCE_DIVISOR;
                if (!(fTargetDistance > NWSCREATURE_MOVE_TARGET_MIN_DISTANCE))
                    fTargetDistance = NWSCREATURE_MOVE_TARGET_MIN_DISTANCE;
                Vector vTargetDelta = vDestination - vTarget;
                if (oidTargetArea != oidArea ||
                    vTargetDelta.m_fX * vTargetDelta.m_fX +
                    vTargetDelta.m_fY * vTargetDelta.m_fY + vTargetDelta.m_fZ * vTargetDelta.m_fZ >
                    fTargetDistance * fTargetDistance)
                {
                    pTarget->GetArea()->ComputeSafeLocation(vTarget,
                        NWSCREATURE_MOVE_SAFE_LOCATION_RADIUS, GetPathfindInformation(), 1, &vTarget);
                    Vector vDelta = vTargetPosition - vTarget;
                    if (vTargetDelta.m_fX * vTargetDelta.m_fX +
                        vTargetDelta.m_fY * vTargetDelta.m_fY + vTargetDelta.m_fZ * vTargetDelta.m_fZ >
                        vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
                        vDelta.m_fZ * vDelta.m_fZ)
                    {
                        AddMoveToPointActionToFront(pAction->m_nGroupId, vTarget,
                            oidTargetArea, oidTarget, bRun, fRange, 0.0f, bUsePath,
                            nMovementRate, nMovementOption, bClientMoving, bOption2,
                            bUsePoint, vOrientation, nForceDay, nForceTime, 0);
                        AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
                    }
                }
                Vector vUsePosition;
                float fUseRange;
                GetUseRange(oidTarget, vUsePosition, fUseRange, 0);
                if (vOrientation.m_fX == 0.0f && vOrientation.m_fY == 0.0f)
                {
                    if (GetIsInUseRange(oidTarget, fRange - fUseRange, 0))
                    {
                        m_pPathfindInformation->ResetAtEndOfPath();
                        SetAnimation(ANIMATION_IDLE);
                        m_nAnimationState = ANIMATION_STATE_MOVE_COMPLETE;
                        AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
                    }
                }
                else
                {
                    Vector vDelta = m_vPosition - vTarget;
                    if (vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
                        vDelta.m_fZ * vDelta.m_fZ < NWSCREATURE_MOVE_FORMATION_TOLERANCE_SQUARED)
                    {
                        m_pPathfindInformation->ResetAtEndOfPath();
                        m_nAnimationState = ANIMATION_STATE_MOVE_COMPLETE;
                        SetAnimation(ANIMATION_IDLE);
                        AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
                    }
                }
            }
            break;
        }
        case SW_OBJECT_TYPE_DOOR:
        {
            CSWSDoor* pDoor = pObject->AsSWSDoor();
            m_pPathfindInformation->SetTargetPosition(pDoor->m_vPosition);
            if (pDoor->GetIsLinked() && GetIsInUseRange(oidTarget, 0.0f, 0))
            {
                if (pDoor->m_nOpenState == CSWSDoor::OPEN_STATE_CLOSED)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PATH_BLOCKED;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, oidTarget, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    AIACTIONMOVETOPOINT_RETURN(ACTION_IN_PROGRESS);
                }
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK;
                pEvent->SetObjectID(0, m_nId);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, oidTarget, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                SetAnimation(ANIMATION_IDLE);
                m_pPathfindInformation->ResetGridSearchData();
                m_pPathfindInformation->ResetWayPointData();
                m_pPathfindInformation->ResetInterTileSearchData();
                m_nAnimationState = ANIMATION_STATE_MOVE_COMPLETE;
                AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
            }
            if (bUsePoint == 0)
            {
                pObject->AsSWSDoor();
                Vector vUsePosition;
                float fUseRange;
                GetUseRange(oidTarget, vUsePosition, fUseRange, 0);
                if (fRange != fUseRange || magnitude(vUsePosition - vDestination) >
                    NWSCREATURE_MOVE_USE_POINT_TOLERANCE)
                {
                    AddMoveToPointActionToFront(pAction->m_nGroupId, vUsePosition,
                        oidArea, oidTarget, bRun, fUseRange, 0.0f, bUsePath,
                        nMovementRate, nMovementOption, bClientMoving, bOption2,
                        1, vOrientation, nForceDay, nForceTime, 0);
                    m_nAnimationState = ANIMATION_STATE_MOVE_COMPLETE;
                    AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
                }
            }
            break;
        }
        case SW_OBJECT_TYPE_PLACEABLE:
        {
            m_pPathfindInformation->SetTargetPosition(pObject->AsSWSPlaceable()->m_vPosition);
            if (bUsePoint == 0)
            {
                pObject->AsSWSPlaceable();
                Vector vUsePosition;
                float fUseRange;
                GetUseRange(oidTarget, vUsePosition, fUseRange, 0);
                if (fRange != fUseRange || magnitude(vUsePosition - vDestination) >
                    NWSCREATURE_MOVE_USE_POINT_TOLERANCE)
                {
                    AddMoveToPointActionToFront(pAction->m_nGroupId, vUsePosition,
                        oidArea, oidTarget, bRun, fUseRange, 0.0f, bUsePath,
                        nMovementRate, nMovementOption, bClientMoving, bOption2,
                        1, vOrientation, nForceDay, nForceTime, 0);
                    m_nAnimationState = ANIMATION_STATE_MOVE_COMPLETE;
                    AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
                }
            }
            break;
        }
        }
    }

    if (!(m_nAIStateFlags & AI_STATE_CAN_MOVE))
    {
        if (bUsePath == 1)
            TerminateClientSidePath(nMovementRate);
        m_pPathfindInformation->HandlePathfindError(0,
            "I have no legs! I can't feel my legs!!!!");
        AIACTIONMOVETOPOINT_RETURN(ACTION_FAILED);
    }
    if (GetArea() == NULL)
        AIACTIONMOVETOPOINT_RETURN(ACTION_IN_PROGRESS);
    if (GetArea() != NULL && GetArea()->GetId() == oidArea)
    {
        Vector vDelta = vDestination - m_vPosition;
        if (vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
            vDelta.m_fZ * vDelta.m_fZ <= NWSCREATURE_MOVE_POINT_TOLERANCE_SQUARED)
        {
            if (GetArea()->TestSafeLocationPoint(vDestination, GetPathfindInformation()))
                SetPosition(vDestination, 1, 1, 0);
            else
                m_pPathfindInformation->HandlePathfindError(0,
                    "Bailed the desired position is unsafe.");
            SetAnimation(ANIMATION_COMBAT_READY);
            AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
        }
    }
    m_pPathfindInformation->m_nPathFlags = bUsePath != 0;
    m_pPathfindInformation->m_oidPathfindCreature = m_nId;
    m_pPathfindInformation->m_oidPathfindTargetCreature = oidTarget;
    m_bUpdateMovingOrientation = 0;
    Vector vPathDestination;
    unsigned long oidPathArea;
    if (m_pPathfindInformation->m_nField_84 == 0)
    {
        vPathDestination = Vector(m_pPathfindInformation->m_fStartX,
            m_pPathfindInformation->m_fStartY, m_pPathfindInformation->m_fStartZ);
        oidPathArea = m_pPathfindInformation->m_nStartArea;
    }
    else
    {
        vPathDestination = Vector(m_pPathfindInformation->m_fEndX,
            m_pPathfindInformation->m_fEndY, m_pPathfindInformation->m_fEndZ);
        oidPathArea = m_pPathfindInformation->m_nEndArea;
    }
    Vector vPathDelta = Vector(fX, fY, fZ) - vPathDestination;
    if (oidPathArea != oidArea || vPathDelta.m_fX * vPathDelta.m_fX +
        vPathDelta.m_fY * vPathDelta.m_fY + vPathDelta.m_fZ * vPathDelta.m_fZ >=
        NWSCREATURE_MOVE_POINT_TOLERANCE_SQUARED)
    {
        m_pPathfindInformation->m_fStartX = fX;
        m_pPathfindInformation->m_fStartY = fY;
        m_pPathfindInformation->m_fStartZ = fZ;
        m_pPathfindInformation->m_nStartArea = oidArea;
        CPathfindInformation* pPathfind = m_pPathfindInformation;
        pPathfind->m_fStartDistance = fRange;
        pPathfind->ComputeStepTolerance();
        m_pPathfindInformation->SetEndX(m_vPosition.m_fX);
        m_pPathfindInformation->SetEndY(m_vPosition.m_fY);
        m_pPathfindInformation->SetEndZ(m_vPosition.m_fZ);
        m_pPathfindInformation->m_nEndArea = m_oidArea;
        m_pPathfindInformation->m_nField_58 = 1;
        m_pPathfindInformation->m_nField_88 = 0;
    }
    int nResult;
    if (m_pPathfindInformation->m_nField_88 == 0)
    {
        m_pPathfindInformation->m_nField_198 = bOption2;
        m_pPathfindInformation->m_nBumpCount = 0;
        Vector vDelta = vDestination - m_vPosition;
        if (m_oidAttemptedAttackTarget != OBJECT_INVALID &&
            vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
            vDelta.m_fZ * vDelta.m_fZ < NWSCREATURE_MOVE_ATTACK_TARGET_RANGE_SQUARED)
            m_pPathfindInformation->m_nField_194 = 1;
        if (m_nClientState != 0)
            m_nAnimationState = ANIMATION_STATE_MOVE_PENDING;
        nResult = g_pAppManager->GetServerExoApp()->GetModule()->PlotPath(
            m_pPathfindInformation, AiTimeSlice);
        m_pPathfindInformation->m_nField_198 = 0;
        if (nResult == ACTION_COMPLETE)
        {
            unsigned long oidPathTarget = m_pPathfindInformation->m_oidPathfindTargetCreature;
            m_oidAttemptedMovementTarget = oidPathTarget;
            m_pPathfindInformation->SetBlockingCreature(OBJECT_INVALID);
            if (oidPathTarget != oidTarget)
                pAction->m_apParam[4] = (void*)oidPathTarget;
            m_pPathfindInformation->m_nField_28 = (unsigned char)nMovementRate;
            m_pPathfindInformation->m_nField_9C = 0;
            if (bRun)
            {
                SetAnimation(CREATURE_ANIMATION_RUN);
                m_bRun = 1;
                ActivityManager(ACTIVITY_ACTION_MOVEMENT);
            }
            else
            {
                SetAnimation(CREATURE_ANIMATION_WALK);
                m_bRun = 0;
                ActivityManager(ACTIVITY_ACTION_MOVEMENT);
            }
            if (m_nClientState != 0)
            {
                g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId())->GetWorldTime(
                    &m_nLastWalkDay, &m_nLastWalkTime);
                m_nAnimationState = ANIMATION_STATE_MOVING;
                nResult = WalkUpdateLocation_QuickWalk(1);
            }
            else
                nResult = WalkUpdateLocation();
            g_pAppManager->GetServerExoApp()->m_pcExoAppInternal->GetClientObjectByObjectId(m_nId);
        }
        else if (nResult == ACTION_FAILED)
        {
            m_pPathfindInformation->HandlePathfindError(0,
                "The Path find has Failed... Why?");
            if (m_nClientState != 0)
                m_nAnimationState = ANIMATION_STATE_MOVE_PENDING;
            m_pPathfindInformation->m_nField_28 = (unsigned char)nMovementRate;
            SetAnimation(ANIMATION_IDLE);
            if (bUsePath == 1)
                TerminateClientSidePath(nMovementRate);
            m_pPathfindInformation->SetStartX(m_vPosition.m_fX);
            m_pPathfindInformation->SetStartY(m_vPosition.m_fY);
            m_pPathfindInformation->SetStartZ(m_vPosition.m_fZ);
            m_pPathfindInformation->m_nStartArea = GetArea()->GetId();
            if (bNoForceMoves == 0 && nForceDay > 0 && nForceTime > 0)
            {
                SetPosition(vDestination, 1, 1, 0);
                AIACTIONMOVETOPOINT_RETURN(ACTION_COMPLETE);
            }
        }
    }
    else
    {
        if (bRun)
            SetAnimation(CREATURE_ANIMATION_RUN);
        else
            SetAnimation(CREATURE_ANIMATION_WALK);
        g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId())->GetWorldTime(
            &m_nLastWalkDay, &m_nLastWalkTime);
        ActivityManager(ACTIVITY_ACTION_MOVEMENT);
        m_nAnimationState = ANIMATION_STATE_MOVING;
        if (m_nClientState != 0)
            nResult = ACTION_IN_PROGRESS;
        else
            nResult = WalkUpdateLocation();
    }
    AIACTIONMOVETOPOINT_RETURN(nResult);
}

static inline void NwscreatureaiTrace1()
{
}

#ifdef _XBOX
#include <stdio.h>
#include "aurconsole.h"

static char* Draw_Marker(char* szArgs)
{
    float fX = 0.0f;
    float fY = 0.0f;
    if (sscanf(szArgs, "%f %f", &fX, &fY) != 2)
        return "usage: Draw_Marker X Y";
    Vector vMarker(fX, fY, 0.0f);
    vMarker.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(), vMarker, 1);
    return " ";
}

static char* Draw_Line(char* szArgs)
{
    float fX1 = 0.0f;
    float fY1 = 0.0f;
    float fX2 = 0.0f;
    float fY2 = 0.0f;
    if (sscanf(szArgs, "%f %f %f %f", &fX1, &fY1, &fX2, &fY2) != 4)
        return "usage: Draw_Line X Y";
    Vector vLine[2] = { Vector(fX1, fY1, 0.0f), Vector(fX2, fY2, 0.0f) };
    vLine[0].m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(), vLine[0], 1);
    vLine[1].m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(), vLine[1], 1);
    return " ";
}

static char* TestLocationIsSafe(char* szArgs)
{
    float fX = 0.0f;
    float fY = 0.0f;
    unsigned long oidCreature = OBJECT_INVALID;
    if (sscanf(szArgs, "%f %f %d", &fX, &fY, &oidCreature) != 3 ||
        oidCreature == OBJECT_INVALID)
        return "usage: TestLocationIsSafe X Y ObjectId";
    Vector vLocation(fX, fY, 0.0f);
    vLocation.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(), vLocation, 1);
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL &&
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->TestSafeLocationPoint(
            vLocation, pCreature->m_pPathfindInformation))
        sprintf(sRet, "Good");
    else
        sprintf(sRet, "Invalid Location");
    return sRet;
}

static char* LocateNearestPathPoint(char* szArgs)
{
    float fX = 0.0f;
    float fY = 0.0f;
    unsigned long oidCreature = OBJECT_INVALID;
    if (sscanf(szArgs, "%f %f %d", &fX, &fY, &oidCreature) != 3 ||
        oidCreature == OBJECT_INVALID)
        return "usage: LocateNearestPathPoint X Y ObjectId";
    Vector vLocation(fX, fY, 0.0f);
    vLocation.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(), vLocation, 1);
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    unsigned long nPathPoint = 0xffffffff;
    if (g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->ClosestPathPoint(vLocation, nPathPoint))
    {
        sprintf(sRet, "Good %d", nPathPoint);
        CSWSAreaPathPoint* pPoint =
            &g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->m_pPathPoints[nPathPoint];
        Vector vPoint(pPoint->m_fX, pPoint->m_fY, 0.0f);
        vPoint.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
            g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(), vPoint, 1);
        return sRet;
    }
    sprintf(sRet, "Invalid Location");
    return sRet;
}

static char* ValidateCreaturePath(char* szArgs)
{
    unsigned long oidCreature = OBJECT_INVALID;
    int nArgs = sscanf(szArgs, "%d", &oidCreature);
    if (oidCreature == 99999)
        oidCreature = g_pAppManager->m_pServerExoApp->GetPlayerCreatureId();
    if (nArgs == 1 && oidCreature != OBJECT_INVALID)
    {
        g_pAppManager->m_pServerExoApp->DebugValidatePath(oidCreature, 0);
        return "done";
    }
    return "usage: LocateNearestPathPoint ObjectId";
}

ConsoleFunc ConVRenderParallelograms("RenderParallelograms", &RenderParallelograms);
ConsoleFunc ConFDraw_Marker("Draw_Marker", Draw_Marker);
ConsoleFunc ConFDraw_Line("Draw_Line", Draw_Line);
ConsoleFunc ConFTestLocationIsSafe("TestLocationIsSafe", TestLocationIsSafe);
ConsoleFunc ConFLocateNearestPathPoint("LocateNearestPathPoint", LocateNearestPathPoint);
ConsoleFunc ConFValidateCreaturePath("ValidateCreaturePath", ValidateCreaturePath);
#endif

#ifdef _XBOX

static __int64 QueryTimer()
{
    LARGE_INTEGER nFrequency;
    LARGE_INTEGER nCount;
    static __int64 performanceFrequency =
        (QueryPerformanceFrequency(&nFrequency), nFrequency.QuadPart);

    QueryPerformanceCounter(&nCount);
    return 1000000 * nCount.QuadPart / performanceFrequency;
}
#endif
