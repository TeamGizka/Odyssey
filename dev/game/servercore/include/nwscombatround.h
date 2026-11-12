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
    int m_aUnmapped28[4];
    Vector m_vTarget;

    unsigned long m_oidSpellTarget;
    int m_nUnknown48;
    int m_nUnknown4C;
    unsigned char m_nUnknown50;
    int m_nEquipFlags;
    int m_nUnknown58;
    int m_nSpecialAttack;
    unsigned char m_nUnknown60;

    unsigned long m_oidSpellItem;
    int m_nUnknown68;
    int m_nUnknown6C;
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
        m_nUnknown0 = 0;
        m_nUnknown4 = 0;
        m_nUnknown8 = 0;
        m_nUnknownC = 0;
        m_nUnknown10 = 0;
        m_nUnknown11 = 0;
    }

    void Clear();

    unsigned char m_nUnknown0;
    int m_nUnknown4;
    unsigned char m_nUnknown8;
    int m_nUnknownC;
    unsigned char m_nUnknown10;
    unsigned char m_nUnknown11;
};

struct CSWSCombatAttackFlags
{
    CSWSCombatAttackFlags()
    {
        m_bUnknown0 = 0;
        m_nArmorModifier = 0;
        m_nDexterityModifier = 0;
        m_nBaseDexterityModifier = 0;
        m_nClassArmorModifier = 0;
        m_nNaturalArmorModifier = 0;
        m_nDodgeDeflectionModifier = 0;
        m_nFeatArmorModifier = 0;
        m_nDebilitatedModifier = 0;
    }

    unsigned char m_bUnknown0;
    unsigned char m_nArmorModifier;
    unsigned char m_nDexterityModifier;
    unsigned char m_nBaseDexterityModifier;
    unsigned char m_nClassArmorModifier;
    unsigned char m_nNaturalArmorModifier;
    unsigned char m_nDodgeDeflectionModifier;
    unsigned char m_nFeatArmorModifier;
    unsigned char m_nDebilitatedModifier;
};

struct CSWSCombatAttackExtra1
{
    CSWSCombatAttackExtra1()
    {
        Clear();
    }

    void Clear();

    unsigned char m_nUnknown0;
    unsigned char m_nUnknown1;
    unsigned char m_nUnknown2;
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
    int m_nUnknown18;
    int m_nUnknown1C;
    unsigned char m_nUnknown20;
    unsigned char m_nUnknown21;
    unsigned char m_nUnknown22;
    unsigned char m_nUnknown23;
    unsigned char m_nUnknown24;
    unsigned char m_nUnknown25;
    unsigned char m_nUnknown26;
    unsigned char m_nUnknown27;
    int m_nUnknown28;
    unsigned char m_nUnknown2C;
};

struct CSWSCombatAttackExtra3
{

    enum { DAMAGE_ABSORPTION_ACTIVE = 2 };

    CSWSCombatAttackExtra3()
    {
        m_nBaseDamage = 0;
        m_nDamageBonus = 0;
        m_nUnusedDamageFeedback = 0;
        m_nSneakAttackDamage = 0;
        m_nStrengthDamage = 0;
        m_nWeaponSpecializationDamage = 0;
        m_nUnknown6 = 0;
        m_nUnknown7 = 0;
        m_nUnknown8 = 0;
        m_nUnknown9 = 0;
        m_nUnknownA = 0;
        m_nEffectDamageBonus = 0;
        m_nUnknownC = 0x102;
        m_nUnknown10 = 0;
        for (int i = 0; i < 15; i++)
        {
            m_nUnknownDamage[i] = -1;
        }
    }

    unsigned char m_nBaseDamage;
    unsigned char m_nDamageBonus;
    unsigned char m_nUnusedDamageFeedback;
    unsigned char m_nSneakAttackDamage;
    char m_nStrengthDamage;
    unsigned char m_nWeaponSpecializationDamage;
    unsigned char m_nUnknown6;
    unsigned char m_nUnknown7;
    unsigned char m_nUnknown8;
    unsigned char m_nUnknown9;
    unsigned char m_nUnknownA;
    unsigned char m_nEffectDamageBonus;
    int m_nUnknownC;
    unsigned char m_nUnknown10;
    short m_nUnknownDamage[15];
};

struct CSWSCombatAttackExtra4
{
    CSWSCombatAttackExtra4()
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
        m_nUnknown14 = 0;
    }

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
    int m_nUnknown14;
};

struct CSWSCombatAttackExtra5
{
    CSWSCombatAttackExtra5()
    {
        Clear();
    }

    void Clear()
    {
        m_nUnknown0 = 0;
        m_nUnknown1 = 0;
        m_nUnknown2 = 0;
        m_nUnknown3 = 0;
        m_nUnknown4 = 0;
        m_nUnknown5 = 0;
        m_nUnknown6 = 0;
        m_nUnknown7 = 0;
        m_nUnknown8 = 0;
        m_nUnknown14 = 0;
        m_nUnknown15 = 0;
        m_nUnknown18 = 0;
        m_nUnknown1C = 0;
        m_nUnknown20 = 0;
        m_nUnknown21 = 0;
        m_nUnknown22 = 0;
        m_nUnknown23 = 0;
    }

    unsigned char m_nUnknown0;
    int m_nUnknown1;
    unsigned char m_nUnknown2;
    unsigned char m_nUnknown3;
    char m_nUnknown4;
    unsigned char m_nUnknown5;
    unsigned char m_nUnknown6;
    unsigned char m_nUnknown7;
    int m_nUnknown8;
    unsigned char m_nUnknown14;
    unsigned char m_nUnknown15;
    int m_nUnknown18;
    int m_nUnknown1C;
    unsigned char m_nUnknown20;
    char m_nUnknown21;
    unsigned char m_nUnknown22;
    char m_nUnknown23;
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
    int m_nUnknown10;
    int m_nUnknown14;
    int m_bRangedAttack;
    unsigned long m_nReactionDelay;
    Vector m_vRangedTarget;
    int m_nUnknown2C;
    short m_nDamage[15];
    short m_nUnknown4E;
    CSWSCombatAttackExtra1 m_Extra1;
    CSWSCombatSubAttackThreat m_Extra2;
    CSWSCombatAttackFlags m_Flags;
    unsigned char m_nUnknown8D;
    CSWSCombatAttackExtra3 m_DamageBreakdown;
    CSWSCombatAttackExtra4 m_Extra4;
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
    CSWSCombatSubAttack GetTopSubAttack();
    void ResolveTopSubAttackToAttack();
    int IsSubAttackReady(int nTime);
    void ResolveAttackToMeleeSubAttack(int nAttackNumber,
                                       unsigned long nAnimationTime);
    void ResolveAttackToRangedSubAttack(int nAttackNumber,
                                        unsigned long nAnimationTime, int nArg3,
                                        int nArg4, int nArg5);

    unsigned char m_nAttackGroup;
    int m_nUnknown4;
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
    CExoArrayList<unsigned long> m_lstUnknown94;

    CExoArrayList<CSWCCMessageData*> m_lstFeedbackMessages;
    CExoArrayList<CSWSCombatSubAttack> m_lstSubAttacks;

    int m_nUnknownB8;
    int m_nUnknownBC;
    CSWSCombatAttackExtra1 m_Extra1;
    CSWSCombatSubAttackThreat m_Extra2;
    CSWSCombatAttackFlags m_Flags;
    unsigned char m_nUnknownFD;
    CSWSCombatAttackExtra3 m_DamageBreakdown;
    CSWSCombatAttackExtra4 m_Extra4;

    CSWSCombatAttackExtra5 m_Extra5;
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
    int CheckActionLengthAtTime(unsigned long oidAction, int nLength,
                                int nTime);
    void SetOffHandAttack(int bOffHand);
    int GetOffHandAttack();

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
    int ResolveChoreographedState(unsigned long oidTarget);
    int ResolveEngagedState(unsigned long oidTarget);
    int ResolveMasterState(unsigned long oidTarget, int& bMaster);
    int HasCreatureWeapons();
    int GetTotalAttacks();
    void SetBonusEffectAttacks(int nAttacks);
    int GetActualBonusEffectAttacks();
    int GetFunctionalBonusEffectAttacks()
    {
        return m_nEffectAttacks > 2 ? 2 : m_nEffectAttacks;
    }
    void DecrementRoundLength(int nAmount, int bClamp);
    int GetActionPending();
    int GetSpellActionPending();
    CSWSCombatRoundAction* GetAction();
    void AddAction(CSWSCombatRoundAction* pAction, int nPosition);
    void RemoveAllActions();
    int RemoveLastAction();
    void ClearAllAttacks();
    void AddCutsceneAttackAction(unsigned long oidTarget, int nArg2, int nArg3,
                                 int nArg4);
    void AddAttackAction(unsigned long oidTarget, unsigned short nSpecialAttack,
                         int nArg3, int nArg4, int nArg5);
    void AddSWSpellAction(unsigned long oidCreator, int nArg2, int nArg3,
                          int nArg4, int nArg5, Vector vTarget,
                          unsigned long oidItem, int nArg8, int nArg9,
                          unsigned char nArg10, int nArg11, int nArg12,
                          unsigned char nArg13, int nAttackFlags);
    void AddSWItemSpellAction(unsigned long oidCreator, int nArg2, int nArg3,
                              Vector vTarget, unsigned long oidItem, int nArg6,
                              int nAttackFlags);
    void AddStanceAction(int nNumAttacks, unsigned long oidTarget,
                         int nAttackFlags);
    void AddSwitchWeaponsAction();
    void AddCutsceneMoveActions(unsigned long oidTarget, Vector vTarget,
                                int nArg3);
    void RemoveSpellAction();
    void AddEquipAction(unsigned long oidItem, unsigned long nSlot, int nArg3,
                        int nArg4);
    void AddUnequipAction(unsigned long oidItem, unsigned long nSlot,
                          int nArg3);
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
    CSWSItem* GetCurrentAttackWeapon(int nAttack);
    void StartCombatRound(unsigned long oidTarget, int nArg2, int nArg3,
                          unsigned short nArg4, int nArg5);

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
    int m_nUnknown940;
    int m_nTimer;
    int m_nUnknown948;
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
    unsigned char m_cB1C;
    char m_aUnmappedB1D[0x3];
    unsigned long m_oidUnknown9CC;
    unsigned char m_nCurrentActionType;
    int m_nUnknown9D4;
};

#endif
