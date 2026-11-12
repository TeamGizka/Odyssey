#ifndef __NWSITEMPROPERTY_H__
#define __NWSITEMPROPERTY_H__

class CSWSCreature;
class CSWSItem;
class CSWItemProperty;



class CItemPropertyApplierRemover
{
public:
    CItemPropertyApplierRemover()
    {
    }

    virtual ~CItemPropertyApplierRemover()
    {
    }

    virtual void InitializeItemProperties() = 0;
    virtual int OnItemPropertyApplied(CSWSItem* pItem,
                                      CSWItemProperty* pProperty,
                                      CSWSCreature* pCreature,
                                      unsigned long nIndex, int bLoading) = 0;
    virtual int OnItemPropertyRemoved(CSWSItem* pItem,
                                      CSWItemProperty* pProperty,
                                      CSWSCreature* pCreature,
                                      unsigned long nIndex) = 0;
};

class CSWSItemPropertyHandler : public CItemPropertyApplierRemover
{
public:
    typedef int (CSWSItemPropertyHandler::*ITEM_PROPERTY_APPLY)(
        CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
        unsigned long nIndex, int bLoading);
    typedef int (CSWSItemPropertyHandler::*ITEM_PROPERTY_REMOVE)(
        CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature,
        unsigned long nIndex);

    virtual ~CSWSItemPropertyHandler();

    virtual void InitializeItemProperties();
    virtual int OnItemPropertyApplied(CSWSItem* pItem,
                                      CSWItemProperty* pProperty,
                                      CSWSCreature* pCreature,
                                      unsigned long nIndex, int bLoading);
    virtual int OnItemPropertyRemoved(CSWSItem* pItem,
                                      CSWItemProperty* pProperty,
                                      CSWSCreature* pCreature,
                                      unsigned long nIndex);

    int ApplyEnhancementBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                              CSWSCreature* pCreature, unsigned long nIndex,
                              int bLoading);
    int ApplyEnhancementPenalty(CSWSItem* pItem, CSWItemProperty* pProperty,
                                CSWSCreature* pCreature, unsigned long nIndex,
                                int bLoading);
    int ApplyDamageBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                         CSWSCreature* pCreature, unsigned long nIndex,
                         int bLoading);
    int ApplyAttackBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                         CSWSCreature* pCreature, unsigned long nIndex,
                         int bLoading);
    int ApplyAttackPenalty(CSWSItem* pItem, CSWItemProperty* pProperty,
                           CSWSCreature* pCreature, unsigned long nIndex,
                           int bLoading);
    int ApplyLight(CSWSItem* pItem, CSWItemProperty* pProperty,
                   CSWSCreature* pCreature, unsigned long nIndex, int bLoading);
    int ApplyAbilityBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                          CSWSCreature* pCreature, unsigned long nIndex,
                          int bLoading);
    int ApplyACBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                     CSWSCreature* pCreature, unsigned long nIndex,
                     int bLoading);
    int ApplyImprovedForceResistance(CSWSItem* pItem,
                                     CSWItemProperty* pProperty,
                                     CSWSCreature* pCreature,
                                     unsigned long nIndex, int bLoading);
    int ApplyChangedSavingThrow(CSWSItem* pItem, int nSaveValue,
                                CSWItemProperty* pProperty,
                                CSWSCreature* pCreature, int bLoading);
    int ApplySpecialWalk(CSWSItem* pItem, CSWItemProperty* pProperty,
                         CSWSCreature* pCreature, unsigned long nIndex,
                         int bLoading);
    int ApplyTrueSeeing(CSWSItem* pItem, CSWItemProperty* pProperty,
                        CSWSCreature* pCreature, unsigned long nIndex,
                        int bLoading);
    int ApplyFreedomOfMovement(CSWSItem* pItem, CSWItemProperty* pProperty,
                               CSWSCreature* pCreature, unsigned long nIndex,
                               int bLoading);
    int ApplyBonusFeat(CSWSItem* pItem, CSWItemProperty* pProperty,
                       CSWSCreature* pCreature, unsigned long nIndex,
                       int bLoading);
    int ApplyDamageImmunity(CSWSItem* pItem, CSWItemProperty* pProperty,
                            CSWSCreature* pCreature, unsigned long nIndex,
                            int bLoading);
    int ApplyDamagePenalty(CSWSItem* pItem, CSWItemProperty* pProperty,
                           CSWSCreature* pCreature, unsigned long nIndex,
                           int bLoading);
    int ApplyDamageReduction(CSWSItem* pItem, CSWItemProperty* pProperty,
                             CSWSCreature* pCreature, unsigned long nIndex,
                             int bLoading);
    int ApplyDamageResistance(CSWSItem* pItem, CSWItemProperty* pProperty,
                              CSWSCreature* pCreature, unsigned long nIndex,
                              int bLoading);
    int ApplyDamageVulnerability(CSWSItem* pItem, CSWItemProperty* pProperty,
                                 CSWSCreature* pCreature, unsigned long nIndex,
                                 int bLoading);
    int ApplyDecreaseAbilityScore(CSWSItem* pItem, CSWItemProperty* pProperty,
                                  CSWSCreature* pCreature, unsigned long nIndex,
                                  int bLoading);
    int ApplyDecreaseAC(CSWSItem* pItem, CSWItemProperty* pProperty,
                        CSWSCreature* pCreature, unsigned long nIndex,
                        int bLoading);
    int ApplyDecreaseSkillModifier(CSWSItem* pItem, CSWItemProperty* pProperty,
                                   CSWSCreature* pCreature,
                                   unsigned long nIndex, int bLoading);
    int ApplyImmunity(CSWSItem* pItem, CSWItemProperty* pProperty,
                      CSWSCreature* pCreature, unsigned long nIndex,
                      int bLoading);
    int ApplySkillBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                        CSWSCreature* pCreature, unsigned long nIndex,
                        int bLoading);
    int ApplyRegeneration(CSWSItem* pItem, CSWItemProperty* pProperty,
                          CSWSCreature* pCreature, unsigned long nIndex,
                          int bLoading);
    int ApplyBlasterBoltDeflectionIncrease(CSWSItem* pItem,
                                           CSWItemProperty* pProperty,
                                           CSWSCreature* pCreature,
                                           unsigned long nIndex, int bLoading);
    int ApplyBlasterBoltDeflectionDecrease(CSWSItem* pItem,
                                           CSWItemProperty* pProperty,
                                           CSWSCreature* pCreature,
                                           unsigned long nIndex, int bLoading);
    int ApplyDisguise(CSWSItem* pItem, CSWItemProperty* pProperty,
                      CSWSCreature* pCreature, unsigned long nIndex,
                      int bLoading);
    int ApplyDampenSound(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading);
    int RemoveDampenSound(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex);
    int ApplyDoorCutting(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading);
    int ApplyDoorSabering(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading);
    int ApplySniperBonus(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading);
    int ApplyRapidShotBonus(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading);
    int ApplyMaxDexBonus(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex, int bLoading);
    int RemoveMaxDexBonus(CSWSItem* pItem, CSWItemProperty* pProperty, CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveEnhancementBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                               CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveEnhancementPenalty(CSWSItem* pItem, CSWItemProperty* pProperty,
                                 CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveDamageBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                          CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveAttackBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                          CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveAttackPenalty(CSWSItem* pItem, CSWItemProperty* pProperty,
                            CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveAbilityBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                           CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveACBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                      CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveImprovedForceResistance(CSWSItem* pItem,
                                      CSWItemProperty* pProperty,
                                      CSWSCreature* pCreature,
                                      unsigned long nIndex);
    int ApplyImprovedSavingThrow(CSWSItem* pItem, CSWItemProperty* pProperty,
                                 CSWSCreature* pCreature, unsigned long nIndex,
                                 int bLoading);
    int ApplyReducedSavingThrow(CSWSItem* pItem, CSWItemProperty* pProperty,
                                CSWSCreature* pCreature, unsigned long nIndex,
                                int bLoading);
    int RemoveChangedSavingThrow(int nSaveValue, CSWSItem* pItem,
                                 CSWItemProperty* pProperty,
                                 CSWSCreature* pCreature);
    int RemoveSpecialWalk(CSWSItem* pItem, CSWItemProperty* pProperty,
                          CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveTrueSeeing(CSWSItem* pItem, CSWItemProperty* pProperty,
                         CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveFreedomOfMovement(CSWSItem* pItem, CSWItemProperty* pProperty,
                                CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveBonusFeat(CSWSItem* pItem, CSWItemProperty* pProperty,
                        CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveDamageImmunity(CSWSItem* pItem, CSWItemProperty* pProperty,
                             CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveDamagePenalty(CSWSItem* pItem, CSWItemProperty* pProperty,
                            CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveDamageReduction(CSWSItem* pItem, CSWItemProperty* pProperty,
                              CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveDamageResistance(CSWSItem* pItem, CSWItemProperty* pProperty,
                               CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveDamageVulnerability(CSWSItem* pItem, CSWItemProperty* pProperty,
                                  CSWSCreature* pCreature,
                                  unsigned long nIndex);
    int RemoveDecreaseAbilityScore(CSWSItem* pItem, CSWItemProperty* pProperty,
                                   CSWSCreature* pCreature,
                                   unsigned long nIndex);
    int RemoveDecreaseAC(CSWSItem* pItem, CSWItemProperty* pProperty,
                         CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveDecreaseSkillModifier(CSWSItem* pItem, CSWItemProperty* pProperty,
                                    CSWSCreature* pCreature,
                                    unsigned long nIndex);
    int RemoveImmunity(CSWSItem* pItem, CSWItemProperty* pProperty,
                       CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveSkillBonus(CSWSItem* pItem, CSWItemProperty* pProperty,
                         CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveRegeneration(CSWSItem* pItem, CSWItemProperty* pProperty,
                           CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveBlasterBoltDeflectionIncrease(CSWSItem* pItem,
                                            CSWItemProperty* pProperty,
                                            CSWSCreature* pCreature,
                                            unsigned long nIndex);
    int RemoveBlasterBoltDeflectionDecrease(CSWSItem* pItem,
                                            CSWItemProperty* pProperty,
                                            CSWSCreature* pCreature,
                                            unsigned long nIndex);
    int RemoveDisguise(CSWSItem* pItem, CSWItemProperty* pProperty,
                       CSWSCreature* pCreature, unsigned long nIndex);
    int RemoveImprovedSavingThrow(CSWSItem* pItem, CSWItemProperty* pProperty,
                                  CSWSCreature* pCreature,
                                  unsigned long nIndex);
    int RemoveReducedSavingThrow(CSWSItem* pItem, CSWItemProperty* pProperty,
                                 CSWSCreature* pCreature, unsigned long nIndex);

    int RemoveLight(CSWSItem* pItem, CSWItemProperty* pProperty,
                    CSWSCreature* pCreature, unsigned long nIndex);

    ITEM_PROPERTY_APPLY* m_pApplyItemProperty;
    ITEM_PROPERTY_REMOVE* m_pRemoveItemProperty;
};

class CSWSItem;
class CSWSCreature;
class CSWItemProperty;

#endif
