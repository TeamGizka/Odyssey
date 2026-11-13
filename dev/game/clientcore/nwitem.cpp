#include <stdio.h>
#include <string.h>
#include "nwitem.h"
#include "nwclass.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "exotlktable.h"
#include "clientexoappinternal.h"

static inline void NwitemTrace1();
#define NWITEM_TRACE() NwitemTrace1()
// PC 0x005FEEB0 GOG 0x005FEE90
CSWBaseItem::CSWBaseItem()
{
    m_nEquipableSlots = 0;
    m_bUnknown18 = 0;
    m_nName = -1;
    memset(m_sItemClass, 0, sizeof(m_sItemClass));
    m_nPartEnvMap = 0;
    m_nInvSoundType = 0;
    m_pRequiredFeats = 0;
    m_nRequiredFeatCount = 0;
    m_nRotateOnGround = 0;
    m_nSpecFeat = 0;
    m_nFocFeat = 0;
    m_nDenySubrace = 0;
    m_nBodyVar = 1;
    m_nDexBonus = -1;
}
// PC 0x005FEF80 GOG 0x005FEF60
CSWBaseItem::~CSWBaseItem()
{
    if (m_pRequiredFeats != NULL)
    {
        delete[] m_pRequiredFeats;
    }
}
// PC 0x005FF010 GOG 0x005FEFF0
CResRef CSWBaseItem::GetIconResRef(short nVariation, char cPrefix)
{
    char sBuffer[20];

    if (m_nGenderSpecific != 0 && cPrefix != 0)
    {
        sprintf(sBuffer, "i%s_%c_%03d", m_sItemClass, cPrefix, nVariation);
    }
    else
    {
        sprintf(sBuffer, "i%s_%03d", m_sItemClass, nVariation);
    }
    return CResRef(sBuffer);
}
// PC 0x005FF080 GOG 0x005FF060
short CSWBaseItem::GetRequiredFeat(unsigned char nIndex)
{
    if (nIndex >= m_nRequiredFeatCount)
    {
        return -1;
    }
    return m_pRequiredFeats[nIndex];
}
// PC 0x005FF0A0 GOG 0x005FF080
CResRef CSWBaseItem::GetModelResRef(short nVariation, char cPrefix)
{
    char sBuffer[20];

    if (m_nGenderSpecific != 0 && cPrefix != 0)
    {
        sprintf(sBuffer, "%s_%c_%03d", m_sItemClass, cPrefix, nVariation);
    }
    else
    {
        sprintf(sBuffer, "%s_%03d", m_sItemClass, nVariation);
    }
    return CResRef(sBuffer);
}
// PC 0x005FF110 GOG 0x005FF0F0
int CSWBaseItem::GetIsTwoHanded()
{
    return m_nWeaponWield == 3 || m_nWeaponWield == 5 || m_nWeaponWield == 6;
}
// PC 0x005FF130 GOG 0x005FF110
int CSWBaseItem::GetIsOneHanded()
{
    return m_nWeaponWield == 1 || m_nWeaponWield == 2 || m_nWeaponWield == 4;
}
// PC 0x005FF150 GOG 0x005FF130
CSWBaseItemArray::CSWBaseItemArray()
{
    m_nNumBaseItems = 0;
    m_pBaseItems = NULL;
}
// PC 0x005FF160 GOG 0x005FF140
CSWBaseItem* CSWBaseItemArray::GetBaseItem(int nIndex) const
{
    if (m_pBaseItems != NULL && nIndex >= 0 && nIndex < m_nNumBaseItems)
    {
        return &m_pBaseItems[nIndex];
    }
    return NULL;
}
// PC 0x005FF190 GOG 0x005FF170
void CSWBaseItemArray::Load()
{
    C2DA* pTable = new C2DA("BASEITEMS", 0);
    pTable->Load2DArray();
    m_nNumBaseItems = pTable->m_nNumRows;
    m_pBaseItems = new CSWBaseItem[m_nNumBaseItems];

    int nValue;
    float fValue;
    CExoString sValue;

    for (int nItem = 0; nItem < m_nNumBaseItems; ++nItem)
    {
        pTable->GetINTEntry(nItem, "Name", &nValue);
        m_pBaseItems[nItem].SetNameStrRef(nValue);
        pTable->GetINTEntry(nItem, "EquipableSlots", &nValue);
        m_pBaseItems[nItem].SetEquipableSlots(nValue);
        pTable->GetINTEntry(nItem, "CanRotateIcon", &nValue);
        m_pBaseItems[nItem].SetCanRotateIcon(nValue);
        pTable->GetINTEntry(nItem, "ModelType", &nValue);
        m_pBaseItems[nItem].SetModelType((unsigned char)nValue);
        pTable->GetCExoStringEntry(nItem, "ItemClass", &sValue);
        m_pBaseItems[nItem].SetItemClass(sValue.CStr());
        pTable->GetINTEntry(nItem, "GenderSpecific", &nValue);
        m_pBaseItems[nItem].SetGenderSpecific((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "PartEnvMap", &nValue);
        m_pBaseItems[nItem].SetPartEnvMap(nValue);
        pTable->GetCExoStringEntry(nItem, "DefaultModel", &sValue);
        m_pBaseItems[nItem].SetDefaultModel(CResRef(sValue));
        pTable->GetCExoStringEntry(nItem, "DefaultIcon", &sValue);
        m_pBaseItems[nItem].SetDefaultIcon(CResRef(sValue));
        pTable->GetINTEntry(nItem, "Container", &nValue);
        m_pBaseItems[nItem].SetIsContainer(nValue);
        pTable->GetINTEntry(nItem, "WeaponWield", &nValue);
        m_pBaseItems[nItem].SetWeaponWield((char)nValue);
        pTable->GetINTEntry(nItem, "WeaponType", &nValue);
        m_pBaseItems[nItem].SetWeaponType((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "DamageFlags", &nValue);
        m_pBaseItems[nItem].SetDamageFlags(nValue);
        pTable->GetINTEntry(nItem, "WeaponSize", &nValue);
        m_pBaseItems[nItem].SetWeaponSize((unsigned char)nValue);
        if (pTable->GetINTEntry(nItem, "RangedWeapon", &nValue))
        {
            m_pBaseItems[nItem].SetRangedWeapon((unsigned char)nValue);
        }
        else
        {
            m_pBaseItems[nItem].SetRangedWeapon(0);
        }
        pTable->GetFLOATEntry(nItem, "MaxAttackRange", &fValue);
        m_pBaseItems[nItem].SetMaxAttackRange(fValue);
        pTable->GetFLOATEntry(nItem, "PrefAttackDist", &fValue);
        m_pBaseItems[nItem].SetPrefAttackDist(fValue);
        pTable->GetINTEntry(nItem, "MinRange", &nValue);
        m_pBaseItems[nItem].SetMinRange(nValue);
        pTable->GetINTEntry(nItem, "MaxRange", &nValue);
        m_pBaseItems[nItem].SetMaxRange(nValue);
        pTable->GetINTEntry(nItem, "NumDice", &nValue);
        m_pBaseItems[nItem].SetNumDice((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "DieToRoll", &nValue);
        m_pBaseItems[nItem].SetDieToRoll((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "CritThreat", &nValue);
        m_pBaseItems[nItem].SetCritThreat((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "CritHitMult", &nValue);
        m_pBaseItems[nItem].SetCritHitMult((unsigned char)nValue);
        pTable->GetFLOATEntry(nItem, "BaseCost", &fValue);
        m_pBaseItems[nItem].SetBaseCost(fValue);
        pTable->GetINTEntry(nItem, "Stacking", &nValue);
        m_pBaseItems[nItem].SetMaxStackSize((unsigned short)nValue);
        m_pBaseItems[nItem].SetCostMultiplier(1.0f);
        pTable->GetINTEntry(nItem, "Description", &nValue);
        m_pBaseItems[nItem].SetDescription(nValue);
        if (pTable->GetINTEntry(nItem, "InvSoundType", &nValue))
        {
            m_pBaseItems[nItem].SetInvSoundType((unsigned char)nValue);
        }
        pTable->GetINTEntry(nItem, "MaxProps", &nValue);
        m_pBaseItems[nItem].SetMaxProps((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "MinProps", &nValue);
        m_pBaseItems[nItem].SetMinProps((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "PropColumn", &nValue);
        m_pBaseItems[nItem].SetPropColumn((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "ArmorCheckPen", &nValue);
        m_pBaseItems[nItem].SetArmorCheckPen((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "ChargesStarting", &nValue);
        m_pBaseItems[nItem].SetChargesStarting((unsigned char)nValue);
        if (!pTable->GetINTEntry(nItem, "BaseItemStatRef", &nValue))
        {
            nValue = CSWBaseItem::INVALID_STAT_REF;
        }
        m_pBaseItems[nItem].SetBaseItemStatRef(nValue);
        pTable->GetINTEntry(nItem, "StorePanel", &nValue);
        m_pBaseItems[nItem].SetStorePanel((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "StorePanelSort", &nValue);
        m_pBaseItems[nItem].SetStorePanelSort((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "RotateOnGround", &nValue);
        m_pBaseItems[nItem].SetRotateOnGround((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "TenthLBS", &nValue);
        m_pBaseItems[nItem].SetTenthLBS((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "BaseAC", &nValue);
        m_pBaseItems[nItem].SetBaseAC((unsigned char)nValue);
        if (pTable->GetINTEntry(nItem, "AC_Enchant", &nValue))
        {
            m_pBaseItems[nItem].SetACEnchantType((unsigned char)nValue);
        }
        else
        {
            m_pBaseItems[nItem].SetACEnchantType(CSWBaseItem::INVALID_AC_ENCHANT);
        }
        pTable->GetINTEntry(nItem, "WeaponMatType", &nValue);
        m_pBaseItems[nItem].SetWeaponMatType((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "AmmunitionType", &nValue);
        m_pBaseItems[nItem].SetAmmunitionType((unsigned char)nValue);
        pTable->GetINTEntry(nItem, "PoweredItem", &nValue);
        m_pBaseItems[nItem].SetPoweredItem(nValue);
        pTable->GetCExoStringEntry(nItem, "PoweredSnd", &sValue);
        m_pBaseItems[nItem].SetPoweredSnd(sValue);
        pTable->GetCExoStringEntry(nItem, "PowerDownSnd", &sValue);
        m_pBaseItems[nItem].SetPowerDownSnd(sValue);
        pTable->GetCExoStringEntry(nItem, "PowerUpSnd", &sValue);
        m_pBaseItems[nItem].SetPowerUpSnd(sValue);
        pTable->GetINTEntry(nItem, "ItemType", &nValue);
        m_pBaseItems[nItem].SetItemType((unsigned char)nValue);
        if (!pTable->GetINTEntry(nItem, "DroidOrHuman", &nValue))
        {
            nValue = 0;
        }
        m_pBaseItems[nItem].SetDroidOrHuman((unsigned char)nValue);
        pTable->GetCExoStringEntry(nItem, "BodyVar", &sValue);
        m_pBaseItems[nItem].SetBodyVar(1);
        sValue.UpperCase();
        if (sValue.GetLength() == 1 && sValue[0] >= 'A' && sValue[0] <= 'Z')
        {
            m_pBaseItems[nItem].SetBodyVar(sValue[0] - 'A' + 1);
        }

        CExoString sColumn;
        unsigned char nFeatCount = 0;
        for (int nColumn = 0; nColumn < 5; ++nColumn)
        {
            sColumn.Format("ReqFeat%d", nColumn);
            if (pTable->GetINTEntry(nItem, sColumn.CStr(), &nValue))
            {
                ++nFeatCount;
            }
        }
        if (nFeatCount > 0)
        {
            m_pBaseItems[nItem].SetRequiredFeatCount(nFeatCount);
            nFeatCount = 0;
            for (int nColumn = 0; nColumn < 5; ++nColumn)
            {
                sColumn.Format("ReqFeat%d", nColumn);
                if (pTable->GetINTEntry(nItem, sColumn.CStr(), &nValue))
                {
                    m_pBaseItems[nItem].SetRequiredFeat(nFeatCount, (unsigned short)nValue);
                    ++nFeatCount;
                }
            }
        }
        if (pTable->GetINTEntry(nItem, "SpecFeat", &nValue))
        {
            m_pBaseItems[nItem].SetSpecFeat((unsigned short)nValue);
        }
        if (pTable->GetINTEntry(nItem, "FocFeat", &nValue))
        {
            m_pBaseItems[nItem].SetFocFeat((unsigned short)nValue);
        }
        if (pTable->GetINTEntry(nItem, "DenySubrace", &nValue))
        {
            m_pBaseItems[nItem].SetDenySubrace(nValue);
        }
        if (pTable->GetCExoStringEntry(nItem, "ArmorType", &sValue))
        {
            m_pBaseItems[nItem].SetArmorType(sValue);
        }
        if (pTable->GetINTEntry(nItem, "DEXBONUS", &nValue))
        {
            m_pBaseItems[nItem].SetDexBonus((char)nValue);
        }
    }
    delete pTable;
}
// PC 0x00600630 GOG 0x00600610
CSWItem::CSWItem()
{
    m_nBaseItem = DEFAULT_BASE_ITEM;
    memset(m_aUnknown04, 0, sizeof(m_aUnknown04));
}
// PC 0x00600650 GOG 0x00600630
CSWItem::~CSWItem()
{
}
// PC 0x00600660 GOG 0x00600640
CExoString CSWBaseItem::GetNameText()
{
    STR_RES strRes;

    g_pTlkTable->Fetch(m_nName, strRes, CTlkTable::GENDER_MALE);
    return strRes.m_sString;
}
// PC 0x00600700 GOG 0x006006E0
CSWBaseItemArray::~CSWBaseItemArray()
{
    if (m_pBaseItems != NULL)
    {
        delete[] m_pBaseItems;
        m_pBaseItems = NULL;
        m_nNumBaseItems = 0;
    }
}
// PC 0x00600760 GOG 0x00600740
CSWBaseItem* CSWItem::GetBaseItem()
{
    NWITEM_TRACE();
    return g_pRules->m_pBaseItems->GetBaseItem(m_nBaseItem);
}

static inline void NwitemTrace1()
{
}
