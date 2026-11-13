#include "exobase.h"
#include <windows.h>
#include <string.h>
// PC 0x00637E70 GOG 0x00637D60
CExoRandInternal::~CExoRandInternal()
{
    if (m_pBuffer != NULL)
    {
        delete m_pBuffer;
        m_pBuffer = NULL;
    }
}
// PC 0x00637E90 GOG 0x00637D80
unsigned long CExoRandInternal::Rand()
{

    unsigned long nState0 = m_anState[0];
    m_anState[0] =
        (((nState0 << m_anLeftShift[0]) ^ nState0) >> m_anRightShift[0]) ^
        ((nState0 & m_anMask[0]) << m_anFeedbackShift[0]);
    unsigned long nState1 = m_anState[1];
    m_anState[1] =
        (((nState1 << m_anLeftShift[1]) ^ nState1) >> m_anRightShift[1]) ^
        ((nState1 & m_anMask[1]) << m_anFeedbackShift[1]);
    unsigned long nState2 = m_anState[2];
    m_anState[2] =
        (((nState2 << m_anLeftShift[2]) ^ nState2) >> m_anRightShift[2]) ^
        ((nState2 & m_anMask[2]) << m_anFeedbackShift[2]);
    unsigned long nState3 = m_anState[3];
    m_anState[3] =
        (((nState3 << m_anLeftShift[3]) ^ nState3) >> m_anRightShift[3]) ^
        ((nState3 & m_anMask[3]) << m_anFeedbackShift[3]);
    return m_anState[0] ^ m_anState[1] ^ m_anState[2] ^ m_anState[3];
}
// PC 0x00637F30 GOG 0x00637E20
void CExoRandInternal::ReSeed(unsigned long nSeed0, unsigned long nSeed1,
                              unsigned long nSeed2, unsigned long nSeed3)
{
    static unsigned long x = 0xFFFFFFFF;

    m_nCurrentSeed = 1;
    m_anRightShift[0] = m_anWidth[0] - m_anFeedbackShift[0];
    m_anRightShift[1] = m_anWidth[1] - m_anFeedbackShift[1];
    m_anRightShift[2] = m_anWidth[2] - m_anFeedbackShift[2];
    m_anRightShift[3] = m_anWidth[3] - m_anFeedbackShift[3];
    m_anMask[0] = x << (32 - m_anWidth[0]);
    m_anMask[1] = x << (32 - m_anWidth[1]);
    m_anMask[2] = x << (32 - m_anWidth[2]);
    m_anMask[3] = x << (32 - m_anWidth[3]);
    if (nSeed0 > (1UL << m_anRightShift[0]))
        m_anState[0] = nSeed0;
    if (nSeed1 > (1UL << m_anRightShift[1]))
        m_anState[1] = nSeed1;
    if (nSeed2 > (1UL << m_anRightShift[2]))
        m_anState[2] = nSeed2;
    if (nSeed3 > (1UL << m_anRightShift[3]))
        m_anState[3] = nSeed3;

    Rand();
}
// PC 0x00638010 GOG 0x00637F00
void CExoRandInternal::Add(unsigned char* pData, int nLength)
{
    if (m_nBufferLength + nLength < 128)
    {
        memcpy(m_pBuffer + m_nBufferLength, pData, nLength);
        m_nBufferLength += nLength;
    }
    else
    {
        memcpy(m_pBuffer + m_nBufferLength, pData, 128 - m_nBufferLength);
        memcpy(m_pBuffer, pData + (128 - m_nBufferLength),
               m_nBufferLength + nLength - 128);
        m_nBufferLength = m_nBufferLength + nLength - 128;
    }
}
// PC 0x00638090 GOG 0x00637F80
CExoString CExoRandInternal::GetString(unsigned short nLength)
{
    unsigned short nBlocks = 128 / nLength;
    unsigned short nRemainder = 128 % nLength;
    unsigned char* pString = new unsigned char[nLength + 1];
    CExoString sString;

    memcpy(pString, m_pBuffer, nLength);
    if (nBlocks > 1)
    {
        int nOffset = nLength;
        int nBlocksLeft = (unsigned short)(nBlocks - 1);
        do
        {
            if (nLength > 0)
            {
                int nIndex = 0;
                unsigned int nBytes = nLength;
                do
                {
                    pString[nIndex] ^= m_pBuffer[nOffset + nIndex];
                    nIndex++;
                    nBytes--;
                } while (nBytes != 0);
            }
            nOffset += nLength;
            nBlocksLeft--;
        } while (nBlocksLeft != 0);
    }

    if (nRemainder > 0)
    {
        int nOffset = nBlocks * nLength;
        int nIndex = 0;
        unsigned int nBytes = nRemainder;
        do
        {
            pString[nIndex] ^= m_pBuffer[nOffset + nIndex];
            nIndex++;
            nBytes--;
        } while (nBytes != 0);
    }

    if (nLength > 0)
    {
        unsigned char* pCurrent = pString;
        unsigned int nBytes = nLength;
        do
        {
            if (*pCurrent == 0)
                *pCurrent = (unsigned char)(Rand() % 250 + 1);
            pCurrent++;
            nBytes--;
        } while (nBytes != 0);
    }

    pString[nLength] = 0;
    int nAddLength = nLength;
    if (m_nBufferLength + nAddLength < 128)
    {
        memcpy(m_pBuffer + m_nBufferLength, pString, nAddLength);
        m_nBufferLength += nAddLength;
    }
    else
    {
        memcpy(m_pBuffer + m_nBufferLength, pString, 128 - m_nBufferLength);
        memcpy(m_pBuffer, pString + (128 - m_nBufferLength),
               m_nBufferLength + nAddLength - 128);
        m_nBufferLength = m_nBufferLength + nAddLength - 128;
    }
    sString = (char*)pString;
    delete pString;
    return sString;
}
// PC 0x00638270 GOG 0x00638160
void CExoRandInternal::GenSeed()
{
    unsigned char aStackData[128];
#ifndef _XBOX
    SYSTEM_INFO sSystemInfo;
#endif
    LARGE_INTEGER nPerformanceCount;

    memcpy(m_pBuffer, aStackData, 128);
    if (QueryPerformanceCounter(&nPerformanceCount))
        Add((unsigned char*)&nPerformanceCount, sizeof(nPerformanceCount));
#ifndef _XBOX
    GetSystemInfo(&sSystemInfo);
    Add((unsigned char*)&sSystemInfo, sizeof(sSystemInfo));
#endif
}
// PC 0x006382D0 GOG 0x006381C0
CExoRandInternal::CExoRandInternal()
{
    CExoString sSeed;

    m_nCurrentSeed = 0;
    m_anState[0] = 0x1745D12D;
    m_anState[1] = 0x249248ED;
    m_anState[2] = 0x333332F9;
    m_anState[3] = 0x383EF9FA;
    m_anWidth[0] = 31;
    m_anWidth[1] = 29;
    m_anWidth[2] = 28;
    m_anWidth[3] = 25;
    m_anLeftShift[0] = 6;
    m_anLeftShift[1] = 2;
    m_anLeftShift[2] = 13;
    m_anLeftShift[3] = 3;
    m_anFeedbackShift[0] = 20;
    m_anFeedbackShift[1] = 12;
    m_anFeedbackShift[2] = 7;
    m_anFeedbackShift[3] = 15;
    m_nBufferLength = 0;
    m_pBuffer = new unsigned char[128];

    GenSeed();

    sSeed = GetString(16);
    unsigned long nSeed0 = *(unsigned long*)sSeed.SubString(0, 4).CStr();
    unsigned long nSeed1 = *(unsigned long*)sSeed.SubString(4, 4).CStr();
    unsigned long nSeed2 = *(unsigned long*)sSeed.SubString(8, 4).CStr();
    unsigned long nSeed3 = *(unsigned long*)sSeed.SubString(12, 4).CStr();
    ReSeed(nSeed0, nSeed1, nSeed2, nSeed3);

    LARGE_INTEGER nPerformanceCount;
    QueryPerformanceCounter(&nPerformanceCount);
    unsigned long nLow = nPerformanceCount.LowPart;
    long nHigh = nPerformanceCount.HighPart;
    m_aClockSamples[0].nLow = nLow;
    m_aClockSamples[0].nHigh = nHigh;
    m_aClockSamples[1].nLow = nLow;
    m_aClockSamples[1].nHigh = nHigh;
    m_aClockSamples[2].nLow = nLow;
    m_aClockSamples[2].nHigh = nHigh;
    m_aClockSamples[3].nLow = nLow;
    m_aClockSamples[3].nHigh = nHigh;
    m_nTickCount = GetTickCount();
}
