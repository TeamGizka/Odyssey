#ifndef __NWRULES_H__
#define __NWRULES_H__

#include "exobase.h"
#include "exoresman.h"

class CResGFF;
class CResStruct;
class CResRef;
class CSWFeat;
class CSWClass;
class CSWRace;
class CSWSkill;
class CSWBaseItemArray;
class CSWSpellArray;
class CTwoDimArrays;

class CCombatInformationNode
{
public:

    enum WeaponWield
    {
        WEAPON_ONHAND = 1,
        WEAPON_OFFHAND = 2,
        WEAPON_MISC = 6,
        WEAPON_UNARMED = 7
    };
    enum DamageType
    {
        DAMAGE_BLUDGEONING = 0,
        DAMAGE_PIERCING = 1,
        DAMAGE_SLASHING = 2,
        DAMAGE_UNKNOWN = 14
    };
    CCombatInformationNode();
    int operator!=(CCombatInformationNode& cNode);

    CCombatInformationNode& operator=(CCombatInformationNode& cNode)
    {
        if (this != &cNode)
        {
            m_nModifier = cNode.m_nModifier;
            m_nModifierType = cNode.m_nModifierType;
            m_nWeaponWield = cNode.m_nWeaponWield;
            m_nVersusGoodEvil = cNode.m_nVersusGoodEvil;
            m_nVersusRace = cNode.m_nVersusRace;
        }
        return *this;
    }

    char m_nModifier;
    char m_nModifierType;
    char m_nWeaponWield;
    char m_nVersusGoodEvil;
    unsigned char m_nVersusRace;
};

class CCombatInformation
{
public:
    CCombatInformation();
    ~CCombatInformation();

    int SaveData(CResGFF* pGFF, CResStruct* pStruct);
    int LoadData(CResGFF* pGFF, CResStruct* pStruct);
    int operator!=(CCombatInformation& cInfo);
    CCombatInformation& operator=(CCombatInformation& cInfo);

    void SetNumAttacks(unsigned char nValue) { m_nNumAttacks = nValue; }
    void SetOnHandAttackMod(char nValue) { m_nOnHandAttackMod = nValue; }
    void SetOnHandDamageMod(char nValue) { m_nOnHandDamageMod = nValue; }
    void SetOffHandAttackMod(char nValue) { m_nOffHandAttackMod = nValue; }
    void SetOffHandDamageMod(char nValue) { m_nOffHandDamageMod = nValue; }
    void SetForceResistance(unsigned char nValue) { m_nForceResistance = nValue; }
    void SetArmorCheckPenalty(unsigned char nValue) { m_nArmorCheckPenalty = nValue; }
    void SetUnarmedDamageDice(unsigned char nValue) { m_nUnarmedDamageDice = nValue; }
    void SetUnarmedDamageDie(unsigned char nValue) { m_nUnarmedDamageDie = nValue; }
    void SetOnHandCriticalMultiplier(unsigned char nValue) { m_nOnHandCriticalMultiplier = nValue; }
    void SetOnHandCriticalRange(unsigned char nValue) { m_nOnHandCriticalRange = nValue; }
    void SetOffHandCriticalMultiplier(unsigned char nValue) { m_nOffHandCriticalMultiplier = nValue; }
    void SetOffHandCriticalRange(unsigned char nValue) { m_nOffHandCriticalRange = nValue; }
    void SetOffHandWeaponEquipped(unsigned long nValue) { m_nOffHandWeaponEquipped = nValue; }
    void SetLeftEquipment(unsigned long nValue) { m_nLeftEquipment = nValue; }
    void SetRightEquipment(unsigned long nValue) { m_nRightEquipment = nValue; }
    void SetDamageDice(unsigned char nValue) { m_nDamageDice = nValue; }
    void SetDamageDie(unsigned char nValue) { m_nDamageDie = nValue; }

    unsigned char GetNumAttacks()
    {
        return m_nNumAttacks;
    }

    char GetOnHandAttackMod()
    {
        return m_nOnHandAttackMod;
    }

    char GetOnHandDamageMod()
    {
        return m_nOnHandDamageMod;
    }

    char GetOffHandAttackMod()
    {
        return m_nOffHandAttackMod;
    }

    char GetOffHandDamageMod()
    {
        return m_nOffHandDamageMod;
    }

    unsigned char GetForceResistance()
    {
        return m_nForceResistance;
    }

    unsigned char GetArcaneSpellFailure()
    {
        return m_nArcaneSpellFailure;
    }

    unsigned char GetArmorCheckPenalty()
    {
        return m_nArmorCheckPenalty;
    }

    unsigned char GetUnarmedDamageDice()
    {
        return m_nUnarmedDamageDice;
    }

    unsigned char GetUnarmedDamageDie()
    {
        return m_nUnarmedDamageDie;
    }

    unsigned char GetOnHandCriticalRange()
    {
        return m_nOnHandCriticalRange;
    }

    unsigned char GetOnHandCriticalMultiplier()
    {
        return m_nOnHandCriticalMultiplier;
    }

    unsigned long GetOffHandWeaponEquipped()
    {
        return m_nOffHandWeaponEquipped;
    }

    unsigned char GetOffHandCriticalRange()
    {
        return m_nOffHandCriticalRange;
    }

    unsigned char GetOffHandCriticalMultiplier()
    {
        return m_nOffHandCriticalMultiplier;
    }

    unsigned long GetLeftEquipment()
    {
        return m_nLeftEquipment;
    }

    unsigned long GetRightEquipment()
    {
        return m_nRightEquipment;
    }

    unsigned char m_nNumAttacks;
    char m_nOnHandAttackMod;
    char m_nOnHandDamageMod;
    char m_nOffHandAttackMod;
    char m_nOffHandDamageMod;
    unsigned char m_nForceResistance;
    unsigned char m_nArcaneSpellFailure;
    unsigned char m_nArmorCheckPenalty;
    unsigned char m_nUnarmedDamageDice;
    unsigned char m_nUnarmedDamageDie;
    unsigned char m_nOnHandCriticalRange;
    unsigned char m_nOnHandCriticalMultiplier;
    unsigned long m_nOffHandWeaponEquipped;
    unsigned char m_nOffHandCriticalRange;
    unsigned char m_nOffHandCriticalMultiplier;
    unsigned char m_aUnmapped12[2];
    CExoArrayList<CCombatInformationNode*> m_lstAttacks;
    CExoArrayList<CCombatInformationNode*> m_lstDamage;
    unsigned long m_nRightEquipment;
    unsigned long m_nLeftEquipment;
    CExoString m_sRightString;
    CExoString m_sLeftString;
    unsigned char m_nDamageDice;
    unsigned char m_nDamageDie;
    unsigned char m_aUnmapped46[2];
};

class CSWRules
{
public:

    enum AttackRollThreshold
    {
        ATTACK_ROLL_AUTOMATIC_MISS = 1,
        ATTACK_ROLL_AUTOMATIC_HIT = 20,
        ATTACK_ROLL_CRITICAL_THREAT = 20
    };

    enum Skill
    {
        SKILL_COMPUTER_USE = 0,
        SKILL_DEMOLITIONS = 1,
        SKILL_STEALTH = 2,
        SKILL_AWARENESS = 3,
        SKILL_PERSUADE = 4,
        SKILL_REPAIR = 5,
        SKILL_SECURITY = 6,
        SKILL_TREAT_INJURY = 7,
        SKILL_MAX_SKILLS = 8
    };

    enum { MAX_TRAP_DISARM_DC = 35 };

    enum DifficultyOption
    {
        DIFFICULTY_PC_DAMAGE = 3,
        DIFFICULTY_NPC_DAMAGE = 4,

        DIFFICULTY_PC_MINIMUM_HP = 5
    };
    CSWRules();
    virtual ~CSWRules();

    CTwoDimArrays* Get2DArrays() { return m_p2DArrays; }

    CSWBaseItemArray* GetBaseItemArray() { return m_pBaseItems; }

    void LoadClassInfo();
    void LoadRaceInfo();
    void LoadSkillInfo();

    enum Ability
    {
        ABILITY_STRENGTH = 0,
        ABILITY_DEXTERITY = 1,
        ABILITY_CONSTITUTION = 2,
        ABILITY_INTELLIGENCE = 3,
        ABILITY_WISDOM = 4,
        ABILITY_CHARISMA = 5
    };
    unsigned short RollDice(unsigned char nDice, unsigned char nDie);
    unsigned char GetMetaMagicLevelCost(unsigned char nMetaMagic);
    CSWFeat* GetFeat(unsigned short nFeat);
    int GetDifficultyOption(int nOption, int nDifficulty);
    void LoadDifficultyInfo();
    int GetWeightedDamageAmount(int nMinimum, int nMaximum, int nCritical,
                                int nMultiplier, int nChance);
    void LoadFeatInfo();

    enum { EXPERIENCE_TABLE_LAST_INDEX = 20 };

    int GetExperienceForLevel(unsigned char nLevel) { return m_anExperience[nLevel]; }

    unsigned short GetNumFeats()
    {
        return m_nNumFeatEntries;
    }

    unsigned char GetNumRaces()
    {
        return m_nNumRaces;
    }

    enum
    {
        RANGE_PERSONAL = 0,
        RANGE_TOUCH = 1,
        RANGE_SHORT = 2,
        RANGE_MEDIUM = 3,
        RANGE_LONG = 4,
        RANGE_WEAPON = 5,

        RANGE_PARTY_MEMBER = 11,
        RANGE_PARTY_LEADER = 12
    };

    float m_afPrimaryRange[6];
    float m_afSecondaryRange[6];
    CSWBaseItemArray* m_pBaseItems;
    int m_anExperience[EXPERIENCE_TABLE_LAST_INDEX + 1];
    CSWSpellArray* m_pSpellArray;
    CSWFeat* m_pFeats;
    unsigned long* m_pMasterFeatStrRefs;
    unsigned long* m_pMasterFeatDescriptionStrRefs;
    CResRef* m_pMasterFeatIcons;
    unsigned short* m_pFeatIndices;
    unsigned short m_nNumFeats;
    unsigned short m_nNumFeatEntries;
    char m_nNumMasterFeats;
    unsigned char m_nNumClasses;
    unsigned char m_nNumRaces;
    unsigned char m_nNumSkills;
    CSWClass* m_pClasses;
    CSWRace* m_pRaces;
    CSWSkill* m_pSkills;
    CTwoDimArrays* m_p2DArrays;
    unsigned char m_aDifficultyOptions[32];
};

extern CSWRules* g_pRules;

class CSWNameGen : public CResHelper<CResLTR, RESTYPE_LTR>
{
public:
    virtual ~CSWNameGen();
    CSWNameGen();

    void UnloadNameTable();
    int LoadNameTable(const CExoString& sNameTable);
    CExoString GetRandomName(unsigned char nLength);

    CExoString GetRandomName(unsigned short nRace, unsigned char nNameType,
                             unsigned char nLength);

    int m_bNameTableLoaded;
    unsigned short m_nRace;
    unsigned char m_nGender;
};

class CSWSRules : public CSWRules
{
public:
    CSWSRules();
    ~CSWSRules();
};

#endif
