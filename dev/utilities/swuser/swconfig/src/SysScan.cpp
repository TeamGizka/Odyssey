#include <vcl.h>
#pragma hdrstop

#include "SysScan.h"
#include "Nwcsysinfo.h"
#include "Swutil.h"
#include <math.h>
#include <io.h>
#include <IniFiles.hpp>
#include <stdio.h>

TSysScan::TSysScan(Classes::TComponent* ProgressControl)
{
    ScanCPU = true;
    ScanOS = true;
    ScanRAM = true;
    ScanHD = true;
    ScanVideo = true;
    ScanSound = true;
    ScanDirectX = true;
    ScanOpenGL = true;
    ScanCDROM = true;
    ProgressBar = NULL;
    Gauge = NULL;
    GaugeClassName = "";

    if (ProgressControl != NULL)
    {
        AnsiString cls = ProgressControl->ClassName();
        if (cls == "TProgressBar")
        {
            ProgressBar = (Comctrls::TProgressBar*)ProgressControl;
            GaugeClassName = "TProgressBar";
        }
        else if (cls == "TCGauge")
        {
            Gauge = (TCGauge*)ProgressControl;
            GaugeClassName = "TCGauge";
        }
        else
        {
            GaugeClassName = "";
        }
    }
    else
    {
        GaugeClassName = "";
    }
}

TSysScan::~TSysScan()
{
}

void TSysScan::Begin()
{
    int total = ScanCPU + ScanOS + ScanRAM + ScanHD + ScanVideo + ScanSound +
                ScanDirectX + ScanOpenGL + ScanCDROM;

    if (GaugeClassName == "TProgressBar")
    {
        ProgressBar->Min = 0;
        ProgressBar->Max = total;
        ProgressBar->Position = 0;
        Application->ProcessMessages();
    }
    else if (GaugeClassName == "TCGauge")
    {
        Gauge->MinValue = 0;
        Gauge->MaxValue = total;
        Gauge->Progress = 0;
        Application->ProcessMessages();
    }
}

void TSysScan::Step()
{
    if (GaugeClassName == "TProgressBar")
    {
        ProgressBar->Position = ProgressBar->Position + 1;
        Application->ProcessMessages();
    }
    else if (GaugeClassName == "TCGauge")
    {
        Gauge->Progress = Gauge->Progress + 1;
        Application->ProcessMessages();
    }
}

static bool ScanOS(TSysScanData* Data)
{
    GetOSVersion(Data->OSVersion, Data->OSName);
    GetOSServicePack(Data->OSServicePack);
    GetSwapFileList(Data->SwapFiles);
    return Data->OSName != "???";
}

static AnsiString CleanCPUName(const AnsiString Name)
{
    AnsiString s = Name;
    int p;

    while ((p = s.Pos(" ")) > 0)
    {
        s.Unique();
        s[p] = '\n';
        while (p < s.Length() && s[p + 1] == ' ')
            s.Delete(p + 1, 1);
    }
    while ((p = s.Pos("\n")) > 0)
    {
        s.Unique();
        s[p] = ' ';
    }
    return s;
}

static bool ScanCPU(TSysScan* Scan, TSysScanData* Data)
{
    bool ok = true;

    Data->CPUCount = GetCPUCount();
    if (Data->CPUCount == -1)
        Data->CPUCount = 1;
    Data->CPUSpeed = GetCPURegistryMHz();

    AnsiString vendor;
    GetCPUVendor(vendor);
    Data->CPUVendor = vendor;

    Data->CPUFamily = GetCPUInfo(4);
    Data->CPUModel = GetCPUInfo(5);
    Data->CPUStepping = GetCPUInfo(6);

    const char* regName = (const char*)GetCPUInfo(3);
    if (regName != NULL && regName[0] != '\0')
    {
        AnsiString name = CleanCPUName(regName);
        if (name.Pos("+") == 0 && LowerCase(name).Pos("hz") == 0)
            Data->CPUName =
                Format("%s %uMHz", ARRAYOFCONST((name, (int)Data->CPUSpeed)));
        else
            Data->CPUName = name;
    }
    else
    {
        AnsiString ident;
        GetCPUDescription(ident);
        AnsiString name = ident + " ";
        if (name.Pos("AthlonXP") != 0 || name.Pos("AthlonMP") != 0)
        {

            int model = Data->CPUSpeed +
                        (int)floor(0.015 * (Data->CPUSpeed - 1000) + 0.5);
            Data->CPUName = Format("%s%d+", ARRAYOFCONST((name, model)));
        }
        else
        {
            Data->CPUName =
                Format("%s%uMHz", ARRAYOFCONST((name, (int)Data->CPUSpeed)));
        }
    }
    return ok;
}

static bool ScanRAM(TSysScanData* Data)
{
    AnsiString error;
    Data->RAM = GetTotalMemory(error);
    return Data->RAM != 0;
}

static bool ScanHD(TSysScanData* Data)
{
    for (int i = 0; i < Data->DiskNames->Count; i++)
        _rtl_close((int)Data->DiskNames->Objects[i]);
    Data->DiskNames->Clear();
    ScanHardDrives(Data->DiskNames);
    for (int i = 0; i < Data->DiskNames->Count; i++)
        if (Data->DiskNames->Objects[i] == NULL)
            return false;
    return true;
}

static bool ScanCDROM(TSysScanData* Data)
{
    Data->CDROMDrives->Clear();
    Data->CDROMLetters->Clear();
    if (GetCDROMLetters(Data->CDROMLetters))
        GetCDROMDriveNames(Data->CDROMDrives);
    else
    {
        Data->CDROMDrives->Clear();
        Data->CDROMLetters->Clear();
    }
    return true;
}

static bool ScanOpenGL(TSysScanData* Data)
{
    int count = GetGLDeviceCount();
    if (count >= 1)
    {
        int i;
        AnsiString name;
        int data = 0;
        for (i = 0; i < count; i++)
            if (GetGLDeviceInfo(i, name, &data))
                break;
        if (i == count)
        {
            Data->OpenGLInstalled = false;
            Data->OpenGLVersion = "Unknown";
            return false;
        }
        GetGLStrings(i, Data->OpenGLVendor, Data->OpenGLRenderer, &data);
        Data->OpenGLVendor = Trim(Data->OpenGLVendor);
        Data->OpenGLRenderer = Trim(Data->OpenGLRenderer);
        Data->OpenGLInstalled = true;
        Data->OpenGLVersion = Trim(name);
        return true;
    }
    Data->OpenGLInstalled = false;
    Data->OpenGLVersion = "";
    return true;
}

static bool ContainsText(const AnsiString Text, const char* Sub)
{
    return LowerCase(Text).Pos(Sub) > 0;
}

static bool ScanVideo(TSysScanData* Data)
{
    bool ok = true;

    GetVideoCardName(Data->VideoCardName);
    int vmem = GetVideoMemory();
    if (vmem == -1)
    {
        Data->VideoMemory = GetVideoMemoryFallback();
        if (Data->VideoMemory == 0)
            ok = false;
    }
    else
        Data->VideoMemory = vmem;

    bool nvidia = ContainsText(Data->VideoCardName, "nvidia") ||
                  ContainsText(Data->VideoCardName, "geforce") ||
                  ContainsText(Data->VideoCardName, "nforce") ||
                  ContainsText(Data->OpenGLVendor, "nvidia") ||
                  ContainsText(Data->OpenGLVendor, "geforce") ||
                  ContainsText(Data->OpenGLVendor, "nforce") ||
                  ContainsText(Data->OpenGLRenderer, "nvidia") ||
                  ContainsText(Data->OpenGLRenderer, "geforce") ||
                  ContainsText(Data->OpenGLRenderer, "nforce");
    if (nvidia)
    {
        Data->GpuVendor = 0;
        if (!GetDriverVersion(0, Data->NVidiaDriver))
            ok = false;
    }
    else
    {
        bool ati = ContainsText(Data->VideoCardName, "ati") ||
                   ContainsText(Data->VideoCardName, "radeon") ||
                   ContainsText(Data->OpenGLVendor, "ati") ||
                   ContainsText(Data->OpenGLVendor, "radeon") ||
                   ContainsText(Data->OpenGLRenderer, "ati") ||
                   ContainsText(Data->OpenGLRenderer, "radeon");
        if (ati)
        {
            Data->GpuVendor = 1;
            if (!GetDriverVersion(1, Data->NVidiaDriver))
                ok = false;
        }
        else
        {
            Data->GpuVendor = -1;
            if (!GetDriverVersion(-1, Data->NVidiaDriver))
                ok = false;
        }
    }

    int width, height, bpp;
    if (GetDisplayMode(width, height, bpp))
    {
        Data->DesktopWidth = width;
        Data->DesktopHeight = height;
        Data->DesktopBitsPerPixel = bpp;
    }
    else
    {
        ok = false;
        Data->DesktopWidth = 0;
        Data->DesktopHeight = 0;
        Data->DesktopBitsPerPixel = 0;
    }

    int refresh;
    if (GetRefreshRate(refresh))
        Data->RefreshRate = refresh;
    else
        Data->RefreshRate = 0;

    return ok;
}

static bool ScanSound(TSysScanData* Data)
{
    if (!GetSoundCardName(Data->SoundCardName) &&
        GetSoundCardNameAlt(Data->SoundCardName) == 0)
    {
        Data->SoundCardName = "";
        return false;
    }
    return true;
}

static bool ScanDirectX(TSysScanData* Data)
{
    AnsiString friendly;
    GetDirectXVersion(Data->DirectXVersion, friendly);
    Data->DirectXFriendly = friendly;
    Data->DirectXInstalled = Data->DirectXVersion != "Unknown";
    return true;
}

bool __fastcall RunScan(TSysScan* Scan, TSysScanData* Data)
{
    bool ok = true;

    Data->Errors->Clear();
    Data->Warnings->Clear();

    Scan->Begin();
    if (Scan->ScanOS)
    {
        ok = ScanOS(Data) && ok;
        Scan->Step();
    }
    if (Scan->ScanCPU)
    {
        ok = ScanCPU(Scan, Data) && ok;
        Scan->Step();
    }
    if (Scan->ScanRAM)
    {
        ok = ScanRAM(Data) && ok;
        Scan->Step();
    }
    if (Scan->ScanHD)
    {
        ok = ScanHD(Data) && ok;
        Scan->Step();
    }
    if (Scan->ScanCDROM)
    {
        ok = ScanCDROM(Data) && ok;
        Scan->Step();
    }
    if (Scan->ScanOpenGL)
    {
        ok = ScanOpenGL(Data) && ok;
        Scan->Step();
    }
    if (Scan->ScanVideo)
    {
        ok = ScanVideo(Data) && ok;
        Scan->Step();
    }
    if (Scan->ScanSound)
    {
        ok = ScanSound(Data) && ok;
        Scan->Step();
    }
    if (Scan->ScanDirectX)
    {
        ok = ScanDirectX(Data) && ok;
        Scan->Step();
    }
    return ok;
}

bool GetRecommendedDefaults(TSysScanData* Data, int Settings[17])
{
    Settings[0] = 800;
    Settings[1] = 600;
    Settings[3] = 60;
    Settings[11] = 0;

    AnsiString gameDir;
    GetGameInstallDir(gameDir);
    AnsiString tpb = gameDir + "TexturePacks\\swpc_tex_tpb.erf";
    AnsiString tpa = gameDir + "TexturePacks\\swpc_tex_tpa.erf";

    if (Data->VideoMemory >= 128 && FileExists(tpb))
        Settings[4] = 2;
    else
        Settings[4] = Data->VideoMemory >= 64 && FileExists(tpa);

    Settings[2] = 0;
    Settings[13] = 0;
    Settings[14] = 0;
    Settings[12] = 0;
    Settings[15] = 1;
    Settings[16] = 0;
    Settings[5] = 1;
    Settings[6] = 1;
    Settings[7] = 0;
    Settings[8] = 0;
    Settings[9] = 57;
    Settings[10] = 1;
    return true;
}

bool __fastcall GetRecommendedSettings(TSysScan* Scan, TSysScanData* Data,
                                       int Settings[17])
{
    if (!RunScan(Scan, Data))
        return false;
    return GetRecommendedDefaults(Data, Settings);
}

static void ReportStatus(FILE* Stream, const char* Key, int Result)
{
    switch (Result)
    {
    case 0:
        fprintf(Stream, "%s=Pass\n", Key);
        break;
    case 1:
        fprintf(Stream, "%s=Fail\n", Key);
        break;
    case 2:
        fprintf(Stream, "%s=Warning\n", Key);
        break;
    }
}

static const char* StrOrEmpty(const AnsiString& S)
{
    return S.IsEmpty() ? "" : S.c_str();
}

static void WriteReportFile(TSysScanData* Data, FILE* Stream,
                            const unsigned char* Results, bool GameExists)
{
    fprintf(Stream, "[SWKotOR]\n");
    fprintf(Stream, "ReportDateTime=%s\n", DateTimeToStr(Now()).c_str());

    AnsiString version;
    if (!GetFileVersionText(Application->ExeName, version, 1))
        version = "Unknown";
    fprintf(Stream, "SysInfoVersion=%s\n", StrOrEmpty(version));

    fprintf(Stream, "GameExists=%d\n", GameExists ? 1 : 0);
    AnsiString gameDir;
    if (GameExists)
    {
        GetGameInstallDir(gameDir);
        if (!GetFileVersionText(gameDir + "swkotor.exe", version, 1))
            version = "Unknown";
        fprintf(Stream, "GameVersion=%s\n", StrOrEmpty(version));
        fprintf(Stream, "GameInstallLocation=%s\n", StrOrEmpty(gameDir));
    }

    fprintf(Stream, "\n");
    fprintf(Stream, "[OS]\n");
    fprintf(Stream, "Name=%s\n", StrOrEmpty(Data->OSName));
    fprintf(Stream, "Version=%s\n", StrOrEmpty(Data->OSVersion));
    fprintf(Stream, "Service Pack=%s\n", StrOrEmpty(Data->OSServicePack));
    if (Results != NULL)
        ReportStatus(Stream, "Status", Results[0]);

    fprintf(Stream, "\n");
    fprintf(Stream, "[SwapFiles]\n");
    for (int i = 0; i < Data->SwapFiles->Count; i++)
        fprintf(Stream, "%s\n", Data->SwapFiles->Strings[i].c_str());

    fprintf(Stream, "\n");
    fprintf(Stream, "[CPU]\n");
    fprintf(Stream, "CPUCount=%u\n", Data->CPUCount);
    fprintf(Stream, "CPUSpeed=%u\n", Data->CPUSpeed);
    fprintf(Stream, "CPUFamily=%u\n", Data->CPUFamily);
    fprintf(Stream, "CPUModel=%u\n", Data->CPUModel);
    fprintf(Stream, "CPUStepping=%u\n", Data->CPUStepping);
    fprintf(Stream, "CPUVendor=%s\n", StrOrEmpty(Data->CPUVendor));
    fprintf(Stream, "CPUName=%s\n", StrOrEmpty(Data->CPUName));
    if (Results != NULL)
        ReportStatus(Stream, "Status", Results[1]);

    fprintf(Stream, "\n");
    fprintf(Stream, "[Memory]\n");
    fprintf(Stream, "RAM=%u\n", Data->RAM);
    if (Results != NULL)
        ReportStatus(Stream, "Status", Results[2]);

    fprintf(Stream, "\n");
    fprintf(Stream, "[Disk Free Space]\n");
    for (int j = 0; j < Data->DiskNames->Count; j++)
    {
        AnsiString name = Data->DiskNames->Strings[j];
        __int64 kb = *(__int64*)Data->DiskNames->Objects[j] / 0x400;
        AnsiString size;
        if (kb <= 0x100000)
            size = Format("%.0lfMB", ARRAYOFCONST(((double)kb / 1024.0)));
        else
            size = Format("%.2lfGB", ARRAYOFCONST(((double)kb / 1048576.0)));
        fprintf(Stream, "%s=%s\n", StrOrEmpty(name), StrOrEmpty(size));
    }
    if (Results != NULL)
        ReportStatus(Stream, "Status", Results[3]);

    fprintf(Stream, "\n");
    fprintf(Stream, "[CD-ROMs]\n");
    fprintf(Stream, "DriveLetters=");
    if (Data->CDROMLetters->Count != 0)
    {
        fprintf(Stream, "%s", Trim(Data->CDROMLetters->Strings[0]).c_str());
        for (int k = 1; k < Data->CDROMLetters->Count; k++)
            fprintf(Stream, ",%s",
                    Trim(Data->CDROMLetters->Strings[k]).c_str());
        fprintf(Stream, "\nDrives=");
        if (Data->CDROMDrives->Count != 0)
        {
            fprintf(Stream, "%s", Trim(Data->CDROMDrives->Strings[0]).c_str());
            for (int m = 1; m < Data->CDROMDrives->Count; m++)
                fprintf(Stream, ",%s",
                        Trim(Data->CDROMDrives->Strings[m]).c_str());
        }
        fprintf(Stream, "\n\n");
    }
    else
    {
        fprintf(Stream, "\nDrives=\n\n");
    }

    AnsiString gl = Data->OpenGLInstalled ? Data->OpenGLVersion
                                          : AnsiString("Not Installed");
    AnsiString dx = Data->DirectXInstalled ? Data->DirectXVersion
                                           : AnsiString("Not Installed");

    fprintf(Stream, "[Video]\n");
    fprintf(Stream, "Video Card Name=%s\n", StrOrEmpty(Data->VideoCardName));
    fprintf(Stream, "Video Memory=%u\n", Data->VideoMemory);
    fprintf(Stream, "Desktop Resolution=%ux%ux%u", Data->DesktopWidth,
            Data->DesktopHeight, Data->DesktopBitsPerPixel);
    if (Data->RefreshRate != 0)
        fprintf(Stream, " @ %uHz", Data->RefreshRate);
    fprintf(Stream, "\n");
    fprintf(Stream, "DirectX=%s\n", StrOrEmpty(dx));
    fprintf(Stream, "OpenGL Version=%s\n", StrOrEmpty(gl));
    fprintf(Stream, "OpenGL Vendor=%s\n", Data->OpenGLVendor.c_str());
    fprintf(Stream, "OpenGL Renderer=%s\n", Data->OpenGLRenderer.c_str());
    if (Data->GpuVendor == 0)
        fprintf(Stream, "NVidia Driver=%s\n", Data->NVidiaDriver.c_str());
    if (Results != NULL)
    {
        ReportStatus(Stream, "Vid Card Status", Results[4]);
        ReportStatus(Stream, "Vid Card Driver Status", Results[8]);
        ReportStatus(Stream, "GL Status", Results[6]);
        ReportStatus(Stream, "DX Status", Results[7]);
    }

    fprintf(Stream, "\n");
    fprintf(Stream, "[Audio]\n");
    fprintf(Stream, "Sound Card Name=%s\n", StrOrEmpty(Data->SoundCardName));
    if (Results != NULL)
        ReportStatus(Stream, "Status", Results[5]);

    if (GameExists)
    {
        fprintf(Stream, "\n\n;Game Options from swkotor.ini\n\n");
        TIniFile* ini = new TIniFile(gameDir + "swkotor.ini");
        Classes::TStringList* sections = new Classes::TStringList();
        try
        {
            ini->ReadSections(sections);
            for (int n = 0; n < sections->Count; n++)
            {
                Classes::TStringList* values = new Classes::TStringList();
                ini->ReadSectionValues(sections->Strings[n], values);
                fprintf(Stream, "[%s]\n", sections->Strings[n].c_str());
                for (int ii = 0; ii < values->Count; ii++)
                    fprintf(Stream, "%s\n", values->Strings[ii].c_str());
                fprintf(Stream, "\n");
                delete values;
            }
        }
        __finally
        {
            delete sections;
            delete ini;
        }
    }
}

static void WriteReportList(TSysScanData* Data, Classes::TStrings* List,
                            bool GameExists)
{
    List->Add("[SWKotOR]");
    List->Add(
        Format("ReportDateTime=%s", ARRAYOFCONST((DateTimeToStr(Now())))));

    AnsiString version;
    if (!GetFileVersionText(Application->ExeName, version, 1))
        version = "Unknown";
    List->Add(Format("SysInfoVersion=%s", ARRAYOFCONST((version))));

    List->Add(Format("GameExists=%d", ARRAYOFCONST((GameExists ? 1 : 0))));
    AnsiString gameDir;
    if (GameExists)
    {
        GetGameInstallDir(gameDir);
        if (!GetFileVersionText(gameDir + "swkotor.exe", version, 1))
            version = "Unknown";
        List->Add(Format("GameVersion=%s", ARRAYOFCONST((version))));
        List->Add(Format("GameInstallLocation=%s", ARRAYOFCONST((gameDir))));
    }

    List->Add("");
    List->Add("[OS]");
    List->Add(Format("Name=%s", ARRAYOFCONST((Data->OSName))));
    List->Add(Format("Version=%s", ARRAYOFCONST((Data->OSVersion))));
    List->Add(Format("Service Pack=%s", ARRAYOFCONST((Data->OSServicePack))));

    List->Add("");
    List->Add("[SwapFiles]");
    for (int i = 0; i < Data->SwapFiles->Count; i++)
        List->Add(Data->SwapFiles->Strings[i]);

    List->Add("");
    List->Add("[CPU]");
    List->Add(Format("CPUCount=%u", ARRAYOFCONST(((int)Data->CPUCount))));
    List->Add(Format("CPUSpeed=%u", ARRAYOFCONST(((int)Data->CPUSpeed))));
    List->Add(Format("CPUFamily=%u", ARRAYOFCONST(((int)Data->CPUFamily))));
    List->Add(Format("CPUModel=%u", ARRAYOFCONST(((int)Data->CPUModel))));
    List->Add(Format("CPUStepping=%u", ARRAYOFCONST(((int)Data->CPUStepping))));
    List->Add(Format("CPUVendor=%s", ARRAYOFCONST((Data->CPUVendor))));
    List->Add(Format("CPUName=%s", ARRAYOFCONST((Data->CPUName))));

    List->Add("");
    List->Add("[Memory]");
    List->Add(Format("RAM=%u", ARRAYOFCONST(((int)Data->RAM))));

    List->Add("");
    List->Add("[Disk Free Space]");
    for (int j = 0; j < Data->DiskNames->Count; j++)
    {
        AnsiString name = Data->DiskNames->Strings[j];
        __int64 kb = *(__int64*)Data->DiskNames->Objects[j] / 0x400;
        AnsiString size;
        if (kb <= 0x100000)
            size = Format("%.0lfMB", ARRAYOFCONST(((double)kb / 1024.0)));
        else
            size = Format("%.2lfGB", ARRAYOFCONST(((double)kb / 1048576.0)));
        List->Add(Format("%s=%s", ARRAYOFCONST((name, size))));
    }

    List->Add("");
    List->Add("[CD-ROMs]");
    if (Data->CDROMLetters->Count != 0)
    {
        AnsiString letters = Trim(Data->CDROMLetters->Strings[0]);
        for (int k = 1; k < Data->CDROMLetters->Count; k++)
            letters += "," + Trim(Data->CDROMLetters->Strings[k]);
        AnsiString drives;
        if (Data->CDROMDrives->Count != 0)
        {
            drives = Trim(Data->CDROMDrives->Strings[0]);
            for (int m = 1; m < Data->CDROMDrives->Count; m++)
                drives += "," + Trim(Data->CDROMDrives->Strings[m]);
        }
        List->Add("DriveLetters=" + letters);
        List->Add("Drives=" + drives);
    }
    else
    {
        List->Add("DriveLetters=Not Installed");
        List->Add("Drives=Not Installed");
    }

    AnsiString gl = Data->OpenGLInstalled ? Data->OpenGLVersion
                                          : AnsiString("Not Installed");
    AnsiString dx = Data->DirectXInstalled ? Data->DirectXVersion
                                           : AnsiString("Not Installed");

    List->Add("");
    List->Add("[Video]");
    List->Add(
        Format("Video Card Name=%s", ARRAYOFCONST((Data->VideoCardName))));
    List->Add(
        Format("Video Memory=%u", ARRAYOFCONST(((int)Data->VideoMemory))));
    AnsiString res =
        Format("%ux%ux%u",
               ARRAYOFCONST(((int)Data->DesktopWidth, (int)Data->DesktopHeight,
                             (int)Data->DesktopBitsPerPixel)));
    if (Data->RefreshRate != 0)
        res += Format(" @ %uHz", ARRAYOFCONST(((int)Data->RefreshRate)));
    List->Add("Desktop Resolution=" + res);
    List->Add(Format("DirectX=%s", ARRAYOFCONST((dx))));
    List->Add(Format("OpenGL Version=%s", ARRAYOFCONST((gl))));
    List->Add(Format("OpenGL Vendor=%s", ARRAYOFCONST((Data->OpenGLVendor))));
    List->Add(
        Format("OpenGL Renderer=%s", ARRAYOFCONST((Data->OpenGLRenderer))));
    if (Data->GpuVendor == 0)
        List->Add(
            Format("NVidia Driver=%s", ARRAYOFCONST((Data->NVidiaDriver))));

    List->Add("");
    List->Add("[Audio]");
    List->Add(
        Format("Sound Card Name=%s", ARRAYOFCONST((Data->SoundCardName))));

    if (GameExists)
    {
        List->Add("");
        List->Add("");
        List->Add(";Game Options from swkotor.ini");
        List->Add("");
        TIniFile* ini = new TIniFile(gameDir + "swkotor.ini");
        Classes::TStringList* sections = new Classes::TStringList();
        try
        {
            ini->ReadSections(sections);
            for (int n = 0; n < sections->Count; n++)
            {
                Classes::TStringList* values = new Classes::TStringList();
                ini->ReadSectionValues(sections->Strings[n], values);
                List->Add(Format("[%s]", ARRAYOFCONST((sections->Strings[n]))));
                for (int ii = 0; ii < values->Count; ii++)
                    List->Add(values->Strings[ii]);
                List->Add("");
                delete values;
            }
        }
        __finally
        {
            delete sections;
            delete ini;
        }
    }
}

bool __fastcall TSysScan::WriteReport(TSysScanData* Data, FILE* Stream,
                                      const unsigned char* Results,
                                      bool GameExists, Classes::TStrings* List)
{
    if (Stream == NULL && List == NULL)
        return false;
    if (Stream != NULL)
        WriteReportFile(Data, Stream, Results, GameExists);
    if (List != NULL)
        WriteReportList(Data, List, GameExists);
    return true;
}
