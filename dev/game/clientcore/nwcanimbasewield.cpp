#include "appmanager.h"
#include "nwcanimbasewield.h"
#include "exobase.h"
#include "gameobjectarray.h"
#include "nwitem.h"
#include "nwcitem.h"
#include "clientexoapp.h"

static inline void NwcanimbasewieldTrace1();
#define NWCANIMBASEWIELD_TRACE() NwcanimbasewieldTrace1()
// PC 0x00699DD0  Xbox 0x000C5980
CSWCAnimBaseWield::CSWCAnimBaseWield()
{
    m_nRightItemId = CGameObjectArray::INVALIDOBJID;
    m_nLeftItemId = CGameObjectArray::INVALIDOBJID;
    m_nAnimationType = ANIMATION_TYPE_WIELD;
    m_bWieldLimited = 0;
    m_resHiddenRight = "";
    m_resHiddenLeft = "";
    m_pHiddenRightModel = 0;
    m_pHiddenLeftModel = 0;
    m_bItemsHidden = 0;
    m_bHeadItemHidden = 0;
    m_bTorchLit = 0;
    m_pTorchSound = 0;
}
// PC 0x00699ED0  Xbox 0x000C5A80
CSWCAnimBaseWield::~CSWCAnimBaseWield()
{
    if (m_pTorchSound != 0)
    {
        delete m_pTorchSound;
        m_pTorchSound = 0;
    }
    if (m_pHiddenRightModel != 0)
    {
        m_pHiddenRightModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        m_pHiddenRightModel->AttachToScene(0);
        delete m_pHiddenRightModel;
        m_pHiddenRightModel = 0;
    }
    if (m_pHiddenLeftModel != 0)
    {
        m_pHiddenLeftModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        m_pHiddenLeftModel->AttachToScene(0);
        delete m_pHiddenLeftModel;
        m_pHiddenLeftModel = 0;
    }
    UnloadModel();
}
// PC 0x00699FC0  Xbox 0x000C5B70
void CSWCAnimBaseWield::SetWieldLimited(int bLimited)
{
    m_bWieldLimited = bLimited;
}
// PC 0x00699FD0  Xbox 0x000C5B80
int CSWCAnimBaseWield::SetAnimationOnSubObjectInternal(
    CAurObject* pObject, unsigned char nSubObject, unsigned short nAnimation,
    float fSpeed, int nFlags, float fBlend)
{
    if (nSubObject != 0xFF)
    {
        return 0;
    }
    return SetAnimationInternal(pObject, nAnimation, fSpeed, nFlags, fBlend);
}
// PC 0x0069A3E0  Xbox 0x000C5C70
int CSWCAnimBaseWield::SetAnimationInternal(CAurObject* pObject,
                                            unsigned short nAnimation,
                                            float fSpeed, int nFlags,
                                            float fBlend)
{
    NWCANIMBASEWIELD_TRACE();
    if (fSpeed == 0.0f)
    {
        fSpeed = 1.0f;
    }

    CSWCItem* pRightItem =
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(m_nRightItemId);
    CSWCItem* pLeftItem =
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(m_nLeftItemId);
    if (pRightItem != 0 || pLeftItem != 0)
    {
        CExoString sAnimation = GetAnimationName(nAnimation);
        if (pRightItem != 0)
        {
            CSWBaseItem* pBaseItem = pRightItem->GetBaseItem();
            if (pBaseItem->m_nItemType != CSWBaseItem::ITEM_TYPE_TORCH)
            {
                Gob* pModel1 = pRightItem->GetModel(MODEL_SLOT_MAIN, 1);
                if (pModel1 != 0)
                {
                    pModel1->PlayAnimation(sAnimation.CStr(), fSpeed, nFlags, fBlend);
                }
                Gob* pModel2 = pRightItem->GetModel(MODEL_SLOT_HEAD, 1);
                if (pModel2 != 0)
                {
                    pModel2->PlayAnimation(sAnimation.CStr(), fSpeed, nFlags, fBlend);
                }
                Gob* pModel3 = pRightItem->GetModel(MODEL_TAIL, 1);
                if (pModel3 != 0)
                {
                    pModel3->PlayAnimation(sAnimation.CStr(), fSpeed, nFlags, fBlend);
                }
            }
        }
        if (pLeftItem != 0)
        {
            Gob* pModel1 = pLeftItem->GetModel(MODEL_SLOT_MAIN, 1);
            if (pModel1 != 0)
            {
                pModel1->PlayAnimation(sAnimation.CStr(), fSpeed, nFlags, fBlend);
            }
            Gob* pModel2 = pLeftItem->GetModel(MODEL_SLOT_HEAD, 1);
            if (pModel2 != 0)
            {
                pModel2->PlayAnimation(sAnimation.CStr(), fSpeed, nFlags, fBlend);
            }
            Gob* pModel3 = pLeftItem->GetModel(MODEL_TAIL, 1);
            if (pModel3 != 0)
            {
                pModel3->PlayAnimation(sAnimation.CStr(), fSpeed, nFlags, fBlend);
            }
        }
    }
    return CSWCAnimBaseTW::SetAnimationInternal(pObject, nAnimation, fSpeed,
                                                nFlags, fBlend);
}
// PC 0x0069A010  Xbox 0x000C6190
void CSWCAnimBaseWield::ShowWieldedItems()
{
    if (m_bItemsHidden != 0)
    {
        if (m_pHiddenRightModel != 0)
        {
            m_pHiddenRightModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            m_pHiddenRightModel->AttachToScene(0);
            delete m_pHiddenRightModel;
            m_pHiddenRightModel = 0;
        }
        if (m_pHiddenLeftModel != 0)
        {
            m_pHiddenLeftModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            m_pHiddenLeftModel->AttachToScene(0);
            delete m_pHiddenLeftModel;
            m_pHiddenLeftModel = 0;
        }

        CSWCItem* pRightItem =
            g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                m_nRightItemId);
        if (pRightItem != 0)
        {
            Gob* pModel = pRightItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
            if (pModel != 0)
            {
                pModel->AttachToObject(m_pModel, (char*)m_sRightAttach.CStr(),
                                       0);
            }
            if (m_pTorchSound != 0)
            {
                m_pTorchSound->Play();
            }
        }

        CSWCItem* pLeftItem =
            g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                m_nLeftItemId);
        if (pLeftItem != 0)
        {
            Gob* pModel = pLeftItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
            if (pModel != 0)
            {
                pModel->AttachToObject(m_pModel, (char*)m_sLeftAttach.CStr(),
                                       0);
            }
        }

        RefreshHilite();
        m_resHiddenRight = "";
        m_resHiddenLeft = "";
        m_bItemsHidden = 0;
    }
}
// PC 0x0069A170  Xbox 0x000C5BC0
void CSWCAnimBaseWield::TurnOnShadows()
{
    CSWCAnimBase::TurnOnShadows();
}
// PC 0x0069A180  Xbox 0x000C5BD0
void CSWCAnimBaseWield::TurnOffShadows()
{
    CSWCAnimBase::TurnOffShadows();
}
// PC 0x0069A190  Xbox 0x000C62F0
void CSWCAnimBaseWield::UpdateSound(int bPlaying)
{
    if (bPlaying != 0 && m_bTorchLit != 0)
    {
        if (m_pTorchSound == 0)
        {
            m_pTorchSound = new CExoSoundSource("al_cv_firetorch1");
            if (m_pTorchSound == 0)
            {
                return;
            }
            m_pTorchSound->Set3D(1);
            m_pTorchSound->SetPriorityGroup(6);
            m_pTorchSound->SetVolume(10);
            m_pTorchSound->SetLooping(1);
            m_pTorchSound->Play();
        }
        if (m_pTorchSound != 0)
        {
            CSWCItem* pItem =
                g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                    m_nRightItemId);
            if (pItem != 0 && pItem->GetModel(255, 1) != 0)
            {
                m_pTorchSound->SetPosition(
                    pItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPosition(), 0.0f);
            }
            else if (m_pModel != 0)
            {
                m_pTorchSound->SetPosition(m_pModel->GetPosition(), 0.0f);
            }
        }
    }
    else
    {
        if (m_pTorchSound != 0)
        {
            delete m_pTorchSound;
            m_pTorchSound = 0;
        }
    }
}
// PC 0x0069AA90  Xbox 0x000C6620
Gob* CSWCAnimBaseWield::GetModel(unsigned char nSlot)
{
    switch (nSlot)
    {
    case 251:
        return GetItemAurObject(m_nRightItemId, 0xFF);
    case 250:
        return GetItemAurObject(m_nRightItemId, 0xFE);
    case 249:
        return GetItemAurObject(m_nRightItemId, 0xFD);
    case 248:
        return GetItemAurObject(m_nRightItemId, 0xFC);
    case 247:
        return GetItemAurObject(m_nLeftItemId, 0xFF);
    case 246:
        return GetItemAurObject(m_nLeftItemId, 0xFE);
    case 245:
        return GetItemAurObject(m_nLeftItemId, 0xFD);
    case 244:
        return GetItemAurObject(m_nLeftItemId, 0xFC);
    }
    return CSWCAnimBaseTW::GetModel(nSlot);
}
// PC 0x0069AB80  Xbox 0x000C6710
int CSWCAnimBaseWield::Wield(unsigned long nItemId, const CExoString& sAttach,
                             unsigned char nHand, float fScale)
{
    CSWCItem* pItem =
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(nItemId);
    Gob* pNewModel;
    if (pItem == 0)
    {
        pNewModel = 0;
    }
    else
    {
        pNewModel = pItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
    }

    if (nHand == 0)
    {
        CSWCItem* pOldItem =
            g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
                m_nRightItemId);
        Gob* pOldModel;
        if (pOldItem == 0)
        {
            pOldModel = 0;
        }
        else
        {
            pOldModel = pOldItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        }
        if (nItemId != m_nRightItemId && pOldModel != 0)
        {
            CSWCItem* pRightItem =
                g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                    m_nRightItemId);
            if (pRightItem != 0 &&
                pRightItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_TORCH)
            {
                pOldModel->PlayAnimation("unequip", 1.0f, 3, 0.0f);
                m_bTorchLit = 0;
            }
            CSWCItem* pDropped =
                g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                    m_nRightItemId);
            if (pDropped != 0 && pDropped->GetEquippedBy() == 0)
            {
                pOldModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
                pOldModel->AttachToScene(0);
                pOldModel->SetObjectScale(1.0f, 1);
            }
        }
        if (pNewModel != 0)
        {
            if (m_bWieldLimited == 0 ||
                g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                    nItemId) != 0)
            {
                pNewModel->AttachToObject(m_pModel, (char*)sAttach.CStr(), Gob::ATTACH_NORMAL);
                pNewModel->SetObjectScale(fScale, 1);
                m_sRightAttach = sAttach;
                CSWCItem* pNewItem =
                    g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                        nItemId);
                if (pNewItem != 0 &&
                    pNewItem->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_TORCH)
                {
                    pNewModel->PlayAnimation("equip", 1.0f, 3, 0.0f);
                    m_bTorchLit = 1;
                }
            }
        }
        else
        {
            if (m_nRightItemId != m_nLeftItemId && pOldModel != 0)
            {
                CSWCItem* pDropped =
                    g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                        m_nRightItemId);
                if (pDropped != 0 && pDropped->GetEquippedBy() == 0)
                {
                    pOldModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
                    pOldModel->AttachToScene(0);
                    pOldModel->SetObjectScale(1.0f, 1);
                }
            }
            m_sRightAttach = "";
        }
        m_nRightItemId = nItemId;
    }
    else if (nHand == 1)
    {
        CSWCItem* pOldItem =
            g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(
                m_nLeftItemId);
        Gob* pOldModel;
        if (pOldItem == 0)
        {
            pOldModel = 0;
        }
        else
        {
            pOldModel = pOldItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        }
        if (nItemId != m_nLeftItemId && pOldModel != 0)
        {
            CSWCItem* pDropped =
                g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                    m_nLeftItemId);
            if (pDropped != 0 && pDropped->GetEquippedBy() == 0)
            {
                pOldModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
                pOldModel->AttachToScene(0);
                pOldModel->SetObjectScale(1.0f, 1);
            }
        }
        if (pNewModel != 0)
        {
            if (ItemIsValid(nItemId))
            {
                pNewModel->AttachToObject(m_pModel, (char*)sAttach.CStr(), Gob::ATTACH_NORMAL);
                pNewModel->SetObjectScale(fScale, 1);
                m_sLeftAttach = sAttach;
            }
        }
        else
        {
            if (m_nLeftItemId != m_nRightItemId && pOldModel != 0)
            {
                CSWCItem* pDropped =
                    g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                        m_nLeftItemId);
                if (pDropped != 0 && pDropped->GetEquippedBy() == 0)
                {
                    pOldModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
                    pOldModel->AttachToScene(0);
                    pOldModel->SetObjectScale(1.0f, 1);
                }
            }
            m_sLeftAttach = "";
        }
        m_nLeftItemId = nItemId;
    }
    else
    {
        return 0;
    }
    RefreshHilite();
    return 1;
}
// PC 0x0069AFE0  Xbox 0x000C6B70
int CSWCAnimBaseWield::LoadModel(const CResRef& sResRef, unsigned char nType)
{
    if (CSWCAnimBaseTW::LoadModel(sResRef, nType) != 0)
    {
        ReAttachWieldedObjects();
        return 1;
    }
    return 0;
}
// PC 0x0069AF10  Xbox 0x000C6AA0
void CSWCAnimBaseWield::ReAttachWieldedObjects()
{
    Gob* pRightModel = GetItemAurObject(m_nRightItemId, CSWCObject::MODEL_SLOT_DEFAULT);
    if (pRightModel != 0)
    {
        pRightModel->AttachToObject(m_pModel, (char*)m_sRightAttach.CStr(), Gob::ATTACH_NORMAL);
    }
    Gob* pLeftModel = GetItemAurObject(m_nLeftItemId, CSWCObject::MODEL_SLOT_DEFAULT);
    if (pLeftModel != 0)
    {
        pLeftModel->AttachToObject(m_pModel, (char*)m_sLeftAttach.CStr(), Gob::ATTACH_NORMAL);
    }
    RefreshHilite();
}
// PC 0x0069A650  Xbox 0x000C5EF0
Gob* CSWCAnimBaseWield::GetItemAurObject(unsigned long nItemId,
                                                unsigned char nSlot)
{
    NWCANIMBASEWIELD_TRACE();
    CSWCItem* pItem =
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(nItemId);
    if (pItem == 0)
    {
        return 0;
    }
    return pItem->GetModel(nSlot, 1);
}
// PC 0x0069A680  Xbox 0x000C5F20
int CSWCAnimBaseWield::ItemIsValid(unsigned long nItemId)
{
    NWCANIMBASEWIELD_TRACE();
    if (m_bWieldLimited == 0)
    {
        return 1;
    }
    return g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(nItemId) !=
           0;
}
// PC 0x0069A6B0  Xbox 0x000C5F50
void CSWCAnimBaseWield::HideWieldedItems(const CResRef& sRight,
                                         const CResRef& sLeft)
{
    NWCANIMBASEWIELD_TRACE();
    CExoString sModel;

    CSWCItem* pRightItem =
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(m_nRightItemId);
    if (pRightItem != 0)
    {
        Gob* pModel = pRightItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        if (pModel != 0)
        {
            pModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            pModel->AttachToScene(0);
            if (m_pTorchSound != 0)
            {
                m_pTorchSound->Stop();
            }
        }
    }

    CSWCItem* pLeftItem =
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(m_nLeftItemId);
    if (pLeftItem != 0)
    {
        Gob* pModel = pLeftItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        if (pModel != 0)
        {
            pModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            pModel->AttachToScene(0);
        }
    }

    if (m_resHiddenRight != sRight)
    {
        if (m_pHiddenRightModel != 0)
        {
            m_pHiddenRightModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            m_pHiddenRightModel->AttachToScene(0);
            delete m_pHiddenRightModel;
            m_pHiddenRightModel = 0;
        }
        if (*const_cast<CResRef&>(sRight).GetResRef() != 0)
        {
            sRight.CopyToString(sModel);
            m_pHiddenRightModel = static_cast<Gob*>(
                NewCAurObject((char*)sModel.CStr(), "dummyright", 0, 0));
            if (m_pHiddenRightModel != 0)
            {
                m_pHiddenRightModel->AttachToObject(m_pModel, "rhand", Gob::ATTACH_NORMAL);
            }
        }
        m_resHiddenRight = sRight;
    }

    if (m_resHiddenLeft != sLeft)
    {
        if (m_pHiddenLeftModel != 0)
        {
            m_pHiddenLeftModel->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            m_pHiddenLeftModel->AttachToScene(0);
            delete m_pHiddenLeftModel;
            m_pHiddenLeftModel = 0;
        }
        if (*const_cast<CResRef&>(sLeft).GetResRef() != 0)
        {
            sLeft.CopyToString(sModel);
            m_pHiddenLeftModel = static_cast<Gob*>(
                NewCAurObject((char*)sModel.CStr(), "dummyleft", 0, 0));
            if (m_pHiddenLeftModel != 0)
            {
                m_pHiddenLeftModel->AttachToObject(m_pModel, "lhand", Gob::ATTACH_NORMAL);
            }
        }
        m_resHiddenLeft = sLeft;
    }

    m_bItemsHidden = 1;
}
// PC 0x0069A8F0  Xbox 0x000C6460
void CSWCAnimBaseWield::EnableDistortion()
{
    Gob* pLeftModel = GetItemAurObject(m_nLeftItemId, CSWCObject::MODEL_SLOT_DEFAULT);
    if (pLeftModel != 0)
    {
        pLeftModel->EnableDistortion();
    }
    Gob* pRightModel = GetItemAurObject(m_nRightItemId, CSWCObject::MODEL_SLOT_DEFAULT);
    if (pRightModel != 0)
    {
        pRightModel->EnableDistortion();
    }
    CSWCAnimBase::EnableDistortion();
}
// PC 0x0069A980  Xbox 0x000C64F0
void CSWCAnimBaseWield::DisableDistortion()
{
    Gob* pLeftModel = GetItemAurObject(m_nLeftItemId, CSWCObject::MODEL_SLOT_DEFAULT);
    if (pLeftModel != 0)
    {
        pLeftModel->DisableDistortion();
    }
    Gob* pRightModel = GetItemAurObject(m_nRightItemId, CSWCObject::MODEL_SLOT_DEFAULT);
    if (pRightModel != 0)
    {
        pRightModel->DisableDistortion();
    }
    CSWCAnimBase::DisableDistortion();
}

static inline void NwcanimbasewieldTrace1()
{
}
