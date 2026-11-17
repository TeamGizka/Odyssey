#ifndef __NWSCOMBATROUND_H__
#define __NWSCOMBATROUND_H__

#define SW_COMBAT_ROUND_SECONDS 6.0f

#include "exobase.h"
#include "math3d.h"

class CResGFF;
class CResStruct;
class CGameEffect;
class CSWCCMessageData;
class CSWSCreature;
class CSWSItem;

extern int g_nCombatStretch;

extern int g_nCombatAttackRoll;
extern int g_bDisableDamage;

class CSWSCombatRoundAction
{
public:

    enum { ACTION_EQUIP_ITEM = 6 };

    enum { ACTION_UNEQUIP_ITEM = 7 };

    enum { ACTION_ATTACK = 1, ACTION_SPECIAL_ATTACK = 11 };

    enum { ACTION_CAST_SPELL = 9, ACTION_ITEM_CAST_SPELL = 10 };

    enum { ACTION_PAUSE = 3, ACTION_CUTSCENE_MOVE = 12 };

    enum { ACTION_STANCE = 13, ACTION_SWITCH_WEAPONS = 14 };

    CSWSCombatRoundAction();
    ~CSWSCombatRoundAction();

    void ClearData();
    int SaveData(CResGFF* pRes, CResStruct* pStruct);
    int LoadData(CResGFF* pRes, CResStruct* pStruct);

    int m_nActionTimer;
    unsigned short m_nAnimation;
    int m_nAnimationTime;
    int m_nNumAttacks;
    unsigned char m_nActionType;
    unsigned long m_oidTarget;
    int m_bRetargettable;
    unsigned long m_nInventorySlot;
    unsigned long m_oidTargetRepository;

    int m_nSpellId;
    int m_aSpellParams[4];
    Vector m_vTarget;

    unsigned long m_oidSpellTarget;
    int m_bTargetLocation;
    int m_bFake;
    unsigned char m_nProjectilePath;
    int m_nEquipFlags;
    int m_nUnknown58;
    int m_nSpecialAttack;
    unsigned char m_nSpellLikeClass;

    unsigned long m_oidSpellItem;
    int m_nItemProperty;
    int m_nItemOption;
    unsigned short m_nUnknown70;
    unsigned short m_nUnknown72;
    int m_bCutsceneAttack;
    int m_nCutsceneArg1;
    int m_nCutsceneArg2;
    int m_nCutsceneArg3;
    int m_nAttackFlags;
};

class CSWSCombatSubAttackThreat
{
public:
    CSWSCombatSubAttackThreat()
    {
        m_nThreatRange = 0;
        m_bThreat = 0;
        m_nCriticalRoll = 0;
        m_bCriticalConfirmed = 0;
        m_nUnknown10 = 0;
        m_nCriticalMultiplier = 0;
    }

    void Clear();

    unsigned char m_nThreatRange;
    int m_bThreat;
    unsigned char m_nCriticalRoll;
    int m_bCriticalConfirmed;
    unsigned char m_nUnknown10;
    unsigned char m_nCriticalMultiplier;
};

struct CSWSCombatAttackFlags
{
    CSWSCombatAttackFlags()
    {
        m_nArmorClass = 0;
        m_nArmorModifier = 0;
        m_nDexterityModifier = 0;
        m_nBaseDexterityModifier = 0;
        m_nClassArmorModifier = 0;
        m_nNaturalArmorModifier = 0;
        m_nDodgeDeflectionModifier = 0;
        m_nFeatArmorModifier = 0;
        m_nDebilitatedModifier = 0;
    }
    void Clear();

    unsigned char m_nArmorClass;
    unsigned char m_nArmorModifier;
    unsigned char m_nDexterityModifier;
    unsigned char m_nBaseDexterityModifier;
    unsigned char m_nClassArmorModifier;
    unsigned char m_nNaturalArmorModifier;
    unsigned char m_nDodgeDeflectionModifier;
    unsigned char m_nFeatArmorModifier;
    unsigned char m_nDebilitatedModifier;
};

struct CSWSCombatAttackBreakdown
{
    CSWSCombatAttackBreakdown()
    {
        Clear();
    }

    void Clear();

    unsigned char m_nUnknown0;
    unsigned char m_nAttackRoll;
    unsigned char m_nAttackModifier;
    unsigned char m_nStrengthModifier;
    unsigned char m_nDexterityModifier;
    unsigned char m_nUnknown5;
    unsigned char m_nSpecialAttackBonus;
    unsigned char m_nBaseAttackBonus;
    int m_nOffHand;
    unsigned char m_nTwoWeaponPenalty;
    unsigned char m_nWeaponSizeBonus;
    unsigned char m_nTwoWeaponFeatBonus;
    unsigned char m_nTwoWeaponFeat;
    unsigned char m_nDuelingBonus;
    unsigned char m_nDuelingFeat;
    unsigned char m_nGoodAimBonus;
    unsigned char m_nOffensiveTrainingBonus;
    unsigned char m_nWeaponFocusBonus;
    unsigned char m_nEffectAttackBonus;
    int m_bAutomaticHit;
    int m_bAutomaticMiss;
    unsigned char m_nDualStrikeBonus;
    unsigned char m_nTargetingFeat;
    unsigned char m_nTargetingBonus;
    unsigned char m_nCrushOppositionSpell;
    unsigned char m_nCrushOppositionPenalty;
    unsigned char m_nInspireFollowersSpell;
    unsigned char m_nInspireFollowersBonus;
    unsigned char m_nLeaderCharismaBonus;
    int m_nForm;
    unsigned char m_nFormBonus;
};

struct CSWSCombatDamageBreakdown
{

    enum { DAMAGE_ABSORPTION_ACTIVE = 2 };

    CSWSCombatDamageBreakdown()
    {
        m_nBaseDamage = 0;
        m_nDamageBonus = 0;
        m_nUnusedDamageFeedback = 0;
        m_nSneakAttackDamage = 0;
        m_nStrengthDamage = 0;
        m_nWeaponSpecializationDamage = 0;
        m_nToughnessReduction = 0;
        m_nEnduranceReduction = 0;
        m_nResistedDamageType = 0;
        m_nToughnessDamage = 0;
        m_nEnduranceDamage = 0;
        m_nEffectDamageBonus = 0;
        m_nUnknownC = 0x102;
        m_nEffectDamage = 0;
        for (int i = 0; i < 15; i++)
        {
            m_nUnknownDamage[i] = -1;
        }
    }
    void Clear();

    unsigned char m_nBaseDamage;
    unsigned char m_nDamageBonus;
    unsigned char m_nUnusedDamageFeedback;
    unsigned char m_nSneakAttackDamage;
    char m_nStrengthDamage;
    unsigned char m_nWeaponSpecializationDamage;
    unsigned char m_nToughnessReduction;
    unsigned char m_nEnduranceReduction;
    unsigned char m_nResistedDamageType;
    unsigned char m_nToughnessDamage;
    unsigned char m_nEnduranceDamage;
    unsigned char m_nEffectDamageBonus;
    int m_nUnknownC;
    unsigned char m_nEffectDamage;
    short m_nUnknownDamage[15];
};

struct CSWSCombatEffectBreakdown
{
    CSWSCombatEffectBreakdown()
    {
        m_oidUnknown0 = OBJECT_INVALID;
        m_nSavingThrowType = 0;
        m_nState = 0;
        m_nSaveVersusType = 0;
        m_nSavingThrowRoll = 0;
        m_nSavingThrowEffectBonus = 0;
        m_nSavingThrowBonus = 0;
        m_nSavingThrowTotal = 0;
        m_nSavingThrowDC = 0;
        m_nSavingThrowResult = 0xFF;
        m_bStateImmune = 0;
        m_bEffectApplied = 0;
    }
    void Clear();

    unsigned long m_oidUnknown0;

    unsigned char m_nSavingThrowType;

    unsigned char m_nState;
    unsigned char m_nSaveVersusType;
    unsigned char m_nSavingThrowRoll;
    unsigned char m_nSavingThrowEffectBonus;
    unsigned char m_nSavingThrowBonus;
    unsigned char m_nSavingThrowTotal;
    unsigned char m_nSavingThrowDC;
    unsigned char m_nSavingThrowResult;
    int m_bStateImmune;
    int m_bEffectApplied;
};

struct CSWSCombatDeflectBreakdown
{
    CSWSCombatDeflectBreakdown()
    {
        Clear();
    }

    void Clear();

    unsigned char m_nDeflectRoll;
    int m_nAttackTotal;
    unsigned char m_nJediDefenseFeat;
    unsigned char m_nJediDefenseBonus;
    char m_nEffectBonus;
    unsigned char m_nDeflectResult;
    unsigned char m_nBaseAttackBonus;
    unsigned char m_nArmorCheckPenalty;
    int m_nDeflectTotal;
    unsigned char m_nDeflectionSpellBonus;
    unsigned char m_nDeflectionSpell;
    int m_nFormDeflectionBonus;
    int m_nForm;
    unsigned char m_nDeflectionClassFeat;
    char m_nClassBonus;
    unsigned char m_nPenaltyFeat;
    char m_nPenalty;
};

class CSWSCombatSubAttack
{
public:
    CSWSCombatSubAttack();

    ~CSWSCombatSubAttack();

    int m_nAttackNumber;
    unsigned char m_nAttackResult;
    unsigned short m_nAttackType;
    unsigned char m_nWeaponAttackType;
    unsigned long m_nAnimationTime;
    int m_bAttacked;
    int m_bHit;
    int m_bRangedAttack;
    unsigned long m_nReactionDelay;
    Vector m_vRangedTarget;
    int m_nDischargeHand;
    short m_nDamage[15];
    short m_nUnknown4E;
    CSWSCombatAttackBreakdown m_AttackBreakdown;
    CSWSCombatSubAttackThreat m_ThreatBreakdown;
    CSWSCombatAttackFlags m_Flags;
    unsigned char m_nUnknown8D;
    CSWSCombatDamageBreakdown m_DamageBreakdown;
    CSWSCombatEffectBreakdown m_EffectBreakdown;
};

class CResGFF;
class CResStruct;

class CSWSCombatAttackData
{
public:

    enum { ATTACK_GROUP_INVALID = 255, UNKNOWN66_UNSET = 255 };

    enum { BASE_DAMAGE_INDEX = 14, DAMAGE_COUNT = 15 };

    enum
    {
        DAMAGE_INDEX_BLUDGEONING = 0,
        DAMAGE_INDEX_PIERCING = 1,
        DAMAGE_INDEX_SLASHING = 2,
        DAMAGE_INDEX_ELECTRICAL = 7, DAMAGE_INDEX_DARK_SIDE = 9
    };

    enum { ATTACK_RESULT_WIDE_MISS = 10 };

    enum { ATTACK_RESULT_CRITICAL_HIT = 2 };

    enum { ATTACK_RESULT_MISS = 4 };

    enum { ATTACK_RESULT_DEFLECTED = 8 };

    enum { ATTACK_RESULT_REFLECTED = 9 };

    CSWSCombatAttackData();
    ~CSWSCombatAttackData();
    void Copy(CSWSCombatAttackData* pAttackData);

    int GetDamage(unsigned short nDamageFlags);
    void SetBaseDamage(int nDamage);
    int GetBaseDamage();
    void SetDamage(unsigned short nDamageFlags, int nDamage);
    void AddDamage(unsigned short nDamageFlags, int nDamage, int bAddToBase);
    int GetTotalDamage(int bIncludeBase);
    unsigned short GetAttackType() { return m_nAttackType; }
    unsigned char GetAttackMode() { return m_nAttackMode; }
    int SaveData(CResGFF* pRes, CResStruct* pStruct);
    int LoadData(CResGFF* pRes, CResStruct* pStruct);
    void ResolveSubAttackToAttack(const CSWSCombatSubAttack& subAttack);
    void ClearAttackData();
    void RemoveTopSubAttack();
    void ClearSubAttacks();
    CSWSCombatSubAttack GetTopSubAttack();
    void ResolveTopSubAttackToAttack();
    int IsSubAttackReady(int nTime);
    void ResolveAttackToMeleeSubAttack(int nAttackNumber,
                                       unsigned long nAnimationTime);
    void AddMeleeSubAttack(unsigned long nAnimationTime);
    void ResolveAttackToRangedSubAttack(int nAttackNumber,
                                        unsigned long nAnimationTime, int bAttack,
                                        int bHit, int nDischargeHand);
    void AddRangedSubAttack(int nAttackNumber, unsigned char nAttackResult,
                            unsigned long nAnimationTime, int bHit, Vector vRangedTarget,
                            unsigned long nReactionDelay, int nDischargeHand);

    unsigned char m_nAttackGroup;
    int m_bAttacked;
    unsigned short m_nAnimationLength;
    unsigned long m_oidReactObject;
    unsigned short m_nReactionDelay;
    unsigned short m_nReactionAnimation;
    unsigned short m_nReactionAnimationLength;
    unsigned long m_nMissedBy;
    short m_nDamage[DAMAGE_COUNT];
    unsigned char m_nWeaponAttackType;
    unsigned char m_nAttackMode;
    unsigned char m_nConcealment;
    int m_bRangedAttack;
    int m_bSneakAttack;
    int m_bKillingBlow;
    int m_bCoupDeGrace;
    int m_bCriticalThreat;
    unsigned char m_nUnknown54;
    int m_bAttackDeflected;
    unsigned char m_nAttackResult;
    int m_nUnknown60;
    unsigned short m_nAttackType;

    unsigned short m_nUnknown66;
    Vector m_vRangedTarget;
    unsigned long m_oidAmmoItem;
    CExoString m_sAttackDebugText;
    CExoString m_sDamageDebugText;
    CExoArrayList<CGameEffect*> m_lstEffects;
    CExoArrayList<unsigned long> m_lstSpellImpacts;

    CExoArrayList<CSWCCMessageData*> m_lstFeedbackMessages;
    CExoArrayList<CSWSCombatSubAttack> m_lstSubAttacks;

    int m_bUseAttackResult;
    int m_nForcedAttackResult;
    CSWSCombatAttackBreakdown m_AttackBreakdown;
    CSWSCombatSubAttackThreat m_ThreatBreakdown;
    CSWSCombatAttackFlags m_Flags;
    unsigned char m_nUnknownFD;
    CSWSCombatDamageBreakdown m_DamageBreakdown;
    CSWSCombatEffectBreakdown m_EffectBreakdown;

    CSWSCombatDeflectBreakdown m_DeflectBreakdown;
};

class CSWSCombatRound
{
public:

    enum { FEAT_ATTACK_TIME = 1500 };

    enum { ACTION_POSITION_BY_TIME = -1 };

    enum { ATTACK_ACTION_TIME = 1500 };

    enum { ATTACK_ANIMATION_DEFAULT = 10009, ATTACK_ACTION_OPTION_DEFAULT = 4 };

    enum WeaponAttackType
    {
        WEAPON_ATTACK_AUTOMATIC = 0,
        WEAPON_ATTACK_RIGHTHAND = 1,
        WEAPON_ATTACK_LEFTHAND = 2,
        WEAPON_ATTACK_CREATURE1 = 3,
        WEAPON_ATTACK_CREATURE2 = 4,
        WEAPON_ATTACK_CREATURE3 = 5,
        WEAPON_ATTACK_EXTRA_RIGHTHAND = 6,
        WEAPON_ATTACK_UNARMED = 7,
        WEAPON_ATTACK_EXTRA_UNARMED = 8
    };
    CSWSCombatRound(CSWSCreature* pCreature);
    ~CSWSCombatRound();

    void SetSpellCastRound(int nSpellId, int nRound);
    void SetRoundPaused(int bPaused, unsigned long oidPauser);
    void SetPauseTimer(int nTime, int bInfinite);
    int CheckActionLength(unsigned long oidAction, int nLength);
    int CheckActionLengthAtTime(unsigned long oidAction, int nLength,
                                int nTime);
    void SetOffHandAttack(int bOffHand);
    int GetOffHandAttack();
    int GetExtraAttack();

    int GetOffHandTaken() { return m_nOffHandTaken; }
    int CalculateOnHandAttacks(unsigned short nFeat);
    int CalculateOffHandAttacks();
    void InitializeNumberOfAttacks(unsigned short nFeat)
    {
        CalculateOnHandAttacks(nFeat);
        CalculateOffHandAttacks();
        m_nOffHandTaken = 0;
        m_nExtraTaken = 0;
    }
    unsigned char GetCurrentAction();
    void SetCurrentAction(unsigned char nAction);
    void SetCurrentCombatRoundAction(CSWSCombatRoundAction* pAction);
    CSWSCombatAttackData* GetAttack(int nAttack);
    unsigned char GetCurrentAttack() { return m_nCurrentAttack; }
    void SetCurrentAttack(unsigned char nCurrentAttack);
    int ResolveChoreographedState(unsigned long oidTarget);
    int ResolveEngagedState(unsigned long oidTarget);
    int ResolveMasterState(unsigned long oidTarget, int& bMaster);
    unsigned short GetSpecialAttack(int nAttack);
    int HasCreatureWeapons();
    int GetCombatStepRequired(unsigned long oidTarget);
    int GetTotalAttacks();
    unsigned short GetSpecialAttackID(int nAttack);
    void SetBonusEffectAttacks(int nAttacks);
    int GetActualBonusEffectAttacks();
    int GetFunctionalBonusEffectAttacks()
    {
        return m_nEffectAttacks > 2 ? 2 : m_nEffectAttacks;
    }
    void DecrementRoundLength(int nAmount, int bClamp);
    int GetActionPending();
    int GetAttackActionPending();
    int GetSpellActionPending();
    CSWSCombatRoundAction* GetAction();
    void AddAction(CSWSCombatRoundAction* pAction, int nPosition);
    void RemoveAllActions();
    int RemoveLastAction();
    void ClearAllAttacks();
    void SetDeflectArrow(int bDeflectArrow);
    void SignalCombatRoundStarted();
    void StartCombatRoundCast(int nSpellId, int nRound, unsigned long oidTarget,
                              int bEngaged, int bMaster);
    void AddSpellAction();
    void AddCutsceneAttackAction(unsigned long oidTarget, int nAnimation, int nAttackResult,
                                 int nDamage);
    void AddAttackAction(unsigned long oidTarget, unsigned short nSpecialAttack,
                         int nAttackFlags, int nActionTime, int nNumAttacks);
    void AddSWSpellAction(unsigned long oidCreator, int nMultiClass, int nSpellLevel,
                          int nMetamagic, int bOverride, Vector vTarget,
                          unsigned long oidItem, int bTargetLocation, int bFake,
                          unsigned char nProjectilePath, int bInstant, int nFeat,
                          unsigned char nSpellLikeClass, int nAttackFlags);
    void AddSWItemSpellAction(unsigned long oidCreator, int nItemProperty, int nItemOption,
                              Vector vTarget, unsigned long oidItem, int bTargetLocation,
                              int nAttackFlags);
    void AddStanceAction(int nNumAttacks, unsigned long oidTarget,
                         int nAttackFlags);
    void AddSwitchWeaponsAction();
    void AddCutsceneMoveActions(unsigned long oidTarget, Vector vTarget,
                                int bRun);
    void RemoveSpellAction();
    void AddParryAttack(unsigned long oidTarget);
    void AddParryIndex();
    void AddCleaveAttack(unsigned long oidTarget);
    void AddAttackOfOpportunity(unsigned long oidTarget);
    void AddEquipAction(unsigned long oidItem, unsigned long nSlot, int nEquipFlags,
                        int nAttackFlags);
    void AddUnequipAction(unsigned long oidItem, unsigned long nSlot,
                          int nEquipFlags);
    void AddCombatStepAction(unsigned long nTime, unsigned long oidTarget);
    int GetWeaponAttackType();
    void DEBUGDumpCombatRoundToScreen(int nArg1, const CExoString& sMessage);
    void UpdateAttackTargetForAllActions(unsigned long oidTarget);
    int SaveCombatRound(CResGFF* pRes, CResStruct* pStruct);
    int LoadCombatRound(CResGFF* pRes, CResStruct* pStruct);
    void EndCombatRound(int bArg);
    void IncrementTimer(int nAmount);
    void DecrementPauseTimer(int nAmount);

    enum { ATTACK_ID_WRAP = 0xFFFE };
    unsigned short GetNewAttackID();
    void InsertSpecialAttack(unsigned short nAttack, int nIndex);
    void ClearAllSpecialAttacks();
    void AddReaction(int nReactionDelay, int nAnimationTime, int nPosition);
    CSWSItem* GetCurrentAttackWeapon(int nAttack);
    void StartCombatRound(unsigned long oidTarget, int bEngaged, int bMaster,
                          unsigned short nFeat, int nArg5);

    int GetTimer() { return m_nTimer; }
    int GetRoundLength() { return m_nRoundLength; }
    int GetEngaged() { return m_bEngaged; }
    int GetMasterState() { return m_bMaster; }
    int GetNumActionsTaken() { return m_nNumActionsTaken; }
    void IncrementNumActionsTaken(int nActions) { m_nNumActionsTaken += nActions; }
    unsigned long GetMasterID() { return m_oidMaster; }

    int m_nUnknown0;
    CSWSCombatAttackData m_Attacks[7];
    CExoArrayList<unsigned short> m_lstSpecialAttacks;
    CExoArrayList<unsigned short> m_lstSpecialAttackIds;
    unsigned short m_nAttackId;
    int m_bRoundStarted;
    int m_bSpellCastRound;
    int m_nSpellCastRound;
    int m_bUseAttackResult;
    int m_nTimer;
    int m_bTargetDebilitated;
    int m_nRoundLength;
    int m_nOverlapAmount;
    int m_nBleedTimer;
    int m_bRoundPaused;
    unsigned long m_oidRoundPausedBy;
    int m_nPauseTimer;
    int m_bInfinitePause;
    int m_nNumActionsTaken;
    unsigned char m_nCurrentAttack;
    int m_bOffHandAttack;
    unsigned char m_nAttackGroup;
    int m_bDeflectArrow;
    int m_bWeaponSucks;
    int m_nParryIndex;
    int m_nNumAOOs;
    int m_nNumCleaves;
    unsigned long m_oidNewAttackTarget;
    int m_nOnHandAttacks;
    int m_nOffHandAttacks;
    int m_nOffHandTaken;
    int m_nExtraTaken;
    int m_nAdditAttacks;
    int m_nEffectAttacks;
    unsigned char m_nParryActions;
    unsigned long m_oidDodgeTarget;
    CExoLinkedList<CSWSCombatRoundAction>* m_pScheduledActions;
    CSWSCreature* m_pBaseCreature;
    int m_bEngaged;
    int m_bMaster;
    unsigned long m_oidMaster;
    unsigned long m_oidMasterTarget;
    unsigned long GetMasterTarget() { return m_oidMasterTarget; }

    CSWSCombatRoundAction* m_pCurrentAction;
    unsigned char m_nCurrentRoundActionType;
    char m_aUnmappedB1D[0x3];
    unsigned long m_oidRoundTarget;
    unsigned char m_nCurrentActionType;
    int m_nUnknown9D4;
};

#endif
