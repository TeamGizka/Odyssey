#include "exobase.h"
#include "nwcreaturelevelstats.h"
#include "nwclass.h"
#include <string.h>
#include "nwrules.h"
// PC 0x00617400 GOG 0x006173E0
unsigned char CSWLevelStats::GetSkillRankChange(unsigned short nSkill)
{
    if (nSkill >= g_pRules->m_nNumSkills)
    {
        return 0;
    }
    return m_pSkillRankChanges[nSkill];
}
// PC 0x00617430 GOG 0x00617410
void CSWLevelStats::SetSkillRankChange(unsigned short nSkill, char nChange)
{
    if (nSkill < g_pRules->m_nNumSkills)
    {
        m_pSkillRankChanges[nSkill] = nChange;
    }
}
// PC 0x00617460 GOG 0x00617440
void CSWLevelStats::AddFeat(unsigned short nFeat)
{
    if (m_lstFeats.Contains(nFeat) == 0)
    {
        m_lstFeats.Add(nFeat);
    }
}
// PC 0x006174C0 GOG 0x006174A0
CSWLevelStats::CSWLevelStats()
{
    m_pSkillRankChanges = new char[g_pRules->m_nNumSkills];
    memset(m_pSkillRankChanges, 0, g_pRules->m_nNumSkills);
    m_nHitDie = 0;
    m_nClass = 0;
    m_nForceDie = 0;
    m_nSkillPoints = 0;
    m_nAbilityIncrease = 6;
}
// PC 0x00617580 GOG 0x00617560
CSWLevelStats::~CSWLevelStats()
{
    if (m_pSkillRankChanges != NULL)
    {
        delete[] m_pSkillRankChanges;
        m_pSkillRankChanges = NULL;
    }
    m_lstFeats.SetSize(0);
    m_lstSpellsAdded[0].SetSize(0);
    m_lstSpellsRemoved[0].SetSize(0);
}
