#include "exoresman.h"
#include "nwscriptinternal.h"

// Dead code: not in the PC executable
CResNSS::CResNSS()
{
    m_bServiced = 0;
    m_nNSSSize = 0;
    m_pNSSData = NULL;
}

// Dead code: not in the PC executable
CResNSS::~CResNSS()
{
}

// Dead code: not in the PC executable
char* CResNSS::GetNSSDataPtr()
{
    return (char*)m_pNSSData;
}

// Dead code: not in the PC executable
unsigned long CResNSS::GetNSSSize()
{
    return m_nNSSSize;
}

// Dead code: not in the PC executable
int CResNSS::IsLoaded()
{
    return m_bServiced;
}

// Dead code: not in the PC executable
int CResNSS::OnResourceFreed()  // Xbox 0x00331990
{
    m_bServiced = 0;
    m_nNSSSize = 0;
    m_pNSSData = 0;
    return 1;
}

// Dead code: not in the PC executable
int CResNSS::OnResourceServiced()  // Xbox 0x00161470
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
