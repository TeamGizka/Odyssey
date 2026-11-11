#include "exoresman.h"
#include "nwscriptinternal.h"

CResNSS::CResNSS()
{
    m_bServiced = 0;
    m_nNSSSize = 0;
    m_pNSSData = NULL;
}

CResNSS::~CResNSS()
{
}

char* CResNSS::GetNSSDataPtr()
{
    return (char*)m_pNSSData;
}

unsigned long CResNSS::GetNSSSize()
{
    return m_nNSSSize;
}

int CResNSS::OnResourceFreed()
{
    m_bServiced = 0;
    m_nNSSSize = 0;
    m_pNSSData = 0;
    return 1;
}

int CResNSS::OnResourceServiced()
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

    m_nNSSSize = m_nSize;
    m_pNSSData = pData;
    m_bServiced = 1;
    return 1;
}
