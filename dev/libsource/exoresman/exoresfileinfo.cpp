#include <stdio.h>
#include <string.h>
#include <time.h>

#define EXORESFILEINFO_LOG "fileinfo.txt"
#define EXORESFILEINFO_LINE 256

static char g_szFileInfoLine[EXORESFILEINFO_LINE];
static FILE* g_pFileInfoLog;
static long g_nFileInfoSize;
static long g_nFileInfoTime;
static int g_nFileInfoCount = 0;

// Dead code: not in the PC executable
long ExoGetFileInfoSize(const char* szFileName)
{
    FILE* pFile = fopen(szFileName, "rb");
    if (pFile == NULL)
    {
        return 0;
    }
    fseek(pFile, 0, SEEK_END);
    g_nFileInfoSize = ftell(pFile);
    fclose(pFile);
    return g_nFileInfoSize;
}

// Dead code: not in the PC executable
struct tm* ExoGetFileInfoTime()
{
    time(&g_nFileInfoTime);
    return localtime(&g_nFileInfoTime);
}

// Dead code: not in the PC executable
long ExoLogFileInfo(const char* szFileName)
{
    struct tm* pTime;

    if (g_pFileInfoLog == NULL)
    {
        g_pFileInfoLog = fopen(EXORESFILEINFO_LOG, "w");
        if (g_pFileInfoLog == NULL)
        {
            return 0;
        }
    }
    if (ExoGetFileInfoSize(szFileName) != 0)
    {
        pTime = ExoGetFileInfoTime();
        strncpy(g_szFileInfoLine, ctime(&g_nFileInfoTime), EXORESFILEINFO_LINE - 1);
        g_szFileInfoLine[EXORESFILEINFO_LINE - 1] = 0;
        fprintf(g_pFileInfoLog, "%-64s %10ld %04d%02d%02d %s %s", szFileName, g_nFileInfoSize,
                pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, "Used",
                g_szFileInfoLine);
        g_nFileInfoCount++;
        return g_nFileInfoSize;
    }
    fprintf(g_pFileInfoLog, "%-64s %10s %s\n", szFileName, "-", "none");
    return 0;
}
