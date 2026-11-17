#ifndef EXOMEMMAN_H
#define EXOMEMMAN_H

#include <stdio.h>

#define EXOMEMMAN_MAX_TYPES 256
#define EXOMEMMAN_TYPE_STACK_SIZE 1024
#define EXOMEMMAN_MAX_FILES 1000
#define EXOMEMMAN_MAX_COMMENTS 40000
#define EXOMEMMAN_COMMENT_LENGTH 128
#define EXOMEMHEAP_NAME_LENGTH 128
#define EXOMEMMAN_LINE_LENGTH 1024
#define EXOMEMMAN_STATS_LINE_LENGTH 512
#define EXOMEMMAN_WALK_LINE_LENGTH 256
#define EXOMEMMAN_FILENAME_LENGTH 128
#define EXOMEMMAN_HEAP_TRACK_STATS 7
#define EXOMEMMAN_HEAP_WALK_STATS 9
#define EXOMEMMAN_REPORT_FILE "Logs\\swcMemReport.txt"

class CExoMemHeap
{
public:
    virtual ~CExoMemHeap() {}
    virtual void* Alloc(unsigned long nSize) = 0;
    virtual void* Realloc(void* pMemory, unsigned long nSize) = 0;
    virtual void Free(void* pMemory) = 0;
    virtual unsigned long GetSize(void* pMemory) = 0;
    virtual void WalkHeap(FILE* pContentsFile, FILE* pStatsFile) = 0;
    virtual void CheckHeap() = 0;

    void SetHeapName(const char* szName);

    static void RemoveHeap(CExoMemHeap* pHeap);
    static void AddHeap(CExoMemHeap* pHeap);

    static CExoMemHeap* m_lHeapsStart;

    CExoMemHeap* m_pNext;
    unsigned long m_aTrackStats[EXOMEMMAN_HEAP_TRACK_STATS];
    unsigned long m_aWalkStats[EXOMEMMAN_HEAP_WALK_STATS];
    char m_szName[EXOMEMHEAP_NAME_LENGTH];
    char m_szType[EXOMEMHEAP_NAME_LENGTH];
};

class CExoMemMan
{
public:
    CExoMemMan();
    ~CExoMemMan();

    void Clear();
    void Destroy();
    void Update();

    int GetFileIndex(char* szFile);
    char* GetFileName(int nFile);
    int AddComment(char* szComment);
    void RemoveComment(int nComment);
    char* GetComment(int nComment);

    void SetLastFile(const char* szFile);
    void SetLastLine(int nLine);
    void SetLastComment(const char* szComment);
    const char* GetLastFile();
    char* GetLastComment();
    int GetLastLine();

    int PushType(unsigned long nType);
    int PopType(unsigned long nType);

    void CheckHeaps();
    CExoMemHeap* GetHeaps();
    void Heap_Walk(int bToFile);
    void PrintHeapWalk(int nDuration);
    void PrintMemTrackStatsOutput();
    void LogMemTrackOutputAlloc(int nAddress, int nSize, int nFile, int nComment);
    void LogMemTrackOutputFree(int nAddress, int nSize, int nFile, int nComment);

    int m_nTrackedAllocations;
    int m_anTypeCount[EXOMEMMAN_MAX_TYPES];
    unsigned char m_anTypeStack[EXOMEMMAN_TYPE_STACK_SIZE];
    unsigned long m_nTypeStackSize;
    const char* m_szLastFile;
    char m_szLastComment[EXOMEMMAN_COMMENT_LENGTH];
    int m_nLastLine;
    char* m_aszFileNames[EXOMEMMAN_MAX_FILES];
    char m_aszComments[EXOMEMMAN_MAX_COMMENTS][EXOMEMMAN_COMMENT_LENGTH];
    int m_nNumComments;
    int m_bTrackFileLine;
    int m_bLogMemTrackOutput;
    int m_bPrintMemTrackStats;
    FILE* m_pLogFile;
};

CExoMemMan* GetMemMan();
int SetFileLine(const char* szFile, int nLine);
int SetComment(const char* szComment);
int SetLineFileComment(const char* szFile, int nLine);
void PrintBasicMemLine(FILE* pFile, int nSize, char* szName, int bUsed);

#endif
