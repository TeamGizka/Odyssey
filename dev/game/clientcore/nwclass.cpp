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
// PC 0x005BCAC0  Xbox 0x00101B60
CSWClass::CSWClass()
{
    m_nNameStrRef = -1;
    m_nNamePluralStrRef = -1;
    m_nDescriptionStrRef = -1;
    for (int n = 0; n < 60; n++)
    {
        m_aAttackBonus[n] = 0;
    }
    for (int i = 0; i < 20; i++)
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
// PC 0x005BCBB0  Xbox 0x00101C50
CSWClass::~CSWClass()
{
    if (m_ppSpellsKnown != 0)
    {
        for (int i = 0; i < 20; i++)
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
// PC 0x005BCCB0  Xbox 0x00101D50
unsigned char CSWClass::GetAttackBonus(unsigned char nLevel)
{
    if (nLevel >= 1 && nLevel <= 60)
    {
        return m_aAttackBonus[nLevel - 1];
    }
    return 0;
}
// PC 0x005BCCD0  Xbox 0x00101D70
unsigned char CSWClass::GetFortSaveBonus(unsigned char nLevel)
{
    if (nLevel >= 1 && nLevel <= 60)
    {
        return m_aFortSaveBonus[nLevel - 1];
    }
    return 0;
}
// PC 0x005BCCF0  Xbox 0x00101D90
unsigned char CSWClass::GetRefSaveBonus(unsigned char nLevel)
{
    if (nLevel >= 1 && nLevel <= 60)
    {
        return m_aRefSaveBonus[nLevel - 1];
    }
    return 0;
}
// PC 0x005BCD10  Xbox 0x00101DB0
unsigned char CSWClass::GetWillSaveBonus(unsigned char nLevel)
{
    if (nLevel >= 1 && nLevel <= 60)
    {
        return m_aWillSaveBonus[nLevel - 1];
    }
    return 0;
}
// PC 0x005BCD30  Xbox 0x00101DD0
unsigned char CSWClass::GetSpellGain(unsigned char nLevel,
                                     unsigned char nSpellLevel)
{
    if (nLevel >= 1 && nLevel <= 20 &&
        nSpellLevel < m_aNumSpellLevels[nLevel - 1])
    {
        return m_aSpellGain[nLevel - 1];
    }
    return -1;
}
// PC 0x005BCD60  Xbox 0x00101E00
unsigned char CSWClass::GetSpellsKnownPerLevel(unsigned char nLevel,
                                               unsigned char nSpellLevel,
                                               unsigned char nClass,
                                               unsigned short nSpell,
                                               unsigned char nMulti)
{
    if (nLevel >= 1 && nLevel <= 20 &&
        nSpellLevel < m_aNumSpellLevels[nLevel - 1] && m_ppSpellsKnown != 0)
    {
        return m_ppSpellsKnown[nLevel - 1][nSpellLevel];
    }
    return 0;
}
// PC 0x005BCDA0  Xbox 0x00101E40
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
// PC 0x005BCF70  Xbox 0x00102010
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
    for (int i = 0; i < 20; i++)
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
// PC 0x005BD0F0  Xbox 0x001021A0
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
        m_pFeats[nFeat].m_nFeat = 0;
        m_pFeats[nFeat].m_nGrantedLevel = 0;
        m_pFeats[nFeat].m_nList = CSWClassFeat::LIST_GRANTED;
        m_pFeats[nFeat].m_nFeat = i;
        if (pRules->GetFeat(m_pFeats[nFeat].m_nFeat) != 0)
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
            p2DA->GetINTEntry(i, sTable + "_Granted", &nValue);
            m_pFeats[nFeat].m_nGrantedLevel = nValue;
            p2DA->GetINTEntry(i, sTable + "_Recom", &nValue);
            m_pFeats[nFeat].m_nRecommended = nValue;
            nFeat++;
        }
    }

    delete p2DA;
}
// PC 0x005BD480  Xbox 0x00102540
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
// PC 0x005BD6C0  Xbox 0x00102780
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
// PC 0x005BD900  Xbox 0x001029D0
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

        for (int i = 0; i < 20; i++)
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
// PC 0x005BDBE0  Xbox 0x00102CC0
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
        for (int i = 0; i < 20; i++)
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

    m_ppSpellsKnown = new unsigned char*[20];
    for (int nLevel = 0; nLevel < 20; nLevel++)
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
// PC 0x005BDE80  Xbox 0x00102F60
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

    m_pArmorBonus = new unsigned char[21];
    for (int i = 0; i <= 20; i++)
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
// PC 0x005BE010  Xbox 0x00103100
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
// PC 0x005BE070  Xbox 0x00103160
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
// PC 0x005BE0E0  Xbox 0x001031D0
CExoString CSWClass::GetNameText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nNameStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x005BE180  Xbox 0x00103280
CExoString CSWClass::GetNameLowerText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nNameLowerStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x005BE220  Xbox 0x00103330
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
// PC 0x005BE270  Xbox 0x00103380
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
// PC 0x005BE2C0  Xbox 0x001033D0
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
// PC 0x005BE310  Xbox 0x00103420
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
// PC 0x005BE370  Xbox 0x00103480
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
// PC 0x005BE3C0  Xbox 0x001034D0
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
// PC 0x005BE430  Xbox 0x00103540
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
// PC 0x005BE4A0  Xbox 0x001035B0
int CSWClass::IsJedi(int nClass)
{
    return (nClass == CCLASS_JEDIGUARDIAN) || ((nClass == CCLASS_JEDICONSULAR) || (nClass == CCLASS_JEDISENTINEL));
}
// PC 0x005BE4C0  Xbox 0x001035D0
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
// PC 0x005BE6D0  Xbox 0x001037F0
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
// PC 0x005BE770  Xbox 0x00103890
char CSWClass::GetArmorBonus(int nLevel)
{
    return m_pArmorBonus[nLevel];
}
