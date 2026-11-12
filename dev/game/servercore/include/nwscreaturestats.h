#ifndef __NWSCREATURESTATS_H__
#define __NWSCREATURESTATS_H__

#include "exobase.h"
#include "exoresman.h"

#define SW_MULTICLASS_EXPERIENCE_PENALTY 0.2

#define SW_MULTICLASS_EXPERIENCE_LEVEL_DIFFERENCE 2

#define SW_BASE_ARMOR_CLASS 10
#define SW_MAX_AC_DODGE_BONUS 10
#define SW_DEBILITATED_AC_PENALTY 4

class CSWLevelStats;
class CCombatInformation;
class CSWSCreature;
class CSWSItem;
class CSWSObject;
class CSWSpell;
class CSWFeat;
class CGameEffect;
class CResGFF;
class CResStruct;
class CSWSCreatureAppearanceInfo;
class CSWCreatureStatsUpdate;

class CSWSStats_SpellLikeAbility
{
public:
    CSWSStats_SpellLikeAbility();
    unsigned long m_nSpell;
    unsigned long m_nReady;
    unsigned char m_nLevel;
    unsigned char m_aUnmapped09[3];
};

class CFeatUseListEntry;

class CSWSCreatureStats_ClassInfo
{
public:

    enum { NUM_SPELL_LEVELS = 1 };
    CExoArrayList<unsigned long> m_lstKnownSpells[1];

    CExoArrayList<unsigned long> m_lstAdditionalSpells[1];

    unsigned char m_nSpellCounts[1];
    unsigned char m_nSpellsPerDayLeft[1];
    unsigned char m_nAdditionalSpellCounts[1];
    unsigned char m_nClass;
    unsigned char m_nLevel;
    unsigned char m_nNegativeLevels;

    unsigned char m_nClassState[3];
    char gap_21[3];
    int m_nClassData;
    ~CSWSCreatureStats_ClassInfo();
    CSWSCreatureStats_ClassInfo();
    void AddKnownSpell(unsigned char nSpellLevel, unsigned long nSpell);

    int GetNumberKnownSpells(unsigned char nLevel)
    {
        return m_lstKnownSpells[nLevel].num;
    }

    int GetKnownSpell(unsigned char nLevel, unsigned char nIndex);

    void RemoveKnownSpell(unsigned char nSpellLevel, unsigned long nSpell);

};

class CSWSCreatureStats
{
public:

    enum { MAX_CLASSES = 2 };

    enum { SPELL_CLASS_CHEAT = 255, SPELL_CLASS_SPELL_LIKE_ABILITY = 254 };

    enum { MOVEMENT_RATE_APPEARANCE = 7 };
    CSWSCreatureStats(CSWSCreature* pCreature);
    ~CSWSCreatureStats();
    int ReadStatsFromGff(CResGFF* pRes, CResStruct* pStruct,
                         CSWSCreatureAppearanceInfo* pAppearance);

    enum LoadStatsError
    {
        LOAD_STATS_INVALID_RACE = 1524,
        LOAD_STATS_INVALID_PERCEPTION = 1525,
        LOAD_STATS_EMPTY_CLASS_LIST = 1526,
        LOAD_STATS_INVALID_CLASS = 1527,
        LOAD_STATS_MISSING_CLASS = 1528
    };

    enum { GENDER_NONE = 4 };

    enum { PERCEPTION_RANGE_DEFAULT = 11, PERCEPTION_RANGE_PC = 12 };

    enum PerceptionDataMask
    {
        PERCEPTION_DATA_SEEN = 1, PERCEPTION_DATA_HEARD = 2,
        PERCEPTION_DATA_INAUDIBLE = 4, PERCEPTION_DATA_VANISHED = 8
    };

    enum { AC_EFFECT_OPTION_INDEX = 6 };
    void SaveStats(CResGFF* pRes, CResStruct* pStruct);
    void ReadSpellsFromGff(CResGFF* pRes, CResStruct* pStruct);
    void SaveClassInfo(CResGFF* pRes, CResStruct* pStruct);

    enum GffStructType
    {
        GFF_LEVEL_STAT = 0, GFF_FEAT = 1, GFF_CLASS = 2,
        GFF_KNOWN_SPELL = 3, GFF_SPECIAL_ABILITY = 4,
        GFF_SPELLS_PER_DAY = 0x4567
    };

    enum { MINIMUM_ABILITY_SCORE = 3 };

    enum { FORCE_SENSITIVE_FORCE_POINT_BONUS = 40 };

    enum EffectImmunitySelector
    {
        IMMUNITY_TYPE_MIND_SPELLS = 1, IMMUNITY_TYPE_POISON = 2,

        IMMUNITY_TYPE_FEAR = 4, IMMUNITY_TYPE_PARALYSIS = 6,
        IMMUNITY_TYPE_BLINDNESS = 7, IMMUNITY_TYPE_DEAFNESS = 8,
        IMMUNITY_TYPE_SLOW = 9, IMMUNITY_TYPE_ENTANGLE = 10,
        IMMUNITY_TYPE_STUN = 12, IMMUNITY_TYPE_SLEEP = 13,
        IMMUNITY_TYPE_CONFUSED = 16,

        IMMUNITY_TYPE_CURSED = 17,

        IMMUNITY_TYPE_KNOCKDOWN = 28,

        IMMUNITY_TYPE_NEGATIVE_LEVEL = 29,

        IMMUNITY_TYPE_DEATH = 32,

        IMMUNITY_TYPE_33 = 33
    };

    enum SimpleGoodEvilAlignment
    {
        ALIGNMENT_NEUTRAL = 1,
        ALIGNMENT_GOOD = 2,
        ALIGNMENT_EVIL = 3
    };

    enum { DARK_PORTRAIT_ALIGNMENT_THRESHOLD = 50 };

    int CalcStatModifier(unsigned char nScore);
    int EffectId2Effect2DAId(int nEffectId, int nEffectInteger);
    int GetEffectLinkImmunity(CGameEffect* pEffect);
    unsigned long CalculateExpNeededForNextLevel();
    void AddClass(unsigned char nClass, unsigned char nLevel);
    int GetUseMonkAttackTables(int bForceUnarmed);
    unsigned char GetUnarmedDamageDie();
    unsigned char GetUnarmedDamageDice();
    int GetUnarmedDamageRoll(CSWSObject* pTarget);
    void GetTotalDamageRange(CSWSCreature* pCreature, CSWSItem* pItem,
                             int& nMinimum, int& nMaximum, int bOffHand,
                             int bIncludeEffects);

    enum { WEAPON_SPECIALIZATION_DAMAGE_BONUS = 2 };
    // PC 0x005ED200
    CSWSCreature* GetCreature() { return m_pCreature; }

    short GetCurrentForcePoints()
    {
        return m_nCurrentForcePoints + m_nTemporaryForcePoints;
    }
    int GetCriticalHitRoll(int bOffHand);
    int GetCriticalHitMultiplier(int bOffHand);
    void SetSkillRank(unsigned char nSkill, char nRank);

    enum { SKILL_CHECK_TAKE_20 = 20 };
    char GetSkillRank(unsigned char nSkill, CSWSObject* pTarget, int bBaseRank);
    int GetFavoredEnemyBonus(CSWSCreature* pCreature);
    unsigned char GetSpellsPerDayLeft(unsigned char nMultiClass,
                                      unsigned char nSpellLevel);
    unsigned char ComputeNumberKnownSpellsLeft(unsigned char nClass);
    unsigned short GetNumberKnownSpells(unsigned char nClass,
                                        unsigned char nSpellLevel);
    int GetKnownSpell(unsigned char nClass, unsigned char nSpellLevel,
                      unsigned char nSpellIndex);
    unsigned char GetClass(unsigned char nClass)
    {
        if (nClass >= m_nClassCount)
        {
            return 0xFF;
        }
        return m_aClassInfo[nClass].m_nClass;
    }
    unsigned char GetClassLeveledUpIn()
    {
        return GetClass(m_nClassCount - 1);
    }
    CExoString GetClassString(unsigned char nClass);
    void SetClass(unsigned char nMultiClass, unsigned char nClass);

    void SetClassLevel(unsigned char nClass, unsigned char nLevel)
    {
        if (nClass < m_nClassCount)
            m_aClassInfo[nClass].m_nLevel = nLevel;
    }
    int ResolveSpecialAttackAttackBonus(CSWSCreature* pTarget);
    int ResolveSpecialAttackDamageBonus(CSWSCreature* pTarget);
    unsigned char GetClassLevel(unsigned char nClass,
                                int bExcludeNegativeLevels);
    unsigned char GetClassNegativeLevels(unsigned char nClass);
    void SetClassNegativeLevels(unsigned char nClass, unsigned char nLevels);
    char GetTotalSTRBonus();
    char GetTotalDEXBonus();
    char GetTotalCONBonus();
    char GetTotalINTBonus();
    char GetTotalWISBonus();
    char GetTotalCHABonus();
    char GetTotalACFeatMod();
    int GetBaseAttackBonus();
    int GetAttackModifierVersus(CSWSCreature* pTarget);

    enum { GOOD_AIM_RANGE_SQUARED = 25, GOOD_AIM_ATTACK_BONUS = 10,
           MELEE_VERSUS_RANGED_ATTACK_BONUS = 10 };
    CExoString GetFullName();
    unsigned char GetSTRStat();
    unsigned char GetDEXStat();
    unsigned char GetCONStat();
    unsigned char GetINTStat();
    unsigned char GetWISStat();
    unsigned char GetCHAStat();
    int GetForceResistance();
    int GetIsJedi();
    unsigned char GetACNaturalBase();
    int AdjustHPAccumulatorBy(float fAdjustment);
    int AdjustFPAccumulatorBy(float fAdjustment);
    void AdjustSpellUsesPerDay();
    unsigned char GetSpellGainWithBonus(unsigned char nMultiClass,
                                       unsigned char nSpellLevel);
    int HasFeat(unsigned short nFeat);
    int FeatAcquired(unsigned short nFeat, CExoArrayList<unsigned short>* pList);
    int FeatRequirementsMet(unsigned short nFeat, CExoArrayList<unsigned short>* pList,
                            int bNextLevel);
    int HasFeatPrereq(CSWFeat* pFeat, CExoArrayList<unsigned short>* pList);
    int HasSpellPrereq(CSWSpell* pSpell, CExoArrayList<unsigned long>* pList, int bNextLevel);
    int HasSpell(unsigned char nSpellLevel, unsigned long nSpell,
                 int bCheckForcePoints);
    int GetHasPowerFromItem(int nPower);
    int GetItemDetailsForPower(int nPower, unsigned long& oidItem,
                               int& nItemProperty);
    void AddKnownSpell(unsigned char nClass, unsigned long nSpell);
    void AddFeat(unsigned short nFeat);
    void RemoveFeat(unsigned short nFeat);
    void DecrementFeatRemainingUses(unsigned short nFeat);
    unsigned char GetFeatRemainingUses(unsigned short nFeat);
    int GetWeaponSpecialization(CSWSItem* pItem);
    int GetRangedDamageBonus();
    int GetWeaponFocus(CSWSItem* pItem);
    int GetMeleeAttackBonus(int bOffHand, int bIncludeBase,
                           int bTouchAttack, int bForceOnHand);

    int GetRangedAttackBonus(int bOffHand, int bIncludeBase, int bTouchAttack);
    int GetMeleeDamageBonus(int bOffHand);
    int GetWeaponFinesse(CSWSItem* pItem);
    int GetIsInSpellLikeAbilityList(unsigned long nSpell);
    unsigned char GetSpellLikeAbilityCasterLevel(unsigned long nSpell);

    int GetNumFeats() { return m_lstFeats.num; }
    unsigned short GetFeat(unsigned short nIndex);
    unsigned short GetBonusFeat(unsigned short nIndex);
    int GetNumBonusFeats();
    void SetNormalBonusFlags(unsigned short nFeat, int& bNormal, int& bBonus);
    int CanChooseFeat(unsigned short nFeat, unsigned char nNormal,
                      unsigned char nBonus,
                      CExoArrayList<unsigned short>* pList, int bNextLevel);
    void CalcLevelUpNumberFeats(unsigned char nClass,
                               unsigned char& nNormalFeats,
                               unsigned char& nBonusFeats);
    unsigned char GetSimpleAlignmentGoodEvil();

    char GetSTRMod() { return m_nStrengthModifier; }
    char GetWISMod() { return m_nWisdomModifier; }
    char GetCHAMod() { return m_nCharismaModifier; }
    char GetCONMod() { return m_nConstitutionModifier; }

    char GetINTMod() { return m_nIntelligenceModifier; }

    short GetForcePointAbilityMod() { return GetWISMod() + GetCHAMod(); }
    unsigned char GetLevel(int bExcludeNegativeLevels);
    CSWLevelStats* GetLevelStats(unsigned char nLevel);
    int ValidateLevelUp(CSWLevelStats* pLevelStats);

    void LevelUp(CSWLevelStats* pLevelStats, int bStoreLevelStats);
    int CanLevelUp();
    unsigned char GetPotentialLevel();
    unsigned long GetExpNeededForLevelUp();
    void UpdateCombatInformation();
    void UpdateLastStatsObject(unsigned __int64 nUpdateMask,
                               CSWCreatureStatsUpdate* pLastStats);
    void ModifyAlignment(short nAdjustment);
    void ClearFeats();
    void SetSTRBase(unsigned char nScore);
    void SetDEXBase(unsigned char nScore);
    void SetCONBase(unsigned char nValue, int bUpdate);
    void SetINTBase(unsigned char nScore);
    void SetWISBase(unsigned char nScore);
    void SetCHABase(unsigned char nScore);

    enum
    {
        STAT_AGE = 0,
        STAT_CHALLENGE_RATING = 1,
        STAT_FORTITUDE_SAVE = 2,
        STAT_REFLEX_SAVE = 3,
        STAT_WILL_SAVE = 4,
        STAT_STRENGTH = 5,
        STAT_DEXTERITY = 6,
        STAT_CONSTITUTION = 7,
        STAT_INTELLIGENCE = 8,
        STAT_WISDOM = 9,
        STAT_CHARISMA = 10,
        STAT_BASE_ATTACK_BONUS = 11,
        STAT_FORCE_RESISTANCE = 12
    };
    float GetStatById(int nStatId);
    void SetStatById(int nStatId, float fValue);

    void UpdateSTRModifier() { m_nStrengthModifier = CalcStatModifier(GetSTRStat()); }
    void UpdateDEXModifier() { m_nDexterityModifier = CalcStatModifier(GetDEXStat()); }
    void UpdateCONModifier() { m_nConstitutionModifier = CalcStatModifier(GetCONStat()); }
    void UpdateINTModifier() { m_nIntelligenceModifier = CalcStatModifier(GetINTStat()); }
    void UpdateWISModifier() { m_nWisdomModifier = CalcStatModifier(GetWISStat()); }
    void UpdateCHAModifier() { m_nCharismaModifier = CalcStatModifier(GetCHAStat()); }
    void RemoveKnownSpell(unsigned char nClass, unsigned long nSpell);
    void SetForceResistance(char nResistance);
    void SetArcaneSpellFailure(unsigned char nFailure);
    void SetForceResistancePenalty(char nPenalty);
    void SetExperience(unsigned long nExperience, int bUseThreshold);

    char GetDamageBonus(CSWSCreature* pTarget, int bOffHand, int bRecordDamage);
    void LevelDown(CSWLevelStats* pLevelStats);
    void AddExperience(unsigned long nExperience);
    int GetCanUseSkill(unsigned char nSkill);
    int GetDamageRoll(CSWSObject* pTarget, int bOffHand, int bCritical,
                      int bSneakAttack, int bDeathAttack, int bForceMax);
    void AddSpellLikeAbilityToList(unsigned long nSpell, unsigned char nReady,
                                   unsigned char nLevel)
    {
        CSWSStats_SpellLikeAbility ability;
        ability.m_nSpell = nSpell;
        ability.m_nLevel = nLevel;
        ability.m_nReady = nReady;
        m_pSpellLikeAbilities->Add(ability);
    }
    void SetSpellLikeAbilityReady(unsigned long nSpell, unsigned char bReady,
                                  unsigned char nClass);
    int UnReadySpell(unsigned long nSpell);
    bool GetSpellUsesLeft(unsigned long nSpell, unsigned char nMultiClass,
                          unsigned char nSpellLevel, unsigned char nSpellIndex);
    void AutoLevelup(int bFeedback);

    unsigned long GetExperience() { return m_nExperience; }
    int GetAlignment();
    char GetDEXMod(int bWithEffects);

    char GetPrimaryMod(unsigned char nMultiClass);
    char GetFortSavingThrow(int bBaseOnly);
    char GetBaseFortSavingThrow();
    char GetReflexSavingThrow(int bBaseOnly);
    unsigned char GetBaseReflexSavingThrow();
    unsigned char GetBaseWillSavingThrow();
    char GetWillSavingThrow(int bBaseOnly);
    unsigned char GetAttacksPerRound();
    void SetMovementRate(int nRate);

    enum MovementSpeed
    {
        MOVEMENT_SPEED_PC = 0,
        MOVEMENT_SPEED_IMMOBILE = 1,
        MOVEMENT_SPEED_DEFAULT = 7
    };

    unsigned short GetAppearanceType() { return m_nAppearance; }

    unsigned char GetGender();
    unsigned char GetPhenotype();
    unsigned char GetAppearanceHead();
    unsigned char GetDuplicatingHead();
    int GetUseBackupHead();
    unsigned char GetTail();
    unsigned char GetWings();
    unsigned char GetSkinColor();
    unsigned char GetHairColor();
    unsigned char GetTattoo1Color();
    unsigned char GetTattoo2Color();

    unsigned char GetNumClasses() { return m_nClassCount; }
    float GetLeaderWeight();
    int GetFactionIndex();
    void SetFactionIndex(int nFaction);
    short GetArmorClassVersus(CSWSCreature* pTarget, int bTouchAttack);
    int GetIsPCFaction();

    int GetIsPC() { return m_bIsPC; }

    CResRef GetPortrait() { return m_cPortrait; }

    void SetPortrait(CResRef cPortrait) { m_cPortrait = cPortrait; }
    int GetEffectImmunity(unsigned char nImmunityType, CSWSCreature* pCreator);
    int GetCreatureHasTalent(int nTalentType, int nTalent);

    enum { INVALID_TALENT = -1, INVALID_TALENT_CLASS = 255,
           INVALID_TALENT_CASTER_LEVEL = 255, INVALID_TALENT_ITEM_PROPERTY = -1 };

    enum TalentClass { TALENT_CLASS_SPECIAL_ABILITY = 254 };
    enum TalentSelection { TALENT_SELECTION_RANDOM = -1 };
    static const int TALENT_INITIAL_MAX_CR = -100;
    int GetCreatureTalentRandom(int nFilter, int nSelection,
                                int& nTalentType, int& nTalent,
                                unsigned char& nClass, unsigned long& oidItem,
                                int& nItemProperty, unsigned char& nCasterLevel,
                                int nExclusion, int nExcludedTalentType,
                                int nExcludedTalent);
    int GetCreatureTalentRandomSpecialAbility(int nFilter, int nSelection,
                                              int& nTalentType, int& nTalent,
                                              unsigned char& nClass,
                                              unsigned char& nCasterLevel,
                                              int nExclusion, int nExcludedPower);
    int GetCreatureTalentRandomPowerFromItem(int nFilter, int nSelection,
                                             int& nTalentType, int& nTalent,
                                             unsigned char& nClass,
                                             unsigned long& oidItem,
                                             int& nItemProperty,
                                             int nExclusion, int nExcludedPower);
    int GetCreatureTalentRandomFeat(int nFilter, int nSelection,
                                    int& nTalentType, int& nTalent,
                                    unsigned char& nClass, int nExclusion,
                                    int nExcludedFeat);
    int GetCreatureTalentRandomPower(int nFilter, int nSelection,
                                    int& nTalentType, int& nTalent,
                                    unsigned char& nClass, int nUnknown,
                                    int nExcludedPower);
    int GetCreatureTalentRandomKnownPower(int nFilter, int nSelection,
                                         int& nTalentType, int& nTalent,
                                         unsigned char& nClass, int nClassIndex,
                                         int nUnknown, int nExcludedPower);
    int CheckPowerSuitability(int nPowerSuitability, int nFilter);
    int CheckPowerSuitability(int nTalent, CSWSpell* pSpell,
                              int nPowerSuitability, int nUnknown1,
                              int nUnknown2, int nUnknown3, int nUnknown4,
                              int nInvalidTalent);
    int CanChooseSpell(unsigned long nSpell, unsigned char nMultiClass,
                       CExoArrayList<unsigned long>* pSpellList, int bNextLevel);
    int CheckInclusionMatch(int nType, int nRow, int nExclusion);

    CExoArrayList<unsigned short> m_lstFeats;
    CExoArrayList<CFeatUseListEntry*> m_lstFeatUses;
    CExoArrayList<unsigned short> m_lstBonusFeats;
    CSWSCreature* m_pCreature;
    CExoArrayList<CSWLevelStats*> m_lstLevelStats;
    CExoLocString m_lsFirstName;
    CExoLocString m_lsLastName;
    CResRef m_cConversation;
    int m_bInterruptable;
    CExoLocString m_lsDescription;
    int m_nAge;
    unsigned char m_nGender;
    unsigned char m_aUnmapped65[3];
    unsigned long m_nExperience;
    int m_bIsPC;

    int m_bIsAIDisabledPossess;
    int m_bIsAIDisabledOverride;
    int m_nFactionId;

    int m_nOriginalFactionId;
    unsigned char m_aUnmapped80[0x4];
    float m_f84;
    float m_fChallengeRating;
    float GetChallengeRating() const { return m_fChallengeRating; }
    unsigned char m_nStartingPackage;
    unsigned char m_nClassCount;
    unsigned char m_aUnmapped8A[2];
    CSWSCreatureStats_ClassInfo m_aClassInfo[MAX_CLASSES];
    unsigned short m_nRace;
    unsigned char m_aUnmappedDE[2];
    CExoString m_sSubrace;
    unsigned char m_nSubraceIndex;
    unsigned char m_nStrengthBase;
    char m_nStrengthModifier;
    unsigned char m_nDexterityBase;
    char m_nDexterityModifier;
    unsigned char m_nConstitutionBase;
    char m_nConstitutionModifier;
    unsigned char m_nIntelligenceBase;
    char m_nIntelligenceModifier;
    unsigned char m_nWisdomBase;
    char m_nWisdomModifier;
    unsigned char m_nCharismaBase;
    char m_nCharismaModifier;
    unsigned char m_nACNaturalBase;
    unsigned char m_nACArmorBase;
    unsigned char m_nACShieldBase;
    char m_nACArmorBonus;
    char m_nACArmorPenalty;
    char m_nACDeflectionBonus;
    char m_nACDeflectionPenalty;
    char m_nACShieldBonus;
    char m_nACShieldPenalty;
    char m_nACNaturalBonus;
    char m_nACNaturalPenalty;
    char m_nACDodgeBonus;
    char m_nACDodgePenalty;

    unsigned char m_nBaseAttackBonus;
    char m_nEnemyAttackBonus;
    unsigned char m_aUnmapped104[4];

    int m_nDamageRoll;
    int m_nCriticalHitRoll;
    int m_nCriticalHitMultiplier;
    unsigned char m_nAttacksPerRound;
    unsigned char m_aUnmapped115[3];
    CCombatInformation* m_pCombatInformation;
    unsigned char m_c120;
    unsigned char m_aUnmapped121A[3];
    int m_n124;
    unsigned char m_nArcaneSpellFailure;

    unsigned char m_nBaseArmorArcaneSpellFailure;
    unsigned char m_nBaseShieldArcaneSpellFailure;
    char m_nForceResistance;
    char m_nForceResistancePenalty;
    unsigned char m_aUnmapped121[0x122 - 0x121];

    short m_nForcePoints;
    short m_nCurrentForcePoints;
    short m_nTemporaryForcePoints;
    CExoArrayList<CSWSStats_SpellLikeAbility>* m_pSpellLikeAbilities;

    unsigned short m_nEffectIndex12C;
    unsigned short m_nACEffectIndex;
    unsigned short m_nAttackEffectIndex;
    unsigned short m_nDamageReductionEffectIndex;
    unsigned short m_nDamageResistanceEffectIndex;
    unsigned short m_nDamageIncreaseEffectIndex;
    unsigned short m_nSetAIStateEffectIndex;
    unsigned short m_nAIState;

    unsigned short m_nEffectIconEffectIndex;
    unsigned short m_nSanctuaryEffectIndex;
    unsigned short m_nInvisibilityEffectIndex;
    unsigned short m_nForceResistanceEffectIndex;
    unsigned short m_nAbilityEffectIndex;
    unsigned short m_nSeeInvisibleEffectIndex;
    unsigned short m_nTrueSeeingEffectIndex;

    unsigned short m_nMissChanceEffectIndex;
    unsigned short m_nConcealmentEffectIndex;
    unsigned short m_nBlindnessEffectIndex;
    unsigned short m_nSpecialWalkEffectIndex;

    unsigned short m_nSpellLevelAbsorptionEffectIndex;
    unsigned short m_nSpellImmunityEffectIndex;

    unsigned short m_nDamageShieldEffectIndex;
    unsigned short m_nSkillDecreaseEffectIndex;

    unsigned short m_nSavingThrowEffectIndex;
    unsigned short m_nHasteEffectIndex;

    unsigned short m_nImmunityEffectIndex;

    unsigned short m_nLimitMovementSpeedEffectIndex;
    unsigned short m_nMovementSpeedDecreaseEffectIndex;
    unsigned short m_nSkillPoints;
    unsigned short GetSkillPoints() { return m_nSkillPoints; }
    unsigned char m_aUnmapped166[2];
    unsigned char* m_pSkillRanks;

    char m_nArmorCheckPenalty;
    char m_nShieldCheckPenalty;
    CResRef m_cPortrait;
    short m_nGoodEvilAlignment;
    void SetGoodEvilAlignment(short nAlignment)
    {
        int nValue;
        if (nAlignment >= 0)
        {
            nValue = nAlignment;
            if (nValue > 100)
                nValue = 100;
        }
        else
            nValue = 0;
        m_nGoodEvilAlignment = nValue;
    }
    short m_n18C;
    unsigned char m_aUnmapped180[2];
    unsigned char m_nSkinColor;
    unsigned char m_nHairColor;
    unsigned char m_nTattoo1Color;
    unsigned char m_nTattoo2Color;
    unsigned short m_nAppearance;
    unsigned char m_nPhenotype;
    unsigned char m_nAppearanceHead;

    unsigned char m_nDuplicatingHead;
    unsigned char m_aUnmapped18B[1];
    int m_bUseBackupHead;
    unsigned char m_nTail;
    unsigned char m_nWings;
    unsigned char m_aUnmapped192[2];
    int m_nMovementRate;
    int GetMovementRate() { return m_nMovementRate; }
    float m_fRunRate;
    float m_fWalkRate;
    char m_nFortSaveBonus;
    char m_nWillSaveBonus;
    char m_nReflexSaveBonus;
    unsigned char m_aUnmapped1A3[1];
    CExoString m_sDeity;
    float m_fHPAccumulator;
    float m_fFPAccumulator;
    unsigned char m_aUnmapped1B4[4];
};

#endif
