#include "nwsobject.h"
#include "nwscombatround.h"
#include "nwitem.h"
#include "nwsitem.h"
#include "nwitem.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "nwrules.h"
#include "nwcanimbase.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "serveraimaster.h"
#include "nwsarea.h"
#include "nwscreaturestats.h"
#include "partytables.h"
#include "nwmessage.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "pathfindinfo.h"
#include "nwfeat.h"
#include "nwccreature.h"
#include "guiingame.h"
#include "nwscript.h"
#include "serverexoappinternal.h"
#include "worldtimer.h"
#include "swparty.h"
#include "nwsdoor.h"
#include "nwsplaceable.h"
#include <stdlib.h>
#include <math.h>
#include "nwspell.h"

#define NWSCREATURE_ATTACK_SIGHT_HEIGHT 1.5f
#define NWSCREATURE_ATTACK_SIGHT_ENDPOINT_MARGIN 0.01f
#define NWSCREATURE_ATTACK_RANGE_TOLERANCE_SQUARED 0.2f
#define NWSCREATURE_ATTACK_MOVE_MESSAGE_DISTANCE_SQUARED 0.25f

int g_nCombatAttackRoll = 0;
int g_bDisableDamage = 0;
// PC 0x00603280
void CSWSCombatRound::SetCurrentCombatRoundAction(CSWSCombatRoundAction* pAction)
{
    m_pCurrentAction = pAction;
    if (pAction != NULL)
    {
        m_cB1C = pAction->m_nActionType;
    }
}
// PC 0x006032A0
int CSWSCreature::IsCreature()
{
    int bCreature = 0;
    CExoString sModelType;
    CExoString sColumn("MODELTYPE");
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pTable = p2DArrays->m_pAppearance;

    pTable->GetCExoStringEntry(m_cAppearanceInfo.m_nAppearance, sColumn, &sModelType);

    char cModelType = sModelType[0];
    if (cModelType == 'S' || cModelType == 'L' || cModelType == 's' ||
        cModelType == 'l')
    {
        bCreature = 1;
    }

    return bCreature;
}
// PC 0x00603360
int CSWSCreature::IsDebilitated(int bCheckMeditation)
{
    int bDebilitated = 0;

    if ((m_bDebilitated > 0 &&
         m_bDebilitated != CSWSEffectListHandler::STATE_CONFUSION &&
         m_bDebilitated != CSWSEffectListHandler::STATE_VARIANT_16) ||
        GetDeadTemp() != 0)
    {
        bDebilitated = 1;
    }

    if (bCheckMeditation != 0 && m_c110C == 1)
    {
        return 1;
    }

    return bDebilitated;
}
// PC 0x006033C0
int CSWSCreature::IsInTotalDefense()
{
    CSWSCombatRound* pCombatRound = m_pcCombatRound;
    if (m_c110C == 0 ||
        (pCombatRound != NULL && pCombatRound->m_cB1C == 13))
    {
        return 1;
    }
    return 0;
}
// PC 0x006033F0
void CSWSCreature::CancelAllCombatModes(int)
{
    SetLockOrientationToObject(OBJECT_INVALID, 0);
    ((CSWSObject*)this)->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
    SetCombatMode(0, 1);
    m_pcCombatRound->ClearAllSpecialAttacks();
}
// PC 0x00603430
int CSWSCreature::HasTargetBeenDetected(unsigned long oidTarget,
                                        int bCheckHidden)
{
    int bDetected = 1;
    CGameObject* pTarget =
        g_pAppManager->m_pServerExoApp->GetGameObject(oidTarget);
    if ((pTarget == NULL ||
         (pTarget->AsSWSDoor() == NULL && pTarget->AsSWSPlaceable() == NULL)) &&
        m_bInParty == 0)
    {
        CSWVisibilityNode* pNode = GetVisibleListElement(oidTarget);
        if (pNode == NULL ||
            (bCheckHidden != 0 &&
             pNode->m_nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_FAILED))
        {
            bDetected = 0;
        }
        else if (pNode->m_bInvisible != 0 ||
                 pNode->m_bSeen == 0)
        {
            bDetected = 0;
        }
    }
    return bDetected;
}
// PC 0x006034B0
void CSWSCreature::InitializeNewAttackTarget(unsigned long oidTarget,
                                             CSWSObjectActionNode* pAction)
{
    if (pAction != NULL)
    {
        pAction->m_apParam[0] = (void*)oidTarget;
    }

    SetLockOrientationToObject(oidTarget, 0);
    m_nUnknown84 = oidTarget;
    SetAttemptedAttackTarget(oidTarget);
    m_pcCombatRound->UpdateAttackTargetForAllActions(oidTarget);
    m_bNewAttackTarget = 1;
}
// PC 0x00603500
void CSWSCreature::ResolveSafeProjectile(int nAttack, int nProjectileType,
                                         int bUseAttackResult,
                                         int nWeaponHand)
{
    unsigned long nSlot = 0;
    if (nWeaponHand == 0)
    {
        nSlot = INVENTORY_SLOT_RIGHTHAND;
    }
    else if (nWeaponHand == 1)
    {
        nSlot = INVENTORY_SLOT_LEFTHAND;
    }

    CSWInventory* pInventory = m_pInventory;
    CSWSItem* pWeapon = pInventory->GetItemInSlot(nSlot);
    if (pWeapon != NULL)
    {
        CSWSCombatAttackData* pAttack = m_pcCombatRound->GetAttack(nAttack);
        pAttack->m_oidAmmoItem = pWeapon->m_nId;

        unsigned char nAttackResult;
        if (bUseAttackResult)
        {
            nAttackResult = pAttack->m_nAttackResult;
        }
        else
        {
            nAttackResult = 4;
        }

        pAttack->GetBaseDamage();
        unsigned char nAmmunitionType =
            pWeapon->GetBaseItem()->m_nAmmunitionType;

        int nAttackType = pAttack->m_nAttackType;
        unsigned char bSpecialAttack = 0;
        if (nAttackType == 18 || nAttackType == 29 || nAttackType == 82)
        {
            bSpecialAttack = 1;
        }

        ((CSWSObject*)this)
            ->BroadcastSafeProjectile(m_nId, pAttack->m_oidReactObject,
                                m_vPosition, pAttack->m_vRangedTarget,
                                nProjectileType, nAttackResult, nWeaponHand,
                                nAmmunitionType, bSpecialAttack,
                                pAttack->m_nMissedBy);
    }
}
// PC 0x00606E40
void CSWSCreature::ClearAttackerList()
{
    if (m_pAttackerList != NULL)
    {
        delete m_pAttackerList;
        m_pAttackerList = NULL;
        m_nNextAttacker = 0;
    }
}
// PC 0x00606E80
void CSWSCreature::SignalDamage(int bRanged, unsigned long oidTarget)
{
    CSWSCombatAttackData* pAttack =
        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
    if (oidTarget == g_pAppManager->GetServerExoApp()->GetPlayerCreatureId() &&
        pAttack->m_nAttackResult == CSWSCombatAttackData::ATTACK_RESULT_CRITICAL_HIT)
    {
        if (bRanged)
        {
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, pAttack->m_nReactionDelay, GetId(), oidTarget,
                CServerAIMaster::EVENT_CONTROLLER_RUMBLE,
                (void*)CServerAIMaster::CONTROLLER_RUMBLE_CRITICAL_HIT);
        }
        else
        {
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), oidTarget,
                CServerAIMaster::EVENT_CONTROLLER_RUMBLE,
                (void*)CServerAIMaster::CONTROLLER_RUMBLE_CRITICAL_HIT);
        }
    }

    if (!pAttack->m_bRangedAttack || pAttack->m_nUnknown4)
    {
        SendAttackSummaryFeedback(oidTarget);
        CSWCCMessageData* pMessage = new CSWCCMessageData;
        pMessage->SetInteger(CSWCCMessageData::ATTACK_TOTAL, pAttack->m_Extra1.m_nUnknown1 + (char)pAttack->m_Extra1.m_nUnknown2);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_ROLL, pAttack->m_Extra1.m_nUnknown1);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_BONUS, (char)pAttack->m_Extra1.m_nUnknown2);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER1, (char)pAttack->m_Extra1.m_nStrengthModifier);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER2, (char)pAttack->m_Extra1.m_nDexterityModifier);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_TYPE, pAttack->m_nAttackType);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER3, (char)pAttack->m_Extra1.m_nSpecialAttackBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_DETAIL, pAttack->m_Extra1.m_nOffHand);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER4, (char)pAttack->m_Extra1.m_nWeaponSizeBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER5, (char)pAttack->m_Extra1.m_nTwoWeaponFeatBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_DETAIL1, pAttack->m_Extra1.m_nTwoWeaponFeat);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER6, (char)pAttack->m_Extra1.m_nDuelingBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_DETAIL2, pAttack->m_Extra1.m_nDuelingFeat);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER7, (char)pAttack->m_Extra1.m_nGoodAimBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER8, (char)pAttack->m_Extra1.m_nOffensiveTrainingBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER9, (char)pAttack->m_Extra1.m_nWeaponFocusBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER10, (char)pAttack->m_Extra1.m_nEffectAttackBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_DETAIL3, pAttack->m_Extra1.m_nBaseAttackBonus);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_MODIFIER11, (char)pAttack->m_Extra1.m_nTwoWeaponPenalty);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_DEFENSE_ROLL, pAttack->m_Extra1.m_nUnknown18);
        pMessage->SetInteger(CSWCCMessageData::ATTACK_DEFENSE_MODIFIER, pAttack->m_Extra1.m_nUnknown1C);
        BroadcastAttackBreakdown(oidTarget, pMessage);
        delete pMessage;

        pMessage = new CSWCCMessageData;
        pMessage->SetInteger(CSWCCMessageData::THREAT_ATTACK_ROLL, pAttack->m_Extra1.m_nUnknown1);
        pMessage->SetInteger(CSWCCMessageData::THREAT_ROLL, pAttack->m_Extra2.m_nUnknown0);
        pMessage->SetInteger(CSWCCMessageData::THREAT_DETAIL, pAttack->m_Extra2.m_nUnknown4);
        pMessage->SetInteger(CSWCCMessageData::THREAT_TOTAL, (char)pAttack->m_Extra1.m_nUnknown2 + pAttack->m_Extra2.m_nUnknown8);
        pMessage->SetInteger(CSWCCMessageData::THREAT_DEFENSE, (char)pAttack->m_Flags.m_bUnknown0);
        pMessage->SetInteger(CSWCCMessageData::THREAT_DETAIL1, pAttack->m_Extra2.m_nUnknown10);
        pMessage->SetInteger(CSWCCMessageData::THREAT_MODIFIER, pAttack->m_Extra2.m_nUnknownC);
        BroadcastThreatBreakdown(oidTarget, pMessage);
        delete pMessage;

        pMessage = new CSWCCMessageData;
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER0, (char)pAttack->m_Flags.m_bUnknown0);
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER1, (char)pAttack->m_Flags.m_nArmorModifier);
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER2, (char)pAttack->m_Flags.m_nDexterityModifier);
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER3, (char)pAttack->m_Flags.m_nBaseDexterityModifier);
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER4, (char)pAttack->m_Flags.m_nClassArmorModifier);
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER5, (char)pAttack->m_Flags.m_nNaturalArmorModifier);
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER6, (char)pAttack->m_Flags.m_nDodgeDeflectionModifier);
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER7, (char)pAttack->m_Flags.m_nFeatArmorModifier);
        pMessage->SetInteger(CSWCCMessageData::DEFENSE_MODIFIER8, (char)pAttack->m_Flags.m_nDebilitatedModifier);
        BroadcastDefenseBreakdown(oidTarget, pMessage);
        delete pMessage;

        pMessage = new CSWCCMessageData;
        int nIndex;
        for (nIndex = 0; nIndex < CSWSCombatAttackData::DAMAGE_COUNT; ++nIndex)
        {
            pMessage->SetInteger(nIndex, pAttack->m_nDamage[nIndex]);
        }
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nBaseDamage);
        pMessage->SetInteger(nIndex++, (char)pAttack->m_DamageBreakdown.m_nStrengthDamage);
        pMessage->SetInteger(nIndex++, pAttack->m_nAttackType);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nDamageBonus);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nUnusedDamageFeedback);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nSneakAttackDamage);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nWeaponSpecializationDamage);
        pMessage->SetInteger(nIndex++, (char)pAttack->m_Extra2.m_nUnknown11);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nUnknown6);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nUnknown7);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nUnknown8);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nUnknown9);
        pMessage->SetInteger(nIndex++, pAttack->m_DamageBreakdown.m_nUnknownA);
        BroadcastDamageBreakdown(oidTarget, pMessage);
        delete pMessage;

        if (pAttack->m_Extra4.m_nUnknown14)
        {
            pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(CSWCCMessageData::BREAKDOWN_REACT_OBJECT, pAttack->m_oidReactObject);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_SAVE_TYPE, pAttack->m_Extra4.m_nSavingThrowType);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_STATE, pAttack->m_Extra4.m_nState);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_VERSUS_TYPE, pAttack->m_Extra4.m_nSaveVersusType);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_SAVE_ROLL, pAttack->m_Extra4.m_nSavingThrowRoll);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_SAVE_EFFECT_BONUS, pAttack->m_Extra4.m_nSavingThrowEffectBonus);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_SAVE_BONUS, pAttack->m_Extra4.m_nSavingThrowBonus);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_SAVE_TOTAL, pAttack->m_Extra4.m_nSavingThrowTotal);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_SAVE_DC, pAttack->m_Extra4.m_nSavingThrowDC);
            pMessage->SetInteger(CSWCCMessageData::EFFECT_SAVE_RESULT, (char)pAttack->m_Extra4.m_nSavingThrowResult);
            BroadcastEffectApplicationBreakdown(oidTarget, pMessage);
            delete pMessage;
        }
        if (IsAttackResultParried(pAttack->m_nAttackResult))
        {
            pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(CSWCCMessageData::BREAKDOWN_REACT_OBJECT, pAttack->m_oidReactObject);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL0, pAttack->m_Extra5.m_nUnknown5);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL1, pAttack->m_Extra5.m_nUnknown1);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL2, pAttack->m_Extra5.m_nUnknown8);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL3, pAttack->m_Extra5.m_nUnknown0);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL4, pAttack->m_Extra5.m_nUnknown3);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL5, pAttack->m_Extra5.m_nUnknown2);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL6, pAttack->m_Extra5.m_nUnknown6);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL7, pAttack->m_Extra5.m_nUnknown7);
            pMessage->SetInteger(CSWCCMessageData::DEFLECT_DETAIL8, pAttack->m_Extra5.m_nUnknown4);
            BroadcastDeflectBreakdown(oidTarget, pMessage);
            delete pMessage;
        }
    }
    if (bRanged)
    {
        SignalRangedDamage(oidTarget);
    }
    else
    {
        SignalMeleeDamage(oidTarget, 1);
    }
}
// PC 0x006077B0
void CSWSCreature::ResolveSneakAttack(CSWSCreature* pTarget)
{
    if (pTarget == NULL)
        return;
    int nAttack = m_pcCombatRound->m_nCurrentAttack;
    CSWSCreatureStats* pStats = m_pStats;
    int bRanged = m_pcCombatRound->GetAttack(nAttack)->m_bRangedAttack;
    if (!pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_1D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_2D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_3D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_4D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_5D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_6D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_7D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_8D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_9D6) &&
        !pStats->HasFeat(CSWFeat::FEAT_SNEAK_ATTACK_10D6))
        return;
    float fDistance;
    if (bRanged != 0)
        fDistance = magnitude(m_vPosition - pTarget->m_vPosition);
    int bSneakAttack = FALSE;
    if (pTarget->IsDebilitated())
    {
        bSneakAttack = TRUE;
    }
    else
    {
        CSWVisibilityNode* pVisibility = pTarget->GetVisibleListElement(GetId());
        if ((pVisibility == NULL || !pVisibility->m_bSeen) &&
            ((bRanged == 1 && fDistance <= NWSCREATURE_SNEAK_ATTACK_RANGE) ||
             bRanged == 0))
        {
            bSneakAttack = TRUE;
        }
        else
        {
            Vector vDirection;
            vDirection = normalize(m_vPosition - pTarget->m_vPosition);
            if (vDirection * normalize(pTarget->m_vOrientation) <
                    NWSCREATURE_SNEAK_ATTACK_BACK_ARC &&
                (bRanged == 0 || fDistance <= NWSCREATURE_SNEAK_ATTACK_RANGE))
            {
                m_pcCombatRound->GetAttack(nAttack)->m_bSneakAttack = 1;
            }
        }
    }
    if (bSneakAttack)
    {
        if (!pTarget->m_pStats->GetEffectImmunity(
                CSWSEffectListHandler::IMMUNITY_TYPE_BACKSTAB, this) &&
            !pTarget->m_pStats->GetEffectImmunity(
                CSWSEffectListHandler::IMMUNITY_TYPE_CRITICAL_HITS, this))
        {
            m_pcCombatRound->GetAttack(nAttack)->m_bSneakAttack = 1;
        }
        else
        {
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, pTarget->GetId());
            pMessage->SetInteger(0, FEEDBACK_SNEAK_ATTACK_IMMUNITY);
            CSWSCombatRound* pCombatRound = m_pcCombatRound;
            pCombatRound->GetAttack(nAttack)->m_lstFeedbackMessages.Add(pMessage);
        }
    }
}
// PC 0x00607AD0
void CSWSCreature::ResolvePostMeleeDamage(CSWSObject* pTarget)
{
    if (pTarget != NULL)
    {
        CSWSCombatAttackData* pAttack;
        int nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
        pAttack = m_pcCombatRound->GetAttack(nCurrentAttack);
        int nDamage = pAttack->GetTotalDamage(1);
        if (nDamage >= pTarget->GetCurrentHitPoints(0))
        {
            pAttack->m_bKillingBlow = 1;
        }

        if (pTarget->AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = pTarget->AsSWSCreature();
            if (pAttack->m_bCoupDeGrace)
            {
                CGameEffect* pDeathEffect = new CGameEffect(1);
                unsigned short nSubType = pDeathEffect->m_nSubType;
                pDeathEffect->m_nType = CSWSEffectListHandler::EFFECT_DEATH;
                pDeathEffect->m_nSubType =
                    (nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                pDeathEffect->SetCreator(m_nId);
                pDeathEffect->SetInteger(0, 0);
                pDeathEffect->SetInteger(1, 1);
                pAttack->m_lstEffects.Add(pDeathEffect);
            }
        }
    }
}
// PC 0x00607BF0
void CSWSCreature::ResolvePostRangedDamage(CSWSObject* pTarget)
{
    if (pTarget != NULL)
    {
        CSWSCombatAttackData* pAttack;
        int nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
        pAttack = m_pcCombatRound->GetAttack(nCurrentAttack);
        int nDamage = pAttack->GetTotalDamage(1);
        int bOffHand =
            pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_LEFTHAND;

        if (pTarget->AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = pTarget->AsSWSCreature();
            if (nDamage >= pTarget->AsSWSCreature()->GetCurrentHitPoints(0) ||
                pAttack->m_bCoupDeGrace == 1)
            {
                pAttack->m_bKillingBlow = 1;
            }

            Vector vDifference = m_vPosition - pTarget->m_vPosition;
            if (pAttack->m_bCoupDeGrace == 1 &&
                vDifference * vDifference <= RANGED_COUP_DE_GRACE_RANGE_SQUARED)
            {
                CGameEffect* pDeathEffect = new CGameEffect(1);
                pDeathEffect->m_nType = CSWSEffectListHandler::EFFECT_DEATH;
                pDeathEffect->m_nSubType =
                    (pDeathEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                pDeathEffect->SetCreator(GetId());
                pDeathEffect->SetInteger(0, 0);
                pDeathEffect->SetInteger(1, 1);
                pAttack->m_lstEffects.Add(pDeathEffect);
            }
        }
        else if (nDamage >= pTarget->GetCurrentHitPoints(0))
        {
            pAttack->m_bKillingBlow = 1;
        }

        if (nDamage <= 0)
        {
            CSWSCombatRound* pCombatRound = m_pcCombatRound;
            if (!pCombatRound->m_bWeaponSucks &&
                !GetIsWeaponEffective(pTarget->GetId(), bOffHand))
            {
                CSWCCMessageData* pMessage = new CSWCCMessageData;
                pMessage->m_nType = CSWCCMessageData::DATA_TYPE_FEEDBACK;
                pMessage->SetInteger(CSWCCMessageData::FEEDBACK_MESSAGE,
                                     FEEDBACK_WEAPON_INEFFECTIVE);
                pAttack->m_lstFeedbackMessages.Add(pMessage);
                m_pcCombatRound->m_bWeaponSucks = 1;
            }
        }
    }
}
enum
{
    SPELL_DEFLECTION_BONUS = 163,
    DEFLECTION_SPELL_BONUS = 3,
    FEAT_CLASS_DEFLECTION_BONUS = 168,
    DEFLECTION_BONUS_CLASS = 11,
    DEFLECTION_CLASS_SLOTS = 2,
    FORM_SABER_II_MAKASHI = 259,
    FORM_SABER_III_DEFLECTION = 260,
    FORM_SABER_IV_DEFLECTION = 261,
    FORM_SABER_V_SHIEN = 262,
    FORM_SABER_VI_NIMAN = 263,
    FEAT_DEFLECTION_PENALTY_1 = 240,
    FEAT_DEFLECTION_PENALTY_2 = 241,
    FEAT_DEFLECTION_PENALTY_3 = 242,
    FEAT_DEFLECTION_PENALTY_4 = 243,
    FEAT_DEFLECTION_PENALTY_5 = 244
};
// PC 0x00607E20
int CSWSCreature::ResolveDefensiveEffects(CSWSObject* pTarget, int bRangedAttack)
{
    int bResult = 0;
    CSWSCombatAttackData* pAttack =
        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
    int nShield = 0;
    int bCanReflect = 1;
    if (IsProjectileParryable(pTarget, bRangedAttack, 0, nShield, bCanReflect, -1) &&
        pTarget->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pTarget->AsSWSCreature();
        unsigned char nResult;
        if (pCreature->m_bHasAssuredDeflection)
        {
            bResult = 1;
            if (pCreature->m_bAssuredDeflection)
            {
                nResult = CSWSCombatAttackData::ATTACK_RESULT_REFLECTED;
            }
            else
            {
                nResult = CSWSCombatAttackData::ATTACK_RESULT_DEFLECTED;
                bResult = 1;
            }
        }
        else
        {
            int nRoll = (unsigned short)g_pRules->RollDice(1, 20);
            pAttack->m_Extra5.m_nUnknown0 = nRoll;
            if (pCreature->m_pStats->HasFeat(CSWFeat::FEAT_MASTER_JEDI_DEFENSE))
            {
                nRoll += 6;
                pAttack->m_Extra5.m_nUnknown2 = CSWFeat::FEAT_MASTER_JEDI_DEFENSE;
                pAttack->m_Extra5.m_nUnknown3 = 6;
            }
            else if (pCreature->m_pStats->HasFeat(CSWFeat::FEAT_ADVANCED_JEDI_DEFENSE))
            {
                nRoll += 3;
                pAttack->m_Extra5.m_nUnknown2 = CSWFeat::FEAT_ADVANCED_JEDI_DEFENSE;
                pAttack->m_Extra5.m_nUnknown3 = 3;
            }
            if (pCreature->m_pStats->HasSpell(0, SPELL_DEFLECTION_BONUS, 0))
            {
                nRoll += DEFLECTION_SPELL_BONUS;
                pAttack->m_Extra5.m_nUnknown15 = SPELL_DEFLECTION_BONUS;
                pAttack->m_Extra5.m_nUnknown14 = DEFLECTION_SPELL_BONUS;
            }
            if (pCreature->m_pStats->HasFeat(FEAT_CLASS_DEFLECTION_BONUS) &&
                pCreature->IsWieldingLightsaber())
            {
                for (unsigned char nClass = 0; nClass < DEFLECTION_CLASS_SLOTS; nClass++)
                {
                    if (pCreature->m_pStats->GetClass(nClass) == DEFLECTION_BONUS_CLASS)
                    {
                        unsigned char nClassBonus =
                            (pCreature->m_pStats->GetClassLevel(nClass, 0) + 1) / 2;
                        nRoll += nClassBonus;
                        pAttack->m_Extra5.m_nUnknown20 = FEAT_CLASS_DEFLECTION_BONUS;
                        pAttack->m_Extra5.m_nUnknown21 = nClassBonus;
                        break;
                    }
                }
            }
            if (pCreature->IsWieldingLightsaber())
            {
                int nForm = pCreature->m_n1110;
                switch (nForm)
                {
                case FORM_SABER_II_MAKASHI:
                    nRoll -= 5;
                    pAttack->m_Extra5.m_nUnknown18 = -5;
                    pAttack->m_Extra5.m_nUnknown1C = nForm;
                    break;
                case FORM_SABER_III_DEFLECTION:
                    nRoll += 4;
                    pAttack->m_Extra5.m_nUnknown18 = 4;
                    pAttack->m_Extra5.m_nUnknown1C = nForm;
                    break;
                case FORM_SABER_IV_DEFLECTION:
                    nRoll -= 4;
                    pAttack->m_Extra5.m_nUnknown18 = -4;
                    pAttack->m_Extra5.m_nUnknown1C = nForm;
                    break;
                case FORM_SABER_V_SHIEN:
                    nRoll += 2;
                    pAttack->m_Extra5.m_nUnknown18 = 2;
                    pAttack->m_Extra5.m_nUnknown1C = nForm;
                    break;
                case FORM_SABER_VI_NIMAN:
                    nRoll += 1;
                    pAttack->m_Extra5.m_nUnknown18 = 1;
                    pAttack->m_Extra5.m_nUnknown1C = nForm;
                    break;
                }
            }
            if (m_pStats->HasFeat(FEAT_DEFLECTION_PENALTY_5))
            {
                pAttack->m_Extra5.m_nUnknown22 = FEAT_DEFLECTION_PENALTY_5;
                pAttack->m_Extra5.m_nUnknown23 = -10;
                nRoll -= 10;
            }
            else if (m_pStats->HasFeat(FEAT_DEFLECTION_PENALTY_4))
            {
                pAttack->m_Extra5.m_nUnknown22 = FEAT_DEFLECTION_PENALTY_4;
                pAttack->m_Extra5.m_nUnknown23 = -8;
                nRoll -= 8;
            }
            else if (m_pStats->HasFeat(FEAT_DEFLECTION_PENALTY_3))
            {
                pAttack->m_Extra5.m_nUnknown22 = FEAT_DEFLECTION_PENALTY_3;
                pAttack->m_Extra5.m_nUnknown23 = -6;
                nRoll -= 6;
            }
            else if (m_pStats->HasFeat(FEAT_DEFLECTION_PENALTY_2))
            {
                pAttack->m_Extra5.m_nUnknown22 = FEAT_DEFLECTION_PENALTY_2;
                pAttack->m_Extra5.m_nUnknown23 = -4;
                nRoll -= 4;
            }
            else if (m_pStats->HasFeat(FEAT_DEFLECTION_PENALTY_1))
            {
                pAttack->m_Extra5.m_nUnknown22 = FEAT_DEFLECTION_PENALTY_1;
                pAttack->m_Extra5.m_nUnknown23 = -2;
                nRoll -= 2;
            }
            nRoll += pCreature->m_pStats->GetBaseAttackBonus();
            pAttack->m_Extra5.m_nUnknown6 = pCreature->m_pStats->GetBaseAttackBonus();
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            nRoll += (char)(pStats->m_nArmorCheckPenalty + pStats->m_nShieldCheckPenalty);
            pAttack->m_Extra5.m_nUnknown7 = pCreature->m_pStats->m_nArmorCheckPenalty +
                                      pCreature->m_pStats->m_nShieldCheckPenalty;
            CExoArrayList<CGameEffect*>* pEffects = &pCreature->m_lstEffects;
            if (pEffects != NULL)
            {
                int nEffectBonus = 0;
                for (int i = 0; i < pEffects->num; ++i)
                {
                    CGameEffect* pEffect = (*pEffects)[i];
                    if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONINCREASE ||
                        pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONDECREASE)
                        nEffectBonus += pEffect->GetInteger(1);
                }
                nRoll += nEffectBonus;
                pAttack->m_Extra5.m_nUnknown4 = nEffectBonus;
            }
            int nAttackRoll = pAttack->m_Extra1.m_nUnknown1 + (char)pAttack->m_Extra1.m_nUnknown2;
            pAttack->m_Extra5.m_nUnknown8 = nRoll;
            pAttack->m_Extra5.m_nUnknown1 = nAttackRoll;
            if (nRoll >= nAttackRoll)
            {
                bResult = 1;
                if (((nRoll >= nAttackRoll + 6 && bRangedAttack) || nRoll >= nAttackRoll + 6) &&
                    bCanReflect)
                    nResult = CSWSCombatAttackData::ATTACK_RESULT_REFLECTED;
                else
                    nResult = CSWSCombatAttackData::ATTACK_RESULT_DEFLECTED;
            }
        }
        if (bResult)
        {
            pAttack->m_nAttackResult = nResult;
            pAttack->m_Extra5.m_nUnknown5 = nResult;
        }
    }
    return bResult;
}
// PC 0x00608280
void CSWSCreature::ResolveOnHitVisuals(CSWSObject* pTarget)
{
    CSWSCombatAttackData* pAttack =
        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);

    for (int nDamageType = 0;
         nDamageType < CSWSCombatAttackData::BASE_DAMAGE_INDEX; nDamageType++)
    {
        if (nDamageType != CSWSCombatAttackData::DAMAGE_INDEX_PIERCING &&
            nDamageType != CSWSCombatAttackData::DAMAGE_INDEX_SLASHING &&
            nDamageType != CSWSCombatAttackData::DAMAGE_INDEX_BLUDGEONING)
        {
            unsigned short nDamageFlag = 1 << nDamageType;
            if (pAttack->GetDamage(nDamageFlag) > 0)
            {
                CExoString sColumn;
                int nVisualEffect = 0;
                if (pAttack->m_bRangedAttack)
                {
                    sColumn = "RangedEffect";
                }
                else
                {
                    sColumn = "VisualEffect";
                }
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                EXOXBOXASSERT(p2DArrays->m_pDamageHitVisual);
                C2DA* pTable = p2DArrays->m_pDamageHitVisual;
                pTable->GetINTEntry(nDamageType, sColumn, &nVisualEffect);

                CGameEffect* pEffect = new CGameEffect(1);
                pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                pEffect->SetCreator(GetId());
                pEffect->SetInteger(0, nVisualEffect);
                pAttack->m_lstEffects.Add(pEffect);
            }
        }
    }

    if (IsAttackResultParried(pAttack->m_nAttackResult))
    {
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pEffect->SetCreator(pTarget->m_nId);
        pEffect->SetInteger(0, CSWSEffectListHandler::VFX_COM_BLASTER_DEFLECTION);
        pAttack->m_lstEffects.Add(pEffect);
    }
}
enum
{
    FEAT_LOW_HEALTH_SAVE_BONUS = 199,
    SAVING_THROW_ALIGNMENT_LIGHT = 60,
    SAVING_THROW_ALIGNMENT_DARK = 40,
    SAVING_THROW_MAX_EFFECT_BONUS = 60
};
// PC 0x006084F0
unsigned char CSWSCreature::SavingThrowRoll(unsigned char nSaveType, unsigned short nDC,
                                            unsigned char nSaveVersusType,
                                            unsigned long oidSaveVersus,
                                            int bSendFeedback, unsigned short nFeat,
                                            int bFromCombat)
{
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CSWSCreature* pSaveVersus = NULL;
    int nSaveBonus;

    switch (nSaveType)
    {
    case SAVING_THROW_FORT:
        nSaveBonus = m_pStats->GetFortSavingThrow(1);
        break;
    case SAVING_THROW_REFLEX:
        nSaveBonus = m_pStats->GetReflexSavingThrow(1);
        break;
    case SAVING_THROW_WILL:
        nSaveBonus = m_pStats->GetWillSavingThrow(1);
        break;
    default:
        return SAVING_THROW_RESULT_FAILED;
    }

    CGameObject* pObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidSaveVersus);
    int nCapped = 0;
    int nEffectBonus;
    if (pObject != NULL)
    {
        nEffectBonus = GetTotalEffectBonus(EFFECT_BONUS_SAVING_THROW,
                                           pObject->AsSWSObject(), nCapped, 0, 0,
                                           nSaveType, nSaveVersusType,
                                           EFFECT_BONUS_FILTER_ALL,
                                           EFFECT_BONUS_FILTER_ALL, 0);
        if (pObject->AsSWSCreature() != NULL)
        {
            pSaveVersus = pObject->AsSWSCreature();
        }
    }
    else
    {
        nEffectBonus = GetTotalEffectBonus(EFFECT_BONUS_SAVING_THROW, NULL,
                                           nCapped, 0, 0, nSaveType,
                                           nSaveVersusType,
                                           EFFECT_BONUS_FILTER_ALL,
                                           EFFECT_BONUS_FILTER_ALL, 0);
    }

    CSWSArea* pArea = GetArea();
    int nRoomForceRating = 0;
    if (pArea != NULL)
        nRoomForceRating = pArea->GetRoomForceRating(m_vPosition);
    if (m_pStats->m_nGoodEvilAlignment > SAVING_THROW_ALIGNMENT_DARK &&
        m_pStats->m_nGoodEvilAlignment >= SAVING_THROW_ALIGNMENT_LIGHT)
    {
        nEffectBonus += nRoomForceRating;
    }
    else if (m_pStats->m_nGoodEvilAlignment <= SAVING_THROW_ALIGNMENT_DARK)
    {
        nEffectBonus -= nRoomForceRating;
    }

    if (m_pStats->HasFeat(FEAT_LOW_HEALTH_SAVE_BONUS))
    {
        float fCurrentHitPoints = GetCurrentHitPoints(0);
        float fPercent = fCurrentHitPoints / GetMaxHitPoints(1) * 100.0f;
        if (fPercent < 50.0f && fPercent >= 40.0f)
        {
            nEffectBonus++;
        }
        else if (fPercent < 40.0f && fPercent >= 30.0f)
        {
            nEffectBonus += 2;
        }
        else if (fPercent < 30.0f && fPercent >= 20.0f)
        {
            nEffectBonus += 3;
        }
        else if (fPercent < 20.0f && fPercent >= 10.0f)
        {
            nEffectBonus += 4;
        }
        else if (fPercent < 10.0f && fPercent >= 0.0f)
        {
            nEffectBonus += 5;
        }
    }

    if (nEffectBonus > SAVING_THROW_MAX_EFFECT_BONUS)
    {
        nEffectBonus = SAVING_THROW_MAX_EFFECT_BONUS;
    }

    int nTotal;
    int nRoll = nTotal = g_pRules->RollDice(1, 20);

    CSWCCMessageData* pData = new CSWCCMessageData;
    pData->m_nType = CSWCCMessageData::DATA_TYPE_SAVING_THROW;
    pData->SetInteger(CSWCCMessageData::SAVING_THROW_SAVE_TYPE, nSaveType);
    pData->SetInteger(CSWCCMessageData::SAVING_THROW_VERSUS_TYPE, nSaveVersusType);
    pData->SetInteger(CSWCCMessageData::SAVING_THROW_ROLL, nRoll);
    pData->SetInteger(CSWCCMessageData::SAVING_THROW_EFFECT_BONUS, nEffectBonus);
    pData->SetInteger(CSWCCMessageData::SAVING_THROW_BASE_BONUS, nSaveBonus);
    pData->SetInteger(CSWCCMessageData::SAVING_THROW_DC, nDC);
    pData->SetInteger(CSWCCMessageData::SAVING_THROW_CREATURE, GetId());

    if (!bFromCombat)
    {
        if (pServer->GetClientObjectByObjectId(GetId()) != NULL)
        {
            BroadcastSavingThrowData(pData);
        }
        if (GetId() != oidSaveVersus && pSaveVersus != NULL)
        {
            pSaveVersus->BroadcastSavingThrowData(pData);
        }
        delete pData;
    }
    else
    {
        CSWSCombatRound* pCombatRound = pSaveVersus->m_pcCombatRound;
        pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack)
            ->m_lstFeedbackMessages.Add(pData);
    }

    nTotal += nEffectBonus + nSaveBonus;
    CSWSCombatAttackData* pAttack = NULL;
    if (pSaveVersus != NULL)
    {
        CSWSCombatRound* pCombatRound = pSaveVersus->m_pcCombatRound;
        pAttack = pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack);
        pAttack->m_Extra4.m_nSavingThrowType = nSaveType;
        pAttack->m_Extra4.m_nSaveVersusType = nSaveVersusType;
        pAttack->m_Extra4.m_nSavingThrowRoll = (unsigned char)nRoll;
        pAttack->m_Extra4.m_nSavingThrowEffectBonus = (unsigned char)nEffectBonus;
        pAttack->m_Extra4.m_nSavingThrowBonus = (unsigned char)nSaveBonus;
        pAttack->m_Extra4.m_nSavingThrowTotal = (unsigned char)nTotal;
        pAttack->m_Extra4.m_nSavingThrowDC = (unsigned char)nDC;
    }

    if (nTotal >= nDC)
    {
        if (pAttack != NULL)
        {
            pAttack->m_Extra4.m_nSavingThrowResult = SAVING_THROW_RESULT_SUCCESS;
        }
        return SAVING_THROW_RESULT_SUCCESS;
    }

    if (nSaveVersusType == SAVING_THROW_TYPE_MIND_AFFECTING)
    {
        if (GetStats()->GetEffectImmunity(
                CSWSEffectListHandler::IMMUNITY_TYPE_MIND_AFFECTING, pSaveVersus))
        {
            if (pAttack != NULL)
            {
                pAttack->m_Extra4.m_nSavingThrowResult = SAVING_THROW_RESULT_IMMUNE;
            }
            return SAVING_THROW_RESULT_IMMUNE;
        }
    }
    else if (nSaveVersusType == SAVING_THROW_TYPE_POISON)
    {
        if (GetStats()->GetEffectImmunity(
                CSWSEffectListHandler::IMMUNITY_TYPE_POISON, pSaveVersus))
        {
            if (pAttack != NULL)
            {
                pAttack->m_Extra4.m_nSavingThrowResult = SAVING_THROW_RESULT_IMMUNE;
            }
            return SAVING_THROW_RESULT_IMMUNE;
        }
    }
    else if (nSaveVersusType == SAVING_THROW_TYPE_DISEASE)
    {
        if (GetStats()->GetEffectImmunity(
                CSWSEffectListHandler::IMMUNITY_TYPE_DISEASE, pSaveVersus))
        {
            if (pAttack != NULL)
            {
                pAttack->m_Extra4.m_nSavingThrowResult = SAVING_THROW_RESULT_IMMUNE;
            }
            return SAVING_THROW_RESULT_IMMUNE;
        }
    }
    else if (nSaveVersusType == SAVING_THROW_TYPE_FEAR)
    {
        if (GetStats()->GetEffectImmunity(
                CSWSEffectListHandler::IMMUNITY_TYPE_FEAR, pSaveVersus))
        {
            if (pAttack != NULL)
            {
                pAttack->m_Extra4.m_nSavingThrowResult = SAVING_THROW_RESULT_IMMUNE;
            }
            return SAVING_THROW_RESULT_IMMUNE;
        }
    }
    else if (nSaveVersusType == SAVING_THROW_TYPE_DEATH)
    {
        if (m_nSavingThrowSpellId != -1)
        {
            CSWSpell* pSpell =
                g_pRules->m_pSpellArray->GetSpell(m_nSavingThrowSpellId);
            if (pSpell != NULL &&
                (pSpell->m_nUserType == CSWSpell::USER_TYPE_FORCE_POWER ||
                 pSpell->m_nUserType == 2))
            {
                if (GetStats()->GetEffectImmunity(
                        CSWSEffectListHandler::IMMUNITY_TYPE_DEATH_MAGIC, pSaveVersus))
                {
                    if (pAttack != NULL)
                    {
                        pAttack->m_Extra4.m_nSavingThrowResult = SAVING_THROW_RESULT_IMMUNE;
                    }
                    return SAVING_THROW_RESULT_IMMUNE;
                }
            }
        }
    }
    else if (nSaveVersusType == SAVING_THROW_TYPE_TRAP)
    {
        if (GetStats()->GetEffectImmunity(
                CSWSEffectListHandler::IMMUNITY_TYPE_MINE, pSaveVersus))
        {
            if (pAttack != NULL)
            {
                pAttack->m_Extra4.m_nSavingThrowResult = SAVING_THROW_RESULT_IMMUNE;
            }
            return SAVING_THROW_RESULT_IMMUNE;
        }
    }

    if (pAttack != NULL)
    {
        pAttack->m_Extra4.m_nSavingThrowResult = SAVING_THROW_RESULT_FAILED;
    }
    return SAVING_THROW_RESULT_FAILED;
}
// PC 0x00608A10
void CSWSCreature::ApplyOnHitConfusion(CSWSObject* pObject, int nDuration)
{
    CGameEffect* pEffect =
        CreateLinkedEffect(CSWSEffectListHandler::EFFECT_SETSTATE,
                           CSWSEffectListHandler::STATE_CONFUSION,
                           CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                           CSWSEffectListHandler::VFX_CONFUSION_PRIMARY);
    if (pEffect != NULL)
    {
        CGameEffect* pStateEffect = pEffect;
        pEffect =
            CreateLinkedEffect(pEffect,
                               CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                               CSWSEffectListHandler::VFX_ON_HIT_STATE);
        if (pEffect == NULL)
        {
            delete pStateEffect;
        }
        else
        {
            CGameEffect* pVisualEffect = pEffect;
            pEffect = CreateLinkedEffect(pEffect,
                                        CSWSEffectListHandler::EFFECT_EFFECT_ICON,
                                        CSWSEffectListHandler::ICON_CONFUSION);
            if (pEffect == NULL)
            {
                delete pVisualEffect;
            }
            else
            {
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_TEMPORARY;
                pEffect->m_fDuration = nDuration * (SW_COMBAT_ROUND_SECONDS / 2.0f);
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
                    SUBTYPE_MAGICAL;
                pEffect->UpdateLinked();
                unsigned char nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);
            }
        }
    }
}
// PC 0x00608B10
void CSWSCreature::ApplyOnHitFear(CSWSObject* pObject, int nDuration)
{
    CGameEffect* pEffect =
        CreateLinkedEffect(CSWSEffectListHandler::EFFECT_SETSTATE,
                           CSWSEffectListHandler::STATE_FEAR,
                           CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                           CSWSEffectListHandler::VFX_FEAR_PRIMARY);
    if (pEffect != NULL)
    {
        CGameEffect* pStateEffect = pEffect;
        pEffect =
            CreateLinkedEffect(pEffect,
                               CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                               CSWSEffectListHandler::VFX_ON_HIT_STATE);
        if (pEffect == NULL)
        {
            delete pStateEffect;
        }
        else
        {
            CGameEffect* pVisualEffect = pEffect;
            pEffect = CreateLinkedEffect(pEffect,
                                        CSWSEffectListHandler::EFFECT_EFFECT_ICON,
                                        CSWSEffectListHandler::ICON_FEAR);
            if (pEffect == NULL)
            {
                delete pVisualEffect;
            }
            else
            {
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_TEMPORARY;
                pEffect->m_fDuration = nDuration * (SW_COMBAT_ROUND_SECONDS / 2.0f);
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
                    SUBTYPE_MAGICAL;
                pEffect->UpdateLinked();
                unsigned char nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);
            }
        }
    }
}
// PC 0x00608C10
void CSWSCreature::ApplyOnHitHold(CSWSObject* pObject, int nDuration)
{
    CGameEffect* pEffect =
        CreateLinkedEffect(CSWSEffectListHandler::EFFECT_SETSTATE,
                           CSWSEffectListHandler::STATE_HOLD,
                           CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                           CSWSEffectListHandler::VFX_HOLD_PRIMARY);
    if (pEffect != NULL)
    {
        CGameEffect* pStateEffect = pEffect;
        pEffect =
            CreateLinkedEffect(pEffect,
                               CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                               CSWSEffectListHandler::VFX_HOLD_SECONDARY);
        if (pEffect == NULL)
        {
            delete pStateEffect;
        }
        else
        {
            pStateEffect = pEffect;
            pEffect = CreateLinkedEffect(pEffect,
                                        CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                                        CSWSEffectListHandler::VFX_ON_HIT_STATE);
            if (pEffect == NULL)
            {
                delete pStateEffect;
            }
            else
            {
                CGameEffect* pVisualEffect = pEffect;
                pEffect = CreateLinkedEffect(pEffect,
                                            CSWSEffectListHandler::EFFECT_EFFECT_ICON,
                                            CSWSEffectListHandler::ICON_HOLD);
                if (pEffect == NULL)
                {
                    delete pVisualEffect;
                }
                else
                {
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pEffect->m_fDuration = nDuration * (SW_COMBAT_ROUND_SECONDS / 2.0f);
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
                        SUBTYPE_MAGICAL;
                    pEffect->UpdateLinked();
                    unsigned char nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                    m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);
                }
            }
        }
    }
}
// PC 0x00608D20
void CSWSCreature::ApplyOnHitSleep(CSWSObject* pObject, int nDuration)
{
    CGameEffect* pEffect = new CGameEffect(1);
    if (pEffect != NULL)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
        pEffect->SetInteger(0, CSWSEffectListHandler::VFX_SLEEP_IMPACT);
        pEffect->SetCreator(m_nId);
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        pAttack->m_lstEffects.Add(pEffect);

        pEffect =
            CreateLinkedEffect(CSWSEffectListHandler::EFFECT_SETSTATE,
                               CSWSEffectListHandler::STATE_SLEEP,
                               CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                               CSWSEffectListHandler::VFX_SLEEP_PRIMARY);
        if (pEffect != NULL)
        {
            CGameEffect* pStateEffect = pEffect;
            pEffect =
                CreateLinkedEffect(pEffect,
                                   CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                                   CSWSEffectListHandler::VFX_ON_HIT_STATE);
            if (pEffect == NULL)
            {
                delete pStateEffect;
            }
            else
            {
                CGameEffect* pVisualEffect = pEffect;
                pEffect = CreateLinkedEffect(pEffect,
                                            CSWSEffectListHandler::EFFECT_EFFECT_ICON,
                                            CSWSEffectListHandler::ICON_SLEEP);
                if (pEffect == NULL)
                {
                    delete pVisualEffect;
                }
                else
                {
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pEffect->m_fDuration = nDuration * (SW_COMBAT_ROUND_SECONDS / 2.0f);
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
                        SUBTYPE_MAGICAL;
                    pEffect->UpdateLinked();
                    unsigned char nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                    m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);
                }
            }
        }
    }
}
// PC 0x00608F20
void CSWSCreature::ApplyOnHitSlow(CSWSObject* pObject, int nDuration)
{
    CGameEffect* pEffect = new CGameEffect(1);
    if (pEffect != NULL)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
        pEffect->SetInteger(0, CSWSEffectListHandler::VFX_SLOW_IMPACT);
        pEffect->SetCreator(m_nId);
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        pAttack->m_lstEffects.Add(pEffect);

        pEffect = CreateLinkedEffect(CSWSEffectListHandler::EFFECT_SLOW, 0,
                                     CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                                     CSWSEffectListHandler::VFX_ON_HIT_STATE);
        if (pEffect != NULL)
        {
            CGameEffect* pVisualEffect = pEffect;
            pEffect = CreateLinkedEffect(pEffect,
                                        CSWSEffectListHandler::EFFECT_EFFECT_ICON,
                                        CSWSEffectListHandler::ICON_SLOW);
            if (pEffect == NULL)
            {
                delete pVisualEffect;
            }
            else
            {
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_TEMPORARY;
                pEffect->m_fDuration = nDuration * (SW_COMBAT_ROUND_SECONDS / 2.0f);
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
                    SUBTYPE_MAGICAL;
                pEffect->UpdateLinked();
                unsigned char nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);
            }
        }
    }
}
// PC 0x006090E0
void CSWSCreature::ApplyOnHitStun(CSWSObject* pObject, int nDuration)
{
    CGameEffect* pEffect =
        CreateLinkedEffect(CSWSEffectListHandler::EFFECT_SETSTATE,
                           CSWSEffectListHandler::STATE_STUN,
                           CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                           CSWSEffectListHandler::VFX_CONFUSION_PRIMARY);
    if (pEffect != NULL)
    {
        CGameEffect* pStateEffect = pEffect;
        pEffect =
            CreateLinkedEffect(pEffect,
                               CSWSEffectListHandler::EFFECT_VISUALEFFECT,
                               CSWSEffectListHandler::VFX_ON_HIT_STATE);
        if (pEffect == NULL)
        {
            delete pStateEffect;
        }
        else
        {
            CGameEffect* pVisualEffect = pEffect;
            pEffect = CreateLinkedEffect(pEffect,
                                        CSWSEffectListHandler::EFFECT_EFFECT_ICON,
                                        CSWSEffectListHandler::ICON_STUN);
            if (pEffect == NULL)
            {
                delete pVisualEffect;
            }
            else
            {
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_TEMPORARY;
                pEffect->m_fDuration = nDuration * (SW_COMBAT_ROUND_SECONDS / 2.0f);
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
                    SUBTYPE_MAGICAL;
                pEffect->UpdateLinked();
                unsigned char nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);
            }
        }
    }
}
// PC 0x006091E0
void CSWSCreature::ApplyOnHitAbilityDamage(CSWSObject* pObject,
                                           CSWItemProperty* pItemProperty)
{
    CGameEffect* pEffect = new CGameEffect(1);
    if (pEffect != NULL)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
        pEffect->SetInteger(0, CSWSEffectListHandler::VFX_ABILITY_DAMAGE_IMPACT);
        pEffect->SetCreator(GetId());
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_TEMPORARY;
        pEffect->m_fDuration = 30.0f;
        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack)
            ->m_lstEffects.Add(pEffect);

        pEffect = new CGameEffect(1);
        if (pEffect != NULL)
        {
            pEffect->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
            pEffect->SetInteger(0, pItemProperty->m_nParam1Value);
            pEffect->SetInteger(1, 1);
            CGameEffect* pAbilityEffect = pEffect;
            pEffect = CreateLinkedEffect(pEffect,
                                        CSWSEffectListHandler::EFFECT_EFFECT_ICON,
                                        CSWSEffectListHandler::ICON_ABILITY_DECREASE);
            if (pEffect == NULL)
            {
                delete pAbilityEffect;
            }
            else
            {
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_TEMPORARY;
                pEffect->m_fDuration = 30.0f;
                if (pItemProperty->m_nPropertyType ==
                    CSWItemProperty::ITEM_PROPERTY_ON_MONSTER_HIT)
                {
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
                        SUBTYPE_SUPERNATURAL;
                }
                else
                {
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
                        SUBTYPE_EXTRAORDINARY;
                }
                pEffect->UpdateLinked();
                unsigned char nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);

                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, pObject->m_nId);
                pData->SetInteger(0, FEEDBACK_ON_HIT_ABILITY_DAMAGE);
                pData->SetInteger(1, pItemProperty->m_nParam1Value);
                pData->SetInteger(2, 1);
                pData->SetFloat(0, 30.0f);
                nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                m_pcCombatRound->GetAttack(nCurrentAttack)
                    ->m_lstFeedbackMessages.Add(pData);
            }
        }
    }
}
// PC 0x00609470
void CSWSCreature::ApplyOnHitPoison(CSWSObject* pObject,
                                    CSWItemProperty* pItemProperty)
{
    unsigned char nParam1Value = pItemProperty->m_nParam1Value;
    CGameEffect* pEffect =
        CreateLinkedEffect(CSWSEffectListHandler::EFFECT_POISON, nParam1Value,
                           CSWSEffectListHandler::EFFECT_EFFECT_ICON, 23);

    if (pEffect != NULL)
    {
        pEffect->m_nSubType =
            (CGameEffect::DURATION_TYPE_PERMANENT &
             ~SUBTYPE_SUPERNATURAL) |
            (pEffect->m_nSubType & ~(CGameEffect::DURATION_MASK |
                                     SUBTYPE_SUPERNATURAL)) |
            SUBTYPE_MAGICAL;

        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        pAttack->m_lstEffects.Add(pEffect);
    }
}
// PC 0x00609510
void CSWSCreature::ApplyOnHitSlayRacialGroup(CSWSObject* pObject,
                                             CSWItemProperty* pItemProperty)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        unsigned short nRace = pStats->m_nRace;
        if (nRace == pItemProperty->m_nParam1Value)
        {
            CGameEffect* pEffect = new CGameEffect(1);
            if (pEffect != NULL)
            {
                pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
                pEffect->SetInteger(0, CSWSEffectListHandler::VFX_SLAY_IMPACT);
                pEffect->SetCreator(GetId());
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                unsigned char nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);

                pEffect = new CGameEffect(1);
                if (pEffect != NULL)
                {
                    pEffect->m_nType = CSWSEffectListHandler::EFFECT_DEATH;
                    pEffect->SetCreator(GetId());
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                    pEffect->SetInteger(0, 0);
                    pEffect->SetInteger(1, 1);
                    nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
                    m_pcCombatRound->GetAttack(nCurrentAttack)->m_lstEffects.Add(pEffect);
                }
            }
        }
    }
}
void CSWSCreature::ApplyOnHitInstantDeath(CSWSObject* pObject,  // PC 0x006096A0
                                          CSWItemProperty* pItemProperty)
{
    CGameEffect* pDeathEffect = new CGameEffect(1);

    if (pDeathEffect != NULL)
    {
        unsigned short nSubType = pDeathEffect->m_nSubType;
        pDeathEffect->m_nType = CSWSEffectListHandler::EFFECT_DEATH;
        pDeathEffect->m_nSubType = (nSubType & ~CGameEffect::DURATION_MASK) |
                                   CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pDeathEffect->SetCreator(m_nId);
        pDeathEffect->SetInteger(0, 0);
        pDeathEffect->SetInteger(1, 1);

        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        pAttack->m_lstEffects.Add(pDeathEffect);
    }
}
// PC 0x006097A0
void CSWSCreature::ApplyOnHitKnockdown(CSWSObject* pObject,
                                       CSWItemProperty* pItemProperty)
{
    CGameEffect* pEffect = new CGameEffect(1);

    if (pEffect != NULL)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEPUSHED;
        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                              CGameEffect::DURATION_TYPE_TEMPORARY;
        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                              CGameEffect::DURATION_TYPE_TEMPORARY;
        pEffect->m_fDuration = 0.1f;
        pEffect->SetCreator(m_nId);

        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        pAttack->m_lstEffects.Add(pEffect);
    }
}

inline void CSWSCreature::ResolveSituationalModifiers(CSWSCreature* pTarget)
{
}
// PC 0x006098A0
unsigned long CSWSCreature::GetFirstAttacker()
{
    CSWSArea* pArea = GetArea();
    if (pArea != NULL)
    {
        CExoArrayList<unsigned long>* pObjects = &pArea->m_lstObjectIds;

        if (m_pAttackerList != NULL)
        {
            delete m_pAttackerList;
        }
        m_pAttackerList = new CExoArrayList<unsigned long>(0);

        for (int nObject = 0; nObject < pObjects->num; nObject++)
        {
            unsigned long oidObject = pObjects->element[nObject];
            CSWSCreature* pCreature =
                g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidObject);
            if (pCreature != NULL && pCreature->m_oidAttackTarget == m_nId)
            {
                m_pAttackerList->Add(oidObject);
            }
        }

        if (m_pAttackerList->num > 0)
        {
            m_nNextAttacker = 1;
            return (*m_pAttackerList)[0];
        }
    }
    return OBJECT_INVALID;
}
enum
{
    FEAT_RANGED_ASSASSINATE_1 = 228,
    FEAT_RANGED_ASSASSINATE_2 = 229,
    FEAT_RANGED_ASSASSINATE_3 = 230,
    RANGED_ASSASSINATE_CHANCE = 20,
    RANGED_ASSASSINATE_BASE_DC = 10
};
// PC 0x006099E0
void CSWSCreature::ResolveRangedSpecialAttack(CSWSObject* pTarget,
                                            int bUseAttackResult)
{
    if (pTarget != NULL && bUseAttackResult == 0)
    {
        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        CSWCCreature* pClientCreature = GetClientCreature();
        if (pClientCreature != NULL)
        {
            unsigned char nWieldClass = pClientCreature->GetAnimationBase()->m_nWieldClass;
            if (nWieldClass == CSWCAnimBase::WIELD_CLASS_4 ||
                nWieldClass == CSWCAnimBase::WIELD_CLASS_DUAL_4 ||
                nWieldClass == CSWCAnimBase::WIELD_CLASS_5)
            {
                int nAssassinateLevel = 0;
                if (m_pStats->HasFeat(FEAT_RANGED_ASSASSINATE_3))
                    nAssassinateLevel = 3;
                else if (m_pStats->HasFeat(FEAT_RANGED_ASSASSINATE_2))
                    nAssassinateLevel = 2;
                else if (m_pStats->HasFeat(FEAT_RANGED_ASSASSINATE_1))
                    nAssassinateLevel = 1;
                if (nAssassinateLevel != 0 &&
                    IsAttackResultHit(pAttack->m_nAttackResult) &&
                    pTarget->AsSWSCreature() != NULL &&
                    pAttack->m_nAttackResult == ATTACK_RESULT_CRITICAL_HIT &&
                    rand() % 100 < RANGED_ASSASSINATE_CHANCE)
                {
                    CGameEffect* pAssassinateEffect = new CGameEffect(1);
                    pAssassinateEffect->m_nType = CSWSEffectListHandler::EFFECT_ASSASSINATE;
                    pAssassinateEffect->m_nSubType =
                        (pAssassinateEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                    pAssassinateEffect->SetInteger(0, nAssassinateLevel);
                    pAssassinateEffect->SetCreator(GetId());
                    CSWSCreature* pCreature = pTarget->AsSWSCreature();
                    int nDC = m_pStats->GetLevel(0) + RANGED_ASSASSINATE_BASE_DC;
                    if (pCreature->m_pStats->GetEffectLinkImmunity(pAssassinateEffect) == 0 &&
                        pCreature->SavingThrowRoll(SAVING_THROW_FORT, nDC, 0,
                                                  GetId(), 1, 0, 1) == 0)
                    {
                        pAttack->m_lstEffects.Add(pAssassinateEffect);
                    }
                    else
                    {
                        delete pAssassinateEffect;
                    }
                }
            }
        }
        int nACPenalty = 0;
        if (pAttack->m_nAttackType != 0)
        {
            if (m_pStats->GetFeatRemainingUses(pAttack->m_nAttackType) <= 0)
            {
                pAttack->m_nAttackType = 0;
            }
            switch (pAttack->m_nAttackType)
            {
            case CSWFeat::FEAT_IMPROVED_POWER_BLAST:
            case CSWFeat::FEAT_POWER_BLAST:
            case CSWFeat::FEAT_MASTER_POWER_BLAST:
                {
                    if (IsAttackResultHit(pAttack->m_nAttackResult) &&
                        pTarget->AsSWSCreature() != NULL &&
                        pAttack->m_nAttackResult == ATTACK_RESULT_CRITICAL_HIT)
                    {
                        CGameEffect* pPushEffect = new CGameEffect(1);
                        pPushEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEPUSHED;
                        pPushEffect->m_nSubType =
                            (pPushEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_TEMPORARY;
                        pPushEffect->m_fDuration = 0.1f;
                        pPushEffect->SetCreator(GetId());
                        CSWSCreature* pCreature = pTarget->AsSWSCreature();
                        CSWSCreatureStats* pStats = m_pStats;
                        int nDexMod = pStats->GetDEXMod(0);
                        int nDC = pStats->GetLevel(0) + nDexMod * 2;
                        if (pCreature->m_pStats->GetEffectLinkImmunity(pPushEffect) == 0 &&
                            pCreature->SavingThrowRoll(SAVING_THROW_FORT, nDC, 0,
                                                      GetId(), 1, 0, 1) == 0)
                        {
                            pAttack->m_lstEffects.Add(pPushEffect);
                            pAttack->m_Extra4.m_nState = CSWSEffectListHandler::STATE_KNOCKDOWN;
                            pAttack->m_Extra4.m_nUnknown14 = 1;
                        }
                        else
                        {
                            delete pPushEffect;
                            pAttack->m_Extra4.m_nUnknown14 = 1;
                        }
                    }
                    break;
                }
            case CSWFeat::FEAT_MULTI_SHOT:
            case CSWFeat::FEAT_RAPID_SHOT:
            case CSWFeat::FEAT_IMPROVED_RAPID_SHOT:
                {
                    switch (pAttack->m_nAttackType)
                    {
                    case CSWFeat::FEAT_MULTI_SHOT:
                        nACPenalty = CSWFeat::MULTI_SHOT_AC_PENALTY;
                        break;
                    case CSWFeat::FEAT_RAPID_SHOT:
                        nACPenalty = CSWFeat::RAPID_SHOT_AC_PENALTY;
                        break;
                    case CSWFeat::FEAT_IMPROVED_RAPID_SHOT:
                        nACPenalty = CSWFeat::IMPROVED_RAPID_SHOT_AC_PENALTY;
                        break;
                    }
                    CGameEffect* pEffect = new CGameEffect(1);
                    pEffect->m_nType = CSWSEffectListHandler::EFFECT_AC_DECREASE;
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pEffect->m_fDuration = 3.0f;
                    pEffect->SetCreator(GetId());
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_TYPE,
                                        CSWSEffectListHandler::AC_DODGE);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_BONUS,
                                        nACPenalty);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_RACIAL_GROUP,
                                        g_pRules->m_nNumRaces);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_DAMAGE_FLAGS,
                                        CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL);
                    ApplyEffect(pEffect, 0, 0);
                    break;
                }
            case CSWFeat::FEAT_IMPROVED_SNIPER_SHOT:
            case CSWFeat::FEAT_SNIPER_SHOT:
            case CSWFeat::FEAT_MASTER_SNIPER_SHOT:
                {
                    CGameEffect* pEffect = new CGameEffect(1);
                    pEffect->m_nType = CSWSEffectListHandler::EFFECT_AC_DECREASE;
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pEffect->m_fDuration = 3.0f;
                    pEffect->SetCreator(GetId());
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_TYPE,
                                        CSWSEffectListHandler::AC_DODGE);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_BONUS,
                                        CSWFeat::SNIPER_SHOT_AC_PENALTY);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_RACIAL_GROUP,
                                        g_pRules->m_nNumRaces);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_DAMAGE_FLAGS,
                                        CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL);
                    ApplyEffect(pEffect, 0, 0);
                    if (IsAttackResultHit(pAttack->m_nAttackResult) &&
                        pTarget->AsSWSCreature() != NULL)
                    {
                        CGameEffect* pStunEffect = new CGameEffect(1);
                        pStunEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
                        pStunEffect->SetInteger(0, CSWSEffectListHandler::STATE_STUN);
                        pStunEffect->m_nSubType =
                            (pStunEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_TEMPORARY;
                        pStunEffect->m_fDuration = 6.0f;
                        pStunEffect->SetCreator(GetId());
                        int nDC = m_pStats->GetLevel(0) + m_pStats->GetINTMod();
                        CSWSCreature* pCreature = pTarget->AsSWSCreature();
                        if (pCreature->m_pStats->GetEffectLinkImmunity(pStunEffect) == 0 &&
                            pCreature->SavingThrowRoll(SAVING_THROW_FORT, nDC, 0,
                                                      GetId(), 1, 0, 1) == 0)
                        {
                            pAttack->m_lstEffects.Add(pStunEffect);
                            pAttack->m_Extra4.m_nState = CSWSEffectListHandler::STATE_STUN;
                        }
                        else
                        {
                            delete pStunEffect;
                        }
                        pAttack->m_Extra4.m_nUnknown14 = 1;
                    }
                    break;
                }
            }
        }
    }
}
enum
{
    FEAT_UNARMED_FORCE_PUSH_1 = 209,
    FEAT_UNARMED_FORCE_PUSH_2 = 210,
    FEAT_UNARMED_FORCE_PUSH_3 = 211,
    FEAT_SHIELD_BREAKER = 220,
    FEAT_UNARMED_DAMAGE_1 = 221,
    FEAT_UNARMED_DAMAGE_2 = 222,
    FEAT_UNARMED_DAMAGE_3 = 223,
    UNARMED_DAMAGE_DIE = 6,
    UNARMED_DAMAGE_FLAGS = 7,
    UNARMED_SLOW_PERCENT = 50,
    UNARMED_SLOW_CHANCE_1 = 10,
    UNARMED_SLOW_CHANCE_2 = 15,
    UNARMED_SLOW_CHANCE_3 = 20,
    UNARMED_FORCE_PUSH_CHANCE_1 = 10,
    UNARMED_FORCE_PUSH_CHANCE_2 = 15,
    UNARMED_FORCE_PUSH_CHANCE_3 = 20,
    UNARMED_SAVE_BASE_DC = 15,
    VISUAL_EFFECT_SHIELD_BREAKER = 8001,
    ATTACK_KILL_COUNT_LIMIT = 6
};
// PC 0x0060A060
void CSWSCreature::ResolveMeleeSpecialAttack(int nAttack, int nAttacks,
                                           CSWSObject* pTarget, int nImpactTime,
                                           int* pbStateApplied)
{
    int nIndex = 0;
    *pbStateApplied = 0;
    if (pTarget != NULL && nAttack == 0)
    {
        int nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
        CSWSCombatAttackData* pAttack = m_pcCombatRound->GetAttack(nCurrentAttack);

        CServerAIMaster* pAIMaster =
            g_pAppManager->GetServerExoApp()->GetServerAIMaster();
        if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_UNARMED ||
            pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED)
        {
            int nUnarmedLevel = 0;
            if (m_pStats->HasFeat(FEAT_UNARMED_DAMAGE_3))
                nUnarmedLevel = 3;
            else if (m_pStats->HasFeat(FEAT_UNARMED_DAMAGE_2))
                nUnarmedLevel = 2;
            else if (m_pStats->HasFeat(FEAT_UNARMED_DAMAGE_1))
                nUnarmedLevel = 1;
            if (nUnarmedLevel != 0 &&
                IsAttackResultHit(pAttack->m_nAttackResult) &&
                pTarget->AsSWSCreature() != NULL)
            {
                int nDamage = g_pRules->RollDice(nUnarmedLevel, UNARMED_DAMAGE_DIE);
                CGameEffect* pDamage = new CGameEffect(1);
                pDamage->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE;
                pDamage->m_nSubType = (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
                                     CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                pDamage->SetCreator(GetId());
                pDamage->SetNumIntegers(CSWSEffectListHandler::APPLIED_DAMAGE_INTEGER_COUNT);
                for (; nIndex < CSWSEffectListHandler::APPLIED_DAMAGE_TYPE_COUNT; nIndex++)
                {
                    pDamage->SetInteger(nIndex, -1);
                }
                pDamage->SetInteger(7, nDamage);
                pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_BASE, nDamage);
                pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_ANIMATION_DELAY, 0);
                pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_FLAGS,
                                    Log_Base2(UNARMED_DAMAGE_FLAGS));
                pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_MELEE, 1);
                pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_RANGED, 1);
                pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_COMBAT, 0);
                pAttack->m_lstEffects.Add(pDamage);

                CGameEffect* pSlowEffect = new CGameEffect(1);
                pSlowEffect->m_nType = CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_DECREASE;
                pSlowEffect->m_nSubType =
                    (pSlowEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
                    SUBTYPE_MAGICAL;
                pSlowEffect->SetInteger(0, UNARMED_SLOW_PERCENT);
                pSlowEffect->SetCreator(GetId());
                pSlowEffect->m_fDuration = 6.0f;
                pSlowEffect->m_nSubType =
                    (pSlowEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    DURATION_TYPE_TEMPORARY;
                CSWSCreature* pCreature = pTarget->AsSWSCreature();
                int nDC = m_pStats->GetLevel(0) + UNARMED_SAVE_BASE_DC;
                if (pCreature->m_pStats->GetEffectLinkImmunity(pSlowEffect) == 0)
                {
                    int nRoll = rand() % 100;
                    int bSlow;
                    switch (nUnarmedLevel)
                    {
                    case 1:
                        bSlow = nRoll < UNARMED_SLOW_CHANCE_1;
                        break;
                    case 2:
                        bSlow = nRoll < UNARMED_SLOW_CHANCE_2;
                        break;
                    case 3:
                        bSlow = nRoll < UNARMED_SLOW_CHANCE_3;
                        break;
                    default:
                        bSlow = FALSE;
                        break;
                    }
                    if (bSlow && pCreature->SavingThrowRoll(SAVING_THROW_FORT, nDC, 0,
                                                           GetId(), 1, 0, 1) == 0)
                    {
                        pAttack->m_lstEffects.Add(pSlowEffect);
                    }
                    else
                    {
                        delete pSlowEffect;
                    }
                }
                else
                {
                    delete pSlowEffect;
                }
            }
        }
        int nACPenalty = 0;
        if (pAttack->m_nAttackType != 0)
        {
            if (GetStats()->GetFeatRemainingUses(pAttack->m_nAttackType) <= 0)
            {
                pAttack->m_nAttackType = 0;
            }
            switch (pAttack->m_nAttackType)
            {
            case CSWFeat::FEAT_CRITICAL_STRIKE:
            case CSWFeat::FEAT_IMPROVED_CRITICAL_STRIKE:
            case CSWFeat::FEAT_MASTER_CRITICAL_STRIKE:
                {
                    CGameEffect* pEffect = new CGameEffect(1);
                    pEffect->m_nType = CSWSEffectListHandler::EFFECT_AC_DECREASE;
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pEffect->m_fDuration = 3.0f;
                    pEffect->SetCreator(GetId());
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_TYPE,
                                        CSWSEffectListHandler::AC_DODGE);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_BONUS,
                                        CSWFeat::CRITICAL_STRIKE_AC_PENALTY);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_RACIAL_GROUP,
                                        g_pRules->m_nNumRaces);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_DAMAGE_FLAGS,
                                        CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL);
                    ApplyEffect(pEffect, 0, 0);
                    if (IsAttackResultHit(pAttack->m_nAttackResult) &&
                        pTarget->AsSWSCreature() != NULL)
                    {
                        CGameEffect* pStunEffect = new CGameEffect(1);
                        pStunEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
                        pStunEffect->SetInteger(0, CSWSEffectListHandler::STATE_STUN);
                        pStunEffect->m_nSubType =
                            (pStunEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_TEMPORARY;
                        pStunEffect->m_fDuration = 6.0f;
                        pStunEffect->SetCreator(GetId());
                        CSWSCreature* pCreature = pTarget->AsSWSCreature();
                        int nDC = m_pStats->GetLevel(0) + m_pStats->GetSTRMod();
                        if (pCreature->m_pStats->GetEffectLinkImmunity(pStunEffect) == 0 &&
                            pCreature->SavingThrowRoll(SAVING_THROW_FORT, nDC, 0,
                                                      GetId(), 1, 0, 1) == 0)
                        {
                            pAttack->m_lstEffects.Add(pStunEffect);
                            pAttack->m_Extra4.m_nState = CSWSEffectListHandler::STATE_STUN;
                            *pbStateApplied = 1;
                            pAttack->m_Extra4.m_nUnknown14 = 1;
                        }
                        else
                        {
                            delete pStunEffect;
                            pAttack->m_Extra4.m_nUnknown14 = 1;
                        }
                    }
                    break;
                }
            case CSWFeat::FEAT_FLURRY:
            case CSWFeat::FEAT_WHIRLWIND_ATTACK:
            case CSWFeat::FEAT_IMPROVED_FLURRY:
                {
                    switch (pAttack->m_nAttackType)
                    {
                    case CSWFeat::FEAT_FLURRY:
                        nACPenalty = CSWFeat::FLURRY_AC_PENALTY;
                        break;
                    case CSWFeat::FEAT_WHIRLWIND_ATTACK:
                        nACPenalty = CSWFeat::WHIRLWIND_ATTACK_AC_PENALTY;
                        break;
                    case CSWFeat::FEAT_IMPROVED_FLURRY:
                        nACPenalty = CSWFeat::IMPROVED_FLURRY_AC_PENALTY;
                        break;
                    }
                    CGameEffect* pEffect = new CGameEffect(1);
                    pEffect->m_nType = CSWSEffectListHandler::EFFECT_AC_DECREASE;
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pEffect->m_fDuration = 3.0f;
                    pEffect->SetCreator(GetId());
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_TYPE,
                                        CSWSEffectListHandler::AC_DODGE);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_BONUS,
                                        nACPenalty);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_RACIAL_GROUP,
                                        g_pRules->m_nNumRaces);
                    pEffect->SetInteger(CSWSEffectListHandler::AC_INTEGER_DAMAGE_FLAGS,
                                        CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL);
                    ApplyEffect(pEffect, 0, 0);
                    break;
                }
            case CSWFeat::FEAT_IMPROVED_POWER_ATTACK:
            case CSWFeat::FEAT_POWER_ATTACK:
            case CSWFeat::FEAT_MASTER_POWER_ATTACK:
                {
                    if (IsAttackResultHit(pAttack->m_nAttackResult) &&
                        pTarget->AsSWSCreature() != NULL &&
                        pAttack->m_nAttackResult == ATTACK_RESULT_CRITICAL_HIT)
                    {
                        CGameEffect* pPushEffect = new CGameEffect(1);
                        pPushEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEPUSHED;
                        pPushEffect->m_nSubType =
                            (pPushEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_TEMPORARY;
                        pPushEffect->m_fDuration = 0.1f;
                        pPushEffect->SetCreator(GetId());
                        CSWSCreature* pCreature = pTarget->AsSWSCreature();
                        int nDC = m_pStats->GetLevel(0) + m_pStats->GetSTRMod() * 2;
                        if (pCreature->m_pStats->GetEffectLinkImmunity(pPushEffect) == 0 &&
                            pCreature->SavingThrowRoll(SAVING_THROW_FORT, nDC, 0,
                                                      GetId(), 1, 0, 1) == 0)
                        {
                            pAttack->m_lstEffects.Add(pPushEffect);
                            pAttack->m_Extra4.m_nState = CSWSEffectListHandler::STATE_KNOCKDOWN;
                            *pbStateApplied = 1;
                            pAttack->m_Extra4.m_nUnknown14 = 1;
                        }
                        else
                        {
                            delete pPushEffect;
                            pAttack->m_Extra4.m_nUnknown14 = 1;
                        }
                    }
                    break;
                }
            case FEAT_SHIELD_BREAKER:
                {
                    if (IsAttackResultHit(pAttack->m_nAttackResult) &&
                        pTarget->AsSWSCreature() != NULL)
                    {
                        CGameEffect* pVisualEffect = new CGameEffect(1);
                        pVisualEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
                        pVisualEffect->m_nSubType =
                            (pVisualEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
                            SUBTYPE_MAGICAL;
                        pVisualEffect->SetInteger(0, VISUAL_EFFECT_SHIELD_BREAKER);
                        pVisualEffect->SetCreator(GetId());
                        pAttack->m_lstEffects.Add(pVisualEffect);
                        CGameEffect* pShieldEffect = new CGameEffect(1);
                        pShieldEffect->m_nType = CSWSEffectListHandler::EFFECT_DESTROYSHIELDS;
                        pShieldEffect->SetCreator(GetId());
                        pAttack->m_lstEffects.Add(pShieldEffect);
                    }
                    break;
                }
            }
        }
    }
}
enum
{
    FORM_SABER_III_SORESU = 260,
    FORM_SABER_IV_ATARU = 261,
    FORM_SABER_VII_JUYO = 264,
    FORM_SABER_VII_JUYO_CRITICAL_BONUS = 4
};
// PC 0x0060A9F0
void CSWSCreature::ResolveAttackRoll(CSWSObject* pTarget, int bUseAttackResult,
                                   int nAttackResult)
{
    if (pTarget != NULL)
    {
        CSWSCombatRound* pRound = m_pcCombatRound;
        int nCurrentAttack = pRound->m_nCurrentAttack;
        CSWSCombatAttackData* pAttack = pRound->GetAttack(nCurrentAttack);
        pAttack->m_Extra2.Clear();
        int bOffHand = pRound->GetOffHandAttack();
        pAttack->m_nWeaponAttackType = m_pcCombatRound->GetWeaponAttackType();

        int nRoll = g_pRules->RollDice(1, 20);
        if (g_nCombatAttackRoll != 0)
            nRoll = g_nCombatAttackRoll;
        int nModifier = GetStats()->GetAttackModifierVersus(pTarget->AsSWSCreature());
        int nArmorClass = 0;
        CSWSCreature* pCreature = NULL;
        if (pTarget->AsSWSCreature() != NULL)
        {
            pCreature = pTarget->AsSWSCreature();
            nArmorClass = pCreature->GetStats()->GetArmorClassVersus(this, 0);
            pAttack->m_Flags.m_bUnknown0 = nArmorClass;
            ResolveSneakAttack(pCreature);
        }

        if (bUseAttackResult)
        {
            pAttack->m_nAttackResult = nAttackResult;
            return;
        }
        if (pAttack->m_bCoupDeGrace)
        {
            pAttack->m_nAttackResult = ATTACK_RESULT_AUTOMATIC_HIT;
            pAttack->m_Extra1.m_nUnknown1 = CSWRules::ATTACK_ROLL_AUTOMATIC_HIT;
            pAttack->m_Extra1.m_nUnknown2 = nModifier;
            return;
        }
        if (m_bAssuredHit)
        {
            pAttack->m_nAttackResult = ATTACK_RESULT_HIT_SUCCESSFUL;
            pAttack->m_Extra1.m_nUnknown1 = nRoll;
            pAttack->m_Extra1.m_nUnknown2 = nModifier;
            return;
        }

        pAttack->m_Extra1.m_nUnknown1 = nRoll;
        pAttack->m_Extra1.m_nUnknown2 = nModifier;
        int bHit = 0;
        int nTotalRoll = nRoll + nModifier;
        if (nTotalRoll >= nArmorClass)
            bHit = 1;
        if (ResolveDefensiveEffects(pTarget, bHit))
            return;

        if (nRoll != CSWRules::ATTACK_ROLL_AUTOMATIC_MISS)
        {
            pAttack->m_Extra1.m_nUnknown1C = 0;
            if (nTotalRoll >= nArmorClass || nRoll == CSWRules::ATTACK_ROLL_AUTOMATIC_HIT)
            {
                if (nRoll == CSWRules::ATTACK_ROLL_AUTOMATIC_HIT)
                    pAttack->m_Extra1.m_nUnknown18 = 1;
                else
                    pAttack->m_Extra1.m_nUnknown18 = 0;
                int nThreatRoll = GetStats()->GetCriticalHitRoll(bOffHand);
                if (m_n1110 == FORM_SABER_III_SORESU && IsWieldingLightsaber())
                {
                    if (nThreatRoll < CSWRules::ATTACK_ROLL_AUTOMATIC_HIT)
                        nThreatRoll++;
                }
                else if (m_n1110 == FORM_SABER_IV_ATARU && IsWieldingLightsaber() &&
                         nThreatRoll > 1)
                {
                    nThreatRoll--;
                }
                pAttack->m_Extra2.m_nUnknown0 = nThreatRoll;
                if (nRoll >= nThreatRoll)
                {
                    pAttack->m_Extra2.m_nUnknown4 = 1;
                    int nCriticalRoll = g_pRules->RollDice(1, 20);
                    pAttack->m_Extra2.m_nUnknown8 = nCriticalRoll;
                    if (m_n1110 == FORM_SABER_VII_JUYO && IsWieldingLightsaber())
                        nCriticalRoll += FORM_SABER_VII_JUYO_CRITICAL_BONUS;
                    if (nCriticalRoll + nModifier >= nArmorClass)
                    {
                        pAttack->m_Extra2.m_nUnknownC = 1;
                        pAttack->m_Extra2.m_nUnknown11 =
                            GetStats()->GetCriticalHitMultiplier(bOffHand);
                        if (pCreature != NULL &&
                            pCreature->GetStats()->GetEffectImmunity(
                                CSWSEffectListHandler::IMMUNITY_TYPE_CRITICAL_HITS,
                                this))
                        {
                            CSWCCMessageData* pMessage = new CSWCCMessageData;
                            pMessage->SetObjectID(0, pCreature->GetId());
                            pMessage->SetInteger(0, FEEDBACK_CRITICAL_HITS_IMMUNITY);
                            pAttack->m_lstFeedbackMessages.Add(pMessage);
                            pAttack->m_nAttackResult = ATTACK_RESULT_HIT_SUCCESSFUL;
                        }
                        else
                            pAttack->m_nAttackResult = ATTACK_RESULT_CRITICAL_HIT;
                    }
                    else
                    {
                        pAttack->m_Extra2.m_nUnknownC = 0;
                        pAttack->m_Extra2.m_nUnknown11 = 0;
                        pAttack->m_nAttackResult = ATTACK_RESULT_HIT_SUCCESSFUL;
                    }
                }
                else
                {
                    pAttack->m_Extra2.m_nUnknown4 = 0;
                    pAttack->m_Extra2.m_nUnknownC = 0;
                    pAttack->m_Extra2.m_nUnknown8 = 0;
                    pAttack->m_Extra2.m_nUnknown11 = 0;
                    pAttack->m_nAttackResult = ATTACK_RESULT_HIT_SUCCESSFUL;
                }
            }
            else
                pAttack->m_nAttackResult = ATTACK_RESULT_MISS;
        }
        else
        {
            pAttack->m_Extra1.m_nUnknown1C = 1;
            pAttack->m_nAttackResult = ATTACK_RESULT_MISS;
        }
    }
}
// PC 0x0060AD50
void CSWSCreature::ResolveOnHitEffect(CSWSObject* pTarget, int bOffHand,
                                     int bCritical)
{
    int nEffectChance;
    int nDurationRounds;
    CSWSCreature* pCreature = pTarget->AsSWSCreature();
    if (pCreature != NULL && m_pInventory != NULL)
    {
        int nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
        CSWSCombatAttackData* pAttack = m_pcCombatRound->GetAttack(nCurrentAttack);
        int nWeaponAttackType = pAttack->m_nWeaponAttackType;
        CSWSItem* pWeapon = m_pcCombatRound->GetCurrentAttackWeapon(nWeaponAttackType);
        if (pWeapon != NULL)
        {
            nEffectChance = 0;
            nDurationRounds = 0;
            for (int nProperty = 0; nProperty < pWeapon->m_nPassiveProperties;
                 ++nProperty)
            {
                CSWItemProperty* pProperty = pWeapon->GetPassiveProperty(nProperty);
                if (pProperty != NULL &&
                    (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                     (pWeapon->m_nUpgrades &
                      (CSWSItem::UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType))) &&
                    pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ON_HIT_PROPERTIES)
                {
                    int bDuration = 0;
                    int bSavingThrow = 0;
                    unsigned char nSaveType;
                    unsigned char nSaveSubtype;
                    typedef void (CSWSCreature::*DurationEffect)(CSWSObject*, int);
                    typedef void (CSWSCreature::*PropertyEffect)(CSWSObject*, CSWItemProperty*);
                    DurationEffect pDurationEffect;
                    PropertyEffect pPropertyEffect;
                    switch (pProperty->m_nSubtype)
                    {
                    case CSWItemProperty::ON_HIT_SLEEP:
                        bDuration = 1;
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_WILL;
                        nSaveSubtype = SAVING_THROW_TYPE_MIND_AFFECTING;
                        pDurationEffect = &CSWSCreature::ApplyOnHitSleep;
                        break;
                    case CSWItemProperty::ON_HIT_STUN:
                        bDuration = 1;
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_WILL;
                        nSaveSubtype = SAVING_THROW_TYPE_MIND_AFFECTING;
                        pDurationEffect = &CSWSCreature::ApplyOnHitStun;
                        break;
                    case CSWItemProperty::ON_HIT_HOLD:
                        bDuration = 1;
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_FORT;
                        nSaveSubtype = SAVING_THROW_TYPE_PARALYSIS;
                        pDurationEffect = &CSWSCreature::ApplyOnHitHold;
                        break;
                    case CSWItemProperty::ON_HIT_CONFUSION:
                        bDuration = 1;
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_WILL;
                        nSaveSubtype = SAVING_THROW_TYPE_MIND_AFFECTING;
                        pDurationEffect = &CSWSCreature::ApplyOnHitConfusion;
                        break;
                    case CSWItemProperty::ON_HIT_FEAR:
                        bDuration = 1;
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_WILL;
                        nSaveSubtype = SAVING_THROW_TYPE_FEAR;
                        pDurationEffect = &CSWSCreature::ApplyOnHitFear;
                        break;
                    case CSWItemProperty::ON_HIT_SLOW:
                        bDuration = 1;
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_WILL;
                        nSaveSubtype = SAVING_THROW_TYPE_NONE;
                        pDurationEffect = &CSWSCreature::ApplyOnHitSlow;
                        break;
                    case CSWItemProperty::ON_HIT_ABILITY_DAMAGE:
                        nEffectChance = 100;
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_REFLEX;
                        nSaveSubtype = SAVING_THROW_TYPE_NONE;
                        pPropertyEffect = &CSWSCreature::ApplyOnHitAbilityDamage;
                        break;
                    case CSWItemProperty::ON_HIT_POISON:
                        nEffectChance = 100;
                        bSavingThrow = 0;
                        nSaveType = SAVING_THROW_FORT;
                        nSaveSubtype = SAVING_THROW_TYPE_POISON;
                        pPropertyEffect = &CSWSCreature::ApplyOnHitPoison;
                        break;
                    case CSWItemProperty::ON_HIT_SLAY_RACIAL_GROUP:
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_FORT;
                        nSaveSubtype = SAVING_THROW_TYPE_DEATH;
                        pPropertyEffect = &CSWSCreature::ApplyOnHitSlayRacialGroup;
                        break;
                    case CSWItemProperty::ON_HIT_SLAY_ALIGNMENT_GROUP:
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_FORT;
                        nSaveSubtype = SAVING_THROW_TYPE_DEATH;
                        pPropertyEffect = &CSWSCreature::ApplyOnHitSlayAlignmentGroup;
                        break;
                    case CSWItemProperty::ON_HIT_INSTANT_DEATH:
                        nEffectChance = 100;
                        pPropertyEffect = &CSWSCreature::ApplyOnHitInstantDeath;
                        break;
                    case CSWItemProperty::ON_HIT_KNOCKDOWN:
                        nEffectChance = 100;
                        bSavingThrow = 1;
                        nSaveType = SAVING_THROW_FORT;
                        nSaveSubtype = SAVING_THROW_TYPE_NONE;
                        pPropertyEffect = &CSWSCreature::ApplyOnHitKnockdown;
                        break;
                    default:
                        continue;
                    }

                    int nParam1ResRef;
                    int bParam1ResRef;
                    {
                        CExoString sColumn("Param1ResRef");
                        unsigned short nSubtype = pProperty->m_nSubtype;
                        CTwoDimArrays* pTables = g_pRules->m_p2DArrays;
                        EXOXBOXASSERT(pTables->m_pOnHit);
                        C2DA* pTable = pTables->m_pOnHit;
                        bParam1ResRef = pTable->GetINTEntry(nSubtype, sColumn, &nParam1ResRef);
                    }
                    if (bParam1ResRef && nParam1ResRef == 1)
                    {
                        {
                            CExoString sColumn("EffectChance");
                            unsigned char nParam1Value = pProperty->m_nParam1Value;
                            CTwoDimArrays* pTables = g_pRules->m_p2DArrays;
                            C2DA* pTable = pTables->m_pOnHitDuration;
                            pTable->GetINTEntry(nParam1Value, sColumn, &nEffectChance);
                        }
                        {
                            CExoString sColumn("DurationRounds");
                            unsigned char nParam1Value = pProperty->m_nParam1Value;
                            CTwoDimArrays* pTables = g_pRules->m_p2DArrays;
                            C2DA* pTable = pTables->m_pOnHitDuration;
                            pTable->GetINTEntry(nParam1Value, sColumn, &nDurationRounds);
                        }
                    }
                    int nDC;
                    bool bFailed;
                    {
                        CExoString sColumn("Value");
                        unsigned short nCostValue = pProperty->m_nCostValue;
                        CTwoDimArrays* pTables = g_pRules->m_p2DArrays;
                        bFailed = !pTables->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_DC)
                            ->GetINTEntry(nCostValue, sColumn, &nDC);
                    }
                    if (bFailed)
                        nDC = DEFAULT_ON_HIT_SAVE_DC;

                    if (bDuration)
                    {
                        if (nEffectChance > 0 && nDurationRounds > 0 &&
                            rand() % 100 < nEffectChance &&
                            (!bSavingThrow || !pCreature->SavingThrowRoll(
                                nSaveType, nDC, nSaveSubtype, GetId(), 1, 0, 1)))
                        {
                            (this->*pDurationEffect)(pTarget, nDurationRounds);
                        }
                    }
                    else if ((nEffectChance == 100 || rand() % 100 < nEffectChance) &&
                             (!bSavingThrow || !pCreature->SavingThrowRoll(
                                 nSaveType, nDC, nSaveSubtype, GetId(), 1, 0, 1)))
                    {
                        (this->*pPropertyEffect)(pTarget, pProperty);
                    }
                }
            }
        }
    }
}
// PC 0x0060B1F0
void CSWSCreature::ResolveDamage(CSWSObject* pTarget, int bUseDamage, int nDamage)
{
    CSWSCombatAttackData* pAttack =
        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
    int bSneakAttack = pAttack->m_bSneakAttack;
    int bCritical = (pAttack->m_nAttackResult == ATTACK_RESULT_CRITICAL_HIT);
    int bOffHand = m_pcCombatRound->GetOffHandAttack();
    int nDamageRoll = 0;
    int bDialogParty =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState != 0 &&
        m_bInParty != 0;

    if (!pTarget->m_bPlotObject && !bDialogParty && !g_bDisableDamage)
    {
        if (!bUseDamage)
        {
            CSWSCreatureStats* pStats = m_pStats;
            nDamageRoll = pStats->GetDamageRoll(
                pTarget, bOffHand, bCritical, bSneakAttack, 0, 1);
            if (pTarget->AsSWSCreature() != NULL)
            {
                CSWSCreature* pCreature = pTarget->AsSWSCreature();
                pCreature->GetCurrentHitPoints(0);
            }
            pAttack->SetBaseDamage(nDamageRoll);
            ResolveOnHitEffect(pTarget, bOffHand, bCritical);
            ResolveOnHitVisuals(pTarget);
            if (bCritical)
            {
                BroadcastVoiceChat(VOICE_CHAT_CRITICAL_HIT);
            }
            return;
        }
    }
    if (bUseDamage)
    {
        nDamageRoll = nDamage;
    }
    pAttack->SetBaseDamage(nDamageRoll);
    pAttack->SetDamage(GetDamageFlags(), nDamageRoll);
}
// PC 0x0060B340
void CSWSCreature::ResolveRangedAttack(CSWSObject* pTarget, int nAttacks,
                                       int nAnimation, int bUseAttackResult,
                                       int nAttackResult, int nAttackRollResult,
                                       int nDamage)
{
    if (pTarget != NULL)
    {
        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        pAttack->m_bRangedAttack = 1;
        pAttack->m_oidReactObject = pTarget->GetId();
        CSWSCombatRound* pCombatRound = m_pcCombatRound;
        pAttack->m_nAttackGroup = ++pCombatRound->m_nAttackGroup;
        int nTotalDamage = 0;
        int nRangedAnimation;
        if (bUseAttackResult)
        {
            nRangedAnimation = nAttackResult;
        }
        else
        {
            nRangedAnimation = GetClientRangedAnimation(pAttack->m_nAttackType, 0, 0);
        }
        int nDischargeShots = GetWeaponDischargeShots(nRangedAnimation);
        int nShotsRemaining = nDischargeShots;
        GetWeaponHitShots(nRangedAnimation);
        int nAttacksRemaining = (unsigned char)m_pcCombatRound->GetTotalAttacks();
        int nAttack = 0;
        int bAnyHit = 0;
        for (int nShot = 0; nShot < nDischargeShots; nShot++)
        {
            int bHit = 0;
            int bAttack = 0;
            pAttack->m_nAttackResult = ATTACK_RESULT_MISS;
            if (rand() % nShotsRemaining < nAttacksRemaining)
            {
                --nAttacksRemaining;
                bAttack = 1;
                m_pcCombatRound->SetOffHandAttack(
                    nAttack >= (unsigned char)m_pcCombatRound->m_nOnHandAttacks);
                pAttack->m_nWeaponAttackType = GetCombatRound()->GetOffHandAttack()
                    ? CSWSCombatRound::WEAPON_ATTACK_LEFTHAND
                    : CSWSCombatRound::WEAPON_ATTACK_RIGHTHAND;
                ResolveAttackRoll(pTarget, bUseAttackResult, nAttackRollResult);
                ResolveRangedSpecialAttack(pTarget, nAttack);
                if (IsAttackResultHit(pAttack->m_nAttackResult) ||
                    IsAttackResultParried(pAttack->m_nAttackResult))
                {
                    bHit = 1;
                    bAnyHit = 1;
                }
                ++nAttack;
            }
            if (IsAttackResultMiss(pAttack->m_nAttackResult))
            {
                int nWeaponHand = GetWeaponDischargeHand(nRangedAnimation, nShot);
                bHit = ResolveRangedMissToParry(pTarget, 1, nWeaponHand);
            }
            if (bHit)
            {
                ResolveRangedHit(pTarget);
                if (IsAttackResultHit(pAttack->m_nAttackResult) ||
                    pAttack->m_nAttackResult == ATTACK_RESULT_DEFLECTED)
                {
                    ResolveDamage(pTarget, bUseAttackResult, nDamage);
                    CSWSCombatAttackData* pDamageAttack =
                        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
                    if (pDamageAttack->GetBaseDamage() > 0)
                    {
                        nTotalDamage += pDamageAttack->GetBaseDamage();
                    }
                    ResolvePostRangedDamage(pTarget);
                }
            }
            else
            {
                ResolveRangedMiss(pTarget);
            }
            int nDischargeTime = GetWeaponDischargeTime(nRangedAnimation, nShot);
            int nDischargeHand = GetWeaponDischargeHand(nRangedAnimation, nShot);
            pAttack->ResolveAttackToRangedSubAttack(
                nShot, nDischargeTime, bAttack, bHit, nDischargeHand);
            --nShotsRemaining;
        }
        if (nTotalDamage >= pTarget->GetCurrentHitPoints(0))
        {
            if (m_pStats->m_c120 != -1 && m_pStats->m_c120 < ATTACK_KILL_COUNT_LIMIT)
            {
                m_pStats->m_c120++;
            }
        }
        ResolveRangedAnimations(pTarget, nAnimation, bAnyHit);
        if (bUseAttackResult)
        {
            pAttack->m_nUnknownB8 = 1;
            pAttack->m_nUnknownBC = nAttackResult;
        }
    }
}

int CSWSCreature::GetClientMeleeAnimation(unsigned long oidTarget, unsigned short nAnimation,
                                          int nAttackType, int bUseAttackResult,
                                          int nAttackResult)
{
    return g_pAppManager->GetClientExoApp()->GetClientMeleeAnimation(
        GetId(), oidTarget, nAnimation, nAttackType, bUseAttackResult, nAttackResult);
}
// PC 0x0060B660
void CSWSCreature::ResolveMeleeAttack(CSWSObject* pTarget, int nAttacks,
                                      int nAnimation, int bUseAttackResult,
                                      int nAttackResult, int nAttackRollResult,
                                      int nDamage)
{
    int nAttack = 0;
    if (pTarget == NULL)
    {
        return;
    }

    int nTotalDamage = 0;
    CSWSCombatAttackData* pAttack =
        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
    unsigned char nTotalAttacks = m_pcCombatRound->GetTotalAttacks();
    for (nAttack = 0; nAttack < nTotalAttacks; nAttack++)
    {
        pAttack->m_oidReactObject = pTarget->m_nId;
        pAttack->m_nWeaponAttackType = m_pcCombatRound->GetWeaponAttackType();
        m_pcCombatRound->SetOffHandAttack(
            nAttack >= (unsigned char)m_pcCombatRound->m_nOnHandAttacks);
        ResolveSituationalModifiers(pTarget->AsSWSCreature());
        ResolveAttackRoll(pTarget, bUseAttackResult, nAttackRollResult);
        int nMeleeAnimation =
            GetClientMeleeAnimation(pTarget->GetId(), GetAnimation(), pAttack->m_nAttackType,
                                    bUseAttackResult, nAttackResult);
        int nImpactTime = GetWeaponImpactTime(nMeleeAnimation, nAttack);
        int bStateApplied = 0;
        ResolveMeleeSpecialAttack(nAttack, nAttacks, pTarget, nImpactTime, &bStateApplied);
        if (IsAttackResultHit(pAttack->m_nAttackResult) &&
            pTarget->AsSWSCreature() != NULL && !bStateApplied)
        {
            if ((pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_UNARMED ||
                 pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED) &&
                nAttack == 0)
            {
                int nChance;
                int bHasFeat = 1;
                if (m_pStats->HasFeat(FEAT_UNARMED_FORCE_PUSH_3))
                    nChance = UNARMED_FORCE_PUSH_CHANCE_3;
                else if (m_pStats->HasFeat(FEAT_UNARMED_FORCE_PUSH_2))
                    nChance = UNARMED_FORCE_PUSH_CHANCE_2;
                else if (m_pStats->HasFeat(FEAT_UNARMED_FORCE_PUSH_1))
                    nChance = UNARMED_FORCE_PUSH_CHANCE_1;
                else
                    bHasFeat = 0;
                if (bHasFeat)
                {
                    CGameEffect* pPushEffect = new CGameEffect(1);
                    pPushEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEPUSHED;
                    pPushEffect->m_nSubType =
                        (pPushEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pPushEffect->m_fDuration = 0.1f;
                    pPushEffect->SetCreator(GetId());
                    CSWSCreature* pCreature = pTarget->AsSWSCreature();
                    int nDC = m_pStats->GetLevel(0) + UNARMED_SAVE_BASE_DC;
                    if (pCreature->m_pStats->GetEffectLinkImmunity(pPushEffect) == 0 &&
                        pCreature->SavingThrowRoll(SAVING_THROW_FORT, nDC, 0,
                                                  GetId(), 1, 0, 1) == 0 &&
                        rand() % 100 < nChance)
                    {
                        pAttack->m_lstEffects.Add(pPushEffect);
                        pAttack->m_Extra4.m_nState = CSWSEffectListHandler::STATE_KNOCKDOWN;
                    }
                    else
                    {
                        delete pPushEffect;
                    }
                    pAttack->m_Extra4.m_nUnknown14 = 1;
                }
            }
        }
        if (IsAttackResultHit(pAttack->m_nAttackResult))
        {
            ResolveDamage(pTarget, bUseAttackResult, nDamage);
            CSWSCombatAttackData* pDamageAttack =
                m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
            if (pDamageAttack->GetBaseDamage() > 0)
            {
                nTotalDamage += pDamageAttack->GetBaseDamage();
            }
            ResolvePostMeleeDamage(pTarget->AsSWSCreature());
        }
        pAttack->ResolveAttackToMeleeSubAttack(nAttack, nImpactTime);
        SetAttackResult(pAttack->m_nAttackResult);
    }
    if (nTotalDamage >= pTarget->GetCurrentHitPoints(0))
    {
        if (m_pStats->m_c120 != -1 && m_pStats->m_c120 < ATTACK_KILL_COUNT_LIMIT)
        {
            m_pStats->m_c120++;
        }
    }
    m_nLastSpecialAttack = pAttack->m_nAttackType;
    ResolveMeleeAnimations(nAttack, nAttacks, pTarget, nAnimation,
                           bUseAttackResult);
    if (bUseAttackResult)
    {
        pAttack->m_nUnknownB8 = 1;
        pAttack->m_nUnknownBC = nAttackResult;
    }
}
// PC 0x0060BA40
void CSWSCreature::ResolveAttack(unsigned long oidTarget, int nA, int nB,
                                 int nC, int nD, int nE, int nF)
{
    CGameObject* pObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);

    if (pObject == NULL ||
        (pObject->AsSWSCreature() == NULL && pObject->AsSWSDoor() == NULL &&
         pObject->AsSWSPlaceable() == NULL))
    {
        m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
        m_pcCombatRound->SetPauseTimer(0, 0);
        SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
        return;
    }

    CSWSObject* pTarget = pObject->AsSWSObject();
    m_oidAttackTarget = oidTarget;
    ActivityManager(ACTIVITY_ACTION_COUNTERSPELL);

    CSWSItem* pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    int bRanged = 0;
    if (pItem != NULL)
    {
        bRanged = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())
                      ->m_nRangedWeapon > 0;
    }
    if (bRanged)
    {
        ResolveRangedAttack(pTarget, nA, nB, nC, nD, nE, nF);
    }
    else
    {
        ResolveMeleeAttack(pTarget, nA, nB, nC, nD, nE, nF);
    }

    if (pTarget->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pTarget->AsSWSCreature();
        if (m_nAttackedPartyLeaderState != 0 &&
            (pCreature->m_pStats->m_bIsPC ||
             g_pAppManager->GetServerExoApp()->GetPartyTable()->GetIsLeader(
                 pCreature->GetId())))
        {
            m_nAttackedPartyLeaderState = -1;
        }
    }
}
// PC 0x0060BBB0
int CSWSCreature::AIActionPhysicalAttacks(CSWSObjectActionNode* pAction)
{
    Vector vTarget;
    int bEngaged;
    float fMaxRange = 0.0f;
    int bCheckRange = 1;
    m_pPathfindInformation->m_fTargetUseRange = 0.0f;
    m_pPathfindInformation->m_oidUseTarget = OBJECT_INVALID;
    m_nAttackMovementTimer = ATTACK_MOVEMENT_TIMEOUT;

    int nAttackRollResult = CSWSCombatAttackData::ATTACK_RESULT_MISS;
    int nAttackResult = 0;
    int nDamage = 0;
    int bUseAttackResult = (int)pAction->m_apParam[0];
    unsigned long oidTarget = (unsigned long)pAction->m_apParam[1];
    int nActionType = (int)pAction->m_apParam[2];
    int nAnimation = (int)pAction->m_apParam[3];
    int nAnimationTime = (int)pAction->m_apParam[4];
    int nAttacks = (int)pAction->m_apParam[5];
    unsigned short nFeat = (unsigned short)(unsigned long)pAction->m_apParam[6];
    if (bUseAttackResult)
    {
        nAttackResult = (int)pAction->m_apParam[7];
        nAttackRollResult = (int)pAction->m_apParam[8];
        nDamage = (int)pAction->m_apParam[9];
    }

    if (GetInParty())
    {
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        int nMember = pClient->GetSWParty()->GetIndex(pClient->ServerToClientObjectId(GetId()));
        if (nMember >= 0)
        {
#ifdef _XBOX
            CSWPartyMemberData* pMember = pClient->GetSWParty()->GetMemberData(nMember); pMember->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_ENGAGED);
#else
            CSWParty* pParty = pClient->GetSWParty(); EXOXBOXASSERT(nMember >= 0 && nMember < 3); pParty->m_aPartyMemberData[nMember].SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_ENGAGED);
#endif
        }
    }

    if (GetDead() || GetIsPCDying() || GetDeadTemp() ||
        !(m_nAIStateFlags & AI_STATE_CAN_ATTACK) ||
        !(m_nAIStateFlags & AI_STATE_CAN_USE_HANDS) || oidTarget == GetId())
    {
        SetLockOrientationToObject(OBJECT_INVALID, 0);
        SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
        SetCombatMode(0, 1);
        m_pcCombatRound->ClearAllSpecialAttacks();
        return ACTION_FAILED;
    }

    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    CSWSObject* pTarget = NULL;
    int bTargetDetected = 0;
    if (pObject != NULL && pObject->AsSWSObject() != NULL)
    {
        pTarget = pObject->AsSWSObject();
        int bDeadTemp = 0;
        if (pTarget->AsSWSCreature() != NULL)
            bDeadTemp = pTarget->AsSWSCreature()->GetDeadTemp();
        if (!pTarget->GetDead() && !bDeadTemp)
            bTargetDetected = HasTargetBeenDetected(oidTarget, 1);
    }
    int bForceJump = 0;
    if (bTargetDetected)
    {
        vTarget = pTarget->m_vPosition;
        CSWSArea* pTargetArea = pTarget->GetArea();
        int bChoreographed = GetCombatRound()->ResolveChoreographedState(oidTarget);
        if (bChoreographed)
            nAnimation = CREATURE_ANIMATION_MELEE_SEQUENCE;
        if (pTarget->AsSWSDoor() != NULL)
        {
            float fUseRange = 0.0f;
            m_pPathfindInformation->m_oidUseTarget = oidTarget;
            GetUseRange(oidTarget, vTarget, fUseRange, 1);
            m_pPathfindInformation->m_fTargetUseRange = fUseRange;
        }
        fMaxRange = MaxAttackRange(oidTarget, bChoreographed, 0);
        float fDesiredRange = DesiredAttackRange(oidTarget, bChoreographed);
        int bLineOfSight = 0;
        unsigned long oidHit = OBJECT_INVALID;
        Vector vStart = m_vPosition;
        vStart.m_fZ += NWSCREATURE_ATTACK_SIGHT_HEIGHT;
        Vector vEnd;
        if (pTarget->m_nObjectType != CGameObject::OBJECT_TYPE_DOOR &&
            pTarget->m_nObjectType != CGameObject::OBJECT_TYPE_PLACEABLE)
            vEnd = pTarget->m_vPosition;
        else
        {
            float fUseRange;
            GetUseRange(pTarget->GetId(), vEnd, fUseRange, 0);
            vTarget = vEnd;
        }
        Vector vHit;
        vEnd.m_fZ += NWSCREATURE_ATTACK_SIGHT_HEIGHT;
        if (GetArea() != NULL)
        {
            vStart.m_fZ += NWSCREATURE_ATTACK_SIGHT_ENDPOINT_MARGIN;
            vEnd.m_fZ -= NWSCREATURE_ATTACK_SIGHT_ENDPOINT_MARGIN;
            bLineOfSight = GetArea()->ClearLineOfSight(
                vStart, vEnd, &vHit, &oidHit, GetId(), oidTarget,
                RangeWeaponEquipped(), 0);
            if (!RangeWeaponEquipped())
            {
                if (bLineOfSight)
                    pTarget->AsSWSCreature();
            }
            if (!bLineOfSight)
            {
                if (equal(m_vLastAttackPosition, m_vPosition) && m_nAreaWaitState)
                {
                    SendFeedbackMessage(FEEDBACK_ATTACK_LINE_OF_SIGHT_BLOCKED, NULL);
                    m_vLastAttackPosition = Vector();
                    m_oidAttackTarget = OBJECT_INVALID;
                    unsigned long oidNewTarget = GetNearestEnemy(
                        MaxAttackRange(GetId(), 0, 1), oidTarget, OBJECT_INVALID);
                    if (oidNewTarget != OBJECT_INVALID)
                    {
                        CGameObject* pNewTarget =
                            g_pAppManager->GetServerExoApp()->GetGameObject(oidNewTarget);
                        if (pNewTarget != NULL &&
                            pNewTarget->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                        {
                            InitializeNewAttackTarget(oidNewTarget, pAction);
                            return ACTION_IN_PROGRESS;
                        }
                    }
                    CancelAllCombatModes(1);
                    return ACTION_COMPLETE;
                }
                m_vLastAttackPosition = m_vPosition;
            }
            else
                m_vLastAttackPosition = Vector();
        }

        Vector vDifference = m_vPosition - vTarget;
        Vector vRetreat;
        float fDistanceSquared = vDifference.m_fX * vDifference.m_fX +
                                 vDifference.m_fY * vDifference.m_fY;
        if (fDistanceSquared < fDesiredRange * fDesiredRange - NWSCREATURE_ATTACK_RANGE_TOLERANCE_SQUARED)
        {
            Vector vDirection = normalize(Vector(-vDifference.m_fX, -vDifference.m_fY, 0.0f));
            vRetreat = pTarget->m_vPosition - vDirection * fDesiredRange;
            CWalkHitInfo cHit;
            float fRadius = m_pPathfindInformation->m_fStepScale;
            float fHalfStep = m_pPathfindInformation->m_fHalfStep;
            if (g_pAppManager->GetServerExoApp()->TestDirectLine(
                    GetId(), GetArea()->GetResRef(), m_vPosition, vRetreat,
                    fRadius, fHalfStep, 1, &cHit) != CSWSArea::DIRECT_LINE_CLEAR)
            {
                vRetreat = cHit.m_vHitPosition;
                fDesiredRange = magnitude(pTarget->m_vPosition - vRetreat);
            }
        }

        if (!(pTargetArea == GetArea() && !(fMaxRange * fMaxRange < fDistanceSquared) &&
              (!(fDesiredRange * fDesiredRange - NWSCREATURE_ATTACK_RANGE_TOLERANCE_SQUARED > fDistanceSquared) || GetInParty()) &&
              bLineOfSight))
        {
            int bRanged = RangeWeaponEquipped();
            int bTooClose = 0;
            if (fDesiredRange * fDesiredRange - NWSCREATURE_ATTACK_RANGE_TOLERANCE_SQUARED > fDistanceSquared)
                bTooClose = 1;
            if (m_bNewAttackTarget)
                goto AttackFailed;
            unsigned long oidArea;
            if (pTargetArea != NULL)
                oidArea = pTargetArea->GetId();
            else
            {
                if (pTarget->AsSWSCreature() == NULL || m_oidSpawnedBy != OBJECT_INVALID)
                    goto AttackFailed;
                CSWSCreature* pTargetCreature = pTarget->AsSWSCreature();
                oidArea = pTargetCreature->m_oidDisappearTarget;
                vTarget = pTarget->AsSWSCreature()->m_vDisappearPosition;
            }

            if (!bLineOfSight ||
                !(bForceJump = ResolveMoveToForceJump(pTarget, nFeat)))
            {
                if (pObject->AsSWSDoor() != NULL)
                {
                    float fUseRange;
                    GetUseRange(oidTarget, vTarget, fUseRange, 1);
                    fDesiredRange = fUseRange;
                    Vector vActionPoint = pObject->AsSWSDoor()->GetNearestActionPoint(m_vPosition, 0);
                    vTarget = vActionPoint;
                    m_pPathfindInformation->m_oidUseTarget = oidTarget;
                    m_pPathfindInformation->m_fTargetUseRange = fUseRange;
                }
                else if (pObject->AsSWSPlaceable() != NULL)
                {
                    float fUseRange;
                    GetUseRange(oidTarget, vTarget, fUseRange, 1);
                    fDesiredRange = fUseRange;
                    Vector vActionPoint = pObject->AsSWSPlaceable()->GetNearestActionPoint(m_vPosition);
                    vTarget = vActionPoint;
                    bTooClose = 0;
                }
                AddActionToFront(ACTION_PHYSICAL_ATTACKS, pAction->m_nGroupId,
                    ACTION_PARAM_INTEGER, &bUseAttackResult, ACTION_PARAM_OBJECT, &oidTarget,
                    ACTION_PARAM_INTEGER, &nActionType, ACTION_PARAM_INTEGER, &nAnimation,
                    ACTION_PARAM_INTEGER, &nAnimationTime, ACTION_PARAM_INTEGER, &nAttacks,
                    ACTION_PARAM_INTEGER, &nFeat, ACTION_PARAM_INTEGER, &nAttackResult,
                    ACTION_PARAM_INTEGER, &nAttackRollResult, ACTION_PARAM_INTEGER, &nDamage,
                    0, NULL, 0, NULL, 0, NULL);
                int bRun = 1;
                if (!bTooClose)
                {
                    int bOption = 0;
                    float fRange = fDesiredRange;
                    if (bRanged)
                        fRange = fMaxRange;
                    AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, pAction->m_nGroupId,
                        ACTION_PARAM_OBJECT, &oidTarget, ACTION_PARAM_INTEGER, &bRun,
                        ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_FLOAT, &fMaxRange,
                        ACTION_PARAM_INTEGER, &bCheckRange, ACTION_PARAM_INTEGER, &bOption,
                        ACTION_PARAM_INTEGER, &bRanged,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                    AddActionToFront(ACTION_CHANGE_FACING_OBJECT, pAction->m_nGroupId,
                        ACTION_PARAM_OBJECT, &oidTarget,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                }
                if (bTooClose)
                {
                    Vector vOrientation;
                    AddMoveToPointActionToFront(pAction->m_nGroupId, vRetreat, oidArea,
                        OBJECT_INVALID, 0, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0,
                        vOrientation, 0, 0, 0);

                    if (GetInParty() && fDistanceSquared > NWSCREATURE_ATTACK_MOVE_MESSAGE_DISTANCE_SQUARED)
                    {
                        CClientExoApp* pClient = g_pAppManager->GetClientExoApp(); int nMember = pClient->GetSWParty()->GetIndex(pClient->ServerToClientObjectId(GetId()));
                        if (nMember >= 0)
                        {
#ifdef _XBOX
                            CSWPartyMemberData* pMember = pClient->GetSWParty()->GetMemberData(nMember); pMember->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_CLOSING_TO_ATTACK);
#else
                            CSWParty* pParty = pClient->GetSWParty(); pParty->m_aPartyMemberData[nMember].SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_CLOSING_TO_ATTACK);
#endif
                        }
                    }
                }
                else
                {
                    float fMoveRange = fDesiredRange;
                    if (bRanged)
                        fMoveRange = fMaxRange;
                    if (pTarget->AsSWSCreature() != NULL)
                    {
                        m_pPathfindInformation->m_fTargetUseRange = fDesiredRange -
                            pTarget->AsSWSCreature()->GetPathfindInformation()->GetCreaturePersonalSpace();
                        m_pPathfindInformation->m_oidUseTarget = oidTarget;
                    }
                    Vector vOrientation;
                    AddMoveToPointActionToFront(pAction->m_nGroupId, vTarget, oidArea,
                        oidTarget, bRun, fMoveRange, 0.0f, 0, 0, 0, 0, 0, 0,
                        vOrientation, 0, 0, bRanged);

                    if (GetInParty() && fDistanceSquared > NWSCREATURE_ATTACK_MOVE_MESSAGE_DISTANCE_SQUARED)
                    {
                        CClientExoApp* pClient = g_pAppManager->GetClientExoApp(); int nMember = pClient->GetSWParty()->GetIndex(pClient->ServerToClientObjectId(GetId()));
                        if (nMember >= 0)
                        {
#ifdef _XBOX
                            CSWPartyMemberData* pMember = pClient->GetSWParty()->GetMemberData(nMember); pMember->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_CLOSING_TO_ATTACK);
#else
                            CSWParty* pParty = pClient->GetSWParty(); pParty->m_aPartyMemberData[nMember].SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_CLOSING_TO_ATTACK);
#endif
                        }
                    }
                }
                goto Done;
            }
        }

        if (m_pcCombatRound != NULL)
        {
            if (g_pAppManager->GetServerExoApp()->GetPauseState(CServerExoAppInternal::PAUSE_TYPE_COMBAT_MASK) || m_pcCombatRound->m_bRoundPaused)
                return ACTION_IN_PROGRESS;
            bEngaged = m_pcCombatRound->ResolveEngagedState(oidTarget);
            int bMaster = GetCombatRound()->ResolveMasterState(oidTarget, bEngaged);
            if (bEngaged)
            {
                if (IsCreature() || (pTarget->AsSWSCreature() != NULL &&
                                    pTarget->AsSWSCreature()->IsCreature()))
                    nAnimation = CREATURE_ANIMATION_MELEE_SEQUENCE;
                if (bMaster)
                {
                    m_pcCombatRound->StartCombatRound(oidTarget, bEngaged, bMaster,
                        nFeat, bUseAttackResult);
                    CSWSCreature* pCreature =
                        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
                    if (pCreature != NULL)
                    {
                        pCreature->m_pcCombatRound->StartCombatRound(GetId(), bEngaged,
                            0, 0, bUseAttackResult);
                        pCreature->m_bResolveInitiative = 1;
                    }
                }
            }
            else
            {
                nAnimation = CSWSCombatRound::ATTACK_ANIMATION_DEFAULT;
                m_pcCombatRound->StartCombatRound(oidTarget, 0, bMaster, nFeat, bUseAttackResult);
            }
            if (!m_pcCombatRound->m_bRoundStarted)
                return ACTION_IN_PROGRESS;
            if ((nActionType == CSWSCombatRoundAction::ACTION_SPECIAL_ATTACK || bForceJump) &&
                m_nGuardStanceFeat == 0 && m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) != NULL)
                m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack)->m_nAttackType =
                    (unsigned short)nFeat;
            SetAnimation(nAnimation);
            SetAttemptedAttackTarget(oidTarget);
            m_pcCombatRound->SetRoundPaused(1, GetId());
            m_pcCombatRound->SetPauseTimer(nAnimationTime, 0);
            m_pcCombatRound->IncrementNumActionsTaken(1);
            if (bEngaged)
            {
                CSWSCreature* pCreature;
                if (bMaster)
                {
                    pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                        m_pcCombatRound->GetMasterTarget());
                    if (pCreature != NULL)
                    {
                        pCreature->m_pcCombatRound->SetRoundPaused(1, pCreature->m_pcCombatRound->m_oidMaster);
                        pCreature->m_pcCombatRound->SetPauseTimer(nAnimationTime, 1);
                    }
                }
                else
                {
                    pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
                    if (pCreature != NULL)
                    {
                        pCreature->m_pcCombatRound->SetRoundPaused(1, pCreature->m_pcCombatRound->m_oidMasterTarget);
                        pCreature->m_pcCombatRound->SetPauseTimer(nAnimationTime, 0);
                    }
                }
            }
            unsigned long oidNewTarget = m_pcCombatRound->m_oidNewAttackTarget;
            if (oidNewTarget != OBJECT_INVALID)
            {
                InitializeNewAttackTarget(oidNewTarget, pAction);
                m_pcCombatRound->m_oidNewAttackTarget = OBJECT_INVALID;
                oidTarget = oidNewTarget;
            }
            SetLockOrientationToObject(oidTarget, 0);
            ResolveAttack(oidTarget, nAttacks, nAnimationTime, bUseAttackResult,
                          nAttackResult, nAttackRollResult, nDamage);
            return ACTION_COMPLETE;
        }
    }
    else
        return ACTION_COMPLETE;
AttackFailed:
    CancelAllCombatModes(1);
    return ACTION_FAILED;
Done:
    return ACTION_COMPLETE;
}
// PC 0x00604E00
int CSWSCreature::AIActionCombat(CSWSObjectActionNode* pNode)
{
    int bRoundPaused = GetCombatRound()->m_bRoundPaused;
    if (bRoundPaused ||
        g_pAppManager->GetServerExoApp()->GetPauseState(
            CServerExoAppInternal::PAUSE_TYPE_COMBAT_MASK))
    {
        return ACTION_IN_PROGRESS;
    }

    if (GetCombatRound()->m_pCurrentAction != NULL)
    {
        delete GetCombatRound()->m_pCurrentAction;
        GetCombatRound()->m_pCurrentAction = NULL;
    }

    if (GetCombatRound()->GetActionPending() &&
        GetCombatRound()->GetNumActionsTaken() < 1)
    {
        pNode->m_apParam[0] = NULL;
        CSWSCombatRoundAction* pAction = GetCombatRound()->GetAction();
        if (pAction == NULL)
            return ACTION_IN_PROGRESS;

        unsigned short nAnimation = pAction->m_nAnimation;
        unsigned long oidTarget = pAction->m_oidTarget;
        int nAnimationTime = pAction->m_nAnimationTime;
        int nAttacks = pAction->m_nNumAttacks;
        unsigned char nActionType = pAction->m_nActionType;
        unsigned short nSpecialAttack = pAction->m_nSpecialAttack;
        int bCutsceneAttack = pAction->m_bCutsceneAttack;

        GetCombatRound()->SetCurrentCombatRoundAction(pAction);
        GetCombatRound()->SetCurrentAction(nActionType);
        if (nActionType != CSWSCombatRoundAction::ACTION_ITEM_CAST_SPELL &&
            nActionType != CSWSCombatRoundAction::ACTION_STANCE &&
            pAction->m_nSpellId != 181 && pAction->m_nSpellId != 200 &&
            pAction->m_nSpellId != 182 && pAction->m_nSpellId != 184 &&
            pAction->m_nSpellId != 201 && pAction->m_nSpellId != 269)
        {
            SetActivity(ACTIVITY_STEALTH, 0, 0);
        }

        switch (nActionType)
        {
        case CSWSCombatRoundAction::ACTION_SWITCH_WEAPONS:
            AsSWSObject()->AddSwitchWeaponsAction(1);
            return ACTION_IN_DELAY;
        case CSWSCombatRoundAction::ACTION_ATTACK:
        case CSWSCombatRoundAction::ACTION_SPECIAL_ATTACK:
        {
            unsigned long nDay = 0;
            unsigned long nTime = 0;
            g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
                &nDay, &nTime);
            if (bCutsceneAttack)
            {
                AddAttackActions(oidTarget, 0, 0, 0, 0, 1, 1, nActionType,
                    nAnimation, nAnimationTime, nAttacks, 1,
                    pAction->m_nCutsceneArg1, pAction->m_nCutsceneArg2,
                    pAction->m_nCutsceneArg3);
            }
            else
            {
                AddAttackActions(oidTarget, nSpecialAttack, 0, 0, 0, 1, 1,
                    nActionType, nAnimation, nAnimationTime, nAttacks, 0,
                    0, CSWSCombatRound::ATTACK_ACTION_OPTION_DEFAULT, 0);
            }
            return ACTION_IN_DELAY;
        }
        case CSWSCombatRoundAction::ACTION_PAUSE:
            GetCombatRound()->SetRoundPaused(1, GetId());
            GetCombatRound()->SetPauseTimer(nAnimationTime, 0);
            return ACTION_IN_PROGRESS;
        case CSWSCombatRoundAction::ACTION_EQUIP_ITEM:
            GetCombatRound()->SetRoundPaused(1, GetId());
            GetCombatRound()->SetPauseTimer(nAnimationTime, 0);
            RunEquip(oidTarget, pAction->m_nInventorySlot, pAction->m_nEquipFlags);
            return ACTION_IN_PROGRESS;
        case CSWSCombatRoundAction::ACTION_UNEQUIP_ITEM:
            GetCombatRound()->SetRoundPaused(1, GetId());
            GetCombatRound()->SetPauseTimer(nAnimationTime, 0);
            RunUnequip(oidTarget, pAction->m_oidTargetRepository, 1, NULL,
                       pAction->m_nEquipFlags);
            return ACTION_IN_PROGRESS;
        case CSWSCombatRoundAction::ACTION_CAST_SPELL:
            AddCastSpellActions(pAction->m_nSpellId,
                pAction->m_aUnmapped28[0], pAction->m_aUnmapped28[1],
                pAction->m_aUnmapped28[2], pAction->m_aUnmapped28[3],
                pAction->m_vTarget, pAction->m_oidSpellTarget,
                pAction->m_nAttackFlags, pAction->m_nUnknown48, 1,
                pAction->m_nUnknown4C, pAction->m_nUnknown50,
                pAction->m_nEquipFlags, pAction->m_nSpecialAttack,
                pAction->m_nUnknown60, 1);
            break;
        case CSWSCombatRoundAction::ACTION_ITEM_CAST_SPELL:
        {
            CGameObject* pTarget = g_pAppManager->GetServerExoApp()->
                GetGameObject(pAction->m_oidSpellTarget);
            if (pTarget != NULL && pTarget->AsSWSObject() != NULL &&
                pTarget->AsSWSCreature() != NULL &&
                pTarget->AsSWSCreature()->GetDead())
                return ACTION_IN_PROGRESS;

            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->
                GetItemByGameObjectID(pAction->m_oidSpellItem);
            if (pItem != NULL)
            {
                CSWItemProperty* pProperty = pItem->GetActiveProperty(
                    pAction->m_nUnknown68);
                if (pProperty != NULL)
                {
                    int nSpell = pProperty->m_nSubtype;
                    int nForceHostile = 0;
                    g_pRules->Get2DArrays()->GetSpells()->GetINTEntry(
                        nSpell, "FORCEHOSTILE", &nForceHostile);
                    int nGrenadeSpell = ConvertGrenadeSpellID(nSpell);
                    if (nGrenadeSpell != nSpell)
                        nSpell = nGrenadeSpell;
                    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
                    if (pSpell != NULL && pSpell->m_bHostileSetting)
                        SetActivity(ACTIVITY_STEALTH, 0, 0);
                    if (nForceHostile <= 0 && pAction->m_oidSpellTarget != GetId() &&
                        pSpell->m_bHostileSetting &&
                        pItem->GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_HOSTILE_USABLE_49 &&
                        pItem->GetBaseItem()->m_nItemType != CSWBaseItem::ITEM_TYPE_HOSTILE_USABLE)
                        pAction->m_oidSpellTarget = OBJECT_INVALID;
                }
            }
            AddItemCastSpellActions(pAction->m_oidSpellItem,
                pAction->m_nUnknown68, pAction->m_nUnknown6C,
                pAction->m_vTarget, pAction->m_oidSpellTarget,
                pAction->m_nAttackFlags, pAction->m_nUnknown48, 1, 1);
            break;
        }
        case CSWSCombatRoundAction::ACTION_CUTSCENE_MOVE:
        {
            unsigned long oidArea = GetArea()->GetId();
            {
                CGameObject* pTarget = g_pAppManager->GetServerExoApp()->
                    GetGameObject(pAction->m_oidTarget);
                if (pTarget == NULL || pTarget->AsSWSObject() == NULL ||
                    pAction->m_oidTarget == OBJECT_INVALID)
                    goto MoveToPoint;
                int bUnknown2 = 0;
                int bUnknown1 = 0;
                float fRange = 0.2f;
                int bUsePath = 1;
                AddActionToFront(ACTION_CHECK_MOVE_TO_OBJECT_RADIUS, pNode->m_nGroupId,
                    ACTION_PARAM_OBJECT, &pAction->m_oidTarget,
                    ACTION_PARAM_INTEGER, &pAction->m_nEquipFlags,
                    ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_FLOAT, &fRange,
                    ACTION_PARAM_INTEGER, &bUsePath, ACTION_PARAM_INTEGER, &bUnknown1,
                    ACTION_PARAM_INTEGER, &bUnknown2,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                Vector vOrientation;
                CSWSObject* pObject = pTarget->AsSWSObject();
                AddMoveToPointActionToFront(pNode->m_nGroupId,
                    pObject->m_vPosition, oidArea, pAction->m_oidTarget,
                    pAction->m_nEquipFlags, 0.2f, 0.1f, 0, 0, 0, 0, 0, 0,
                    vOrientation, 0, 0, 0);
                break;
            }
MoveToPoint:
            {
                int bUnknown = 0;
                float fRange = 0.2f;
                unsigned long oidMoveTarget = OBJECT_INVALID;
                AddActionToFront(ACTION_CHECK_MOVE_TO_POINT_RADIUS, pNode->m_nGroupId,
                    ACTION_PARAM_FLOAT, &pAction->m_vTarget.m_fX,
                    ACTION_PARAM_FLOAT, &pAction->m_vTarget.m_fY,
                    ACTION_PARAM_FLOAT, &pAction->m_vTarget.m_fZ,
                    ACTION_PARAM_OBJECT, &oidArea, ACTION_PARAM_OBJECT, &oidMoveTarget,
                    ACTION_PARAM_INTEGER, &pAction->m_nEquipFlags,
                    ACTION_PARAM_FLOAT, &fRange, ACTION_PARAM_INTEGER, &bUnknown,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                Vector vOrientation;
                AddMoveToPointActionToFront(pNode->m_nGroupId,
                    pAction->m_vTarget, oidArea, OBJECT_INVALID,
                    pAction->m_nEquipFlags, 0.2f, 0.1f, 0, 0, 0, 0, 0, 0,
                    vOrientation, 0, 0, 0);
            }
            break;
        }
        case CSWSCombatRoundAction::ACTION_STANCE:
            AddStanceActions(nAttacks, oidTarget, 0, 0, 1, 1);
            return ACTION_IN_DELAY;
        default:
            return ACTION_IN_PROGRESS;
        }
        if (GetCombatRound()->m_pCurrentAction != NULL)
            delete GetCombatRound()->m_pCurrentAction;
        GetCombatRound()->m_pCurrentAction = NULL;
        return ACTION_IN_DELAY;
    }

    GetCombatRound()->GetActionPending();
    if (!GetCombatRound()->m_bRoundStarted)
        return ACTION_COMPLETE;
    return ACTION_IN_PROGRESS;
}
// PC 0x006055B0
void CSWSCreature::CreateNewAttackActions(int bFindNewTarget,
                                         int bCheckCleaveRange,
                                         unsigned long oidKilledCreature)
{
    int bValidTarget = 0;
    int bFindTarget = 0;
    unsigned long oidTarget;
    if (bCheckCleaveRange)
    {
        oidTarget = m_oidAttemptedSpellTarget;
    }
    else
    {
        oidTarget = m_pcCombatRound->m_oidUnknown9CC;
    }

    if (oidTarget != OBJECT_INVALID)
    {
        if (m_pcCombatRound->m_pScheduledActions->GetHeadPos() == NULL)
        {
            bValidTarget = 1;
            CGameObject* pObject =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
            if (pObject != NULL)
            {
                if (pObject->AsSWSCreature() != NULL)
                {
                    CSWSCreature* pTarget = pObject->AsSWSCreature();
                    if (!pTarget->GetDead() && !pTarget->GetDeadTemp() &&
                        !GetDead() && !GetDeadTemp())
                    {
                        if (GetReputation(oidTarget, 0) >
                            CSWSObject::MAX_HOSTILE_REPUTATION)
                        {
                            if (oidTarget == GetId())
                            {
                                bFindTarget = 1;
                            }
                        }
                        else if (bCheckCleaveRange)
                        {
                            Vector vDelta = pTarget->m_vPosition - m_vPosition;
                            float fRange = MaxCleaveRange(pTarget->GetId(), 0, 1);
                            if (vDelta.m_fX * vDelta.m_fX +
                                vDelta.m_fY * vDelta.m_fY +
                                vDelta.m_fZ * vDelta.m_fZ > fRange * fRange)
                            {
                                bValidTarget = 0;
                            }
                        }
                    }
                    else
                    {
                        bValidTarget = 0;
                    }
                }
            }
            else
            {
                m_pcCombatRound->m_oidUnknown9CC = OBJECT_INVALID;
                bValidTarget = 0;
            }
        }
    }

    if ((oidTarget == OBJECT_INVALID &&
         m_pcCombatRound->m_pScheduledActions->GetHeadPos() == NULL) ||
        bFindNewTarget)
    {
        bValidTarget = 1;
        bFindTarget = 1;
    }

    if (bFindTarget)
    {
        CSWSCreature* pKilledCreature =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                oidKilledCreature);
        unsigned long oidNewTarget = OBJECT_INVALID;
        if (pKilledCreature != NULL)
        {
            oidNewTarget = pKilledCreature->GetNearestEnemy(
                MaxCleaveRange(GetId(), 0, 1), GetId(), GetId());
        }
        if (oidNewTarget == OBJECT_INVALID)
        {
            oidNewTarget = GetNearestEnemy(MaxCleaveRange(GetId(), 0, 1),
                                           GetId(), OBJECT_INVALID);
        }
        if (oidNewTarget != OBJECT_INVALID)
        {
            CGameObject* pObject =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidNewTarget);
            if (pObject->AsSWSCreature() == NULL)
            {
                CancelAllCombatModes(1);
                return;
            }
            InitializeNewAttackTarget(oidNewTarget, NULL);
            oidTarget = oidNewTarget;
            g_pAppManager->GetClientExoApp()->SetLastTarget(
                g_pAppManager->GetClientExoApp()->ServerToClientObjectId(
                    oidNewTarget), 1);
        }
        else
        {
            CancelAllCombatModes(1);
            return;
        }
    }

    if (bValidTarget)
    {
        if (m_pcCombatRound->m_cB1C == 13)
        {
            m_pcCombatRound->AddStanceAction(0, oidTarget, 0);
        }
        else
        {
            m_pcCombatRound->AddAttackAction(oidTarget, 0, 0,
                                            CSWSCombatRound::ATTACK_ACTION_TIME, 1);
            if (m_oidAttemptedMoveToTarget == oidTarget)
            {
                m_oidAttemptedMoveToTarget = OBJECT_INVALID;
            }
        }
    }
}
// PC 0x006058B0
void CSWSCreature::ResolveRangedAnimations(CSWSObject* pTarget,
                                         int nAnimationLength,
                                         int bForceReaction)
{
    if (pTarget != NULL)
    {
        int nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(nCurrentAttack);
        pAttack->m_nAnimationLength = (unsigned short)nAnimationLength;
        pAttack->m_nReactionDelay = 0;
        m_pcCombatRound->DecrementRoundLength(nAnimationLength, 0);

        CSWSCreature* pCreature = pTarget->AsSWSCreature();
        if (pCreature != NULL)
        {
            int bDebilitated = 0;
            if ((pCreature->m_bDebilitated > 0 &&
                 pCreature->m_bDebilitated != CSWSEffectListHandler::STATE_CONFUSION &&
                 pCreature->m_bDebilitated != CSWSEffectListHandler::STATE_VARIANT_16) ||
                pCreature->GetDeadTemp() != 0)
            {
                bDebilitated = 1;
            }
            if (pCreature->m_c110C == 1)
            {
                bDebilitated = 1;
            }
            m_pcCombatRound->m_nUnknown948 = bDebilitated;
            if (!bForceReaction && !pCreature->IsLightsaberEquipped() &&
                !pCreature->IsSpellCastingAnimation(CREATURE_ANIMATION_CURRENT) &&
                pCreature->m_pcCombatRound->m_bEngaged)
            {
                pAttack->m_nReactionAnimation = CREATURE_ANIMATION_RANGED_REACTION;
            }
            else
            {
                pAttack->m_nReactionAnimation = CREATURE_ANIMATION_COMBAT_READY;
            }

            int bTargetDebilitated = 0;
            if ((pCreature->m_bDebilitated > 0 &&
                 pCreature->m_bDebilitated != CSWSEffectListHandler::STATE_CONFUSION &&
                 pCreature->m_bDebilitated != CSWSEffectListHandler::STATE_VARIANT_16) ||
                pCreature->GetDeadTemp() != 0)
            {
                bTargetDebilitated = 1;
            }
            if (pCreature->m_c110C == 1)
            {
                bTargetDebilitated = 1;
            }
            if (bTargetDebilitated)
            {
                pAttack->m_nReactionAnimation = CREATURE_ANIMATION_COMBAT_READY;
            }
            else
            {
                unsigned long oidAttacker = GetId();
                CSWSCreature* pTargetCreature = pTarget->AsSWSCreature();
                CSWSCombatRound* pTargetRound = pTargetCreature->m_pcCombatRound;
                if (pTargetRound->CheckActionLengthAtTime(
                        oidAttacker, nAnimationLength, 0) &&
                    m_pcCombatRound->m_bEngaged)
                {
                    SetCombatReactionTime(nCurrentAttack, nAnimationLength, 0);
                    pCreature->SetAnimation(pAttack->m_nReactionAnimation);
                    pCreature->m_pcCombatRound->DecrementRoundLength(
                        nAnimationLength, 0);
                }
            }
        }
    }
}
// PC 0x00605A70
void CSWSCreature::ResolveRangedHit(CSWSObject* pTarget)
{
    if (pTarget != NULL)
    {
        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);

        if (IsAttackResultHit(pAttack->m_nAttackResult))
        {
            pAttack->m_nReactionAnimation = CREATURE_ANIMATION_DAMAGE;
        }
        else if (IsAttackResultParried(pAttack->m_nAttackResult))
        {
            pAttack->m_nReactionAnimation = CREATURE_ANIMATION_PARRY;
        }

        const Vector& vTargetPosition = pTarget->m_vPosition;

        pAttack->m_vRangedTarget = vTargetPosition;

        Vector vDelta;
        vDelta.m_fX = vTargetPosition.m_fX - m_vPosition.m_fX;
        vDelta.m_fY = vTargetPosition.m_fY - m_vPosition.m_fY;
        vDelta.m_fZ = vTargetPosition.m_fZ - m_vPosition.m_fZ;
        pAttack->m_nReactionDelay =
            (unsigned short)(magnitude(vDelta) * 23.809525f);
    }
}
// PC 0x00605B50
void CSWSCreature::ResolveRangedMiss(CSWSObject* pTarget)
{
    if (pTarget != NULL)
    {
        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);

        pAttack->m_vRangedTarget = ResolveProjectileMissLocation(pTarget);
        pAttack->m_nReactionDelay =
            (unsigned short)CalculateProjectileTimeToTarget(
                pAttack->m_vRangedTarget, 0);
    }
}
// PC 0x006035E0
Vector CSWSCreature::ResolveProjectileMissLocation(CSWSObject* pTarget)
{
    Vector vHit;
    if (pTarget != NULL)
    {
        float fMissRadius = 0.6f * 0.6667f;
        float fMissSpread = 1.0f;
        if (pTarget->AsSWSCreature() != NULL)
        {
            fMissRadius = pTarget->AsSWSCreature()
                              ->GetPathfindInformation()->m_fStepScale * 0.6667f;

            CSWSCreature* pTargetCreature = pTarget->AsSWSCreature();
            CSWSCreature* pCombatTarget = pTarget->AsSWSCreature();
            if (pCombatTarget->m_pcCombatRound->GetAttack(
                    pCombatTarget->m_pcCombatRound->m_nCurrentAttack)->m_nAttackResult ==
                CSWSCombatAttackData::ATTACK_RESULT_WIDE_MISS)
            {
                fMissSpread = (float)(rand() % 3 + rand() % 3 + rand() % 3);
            }
        }

        Vector vDelta = m_vPosition - pTarget->m_vPosition;
        Vector vDirection = normalize(vDelta);
        Vector vSide = cross(vDirection, Vector(0.0f, 0.0f, 1.0f));
        if (rand() % 2)
        {
            vSide = -vSide;
        }
        vSide = vSide * fMissSpread;
        Vector vMiss = pTarget->m_vPosition + vSide * fMissRadius;
        float fHeightRange = (vDelta * vDelta) * 1.5f / 100.0f;
        float fRandomHeight = (float)(rand() % 1000) / 1000.0f;
        float fHeightOffset = 1.5f - fHeightRange / 1.5f;
        fHeightOffset += fRandomHeight * fHeightRange * 0.75f;
        vMiss.m_fZ += fHeightOffset;

        Vector vStart = m_vPosition;
        vStart.m_fZ += 1.5f;
        Vector vRay = normalize(vMiss - vStart);
        Vector vEnd = vStart + vRay * 1000.0f;
        Vector vIntersection;
        unsigned long oidHit = OBJECT_INVALID;
        if (GetArea()->ClearLineOfSight(vStart, vEnd, &vIntersection, &oidHit,
                                        GetId(), pTarget->GetId(), 1, 1))
        {
            vHit = vEnd;
        }
        else
        {
            vHit = vIntersection;
        }
    }
    return vHit;
}
// PC 0x00603960
int CSWSCreature::ResolveProjectileMissImpact(const Vector& vImpact, int nDelay)
{
    CGameEffect* pEffect = new CGameEffect(1);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
    pEffect->m_nSubType =
        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
    pEffect->SetCreator(m_nId);
    pEffect->SetInteger(0, CSWSEffectListHandler::VFX_COM_BLASTER_IMPACT_GROUND);
    pEffect->SetFloat(0, vImpact.m_fX);
    pEffect->SetFloat(1, vImpact.m_fY);
    pEffect->SetFloat(2, vImpact.m_fZ);
    unsigned long oidArea = GetArea()->GetId();
    unsigned long oidSelf = m_nId;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    return pServer->GetServerAIMaster()->AddEventDeltaTime(
        0, nDelay, oidSelf, oidArea, CServerAIMaster::EVENT_APPLY_EFFECT, pEffect);
}

int CSWSCreature::GetWeaponImpactHits(int nWeapon)
{
    CExoString sRow(nWeapon);
    int nHits = 0;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pTable = p2DArrays->m_pCombatAnimations;
    CExoString sColumn("Hits");

    pTable->GetINTEntry(sRow, sColumn, &nHits);
    return nHits;
}
// PC 0x00603A60
int CSWSCreature::GetWeaponImpactTime(int nWeapon, int nImpact)
{
    CExoString sColumn(nWeapon);
    int nTime = 0;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pTable = p2DArrays->m_pCombatAnimations;

    pTable->GetINTEntry(sColumn, nImpact + 1, &nTime);
    return nTime;
}
// PC 0x00603AF0
CExoString CSWSCreature::GetWeaponDischargeRow(int nWeapon)
{
    CExoString sRow(nWeapon);
    int bDroid = 0;
    {
        CExoString sColumn("Droid");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pWeaponDischarge);
        C2DA* pTable = p2DArrays->m_pWeaponDischarge;

        pTable->GetINTEntry(sRow, sColumn, &bDroid);
    }

    if (bDroid)
    {
        CExoString sWeapon(nWeapon);
        CExoString sRace;
        CExoString sPrefix;

        {
            CExoString sColumn("RACE");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pAppearance;

            pTable->GetCExoStringEntry(m_cAppearanceInfo.m_nAppearance, sColumn, &sRace);
        }
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pDroidDischarge);
            C2DA* pTable = p2DArrays->m_pDroidDischarge;

            pTable->GetCExoStringEntry(sRace, sWeapon, &sPrefix);
        }
        sRow = sPrefix + "_" + sRow;
    }
    else if (m_cAppearanceInfo.m_nAppearance == 3)
    {
        sRow = CExoString("3_") + CExoString(nWeapon);
    }

    return sRow;
}
// PC 0x00603D70
int CSWSCreature::GetWeaponDischargeShots(int nWeapon)
{
    CExoString sRow = GetWeaponDischargeRow(nWeapon);
    int nShots = 0;
    {
        CExoString sColumn("Shots");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pWeaponDischarge);
        C2DA* pTable = p2DArrays->m_pWeaponDischarge;

        pTable->GetINTEntry(sRow, sColumn, &nShots);
    }
    return nShots;
}
// PC 0x00603E20
int CSWSCreature::GetWeaponHitShots(int nWeapon)
{
    CExoString sRow = GetWeaponDischargeRow(nWeapon);
    int nShots = 0;
    {
        CExoString sColumn("Hits");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pWeaponDischarge);
        C2DA* pTable = p2DArrays->m_pWeaponDischarge;

        pTable->GetINTEntry(sRow, sColumn, &nShots);
    }
    return nShots;
}
// PC 0x00603ED0
int CSWSCreature::GetWeaponDischargeTime(int nWeapon, int nDischarge)
{
    CExoString sRow = GetWeaponDischargeRow(nWeapon);
    int nTime = 0;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pWeaponDischarge);
    C2DA* pTable = p2DArrays->m_pWeaponDischarge;

    pTable->GetINTEntry(sRow, nDischarge + 4, &nTime);
    return nTime;
}
// PC 0x00603F60
int CSWSCreature::GetWeaponDischargeHand(int nWeapon, int nDischarge)
{
    CExoString sRow = GetWeaponDischargeRow(nWeapon);
    CExoString sSwitchMask;
    {
        CExoString sColumn("SwitchMask");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pWeaponDischarge);
        C2DA* pTable = p2DArrays->m_pWeaponDischarge;

        pTable->GetCExoStringEntry(sRow, sColumn, &sSwitchMask);
    }
    return sSwitchMask.SubString(nDischarge, 1).AsINT();
}
// PC 0x00605BF0
void CSWSCreature::SignalRangedDamage(unsigned long oidTarget)
{
    int nAttack = m_pcCombatRound->m_nCurrentAttack;
    CSWSCombatAttackData* pAttack = m_pcCombatRound->GetAttack(nAttack);
    int nTime = pAttack->m_nReactionDelay;
    CSWSCombatSubAttack subAttack = pAttack->GetTopSubAttack();
    int bHit = subAttack.m_nUnknown14;
    int nWeaponHand = subAttack.m_nUnknown2C;
    int bAttacked = subAttack.m_nUnknown10;
    CServerAIMaster* pAIMaster =
        g_pAppManager->GetServerExoApp()->GetServerAIMaster();
    if (bAttacked)
    {
        pAIMaster->AddEventDeltaTime(0, nAttack, GetId(), oidTarget,
            CServerAIMaster::EVENT_ON_MELEE_ATTACKED, pAttack);
    }
    int nEffect;
    CExoArrayList<CSWCCMessageData*>& lstFeedback = pAttack->m_lstFeedbackMessages;
    for (nEffect = 0; nEffect < lstFeedback.num; ++nEffect)
    {
        pAIMaster->AddEventDeltaTime(0, nTime, GetId(), oidTarget,
            CServerAIMaster::EVENT_FEEDBACK_MESSAGE, lstFeedback[nEffect]);
    }
    lstFeedback.num = 0;
    if (bHit)
    {
        int bReflected = 0;
        int bDeflected = 0;
        CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->
            GetCreatureByGameObjectID(oidTarget);
        if (IsAttackResultParried(pAttack->m_nAttackResult) && pTarget != NULL)
        {
            if (pAttack->m_nAttackResult == CSWSCombatAttackData::ATTACK_RESULT_DEFLECTED ||
                pAttack->m_nAttackResult == CSWSCombatAttackData::ATTACK_RESULT_WIDE_MISS)
            {
                bDeflected = 1;
            }
            else
            {
                bReflected = 1;
            }
        }
        if (bDeflected)
        {
                Vector vMiss = pTarget->ResolveProjectileMissLocation(this);
                int nMissTime = pTarget->CalculateProjectileTimeToTarget(vMiss, 0);
                pTarget->ResolveProjectileMissImpact(vMiss, nMissTime);
                pAttack->m_vRangedTarget = vMiss;
                pAttack->m_nMissedBy = nMissTime;
        }
        else
        {
            CGameEffect* pDamage = new CGameEffect(1);
            pDamage->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE;
            pDamage->m_nSubType = (pDamage->m_nSubType & ~CGameEffect::DURATION_MASK) |
                                 CGameEffect::DURATION_TYPE_INSTANTANEOUS;
            pDamage->SetCreator(GetId());
            pDamage->SetNumIntegers(CSWSEffectListHandler::APPLIED_DAMAGE_INTEGER_COUNT);
            for (int nDamage = 0; nDamage < CSWSEffectListHandler::APPLIED_DAMAGE_TYPE_COUNT; ++nDamage)
            {
                pDamage->SetInteger(nDamage, pAttack->m_nDamage[nDamage]);
            }
            pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_ANIMATION_DELAY, nTime);
            pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_MELEE, 1);
            pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_RANGED, 1);
            pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_COMBAT, 1);
            if (bReflected)
            {
                if (pTarget != NULL)
                {
                    int nBaseDamage = pAttack->m_nDamage[CSWSCombatAttackData::BASE_DAMAGE_INDEX];
                    int nResistedDamage = DoDamageResistance(this, nBaseDamage, GetDamageFlags(), 0, 1, 1);
                    int nDamage = DoDamageImmunity(this, nResistedDamage, GetDamageFlags(), 0, 1);
                    pDamage->SetInteger(Log_Base2(GetDamageFlags()), nDamage);
                    pDamage->SetInteger(CSWSCombatAttackData::BASE_DAMAGE_INDEX, nDamage);
                    pAIMaster->AddEventDeltaTime(0, nTime * 2, oidTarget, GetId(),
                        CServerAIMaster::EVENT_APPLY_EFFECT, pDamage);
                }
            }
            else
            {
                pAIMaster->AddEventDeltaTime(0, nTime, GetId(), oidTarget,
                    CServerAIMaster::EVENT_APPLY_EFFECT, pDamage);
            }
        }
        CExoArrayList<CGameEffect*>& lstEffects = pAttack->m_lstEffects;
        for (nEffect = 0; nEffect < lstEffects.num; ++nEffect)
        {
            pAIMaster->AddEventDeltaTime(0, nTime, GetId(), oidTarget,
                CServerAIMaster::EVENT_APPLY_EFFECT, lstEffects[nEffect]);
            if (lstEffects[nEffect]->m_nType == CSWSEffectListHandler::EFFECT_SETSTATE)
            {
                SetAttackSummaryFeedbackEffectData(lstEffects[nEffect]->GetInteger(0), oidTarget);
            }
            lstEffects[nEffect] = NULL;
        }
        lstEffects.num = 0;
        CExoArrayList<unsigned long>& lstSpellImpacts = pAttack->m_lstUnknown94;
        for (nEffect = 0; nEffect < lstSpellImpacts.num; ++nEffect)
        {
            pAIMaster->AddEventDeltaTime(0, nTime, GetId(), GetId(),
                CServerAIMaster::EVENT_ITEM_ON_HIT_SPELL_IMPACT,
                (void*)lstSpellImpacts[nEffect]);
        }
        lstSpellImpacts.num = 0;
    }
    else
    {
        ResolveProjectileMissImpact(pAttack->m_vRangedTarget, nTime);
    }
    ResolveSafeProjectile(nAttack, nTime, bHit, nWeaponHand);
}
// PC 0x006060C0
inline int CSWSCreature::SetAttackResult(int nAttackResult)
{
    int nResult = nAttackResult;

    if (nAttackResult == -1)
    {
        nResult = m_nAttackResult;
        if (nResult == 0)
        {
            m_nAttackResult = 0;
        }
        else if (nResult == 1)
        {
            m_nAttackResult = 1;
        }
        else if (nResult == 2)
        {
            m_nAttackResult = 2;
        }
        return nResult;
    }

    if (IsAttackResultHit(nAttackResult))
    {
        m_nAttackResult = 0;
    }
    else if (IsAttackResultMiss(nAttackResult))
    {
        m_nAttackResult = 1;
    }
    else if (nAttackResult >= 8 && nAttackResult <= 10)
    {
        m_nAttackResult = 2;
    }

    return nResult;
}
// PC 0x00606130
void CSWSCreature::ResolveMeleeAnimations(int nAttackNumber, int nTotalAttacks,
                                          CSWSObject* pTarget,
                                          int nAnimationLength,
                                          int bTimeAnimation)
{
    if (pTarget == NULL)
    {
        return;
    }

    int nCurrentAttack = m_pcCombatRound->m_nCurrentAttack;
    CSWSCombatAttackData* pAttack = m_pcCombatRound->GetAttack(nCurrentAttack);
    pAttack->m_nAnimationLength = nAnimationLength;
    m_pcCombatRound->DecrementRoundLength(nAnimationLength, 0);

    CSWSCreature* pCreature = pTarget->AsSWSCreature();
    if (pCreature == NULL)
    {
        return;
    }

    if (IsAttackResultHit(pAttack->m_nAttackResult))
    {
        pAttack->m_nReactionAnimation = CREATURE_ANIMATION_DAMAGE;
    }
    else if (IsAttackResultParried(pAttack->m_nAttackResult))
    {
        pAttack->m_nReactionAnimation = CREATURE_ANIMATION_PARRY;
    }
    else if (IsAttackResultMiss(pAttack->m_nAttackResult))
    {
        pAttack->m_nReactionAnimation = CREATURE_ANIMATION_DODGE;
    }

    int bDebilitated = 0;
    if ((pCreature->m_bDebilitated > 0 &&
         pCreature->m_bDebilitated != CSWSEffectListHandler::STATE_CONFUSION &&
         pCreature->m_bDebilitated != CSWSEffectListHandler::STATE_VARIANT_16) ||
        pCreature->GetDeadTemp() != 0)
    {
        bDebilitated = 1;
    }
    if (pCreature->m_c110C == 1)
    {
        bDebilitated = 1;
    }
    m_pcCombatRound->m_nUnknown948 = bDebilitated;

    int bTargetDebilitated = 0;
    if ((pCreature->m_bDebilitated > 0 &&
         pCreature->m_bDebilitated != CSWSEffectListHandler::STATE_CONFUSION &&
         pCreature->m_bDebilitated != CSWSEffectListHandler::STATE_VARIANT_16) ||
        pCreature->GetDeadTemp() != 0)
    {
        bTargetDebilitated = 1;
    }
    if (pCreature->m_c110C == 1)
    {
        bTargetDebilitated = 1;
    }

    if (bTargetDebilitated)
    {
        pAttack->m_nReactionAnimation = CSWSObject::ANIMATION_COMBAT_READY;
        return;
    }
    if ((pCreature->m_pcCombatRound->CheckActionLengthAtTime(
                  GetId(), nAnimationLength, 0) ||
              bTimeAnimation) &&
             m_pcCombatRound->m_bEngaged)
    {
        SetCombatReactionTime(nCurrentAttack, nAnimationLength, 0);
        pCreature->SetAnimation(pAttack->m_nReactionAnimation);
        pCreature->m_pcCombatRound->DecrementRoundLength(nAnimationLength, 0);
    }
    else
    {
        pAttack->m_nReactionAnimation = CSWSObject::ANIMATION_COMBAT_READY;
    }
}
// PC 0x00606310
void CSWSCreature::SignalMeleeDamage(unsigned long oidTarget, int nAttacks)
{
    CServerAIMaster* pAIMaster =
        g_pAppManager->GetServerExoApp()->GetServerAIMaster();
    CSWSCombatRound* pCombatRound = m_pcCombatRound;
    int nTime = 0;
    int nAttack;
    for (nAttack = 0; nAttack < nAttacks; ++nAttack)
    {
        CSWSCombatAttackData* pAttack = pCombatRound->GetAttack(nAttack);
        pAIMaster->AddEventDeltaTime(0, nAttack, GetId(), oidTarget,
                                   CServerAIMaster::EVENT_ON_MELEE_ATTACKED,
                                   pAttack);

        m_pcCombatRound->GetAttack(nAttack);
        if (IsAttackResultHit(pAttack->m_nAttackResult))
        {
            CGameEffect* pDamage = new CGameEffect(1);
            unsigned short nSubType = pDamage->m_nSubType;
            pDamage->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE;
            pDamage->m_nSubType = (nSubType & ~CGameEffect::DURATION_MASK) |
                                 CGameEffect::DURATION_TYPE_INSTANTANEOUS;
            pDamage->SetCreator(GetId());
            pDamage->SetNumIntegers(CSWSEffectListHandler::APPLIED_DAMAGE_INTEGER_COUNT);
            for (int nDamage = 0; nDamage < CSWSEffectListHandler::APPLIED_DAMAGE_TYPE_COUNT; ++nDamage)
            {
                pDamage->SetInteger(nDamage, pAttack->m_nDamage[nDamage]);
            }
            pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_FROM_ATTACK, 1);
            pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_ANIMATION_DELAY,
                                pAttack->m_nAnimationLength / 2);
            pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_MELEE, 1);
            pDamage->SetInteger(CSWSEffectListHandler::APPLIED_DAMAGE_COMBAT, 1);
            pAIMaster->AddEventDeltaTime(0, nTime, GetId(), oidTarget,
                                       CServerAIMaster::EVENT_APPLY_EFFECT, pDamage);
            int nEffect;
            CExoArrayList<CSWCCMessageData*>& lstFeedback = pAttack->m_lstFeedbackMessages;
            for (nEffect = 0; nEffect < lstFeedback.num; ++nEffect)
            {
                pAIMaster->AddEventDeltaTime(0, nTime, GetId(), oidTarget,
                    CServerAIMaster::EVENT_FEEDBACK_MESSAGE,
                    lstFeedback[nEffect]);
            }
            lstFeedback.num = 0;
            CExoArrayList<CGameEffect*>& lstEffects = pAttack->m_lstEffects;
            for (nEffect = 0; nEffect < lstEffects.num; ++nEffect)
            {
                CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->
                    GetCreatureByGameObjectID(oidTarget);
                if (pTarget != NULL)
                {
                    pTarget->ApplyEffect(lstEffects[nEffect], 0, 0);
                }
                else
                {
                    pAIMaster->AddEventDeltaTime(0, nTime, GetId(), oidTarget,
                        CServerAIMaster::EVENT_APPLY_EFFECT,
                        lstEffects[nEffect]);
                }
                lstEffects[nEffect] = NULL;
            }
            lstEffects.num = 0;
            CExoArrayList<unsigned long>& lstSpellImpacts = pAttack->m_lstUnknown94;
            for (nEffect = 0; nEffect < lstSpellImpacts.num; ++nEffect)
            {
                pAIMaster->AddEventDeltaTime(0, nTime, GetId(), GetId(),
                    CServerAIMaster::EVENT_ITEM_ON_HIT_SPELL_IMPACT,
                    (void*)lstSpellImpacts[nEffect]);
            }
            lstSpellImpacts.num = 0;
        }
        else
        {
            CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
            if (pClient->ServerToClientObjectId(GetId()) == pClient->GetPlayerCreatureId())
            {
                pClient->AddFloatyText(pClient->ServerToClientObjectId(oidTarget),
                                  CClientExoApp::FLOATY_TEXT_MISS, 0);
            }
        }
        nTime += pAttack->m_nReactionAnimationLength / 2;
    }
}
// PC 0x00604050
int CSWSCreature::IsAttackResultHit(int nAttackResult)
{
    int bHit = 0;

    if (nAttackResult == -1)
    {
        if (m_nAttackResult == 0)
        {
            bHit = 1;
        }
        return bHit;
    }

    if (nAttackResult > 0 && nAttackResult <= 3)
    {
        bHit = 1;
    }

    return bHit;
}
// PC 0x00604080
int CSWSCreature::IsAttackResultMiss(int nAttackResult)
{
    int bMiss = 0;

    if (nAttackResult == -1)
    {
        if (m_nAttackResult == 1)
        {
            bMiss = 1;
        }
        return bMiss;
    }

    if (nAttackResult >= 4 && nAttackResult <= 6)
    {
        bMiss = 1;
    }

    return bMiss;
}
// PC 0x006040B0
int CSWSCreature::IsAttackResultParried(int nAttackResult)
{
    int bParried = 0;

    if (nAttackResult == -1)
    {
        if (m_nAttackResult == 2)
        {
            bParried = 1;
        }
        return bParried;
    }

    if (nAttackResult >= 8 && nAttackResult <= 10)
    {
        bParried = 1;
    }

    return bParried;
}
// PC 0x006040E0
int CSWSCreature::IsSpellCastingAnimation(unsigned short nAnimation)
{
    if ((int)nAnimation == -1)
    {
        nAnimation = (unsigned short)m_nAnimation;
    }

    int bSpellCastingAnimation = 0;
    int nSpellCastingAnimation = nAnimation;

    if (nSpellCastingAnimation <= 10071)
    {
        if (nSpellCastingAnimation >= 10070)
        {
            bSpellCastingAnimation = 1;
        }
        else if (nSpellCastingAnimation >= 10015)
        {
            if (nSpellCastingAnimation <= 10020)
            {
                bSpellCastingAnimation = 1;
            }
            else if (nSpellCastingAnimation != 10061)
            {
                return bSpellCastingAnimation;
            }
            else
            {
                bSpellCastingAnimation = 1;
            }
        }
    }
    else if (nSpellCastingAnimation == 10401 ||
             nSpellCastingAnimation == 11000)
    {
        bSpellCastingAnimation = 1;
    }

    return bSpellCastingAnimation;
}
// PC 0x00604140
void CSWSCreature::SetCombatReactionTime(int nAttack, int nTime,
                                         int nReactionDelay)
{
    m_pcCombatRound->GetAttack(nAttack)->m_nReactionAnimationLength = nTime;
    m_pcCombatRound->GetAttack(nAttack)->m_nReactionDelay = nReactionDelay;
}
// PC 0x00604180
int CSWSCreature::CalculateMaxElementalDamage(CSWSObject* pObject, int)
{
    if (pObject == NULL || pObject->m_bPlotObject != 0)
    {
        return 0;
    }

    int nElementalDamage = 0;

    return GetTotalEffectBonus(2, pObject, nElementalDamage, 1, 1, 0, 0, 255,
                               255, 0);
}
// PC 0x00606620
int CSWSCreature::IsProjectileParryable(CSWSObject* pTarget, int bRangedAttack,
                                       int bCheckShield, int& nShield,
                                       int& bCanReflect, int nWeaponHand)
{
    int bResult = 0;
    bCanReflect = 1;
    CSWSCombatAttackData* pAttack =
        m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
    if (pTarget->AsSWSCreature() != NULL && pAttack->m_bRangedAttack)
    {
        CSWSCreature* pCreature = pTarget->AsSWSCreature();
        if ((pCreature->m_pStats->HasFeat(CSWFeat::FEAT_JEDI_DEFENSE) ||
             pCreature->m_pStats->HasFeat(CSWFeat::FEAT_ADVANCED_JEDI_DEFENSE) ||
             pCreature->m_pStats->HasFeat(CSWFeat::FEAT_MASTER_JEDI_DEFENSE)) &&
            !pCreature->IsDebilitated() &&
            pCreature->GetClientCreature() != NULL &&
            pCreature->IsLightsaberEquipped() &&
            !pCreature->m_bLightsaberThrow)
        {
            unsigned long nSlot = 0;
            if (nWeaponHand == -1)
            {
                if (pAttack->m_nWeaponAttackType == 1)
                    nWeaponHand = 0;
                else if (pAttack->m_nWeaponAttackType == 2)
                    nWeaponHand = 1;
            }
            if (nWeaponHand == 0)
                nSlot = INVENTORY_SLOT_RIGHTHAND;
            else if (nWeaponHand == 1)
                nSlot = INVENTORY_SLOT_LEFTHAND;
            CSWSItem* pWeapon = m_pInventory->GetItemInSlot(nSlot);
            if (pWeapon != NULL &&
                (pWeapon->GetBaseItem()->m_nDamageFlags & DAMAGE_TYPE_BLASTER))
            {
                Vector vOrientation = pCreature->GetClientCreature()->m_vOrientation;
                if (vOrientation * m_vOrientation <= 0.0f)
                    bResult = 1;
            }
        }
        if (!bResult && bCheckShield && bRangedAttack &&
            pCreature->GetClientCreature() != NULL &&
            pCreature->GetClientCreature()->GetForceShield())
        {
            unsigned long nSlot = 0;
            if (nWeaponHand == 0)
                nSlot = INVENTORY_SLOT_RIGHTHAND;
            else if (nWeaponHand == 1)
                nSlot = INVENTORY_SLOT_LEFTHAND;
            CSWInventory* pInventory = m_pInventory;
            CSWSItem* pWeapon = pInventory->GetItemInSlot(nSlot);
            if (pWeapon != NULL)
            {
                unsigned char nAmmunitionType = pWeapon->GetBaseItem()->m_nAmmunitionType;
                int bShieldHit = 0;
                {
                    CExoString sColumn("ShieldHit");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    C2DA* pTable = p2DArrays->m_pAmmunitionTypes;
                    pTable->GetINTEntry(nAmmunitionType, sColumn, &bShieldHit);
                }
                if (bShieldHit)
                {
                    bResult = 1;
                    nShield = pCreature->GetClientCreature()->GetForceShield();
                }
            }
        }
        int bDeflection = pCreature->m_pStats->HasSpell(0, 162, 0);
        int bRedirection = pCreature->m_pStats->HasSpell(0, 163, 0);
        if (!bResult && (bDeflection || bRedirection) &&
            !pCreature->IsDebilitated() && !pCreature->m_bLightsaberThrow)
        {
            Vector vOrientation = pCreature->GetClientCreature()->m_vOrientation;
            if (vOrientation * m_vOrientation <= 0.0f)
            {
                bResult = 1;
                if (bDeflection && !bRedirection)
                    bCanReflect = 0;
            }
        }
    }
    return bResult;
}
// PC 0x00608220
int CSWSCreature::ResolveRangedMissToParry(CSWSObject* pTarget, int bCheckWeapon,
                                           int nWeaponHand)
{
    int nShield = 0;
    int bCanReflect;
    int bParried = IsProjectileParryable(pTarget, bCheckWeapon, 1, nShield,
                                         bCanReflect, nWeaponHand);
    if (bParried)
    {
        CSWSCombatAttackData* pCurrentAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        if (nShield)
        {
            pCurrentAttack->m_nAttackResult = CSWSCombatAttackData::ATTACK_RESULT_WIDE_MISS;
        }
        else
        {
            pCurrentAttack->m_nAttackResult = ATTACK_RESULT_PARRIED;
        }
    }
    return bParried;
}
// PC 0x00606970
int CSWSCreature::ResolveMoveToForceJump(CSWSObject* pTarget,
                                       unsigned short& nFeat)
{
    int bResult = 0;
    if (pTarget != NULL && pTarget->AsSWSCreature() != NULL &&
        nFeat == 0 && m_pStats->HasFeat(CSWFeat::FEAT_FORCE_JUMP) &&
        IsLightsaberEquipped())
    {
        Vector vDifference = m_vPosition - pTarget->m_vPosition;
        if (vDifference * vDifference >= 100.0f)
        {
            GetPathfindInformation()->m_oidPathfindTargetCreature =
                pTarget->GetId();
            float fDistance = GetPathfindInformation()->m_fStepScale;
            CResRef cAreaResRef("");
            float fWidth = GetPathfindInformation()->m_fHalfStep;
            if (g_pAppManager->GetServerExoApp()->TestDirectLine(
                    GetId(), cAreaResRef, m_vPosition, pTarget->m_vPosition,
                    fDistance, fWidth, 0, NULL) == CSWSArea::DIRECT_LINE_CLEAR)
            {
                CGameEffect* pEffect = new CGameEffect(1);
                pEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEJUMP;
                pEffect->SetObjectID(0, pTarget->GetId());
                pEffect->SetInteger(0, 0);
                ApplyEffect(pEffect, 0, 0);
                if (m_pStats->HasFeat(CSWFeat::FEAT_FORCE_JUMP_MASTERY))
                {
                    nFeat = CSWFeat::FEAT_FORCE_JUMP_MASTERY;
                }
                else if (m_pStats->HasFeat(CSWFeat::FEAT_FORCE_JUMP_ADVANCED))
                {
                    nFeat = CSWFeat::FEAT_FORCE_JUMP_ADVANCED;
                }
                else
                {
                    nFeat = CSWFeat::FEAT_FORCE_JUMP;
                }
                bResult = 1;
            }
        }
    }
    return bResult;
}
// PC 0x00606B50
void CSWSCreature::ResolveDamageShields(CSWSCreature* pAttacker)
{
    int nNumDice;
    int nDie;
    CExoArrayList<CGameEffect*>* pEffects = &m_lstEffects;
    int nEffect = GetStats()->m_nDamageShieldEffectIndex;

    if (pAttacker != NULL)
    {
        for (; nEffect < pEffects->num &&
               (*pEffects)[nEffect]->m_nType <=
                   CSWSEffectListHandler::EFFECT_DAMAGE_SHIELD;
             nEffect++)
        {
            CGameEffect* pEffect = (*pEffects)[nEffect];
            if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_DAMAGE_SHIELD)
            {
                int nDamageAmount = pEffect->GetInteger(0);
                int nRandomAmount = pEffect->GetInteger(1);
                int nDamageFlags = pEffect->GetInteger(2);

                if (nRandomAmount > 5)
                {
                    {
                        CExoString sColumn("NumDice");
                        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                        C2DA* pTable = p2DArrays->m_pIPRPDamage;
                        pTable->GetINTEntry(nRandomAmount, sColumn, &nNumDice);
                    }
                    {
                        CExoString sColumn("Die");
                        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                        C2DA* pTable = p2DArrays->m_pIPRPDamage;
                        pTable->GetINTEntry(nRandomAmount, sColumn, &nDie);
                    }
                    nRandomAmount = g_pRules->RollDice(nNumDice, nDie);
                }

                int nDamage = nDamageAmount + nRandomAmount;
                int nDamageIndex = Log_Base2(nDamageFlags);

                CGameEffect* pDamageEffect = new CGameEffect(1);
                pDamageEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE;
                pDamageEffect->m_nSubType =
                    (pDamageEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                pDamageEffect->SetCreator(GetId());
                pDamageEffect->SetNumIntegers(21);
                for (int nIndex = 0; nIndex < 15; nIndex++)
                {
                    if (nIndex != nDamageIndex)
                    {
                        pDamageEffect->SetInteger(nIndex, -1);
                    }
                    else
                    {
                        pDamageEffect->SetInteger(nDamageIndex, nDamage);
                    }
                }
                pDamageEffect->SetInteger(16, 1000);
                pAttacker->ApplyEffect(pDamageEffect, 0, 0);

                CGameEffect* pVisualEffect = new CGameEffect(1);
                pVisualEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
                pVisualEffect->m_nSubType =
                    (pVisualEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                pVisualEffect->SetCreator(GetId());

                pVisualEffect->SetInteger(0, 0);
                pAttacker->ApplyEffect(pVisualEffect, 0, 0);
            }
        }
    }
}
// PC 0x00606E10
unsigned long CSWSCreature::GetNextAttacker()
{
    unsigned long oidAttacker = OBJECT_INVALID;
    CExoArrayList<unsigned long>* pAttackerList = m_pAttackerList;

    if (pAttackerList != NULL)
    {
        int nAttacker = m_nNextAttacker;
        if (nAttacker < pAttackerList->num)
        {
            m_nNextAttacker = nAttacker + 1;
            oidAttacker = (*pAttackerList)[nAttacker];
        }
    }

    return oidAttacker;
}
// PC 0x006041D0
int CSWSCreature::IsLightsaberEquipped()
{
    int bLightsaberEquipped = 0;
    CSWSItem* pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
    if (pItem != 0)
    {
        if (pItem->IsLightsaber() != 0)
        {
            bLightsaberEquipped = 1;
        }
    }

    pItem = m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
    if (pItem != 0 && pItem->IsLightsaber() != 0)
    {
        return 1;
    }
    return bLightsaberEquipped;
}
// PC 0x00604220
void CSWSCreature::DoCombatStep(unsigned char nStep, int nTime,
                              unsigned long oidTarget)
{
    Vector vNewPosition;
    int bClear = 0;
    CGameObject* pGameObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        if (GetArea() == NULL)
        {
            m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
            m_pcCombatRound->SetPauseTimer(0, 0);
            SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
            return;
        }
        else
        {
            CSWSObject* pTarget = pGameObject->AsSWSObject();
            if (!(m_nAIStateFlags & AI_STATE_CAN_MOVE))
            {
                m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
                m_pcCombatRound->SetPauseTimer(0, 0);
                SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
                return;
            }
            else
            {
                float fDistance = magnitude(m_vPosition - pTarget->m_vPosition);
                Vector vDirection;
                vDirection = normalize(m_vPosition - pTarget->m_vPosition);
                Vector vStepDirection;
                float fRange;
                float fPersonalSpace;
                int nAnimation;
                if (nStep == COMBAT_STEP_AUTOMATIC ||
                    nStep == COMBAT_STEP_ADJUST_RANGE)
                {
                    fRange = DesiredAttackRange(oidTarget, 0);
                    float fTargetRange;
                    if (pTarget->AsSWSCreature() != NULL)
                    {
                        pTarget->AsSWSCreature()->DesiredAttackRange(GetId(), 0);
                        fTargetRange =
                            pTarget->AsSWSCreature()->DesiredAttackRange(GetId(), 1);
                    }
                    else
                    {
                        fTargetRange = 1.0f;
                    }
                    int bRanged = RangeWeaponEquipped();
                    if ((fRange - NWSCREATURE_COMBAT_STEP_RANGE_MARGIN > fDistance && bRanged == 0) ||
                        (bRanged == 1 && fDistance < fTargetRange))
                    {
                        nAnimation = CREATURE_ANIMATION_FOOTSTEP_2;
                    }
                    else if (fRange + NWSCREATURE_COMBAT_STEP_RANGE_MARGIN < fDistance)
                    {
                        nAnimation = CREATURE_ANIMATION_FOOTSTEP_1;
                    }
                    else if (nStep == COMBAT_STEP_ADJUST_RANGE)
                    {
                        m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
                        m_pcCombatRound->SetPauseTimer(0, 0);
                        SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
                        return;
                    }
                    else
                    {
                        nAnimation = rand() % 2 ?
                            CREATURE_ANIMATION_FOOTSTEP_3 :
                            CREATURE_ANIMATION_FOOTSTEP_4;
                    }
                    if (bRanged == 1 && nAnimation != CREATURE_ANIMATION_FOOTSTEP_2)
                    {
                        m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
                        m_pcCombatRound->SetPauseTimer(0, 0);
                        SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
                        return;
                    }
                }

                else if (nStep == COMBAT_STEP_FORWARD)
                    nAnimation = CREATURE_ANIMATION_FOOTSTEP_1;
                else if (nStep == COMBAT_STEP_BACKWARD)
                    nAnimation = CREATURE_ANIMATION_FOOTSTEP_2;
                else if (nStep == COMBAT_STEP_LEFT)
                    nAnimation = CREATURE_ANIMATION_FOOTSTEP_3;
                else if (nStep == COMBAT_STEP_RIGHT)
                    nAnimation = CREATURE_ANIMATION_FOOTSTEP_4;
                else
                {
                    m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
                    m_pcCombatRound->SetPauseTimer(0, 0);
                    SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
                    return;
                }

                if (nAnimation == CREATURE_ANIMATION_FOOTSTEP_3)
                {
                    float fAngle = (float)atan2(vDirection.m_fY, vDirection.m_fX);
                    float fStepAngle = (float)asin(NWSCREATURE_COMBAT_STEP_SIDE_HALF_CHORD / fDistance);
                    fAngle -= fStepAngle * 2.0f;
                    if (fAngle < 0.0f)
                        fAngle += TWO_PI;
                    vNewPosition.m_fX = (float)cos(fAngle);
                    vNewPosition.m_fY = (float)sin(fAngle);
                    vNewPosition = pTarget->m_vPosition + vNewPosition * fDistance;
                }
                else if (nAnimation == CREATURE_ANIMATION_FOOTSTEP_4)
                {
                    float fAngle = (float)atan2(vDirection.m_fY, vDirection.m_fX);
                    float fStepAngle = (float)asin(NWSCREATURE_COMBAT_STEP_SIDE_HALF_CHORD / fDistance);
                    fAngle += fStepAngle * 2.0f;
                    if (fAngle > TWO_PI)
                        fAngle = TWO_PI - fAngle;
                    vNewPosition.m_fX = (float)cos(fAngle);
                    vNewPosition.m_fY = (float)sin(fAngle);
                    vNewPosition = pTarget->m_vPosition + vNewPosition * fDistance;
                }
                else if (nAnimation == CREATURE_ANIMATION_FOOTSTEP_1)
                {
                    vStepDirection = -vDirection;
                    float fStep;
                    if (nStep == COMBAT_STEP_AUTOMATIC ||
                        nStep == COMBAT_STEP_ADJUST_RANGE)
                    {
                        fStep = fDistance - fRange > NWSCREATURE_COMBAT_STEP_MAX_DISTANCE ? NWSCREATURE_COMBAT_STEP_MAX_DISTANCE : fDistance - fRange;
                    }
                    else
                    {
                        fStep = NWSCREATURE_COMBAT_STEP_MAX_DISTANCE;
                    }
                    vNewPosition = m_vPosition + fStep * vStepDirection;
                }
                else if (nAnimation == CREATURE_ANIMATION_FOOTSTEP_2)
                {
                    vStepDirection = vDirection;
                    float fStep;
                    if (nStep == COMBAT_STEP_AUTOMATIC ||
                        nStep == COMBAT_STEP_ADJUST_RANGE)
                    {
                        fStep = fRange - fDistance > NWSCREATURE_COMBAT_STEP_MAX_DISTANCE ? NWSCREATURE_COMBAT_STEP_MAX_DISTANCE : fRange - fDistance;
                    }
                    else
                    {
                        fStep = NWSCREATURE_COMBAT_STEP_MAX_DISTANCE;
                    }
                    vNewPosition = m_vPosition + fStep * vStepDirection;
                }

                fPersonalSpace = GetPathfindInformation()->m_fStepScaleY;
                float fHalfStep = GetPathfindInformation()->m_fHalfStep;
                GetArea()->m_pPathfindInformation = GetPathfindInformation();
                unsigned long oidOldTarget =
                    GetPathfindInformation()->m_oidPathfindTargetCreature;
                GetPathfindInformation()->m_oidPathfindTargetCreature = OBJECT_INVALID;
                if (GetArea()->TestDirectLine(m_vPosition, vNewPosition,
                    fPersonalSpace, fHalfStep, 0, NULL) == CSWSArea::DIRECT_LINE_CLEAR)
                {
                    bClear = 1;
                }
                GetPathfindInformation()->m_oidPathfindTargetCreature = oidOldTarget;
                if (!(vNewPosition.m_fX < 0.0f ||
                    vNewPosition.m_fX > GetArea()->m_nWidth * 10.0f ||
                    vNewPosition.m_fY < 0.0f ||
                    vNewPosition.m_fY > GetArea()->m_nHeight * 10.0f))
                {
                    if (bClear == 1)
                    {
                        UpdateSubareasOnMoveTo(m_vPosition, vNewPosition, 0, NULL, 0);
                        SetPosition(vNewPosition, 1, 1, 0);
                        return;
                    }
                    else
                    {
                        m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
                        m_pcCombatRound->SetPauseTimer(0, 0);
                        SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
                        return;
                    }
                }
            }
        }
        m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
        m_pcCombatRound->SetPauseTimer(0, 0);
        SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
        return;
    }
    m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
    m_pcCombatRound->SetPauseTimer(0, 0);
    SetAnimation(CREATURE_ANIMATION_PAUSE_READY);
}

inline int CSWSCreature::CalculateProjectileTimeToTarget(const Vector& vTarget,
                                                          int bFast)
{
    Vector vDelta(vTarget.m_fX - m_vPosition.m_fX,
                  vTarget.m_fY - m_vPosition.m_fY,
                  vTarget.m_fZ - m_vPosition.m_fZ);
    float fDistance = magnitude(vDelta);
    float fSpeed;
    if (bFast != 0)
    {
        fSpeed = 37.0f;
    }
    else
    {
        fSpeed = 42.0f;
    }
    return (unsigned long)(fDistance / fSpeed * 1000.0f);
}
// PC 0x00604780
CGameEffect* CSWSCreature::CreateLinkedEffect(int nEffectType,
                                              int nEffectInteger,
                                              int nLinkedEffectType,
                                              int nLinkedEffectInteger)
{
    CGameEffect* pLeftEffect = new CGameEffect(1);
    if (pLeftEffect == NULL)
    {
        return NULL;
    }

    CGameEffect* pRightEffect = new CGameEffect(1);
    if (pRightEffect == NULL)
    {
        return NULL;
    }

    CGameEffect* pLinkEffect = new CGameEffect(1);
    if (pLinkEffect == NULL)
    {
        return NULL;
    }

    pLeftEffect->m_nType = nEffectType;
    pLeftEffect->SetInteger(0, nEffectInteger);
    pRightEffect->m_nType = nLinkedEffectType;
    pRightEffect->SetInteger(0, nLinkedEffectInteger);
    pLinkEffect->m_nType = CSWSEffectListHandler::EFFECT_LINK;
    pLinkEffect->SetCreator(m_nId);
    pLinkEffect->SetLinked(pLeftEffect, pRightEffect);
    return pLinkEffect;
}
inline int CSWSCreature::GetClientRangedAnimation(int nAttackType, int nOption1,
                                                   int nOption2)
{
    return g_pAppManager->GetClientExoApp()->GetClientRangedAnimation(
        GetId(), nAttackType, nOption1, nOption2);
}
// PC 0x006048E0
CGameEffect* CSWSCreature::CreateLinkedEffect(CGameEffect* pEffect,
                                              int nEffectType,
                                              int nEffectInteger)
{
    CGameEffect* pRightEffect = new CGameEffect(1);
    if (pRightEffect == NULL)
    {
        return NULL;
    }

    CGameEffect* pLinkEffect = new CGameEffect(1);
    if (pLinkEffect == NULL)
    {
        return NULL;
    }

    pRightEffect->m_nType = nEffectType;
    pRightEffect->SetInteger(0, nEffectInteger);
    pLinkEffect->m_nType = CSWSEffectListHandler::EFFECT_LINK;
    pLinkEffect->SetCreator(m_nId);
    pLinkEffect->SetLinked(pEffect, pRightEffect);
    return pLinkEffect;
}
// PC 0x006049D0
void CSWSCreature::ApplyOnHitSlayAlignmentGroup(
    CSWSObject* pObject, CSWItemProperty* pItemProperty)
{
}
// PC 0x006049E0
void CSWSCreature::SendAttackSummaryFeedback(unsigned long oidTarget)
{
    CSWSCombatRound* pCombatRound = m_pcCombatRound;
    CSWCCMessageData* pMessage = new CSWCCMessageData;
    CSWSCreature* pTarget =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);

    if (pCombatRound != NULL)
    {
        CSWSCombatAttackData* pAttack =
            pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack);
        int nDamage = pAttack->GetBaseDamage();

        if (pTarget != NULL)
        {
            CSWPartyTable* pParty = g_pAppManager->GetServerExoApp()->GetPartyTable();
            if ((pParty != NULL && pParty->GetIsMember(oidTarget)) ||
                pTarget->m_nAreaWaitState)
            {
                unsigned char nDifficulty =
                    g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nDifficulty;
                int nRow = nDifficulty;
                float fMultiplier;
                int bFound;
                {
                    CExoString sColumn("MULTIPLIER");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    C2DA* pTable = p2DArrays->m_pDifficultyOptions;
                    bFound = pTable->GetFLOATEntry(nRow, sColumn, &fMultiplier);
                }
                if (bFound)
                {
                    nDamage = (int)(nDamage * fMultiplier);
                }
            }
        }

        pMessage->SetInteger(CSWCCMessageData::SUMMARY_ATTACK_RESULT, pAttack->m_nAttackResult);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_ATTACK_TYPE, pAttack->m_nAttackType);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_DEFENSE_TOTAL,
            pAttack->m_Extra1.m_nUnknown1 + (char)pAttack->m_Extra1.m_nUnknown2);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_ATTACK_FLAGS, (char)pAttack->m_Flags.m_bUnknown0);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_BASE_DAMAGE, nDamage);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_DAMAGE_DETAIL, pAttack->m_Extra2.m_nUnknownC);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_SNEAK_ATTACK, pAttack->m_bSneakAttack);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_COUP_DE_GRACE, pAttack->m_bCoupDeGrace);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_HIT_DETAIL, pAttack->m_Extra4.m_nState);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_HIT_MODIFIER, pAttack->m_Extra4.m_bStateImmune);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_DEFENSE_ROLL, pAttack->m_Extra1.m_nUnknown18);
        pMessage->SetInteger(CSWCCMessageData::SUMMARY_DEFENSE_MODIFIER, pAttack->m_Extra1.m_nUnknown1C);
        pMessage->SetObjectID(CSWCCMessageData::SUMMARY_ATTACKER, m_nId);
        pMessage->SetObjectID(CSWCCMessageData::SUMMARY_TARGET, oidTarget);
        BroadcastAttackSummaryToParty(pMessage);
        if (pTarget != NULL)
        {
            pTarget->BroadcastAttackSummaryToParty(pMessage);
        }
    }

    delete pMessage;
}
// PC 0x00604C30
enum
{
    FEAT_IMMUNITY_HORRIFIED = 200,
    IMMUNITY_TYPE_VARIANT_33 = 33
};
void CSWSCreature::SetAttackSummaryFeedbackEffectData(int nState, unsigned long oidTarget)
{
    CSWSCreature* pTarget =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidTarget);
    if (pTarget != NULL)
    {
        CSWSCombatAttackData* pAttack =
            m_pcCombatRound->GetAttack(m_pcCombatRound->m_nCurrentAttack);
        switch (pAttack->m_Extra4.m_nState)
        {
        case CSWSEffectListHandler::STATE_SLEEP:
        {
            CSWSCreatureStats* pStats = pTarget->m_pStats;
            if (pStats->GetEffectImmunity(CSWSEffectListHandler::IMMUNITY_TYPE_SLEEP, this))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        case CSWSEffectListHandler::STATE_HOLD:
        {
            CSWSCreatureStats* pStats = pTarget->m_pStats;
            if (pStats->GetEffectImmunity(CSWSEffectListHandler::IMMUNITY_TYPE_PARALYSIS, this) ||
                pTarget->m_pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_PARALYSIS))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        case CSWSEffectListHandler::STATE_MIND_TRICK_18:
        {
            CSWSCreatureStats* pStats = pTarget->m_pStats;
            if (pStats->GetEffectImmunity(CSWSEffectListHandler::IMMUNITY_TYPE_MIND_AFFECTING, this))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        case CSWSEffectListHandler::STATE_STUN:
        {
            CSWSCreatureStats* pStats = pTarget->m_pStats;
            if (pStats->GetEffectImmunity(CSWSEffectListHandler::IMMUNITY_TYPE_STUN, this) ||
                pTarget->m_pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_STUN) ||
                pTarget->m_pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_PARALYSIS))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        case CSWSEffectListHandler::STATE_CONFUSION:
        {
            CSWSCreatureStats* pStats = pTarget->m_pStats;
            if (pStats->GetEffectImmunity(CSWSEffectListHandler::IMMUNITY_TYPE_CONFUSED, this))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        case CSWSEffectListHandler::STATE_VARIANT_16:
        {
            CSWSCreatureStats* pStats = pTarget->m_pStats;
            if (pStats->GetEffectImmunity(IMMUNITY_TYPE_VARIANT_33, this))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        case CSWSEffectListHandler::STATE_FEAR:
        {
            CSWSCreatureStats* pStats = pTarget->m_pStats;
            if (pStats->GetEffectImmunity(CSWSEffectListHandler::IMMUNITY_TYPE_FEAR, this) ||
                pTarget->m_pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_FEAR) ||
                pTarget->m_pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_STUN) ||
                pTarget->m_pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_PARALYSIS) ||
                pTarget->m_pStats->HasFeat(FEAT_IMMUNITY_HORRIFIED))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        case CSWSEffectListHandler::STATE_HORRIFIED:
        {
            if (pTarget->m_pStats->HasFeat(FEAT_IMMUNITY_HORRIFIED))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        case CSWSEffectListHandler::STATE_KNOCKDOWN:
        {
            CSWSCreatureStats* pStats = pTarget->m_pStats;
            if (pStats->GetEffectImmunity(CSWSEffectListHandler::IMMUNITY_TYPE_KNOCKDOWN, this))
                pAttack->m_Extra4.m_bStateImmune = 1;
            break;
        }
        }
    }
}
