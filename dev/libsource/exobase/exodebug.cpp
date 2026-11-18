#include "exobase.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef _DEBUG
#include <stdarg.h>
#endif
// PC 0x005E9350  Xbox 0x00134D70
void* ExoMalloc(unsigned long nSize)
{
    return malloc(nSize);
}
// PC 0x005E9360  Xbox 0x00134D80
void ExoFree(void* pMemory)
{
    free(pMemory);
}
// PC 0x005E9370  Xbox 0x00134D90
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
// PC 0x005E93E0  Xbox 0x00134E00
void CExoDebug::FlushScriptLogFile()
{
    if (m_pInternal)
    {
        m_pInternal->FlushScriptLogFile();
    }
}
// Dead code: not in the PC executable
void CExoDebug::FlushResourcesLogFile()
{
    if (m_pInternal)
    {
        m_pInternal->FlushResourcesLogFile();
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
// PC 0x005E93F0  Xbox 0x00134E10
void CExoDebug::OpenLogFiles(const CExoString& path)
{
    if (m_pInternal)
    {
        m_pInternal->OpenLogFiles(path);
    }
}
// PC 0x005E9400  Xbox 0x00134E20
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
    abort();
}
// PC 0x005E9410  Xbox 0x00134E30
void CExoDebug::GetCurrentTimestamp(CExoString& timestamp)
{
    if (m_pInternal)
    {
        m_pInternal->GetCurrentTimestamp(timestamp);
    }
}
// PC 0x005E9420  Xbox 0x00134E40
void CExoDebug::WriteToScriptLogFile(const CExoString& text)
{
    if (m_pInternal)
    {
        m_pInternal->WriteToScriptLogFile(text);
    }
}
// PC 0x005E9430  Xbox 0x00134E50
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
