#include "exoresman.h"

#include <string.h>
// PC 0x0070C280  Xbox 0x001498E0
float CLIP::GetAnimLength()
{
    return m_AnimLength;
}
// PC 0x005A4C50
int CLIP::GetNumVisemes()
{
    return m_nEntryCount;
}
// PC 0x0070C290  Xbox 0x001498F0
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
// PC 0x0070C2C0  Xbox 0x00149920
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
// PC 0x0070C4D0  Xbox 0x00149B30
CLIP::CLIP()
{
    m_nEntryCount = 0;
    m_AnimLength = 0.0;
    m_pLipData = 0;
}
// PC 0x0070C520  Xbox 0x00149B80
CLIP::~CLIP()
{
    if (m_bLipLoaded != 0)
    {
        m_pRes->Release();
        m_bLipLoaded = 0;
    }

    m_pLipData = 0;
}
// PC 0x0070C590  Xbox 0x00149BF0
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
// PC 0x005DE450
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
