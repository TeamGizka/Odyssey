#include "nwsitemproperty.h"
#include "exobase.h"
#include "twodimarray.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "nwsitem.h"
#include "nwitem.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "nwscript.h"
#include "nwscreaturestats.h"
#include "nwrules.h"
// PC 0x00539330
CSWSItemPropertyHandler::~CSWSItemPropertyHandler()
{
    if (m_pApplyItemProperty != NULL)
    {
        delete[] m_pApplyItemProperty;
        m_pApplyItemProperty = NULL;
    }

    if (m_pRemoveItemProperty != NULL)
    {
        delete[] m_pRemoveItemProperty;
        m_pRemoveItemProperty = NULL;
    }
}
// PC 0x005393A0
int CSWSItemPropertyHandler::OnItemPropertyApplied(CSWSItem* pItem,
                                                   CSWItemProperty* pProperty,
                                                   CSWSCreature* pCreature,
                                                   unsigned long nIndex,
                                                   int bLoading)
{
    ITEM_PROPERTY_APPLY pfnApply;

    if (pProperty->m_nPropertyType < 70 &&
        (pfnApply = m_pApplyItemProperty[pProperty->m_nPropertyType]) != NULL)
    {
        return (this->*pfnApply)(pItem, pProperty, pCreature, nIndex, bLoading);
    }

    return 0;
}
// PC 0x005393E0
int CSWSItemPropertyHandler::OnItemPropertyRemoved(CSWSItem* pItem,
                                                   CSWItemProperty* pProperty,
                                                   CSWSCreature* pCreature,
                                                   unsigned long nIndex)
{
    ITEM_PROPERTY_REMOVE pfnRemove;

    if (pProperty->m_nPropertyType < 70 &&
        (pfnRemove = m_pRemoveItemProperty[pProperty->m_nPropertyType]) != NULL)
    {
        return (this->*pfnRemove)(pItem, pProperty, pCreature, nIndex);
    }

    return 0;
}
// PC 0x00539420
int CSWSItemPropertyHandler::ApplyEnhancementBonus(CSWSItem* pItem,
                                                    CSWItemProperty* pProperty,
                                                    CSWSCreature* pCreature,
                                                    unsigned long nIndex,
                                                    int bLoading)
{
    CGameEffect* pAttackEffect;
    CGameEffect* pDamageEffect;
    CGameEffect* pOffHandEffect;
    int nBaseItem;
    int nAmount;
    char nWeaponWield;

    nBaseItem = pItem->m_nBaseItem;
    g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
    pAttackEffect = new CGameEffect(1);
    pDamageEffect = new CGameEffect(1);
    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->m_pIPRPMelee
            ->GetINTEntry(nCostValue, sColumn, &nAmount);
    }
    if (nAmount == 0)
    {
        return 0;
    }
    pAttackEffect->m_nType = CSWSEffectListHandler::EFFECT_ATTACK_INCREASE;
    pAttackEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
        (pAttackEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    {
        unsigned long oidItem = pItem->m_nId;

        pAttackEffect->SetCreator(oidItem);
    }
    pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_BONUS, nAmount);
    if (nIndex == INVENTORY_SLOT_RIGHTHAND)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_ONHAND);
    }
    else if (nIndex == INVENTORY_SLOT_LEFTHAND)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_OFFHAND);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3);
    }
    else if (nIndex == INVENTORY_SLOT_HANDS)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_UNARMED);
    }
    pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP,
            g_pRules->m_nNumRaces);
    if (pProperty->m_nPropertyType ==
        CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP)
    {
        unsigned short nSubtype = pProperty->m_nSubtype;

        if (nSubtype == CSWSCreatureStats::ALIGNMENT_NEUTRAL)
        {
            pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP1,
                               CSWSCreatureStats::ALIGNMENT_NEUTRAL);
        }
        else if (nSubtype == CSWSCreatureStats::ALIGNMENT_GOOD)
        {
            pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2,
                               CSWSCreatureStats::ALIGNMENT_GOOD);
        }
        else if (nSubtype == CSWSCreatureStats::ALIGNMENT_EVIL)
        {
            pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2,
                               CSWSCreatureStats::ALIGNMENT_EVIL);
        }
    }
    else if (pProperty->m_nPropertyType ==
             CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_RACIAL_GROUP)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP,
                           pProperty->m_nSubtype);
    }
    nBaseItem = pItem->m_nBaseItem;
    nWeaponWield = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nWeaponWield;
    if (nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
    {
        pOffHandEffect = new CGameEffect(0);
        pOffHandEffect->CopyEffect(pAttackEffect, 0);
        pOffHandEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                                   CSWSEffectListHandler::ATTACK_BONUS_OFFHAND);
        pCreature->ApplyEffect(pOffHandEffect, bLoading, 0);
    }
    pCreature->ApplyEffect(pAttackEffect, bLoading, 0);

    pDamageEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE;
    pDamageEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
        (pDamageEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    {
        unsigned long oidItem = pItem->m_nId;

        pDamageEffect->SetCreator(oidItem);
    }
    if (nIndex == INVENTORY_SLOT_RIGHTHAND)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_ONHAND);
    }
    else if (nIndex == INVENTORY_SLOT_LEFTHAND)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3);
    }
    else if (nIndex == INVENTORY_SLOT_HANDS)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_UNARMED);
    }
    pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS, nAmount);
    pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_TYPE,
            pItem->GetDamageFlags());
    pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP,
            g_pRules->m_nNumRaces);
    if (pProperty->m_nPropertyType ==
        CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP)
    {
        unsigned short nSubtype = pProperty->m_nSubtype;

        if (nSubtype == CSWSCreatureStats::ALIGNMENT_NEUTRAL)
        {
            pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP1,
                               CSWSCreatureStats::ALIGNMENT_NEUTRAL);
        }
        else if (nSubtype == CSWSCreatureStats::ALIGNMENT_GOOD)
        {
            pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2,
                               CSWSCreatureStats::ALIGNMENT_GOOD);
        }
        else if (nSubtype == CSWSCreatureStats::ALIGNMENT_EVIL)
        {
            pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2,
                               CSWSCreatureStats::ALIGNMENT_EVIL);
        }
        nBaseItem = pItem->m_nBaseItem;
    }
    else if (pProperty->m_nPropertyType ==
             CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_RACIAL_GROUP)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP,
                           pProperty->m_nSubtype);
        nBaseItem = pItem->m_nBaseItem;
    }
    else
    {
        nBaseItem = pItem->m_nBaseItem;
    }
    nWeaponWield = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nWeaponWield;
    if (nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
    {
        pOffHandEffect = new CGameEffect(0);
        pOffHandEffect->CopyEffect(pDamageEffect, 0);
        pOffHandEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
                                   CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND);
        pCreature->ApplyEffect(pOffHandEffect, bLoading, 0);
    }
    pCreature->ApplyEffect(pDamageEffect, bLoading, 0);

    return 0;
}
// PC 0x00539860
int CSWSItemPropertyHandler::ApplyEnhancementPenalty(CSWSItem* pItem,
                                                    CSWItemProperty* pProperty,
                                                    CSWSCreature* pCreature,
                                                    unsigned long nIndex,
                                                    int bLoading)
{
    CGameEffect* pAttackEffect;
    CGameEffect* pDamageEffect;
    CGameEffect* pOffHandEffect;
    int nBaseItem;
    int nAmount;
    char nWeaponWield;

    nBaseItem = pItem->m_nBaseItem;
    g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
    pAttackEffect = new CGameEffect(1);
    pDamageEffect = new CGameEffect(1);
    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)
            ->GetINTEntry(nCostValue, sColumn, &nAmount);
    }
    nAmount = -nAmount;
    if (nAmount == 0)
    {
        return 0;
    }
    pAttackEffect->m_nType = CSWSEffectListHandler::EFFECT_ATTACK_DECREASE;
    pAttackEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
        (pAttackEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    {
        unsigned long oidItem = pItem->m_nId;

        pAttackEffect->SetCreator(oidItem);
    }
    pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_BONUS, nAmount);
    pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP,
            g_pRules->m_nNumRaces);
    if (nIndex == INVENTORY_SLOT_RIGHTHAND)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_ONHAND);
    }
    else if (nIndex == INVENTORY_SLOT_LEFTHAND)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_OFFHAND);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3);
    }
    else if (nIndex == INVENTORY_SLOT_HANDS)
    {
        pAttackEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
            CSWSEffectListHandler::ATTACK_BONUS_UNARMED);
    }
    nBaseItem = pItem->m_nBaseItem;
    nWeaponWield = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nWeaponWield;
    if (nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
    {
        pOffHandEffect = new CGameEffect(0);
        pOffHandEffect->CopyEffect(pAttackEffect, 0);
        pOffHandEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                                   CSWSEffectListHandler::ATTACK_BONUS_OFFHAND);
        pCreature->ApplyEffect(pOffHandEffect, bLoading, 0);
    }
    pCreature->ApplyEffect(pAttackEffect, bLoading, 0);

    pDamageEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE;
    pDamageEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
        (pDamageEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    {
        unsigned long oidItem = pItem->m_nId;

        pDamageEffect->SetCreator(oidItem);
    }
    if (nIndex == INVENTORY_SLOT_RIGHTHAND)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_ONHAND);
    }
    else if (nIndex == INVENTORY_SLOT_LEFTHAND)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2);
    }
    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3);
    }
    else if (nIndex == INVENTORY_SLOT_HANDS)
    {
        pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
            CSWSEffectListHandler::DAMAGE_BONUS_UNARMED);
    }
    pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS, nAmount);
    pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_TYPE,
            pItem->GetDamageFlags());
    pDamageEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP,
            g_pRules->m_nNumRaces);
    nBaseItem = pItem->m_nBaseItem;
    nWeaponWield = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nWeaponWield;
    if (nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
    {
        pOffHandEffect = new CGameEffect(0);
        pOffHandEffect->CopyEffect(pDamageEffect, 0);
        pOffHandEffect->SetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON,
                                   CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND);
        pCreature->ApplyEffect(pOffHandEffect, bLoading, 0);
    }
    pCreature->ApplyEffect(pDamageEffect, bLoading, 0);

    return 0;
}
// PC 0x0053C5A0
int CSWSItemPropertyHandler::ApplyDamageBonus(CSWSItem* pItem,
                                              CSWItemProperty* pProperty,
                                              CSWSCreature* pCreature,
                                              unsigned long nIndex,
                                              int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);
    int nBaseItem;
    int nAmount = pProperty->m_nCostValue;

    if (nAmount > 0)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        {
            unsigned long oidItem = pItem->m_nId;

            pEffect->SetCreator(oidItem);
        }
        pEffect->SetInteger(0, nAmount);
        if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS)
        {
            unsigned short nDamageType = pProperty->m_nSubtype;

            pEffect->SetInteger(1, 1 << nDamageType);
        }
        else
        {
            pEffect->SetInteger(1, 1 << pProperty->m_nParam1Value);
        }
        if (nIndex == 16)
        {
            pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_ONHAND);
        }
        else if (nIndex == 32)
        {
            pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND);
        }
        else if (nIndex == 16384)
        {
            pEffect->SetInteger(5,
                                CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1);
        }
        else if (nIndex == 32768)
        {
            pEffect->SetInteger(5,
                                CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2);
        }
        else if (nIndex == 65536)
        {
            pEffect->SetInteger(5,
                                CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3);
        }
        else if (nIndex == 8)
        {
            pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_UNARMED);
        }
        pEffect->SetInteger(2, g_pRules->m_nNumRaces);
        if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_ALIGNMENT_GROUP)
        {
            unsigned short nSubtype = pProperty->m_nSubtype;

            if (nSubtype == 1)
            {
                pEffect->SetInteger(3, 1);
            }
            else if (nSubtype == 2)
            {
                pEffect->SetInteger(4, 2);
            }
            else if (nSubtype == 3)
            {
                pEffect->SetInteger(4, 3);
            }
        }
        else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_RACIAL_GROUP)
        {
            pEffect->SetInteger(2, pProperty->m_nSubtype);
        }
        nBaseItem = pItem->m_nBaseItem;
        if (g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nWeaponWield == 3)
        {
            CGameEffect* pOffHandEffect = new CGameEffect(0);

            pOffHandEffect->CopyEffect(pEffect, 0);
            pOffHandEffect->SetInteger(
                5, CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND);
            ((CSWSObject*)pCreature)->ApplyEffect(pOffHandEffect, bLoading, 0);
        }
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x00539C00
int CSWSItemPropertyHandler::ApplyAttackBonus(CSWSItem* pItem,
                                              CSWItemProperty* pProperty,
                                              CSWSCreature* pCreature,
                                              unsigned long nIndex,
                                              int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);
    int nBaseItem;
    int nAmount;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->m_pIPRPMelee->GetINTEntry(nCostValue, sColumn, &nAmount);
    }
    if (nAmount != 0)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_ATTACK_INCREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->GetId());
        pEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_BONUS,
                            nAmount);
        if (nIndex == INVENTORY_SLOT_RIGHTHAND)
        {
            pEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                CSWSEffectListHandler::ATTACK_BONUS_ONHAND);
        }
        else if (nIndex == INVENTORY_SLOT_LEFTHAND)
        {
            pEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                CSWSEffectListHandler::ATTACK_BONUS_OFFHAND);
        }
        else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
        {
            pEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1);
        }
        else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
        {
            pEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2);
        }
        else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
        {
            pEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3);
        }
        else if (nIndex == INVENTORY_SLOT_HANDS)
        {
            pEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                CSWSEffectListHandler::ATTACK_BONUS_UNARMED);
        }
        pEffect->SetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP,
                            g_pRules->m_nNumRaces);
        if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_ALIGNMENT_GROUP)
        {
            unsigned short nSubtype = pProperty->m_nSubtype;

            if (nSubtype == CSWSCreatureStats::ALIGNMENT_NEUTRAL)
            {
                pEffect->SetInteger(
                    CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP1,
                    CSWSCreatureStats::ALIGNMENT_NEUTRAL);
            }
            else if (nSubtype == CSWSCreatureStats::ALIGNMENT_GOOD)
            {
                pEffect->SetInteger(
                    CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2,
                    CSWSCreatureStats::ALIGNMENT_GOOD);
            }
            else if (nSubtype == CSWSCreatureStats::ALIGNMENT_EVIL)
            {
                pEffect->SetInteger(
                    CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2,
                    CSWSCreatureStats::ALIGNMENT_EVIL);
            }
        }
        else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_RACIAL_GROUP)
        {
            int nRacialGroup = pProperty->m_nSubtype;

            pEffect->SetInteger(
                CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP,
                nRacialGroup);
        }
        nBaseItem = pItem->m_nBaseItem;
        if (g_pRules->GetBaseItemArray()->GetBaseItem(nBaseItem)
                ->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
        {
            CGameEffect* pOffHandEffect = new CGameEffect(0);

            pOffHandEffect->CopyEffect(pEffect, 0);
            pOffHandEffect->SetInteger(
                CSWSEffectListHandler::ATTACK_INTEGER_WEAPON,
                CSWSEffectListHandler::ATTACK_BONUS_OFFHAND);
            pCreature->ApplyEffect(pOffHandEffect, bLoading, 0);
        }
        pCreature->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x00539E40
int CSWSItemPropertyHandler::ApplyAttackPenalty(CSWSItem* pItem,
                                                CSWItemProperty* pProperty,
                                                CSWSCreature* pCreature,
                                                unsigned long nIndex,
                                                int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);
    CGameEffect* pOffHandEffect;
    int nBaseItem;
    int nAmount;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    nAmount = -nAmount;
    if (nAmount == 0)
    {
        return 0;
    }
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_ATTACK_DECREASE;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    {
        unsigned long oidItem = pItem->m_nId;

        pEffect->SetCreator(oidItem);
    }
    pEffect->SetInteger(0, nAmount);
    pEffect->SetInteger(2, g_pRules->m_nNumRaces);
    if (nIndex == 16)
    {
        pEffect->SetInteger(1, CSWSEffectListHandler::ATTACK_BONUS_ONHAND);
    }
    else if (nIndex == 32)
    {
        pEffect->SetInteger(1, CSWSEffectListHandler::ATTACK_BONUS_OFFHAND);
    }
    else if (nIndex == 16384)
    {
        pEffect->SetInteger(1, CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1);
    }
    else if (nIndex == 32768)
    {
        pEffect->SetInteger(1, CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2);
    }
    else if (nIndex == 65536)
    {
        pEffect->SetInteger(1, CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3);
    }
    else if (nIndex == 8)
    {
        pEffect->SetInteger(1, CSWSEffectListHandler::ATTACK_BONUS_UNARMED);
    }
    nBaseItem = pItem->m_nBaseItem;
    if (g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nWeaponWield == 3)
    {
        pOffHandEffect = new CGameEffect(0);
        pOffHandEffect->CopyEffect(pEffect, 0);
        pOffHandEffect->SetInteger(1,
                                   CSWSEffectListHandler::ATTACK_BONUS_OFFHAND);
        ((CSWSObject*)pCreature)->ApplyEffect(pOffHandEffect, bLoading, 0);
    }
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);

    return 0;
}
// PC 0x0053A060
int CSWSItemPropertyHandler::ApplyLight(CSWSItem* pItem,
                                        CSWItemProperty* pProperty,
                                        CSWSCreature* pCreature,
                                        unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect;

    if (pItem->GetPropertyByTypeExists(29, 0) != 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_LIGHT;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        pEffect->SetInteger(0, 5000);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);
    }

    return 0;
}
// PC 0x0053A120
int CSWSItemPropertyHandler::ApplyAbilityBonus(CSWSItem* pItem,
                                               CSWItemProperty* pProperty,
                                               CSWSCreature* pCreature,
                                               unsigned long nIndex,
                                               int bLoading)
{
    CGameEffect* pEffect;
    int nAmount;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(1)->GetINTEntry(nCostValue, sColumn,
                                                    &nAmount);
    }
    if (nAmount != 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_INCREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        switch (pProperty->m_nSubtype)
        {
        case 0:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH);
            break;
        case 1:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_DEXTERITY);
            break;
        case 2:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_CONSTITUTION);
            break;
        case 3:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_INTELLIGENCE);
            break;
        case 4:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_WISDOM);
            break;
        case 5:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA);
            break;
        default:
            break;
        }
        pEffect->SetInteger(1, nAmount);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053A2A0
int CSWSItemPropertyHandler::ApplyACBonus(CSWSItem* pItem,
                                          CSWItemProperty* pProperty,
                                          CSWSCreature* pCreature,
                                          unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);
    int nBaseItem;
    int nAmount;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(1)->GetINTEntry(nCostValue, sColumn,
                                                    &nAmount);
    }
    if (nAmount != 0)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_AC_INCREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        nBaseItem = pItem->m_nBaseItem;
        pEffect->SetInteger(
            0,
            g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nACEnchantType);
        pEffect->SetInteger(1, nAmount);
        pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL);
        pEffect->SetInteger(2, g_pRules->m_nNumRaces);
        if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_ALIGNMENT_GROUP)
        {
            int nSubtype = pProperty->m_nSubtype;

            if (nSubtype > 0 && nSubtype <= 3)
            {
                pEffect->SetInteger(4, nSubtype);
            }
        }
        else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_DAMAGE_TYPE)
        {
            pEffect->SetInteger(5, pProperty->m_nSubtype);
        }
        else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_RACIAL_GROUP)
        {
            pEffect->SetInteger(2, pProperty->m_nSubtype);
        }
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053A460
int CSWSItemPropertyHandler::ApplyImprovedForceResistance(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect;
    int nAmount;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(11)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    if (nAmount != 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType =
            CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_INCREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        pEffect->SetInteger(0, nAmount);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053A580
int CSWSItemPropertyHandler::ApplyChangedSavingThrow(CSWSItem* pItem,
                                                     int nSaveValue,
                                                     CSWItemProperty* pProperty,
                                                     CSWSCreature* pCreature,
                                                     int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);

    if (nSaveValue >= 0)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_SAVING_THROW_INCREASE;
    }
    else
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_SAVING_THROW_DECREASE;
        nSaveValue = -nSaveValue;
    }
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pEffect->SetCreator(pItem->m_nId);
    switch (pProperty->m_nSubtype)
    {
    case 1:
        pEffect->SetInteger(1, 1);
        break;
    case 2:
        pEffect->SetInteger(1, 2);
        break;
    case 3:
        pEffect->SetInteger(1, 3);
        break;
    default:
        break;
    }
    pEffect->SetInteger(0, nSaveValue);
    pEffect->SetInteger(2, 0);
    pEffect->SetInteger(3, g_pRules->m_nNumRaces);
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);

    return 0;
}
// PC 0x0053A690
int CSWSItemPropertyHandler::ApplySpecialWalk(CSWSItem* pItem,
                                              CSWItemProperty* pProperty,
                                              CSWSCreature* pCreature,
                                              unsigned long nIndex,
                                              int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);

    pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
                          SUBTYPE_MAGICAL;
    {
        unsigned long oidItem = pItem->m_nId;

        pEffect->SetCreator(oidItem);
    }
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_SETWALKANIMATION;
    pEffect->SetInteger(0, pProperty->m_nSubtype);
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);

    pEffect = new CGameEffect(1);
    pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
                          SUBTYPE_MAGICAL;
    {
        unsigned long oidItem = pItem->m_nId;

        pEffect->SetCreator(oidItem);
    }
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_LIMIT_MOVEMENT_SPEED;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);

    return 0;
}
// PC 0x0053A7E0
int CSWSItemPropertyHandler::ApplyTrueSeeing(CSWSItem* pItem,
                                             CSWItemProperty* pProperty,
                                             CSWSCreature* pCreature,
                                             unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);

    pEffect->m_nType = CSWSEffectListHandler::EFFECT_TRUESEEING;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pEffect->SetCreator(pItem->m_nId);
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);

    return 0;
}
// PC 0x0053A880
int CSWSItemPropertyHandler::ApplyFreedomOfMovement(CSWSItem* pItem,
                                                    CSWItemProperty* pProperty,
                                                    CSWSCreature* pCreature,
                                                    unsigned long nIndex,
                                                    int bLoading)
{
    {
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_IMMUNITY;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        {
            unsigned long oidItem = pItem->m_nId;

            pEffect->SetCreator(oidItem);
        }
        pEffect->SetInteger(0, CSWSEffectListHandler::IMMUNITY_TYPE_PARALYSIS);
        pEffect->SetInteger(1, g_pRules->m_nNumRaces);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);
    }
    {
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_IMMUNITY;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        {
            unsigned long oidItem = pItem->m_nId;

            pEffect->SetCreator(oidItem);
        }
        pEffect->SetInteger(0, CSWSEffectListHandler::IMMUNITY_TYPE_SLOW);
        pEffect->SetInteger(1, g_pRules->m_nNumRaces);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);
    }
    {
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_IMMUNITY;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        {
            unsigned long oidItem = pItem->m_nId;

            pEffect->SetCreator(oidItem);
        }
        pEffect->SetInteger(0, CSWSEffectListHandler::IMMUNITY_TYPE_ENTANGLE);
        pEffect->SetInteger(1, g_pRules->m_nNumRaces);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);
    }
    {
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_IMMUNITY;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        {
            unsigned long oidItem = pItem->m_nId;

            pEffect->SetCreator(oidItem);
        }
        pEffect->SetInteger(
            0, CSWSEffectListHandler::IMMUNITY_TYPE_MOVEMENT_SPEED_DECREASE);
        pEffect->SetInteger(1, g_pRules->m_nNumRaces);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);
    }
    return 0;
}
// PC 0x0053AB00
int CSWSItemPropertyHandler::ApplyBonusFeat(CSWSItem* pItem,
                                            CSWItemProperty* pProperty,
                                            CSWSCreature* pCreature,
                                            unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect;

    if (pCreature->m_pStats != NULL)
    {
        pEffect = new CGameEffect(1);
        if (pEffect != NULL)
        {
            pEffect->m_nType = CSWSEffectListHandler::EFFECT_BONUSFEAT;
            pEffect->m_nSubType =
                (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) | 3;
            pEffect->SetInteger(0, pProperty->m_nSubtype);
            pEffect->SetCreator(pItem->m_nId);
            ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);
        }
    }

    return 0;
}
// PC 0x0053ABD0
int CSWSItemPropertyHandler::ApplyDamageImmunity(CSWSItem* pItem,
                                                 CSWItemProperty* pProperty,
                                                 CSWSCreature* pCreature,
                                                 unsigned long nIndex,
                                                 int bLoading)
{
    unsigned char nCostTable;
    CTwoDimArrays* p2DArrays;
    C2DA* pCostTable;
    CGameEffect* pEffect;
    int nDamageFlags;
    int nAmount;

    nCostTable = pProperty->m_nCostTable;
    p2DArrays = g_pRules->m_p2DArrays;
    pCostTable = p2DArrays->GetIPRPCostTable(nCostTable);
    if (pCostTable != NULL)
    {
        short nDamageType = pProperty->m_nSubtype;
        {
            CExoString sColumn("Value");
            unsigned short nCostValue = pProperty->m_nCostValue;

            pCostTable->GetINTEntry(nCostValue, sColumn, &nAmount);
        }
        nDamageFlags = 1 << nDamageType;
        pEffect = new CGameEffect(1);
        if (pEffect != NULL)
        {
            pEffect->m_nType =
                CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_INCREASE;
            pEffect->m_nSubType =
                (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) | 3;
            pEffect->SetInteger(0, nDamageFlags);
            pEffect->SetInteger(1, nAmount);
            pEffect->SetCreator(pItem->m_nId);
            ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);
        }
    }

    return 0;
}
// PC 0x0053AD10
int CSWSItemPropertyHandler::ApplyDamagePenalty(CSWSItem* pItem,
                                                CSWItemProperty* pProperty,
                                                CSWSCreature* pCreature,
                                                unsigned long nIndex,
                                                int bLoading)
{
    CGameEffect* pEffect;
    CGameEffect* pOffHandEffect;
    int nBaseItem;
    int nAmount;

    nBaseItem = pItem->m_nBaseItem;
    g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
    pEffect = new CGameEffect(1);
    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    nAmount = -nAmount;
    if (nAmount == 0)
    {
        return 0;
    }
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    {
        unsigned long oidItem = pItem->m_nId;

        pEffect->SetCreator(oidItem);
    }
    if (nIndex == 16)
    {
        pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_ONHAND);
    }
    else if (nIndex == 32)
    {
        pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND);
    }
    else if (nIndex == 16384)
    {
        pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1);
    }
    else if (nIndex == 32768)
    {
        pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2);
    }
    else if (nIndex == 65536)
    {
        pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3);
    }
    else if (nIndex == 8)
    {
        pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_BONUS_UNARMED);
    }
    pEffect->SetInteger(0, nAmount);
    pEffect->SetInteger(1, pItem->GetDamageFlags());
    pEffect->SetInteger(2, g_pRules->m_nNumRaces);
    nBaseItem = pItem->m_nBaseItem;
    if (g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nWeaponWield == 3)
    {
        pOffHandEffect = new CGameEffect(0);
        pOffHandEffect->CopyEffect(pEffect, 0);
        pOffHandEffect->SetInteger(5,
                                   CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND);
        ((CSWSObject*)pCreature)->ApplyEffect(pOffHandEffect, bLoading, 0);
    }
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);

    return 0;
}
// PC 0x0053AF50
int CSWSItemPropertyHandler::ApplyDamageReduction(CSWSItem* pItem,
                                                  CSWItemProperty* pProperty,
                                                  CSWSCreature* pCreature,
                                                  unsigned long nIndex,
                                                  int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);
    int nAmount;

    pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pEffect->SetCreator(pItem->m_nId);
    {
        CExoString sColumn("Amount");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(6)->GetINTEntry(nCostValue, sColumn,
                                                    &nAmount);
    }
    pEffect->SetInteger(0, nAmount);
    pEffect->SetInteger(1, pProperty->m_nSubtype + 1);
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);

    return 0;
}
// PC 0x0053B070
int CSWSItemPropertyHandler::ApplyDamageResistance(CSWSItem* pItem,
                                                   CSWItemProperty* pProperty,
                                                   CSWSCreature* pCreature,
                                                   unsigned long nIndex,
                                                   int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);
    int nAmount;

    pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pEffect->SetCreator(pItem->m_nId);
    {
        CExoString sColumn("Amount");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(7)->GetINTEntry(nCostValue, sColumn,
                                                    &nAmount);
    }
    pEffect->SetInteger(0, 1 << pProperty->m_nSubtype);
    pEffect->SetInteger(1, nAmount);
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);

    return 0;
}
// PC 0x0053B190
int CSWSItemPropertyHandler::ApplyDamageVulnerability(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);
    short nDamageType = pProperty->m_nSubtype;
    int nAmount;

    pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_DECREASE;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    {
        unsigned long oidItem = pItem->m_nId;

        pEffect->SetCreator(oidItem);
    }
    {
        CExoString sColumn("Value");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        unsigned short nCostValue = pProperty->m_nCostValue;

        p2DArrays->GetIPRPCostTable(22)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    pEffect->SetInteger(0, 1 << nDamageType);
    pEffect->SetInteger(1, nAmount);
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);

    return 0;
}
// PC 0x0053B2B0
int CSWSItemPropertyHandler::ApplyDecreaseAbilityScore(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect;
    int nAmount;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(21)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    nAmount = -nAmount;
    if (nAmount != 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        switch (pProperty->m_nSubtype)
        {
        case 0:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH);
            break;
        case 1:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_DEXTERITY);
            break;
        case 2:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_CONSTITUTION);
            break;
        case 3:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_INTELLIGENCE);
            break;
        case 4:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_WISDOM);
            break;
        case 5:
            pEffect->SetInteger(0, CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA);
            break;
        default:
            break;
        }
        pEffect->SetInteger(1, nAmount);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053B430
int CSWSItemPropertyHandler::ApplyDecreaseAC(CSWSItem* pItem,
                                             CSWItemProperty* pProperty,
                                             CSWSCreature* pCreature,
                                             unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect;
    int nAmount;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    nAmount = -nAmount;
    if (nAmount != 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_AC_DECREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        pEffect->SetInteger(0, pProperty->m_nSubtype);
        pEffect->SetInteger(1, nAmount);
        pEffect->SetInteger(2, g_pRules->m_nNumRaces);
        pEffect->SetInteger(5, CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053B580
int CSWSItemPropertyHandler::ApplyDecreaseSkillModifier(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex, int bLoading)
{
    CGameEffect* pEffect;
    int nAmount;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(21)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    nAmount = -nAmount;
    if (nAmount != 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_SKILL_DECREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        pEffect->SetInteger(0, pProperty->m_nSubtype);
        pEffect->SetInteger(1, nAmount);
        pEffect->SetInteger(2, g_pRules->m_nNumRaces);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053B6C0
int CSWSItemPropertyHandler::ApplyImmunity(CSWSItem* pItem,
                                           CSWItemProperty* pProperty,
                                           CSWSCreature* pCreature,
                                           unsigned long nIndex,
                                           int bLoading)
{
    int nImmunity;
    int nSecondImmunity;
    int bSecondImmunity = 0;
    CGameEffect* pEffect;

    switch (pProperty->m_nSubtype)
    {
    case CSWItemProperty::IMMUNITY_SUBTYPE_SNEAK_ATTACK:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_BACKSTAB;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_NEGATIVE_LEVEL:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_NEGATIVE_LEVEL;
        nSecondImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_ABILITY_DECREASE;
        bSecondImmunity = 1;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_MIND_SPELLS:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_MIND_AFFECTING;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_POISON:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_POISON;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_DISEASE:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_DISEASE;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_FEAR:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_FEAR;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_KNOCKDOWN:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_KNOCKDOWN;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_PARALYSIS:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_PARALYSIS;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_CRITICAL_HITS:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_CRITICAL_HITS;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_DEATH:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_DEATH_MAGIC;
        break;
    default:
        return 0;
    }

    pEffect = new CGameEffect(1);
    if (pEffect == NULL)
        return 0;

    pEffect->m_nType = CSWSEffectListHandler::EFFECT_IMMUNITY;
    pEffect->m_nSubType =
        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
        CGameEffect::DURATION_EQUIPPED;
    pEffect->SetInteger(0, nImmunity);
    pEffect->SetInteger(1, g_pRules->m_nNumRaces);
    pEffect->SetCreator(pItem->GetId());
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);

    if (bSecondImmunity)
    {
        pEffect = new CGameEffect(1);
        if (pEffect == NULL)
            return 0;
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_IMMUNITY;
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_EQUIPPED;
        pEffect->SetInteger(0, nSecondImmunity);
        pEffect->SetInteger(1, g_pRules->m_nNumRaces);
        pEffect->SetCreator(pItem->GetId());
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, bLoading);
    }
    return 0;
}
// PC 0x0053B8D0
int CSWSItemPropertyHandler::ApplySkillBonus(CSWSItem* pItem,
                                             CSWItemProperty* pProperty,
                                             CSWSCreature* pCreature,
                                             unsigned long nIndex, int bLoading)
{
    int nAmount = pProperty->m_nCostValue;
    CGameEffect* pEffect;

    if (nAmount > 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_SKILL_INCREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        pEffect->SetInteger(0, pProperty->m_nSubtype);
        pEffect->SetInteger(1, nAmount);
        pEffect->SetInteger(2, g_pRules->m_nNumRaces);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053B9C0
int CSWSItemPropertyHandler::ApplyRegeneration(CSWSItem* pItem,
                                               CSWItemProperty* pProperty,
                                               CSWSCreature* pCreature,
                                               unsigned long nIndex,
                                               int bLoading)
{
    CGameEffect* pEffect = new CGameEffect(1);
    int nAmount = pProperty->m_nCostValue;

    if (nAmount > 0)
    {
        pEffect->SetCreator(pItem->m_nId);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_REGENERATE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetInteger(0, nAmount);
        pEffect->SetInteger(1, 6000);
        pEffect->SetInteger(4, pProperty->m_nPropertyType);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053BAB0
int CSWSItemPropertyHandler::ApplyBlasterBoltDeflectionIncrease(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex, int bLoading)
{
    int nAmount = pProperty->m_nCostValue;
    CGameEffect* pEffect;

    if (nAmount > 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType =
            CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONINCREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        pEffect->SetInteger(0, pProperty->m_nSubtype);
        pEffect->SetInteger(1, nAmount);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053BB80
int CSWSItemPropertyHandler::ApplyBlasterBoltDeflectionDecrease(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex, int bLoading)
{
    int nAmount = pProperty->m_nCostValue;
    CGameEffect* pEffect;

    if (nAmount > 0)
    {
        pEffect = new CGameEffect(1);
        pEffect->m_nType =
            CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONDECREASE;
        pEffect->m_nSubType =
            CGameEffect::DURATION_TYPE_EQUIPPED |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetCreator(pItem->m_nId);
        pEffect->SetInteger(0, pProperty->m_nSubtype);
        pEffect->SetInteger(1, nAmount);
        ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);
    }

    return 0;
}
// PC 0x0053BC50
int CSWSItemPropertyHandler::ApplyDisguise(CSWSItem* pItem,
                                           CSWItemProperty* pProperty,
                                           CSWSCreature* pCreature,
                                           unsigned long nIndex, int bLoading)
{
    int nDisguise = pProperty->m_nSubtype;
    CGameEffect* pEffect = new CGameEffect(1);

    pEffect->m_nType = CSWSEffectListHandler::EFFECT_DISGUISE;
    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_EQUIPPED |
                          (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
    pEffect->SetCreator(pItem->m_nId);
    pEffect->SetInteger(0, nDisguise);
    pEffect->SetInteger(1, 0);
    ((CSWSObject*)pCreature)->ApplyEffect(pEffect, bLoading, 0);

    return 0;
}
// PC 0x0053BD10
int CSWSItemPropertyHandler::ApplyDampenSound(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading)
{
    pCreature->SetSilentMove(1);

    return 0;
}
// PC 0x0053BD20
int CSWSItemPropertyHandler::RemoveDampenSound(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex)
{
    pCreature->SetSilentMove(0);

    return 0;
}
// PC 0x0053BD30
int CSWSItemPropertyHandler::ApplyDoorCutting(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading)
{
    if (pItem != NULL)
    {
        pItem->m_n2DC = 1;
    }

    return 0;
}
// PC 0x0053BD50
int CSWSItemPropertyHandler::ApplyDoorSabering(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading)
{
    if (pItem != NULL)
    {
        pItem->m_n2E0 = 1;
    }

    return 0;
}
// PC 0x0053BD70
int CSWSItemPropertyHandler::ApplySniperBonus(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading)
{
    if (pItem != NULL)
    {
        pItem->m_n2E8 = 1;
    }

    return 0;
}
// PC 0x0053BD90
int CSWSItemPropertyHandler::ApplyRapidShotBonus(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading)
{
    if (pItem != NULL)
    {
        pItem->m_n2E4 = 1;
    }

    return 0;
}
// PC 0x0053BDB0
int CSWSItemPropertyHandler::ApplyMaxDexBonus(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading)
{
    if (pItem != NULL)
    {
        pItem->m_n2EC += pProperty->m_nCostValue;
    }

    return 0;
}
// PC 0x0053BDD0
int CSWSItemPropertyHandler::RemoveMaxDexBonus(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex)
{
    if (pItem != NULL)
    {
        pItem->m_n2EC -= pProperty->m_nCostValue;
    }

    return 0;
}
// PC 0x0053BDF0
int CSWSItemPropertyHandler::RemoveEnhancementBonus(CSWSItem* pItem,
                                                       CSWItemProperty* pProperty,
                                                       CSWSCreature* pCreature,
                                                       unsigned long nIndex)
{
    CGameEffect* pEffect;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->m_pIPRPMelee->GetINTEntry(nCostValue, sColumn, &nAmount);
    }
    i = pCreature->m_pStats->m_nAttackEffectIndex;
    while (i < pCreature->m_lstEffects.num &&
           pCreature->m_lstEffects.element[i]->m_nType <=
               CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_ATTACK_INCREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_BONUS) == nAmount)
        {
            if (nIndex == INVENTORY_SLOT_RIGHTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_ONHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_LEFTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_OFFHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_HANDS)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_UNARMED)
                {
                    i++;
                    continue;
                }
            }
            else
            {
                i++;
                continue;
            }
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS ||
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP) !=
                    g_pRules->GetNumRaces())
                {
                    i++;
                    continue;
                }
            }
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP)
            {
                unsigned short nSubtype = pProperty->m_nSubtype;

                if (nSubtype == CSWSCreatureStats::ALIGNMENT_NEUTRAL)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP1) !=
                        CSWSCreatureStats::ALIGNMENT_NEUTRAL)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nSubtype == CSWSCreatureStats::ALIGNMENT_GOOD)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2) !=
                        CSWSCreatureStats::ALIGNMENT_GOOD)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nSubtype == CSWSCreatureStats::ALIGNMENT_EVIL)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2) !=
                        CSWSCreatureStats::ALIGNMENT_EVIL)
                    {
                        i++;
                        continue;
                    }
                }
            }
            else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_RACIAL_GROUP)
            {
                unsigned short nSubtype = pProperty->m_nSubtype;

                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP) !=
                    nSubtype)
                {
                    i++;
                    continue;
                }
            }
            pCreature->RemoveEffectById(pEffect->m_nID);
            i = pCreature->m_pStats->m_nAttackEffectIndex;
            continue;
        }
        else if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE &&
                 (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                     CGameEffect::DURATION_TYPE_EQUIPPED &&
                 pEffect->m_oidCreator == pItem->m_nId &&
                 pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS) == nAmount)
        {
            if (nIndex == INVENTORY_SLOT_RIGHTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_ONHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_LEFTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_HANDS)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_UNARMED)
                {
                    i++;
                    continue;
                }
            }
            else
            {
                i++;
                continue;
            }
            if (!(pItem->GetDamageFlags() &
                  pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_TYPE)))
            {
                i++;
                continue;
            }
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS ||
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP) !=
                    g_pRules->GetNumRaces())
                {
                    i++;
                    continue;
                }
            }
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP)
            {
                unsigned short nSubtype = pProperty->m_nSubtype;

                if (nSubtype == CSWSCreatureStats::ALIGNMENT_NEUTRAL)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP1) !=
                        CSWSCreatureStats::ALIGNMENT_NEUTRAL)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nSubtype == CSWSCreatureStats::ALIGNMENT_GOOD)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2) !=
                        CSWSCreatureStats::ALIGNMENT_GOOD)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nSubtype == CSWSCreatureStats::ALIGNMENT_EVIL)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2) !=
                        CSWSCreatureStats::ALIGNMENT_EVIL)
                    {
                        i++;
                        continue;
                    }
                }
            }
            else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_RACIAL_GROUP)
            {
                unsigned short nSubtype = pProperty->m_nSubtype;

                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP) !=
                    nSubtype)
                {
                    i++;
                    continue;
                }
            }
            pCreature->RemoveEffectById(pEffect->m_nID);
            break;
        }
        i++;
    }
    return 0;
}
// PC 0x0053C250
int CSWSItemPropertyHandler::RemoveEnhancementPenalty(CSWSItem* pItem,
                                                       CSWItemProperty* pProperty,
                                                       CSWSCreature* pCreature,
                                                       unsigned long nIndex)
{
    CGameEffect* pEffect;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)
            ->GetINTEntry(nCostValue, sColumn, &nAmount);
    }
    nAmount = -nAmount;
    i = pCreature->m_pStats->m_nAttackEffectIndex;
    while (i < pCreature->m_lstEffects.num &&
           pCreature->m_lstEffects.element[i]->m_nType <=
               CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_ATTACK_DECREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_BONUS) == nAmount &&
            pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP) ==
                g_pRules->GetNumRaces())
        {
            if (nIndex == INVENTORY_SLOT_RIGHTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_ONHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_LEFTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_OFFHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_HANDS)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_UNARMED)
                {
                    i++;
                    continue;
                }
            }
            else
            {
                i++;
                continue;
            }
            pCreature->RemoveEffectById(pEffect->m_nID);
            i = pCreature->m_pStats->m_nAttackEffectIndex;
            continue;
        }
        else if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE &&
                 (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                     CGameEffect::DURATION_TYPE_EQUIPPED &&
                 pEffect->m_oidCreator == pItem->m_nId &&
                 pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS) == nAmount &&
                 pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP) ==
                     g_pRules->GetNumRaces())
        {
            if (nIndex == INVENTORY_SLOT_RIGHTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_ONHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_LEFTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_HANDS)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_UNARMED)
                {
                    i++;
                    continue;
                }
            }
            else
            {
                i++;
                continue;
            }
            if (pItem->GetDamageFlags() &
                pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_TYPE))
            {
                pCreature->RemoveEffectById(pEffect->m_nID);
                break;
            }
        }
        i++;
    }
    return 0;
}
// PC 0x0053C810
int CSWSItemPropertyHandler::RemoveDamageBonus(CSWSItem* pItem,
                                               CSWItemProperty* pProperty,
                                               CSWSCreature* pCreature,
                                               unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned char nNumRaces;
    int nAmount = pProperty->m_nCostValue;
    int nDamageType;
    int i;

    if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS)
    {
        nDamageType = pProperty->m_nSubtype;
    }
    else
    {
        nDamageType = pProperty->m_nParam1Value;
    }
    i = pCreature->m_pStats->m_nDamageIncreaseEffectIndex;
    while (i < pCreature->m_lstEffects.num &&
           pCreature->m_lstEffects.element[i]->m_nType <=
               CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS) ==
                nAmount)
        {
            if (nIndex == INVENTORY_SLOT_RIGHTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_ONHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_LEFTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_HANDS)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::DAMAGE_BONUS_UNARMED)
                {
                    i++;
                    continue;
                }
            }
            else
            {
                i++;
                continue;
            }
            if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_TYPE) !=
                (1 << nDamageType))
            {
                i++;
                continue;
            }
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS ||
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_ALIGNMENT_GROUP)
            {
                nNumRaces = g_pRules->m_nNumRaces;
                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP) !=
                    nNumRaces)
                {
                    i++;
                    continue;
                }
            }
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_ALIGNMENT_GROUP)
            {
                unsigned short nSubtype = pProperty->m_nSubtype;

                if (nSubtype == CSWSCreatureStats::ALIGNMENT_NEUTRAL)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP1) !=
                        CSWSCreatureStats::ALIGNMENT_NEUTRAL)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nSubtype == CSWSCreatureStats::ALIGNMENT_GOOD)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2) !=
                        CSWSCreatureStats::ALIGNMENT_GOOD)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nSubtype == CSWSCreatureStats::ALIGNMENT_EVIL)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_ALIGNMENT_GROUP2) !=
                        CSWSCreatureStats::ALIGNMENT_EVIL)
                    {
                        i++;
                        continue;
                    }
                }
            }
            else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_RACIAL_GROUP)
            {
                unsigned short nSubtype = pProperty->m_nSubtype;

                if (pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP) !=
                    nSubtype)
                {
                    i++;
                    continue;
                }
            }
            pCreature->RemoveEffectById(pEffect->m_nID);
            break;
        }
        i++;
    }

    return 0;
}
// PC 0x0053CA70
int CSWSItemPropertyHandler::RemoveAttackBonus(CSWSItem* pItem,
                                               CSWItemProperty* pProperty,
                                               CSWSCreature* pCreature,
                                               unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned char nNumRaces;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pCostTable = p2DArrays->m_pIPRPMelee;

        pCostTable->GetINTEntry(nCostValue, sColumn, &nAmount);
    }
    i = pCreature->m_pStats->m_nAttackEffectIndex;
    while (i < pCreature->m_lstEffects.num &&
           pCreature->m_lstEffects.element[i]->m_nType <=
               CSWSEffectListHandler::EFFECT_ATTACK_INCREASE)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_ATTACK_INCREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_BONUS) ==
                nAmount)
        {
            if (nIndex == INVENTORY_SLOT_RIGHTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_ONHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_LEFTHAND)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_OFFHAND)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2)
                {
                    i++;
                    continue;
                }
            }
            else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
            {
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                    CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3)
                {
                    i++;
                    continue;
                }
            }
            else
            {
                if (nIndex != INVENTORY_SLOT_HANDS ||
                    pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_WEAPON) !=
                        CSWSEffectListHandler::ATTACK_BONUS_UNARMED)
                {
                    i++;
                    continue;
                }
            }
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS ||
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_ALIGNMENT_GROUP)
            {
                nNumRaces = g_pRules->m_nNumRaces;
                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP) !=
                    nNumRaces)
                {
                    i++;
                    continue;
                }
            }
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_ALIGNMENT_GROUP)
            {
                unsigned short nSubtype = pProperty->m_nSubtype;

                if (nSubtype == CSWSCreatureStats::ALIGNMENT_NEUTRAL)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP1) !=
                        CSWSCreatureStats::ALIGNMENT_NEUTRAL)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nSubtype == CSWSCreatureStats::ALIGNMENT_GOOD)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2) !=
                        CSWSCreatureStats::ALIGNMENT_GOOD)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nSubtype == CSWSCreatureStats::ALIGNMENT_EVIL)
                {
                    if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_ALIGNMENT_GROUP2) !=
                        CSWSCreatureStats::ALIGNMENT_EVIL)
                    {
                        i++;
                        continue;
                    }
                }
            }
            else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_RACIAL_GROUP)
            {
                unsigned short nSubtype = pProperty->m_nSubtype;

                if (pEffect->GetInteger(CSWSEffectListHandler::ATTACK_INTEGER_RACIAL_GROUP) !=
                    nSubtype)
                {
                    i++;
                    continue;
                }
            }
            pCreature->RemoveEffectById(pEffect->m_nID);
            break;
        }
        i++;
    }

    return 0;
}
// PC 0x0053CCE0
int CSWSItemPropertyHandler::RemoveAttackPenalty(CSWSItem* pItem,
                                                CSWItemProperty* pProperty,
                                                CSWSCreature* pCreature,
                                                unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned char nNumRaces;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)
            ->GetINTEntry(nCostValue, sColumn, &nAmount);
    }
    nAmount = -nAmount;
    i = pCreature->m_pStats->m_nAttackEffectIndex;
    while (i < pCreature->m_lstEffects.num &&
           pCreature->m_lstEffects.element[i]->m_nType <=
               CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_ATTACK_DECREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            if (pEffect->GetInteger(0) == nAmount)
            {
                nNumRaces = g_pRules->m_nNumRaces;
                if (pEffect->GetInteger(2) == nNumRaces)
                {
                    if (nIndex == INVENTORY_SLOT_RIGHTHAND)
                    {
                        if (pEffect->GetInteger(1) !=
                            CSWSEffectListHandler::ATTACK_BONUS_ONHAND)
                        {
                            i++;
                            continue;
                        }
                    }
                    else if (nIndex == INVENTORY_SLOT_LEFTHAND)
                    {
                        if (pEffect->GetInteger(1) !=
                            CSWSEffectListHandler::ATTACK_BONUS_OFFHAND)
                        {
                            i++;
                            continue;
                        }
                    }
                    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
                    {
                        if (pEffect->GetInteger(1) !=
                            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON1)
                        {
                            i++;
                            continue;
                        }
                    }
                    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
                    {
                        if (pEffect->GetInteger(1) !=
                            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON2)
                        {
                            i++;
                            continue;
                        }
                    }
                    else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
                    {
                        if (pEffect->GetInteger(1) !=
                            CSWSEffectListHandler::ATTACK_BONUS_CWEAPON3)
                        {
                            i++;
                            continue;
                        }
                    }
                    else if (nIndex == INVENTORY_SLOT_HANDS)
                    {
                        if (pEffect->GetInteger(1) !=
                            CSWSEffectListHandler::ATTACK_BONUS_UNARMED)
                        {
                            i++;
                            continue;
                        }
                    }
                    else
                    {
                        i++;
                        continue;
                    }
                    pCreature->RemoveEffectById(pEffect->m_nID);
                    break;
                }
            }
        }
        i++;
    }

    return 0;
}
// PC 0x0053CED0
int CSWSItemPropertyHandler::RemoveAbilityBonus(CSWSItem* pItem,
                                                CSWItemProperty* pProperty,
                                                CSWSCreature* pCreature,
                                                unsigned long nIndex)
{
    CGameEffect* pEffect;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(1)->GetINTEntry(nCostValue, sColumn,
                                                    &nAmount);
    }
    for (i = pCreature->m_pStats->m_nAbilityEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_ABILITY_INCREASE;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_ABILITY_INCREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            if (pEffect->GetInteger(1) == nAmount)
            {
                switch (pProperty->m_nSubtype)
                {
                case 0:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH)
                    {
                        continue;
                    }
                    break;
                case 1:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_DEXTERITY)
                    {
                        continue;
                    }
                    break;
                case 2:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_CONSTITUTION)
                    {
                        continue;
                    }
                    break;
                case 3:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_INTELLIGENCE)
                    {
                        continue;
                    }
                    break;
                case 4:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_WISDOM)
                    {
                        continue;
                    }
                    break;
                case 5:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA)
                    {
                        continue;
                    }
                    break;
                default:
                    break;
                }
                ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
                break;
            }
        }
    }

    return 0;
}
// PC 0x0053D090
int CSWSItemPropertyHandler::RemoveACBonus(CSWSItem* pItem,
                                           CSWItemProperty* pProperty,
                                           CSWSCreature* pCreature,
                                           unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned short nSubtype;
    int nBaseItem;
    int nACEnchantType;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(1)->GetINTEntry(nCostValue, sColumn,
                                                    &nAmount);
    }
    nBaseItem = pItem->m_nBaseItem;
    nACEnchantType =
        g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_nACEnchantType;
    i = pCreature->m_pStats->m_nACEffectIndex;
    while (i < pCreature->m_lstEffects.num &&
           pCreature->m_lstEffects.element[i]->m_nType <=
               CSWSEffectListHandler::EFFECT_AC_INCREASE)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_AC_INCREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            pEffect->GetInteger(0) == nACEnchantType &&
            pEffect->GetInteger(1) == nAmount)
        {
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_ALIGNMENT_GROUP)
            {
                int nAlignmentGroup = pProperty->m_nSubtype;
                if (nAlignmentGroup > 0 && nAlignmentGroup <= 3)
                {
                    nSubtype = pProperty->m_nSubtype;
                    if (pEffect->GetInteger(4) != nSubtype)
                    {
                        i++;
                        continue;
                    }
                }
            }
            else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_RACIAL_GROUP)
            {
                nSubtype = pProperty->m_nSubtype;
                if (pEffect->GetInteger(2) != nSubtype)
                {
                    i++;
                    continue;
                }
            }
            else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_DAMAGE_TYPE)
            {
                nSubtype = pProperty->m_nSubtype;
                if (pEffect->GetInteger(5) != nSubtype)
                {
                    i++;
                    continue;
                }
            }
            pCreature->RemoveEffectById(pEffect->m_nID);
            break;
        }
        i++;
    }

    return 0;
}
// PC 0x0053D230
int CSWSItemPropertyHandler::RemoveImprovedForceResistance(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex)
{
    CGameEffect* pEffect;
    int nValue;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(11)->GetINTEntry(nCostValue, sColumn,
                                                     &nValue);
    }
    for (i = pCreature->m_pStats->m_nForceResistanceEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_INCREASE;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_INCREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            if (pEffect->GetInteger(0) == nValue)
            {
                ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
                break;
            }
        }
    }

    return 0;
}
// PC 0x0053D350
int CSWSItemPropertyHandler::ApplyImprovedSavingThrow(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex, int bLoading)
{
    int nValue;
    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(1)->GetINTEntry(nCostValue, sColumn,
                                                    &nValue);
    }
    if (nValue != 0)
    {
        ApplyChangedSavingThrow(pItem, nValue, pProperty, pCreature, bLoading);
    }
    return 0;
}
// PC 0x0053D400
int CSWSItemPropertyHandler::ApplyReducedSavingThrow(CSWSItem* pItem,
                                                     CSWItemProperty* pProperty,
                                                     CSWSCreature* pCreature,
                                                     unsigned long nIndex,
                                                     int bLoading)
{
    int nValue;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)->GetINTEntry(nCostValue, sColumn,
                                                     &nValue);
    }
    if (nValue != 0)
    {
        ApplyChangedSavingThrow(pItem, nValue, pProperty, pCreature, bLoading);
    }

    return 0;
}
// PC 0x0053D4B0
int CSWSItemPropertyHandler::RemoveChangedSavingThrow(
    int nSaveValue, CSWSItem* pItem, CSWItemProperty* pProperty,
    CSWSCreature* pCreature)
{
    CGameEffect* pEffect;
    int nSubtype = pProperty->m_nSubtype;
    int nEffectType;
    int nEffectSubtype;
    unsigned char nNumRaces;
    int i;

    for (i = 0; i < pCreature->m_lstEffects.num; i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        nEffectType = pEffect->m_nType;
        if (nEffectType !=
                CSWSEffectListHandler::EFFECT_SAVING_THROW_INCREASE &&
            nEffectType != CSWSEffectListHandler::EFFECT_SAVING_THROW_DECREASE)
        {
            continue;
        }
        if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) !=
                CGameEffect::DURATION_TYPE_EQUIPPED ||
            pEffect->m_oidCreator != pItem->m_nId ||
            pEffect->GetInteger(0) != nSaveValue || pEffect->GetInteger(1) != 0)
        {
            continue;
        }
        nNumRaces = g_pRules->m_nNumRaces;
        if (pEffect->GetInteger(3) != nNumRaces)
        {
            continue;
        }
        nEffectSubtype = pCreature->m_lstEffects.element[i]->GetInteger(1);
        if (nEffectSubtype == 1)
        {
            if (nSubtype != 1)
            {
                continue;
            }
        }
        else if (nEffectSubtype == 3)
        {
            if (nSubtype != 3)
            {
                continue;
            }
        }
        else if (nEffectSubtype == 2)
        {
            if (nSubtype != 2)
            {
                continue;
            }
        }
        else
        {
            continue;
        }
        ((CSWSObject*)pCreature)
            ->RemoveEffectById(pCreature->m_lstEffects.element[i]->m_nID);
        return 0;
    }

    return 0;
}
// PC 0x0053D5D0
int CSWSItemPropertyHandler::RemoveSpecialWalk(CSWSItem* pItem,
                                               CSWItemProperty* pProperty,
                                               CSWSCreature* pCreature,
                                               unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned short nWalk;
    int i;

    for (i = pCreature->m_pStats->m_nSpecialWalkEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_LIMIT_MOVEMENT_SPEED;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_SETWALKANIMATION &&
            pEffect->m_oidCreator == pItem->m_nId &&
            (nWalk = pProperty->m_nSubtype, pEffect->GetInteger(0) == nWalk) &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED)
        {
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
        }
        else if (pEffect->m_nType ==
                     CSWSEffectListHandler::EFFECT_LIMIT_MOVEMENT_SPEED &&
                 pEffect->m_oidCreator == pItem->m_nId &&
                 (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                     CGameEffect::DURATION_TYPE_EQUIPPED)
        {
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
            break;
        }
    }

    return 0;
}
// PC 0x0053D6C0
int CSWSItemPropertyHandler::RemoveTrueSeeing(CSWSItem* pItem,
                                              CSWItemProperty* pProperty,
                                              CSWSCreature* pCreature,
                                              unsigned long nIndex)
{
    CGameEffect* pEffect;
    int i;

    for (i = 0; i < pCreature->m_lstEffects.num; i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_TRUESEEING &&
            pEffect->m_oidCreator == pItem->m_nId &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED)
        {
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
            break;
        }
    }

    return 0;
}
// PC 0x0053D740
int CSWSItemPropertyHandler::RemoveFreedomOfMovement(CSWSItem* pItem,
                                                     CSWItemProperty* pProperty,
                                                     CSWSCreature* pCreature,
                                                     unsigned long nIndex)
{
    CGameEffect* pEffect;
    int i;

    for (i = pCreature->m_pStats->m_nImmunityEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_IMMUNITY;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_IMMUNITY &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            (pEffect->GetInteger(0) ==
                 CSWSEffectListHandler::IMMUNITY_TYPE_PARALYSIS ||
             pEffect->GetInteger(0) ==
                 CSWSEffectListHandler::IMMUNITY_TYPE_SLOW ||
             pEffect->GetInteger(0) ==
                 CSWSEffectListHandler::IMMUNITY_TYPE_ENTANGLE ||
             pEffect->GetInteger(0) ==
                 CSWSEffectListHandler::IMMUNITY_TYPE_MOVEMENT_SPEED_DECREASE))
        {
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
        }
    }

    return 0;
}
// PC 0x0053D820
int CSWSItemPropertyHandler::RemoveBonusFeat(CSWSItem* pItem,
                                             CSWItemProperty* pProperty,
                                             CSWSCreature* pCreature,
                                             unsigned long nIndex)
{
    CExoArrayList<CGameEffect*>* pEffects;
    CGameEffect* pEffect;
    unsigned short nFeat;
    int i;

    if (pCreature->m_pStats == NULL)
    {
        return 0;
    }
    pEffects = &pCreature->m_lstEffects;
    if (pEffects == NULL)
    {
        return 0;
    }
    for (i = 0; i < pEffects->num; i++)
    {
        pEffect = pEffects->element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_BONUSFEAT &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            nFeat = pProperty->m_nSubtype;
            if (pEffect->GetInteger(0) == nFeat &&
                (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                    CGameEffect::DURATION_TYPE_EQUIPPED)
            {
                ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
                break;
            }
        }
    }

    return 0;
}
// PC 0x0053D8C0
int CSWSItemPropertyHandler::RemoveDamageImmunity(CSWSItem* pItem,
                                                  CSWItemProperty* pProperty,
                                                  CSWSCreature* pCreature,
                                                  unsigned long nIndex)
{
    unsigned short nDamageType = pProperty->m_nSubtype;
    CExoArrayList<CGameEffect*>* pEffects = &pCreature->m_lstEffects;
    unsigned char nCostTable;
    CTwoDimArrays* p2DArrays;
    C2DA* pCostTable;
    CGameEffect* pEffect;
    int nAmount;
    int nDamageFlags;
    int i;

    if (pEffects == NULL)
    {
        return 0;
    }
    nCostTable = pProperty->m_nCostTable;
    p2DArrays = g_pRules->m_p2DArrays;
    pCostTable = p2DArrays->GetIPRPCostTable(nCostTable);

    if (pCostTable != NULL)
    {
        {
            CExoString sColumn("Value");
            unsigned short nCostValue = pProperty->m_nCostValue;

            pCostTable->GetINTEntry(nCostValue, sColumn, &nAmount);
        }
        switch (nDamageType)
        {
        case 0:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_BLUDGEONING;
            break;
        case 1:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_PIERCING;
            break;
        case 2:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_SLASHING;
            break;
        case 3:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL;
            break;
        case 4:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_ACID;
            break;
        case 5:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_COLD;
            break;
        case 6:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_LIGHT_SIDE;
            break;
        case 7:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_ELECTRICAL;
            break;
        case 8:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_FIRE;
            break;
        case 9:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_DARK_SIDE;
            break;
        case 10:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_SONIC;
            break;
        case 11:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_ION;
            break;
        case 12:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_BLASTER;
            break;
        case 13:
            nDamageFlags = CSWSEffectListHandler::DAMAGE_FLAG_POISON;
            break;
        default:
            return 0;
        }
        i = 0;
        if (pEffects->num > 0)
        {
            while (1)
            {
                pEffect = pEffects->element[i];
                if (pEffect->m_nType == CSWSEffectListHandler::
                                            EFFECT_DAMAGE_IMMUNITY_INCREASE &&
                    pEffect->GetInteger(0) == nDamageFlags)
                {
                    if (pEffect->GetInteger(1) == nAmount &&
                        (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                            CGameEffect::DURATION_TYPE_EQUIPPED &&
                        pEffect->m_oidCreator == pItem->m_nId)
                    {
                        break;
                    }
                }
                i++;
                if (i >= pEffects->num)
                {
                    return 0;
                }
            }
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
        }
    }

    return 0;
}
// PC 0x0053DAB0
int CSWSItemPropertyHandler::RemoveDamagePenalty(CSWSItem* pItem,
                                                CSWItemProperty* pProperty,
                                                CSWSCreature* pCreature,
                                                unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned char nNumRaces;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;

        g_pRules->Get2DArrays()
            ->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)
            ->GetINTEntry(nCostValue, sColumn, &nAmount);
    }
    nAmount = -nAmount;
    i = pCreature->m_pStats->m_nAttackEffectIndex;
    while (i < pCreature->m_lstEffects.num &&
           pCreature->m_lstEffects.element[i]->m_nType <=
               CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_BONUS) ==
                nAmount)
        {
            nNumRaces = g_pRules->m_nNumRaces;
            if (pEffect->GetInteger(
                    CSWSEffectListHandler::DAMAGE_INTEGER_RACIAL_GROUP) ==
                nNumRaces)
            {
                if (nIndex == INVENTORY_SLOT_RIGHTHAND)
                {
                    if (pEffect->GetInteger(
                            CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                        CSWSEffectListHandler::DAMAGE_BONUS_ONHAND)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nIndex == INVENTORY_SLOT_LEFTHAND)
                {
                    if (pEffect->GetInteger(
                            CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                        CSWSEffectListHandler::DAMAGE_BONUS_OFFHAND)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON1)
                {
                    if (pEffect->GetInteger(
                            CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                        CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON1)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON2)
                {
                    if (pEffect->GetInteger(
                            CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                        CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON2)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nIndex == INVENTORY_SLOT_CREATUREWEAPON3)
                {
                    if (pEffect->GetInteger(
                            CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                        CSWSEffectListHandler::DAMAGE_BONUS_CWEAPON3)
                    {
                        i++;
                        continue;
                    }
                }
                else if (nIndex == INVENTORY_SLOT_HANDS)
                {
                    if (pEffect->GetInteger(
                            CSWSEffectListHandler::DAMAGE_INTEGER_WEAPON) !=
                        CSWSEffectListHandler::DAMAGE_BONUS_UNARMED)
                    {
                        i++;
                        continue;
                    }
                }
                else
                {
                    i++;
                    continue;
                }
                if (pItem->GetDamageFlags() &
                    pEffect->GetInteger(CSWSEffectListHandler::DAMAGE_INTEGER_TYPE))
                {
                    pCreature->RemoveEffectById(pEffect->m_nID);
                    break;
                }
            }
        }
        i++;
    }

    return 0;
}
// PC 0x0053DCD0
int CSWSItemPropertyHandler::RemoveDamageReduction(CSWSItem* pItem,
                                                   CSWItemProperty* pProperty,
                                                   CSWSCreature* pCreature,
                                                   unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned short nSubtype;
    int nAmount;
    int i;

    {
        CExoString sColumn("Amount");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(6)->GetINTEntry(nCostValue, sColumn,
                                                    &nAmount);
    }
    for (i = pCreature->m_pStats->m_nDamageReductionEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            if (pEffect->GetInteger(0) == nAmount)
            {
                nSubtype = pProperty->m_nSubtype;
                if (pEffect->GetInteger(1) == nSubtype + 1)
                {
                    ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
                    break;
                }
            }
        }
    }

    return 0;
}
// PC 0x0053DE10
int CSWSItemPropertyHandler::RemoveDamageResistance(CSWSItem* pItem,
                                                    CSWItemProperty* pProperty,
                                                    CSWSCreature* pCreature,
                                                    unsigned long nIndex)
{
    short nDamageType = pProperty->m_nSubtype;
    CGameEffect* pEffect;
    int nAmount;
    int i;

    {
        CExoString sColumn("Amount");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(7)->GetINTEntry(nCostValue, sColumn,
                                                    &nAmount);
    }
    for (i = pCreature->m_pStats->m_nDamageResistanceEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            pEffect->GetInteger(0) == 1 << nDamageType)
        {
            if (pEffect->GetInteger(1) == nAmount)
            {
                ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
                break;
            }
        }
    }

    return 0;
}
// PC 0x0053DF50
int CSWSItemPropertyHandler::RemoveDamageVulnerability(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex)
{
    short nDamageType = pProperty->m_nSubtype;
    CGameEffect* pEffect;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(22)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    for (i = 0; i < pCreature->m_lstEffects.num &&
                pCreature->m_lstEffects.element[i]->m_nType <=
                    CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_DECREASE;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_DECREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId &&
            pEffect->GetInteger(0) == 1 << nDamageType)
        {
            if (pEffect->GetInteger(1) == nAmount)
            {
                ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
                break;
            }
        }
    }

    return 0;
}
// PC 0x0053E080
int CSWSItemPropertyHandler::RemoveDecreaseAbilityScore(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex)
{
    CGameEffect* pEffect;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(21)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    nAmount = -nAmount;
    for (i = pCreature->m_pStats->m_nAbilityEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_ABILITY_DECREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            if (pEffect->GetInteger(1) == nAmount)
            {
                switch (pProperty->m_nSubtype)
                {
                case 0:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH)
                    {
                        continue;
                    }
                    break;
                case 1:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_DEXTERITY)
                    {
                        continue;
                    }
                    break;
                case 2:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_CONSTITUTION)
                    {
                        continue;
                    }
                    break;
                case 3:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_INTELLIGENCE)
                    {
                        continue;
                    }
                    break;
                case 4:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_WISDOM)
                    {
                        continue;
                    }
                    break;
                case 5:
                    if (pEffect->GetInteger(0) !=
                        CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA)
                    {
                        continue;
                    }
                    break;
                default:
                    break;
                }
                ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
                break;
            }
        }
    }

    return 0;
}
// PC 0x0053E250
int CSWSItemPropertyHandler::RemoveDecreaseAC(CSWSItem* pItem,
                                              CSWItemProperty* pProperty,
                                              CSWSCreature* pCreature,
                                              unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned short nSubtype;
    unsigned char nNumRaces;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    nAmount = -nAmount;
    for (i = pCreature->m_pStats->m_nACEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_AC_DECREASE;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_AC_DECREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            nSubtype = pProperty->m_nSubtype;
            if (pEffect->GetInteger(0) == nSubtype)
            {
                if (pEffect->GetInteger(1) == nAmount)
                {
                    nNumRaces = g_pRules->m_nNumRaces;
                    if (pEffect->GetInteger(2) == nNumRaces)
                    {
                        ((CSWSObject*)pCreature)
                            ->RemoveEffectById(pEffect->m_nID);
                        break;
                    }
                }
            }
        }
    }

    return 0;
}
// PC 0x0053E3B0
int CSWSItemPropertyHandler::RemoveDecreaseSkillModifier(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned short nSubtype;
    unsigned char nNumRaces;
    int nAmount;
    int i;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(21)->GetINTEntry(nCostValue, sColumn,
                                                     &nAmount);
    }
    nAmount = -nAmount;
    for (i = pCreature->m_pStats->m_nSkillDecreaseEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_SKILL_DECREASE;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_SKILL_DECREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            nSubtype = pProperty->m_nSubtype;
            if (pEffect->GetInteger(0) == nSubtype)
            {
                if (pEffect->GetInteger(1) == nAmount)
                {
                    nNumRaces = g_pRules->m_nNumRaces;
                    if (pEffect->GetInteger(2) == nNumRaces)
                    {
                        ((CSWSObject*)pCreature)
                            ->RemoveEffectById(pEffect->m_nID);
                        break;
                    }
                }
            }
        }
    }

    return 0;
}
// PC 0x0053E510
int CSWSItemPropertyHandler::RemoveImmunity(CSWSItem* pItem,
                                            CSWItemProperty* pProperty,
                                            CSWSCreature* pCreature,
                                            unsigned long nIndex)
{
    CExoArrayList<CGameEffect*>* pEffects = &pCreature->m_lstEffects;
    int nImmunity;
    int nSecondImmunity;
    int bSecondImmunity = 0;
    CGameEffect* pEffect;
    int i;

    if (pEffects == NULL)
    {
        return 0;
    }
    switch (pProperty->m_nSubtype)
    {
    case CSWItemProperty::IMMUNITY_SUBTYPE_SNEAK_ATTACK:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_BACKSTAB;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_NEGATIVE_LEVEL:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_NEGATIVE_LEVEL;
        nSecondImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_ABILITY_DECREASE;
        bSecondImmunity = 1;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_MIND_SPELLS:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_MIND_AFFECTING;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_POISON:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_POISON;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_DISEASE:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_DISEASE;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_FEAR:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_FEAR;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_KNOCKDOWN:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_KNOCKDOWN;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_PARALYSIS:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_PARALYSIS;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_CRITICAL_HITS:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_CRITICAL_HITS;
        break;
    case CSWItemProperty::IMMUNITY_SUBTYPE_DEATH:
        nImmunity = CSWSEffectListHandler::IMMUNITY_TYPE_DEATH_MAGIC;
        break;
    default:
        return 0;
    }

    for (i = 0; i < pEffects->num; i++)
    {
        pEffect = pEffects->element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_IMMUNITY &&
            pEffect->GetInteger(CSWSEffectListHandler::IMMUNITY_INTEGER_TYPE) ==
                nImmunity &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            pCreature->RemoveEffectById(pEffect->m_nID);
            break;
        }
        if (bSecondImmunity &&
            pEffect->m_nType == CSWSEffectListHandler::EFFECT_IMMUNITY &&
            pEffect->GetInteger(CSWSEffectListHandler::IMMUNITY_INTEGER_TYPE) ==
                nSecondImmunity &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            pCreature->RemoveEffectById(pEffect->m_nID);
            break;
        }
    }

    return 0;
}
// PC 0x0053E6A0
int CSWSItemPropertyHandler::RemoveSkillBonus(CSWSItem* pItem,
                                              CSWItemProperty* pProperty,
                                              CSWSCreature* pCreature,
                                              unsigned long nIndex)
{
    CGameEffect* pEffect;
    unsigned short nSkill;
    unsigned short nValue;
    int i;

    for (i = pCreature->m_pStats->m_nAttackEffectIndex;
         i < pCreature->m_lstEffects.num &&
         pCreature->m_lstEffects.element[i]->m_nType <=
             CSWSEffectListHandler::EFFECT_SKILL_INCREASE;
         i++)
    {
        pEffect = pCreature->m_lstEffects.element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_SKILL_INCREASE &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED &&
            pEffect->m_oidCreator == pItem->m_nId)
        {
            nSkill = pProperty->m_nSubtype;
            if (pEffect->GetInteger(0) == nSkill)
            {
                nValue = pProperty->m_nCostValue;
                if (pEffect->GetInteger(1) == nValue)
                {
                    ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
                    break;
                }
            }
        }
    }

    return 0;
}
// PC 0x0053E770
int CSWSItemPropertyHandler::RemoveRegeneration(CSWSItem* pItem,
                                                CSWItemProperty* pProperty,
                                                CSWSCreature* pCreature,
                                                unsigned long nIndex)
{
    int nAmount = pProperty->m_nCostValue;
    CExoArrayList<CGameEffect*>* pEffects = &pCreature->m_lstEffects;
    CGameEffect* pEffect;
    int i;

    if (pEffects == NULL)
    {
        return 0;
    }
    for (i = 0; i < pEffects->num; i++)
    {
        pEffect = pEffects->element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_REGENERATE &&
            pEffect->GetInteger(0) == nAmount &&
            pEffect->m_oidCreator == pItem->m_nId &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED)
        {
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
            break;
        }
    }

    return 0;
}
// PC 0x0053E800
int CSWSItemPropertyHandler::RemoveBlasterBoltDeflectionIncrease(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex)
{
    int nAmount = pProperty->m_nCostValue;
    CExoArrayList<CGameEffect*>* pEffects = &pCreature->m_lstEffects;
    CGameEffect* pEffect;
    int i;

    if (pEffects == NULL)
    {
        return 0;
    }
    for (i = 0; i < pEffects->num; i++)
    {
        pEffect = pEffects->element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONINCREASE &&
            pEffect->GetInteger(0) == nAmount &&
            pEffect->m_oidCreator == pItem->m_nId &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED)
        {
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
            break;
        }
    }

    return 0;
}
// PC 0x0053E890
int CSWSItemPropertyHandler::RemoveBlasterBoltDeflectionDecrease(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex)
{
    int nAmount = pProperty->m_nCostValue;
    CExoArrayList<CGameEffect*>* pEffects = &pCreature->m_lstEffects;
    CGameEffect* pEffect;
    int i;

    if (pEffects == NULL)
    {
        return 0;
    }
    for (i = 0; i < pEffects->num; i++)
    {
        pEffect = pEffects->element[i];
        if (pEffect->m_nType ==
                CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONDECREASE &&
            pEffect->GetInteger(0) == nAmount &&
            pEffect->m_oidCreator == pItem->m_nId &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED)
        {
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
            break;
        }
    }

    return 0;
}
// PC 0x0053E920
int CSWSItemPropertyHandler::RemoveDisguise(CSWSItem* pItem,
                                            CSWItemProperty* pProperty,
                                            CSWSCreature* pCreature,
                                            unsigned long nIndex)
{
    int nAmount = pProperty->m_nSubtype;
    CExoArrayList<CGameEffect*>* pEffects = &pCreature->m_lstEffects;
    CGameEffect* pEffect;
    int i;

    if (pEffects == NULL)
    {
        return 0;
    }
    for (i = 0; i < pEffects->num; i++)
    {
        pEffect = pEffects->element[i];
        if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_DISGUISE &&
            pEffect->GetInteger(0) == nAmount &&
            pEffect->m_oidCreator == pItem->m_nId &&
            (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_EQUIPPED)
        {
            ((CSWSObject*)pCreature)->RemoveEffectById(pEffect->m_nID);
            break;
        }
    }

    return 0;
}
// PC 0x0053E9B0
int CSWSItemPropertyHandler::RemoveImprovedSavingThrow(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex)
{
    int nValue;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(1)->GetINTEntry(nCostValue, sColumn,
                                                    &nValue);
    }
    if (nValue != 0)
    {
        RemoveChangedSavingThrow(nValue, pItem, pProperty, pCreature);
    }

    return 0;
}
// PC 0x00401590
int CSWSItemPropertyHandler::RemoveLight(
    CSWSItem* pItem, CSWItemProperty* pProperty,
    CSWSCreature* pCreature, unsigned long nIndex)
{
    return 0;
}
// PC 0x0053EA60
int CSWSItemPropertyHandler::RemoveReducedSavingThrow(
    CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
    unsigned long nIndex)
{
    int nValue;

    {
        CExoString sColumn("Value");
        unsigned short nCostValue = pProperty->m_nCostValue;
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;

        p2DArrays->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_MELEE)->GetINTEntry(nCostValue, sColumn,
                                                     &nValue);
    }
    if (nValue != 0)
    {
        RemoveChangedSavingThrow(nValue, pItem, pProperty, pCreature);
    }

    return 0;
}
// PC 0x0053EB10
void CSWSItemPropertyHandler::InitializeItemProperties()
{
    int i;

    m_pApplyItemProperty = new ITEM_PROPERTY_APPLY[70];
    m_pRemoveItemProperty = new ITEM_PROPERTY_REMOVE[70];
    for (i = 0; i < 70; i++)
    {
        m_pApplyItemProperty[i] = NULL;
        m_pRemoveItemProperty[i] = NULL;
    }

    m_pApplyItemProperty[0] = &CSWSItemPropertyHandler::ApplyAbilityBonus;
    m_pRemoveItemProperty[0] = &CSWSItemPropertyHandler::RemoveAbilityBonus;
    m_pApplyItemProperty[1] = &CSWSItemPropertyHandler::ApplyACBonus;
    m_pRemoveItemProperty[1] = &CSWSItemPropertyHandler::RemoveACBonus;
    m_pApplyItemProperty[2] = &CSWSItemPropertyHandler::ApplyACBonus;
    m_pRemoveItemProperty[2] = &CSWSItemPropertyHandler::RemoveACBonus;
    m_pApplyItemProperty[3] = &CSWSItemPropertyHandler::ApplyACBonus;
    m_pRemoveItemProperty[3] = &CSWSItemPropertyHandler::RemoveACBonus;
    m_pApplyItemProperty[4] = &CSWSItemPropertyHandler::ApplyACBonus;
    m_pRemoveItemProperty[4] = &CSWSItemPropertyHandler::RemoveACBonus;
    m_pApplyItemProperty[5] = &CSWSItemPropertyHandler::ApplyEnhancementBonus;
    m_pRemoveItemProperty[5] = &CSWSItemPropertyHandler::RemoveEnhancementBonus;
    m_pApplyItemProperty[6] = &CSWSItemPropertyHandler::ApplyEnhancementBonus;
    m_pRemoveItemProperty[6] = &CSWSItemPropertyHandler::RemoveEnhancementBonus;
    m_pApplyItemProperty[7] = &CSWSItemPropertyHandler::ApplyEnhancementBonus;
    m_pRemoveItemProperty[7] = &CSWSItemPropertyHandler::RemoveEnhancementBonus;
    m_pApplyItemProperty[8] = &CSWSItemPropertyHandler::ApplyEnhancementPenalty;
    m_pRemoveItemProperty[8] =
        &CSWSItemPropertyHandler::RemoveEnhancementPenalty;
    m_pApplyItemProperty[9] = &CSWSItemPropertyHandler::ApplyBonusFeat;
    m_pRemoveItemProperty[9] = &CSWSItemPropertyHandler::RemoveBonusFeat;
    m_pApplyItemProperty[11] = &CSWSItemPropertyHandler::ApplyDamageBonus;
    m_pRemoveItemProperty[11] = &CSWSItemPropertyHandler::RemoveDamageBonus;
    m_pApplyItemProperty[12] = &CSWSItemPropertyHandler::ApplyDamageBonus;
    m_pRemoveItemProperty[12] = &CSWSItemPropertyHandler::RemoveDamageBonus;
    m_pApplyItemProperty[13] = &CSWSItemPropertyHandler::ApplyDamageBonus;
    m_pRemoveItemProperty[13] = &CSWSItemPropertyHandler::RemoveDamageBonus;
    m_pApplyItemProperty[14] = &CSWSItemPropertyHandler::ApplyDamageImmunity;
    m_pRemoveItemProperty[14] = &CSWSItemPropertyHandler::RemoveDamageImmunity;
    m_pApplyItemProperty[15] = &CSWSItemPropertyHandler::ApplyDamagePenalty;
    m_pRemoveItemProperty[15] = &CSWSItemPropertyHandler::RemoveDamagePenalty;
    m_pApplyItemProperty[16] = &CSWSItemPropertyHandler::ApplyDamageReduction;
    m_pRemoveItemProperty[16] = &CSWSItemPropertyHandler::RemoveDamageReduction;
    m_pApplyItemProperty[17] = &CSWSItemPropertyHandler::ApplyDamageResistance;
    m_pRemoveItemProperty[17] =
        &CSWSItemPropertyHandler::RemoveDamageResistance;
    m_pApplyItemProperty[18] =
        &CSWSItemPropertyHandler::ApplyDamageVulnerability;
    m_pRemoveItemProperty[18] =
        &CSWSItemPropertyHandler::RemoveDamageVulnerability;
    m_pApplyItemProperty[19] =
        &CSWSItemPropertyHandler::ApplyDecreaseAbilityScore;
    m_pRemoveItemProperty[19] =
        &CSWSItemPropertyHandler::RemoveDecreaseAbilityScore;
    m_pApplyItemProperty[20] = &CSWSItemPropertyHandler::ApplyDecreaseAC;
    m_pRemoveItemProperty[20] = &CSWSItemPropertyHandler::RemoveDecreaseAC;
    m_pApplyItemProperty[21] =
        &CSWSItemPropertyHandler::ApplyDecreaseSkillModifier;
    m_pRemoveItemProperty[21] =
        &CSWSItemPropertyHandler::RemoveDecreaseSkillModifier;
    m_pApplyItemProperty[24] = &CSWSItemPropertyHandler::ApplyImmunity;
    m_pRemoveItemProperty[24] = &CSWSItemPropertyHandler::RemoveImmunity;
    m_pApplyItemProperty[25] =
        &CSWSItemPropertyHandler::ApplyImprovedForceResistance;
    m_pRemoveItemProperty[25] =
        &CSWSItemPropertyHandler::RemoveImprovedForceResistance;
    m_pApplyItemProperty[26] =
        &CSWSItemPropertyHandler::ApplyImprovedSavingThrow;
    m_pRemoveItemProperty[26] =
        &CSWSItemPropertyHandler::RemoveImprovedSavingThrow;
    m_pApplyItemProperty[27] =
        &CSWSItemPropertyHandler::ApplyImprovedSavingThrow;
    m_pRemoveItemProperty[27] =
        &CSWSItemPropertyHandler::RemoveImprovedSavingThrow;
    m_pApplyItemProperty[29] = &CSWSItemPropertyHandler::ApplyLight;
    m_pRemoveItemProperty[29] = &CSWSItemPropertyHandler::RemoveLight;
    m_pApplyItemProperty[33] =
        &CSWSItemPropertyHandler::ApplyReducedSavingThrow;
    m_pRemoveItemProperty[33] =
        &CSWSItemPropertyHandler::RemoveReducedSavingThrow;
    m_pApplyItemProperty[34] =
        &CSWSItemPropertyHandler::ApplyReducedSavingThrow;
    m_pRemoveItemProperty[34] =
        &CSWSItemPropertyHandler::RemoveReducedSavingThrow;
    m_pApplyItemProperty[35] = &CSWSItemPropertyHandler::ApplyRegeneration;
    m_pRemoveItemProperty[35] = &CSWSItemPropertyHandler::RemoveRegeneration;
    m_pApplyItemProperty[54] = &CSWSItemPropertyHandler::ApplyRegeneration;
    m_pRemoveItemProperty[54] = &CSWSItemPropertyHandler::RemoveRegeneration;
    m_pApplyItemProperty[36] = &CSWSItemPropertyHandler::ApplySkillBonus;
    m_pRemoveItemProperty[36] = &CSWSItemPropertyHandler::RemoveSkillBonus;
    m_pApplyItemProperty[38] = &CSWSItemPropertyHandler::ApplyAttackBonus;
    m_pRemoveItemProperty[38] = &CSWSItemPropertyHandler::RemoveAttackBonus;
    m_pApplyItemProperty[39] = &CSWSItemPropertyHandler::ApplyAttackBonus;
    m_pRemoveItemProperty[39] = &CSWSItemPropertyHandler::RemoveAttackBonus;
    m_pApplyItemProperty[40] = &CSWSItemPropertyHandler::ApplyAttackBonus;
    m_pRemoveItemProperty[40] = &CSWSItemPropertyHandler::RemoveAttackBonus;
    m_pApplyItemProperty[8] = &CSWSItemPropertyHandler::ApplyAttackPenalty;
    m_pRemoveItemProperty[8] = &CSWSItemPropertyHandler::RemoveAttackPenalty;
    m_pApplyItemProperty[47] = &CSWSItemPropertyHandler::ApplyTrueSeeing;
    m_pRemoveItemProperty[47] = &CSWSItemPropertyHandler::RemoveTrueSeeing;
    m_pApplyItemProperty[50] = &CSWSItemPropertyHandler::ApplyFreedomOfMovement;
    m_pRemoveItemProperty[50] =
        &CSWSItemPropertyHandler::RemoveFreedomOfMovement;
    m_pApplyItemProperty[52] = &CSWSItemPropertyHandler::ApplySpecialWalk;
    m_pRemoveItemProperty[52] = &CSWSItemPropertyHandler::RemoveSpecialWalk;
    m_pApplyItemProperty[55] =
        &CSWSItemPropertyHandler::ApplyBlasterBoltDeflectionIncrease;
    m_pRemoveItemProperty[55] =
        &CSWSItemPropertyHandler::RemoveBlasterBoltDeflectionIncrease;
    m_pApplyItemProperty[56] =
        &CSWSItemPropertyHandler::ApplyBlasterBoltDeflectionDecrease;
    m_pRemoveItemProperty[56] =
        &CSWSItemPropertyHandler::RemoveBlasterBoltDeflectionDecrease;
    m_pApplyItemProperty[59] = &CSWSItemPropertyHandler::ApplyDisguise;
    m_pRemoveItemProperty[59] = &CSWSItemPropertyHandler::RemoveDisguise;
    m_pApplyItemProperty[63] = &CSWSItemPropertyHandler::ApplyDampenSound;
    m_pRemoveItemProperty[63] = &CSWSItemPropertyHandler::RemoveDampenSound;
    m_pApplyItemProperty[64] = &CSWSItemPropertyHandler::ApplyDoorCutting;
    m_pApplyItemProperty[65] = &CSWSItemPropertyHandler::ApplyDoorSabering;
    m_pApplyItemProperty[66] = &CSWSItemPropertyHandler::ApplySniperBonus;
    m_pApplyItemProperty[67] = &CSWSItemPropertyHandler::ApplyRapidShotBonus;
    m_pApplyItemProperty[68] = &CSWSItemPropertyHandler::ApplyMaxDexBonus;
    m_pRemoveItemProperty[68] = &CSWSItemPropertyHandler::RemoveMaxDexBonus;
}
