#ifndef __NWCITEM_H__
#define __NWCITEM_H__

class CExoSoundSource;
class CSWCAnimBase;
class CSWSItem;
class Vector;
#include "nwitem.h"
#include "nwcobject.h"
#include "exoresman.h"

class CSWCCreature;
class CSWItemProperty;
class Gob;

class CSWCItem : public CSWItem, public CSWCObject
{
public:

    enum { INVALID_ACTIVE_PROPERTY_INDEX = -1 };

    static const unsigned char MODEL_SLOT_VISUAL_EFFECT = 252;

    CSWCItem(unsigned long nBaseItem);
    virtual ~CSWCItem();
    // PC 0x00646380  Xbox 0x0009F3C0
    virtual CSWCItem* AsSWCItem()
    {
        return this;
    }

    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    virtual void SetPosition(const Vector& vPosition);
    virtual void SetOrientation(const Vector& vOrientation);
    virtual void SetArea(CSWCArea* pArea);

    void ComputeZValue(Vector* pPosition, float fHeight, Vector* pNormal);
    void UpdateOrientation(const Vector& vOrientation);

    int SetEquippedBy(CSWCCreature* pCreature);
    CSWCCreature* GetEquippedBy();
    int GetArmorClass() { return m_nArmorClass; }
    int GetNumActiveProperties() { return m_nNumActiveProperties; }
    void SetNumActiveProperties(int nNumActiveProperties);
    CSWItemProperty* GetActiveProperty(int nIndex);
    int GetActivePropertySpellID(int nIndex);
    void SetStackSize(int nStackSize);
    int LoadVisualEffect(unsigned char nEffect);
    void PlayDropSound();
    void ShowVisualEffect();
    void HideVisualEffect();
    void SpawnVisualEffect(const CExoString& sModel, const CExoString& sHook);
    void SetItemPropertyUsesLeft(int nIndex, unsigned short nUsesLeft);
    CSWSItem* GetServerItem();
    CExoString GetName(int bFullName);
    void PowerItem(int bPower, int bOverride, int bAnimate, int bSilent);
    void ResolveCreaturePoweredAnimations(int bPower);

    enum ArmorType
    {
        ARMOR_TYPE_LEATHER = 0,
        ARMOR_TYPE_CHAIN = 1,
        ARMOR_TYPE_PLATE = 2,
        ARMOR_TYPE_CLOTH = 3
    };

    int m_nArmorClass;
    unsigned char m_nArmorType;
    int m_bIdentified;
    int m_bStolen;
    unsigned char m_nModelVariation;
    unsigned char m_nBodyVariation;
    unsigned char m_nTextureVar;
    int m_nNumActiveProperties;
    CSWItemProperty* m_pActiveProperties;
    CResRef m_cModelResRef;
    CResRef m_cIconResRef;
    int m_bAttached;
    int m_nAttachSlot;
    int m_nUnknown150;
    int m_nEquipSlot;
    int m_nInventorySlot;
    unsigned char m_nContainerSlot;
    unsigned char m_nContainerType;

    unsigned char m_nUnknownContainer1;
    unsigned char m_nUnknownContainer2;
    unsigned long m_oidEquippedBy;

    unsigned long m_oidVisibleOwner;
    int m_bInContainer;
    unsigned short m_anItemPropertyUsesLeft[8];
    unsigned char m_nActivePropertyIndex;
    int m_nCost;
    int m_nStackSize;
    int m_bCanUse;

    CExoString m_sName;
    Gob* m_pVisualEffect;
    int m_nUnknown198;
    int m_bPowered;
    int m_nUnknown1A0;
    CExoSoundSource* m_pPowerSound;
};

#endif
