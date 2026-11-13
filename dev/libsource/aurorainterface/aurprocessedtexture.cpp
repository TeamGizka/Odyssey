#include "aurora.h"
// PC 0x00783630 GOG 0x00783460
void CAuroraProcessedTexture::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x00782B60 GOG 0x007829A0
int CAuroraProcessedTexture::GetProcessedTextureAttrib(
    int* pnWidth, int* pnHeight, char* pnEncoding, int* pnCompressedSize,
    int* pnDataSize, int* pnTXISize, float* pfAlphaBlending,
    char* pnMipMapCount)
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTPCAttrib(pnWidth, pnHeight, pnEncoding,
                                    pnCompressedSize, pnDataSize, pnTXISize,
                                    pfAlphaBlending, pnMipMapCount);
    }
    return 0;
}
// PC 0x00782BB0 GOG 0x007810A0
unsigned char* CAuroraProcessedTexture::GetProcessedTextureDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTPCDataPtr();
    }
    return 0;
}
// PC 0x00782BE0 GOG 0x007829F0
int CAuroraProcessedTexture::GetProcessedTextureDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTPCSize();
    }
    return 0;
}
// PC 0x00782C10 GOG 0x00782A20
unsigned char* CAuroraProcessedTexture::GetProcessedTextureTXIPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTPCTXIPtr();
    }
    return 0;
}
// PC 0x00782C40 GOG 0x00782A50
int CAuroraProcessedTexture::GetProcessedTextureTXISize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTPCTXISize();
    }
    return 0;
}
// PC 0x00782C70 GOG 0x00782A80
int CAuroraProcessedTexture::Load()
{
    int nWidth;
    int nHeight;
    char nEncoding;
    int nCompressedSize;
    int nDataSize;
    int nTXISize;
    float fAlphaBlending;
    char nMipMapCount;
    int nBlockSize;
    int nFaces = 1;
    int nSize = 0;
    unsigned char* pData;

    if (m_pRes->Demand())
    {
        m_pRes->GetTPCAttrib(&nWidth, &nHeight, &nEncoding, &nCompressedSize,
                             &nDataSize, &nTXISize, &fAlphaBlending, &nMipMapCount);
        if ((nEncoding != CResTPC::TPC_ENCODING_SINGLE_CHANNEL &&
             nEncoding != CResTPC::TPC_ENCODING_RGB &&
             nEncoding != CResTPC::TPC_ENCODING_RGBA) ||
            !(pData = (unsigned char*)m_pRes->GetTPCDataPtr()))
        {
            m_pRes->Release();
            return 0;
        }
    }
    else
    {
        return 0;
    }
    if (nCompressedSize != 0)
    {
        if (nEncoding == CResTPC::TPC_ENCODING_RGB)
        {
            nBlockSize = CResTPC::TPC_COMPRESSED_RGB_BLOCK_SIZE;
        }
        else if (nEncoding == CResTPC::TPC_ENCODING_RGBA)
        {
            nBlockSize = CResTPC::TPC_COMPRESSED_RGBA_BLOCK_SIZE;
        }

        int nMipWidth = nWidth;
        int nMipHeight = nHeight;
        if (nMipHeight / nMipWidth == CResTPC::TPC_CUBE_FACE_COUNT)
        {
            nFaces = CResTPC::TPC_CUBE_FACE_COUNT;
            nMipHeight /= CResTPC::TPC_CUBE_FACE_COUNT;
        }
        nSize = nCompressedSize;
        for (int nMipMap = 0; nMipMap < nMipMapCount - 1; ++nMipMap)
        {
            nMipWidth >>= 1;
            nMipHeight >>= 1;
            nSize += ((nMipWidth + CResTPC::TPC_COMPRESSED_BLOCK_DIMENSION - 1) / CResTPC::TPC_COMPRESSED_BLOCK_DIMENSION) * ((nMipHeight + CResTPC::TPC_COMPRESSED_BLOCK_DIMENSION - 1) / CResTPC::TPC_COMPRESSED_BLOCK_DIMENSION) * nBlockSize;
        }
        if (nFaces == CResTPC::TPC_CUBE_FACE_COUNT)
        {
            nSize *= CResTPC::TPC_CUBE_FACE_COUNT;
        }
    }
    else
    {
        nSize = nDataSize;
    }
    SProcessedTextureDataTrailer* pTrailer = (SProcessedTextureDataTrailer*)
        (m_pRes->GetTPCTXISize() + pData + nSize + sizeof(unsigned long));
    pTrailer->nType = RESTYPE_TPC;
    pTrailer->pOwner = this;
    return 1;
}
// PC 0x00782DC0 GOG 0x00782BD0
int CAuroraProcessedTexture::ReadProcessedTextureHeader(
    int* pnWidth, int* pnHeight, char* pnEncoding, int* pnCompressedSize,
    int* pnDataSize, float* pfAlphaBlending, char* pnMipMapCount)
{
    TPCHeader header;
    if (m_pRes == NULL)
    {
        return 0;
    }
    if (!m_pRes->ReadRaw(sizeof(header), (char*)&header))
    {
        return 0;
    }
    int nWidth = header.m_nWidth;
    *pnWidth = nWidth;
    int nHeight = header.m_nHeight;
    *pnHeight = nHeight;
    *pfAlphaBlending = header.m_fAlphaBlending;
    *pnMipMapCount = header.m_nMipMapCount;
    if (header.m_bFormat1)
    {
        *pnEncoding = 1;
    }
    else if (header.m_bFormat3)
    {
        *pnEncoding = 3;
    }
    else if (header.m_bFormat4)
    {
        *pnEncoding = 4;
    }

    if (header.m_nDataSize == 0)
    {
        *pnDataSize = 0;
        *pnCompressedSize = 0;
        for (int nMipMap = 0; nMipMap < header.m_nMipMapCount; ++nMipMap)
        {
            *pnDataSize += *pnEncoding * nHeight * nWidth;
            nWidth >>= 1;
            nHeight >>= 1;
        }
    }
    else
    {
        *pnDataSize = 0;
        *pnCompressedSize = header.m_nDataSize;
    }
    return 1;
}
// PC 0x007831B0 GOG 0x00782FC0
CAuroraProcessedTexture::CAuroraProcessedTexture(CResRef cResRef, int bRequest)
    : CResHelper<CResTPC, RESTYPE_TPC>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00783240 GOG 0x00783050
CAuroraProcessedTexture::~CAuroraProcessedTexture()
{
}
// PC 0x00783250 GOG 0x00783060
unsigned char* CAuroraProcessedTexture::GetPtrToResHelperInfo()
{
    if (m_pRes == NULL || m_pRes->GetDemands() < 1)
    {
        return 0;
    }
    unsigned char* nData = GetProcessedTextureDataPtr();
    return GetProcessedTextureTXISize() + GetTextureSize() + nData + 4;
}
// PC 0x00782EC0 GOG 0x00782CD0
int CAuroraProcessedTexture::GetTextureSize()
{
    int nWidth;
    int nHeight;
    char nEncoding;
    int nCompressedSize;
    int nDataSize;
    int nTXISize;
    float fAlphaBlending;
    char nMipMapCount;
    int nBlockSize;
    int nFaces = 1;
    int nSize;

    GetProcessedTextureAttrib(&nWidth, &nHeight, &nEncoding, &nCompressedSize,
                              &nDataSize, &nTXISize, &fAlphaBlending,
                              &nMipMapCount);
    if (nCompressedSize != 0)
    {
        if (nEncoding == 3)
        {
            nBlockSize = 8;
        }
        else if (nEncoding == 4)
        {
            nBlockSize = 16;
        }

        nSize = nCompressedSize;
        int nMipWidth = nWidth;
        int nMipHeight = nHeight;
        if (nMipHeight / nMipWidth == 6)
        {
            nFaces = 6;
            nMipHeight /= 6;
        }
        for (int nMipMap = 0; nMipMap < nMipMapCount - 1; ++nMipMap)
        {
            nMipWidth >>= 1;
            nMipHeight >>= 1;
            nSize += ((nMipWidth + 3) / 4) * ((nMipHeight + 3) / 4) * nBlockSize;
        }
        if (nFaces == 6)
        {
            nSize *= 6;
        }
    }
    else
    {
        nSize = nDataSize;
    }
    return nSize;
}
