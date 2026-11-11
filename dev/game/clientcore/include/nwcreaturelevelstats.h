#ifndef __NWCREATURELEVELSTATS_H__
#define __NWCREATURELEVELSTATS_H__

#include "exobase.h"

class CSWLevelStats
{
public:
    CSWLevelStats();
    ~CSWLevelStats();

    unsigned char GetSkillRankChange(unsigned short nSkill);
    void SetSkillRankChange(unsigned short nSkill, char nChange);
    void AddFeat(unsigned short nFeat);

    enum { NO_ABILITY_INCREASE = 6 };
    unsigned char GetAbilityIncrease() { return m_nAbilityIncrease; }
    unsigned char GetHitDie() { return m_nHitDie; }
    unsigned char GetForceDie() { return m_nForceDie; }
    unsigned char GetClass() { return m_nClass; }
    unsigned short GetSkillPoints() { return m_nSkillPoints; }
    unsigned long GetKnownSpell(unsigned char nLevel, unsigned char nSpell)
    {
        return m_lstSpellsAdded[nLevel][nSpell];
    }
    unsigned long GetRemovedSpell(unsigned char nLevel, unsigned char nSpell)
    {
        return m_lstSpellsRemoved[nLevel][nSpell];
    }

    CExoArrayList<unsigned long> m_lstSpellsAdded[1];
    CExoArrayList<unsigned long> m_lstSpellsRemoved[1];
    CExoArrayList<unsigned short> m_lstFeats;
    char* m_pSkillRankChanges;
    short m_nSkillPoints;
    char m_nAbilityIncrease;

    unsigned char m_nHitDie;
    char m_nClass;
    char m_nForceDie;
};

#endif
