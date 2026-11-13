#include "exobase.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

char g_szEmptyString[] = "";

char* CExoStringFormatBuffer = NULL;
int CExoStringFormatBufferSize = 0;
// PC 0x005FF150 GOG 0x005FF130
CExoString::CExoString()
{
    m_sString = NULL;
    m_nBufferLength = 0;
}
// PC 0x00630390 GOG 0x00630280
BOOL CExoString::operator==(const CExoString& sString) const
{
    if (m_sString != NULL && sString.m_sString != NULL)
    {
        return strcmp(m_sString, sString.m_sString) == 0;
    }

    if (m_sString == NULL && sString.m_sString == NULL)
    {
        return TRUE;
    }
    if (m_sString != NULL && m_sString[0] == 0)
    {
        return TRUE;
    }
    if (sString.m_sString != NULL && sString.m_sString[0] == 0)
    {
        return TRUE;
    }

    return FALSE;
}
// PC 0x00630420 GOG 0x00630310
BOOL CExoString::operator==(const char* szString) const
{
    if (m_sString != NULL && szString != NULL)
    {
        return strcmp(m_sString, szString) == 0;
    }

    if (m_sString == NULL && szString == NULL)
    {
        return TRUE;
    }
    if (m_sString != NULL && m_sString[0] == 0)
    {
        return TRUE;
    }
    if (szString != NULL && szString[0] == 0)
    {
        return TRUE;
    }

    return FALSE;
}
// PC 0x006304A0 GOG 0x00630390
BOOL CExoString::operator!=(const CExoString& sString) const
{
    if (m_sString != NULL && sString.m_sString != NULL)
    {
        return strcmp(m_sString, sString.m_sString) != 0;
    }

    if (m_sString != NULL && m_sString[0] != 0)
    {
        return TRUE;
    }
    if (sString.m_sString != NULL && sString.m_sString[0] != 0)
    {
        return TRUE;
    }

    return FALSE;
}
// PC 0x00630520 GOG 0x00630410
BOOL CExoString::operator!=(const char* szString) const
{
    if (m_sString != NULL)
    {
        if (szString != NULL)
        {
            return strcmp(m_sString, szString) != 0;
        }
        if (m_sString[0] == 0)
        {
            return FALSE;
        }
    }
    else
    {
        if (szString == NULL || szString[0] == 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}
// PC 0x006305A0 GOG 0x00630490
BOOL CExoString::operator<(const CExoString& sString) const
{
    if (m_sString != NULL && sString.m_sString != NULL)
    {
        return strcmp(m_sString, sString.m_sString) < 0;
    }

    if (sString.m_sString != NULL && sString.m_sString[0] != 0)
    {
        return TRUE;
    }

    return FALSE;
}
// PC 0x00630620 GOG 0x00630510
BOOL CExoString::operator>(const CExoString& sString) const
{
    if (m_sString != NULL && sString.m_sString != NULL)
    {
        return strcmp(m_sString, sString.m_sString) > 0;
    }

    if (m_sString != NULL && m_sString[0] != 0)
    {
        return TRUE;
    }

    return FALSE;
}
// PC 0x00630690 GOG 0x00630580
BOOL CExoString::operator<=(const CExoString& sString) const
{
    if (m_sString != NULL && sString.m_sString != NULL)
    {
        return strcmp(m_sString, sString.m_sString) <= 0;
    }

    if ((m_sString != NULL || sString.m_sString != NULL) &&
        (m_sString == NULL || m_sString[0] != 0) && sString.m_sString == NULL)
    {
        return FALSE;
    }

    return TRUE;
}
// PC 0x00630710 GOG 0x00630600
BOOL CExoString::operator>=(const CExoString& sString) const
{
    if (m_sString != NULL && sString.m_sString != NULL)
    {
        return strcmp(m_sString, sString.m_sString) >= 0;
    }

    if ((m_sString != NULL || sString.m_sString != NULL) &&
        (sString.m_sString == NULL || sString.m_sString[0] != 0) &&
        m_sString == NULL)
    {
        return FALSE;
    }

    return TRUE;
}
// PC 0x00630790 GOG 0x00630680
char CExoString::operator[](int nIndex) const
{
    if (nIndex >= 0 && m_sString != NULL && strlen(m_sString) > nIndex)
    {
        return m_sString[nIndex];
    }

    return 0;
}
// PC 0x006307C0 GOG 0x006306B0
char* CExoString::CStr() const
{
    if (m_sString == NULL)
    {
        return g_szEmptyString;
    }

    return m_sString;
}
// PC 0x006307D0 GOG 0x006306C0
void CExoString::Format(char* szFormat, ...)
{
    va_list args;

    if (CExoStringFormatBuffer == NULL && CExoStringFormatBufferSize == 0)
    {
        CExoStringFormatBufferSize = 1024;
        CExoStringFormatBuffer = new char[CExoStringFormatBufferSize];
    }

    va_start(args, szFormat);
    int nLength = _vsnprintf(CExoStringFormatBuffer, CExoStringFormatBufferSize,
                             szFormat, args);
    if (nLength < -1)
    {
        return;
    }

    while (nLength < 0)
    {
        CExoStringFormatBufferSize += 1024;
        if (CExoStringFormatBuffer != NULL)
        {
            delete[] CExoStringFormatBuffer;
        }
        CExoStringFormatBuffer = new char[CExoStringFormatBufferSize];
        nLength = _vsnprintf(CExoStringFormatBuffer, CExoStringFormatBufferSize,
                             szFormat, args);
    }

    if (m_sString != NULL && nLength + 1 > m_nBufferLength)
    {
        m_nBufferLength = 0;
        delete[] m_sString;
        m_sString = NULL;
    }
    if (m_sString == NULL)
    {
        m_nBufferLength = nLength + 1;
        m_sString = new char[m_nBufferLength];
    }

    strncpy(m_sString, CExoStringFormatBuffer, nLength);
    m_sString[nLength] = 0;
}
// PC 0x006308E0 GOG 0x006307D0
void CExoString::Insert(const CExoString& sString, int nPosition)
{
    if (sString.m_sString == NULL)
    {
        return;
    }

    int nLength = strlen(sString.m_sString);
    unsigned int nMyLength = GetLength();

    if (nLength != 0 && nPosition >= 0 && nMyLength > nPosition)
    {
        char* szNew = new char[nMyLength + nLength + 1];
        szNew[0] = 0;

        char* szOld = m_sString;
        m_sString = szNew;
        if (szOld != NULL)
        {
            strncpy(m_sString, szOld, nPosition);
        }
        m_sString[nPosition] = 0;
        strcat(m_sString, sString.m_sString);
        strcat(m_sString, szOld + nPosition);

        if (szOld != NULL)
        {
            delete[] szOld;
        }
    }
}
// PC 0x006309D0 GOG 0x006308C0
BOOL CExoString::CompareNoCase(const char* szString) const
{
    const char* sThisString = m_sString;

    if (szString == NULL && sThisString == NULL)
    {
        return TRUE;
    }
    if (szString == NULL || sThisString == NULL)
    {
        return FALSE;
    }

    return stricmp(szString, sThisString) == 0;
}
// PC 0x00630A10 GOG 0x00630900
BOOL CExoString::ComparePrefixNoCase(const CExoString& sString, int nSize) const
{
    const char* szString = sString.CStr();
    const char* sThisString = m_sString;

    if (szString == NULL && sThisString == NULL)
    {
        return TRUE;
    }
    if (szString == NULL || sThisString == NULL)
    {
        return FALSE;
    }

    return strnicmp(szString, sThisString, nSize) == 0;
}
// PC 0x00630A60 GOG 0x00630950
BOOL CExoString::StripNonAlphaNumeric(BOOL bLowerCase, BOOL bAllowEmail,
                                      BOOL bAllowPunctuation)
{
    char szValid[128];
    BOOL bChanged = FALSE;

    if (m_sString == NULL)
    {
        return FALSE;
    }

    unsigned int nLength = strlen(m_sString);
    if (nLength == 0)
    {
        return FALSE;
    }

    if (bLowerCase)
    {
        strcpy(szValid, "abcdefghijklmnopqrstuvwxyz0123456789'"
                        "\xdf\xe9\xc9\xe8\xc8\xc7\xe7\xf9\xd9\xf1\xd1\xe0\xc0"
                        "\xc4\xe4\xdc\xfc\xd6\xf6_-");
    }
    else
    {
        strcpy(szValid, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz0123456789");
    }
    if (bAllowEmail)
    {
        strcat(szValid, "@.");
    }
    if (bAllowPunctuation)
    {
        strcat(szValid, ".',"
                        "\xdf\xe9\xc9\xe8\xc8\xc7\xe7\xf9\xd9\xf1\xd1\xe0\xc0"
                        "\xc4\xe4\xdc\xfc\xd6\xf6_- ");
    }

    for (unsigned int i = 0; i < nLength; i++)
    {
        if (bLowerCase)
        {
            if (m_sString[i] >= 'A' && m_sString[i] <= 'Z')
            {
                m_sString[i] += 32;
            }
        }

        if (strchr(szValid, m_sString[i]) == NULL)
        {
            for (unsigned int j = i; j < nLength; j++)
            {
                m_sString[j] = m_sString[j + 1];
            }
            i--;
            nLength--;
            bChanged = TRUE;
        }
    }

    return bChanged;
}
// PC 0x00630BA0 GOG 0x00630A90
CExoString::CExoString(const char* szString)
{
    if (szString != NULL && strlen(szString) != 0)
    {
        m_nBufferLength = strlen(szString) + 1;
        m_sString = new char[m_nBufferLength];
        strcpy(m_sString, szString);
    }
    else
    {
        m_sString = NULL;
        m_nBufferLength = 0;
    }
}
// PC 0x00630C10 GOG 0x00630B00
CExoString::CExoString(const CExoString& sString)
{
    if (sString.m_sString != NULL && strlen(sString.m_sString) != 0)
    {
        m_nBufferLength = strlen(sString.m_sString) + 1;
        m_sString = new char[m_nBufferLength];
        strcpy(m_sString, sString.m_sString);
    }
    else
    {
        m_sString = NULL;
        m_nBufferLength = 0;
    }
}
// PC 0x00630C80 GOG 0x00630B70
CExoString::CExoString(const char* szString, int nLength)
{
    if (nLength > 0)
    {
        m_nBufferLength = nLength + 1;
        m_sString = new char[m_nBufferLength];
        strncpy(m_sString, szString, nLength);
        m_sString[nLength] = 0;
    }
    else
    {
        m_sString = NULL;
        m_nBufferLength = 0;
    }
}
// PC 0x00630CD0 GOG 0x00630BC0
CExoString::CExoString(int nValue)
{
    char szBuffer[36];

    sprintf(szBuffer, "%i", nValue);
    m_nBufferLength = strlen(szBuffer) + 1;
    m_sString = new char[m_nBufferLength];
    strcpy(m_sString, szBuffer);
}
// PC 0x00630D30 GOG 0x00630C20
CExoString::~CExoString()
{
    if (m_sString != NULL)
    {
        m_nBufferLength = 0;
        delete[] m_sString;
        m_sString = NULL;
    }
}
// PC 0x00630D60 GOG 0x00630C50
CExoString& CExoString::operator=(const CExoString& sString)
{
    if (this == &sString)
    {
        return *this;
    }

    if (m_sString != NULL && (sString.m_sString == NULL ||
                              strlen(sString.m_sString) + 1 > m_nBufferLength))
    {
        m_nBufferLength = 0;
        delete[] m_sString;
        m_sString = NULL;
    }

    int nLength = 0;
    if (sString.m_sString != NULL)
    {
        nLength = strlen(sString.m_sString);
    }

    if (nLength > 0)
    {
        if (m_sString == NULL)
        {
            m_nBufferLength = nLength + 1;
            m_sString = new char[m_nBufferLength];
        }
        strcpy(m_sString, sString.m_sString);
    }
    else
    {
        if (m_sString != NULL)
        {
            delete[] m_sString;
            m_sString = NULL;
            m_nBufferLength = 0;
        }
    }

    return *this;
}

// PC 0x00630E20 GOG 0x00630D10
CExoString& CExoString::operator=(const char* szString)
{
    if (m_sString != NULL &&
        (szString == NULL || strlen(szString) + 1 > m_nBufferLength))
    {
        m_nBufferLength = 0;
        delete[] m_sString;
        m_sString = NULL;
    }

    int nLength = 0;
    if (szString != NULL)
    {
        nLength = strlen(szString);
    }

    if (nLength > 0)
    {
        if (m_sString == NULL)
        {
            m_nBufferLength = strlen(szString) + 1;
            m_sString = new char[m_nBufferLength];
        }
        strcpy(m_sString, szString);
    }
    else
    {
        if (m_sString != NULL)
        {
            delete[] m_sString;
            m_sString = NULL;
            m_nBufferLength = 0;
        }
    }

    return *this;
}
// PC 0x00630EE0 GOG 0x00630DD0
CExoString CExoString::operator+(const CExoString& sString) const
{
    CExoString sNew;
    int nOtherLength = sString.GetLength();
    int nLength = GetLength();

    if (nOtherLength == 0)
    {
        if (nLength == 0)
        {
            return sNew;
        }
        sNew.m_nBufferLength = nLength + 1;
        sNew.m_sString = new char[sNew.m_nBufferLength];
        strcpy(sNew.m_sString, m_sString);
        return sNew;
    }

    if (nLength == 0)
    {
        sNew.m_nBufferLength = nOtherLength + 1;
        sNew.m_sString = new char[sNew.m_nBufferLength];
        strcpy(sNew.m_sString, sString.m_sString);
        return sNew;
    }

    sNew.m_nBufferLength = nLength + nOtherLength + 1;
    sNew.m_sString = new char[sNew.m_nBufferLength];
    strcpy(sNew.m_sString, m_sString);
    strcat(sNew.m_sString, sString.m_sString);

    return sNew;
}
// PC 0x00631080 GOG 0x00630F70
int CExoString::Find(const CExoString& sString, int nStart) const
{
    if (m_sString == NULL || sString.m_sString == NULL || nStart < 0)
    {
        return -1;
    }

    const char* szPos = m_sString;
    for (int i = 0; i < nStart; i++)
    {
        if (*szPos == 0)
        {
            return -1;
        }
        szPos++;
    }

    int j = 0;
    while (szPos[j] != 0)
    {
        if (sString.m_sString[j] == 0)
        {
            return szPos - m_sString;
        }
        if (szPos[j] != sString.m_sString[j])
        {
            szPos++;
            j = 0;
        }
        else
        {
            j++;
        }
    }

    if (sString.m_sString[j] == 0)
    {
        return szPos - m_sString;
    }

    return -1;
}
// PC 0x006310F0 GOG 0x00630FE0
int CExoString::Find(char cChar, int nStart) const
{
    const char* szPos = m_sString;
    int nPosition = nStart;

    if (szPos == NULL || nStart < 0)
    {
        return -1;
    }

    for (int i = 0; i < nStart; i++)
    {
        if (*szPos == 0)
        {
            return -1;
        }
        szPos++;
    }

    while (*szPos != 0)
    {
        if (*szPos == cChar)
        {
            return nPosition;
        }
        nPosition++;
        szPos++;
    }

    return -1;
}
// PC 0x00631140 GOG 0x00631030
CExoString CExoString::Left(int nCount) const
{
    CExoString sNew;

    if (m_sString == NULL)
    {
        return sNew;
    }

    int nLength = strlen(m_sString);
    if (nCount < 0)
    {
        return sNew;
    }
    if (nLength == 0)
    {
        return sNew;
    }

    unsigned int nCopyLength = nCount;
    if (nCopyLength > (unsigned int)nLength)
    {
        nCopyLength = nLength;
    }

    sNew.m_nBufferLength = nCopyLength + 1;
    sNew.m_sString = new char[nCopyLength + 1];
    strncpy(sNew.m_sString, m_sString, nCopyLength);
    sNew.m_sString[nCopyLength] = 0;

    return sNew;
}
// PC 0x00631250 GOG 0x00631140
CExoString CExoString::LowerCase() const
{
    CExoString sNew;

    if (m_sString == NULL)
    {
        return sNew;
    }

    sNew.m_nBufferLength = strlen(m_sString) + 1;
    sNew.m_sString = new char[sNew.m_nBufferLength];

    int i = 0;
    while (m_sString[i] != 0)
    {
        if (m_sString[i] >= 'A' && m_sString[i] <= 'Z')
        {
            sNew.m_sString[i] = m_sString[i] + 32;
        }
        else
        {
            sNew.m_sString[i] = m_sString[i];
        }
        i++;
    }
    sNew.m_sString[i] = 0;

    return sNew;
}
// PC 0x00631340 GOG 0x00631230
CExoString CExoString::Right(int nCount) const
{
    CExoString sNew;

    if (m_sString == NULL)
    {
        return sNew;
    }

    int nLength = strlen(m_sString);
    if (nCount < 0)
    {
        return sNew;
    }
    if (nLength == 0)
    {
        return sNew;
    }

    unsigned int nCopyLength = nCount;
    if (nCopyLength > (unsigned int)nLength)
    {
        nCopyLength = nLength;
    }

    sNew.m_nBufferLength = nCopyLength + 1;
    sNew.m_sString = new char[nCopyLength + 1];
    strncpy(sNew.m_sString, m_sString + (nLength - nCopyLength), nCopyLength);
    sNew.m_sString[nCopyLength] = 0;

    return sNew;
}
// PC 0x00631440 GOG 0x00631330
CExoString CExoString::SubString(int nStart, int nCount) const
{
    CExoString sNew;

    if (m_sString == NULL)
    {
        return sNew;
    }

    unsigned int nLength = strlen(m_sString);
    if (nStart < 0 || nStart >= nLength || nCount == 0)
    {
        return sNew;
    }
    if (nCount < 0)
    {
        nCount = nLength - nStart;
    }

    sNew.m_nBufferLength = nCount + 1;
    sNew.m_sString = new char[sNew.m_nBufferLength];
    strncpy(sNew.m_sString, m_sString + nStart, nCount);
    sNew.m_sString[nCount] = 0;

    return sNew;
}
// PC 0x00631530 GOG 0x00631420
CExoString CExoString::UpperCase() const
{
    CExoString sNew;

    if (m_sString == NULL)
    {
        return sNew;
    }

    sNew.m_nBufferLength = strlen(m_sString) + 1;
    sNew.m_sString = new char[sNew.m_nBufferLength];

    int i = 0;
    while (m_sString[i] != 0)
    {
        if (m_sString[i] >= 'a' && m_sString[i] <= 'z')
        {
            sNew.m_sString[i] = m_sString[i] - 32;
        }
        else
        {
            sNew.m_sString[i] = m_sString[i];
        }
        i++;
    }
    sNew.m_sString[i] = 0;

    return sNew;
}
// PC 0x00631620 GOG 0x00631510
BOOL CExoString::CompareNoCase(const CExoString& sString) const
{
    int nOtherLength = sString.GetLength();
    const char* sThisString = m_sString;

    if (sString.m_sString == NULL && sThisString == NULL)
    {
        return TRUE;
    }
    if (sString.m_sString == NULL || sThisString == NULL)
    {
        return FALSE;
    }
    if (nOtherLength != strlen(sThisString))
    {
        return FALSE;
    }

    const char* sOtherString = sString.m_sString;
    for (int i = 0; i < nOtherLength; i++)
    {
        if (sOtherString[i] != sThisString[i])
        {
            if (sOtherString[i] < 'A' || sOtherString[i] > 'Z')
            {
                if (sThisString[i] < 'A' || sThisString[i] > 'Z')
                {
                    return FALSE;
                }
                if (sThisString[i] + 32 != sOtherString[i])
                {
                    return FALSE;
                }
            }
            else
            {
                if (sOtherString[i] + 32 != sThisString[i])
                {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}
