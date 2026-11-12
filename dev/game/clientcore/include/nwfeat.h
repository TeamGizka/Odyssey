#ifndef __NWFEAT_H__
#define __NWFEAT_H__

#include "exobase.h"
#include "exoresman.h"

class CSWFeat
{
public:

    enum
    {
        FEAT_ADVANCED_GUARD_STANCE = 2,
        FEAT_ADVANCED_JEDI_DEFENSE = 1,
        FEAT_MASTER_JEDI_DEFENSE = 24,
        FEAT_JEDI_DEFENSE = 55,
        FEAT_AMBIDEXTERITY = 3,
        FEAT_ARMOUR_PROF_HEAVY = 4,
        FEAT_ARMOUR_PROF_LIGHT = 5,
        FEAT_ARMOUR_PROF_MEDIUM = 6,
        FEAT_SNEAK_ATTACK_1D6 = 60,
        FEAT_SNEAK_ATTACK_2D6 = 61,
        FEAT_SNEAK_ATTACK_3D6 = 62,
        FEAT_SNEAK_ATTACK_4D6 = 63,
        FEAT_SNEAK_ATTACK_5D6 = 64,
        FEAT_SNEAK_ATTACK_6D6 = 65,
        FEAT_SNEAK_ATTACK_7D6 = 66,
        FEAT_SNEAK_ATTACK_8D6 = 67,
        FEAT_SNEAK_ATTACK_9D6 = 68,
        FEAT_SNEAK_ATTACK_10D6 = 69,
        FEAT_FORCE_SENSITIVE = 116,
        FEAT_CRITICAL_STRIKE = 8,
        FEAT_DOUBLE_WEAPON_FIGHTING = 9,
        FEAT_EMPATHY = 10,
        FEAT_FLURRY = 11,
        FEAT_GEAR_HEAD = 12,
        FEAT_GREAT_FORTITUDE = 13,
        FEAT_IMPROVED_POWER_ATTACK = 17,
        FEAT_IMPROVED_POWER_BLAST = 18,
        FEAT_IMPROVED_CRITICAL_STRIKE = 19,
        FEAT_IMPROVED_SNIPER_SHOT = 20,
        FEAT_IRON_WILL = 21,
        FEAT_LIGHTNING_REFLEXES = 22,
        FEAT_MASTER_GUARD_STANCE = 25,
        FEAT_MULTI_SHOT = 26,
        FEAT_POWER_ATTACK = 28,
        FEAT_POWER_BLAST = 29,
        FEAT_RAPID_SHOT = 30,
        FEAT_SNIPER_SHOT = 31,
        FEAT_WHIRLWIND_ATTACK = 53,
        FEAT_GUARD_STANCE = 54,
        FEAT_MASTER_SNIPER_SHOT = 77,
        FEAT_MASTER_CRITICAL_STRIKE = 81,
        FEAT_MASTER_POWER_BLAST = 82,
        FEAT_MASTER_POWER_ATTACK = 83,
        FEAT_TOUGHNESS = 84,
        FEAT_ADVANCED_DOUBLE_WEAPON_FIGHTING = 85,
        FEAT_IMPROVED_FLURRY = 91,
        FEAT_IMPROVED_RAPID_SHOT = 92,

        FEAT_CAUTIOUS = 7,
        FEAT_FORCE_FOCUS_SENSE = 88,
        FEAT_FORCE_FOCUS_ADVANCED = 89,
        FEAT_FORCE_FOCUS_MASTERY = 90,
        FEAT_PROFICIENCY_ALL = 93
    };

    enum
    {
        FEAT_WOOKIE_ENDURANCE = 95,
        FEAT_CAUTIOUS_2 = 117,
        FEAT_CAUTIOUS_3 = 118,
        FEAT_GEAR_HEAD_2 = 119,
        FEAT_GEAR_HEAD_3 = 120,
        FEAT_EMPATHY_2 = 121,
        FEAT_EMPATHY_3 = 122,
        FEAT_IMPROVED_TOUGHNESS = 123,
        FEAT_MASTER_TOUGHNESS = 124
    };

    enum RangedAttackACPenalty
    {
        MULTI_SHOT_AC_PENALTY = 1,
        RAPID_SHOT_AC_PENALTY = 4,
        IMPROVED_RAPID_SHOT_AC_PENALTY = 2,
        SNIPER_SHOT_AC_PENALTY = 5
    };

    enum MeleeAttackACPenalty
    {
        CRITICAL_STRIKE_AC_PENALTY = 5,
        FLURRY_AC_PENALTY = 4,
        WHIRLWIND_ATTACK_AC_PENALTY = 1,
        IMPROVED_FLURRY_AC_PENALTY = 2
    };

    enum
    {

        FEAT_FORCE_IMMUNITY_FEAR = 98,
        FEAT_FORCE_IMMUNITY_STUN = 99,
        FEAT_FORCE_IMMUNITY_PARALYSIS = 100,
        FEAT_FORCE_JUMP = 101,
        FEAT_FORCE_JUMP_ADVANCED = 102,
        FEAT_FORCE_JUMP_MASTERY = 103
    };

    enum
    {
        FEAT_DUELING = 113,
        FEAT_IMPROVED_DUELING = 114,
        FEAT_MASTER_DUELING = 115
    };

    static const unsigned short FEAT_INVALID = 0xFFFF;

    CSWFeat();

    enum
    {

        MAX_OR_PREREQ_FEATS = 5,
        MASTER_FEAT_NONE = 0xFF,

        MAX_FEAT_USES = 100,

        USES_PER_DAY_UNLIMITED = -1
    };

    ~CSWFeat();

    CExoString GetNameText();
    CExoString GetDescriptionText();

    void SetSuccessor(long nFeat)
    {
        m_nSuccessor = nFeat;
    }

    void SetSpellId(long nSpellId)
    {
        m_nSpellId = nSpellId;
    }

    void SetNameStrRef(unsigned long nStrRef)
    {
        m_nNameStrRef = nStrRef;
    }

    void SetDescriptionStrRef(unsigned long nStrRef)
    {
        m_nDescriptionStrRef = nStrRef;
    }

    void SetCategory(int nCategory)
    {
        m_nCategory = nCategory;
    }

    void SetMaxCR(int nMaxCR)
    {
        m_nMaxCR = nMaxCR;
    }

    void SetIcon(const CResRef& cIcon)
    {
        m_cIcon = cIcon;
    }

    void SetGainMultiple(int bGainMultiple)
    {
        m_bGainMultiple = bGainMultiple;
    }

    void SetAllClassesCanUse(int bAllClassesCanUse)
    {
        m_bAllClassesCanUse = bAllClassesCanUse;
    }

    void SetTargetSelf(int bTargetSelf)
    {
        m_bTargetSelf = bTargetSelf;
    }

    void SetHostileFeat(int bHostileFeat)
    {
        m_bHostileFeat = bHostileFeat;
    }

    void SetAvailable(int bAvailable)
    {
        m_bAvailable = bAvailable;
    }

    void SetMinAttackBonus(unsigned char nBonus)
    {
        m_nMinAttackBonus = nBonus;
    }

    void SetMinCharLevel(unsigned char nLevel)
    {
        m_nMinCharLevel = nLevel;
    }

    void SetMinStrength(unsigned char nStrength)
    {
        m_nMinStrength = nStrength;
    }

    void SetMinDexterity(unsigned char nDexterity)
    {
        m_nMinDexterity = nDexterity;
    }

    void SetMinIntelligence(unsigned char nIntelligence)
    {
        m_nMinIntelligence = nIntelligence;
    }

    void SetMinWisdom(unsigned char nWisdom)
    {
        m_nMinWisdom = nWisdom;
    }

    void SetMinSpellLevel(unsigned char nLevel)
    {
        m_nMinSpellLevel = nLevel;
    }

    void SetUsesPerDay(unsigned char nUses)
    {
        m_nUsesPerDay = nUses;
    }

    void SetMasterFeat(unsigned char nMasterFeat)
    {
        m_nMasterFeat = nMasterFeat;
    }

    void SetPrereqFeat1(unsigned short nFeat)
    {
        m_nPrereqFeat1 = nFeat;
    }

    void SetPrereqFeat2(unsigned short nFeat)
    {
        m_nPrereqFeat2 = nFeat;
    }

    void SetOrPrereqFeat(unsigned char nIndex, unsigned short nFeat)
    {
        if (nIndex < MAX_OR_PREREQ_FEATS)
        {
            m_lstOrPrereqFeats[nIndex] = nFeat;
        }
    }

    unsigned short GetOrPrereqFeat(unsigned char nIndex)
    {
        if (nIndex < 5)
        {
            return m_lstOrPrereqFeats[nIndex];
        }
        return FEAT_INVALID;
    }

    void SetRequiredSkill(unsigned short nSkill)
    {
        m_nRequiredSkill = nSkill;
    }

    long m_nSuccessor;
    long m_nSpellId;
    unsigned long m_nNameStrRef;
    unsigned long m_nDescriptionStrRef;
    int m_nCategory;
    int m_nMaxCR;
    CResRef m_cIcon;
    unsigned long m_bGainMultiple : 1;
    unsigned long m_bAllClassesCanUse : 1;
    unsigned long m_bTargetSelf : 1;
    unsigned long m_bHostileFeat : 1;
    unsigned long m_bAvailable : 1;
    unsigned long m_nOtherFlags : 27;
    unsigned char m_nMinAttackBonus;
    unsigned char m_nMinCharLevel;
    unsigned char m_nMinStrength;
    unsigned char m_nMinDexterity;
    unsigned char m_nMinIntelligence;
    unsigned char m_nMinWisdom;
    unsigned char m_nMinSpellLevel;
    unsigned char m_nUsesPerDay;
    unsigned char m_nMasterFeat;
    unsigned char m_aUnmapped35[3];
    unsigned short m_nPrereqFeat1;

    unsigned short GetPrereqFeat1() { return m_nPrereqFeat1; }
    unsigned short m_nPrereqFeat2;

    unsigned short GetPrereqFeat2() { return m_nPrereqFeat2; }
    unsigned short m_lstOrPrereqFeats[MAX_OR_PREREQ_FEATS];
    unsigned short m_nRequiredSkill;
    unsigned char m_bHandmaiden;
    unsigned char m_bBaoDur;
    unsigned char m_bHanharr;
    unsigned char m_bHK47;
    unsigned char m_bG0T0;
    unsigned char m_bAtton;
    unsigned char m_bKreia;
};

class CFeatUseListEntry
{
public:
    unsigned short m_nFeat;
    unsigned char m_nUses;
};

#endif
