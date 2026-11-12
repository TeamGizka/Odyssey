#include "exobase.h"
#include "exolocstringinternal.h"
#include "exotlktable.h"
#include "clientexoappinternal.h"

static inline void ExolocstringTrace1();
#define EXOLOCSTRING_TRACE() ExolocstringTrace1()

const float CExoLocString::TIME_PER_CHARACTER_SPOKEN = 0.11f;
// PC 0x006316D0
CExoLocString::CExoLocString()
{
    m_pExoLocStringInternal = new CExoLocStringInternal();
    m_dwStringRef = -1;
}
// PC 0x00631730
CExoLocString& CExoLocString::operator=(const CExoLocString& lsString)
{
    if (this != &lsString)
    {
        m_pExoLocStringInternal->Assign(lsString.m_pExoLocStringInternal);
        m_dwStringRef = lsString.m_dwStringRef;
    }
    return *this;
}
// PC 0x00631760
void CExoLocString::ClearLocString()
{
    m_pExoLocStringInternal->ClearLocString();
}
// PC 0x00631770
int CExoLocString::GetString(unsigned long nIndex, int* pnID,
                             CExoString* psString,
                             unsigned char* pnGender) const
{
    int bFound = m_pExoLocStringInternal->GetString(nIndex, pnID, psString);
    *pnGender = *pnID % 2;
    *pnID /= 2;
    return bFound;
}
// PC 0x006317B0
int CExoLocString::GetStringCount() const
{
    return m_pExoLocStringInternal->m_nCount;
}
// PC 0x006317C0
int CExoLocString::GetStringLength(unsigned long nIndex) const
{
    return m_pExoLocStringInternal->GetStringLength(nIndex);
}
// PC 0x006317D0
int CExoLocString::Compare(const CExoLocString& lsString) const
{
    EXOLOCSTRING_TRACE();
    if (m_pExoLocStringInternal->m_bFetched &&
        lsString.m_pExoLocStringInternal->m_bFetched)
    {
        return m_dwStringRef == lsString.m_dwStringRef;
    }
    if (m_pExoLocStringInternal->m_bFetched)
    {
        if (lsString.m_pExoLocStringInternal->m_nCount == 0)
        {
            return m_dwStringRef == lsString.m_dwStringRef;
        }
        return 0;
    }
    if (lsString.m_pExoLocStringInternal->m_bFetched)
    {
        if (m_pExoLocStringInternal->m_nCount == 0)
        {
            return m_dwStringRef == lsString.m_dwStringRef;
        }
        return 0;
    }
    return m_pExoLocStringInternal->Compare(lsString) != 0 &&
           m_dwStringRef == lsString.m_dwStringRef;
}
// PC 0x00631840
CExoLocString::CExoLocString(const CExoLocString& lsString)
{
    EXOLOCSTRING_TRACE();
    m_pExoLocStringInternal = new CExoLocStringInternal();
    if (this != &lsString)
    {
        m_pExoLocStringInternal->Assign(lsString.m_pExoLocStringInternal);
        m_dwStringRef = lsString.m_dwStringRef;
    }
}
// PC 0x006318C0
CExoLocString::~CExoLocString()
{
    if (m_pExoLocStringInternal != 0)
    {
        delete m_pExoLocStringInternal;
    }
}
// PC 0x006318E0
int CExoLocString::operator==(const CExoLocString& lsString) const
{
    return Compare(lsString);
}
// PC 0x006318F0
int CExoLocString::operator!=(const CExoLocString& lsString) const
{
    return Compare(lsString) == 0;
}
// PC 0x00631910
int CExoLocString::GetString(int nID, CExoString* psString,
                             unsigned char nGender) const
{
    nGender = (nID == 0) ? 0 : nGender;
    int nStringID = nGender + 2 * nID;
    if (GetStringInternal(nStringID, psString) != 0)
    {
        return 1;
    }

    unsigned char bFound;
    STR_RES strRes;
    if (g_pTlkTable != 0 &&
        g_pTlkTable->Fetch(m_dwStringRef, strRes, nGender) != 0)
    {
        *psString = strRes.m_sString;
        bFound = 1;
    }
    else
    {
        bFound = 0;
    }
    return bFound;
}
// PC 0x006319F0
int CExoLocString::GetString(int nID, CExoString* psString,
                             unsigned char nGender)
{
    nGender = (nID == 0) ? 0 : nGender;
    if (GetStringInternal(nGender + 2 * nID, psString) != 0)
    {
        return 1;
    }

    unsigned char bFound;
    STR_RES strRes;
    if (g_pTlkTable != 0 &&
        g_pTlkTable->Fetch(m_dwStringRef, strRes, nGender) != 0)
    {
        *psString = strRes.m_sString;
        if (*psString != "")
        {
            m_pExoLocStringInternal->AddString(nGender + 2 * nID, *psString);
            CExoLocStringInternal* pInternal = m_pExoLocStringInternal;
            pInternal->m_bFetched = 1;
        }
        bFound = 1;
    }
    else
    {
        bFound = 0;
    }
    return bFound;
}
// PC 0x00631B00
int CExoLocString::GetStringLoc(int nID, CExoString* psString,
                                unsigned char nGender) const
{
    nGender = (nID == 0) ? 0 : nGender;
    return GetStringInternal(nGender + 2 * nID, psString);
}

int CExoLocString::GetStringInternal(int nStringID, CExoString* psString) const
{
    return m_pExoLocStringInternal->GetString(nStringID, psString);
}
// PC 0x00631B30
float CExoLocString::GetSpokenTimeEstimate(int nID, unsigned char nGender) const
{
    CExoString sString;
    int nStringID;

    nGender = (nID == 0) ? 0 : nGender;
    nStringID = nGender + 2 * nID;
    GetString(nStringID, &sString, 0);
    int nTokenStart = sString.Find('<', 0);
    int nCustomStart = sString.Find('{', 0);
    if (nTokenStart != -1 || nCustomStart != -1)
    {
        g_pTlkTable->ParseStr(sString);
    }
    while (sString.GetLength() > 0 && sString[0] == ' ')
    {
        sString = sString.SubString(1, sString.GetLength() - 1);
    }
    while (sString.GetLength() > 0 &&
           sString[sString.GetLength() - 1] == ' ')
    {
        sString = sString.SubString(0, sString.GetLength() - 1);
    }
    if (sString.GetLength() != 0)
    {
        return sString.GetLength() * TIME_PER_CHARACTER_SPOKEN + 1.0f;
    }
    return 0.0f;
}

void CExoLocString::RemoveString(int nID, unsigned char nGender)
{
    nGender = (nID == 0) ? 0 : nGender;
    int nStringID = nGender + 2 * nID;
    m_pExoLocStringInternal->RemoveString(nStringID);
}
// PC 0x00631D50
int CExoLocString::AddString(int nID, const CExoString& sString,
                             unsigned char nGender)
{
    nGender = (nID == 0) ? 0 : nGender;
    int nStringID = 2 * nID + nGender;
    RemoveString(nID, nGender);

    CExoString sExisting;
    GetString(nID, &sExisting, nGender);
    if (sExisting == sString)
    {
        return 1;
    }
    return m_pExoLocStringInternal->AddString(nStringID, sString);
}

static inline void ExolocstringTrace1()
{
}
