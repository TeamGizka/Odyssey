#ifndef __NWSKILL_H__
#define __NWSKILL_H__

#include "exobase.h"

class CSWSkill
{
public:

    static const unsigned short SKILL_INVALID = 0xFFFF;

    CSWSkill();
    ~CSWSkill();

    enum KeyAbility
    {
        KEY_ABILITY_STRENGTH = 0,
        KEY_ABILITY_DEXTERITY = 1,
        KEY_ABILITY_CONSTITUTION = 2,
        KEY_ABILITY_INTELLIGENCE = 3,
        KEY_ABILITY_WISDOM = 4,
        KEY_ABILITY_CHARISMA = 5
    };

    enum
    {
        FLAG_UNTRAINED = 2,
        FLAG_ALL_CLASSES_CAN_USE = 8,

        FLAG_PLAYER_USABLE = 16,
        FLAG_DROID_USABLE = 32
    };

    enum SkillSelector
    {

        SKILL_COMPUTER_USE = 0,
        SKILL_DEMOLITIONS = 1,

        SKILL_STEALTH = 2,
        SKILL_AWARENESS = 3,
        SKILL_PERSUADE = 4,
        SKILL_REPAIR = 5,
        SKILL_SECURITY = 6,
        SKILL_TREAT_INJURY = 7,

        NUM_SKILLS = 8,
        SUBSKILL_FLAGTRAP = 100,
        SUBSKILL_RECOVERTRAP = 101,
        SUBSKILL_EXAMINETRAP = 102
    };

    CExoString GetNameText();
    CExoString GetDescriptionText();

    unsigned long GetNameStrRef() { return m_nNameStrRef; }

    void SetNameStrRef(unsigned long value)
    {
        m_nNameStrRef = value;
    }

    void SetDescriptionStrRef(unsigned long value)
    {
        m_nDescriptionStrRef = value;
    }

    void SetIcon(const CExoString& value)
    {
        m_sIcon = value;
    }

    void SetKeyAbility(int value)
    {
        m_nKeyAbility = value;
    }

    void SetCategory(int value)
    {
        m_nCategory = value;
    }

    void SetMaxCR(int value)
    {
        m_nMaxCR = value;
    }

    void SetHostileSkill(int value)
    {
        m_bHostileSkill = value;
    }

    void SetUntrained(int value)
    {
        m_bUntrained = value;
    }

    void SetArmorCheckPenalty(int value)
    {
        m_bArmorCheckPenalty = value;
    }

    void SetAllClassesCanUse(int value)
    {
        m_bAllClassesCanUse = value;
    }

    void SetNPCCanUse(int value)
    {
        m_bNPCCanUse = value;
    }

    void SetDroidCanUse(int value)
    {
        m_bDroidCanUse = value;
    }

    unsigned long m_nNameStrRef;
    unsigned long m_nDescriptionStrRef;

    int m_nCategory;
    int m_nMaxCR;
    CExoString m_sIcon;
    int m_nKeyAbility;
    unsigned long m_bHostileSkill : 1;
    unsigned long m_bUntrained : 1;
    unsigned long m_bArmorCheckPenalty : 1;
    unsigned long m_bAllClassesCanUse : 1;
    unsigned long m_bNPCCanUse : 1;
    unsigned long m_bDroidCanUse : 1;
};

#endif
