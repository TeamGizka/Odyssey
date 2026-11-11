#ifndef __NWCANIMBASEWIELD_H__
#define __NWCANIMBASEWIELD_H__

class CAurObject;
class CResRef;
#include "exobase.h"
#include "exoresman.h"
#include "nwcanimbase.h"

class CExoSoundSource;

class CSWCAnimBaseWield : public virtual CSWCAnimBaseTW
{
public:
    CSWCAnimBaseWield();
    virtual ~CSWCAnimBaseWield();

    virtual Gob* GetModel(unsigned char nSlot);
    virtual int LoadModel(const CResRef& sResRef, unsigned char nType);
    virtual int Wield(unsigned long nItemId, const CExoString& sAttach, unsigned char nHand, float fScale);
    virtual void HideWieldedItems(const CResRef& sRight, const CResRef& sLeft);
    virtual void ShowWieldedItems();
    virtual void HideHeadItem()  // PC 0x004015A0
    {
    }
    virtual void ShowHeadItem()  // PC 0x004015A0  Xbox 0x000C65B0
    {
    }
    virtual void SetWieldLimited(int bLimited);
    virtual void TurnOnShadows();
    virtual void TurnOffShadows();
    virtual void UpdateSound(int bPlaying);
    virtual void EnableDistortion();
    virtual void DisableDistortion();
    virtual int SetAnimationInternal(CAurObject* pObject, unsigned short nAnimation, float fSpeed, int nFlags, float fBlend);
    virtual int SetAnimationOnSubObjectInternal(CAurObject* pObject, unsigned char nSubObject, unsigned short nAnimation, float fSpeed, int nFlags, float fBlend);

    Gob* GetItemAurObject(unsigned long nItemId, unsigned char nSlot);
    int ItemIsValid(unsigned long nItemId);
    void ReAttachWieldedObjects();

    CResRef m_resHiddenRight;
    CResRef m_resHiddenLeft;
    CExoString m_sRightAttach;
    CExoString m_sLeftAttach;
    Gob* m_pHiddenRightModel;
    Gob* m_pHiddenLeftModel;
    unsigned long m_nRightItemId;
    unsigned long m_nLeftItemId;
    int m_bWieldLimited;
    int m_bItemsHidden;
    int m_bHeadItemHidden;
    int m_bTorchLit;
    CExoSoundSource* m_pTorchSound;
};

#endif
