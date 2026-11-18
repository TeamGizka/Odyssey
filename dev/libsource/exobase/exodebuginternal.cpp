#include "exobase.h"

#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>
#include <eh.h>
#include <exception>
#include <new.h>
#include <stdlib.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

#define EXODEBUGINTERNAL_DEBUG 0

bool g_bDisplayServicedResources = false;
bool g_bExtendedResourceTracking = false;
// PC 0x00637A30 GOG 0x00637930 Xbox 0x00149D50
CExoDebugInternal::CExoDebugInternal()
{
    m_pScriptLogFile = 0;
    m_pLogFile = 0;
    m_pAuxiliaryLogFileOne = 0;
    m_pAuxiliaryLogFileTwo = 0;
    m_bLogFilesOpen = 0;
}

// Dead code: not in the PC executable
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

    abort();
}

// Dead code: not in the PC executable
void CExoDebugInternal::FlushErrorFile()
{
    if (m_bLogFilesOpen)
    {
        m_pAuxiliaryLogFileOne->Flush();
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::FlushLogFile()
{
    if (m_bLogFilesOpen)
    {
        m_pLogFile->Flush();
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::FlushResourcesLogFile()
{
    if (m_bLogFilesOpen)
    {
        m_pAuxiliaryLogFileTwo->Flush();
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::FlushSoundLogFile()
{
    if (m_bLogFilesOpen)
    {
        m_pAuxiliaryLogFileThree->Flush();
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::FlushResourceTrackingFile()
{
    if (m_bLogFilesOpen)
    {
        m_pAuxiliaryLogFileFour->Flush();
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::FlushVOTestFile()  // Xbox 0x00149DD0
{
    if (m_bLogFilesOpen)
    {
#ifdef _XBOX
        m_pScriptLogFile->Flush();
#else
        m_pAuxiliaryLogFileFive->Flush();
#endif
    }
}

// Dead code: not in the PC executable
CExoFile* CExoDebugInternal::OpenLogFile(const CExoString& path, CExoString name)
{
    CExoString sFileName;
    CExoString sMode;
    CExoFile* pFile;

    sFileName.Format("LOGS:%s%s", path.CStr(), name.CStr());

    if (name == CExoString("Tracking"))
    {
        if (g_bExtendedResourceTracking)
        {
            pFile = new CExoFile(sFileName, 10, "a");
        }
        else
        {
            pFile = new CExoFile(sFileName, 10, "w");
        }
    }
    else
    {
        pFile = new CExoFile(sFileName, 10, "w");
    }

    return pFile;
}

// Dead code: not in the PC executable
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

#ifdef _XBOX
    FILE* pFile = fopen("t:\\Logs\\swcWarnings.txt", "at");
#else
    FILE* pFile = fopen("Logs\\swcWarnings.txt", "at");
#endif

    if (pFile)
    {
        fprintf(pFile, sHeader.CStr());
        fclose(pFile);
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::InitializeAssertFile()
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

    FILE* pFile = fopen("Logs\\swcAsserts.txt", "at");

    if (pFile)
    {
        fprintf(pFile, sHeader.CStr());
        fclose(pFile);
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::WriteToErrorFile(const CExoString& text)
{
    if (m_bLogFilesOpen)
    {
        m_pAuxiliaryLogFileOne->Write(text);
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::WriteToLogFile(const CExoString& text)
{
    if (m_bLogFilesOpen)
    {
        m_pLogFile->Write(text);
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::WriteToResourcesLogFile(const CExoString& text)
{
    if (m_bLogFilesOpen)
    {
        m_pAuxiliaryLogFileTwo->Write(text);
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::WriteToSoundLogFile(const CExoString& text)
{
    if (m_bLogFilesOpen)
    {
        m_pAuxiliaryLogFileThree->Write(text);
        FlushSoundLogFile();
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::WriteToResourceTrackingFile(const CExoString& text)
{
    if (m_bLogFilesOpen)
    {
        m_pAuxiliaryLogFileFour->Write(text);
        FlushResourceTrackingFile();
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::WriteToVOTestFile(const CExoString& text)
{
    if (m_bLogFilesOpen)
    {
#ifdef _XBOX
        m_pScriptLogFile->Write(text);
#else
        m_pAuxiliaryLogFileFive->Write(text);
#endif
        FlushVOTestFile();
    }
}

// Dead code: not in the PC executable
void CExoDebugInternal::WriteResourceToScreen(const CExoString& text,
                                              int nColumn, int nRow,
                                              float fDuration)
{
    if (g_bDisplayServicedResources)
    {
        text.CStr();
    }
}
// PC 0x00637A50 GOG 0x00637950 Xbox 0x00149D80
void CExoDebugInternal::GetCurrentTimestamp(CExoString& timestamp)
{
    struct _timeb tb;
    char szTimestamp[20];

    _ftime(&tb);
    sprintf(szTimestamp, "%.19s", ctime(&tb.time));
    timestamp.Format(szTimestamp);
}
// PC 0x00637A90 GOG 0x00637990 Xbox 0x00149DC0
void CExoDebugInternal::WriteToScriptLogFile(const CExoString& text)
{
    if (m_bLogFilesOpen)
    {
        m_pScriptLogFile->Write(text);
    }
}
// PC 0x00637AA0 GOG 0x006379A0 Xbox 0x00149DD0
void CExoDebugInternal::FlushScriptLogFile()
{
    if (m_bLogFilesOpen)
    {
        m_pScriptLogFile->Flush();
    }
}
// PC 0x005EE1C0 GOG 0x005EE1A0 Xbox 0x00149D70
void CExoDebugInternal::OpenLogFiles(const CExoString& path)
{
}
// PC 0x00637AB0 GOG 0x006379B0 Xbox 0x00149DE0
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
#ifndef _XBOX
        if (m_pAuxiliaryLogFileFive)
        {
            delete m_pAuxiliaryLogFileFive;
            m_pAuxiliaryLogFileFive = NULL;
        }
#endif
        m_bLogFilesOpen = 0;
    }
}

// PC 0x00637B80 GOG 0x00637A70 Xbox 0x00149EA0
void CExoDebugInternal::Warning(int line, char* file, const char* message)
{
    if (EXODEBUGINTERNAL_DEBUG)
    {
        CloseLogFiles();
    }
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
#ifdef _XBOX
    FILE* pFile = fopen("t:\\Logs\\swcWarnings.txt", "at");
#else
    FILE* pFile = fopen("Logs\\swcWarnings.txt", "at");
#endif
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
    sLine.CStr();
    if (message)
    {
        sComment = message;
        int nLength;
        while ((nLength = sComment.GetLength()) != 0)
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
                if (szBreak && szBreak != sLine.CStr())
                {
                    nBreak = szBreak - sLine.CStr();
                }
                szBreak = strrchr(sLine.CStr(), '\t');
                if (szBreak && szBreak != sLine.CStr())
                {
                    nBreak = szBreak - sLine.CStr();
                }
                szBreak = strrchr(sLine.CStr(), '\r');
                if (szBreak && szBreak != sLine.CStr())
                {
                    nBreak = szBreak - sLine.CStr();
                }

                sLine = sComment.Left(nBreak);
                sComment = sComment.Right(nLength - nBreak);
            }

            sLine.CStr();
        }
    }

    if (pFile)
    {
        fclose(pFile);
    }
}
// PC 0x00637E60 GOG 0x00637D50 Xbox 0x0014A110
CExoDebugInternal::~CExoDebugInternal()
{
    CloseLogFiles();
}

// Dead code: not in the PC executable
static void ExoTranslateStructuredException(unsigned int nCode, EXCEPTION_POINTERS* pExceptionPointers)
{
    if (std::uncaught_exception())
    {
        abort();
    }
    throw nCode;
}

// Dead code: not in the PC executable
static void ExoTerminate()
{
    abort();
}

// Dead code: not in the PC executable
void ExoInstallExceptionHandlers()
{
    _set_se_translator(ExoTranslateStructuredException);
    set_terminate(ExoTerminate);
    _set_new_mode(1);
}
