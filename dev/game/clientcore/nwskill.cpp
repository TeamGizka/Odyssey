#include "nwskill.h"
#include "clientexoappinternal.h"
#include "exotlktable.h"
// PC 0x00619FE0 GOG 0x00619FC0
CSWSkill::~CSWSkill()
{
}
// PC 0x00619FB0 GOG 0x00619F90
CSWSkill::CSWSkill()
{
    m_bHostileSkill = 0;
    m_bUntrained = 0;
    m_bArmorCheckPenalty = 0;
    m_bAllClassesCanUse = 1;
    m_bNPCCanUse = 1;
    m_bDroidCanUse = 1;
    m_nNameStrRef = -1;
    m_nDescriptionStrRef = -1;
}
// PC 0x00619FF0 GOG 0x00619FD0
CExoString CSWSkill::GetNameText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nNameStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x0061A090 GOG 0x0061A070
CExoString CSWSkill::GetDescriptionText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nDescriptionStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
