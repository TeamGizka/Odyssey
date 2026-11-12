#include "exobase.h"

#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>
#include <eh.h>
#include <exception>
#include <new.h>
#include <stdlib.h>
#include <windows.h>

bool g_bDisplayServicedResources = false;
bool g_bExtendedResourceTracking = false;
// PC 0x00637A30
CExoDebugInternal::CExoDebugInternal()
{
    m_pScriptLogFile = 0;
    m_pLogFile = 0;
    m_pAuxiliaryLogFileOne = 0;
    m_pAuxiliaryLogFileTwo = 0;
    m_bLogFilesOpen = 0;
}

void CExoDebugInternal::Assert(int line, char* file, const char* message)
{
    CExoString sAssert;

    if (message == NULL)
    {
        sAssert.Format("An assertion has failed:\n\n File: %s\n\n"
                       "Line #: %d\n\n", file, line);
    }
    else
    {
        sAssert.Format("An assertion has failed:\n\n File: %s\n\n"
                       "Line #: %d \n\n"
                       "Programmer says: %s\n\n", file, line, message);
    }

    FILE* pFile = fopen("Logs\\swcAsserts.txt", "at");

    if (pFile)
    {
        fprintf(pFile, sAssert.CStr());
        fclose(pFile);
    }
}

void CExoDebugInternal::InitializeWarningFile()
{
    time_t tNow;
    char szHeader[256];

    time(&tNow);
    sprintf(szHeader,
            "\n--------------------------------------\n"
            "Messages for: %s"
            "--------------------------------------\n",
            ctime(&tNow));

    CExoString sHeader(szHeader);

    FILE* pFile = fopen("Logs\\swcWarnings.txt", "at");

    if (pFile)
    {
        fprintf(pFile, sHeader.CStr());
        fclose(pFile);
    }
}
// PC 0x00637B80
void CExoDebugInternal::Warning(int line, char* file, const char* message)
{
    CExoString sWarning;
    if (message == NULL)
    {
        sWarning.Format("A warning has occured:\n\n File: %s\n\n"
                        "Line #: %d\n\n"
                        "Press Abort to quit, Retry to debug, "
                        "or Ignore to continue",
                        file, line);
    }
    else
    {
        sWarning.Format("A warning has occured:\n\n File: %s\n\n"
                        "Line #: %d \n\n"
                        "Programmer says: %s\n\n"
                        "Press Abort to quit, Retry to debug, "
                        "or Ignore to continue",
                        file, line, message);
    }
    FILE* pFile = fopen("Logs\\swcWarnings.txt", "at");
    if (pFile)
    {
        fprintf(pFile, sWarning.CStr());
    }
    CExoString sLine;
    CExoString sComment;
    char* szFileName = strrchr(file, '\\');
    if (szFileName == NULL)
    {
        szFileName = file;
    }
    else
    {
        szFileName = szFileName + 1;
    }
    sLine.Format("WARNING: %s(%d) Comment:", szFileName, line);
#ifdef _XBOX
    void AurPostString(char* szString, int nColumn, int nRow, float fDuration);
    AurPostString(sLine.CStr(), 7, 10, 5.0f);
    int nRow = 11;
#else
    sLine.CStr();
#endif
    if (message)
    {
        sComment = message;
        int nLength = sComment.GetLength();
        while (nLength != 0)
        {
            if (nLength <= 50)
            {
                sLine = sComment;
                sComment = "";
            }
            else
            {
                sLine = sComment.Left(50);
                int nBreak = 50;
                const char* szBreak;

                szBreak = strrchr(sLine.CStr(), ' ');
                if (szBreak)
                {
                    nBreak = szBreak - sLine.CStr();
                }
                szBreak = strrchr(sLine.CStr(), '\t');
                if (szBreak)
                {
                    nBreak = szBreak - sLine.CStr();
                }
                szBreak = strrchr(sLine.CStr(), '\r');
                if (szBreak)
                {
                    nBreak = szBreak - sLine.CStr();
                }

                sLine = sComment.Left(nBreak);
                sComment = sComment.Right(nLength - nBreak);
            }

#ifdef _XBOX
            AurPostString(sLine.CStr(), 7, nRow, 15.0f);
            nRow++;
#else
            sLine.CStr();
#endif
            nLength = sComment.GetLength();
        }
    }

    if (pFile)
    {
        fclose(pFile);
    }
}
// PC 0x00637A50
void CExoDebugInternal::GetCurrentTimestamp(CExoString& timestamp)
{
    struct _timeb tb;
    char szTimestamp[20];

    _ftime(&tb);
    sprintf(szTimestamp, "%.19s", ctime(&tb.time));
    timestamp.Format(szTimestamp);
}
// PC 0x00637A90
void CExoDebugInternal::WriteToScriptLogFile(const CExoString& text)
{
    if (m_bLogFilesOpen)
    {
        m_pScriptLogFile->Write(text);
    }
}
// PC 0x00637AA0
void CExoDebugInternal::FlushScriptLogFile()
{
    if (m_bLogFilesOpen)
    {
        m_pScriptLogFile->Flush();
    }
}
// PC 0x005EE1C0
void CExoDebugInternal::OpenLogFiles(const CExoString& path)
{
}
// PC 0x00637AB0
void CExoDebugInternal::CloseLogFiles()
{
    if (m_bLogFilesOpen)
    {
        if (m_pLogFile)
        {
            delete m_pLogFile;
            m_pLogFile = NULL;
        }
        if (m_pAuxiliaryLogFileOne)
        {
            delete m_pAuxiliaryLogFileOne;
            m_pAuxiliaryLogFileOne = NULL;
        }
        if (m_pAuxiliaryLogFileTwo)
        {
            delete m_pAuxiliaryLogFileTwo;
            m_pAuxiliaryLogFileTwo = NULL;
        }
        if (m_pAuxiliaryLogFileThree)
        {
            delete m_pAuxiliaryLogFileThree;
            m_pAuxiliaryLogFileThree = NULL;
        }
        if (m_pAuxiliaryLogFileFour)
        {
            delete m_pAuxiliaryLogFileFour;
            m_pAuxiliaryLogFileFour = NULL;
        }
        if (m_pScriptLogFile)
        {
            delete m_pScriptLogFile;
            m_pScriptLogFile = NULL;
        }
        if (m_p1C)
        {
            delete m_p1C;
            m_p1C = NULL;
        }
        m_bLogFilesOpen = 0;
    }
}
// PC 0x00637E60
CExoDebugInternal::~CExoDebugInternal()
{
    CloseLogFiles();
}

static void ExoTranslateStructuredException(unsigned int nCode, EXCEPTION_POINTERS* pExceptionPointers)
{
    if (std::uncaught_exception())
    {
        abort();
    }
    throw nCode;
}

static void ExoTerminate()
{
    abort();
}

void ExoInstallExceptionHandlers()
{
    _set_se_translator(ExoTranslateStructuredException);
    set_terminate(ExoTerminate);
    _set_new_mode(1);
}
