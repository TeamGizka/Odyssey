#include "nwspell.h"
#include "clientexoappinternal.h"
#include "exotlktable.h"
#include "nwclass.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "nwrules.h"

#define FORCE_COST_REDUCTION_STEP 0.05f
#define FORM_FORCE_COST_MULTIPLIER 1.2f

const int CSWSpell::USERTYPE_NONE = 0;
const int CSWSpell::USERTYPE_FORCEPOWER = 1;
const int CSWSpell::USERTYPE_MONSTERPOWER = 2;
const int CSWSpell::USERTYPE_FEAT = 3;
const int CSWSpell::USERTYPE_ITEM = 4;
const int CSWSpell::USERTYPE_MASTER = 5;
// PC 0x005E1D00
CSWSpell::CSWSpell()
{
    m_nConjureTime = 3000;
    m_resConjureSoundMale = "amb_e08b";
    m_resConjureSoundFemale = "amb_e09a";
    m_resConjureSoundVFX = "";
    m_resCastSound = "";
    m_nTargetType = 0;
    m_sImpactScript = "default";
    m_nProjectile = 0;
    m_resProjectileModel = "sp_sumfire";
    m_nProjectileType = 1;
    m_resProjectileSound = "amb_e14";
    m_resCastHeadVisual = "";
    m_resCastHandVisual = "";
    m_resCastGroundVisual = "";
    m_resConjureHeadVisual = "";
    m_resConjureHandVisual = "";
    m_resConjureGroundVisual = "";
    m_nCounter1 = -1;
    m_nCounter2 = -1;
    m_nProjectileSpawnPoint = 0;
    m_nCastTime = 0;
    m_nProjectileOrientation = 0;
    m_bUnknown11E = 0;
    m_nUnknown120 = 0;
    m_pSubRadialSpells = 0;
    m_nSubRadialSpellCount = 0;
    m_nUnknown134 = 0;
    m_nUserType = USERTYPE_NONE;
    m_nAltMessage = 0;
    m_bHostileSetting = 0;
    m_nExclusion = 0;
    m_nForbidItemMask = 0;
    m_nRequireItemMask = 0;
    m_nItemTargeting = 0;
    m_nFormMask = 0;
    m_nCatchTime = 0;
    m_nCatchAnim = 0;
}
// PC 0x005E1FB0
CSWSpell::~CSWSpell()
{
    if (m_pSubRadialSpells != 0)
    {
        delete m_pSubRadialSpells;
    }
}
// PC 0x005E2050
unsigned char CSWSpell::GetSpellLevel(unsigned char nClass)
{
    switch (nClass)
    {
    case CSWClass::CLASS_INDEX_SOLDIER:
    case CSWClass::CLASS_INDEX_SCOUT:
    case CSWClass::CLASS_INDEX_SCOUNDREL:
        return -1;
    case CSWClass::CLASS_INDEX_JEDIGUARDIAN:
        return m_nGuardianLevel;
    case CSWClass::CLASS_INDEX_JEDICONSULAR:
        return m_nConsularLevel;
    case CSWClass::CLASS_INDEX_JEDISENTINEL:
        return m_nSentinelLevel;
    case CSWClass::CLASS_INDEX_TECHSPECIALIST:
    case CSWClass::CLASS_INDEX_BOUNTYHUNTER:
        return -1;
    case CSWClass::CLASS_INDEX_JEDIWEAPONMASTER:
        return m_nWeaponMasterLevel;
    case CSWClass::CLASS_INDEX_JEDIMASTER:
        return m_nJediMasterLevel;
    case CSWClass::CLASS_INDEX_JEDIWATCHMAN:
        return m_nWatchmanLevel;
    case CSWClass::CLASS_INDEX_SITHMARAUDER:
        return m_nMarauderLevel;
    case CSWClass::CLASS_INDEX_SITHLORD:
        return m_nSithLordLevel;
    case CSWClass::CLASS_INDEX_SITHASSASSIN:
        return m_nAssassinLevel;
    default:
        return m_nInnateLevel;
    }
}
// PC 0x005E20F0
int CSWSpell::GetSubRadialSpell(unsigned char nIndex)
{
    if (nIndex >= m_nSubRadialSpellCount)
    {
        return -1;
    }
    return m_pSubRadialSpells[nIndex];
}
// PC 0x005FF150
CSWSpellArray::CSWSpellArray()
{
    m_nNumSpells = 0;
    m_pSpells = NULL;
}
// PC 0x005E2120
CSWSpell* CSWSpellArray::GetSpell(int nIndex) const
{
    if (m_pSpells != 0 && nIndex >= 0 && nIndex < m_nNumSpells)
    {
        return &m_pSpells[nIndex];
    }
    return 0;
}
// PC 0x005E2150
void CSWSpell::SetPrerequisites(const CExoString& sSpells)
{
    m_sPrerequisites = sSpells;
    int nLength = sSpells.GetLength();
    int nPos = 0;
    for (; nPos < nLength; nPos++)
    {
        if (sSpells[nPos] != '_' && sSpells[nPos] != ':')
        {
            break;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        int nValue = 0;
        m_aPrerequisites[i] = -1;
        if (nPos < nLength)
        {
            while (sSpells[nPos] != '_' && sSpells[nPos] != ':')
            {
                nValue = nValue * 10 + (unsigned char)sSpells[nPos++] - '0';
            }
            nPos++;
            m_aPrerequisites[i] = nValue;
        }
    }
}
// PC 0x005E2500
CSWSpellArray::~CSWSpellArray()
{
    if (m_pSpells != 0)
    {
        delete[] m_pSpells;
        m_pSpells = 0;
        m_nNumSpells = 0;
    }
}
// PC 0x005E2540
void CSWSpellArray::Load()
{
    C2DA* p2DA = new C2DA("SPELLS", 0);
#ifdef _XBOX
    EXOXBOXASSERT(p2DA->Load2DArray());
#else
    p2DA->Load2DArray();
#endif
    m_nNumSpells = p2DA->m_nNumRows;
    m_pSpells = new CSWSpell[m_nNumSpells];

    CExoString sValue;
    for (int i = 0; i < m_nNumSpells; i++)
    {
        int nValue;
        { CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_n00 = i; }
        p2DA->GetINTEntry(i, "Category", &nValue);
        { int nCategory = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nCategory = nCategory; }
        p2DA->GetINTEntry(i, "MaxCR", &nValue);
        { int nMaxCR = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nMaxCR = nMaxCR; }
        p2DA->GetCExoStringEntry(i, "Prerequisites", &sValue);
        if (sValue.Left(1) != "_" && sValue.Left(1) != ":")
        {
            sValue = CExoString("_") + sValue;
        }
        if (sValue.Right(1) != "_" && sValue.Right(1) != ":")
        {
            sValue = sValue + "_";
        }
        m_pSpells[i].SetPrerequisites(sValue);
        p2DA->GetCExoStringEntry(i, "MasterSpell", &sValue);
        if (sValue.Left(1) != "_" && sValue.Left(1) != ":")
        {
            sValue = CExoString("_") + sValue;
        }
        if (sValue.Right(1) != "_" && sValue.Right(1) != ":")
        {
            sValue = sValue + "_";
        }
        m_pSpells[i].SetMasterSpells(sValue);
        p2DA->GetCExoStringEntry(i, "Label", &sValue);
        m_pSpells[i].SetLabel(sValue);
        p2DA->GetINTEntry(i, "Name", &nValue);
        { unsigned long nNameStrRef = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nNameStrRef = nNameStrRef; }
        p2DA->GetINTEntry(i, "ForcePoints", &nValue);
        { unsigned char nForcePoints = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nForcePoints = nForcePoints; }
        p2DA->GetCExoStringEntry(i, "GoodEvil", &sValue);
        { char nGoodEvil = sValue[0]; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_cGoodEvil = nGoodEvil; }
        p2DA->GetINTEntry(i, "SpellDesc", &nValue);
        { unsigned long nDescriptionStrRef = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nDescriptionStrRef = nDescriptionStrRef; }
        p2DA->GetCExoStringEntry(i, "IconResRef", &sValue);
        m_pSpells[i].SetIcon(sValue);
        p2DA->GetCExoStringEntry(i, "Range", &sValue);
        m_pSpells[i].SetRange(sValue.Left(1));
        p2DA->GetINTEntry(i, "TargetType", &nValue);
        { short nTargetType = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nTargetType = nTargetType; }
        p2DA->GetCExoStringEntry(i, "ImpactScript", &sValue);
        m_pSpells[i].SetImpactScript(sValue);
        { unsigned char nGuardianLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nGuardianLevel = nGuardianLevel; }
        { unsigned char nConsularLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nConsularLevel = nConsularLevel; }
        { unsigned char nSentinelLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nSentinelLevel = nSentinelLevel; }
        { unsigned char nInnateLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nInnateLevel = nInnateLevel; }
        { unsigned char nWeaponMasterLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nWeaponMasterLevel = nWeaponMasterLevel; }
        { unsigned char nJediMasterLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nJediMasterLevel = nJediMasterLevel; }
        { unsigned char nWatchmanLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nWatchmanLevel = nWatchmanLevel; }
        { unsigned char nMarauderLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nMarauderLevel = nMarauderLevel; }
        { unsigned char nSithLordLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nSithLordLevel = nSithLordLevel; }
        { unsigned char nAssassinLevel = -1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nAssassinLevel = nAssassinLevel; }
        if (p2DA->GetINTEntry(i, "Guardian", &nValue))
        {
            { unsigned char nGuardianLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nGuardianLevel = nGuardianLevel; }
        }
        if (p2DA->GetINTEntry(i, "Consular", &nValue))
        {
            { unsigned char nConsularLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nConsularLevel = nConsularLevel; }
        }
        if (p2DA->GetINTEntry(i, "Sentinel", &nValue))
        {
            { unsigned char nSentinelLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nSentinelLevel = nSentinelLevel; }
        }
        if (p2DA->GetINTEntry(i, "Inate", &nValue))
        {
            { unsigned char nInnateLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nInnateLevel = nInnateLevel; }
        }
        if (p2DA->GetINTEntry(i, "WeapMstr", &nValue))
        {
            { unsigned char nWeaponMasterLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nWeaponMasterLevel = nWeaponMasterLevel; }
        }
        if (p2DA->GetINTEntry(i, "JediMaster", &nValue))
        {
            { unsigned char nJediMasterLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nJediMasterLevel = nJediMasterLevel; }
        }
        if (p2DA->GetINTEntry(i, "Watchman", &nValue))
        {
            { unsigned char nWatchmanLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nWatchmanLevel = nWatchmanLevel; }
        }
        if (p2DA->GetINTEntry(i, "Marauder", &nValue))
        {
            { unsigned char nMarauderLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nMarauderLevel = nMarauderLevel; }
        }
        if (p2DA->GetINTEntry(i, "SithLord", &nValue))
        {
            { unsigned char nSithLordLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nSithLordLevel = nSithLordLevel; }
        }
        if (p2DA->GetINTEntry(i, "Assassin", &nValue))
        {
            { unsigned char nAssassinLevel = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nAssassinLevel = nAssassinLevel; }
        }
        p2DA->GetINTEntry(i, "ConjTime", &nValue);
        { int nConjureTime = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nConjureTime = nConjureTime; }
        p2DA->GetCExoStringEntry(i, "ConjSoundMale", &sValue);
        m_pSpells[i].SetConjureSoundMale(sValue);
        p2DA->GetCExoStringEntry(i, "ConjSoundFemale", &sValue);
        m_pSpells[i].SetConjureSoundFemale(sValue);
        p2DA->GetCExoStringEntry(i, "ConjSoundVFX", &sValue);
        m_pSpells[i].SetConjureSoundVFX(sValue);
        p2DA->GetCExoStringEntry(i, "ConjHeadVisual", &sValue);
        m_pSpells[i].SetConjureHeadVisual(sValue);
        p2DA->GetCExoStringEntry(i, "ConjHandVisual", &sValue);
        m_pSpells[i].SetConjureHandVisual(sValue);
        p2DA->GetCExoStringEntry(i, "ConjGrndVisual", &sValue);
        m_pSpells[i].SetConjureGroundVisual(sValue);
        p2DA->GetINTEntry(i, "CastTime", &nValue);
        { int nCastTime = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nCastTime = nCastTime; }
        p2DA->GetCExoStringEntry(i, "CastAnim", &sValue);
        sValue = sValue.LowerCase();
        int nType;
        if (sValue == "dark")
        {
            nType = 2;
        }
        else if (sValue == "self")
        {
            nType = 1;
        }
        else if (sValue == "up")
        {
            nType = 3;
        }
        else if (sValue == "area")
        {
            nType = 4;
        }
        else if (sValue == "point")
        {
            nType = 6;
        }
        else if (sValue == "touch")
        {
            nType = 6;
        }
        else if (sValue == "throw")
        {
            nType = 7;
        }
        else if (sValue == "jump")
        {
            nType = 8;
        }
        else if (sValue == "monster")
        {
            nType = 9;
        }
        else if (sValue == "fury")
        {
            nType = 10;
        }
        else if (sValue == "monsterfury")
        {
            nType = 12;
        }
        else if (sValue == "crush")
        {
            nType = 11;
        }
        else
        {
            nType = 0;
        }
        nValue = nType;
        { short nCastAnim = nType; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nCastAnim = nCastAnim; }
        { short nConjureAnim = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nConjureAnim = nConjureAnim; }
        p2DA->GetCExoStringEntry(i, "CastHeadVisual", &sValue);
        m_pSpells[i].SetCastHeadVisual(sValue);
        p2DA->GetCExoStringEntry(i, "CastHandVisual", &sValue);
        m_pSpells[i].SetCastHandVisual(sValue);
        p2DA->GetCExoStringEntry(i, "CastSound", &sValue);
        m_pSpells[i].SetCastSound(sValue);
        p2DA->GetCExoStringEntry(i, "CastGrndVisual", &sValue);
        m_pSpells[i].SetCastGroundVisual(sValue);
        p2DA->GetINTEntry(i, "CatchTime", &nValue);
        { int nCatchTime = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nCatchTime = nCatchTime; }
        p2DA->GetCExoStringEntry(i, "CatchAnim", &sValue);
        sValue = sValue.LowerCase();
        if (sValue == "catch")
        {
            nValue = 0;
        }
        int nNumEntries = 0;
        int aEntries[5] = {0, 0, 0, 0, 0};
        if (p2DA->GetINTEntry(i, "FORCEFRIENDLY", &nValue))
        {
            aEntries[0] = 2 * (5 * nValue + 1500);
            p2DA->GetINTEntry(i, "FORCEPRIORITY", &nValue);
            aEntries[0] += nValue;
            nNumEntries = 1;
        }
        if (p2DA->GetINTEntry(i, "FORCEHOSTILE", &nValue))
        {
            aEntries[nNumEntries] = 2 * (5 * nValue + 500);
            p2DA->GetINTEntry(i, "FORCEPRIORITY", &nValue);
            aEntries[nNumEntries] += nValue;
            nNumEntries++;
        }
        if (p2DA->GetINTEntry(i, "FORCEPASSIVE", &nValue))
        {
            aEntries[nNumEntries] = 2 * (5 * nValue + 1000);
            p2DA->GetINTEntry(i, "FORCEPRIORITY", &nValue);
            aEntries[nNumEntries] += nValue;
            nNumEntries++;
        }
        m_pSpells[i].SetSubRadialSpellCount(nNumEntries);
        if (nNumEntries > 0)
        {
            for (int j = 0; j < nNumEntries; j++)
            {
                int nSpell = aEntries[j];
                CSWSpell* pSpell = &m_pSpells[i];
                if ((unsigned char)j < pSpell->m_nSubRadialSpellCount)
                {
                    pSpell->m_pSubRadialSpells[(unsigned char)j] = nSpell;
                }
            }
        }
        if (p2DA->GetINTEntry(i, "Counter2", &nValue))
        {
            CSWSpell* pSpell = &m_pSpells[i];
            pSpell->m_nCounter2 = nValue;
        }
        else
        {
            CSWSpell* pSpell = &m_pSpells[i];
            pSpell->m_nCounter2 = -1;
        }
        p2DA->GetINTEntry(i, "Proj", &nValue);
        { int nProjectile = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nProjectile = nProjectile; }
        if (nValue == 0)
        {
            sValue = "sp_sumfire";
            m_pSpells[i].SetProjectileModel(sValue);
            { int nProjectileType = 1; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nProjectileType = nProjectileType; }
            m_pSpells[i].SetProjectileSound("amb_e14");
        }
        else
        {
            p2DA->GetCExoStringEntry(i, "ProjModel", &sValue);
            m_pSpells[i].SetProjectileModel(sValue);
            p2DA->GetCExoStringEntry(i, "ProjType", &sValue);
            sValue = sValue.LowerCase();
            nValue = 0;
            if (sValue == "homing")
            {
                nValue = 1;
            }
            else if (sValue == "ballistic")
            {
                nValue = 2;
            }
            else if (sValue == "highballistic")
            {
                nValue = 3;
            }
            else if (sValue == "burst")
            {
                nValue = 9;
            }
            else if (sValue == "accelerating")
            {
                nValue = 5;
            }
            else if (sValue == "spiral")
            {
                nValue = 6;
            }
            else if (sValue == "linked")
            {
                nValue = 7;
            }
            else if (sValue == "grenade")
            {
                nValue = 11;
            }
            else if (sValue == "bounce")
            {
                nValue = 8;
            }
            else if (sValue == "grenade")
            {
                nValue = 11;
            }
            else if (sValue == "test")
            {
                nValue = 50;
            }
            { int nProjectileType = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nProjectileType = nProjectileType; }
            p2DA->GetCExoStringEntry(i, "ProjSpwnPoint", &sValue);
            sValue = sValue.LowerCase();
            if (sValue == "hand")
            {
                nType = 1;
            }
            else if (sValue == "head")
            {
                nType = 2;
            }
            else if (sValue == "throw")
            {
                nType = 3;
            }
            else
            {
                nType = 0;
            }
            nValue = nType;
            { unsigned char nProjectileSpawnPoint = nType; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nProjectileSpawnPoint = nProjectileSpawnPoint; }
            p2DA->GetCExoStringEntry(i, "ProjSound", &sValue);
            m_pSpells[i].SetProjectileSound(sValue);
            p2DA->GetCExoStringEntry(i, "ProjOrientation", &sValue);
            sValue = sValue.LowerCase();
            nValue = 0;
            if (sValue == "target")
            {
                nValue = 1;
            }
            else if (sValue == "path")
            {
                nValue = 2;
            }
            { unsigned char nProjectileOrientation = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nProjectileOrientation = nProjectileOrientation; }
        }
        p2DA->GetINTEntry(i, "UserType", &nValue);
        { unsigned char nUserType = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nUserType = nUserType; }
        p2DA->GetINTEntry(i, "AltMessage", &nValue);
        { int nAltMessage = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nAltMessage = nAltMessage; }
        p2DA->GetINTEntry(i, "HostileSetting", &nValue);
        { int nHostileSetting = nValue != 0; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_bHostileSetting = nHostileSetting; }
        p2DA->GetINTEntry(i, "FeatID", &nValue);
        { short nFeatID = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nFeatID = nFeatID; }
        p2DA->GetINTEntry(i, "Exclusion", &nValue);
        { int nExclusion = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nExclusion = nExclusion; }
        p2DA->GetINTEntry(i, "RequireItemMask", &nValue);
        { int nRequireItemMask = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nRequireItemMask = nRequireItemMask; }
        p2DA->GetINTEntry(i, "ForbidItemMask", &nValue);
        { int nForbidItemMask = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nForbidItemMask = nForbidItemMask; }
        if (p2DA->GetINTEntry(i, "ItemTargeting", &nValue))
        {
            { unsigned char nItemTargeting = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nItemTargeting = nItemTargeting; }
        }
        else
        {
            { unsigned char nItemTargeting = 0; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nItemTargeting = nItemTargeting; }
        }
        p2DA->GetINTEntry(i, "FormMask", &nValue);
        { int nFormMask = nValue; CSWSpell* pSpell = &m_pSpells[i]; pSpell->m_nFormMask = nFormMask; }
    }
    delete p2DA;
}
// PC 0x005E2230
void CSWSpell::SetMasterSpells(const CExoString& sSpells)
{
    m_sMasterSpells = sSpells;
    int nLength = sSpells.GetLength();
    int nPos = 0;
    for (; nPos < nLength; nPos++)
    {
        if (sSpells[nPos] != '_' && sSpells[nPos] != ':')
        {
            break;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        int nValue = 0;
        m_aMasterSpells[i] = -1;
        if (nPos < nLength)
        {
            while (sSpells[nPos] != '_' && sSpells[nPos] != ':')
            {
                nValue = nValue * 10 + (unsigned char)sSpells[nPos++] - '0';
            }
            nPos++;
            m_aMasterSpells[i] = nValue;
        }
    }
}
// PC 0x005E2310
int CSWSpell::GetAdjustedForcePoints(unsigned long nLevel, int bAdjust, int nRoomForceRating,
                                     int nForm, char nCharismaModifier)
{
    int nForcePoints = m_nForcePoints;
    if (nForcePoints == 0)
    {
        return 0;
    }
    unsigned long nRow = nLevel / 10;
    if (bAdjust)
    {
        float fMultiplier = 1.0f;
        if (m_cGoodEvil == 'G')
        {
            g_pRules->m_p2DArrays->m_pForcePointAdjust->GetFLOATEntry(
                nRow, CTwoDimArrays::COL2DA_FORCEADJUST_GOODCOST, &fMultiplier);
        }
        else if (m_cGoodEvil == 'E')
        {
            g_pRules->m_p2DArrays->m_pForcePointAdjust->GetFLOATEntry(
                nRow, CTwoDimArrays::COL2DA_FORCEADJUST_EVILCOST, &fMultiplier);
        }
        if (fMultiplier > 1.0f && nCharismaModifier > 0)
        {
            fMultiplier = fMultiplier - nCharismaModifier * FORCE_COST_REDUCTION_STEP;
            if (fMultiplier < 1.0f)
            {
                fMultiplier = 1.0f;
            }
        }
        nForcePoints = (int)(nForcePoints * fMultiplier);
    }
    int nRoomAdjust = nRoomForceRating / 20;
    if (nRow > ALIGNMENT_ROW_NEUTRAL)
    {
        nForcePoints -= nRoomAdjust;
    }
    else if (nRow < ALIGNMENT_ROW_NEUTRAL)
    {
        nForcePoints += nRoomAdjust;
    }
    if (nForcePoints < 1)
    {
        nForcePoints = 1;
    }
    switch (nForm)
    {
    case FORM_FORCE_II_POTENCY:
    case FORM_FORCE_IV_MASTERY:
        nForcePoints = (int)(nForcePoints * FORM_FORCE_COST_MULTIPLIER);
        break;
    }
    return nForcePoints;
}
// PC 0x005E2460
CExoString CSWSpell::GetSpellNameText()
{
    STR_RES strRes;
    g_pTlkTable->Fetch(m_nNameStrRef, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
