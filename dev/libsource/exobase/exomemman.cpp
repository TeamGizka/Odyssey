#include "exomemman.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

extern int usedtexturememory;
extern int g_bProcessModuleMemUsage;
extern char* g_szProcessModuleMemName;

void AurPostString(char* szString, int nColumn, int nRow, float fDuration);

int g_nTotalNewAllocated;
int g_nTotalContiguousMemory;
int s_nLastFreeComment;
static char s_szHeapWalkLine[EXOMEMMAN_WALK_LINE_LENGTH];

CExoMemHeap* CExoMemHeap::m_lHeapsStart;

// Dead code: not in the PC executable
CExoMemMan* GetMemMan()
{
    static CExoMemMan cMemMan;
    return &cMemMan;
}

// Dead code: not in the PC executable
int SetFileLine(const char* szFile, int nLine)
{
    if (GetMemMan()->m_bTrackFileLine)
    {
        GetMemMan()->SetLastFile(szFile);
        GetMemMan()->SetLastLine(nLine);
    }
    return 0;
}

// Dead code: not in the PC executable
void CExoMemMan::SetLastFile(const char* szFile)
{
    m_szLastFile = szFile;
}

// Dead code: not in the PC executable
void CExoMemMan::SetLastLine(int nLine)
{
    m_nLastLine = nLine;
}

// Dead code: not in the PC executable
int SetComment(const char* szComment)
{
    GetMemMan()->SetLastComment(szComment);
    return 0;
}

// Dead code: not in the PC executable
void CExoMemMan::SetLastComment(const char* szComment)
{
    if (szComment != NULL)
    {
        strncpy(m_szLastComment, szComment, EXOMEMMAN_COMMENT_LENGTH);
    }
    else
    {
        m_szLastComment[0] = '\0';
    }
}

// Dead code: not in the PC executable
int SetLineFileComment(const char* szFile, int nLine)
{
    char szLine[EXOMEMMAN_LINE_LENGTH];

    sprintf(szLine, "%s:%d", szFile, nLine);
    GetMemMan()->SetLastComment(szLine);
    return 0;
}

// Dead code: not in the PC executable
CExoMemMan::CExoMemMan()
{
    m_nTrackedAllocations = 0;
    Clear();
    GetFileIndex("none");
    m_bTrackFileLine = 1;
    m_bLogMemTrackOutput = 0;
    m_pLogFile = fopen(EXOMEMMAN_REPORT_FILE, "wt");
    if (m_pLogFile != NULL)
    {
        time_t nTime;
        time(&nTime);
        fprintf(m_pLogFile, ">> File Opened: %s\n\n", ctime(&nTime));
    }
}

// Dead code: not in the PC executable
void CExoMemMan::Clear()
{
    int nComment;

    memset(m_aszFileNames, 0, sizeof(m_aszFileNames));
    for (nComment = 0; nComment < EXOMEMMAN_MAX_COMMENTS; nComment++)
    {
        m_aszComments[nComment][0] = '\0';
    }
    m_szLastFile = NULL;
    m_szLastComment[0] = '\0';
    m_nLastLine = -1;
    memset(m_anTypeCount, 0, sizeof(m_anTypeCount));
    m_anTypeStack[0] = 0;
    m_nTypeStackSize = 1;
}

// Dead code: not in the PC executable
int CExoMemMan::GetFileIndex(char* szFile)
{
    int nFile;

    if (szFile == NULL)
    {
        return 0;
    }
    for (nFile = 0; nFile < EXOMEMMAN_MAX_FILES; nFile++)
    {
        if (m_aszFileNames[nFile] == szFile)
        {
            return nFile;
        }
        if (m_aszFileNames[nFile] == NULL)
        {
            m_aszFileNames[nFile] = szFile;
            return nFile;
        }
    }
    return -1;
}

// Dead code: not in the PC executable
CExoMemMan::~CExoMemMan()
{
    fclose(m_pLogFile);
}

// Dead code: not in the PC executable
void CExoMemMan::Destroy()
{
}

// Dead code: not in the PC executable
int CExoMemMan::PushType(unsigned long nType)
{
    if (m_nTypeStackSize < EXOMEMMAN_TYPE_STACK_SIZE && nType < EXOMEMMAN_MAX_TYPES)
    {
        m_anTypeStack[m_nTypeStackSize] = (unsigned char)nType;
        m_nTypeStackSize++;
        return 1;
    }
    return 0;
}

// Dead code: not in the PC executable
int CExoMemMan::PopType(unsigned long nType)
{
    if (m_nTypeStackSize > 1)
    {
        m_nTypeStackSize--;
        return m_anTypeStack[m_nTypeStackSize] == nType;
    }
    return 0;
}

// Dead code: not in the PC executable
void CExoMemMan::CheckHeaps()
{
    CExoMemHeap* pHeap = CExoMemHeap::m_lHeapsStart;

    while (pHeap != NULL)
    {
        pHeap->CheckHeap();
    }
}

// Dead code: not in the PC executable
CExoMemHeap* CExoMemMan::GetHeaps()
{
    return CExoMemHeap::m_lHeapsStart;
}

// Dead code: not in the PC executable
void PrintBasicMemLine(FILE* pFile, int nSize, char* szName, int bUsed)
{
    fprintf(pFile, "%d \t 0x%08X \t %d \t %d ", 0, 0, nSize, bUsed ? 1 : 2);
    fprintf(pFile, "\n");
}

// Dead code: not in the PC executable
void CExoMemMan::Heap_Walk(int bToFile)
{
    char szFileName[EXOMEMMAN_FILENAME_LENGTH];
    time_t nTime;
    struct tm* pTime;
    CExoMemHeap* pHeap;
    FILE* pContentsFile;
    FILE* pStatsFile;

    time(&nTime);
    pTime = localtime(&nTime);
    for (pHeap = CExoMemHeap::m_lHeapsStart; pHeap != NULL; pHeap = pHeap->m_pNext)
    {
        if (bToFile)
        {
            sprintf(szFileName, "logs\\HeapContents_%s_%d%02d%02d%02d%02d.txt", pHeap->m_szName,
                    pTime->tm_year + 1900, pTime->tm_mon, pTime->tm_mday, pTime->tm_hour, pTime->tm_min);
            if (g_bProcessModuleMemUsage)
            {
                sprintf(szFileName, "logs\\%010d_%s.txt",
                        g_nTotalNewAllocated + usedtexturememory + g_nTotalContiguousMemory,
                        g_szProcessModuleMemName);
            }
            pContentsFile = fopen(szFileName, "wt");
            sprintf(szFileName, "logs\\HeapStats_%s_%d%02d%02d%02d%02d.txt", pHeap->m_szName,
                    pTime->tm_year + 1900, pTime->tm_mon, pTime->tm_mday, pTime->tm_hour, pTime->tm_min);
            pStatsFile = fopen(szFileName, "wt");
            if (pContentsFile != NULL && pStatsFile != NULL)
            {
                pHeap->WalkHeap(pContentsFile, pStatsFile);
                PrintBasicMemLine(pContentsFile, usedtexturememory, NULL, 1);
                fclose(pContentsFile);
                fclose(pStatsFile);
            }
        }
        else
        {
            pHeap->WalkHeap(NULL, NULL);
        }
    }
}

// Dead code: not in the PC executable
const char* CExoMemMan::GetLastFile()
{
    return m_szLastFile;
}

// Dead code: not in the PC executable
char* CExoMemMan::GetLastComment()
{
    return m_szLastComment;
}

// Dead code: not in the PC executable
int CExoMemMan::GetLastLine()
{
    return m_nLastLine;
}

// Dead code: not in the PC executable
int CExoMemMan::AddComment(char* szComment)
{
    while (szComment[0] != '\0' && m_nNumComments < EXOMEMMAN_MAX_COMMENTS)
    {
        for (; s_nLastFreeComment < EXOMEMMAN_MAX_COMMENTS; s_nLastFreeComment++)
        {
            if (m_aszComments[s_nLastFreeComment][0] == '\0')
            {
                break;
            }
        }
        if (s_nLastFreeComment != EXOMEMMAN_MAX_COMMENTS)
        {
            strncpy(m_aszComments[s_nLastFreeComment], szComment, EXOMEMMAN_COMMENT_LENGTH);
            m_aszComments[s_nLastFreeComment][EXOMEMMAN_COMMENT_LENGTH - 1] = '\0';
            m_nNumComments++;
            return s_nLastFreeComment;
        }
        s_nLastFreeComment = 0;
    }
    return -1;
}

// Dead code: not in the PC executable
void CExoMemMan::RemoveComment(int nComment)
{
    if (nComment != -1 && m_aszComments[nComment][0] != '\0')
    {
        m_aszComments[nComment][0] = '\0';
        m_nNumComments--;
    }
}

// Dead code: not in the PC executable
char* CExoMemMan::GetFileName(int nFile)
{
    return m_aszFileNames[nFile];
}

// Dead code: not in the PC executable
char* CExoMemMan::GetComment(int nComment)
{
    if (nComment == -1)
    {
        return "-";
    }
    return m_aszComments[nComment];
}

// Dead code: not in the PC executable
void CExoMemMan::PrintMemTrackStatsOutput()
{
    char szLine[EXOMEMMAN_STATS_LINE_LENGTH];
    CExoMemHeap* pHeap;
    int nRow;

    if (m_bPrintMemTrackStats)
    {
        GetMemMan();
        nRow = 5;
        for (pHeap = CExoMemHeap::m_lHeapsStart; pHeap != NULL; pHeap = pHeap->m_pNext)
        {
            sprintf(szLine, "Heap: %s", pHeap->m_szName);
            AurPostString(szLine, 5, nRow++, 0.1f);
            sprintf(szLine, "Blocks: %21d | %10d / %10d", pHeap->m_aTrackStats[2], pHeap->m_aTrackStats[1],
                    pHeap->m_aTrackStats[0]);
            AurPostString(szLine, 7, nRow++, 0.1f);
            sprintf(szLine, "Bytes: %10d(%10d) | %10d / %10d", pHeap->m_aTrackStats[4], pHeap->m_aTrackStats[5],
                    pHeap->m_aTrackStats[6], pHeap->m_aTrackStats[3]);
            AurPostString(szLine, 7, nRow, 0.1f);
            nRow += 2;
        }
    }
}

// Dead code: not in the PC executable
void CExoMemMan::LogMemTrackOutputAlloc(int nAddress, int nSize, int nFile, int nComment)
{
    time_t nTime;

    if (m_bLogMemTrackOutput && m_pLogFile != NULL)
    {
        time(&nTime);
        fprintf(m_pLogFile, " %d | %d | %s | %s | %s", nAddress, nSize, m_aszFileNames[nFile], GetComment(nComment),
                ctime(&nTime));
    }
}

// Dead code: not in the PC executable
void CExoMemMan::LogMemTrackOutputFree(int nAddress, int nSize, int nFile, int nComment)
{
    time_t nTime;

    if (m_bLogMemTrackOutput && m_pLogFile != NULL)
    {
        time(&nTime);
        fprintf(m_pLogFile, "-%d | %d | %s | %s | %s", nAddress, nSize, m_aszFileNames[nFile], GetComment(nComment),
                ctime(&nTime));
    }
}

// Dead code: not in the PC executable
void CExoMemMan::Update()
{
}

// Dead code: not in the PC executable
void CExoMemMan::PrintHeapWalk(int nDuration)
{
    CExoMemHeap* pHeap;
    int nRow;

    GetMemMan();
    nRow = 7;
    for (pHeap = CExoMemHeap::m_lHeapsStart; pHeap != NULL; pHeap = pHeap->m_pNext)
    {
        sprintf(s_szHeapWalkLine, "Heap: %s", pHeap->m_szName);
        AurPostString(s_szHeapWalkLine, 5, nRow - 1, (float)nDuration);
        sprintf(s_szHeapWalkLine, "         %s        %s         %s", "Total", "Used", "Free");
        AurPostString(s_szHeapWalkLine, 5, nRow, (float)nDuration);
        sprintf(s_szHeapWalkLine, "Blocks: [%10d] [%10d] [%10d]", pHeap->m_aWalkStats[6], pHeap->m_aWalkStats[8],
                pHeap->m_aWalkStats[7]);
        AurPostString(s_szHeapWalkLine, 5, nRow + 1, (float)nDuration);
        sprintf(s_szHeapWalkLine, "         %s      %s     %s", "Largest", "Smallest", "Average");
        AurPostString(s_szHeapWalkLine, 5, nRow + 2, (float)nDuration);
        sprintf(s_szHeapWalkLine, "Used  : [%10d] [%10d] [%10d]", pHeap->m_aWalkStats[1], pHeap->m_aWalkStats[0],
                pHeap->m_aWalkStats[2]);
        AurPostString(s_szHeapWalkLine, 5, nRow + 3, (float)nDuration);
        sprintf(s_szHeapWalkLine, "Free  : [%10d] [%10d] [%10d]", pHeap->m_aWalkStats[4], pHeap->m_aWalkStats[3],
                pHeap->m_aWalkStats[5]);
        AurPostString(s_szHeapWalkLine, 5, nRow + 4, (float)nDuration);
        nRow += 7;
    }
}

// Dead code: not in the PC executable
void CExoMemHeap::SetHeapName(const char* szName)
{
    sprintf(m_szName, "%s-%s", szName, m_szType);
}

// Dead code: not in the PC executable
void CExoMemHeap::RemoveHeap(CExoMemHeap* pHeap)
{
    CExoMemHeap* pPrevious = m_lHeapsStart;

    if (pPrevious == pHeap)
    {
        m_lHeapsStart = pHeap->m_pNext;
        return;
    }
    while (pPrevious->m_pNext != pHeap)
    {
        pPrevious = pPrevious->m_pNext;
    }
    pPrevious->m_pNext = pHeap->m_pNext;
}

// Dead code: not in the PC executable
void CExoMemHeap::AddHeap(CExoMemHeap* pHeap)
{
    CExoMemHeap* pLast;

    pHeap->m_pNext = NULL;
    if (m_lHeapsStart == NULL)
    {
        m_lHeapsStart = pHeap;
        return;
    }
    pLast = m_lHeapsStart;
    while (pLast->m_pNext != NULL)
    {
        pLast = pLast->m_pNext;
    }
    pLast->m_pNext = pHeap;
}
