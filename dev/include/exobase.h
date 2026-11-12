#ifndef __EXOBASE_H__
#define __EXOBASE_H__

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

#include <stdlib.h>
#include <string.h>

extern char g_szEmptyString[];




#ifdef _XBOX
void ExoXboxRestart();
#define EXOXBOXASSERT(x) if (!(x)) ExoXboxRestart()
#else
#define EXOXBOXASSERT(x)
#endif









class CExoString
{
public:
    char* m_sString;
    unsigned long m_nBufferLength;

    CExoString();
    CExoString(const char* szString);
    CExoString(const CExoString& sString);
    CExoString(const char* szString, int nLength);
    CExoString(int nValue);
    ~CExoString();

    CExoString& operator=(const CExoString& sString);
    CExoString& operator=(const char* szString);

    BOOL operator==(const CExoString& sString) const;
    BOOL operator==(const char* szString) const;
    BOOL operator!=(const CExoString& sString) const;
    BOOL operator!=(const char* szString) const;
    BOOL operator<(const CExoString& sString) const;
    BOOL operator>(const CExoString& sString) const;
    BOOL operator<=(const CExoString& sString) const;
    BOOL operator>=(const CExoString& sString) const;
    char operator[](int nIndex) const;
    CExoString operator+(const CExoString& sString) const;

    int AsINT() const
    {
        if (m_sString != NULL)
        {
            return atoi(m_sString);
        }

        return 0;
    }
    float AsFLOAT() const
    {
        if (m_sString != NULL)
        {
            return atof(m_sString);
        }

        return 0.0f;
    }
    char* CStr() const;
    void Format(char* szFormat, ...);
    int GetLength() const
    {
        if (m_sString != NULL)
        {
            return strlen(m_sString);
        }

        return 0;
    }
    void Insert(const CExoString& sString, int nPosition);
    BOOL IsEmpty() const
    {
        if (m_sString == NULL || m_sString[0] == 0)
        {
            return TRUE;
        }

        return FALSE;
    }
    BOOL CompareNoCase(const char* szString) const;
    BOOL CompareNoCase(const CExoString& sString) const;
    BOOL ComparePrefixNoCase(const CExoString& sString, int nSize) const;
    BOOL StripNonAlphaNumeric(BOOL bLowerCase, BOOL bAllowEmail,
                              BOOL bAllowPunctuation);
    int Find(const CExoString& sString, int nStart) const;
    int Find(char cChar, int nStart) const;
    CExoString Left(int nCount) const;
    CExoString Right(int nCount) const;
    CExoString SubString(int nStart, int nCount) const;
    CExoString LowerCase() const;
    CExoString UpperCase() const;
};

class CSWSObjectActionNode;
class CExoLinkedListNode
{
public:
    CExoLinkedListNode();

    CExoLinkedListNode* m_pPrev;
    CExoLinkedListNode* m_pNext;
    void* m_pData;
};

class CExoLinkedListInternal
{
public:
    CExoLinkedListInternal();
    ~CExoLinkedListInternal();
    CExoLinkedListNode* AddAfter(void* pData, CExoLinkedListNode* pPosition);
    CExoLinkedListNode* AddBefore(void* pData, CExoLinkedListNode* pPosition);
    void* GetAtPos(CExoLinkedListNode* pPosition) const;
    void* GetHead() const
    {
        if (m_pHead == 0)
        {
            return 0;
        }
        return m_pHead->m_pData;
    }
    void* GetTail() const
    {
        if (m_pTail == 0)
        {
            return 0;
        }
        return m_pTail->m_pData;
    }
    void* GetNext(CExoLinkedListNode*& pPosition) const;
    void* GetPrev(CExoLinkedListNode*& pPosition) const;
    void* Remove(CExoLinkedListNode* pPosition);
    void* RemoveHead();
    void* RemoveTail();
    int Contains(void* pData) const;
    CExoLinkedListNode* AddHead(void* pData);
    CExoLinkedListNode* AddTail(void* pData);

    CExoLinkedListNode* m_pHead;
    CExoLinkedListNode* m_pTail;
    int m_nCount;
};





class CExoStringList
{
public:
    CExoStringList();
    CExoStringList(int nSortMode, int nDuplicateMode, int nSize);
    ~CExoStringList();

    void Add(CExoString* pString);
    void AddSorted(CExoString* pString);
    void Clear();
    CExoString* GetString(int nIndex);
    void Insert(CExoString* pString, int nPosition);
    CExoStringList* Merge(CExoStringList* pList, int nSortMode,
                          int nDuplicateMode);

    int GetCount();
    CExoString* operator[](int nIndex);

private:
    void Introduce(CExoString* pString, int nPosition);

public:
    CExoString** m_pStrings;
    int m_nCount;
    int m_nDuplicateMode;
    int m_nSortMode;
    int m_nAllocated;
};





template <class T> class CExoLinkedList
{
public:
    CExoLinkedList();

    ~CExoLinkedList();

    CExoLinkedListNode* AddHead(T* pData);

    CExoLinkedListNode* AddTail(T* pData);

    CExoLinkedListNode* AddAfter(T* pData, CExoLinkedListNode* pPosition);

    CExoLinkedListNode* AddBefore(T* pData, CExoLinkedListNode* pPosition);

    int Contains(T* pData);

    T* GetHead();

    CExoLinkedListNode* GetHeadPos();

    T* GetTail();

    CExoLinkedListNode* GetTailPos();

    T* Remove(CExoLinkedListNode* pPosition);

    T* RemoveHead();

    T* RemoveTail();

    unsigned long Count();

    int IsEmpty();

    T* GetAtPos(CExoLinkedListNode* pPosition);

    T* GetNext(CExoLinkedListNode*& pPosition);

    T* GetPrev(CExoLinkedListNode*& pPosition);

    CExoLinkedListInternal* m_pInternal;
};

template <class T> inline CExoLinkedList<T>::CExoLinkedList()
{
    m_pInternal = new CExoLinkedListInternal;
}
// PC 0x00685200
template <class T> inline CExoLinkedList<T>::~CExoLinkedList()
{
    delete m_pInternal;
}
// PC 0x00622930
template <class T> inline CExoLinkedListNode* CExoLinkedList<T>::AddHead(T* pData)
{
    return m_pInternal->AddHead(pData);
}

template <class T> inline CExoLinkedListNode* CExoLinkedList<T>::AddTail(T* pData)
{
    return m_pInternal->AddTail(pData);
}

template <class T> inline CExoLinkedListNode* CExoLinkedList<T>::AddAfter(T* pData, CExoLinkedListNode* pPosition)
{
    return m_pInternal->AddAfter(pData, pPosition);
}

template <class T> inline CExoLinkedListNode* CExoLinkedList<T>::AddBefore(T* pData, CExoLinkedListNode* pPosition)
{
    return m_pInternal->AddBefore(pData, pPosition);
}
// PC 0x00622940
template <class T> inline int CExoLinkedList<T>::Contains(T* pData)
{
    return m_pInternal->Contains(pData);
}
// PC 0x00622950
template <class T> inline T* CExoLinkedList<T>::GetHead()
{
    return (T*)m_pInternal->GetHead();
}
// PC 0x00622960
template <class T> inline CExoLinkedListNode* CExoLinkedList<T>::GetHeadPos()
{
    return m_pInternal->m_pHead;
}

template <class T> inline T* CExoLinkedList<T>::GetTail()
{
    return (T*)m_pInternal->GetTail();
}

template <class T> inline CExoLinkedListNode* CExoLinkedList<T>::GetTailPos()
{
    return m_pInternal->m_pTail;
}
// PC 0x00622970
template <class T> inline T* CExoLinkedList<T>::Remove(CExoLinkedListNode* pPosition)
{
    return (T*)m_pInternal->Remove(pPosition);
}

template <class T> inline T* CExoLinkedList<T>::RemoveHead()
{
    return (T*)m_pInternal->RemoveHead();
}

template <class T> inline T* CExoLinkedList<T>::RemoveTail()
{
    return (T*)m_pInternal->RemoveTail();
}
// PC 0x00622980
template <class T> inline unsigned long CExoLinkedList<T>::Count()
{
    return m_pInternal->m_nCount;
}

template <class T> inline int CExoLinkedList<T>::IsEmpty()
{
    return m_pInternal->m_pHead == NULL;
}
// PC 0x00622990
template <class T> inline T* CExoLinkedList<T>::GetAtPos(CExoLinkedListNode* pPosition)
{
    return (T*)m_pInternal->GetAtPos(pPosition);
}
// PC 0x006229A0
template <class T> inline T* CExoLinkedList<T>::GetNext(CExoLinkedListNode*& pPosition)
{
    return (T*)m_pInternal->GetNext(pPosition);
}

template <class T> inline T* CExoLinkedList<T>::GetPrev(CExoLinkedListNode*& pPosition)
{
    return (T*)m_pInternal->GetPrev(pPosition);
}





class CExoCriticalSectionInternal;

class CExoCriticalSection
{
public:
    CExoCriticalSection();
    ~CExoCriticalSection();
    void EnterCriticalSection();
    void LeaveCriticalSection();

    CExoCriticalSectionInternal* m_pInternal;
};





class CExoDebugInternal;

class CExoDebug
{
public:
    CExoDebug();
    ~CExoDebug();

    void SetEnabled(int bEnabled) { m_bEnabled = bEnabled; }
    void FlushScriptLogFile();
    void OpenLogFiles(const CExoString& path);
    void Warning(int line, char* file, const char* message);
    void GetCurrentTimestamp(CExoString& timestamp);
    void WriteToScriptLogFile(const CExoString& text);







    void Assert(int line, char* file, const char* message);
    unsigned long GetCurrentMemoryAllocations();
    unsigned long GetMaxAllocatedMemory();
    unsigned long GetTotalMemoryAllocations();
    void SetRotateLogFile(int bRotate);
    void WriteToErrorFile(const CExoString& text);
    void WriteToLogFile(const CExoString& text);
#ifdef _DEBUG

    void Log(const char* format, ...);
#endif





    void NoLog(...) {}

    CExoString m_sLogDirectory;
    CExoDebugInternal* m_pInternal;
    int m_bEnabled;
};

void* EXOMALLOC(unsigned long nSize, const char* szFile, int nLine);
void EXOFREE(void* pMemory, const char* szFile, int nLine);





class CExoFileInternal;
class CExoString;

class CExoFile
{
public:
    CExoFile(const CExoString& path, unsigned short resourceType,
             const CExoString& mode);
    ~CExoFile();
    int FileOpened();
    int Flush();
    unsigned long GetOffset();
    int GetSize();
    unsigned long Read(void* data, unsigned long size, unsigned long count);
    unsigned long Read(CExoString& text, unsigned long count);
    void ReadAsync(void* data, unsigned long size, unsigned long count);
    int ReadAsyncComplete();
#ifdef _XBOX
    unsigned long ReadAsyncBytesRead();
#endif
    unsigned long Write(const void* data, unsigned long size,
                        unsigned long count);
    unsigned long Write(const char* text);
    unsigned long Write(const CExoString& text);
    int Seek(int offset, int origin);
    int SeekBeginning();
    int SeekEnd();

    CExoFileInternal* m_pInternal;
};





class CExoLocStringInternal;

class CExoLocString
{
public:
    CExoLocString();
    CExoLocString(const CExoLocString& lsString);
    ~CExoLocString();

    CExoLocString& operator=(const CExoLocString& lsString);
    int operator==(const CExoLocString& lsString) const;
    int operator!=(const CExoLocString& lsString) const;
    int Compare(const CExoLocString& lsString) const;
    void ClearLocString();
    int GetString(unsigned long nIndex, int* pnID, CExoString* psString,
                  unsigned char* pnGender) const;
    int GetString(int nID, CExoString* psString, unsigned char nGender) const;
    int GetString(int nID, CExoString* psString, unsigned char nGender);
    int GetStringCount() const;
    int GetStringLength(unsigned long nIndex) const;
    int GetStringLoc(int nID, CExoString* psString,
                     unsigned char nGender) const;
    float GetSpokenTimeEstimate(int nID, unsigned char nGender) const;
    int AddString(int nID, const CExoString& sString, unsigned char nGender);
    void RemoveString(int nID, unsigned char nGender);

private:

    int GetStringInternal(int nStringID, CExoString* psString) const;

public:
    CExoLocStringInternal* m_pExoLocStringInternal;
    unsigned long m_dwStringRef;

    static const float TIME_PER_CHARACTER_SPOKEN;
};





class CExoString;
class CExoIniInternal;

class CExoIni
{
public:
    CExoIni();
    ~CExoIni();
    int ReadIniEntry(CExoString&, const CExoString&, const CExoString&,
                     const CExoString&);
    int WriteIniEntry(const CExoString&, const CExoString&, const CExoString&,
                      const CExoString&);

    CExoIniInternal* m_pInternal;
};





class CExoRandInternal;

class CExoRand
{
public:
    CExoRand();
    ~CExoRand();
    unsigned long Rand();

    CExoRandInternal* m_pInternal;
};





class CExoTimersInternal;

class CExoTimers
{
public:
    CExoTimers();
    ~CExoTimers();
    unsigned long GetLowResolutionTimer();
    unsigned __int64 GetHighResolutionTimer();

    CExoTimersInternal* m_pInternal;
};





class CExoIni;
class CExoTimers;
class CExoDebug;
class CExoRand;
class CExoBaseInternal;
class CExoString;

class CExoAliasListInternal;

class CExoAliasList
{
public:
    CExoAliasList();
    ~CExoAliasList();
    void Add(const CExoString& sAlias, const CExoString& sPath);
#ifdef _XBOX

    void ChangePath(const CExoString& sAlias, const CExoString& sPath);
#endif
    const CExoString& GetAliasPath(const CExoString& sAlias, int nLength) const;
    CExoString ResolveFileName(const CExoString& sAlias,
                               unsigned short nResType) const;

    CExoAliasListInternal* m_pInternal;
};



#ifndef EXOARRAYLIST_INITIAL_ALLOCATION
#define EXOARRAYLIST_INITIAL_ALLOCATION 16
#endif



inline void ExoArrayListReleased(void*)
{
}

template <class T> class CExoArrayList
{
public:


    CExoArrayList(int nSize = 0);
    ~CExoArrayList();
    void SetSize(int nSize);
    CExoArrayList(const CExoArrayList<T>& list);
    CExoArrayList<T>& operator=(const CExoArrayList<T>& list);
    T& operator[](int nIndex)
    {
        return element[nIndex];
    }

    const T& operator[](int nIndex) const
    {
        return element[nIndex];
    }

    void Add(T element);
    void AddUnique(T element);
    int IndexOf(T element) const;
    int IndexOf(void* pValue, int (*pCompare)(void*, T)) const;
    void Insert(T element, int nIndex);
    void DelIndex(int nIndex);

    int Contains(T value) const;
    int DerefContains(T value) const;

    void Remove(T value)
    {
        int nIndex;
        for (nIndex = 0; nIndex < num; nIndex++)
        {
            if ((*this)[nIndex] == value)
                break;
        }
        if (nIndex == num)
            return;
        num--;
        for (; nIndex < num; nIndex++)
        {
            element[nIndex] = element[nIndex + 1];
        }
    }

    void Allocate(int nSize)
    {
        T* pOld = element;

        array_size = nSize;
        element = new T[nSize];

        for (int i = 0; i < num; i++)
        {
            element[i] = pOld[i];
        }

        if (pOld != NULL)
        {
            delete[] pOld;
        }
    }

    void RemoveAll(int bReleaseStorage = 1);


    void Pack()
    {
        Allocate(num);
    }

    T* element;
    int num;
    int array_size;
};

// PC 0x00405E10
template <class T> void CExoArrayList<T>::RemoveAll(int bReleaseStorage)
{
    num = 0;
    if (bReleaseStorage)
    {
        ExoArrayListReleased(this);
        SetSize(0);
    }
}

template <class T> CExoArrayList<T>::CExoArrayList(int nSize)
{
    num = 0;
    array_size = 0;
    element = NULL;
    if (nSize != 0)
    {
        Allocate(nSize);
    }
}

template <class T> CExoArrayList<T>::CExoArrayList(const CExoArrayList<T>& list)
{
    num = 0;
    array_size = 0;
    element = NULL;
    for (int i = 0; i < list.num; i++)
    {
        Add(list[i]);
    }
}

template <class T> CExoArrayList<T>::~CExoArrayList()
{
    delete[] element;
    element = NULL;
}

template <class T> CExoArrayList<T>& CExoArrayList<T>::operator=(const CExoArrayList<T>& list)
{
    num = 0;
    for (int i = 0; i < list.num; i++)
    {
        Add(list[i]);
    }
    return *this;
}

template <class T> void CExoArrayList<T>::Add(T value)
{
    if (num == array_size)
    {
        Allocate(array_size != 0 ? array_size * 2 : EXOARRAYLIST_INITIAL_ALLOCATION);
    }
    element[num++] = value;
}

template <class T> void CExoArrayList<T>::AddUnique(T value)
{
    if (!Contains(value))
    {
        Add(value);
    }
}

template <class T> int CExoArrayList<T>::IndexOf(T value) const
{
    for (int i = 0; i < num; i++)
    {
        if (element[i] == value)
        {
            return i;
        }
    }
    return -1;
}
// PC 0x00640890
template <class T> int CExoArrayList<T>::IndexOf(void* pValue, int (*pCompare)(void*, T)) const
{
    if (pCompare == NULL)
        return -1;

    for (int i = 0; i < num; i++)
    {
        if (pCompare(pValue, element[i]))
            return i;
    }

    return -1;
}

template <class T> void CExoArrayList<T>::Insert(T value, int nIndex)
{
    int nCount = num;
    Add(value);
    for (; nCount > nIndex; nCount--)
    {
        element[nCount] = element[nCount - 1];
    }
    element[nIndex] = value;
}

template <class T> void CExoArrayList<T>::DelIndex(int nIndex)
{
    num--;
    for (; nIndex < num; nIndex++)
    {
        element[nIndex] = element[nIndex + 1];
    }
}

template <class T> int CExoArrayList<T>::Contains(T value) const
{
    int nCount = 0;
    for (int i = 0; i < num; i++)
    {
        if (element[i] == value)
            nCount++;
    }
    return nCount;
}

// PC 0x00506110
template <class T> int CExoArrayList<T>::DerefContains(T value) const
{
    for (int i = 0; i < num; i++)
    {
        if (*element[i] == *value)
            return 1;
    }
    return 0;
}

template <class T> void CExoArrayList<T>::SetSize(int nSize)
{
    if (nSize == 0)
    {
        if (element != NULL)
        {
            delete[] element;
            element = NULL;
            array_size = 0;
        }
    }
    else
    {
        Allocate(nSize);
    }

    num = nSize;
}





template <class T> class CExoStaticArrayList : public CExoArrayList<T>
{
public:

    CExoStaticArrayList(int bStatic = 0)
    {
        m_bStatic = bStatic;
    }

    int m_bStatic;



    void Add(T value)
    {
        if (m_bStatic == 0)
        {
            CExoArrayList<T>::Add(value);
        }
    }
};

class CExoBase
{
public:
    CExoBase();
    ~CExoBase();
#ifdef _XBOX
    int GetDirectoryList(CExoArrayList<CExoString>* pList,
                         const CExoString& sPath, unsigned short nResType,
                         int nArg4, int nArg5);
#else
    void GetDirectoryList(CExoArrayList<CExoString>* pList,
                          const CExoString& sPath, unsigned short nResType,
                          int nArg4, int nArg5);
#endif
    int GetDirectorySize(const CExoString& sPath);
    const CExoString& GetResourceExtension(unsigned short resourceType);
    unsigned short GetResTypeFromExtension(const CExoString& extension);
    void LoadAliases(const CExoString& sFile);
    void SpawnExternalApplication(const CExoString& sApplication,
                                  const CExoString& sCommandLine,
                                  int bQuit);
    int WcsToMbs(char* sDest, const wchar_t* sSource, unsigned long nLength);

    CExoIni* m_pExoIni;
    CExoTimers* m_pExoTimers;
    CExoDebug* m_pExoDebug;
    CExoAliasList* m_pExoAliasList;
    CExoRand* m_pExoRand;
    CExoBaseInternal* m_pInternal;
};

extern CExoBase* g_pExoBase;



#ifdef _DEBUG
#define EXOLOG g_pExoBase->m_pExoDebug->Log
#else
#define EXOLOG g_pExoBase->m_pExoDebug->NoLog
#endif




class CFileInfo;

class CExoBaseInternal
{
public:
    CExoBaseInternal();
    ~CExoBaseInternal();
    int GetDirectoryList(CExoArrayList<CExoString>* pList,
                         const CExoString& sDir, unsigned short nResType,
                         int bDirectories, int nSortMode);
    void AddAlias(const CExoString& sFile, const CExoString& sAlias,
                  const CExoString& sDefault, const CExoString& sUnused);
    int GetDirectorySize(const CExoString& sPath);
    const CExoString& GetResourceExtension(unsigned short resourceType);
    unsigned short GetResTypeFromExtension(const CExoString& extension);
    void LoadAliases(const CExoString& sFile);
    void SpawnExternalApplication(const CExoString& sApplication,
                                  const CExoString& sCommandLine,
                                  int bQuit);
    int WcsToMbs(char* sDest, const wchar_t* sSource, unsigned long nLength);
    int CheckForCD();
    int GetAugmentedDirectoryList(CExoArrayList<CFileInfo>* pList,
                                  const CExoString& sDir,
                                  unsigned short nResType, int bSorted);

    int m_nResourceTypeCount;
    unsigned short* m_pResourceTypes;
    CExoString* m_pResourceExtensions;

private:
    void CreateResourceExtensionTable();
};


class CExoCriticalSectionInternal
{
public:
    CExoCriticalSectionInternal();
    ~CExoCriticalSectionInternal();
    void EnterCriticalSection();
    void LeaveCriticalSection();

    CRITICAL_SECTION m_CriticalSection;
};

class CExoFile;
class CExoString;

class CExoDebugInternal
{
public:
    CExoDebugInternal();
    ~CExoDebugInternal();
    void CloseLogFiles();
    void FlushScriptLogFile();
    void OpenLogFiles(const CExoString& path);
    void Assert(int line, char* file, const char* message);
    void InitializeWarningFile();
    void Warning(int line, char* file, const char* message);
    void GetCurrentTimestamp(CExoString& timestamp);
    void WriteToScriptLogFile(const CExoString& text);

    int m_bLogFilesOpen;
    CExoFile* m_pLogFile;
    CExoFile* m_pScriptLogFile;
    CExoFile* m_pAuxiliaryLogFileOne;
    CExoFile* m_pAuxiliaryLogFileTwo;
    CExoFile* m_pAuxiliaryLogFileThree;
    CExoFile* m_pAuxiliaryLogFileFour;
    CExoFile* m_pAuxiliaryLogFileFive;
};

class CExoString;

class CExoRandInternal
{
public:
    CExoRandInternal();

    ~CExoRandInternal();
    unsigned long Rand();
    void Add(unsigned char* pData, int nLength);
    void GenSeed();
    CExoString GetString(unsigned short nLength);
    void ReSeed(unsigned long nSeed0, unsigned long nSeed1,
                unsigned long nSeed2, unsigned long nSeed3);

    unsigned long m_anState[4];
    unsigned long m_anMask[4];
    unsigned long m_anRightShift[4];
    unsigned long m_anWidth[4];
    unsigned long m_anLeftShift[4];
    unsigned long m_anFeedbackShift[4];
    unsigned long m_nCurrentSeed;
    unsigned char* m_pBuffer;
    unsigned short m_nBufferLength;

#ifdef _XBOX



    union
    {
        struct
        {
            unsigned long nLow;
            long nHigh;
        };
        __int64 nCount;
    } m_aClockSamples[4];
#else
    struct
    {
        unsigned long nLow;
        long nHigh;
    } m_aClockSamples[4];
#endif

    unsigned long m_nTickCount;
};

class CExoTimersInternal
{
public:
    CExoTimersInternal();
    ~CExoTimersInternal();
    unsigned long GetLowResolutionTimer();
    unsigned __int64 GetHighResolutionTimer();

    int m_bHighResolution;
    int m_bLowFrequency;
    __int64 m_nFrequency;
    __int64 m_nFrequencyMHz;
    double m_dMicrosecondsPerTick;
};




class CExoMoviePlayerInternal;

class CExoMoviePlayer
{
public:
    CExoMoviePlayer();
    ~CExoMoviePlayer();

#ifdef _XBOX
    void CancelMovie();
    int IsMoviePlaying();
    int PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies, int nA);
    void ShowFadePanel();
    void HideFadePanel(int bWait);
    void SetFadeMsg(unsigned long nStrRef);
    int Initialize(void* pParam);
#else
    int CancelMovie(int nA, int nB);
    int IsMoviePlaying();
    int PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies, int nA,
                        const CExoArrayList<int>* lstSkippable);
    void ShowFadePanel();
    void HideFadePanel();
    int GetNumberOfMoviesToPlay();
    int RestartMoviesAsync();
    int Initialize(void* pParam);
#endif

    CExoMoviePlayerInternal* m_pInternal;
};



class CExoSoundSourceInternal;
class CExoSoundInternal;
class CResRef;

class Vector;


class CExoSoundSource
{
public:
    CExoSoundSource(const CResRef& cResRef);
    CExoSoundSource();
    virtual ~CExoSoundSource();

    int IsHardwarePlayingSoundPleaseDontUseThis();
    void SetLooping(int bLooping);
    void SetVolume(unsigned char nVolume);
    void Set3D(int b3D);
    void SetPriorityGroup(unsigned char nGroup);
    void SetPosition(const Vector& vPosition, float fRadius);
    int Play();
    void Stop();
    int SetFixedVariance(float fVariance);
    void SetPitchVariance(float fVariance);
    CExoSoundSourceInternal* SetDistance(float fMinDistance,
                                         float fMaxDistance);
    void SetResRef(const CResRef& cResRef, int bRequest);
    int GetLooping();

    CExoSoundSourceInternal* m_pInternal;
};



#define EXOSOUND_PRIORITY_SOUND_OBJECT_2D 21
#define EXOSOUND_PRIORITY_SOUND_OBJECT_3D 22
#define EXOSOUND_PRIORITY_STREAMING_SOUND_OBJECT_2D 4
#define EXOSOUND_PRIORITY_STREAMING_SOUND_OBJECT_3D 5

class CExoSound
{
public:

    enum EAXMode { EAX_MODE_DISABLED = 0, EAX_MODE_DEFAULT = 3 };
    static const unsigned char DEFAULT_2D_VOICES = 24;
    static const unsigned char DEFAULT_3D_VOICES = 16;



    enum SoundType { SOUND_TYPE_GUI = 11 };



    enum PriorityGroup
    {

        PRIORITY_GROUP_GUI = 11,

        PRIORITY_GROUP_CHAT_SOUND = 10,

        PRIORITY_GROUP_VISUAL_EFFECT = 13,

        PRIORITY_GROUP_PROJECTILE = 14,

        PRIORITY_GROUP_WEAPON_SWING = 15,
        PRIORITY_GROUP_LARGE_CREATURE_FOOTSTEP = 18,
        PRIORITY_GROUP_PLAYER_FOOTSTEP = 19,
        PRIORITY_GROUP_CREATURE_FOOTSTEP = 20,

        PRIORITY_GROUP_POWERED_ITEM = 21,

        PRIORITY_GROUP_DOOR = 22,
        PRIORITY_GROUP_ACTION_ANIMATION = 22,

        PRIORITY_GROUP_PLACEABLE = 22,


        PRIORITY_GROUP_TRIGGER = 22,

        PRIORITY_GROUP_DEFAULT = 23
    };
    CExoSound(unsigned char n2DVoices, unsigned char n3DVoices, int nEAXMode,
              int bA3D);
#ifdef _XBOX


    CExoSound(unsigned char n2DVoices, unsigned char n3DVoices, const CExoString& s3DProvider);

    void SetRollOffFactor(float fFactor);
    void SetSpeakerType(char nSpeakerType);
    void EnableEnvironmentEffects(int bEnabled);
    void SetEnvironmentEffectLevel(float fLevel);
#endif
    ~CExoSound();
    int Play3DOneShotSound(const CResRef& cResRef, Vector vPosition,
                           float fRadius, unsigned char nGroup,
                           unsigned long nA, unsigned char nB, float fA,
                           float fB);
    void PlaySound(CResRef& cResRef, unsigned char nType, unsigned long nA,
                   unsigned char nB, float fA, float fB);
    int GetBestEAXAvailable();
    void SetMusicVolume(float fVolume, int bFade);
    void SetDialogVolume(float fVolume);


    float GetMusicVolume();
    float GetSoundEffectVolume();
    float GetDialogVolume();
    void SetSoundEffectVolume(float fVolume);
    void ReInitialize(int nEAXMode, int bA3D);
    int GetEAX();
    int GetSoundMode();
    void SetSoundMode(int nMode, int bMovie = 0);
    void ResumeAllStreamingMusicForMovie();
    void Render();
    void SetListenerOrientation(const Vector& vOrientation, const Vector& vUp);
    void SetListenerPosition(const Vector& vPosition);
    int PlayOneShotSound(const CResRef& cResRef, unsigned char nType,
                         unsigned long nDelay, unsigned char nGroup,
                         float fVolume, float fPitchVariance);
    void StopAllOneShots();
    void Set2D3DBias(float fBias);
    float Get2D3DBias();
    unsigned char GetNumber2DVoices();
    unsigned char GetNumber3DVoices();
    void GetListenerPosition(Vector* pPosition);
    CExoSoundInternal* EnableStreamingEnvironmentEffects(int bEnabled);
    CExoSoundInternal* EnableNonStreamingEnvironmentEffects(int bEnabled);
    CExoSoundInternal* SetDesiredEnvironment(int nEnvironment);
    CExoSoundInternal* SetDesiredAmbientScale(float fScale);
    void GetPriorityGroupDistance(int nPriorityGroup, float* pfMaxDistance,
                                  float* pfMinDistance);
    int GetLostFocusPaused();

    CExoSoundInternal* m_pInternal;

    static const int CEXOSOUND_MODE_LOSTFOCUS;
    static const int CEXOSOUND_MODE_GAINFOCUS;
    static const int CEXOSOUND_MODE_GENERIC;
    static const int CEXOSOUND_MODE_NORMAL;
    static const int CEXOSOUND_MODE_PAUSED;
    static const int CEXOSOUND_MODE_FADEOUT;
    static const int CEXOSOUND_MODE_FADEIN;
    static const int CEXOSOUND_MODE_MUTE;
    static const int CEXOSOUND_MODE_PAUSE;
    static const int CEXOSOUND_MODE_PAUSE_ALL;
    static const int CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC;
    static const unsigned char CMAX_VOLUME;
};

extern CExoSound* g_pExoSound;


class CExoStreamingSoundSourceInternal;
class Vector;

class CExoStreamingSoundSource
{
public:


    enum PriorityGroup
    {
        PRIORITY_GROUP_STINGER = 1,

        PRIORITY_GROUP_MUSIC = 2,

        PRIORITY_GROUP_PLAYER_VOICE = 8,
        PRIORITY_GROUP_PARTY_VOICE = 9,
        PRIORITY_GROUP_CREATURE_VOICE = 17,
        PRIORITY_GROUP_LARGE_CREATURE_VOICE = 16,


        PRIORITY_GROUP_DIALOG_AMBIENT = 4,


        PRIORITY_GROUP_DIALOG_VOICE = 9
    };

    CExoStreamingSoundSource(int bReservedStream = 0);
    ~CExoStreamingSoundSource();

    int IsPlaying();
    int IsDone();
    int GetHasStartedStreaming();
    int GetPlayFailed();
    int GetLooping();
    int Play(const CResRef& cResRef, unsigned short nResourceType, int bLooping,
             int b3D, int bReverb);
    void Pause();
    void Resume();
    void SetLooping(int bLooping);
    void SetVolume(unsigned char nVolume);
    unsigned char GetVolume();
    void Stop(int bCloseStream = 0);
    void FadeAndStop(unsigned long nFadeTime);
    void SetStreamingMusicForMovie(int bStreamingMusicForMovie);
    void SetPriorityGroup(unsigned char nGroup);
    unsigned char GetPriorityGroup();
    void SetReverbEnabled(int bEnabled);
    CResRef GetFilePlaying();
#ifdef _XBOX


    void Set3D(int b3D);
    void SetDistance(float fMinDistance, float fMaxDistance);
    void SetPitchVariance(float fVariance);
    void SetFixedVariance(float fVariance);
#else
    CExoStreamingSoundSourceInternal* Set3D(int b3D);
#endif
    int Get3D();
#ifndef _XBOX
    CExoStreamingSoundSourceInternal* SetDistance(float fMinDistance,
                                                  float fMaxDistance);
#endif
    void SetPosition(const Vector& vPosition);

    CExoStreamingSoundSourceInternal* m_pInternal;
};

void GetGameVersionInfo();
extern bool g_bExtendedResourceTracking;
extern HWND g_hMovieWnd;

#endif
