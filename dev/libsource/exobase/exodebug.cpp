#include "exobase.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef _DEBUG
#include <stdarg.h>
#endif

// PC 0x006356C0
void* EXOMALLOC(unsigned long nSize, const char* szFile, int nLine)
{
    return malloc(nSize);
}
// PC 0x006356D0
void EXOFREE(void* pMemory, const char* szFile, int nLine)
{
    free(pMemory);
}
// PC 0x006356E0
CExoDebug::CExoDebug()
{
    m_pInternal = new CExoDebugInternal;
    m_bEnabled = 0;
}
// PC 0x00635750
void CExoDebug::FlushScriptLogFile()
{
    if (m_pInternal)
    {
        m_pInternal->FlushScriptLogFile();
    }
}
// PC 0x00635760
void CExoDebug::OpenLogFiles(const CExoString& path)
{
    if (m_pInternal)
    {
        m_pInternal->OpenLogFiles(path);
    }
}
// PC 0x00635770
void CExoDebug::Warning(int line, char* file, const char* message)
{
    if (m_pInternal)
    {
        m_pInternal->Warning(line, file, message);
    }
}

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
// PC 0x00635780
void CExoDebug::GetCurrentTimestamp(CExoString& timestamp)
{
    if (m_pInternal)
    {
        m_pInternal->GetCurrentTimestamp(timestamp);
    }
}
// PC 0x00635790
void CExoDebug::WriteToScriptLogFile(const CExoString& text)
{
    if (m_pInternal)
    {
        m_pInternal->WriteToScriptLogFile(text);
    }
}
// PC 0x006357A0
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
