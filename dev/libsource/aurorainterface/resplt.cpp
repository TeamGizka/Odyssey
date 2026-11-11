#include "exoresman.h"
// PC 0x00710B50  Xbox 0x002FAD80
CResPLT::CResPLT()
{
    m_bDecoded = 0;
    m_bCompressed = 0;
    m_bServiced = 0;
    m_nPLTSize = 0;
    m_nDataSize = 0;
    m_pPLTData = 0;
    m_pPLTHeader = 0;
    m_bAllocSuffix = 1;
}
// PC 0x00710B80  Xbox 0x002FADB0
CResPLT::~CResPLT()
{
}
// PC 0x00710B90  Xbox 0x002FADC0
int CResPLT::GetPLTAttrib(unsigned long* pnA, unsigned long* pnB, unsigned long* pnC)
{
    if (m_pPLTHeader == 0)
    {
        return 0;
    }
    *pnA = m_pPLTHeader[4];
    *pnB = m_pPLTHeader[5];
    *pnC = m_pPLTHeader[2];
    return 1;
}
// PC 0x00710BD0  Xbox 0x002FAE00
int CResPLT::OnResourceFreed()
{
    if (m_bDecoded != 0 && m_pPLTData != 0)
    {
        delete m_pPLTData;
    }
    m_bDecoded = 0;
    m_bCompressed = 0;
    m_bServiced = 0;
    m_nPLTSize = 0;
    m_nDataSize = 0;
    m_pPLTData = 0;
    m_pPLTHeader = 0;
    return 1;
}
// PC 0x00710CC0  Xbox 0x002FAEF0
int CResPLT::OnResourceServiced()
{
    unsigned long* pHeader = (unsigned long*)m_pResource;
    if (pHeader == 0)
    {
        return 0;
    }
    if (m_bServiced == 0)
    {
        if (m_pPLTHeader != 0)
        {
            return 0;
        }
        if (m_pPLTData != 0)
        {
            return 0;
        }
        m_pPLTHeader = pHeader;
        if (pHeader[0] != PLT_FILE_SIGNATURE)
        {
            return 0;
        }
        int bResult;
        if (pHeader[3] != 0)
        {
            m_bCompressed = 1;
            m_nDataSize = m_nSize - 24;
            m_pPLTData = (unsigned char*)(pHeader + 6);
            bResult = ReadRLEPLT();
        }
        else
        {
            m_bCompressed = 0;
            m_bDecoded = 0;
            m_nPLTSize = m_nSize - 24;
            m_nDataSize = m_nSize - 24;
            m_pPLTData = (unsigned char*)(pHeader + 6);
            bResult = 1;
        }
        if (bResult == 0)
        {
            OnResourceFreed();
            return 0;
        }
        m_bServiced = 1;
    }
    return 1;
}
// PC 0x00710C10  Xbox 0x002FAE40
int CResPLT::ReadRLEPLT()
{
    unsigned char* pSource = m_pPLTData;
    m_pPLTData = (unsigned char*)operator new(m_nPLTSize);
    if (m_pPLTData == 0)
    {
        return 0;
    }
    unsigned long nSource = 0;
    unsigned long nDest = 0;
    while (nDest < m_nPLTSize)
    {
        unsigned long nEnd = (pSource[nSource] & 0x7F) + nDest + 1;
        if (pSource[nSource++] & 0x80)
        {
            unsigned char nValue = pSource[nSource];
            ++nSource;
            for (; nDest < nEnd; ++nDest)
            {
                m_pPLTData[nDest] = nValue;
            }
        }
        else
        {
            for (; nDest < nEnd; ++nSource)
            {
                m_pPLTData[nDest] = pSource[nSource];
                ++nDest;
            }
        }
    }
    m_bDecoded = 1;
    return 1;
}
// PC 0x00712F90
unsigned char* CResPLT::GetPLTDataPtr()
{
    return m_pPLTData;
}
