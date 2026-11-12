#ifndef __NWCCREATURESTATS_H__
#define __NWCCREATURESTATS_H__

#include "exobase.h"
#include "exoresman.h"
#include "nwrules.h"
class CSWGuiInterfaceAction;
#include "nwclass.h"

#include "nwfeat.h"
#include "nwportrait.h"
#include "nwspell.h"
#include "nwrace.h"
#include "nwskill.h"
class CSWSpellArray;
class CCombatInformation;
class CSWCCreature;

class CSWCStats_SpellLikeAbility
{
public:
    CSWCStats_SpellLikeAbility();

    unsigned long m_nSpell;
    int m_nCasterLevel;
    unsigned char m_bReady;
};

class CSWCCreatureStats_ClassInfo
{
public:

    enum { SPELL_LEVEL_COUNT = 1 };

    CSWCCreatureStats_ClassInfo();
    ~CSWCCreatureStats_ClassInfo();

    void ClearKnownSpells();

    int GetNumberKnownSpells(unsigned char nLevel) { return m_lstKnownSpells[nLevel].num; }
    unsigned long GetKnownSpell(unsigned char nLevel, unsigned char nIndex);
    void AddKnownSpell(unsigned char nLevel, unsigned long nSpell);
    void RemoveKnownSpell(unsigned char nLevel, unsigned long nSpell);

    CExoArrayList<unsigned long> m_lstKnownSpells[SPELL_LEVEL_COUNT];
    CExoArrayList<unsigned long> m_lstUnknown0C[1];
    unsigned char m_aSpellsPerDayLeft[SPELL_LEVEL_COUNT];
    unsigned char m_nUnknown19;
    unsigned char m_nUnknown1A;
    unsigned char m_nUnknown1B;
    unsigned char m_nClass;
    unsigned char m_nLevel;
    unsigned char m_aUnknown1E[2];
};

class CSWCCreatureStats
{
public:

    enum Ability
    {
        ABILITY_STRENGTH = 0,
        ABILITY_DEXTERITY = 1,
        ABILITY_CONSTITUTION = 2,
        ABILITY_INTELLIGENCE = 3,
        ABILITY_WISDOM = 4,
        ABILITY_CHARISMA = 5
    };

    enum
    {
        DEFAULT_ABILITY_SCORE = 8,
        NEUTRAL_GOOD_EVIL_ALIGNMENT = 50
    };

    enum
    {
        GOOD_EVIL_DARK_SIDE = 0,
        GOOD_EVIL_LIGHT_SIDE = 100,
        GOOD_EVIL_NEUTRAL_MIN = 40,
        GOOD_EVIL_LIGHT_SIDE_MIN = 60,
        ARMOR_CLASS_TYPE_1 = 1
    };

    CSWCCreatureStats(CSWCCreature* pCreature);
    virtual ~CSWCCreatureStats();

    unsigned char GetClass(unsigned char nIndex);
    unsigned char GetClassLevel(unsigned char nIndex);
    unsigned char GetClassLeveledUpIn();
    void SetClass(unsigned char nIndex, unsigned char nClass);
    void SetClassLevel(unsigned char nIndex, unsigned char nLevel);
    char CalcStatModifier(unsigned char nScore);
    unsigned char GetLevel();

    unsigned long GetExperience() { return m_nExperience; }

    int GetAge() { return m_nAge; }

    unsigned short GetSkillPoints() { return m_nSkillPoints; }

    unsigned char GetSTRStat() { return m_nStrength; }
    unsigned char GetDEXStat() { return m_nDexterity; }
    unsigned char GetCONStat() { return m_nConstitution; }
    unsigned char GetINTStat() { return m_nIntelligence; }
    unsigned char GetWISStat() { return m_nWisdom; }
    unsigned char GetCHAStat() { return m_nCharisma; }

    short GetBaseHitPoints() { return m_nHitPointsBase; }
    short GetCurrentHitPoints() { return m_nCurrentHitPoints; }
    short GetHitPointBonus();

    unsigned char GetNumClasses()
    {
        return m_nClassCount;
    }

    short GetBaseForcePoints() { return m_nForcePointsBase; }
    short GetForcePoints() { short nForcePoints = m_nCurrentForcePoints + m_nForcePointBonus; return nForcePoints; }
    short GetForcePointBonus();
    short GetMaxForcePoints() { return m_nMaxForcePoints; }
    int GetAlignment()
    {
        if (m_nGoodEvilAlignment >= 0)
        {
            int nAlignment = m_nGoodEvilAlignment;
            if (nAlignment > 100)
                nAlignment = 100;
            return nAlignment;
        }
        return 0;
    }
    void UpdateAbilityModifiedSkillRanks();
    unsigned short GetFeat(unsigned short nIndex);

    int GetNumFeats() { return m_lstFeats.num; }
    unsigned short GetNumberKnownSpells(unsigned char nClass,
                                        unsigned char nLevel);
    unsigned long GetKnownSpell(unsigned char nClass, unsigned char nLevel,
                                unsigned char nIndex);
    void ClearSkills();
    char GetSkillRank(unsigned short nSkill);
    void SetSkillRank(unsigned short nSkill, char nRank)  // PC 0x0069FD50
    {
        if (nSkill < g_pRules->m_nNumSkills)
        {
            m_pSkillRanksTotal[nSkill] = nRank;
        }
    }
    void SetSkillRankBase(unsigned short nSkill, char nRank)  // PC 0x0069FD80
    {
        if (nSkill < g_pRules->m_nNumSkills)
        {
            m_pSkillRanks[nSkill] = nRank;
        }
    }

    char GetSkillRankBase(unsigned short nSkill)
    {
        if (nSkill >= g_pRules->m_nNumSkills)
        {
            return 0;
        }
        return m_pSkillRanks[nSkill];
    }

    int GetCanUseSkill(unsigned short nSkill);
    // PC 0x006575D0
    unsigned char GetSimpleAlignmentGoodEvil()
    {
        return m_nGoodEvilAlignment <= 40 ? 3 : (m_nGoodEvilAlignment >= 60) + 1;
    }

    short GetArmorClass(int nType);
    short GetMaxHitPoints();
    int CalculateNumAttacks();
    int CalculateAttackModifier();

    void SetGender(unsigned char nGender) { m_nGender = nGender; }
    void SetPortraitId(unsigned short nPortraitId);
    static int CheckCustomPortrait(CResRef* pResRef, int bFemale);
    CExoString GetFirstName();
    CExoString GetLastName();
    CExoString GetTag()
    {
        return m_sTag;
    }
    void ClearKnownSpells(unsigned char nClass);
    void RemoveKnownSpell(unsigned char nClass, unsigned long nSpell);
    CExoString GetRaceString();
    CExoString GetFullName();

    void SetName(const CExoString& sFirstName, const CExoString& sLastName)
    {
        m_sFirstName = sFirstName;
        m_sLastName = sLastName;
    }
    CExoString GetClassesString(char cSeparator);

    enum
    {
        STRREF_ALIGNMENT_NEUTRAL = 1027,
        STRREF_ALIGNMENT_LIGHT = 1388,
        STRREF_ALIGNMENT_DARK = 1389
    };
    CExoString GetAlignmentInfo();

    int GetBaseAttackBonus();
    int GetCombatModeAttackModifier(CSWCCreature* pCreature);
    CExoString GetAttackBonusString(CSWCCreature* pCreature, int nAttack, int bOffHand);
    CExoString GetGeneralDamageBonusString(CSWCCreature* pCreature, int bOffHand);
    int GetCombatModeDamageModifier(CSWCCreature* pCreature);

    enum { STRREF_ATTACK_BONUS_VERSUS = 0 };
    CExoString GetAlignmentString(int nAlignment);
    CResRef GetPortrait(int bAlternate);
    void GetMenuInfo(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                     unsigned long nType, unsigned long nForcePoints,
                     int nExclusion, unsigned long nItemMask, float fDistance,
                     int nRoomForceRating, int nForceBodyLevel, int nHitPoints,
                     int bForms, int nForm, int bBeastTarget,
                     char nCharismaModifier);
    float GetMinSpellRange(const CExoString& sRange);
    void ClearKnownSpells();
    void AddKnownSpell(unsigned char nClass, unsigned long nSpell);
    char GetSTRMod();
    char GetDEXMod();
    int GetCONMod();
    int GetINTMod();
    int GetWISMod();
    int GetCHAMod();
    char GetBaseINTMod();
    char GetSkillModifier(unsigned short nSkill);
    unsigned char GetSpellsPerDayLeft(unsigned char nClass,
                                      unsigned char nLevel);
    int HasSpell(unsigned char nClass, unsigned long nSpell);
    int HasFeat(unsigned short nFeat);
    int FeatAcquired(unsigned short nFeat);
    int HasClassSkillFeat(unsigned short nSkill);
    void ClearFeats();
    void InitializeCombatInformation();
    unsigned char CalculateUnarmedDamageDice();
    unsigned char CalculateUnarmedDamageDie();
    void SetCombatInformation(CCombatInformation* pInfo);
    void SetFeatRemainingUses(unsigned short nFeat, unsigned char nUses);

    CResRef m_cConversation;
    CExoString m_sFirstName;
    CExoString m_sLastName;
    unsigned short m_nRace;
    unsigned char m_aUnknown26[2];
    CExoString m_sSubRace;
    unsigned char m_nSubraceIndex;
    unsigned char m_nGender;
    unsigned char m_nClassCount;
    unsigned char m_nStartingPackage;
    unsigned char m_nStrength;
    unsigned char m_nDexterity;
    unsigned char m_nConstitution;
    unsigned char m_nIntelligence;
    unsigned char m_nWisdom;
    unsigned char m_nCharisma;
    unsigned char m_aAbilities[6];
    unsigned char m_nDexterityBonus;
    unsigned char m_aUnknown41[6];
    unsigned char m_nUnknown47;
    short m_nHitPointsBase;

    short m_nHitPointBonus;
    short m_nCurrentHitPoints;
    short m_nMaxHitPoints;
    short m_nDefenseBase;
    unsigned char m_aUnknown52[2];
    float m_fUnknown54;

    unsigned long m_nExperience;
    char m_nFortitudeSave;
    char m_nWillSave;
    char m_nReflexSave;
    unsigned char m_nUnknown5F;
    int m_nUnknown60;
    unsigned char m_nUnknown64;
    unsigned char m_aUnknown65[3];
    CCombatInformation* m_pCombatInformation;
    int m_bOwnsCombatInformation;
    CSWPortrait m_cPortrait;

    short m_nGoodEvilAlignment;
    short m_n82;
    CExoString m_sTag;
    char* m_pSkillRanksTotal;
    char* m_pSkillRanks;
    unsigned short m_nAppearance;
    unsigned char m_nColorSkin;
    unsigned char m_nColorHair;
    unsigned char m_nColorTattoo1;
    unsigned char m_nColorTattoo2;
    unsigned char m_nPhenotype;
    unsigned char m_nAppearanceHead;
    unsigned char m_nDuplicatingHead;
    unsigned char m_aUnknown95[3];
    int m_bUseBackupHead;
    unsigned char m_nTail;
    unsigned char m_nWings;
    unsigned char m_aUnknown9E[2];
    CExoString m_sDescription;
    int m_nAge;
    unsigned char m_nCreationStage;
    unsigned char m_nUnknownAD;
    unsigned short m_nSkillPoints;
    CExoArrayList<unsigned short> m_lstFeats;
    CExoArrayList<unsigned short> m_lstBonusFeats;
    CExoArrayList<CFeatUseListEntry*> m_lstFeatUses;
    CSWCCreatureStats_ClassInfo m_aClassInfo[2];
    CExoString m_sDeity;
    unsigned short m_nPortraitId;

    short m_nMaxForcePoints;

    short m_nForcePointsBase;
    short m_nCurrentForcePoints;
    short m_nForcePointBonus;
    CSWCCreature* m_pCreature;
};

class CSWCLevelUpStats : public CSWCCreatureStats
{
public:

    enum
    {
        CLASS_SKILL_POINT_COST = 1,
        CROSS_CLASS_SKILL_POINT_COST = 2,
        SKILL_RANK_LEVEL_BONUS = 3,

        MIN_SKILL_POINTS_PER_LEVEL = 1,
        FIRST_LEVEL_SKILL_POINT_MULTIPLIER = 4
    };

    CSWCLevelUpStats(CSWCCreature* pCreature);

    int FeatAcquired(unsigned short nFeat,
                     CExoArrayList<unsigned short>* pList);
    int HasFeatPrereq(CSWFeat* pFeat, CExoArrayList<unsigned short>* pList);
    int HasSpellPrereq(CSWSpell* pSpell, CExoArrayList<unsigned long>* pList);

    void CalcNumberFeats(unsigned char& nGeneral, unsigned char& nBonus);
    void SetSTRBase(unsigned char nValue);
    void SetDEXBase(unsigned char nValue, int bUpdate);
    void SetCONBase(unsigned char nValue);
    void SetINTBase(unsigned char nValue);
    void SetWISBase(unsigned char nValue);
    void SetCHABase(unsigned char nValue);
    unsigned char AddDefaultPackageFeats(int nClass,
                                         CExoArrayList<unsigned short>* pList,
                                         int bNormal, int bBonus);
    void AddDefaultPackageSkills(int nClass, int bAllSkills);
    void AddGrantedFeats(CExoArrayList<unsigned long>* pList, bool bPCGranted = false);
    void AddGrantedPowers(CExoArrayList<unsigned long>* pList);
    int GetSkillPointCostForSkillRank(unsigned short nSkill, int bIncrease);
    int GetBaseAttackBonus();
    void SetSkillRankBase(unsigned short nSkill, char nRank);
    void SetFortSavingThrow();
    void SetWillSavingThrow();
    void SetReflexSavingThrow();
    int IncreaseSkillRank(unsigned short nSkill);
    void DecreaseSkillRank(unsigned short nSkill, unsigned char nMinimum);
    void SetNormalBonusFlags(unsigned short nFeat, int& bNormal, int& bBonus);
    void SetClass(unsigned char nIndex, unsigned char nClass);
    void SetClassLevel(unsigned char nIndex, unsigned char nLevel);
    void ClearPackage();
    int FeatRequirementsMet(unsigned short nFeat,
                            CExoArrayList<unsigned short>* pList);
    int CanChooseFeat(unsigned short nFeat, unsigned char nNormal,
                      unsigned char nBonus,
                      CExoArrayList<unsigned short>* pList);
    int CanChooseSpell(unsigned long nSpell, unsigned char nMax,
                       CExoArrayList<unsigned long>* pList);
    unsigned char AddDefaultPackageSpells(CExoArrayList<unsigned long>* pList);
    void AddSkillPointsPerLevel();
    unsigned char ComputeNumberKnownSpellsLeft();

    unsigned char& MainCharGenAbility(int nAbility);
    // PC 0x006575F0
    char GetSkillRank(unsigned short nSkill)
    {
        unsigned char nNumSkills = g_pRules->m_nNumSkills;
        if (nSkill >= nNumSkills)
        {
            return 0;
        }
        return m_pSkillRanks[nSkill];
    }

    char& SkillRankBase(unsigned short nSkill)
    {
        return m_pSkillRanksTotal[nSkill];
    }

    CExoArrayList<CResRef> m_lstInventoryRes;
};

#endif
