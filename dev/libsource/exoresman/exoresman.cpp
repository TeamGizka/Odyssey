#include "exoresman.h"
#include "exobase.h"
#include "resgff.h"
#include "swmain.h"
#include <windows.h>
#include <ctype.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

char CResRef::RETURN_BUFFERS[4][17];

int g_nResRefUnused = 0;
int CResRef::s_nCurBufferIndex = 0;

static inline void ExoresmanTrace1();
// PC 0x00405F70 GOG 0x00405F40
CResRef::CResRef()
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        m_resRef[i] = 0;
    }
}
// PC 0x00405F90 GOG 0x00405F60
CResRef::CResRef(unsigned char* pResRef, unsigned char nLength)
{
    if (pResRef)
    {
        unsigned char nRemaining = 16 - (nLength < 16 ? nLength : 16);
        memset(m_resRef + 16 - nRemaining, 0, nRemaining);
        memcpy(m_resRef, pResRef, 16 - nRemaining);
    }
    else
    {
        for (unsigned int i = 0; i < 16; ++i)
        {
            m_resRef[i] = 0;
        }
    }
}
// PC 0x00406EA0 GOG 0x00406E70
CResRef::CResRef(const CExoString& sString)
{
    *this = sString;
}
// PC 0x00406EC0 GOG 0x00406E90
CResRef::CResRef(char* pString)
{
    *this = pString;
}
// PC 0x00406EE0 GOG 0x00406EB0
CResRef::CResRef(unsigned char* pResRef)
{
    *this = pResRef;
}
// PC 0x00406F00 GOG 0x00406ED0
int CResRef::operator!=(const CResRef& other) const
{
    return !(*this == other);
}

int CResRef::operator!=(const CExoString& sString) const
{
    ExoresmanTrace1();
    sString.IsEmpty();

    const char* pString = sString.CStr();
    unsigned int nLength = sString.GetLength() + 1;
    if (nLength > 16)
    {
        nLength = 16;
    }
    return memcmp(pString, m_resRef, nLength) != 0;
}
// PC 0x00698BD0 GOG 0x00698980
unsigned char* CResRef::GetResRef() const
{
    return (unsigned char*)m_resRef;
}

#ifndef _XBOX
int CExoResMan::FindModuleDownloadedPkg(const CExoString& sModuleName)  // PC 0x006955A0 GOG 0x00695350
{
    return 0;
}
#endif
// PC 0x004088D0 GOG 0x004088A0
CExoResMan::~CExoResMan()
{
    m_bExitAsyncThread = 1;
    ResumeThread(m_hAsyncThread);

    if (m_hAsyncThread != INVALID_HANDLE_VALUE)
    {
        WaitForSingleObject(m_hAsyncThread, INFINITE);
        CloseHandle(m_hAsyncThread);
    }
}
// PC 0x00408A00 GOG 0x004089D0
int CExoResMan::AddEncapsulatedResourceFile(const CExoString& sFileName,
                                            unsigned long nId)
{
    return AddKeyTable(sFileName, TABLE_ENCAPSULATED, nId);
}
// PC 0x00408A20 GOG 0x004089F0
void CExoResMan::AddResourceImageFile(const CExoString& sFileName)
{
    AddKeyTable(sFileName, TABLE_RESOURCE_IMAGE, 0);
}
// PC 0x00408A40 GOG 0x00408A10
void CExoResMan::AddFixedKeyTableFile(const CExoString& sFileName)
{
    AddKeyTable(sFileName, TABLE_KEY, 0);
}
// PC 0x00408A60 GOG 0x00408A30
void CExoResMan::AddResourceDirectory(const CExoString& sFileName)
{
    AddKeyTable(sFileName, TABLE_DIRECTORY, 0);
}
// PC 0x00406F20 GOG 0x00406EF0
int CExoResMan::AddKeyTable(const CExoString& sFileName, unsigned long nType,
                            unsigned long nId)
{
    CExoLinkedList<CExoKeyTable>* pList;
    unsigned long nIdBase;
    unsigned long nTableId = 0;
#ifndef _XBOX
    unsigned long anUsedIds[2];
#endif
    CExoKeyTable* pTable;

    switch (nType)
    {
    case 1:
        pList = &m_lstKeyTables;
        nIdBase = 0;
        break;
    case 2:
        pList = &m_lstDirectories;
        nIdBase = 0x80000000;
        break;
    case 3:
        pList = &m_lstEncapsulated;
        nIdBase = 0x40000000;
        break;
    case 4:
        pList = &m_lstResourceImages;
        nIdBase = 0x20000000;
        break;
    default:
        return 0;
    }

#ifndef _XBOX
    anUsedIds[0] = 0;
    anUsedIds[1] = 0;
#endif

    CExoLinkedListNode* pPos = pList->GetHeadPos();

    if (pPos != NULL)
    {
        pTable = pList->GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL)
        {
            if (pTable->m_sResourceName == sFileName)
            {
#ifndef _XBOX
                CExoString sMessage;

                if (nType == 4)
                {
                    sMessage.Format(
                        "Table being rebuilt, this RIM is being leaked: %s",
                        sFileName);
                }
#endif

                pTable->RebuildTable();

                return 1;
            }

#ifdef _XBOX
            if (pTable->m_nId > nTableId)
            {
                nTableId = pTable->m_nId;
            }
#else
            unsigned long nUsedId = pTable->m_nId & 0x0FFFFFFF;

            anUsedIds[nUsedId >> 5] |= 1 << (nUsedId & 0x1F);
#endif
        }

        pTable = pList->GetNext(pPos);
    }

#ifdef _XBOX
    EXOXBOXASSERT((nTableId & 0x0FFFFFFF) < 0x0FFFFFFF);

    nTableId = ((nTableId + 1) & 0x0FFFFFFF) | nIdBase;
#else
    for (int i = 1; i < 64; i++)
    {
        if (!(anUsedIds[i >> 5] & (1 << (i & 0x1F))))
        {
            nTableId = i;
            break;
        }
    }

    nTableId |= nIdBase;
#endif

    CExoKeyTable* pNewTable = new CExoKeyTable;
    CExoLinkedListNode* pNode = pList->AddHead(pNewTable);

    int bResult = pNewTable->BuildNewTable(nType, sFileName, nTableId, 1);
    pNewTable->m_nPriority = nId;

    if (bResult == 0)
    {
        pList->Remove(pNode);
        delete pNewTable;
    }

    return bResult;
}
// PC 0x00408A80 GOG 0x00408A50
int CExoResMan::RemoveEncapsulatedResourceFile(const CExoString& sFileName)
{
    return RemoveKeyTable(sFileName, TABLE_ENCAPSULATED);
}
// PC 0x00408A90 GOG 0x00408A60
int CExoResMan::RemoveResourceImageFile(const CExoString& sFileName)
{
    return RemoveKeyTable(sFileName, TABLE_RESOURCE_IMAGE);
}
// PC 0x00408B20 GOG 0x00408AF0
int CExoResMan::RemoveFixedKeyTableFile(const CExoString& sFileName)
{
    return RemoveKeyTable(sFileName, TABLE_KEY);
}
// PC 0x00408B30 GOG 0x00408B00
int CExoResMan::RemoveResourceDirectory(const CExoString& sFileName)
{
    return RemoveKeyTable(sFileName, TABLE_DIRECTORY);
}
// PC 0x00408B40 GOG 0x00408B10
int CExoResMan::UpdateResourceDirectory(const CExoString& sFileName)
{
    return UpdateKeyTable(sFileName, 2);
}
// PC 0x00408B50 GOG 0x00408B20
int CExoResMan::CancelRequest(CRes* pRes)
{
    if (pRes == NULL)
    {
        return 0;
    }

    unsigned long nFlags = pRes->m_nFlags;

    if (nFlags & 0x4)
    {
        return 0;
    }

    if (pRes->m_nRequestCount == 0)
    {
        return 0;
    }

    pRes->m_nRequestCount--;

    if (pRes->m_nRequestCount == 0 && (nFlags & 0x10))
    {
        CExoKeyTable* pTable = GetTable(pRes);

        if (pTable == NULL)
        {
            return 0;
        }

        switch (pRes->m_nResId >> 30)
        {
        case 3:
            if (m_pCurrentAsyncFile != NULL && pRes->m_nRequestCount == 0)
            {
                delete m_pCurrentAsyncFile;
            }
            break;
        case 1:
        case 2:
            pTable->m_pPackedFiles[0]->DeleteRefCount();
            break;
        case 0:
            pTable->m_pPackedFiles[(pRes->m_nResId >> 20) & 0x3FF]
                ->DeleteRefCount();
            break;
        }

        m_pCurrentAsyncRes = NULL;
        m_pCurrentAsyncFile = NULL;
    }

    CExoLinkedListNode* pPos = m_lstRequests.GetHeadPos();
    CRes* pCurrent = m_lstRequests.GetHead();

    while (pPos != NULL)
    {
        if (pCurrent == pRes)
        {
            if (pRes->m_nRequestCount == 0)
            {
                m_lstRequests.Remove(pPos);
            }

            break;
        }

        pCurrent = m_lstRequests.GetNext(pPos);
    }

    return pRes->m_nRequestCount;
}
// PC 0x00408C50 GOG 0x00408C20
void* CExoResMan::Demand(CRes* pRes)
{
    CExoLinkedListNode* pPos;
    CRes* pCurrent;

    if (pRes != NULL && pRes->m_nResId != 0xFFFFFFFF)
    {
        unsigned long nFlags = pRes->m_nFlags;

        if ((nFlags & 0x4) && pRes->m_pResource != NULL)
        {
#ifndef _XBOX
            if (pRes->m_nDemandCount == 0 && (nFlags & 0x100))
            {
                pPos = m_lstReleased.GetHeadPos();
                pCurrent = m_lstReleased.GetHead();

                while (pCurrent != NULL)
                {
                    if (pCurrent == pRes)
                    {
                        m_lstReleased.Remove(pPos);
                        pRes->m_nFlags &= ~0x100;
                        break;
                    }

                    pCurrent = m_lstReleased.GetNext(pPos);
                }
            }
#endif

            pRes->m_nDemandCount++;
            return pRes->m_pResource;
        }

        if (nFlags & 0x10)
        {
            if (m_pCurrentAsyncFile == NULL)
            {
                return pRes->m_pResource;
            }

            while (!m_pCurrentAsyncFile->ReadAsyncComplete())
            {
                Sleep(1);
            }

            if (ServiceCurrentAsyncRes())
            {
                pRes->m_nDemandCount++;
                return pRes->m_pResource;
            }
        }
        else
        {
            int bServiced;

            switch (pRes->m_nResId >> 30)
            {
            case 3:
                bServiced = ServiceFromDirectory(pRes, 0);
                break;
            case 2:
                bServiced = ServiceFromEncapsulated(pRes, 0);
                break;
            case 0:
                bServiced = ServiceFromResFile(pRes, 0);
                break;
            case 1:
                bServiced = ServiceFromImage(pRes, 0);
                break;
            }

            if (bServiced)
            {
                pPos = m_lstRequests.GetHeadPos();
                pCurrent = m_lstRequests.GetHead();

                while (pPos != NULL)
                {
                    if (pCurrent == pRes)
                    {
                        m_lstRequests.Remove(pPos);
                        break;
                    }

                    pCurrent = m_lstRequests.GetNext(pPos);
                }

                pRes->m_nDemandCount++;
                return pRes->m_pResource;
            }
        }
    }

    return NULL;
}
// PC 0x00408DF0 GOG 0x00408DC0
void CExoResMan::Dump(CRes* pRes, int bFlag)
{
    if (pRes != NULL)
    {
        if (pRes->m_nDemandCount > 0)
        {
            pRes->m_nFlags |= 0x100;
        }
        else if ((pRes->m_nFlags & 0x14) && pRes->m_pResource != NULL)
        {
            Free(pRes);
        }
    }
}
// PC 0x00408E20 GOG 0x00408DF0
int CExoResMan::Exists(const CResRef& cResRef, unsigned short nType,
                       unsigned long* pnTableType)
{
    CExoKeyTable* pTable = NULL;
    CKeyTableEntry* pEntry;

    int bExists = GetKeyEntry(cResRef, nType, &pTable, &pEntry);

    if (bExists && pnTableType != NULL)
    {
        *pnTableType = pTable->m_nTableType;
    }

    return bExists;
}
// PC 0x00408E60 GOG 0x00408E30
int CExoResMan::Exists(const CResRef& cResRef, unsigned short nType,
                       unsigned long nTableType)
{
    CExoLinkedList<CExoKeyTable>* pList = NULL;
    CExoKeyTable* pTable;
    CKeyTableEntry* pEntry;

    switch (nTableType)
    {
    case 1:
        pList = &m_lstKeyTables;
        break;
    case 2:
        pList = &m_lstDirectories;
        break;
    case 3:
        pList = &m_lstEncapsulated;
        break;
    case 4:
        pList = &m_lstResourceImages;
        break;
    }

    if (pList != NULL)
    {
        return GetKeyEntryFromTable(*pList, cResRef, nType, &pTable, &pEntry,
                                    0);
    }

    return 0;
}
// PC 0x00408EC0 GOG 0x00408E90
CExoStringList* CExoResMan::GetResOfType(unsigned short nType, CRes* pRes)
{
    EXOXBOXASSERT(pRes);
    CExoKeyTable* pTable = GetTable(pRes);
    CExoStringList* pList = NULL;

    if (pTable != NULL && !pTable->m_bDestroying)
    {
        pList = pTable->GetKeysOfType(nType);
    }

    return pList;
}
// PC 0x00408EF0 GOG 0x00408EC0
int CExoResMan::Release(CRes* pRes)
{
    if (pRes == NULL)
    {
        return -1;
    }

    if ((pRes->m_nResId & 0xC0000000) == 0x40000000)
    {
        CExoKeyTable* pTable = GetTable(pRes);

        pTable->m_pPackedFiles[0]->DeleteRefCount();
    }

    if (pRes->m_nDemandCount > 1)
    {
        return pRes->m_nDemandCount--;
    }

    if (pRes->m_nDemandCount == 0)
    {
        return -1;
    }

    pRes->m_nDemandCount = 0;

    unsigned long nFlags = pRes->m_nFlags;

    if (nFlags & 0x100)
    {
        Dump(pRes, 0);
#ifndef _XBOX
        if (pRes->m_pResource != NULL)
        {
            m_lstReleased.AddHead(pRes);
            Dump(pRes, 0);
        }
#endif
        pRes->m_nFlags &= ~0x100;

        return 0;
    }

    m_lstReleased.AddHead(pRes);
    pRes->m_nFlags |= 0x100;

    return 0;
}
// PC 0x00408FA0 GOG 0x00408F70
void CExoResMan::Update(unsigned long nTime)
{
    int bStarted;

    if (m_bUpdateDisabled)
    {
        return;
    }

    if (m_lstRequests.IsEmpty())
    {
        return;
    }

    if (m_pCurrentAsyncRes == NULL)
    {
        m_pCurrentAsyncRes = m_lstRequests.GetHead();
    }

    unsigned long nResId = m_pCurrentAsyncRes->m_nResId;

    if (nResId == 0xFFFFFFFF)
    {
        m_lstRequests.RemoveHead();
        m_pCurrentAsyncRes = NULL;
        m_pCurrentAsyncFile = NULL;
        return;
    }

    if (m_pCurrentAsyncRes->m_nFlags & 0x4)
    {
        m_lstRequests.RemoveHead();
        m_pCurrentAsyncRes = NULL;
        m_pCurrentAsyncFile = NULL;
        return;
    }

    if (m_pCurrentAsyncRes->m_nFlags & 0x10)
    {
        if (m_pCurrentAsyncFile != NULL && m_pCurrentAsyncFile->ReadAsyncComplete())
        {
#ifdef _XBOX
            m_pCurrentAsyncFile->ReadAsyncBytesRead();
#endif
            ServiceCurrentAsyncRes();
        }
        return;
    }

    switch (nResId >> 30)
    {
    case 3:
        bStarted = ServiceFromDirectory(m_pCurrentAsyncRes, 1);
        break;
    case 2:
        bStarted = ServiceFromEncapsulated(m_pCurrentAsyncRes, 1);
        break;
    case 0:
        bStarted = ServiceFromResFile(m_pCurrentAsyncRes, 1);
        break;
    case 1:
        bStarted = ServiceFromImage(m_pCurrentAsyncRes, 1);
        break;
    }

    if (bStarted)
    {
        if (!(m_pCurrentAsyncRes->m_nFlags & 0x4))
        {
            m_pCurrentAsyncRes->m_nFlags |= 0x10;
        }
    }
}
// PC 0x00409090 GOG 0x00409060
int CExoResMan::ReadRaw(CRes* pRes, int nSize, char* pBuffer)
{
    int bResult;

    switch (pRes->m_nResId >> 30)
    {
    case 3:
        bResult = ServiceFromDirectoryRaw(pRes, nSize, pBuffer);
        break;
    case 2:
        bResult = ServiceFromEncapsulatedRaw(pRes, nSize, pBuffer);
        break;
    case 1:
        bResult = ServiceFromImageRaw(pRes, nSize, pBuffer);
        break;
    case 0:
        bResult = ServiceFromResFileRaw(pRes, nSize, pBuffer);
        break;
    }

    return bResult;
}
// PC 0x004090F0 GOG 0x004090C0
int CExoResMan::WipeDirectory(const CExoString& sPath, int bDeleteFiles,
                              int bRemoveDirectory, int bRecurseClean,
                              int bRecurse)
{
    CExoString sFullPath;
    int bResult = 1;

    if (bDeleteFiles)
    {
        CExoArrayList<CExoString> lstFiles;
        CExoString sFile;

        g_pExoBase->GetDirectoryList(&lstFiles, sPath, RESTYPE_INVALID, 0, 0);

        for (int i = 0; i < lstFiles.num; i++)
        {
            sFile = lstFiles[i];

            unsigned short nType = g_pExoResMan->GetResTypeFromFile(sFile);

            if (nType != 0xFFFF)
            {
                int nDot = sFile.Find('.', 0);

                if (nDot < 0)
                {
                    return 0;
                }

                if (!RemoveFile(sPath + "\\" + sFile.SubString(0, nDot), nType))
                {
                    return 0;
                }
            }
        }
    }

    if (bRecurse)
    {
        CExoArrayList<CExoString> lstDirectories;
        CExoString sDirectory;

        g_pExoBase->GetDirectoryList(&lstDirectories, sPath, RESTYPE_INVALID, 1, 0);

        for (int i = 0; i < lstDirectories.num; i++)
        {
            sDirectory = lstDirectories[i];

            if (!WipeDirectory(sPath + "\\" + sDirectory, bDeleteFiles, 1, 1,
                               1))
            {
                return 0;
            }
        }
    }
    else if (bRecurseClean)
    {
        CExoArrayList<CExoString> lstDirectories;
        CExoString sDirectory;

        g_pExoBase->GetDirectoryList(&lstDirectories, sPath, RESTYPE_INVALID, 1, 0);

        for (int i = 0; i < lstDirectories.num; i++)
        {
            sDirectory = lstDirectories[i];

            if (!WipeDirectory(sPath + "\\" + sDirectory, bDeleteFiles, 0, 1,
                               0))
            {
                return 0;
            }
        }
    }

    if (bRemoveDirectory)
    {
        sFullPath = g_pExoBase->m_pExoAliasList->ResolveFileName(sPath, RESTYPE_INVALID);

        if (sFullPath.GetLength() == 0)
        {
            return 0;
        }

        bResult = RemoveDirectoryA(sFullPath.CStr());
    }

    return bResult;
}
// PC 0x004096A0 GOG 0x00409670
int CExoResMan::CleanDirectory(const CExoString& sPath, int bRecurse,
                               int bRecurseClean)
{
    return WipeDirectory(sPath, 1, 0, bRecurseClean, bRecurse);
}
// PC 0x004096C0 GOG 0x00409690
int CExoResMan::NukeDirectory(const CExoString& sPath, int bDeleteFiles,
                              int bRecurse)
{
    return WipeDirectory(sPath, bDeleteFiles, 1, 0, bRecurse);
}
#ifndef _XBOX
void CExoResMan::AsyncLoad()  // PC 0x004096E0 GOG 0x004096B0
{
    CExoCriticalSection* pCriticalSection = &m_csAsync;
    pCriticalSection->EnterCriticalSection();

    int bResult = 1;
    int bSaveGame = 0;
    CExoString sPath;
    CExoString sName;
    int nDownloadedPackage = FindModuleDownloadedPkg(m_sAsyncModuleName);

    if (m_bAsyncIsModule == 0)
    {
        sPath = CExoString("RIMS:") + m_sAsyncModuleName;
        bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);
    }
    else
    {
        sPath = m_sAsyncModuleName + CExoString("_a");
        if (Exists(sPath, RESTYPE_RIM, (unsigned long*)NULL))
        {
            if (nDownloadedPackage == 0)
            {
                sPath = CExoString("RIMS:") + sPath;
            }
            else
            {
                CExoString sDirectory;
                sDirectory.Format("LIVE%d:RIMSXBOX\\%s", nDownloadedPackage,
                                  sPath.CStr());
                sPath = sDirectory;
            }
            bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);

            if (bResult)
            {
                m_nAsyncLoadedFlags |= LOADED_MODULE_AREA_RIM;
            }
        }

        if (bResult)
        {
            sPath = m_sAsyncModuleName + CExoString("_adx");
            if (Exists(sPath, RESTYPE_RIM, (unsigned long*)NULL))
            {
                if (nDownloadedPackage == 0)
                {
                    sPath = CExoString("RIMS:") + sPath;
                }
                else
                {
                    CExoString sDirectory;
                    sDirectory.Format("LIVE%d:RIMSXBOX\\%s",
                                      nDownloadedPackage, sPath.CStr());
                    sPath = sDirectory;
                }
                bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);

                if (bResult)
                {
                    m_nAsyncLoadedFlags |= LOADED_MODULE_ADX_RIM;
                }
            }

            if (bResult)
            {
                AddKeyTable("MODULES:", TABLE_DIRECTORY, 0);
                if (!Exists(m_sAsyncModuleName, RESTYPE_MOD, (unsigned long*)NULL))
                {
                    sPath = m_sAsyncModuleName + CExoString("_s");
                    if (Exists(sPath, RESTYPE_RIM, (unsigned long*)NULL))
                    {
                        if (nDownloadedPackage == 0)
                        {
                            sPath = CExoString("MODULES:") + sPath;
                        }
                        else
                        {
                            sPath = CExoString("MODULES\\") + sPath;
                            CExoString sDirectory;
                            sDirectory.Format("LIVE%d:%s", nDownloadedPackage,
                                              sPath.CStr());
                            sPath = sDirectory;
                        }
                        bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);

                        if (bResult)
                        {
                            bSaveGame = 1;
                            m_nAsyncLoadedFlags |= LOADED_SAVE_GAME_RIM;
                        }
                    }

                    if (nDownloadedPackage == 0)
                    {
                        sPath = CExoString("MODULES:") + m_sAsyncModuleName +
                                CExoString("_dlg");
                    }
                    else
                    {
                        sPath = CExoString("MODULES\\") + m_sAsyncModuleName +
                                CExoString("_dlg");
                        CExoString sDirectory;
                        sDirectory.Format("LIVE%d:%s", nDownloadedPackage,
                                          sPath.CStr());
                        sPath = sDirectory;
                    }
                    AddKeyTable(CacheResource(sPath, RESTYPE_ERF),
                                TABLE_ENCAPSULATED, PRIORITY_MODULE);
                }
                else
                {
                    if (nDownloadedPackage == 0)
                    {
                        sPath = CExoString("MODULES:") + m_sAsyncModuleName;
                    }
                    else
                    {
                        sPath = CExoString("MODULES\\") + m_sAsyncModuleName;
                        CExoString sDirectory;
                        sDirectory.Format("LIVE%d:%s", nDownloadedPackage,
                                          sPath.CStr());
                        sPath = sDirectory;
                    }
                    bResult = AddKeyTable(sPath, TABLE_ENCAPSULATED, PRIORITY_MODULE);

                    if (bResult)
                    {
                        m_nAsyncLoadedFlags |= LOADED_MODULE_RIM;
                    }
                }

                RemoveKeyTable("MODULES:", TABLE_DIRECTORY);
            }
        }

        if (!m_bAsyncFromCurrentGame && bResult)
        {
            sPath = CExoString("CURRENTGAME:") + m_sAsyncModuleName;

            AddKeyTable("CURRENTGAME:", TABLE_DIRECTORY, 0);
            if (Exists(m_sAsyncModuleName, RESTYPE_RSV, (unsigned long*)NULL) ||
                Exists(m_sAsyncModuleName, RESTYPE_RIM, (unsigned long*)NULL))
            {
                bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);

                if (bResult)
                {
                    m_nAsyncLoadedFlags |= LOADED_CURRENT_GAME_RIM;
                }
            }

            RemoveKeyTable("CURRENTGAME:", TABLE_DIRECTORY);
        }
        else
        {
            sPath = CExoString("CURRENTGAME:") + m_sAsyncModuleName;
            bResult = AddKeyTable(sPath, TABLE_ENCAPSULATED, PRIORITY_MODULE);

            if (bResult)
            {
                m_nAsyncLoadedFlags |= LOADED_CURRENT_GAME_KEY;
            }
        }
    }

    if (bResult)
    {
        m_nAsyncState = (bSaveGame == 0) + ASYNC_MODULE_LOADED;
    }
    else
    {
        m_nAsyncState = ASYNC_LOAD_FAILED;
    }

    pCriticalSection->LeaveCriticalSection();

}
#endif

int CExoResMan::GetNewResRef(const CResRef& cResRef, unsigned short nType,
                             CResRef& cNewResRef)
{
    char szResRef[20];
    char* pPrefix = NULL;
    char* pNumber = NULL;
    int nLength, nSplit, nPrefixLength, nDigits, nNumber, nNext;
    int bFound = 0;

    cNewResRef = cResRef;
    memcpy(szResRef, cNewResRef.GetResRef(), 16);
    szResRef[16] = 0;
    nLength = strlen(szResRef);

    nSplit = nLength;
    nDigits = 0;
    while (nDigits < 8 && nSplit > 0 && szResRef[nSplit - 1] >= '0' &&
           szResRef[nSplit - 1] <= '9')
    {
        nSplit--;
        nDigits++;
    }
    nPrefixLength = nSplit <= 13 ? nSplit : 13;
    if (nPrefixLength != 0)
    {
        pPrefix = (char*)calloc(nPrefixLength + 1, 1);
        strncpy(pPrefix, szResRef, nPrefixLength);
    }
    if (nSplit <= 13 && nLength != nPrefixLength)
    {
        pNumber = (char*)calloc(nLength - nPrefixLength, 1);
        strncpy(pNumber, szResRef + nPrefixLength, nLength - nPrefixLength);
        nNumber = atoi(pNumber);
    }
    else
    {
        pNumber = (char*)calloc(nPrefixLength > 3 ? nPrefixLength : 3, 1);
        nNumber = 0;
    }
    sprintf(pNumber, "%d", nNumber);
    nNext = nNumber + 1;

    if (strlen(pNumber) <= 3)
    {
        while (nNumber >= 0)
        {
            if (pPrefix != NULL)
            {
                sprintf(szResRef, "%s%03d", pPrefix, nNext % 1000);
            }
            else
            {
                sprintf(szResRef, "%03d", nNext);
            }
            cNewResRef = szResRef;

            CExoKeyTable* pTable = NULL;
            CKeyTableEntry* pEntry = NULL;
            int bExists = GetKeyEntry(cNewResRef, nType, &pTable, &pEntry);
            sprintf(pNumber, "%d", nNext++);
            if (!bExists)
            {
                bFound = 1;
                break;
            }
            if (strlen(pNumber) > 3)
            {
                break;
            }
        }
    }

    if (pPrefix != NULL)
    {
        free(pPrefix);
    }
    if (pNumber != NULL)
    {
        free(pNumber);
    }
    return bFound;
}
// PC 0x00409E10 GOG 0x00409DE0
void CRes::CancelRequest()
{
    g_pExoResMan->CancelRequest(this);
}
// PC 0x00409E20 GOG 0x00409DF0
void* CRes::Demand()
{
    return g_pExoResMan->Demand(this);
}
// PC 0x00409E30 GOG 0x00409E00
int CRes::ReadRaw(int nSize, char* pBuffer)
{
    return g_pExoResMan->ReadRaw(this, nSize, pBuffer);
}
// PC 0x00409E50 GOG 0x00409E20
void CRes::Dump(int bFlag)
{
    g_pExoResMan->Dump(this, bFlag);
}
// PC 0x00409E90 GOG 0x00409E60
int CRes::Release()
{
    return g_pExoResMan->Release(this);
}
// PC 0x00409EA0 GOG 0x00409E70
void AsyncRimLoadThreadProc(void* pParameter)
{
    while (g_pExoResMan != NULL)
    {
        if (g_pExoResMan->m_bExitAsyncThread)
        {
            break;
        }

        g_pExoResMan->m_csAsync.EnterCriticalSection();
        g_pExoResMan->AsyncLoad();
        g_pExoResMan->m_csAsync.LeaveCriticalSection();

        SuspendThread(g_pExoResMan->m_hAsyncThread);
    }

    ExitThread(1);
}
// PC 0x00409F00 GOG 0x00409ED0
CExoResMan::CExoResMan()
{
    MEMORYSTATUS msStatus;

    CResGFFFieldIDHash::Initialize();

    m_pCurrentAsyncRes = NULL;
    m_bUpdateDisabled = 0;
    m_bCharGenImageLoaded = 0;
    m_bMainMenuImageLoaded = 0;
    m_nImageLoadedFlagsUnused = 0;

    GlobalMemoryStatus(&msStatus);

    m_nTotalPhysicalMemory = msStatus.dwTotalPhys;

    int nLimit = (int)m_nTotalPhysicalMemory >> 1;

    if (nLimit > 0x1000000)
    {
        m_nMemoryLimit = nLimit;
        m_nAvailableMemory = nLimit;
    }
    else
    {
        m_nMemoryLimit = 0x1000000;
        m_nAvailableMemory = 0x1000000;
    }

    m_nAsyncState = CExoResMan::ASYNC_IDLE;
    m_nAsyncLoadedFlags = 0;
    m_bExitAsyncThread = 0;

    int nParameter = 0;

    m_hAsyncThread = CreateThread(
        NULL, 0x10000, (LPTHREAD_START_ROUTINE)AsyncRimLoadThreadProc,
        &nParameter, CREATE_SUSPENDED, &m_nAsyncThreadId);
}
// PC 0x0040A000 GOG 0x00409FD0
int CExoResMan::ReleaseResObject(CRes* pRes)
{
    if (pRes != NULL)
    {
        if (pRes->m_nRefCount > 1)
        {
            pRes->m_nRefCount--;
            return pRes->m_nRefCount;
        }

        pRes->m_nRefCount = 0;

        if (pRes->m_nDemandCount > 0)
        {
            return 0;
        }

        if (pRes->m_pKeyEntry != NULL)
        {
            pRes->m_pKeyEntry->m_pRes = NULL;
        }

        pRes->Dump(1);

        if (pRes->m_nFlags & 0x8)
        {
            pRes->m_nFlags |= 0x200;
        }
    }

    return 0;
}
// PC 0x0040A050 GOG 0x0040A020
CRes::~CRes()
{
    if (m_nDemandCount > 0)
    {
        m_nFlags |= 0x100;
    }
    else if ((m_nFlags & 0x14) && m_pResource != NULL)
    {
        g_pExoResMan->Free(this);
    }
}
// PC 0x00407120 GOG 0x004070F0
int CExoResMan::Free(CRes* pRes)
{
    if (pRes == NULL || pRes->m_pResource == NULL)
    {
        return 0;
    }

    CExoLinkedListNode* pPos = m_lstReleased.GetHeadPos();
    CRes* pCurrent = m_lstReleased.GetHead();

    if (pCurrent == NULL)
    {
        return 0;
    }

    while (pCurrent != NULL)
    {
        if (pCurrent == pRes)
        {
            pRes->OnResourceFreed();

            if (pRes->m_nFlags & 0x10)
            {
                return 0;
            }

            m_lstReleased.Remove(pPos);
            FreeResourceData(pRes);
            pRes->m_nFlags &= ~0x114;

            return 1;
        }

        pCurrent = m_lstReleased.GetNext(pPos);
    }

    return 0;
}
// PC 0x004071A0 GOG 0x00407170
int CExoResMan::FreeChunk()
{
    unsigned long nFreed = 0;
    CExoLinkedListNode* pPos = m_lstReleased.GetHeadPos();
    CRes* pRes = m_lstReleased.GetHead();

    while (pRes != NULL)
    {
        if (m_lstReleased.GetHeadPos() == NULL || nFreed >= 0x100000)
        {
            break;
        }

        if (pRes->m_nFlags & 0x10)
        {
            pRes = m_lstReleased.GetNext(pPos);
        }
        else if (pRes->m_nDemandCount > 0)
        {
            pRes->m_nFlags &= ~0x100;

            CExoLinkedListNode* pRemove = pPos;

            pRes = m_lstReleased.GetNext(pPos);
            m_lstReleased.Remove(pRemove);
        }
        else
        {
            pRes->OnResourceFreed();

            nFreed -= m_nAvailableMemory;
            FreeResourceData(pRes);
            nFreed += m_nAvailableMemory;

            pRes->m_nFlags &= ~0x114;

            CExoLinkedListNode* pRemove = pPos;

            pRes = m_lstReleased.GetNext(pPos);
            m_lstReleased.Remove(pRemove);
        }
    }

    return nFreed > 0;
}
// PC 0x004072A0 GOG 0x00407270
int CExoResMan::GetKeyEntryFromTable(CExoLinkedList<CExoKeyTable>& lstTables,
                                     const CResRef& cResRef,
                                     unsigned short nType,
                                     CExoKeyTable** ppTable,
                                     CKeyTableEntry** ppEntry,
                                     unsigned long nId)
{
    CKeyTableEntry* pEntry;
    CExoKeyTable* pTable;

    CExoLinkedListNode* pPos = lstTables.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = lstTables.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if ((nId == 0 || pTable->m_nPriority == nId) && pTable != NULL &&
            !pTable->m_bDestroying)
        {
            pEntry = pTable->FindKey(cResRef, nType);

            if (pEntry != NULL)
            {
                *ppTable = pTable;
                *ppEntry = pEntry;
                return 1;
            }
        }

        pTable = lstTables.GetNext(pPos);
    }

    return 0;
}
// PC 0x00407330 GOG 0x00407300
int CExoResMan::GetKeyEntry(const CResRef& cResRef, unsigned short nType,
                            CExoKeyTable** ppTable, CKeyTableEntry** ppEntry)
{
    if (GetKeyEntryFromTable(m_lstDirectories, cResRef, nType, ppTable, ppEntry,
                             0))
    {
        return 1;
    }

    if (GetKeyEntryFromTable(m_lstEncapsulated, cResRef, nType, ppTable,
                             ppEntry, 1))
    {
        return 1;
    }

    if (GetKeyEntryFromTable(m_lstResourceImages, cResRef, nType, ppTable,
                             ppEntry, 0))
    {
        return 1;
    }

    if (GetKeyEntryFromTable(m_lstEncapsulated, cResRef, nType, ppTable,
                             ppEntry, 2))
    {
        return 1;
    }

    if (GetKeyEntryFromTable(m_lstKeyTables, cResRef, nType, ppTable, ppEntry,
                             0))
    {
        return 1;
    }

    *ppTable = NULL;
    *ppEntry = NULL;

    return 0;
}
// PC 0x004073E0 GOG 0x004073B0
CExoStringList* CExoResMan::GetResOfTypeFromTable(
    CExoLinkedList<CExoKeyTable>& lstTables, unsigned short nType)
{
    CExoStringList* pList = NULL;

    CExoLinkedListNode* pPos = lstTables.GetHeadPos();

    if (pPos != NULL)
    {
        CExoKeyTable* pTable = lstTables.GetAtPos(pPos);

        while (pPos != NULL)
        {
            if (pTable != NULL && !pTable->m_bDestroying)
            {
                if (pList != NULL)
                {
                    CExoStringList* pTableList = pTable->GetKeysOfType(nType);
                    CExoStringList* pMerged = pList->Merge(pTableList, 0, 1);

                    delete pList;
                    delete pTableList;

                    pList = pMerged;
                }
                else
                {
                    pList = pTable->GetKeysOfType(nType);
                }
            }

            pTable = lstTables.GetNext(pPos);
        }
    }

    return pList;
}
// PC 0x00407490 GOG 0x00407460
CExoStringList* CExoResMan::GetResOfType(unsigned short nType,
                                         int bEncapsulatedOnly)
{
    CExoStringList* pList = GetResOfTypeFromTable(m_lstEncapsulated, nType);

    if (bEncapsulatedOnly)
    {
        return pList;
    }

    if (pList != NULL)
    {
        CExoStringList* pImageList =
            GetResOfTypeFromTable(m_lstResourceImages, nType);
        CExoStringList* pMerged = pList->Merge(pImageList, 0, 1);

        delete pList;
        delete pImageList;

        pList = pMerged;
    }
    else
    {
        pList = GetResOfTypeFromTable(m_lstResourceImages, nType);
    }

    if (pList != NULL)
    {
        CExoStringList* pDirectoryList =
            GetResOfTypeFromTable(m_lstDirectories, nType);
        CExoStringList* pMerged = pList->Merge(pDirectoryList, 0, 1);

        delete pList;
        delete pDirectoryList;

        pList = pMerged;
    }
    else
    {
        pList = GetResOfTypeFromTable(m_lstDirectories, nType);
    }

    if (pList != NULL)
    {
        CExoStringList* pKeyTableList =
            GetResOfTypeFromTable(m_lstKeyTables, nType);
        CExoStringList* pMerged = pList->Merge(pKeyTableList, 0, 1);

        delete pList;
        delete pKeyTableList;

        return pMerged;
    }

    return GetResOfTypeFromTable(m_lstKeyTables, nType);
}
// PC 0x004075D0 GOG 0x004075A0
CRes* CExoResMan::GetResObject(const CResRef& cResRef, unsigned short nType)
{
    CExoKeyTable* pTable = NULL;
    CKeyTableEntry* pEntry = NULL;

    if (GetKeyEntry(cResRef, nType, &pTable, &pEntry))
    {
        if (pEntry->m_pRes != NULL)
        {
            pEntry->m_pRes->m_nRefCount++;
        }

        if (pTable->m_nTableType != 4 &&
            (pEntry->m_pRes == NULL || pEntry->m_pRes->m_nRefCount == 1))
        {
            CExoString sResRef;
            CExoString sTableType;
            CExoString sExtension;
            int bReport = TRUE;

            switch (pTable->m_nTableType)
            {
            case 1:
                sTableType = "BIF";
                bReport = FALSE;
                break;
            case 2:
                CExoKeyTable* pImageTable;
                CKeyTableEntry* pImageEntry;

                if (GetKeyEntryFromTable(m_lstResourceImages, cResRef, nType,
                                         &pImageTable, &pImageEntry, 0))
                {
                    bReport = FALSE;
                }
                else
                {
                    sTableType = "DIR";
                }
                break;
            case 3:
                sTableType = "ERF";
                break;
            }

            if (bReport)
            {
                sExtension = g_pExoBase->GetResourceExtension(pEntry->m_nType);
                cResRef.CopyToString(sResRef);

#ifdef _XBOX
                pTable->m_sResourceName.CStr();
                sTableType.CStr();
                sExtension.CStr();
                sResRef.CStr();
#else
                sResRef.CStr();
                sExtension.CStr();
                sTableType.CStr();
                pTable->m_sResourceName.CStr();
#endif
            }
        }

        return pEntry->m_pRes;
    }

    return NULL;
}
// PC 0x00407780 GOG 0x00407750
void CExoResMan::SetResObject(const CResRef& cResRef, unsigned short nType,
                              CRes* pRes)
{
    CExoKeyTable* pTable;
    CKeyTableEntry* pEntry = NULL;

    if (pRes != NULL && GetKeyEntry(cResRef, nType, &pTable, &pEntry))
    {
        if (pEntry->m_pRes == NULL)
        {
            pEntry->m_pRes = pRes;
            pRes->m_pKeyEntry = pEntry;
            pEntry->m_pRes->m_nRefCount = 1;
            pEntry->m_pRes->m_nResId = pEntry->m_nResId;
        }
    }
}
// PC 0x004077E0 GOG 0x004077B0
CExoKeyTable* CExoResMan::GetTable(CRes* pRes)
{
    CExoLinkedList<CExoKeyTable>* pList;

    switch (pRes->m_nResId >> 30)
    {
    case 3:
        pList = &m_lstDirectories;
        break;
    case 2:
        pList = &m_lstEncapsulated;
        break;
    case 0:
        pList = &m_lstKeyTables;
        break;
    case 1:
        pList = &m_lstResourceImages;
        break;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = pList->GetHeadPos();

    if (pPos != NULL)
    {
        pTable = pList->GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & 0x0FFFFFFF) == ((pRes->m_nResId >> 14) & 0x3F))
        {
            return pTable;
        }

        pTable = pList->GetNext(pPos);
    }

    return NULL;
}
// PC 0x00407880 GOG 0x00407850
int CExoResMan::GetTableCount(CRes* pRes, int bCountAll)
{
    CExoKeyTable* pTable = GetTable(pRes);

    if (pTable != NULL)
    {
        return pTable->GetEntryCount(bCountAll);
    }

    return -1;
}

#ifdef _XBOX

int g_nContiguousResourceMemory;
#endif
// PC 0x004078B0 GOG 0x00407880
int CExoResMan::Malloc(CRes* pRes)
{
    while ((int)pRes->m_nSize > m_nAvailableMemory && FreeChunk())
    {
    }

    m_nAvailableMemory -= pRes->m_nSize;

#ifdef _XBOX
    if (pRes->m_bAllocPrefix)
    {
        if (pRes->m_pKeyEntry->m_nType == RESTYPE_MDX)
        {
            pRes->m_pResource = D3D_AllocContiguousMemory(pRes->m_nSize + 6, 128);
            g_nContiguousResourceMemory += pRes->m_nSize + 6;
        }
        else
        {
            pRes->m_pResource = new char[pRes->m_nSize + 6];
        }
        pRes->m_pResource = (char*)pRes->m_pResource + 6;
    }
    else if (pRes->m_bAllocSuffix)
    {
        if (pRes->m_pKeyEntry->m_nType == RESTYPE_TPC)
        {
            pRes->m_pResource = D3D_AllocContiguousMemory(pRes->m_nSize + 10, 128);
            g_nContiguousResourceMemory += pRes->m_nSize + 10;
        }
        else
        {
            pRes->m_pResource = new char[pRes->m_nSize + 10];
        }
    }
#else
    if (pRes->m_bAllocPrefix)
    {
        pRes->m_pResource = new char[pRes->m_nSize + 6] + 6;
    }
    else if (pRes->m_bAllocSuffix)
    {
        pRes->m_pResource = new char[pRes->m_nSize + 10];
    }
#endif
    else
    {
        pRes->m_pResource = new char[pRes->m_nSize];
    }

    if (pRes->m_pResource == NULL)
    {
        m_nAvailableMemory += pRes->m_nSize;
        return 0;
    }

    return 1;
}
// PC 0x00407930 GOG 0x00407900
int CExoResMan::RemoveKeyTable(const CExoString& sFileName, unsigned long nType)
{
    CExoLinkedList<CExoKeyTable>* pList;

    switch (nType)
    {
    case 1:
        pList = &m_lstKeyTables;
        break;
    case 2:
        pList = &m_lstDirectories;
        break;
    case 3:
        pList = &m_lstEncapsulated;
        break;
    case 4:
        pList = &m_lstResourceImages;
        break;
    default:
        return 0;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = pList->GetHeadPos();

    if (pPos != NULL)
    {
        pTable = pList->GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL && pTable->m_sResourceName == sFileName)
        {
            pTable->DestroyTable();
            pList->Remove(pPos);
            delete pTable;

            return 1;
        }

        pTable = pList->GetNext(pPos);
    }

    return 0;
}
// PC 0x004079F0 GOG 0x004079C0
int CExoResMan::ServiceFromDirectory(CRes* pRes, int bAsync)
{
    CExoString sFullPath;
    CExoString sResRef;

    if (pRes == NULL)
    {
        return 0;
    }

    if (pRes->m_nFlags & CRes::FLAG_SERVICED)
    {
        return 0;
    }

    if (pRes->m_pKeyEntry == NULL)
    {
        return 0;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = m_lstDirectories.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = m_lstDirectories.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & CExoKeyTable::TABLE_ID_MASK) ==
                ((pRes->m_nResId >> CRes::RESID_TABLE_SHIFT) & CRes::RESID_TABLE_MASK))
        {
            pRes->m_pKeyEntry->m_resRef.CopyToString(sResRef);
            if (pRes->m_pKeyEntry->m_sDirectory.IsEmpty())
            {
                sFullPath = pTable->m_sResourceName + "\\" + sResRef;
            }
            else
            {
                CExoString sDirectory(pRes->m_pKeyEntry->m_sDirectory);
                sFullPath = pTable->m_sResourceName + "\\" + sDirectory + "\\" + sResRef;
            }

            if (pTable->m_sResourceName == "REBOOTDATA:")
            {
#ifdef _XBOX
                if (!VerifySignatureFile(sFullPath, pRes->m_pKeyEntry->m_nType,
                                         0, 0))
                {
                    return 0;
                }
#endif

#ifdef _DEBUG
                EXOLOG("Loading %s from the reboot directory\n", sFullPath.CStr());
#endif
            }

            CExoFile* pFile =
                new CExoFile(sFullPath, pRes->m_pKeyEntry->m_nType, "rb");

            if (pFile == NULL)
            {
                return 0;
            }

            if (!pFile->FileOpened())
            {
                delete pFile;
                return 0;
            }

            unsigned long nSize = pFile->GetSize();

            pRes->m_nSize = nSize;

            if (nSize == 0)
            {
                delete pFile;
                return 0;
            }

            if (!Malloc(pRes))
            {
                delete pFile;
                return 0;
            }

            if (bAsync)
            {
                m_pCurrentAsyncFile = pFile;
                pFile->ReadAsync(pRes->m_pResource, nSize, 1);
                return 1;
            }

            pFile->Read(pRes->m_pResource, nSize, 1);
            delete pFile;

            int bServiced = pRes->OnResourceServiced();

            pRes->m_nFlags = (pRes->m_nFlags & ~CRes::FLAG_SERVICED) | (bServiced ? CRes::FLAG_SERVICED : 0);

            return bServiced;
        }
        pTable = m_lstDirectories.GetNext(pPos);
    }

    return 0;
}
// PC 0x00407DE0 GOG 0x00407DB0
int CExoResMan::ServiceFromEncapsulated(CRes* pRes, int bAsync)
{
    if (pRes == NULL)
    {
        return 0;
    }

    if (pRes->m_nFlags & 0x4)
    {
        return 0;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = m_lstEncapsulated.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = m_lstEncapsulated.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & 0x0FFFFFFF) == ((pRes->m_nResId >> 14) & 0x3F))
        {
            if (bAsync)
            {
                pTable->m_pPackedFiles[0]->AddAsyncRefCount();

                unsigned long nSize =
                    pTable->m_pPackedFiles[0]->GetResourceSize(pRes->m_nResId &
                                                               0x3FFF);

                pRes->m_nSize = nSize;

                if (!Malloc(pRes))
                {
                    pTable->m_pPackedFiles[0]->DeleteAsyncRefCount();
                    return 0;
                }

                m_pCurrentAsyncFile = pTable->m_pPackedFiles[0]->GetAsyncFile();
                pTable->m_pPackedFiles[0]->ReadResourceAsync(
                    pRes->m_nResId, pRes->m_pResource, nSize, 0);
                return 1;
            }

            pTable->m_pPackedFiles[0]->AddRefCount();

            unsigned long nSize = pTable->m_pPackedFiles[0]->GetResourceSize(
                pRes->m_nResId & 0x3FFF);

            pRes->m_nSize = nSize;

            if (!Malloc(pRes))
            {
                pTable->m_pPackedFiles[0]->DeleteRefCount();
                return 0;
            }

            if (pTable->m_pPackedFiles[0]->ReadResource(
                    pRes->m_nResId, pRes->m_pResource, nSize, 0) != nSize)
            {
                pTable->m_pPackedFiles[0]->DeleteRefCount();
                return 0;
            }

            pTable->m_pPackedFiles[0]->DeleteRefCount();

            int bServiced = pRes->OnResourceServiced();

            pRes->m_nFlags = (pRes->m_nFlags & ~0x4) | (bServiced ? 0x4 : 0);

            return bServiced;
        }

        pTable = m_lstEncapsulated.GetNext(pPos);
    }

    return 0;
}
// PC 0x00407F60 GOG 0x00407F30
int CExoResMan::ServiceFromImage(CRes* pRes, int bAsync)
{
    if (pRes == NULL)
    {
        return 0;
    }

    if (pRes->m_nFlags & 0x4)
    {
        return 0;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = m_lstResourceImages.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = m_lstResourceImages.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & 0x0FFFFFFF) == ((pRes->m_nResId >> 14) & 0x3F))
        {
            unsigned long nIndex = pRes->m_nResId & 0x3FFF;

            pRes->m_nSize = pTable->m_pPackedFiles[0]->GetResourceSize(nIndex);
            pRes->m_pResource =
                ((CExoResourceImageFile*)pTable->m_pPackedFiles[0])
                    ->GetResource(nIndex);
            pRes->m_nFlags |= 0x4;

            return pRes->OnResourceServiced();
        }

        pTable = m_lstResourceImages.GetNext(pPos);
    }

    return 0;
}
// PC 0x00408010 GOG 0x00407FE0
int CExoResMan::ServiceFromResFile(CRes* pRes, int bAsync)
{
    if (pRes == NULL)
    {
        return 0;
    }

    if (pRes->m_nFlags & 0x4)
    {
        return 0;
    }

    unsigned long nFile = (pRes->m_nResId >> 20) & 0x3FF;

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = m_lstKeyTables.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = m_lstKeyTables.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & 0x0FFFFFFF) == ((pRes->m_nResId >> 14) & 0x3F))
        {
            if (bAsync)
            {
                pTable->m_pPackedFiles[nFile]->AddAsyncRefCount();

                unsigned long nSize =
                    pTable->m_pPackedFiles[nFile]->GetResourceSize(
                        pRes->m_nResId & 0x3FFF);

                pRes->m_nSize = nSize;

                if (!Malloc(pRes))
                {
                    pTable->m_pPackedFiles[nFile]->DeleteAsyncRefCount();
                    return 0;
                }

                m_pCurrentAsyncFile =
                    pTable->m_pPackedFiles[nFile]->GetAsyncFile();
                pTable->m_pPackedFiles[nFile]->ReadResourceAsync(
                    pRes->m_nResId, pRes->m_pResource, nSize, 0);
                return 1;
            }

            pTable->m_pPackedFiles[nFile]->AddRefCount();

            unsigned long nSize =
                pTable->m_pPackedFiles[nFile]->GetResourceSize(pRes->m_nResId &
                                                               0x3FFF);

            pRes->m_nSize = nSize;

            if (!Malloc(pRes))
            {
                pTable->m_pPackedFiles[nFile]->DeleteRefCount();
                return 0;
            }

            if (pTable->m_pPackedFiles[nFile]->ReadResource(
                    pRes->m_nResId, pRes->m_pResource, nSize, 0) != nSize)
            {
                pTable->m_pPackedFiles[nFile]->DeleteRefCount();
                return 0;
            }

            pTable->m_pPackedFiles[nFile]->DeleteRefCount();

            int bServiced = pRes->OnResourceServiced();

            if (bServiced)
            {

                pRes->m_pKeyEntry->m_resRef.GetResRefStr();
#ifdef _XBOX
                pTable->m_pPackedFiles[nFile]->m_sFileName.CStr();
                g_pExoBase->GetResourceExtension(pRes->m_pKeyEntry->m_nType)
                    .CStr();
#else
                g_pExoBase->GetResourceExtension(pRes->m_pKeyEntry->m_nType)
                    .CStr();
                pTable->m_pPackedFiles[nFile]->m_sFileName.CStr();
#endif
                pRes->m_pKeyEntry->m_resRef.GetResRefStr();
            }

            pRes->m_nFlags = (pRes->m_nFlags & ~0x4) | (bServiced ? 0x4 : 0);

            return bServiced;
        }

        pTable = m_lstKeyTables.GetNext(pPos);
    }

    return 0;
}
// PC 0x004081F0 GOG 0x004081C0
int CExoResMan::UpdateKeyTable(const CExoString& sFileName, unsigned long nType)
{
    CExoLinkedList<CExoKeyTable>* pList;

    switch (nType)
    {
    case 1:
        pList = &m_lstKeyTables;
        break;
    case 2:
        pList = &m_lstDirectories;
        break;
    case 3:
        pList = &m_lstEncapsulated;
        break;
    case 4:
        pList = &m_lstResourceImages;
        break;
    default:
        return 0;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = pList->GetHeadPos();

    if (pPos != NULL)
    {
        pTable = pList->GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL && sFileName.CompareNoCase(pTable->m_sResourceName))
        {
            pTable->RebuildTable();
            return 1;
        }

        pTable = pList->GetNext(pPos);
    }

    return 0;
}
// PC 0x00408290 GOG 0x00408260
int CExoResMan::ServiceFromDirectoryRaw(CRes* pRes, int nSize, char* pBuffer)
{
    CExoString sFullPath;
    CExoString sResRef;

    if (pRes == NULL)
    {
        return 0;
    }

    if (pRes->m_pKeyEntry == NULL)
    {
        return 0;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = m_lstDirectories.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = m_lstDirectories.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & 0x0FFFFFFF) == ((pRes->m_nResId >> 14) & 0x3F))
        {
            pRes->m_pKeyEntry->m_resRef.CopyToString(sResRef);
            sFullPath = pTable->m_sResourceName + "\\" + sResRef;

            CExoFile* pFile =
                new CExoFile(sFullPath, pRes->m_pKeyEntry->m_nType, "rb");

            if (pFile == NULL)
            {
                return 0;
            }

            if (!pFile->FileOpened())
            {
                delete pFile;
                return 0;
            }

            pRes->m_nSize = nSize;
            pFile->Read(pBuffer, nSize, 1);
            delete pFile;

            return 1;
        }

        pTable = m_lstDirectories.GetNext(pPos);
    }

    return 0;
}
// PC 0x00406010 GOG 0x00405FE0
void CResRef::CopyToString(CExoString& sString) const
{
    char szResRef[17];

    memcpy(szResRef, m_resRef, 16);
    szResRef[16] = 0;
    sString = szResRef;
}
// PC 0x00406050 GOG 0x00406020
void CResRef::CopyToString(char* pString) const
{
    memcpy(pString, m_resRef, 16);
    pString[16] = 0;
}
// PC 0x00406080 GOG 0x00406050
char* CResRef::GetResRefStr() const
{
    s_nCurBufferIndex = (s_nCurBufferIndex + 1) % 4;
    memcpy(RETURN_BUFFERS[s_nCurBufferIndex], m_resRef, 16);
    RETURN_BUFFERS[s_nCurBufferIndex][16] = 0;
    return RETURN_BUFFERS[s_nCurBufferIndex];
}
// PC 0x004060D0 GOG 0x004060A0
int CResRef::IsValid() const
{
    return m_resRef[0] != 0;
}
// PC 0x004060E0 GOG 0x004060B0
unsigned char CResRef::GetLength() const
{
    unsigned char nLength = 0;
    while (nLength < 16 && m_resRef[nLength])
    {
        ++nLength;
    }
    return nLength;
}
// PC 0x00406100 GOG 0x004060D0
int CResRef::operator!=(char* pString) const
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        int nChar = tolower((signed char)pString[i]);
        if (nChar != tolower(m_resRef[i]))
        {
            return 1;
        }
        if (!nChar)
        {
            break;
        }
    }
    return 0;
}
// PC 0x00406150 GOG 0x00406120
int CResRef::operator==(const CResRef& other) const
{
    return memcmp(other.m_resRef, m_resRef, 16) == 0;
}
// PC 0x00406170 GOG 0x00406140
int CResRef::operator==(char* pString) const
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        int nChar = tolower((signed char)pString[i]);
        if (nChar != tolower(m_resRef[i]))
        {
            return 0;
        }
        if (!nChar)
        {
            break;
        }
    }
    return 1;
}
// PC 0x004061C0 GOG 0x00406190
CResRef CResRef::operator=(const CResRef& other)
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        m_resRef[i] = (unsigned char)tolower(other.m_resRef[i]);
    }
    return *this;
}
// PC 0x00406210 GOG 0x004061E0
CResRef CResRef::operator=(unsigned char* pResRef)
{
    memcpy(m_resRef, pResRef, 16);
    for (unsigned int i = 0; i < 16; ++i)
    {
        if (!m_resRef[i])
        {
            memset(m_resRef + i + 1, 0, 15 - i);
            break;
        }
        m_resRef[i] = (unsigned char)tolower(m_resRef[i]);
    }
    return *this;
}
// PC 0x004062D0 GOG 0x004062A0
CResRef CResRef::operator=(const CExoString& sString)
{
    unsigned int nLength = sString.GetLength();

    for (unsigned int i = 0; i < 16; ++i)
    {
        m_resRef[i] = 0;
    }

    if (nLength > 0)
    {
        const char* pString = sString.CStr();

        memcpy(m_resRef, pString, nLength > 16 ? 16 : nLength);

        for (unsigned int i = 0; i < (nLength > 16 ? 16 : nLength); ++i)
        {
            m_resRef[i] = (unsigned char)tolower(m_resRef[i]);
        }
    }

    return *this;
}
// PC 0x00406380 GOG 0x00406350
CResRef CResRef::operator=(char* pString)
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        m_resRef[i] = 0;
    }
    if (pString)
    {
        EXOXBOXASSERT(strlen(pString) <= 16);
        for (unsigned int i = 0; i < 16; ++i)
        {
            if (!pString[i])
            {
                break;
            }
            m_resRef[i] = (unsigned char)tolower((signed char)pString[i]);
        }
    }
    return *this;
}
// PC 0x004063F0 GOG 0x004063C0
CResRef CResRef::operator+=(const CExoString& sString)
{
    CExoString sTemp;
    char szResRef[17];

    memcpy(szResRef, m_resRef, 16);
    szResRef[16] = 0;
    sTemp = szResRef;
    sTemp = sTemp + sString;
    sTemp = sTemp.LowerCase();

    for (unsigned int i = 0; i < 16; ++i)
    {
        m_resRef[i] = 0;
    }
    if (sTemp.GetLength() > 0)
    {
        memcpy(m_resRef, sTemp.CStr(),
               sTemp.GetLength() > sizeof(m_resRef) ? sizeof(m_resRef)
                                                    : sTemp.GetLength());
    }
    return *this;
}
// PC 0x00406560 GOG 0x00406530
CRes::CRes()
{
    m_nDemandCount = 0;
    m_nResId = 0xFFFFFFFF;
    m_nRequestCount = 0;
    m_nSize = 0;
    m_pResource = NULL;
    m_pKeyEntry = NULL;
    m_nRefCount = 0;
    m_bAllocPrefix = 0;
    m_bAllocSuffix = 0;
    m_nFlags = 0;
}
// PC 0x004065B0 GOG 0x00406580
void* CRes::GetData()
{
    return m_pResource;
}
// PC 0x00406600 GOG 0x004065D0
unsigned long CRes::GetSize()
{
    return m_nSize;
}
// PC 0x004065C0 GOG 0x00406590
int CRes::GetDemands()
{
    return m_nDemandCount;
}
// PC 0x004065D0 GOG 0x004065A0
void CRes::GetResRef(CResRef& cResRef, unsigned short& nType)
{
    cResRef = m_pKeyEntry->m_resRef;
    nType = m_pKeyEntry->m_nType;
}
// PC 0x00406610 GOG 0x004065E0
void CExoResMan::AddResourceImageFilesAsync(const CExoString& sModuleName,
                                            int bIsModule, int bFromCurrentGame)
{
    while (m_nAsyncState == CExoResMan::ASYNC_LOAD_REQUESTED)
    {
    }

    m_csAsync.EnterCriticalSection();

    m_sAsyncModuleName = sModuleName;
    m_bAsyncIsModule = bIsModule;
    m_bAsyncFromCurrentGame = bFromCurrentGame;
    m_nAsyncState = CExoResMan::ASYNC_LOAD_REQUESTED;
    m_nAsyncLoadedFlags = 0;

    ResumeThread(m_hAsyncThread);

    m_csAsync.LeaveCriticalSection();
}
// PC 0x00406660 GOG 0x00406630
void CExoResMan::FreeResourceData(CRes* pRes)
{
    if (pRes != NULL)
    {
        if (pRes->m_pResource != NULL)
        {
            if ((pRes->m_nResId & 0xC0000000) != 0x40000000)
            {
#ifdef _XBOX
                if (pRes->m_bAllocPrefix)
                {
                    if (pRes->m_pKeyEntry->m_nType == RESTYPE_MDX)
                    {
                        g_nContiguousResourceMemory -= pRes->m_nSize + 6;
                        D3D_FreeContiguousMemory((char*)pRes->m_pResource - 6);
                    }
                    else
                    {
                        delete[] ((char*)pRes->m_pResource - 6);
                    }
                }
                else
                {
                    if (pRes->m_pKeyEntry->m_nType == RESTYPE_TPC)
                    {
                        g_nContiguousResourceMemory -= pRes->m_nSize + 10;
                        D3D_FreeContiguousMemory(pRes->m_pResource);
                    }
                    else
                    {
                        delete[] (char*)pRes->m_pResource;
                    }
                }
#else
                if (pRes->m_bAllocPrefix)
                {
                    delete[] ((char*)pRes->m_pResource - 6);
                }
                else
                {
                    delete[] (char*)pRes->m_pResource;
                }
#endif
                m_nAvailableMemory += pRes->m_nSize;
            }

            pRes->m_nSize = 0;
            pRes->m_pResource = NULL;
        }
    }
}
// PC 0x004066C0 GOG 0x00406690
void CExoResMan::GetResRefFromFile(CResRef& cResRef,
                                   const CExoString& sFileName)
{
    int nDot = sFileName.Find('.', 0);

    if (nDot <= 0)
    {
        cResRef = "";
    }
    else
    {
        if (nDot > 16)
        {
            nDot = 16;
        }

        cResRef = sFileName.SubString(0, nDot);
    }
}
// PC 0x00406770 GOG 0x00406740
unsigned short CExoResMan::GetResTypeFromFile(const CExoString& sFileName)
{
    CExoString sExtension;

    int nDot = sFileName.Find('.', 0);

    if (nDot < 0)
    {
        return 0xFFFF;
    }

    sExtension = sFileName.SubString(nDot + 1, 3);

    return g_pExoBase->GetResTypeFromExtension(sExtension);
}
// PC 0x00406840 GOG 0x00406810
int CExoResMan::GetIsStaticType(unsigned short nType)
{
    switch (nType)
    {
    case 2009:
    case RESTYPE_NCS:
    case 2012:
    case 2025:
    case 2027:
    case 2029:
    case 2030:
    case 2032:
    case 2035:
    case 2040:
    case 2042:
    case 2044:
    case 2046:
    case 2051:
    case 2056:
    case 2058:
    case 3003:
        return 1;
    }

    return 0;
}
// PC 0x004068D0 GOG 0x004068A0
int CExoResMan::GetFreeDiskSpace(const CExoString& sPath,
                                 unsigned __int64* pnFreeBytes)
{
    CExoString sFullPath;
    ULARGE_INTEGER nFreeBytesAvailable;
    ULARGE_INTEGER nTotalFreeBytes;
    ULARGE_INTEGER nTotalBytes;

    sFullPath = g_pExoBase->m_pExoAliasList->ResolveFileName(sPath, RESTYPE_INVALID);

    if (sFullPath.GetLength() == 0)
    {
        return 0;
    }

    int bResult;

#ifdef _XBOX
    bResult = GetDiskFreeSpaceExA(sFullPath.CStr(), &nFreeBytesAvailable,
                                  &nTotalBytes, &nTotalFreeBytes);
    GetLastError();
    if (bResult)
    {
        *pnFreeBytes = nFreeBytesAvailable.QuadPart;
    }
#else
    if (GetProcAddress(GetModuleHandleA("kernel32.dll"),
                       "GetDiskFreeSpaceExA") != NULL)
    {
        GetDiskFreeSpaceExA(sFullPath.CStr(), &nFreeBytesAvailable,
                            &nTotalBytes, &nTotalFreeBytes);
        bResult = 1;
        *pnFreeBytes = nFreeBytesAvailable.QuadPart;
    }
    else
    {
        bResult = 0;
    }
#endif

    return bResult;
}
// PC 0x004069F0 GOG 0x004069C0
int CExoResMan::CreateDirectory2(const CExoString& sPath)
{
    CExoString sFullPath;

    sFullPath = g_pExoBase->m_pExoAliasList->ResolveFileName(sPath, RESTYPE_INVALID);

    if (sFullPath.GetLength() == 0)
    {
        return 0;
    }

    return CreateDirectoryA(sFullPath.CStr(), NULL);
}
// PC 0x00406AD0 GOG 0x00406AA0
int CExoResMan::FileCopy(const CExoString& sSource, const CExoString& sDest,
                         unsigned short nSourceType, unsigned short nDestType)
{
    if (sSource == sDest)
    {
        return 0;
    }

    if (nDestType == 0xFFFF)
    {
        nDestType = nSourceType;
    }

    CExoFile fSource(sSource, nSourceType, "rb");
    CExoFile fDest(sDest, nDestType, "wb");

    if (fSource.FileOpened() && fDest.FileOpened())
    {
        char* pBuffer = new char[fSource.GetSize()];

        fSource.Read(pBuffer, fSource.GetSize(), 1);
        fDest.Write(pBuffer, fSource.GetSize(), 1);
        fDest.Flush();

        delete[] pBuffer;

        return 1;
    }

    return 0;
}
// PC 0x00406C60 GOG 0x00406C30
int CExoResMan::RemoveFile(const CExoString& sFileName, unsigned short nType)
{
    CExoString sFullPath;
    struct stat stStatus;

    sFullPath = g_pExoBase->m_pExoAliasList->ResolveFileName(sFileName, nType);

    if (sFullPath.GetLength() == 0)
    {
        return 0;
    }
#ifdef _XBOX
    return DeleteFile(sFullPath.CStr()) != 0;
#else
    if (stat(sFullPath.CStr(), &stStatus) == -1)
    {
        return 0;
    }
    if (stStatus.st_mode & _S_IFDIR)
    {
        return 0;
    }
    chmod(sFullPath.CStr(), _S_IWRITE);
    return remove(sFullPath.CStr()) != -1;
#endif
}

#ifdef _XBOX

#define RESTYPE_SIG 3010
#define SIGNATURE_SIZE 20
#include "appmanager.h"
#include "clientexoapp.h"
#include "aurora.h"
#include "aurgui.h"

void AurFlushPostedStrings();

void CExoResMan::ReportMissingFile(const CExoString& sFileName,
                                   unsigned short nType, int bFatal)
{
    bool bReport =
        bFatal || nType == 9998 || nType == 3002 ||
        (nType == 9999 ? sFileName.Right(6).CompareNoCase("chitin")
                       : nType == 9997 ? sFileName.Right(7).CompareNoCase("players")
                                       : 0);

    if (bReport)
    {
        bool bOptional = sFileName.Right(5).CompareNoCase("legal") ||
                         sFileName.Left(4).CompareNoCase("live");

        if (!bOptional)
        {
            if (g_pAuroraInterface == NULL)
            {
                g_pAppManager->m_pClientExoApp->SetMissingFilePending(1);
            }
            else
            {
                g_pAppManager->m_pClientExoApp->ShutDownLegalScreen();
                AurGUIStartLayer();
                AurGUISetupViewport(0, 0, 640, 480, Vector(0.0f, 0.0f, 0.0f),
                                    false, 1.0f);
                CAurGUIImageInternal* pImage = NewCAurGUIImage("lbl_warning");
                AurMakeCurrent();
                pImage->Draw(0, 0, 640, 480, 0, 0.0f,
                             Vector(-1.0f, -1.0f, -1.0f), 1.0f);
                AurGUICloseViewport();
                AurGUIStopLayer();

                AurFlushPostedStrings();

                for (;;)
                {
                }
            }
        }
    }
}
#endif

#ifdef _XBOX

#define SIGNATURE_BUFFER_SIZE 0x10000

int CExoResMan::CreateSignatureFile(const CExoString& sFileName,
                                    unsigned short nType, unsigned long nSize,
                                    unsigned char* pSignature,
                                    unsigned long nFlags)
{
    unsigned long nTotalRead = 0;
    memset(pSignature, 0, SIGNATURE_SIZE);
    int bResult = 0;
    unsigned char* pBuffer = NULL;

    CExoFile* pFile = new CExoFile(sFileName, nType, "rb");
    if (pFile != NULL)
    {
        if (pFile->FileOpened())
        {
            if (nSize == 0)
            {
                nSize = pFile->GetSize();
            }
            pBuffer = new unsigned char[SIGNATURE_BUFFER_SIZE];
            memset(pBuffer, 0, SIGNATURE_BUFFER_SIZE);
            if (pBuffer != NULL)
            {
                HANDLE hSignature = XCalculateSignatureBegin(nFlags);
                if (hSignature != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        unsigned long nChunk = nSize - nTotalRead;
                        if (nChunk > SIGNATURE_BUFFER_SIZE)
                        {
                            nChunk = SIGNATURE_BUFFER_SIZE;
                        }
                        unsigned long nRead = pFile->Read(pBuffer, nChunk, 1);
                        nTotalRead += nRead;
                        if (nRead == 0)
                        {
                            break;
                        }
                        if (XCalculateSignatureUpdate(hSignature, pBuffer, nRead) == ERROR_SUCCESS)
                        {
                            bResult = 1;
                        }
                    } while (nTotalRead < nSize);

                    if (!bResult || XCalculateSignatureEnd(hSignature, pSignature) != ERROR_SUCCESS)
                    {
                        bResult = 0;
                    }
                }
            }
        }
        delete pFile;
        if (pBuffer != NULL)
        {
            delete[] pBuffer;
        }
    }
    return bResult;
}

int CExoResMan::VerifySignatureFile(const CExoString& sFileName,
                                    const unsigned char* pSignature)
{
    unsigned char aSignature[SIGNATURE_SIZE];

    g_pExoBase->m_pExoAliasList->ResolveFileName(sFileName, RESTYPE_SIG).CStr();
    CExoFile* pFile = new CExoFile(sFileName, RESTYPE_SIG, "rb+");
    if (pFile != NULL)
    {
        if (pFile->GetSize() != SIGNATURE_SIZE)
        {
            delete pFile;
            return 0;
        }
        if (pFile->Read(aSignature, SIGNATURE_SIZE, 1) == SIGNATURE_SIZE)
        {
            delete pFile;
            return strncmp((char*)aSignature, (char*)pSignature, SIGNATURE_SIZE) == 0;
        }
        delete pFile;
        return 0;
    }
    return 0;
}

void CExoResMan::WriteSignatureFile(const CExoString& sFileName,
                                    const unsigned char* pSignature)
{
    CExoFile* pFile = new CExoFile(sFileName, RESTYPE_SIG, "wb+");
    if (pFile != NULL)
    {
        pFile->Write(pSignature, SIGNATURE_SIZE, 1);
        delete pFile;
    }
}

CExoString CExoResMan::CacheResource(const CExoString& sFileName,
                                     unsigned short nType, int& nResult)
{
    CExoString sResult(sFileName);
    CExoString sCacheName("CACHE:");
    CExoString sSource =
        g_pExoBase->m_pExoAliasList->ResolveFileName(sFileName, nType);
    CExoString sDest;
    CExoString sDirectory;
    int bCached = 0;

    nResult = 0;

    int nColon = sFileName.Find(":", 0);
    if (nColon == -1)
    {
        sCacheName = sCacheName + sFileName;
    }
    else
    {
        sCacheName =
            sCacheName + sFileName.Right(sFileName.GetLength() - nColon - 1);
    }

    CreateDirectory2("CACHE:");

    int nSlash = sCacheName.Find("\\", 0);
    if (nSlash != -1)
    {
        sDirectory = sCacheName.Left(nSlash);
        CreateDirectory2(sDirectory);
    }

    sDest = g_pExoBase->m_pExoAliasList->ResolveFileName(sCacheName, nType);

    if (!CopyFile(sSource.CStr(), sDest.CStr(), TRUE))
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            HANDLE hSource = CreateFile(sSource.CStr(), GENERIC_READ,
                                        FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL, NULL);
            HANDLE hDest = CreateFile(sDest.CStr(), GENERIC_READ,
                                      FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL, NULL);
            unsigned long nSourceSize = GetFileSize(hSource, NULL);
            unsigned long nDestSize = GetFileSize(hDest, NULL);
            CloseHandle(hSource);
            CloseHandle(hDest);

            if (nSourceSize != nDestSize)
            {
                SetFileAttributes(sDest.CStr(), FILE_ATTRIBUTE_NORMAL);
                if (!CopyFile(sSource.CStr(), sDest.CStr(), FALSE))
                {
                    return sResult;
                }
                nResult = 1;
            }

            sResult = sCacheName;
            bCached = 1;
        }
    }
    else
    {
        sResult = sCacheName;
        nResult = 1;
        bCached = 1;
    }

    if (nResult)
    {
        SetFileAttributes(sDest.CStr(), FILE_ATTRIBUTE_NORMAL);
    }

    if (!bCached)
    {
        AurPostString("!!!WARNING!!! ATTEMPT TO CACHE FILE FAILED!!!", 5, 5,
                      60.0f);
        AurPostString(sDest.CStr(), 5, 7, 60.0f);
        AurPostString("!!!WARNING!!! CACHE MAY CONTAIN INVALID FILES!!!", 5, 10,
                      60.0f);
        AurPostString("!!!WARNING!!! PROGRAM MAY NOT BEHAVE AS EXPECTED!!!", 5,
                      15, 60.0f);
    }

    return sResult;
}
#endif

#ifndef _XBOX
int CExoResMan::CreateSignatureFile(const CExoString& sFileName,  // PC 0x00406DA0 GOG 0x00406D70
                                    unsigned short nType, unsigned long nArg3,
                                    unsigned long nArg4)
{
    return 1;
}
#endif

int CExoResMan::GetResourcesInMemory(int* pnResources, int* pnKeys,
                                     int* pnServicedSize, int* pnImageSize,
                                     int* pnModelSize, int* pnModelDataSize,
                                     int* pnTextureSize, int* pnWaveSize)
{
    CResRef cResRef;
    CExoKeyTable* pImageTable;
    CKeyTableEntry* pImageEntry;
    int nTotal = 0;

    *pnResources = 0;
    *pnKeys = 0;
    *pnServicedSize = 0;
    *pnImageSize = 0;
    *pnWaveSize = 0;
    *pnTextureSize = 0;
    *pnModelDataSize = 0;
    *pnModelSize = 0;

    CExoLinkedListNode* pPos = m_lstResourceImages.GetHeadPos();
    if (pPos != NULL)
    {
        CExoKeyTable* pTable = m_lstResourceImages.GetAtPos(pPos);
        while (pPos != NULL)
        {
            if (pTable != NULL && !pTable->m_bDestroying)
            {
                *pnImageSize += pTable->m_pPackedFiles[0]->m_nFileSize;
                for (unsigned long i = 0; i < pTable->m_nTableSize; i++)
                {
                    CKeyTableEntry* pEntry = &pTable->m_pKeyTable[i];
                    if (pEntry == NULL)
                    {
                        continue;
                    }

                    (*pnKeys)++;
                    if (pEntry->m_nResId == 0xFFFFFFFF)
                    {
                        continue;
                    }

                    int nSize = pTable->m_pPackedFiles[0]->GetResourceSize(
                        pEntry->m_nResId);
                    (*pnResources)++;
                    nTotal += nSize;
                    switch (pEntry->m_nType)
                    {
                    case RESTYPE_MDL:
                        *pnModelSize += nSize;
                        break;
                    case RESTYPE_MDX:
                        *pnModelDataSize += nSize;
                        break;
                    case RESTYPE_TPC:
                        *pnTextureSize += nSize;
                        break;
                    case RESTYPE_WAV:
                        *pnWaveSize += nSize;
                        break;
                    }
                }
            }
            pTable = m_lstResourceImages.GetNext(pPos);
        }
    }

    pPos = m_lstDirectories.GetHeadPos();
    if (pPos != NULL)
    {
        CExoKeyTable* pTable = m_lstDirectories.GetAtPos(pPos);
        while (pPos != NULL)
        {
            if (pTable != NULL && !pTable->m_bDestroying)
            {
                for (unsigned long i = 0; i < pTable->m_nTableSize; i++)
                {
                    CKeyTableEntry* pEntry = &pTable->m_pKeyTable[i];
                    if (pEntry == NULL)
                    {
                        continue;
                    }

                    (*pnKeys)++;
                    CRes* pRes = pEntry->m_pRes;
                    if (pRes == NULL || !(pRes->m_nFlags & CRes::FLAG_SERVICED))
                    {
                        continue;
                    }

                    pImageEntry = NULL;
                    cResRef = pEntry->m_resRef;
                    GetKeyEntryFromTable(m_lstResourceImages, cResRef,
                                         pEntry->m_nType, &pImageTable,
                                         &pImageEntry, 0);
                    if (pImageEntry != NULL)
                    {
                        int nImageSize =
                            pImageTable->m_pPackedFiles[0]->GetResourceSize(
                                pImageEntry->m_nResId);
                        nTotal -= nImageSize;
                        (*pnResources)--;
                        switch (pImageEntry->m_nType)
                        {
                        case RESTYPE_MDL:
                            *pnModelSize -= nImageSize;
                            break;
                        case RESTYPE_MDX:
                            *pnModelDataSize -= nImageSize;
                            break;
                        case RESTYPE_TPC:
                            *pnTextureSize -= nImageSize;
                            break;
                        case RESTYPE_WAV:
                            *pnWaveSize -= nImageSize;
                            break;
                        }
                    }

                    int nSize = pRes->m_nSize;
                    *pnServicedSize += nSize;
                    nTotal += nSize;
                    (*pnResources)++;
                    switch (pEntry->m_nType)
                    {
                    case RESTYPE_MDL:
                        *pnModelSize += nSize;
                        break;
                    case RESTYPE_MDX:
                        *pnModelDataSize += nSize;
                        break;
                    case RESTYPE_TPC:
                        *pnTextureSize += nSize;
                        break;
                    case RESTYPE_WAV:
                        *pnWaveSize += nSize;
                        break;
                    }
                }
            }
            pTable = m_lstDirectories.GetNext(pPos);
        }
    }

    pPos = m_lstEncapsulated.GetHeadPos();
    if (pPos != NULL)
    {
        CExoKeyTable* pTable = m_lstEncapsulated.GetAtPos(pPos);
        while (pPos != NULL)
        {
            if (pTable != NULL && !pTable->m_bDestroying)
            {
                for (unsigned long i = 0; i < pTable->m_nTableSize; i++)
                {
                    CKeyTableEntry* pEntry = &pTable->m_pKeyTable[i];
                    if (pEntry == NULL)
                    {
                        continue;
                    }

                    (*pnKeys)++;
                    CRes* pRes = pEntry->m_pRes;
                    if (pRes == NULL || !(pRes->m_nFlags & CRes::FLAG_SERVICED))
                    {
                        continue;
                    }

                    if (pTable->m_nPriority == 1)
                    {
                        pImageEntry = NULL;
                        cResRef = pEntry->m_resRef;
                        GetKeyEntryFromTable(m_lstResourceImages, cResRef,
                                             pEntry->m_nType, &pImageTable,
                                             &pImageEntry, 0);
                        if (pImageEntry != NULL)
                        {
                            int nImageSize =
                                pImageTable->m_pPackedFiles[0]->GetResourceSize(
                                    pImageEntry->m_nResId);
                            (*pnResources)--;
                            nTotal -= nImageSize;
                            switch (pImageEntry->m_nType)
                            {
                            case RESTYPE_MDL:
                                *pnModelSize -= nImageSize;
                                break;
                            case RESTYPE_MDX:
                                *pnModelDataSize -= nImageSize;
                                break;
                            case RESTYPE_TPC:
                                *pnTextureSize -= nImageSize;
                                break;
                            case RESTYPE_WAV:
                                *pnWaveSize -= nImageSize;
                                break;
                            }
                        }
                    }

                    int nSize = pRes->m_nSize;
                    nTotal += nSize;
                    *pnServicedSize += nSize;
                    (*pnResources)++;
                    switch (pEntry->m_nType)
                    {
                    case RESTYPE_MDL:
                        *pnModelSize += nSize;
                        break;
                    case RESTYPE_MDX:
                        *pnModelDataSize += nSize;
                        break;
                    case RESTYPE_TPC:
                        *pnTextureSize += nSize;
                        break;
                    case RESTYPE_WAV:
                        *pnWaveSize += nSize;
                        break;
                    }
                }
            }
            pTable = m_lstEncapsulated.GetNext(pPos);
        }
    }

    pPos = m_lstKeyTables.GetHeadPos();
    if (pPos != NULL)
    {
        CExoKeyTable* pTable = m_lstKeyTables.GetAtPos(pPos);
        while (pPos != NULL)
        {
            if (pTable != NULL && !pTable->m_bDestroying)
            {
                for (unsigned long i = 0; i < pTable->m_nTableSize; i++)
                {
                    CKeyTableEntry* pEntry = &pTable->m_pKeyTable[i];
                    if (pEntry == NULL)
                    {
                        continue;
                    }

                    (*pnKeys)++;
                    CRes* pRes = pEntry->m_pRes;
                    if (pRes == NULL || !(pRes->m_nFlags & CRes::FLAG_SERVICED))
                    {
                        continue;
                    }

                    int nSize = pRes->m_nSize;
                    nTotal += nSize;
                    *pnServicedSize += nSize;
                    (*pnResources)++;
                    switch (pEntry->m_nType)
                    {
                    case RESTYPE_MDL:
                        *pnModelSize += nSize;
                        break;
                    case RESTYPE_MDX:
                        *pnModelDataSize += nSize;
                        break;
                    case RESTYPE_TPC:
                        *pnTextureSize += nSize;
                        break;
                    case RESTYPE_WAV:
                        *pnWaveSize += nSize;
                        break;
                    }
                }
            }
            pTable = m_lstKeyTables.GetNext(pPos);
        }
    }

    return nTotal;
}
// PC 0x004084E0 GOG 0x004084B0
int CExoResMan::ServiceFromEncapsulatedRaw(CRes* pRes, int nSize, char* pBuffer)
{
    if (pRes == NULL)
    {
        return 0;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = m_lstEncapsulated.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = m_lstEncapsulated.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & 0x0FFFFFFF) == ((pRes->m_nResId >> 14) & 0x3F))
        {
            pTable->m_pPackedFiles[0]->AddRefCount();

            pRes->m_nSize = nSize;

            if (pTable->m_pPackedFiles[0]->ReadResource(
                    pRes->m_nResId, pBuffer, nSize, 0) != (unsigned long)nSize)
            {
                return 0;
            }

            Free(pRes);
            pTable->m_pPackedFiles[0]->DeleteRefCount();

            return 1;
        }

        pTable = m_lstEncapsulated.GetNext(pPos);
    }

    return 0;
}
// PC 0x004085A0 GOG 0x00408570
int CExoResMan::ServiceFromImageRaw(CRes* pRes, int nSize, char* pBuffer)
{
    if (pRes == NULL)
    {
        return 0;
    }

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = m_lstResourceImages.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = m_lstResourceImages.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & 0x0FFFFFFF) == ((pRes->m_nResId >> 14) & 0x3F))
        {
            pTable->m_pPackedFiles[0]->GetResourceSize(pRes->m_nResId & 0x3FFF);

            pRes->m_nSize = nSize;

            if (!Malloc(pRes))
            {
                return 0;
            }

            if (pTable->m_pPackedFiles[0]->ReadResource(
                    pRes->m_nResId, pBuffer, nSize, 0) != (unsigned long)nSize)
            {
                return 0;
            }

            return 1;
        }

        pTable = m_lstResourceImages.GetNext(pPos);
    }

    return 0;
}
// PC 0x00408660 GOG 0x00408630
int CExoResMan::ServiceFromResFileRaw(CRes* pRes, int nSize, char* pBuffer)
{
    if (pRes == NULL)
    {
        return 0;
    }

    unsigned long nFile = (pRes->m_nResId >> 20) & 0x3FF;

    CExoKeyTable* pTable;
    CExoLinkedListNode* pPos = m_lstKeyTables.GetHeadPos();

    if (pPos != NULL)
    {
        pTable = m_lstKeyTables.GetAtPos(pPos);
    }

    while (pPos != NULL)
    {
        if (pTable != NULL &&
            (pTable->m_nId & 0x0FFFFFFF) == ((pRes->m_nResId >> 14) & 0x3F))
        {
            pTable->m_pPackedFiles[nFile]->AddRefCount();

            int nResourceSize = pTable->m_pPackedFiles[nFile]->GetResourceSize(
                pRes->m_nResId & 0x3FFF);
            int nRead = nSize;

            if (nRead > nResourceSize)
            {
                nRead = nResourceSize;
            }

            pRes->m_nSize = nRead;

            if (pTable->m_pPackedFiles[nFile]->ReadResource(
                    pRes->m_nResId, pBuffer, nRead, 0) != nRead)
            {
                return 0;
            }

            pTable->m_pPackedFiles[nFile]->DeleteRefCount();

            return 1;
        }

        pTable = m_lstKeyTables.GetNext(pPos);
    }

    return 0;
}
// PC 0x00408740 GOG 0x00408710
int CExoResMan::ServiceCurrentAsyncRes()
{
    int bServiced = 0;

    CExoKeyTable* pTable = GetTable(m_pCurrentAsyncRes);

    if (pTable == NULL)
    {
        return 0;
    }

    switch (m_pCurrentAsyncRes->m_nResId >> 30)
    {
    case 3:
        if (m_pCurrentAsyncFile != NULL)
        {
            delete m_pCurrentAsyncFile;
            m_pCurrentAsyncFile = NULL;
        }
        break;
    case 1:
    case 2:
        pTable->m_pPackedFiles[0]->DeleteAsyncRefCount();
        break;
    case 0:
        pTable->m_pPackedFiles[(m_pCurrentAsyncRes->m_nResId >> 20) & 0x3FF]
            ->DeleteAsyncRefCount();
        break;
    }

    m_pCurrentAsyncRes->m_nFlags &= ~0x10;
    m_pCurrentAsyncRes->m_nFlags |= 0x4;

    if (m_pCurrentAsyncRes->m_pResource != NULL)
    {
        m_pCurrentAsyncRes->m_nRequestCount = 0;
        bServiced = m_pCurrentAsyncRes->OnResourceServiced();
        m_lstRequests.RemoveHead();
        m_pCurrentAsyncRes = NULL;
        m_pCurrentAsyncFile = NULL;
    }

    return bServiced;
}
// PC 0x00408800 GOG 0x004087D0
CExoString CExoResMan::CacheResource(const CExoString& sFileName,
                                     unsigned short nType)
{
    int nResult;

    return CacheResource(sFileName, nType, nResult);
}

#ifndef _XBOX
CExoString CExoResMan::CacheResource(const CExoString& sFileName,  // PC 0x00406DB0 GOG 0x00406D80
                                     unsigned short nType, int& nResult)
{
    CExoString sCached(sFileName);

    return sCached;
}
#endif
// PC 0x00408880 GOG 0x00408850
int CRes::Request()
{
    return g_pExoResMan->Request(this);
}

static inline void ExoresmanTrace1()
{
}

#ifdef _XBOX
#include "exotlktable.h"

int CExoResMan::VerifySignatureFile(const CExoString& sFileName,
                                    unsigned short nType, unsigned long nSize,
                                    unsigned long nFlags)
{
    unsigned char aSignature[SIGNATURE_SIZE];
    unsigned char aStored[SIGNATURE_SIZE];
    int bResult = 0;

    CExoFile* pFile = new CExoFile(sFileName, RESTYPE_SIG, "rb");
    if (pFile != NULL)
    {
        if (pFile->FileOpened() &&
            CreateSignatureFile(sFileName, nType, nSize, aSignature, nFlags) == 1)
        {
            pFile->Read(aStored, SIGNATURE_SIZE, 1);
            if (memcmp(aSignature, aStored, SIGNATURE_SIZE) == 0)
            {
                bResult = 1;
            }
        }
        delete pFile;
    }
    return bResult;
}

int CExoResMan::CreateSignatureFile(const CExoString& sFileName,
                                    unsigned short nType, unsigned long nSize,
                                    unsigned long nFlags)
{
    unsigned char aSignature[SIGNATURE_SIZE];
    int bResult = 0;

    if (CreateSignatureFile(sFileName, nType, nSize, aSignature, nFlags) == 1)
    {
        CExoFile* pFile = new CExoFile(sFileName, RESTYPE_SIG, "wb");
        if (pFile->Write(aSignature, SIGNATURE_SIZE, 1) == SIGNATURE_SIZE)
        {
            pFile->Flush();
            bResult = 1;
        }
        if (pFile != NULL)
        {
            delete pFile;
        }
    }
    return bResult;
}

int CExoResMan::FindModuleDownloadedPkg(const CExoString& sModuleName)
{
    CExoString sAlias;
    int nPackage;

    for (nPackage = 1; nPackage < CTlkTable::NUM_CONTENT_PKGS; nPackage++)
    {
        sAlias.Format("LIVE%d", nPackage);
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength())
        {
            sAlias = sAlias + ":MODULES";
            AddResourceDirectory(sAlias);
            if (Exists(CResRef(sModuleName), RESTYPE_MOD, (unsigned long*)NULL) ||
                Exists(CResRef(sModuleName), RESTYPE_RIM, (unsigned long*)NULL))
            {
                RemoveResourceDirectory(sAlias);
                break;
            }
            RemoveResourceDirectory(sAlias);
        }
    }

    return nPackage < CTlkTable::NUM_CONTENT_PKGS ? nPackage : 0;
}
#endif

#ifdef _XBOX

void CExoResMan::AsyncLoad()
{
    CExoCriticalSection* pCriticalSection = &m_csAsync;
    pCriticalSection->EnterCriticalSection();

    int bResult = 1;
    int bSaveGame = 0;
    CExoString sPath;
    CExoString sDirectory;

    if (m_bAsyncIsModule == 0)
    {
        sPath = CExoString("RIMS:") + m_sAsyncModuleName;
        bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);
    }
    else
    {
        int nDownloadedPackage = FindModuleDownloadedPkg(m_sAsyncModuleName);

        sPath = m_sAsyncModuleName + CExoString("_a");
        if (Exists(sPath, RESTYPE_RIM, (unsigned long*)NULL))
        {
            if (nDownloadedPackage == 0)
            {
                sPath = CExoString("RIMSXBOX:") + sPath;
                bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);
            }
            else
            {
                sDirectory.Format("LIVE%d:RIMSXBOX\\", nDownloadedPackage);
                sPath = sDirectory + sPath;
                bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);
            }

            if (bResult)
            {
                m_nAsyncLoadedFlags |= LOADED_MODULE_AREA_RIM;
            }
        }

        if (bResult)
        {
            sPath = m_sAsyncModuleName + CExoString("_adx");
            if (Exists(sPath, RESTYPE_RIM, (unsigned long*)NULL))
            {
                if (nDownloadedPackage == 0)
                {
                    sPath = CExoString("RIMSXBOX:") + sPath;
                    bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);
                }
                else
                {
                    sDirectory.Format("LIVE%d:RIMSXBOX\\", nDownloadedPackage);
                    sPath = sDirectory + sPath;
                    bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);
                }

                if (bResult)
                {
                    m_nAsyncLoadedFlags |= LOADED_MODULE_ADX_RIM;
                }
            }

            if (bResult)
            {
                AddKeyTable("MODULES:", TABLE_DIRECTORY, 0);
                if (nDownloadedPackage != 0)
                {
                    sDirectory.Format("LIVE%d:MODULES", nDownloadedPackage);
                    AddKeyTable(sDirectory, TABLE_DIRECTORY, 0);
                }
                if (!Exists(m_sAsyncModuleName, RESTYPE_MOD, (unsigned long*)NULL))
                {
                    sPath = m_sAsyncModuleName + CExoString("_s");
                    if (Exists(sPath, RESTYPE_RIM, (unsigned long*)NULL))
                    {
                        if (nDownloadedPackage == 0)
                        {
                            sPath = CExoString("MODULES:") + sPath;
                            bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);
                        }
                        else
                        {
                            sDirectory = sDirectory + "\\";
                            sPath = sDirectory + sPath;
                            bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);
                        }

                        if (bResult)
                        {
                            bSaveGame = 1;
                            m_nAsyncLoadedFlags |= LOADED_SAVE_GAME_RIM;
                        }
                    }
                }
                else
                {
                    if (nDownloadedPackage == 0)
                    {
                        sPath = CExoString("MODULES:") + m_sAsyncModuleName;
                    }
                    else
                    {
                        sDirectory = sDirectory + "\\";
                        sPath = sDirectory + m_sAsyncModuleName;
                    }
                    bResult = AddKeyTable(sPath, TABLE_ENCAPSULATED, PRIORITY_MODULE);

                    if (bResult)
                    {
                        m_nAsyncLoadedFlags |= LOADED_MODULE_RIM;
                    }
                }

                RemoveKeyTable("MODULES:", TABLE_DIRECTORY);
                if (nDownloadedPackage != 0)
                {
                    sDirectory.Format("LIVE%d:MODULES", nDownloadedPackage);
                    RemoveKeyTable(sDirectory, TABLE_DIRECTORY);
                }
            }
        }

        if (!m_bAsyncFromCurrentGame && bResult)
        {
            sPath = CExoString("CURRENTGAME:") + m_sAsyncModuleName;

            AddKeyTable("CURRENTGAME:", TABLE_DIRECTORY, 0);
            if (Exists(m_sAsyncModuleName, RESTYPE_RSV, (unsigned long*)NULL) ||
                Exists(m_sAsyncModuleName, RESTYPE_RIM, (unsigned long*)NULL))
            {
                bResult = AddKeyTable(sPath, TABLE_RESOURCE_IMAGE, 0);

                if (bResult)
                {
                    m_nAsyncLoadedFlags |= LOADED_CURRENT_GAME_RIM;
                }
            }

            RemoveKeyTable("CURRENTGAME:", TABLE_DIRECTORY);
        }
        else
        {
            sPath = CExoString("CURRENTGAME:") + m_sAsyncModuleName;
            bResult = AddKeyTable(sPath, TABLE_ENCAPSULATED, PRIORITY_MODULE);

            if (bResult)
            {
                m_nAsyncLoadedFlags |= LOADED_CURRENT_GAME_KEY;
            }
        }
    }

    if (bResult)
    {
        m_nAsyncState = (bSaveGame == 0) + ASYNC_MODULE_LOADED;
    }
    else
    {
        m_nAsyncState = ASYNC_LOAD_FAILED;
    }

    pCriticalSection->LeaveCriticalSection();
}
#endif

