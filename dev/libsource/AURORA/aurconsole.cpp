#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "exobase.h"
#include "aurconsole.h"
#include "aurmodel.h"

const int MAX_CONSOLE_LINE = 16384;

ConsoleFunc* ConsoleFuncs[16384];

char junkstring[256];
char returnvalue[1024];
int ConsoleFuncs_num = 0;
int g_nHistoryPosition = 0;
bool g_bKeepHistory = false;
void* (*AurGetResource)(char*, char*, int*) = 0;
void (*AurReleaseResource)(void*, int) = 0;
List<char*> g_listHistory;

funcholderbase::~funcholderbase()
{
}
// PC 0x0045FF80 GOG 0x00460090
template <> void funcholder<void (*)()>::Engage(char* sArguments)
{
    m_pFunction();
    returnvalue[0] = 0;
}
// PC 0x0045FF90 GOG 0x004600A0
template <> void funcholder<void (*)(char*)>::Engage(char* sArguments)
{
    m_pFunction(sArguments);
    returnvalue[0] = 0;
}
// PC 0x0045FFB0 GOG 0x004600C0
template <> void funcholder<void (*)(int)>::Engage(char* sArguments)
{
    int nValue;

    sscanf(sArguments, "%d", &nValue);
    m_pFunction(nValue);
    returnvalue[0] = 0;
}
// PC 0x0045FFE0 GOG 0x004600F0
void ConsoleFuncs_Add(ConsoleFunc* pFunc)
{
    ConsoleFunc* pTemp;

    for (int i = 0; i < ConsoleFuncs_num; i++)
    {
        if (strcmp(pFunc->m_sName, ConsoleFuncs[i]->m_sName) < 0)
        {
            pTemp = pFunc;
            pFunc = ConsoleFuncs[i];
            ConsoleFuncs[i] = pTemp;
        }
    }

    ConsoleFuncs[ConsoleFuncs_num++] = pFunc;
}
// PC 0x00460070 GOG 0x00460180
void ConsoleFuncs_Remove(const char* sName)
{
    int nCount = ConsoleFuncs_num;
    int i;

    for (i = 0; i < nCount; i++)
    {
        if (strcmp(sName, ConsoleFuncs[i]->m_sName) == 0)
        {
            break;
        }
    }

    for (; i < ConsoleFuncs_num - 1; i++)
    {
        ConsoleFuncs[i] = ConsoleFuncs[i + 1];
    }

    ConsoleFuncs_num--;
}
// PC 0x004600F0 GOG 0x00460200
funcholderbase* FindConCallBack(char* sName)
{
    static int i = -1;

    if (i >= 0 && i < ConsoleFuncs_num &&
        strcmp(sName, ConsoleFuncs[i]->m_sName) == 0)
    {
        return ConsoleFuncs[i]->m_pHolder;
    }

    for (i = 0; i < ConsoleFuncs_num; i++)
    {
        if (strcmp(sName, ConsoleFuncs[i]->m_sName) == 0)
        {
            return ConsoleFuncs[i]->m_pHolder;
        }
    }

    return 0;
}
// PC 0x004601D0 GOG 0x004602E0
char* FuncPreInterp(char* sLine)
{
    char sName[128];
    int i;
    funcholderbase* pHolder;

    returnvalue[0] = 0;

    for (i = 0; *sLine != 0 && *sLine != ' ' && *sLine != '='; sLine++)
    {
        sName[i++] = *sLine;
    }

    sName[i] = 0;

    while (*sLine == '=' || *sLine == ' ')
    {
        sLine++;
    }

    pHolder = FindConCallBack(sName);

    if (pHolder != 0)
    {
        pHolder->PreEngage(sLine);
    }

    return returnvalue;
}
// PC 0x00460250 GOG 0x00460360
ConsoleFunc::~ConsoleFunc()
{
    ConsoleFuncs_Remove(m_sName);
    delete m_pHolder;
}

List<OpenResource*> Resources;
#ifdef _XBOX

OpenResource::OpenResource(int nBufferSize)
{
    m_pFile = 0;
    m_nLines = -1;
    m_nBufferSize = max(nBufferSize, 0x2000);
    m_pBuffer = 0;
    m_pData = 0;
    m_nDataLeft = 0;
    m_pResource = 0;
}

OpenResource::~OpenResource()
{
    if (m_pBuffer != 0)
    {
        delete m_pBuffer;
        m_pBuffer = 0;
    }
}
#endif
// PC 0x00460270 GOG 0x00460380
char* AurResGetDataBytes(unsigned long nBytes, void* pResource)
{
    OpenResource* pOpenResource = (OpenResource*)pResource;

    if (pOpenResource->m_pFile == 0)
    {
        char* pData = pOpenResource->m_pData;
        pOpenResource->m_pData += nBytes;
        pOpenResource->m_nDataLeft -= nBytes;
        return pData;
    }

    char* pData = new char[nBytes];
    fread(pData, 1, nBytes, pOpenResource->m_pFile);
    return pData;
}
// PC 0x004602B0 GOG 0x004603C0
void AurResFreeDataBytes(void* pResource, unsigned char* pData)
{
    OpenResource* pOpenResource = (OpenResource*)pResource;

    if (pOpenResource->m_pFile != 0 && pData != 0)
    {
        delete[] pData;
    }
}
// PC 0x004602D0 GOG 0x004603E0
OpenResource* GetResource(void* pResource)
{
    int i;

    if (pResource == 0)
    {
        return Resources.m_pList[Resources.m_nCount - 1];
    }

    for (i = 0; i < Resources.m_nCount; i++)
    {
        if (pResource == Resources.m_pList[i])
        {
            return Resources.m_pList[i];
        }
    }

    return 0;
}

#ifdef _XBOX
void AurResRemove(void* pResource)
{
    for (int nIndex = 0; nIndex < Resources.m_nCount; nIndex++)
    {
        if (pResource == Resources.m_pList[nIndex])
        {
            Resources.DelIndex(nIndex);
            return;
        }
    }
}
#else
int AurResRemove(void* pResource)  // PC 0x00460320 GOG 0x00460430
{
    int nIndex = 0;

    if (Resources.m_nCount > 0)
    {
        do
        {
            if (pResource == Resources.m_pList[nIndex])
            {
                if (nIndex < --Resources.m_nCount)
                {
                    for (; nIndex < Resources.m_nCount; nIndex++)
                    {
                        Resources.m_pList[nIndex] =
                            Resources.m_pList[nIndex + 1];
                    }
                }

                break;
            }
        } while (++nIndex < Resources.m_nCount);
    }

    return nIndex;
}
#endif
// PC 0x00460380 GOG 0x00460490
static char* getnextline_file(void* pResource)
{
    OpenResource* pOpenResource = GetResource(pResource);

    pOpenResource->m_nLines++;

    if (pOpenResource->m_pBuffer == 0)
    {
        pOpenResource->m_pBuffer = new char[pOpenResource->m_nBufferSize];
        pOpenResource->m_pBuffer[0] = 0;
    }

    return fgets(pOpenResource->m_pBuffer, 4000, pOpenResource->m_pFile);
}
// PC 0x004603D0 GOG 0x004604E0
static char* getnextline_res(void* pResource)
{
    OpenResource* pOpenResource = GetResource(pResource);
    char* pData = pOpenResource->m_pData;
    int i = 0;

    if (pOpenResource->m_nDataLeft <= 0)
    {
        return 0;
    }

    if (pOpenResource->m_pBuffer == 0)
    {
        pOpenResource->m_pBuffer = new char[pOpenResource->m_nBufferSize];
        pOpenResource->m_pBuffer[0] = 0;
    }

    do
    {
        pOpenResource->m_pBuffer[i] = *pData;
        i++;
        pData++;
    } while (i < pOpenResource->m_nDataLeft &&
             pOpenResource->m_pBuffer[i - 1] != '\n');

    pOpenResource->m_nDataLeft -= i;
    pOpenResource->m_pData = pData;

    if (pOpenResource->m_nDataLeft < 0)
    {
        return 0;
    }

    pOpenResource->m_pBuffer[i] = 0;
    return pOpenResource->m_pBuffer;
}
// PC 0x00460440 GOG 0x00460550
char* GetDataFromFile(int nBytes)
{
    OpenResource* pOpenResource = Resources.m_pList[Resources.m_nCount - 1];

    if (pOpenResource->m_pBuffer == 0)
    {
        pOpenResource->m_pBuffer = new char[pOpenResource->m_nBufferSize];
        pOpenResource->m_pBuffer[0] = 0;
    }

    fread(pOpenResource->m_pBuffer, 1, nBytes, pOpenResource->m_pFile);
    return pOpenResource->m_pBuffer;
}
// PC 0x00460490 GOG 0x004605A0
static char* GetDataFromResource(int nBytes)
{
    OpenResource* pOpenResource =
        Resources.m_pList[Resources.m_nCount - 1];
    char* pData = pOpenResource->m_pData;
    int i = 0;

    if (pOpenResource->m_pBuffer == 0)
    {
        pOpenResource->m_pBuffer = new char[pOpenResource->m_nBufferSize];
        pOpenResource->m_pBuffer[0] = 0;
    }

    do
    {
        pOpenResource->m_pBuffer[i] = *pData;
        i++;
        pData++;
    } while (i < pOpenResource->m_nDataLeft && i < nBytes);

    pOpenResource->m_nDataLeft -= i;
    pOpenResource->m_pData = pData;

    if (pOpenResource->m_nDataLeft < 0)
    {
        return 0;
    }

    pOpenResource->m_pBuffer[i] = 0;
    return pOpenResource->m_pBuffer;
}
// PC 0x00460500 GOG 0x00460610
char* AurResGetNextLine()
{
    if (Resources.m_nCount == 0)
    {
        return 0;
    }

    if (Resources.m_pList[Resources.m_nCount - 1]->m_pFile != 0)
    {
        return getnextline_file(0);
    }

    return getnextline_res(0);
}
// PC 0x00460530 GOG 0x00460640
void* AurResGetData(int nBytes)
{
    if (Resources.m_nCount == 0)
    {
        return 0;
    }

    if (Resources.m_pList[Resources.m_nCount - 1]->m_pFile != 0)
    {
        return GetDataFromFile(nBytes);
    }

    return GetDataFromResource(nBytes);
}
// PC 0x004605A0 GOG 0x004606B0
char* CommandCompletion(char* sLine, List<char*>& lstCompletions)
{
    char* sPartial;
    char* sOutput;
    int nLength;
    int i;

    lstCompletions.m_nCount = 0;
    returnvalue[0] = 0;

    if (sLine == 0)
    {
        return returnvalue;
    }

    strcpy(returnvalue, sLine);
    sOutput = returnvalue;
    sPartial = sLine;
    nLength = strlen(sLine);

    for (i = 0; i < nLength; i++)
    {
        if (sLine[i] == ' ')
        {
            sPartial = &sLine[i + 1];
            sOutput = &returnvalue[i + 1];
        }
    }

    for (i = 0; i < ConsoleFuncs_num; i++)
    {
        if (strncmp(sPartial, ConsoleFuncs[i]->m_sName, strlen(sPartial)) == 0)
        {
            char* sName = ConsoleFuncs[i]->m_sName;

            lstCompletions.Add(sName);
        }
    }

    if (lstCompletions.m_nCount != 0)
    {
        if (lstCompletions.m_nCount == 1)
        {
            sprintf(sOutput, "%s ", lstCompletions.m_pList[0]);
            return returnvalue;
        }

        i = 0;
        char cChar;

        do
        {
            cChar = lstCompletions.m_pList[0][i];

            for (int j = 0; cChar != 0 && j < lstCompletions.m_nCount; j++)
            {
                if (cChar != lstCompletions.m_pList[j][i])
                {
                    cChar = 0;
                }
            }

            sOutput[i++] = cChar;
        } while (cChar != 0);
    }

    return returnvalue;
}

char* AurResGetNextLine(void* pResource)
{
    OpenResource* pOpenResource = GetResource(pResource);

    if (pOpenResource == 0)
    {
        return 0;
    }

    if (pOpenResource->m_pFile != 0)
    {
#ifdef _XBOX
        return getnextline_file(pOpenResource);
    }

    return getnextline_res(pOpenResource);
#else
        return getnextline_file(pResource);
    }

    return getnextline_res(pResource);
#endif
}
// PC 0x00460750 GOG 0x00460860
char* FuncInterp(char* sLine)
{
    char sName[128];
    char sBuffer[MAX_CONSOLE_LINE];
    char sArguments[MAX_CONSOLE_LINE];
    char sTemp[MAX_CONSOLE_LINE];
    char* pStart;
    char* pEnd;
    char* sResult;
    int nDepth;
    int i;
    funcholderbase* pHolder;

    while (*sLine == ' ' || *sLine == '\t')
    {
        sLine++;
    }

    if (g_bKeepHistory)
    {
        char* sHistory = new char[strlen(sLine) + 1];
        strcpy(sHistory, sLine);

        if (g_listHistory.m_nCount == 0 ||
            strcmp(sHistory, g_listHistory.m_pList[g_listHistory.m_nCount - 1]) != 0)
        {
            g_listHistory.Add(sHistory);
            g_nHistoryPosition = g_listHistory.m_nCount;
        }
    }

    returnvalue[0] = 0;
    strcpy(sBuffer, sLine);
    sLine = sBuffer;

    pStart = sBuffer;

    if (sBuffer[0] == '"')
    {
        while (*pStart != 0)
        {
            if (*pStart == '"')
            {
                *pStart = ' ';
                break;
            }

            pStart++;
        }
    }

    for (pStart = sBuffer; *pStart != 0; pStart++)
    {
        if (*pStart == '"')
        {
            pStart++;

            while (*pStart != 0 && *pStart != '"')
            {
                pStart++;
            }
        }

        if (*pStart == '[')
        {
            pEnd = pStart;
            nDepth = 1;

            do
            {
                pEnd++;

                if (*pEnd == 0)
                {
                    strcpy(returnvalue, "missing ']'");
                    return returnvalue;
                }

                if (*pEnd == '[')
                {
                    nDepth++;
                }

                if (*pEnd == ']')
                {
                    nDepth--;
                }
            } while (nDepth != 0);

            *pEnd = 0;
            *pStart = 0;

            sResult = FuncInterp(pStart + 1);

            strcpy(sTemp, sBuffer);
            strcat(sTemp, sResult);
            strcat(sTemp, pEnd + 1);
            strcpy(sBuffer, sTemp);

            pStart = sBuffer;
        }
    }

    while (*sLine == ' ' || *sLine == '\t')
    {
        sLine++;
    }

    for (i = 0; *sLine != 0 && *sLine != ' ' && *sLine != '='; sLine++)
    {
        sName[i++] = *sLine;
    }

    sName[i] = 0;

    while (*sLine == '=' || *sLine == ' ')
    {
        sLine++;
    }

    pHolder = FindConCallBack(sName);

    if (pHolder == 0)
    {
        sprintf(returnvalue, "Function %s not found", sName);
        return returnvalue;
    }

    strcpy(sArguments, sLine);

    for (pEnd = sArguments + strlen(sArguments); pEnd > sArguments; *--pEnd = 0)
    {
        if (pEnd[-1] != ' ' && pEnd[-1] != '\t' && pEnd[-1] != '\r' &&
            pEnd[-1] != '\n')
        {
            break;
        }
    }

    pHolder->Engage(sArguments);
    return returnvalue;
}
// PC 0x00460AC0 GOG 0x00460BD0
ConsoleFunc::ConsoleFunc(const char* sName, void (*pFunction)())
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<void (*)()>(pFunction);
    ConsoleFuncs_Add(this);
}
// PC 0x00460B20 GOG 0x00460C30
ConsoleFunc::ConsoleFunc(const char* sName, void (*pFunction)(char*))
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<void (*)(char*)>(pFunction);
    ConsoleFuncs_Add(this);
}
// PC 0x00460B80 GOG 0x00460C90
ConsoleFunc::ConsoleFunc(const char* sName, void (*pFunction)(int))
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<void (*)(int)>(pFunction);
    ConsoleFuncs_Add(this);
}
// PC 0x00460BE0 GOG 0x00460CF0
static OpenResource* GetOpenResource(char* sName, char* sExtension, int* pnSize)
{
    int nSize;
    void* pData = (void*)AurGetResource(sName, sExtension, &nSize);

    if (pData == 0)
    {
        return 0;
    }

#ifdef _XBOX
    OpenResource* pOpenResource = new OpenResource(0);
#else
    OpenResource* pOpenResource = new OpenResource(0x2000);
#endif

    pOpenResource->m_pResource = pData;
    pOpenResource->m_pData = (char*)pData;

    if (nSize >= 0)
    {
        pOpenResource->m_nDataLeft = nSize;
    }
    else
    {
        pOpenResource->m_nDataLeft = -1 - nSize;
    }

    if (pnSize != 0)
    {
        *pnSize = nSize;
    }

    Resources.Add(pOpenResource);
    return pOpenResource;
}

#ifdef _XBOX

static const int MAX_MISSING_FILES = 1024;
static const int MISSING_FILE_NAME_LENGTH = 64;
static char g_aMissingFiles[MAX_MISSING_FILES][MISSING_FILE_NAME_LENGTH];
static int g_nMissingFiles;

void* AurResGet(char* sName, char* sExtension, int* pnSize, bool bRaw)
{
    char sPath[MISSING_FILE_NAME_LENGTH];

    if (sExtension == 0)
    {
        sExtension = "";
    }

    if (AurGetResource != 0)
    {
        OpenResource* pResource = GetOpenResource(sName, sExtension, pnSize);

        if (pResource != 0)
        {
            return pResource;
        }

        if (bRaw)
        {
            return 0;
        }
    }

    const char* sSeparator;

    if (*sExtension == 0 || *sExtension != '.')
    {
        sSeparator = ".";
    }
    else
    {
        sSeparator = "";
    }

    sprintf(sPath, "d:\\%s%s%s", sName, sSeparator, sExtension);

    for (int i = 0; i < g_nMissingFiles; i++)
    {
        if (strcmp(sPath, g_aMissingFiles[i]) == 0)
        {
            return 0;
        }
    }

    FILE* pFile = fopen(sPath, "rb");

    if (pFile == 0)
    {
        strcpy(g_aMissingFiles[g_nMissingFiles++], sPath);
        return 0;
    }

    OpenResource* pOpenResource = new OpenResource(500000);

    pOpenResource->m_pFile = pFile;
    pOpenResource->m_nLines = 0;

    Resources.Add(pOpenResource);

    if (pnSize != 0)
    {
        *pnSize = 1;
    }

    return pOpenResource;
}
#else
void* AurResGet(char* sName, char* sExtension, int* pnSize, bool bRaw)  // PC 0x00460CA0 GOG 0x00460DB0
{
    char sPath[260];

    if (sExtension == 0)
    {
        sExtension = "";
    }

    if (AurGetResource != 0)
    {
        OpenResource* pResource = GetOpenResource(sName, sExtension, pnSize);

        if (pResource != 0)
        {
            return pResource;
        }

        if (bRaw)
        {
            return 0;
        }
    }

    const char* sSeparator;

    if (*sExtension == 0 || *sExtension != '.')
    {
        sSeparator = ".";
    }
    else
    {
        sSeparator = "";
    }

    sprintf(sPath, "%s%s%s", sName, sSeparator, sExtension);

    FILE* pFile = fopen(sPath, "rb");

    if (pFile == 0)
    {
        return 0;
    }

    OpenResource* pOpenResource = new OpenResource(500000);

    pOpenResource->m_pFile = pFile;
    pOpenResource->m_nLines = 0;

    Resources.Add(pOpenResource);

    if (pnSize != 0)
    {
        *pnSize = 1;
    }

    return pOpenResource;
}
#endif
// PC 0x00460DD0 GOG 0x00460EE0
void* AurResGet(FILE* pFile)
{
    OpenResource* pOpenResource = new OpenResource(500000);

    pOpenResource->m_pFile = pFile;
    pOpenResource->m_nLines = 0;

    Resources.Add(pOpenResource);
    return pOpenResource;
}
// PC 0x00460E50 GOG 0x00460F60
void AurResFree(void* pResource, int nSize)
{
    OpenResource* pOpenResource = (OpenResource*)pResource;

    if (pOpenResource != 0)
    {
        if (pOpenResource->m_pFile != 0)
        {
            fclose(pOpenResource->m_pFile);
            pOpenResource->m_pFile = 0;
        }
        else
        {
            AurReleaseResource(pOpenResource->m_pResource, nSize);
            pOpenResource->m_pResource = 0;
        }

        if (Resources.Contains(pOpenResource))
        {
            Resources.Remove(pOpenResource);
        }

#ifdef _XBOX
        delete pOpenResource;
#else
        if (pOpenResource->m_pBuffer != 0)
        {
            delete pOpenResource->m_pBuffer;
            pOpenResource->m_pBuffer = 0;
        }

        delete pOpenResource;
#endif
    }
}

Model* FindModel(char* sName);

#ifndef _XBOX
char* converttobinary(char* sArguments)
{
    char sPattern[MAX_PATH];
    char sDirectory[MAX_PATH];
    char sCurrentDirectory[MAX_PATH];
    char sModelName[MAX_PATH];
    WIN32_FIND_DATA cFindData;
    HANDLE hFind;

    GetCurrentDirectory(MAX_PATH, sCurrentDirectory);
    if (sscanf(sArguments, "%s %s", sPattern, sDirectory) != 2)
        return "FAILURE";

    memset(&cFindData, 0, sizeof(cFindData));
    hFind = FindFirstFile(sPattern, &cFindData);
    if (hFind == INVALID_HANDLE_VALUE)
        return "Finished";

    do
    {
        sprintf(sModelName, "%s", cFindData.cFileName);
        sModelName[strlen(sModelName) - 4] = 0;
        if (!SetCurrentDirectory(sDirectory))
            return "FAILURE";
        Model* pModel = FindModel(sModelName);
        if (pModel != 0)
        {
            IODispatcher::GetRef()->WriteSync(pModel, NULL);
        }
        SetCurrentDirectory(sCurrentDirectory);
    } while (FindNextFile(hFind, &cFindData));

    if (GetLastError() != ERROR_NO_MORE_FILES)
        return "FAILURE";
    FindClose(hFind);
    return "Finished";
}
#endif
// PC 0x00460EE0 GOG 0x00460FF0
char* dofile(char* sFileName)
{
    static char returnbuf[256];
    char sName[64];
    char sExtension[16];
    int i;

    i = 0;
    while (*sFileName != 0 && *sFileName != '.')
    {
        sName[i] = *sFileName++;
        i++;
    }
    sName[i] = 0;

    i = 0;
    while (*sFileName != 0)
    {
        sExtension[i++] = *sFileName++;
    }
    sExtension[i] = 0;

    void* pResource = AurResGet(sName, sExtension, 0, false);

    if (pResource == 0)
    {
        sprintf(returnbuf, "ERROR:  file '%s' doesn't exist", sFileName);
        return returnbuf;
    }

    while (1)
    {
#ifdef _XBOX
        char* sLine = AurResGetNextLine(pResource);
#else
        OpenResource* pOpenResource = GetResource(pResource);
        if (pOpenResource == 0)
        {
            break;
        }
        char* sLine;
        if (pOpenResource->m_pFile != 0)
        {
            sLine = getnextline_file(pOpenResource);
        }
        else
        {
            sLine = getnextline_res(pOpenResource);
        }
#endif
        if (sLine == 0)
        {
            break;
        }
        while (*sLine == ' ' || *sLine == '\t')
        {
            sLine++;
        }

        if (*sLine != 0 && *sLine != '#')
        {
            char* sEnd = sLine + strlen(sLine) - 1;
            for (; (ptrdiff_t)sEnd >= (ptrdiff_t)sLine; sEnd--)
            {
                if (*sEnd != '\n' && *sEnd != '\r' &&
                    *sEnd != '\t' && *sEnd != ' ')
                {
                    break;
                }
                *sEnd = 0;
            }

            FuncInterp(sLine);
        }
    }

    AurResFree(pResource, 0);
    sprintf(returnbuf, "Done executing file '%s'", sFileName);
    return returnbuf;
}

#ifdef _XBOX

template <> void funcholder<int (*)(char*)>::Engage(char* sArguments)
{
    returnvalue[0] = 0;
    sprintf(returnvalue, "%d", m_pFunction(sArguments));
}

template <> void funcholder<float (*)(char*)>::Engage(char* sArguments)
{
    sprintf(returnvalue, "%f", m_pFunction(sArguments));
}

template <> void funcholder<char* (*)(char*)>::Engage(char* sArguments)
{
    char* sResult = m_pFunction(sArguments);

    if (sResult == 0)
    {
        sResult = "";
    }

    sprintf(returnvalue, "%s", sResult);
}

template <> void funcholder<int*>::Engage(char* sArguments)
{
    returnvalue[0] = 0;
    sscanf(sArguments, "%d", m_pFunction);
    sprintf(returnvalue, "%d", *m_pFunction);
}

template <> void funcholder<int*>::PreEngage(char* sArguments)
{
    sprintf(returnvalue, "%d", *m_pFunction);
}

template <> void funcholder<float*>::Engage(char* sArguments)
{
    returnvalue[0] = 0;
    sscanf(sArguments, "%f", m_pFunction);
    sprintf(returnvalue, "%f", *m_pFunction);
}

template <> void funcholder<float*>::PreEngage(char* sArguments)
{
    sprintf(returnvalue, "%f", *m_pFunction);
}

template <> void funcholder<Vector*>::Engage(char* sArguments)
{
    returnvalue[0] = 0;
    sscanf(sArguments, "%f %f %f", &m_pFunction->m_fX, &m_pFunction->m_fY,
           &m_pFunction->m_fZ);
    sprintf(returnvalue, "%f %f %f", m_pFunction->m_fX, m_pFunction->m_fY,
            m_pFunction->m_fZ);
}

template <> void funcholder<Vector*>::PreEngage(char* sArguments)
{
    sprintf(returnvalue, "%f %f %f", m_pFunction->m_fX, m_pFunction->m_fY,
            m_pFunction->m_fZ);
}

template <> void funcholder<Quaternion*>::Engage(char* sArguments)
{
    returnvalue[0] = 0;

    float fYaw = Yaw(*m_pFunction);
    float fPitch = Pitch(*m_pFunction);
    float fRoll = Roll(*m_pFunction);

    sscanf(sArguments, "%f %f %f", &fYaw, &fPitch, &fRoll);
    *m_pFunction = YawPitchRoll(fYaw, fPitch, fRoll);
    sprintf(returnvalue, "%f %f %f", fYaw, fPitch, fRoll);
}

template <> void funcholder<Quaternion*>::PreEngage(char* sArguments)
{
    returnvalue[0] = 0;

    float fYaw = Yaw(*m_pFunction);
    float fPitch = Pitch(*m_pFunction);

    sprintf(returnvalue, "%f %f %f", fYaw, fPitch, Roll(*m_pFunction));
}

void HistoryOn()
{
    g_bKeepHistory = true;
}

void HistoryOff()
{
    g_bKeepHistory = false;
}

int DeleteFuncWithPrefix(char* sPrefix)
{
    int nDeleted = 0;
    int i = ConsoleFuncs_num;

    while (i-- != 0)
    {
        if (strncmp(ConsoleFuncs[i]->m_sName, sPrefix, strlen(sPrefix)) == 0)
        {
            delete ConsoleFuncs[i];
            nDeleted++;
        }
    }

    return nDeleted;
}

char* echo(char* sArguments)
{
    return sArguments;
}

const int MAX_CONSOLE_OPERANDS = 6;
const int MAX_CONSOLE_OPERAND_BUFFER = 10;

char* add(char* sArguments)
{
    static char returnbuf[256];
    char sTemp[64];
    float afValue[MAX_CONSOLE_OPERAND_BUFFER];
    int nRead;
    int nCount;
    int i;

    returnbuf[0] = 0;

    for (nCount = 0; nCount < MAX_CONSOLE_OPERANDS;
         nCount++, sArguments += nRead)
    {
        if (sscanf(sArguments, "%f%n", &afValue[nCount], &nRead) != 1)
        {
            break;
        }
    }

    int nHalf = nCount / 2;

    for (i = 0; i < nHalf; i++)
    {
        sprintf(sTemp, " %f ", afValue[i] + afValue[i + nHalf]);
        strcat(returnbuf, sTemp);
    }

    return returnbuf;
}

char* sub(char* sArguments)
{
    static char returnbuf[256];
    char sTemp[64];
    float afValue[MAX_CONSOLE_OPERAND_BUFFER];
    int nRead;
    int nCount;
    int i;

    returnbuf[0] = 0;

    for (nCount = 0; nCount < MAX_CONSOLE_OPERANDS;
         nCount++, sArguments += nRead)
    {
        if (sscanf(sArguments, "%f%n", &afValue[nCount], &nRead) != 1)
        {
            break;
        }
    }

    int nHalf = nCount / 2;

    for (i = 0; i < nHalf; i++)
    {
        sprintf(sTemp, " %f ", afValue[i] - afValue[i + nHalf]);
        strcat(returnbuf, sTemp);
    }

    return returnbuf;
}

char* mul(char* sArguments)
{
    static char returnbuf[256];
    char sTemp[64];
    float afValue[MAX_CONSOLE_OPERAND_BUFFER];
    int nRead;
    int nCount;
    int i;

    returnbuf[0] = 0;

    for (nCount = 0; nCount < MAX_CONSOLE_OPERANDS;
         nCount++, sArguments += nRead)
    {
        if (sscanf(sArguments, "%f%n", &afValue[nCount], &nRead) != 1)
        {
            break;
        }
    }

    for (i = 0; i < nCount - 1; i++)
    {
        sprintf(sTemp, " %f ", afValue[i] * afValue[nCount - 1]);
        strcat(returnbuf, sTemp);
    }

    return returnbuf;
}

char* mulqv(char* sArguments)
{
    static char returnbuf[256];
    float fYaw;
    float fPitch;
    float fRoll;

    returnbuf[0] = 0;

    Vector vVector;

    if (sscanf(sArguments, "%f%f%f%f%f%f", &fYaw, &fPitch, &fRoll,
               &vVector.m_fX, &vVector.m_fY, &vVector.m_fZ) != 6)
    {
        return "mulqv: invalid operands";
    }

    vVector = YawPitchRoll(fYaw, fPitch, fRoll) * vVector;
    sprintf(returnbuf, " %g %g %g ", vVector.m_fX, vVector.m_fY,
            vVector.m_fZ);
    return returnbuf;
}

char* outputmodel(char* sArguments)
{
    Model* pModel = FindModel(GetToken(&sArguments));

    if (pModel == 0)
    {
        return "FAILURE";
    }

    IODispatcher* pDispatcher = IODispatcher::GetRef();

    if (!pDispatcher->WriteSync(pModel, NULL))
    {
        return "FAILURE";
    }

    return "SUCCESS";
}

char* bload(char* sArguments)
{
    IODispatcher* pDispatcher = IODispatcher::GetRef();

    if (pDispatcher->ReadSync(sArguments) == 0)
    {
        return "FAILURE";
    }

    return "SUCCESS";
}

const char* spawn(char* sArguments);

char* converttobinary(char* sArguments)
{
    char sModelName[MAX_PATH];
    WIN32_FIND_DATA cFindData;
    HANDLE hFind;
    char* sPattern = GetToken(&sArguments);
    char* sDirectory = GetToken(&sArguments);

    if (sPattern == "")
    {
        return "FAILURE";
    }

    if (sDirectory == "")
    {
        return "FAILURE";
    }

    IODispatcher::GetRef();
    memset(&cFindData, 0, sizeof(cFindData));
    hFind = FindFirstFile(sPattern, &cFindData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            sprintf(sModelName, cFindData.cFileName);
            sModelName[strlen(sModelName) - 4] = 0;
            spawn(sModelName);

            for (int i = strlen(sModelName - 5); i > -1; i--)
            {
                if (sModelName[i] == '\\' || sModelName[i] == '/')
                {
                    break;
                }
            }
        } while (FindNextFile(hFind, &cFindData));

        if (GetLastError() != ERROR_NO_MORE_FILES)
        {
            return "FAILURE";
        }
    }

    FindClose(hFind);
    return "Finished";
}

int logbase2(unsigned int nValue);

char* testlogbase2(char* sArguments)
{
    static char returnbuf[256];

    sprintf(returnbuf, "%i\n", logbase2(atoi(GetToken(&sArguments))));
    return returnbuf;
}

extern List<Gob*> gobs;

char* getgobfrommodel(char* sArguments)
{
    char* sModel = GetToken(&sArguments);

    for (int i = 0; i < gobs.m_nCount; i++)
    {
        Gob* pGob = gobs.m_pList[i];

        if (pGob->m_pModel != 0 &&
            stricmp(pGob->m_pModel->m_sName, sModel) == 0)
        {
            return pGob->m_sName;
        }
    }

    return "NULL";
}

ConsoleFunc::ConsoleFunc(const char* sName, int (*pFunction)(char*))
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<int (*)(char*)>(pFunction);
    ConsoleFuncs_Add(this);
}

ConsoleFunc::ConsoleFunc(const char* sName, float (*pFunction)(char*))
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<float (*)(char*)>(pFunction);
    ConsoleFuncs_Add(this);
}

ConsoleFunc::ConsoleFunc(const char* sName, char* (*pFunction)(char*))
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<char* (*)(char*)>(pFunction);
    ConsoleFuncs_Add(this);
}

ConsoleFunc::ConsoleFunc(const char* sName, int* pVariable)
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<int*>(pVariable);
    ConsoleFuncs_Add(this);
}

ConsoleFunc::ConsoleFunc(const char* sName, float* pVariable)
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<float*>(pVariable);
    ConsoleFuncs_Add(this);
}

ConsoleFunc::ConsoleFunc(const char* sName, Vector* pVariable)
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<Vector*>(pVariable);
    ConsoleFuncs_Add(this);
}

ConsoleFunc::ConsoleFunc(const char* sName, Quaternion* pVariable)
{
    strcpy(m_sName, sName);
    m_pHolder = new funcholder<Quaternion*>(pVariable);
    ConsoleFuncs_Add(this);
}

ConsoleFunc ConFHistoryOn("HistoryOn", HistoryOn);
ConsoleFunc ConFHistoryOff("HistoryOff", HistoryOff);
ConsoleFunc ConFecho("echo", echo);
ConsoleFunc ConFadd("add", add);
ConsoleFunc ConFsub("sub", sub);
ConsoleFunc ConFmul("mul", mul);
ConsoleFunc ConFmulqv("mulqv", mulqv);
ConsoleFunc ConFdofile("dofile", dofile);
ConsoleFunc ConFgetgobfrommodel("getgobfrommodel", getgobfrommodel);
ConsoleFunc ConFoutputmodel("outputmodel", outputmodel);
ConsoleFunc ConFbload("bload", bload);
ConsoleFunc ConFconverttobinary("converttobinary", converttobinary);
ConsoleFunc ConFtestlogbase2("testlogbase2", testlogbase2);
#endif

#ifdef _XBOX
char g_sObjectFuncName[256];
#endif
