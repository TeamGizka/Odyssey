#include <vcl.h>
#pragma hdrstop

#include "Nwcsysinfo.h"
#include "gls.h"
#include "Getprocessorspeed.h"

#include <Registry.hpp>
#include <ddraw.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#pragma package(smart_init)

static void DiscardSystemErrorMessage(DWORD MessageId)
{
    AnsiString msg;
    char* buffer = NULL;
    DWORD n = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                 FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                             NULL, MessageId, 0, (char*)&buffer, 0, NULL);
    for (DWORD i = 0; i < n; i++)
        msg += AnsiString((char)buffer[i]);
    LocalFree(buffer);
}

static int GetPlatform()
{
    OSVERSIONINFOA* vi = new OSVERSIONINFOA;
    vi->dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(vi);
    int platform;
    if (vi->dwPlatformId == 1)
        platform = vi->dwMajorVersion;
    else if (vi->dwPlatformId == 2)
        platform = (vi->dwMajorVersion >= 5) ? 5 : 2;
    else
        platform = vi->dwPlatformId;
    delete vi;
    return platform;
}

static AnsiString JoinPath(const AnsiString Path, const AnsiString Name)
{
    AnsiString result = Path;
    if (Path.Length() != 0 && Name.Length() != 0 && Path[Path.Length()] != '\\')
        result = Path + "\\";
    if (Name.Length() != 0)
    {
        int start = 1;
        for (int i = 1; i < Name.Length(); i++)
        {
            if (Name[i] != '\\')
                break;
            start = i + 1;
        }
        result = result + Name.SubString(start, Name.Length());
    }
    int len = result.Length();
    while (len >= 1 && result[len] == '\\')
        len--;
    result = result.SubString(1, len);
    return result;
}

static __int64 GetFileSize64(const AnsiString FileName)
{
    __int64 result = 0;
    if (FileExists(FileName))
    {
        TFileStream* s =
            new TFileStream(FileName, fmOpenRead | fmShareDenyNone);
        result = s->Size;
        delete s;
    }
    return result;
}

static AnsiString ReadRegistryString(HKEY RootKey, const AnsiString Key,
                                     const AnsiString ValueName)
{
    AnsiString result = "";
    TRegistry* r = new TRegistry;
    TStringList* names = new TStringList;
    r->RootKey = RootKey;
    if (Key != "" && r->KeyExists(Key) && r->OpenKey(Key, false))
    {
        r->GetValueNames(names);
        for (int i = 0; i < names->Count; i++)
        {
            if (AnsiCompareText(names->Strings[i], ValueName) == 0)
            {
                result = r->ReadString(names->Strings[i]);
                break;
            }
        }
        r->CloseKey();
    }
    else
    {
        result = "Error -- [" + Key + "]";
    }
    delete r;
    return result;
}

static void ScanRegistryKey(const AnsiString Key, const AnsiString ValName1,
                            const AnsiString ValName2, const AnsiString MatchA,
                            const AnsiString NameA, const AnsiString MatchB,
                            const AnsiString NameB, TStrings* List)
{
    TRegistry* r = new TRegistry;
    AnsiString valA = "";
    AnsiString valB = "";
    r->RootKey = HKEY_LOCAL_MACHINE;
    if (r->OpenKey(Key, false))
    {
        if (NameA != "")
            valA = r->ReadString(NameA);
        if (NameB != "")
            valB = r->ReadString(NameB);

        bool aOK = true;
        if (NameA != "")
        {
            aOK = false;
            if (MatchA != "")
                aOK = stricmp(valA.c_str(), MatchA.c_str()) == 0;
        }
        bool bOK = true;
        if (NameB != "")
        {
            bOK = false;
            if (MatchB != "")
                bOK = stricmp(valB.c_str(), MatchB.c_str()) == 0;
        }

        if (aOK && bOK)
        {
            AnsiString s1 =
                (ValName1 != "") ? r->ReadString(ValName1) : AnsiString("");
            AnsiString s2 =
                (ValName2 != "") ? r->ReadString(ValName2) : AnsiString("");
            List->Add(s1 + " " + s2);
        }
        r->CloseKey();
    }
    delete r;
}

static int EnumRegistryDevices(const AnsiString Key, const AnsiString ValName1,
                               const AnsiString ValName2,
                               const AnsiString MatchA, const AnsiString NameA,
                               const AnsiString MatchB, const AnsiString NameB,
                               TStrings* List)
{
    int found = 0;
    TRegistry* r = new TRegistry;
    TStringList* l1 = new TStringList;
    TStringList* l2 = new TStringList;
    r->RootKey = HKEY_LOCAL_MACHINE;
    if (r->OpenKey(Key, false))
    {
        r->GetKeyNames(l1);
        r->CloseKey();
        for (int i = 0; i < l1->Count; i++)
        {
            r->OpenKey(JoinPath(Key, l1->Strings[i]), false);
            l2->Clear();
            r->GetKeyNames(l2);
            r->CloseKey();
            for (int j = 0; j < l2->Count; j++)
            {
                AnsiString subKey =
                    JoinPath(JoinPath(Key, l1->Strings[i]), l2->Strings[j]);
                if (r->OpenKey(subKey, false))
                {
                    AnsiString valA =
                        (NameA != "") ? r->ReadString(NameA) : AnsiString("");
                    AnsiString valB =
                        (NameB != "") ? r->ReadString(NameB) : AnsiString("");

                    bool aOK = true;
                    if (NameA != "")
                    {
                        aOK = false;
                        if (MatchA != "")
                            aOK = stricmp(valA.c_str(), MatchA.c_str()) == 0;
                    }
                    bool bOK = true;
                    if (NameB != "")
                    {
                        bOK = false;
                        if (MatchB != "")
                            bOK = stricmp(valB.c_str(), MatchB.c_str()) == 0;
                    }

                    if (aOK && bOK)
                    {
                        AnsiString s1 = (ValName1 != "")
                                            ? r->ReadString(ValName1)
                                            : AnsiString("");
                        AnsiString s2 = (ValName2 != "")
                                            ? r->ReadString(ValName2)
                                            : AnsiString("");
                        List->Add(s1 + " " + s2);
                        found++;
                    }
                    r->CloseKey();
                }
            }
        }
    }
    delete l2;
    delete l1;
    delete r;
    return found;
}

static bool RegisterInfoWindowClass()
{
    WNDCLASSEXA wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = 8;
    wc.lpfnWndProc = DefWindowProcA;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = MainInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursorA(NULL, (LPCSTR)0x7F00);
    wc.hbrBackground = (HBRUSH)GetStockObject(4);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "launchclass";
    wc.hIconSm = NULL;
    return RegisterClassExA(&wc) != 0;
}

static bool SetupPixelFormat(HDC dc, TStrings* Errors)
{
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 16;
    pfd.cDepthBits = 16;

    int fmt = pChoosePixelFormat(dc, &pfd);
    if (fmt == 0)
    {
        Errors->Add("Error: ChoosePixelFormat failed");
        return false;
    }
    if (!pSetPixelFormat(dc, fmt, &pfd))
    {
        Errors->Add("Error: SetPixelFormat failed");
        return false;
    }
    pDescribePixelFormat(dc, fmt, sizeof(pfd), &pfd);
    return true;
}

static int RoundUpToPowerOfTwoMB(unsigned long Bytes)
{
    double mb = (double)Bytes * 0.00000095367432;
    return (int)(__int64)ceil(exp(log(2.0) * ceil(log(mb) / log(2.0))));
}

static void MapDXVersion(const AnsiString Raw, AnsiString& Friendly)
{
    int a, b, c, d;
    AnsiString name;
    TVersion v(0, 0, 0, 0);
    if (sscanf(Raw.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) == 4)
    {
        v = TVersion(a, b, c, d);
        if (v >= TVersion(4, 9, 0, 903))
            name = "DirectX (9.0+)";
        else
        {
            if (v >= TVersion(4, 9, 0, 902))
                name = "DirectX 9.0b";
            else
            {
                if (v >= TVersion(4, 9, 0, 901))
                    name = "DirectX 9.0a";
                else
                {
                    if (v >= TVersion(4, 9, 0, 0))
                        name = "DirectX 9.0";
                    else
                    {
                        if (v < TVersion(4, 8, 2, 0) && v > TVersion(4, 8, 1, 0))
                            name = "DirectX 8.1";
                        else
                        {
                            if (v < TVersion(4, 8, 1, 0) && v > TVersion(4, 8, 0, 0))
                                name = "DirectX 8.0";
                            else
                            {
                                if (v >= TVersion(4, 7, 1, 3000))
                                    name = "DirectX 7.1";
                                else
                                {
                                    if (v >= TVersion(4, 7, 0, 716))
                                        name = "DirectX 7.0a";
                                    else
                                    {
                                        if (v >= TVersion(4, 7, 0, 700))
                                            name = "DirectX 7.0";
                                        else
                                        {
                                            if (v >= TVersion(4, 6, 3, 518))
                                                name = "DirectX 6.1a";
                                            else
                                            {
                                                if (v >= TVersion(4, 6, 2, 436))
                                                    name = "DirectX 6.1";
                                                else
                                                {
                                                    if (v >= TVersion(4, 6, 0, 318))
                                                        name = "DirectX 6.0";
                                                    else
                                                    {
                                                        if (v >= TVersion(4, 5, 1, 1998))
                                                            name = "DirectX 5.2";
                                                        else
                                                        {
                                                            if (v >= TVersion(4, 5, 0, 155))
                                                                name = "DirectX 5.0";
                                                            else
                                                            {
                                                                if (v >= TVersion(4, 4, 0, 68))
                                                                    name = "DirectX 3.0";
                                                                else
                                                                {
                                                                    if (v >= TVersion(4, 3, 0, 1096))
                                                                        name = "DirectX 2.0";
                                                                    else
                                                                        name = "DirectX 1.0";
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        Friendly = name + " (" + Raw + ")";
    }
    else
    {
        Friendly = "??? (" + Raw + ")";
    }
}

void __fastcall GetOSVersion(System::AnsiString& Version, System::AnsiString& Name)
{

    OSVERSIONINFOA* vi = new OSVERSIONINFOA;

    AnsiString longName = "";
    AnsiString shortName = "";

    vi->dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(vi);

    Version = "";

    switch (vi->dwPlatformId)
    {
    case 0:
        longName = "Windows 3.1x running Win32s.";
        shortName = "Win32s";
        break;

    case 1:
        if (vi->dwMajorVersion == 4)
        {
            if (vi->dwMinorVersion == 0)
            {
                longName = "Windows 95";
                shortName = "Win95";
            }
            else if (vi->dwMinorVersion == 10)
            {
                longName = "Windows 98";
                shortName = "Win98";
            }
            else if (vi->dwMinorVersion == 90)
            {
                longName = "Windows ME";
                shortName = "WinME";
            }
            else
            {
                longName = "Windows 9X";
                shortName = "Win9x";
            }
        }
        else
        {
            longName = "Windows 9X";
            shortName = "Win9x";
        }

        longName = longName + " v" + IntToStr(vi->dwMajorVersion) + "." +
                   IntToStr(vi->dwMinorVersion) + " build " +
                   IntToStr((unsigned short)vi->dwBuildNumber) + " " +
                   AnsiString(vi->szCSDVersion);
        break;

    case 2:
        switch (vi->dwMajorVersion)
        {
        case 3:
            longName = "Windows NT";
            shortName = "WinNT351";
            break;
        case 4:
            longName = "Windows NT";
            shortName = "WinNT4";
            break;
        case 5:
            if (vi->dwMinorVersion == 0)
            {
                longName = "Windows 2000";
                shortName = "WinNT2000";
            }
            else if (vi->dwMinorVersion == 1)
            {
                longName = "Windows XP";
                shortName = "WinXP";
            }
            else if (vi->dwMinorVersion == 2)
            {
                longName = "Windows Server 2003";
                shortName = "WinSrv2003";
            }
            else
            {
                longName = "Windows Server";
                shortName = "WinSrv";
            }
            break;
        default:
            longName = "Windows 9X";
            shortName = "Win9x";
            break;
        }
        longName = longName + " v" + IntToStr(vi->dwMajorVersion) + "." +
                   IntToStr(vi->dwMinorVersion) + " build " +
                   IntToStr(vi->dwBuildNumber) + " " +
                   AnsiString(vi->szCSDVersion);
        break;

    default:
        longName = "Unknown";
        shortName = "???";
        break;
    }

    Version = longName;
    Name = shortName;
    delete vi;
}

bool GetOSServicePack(System::AnsiString& ServicePack)
{
    AnsiString csd = "";
    TRegistry* r = new TRegistry;
    r->RootKey = HKEY_LOCAL_MACHINE;
    if (r->KeyExists("\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion") &&
        r->OpenKey("\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                   false) &&
        r->ValueExists("CSDVersion"))
    {
        csd = r->ReadString("CSDVersion");
        r->CloseKey();
    }

    if (csd != "")
    {
        ServicePack = csd;
        return true;
    }
    ServicePack = "None";
    return false;
}

static void ScanSwapFiles9x(Classes::TStrings* List)
{
    char buf[256];
    GetWindowsDirectoryA(buf, 256);
    AnsiString path = JoinPath(buf, "win386.swp");
    if (FileExists(path))
        List->Add(path + "=" + IntToStr(GetFileSize64(path)));

    for (int i = 1; i <= 25; i++)
    {
        AnsiString root = AnsiString(char('A' + i)) + ":\\";
        if (GetDriveTypeA(root.c_str()) == 3)
        {
            AnsiString p = JoinPath(root, "win386.swp");
            if (FileExists(p))
                List->Add(p + "=-1");
        }
        Application->ProcessMessages();
    }
}

static void ScanSwapFilesNT(Classes::TStrings* List)
{
    TRegistry* r = new TRegistry;
    r->RootKey = HKEY_LOCAL_MACHINE;
    if (r->OpenKey("SYSTEM\\CurrentControlSet\\Control\\Session "
                   "Manager\\Memory Management",
                   false))
    {
        if (r->ValueExists("PagingFiles"))
        {
            char data[256];
            memset(data, 0, sizeof(data));
            r->ReadBinaryData("PagingFiles", data, 256);

            AnsiString buffer = "";
            for (int i = 0; i < 256; i++)
            {
                if (data[i] != 0)
                {
                    buffer += AnsiString((char)data[i]);
                }
                else
                {
                    if (Trim(buffer) != "")
                    {
                        char path[256];
                        int initial = 0, maximum = 0;
                        sscanf(buffer.c_str(), "%s %d %d", path, &initial,
                               &maximum);
                        List->Add(AnsiString(path) + "=" + IntToStr(initial));
                    }
                    buffer = "";
                }
            }
        }
        r->CloseKey();
    }
    delete r;
}

void GetSwapFileList(Classes::TStrings* List)
{
    if (GetPlatform() == 4)
        ScanSwapFiles9x(List);
    else
        ScanSwapFilesNT(List);
}

System::AnsiString __fastcall GetFileVersionString(System::AnsiString FileName,
                                                   System::AnsiString  )
{
    AnsiString text = "";
    char* file = (char*)FileName.c_str();

    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeA(file, &handle);
    if (size != 0)
    {
        void* data = malloc(size);
        if (data != NULL)
        {
            if (GetFileVersionInfoA(file, 0, size, data))
            {
                void* vq = NULL;
                UINT len = 0;
                if (VerQueryValueA(data, "\\", &vq, &len))
                {
                    VS_FIXEDFILEINFO* fi = (VS_FIXEDFILEINFO*)vq;
                    text.sprintf("%d.%d.%d.%d", HIWORD(fi->dwFileVersionMS),
                                 LOWORD(fi->dwFileVersionMS),
                                 HIWORD(fi->dwFileVersionLS),
                                 LOWORD(fi->dwFileVersionLS));
                }
            }
            free(data);
        }
    }
    return text;
}

static unsigned long s_FeatureEDX;
static unsigned long s_ExtFeatureEDX;
static unsigned long s_Signature;
static unsigned long s_FeatureEBX;
static unsigned long s_Vendor[3];
static char s_Brand[48];
static unsigned long s_L1Info[4];
static unsigned long s_L2Info[4];
static int s_CPUIDInit;

static void DoCPUID(unsigned long Leaf, unsigned long* pOut)
{
    asm {
        push eax
        push ebx
        push ecx
        push edx
        push esi
        mov  eax, Leaf
        db   0Fh, 0A2h
        mov  esi, pOut
        mov  [esi], eax
        mov  [esi+4], ebx
        mov  [esi+8], ecx
        mov  [esi+12], edx
        pop  esi
        pop  edx
        pop  ecx
        pop  ebx
        pop  eax
    }
}

static unsigned int CollectCPUID()
{
    unsigned long r[4];

    DoCPUID(0, r);
    s_Vendor[0] = r[1];
    s_Vendor[1] = r[2];
    s_Vendor[2] = r[3];

    if (r[0] != 0)
    {
        DoCPUID(1, r);
        s_Signature = r[0];
        s_FeatureEDX = r[2];
        s_FeatureEBX = r[1];

        DoCPUID(0x80000000, r);
        unsigned long maxExt = r[0];
        if (maxExt >= 0x80000001)
        {
            DoCPUID(0x80000001, r);
            s_ExtFeatureEDX = r[3];
        }
        if (maxExt >= 0x80000004)
        {
            DoCPUID(0x80000002, r);
            memcpy(s_Brand + 0, r, 16);
            DoCPUID(0x80000003, r);
            memcpy(s_Brand + 16, r, 16);
            DoCPUID(0x80000004, r);
            memcpy(s_Brand + 32, r, 16);
        }
        if (maxExt >= 0x80000005)
        {
            DoCPUID(0x80000005, r);
            memcpy(s_L1Info, r, 16);
        }
        if (maxExt >= 0x80000006)
        {
            DoCPUID(0x80000006, r);
            memcpy(s_L2Info, r, 16);
        }
    }
    return 1;
}

unsigned int GetCPUInfo(unsigned int Index)
{
    unsigned int result = 0;

    if (s_CPUIDInit == 0)
        s_CPUIDInit = CollectCPUID();
    if (s_CPUIDInit == -1)
        return 0;

    unsigned long family = (s_Signature >> 8) & 0xF;
    unsigned long model = (unsigned char)s_Signature >> 4;

    switch (Index)
    {
    case 0:
        if (strncmp((char*)s_Vendor, "AuthenticAMD", 12) == 0)
            result = 1;
        else if (strncmp((char*)s_Vendor, "GenuineIntel", 12) == 0)
            result = 2;
        else if (strncmp((char*)s_Vendor, "CyrixInstead", 12) == 0)
            result = 3;
        else if (strncmp((char*)s_Vendor, "CentaurHauls", 12) == 0)
            result = 4;
        else
            result = 0;
        break;

    case 1:
        if (GetCPUInfo(0) == 1)
        {
            switch (family)
            {
            case 4:
                result = 1;
                break;
            case 5:
                if (model < 4)
                    result = 2;
                else if (model < 8)
                    result = 3;
                else if (model == 8)
                    result = 4;
                else if (model <= 15)
                    result = 5;
                break;
            case 6:
                switch (model)
                {
                case 0:
                case 1:
                case 2:
                case 4:
                case 5:
                case 6:
                    result = 6;
                    break;
                case 3:
                case 7:
                    result = 7;
                    break;
                case 8:
                    result = GetCPUInfo(25) ? 9 : 8;
                    break;
                default:
                    return result;
                }
                break;
            default:
                break;
            }
        }
        else if (GetCPUInfo(0) == 2)
        {
            switch (family)
            {
            case 4:
                switch (model)
                {
                case 0:
                case 1:
                    result = 10;
                    break;
                case 2:
                    result = 11;
                    break;
                case 3:
                    result = 12;
                    break;
                case 4:
                    result = 13;
                    break;
                case 5:
                    result = 14;
                    break;
                case 7:
                    result = 15;
                    break;
                default:
                    result = 16;
                    break;
                }
                break;
            case 5:
                if (model == 1 || model == 2 || model == 3)
                    result = 17;
                else
                    result = 18;
                break;
            case 6:
                switch (model)
                {
                case 0:
                case 1:
                    result = 19;
                    break;
                case 3:
                case 5:
                    result = 20;
                    break;
                case 6:
                    result = 21;
                    break;
                case 7:
                    result = 22;
                    break;
                case 8:

                    result = GetCPUInfo(25) ? 9 : 8;
                    break;
                case 10:
                case 11:
                    result = 24;
                    break;
                default:
                    return result;
                }
                break;
            case 7:
                result = 25;
                break;
            case 0xF:
                result = 27;
                break;
            default:
                return result;
            }
        }
        break;

    case 2:
        result = (unsigned int)(unsigned long)(char*)s_Vendor;
        break;

    case 3:
        result = (unsigned int)(unsigned long)(char*)s_Brand;
        break;

    case 4:
        result = family;
        break;

    case 5:
        result = model;
        break;

    case 6:
        result = s_Signature & 0xF;
        break;

    case 7:
        result = 1;
        break;

    case 8:
        result = (s_FeatureEDX & 0x00000001) != 0;
        break;
    case 9:
        result = (s_FeatureEDX & 0x00000002) != 0;
        break;
    case 10:
        result = (s_FeatureEDX & 0x00000004) != 0;
        break;
    case 11:
        result = (s_FeatureEDX & 0x00000008) != 0;
        break;
    case 12:
        result = (s_FeatureEDX & 0x00000010) != 0;
        break;
    case 13:
        result = (s_FeatureEDX & 0x00000020) != 0;
        break;
    case 14:
        result = (s_FeatureEDX & 0x00000040) != 0;
        break;
    case 15:
        result = (s_FeatureEDX & 0x00000080) != 0;
        break;
    case 16:
        result = (s_FeatureEDX & 0x00000100) != 0;
        break;
    case 17:
        result = (s_FeatureEDX & 0x00000200) != 0;
        break;
    case 18:
        result = (s_FeatureEDX & 0x00000800) != 0;
        break;
    case 19:
        result = (s_FeatureEDX & 0x00001000) != 0;
        break;
    case 20:
        result = (s_FeatureEDX & 0x00002000) != 0;
        break;
    case 21:
        result = (s_FeatureEDX & 0x00004000) != 0;
        break;
    case 22:
        result = (s_FeatureEDX & 0x00008000) != 0;
        break;
    case 23:
        result = (s_FeatureEDX & 0x00010000) != 0;
        break;
    case 24:
        result = (s_FeatureEDX & 0x00020000) != 0;
        break;
    case 25:
        result = (s_FeatureEDX & 0x00080000) != 0;
        break;

    case 26:
        result = ((s_ExtFeatureEDX & 0x00400000) != 0) ||
                 ((s_FeatureEDX & 0x02000000) != 0);
        break;
    case 27:
        result = (s_FeatureEDX & 0x00800000) != 0;
        break;
    case 28:
        result = (s_FeatureEDX & 0x01000000) != 0;
        break;
    case 29:
        result = (s_ExtFeatureEDX & 0x40000000) != 0;
        break;
    case 30:
        result = (unsigned int)s_ExtFeatureEDX >> 31;
        break;
    case 31:
        result = (s_ExtFeatureEDX & 0x00400000) != 0;
        break;
    case 32:
        result = (s_FeatureEDX & 0x02000000) != 0;
        break;
    case 33:
        result = GetCPUInfo(32) ? 1 : 0;
        break;

    default:
        if (GetCPUInfo(0) == 2)
        {
            if (Index == 34)
                result = (s_FeatureEDX & 0x10000000) != 0;
            else if (Index == 35)
                result = GetCPUInfo(34) ? ((s_FeatureEBX & 0xFF0000) >> 16) : 1;
        }
        else if (GetCPUInfo(0) == 1)
        {
            switch (Index)
            {
            case 36:
                result = (unsigned char)(s_L1Info[1] >> 24);
                break;
            case 37:
                result = (unsigned char)(s_L1Info[1] >> 16);
                break;
            case 38:
                result = (unsigned char)(s_L1Info[1] >> 8);
                break;
            case 39:
                result = (unsigned char)(s_L1Info[1]);
                break;
            case 44:
                result = (unsigned char)(s_L1Info[2] >> 24);
                break;
            case 45:
                result = (unsigned char)(s_L1Info[2] >> 16);
                break;
            case 46:
                result = (unsigned char)(s_L1Info[2] >> 8);
                break;
            case 47:
                result = (unsigned char)(s_L1Info[2]);
                break;
            case 48:
                result = (unsigned char)(s_L1Info[3] >> 24);
                break;
            case 49:
                result = (unsigned char)(s_L1Info[3] >> 16);
                break;
            case 50:
                result = (unsigned char)(s_L1Info[3] >> 8);
                break;
            case 51:
                result = (unsigned char)(s_L1Info[3]);
                break;
            case 52:
                result = s_L2Info[2] >> 16;
                break;
            case 53:
                result = (unsigned __int16)s_L2Info[2] >> 12;
                break;
            case 54:
                result = (s_L2Info[2] >> 8) & 0xF;
                break;
            case 55:
                result = (unsigned char)s_L2Info[2];
                break;
            default:
                break;
            }

            if (GetCPUInfo(1) == 6)
            {
                switch (Index)
                {
                case 40:
                    result = (unsigned char)(s_L1Info[0] >> 24);
                    break;
                case 41:
                    result = (unsigned char)(s_L1Info[0] >> 16);
                    break;
                case 42:
                    result = (unsigned char)(s_L1Info[0] >> 8);
                    break;
                case 43:
                    result = (unsigned char)(s_L1Info[0]);
                    break;
                case 56:
                    result = s_L2Info[0] >> 28;
                    break;
                case 57:
                    result = (s_L2Info[0] >> 16) & 0xFFF;
                    break;
                case 58:
                    result = (unsigned __int16)s_L2Info[0] >> 12;
                    break;
                case 59:
                    result = s_L2Info[0] & 0xFFF;
                    break;
                case 60:
                    result = s_L2Info[1] >> 28;
                    break;
                case 61:
                    result = (s_L2Info[1] >> 16) & 0xFFF;
                    break;
                case 62:
                    result = (unsigned __int16)s_L2Info[1] >> 12;
                    break;
                case 63:
                    result = s_L2Info[1] & 0xFFF;
                    break;
                default:
                    return result;
                }
            }
        }
        break;
    }
    return result;
}

void GetCPUVendor(System::AnsiString& Vendor)
{
    switch (GetCPUInfo(0))
    {
    case 1:
        Vendor = "AMD";
        break;
    case 2:
        Vendor = "Intel";
        break;
    case 3:
        Vendor = "Cyrix";
        break;
    case 4:
        Vendor = "Centaur";
        break;
    default:
        Vendor = "Unknown";
        break;
    }
}

int GetCPUCount()
{
    SYSTEM_INFO* si = new SYSTEM_INFO;
    GetSystemInfo(si);
    int n = si->dwNumberOfProcessors;

    if (GetCPUInfo(34))
    {
        unsigned int logical = GetCPUInfo(35);
        if (logical != 0 && n % logical == 0)
            n /= logical;
    }
    return n;
}

int GetCPURegistryMHz()
{
    int mhz;
    TRegistry* r = new TRegistry;
    r->RootKey = HKEY_LOCAL_MACHINE;
    if (r->KeyExists("\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0") &&
        r->OpenKey("\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                   false) &&
        r->ValueExists("~MHz"))
    {
        mhz = r->ReadInteger("~MHz");
    }
    else
    {
        mhz = GetProcessorSpeed();
    }
    r->CloseKey();

    return mhz;
}

void GetCPUDescription(System::AnsiString& Description)
{
    switch (GetCPUInfo(1))
    {
    case 1:
        Description = "AMD Am486";
        break;
    case 2:
        Description = "AMD K5";
        break;
    case 3:
        Description = "AMD K6";
        break;
    case 4:
        Description = "AMD K6 2";
        break;
    case 5:
        Description = "AMD K6 3";
        break;
    case 6:
        Description = "AMD Athlon";
        break;
    case 7:
        Description = "AMD Duron";
        break;
    case 8:
        Description = "AMD AthlonXP";
        break;
    case 9:
        Description = "AMD AthlonMP";
        break;
    case 10:
        Description = "486 DX";
        break;
    case 11:
        Description = "486 SX";
        break;
    case 12:
        Description = "486 DX2";
        break;
    case 13:
        Description = "486 SL";
        break;
    case 14:
        Description = "486 SX2";
        break;
    case 15:
        Description = "486 DX2E";
        break;
    case 16:
        Description = "486 DX4";
        break;
    case 17:
        Description = "Intel Pentium";
        break;
    case 18:
        Description = "Intel Pentium MMX";
        break;
    case 19:
        Description = "Intel Pentium Pro";
        break;
    case 20:
        Description = "Intel Pentium II";
        break;
    case 21:
        Description = "Intel Celeron";
        break;
    case 22:
        Description = "Intel Pentium III";
        break;
    case 23:
        Description = "Intel Pentium IIIXC";
        break;
    case 24:
        Description = "Intel Pentium III Xenon A";
        break;
    case 25:
        Description = "Intel IA-64 Merced";
        break;
    case 26:
        Description = "Intel IA-64 McKinley";
        break;
    case 27:
        Description = "Intel Pentium IV";
        break;
    default:
        Description = "Unknown";
        break;
    }
}

int GetTotalMemory(System::AnsiString& Error)
{
    MEMORYSTATUS* ms = new MEMORYSTATUS;
    ms->dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(ms);

    int mb = ms->dwTotalPhys >> 20;
    if (mb & 7)
        mb = mb + 8 - (mb & 7);
    if (mb <= 1)
        Error = "";
    else
        Error = IntToStr(mb) + " MB";
    delete ms;
    return mb;
}

static void GetDriveInfo(const AnsiString RootPathName, Classes::TStrings* List,
                         bool* OK, unsigned __int64* FreeBytes)
{
    unsigned __int64 free = 0, total = 0, freeCaller = 0;
    bool fail = false;
    bool useOld = false;
    DWORD message = 0;

    UINT driveType = GetDriveTypeA(RootPathName.c_str());

    HMODULE kernel = LoadLibraryA("KERNEL32.DLL");
    BOOL(WINAPI * GetDiskFreeSpaceExA_)(LPCSTR, PULARGE_INTEGER,
                                        PULARGE_INTEGER, PULARGE_INTEGER) =
        (BOOL(WINAPI*)(LPCSTR, PULARGE_INTEGER, PULARGE_INTEGER,
                       PULARGE_INTEGER))GetProcAddress(kernel,
                                                       "GetDiskFreeSpaceExA");
    if (GetDiskFreeSpaceExA_ != NULL)
    {
        SetErrorMode(SEM_FAILCRITICALERRORS);
        if (GetDiskFreeSpaceExA_(
                RootPathName.c_str(), (PULARGE_INTEGER)&freeCaller,
                (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free))
        {
            useOld = false;
            *OK = true;
            if (driveType == 3)
                free = freeCaller;
        }
        else
        {
            message = GetLastError();
            if (message == 120)
                useOld = true;
            else
                fail = true;
        }
    }
    else
    {
        useOld = true;
    }
    FreeLibrary(kernel);

    if (useOld)
    {
        SetErrorMode(SEM_FAILCRITICALERRORS);
        DWORD spc = 0, bps = 0, freeClusters = 0, totalClusters = 0;
        if (GetDiskFreeSpaceA(RootPathName.c_str(), &spc, &bps, &freeClusters,
                              &totalClusters))
        {
            free = (unsigned __int64)freeClusters * spc * bps;
            total = (unsigned __int64)totalClusters * spc * bps;
            *OK = true;
        }
        else
        {
            message = GetLastError();
            fail = true;
        }
    }

    if (fail)
    {
        *OK = false;
        DiscardSystemErrorMessage(message);
    }

    SetErrorMode(SEM_FAILCRITICALERRORS);
    char volName[256], fsName[256];
    DWORD serial = 0, maxComp = 0, flags = 0;
    if (GetVolumeInformationA(RootPathName.c_str(), volName, 256, &serial,
                              &maxComp, &flags, fsName, 256))
    {
        List->Add(AnsiString(fsName));
    }
    else
    {
        DiscardSystemErrorMessage(GetLastError());
    }

    *FreeBytes = free;
}

void ScanHardDrives(Classes::TStrings* List)
{
    DWORD mask = GetLogicalDrives();
    for (int i = 1; i <= 25; i++)
    {
        if ((mask >> i) & 1)
        {
            AnsiString root = AnsiString(char('A' + i)) + ":\\";
            if (GetDriveTypeA(root.c_str()) == 3)
            {
                bool ok = true;
                unsigned __int64* freeBytes = new unsigned __int64;
                TStringList* fileSystems = new TStringList;
                AnsiString name = "";

                char volName[256], fsName[256];
                DWORD serial = 0, maxComp = 0, flags = 0;
                if (GetVolumeInformationA(root.c_str(), volName, 256, &serial,
                                          &maxComp, &flags, fsName, 256))
                    name =
                        AnsiString(char('A' + i)) + ": " + "(" + fsName + ")";
                if (flags & 4)
                    name += ", UNICODE";
                if (flags & 0x8000)
                    name += ", COMPRESSED VOLUME";

                GetDriveInfo(root, fileSystems, &ok, freeBytes);
                if (ok)
                    List->AddObject(name, (TObject*)freeBytes);
                delete fileSystems;
            }
        }
    }
}

bool GetCDROMLetters(Classes::TStrings* List)
{
    int count = 0;
    DWORD mask = GetLogicalDrives();
    for (int i = 1; i <= 25; i++)
    {
        if ((mask >> i) & 1)
        {
            AnsiString root = AnsiString(char('A' + i)) + ":\\";
            if (GetDriveTypeA(root.c_str()) == 5)
            {
                count++;
                List->Add(root);
            }
        }
    }
    for (int j = List->Count - 1; j >= 0; j--)
    {
        if (Trim(List->Strings[j]).Length() == 0)
            List->Delete(j);
    }
    return count != 0;
}

static void ScanCDROMNamesNT4(Classes::TStrings* List)
{
    ScanRegistryKey("\\HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 1\\Scsi Bus 0\\"
                    "Target Id 0\\Logical Unit Id 0\\",
                    "Identifier", "", "CdRomPeripheral", "Type", "", "", List);
    List->Add("");
}

static void ScanCDROMNames9x(Classes::TStrings* List)
{
    ScanRegistryKey("Enum\\SCSI\\", "Manufacturer", "ProductId", "CDROM",
                    "Class", "", "", List);
    List->Add("");
}

static void ScanCDROMNamesNT5(Classes::TStrings* List)
{
    ScanRegistryKey("\\SYSTEM\\CurrentControlSet\\Enum\\IDE\\", "FriendlyName",
                    "", "CDROM", "Class", "", "", List);
    List->Add("");
}

void GetCDROMDriveNames(Classes::TStrings* List)
{
    int platform = GetPlatform();
    if (platform == 2)
        ScanCDROMNamesNT4(List);
    else if (platform == 4)
        ScanCDROMNames9x(List);
    else
        ScanCDROMNamesNT5(List);

    for (int j = List->Count - 1; j >= 0; j--)
    {
        if (Trim(List->Strings[j]).Length() == 0)
            List->Delete(j);
    }
}

int GetGLDeviceCount()
{
    return glsEnumDrivers();
}

bool GetGLDeviceInfo(int Index, System::AnsiString& Name, int* Error)
{
    bool result = false;
    Name = "Unknown";

    if (!RegisterInfoWindowClass())
    {
        *Error = -10;
        return false;
    }

    TGLDriverInfo info;
    if (glsGetDriverInfo(Index, &info) != 0)
    {
        *Error = -1;
        return false;
    }

    if (glsSelectDriver(Index) == 0)
    {
        HWND w = CreateWindowExA(0, "launchclass", "SysInfo", 0, 0, 0, 0, 0,
                                 NULL, NULL, MainInstance, NULL);
        if (w != NULL)
        {
            HDC dc = GetDC(w);
            if (dc != NULL)
            {
                TStringList* errors = new TStringList;
                bool pixelFormat = SetupPixelFormat(dc, errors);
                errors->Clear();
                delete errors;
                if (pixelFormat)
                {
                    HGLRC ctx = pwglCreateContext(dc);
                    if (ctx != NULL)
                    {
                        pwglMakeCurrent(dc, ctx);
                        const unsigned char* s = pglGetString(GLS_GL_VERSION);
                        if (s != NULL)
                        {
                            Name = (const char*)s;
                            *Error = 0;
                            result = true;
                        }
                        pwglDeleteContext(ctx);
                    }
                    else
                    {
                        *Error = -5;
                    }
                }
                else
                {
                    *Error = -6;
                }
            }
            else
            {
                *Error = -7;
            }
            pwglMakeCurrent(0, 0);
            ReleaseDC(w, dc);
            DestroyWindow(w);
            UnregisterClassA("launchclass", MainInstance);
        }
    }
    return result;
}

bool GetGLStrings(int Index, System::AnsiString& Vendor,
                  System::AnsiString& Renderer, int* Error)
{
    bool result = false;
    Vendor = "Unknown";
    Renderer = "Unknown";

    if (!RegisterInfoWindowClass())
    {
        *Error = -10;
        return false;
    }

    TGLDriverInfo info;
    if (glsGetDriverInfo(Index, &info) != 0)
    {
        *Error = -1;
        return false;
    }

    if (glsSelectDriver(Index) == 0)
    {
        HWND w = CreateWindowExA(0, "launchclass", "SysInfo", 0, 0, 0, 0, 0,
                                 NULL, NULL, MainInstance, NULL);
        if (w != NULL)
        {
            HDC dc = GetDC(w);
            if (dc != NULL)
            {
                TStringList* errors = new TStringList;
                bool pixelFormat = SetupPixelFormat(dc, errors);
                errors->Clear();
                delete errors;
                if (pixelFormat)
                {
                    HGLRC ctx = pwglCreateContext(dc);
                    if (ctx != NULL)
                    {
                        pwglMakeCurrent(dc, ctx);
                        const unsigned char* s = pglGetString(GLS_GL_VENDOR);
                        if (s != NULL)
                            Vendor = (const char*)s;
                        s = pglGetString(GLS_GL_RENDERER);
                        if (s != NULL)
                            Renderer = (const char*)s;
                        *Error = 0;
                        result = true;
                        pwglDeleteContext(ctx);
                    }
                    else
                    {
                        *Error = -5;
                    }
                }
                else
                {
                    *Error = -6;
                }
            }
            else
            {
                *Error = -7;
            }
            pwglMakeCurrent(0, 0);
            ReleaseDC(w, dc);
            DestroyWindow(w);
            UnregisterClassA("launchclass", MainInstance);
        }
    }
    return result;
}

void GetVideoCardName(System::AnsiString& Name)
{
    TStringList* list = new TStringList;
    int platform = GetPlatform();
    if (platform == 4)
    {
        EnumRegistryDevices("Enum\\PCI\\", "DeviceDesc", "", "Display", "Class",
                            "", "", list);
        if (list->Count != 0)
            Name = list->Strings[list->Count - 1];
    }
    else if (platform == 2 || platform == 5)
    {
        AnsiString mapped = ReadRegistryString(HKEY_LOCAL_MACHINE,
                                               "HARDWARE\\DEVICEMAP\\VIDEO\\",
                                               "\\Device\\Video0");
        int pos = mapped.Pos("System\\");
        if (pos != 0)
            mapped = mapped.SubString(pos, mapped.Length());
        Name = ReadRegistryString(HKEY_LOCAL_MACHINE, mapped,
                                  "Device Description");
    }
    else
    {
        Name = "";
    }
    delete list;
}

int GetVideoMemory()
{
    int result = -1;
    LPDIRECTDRAW dd = NULL;

    if (DirectDrawCreate(NULL, &dd, NULL) >= 0)
    {
        dd->Release();
        DirectDrawCreate(NULL, &dd, NULL);

        unsigned long driverCaps[95];
        unsigned long helCaps[95];
        memset(driverCaps, 0, sizeof(driverCaps));
        memset(helCaps, 0, sizeof(helCaps));
        driverCaps[0] = 380;
        helCaps[0] = 380;
        dd->GetCaps((LPDDCAPS)driverCaps, (LPDDCAPS)helCaps);
        result = driverCaps[15];
        dd->Release();
    }

    if (result <= 0)
    {

        if (DirectDrawCreate(NULL, &dd, NULL) >= 0)
        {
            LPDIRECTDRAW7 dd7 = NULL;

            static const IID kIID_IDirectDraw7 = {
                0x15e65ec0,
                0x3b9c,
                0x11d2,
                {0xb9, 0x2f, 0x00, 0x60, 0x97, 0x97, 0xea, 0x5b}};
            if (dd->QueryInterface(kIID_IDirectDraw7, (void**)&dd7) >= 0)
            {
                dd->Release();
                DDSCAPS2 caps;
                memset(&caps, 0, sizeof(caps));
                caps.dwCaps = DDSCAPS_VIDEOMEMORY;
                DWORD bytes = 0;
                if (dd7->GetAvailableVidMem(&caps, &bytes, NULL) != 0)
                    result = -1;
                else if (bytes != 0)
                    result = RoundUpToPowerOfTwoMB(bytes);
                else
                    result = -1;
                dd7->Release();
            }
            else
            {
                dd->Release();
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        result = RoundUpToPowerOfTwoMB(result);
    }
    return result;
}

int GetVideoMemoryFallback()
{
    int result = 0;
    AnsiString version, name;
    GetOSVersion(version, name);
    if (name.Pos("9") != 0 || name.Pos("ME") != 0)
    {
        TRegistry* r = new TRegistry;
        r->RootKey = HKEY_LOCAL_MACHINE;
        if (r->KeyExists("\\System\\CurrentControlSet\\Services\\Class\\Display"
                         "\\0000\\INFO") &&
            r->OpenKeyReadOnly("\\System\\CurrentControlSet\\Services\\Class\\D"
                               "isplay\\0000\\INFO") &&
            r->ValueExists("VideoMemory"))
        {
            result = r->ReadInteger("VideoMemory");
        }

    }
    return result;
}

bool GetDriverVersion(int Vendor, System::AnsiString& Version)
{
    bool result = false;
    Version = "";

    if (Vendor == 0)
    {
        char buf[260];
        GetSystemDirectoryA(buf, 260);
        AnsiString path =
            AnsiString(buf) +
            (GetPlatform() == 4 ? "\\nvopengl.dll" : "\\nvoglnt.dll");
        int* quad = new int[4];
        GetVersionQuad(quad, path);
        Version.sprintf("%d.%d.%d.%d", quad[0], quad[1], quad[2], quad[3]);
        result = true;
        delete[] quad;
    }
    else if (Vendor == 1)
    {
        int n = GetGLDeviceCount();
        if (n > 0)
        {
            int error;
            int i = 0;
            do
            {
                if (GetGLDeviceInfo(i, Version, &error))
                    break;
                i++;
            } while (n > i);
            Version = Trim(Version);
            result = (n > i);
        }
    }
    return result;
}

bool GetDisplayMode(int& Width, int& Height, int& BitsPerPixel)
{
    HWND desktop = GetDesktopWindow();
    HDC dc = GetDC(desktop);
    if (dc == NULL)
        return false;

    Width = 0;
    Height = 0;
    BitsPerPixel = 0;

    BitsPerPixel = GetDeviceCaps(dc, 12);
    Width = GetSystemMetrics(0);
    Height = GetSystemMetrics(1);

    ReleaseDC(desktop, dc);
    return true;
}

bool GetRefreshRate(int& RefreshRate)
{
    AnsiString version, name;
    GetOSVersion(version, name);
    if (name.Pos("9") != 0 || name.Pos("ME") != 0)
        return false;

    HDC dc = GetDC(GetDesktopWindow());
    if (dc == NULL)
        return false;

    int rate = GetDeviceCaps(dc, 116);
    RefreshRate = rate;
    ReleaseDC(GetDesktopWindow(), dc);
    return rate > 1;
}

static AnsiString g_DSSelectedDescription;

struct TDSDeviceRec
{
    GUID* Guid;
    AnsiString Description;
    AnsiString Module;
};

static TList* g_DSDeviceList = NULL;

static BOOL __stdcall DSEnumCallbackFirstDevice(GUID*  ,
                                                const char* Description,
                                                const char*  ,
                                                void*  )
{
    AnsiString t = Trim(AnsiString(Description));
    if (t.Pos("emulat") == 0 && t.Pos("primary") == 0)
        g_DSSelectedDescription = AnsiString(Description);
    return FALSE;
}

static BOOL __stdcall DSEnumCallbackCollect(GUID* Guid, const char* Description,
                                            const char* Module,
                                            void*  )
{
    TDSDeviceRec* rec = new TDSDeviceRec;
    rec->Guid = NULL;
    if (Guid != NULL)
    {
        rec->Guid = new GUID;
        memcpy(rec->Guid, Guid, sizeof(GUID));
    }
    rec->Description = AnsiString(Description);
    rec->Module = AnsiString(Module);
    g_DSDeviceList->Add(rec);
    return TRUE;
}

static void ScanSoundDevicesNT4(Classes::TStrings* Descriptions)
{
    DirectSoundEnumerateA(DSEnumCallbackFirstDevice, NULL);
    Descriptions->Add(g_DSSelectedDescription);
    Descriptions->Add("");
}

static void ScanSoundDevices9x(Classes::TStrings* Descriptions)
{
    EnumRegistryDevices("Enum\\ISAPNP\\", "DeviceDesc", "", "Media", "Class",
                        "Audio", "FunctionDesc", Descriptions);
    EnumRegistryDevices("Enum\\MF\\", "DeviceDesc", "", "Media", "Class", "",
                        "", Descriptions);
    EnumRegistryDevices("Enum\\PCI\\", "DeviceDesc", "", "Media", "Class", "",
                        "", Descriptions);
    EnumRegistryDevices("Enum\\USB\\", "DeviceDesc", "", "Media", "Class", "",
                        "", Descriptions);
    Descriptions->Add("");
}

static void ScanSoundDevicesNT5(Classes::TStrings* Descriptions)
{
    EnumRegistryDevices("\\SYSTEM\\CurrentControlSet\\Enum\\ISAPNP\\",
                        "DeviceDesc", "", "Media", "Class", "", "",
                        Descriptions);
    EnumRegistryDevices("\\SYSTEM\\CurrentControlSet\\Enum\\MF\\", "DeviceDesc",
                        "", "Media", "Class", "", "", Descriptions);
    EnumRegistryDevices("\\SYSTEM\\CurrentControlSet\\Enum\\PCI\\",
                        "DeviceDesc", "", "Media", "Class", "", "",
                        Descriptions);
    EnumRegistryDevices("\\SYSTEM\\CurrentControlSet\\Enum\\USB\\",
                        "DeviceDesc", "", "Media", "Class", "", "",
                        Descriptions);
    Descriptions->Add("");
}

static void EnumSoundDevices(int*  , Classes::TStrings*  ,
                             Classes::TStrings* Descriptions)
{
    int platform = GetPlatform();
    if (platform == 2)
        ScanSoundDevicesNT4(Descriptions);
    else if (platform == 4)
        ScanSoundDevices9x(Descriptions);
    else
        ScanSoundDevicesNT5(Descriptions);
}

static bool GetSoundCardNameDS(System::AnsiString& Name)
{
    bool found = false;
    Name = "";

    g_DSDeviceList = new TList;
    if (DirectSoundEnumerateA(DSEnumCallbackCollect, NULL) >= 0)
    {
        for (int i = 0; i < g_DSDeviceList->Count; i++)
        {
            TDSDeviceRec* rec = (TDSDeviceRec*)g_DSDeviceList->Items[i];
            if (rec != NULL && rec->Description.Length() != 0 &&
                Trim(rec->Description).Pos("default") == 0 &&
                Trim(rec->Description).Pos("primary") == 0)
            {
                if (!found)
                    Name = rec->Description;
                found = true;
            }
            delete rec->Guid;
            rec->Description = "";
            rec->Module = "";
            delete rec;
        }
        delete g_DSDeviceList;
        return found;
    }
    delete g_DSDeviceList;
    return false;
}

bool GetSoundCardName(System::AnsiString& Name)
{
    int error = 0;
    TStringList* list = new TStringList;
    TStringList* descriptions = new TStringList;

    EnumSoundDevices(&error, list, descriptions);
    if (descriptions->Count != 0)
    {
        Name = Trim(descriptions->Strings[0]);
        for (int i = 1; i < descriptions->Count; i++)
        {
            if (Name != "")
                break;
            Name = Trim(descriptions->Strings[i]);
        }
        if (Name == "")
        {
            delete list;
            delete descriptions;
            return GetSoundCardNameDS(Name);
        }
        delete list;
        delete descriptions;
        return true;
    }
    delete list;
    delete descriptions;
    return GetSoundCardNameDS(Name);
}

int GetSoundCardNameAlt(System::AnsiString& Name)
{
    TRegistry* r = new TRegistry;
    r->RootKey = HKEY_LOCAL_MACHINE;
    if (r->KeyExists(
            "\\System\\CurrentControlSet\\Services\\Class\\Media\\0000") &&
        r->OpenKeyReadOnly(
            "\\System\\CurrentControlSet\\Services\\Class\\Media\\0000"))
    {
        if (r->ValueExists("DriverDesc"))
        {
            Name = r->ReadString("DriverDesc");
            delete r;
            return 1;
        }
        if (r->ValueExists("DriverDescription"))
        {
            Name = r->ReadString("DriverDescription");
            delete r;
            return 1;
        }
    }

    AnsiString waveKey =
        "\\System\\CurrentControlSet\\Control\\MediaResources\\Wave";
    if (r->KeyExists(waveKey) && r->OpenKeyReadOnly(waveKey))
    {
        TStringList* names = new TStringList;
        r->GetKeyNames(names);
        for (int i = 0; i < names->Count; i++)
        {
            AnsiString sub = waveKey + "\\" + names->Strings[i];
            if (r->KeyExists(sub) && r->OpenKeyReadOnly(sub))
            {
                if (r->ValueExists("FriendlyName"))
                {
                    Name = r->ReadString("FriendlyName");
                    delete names;
                    delete r;
                    return 1;
                }
                if (r->ValueExists("Description"))
                {
                    Name = r->ReadString("Description");
                    delete names;
                    delete r;
                    return 1;
                }
            }
        }
        delete names;
    }
    delete r;
    return 0;
}

void GetDirectXVersion(System::AnsiString& Version,
                       System::AnsiString& Friendly)
{
    AnsiString version = "";
    AnsiString friendly = "Unknown";

    TRegistry* r = new TRegistry;
    r->RootKey = HKEY_LOCAL_MACHINE;
    if (r->OpenKey("SOFTWARE\\MICROSOFT\\DirectX", false))
    {
        if (r->ValueExists("Version"))
        {
            version = r->ReadString("Version");
            MapDXVersion(version, friendly);
        }
        if (r->ValueExists("InstalledVersion"))
        {
            int size = r->GetDataSize("InstalledVersion");
            unsigned char* data = new unsigned char[size];
            r->ReadBinaryData("InstalledVersion", data, size);
            if (data[0] == 3)
            {
                friendly = "DirectX 3 (WinNT)";
                version = "4.04.00.0068";
            }

        }
    }
    Version = version;
    Friendly = friendly;
    delete r;
}

