#ifndef SwutilH
#define SwutilH

#include <System.hpp>
#include <Sysutils.hpp>
#include <Classes.hpp>
#include <stdio.h>

HANDLE AcquireInstanceMutex();

bool GetGameInstallDir(System::AnsiString& Directory);

bool SetGameInstallPath(const System::AnsiString Directory);

bool IsGameInstalled();

bool GetFileVersionText(const System::AnsiString FileName,
                        System::AnsiString& Version, int Mode);

class CTalkTable
{
private:
    FILE* Handle;
    unsigned long FileType;
    unsigned long Version;
    unsigned long LanguageCount;
    unsigned long StringCount;
    unsigned long StringEntryOffset;
    void* OldStringEntries;
    void* NewStringEntries;
    unsigned long FileSize;

    void Close();

public:
    CTalkTable();
    ~CTalkTable();

    bool Open(const char* FileName);
    System::AnsiString GetString(int StrRef);
};

#endif
