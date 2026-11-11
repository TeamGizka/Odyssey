#include "exoresman.h"
#ifndef _XBOX
#include "mss.h"
#endif
// PC 0x0062B4B0
CResWave::CResWave()
{
    m_bServiced = 0;
    m_bDataOwned = 0;
    m_bLegacyHeader = 0;
    m_nDataSize = 0;
#ifndef _XBOX
    m_nUnknown38 = 0;
#endif
}
// PC 0x0062B4E0
CResWave::~CResWave()
{
}

#ifdef _XBOX

int CResWave::OnResourceFreed()
{
    m_bServiced = 0;
    m_bDataOwned = 0;
    m_bLegacyHeader = 0;
    m_nDataSize = 0;
    return 1;
}

int CResWave::OnResourceServiced()
{
    m_bServiced = 1;
    return 1;
}

void* CResWave::GetData()
{
    return m_pResource;
}
#else
int CResWave::OnResourceFreed()  // PC 0x0062B4F0
{
    if (m_bDataOwned != 0)
    {
        AIL_mem_free_lock(m_pResource);
        m_pResource = 0;
    }

    m_bServiced = 0;
    m_bDataOwned = 0;
    m_bLegacyHeader = 0;
    m_nDataSize = 0;
    m_nUnknown38 = 0;
    return 1;
}
// PC 0x0062B530
int CResWave::OnResourceServiced()
{
    AILSOUNDINFO SoundInfo;
    void* pOutput;
    if (g_bModifiedSound)
    {
        CBMUFileHeader* pBMU = (CBMUFileHeader*) m_pResource;
        if (pBMU->m_nFileType == BMU_FILE_TYPE)
        {
            if (pBMU->m_nVersion == BMU_FILE_VERSION)
            {
                if (AIL_decompress_ASI((char*) m_pResource + sizeof(CBMUFileHeader),
                                       GetSize() - sizeof(CBMUFileHeader), ".mp3", &pOutput,
                                       &m_nDataSize, 0) != 0)
                {
        
                    m_bDataOwned = 1;
                    g_pExoResMan->FreeResourceData(this);
                    m_pResource = pOutput;
                    m_bServiced = 1;
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
        }
        else if (pBMU->m_nFileType == WAVE_RIFF_FILE_TYPE)
        {
            if (AIL_WAV_info(m_pResource, &SoundInfo) == 1)
            {
                if (SoundInfo.format == WAVE_FORMAT_IMA_ADPCM)
                {
                    if (AIL_decompress_ADPCM(&SoundInfo, &pOutput, &m_nDataSize) == 0)
                    {
                        return 0;
                    }
    
                    m_bDataOwned = 1;
                    g_pExoResMan->FreeResourceData(this);
                    m_pResource = pOutput;
                }
                m_bServiced = 1;
                return 1;
            }
            else
            {
                return 0;
            }
        }
        CLegacyWaveHeader* pLegacy = (CLegacyWaveHeader*) m_pResource;
        unsigned long* pOffsetData = (unsigned long*) (pLegacy + 1);
        if (*pOffsetData != WAVE_RIFF_FILE_TYPE)
        {
            return 0;
        }

        m_bLegacyHeader = 1;
        if (AIL_WAV_info(pOffsetData, &SoundInfo) == 1)
        {
            if (SoundInfo.format == WAVE_FORMAT_IMA_ADPCM)
            {
                if (AIL_decompress_ADPCM(&SoundInfo, &pOutput, &m_nDataSize) == 0)
                {
                    return 0;
                }

                m_bDataOwned = 1;
                g_pExoResMan->FreeResourceData(this);
                m_pResource = pOutput;
            }
        }
    }
    else
    {
        CBMUFileHeader* pBMU = (CBMUFileHeader*) m_pResource;
        if (pBMU->m_nFileType == BMU_FILE_TYPE && pBMU->m_nVersion == BMU_FILE_VERSION)
        {
            if (AIL_decompress_ASI((char*) m_pResource + sizeof(CBMUFileHeader),
                                   GetSize() - sizeof(CBMUFileHeader), ".mp3", &pOutput,
                                   &m_nDataSize, 0) != 0)
            {
                m_bDataOwned = 1;
                g_pExoResMan->FreeResourceData(this);
                m_pResource = pOutput;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (AIL_WAV_info(m_pResource, &SoundInfo) == 1)
            {
                if (SoundInfo.format == WAVE_FORMAT_IMA_ADPCM)
                {
                    if (AIL_decompress_ADPCM(&SoundInfo, &pOutput, &m_nDataSize) == 0)
                    {
                        return 0;
                    }
    
                    m_bDataOwned = 1;
                    g_pExoResMan->FreeResourceData(this);
                    m_pResource = pOutput;
                }
            }
            else
            {
                return 0;
            }
        }
    }

    m_bServiced = 1;
    return 1;
}
// PC 0x0062B660
void* CResWave::GetData()
{
    if (m_bDataOwned == 0 && m_bLegacyHeader == 1)
    {
        return (CLegacyWaveHeader*) m_pResource + 1;
    }

    return m_pResource;
}
#endif
