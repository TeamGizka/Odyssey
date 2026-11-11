#include "nwfeat.h"
#include "exotlktable.h"
// PC 0x00619DF0
CSWFeat::CSWFeat()
{
    m_nNameStrRef = -1;
    m_nDescriptionStrRef = -1;
    m_nMinAttackBonus = 0;
    m_nMinStrength = 0;
    m_nMinDexterity = 0;
    m_nMinIntelligence = 0;
    m_nMinWisdom = 0;
    m_nPrereqFeat1 = -1;
    m_nPrereqFeat2 = -1;
    m_nRequiredSkill = -1;
    for (int i = 0; i < MAX_OR_PREREQ_FEATS; i++)
    {
        m_lstOrPrereqFeats[i] = -1;
    }
    m_nMinSpellLevel = 0;
    m_nUsesPerDay = 0;
    m_nSpellId = -1;
    m_nSuccessor = -1;
    m_bGainMultiple = 0;
    m_bAllClassesCanUse = 0;
    m_bTargetSelf = 0;
    m_bHostileFeat = 0;
    m_bAvailable = 1;
}
// PC 0x004015A0
CSWFeat::~CSWFeat()
{
}
// PC 0x00619E70
CExoString CSWFeat::GetNameText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nNameStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x00619F10
CExoString CSWFeat::GetDescriptionText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nDescriptionStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
