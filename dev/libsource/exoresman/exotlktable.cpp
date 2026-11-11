#include "exotlktable.h"
#include "exoresman.h"
#include "twodimarray.h"
#include "resgff.h"

#include <string.h>

const unsigned long CTlkFile::TLK_FILE_TYPE = 0x204B4C54;
const char* CTlkFile::TLK_FILE_VERSION = "V3.0";

const unsigned long CTlkTable::STRING_ID_MASK = 0xffffffUL;
const unsigned char CTlkTable::INIT_FILESARRAY_SLOTS = 8;
const unsigned char CTlkTable::FILESARRAY_GROW_SIZE = 8;
const unsigned short CTlkTable::TEXT_PRESENT = 1;
const unsigned short CTlkTable::SND_PRESENT = 2;
const unsigned short CTlkTable::SNDLENGTH_PRESENT = 4;
const unsigned short CTlkTable::STRING_NOT_IN_PROJECT = 32768;
const int CTlkTable::CUSTOMTOKEN_RESERVED_END = 9;

const unsigned char CTlkTable::GENDER_MALE = 0;
const unsigned char CTlkTable::GENDER_FEMALE = 1;
const unsigned char CTlkTable::NUM_CONTENT_PKGS = 7;
// PC 0x0041EC10
CTlkFile::CTlkFile(const CExoString& sResRef)
    : CExoFile(sResRef, RESTYPE_TLK, "rb")
{
    memset(&m_nFileType, 0, 20);
}
// PC 0x0041EC90
int CTlkFile::ReadHeader()
{
    SeekBeginning();

    if (Read(&m_nFileType, 1, 20) != 20)
    {
        return FALSE;
    }

    return m_nFileType == TLK_FILE_TYPE;
}
// PC 0x0041ECD0
CTlkTable::CTlkTable()
{
    m_nCurGender = GENDER_MALE;
    for (int i = 0; i < 14; i++)
    {
        m_aFiles[i] = NULL;
    }
    m_pLanguageTokens = NULL;
    m_nLanguageTokenCount = 0;
    m_pCustomTokens = NULL;
    m_nCustomTokenCount = 0;
    m_bDebugMode = 0;
}

inline void TlkTableProbeFile(const CExoString& sResRef, CTlkTable::SCustomToken* pToken)
{
    CTlkFile file(sResRef);
    pToken->~SCustomToken();
}

inline void TlkTableReleaseEntries(CTlkFile* pFile, CTlkTable::SCustomToken* pTokens, int nCount)
{
    delete pFile;
    CTlkTable::SCustomToken* pNew = new CTlkTable::SCustomToken[nCount];
    pNew[0] = pTokens[0];
    delete[] pTokens;
}
// PC 0x0041ED10
int CTlkTable::AddFile(const CExoString& sResRef, int nContentPkg)
{
    if (nContentPkg < 0 || nContentPkg > NUM_CONTENT_PKGS || sResRef == "")
    {
        return FALSE;
    }

    if (m_aFiles[nContentPkg * 2 + 1] == m_aFiles[nContentPkg * 2])
    {
        m_aFiles[nContentPkg * 2 + 1] = NULL;
    }
    delete m_aFiles[nContentPkg * 2 + 1];
    delete m_aFiles[nContentPkg * 2];

    m_aFiles[nContentPkg * 2] = new CTlkFile(sResRef);
    if (!m_aFiles[nContentPkg * 2]->FileOpened())
    {
        return FALSE;
    }
    if (!m_aFiles[nContentPkg * 2]->ReadHeader())
    {
        delete m_aFiles[nContentPkg * 2];
        m_aFiles[nContentPkg * 2] = NULL;
        return FALSE;
    }

    m_aFiles[nContentPkg * 2 + 1] = new CTlkFile(sResRef + "F");
    if (!m_aFiles[nContentPkg * 2 + 1]->FileOpened())
    {
        delete m_aFiles[nContentPkg * 2 + 1];
        m_aFiles[nContentPkg * 2 + 1] = m_aFiles[nContentPkg * 2];
    }
    else if (!m_aFiles[nContentPkg * 2 + 1]->ReadHeader())
    {
        delete m_aFiles[nContentPkg * 2];
        m_aFiles[nContentPkg * 2] = NULL;
        delete m_aFiles[nContentPkg * 2 + 1];
        m_aFiles[nContentPkg * 2 + 1] = NULL;
        return FALSE;
    }

    return TRUE;
}

#ifndef _XBOX
int CTlkTable::ExoIsDBCSLeadByte(unsigned char c)
{
    if (IsDBCSLeadByte(c) || c > 0x7f)
    {
        return TRUE;
    }
    return FALSE;
}
#endif
// PC 0x0041EF40
void CTlkTable::SetCustomToken(int nToken, const CExoString& sValue)
{
    if (nToken < 0)
    {
        return;
    }

    unsigned long nLow = 0;
    unsigned long nHigh = m_nCustomTokenCount;
    while (nHigh > nLow)
    {
        unsigned long nMiddle = (nLow + nHigh) / 2;
        if (m_pCustomTokens[nMiddle].m_nToken > nToken)
        {
            nHigh = nMiddle;
        }
        else if (m_pCustomTokens[nMiddle].m_nToken < nToken)
        {
            nLow = nMiddle + 1;
        }
        else
        {
            m_pCustomTokens[nMiddle].m_sValue = sValue;
            return;
        }
    }

    SCustomToken* pTokens = new SCustomToken[m_nCustomTokenCount + 1];
    unsigned long i;
    for (i = 0; i < nLow; i++)
    {
        pTokens[i] = m_pCustomTokens[i];
    }
    pTokens[i].m_nToken = nToken;
    pTokens[i].m_sValue = sValue;
    for (; i < m_nCustomTokenCount; i++)
    {
        pTokens[i + 1] = m_pCustomTokens[i];
    }

    delete[] m_pCustomTokens;
    m_pCustomTokens = pTokens;
    m_nCustomTokenCount++;
}
// PC 0x0041FAB0
int CTlkTable::Fetch(unsigned long nStrRef, STR_RES& pResult,
                     unsigned char nGender)
{
    int nResult;

    m_nCurGender = nGender;
    nResult = FetchInternal(nStrRef, pResult, FALSE);

    if (m_bDebugMode)
    {
        pResult.m_sString.Format("[%d]%s", nStrRef, pResult.m_sString.CStr());
    }

    return nResult;
}
// PC 0x0041FB10
int CTlkTable::OpenFile(const CExoString& sResRef)
{
    if (!AddFile(sResRef, 0))
    {
        return 0;
    }

    if (m_pLanguageTokens != NULL)
    {
        delete[] m_pLanguageTokens;
    }

    CTlkTableToken token;
    C2DA c2DA("StringTokens", 0);

    if (!c2DA.Load2DArray())
    {
        return 0;
    }

    m_nLanguageTokenCount = c2DA.m_nNumRows;

    m_pLanguageTokens = new CTlkTableToken[m_nLanguageTokenCount];

    for (unsigned long nRow = 0; nRow < m_nLanguageTokenCount; nRow++)
    {
        int nValue;

        c2DA.GetCExoStringEntry(nRow, 0, &m_pLanguageTokens[nRow].m_sToken);
        c2DA.GetINTEntry(nRow, 1, &nValue);
        m_pLanguageTokens[nRow].m_nType = nValue;
        c2DA.GetINTEntry(nRow, 2, &nValue);
        m_pLanguageTokens[nRow].m_nTokenValue = nValue;
        for (unsigned long nCol = 0; nCol < 4; nCol++)
        {
            c2DA.GetINTEntry(nRow, nCol + 3, &nValue);
            m_pLanguageTokens[nRow].m_nStringRefs[nCol] = nValue;
        }
        m_pLanguageTokens[nRow].m_nHash = CResGFFFieldIDHash::Hash(
            m_pLanguageTokens[nRow].m_sToken.CStr(),
            m_pLanguageTokens[nRow].m_sToken.GetLength());

        unsigned long nSortIdx = nRow;
        while (nSortIdx > 0 && m_pLanguageTokens[nSortIdx].m_nHash <
               m_pLanguageTokens[nSortIdx - 1].m_nHash)
        {
            token = m_pLanguageTokens[nSortIdx - 1];
            m_pLanguageTokens[nSortIdx - 1] = m_pLanguageTokens[nSortIdx];
            m_pLanguageTokens[nSortIdx] = token;
            nSortIdx--;
        }
        EXOXBOXASSERT(nSortIdx == 0 || m_pLanguageTokens[nSortIdx].m_nHash > m_pLanguageTokens[nSortIdx - 1].m_nHash);
    }

    return 1;
}
// PC 0x0041FE60
CExoString CTlkTable::GetSimpleString(unsigned long nStrRef)
{
    STR_RES strRes;

    m_nCurGender = GENDER_MALE;
    FetchInternal(nStrRef, strRes, FALSE);

    if (m_bDebugMode)
    {
        strRes.m_sString.Format("[%d]%s", nStrRef, strRes.m_sString.CStr());
    }

    return strRes.m_sString;
}
// PC 0x0041F660
CTlkTable::~CTlkTable()
{
    delete[] m_pCustomTokens;
    delete[] m_pLanguageTokens;

    CTlkFile** pFile = &m_aFiles[1];
    for (int i = 0; i < NUM_CONTENT_PKGS; i++)
    {
        if (*pFile != NULL && *pFile != *(pFile - 1))
        {
            delete *pFile;
        }
        if (*(pFile - 1) != NULL)
        {
            delete *(pFile - 1);
        }
        pFile += 2;
    }
}
// PC 0x0041F700
int CTlkTable::FetchInternal(unsigned long nStrRef, STR_RES& strRes, int bParseTokens)
{
    CExoString sString;
    int bFound = FALSE;
    CExoString sError;
    sError.Format("Invalid STRREF %d passed to Fetch", nStrRef);

    if (nStrRef == STRREF_INVALID || m_nCurGender > GENDER_FEMALE)
    {
        strRes.m_sString = "";
        strRes.m_resSound = "";
        return TRUE;
    }

    for (int nPackage = 0; nPackage < NUM_CONTENT_PKGS; ++nPackage)
    {
        CTlkFile* pFile = m_aFiles[nPackage * 2 + m_nCurGender];
        if (pFile != NULL)
        {
            unsigned long nVersion = pFile->m_nFileVersion;
            unsigned long nEntrySize = nVersion ==
                ((unsigned char)CTlkFile::TLK_FILE_VERSION[0] |
                ((unsigned char)CTlkFile::TLK_FILE_VERSION[1] << 8) |
                ((unsigned char)CTlkFile::TLK_FILE_VERSION[2] << 16) |
                ((unsigned char)CTlkFile::TLK_FILE_VERSION[3] << 24)) ?
                sizeof(CTlkFile::StringEntry) : 36;
            nStrRef &= STRREF_INDEX_MASK;
            unsigned long nStringCount = pFile->m_nStringCount;
            if (nStrRef < nStringCount)
            {
                if (nStrRef * nEntrySize + 20 <= pFile->GetSize() &&
                    pFile->Seek(nStrRef * nEntrySize + 20, 0))
                {
                    CTlkFile::StringEntry entry;
                    entry.m_fSoundLength = 0.0f;
                    if (pFile->Read(&entry, 1, nEntrySize) == nEntrySize &&
                        !(entry.m_nFlags & CTlkTable::STRING_NOT_IN_PROJECT))
                    {
                        bFound = TRUE;
                        if (entry.m_nFlags & CTlkTable::SNDLENGTH_PRESENT)
                        {
                            strRes.m_fSoundLength = entry.m_fSoundLength;
                        }
                        else
                        {
                            strRes.m_fSoundLength = 0.0f;
                        }
                        if (entry.m_nFlags & CTlkTable::SND_PRESENT)
                        {
                            strRes.m_resSound = entry.m_resSound;
                        }
                        else
                        {
                            strRes.m_resSound = "";
                        }
                        if (entry.m_nFlags & CTlkTable::TEXT_PRESENT)
                        {
                            unsigned long nStringOffset = pFile->m_nStringEntriesOffset;
                            unsigned long nFileSize = pFile->GetSize();
                            if (entry.m_nOffset + nStringOffset > nFileSize)
                            {
                                strRes.m_sString = CExoString(nStrRef);
                                strRes.m_resSound = "";
                                return FALSE;
                            }
                            unsigned long nStringPos = entry.m_nOffset + pFile->m_nStringEntriesOffset;
                            if (!pFile->Seek(nStringPos, 0))
                            {
                                strRes.m_sString = "";
                                strRes.m_resSound = "";
                                return FALSE;
                            }
                            char* pBuffer = new char[entry.m_nLength];
                            pFile->Read(pBuffer, 1, entry.m_nLength);
                            sString = CExoString(pBuffer, entry.m_nLength);
                            delete[] pBuffer;
                            if (bParseTokens)
                            {
                                ParseStr(sString);
                            }
                            strRes.m_sString = sString;
                        }
                    }
                }
            }
        }
        if (bFound)
        {
            break;
        }
    }
    if (m_nCurGender == GENDER_FEMALE && !bFound)
    {
        m_nCurGender = GENDER_MALE;
        return FetchInternal(nStrRef, strRes, bParseTokens);
    }
    if (!bFound)
    {
        strRes.m_sString = "";
        strRes.m_resSound = "";
        return FALSE;
    }
    return TRUE;
}
// PC 0x0041F0E0
void CTlkTable::ClearCustomTokens()
{
    delete[] m_pCustomTokens;
    m_pCustomTokens = NULL;
    m_nCustomTokenCount = 0;
}
// PC 0x0041F120
void CTlkTable::ParseStr(CExoString& sText)
{
    CExoString sParsed;
    CExoString sValue;
    m_bParsing = FALSE;
    int nStart = 0;
    const char* pText = sText.CStr();
    int nPos = 0;
    while (pText[nPos] != '\0')
    {
        if (pText[nPos] == '<')
        {
            if (pText[nPos + 1] == '<')
            {
                ++nPos;
                int bParsing = m_bParsing;
                if (!bParsing)
                {
                    sParsed = sParsed + CExoString(pText + nStart, nPos - nStart);
                }
                nStart = nPos + 1;
            }
            else
            {
                int bParsing = m_bParsing;
                if (!bParsing)
                {
                    sParsed = sParsed + CExoString(pText + nStart, nPos - nStart);
                }
                sValue = "<UNRECOGNIZED TOKEN>";
                int nTokenStart = nPos + 1;
                for (++nPos; ; ++nPos)
                {
                    if (pText[nPos] == '\0')
                    {
                        goto TokenDone;
                    }
                    if (pText[nPos] == '>')
                    {
                        break;
                    }
                }
                if (nTokenStart < nPos)
                {
                    unsigned long nLength = nPos - nTokenStart;
                    if (nLength >= CUSTOMTOKEN_MIN_LENGTH && memcmp(&pText[nTokenStart], "CUSTOM", CUSTOMTOKEN_PREFIX_LENGTH) == 0)
                    {
                        unsigned long nToken = 0;
                        int nMultiplier = 1;
                        for (int nDigit = nPos - 1; nDigit >= nTokenStart + CUSTOMTOKEN_PREFIX_LENGTH; --nDigit)
                        {
                            if (pText[nDigit] < '0' || pText[nDigit] > '9')
                            {
                                goto TokenDone;
                            }
                            nToken += (pText[nDigit] - '0') * nMultiplier;
                            nMultiplier *= 10;
                        }
                        if (nToken == CUSTOMTOKEN_INVALID)
                        {
                            goto TokenDone;
                        }
                        unsigned long nLow = 0;
                        unsigned long nHigh = m_nCustomTokenCount;
                        while (nHigh > nLow)
                        {
                            unsigned long nMiddle = (nHigh + nLow) / 2;
                            if (m_pCustomTokens[nMiddle].m_nToken > nToken)
                            {
                                nHigh = nMiddle;
                            }
                            else if (m_pCustomTokens[nMiddle].m_nToken < nToken)
                            {
                                nLow = nMiddle + 1;
                            }
                            else
                            {
                                sValue = m_pCustomTokens[nMiddle].m_sValue;
                                break;
                            }
                        }
                    }
                    else
                    {
                        const char* pToken = &pText[nTokenStart];
                        unsigned long nHash = CResGFFFieldIDHash::Hash(pToken, nLength);
                        unsigned long nLow = 0;
                        unsigned long nHigh = m_nLanguageTokenCount;
                        while (nHigh > nLow)
                        {
                            unsigned long nMiddle = (nHigh + nLow) / 2;
                            if (m_pLanguageTokens[nMiddle].m_nHash > nHash)
                            {
                                nHigh = nMiddle;
                            }
                            else if (m_pLanguageTokens[nMiddle].m_nHash < nHash)
                            {
                                nLow = nMiddle + 1;
                            }
                            else
                            {
                                if (m_pLanguageTokens[nMiddle].m_sToken.GetLength() == nLength &&
                                    strncmp(m_pLanguageTokens[nMiddle].m_sToken.CStr(), pToken, nLength) == 0)
                                {
                                    GetTokenValue(m_pLanguageTokens[nMiddle], sValue);
                                }
                                break;
                            }
                        }
                    }
                }
TokenDone:
                nStart = nPos + 1;
                sParsed = sParsed + sValue;
            }
        }
        ++nPos;
    }
    if (nStart > 0)
    {
        if (nStart < nPos && !m_bParsing)
        {
            pText += nStart;
            sParsed = sParsed + CExoString(pText);
        }
        sText = sParsed;
    }
}
// PC 0x0041F650
int CTlkTable::GetLanguageVersion()
{
    CTlkFile* pFile = m_aFiles[0];

    if (pFile != NULL)
    {
        return pFile->m_nLanguageID;
    }
    else
    {
        return 0;
    }
}

long ExoResFileInfo(const char* szFileName);

long ExoLogResFileInfo(const CExoString& sFileName)
{
    return ExoResFileInfo(sFileName.CStr());
}
