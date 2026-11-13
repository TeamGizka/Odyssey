#include "nwerffile.h"
#include "appmanager.h"
#include "resgff.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static inline void NwerffileTrace3();
static inline void NwerffileTrace2();
static inline void NwerffileTrace1();
#define NWERFFILE_TRACE() NwerffileTrace1()

int CERFRes::Read(CExoFile& file)
{
    file.Read(this, 4, 1);
    file.Read(&m_nSize, 4, 1);
    return 1;
}
// PC 0x0062B200 GOG 0x0062B0D0
void CERFFile::ImportFiles(const CExoString& sDirectory, void* pUnused,
                          int (*pfnProgress)(int, int, int, int), int nArg1,
                          int nArg2)
{
    NwerffileTrace3();
    CExoArrayList<CExoString> lstFiles;
    CExoString sFile;

    g_pExoBase->GetDirectoryList(&lstFiles, sDirectory, RESTYPE_INVALID, 0, 0);
    SetNumEntries(lstFiles.num);

    for (int i = 0; i < lstFiles.num; i++)
    {
        sFile.Format("%s%s", sDirectory.CStr(), lstFiles.element[i]);
        WriteResource(sFile, pUnused);

        if (pfnProgress != NULL)
        {
            pfnProgress(i, lstFiles.num, nArg1, nArg2);
        }
    }

}

#ifdef _XBOX

int CERFFile::ReadHeaderVariance(const CExoString& sFileName, void* pSignature)
{
    CExoFile file(sFileName, 2057, "rb");
    HANDLE hSignature = INVALID_HANDLE_VALUE;

    if (pSignature != NULL)
    {
        hSignature = XCalculateSignatureBegin(0);
    }

    if (file.FileOpened())
    {
        file.Seek(160, 0);

        for (unsigned long i = 0; i < m_nLanguageCount; i++)
        {
            CERFString* pString = new CERFString;
            pString->Read(file);

            if (pSignature != NULL && hSignature != INVALID_HANDLE_VALUE)
            {
                XCalculateSignatureUpdate(hSignature, (BYTE*)pString, 4);
                XCalculateSignatureUpdate(hSignature, (BYTE*)pString->m_pText,
                                          pString->m_nLength);
            }

            m_lstStrings.AddTail(pString);
        }

        for (unsigned long j = 0; j < m_nEntryCount; j++)
        {
            CERFKey* pKey = new CERFKey;
            pKey->Read(file);

            if (pSignature != NULL && hSignature != INVALID_HANDLE_VALUE)
            {
                XCalculateSignatureUpdate(hSignature, (BYTE*)pKey, 16);
                XCalculateSignatureUpdate(hSignature, (BYTE*)&pKey->m_nResId,
                                          4);
                XCalculateSignatureUpdate(hSignature, (BYTE*)&pKey->m_nResType,
                                          2);
                XCalculateSignatureUpdate(hSignature, (BYTE*)&pKey->m_nUnused,
                                          2);
            }

            m_lstKeys.AddTail(pKey);
        }

        for (unsigned long k = 0; k < m_nEntryCount; k++)
        {
            CERFRes* pRes = new CERFRes;
            pRes->Read(file);

            if (pSignature != NULL && hSignature != INVALID_HANDLE_VALUE)
            {
                XCalculateSignatureUpdate(hSignature, (BYTE*)pRes, 4);
                XCalculateSignatureUpdate(hSignature, (BYTE*)&pRes->m_nSize, 4);
            }

            m_lstResources.AddTail(pRes);
        }
    }

    if (pSignature != NULL && hSignature != INVALID_HANDLE_VALUE)
    {
        XCalculateSignatureEnd(hSignature, (PXCALCSIG_SIGNATURE)pSignature);
    }

    return 1;
}
#else
int CERFFile::ReadHeaderVariance(const CExoString& sFileName)  // PC 0x0062A3E0 GOG 0x0062A2B0
{
    CExoFile file(sFileName, 2057, "rb");

    if (file.FileOpened())
    {
        file.Seek(160, 0);

        for (unsigned long i = 0; i < m_nLanguageCount; i++)
        {
            CERFString* pString = new CERFString;
            pString->Read(file);
            m_lstStrings.AddTail(pString);
        }

        for (unsigned long j = 0; j < m_nEntryCount; j++)
        {
            CERFKey* pKey = new CERFKey;
            pKey->Read(file);
            m_lstKeys.AddTail(pKey);
        }

        for (unsigned long k = 0; k < m_nEntryCount; k++)
        {
            CERFRes* pRes = new CERFRes;
            pRes->Read(file);
            m_lstResources.AddTail(pRes);
        }
    }

    return 1;
}
#endif
// PC 0x0062A5C0 GOG 0x0062A490
int CERFFile::WriteStringTable()
{
    CExoFile* pFile = m_pFile;

    if (pFile != NULL)
    {
        unsigned long nCount = m_lstStrings.Count();
        unsigned long nSize = 0;
        unsigned long nStart = pFile->GetOffset();

        CExoLinkedListNode* pNode = m_lstStrings.GetHeadPos();

        while (pNode != NULL)
        {
            CERFString* pString = m_lstStrings.GetAtPos(pNode);
            nSize = pString->m_nLength + 8;
            pString->Write(*m_pFile);
            m_lstStrings.GetNext(pNode);
        }

        unsigned long nEnd = m_pFile->GetOffset();
        m_pFile->Seek(8, 0);
        m_pFile->Write(&nCount, 4, 1);
        m_pFile->Write(&nSize, 4, 1);
        int nPos = m_pFile->GetOffset() + 4;
        m_pFile->Seek(nPos, 0);
        m_nStringTablePos = nStart;
        m_pFile->Write(&nStart, 4, 1);
        m_nKeyListPos = nEnd;
        m_pFile->Write(&nEnd, 4, 1);
        m_pFile->SeekEnd();
        return 1;
    }

    return 0;
}
// PC 0x0062B320 GOG 0x0062B1F0
int CERFFile::Reset()
{
    NwerffileTrace3();
    memset(this, 0, 160);

    while (!m_lstStrings.IsEmpty())
    {
        CERFString* pString = m_lstStrings.RemoveHead();

        if (pString != NULL)
        {
            delete pString;
        }
    }

    while (!m_lstKeys.IsEmpty())
    {
        CERFKey* pKey = m_lstKeys.RemoveHead();

        if (pKey != NULL)
        {
            delete pKey;
        }
    }

    while (!m_lstResources.IsEmpty())
    {
        CERFRes* pRes = m_lstResources.RemoveHead();

        if (pRes != NULL)
        {
            delete pRes;
        }
    }

    return 1;
}
// PC 0x0062B400 GOG 0x0062B2D0
CERFFile::~CERFFile()
{
    Reset();
}

int CERFString::Write(CExoFile& file)
{
    file.Write(this, 4, 1);
    file.Write(&m_nLength, 4, 1);
    file.Write(m_pText, m_nLength, 1);
    return 1;
}
// PC 0x0062AFA0 GOG 0x0062AE70
void CERFFile::ExportFilesFromERF(const CExoString& sFileName,
                                 const CExoString& sDirectory, void* pUnused,
                                 int (*pfnProgress)(int, int, int, int),
                                 int nArg1, int nArg2)
{
    CExoFile file(sFileName, 2057, "rb");

    if (file.FileOpened())
    {
        CExoString sPath;
        CExoLinkedListNode* pKeyPos = m_lstKeys.GetHeadPos();
        CExoLinkedListNode* pResPos = m_lstResources.GetHeadPos();

        for (unsigned long i = 0; i < m_nEntryCount; i++)
        {
            CERFKey* pKey = m_lstKeys.GetAtPos(pKeyPos);
            CERFRes* pRes = m_lstResources.GetAtPos(pResPos);
            sPath = sDirectory + pKey->m_sResRef;
            CExoFile* pOut = new CExoFile(sPath, pKey->m_nResType, "wb");
            pRes->CopyToFile(&file, pOut, pUnused);
            pOut->Flush();

            if (pfnProgress != NULL)
            {
                pfnProgress(i, m_nEntryCount, nArg1, nArg2);
            }

            delete pOut;
            m_lstKeys.GetNext(pKeyPos);
            m_lstResources.GetNext(pResPos);
        }

        if (pfnProgress != NULL)
        {
            pfnProgress(m_nEntryCount, m_nEntryCount, nArg1, nArg2);
        }
        return;
    }
}
// PC 0x0062A730 GOG 0x0062A600
inline CERFKey::~CERFKey()
{
    NwerffileTrace1();
    m_nResId = 0;
    m_nResType = -1;
    memset(m_sResRef, 0, 16);
}
// PC 0x0062A750 GOG 0x0062A620
inline CERFRes::~CERFRes()
{
    NwerffileTrace1();

    if (m_pData != NULL)
    {
        delete m_pData;
        m_pData = NULL;
    }

    m_nOffset = 0;
    m_nSize = 0;
}
// PC 0x0062A7F0 GOG 0x0062A6C0
CERFFile::CERFFile()
{
    memset(this, 0, 160);
    m_pFile = NULL;
    m_nNumEntries = 0;
    m_nCurEntry = 0;
}
// PC 0x0062A880 GOG 0x0062A750
int CERFFile::SetNumEntries(unsigned long nEntries)
{
    NwerffileTrace2();
    CERFKey key;
    unsigned long nFiller;

    if (m_pFile != NULL)
    {
        m_nNumEntries = nEntries;
        m_nCurEntry = 0;
        m_pFile->Seek(16, 0);
        m_pFile->Write(&nEntries, 4, 1);
        m_pFile->Seek(m_nKeyListPos, 0);

        for (unsigned long i = 0; i < nEntries; i++)
        {
            m_pFile->Write(&key, 24, 1);
        }

        m_nResourceListPos = m_pFile->GetOffset();
        m_pFile->Seek(28, 0);
        m_pFile->Write(&m_nResourceListPos, 4, 1);
        m_pFile->SeekEnd();
        nFiller = 0xBAADF00D;

        for (unsigned long j = 0; j < nEntries; j++)
        {
            m_pFile->Write(&nFiller, 4, 1);
            m_pFile->Write(&nFiller, 4, 1);
        }

        return 1;
    }

    return 0;
}
// PC 0x0062A9F0 GOG 0x0062A8C0
int CERFFile::WriteResource(const CExoString& sFileName, void* pUnused)
{
    NwerffileTrace2();
    if (m_nCurEntry < m_nNumEntries && m_pFile != NULL)
    {
        CERFKey key;
        CERFRes res;
        CExoFile* pIn;
        pIn = new CExoFile(sFileName, RESTYPE_INVALID, "rb");

        int nColon = sFileName.Find(":", 0);
        CExoString sName =
            sFileName.Right(sFileName.GetLength() - nColon - 1);
        int nDot = sName.Find(".", 0);
        CExoString sExtension = sName.Right(sName.GetLength() - nDot - 1);
        sName = sName.Left(nDot);

        unsigned long nType = g_pExoBase->GetResTypeFromExtension(sExtension);
        key.SetName(sName);
        key.m_nResId = m_nCurEntry;
        key.m_nResType = nType;

        int nSize = pIn->GetSize();
        int nRemaining = nSize;
        CAppManager* pApp = g_pAppManager;

        if (g_pAppManager->m_pLargeTempBuffer == NULL)
        {
            g_pAppManager->AllocLargeTempBuffer();
        }

        void* pTemp = pApp->m_pLargeTempBuffer;
        unsigned long nOffset = m_pFile->GetOffset();

        while (nRemaining > 0)
        {
            int nChunk = nRemaining > 0x3FFFF ? 0x3FFFF : nRemaining;

            pIn->Read(pTemp, 1, nChunk);
#ifdef _XBOX
            if (pUnused != INVALID_HANDLE_VALUE)
            {
                XCalculateSignatureUpdate((HANDLE)pUnused,
                                          (unsigned char*)pTemp, nChunk);
            }
#endif
            m_pFile->Write(pTemp, nChunk, 1);
            nRemaining -= nChunk;
        }
        m_pFile->Seek(m_nResourceListPos + m_nCurEntry * 8, 0);
        m_pFile->Write(&nOffset, 4, 1);
        m_pFile->Write(&nSize, 4, 1);
        m_pFile->SeekEnd();

        delete pIn;

        m_pFile->Seek(m_nKeyListPos + m_nCurEntry * 24, 0);
        key.Write(*m_pFile);
        m_pFile->SeekEnd();
        m_nCurEntry++;
        return 1;
    }

    return 0;
}
// PC 0x00629E70 GOG 0x00629D40
void CERFFile::SetVersion(char* sVersion)
{
    memcpy(m_sFileType, sVersion, 4);
    memcpy(m_sVersion, sVersion + 4, 4);
}
// PC 0x00629E90 GOG 0x00629D60
#ifdef _XBOX

int CERFFile::Read(const CExoString& sFileName, void* pSignature)
#else
int CERFFile::Read(const CExoString& sFileName)
#endif
{
    CExoFile file(sFileName, 2057, "rb");

    if (file.FileOpened())
    {
        file.Seek(0, 0);
        m_nFileSize = file.GetSize();
        file.Read(this, 8, 1);
        file.Read(&m_nLanguageCount, 4, 1);
        file.Read(&m_nLocStringSize, 4, 1);
        file.Read(&m_nEntryCount, 4, 1);
        file.Read(&m_nOffsetToLocStrings, 4, 1);
        file.Read(&m_nOffsetToKeyList, 4, 1);
        file.Read(&m_nOffsetToResourceList, 4, 1);
        file.Read(&m_nBuildYear, 4, 1);
        file.Read(&m_nBuildDay, 4, 1);
        file.Read(&m_nDescriptionStrRef, 4, 1);
        file.Read(m_aReserved, 116, 1);
#ifdef _XBOX
        if (pSignature != NULL)
        {
            HANDLE hSignature = XCalculateSignatureBegin(0);

            if (hSignature != INVALID_HANDLE_VALUE)
            {
                XCalculateSignatureUpdate(hSignature, (BYTE*)this, 160);
                XCalculateSignatureUpdate(hSignature, (BYTE*)&m_nFileSize, 4);
                XCalculateSignatureEnd(hSignature,
                                       (PXCALCSIG_SIGNATURE)pSignature);
            }
        }
#endif
    }

    return 1;
}
// PC 0x0062A000 GOG 0x00629ED0
int CERFFile::Create(CExoString& sFileName)
{
    m_pFile = new CExoFile(sFileName, RESTYPE_SAV, "wb");
    return m_pFile->FileOpened() != 0;
}
// PC 0x0062A0C0 GOG 0x00629F90
int CERFFile::WriteHeader()
{
    if (m_pFile != NULL)
    {
        time_t tTime = time(NULL);
        struct tm* pTime = localtime(&tTime);

#ifndef _XBOX
        if (pTime != NULL)
#endif
        {
            m_nBuildYear = pTime->tm_year;
            m_nBuildDay = pTime->tm_yday;
        }

        m_pFile->Write(this, 160, 1);
        return 1;
    }

    return 0;
}
// PC 0x0062A120 GOG 0x00629FF0
int CERFFile::Finish()
{
    m_pFile->Flush();
    delete m_pFile;
    m_pFile = NULL;
    return 1;
}

int CERFKey::Read(CExoFile& file)
{
    EXOXBOXASSERT(file.FileOpened());
    file.Read(this, 16, 1);
    file.Read(&m_nResId, 4, 1);
    file.Read(&m_nResType, 2, 1);
    file.Read(&m_nUnused, 2, 1);
    return 1;
}
// PC 0x0062A160 GOG 0x0062A030
void CERFKey::SetName(CExoString& sName)
{
    memset(m_sResRef, 0, 16);
    memcpy(m_sResRef, sName.CStr(), sName.GetLength());
}
// PC 0x0062A1B0 GOG 0x0062A080
int CERFKey::Write(CExoFile& file)
{
    EXOXBOXASSERT(file.FileOpened());
    file.Write(this, 16, 1);
    file.Write(&m_nResId, 4, 1);
    file.Write(&m_nResType, 2, 1);
    file.Write(&m_nUnused, 2, 1);
    return 1;
}
// PC 0x0062A780 GOG 0x0062A650
int CERFRes::CopyToFile(CExoFile* pIn, CExoFile* pOut, void* pBuffer)
{
    pIn->Seek(m_nOffset, 0);

    CAppManager* pApp = g_pAppManager;

    if (g_pAppManager->m_pLargeTempBuffer == NULL)
    {
        g_pAppManager->AllocLargeTempBuffer();
    }

    void* pTemp = pApp->m_pLargeTempBuffer;
    int nRemaining;
    unsigned long nChunk;

    for (nRemaining = m_nSize; nRemaining > 0; nRemaining -= nChunk)
    {
        nChunk = nRemaining;

        if (nRemaining >= 0x40000)
        {
            nChunk = 0x40000;
        }

        pIn->Read(pTemp, nChunk, 1);
        pOut->Write(pTemp, nChunk, 1);
#ifdef _XBOX
        if (pBuffer != INVALID_HANDLE_VALUE && nChunk != 0)
        {
            XCalculateSignatureUpdate((HANDLE)pBuffer, (unsigned char*)pTemp,
                                      nChunk);
        }
#endif
    }

    return 1;
}
// PC 0x0062AD60 GOG 0x0062AC30
int CERFFile::WriteResource(char* sResRef, unsigned short nResType, CRes* pRes,
                            int bIsGFF, void* pUnused)
{
    NwerffileTrace2();
    CERFKey key;
    CERFRes res;

    if (m_nCurEntry >= m_nNumEntries || m_pFile == NULL)
    {
        return 0;
    }

    CExoString sName(sResRef);
    key.SetName(sName);
    key.m_nResId = m_nCurEntry;
    key.m_nResType = nResType;

    if (bIsGFF != 0)
    {
        unsigned long nOffset = m_pFile->GetOffset();
        CResGFF* pGFF = (CResGFF*)pRes;
        pGFF->Pack(0, 0);
        pGFF->GetTotalSize();
#ifdef _XBOX
        if (pUnused != INVALID_HANDLE_VALUE)
        {
            XCalculateSignatureUpdate((HANDLE)pUnused,
                                      (unsigned char*)pGFF->GetData(),
                                      pGFF->GetSize());
        }
#endif
        unsigned long nSize;
        pGFF->WriteGFFData(*m_pFile, nSize);
        m_pFile->Seek(m_nResourceListPos + m_nCurEntry * 8, 0);
        m_pFile->Write(&nOffset, 4, 1);
        m_pFile->Write(&nSize, 4, 1);
    }
    else
    {
        res.m_nSize = pRes->GetSize();
        void* pData = new char[pRes->GetSize()];
        memcpy(pData, pRes->GetData(), pRes->GetSize());
#ifdef _XBOX
        if (pUnused != INVALID_HANDLE_VALUE)
        {
            XCalculateSignatureUpdate((HANDLE)pUnused,
                                      (unsigned char*)pRes->GetData(),
                                      pRes->GetSize());
        }
#endif
        res.m_pData = pData;
        res.Write(*m_pFile, m_nResourceListPos + m_nCurEntry * 8);
    }

    m_pFile->Seek(m_nKeyListPos + m_nCurEntry * 24, 0);
    key.Write(*m_pFile);
    m_pFile->SeekEnd();
    m_nCurEntry++;
    return 1;
}
// PC 0x0062A200 GOG 0x0062A0D0
int CERFRes::Write(CExoFile& file, unsigned long nTableOffset)
{
    m_nOffset = file.GetOffset();
    file.Write(m_pData, m_nSize, 1);
    file.Seek(nTableOffset, 0);
    file.Write(this, 4, 1);
    file.Write(&m_nSize, 4, 1);
    file.SeekEnd();
    return 1;
}
// PC 0x0062A260 GOG 0x0062A130
int CERFString::SetText(CExoString& sText)
{
    if (m_pText != NULL)
    {
        delete m_pText;
    }

    m_nLength = sText.GetLength();
#ifdef _XBOX
    m_pText = new char[m_nLength + 1];
    EXOXBOXASSERT(m_pText);
    int nLength = m_nLength;
#else
    char* pText = new char[m_nLength + 1];
    int nLength = m_nLength;
    m_pText = pText;
#endif
    memcpy(m_pText, sText.CStr(), nLength);
    m_pText[m_nLength] = 0;
    return nLength;
}
// PC 0x0062A2D0 GOG 0x0062A1A0
int CERFString::Read(CExoFile& file)
{
    CExoString sText;

    file.Read(this, 4, 1);
    file.Read(&m_nLength, 4, 1);
    file.Read(sText, m_nLength);
    SetText(sText);
    return 1;
}

static inline void NwerffileTrace3()
{
    NwerffileTrace2();
}

static inline void NwerffileTrace2()
{
    NwerffileTrace1();
}

static inline void NwerffileTrace1()
{
}
