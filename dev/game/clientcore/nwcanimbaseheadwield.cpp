#include "nwcanimbaseheadwield.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwcitem.h"

static inline void NwcanimbaseheadwieldTrace1();
#define NWCANIMBASEHEADWIELD_TRACE() NwcanimbaseheadwieldTrace1()
// PC 0x00698EC0  Xbox 0x000C49D0
CSWCAnimBaseHeadWield::CSWCAnimBaseHeadWield()
{
    m_nAnimationType = ANIMATION_TYPE_BTYPE;
}
// PC 0x00698F90  Xbox 0x000C4AA0
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
// PC 0x00699000  Xbox 0x000C4B10
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
// PC 0x00699080  Xbox 0x000C4B90
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
// PC 0x006990C0  Xbox 0x000C4BD0
int CSWCAnimBaseHeadWield::ReplaceTexture(const CResRef& sOld,
                                          const CResRef& sNew, int nFlags)
{
    return CSWCAnimBaseTW::ReplaceTexture(sOld, sNew, nFlags) != 0 &&
           CSWCAnimBaseHead::ReplaceTexture(sOld, sNew, nFlags) != 0;
}
// PC 0x00699120  Xbox 0x000C4C30
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
// PC 0x00699180  Xbox 0x000C4C90
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
// PC 0x00699200  Xbox 0x000C4D10
int CSWCAnimBaseHeadWield::RestoreTexture()
{
    return CSWCAnimBaseHead::RestoreTexture();
}
// PC 0x00699210  Xbox 0x000C4D20
int CSWCAnimBaseHeadWield::RestoreTexture(unsigned char nModel)
{
    return CSWCAnimBaseHead::RestoreTexture(nModel);
}
// PC 0x00699220  Xbox 0x000C4D30
void CSWCAnimBaseHeadWield::UnloadModel()
{
    CSWCAnimBaseTW::UnloadModel();
    return CSWCAnimBaseHead::UnloadModel();
}
// PC 0x00699250  Xbox 0x000C4D60
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
// PC 0x00699290  Xbox 0x000C4DA0
int CSWCAnimBaseHeadWield::Wield(unsigned long nObject,
                                 const CExoString& sSlot,
                                 unsigned char nModel, float fScale)
{
    return CSWCAnimBaseWield::Wield(nObject, sSlot, nModel, fScale);
}
// PC 0x006992A0  Xbox 0x000C4DB0
void CSWCAnimBaseHeadWield::HideWieldedItems(const CResRef& sLeft,
                                            const CResRef& sRight)
{
    return CSWCAnimBaseWield::HideWieldedItems(sLeft, sRight);
}
// PC 0x006992B0  Xbox 0x000C4DC0
void CSWCAnimBaseHeadWield::ShowWieldedItems()
{
    return CSWCAnimBaseWield::ShowWieldedItems();
}
// PC 0x006992C0  Xbox 0x000C4DD0
void CSWCAnimBaseHeadWield::SetWieldLimited(int bLimited)
{
    CSWCAnimBaseWield::SetWieldLimited(bLimited);
}
// PC 0x006992D0  Xbox 0x000C4DE0
int CSWCAnimBaseHeadWield::SetEnvMap(int nType, int nFlags,
                                     const CResRef& sMap)
{
    return CSWCAnimBaseTW::SetEnvMap(nType, nFlags, sMap) != 0 &&
           CSWCAnimBaseHead::SetEnvMap(nType, nFlags, sMap) != 0;
}
// PC 0x00699330  Xbox 0x000C4E40
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
// PC 0x00699370  Xbox 0x000C4E80
int CSWCAnimBaseHeadWield::RestoreEnvMap()
{
    return CSWCAnimBaseTW::RestoreEnvMap() != 0 &&
           CSWCAnimBaseHead::RestoreEnvMap() != 0;
}
// PC 0x006993B0  Xbox 0x000C4EC0
int CSWCAnimBaseHeadWield::SetAnimation(unsigned short nAnimation,
                                        float fSpeed, int nFlags, float fTime)
{
    return CSWCAnimBaseWield::SetAnimationInternal(m_pModel, nAnimation,
                                                   fSpeed, nFlags, fTime) !=
               0 &&
           CSWCAnimBaseTW::SetAnimationInternal(m_pHeadModel, nAnimation,
                                                fSpeed, nFlags, fTime) != 0;
}
// PC 0x00699420  Xbox 0x000C4F30
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
// PC 0x006994A0  Xbox 0x000C4FB0
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
// PC 0x00699560  Xbox 0x000C50A0
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
// PC 0x006995D0  Xbox 0x000C5140
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
// PC 0x00699640  Xbox 0x000C51E0
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
// PC 0x006996B0  Xbox 0x000C5250
int CSWCAnimBaseHeadWield::SetAnimationInternal(CAurObject* pObject,
                                                unsigned short nAnimation,
                                                float fSpeed, int nFlags,
                                                float fTime)
{
    return CSWCAnimBase::SetAnimationInternal(pObject, nAnimation, fSpeed,
                                              nFlags, fTime);
}
// PC 0x006996C0  Xbox 0x000C5260
int CSWCAnimBaseHeadWield::SetAnimationOnSubObjectInternal(
    CAurObject* pObject, unsigned char nModel, unsigned short nAnimation,
    float fSpeed, int nFlags, float fTime)
{
    return CSWCAnimBase::SetAnimationOnSubObjectInternal(
        pObject, nModel, nAnimation, fSpeed, nFlags, fTime);
}
// PC 0x006996D0  Xbox 0x000C5270
int CSWCAnimBaseHeadWield::AddAnimationFlag(int nFlag)
{
    return CSWCAnimBase::AddAnimationFlagInternal(m_pModel, nFlag) != 0 &&
           CSWCAnimBase::AddAnimationFlagInternal(m_pHeadModel, nFlag) != 0;
}
// PC 0x00699730  Xbox 0x000C52D0
int CSWCAnimBaseHeadWield::RemoveAnimationFlag(int nFlag)
{
    return CSWCAnimBase::RemoveAnimationFlagInternal(m_pModel, nFlag) != 0 &&
           CSWCAnimBase::RemoveAnimationFlagInternal(m_pHeadModel, nFlag) !=
               0;
}
// PC 0x00699790  Xbox 0x000C5330
void CSWCAnimBaseHeadWield::TurnOnShadows()
{
    CSWCAnimBaseHead::TurnOnShadows();
    return CSWCAnimBaseWield::TurnOnShadows();
}
// PC 0x006997C0  Xbox 0x000C5360
void CSWCAnimBaseHeadWield::TurnOffShadows()
{
    CSWCAnimBaseHead::TurnOffShadows();
    return CSWCAnimBaseWield::TurnOffShadows();
}
// PC 0x006997F0  Xbox 0x000C5390
void CSWCAnimBaseHeadWield::UpdateSound(int nEvent)
{
    return CSWCAnimBaseWield::UpdateSound(nEvent);
}
// PC 0x00699800  Xbox 0x000C53A0
int CSWCAnimBaseHeadWield::EnableBumpedOutShell(const CResRef& sShell,
                                                float fScale)
{
    return CSWCAnimBaseHead::EnableBumpedOutShell(sShell, fScale);
}
// PC 0x00699810  Xbox 0x000C53B0
int CSWCAnimBaseHeadWield::DisableBumpedOutShell()
{
    return CSWCAnimBaseHead::DisableBumpedOutShell();
}
// PC 0x00699820  Xbox 0x000C53C0
int CSWCAnimBaseHeadWield::SetMotionBlur(int bBlur)
{
    return CSWCAnimBaseHead::SetMotionBlur(bBlur);
}
// PC 0x00699830  Xbox 0x000C53D0
void CSWCAnimBaseHeadWield::EnableDistortion()
{
    CSWCAnimBaseHead::EnableDistortion();
    return CSWCAnimBaseWield::EnableDistortion();
}
// PC 0x00699860  Xbox 0x000C5400
void CSWCAnimBaseHeadWield::DisableDistortion()
{
    CSWCAnimBaseHead::DisableDistortion();
    return CSWCAnimBaseWield::DisableDistortion();
}
// PC 0x00699AE0  Xbox 0x000C5680
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
// PC 0x00699B90  Xbox 0x000C5730
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
// PC 0x00699C10  Xbox 0x000C57B0
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
// PC 0x00699CE0  Xbox 0x000C5890
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
