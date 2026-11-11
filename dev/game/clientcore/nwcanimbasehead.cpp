#include <string.h>
#include "nwcanimbasehead.h"
#include "aurmodel.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwcitem.h"

static const unsigned char ANIM_MODEL_HEAD = 254;

static inline void NwcanimbaseheadTrace1();
// PC 0x0069BB80  Xbox 0x000C7770
CSWCAnimBaseHead::CSWCAnimBaseHead()
{
    m_pHeadModel = NULL;
    m_nAnimationType = ANIMATION_TYPE_HEAD;
    m_cReplacedTexture = "";
    m_bTextureReplaced = 0;
    m_nPartCount = 0;
    m_nPartStart = 0;
    memset(m_aParts, 0, sizeof(m_aParts));
    m_cEnvMap = "";
    m_nEnvMapType = 0;
    m_nHeadItem = OBJECT_INVALID;
    NwcanimbaseheadTrace1();
}
// PC 0x0069B030  Xbox 0x000C6BC0
CSWCAnimBaseHead::~CSWCAnimBaseHead()
{
    UnloadModel();
}
// PC 0x0069B060  Xbox 0x000C6BF0
Gob* CSWCAnimBaseHead::GetModel(unsigned char nModel)
{
    switch (nModel)
    {
    case 254:
        return m_pHeadModel;
    case 255:
        return CSWCAnimBaseTW::GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
    }
    return NULL;
}
// PC 0x0069B0A0  Xbox 0x000C6C30
int CSWCAnimBaseHead::LoadModel(const CResRef& cModel, unsigned char nModel)
{
    char szModel[20];

    switch (nModel)
    {
    case MODEL_SLOT_MAIN:
        if (CSWCAnimBaseTW::LoadModel(cModel, MODEL_SLOT_MAIN) == 0)
        {
            return 0;
        }
        if (m_pHeadModel != NULL)
        {
            m_pHeadModel->AttachToObject(m_pModel, "HEADHOOK", Gob::ATTACH_NORMAL);
        }
        return 1;
    case MODEL_WINGS:
    case MODEL_TAIL:
        return CSWCAnimBaseTW::LoadModel(cModel, nModel) != 0;
    case ANIM_MODEL_HEAD:
        cModel.CopyToString(szModel);
        m_pHeadModel = (Gob*)NewCAurObject(szModel, "head", 0, 0);
        if (m_pHeadModel == NULL)
        {
            return 0;
        }
        m_cReplacedTexture = cModel;
        m_bTextureReplaced = 0;
        m_pHeadModel->AttachToObject(m_pModel, "HEADHOOK", Gob::ATTACH_NORMAL);
        return 1;
    }
    return 0;
}
// PC 0x0069B1C0  Xbox 0x000C6D80
int CSWCAnimBaseHead::ReplaceTexture(const CResRef& cOld, const CResRef& cNew,
                                     int bSave)
{
    char sOld[20];
    char sNew[20];

    cOld.CopyToString(sOld);
    cNew.CopyToString(sNew);
    strlwr(sNew);
    strlwr(sOld);
    if (m_pHeadModel != NULL)
    {
        m_pHeadModel->ReplaceTexture(sOld, sNew);
    }
    if (bSave != 0)
    {
        m_cReplacedTexture = cNew;
        m_bTextureReplaced = 0;
    }
    return CSWCAnimBaseTW::ReplaceTexture(cOld, cNew, bSave);
}
// PC 0x0069B250  Xbox 0x000C6E30
int CSWCAnimBaseHead::ReplaceTexture(unsigned char nModel, const CResRef& cOld,
                                     const CResRef& cNew, int bSave)
{
    char sNew[20];
    char sOld[20];

    if (nModel == 254)
    {
        if (m_pHeadModel == NULL)
        {
            return 0;
        }
        cOld.CopyToString(sOld);
        cNew.CopyToString(sNew);
        strlwr(sOld);
        strlwr(sNew);
        m_pHeadModel->ReplaceTexture(sOld, sNew);
        if (bSave != 0)
        {
            m_cReplacedTexture = cNew;
            m_bTextureReplaced = 0;
        }
        return 1;
    }
    return CSWCAnimBaseTW::ReplaceTexture(nModel, cOld, cNew, bSave);
}
// PC 0x0069B310  Xbox 0x000C6F10
int CSWCAnimBaseHead::ReplaceTexture(unsigned char nModel, const CResRef& cOld,
                                     const CResRef& cNew, unsigned long nCount,
                                     unsigned short* pParts, int bSave,
                                     unsigned long nStart)
{
    char sNew[20];
    char sOld[20];

    if (nModel == 254)
    {
        if (m_pHeadModel == NULL)
        {
            return 0;
        }
        cOld.CopyToString(sOld);
        cNew.CopyToString(sNew);
        strlwr(sOld);
        strlwr(sNew);
        m_pHeadModel->ReplaceTexture(sOld, sNew, nCount, pParts, nStart);
        if (bSave != 0)
        {
            m_cReplacedTexture = cNew;
            m_bTextureReplaced = 1;
            m_nPartCount = (unsigned char)nCount;
            m_nPartStart = (unsigned char)nStart;
            memcpy(m_aParts, &pParts[nStart], nCount * 2);
        }
        return 1;
    }
    return CSWCAnimBaseTW::ReplaceTexture(nModel, cOld, cNew, nCount, pParts,
                                          bSave, nStart);
}
// PC 0x0069B410  Xbox 0x000C7040
int CSWCAnimBaseHead::ReplaceTexture(const CResRef& cOld, const CResRef& cNew,
                                     unsigned long nCount,
                                     unsigned short* pParts, int bSave,
                                     unsigned long nStart)
{
    char sNew[20];
    char sOld[20];

    cOld.CopyToString(sOld);
    cNew.CopyToString(sNew);
    strlwr(sOld);
    strlwr(sNew);
    if (m_pHeadModel == NULL)
    {
        m_pHeadModel->ReplaceTexture(sOld, sNew, nCount, pParts, nStart);
    }
    if (bSave != 0)
    {
        m_cReplacedTexture = cNew;
        m_bTextureReplaced = 1;
        m_nPartCount = (unsigned char)nCount;
        m_nPartStart = (unsigned char)nStart;
        memcpy(m_aParts, &pParts[nStart], nCount * 2);
    }
    return CSWCAnimBaseTW::ReplaceTexture(cOld, cNew, nCount, pParts, bSave,
                                          nStart);
}
// PC 0x0069B4F0  Xbox 0x000C7130
int CSWCAnimBaseHead::RestoreTexture()
{
    if (m_pHeadModel != NULL)
    {
        if (m_bTextureReplaced != 0)
        {
            CResRef sTexture("");
            ReplaceTexture(ANIM_MODEL_HEAD, sTexture, m_cReplacedTexture,
                           m_nPartCount, m_aParts, 0, m_nPartStart);
        }
        else
        {
            m_pHeadModel->RestoreTexture();
        }
    }
    return CSWCAnimBaseTW::RestoreTexture();
}
// PC 0x0069B570  Xbox 0x000C71C0
int CSWCAnimBaseHead::RestoreTexture(unsigned char nModel)
{
    if (nModel == ANIM_MODEL_HEAD)
    {
        if (m_pHeadModel != NULL)
        {
            if (m_bTextureReplaced != 0)
            {
                CResRef sTexture("");
                ReplaceTexture(ANIM_MODEL_HEAD, sTexture, m_cReplacedTexture,
                               m_nPartCount, m_aParts, 0, m_nPartStart);
                return 1;
            }
            m_pHeadModel->RestoreTexture();
            return 1;
        }
        return 0;
    }
    return CSWCAnimBaseTW::RestoreTexture(nModel);
}
// PC 0x0069B610  Xbox 0x000C7260
void CSWCAnimBaseHead::UnloadModel()
{
    if (m_pHeadModel != NULL)
    {
        m_pHeadModel->AttachToObject(NULL, NULL, Gob::ATTACH_NORMAL);
        delete m_pHeadModel;
        m_pHeadModel = NULL;
    }
    CSWCAnimBaseTW::UnloadModel();
}
// PC 0x0069B650  Xbox 0x000C72A0
void CSWCAnimBaseHead::UnloadModel(unsigned char nModel)
{
    if (m_pHeadModel != NULL && nModel == ANIM_MODEL_HEAD)
    {
        m_pHeadModel->AttachToObject(NULL, NULL, Gob::ATTACH_NORMAL);
        delete m_pHeadModel;
        m_pHeadModel = NULL;
    }
    else
    {
        CSWCAnimBaseTW::UnloadModel(nModel);
    }
}
// PC 0x0069B6A0  Xbox 0x000C72F0
int CSWCAnimBaseHead::SetEnvMap(int nType, int nFlags, const CResRef& cEnvMap)
{
    char sEnvMap[20];
    if (m_pHeadModel == NULL)
    {
        return 0;
    }
    if (nType != 0)
    {
        cEnvMap.CopyToString(sEnvMap);
        m_pHeadModel->SetEnvironmentMap(sEnvMap);
    }
    else
    {
        m_pHeadModel->SetEnvironmentMap("");
    }
    if (nFlags != 0)
    {
        m_cEnvMap = cEnvMap;
        m_nEnvMapType = nType;
    }
    return CSWCAnimBaseTW::SetEnvMap(nType, nFlags, cEnvMap);
}
// PC 0x0069B720  Xbox 0x000C7390
int CSWCAnimBaseHead::SetEnvMap(unsigned char nModel, int nType, int nFlags,
                                const CResRef& cEnvMap)
{
    char sEnvMap[20];
    if (nModel == 254)
    {
        if (m_pHeadModel == NULL)
        {
            return 0;
        }
        if (nType != 0)
        {
            cEnvMap.CopyToString(sEnvMap);
            m_pHeadModel->SetEnvironmentMap(sEnvMap);
        }
        else
        {
            m_pHeadModel->SetEnvironmentMap("");
        }
        if (nFlags != 0)
        {
            m_cEnvMap = cEnvMap;
            m_nEnvMapType = nType;
        }
        return 1;
    }
    return CSWCAnimBaseTW::SetEnvMap(nModel, nType, nFlags, cEnvMap);
}
// PC 0x0069B7C0  Xbox 0x000C7450
int CSWCAnimBaseHead::RestoreEnvMap()
{
    if (m_pHeadModel == NULL)
    {
        return 0;
    }
    if (m_nEnvMapType != 0)
    {
        char sEnvMap[20];
        m_cEnvMap.CopyToString(sEnvMap);
        m_pHeadModel->SetEnvironmentMap(sEnvMap);
    }
    else
    {
        m_pHeadModel->SetEnvironmentMap("");
    }
    return CSWCAnimBaseTW::RestoreEnvMap();
}
// PC 0x0069B810  Xbox 0x000C74C0
void CSWCAnimBaseHead::TurnOnShadows()
{
    if (m_pHeadModel != NULL)
    {
        m_pHeadModel->TurnOnShadows();
    }
    CSWCAnimBase::TurnOnShadows();
}
// PC 0x0069B840  Xbox 0x000C74F0
void CSWCAnimBaseHead::TurnOffShadows()
{
    if (m_pHeadModel != NULL)
    {
        m_pHeadModel->TurnOffShadows();
    }
    CSWCAnimBase::TurnOffShadows();
}
// PC 0x0069B870  Xbox 0x000C7520
int CSWCAnimBaseHead::EnableBumpedOutShell(const CResRef& cTexture,
                                           float fHeight)
{
    char sTexture[20];

    if (m_pHeadModel == NULL)
    {
        return 0;
    }
    cTexture.CopyToString(sTexture);
    strlwr(sTexture);
    m_pHeadModel->EnableRenderBumpedOut(sTexture, fHeight);
    return CSWCAnimBase::EnableBumpedOutShell(cTexture, fHeight);
}
// PC 0x0069B8E0  Xbox 0x000C75A0
int CSWCAnimBaseHead::DisableBumpedOutShell()
{
    if (m_pHeadModel == NULL)
    {
        return 0;
    }
    m_pHeadModel->DisableRenderBumpedOut();
    return CSWCAnimBase::DisableBumpedOutShell();
}
// PC 0x0069B910  Xbox 0x000C75D0
int CSWCAnimBaseHead::SetMotionBlur(int bBlur)
{
    if (m_pHeadModel == NULL)
    {
        return 0;
    }
    m_pHeadModel->SetMotionBlurred(bBlur != 0);
    return CSWCAnimBase::SetMotionBlur(bBlur);
}
// PC 0x0069B950  Xbox 0x000C7860
void CSWCAnimBaseHead::EnableDistortion()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCItem* pItem = pApp->GetItemByGameObjectID(m_nHeadItem);
    if (m_pHeadModel != NULL)
    {
        m_pHeadModel->EnableDistortion();
    }
    if (pItem != NULL)
    {
        CSWCObject* pObject = pItem;
        if (pObject->GetModel(255, 1) != NULL)
        {
            pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->EnableDistortion();
        }
    }
    CSWCAnimBase::EnableDistortion();
}
// PC 0x0069B9C0  Xbox 0x000C78E0
void CSWCAnimBaseHead::DisableDistortion()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    CSWCItem* pItem = pApp->GetItemByGameObjectID(m_nHeadItem);
    if (m_pHeadModel != NULL)
    {
        m_pHeadModel->DisableDistortion();
    }
    if (pItem != NULL)
    {
        CSWCObject* pObject = pItem;
        if (pObject->GetModel(255, 1) != NULL)
        {
            pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->DisableDistortion();
        }
    }
    CSWCAnimBase::DisableDistortion();
}

static inline void NwcanimbaseheadTrace1()
{
}
