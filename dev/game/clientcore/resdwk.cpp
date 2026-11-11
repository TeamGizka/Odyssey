#include "exoresman.h"

CResDWK::CResDWK()
{
    m_bServiced = 0;
    m_nDWKSize = 0;
    m_pDWKData = 0;
    m_bAllocPrefix = 1;
}

CResDWK::~CResDWK()
{
}

unsigned char* CResDWK::GetDWKDataPtr()
{
    return m_pDWKData;
}

unsigned long CResDWK::GetDWKSize()
{
    return m_nDWKSize;
}

int CResDWK::OnResourceFreed()
{
    m_bServiced = 0;
    m_nDWKSize = 0;
    m_pDWKData = 0;
    return 1;
}

int CResDWK::OnResourceServiced()
{
    unsigned char* pData;

    pData = (unsigned char*)m_pResource;
    if (pData == NULL)
    {
        return 0;
    }

    if (m_bServiced != 0)
    {
        return 1;
    }

    m_nDWKSize = m_nSize;
    m_pDWKData = pData;
    m_bServiced = 1;
    return 1;
}
