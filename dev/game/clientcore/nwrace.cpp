#include "nwrace.h"
#include "clientexoappinternal.h"
#include "exotlktable.h"
#include "twodimarray.h"

const unsigned short CSWRace::CRACE_UNKNOWN = 0;
const unsigned short CSWRace::CRACE_ELF = 1;
const unsigned short CSWRace::CRACE_GNOME = 2;
const unsigned short CSWRace::CRACE_HALFELF = 4;
const unsigned short CSWRace::CRACE_HALFLING = 3;
const unsigned short CSWRace::CRACE_DROID = 5;
const unsigned short CSWRace::CRACE_HUMAN = 6;
const unsigned short CSWRace::CRACE_ALL_RACES = 7;
const unsigned short CSWRace::CRACE_INVALID_RACE = 8;
const unsigned short CSWRace::CRACE_UNKNOWN_MASK = 0x0000;
const unsigned short CSWRace::CRACE_ELF_MASK = 0x0000;
const unsigned short CSWRace::CRACE_GNOME_MASK = 0x0000;
const unsigned short CSWRace::CRACE_HALFELF_MASK = 0x0000;
const unsigned short CSWRace::CRACE_HALFLING_MASK = 0x0000;
const unsigned short CSWRace::CRACE_HUMAN_MASK = 0x0001;
const unsigned short CSWRace::CRACE_DROID_MASK = 0x0002;
const unsigned short CSWRace::CRACE_PLACEABLE_MASK = 0x0004;
const unsigned short CSWRace::CRACE_ALL_RACES_MASK = 0x0000;
const unsigned short CSWRace::CRACE_INVALID_RACE_MASK = 0x0000;
// PC 0x00614D40 GOG 0x00614D20
CSWRace::CSWRace()
{
    m_nNameStrRef = -1;
    m_nNamePluralStrRef = -1;
    m_nDescriptionStrRef = -1;
    m_nBiographyStrRef = -1;
    m_nStrengthAdjust = 0;
    m_nDexterityAdjust = 0;
    m_nIntelligenceAdjust = 0;
    m_nCharismaAdjust = 0;
    m_nWisdomAdjust = 0;
    m_nConstitutionAdjust = 0;
    m_nEnduranceAdjust = 0;
    m_pFirstLevelGrantedFeats = 0;
    m_nFavoredClass = -1;
    m_bPlayerRace = 0;
}
// PC 0x00614D80 GOG 0x00614D60
CSWRace::~CSWRace()
{
    if (m_pFirstLevelGrantedFeats != 0)
    {
        delete m_pFirstLevelGrantedFeats;
        m_pFirstLevelGrantedFeats = 0;
    }
}
// PC 0x00614DA0 GOG 0x00614D80
CExoString CSWRace::GetNameText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nNameStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x00614E40 GOG 0x00614E20
CExoString CSWRace::GetConverNameText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nConverNameStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x00614EE0 GOG 0x00614EC0
CExoString CSWRace::GetConverNameLowerText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nConverNameLowerStrRef, strRes,
                       CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x00614F80 GOG 0x00614F60
void CSWRace::LoadFeatsTable(const CExoString& sTable)
{
    C2DA* p2DA = new C2DA(sTable, 0);
    if (p2DA->Load2DArray() == 0)
    {
        CExoString sError;
        sError = CExoString("CSWRace::LoadFeatTable: Can't load") + sTable;
        sError = sError + " 2DA file";
    }

    if (m_pFirstLevelGrantedFeats != 0)
    {
        delete m_pFirstLevelGrantedFeats;
        m_pFirstLevelGrantedFeats = 0;
    }

    int nNumRows = p2DA->GetNumRows();
    m_nFirstLevelGrantedFeatCount = nNumRows;
    m_pFirstLevelGrantedFeats =
        new unsigned short[m_nFirstLevelGrantedFeatCount];

    for (int i = 0; i < m_nFirstLevelGrantedFeatCount; i++)
    {
        int nFeatIndex;
        m_pFirstLevelGrantedFeats[i] = 0;
        if (p2DA->GetINTEntry(i, "FeatIndex", &nFeatIndex))
        {
            m_pFirstLevelGrantedFeats[i] = nFeatIndex;
        }
    }

    delete p2DA;
}
// PC 0x00615170 GOG 0x00615150
int CSWRace::IsFirstLevelGrantedFeat(unsigned short nFeat)
{
    unsigned short* pFeats = m_pFirstLevelGrantedFeats;
    if (pFeats == 0)
    {
        return 0;
    }
    unsigned short nCount = m_nFirstLevelGrantedFeatCount;
    for (unsigned short i = 0; i < nCount; i++)
    {
        if (pFeats[i] == nFeat)
        {
            return 1;
        }
    }
    return 0;
}
