#include "nwclass.h"
#include "nwskill.h"
#include "exotlktable.h"
#include "twodimarray.h"
#include "aurmodel.h"
#include "nwrules.h"

const unsigned char CSWClass::CCLASS_SOLDIER = 0;
const unsigned char CSWClass::CCLASS_SCOUT = 1;
const unsigned char CSWClass::CCLASS_SCOUNDREL = 2;
const unsigned char CSWClass::CCLASS_JEDIGUARDIAN = 3;
const unsigned char CSWClass::CCLASS_JEDICONSULAR = 4;
const unsigned char CSWClass::CCLASS_JEDISENTINEL = 5;
const unsigned char CSWClass::CCLASS_COMBATDROID = 6;
const unsigned char CSWClass::CCLASS_EXPERTDROID = 7;
// PC 0x0060CD40
CSWClass::CSWClass()
{
    m_nNameStrRef = -1;
    m_nNamePluralStrRef = -1;
    m_nDescriptionStrRef = -1;
    for (int n = 0; n < 60; n++)
    {
        m_aAttackBonus[n] = 0;
    }
    for (int i = 0; i < 50; i++)
    {
        m_aSpellGain[i] = 0;
        m_aEffectiveCRLevel[i] = 0;
        m_aBonusFeatGain[i] = 0;
        m_aFeatGain[i] = 0;
    }
    for (int j = 0; j < 6; j++)
    {
        (&m_nStrength)[j] = 0;
    }
    memset(m_aFortSaveBonus, 0, sizeof(m_aFortSaveBonus));
    memset(m_aWillSaveBonus, 0, sizeof(m_aWillSaveBonus));
    memset(m_aRefSaveBonus, 0, sizeof(m_aRefSaveBonus));
    memset(m_aNumSpellLevels, 0, sizeof(m_aNumSpellLevels));
    memset(m_aSpellGain, 0, sizeof(m_aSpellGain));
    m_pSkills = 0;
    m_ppSpellsKnown = 0;
    m_pSpells = 0;
    m_pArmorBonus = 0;
    m_nHitDie = 0;
    m_nNumSkills = 0;
    m_bPlayerClass = 0;
    m_pFeats = 0;
    m_nNumFeats = 0;
    m_bSpellCaster = 0;
    m_nPrimaryAbility = 0;
    m_nAlignRestrict = 0;
    m_nAlignRestrictType = 0;
}
// PC 0x0060CE30
CSWClass::~CSWClass()
{
    if (m_ppSpellsKnown != 0)
    {
        for (int i = 0; i < 50; i++)
        {
            if (m_ppSpellsKnown[i] != 0)
            {
                delete[] m_ppSpellsKnown[i];
                m_ppSpellsKnown[i] = 0;
            }
        }
        delete[] m_ppSpellsKnown;
        m_ppSpellsKnown = 0;
    }
    if (m_pSkills != 0)
    {
        delete[] m_pSkills;
        m_pSkills = 0;
    }
    if (m_pFeats != 0)
    {
        delete[] m_pFeats;
        m_pFeats = 0;
    }
    if (m_pArmorBonus != 0)
    {
        delete[] m_pArmorBonus;
        m_pArmorBonus = 0;
    }
    if (m_pSpells != 0)
    {
        delete[] m_pSpells;
        m_pSpells = 0;
    }
}
// PC 0x0060CF30
unsigned char CSWClass::GetAttackBonus(unsigned char nLevel)
{
    if (nLevel >= 1 && nLevel <= 60)
    {
        return m_aAttackBonus[nLevel - 1];
    }
    return 0;
}
// PC 0x0060CF50
unsigned char CSWClass::GetFortSaveBonus(unsigned char nLevel)
{
    if (nLevel >= 1 && nLevel <= 60)
    {
        return m_aFortSaveBonus[nLevel - 1];
    }
    return 0;
}
// PC 0x0060CF70
unsigned char CSWClass::GetRefSaveBonus(unsigned char nLevel)
{
    if (nLevel >= 1 && nLevel <= 60)
    {
        return m_aRefSaveBonus[nLevel - 1];
    }
    return 0;
}
// PC 0x0060CF90
unsigned char CSWClass::GetWillSaveBonus(unsigned char nLevel)
{
    if (nLevel >= 1 && nLevel <= 60)
    {
        return m_aWillSaveBonus[nLevel - 1];
    }
    return 0;
}
// PC 0x0060CFB0
unsigned char CSWClass::GetSpellGain(unsigned char nLevel,
                                     unsigned char nSpellLevel)
{
    if (nLevel >= 1 && nLevel <= 50 &&
        nSpellLevel < m_aNumSpellLevels[nLevel - 1])
    {
        return m_aSpellGain[nLevel - 1];
    }
    return -1;
}
// PC 0x0060CFE0
unsigned char CSWClass::GetSpellsKnownPerLevel(unsigned char nLevel,
                                               unsigned char nSpellLevel,
                                               unsigned char nClass,
                                               unsigned short nSpell,
                                               unsigned char nMulti)
{
    if (nLevel >= 1 && nLevel <= 50 &&
        nSpellLevel < m_aNumSpellLevels[nLevel - 1] && m_ppSpellsKnown != 0)
    {
        return m_ppSpellsKnown[nLevel - 1][nSpellLevel];
    }
    return 0;
}
// PC 0x0060D020
void CSWClass::LoadAttackBonusTable(const CExoString& sTable)
{
    int nValue;
    CExoString sColumn;
    C2DA* p2DA = new C2DA(sTable, 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError;
        sError =
            CExoString("CSWClass::LoadAttackBonusTable: Can't load") + sTable;
        sError = sError + " 2DA file";
        EXOXBOXASSERT(0);
    }

    for (int i = 0; i < 60; i++)
    {
        int bFound = p2DA->GetINTEntry(i, "BAB", &nValue);
        m_aAttackBonus[i] = (bFound == 0) ? 0 : nValue;
    }

    delete p2DA;
}
// PC 0x0060D1F0
void CSWClass::LoadFeatGain(const CExoString& sTable)
{
    int nValue;
    C2DA* p2DA = new C2DA("featgain", 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError("CSWClass::LoadFeatGain: can't load featgain.2da");
        EXOXBOXASSERT(0);
    }

    CExoString sRegular = sTable + "_REG";
    CExoString sBonus = sTable + "_BON";
    for (int i = 0; i < 50; i++)
    {
        if (p2DA->GetINTEntry(i, sRegular, &nValue) != 0)
        {
            m_aFeatGain[i] = nValue;
        }
        if (p2DA->GetINTEntry(i, sBonus, &nValue) != 0)
        {
            m_aBonusFeatGain[i] = nValue;
        }
    }

    delete p2DA;
}
// PC 0x0060D370
void CSWClass::LoadFeatsTable(const CExoString& sTable, CSWRules* pRules)
{
    int nValue;
    C2DA* p2DA = new C2DA("Feat", 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError;
        sError = CExoString("CSWClass::LoadFeatTable: Can't load feat.2da");
        sError = sError + " 2DA file";
        EXOXBOXASSERT(0);
    }

    if (m_pFeats != 0)
    {
        delete[] m_pFeats;
        m_pFeats = 0;
    }

    int nNumRows = p2DA->m_nNumRows;
    m_nNumFeats = nNumRows;
    m_pFeats = new CSWClassFeat[m_nNumFeats];

    int nFeat = 0;
    for (int i = 0; i < m_nNumFeats; i++)
    {
        m_pFeats[i].m_nFeat = 0;
        m_pFeats[i].m_nGrantedLevel = 0;
        m_pFeats[i].m_nList = CSWClassFeat::LIST_GRANTED;
        m_pFeats[i].m_nPCGrantedLevel = 0;
        m_pFeats[nFeat].m_nFeat = 0;
        m_pFeats[nFeat].m_nGrantedLevel = 0;
        m_pFeats[nFeat].m_nList = CSWClassFeat::LIST_GRANTED;
        m_pFeats[nFeat].m_nPCGrantedLevel = 0;
        if (pRules->GetFeat(i) != 0)
        {
            p2DA->GetINTEntry(i, sTable + "_List", &nValue);
            switch ((unsigned char)nValue)
            {
                case CSWClassFeat::FEAT_TABLE_NORMAL:
                    m_pFeats[nFeat].m_nList = CSWClassFeat::LIST_NORMAL;
                    break;
                case CSWClassFeat::FEAT_TABLE_NORMAL_AND_BONUS:
                    m_pFeats[nFeat].m_nList =
                        CSWClassFeat::LIST_NORMAL | CSWClassFeat::LIST_BONUS;
                    break;
                case CSWClassFeat::FEAT_TABLE_BONUS:
                    m_pFeats[nFeat].m_nList = CSWClassFeat::LIST_BONUS;
                    break;
                case CSWClassFeat::FEAT_TABLE_GRANTED:
                    m_pFeats[nFeat].m_nList = CSWClassFeat::LIST_GRANTED;
                    break;
                case CSWClassFeat::FEAT_TABLE_UNAVAILABLE:
                    continue;
            }
            m_pFeats[nFeat].m_nFeat = i;
            p2DA->GetINTEntry(i, sTable + "_Granted", &nValue);
            m_pFeats[nFeat].m_nGrantedLevel = nValue;
            p2DA->GetINTEntry(i, sTable + "_PC_Granted", &nValue);
            m_pFeats[nFeat].m_nPCGrantedLevel = nValue;
            p2DA->GetINTEntry(i, sTable + "_Recom", &nValue);
            m_pFeats[nFeat].m_nRecommended = nValue;
            nFeat++;
        }
    }

    delete p2DA;
}
// PC 0x0060D780
void CSWClass::LoadSavingThrowTable(const CExoString& sTable)
{
    int nValue;
    C2DA* p2DA = new C2DA(sTable, 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError;
        sError =
            CExoString("CSWClass::LoadSavingThrowTable: Can't load") + sTable;
        sError = sError + " 2DA file";
        EXOXBOXASSERT(0);
    }

    for (int i = 0; i < 60; i++)
    {
        p2DA->GetINTEntry(i, "FortSave", &nValue);
        m_aFortSaveBonus[i] = nValue;
        p2DA->GetINTEntry(i, "WillSave", &nValue);
        m_aWillSaveBonus[i] = nValue;
        p2DA->GetINTEntry(i, "RefSave", &nValue);
        m_aRefSaveBonus[i] = nValue;
    }

    delete p2DA;
}
// PC 0x0060D9C0
void CSWClass::LoadSkillsTable(const CExoString& sTable)
{
    C2DA* p2DA = new C2DA("skills", 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError;
        sError = CExoString("CSWClass::LoadSkillsTable: Can't load skills.2da");
        EXOXBOXASSERT(0);
    }

    if (m_pSkills != 0)
    {
        delete[] m_pSkills;
        m_pSkills = 0;
    }

    int nNumRows = p2DA->m_nNumRows;
    m_nNumSkills = nNumRows;
    m_pSkills = new CSWClassSkill[m_nNumSkills];

    for (int i = 0; i < m_nNumSkills; i++)
    {
        int nValue;
        m_pSkills[i].m_nSkill = i;
        p2DA->GetINTEntry(i, sTable + "_Class", &nValue);
        m_pSkills[i].m_bClassSkill = nValue;
        int bFound = p2DA->GetINTEntry(i, sTable + "_Reco", &nValue);
        if (bFound != 0)
        {
            m_pSkills[i].m_nRecommended = nValue;
        }
        else
        {
            m_pSkills[i].m_nRecommended = -1;
        }
    }

    delete p2DA;
}
// PC 0x0060DC00
void CSWClass::LoadSpellGainTable(const CExoString& sTable)
{
    int nValue;
    if (sTable.GetLength() >= 1)
    {
        C2DA* pNumLevels = new C2DA("CLS_SPGN_JEDI", 0);
        if (pNumLevels->Load2DArray() == 0)
        {
            CExoString sError;
            sError = CExoString(
                "CSWClass::LoadSpellGainTable: Can't load CLS_SPGN_JEDI");
            sError = sError + " 2DA file";
            EXOXBOXASSERT(0);
        }

        C2DA* pGain = new C2DA("ClassPowerGain", 0);
        if (pGain->Load2DArray() == 0)
        {
            CExoString sError;
            sError = CExoString(
                "CSWClass::LoadSpellGainTable: Can't load ClassPowerGain");
            sError = sError + " 2DA file";
            EXOXBOXASSERT(0);
        }

        for (int i = 0; i < 50; i++)
        {
            pNumLevels->GetINTEntry(i, "NumSpellLevels", &nValue);
            m_aNumSpellLevels[i] = nValue;
            if (m_aNumSpellLevels[i] != 0)
            {
                for (int j = 0; j < m_aNumSpellLevels[i]; j++)
                {
                    if (pGain->GetINTEntry(i, sTable, &nValue) != 0)
                    {
                        m_aSpellGain[i] = nValue;
                    }
                    else
                    {
                        m_aSpellGain[i] = -1;
                    }
                }
            }
        }

        delete pGain;
        delete pNumLevels;
    }
}
// PC 0x0060DF00
void CSWClass::LoadSpellKnownTable(const CExoString& sTable)
{
    int nValue;
    CExoString sColumn;
    C2DA* p2DA = new C2DA(sTable, 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError;
        sError =
            CExoString("CSWClass::LoadSpellKnownTable: Can't load") + sTable;
        sError = sError + " 2DA file";
        EXOXBOXASSERT(0);
    }

    if (m_ppSpellsKnown != 0)
    {
        for (int i = 0; i < 50; i++)
        {
            if (m_ppSpellsKnown[i] != 0)
            {
                delete[] m_ppSpellsKnown[i];
                m_ppSpellsKnown[i] = 0;
            }
        }
        delete[] m_ppSpellsKnown;
        m_ppSpellsKnown = 0;
    }

    m_ppSpellsKnown = new unsigned char*[50];
    for (int nLevel = 0; nLevel < 50; nLevel++)
    {
        if (m_aNumSpellLevels[nLevel] != 0)
        {
            m_ppSpellsKnown[nLevel] =
                new unsigned char[m_aNumSpellLevels[nLevel]];
            for (int nSpellLevel = 0; nSpellLevel < m_aNumSpellLevels[nLevel];
                 nSpellLevel++)
            {
                sColumn.Format("SpellLevel%d", nSpellLevel);
                if (p2DA->GetINTEntry(nLevel, sColumn, &nValue) != 0)
                {
                    m_ppSpellsKnown[nLevel][nSpellLevel] = nValue;
                }
                else
                {
                    m_ppSpellsKnown[nLevel][nSpellLevel] = 0;
                }
            }
        }
    }

    delete p2DA;
}
// PC 0x0060E1A0
void CSWClass::LoadArmorClassTable(const CExoString& sTable)
{
    int nValue;
    C2DA* p2DA = new C2DA("ACBonus", 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError;
        sError =
            CExoString("CSWClass::LoadArmorClassTable: Can't load ACBonus");
        sError = sError + " 2DA file";
        EXOXBOXASSERT(0);
    }

    if (m_pArmorBonus != 0)
    {
        delete[] m_pArmorBonus;
        m_pArmorBonus = 0;
    }

    m_pArmorBonus = new unsigned char[51];
    for (int i = 0; i <= 50; i++)
    {
        if (p2DA->GetINTEntry(i, sTable, &nValue) != 0)
        {
            m_pArmorBonus[i] = nValue;
        }
        else
        {
            m_pArmorBonus[i] = 0;
        }
    }

    delete p2DA;
}
// PC 0x0060E330
int CSWClass::IsSkillClassSkill(unsigned short nSkill)
{
    if (nSkill >= g_pRules->m_nNumSkills)
    {
        return 0;
    }
    for (unsigned short i = 0; i < m_nNumSkills; i++)
    {
        if (m_pSkills[i].m_nSkill == nSkill)
        {
            return m_pSkills[i].m_bClassSkill;
        }
    }
    return 0;
}
// PC 0x0060E390
int CSWClass::IsSkillUseable(unsigned short nSkill)
{
    if (nSkill >= g_pRules->m_nNumSkills)
    {
        return 0;
    }
    CSWSkill* pSkill = &g_pRules->m_pSkills[nSkill];
    if (pSkill != 0)
    {
        if (pSkill->m_bAllClassesCanUse)
        {
            return 1;
        }
        for (unsigned short i = 0; i < m_nNumSkills; i++)
        {
            if (m_pSkills[i].m_nSkill == nSkill)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x0060E400
CExoString CSWClass::GetNameText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nNameStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x0060E4A0
CExoString CSWClass::GetNameLowerText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nNameLowerStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x0060E540
unsigned char CSWClass::GetLevelFeatGranted(unsigned short nFeat)
{
    for (unsigned short i = 0; i < m_nNumFeats; i++)
    {
        if (m_pFeats[i].m_nFeat == nFeat &&
            m_pFeats[i].m_nList == CSWClassFeat::LIST_GRANTED)
        {
            return m_pFeats[i].m_nGrantedLevel;
        }
    }
    return -1;
}
// PC 0x0060E5A0
int CSWClass::IsBonusFeat(unsigned short nFeat)
{
    for (unsigned short i = 0; i < m_nNumFeats; i++)
    {
        if (m_pFeats[i].m_nFeat == nFeat)
        {
            return (m_pFeats[i].m_nList & CSWClassFeat::LIST_BONUS) != 0;
        }
    }
    return 0;
}
// PC 0x0060E5F0
int CSWClass::IsNormalFeat(unsigned short nFeat)
{
    for (unsigned short i = 0; i < m_nNumFeats; i++)
    {
        if (m_pFeats[i].m_nFeat == nFeat)
        {
            return m_pFeats[i].m_nList & CSWClassFeat::LIST_NORMAL;
        }
    }
    return 0;
}
// PC 0x0060E640
int CSWClass::IsGrantedFeat(unsigned short nFeat, unsigned char& nLevel)
{
    nLevel = -1;
    for (unsigned short i = 0; i < m_nNumFeats; i++)
    {
        if (m_pFeats[i].m_nFeat == nFeat)
        {
            if (m_pFeats[i].m_nList != CSWClassFeat::LIST_GRANTED)
            {
                return 0;
            }
            nLevel = m_pFeats[i].m_nGrantedLevel;
            return 1;
        }
    }
    return 0;
}
// PC 0x0060E700
int CSWClass::IsFeatUseable(unsigned short nFeat)
{
    if (g_pRules->GetFeat(nFeat) == 0)
    {
        return 0;
    }
    for (unsigned short i = 0; i < m_nNumFeats; i++)
    {
        if (m_pFeats[i].m_nFeat == nFeat)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x0060E760
int CSWClass::GetRecommendedFeat(int nIndex)
{
    int nCount = -1;
    int nFeat = -1;
    for (unsigned short i = 0; i < m_nNumFeats; i++)
    {
        int nRecommended = m_pFeats[i].m_nRecommended;
        if (nRecommended == nIndex)
        {
            return m_pFeats[i].m_nFeat;
        }
        if (nRecommended == 0 && nCount != nIndex)
        {
            nCount++;
            nFeat = m_pFeats[i].m_nFeat;
        }
    }
    return nFeat;
}
// PC 0x0060E7D0
int CSWClass::GetRecommendedSkill(int nIndex)
{
    int nCount = -1;
    int nSkill = -1;
    for (unsigned short i = 0; i < m_nNumSkills; i++)
    {
        int nRecommended = m_pSkills[i].m_nRecommended;
        if (nRecommended == nIndex)
        {
            return m_pSkills[i].m_nSkill;
        }
        if (nRecommended == 0 && nCount != nIndex)
        {
            nCount++;
            nSkill = m_pSkills[i].m_nSkill;
        }
    }
    return nSkill;
}
// PC 0x0060E840
int CSWClass::IsJedi(int nClass)
{
    return (nClass == CCLASS_JEDIGUARDIAN) || ((nClass == CCLASS_JEDICONSULAR) || (nClass == CCLASS_JEDISENTINEL));
}
// PC 0x0060E880
void CSWClass::LoadSpellsTable()
{
    C2DA* p2DA = new C2DA("spells", 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError;
        sError = CExoString("CSWClass::LoadSpellsTable: Can't load spells.2da");
        EXOXBOXASSERT(0);
    }

    if (m_pSpells != 0)
    {
        delete[] m_pSpells;
        m_pSpells = 0;
    }

    int nNumRows = p2DA->m_nNumRows;
    m_nNumSpells = nNumRows;
    m_pSpells = new CSWClassSpell[m_nNumSpells];

    for (int i = 0; i < m_nNumSpells; i++)
    {
        int nValue;
        m_pSpells[i].m_nSpell = i;
        p2DA->GetINTEntry(i, "Light_Recom", &nValue);
        m_pSpells[i].m_nLightRecommended = nValue;
        p2DA->GetINTEntry(i, "Dark_Recom", &nValue);
        m_pSpells[i].m_nDarkRecommended = nValue;
        p2DA->GetINTEntry(i, "UserType", &nValue);
        m_pSpells[i].m_nUserType = nValue;
    }

    delete p2DA;
}
// PC 0x0060EA90
int CSWClass::GetRecommendedSpell(int nSide, int nIndex)
{
    int nSpell = -1;
    int nCount = 0;
    for (unsigned short i = 0; i < m_nNumSpells; i++)
    {
        if (m_pSpells[i].m_nUserType == 1 ||
            m_pSpells[i].m_nUserType == 5)
        {
            if (nSide != 0)
            {
                int nRecommended = m_pSpells[i].m_nLightRecommended;
                if (nRecommended == nIndex)
                    return m_pSpells[i].m_nSpell;
                if (nRecommended == 0 && nCount != nIndex)
                {
                    nSpell = m_pSpells[i].m_nSpell;
                    ++nCount;
                }
            }
            else
            {
                int nRecommended = m_pSpells[i].m_nDarkRecommended;
                if (nRecommended == nIndex)
                    return m_pSpells[i].m_nSpell;
                if (nRecommended == 0 && nCount != nIndex)
                {
                    nSpell = m_pSpells[i].m_nSpell;
                    ++nCount;
                }
            }
        }
    }
    return nSpell;
}
// PC 0x0060EB30
char CSWClass::GetArmorBonus(int nLevel)
{
    return m_pArmorBonus[nLevel];
}
