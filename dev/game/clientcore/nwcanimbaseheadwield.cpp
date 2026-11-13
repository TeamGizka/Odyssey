#include "nwcanimbaseheadwield.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwcitem.h"

static inline void NwcanimbaseheadwieldTrace1();
#define NWCANIMBASEHEADWIELD_TRACE() NwcanimbaseheadwieldTrace1()
// PC 0x006F36C0 GOG 0x006F32A0
CSWCAnimBaseHeadWield::CSWCAnimBaseHeadWield()
{
    m_nAnimationType = ANIMATION_TYPE_BTYPE;
}
// PC 0x006F3790 GOG 0x006F3370
Gob* CSWCAnimBaseHeadWield::GetModel(unsigned char nModel)
{
    Gob* pModel;
    switch (nModel)
    {
    case 0xF4:
    case 0xF5:
    case 0xF6:
    case 0xF7:
    case 0xF8:
    case 0xF9:
    case 0xFA:
    case 0xFB:
    case 0xFF:
        pModel = CSWCAnimBaseWield::GetModel(nModel);
        break;
    case 0xFE:
        pModel = CSWCAnimBaseHead::GetModel(nModel);
        break;
    default:
        pModel = NULL;
        break;
    }
    return pModel;
}
// PC 0x006F3800 GOG 0x006F33E0
int CSWCAnimBaseHeadWield::LoadModel(const CResRef& sModel,
                                     unsigned char nModel)
{
    if (nModel == 0xFE)
    {
        if (CSWCAnimBaseHead::LoadModel(sModel, 0xFE) != 0)
        {
            ReAttachWieldedObjects();
            return 1;
        }
    }
    else
    {
        if (CSWCAnimBaseWield::LoadModel(sModel, nModel) != 0)
        {
            ReAttachWieldedObjects();
            return 1;
        }
    }
    return 0;
}
// PC 0x006F3880 GOG 0x006F3460
int CSWCAnimBaseHeadWield::ReplaceTexture(unsigned char nModel,
                                          const CResRef& sOld,
                                          const CResRef& sNew, int nFlags)
{
    switch (nModel)
    {
    case 0xFE:
        return CSWCAnimBaseHead::ReplaceTexture(nModel, sOld, sNew, nFlags);
    case 0xFF:
        return CSWCAnimBaseTW::ReplaceTexture(nModel, sOld, sNew, nFlags);
    }
    return 0;
}
// PC 0x006F38C0 GOG 0x006F34A0
int CSWCAnimBaseHeadWield::ReplaceTexture(const CResRef& sOld,
                                          const CResRef& sNew, int nFlags)
{
    return CSWCAnimBaseTW::ReplaceTexture(sOld, sNew, nFlags) != 0 &&
           CSWCAnimBaseHead::ReplaceTexture(sOld, sNew, nFlags) != 0;
}
// PC 0x006F3920 GOG 0x006F3500
int CSWCAnimBaseHeadWield::ReplaceTexture(unsigned char nModel,
                                          const CResRef& sOld,
                                          const CResRef& sNew,
                                          unsigned long nColor,
                                          unsigned short* pnPalette,
                                          int nPaletteSize,
                                          unsigned long nFlags)
{
    switch (nModel)
    {
    case 0xFE:
        CSWCAnimBaseHead::ReplaceTexture(nModel, sOld, sNew, nColor,
                                         pnPalette, nPaletteSize, nFlags);
        break;
    case 0xFF:
        return CSWCAnimBaseTW::ReplaceTexture(nModel, sOld, sNew, nColor,
                                              pnPalette, nPaletteSize,
                                              nFlags);
    }
    return 0;
}
// PC 0x006F3980 GOG 0x006F3560
int CSWCAnimBaseHeadWield::ReplaceTexture(const CResRef& sOld,
                                          const CResRef& sNew,
                                          unsigned long nColor,
                                          unsigned short* pnPalette,
                                          int nPaletteSize,
                                          unsigned long nFlags)
{
    return CSWCAnimBaseTW::ReplaceTexture(sOld, sNew, nColor, pnPalette,
                                          nPaletteSize, nFlags) != 0 &&
           CSWCAnimBaseHead::ReplaceTexture(sOld, sNew, nColor, pnPalette,
                                            nPaletteSize, nFlags) != 0;
}
// PC 0x006F3A00 GOG 0x006F35E0
int CSWCAnimBaseHeadWield::RestoreTexture()
{
    return CSWCAnimBaseHead::RestoreTexture();
}
// PC 0x006F3A10 GOG 0x006F35F0
int CSWCAnimBaseHeadWield::RestoreTexture(unsigned char nModel)
{
    return CSWCAnimBaseHead::RestoreTexture(nModel);
}
// PC 0x006F3A20 GOG 0x006F3600
void CSWCAnimBaseHeadWield::UnloadModel()
{
    CSWCAnimBaseTW::UnloadModel();
    return CSWCAnimBaseHead::UnloadModel();
}
// PC 0x006F3A50 GOG 0x006F3630
void CSWCAnimBaseHeadWield::UnloadModel(unsigned char nModel)
{
    if (nModel == 0xFF)
    {
        CSWCAnimBaseTW::UnloadModel(nModel);
    }
    else
    {
        CSWCAnimBaseHead::UnloadModel(nModel);
    }
}
// PC 0x006F3A90 GOG 0x006F3670
int CSWCAnimBaseHeadWield::Wield(unsigned long nObject,
                                 const CExoString& sSlot,
                                 unsigned char nModel, float fScale)
{
    return CSWCAnimBaseWield::Wield(nObject, sSlot, nModel, fScale);
}
// PC 0x006F3AA0 GOG 0x006F3680
void CSWCAnimBaseHeadWield::HideWieldedItems(const CResRef& sLeft,
                                            const CResRef& sRight)
{
    return CSWCAnimBaseWield::HideWieldedItems(sLeft, sRight);
}
// PC 0x006F3AB0 GOG 0x006F3690
void CSWCAnimBaseHeadWield::ShowWieldedItems()
{
    return CSWCAnimBaseWield::ShowWieldedItems();
}
// PC 0x006F3BF0 GOG 0x006F36A0
void CSWCAnimBaseHeadWield::SetWieldLimited(int bLimited)
{
    CSWCAnimBaseWield::SetWieldLimited(bLimited);
}
// PC 0x006F3C00 GOG 0x006F36B0
int CSWCAnimBaseHeadWield::SetEnvMap(int nType, int nFlags,
                                     const CResRef& sMap)
{
    return CSWCAnimBaseTW::SetEnvMap(nType, nFlags, sMap) != 0 &&
           CSWCAnimBaseHead::SetEnvMap(nType, nFlags, sMap) != 0;
}
// PC 0x006F3C60 GOG 0x006F3710
int CSWCAnimBaseHeadWield::SetEnvMap(unsigned char nModel, int nType,
                                     int nFlags, const CResRef& sMap)
{
    if (nModel == 0xFE)
    {
        return CSWCAnimBaseHead::SetEnvMap(nModel, nType, nFlags, sMap);
    }
    else
    {
        return CSWCAnimBaseTW::SetEnvMap(nModel, nType, nFlags, sMap);
    }
}
// PC 0x006F3CA0 GOG 0x006F3750
int CSWCAnimBaseHeadWield::RestoreEnvMap()
{
    return CSWCAnimBaseTW::RestoreEnvMap() != 0 &&
           CSWCAnimBaseHead::RestoreEnvMap() != 0;
}
// PC 0x006F3CE0 GOG 0x006F3790
int CSWCAnimBaseHeadWield::SetAnimation(unsigned short nAnimation,
                                        float fSpeed, int nFlags, float fTime)
{
    return CSWCAnimBaseWield::SetAnimationInternal(m_pModel, nAnimation,
                                                   fSpeed, nFlags, fTime) !=
               0 &&
           CSWCAnimBaseTW::SetAnimationInternal(m_pHeadModel, nAnimation,
                                                fSpeed, nFlags, fTime) != 0;
}
// PC 0x006F3D50 GOG 0x006F3800
int CSWCAnimBaseHeadWield::SetOverlayAnimation(unsigned short nAnimation,
                                               int bLoop)
{
    if (m_pModel == NULL && m_pHeadModel == NULL)
    {
        return 0;
    }
    return CSWCAnimBase::SetOverlayAnimation(m_pModel, nAnimation, bLoop) !=
               0 &&
           CSWCAnimBase::SetOverlayAnimation(m_pHeadModel, nAnimation,
                                             bLoop) != 0;
}
// PC 0x006F3DD0 GOG 0x006F3880
int CSWCAnimBaseHeadWield::SetAnimationVisemes(float* pfWeights,
                                               float* pfTimes, int nCount,
                                               float fLength)
{
    if (pfWeights != NULL && pfTimes != NULL)
    {
        if (m_pModel != NULL)
        {
            m_pModel->PlayOutOfOrderAnimation("talk", pfWeights, pfTimes,
                                              nCount, fLength);
        }
        if (m_pHeadModel != NULL)
        {
            m_pHeadModel->PlayOutOfOrderAnimation("talk", pfWeights, pfTimes,
                                                  nCount, fLength);
        }
    }
    else
    {
        if (m_pModel != NULL)
        {
            m_pModel->StopOutOfOrderAnimation("talk");
        }
        if (m_pHeadModel != NULL)
        {
            m_pHeadModel->StopOutOfOrderAnimation("talk");
        }
    }
    return 1;
}
// PC 0x006F3E90 GOG 0x006F3940
void CSWCAnimBaseHeadWield::SetAlwaysRender(int bAlwaysRender)
{
    if (bAlwaysRender != 0)
    {
        if (m_pModel != NULL)
        {
            m_pModel->EnableAlwaysRender();
        }
        if (m_pHeadModel != NULL)
        {
            m_pHeadModel->EnableAlwaysRender();
        }
    }
    else
    {
        if (m_pModel != NULL)
        {
            m_pModel->DisableAlwaysRender();
        }
        if (m_pHeadModel != NULL)
        {
            m_pHeadModel->DisableAlwaysRender();
        }
    }
}
// PC 0x006F3F00 GOG 0x006F39B0
void CSWCAnimBaseHeadWield::SetInCutscene(int bInCutscene)
{
    if (bInCutscene != 0)
    {
        if (m_pModel != NULL)
        {
            m_pModel->EnableInCutscene();
        }
        if (m_pHeadModel != NULL)
        {
            m_pHeadModel->EnableInCutscene();
        }
    }
    else
    {
        if (m_pModel != NULL)
        {
            m_pModel->DisableInCutscene();
        }
        if (m_pHeadModel != NULL)
        {
            m_pHeadModel->DisableInCutscene();
        }
    }
}
// PC 0x006F3F70 GOG 0x006F3A20
int CSWCAnimBaseHeadWield::SetAnimationOnSubObject(unsigned char nModel,
                                                   unsigned short nAnimation,
                                                   float fSpeed, int nFlags,
                                                   float fTime)
{
    if (nModel == 0xFF)
    {
        return CSWCAnimBaseWield::SetAnimationOnSubObjectInternal(
            m_pModel, 0xFF, nAnimation, fSpeed, nFlags, fTime);
    }
    else
    {
        return CSWCAnimBaseTW::SetAnimationOnSubObjectInternal(
            m_pHeadModel, nModel, nAnimation, fSpeed, nFlags, fTime);
    }
}
// PC 0x006F3FE0 GOG 0x006F3A90
int CSWCAnimBaseHeadWield::SetAnimationInternal(CAurObject* pObject,
                                                unsigned short nAnimation,
                                                float fSpeed, int nFlags,
                                                float fTime)
{
    return CSWCAnimBase::SetAnimationInternal(pObject, nAnimation, fSpeed,
                                              nFlags, fTime);
}
// PC 0x006F3FF0 GOG 0x006F3AA0
int CSWCAnimBaseHeadWield::SetAnimationOnSubObjectInternal(
    CAurObject* pObject, unsigned char nModel, unsigned short nAnimation,
    float fSpeed, int nFlags, float fTime)
{
    return CSWCAnimBase::SetAnimationOnSubObjectInternal(
        pObject, nModel, nAnimation, fSpeed, nFlags, fTime);
}
// PC 0x006F4000 GOG 0x006F3AB0
int CSWCAnimBaseHeadWield::AddAnimationFlag(int nFlag)
{
    return CSWCAnimBase::AddAnimationFlagInternal(m_pModel, nFlag) != 0 &&
           CSWCAnimBase::AddAnimationFlagInternal(m_pHeadModel, nFlag) != 0;
}
// PC 0x006F4060 GOG 0x006F3B10
int CSWCAnimBaseHeadWield::RemoveAnimationFlag(int nFlag)
{
    return CSWCAnimBase::RemoveAnimationFlagInternal(m_pModel, nFlag) != 0 &&
           CSWCAnimBase::RemoveAnimationFlagInternal(m_pHeadModel, nFlag) !=
               0;
}
// PC 0x006F40C0 GOG 0x006F3B70
void CSWCAnimBaseHeadWield::TurnOnShadows()
{
    CSWCAnimBaseHead::TurnOnShadows();
    return CSWCAnimBaseWield::TurnOnShadows();
}
// PC 0x006F40F0 GOG 0x006F3BA0
void CSWCAnimBaseHeadWield::TurnOffShadows()
{
    CSWCAnimBaseHead::TurnOffShadows();
    return CSWCAnimBaseWield::TurnOffShadows();
}
// PC 0x006F4120 GOG 0x006F3BD0
void CSWCAnimBaseHeadWield::UpdateSound(int nEvent)
{
    return CSWCAnimBaseWield::UpdateSound(nEvent);
}
// PC 0x006F4200 GOG 0x006F3BE0
int CSWCAnimBaseHeadWield::EnableBumpedOutShell(const CResRef& sShell,
                                                float fScale)
{
    return CSWCAnimBaseHead::EnableBumpedOutShell(sShell, fScale);
}
// PC 0x006F4210 GOG 0x006F3BF0
int CSWCAnimBaseHeadWield::DisableBumpedOutShell()
{
    return CSWCAnimBaseHead::DisableBumpedOutShell();
}
// PC 0x006F4220 GOG 0x006F3C00
int CSWCAnimBaseHeadWield::SetMotionBlur(int bBlur)
{
    return CSWCAnimBaseHead::SetMotionBlur(bBlur);
}
// PC 0x006F4230 GOG 0x006F3C10
void CSWCAnimBaseHeadWield::EnableDistortion()
{
    CSWCAnimBaseHead::EnableDistortion();
    return CSWCAnimBaseWield::EnableDistortion();
}
// PC 0x006F4260 GOG 0x006F3C40
void CSWCAnimBaseHeadWield::DisableDistortion()
{
    CSWCAnimBaseHead::DisableDistortion();
    return CSWCAnimBaseWield::DisableDistortion();
}
// PC 0x006F4290 GOG 0x006F40C0
void CSWCAnimBaseHeadWield::SetHeadItem(unsigned long nItem)
{
    NWCANIMBASEHEADWIELD_TRACE();
    CSWCAnimBaseHead::SetHeadItem(nItem);
}

CSWCAnimBaseHeadWield::~CSWCAnimBaseHeadWield()
{
    UnloadModel();
}

static inline void NwcanimbaseheadwieldTrace1()
{
}
// PC 0x006F4130 GOG 0x006F3FF0
int CSWCAnimBaseHeadWield::GetBoundingRect(CSWGuiExtent& extent)
{
    NWCANIMBASEHEADWIELD_TRACE();
    if (m_pHeadModel != NULL)
    {
        CSWGuiExtent extentHead;
        CSWGuiExtent extentBody;
        extentHead.m_nLeft = extentHead.m_nTop = extentHead.m_nWidth =
            extentHead.m_nHeight = 0;
        extentBody.m_nLeft = extentBody.m_nTop = extentBody.m_nWidth =
            extentBody.m_nHeight = 0;
        if (CSWCAnimBase::GetBoundingRect(m_pHeadModel, extentHead) != 0 &&
            CSWCAnimBase::GetBoundingRect(extentBody) != 0)
        {
            extent = CSWGuiExtent::Union(extentHead, extentBody);
            return 1;
        }
        return 0;
    }
    return CSWCAnimBase::GetBoundingRect(extent);
}
// PC 0x006F3B70 GOG 0x006F3F70
void CSWCAnimBaseHeadWield::ShowHeadItem()
{
    NWCANIMBASEHEADWIELD_TRACE();
    if (m_bHeadItemHidden != 0)
    {
        CClientExoApp* pApp = g_pAppManager->GetClientExoApp();
        CSWCItem* pItem = pApp->GetItemByGameObjectID(m_nHeadItem);
        if (pItem != NULL)
        {
            Gob* pModel = pItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
            if (pModel != NULL)
            {
                pModel->AttachToObject(m_pHeadModel, "GoggleHook", Gob::ATTACH_NORMAL);
            }
        }
        RefreshHilite();
        m_bHeadItemHidden = 0;
    }
}
// PC 0x006F3AC0 GOG 0x006F3EC0
void CSWCAnimBaseHeadWield::HideHeadItem()
{
    NWCANIMBASEHEADWIELD_TRACE();
    CExoString sName;
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCItem* pItem = pApp->GetItemByGameObjectID(m_nHeadItem);
    if (pItem != NULL)
    {
        Gob* pModel = pItem->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        if (pModel != NULL)
        {
            pModel->AttachToObject(NULL, NULL, Gob::ATTACH_NORMAL);
            pModel->AttachToScene(NULL);
        }
    }
    m_bHeadItemHidden = 1;
}
