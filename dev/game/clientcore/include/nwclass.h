#ifndef __NWCLASS_H__
#define __NWCLASS_H__

class CSWRules;
#include "swplayercontrol.h"
#include "twodimarrays.h"
class CTwoDimArrays;

struct CSWClassFeat
{

    enum
    {
        FEAT_TABLE_NORMAL = 0,
        FEAT_TABLE_NORMAL_AND_BONUS = 1,
        FEAT_TABLE_BONUS = 2,
        FEAT_TABLE_GRANTED = 3,
        FEAT_TABLE_UNAVAILABLE = 4
    };

    enum
    {
        LIST_GRANTED = 0,
        LIST_NORMAL = 1,
        LIST_BONUS = 2
    };

    unsigned short m_nFeat;
    unsigned char m_nGrantedLevel;
    unsigned char m_nList;
    unsigned char m_nPCGrantedLevel;
    int m_nRecommended;
};

struct CSWClassSkill
{
    unsigned short m_nSkill;
    int m_bClassSkill;
    int m_nRecommended;
};

struct CSWClassSpell
{
    unsigned short m_nSpell;
    int m_nLightRecommended;
    int m_nDarkRecommended;
    unsigned char m_nUserType;
};

struct CSWClass
{

    enum ClassIndex
    {
        CLASS_INDEX_SOLDIER = 0,
        CLASS_INDEX_SCOUT = 1,
        CLASS_INDEX_SCOUNDREL = 2
    };
    static const unsigned char CCLASS_SCOUNDREL;
    static const unsigned char CCLASS_SOLDIER;
    static const unsigned char CCLASS_SCOUT;

    static const unsigned char CCLASS_INVALID = 0xFF;
    static const unsigned char CCLASS_JEDICONSULAR;
    static const unsigned char CCLASS_JEDIGUARDIAN;
    static const unsigned char CCLASS_JEDISENTINEL;
    static const unsigned char CCLASS_COMBATDROID;
    static const unsigned char CCLASS_EXPERTDROID;
    static const unsigned char CCLASS_MINION;
    static const unsigned char CCLASS_TECHSPECIALIST;
    static const unsigned char CCLASS_BOUNTYHUNTER;
    static const unsigned char CCLASS_JEDIWEAPONMASTER;
    static const unsigned char CCLASS_JEDIMASTER;
    static const unsigned char CCLASS_JEDIWATCHMAN;
    static const unsigned char CCLASS_SITHMARAUDER;
    static const unsigned char CCLASS_SITHLORD;
    static const unsigned char CCLASS_SITHASSASSIN;

    static const unsigned char SPELL_GAIN_INVALID = 0xFF;

    static const unsigned char FEAT_GRANTED_LEVEL_INVALID = 0xFF;

    enum SpellRecommendationSide
    {
        SPELL_RECOMMEND_DARK = 0,
        SPELL_RECOMMEND_LIGHT = 1
    };

    enum { MAX_FEAT_GAIN_LEVELS = 50 };
    CSWClass();
    ~CSWClass();

    char GetArmorBonus(int nLevel);
    static int IsJedi(int nClass);
    unsigned long GetNameStrRef();
    CExoString GetNameText();
    CExoString GetNameLowerText();
    unsigned char GetAttackBonus(unsigned char nLevel);
    unsigned char GetFortSaveBonus(unsigned char nLevel);
    unsigned char GetRefSaveBonus(unsigned char nLevel);
    unsigned char GetWillSaveBonus(unsigned char nLevel);
    unsigned char GetSpellGain(unsigned char nLevel, unsigned char nSpellLevel);
    unsigned char GetSpellsKnownPerLevel(unsigned char nLevel,
                                         unsigned char nSpellLevel,
                                         unsigned char nClass,
                                         unsigned short nSpell,
                                         unsigned char nMulti);
    void LoadAttackBonusTable(const CExoString& sTable);
    void LoadFeatGain(const CExoString& sTable);
    void LoadFeatsTable(const CExoString& sTable, CSWRules* pRules);
    void LoadSavingThrowTable(const CExoString& sTable);
    void LoadSkillsTable(const CExoString& sTable);
    void LoadSpellGainTable(const CExoString& sTable);
    void LoadSpellKnownTable(const CExoString& sTable);
    void LoadArmorClassTable(const CExoString& sTable);
    int IsSkillClassSkill(unsigned short nSkill);
    int IsSkillUseable(unsigned short nSkill);
    unsigned char GetLevelFeatGranted(unsigned short nFeat);
    int IsBonusFeat(unsigned short nFeat);
    int IsNormalFeat(unsigned short nFeat);
    int IsGrantedFeat(unsigned short nFeat, unsigned char& nLevel);
    int IsFeatUseable(unsigned short nFeat);
    int GetRecommendedFeat(int nIndex);
    int GetRecommendedSkill(int nIndex);
    void LoadSpellsTable();
    int GetRecommendedSpell(int nSide, int nIndex);

    void SetNameStrRef(unsigned long nStrRef)
    {
        m_nNameStrRef = nStrRef;
    }

    void SetNameLowerStrRef(unsigned long nStrRef)
    {
        m_nNameLowerStrRef = nStrRef;
    }

    void SetNamePluralStrRef(unsigned long nStrRef)
    {
        m_nNamePluralStrRef = nStrRef;
    }

    void SetDescriptionStrRef(unsigned long nStrRef)
    {
        m_nDescriptionStrRef = nStrRef;
    }

    void SetIcon(const CExoString& sIcon)
    {
        m_sIcon = sIcon;
    }

    void SetHitDie(unsigned char nDie)
    {
        m_nHitDie = nDie;
    }

    void SetForceDie(unsigned char nDie)
    {
        m_nForceDie = nDie;
    }

    unsigned char GetHitDie()
    {
        return m_nHitDie;
    }

    unsigned char GetForceDie()
    {
        return m_nForceDie;
    }

    unsigned char GetSkillPointBase()
    {
        return m_nSkillPointBase;
    }

    void SetSkillPointBase(unsigned char nPoints)
    {
        m_nSkillPointBase = nPoints;
    }

    void SetPrimaryAbility(unsigned char nAbility)
    {
        m_nPrimaryAbility = nAbility;
    }

    unsigned char GetStrength()
    {
        return m_nStrength;
    }

    unsigned char GetDexterity()
    {
        return m_nDexterity;
    }

    unsigned char GetConstitution()
    {
        return m_nConstitution;
    }

    unsigned char GetWisdom()
    {
        return m_nWisdom;
    }

    unsigned char GetIntelligence()
    {
        return m_nIntelligence;
    }

    unsigned char GetCharisma()
    {
        return m_nCharisma;
    }

    void SetStrength(unsigned char nValue)
    {
        m_nStrength = nValue;
    }

    void SetDexterity(unsigned char nValue)
    {
        m_nDexterity = nValue;
    }

    void SetConstitution(unsigned char nValue)
    {
        m_nConstitution = nValue;
    }

    void SetIntelligence(unsigned char nValue)
    {
        m_nIntelligence = nValue;
    }

    void SetWisdom(unsigned char nValue)
    {
        m_nWisdom = nValue;
    }

    void SetCharisma(unsigned char nValue)
    {
        m_nCharisma = nValue;
    }

    void SetAlignRestrict(unsigned char nAlign)
    {
        m_nAlignRestrict = nAlign;
    }

    void SetAlignRestrictType(unsigned char nType)
    {
        m_nAlignRestrictType = nType;
    }

    void SetPlayerClass(int bPlayerClass)
    {
        m_bPlayerClass = bPlayerClass;
    }

    void SetSpellCaster(int bSpellCaster)
    {
        m_bSpellCaster = bSpellCaster;
    }

    void SetEffectiveCRLevel(unsigned char nLevel, unsigned char nCR)
    {
        m_aEffectiveCRLevel[nLevel] = nCR;
    }

    unsigned long m_nNameStrRef;
    unsigned long m_nNameLowerStrRef;
    unsigned long m_nNamePluralStrRef;
    unsigned long m_nDescriptionStrRef;
    CExoString m_sIcon;
    unsigned char m_aAttackBonus[60];
    unsigned char m_nHitDie;
    unsigned char m_nForceDie;
    unsigned char m_nSkillPointBase;
    unsigned char m_aFortSaveBonus[60];
    unsigned char m_aWillSaveBonus[60];
    unsigned char m_aRefSaveBonus[60];
    unsigned char m_aUnknown10B[1];
    unsigned char* m_pArmorBonus;
    unsigned char** m_ppSpellsKnown;
    unsigned char m_aNumSpellLevels[50];
    unsigned char m_aSpellGain[50];
    unsigned char m_aBonusFeatGain[MAX_FEAT_GAIN_LEVELS];
    unsigned char m_aFeatGain[MAX_FEAT_GAIN_LEVELS];
    CSWClassSkill* m_pSkills;
    unsigned short m_nNumSkills;
    CSWClassFeat* m_pFeats;
    unsigned short m_nNumFeats;
    CSWClassSpell* m_pSpells;
    unsigned short m_nNumSpells;
    unsigned char m_nPrimaryAbility;
    unsigned char m_nStrength;
    unsigned char m_nDexterity;
    unsigned char m_nConstitution;
    unsigned char m_nIntelligence;
    unsigned char m_nWisdom;
    unsigned char m_nCharisma;
    unsigned char m_nAlignRestrict;
    unsigned char m_nAlignRestrictType;
    unsigned char m_aEffectiveCRLevel[51];
    int m_bPlayerClass;
    int m_bSpellCaster;
};

extern CSWRules* g_pRules;

#endif
