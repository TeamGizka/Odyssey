#include "nwseffectlist.h"
#include "gameobjectarray.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwseffect.h"
#include "nwsobject.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "nwscreaturestats.h"
#include "nwrules.h"
#include "nwclass.h"
#include "nwsplayer.h"
#include "nwrace.h"
#include "serveraimaster.h"
#include "gameobjectarray.h"
#include "serveraimaster.h"
#include "worldtimer.h"
#include "nwscombatround.h"
#include "nwsspellimpact.h"
#include "nwsscriptevent.h"
#include "nwmessage.h"
#include "nwsplaceable.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "pathfindinfo.h"
#include "nwsitem.h"
#include <stdlib.h>

#include "clientexoapp.h"
#include "nwccreature.h"
#include "swparty.h"
#include "nwscript.h"
#include "nwfeat.h"
#include "nwspell.h"
#include "nwitem.h"
#include "nwcanimbase.h"
#include "partytables.h"
#include "nwsdoor.h"
#include "factionmanager.h"
#include "nwsparty.h"
#include "nwsencounter.h"
#include "clientoptions.h"
#include "guiingame.h"

static inline void NwseffectlistTrace2();
static inline void NwseffectlistTrace1();
#define NWSEFFECTLIST_TRACE() NwseffectlistTrace2()

static inline CServerExoApp* ServerExoAppLate2(CAppManager* p);
static inline CServerExoApp* ServerExoAppLate1(CAppManager* p);

const int CSWSEffectListHandler::STORE_EFFECT = 0;
const int CSWSEffectListHandler::DELETE_EFFECT = 1;
const int CSWSEffectListHandler::EFFECT_INVALIDEFFECT = 0;
const int CSWSEffectListHandler::EFFECT_HASTE = 1;
const int CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE = 2;
const int CSWSEffectListHandler::EFFECT_SLOW = 3;
const int CSWSEffectListHandler::EFFECT_RESURRECTION = 4;
const int CSWSEffectListHandler::EFFECT_DISEASE = 5;
const int CSWSEffectListHandler::EFFECT_SUMMON_CREATURE = 6;
const int CSWSEffectListHandler::EFFECT_REGENERATE = 7;
const int CSWSEffectListHandler::EFFECT_SETSTATE = 8;
const int CSWSEffectListHandler::EFFECT_SETSTATE_INTERNAL = 9;
const int CSWSEffectListHandler::EFFECT_ATTACK_INCREASE = 10;
const int CSWSEffectListHandler::EFFECT_ATTACK_DECREASE = 11;
const int CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION = 12;
const int CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE = 13;
const int CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE = 14;
const int CSWSEffectListHandler::EFFECT_TEMPORARY_HITPOINTS = 15;
const int CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_INCREASE = 16;
const int CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_DECREASE = 17;
const int CSWSEffectListHandler::EFFECT_ENTANGLE = 18;
const int CSWSEffectListHandler::EFFECT_DEATH = 19;
const int CSWSEffectListHandler::EFFECT_KNOCKDOWN = 20;
const int CSWSEffectListHandler::EFFECT_DEAF = 21;
const int CSWSEffectListHandler::EFFECT_IMMUNITY = 22;
const int CSWSEffectListHandler::EFFECT_SET_AI_STATE = 23;
const int CSWSEffectListHandler::EFFECT_ENEMY_ATTACK_BONUS = 24;
const int CSWSEffectListHandler::EFFECT_ARCANE_SPELL_FAILURE = 25;
const int CSWSEffectListHandler::EFFECT_SAVING_THROW_INCREASE = 26;
const int CSWSEffectListHandler::EFFECT_SAVING_THROW_DECREASE = 27;
const int CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_INCREASE = 28;
const int CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_DECREASE = 29;
const int CSWSEffectListHandler::EFFECT_VISUALEFFECT = 30;
const int CSWSEffectListHandler::EFFECT_AREA_OF_EFFECT = 31;
const int CSWSEffectListHandler::EFFECT_BEAM = 32;
const int CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_INCREASE = 33;
const int CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_DECREASE = 34;
const int CSWSEffectListHandler::EFFECT_POISON = 35;
const int CSWSEffectListHandler::EFFECT_ABILITY_INCREASE = 36;
const int CSWSEffectListHandler::EFFECT_ABILITY_DECREASE = 37;
const int CSWSEffectListHandler::EFFECT_DAMAGE = 38;
const int CSWSEffectListHandler::EFFECT_HEAL = 39;
const int CSWSEffectListHandler::EFFECT_LINK = 40;
const int CSWSEffectListHandler::EFFECT_HASTE_INTERNAL = 41;
const int CSWSEffectListHandler::EFFECT_SLOW_INTERNAL = 42;
const int CSWSEffectListHandler::EFFECT_DROIDSTUN = 43;
const int CSWSEffectListHandler::EFFECT_MODIFYNUMATTACKS = 44;
const int CSWSEffectListHandler::EFFECT_CURSE = 45;
const int CSWSEffectListHandler::EFFECT_SILENCE = 46;
const int CSWSEffectListHandler::EFFECT_INVISIBILITY = 47;
const int CSWSEffectListHandler::EFFECT_AC_INCREASE = 48;
const int CSWSEffectListHandler::EFFECT_AC_DECREASE = 49;
const int CSWSEffectListHandler::EFFECT_SPELL_IMMUNITY = 50;
const int CSWSEffectListHandler::EFFECT_DISPEL_ALL_MAGIC = 51;
const int CSWSEffectListHandler::EFFECT_DISPEL_BEST_MAGIC = 52;
const int CSWSEffectListHandler::EFFECT_TAUNT = 53;
const int CSWSEffectListHandler::EFFECT_LIGHT = 54;
const int CSWSEffectListHandler::EFFECT_SKILL_INCREASE = 55;
const int CSWSEffectListHandler::EFFECT_SKILL_DECREASE = 56;
const int CSWSEffectListHandler::EFFECT_HITPOINTCHANGEWHENDYING = 57;
const int CSWSEffectListHandler::EFFECT_SETWALKANIMATION = 58;
const int CSWSEffectListHandler::EFFECT_LIMIT_MOVEMENT_SPEED = 59;
const int CSWSEffectListHandler::EFFECT_FORCEPUSHED = 60;
const int CSWSEffectListHandler::EFFECT_DAMAGE_SHIELD = 61;
const int CSWSEffectListHandler::EFFECT_DISGUISE = 62;
const int CSWSEffectListHandler::EFFECT_SANCTUARY = 63;
const int CSWSEffectListHandler::EFFECT_TIMESTOP = 64;
const int CSWSEffectListHandler::EFFECT_SPELL_LEVEL_ABSORPTION = 65;
const int CSWSEffectListHandler::EFFECT_WHIRLWIND = 66;
const int CSWSEffectListHandler::EFFECT_EFFECT_ICON = 67;
const int CSWSEffectListHandler::EFFECT_RACIAL_TYPE = 68;
const int CSWSEffectListHandler::EFFECT_VISION = 69;
const int CSWSEffectListHandler::EFFECT_SEEINVISIBLE = 70;
const int CSWSEffectListHandler::EFFECT_ULTRAVISION = 71;
const int CSWSEffectListHandler::EFFECT_TRUESEEING = 72;
const int CSWSEffectListHandler::EFFECT_BLINDNESS = 73;
const int CSWSEffectListHandler::EFFECT_DARKNESS = 74;
const int CSWSEffectListHandler::EFFECT_MISS_CHANCE = 75;
const int CSWSEffectListHandler::EFFECT_CONCEALMENT = 76;
const int CSWSEffectListHandler::EFFECT_TURN_RESISTANCE_INCREASE = 77;
const int CSWSEffectListHandler::EFFECT_BONUS_SPELL_OF_LEVEL = 78;
const int CSWSEffectListHandler::EFFECT_DISAPPEARAPPEAR = 79;
const int CSWSEffectListHandler::EFFECT_DISAPPEAR = 80;
const int CSWSEffectListHandler::EFFECT_APPEAR = 81;
const int CSWSEffectListHandler::EFFECT_NEGATIVE_LEVEL = 82;
const int CSWSEffectListHandler::EFFECT_BONUSFEAT = 83;
const int CSWSEffectListHandler::EFFECT_WOUNDING = 84;
const int CSWSEffectListHandler::EFFECT_VAMPIRIC_REGENERATION = 86;
const int CSWSEffectListHandler::EFFECT_DISARM = 87;
const int CSWSEffectListHandler::EFFECT_TURN_RESISTANCE_DECREASE = 88;
const int CSWSEffectListHandler::EFFECT_SUMMON_PARTY = 89;
const int CSWSEffectListHandler::EFFECT_FORCEDRAIN = 90;
const int CSWSEffectListHandler::EFFECT_TEMPORARY_FORCE = 91;
const int CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONINCREASE = 92;
const int CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONDECREASE = 93;
const int CSWSEffectListHandler::EFFECT_HORRIFIED = 94;
const int CSWSEffectListHandler::EFFECT_DAMAGEFORCEPOINTS = 95;
const int CSWSEffectListHandler::EFFECT_HEALFORCEPOINTS = 96;
const int CSWSEffectListHandler::EFFECT_CHOKE = 97;
const int CSWSEffectListHandler::EFFECT_BODYFUEL = 98;
const int CSWSEffectListHandler::EFFECT_PSYCHICSTATIC = 99;
const int CSWSEffectListHandler::EFFECT_LIGHTSABERTHROW = 100;
const int CSWSEffectListHandler::EFFECT_ASSUREDHIT = 101;
const int CSWSEffectListHandler::EFFECT_FORCEJUMP = 102;
const int CSWSEffectListHandler::EFFECT_FORCEJUMPDELAYED = 103;
const int CSWSEffectListHandler::EFFECT_ASSUREDDEFLECTION = 104;
const int CSWSEffectListHandler::EFFECT_FORCERESISTED = 105;
const int CSWSEffectListHandler::EFFECT_FORCEFIZZLE = 106;
const int CSWSEffectListHandler::EFFECT_FORCESHIELD = 107;
const int CSWSEffectListHandler::EFFECT_PUREGOODPOWERS = 108;
const int CSWSEffectListHandler::EFFECT_PUREEVILPOWERS = 109;
#ifdef _XBOX
const int CSWSEffectListHandler::MAX_EFFECTS = 108;
#else
const int CSWSEffectListHandler::MAX_EFFECTS = 118;
#endif
const int CSWSEffectListHandler::EFFECT_ABILITY_INVALID = -1;
const int CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH = 0;
const int CSWSEffectListHandler::EFFECT_ABILITY_DEXTERITY = 1;
const int CSWSEffectListHandler::EFFECT_ABILITY_CONSTITUTION = 2;
const int CSWSEffectListHandler::EFFECT_ABILITY_INTELLIGENCE = 3;
const int CSWSEffectListHandler::EFFECT_ABILITY_WISDOM = 4;
const int CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA = 5;
const int CSWSEffectListHandler::DAMAGE_FLAG_BLUDGEONING = 1;
const int CSWSEffectListHandler::DAMAGE_FLAG_PIERCING = 0x2;
const int CSWSEffectListHandler::DAMAGE_FLAG_SLASHING = 0x4;
const int CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL = 0x8;
const int CSWSEffectListHandler::DAMAGE_FLAG_ACID = 0x10;
const int CSWSEffectListHandler::DAMAGE_FLAG_COLD = 0x20;
const int CSWSEffectListHandler::DAMAGE_FLAG_LIGHT_SIDE = 0x40;
const int CSWSEffectListHandler::DAMAGE_FLAG_ELECTRICAL = 0x80;
const int CSWSEffectListHandler::DAMAGE_FLAG_FIRE = 0x100;
const int CSWSEffectListHandler::DAMAGE_FLAG_DARK_SIDE = 0x200;
const int CSWSEffectListHandler::DAMAGE_FLAG_SONIC = 1024;
const int CSWSEffectListHandler::DAMAGE_FLAG_ION = 0x800;
const int CSWSEffectListHandler::DAMAGE_FLAG_BLASTER = 0x1000;
const int CSWSEffectListHandler::DAMAGE_FLAG_POISON = 0x2000;
const int CSWSEffectListHandler::DAMAGE_FLAG_BASE_DAMAGE = 0x4000;
const int CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL = 0x4007;
const int CSWSEffectListHandler::DAMAGE_FLAG_ALL = 0x3fff;
const int CSWSEffectListHandler::DAMAGE_POWER_NORMAL = 0;
const int CSWSEffectListHandler::DAMAGE_POWER_PLUS_ONE = 1;
const int CSWSEffectListHandler::DAMAGE_POWER_PLUS_TWO = 2;
const int CSWSEffectListHandler::DAMAGE_POWER_PLUS_THREE = 3;
const int CSWSEffectListHandler::DAMAGE_POWER_PLUS_FOUR = 4;
const int CSWSEffectListHandler::DAMAGE_POWER_PLUS_FIVE = 5;
const int CSWSEffectListHandler::DAMAGE_POWER_ENERGY = 6;
const int CSWSEffectListHandler::AC_DODGE = 0;
const int CSWSEffectListHandler::AC_NATURAL = 1;
const int CSWSEffectListHandler::AC_ARMOUR_BONUS = 2;
const int CSWSEffectListHandler::AC_SHIELD_BONUS = 3;
const int CSWSEffectListHandler::AC_DEFLECTION = 4;
const int CSWSEffectListHandler::ATTACK_BONUS_MISC = 0;
const int CSWSEffectListHandler::ATTACK_BONUS_ONHAND = 1;
const int CSWSEffectListHandler::ATTACK_BONUS_OFFHAND = 2;
const int CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1 = 3;
const int CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2 = 4;
const int CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3 = 5;
const int CSWSEffectListHandler::ATTACK_BONUS_UNARMED = 7;
const int CSWSEffectListHandler::DAMAGE_BONUS_MISC = 0;
const int CSWSEffectListHandler::DAMAGE_BONUS_ONHAND = 1;
const int CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND = 2;
const int CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1 = 3;
const int CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2 = 4;
const int CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3 = 5;
const int CSWSEffectListHandler::DAMAGE_BONUS_UNARMED = 7;
const int CSWSEffectListHandler::IMMUNITY_TYPE_ALL = 0;
const int CSWSEffectListHandler::IMMUNITY_TYPE_MIND_AFFECTING = 1;
const int CSWSEffectListHandler::IMMUNITY_TYPE_POISON = 2;
const int CSWSEffectListHandler::IMMUNITY_TYPE_DISEASE = 3;
const int CSWSEffectListHandler::IMMUNITY_TYPE_FEAR = 4;
const int CSWSEffectListHandler::IMMUNITY_TYPE_MINE = 5;
const int CSWSEffectListHandler::IMMUNITY_TYPE_PARALYSIS = 6;
const int CSWSEffectListHandler::IMMUNITY_TYPE_BLINDNESS = 7;
const int CSWSEffectListHandler::IMMUNITY_TYPE_DEAFNESS = 8;
const int CSWSEffectListHandler::IMMUNITY_TYPE_SLOW = 9;
const int CSWSEffectListHandler::IMMUNITY_TYPE_ENTANGLE = 10;
const int CSWSEffectListHandler::IMMUNITY_TYPE_SILENCE = 11;
const int CSWSEffectListHandler::IMMUNITY_TYPE_STUN = 12;
const int CSWSEffectListHandler::IMMUNITY_TYPE_SLEEP = 13;
const int CSWSEffectListHandler::IMMUNITY_TYPE_CONFUSED = 16;
const int CSWSEffectListHandler::IMMUNITY_TYPE_CURSED = 17;
const int CSWSEffectListHandler::IMMUNITY_TYPE_DAZED = 18;
const int CSWSEffectListHandler::IMMUNITY_TYPE_ABILITY_DECREASE = 19;
const int CSWSEffectListHandler::IMMUNITY_TYPE_ATTACK_DECREASE = 20;
const int CSWSEffectListHandler::IMMUNITY_TYPE_DAMAGE_DECREASE = 21;
const int CSWSEffectListHandler::IMMUNITY_TYPE_DAMAGE_IMMUNITY_DECREASE = 22;
const int CSWSEffectListHandler::IMMUNITY_TYPE_AC_DECREASE = 23;
const int CSWSEffectListHandler::IMMUNITY_TYPE_MOVEMENT_SPEED_DECREASE = 24;
const int CSWSEffectListHandler::IMMUNITY_TYPE_SAVING_THROW_DECREASE = 25;
const int CSWSEffectListHandler::IMMUNITY_TYPE_FORCE_RESISTANCE_DECREASE = 26;
const int CSWSEffectListHandler::IMMUNITY_TYPE_SKILL_DECREASE = 27;
const int CSWSEffectListHandler::IMMUNITY_TYPE_KNOCKDOWN = 28;
const int CSWSEffectListHandler::IMMUNITY_TYPE_NEGATIVE_LEVEL = 29;
const int CSWSEffectListHandler::IMMUNITY_TYPE_BACKSTAB = 30;
const int CSWSEffectListHandler::IMMUNITY_TYPE_CRITICAL_HITS = 31;
const int CSWSEffectListHandler::IMMUNITY_TYPE_DEATH_MAGIC = 32;
const int CSWSEffectListHandler::IMMUNITY_TYPE_MAX = 33;
const int CSWSEffectListHandler::VISION_EFFECT_TYPE_BLINDVISION = 0;
const int CSWSEffectListHandler::VISION_EFFECT_TYPE_LOWLIGHTVISION = 1;
const int CSWSEffectListHandler::VISION_EFFECT_TYPE_DARKVISION = 2;
const int CSWSEffectListHandler::VISION_EFFECT_TYPE_ULTRAVISION = 3;
const int CSWSEffectListHandler::VISION_EFFECT_TYPE_BLIND = 4;
const int CSWSEffectListHandler::MISS_CHANCE_TYPE_NORMAL = 0;
const int CSWSEffectListHandler::MISS_CHANCE_TYPE_DARKNESS = 1;
const int CSWSEffectListHandler::VFX_NONE = -1;
// PC 0x0052B600
CSWSEffectListHandler::~CSWSEffectListHandler()
{
    if (m_pEffectApplied != NULL)
    {
        delete[] m_pEffectApplied;
        m_pEffectApplied = NULL;
    }

    if (m_pEffectRemoved != NULL)
    {
        delete[] m_pEffectRemoved;
        m_pEffectRemoved = NULL;
    }
}
// PC 0x0052B670
int CSWSEffectListHandler::OnEffectApplied(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    int nEffectType;
    EffectAppliedHandler pHandler;

    nEffectType = pEffect->m_nType;
    if (nEffectType > 0 && nEffectType < MAX_EFFECTS &&
        (pHandler = m_pEffectApplied[nEffectType]) != NULL)
    {
        return (this->*pHandler)(pObject, pEffect, bLoadingModule);
    }
    return 1;
}
// PC 0x0052B6B0
int CSWSEffectListHandler::OnEffectRemoved(CSWSObject* pObject,
                                           CGameEffect* pEffect)
{
    int nEffectType = pEffect->GetEffectType();
    if (pEffect->GetEffectType() < MAX_EFFECTS &&
        m_pEffectRemoved[nEffectType] != NULL)
    {
        return (this->*m_pEffectRemoved[nEffectType])(pObject, pEffect);
    }
    return 1;
}
// PC 0x0052BCF0
int CSWSEffectListHandler::OnApplyAssuredHit(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    CSWSCreature* pCreature;
    int nResult;

    nResult = 1;
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect != NULL && pCreature->m_bAssuredHit == 0)
        {
            pCreature->m_bAssuredHit = 1;
            nResult = 0;
        }
    }
    return nResult;
}
// PC 0x0052BD40
int CSWSEffectListHandler::OnRemoveAssuredHit(CSWSObject* pObject,
                                              CGameEffect* pEffect)
{
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_bAssuredHit = 0;
    }
    return 1;
}
// PC 0x0052BD70
int CSWSEffectListHandler::OnApplyAssuredDeflection(CSWSObject* pObject,
                                                    CGameEffect* pEffect,
                                                    int bLoadingModule)
{
    CSWSCreature* pCreature;
    int nResult;

    nResult = 1;
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pCreature->m_bAssuredDeflection == 0 &&
            pCreature->m_bHasAssuredDeflection == 0 && pEffect != NULL)
        {
            pCreature->m_bHasAssuredDeflection = 1;
            if (pEffect->GetInteger(0) != 0)
            {
                pCreature->m_bAssuredDeflection = 1;
            }
            nResult = 0;
        }
    }
    return nResult;
}
// PC 0x0052BDE0
int CSWSEffectListHandler::OnRemoveAssuredDeflection(CSWSObject* pObject,
                                                     CGameEffect* pEffect)
{
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_bHasAssuredDeflection = 0;
        pObject->AsSWSCreature()->m_bAssuredDeflection = 0;
    }
    return 1;
}
// PC 0x0052BE20
int CSWSEffectListHandler::OnApplyAbilityIncrease(CSWSObject* pObject,
                                                  CGameEffect* pEffect,
                                                  int bLoadingModule)
{
    CSWSCreature* pCreature;
    int nValue;

    if (pEffect == NULL)
    {
        return 1;
    }
    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        nValue = pEffect->GetInteger(1);
        if (bLoadingModule == 0 &&
            (((CSWSObject*)pCreature)->GetDead() != 0 ||
             ((CSWSObject*)pCreature)->GetIsPCDying() != 0 ||
             pCreature->GetDeadTemp() != 0))
        {
            return 1;
        }
        return nValue < 1;
    }
    return 1;
}
// PC 0x0052BE90
int CSWSEffectListHandler::OnRemoveAbilityIncrease(CSWSObject* pObject,
                                                   CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature();
        pEffect->GetInteger(0);
        pEffect->GetInteger(1);
    }

    return 1;
}
// PC 0x0052BED0
int CSWSEffectListHandler::OnApplyAbilityDecrease(CSWSObject* pObject,
                                                  CGameEffect* pEffect,
                                                  int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;
    CSWSCreature* pCreator;
    int nValue;

    if (pEffect == NULL)
    {
        return 1;
    }
    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            pStats = pCreature->m_pStats;
        }
        pCreator = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            pEffect->m_oidCreator);
        if (pStats->GetEffectImmunity(IMMUNITY_TYPE_ABILITY_DECREASE, pCreator) != 0)
        {
            return 1;
        }
        nValue = pEffect->GetInteger(1);
        if (bLoadingModule == 0 &&
            (((CSWSObject*)pCreature)->GetDead() != 0 ||
             ((CSWSObject*)pCreature)->GetIsPCDying() != 0 ||
             pCreature->GetDeadTemp() != 0))
        {
            return 1;
        }
        if (nValue < 1 || pCreature->m_bPlotObject != 0)
        {
            return 1;
        }
        return 0;
    }
    return 1;
}
// PC 0x0052BF80
int CSWSEffectListHandler::OnRemoveAbilityDecrease(CSWSObject* pObject,
                                                   CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature();
        pEffect->GetInteger(0);
    }

    return 1;
}
// PC 0x0052BFB0
int CSWSEffectListHandler::OnApplySummonCreature(CSWSObject* pObject,
                                                  CGameEffect* pEffect,
                                                  int bLoadingModule)
{
    if (bLoadingModule == 1)
        return 0;

    CSWSCreature* pSummoner = pObject->AsSWSCreature();
    if (pEffect != NULL && pSummoner != NULL)
    {
        CSWSCreature* pCreature = new CSWSCreature(OBJECT_INVALID, 0);
        if (pEffect->GetObjectID(0) == OBJECT_INVALID)
        {
            pEffect->SetObjectID(0, pObject->GetAreaId());
            pEffect->SetFloat(0, pObject->m_vPosition.m_fX);
            pEffect->SetFloat(1, pObject->m_vPosition.m_fY);
            pEffect->SetFloat(2, pObject->m_vPosition.m_fZ);
        }
        CSWSArea* pArea = g_pAppManager->GetServerExoApp()
                             ->GetAreaByGameObjectID(pEffect->GetObjectID(0));
        if (pArea != NULL)
        {
            float fX = pEffect->GetFloat(0);
            float fY = pEffect->GetFloat(1);
            float fZ = pEffect->GetFloat(2);
            float fDelay = 0.0f;
            if (pEffect->GetFloat(3) > 0.0f)
                fDelay = pEffect->GetFloat(3);

            CResRef cTemplate(pEffect->GetString(0));
            pCreature->LoadFromTemplate(cTemplate, 0);
            Vector vTargetPosition(fX, fY, fZ);
            Vector vSafePosition;
            if (pArea->ComputeSafeLocation(vTargetPosition, 20.0f,
                                           pCreature->GetPathfindInformation(),
                                           0, &vSafePosition) == 1)
            {
                fX = vSafePosition.m_fX;
                fY = vSafePosition.m_fY;
                fZ = pArea->ComputeHeight(vSafePosition, 0, NULL);
            }
            else
            {
                fX = vTargetPosition.m_fX;
                fY = vTargetPosition.m_fY;
                fZ = vTargetPosition.m_fZ;
            }
            int nVisualEffect = pEffect->GetInteger(0);
            if (nVisualEffect != VFX_NONE)
            {
                CGameEffect* pVisualEffect = new CGameEffect(1);
                pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
                pVisualEffect->m_nSubType =
                    (pVisualEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                pVisualEffect->SetInteger(0, nVisualEffect);
                pArea->ApplyEffect(pVisualEffect, Vector(fX, fY, fZ),
                                   pSummoner->m_vOrientation);
            }
            unsigned long oidCreature = pCreature->GetId();
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->SetFloat(0, fX);
            pEvent->SetFloat(1, fY);
            pEvent->SetFloat(2, fZ);
            pEvent->SetObjectID(0, oidCreature);
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                ->AddEventDeltaTime(0, (unsigned long)fDelay * 1000,
                                   pObject->GetId(), pObject->GetId(),
                                   CServerAIMaster::EVENT_SUMMON_CREATURE,
                                   pEvent);
            pEffect->SetObjectID(1, oidCreature);
        }
        else
        {
            delete pCreature;
        }
    }
    return 0;
}
// PC 0x0052C5C0
int CSWSEffectListHandler::OnRemoveSummonCreature(CSWSObject* pObject,
                                                  CGameEffect* pEffect)
{
    CServerExoApp* pServerExoApp;
    unsigned int oidCreature;
    CSWSCreature* pCreature;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidCreature = pEffect->GetObjectID(1);
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        pCreature->UnsummonMyself();
    }
    return 1;
}
// PC 0x0052C5C0
int CSWSEffectListHandler::OnRemoveSummonParty(CSWSObject* pObject,
                                               CGameEffect* pEffect)
{
    CServerExoApp* pServerExoApp;
    unsigned int oidCreature;
    CSWSCreature* pCreature;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidCreature = pEffect->GetObjectID(1);
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        pCreature->UnsummonMyself();
    }
    return 1;
}
// PC 0x0052C340
int CSWSEffectListHandler::OnApplySummonParty(CSWSObject* pObject,
                                               CGameEffect* pEffect,
                                               int bLoadingModule)
{
    if (g_pAppManager->GetClientExoApp()->GetSWParty()->m_nCharacters >=
        CSWParty::MAX_MEMBERS)
    {
        return 1;
    }

    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pEffect != NULL && pCreature != NULL)
    {
        pCreature = new CSWSCreature(OBJECT_INVALID, 0);
        pCreature->SetInParty(1, 1);
        CResRef cTemplate(pEffect->GetString(0));
        pCreature->LoadFromTemplate(cTemplate, 0);
        if (pEffect->GetObjectID(0) == OBJECT_INVALID)
        {
            pEffect->SetObjectID(0, pObject->m_oidArea);
            pEffect->SetFloat(0, pObject->m_vPosition.m_fX);
            pEffect->SetFloat(1, pObject->m_vPosition.m_fY);
            pEffect->SetFloat(2, pObject->m_vPosition.m_fZ);
        }
        CSWSArea* pArea = g_pAppManager->GetServerExoApp()
                             ->GetAreaByGameObjectID(pEffect->GetObjectID(0));
        if (pArea != NULL)
        {
            float fX = pEffect->GetFloat(0);
            float fY = pEffect->GetFloat(1);
            float fZ = pEffect->GetFloat(2);
            Vector vTargetPosition(fX, fY, fZ);
            Vector vSafePosition;
            if (pArea->ComputeSafeLocation(vTargetPosition, 10.0f,
                                           pCreature->GetPathfindInformation(),
                                           0, &vSafePosition) == 1)
            {
                fX = vSafePosition.m_fX;
                fY = vSafePosition.m_fY;
                fZ = pArea->ComputeHeight(vSafePosition, 1, NULL);
            }
            else
            {
                fX = vTargetPosition.m_fX;
                fY = vTargetPosition.m_fY;
                fZ = vTargetPosition.m_fZ;
            }
            pCreature->AddToArea(pArea, fX, fY, fZ, 0, 0);
            pCreature->m_bCreationScriptFired = 1;
            g_pVirtualMachine->RunScript(
                &pCreature->m_sScripts[CSWSCreature::SCRIPT_SPAWN],
                pCreature->GetId(), 1);
            pEffect->SetObjectID(1, pCreature->GetId());
        }
        else
        {
            delete pCreature;
        }
    }
    return 0;
}
// PC 0x0052C5F0
int CSWSEffectListHandler::OnApplySavingThrowIncrease(CSWSObject* pObject,
                                                      CGameEffect* pEffect,
                                                      int bLoadingModule)
{
    CSWSCreature* pCreature;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            if (pEffect->GetInteger(0) < 1)
            {
                return 1;
            }
            pCreature->m_nUnknown344 = 1;
        }
    }
    return 0;
}
// PC 0x0052C640
int CSWSEffectListHandler::OnApplySavingThrowDecrease(CSWSObject* pObject,
                                                      CGameEffect* pEffect,
                                                      int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pCreator = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            pEffect->m_oidCreator);
        if (pCreature->m_pStats->GetEffectImmunity(IMMUNITY_TYPE_SAVING_THROW_DECREASE, pCreator) != 0 ||
            pEffect->GetInteger(0) < 1 || pCreature->m_bPlotObject != 0)
        {
            return 1;
        }
        pCreature->m_nUnknown344 = 1;
    }
    return 0;
}
// PC 0x0052C6C0
int CSWSEffectListHandler::OnApplyACIncrease(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    CSWSCreature* pCreature;
    int nType;
    int nBonus;
    int nRace;
    int nAlignmentGroup1;
    int nAlignmentGroup2;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            nType = pEffect->GetInteger(AC_INTEGER_TYPE);
            nBonus = pEffect->GetInteger(AC_INTEGER_BONUS);
            nRace = pEffect->GetInteger(AC_INTEGER_RACIAL_GROUP);
            nAlignmentGroup1 = pEffect->GetInteger(AC_INTEGER_ALIGNMENT_GROUP1);
            nAlignmentGroup2 = pEffect->GetInteger(AC_INTEGER_ALIGNMENT_GROUP2);

            pEffect->GetInteger(AC_INTEGER_DAMAGE_FLAGS);
            if (nBonus < 1)
                return 1;
            if (nRace == g_pRules->m_nNumRaces &&
                nAlignmentGroup1 == 0 && nAlignmentGroup2 == 0)
            {
                switch (nType)
                {
                case AC_DODGE_BONUS:
                    pCreature->m_pStats->m_nACDodgeBonus += nBonus;
                    break;
                case AC_NATURAL_BONUS:
                    if (pCreature->m_pStats->m_nACNaturalBonus > 0)
                        pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_AC_NATURAL_BONUS_ACTIVE, NULL);
                    if (nBonus > pCreature->m_pStats->m_nACNaturalBonus)
                        pCreature->m_pStats->m_nACNaturalBonus = nBonus;
                    break;
                case AC_ARMOUR_ENCHANTMENT_BONUS:
                    if (pCreature->m_pStats->m_nACArmorBonus > 0)
                        pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_AC_ARMOUR_BONUS_ACTIVE, NULL);
                    if (nBonus > pCreature->m_pStats->m_nACArmorBonus)
                        pCreature->m_pStats->m_nACArmorBonus = nBonus;
                    break;
                case AC_SHIELD_ENCHANTMENT_BONUS:
                    if (pCreature->m_pStats->m_nACShieldBonus > 0)
                        pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_AC_SHIELD_BONUS_ACTIVE, NULL);
                    if (nBonus > pCreature->m_pStats->m_nACShieldBonus)
                        pCreature->m_pStats->m_nACShieldBonus = nBonus;
                    break;
                case AC_DEFLECTION_BONUS:
                    if (pCreature->m_pStats->m_nACDeflectionBonus > 0)
                        pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_AC_DEFLECTION_BONUS_ACTIVE, NULL);
                    if (nBonus > pCreature->m_pStats->m_nACDeflectionBonus)
                        pCreature->m_pStats->m_nACDeflectionBonus = nBonus;
                    break;
                }
            }
        }
    }
    return 0;
}
// PC 0x0052C8D0
int CSWSEffectListHandler::OnApplyACDecrease(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    int nType;
    int nPenalty;
    int nRace;
    int nUnknown3;
    int nUnknown4;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pCreator = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            pEffect->m_oidCreator);
        if (pCreature->m_pStats->GetEffectImmunity(IMMUNITY_TYPE_AC_DECREASE, pCreator) != 0)
        {
            return 1;
        }
        nType = pEffect->GetInteger(0);
        nPenalty = pEffect->GetInteger(1);
        nRace = pEffect->GetInteger(2);
        nUnknown3 = pEffect->GetInteger(3);
        nUnknown4 = pEffect->GetInteger(4);
        pEffect->GetInteger(5);
        if (nPenalty < 1 || pCreature->m_bPlotObject != 0)
        {
            return 1;
        }
        if (nRace != (unsigned char)g_pRules->m_nNumRaces || nUnknown3 != 0 ||
            nUnknown4 != 0)
        {
            return 0;
        }
        switch (nType)
        {
        case AC_DODGE:
            pCreature->m_pStats->m_nACDodgePenalty += nPenalty;
            break;
        case AC_NATURAL:
            if (nPenalty > pCreature->m_pStats->m_nACNaturalPenalty)
            {
                pCreature->m_pStats->m_nACNaturalPenalty = nPenalty;
            }
            break;
        case AC_ARMOUR_BONUS:
            if (nPenalty > pCreature->m_pStats->m_nACArmorPenalty)
            {
                pCreature->m_pStats->m_nACArmorPenalty = nPenalty;
            }
            break;
        case AC_SHIELD_BONUS:
            if (nPenalty > pCreature->m_pStats->m_nACShieldPenalty)
            {
                pCreature->m_pStats->m_nACShieldPenalty = nPenalty;
            }
            break;
        case AC_DEFLECTION:
            if (nPenalty > pCreature->m_pStats->m_nACDeflectionPenalty)
            {
                pCreature->m_pStats->m_nACDeflectionPenalty = nPenalty;
            }
            break;
        }
    }
    return 0;
}
// PC 0x0052CA70
int CSWSEffectListHandler::OnApplyAttackIncrease(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    if (pObject->AsSWSCreature() != NULL && pEffect != NULL)
    {
        if (pEffect->GetInteger(0) < 1)
        {
            return 1;
        }
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 0;
}
// PC 0x0052CAC0
int CSWSEffectListHandler::OnApplyAttackDecrease(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    CSWSCreature* pCreator;
    CSWSCreatureStats* pStats;
    unsigned long oidCreator;

    if (pObject->AsSWSCreature() != NULL && pEffect != NULL)
    {
        if (pEffect->GetInteger(0) < 1)
        {
            return 1;
        }
        if (pObject->m_bPlotObject != 0)
        {
            return 1;
        }
        oidCreator = pEffect->m_oidCreator;
        pCreator = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            oidCreator);
        pStats = pObject->AsSWSCreature()->m_pStats;
        if (pStats->GetEffectImmunity(IMMUNITY_TYPE_ATTACK_DECREASE, pCreator) != 0)
        {
            return 1;
        }
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 0;
}
// PC 0x0052CB50
int CSWSEffectListHandler::OnApplyDamageIncrease(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 0;
}
// PC 0x0052CB80
int CSWSEffectListHandler::OnApplyDamageDecrease(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    CSWSCreature* pCreator;
    CSWSCreatureStats* pStats;
    unsigned long oidCreator;

    if (pObject->AsSWSCreature() != NULL)
    {
        oidCreator = pEffect->m_oidCreator;
        pCreator = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            oidCreator);
        pStats = pObject->AsSWSCreature()->m_pStats;
        if (pStats->GetEffectImmunity(IMMUNITY_TYPE_DAMAGE_DECREASE, pCreator) != 0)
        {
            return 1;
        }
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 0;
}
// PC 0x0052CBF0
int CSWSEffectListHandler::OnRemoveSavingThrowIncrease(CSWSObject* pObject,
                                                       CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 1;
}
// PC 0x0052CBF0
int CSWSEffectListHandler::OnRemoveSavingThrowDecrease(CSWSObject* pObject,
                                                       CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 1;
}
// PC 0x0052CBF0
int CSWSEffectListHandler::OnRemoveAttackIncrease(CSWSObject* pObject,
                                                  CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 1;
}
// PC 0x0052CBF0
int CSWSEffectListHandler::OnRemoveAttackDecrease(CSWSObject* pObject,
                                                  CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 1;
}
// PC 0x0052CBF0
int CSWSEffectListHandler::OnRemoveDamageIncrease(CSWSObject* pObject,
                                                  CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 1;
}
// PC 0x0052CBF0
int CSWSEffectListHandler::OnRemoveDamageDecrease(CSWSObject* pObject,
                                                  CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_nUnknown344 = 1;
    }
    return 1;
}
// PC 0x0052CC20
int CSWSEffectListHandler::OnApplyTemporaryHitpoints(CSWSObject* pObject,
                                                     CGameEffect* pEffect,
                                                     int bLoadingModule)
{
    CSWSCreature* pCreature;
    short nTemporaryHitPoints;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            nTemporaryHitPoints = pCreature->m_nTemporaryHitPoints;
            pCreature->m_nTemporaryHitPoints =
                nTemporaryHitPoints + pEffect->GetInteger(0);
        }
    }
    return 0;
}
// PC 0x0052CC70
int CSWSEffectListHandler::OnRemoveTemporaryHitpoints(CSWSObject* pObject,
                                                      CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    int bWasPCDying;
    int bWasDead;
    int bWasDeadTemp;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        bWasPCDying = pCreature->GetIsPCDying();
        bWasDead = pCreature->GetDead();
        bWasDeadTemp = pCreature->GetDeadTemp();
        if (pEffect != NULL)
        {
            short nTemporaryHitPoints;
            pCreature->m_nTemporaryHitPoints = max(
                (nTemporaryHitPoints = pCreature->m_nTemporaryHitPoints) -
                    pEffect->GetInteger(0), 0);
        }
        if (!bWasPCDying && pCreature->GetIsPCDying() == 1)
        {
            pCreature->RecomputeAmbientAnimationState();
            pCreature->m_bCommandable = 0;
            unsigned long oidModule =
                g_pAppManager->GetServerExoApp()->GetModule()->GetId();
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_DYING;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, pCreature->GetId(), oidModule,
                CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            pCreature->AutoCloseGUIPanels(0);
        }
        else if ((!bWasDead && pCreature->GetDead() == 1) ||
                 (!bWasDeadTemp && pCreature->GetDeadTemp() == 1))
        {
            CGameEffect* pDeathEffect = new CGameEffect(1);
            pDeathEffect->m_nType = EFFECT_DEATH;
            pDeathEffect->m_nSubType =
                (pDeathEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                CGameEffect::DURATION_TYPE_INSTANTANEOUS;
            pDeathEffect->SetCreator(OBJECT_INVALID);
            CExoString sBLOODCOLR;
            {
                CExoString sColumn("BLOODCOLR");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pAppearance = p2DArrays->m_pAppearance;
                pAppearance->GetCExoStringEntry(
                    pCreature->m_cAppearanceInfo.m_nAppearance, sColumn, &sBLOODCOLR);
            }
            CGameEffect* pBloodEffect = new CGameEffect(1);
            pBloodEffect->m_nType = EFFECT_VISUALEFFECT;
            pBloodEffect->m_nSubType = CGameEffect::DURATION_TYPE_INSTANTANEOUS |
                (pBloodEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
            if (sBLOODCOLR == "R")
            {
                pBloodEffect->SetInteger(0, VFX_BLOOD_RED);
            }
            else if (sBLOODCOLR == "G")
            {
                pBloodEffect->SetInteger(0, VFX_BLOOD_GREEN);
            }
            else if (sBLOODCOLR == "Y")
            {
                pBloodEffect->SetInteger(0, VFX_BLOOD_YELLOW);
            }
            pCreature->ApplyEffect(pBloodEffect, 0, 0);
            pDeathEffect->SetInteger(1, 1);
            pCreature->ApplyEffect(pDeathEffect, 0, 0);
        }
    }
    return 1;
}
// PC 0x0052CF10
int CSWSEffectListHandler::OnApplyDamageImmunityIncrease(CSWSObject* pObject,
                                                         CGameEffect* pEffect,
                                                         int bLoadingModule)
{
    int nDamageType;
    int nPercent;

    if (pEffect != NULL)
    {
        nDamageType = pEffect->GetInteger(0);
        nPercent = pEffect->GetInteger(1);
        if (nPercent < 0)
        {
            return 1;
        }
        nPercent += (char)pObject->GetDamageImmunityByFlags(nDamageType);
        pObject->SetDamageImmunity(nDamageType, nPercent);
    }
    return 0;
}
// PC 0x0052CF70
int CSWSEffectListHandler::OnApplyDamageImmunityDecrease(CSWSObject* pObject,
                                                         CGameEffect* pEffect,
                                                         int bLoadingModule)
{
    int nDamageType;
    int nPercent;
    CSWSCreature* pCreator;
    CSWSCreatureStats* pStats;
    unsigned long oidCreator;
    int nValue;

    if (pEffect != NULL)
    {
        nDamageType = pEffect->GetInteger(0);
        nPercent = pEffect->GetInteger(1);
        if (nPercent < 0)
        {
            return 1;
        }
        if (pObject->m_bPlotObject != 0)
        {
            return 1;
        }
        if (pObject->AsSWSCreature() != NULL)
        {
            oidCreator = pEffect->m_oidCreator;
            pCreator =
                g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                    oidCreator);
            pStats = pObject->AsSWSCreature()->m_pStats;
            if (pStats->GetEffectImmunity(IMMUNITY_TYPE_DAMAGE_IMMUNITY_DECREASE, pCreator) != 0)
            {
                return 1;
            }
        }
        char nImmunity = pObject->GetDamageImmunityByFlags(nDamageType);
        nValue = nImmunity - nPercent;
        pObject->SetDamageImmunity(nDamageType, nValue);
    }
    return 0;
}
// PC 0x0052D010
int CSWSEffectListHandler::OnApplyEntangled(CSWSObject* pObject,
                                          CGameEffect* pEffect,
                                          int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    CSWSCreatureStats* pStats;
    int bImmune;
    int bPlot;
    CSWCCMessageData* pMessageData;
    CSWCCMessageData* pCreatorMessageData;
    CGameEffect* pAttackEffect;
    CGameEffect* pAbilityEffect;
    CGameEffect* pMovementEffect;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pCreator = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            pEffect->m_oidCreator);
        pStats = pCreature->m_pStats;
        bImmune = pStats->GetEffectImmunity(
            CSWSCreatureStats::IMMUNITY_TYPE_ENTANGLE, pCreator);
        if (bImmune != 0)
        {
            pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, pObject->GetId());
            if (pCreator != NULL)
            {
                pCreatorMessageData = new CSWCCMessageData;
                pMessageData->CopyTo(pCreatorMessageData);
                pCreator->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_ENTANGLE_IMMUNITY, pCreatorMessageData);
            }
            pObject->AsSWSCreature()->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_ENTANGLE_IMMUNITY, pMessageData);
            return 1;
        }
        bPlot = pCreature->m_bPlotObject;
        if (bPlot != 0)
        {
            return 1;
        }
        pCreature->ClearAllActions(1);
        pAttackEffect = new CGameEffect(pEffect);
        pAbilityEffect = new CGameEffect(pEffect);
        pMovementEffect = new CGameEffect(pEffect);
        pAttackEffect->m_nType = EFFECT_ATTACK_DECREASE;
        pAttackEffect->SetInteger(0, ENTANGLE_ATTACK_PENALTY);
        pAttackEffect->SetInteger(1, 0);
        pAttackEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
        pCreature->ApplyEffect(pAttackEffect, bLoadingModule, 0);
        pAbilityEffect->m_nType = EFFECT_ABILITY_DECREASE;
        pAbilityEffect->SetInteger(0, CSWRules::ABILITY_DEXTERITY);
        pAbilityEffect->SetInteger(1, ENTANGLE_DEXTERITY_PENALTY);
        pCreature->ApplyEffect(pAbilityEffect, bLoadingModule, 0);
        pMovementEffect->m_nType = EFFECT_SET_AI_STATE;
        pMovementEffect->SetInteger(0, ENTANGLE_MOVEMENT_DECREASE);
        pCreature->ApplyEffect(pMovementEffect, bLoadingModule, 0);
    }
    return 0;
}
// PC 0x0052D260
int CSWSEffectListHandler::OnRemoveEntangled(CSWSObject* pObject,
                                             CGameEffect* pEffect)
{
    CSWSCreature* pCreature;

    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pCreature->m_nAreaWaitState == 0)
        {
            pCreature->ClearVisibleList();
            pCreature->UpdateVisibleList(0);
        }
    }
    return 1;
}
// PC 0x0052D2A0
int CSWSEffectListHandler::OnApplyKnockdown(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    unsigned long oidCreator = pEffect->m_oidCreator;
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()
            ->GetCreatureByGameObjectID(oidCreator);
        CSWSCreatureStats* pStats = pObject->AsSWSCreature()->m_pStats;
        if (pStats->GetEffectImmunity(
                CSWSCreatureStats::IMMUNITY_TYPE_KNOCKDOWN, pCreator) != 0)
        {
            CSWCCMessageData* pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, pObject->GetId());
            if (pCreator != NULL)
            {
                CSWCCMessageData* pCreatorMessageData = new CSWCCMessageData;
                pMessageData->CopyTo(pCreatorMessageData);
                pCreator->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_KNOCKDOWN_IMMUNITY,
                    pCreatorMessageData);
            }
            pObject->AsSWSCreature()->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_KNOCKDOWN_IMMUNITY, pMessageData);
            return 1;
        }
        if (pCreature->m_bPlotObject != 0)
        {
            return 1;
        }
        if (!(pCreature->GetDead() != 0 ||
              pCreature->GetIsPCDying() != 0 ||
              pCreature->GetDeadTemp() != 0))
        {
            pCreature->m_pcCombatRound->SetRoundPaused(1, oidCreator);
            pCreature->m_pcCombatRound->SetPauseTimer(0, 1);
            CGameEffect* pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_fDuration = pEffect->GetDuration() + 1.5f;
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, KNOCKDOWN_AI_STATE_MASK);
            pObject->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pObject->m_bCommandable = 0;
            CGameObject* pCreatorObject;
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()
                    ->GetGameObject(oidCreator, &pCreatorObject) ==
                CGameObjectArray::SUCCESS)
            {
                CSWSCreature* pCreatorCreature = pCreatorObject->AsSWSCreature();
                Vector vDirection;
                vDirection = normalize(
                    pCreatorCreature->m_vPosition - pCreature->m_vPosition);
                if (vDirection * normalize(pCreature->m_vOrientation) >= 0.707)
                {
                    pCreature->SetAnimation(ANIMATION_KNOCKDOWN_FRONT);
                }
                else
                {
                    pCreature->SetAnimation(ANIMATION_KNOCKDOWN_BACK);
                }
            }
            else
            {
                pCreature->SetAnimation(ANIMATION_KNOCKDOWN_FRONT);
            }
        }
    }
    return 0;
}
// PC 0x0052D5A0
int CSWSEffectListHandler::OnRemoveKnockdown(CSWSObject* pObject,
                                            CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CSWSCombatRound* pCombatRound;

    pCreature = pObject->AsSWSCreature();
    if (pCreature == NULL || pEffect == NULL)
    {
        return 1;
    }

    pCombatRound = pCreature->m_pcCombatRound;
    if (pCombatRound->m_bRoundPaused != 0)
    {
        pCombatRound->SetRoundPaused(0, OBJECT_INVALID);
        pCreature->m_pcCombatRound->SetPauseTimer(0, 0);
    }

    if (!(pCreature->GetDead() != 0 ||
          pCreature->GetIsPCDying() != 0 ||
          pCreature->GetDeadTemp() != 0))
    {
        pCreature->SetAnimation(CSWSObject::ANIMATION_COMBAT_READY);
        pObject->m_bCommandable = 1;
        return 1;
    }
    return 1;
}
// PC 0x0052D630
int CSWSEffectListHandler::OnApplyDisarm(CSWSObject* pObject,
                                       CGameEffect* pEffect,
                                       int bLoadingModule)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        Vector vPosition;
        Vector vSafePosition;
        if (pCreature->m_nDisarmable != 0)
        {
            unsigned long oidCreator = pEffect->m_oidCreator;
            CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID(oidCreator);
            CSWSItem* pItem;
            if (pEffect->GetInteger(0) == 1 && pCreator != NULL &&
                pCreator->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) == NULL &&
                (pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND)) != NULL)
            {
                pCreature->RemoveItem(pItem, 0, 1, NULL);
                if (pItem->m_bDropable)
                {
                    if (!pCreator->GetItemRepository(1)->AddItem(&pItem, 1, 0, 1))
                    {
                        if (pItem != NULL)
                        {
                            vSafePosition = pCreator->m_vPosition;
                            vSafePosition.m_fZ += 0.1f;
                            pItem->AddToArea(pCreature->GetArea(), vSafePosition.m_fX,
                                vSafePosition.m_fY, vSafePosition.m_fZ, 1);
                        }
                    }
                    if (pItem != NULL)
                    {
                        pItem->SetPossessor(pCreator->GetId(), 1, 1, OBJECT_INVALID);
                    }
                    return 1;
                }
                else if (pItem->m_nEC == 1)
                {
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                        ->AddEventDeltaTime(0, 0, pCreator->GetId(), pItem->GetId(),
                            CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                }
            }
            else
            {
                pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                if (pItem != NULL)
                {
                    pCreature->RemoveItem(pItem, 0, 1, NULL);
                    if (pItem->m_bDropable)
                    {
                        vPosition = pCreature->m_vPosition;
                        if (rand() % 2)
                            vPosition.m_fX += (rand() % 25) / 10.0f;
                        else
                            vPosition.m_fX -= (rand() % 25) / 10.0f;
                        if (rand() % 2)
                            vPosition.m_fY += (rand() % 25) / 10.0f;
                        else
                            vPosition.m_fY -= (rand() % 25) / 10.0f;
                        pItem->SetPossessor(OBJECT_INVALID, 1, 1, OBJECT_INVALID);
                        pCreature->GetArea()->ComputeSafeLocation(vPosition, 20.0f,
                            pCreature->GetPathfindInformation(), 1, &vSafePosition);
                        vSafePosition.m_fZ = pCreature->GetArea()
                            ->ComputeHeight(vSafePosition, 0, NULL) + 0.2f;
                        pItem->AddToArea(pCreature->GetArea(), vSafePosition.m_fX,
                            vSafePosition.m_fY, vSafePosition.m_fZ, 1);
                    }
                    else if (pItem->m_nEC == 1)
                    {
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                            ->AddEventDeltaTime(0, 0, pCreator->GetId(), pItem->GetId(),
                                CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                    }
                }
            }
        }
    }
    return 1;
}
// PC 0x0052D9A0
int CSWSEffectListHandler::OnApplySetAIState(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    CSWSCreature* pCreature;
    short nAIStateFlags;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            nAIStateFlags = pCreature->m_nAIStateFlags;
            pCreature->m_nAIStateFlags = nAIStateFlags & pEffect->GetInteger(0);
        }
    }
    return 0;
}
// PC 0x0052D9E0
int CSWSEffectListHandler::OnApplyDeaf(CSWSObject* pObject,
                                      CGameEffect* pEffect,
                                      int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    CSWCCMessageData* pMessageData;
    CSWCCMessageData* pCreatorMessageData;
    CGameEffect* pAIStateEffect;
    CGameEffect* pSpellFailureEffect;
    int bPlot;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        unsigned long oidCreator = pEffect->m_oidCreator;
        pCreator = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
            oidCreator);
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        int bImmune = pStats->GetEffectImmunity(
            CSWSCreatureStats::IMMUNITY_TYPE_DEAFNESS, pCreator);
        if (bImmune != 0)
        {
            pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, pObject->GetId());
            if (pCreator != NULL)
            {
                pCreatorMessageData = new CSWCCMessageData;
                pMessageData->CopyTo(pCreatorMessageData);
                pCreator->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_DEAFNESS_IMMUNITY, pCreatorMessageData);
            }
            pObject->AsSWSCreature()->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_DEAFNESS_IMMUNITY, pMessageData);
            return 1;
        }
        bPlot = pCreature->m_bPlotObject;
        if (bPlot != 0)
        {
            return 1;
        }
        pAIStateEffect = new CGameEffect(pEffect);
        pSpellFailureEffect = new CGameEffect(pEffect);
        pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
        pAIStateEffect->SetInteger(0, DEAF_AI_STATE_MASK);
        pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
        pSpellFailureEffect->m_nType = EFFECT_ARCANE_SPELL_FAILURE;
        pSpellFailureEffect->SetInteger(0, DEAF_ARCANE_SPELL_FAILURE);
        pCreature->ApplyEffect(pSpellFailureEffect, bLoadingModule, 0);
    }
    return 0;
}
// PC 0x0052DBB0
int CSWSEffectListHandler::OnApplyEnemyAttackBonus(CSWSObject* pObject,
                                                   CGameEffect* pEffect,
                                                   int bLoadingModule)
{
    CSWSCreatureStats* pStats;
    char nEnemyAttackBonus;

    if (pObject->AsSWSCreature() != NULL)
    {
        if (pObject->AsSWSCreature()->m_bPlotObject != 0)
        {
            return 1;
        }
        pStats = pObject->AsSWSCreature()->m_pStats;
        if (pEffect != NULL)
        {
            nEnemyAttackBonus = pStats->m_nEnemyAttackBonus;
            pStats->m_nEnemyAttackBonus =
                nEnemyAttackBonus + pEffect->GetInteger(0);
        }
    }
    return 0;
}
// PC 0x0052DC10
int CSWSEffectListHandler::OnApplyArcaneSpellFailure(CSWSObject* pObject,
                                                     CGameEffect* pEffect,
                                                     int bLoadingModule)
{
    CSWSCreatureStats* pStats;
    unsigned char nFailure;

    if (pObject->AsSWSCreature() != NULL)
    {
        pStats = pObject->AsSWSCreature()->m_pStats;
        if (pEffect != NULL)
        {
            if (pObject->AsSWSCreature() != NULL)
            {
                pObject->AsSWSCreature()->m_nUnknown344 = 1;
            }
            nFailure = pStats->m_nArcaneSpellFailure;
            pStats->SetArcaneSpellFailure(nFailure + pEffect->GetInteger(0));
        }
    }
    return 0;
}
// PC 0x0052DC80
int CSWSEffectListHandler::OnApplySetStateInternal(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    CSWSCreature* pCreature;
    CGameEffect* pAIStateEffect;
    CGameEffect* pVisualEffect;
    CGameEffect* pSaveEffect;
    int nState;
    int bActionsCleared;
    unsigned long nExpiryDay;
    unsigned long nExpiryTime;

    pCreature = pObject->AsSWSCreature();
    bActionsCleared = 0;
    if (pCreature != NULL && pEffect != NULL)
    {
        nState = pEffect->GetInteger(0);
        pCreature->m_nLastHeartbeatDay = 0;
        pCreature->m_nLastHeartbeatTime = 1;
        bActionsCleared = 0;
        if (nState != STATE_FORCE_PUSH)
        {
            pCreature->m_bCommandable = 1;
            pCreature->ClearAllActions(1);
            pCreature->m_pcCombatRound->RemoveAllActions();
            pCreature->m_bCommandable = 0;
            bActionsCleared = 1;
        }
        switch (nState)
        {
        case STATE_STUN:
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_NO_MOVEMENT);
            pEffect->GetExpiryTime(&nExpiryDay, &nExpiryTime);
            pAIStateEffect->SetExpiryTime(nExpiryDay, nExpiryTime);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pVisualEffect = new CGameEffect(pEffect);
            pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
            pVisualEffect->SetInteger(0, VFX_STUN);
            pVisualEffect->SetExpiryTime(nExpiryDay, nExpiryTime);
            pCreature->ApplyEffect(pVisualEffect, bLoadingModule, 0);
            pCreature->RecomputeAmbientAnimationState();
            break;
        case STATE_CHOKE:
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_NO_MOVEMENT);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pCreature->RecomputeAmbientAnimationState();
            break;
        case STATE_FORCE_PUSH:
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_NO_MOVEMENT);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pCreature->RecomputeAmbientAnimationState();
            break;
        case STATE_FEAR:
            pSaveEffect = new CGameEffect(pEffect);
            pSaveEffect->m_nType = EFFECT_SAVING_THROW_DECREASE;
            pSaveEffect->SetInteger(0, SETSTATE_SAVING_THROW_PENALTY);
            pSaveEffect->SetInteger(1, 0);
            pSaveEffect->SetInteger(3, (unsigned char)g_pRules->m_nNumRaces);
            pCreature->ApplyEffect(pSaveEffect, bLoadingModule, 0);
            break;
        case STATE_DROID_STUN:
            pVisualEffect = new CGameEffect(pEffect);
            pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
            pVisualEffect->SetInteger(0, VFX_DROID_STUN);
            pCreature->ApplyEffect(pVisualEffect, bLoadingModule, 0);
            pSaveEffect = new CGameEffect(pEffect);
            pSaveEffect->m_nType = EFFECT_SAVING_THROW_DECREASE;
            pSaveEffect->SetInteger(0, SETSTATE_SAVING_THROW_PENALTY);
            pSaveEffect->SetInteger(1, 0);
            pSaveEffect->SetInteger(3, (unsigned char)g_pRules->m_nNumRaces);
            pCreature->ApplyEffect(pSaveEffect, bLoadingModule, 0);
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_NO_MOVEMENT);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pCreature->RecomputeAmbientAnimationState();
            break;
        case STATE_HOLD:
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_HOLD);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pCreature->RecomputeAmbientAnimationState();
            break;
        case STATE_SLEEP:
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_SLEEP);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pCreature->RecomputeAmbientAnimationState();
            break;
        case STATE_WHIRLWIND:
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_NO_MOVEMENT);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pCreature->RecomputeAmbientAnimationState();
            break;
        case STATE_HORRIFIED:
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_NO_MOVEMENT);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            pCreature->RecomputeAmbientAnimationState();
            break;
        }
    }
    unsigned long oidCreator = pEffect->m_oidCreator;
    CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()
        ->GetCreatureByGameObjectID(oidCreator);
    if (bActionsCleared == 0 && pCreator != NULL)
    {
        CSWSCombatAttackData* pAttack = pCreator->m_pcCombatRound->GetAttack(
            pCreator->m_pcCombatRound->m_nCurrentAttack);
        if (pAttack != NULL)
        {
            pAttack->m_Extra4.m_nState = (unsigned char)nState;
            pAttack->m_Extra4.m_nUnknown14 = 1;
            pCreator->SetAttackSummaryFeedbackEffectData(nState, pCreature->GetId());
        }
    }
    return 0;
}
int CSWSEffectListHandler::OnRemoveSetStateInternal(CSWSObject* pObject,  // PC 0x0052E1F0
                                                    CGameEffect* pEffect)
{
    CSWSCreature* pCreature;

    pCreature = pObject->AsSWSCreature();
    if (pEffect != NULL && pCreature != NULL)
    {
        switch (pEffect->GetInteger(0))
        {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            g_pVirtualMachine->RunScript(&pCreature->m_sScripts[CSWSCreature::SCRIPT_END_ROUND],
                                         pCreature->m_nId, 1);
        case 12:
        case 13:
            pCreature->RecomputeAmbientAnimationState();
            break;
        }
        if (pCreature->m_nAreaWaitState == 0)
        {
            pCreature->ClearVisibleList();
            pCreature->UpdateVisibleList(0);
        }
    }
    return 1;
}
// PC 0x0052E290
int CSWSEffectListHandler::OnApplyMovementSpeedIncrease(CSWSObject* pObject,
                                                        CGameEffect* pEffect,
                                                        int bLoadingModule)
{
    CSWSCreature* pCreature;
    float fFactor;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect->GetInteger(0) < 100)
        {
            pEffect->SetInteger(0, pEffect->GetInteger(0) + 100);
        }
        fFactor = pEffect->GetInteger(0) * 0.01f;
        pCreature->SetMovementRateFactor(pCreature->GetMovementRateFactor() *
                                         fFactor);
    }
    return 0;
}
// PC 0x0052E310
int CSWSEffectListHandler::OnApplyMovementSpeedDecrease(CSWSObject* pObject,
                                                        CGameEffect* pEffect,
                                                        int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    float fFactor;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pCreator = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            pEffect->m_oidCreator);
        if (pCreature->m_pStats->GetEffectImmunity(IMMUNITY_TYPE_MOVEMENT_SPEED_DECREASE, pCreator) != 0 ||
            pEffect->GetInteger(0) >= 100 || pCreature->m_bPlotObject != 0)
        {
            return 1;
        }
        fFactor = 1.0f - pEffect->GetInteger(0) * 0.01f;
        pCreature->SetMovementRateFactor(pCreature->GetMovementRateFactor() *
                                         fFactor);
    }
    return 0;
}
// PC 0x0052E3C0
int CSWSEffectListHandler::OnApplyVisualEffect(CSWSObject* pObject,
                                               CGameEffect* pEffect,
                                               int bLoadingModule)
{
    if (pObject == NULL || pEffect == NULL)
    {
        return 1;
    }

    int bFireAndForget = 0;
    Vector vTargetPosition;
    unsigned long oidTarget = pObject->GetId();
    unsigned long oidCreator;
    if (pEffect->GetObjectID(0) != OBJECT_INVALID)
    {
        oidCreator = pEffect->GetObjectID(0);
    }
    else
    {
        oidCreator = pEffect->GetCreator();
    }
    unsigned long oidBeamTarget = pEffect->GetObjectID(1);
    int nVisualEffect = pEffect->GetInteger(0);
    unsigned char nBeamType = (unsigned char)pEffect->GetInteger(1);
    unsigned char nBeamMode = VISUAL_EFFECT_TARGET_BODY;
    int bMiss = pEffect->GetInteger(2);
    float fDuration = pEffect->GetDuration();
    if (bMiss == 1)
    {
        oidTarget = OBJECT_INVALID;
        vTargetPosition = pObject->CalculateSpellRangedMissTarget(
            pEffect->GetCreator(), pObject->GetId());
        nBeamMode = VISUAL_EFFECT_TARGET_GROUND;
        if (pObject->m_vPosition.m_fZ + 1.0f == vTargetPosition.m_fZ)
        {
            nBeamMode = VISUAL_EFFECT_TARGET_BODY;
        }
    }

    C2DA* pVisualEffects = g_pRules->Get2DArrays()->m_pVisualEffect;
    CExoString sType;
    CExoString sVisualEffect(nVisualEffect);
    if (pVisualEffects->GetCExoStringEntry(sVisualEffect, "Type_FD", &sType) == 1)
    {
        if (sType == "F")
            bFireAndForget = 1;
    }
    if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
            CGameEffect::DURATION_TYPE_INSTANTANEOUS ||
        bMiss == 1)
    {
        bFireAndForget = 1;
    }
    if (bFireAndForget == 1)
    {
        float fCheckDistance = CSWSArea::MAX_OBJECT_VISIBLE_DISTANCE + 2.0f;
        Vector vPosition = pObject->m_vPosition;
        if (pObject == NULL || pObject->GetArea() == NULL)
        {
            return 1;
        }
        CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
        if (CLoopingVisualEffect::GetIsBeam((unsigned short)nVisualEffect) != 0 &&
            pServerExoApp->GetGameObject(oidCreator) == NULL)
        {
            return 1;
        }
        unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
        CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
        if (pPlayer == NULL)
        {
            return 1;
        }
        CSWSObject* pPlayerObject = pPlayer->GetGameObject();
        Vector vDistance = pPlayerObject->m_vPosition - vPosition;
        if (vDistance.m_fX * vDistance.m_fX +
            vDistance.m_fY * vDistance.m_fY +
            vDistance.m_fZ * vDistance.m_fZ <=
            fCheckDistance * fCheckDistance)
        {
            CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
            pMessage->SendServerToPlayerGameObjUpdateVisEffect(pPlayer,
                    (unsigned short)nVisualEffect, oidTarget,
                    oidCreator, oidBeamTarget, nBeamType, nBeamMode,
                    vTargetPosition, fDuration);
        }
        return 1;
    }

    pObject->AddLoopingVisualEffect((unsigned short)nVisualEffect,
                                   oidCreator, nBeamType);
    return 0;
}
// PC 0x0052E740
int CSWSEffectListHandler::OnRemoveVisualEffect(CSWSObject* pObject,
                                                CGameEffect* pEffect)
{
    unsigned short nVisualEffect;

    nVisualEffect = pEffect->GetInteger(0);
    pObject->RemoveLoopingVisualEffect(nVisualEffect);
    return 1;
}
// PC 0x0052E760
int CSWSEffectListHandler::OnApplyAreaOfEffect(CSWSObject* pObject,
                                               CGameEffect* pEffect,
                                               int bLoadingModule)
{
    CSWSAreaOfEffectObject* pAreaEffect;
    CSWSArea* pArea;
    CSWSObject* pCreatureObject;

    pCreatureObject = (CSWSObject*)pObject->AsSWSCreature();
    if (pCreatureObject == NULL)
    {
        return 1;
    }

    pAreaEffect = new CSWSAreaOfEffectObject(OBJECT_INVALID);
    unsigned long oidCreator = pEffect->m_oidCreator;
    pAreaEffect->SetCreator(oidCreator);
    pAreaEffect->LoadAreaEffect((unsigned char)pEffect->GetInteger(0));
    if (pEffect->GetString(0) != "")
    {
        pAreaEffect->m_asScripts[2] = pEffect->GetString(0);
    }
    if (pEffect->GetString(1) != "")
    {
        pAreaEffect->m_asScripts[0] = pEffect->GetString(1);
    }
    if (pEffect->GetString(2) != "")
    {
        pAreaEffect->m_asScripts[3] = pEffect->GetString(2);
    }
    pAreaEffect->SetEffectSpellId(pEffect->m_nSpellId);
    pArea = pCreatureObject->GetArea();
    pAreaEffect->SetTargetObjID(pCreatureObject->m_nId);
    Vector vPosition = pCreatureObject->m_vPosition;
    pAreaEffect->AddToArea(pArea, vPosition.m_fX,
                         vPosition.m_fY, vPosition.m_fZ, 1);
    pEffect->SetObjectID(0, pAreaEffect->m_nId);
    return 0;
}
// PC 0x0052E990
int CSWSEffectListHandler::OnRemoveAreaOfEffect(CSWSObject* pObject,
                                                CGameEffect* pEffect)
{
    unsigned long oidArea;
    CGameObject* pArea;
    CServerExoApp* pApp;

    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature();
        oidArea = pEffect->GetObjectID(0);
        pApp = g_pAppManager->m_pServerExoApp;
        if (pApp->GetObjectArray()->GetGameObject(oidArea, &pArea) ==
                CGameObjectArray::SUCCESS &&
            pArea != NULL)
        {
            pApp = g_pAppManager->m_pServerExoApp;
            pApp->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, oidArea, oidArea, CServerAIMaster::EVENT_DESTROY_OBJECT,
                NULL);
        }
    }
    return 1;
}
// PC 0x0052EA10
int CSWSEffectListHandler::OnApplyBeam(CSWSObject* pObject,
                                       CGameEffect* pEffect, int bLoadingModule)
{
    CGameEffect* pBeamEffect;

    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE ||
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE ||
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        pBeamEffect = new CGameEffect(pEffect);
        pBeamEffect->m_nType = EFFECT_VISUALEFFECT;
        pBeamEffect->SetObjectID(0, pEffect->GetObjectID(0));
        pBeamEffect->SetInteger(0, pEffect->GetInteger(0));
        pBeamEffect->SetInteger(1, pEffect->GetInteger(1));
        pBeamEffect->SetInteger(2, pEffect->GetInteger(2));
        pObject->ApplyEffect(pBeamEffect, bLoadingModule, 0);
    }
    return 0;
}
// PC 0x0052EB10
int CSWSEffectListHandler::OnApplyForceResistanceIncrease(CSWSObject* pObject,
                                                          CGameEffect* pEffect,
                                                          int bLoadingModule)
{
    CSWSCreatureStats* pStats;
    int nResistance;
    char nCurrent;

    if (pObject->AsSWSCreature() != NULL)
    {
        pStats = pObject->AsSWSCreature()->m_pStats;
        nResistance = pEffect->GetInteger(0);
        if (nResistance > 128)
        {
            nResistance = 128;
        }
        else if (nResistance < 0)
        {
            return 1;
        }
        if (pObject->AsSWSCreature() != NULL)
        {
            pObject->AsSWSCreature()->m_nUnknown344 = 1;
        }
        nCurrent = pStats->GetForceResistance();
        if (nResistance > nCurrent)
        {
            pStats->SetForceResistance(nResistance);
        }
        return 0;
    }
    return 0;
}
// PC 0x0052EBA0
int CSWSEffectListHandler::OnApplyForceResistanceDecrease(CSWSObject* pObject,
                                                          CGameEffect* pEffect,
                                                          int bLoadingModule)
{
    CSWSCreatureStats* pStats;
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    unsigned long oidCreator;
    int nPenalty;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pStats = pCreature->m_pStats;
        oidCreator = pEffect->m_oidCreator;
        pCreator =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                oidCreator);
        if (pStats->GetEffectImmunity(IMMUNITY_TYPE_FORCE_RESISTANCE_DECREASE, pCreator) == 0)
        {
            nPenalty = pEffect->GetInteger(0);
            if (nPenalty >= 0 && pObject->m_bPlotObject == 0)
            {
                if (nPenalty > 128)
                {
                    nPenalty = 128;
                }
                if (pObject->AsSWSCreature() != NULL)
                {
                    pObject->AsSWSCreature()->m_nUnknown344 = 1;
                }
                if (pStats->m_nForceResistancePenalty <= nPenalty)
                {
                    pStats->SetForceResistancePenalty(nPenalty);
                }
                return 0;
            }
        }
        return 1;
    }
    return 0;
}
// PC 0x0052EC50
int CSWSEffectListHandler::OnApplyRegenerate(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    CServerExoApp* pApp;
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;

    if (pObject->AsSWSCreature() != NULL)
    {
        pApp = g_pAppManager->m_pServerExoApp;
        pApp->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
        pEffect->SetInteger(2, nCalendarDay);
        pEffect->SetInteger(3, nTimeOfDay);
    }
    return 0;
}

inline unsigned short CGameEffect::GetSubType() { return m_nSubType; }
// PC 0x00532FD0
int CSWSEffectListHandler::OnApplyForceShield(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    if (pObject != NULL && pObject->AsSWSCreature() != NULL &&
        pEffect != NULL)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        unsigned short nAppearance = pCreature->m_cAppearanceInfo.m_nAppearance;
        if (pCreature->GetClientCreature() != NULL &&
            pCreature->GetClientCreature()->GetForceShield())
        {
            CGameEffect* pShieldEffect = NULL;
            int nEffects = pCreature->m_lstEffects.num;
            for (int i = 0; i < nEffects; i++)
            {
                if (pCreature->m_lstEffects[i]->GetEffectType() ==
                    EFFECT_FORCESHIELD)
                {
                    pShieldEffect = pCreature->m_lstEffects[i];
                    break;
                }
                if (pCreature->m_lstEffects[i]->GetEffectType() >
                    EFFECT_FORCESHIELD)
                    break;
            }
            unsigned __int64 nEffectId = pShieldEffect->m_nID;
            pCreature->RemoveEffectById(nEffectId);
        }

        CExoString sRow(pEffect->GetInteger(0));
#ifdef _XBOX
        CTwoDimArrays* pArraysForForceShields = g_pRules->Get2DArrays();
        EXOXBOXASSERT(pArraysForForceShields->m_pForceShields);
        C2DA* pForceShields = pArraysForForceShields->m_pForceShields;
#else
        C2DA* pForceShields = g_pRules->Get2DArrays()->m_pForceShields;
#endif
        int nShieldAppearance = 0;
        int nVisualEffect = 0;
        CExoString sVisualEffectColumn("VisualEffectDef");
        if (pForceShields->GetINTEntry(sRow, "Appearance_01", &nShieldAppearance) &&
            nAppearance == nShieldAppearance)
            sVisualEffectColumn = "VisualEffect_01";
        else if (pForceShields->GetINTEntry(sRow, "Appearance_02", &nShieldAppearance) &&
                 nAppearance == nShieldAppearance)
            sVisualEffectColumn = "VisualEffect_02";
        else if (pForceShields->GetINTEntry(sRow, "Appearance_03", &nShieldAppearance) &&
                 nAppearance == nShieldAppearance)
            sVisualEffectColumn = "VisualEffect_03";
        else if (pForceShields->GetINTEntry(sRow, "Appearance_04", &nShieldAppearance) &&
                 nAppearance == nShieldAppearance)
            sVisualEffectColumn = "VisualEffect_04";
        pForceShields->GetINTEntry(sRow, sVisualEffectColumn, &nVisualEffect);

        int nDamageFlags = 0;
        int nVulnerFlags = 0;
        int nResistance = 0;
        int nAmount = 0;
        int nPermanent = 0;
        pForceShields->GetINTEntry(sRow, "DamageFlags", &nDamageFlags);
        pForceShields->GetINTEntry(sRow, "VulnerFlags", &nVulnerFlags);
        pForceShields->GetINTEntry(sRow, "Resistance", &nResistance);
        pForceShields->GetINTEntry(sRow, "Amount", &nAmount);
        pForceShields->GetINTEntry(sRow, "Permanent", &nPermanent);

        CGameEffect* pVisualEffect = new CGameEffect(pEffect);
        pVisualEffect->SetCreator(pObject->GetId());
        pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
        pVisualEffect->SetInteger(0, nVisualEffect);
        CGameEffect* pResistanceEffect = new CGameEffect(pEffect);
        pResistanceEffect->SetCreator(pObject->GetId());
        pResistanceEffect->m_nType = EFFECT_DAMAGE_RESISTANCE;
        pResistanceEffect->m_nSubType =
            (pResistanceEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        pResistanceEffect->SetInteger(0, nDamageFlags);
        pResistanceEffect->SetInteger(1, nResistance);
        pResistanceEffect->SetInteger(2, nAmount);
        pResistanceEffect->SetInteger(3, nVulnerFlags);
        pObject->ApplyEffect(pVisualEffect, 0, 0);
        pObject->ApplyEffect(pResistanceEffect, 0, 0);
    }
    return 0;
}
// PC 0x005335F0
int CSWSEffectListHandler::OnApplyDamage(CSWSObject* pObject,
                                         CGameEffect* pEffect,
                                         int bLoadingModule)
{
    CExoString sDebug;
    int bDeadTemp = 0;
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
        bDeadTemp = pObject->AsSWSCreature()->GetDeadTemp();

    if (!pObject->GetDead() && !bDeadTemp)
    {
        int nDamage = 0;
        int nDamageAmounts[APPLIED_DAMAGE_TYPE_COUNT];
        int i;
        for (i = 0; i < APPLIED_DAMAGE_TYPE_COUNT; ++i)
        {
            nDamageAmounts[i] = pEffect->GetInteger(i);
            if ((pObject->m_bPlotObject || g_bDisableDamage) &&
                nDamageAmounts[i] > -1)
                nDamageAmounts[i] = 0;
            if (i == CSWSCombatAttackData::BASE_DAMAGE_INDEX)
                nDamage = nDamageAmounts[i];
        }

        if (nDamage == 0 && !pObject->m_bPlotObject)
            return 1;
        {
            pEffect->GetInteger(APPLIED_DAMAGE_FROM_ATTACK);
            int nAnimationDelay = pEffect->GetInteger(APPLIED_DAMAGE_ANIMATION_DELAY);
            int nDamageFlags = pEffect->GetInteger(APPLIED_DAMAGE_FLAGS);
            pEffect->GetInteger(APPLIED_DAMAGE_POWER);
            int bMelee = pEffect->GetInteger(APPLIED_DAMAGE_MELEE);
            int bRanged = pEffect->GetInteger(APPLIED_DAMAGE_RANGED);
            int bCombat = 0;
            int nNumIntegers = pEffect->m_nNumIntegers;
            if (nNumIntegers > APPLIED_DAMAGE_COMBAT)
                bCombat = pEffect->GetInteger(APPLIED_DAMAGE_COMBAT);
            if (g_bEnableCombatDebugging)
                sDebug = pEffect->GetString(0);

            unsigned long oidCreator = pEffect->GetCreator();
            CSWSCreature* pCreator;
            if (pObject->AsSWSCreature() != NULL)
            {
                CSWSCreature* pCreature = pObject->AsSWSCreature();
                CGameObject* pCreatorObject =
                    g_pAppManager->GetServerExoApp()->GetGameObject(oidCreator);
                pCreator = NULL;
                if (pCreatorObject != NULL)
                    pCreator = pCreatorObject->AsSWSCreature();
                int bWasDying = pCreature->GetIsPCDying();

                if (!bMelee)
                {
                    int nDamagePower = (nDamageFlags & DAMAGE_FLAG_PHYSICAL) ? DAMAGE_POWER_NORMAL : DAMAGE_POWER_ENERGY;
                    int nImmuneDamage = pCreature->DoDamageImmunity(
                        pCreator, nDamage, nDamageFlags, 0, 0);
                    nDamage = pCreature->DoDamageResistance(
                        pCreator, nImmuneDamage, nDamageFlags, 0, 0, 0);
                    if (nDamagePower != DAMAGE_POWER_ENERGY)
                        nDamage = pCreature->DoDamageReduction(
                            pCreator, nDamage, nDamagePower, 0, 0);
                    for (i = 0; i < APPLIED_DAMAGE_TYPE_COUNT; ++i)
                    {
                        if (nDamageAmounts[i] > -1)
                            nDamageAmounts[i] = nDamage;
                    }
                }

                pCreature->m_oidLastDamager = oidCreator;
                int bSetHostileActor = 1;
                if (pCreature->m_nPoisonState)
                {
                    for (i = 0; i < pCreature->m_lstEffects.num; ++i)
                    {
                        CGameEffect* pExistingEffect = pCreature->m_lstEffects.element[i];
                        if (pExistingEffect->m_nType == EFFECT_POISON)
                        {
                            unsigned __int64 nExistingID = pExistingEffect->m_nID;
                            unsigned __int64 nEffectID = pEffect->m_nID;
                            if (nExistingID == nEffectID)
                            {
                                bSetHostileActor = 0;
                                break;
                            }
                        }
                    }
                }
                if (bSetHostileActor)
                    pCreature->SetLastHostileActor(oidCreator, 0);

                for (i = 0; i < APPLIED_DAMAGE_TYPE_COUNT; ++i)
                    pCreature->SetLastDamageAmount(i, nDamageAmounts[i]);
                if (!bRanged && bMelee)
                    pCreature->ResolveDamageShields(pCreator);

                if (nDamage > 0)
                {
                    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                    if (pClient->GetPlayerCreatureId() ==
                        pClient->ServerToClientObjectId(oidCreator))
                    {
                        pClient->AddFloatyText(
                            pClient->ServerToClientObjectId(pCreature->GetId()), 0, nDamage);
                        pCreature->DoDamage(nDamage, 1);
                    }
                    else
                        pCreature->DoDamage(nDamage, 0);

                    if (pCreature->m_nAnimation == CREATURE_ANIMATION_CONJURE_1 ||
                        pCreature->m_nAnimation == CREATURE_ANIMATION_CONJURE_2 ||
                        pCreature->m_nAnimation == CREATURE_ANIMATION_CONJURE_3)
                    {
                        int nSpellId = pCreature->m_nLastSpellId;
                        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
                        if (pSpell != NULL && pSpell->m_nUnknown134 == 1)
                        {
                            int nRoll = g_pRules->RollDice(1, 20);
                            pCreature->GetNearestEnemy(4.0f, OBJECT_INVALID, OBJECT_INVALID);
                            int nSpellLevel;
                            if (pCreature->m_c1D4 != CSWSObject::CASTING_CLASS_INNATE_FF)
                            {
                                CSWSCreatureStats* pStats = pCreature->m_pStats;
                                unsigned char nCastingClass = pCreature->m_c1D4;
                                nSpellLevel = pSpell->GetSpellLevel(
                                    pStats->GetClass(nCastingClass));
                            }
                            else
                                nSpellLevel = pSpell->m_nInnateLevel;
                            int nDC = 10 + nDamage + nSpellLevel;
                            if (nDC < 1)
                                nDC = 1;
                            if (nRoll < nDC)
                            {
                                pCreature->DeleteCurrentAIAction();
                                pCreature->SetAnimation(CREATURE_ANIMATION_DAMAGE);
                                unsigned char nCastingClass = pCreature->m_c1D4;
                                pCreature->DecrementSpellReadyCount(
                                    pCreature->m_nLastSpellId, nCastingClass, 0, 0, 255);
                                CGameEffect* pVisualEffect = new CGameEffect(1);
                                pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
                                pVisualEffect->m_nSubType =
                                    (pVisualEffect->GetSubType() & ~CGameEffect::DURATION_MASK) |
                                    CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                                pVisualEffect->SetCreator(pCreature->GetId());
                                pCreature->ApplyEffect(pVisualEffect, bLoadingModule, 0);
                            }
                        }
                    }

                    if (!bMelee && !pCreature->IsDebilitated())
                    {
                        if (nDamageAmounts[CSWSCombatAttackData::DAMAGE_INDEX_ELECTRICAL] > 0 ||
                            nDamageAmounts[CSWSCombatAttackData::DAMAGE_INDEX_DARK_SIDE] > 0)
                        {
                            if (pCreature->m_nAnimation == CREATURE_ANIMATION_COMBAT_IDLE ||
                                pCreature->m_nAnimation == CREATURE_ANIMATION_COMBAT_READY)
                            {
                                if (!pCreature->m_pcCombatRound->m_bRoundPaused)
                                {
                                    pCreature->m_pcCombatRound->SetRoundPaused(1, oidCreator);
                                    pCreature->m_pcCombatRound->SetPauseTimer(nAnimationDelay, 0);
                                    pCreature->m_pcCombatRound->DecrementRoundLength(nAnimationDelay, 0);
                                }
                                pCreature->SetAnimation(CREATURE_ANIMATION_ELECTRICAL_DARK_SIDE_DAMAGE);
                            }
                        }
                        else
                            pCreature->SetAnimation(CREATURE_ANIMATION_NONMELEE_DAMAGE);
                    }
                }

                g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(pCreature->GetId());
                if (!pCreature->m_nAreaWaitState && !pCreature->GetDead() &&
                    !pCreature->GetDeadTemp())
                {
                    if (!g_pAppManager->GetServerExoApp()->GetPartyTable()->GetIsLeader(pCreature->GetId()))
                    {
                        unsigned long oidScriptObject = pCreature->GetId();
                        CExoString* pScript = &pCreature->m_sScripts[CSWSCreature::SCRIPT_DAMAGED];
                        g_pVirtualMachine->RunScript(pScript, oidScriptObject, 1);
                    }
                }

                if (!bCombat)
                {
                    if (pCreatorObject != NULL)
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetObjectID(0, oidCreator);
                        pData->SetObjectID(1, pCreature->GetId());
                        for (i = 0; i < APPLIED_DAMAGE_TYPE_COUNT; ++i)
                            pData->SetInteger(i, nDamageAmounts[i]);
                        if (g_bEnableCombatDebugging)
                            pData->SetString(0, sDebug);
                        pCreature->BroadcastDamageDataToParty(pData, 0);
                        if (pCreator != NULL &&
                            pCreature->GetFactionId() != pCreator->GetFactionId())
                            pCreator->BroadcastDamageDataToParty(pData, 0);
                        delete pData;
                    }
                    else
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetObjectID(0, pCreature->GetId());
                        for (i = 0; i < APPLIED_DAMAGE_TYPE_COUNT; ++i)
                            pData->SetInteger(i, nDamageAmounts[i]);
                        pCreature->BroadcastDamageDataToParty(pData, 1);
                        delete pData;
                    }
                }

                if (!bWasDying && pCreature->GetIsPCDying())
                {
                    if (pCreature->m_nAmbientAnimationState != CSWSCreature::AMBIENT_STATE_3 &&
                        pCreature->m_nAmbientAnimationState != CSWSCreature::AMBIENT_STATE_4 &&
                        pCreature->m_nAmbientAnimationState != CSWSCreature::AMBIENT_STATE_14)
                    {
                        if (rand() % 2)
                        {
                            pCreature->SetAmbientAnimationState(CSWSCreature::AMBIENT_STATE_3);
                            pCreature->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
                        }
                        else
                        {
                            pCreature->SetAmbientAnimationState(CSWSCreature::AMBIENT_STATE_4);
                            pCreature->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
                        }
                    }
                    pCreature->m_bCommandable = 0;
                    unsigned long oidModule = g_pAppManager->GetServerExoApp()->GetModule()->GetId();
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_PLAYER_DYING;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, pCreature->GetId(), oidModule,
                        CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                    pCreature->AutoCloseGUIPanels(0);
                }
                else if (pCreature->GetDead() || pCreature->GetDeadTemp())
                {
                    CGameEffect* pDeathEffect = new CGameEffect(1);
                    pDeathEffect->m_nType = EFFECT_DEATH;
                    pDeathEffect->m_nSubType =
                        (pDeathEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                    pDeathEffect->SetCreator(oidCreator);
                    if (pCreator != NULL &&
                        !pCreator->m_pcCombatRound->GetAttack(
                            pCreator->m_pcCombatRound->m_nCurrentAttack)->m_bRangedAttack)
                        pDeathEffect->SetInteger(0, 1);
                    pDeathEffect->SetInteger(1, 1);
                    pCreature->ApplyEffect(pDeathEffect, bLoadingModule, 0);
                }
            }
            else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE ||
                     pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
            {
                pCreator =
                    g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreator);
                if (!bMelee)
                {
                    int nDamagePower = (nDamageFlags & DAMAGE_FLAG_PHYSICAL) ? DAMAGE_POWER_NORMAL : DAMAGE_POWER_ENERGY;
                    int nImmuneDamage = pObject->DoDamageImmunity(pCreator, nDamage, nDamageFlags, 0, 0);
                    nDamage = pObject->DoDamageResistance(pCreator, nImmuneDamage, nDamageFlags, 0, 0, 0);
                    if (nDamagePower != DAMAGE_POWER_ENERGY)
                        nDamage = pObject->DoDamageReduction(pCreator, nDamage, nDamagePower, 0, 0);
                    if (pObject->m_nAnimation == DOOR_ANIMATION_CLOSED ||
                        pObject->m_nAnimation == PLACEABLE_ANIMATION_CLOSED)
                    {
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, pEffect->GetCreator(), pObject->GetId(), CServerAIMaster::EVENT_PLAY_ANIMATION,
                            (void*)pObject->m_nAnimation);
                        pObject->SetAnimation(CREATURE_ANIMATION_DAMAGE);
                    }
                }
                CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                if (pClient->GetPlayerCreatureId() ==
                    pClient->ServerToClientObjectId(oidCreator))
                    pClient->AddFloatyText(pClient->ServerToClientObjectId(pObject->GetId()), 0, nDamage);
                pObject->DoDamage(nDamage);
                if (FALSE)
                {
                    CExoString sMessage;
                    sMessage = CExoString(nDamage);
                    sMessage = CExoString(oidCreator);
                }

                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DAMAGED;
                pEvent->SetInteger(0, nDamage);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, pEffect->GetCreator(), pObject->GetId(),
                    CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, oidCreator);
                pData->SetObjectID(1, pObject->GetId());
                for (i = 0; i < APPLIED_DAMAGE_TYPE_COUNT; ++i)
                    pData->SetInteger(i, nDamageAmounts[i]);
                if (g_bEnableCombatDebugging)
                    pData->SetString(0, sDebug);
                if (pCreator != NULL)
                {
                    pCreator->BroadcastCombatStateToParty(OBJECT_INVALID);
                    pCreator->BroadcastDamageDataToParty(pData, 0);
                }
                delete pData;

                if (pObject->GetDead() == 1)
                {
                    if (pObject->AsSWSDoor() != NULL)
                    {
                        pObject->m_bPlotObject = 1;
                        pObject->AsSWSDoor()->m_bLocked = 0;
                        pObject->AsSWSDoor()->OpenDoor(oidCreator);
                    }
                    else if (pObject->AsSWSPlaceable() != NULL)
                    {
                        if (pObject->AsSWSPlaceable()->m_bHasInventory)
                        {
                            pObject->m_bPlotObject = 1;
                            pObject->AsSWSPlaceable()->m_bLocked = 0;
                            unsigned long* poidOpener = new unsigned long;
                            if (poidOpener != NULL)
                            {
                                *poidOpener = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                                    0, 0, *poidOpener, pObject->GetId(), CServerAIMaster::EVENT_PLAY_ANIMATION, (void*)PLACEABLE_ANIMATION_OPEN);
                                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                                    0, 1000, *poidOpener, pObject->GetId(), CServerAIMaster::EVENT_OPEN_OBJECT, poidOpener);
                            }
                            else
                                pObject->AsSWSPlaceable()->OpenInventory(
                                    g_pAppManager->GetServerExoApp()->GetPlayerCreatureId(), 1);
                        }
                        else
                        {
                            CGameEffect* pDeathEffect = new CGameEffect(1);
                            pDeathEffect->m_nType = EFFECT_DEATH;
                            pDeathEffect->m_nSubType =
                                (pDeathEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                                CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                            pDeathEffect->SetCreator(oidCreator);
                            pDeathEffect->SetInteger(0, 0);
                            pDeathEffect->SetInteger(1, 1);
                            pObject->ApplyEffect(pDeathEffect, bLoadingModule, 0);
                        }
                    }
                    if (pCreator != NULL)
                        pCreator->ClearAllActions(1);
                }
            }
        }
    }
    return 1;
}
// PC 0x0052ECB0
int CSWSEffectListHandler::OnApplyPoison(CSWSObject* pObject,
                                         CGameEffect* pEffect, int bLoadingModule)
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    int nDuration;
    int nDcSave;
    int nPeriod;
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        if (pCreature->m_bPlotObject != 0)
        {
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime( 0, 0, pEffect->GetCreator(), pCreature->GetId(), CServerAIMaster::EVENT_REMOVE_EFFECT, pEffect);
        }
        else if (pCreature->GetDead() != 0 || pCreature->GetDeadTemp() != 0 || pCreature->GetPoisoned() == 1)
        {
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, pEffect->GetCreator(), pCreature->GetId(),
                CServerAIMaster::EVENT_REMOVE_EFFECT, pEffect);
        }
        else
        {
            if (!bLoadingModule)
            {
                int nName;
                CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()
                    ->GetCreatureByGameObjectID(pEffect->GetCreator());
                if (pCreature->m_pStats->GetEffectImmunity(
                        CSWSCreatureStats::IMMUNITY_TYPE_POISON, pCreator) != 0)
                {
                    CSWCCMessageData* pMessageData = new CSWCCMessageData;
                    pMessageData->SetObjectID(0, pObject->GetId());
                    pMessageData->SetInteger(0, 0);
                    pMessageData->SetInteger(1, 0);
                    pMessageData->SetInteger(2, DAMAGE_FLAG_POISON);
                    if (pCreator != NULL)
                    {
                        CSWCCMessageData* pCreatorMessageData = new CSWCCMessageData;
                        pMessageData->CopyTo(pCreatorMessageData);
                        pCreator->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY, pCreatorMessageData);
                    }
                    pCreature->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY, pMessageData);
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, pEffect->GetCreator(), pCreature->GetId(),
                        CServerAIMaster::EVENT_REMOVE_EFFECT, pEffect);
                    return 0;
                }
                else
                {
                    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
                        &nCalendarDay, &nTimeOfDay);
                    pEffect->SetInteger(1, nCalendarDay);
                    pEffect->SetInteger(2, nTimeOfDay);
#ifdef _XBOX
                    C2DA* pPoison = g_pRules->Get2DArrays()->GetPoison();
#else
                    C2DA* pPoison = g_pRules->Get2DArrays()->m_pPoison;
#endif
                    if (pPoison == NULL)
                    {
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, pEffect->GetCreator(), pCreature->GetId(),
                            CServerAIMaster::EVENT_REMOVE_EFFECT, pEffect);
                        return 0;
                    }
                    int nPoison = pEffect->GetInteger(0);
                    pPoison->GetINTEntry(nPoison, "DC_SAVE", &nDcSave);
                    if (pCreature->SavingThrowRoll(CSWSCreature::SAVING_THROW_FORT,
                            nDcSave, CSWSCreature::SAVING_THROW_TYPE_POISON,
                            pEffect->GetCreator(), 1, 0, 0) != 0)
                    {
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, pEffect->GetCreator(), pCreature->GetId(),
                            CServerAIMaster::EVENT_REMOVE_EFFECT, pEffect);
                        return 0;
                    }
                    pPoison->GetINTEntry(nPoison, "Name", &nName);
                    CSWCCMessageData* pPoisonMessageData = new CSWCCMessageData;
                    pPoisonMessageData->SetInteger(0, nName);
                    pPoisonMessageData->SetObjectID(0, pCreature->GetId());
                    pCreature->BroadcastPoisoned(pPoisonMessageData);
                    delete pPoisonMessageData;
                    pCreature->BroadcastVoiceChat(28);
                    pEffect->SetFloat(0, 2.0f);
                    pCreature->ApplyPoisonDamage(nPoison, pEffect, 0, 1.0f);
                    pEffect->SetInteger(3, nCalendarDay);
                    pEffect->SetInteger(4, nTimeOfDay);
                    pPoison->GetINTEntry(nPoison, "DURATION", &nDuration);
                    pEffect->SetInteger(5, nDuration);
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pEffect->m_fDuration = nDuration * 1000.0f;
                    pPoison->GetINTEntry(nPoison, "PERIOD", &nPeriod);
                    pEffect->SetInteger(6, nPeriod);
                    pEffect->SetInteger(7, 0);
                }
            }
            CGameEffect* pVisualEffect = new CGameEffect(1);
            pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
            pVisualEffect->m_nSubType =
                (pVisualEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                CGameEffect::DURATION_TYPE_INSTANTANEOUS;
            pVisualEffect->SetInteger(0, VFX_PRO_AFFLICT);
            pVisualEffect->SetInteger(2, 0);
            pVisualEffect->SetObjectID(0, pEffect->GetCreator());
            pCreature->ApplyEffect(pVisualEffect, 0, 0);
            pCreature->SetPoisoned(1, pEffect->m_nID);
        }
    }
    return 0;
}
// PC 0x0052F210
int CSWSEffectListHandler::OnRemovePoison(CSWSObject* pObject,
                                          CGameEffect* pEffect)
{
    CSWSCreature* pCreature;

    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pCreature->m_nPoisonState == 1 &&
            pCreature->m_nPoisonEffectID == pEffect->m_nID)
        {
            pCreature->m_nPoisonState = 0;
            pCreature->m_nPoisonEffectID = -1;
        }
    }
    return 1;
}
// PC 0x0052F270
int CSWSEffectListHandler::OnRemoveDisease(CSWSObject* pObject,
                                           CGameEffect* pEffect)
{
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_bDiseased = 0;
    }
    return 1;
}
// PC 0x0052F2A0
int CSWSEffectListHandler::OnApplyLink(CSWSObject* pObject,
                                       CGameEffect* pEffect, int bLoadingModule)
{
    CGameEffect* pLeftEffect;
    CGameEffect* pRightEffect;

    pEffect->UpdateLinked();
    pLeftEffect = pEffect->m_pLinkLeft;
    if (pLeftEffect != NULL)
    {
        pObject->ApplyEffect(pLeftEffect, bLoadingModule, 0);
    }
    pRightEffect = pEffect->m_pLinkRight;
    if (pRightEffect != NULL)
    {
        pObject->ApplyEffect(pRightEffect, bLoadingModule, 0);
    }
    pEffect->SetLinked(NULL, NULL);
    return 1;
}
// PC 0x0052F2F0
int CSWSEffectListHandler::OnApplyHasteInternal(CSWSObject* pObject,
                                                CGameEffect* pEffect,
                                                int bLoadingModule)
{
    CSWSCreature* pCreature;
    CGameEffect* pMovementEffect;
    CGameEffect* pAcEffect;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pMovementEffect = new CGameEffect(pEffect);
        pMovementEffect->m_nType = EFFECT_MOVEMENT_SPEED_INCREASE;
        pMovementEffect->SetInteger(0, 150);
        pObject->ApplyEffect(pMovementEffect, bLoadingModule, 0);
        pAcEffect = new CGameEffect(pEffect);
        pAcEffect->SetInteger(0, 0);
        pAcEffect->SetInteger(1, 4);
        pAcEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
        pAcEffect->SetInteger(5, DAMAGE_FLAG_PHYSICAL);
        pAcEffect->m_nType = EFFECT_AC_INCREASE;
        pObject->ApplyEffect(pAcEffect, bLoadingModule, 0);
        pCreature->m_bSlowed = 0;
        pCreature->m_bHasted = 1;
    }
    return 0;
}
// PC 0x0052F430
int CSWSEffectListHandler::OnRemoveHasteInternal(CSWSObject* pObject,
                                                 CGameEffect* pEffect)
{
    CSWSCreature* pCreature;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->m_bHasted = 0;
    }
    return 1;
}
// PC 0x0052F450
int CSWSEffectListHandler::OnApplySlowInternal(CSWSObject* pObject,
                                               CGameEffect* pEffect,
                                               int bLoadingModule)
{
    CSWSCreature* pCreature;
    CGameEffect* pMovementEffect;
    CGameEffect* pAcEffect;
    CGameEffect* pAttackEffect;
    CGameEffect* pSaveEffect;
    CGameEffect* pNumAttacksEffect;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pMovementEffect = new CGameEffect(pEffect);
        pMovementEffect->m_nType = EFFECT_MOVEMENT_SPEED_DECREASE;
        pMovementEffect->SetInteger(0, 50);
        pObject->ApplyEffect(pMovementEffect, bLoadingModule, 0);
        pAcEffect = new CGameEffect(pEffect);
        pAcEffect->SetInteger(0, 0);
        pAcEffect->SetInteger(1, 2);
        pAcEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
        pAcEffect->SetInteger(5, DAMAGE_FLAG_PHYSICAL);
        pAcEffect->m_nType = EFFECT_AC_DECREASE;
        pObject->ApplyEffect(pAcEffect, bLoadingModule, 0);
        pAttackEffect = new CGameEffect(pEffect);
        pAttackEffect->m_nType = EFFECT_ATTACK_DECREASE;
        pAttackEffect->SetInteger(0, 2);
        pAttackEffect->SetInteger(1, 0);
        pAttackEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
        pObject->ApplyEffect(pAttackEffect, bLoadingModule, 0);
        pSaveEffect = new CGameEffect(pEffect);
        pSaveEffect->SetInteger(0, 2);
        pSaveEffect->SetInteger(1, 2);
        pSaveEffect->SetInteger(3, (unsigned char)g_pRules->m_nNumRaces);
        pSaveEffect->m_nType = EFFECT_SAVING_THROW_DECREASE;
        pObject->ApplyEffect(pSaveEffect, bLoadingModule, 0);
        pNumAttacksEffect = new CGameEffect(pEffect);
        pNumAttacksEffect->m_nType = EFFECT_LIMIT_MOVEMENT_SPEED;
        pNumAttacksEffect->SetInteger(0, 0);
        pObject->ApplyEffect(pNumAttacksEffect, bLoadingModule, 0);
        pCreature->m_bHasted = 0;
        pCreature->m_bSlowed = 1;
    }
    return 0;
}
// PC 0x0052F6C0
int CSWSEffectListHandler::OnRemoveSlowInternal(CSWSObject* pObject,
                                                CGameEffect* pEffect)
{
    CSWSCreature* pCreature;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->m_bSlowed = 0;
    }
    return 1;
}
// PC 0x0052F6E0
int CSWSEffectListHandler::OnApplyLight(CSWSObject* pObject,
                                        CGameEffect* pEffect,
                                        int bLoadingModule)
{
    CGameEffect* pLightEffect;

    if (pObject->AsSWSCreature() != NULL)
    {
        pLightEffect = new CGameEffect(pEffect);
        pLightEffect->m_nType = EFFECT_VISUALEFFECT;
        pLightEffect->SetInteger(0, pEffect->GetInteger(0));
        pObject->AsSWSCreature()->ApplyEffect(pLightEffect,
                                                    bLoadingModule, 0);
    }
    return 0;
}
// PC 0x0052F790
int CSWSEffectListHandler::OnApplyModifyNumAttacks(CSWSObject* pObject,
                                                     CGameEffect* pEffect,
                                                     int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCombatRound* pCombatRound;
    int nCurrent;
    int nEffectAttacks;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCombatRound = pCreature->m_pcCombatRound;
        nCurrent = pCombatRound->m_nEffectAttacks;
        if (nCurrent > 2)
        {
            return 1;
        }
        nEffectAttacks = nCurrent + pEffect->GetInteger(0);
        pCombatRound->m_nEffectAttacks = nEffectAttacks;
        if (nEffectAttacks > 2)
        {
            nEffectAttacks = 2;
        }
        pCombatRound->m_nEffectAttacks = nEffectAttacks;
        pCombatRound->m_nEffectAttacks =
            (pCombatRound->m_nEffectAttacks < 0)
                ? 0
                : pCombatRound->m_nEffectAttacks;
    }
    return 0;
}
// PC 0x0052F7D0
int CSWSEffectListHandler::OnRemoveModifyNumAttacks(CSWSObject* pObject,
                                                    CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CSWSCombatRound* pCombatRound;
    int nCurrent;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
#ifdef _XBOX
        nCurrent = pCreature->m_pcCombatRound->m_nEffectAttacks;
        EXOXBOXASSERT(nCurrent - pEffect->GetInteger(0) >= 0);
#endif
        pCombatRound = pCreature->m_pcCombatRound;
        nCurrent = pCombatRound->m_nEffectAttacks;
        pCombatRound->m_nEffectAttacks = nCurrent - pEffect->GetInteger(0);
        pCombatRound->m_nEffectAttacks =
            (pCombatRound->m_nEffectAttacks > 2)
                ? 2
                : pCombatRound->m_nEffectAttacks;
        pCombatRound->m_nEffectAttacks =
            (pCombatRound->m_nEffectAttacks < 0)
                ? 0
                : pCombatRound->m_nEffectAttacks;
    }
    return 1;
}
// PC 0x0052F810
int CSWSEffectListHandler::OnApplyCurse(CSWSObject* pObject,
                                     CGameEffect* pEffect, int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    CSWSCreature* pCreator;
    CGameEffect* pAbilityEffect;

    if (pCreature != NULL)
    {
        if (pCreature->m_bPlotObject != 0)
        {
            return 1;
        }

        unsigned long oidCreator = pEffect->m_oidCreator;
        pCreator = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
            oidCreator);
        if (pCreature->m_pStats->GetEffectImmunity(
                CSWSCreatureStats::IMMUNITY_TYPE_CURSED, pCreator) != 0)
        {
            CSWCCMessageData* pMessageData = new CSWCCMessageData;
            CSWCCMessageData* pCreatorMessageData = NULL;
            pMessageData->SetObjectID(0, pObject->GetId());
            if (pCreator != NULL)
            {
                pCreatorMessageData = new CSWCCMessageData;
                pMessageData->CopyTo(pCreatorMessageData);
                pCreator->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_CURSED_IMMUNITY, pCreatorMessageData);
            }
            pObject->AsSWSCreature()->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_CURSED_IMMUNITY, pMessageData);
            return 1;
        }

        pAbilityEffect = new CGameEffect(pEffect);
        pAbilityEffect->m_nType = EFFECT_ABILITY_DECREASE;
        pAbilityEffect->SetInteger(0, CSWRules::ABILITY_STRENGTH);
        pAbilityEffect->SetInteger(1, pEffect->GetInteger(CSWRules::ABILITY_STRENGTH));
        pObject->ApplyEffect(pAbilityEffect, bLoadingModule, 0);

        pAbilityEffect = new CGameEffect(pEffect);
        pAbilityEffect->m_nType = EFFECT_ABILITY_DECREASE;
        pAbilityEffect->SetInteger(0, CSWRules::ABILITY_DEXTERITY);
        pAbilityEffect->SetInteger(1, pEffect->GetInteger(CSWRules::ABILITY_DEXTERITY));
        pObject->ApplyEffect(pAbilityEffect, bLoadingModule, 0);

        pAbilityEffect = new CGameEffect(pEffect);
        pAbilityEffect->m_nType = EFFECT_ABILITY_DECREASE;
        pAbilityEffect->SetInteger(0, CSWRules::ABILITY_CONSTITUTION);
        pAbilityEffect->SetInteger(1, pEffect->GetInteger(CSWRules::ABILITY_CONSTITUTION));
        unsigned long oidTarget = pObject->GetId();
        oidCreator = pCreator->GetId();
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidCreator, oidTarget,
            CServerAIMaster::EVENT_APPLY_EFFECT, pAbilityEffect);

        pAbilityEffect = new CGameEffect(pEffect);
        pAbilityEffect->m_nType = EFFECT_ABILITY_DECREASE;
        pAbilityEffect->SetInteger(0, CSWRules::ABILITY_INTELLIGENCE);
        pAbilityEffect->SetInteger(1, pEffect->GetInteger(CSWRules::ABILITY_INTELLIGENCE));
        pObject->ApplyEffect(pAbilityEffect, bLoadingModule, 0);

        pAbilityEffect = new CGameEffect(pEffect);
        pAbilityEffect->m_nType = EFFECT_ABILITY_DECREASE;
        pAbilityEffect->SetInteger(0, CSWRules::ABILITY_WISDOM);
        pAbilityEffect->SetInteger(1, pEffect->GetInteger(CSWRules::ABILITY_WISDOM));
        pObject->ApplyEffect(pAbilityEffect, bLoadingModule, 0);

        pAbilityEffect = new CGameEffect(pEffect);
        pAbilityEffect->m_nType = EFFECT_ABILITY_DECREASE;
        pAbilityEffect->SetInteger(0, CSWRules::ABILITY_CHARISMA);
        pAbilityEffect->SetInteger(1, pEffect->GetInteger(CSWRules::ABILITY_CHARISMA));
        pObject->ApplyEffect(pAbilityEffect, bLoadingModule, 0);
    }
    return 0;
}
// PC 0x0052FBB0
int CSWSEffectListHandler::OnApplySilence(CSWSObject* pObject,
                                         CGameEffect* pEffect,
                                         int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    CSWCCMessageData* pMessageData;
    CSWCCMessageData* pCreatorMessageData;
    CGameEffect* pAIStateEffect;
    unsigned long oidCreator;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        oidCreator = pEffect->m_oidCreator;
        pCreator =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                oidCreator);
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        int bImmune = pStats->GetEffectImmunity(IMMUNITY_TYPE_SILENCE,
                                               pCreator);
        if (bImmune != 0)
        {
            pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, pObject->GetId());
            if (pCreator != NULL)
            {
                pCreatorMessageData = new CSWCCMessageData;
                pMessageData->CopyTo(pCreatorMessageData);
                pCreator->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_SILENCE_IMMUNITY,
                    pCreatorMessageData);
            }
            pObject->AsSWSCreature()->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_SILENCE_IMMUNITY, pMessageData);
            return 1;
        }
        if (pCreature->m_bPlotObject != 0)
            return 1;

        pAIStateEffect = new CGameEffect(pEffect);
        pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
        pAIStateEffect->SetInteger(0, SILENCE_AI_STATE_MASK);
        pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
        pCreature->m_bSilenced = 1;
    }
    return 0;
}
// PC 0x0052FD30
int CSWSEffectListHandler::OnRemoveSilence(CSWSObject* pObject,
                                           CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_bSilenced = 0;
    }
    return 1;
}
// PC 0x0052FD60
int CSWSEffectListHandler::OnApplyTaunt(CSWSObject* pObject,
                                       CGameEffect* pEffect,
                                       int bLoadingModule)
{
    CSWSCreature* pCreature;
    CGameEffect* pACEffect;
    CGameEffect* pSpellFailure;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        int bPlot = pCreature->m_bPlotObject;
        if (bPlot == 0)
        {
            pCreature->m_bTaunted = 1;

            pACEffect = new CGameEffect(pEffect);
            pACEffect->m_nType = EFFECT_AC_DECREASE;
            pACEffect->SetInteger(AC_INTEGER_TYPE, AC_DODGE_BONUS);
            pACEffect->SetInteger(AC_INTEGER_BONUS, pEffect->GetInteger(0));
            pACEffect->SetInteger(AC_INTEGER_RACIAL_GROUP,
                                  g_pRules->m_nNumRaces);
            pACEffect->SetInteger(AC_INTEGER_DAMAGE_FLAGS,
                                  DAMAGE_FLAG_PHYSICAL);
            pObject->ApplyEffect(pACEffect, bLoadingModule, 0);

            pSpellFailure = new CGameEffect(pEffect);
            pSpellFailure->m_nType = EFFECT_ARCANE_SPELL_FAILURE;
            pSpellFailure->SetInteger(0, TAUNT_ARCANE_SPELL_FAILURE);
            pObject->ApplyEffect(pSpellFailure, bLoadingModule, 0);
            return 0;
        }
    }
    return 1;
}
// PC 0x0052FEB0
int CSWSEffectListHandler::OnRemoveTaunt(CSWSObject* pObject,
                                         CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_bTaunted = 0;
    }
    return 1;
}
// PC 0x0052FEE0
int CSWSEffectListHandler::OnApplyLimitMovementSpeed(CSWSObject* pObject,
                                                     CGameEffect* pEffect,
                                                     int bLoadingModule)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        if (pObject->AsSWSCreature()->m_bPlotObject != 0)
        {
            return 1;
        }
        pObject->AsSWSCreature()->m_nUnknown8E8 = 1;
    }
    return 0;
}
// PC 0x0052FF30
int CSWSEffectListHandler::OnApplySkillIncrease(CSWSObject* pObject,
                                                CGameEffect* pEffect,
                                                int bLoadingModule)
{
    int nSkill;
    int nValue;
    int nUnknown2;
    int nUnknown3;

    nSkill = pEffect->GetInteger(0);
    nValue = pEffect->GetInteger(1);
    nUnknown2 = pEffect->GetInteger(2);
    nUnknown3 = pEffect->GetInteger(3);
    if (nValue < 0)
    {
        return 1;
    }
    return pObject->AsSWSCreature() == NULL;
}
// PC 0x0052FF80
int CSWSEffectListHandler::OnApplySkillDecrease(CSWSObject* pObject,
                                                CGameEffect* pEffect,
                                                int bLoadingModule)
{
    int nSkill;
    int nValue;
    int nUnknown2;
    int nUnknown3;
    CSWSCreature* pCreator;
    CSWSCreatureStats* pStats;
    unsigned long oidCreator;

    nSkill = pEffect->GetInteger(0);
    nValue = pEffect->GetInteger(1);
    nUnknown2 = pEffect->GetInteger(2);
    nUnknown3 = pEffect->GetInteger(3);
    if (nValue < 0 || pObject->m_bPlotObject != 0 ||
        pObject->AsSWSCreature() == NULL)
    {
        return 1;
    }
    oidCreator = pEffect->m_oidCreator;
    pCreator =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidCreator);
    pStats = pObject->AsSWSCreature()->m_pStats;
    return pStats->GetEffectImmunity(IMMUNITY_TYPE_SKILL_DECREASE, pCreator) != 0;
}
// PC 0x00530010
int CSWSEffectListHandler::OnApplyHitPointChangeWhenDying(CSWSObject* pObject,
                                                        CGameEffect* pEffect,
                                                        int bLoadingModule)
{
    CGameEffect* pDyingEffect;

    if (pObject->AsSWSCreature() == NULL ||
        pObject->AsSWSCreature()->m_pStats->GetIsPC() == 0)
    {
        return 1;
    }

    if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
            CGameEffect::DURATION_TYPE_INSTANTANEOUS &&
        pEffect->GetFloat(0) >= 0.1f)
    {
        pDyingEffect = new CGameEffect(1);
        pDyingEffect->SetCreator(pEffect->m_oidCreator);
        pDyingEffect->m_nType = EFFECT_HITPOINTCHANGEWHENDYING;
        pDyingEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_TEMPORARY |
            (pDyingEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pDyingEffect->SetFloat(0, pEffect->GetFloat(0));
        pDyingEffect->m_fDuration = fabs(6.0f / pEffect->GetFloat(0));
        pObject->ApplyEffect(pDyingEffect, bLoadingModule, 0);
    }
    return 0;
}
// PC 0x00530150
int CSWSEffectListHandler::OnRemoveHitPointChangeWhenDying(CSWSObject* pObject,
                                                         CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pNewEffect;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        if (pCreature->GetStats()->GetIsPC() != 0)
        {
            if (pEffect->GetFloat(0) > 0.0f)
            {
                pCreature->SetCurrentHitPoints(pCreature->GetCurrentHitPoints(1) + 1);
            }
            else
            {
                pCreature->SetCurrentHitPoints(pCreature->GetCurrentHitPoints(1) - 1);
                CExoString sBLOODCOLR;
                {
                    CExoString sColumn("BLOODCOLR");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    C2DA* pAppearance = p2DArrays->m_pAppearance;
                    pAppearance->GetCExoStringEntry(
                        pCreature->m_cAppearanceInfo.m_nAppearance, sColumn, &sBLOODCOLR);
                }
                pNewEffect = new CGameEffect(1);
                pNewEffect->m_nType = EFFECT_VISUALEFFECT;
                pNewEffect->m_nSubType =
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS |
                    (pNewEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
                if (sBLOODCOLR == "R")
                {
                    pNewEffect->SetInteger(0, VFX_BLOOD_RED);
                }
                else if (sBLOODCOLR == "G")
                {
                    pNewEffect->SetInteger(0, VFX_BLOOD_GREEN);
                }
                else if (sBLOODCOLR == "Y")
                {
                    pNewEffect->SetInteger(0, VFX_BLOOD_YELLOW);
                }
                pCreature->ApplyEffect(pNewEffect, 0, 0);
            }
        }
    }

    if (pCreature->GetIsPCDying())
    {
        pNewEffect = new CGameEffect(1);
        pNewEffect->SetCreator(pEffect->m_oidCreator);
        pNewEffect->m_nType = EFFECT_HITPOINTCHANGEWHENDYING;
        pNewEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_TEMPORARY |
            (pNewEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pNewEffect->m_fDuration = pEffect->GetDuration();
        pNewEffect->SetFloat(0, pEffect->GetFloat(0));
        pObject->ApplyEffect(pNewEffect, 0, 0);
    }
    else if (pCreature->GetDead() || pCreature->GetDeadTemp())
    {
        pNewEffect = new CGameEffect(1);
        pNewEffect->m_nType = EFFECT_DEATH;
        pNewEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_INSTANTANEOUS |
            (pNewEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pNewEffect->SetCreator(pEffect->m_oidCreator);
        pNewEffect->SetInteger(0, 0);
        pNewEffect->SetInteger(1, 1);
        pCreature->ApplyEffect(pNewEffect, 0, 0);
    }
    return 1;
}
// PC 0x005303C0
int CSWSEffectListHandler::OnApplySpecialWalkAnimation(CSWSObject* pObject,
                                                       CGameEffect* pEffect,
                                                       int bLoadingModule)
{
    char nAnimation;

    nAnimation = pEffect->GetInteger(0);
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_nSpecialWalkAnimation = nAnimation;
        return 0;
    }
    return 1;
}
// PC 0x00530400
int CSWSEffectListHandler::OnRemoveDisguise(CSWSObject* pObject,
                                             CGameEffect* pEffect)
{
    CSWSCreature* pCreature;

    pCreature = pObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 1;
    }
    if (((CSWSObject*)pCreature)->GetDead() == 0 || pCreature->m_nAreaWaitState != 0 ||
        pCreature->m_nEC == 0)
    {
        if (pEffect->GetInteger(1) == 0)
        {
            pEffect->SetInteger(1, 1);
            pEffect->m_nSubType =
                CGameEffect::DURATION_TYPE_TEMPORARY |
                (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
            pEffect->m_fDuration = 0.0f;
            pCreature->UnDisguise(pEffect);
        }
        else
        {
            pCreature->UnDisguise(pEffect);
        }
    }
    return 1;
}
// PC 0x00530480
int CSWSEffectListHandler::OnApplyTimestop(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    float fDuration;
    unsigned long nDurationDay;
    unsigned long nDurationTime;
    unsigned long nCurrentDay;
    unsigned long nCurrentTime;
    unsigned long nExpireDay;
    unsigned long nExpireTime;
    unsigned long oidObject;
    int nIndex;

    if (!g_pAppManager->GetServerExoApp()->GetActivePauseState())
    {
        g_pAppManager->GetServerExoApp()->AddToExclusionList(pObject->m_nId,
                                                             1);
        g_pAppManager->GetServerExoApp()->TogglePauseState(1);
    }
    else if (g_pAppManager->GetServerExoApp()->GetActivePauseState() == 1)
    {
        fDuration = pEffect->m_fDuration;
        nDurationDay = g_pAppManager->GetServerExoApp()->GetWorldTimer()
                           ->GetCalendarDayFromSeconds(fDuration);
        fDuration = pEffect->m_fDuration;
        nDurationTime = g_pAppManager->GetServerExoApp()->GetWorldTimer()
                            ->GetTimeOfDayFromSeconds(fDuration);
        oidObject = pObject->m_nId;
        g_pAppManager->GetServerExoApp()->GetActiveTimer(oidObject)
            ->GetWorldTime(&nCurrentDay, &nCurrentTime);
        g_pAppManager->GetServerExoApp()->GetWorldTimer()->AddWorldTimes(
            nCurrentDay, nCurrentTime, nDurationDay, nDurationTime,
            &nExpireDay, &nExpireTime);
        for (nIndex = 0; nIndex < pObject->m_lstEffects.num; nIndex++)
        {
            if (pObject->m_lstEffects.element[nIndex]->m_nType ==
                EFFECT_TIMESTOP)
            {
                pObject->m_lstEffects.element[nIndex]->SetExpiryTime(
                    nExpireDay, nExpireTime);
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x005305D0
int CSWSEffectListHandler::OnRemoveTimestop(CSWSObject* pObject,
                                            CGameEffect* pEffect)
{
    g_pAppManager->m_pServerExoApp->TogglePauseState(1);
    g_pAppManager->m_pServerExoApp->RemoveFromExclusionList(pObject->m_nId,
                                                            1);
    return 1;
}
// PC 0x00530600
int CSWSEffectListHandler::OnApplyRacialType(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    if (pObject->AsSWSCreature() == NULL)
    {
        return 1;
    }
    return &g_pRules->m_pRaces[(unsigned short)pEffect->GetInteger(0)] == NULL;
}
// PC 0x00530640
int CSWSEffectListHandler::OnApplySeeInvisible(CSWSObject* pObject,
                                               CGameEffect* pEffect,
                                               int bLoadingModule)
{
    CSWSCreature* pCreature;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pCreature->m_nVisionFlags |= CSWSCreature::VISION_SEE_INVISIBLE;
    }
    return 0;
}
// PC 0x00530670
int CSWSEffectListHandler::OnApplyBlindness(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    CSWCCMessageData* pMessageData;
    CSWCCMessageData* pCreatorMessageData;
    CGameEffect* pMissChanceEffect;
    CGameEffect* pVisionEffect;
    CGameEffect* pVisualEffect;
    int nBlindness;
    int nMissChanceType;
    int bPlot;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        nBlindness = pEffect->GetInteger(0);
        if (pCreature == NULL ||
            (nBlindness != CSWSCreature::VISION_BLIND &&
             nBlindness != CSWSCreature::VISION_DARKNESS))
        {
            return 1;
        }
        unsigned long oidCreator = pEffect->m_oidCreator;
        pCreator = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
            oidCreator);
        if (nBlindness == CSWSCreature::VISION_BLIND &&
            pCreature->m_pStats->GetEffectImmunity(
                CSWSCreatureStats::IMMUNITY_TYPE_BLINDNESS, pCreator) != 0)
        {
            pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, pObject->GetId());
            if (pCreator != NULL)
            {
                pCreatorMessageData = new CSWCCMessageData;
                pMessageData->CopyTo(pCreatorMessageData);
                pCreator->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_BLINDNESS_IMMUNITY,
                    pCreatorMessageData);
            }
            pObject->AsSWSCreature()->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_BLINDNESS_IMMUNITY, pMessageData);
            return 1;
        }
        bPlot = pCreature->m_bPlotObject;
        if (bPlot != 0)
        {
            return 1;
        }
        if (nBlindness == CSWSCreature::VISION_DARKNESS)
        {
            nMissChanceType = MISS_CHANCE_TYPE_DARKNESS;
        }
        else
        {
            nMissChanceType = MISS_CHANCE_TYPE_NORMAL;
        }
        if (nBlindness != CSWSCreature::VISION_BLIND)
        {
            if (nBlindness != CSWSCreature::VISION_DARKNESS)
            {
                return 0;
            }
            if (pCreature->m_nVisionFlags & CSWSCreature::VISION_ULTRAVISION)
            {
                return 0;
            }
            if (pCreature->m_nVisionFlags & CSWSCreature::VISION_TRUESEEING)
            {
                return 0;
            }
        }
        pMissChanceEffect = new CGameEffect(pEffect);
        pMissChanceEffect->m_nType = EFFECT_MISS_CHANCE;
        pMissChanceEffect->SetInteger(0, BLINDNESS_MISS_CHANCE_PERCENT);
        pMissChanceEffect->SetInteger(1, nMissChanceType);
        pCreature->ApplyEffect(pMissChanceEffect, bLoadingModule, 0);
        pVisionEffect = new CGameEffect(pEffect);
        pVisionEffect->m_nType = EFFECT_VISION;
        pVisionEffect->SetInteger(0, VISION_EFFECT_TYPE_BLIND);
        pCreature->ApplyEffect(pVisionEffect, bLoadingModule, 0);
        pVisualEffect = new CGameEffect(pEffect);
        pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
        pVisualEffect->SetInteger(0, VFX_ARD_LIGHT_BLIND);
        pCreature->ApplyEffect(pVisualEffect, bLoadingModule, 0);
        pCreature->m_nVisionFlags |= nBlindness;
    }
    return 0;
}
// PC 0x00530900
int CSWSEffectListHandler::OnApplyMissChance(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    return pEffect->GetInteger(0) > 100 || pEffect->GetInteger(0) < 1;
}
// PC 0x00530900
int CSWSEffectListHandler::OnApplyConcealment(CSWSObject* pObject,
                                          CGameEffect* pEffect,
                                          int bLoadingModule)
{
    return pEffect->GetInteger(0) > 100 || pEffect->GetInteger(0) < 1;
}
// PC 0x00530930
int CSWSEffectListHandler::OnApplyDarkness(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    CSWSCreature* pCreature;
    CGameEffect* pDarkEffect;
    CGameEffect* pBlindEffect;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pDarkEffect = new CGameEffect(pEffect);
        pDarkEffect->m_nType = EFFECT_INVISIBILITY;
        pDarkEffect->SetInteger(0, 2);
        pDarkEffect->SetInteger(1, (unsigned char)g_pRules->m_nNumRaces);
        pCreature->ApplyEffect(pDarkEffect, bLoadingModule, 0);
        pBlindEffect = new CGameEffect(pEffect);
        pBlindEffect->m_nType = EFFECT_BLINDNESS;
        pBlindEffect->SetInteger(0, 8);
        pCreature->ApplyEffect(pBlindEffect, bLoadingModule, 0);
    }
    return 0;
}
// PC 0x00530A40
int CSWSEffectListHandler::OnApplyTurnResistance(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    return pObject == NULL || pObject->AsSWSCreature() == NULL ||
           pObject->AsSWSCreature()->m_pStats->m_bIsPC != 0;
}
// PC 0x00530A80
int CSWSEffectListHandler::OnApplyDisappearAppear(CSWSObject* pObject,
                                                  CGameEffect* pEffect,
                                                  int bLoadingModule)
{
    CSWSCreature* pCreature;
    CServerExoApp* pApp;
    unsigned long oidTarget;
    unsigned long oidCreator;
    unsigned long oidSelf;
    Vector vTargetPosition(pEffect->GetFloat(0), pEffect->GetFloat(1),
                           pEffect->GetFloat(2));

    oidTarget = pEffect->GetObjectID(0);
    if (g_pAppManager->m_pServerExoApp->GetAreaByGameObjectID(oidTarget) ==
        NULL)
    {
        return 1;
    }
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        pCreature = pObject->AsSWSCreature();
        pCreature->ClearAllActions(1);
        pCreature->PacifyCreature();
        pCreature->m_oidDisappearTarget = oidTarget;
        pCreature->m_vDisappearPosition =
            pObject->AsSWSCreature()->m_vPosition;
        pCreature->m_bWorkRemainingCompleteSent = 0;
        pCreature->m_pcCombatRound->SetPauseTimer(0, 1);
        pCreature->m_pcCombatRound->SetRoundPaused(1, pObject->m_nId);
        pCreature->AddDisappearActions(1);
        return 0;
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        oidCreator = pEffect->m_oidCreator;
        oidSelf = pObject->m_nId;
        pApp = g_pAppManager->m_pServerExoApp;
        pApp->GetServerAIMaster()->AddEventDeltaTime(
            0, 0x7D0, oidCreator, oidSelf,
            CServerAIMaster::EVENT_REMOVE_FROM_AREA, NULL);
        return 0;
    }
    return 1;
}
// PC 0x00530BB0
int CSWSEffectListHandler::OnRemoveDisappearAppear(CSWSObject* pObject,
                                                  CGameEffect* pEffect)
{
    Vector vTargetPosition(pEffect->GetFloat(0), pEffect->GetFloat(1),
                           pEffect->GetFloat(2));
    Vector vSafePosition;
    unsigned long oidTarget = pEffect->GetObjectID(0);
    CSWSArea* pArea = g_pAppManager->GetServerExoApp()->GetAreaByGameObjectID(
        oidTarget);
    EXOXBOXASSERT(pArea);

    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        pCreature->ClearAllActions(1);
        pCreature->m_pcCombatRound->SetRoundPaused(0, OBJECT_INVALID);
        pCreature->m_pcCombatRound->SetPauseTimer(2000, 0);
        pCreature->m_pcCombatRound->SetRoundPaused(1, OBJECT_INVALID);
        pCreature->AddAppearActions();
        if (pArea->ComputeSafeLocation(vTargetPosition, 20.0f,
                                      pCreature->GetPathfindInformation(), 0,
                                      &vSafePosition))
        {
            vSafePosition.m_fZ = pArea->ComputeHeight(vSafePosition, 0, NULL);
            pCreature->AddToArea(pArea, vSafePosition.m_fX, vSafePosition.m_fY,
                                 vSafePosition.m_fZ, 0, 0);
        }
        else
        {
            pEffect->SetFloat(2, pArea->ComputeHeight(vTargetPosition, 0, NULL));
            pCreature->AddToArea(pArea, pEffect->GetFloat(0),
                                 pEffect->GetFloat(1), pEffect->GetFloat(2),
                                 0, 0);
        }
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        pObject->AsSWSPlaceable()->AddToArea(
            pArea, Vector(pEffect->GetFloat(0), pEffect->GetFloat(1),
                          pEffect->GetFloat(2)), 0);
    }
    return 1;
}
// PC 0x00530DB0
int CSWSEffectListHandler::OnApplyDisappear(CSWSObject* pObject,
                                            CGameEffect* pEffect,
                                            int bLoadingModule)
{
    CSWSCreature* pCreature;
    CServerExoApp* pApp;
    unsigned long oidCreator;
    unsigned long oidSelf;

    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        pCreature = pObject->AsSWSCreature();
        pCreature->ClearAllActions(1);
        pCreature->AddDisappearActions(0);
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        oidSelf = pObject->m_nId;
        oidCreator = pEffect->m_oidCreator;
        pApp = g_pAppManager->m_pServerExoApp;
        pApp->GetServerAIMaster()->AddEventDeltaTime(
            0, 2000, oidCreator, oidSelf,
            CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
    }
    return 1;
}
// PC 0x00530E30
int CSWSEffectListHandler::OnApplyAppear(CSWSObject* pObject,
                                       CGameEffect* pEffect,
                                       int bLoadingModule)
{
    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        pCreature->ClearAllActions(1);
        pCreature->AddAppearActions();
    }
    return 1;
}
// PC 0x00530E60
int CSWSEffectListHandler::OnApplyNegativeLevel(CSWSObject* pObject,
                                                CGameEffect* pEffect,
                                                int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    CSWCCMessageData* pMessageData;
    CSWCCMessageData* pCreatorMessageData;
    CGameEffect* pNewEffect;
    int nClass;
    int nHighestLevel;
    int nNegativeLevels;

    if (pEffect->GetInteger(0) > NEGATIVE_LEVEL_MAXIMUM)
    {
        return 1;
    }
    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        unsigned long oidCreator = pEffect->m_oidCreator;
        pCreator = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
            oidCreator);
        if (pCreature->GetStats()->GetEffectImmunity(
                CSWSCreatureStats::IMMUNITY_TYPE_NEGATIVE_LEVEL, pCreator) != 0)
        {
            pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, pObject->GetId());
            if (pCreator != NULL)
            {
                pCreatorMessageData = new CSWCCMessageData;
                pMessageData->CopyTo(pCreatorMessageData);
                pCreator->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_NEGATIVE_LEVEL_IMMUNITY,
                    pCreatorMessageData);
            }
            pCreature->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_NEGATIVE_LEVEL_IMMUNITY, pMessageData);
            return 1;
        }

        pNewEffect = new CGameEffect(pEffect);
        pNewEffect->m_nType = EFFECT_ATTACK_DECREASE;
        pNewEffect->SetInteger(0, pEffect->GetInteger(0));
        pNewEffect->SetInteger(1, 0);
        pNewEffect->SetInteger(2, g_pRules->m_nNumRaces);
        pCreature->ApplyEffect(pNewEffect, bLoadingModule, 0);

        pNewEffect = new CGameEffect(pEffect);
        pNewEffect->m_nType = EFFECT_SAVING_THROW_DECREASE;
        pNewEffect->SetInteger(0, pEffect->GetInteger(0));
        pNewEffect->SetInteger(1, 0);
        pNewEffect->SetInteger(3, g_pRules->m_nNumRaces);
        pCreature->ApplyEffect(pNewEffect, bLoadingModule, 0);

        pNewEffect = new CGameEffect(pEffect);
        pNewEffect->m_nType = EFFECT_SKILL_DECREASE;
        pNewEffect->SetInteger(0, NEGATIVE_LEVEL_ALL_SKILLS);
        pNewEffect->SetInteger(1, pEffect->GetInteger(0));
        pNewEffect->SetInteger(2, g_pRules->m_nNumRaces);
        pCreature->ApplyEffect(pNewEffect, bLoadingModule, 0);

        nClass = 0;
        nHighestLevel = 0;
        for (int i = 0; i < pCreature->GetStats()->GetNumClasses(); ++i)
        {
            if (pCreature->GetStats()->GetClassLevel(i, 0) > nHighestLevel)
            {
                nClass = i;
                nHighestLevel = pCreature->GetStats()->GetClassLevel(i, 0);
            }
        }
        if (nHighestLevel > 0)
        {
            nNegativeLevels = pCreature->GetStats()->GetClassNegativeLevels(nClass);
            pCreature->GetStats()->SetClassNegativeLevels(
                nClass, nNegativeLevels + pEffect->GetInteger(0));
            pEffect->SetInteger(1, nClass);
            if (pCreature->GetStats()->GetLevel(1) <= 0)
            {
                pNewEffect = new CGameEffect(1);
                pNewEffect->m_nType = EFFECT_DEATH;
                pNewEffect->m_nSubType =
                    CGameEffect::DURATION_TYPE_INSTANTANEOUS |
                    (pNewEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
                pNewEffect->SetInteger(0, 1);
                pNewEffect->SetInteger(1, 1);
                oidCreator = pEffect->m_oidCreator;
                pNewEffect->SetCreator(oidCreator);
                pCreature->ApplyEffect(pNewEffect, bLoadingModule, 0);
            }
        }
        if (pCreator != NULL)
        {
            pNewEffect = new CGameEffect(1);
            pNewEffect->m_nSubType =
                CGameEffect::DURATION_TYPE_PERMANENT |
                (pNewEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
            pNewEffect->m_nType = EFFECT_TEMPORARY_HITPOINTS;
            pNewEffect->SetInteger(0, pEffect->GetInteger(0) * NEGATIVE_LEVEL_HITPOINTS);
            pCreator->ApplyEffect(pNewEffect, bLoadingModule, 0);
        }
    }
    return 0;
}
// PC 0x005312B0
int CSWSEffectListHandler::OnRemoveNegativeLevel(CSWSObject* pObject,
                                                 CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;
    int nClass;
    int nLevels;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        nClass = pEffect->GetInteger(1);
        nLevels = pCreature->m_pStats->GetClassNegativeLevels(nClass);
        pStats = pCreature->m_pStats;
        pStats->SetClassNegativeLevels(nClass,
                                       nLevels - pEffect->GetInteger(0));
    }
    return 1;
}
// PC 0x00531310
int CSWSEffectListHandler::OnApplyWounding(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    CGameEffect* pWoundEffect;

    if (pObject->AsSWSCreature() == NULL ||
        (pEffect->m_nSubType & CGameEffect::DURATION_MASK) !=
            CGameEffect::DURATION_TYPE_INSTANTANEOUS)
    {
        return 1;
    }
    pWoundEffect = new CGameEffect(1);
    pWoundEffect->SetCreator(pEffect->m_oidCreator);
    pWoundEffect->m_nType = EFFECT_WOUNDING;
    pWoundEffect->m_nSubType =
        CGameEffect::DURATION_TYPE_TEMPORARY |
        (pWoundEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pWoundEffect->m_fDuration = 6.0f;
    pObject->ApplyEffect(pWoundEffect, bLoadingModule, 0);
    return 0;
}
// PC 0x00531400
int CSWSEffectListHandler::OnRemoveWounding(CSWSObject* pObject,
                                            CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    int bDead;
    int bDeadTemp;
    CGameEffect* pBloodEffect;
    CGameEffect* pDeathEffect;
    CGameEffect* pWoundEffect;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->GetIsPCDying();
        bDead = pCreature->GetDead();
        bDeadTemp = pCreature->GetDeadTemp();
        pCreature->SetCurrentHitPoints(pCreature->GetCurrentHitPoints(1) - 1);

        CExoString sBloodColor;
        g_pRules->Get2DArrays()->GetAppearance()->GetCExoStringEntry(
            pCreature->m_cAppearanceInfo.m_nAppearance, "BLOODCOLR", &sBloodColor);
        pBloodEffect = new CGameEffect(1);
        pBloodEffect->m_nType = EFFECT_VISUALEFFECT;
        pBloodEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_INSTANTANEOUS |
            (pBloodEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        if (sBloodColor == "R")
        {
            pBloodEffect->SetInteger(0, VFX_BLOOD_RED);
        }
        else if (sBloodColor == "G")
        {
            pBloodEffect->SetInteger(0, VFX_BLOOD_GREEN);
        }
        else if (sBloodColor == "Y")
        {
            pBloodEffect->SetInteger(0, VFX_BLOOD_YELLOW);
        }
        pCreature->ApplyEffect(pBloodEffect, 0, 0);

        if ((!bDead && pCreature->GetDead()) ||
            (!bDeadTemp && pCreature->GetDeadTemp()))
        {
            pDeathEffect = new CGameEffect(1);
            pDeathEffect->m_nType = EFFECT_DEATH;
            pDeathEffect->m_nSubType =
                CGameEffect::DURATION_TYPE_INSTANTANEOUS |
                (pDeathEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
            pDeathEffect->SetCreator(pEffect->GetCreator());
            pDeathEffect->SetInteger(0, 0);
            pDeathEffect->SetInteger(1, 1);
            pCreature->ApplyEffect(pDeathEffect, 0, 0);
        }
        else
        {
            pWoundEffect = new CGameEffect(1);
            pWoundEffect->SetCreator(pEffect->GetCreator());
            pWoundEffect->m_nType = EFFECT_WOUNDING;
            pWoundEffect->m_nSubType =
                CGameEffect::DURATION_TYPE_TEMPORARY |
                (pWoundEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
            pWoundEffect->m_fDuration = 6.0f;
            pObject->ApplyEffect(pWoundEffect, 0, 0);
        }
    }
    return 1;
}
// PC 0x005316C0
int CSWSEffectListHandler::OnApplyVampiricRegeneration(CSWSObject* pObject,
                                                       CGameEffect* pEffect,
                                                       int bLoadingModule)
{
    CSWSCreature* pCreator;
    CGameEffect* pHealEffect;
    int nDamage;
    int nMaximum;

    pHealEffect = NULL;
    pCreator = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
        pEffect->m_oidCreator);
    if (pCreator != NULL)
    {
        nDamage = pEffect->GetInteger(0);
        nMaximum = pEffect->GetInteger(1);
        pHealEffect = new CGameEffect(1);
        pHealEffect->m_nType = EFFECT_HEAL;
        pHealEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_INSTANTANEOUS |
            (pHealEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pHealEffect->SetCreator(pEffect->m_oidCreator);
        if (nMaximum < nDamage)
        {
            nDamage = nMaximum;
        }
        pHealEffect->SetInteger(0, nDamage);
        pCreator->ApplyEffect(pHealEffect, bLoadingModule, 0);
    }
    return 1;
}
// PC 0x005317B0
int CSWSEffectListHandler::OnApplyTemporaryForce(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;
    short nTemporaryForcePoints;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pStats = pCreature->m_pStats;
        if (pStats != NULL)
        {
            nTemporaryForcePoints = pStats->m_nTemporaryForcePoints;
            pStats->m_nTemporaryForcePoints =
                nTemporaryForcePoints + pEffect->GetInteger(0);
        }
    }
    return 0;
}
// PC 0x005317F0
int CSWSEffectListHandler::OnRemoveTemporaryForce(CSWSObject* pObject,
                                                  CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;
    short nTemporaryForcePoints;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pStats = pCreature->m_pStats;
        if (pStats != NULL)
        {
            nTemporaryForcePoints = pStats->m_nTemporaryForcePoints;
            pStats->m_nTemporaryForcePoints =
                nTemporaryForcePoints - pEffect->GetInteger(0);
        }
    }
    return 1;
}
// PC 0x00531830
int CSWSEffectListHandler::OnApplyForceDrain(CSWSObject* pObject,
                                             CGameEffect* pEffect,
                                             int bLoadingModule)
{
    CSWSCreatureStats* pStats;
    int nDrain;

    nDrain = pEffect->GetInteger(0);
    if (pObject->AsSWSCreature() != NULL)
    {
        pStats = pObject->AsSWSCreature()->m_pStats;
        pStats->m_nCurrentForcePoints =
            (short)(pStats->m_nTemporaryForcePoints +
                    pStats->m_nCurrentForcePoints) -
                        nDrain <
                    0
                ? 0
                : (short)(pStats->m_nTemporaryForcePoints +
                          pStats->m_nCurrentForcePoints) -
                      nDrain;
    }
    return 1;
}
// PC 0x00531890
int CSWSEffectListHandler::OnRemoveDroidStun(CSWSObject* pObject,
                                                  CGameEffect* pEffect)
{
    pObject->AsSWSCreature();
    return 1;
}
// PC 0x005318B0
int CSWSEffectListHandler::OnApplyForceJump(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pTarget;
    CGameEffect* pJumpEffect;
    CGameEffect* pVisualEffect;
    CServerExoApp* pApp;
    unsigned long oidCreature;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pApp = g_pAppManager->GetServerExoApp();
        pTarget = pApp->GetCreatureByGameObjectID(pEffect->GetObjectID(0));
        if (pTarget != NULL)
        {
            pCreature->SetLockOrientationToObject(pTarget->GetId(), 0);
            pJumpEffect = new CGameEffect(pEffect);
            pJumpEffect->m_nType = EFFECT_FORCEJUMPDELAYED;
            pJumpEffect->m_nSubType =
                CGameEffect::DURATION_TYPE_TEMPORARY |
                (pJumpEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
            pJumpEffect->SetObjectID(0, pTarget->GetId());
            oidCreature = pCreature->GetId();
            pApp = g_pAppManager->GetServerExoApp();
            pApp->GetServerAIMaster()->AddEventDeltaTime(
                0, FORCE_JUMP_TIME_DELTA, oidCreature, oidCreature,
                CServerAIMaster::EVENT_APPLY_EFFECT, pJumpEffect);
            pVisualEffect = new CGameEffect(pEffect);
            pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
            pVisualEffect->m_nSubType =
                CGameEffect::DURATION_TYPE_INSTANTANEOUS |
                (pVisualEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
            pVisualEffect->SetInteger(0, VFX_IMP_FORCE_JUMP_ADVANCED);
            pCreature->ApplyEffect(pVisualEffect, 0, 0);
        }
    }
    return 0;
}
// PC 0x00531A10
int CSWSEffectListHandler::OnApplyForceJumpDelayed(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        CServerExoApp* pApp = g_pAppManager->GetServerExoApp();
        CSWSCreature* pTarget = pApp->GetCreatureByGameObjectID(
            pEffect->GetObjectID(0));
        if (pTarget != NULL)
        {
            pCreature->SetLockOrientationToObject(pTarget->GetId(), 0);
            Vector vDirection = normalize(pTarget->m_vPosition -
                                          pCreature->m_vPosition);
            Vector vDestination = pTarget->m_vPosition - vDirection *
                pCreature->DesiredAttackRange(pTarget->GetId(), 1);
            if (!g_pAppManager->GetServerExoApp()->TestSafeLocation(
                    pTarget->GetId(), vDestination))
            {
                CSWSCombatRound* pCombatRound = pCreature->m_pcCombatRound;
                int bEngaged = pCombatRound->m_bEngaged;
                Vector vTargetDestination = pTarget->m_vPosition + vDirection *
                    pCreature->DesiredAttackRange(pTarget->GetId(), bEngaged);
                vDestination = pTarget->m_vPosition;
                if (pTarget->GetPathfindInformation()->m_nWayPointCount != 0)
                {
                    float* pOldWayPoints = pTarget->GetPathfindInformation()->m_pWayPoints;
                    pTarget->GetPathfindInformation()->m_pWayPoints = NULL;
                    delete pOldWayPoints;
                }
                float* pTargetWayPoints = new float[4];
                pTargetWayPoints[0] = pTarget->m_vPosition.m_fX;
                pTargetWayPoints[1] = pTarget->m_vPosition.m_fY;
                pTargetWayPoints[2] = vTargetDestination.m_fX;
                pTargetWayPoints[3] = vTargetDestination.m_fY;
                unsigned long oidArea = pTarget->m_oidArea;
                pTarget->AddMoveToPointActionToFront(
                    CSWSObject::ACTION_GROUP_NEW, vTargetDestination,
                    oidArea, OBJECT_INVALID, 0, 0.0f, 0.0f,
                    0, 0, 0, 0, 0, 0, Vector(), 0, 0, 0);
                pTarget->m_nAnimationState = CSWSCreature::ANIMATION_STATE_FORCE_JUMP_TARGET;
                pTarget->GetPathfindInformation()->m_nWayPointCount = 4;
                pTarget->GetPathfindInformation()->m_pWayPoints = pTargetWayPoints;
                pTarget->GetPathfindInformation()->m_nField_9C = 0;
                pTarget->WalkUpdateLocation_QuickWalk(1);
                CGameEffect* pStateEffect = new CGameEffect(pEffect);
                pStateEffect->m_nType = EFFECT_SETSTATE;
                pStateEffect->m_nSubType = CGameEffect::DURATION_TYPE_TEMPORARY |
                    (pStateEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
                pStateEffect->SetInteger(0, STATE_FORCE_JUMP_TARGET);
                pTarget->ApplyEffect(pStateEffect, 0, 0);
            }
            if (pCreature->GetPathfindInformation()->m_nWayPointCount != 0)
            {
                float* pOldWayPoints = pCreature->GetPathfindInformation()->m_pWayPoints;
                pCreature->GetPathfindInformation()->m_pWayPoints = NULL;
                delete pOldWayPoints;
            }
            float* pWayPoints = new float[4];
            pWayPoints[0] = pCreature->m_vPosition.m_fX;
            pWayPoints[1] = pCreature->m_vPosition.m_fY;
            pWayPoints[2] = vDestination.m_fX;
            pWayPoints[3] = vDestination.m_fY;
            pCreature->GetPathfindInformation()->m_nWayPointCount = 4;
            pCreature->GetPathfindInformation()->m_pWayPoints = pWayPoints;
            pCreature->GetPathfindInformation()->m_nField_9C = 0;
            pCreature->m_nAnimationState = CSWSCreature::ANIMATION_STATE_FORCE_JUMP;
            pCreature->WalkUpdateLocation_QuickWalk(1);
        }
    }
    return 0;
}
// PC 0x00531D70
int CSWSEffectListHandler::OnRemoveForcePush(CSWSObject* pObject,
                                             CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pPushEffect;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->SetLockOrientationToObject(OBJECT_INVALID, 0);
        pPushEffect = new CGameEffect(1);
        pPushEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_TEMPORARY |
            (pPushEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pPushEffect->m_fDuration = 2.55f;
        pPushEffect->SetCreator(pPushEffect->m_oidCreator);
        pPushEffect->m_nType = EFFECT_SET_AI_STATE;
        pPushEffect->SetInteger(0, -351);
        pCreature->ApplyEffect(pPushEffect, 0, 0);
    }
    return 1;
}
// PC 0x00531E40
int CSWSEffectListHandler::OnApplyDamageForcePoints(CSWSObject* pObject,
                                                    CGameEffect* pEffect,
                                                    int bLoadingModule)
{
    CSWSCreature* pCreature;
    int nForcePoints;
    int nDamage;

    nForcePoints = 0;
    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL && pCreature->m_pStats != NULL)
    {
        nDamage = pEffect->GetInteger(0);
        if (pCreature->m_pStats->GetCurrentForcePoints() > nDamage)
        {
            nForcePoints =
                pCreature->m_pStats->GetCurrentForcePoints() - nDamage;
        }
        pCreature->m_pStats->m_nCurrentForcePoints = nForcePoints;
    }
    return 1;
}
// PC 0x00531EB0
int CSWSEffectListHandler::OnApplyHealForcePoints(CSWSObject* pObject,
                                                  CGameEffect* pEffect,
                                                  int bLoadingModule)
{
    CSWSCreature* pCreature;
    int nForcePoints;
    int nHeal;
    short nCurrent;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL && pCreature->m_pStats != NULL)
    {
        nHeal = pEffect->GetInteger(0);
        nCurrent = pCreature->m_pStats->m_nTemporaryForcePoints +
                   pCreature->m_pStats->m_nCurrentForcePoints;
        nForcePoints = nHeal + nCurrent;
        if (nForcePoints > pCreature->GetMaxForcePoints())
        {
            nForcePoints = pCreature->GetMaxForcePoints();
        }
        pCreature->m_pStats->m_nCurrentForcePoints = nForcePoints;
    }
    return 1;
}
// PC 0x00531FA0
int CSWSEffectListHandler::OnApplyBodyFuel(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    CSWSCreature* pCreature;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->m_bBodyFuel = 1;
    }
    return 0;
}
// PC 0x00531FC0
int CSWSEffectListHandler::OnRemoveBodyFuel(CSWSObject* pObject,
                                            CGameEffect* pEffect)
{
    CSWSCreature* pCreature;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->m_bBodyFuel = 0;
    }
    return 1;
}
// PC 0x00531FE0
int CSWSEffectListHandler::OnApplyEffectImmunity(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    pObject->AsSWSCreature();
    return 0;
}
// PC 0x00531FE0
int CSWSEffectListHandler::OnApplyDroidStun(CSWSObject* pObject,
                                            CGameEffect* pEffect,
                                            int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 0;
}
// PC 0x00531FE0
int CSWSEffectListHandler::OnApplyWhirlWind(CSWSObject* pObject,
                                            CGameEffect* pEffect,
                                            int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 0;
}
// PC 0x00531FE0
int CSWSEffectListHandler::OnApplyBlasterDeflectionIncrease(CSWSObject* pObject,
                                                            CGameEffect* pEffect,
                                                            int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 0;
}
// PC 0x00531FE0
int CSWSEffectListHandler::OnApplyBlasterDeflectionDecrease(CSWSObject* pObject,
                                                            CGameEffect* pEffect,
                                                            int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 0;
}
// PC 0x00531FE0
int CSWSEffectListHandler::OnApplyHorrified(CSWSObject* pObject,
                                            CGameEffect* pEffect,
                                            int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 0;
}
// PC 0x00531FE0
int CSWSEffectListHandler::OnApplyChoke(CSWSObject* pObject,
                                        CGameEffect* pEffect,
                                        int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 0;
}
// PC 0x00531FE0
int CSWSEffectListHandler::OnApplyPsychicStatic(CSWSObject* pObject,
                                                CGameEffect* pEffect,
                                                int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 0;
}
// PC 0x00531FF0
int CSWSEffectListHandler::OnApplyLightsaberThrow(CSWSObject* pObject,
                                                  CGameEffect* pEffect,
                                                  int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->m_bLightsaberThrow = 1;
        CSWInventory* pInventory = pCreature->m_pInventory;
        if (pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND) != NULL)
        {
            unsigned long oidTargets[LIGHTSABER_THROW_ROUTE_LENGTH];
            oidTargets[0] = pObject->GetId();
            oidTargets[1] = pEffect->GetObjectID(0);
            oidTargets[2] = pEffect->GetObjectID(1);
            oidTargets[3] = pEffect->GetObjectID(2);
            CSWSpell* pSpell;
            if (pEffect->GetInteger(0))
                pSpell = g_pRules->m_pSpellArray->GetSpell(
                    CSWSpell::FORCE_POWER_LIGHT_SABER_THROW_ADVANCED);
            else
                pSpell = g_pRules->m_pSpellArray->GetSpell(
                    CSWSpell::FORCE_POWER_LIGHT_SABER_THROW);

            int nCastTime = pSpell->m_nCastTime;
            CGameObject* pTargets[LIGHTSABER_THROW_ROUTE_LENGTH];
            pTargets[0] = NULL;
            if (oidTargets[0] != OBJECT_INVALID)
                pTargets[0] = g_pAppManager->GetServerExoApp()->GetGameObject(oidTargets[0]);
            pTargets[1] = NULL;
            if (oidTargets[0] != OBJECT_INVALID)
                pTargets[1] = g_pAppManager->GetServerExoApp()->GetGameObject(oidTargets[1]);
            pTargets[2] = NULL;
            if (oidTargets[0] != OBJECT_INVALID)
                pTargets[2] = g_pAppManager->GetServerExoApp()->GetGameObject(oidTargets[2]);
            pTargets[3] = NULL;
            if (oidTargets[0] != OBJECT_INVALID)
                pTargets[3] = g_pAppManager->GetServerExoApp()->GetGameObject(oidTargets[3]);

            float fDistance = 0.0f;
            float fReturnDistance = 0.0f;
            if (pTargets[1] != NULL)
            {
                CSWSObject* pToObject = pTargets[1]->AsSWSObject();
                CSWSObject* pFromObject = pTargets[0]->AsSWSObject();
                fReturnDistance = magnitude(Vector(pFromObject->m_vPosition.m_fX - pToObject->m_vPosition.m_fX,
                                     pFromObject->m_vPosition.m_fY - pToObject->m_vPosition.m_fY,
                                     pFromObject->m_vPosition.m_fZ - pToObject->m_vPosition.m_fZ));
                fDistance += fReturnDistance;
                if (pTargets[2] != NULL)
                {
                    CSWSObject* pToObject = pTargets[2]->AsSWSObject();
                    CSWSObject* pFromObject = pTargets[1]->AsSWSObject();
                    fDistance += magnitude(Vector(pFromObject->m_vPosition.m_fX - pToObject->m_vPosition.m_fX,
                                     pFromObject->m_vPosition.m_fY - pToObject->m_vPosition.m_fY,
                                     pFromObject->m_vPosition.m_fZ - pToObject->m_vPosition.m_fZ));
                    pToObject = pTargets[2]->AsSWSObject();
                    pFromObject = pTargets[0]->AsSWSObject();
                    fReturnDistance = magnitude(Vector(pFromObject->m_vPosition.m_fX - pToObject->m_vPosition.m_fX,
                                     pFromObject->m_vPosition.m_fY - pToObject->m_vPosition.m_fY,
                                     pFromObject->m_vPosition.m_fZ - pToObject->m_vPosition.m_fZ));
                }
            }
            if (pTargets[2] != NULL && pTargets[3] != NULL)
            {
                CSWSObject* pToObject = pTargets[3]->AsSWSObject();
                CSWSObject* pFromObject = pTargets[2]->AsSWSObject();
                fDistance += magnitude(Vector(pFromObject->m_vPosition.m_fX - pToObject->m_vPosition.m_fX,
                                     pFromObject->m_vPosition.m_fY - pToObject->m_vPosition.m_fY,
                                     pFromObject->m_vPosition.m_fZ - pToObject->m_vPosition.m_fZ));
                pToObject = pTargets[3]->AsSWSObject();
                pFromObject = pTargets[0]->AsSWSObject();
                fReturnDistance = magnitude(Vector(pFromObject->m_vPosition.m_fX - pToObject->m_vPosition.m_fX,
                                     pFromObject->m_vPosition.m_fY - pToObject->m_vPosition.m_fY,
                                     pFromObject->m_vPosition.m_fZ - pToObject->m_vPosition.m_fZ));
            }
            int nSegment = 1;
            fDistance += fReturnDistance;
            float fThrowRate = fDistance / nCastTime *
                              (float)LIGHTSABER_THROW_TIME_SCALE;
            pCreature->m_fLightsaberThrowRate = fThrowRate;
            unsigned long nTime = 0;
            CServerAIMaster* pAIMaster = g_pAppManager->GetServerExoApp()->GetServerAIMaster();
            while (pTargets[nSegment - 1] != NULL && nSegment <= LIGHTSABER_THROW_ROUTE_LENGTH)
            {
                CGameObject* pPreviousTarget = pTargets[nSegment - 1];
                int nNextTarget;
                if (oidTargets[nSegment] == OBJECT_INVALID)
                    nNextTarget = 0;
                else
                    nNextTarget = nSegment > LIGHTSABER_THROW_TARGET_COUNT ? 0 : nSegment;
                CGameObject* pNextTarget = pTargets[nNextTarget];
                if (pPreviousTarget != NULL &&
                    pPreviousTarget->AsSWSObject() != NULL &&
                    pNextTarget != NULL && pNextTarget->AsSWSObject() != NULL)
                {
                    CGameEffect* pVisualEffect = new CGameEffect(1);
                    pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
                    pVisualEffect->m_nSubType =
                        (pVisualEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_TEMPORARY;
                    pVisualEffect->SetInteger(0, VFX_IMP_MIRV);
                    pVisualEffect->SetInteger(2, 0);
                    unsigned long oidSource = oidTargets[nSegment - 1];
                    pVisualEffect->SetObjectID(0, oidSource);
                    pVisualEffect->SetObjectID(1, pCreature->GetId());
                    CSWSObject* pNextObject = pNextTarget->AsSWSObject();
                    CSWSObject* pPreviousObject = pTargets[nSegment - 1]->AsSWSObject();
                    unsigned long nDeltaTime = (unsigned long)(magnitude(
                        pPreviousObject->m_vPosition - pNextObject->m_vPosition) /
                        fThrowRate * (float)LIGHTSABER_THROW_TIME_SCALE);
                    unsigned long oidTarget = oidTargets[nNextTarget];
                    pAIMaster->AddEventDeltaTime(0, nTime, oidSource, oidTarget,
                        CServerAIMaster::EVENT_APPLY_EFFECT, pVisualEffect);
                    if (nNextTarget != 0)
                    {
                        CGameEffect* pDamageEffect = new CGameEffect(1);
                        pDamageEffect->m_nType = EFFECT_DAMAGE;
                        pDamageEffect->m_nSubType =
                            (pDamageEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_TEMPORARY;
                        pDamageEffect->SetCreator(oidTargets[0]);
                        pDamageEffect->SetNumIntegers(21);
                        for (int nDamageType = 0; nDamageType < APPLIED_DAMAGE_TYPE_COUNT;
                             ++nDamageType)
                            pDamageEffect->SetInteger(nDamageType, -1);
                        int nDamage = g_pRules->RollDice(
                            (unsigned char)(pCreature->GetStats()->GetLevel(0) * 0.5f), 6);
                        if (pNextTarget->AsSWSCreature() != NULL)
                        {
                            CSWSCreature* pTargetCreature = pNextTarget->AsSWSCreature();
                            nDamage = pTargetCreature->DoDamageResistance(
                                pCreature, nDamage, DAMAGE_FLAG_BLASTER, 0, 0, 1);
                            nDamage = pTargetCreature->DoDamageImmunity(
                                pCreature, nDamage, DAMAGE_FLAG_BLASTER, 0, 0);
                        }
                        pDamageEffect->SetInteger(APPLIED_DAMAGE_BLASTER, nDamage);
                        pDamageEffect->SetInteger(APPLIED_DAMAGE_BASE, nDamage);
                        pDamageEffect->SetInteger(APPLIED_DAMAGE_ANIMATION_DELAY, 0);
                        pDamageEffect->SetInteger(APPLIED_DAMAGE_MELEE, 1);
                        pDamageEffect->SetInteger(APPLIED_DAMAGE_RANGED, 1);
                        CGameEffect* pImpactEffect = new CGameEffect(1);
                        pImpactEffect->m_nType = EFFECT_VISUALEFFECT;
                        pImpactEffect->m_nSubType =
                            (pImpactEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                        pImpactEffect->SetInteger(0, VFX_IMP_MIRV_IMPACT);
                        pImpactEffect->SetInteger(2, 0);
                        pImpactEffect->SetObjectID(0, pCreature->GetId());
                        pAIMaster->AddEventDeltaTime(0, nTime + nDeltaTime,
                            oidSource, oidTarget, CServerAIMaster::EVENT_APPLY_EFFECT,
                            pDamageEffect);
                        pAIMaster->AddEventDeltaTime(0, nTime + nDeltaTime,
                            oidSource, oidTarget, CServerAIMaster::EVENT_APPLY_EFFECT,
                            pImpactEffect);
                    }
                    nTime += nDeltaTime;
                }
                ++nSegment;
            }
        }
    }
    return 0;
}
// PC 0x00532700
int CSWSEffectListHandler::OnRemoveLightsaberThrow(CSWSObject* pObject,
                                                   CGameEffect* pEffect)
{
    CSWSCreature* pCreature;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->m_bLightsaberThrow = 0;
    }
    return 1;
}
// PC 0x00532720
int CSWSEffectListHandler::OnApplyForceResisted(CSWSObject* pObject,
                                                CGameEffect* pEffect,
                                                int bLoadingModule)
{
    CGameEffect* pResistedEffect;

    pResistedEffect = new CGameEffect(1);
    if (pResistedEffect != NULL)
    {
        pResistedEffect->m_nType = EFFECT_VISUALEFFECT;
        pResistedEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_INSTANTANEOUS |
            (pResistedEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pResistedEffect->SetCreator(pObject->m_nId);
        pResistedEffect->SetInteger(0, 4037);
        pResistedEffect->SetObjectID(0, pEffect->GetObjectID(0));
        pObject->ApplyEffect(pResistedEffect, bLoadingModule, 0);
        pObject->SetAnimation(10145);
    }
    return 1;
}
// PC 0x00532800
int CSWSEffectListHandler::OnApplyForceFizzle(CSWSObject* pObject,
                                              CGameEffect* pEffect,
                                              int bLoadingModule)
{
    CGameEffect* pFizzleEffect;

    pFizzleEffect = new CGameEffect(1);
    if (pFizzleEffect != NULL)
    {
        pFizzleEffect->m_nType = EFFECT_VISUALEFFECT;
        pFizzleEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_INSTANTANEOUS |
            (pFizzleEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pFizzleEffect->SetInteger(0, 4036);
        pFizzleEffect->SetCreator(pObject->m_nId);
        pObject->ApplyEffect(pFizzleEffect, bLoadingModule, 0);
    }
    return 1;
}

int CSWSEffectListHandler::OnRemoveNothing(CSWSObject* pObject,
                                           CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveSpellLevelAbsorption(CSWSObject* pObject,
                                                       CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveSpellImmunity(CSWSObject* pObject,
                                                 CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveMissChance(CSWSObject* pObject,
                                             CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveEffectImmunity(CSWSObject* pObject,
                                                 CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveLight(CSWSObject* pObject,
                                        CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveSkillIncrease(CSWSObject* pObject,
                                                CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveSkillDecrease(CSWSObject* pObject,
                                                CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveRacialType(CSWSObject* pObject,
                                             CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveVision(CSWSObject* pObject,
                                         CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveTurnResistance(CSWSObject* pObject,
                                                 CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveForceJump(CSWSObject* pObject,
                                            CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveForceJumpDelayed(CSWSObject* pObject,
                                                   CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveForceShield(CSWSObject* pObject,
                                              CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemovePureGoodPowers(CSWSObject* pObject,
                                                 CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemovePureEvilPowers(CSWSObject* pObject,
                                                 CGameEffect* pEffect)
{
    return 1;
}
// PC 0x0069E520
int CSWSEffectListHandler::OnApplyDamageResistance(CSWSObject* pObject,
                                                  CGameEffect* pEffect,
                                                  int bLoadingModule)
{
    return 0;
}
// PC 0x0069E520
int CSWSEffectListHandler::OnApplyDamageReduction(CSWSObject* pObject,
                                                 CGameEffect* pEffect,
                                                 int bLoadingModule)
{
    return 0;
}
// PC 0x0069E520
int CSWSEffectListHandler::OnApplySpellImmunity(CSWSObject* pObject,
                                               CGameEffect* pEffect,
                                               int bLoadingModule)
{
    return 0;
}
// PC 0x0069E520
int CSWSEffectListHandler::OnApplyDamageShield(CSWSObject* pObject,
                                              CGameEffect* pEffect,
                                              int bLoadingModule)
{
    return 0;
}
// PC 0x0069E520
int CSWSEffectListHandler::OnApplySpellLevelAbsorption(CSWSObject* pObject,
                                                      CGameEffect* pEffect,
                                                      int bLoadingModule)
{
    return 0;
}
// PC 0x0069E520
int CSWSEffectListHandler::OnApplyBonusSpellOfLevel(CSWSObject* pObject,
                                                   CGameEffect* pEffect,
                                                   int bLoadingModule)
{
    return 0;
}
// PC 0x005328B0
int CSWSEffectListHandler::OnApplyPureGoodPowers(CSWSObject* pObject,
                                              CGameEffect* pEffect,
                                              int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;
    CSWSPlayer* pPlayer;
    CGameEffect* pBonusEffect;
    unsigned char nClassIndex;
    unsigned char nClass;

    pCreature = pObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 1;
    }
    pStats = pCreature->m_pStats;
    if (pStats == NULL)
    {
        return 1;
    }
    unsigned long oidCreature = pCreature->m_nId;
    pPlayer = g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(
        oidCreature);
    if (pPlayer != NULL)
    {
        unsigned long oidControlled = pPlayer->m_oidPCObject;
        if (oidControlled == pCreature->m_nId)
        {
            for (nClassIndex = 0; nClassIndex < pStats->m_nClassCount; nClassIndex++)
            {
                nClass = pStats->GetClass(nClassIndex);
                if (nClass == CSWClass::CCLASS_JEDICONSULAR)
                {
                    int nOldForcePoints = pCreature->GetMaxForcePoints();
                    pBonusEffect = new CGameEffect(pEffect);
                    pBonusEffect->m_nType = EFFECT_ABILITY_INCREASE;
                    pBonusEffect->SetInteger(0, CSWRules::ABILITY_CHARISMA);
                    pBonusEffect->SetInteger(1, PURE_GOOD_ABILITY_BONUS);
                    pCreature->CSWSObject::ApplyEffect(pBonusEffect, 0, 0);
                    int nNewForcePoints = pCreature->GetMaxForcePoints();
                    pStats->m_nCurrentForcePoints += nNewForcePoints - nOldForcePoints;
                }
                else if (nClass == CSWClass::CCLASS_JEDIGUARDIAN)
                {
                    pBonusEffect = new CGameEffect(pEffect);
                    pBonusEffect->m_nType = EFFECT_ABILITY_INCREASE;
                    pBonusEffect->SetInteger(0, CSWRules::ABILITY_STRENGTH);
                    pBonusEffect->SetInteger(1, PURE_GOOD_ABILITY_BONUS);
                    pCreature->CSWSObject::ApplyEffect(pBonusEffect, 0, 0);
                }
                else if (nClass == CSWClass::CCLASS_JEDISENTINEL)
                {
                    short nOldHitPoints = pCreature->GetMaxHitPoints(0);
                    pBonusEffect = new CGameEffect(pEffect);
                    pBonusEffect->m_nType = EFFECT_ABILITY_INCREASE;
                    pBonusEffect->SetInteger(0, CSWRules::ABILITY_CONSTITUTION);
                    pBonusEffect->SetInteger(1, PURE_GOOD_ABILITY_BONUS);
                    pCreature->CSWSObject::ApplyEffect(pBonusEffect, 0, 0);
                    short nHitPointsGained = pCreature->GetMaxHitPoints(0) - nOldHitPoints;
                    pCreature->SetCurrentHitPoints(
                        pCreature->GetCurrentHitPoints(1) + nHitPointsGained);
                }
            }
        }
    }
    pBonusEffect = new CGameEffect(pEffect);
    pBonusEffect->m_nType = EFFECT_EFFECT_ICON;
    pBonusEffect->SetInteger(0, ICON_PURE_GOOD_POWERS);
    pCreature->CSWSObject::ApplyEffect(pBonusEffect, 0, 0);
    return 0;
}
// PC 0x00532BC0
int CSWSEffectListHandler::OnApplyPureEvilPowers(CSWSObject* pObject,
                                                CGameEffect* pEffect,
                                                int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;
    CSWSPlayer* pPlayer;
    CGameEffect* pBonusEffect;
    unsigned char nClassIndex;
    unsigned char nClass;

    pCreature = pObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 1;
    }
    pStats = pCreature->m_pStats;
    if (pStats == NULL)
    {
        return 1;
    }
    unsigned long oidCreature = pCreature->m_nId;
    pPlayer = g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(
        oidCreature);
    if (pPlayer != NULL)
    {
        unsigned long oidControlled = pPlayer->m_oidPCObject;
        if (oidControlled == pCreature->m_nId)
        {
            for (nClassIndex = 0; nClassIndex < pStats->m_nClassCount; nClassIndex++)
            {
                nClass = pStats->GetClass(nClassIndex);
                if (nClass == CSWClass::CCLASS_JEDICONSULAR)
                {
                    pStats->m_nCurrentForcePoints += PURE_EVIL_FORCE_POINT_BONUS;
                }
                else if (nClass == CSWClass::CCLASS_JEDIGUARDIAN)
                {
                    pBonusEffect = new CGameEffect(pEffect);
                    pBonusEffect->m_nType = EFFECT_DAMAGE_INCREASE;
                    pBonusEffect->SetInteger(0, PURE_EVIL_DAMAGE_BONUS);
                    pBonusEffect->SetInteger(1, CSWSCreatureStats::ALIGNMENT_EVIL);
                    pBonusEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
                    pBonusEffect->SetInteger(5, DAMAGE_EFFECT_WEAPON_FIRST);
                    pCreature->CSWSObject::ApplyEffect(pBonusEffect, 0, 0);
                    pBonusEffect = new CGameEffect(pEffect);
                    pBonusEffect->m_nType = EFFECT_DAMAGE_INCREASE;
                    pBonusEffect->SetInteger(0, PURE_EVIL_DAMAGE_BONUS);
                    pBonusEffect->SetInteger(1, CSWSCreatureStats::ALIGNMENT_EVIL);
                    pBonusEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
                    pBonusEffect->SetInteger(5, DAMAGE_EFFECT_WEAPON_SECOND);
                    pCreature->CSWSObject::ApplyEffect(pBonusEffect, 0, 0);
                }
                else if (nClass == CSWClass::CCLASS_JEDISENTINEL)
                {
                    pBonusEffect = new CGameEffect(pEffect);
                    pBonusEffect->m_nType = EFFECT_IMMUNITY;
                    pBonusEffect->SetInteger(0, CSWSCreatureStats::IMMUNITY_TYPE_POISON);
                    pBonusEffect->SetInteger(1, (unsigned char)g_pRules->m_nNumRaces);
                    pCreature->CSWSObject::ApplyEffect(pBonusEffect, 0, 0);
                }
            }
        }
    }
    pBonusEffect = new CGameEffect(pEffect);
    pBonusEffect->m_nType = EFFECT_EFFECT_ICON;
    pBonusEffect->SetInteger(0, ICON_PURE_EVIL_POWERS);
    pCreature->CSWSObject::ApplyEffect(pBonusEffect, 0, 0);
    return 0;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveBeam(CSWSObject* pObject,
                                       CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveBonusSpellOfLevel(CSWSObject* pObject,
                                                    CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveConcealment(CSWSObject* pObject,
                                              CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveCurse(CSWSObject* pObject,
                                        CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveDamageShield(CSWSObject* pObject,
                                               CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveDarkness(CSWSObject* pObject,
                                           CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EA0
int CSWSEffectListHandler::OnRemoveDisarm(CSWSObject* pObject,
                                         CGameEffect* pEffect)
{
    return 1;
}
// PC 0x00532EB0
int CSWSEffectListHandler::IsEffectToIgnoreOnDeath(CGameEffect* pEffect)
{
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pRemoveFXOnDeath);
    C2DA* pRemoveFXOnDeath = p2DArrays->m_pRemoveFXOnDeath;
    int nEffectType;

    for (int nRow = 0; nRow < pRemoveFXOnDeath->GetNumRows(); nRow++)
    {
        if (pRemoveFXOnDeath->GetINTEntry(nRow, "EffectType", &nEffectType))
        {
            if (pEffect->GetEffectTypeScriptConstant() == nEffectType)
            {
                return 1;
            }
            if (nEffectType == EFFECT_TYPE_BEAM &&
                pEffect->m_nType == EFFECT_VISUALEFFECT &&
                CLoopingVisualEffect::GetIsBeam(
                    (unsigned short)pEffect->GetInteger(0)))
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x005342B0
int CSWSEffectListHandler::OnApplyHeal(CSWSObject* pObject,
                                      CGameEffect* pEffect,
                                      int bLoadingModule)
{
    CSWSCreature* pCreature;
    int nHeal;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            nHeal = pEffect->GetInteger(0);
            if (pEffect->GetInteger(1) == HEAL_FORCE_POINTS)
            {
                if (pCreature->m_pStats != NULL)
                {
                    int nCurrent = pCreature->m_pStats->GetCurrentForcePoints();
                    short nMaximum = pCreature->GetMaxForcePoints();
                    if (nCurrent + nHeal > nMaximum)
                    {
                        nHeal = nMaximum - nCurrent;
                    }
                    pCreature->GetStats()->m_nCurrentForcePoints =
                        nHeal + nCurrent;
                    CSWCCMessageData* pData = new CSWCCMessageData;
                    pData->SetObjectID(0, pCreature->GetId());
                    pData->SetInteger(0, nHeal);
                    pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_FORCE_POINTS_REGAINED, pData);
                }
            }
            else if (!pCreature->GetDead() && !pCreature->GetDeadTemp())
            {
                int bWasDying = pCreature->GetIsPCDying();
                if (pCreature->GetCurrentHitPoints(1) + nHeal >
                    pCreature->GetMaxHitPoints(1))
                {
                    nHeal = pCreature->GetMaxHitPoints(1) -
                            pCreature->GetCurrentHitPoints(1);
                }
                CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                unsigned long oidClient =
                    pClient->ServerToClientObjectId(pCreature->GetId());
                if (pClient->GetLastTarget() == oidClient ||
                    pClient->GetSWParty()->GetIndex(oidClient) != -1)
                {
                    pClient->AddFloatyText(oidClient, 1, nHeal);
                }
                pCreature->SetCurrentHitPoints(
                    pCreature->GetCurrentHitPoints(1) + nHeal);
                CSWCCMessageData* pData = new CSWCCMessageData;
                CSWCCMessageData* pCreatorData = new CSWCCMessageData;
                pData->SetObjectID(0, pCreature->GetId());
                pData->SetInteger(0, nHeal);
                pData->CopyTo(pCreatorData);
                pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_HEALED, pData);
                unsigned long oidCreator = pEffect->m_oidCreator;
                CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()->
                    GetCreatureByGameObjectID(oidCreator);
                if (pCreator != NULL && pCreator != pCreature)
                {
                    pCreator->SendFeedbackMessage(CSWSCreature::FEEDBACK_HEALED, pCreatorData);
                }
                else
                {
                    delete pCreatorData;
                }
                int nIndex = 0;
                while (nIndex < pCreature->m_lstEffects.num &&
                       pCreature->m_lstEffects[nIndex]->GetEffectType() <=
                           EFFECT_WOUNDING)
                {
                    CGameEffect* pOther =
                        pCreature->m_lstEffects[nIndex];
                    if (pOther->GetEffectType() ==
                            EFFECT_WOUNDING &&
                        (pOther->m_nSubType & CGameEffect::DURATION_MASK) ==
                            CGameEffect::DURATION_TYPE_TEMPORARY)
                    {
                        unsigned __int64 nEffectId = pOther->m_nID;
                        pCreature->RemoveEffectById(nEffectId);
                        nIndex = 0;
                    }
                    else
                    {
                        nIndex++;
                    }
                }
                if (bWasDying == 1 && !pCreature->GetIsPCDying())
                {
                    CGameEffect* pRecovery = new CGameEffect(1);
                    pRecovery->m_nType = EFFECT_RESURRECTION;
                    pRecovery->m_nSubType =
                        (pRecovery->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                    pCreature->ApplyEffect(pRecovery, bLoadingModule, 0);
                }
            }
        }
    }
    return 1;
}
// PC 0x005345C0
int CSWSEffectListHandler::OnApplyDeath(CSWSObject* pObject,
                                      CGameEffect* pEffect,
                                      int bLoadingModule)
{
    unsigned long oidCreator = pEffect->GetCreator();
    if (pObject->m_bPlotObject)
        return 1;
    if (pObject->m_bMin1HP)
    {
        pObject->SetCurrentHitPoints(1);
        return 1;
    }

    CSWCObject* pClientObject = pObject->GetClientObject();
    if (pClientObject != NULL && pClientObject->AsSWCCreature() != NULL)
        pClientObject->AsSWCCreature()->SetLockOrientationToObject(OBJECT_INVALID, 1);
    if (pObject->AsSWSCreature() != NULL)
        pObject->AsSWSCreature()->SetLockOrientationToObject(OBJECT_INVALID, 1);
    pObject->ClearAllActions(1);
    CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()
                               ->GetCreatureByGameObjectID(oidCreator);

    if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature != NULL && pCreature->m_nAIStateFlags != 0)
        {
            g_pAppManager->GetClientExoApp()->SetLastPartyMemberTempKilled(
                pCreature->GetId());
            if ((pEffect->m_nSubType & CGameEffect::SUBTYPE_MASK) ==
                    SUBTYPE_MAGICAL &&
                pCreature->m_pStats->GetEffectImmunity(
                    CSWSCreatureStats::IMMUNITY_TYPE_DEATH, pCreator) &&
                pEffect->GetSpellId() != CGameEffect::SPELL_ID_INVALID)
            {
                CSWCCMessageData* pMessageData = new CSWCCMessageData;
                pMessageData->SetObjectID(0, pObject->GetId());
                if (pCreator != NULL)
                {
                    CSWCCMessageData* pCreatorMessageData = new CSWCCMessageData;
                    pMessageData->CopyTo(pCreatorMessageData);
                    pCreator->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_DEATH_IMMUNITY, pCreatorMessageData);
                }
                pCreature->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_DEATH_IMMUNITY, pMessageData);
                return 1;
            }
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                ->AdjustTargetAndWitnessReputations(pCreature->GetId(),
                    oidCreator, CServerAIMaster::REPUTATION_ACTION_DEATH);
            int bDestroy = pEffect->GetInteger(0);
            pEffect->GetInteger(1);
            pCreature->m_n154 = oidCreator;
            pCreature->ApplyDeathExperience();
            if (!pCreature->GetInParty())
            {
                unsigned long oidCreature = pCreature->GetId();
                CSWSFaction* pNeutral = g_pAppManager->GetServerExoApp()
                    ->GetFactionManager()->m_pNeutral;
                pNeutral->AddMember(oidCreature, 0);
            }
            if (!pCreature->m_nEC)
                bDestroy = 0;
            pCreature->m_pcCombatRound->EndCombatRound(0);
            pCreature->GetCombatRound()->m_oidUnknown9CC = OBJECT_INVALID;
            unsigned long oidScriptObject = pCreature->GetId();
            CExoString* psDeathScript =
                &pCreature->m_sScripts[CSWSCreature::SCRIPT_DEATH];
            g_pVirtualMachine->RunScript(psDeathScript, oidScriptObject, 1);
            if (pCreature->m_nAmbientAnimationState != CSWSCreature::AMBIENT_STATE_3 &&
                pCreature->m_nAmbientAnimationState != CSWSCreature::AMBIENT_STATE_4 &&
                pCreature->m_nAmbientAnimationState != CSWSCreature::AMBIENT_STATE_14)
            {
                if (rand() % 2)
                {
                    pCreature->SetAmbientAnimationState(CSWSCreature::AMBIENT_STATE_3);
                    pCreature->SetAnimation(CSWSObject::ANIMATION_IDLE);
                }
                else
                {
                    pCreature->SetAmbientAnimationState(CSWSCreature::AMBIENT_STATE_4);
                    pCreature->SetAnimation(CSWSObject::ANIMATION_IDLE);
                }
            }
            pCreature->m_bListening = 0;
            pCreature->m_nAIStateFlags = 0;
            if (pCreature->GetCurrentHitPoints(1) > CSWSObject::DEAD_HIT_POINTS)
                pCreature->SetCurrentHitPoints(CSWSObject::DEAD_HIT_POINTS);
            pCreature->AutoCloseGUIPanels(0);
            pCreature->UpdateActionQueue();
            int nEffect = 0;
            while (nEffect < pCreature->m_lstEffects.num)
            {
                if ((pCreature->m_lstEffects[nEffect]->m_nSubType &
                         CGameEffect::DURATION_MASK) == CGameEffect::DURATION_TYPE_INNATE ||
                    (pCreature->m_lstEffects[nEffect]->m_nSubType &
                     CGameEffect::DURATION_MASK) == CGameEffect::DURATION_TYPE_EQUIPPED ||
                    IsEffectToIgnoreOnDeath(pCreature->m_lstEffects[nEffect]))
                    ++nEffect;
                else
                {
                    unsigned __int64 nID = pCreature->m_lstEffects[nEffect]->m_nID;
                    pCreature->RemoveEffectById(nID);
                    nEffect = 0;
                }
            }
            if (pCreature->GetCurrentHitPoints(1) > CSWSObject::DEAD_HIT_POINTS)
                pCreature->SetCurrentHitPoints(CSWSObject::DEAD_HIT_POINTS);
            float fDestroyObjectDelay;
            bool bDestroyDelayMissing;
            {
                CExoString sColumn("DestroyObjectDelay");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pAppearance = p2DArrays->m_pAppearance;
                bDestroyDelayMissing = !pAppearance->GetFLOATEntry(
                    pCreature->m_cAppearanceInfo.m_nAppearance, sColumn,
                    &fDestroyObjectDelay);
            }
            if (bDestroyDelayMissing)
                fDestroyObjectDelay = 3.0f;
            fDestroyObjectDelay *= 1000.0f;
            if (!pCreature->GetDeadTemp())
            {
                if (!bDestroy)
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                        ->AddEventDeltaTime(0, (unsigned long)fDestroyObjectDelay,
                            pEffect->GetCreator(), pObject->GetId(),
                            CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                else
                {
                    CGameEffect* pVisualEffect = new CGameEffect(1);
                    pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
                    pVisualEffect->m_nSubType =
                        (pVisualEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                    pVisualEffect->SetInteger(0, VFX_DEATH_DESTRUCTION);
                    pCreature->AsSWSCreature()->ApplyEffect(pVisualEffect, bLoadingModule, 0);
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                        ->AddEventDeltaTime(0, (unsigned long)fDestroyObjectDelay,
                            pEffect->GetCreator(), pObject->GetId(),
                            CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                    if (pCreature->m_oidSpawnedBy != OBJECT_INVALID)
                    {
                        CSWSEncounter* pEncounter = g_pAppManager->GetServerExoApp()
                            ->GetEncounterByGameObjectID(pCreature->m_oidSpawnedBy);
                        if (pEncounter != NULL)
                        {
                            pEncounter->RemoveFromActiveCreatureCount(
                                pCreature->GetStats()->GetChallengeRating(), 0);
                            pCreature->m_bEncounterCountRemoved = 1;
                        }
                    }
                }
            }
            else
            {
                unsigned long oidModule = g_pAppManager->GetServerExoApp()
                    ->GetModule()->GetId();
                CScriptEvent* pScriptEvent = new CScriptEvent;
                pScriptEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DEATH;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                    ->AddEventDeltaTime(0, 0, pCreature->GetId(), oidModule,
                        CServerAIMaster::EVENT_SIGNAL_EVENT, pScriptEvent);
            }
            pCreature->BroadcastVoiceChat(CSWSCreature::VOICE_CHAT_DEATH);
            CExoString sTimestamp;
            CExoString sName;
            g_pExoBase->m_pExoDebug->GetCurrentTimestamp(sTimestamp);
            sName = pCreature->m_pStats->GetFullName();
            if (pCreature->m_nAreaWaitState)
                sName = sName + "(PC)";
            if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bAutopausePartyKilled &&
                pCreature->m_bInParty)
            {
                int bLivingPartyMember = 0;
                for (int i = 0; i < g_pAppManager->GetClientExoApp()->GetSWParty()->m_nCharacters; ++i)
                {
                    CSWCCreature* pMember = g_pAppManager->GetClientExoApp()
                        ->GetSWParty()->GetCharacter(i);
                    if (pMember != NULL && pMember->GetServerCreature() != NULL)
                    {
                        if (!pMember->GetServerCreature()->GetDeadTemp())
                            bLivingPartyMember = 1;
                    }
                }
                if (bLivingPartyMember)
                    g_pAppManager->GetClientExoApp()->SetAutoPaused(1,
                        CClientExoApp::AUTOPAUSE_PARTY_MEMBER_KILLED);
            }
            if (g_pAppManager->GetServerExoApp()->GetPlayerCreatureId() == pCreature->GetId())
            {
                g_pAppManager->GetClientExoApp()
                    ->ChangeCharacterToNextLivingPartyMember(1, 1, 1);
                g_pAppManager->GetClientExoApp()->GetInGameGui()->RemoveGUIPopUps();
            }

            EXOLOG("%s Died [%s]\n", sName.CStr(), sTimestamp.CStr());
        }
        EXOLOG("%x is now dead (%s)\n", pObject->GetId(),
               rand() & 1 ? "as a doornail" : "definitely deceased");
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE ||
             pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            if (pObject->AsSWSPlaceable() != NULL)
                pObject->AsSWSPlaceable()->ClosePlaceableForAllPlayers();
            pObject->SetAnimation(DOOR_ANIMATION_DESTROYED);
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            pObject->AsSWSDoor()->SetOpenState(CSWSDoor::OPEN_STATE_DESTROYED, 1);
            CSWSObject* pLinkedObject = pObject->AsSWSDoor()->GetLinkedObject();
            if (pLinkedObject != NULL && pLinkedObject->AsSWSDoor() != NULL &&
                pLinkedObject->m_nAnimation != DOOR_ANIMATION_DESTROYED)
            {
                CGameEffect* pLinkedDeath = new CGameEffect(pEffect);
                pLinkedDeath->m_nType = EFFECT_DEATH;
                pLinkedDeath->SetInteger(0, 0);
                pLinkedDeath->SetInteger(1, 1);
                pLinkedObject->ApplyEffect(pLinkedDeath, bLoadingModule, 0);
            }
        }
        CScriptEvent* pScriptEvent = new CScriptEvent;
        pScriptEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DEATH;
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()
            ->AddEventDeltaTime(0, 0, pEffect->GetCreator(), pObject->GetId(),
                CServerAIMaster::EVENT_SIGNAL_EVENT, pScriptEvent);
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()
            ->AddEventDeltaTime(0, 2000, pEffect->GetCreator(), pObject->GetId(),
                CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
    }
    return 1;
}
// PC 0x00534EE0
int CSWSEffectListHandler::OnApplyResurrection(CSWSObject* pObject,
                                               CGameEffect* pEffect,
                                               int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSPlayer* pPlayer;
    CGameEffect* pAppliedEffect;
    int nIndex;

    if (pObject->m_nF0 == 0)
    {
        return 1;
    }
    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pCreature->GetCurrentHitPoints(0) < 1)
        {
            pCreature->SetCurrentHitPoints(1);
        }
        pCreature->ClearAllActions(1);
        pCreature->m_bListening = 1;
        pCreature->m_nAIStateFlags = CSWSCreature::AI_STATE_ALL;
        pCreature->m_bCommandable = 1;
        pCreature->RecomputeAmbientAnimationState();

        unsigned long oidObject = pObject->m_nId;
        pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(
            oidObject);
        if (pPlayer != NULL)
        {
            CSWSMessage* pMessage =
                g_pAppManager->GetServerExoApp()->GetSWSMessage();
            pMessage->SendServerToPlayerDestroyDeathGUI(pPlayer->m_nPlayerId);
        }

        for (nIndex = 0; nIndex < pCreature->m_lstEffects.num &&
                         pCreature->m_lstEffects[nIndex]->m_nType <=
                             EFFECT_HITPOINTCHANGEWHENDYING;
             nIndex++)
        {
            pAppliedEffect = pCreature->m_lstEffects[nIndex];
            if (pAppliedEffect->m_nType == EFFECT_HITPOINTCHANGEWHENDYING &&
                (pAppliedEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                    CGameEffect::DURATION_TYPE_TEMPORARY)
            {
                pCreature->RemoveEffectById(pAppliedEffect->m_nID);
            }
        }
    }
    return 1;
}
// PC 0x00535020
int CSWSEffectListHandler::OnRemoveACIncrease(CSWSObject* pObject,
                                              CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CExoArrayList<CGameEffect*>* pEffects;
    int nType;
    int nBonus;
    int nRace;
    int nAlignmentGroup1;
    int nAlignmentGroup2;
    int nIndex;
    int nMaximum;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            nType = pEffect->GetInteger(AC_INTEGER_TYPE);
            nBonus = pEffect->GetInteger(AC_INTEGER_BONUS);
            nRace = pEffect->GetInteger(AC_INTEGER_RACIAL_GROUP);
            nAlignmentGroup1 = pEffect->GetInteger(AC_INTEGER_ALIGNMENT_GROUP1);
            nAlignmentGroup2 = pEffect->GetInteger(AC_INTEGER_ALIGNMENT_GROUP2);
            pEffects = &pCreature->m_lstEffects;
            nIndex = pCreature->m_pStats->m_nACEffectIndex;
            if (nRace == (unsigned char)g_pRules->m_nNumRaces &&
                nAlignmentGroup1 == 0 && nAlignmentGroup2 == 0)
            {
                if (nType == AC_DODGE_BONUS)
                {
                    pCreature->m_pStats->m_nACDodgeBonus -= nBonus;
                }
                else
                {
                    nMaximum = 0;
                    for (; nIndex < pEffects->num &&
                           (*pEffects)[nIndex]->GetEffectType() ==
                               EFFECT_AC_INCREASE; nIndex++)
                    {
                        CGameEffect* pOther = (*pEffects)[nIndex];
                        if (pOther != pEffect &&
                            pOther->GetInteger(
                                AC_INTEGER_TYPE) == nType)
                        {
                            unsigned char nNumRaces = g_pRules->m_nNumRaces;
                            if (pOther->GetInteger(
                                    AC_INTEGER_RACIAL_GROUP) == nNumRaces &&
                                pOther->GetInteger(
                                    AC_INTEGER_ALIGNMENT_GROUP1) == 0 &&
                                pOther->GetInteger(
                                    AC_INTEGER_ALIGNMENT_GROUP2) == 0 &&
                                pOther->GetInteger(
                                    AC_INTEGER_BONUS) > nMaximum)
                            {
                                nMaximum = pOther->GetInteger(
                                    AC_INTEGER_BONUS);
                            }
                        }
                    }
                    switch (nType)
                    {
                    case AC_NATURAL_BONUS:
                        pCreature->m_pStats->m_nACNaturalBonus = nMaximum;
                        break;
                    case AC_DEFLECTION_BONUS:
                        pCreature->m_pStats->m_nACDeflectionBonus = nMaximum;
                        break;
                    case AC_ARMOUR_ENCHANTMENT_BONUS:
                        pCreature->m_pStats->m_nACArmorBonus = nMaximum;
                        break;
                    case AC_SHIELD_ENCHANTMENT_BONUS:
                        pCreature->m_pStats->m_nACShieldBonus = nMaximum;
                        break;
                    }
                }
            }
        }
    }
    return 1;
}
// PC 0x00535260
int CSWSEffectListHandler::OnRemoveACDecrease(CSWSObject* pObject,
                                              CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CExoArrayList<CGameEffect*>* pEffects;
    int nType;
    int nPenalty;
    int nRace;
    int nAlignmentGroup1;
    int nAlignmentGroup2;
    int nIndex;
    int nMaximum;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            nType = pEffect->GetInteger(AC_INTEGER_TYPE);
            nPenalty = pEffect->GetInteger(AC_INTEGER_BONUS);
            nRace = pEffect->GetInteger(AC_INTEGER_RACIAL_GROUP);
            nAlignmentGroup1 = pEffect->GetInteger(AC_INTEGER_ALIGNMENT_GROUP1);
            nAlignmentGroup2 = pEffect->GetInteger(AC_INTEGER_ALIGNMENT_GROUP2);
            pEffects = &pCreature->m_lstEffects;
            nIndex = pCreature->m_pStats->m_nACEffectIndex;
            if (nRace == (unsigned char)g_pRules->m_nNumRaces &&
                nAlignmentGroup1 == 0 && nAlignmentGroup2 == 0)
            {
                if (nType == AC_DODGE_BONUS)
                {
                    pCreature->m_pStats->m_nACDodgePenalty -= nPenalty;
                }
                else
                {
                    nMaximum = 0;
                    for (; nIndex < pEffects->num &&
                           (*pEffects)[nIndex]->GetEffectType() ==
                               EFFECT_AC_DECREASE; nIndex++)
                    {
                        if ((*pEffects)[nIndex] != pEffect &&
                            (*pEffects)[nIndex]->GetInteger(
                                AC_INTEGER_TYPE) == nType)
                        {
                            unsigned char nNumRaces = g_pRules->m_nNumRaces;
                            if ((*pEffects)[nIndex]->GetInteger(
                                    AC_INTEGER_RACIAL_GROUP) == nNumRaces &&
                                (*pEffects)[nIndex]->GetInteger(
                                    AC_INTEGER_BONUS) > nMaximum)
                            {
                                nMaximum = (*pEffects)[nIndex]->GetInteger(
                                    AC_INTEGER_BONUS);
                            }
                        }
                    }
                    switch (nType)
                    {
                    case AC_NATURAL_BONUS:
                        pCreature->m_pStats->m_nACNaturalPenalty = nMaximum;
                        break;
                    case AC_ARMOUR_ENCHANTMENT_BONUS:
                        pCreature->m_pStats->m_nACArmorPenalty = nMaximum;
                        break;
                    case AC_SHIELD_ENCHANTMENT_BONUS:
                        pCreature->m_pStats->m_nACShieldPenalty = nMaximum;
                        break;
                    case AC_DEFLECTION_BONUS:
                        pCreature->m_pStats->m_nACDeflectionPenalty = nMaximum;
                        break;
                    }
                }
            }
        }
    }
    return 1;
}
// PC 0x00535450
int CSWSEffectListHandler::OnRemoveDamageImmunityIncrease(CSWSObject* pObject,
                                                          CGameEffect* pEffect)
{
    int nDamageType;
    int nPercent;
    int nTotal;
    int nIndex;
    CGameEffect* pOther;

    if (pEffect != NULL)
    {
        nDamageType = pEffect->GetInteger(0);
        nPercent = pEffect->GetInteger(1);
        pObject->GetDamageImmunityByFlags(nDamageType);
        nTotal = 0;
        for (nIndex = 0; nIndex < pObject->m_lstEffects.num; nIndex++)
        {
            pOther = pObject->m_lstEffects[nIndex];
            if (pOther->m_nType == EFFECT_DAMAGE_IMMUNITY_INCREASE && pOther->GetInteger(0) == nDamageType ||
                pOther->m_nType == EFFECT_DAMAGE_IMMUNITY_DECREASE && pOther->GetInteger(0) == nDamageType)
            {
                if (pOther->m_nType == EFFECT_DAMAGE_IMMUNITY_INCREASE)
                {
                    nTotal += pOther->GetInteger(1);
                }
                else
                {
                    nTotal -= pOther->GetInteger(1);
                }
            }
        }
        pObject->SetDamageImmunity(nDamageType, nTotal - nPercent);
    }
    return 1;
}
// PC 0x00535520
int CSWSEffectListHandler::OnRemoveDamageImmunityDecrease(CSWSObject* pObject,
                                                          CGameEffect* pEffect)
{
    int nDamageType;
    int nPercent;
    int nTotal;
    int nIndex;
    CGameEffect* pOther;

    if (pEffect != NULL)
    {
        nDamageType = pEffect->GetInteger(0);
        nPercent = pEffect->GetInteger(1);
        pObject->GetDamageImmunity(nDamageType);
        nTotal = 0;
        for (nIndex = 0; nIndex < pObject->m_lstEffects.num; nIndex++)
        {
            pOther = pObject->m_lstEffects[nIndex];
            if (pOther->m_nType == EFFECT_DAMAGE_IMMUNITY_INCREASE && pOther->GetInteger(0) == nDamageType ||
                pOther->m_nType == EFFECT_DAMAGE_IMMUNITY_DECREASE && pOther->GetInteger(0) == nDamageType)
            {
                if (pOther->m_nType == EFFECT_DAMAGE_IMMUNITY_INCREASE)
                {
                    nTotal += pOther->GetInteger(1);
                }
                else
                {
                    nTotal -= pOther->GetInteger(1);
                }
            }
        }
        pObject->SetDamageImmunity(nDamageType, nTotal - nPercent);
    }
    return 1;
}
// PC 0x005355F0
int CSWSEffectListHandler::OnRemoveSetAIState(CSWSObject* pObject,
                                             CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    unsigned short nAIStateFlags;
    int nIndex;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pCreature->m_nAIStateFlags != 0)
        {
            nAIStateFlags = CSWSCreature::AI_STATE_ALL_FLAGS;
            if (pCreature->m_lstEffects.num > 0)
            {
                for (nIndex = pCreature->m_pStats->m_nSetAIStateEffectIndex;
                     nIndex < pCreature->m_lstEffects.num &&
                     pCreature->m_lstEffects[nIndex]->m_nType ==
                         EFFECT_SET_AI_STATE;
                     nIndex++)
                {
                    pOther = pCreature->m_lstEffects[nIndex];
                    if (pOther->m_nType == EFFECT_SET_AI_STATE &&
                        pOther != pEffect)
                    {
                        nAIStateFlags &= pOther->GetInteger(0);
                    }
                }
            }
            pCreature->m_nAIStateFlags = nAIStateFlags;
        }
    }
    return 1;
}
// PC 0x00535680
int CSWSEffectListHandler::OnRemoveEnemyAttackBonus(CSWSObject* pObject,
                                                    CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    int nIndex;
    int nBonus;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        nIndex = 0;
        nBonus = 0;
        if (pEffect != NULL && pCreature->m_lstEffects.num > 0)
        {
            do
            {
                pOther = pCreature->m_lstEffects[nIndex];
                if (pOther->m_nType > EFFECT_ENEMY_ATTACK_BONUS)
                {
                    break;
                }
                if (pOther->m_nType == EFFECT_ENEMY_ATTACK_BONUS && pOther != pEffect)
                {
                    nBonus += pOther->GetInteger(0);
                }
                nIndex++;
            } while (nIndex < pCreature->m_lstEffects.num);
        }
        pCreature->m_pStats->m_nEnemyAttackBonus = nBonus;
    }
    return 1;
}
// PC 0x00535700
int CSWSEffectListHandler::OnRemoveArcaneSpellFailure(CSWSObject* pObject,
                                                      CGameEffect* pEffect)
{
    int nIndex;
    int nTotal;
    CSWSCreature* pCreature;
    CGameEffect* pOther;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        nIndex = 0;
        nTotal = 0;
        if (pEffect != NULL && pCreature->m_lstEffects.num > 0)
        {
            while (nIndex < pCreature->m_lstEffects.num)
            {
                pOther = pCreature->m_lstEffects[nIndex];
                if (pOther->m_nType > EFFECT_ARCANE_SPELL_FAILURE)
                {
                    break;
                }
                if (pOther->m_nType == EFFECT_ARCANE_SPELL_FAILURE && pOther != pEffect)
                {
                    nTotal += pOther->GetInteger(0);
                }
                nIndex++;
            }
        }
        if (pObject->AsSWSCreature() != NULL)
        {
            pObject->AsSWSCreature()->m_nUnknown344 = 1;
        }
        pCreature->m_pStats->SetArcaneSpellFailure(nTotal);
    }
    return 1;
}
// PC 0x005357A0
int CSWSEffectListHandler::OnApplySetState(CSWSObject* pObject,
                                          CGameEffect* pEffect,
                                          int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pCreator;
    CSWSCreatureStats* pStats;
    CSWSCombatAttackData* pAttack;
    CSWCCMessageData* pMessageData;
    CSWCCMessageData* pCreatorMessageData;
    CGameEffect* pAIStateEffect;
    CGameEffect* pStateEffect;
    int bIgnoreImmunity;
    int nState;
    int nIndex;
    unsigned __int64 nEffectId;

    pCreature = pObject->AsSWSCreature();
    pAttack = NULL;
    if (pCreature != NULL)
    {
        pStats = pCreature->m_pStats;
        if (pEffect != NULL)
        {
            bIgnoreImmunity = pEffect->GetInteger(1);
            if (bIgnoreImmunity == 0 && pStats != NULL &&
                pStats->GetEffectLinkImmunity(pEffect) != 0)
            {
                return 1;
            }
            pCreator = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID(pEffect->GetCreator());
            nState = pEffect->GetInteger(0);
            if (pCreator != NULL && pCreator->m_pcCombatRound != NULL)
            {
                pAttack = pCreator->m_pcCombatRound->GetAttack(
                    pCreator->m_pcCombatRound->m_nCurrentAttack);
                if (pAttack != NULL)
                {
                    pAttack->m_Extra4.m_nState = (unsigned char)nState;
                }
            }
            if (nState == STATE_SLEEP)
            {
                if (pStats->GetEffectImmunity(
                    CSWSCreatureStats::IMMUNITY_TYPE_SLEEP, pCreator) == 1)
                {
                    pMessageData = new CSWCCMessageData;
                    pMessageData->SetObjectID(0, pObject->GetId());
                    if (pCreator != NULL)
                    {
                        pCreatorMessageData = new CSWCCMessageData;
                        pMessageData->CopyTo(pCreatorMessageData);
                        pCreator->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_SLEEP_IMMUNITY, pCreatorMessageData);
                        if (pAttack != NULL)
                        {
                            pAttack->m_Extra4.m_bStateImmune = 1;
                        }
                    }
                    pObject->AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_SLEEP_IMMUNITY, pMessageData);
                    return 1;
                }
            }
            else if (nState == STATE_HOLD)
            {
                if (bIgnoreImmunity == 0 &&
                    (pStats->GetEffectImmunity(
                         CSWSCreatureStats::IMMUNITY_TYPE_PARALYSIS, pCreator) != 0 ||
                     pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_PARALYSIS) != 0))
                {
                    pMessageData = new CSWCCMessageData;
                    pMessageData->SetObjectID(0, pObject->GetId());
                    if (pCreator != NULL)
                    {
                        pCreatorMessageData = new CSWCCMessageData;
                        pMessageData->CopyTo(pCreatorMessageData);
                        pCreator->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_PARALYSIS_IMMUNITY, pCreatorMessageData);
                        if (pAttack != NULL)
                        {
                            pAttack->m_Extra4.m_bStateImmune = 1;
                        }
                    }
                    pObject->AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_PARALYSIS_IMMUNITY, pMessageData);
                    return 1;
                }
            }
            else if (nState == STATE_STUN)
            {
                if (bIgnoreImmunity == 0 &&
                    (pStats->GetEffectImmunity(
                         CSWSCreatureStats::IMMUNITY_TYPE_STUN, pCreator) != 0 ||
                     pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_STUN) != 0 ||
                     pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_PARALYSIS) != 0))
                {
                    pMessageData = new CSWCCMessageData;
                    pMessageData->SetObjectID(0, pObject->GetId());
                    if (pCreator != NULL)
                    {
                        pCreatorMessageData = new CSWCCMessageData;
                        pMessageData->CopyTo(pCreatorMessageData);
                        pCreator->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_STUN_IMMUNITY, pCreatorMessageData);
                        if (pAttack != NULL)
                        {
                            pAttack->m_Extra4.m_bStateImmune = 1;
                        }
                    }
                    pObject->AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_STUN_IMMUNITY, pMessageData);
                    return 1;
                }
            }
            else if (nState == STATE_CONFUSION)
            {
                if (pStats->GetEffectImmunity(
                    CSWSCreatureStats::IMMUNITY_TYPE_CONFUSED, pCreator) == 1)
                {
                    pMessageData = new CSWCCMessageData;
                    pMessageData->SetObjectID(0, pObject->GetId());
                    if (pCreator != NULL)
                    {
                        pCreatorMessageData = new CSWCCMessageData;
                        pMessageData->CopyTo(pCreatorMessageData);
                        pCreator->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_CONFUSED_IMMUNITY, pCreatorMessageData);
                        if (pAttack != NULL)
                        {
                            pAttack->m_Extra4.m_bStateImmune = 1;
                        }
                    }
                    pObject->AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_CONFUSED_IMMUNITY, pMessageData);
                    return 1;
                }
            }
            else if (nState == STATE_FEAR)
            {
                if (pStats->GetEffectImmunity(
                    CSWSCreatureStats::IMMUNITY_TYPE_FEAR, pCreator) != 0)
                {
                    pMessageData = new CSWCCMessageData;
                    pMessageData->SetObjectID(0, pObject->GetId());
                    if (pCreator != NULL)
                    {
                        pCreatorMessageData = new CSWCCMessageData;
                        pMessageData->CopyTo(pCreatorMessageData);
                        pCreator->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_FEAR_IMMUNITY, pCreatorMessageData);
                        if (pAttack != NULL)
                        {
                            pAttack->m_Extra4.m_bStateImmune = 1;
                        }
                    }
                    pObject->AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_FEAR_IMMUNITY, pMessageData);
                    return 1;
                }
            }
            else if (nState == STATE_HORRIFIED)
            {
                if (bIgnoreImmunity == 0 &&
                    pStats->HasFeat(CSWFeat::FEAT_FORCE_IMMUNITY_FEAR) != 0)
                {
                    pMessageData = new CSWCCMessageData;
                    pMessageData->SetObjectID(0, pObject->GetId());
                    if (pCreator != NULL)
                    {
                        pCreatorMessageData = new CSWCCMessageData;
                        pMessageData->CopyTo(pCreatorMessageData);
                        pCreator->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_HORRIFIED_IMMUNITY, pCreatorMessageData);
                        if (pAttack != NULL)
                        {
                            pAttack->m_Extra4.m_bStateImmune = 1;
                        }
                    }
                    pObject->AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_HORRIFIED_IMMUNITY, pMessageData);
                    return 1;
                }
            }
            pAIStateEffect = new CGameEffect(pEffect);
            pAIStateEffect->m_nType = EFFECT_SET_AI_STATE;
            pAIStateEffect->SetInteger(0, SETSTATE_AI_STATE_MASK);
            pCreature->ApplyEffect(pAIStateEffect, bLoadingModule, 0);
            if (nState > pCreature->m_bDebilitated)
            {
                pCreature->m_bDebilitated = (unsigned char)nState;
                for (nIndex = 0; nIndex < pObject->m_lstEffects.num; nIndex++)
                {
                    if (pObject->m_lstEffects[nIndex]->m_nType >
                        EFFECT_SETSTATE_INTERNAL)
                    {
                        break;
                    }
                    if (pObject->m_lstEffects[nIndex]->m_nType ==
                        EFFECT_SETSTATE_INTERNAL)
                    {
                        nEffectId = pObject->m_lstEffects[nIndex]->m_nID;
                        pCreature->RemoveEffectById(nEffectId);
                    }
                }
                pStateEffect = new CGameEffect(1);
                pStateEffect->SetCreator(pEffect->GetCreator());
                pStateEffect->m_nSubType =
                    (pStateEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INNATE;
                pStateEffect->m_bExposed = 0;
                pStateEffect->m_nType = EFFECT_SETSTATE_INTERNAL;
                pStateEffect->m_nSpellId = pEffect->m_nSpellId;
                pStateEffect->SetInteger(0, nState);
                pCreature->ApplyEffect(pStateEffect, bLoadingModule, 0);
            }
        }
    }
    return 0;
}
// PC 0x005360E0
int CSWSEffectListHandler::OnRemoveSetState(CSWSObject* pObject,
                                            CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pStateEffect;
    unsigned long oidCreator;
    int nState;
    int nIndex;
    unsigned __int64 nEffectId;

    pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        pObject->m_bCommandable = 1;
        pCreature->m_bDebilitated = 0;
        if (pObject->m_lstEffects.num > 0)
        {
            nIndex = 0;
            nState = 0;
            oidCreator = OBJECT_INVALID;
            while (nIndex < pObject->m_lstEffects.num &&
                   pObject->m_lstEffects[nIndex]->m_nType <=
                       EFFECT_SETSTATE)
            {
                if (pObject->m_lstEffects[nIndex]->m_nType ==
                        EFFECT_SETSTATE &&
                    pObject->m_lstEffects[nIndex] != pEffect &&
                    pObject->m_lstEffects[nIndex]->GetInteger(0) > nState)
                {
                    nState = pObject->m_lstEffects[nIndex]->GetInteger(0);
                    oidCreator =
                        pObject->m_lstEffects[nIndex]->m_oidCreator;
                }
                nIndex++;
            }
            while (nIndex < pObject->m_lstEffects.num &&
                   pObject->m_lstEffects[nIndex]->m_nType <=
                       EFFECT_SETSTATE_INTERNAL)
            {
                if (pObject->m_lstEffects[nIndex]->m_nType ==
                    EFFECT_SETSTATE_INTERNAL)
                {
                    nEffectId = pObject->m_lstEffects[nIndex]->m_nID;
                    pCreature->RemoveEffectById(nEffectId);
                }
                else
                {
                    nIndex++;
                }
            }
            if (nState != 0)
            {
                pCreature->m_bDebilitated = (unsigned char)nState;
                pObject->m_bCommandable = 0;
                pStateEffect = new CGameEffect(1);
                pStateEffect->SetCreator(oidCreator);
                pStateEffect->m_nSubType =
                    (pStateEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_PERMANENT;
                pStateEffect->m_bExposed = 0;
                pStateEffect->m_nType = EFFECT_SETSTATE_INTERNAL;
                pStateEffect->SetInteger(0, nState);
                pCreature->ApplyEffect(pStateEffect, 0, 0);
            }
        }
    }
    return 1;
}
// PC 0x00536280
int CSWSEffectListHandler::OnRemoveMovementSpeedIncrease(CSWSObject* pObject,
                                                         CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    float fFactor;
    int nIndex;

    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            fFactor = 1.0f;
            pCreature = pObject->AsSWSCreature();
            for (nIndex = 0; nIndex < pCreature->m_lstEffects.num; nIndex++)
            {
                if (pCreature->m_lstEffects[nIndex]->m_nType > EFFECT_MOVEMENT_SPEED_DECREASE)
                {
                    break;
                }
                pOther = pCreature->m_lstEffects[nIndex];
                if (pOther->m_nID != pEffect->m_nID)
                {
                    if (pOther->m_nType == EFFECT_MOVEMENT_SPEED_INCREASE)
                    {
                        fFactor = pOther->GetInteger(0) * 0.01f + fFactor;
                    }
                    else if (pOther->m_nType == EFFECT_MOVEMENT_SPEED_DECREASE)
                    {
                        fFactor = fFactor - pOther->GetInteger(0) * 0.01f;
                    }
                }
            }
            pCreature->SetMovementRateFactor(fFactor);
        }
    }
    return 1;
}
// PC 0x00536280
int CSWSEffectListHandler::OnRemoveMovementSpeedDecrease(CSWSObject* pObject,
                                                         CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    float fFactor;
    int nIndex;

    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature();
        if (pEffect != NULL)
        {
            fFactor = 1.0f;
            pCreature = pObject->AsSWSCreature();
            for (nIndex = 0; nIndex < pCreature->m_lstEffects.num; nIndex++)
            {
                if (pCreature->m_lstEffects[nIndex]->m_nType >
                    EFFECT_MOVEMENT_SPEED_DECREASE)
                {
                    break;
                }
                pOther = pCreature->m_lstEffects[nIndex];
                if (pOther->m_nID != pEffect->m_nID)
                {
                    if (pOther->m_nType == EFFECT_MOVEMENT_SPEED_INCREASE)
                    {
                        fFactor = pOther->GetInteger(0) * 0.01f + fFactor;
                    }
                    else if (pOther->m_nType == EFFECT_MOVEMENT_SPEED_DECREASE)
                    {
                        fFactor = fFactor - pOther->GetInteger(0) * 0.01f;
                    }
                }
            }
            pCreature->SetMovementRateFactor(fFactor);
        }
    }
    return 1;
}
// PC 0x00536360
int CSWSEffectListHandler::OnRemoveForceResistanceIncrease(CSWSObject* pObject,
                                                           CGameEffect* pEffect)
{
    CSWSCreatureStats* pStats;
    CGameEffect* pOther;
    int nIndex;
    int nMaximum;
    int nPenalty;
    short nType;

    if (pObject->AsSWSCreature() != NULL)
    {
        pStats = pObject->AsSWSCreature()->m_pStats;
        nMaximum = 0;
        nIndex = 0;
        nPenalty = 0;
        for (; nIndex < pObject->m_lstEffects.num; nIndex++)
        {
            pOther = pObject->m_lstEffects[nIndex];
            if (pOther->m_nID != pEffect->m_nID)
            {
                nType = pOther->m_nType;
                if (nType == EFFECT_FORCE_RESISTANCE_INCREASE || nType == EFFECT_FORCE_RESISTANCE_DECREASE)
                {
                    if (pOther->m_nType == EFFECT_FORCE_RESISTANCE_DECREASE)
                    {
                        nPenalty = pOther->GetInteger(0);
                    }
                    else if (pOther->GetInteger(0) > nMaximum)
                    {
                        nMaximum =
                            pObject->m_lstEffects[nIndex]->GetInteger(
                                0);
                    }
                }
                else if (pOther->m_nType > EFFECT_FORCE_RESISTANCE_DECREASE)
                {
                    break;
                }
            }
        }
        if (pObject->AsSWSCreature() != NULL)
        {
            pObject->AsSWSCreature()->m_nUnknown344 = 1;
        }
        pStats->SetForceResistance(nMaximum - nPenalty);
    }
    return 1;
}
// PC 0x00536440
int CSWSEffectListHandler::OnRemoveForceResistanceDecrease(CSWSObject* pObject,
                                                           CGameEffect* pEffect)
{
    CSWSCreatureStats* pStats;
    CGameEffect* pOther;
    int nIndex;
    int nMaximum;
    short nType;

    if (pObject->AsSWSCreature() != NULL)
    {
        pStats = pObject->AsSWSCreature()->m_pStats;
        nMaximum = 0;
        for (nIndex = 0; nIndex < pObject->m_lstEffects.num; nIndex++)
        {
            pOther = pObject->m_lstEffects[nIndex];
            nType = pOther->m_nType;
            if (nType == EFFECT_FORCE_RESISTANCE_INCREASE || nType == EFFECT_FORCE_RESISTANCE_DECREASE)
            {
                if (pOther != pEffect)
                {
                    if (nType == EFFECT_FORCE_RESISTANCE_DECREASE)
                    {
                        pOther->GetInteger(0);
                    }
                    else if (pOther->GetInteger(0) > nMaximum)
                    {
                        nMaximum =
                            pObject->m_lstEffects[nIndex]->GetInteger(
                                0);
                    }
                }
            }
            else if (pOther->m_nType > EFFECT_FORCE_RESISTANCE_DECREASE)
            {
                break;
            }
        }
        if (pObject->AsSWSCreature() != NULL)
        {
            pObject->AsSWSCreature()->m_nUnknown344 = 1;
        }
        pStats->SetForceResistancePenalty(nMaximum);
    }
    return 1;
}
// PC 0x00536510
int CSWSEffectListHandler::OnApplyDisease(CSWSObject* pObject,
                                          CGameEffect* pEffect,
                                          int bLoadingModule)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        if (bLoadingModule == 0)
        {
            if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_INSTANTANEOUS)
            {
                pEffect->m_nSubType =
                    (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_PERMANENT;
            }
#ifdef _XBOX
            CTwoDimArrays* pArraysForDisease = g_pRules->Get2DArrays();
            EXOXBOXASSERT(pArraysForDisease->m_pDisease);
            C2DA* pDisease = pArraysForDisease->m_pDisease;
#else
            C2DA* pDisease = g_pRules->Get2DArrays()->m_pDisease;
#endif
            CWorldTimer* pWorldTimer = g_pAppManager->GetServerExoApp()->GetWorldTimer();
            int nDisease = pEffect->GetInteger(0);
            if (pObject->AsSWSCreature()->m_bPlotObject != 0)
            {
                return 1;
            }
            CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID(pEffect->GetCreator());
            CSWSCreatureStats* pStats = pObject->AsSWSCreature()->m_pStats;
            if (pStats->GetEffectImmunity(IMMUNITY_TYPE_DISEASE, pCreator) != 0)
            {
                CSWCCMessageData* pMessageData = new CSWCCMessageData;
                pMessageData->SetObjectID(0, pObject->GetId());
                if (pCreator != NULL)
                {
                    CSWCCMessageData* pCreatorMessageData = new CSWCCMessageData;
                    pMessageData->CopyTo(pCreatorMessageData);
                    pCreator->SendFeedbackMessage(CSWSCreature::FEEDBACK_DISEASE_IMMUNITY,
                                                  pCreatorMessageData);
                }
                pObject->AsSWSCreature()->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_DISEASE_IMMUNITY, pMessageData);
                return 1;
            }
            for (int nIndex = 0; nIndex < pObject->m_lstEffects.num; nIndex++)
            {
                if (pObject->m_lstEffects[nIndex]->GetEffectType() == EFFECT_DISEASE)
                {
                    return 1;
                }
            }

            int nFirstSave;
            pDisease->GetINTEntry(nDisease, "First_Save", &nFirstSave);
            if (pObject->AsSWSCreature()->SavingThrowRoll(CSWSCreature::SAVING_THROW_FORT,
                    (unsigned short)nFirstSave, CSWSCreature::SAVING_THROW_TYPE_DISEASE,
                    pEffect->GetCreator(), 1, 0, 0) != 0)
            {
                return 1;
            }
            int nName;
            pDisease->GetINTEntry(nDisease, "Name", &nName);
            if (pObject->AsSWSCreature() != NULL)
            {
                pObject->AsSWSCreature()->DisplayFloatyDataToSelf(0);
            }
            unsigned long nCalendarDay;
            unsigned long nTimeOfDay;
            pWorldTimer->GetWorldTime(&nCalendarDay, &nTimeOfDay);
            int nIncubationHours;
            pDisease->GetINTEntry(nDisease, "Incu_Hours", &nIncubationHours);
            unsigned long nIncubationMilliseconds =
                nIncubationHours * CWorldTimer::MILLISECONDS_IN_MINUTE *
                g_pAppManager->GetServerExoApp()->GetModule()->m_nMinutesPerHour;
            unsigned long nIncubationDay;
            unsigned long nIncubationTime;
            pWorldTimer->AddWorldTimes(nCalendarDay, nTimeOfDay, 0,
                nIncubationMilliseconds, &nIncubationDay, &nIncubationTime);
            pEffect->SetInteger(3, nIncubationDay);
            pEffect->SetInteger(4, nIncubationTime);
        }

        CGameEffect* pVisualEffect = new CGameEffect(pEffect);
        pVisualEffect->m_nType = EFFECT_VISUALEFFECT;
        pVisualEffect->SetInteger(0, VFX_DISEASE);
        pObject->ApplyEffect(pVisualEffect, bLoadingModule, 0);
        return 0;
    }
    return 1;
}
// PC 0x00536880
int CSWSEffectListHandler::OnApplyHasteOrSlow(CSWSObject* pObject,
                                              CGameEffect* pEffect,
                                              int bLoadingModule)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        int bHaste = pEffect->GetEffectType() == EFFECT_HASTE;
        unsigned long oidCreator = pEffect->m_oidCreator;
        CSWSCreature* pCreator =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                oidCreator);
        if (bHaste == 0)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            if (pStats->GetEffectImmunity(
                    CSWSCreatureStats::IMMUNITY_TYPE_SLOW, pCreator) != 0)
            {
                CSWCCMessageData* pMessageData = new CSWCCMessageData;
                pMessageData->SetObjectID(0, pObject->GetId());
                if (pCreator != NULL)
                {
                    CSWCCMessageData* pCreatorMessageData = new CSWCCMessageData;
                    pMessageData->CopyTo(pCreatorMessageData);
                    pCreator->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_SLOW_IMMUNITY, pCreatorMessageData);
                }
                pObject->AsSWSCreature()->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_SLOW_IMMUNITY, pMessageData);
                return 1;
            }
        }
        int nTotal = 0;
        int nNumEffects = pObject->m_lstEffects.num;
        int nIndex;
        for (nIndex = 0; nIndex < nNumEffects; nIndex++)
        {
            int nType = pObject->m_lstEffects[nIndex]->GetEffectType();
            if (nType == EFFECT_HASTE)
            {
                nTotal++;
            }
            else if (nType == EFFECT_SLOW)
            {
                nTotal--;
            }
            else if (nType > EFFECT_SLOW)
            {
                break;
            }
        }
        int nNewState = nTotal + (bHaste ? 1 : -1);
        nNewState = max(min(nNewState, 1), -1);
        nTotal = max(min(nTotal, 1), -1);
        if (nTotal != nNewState)
        {
            if (nTotal > 0)
            {
                for (nIndex = 0; nIndex < nNumEffects; nIndex++)
                {
                    if (pObject->m_lstEffects[nIndex]->GetEffectType() ==
                        EFFECT_HASTE_INTERNAL)
                    {
                        pObject->RemoveEffectById(
                            pObject->m_lstEffects[nIndex]->m_nID);
                        break;
                    }
                    else if (pObject->m_lstEffects[nIndex]->GetEffectType() >
                             EFFECT_HASTE_INTERNAL)
                    {
                        break;
                    }
                }
            }
            else if (nTotal < 0)
            {
                for (nIndex = 0; nIndex < nNumEffects; nIndex++)
                {
                    if (pObject->m_lstEffects[nIndex]->GetEffectType() ==
                        EFFECT_SLOW_INTERNAL)
                    {
                        pObject->RemoveEffectById(
                            pObject->m_lstEffects[nIndex]->m_nID);
                        break;
                    }
                    else if (pObject->m_lstEffects[nIndex]->GetEffectType() >
                             EFFECT_SLOW_INTERNAL)
                    {
                        break;
                    }
                }
            }
            else
            {
                CGameEffect* pInternalEffect = new CGameEffect(1);
                pInternalEffect->m_nSubType =
                    (pInternalEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INNATE;
                pInternalEffect->m_nSubType =
                    (pInternalEffect->m_nSubType &
                     (unsigned short)~SUBTYPE_SUPERNATURAL) |
                    SUBTYPE_MAGICAL;
                pInternalEffect->m_nSpellId = pEffect->m_nSpellId;
                pInternalEffect->m_bExposed = 0;
                pInternalEffect->m_nType = nNewState > 0 ?
                    EFFECT_HASTE_INTERNAL : EFFECT_SLOW_INTERNAL;
                pObject->ApplyEffect(pInternalEffect, bLoadingModule, 0);
            }
        }
    }
    return 0;
}
// PC 0x00536B40
int CSWSEffectListHandler::OnRemoveHasteOrSlow(CSWSObject* pObject,
                                            CGameEffect* pEffect)
{
    if (pObject->AsSWSCreature() != NULL)
    {
        pObject->AsSWSCreature()->m_bHasted = 0;
        pObject->AsSWSCreature()->m_bSlowed = 0;

        int nTotal = 0;
        int bHaste = pEffect->GetEffectType() == EFFECT_HASTE;
        int nNumEffects = pObject->m_lstEffects.num;
        int nIndex;
        for (nIndex = 0; nIndex < nNumEffects; nIndex++)
        {
            int nType = pObject->m_lstEffects[nIndex]->GetEffectType();
            if (nType == EFFECT_HASTE)
            {
                nTotal++;
            }
            else if (nType == EFFECT_SLOW)
            {
                nTotal--;
            }
            else if (nType > EFFECT_SLOW)
            {
                break;
            }
        }
        int nNewState = max(min(nTotal + (bHaste ? -1 : 1), 1), -1);
        nTotal = max(min(nTotal, 1), -1);
        if (nTotal != nNewState)
        {
            if (nTotal > 0)
            {
                for (nIndex = 0; nIndex < nNumEffects; nIndex++)
                {
                    if (pObject->m_lstEffects[nIndex]->GetEffectType() ==
                        EFFECT_HASTE_INTERNAL)
                    {
                        pObject->RemoveEffectById(
                            pObject->m_lstEffects[nIndex]->m_nID);
                        break;
                    }
                    else if (pObject->m_lstEffects[nIndex]->GetEffectType() >
                             EFFECT_HASTE_INTERNAL)
                    {
                        break;
                    }
                }
            }
            else if (nTotal < 0)
            {
                for (nIndex = 0; nIndex < nNumEffects; nIndex++)
                {
                    if (pObject->m_lstEffects[nIndex]->GetEffectType() ==
                        EFFECT_SLOW_INTERNAL)
                    {
                        pObject->RemoveEffectById(
                            pObject->m_lstEffects[nIndex]->m_nID);
                        break;
                    }
                    else if (pObject->m_lstEffects[nIndex]->GetEffectType() >
                             EFFECT_SLOW_INTERNAL)
                    {
                        break;
                    }
                }
            }
            else
            {
                CGameEffect* pInternalEffect = new CGameEffect(1);
                pInternalEffect->m_nSubType =
                    (pInternalEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                    CGameEffect::DURATION_TYPE_INNATE;
                pInternalEffect->m_nSubType =
                    (pInternalEffect->m_nSubType & (unsigned short)~SUBTYPE_SUPERNATURAL) |
                    SUBTYPE_MAGICAL;
                pInternalEffect->m_bExposed = 0;
                pInternalEffect->m_nType = nNewState > 0 ?
                    EFFECT_HASTE_INTERNAL : EFFECT_SLOW_INTERNAL;
                pObject->ApplyEffect(pInternalEffect, 0, 0);
            }
        }
    }
    return 1;
}
// PC 0x00536D30
int CSWSEffectListHandler::OnApplyVision(CSWSObject* pObject,
                                         CGameEffect* pEffect,
                                         int bLoadingModule)
{
    CGameEffect* pVisionEffect;

    if (pObject->AsSWSCreature() != NULL)
    {
        pEffect->GetInteger(0);
        pVisionEffect = new CGameEffect(pEffect);
        if (pVisionEffect != NULL)
        {
            pVisionEffect->m_nType = EFFECT_VISUALEFFECT;
            delete pVisionEffect;
        }
    }
    return 1;
}
// PC 0x00536DD0
int CSWSEffectListHandler::OnRemoveLimitMovementSpeed(CSWSObject* pObject,
                                                      CGameEffect* pEffect)
{
    CExoArrayList<CGameEffect*>* pList;
    CGameEffect* pOther;
    int nCount;
    int nIndex;
    int bLimited;

    if (pObject->AsSWSCreature() != NULL)
    {
        pList = &pObject->AsSWSCreature()->m_lstEffects;
        nCount = pList->num;
        bLimited = 0;
        if (nCount > 0)
        {
            for (nIndex = 0; nIndex < nCount; nIndex++)
            {
                pOther = (*pList)[nIndex];
                if (pOther->m_nType > EFFECT_LIMIT_MOVEMENT_SPEED)
                {
                    break;
                }
                if (pOther->m_nType == EFFECT_LIMIT_MOVEMENT_SPEED && pOther != pEffect)
                {
                    bLimited = 1;
                    break;
                }
            }
        }
        pObject->AsSWSCreature()->m_nUnknown8E8 = bLimited;
    }
    return 1;
}
// PC 0x00536E40
int CSWSEffectListHandler::OnRemoveSpecialWalkAnimation(CSWSObject* pObject,
                                                        CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CExoArrayList<CGameEffect*>* pList;
    CGameEffect* pOther;
    int nCount;
    int nIndex;
    char nAnimation;

    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pList = &pCreature->m_lstEffects;
        nAnimation = 0;
        if (pList != NULL)
        {
            nCount = pList->num;
            if (nCount > 0)
            {
                for (nIndex = 0; nIndex < nCount; nIndex++)
                {
                    pOther = (*pList)[nIndex];
                    if (pOther != pEffect)
                    {
                        if (pOther->m_nType == EFFECT_SETWALKANIMATION)
                        {
                            nAnimation = pOther->GetInteger(0);
                            break;
                        }
                        if (pOther->m_nType > EFFECT_SETWALKANIMATION)
                        {
                            break;
                        }
                    }
                }
            }
        }
        pCreature->m_nSpecialWalkAnimation = nAnimation;
    }
    return 1;
}
// PC 0x00536EC0
int CSWSEffectListHandler::OnRemoveEffectIcon(CSWSObject* pObject,
                                              CGameEffect* pEffect)
{
    CExoArrayList<CEffectIconObject*>* pIcons;
    CGameEffect* pOther;
    unsigned short nIndex;
    unsigned char nIcon;
    int bStillActive;

    if (pObject->AsSWSCreature() != NULL)
    {
        pIcons = &pObject->AsSWSCreature()->m_lstEffectIcons;
        unsigned short nFirstEffect =
            pObject->AsSWSCreature()->m_pStats->m_nEffectIconEffectIndex;
        nIcon = pEffect->GetInteger(0);
        bStillActive = FALSE;
        for (unsigned short nEffect = nFirstEffect;
             nEffect < pObject->m_lstEffects.num; nEffect++)
        {
            pOther = pObject->m_lstEffects[nEffect];
            if (pOther->m_nType != EFFECT_EFFECT_ICON)
            {
                break;
            }
            if (pOther != pEffect && pOther->GetInteger(0) == nIcon)
            {
                bStillActive = TRUE;
            }
        }
        for (nIndex = pIcons->num - 1; nIndex >= 0; nIndex--)
        {
            if ((*pIcons)[nIndex]->m_nIcon == nIcon)
            {
                break;
            }
        }
        if (bStillActive == FALSE)
        {
            delete (*pIcons)[nIndex];
            pIcons->DelIndex(nIndex);
        }
    }
    return 1;
}
// PC 0x00536FC0
int CSWSEffectListHandler::OnApplyDisguise(CSWSObject* pObject,
                                         CGameEffect* pEffect,
                                         int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 1;
    }
    if (!bLoadingModule && pCreature->GetDead())
    {
        return 1;
    }
    CExoArrayList<CGameEffect*>* pEffects = &pCreature->m_lstEffects;
    if (pEffects == NULL)
    {
        return 1;
    }
    int nNumEffects = pEffects->num;
    int nIndex;
    for (nIndex = 0; nIndex < nNumEffects; nIndex++)
    {
        if ((*pEffects)[nIndex]->m_nType == EFFECT_DISGUISE &&
            (*pEffects)[nIndex]->m_nID != pEffect->m_nID)
        {
            (*pEffects)[nIndex]->SetInteger(1, 1);
            pCreature->RemoveEffectById((*pEffects)[nIndex]->m_nID);
            break;
        }
    }
    int nAppearance = pEffect->GetInteger(0);
    pCreature->Disguise(nAppearance, pEffect, bLoadingModule);
    return 0;
}
// PC 0x005379D0
int CSWSEffectListHandler::OnApplyInvisibility(CSWSObject* pObject,
                                               CGameEffect* pGameEffect,
                                               int nValue)
{
    CSWSCreature* pCreature;
    CSWSModule* pModule;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pGameEffect != NULL)
        {
            pModule = g_pAppManager->m_pServerExoApp->GetModule();
            pModule->m_lstInvisibleCreatures.AddUnique(pCreature->m_nId);
        }
    }
    return 0;
}
// PC 0x00537A20
int CSWSEffectListHandler::OnRemoveInvisibility(CSWSObject* pObject,
                                                CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    CSWSModule* pModule;
    int bStillInvisible;
    int nIndex;
    unsigned long oidCreature;

    bStillInvisible = FALSE;
    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        for (nIndex = pCreature->m_pStats->m_nInvisibilityEffectIndex;
             nIndex < pCreature->m_lstEffects.num; nIndex++)
        {
            pOther = pCreature->m_lstEffects[nIndex];
            if (pOther->m_nType > EFFECT_SANCTUARY)
            {
                break;
            }
            if ((pOther->m_nType == EFFECT_INVISIBILITY ||
                 pOther->m_nType == EFFECT_SANCTUARY) &&
                pEffect->m_nID != pOther->m_nID)
            {
                bStillInvisible = TRUE;
            }
        }
        if (bStillInvisible == FALSE)
        {
            pModule = ServerExoAppLate2(g_pAppManager)->GetModule();
            oidCreature = pCreature->m_nId;
            if (pModule->m_lstInvisibleCreatures.Contains(oidCreature))
            {
                pModule->m_lstInvisibleCreatures.Remove(oidCreature);
            }
        }
    }
    return 1;
}
// PC 0x00537B00
int CSWSEffectListHandler::OnApplySanctuary(CSWSObject* pObject,
                                            CGameEffect* pEffect,
                                            int bLoadingModule)
{
    CSWSCreature* pCreature;
    CSWSCreature* pOther;
    CSWSArea* pArea;
    CSWSModule* pModule;
    CSWVisibilityNode* pNode;
    unsigned char nDC;
    int nObject;

    if (pObject->AsSWSCreature() != NULL)
    {
        CExoArrayList<unsigned long> lstCreatures;

        pCreature = pObject->AsSWSCreature();
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            nDC = pEffect->GetInteger(4);
            for (nObject = 0; nObject < pArea->m_lstObjectIds.num; nObject++)
            {
                pOther = g_pAppManager->GetServerExoApp()
                             ->GetCreatureByGameObjectID(
                                 pArea->m_lstObjectIds[nObject]);
                if (pOther != NULL && pOther->GetId() != pCreature->GetId())
                {
                    pNode = pOther->GetVisibleListElement(pCreature->GetId());
                    if (pNode != NULL)
                    {
                        if (pOther->SavingThrowRoll(
                                CSWSCreature::SAVING_THROW_WILL, nDC, 0,
                                pCreature->GetId(), 1, 0, 0) == 0)
                        {
                            pCreature->PacifyCreature(pOther->GetId());
                            pNode->m_nSanctuary = CSWVisibilityNode::SANCTUARY_SAVE_FAILED;
                        }
                        else
                        {
                            pNode->m_nSanctuary = CSWVisibilityNode::SANCTUARY_SAVE_SUCCEEDED;
                        }
                    }
                }
            }
            pModule = g_pAppManager->GetServerExoApp()->GetModule();
            pModule->m_lstInvisibleCreatures.AddUnique(pCreature->GetId());
        }
    }
    return 0;
}
// PC 0x00537C60
int CSWSEffectListHandler::OnRemoveSanctuary(CSWSObject* pObject,
                                           CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CSWSCreature* pOther;
    CSWSArea* pArea;
    CSWSModule* pModule;
    CSWVisibilityNode* pNode;
    CGameEffect* pOtherEffect;
    int nObject;
    int nIndex;
    int nNumEffects;
    unsigned char nSanctuary;
    int bStillInvisible;
    int bStillSanctuary;
    unsigned long oidCreature;

    if (pObject->AsSWSCreature() != NULL)
    {
        bStillInvisible = FALSE;
        bStillSanctuary = FALSE;
        pCreature = pObject->AsSWSCreature();
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            CExoArrayList<unsigned long> lstCreatures;
            for (nObject = 0; nObject < pArea->m_lstObjectIds.num; nObject++)
            {
                pOther = g_pAppManager->GetServerExoApp()
                             ->GetCreatureByGameObjectID(
                                 pArea->m_lstObjectIds[nObject]);
                if (pOther != NULL)
                {
                    pNode = pOther->GetVisibleListElement(pCreature->GetId());
                    if (pNode != NULL)
                    {
                        nSanctuary = pNode->m_nSanctuary;
                        if (nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_FAILED)
                        {
                            lstCreatures.Add(pOther->GetId());
                        }
                        else if (nSanctuary == CSWVisibilityNode::SANCTUARY_SAVE_SUCCEEDED)
                        {
                            pNode->m_nSanctuary = CSWVisibilityNode::SANCTUARY_SAVE_NOT_ROLLED;
                        }
                    }
                }
            }
            nIndex = pCreature->GetStats()->m_nInvisibilityEffectIndex;
            nNumEffects = pCreature->m_lstEffects.num;
            for (; nIndex < nNumEffects; nIndex++)
            {
                pOtherEffect = pCreature->m_lstEffects[nIndex];
                if (pOtherEffect->m_nType > EFFECT_SANCTUARY)
                {
                    break;
                }
                if (pOtherEffect->m_nType == EFFECT_INVISIBILITY ||
                    pOtherEffect->m_nType == EFFECT_SANCTUARY)
                {
                    unsigned __int64 nEffectID = pEffect->m_nID;
                    unsigned __int64 nOtherID = pOtherEffect->m_nID;
                    if (nEffectID != nOtherID)
                    {
                        bStillInvisible = TRUE;
                        if (pOtherEffect->m_nType == EFFECT_SANCTUARY)
                        {
                            bStillSanctuary = TRUE;
                        }
                    }
                }
            }
            if (bStillInvisible == FALSE)
            {
                pModule = g_pAppManager->GetServerExoApp()->GetModule();
                oidCreature = pCreature->GetId();
                if (pModule->m_lstInvisibleCreatures.Contains(oidCreature))
                {
                    pModule->m_lstInvisibleCreatures.Remove(oidCreature);
                }
            }
            if (bStillSanctuary == FALSE)
            {
                int nNumCreatures = lstCreatures.num;
                for (nObject = 0; nObject < nNumCreatures; nObject++)
                {
                    pOther = g_pAppManager->GetServerExoApp()
                                 ->GetCreatureByGameObjectID(
                                     lstCreatures[nObject]);
                    if (pOther != NULL)
                    {
                        pOther->RemoveFromVisibleList(pCreature->GetId());
                    }
                }
            }
        }
    }
    return 1;
}
// PC 0x00537ED0
int CSWSEffectListHandler::OnApplyDispelAllMagic(CSWSObject* pObject,
                                               CGameEffect* pEffect,
                                               int bLoadingModule)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    CExoArrayList<CGameEffect*>* pEffects = &pObject->m_lstEffects;
    CGameObject* pCreator;
    unsigned long oidCreator = pEffect->m_oidCreator;
    if (pObjectArray->GetGameObject(oidCreator, &pCreator) ==
            CGameObjectArray::SUCCESS &&
        pCreator != NULL && pCreator->AsSWSCreature() != NULL)
    {
        CSWSCreatureStats* pStats = pCreator->AsSWSCreature()->m_pStats;
        int nDispelLevel = pStats->GetLevel(0);
        CExoArrayList<unsigned __int64> lstDispelled;
        CSWCCMessageData* pMessageData = new CSWCCMessageData;
        pMessageData->SetObjectID(0, pObject->GetId());
        int nDispelled = 0;
        for (int i = 0; i < pEffects->num; i++)
        {
            CGameEffect* pAppliedEffect = (*pEffects)[i];
            if ((pAppliedEffect->m_nSubType & CGameEffect::SUBTYPE_MASK) ==
                    SUBTYPE_MAGICAL &&
                ((pAppliedEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                     CGameEffect::DURATION_TYPE_TEMPORARY ||
                 (pAppliedEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                     CGameEffect::DURATION_TYPE_PERMANENT))
            {
                unsigned __int64 nEffectId = pAppliedEffect->m_nID;
                unsigned long nSpellId = pAppliedEffect->m_nSpellId;
                if (!lstDispelled.Contains(nEffectId))
                {
                    CGameObject* pEffectCreator;
                    int nCasterLevel;
                    unsigned long oidEffectCreator = pAppliedEffect->m_oidCreator;
                    if (pObjectArray->GetGameObject(oidEffectCreator,
                                                   &pEffectCreator) ==
                            CGameObjectArray::SUCCESS &&
                        pEffectCreator->AsSWSCreature() != NULL)
                    {
                        pStats = pEffectCreator->AsSWSCreature()->m_pStats;
                        nCasterLevel = pStats->GetLevel(0);
                    }
                    else
                    {
                        nCasterLevel = 0;
                    }
                    if (g_pRules->RollDice(1, 20) + nDispelLevel >
                        nCasterLevel + DISPEL_DIFFICULTY_BASE)
                    {
                        unsigned long oidTarget = pObject->GetId();
                        unsigned long oidDispelCreator = pEffect->m_oidCreator;
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                            ->AddEventDeltaTime(0, 0, oidDispelCreator,
                                               oidTarget,
                                               CServerAIMaster::EVENT_REMOVE_EFFECT,
                                               pAppliedEffect);
                        if (pAppliedEffect->m_nSpellId != CGameEffect::SPELL_ID_INVALID)
                        {
                            pMessageData->SetInteger(++nDispelled, nSpellId);
                        }
                    }
                    lstDispelled.AddUnique(nEffectId);
                }
            }
        }
        pMessageData->SetInteger(0, nDispelled);
        if (nDispelled > 0)
        {
            CSWSClient* pClient = g_pAppManager->GetServerExoApp()
                                     ->GetClientObjectByObjectId(pObject->GetId());
            if (pClient != NULL)
            {
                g_pAppManager->GetServerExoApp()->GetSWSMessage()
                    ->SendServerToPlayerCCMessage(pClient->GetPlayerId(),
                                                 CSWSMessage::CC_MESSAGE_DISPEL_MAGIC,
                                                 pMessageData, NULL);
            }
            if (pCreator->GetId() != pObject->GetId())
            {
                pClient = g_pAppManager->GetServerExoApp()
                              ->GetClientObjectByObjectId(pCreator->GetId());
                if (pClient != NULL)
                {
                    g_pAppManager->GetServerExoApp()->GetSWSMessage()
                        ->SendServerToPlayerCCMessage(pClient->GetPlayerId(),
                                                     CSWSMessage::CC_MESSAGE_DISPEL_MAGIC,
                                                     pMessageData, NULL);
                }
            }
        }
        delete pMessageData;
    }
    return 1;
}
// PC 0x005381C0
int CSWSEffectListHandler::OnApplyDispelBestMagic(CSWSObject* pObject,
                                                CGameEffect* pEffect,
                                                int bLoadingModule)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    CExoArrayList<CGameEffect*>* pEffects = &pObject->m_lstEffects;
    CGameObject* pCreator;
    unsigned long oidCreator = pEffect->m_oidCreator;
    if (pObjectArray->GetGameObject(oidCreator, &pCreator) ==
            CGameObjectArray::SUCCESS &&
        pCreator != NULL && pCreator->AsSWSCreature() != NULL)
    {
        CSWSCreatureStats* pStats = pCreator->AsSWSCreature()->m_pStats;
        int nDispelLevel = pStats->GetLevel(0);
        CExoArrayList<unsigned __int64> lstResisted;
        int bDispelled = 0;
        int bFound;
        do
        {
            bFound = 0;
            int nBestEffect;
            for (int i = 0; i < pEffects->num && !bFound; i++)
            {
                CGameEffect* pAppliedEffect = (*pEffects)[i];
                if ((pAppliedEffect->m_nSubType & CGameEffect::SUBTYPE_MASK) ==
                        SUBTYPE_MAGICAL &&
                    ((pAppliedEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                         CGameEffect::DURATION_TYPE_TEMPORARY ||
                     (pAppliedEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                         CGameEffect::DURATION_TYPE_PERMANENT))
                {
                    unsigned __int64 nEffectId = pAppliedEffect->m_nID;
                    if (!lstResisted.Contains(nEffectId))
                    {
                        unsigned long oidEffectCreator = pAppliedEffect->m_oidCreator;
                        CSWSCreature* pEffectCreator = g_pAppManager
                            ->GetServerExoApp()->GetCreatureByGameObjectID(
                                oidEffectCreator);
                        if (pEffectCreator != NULL)
                        {
                            pStats = pEffectCreator->m_pStats;
                            if (pStats->GetLevel(0) > 0)
                            {
                                nBestEffect = i;
                                bFound = 1;
                            }
                        }
                        else
                        {
                            nBestEffect = i;
                            bFound = 1;
                        }
                    }
                }
            }
            if (bFound)
            {
                CGameEffect* pBestEffect = (*pEffects)[nBestEffect];
                if (g_pRules->RollDice(1, 20) + nDispelLevel >
                    DISPEL_DIFFICULTY_BASE)
                {
                    if (pBestEffect->m_nSpellId != CGameEffect::SPELL_ID_INVALID)
                    {
                        CSWCCMessageData* pMessageData = new CSWCCMessageData;
                        pMessageData->SetObjectID(0, pObject->GetId());
                        pMessageData->SetInteger(0, 1);
                        unsigned long nSpellId = pBestEffect->m_nSpellId;
                        pMessageData->SetInteger(1, nSpellId);
                        CSWSClient* pClient = g_pAppManager->GetServerExoApp()
                            ->GetClientObjectByObjectId(pObject->GetId());
                        if (pClient != NULL)
                        {
                            g_pAppManager->GetServerExoApp()->GetSWSMessage()
                                ->SendServerToPlayerCCMessage(
                                    pClient->GetPlayerId(),
                                    CSWSMessage::CC_MESSAGE_DISPEL_MAGIC,
                                    pMessageData, NULL);
                        }
                        delete pMessageData;
                    }
                    unsigned long oidTarget = pObject->GetId();
                    unsigned long oidDispelCreator = pEffect->m_oidCreator;
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                        ->AddEventDeltaTime(0, 0, oidDispelCreator, oidTarget,
                                           CServerAIMaster::EVENT_REMOVE_EFFECT,
                                           pBestEffect);
                    bDispelled = 1;
                }
                else
                {
                    lstResisted.Add(pBestEffect->m_nID);
                }
            }
        } while (bFound && !bDispelled);
    }
    return 1;
}
// PC 0x005384E0
int CSWSEffectListHandler::OnApplyBonusFeat(CSWSObject* pObject,
                                            CGameEffect* pEffect,
                                            int bLoadingModule)
{
    unsigned short nFeat;

    if (pObject->AsSWSCreature() == NULL)
    {
        return 1;
    }
    nFeat = pEffect->GetInteger(0);
    pObject->AsSWSCreature()->m_pStats->m_lstBonusFeats.Add(nFeat);
    return 0;
}
// PC 0x00538550
int CSWSEffectListHandler::OnRemoveBonusFeat(CSWSObject* pObject,
                                             CGameEffect* pGameEffect)
{
    NWSEFFECTLIST_TRACE();
    unsigned short nFeat;
    CSWSCreature* pCreature;

    if (pObject->AsSWSCreature() != NULL)
    {
        nFeat = pGameEffect->GetInteger(0);
        pCreature = pObject->AsSWSCreature();
        pCreature->m_pStats->m_lstBonusFeats.Remove(nFeat);
    }
    return 1;
}
// PC 0x00537090
int CSWSEffectListHandler::OnRemoveSeeInvisible(CSWSObject* pObject,
                                                CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    int nIndex;
    int nCount;
    int nFlags;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        nIndex = pCreature->m_pStats->m_nSeeInvisibleEffectIndex;
        nCount = pCreature->m_lstEffects.num;
        nFlags =
            pCreature->m_nVisionFlags & ~CSWSCreature::VISION_SEE_INVISIBLE;
        if (nIndex < nCount)
        {
            do
            {
                pOther = pCreature->m_lstEffects[nIndex];
                if (pOther->m_nType > EFFECT_SEEINVISIBLE)
                {
                    break;
                }
                if (pOther->m_nType == EFFECT_SEEINVISIBLE && pOther != pEffect)
                {
                    nFlags |= CSWSCreature::VISION_SEE_INVISIBLE;
                }
                nIndex++;
            } while (nIndex < nCount);
        }
        pCreature->m_nVisionFlags = nFlags;
    }
    return 1;
}
// PC 0x00537110
int CSWSEffectListHandler::OnApplyUltraVision(CSWSObject* pObject,
                                              CGameEffect* pEffect,
                                              int bLoadingModule)
{
    CSWSCreature* pCreature;
    CExoArrayList<CGameEffect*>* pEffects;
    CGameEffect* pVisionEffect;
    CGameEffect* pBlindness;
    CGameEffect* pOther;
    int nIndex;
    int nOtherIndex;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pEffects = &pCreature->m_lstEffects;
        pVisionEffect = new CGameEffect(pEffect);
        pVisionEffect->m_nType = EFFECT_VISION;
        pVisionEffect->SetInteger(0, VISION_EFFECT_TYPE_ULTRAVISION);
        pCreature->ApplyEffect(pVisionEffect, bLoadingModule, 0);
        pCreature->m_nVisionFlags |= CSWSCreature::VISION_ULTRAVISION;
        for (nIndex = pCreature->m_pStats->m_nBlindnessEffectIndex;
             nIndex < pEffects->num &&
             (*pEffects)[nIndex]->m_nType <= EFFECT_BLINDNESS;
             nIndex++)
        {
            pBlindness = (*pEffects)[nIndex];
            if (pBlindness->m_nType == EFFECT_BLINDNESS &&
                pBlindness->GetInteger(0) == BLINDNESS_REMOVABLE_BY_TRUESEEING)
            {
                nOtherIndex = 0;
                while (nOtherIndex < pEffects->num)
                {
                    pOther = (*pEffects)[nOtherIndex];
                    if (pOther->m_nType > TRUESEEING_LINKED_EFFECT_LIMIT)
                    {
                        break;
                    }
                    if (pOther->m_nID == pBlindness->m_nID && pOther != pBlindness)
                    {
                        pCreature->CSWSObject::RemoveEffect(pOther);
                        nIndex = pCreature->m_pStats->m_nBlindnessEffectIndex;
                        nOtherIndex = 0;
                    }
                    else
                    {
                        nOtherIndex++;
                    }
                }
            }
        }
    }
    return 0;
}

static inline unsigned short NwseffectlistBlindnessIndex(CSWSCreatureStats* pStats)
{
    return pStats->m_nBlindnessEffectIndex;
}

static inline unsigned short NwseffectlistTrueSeeingIndex(CSWSCreatureStats* pStats)
{
    return pStats->m_nTrueSeeingEffectIndex;
}

static inline int NwseffectlistEffectCount(CExoArrayList<CGameEffect*>* pList)
{
    return pList->num;
}
// PC 0x00537270
int CSWSEffectListHandler::OnRemoveUltraVision(CSWSObject* pObject,
                                               CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    CGameEffect* pCopy;
    CExoArrayList<CGameEffect*>* pEffects;
    int nIndex;
    int nCount;
    int nFlags;
    unsigned char nVisionFlags;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pEffects = &pCreature->m_lstEffects;

        nIndex = NwseffectlistTrueSeeingIndex(pCreature->m_pStats);
        nCount = pEffects->num;
        nVisionFlags = pCreature->m_nVisionFlags;
        nFlags = nVisionFlags & ~CSWSCreature::VISION_ULTRAVISION;
        for (; nIndex < nCount; nIndex++)
        {
            pOther = (*pEffects)[nIndex];
            if (pOther->m_nType > EFFECT_ULTRAVISION)
            {
                break;
            }
            if (pOther->m_nType == EFFECT_ULTRAVISION && pOther != pEffect)
            {
                nFlags |= CSWSCreature::VISION_ULTRAVISION;
            }
        }
        pCreature->m_nVisionFlags = nFlags;
        for (nIndex = NwseffectlistBlindnessIndex(pCreature->m_pStats);
             nIndex < NwseffectlistEffectCount(&pCreature->m_lstEffects) &&
             (*pEffects)[nIndex]->m_nType <= EFFECT_BLINDNESS;
             nIndex++)
        {
            pOther = (*pEffects)[nIndex];
            if (pOther->m_nType == EFFECT_BLINDNESS)
            {
                pCopy = new CGameEffect(TRUE);
                pCopy->CopyEffect(pOther, FALSE);
                pCreature->CSWSObject::RemoveEffect(pOther);
                pCreature->ApplyEffect(pCopy, FALSE, FALSE);
            }
        }
    }
    return 1;
}
// PC 0x005373A0
int CSWSEffectListHandler::OnApplyTrueSeeing(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    CSWSCreature* pCreature;
    CExoArrayList<CGameEffect*>* pEffects;
    CGameEffect* pBlindness;
    CGameEffect* pOther;
    int nIndex;
    int nOtherIndex;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pEffects = &pCreature->m_lstEffects;
        pCreature->m_nVisionFlags |= CSWSCreature::VISION_TRUESEEING;
        for (nIndex = pCreature->m_pStats->m_nBlindnessEffectIndex;
             nIndex < pEffects->num &&
             (*pEffects)[nIndex]->m_nType <= EFFECT_BLINDNESS;
             nIndex++)
        {
            pBlindness = (*pEffects)[nIndex];
            if (pBlindness->m_nType == EFFECT_BLINDNESS &&
                pBlindness->GetInteger(0) == BLINDNESS_REMOVABLE_BY_TRUESEEING)
            {
                for (nOtherIndex = 0; nOtherIndex < pEffects->num; nOtherIndex++)
                {
                    pOther = (*pEffects)[nOtherIndex];
                    if (pOther->m_nType > TRUESEEING_LINKED_EFFECT_LIMIT)
                    {
                        break;
                    }
                    if (pOther->m_nID == pBlindness->m_nID && pOther != pBlindness)
                    {
                        pCreature->CSWSObject::RemoveEffect((*pEffects)[nIndex]);
                        nIndex = pCreature->m_pStats->m_nBlindnessEffectIndex;
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x00537480
int CSWSEffectListHandler::OnRemoveTrueSeeing(CSWSObject* pObject,
                                              CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    CExoArrayList<CGameEffect*>* pEffects;
    int nIndex;
    int nCount;
    int nFlags;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        pEffects = &pCreature->m_lstEffects;
        nIndex = pCreature->m_pStats->m_nTrueSeeingEffectIndex;
        nCount = pEffects->num;
        nFlags = pCreature->m_nVisionFlags & ~CSWSCreature::VISION_TRUESEEING;
        for (; nIndex < nCount; nIndex++)
        {
            pOther = (*pEffects)[nIndex];
            if (pOther->m_nType > EFFECT_TRUESEEING)
            {
                break;
            }
            if (pOther->m_nType == EFFECT_TRUESEEING && pOther != pEffect)
            {
                nFlags |= CSWSCreature::VISION_ULTRAVISION;
            }
        }
        pCreature->m_nVisionFlags = nFlags;
        for (nIndex = pCreature->m_pStats->m_nBlindnessEffectIndex;
             nIndex < pEffects->num &&
             (*pEffects)[nIndex]->m_nType <= EFFECT_BLINDNESS;
             nIndex++)
        {
            pOther = (*pEffects)[nIndex];
            if (pOther->m_nType == EFFECT_BLINDNESS)
            {
                pCreature->CSWSObject::RemoveEffect(pOther);
                pCreature->ApplyEffect(pOther, 0, 0);
            }
        }
    }
    return 1;
}
// PC 0x00537550
int CSWSEffectListHandler::OnRemoveBlindness(CSWSObject* pObject,
                                             CGameEffect* pEffect)
{
    CSWSCreature* pCreature;
    CGameEffect* pOther;
    unsigned char nVisionFlags;
    int nBlindness;
    int nFlags;
    int nIndex;

    if (pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        nVisionFlags = pCreature->m_nVisionFlags;
        nBlindness = pEffect->GetInteger(0);
        nFlags = nVisionFlags & ~nBlindness;
        for (nIndex = pCreature->m_pStats->m_nBlindnessEffectIndex;
             nIndex < pCreature->m_lstEffects.num; nIndex++)
        {
            pOther = pCreature->m_lstEffects[nIndex];
            if (pOther->m_nType > EFFECT_BLINDNESS)
            {
                break;
            }
            if (pOther->m_nType == EFFECT_BLINDNESS && pOther != pEffect)
            {
                nFlags |= pOther->GetInteger(0);
            }
        }
        pCreature->m_nVisionFlags = nFlags;
    }
    return 1;
}
// PC 0x005375F0
int CSWSEffectListHandler::OnApplyForcePush(CSWSObject* pObject,
                                          CGameEffect* pEffect,
                                          int bLoadingModule)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL)
    {
        unsigned long oidCreator = pEffect->m_oidCreator;
        CGameObject* pCreator = g_pAppManager->GetServerExoApp()->
            GetGameObject(oidCreator);
        if (pCreator != NULL && pCreator->AsSWSObject() != NULL)
        {
            Vector vSource;
            int bSourcePosition = pEffect->GetInteger(0);
            int bIgnoreObstructions = 0;
            if (bSourcePosition != 0)
            {
                vSource.m_fX = pEffect->GetFloat(0);
                vSource.m_fY = pEffect->GetFloat(1);
                vSource.m_fZ = pEffect->GetFloat(2);
                bIgnoreObstructions = pEffect->GetInteger(1);
            }
            else
            {
                vSource = pCreator->AsSWSObject()->m_vPosition;
            }
            float fDistance = pEffect->GetFloat(3);
            if (fDistance == 0.0f)
                fDistance = 5.0f;
            Vector vDirection = normalize(pCreature->m_vPosition - vSource);
            Vector vDestination = pCreature->m_vPosition + vDirection * fDistance;
            pCreature->SetOrientation(-vDirection);
            if (!bIgnoreObstructions)
            {
                CWalkHitInfo hitInfo;
                float fRadius = pCreature->m_pPathfindInformation->m_fStepScale;
                float fWidth = pCreature->m_pPathfindInformation->m_fHalfStep;
                int nResult = g_pAppManager->GetServerExoApp()->TestDirectLine(
                    pCreature->GetId(), pCreature->GetArea()->GetResRef(),
                    pCreature->m_vPosition, vDestination, fRadius, fWidth,
                    1, &hitInfo);
                if (nResult != CSWSArea::DIRECT_LINE_CLEAR)
                {
                    if (nResult == CSWSArea::DIRECT_LINE_NO_ROOM)
                        vDestination = pCreature->m_vPosition;
                    else
                        vDestination = hitInfo.m_vHitPosition;
                }
            }
            float* pWayPoints = new float[4];
            pWayPoints[0] = pCreature->m_vPosition.m_fX;
            pWayPoints[1] = pCreature->m_vPosition.m_fY;
            pWayPoints[2] = vDestination.m_fX;
            pWayPoints[3] = vDestination.m_fY;
            if (pCreature->m_pPathfindInformation->m_nWayPointCount != 0)
            {
                float* pOldWayPoints = pCreature->m_pPathfindInformation->m_pWayPoints;
                pCreature->m_pPathfindInformation->m_pWayPoints = NULL;
                delete pOldWayPoints;
            }
            pCreature->GetPathfindInformation()->m_nWayPointCount = 4;
            pCreature->GetPathfindInformation()->m_pWayPoints = pWayPoints;
            pCreature->GetPathfindInformation()->m_nField_9C = 0;
            pCreature->m_nAnimationState = CSWSCreature::ANIMATION_STATE_FORCE_PUSH;
            pCreature->WalkUpdateLocation_QuickWalk(1);
            CGameEffect* pStateEffect = new CGameEffect(pEffect);
            pStateEffect->m_nType = EFFECT_SETSTATE;
            pStateEffect->m_nSubType = CGameEffect::DURATION_TYPE_TEMPORARY |
                (pStateEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
            pStateEffect->SetInteger(0, STATE_FORCE_PUSH);
            pCreature->ApplyEffect(pStateEffect, 0, 0);
        }
    }
    return 0;
}
// PC 0x00538590
int CSWSEffectListHandler::OnApplyEffectIcon(CSWSObject* pObject,
                                           CGameEffect* pEffect,
                                           int bLoadingModule)
{
    if (pObject == NULL || pObject->AsSWSCreature() == NULL)
        return 1;

    short nIcon = (short)pEffect->GetInteger(0);
    if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
        CGameEffect::DURATION_TYPE_INSTANTANEOUS)
        return 1;

    CSWSCreature* pCreature = pObject->AsSWSCreature();
    CExoArrayList<CEffectIconObject*>* pIcons = &pCreature->m_lstEffectIcons;
    if (pIcons != NULL)
    {
        int i;
        for (i = 0; i < pIcons->num; ++i)
        {
            if ((*pIcons)[i]->m_nIcon == nIcon)
                return 0;
        }

        CEffectIconObject* pIcon = new CEffectIconObject;
        if (pIcon == NULL)
            return 1;

        CExoString sIconResRef;
        int nPriority;
        int nGood;
        if (!g_pRules->Get2DArrays()->GetEffectIcon()->GetCExoStringEntry(
                nIcon, "IconResRef", &sIconResRef))
        {
            delete pIcon;
            return 1;
        }
        if (!g_pRules->Get2DArrays()->GetEffectIcon()->GetINTEntry(
                nIcon, "Priority", &nPriority))
        {
            delete pIcon;
            return 1;
        }

        pIcon->m_nGood = g_pRules->Get2DArrays()->GetEffectIcon()->GetINTEntry(
            nIcon, "Good", &nGood);
        pIcon->m_nIcon = nIcon;
        pIcon->m_sIconResRef = sIconResRef;
        pIcon->m_bGood = (nGood != 0);
        pIcon->m_nPriority = (unsigned short)nPriority;
        for (i = 0; i < pIcons->num; ++i)
        {
            if ((*pIcons)[i]->m_nPriority > nPriority)
                break;
        }
        pIcons->Insert(pIcon, i);
    }
    return 0;
}
// PC 0x005388E0
void CSWSEffectListHandler::InitializeEffects()
{
    m_pEffectApplied = new EffectAppliedHandler[MAX_EFFECTS];
    m_pEffectRemoved = new EffectRemovedHandler[MAX_EFFECTS];
    for (int nEffect = 0; nEffect < MAX_EFFECTS; nEffect++)
    {
        m_pEffectApplied[nEffect] = NULL;
        m_pEffectRemoved[nEffect] = NULL;
    }

    m_pEffectApplied[EFFECT_DAMAGE] = &CSWSEffectListHandler::OnApplyDamage;
    m_pEffectApplied[EFFECT_HEAL] = &CSWSEffectListHandler::OnApplyHeal;
    m_pEffectApplied[EFFECT_DEATH] = &CSWSEffectListHandler::OnApplyDeath;
    m_pEffectApplied[EFFECT_RESURRECTION] = &CSWSEffectListHandler::OnApplyResurrection;
    m_pEffectApplied[EFFECT_ABILITY_INCREASE] = &CSWSEffectListHandler::OnApplyAbilityIncrease;
    m_pEffectRemoved[EFFECT_ABILITY_INCREASE] = &CSWSEffectListHandler::OnRemoveAbilityIncrease;
    m_pEffectApplied[EFFECT_ABILITY_DECREASE] = &CSWSEffectListHandler::OnApplyAbilityDecrease;
    m_pEffectRemoved[EFFECT_ABILITY_DECREASE] = &CSWSEffectListHandler::OnRemoveAbilityDecrease;
    m_pEffectApplied[EFFECT_SUMMON_CREATURE] = &CSWSEffectListHandler::OnApplySummonCreature;
    m_pEffectRemoved[EFFECT_SUMMON_CREATURE] = &CSWSEffectListHandler::OnRemoveSummonCreature;
    m_pEffectApplied[EFFECT_DAMAGE_RESISTANCE] = &CSWSEffectListHandler::OnApplyDamageResistance;
    m_pEffectApplied[EFFECT_SAVING_THROW_INCREASE] = &CSWSEffectListHandler::OnApplySavingThrowIncrease;
    m_pEffectRemoved[EFFECT_SAVING_THROW_INCREASE] = &CSWSEffectListHandler::OnRemoveSavingThrowIncrease;
    m_pEffectApplied[EFFECT_SAVING_THROW_DECREASE] = &CSWSEffectListHandler::OnApplySavingThrowDecrease;
    m_pEffectRemoved[EFFECT_SAVING_THROW_DECREASE] = &CSWSEffectListHandler::OnRemoveSavingThrowDecrease;
    m_pEffectApplied[EFFECT_AC_INCREASE] = &CSWSEffectListHandler::OnApplyACIncrease;
    m_pEffectRemoved[EFFECT_AC_INCREASE] = &CSWSEffectListHandler::OnRemoveACIncrease;
    m_pEffectApplied[EFFECT_AC_DECREASE] = &CSWSEffectListHandler::OnApplyACDecrease;
    m_pEffectRemoved[EFFECT_AC_DECREASE] = &CSWSEffectListHandler::OnRemoveACDecrease;
    m_pEffectApplied[EFFECT_ATTACK_INCREASE] = &CSWSEffectListHandler::OnApplyAttackIncrease;
    m_pEffectRemoved[EFFECT_ATTACK_INCREASE] = &CSWSEffectListHandler::OnRemoveAttackIncrease;
    m_pEffectApplied[EFFECT_ATTACK_DECREASE] = &CSWSEffectListHandler::OnApplyAttackDecrease;
    m_pEffectRemoved[EFFECT_ATTACK_DECREASE] = &CSWSEffectListHandler::OnRemoveAttackDecrease;
    m_pEffectApplied[EFFECT_DAMAGE_INCREASE] = &CSWSEffectListHandler::OnApplyDamageIncrease;
    m_pEffectRemoved[EFFECT_DAMAGE_INCREASE] = &CSWSEffectListHandler::OnRemoveDamageIncrease;
    m_pEffectApplied[EFFECT_DAMAGE_DECREASE] = &CSWSEffectListHandler::OnApplyDamageDecrease;
    m_pEffectRemoved[EFFECT_DAMAGE_DECREASE] = &CSWSEffectListHandler::OnRemoveDamageDecrease;
    m_pEffectApplied[EFFECT_TEMPORARY_HITPOINTS] = &CSWSEffectListHandler::OnApplyTemporaryHitpoints;
    m_pEffectRemoved[EFFECT_TEMPORARY_HITPOINTS] = &CSWSEffectListHandler::OnRemoveTemporaryHitpoints;
    m_pEffectApplied[EFFECT_DAMAGE_IMMUNITY_INCREASE] = &CSWSEffectListHandler::OnApplyDamageImmunityIncrease;
    m_pEffectRemoved[EFFECT_DAMAGE_IMMUNITY_INCREASE] = &CSWSEffectListHandler::OnRemoveDamageImmunityIncrease;
    m_pEffectApplied[EFFECT_DAMAGE_IMMUNITY_DECREASE] = &CSWSEffectListHandler::OnApplyDamageImmunityDecrease;
    m_pEffectRemoved[EFFECT_DAMAGE_IMMUNITY_DECREASE] = &CSWSEffectListHandler::OnRemoveDamageImmunityDecrease;
    m_pEffectApplied[EFFECT_ENTANGLE] = &CSWSEffectListHandler::OnApplyEntangled;
    m_pEffectRemoved[EFFECT_ENTANGLE] = &CSWSEffectListHandler::OnRemoveEntangled;
    m_pEffectApplied[EFFECT_KNOCKDOWN] = &CSWSEffectListHandler::OnApplyKnockdown;
    m_pEffectRemoved[EFFECT_KNOCKDOWN] = &CSWSEffectListHandler::OnRemoveKnockdown;
    m_pEffectApplied[EFFECT_DISARM] = &CSWSEffectListHandler::OnApplyDisarm;
    m_pEffectRemoved[EFFECT_DISARM] = &CSWSEffectListHandler::OnRemoveDisarm;
    m_pEffectApplied[EFFECT_SET_AI_STATE] = &CSWSEffectListHandler::OnApplySetAIState;
    m_pEffectRemoved[EFFECT_SET_AI_STATE] = &CSWSEffectListHandler::OnRemoveSetAIState;
    m_pEffectApplied[EFFECT_DEAF] = &CSWSEffectListHandler::OnApplyDeaf;
    m_pEffectApplied[EFFECT_ENEMY_ATTACK_BONUS] = &CSWSEffectListHandler::OnApplyEnemyAttackBonus;
    m_pEffectRemoved[EFFECT_ENEMY_ATTACK_BONUS] = &CSWSEffectListHandler::OnRemoveEnemyAttackBonus;
    m_pEffectApplied[EFFECT_SETSTATE] = &CSWSEffectListHandler::OnApplySetState;
    m_pEffectRemoved[EFFECT_SETSTATE] = &CSWSEffectListHandler::OnRemoveSetState;
    m_pEffectApplied[EFFECT_NEGATIVE_LEVEL] = &CSWSEffectListHandler::OnApplyNegativeLevel;
    m_pEffectRemoved[EFFECT_NEGATIVE_LEVEL] = &CSWSEffectListHandler::OnRemoveNegativeLevel;
    m_pEffectApplied[EFFECT_DAMAGE_SHIELD] = &CSWSEffectListHandler::OnApplyDamageShield;
    m_pEffectRemoved[EFFECT_DAMAGE_SHIELD] = &CSWSEffectListHandler::OnRemoveDamageShield;
    m_pEffectApplied[EFFECT_REGENERATE] = &CSWSEffectListHandler::OnApplyRegenerate;
    m_pEffectApplied[EFFECT_DISEASE] = &CSWSEffectListHandler::OnApplyDisease;
    m_pEffectRemoved[EFFECT_DISEASE] = &CSWSEffectListHandler::OnRemoveDisease;
    m_pEffectApplied[EFFECT_POISON] = &CSWSEffectListHandler::OnApplyPoison;
    m_pEffectRemoved[EFFECT_POISON] = &CSWSEffectListHandler::OnRemovePoison;
    m_pEffectApplied[EFFECT_DAMAGE_REDUCTION] = &CSWSEffectListHandler::OnApplyDamageReduction;
    m_pEffectApplied[EFFECT_MOVEMENT_SPEED_INCREASE] = &CSWSEffectListHandler::OnApplyMovementSpeedIncrease;
    m_pEffectRemoved[EFFECT_MOVEMENT_SPEED_INCREASE] = &CSWSEffectListHandler::OnRemoveMovementSpeedIncrease;
    m_pEffectApplied[EFFECT_MOVEMENT_SPEED_DECREASE] = &CSWSEffectListHandler::OnApplyMovementSpeedDecrease;
    m_pEffectRemoved[EFFECT_MOVEMENT_SPEED_DECREASE] = &CSWSEffectListHandler::OnRemoveMovementSpeedDecrease;
    m_pEffectApplied[EFFECT_VISUALEFFECT] = &CSWSEffectListHandler::OnApplyVisualEffect;
    m_pEffectRemoved[EFFECT_VISUALEFFECT] = &CSWSEffectListHandler::OnRemoveVisualEffect;
    m_pEffectApplied[EFFECT_AREA_OF_EFFECT] = &CSWSEffectListHandler::OnApplyAreaOfEffect;
    m_pEffectRemoved[EFFECT_AREA_OF_EFFECT] = &CSWSEffectListHandler::OnRemoveAreaOfEffect;
    m_pEffectApplied[EFFECT_BEAM] = &CSWSEffectListHandler::OnApplyBeam;
    m_pEffectRemoved[EFFECT_BEAM] = &CSWSEffectListHandler::OnRemoveBeam;
    m_pEffectApplied[EFFECT_FORCE_RESISTANCE_INCREASE] = &CSWSEffectListHandler::OnApplyForceResistanceIncrease;
    m_pEffectRemoved[EFFECT_FORCE_RESISTANCE_INCREASE] = &CSWSEffectListHandler::OnRemoveForceResistanceIncrease;
    m_pEffectApplied[EFFECT_FORCE_RESISTANCE_DECREASE] = &CSWSEffectListHandler::OnApplyForceResistanceDecrease;
    m_pEffectRemoved[EFFECT_FORCE_RESISTANCE_DECREASE] = &CSWSEffectListHandler::OnRemoveForceResistanceDecrease;
    m_pEffectApplied[EFFECT_SPELL_LEVEL_ABSORPTION] = &CSWSEffectListHandler::OnApplySpellLevelAbsorption;
    m_pEffectRemoved[EFFECT_SPELL_LEVEL_ABSORPTION] = &CSWSEffectListHandler::OnRemoveSpellLevelAbsorption;
    m_pEffectApplied[EFFECT_SPELL_IMMUNITY] = &CSWSEffectListHandler::OnApplySpellImmunity;
    m_pEffectRemoved[EFFECT_SPELL_IMMUNITY] = &CSWSEffectListHandler::OnRemoveSpellImmunity;
    m_pEffectApplied[EFFECT_HASTE] = &CSWSEffectListHandler::OnApplyHasteOrSlow;
    m_pEffectRemoved[EFFECT_HASTE] = &CSWSEffectListHandler::OnRemoveHasteOrSlow;
    m_pEffectApplied[EFFECT_SLOW] = &CSWSEffectListHandler::OnApplyHasteOrSlow;
    m_pEffectRemoved[EFFECT_SLOW] = &CSWSEffectListHandler::OnRemoveHasteOrSlow;
    m_pEffectApplied[EFFECT_HASTE_INTERNAL] = &CSWSEffectListHandler::OnApplyHasteInternal;
    m_pEffectRemoved[EFFECT_HASTE_INTERNAL] = &CSWSEffectListHandler::OnRemoveHasteInternal;
    m_pEffectApplied[EFFECT_SLOW_INTERNAL] = &CSWSEffectListHandler::OnApplySlowInternal;
    m_pEffectRemoved[EFFECT_SLOW_INTERNAL] = &CSWSEffectListHandler::OnRemoveSlowInternal;
    m_pEffectApplied[EFFECT_LINK] = &CSWSEffectListHandler::OnApplyLink;
    m_pEffectApplied[EFFECT_MODIFYNUMATTACKS] = &CSWSEffectListHandler::OnApplyModifyNumAttacks;
    m_pEffectRemoved[EFFECT_MODIFYNUMATTACKS] = &CSWSEffectListHandler::OnRemoveModifyNumAttacks;
    m_pEffectApplied[EFFECT_CURSE] = &CSWSEffectListHandler::OnApplyCurse;
    m_pEffectRemoved[EFFECT_CURSE] = &CSWSEffectListHandler::OnRemoveCurse;
    m_pEffectApplied[EFFECT_SILENCE] = &CSWSEffectListHandler::OnApplySilence;
    m_pEffectRemoved[EFFECT_SILENCE] = &CSWSEffectListHandler::OnRemoveSilence;
    m_pEffectApplied[EFFECT_INVISIBILITY] = &CSWSEffectListHandler::OnApplyInvisibility;
    m_pEffectRemoved[EFFECT_INVISIBILITY] = &CSWSEffectListHandler::OnRemoveInvisibility;
    m_pEffectApplied[EFFECT_SANCTUARY] = &CSWSEffectListHandler::OnApplySanctuary;
    m_pEffectRemoved[EFFECT_SANCTUARY] = &CSWSEffectListHandler::OnRemoveSanctuary;
    m_pEffectApplied[EFFECT_SEEINVISIBLE] = &CSWSEffectListHandler::OnApplySeeInvisible;
    m_pEffectRemoved[EFFECT_SEEINVISIBLE] = &CSWSEffectListHandler::OnRemoveSeeInvisible;
    m_pEffectApplied[EFFECT_ULTRAVISION] = &CSWSEffectListHandler::OnApplyUltraVision;
    m_pEffectRemoved[EFFECT_ULTRAVISION] = &CSWSEffectListHandler::OnRemoveUltraVision;
    m_pEffectApplied[EFFECT_TRUESEEING] = &CSWSEffectListHandler::OnApplyTrueSeeing;
    m_pEffectRemoved[EFFECT_TRUESEEING] = &CSWSEffectListHandler::OnRemoveTrueSeeing;
    m_pEffectApplied[EFFECT_BLINDNESS] = &CSWSEffectListHandler::OnApplyBlindness;
    m_pEffectRemoved[EFFECT_BLINDNESS] = &CSWSEffectListHandler::OnRemoveBlindness;
    m_pEffectApplied[EFFECT_DARKNESS] = &CSWSEffectListHandler::OnApplyDarkness;
    m_pEffectRemoved[EFFECT_DARKNESS] = &CSWSEffectListHandler::OnRemoveDarkness;
    m_pEffectApplied[EFFECT_MISS_CHANCE] = &CSWSEffectListHandler::OnApplyMissChance;
    m_pEffectRemoved[EFFECT_MISS_CHANCE] = &CSWSEffectListHandler::OnRemoveMissChance;
    m_pEffectApplied[EFFECT_CONCEALMENT] = &CSWSEffectListHandler::OnApplyConcealment;
    m_pEffectRemoved[EFFECT_CONCEALMENT] = &CSWSEffectListHandler::OnRemoveConcealment;
    m_pEffectApplied[EFFECT_DISAPPEARAPPEAR] = &CSWSEffectListHandler::OnApplyDisappearAppear;
    m_pEffectRemoved[EFFECT_DISAPPEARAPPEAR] = &CSWSEffectListHandler::OnRemoveDisappearAppear;
    m_pEffectApplied[EFFECT_DISAPPEAR] = &CSWSEffectListHandler::OnApplyDisappear;
    m_pEffectApplied[EFFECT_APPEAR] = &CSWSEffectListHandler::OnApplyAppear;
    m_pEffectApplied[EFFECT_BONUS_SPELL_OF_LEVEL] = &CSWSEffectListHandler::OnApplyBonusSpellOfLevel;
    m_pEffectRemoved[EFFECT_BONUS_SPELL_OF_LEVEL] = &CSWSEffectListHandler::OnRemoveBonusSpellOfLevel;
    m_pEffectApplied[EFFECT_IMMUNITY] = &CSWSEffectListHandler::OnApplyEffectImmunity;
    m_pEffectRemoved[EFFECT_IMMUNITY] = &CSWSEffectListHandler::OnRemoveEffectImmunity;
    m_pEffectApplied[EFFECT_SETSTATE_INTERNAL] = &CSWSEffectListHandler::OnApplySetStateInternal;
    m_pEffectRemoved[EFFECT_SETSTATE_INTERNAL] = &CSWSEffectListHandler::OnRemoveSetStateInternal;
    m_pEffectApplied[EFFECT_ARCANE_SPELL_FAILURE] = &CSWSEffectListHandler::OnApplyArcaneSpellFailure;
    m_pEffectRemoved[EFFECT_ARCANE_SPELL_FAILURE] = &CSWSEffectListHandler::OnRemoveArcaneSpellFailure;
    m_pEffectApplied[EFFECT_DISPEL_ALL_MAGIC] = &CSWSEffectListHandler::OnApplyDispelAllMagic;
    m_pEffectApplied[EFFECT_DISPEL_BEST_MAGIC] = &CSWSEffectListHandler::OnApplyDispelBestMagic;
    m_pEffectApplied[EFFECT_TAUNT] = &CSWSEffectListHandler::OnApplyTaunt;
    m_pEffectRemoved[EFFECT_TAUNT] = &CSWSEffectListHandler::OnRemoveTaunt;
    m_pEffectApplied[EFFECT_LIGHT] = &CSWSEffectListHandler::OnApplyLight;
    m_pEffectRemoved[EFFECT_LIGHT] = &CSWSEffectListHandler::OnRemoveLight;
    m_pEffectApplied[EFFECT_SKILL_INCREASE] = &CSWSEffectListHandler::OnApplySkillIncrease;
    m_pEffectRemoved[EFFECT_SKILL_INCREASE] = &CSWSEffectListHandler::OnRemoveSkillIncrease;
    m_pEffectApplied[EFFECT_SKILL_DECREASE] = &CSWSEffectListHandler::OnApplySkillDecrease;
    m_pEffectRemoved[EFFECT_SKILL_DECREASE] = &CSWSEffectListHandler::OnRemoveSkillDecrease;
    m_pEffectApplied[EFFECT_HITPOINTCHANGEWHENDYING] = &CSWSEffectListHandler::OnApplyHitPointChangeWhenDying;
    m_pEffectRemoved[EFFECT_HITPOINTCHANGEWHENDYING] = &CSWSEffectListHandler::OnRemoveHitPointChangeWhenDying;
    m_pEffectApplied[EFFECT_SETWALKANIMATION] = &CSWSEffectListHandler::OnApplySpecialWalkAnimation;
    m_pEffectRemoved[EFFECT_SETWALKANIMATION] = &CSWSEffectListHandler::OnRemoveSpecialWalkAnimation;
    m_pEffectApplied[EFFECT_LIMIT_MOVEMENT_SPEED] = &CSWSEffectListHandler::OnApplyLimitMovementSpeed;
    m_pEffectRemoved[EFFECT_LIMIT_MOVEMENT_SPEED] = &CSWSEffectListHandler::OnRemoveLimitMovementSpeed;
    m_pEffectApplied[EFFECT_EFFECT_ICON] = &CSWSEffectListHandler::OnApplyEffectIcon;
    m_pEffectRemoved[EFFECT_EFFECT_ICON] = &CSWSEffectListHandler::OnRemoveEffectIcon;
    m_pEffectApplied[EFFECT_DISGUISE] = &CSWSEffectListHandler::OnApplyDisguise;
    m_pEffectRemoved[EFFECT_DISGUISE] = &CSWSEffectListHandler::OnRemoveDisguise;
    m_pEffectApplied[EFFECT_TIMESTOP] = &CSWSEffectListHandler::OnApplyTimestop;
    m_pEffectRemoved[EFFECT_TIMESTOP] = &CSWSEffectListHandler::OnRemoveTimestop;
    m_pEffectApplied[EFFECT_RACIAL_TYPE] = &CSWSEffectListHandler::OnApplyRacialType;
    m_pEffectRemoved[EFFECT_RACIAL_TYPE] = &CSWSEffectListHandler::OnRemoveRacialType;
    m_pEffectApplied[EFFECT_VISION] = &CSWSEffectListHandler::OnApplyVision;
    m_pEffectRemoved[EFFECT_VISION] = &CSWSEffectListHandler::OnRemoveVision;
    m_pEffectApplied[EFFECT_TURN_RESISTANCE_INCREASE] = &CSWSEffectListHandler::OnApplyTurnResistance;
    m_pEffectRemoved[EFFECT_TURN_RESISTANCE_INCREASE] = &CSWSEffectListHandler::OnRemoveTurnResistance;
    m_pEffectApplied[EFFECT_TURN_RESISTANCE_DECREASE] = &CSWSEffectListHandler::OnApplyTurnResistance;
    m_pEffectRemoved[EFFECT_TURN_RESISTANCE_DECREASE] = &CSWSEffectListHandler::OnRemoveTurnResistance;
    m_pEffectApplied[EFFECT_BONUSFEAT] = &CSWSEffectListHandler::OnApplyBonusFeat;
    m_pEffectRemoved[EFFECT_BONUSFEAT] = &CSWSEffectListHandler::OnRemoveBonusFeat;
    m_pEffectApplied[EFFECT_WOUNDING] = &CSWSEffectListHandler::OnApplyWounding;
    m_pEffectRemoved[EFFECT_WOUNDING] = &CSWSEffectListHandler::OnRemoveWounding;
    m_pEffectApplied[EFFECT_VAMPIRIC_REGENERATION] = &CSWSEffectListHandler::OnApplyVampiricRegeneration;
    m_pEffectApplied[EFFECT_SUMMON_PARTY] = &CSWSEffectListHandler::OnApplySummonParty;
    m_pEffectRemoved[EFFECT_SUMMON_PARTY] = &CSWSEffectListHandler::OnRemoveSummonParty;
    m_pEffectApplied[EFFECT_FORCEDRAIN] = &CSWSEffectListHandler::OnApplyForceDrain;
    m_pEffectApplied[EFFECT_TEMPORARY_FORCE] = &CSWSEffectListHandler::OnApplyTemporaryForce;
    m_pEffectRemoved[EFFECT_TEMPORARY_FORCE] = &CSWSEffectListHandler::OnRemoveTemporaryForce;
    m_pEffectApplied[EFFECT_DROIDSTUN] = &CSWSEffectListHandler::OnApplyDroidStun;
    m_pEffectRemoved[EFFECT_DROIDSTUN] = &CSWSEffectListHandler::OnRemoveDroidStun;
    m_pEffectApplied[EFFECT_FORCEPUSHED] = &CSWSEffectListHandler::OnApplyForcePush;
    m_pEffectRemoved[EFFECT_FORCEPUSHED] = &CSWSEffectListHandler::OnRemoveForcePush;
    m_pEffectApplied[EFFECT_WHIRLWIND] = &CSWSEffectListHandler::OnApplyWhirlWind;
    m_pEffectRemoved[EFFECT_WHIRLWIND] = &CSWSEffectListHandler::OnRemoveWhirlWind;
    m_pEffectApplied[EFFECT_BLASTERDEFLECTIONINCREASE] = &CSWSEffectListHandler::OnApplyBlasterDeflectionIncrease;
    m_pEffectRemoved[EFFECT_BLASTERDEFLECTIONINCREASE] = &CSWSEffectListHandler::OnRemoveBlasterDeflectionIncrease;
    m_pEffectApplied[EFFECT_BLASTERDEFLECTIONDECREASE] = &CSWSEffectListHandler::OnApplyBlasterDeflectionDecrease;
    m_pEffectRemoved[EFFECT_BLASTERDEFLECTIONDECREASE] = &CSWSEffectListHandler::OnRemoveBlasterDeflectionDecrease;
    m_pEffectApplied[EFFECT_HORRIFIED] = &CSWSEffectListHandler::OnApplyHorrified;
    m_pEffectRemoved[EFFECT_HORRIFIED] = &CSWSEffectListHandler::OnRemoveHorrified;
    m_pEffectApplied[EFFECT_DAMAGEFORCEPOINTS] = &CSWSEffectListHandler::OnApplyDamageForcePoints;
    m_pEffectApplied[EFFECT_HEALFORCEPOINTS] = &CSWSEffectListHandler::OnApplyHealForcePoints;
    m_pEffectApplied[EFFECT_CHOKE] = &CSWSEffectListHandler::OnApplyChoke;
    m_pEffectRemoved[EFFECT_CHOKE] = &CSWSEffectListHandler::OnRemoveChoke;
    m_pEffectApplied[EFFECT_BODYFUEL] = &CSWSEffectListHandler::OnApplyBodyFuel;
    m_pEffectRemoved[EFFECT_BODYFUEL] = &CSWSEffectListHandler::OnRemoveBodyFuel;
    m_pEffectApplied[EFFECT_PSYCHICSTATIC] = &CSWSEffectListHandler::OnApplyPsychicStatic;
    m_pEffectRemoved[EFFECT_PSYCHICSTATIC] = &CSWSEffectListHandler::OnRemovePsychicStatic;
    m_pEffectApplied[EFFECT_LIGHTSABERTHROW] = &CSWSEffectListHandler::OnApplyLightsaberThrow;
    m_pEffectRemoved[EFFECT_LIGHTSABERTHROW] = &CSWSEffectListHandler::OnRemoveLightsaberThrow;
    m_pEffectApplied[EFFECT_ASSUREDHIT] = &CSWSEffectListHandler::OnApplyAssuredHit;
    m_pEffectRemoved[EFFECT_ASSUREDHIT] = &CSWSEffectListHandler::OnRemoveAssuredHit;
    m_pEffectApplied[EFFECT_FORCEJUMP] = &CSWSEffectListHandler::OnApplyForceJump;
    m_pEffectRemoved[EFFECT_FORCEJUMP] = &CSWSEffectListHandler::OnRemoveForceJump;
    m_pEffectApplied[EFFECT_FORCEJUMPDELAYED] = &CSWSEffectListHandler::OnApplyForceJumpDelayed;
    m_pEffectRemoved[EFFECT_FORCEJUMPDELAYED] = &CSWSEffectListHandler::OnRemoveForceJumpDelayed;
    m_pEffectApplied[EFFECT_ASSUREDDEFLECTION] = &CSWSEffectListHandler::OnApplyAssuredDeflection;
    m_pEffectRemoved[EFFECT_ASSUREDDEFLECTION] = &CSWSEffectListHandler::OnRemoveAssuredDeflection;
    m_pEffectApplied[EFFECT_FORCERESISTED] = &CSWSEffectListHandler::OnApplyForceResisted;
    m_pEffectApplied[EFFECT_FORCEFIZZLE] = &CSWSEffectListHandler::OnApplyForceFizzle;
    m_pEffectApplied[EFFECT_FORCESHIELD] = &CSWSEffectListHandler::OnApplyForceShield;
    m_pEffectRemoved[EFFECT_FORCESHIELD] = &CSWSEffectListHandler::OnRemoveForceShield;
#ifndef _XBOX
    m_pEffectApplied[EFFECT_PUREGOODPOWERS] = &CSWSEffectListHandler::OnApplyPureGoodPowers; m_pEffectRemoved[EFFECT_PUREGOODPOWERS] = &CSWSEffectListHandler::OnRemovePureGoodPowers;
    m_pEffectApplied[EFFECT_PUREEVILPOWERS] = &CSWSEffectListHandler::OnApplyPureEvilPowers; m_pEffectRemoved[EFFECT_PUREEVILPOWERS] = &CSWSEffectListHandler::OnRemovePureEvilPowers;
#endif
}
// PC 0x00531890
int CSWSEffectListHandler::OnRemoveWhirlWind(CSWSObject* pObject,
                                          CGameEffect* pEffect)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 1;
}
// PC 0x00531890
int CSWSEffectListHandler::OnRemoveBlasterDeflectionIncrease(
    CSWSObject* pObject, CGameEffect* pEffect)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 1;
}
// PC 0x00531890
int CSWSEffectListHandler::OnRemoveBlasterDeflectionDecrease(
    CSWSObject* pObject, CGameEffect* pEffect)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 1;
}
// PC 0x00531890
int CSWSEffectListHandler::OnRemoveHorrified(CSWSObject* pObject,
                                          CGameEffect* pEffect)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 1;
}
// PC 0x00531890
int CSWSEffectListHandler::OnRemoveChoke(CSWSObject* pObject,
                                      CGameEffect* pEffect)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 1;
}
// PC 0x00531890
int CSWSEffectListHandler::OnRemovePsychicStatic(CSWSObject* pObject,
                                              CGameEffect* pEffect)
{
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    return 1;
}

static inline CServerExoApp* ServerExoAppLate2(CAppManager* p)
{
    return ServerExoAppLate1(p);
}

static inline CServerExoApp* ServerExoAppLate1(CAppManager* p)
{
    return p->m_pServerExoApp;
}

static inline void NwseffectlistTrace2()
{
    NwseffectlistTrace1();
}

static inline void NwseffectlistTrace1()
{
}
