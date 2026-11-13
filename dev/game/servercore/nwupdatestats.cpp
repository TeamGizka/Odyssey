#include "exobase.h"
#include "nwupdatestats.h"
#include <string.h>
// PC 0x00617160 GOG 0x00617140
void CSWCreatureStatsUpdate::SetCombatInformation(CCombatInformation* pInfo)
{
    *m_pCombatInformation = *pInfo;
}
// PC 0x006171D0 GOG 0x006171B0
CSWCreatureStatsUpdate::CSWCreatureStatsUpdate()
{
    m_nExperience = -20;
    m_n42 = -20;
    m_n44 = -20;
    m_nStrengthBase = 0xEC;
    m_nDexterityBase = 0xEC;
    m_nConstitutionBase = 0xEC;
    m_nIntelligenceBase = 0xEC;
    m_nWisdomBase = 0xEC;
    m_nCharismaBase = 0xEC;
    m_nStrength = 0xEC;
    m_nDexterity = 0xEC;
    m_nConstitution = 0xEC;
    m_nIntelligence = 0xEC;
    m_nWisdom = 0xEC;
    m_nCharisma = 0xEC;
    m_nDexterityModifierBonus = -20;
    m_nArmorClass = -20;
    m_nStrengthBonus = -20;
    m_nDexterityBonus = -20;
    m_nConstitutionBonus = -20;
    m_nIntelligenceBonus = -20;
    m_nWisdomBonus = -20;
    m_nCharismaBonus = -20;
    m_nFortitudeSave = -20;
    m_nWillSave = -20;
    m_nReflexSave = -20;
    m_pSkillRanks = 0;
    m_bUseMonkAttackTables = 0;
    m_pCombatInformation = new CCombatInformation;
    if (g_pRules->m_nNumSkills != 0)
    {
        m_pSkillRanks = new unsigned char[g_pRules->m_nNumSkills];
        memset(m_pSkillRanks, 0xEC, g_pRules->m_nNumSkills);
    }
}
// PC 0x00617300 GOG 0x006172E0
CSWCreatureStatsUpdate::~CSWCreatureStatsUpdate()
{
    unsigned char* pBuffer = m_pSkillRanks;
    m_lstFeats.num = 0;
    m_lstBonusFeats.num = 0;
    if (pBuffer != 0)
    {
        delete pBuffer;
    }
    if (m_pCombatInformation != 0)
    {
        delete m_pCombatInformation;
        m_pCombatInformation = 0;
    }
    ClearEffectIcons();
}
// PC 0x00617170 GOG 0x00617150
void CSWCreatureStatsUpdate::ClearEffectIcons()
{
    for (int i = m_lstEffectIcons.num - 1; i >= 0; i--)
    {
        if (m_lstEffectIcons[i] != 0)
        {
            delete m_lstEffectIcons[i];
            m_lstEffectIcons[i] = 0;
        }
        m_lstEffectIcons.DelIndex(i);
    }
}
