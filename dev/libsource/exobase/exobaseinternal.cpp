#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "exobase.h"
#include "swmain.h"

#ifndef _XBOX
void GetGameVersionInfo()  // PC 0x00632460
{
    char szFileName[256];
    DWORD dwHandle = 0;
    LPVOID pBuffer;
    UINT nLength;
    BOOL bResult;

    GetModuleFileName(g_hInst, szFileName, 256);

    DWORD nSize = GetFileVersionInfoSize(szFileName, &dwHandle);

    if (nSize != 0)
    {
        void* pData = operator new(nSize);

        GetFileVersionInfo(szFileName, dwHandle, nSize, pData);

        bResult =
            VerQueryValue(pData, "\\StringFileInfo\\040904B0\\ProductVersion",
                          &pBuffer, &nLength);
        if (bResult)
        {
            g_sVersionNumber = CExoString((char*)pBuffer);
        }
        else
        {
            g_sVersionNumber = "";
        }

        bResult =
            VerQueryValue(pData, "\\StringFileInfo\\040904B0\\InternalName",
                          &pBuffer, &nLength);
        if (bResult)
        {
            g_sGameName = CExoString((char*)pBuffer);
        }
        else
        {
            g_sGameName = "";
        }

        bResult =
            VerQueryValue(pData, "\\StringFileInfo\\040904B0\\LegalCopyright",
                          &pBuffer, &nLength);
        if (bResult)
        {
            g_sCopyrightInfo = CExoString((char*)pBuffer);
        }
        else
        {
            g_sCopyrightInfo = "";
        }

        if (pData != NULL)
        {
            operator delete(pData);
        }
    }
}
#endif
// PC 0x00632620
void CExoBaseInternal::CreateResourceExtensionTable()
{
    m_nResourceTypeCount = 87;
    m_pResourceTypes = new unsigned short[m_nResourceTypeCount];
    m_pResourceExtensions = new CExoString[m_nResourceTypeCount];

    m_pResourceTypes[0] = 0;
    m_pResourceExtensions[0] = "res";
    m_pResourceTypes[1] = 1;
    m_pResourceExtensions[1] = "bmp";
    m_pResourceTypes[2] = 2;
    m_pResourceExtensions[2] = "mve";
    m_pResourceTypes[3] = 3;
    m_pResourceExtensions[3] = "tga";
    m_pResourceTypes[4] = 4;
    m_pResourceExtensions[4] = "wav";
    m_pResourceTypes[5] = 7;
    m_pResourceExtensions[5] = "ini";
    m_pResourceTypes[6] = 10;
    m_pResourceExtensions[6] = "txt";
    m_pResourceTypes[7] = 2022;
    m_pResourceExtensions[7] = "txi";
    m_pResourceTypes[8] = 9999;
    m_pResourceExtensions[8] = "key";
    m_pResourceTypes[9] = 9998;
    m_pResourceExtensions[9] = "bif";
    m_pResourceTypes[10] = 9997;
    m_pResourceExtensions[10] = "erf";
    m_pResourceTypes[11] = 2000;
    m_pResourceExtensions[11] = "plh";
    m_pResourceTypes[12] = 2001;
    m_pResourceExtensions[12] = "tex";
    m_pResourceTypes[13] = 2002;
    m_pResourceExtensions[13] = "mdl";
    m_pResourceTypes[14] = 2007;
    m_pResourceExtensions[14] = "lua";
    m_pResourceTypes[15] = 2003;
    m_pResourceExtensions[15] = "thg";
    m_pResourceTypes[16] = 2008;
    m_pResourceExtensions[16] = "slt";
    m_pResourceTypes[17] = 2009;
    m_pResourceExtensions[17] = "nss";
    m_pResourceTypes[18] = 2010;
    m_pResourceExtensions[18] = "ncs";
    m_pResourceTypes[19] = 2011;
    m_pResourceExtensions[19] = "mod";
    m_pResourceTypes[20] = 2012;
    m_pResourceExtensions[20] = "are";
    m_pResourceTypes[21] = 2013;
    m_pResourceExtensions[21] = "set";
    m_pResourceTypes[22] = 2014;
    m_pResourceExtensions[22] = "ifo";
    m_pResourceTypes[23] = 2015;
    m_pResourceExtensions[23] = "bic";
    m_pResourceTypes[24] = 2016;
    m_pResourceExtensions[24] = "wok";
    m_pResourceTypes[25] = 6;
    m_pResourceExtensions[25] = "plt";
    m_pResourceTypes[26] = 2005;
    m_pResourceExtensions[26] = "fnt";
    m_pResourceTypes[27] = 2017;
    m_pResourceExtensions[27] = "2da";
    m_pResourceTypes[28] = 2018;
    m_pResourceExtensions[28] = "tlk";
    m_pResourceTypes[29] = 8;
    m_pResourceExtensions[29] = "mp3";
    m_pResourceTypes[30] = 9;
    m_pResourceExtensions[30] = "mpg";
    m_pResourceTypes[31] = 2024;
    m_pResourceExtensions[31] = "bti";
    m_pResourceTypes[32] = 2025;
    m_pResourceExtensions[32] = "uti";
    m_pResourceTypes[33] = 2026;
    m_pResourceExtensions[33] = "btc";
    m_pResourceTypes[34] = 2027;
    m_pResourceExtensions[34] = "utc";
    m_pResourceTypes[35] = 2031;
    m_pResourceExtensions[35] = "btt";
    m_pResourceTypes[36] = 2032;
    m_pResourceExtensions[36] = "utt";
    m_pResourceTypes[37] = 2023;
    m_pResourceExtensions[37] = "git";
    m_pResourceTypes[38] = 2029;
    m_pResourceExtensions[38] = "dlg";
    m_pResourceTypes[39] = 2030;
    m_pResourceExtensions[39] = "itp";
    m_pResourceTypes[40] = 2033;
    m_pResourceExtensions[40] = "dds";
    m_pResourceTypes[41] = 2034;
    m_pResourceExtensions[41] = "bts";
    m_pResourceTypes[42] = 2035;
    m_pResourceExtensions[42] = "uts";
    m_pResourceTypes[43] = 2036;
    m_pResourceExtensions[43] = "ltr";
    m_pResourceTypes[44] = 2037;
    m_pResourceExtensions[44] = "gff";
    m_pResourceTypes[45] = 2038;
    m_pResourceExtensions[45] = "fac";
    m_pResourceTypes[46] = 2039;
    m_pResourceExtensions[46] = "bte";
    m_pResourceTypes[47] = 2040;
    m_pResourceExtensions[47] = "ute";
    m_pResourceTypes[48] = 2041;
    m_pResourceExtensions[48] = "btd";
    m_pResourceTypes[49] = 2042;
    m_pResourceExtensions[49] = "utd";
    m_pResourceTypes[50] = 2043;
    m_pResourceExtensions[50] = "btp";
    m_pResourceTypes[51] = 2044;
    m_pResourceExtensions[51] = "utp";
    m_pResourceTypes[52] = 2045;
    m_pResourceExtensions[52] = "dft";
    m_pResourceTypes[53] = 2046;
    m_pResourceExtensions[53] = "gic";
    m_pResourceTypes[54] = 2047;
    m_pResourceExtensions[54] = "gui";
    m_pResourceTypes[55] = 2048;
    m_pResourceExtensions[55] = "css";
    m_pResourceTypes[56] = 2049;
    m_pResourceExtensions[56] = "ccs";
    m_pResourceTypes[57] = 2050;
    m_pResourceExtensions[57] = "btm";
    m_pResourceTypes[58] = 2051;
    m_pResourceExtensions[58] = "utm";
    m_pResourceTypes[59] = 2052;
    m_pResourceExtensions[59] = "dwk";
    m_pResourceTypes[60] = 2053;
    m_pResourceExtensions[60] = "pwk";
    m_pResourceTypes[61] = 2054;
    m_pResourceExtensions[61] = "btg";
    m_pResourceTypes[62] = 2055;
    m_pResourceExtensions[62] = "utg";
    m_pResourceTypes[63] = 2056;
    m_pResourceExtensions[63] = "jrl";
    m_pResourceTypes[64] = 2057;
    m_pResourceExtensions[64] = "sav";
    m_pResourceTypes[65] = 2058;
    m_pResourceExtensions[65] = "utw";
    m_pResourceTypes[66] = 2059;
    m_pResourceExtensions[66] = "4pc";
    m_pResourceTypes[67] = 2060;
    m_pResourceExtensions[67] = "ssf";
    m_pResourceTypes[68] = 2061;
    m_pResourceExtensions[68] = "hak";
    m_pResourceTypes[69] = 3000;
    m_pResourceExtensions[69] = "lyt";
    m_pResourceTypes[70] = 3001;
    m_pResourceExtensions[70] = "vis";
    m_pResourceTypes[71] = 3002;
    m_pResourceExtensions[71] = "rim";
    m_pResourceTypes[72] = 11;
    m_pResourceExtensions[72] = "wma";
    m_pResourceTypes[73] = 3003;
    m_pResourceExtensions[73] = "pth";
    m_pResourceTypes[74] = 3005;
    m_pResourceExtensions[74] = "bwm";
    m_pResourceTypes[75] = 12;
    m_pResourceExtensions[75] = "wmv";
    m_pResourceTypes[76] = 3004;
    m_pResourceExtensions[76] = "lip";
    m_pResourceTypes[77] = 2062;
    m_pResourceExtensions[77] = "nwm";
    m_pResourceTypes[78] = 2063;
    m_pResourceExtensions[78] = "bik";
    m_pResourceTypes[79] = 3006;
    m_pResourceExtensions[79] = "txb";
    m_pResourceTypes[80] = 3007;
    m_pResourceExtensions[80] = "tpc";
    m_pResourceTypes[81] = 3008;
    m_pResourceExtensions[81] = "mdx";
    m_pResourceTypes[82] = 3009;
    m_pResourceExtensions[82] = "rsv";
    m_pResourceTypes[83] = 3010;
    m_pResourceExtensions[83] = "sig";
    m_pResourceTypes[84] = 13;
    m_pResourceExtensions[84] = "xmv";
    m_pResourceTypes[85] = 3011;
    m_pResourceExtensions[85] = "xbx";
    m_pResourceTypes[86] = 65535;
    m_pResourceExtensions[86] = "";
}
#ifndef _XBOX
void CExoBaseInternal::AddAlias(const CExoString& sFile,  // PC 0x00633040
                                const CExoString& sAlias,
                                const CExoString& sDefault,
                                const CExoString& sUnused)
{
    CExoString sPath;

    if (!g_pExoBase->m_pExoIni->ReadIniEntry(sPath, sFile, "Alias", sAlias))
    {
        sPath = sDefault;
    }

    g_pExoBase->m_pExoAliasList->Add(sAlias, sPath);
}
#endif
int CExoBaseInternal::GetDirectorySize(const CExoString& sDir)  // PC 0x00633100
{
    CExoString sPath;
    CExoString sFile;
    CExoString sUnused;
    WIN32_FIND_DATA sFindData;
    int nSize = 0;

    sPath = g_pExoBase->m_pExoAliasList->ResolveFileName(sDir, 0xFFFF);

    if (sPath[sPath.GetLength() - 1] == '\\')
    {
        sPath.Format("%s*.*", sPath.CStr());
    }
    else
    {
        sPath.Format("%s\\*.*", sPath.CStr());
    }

    HANDLE hFind = FindFirstFile(sPath.CStr(), &sFindData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        DWORD nError = 0;

        do
        {
            sFile = sFindData.cFileName;
            nSize += sFindData.nFileSizeLow;

            if (!FindNextFile(hFind, &sFindData))
            {
                nError = GetLastError();
            }
        } while (nError != ERROR_NO_MORE_FILES);

        FindClose(hFind);

        return nSize;
    }
    else
    {
        return 0;
    }
}
// PC 0x006332F0
const CExoString& CExoBaseInternal::GetResourceExtension(
    unsigned short resourceType)
{
    for (int i = 0; i < m_nResourceTypeCount; ++i)
    {
        if (m_pResourceTypes[i] == resourceType)
            return m_pResourceExtensions[i];
    }
    return m_pResourceExtensions[86];
}
// PC 0x00633330
unsigned short CExoBaseInternal::GetResTypeFromExtension(
    const CExoString& extension)
{
    for (int i = 0; i < m_nResourceTypeCount; ++i)
    {
        if (m_pResourceExtensions[i].CompareNoCase(extension))
            return m_pResourceTypes[i];
    }
    return m_pResourceTypes[86];
}
#ifndef _XBOX
void CExoBaseInternal::LoadAliases(const CExoString& sFile)  // PC 0x00633380
{
    CExoString sAliasFile;

    sAliasFile.Format(".\\%s", sFile.CStr());

    {
        CExoString sUnused("d:\\");
        CExoString sDefault(".\\");
        CExoString sAlias("HD0");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\");
        CExoString sDefault("d:\\");
        CExoString sAlias("CD0");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\override");
        CExoString sDefault(".\\override");
        CExoString sAlias("OVERRIDE");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\errortex");
        CExoString sDefault(".\\errortex");
        CExoString sAlias("ERRORTEX");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\temp");
        CExoString sDefault(".\\temp");
        CExoString sAlias("TEMP");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\modules");
        CExoString sDefault(".\\modules");
        CExoString sAlias("MODULES");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\nwm");
        CExoString sDefault(".\\nwm");
        CExoString sAlias("NWMFILES");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\logs");
        CExoString sDefault(".\\logs");
        CExoString sAlias("LOGS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\localvault");
        CExoString sDefault(".\\localvault");
        CExoString sAlias("LOCALVAULT");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\dmvault");
        CExoString sDefault(".\\dmvault");
        CExoString sAlias("DMVAULT");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\servervault");
        CExoString sDefault(".\\servervault");
        CExoString sAlias("SERVERVAULT");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("u:\\");
        CExoString sDefault(".\\saves");
        CExoString sAlias("SAVES");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\music");
        CExoString sDefault(".\\music");
        CExoString sAlias("MUSIC");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\streammusic");
        CExoString sDefault(".\\streammusic");
        CExoString sAlias("STREAMMUSIC");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\movies");
        CExoString sDefault(".\\movies");
        CExoString sAlias("MOVIES");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\tempclient");
        CExoString sDefault(".\\tempclient");
        CExoString sAlias("TEMPCLIENT");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\currentgame");
        CExoString sDefault(".\\currentgame");
        CExoString sAlias("CURRENTGAME");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\hak");
        CExoString sDefault(".\\hak");
        CExoString sAlias("HAK");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\texturepacks");
        CExoString sDefault(".\\texturepacks");
        CExoString sAlias("TEXTUREPACKS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\streamvoice");
        CExoString sDefault(".\\streamvoice");
        CExoString sAlias("STREAMVOICE");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\supermodels");
        CExoString sDefault(".\\supermodels");
        CExoString sAlias("SUPERMODELS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("t:\\");
        CExoString sDefault(".\\downloads");
        CExoString sAlias("DOWNLOADS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("t:\\");
        CExoString sDefault(".\\");
        CExoString sAlias("OPTIONS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\ambient");
        CExoString sDefault(".\\ambient");
        CExoString sAlias("AMBIENT");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\patch");
        CExoString sDefault(".\\patch");
        CExoString sAlias("PATCH");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\portraits");
        CExoString sDefault(".\\portraits");
        CExoString sAlias("PORTRAITS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\music");
        CExoString sDefault(".\\music");
        CExoString sAlias("MUSIC");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\currentgame");
        CExoString sDefault(".\\currentgame");
        CExoString sAlias("CURRENTGAME");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("z:\\gameinprogress");
        CExoString sDefault(".\\gameinprogress");
        CExoString sAlias("GAMEINPROGRESS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("z:\\futuregame");
        CExoString sDefault(".\\futuregame");
        CExoString sAlias("FUTUREGAME");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\rims");
        CExoString sDefault(".\\rims");
        CExoString sAlias("RIMS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\rimsxbox");
        CExoString sDefault(".\\rimsxbox");
        CExoString sAlias("RIMSXBOX");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\rebootdata");
        CExoString sDefault(".\\rebootdata");
        CExoString sAlias("REBOOTDATA");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("z:\\cache");
        CExoString sDefault(".\\");
        CExoString sAlias("CACHE");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }

    {
        CExoString sUnused("d:\\lips");
        CExoString sDefault(".\\lips");
        CExoString sAlias("LIPS");

        AddAlias(sAliasFile, sAlias, sDefault, sUnused);
    }
}
#endif
#ifndef _XBOX
void CExoBaseInternal::SpawnExternalApplication(const CExoString& sApplication,
                                                const CExoString& sCommandLine,
                                                int bQuit)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(STARTUPINFO));

    if (CreateProcess(sApplication.CStr(), sCommandLine.CStr(), NULL, NULL,
                      FALSE, 0, NULL, NULL, &si, &pi))
    {
        if (bQuit)
        {
            PostQuitMessage(0);
        }
    }
}
#endif
// PC 0x00634510
int CExoBaseInternal::WcsToMbs(char* sDest, const wchar_t* sSource,
                               unsigned long nLength)
{
    int nDest = 0;
    unsigned long nSource = wcslen(sSource);

    for (unsigned long i = 0; i < (nLength < nSource ? nLength : nSource); i++)
    {
        int nBytes = wctomb(&sDest[nDest], sSource[i]);

        if (nBytes == -1 || (sSource[i] >= 1 && sSource[i] <= 0x1F))
        {
            sDest[nDest] = 127;
            nDest++;
        }
        else
        {
            nDest += nBytes;
        }
    }

    return 1;
}
// PC 0x00634590
CExoBaseInternal::CExoBaseInternal()
{
    CreateResourceExtensionTable();
}
// PC 0x006345A0
CExoBaseInternal::~CExoBaseInternal()
{
    if (m_pResourceTypes)
        delete[] m_pResourceTypes;
    delete[] m_pResourceExtensions;
}

static inline int GetListCount(const CExoArrayList<CExoString>* pList);
// PC 0x006346E0
int CExoBaseInternal::GetDirectoryList(CExoArrayList<CExoString>* pList,
                                       const CExoString& sDir,
                                       unsigned short nResType,
                                       int bDirectories, int nSortMode)
{
    CExoString sPath;
    CExoString sFile;
    CExoString sExtension;
    WIN32_FIND_DATA sFindData;

    sExtension = g_pExoBase->GetResourceExtension(nResType);

    if (sExtension == "" || bDirectories != 0)
    {
        sExtension = "*";
    }

    sPath = g_pExoBase->m_pExoAliasList->ResolveFileName(sDir, 0xFFFF);

    if (sPath[sPath.GetLength() - 1] == '\\')
    {
        sPath.Format("%s*.%s", sPath.CStr(), sExtension.CStr());
    }
    else
    {
        sPath.Format("%s\\*.%s", sPath.CStr(), sExtension.CStr());
    }

    HANDLE hFind = FindFirstFile(sPath.CStr(), &sFindData);
    CExoArrayList<FILETIME> lstTimes;

    if (hFind != INVALID_HANDLE_VALUE)
    {
        DWORD nError = 0;
        int i;

        do
        {
            sFile = sFindData.cFileName;

            CExoString sLower = sFile.LowerCase();

            if (bDirectories != 0)
            {
                if ((sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                    strcmp(sFile.CStr(), ".") != 0 &&
                    strcmp(sFile.CStr(), "..") != 0)
                {
                    if (nSortMode != 0)
                    {
                        for (i = 0; i < GetListCount(pList); i++)
                        {
                            if ((*pList)[i].LowerCase() >= sLower)
                            {
                                break;
                            }
                        }

                        pList->Insert(sFile, i);
                    }
                    else
                    {
                        pList->Add(sFile);
                    }
                }
            }
            else if (!(sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                if (nSortMode == 1)
                {
                    int nLow = 0;
                    int nHigh = pList->num - 1;

                    i = (pList->num - 1) / 2;

                    while (nLow < nHigh)
                    {
                        if ((*pList)[i].LowerCase() < sLower)
                        {
                            nLow = i + 1;
                        }
                        else
                        {
                            nHigh = i;
                        }

                        i = (nLow + nHigh) / 2;
                    }

                    if (nHigh >= 0 && (*pList)[i].LowerCase() < sLower)
                    {
                        pList->Insert(sFile, i + 1);
                    }
                    else
                    {
                        pList->Insert(sFile, i);
                    }
                }
                else if (nSortMode == 2)
                {
                    for (i = 0; i < pList->num; i++)
                    {
                        if (CompareFileTime(&sFindData.ftLastWriteTime,
                                            &lstTimes[i]) == 1)
                        {
                            break;
                        }
                    }

                    lstTimes.Insert(sFindData.ftLastWriteTime, i);
                    pList->Insert(sFile, i);
                }
                else
                {
                    pList->Add(sFile);
                }
            }

            if (!FindNextFile(hFind, &sFindData))
            {
                nError = GetLastError();
            }
        } while (nError != ERROR_NO_MORE_FILES);

        FindClose(hFind);

        return 1;
    }

    return 0;
}

static inline int GetListCount(const CExoArrayList<CExoString>* pList)
{
    return pList->num;
}

class CFileInfo
{
public:
    CExoString m_sName;
    unsigned short m_nYear;
    unsigned short m_nMonth;
    unsigned short m_nDay;
    unsigned short m_nHour;
    unsigned short m_nMinute;
    unsigned short m_nSecond;
};

#ifndef _XBOX
int CExoBaseInternal::CheckForCD()
{
    CExoString sSearch;
    CExoString sFile;
    CExoString sRoot;
    CExoArrayList<CExoString> lstCDFiles;
    WIN32_FIND_DATA sFindData;
    int bHaveAlias;
    int nDrive;

    bHaveAlias = g_pExoBase->m_pExoIni->ReadIniEntry(sSearch, ".\\swkotor.ini",
                                                     "Alias", "CD0");
    if (bHaveAlias)
    {
        sSearch = sSearch.Left(1);
    }

    lstCDFiles.Add("data1.cab");
    lstCDFiles.Add("data1.hdr");
    lstCDFiles.Add("data2.cab");
    lstCDFiles.Add("layout.bin");
    lstCDFiles.Add("setup.exe");
    lstCDFiles.Add("setup.ini");
    lstCDFiles.Add("data");
    lstCDFiles.Add("readme-kotor.rtf");

    nDrive = 2;
    for (;;)
    {
        if (bHaveAlias)
        {
            nDrive = sSearch[0] - 'A' - 1;
            sSearch.Format("%s:\\*.*", sSearch.CStr());
            bHaveAlias = 0;
        }
        else
        {
            sSearch.Format("%c:\\*.*", nDrive + 'A');
        }

        sRoot = sSearch.Left(3);

        if (GetDriveType(sRoot.CStr()) == DRIVE_CDROM)
        {
            int nFound = 0;
            HANDLE hFind = FindFirstFile(sSearch.CStr(), &sFindData);

            if (hFind != INVALID_HANDLE_VALUE)
            {
                DWORD nError = 0;

                do
                {
                    sFile = sFindData.cFileName;
                    sFile = sFile.LowerCase();

                    if (!(sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                        (strcmp(sFile.CStr(), ".") != 0 &&
                         strcmp(sFile.CStr(), "..") != 0))
                    {
                        int nMatches = 0;

                        for (int i = 0; i < lstCDFiles.num; i++)
                        {
                            if (lstCDFiles[i] == sFile)
                            {
                                nMatches++;
                            }
                        }

                        if (nMatches != 0)
                        {
                            nFound++;
                        }
                    }

                    if (!FindNextFile(hFind, &sFindData))
                    {
                        nError = GetLastError();
                    }
                } while (nError != ERROR_NO_MORE_FILES);

                FindClose(hFind);

                if (nFound == lstCDFiles.num)
                {
                    break;
                }
            }
        }

        if (++nDrive >= 26)
        {
            return 0;
        }
    }

    CExoString sAlias;
    if (!g_pExoBase->m_pExoIni->ReadIniEntry(sAlias, ".\\swkotor.ini", "Alias",
                                             "CD0"))
    {
        g_pExoBase->m_pExoIni->WriteIniEntry(".\\swkotor.ini", "Alias", "CD0",
                                             sSearch.Left(3));
    }

    return 1;
}
#endif

int CExoBaseInternal::GetAugmentedDirectoryList(CExoArrayList<CFileInfo>* pList,
                                                const CExoString& sDir,
                                                unsigned short nResType,
                                                int bSorted)
{
    CExoString sFile;
    CExoString sPath;
    CExoString sDirectory;
    CExoString sExtension;
    WIN32_FIND_DATA sFindData;
    FILETIME sLocalTime;
    SYSTEMTIME sSystemTime;

    sExtension = g_pExoBase->GetResourceExtension(nResType);

    if (sExtension == "")
    {
        sExtension = "*";
    }

    sDirectory = g_pExoBase->m_pExoAliasList->ResolveFileName(sDir, 0xFFFF);
    sPath.Format("%s\\*.%s", sDirectory.CStr(), sExtension.CStr());

    HANDLE hFind = FindFirstFile(sPath.CStr(), &sFindData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    DWORD nError = 0;

    do
    {
        sFile = sFindData.cFileName;
        FileTimeToLocalFileTime(&sFindData.ftLastWriteTime, &sLocalTime);
        FileTimeToSystemTime(&sLocalTime, &sSystemTime);

        if (!(sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            CFileInfo cInfo;
            cInfo.m_sName = sFile;
            cInfo.m_nYear = sSystemTime.wYear;
            cInfo.m_nMonth = sSystemTime.wMonth;
            cInfo.m_nDay = sSystemTime.wDay;
            cInfo.m_nHour = sSystemTime.wHour;
            cInfo.m_nMinute = sSystemTime.wMinute;
            cInfo.m_nSecond = sSystemTime.wSecond;

            if (bSorted)
            {
                int i;
                for (i = 0; i < pList->num; i++)
                {
                    if ((*pList)[i].m_sName > sFile)
                    {
                        break;
                    }
                }
                pList->Insert(cInfo, i);
            }
            else
            {
                pList->Add(cInfo);
            }
        }

        if (!FindNextFile(hFind, &sFindData))
        {
            nError = GetLastError();
        }
    } while (nError != ERROR_NO_MORE_FILES);

    FindClose(hFind);

    return 1;
}
#ifdef _XBOX

void CExoBaseInternal::AddAlias(const CExoString& sFile,
                                const CExoString& sAlias,
                                const CExoString& sDefault,
                                const CExoString& sXboxPath)
{
    CExoString sPath;

    if (!g_pExoBase->m_pExoIni->ReadIniEntry(sPath, sFile, "Alias", sAlias))
    {
        sPath = sXboxPath;
    }

    g_pExoBase->m_pExoAliasList->Add(sAlias, sPath);
}

void CExoBaseInternal::LoadAliases(const CExoString& sFile)
{
    CExoString sAliasFile;

    sAliasFile.Format("d:\\%s", sFile.CStr());

    {
        CExoString sXboxPath("d:\\");
        CExoString sDefault(".\\");
        CExoString sAlias("HD0");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("t:\\");
        CExoString sDefault("d:\\");
        CExoString sAlias("CD0");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("t:\\override");
        CExoString sDefault(".\\override");
        CExoString sAlias("OVERRIDE");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\errortex");
        CExoString sDefault(".\\errortex");
        CExoString sAlias("ERRORTEX");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("z:\\temp");
        CExoString sDefault(".\\temp");
        CExoString sAlias("TEMP");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\modules");
        CExoString sDefault(".\\modules");
        CExoString sAlias("MODULES");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\nwm");
        CExoString sDefault(".\\nwm");
        CExoString sAlias("NWMFILES");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("t:\\logs");
        CExoString sDefault(".\\logs");
        CExoString sAlias("LOGS");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\localvault");
        CExoString sDefault(".\\localvault");
        CExoString sAlias("LOCALVAULT");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\dmvault");
        CExoString sDefault(".\\dmvault");
        CExoString sAlias("DMVAULT");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\servervault");
        CExoString sDefault(".\\servervault");
        CExoString sAlias("SERVERVAULT");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("u:\\");
        CExoString sDefault(".\\saves");
        CExoString sAlias("SAVES");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\music");
        CExoString sDefault(".\\music");
        CExoString sAlias("MUSIC");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\streammusic");
        CExoString sDefault(".\\streammusic");
        CExoString sAlias("STREAMMUSIC");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\movies");
        CExoString sDefault(".\\movies");
        CExoString sAlias("MOVIES");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("z:\\tempclient");
        CExoString sDefault(".\\tempclient");
        CExoString sAlias("TEMPCLIENT");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("z:\\currentgame");
        CExoString sDefault(".\\currentgame");
        CExoString sAlias("CURRENTGAME");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\hak");
        CExoString sDefault(".\\hak");
        CExoString sAlias("HAK");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\texturepacks");
        CExoString sDefault(".\\texturepacks");
        CExoString sAlias("TEXTUREPACKS");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\streamwaves");
        CExoString sDefault(".\\streamwaves");
        CExoString sAlias("STREAMWAVES");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("t:\\");
        CExoString sDefault(".\\downloads");
        CExoString sAlias("DOWNLOADS");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("t:\\");
        CExoString sDefault(".\\");
        CExoString sAlias("OPTIONS");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\ambient");
        CExoString sDefault(".\\ambient");
        CExoString sAlias("AMBIENT");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\patch");
        CExoString sDefault(".\\patch");
        CExoString sAlias("PATCH");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\portraits");
        CExoString sDefault(".\\portraits");
        CExoString sAlias("PORTRAITS");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\music");
        CExoString sDefault(".\\music");
        CExoString sAlias("MUSIC");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("z:\\currentgame");
        CExoString sDefault(".\\currentgame");
        CExoString sAlias("CURRENTGAME");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("z:\\gameinprogress");
        CExoString sDefault(".\\gameinprogress");
        CExoString sAlias("GAMEINPROGRESS");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("z:\\futuregame");
        CExoString sDefault(".\\futuregame");
        CExoString sAlias("FUTUREGAME");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\rims");
        CExoString sDefault(".\\rims");
        CExoString sAlias("RIMS");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\rimsxbox");
        CExoString sDefault(".\\rimsxbox");
        CExoString sAlias("RIMSXBOX");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("z:\\rebootdata");
        CExoString sDefault(".\\rebootdata");
        CExoString sAlias("REBOOTDATA");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("z:\\cache");
        CExoString sDefault(".\\");
        CExoString sAlias("CACHE");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }

    {
        CExoString sXboxPath("d:\\lips");
        CExoString sDefault(".\\lips");
        CExoString sAlias("LIPS");

        AddAlias(sAliasFile, sAlias, sDefault, sXboxPath);
    }
}
#endif
