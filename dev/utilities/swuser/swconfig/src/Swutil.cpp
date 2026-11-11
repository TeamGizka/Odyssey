#include <vcl.h>
#pragma hdrstop

#include <Registry.hpp>
#include <Filectrl.hpp>
#include <stdio.h>
#include <string.h>

#include "Swutil.h"

struct TTlkEntryOld
{
    unsigned long Flags;
    char SoundResRef[16];
    unsigned long VolumeVariance;
    unsigned long PitchVariance;
    unsigned long OffsetToString;
    unsigned long StringSize;
};

struct TTlkEntryNew
{
    unsigned long Flags;
    char SoundResRef[16];
    unsigned long VolumeVariance;
    unsigned long PitchVariance;
    unsigned long OffsetToString;
    unsigned long StringSize;
    float SoundLength;
};

CTalkTable::CTalkTable()
{
    Handle = NULL;
    memset(&FileType, 0, 20);
    OldStringEntries = NULL;
    NewStringEntries = NULL;
    FileSize = 0;
}

CTalkTable::~CTalkTable()
{
    Close();
}

bool CTalkTable::Open(const char* FileName)
{
    Close();

    FILE* f = fopen(FileName, "rb");
    if (f == NULL)
        return false;

    fseek(f, 0, SEEK_END);
    FileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char header[20];
    if (fread(header, 20, 1, f) != 1 || memcmp(header, "TLK ", 4) != 0)
    {
        fclose(f);
        return false;
    }

    unsigned long count = *(unsigned long*)(header + 12);
    if (memcmp(header + 4, "V3.0", 4) == 0)
    {
        NewStringEntries = new unsigned char[40 * count];
        if (fread(NewStringEntries, 40, count, f) != count)
        {
            delete[] NewStringEntries;
            NewStringEntries = NULL;
            fclose(f);
            return false;
        }
    }
    else
    {
        OldStringEntries = new unsigned char[36 * count];
        if (fread(OldStringEntries, 36, count, f) != count)
        {
            delete[] OldStringEntries;
            OldStringEntries = NULL;
            fclose(f);
            return false;
        }
    }

    Handle = f;
    memcpy(&FileType, header, 20);
    return true;
}

void CTalkTable::Close()
{
    if (Handle != NULL)
    {
        fclose(Handle);
        Handle = NULL;
    }
    if (OldStringEntries != NULL)
    {
        delete[] OldStringEntries;
        OldStringEntries = NULL;
    }
    if (NewStringEntries != NULL)
    {
        delete[] NewStringEntries;
        NewStringEntries = NULL;
    }
    memset(&FileType, 0, 20);
}

System::AnsiString CTalkTable::GetString(int StrRef)
{
    if (Handle == NULL)
        return "";

    if (StrRef < 0 || (unsigned long)StrRef >= StringCount)
        return "";

    unsigned long offset;
    unsigned long size;
    if (NewStringEntries != NULL)
    {
        offset = ((TTlkEntryNew*)NewStringEntries)[StrRef].OffsetToString;
        size = ((TTlkEntryNew*)NewStringEntries)[StrRef].StringSize;
    }
    else
    {
        offset = ((TTlkEntryOld*)OldStringEntries)[StrRef].OffsetToString;
        size = ((TTlkEntryOld*)OldStringEntries)[StrRef].StringSize;
    }

    unsigned long pos = StringEntryOffset + offset;
    if ((long)(size + pos) > (long)FileSize)
        return "";

    if (fseek(Handle, pos, SEEK_SET) != 0)
        return "";

    char* buffer = new char[size];
    AnsiString s;
    if (fread(buffer, size, 1, Handle) == 1)
        s = AnsiString(buffer, size);
    delete[] buffer;
    return s;
}

bool GetFileVersionText(const System::AnsiString FileName,
                        System::AnsiString& Version, int Mode)
{
    AnsiString privateBuild = "";
    const char* file = FileName.c_str();
    if (file == NULL)
        file = "";

    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeA((char*)file, &handle);
    if (size == 0)
        return false;

    void* data = malloc(size);
    if (data == NULL)
        return false;

    if (!GetFileVersionInfoA((char*)file, 0, size, data))
    {
        free(data);
        return false;
    }

    VS_FIXEDFILEINFO* fi = NULL;
    UINT len = 0;
    if (!VerQueryValueA(data, "\\", (void**)&fi, &len))
    {
        free(data);
        return false;
    }

    if (Mode != 0 && (fi->dwFileFlags & VS_FF_PRIVATEBUILD))
    {
        LANGID lang = GetUserDefaultLangID();
        DWORD* translations = NULL;
        UINT count = 0;
        if (VerQueryValueA(data, "\\VarFileInfo\\Translation",
                           (void**)&translations, &count))
        {
            DWORD match = (DWORD)-1;
            for (UINT i = 0; i < count / 4; i++)
            {
                if (lang == LOWORD(translations[i]))
                {
                    match = translations[i];
                    break;
                }
            }
            if (match == (DWORD)-1 && count != 0)
                match = translations[0];

            char subBlock[260];
            wsprintfA(subBlock, "\\StringFileInfo\\%04X%04X\\PrivateBuild",
                      LOWORD(match), HIWORD(match));
            char* text = NULL;
            UINT textLen = 0;
            if (VerQueryValueA(data, subBlock, (void**)&text, &textLen))
                privateBuild = text;
        }
    }

    Version.sprintf("v%d.%d%d%s",
                    HIWORD(fi->dwFileVersionMS),
                    HIBYTE(LOWORD(fi->dwFileVersionMS)),
                    LOBYTE(LOWORD(fi->dwFileVersionMS)),
                    privateBuild.c_str());
    free(data);
    return true;
}

bool IsValidGameDirectory(const AnsiString Directory);

bool IsGameInstalled()
{
    AnsiString dir = "";
    if (GetGameInstallDir(dir))
    {
        if (IsValidGameDirectory(dir))
            return true;
        dir = ExtractFilePath(Application->ExeName);
        if (IsValidGameDirectory(dir))
            return SetGameInstallPath(dir);
        return false;
    }
    dir = ExtractFilePath(Application->ExeName);
    if (IsValidGameDirectory(dir))
        return SetGameInstallPath(dir);
    return false;
}

HANDLE AcquireInstanceMutex()
{
    SECURITY_ATTRIBUTES Attributes;
    memset(&Attributes, 0, sizeof(Attributes));
    Attributes.nLength = sizeof(Attributes);
    HANDLE Instance = CreateMutexA(&Attributes, false, "swkotor");
    if (Instance == NULL)
        return NULL;
    if (WaitForSingleObject(Instance, 0) == 0)
        return Instance;
    CloseHandle(Instance);
    return NULL;
}

bool GetGameInstallDir(System::AnsiString& Directory)
{
    bool result;
    TRegistry* r = new TRegistry;
    r->RootKey = HKEY_LOCAL_MACHINE;
    Directory = "";
    if (r->OpenKeyReadOnly("\\SOFTWARE\\BioWare\\SW\\KotoR"))
        Directory = r->ReadString("Path");
    result = (Directory != "");
    if (result)
    {
        int len = Directory.Length();
        Directory.Unique();
        if (Directory[len] != '\\')
            Directory += "\\";
    }
    delete r;
    return result;
}

bool IsValidGameDirectory(const AnsiString Directory)
{
    AnsiString dir = "";
    if (Directory != "")
        dir = Directory;
    else
        dir = ExtractFilePath(Application->ExeName);

    int len = dir.Length();
    dir.Unique();
    if (dir[len] != '\\')
        dir += "\\";

    AnsiString files = "chitin.key;dialog.tlk;swkotor.exe";
    while (files.Length() > 0)
    {
        AnsiString file;
        int semi = files.Pos(";");
        if (semi != 0)
        {
            file = files.SubString(1, semi - 1);
            files = files.SubString(semi + 1, files.Length());
        }
        else
        {
            file = files;
            files = "";
        }
        file = Trim(file);
        if (!FileExists(dir + file))
            return false;
    }

    AnsiString dirs = "data";
    while (dirs.Length() > 0)
    {
        AnsiString name;
        int semi = dirs.Pos(";");
        if (semi != 0)
        {
            name = dirs.SubString(1, semi - 1);
            dirs = dirs.SubString(semi + 1, dirs.Length());
        }
        else
        {
            name = dirs;
            dirs = "";
        }
        name = Trim(name);
        if (!Filectrl::DirectoryExists(dir + name))
            return false;
    }
    return true;
}

bool SetGameInstallPath(const System::AnsiString Directory)
{
    TRegistry* r = new TRegistry;
    r->RootKey = HKEY_LOCAL_MACHINE;
    r->OpenKey("\\SOFTWARE\\BioWare\\SW\\KotoR", true);

    AnsiString dir = "";
    if (Directory != "")
        dir = Directory;
    else
        dir = ExtractFilePath(Application->ExeName);

    int len = dir.Length();
    dir.Unique();
    if (dir[len] != '\\')
        dir += "\\";
    r->WriteString("Path", dir.SubString(1, dir.Length() - 1));

    delete r;
    return true;
}

