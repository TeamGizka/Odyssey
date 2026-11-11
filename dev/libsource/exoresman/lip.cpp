#include "exoresman.h"

#include <string.h>
// PC 0x0077FA20
float CLIP::GetAnimLength()
{
    return m_AnimLength;
}
// PC 0x005ED200
int CLIP::GetNumVisemes()
{
    return m_nEntryCount;
}
// PC 0x0077FA30
int CLIP::GetVisemeIndex(unsigned long nIndex)
{
    if (nIndex < m_nEntryCount)
    {
        unsigned long nOffset = nIndex * 5;
        nIndex = 0;
        memcpy(&nIndex, &m_pLipData[nOffset + 4], 1);
        return nIndex;
    }

    return 0;
}
// PC 0x0077FA60
float CLIP::GetTimeIndex(unsigned long nIndex)
{
    float fTime;

    if (nIndex < m_nEntryCount)
    {
        memcpy(&fTime, &m_pLipData[nIndex * 5], sizeof(float));
        return fTime;
    }

    return 0.0f;
}
// PC 0x0077FC70
CLIP::CLIP()
{
    m_nEntryCount = 0;
    m_AnimLength = 0.0;
    m_pLipData = 0;
}
// PC 0x0077FCC0
CLIP::~CLIP()
{
    if (m_bLipLoaded != 0)
    {
        m_pRes->Release();
        m_bLipLoaded = 0;
    }

    m_pLipData = 0;
}
// PC 0x0077FD30
int CLIP::LoadLip(CExoString sLip)
{
    CResRef resRef(sLip);

    SetResRef(resRef, 0);
    if (m_pRes->Demand() == 0)
    {
        return 0;
    }

    m_pLipData = m_pRes->GetLIPDataPtr();
    if (strncmp("LIP V1.0", m_pLipData, 8) != 0)
    {
        return 0;
    }

    m_pLipData += 8;
    m_AnimLength = ((LIPHeader*)(m_pLipData - 8))->m_fAnimLength;
    m_pLipData += 4;
    m_nEntryCount = ((LIPHeader*)(m_pLipData - 12))->m_nEntryCount;
    m_pLipData += 4;
    m_bLipLoaded = 1;
    return 1;
}
// PC 0x0077FA00
int CLIP::UnloadLip()
{
    int nResult = m_bLipLoaded;
    if (nResult != 0)
    {
        nResult = m_pRes->Release();
        m_bLipLoaded = 0;
    }
    return nResult;
}
