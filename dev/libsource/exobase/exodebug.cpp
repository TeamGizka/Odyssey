#include "exobase.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef _DEBUG
#include <stdarg.h>
#endif

// PC 0x006356C0 GOG 0x006355B0 Xbox 0x00149C70
void* EXOMALLOC(unsigned long nSize, const char* szFile, int nLine)
{
    return malloc(nSize);
}
// PC 0x006356D0 GOG 0x006355C0 Xbox 0x00149C80
void EXOFREE(void* pMemory, const char* szFile, int nLine)
{
    free(pMemory);
}
// PC 0x006356E0 GOG 0x006355D0 Xbox 0x00149C90
CExoDebug::CExoDebug()
{
    m_pInternal = new CExoDebugInternal;
    m_bEnabled = 0;
}

// Dead code: not in the PC executable
void CExoDebug::CloseLogFiles()
{
}

// Dead code: not in the PC executable
void CExoDebug::FlushErrorFile()
{
}

// Dead code: not in the PC executable
void CExoDebug::FlushLogFile()
{
}
// PC 0x00635750 GOG 0x00635640 Xbox 0x00149CD0
void CExoDebug::FlushScriptLogFile()
{
    if (m_pInternal)
    {
        m_pInternal->FlushScriptLogFile();
    }
}

// Dead code: not in the PC executable
void CExoDebug::FlushResourcesLogFile()  // Xbox 0x00149CD0
{
    if (m_pInternal)
    {
        m_pInternal->FlushResourcesLogFile();
    }
}

// Dead code: not in the PC executable
void CExoDebug::FlushVOTestFile()  // Xbox 0x00149CD0
{
    if (m_pInternal)
    {
        m_pInternal->FlushVOTestFile();
    }
}

// Dead code: not in the PC executable
unsigned long CExoDebug::GetCurrentAllocatedMemory()
{
    return 0;
}

// Dead code: not in the PC executable
unsigned long CExoDebug::GetMaxAllocatedMemory()
{
    return 0;
}

// Dead code: not in the PC executable
unsigned long CExoDebug::GetTotalMemoryAllocations()
{
    return 0;
}

// Dead code: not in the PC executable
unsigned long CExoDebug::GetCurrentMemoryAllocations()
{
    return 0;
}
// PC 0x00635760 GOG 0x00635650 Xbox 0x00149CE0
void CExoDebug::OpenLogFiles(const CExoString& path)
{
    if (m_pInternal)
    {
        m_pInternal->OpenLogFiles(path);
    }
}
// PC 0x00635770 GOG 0x00635660 Xbox 0x00149CF0
void CExoDebug::Warning(int line, char* file, const char* message)
{
    if (m_pInternal)
    {
        m_pInternal->Warning(line, file, message);
    }
}

// Dead code: not in the PC executable
void CExoDebug::WriteToErrorFile(const CExoString& text)
{
}

// Dead code: not in the PC executable
void CExoDebug::WriteToLogFile(const CExoString& text)
{
}

// Dead code: not in the PC executable
void CExoDebug::WriteToResourcesLogFile(const CExoString& text)
{
    if (m_pInternal)
    {
        m_pInternal->WriteToResourcesLogFile(text);
    }
}

// Dead code: not in the PC executable
void CExoDebug::WriteToSoundLogFile(const CExoString& text)
{
}

// Dead code: not in the PC executable
void CExoDebug::WriteToResourceTrackingFile(const CExoString& text)
{
    if (m_pInternal)
    {
        m_pInternal->WriteToResourceTrackingFile(text);
    }
}

// Dead code: not in the PC executable
void CExoDebug::WriteToVOTestFile(const CExoString& text)
{
    if (m_pInternal)
    {
        m_pInternal->WriteToVOTestFile(text);
    }
}

// Dead code: not in the PC executable
void CExoDebug::WriteResourceToScreen(const CExoString& text, int nColumn,
                                      int nRow, float fDuration)
{
    if (m_pInternal)
    {
        m_pInternal->WriteResourceToScreen(text, nColumn, nRow, fDuration);
    }
}

// Dead code: not in the PC executable
void CExoDebug::Assert(int line, char* file, const char* message)
{
    CExoString sAssert;

    if (m_pInternal)
    {
        m_pInternal->Assert(line, file, message);
    }

    if (message == NULL)
    {
        sAssert.Format("An Assertion failed in %s\n\nLine number %d", file,
                       line);
    }
    else
    {
        sAssert.Format("An Assertion failed in %s\n\nLine number %d \n\n"
                       "Programmer says: %s", file, line, message);
    }

    printf("%s", sAssert.CStr());
}
// PC 0x00635780 GOG 0x00635670 Xbox 0x00149D00
void CExoDebug::GetCurrentTimestamp(CExoString& timestamp)
{
    if (m_pInternal)
    {
        m_pInternal->GetCurrentTimestamp(timestamp);
    }
}
// PC 0x00635790 GOG 0x00635680 Xbox 0x00149D10
void CExoDebug::WriteToScriptLogFile(const CExoString& text)
{
    if (m_pInternal)
    {
        m_pInternal->WriteToScriptLogFile(text);
    }
}
// PC 0x006357A0 GOG 0x00635690 Xbox 0x00149D20
CExoDebug::~CExoDebug()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}

#ifdef _DEBUG
void CExoDebug::Log(const char* format, ...)
{
    char sBuffer[1024];
    va_list args;
    va_start(args, format);
    _vsnprintf(sBuffer, sizeof(sBuffer) - 1, format, args);
    va_end(args);
    sBuffer[sizeof(sBuffer) - 1] = 0;
    WriteToLogFile(CExoString(sBuffer));
}
#endif
