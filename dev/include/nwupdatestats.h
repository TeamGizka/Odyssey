#ifndef __NWUPDATESTATS_H__
#define __NWUPDATESTATS_H__

#include "exobase.h"
#include "nwrules.h"

class CResGFF;
class CResStruct;

class CSWCreatureStatsUpdate
{
public:

    enum { SKILL_UPDATE_BIT = 1 };
    CSWCreatureStatsUpdate();
    ~CSWCreatureStatsUpdate();

    void SetCombatInformation(CCombatInformation* pInfo);
    void ClearEffectIcons();

    CExoArrayList<unsigned short> m_lstFeats;
    CExoArrayList<unsigned short> m_lstBonusFeats;
    unsigned long m_oidCreature;
    unsigned long m_nSkillUpdates;
    CExoArrayList<void*> m_lstEffectIcons;
    unsigned char m_nStrength;
    unsigned char m_nDexterity;
    unsigned char m_nConstitution;
    unsigned char m_nIntelligence;
    unsigned char m_nWisdom;
    unsigned char m_nCharisma;
    unsigned char m_nStrengthBase;
    unsigned char m_nDexterityBase;
    unsigned char m_nConstitutionBase;
    unsigned char m_nIntelligenceBase;
    unsigned char m_nWisdomBase;
    unsigned char m_nCharismaBase;
    signed char m_nStrengthBonus;
    signed char m_nDexterityBonus;
    signed char m_nConstitutionBonus;
    signed char m_nIntelligenceBonus;
    signed char m_nWisdomBonus;
    signed char m_nCharismaBonus;
    signed char m_nDexterityModifierBonus;
    signed char m_nFortitudeSave;
    signed char m_nWillSave;
    signed char m_nReflexSave;
    short m_n42;
    short m_n44;
    char m_aReserved46[2];
    long m_nExperience;
    long m_bUseMonkAttackTables;
    unsigned char m_nBaseAttackBonus;
    char m_aReserved51[1];
    short m_nArmorClass;
    CCombatInformation* m_pCombatInformation;
    unsigned char* m_pSkillRanks;
};

#endif
