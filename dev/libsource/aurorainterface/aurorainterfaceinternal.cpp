#include "aurora.h"

#include "exoresman.h"
#include "aurfonttexture.h"
#include "math3d.h"
// PC 0x00781650 GOG 0x00781450
CAuroraModel* CAuroraInterface::GetModelResHelper(
    const CExoString& sName, int bRequest, int* pbNew,
    CExoLinkedListNode** ppPosition)
{
    CExoString sLowerName;
    CExoString sModelName;
    CExoLinkedListNode* pPosition;
    CAuroraModel* pModel;
    unsigned char* pData;
    CAuroraModel* pOwner;

    sLowerName = sName.LowerCase();
    pPosition = m_lstRequestedModel.GetHeadPos();
    pModel = m_lstRequestedModel.GetHead();
    if (ppPosition != NULL)
    {
        *ppPosition = NULL;
    }
    while (pPosition != NULL)
    {
        if (pModel != NULL)
        {
            pModel->m_cResRef.CopyToString(sModelName);
            sModelName = sModelName.LowerCase();
            if (sModelName == sLowerName)
            {
                if (pbNew != NULL)
                {
                    *pbNew = 0;
                }
                if (ppPosition != NULL)
                {
                    *ppPosition = NULL;
                }
                if (bRequest != 0)
                {
                    return pModel;
                }
                m_lstRequestedModel.Remove(pPosition);
                break;
            }
            if (sModelName > sLowerName)
            {
                if (pbNew != NULL)
                {
                    *pbNew = 1;
                }
                if (ppPosition != NULL)
                {
                    *ppPosition = pPosition;
                }
                pModel = NULL;
                break;
            }
        }
        pModel = m_lstRequestedModel.GetNext(pPosition);
    }
    if (pModel == NULL)
    {
        pModel = new CAuroraModel(sName, bRequest);
        if (pModel == NULL)
        {
            return NULL;
        }
    }
    if (pModel->m_pRes != NULL && pModel->m_pRes->GetDemands() > 0)
    {
        pData = (unsigned char*)pModel->GetPtrToResHelperInfo();
        if (pData != NULL && *(unsigned short*)pData == RESTYPE_MDL)
        {
            pData += sizeof(unsigned short);
            pOwner = *(CAuroraModel**)pData;
            if (pOwner != NULL)
            {
                if (pbNew != NULL)
                {
                    *pbNew = 0;
                }
                delete pModel;
                pModel = pOwner;
            }
        }
    }
    return pModel;
}
// PC 0x007818B0 GOG 0x007816B0
CAuroraModel* CAuroraInterface::GetModelInternal(
    const CExoString& sName, int bRequest, int* pbNew,
    CExoLinkedListNode** ppPosition)
{
    CAuroraModel* pModel;
    CRes* pRes;

    pModel =
        GetModelResHelper(sName, bRequest, pbNew, ppPosition);
    if (pModel == NULL)
    {
        return NULL;
    }
    if (bRequest == 0 && pModel->Load() == 0)
    {
        pRes = pModel->m_pRes;
        if (pRes == NULL || pRes->GetDemands() < 1)
        {
            delete pModel;
        }
        return NULL;
    }
    return pModel;
}
// PC 0x00781910 GOG 0x00781710
void CAuroraInterface::ReleaseModelInternal(CAuroraModel* pModel)
{
    SMDLDataTrailer* pTrailer;

    if (pModel->m_pRes->GetDemands() >= 1)
    {
        pTrailer = (SMDLDataTrailer*)pModel->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_MDL &&
            pTrailer->pOwner == pModel)
        {
            pModel->Unload();
            if (pModel->m_pRes->GetDemands() < 1)
            {
                delete pModel;
            }
        }
    }
}
// PC 0x00781960 GOG 0x00781760
CAuroraTexture* CAuroraInterface::GetTextureInternal(const CExoString& sName)
{
    CAuroraTexture* pTexture;
    STGATextureTrailer* pTrailer;
    CAuroraTexture* pOwner;
    CRes* pRes;

    pTexture = new CAuroraTexture(sName, 0);
    if (pTexture != NULL && pTexture->m_pRes != NULL &&
        pTexture->m_pRes->GetDemands() > 0)
    {
        pTrailer = (STGATextureTrailer*)pTexture->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_TGA &&
            pTrailer->pOwner != NULL)
        {
            pOwner = pTrailer->pOwner;
            delete pTexture;
            pTexture = pOwner;
        }
    }
    if (pTexture == NULL)
    {
        return NULL;
    }
    if (pTexture->Load() == 0)
    {
        pRes = pTexture->m_pRes;
        if (pRes == NULL || pRes->GetDemands() < 1)
        {
            delete pTexture;
        }
        return NULL;
    }
    return pTexture;
}
// PC 0x00781A50 GOG 0x00781850
CAurora16BitTexture* CAuroraInterface::Get16BitTextureInternal(
    const CExoString& sName)
{
    CAurora16BitTexture* pTexture;
    CAurora16BitTexture* pOwner;
    S16BitTextureDataTrailer* pTrailer;

    pTexture = new CAurora16BitTexture(sName, 0);
    if (pTexture != NULL)
    {
        if (pTexture->m_pRes != NULL &&
            pTexture->m_pRes->GetDemands() > 0)
        {
            pTrailer =
                (S16BitTextureDataTrailer*)pTexture->GetPtrToResHelperInfo();
            if (pTrailer != NULL && pTrailer->nType == RESTYPE_4PC)
            {
                pOwner = pTrailer->pOwner;
                if (pOwner != NULL)
                {
                    delete pTexture;
                    pTexture = pOwner;
                }
            }
        }
    }
    if (pTexture == NULL)
    {
        return NULL;
    }
    if (pTexture->Load() == 0)
    {
        if (pTexture->m_pRes == NULL ||
            pTexture->m_pRes->GetDemands() < 1)
        {
            delete pTexture;
        }
        return NULL;
    }
    return pTexture;
}

CAuroraFontTexture* CAuroraInterface::GetFontTextureInternal(
    const CExoString& sName)
{
    CAuroraFontTexture* pTexture;
    CAuroraFontTexture* pOwner;
    SFontTextureDataTrailer* pTrailer;

    pTexture = new CAuroraFontTexture(sName, 0);
    if (pTexture != NULL)
    {
        if (pTexture->m_pRes != NULL &&
            pTexture->m_pRes->GetDemands() > 0)
        {
            pTrailer =
                (SFontTextureDataTrailer*)pTexture->GetPtrToResHelperInfo();
            if (pTrailer != NULL && pTrailer->nType == RESTYPE_FNT)
            {
                pOwner = pTrailer->pOwner;
                if (pOwner != NULL)
                {
                    delete pTexture;
                    pTexture = pOwner;
                }
            }
        }
    }
    if (pTexture == NULL)
    {
        return NULL;
    }
    if (pTexture->Load() == 0)
    {
        if (pTexture->m_pRes == NULL ||
            pTexture->m_pRes->GetDemands() < 1)
        {
            delete pTexture;
        }
        return NULL;
    }
    return pTexture;
}
// PC 0x00781B40 GOG 0x00781940
CAuroraLayeredTexture* CAuroraInterface::GetLayeredTextureInternal(
    const CExoString& sName)
{
    CAuroraLayeredTexture* pTexture;
    CAuroraLayeredTexture* pOwner;
    SPLTLayeredTrailer* pTrailer;

    pTexture = new CAuroraLayeredTexture(sName, 0);
    if (pTexture != NULL)
    {
        if (pTexture->m_pRes != NULL &&
            pTexture->m_pRes->GetDemands() > 0)
        {
            pTrailer =
                (SPLTLayeredTrailer*)pTexture->GetPtrToResHelperInfo();
            if (pTrailer != NULL && pTrailer->nType == RESTYPE_PLT)
            {
                pOwner = pTrailer->pOwner;
                if (pOwner != NULL)
                {
                    delete pTexture;
                    pTexture = pOwner;
                }
            }
        }
    }
    if (pTexture == NULL)
    {
        return NULL;
    }
    if (pTexture->Load() == 0)
    {
        if (pTexture->m_pRes == NULL ||
            pTexture->m_pRes->GetDemands() < 1)
        {
            delete pTexture;
        }
        return NULL;
    }
    return pTexture;
}
// PC 0x00781C30 GOG 0x00781A30
CAuroraCompressedTexture* CAuroraInterface::GetCompressedTextureInternal(
    const CExoString& sName)
{
    CAuroraCompressedTexture* pTexture;
    CAuroraCompressedTexture* pOwner;
    SCompressedTextureDataTrailer* pTrailer;

    pTexture = new CAuroraCompressedTexture(sName, 0);
    if (pTexture != NULL)
    {
        if (pTexture->m_pRes != NULL &&
            pTexture->m_pRes->GetDemands() > 0)
        {
            pTrailer = (SCompressedTextureDataTrailer*)
                pTexture->GetPtrToResHelperInfo();
            if (pTrailer != NULL && pTrailer->nType == RESTYPE_DDS)
            {
                pOwner = pTrailer->pOwner;
                if (pOwner != NULL)
                {
                    delete pTexture;
                    pTexture = pOwner;
                }
            }
        }
    }
    if (pTexture == NULL)
    {
        return NULL;
    }
    if (pTexture->Load() == 0)
    {
        if (pTexture->m_pRes == NULL ||
            pTexture->m_pRes->GetDemands() < 1)
        {
            delete pTexture;
        }
        return NULL;
    }
    return pTexture;
}
// PC 0x00781D20 GOG 0x00781B20
CAuroraProcessedTexture* CAuroraInterface::GetProcessedTextureInternal(
    const CExoString& sName)
{
    CAuroraProcessedTexture* pTexture;
    CAuroraProcessedTexture* pOwner;
    SProcessedTextureDataTrailer* pTrailer;

    pTexture = new CAuroraProcessedTexture(sName, 0);
    if (pTexture != NULL)
    {
        if (pTexture->m_pRes != NULL &&
            pTexture->m_pRes->GetDemands() > 0)
        {
            pTrailer = (SProcessedTextureDataTrailer*)
                pTexture->GetPtrToResHelperInfo();
#ifdef _XBOX
            if (pTrailer != NULL && pTrailer->nType == 3006)
#else
            if (pTrailer != NULL && pTrailer->nType == RESTYPE_TPC)
#endif
            {
                pOwner = pTrailer->pOwner;
                if (pOwner != NULL)
                {
                    delete pTexture;
                    pTexture = pOwner;
                }
            }
        }
    }
    if (pTexture == NULL)
    {
        return NULL;
    }
    if (pTexture->Load() == 0)
    {
        if (pTexture->m_pRes == NULL ||
            pTexture->m_pRes->GetDemands() < 1)
        {
            delete pTexture;
        }
        return NULL;
    }
    return pTexture;
}
// PC 0x00781E10 GOG 0x00781C10
CAuroraTXI* CAuroraInterface::GetTXIInternal(const CExoString& sName)
{
    CAuroraTXI* pTXI;
    CAuroraTXI* pOwner;
    STXIResourcePrefix* pTrailer;

    pTXI = new CAuroraTXI(sName, 0);
    if (pTXI != NULL)
    {
        if (pTXI->m_pRes != NULL && pTXI->m_pRes->GetDemands() > 0)
        {
            pTrailer = (STXIResourcePrefix*)pTXI->GetPtrToResHelperInfo();
            if (pTrailer != NULL && pTrailer->nType == RESTYPE_TXI)
            {
                pOwner = pTrailer->pOwner;
                if (pOwner != NULL)
                {
                    delete pTXI;
                    pTXI = pOwner;
                }
            }
        }
    }
    if (pTXI == NULL)
    {
        return NULL;
    }
    if (pTXI->Load() == 0)
    {
        if (pTXI->m_pRes == NULL || pTXI->m_pRes->GetDemands() < 1)
        {
            delete pTXI;
        }
        return NULL;
    }
    return pTXI;
}
// PC 0x00781F00 GOG 0x00781D00
CAuroraVisibility* CAuroraInterface::GetVisibilityInternal(
    const CExoString& sName)
{
    CAuroraVisibility* pVisibility;
    CAuroraVisibility* pOwner;
    SVISDataTrailer* pTrailer;

    pVisibility = new CAuroraVisibility(sName, 0);
    if (pVisibility != NULL)
    {
        if (pVisibility->m_pRes != NULL &&
            pVisibility->m_pRes->GetDemands() > 0)
        {
            pTrailer = (SVISDataTrailer*)pVisibility->GetPtrToResHelperInfo();
            if (pTrailer != NULL && pTrailer->nType == RESTYPE_VIS)
            {
                pOwner = pTrailer->pOwner;
                if (pOwner != NULL)
                {
                    delete pVisibility;
                    pVisibility = pOwner;
                }
            }
        }
    }
    if (pVisibility == NULL)
    {
        return NULL;
    }
    if (pVisibility->Load() == 0)
    {
        if (pVisibility->m_pRes == NULL ||
            pVisibility->m_pRes->GetDemands() < 1)
        {
            delete pVisibility;
        }
        return NULL;
    }
    return pVisibility;
}
// PC 0x00781FF0 GOG 0x00781DF0
CAuroraModelX* CAuroraInterface::GetModelXInternal(const CExoString& sName)
{
    CAuroraModelX* pModel;
    CAuroraModelX* pOwner;
    SMDXResPrefix* pTrailer;

    pModel = new CAuroraModelX(sName, 0);
    if (pModel != NULL)
    {
        if (pModel->m_pRes != NULL && pModel->m_pRes->GetDemands() > 0)
        {
            pTrailer = (SMDXResPrefix*)pModel->GetPtrToResHelperInfo();
            if (pTrailer != NULL && pTrailer->nType == RESTYPE_MDX)
            {
                pOwner = pTrailer->pOwner;
                if (pOwner != NULL)
                {
                    delete pModel;
                    pModel = pOwner;
                }
            }
        }
    }
    if (pModel == NULL)
    {
        return NULL;
    }
    if (pModel->Load() == 0)
    {
        if (pModel->m_pRes == NULL || pModel->m_pRes->GetDemands() < 1)
        {
            delete pModel;
        }
        return NULL;
    }
    return pModel;
}
// PC 0x007820E0 GOG 0x00781EE0
CAuroraTexture* CAuroraInterface::GetTexture(const CExoString& sName)
{
    return GetTextureInternal(sName);
}
// PC 0x007820F0 GOG 0x00781EF0
void CAuroraInterface::ReleaseTextureInternal(CAuroraTexture* pTexture)
{
    STGATextureTrailer* pTrailer;

    if (pTexture->m_pRes->GetDemands() >= 1)
    {
        pTrailer =
            (STGATextureTrailer*)pTexture->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_TGA &&
            pTrailer->pOwner == pTexture)
        {
            pTexture->Unload();
            if (pTexture->m_pRes->GetDemands() < 1)
            {
                delete pTexture;
            }
        }
    }
}
// PC 0x00782140 GOG 0x00781F40
CAurora16BitTexture* CAuroraInterface::Get16BitTexture(const CExoString& sName)
{
    return Get16BitTextureInternal(sName);
}
// PC 0x00782150 GOG 0x00781F50
void CAuroraInterface::Release16BitTextureInternal(
    CAurora16BitTexture* pTexture)
{
    S16BitTextureDataTrailer* pTrailer;

    if (pTexture->m_pRes->GetDemands() >= 1)
    {
        pTrailer =
            (S16BitTextureDataTrailer*)pTexture->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_4PC &&
            pTrailer->pOwner == pTexture)
        {
            pTexture->Unload();
            if (pTexture->m_pRes->GetDemands() < 1)
            {
                delete pTexture;
            }
        }
    }
}

void CAuroraInterface::ReleaseFontTextureInternal(
    CAuroraFontTexture* pTexture)
{
    SFontTextureDataTrailer* pTrailer;

    if (pTexture->m_pRes->GetDemands() >= 1)
    {
        pTrailer =
            (SFontTextureDataTrailer*)pTexture->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_FNT &&
            pTrailer->pOwner == pTexture)
        {
            pTexture->Unload();
            if (pTexture->m_pRes->GetDemands() < 1)
            {
                delete pTexture;
            }
        }
    }
}
// PC 0x007821A0 GOG 0x00781FA0
CAuroraLayeredTexture* CAuroraInterface::GetLayeredTexture(const CExoString& sName)
{
    return GetLayeredTextureInternal(sName);
}
// PC 0x007821B0 GOG 0x00781FB0
void CAuroraInterface::ReleaseLayeredTextureInternal(
    CAuroraLayeredTexture* pTexture)
{
    SPLTLayeredTrailer* pTrailer;

    if (pTexture->m_pRes->GetDemands() >= 1)
    {
        pTrailer =
            (SPLTLayeredTrailer*)pTexture->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_PLT &&
            pTrailer->pOwner == pTexture)
        {
            pTexture->Unload();
            if (pTexture->m_pRes->GetDemands() < 1)
            {
                delete pTexture;
            }
        }
    }
}
// PC 0x00782200 GOG 0x00782000
CAuroraCompressedTexture* CAuroraInterface::GetCompressedTexture(const CExoString& sName)
{
    return GetCompressedTextureInternal(sName);
}
// PC 0x00782210 GOG 0x00782010
void CAuroraInterface::ReleaseCompressedTextureInternal(
    CAuroraCompressedTexture* pTexture)
{
    SCompressedTextureDataTrailer* pTrailer;

    if (pTexture->m_pRes->GetDemands() >= 1)
    {
        pTrailer = (SCompressedTextureDataTrailer*)
            pTexture->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_DDS &&
            pTrailer->pOwner == pTexture)
        {
            pTexture->Unload();
            if (pTexture->m_pRes->GetDemands() < 1)
            {
                delete pTexture;
            }
        }
    }
}
// PC 0x00782260 GOG 0x00782060
CAuroraProcessedTexture* CAuroraInterface::GetProcessedTexture(const CExoString& sName)
{
    return GetProcessedTextureInternal(sName);
}
// PC 0x00782270 GOG 0x00782070
void CAuroraInterface::ReleaseProcessedTextureInternal(
    CAuroraProcessedTexture* pTexture)
{
    SProcessedTextureDataTrailer* pTrailer;

    if (pTexture->m_pRes->GetDemands() >= 1)
    {
        pTrailer = (SProcessedTextureDataTrailer*)
            pTexture->GetPtrToResHelperInfo();
#ifdef _XBOX
        if (pTrailer != NULL && pTrailer->nType == 3006 &&
#else
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_TPC &&
#endif
            pTrailer->pOwner == pTexture)
        {
            pTexture->Unload();
            if (pTexture->m_pRes->GetDemands() < 1)
            {
                delete pTexture;
            }
        }
    }
}
// PC 0x007822C0 GOG 0x007820C0
CAuroraTXI* CAuroraInterface::GetTXI(const CExoString& sName)
{
    return GetTXIInternal(sName);
}
// PC 0x007822D0 GOG 0x007820D0
void CAuroraInterface::ReleaseTXIInternal(CAuroraTXI* pTXI)
{
    STXIResourcePrefix* pTrailer;

    if (pTXI->m_pRes->GetDemands() >= 1)
    {
        pTrailer = (STXIResourcePrefix*)pTXI->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_TXI &&
            pTrailer->pOwner == pTXI)
        {
            pTXI->Unload();
            if (pTXI->m_pRes->GetDemands() < 1)
            {
                delete pTXI;
            }
        }
    }
}
// PC 0x00782320 GOG 0x00782120
CAuroraVisibility* CAuroraInterface::GetVisibility(const CExoString& sName)
{
    return GetVisibilityInternal(sName);
}
// PC 0x00782330 GOG 0x00782130
void CAuroraInterface::ReleaseVisibilityInternal(CAuroraVisibility* pVisibility)
{
    SVISDataTrailer* pTrailer;

    if (pVisibility->m_pRes->GetDemands() >= 1)
    {
        pTrailer = (SVISDataTrailer*)pVisibility->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_VIS &&
            pTrailer->pOwner == pVisibility)
        {
            pVisibility->Unload();
            if (pVisibility->m_pRes->GetDemands() < 1)
            {
                delete pVisibility;
            }
        }
    }
}
// PC 0x00782380 GOG 0x00782180
CAuroraModelX* CAuroraInterface::GetModelX(const CExoString& sName)
{
    return GetModelXInternal(sName);
}
// PC 0x00782390 GOG 0x00782190
void CAuroraInterface::ReleaseModelXInternal(CAuroraModelX* pModel)
{
    SMDXResPrefix* pTrailer;

    if (pModel->m_pRes->GetDemands() >= 1)
    {
        pTrailer = (SMDXResPrefix*)pModel->GetPtrToResHelperInfo();
        if (pTrailer != NULL && pTrailer->nType == RESTYPE_MDX &&
            pTrailer->pOwner == pModel)
        {
            pModel->Unload();
            if (pModel->m_pRes->GetDemands() < 1)
            {
                delete pModel;
            }
        }
    }
}
